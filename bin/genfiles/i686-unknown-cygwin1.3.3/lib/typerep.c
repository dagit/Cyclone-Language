// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
// should be size_t, but int is fine.
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

//// Tagged arrays
struct _tagged_arr { 
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);

//// Exceptions 
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void * e);
#endif

extern struct _xtunion_struct *_exn_thrown;

//// Built-in Exceptions
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

//// Built-in Run-time Checks and company
static inline 
void * _check_null(void * ptr) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
//// Allocation
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned int x, unsigned int y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned int word_ans = (unsigned int)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Typerep_Var=0;struct
Cyc_Typerep_Var_struct{int tag;struct _tagged_arr*f1;};static const int Cyc_Typerep_Int=
1;struct Cyc_Typerep_Int_struct{int tag;unsigned int f1;};static const int Cyc_Typerep_Float=
0;static const int Cyc_Typerep_Double=1;static const int Cyc_Typerep_ThinPtr=2;struct
Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;};static const int Cyc_Typerep_FatPtr=
3;struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};static const int Cyc_Typerep_Tuple=
4;struct _tuple0{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _tagged_arr f2;};static const int Cyc_Typerep_TUnion=5;struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Typerep_XTUnion=
6;struct _tuple1{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Typerep_Union=7;struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Typerep_Forall=8;struct Cyc_Typerep_Forall_struct{
int tag;struct _tagged_arr f1;void**f2;};static const int Cyc_Typerep_App=9;struct Cyc_Typerep_App_struct{
int tag;void*f1;struct _tagged_arr f2;};extern void Cyc_Typerep_print_typestruct(void*
rep);extern void Cyc_Typerep_xmlize_typestruct(void*rep);extern void*Cyc_Typerep_normalize(
void*ts);extern void*Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_tagged_arr l);extern void*Cyc_Typerep_get_xtunionbranch(struct _tagged_arr tag,
struct _tagged_arr l);unsigned int Cyc_Typerep_size_type(void*rep);struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[8];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);void*Cyc_Typerep_tuple_tl(void*typ){
void*_tmp0=typ;struct _tagged_arr _tmp6;unsigned int _tmp8;_LL2: if((unsigned int)
_tmp0 > 2?*((int*)_tmp0)== Cyc_Typerep_Tuple: 0){_LL9: _tmp8=((struct Cyc_Typerep_Tuple_struct*)
_tmp0)->f1;goto _LL7;_LL7: _tmp6=((struct Cyc_Typerep_Tuple_struct*)_tmp0)->f2;goto
_LL3;}else{goto _LL4;}_LL4: goto _LL5;_LL3: {int sz1=(int)Cyc_Typerep_size_type((*(*((
struct _tuple0**)_check_unknown_subscript(_tmp6,sizeof(struct _tuple0*),0)))).f2);
struct _tagged_arr _tmpA=({unsigned int _tmpD=_get_arr_size(_tmp6,sizeof(struct
_tuple0*))- 1;struct _tuple0**_tmpE=(struct _tuple0**)_cycalloc(_check_times(
sizeof(struct _tuple0*),_tmpD));struct _tagged_arr _tmp11=_tag_arr(_tmpE,sizeof(
struct _tuple0*),_get_arr_size(_tmp6,sizeof(struct _tuple0*))- 1);{unsigned int
_tmpF=_tmpD;unsigned int i;for(i=0;i < _tmpF;i ++){_tmpE[i]=({struct _tuple0*_tmp10=
_cycalloc(sizeof(struct _tuple0));_tmp10->f1=(*(*((struct _tuple0**)
_check_unknown_subscript(_tmp6,sizeof(struct _tuple0*),(int)(i + 1))))).f1 - sz1;
_tmp10->f2=(*(*((struct _tuple0**)_check_unknown_subscript(_tmp6,sizeof(struct
_tuple0*),(int)(i + 1))))).f2;_tmp10;});}};_tmp11;});return(void*)({struct Cyc_Typerep_Tuple_struct*
_tmpB=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));_tmpB[0]=({struct Cyc_Typerep_Tuple_struct
_tmpC;_tmpC.tag=Cyc_Typerep_Tuple;_tmpC.f1=_tmp8 - sz1;_tmpC.f2=_tmpA;_tmpC;});
_tmpB;});}_LL5: return typ;_LL1:;}void*Cyc_Typerep_get_unionbranch(unsigned int tag,
struct _tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,sizeof(struct _tuple0*));i ++){
void*_tmp14;unsigned int _tmp16;struct _tuple0 _tmp12=*((struct _tuple0**)l.curr)[i];
_LL23: _tmp16=_tmp12.f1;goto _LL21;_LL21: _tmp14=_tmp12.f2;goto _LL19;_LL19: if(
_tmp16 == tag){return Cyc_Typerep_tuple_tl(_tmp14);}}}(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp18=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp18[0]=({struct Cyc_Core_Failure_struct _tmp19;_tmp19.tag=Cyc_Core_Failure;
_tmp19.f1=_tag_arr("Could not find tag in TUnion",sizeof(unsigned char),29);
_tmp19;});_tmp18;}));}void*Cyc_Typerep_get_xtunionbranch(struct _tagged_arr tag,
struct _tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,sizeof(struct _tuple1*));i ++){
void*_tmp1C;struct _tagged_arr _tmp1E;struct _tuple1 _tmp1A=*((struct _tuple1**)l.curr)[
i];_LL31: _tmp1E=_tmp1A.f1;goto _LL29;_LL29: _tmp1C=_tmp1A.f2;goto _LL27;_LL27: if(
Cyc_Std_strcmp(_tmp1E,tag)== 0){return Cyc_Typerep_tuple_tl(_tmp1C);}}}(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp20=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp20[0]=({struct Cyc_Core_Failure_struct _tmp21;_tmp21.tag=Cyc_Core_Failure;
_tmp21.f1=_tag_arr("Could not find tag in XTUnion",sizeof(unsigned char),30);
_tmp21;});_tmp20;}));}unsigned int Cyc_Typerep_size_type(void*rep){void*_tmp22=
rep;unsigned int _tmp3C;unsigned int _tmp3E;struct _tagged_arr _tmp40;void**_tmp42;
void*_tmp44;_LL36: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== Cyc_Typerep_Int: 0){
_LL61: _tmp3C=((struct Cyc_Typerep_Int_struct*)_tmp22)->f1;goto _LL37;}else{goto
_LL38;}_LL38: if(_tmp22 == (void*)Cyc_Typerep_Float){goto _LL39;}else{goto _LL40;}
_LL40: if(_tmp22 == (void*)Cyc_Typerep_Double){goto _LL41;}else{goto _LL42;}_LL42:
if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== Cyc_Typerep_ThinPtr: 0){goto _LL43;}
else{goto _LL44;}_LL44: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== Cyc_Typerep_FatPtr:
0){goto _LL45;}else{goto _LL46;}_LL46: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 
Cyc_Typerep_Tuple: 0){_LL63: _tmp3E=((struct Cyc_Typerep_Tuple_struct*)_tmp22)->f1;
goto _LL47;}else{goto _LL48;}_LL48: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 
Cyc_Typerep_TUnion: 0){goto _LL49;}else{goto _LL50;}_LL50: if((unsigned int)_tmp22 > 
2?*((int*)_tmp22)== Cyc_Typerep_XTUnion: 0){goto _LL51;}else{goto _LL52;}_LL52: if((
unsigned int)_tmp22 > 2?*((int*)_tmp22)== Cyc_Typerep_Union: 0){_LL65: _tmp40=((
struct Cyc_Typerep_Union_struct*)_tmp22)->f1;goto _LL53;}else{goto _LL54;}_LL54: if((
unsigned int)_tmp22 > 2?*((int*)_tmp22)== Cyc_Typerep_Var: 0){goto _LL55;}else{goto
_LL56;}_LL56: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== Cyc_Typerep_Forall: 0){
_LL67: _tmp42=((struct Cyc_Typerep_Forall_struct*)_tmp22)->f2;goto _LL57;}else{goto
_LL58;}_LL58: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== Cyc_Typerep_App: 0){
_LL69: _tmp44=(void*)((struct Cyc_Typerep_App_struct*)_tmp22)->f1;goto _LL59;}else{
goto _LL35;}_LL37: return _tmp3C >> 3;_LL39: return sizeof(float);_LL41: return sizeof(
double);_LL43: return sizeof(void*);_LL45: return sizeof(struct _tagged_arr);_LL47:
return _tmp3E;_LL49: return sizeof(void*);_LL51: return sizeof(void*);_LL53: {int max=
0;{int i=0;for(0;i < _get_arr_size(_tmp40,sizeof(void**));i ++){unsigned int _tmp46=
Cyc_Typerep_size_type(*((void***)_tmp40.curr)[i]);max=(int)(max < _tmp46? _tmp46:(
unsigned int)max);}}return(unsigned int)max;}_LL55: return sizeof(void*);_LL57:
return Cyc_Typerep_size_type(*_tmp42);_LL59: return Cyc_Typerep_size_type(_tmp44);
_LL35:;}void Cyc_Typerep_print_typestruct(void*rep){void*_tmp47=rep;struct
_tagged_arr*_tmp61;unsigned int _tmp63;void*_tmp65;unsigned int _tmp67;void*_tmp69;
struct _tagged_arr _tmp6B;unsigned int _tmp6D;struct _tagged_arr _tmp6F;struct
_tagged_arr _tmp71;struct _tagged_arr _tmp73;void**_tmp75;struct _tagged_arr _tmp77;
struct _tagged_arr _tmp79;void*_tmp7B;_LL73: if((unsigned int)_tmp47 > 2?*((int*)
_tmp47)== Cyc_Typerep_Var: 0){_LL98: _tmp61=((struct Cyc_Typerep_Var_struct*)_tmp47)->f1;
goto _LL74;}else{goto _LL75;}_LL75: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== 
Cyc_Typerep_Int: 0){_LL100: _tmp63=((struct Cyc_Typerep_Int_struct*)_tmp47)->f1;
goto _LL76;}else{goto _LL77;}_LL77: if(_tmp47 == (void*)Cyc_Typerep_Float){goto _LL78;}
else{goto _LL79;}_LL79: if(_tmp47 == (void*)Cyc_Typerep_Double){goto _LL80;}else{
goto _LL81;}_LL81: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_ThinPtr:
0){_LL104: _tmp67=((struct Cyc_Typerep_ThinPtr_struct*)_tmp47)->f1;goto _LL102;
_LL102: _tmp65=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp47)->f2;goto _LL82;}
else{goto _LL83;}_LL83: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_FatPtr:
0){_LL106: _tmp69=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp47)->f1;goto _LL84;}
else{goto _LL85;}_LL85: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_Tuple:
0){_LL110: _tmp6D=((struct Cyc_Typerep_Tuple_struct*)_tmp47)->f1;goto _LL108;_LL108:
_tmp6B=((struct Cyc_Typerep_Tuple_struct*)_tmp47)->f2;goto _LL86;}else{goto _LL87;}
_LL87: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_TUnion: 0){_LL112:
_tmp6F=((struct Cyc_Typerep_TUnion_struct*)_tmp47)->f1;goto _LL88;}else{goto _LL89;}
_LL89: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_XTUnion: 0){_LL114:
_tmp71=((struct Cyc_Typerep_XTUnion_struct*)_tmp47)->f1;goto _LL90;}else{goto _LL91;}
_LL91: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_Union: 0){_LL116:
_tmp73=((struct Cyc_Typerep_Union_struct*)_tmp47)->f1;goto _LL92;}else{goto _LL93;}
_LL93: if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_Forall: 0){_LL120:
_tmp77=((struct Cyc_Typerep_Forall_struct*)_tmp47)->f1;goto _LL118;_LL118: _tmp75=((
struct Cyc_Typerep_Forall_struct*)_tmp47)->f2;goto _LL94;}else{goto _LL95;}_LL95:
if((unsigned int)_tmp47 > 2?*((int*)_tmp47)== Cyc_Typerep_App: 0){_LL124: _tmp7B=(
void*)((struct Cyc_Typerep_App_struct*)_tmp47)->f1;goto _LL122;_LL122: _tmp79=((
struct Cyc_Typerep_App_struct*)_tmp47)->f2;goto _LL96;}else{goto _LL72;}_LL74:({
struct Cyc_Std_String_pa_struct _tmp7E;_tmp7E.tag=Cyc_Std_String_pa;_tmp7E.f1=(
struct _tagged_arr)*_tmp61;{void*_tmp7D[1]={& _tmp7E};Cyc_Std_printf(_tag_arr("Var(%s)",
sizeof(unsigned char),8),_tag_arr(_tmp7D,sizeof(void*),1));}});goto _LL72;_LL76:({
struct Cyc_Std_Int_pa_struct _tmp80;_tmp80.tag=Cyc_Std_Int_pa;_tmp80.f1=(int)
_tmp63;{void*_tmp7F[1]={& _tmp80};Cyc_Std_printf(_tag_arr("Int(%d)",sizeof(
unsigned char),8),_tag_arr(_tmp7F,sizeof(void*),1));}});goto _LL72;_LL78:({void*
_tmp81[0]={};Cyc_Std_printf(_tag_arr("Float",sizeof(unsigned char),6),_tag_arr(
_tmp81,sizeof(void*),0));});goto _LL72;_LL80:({void*_tmp82[0]={};Cyc_Std_printf(
_tag_arr("Double",sizeof(unsigned char),7),_tag_arr(_tmp82,sizeof(void*),0));});
goto _LL72;_LL82:({struct Cyc_Std_Int_pa_struct _tmp84;_tmp84.tag=Cyc_Std_Int_pa;
_tmp84.f1=(int)_tmp67;{void*_tmp83[1]={& _tmp84};Cyc_Std_printf(_tag_arr("ThinPtr(%d,",
sizeof(unsigned char),12),_tag_arr(_tmp83,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmp65);({void*_tmp85[0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),
_tag_arr(_tmp85,sizeof(void*),0));});goto _LL72;_LL84:({void*_tmp86[0]={};Cyc_Std_printf(
_tag_arr("FatPtr(",sizeof(unsigned char),8),_tag_arr(_tmp86,sizeof(void*),0));});
Cyc_Typerep_print_typestruct(_tmp69);({void*_tmp87[0]={};Cyc_Std_printf(_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(_tmp87,sizeof(void*),0));});goto _LL72;_LL86:({
struct Cyc_Std_Int_pa_struct _tmp89;_tmp89.tag=Cyc_Std_Int_pa;_tmp89.f1=(int)
_tmp6D;{void*_tmp88[1]={& _tmp89};Cyc_Std_printf(_tag_arr("Tuple(%d,[",sizeof(
unsigned char),11),_tag_arr(_tmp88,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp6B,sizeof(struct _tuple0*));i ++){void*_tmp8C;unsigned int _tmp8E;
struct _tuple0 _tmp8A=*((struct _tuple0**)_tmp6B.curr)[i];_LL143: _tmp8E=_tmp8A.f1;
goto _LL141;_LL141: _tmp8C=_tmp8A.f2;goto _LL139;_LL139:({struct Cyc_Std_Int_pa_struct
_tmp91;_tmp91.tag=Cyc_Std_Int_pa;_tmp91.f1=(int)_tmp8E;{void*_tmp90[1]={& _tmp91};
Cyc_Std_printf(_tag_arr("$(%d,",sizeof(unsigned char),6),_tag_arr(_tmp90,sizeof(
void*),1));}});Cyc_Typerep_print_typestruct(_tmp8C);({struct Cyc_Std_Int_pa_struct
_tmp93;_tmp93.tag=Cyc_Std_Int_pa;_tmp93.f1=(int)((unsigned int)((int)(i != 
_get_arr_size(_tmp6B,sizeof(struct _tuple0*))- 1?',':' ')));{void*_tmp92[1]={&
_tmp93};Cyc_Std_printf(_tag_arr(")%c",sizeof(unsigned char),4),_tag_arr(_tmp92,
sizeof(void*),1));}});}}({void*_tmp94[0]={};Cyc_Std_printf(_tag_arr(" ]",sizeof(
unsigned char),3),_tag_arr(_tmp94,sizeof(void*),0));});goto _LL72;_LL88:({void*
_tmp95[0]={};Cyc_Std_printf(_tag_arr("TUnion(",sizeof(unsigned char),8),_tag_arr(
_tmp95,sizeof(void*),0));});{int i=0;for(0;i < _get_arr_size(_tmp6F,sizeof(struct
_tuple0*));i ++){void*_tmp98;unsigned int _tmp9A;struct _tuple0 _tmp96=*((struct
_tuple0**)_tmp6F.curr)[i];_LL155: _tmp9A=_tmp96.f1;goto _LL153;_LL153: _tmp98=
_tmp96.f2;goto _LL151;_LL151:({struct Cyc_Std_Int_pa_struct _tmp9D;_tmp9D.tag=Cyc_Std_Int_pa;
_tmp9D.f1=(int)_tmp9A;{void*_tmp9C[1]={& _tmp9D};Cyc_Std_printf(_tag_arr("$(%d,",
sizeof(unsigned char),6),_tag_arr(_tmp9C,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmp98);({struct Cyc_Std_Int_pa_struct _tmp9F;_tmp9F.tag=Cyc_Std_Int_pa;_tmp9F.f1=(
int)((unsigned int)((int)(i != _get_arr_size(_tmp6F,sizeof(struct _tuple0*))- 1?',':' ')));{
void*_tmp9E[1]={& _tmp9F};Cyc_Std_printf(_tag_arr(")%c",sizeof(unsigned char),4),
_tag_arr(_tmp9E,sizeof(void*),1));}});}}({void*_tmpA0[0]={};Cyc_Std_printf(
_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(_tmpA0,sizeof(void*),0));});goto
_LL72;_LL90:({void*_tmpA1[0]={};Cyc_Std_printf(_tag_arr("XTUnion(",sizeof(
unsigned char),9),_tag_arr(_tmpA1,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp71,sizeof(struct _tuple1*));i ++){void*_tmpA4;struct _tagged_arr
_tmpA6;struct _tuple1 _tmpA2=*((struct _tuple1**)_tmp71.curr)[i];_LL167: _tmpA6=
_tmpA2.f1;goto _LL165;_LL165: _tmpA4=_tmpA2.f2;goto _LL163;_LL163:({struct Cyc_Std_String_pa_struct
_tmpA9;_tmpA9.tag=Cyc_Std_String_pa;_tmpA9.f1=(struct _tagged_arr)
_tagged_arr_plus(_tmpA6,sizeof(unsigned char),4);{void*_tmpA8[1]={& _tmpA9};Cyc_Std_printf(
_tag_arr("$(%s,",sizeof(unsigned char),6),_tag_arr(_tmpA8,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmpA4);({struct Cyc_Std_Int_pa_struct _tmpAB;_tmpAB.tag=
Cyc_Std_Int_pa;_tmpAB.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp71,
sizeof(struct _tuple1*))- 1?',':' ')));{void*_tmpAA[1]={& _tmpAB};Cyc_Std_printf(
_tag_arr(")%c",sizeof(unsigned char),4),_tag_arr(_tmpAA,sizeof(void*),1));}});}}({
void*_tmpAC[0]={};Cyc_Std_printf(_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(
_tmpAC,sizeof(void*),0));});goto _LL72;_LL92:({void*_tmpAD[0]={};Cyc_Std_printf(
_tag_arr("Union(",sizeof(unsigned char),7),_tag_arr(_tmpAD,sizeof(void*),0));});{
int i=0;for(0;i < _get_arr_size(_tmp73,sizeof(void**));i ++){Cyc_Typerep_print_typestruct(*((
void***)_tmp73.curr)[i]);({struct Cyc_Std_Int_pa_struct _tmpAF;_tmpAF.tag=Cyc_Std_Int_pa;
_tmpAF.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp73,sizeof(void**))- 1?',':' ')));{
void*_tmpAE[1]={& _tmpAF};Cyc_Std_printf(_tag_arr("%c",sizeof(unsigned char),3),
_tag_arr(_tmpAE,sizeof(void*),1));}});}}({void*_tmpB0[0]={};Cyc_Std_printf(
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(_tmpB0,sizeof(void*),0));});goto
_LL72;_LL94:({void*_tmpB1[0]={};Cyc_Std_printf(_tag_arr("Forall([",sizeof(
unsigned char),9),_tag_arr(_tmpB1,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp77,sizeof(struct _tagged_arr*));i ++){({void*_tmpB2[0]={};Cyc_Std_printf(*((
struct _tagged_arr**)_tmp77.curr)[i],_tag_arr(_tmpB2,sizeof(void*),0));});({
struct Cyc_Std_Int_pa_struct _tmpB4;_tmpB4.tag=Cyc_Std_Int_pa;_tmpB4.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp77,sizeof(struct _tagged_arr*))- 1?',':' ')));{
void*_tmpB3[1]={& _tmpB4};Cyc_Std_printf(_tag_arr("%c",sizeof(unsigned char),3),
_tag_arr(_tmpB3,sizeof(void*),1));}});}}({void*_tmpB5[0]={};Cyc_Std_printf(
_tag_arr("],",sizeof(unsigned char),3),_tag_arr(_tmpB5,sizeof(void*),0));});Cyc_Typerep_print_typestruct(*
_tmp75);({void*_tmpB6[0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),
_tag_arr(_tmpB6,sizeof(void*),0));});goto _LL72;_LL96:({void*_tmpB7[0]={};Cyc_Std_printf(
_tag_arr("App(",sizeof(unsigned char),5),_tag_arr(_tmpB7,sizeof(void*),0));});
Cyc_Typerep_print_typestruct(_tmp7B);({void*_tmpB8[0]={};Cyc_Std_printf(_tag_arr(",[",
sizeof(unsigned char),3),_tag_arr(_tmpB8,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp79,sizeof(void*));i ++){Cyc_Typerep_print_typestruct(((void**)
_tmp79.curr)[i]);({struct Cyc_Std_Int_pa_struct _tmpBA;_tmpBA.tag=Cyc_Std_Int_pa;
_tmpBA.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp79,sizeof(void*))- 1?',':' ')));{
void*_tmpB9[1]={& _tmpBA};Cyc_Std_printf(_tag_arr("%c",sizeof(unsigned char),3),
_tag_arr(_tmpB9,sizeof(void*),1));}});}}({void*_tmpBB[0]={};Cyc_Std_printf(
_tag_arr("])",sizeof(unsigned char),3),_tag_arr(_tmpBB,sizeof(void*),0));});goto
_LL72;_LL72:;}void Cyc_Typerep_xmlize_typestruct(void*rep){void*_tmpBC=rep;struct
_tagged_arr*_tmpD6;unsigned int _tmpD8;void*_tmpDA;unsigned int _tmpDC;void*_tmpDE;
struct _tagged_arr _tmpE0;unsigned int _tmpE2;struct _tagged_arr _tmpE4;struct
_tagged_arr _tmpE6;struct _tagged_arr _tmpE8;void**_tmpEA;struct _tagged_arr _tmpEC;
struct _tagged_arr _tmpEE;void*_tmpF0;_LL190: if((unsigned int)_tmpBC > 2?*((int*)
_tmpBC)== Cyc_Typerep_Var: 0){_LL215: _tmpD6=((struct Cyc_Typerep_Var_struct*)
_tmpBC)->f1;goto _LL191;}else{goto _LL192;}_LL192: if((unsigned int)_tmpBC > 2?*((
int*)_tmpBC)== Cyc_Typerep_Int: 0){_LL217: _tmpD8=((struct Cyc_Typerep_Int_struct*)
_tmpBC)->f1;goto _LL193;}else{goto _LL194;}_LL194: if(_tmpBC == (void*)Cyc_Typerep_Float){
goto _LL195;}else{goto _LL196;}_LL196: if(_tmpBC == (void*)Cyc_Typerep_Double){goto
_LL197;}else{goto _LL198;}_LL198: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_ThinPtr:
0){_LL221: _tmpDC=((struct Cyc_Typerep_ThinPtr_struct*)_tmpBC)->f1;goto _LL219;
_LL219: _tmpDA=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmpBC)->f2;goto _LL199;}
else{goto _LL200;}_LL200: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_FatPtr:
0){_LL223: _tmpDE=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpBC)->f1;goto
_LL201;}else{goto _LL202;}_LL202: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_Tuple:
0){_LL227: _tmpE2=((struct Cyc_Typerep_Tuple_struct*)_tmpBC)->f1;goto _LL225;_LL225:
_tmpE0=((struct Cyc_Typerep_Tuple_struct*)_tmpBC)->f2;goto _LL203;}else{goto _LL204;}
_LL204: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_TUnion: 0){_LL229:
_tmpE4=((struct Cyc_Typerep_TUnion_struct*)_tmpBC)->f1;goto _LL205;}else{goto
_LL206;}_LL206: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_XTUnion:
0){_LL231: _tmpE6=((struct Cyc_Typerep_XTUnion_struct*)_tmpBC)->f1;goto _LL207;}
else{goto _LL208;}_LL208: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_Union:
0){_LL233: _tmpE8=((struct Cyc_Typerep_Union_struct*)_tmpBC)->f1;goto _LL209;}else{
goto _LL210;}_LL210: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_Forall:
0){_LL237: _tmpEC=((struct Cyc_Typerep_Forall_struct*)_tmpBC)->f1;goto _LL235;
_LL235: _tmpEA=((struct Cyc_Typerep_Forall_struct*)_tmpBC)->f2;goto _LL211;}else{
goto _LL212;}_LL212: if((unsigned int)_tmpBC > 2?*((int*)_tmpBC)== Cyc_Typerep_App:
0){_LL241: _tmpF0=(void*)((struct Cyc_Typerep_App_struct*)_tmpBC)->f1;goto _LL239;
_LL239: _tmpEE=((struct Cyc_Typerep_App_struct*)_tmpBC)->f2;goto _LL213;}else{goto
_LL189;}_LL191:({struct Cyc_Std_String_pa_struct _tmpF3;_tmpF3.tag=Cyc_Std_String_pa;
_tmpF3.f1=(struct _tagged_arr)*_tmpD6;{void*_tmpF2[1]={& _tmpF3};Cyc_Std_printf(
_tag_arr("<Var name=\"%s\"/>",sizeof(unsigned char),17),_tag_arr(_tmpF2,sizeof(
void*),1));}});goto _LL189;_LL193:({struct Cyc_Std_Int_pa_struct _tmpF5;_tmpF5.tag=
Cyc_Std_Int_pa;_tmpF5.f1=(int)_tmpD8;{void*_tmpF4[1]={& _tmpF5};Cyc_Std_printf(
_tag_arr("<Int sz=\"%d\"/>",sizeof(unsigned char),15),_tag_arr(_tmpF4,sizeof(
void*),1));}});goto _LL189;_LL195:({void*_tmpF6[0]={};Cyc_Std_printf(_tag_arr("<Float/>",
sizeof(unsigned char),9),_tag_arr(_tmpF6,sizeof(void*),0));});goto _LL189;_LL197:({
void*_tmpF7[0]={};Cyc_Std_printf(_tag_arr("<Double/>",sizeof(unsigned char),10),
_tag_arr(_tmpF7,sizeof(void*),0));});goto _LL189;_LL199:({struct Cyc_Std_Int_pa_struct
_tmpF9;_tmpF9.tag=Cyc_Std_Int_pa;_tmpF9.f1=(int)_tmpDC;{void*_tmpF8[1]={& _tmpF9};
Cyc_Std_printf(_tag_arr("<ThinPtr sz=\"%d\">",sizeof(unsigned char),18),_tag_arr(
_tmpF8,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmpDA);({void*_tmpFA[
0]={};Cyc_Std_printf(_tag_arr("</ThinPtr",sizeof(unsigned char),10),_tag_arr(
_tmpFA,sizeof(void*),0));});goto _LL189;_LL201:({void*_tmpFB[0]={};Cyc_Std_printf(
_tag_arr("<FatPtr>",sizeof(unsigned char),9),_tag_arr(_tmpFB,sizeof(void*),0));});
Cyc_Typerep_xmlize_typestruct(_tmpDE);({void*_tmpFC[0]={};Cyc_Std_printf(
_tag_arr("</FatPtr>",sizeof(unsigned char),10),_tag_arr(_tmpFC,sizeof(void*),0));});
goto _LL189;_LL203:({struct Cyc_Std_Int_pa_struct _tmpFE;_tmpFE.tag=Cyc_Std_Int_pa;
_tmpFE.f1=(int)_tmpE2;{void*_tmpFD[1]={& _tmpFE};Cyc_Std_printf(_tag_arr("<Tuple sz=\"%d\">",
sizeof(unsigned char),16),_tag_arr(_tmpFD,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmpE0,sizeof(struct _tuple0*));i ++){void*_tmp101;unsigned int
_tmp103;struct _tuple0 _tmpFF=*((struct _tuple0**)_tmpE0.curr)[i];_LL260: _tmp103=
_tmpFF.f1;goto _LL258;_LL258: _tmp101=_tmpFF.f2;goto _LL256;_LL256:({struct Cyc_Std_Int_pa_struct
_tmp106;_tmp106.tag=Cyc_Std_Int_pa;_tmp106.f1=(int)_tmp103;{void*_tmp105[1]={&
_tmp106};Cyc_Std_printf(_tag_arr("<Component ofs=\"%d\">",sizeof(unsigned char),
21),_tag_arr(_tmp105,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmp101);({
void*_tmp107[0]={};Cyc_Std_printf(_tag_arr("</Component>",sizeof(unsigned char),
13),_tag_arr(_tmp107,sizeof(void*),0));});}}({void*_tmp108[0]={};Cyc_Std_printf(
_tag_arr("</Tuple>",sizeof(unsigned char),9),_tag_arr(_tmp108,sizeof(void*),0));});
goto _LL189;_LL205:({void*_tmp109[0]={};Cyc_Std_printf(_tag_arr("<TUnion>",
sizeof(unsigned char),9),_tag_arr(_tmp109,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmpE4,sizeof(struct _tuple0*));i ++){void*_tmp10C;unsigned int
_tmp10E;struct _tuple0 _tmp10A=*((struct _tuple0**)_tmpE4.curr)[i];_LL271: _tmp10E=
_tmp10A.f1;goto _LL269;_LL269: _tmp10C=_tmp10A.f2;goto _LL267;_LL267:({struct Cyc_Std_Int_pa_struct
_tmp111;_tmp111.tag=Cyc_Std_Int_pa;_tmp111.f1=(int)_tmp10E;{void*_tmp110[1]={&
_tmp111};Cyc_Std_printf(_tag_arr("<Tag tag=\"%d\">",sizeof(unsigned char),15),
_tag_arr(_tmp110,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmp10C);({
void*_tmp112[0]={};Cyc_Std_printf(_tag_arr("</Tag>",sizeof(unsigned char),7),
_tag_arr(_tmp112,sizeof(void*),0));});}}({void*_tmp113[0]={};Cyc_Std_printf(
_tag_arr("</Tunion>",sizeof(unsigned char),10),_tag_arr(_tmp113,sizeof(void*),0));});
goto _LL189;_LL207:({void*_tmp114[0]={};Cyc_Std_printf(_tag_arr("<XTUnion>",
sizeof(unsigned char),10),_tag_arr(_tmp114,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmpE6,sizeof(struct _tuple1*));i ++){void*_tmp117;struct _tagged_arr
_tmp119;struct _tuple1 _tmp115=*((struct _tuple1**)_tmpE6.curr)[i];_LL282: _tmp119=
_tmp115.f1;goto _LL280;_LL280: _tmp117=_tmp115.f2;goto _LL278;_LL278:({struct Cyc_Std_String_pa_struct
_tmp11C;_tmp11C.tag=Cyc_Std_String_pa;_tmp11C.f1=(struct _tagged_arr)
_tagged_arr_plus(_tmp119,sizeof(unsigned char),4);{void*_tmp11B[1]={& _tmp11C};
Cyc_Std_printf(_tag_arr("<Tag tag=\"%s\">",sizeof(unsigned char),15),_tag_arr(
_tmp11B,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmp117);({void*
_tmp11D[0]={};Cyc_Std_printf(_tag_arr("</Tag>",sizeof(unsigned char),7),_tag_arr(
_tmp11D,sizeof(void*),0));});}}({void*_tmp11E[0]={};Cyc_Std_printf(_tag_arr("</XTunion>",
sizeof(unsigned char),11),_tag_arr(_tmp11E,sizeof(void*),0));});goto _LL189;
_LL209:({void*_tmp11F[0]={};Cyc_Std_printf(_tag_arr("<Union>",sizeof(
unsigned char),8),_tag_arr(_tmp11F,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmpE8,sizeof(void**));i ++){({void*_tmp120[0]={};Cyc_Std_printf(
_tag_arr("<Case>",sizeof(unsigned char),7),_tag_arr(_tmp120,sizeof(void*),0));});
Cyc_Typerep_xmlize_typestruct(*((void***)_tmpE8.curr)[i]);({void*_tmp121[0]={};
Cyc_Std_printf(_tag_arr("</Case>",sizeof(unsigned char),8),_tag_arr(_tmp121,
sizeof(void*),0));});}}({void*_tmp122[0]={};Cyc_Std_printf(_tag_arr("</Union>",
sizeof(unsigned char),9),_tag_arr(_tmp122,sizeof(void*),0));});goto _LL189;_LL211:({
void*_tmp123[0]={};Cyc_Std_printf(_tag_arr("<Forall vars=\"",sizeof(
unsigned char),15),_tag_arr(_tmp123,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmpEC,sizeof(struct _tagged_arr*));i ++){if(i != 0){({void*_tmp124[0]={};
Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(_tmp124,sizeof(
void*),0));});}({void*_tmp125[0]={};Cyc_Std_printf(*((struct _tagged_arr**)_tmpEC.curr)[
i],_tag_arr(_tmp125,sizeof(void*),0));});}}({void*_tmp126[0]={};Cyc_Std_printf(
_tag_arr("\">",sizeof(unsigned char),3),_tag_arr(_tmp126,sizeof(void*),0));});
Cyc_Typerep_xmlize_typestruct(*_tmpEA);({void*_tmp127[0]={};Cyc_Std_printf(
_tag_arr("</Forall>",sizeof(unsigned char),10),_tag_arr(_tmp127,sizeof(void*),0));});
goto _LL189;_LL213:({void*_tmp128[0]={};Cyc_Std_printf(_tag_arr("<App>",sizeof(
unsigned char),6),_tag_arr(_tmp128,sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(
_tmpF0);{int i=0;for(0;i < _get_arr_size(_tmpEE,sizeof(void*));i ++){if(i != 0){({
void*_tmp129[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),
_tag_arr(_tmp129,sizeof(void*),0));});}Cyc_Typerep_xmlize_typestruct(((void**)
_tmpEE.curr)[i]);}}({void*_tmp12A[0]={};Cyc_Std_printf(_tag_arr("</App>",sizeof(
unsigned char),7),_tag_arr(_tmp12A,sizeof(void*),0));});goto _LL189;_LL189:;}
static void*Cyc_Typerep_normalize_env(struct Cyc_Dict_Dict*env,void*ts){void*
_tmp12B=ts;struct _tagged_arr*_tmp145;void*_tmp147;unsigned int _tmp149;void*
_tmp14B;struct _tagged_arr _tmp14D;unsigned int _tmp14F;struct _tagged_arr _tmp151;
struct _tagged_arr _tmp153;struct _tagged_arr _tmp155;void**_tmp157;struct
_tagged_arr _tmp159;struct _tagged_arr _tmp15B;void*_tmp15D;_LL301: if((unsigned int)
_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_Var: 0){_LL326: _tmp145=((struct Cyc_Typerep_Var_struct*)
_tmp12B)->f1;goto _LL302;}else{goto _LL303;}_LL303: if((unsigned int)_tmp12B > 2?*((
int*)_tmp12B)== Cyc_Typerep_Int: 0){goto _LL304;}else{goto _LL305;}_LL305: if(
_tmp12B == (void*)Cyc_Typerep_Float){goto _LL306;}else{goto _LL307;}_LL307: if(
_tmp12B == (void*)Cyc_Typerep_Double){goto _LL308;}else{goto _LL309;}_LL309: if((
unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_ThinPtr: 0){_LL330: _tmp149=((
struct Cyc_Typerep_ThinPtr_struct*)_tmp12B)->f1;goto _LL328;_LL328: _tmp147=(void*)((
struct Cyc_Typerep_ThinPtr_struct*)_tmp12B)->f2;goto _LL310;}else{goto _LL311;}
_LL311: if((unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_FatPtr: 0){
_LL332: _tmp14B=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp12B)->f1;goto _LL312;}
else{goto _LL313;}_LL313: if((unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_Tuple:
0){_LL336: _tmp14F=((struct Cyc_Typerep_Tuple_struct*)_tmp12B)->f1;goto _LL334;
_LL334: _tmp14D=((struct Cyc_Typerep_Tuple_struct*)_tmp12B)->f2;goto _LL314;}else{
goto _LL315;}_LL315: if((unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_TUnion:
0){_LL338: _tmp151=((struct Cyc_Typerep_TUnion_struct*)_tmp12B)->f1;goto _LL316;}
else{goto _LL317;}_LL317: if((unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_XTUnion:
0){_LL340: _tmp153=((struct Cyc_Typerep_XTUnion_struct*)_tmp12B)->f1;goto _LL318;}
else{goto _LL319;}_LL319: if((unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_Union:
0){_LL342: _tmp155=((struct Cyc_Typerep_Union_struct*)_tmp12B)->f1;goto _LL320;}
else{goto _LL321;}_LL321: if((unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_Forall:
0){_LL346: _tmp159=((struct Cyc_Typerep_Forall_struct*)_tmp12B)->f1;goto _LL344;
_LL344: _tmp157=((struct Cyc_Typerep_Forall_struct*)_tmp12B)->f2;goto _LL322;}else{
goto _LL323;}_LL323: if((unsigned int)_tmp12B > 2?*((int*)_tmp12B)== Cyc_Typerep_App:
0){_LL350: _tmp15D=(void*)((struct Cyc_Typerep_App_struct*)_tmp12B)->f1;goto _LL348;
_LL348: _tmp15B=((struct Cyc_Typerep_App_struct*)_tmp12B)->f2;goto _LL324;}else{
goto _LL300;}_LL302: return((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))
Cyc_Dict_lookup)(env,(struct _tagged_arr*)_tmp145);_LL304: goto _LL306;_LL306: goto
_LL308;_LL308: return ts;_LL310: return(void*)({struct Cyc_Typerep_ThinPtr_struct*
_tmp15F=_cycalloc(sizeof(struct Cyc_Typerep_ThinPtr_struct));_tmp15F[0]=({struct
Cyc_Typerep_ThinPtr_struct _tmp160;_tmp160.tag=Cyc_Typerep_ThinPtr;_tmp160.f1=
_tmp149;_tmp160.f2=(void*)Cyc_Typerep_normalize_env(env,_tmp147);_tmp160;});
_tmp15F;});_LL312: return(void*)({struct Cyc_Typerep_FatPtr_struct*_tmp161=
_cycalloc(sizeof(struct Cyc_Typerep_FatPtr_struct));_tmp161[0]=({struct Cyc_Typerep_FatPtr_struct
_tmp162;_tmp162.tag=Cyc_Typerep_FatPtr;_tmp162.f1=(void*)Cyc_Typerep_normalize_env(
env,_tmp14B);_tmp162;});_tmp161;});_LL314: {struct _tagged_arr _tmp163=({
unsigned int _tmp173=_get_arr_size(_tmp14D,sizeof(struct _tuple0*));struct _tuple0**
_tmp174=(struct _tuple0**)_cycalloc(_check_times(sizeof(struct _tuple0*),_tmp173));
struct _tagged_arr _tmp176=_tag_arr(_tmp174,sizeof(struct _tuple0*),_get_arr_size(
_tmp14D,sizeof(struct _tuple0*)));{unsigned int _tmp175=_tmp173;unsigned int i;for(
i=0;i < _tmp175;i ++){_tmp174[i]=0;}};_tmp176;});{int i=0;for(0;i < _get_arr_size(
_tmp14D,sizeof(struct _tuple0*));i ++){struct _tuple0 _tmp166;void*_tmp167;
unsigned int _tmp169;struct _tuple0*_tmp164=((struct _tuple0**)_tmp14D.curr)[i];
_tmp166=*_tmp164;_LL362: _tmp169=_tmp166.f1;goto _LL360;_LL360: _tmp167=_tmp166.f2;
goto _LL357;_LL357:((struct _tuple0**)_tmp163.curr)[i]=({struct _tuple0*_tmp16B=
_cycalloc(sizeof(struct _tuple0));_tmp16B->f1=_tmp169;_tmp16B->f2=Cyc_Typerep_normalize_env(
env,_tmp167);_tmp16B;});}}{struct _tagged_arr _tmp16C=({unsigned int _tmp16F=
_get_arr_size(_tmp14D,sizeof(struct _tuple0*));struct _tuple0**_tmp170=(struct
_tuple0**)_cycalloc(_check_times(sizeof(struct _tuple0*),_tmp16F));struct
_tagged_arr _tmp172=_tag_arr(_tmp170,sizeof(struct _tuple0*),_get_arr_size(_tmp14D,
sizeof(struct _tuple0*)));{unsigned int _tmp171=_tmp16F;unsigned int i;for(i=0;i < 
_tmp171;i ++){_tmp170[i]=(struct _tuple0*)_check_null(*((struct _tuple0**)
_check_unknown_subscript(_tmp163,sizeof(struct _tuple0*),(int)i)));}};_tmp172;});
return(void*)({struct Cyc_Typerep_Tuple_struct*_tmp16D=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));
_tmp16D[0]=({struct Cyc_Typerep_Tuple_struct _tmp16E;_tmp16E.tag=Cyc_Typerep_Tuple;
_tmp16E.f1=_tmp14F;_tmp16E.f2=(struct _tagged_arr)_tmp16C;_tmp16E;});_tmp16D;});}}
_LL316: {struct _tagged_arr _tmp177=({unsigned int _tmp187=_get_arr_size(_tmp151,
sizeof(struct _tuple0*));struct _tuple0**_tmp188=(struct _tuple0**)_cycalloc(
_check_times(sizeof(struct _tuple0*),_tmp187));struct _tagged_arr _tmp18A=_tag_arr(
_tmp188,sizeof(struct _tuple0*),_get_arr_size(_tmp151,sizeof(struct _tuple0*)));{
unsigned int _tmp189=_tmp187;unsigned int i;for(i=0;i < _tmp189;i ++){_tmp188[i]=0;}};
_tmp18A;});{int i=0;for(0;i < _get_arr_size(_tmp151,sizeof(struct _tuple0*));i ++){
struct _tuple0 _tmp17A;void*_tmp17B;unsigned int _tmp17D;struct _tuple0*_tmp178=((
struct _tuple0**)_tmp151.curr)[i];_tmp17A=*_tmp178;_LL382: _tmp17D=_tmp17A.f1;goto
_LL380;_LL380: _tmp17B=_tmp17A.f2;goto _LL377;_LL377:((struct _tuple0**)_tmp177.curr)[
i]=({struct _tuple0*_tmp17F=_cycalloc(sizeof(struct _tuple0));_tmp17F->f1=_tmp17D;
_tmp17F->f2=Cyc_Typerep_normalize_env(env,_tmp17B);_tmp17F;});}}{struct
_tagged_arr _tmp180=({unsigned int _tmp183=_get_arr_size(_tmp151,sizeof(struct
_tuple0*));struct _tuple0**_tmp184=(struct _tuple0**)_cycalloc(_check_times(
sizeof(struct _tuple0*),_tmp183));struct _tagged_arr _tmp186=_tag_arr(_tmp184,
sizeof(struct _tuple0*),_get_arr_size(_tmp151,sizeof(struct _tuple0*)));{
unsigned int _tmp185=_tmp183;unsigned int i;for(i=0;i < _tmp185;i ++){_tmp184[i]=(
struct _tuple0*)_check_null(*((struct _tuple0**)_check_unknown_subscript(_tmp177,
sizeof(struct _tuple0*),(int)i)));}};_tmp186;});return(void*)({struct Cyc_Typerep_TUnion_struct*
_tmp181=_cycalloc(sizeof(struct Cyc_Typerep_TUnion_struct));_tmp181[0]=({struct
Cyc_Typerep_TUnion_struct _tmp182;_tmp182.tag=Cyc_Typerep_TUnion;_tmp182.f1=(
struct _tagged_arr)_tmp180;_tmp182;});_tmp181;});}}_LL318: {struct _tagged_arr
_tmp18B=({unsigned int _tmp19B=_get_arr_size(_tmp153,sizeof(struct _tuple1*));
struct _tuple1**_tmp19C=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct
_tuple1*),_tmp19B));struct _tagged_arr _tmp19E=_tag_arr(_tmp19C,sizeof(struct
_tuple1*),_get_arr_size(_tmp153,sizeof(struct _tuple1*)));{unsigned int _tmp19D=
_tmp19B;unsigned int i;for(i=0;i < _tmp19D;i ++){_tmp19C[i]=0;}};_tmp19E;});{int i=0;
for(0;i < _get_arr_size(_tmp153,sizeof(struct _tuple1*));i ++){struct _tuple1 _tmp18E;
void*_tmp18F;struct _tagged_arr _tmp191;struct _tuple1*_tmp18C=((struct _tuple1**)
_tmp153.curr)[i];_tmp18E=*_tmp18C;_LL402: _tmp191=_tmp18E.f1;goto _LL400;_LL400:
_tmp18F=_tmp18E.f2;goto _LL397;_LL397:((struct _tuple1**)_tmp18B.curr)[i]=({struct
_tuple1*_tmp193=_cycalloc(sizeof(struct _tuple1));_tmp193->f1=_tmp191;_tmp193->f2=
Cyc_Typerep_normalize_env(env,_tmp18F);_tmp193;});}}{struct _tagged_arr _tmp194=({
unsigned int _tmp197=_get_arr_size(_tmp153,sizeof(struct _tuple1*));struct _tuple1**
_tmp198=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct _tuple1*),_tmp197));
struct _tagged_arr _tmp19A=_tag_arr(_tmp198,sizeof(struct _tuple1*),_get_arr_size(
_tmp153,sizeof(struct _tuple1*)));{unsigned int _tmp199=_tmp197;unsigned int i;for(
i=0;i < _tmp199;i ++){_tmp198[i]=(struct _tuple1*)_check_null(*((struct _tuple1**)
_check_unknown_subscript(_tmp18B,sizeof(struct _tuple1*),(int)i)));}};_tmp19A;});
return(void*)({struct Cyc_Typerep_XTUnion_struct*_tmp195=_cycalloc(sizeof(struct
Cyc_Typerep_XTUnion_struct));_tmp195[0]=({struct Cyc_Typerep_XTUnion_struct
_tmp196;_tmp196.tag=Cyc_Typerep_XTUnion;_tmp196.f1=(struct _tagged_arr)_tmp194;
_tmp196;});_tmp195;});}}_LL320: {struct _tagged_arr _tmp19F=({unsigned int _tmp1A8=
_get_arr_size(_tmp155,sizeof(void**));void***_tmp1A9=(void***)_cycalloc(
_check_times(sizeof(void**),_tmp1A8));struct _tagged_arr _tmp1AB=_tag_arr(_tmp1A9,
sizeof(void**),_get_arr_size(_tmp155,sizeof(void**)));{unsigned int _tmp1AA=
_tmp1A8;unsigned int i;for(i=0;i < _tmp1AA;i ++){_tmp1A9[i]=0;}};_tmp1AB;});{int i=0;
for(0;i < _get_arr_size(_tmp155,sizeof(void**));i ++){((void***)_tmp19F.curr)[i]=({
void**_tmp1A0=_cycalloc(sizeof(void*));_tmp1A0[0]=Cyc_Typerep_normalize_env(env,*((
void***)_tmp155.curr)[i]);_tmp1A0;});}}{struct _tagged_arr _tmp1A1=({unsigned int
_tmp1A4=_get_arr_size(_tmp155,sizeof(void**));void***_tmp1A5=(void***)_cycalloc(
_check_times(sizeof(void**),_tmp1A4));struct _tagged_arr _tmp1A7=_tag_arr(_tmp1A5,
sizeof(void**),_get_arr_size(_tmp155,sizeof(void**)));{unsigned int _tmp1A6=
_tmp1A4;unsigned int i;for(i=0;i < _tmp1A6;i ++){_tmp1A5[i]=(void**)_check_null(*((
void***)_check_unknown_subscript(_tmp19F,sizeof(void**),(int)i)));}};_tmp1A7;});
return(void*)({struct Cyc_Typerep_Union_struct*_tmp1A2=_cycalloc(sizeof(struct Cyc_Typerep_Union_struct));
_tmp1A2[0]=({struct Cyc_Typerep_Union_struct _tmp1A3;_tmp1A3.tag=Cyc_Typerep_Union;
_tmp1A3.f1=(struct _tagged_arr)_tmp1A1;_tmp1A3;});_tmp1A2;});}}_LL322: return(void*)({
struct Cyc_Typerep_Forall_struct*_tmp1AC=_cycalloc(sizeof(struct Cyc_Typerep_Forall_struct));
_tmp1AC[0]=({struct Cyc_Typerep_Forall_struct _tmp1AD;_tmp1AD.tag=Cyc_Typerep_Forall;
_tmp1AD.f1=_tmp159;_tmp1AD.f2=({void**_tmp1AE=_cycalloc(sizeof(void*));_tmp1AE[0]=
Cyc_Typerep_normalize_env(env,*_tmp157);_tmp1AE;});_tmp1AD;});_tmp1AC;});_LL324: {
void*_tmp1AF=Cyc_Typerep_normalize_env(env,_tmp15D);void**_tmp1B5;struct
_tagged_arr _tmp1B7;_LL433: if((unsigned int)_tmp1AF > 2?*((int*)_tmp1AF)== Cyc_Typerep_Forall:
0){_LL440: _tmp1B7=((struct Cyc_Typerep_Forall_struct*)_tmp1AF)->f1;goto _LL438;
_LL438: _tmp1B5=((struct Cyc_Typerep_Forall_struct*)_tmp1AF)->f2;goto _LL434;}else{
goto _LL435;}_LL435: goto _LL436;_LL434: if(_get_arr_size(_tmp15B,sizeof(void*))!= 
_get_arr_size(_tmp1B7,sizeof(struct _tagged_arr*))){(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp1B9=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp1B9[0]=({struct Cyc_Core_Failure_struct _tmp1BA;_tmp1BA.tag=Cyc_Core_Failure;
_tmp1BA.f1=_tag_arr("app appiled wrong number of arguments",sizeof(unsigned char),
38);_tmp1BA;});_tmp1B9;}));}{int i=0;for(0;i < _get_arr_size(_tmp15B,sizeof(void*));
i ++){env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(env,((struct _tagged_arr**)_tmp1B7.curr)[i],((void**)
_tmp15B.curr)[i]);}}return Cyc_Typerep_normalize_env(env,*_tmp1B5);_LL436:(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp1BB=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp1BB[0]=({struct Cyc_Core_Failure_struct _tmp1BC;_tmp1BC.tag=Cyc_Core_Failure;
_tmp1BC.f1=_tag_arr("app misapplied",sizeof(unsigned char),15);_tmp1BC;});
_tmp1BB;}));_LL432:;}_LL300:;}int Cyc_Typerep_name_order(struct _tagged_arr*s1,
struct _tagged_arr*s2){return Cyc_Std_strcmp(*s1,*s2);}void*Cyc_Typerep_normalize(
void*ts){return Cyc_Typerep_normalize_env(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Typerep_name_order),
ts);}
