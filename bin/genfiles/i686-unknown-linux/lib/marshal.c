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
 extern void exit(int);struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _tagged_arr Cstring_to_string(unsigned char*);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[18];
extern unsigned char Cyc_List_Nth[8];struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(
int(*cmp)(void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*
s,void*elt);extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern
unsigned char Cyc_Set_Absent[11];static const int Cyc_Typerep_Var=0;struct Cyc_Typerep_Var_struct{
int tag;struct _tagged_arr*f1;};static const int Cyc_Typerep_Int=1;struct Cyc_Typerep_Int_struct{
int tag;unsigned int f1;};static const int Cyc_Typerep_Float=0;static const int Cyc_Typerep_Double=
1;static const int Cyc_Typerep_ThinPtr=2;struct Cyc_Typerep_ThinPtr_struct{int tag;
unsigned int f1;void*f2;};static const int Cyc_Typerep_FatPtr=3;struct Cyc_Typerep_FatPtr_struct{
int tag;void*f1;};static const int Cyc_Typerep_Tuple=4;struct _tuple0{unsigned int f1;
void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;struct _tagged_arr
f2;};static const int Cyc_Typerep_TUnion=5;struct Cyc_Typerep_TUnion_struct{int tag;
struct _tagged_arr f1;};static const int Cyc_Typerep_XTUnion=6;struct _tuple1{struct
_tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Typerep_Union=7;struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Typerep_Forall=8;struct Cyc_Typerep_Forall_struct{
int tag;struct _tagged_arr f1;void**f2;};static const int Cyc_Typerep_App=9;struct Cyc_Typerep_App_struct{
int tag;void*f1;struct _tagged_arr f2;};extern void*Cyc_Typerep_get_unionbranch(
unsigned int tag,struct _tagged_arr l);extern void*Cyc_Typerep_get_xtunionbranch(
struct _tagged_arr tag,struct _tagged_arr l);unsigned int Cyc_Typerep_size_type(void*
rep);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*
Cyc_Std_stderr;extern int Cyc_Std_fgetc(struct Cyc_Std___sFILE*__stream);extern int
Cyc_Std_fputc(int __c,struct Cyc_Std___sFILE*__stream);extern unsigned int Cyc_Std_fread(
struct _tagged_arr __ptr,unsigned int __size,unsigned int __n,struct Cyc_Std___sFILE*
__stream);extern unsigned int Cyc_Std_fwrite(struct _tagged_arr __ptr,unsigned int
__size,unsigned int __n,struct Cyc_Std___sFILE*__s);extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(
struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};extern int isalnum(int);extern int isalpha(int);extern int isascii(int);
extern int iscntrl(int);extern int isdigit(int);extern int isgraph(int);extern int
islower(int);extern int isprint(int);extern int ispunct(int);extern int isspace(int);
extern int isupper(int);extern int isxdigit(int);extern int toascii(int);extern int
tolower(int);extern int toupper(int);extern int _tolower(int);extern int _toupper(int);
extern void Cyc_Marshal_print_type(void*rep,void*val);extern void Cyc_Marshal_write_type(
void*rep,struct Cyc_Std___sFILE*fp,void*val);extern void*Cyc_Marshal_read_type(
void*rep,struct Cyc_Std___sFILE*fp);struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);extern void*unsafe_cast(void*x);struct _tuple2{struct Cyc_Dict_Dict*f1;
int f2;};struct _tuple3{struct _tagged_arr f1;int f2;};static int Cyc_Marshal_uint_cmp(
unsigned int a,unsigned int b){if(a == b){return 0;}else{if(a < b){return 1;}else{
return - 1;}}}struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set(){return((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(((struct Cyc_Set_Set*(*)(int(*
cmp)(unsigned int,unsigned int)))Cyc_Set_empty)(Cyc_Marshal_uint_cmp),0);}struct
_tuple4{int f1;struct Cyc_Set_Set*f2;};struct _tuple4*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val){return({struct _tuple4*_tmp0=_cycalloc(
sizeof(struct _tuple4));_tmp0->f1=((int(*)(struct Cyc_Set_Set*s,unsigned int elt))
Cyc_Set_member)(set,val);_tmp0->f2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,
unsigned int elt))Cyc_Set_insert)(set,val);_tmp0;});}inline int Cyc_Marshal_index_member(
struct _tuple2 idx,unsigned int m){int _tmp3;struct _tuple2 _tmp1=idx;_LL4: _tmp3=_tmp1.f2;
goto _LL2;_LL2: return m < _tmp3;}struct _tuple2 Cyc_Marshal_empty_addr_index(){return({
struct _tuple2 _tmp5;_tmp5.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);_tmp5.f2=
1;_tmp5;});}int Cyc_Marshal_index_lookup(struct _tuple2 index,unsigned int val){int
_tmp8;struct Cyc_Dict_Dict*_tmpA;struct _tuple2 _tmp6=index;_LL11: _tmpA=_tmp6.f1;
goto _LL9;_LL9: _tmp8=_tmp6.f2;goto _LL7;_LL7: {struct Cyc_Core_Opt*_tmpC=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup_opt)(_tmpA,
val);if(_tmpC != 0){return(int)_tmpC->v;}else{return _tmp8;}}}struct _tuple2 Cyc_Marshal_index_insert(
struct _tuple2 index,unsigned int val){int _tmpF;struct Cyc_Dict_Dict*_tmp11;struct
_tuple2 _tmpD=index;_LL18: _tmp11=_tmpD.f1;goto _LL16;_LL16: _tmpF=_tmpD.f2;goto
_LL14;_LL14: return({struct _tuple2 _tmp13;_tmp13.f1=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,int v))Cyc_Dict_insert)(_tmp11,val,_tmpF);
_tmp13.f2=_tmpF + 1;_tmp13;});}struct _tuple3 Cyc_Marshal_empty_addr_table(){return({
struct _tuple3 _tmp14;_tmp14.f1=_tag_arr(({unsigned int _tmp15=(unsigned int)1;
unsigned int*_tmp16=(unsigned int*)_cycalloc_atomic(_check_times(sizeof(
unsigned int),_tmp15));{unsigned int _tmp17=_tmp15;unsigned int i;for(i=0;i < 
_tmp17;i ++){_tmp16[i]=0;}};_tmp16;}),sizeof(unsigned int),(unsigned int)1);
_tmp14.f2=1;_tmp14;});}inline int Cyc_Marshal_table_member(struct _tuple3 table,
unsigned int m){int _tmp1A;struct _tuple3 _tmp18=table;_LL27: _tmp1A=_tmp18.f2;goto
_LL25;_LL25: return m < _tmp1A;}struct _tuple3 Cyc_Marshal_table_insert(struct _tuple3
tbl,unsigned int i){int _tmp1E;struct _tagged_arr _tmp20;struct _tuple3 _tmp1C=tbl;
_LL33: _tmp20=_tmp1C.f1;goto _LL31;_LL31: _tmp1E=_tmp1C.f2;goto _LL29;_LL29: if(
_get_arr_size(_tmp20,sizeof(unsigned int))== _tmp1E){_tmp20=({unsigned int _tmp22=(
unsigned int)(2 * _tmp1E);unsigned int*_tmp23=(unsigned int*)_cycalloc_atomic(
_check_times(sizeof(unsigned int),_tmp22));struct _tagged_arr _tmp25=_tag_arr(
_tmp23,sizeof(unsigned int),(unsigned int)(2 * _tmp1E));{unsigned int _tmp24=
_tmp22;unsigned int j;for(j=0;j < _tmp24;j ++){_tmp23[j]=j < _tmp1E?*((unsigned int*)
_check_unknown_subscript(_tmp20,sizeof(unsigned int),(int)j)): 0;}};_tmp25;});}((
unsigned int*)_tmp20.curr)[_tmp1E]=i;return({struct _tuple3 _tmp26;_tmp26.f1=
_tmp20;_tmp26.f2=_tmp1E + 1;_tmp26;});}unsigned int Cyc_Marshal_table_lookup(
struct _tuple3 tbl,int i){return*((unsigned int*)_check_unknown_subscript(tbl.f1,
sizeof(unsigned int),i));}struct _tuple5{unsigned int f1;unsigned int f2;
unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_print_type_base(void*rep,struct
Cyc_Set_Set*env,void*val){{void*_tmp27=rep;unsigned int _tmp3D;void*_tmp3F;
unsigned int _tmp41;void*_tmp43;struct _tagged_arr _tmp45;struct _tagged_arr _tmp47;
struct _tagged_arr _tmp49;struct _tagged_arr _tmp4B;_LL41: if((unsigned int)_tmp27 > 2?*((
int*)_tmp27)== Cyc_Typerep_Int: 0){_LL62: _tmp3D=((struct Cyc_Typerep_Int_struct*)
_tmp27)->f1;goto _LL42;}else{goto _LL43;}_LL43: if(_tmp27 == (void*)Cyc_Typerep_Float){
goto _LL44;}else{goto _LL45;}_LL45: if(_tmp27 == (void*)Cyc_Typerep_Double){goto
_LL46;}else{goto _LL47;}_LL47: if((unsigned int)_tmp27 > 2?*((int*)_tmp27)== Cyc_Typerep_ThinPtr:
0){_LL66: _tmp41=((struct Cyc_Typerep_ThinPtr_struct*)_tmp27)->f1;goto _LL64;_LL64:
_tmp3F=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp27)->f2;goto _LL48;}else{
goto _LL49;}_LL49: if((unsigned int)_tmp27 > 2?*((int*)_tmp27)== Cyc_Typerep_FatPtr:
0){_LL68: _tmp43=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp27)->f1;goto _LL50;}
else{goto _LL51;}_LL51: if((unsigned int)_tmp27 > 2?*((int*)_tmp27)== Cyc_Typerep_Tuple:
0){_LL70: _tmp45=((struct Cyc_Typerep_Tuple_struct*)_tmp27)->f2;goto _LL52;}else{
goto _LL53;}_LL53: if((unsigned int)_tmp27 > 2?*((int*)_tmp27)== Cyc_Typerep_TUnion:
0){_LL72: _tmp47=((struct Cyc_Typerep_TUnion_struct*)_tmp27)->f1;goto _LL54;}else{
goto _LL55;}_LL55: if((unsigned int)_tmp27 > 2?*((int*)_tmp27)== Cyc_Typerep_XTUnion:
0){_LL74: _tmp49=((struct Cyc_Typerep_XTUnion_struct*)_tmp27)->f1;goto _LL56;}else{
goto _LL57;}_LL57: if((unsigned int)_tmp27 > 2?*((int*)_tmp27)== Cyc_Typerep_Union:
0){_LL76: _tmp4B=((struct Cyc_Typerep_Union_struct*)_tmp27)->f1;goto _LL58;}else{
goto _LL59;}_LL59: goto _LL60;_LL42: switch(_tmp3D){case 8: _LL77: {unsigned char*x=((
unsigned char*(*)(void*x))unsafe_cast)(val);if(isprint((int)*x)){({struct Cyc_Std_Int_pa_struct
_tmp50;_tmp50.tag=Cyc_Std_Int_pa;_tmp50.f1=(int)((unsigned int)((int)*x));{void*
_tmp4F[1]={& _tmp50};Cyc_Std_printf(_tag_arr("'%c'",sizeof(unsigned char),5),
_tag_arr(_tmp4F,sizeof(void*),1));}});}else{({struct Cyc_Std_Int_pa_struct _tmp52;
_tmp52.tag=Cyc_Std_Int_pa;_tmp52.f1=(int)((unsigned int)((int)*x));{void*_tmp51[
1]={& _tmp52};Cyc_Std_printf(_tag_arr("'\\%d'",sizeof(unsigned char),6),_tag_arr(
_tmp51,sizeof(void*),1));}});}break;}case 16: _LL78: {short*x=((short*(*)(void*x))
unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct _tmp55;_tmp55.tag=Cyc_Std_Int_pa;
_tmp55.f1=(int)((unsigned int)((int)*x));{void*_tmp54[1]={& _tmp55};Cyc_Std_printf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp54,sizeof(void*),1));}});
break;}case 32: _LL83: {int*x=((int*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp58;_tmp58.tag=Cyc_Std_Int_pa;_tmp58.f1=(int)((unsigned int)*x);{void*_tmp57[
1]={& _tmp58};Cyc_Std_printf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(
_tmp57,sizeof(void*),1));}});break;}default: _LL86:({struct Cyc_Std_Int_pa_struct
_tmp5B;_tmp5B.tag=Cyc_Std_Int_pa;_tmp5B.f1=(int)_tmp3D;{void*_tmp5A[1]={& _tmp5B};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("print_type_base: illegal int size %d bits\n",
sizeof(unsigned char),43),_tag_arr(_tmp5A,sizeof(void*),1));}});exit(1);}goto
_LL40;_LL44: {float*x=((float*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp5D;_tmp5D.tag=Cyc_Std_Double_pa;_tmp5D.f1=(double)*x;{void*_tmp5C[1]={&
_tmp5D};Cyc_Std_printf(_tag_arr("%f",sizeof(unsigned char),3),_tag_arr(_tmp5C,
sizeof(void*),1));}});goto _LL40;}_LL46: {double*x=((double*(*)(void*x))
unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct _tmp5F;_tmp5F.tag=Cyc_Std_Double_pa;
_tmp5F.f1=*x;{void*_tmp5E[1]={& _tmp5F};Cyc_Std_printf(_tag_arr("%f",sizeof(
unsigned char),3),_tag_arr(_tmp5E,sizeof(void*),1));}});goto _LL40;}_LL48: {int*x=((
int*(*)(void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));struct _tuple4
_tmp62;struct Cyc_Set_Set*_tmp63;int _tmp65;struct _tuple4*_tmp60=Cyc_Marshal_member_insert(
env,(unsigned int)y);_tmp62=*_tmp60;_LL102: _tmp65=_tmp62.f1;goto _LL100;_LL100:
_tmp63=_tmp62.f2;goto _LL97;_LL97: env=_tmp63;if(! _tmp65){({struct Cyc_Std_Int_pa_struct
_tmp68;_tmp68.tag=Cyc_Std_Int_pa;_tmp68.f1=(unsigned int)y;{void*_tmp67[1]={&
_tmp68};Cyc_Std_printf(_tag_arr("{(@%x) ",sizeof(unsigned char),8),_tag_arr(
_tmp67,sizeof(void*),1));}});if(y != 0){int typ_szb=(int)Cyc_Typerep_size_type(
_tmp3F);int i=0;for(0;i < _tmp41;i ++){env=Cyc_Marshal_print_type_base(_tmp3F,env,(
void*)((void*(*)(int x))unsafe_cast)(y));if(i != _tmp41 - 1){({void*_tmp69[0]={};
Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(_tmp69,sizeof(
void*),0));});}y +=typ_szb;}}else{({void*_tmp6A[0]={};Cyc_Std_printf(_tag_arr("NULL",
sizeof(unsigned char),5),_tag_arr(_tmp6A,sizeof(void*),0));});}({void*_tmp6B[0]={};
Cyc_Std_printf(_tag_arr(" }",sizeof(unsigned char),3),_tag_arr(_tmp6B,sizeof(
void*),0));});}else{({struct Cyc_Std_Int_pa_struct _tmp6D;_tmp6D.tag=Cyc_Std_Int_pa;
_tmp6D.f1=(unsigned int)y;{void*_tmp6C[1]={& _tmp6D};Cyc_Std_printf(_tag_arr("(#%x)",
sizeof(unsigned char),6),_tag_arr(_tmp6C,sizeof(void*),1));}});}goto _LL40;}_LL50: {
struct _tuple5*x=((struct _tuple5*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmp43);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;struct _tuple4 _tmp70;struct Cyc_Set_Set*
_tmp71;int _tmp73;struct _tuple4*_tmp6E=Cyc_Marshal_member_insert(env,base);_tmp70=*
_tmp6E;_LL116: _tmp73=_tmp70.f1;goto _LL114;_LL114: _tmp71=_tmp70.f2;goto _LL111;
_LL111: env=_tmp71;if(! _tmp73){({struct Cyc_Std_Int_pa_struct _tmp76;_tmp76.tag=Cyc_Std_Int_pa;
_tmp76.f1=base;{void*_tmp75[1]={& _tmp76};Cyc_Std_printf(_tag_arr("[(@%x) ",
sizeof(unsigned char),8),_tag_arr(_tmp75,sizeof(void*),1));}});{unsigned int i=
base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_print_type_base(_tmp43,
env,(void*)((void*(*)(unsigned int x))unsafe_cast)(i));if(i + typ_szb < 
last_plus_one){({void*_tmp77[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(
unsigned char),3),_tag_arr(_tmp77,sizeof(void*),0));});}}}({void*_tmp78[0]={};
Cyc_Std_printf(_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp78,sizeof(
void*),0));});}else{({struct Cyc_Std_Int_pa_struct _tmp7A;_tmp7A.tag=Cyc_Std_Int_pa;
_tmp7A.f1=base;{void*_tmp79[1]={& _tmp7A};Cyc_Std_printf(_tag_arr("(#%x)",sizeof(
unsigned char),6),_tag_arr(_tmp79,sizeof(void*),1));}});}goto _LL40;}_LL52:({void*
_tmp7B[0]={};Cyc_Std_printf(_tag_arr("$( ",sizeof(unsigned char),4),_tag_arr(
_tmp7B,sizeof(void*),0));});{int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;
for(0;i < _get_arr_size(_tmp45,sizeof(struct _tuple0*));i ++){void*_tmp7E;
unsigned int _tmp80;struct _tuple0 _tmp7C=*((struct _tuple0**)_tmp45.curr)[i];_LL129:
_tmp80=_tmp7C.f1;goto _LL127;_LL127: _tmp7E=_tmp7C.f2;goto _LL125;_LL125: {int x_ofs=(
int)(x + _tmp80);env=Cyc_Marshal_print_type_base(_tmp7E,env,(void*)((void*(*)(int
x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp45,sizeof(struct _tuple0*))- 1){({
void*_tmp82[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(
_tmp82,sizeof(void*),0));});}}}}({void*_tmp83[0]={};Cyc_Std_printf(_tag_arr(" )",
sizeof(unsigned char),3),_tag_arr(_tmp83,sizeof(void*),0));});goto _LL40;}_LL54:({
void*_tmp84[0]={};Cyc_Std_printf(_tag_arr("&[( ",sizeof(unsigned char),5),
_tag_arr(_tmp84,sizeof(void*),0));});{unsigned int*tagp=((unsigned int*(*)(void*
x))unsafe_cast)(val);if(*tagp < 1024){({struct Cyc_Std_Int_pa_struct _tmp86;_tmp86.tag=
Cyc_Std_Int_pa;_tmp86.f1=(int)*tagp;{void*_tmp85[1]={& _tmp86};Cyc_Std_printf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp85,sizeof(void*),1));}});}
else{struct _tuple4 _tmp89;struct Cyc_Set_Set*_tmp8A;int _tmp8C;struct _tuple4*_tmp87=
Cyc_Marshal_member_insert(env,*tagp);_tmp89=*_tmp87;_LL141: _tmp8C=_tmp89.f1;goto
_LL139;_LL139: _tmp8A=_tmp89.f2;goto _LL136;_LL136: env=_tmp8A;if(! _tmp8C){({struct
Cyc_Std_Int_pa_struct _tmp8F;_tmp8F.tag=Cyc_Std_Int_pa;_tmp8F.f1=*tagp;{void*
_tmp8E[1]={& _tmp8F};Cyc_Std_printf(_tag_arr("(@%x)",sizeof(unsigned char),6),
_tag_arr(_tmp8E,sizeof(void*),1));}});{unsigned int*x=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;void*typ=Cyc_Typerep_get_unionbranch(
tag,_tmp47);env=((struct Cyc_Set_Set*(*)(void*rep,struct Cyc_Set_Set*env,
unsigned int*val))Cyc_Marshal_print_type_base)(typ,env,x);}}else{({struct Cyc_Std_Int_pa_struct
_tmp91;_tmp91.tag=Cyc_Std_Int_pa;_tmp91.f1=*tagp;{void*_tmp90[1]={& _tmp91};Cyc_Std_printf(
_tag_arr("(#%x)",sizeof(unsigned char),6),_tag_arr(_tmp90,sizeof(void*),1));}});}}({
void*_tmp92[0]={};Cyc_Std_printf(_tag_arr(" )]",sizeof(unsigned char),4),
_tag_arr(_tmp92,sizeof(void*),0));});goto _LL40;}_LL56: {unsigned int*xtunionp=((
unsigned int*(*)(void*x))unsafe_cast)(val);struct _tuple4 _tmp95;struct Cyc_Set_Set*
_tmp96;int _tmp98;struct _tuple4*_tmp93=Cyc_Marshal_member_insert(env,*xtunionp);
_tmp95=*_tmp93;_LL153: _tmp98=_tmp95.f1;goto _LL151;_LL151: _tmp96=_tmp95.f2;goto
_LL148;_LL148: env=_tmp96;if(! _tmp98){({struct Cyc_Std_Int_pa_struct _tmp9B;_tmp9B.tag=
Cyc_Std_Int_pa;_tmp9B.f1=*xtunionp;{void*_tmp9A[1]={& _tmp9B};Cyc_Std_printf(
_tag_arr("(@%x)",sizeof(unsigned char),6),_tag_arr(_tmp9A,sizeof(void*),1));}});{
unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);
if(*xtstructp != 0){unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))
unsafe_cast)(*xtstructp + 4);struct _tagged_arr xtname=(struct _tagged_arr)
Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct _tmp9D;_tmp9D.tag=Cyc_Std_String_pa;
_tmp9D.f1=(struct _tagged_arr)xtname;{void*_tmp9C[1]={& _tmp9D};Cyc_Std_printf(
_tag_arr("&%s[( ",sizeof(unsigned char),7),_tag_arr(_tmp9C,sizeof(void*),1));}});{
void*typ=Cyc_Typerep_get_xtunionbranch(xtname,_tmp49);env=((struct Cyc_Set_Set*(*)(
void*rep,struct Cyc_Set_Set*env,unsigned int*val))Cyc_Marshal_print_type_base)(
typ,env,(unsigned int*)_check_null(xtstructp));}}else{unsigned char*xtnamec=((
unsigned char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmp9F;_tmp9F.tag=Cyc_Std_String_pa;_tmp9F.f1=(struct _tagged_arr)xtname;{void*
_tmp9E[1]={& _tmp9F};Cyc_Std_printf(_tag_arr("&%s[( ",sizeof(unsigned char),7),
_tag_arr(_tmp9E,sizeof(void*),1));}});}}}else{({struct Cyc_Std_Int_pa_struct
_tmpA1;_tmpA1.tag=Cyc_Std_Int_pa;_tmpA1.f1=*xtunionp;{void*_tmpA0[1]={& _tmpA1};
Cyc_Std_printf(_tag_arr("(#%x)",sizeof(unsigned char),6),_tag_arr(_tmpA0,sizeof(
void*),1));}});}({void*_tmpA2[0]={};Cyc_Std_printf(_tag_arr(" )]",sizeof(
unsigned char),4),_tag_arr(_tmpA2,sizeof(void*),0));});goto _LL40;}_LL58:({void*
_tmpA3[0]={};Cyc_Std_printf(_tag_arr("[[",sizeof(unsigned char),3),_tag_arr(
_tmpA3,sizeof(void*),0));});{int i=0;for(0;i < _get_arr_size(_tmp4B,sizeof(void**));
i ++){Cyc_Marshal_print_type_base(*((void***)_tmp4B.curr)[i],env,val);if(i < 
_get_arr_size(_tmp4B,sizeof(void**))- 1){({void*_tmpA4[0]={};Cyc_Std_printf(
_tag_arr(" | ",sizeof(unsigned char),4),_tag_arr(_tmpA4,sizeof(void*),0));});}}}({
void*_tmpA5[0]={};Cyc_Std_printf(_tag_arr("]]",sizeof(unsigned char),3),_tag_arr(
_tmpA5,sizeof(void*),0));});goto _LL40;_LL60:(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpA6=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpA6[0]=({struct Cyc_Core_Failure_struct
_tmpA7;_tmpA7.tag=Cyc_Core_Failure;_tmpA7.f1=_tag_arr("Unhandled case in print_type_base",
sizeof(unsigned char),34);_tmpA7;});_tmpA6;}));_LL40:;}return env;}extern int*
__errno_location();void Cyc_Marshal_print_type(void*ts,void*val){struct
_handler_cons _tmpA8;_push_handler(& _tmpA8);{int _tmpAA=0;if(setjmp(_tmpA8.handler)){
_tmpAA=1;}if(! _tmpAA){Cyc_Marshal_print_type_base(ts,Cyc_Marshal_empty_addr_set(),
val);;_pop_handler();}else{void*_tmpA9=(void*)_exn_thrown;void*_tmpAC=_tmpA9;
struct _tagged_arr _tmpB2;_LL174: if(*((void**)_tmpAC)== Cyc_Core_Failure){_LL179:
_tmpB2=((struct Cyc_Core_Failure_struct*)_tmpAC)->f1;goto _LL175;}else{goto _LL176;}
_LL176: goto _LL177;_LL175:({struct Cyc_Std_String_pa_struct _tmpB5;_tmpB5.tag=Cyc_Std_String_pa;
_tmpB5.f1=(struct _tagged_arr)_tmpB2;{void*_tmpB4[1]={& _tmpB5};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(
_tmpB4,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpB6=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpB6[0]=({struct Cyc_Core_Failure_struct
_tmpB7;_tmpB7.tag=Cyc_Core_Failure;_tmpB7.f1=_tmpB2;_tmpB7;});_tmpB6;}));_LL177:(
void)_throw(_tmpAC);_LL173:;}}}void Cyc_Marshal_cycputchar(unsigned char x,struct
Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct _tagged_arr)
_tag_arr(& x,sizeof(unsigned char),1)),sizeof(unsigned char),1,fp)< 1){(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpB8=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmpB8[0]=({struct Cyc_Core_Failure_struct _tmpB9;_tmpB9.tag=Cyc_Core_Failure;
_tmpB9.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmpB9;});_tmpB8;}));}}
void Cyc_Marshal_cycputshort(short x,struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(short),1)),sizeof(short),
1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpBA=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmpBA[0]=({struct Cyc_Core_Failure_struct
_tmpBB;_tmpBB.tag=Cyc_Core_Failure;_tmpBB.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmpBB;});_tmpBA;}));}}void Cyc_Marshal_cycputint(int x,struct
Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct _tagged_arr)
_tag_arr(& x,sizeof(int),1)),sizeof(int),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpBC=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpBC[0]=({struct Cyc_Core_Failure_struct
_tmpBD;_tmpBD.tag=Cyc_Core_Failure;_tmpBD.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmpBD;});_tmpBC;}));}}void Cyc_Marshal_cycputfloat(float x,
struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(float),1)),sizeof(float),1,fp)< 1){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmpBE=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmpBE[0]=({struct Cyc_Core_Failure_struct _tmpBF;_tmpBF.tag=Cyc_Core_Failure;
_tmpBF.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmpBF;});_tmpBE;}));}}
void Cyc_Marshal_cycputdouble(double x,struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(double),1)),sizeof(
double),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpC0=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpC0[0]=({struct Cyc_Core_Failure_struct
_tmpC1;_tmpC1.tag=Cyc_Core_Failure;_tmpC1.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmpC1;});_tmpC0;}));}}void Cyc_Marshal_cycputvarint(
unsigned int i,struct Cyc_Std___sFILE*fp){int width=0;unsigned int mask=-256;while((
int)mask){if((int)(i & mask)){width ++;}mask <<=8;}Cyc_Std_fputc((int)(width << 6 | i
& 63),fp);i >>=6;{int j=0;for(0;j < width;j ++){Cyc_Std_fputc((int)(i & 255),fp);i >>=
8;}}}unsigned char Cyc_Marshal_cycgetchar(struct Cyc_Std___sFILE*fp){struct
_RegionHandle _tmpC2=_new_region("r");struct _RegionHandle*r=& _tmpC2;_push_region(
r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpC6=sizeof(unsigned char);
unsigned char*_tmpC7=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmpC6));{unsigned int _tmpC8=_tmpC6;unsigned int i;for(i=0;i < 
_tmpC8;i ++){_tmpC7[i]='\000';}};_tmpC7;}),sizeof(unsigned char),sizeof(
unsigned char));if(Cyc_Std_fread(x,sizeof(unsigned char),1,fp)< 1){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmpC3=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmpC3[0]=({struct Cyc_Core_Failure_struct _tmpC4;_tmpC4.tag=Cyc_Core_Failure;
_tmpC4.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmpC4;});_tmpC3;}));}{
unsigned char*xx=((unsigned char*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));unsigned char
_tmpC5=*xx;_npop_handler(0);return _tmpC5;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc_Std___sFILE*fp){struct _RegionHandle _tmpC9=_new_region("r");struct
_RegionHandle*r=& _tmpC9;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpCD=sizeof(short);unsigned char*_tmpCE=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmpCD));{unsigned int _tmpCF=
_tmpCD;unsigned int i;for(i=0;i < _tmpCF;i ++){_tmpCE[i]='\000';}};_tmpCE;}),
sizeof(unsigned char),sizeof(short));if(Cyc_Std_fread(x,sizeof(short),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpCA=_cycalloc(sizeof(struct
Cyc_Core_Failure_struct));_tmpCA[0]=({struct Cyc_Core_Failure_struct _tmpCB;_tmpCB.tag=
Cyc_Core_Failure;_tmpCB.f1=_tag_arr("Read failure",sizeof(unsigned char),13);
_tmpCB;});_tmpCA;}));}{short*xx=((short*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));short _tmpCC=*
xx;_npop_handler(0);return _tmpCC;}};_pop_region(r);}int Cyc_Marshal_cycgetint(
struct Cyc_Std___sFILE*fp){struct _RegionHandle _tmpD0=_new_region("r");struct
_RegionHandle*r=& _tmpD0;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpD4=sizeof(int);unsigned char*_tmpD5=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmpD4));{unsigned int _tmpD6=
_tmpD4;unsigned int i;for(i=0;i < _tmpD6;i ++){_tmpD5[i]='\000';}};_tmpD5;}),
sizeof(unsigned char),sizeof(int));if(Cyc_Std_fread(x,sizeof(int),1,fp)< 1){(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpD1=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmpD1[0]=({struct Cyc_Core_Failure_struct _tmpD2;_tmpD2.tag=Cyc_Core_Failure;
_tmpD2.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmpD2;});_tmpD1;}));}{
int*xx=((int*(*)(unsigned char*x))unsafe_cast)((unsigned char*)_check_null(
_untag_arr(x,sizeof(unsigned char),1)));int _tmpD3=*xx;_npop_handler(0);return
_tmpD3;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(struct Cyc_Std___sFILE*fp){
struct _RegionHandle _tmpD7=_new_region("r");struct _RegionHandle*r=& _tmpD7;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpDB=sizeof(float);
unsigned char*_tmpDC=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmpDB));{unsigned int _tmpDD=_tmpDB;unsigned int i;for(i=0;i < 
_tmpDD;i ++){_tmpDC[i]='\000';}};_tmpDC;}),sizeof(unsigned char),sizeof(float));
if(Cyc_Std_fread(x,sizeof(float),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpD8=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpD8[0]=({struct Cyc_Core_Failure_struct
_tmpD9;_tmpD9.tag=Cyc_Core_Failure;_tmpD9.f1=_tag_arr("Read failure",sizeof(
unsigned char),13);_tmpD9;});_tmpD8;}));}{float*xx=((float*(*)(unsigned char*x))
unsafe_cast)((unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));
float _tmpDA=*xx;_npop_handler(0);return _tmpDA;}};_pop_region(r);}double Cyc_Marshal_cycgetdouble(
struct Cyc_Std___sFILE*fp){struct _RegionHandle _tmpDE=_new_region("r");struct
_RegionHandle*r=& _tmpDE;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpE2=sizeof(double);unsigned char*_tmpE3=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmpE2));{unsigned int _tmpE4=
_tmpE2;unsigned int i;for(i=0;i < _tmpE4;i ++){_tmpE3[i]='\000';}};_tmpE3;}),
sizeof(unsigned char),sizeof(double));if(Cyc_Std_fread(x,sizeof(double),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpDF=_cycalloc(sizeof(struct
Cyc_Core_Failure_struct));_tmpDF[0]=({struct Cyc_Core_Failure_struct _tmpE0;_tmpE0.tag=
Cyc_Core_Failure;_tmpE0.f1=_tag_arr("Read failure",sizeof(unsigned char),13);
_tmpE0;});_tmpDF;}));}{double*xx=((double*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));double _tmpE1=*
xx;_npop_handler(0);return _tmpE1;}};_pop_region(r);}unsigned int Cyc_Marshal_cycgetvarint(
struct Cyc_Std___sFILE*fp){int i=0;int j=(int)Cyc_Marshal_cycgetchar(fp);int width=(
j & 192)>> 6;int pos=6;i +=j & 63;{int k=0;for(0;k < width;k ++){j=(int)Cyc_Marshal_cycgetchar(
fp);i |=j << pos;pos +=8;}}return(unsigned int)i;}struct _tuple2 Cyc_Marshal_write_type_base(
void*rep,struct _tuple2 env,struct Cyc_Std___sFILE*fp,void*val){{void*_tmpE5=rep;
unsigned int _tmpFB;void*_tmpFD;unsigned int _tmpFF;void*_tmp101;struct _tagged_arr
_tmp103;struct _tagged_arr _tmp105;struct _tagged_arr _tmp107;struct _tagged_arr
_tmp109;_LL231: if((unsigned int)_tmpE5 > 2?*((int*)_tmpE5)== Cyc_Typerep_Int: 0){
_LL252: _tmpFB=((struct Cyc_Typerep_Int_struct*)_tmpE5)->f1;goto _LL232;}else{goto
_LL233;}_LL233: if(_tmpE5 == (void*)Cyc_Typerep_Float){goto _LL234;}else{goto _LL235;}
_LL235: if(_tmpE5 == (void*)Cyc_Typerep_Double){goto _LL236;}else{goto _LL237;}
_LL237: if((unsigned int)_tmpE5 > 2?*((int*)_tmpE5)== Cyc_Typerep_ThinPtr: 0){
_LL256: _tmpFF=((struct Cyc_Typerep_ThinPtr_struct*)_tmpE5)->f1;goto _LL254;_LL254:
_tmpFD=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmpE5)->f2;goto _LL238;}else{
goto _LL239;}_LL239: if((unsigned int)_tmpE5 > 2?*((int*)_tmpE5)== Cyc_Typerep_FatPtr:
0){_LL258: _tmp101=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpE5)->f1;goto
_LL240;}else{goto _LL241;}_LL241: if((unsigned int)_tmpE5 > 2?*((int*)_tmpE5)== Cyc_Typerep_Tuple:
0){_LL260: _tmp103=((struct Cyc_Typerep_Tuple_struct*)_tmpE5)->f2;goto _LL242;}
else{goto _LL243;}_LL243: if((unsigned int)_tmpE5 > 2?*((int*)_tmpE5)== Cyc_Typerep_TUnion:
0){_LL262: _tmp105=((struct Cyc_Typerep_TUnion_struct*)_tmpE5)->f1;goto _LL244;}
else{goto _LL245;}_LL245: if((unsigned int)_tmpE5 > 2?*((int*)_tmpE5)== Cyc_Typerep_XTUnion:
0){_LL264: _tmp107=((struct Cyc_Typerep_XTUnion_struct*)_tmpE5)->f1;goto _LL246;}
else{goto _LL247;}_LL247: if((unsigned int)_tmpE5 > 2?*((int*)_tmpE5)== Cyc_Typerep_Union:
0){_LL266: _tmp109=((struct Cyc_Typerep_Union_struct*)_tmpE5)->f1;goto _LL248;}
else{goto _LL249;}_LL249: goto _LL250;_LL232: switch(_tmpFB){case 8: _LL267: {
unsigned char*x=((unsigned char*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputchar(*
x,fp);break;}case 16: _LL268: {short*x=((short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*
x,fp);break;}case 32: _LL269: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*
x,fp);break;}default: _LL270:({struct Cyc_Std_Int_pa_struct _tmp111;_tmp111.tag=Cyc_Std_Int_pa;
_tmp111.f1=(int)_tmpFB;{void*_tmp110[1]={& _tmp111};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("write_type_base: illegal int size %d bits\n",sizeof(unsigned char),43),
_tag_arr(_tmp110,sizeof(void*),1));}});exit(1);}goto _LL230;_LL234: {float*x=((
float*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputfloat(*x,fp);goto _LL230;}
_LL236: {double*x=((double*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*
x,fp);goto _LL230;}_LL238: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));int _tmp112=Cyc_Marshal_index_lookup(env,(unsigned int)y);Cyc_Marshal_cycputvarint((
unsigned int)_tmp112,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmp112)){
env=Cyc_Marshal_index_insert(env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmpFD);int i=0;for(0;i < _tmpFF;i ++){env=Cyc_Marshal_write_type_base(_tmpFD,env,
fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}goto _LL230;}_LL240: {
struct _tuple5*x=((struct _tuple5*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmp101);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;unsigned int pos=(curr - base)/ typ_szb;
unsigned int sz=(last_plus_one - base)/ typ_szb;int _tmp113=Cyc_Marshal_index_lookup(
env,base);Cyc_Marshal_cycputvarint((unsigned int)_tmp113,fp);Cyc_Marshal_cycputvarint(
pos,fp);Cyc_Marshal_cycputvarint(sz,fp);if(! Cyc_Marshal_index_member(env,(
unsigned int)_tmp113)){env=Cyc_Marshal_index_insert(env,base);{unsigned int i=
base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(_tmp101,
env,fp,(void*)((void*(*)(unsigned int x))unsafe_cast)(i));}}}goto _LL230;}_LL242: {
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmp103,
sizeof(struct _tuple0*));i ++){void*_tmp116;unsigned int _tmp118;struct _tuple0
_tmp114=*((struct _tuple0**)_tmp103.curr)[i];_LL281: _tmp118=_tmp114.f1;goto _LL279;
_LL279: _tmp116=_tmp114.f2;goto _LL277;_LL277: {int x_ofs=(int)(x + _tmp118);env=Cyc_Marshal_write_type_base(
_tmp116,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL230;}
_LL244: {unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp
< 1024){Cyc_Marshal_cycputvarint(*tagp,fp);}else{int _tmp11A=Cyc_Marshal_index_lookup(
env,*tagp);Cyc_Marshal_cycputvarint((unsigned int)(_tmp11A + 1024),fp);if(! Cyc_Marshal_index_member(
env,(unsigned int)_tmp11A)){env=Cyc_Marshal_index_insert(env,*tagp);{
unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);
unsigned int tag=*x;Cyc_Marshal_cycputvarint(tag,fp);{void*typ=Cyc_Typerep_get_unionbranch(
tag,_tmp105);env=((struct _tuple2(*)(void*rep,struct _tuple2 env,struct Cyc_Std___sFILE*
fp,unsigned int*val))Cyc_Marshal_write_type_base)(typ,env,fp,x);}}}}goto _LL230;}
_LL246: {unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(val);int
_tmp11B=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint((
unsigned int)_tmp11B,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmp11B)){
env=Cyc_Marshal_index_insert(env,*xtunionp);{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',
fp);{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtstructp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmp11D;_tmp11D.tag=Cyc_Std_String_pa;
_tmp11D.f1=(struct _tagged_arr)xtname;{void*_tmp11C[1]={& _tmp11D};Cyc_Std_fprintf(
fp,_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp11C,sizeof(void*),1));}});
Cyc_Marshal_cycputchar('\000',fp);{void*typ=Cyc_Typerep_get_xtunionbranch(xtname,
_tmp107);env=((struct _tuple2(*)(void*rep,struct _tuple2 env,struct Cyc_Std___sFILE*
fp,unsigned int*val))Cyc_Marshal_write_type_base)(typ,env,fp,(unsigned int*)
_check_null(xtstructp));}}}else{Cyc_Marshal_cycputchar('\000',fp);{unsigned char*
xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct
_tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmp11F;_tmp11F.tag=Cyc_Std_String_pa;_tmp11F.f1=(struct _tagged_arr)xtname;{void*
_tmp11E[1]={& _tmp11F};Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(unsigned char),3),
_tag_arr(_tmp11E,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',fp);}}}}
goto _LL230;}_LL248: {int sz=(int)Cyc_Typerep_size_type(rep);env=Cyc_Marshal_write_type_base((
void*)({struct Cyc_Typerep_Tuple_struct*_tmp120=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));
_tmp120[0]=({struct Cyc_Typerep_Tuple_struct _tmp121;_tmp121.tag=Cyc_Typerep_Tuple;
_tmp121.f1=(unsigned int)sz;_tmp121.f2=({unsigned int _tmp122=(unsigned int)sz;
struct _tuple0**_tmp123=(struct _tuple0**)_cycalloc(_check_times(sizeof(struct
_tuple0*),_tmp122));struct _tagged_arr _tmp128=_tag_arr(_tmp123,sizeof(struct
_tuple0*),(unsigned int)sz);{unsigned int _tmp124=_tmp122;unsigned int i;for(i=0;i
< _tmp124;i ++){_tmp123[i]=({struct _tuple0*_tmp125=_cycalloc(sizeof(struct _tuple0));
_tmp125->f1=i;_tmp125->f2=(void*)({struct Cyc_Typerep_Int_struct*_tmp126=
_cycalloc_atomic(sizeof(struct Cyc_Typerep_Int_struct));_tmp126[0]=({struct Cyc_Typerep_Int_struct
_tmp127;_tmp127.tag=Cyc_Typerep_Int;_tmp127.f1=8;_tmp127;});_tmp126;});_tmp125;});}};
_tmp128;});_tmp121;});_tmp120;}),env,fp,val);goto _LL230;}_LL250:(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp129=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp129[0]=({struct Cyc_Core_Failure_struct _tmp12A;_tmp12A.tag=Cyc_Core_Failure;
_tmp12A.f1=_tag_arr("Unhandled case in write_type_base",sizeof(unsigned char),34);
_tmp12A;});_tmp129;}));_LL230:;}return env;}void Cyc_Marshal_write_type(void*ts,
struct Cyc_Std___sFILE*fp,void*val){struct _handler_cons _tmp12B;_push_handler(&
_tmp12B);{int _tmp12D=0;if(setjmp(_tmp12B.handler)){_tmp12D=1;}if(! _tmp12D){Cyc_Marshal_write_type_base(
ts,Cyc_Marshal_empty_addr_index(),fp,val);;_pop_handler();}else{void*_tmp12C=(
void*)_exn_thrown;void*_tmp12F=_tmp12C;struct _tagged_arr _tmp135;_LL305: if(*((
void**)_tmp12F)== Cyc_Core_Failure){_LL310: _tmp135=((struct Cyc_Core_Failure_struct*)
_tmp12F)->f1;goto _LL306;}else{goto _LL307;}_LL307: goto _LL308;_LL306:({struct Cyc_Std_String_pa_struct
_tmp138;_tmp138.tag=Cyc_Std_String_pa;_tmp138.f1=(struct _tagged_arr)_tmp135;{
void*_tmp137[1]={& _tmp138};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",
sizeof(unsigned char),19),_tag_arr(_tmp137,sizeof(void*),1));}});(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp139=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp139[0]=({struct Cyc_Core_Failure_struct _tmp13A;_tmp13A.tag=Cyc_Core_Failure;
_tmp13A.f1=_tmp135;_tmp13A;});_tmp139;}));_LL308:(void)_throw(_tmp12F);_LL304:;}}}
void Cyc_Marshal_fscanntstring(struct Cyc_Std___sFILE*fp,struct _tagged_arr s){int c;
int i=0;while(c=Cyc_Std_fgetc(fp)){if(c == - 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp13B=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp13B[0]=({struct Cyc_Core_Failure_struct
_tmp13C;_tmp13C.tag=Cyc_Core_Failure;_tmp13C.f1=_tag_arr("Read error",sizeof(
unsigned char),11);_tmp13C;});_tmp13B;}));}((unsigned char*)s.curr)[i]=(
unsigned char)c;i ++;}((unsigned char*)s.curr)[i]='\000';}struct _tuple3 Cyc_Marshal_read_type_base(
void*rep,struct _tuple3 env,struct Cyc_Std___sFILE*fp,void*result){{void*_tmp13D=
rep;unsigned int _tmp153;void*_tmp155;unsigned int _tmp157;void*_tmp159;struct
_tagged_arr _tmp15B;struct _tagged_arr _tmp15D;struct _tagged_arr _tmp15F;struct
_tagged_arr _tmp161;_LL319: if((unsigned int)_tmp13D > 2?*((int*)_tmp13D)== Cyc_Typerep_Int:
0){_LL340: _tmp153=((struct Cyc_Typerep_Int_struct*)_tmp13D)->f1;goto _LL320;}else{
goto _LL321;}_LL321: if(_tmp13D == (void*)Cyc_Typerep_Float){goto _LL322;}else{goto
_LL323;}_LL323: if(_tmp13D == (void*)Cyc_Typerep_Double){goto _LL324;}else{goto
_LL325;}_LL325: if((unsigned int)_tmp13D > 2?*((int*)_tmp13D)== Cyc_Typerep_ThinPtr:
0){_LL344: _tmp157=((struct Cyc_Typerep_ThinPtr_struct*)_tmp13D)->f1;goto _LL342;
_LL342: _tmp155=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp13D)->f2;goto
_LL326;}else{goto _LL327;}_LL327: if((unsigned int)_tmp13D > 2?*((int*)_tmp13D)== 
Cyc_Typerep_FatPtr: 0){_LL346: _tmp159=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp13D)->f1;goto _LL328;}else{goto _LL329;}_LL329: if((unsigned int)_tmp13D > 2?*((
int*)_tmp13D)== Cyc_Typerep_Tuple: 0){_LL348: _tmp15B=((struct Cyc_Typerep_Tuple_struct*)
_tmp13D)->f2;goto _LL330;}else{goto _LL331;}_LL331: if((unsigned int)_tmp13D > 2?*((
int*)_tmp13D)== Cyc_Typerep_TUnion: 0){_LL350: _tmp15D=((struct Cyc_Typerep_TUnion_struct*)
_tmp13D)->f1;goto _LL332;}else{goto _LL333;}_LL333: if((unsigned int)_tmp13D > 2?*((
int*)_tmp13D)== Cyc_Typerep_XTUnion: 0){_LL352: _tmp15F=((struct Cyc_Typerep_XTUnion_struct*)
_tmp13D)->f1;goto _LL334;}else{goto _LL335;}_LL335: if((unsigned int)_tmp13D > 2?*((
int*)_tmp13D)== Cyc_Typerep_Union: 0){_LL354: _tmp161=((struct Cyc_Typerep_Union_struct*)
_tmp13D)->f1;goto _LL336;}else{goto _LL337;}_LL337: goto _LL338;_LL320: switch(
_tmp153){case 8: _LL355: {unsigned char*x=((unsigned char*(*)(void*x))unsafe_cast)(
result);*x=Cyc_Marshal_cycgetchar(fp);break;}case 16: _LL356: {short*x=((short*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetshort(fp);break;}case 32: _LL357: {
int*x=((int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(fp);break;}
default: _LL358:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp168=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp168[0]=({struct Cyc_Core_Failure_struct
_tmp169;_tmp169.tag=Cyc_Core_Failure;_tmp169.f1=_tag_arr("read_type_base: illegal int size bits",
sizeof(unsigned char),38);_tmp169;});_tmp168;}));}goto _LL318;_LL322: {float*y=((
float*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetfloat(fp);goto _LL318;}
_LL324: {double*y=((double*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(
fp);goto _LL318;}_LL326: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmp155);struct
_tagged_arr buf=({unsigned int _tmp16A=_tmp157 * typ_szb;unsigned char*_tmp16B=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp16A));
struct _tagged_arr _tmp16D=_tag_arr(_tmp16B,sizeof(unsigned char),_tmp157 * typ_szb);{
unsigned int _tmp16C=_tmp16A;unsigned int i;for(i=0;i < _tmp16C;i ++){_tmp16B[i]='\000';}};
_tmp16D;});unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(
_untag_arr(buf,sizeof(unsigned char),1)));unsigned int*x=((unsigned int*(*)(void*
x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(
int)bufptr;int i=0;for(0;i < _tmp157;i ++){env=Cyc_Marshal_read_type_base(_tmp155,
env,fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*
x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(
env,mem);}goto _LL318;}_LL328: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);int pos=(
int)Cyc_Marshal_cycgetvarint(fp);int sz=(int)Cyc_Marshal_cycgetvarint(fp);int
typ_szb=(int)Cyc_Typerep_size_type(_tmp159);if(! Cyc_Marshal_table_member(env,(
unsigned int)mem)){struct _tagged_arr buf=({unsigned int _tmp16E=(unsigned int)(sz * 
typ_szb);unsigned char*_tmp16F=(unsigned char*)_cycalloc_atomic(_check_times(
sizeof(unsigned char),_tmp16E));struct _tagged_arr _tmp171=_tag_arr(_tmp16F,
sizeof(unsigned char),(unsigned int)(sz * typ_szb));{unsigned int _tmp170=_tmp16E;
unsigned int i;for(i=0;i < _tmp170;i ++){_tmp16F[i]='\000';}};_tmp171;});
unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,
sizeof(unsigned char),1)));struct _tuple5*x=((struct _tuple5*(*)(void*x))
unsafe_cast)(result);(*x).f1=bufptr;(*x).f2=bufptr + pos * typ_szb;(*x).f3=bufptr + 
sz * typ_szb;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int i=0;
for(0;i < sz;i ++){env=Cyc_Marshal_read_type_base(_tmp159,env,fp,(void*)((void*(*)(
int x))unsafe_cast)(y));y +=typ_szb;}}}else{struct _tuple5*x=((struct _tuple5*(*)(
void*x))unsafe_cast)(result);(*x).f1=Cyc_Marshal_table_lookup(env,mem);(*x).f2=(*
x).f1 + typ_szb * pos;(*x).f3=(*x).f1 + typ_szb * sz;}goto _LL318;}_LL330: {
unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < 
_get_arr_size(_tmp15B,sizeof(struct _tuple0*));i ++){void*_tmp174;unsigned int
_tmp176;struct _tuple0 _tmp172=*((struct _tuple0**)_tmp15B.curr)[i];_LL375: _tmp176=
_tmp172.f1;goto _LL373;_LL373: _tmp174=_tmp172.f2;goto _LL371;_LL371: {int x_ofs=(
int)(x + _tmp176);env=Cyc_Marshal_read_type_base(_tmp174,env,fp,(void*)((void*(*)(
int x))unsafe_cast)(x_ofs));}}}goto _LL318;}_LL332: {int tagmem=(int)Cyc_Marshal_cycgetvarint(
fp);if(tagmem < 1024){int tag=tagmem;unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=(unsigned int)tag;}else{int mem=tagmem - 1024;if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){int tag=(int)Cyc_Marshal_cycgetvarint(fp);void*typ=Cyc_Typerep_get_unionbranch((
unsigned int)tag,_tmp15D);struct _tagged_arr buf=({unsigned int _tmp178=Cyc_Typerep_size_type(
typ);unsigned char*_tmp179=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp178));struct _tagged_arr _tmp17B=_tag_arr(_tmp179,sizeof(
unsigned char),Cyc_Typerep_size_type(typ));{unsigned int _tmp17A=_tmp178;
unsigned int i;for(i=0;i < _tmp17A;i ++){_tmp179[i]='\000';}};_tmp17B;});
unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,
sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(env,bufptr);{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_read_type_base(
typ,env,fp,(void*)((void*(*)(unsigned int x))unsafe_cast)(bufptr));}}else{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(
env,mem);}}goto _LL318;}_LL334: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){unsigned char nonnull=(unsigned char)Cyc_Marshal_cycgetchar(
fp);struct _tagged_arr s=_tag_arr(({unsigned int _tmp180=(unsigned int)100;
unsigned char*_tmp181=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp180));{unsigned int _tmp182=_tmp180;unsigned int i;for(i=0;i < 
_tmp182;i ++){_tmp181[i]='\000';}};_tmp181;}),sizeof(unsigned char),(unsigned int)
100);struct _tagged_arr xtname=_tagged_arr_plus(s,sizeof(unsigned char),4);Cyc_Marshal_fscanntstring(
fp,xtname);if((int)nonnull){void*typ=Cyc_Typerep_get_xtunionbranch((struct
_tagged_arr)xtname,_tmp15F);struct _tagged_arr buf=({unsigned int _tmp17C=Cyc_Typerep_size_type(
typ);unsigned char*_tmp17D=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp17C));struct _tagged_arr _tmp17F=_tag_arr(_tmp17D,sizeof(
unsigned char),Cyc_Typerep_size_type(typ));{unsigned int _tmp17E=_tmp17C;
unsigned int i;for(i=0;i < _tmp17E;i ++){_tmp17D[i]='\000';}};_tmp17F;});
unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,
sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(env,bufptr);{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_read_type_base(
typ,env,fp,(void*)((void*(*)(unsigned int x))unsafe_cast)(bufptr));{unsigned int*
tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*tagptr=(
unsigned int)((unsigned char*)_check_null(_untag_arr(s,sizeof(unsigned char),1)));}}}
else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(
unsigned int)((unsigned char*)_check_null(_untag_arr(s,sizeof(unsigned char),1)));
env=Cyc_Marshal_table_insert(env,*x);}}else{unsigned int*x=((unsigned int*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL318;}
_LL336: {int sz=(int)Cyc_Typerep_size_type(rep);env=Cyc_Marshal_read_type_base((
void*)({struct Cyc_Typerep_Tuple_struct*_tmp183=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));
_tmp183[0]=({struct Cyc_Typerep_Tuple_struct _tmp184;_tmp184.tag=Cyc_Typerep_Tuple;
_tmp184.f1=(unsigned int)sz;_tmp184.f2=({unsigned int _tmp185=(unsigned int)sz;
struct _tuple0**_tmp186=(struct _tuple0**)_cycalloc(_check_times(sizeof(struct
_tuple0*),_tmp185));struct _tagged_arr _tmp18B=_tag_arr(_tmp186,sizeof(struct
_tuple0*),(unsigned int)sz);{unsigned int _tmp187=_tmp185;unsigned int i;for(i=0;i
< _tmp187;i ++){_tmp186[i]=({struct _tuple0*_tmp188=_cycalloc(sizeof(struct _tuple0));
_tmp188->f1=i;_tmp188->f2=(void*)({struct Cyc_Typerep_Int_struct*_tmp189=
_cycalloc_atomic(sizeof(struct Cyc_Typerep_Int_struct));_tmp189[0]=({struct Cyc_Typerep_Int_struct
_tmp18A;_tmp18A.tag=Cyc_Typerep_Int;_tmp18A.f1=8;_tmp18A;});_tmp189;});_tmp188;});}};
_tmp18B;});_tmp184;});_tmp183;}),env,fp,result);goto _LL318;}_LL338:(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp18C=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp18C[0]=({struct Cyc_Core_Failure_struct _tmp18D;_tmp18D.tag=Cyc_Core_Failure;
_tmp18D.f1=_tag_arr("Unhandled case in read_type_base",sizeof(unsigned char),33);
_tmp18D;});_tmp18C;}));_LL318:;}return env;}void*Cyc_Marshal_read_type(void*ts,
struct Cyc_Std___sFILE*fp){struct _handler_cons _tmp18E;_push_handler(& _tmp18E);{
int _tmp190=0;if(setjmp(_tmp18E.handler)){_tmp190=1;}if(! _tmp190){{struct
_tagged_arr buf=({unsigned int _tmp192=Cyc_Typerep_size_type(ts);unsigned char*
_tmp193=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),
_tmp192));struct _tagged_arr _tmp195=_tag_arr(_tmp193,sizeof(unsigned char),Cyc_Typerep_size_type(
ts));{unsigned int _tmp194=_tmp192;unsigned int i;for(i=0;i < _tmp194;i ++){_tmp193[
i]='\000';}};_tmp195;});void*result=((void*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(buf,sizeof(unsigned char),1)));Cyc_Marshal_read_type_base(
ts,Cyc_Marshal_empty_addr_table(),fp,result);{void*_tmp191=result;_npop_handler(
0);return _tmp191;}};_pop_handler();}else{void*_tmp18F=(void*)_exn_thrown;void*
_tmp197=_tmp18F;struct _tagged_arr _tmp19D;_LL409: if(*((void**)_tmp197)== Cyc_Core_Failure){
_LL414: _tmp19D=((struct Cyc_Core_Failure_struct*)_tmp197)->f1;goto _LL410;}else{
goto _LL411;}_LL411: goto _LL412;_LL410:({struct Cyc_Std_String_pa_struct _tmp1A0;
_tmp1A0.tag=Cyc_Std_String_pa;_tmp1A0.f1=(struct _tagged_arr)_tmp19D;{void*
_tmp19F[1]={& _tmp1A0};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",
sizeof(unsigned char),19),_tag_arr(_tmp19F,sizeof(void*),1));}});(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp1A1=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp1A1[0]=({struct Cyc_Core_Failure_struct _tmp1A2;_tmp1A2.tag=Cyc_Core_Failure;
_tmp1A2.f1=_tmp19D;_tmp1A2;});_tmp1A1;}));_LL412:(void)_throw(_tmp197);_LL408:;}}}
