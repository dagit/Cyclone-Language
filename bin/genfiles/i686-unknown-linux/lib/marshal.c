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
unsigned char Cyc_Set_Absent[11];struct Cyc_Typerep_Var_struct{int tag;struct
_tagged_arr*f1;};struct Cyc_Typerep_Int_struct{int tag;unsigned int f1;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple0{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{int tag;
struct _tagged_arr f1;};struct _tuple1{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Typerep_Forall_struct{int tag;struct _tagged_arr f1;void**
f2;};struct Cyc_Typerep_App_struct{int tag;void*f1;struct _tagged_arr f2;};extern
void*Cyc_Typerep_get_unionbranch(unsigned int tag,struct _tagged_arr l);extern void*
Cyc_Typerep_get_xtunionbranch(struct _tagged_arr tag,struct _tagged_arr l);
unsigned int Cyc_Typerep_size_type(void*rep);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern int Cyc_Std_fgetc(struct Cyc_Std___sFILE*
__stream);extern int Cyc_Std_fputc(int __c,struct Cyc_Std___sFILE*__stream);extern
unsigned int Cyc_Std_fread(struct _tagged_arr __ptr,unsigned int __size,unsigned int
__n,struct Cyc_Std___sFILE*__stream);extern unsigned int Cyc_Std_fwrite(struct
_tagged_arr __ptr,unsigned int __size,unsigned int __n,struct Cyc_Std___sFILE*__s);
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(struct _tagged_arr fmt,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern int isprint(int);
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
struct _tuple2 idx,unsigned int m){int _tmp2;struct _tuple2 _tmp1=idx;_LL1: _tmp2=_tmp1.f2;
goto _LL0;_LL0: return m < _tmp2;}struct _tuple2 Cyc_Marshal_empty_addr_index(){return({
struct _tuple2 _tmp3;_tmp3.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);_tmp3.f2=
1;_tmp3;});}int Cyc_Marshal_index_lookup(struct _tuple2 index,unsigned int val){int
_tmp5;struct Cyc_Dict_Dict*_tmp6;struct _tuple2 _tmp4=index;_LL4: _tmp6=_tmp4.f1;
goto _LL3;_LL3: _tmp5=_tmp4.f2;goto _LL2;_LL2: {struct Cyc_Core_Opt*_tmp7=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup_opt)(_tmp6,
val);if(_tmp7 != 0){return(int)_tmp7->v;}else{return _tmp5;}}}struct _tuple2 Cyc_Marshal_index_insert(
struct _tuple2 index,unsigned int val){int _tmp9;struct Cyc_Dict_Dict*_tmpA;struct
_tuple2 _tmp8=index;_LL7: _tmpA=_tmp8.f1;goto _LL6;_LL6: _tmp9=_tmp8.f2;goto _LL5;
_LL5: return({struct _tuple2 _tmpB;_tmpB.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,unsigned int k,int v))Cyc_Dict_insert)(_tmpA,val,_tmp9);_tmpB.f2=_tmp9 + 1;_tmpB;});}
struct _tuple3 Cyc_Marshal_empty_addr_table(){return({struct _tuple3 _tmpC;_tmpC.f1=
_tag_arr(({unsigned int _tmpD=(unsigned int)1;unsigned int*_tmpE=(unsigned int*)
_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmpD));{unsigned int _tmpF=
_tmpD;unsigned int i;for(i=0;i < _tmpF;i ++){_tmpE[i]=0;}};_tmpE;}),sizeof(
unsigned int),(unsigned int)1);_tmpC.f2=1;_tmpC;});}inline int Cyc_Marshal_table_member(
struct _tuple3 table,unsigned int m){int _tmp11;struct _tuple3 _tmp10=table;_LL9:
_tmp11=_tmp10.f2;goto _LL8;_LL8: return m < _tmp11;}struct _tuple3 Cyc_Marshal_table_insert(
struct _tuple3 tbl,unsigned int i){int _tmp13;struct _tagged_arr _tmp14;struct _tuple3
_tmp12=tbl;_LLC: _tmp14=_tmp12.f1;goto _LLB;_LLB: _tmp13=_tmp12.f2;goto _LLA;_LLA:
if(_get_arr_size(_tmp14,sizeof(unsigned int))== _tmp13){_tmp14=({unsigned int
_tmp15=(unsigned int)(2 * _tmp13);unsigned int*_tmp16=(unsigned int*)
_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmp15));struct _tagged_arr
_tmp18=_tag_arr(_tmp16,sizeof(unsigned int),(unsigned int)(2 * _tmp13));{
unsigned int _tmp17=_tmp15;unsigned int j;for(j=0;j < _tmp17;j ++){_tmp16[j]=j < 
_tmp13?*((unsigned int*)_check_unknown_subscript(_tmp14,sizeof(unsigned int),(
int)j)): 0;}};_tmp18;});}((unsigned int*)_tmp14.curr)[_tmp13]=i;return({struct
_tuple3 _tmp19;_tmp19.f1=_tmp14;_tmp19.f2=_tmp13 + 1;_tmp19;});}unsigned int Cyc_Marshal_table_lookup(
struct _tuple3 tbl,int i){return*((unsigned int*)_check_unknown_subscript(tbl.f1,
sizeof(unsigned int),i));}struct _tuple5{unsigned int f1;unsigned int f2;
unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_print_type_base(void*rep,struct
Cyc_Set_Set*env,void*val){{void*_tmp1A=rep;unsigned int _tmp1B;void*_tmp1C;
unsigned int _tmp1D;void*_tmp1E;struct _tagged_arr _tmp1F;struct _tagged_arr _tmp20;
struct _tagged_arr _tmp21;struct _tagged_arr _tmp22;_LLE: if((unsigned int)_tmp1A > 2?*((
int*)_tmp1A)== 1: 0){_LL22: _tmp1B=((struct Cyc_Typerep_Int_struct*)_tmp1A)->f1;
goto _LLF;}else{goto _LL10;}_LL10: if((int)_tmp1A == 0){goto _LL11;}else{goto _LL12;}
_LL12: if((int)_tmp1A == 1){goto _LL13;}else{goto _LL14;}_LL14: if((unsigned int)
_tmp1A > 2?*((int*)_tmp1A)== 2: 0){_LL24: _tmp1D=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1A)->f1;goto _LL23;_LL23: _tmp1C=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1A)->f2;goto _LL15;}else{goto _LL16;}_LL16: if((unsigned int)_tmp1A > 2?*((int*)
_tmp1A)== 3: 0){_LL25: _tmp1E=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp1A)->f1;
goto _LL17;}else{goto _LL18;}_LL18: if((unsigned int)_tmp1A > 2?*((int*)_tmp1A)== 4:
0){_LL26: _tmp1F=((struct Cyc_Typerep_Tuple_struct*)_tmp1A)->f2;goto _LL19;}else{
goto _LL1A;}_LL1A: if((unsigned int)_tmp1A > 2?*((int*)_tmp1A)== 5: 0){_LL27: _tmp20=((
struct Cyc_Typerep_TUnion_struct*)_tmp1A)->f1;goto _LL1B;}else{goto _LL1C;}_LL1C:
if((unsigned int)_tmp1A > 2?*((int*)_tmp1A)== 6: 0){_LL28: _tmp21=((struct Cyc_Typerep_XTUnion_struct*)
_tmp1A)->f1;goto _LL1D;}else{goto _LL1E;}_LL1E: if((unsigned int)_tmp1A > 2?*((int*)
_tmp1A)== 7: 0){_LL29: _tmp22=((struct Cyc_Typerep_Union_struct*)_tmp1A)->f1;goto
_LL1F;}else{goto _LL20;}_LL20: goto _LL21;_LLF: switch(_tmp1B){case 8: _LL2A: {
unsigned char*x=((unsigned char*(*)(void*x))unsafe_cast)(val);if(isprint((int)*x)){({
struct Cyc_Std_Int_pa_struct _tmp24;_tmp24.tag=1;_tmp24.f1=(int)((unsigned int)((
int)*x));{void*_tmp23[1]={& _tmp24};Cyc_Std_printf(_tag_arr("'%c'",sizeof(
unsigned char),5),_tag_arr(_tmp23,sizeof(void*),1));}});}else{({struct Cyc_Std_Int_pa_struct
_tmp26;_tmp26.tag=1;_tmp26.f1=(int)((unsigned int)((int)*x));{void*_tmp25[1]={&
_tmp26};Cyc_Std_printf(_tag_arr("'\\%d'",sizeof(unsigned char),6),_tag_arr(
_tmp25,sizeof(void*),1));}});}break;}case 16: _LL2B: {short*x=((short*(*)(void*x))
unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct _tmp28;_tmp28.tag=1;_tmp28.f1=(
int)((unsigned int)((int)*x));{void*_tmp27[1]={& _tmp28};Cyc_Std_printf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp27,sizeof(void*),1));}});break;}case 32:
_LL2C: {int*x=((int*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp2A;_tmp2A.tag=1;_tmp2A.f1=(int)((unsigned int)*x);{void*_tmp29[1]={& _tmp2A};
Cyc_Std_printf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp29,sizeof(
void*),1));}});break;}default: _LL2D:({struct Cyc_Std_Int_pa_struct _tmp2C;_tmp2C.tag=
1;_tmp2C.f1=(int)_tmp1B;{void*_tmp2B[1]={& _tmp2C};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("print_type_base: illegal int size %d bits\n",sizeof(unsigned char),43),
_tag_arr(_tmp2B,sizeof(void*),1));}});exit(1);}goto _LLD;_LL11: {float*x=((float*(*)(
void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct _tmp2E;_tmp2E.tag=2;
_tmp2E.f1=(double)*x;{void*_tmp2D[1]={& _tmp2E};Cyc_Std_printf(_tag_arr("%f",
sizeof(unsigned char),3),_tag_arr(_tmp2D,sizeof(void*),1));}});goto _LLD;}_LL13: {
double*x=((double*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp30;_tmp30.tag=2;_tmp30.f1=*x;{void*_tmp2F[1]={& _tmp30};Cyc_Std_printf(
_tag_arr("%f",sizeof(unsigned char),3),_tag_arr(_tmp2F,sizeof(void*),1));}});
goto _LLD;}_LL15: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));struct _tuple4 _tmp32;struct Cyc_Set_Set*_tmp33;int _tmp34;struct
_tuple4*_tmp31=Cyc_Marshal_member_insert(env,(unsigned int)y);_tmp32=*_tmp31;
_LL31: _tmp34=_tmp32.f1;goto _LL30;_LL30: _tmp33=_tmp32.f2;goto _LL2F;_LL2F: env=
_tmp33;if(! _tmp34){({struct Cyc_Std_Int_pa_struct _tmp36;_tmp36.tag=1;_tmp36.f1=(
unsigned int)y;{void*_tmp35[1]={& _tmp36};Cyc_Std_printf(_tag_arr("{(@%x) ",
sizeof(unsigned char),8),_tag_arr(_tmp35,sizeof(void*),1));}});if(y != 0){int
typ_szb=(int)Cyc_Typerep_size_type(_tmp1C);int i=0;for(0;i < _tmp1D;i ++){env=Cyc_Marshal_print_type_base(
_tmp1C,env,(void*)((void*(*)(int x))unsafe_cast)(y));if(i != _tmp1D - 1){({void*
_tmp37[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(
_tmp37,sizeof(void*),0));});}y +=typ_szb;}}else{({void*_tmp38[0]={};Cyc_Std_printf(
_tag_arr("NULL",sizeof(unsigned char),5),_tag_arr(_tmp38,sizeof(void*),0));});}({
void*_tmp39[0]={};Cyc_Std_printf(_tag_arr(" }",sizeof(unsigned char),3),_tag_arr(
_tmp39,sizeof(void*),0));});}else{({struct Cyc_Std_Int_pa_struct _tmp3B;_tmp3B.tag=
1;_tmp3B.f1=(unsigned int)y;{void*_tmp3A[1]={& _tmp3B};Cyc_Std_printf(_tag_arr("(#%x)",
sizeof(unsigned char),6),_tag_arr(_tmp3A,sizeof(void*),1));}});}goto _LLD;}_LL17: {
struct _tuple5*x=((struct _tuple5*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmp1E);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;struct _tuple4 _tmp3D;struct Cyc_Set_Set*
_tmp3E;int _tmp3F;struct _tuple4*_tmp3C=Cyc_Marshal_member_insert(env,base);_tmp3D=*
_tmp3C;_LL34: _tmp3F=_tmp3D.f1;goto _LL33;_LL33: _tmp3E=_tmp3D.f2;goto _LL32;_LL32:
env=_tmp3E;if(! _tmp3F){({struct Cyc_Std_Int_pa_struct _tmp41;_tmp41.tag=1;_tmp41.f1=
base;{void*_tmp40[1]={& _tmp41};Cyc_Std_printf(_tag_arr("[(@%x) ",sizeof(
unsigned char),8),_tag_arr(_tmp40,sizeof(void*),1));}});{unsigned int i=base;for(
0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_print_type_base(_tmp1E,env,(void*)((
void*(*)(unsigned int x))unsafe_cast)(i));if(i + typ_szb < last_plus_one){({void*
_tmp42[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(
_tmp42,sizeof(void*),0));});}}}({void*_tmp43[0]={};Cyc_Std_printf(_tag_arr(" ]",
sizeof(unsigned char),3),_tag_arr(_tmp43,sizeof(void*),0));});}else{({struct Cyc_Std_Int_pa_struct
_tmp45;_tmp45.tag=1;_tmp45.f1=base;{void*_tmp44[1]={& _tmp45};Cyc_Std_printf(
_tag_arr("(#%x)",sizeof(unsigned char),6),_tag_arr(_tmp44,sizeof(void*),1));}});}
goto _LLD;}_LL19:({void*_tmp46[0]={};Cyc_Std_printf(_tag_arr("$( ",sizeof(
unsigned char),4),_tag_arr(_tmp46,sizeof(void*),0));});{int x=((int(*)(void*x))
unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmp1F,sizeof(struct _tuple0*));
i ++){void*_tmp48;unsigned int _tmp49;struct _tuple0 _tmp47=*((struct _tuple0**)
_tmp1F.curr)[i];_LL37: _tmp49=_tmp47.f1;goto _LL36;_LL36: _tmp48=_tmp47.f2;goto
_LL35;_LL35: {int x_ofs=(int)(x + _tmp49);env=Cyc_Marshal_print_type_base(_tmp48,
env,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp1F,
sizeof(struct _tuple0*))- 1){({void*_tmp4A[0]={};Cyc_Std_printf(_tag_arr(", ",
sizeof(unsigned char),3),_tag_arr(_tmp4A,sizeof(void*),0));});}}}}({void*_tmp4B[
0]={};Cyc_Std_printf(_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(_tmp4B,
sizeof(void*),0));});goto _LLD;}_LL1B:({void*_tmp4C[0]={};Cyc_Std_printf(_tag_arr("&[( ",
sizeof(unsigned char),5),_tag_arr(_tmp4C,sizeof(void*),0));});{unsigned int*tagp=((
unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024){({struct Cyc_Std_Int_pa_struct
_tmp4E;_tmp4E.tag=1;_tmp4E.f1=(int)*tagp;{void*_tmp4D[1]={& _tmp4E};Cyc_Std_printf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp4D,sizeof(void*),1));}});}
else{struct _tuple4 _tmp50;struct Cyc_Set_Set*_tmp51;int _tmp52;struct _tuple4*_tmp4F=
Cyc_Marshal_member_insert(env,*tagp);_tmp50=*_tmp4F;_LL3A: _tmp52=_tmp50.f1;goto
_LL39;_LL39: _tmp51=_tmp50.f2;goto _LL38;_LL38: env=_tmp51;if(! _tmp52){({struct Cyc_Std_Int_pa_struct
_tmp54;_tmp54.tag=1;_tmp54.f1=*tagp;{void*_tmp53[1]={& _tmp54};Cyc_Std_printf(
_tag_arr("(@%x)",sizeof(unsigned char),6),_tag_arr(_tmp53,sizeof(void*),1));}});{
unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);
unsigned int tag=*x;void*typ=Cyc_Typerep_get_unionbranch(tag,_tmp20);env=((struct
Cyc_Set_Set*(*)(void*rep,struct Cyc_Set_Set*env,unsigned int*val))Cyc_Marshal_print_type_base)(
typ,env,x);}}else{({struct Cyc_Std_Int_pa_struct _tmp56;_tmp56.tag=1;_tmp56.f1=*
tagp;{void*_tmp55[1]={& _tmp56};Cyc_Std_printf(_tag_arr("(#%x)",sizeof(
unsigned char),6),_tag_arr(_tmp55,sizeof(void*),1));}});}}({void*_tmp57[0]={};
Cyc_Std_printf(_tag_arr(" )]",sizeof(unsigned char),4),_tag_arr(_tmp57,sizeof(
void*),0));});goto _LLD;}_LL1D: {unsigned int*xtunionp=((unsigned int*(*)(void*x))
unsafe_cast)(val);struct _tuple4 _tmp59;struct Cyc_Set_Set*_tmp5A;int _tmp5B;struct
_tuple4*_tmp58=Cyc_Marshal_member_insert(env,*xtunionp);_tmp59=*_tmp58;_LL3D:
_tmp5B=_tmp59.f1;goto _LL3C;_LL3C: _tmp5A=_tmp59.f2;goto _LL3B;_LL3B: env=_tmp5A;if(
! _tmp5B){({struct Cyc_Std_Int_pa_struct _tmp5D;_tmp5D.tag=1;_tmp5D.f1=*xtunionp;{
void*_tmp5C[1]={& _tmp5D};Cyc_Std_printf(_tag_arr("(@%x)",sizeof(unsigned char),6),
_tag_arr(_tmp5C,sizeof(void*),1));}});{unsigned int*xtstructp=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){unsigned char*xtnamec=((
unsigned char*(*)(unsigned int x))unsafe_cast)(*xtstructp + 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmp5F;_tmp5F.tag=0;_tmp5F.f1=(struct _tagged_arr)xtname;{void*_tmp5E[1]={& _tmp5F};
Cyc_Std_printf(_tag_arr("&%s[( ",sizeof(unsigned char),7),_tag_arr(_tmp5E,
sizeof(void*),1));}});{void*typ=Cyc_Typerep_get_xtunionbranch(xtname,_tmp21);env=((
struct Cyc_Set_Set*(*)(void*rep,struct Cyc_Set_Set*env,unsigned int*val))Cyc_Marshal_print_type_base)(
typ,env,(unsigned int*)_check_null(xtstructp));}}else{unsigned char*xtnamec=((
unsigned char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmp61;_tmp61.tag=0;_tmp61.f1=(struct _tagged_arr)xtname;{void*_tmp60[1]={& _tmp61};
Cyc_Std_printf(_tag_arr("&%s[( ",sizeof(unsigned char),7),_tag_arr(_tmp60,
sizeof(void*),1));}});}}}else{({struct Cyc_Std_Int_pa_struct _tmp63;_tmp63.tag=1;
_tmp63.f1=*xtunionp;{void*_tmp62[1]={& _tmp63};Cyc_Std_printf(_tag_arr("(#%x)",
sizeof(unsigned char),6),_tag_arr(_tmp62,sizeof(void*),1));}});}({void*_tmp64[0]={};
Cyc_Std_printf(_tag_arr(" )]",sizeof(unsigned char),4),_tag_arr(_tmp64,sizeof(
void*),0));});goto _LLD;}_LL1F:({void*_tmp65[0]={};Cyc_Std_printf(_tag_arr("[[",
sizeof(unsigned char),3),_tag_arr(_tmp65,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp22,sizeof(void**));i ++){Cyc_Marshal_print_type_base(*((void***)
_tmp22.curr)[i],env,val);if(i < _get_arr_size(_tmp22,sizeof(void**))- 1){({void*
_tmp66[0]={};Cyc_Std_printf(_tag_arr(" | ",sizeof(unsigned char),4),_tag_arr(
_tmp66,sizeof(void*),0));});}}}({void*_tmp67[0]={};Cyc_Std_printf(_tag_arr("]]",
sizeof(unsigned char),3),_tag_arr(_tmp67,sizeof(void*),0));});goto _LLD;_LL21:(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp68=_cycalloc(sizeof(struct
Cyc_Core_Failure_struct));_tmp68[0]=({struct Cyc_Core_Failure_struct _tmp69;_tmp69.tag=
Cyc_Core_Failure;_tmp69.f1=_tag_arr("Unhandled case in print_type_base",sizeof(
unsigned char),34);_tmp69;});_tmp68;}));_LLD:;}return env;}void Cyc_Marshal_print_type(
void*ts,void*val){struct _handler_cons _tmp6A;_push_handler(& _tmp6A);{int _tmp6C=0;
if(setjmp(_tmp6A.handler)){_tmp6C=1;}if(! _tmp6C){Cyc_Marshal_print_type_base(ts,
Cyc_Marshal_empty_addr_set(),val);;_pop_handler();}else{void*_tmp6B=(void*)
_exn_thrown;void*_tmp6E=_tmp6B;struct _tagged_arr _tmp6F;_LL3F: if(*((void**)_tmp6E)
== Cyc_Core_Failure){_LL43: _tmp6F=((struct Cyc_Core_Failure_struct*)_tmp6E)->f1;
goto _LL40;}else{goto _LL41;}_LL41: goto _LL42;_LL40:({struct Cyc_Std_String_pa_struct
_tmp71;_tmp71.tag=0;_tmp71.f1=(struct _tagged_arr)_tmp6F;{void*_tmp70[1]={& _tmp71};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(
unsigned char),19),_tag_arr(_tmp70,sizeof(void*),1));}});(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp72=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp72[0]=({struct Cyc_Core_Failure_struct _tmp73;_tmp73.tag=Cyc_Core_Failure;
_tmp73.f1=_tmp6F;_tmp73;});_tmp72;}));_LL42:(void)_throw(_tmp6E);_LL3E:;}}}void
Cyc_Marshal_cycputchar(unsigned char x,struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(unsigned char),1)),
sizeof(unsigned char),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp74=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp74[0]=({struct Cyc_Core_Failure_struct
_tmp75;_tmp75.tag=Cyc_Core_Failure;_tmp75.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmp75;});_tmp74;}));}}void Cyc_Marshal_cycputshort(short x,
struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(short),1)),sizeof(short),1,fp)< 1){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp76=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp76[0]=({struct Cyc_Core_Failure_struct _tmp77;_tmp77.tag=Cyc_Core_Failure;
_tmp77.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmp77;});_tmp76;}));}}
void Cyc_Marshal_cycputint(int x,struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(int),1)),sizeof(int),1,
fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp78=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmp78[0]=({struct Cyc_Core_Failure_struct
_tmp79;_tmp79.tag=Cyc_Core_Failure;_tmp79.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmp79;});_tmp78;}));}}void Cyc_Marshal_cycputfloat(float x,
struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(float),1)),sizeof(float),1,fp)< 1){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp7A=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp7A[0]=({struct Cyc_Core_Failure_struct _tmp7B;_tmp7B.tag=Cyc_Core_Failure;
_tmp7B.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmp7B;});_tmp7A;}));}}
void Cyc_Marshal_cycputdouble(double x,struct Cyc_Std___sFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(double),1)),sizeof(
double),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp7C=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp7C[0]=({struct Cyc_Core_Failure_struct
_tmp7D;_tmp7D.tag=Cyc_Core_Failure;_tmp7D.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmp7D;});_tmp7C;}));}}void Cyc_Marshal_cycputvarint(
unsigned int i,struct Cyc_Std___sFILE*fp){int width=0;unsigned int mask=-256;while((
int)mask){if((int)(i & mask)){width ++;}mask <<=8;}Cyc_Std_fputc((int)(width << 6 | i
& 63),fp);i >>=6;{int j=0;for(0;j < width;j ++){Cyc_Std_fputc((int)(i & 255),fp);i >>=
8;}}}unsigned char Cyc_Marshal_cycgetchar(struct Cyc_Std___sFILE*fp){struct
_RegionHandle _tmp7E=_new_region("r");struct _RegionHandle*r=& _tmp7E;_push_region(
r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmp82=sizeof(unsigned char);
unsigned char*_tmp83=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmp82));{unsigned int _tmp84=_tmp82;unsigned int i;for(i=0;i < 
_tmp84;i ++){_tmp83[i]='\000';}};_tmp83;}),sizeof(unsigned char),sizeof(
unsigned char));if(Cyc_Std_fread(x,sizeof(unsigned char),1,fp)< 1){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp7F=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp7F[0]=({struct Cyc_Core_Failure_struct _tmp80;_tmp80.tag=Cyc_Core_Failure;
_tmp80.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmp80;});_tmp7F;}));}{
unsigned char*xx=((unsigned char*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));unsigned char
_tmp81=*xx;_npop_handler(0);return _tmp81;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc_Std___sFILE*fp){struct _RegionHandle _tmp85=_new_region("r");struct
_RegionHandle*r=& _tmp85;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmp89=sizeof(short);unsigned char*_tmp8A=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmp89));{unsigned int _tmp8B=
_tmp89;unsigned int i;for(i=0;i < _tmp8B;i ++){_tmp8A[i]='\000';}};_tmp8A;}),
sizeof(unsigned char),sizeof(short));if(Cyc_Std_fread(x,sizeof(short),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp86=_cycalloc(sizeof(struct
Cyc_Core_Failure_struct));_tmp86[0]=({struct Cyc_Core_Failure_struct _tmp87;_tmp87.tag=
Cyc_Core_Failure;_tmp87.f1=_tag_arr("Read failure",sizeof(unsigned char),13);
_tmp87;});_tmp86;}));}{short*xx=((short*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));short _tmp88=*
xx;_npop_handler(0);return _tmp88;}};_pop_region(r);}int Cyc_Marshal_cycgetint(
struct Cyc_Std___sFILE*fp){struct _RegionHandle _tmp8C=_new_region("r");struct
_RegionHandle*r=& _tmp8C;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmp90=sizeof(int);unsigned char*_tmp91=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmp90));{unsigned int _tmp92=
_tmp90;unsigned int i;for(i=0;i < _tmp92;i ++){_tmp91[i]='\000';}};_tmp91;}),
sizeof(unsigned char),sizeof(int));if(Cyc_Std_fread(x,sizeof(int),1,fp)< 1){(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp8D=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp8D[0]=({struct Cyc_Core_Failure_struct _tmp8E;_tmp8E.tag=Cyc_Core_Failure;
_tmp8E.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmp8E;});_tmp8D;}));}{
int*xx=((int*(*)(unsigned char*x))unsafe_cast)((unsigned char*)_check_null(
_untag_arr(x,sizeof(unsigned char),1)));int _tmp8F=*xx;_npop_handler(0);return
_tmp8F;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(struct Cyc_Std___sFILE*fp){
struct _RegionHandle _tmp93=_new_region("r");struct _RegionHandle*r=& _tmp93;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmp97=sizeof(float);
unsigned char*_tmp98=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmp97));{unsigned int _tmp99=_tmp97;unsigned int i;for(i=0;i < 
_tmp99;i ++){_tmp98[i]='\000';}};_tmp98;}),sizeof(unsigned char),sizeof(float));
if(Cyc_Std_fread(x,sizeof(float),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp94=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp94[0]=({struct Cyc_Core_Failure_struct
_tmp95;_tmp95.tag=Cyc_Core_Failure;_tmp95.f1=_tag_arr("Read failure",sizeof(
unsigned char),13);_tmp95;});_tmp94;}));}{float*xx=((float*(*)(unsigned char*x))
unsafe_cast)((unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));
float _tmp96=*xx;_npop_handler(0);return _tmp96;}};_pop_region(r);}double Cyc_Marshal_cycgetdouble(
struct Cyc_Std___sFILE*fp){struct _RegionHandle _tmp9A=_new_region("r");struct
_RegionHandle*r=& _tmp9A;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmp9E=sizeof(double);unsigned char*_tmp9F=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmp9E));{unsigned int _tmpA0=
_tmp9E;unsigned int i;for(i=0;i < _tmpA0;i ++){_tmp9F[i]='\000';}};_tmp9F;}),
sizeof(unsigned char),sizeof(double));if(Cyc_Std_fread(x,sizeof(double),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp9B=_cycalloc(sizeof(struct
Cyc_Core_Failure_struct));_tmp9B[0]=({struct Cyc_Core_Failure_struct _tmp9C;_tmp9C.tag=
Cyc_Core_Failure;_tmp9C.f1=_tag_arr("Read failure",sizeof(unsigned char),13);
_tmp9C;});_tmp9B;}));}{double*xx=((double*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));double _tmp9D=*
xx;_npop_handler(0);return _tmp9D;}};_pop_region(r);}unsigned int Cyc_Marshal_cycgetvarint(
struct Cyc_Std___sFILE*fp){int i=0;int j=(int)Cyc_Marshal_cycgetchar(fp);int width=(
j & 192)>> 6;int pos=6;i +=j & 63;{int k=0;for(0;k < width;k ++){j=(int)Cyc_Marshal_cycgetchar(
fp);i |=j << pos;pos +=8;}}return(unsigned int)i;}struct _tuple2 Cyc_Marshal_write_type_base(
void*rep,struct _tuple2 env,struct Cyc_Std___sFILE*fp,void*val){{void*_tmpA1=rep;
unsigned int _tmpA2;void*_tmpA3;unsigned int _tmpA4;void*_tmpA5;struct _tagged_arr
_tmpA6;struct _tagged_arr _tmpA7;struct _tagged_arr _tmpA8;struct _tagged_arr _tmpA9;
_LL45: if((unsigned int)_tmpA1 > 2?*((int*)_tmpA1)== 1: 0){_LL59: _tmpA2=((struct Cyc_Typerep_Int_struct*)
_tmpA1)->f1;goto _LL46;}else{goto _LL47;}_LL47: if((int)_tmpA1 == 0){goto _LL48;}
else{goto _LL49;}_LL49: if((int)_tmpA1 == 1){goto _LL4A;}else{goto _LL4B;}_LL4B: if((
unsigned int)_tmpA1 > 2?*((int*)_tmpA1)== 2: 0){_LL5B: _tmpA4=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpA1)->f1;goto _LL5A;_LL5A: _tmpA3=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmpA1)->f2;goto _LL4C;}else{goto _LL4D;}_LL4D: if((unsigned int)_tmpA1 > 2?*((int*)
_tmpA1)== 3: 0){_LL5C: _tmpA5=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpA1)->f1;
goto _LL4E;}else{goto _LL4F;}_LL4F: if((unsigned int)_tmpA1 > 2?*((int*)_tmpA1)== 4:
0){_LL5D: _tmpA6=((struct Cyc_Typerep_Tuple_struct*)_tmpA1)->f2;goto _LL50;}else{
goto _LL51;}_LL51: if((unsigned int)_tmpA1 > 2?*((int*)_tmpA1)== 5: 0){_LL5E: _tmpA7=((
struct Cyc_Typerep_TUnion_struct*)_tmpA1)->f1;goto _LL52;}else{goto _LL53;}_LL53:
if((unsigned int)_tmpA1 > 2?*((int*)_tmpA1)== 6: 0){_LL5F: _tmpA8=((struct Cyc_Typerep_XTUnion_struct*)
_tmpA1)->f1;goto _LL54;}else{goto _LL55;}_LL55: if((unsigned int)_tmpA1 > 2?*((int*)
_tmpA1)== 7: 0){_LL60: _tmpA9=((struct Cyc_Typerep_Union_struct*)_tmpA1)->f1;goto
_LL56;}else{goto _LL57;}_LL57: goto _LL58;_LL46: switch(_tmpA2){case 8: _LL61: {
unsigned char*x=((unsigned char*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputchar(*
x,fp);break;}case 16: _LL62: {short*x=((short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*
x,fp);break;}case 32: _LL63: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*
x,fp);break;}default: _LL64:({struct Cyc_Std_Int_pa_struct _tmpAB;_tmpAB.tag=1;
_tmpAB.f1=(int)_tmpA2;{void*_tmpAA[1]={& _tmpAB};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("write_type_base: illegal int size %d bits\n",sizeof(unsigned char),43),
_tag_arr(_tmpAA,sizeof(void*),1));}});exit(1);}goto _LL44;_LL48: {float*x=((float*(*)(
void*x))unsafe_cast)(val);Cyc_Marshal_cycputfloat(*x,fp);goto _LL44;}_LL4A: {
double*x=((double*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*x,fp);
goto _LL44;}_LL4C: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));int _tmpAC=Cyc_Marshal_index_lookup(env,(unsigned int)y);Cyc_Marshal_cycputvarint((
unsigned int)_tmpAC,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmpAC)){
env=Cyc_Marshal_index_insert(env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmpA3);int i=0;for(0;i < _tmpA4;i ++){env=Cyc_Marshal_write_type_base(_tmpA3,env,
fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}goto _LL44;}_LL4E: {
struct _tuple5*x=((struct _tuple5*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmpA5);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;unsigned int pos=(curr - base)/ typ_szb;
unsigned int sz=(last_plus_one - base)/ typ_szb;int _tmpAD=Cyc_Marshal_index_lookup(
env,base);Cyc_Marshal_cycputvarint((unsigned int)_tmpAD,fp);Cyc_Marshal_cycputvarint(
pos,fp);Cyc_Marshal_cycputvarint(sz,fp);if(! Cyc_Marshal_index_member(env,(
unsigned int)_tmpAD)){env=Cyc_Marshal_index_insert(env,base);{unsigned int i=base;
for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(_tmpA5,env,fp,(
void*)((void*(*)(unsigned int x))unsafe_cast)(i));}}}goto _LL44;}_LL50: {int x=((
int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpA6,sizeof(
struct _tuple0*));i ++){void*_tmpAF;unsigned int _tmpB0;struct _tuple0 _tmpAE=*((
struct _tuple0**)_tmpA6.curr)[i];_LL68: _tmpB0=_tmpAE.f1;goto _LL67;_LL67: _tmpAF=
_tmpAE.f2;goto _LL66;_LL66: {int x_ofs=(int)(x + _tmpB0);env=Cyc_Marshal_write_type_base(
_tmpAF,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL44;}_LL52: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024){
Cyc_Marshal_cycputvarint(*tagp,fp);}else{int _tmpB1=Cyc_Marshal_index_lookup(env,*
tagp);Cyc_Marshal_cycputvarint((unsigned int)(_tmpB1 + 1024),fp);if(! Cyc_Marshal_index_member(
env,(unsigned int)_tmpB1)){env=Cyc_Marshal_index_insert(env,*tagp);{unsigned int*
x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;Cyc_Marshal_cycputvarint(
tag,fp);{void*typ=Cyc_Typerep_get_unionbranch(tag,_tmpA7);env=((struct _tuple2(*)(
void*rep,struct _tuple2 env,struct Cyc_Std___sFILE*fp,unsigned int*val))Cyc_Marshal_write_type_base)(
typ,env,fp,x);}}}}goto _LL44;}_LL54: {unsigned int*xtunionp=((unsigned int*(*)(
void*x))unsafe_cast)(val);int _tmpB2=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint((
unsigned int)_tmpB2,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmpB2)){
env=Cyc_Marshal_index_insert(env,*xtunionp);{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',
fp);{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtstructp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmpB4;_tmpB4.tag=0;_tmpB4.f1=(struct
_tagged_arr)xtname;{void*_tmpB3[1]={& _tmpB4};Cyc_Std_fprintf(fp,_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpB3,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',
fp);{void*typ=Cyc_Typerep_get_xtunionbranch(xtname,_tmpA8);env=((struct _tuple2(*)(
void*rep,struct _tuple2 env,struct Cyc_Std___sFILE*fp,unsigned int*val))Cyc_Marshal_write_type_base)(
typ,env,fp,(unsigned int*)_check_null(xtstructp));}}}else{Cyc_Marshal_cycputchar('\000',
fp);{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtunionp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct
_tagged_arr)xtname;{void*_tmpB5[1]={& _tmpB6};Cyc_Std_fprintf(fp,_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpB5,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',
fp);}}}}goto _LL44;}_LL56: {int sz=(int)Cyc_Typerep_size_type(rep);env=Cyc_Marshal_write_type_base((
void*)({struct Cyc_Typerep_Tuple_struct*_tmpB7=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));
_tmpB7[0]=({struct Cyc_Typerep_Tuple_struct _tmpB8;_tmpB8.tag=4;_tmpB8.f1=(
unsigned int)sz;_tmpB8.f2=({unsigned int _tmpB9=(unsigned int)sz;struct _tuple0**
_tmpBA=(struct _tuple0**)_cycalloc(_check_times(sizeof(struct _tuple0*),_tmpB9));
struct _tagged_arr _tmpBF=_tag_arr(_tmpBA,sizeof(struct _tuple0*),(unsigned int)sz);{
unsigned int _tmpBB=_tmpB9;unsigned int i;for(i=0;i < _tmpBB;i ++){_tmpBA[i]=({
struct _tuple0*_tmpBC=_cycalloc(sizeof(struct _tuple0));_tmpBC->f1=i;_tmpBC->f2=(
void*)({struct Cyc_Typerep_Int_struct*_tmpBD=_cycalloc_atomic(sizeof(struct Cyc_Typerep_Int_struct));
_tmpBD[0]=({struct Cyc_Typerep_Int_struct _tmpBE;_tmpBE.tag=1;_tmpBE.f1=8;_tmpBE;});
_tmpBD;});_tmpBC;});}};_tmpBF;});_tmpB8;});_tmpB7;}),env,fp,val);goto _LL44;}
_LL58:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpC0=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmpC0[0]=({struct Cyc_Core_Failure_struct
_tmpC1;_tmpC1.tag=Cyc_Core_Failure;_tmpC1.f1=_tag_arr("Unhandled case in write_type_base",
sizeof(unsigned char),34);_tmpC1;});_tmpC0;}));_LL44:;}return env;}void Cyc_Marshal_write_type(
void*ts,struct Cyc_Std___sFILE*fp,void*val){struct _handler_cons _tmpC2;
_push_handler(& _tmpC2);{int _tmpC4=0;if(setjmp(_tmpC2.handler)){_tmpC4=1;}if(!
_tmpC4){Cyc_Marshal_write_type_base(ts,Cyc_Marshal_empty_addr_index(),fp,val);;
_pop_handler();}else{void*_tmpC3=(void*)_exn_thrown;void*_tmpC6=_tmpC3;struct
_tagged_arr _tmpC7;_LL6A: if(*((void**)_tmpC6)== Cyc_Core_Failure){_LL6E: _tmpC7=((
struct Cyc_Core_Failure_struct*)_tmpC6)->f1;goto _LL6B;}else{goto _LL6C;}_LL6C: goto
_LL6D;_LL6B:({struct Cyc_Std_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(
struct _tagged_arr)_tmpC7;{void*_tmpC8[1]={& _tmpC9};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(_tmpC8,sizeof(
void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpCA=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmpCA[0]=({struct Cyc_Core_Failure_struct
_tmpCB;_tmpCB.tag=Cyc_Core_Failure;_tmpCB.f1=_tmpC7;_tmpCB;});_tmpCA;}));_LL6D:(
void)_throw(_tmpC6);_LL69:;}}}void Cyc_Marshal_fscanntstring(struct Cyc_Std___sFILE*
fp,struct _tagged_arr s){int c;int i=0;while(c=Cyc_Std_fgetc(fp)){if(c == - 1){(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpCC=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmpCC[0]=({struct Cyc_Core_Failure_struct _tmpCD;_tmpCD.tag=Cyc_Core_Failure;
_tmpCD.f1=_tag_arr("Read error",sizeof(unsigned char),11);_tmpCD;});_tmpCC;}));}((
unsigned char*)s.curr)[i]=(unsigned char)c;i ++;}((unsigned char*)s.curr)[i]='\000';}
struct _tuple3 Cyc_Marshal_read_type_base(void*rep,struct _tuple3 env,struct Cyc_Std___sFILE*
fp,void*result){{void*_tmpCE=rep;unsigned int _tmpCF;void*_tmpD0;unsigned int
_tmpD1;void*_tmpD2;struct _tagged_arr _tmpD3;struct _tagged_arr _tmpD4;struct
_tagged_arr _tmpD5;struct _tagged_arr _tmpD6;_LL70: if((unsigned int)_tmpCE > 2?*((
int*)_tmpCE)== 1: 0){_LL84: _tmpCF=((struct Cyc_Typerep_Int_struct*)_tmpCE)->f1;
goto _LL71;}else{goto _LL72;}_LL72: if((int)_tmpCE == 0){goto _LL73;}else{goto _LL74;}
_LL74: if((int)_tmpCE == 1){goto _LL75;}else{goto _LL76;}_LL76: if((unsigned int)
_tmpCE > 2?*((int*)_tmpCE)== 2: 0){_LL86: _tmpD1=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpCE)->f1;goto _LL85;_LL85: _tmpD0=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmpCE)->f2;goto _LL77;}else{goto _LL78;}_LL78: if((unsigned int)_tmpCE > 2?*((int*)
_tmpCE)== 3: 0){_LL87: _tmpD2=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpCE)->f1;
goto _LL79;}else{goto _LL7A;}_LL7A: if((unsigned int)_tmpCE > 2?*((int*)_tmpCE)== 4:
0){_LL88: _tmpD3=((struct Cyc_Typerep_Tuple_struct*)_tmpCE)->f2;goto _LL7B;}else{
goto _LL7C;}_LL7C: if((unsigned int)_tmpCE > 2?*((int*)_tmpCE)== 5: 0){_LL89: _tmpD4=((
struct Cyc_Typerep_TUnion_struct*)_tmpCE)->f1;goto _LL7D;}else{goto _LL7E;}_LL7E:
if((unsigned int)_tmpCE > 2?*((int*)_tmpCE)== 6: 0){_LL8A: _tmpD5=((struct Cyc_Typerep_XTUnion_struct*)
_tmpCE)->f1;goto _LL7F;}else{goto _LL80;}_LL80: if((unsigned int)_tmpCE > 2?*((int*)
_tmpCE)== 7: 0){_LL8B: _tmpD6=((struct Cyc_Typerep_Union_struct*)_tmpCE)->f1;goto
_LL81;}else{goto _LL82;}_LL82: goto _LL83;_LL71: switch(_tmpCF){case 8: _LL8C: {
unsigned char*x=((unsigned char*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetchar(
fp);break;}case 16: _LL8D: {short*x=((short*(*)(void*x))unsafe_cast)(result);*x=
Cyc_Marshal_cycgetshort(fp);break;}case 32: _LL8E: {int*x=((int*(*)(void*x))
unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(fp);break;}default: _LL8F:(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpD7=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmpD7[0]=({struct Cyc_Core_Failure_struct _tmpD8;_tmpD8.tag=Cyc_Core_Failure;
_tmpD8.f1=_tag_arr("read_type_base: illegal int size bits",sizeof(unsigned char),
38);_tmpD8;});_tmpD7;}));}goto _LL6F;_LL73: {float*y=((float*(*)(void*x))
unsafe_cast)(result);*y=Cyc_Marshal_cycgetfloat(fp);goto _LL6F;}_LL75: {double*y=((
double*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(fp);goto
_LL6F;}_LL77: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmpD0);struct
_tagged_arr buf=({unsigned int _tmpD9=_tmpD1 * typ_szb;unsigned char*_tmpDA=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmpD9));
struct _tagged_arr _tmpDC=_tag_arr(_tmpDA,sizeof(unsigned char),_tmpD1 * typ_szb);{
unsigned int _tmpDB=_tmpD9;unsigned int i;for(i=0;i < _tmpDB;i ++){_tmpDA[i]='\000';}};
_tmpDC;});unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(
_untag_arr(buf,sizeof(unsigned char),1)));unsigned int*x=((unsigned int*(*)(void*
x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(
int)bufptr;int i=0;for(0;i < _tmpD1;i ++){env=Cyc_Marshal_read_type_base(_tmpD0,env,
fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,
mem);}goto _LL6F;}_LL79: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);int pos=(int)
Cyc_Marshal_cycgetvarint(fp);int sz=(int)Cyc_Marshal_cycgetvarint(fp);int typ_szb=(
int)Cyc_Typerep_size_type(_tmpD2);if(! Cyc_Marshal_table_member(env,(unsigned int)
mem)){struct _tagged_arr buf=({unsigned int _tmpDD=(unsigned int)(sz * typ_szb);
unsigned char*_tmpDE=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmpDD));struct _tagged_arr _tmpE0=_tag_arr(_tmpDE,sizeof(
unsigned char),(unsigned int)(sz * typ_szb));{unsigned int _tmpDF=_tmpDD;
unsigned int i;for(i=0;i < _tmpDF;i ++){_tmpDE[i]='\000';}};_tmpE0;});unsigned int
bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,sizeof(
unsigned char),1)));struct _tuple5*x=((struct _tuple5*(*)(void*x))unsafe_cast)(
result);(*x).f1=bufptr;(*x).f2=bufptr + pos * typ_szb;(*x).f3=bufptr + sz * typ_szb;
env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int i=0;for(0;i < sz;i
++){env=Cyc_Marshal_read_type_base(_tmpD2,env,fp,(void*)((void*(*)(int x))
unsafe_cast)(y));y +=typ_szb;}}}else{struct _tuple5*x=((struct _tuple5*(*)(void*x))
unsafe_cast)(result);(*x).f1=Cyc_Marshal_table_lookup(env,mem);(*x).f2=(*x).f1 + 
typ_szb * pos;(*x).f3=(*x).f1 + typ_szb * sz;}goto _LL6F;}_LL7B: {unsigned int x=((
unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(
_tmpD3,sizeof(struct _tuple0*));i ++){void*_tmpE2;unsigned int _tmpE3;struct _tuple0
_tmpE1=*((struct _tuple0**)_tmpD3.curr)[i];_LL93: _tmpE3=_tmpE1.f1;goto _LL92;_LL92:
_tmpE2=_tmpE1.f2;goto _LL91;_LL91: {int x_ofs=(int)(x + _tmpE3);env=Cyc_Marshal_read_type_base(
_tmpE2,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL6F;}_LL7D: {
int tagmem=(int)Cyc_Marshal_cycgetvarint(fp);if(tagmem < 1024){int tag=tagmem;
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(unsigned int)
tag;}else{int mem=tagmem - 1024;if(! Cyc_Marshal_table_member(env,(unsigned int)mem)){
int tag=(int)Cyc_Marshal_cycgetvarint(fp);void*typ=Cyc_Typerep_get_unionbranch((
unsigned int)tag,_tmpD4);struct _tagged_arr buf=({unsigned int _tmpE4=Cyc_Typerep_size_type(
typ);unsigned char*_tmpE5=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmpE4));struct _tagged_arr _tmpE7=_tag_arr(_tmpE5,sizeof(
unsigned char),Cyc_Typerep_size_type(typ));{unsigned int _tmpE6=_tmpE4;
unsigned int i;for(i=0;i < _tmpE6;i ++){_tmpE5[i]='\000';}};_tmpE7;});unsigned int
bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,sizeof(
unsigned char),1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_read_type_base(
typ,env,fp,(void*)((void*(*)(unsigned int x))unsafe_cast)(bufptr));}}else{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(
env,mem);}}goto _LL6F;}_LL7F: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){unsigned char nonnull=(unsigned char)Cyc_Marshal_cycgetchar(
fp);struct _tagged_arr s=_tag_arr(({unsigned int _tmpEC=(unsigned int)100;
unsigned char*_tmpED=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmpEC));{unsigned int _tmpEE=_tmpEC;unsigned int i;for(i=0;i < 
_tmpEE;i ++){_tmpED[i]='\000';}};_tmpED;}),sizeof(unsigned char),(unsigned int)
100);struct _tagged_arr xtname=_tagged_arr_plus(s,sizeof(unsigned char),4);Cyc_Marshal_fscanntstring(
fp,xtname);if((int)nonnull){void*typ=Cyc_Typerep_get_xtunionbranch((struct
_tagged_arr)xtname,_tmpD5);struct _tagged_arr buf=({unsigned int _tmpE8=Cyc_Typerep_size_type(
typ);unsigned char*_tmpE9=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmpE8));struct _tagged_arr _tmpEB=_tag_arr(_tmpE9,sizeof(
unsigned char),Cyc_Typerep_size_type(typ));{unsigned int _tmpEA=_tmpE8;
unsigned int i;for(i=0;i < _tmpEA;i ++){_tmpE9[i]='\000';}};_tmpEB;});unsigned int
bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,sizeof(
unsigned char),1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_read_type_base(
typ,env,fp,(void*)((void*(*)(unsigned int x))unsafe_cast)(bufptr));{unsigned int*
tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*tagptr=(
unsigned int)((unsigned char*)_check_null(_untag_arr(s,sizeof(unsigned char),1)));}}}
else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(
unsigned int)((unsigned char*)_check_null(_untag_arr(s,sizeof(unsigned char),1)));
env=Cyc_Marshal_table_insert(env,*x);}}else{unsigned int*x=((unsigned int*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL6F;}
_LL81: {int sz=(int)Cyc_Typerep_size_type(rep);env=Cyc_Marshal_read_type_base((
void*)({struct Cyc_Typerep_Tuple_struct*_tmpEF=_cycalloc(sizeof(struct Cyc_Typerep_Tuple_struct));
_tmpEF[0]=({struct Cyc_Typerep_Tuple_struct _tmpF0;_tmpF0.tag=4;_tmpF0.f1=(
unsigned int)sz;_tmpF0.f2=({unsigned int _tmpF1=(unsigned int)sz;struct _tuple0**
_tmpF2=(struct _tuple0**)_cycalloc(_check_times(sizeof(struct _tuple0*),_tmpF1));
struct _tagged_arr _tmpF7=_tag_arr(_tmpF2,sizeof(struct _tuple0*),(unsigned int)sz);{
unsigned int _tmpF3=_tmpF1;unsigned int i;for(i=0;i < _tmpF3;i ++){_tmpF2[i]=({
struct _tuple0*_tmpF4=_cycalloc(sizeof(struct _tuple0));_tmpF4->f1=i;_tmpF4->f2=(
void*)({struct Cyc_Typerep_Int_struct*_tmpF5=_cycalloc_atomic(sizeof(struct Cyc_Typerep_Int_struct));
_tmpF5[0]=({struct Cyc_Typerep_Int_struct _tmpF6;_tmpF6.tag=1;_tmpF6.f1=8;_tmpF6;});
_tmpF5;});_tmpF4;});}};_tmpF7;});_tmpF0;});_tmpEF;}),env,fp,result);goto _LL6F;}
_LL83:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpF8=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmpF8[0]=({struct Cyc_Core_Failure_struct
_tmpF9;_tmpF9.tag=Cyc_Core_Failure;_tmpF9.f1=_tag_arr("Unhandled case in read_type_base",
sizeof(unsigned char),33);_tmpF9;});_tmpF8;}));_LL6F:;}return env;}void*Cyc_Marshal_read_type(
void*ts,struct Cyc_Std___sFILE*fp){struct _handler_cons _tmpFA;_push_handler(&
_tmpFA);{int _tmpFC=0;if(setjmp(_tmpFA.handler)){_tmpFC=1;}if(! _tmpFC){{struct
_tagged_arr buf=({unsigned int _tmpFE=Cyc_Typerep_size_type(ts);unsigned char*
_tmpFF=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),
_tmpFE));struct _tagged_arr _tmp101=_tag_arr(_tmpFF,sizeof(unsigned char),Cyc_Typerep_size_type(
ts));{unsigned int _tmp100=_tmpFE;unsigned int i;for(i=0;i < _tmp100;i ++){_tmpFF[i]='\000';}};
_tmp101;});void*result=((void*(*)(unsigned char*x))unsafe_cast)((unsigned char*)
_check_null(_untag_arr(buf,sizeof(unsigned char),1)));Cyc_Marshal_read_type_base(
ts,Cyc_Marshal_empty_addr_table(),fp,result);{void*_tmpFD=result;_npop_handler(0);
return _tmpFD;}};_pop_handler();}else{void*_tmpFB=(void*)_exn_thrown;void*_tmp103=
_tmpFB;struct _tagged_arr _tmp104;_LL95: if(*((void**)_tmp103)== Cyc_Core_Failure){
_LL99: _tmp104=((struct Cyc_Core_Failure_struct*)_tmp103)->f1;goto _LL96;}else{goto
_LL97;}_LL97: goto _LL98;_LL96:({struct Cyc_Std_String_pa_struct _tmp106;_tmp106.tag=
0;_tmp106.f1=(struct _tagged_arr)_tmp104;{void*_tmp105[1]={& _tmp106};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(
_tmp105,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp107=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp107[0]=({struct Cyc_Core_Failure_struct
_tmp108;_tmp108.tag=Cyc_Core_Failure;_tmp108.f1=_tmp104;_tmp108;});_tmp107;}));
_LL98:(void)_throw(_tmp103);_LL94:;}}}
