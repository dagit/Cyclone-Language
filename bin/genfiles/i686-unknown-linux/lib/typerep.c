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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Typerep_Var_struct{int tag;struct _tagged_arr*f1;};struct Cyc_Typerep_Int_struct{
int tag;unsigned int f1;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;
void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple0{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;
};struct _tuple1{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Typerep_Forall_struct{int tag;struct _tagged_arr f1;void**
f2;};struct Cyc_Typerep_App_struct{int tag;void*f1;struct _tagged_arr f2;};extern
void Cyc_Typerep_print_typestruct(void*rep);extern void Cyc_Typerep_xmlize_typestruct(
void*rep);extern void*Cyc_Typerep_normalize(void*ts);extern void*Cyc_Typerep_get_unionbranch(
unsigned int tag,struct _tagged_arr l);extern void*Cyc_Typerep_get_xtunionbranch(
struct _tagged_arr tag,struct _tagged_arr l);unsigned int Cyc_Typerep_size_type(void*
rep);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[8];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);void*Cyc_Typerep_tuple_tl(void*typ){
void*_tmp0=typ;struct _tagged_arr _tmp1;unsigned int _tmp2;_LL1: if((unsigned int)
_tmp0 > 2?*((int*)_tmp0)== 4: 0){_LL6: _tmp2=((struct Cyc_Typerep_Tuple_struct*)
_tmp0)->f1;goto _LL5;_LL5: _tmp1=((struct Cyc_Typerep_Tuple_struct*)_tmp0)->f2;goto
_LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2: {int sz1=(int)Cyc_Typerep_size_type((*(*((
struct _tuple0**)_check_unknown_subscript(_tmp1,sizeof(struct _tuple0*),0)))).f2);
struct _tagged_arr _tmp3=({unsigned int _tmp6=_get_arr_size(_tmp1,sizeof(struct
_tuple0*))- 1;struct _tuple0**_tmp7=(struct _tuple0**)_cycalloc(_check_times(
sizeof(struct _tuple0*),_tmp6));struct _tagged_arr _tmpA=_tag_arr(_tmp7,sizeof(
struct _tuple0*),_get_arr_size(_tmp1,sizeof(struct _tuple0*))- 1);{unsigned int
_tmp8=_tmp6;unsigned int i;for(i=0;i < _tmp8;i ++){_tmp7[i]=({struct _tuple0*_tmp9=
_cycalloc(sizeof(struct _tuple0));_tmp9->f1=(*(*((struct _tuple0**)
_check_unknown_subscript(_tmp1,sizeof(struct _tuple0*),(int)(i + 1))))).f1 - sz1;
_tmp9->f2=(*(*((struct _tuple0**)_check_unknown_subscript(_tmp1,sizeof(struct
_tuple0*),(int)(i + 1))))).f2;_tmp9;});}};_tmpA;});return(void*)({struct Cyc_Typerep_Tuple_struct*
_tmp4=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));_tmp4[0]=({struct Cyc_Typerep_Tuple_struct
_tmp5;_tmp5.tag=4;_tmp5.f1=_tmp2 - sz1;_tmp5.f2=_tmp3;_tmp5;});_tmp4;});}_LL4:
return typ;_LL0:;}void*Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,sizeof(struct _tuple0*));i ++){void*
_tmpC;unsigned int _tmpD;struct _tuple0 _tmpB=*((struct _tuple0**)l.curr)[i];_LL9:
_tmpD=_tmpB.f1;goto _LL8;_LL8: _tmpC=_tmpB.f2;goto _LL7;_LL7: if(_tmpD == tag){return
Cyc_Typerep_tuple_tl(_tmpC);}}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpE=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpE[0]=({struct Cyc_Core_Failure_struct
_tmpF;_tmpF.tag=Cyc_Core_Failure;_tmpF.f1=_tag_arr("Could not find tag in TUnion",
sizeof(unsigned char),29);_tmpF;});_tmpE;}));}void*Cyc_Typerep_get_xtunionbranch(
struct _tagged_arr tag,struct _tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,
sizeof(struct _tuple1*));i ++){void*_tmp11;struct _tagged_arr _tmp12;struct _tuple1
_tmp10=*((struct _tuple1**)l.curr)[i];_LLC: _tmp12=_tmp10.f1;goto _LLB;_LLB: _tmp11=
_tmp10.f2;goto _LLA;_LLA: if(Cyc_Std_strcmp(_tmp12,tag)== 0){return Cyc_Typerep_tuple_tl(
_tmp11);}}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp13=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmp13[0]=({struct Cyc_Core_Failure_struct
_tmp14;_tmp14.tag=Cyc_Core_Failure;_tmp14.f1=_tag_arr("Could not find tag in XTUnion",
sizeof(unsigned char),30);_tmp14;});_tmp13;}));}unsigned int Cyc_Typerep_size_type(
void*rep){void*_tmp15=rep;unsigned int _tmp16;unsigned int _tmp17;struct
_tagged_arr _tmp18;void**_tmp19;void*_tmp1A;_LLE: if((unsigned int)_tmp15 > 2?*((
int*)_tmp15)== 1: 0){_LL26: _tmp16=((struct Cyc_Typerep_Int_struct*)_tmp15)->f1;
goto _LLF;}else{goto _LL10;}_LL10: if((int)_tmp15 == 0){goto _LL11;}else{goto _LL12;}
_LL12: if((int)_tmp15 == 1){goto _LL13;}else{goto _LL14;}_LL14: if((unsigned int)
_tmp15 > 2?*((int*)_tmp15)== 2: 0){goto _LL15;}else{goto _LL16;}_LL16: if((
unsigned int)_tmp15 > 2?*((int*)_tmp15)== 3: 0){goto _LL17;}else{goto _LL18;}_LL18:
if((unsigned int)_tmp15 > 2?*((int*)_tmp15)== 4: 0){_LL27: _tmp17=((struct Cyc_Typerep_Tuple_struct*)
_tmp15)->f1;goto _LL19;}else{goto _LL1A;}_LL1A: if((unsigned int)_tmp15 > 2?*((int*)
_tmp15)== 5: 0){goto _LL1B;}else{goto _LL1C;}_LL1C: if((unsigned int)_tmp15 > 2?*((
int*)_tmp15)== 6: 0){goto _LL1D;}else{goto _LL1E;}_LL1E: if((unsigned int)_tmp15 > 2?*((
int*)_tmp15)== 7: 0){_LL28: _tmp18=((struct Cyc_Typerep_Union_struct*)_tmp15)->f1;
goto _LL1F;}else{goto _LL20;}_LL20: if((unsigned int)_tmp15 > 2?*((int*)_tmp15)== 0:
0){goto _LL21;}else{goto _LL22;}_LL22: if((unsigned int)_tmp15 > 2?*((int*)_tmp15)== 
8: 0){_LL29: _tmp19=((struct Cyc_Typerep_Forall_struct*)_tmp15)->f2;goto _LL23;}
else{goto _LL24;}_LL24: if((unsigned int)_tmp15 > 2?*((int*)_tmp15)== 9: 0){_LL2A:
_tmp1A=(void*)((struct Cyc_Typerep_App_struct*)_tmp15)->f1;goto _LL25;}else{goto
_LLD;}_LLF: return _tmp16 >> 3;_LL11: return sizeof(float);_LL13: return sizeof(double);
_LL15: return sizeof(void*);_LL17: return sizeof(struct _tagged_arr);_LL19: return
_tmp17;_LL1B: return sizeof(void*);_LL1D: return sizeof(void*);_LL1F: {int max=0;{int
i=0;for(0;i < _get_arr_size(_tmp18,sizeof(void**));i ++){unsigned int _tmp1B=Cyc_Typerep_size_type(*((
void***)_tmp18.curr)[i]);max=(int)(max < _tmp1B? _tmp1B:(unsigned int)max);}}
return(unsigned int)max;}_LL21: return sizeof(void*);_LL23: return Cyc_Typerep_size_type(*
_tmp19);_LL25: return Cyc_Typerep_size_type(_tmp1A);_LLD:;}void Cyc_Typerep_print_typestruct(
void*rep){void*_tmp1C=rep;struct _tagged_arr*_tmp1D;unsigned int _tmp1E;void*
_tmp1F;unsigned int _tmp20;void*_tmp21;struct _tagged_arr _tmp22;unsigned int _tmp23;
struct _tagged_arr _tmp24;struct _tagged_arr _tmp25;struct _tagged_arr _tmp26;void**
_tmp27;struct _tagged_arr _tmp28;struct _tagged_arr _tmp29;void*_tmp2A;_LL2C: if((
unsigned int)_tmp1C > 2?*((int*)_tmp1C)== 0: 0){_LL44: _tmp1D=((struct Cyc_Typerep_Var_struct*)
_tmp1C)->f1;goto _LL2D;}else{goto _LL2E;}_LL2E: if((unsigned int)_tmp1C > 2?*((int*)
_tmp1C)== 1: 0){_LL45: _tmp1E=((struct Cyc_Typerep_Int_struct*)_tmp1C)->f1;goto
_LL2F;}else{goto _LL30;}_LL30: if((int)_tmp1C == 0){goto _LL31;}else{goto _LL32;}
_LL32: if((int)_tmp1C == 1){goto _LL33;}else{goto _LL34;}_LL34: if((unsigned int)
_tmp1C > 2?*((int*)_tmp1C)== 2: 0){_LL47: _tmp20=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1C)->f1;goto _LL46;_LL46: _tmp1F=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1C)->f2;goto _LL35;}else{goto _LL36;}_LL36: if((unsigned int)_tmp1C > 2?*((int*)
_tmp1C)== 3: 0){_LL48: _tmp21=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp1C)->f1;
goto _LL37;}else{goto _LL38;}_LL38: if((unsigned int)_tmp1C > 2?*((int*)_tmp1C)== 4:
0){_LL4A: _tmp23=((struct Cyc_Typerep_Tuple_struct*)_tmp1C)->f1;goto _LL49;_LL49:
_tmp22=((struct Cyc_Typerep_Tuple_struct*)_tmp1C)->f2;goto _LL39;}else{goto _LL3A;}
_LL3A: if((unsigned int)_tmp1C > 2?*((int*)_tmp1C)== 5: 0){_LL4B: _tmp24=((struct Cyc_Typerep_TUnion_struct*)
_tmp1C)->f1;goto _LL3B;}else{goto _LL3C;}_LL3C: if((unsigned int)_tmp1C > 2?*((int*)
_tmp1C)== 6: 0){_LL4C: _tmp25=((struct Cyc_Typerep_XTUnion_struct*)_tmp1C)->f1;goto
_LL3D;}else{goto _LL3E;}_LL3E: if((unsigned int)_tmp1C > 2?*((int*)_tmp1C)== 7: 0){
_LL4D: _tmp26=((struct Cyc_Typerep_Union_struct*)_tmp1C)->f1;goto _LL3F;}else{goto
_LL40;}_LL40: if((unsigned int)_tmp1C > 2?*((int*)_tmp1C)== 8: 0){_LL4F: _tmp28=((
struct Cyc_Typerep_Forall_struct*)_tmp1C)->f1;goto _LL4E;_LL4E: _tmp27=((struct Cyc_Typerep_Forall_struct*)
_tmp1C)->f2;goto _LL41;}else{goto _LL42;}_LL42: if((unsigned int)_tmp1C > 2?*((int*)
_tmp1C)== 9: 0){_LL51: _tmp2A=(void*)((struct Cyc_Typerep_App_struct*)_tmp1C)->f1;
goto _LL50;_LL50: _tmp29=((struct Cyc_Typerep_App_struct*)_tmp1C)->f2;goto _LL43;}
else{goto _LL2B;}_LL2D:({struct Cyc_Std_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(
struct _tagged_arr)*_tmp1D;{void*_tmp2B[1]={& _tmp2C};Cyc_Std_printf(_tag_arr("Var(%s)",
sizeof(unsigned char),8),_tag_arr(_tmp2B,sizeof(void*),1));}});goto _LL2B;_LL2F:({
struct Cyc_Std_Int_pa_struct _tmp2E;_tmp2E.tag=1;_tmp2E.f1=(int)_tmp1E;{void*
_tmp2D[1]={& _tmp2E};Cyc_Std_printf(_tag_arr("Int(%d)",sizeof(unsigned char),8),
_tag_arr(_tmp2D,sizeof(void*),1));}});goto _LL2B;_LL31:({void*_tmp2F[0]={};Cyc_Std_printf(
_tag_arr("Float",sizeof(unsigned char),6),_tag_arr(_tmp2F,sizeof(void*),0));});
goto _LL2B;_LL33:({void*_tmp30[0]={};Cyc_Std_printf(_tag_arr("Double",sizeof(
unsigned char),7),_tag_arr(_tmp30,sizeof(void*),0));});goto _LL2B;_LL35:({struct
Cyc_Std_Int_pa_struct _tmp32;_tmp32.tag=1;_tmp32.f1=(int)_tmp20;{void*_tmp31[1]={&
_tmp32};Cyc_Std_printf(_tag_arr("ThinPtr(%d,",sizeof(unsigned char),12),_tag_arr(
_tmp31,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmp1F);({void*_tmp33[
0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),_tag_arr(_tmp33,
sizeof(void*),0));});goto _LL2B;_LL37:({void*_tmp34[0]={};Cyc_Std_printf(_tag_arr("FatPtr(",
sizeof(unsigned char),8),_tag_arr(_tmp34,sizeof(void*),0));});Cyc_Typerep_print_typestruct(
_tmp21);({void*_tmp35[0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),
_tag_arr(_tmp35,sizeof(void*),0));});goto _LL2B;_LL39:({struct Cyc_Std_Int_pa_struct
_tmp37;_tmp37.tag=1;_tmp37.f1=(int)_tmp23;{void*_tmp36[1]={& _tmp37};Cyc_Std_printf(
_tag_arr("Tuple(%d,[",sizeof(unsigned char),11),_tag_arr(_tmp36,sizeof(void*),1));}});{
int i=0;for(0;i < _get_arr_size(_tmp22,sizeof(struct _tuple0*));i ++){void*_tmp39;
unsigned int _tmp3A;struct _tuple0 _tmp38=*((struct _tuple0**)_tmp22.curr)[i];_LL54:
_tmp3A=_tmp38.f1;goto _LL53;_LL53: _tmp39=_tmp38.f2;goto _LL52;_LL52:({struct Cyc_Std_Int_pa_struct
_tmp3C;_tmp3C.tag=1;_tmp3C.f1=(int)_tmp3A;{void*_tmp3B[1]={& _tmp3C};Cyc_Std_printf(
_tag_arr("$(%d,",sizeof(unsigned char),6),_tag_arr(_tmp3B,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmp39);({struct Cyc_Std_Int_pa_struct _tmp3E;_tmp3E.tag=
1;_tmp3E.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp22,sizeof(struct
_tuple0*))- 1?',':' ')));{void*_tmp3D[1]={& _tmp3E};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp3D,sizeof(void*),1));}});}}({void*_tmp3F[0]={};
Cyc_Std_printf(_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp3F,sizeof(
void*),0));});goto _LL2B;_LL3B:({void*_tmp40[0]={};Cyc_Std_printf(_tag_arr("TUnion(",
sizeof(unsigned char),8),_tag_arr(_tmp40,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp24,sizeof(struct _tuple0*));i ++){void*_tmp42;unsigned int _tmp43;
struct _tuple0 _tmp41=*((struct _tuple0**)_tmp24.curr)[i];_LL57: _tmp43=_tmp41.f1;
goto _LL56;_LL56: _tmp42=_tmp41.f2;goto _LL55;_LL55:({struct Cyc_Std_Int_pa_struct
_tmp45;_tmp45.tag=1;_tmp45.f1=(int)_tmp43;{void*_tmp44[1]={& _tmp45};Cyc_Std_printf(
_tag_arr("$(%d,",sizeof(unsigned char),6),_tag_arr(_tmp44,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmp42);({struct Cyc_Std_Int_pa_struct _tmp47;_tmp47.tag=
1;_tmp47.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp24,sizeof(struct
_tuple0*))- 1?',':' ')));{void*_tmp46[1]={& _tmp47};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp46,sizeof(void*),1));}});}}({void*_tmp48[0]={};
Cyc_Std_printf(_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(_tmp48,sizeof(
void*),0));});goto _LL2B;_LL3D:({void*_tmp49[0]={};Cyc_Std_printf(_tag_arr("XTUnion(",
sizeof(unsigned char),9),_tag_arr(_tmp49,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp25,sizeof(struct _tuple1*));i ++){void*_tmp4B;struct _tagged_arr
_tmp4C;struct _tuple1 _tmp4A=*((struct _tuple1**)_tmp25.curr)[i];_LL5A: _tmp4C=
_tmp4A.f1;goto _LL59;_LL59: _tmp4B=_tmp4A.f2;goto _LL58;_LL58:({struct Cyc_Std_String_pa_struct
_tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _tagged_arr)_tagged_arr_plus(_tmp4C,sizeof(
unsigned char),4);{void*_tmp4D[1]={& _tmp4E};Cyc_Std_printf(_tag_arr("$(%s,",
sizeof(unsigned char),6),_tag_arr(_tmp4D,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmp4B);({struct Cyc_Std_Int_pa_struct _tmp50;_tmp50.tag=1;_tmp50.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp25,sizeof(struct _tuple1*))- 1?',':' ')));{
void*_tmp4F[1]={& _tmp50};Cyc_Std_printf(_tag_arr(")%c",sizeof(unsigned char),4),
_tag_arr(_tmp4F,sizeof(void*),1));}});}}({void*_tmp51[0]={};Cyc_Std_printf(
_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(_tmp51,sizeof(void*),0));});goto
_LL2B;_LL3F:({void*_tmp52[0]={};Cyc_Std_printf(_tag_arr("Union(",sizeof(
unsigned char),7),_tag_arr(_tmp52,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp26,sizeof(void**));i ++){Cyc_Typerep_print_typestruct(*((void***)
_tmp26.curr)[i]);({struct Cyc_Std_Int_pa_struct _tmp54;_tmp54.tag=1;_tmp54.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp26,sizeof(void**))- 1?',':' ')));{void*
_tmp53[1]={& _tmp54};Cyc_Std_printf(_tag_arr("%c",sizeof(unsigned char),3),
_tag_arr(_tmp53,sizeof(void*),1));}});}}({void*_tmp55[0]={};Cyc_Std_printf(
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(_tmp55,sizeof(void*),0));});goto
_LL2B;_LL41:({void*_tmp56[0]={};Cyc_Std_printf(_tag_arr("Forall([",sizeof(
unsigned char),9),_tag_arr(_tmp56,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp28,sizeof(struct _tagged_arr*));i ++){({void*_tmp57[0]={};Cyc_Std_printf(*((
struct _tagged_arr**)_tmp28.curr)[i],_tag_arr(_tmp57,sizeof(void*),0));});({
struct Cyc_Std_Int_pa_struct _tmp59;_tmp59.tag=1;_tmp59.f1=(int)((unsigned int)((
int)(i != _get_arr_size(_tmp28,sizeof(struct _tagged_arr*))- 1?',':' ')));{void*
_tmp58[1]={& _tmp59};Cyc_Std_printf(_tag_arr("%c",sizeof(unsigned char),3),
_tag_arr(_tmp58,sizeof(void*),1));}});}}({void*_tmp5A[0]={};Cyc_Std_printf(
_tag_arr("],",sizeof(unsigned char),3),_tag_arr(_tmp5A,sizeof(void*),0));});Cyc_Typerep_print_typestruct(*
_tmp27);({void*_tmp5B[0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),
_tag_arr(_tmp5B,sizeof(void*),0));});goto _LL2B;_LL43:({void*_tmp5C[0]={};Cyc_Std_printf(
_tag_arr("App(",sizeof(unsigned char),5),_tag_arr(_tmp5C,sizeof(void*),0));});
Cyc_Typerep_print_typestruct(_tmp2A);({void*_tmp5D[0]={};Cyc_Std_printf(_tag_arr(",[",
sizeof(unsigned char),3),_tag_arr(_tmp5D,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp29,sizeof(void*));i ++){Cyc_Typerep_print_typestruct(((void**)
_tmp29.curr)[i]);({struct Cyc_Std_Int_pa_struct _tmp5F;_tmp5F.tag=1;_tmp5F.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp29,sizeof(void*))- 1?',':' ')));{void*
_tmp5E[1]={& _tmp5F};Cyc_Std_printf(_tag_arr("%c",sizeof(unsigned char),3),
_tag_arr(_tmp5E,sizeof(void*),1));}});}}({void*_tmp60[0]={};Cyc_Std_printf(
_tag_arr("])",sizeof(unsigned char),3),_tag_arr(_tmp60,sizeof(void*),0));});goto
_LL2B;_LL2B:;}void Cyc_Typerep_xmlize_typestruct(void*rep){void*_tmp61=rep;struct
_tagged_arr*_tmp62;unsigned int _tmp63;void*_tmp64;unsigned int _tmp65;void*_tmp66;
struct _tagged_arr _tmp67;unsigned int _tmp68;struct _tagged_arr _tmp69;struct
_tagged_arr _tmp6A;struct _tagged_arr _tmp6B;void**_tmp6C;struct _tagged_arr _tmp6D;
struct _tagged_arr _tmp6E;void*_tmp6F;_LL5C: if((unsigned int)_tmp61 > 2?*((int*)
_tmp61)== 0: 0){_LL74: _tmp62=((struct Cyc_Typerep_Var_struct*)_tmp61)->f1;goto
_LL5D;}else{goto _LL5E;}_LL5E: if((unsigned int)_tmp61 > 2?*((int*)_tmp61)== 1: 0){
_LL75: _tmp63=((struct Cyc_Typerep_Int_struct*)_tmp61)->f1;goto _LL5F;}else{goto
_LL60;}_LL60: if((int)_tmp61 == 0){goto _LL61;}else{goto _LL62;}_LL62: if((int)_tmp61
== 1){goto _LL63;}else{goto _LL64;}_LL64: if((unsigned int)_tmp61 > 2?*((int*)_tmp61)
== 2: 0){_LL77: _tmp65=((struct Cyc_Typerep_ThinPtr_struct*)_tmp61)->f1;goto _LL76;
_LL76: _tmp64=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp61)->f2;goto _LL65;}
else{goto _LL66;}_LL66: if((unsigned int)_tmp61 > 2?*((int*)_tmp61)== 3: 0){_LL78:
_tmp66=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp61)->f1;goto _LL67;}else{
goto _LL68;}_LL68: if((unsigned int)_tmp61 > 2?*((int*)_tmp61)== 4: 0){_LL7A: _tmp68=((
struct Cyc_Typerep_Tuple_struct*)_tmp61)->f1;goto _LL79;_LL79: _tmp67=((struct Cyc_Typerep_Tuple_struct*)
_tmp61)->f2;goto _LL69;}else{goto _LL6A;}_LL6A: if((unsigned int)_tmp61 > 2?*((int*)
_tmp61)== 5: 0){_LL7B: _tmp69=((struct Cyc_Typerep_TUnion_struct*)_tmp61)->f1;goto
_LL6B;}else{goto _LL6C;}_LL6C: if((unsigned int)_tmp61 > 2?*((int*)_tmp61)== 6: 0){
_LL7C: _tmp6A=((struct Cyc_Typerep_XTUnion_struct*)_tmp61)->f1;goto _LL6D;}else{
goto _LL6E;}_LL6E: if((unsigned int)_tmp61 > 2?*((int*)_tmp61)== 7: 0){_LL7D: _tmp6B=((
struct Cyc_Typerep_Union_struct*)_tmp61)->f1;goto _LL6F;}else{goto _LL70;}_LL70: if((
unsigned int)_tmp61 > 2?*((int*)_tmp61)== 8: 0){_LL7F: _tmp6D=((struct Cyc_Typerep_Forall_struct*)
_tmp61)->f1;goto _LL7E;_LL7E: _tmp6C=((struct Cyc_Typerep_Forall_struct*)_tmp61)->f2;
goto _LL71;}else{goto _LL72;}_LL72: if((unsigned int)_tmp61 > 2?*((int*)_tmp61)== 9:
0){_LL81: _tmp6F=(void*)((struct Cyc_Typerep_App_struct*)_tmp61)->f1;goto _LL80;
_LL80: _tmp6E=((struct Cyc_Typerep_App_struct*)_tmp61)->f2;goto _LL73;}else{goto
_LL5B;}_LL5D:({struct Cyc_Std_String_pa_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(
struct _tagged_arr)*_tmp62;{void*_tmp70[1]={& _tmp71};Cyc_Std_printf(_tag_arr("<Var name=\"%s\"/>",
sizeof(unsigned char),17),_tag_arr(_tmp70,sizeof(void*),1));}});goto _LL5B;_LL5F:({
struct Cyc_Std_Int_pa_struct _tmp73;_tmp73.tag=1;_tmp73.f1=(int)_tmp63;{void*
_tmp72[1]={& _tmp73};Cyc_Std_printf(_tag_arr("<Int sz=\"%d\"/>",sizeof(
unsigned char),15),_tag_arr(_tmp72,sizeof(void*),1));}});goto _LL5B;_LL61:({void*
_tmp74[0]={};Cyc_Std_printf(_tag_arr("<Float/>",sizeof(unsigned char),9),
_tag_arr(_tmp74,sizeof(void*),0));});goto _LL5B;_LL63:({void*_tmp75[0]={};Cyc_Std_printf(
_tag_arr("<Double/>",sizeof(unsigned char),10),_tag_arr(_tmp75,sizeof(void*),0));});
goto _LL5B;_LL65:({struct Cyc_Std_Int_pa_struct _tmp77;_tmp77.tag=1;_tmp77.f1=(int)
_tmp65;{void*_tmp76[1]={& _tmp77};Cyc_Std_printf(_tag_arr("<ThinPtr sz=\"%d\">",
sizeof(unsigned char),18),_tag_arr(_tmp76,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp64);({void*_tmp78[0]={};Cyc_Std_printf(_tag_arr("</ThinPtr",sizeof(
unsigned char),10),_tag_arr(_tmp78,sizeof(void*),0));});goto _LL5B;_LL67:({void*
_tmp79[0]={};Cyc_Std_printf(_tag_arr("<FatPtr>",sizeof(unsigned char),9),
_tag_arr(_tmp79,sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(_tmp66);({
void*_tmp7A[0]={};Cyc_Std_printf(_tag_arr("</FatPtr>",sizeof(unsigned char),10),
_tag_arr(_tmp7A,sizeof(void*),0));});goto _LL5B;_LL69:({struct Cyc_Std_Int_pa_struct
_tmp7C;_tmp7C.tag=1;_tmp7C.f1=(int)_tmp68;{void*_tmp7B[1]={& _tmp7C};Cyc_Std_printf(
_tag_arr("<Tuple sz=\"%d\">",sizeof(unsigned char),16),_tag_arr(_tmp7B,sizeof(
void*),1));}});{int i=0;for(0;i < _get_arr_size(_tmp67,sizeof(struct _tuple0*));i ++){
void*_tmp7E;unsigned int _tmp7F;struct _tuple0 _tmp7D=*((struct _tuple0**)_tmp67.curr)[
i];_LL84: _tmp7F=_tmp7D.f1;goto _LL83;_LL83: _tmp7E=_tmp7D.f2;goto _LL82;_LL82:({
struct Cyc_Std_Int_pa_struct _tmp81;_tmp81.tag=1;_tmp81.f1=(int)_tmp7F;{void*
_tmp80[1]={& _tmp81};Cyc_Std_printf(_tag_arr("<Component ofs=\"%d\">",sizeof(
unsigned char),21),_tag_arr(_tmp80,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp7E);({void*_tmp82[0]={};Cyc_Std_printf(_tag_arr("</Component>",sizeof(
unsigned char),13),_tag_arr(_tmp82,sizeof(void*),0));});}}({void*_tmp83[0]={};
Cyc_Std_printf(_tag_arr("</Tuple>",sizeof(unsigned char),9),_tag_arr(_tmp83,
sizeof(void*),0));});goto _LL5B;_LL6B:({void*_tmp84[0]={};Cyc_Std_printf(_tag_arr("<TUnion>",
sizeof(unsigned char),9),_tag_arr(_tmp84,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp69,sizeof(struct _tuple0*));i ++){void*_tmp86;unsigned int _tmp87;
struct _tuple0 _tmp85=*((struct _tuple0**)_tmp69.curr)[i];_LL87: _tmp87=_tmp85.f1;
goto _LL86;_LL86: _tmp86=_tmp85.f2;goto _LL85;_LL85:({struct Cyc_Std_Int_pa_struct
_tmp89;_tmp89.tag=1;_tmp89.f1=(int)_tmp87;{void*_tmp88[1]={& _tmp89};Cyc_Std_printf(
_tag_arr("<Tag tag=\"%d\">",sizeof(unsigned char),15),_tag_arr(_tmp88,sizeof(
void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmp86);({void*_tmp8A[0]={};Cyc_Std_printf(
_tag_arr("</Tag>",sizeof(unsigned char),7),_tag_arr(_tmp8A,sizeof(void*),0));});}}({
void*_tmp8B[0]={};Cyc_Std_printf(_tag_arr("</Tunion>",sizeof(unsigned char),10),
_tag_arr(_tmp8B,sizeof(void*),0));});goto _LL5B;_LL6D:({void*_tmp8C[0]={};Cyc_Std_printf(
_tag_arr("<XTUnion>",sizeof(unsigned char),10),_tag_arr(_tmp8C,sizeof(void*),0));});{
int i=0;for(0;i < _get_arr_size(_tmp6A,sizeof(struct _tuple1*));i ++){void*_tmp8E;
struct _tagged_arr _tmp8F;struct _tuple1 _tmp8D=*((struct _tuple1**)_tmp6A.curr)[i];
_LL8A: _tmp8F=_tmp8D.f1;goto _LL89;_LL89: _tmp8E=_tmp8D.f2;goto _LL88;_LL88:({struct
Cyc_Std_String_pa_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(struct _tagged_arr)
_tagged_arr_plus(_tmp8F,sizeof(unsigned char),4);{void*_tmp90[1]={& _tmp91};Cyc_Std_printf(
_tag_arr("<Tag tag=\"%s\">",sizeof(unsigned char),15),_tag_arr(_tmp90,sizeof(
void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmp8E);({void*_tmp92[0]={};Cyc_Std_printf(
_tag_arr("</Tag>",sizeof(unsigned char),7),_tag_arr(_tmp92,sizeof(void*),0));});}}({
void*_tmp93[0]={};Cyc_Std_printf(_tag_arr("</XTunion>",sizeof(unsigned char),11),
_tag_arr(_tmp93,sizeof(void*),0));});goto _LL5B;_LL6F:({void*_tmp94[0]={};Cyc_Std_printf(
_tag_arr("<Union>",sizeof(unsigned char),8),_tag_arr(_tmp94,sizeof(void*),0));});{
int i=0;for(0;i < _get_arr_size(_tmp6B,sizeof(void**));i ++){({void*_tmp95[0]={};
Cyc_Std_printf(_tag_arr("<Case>",sizeof(unsigned char),7),_tag_arr(_tmp95,
sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(*((void***)_tmp6B.curr)[i]);({
void*_tmp96[0]={};Cyc_Std_printf(_tag_arr("</Case>",sizeof(unsigned char),8),
_tag_arr(_tmp96,sizeof(void*),0));});}}({void*_tmp97[0]={};Cyc_Std_printf(
_tag_arr("</Union>",sizeof(unsigned char),9),_tag_arr(_tmp97,sizeof(void*),0));});
goto _LL5B;_LL71:({void*_tmp98[0]={};Cyc_Std_printf(_tag_arr("<Forall vars=\"",
sizeof(unsigned char),15),_tag_arr(_tmp98,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp6D,sizeof(struct _tagged_arr*));i ++){if(i != 0){({void*_tmp99[0]={};
Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(_tmp99,sizeof(
void*),0));});}({void*_tmp9A[0]={};Cyc_Std_printf(*((struct _tagged_arr**)_tmp6D.curr)[
i],_tag_arr(_tmp9A,sizeof(void*),0));});}}({void*_tmp9B[0]={};Cyc_Std_printf(
_tag_arr("\">",sizeof(unsigned char),3),_tag_arr(_tmp9B,sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(*
_tmp6C);({void*_tmp9C[0]={};Cyc_Std_printf(_tag_arr("</Forall>",sizeof(
unsigned char),10),_tag_arr(_tmp9C,sizeof(void*),0));});goto _LL5B;_LL73:({void*
_tmp9D[0]={};Cyc_Std_printf(_tag_arr("<App>",sizeof(unsigned char),6),_tag_arr(
_tmp9D,sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(_tmp6F);{int i=0;for(0;
i < _get_arr_size(_tmp6E,sizeof(void*));i ++){if(i != 0){({void*_tmp9E[0]={};Cyc_Std_printf(
_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(_tmp9E,sizeof(void*),0));});}Cyc_Typerep_xmlize_typestruct(((
void**)_tmp6E.curr)[i]);}}({void*_tmp9F[0]={};Cyc_Std_printf(_tag_arr("</App>",
sizeof(unsigned char),7),_tag_arr(_tmp9F,sizeof(void*),0));});goto _LL5B;_LL5B:;}
static void*Cyc_Typerep_normalize_env(struct Cyc_Dict_Dict*env,void*ts){void*
_tmpA0=ts;struct _tagged_arr*_tmpA1;void*_tmpA2;unsigned int _tmpA3;void*_tmpA4;
struct _tagged_arr _tmpA5;unsigned int _tmpA6;struct _tagged_arr _tmpA7;struct
_tagged_arr _tmpA8;struct _tagged_arr _tmpA9;void**_tmpAA;struct _tagged_arr _tmpAB;
struct _tagged_arr _tmpAC;void*_tmpAD;_LL8C: if((unsigned int)_tmpA0 > 2?*((int*)
_tmpA0)== 0: 0){_LLA4: _tmpA1=((struct Cyc_Typerep_Var_struct*)_tmpA0)->f1;goto
_LL8D;}else{goto _LL8E;}_LL8E: if((unsigned int)_tmpA0 > 2?*((int*)_tmpA0)== 1: 0){
goto _LL8F;}else{goto _LL90;}_LL90: if((int)_tmpA0 == 0){goto _LL91;}else{goto _LL92;}
_LL92: if((int)_tmpA0 == 1){goto _LL93;}else{goto _LL94;}_LL94: if((unsigned int)
_tmpA0 > 2?*((int*)_tmpA0)== 2: 0){_LLA6: _tmpA3=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpA0)->f1;goto _LLA5;_LLA5: _tmpA2=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmpA0)->f2;goto _LL95;}else{goto _LL96;}_LL96: if((unsigned int)_tmpA0 > 2?*((int*)
_tmpA0)== 3: 0){_LLA7: _tmpA4=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpA0)->f1;
goto _LL97;}else{goto _LL98;}_LL98: if((unsigned int)_tmpA0 > 2?*((int*)_tmpA0)== 4:
0){_LLA9: _tmpA6=((struct Cyc_Typerep_Tuple_struct*)_tmpA0)->f1;goto _LLA8;_LLA8:
_tmpA5=((struct Cyc_Typerep_Tuple_struct*)_tmpA0)->f2;goto _LL99;}else{goto _LL9A;}
_LL9A: if((unsigned int)_tmpA0 > 2?*((int*)_tmpA0)== 5: 0){_LLAA: _tmpA7=((struct Cyc_Typerep_TUnion_struct*)
_tmpA0)->f1;goto _LL9B;}else{goto _LL9C;}_LL9C: if((unsigned int)_tmpA0 > 2?*((int*)
_tmpA0)== 6: 0){_LLAB: _tmpA8=((struct Cyc_Typerep_XTUnion_struct*)_tmpA0)->f1;goto
_LL9D;}else{goto _LL9E;}_LL9E: if((unsigned int)_tmpA0 > 2?*((int*)_tmpA0)== 7: 0){
_LLAC: _tmpA9=((struct Cyc_Typerep_Union_struct*)_tmpA0)->f1;goto _LL9F;}else{goto
_LLA0;}_LLA0: if((unsigned int)_tmpA0 > 2?*((int*)_tmpA0)== 8: 0){_LLAE: _tmpAB=((
struct Cyc_Typerep_Forall_struct*)_tmpA0)->f1;goto _LLAD;_LLAD: _tmpAA=((struct Cyc_Typerep_Forall_struct*)
_tmpA0)->f2;goto _LLA1;}else{goto _LLA2;}_LLA2: if((unsigned int)_tmpA0 > 2?*((int*)
_tmpA0)== 9: 0){_LLB0: _tmpAD=(void*)((struct Cyc_Typerep_App_struct*)_tmpA0)->f1;
goto _LLAF;_LLAF: _tmpAC=((struct Cyc_Typerep_App_struct*)_tmpA0)->f2;goto _LLA3;}
else{goto _LL8B;}_LL8D: return((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))
Cyc_Dict_lookup)(env,(struct _tagged_arr*)_tmpA1);_LL8F: goto _LL91;_LL91: goto _LL93;
_LL93: return ts;_LL95: return(void*)({struct Cyc_Typerep_ThinPtr_struct*_tmpAE=
_cycalloc(sizeof(struct Cyc_Typerep_ThinPtr_struct));_tmpAE[0]=({struct Cyc_Typerep_ThinPtr_struct
_tmpAF;_tmpAF.tag=2;_tmpAF.f1=_tmpA3;_tmpAF.f2=(void*)Cyc_Typerep_normalize_env(
env,_tmpA2);_tmpAF;});_tmpAE;});_LL97: return(void*)({struct Cyc_Typerep_FatPtr_struct*
_tmpB0=_cycalloc(sizeof(struct Cyc_Typerep_FatPtr_struct));_tmpB0[0]=({struct Cyc_Typerep_FatPtr_struct
_tmpB1;_tmpB1.tag=3;_tmpB1.f1=(void*)Cyc_Typerep_normalize_env(env,_tmpA4);
_tmpB1;});_tmpB0;});_LL99: {struct _tagged_arr _tmpB2=({unsigned int _tmpBF=
_get_arr_size(_tmpA5,sizeof(struct _tuple0*));struct _tuple0**_tmpC0=(struct
_tuple0**)_cycalloc(_check_times(sizeof(struct _tuple0*),_tmpBF));struct
_tagged_arr _tmpC2=_tag_arr(_tmpC0,sizeof(struct _tuple0*),_get_arr_size(_tmpA5,
sizeof(struct _tuple0*)));{unsigned int _tmpC1=_tmpBF;unsigned int i;for(i=0;i < 
_tmpC1;i ++){_tmpC0[i]=0;}};_tmpC2;});{int i=0;for(0;i < _get_arr_size(_tmpA5,
sizeof(struct _tuple0*));i ++){struct _tuple0 _tmpB4;void*_tmpB5;unsigned int _tmpB6;
struct _tuple0*_tmpB3=((struct _tuple0**)_tmpA5.curr)[i];_tmpB4=*_tmpB3;_LLB3:
_tmpB6=_tmpB4.f1;goto _LLB2;_LLB2: _tmpB5=_tmpB4.f2;goto _LLB1;_LLB1:((struct
_tuple0**)_tmpB2.curr)[i]=({struct _tuple0*_tmpB7=_cycalloc(sizeof(struct _tuple0));
_tmpB7->f1=_tmpB6;_tmpB7->f2=Cyc_Typerep_normalize_env(env,_tmpB5);_tmpB7;});}}{
struct _tagged_arr _tmpB8=({unsigned int _tmpBB=_get_arr_size(_tmpA5,sizeof(struct
_tuple0*));struct _tuple0**_tmpBC=(struct _tuple0**)_cycalloc(_check_times(sizeof(
struct _tuple0*),_tmpBB));struct _tagged_arr _tmpBE=_tag_arr(_tmpBC,sizeof(struct
_tuple0*),_get_arr_size(_tmpA5,sizeof(struct _tuple0*)));{unsigned int _tmpBD=
_tmpBB;unsigned int i;for(i=0;i < _tmpBD;i ++){_tmpBC[i]=(struct _tuple0*)
_check_null(*((struct _tuple0**)_check_unknown_subscript(_tmpB2,sizeof(struct
_tuple0*),(int)i)));}};_tmpBE;});return(void*)({struct Cyc_Typerep_Tuple_struct*
_tmpB9=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));_tmpB9[0]=({struct Cyc_Typerep_Tuple_struct
_tmpBA;_tmpBA.tag=4;_tmpBA.f1=_tmpA6;_tmpBA.f2=(struct _tagged_arr)_tmpB8;_tmpBA;});
_tmpB9;});}}_LL9B: {struct _tagged_arr _tmpC3=({unsigned int _tmpD0=_get_arr_size(
_tmpA7,sizeof(struct _tuple0*));struct _tuple0**_tmpD1=(struct _tuple0**)_cycalloc(
_check_times(sizeof(struct _tuple0*),_tmpD0));struct _tagged_arr _tmpD3=_tag_arr(
_tmpD1,sizeof(struct _tuple0*),_get_arr_size(_tmpA7,sizeof(struct _tuple0*)));{
unsigned int _tmpD2=_tmpD0;unsigned int i;for(i=0;i < _tmpD2;i ++){_tmpD1[i]=0;}};
_tmpD3;});{int i=0;for(0;i < _get_arr_size(_tmpA7,sizeof(struct _tuple0*));i ++){
struct _tuple0 _tmpC5;void*_tmpC6;unsigned int _tmpC7;struct _tuple0*_tmpC4=((struct
_tuple0**)_tmpA7.curr)[i];_tmpC5=*_tmpC4;_LLB6: _tmpC7=_tmpC5.f1;goto _LLB5;_LLB5:
_tmpC6=_tmpC5.f2;goto _LLB4;_LLB4:((struct _tuple0**)_tmpC3.curr)[i]=({struct
_tuple0*_tmpC8=_cycalloc(sizeof(struct _tuple0));_tmpC8->f1=_tmpC7;_tmpC8->f2=Cyc_Typerep_normalize_env(
env,_tmpC6);_tmpC8;});}}{struct _tagged_arr _tmpC9=({unsigned int _tmpCC=
_get_arr_size(_tmpA7,sizeof(struct _tuple0*));struct _tuple0**_tmpCD=(struct
_tuple0**)_cycalloc(_check_times(sizeof(struct _tuple0*),_tmpCC));struct
_tagged_arr _tmpCF=_tag_arr(_tmpCD,sizeof(struct _tuple0*),_get_arr_size(_tmpA7,
sizeof(struct _tuple0*)));{unsigned int _tmpCE=_tmpCC;unsigned int i;for(i=0;i < 
_tmpCE;i ++){_tmpCD[i]=(struct _tuple0*)_check_null(*((struct _tuple0**)
_check_unknown_subscript(_tmpC3,sizeof(struct _tuple0*),(int)i)));}};_tmpCF;});
return(void*)({struct Cyc_Typerep_TUnion_struct*_tmpCA=_cycalloc(sizeof(struct Cyc_Typerep_TUnion_struct));
_tmpCA[0]=({struct Cyc_Typerep_TUnion_struct _tmpCB;_tmpCB.tag=5;_tmpCB.f1=(struct
_tagged_arr)_tmpC9;_tmpCB;});_tmpCA;});}}_LL9D: {struct _tagged_arr _tmpD4=({
unsigned int _tmpE1=_get_arr_size(_tmpA8,sizeof(struct _tuple1*));struct _tuple1**
_tmpE2=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct _tuple1*),_tmpE1));
struct _tagged_arr _tmpE4=_tag_arr(_tmpE2,sizeof(struct _tuple1*),_get_arr_size(
_tmpA8,sizeof(struct _tuple1*)));{unsigned int _tmpE3=_tmpE1;unsigned int i;for(i=0;
i < _tmpE3;i ++){_tmpE2[i]=0;}};_tmpE4;});{int i=0;for(0;i < _get_arr_size(_tmpA8,
sizeof(struct _tuple1*));i ++){struct _tuple1 _tmpD6;void*_tmpD7;struct _tagged_arr
_tmpD8;struct _tuple1*_tmpD5=((struct _tuple1**)_tmpA8.curr)[i];_tmpD6=*_tmpD5;
_LLB9: _tmpD8=_tmpD6.f1;goto _LLB8;_LLB8: _tmpD7=_tmpD6.f2;goto _LLB7;_LLB7:((struct
_tuple1**)_tmpD4.curr)[i]=({struct _tuple1*_tmpD9=_cycalloc(sizeof(struct _tuple1));
_tmpD9->f1=_tmpD8;_tmpD9->f2=Cyc_Typerep_normalize_env(env,_tmpD7);_tmpD9;});}}{
struct _tagged_arr _tmpDA=({unsigned int _tmpDD=_get_arr_size(_tmpA8,sizeof(struct
_tuple1*));struct _tuple1**_tmpDE=(struct _tuple1**)_cycalloc(_check_times(sizeof(
struct _tuple1*),_tmpDD));struct _tagged_arr _tmpE0=_tag_arr(_tmpDE,sizeof(struct
_tuple1*),_get_arr_size(_tmpA8,sizeof(struct _tuple1*)));{unsigned int _tmpDF=
_tmpDD;unsigned int i;for(i=0;i < _tmpDF;i ++){_tmpDE[i]=(struct _tuple1*)
_check_null(*((struct _tuple1**)_check_unknown_subscript(_tmpD4,sizeof(struct
_tuple1*),(int)i)));}};_tmpE0;});return(void*)({struct Cyc_Typerep_XTUnion_struct*
_tmpDB=_cycalloc(sizeof(struct Cyc_Typerep_XTUnion_struct));_tmpDB[0]=({struct Cyc_Typerep_XTUnion_struct
_tmpDC;_tmpDC.tag=6;_tmpDC.f1=(struct _tagged_arr)_tmpDA;_tmpDC;});_tmpDB;});}}
_LL9F: {struct _tagged_arr _tmpE5=({unsigned int _tmpEE=_get_arr_size(_tmpA9,
sizeof(void**));void***_tmpEF=(void***)_cycalloc(_check_times(sizeof(void**),
_tmpEE));struct _tagged_arr _tmpF1=_tag_arr(_tmpEF,sizeof(void**),_get_arr_size(
_tmpA9,sizeof(void**)));{unsigned int _tmpF0=_tmpEE;unsigned int i;for(i=0;i < 
_tmpF0;i ++){_tmpEF[i]=0;}};_tmpF1;});{int i=0;for(0;i < _get_arr_size(_tmpA9,
sizeof(void**));i ++){((void***)_tmpE5.curr)[i]=({void**_tmpE6=_cycalloc(sizeof(
void*));_tmpE6[0]=Cyc_Typerep_normalize_env(env,*((void***)_tmpA9.curr)[i]);
_tmpE6;});}}{struct _tagged_arr _tmpE7=({unsigned int _tmpEA=_get_arr_size(_tmpA9,
sizeof(void**));void***_tmpEB=(void***)_cycalloc(_check_times(sizeof(void**),
_tmpEA));struct _tagged_arr _tmpED=_tag_arr(_tmpEB,sizeof(void**),_get_arr_size(
_tmpA9,sizeof(void**)));{unsigned int _tmpEC=_tmpEA;unsigned int i;for(i=0;i < 
_tmpEC;i ++){_tmpEB[i]=(void**)_check_null(*((void***)_check_unknown_subscript(
_tmpE5,sizeof(void**),(int)i)));}};_tmpED;});return(void*)({struct Cyc_Typerep_Union_struct*
_tmpE8=_cycalloc(sizeof(struct Cyc_Typerep_Union_struct));_tmpE8[0]=({struct Cyc_Typerep_Union_struct
_tmpE9;_tmpE9.tag=7;_tmpE9.f1=(struct _tagged_arr)_tmpE7;_tmpE9;});_tmpE8;});}}
_LLA1: return(void*)({struct Cyc_Typerep_Forall_struct*_tmpF2=_cycalloc(sizeof(
struct Cyc_Typerep_Forall_struct));_tmpF2[0]=({struct Cyc_Typerep_Forall_struct
_tmpF3;_tmpF3.tag=8;_tmpF3.f1=_tmpAB;_tmpF3.f2=({void**_tmpF4=_cycalloc(sizeof(
void*));_tmpF4[0]=Cyc_Typerep_normalize_env(env,*_tmpAA);_tmpF4;});_tmpF3;});
_tmpF2;});_LLA3: {void*_tmpF5=Cyc_Typerep_normalize_env(env,_tmpAD);void**_tmpF6;
struct _tagged_arr _tmpF7;_LLBB: if((unsigned int)_tmpF5 > 2?*((int*)_tmpF5)== 8: 0){
_LLC0: _tmpF7=((struct Cyc_Typerep_Forall_struct*)_tmpF5)->f1;goto _LLBF;_LLBF:
_tmpF6=((struct Cyc_Typerep_Forall_struct*)_tmpF5)->f2;goto _LLBC;}else{goto _LLBD;}
_LLBD: goto _LLBE;_LLBC: if(_get_arr_size(_tmpAC,sizeof(void*))!= _get_arr_size(
_tmpF7,sizeof(struct _tagged_arr*))){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpF8=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpF8[0]=({struct Cyc_Core_Failure_struct
_tmpF9;_tmpF9.tag=Cyc_Core_Failure;_tmpF9.f1=_tag_arr("app appiled wrong number of arguments",
sizeof(unsigned char),38);_tmpF9;});_tmpF8;}));}{int i=0;for(0;i < _get_arr_size(
_tmpAC,sizeof(void*));i ++){env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(env,((struct _tagged_arr**)_tmpF7.curr)[
i],((void**)_tmpAC.curr)[i]);}}return Cyc_Typerep_normalize_env(env,*_tmpF6);
_LLBE:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpFA=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmpFA[0]=({struct Cyc_Core_Failure_struct
_tmpFB;_tmpFB.tag=Cyc_Core_Failure;_tmpFB.f1=_tag_arr("app misapplied",sizeof(
unsigned char),15);_tmpFB;});_tmpFA;}));_LLBA:;}_LL8B:;}int Cyc_Typerep_name_order(
struct _tagged_arr*s1,struct _tagged_arr*s2){return Cyc_Std_strcmp(*s1,*s2);}void*
Cyc_Typerep_normalize(void*ts){return Cyc_Typerep_normalize_env(((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Typerep_name_order),
ts);}
