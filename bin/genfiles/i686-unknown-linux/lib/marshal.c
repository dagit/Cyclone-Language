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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

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
unsigned char Cyc_Set_Absent[11];struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple0{unsigned int f1;
struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple1{unsigned int f1;
void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;struct _tagged_arr
f2;};struct _tuple2{unsigned int f1;struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;unsigned int f3;struct _tagged_arr
f4;};struct _tuple3{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct _tuple4{int f1;void*
f2;};extern struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_tagged_arr l);extern struct _tagged_arr Cyc_Typerep_get_tagname(unsigned int tag,
struct _tagged_arr l);extern struct _tagged_arr Cyc_Typerep_get_tagname2(unsigned int
tag,struct _tagged_arr l);extern struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct
_tagged_arr tag,struct _tagged_arr l);unsigned int Cyc_Typerep_size_type(void*rep);
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stdout;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern int Cyc_Std_fgetc(
struct Cyc_Std___cycFILE*__stream);extern int Cyc_Std_fputc(int __c,struct Cyc_Std___cycFILE*
__stream);extern unsigned int Cyc_Std_fread(struct _tagged_arr __ptr,unsigned int
__size,unsigned int __n,struct Cyc_Std___cycFILE*__stream);extern unsigned int Cyc_Std_fwrite(
struct _tagged_arr __ptr,unsigned int __size,unsigned int __n,struct Cyc_Std___cycFILE*
__s);extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(struct _tagged_arr fmt,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern int isprint(int);
extern void Cyc_Marshal_fprint_type(void*rep,struct Cyc_Std___cycFILE*fp,void*val);
extern void Cyc_Marshal_print_type(void*rep,void*val);extern void Cyc_Marshal_write_type(
void*rep,struct Cyc_Std___cycFILE*fp,void*val);extern void*Cyc_Marshal_read_type(
void*rep,struct Cyc_Std___cycFILE*fp);struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);extern void*unsafe_cast(void*x);struct _tuple5{struct Cyc_Dict_Dict*f1;
int f2;};struct _tuple6{struct _tagged_arr f1;int f2;};static int Cyc_Marshal_uint_cmp(
unsigned int a,unsigned int b){if(a == b){return 0;}else{if(a < b){return 1;}else{
return - 1;}}}static struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set(){return((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(((struct Cyc_Set_Set*(*)(
int(*cmp)(unsigned int,unsigned int)))Cyc_Set_empty)(Cyc_Marshal_uint_cmp),0);}
struct _tuple7{int f1;struct Cyc_Set_Set*f2;};static struct _tuple7*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val){return({struct _tuple7*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0->f1=((int(*)(struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_member)(
set,val);_tmp0->f2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,unsigned int elt))
Cyc_Set_insert)(set,val);_tmp0;});}inline static int Cyc_Marshal_index_member(
struct _tuple5 idx,unsigned int m){int _tmp2;struct _tuple5 _tmp1=idx;_LL1: _tmp2=_tmp1.f2;
goto _LL0;_LL0: return m < _tmp2;}static struct _tuple5 Cyc_Marshal_empty_addr_index(){
return({struct _tuple5 _tmp3;_tmp3.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,unsigned int k,int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);_tmp3.f2=
1;_tmp3;});}static int Cyc_Marshal_index_lookup(struct _tuple5 index,unsigned int val){
int _tmp5;struct Cyc_Dict_Dict*_tmp6;struct _tuple5 _tmp4=index;_LL4: _tmp6=_tmp4.f1;
goto _LL3;_LL3: _tmp5=_tmp4.f2;goto _LL2;_LL2: {struct Cyc_Core_Opt*_tmp7=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup_opt)(_tmp6,
val);if(_tmp7 != 0){return(int)_tmp7->v;}else{return _tmp5;}}}static struct _tuple5
Cyc_Marshal_index_insert(struct _tuple5 index,unsigned int val){int _tmp9;struct Cyc_Dict_Dict*
_tmpA;struct _tuple5 _tmp8=index;_LL7: _tmpA=_tmp8.f1;goto _LL6;_LL6: _tmp9=_tmp8.f2;
goto _LL5;_LL5: return({struct _tuple5 _tmpB;_tmpB.f1=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,int v))Cyc_Dict_insert)(_tmpA,val,_tmp9);
_tmpB.f2=_tmp9 + 1;_tmpB;});}static struct _tuple6 Cyc_Marshal_empty_addr_table(){
return({struct _tuple6 _tmpC;_tmpC.f1=_tag_arr(({unsigned int _tmpD=(unsigned int)1;
unsigned int*_tmpE=(unsigned int*)_cycalloc_atomic(_check_times(sizeof(
unsigned int),_tmpD));{unsigned int _tmpF=_tmpD;unsigned int i;for(i=0;i < _tmpF;i
++){_tmpE[i]=0;}};_tmpE;}),sizeof(unsigned int),(unsigned int)1);_tmpC.f2=1;
_tmpC;});}inline static int Cyc_Marshal_table_member(struct _tuple6 table,
unsigned int m){int _tmp11;struct _tuple6 _tmp10=table;_LL9: _tmp11=_tmp10.f2;goto
_LL8;_LL8: return m < _tmp11;}static struct _tuple6 Cyc_Marshal_table_insert(struct
_tuple6 tbl,unsigned int i){int _tmp13;struct _tagged_arr _tmp14;struct _tuple6 _tmp12=
tbl;_LLC: _tmp14=_tmp12.f1;goto _LLB;_LLB: _tmp13=_tmp12.f2;goto _LLA;_LLA: if(
_get_arr_size(_tmp14,sizeof(unsigned int))== _tmp13){_tmp14=({unsigned int _tmp15=(
unsigned int)(2 * _tmp13);unsigned int*_tmp16=(unsigned int*)_cycalloc_atomic(
_check_times(sizeof(unsigned int),_tmp15));struct _tagged_arr _tmp18=_tag_arr(
_tmp16,sizeof(unsigned int),(unsigned int)(2 * _tmp13));{unsigned int _tmp17=
_tmp15;unsigned int j;for(j=0;j < _tmp17;j ++){_tmp16[j]=j < _tmp13?*((unsigned int*)
_check_unknown_subscript(_tmp14,sizeof(unsigned int),(int)j)): 0;}};_tmp18;});}((
unsigned int*)_tmp14.curr)[_tmp13]=i;return({struct _tuple6 _tmp19;_tmp19.f1=
_tmp14;_tmp19.f2=_tmp13 + 1;_tmp19;});}static unsigned int Cyc_Marshal_table_lookup(
struct _tuple6 tbl,int i){return*((unsigned int*)_check_unknown_subscript(tbl.f1,
sizeof(unsigned int),i));}static void Cyc_Marshal_fprint_ref(struct Cyc_Std___cycFILE*
fp,unsigned int ref){if(ref == 0){({void*_tmp1A[0]={};Cyc_Std_fprintf(fp,_tag_arr("NULL",
sizeof(unsigned char),5),_tag_arr(_tmp1A,sizeof(void*),0));});}else{({struct Cyc_Std_Int_pa_struct
_tmp1C;_tmp1C.tag=1;_tmp1C.f1=ref;{void*_tmp1B[1]={& _tmp1C};Cyc_Std_fprintf(fp,
_tag_arr("(#%x)",sizeof(unsigned char),6),_tag_arr(_tmp1B,sizeof(void*),1));}});}}
struct _tuple8{unsigned int f1;unsigned int f2;unsigned int f3;};struct Cyc_Set_Set*
Cyc_Marshal_fprint_type_base(void*rep,struct Cyc_Set_Set*env,struct Cyc_Std___cycFILE*
fp,void*val){{void*_tmp1D=rep;unsigned int _tmp1E;int _tmp1F;void*_tmp20;
unsigned int _tmp21;void*_tmp22;struct _tagged_arr _tmp23;struct _tagged_arr*_tmp24;
struct _tagged_arr _tmp25;struct _tagged_arr _tmp26;struct _tagged_arr _tmp27;struct
_tagged_arr _tmp28;struct _tagged_arr _tmp29;struct _tagged_arr _tmp2A;struct
_tagged_arr _tmp2B;struct _tagged_arr _tmp2C;struct _tagged_arr _tmp2D;struct
_tagged_arr _tmp2E;struct _tagged_arr*_tmp2F;struct _tagged_arr _tmp30;int _tmp31;
_LLE: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 0: 0){_LL27: _tmp1F=((struct Cyc_Typerep_Int_struct*)
_tmp1D)->f1;goto _LL26;_LL26: _tmp1E=((struct Cyc_Typerep_Int_struct*)_tmp1D)->f2;
goto _LLF;}else{goto _LL10;}_LL10: if((int)_tmp1D == 0){goto _LL11;}else{goto _LL12;}
_LL12: if((int)_tmp1D == 1){goto _LL13;}else{goto _LL14;}_LL14: if((unsigned int)
_tmp1D > 2?*((int*)_tmp1D)== 1: 0){_LL29: _tmp21=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1D)->f1;goto _LL28;_LL28: _tmp20=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1D)->f2;goto _LL15;}else{goto _LL16;}_LL16: if((unsigned int)_tmp1D > 2?*((int*)
_tmp1D)== 2: 0){_LL2A: _tmp22=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp1D)->f1;
goto _LL17;}else{goto _LL18;}_LL18: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 3:
0){_LL2C: _tmp24=((struct Cyc_Typerep_Struct_struct*)_tmp1D)->f1;goto _LL2B;_LL2B:
_tmp23=((struct Cyc_Typerep_Struct_struct*)_tmp1D)->f3;goto _LL19;}else{goto _LL1A;}
_LL1A: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 4: 0){_LL2D: _tmp25=((struct Cyc_Typerep_Tuple_struct*)
_tmp1D)->f2;goto _LL1B;}else{goto _LL1C;}_LL1C: if((unsigned int)_tmp1D > 2?*((int*)
_tmp1D)== 6: 0){_LL30: _tmp28=((struct Cyc_Typerep_TUnionField_struct*)_tmp1D)->f1;
goto _LL2F;_LL2F: _tmp27=((struct Cyc_Typerep_TUnionField_struct*)_tmp1D)->f2;goto
_LL2E;_LL2E: _tmp26=((struct Cyc_Typerep_TUnionField_struct*)_tmp1D)->f4;goto _LL1D;}
else{goto _LL1E;}_LL1E: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 5: 0){_LL33:
_tmp2B=((struct Cyc_Typerep_TUnion_struct*)_tmp1D)->f1;goto _LL32;_LL32: _tmp2A=((
struct Cyc_Typerep_TUnion_struct*)_tmp1D)->f2;goto _LL31;_LL31: _tmp29=((struct Cyc_Typerep_TUnion_struct*)
_tmp1D)->f3;goto _LL1F;}else{goto _LL20;}_LL20: if((unsigned int)_tmp1D > 2?*((int*)
_tmp1D)== 7: 0){_LL35: _tmp2D=((struct Cyc_Typerep_XTUnion_struct*)_tmp1D)->f1;goto
_LL34;_LL34: _tmp2C=((struct Cyc_Typerep_XTUnion_struct*)_tmp1D)->f2;goto _LL21;}
else{goto _LL22;}_LL22: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 8: 0){_LL37:
_tmp2F=((struct Cyc_Typerep_Union_struct*)_tmp1D)->f1;goto _LL36;_LL36: _tmp2E=((
struct Cyc_Typerep_Union_struct*)_tmp1D)->f3;goto _LL23;}else{goto _LL24;}_LL24: if((
unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 9: 0){_LL39: _tmp31=((struct Cyc_Typerep_Enum_struct*)
_tmp1D)->f2;goto _LL38;_LL38: _tmp30=((struct Cyc_Typerep_Enum_struct*)_tmp1D)->f3;
goto _LL25;}else{goto _LLD;}_LLF: switch(_tmp1E){case 8: _LL3A: {unsigned char*x=((
unsigned char*(*)(void*x))unsafe_cast)(val);if(isprint((int)*x)){({struct Cyc_Std_Int_pa_struct
_tmp33;_tmp33.tag=1;_tmp33.f1=(int)((unsigned int)((int)*x));{void*_tmp32[1]={&
_tmp33};Cyc_Std_fprintf(fp,_tag_arr("'%c'",sizeof(unsigned char),5),_tag_arr(
_tmp32,sizeof(void*),1));}});}else{({struct Cyc_Std_Int_pa_struct _tmp35;_tmp35.tag=
1;_tmp35.f1=(int)((unsigned int)((int)*x));{void*_tmp34[1]={& _tmp35};Cyc_Std_fprintf(
fp,_tag_arr("'\\%d'",sizeof(unsigned char),6),_tag_arr(_tmp34,sizeof(void*),1));}});}
break;}case 16: _LL3B: {short*x=((short*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp37;_tmp37.tag=1;_tmp37.f1=(unsigned int)((int)*x);{void*_tmp36[1]={& _tmp37};
Cyc_Std_fprintf(fp,_tmp1F? _tag_arr("%d",sizeof(unsigned char),3): _tag_arr("%u",
sizeof(unsigned char),3),_tag_arr(_tmp36,sizeof(void*),1));}});break;}case 32:
_LL3C: {int*x=((int*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp39;_tmp39.tag=1;_tmp39.f1=(unsigned int)*x;{void*_tmp38[1]={& _tmp39};Cyc_Std_fprintf(
fp,_tmp1F? _tag_arr("%d",sizeof(unsigned char),3): _tag_arr("%u",sizeof(
unsigned char),3),_tag_arr(_tmp38,sizeof(void*),1));}});break;}default: _LL3D:({
struct Cyc_Std_Int_pa_struct _tmp3B;_tmp3B.tag=1;_tmp3B.f1=(int)_tmp1E;{void*
_tmp3A[1]={& _tmp3B};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("fprint_type_base: illegal int size %d bits\n",
sizeof(unsigned char),44),_tag_arr(_tmp3A,sizeof(void*),1));}});exit(1);}goto
_LLD;_LL11: {float*x=((float*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp3D;_tmp3D.tag=2;_tmp3D.f1=(double)*x;{void*_tmp3C[1]={& _tmp3D};Cyc_Std_printf(
_tag_arr("%f",sizeof(unsigned char),3),_tag_arr(_tmp3C,sizeof(void*),1));}});
goto _LLD;}_LL13: {double*x=((double*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp3F;_tmp3F.tag=2;_tmp3F.f1=*x;{void*_tmp3E[1]={& _tmp3F};Cyc_Std_printf(
_tag_arr("%f",sizeof(unsigned char),3),_tag_arr(_tmp3E,sizeof(void*),1));}});
goto _LLD;}_LL15: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));struct _tuple7 _tmp41;struct Cyc_Set_Set*_tmp42;int _tmp43;struct
_tuple7*_tmp40=Cyc_Marshal_member_insert(env,(unsigned int)y);_tmp41=*_tmp40;
_LL41: _tmp43=_tmp41.f1;goto _LL40;_LL40: _tmp42=_tmp41.f2;goto _LL3F;_LL3F: env=
_tmp42;if(! _tmp43){({struct Cyc_Std_Int_pa_struct _tmp45;_tmp45.tag=1;_tmp45.f1=(
unsigned int)y;{void*_tmp44[1]={& _tmp45};Cyc_Std_fprintf(fp,_tag_arr("{(@%x) ",
sizeof(unsigned char),8),_tag_arr(_tmp44,sizeof(void*),1));}});if(y != 0){int
typ_szb=(int)Cyc_Typerep_size_type(_tmp20);int i=0;for(0;i < _tmp21;i ++){env=Cyc_Marshal_fprint_type_base(
_tmp20,env,fp,(void*)((void*(*)(int x))unsafe_cast)(y));if(i != _tmp21 - 1){({void*
_tmp46[0]={};Cyc_Std_fprintf(fp,_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(
_tmp46,sizeof(void*),0));});}y +=typ_szb;}}else{({void*_tmp47[0]={};Cyc_Std_printf(
_tag_arr("NULL",sizeof(unsigned char),5),_tag_arr(_tmp47,sizeof(void*),0));});}({
void*_tmp48[0]={};Cyc_Std_fprintf(fp,_tag_arr(" }",sizeof(unsigned char),3),
_tag_arr(_tmp48,sizeof(void*),0));});}else{Cyc_Marshal_fprint_ref(fp,(
unsigned int)y);}goto _LLD;}_LL17: {struct _tuple8*x=((struct _tuple8*(*)(void*x))
unsafe_cast)(val);unsigned int typ_szb=Cyc_Typerep_size_type(_tmp22);unsigned int
base=(*x).f1;unsigned int curr=(*x).f2;unsigned int last_plus_one=(*x).f3;struct
_tuple7 _tmp4A;struct Cyc_Set_Set*_tmp4B;int _tmp4C;struct _tuple7*_tmp49=Cyc_Marshal_member_insert(
env,base);_tmp4A=*_tmp49;_LL44: _tmp4C=_tmp4A.f1;goto _LL43;_LL43: _tmp4B=_tmp4A.f2;
goto _LL42;_LL42: env=_tmp4B;if(! _tmp4C){({struct Cyc_Std_Int_pa_struct _tmp4E;
_tmp4E.tag=1;_tmp4E.f1=base;{void*_tmp4D[1]={& _tmp4E};Cyc_Std_fprintf(fp,
_tag_arr("[(@%x) ",sizeof(unsigned char),8),_tag_arr(_tmp4D,sizeof(void*),1));}});{
unsigned int i=base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_fprint_type_base(
_tmp22,env,fp,(void*)((void*(*)(unsigned int x))unsafe_cast)(i));if(i + typ_szb < 
last_plus_one){({void*_tmp4F[0]={};Cyc_Std_fprintf(fp,_tag_arr(", ",sizeof(
unsigned char),3),_tag_arr(_tmp4F,sizeof(void*),0));});}}}({void*_tmp50[0]={};
Cyc_Std_fprintf(fp,_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp50,
sizeof(void*),0));});}else{Cyc_Marshal_fprint_ref(fp,base);}goto _LLD;}_LL19: if(
_tmp24 == 0){({void*_tmp51[0]={};Cyc_Std_fprintf(fp,_tag_arr("{ ",sizeof(
unsigned char),3),_tag_arr(_tmp51,sizeof(void*),0));});}else{({struct Cyc_Std_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)*_tmp24;{void*_tmp52[1]={&
_tmp53};Cyc_Std_fprintf(fp,_tag_arr("%s{ ",sizeof(unsigned char),5),_tag_arr(
_tmp52,sizeof(void*),1));}});}{int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;
for(0;i < _get_arr_size(_tmp23,sizeof(struct _tuple0*));i ++){void*_tmp55;struct
_tagged_arr _tmp56;unsigned int _tmp57;struct _tuple0 _tmp54=*((struct _tuple0**)
_tmp23.curr)[i];_LL48: _tmp57=_tmp54.f1;goto _LL47;_LL47: _tmp56=_tmp54.f2;goto
_LL46;_LL46: _tmp55=_tmp54.f3;goto _LL45;_LL45:({struct Cyc_Std_String_pa_struct
_tmp59;_tmp59.tag=0;_tmp59.f1=(struct _tagged_arr)_tmp56;{void*_tmp58[1]={& _tmp59};
Cyc_Std_fprintf(fp,_tag_arr("%s=",sizeof(unsigned char),4),_tag_arr(_tmp58,
sizeof(void*),1));}});{int x_ofs=(int)(x + _tmp57);env=Cyc_Marshal_fprint_type_base(
_tmp55,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(
_tmp23,sizeof(struct _tuple0*))- 1){({void*_tmp5A[0]={};Cyc_Std_fprintf(fp,
_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(_tmp5A,sizeof(void*),0));});}}}}({
void*_tmp5B[0]={};Cyc_Std_fprintf(fp,_tag_arr(" }",sizeof(unsigned char),3),
_tag_arr(_tmp5B,sizeof(void*),0));});goto _LLD;}_LL1B:({void*_tmp5C[0]={};Cyc_Std_fprintf(
fp,_tag_arr("$( ",sizeof(unsigned char),4),_tag_arr(_tmp5C,sizeof(void*),0));});{
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmp25,
sizeof(struct _tuple1*));i ++){void*_tmp5E;unsigned int _tmp5F;struct _tuple1 _tmp5D=*((
struct _tuple1**)_tmp25.curr)[i];_LL4B: _tmp5F=_tmp5D.f1;goto _LL4A;_LL4A: _tmp5E=
_tmp5D.f2;goto _LL49;_LL49: {int x_ofs=(int)(x + _tmp5F);env=Cyc_Marshal_fprint_type_base(
_tmp5E,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(
_tmp25,sizeof(struct _tuple1*))- 1){({void*_tmp60[0]={};Cyc_Std_fprintf(fp,
_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(_tmp60,sizeof(void*),0));});}}}}({
void*_tmp61[0]={};Cyc_Std_fprintf(fp,_tag_arr(" )",sizeof(unsigned char),3),
_tag_arr(_tmp61,sizeof(void*),0));});goto _LLD;}_LL1D:({struct Cyc_Std_String_pa_struct
_tmp64;_tmp64.tag=0;_tmp64.f1=(struct _tagged_arr)_tmp27;{struct Cyc_Std_String_pa_struct
_tmp63;_tmp63.tag=0;_tmp63.f1=(struct _tagged_arr)_tmp28;{void*_tmp62[2]={& _tmp63,&
_tmp64};Cyc_Std_fprintf(fp,_tag_arr("%s.%s(",sizeof(unsigned char),7),_tag_arr(
_tmp62,sizeof(void*),2));}}});{int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;
for(0;i < _get_arr_size(_tmp26,sizeof(struct _tuple1*));i ++){void*_tmp66;
unsigned int _tmp67;struct _tuple1 _tmp65=*((struct _tuple1**)_tmp26.curr)[i];_LL4E:
_tmp67=_tmp65.f1;goto _LL4D;_LL4D: _tmp66=_tmp65.f2;goto _LL4C;_LL4C: {int x_ofs=(
int)(x + _tmp67);env=Cyc_Marshal_fprint_type_base(_tmp66,env,fp,(void*)((void*(*)(
int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp26,sizeof(struct _tuple1*))- 
1){({void*_tmp68[0]={};Cyc_Std_fprintf(fp,_tag_arr(", ",sizeof(unsigned char),3),
_tag_arr(_tmp68,sizeof(void*),0));});}}}}({void*_tmp69[0]={};Cyc_Std_fprintf(fp,
_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(_tmp69,sizeof(void*),0));});goto
_LLD;}_LL1F: {unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*
tagp < 1024){({struct Cyc_Std_String_pa_struct _tmp6B;_tmp6B.tag=0;_tmp6B.f1=(
struct _tagged_arr)Cyc_Typerep_get_tagname(*tagp,_tmp2A);{void*_tmp6A[1]={& _tmp6B};
Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp6A,
sizeof(void*),1));}});}else{struct _tuple7 _tmp6D;struct Cyc_Set_Set*_tmp6E;int
_tmp6F;struct _tuple7*_tmp6C=Cyc_Marshal_member_insert(env,*tagp);_tmp6D=*_tmp6C;
_LL51: _tmp6F=_tmp6D.f1;goto _LL50;_LL50: _tmp6E=_tmp6D.f2;goto _LL4F;_LL4F: env=
_tmp6E;if(! _tmp6F){({struct Cyc_Std_Int_pa_struct _tmp71;_tmp71.tag=1;_tmp71.f1=*
tagp;{void*_tmp70[1]={& _tmp71};Cyc_Std_fprintf(fp,_tag_arr("(@%x)",sizeof(
unsigned char),6),_tag_arr(_tmp70,sizeof(void*),1));}});{unsigned int*x=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;void*
_tmp73;int _tmp74;struct _tuple4 _tmp72=Cyc_Typerep_get_unionbranch(tag,_tmp29);
_LL54: _tmp74=_tmp72.f1;goto _LL53;_LL53: _tmp73=_tmp72.f2;goto _LL52;_LL52: {void*
xoff=((void*(*)(unsigned int x))unsafe_cast)(*tagp + _tmp74);({struct Cyc_Std_String_pa_struct
_tmp76;_tmp76.tag=0;_tmp76.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname2(tag,
_tmp29);{void*_tmp75[1]={& _tmp76};Cyc_Std_fprintf(fp,_tag_arr("&%s[",sizeof(
unsigned char),5),_tag_arr(_tmp75,sizeof(void*),1));}});env=Cyc_Marshal_fprint_type_base(
_tmp73,env,fp,xoff);({void*_tmp77[0]={};Cyc_Std_fprintf(fp,_tag_arr("]",sizeof(
unsigned char),2),_tag_arr(_tmp77,sizeof(void*),0));});}}}else{Cyc_Marshal_fprint_ref(
fp,*tagp);}}goto _LLD;}_LL21: {unsigned int*xtunionp=((unsigned int*(*)(void*x))
unsafe_cast)(val);struct _tuple7 _tmp79;struct Cyc_Set_Set*_tmp7A;int _tmp7B;struct
_tuple7*_tmp78=Cyc_Marshal_member_insert(env,*xtunionp);_tmp79=*_tmp78;_LL57:
_tmp7B=_tmp79.f1;goto _LL56;_LL56: _tmp7A=_tmp79.f2;goto _LL55;_LL55: env=_tmp7A;if(
! _tmp7B){({struct Cyc_Std_Int_pa_struct _tmp7D;_tmp7D.tag=1;_tmp7D.f1=*xtunionp;{
void*_tmp7C[1]={& _tmp7D};Cyc_Std_fprintf(fp,_tag_arr("(@%x)",sizeof(
unsigned char),6),_tag_arr(_tmp7C,sizeof(void*),1));}});{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){
unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*xtstructp
+ 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({
struct Cyc_Std_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct _tagged_arr)
xtname;{void*_tmp7E[1]={& _tmp7F};Cyc_Std_fprintf(fp,_tag_arr("&%s[( ",sizeof(
unsigned char),7),_tag_arr(_tmp7E,sizeof(void*),1));}});{void*_tmp81;int _tmp82;
struct _tuple4 _tmp80=Cyc_Typerep_get_xtunionbranch(xtname,_tmp2C);_LL5A: _tmp82=
_tmp80.f1;goto _LL59;_LL59: _tmp81=_tmp80.f2;goto _LL58;_LL58: {void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*xtunionp + _tmp82);env=Cyc_Marshal_fprint_type_base(
_tmp81,env,fp,xoff);({void*_tmp83[0]={};Cyc_Std_fprintf(fp,_tag_arr(" )]",
sizeof(unsigned char),4),_tag_arr(_tmp83,sizeof(void*),0));});}}}else{
unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 
4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({
struct Cyc_Std_String_pa_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(struct _tagged_arr)
xtname;{void*_tmp84[1]={& _tmp85};Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp84,sizeof(void*),1));}});}}}else{Cyc_Marshal_fprint_ref(
fp,*xtunionp);}goto _LLD;}_LL23: if(_tmp2F == 0){({void*_tmp86[0]={};Cyc_Std_fprintf(
fp,_tag_arr("{",sizeof(unsigned char),2),_tag_arr(_tmp86,sizeof(void*),0));});}
else{({struct Cyc_Std_String_pa_struct _tmp88;_tmp88.tag=0;_tmp88.f1=(struct
_tagged_arr)*_tmp2F;{void*_tmp87[1]={& _tmp88};Cyc_Std_fprintf(fp,_tag_arr("%s{",
sizeof(unsigned char),4),_tag_arr(_tmp87,sizeof(void*),1));}});}{int i=0;for(0;i < 
_get_arr_size(_tmp2E,sizeof(struct _tuple3*));i ++){void*_tmp8A;struct _tagged_arr
_tmp8B;struct _tuple3 _tmp89=*((struct _tuple3**)_tmp2E.curr)[i];_LL5D: _tmp8B=
_tmp89.f1;goto _LL5C;_LL5C: _tmp8A=_tmp89.f2;goto _LL5B;_LL5B:({struct Cyc_Std_String_pa_struct
_tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _tagged_arr)_tmp8B;{void*_tmp8C[1]={& _tmp8D};
Cyc_Std_fprintf(fp,_tag_arr("%s=",sizeof(unsigned char),4),_tag_arr(_tmp8C,
sizeof(void*),1));}});Cyc_Marshal_fprint_type_base(_tmp8A,env,fp,val);if(i < 
_get_arr_size(_tmp2E,sizeof(struct _tuple3*))- 1){({void*_tmp8E[0]={};Cyc_Std_fprintf(
fp,_tag_arr(" | ",sizeof(unsigned char),4),_tag_arr(_tmp8E,sizeof(void*),0));});}}}({
void*_tmp8F[0]={};Cyc_Std_fprintf(fp,_tag_arr("}",sizeof(unsigned char),2),
_tag_arr(_tmp8F,sizeof(void*),0));});goto _LLD;_LL25: {unsigned int*tagp=((
unsigned int*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_String_pa_struct
_tmp91;_tmp91.tag=0;_tmp91.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname(*tagp,
_tmp30);{void*_tmp90[1]={& _tmp91};Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp90,sizeof(void*),1));}});goto _LLD;}_LLD:;}return
env;}void Cyc_Marshal_fprint_type(void*ts,struct Cyc_Std___cycFILE*fp,void*val){
struct _handler_cons _tmp92;_push_handler(& _tmp92);{int _tmp94=0;if(setjmp(_tmp92.handler)){
_tmp94=1;}if(! _tmp94){Cyc_Marshal_fprint_type_base(ts,Cyc_Marshal_empty_addr_set(),
fp,val);;_pop_handler();}else{void*_tmp93=(void*)_exn_thrown;void*_tmp96=_tmp93;
struct _tagged_arr _tmp97;_LL5F: if(*((void**)_tmp96)== Cyc_Core_Failure){_LL63:
_tmp97=((struct Cyc_Core_Failure_struct*)_tmp96)->f1;goto _LL60;}else{goto _LL61;}
_LL61: goto _LL62;_LL60:({struct Cyc_Std_String_pa_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(
struct _tagged_arr)_tmp97;{void*_tmp98[1]={& _tmp99};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(_tmp98,sizeof(
void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp9A=
_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Core_Failure_struct _tmp9B;
_tmp9B.tag=Cyc_Core_Failure;_tmp9B.f1=_tmp97;_tmp9B;});_tmp9A;}));_LL62:(void)
_throw(_tmp96);_LL5E:;}}}void Cyc_Marshal_print_type(void*ts,void*val){Cyc_Marshal_fprint_type(
ts,Cyc_Std_stdout,val);}void Cyc_Marshal_cycputchar(unsigned char x,struct Cyc_Std___cycFILE*
fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(
unsigned char),1)),sizeof(unsigned char),1,fp)< 1){(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Core_Failure_struct
_tmp9D;_tmp9D.tag=Cyc_Core_Failure;_tmp9D.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmp9D;});_tmp9C;}));}}void Cyc_Marshal_cycputshort(short x,
struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(short),1)),sizeof(short),1,fp)< 1){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_Core_Failure_struct _tmp9F;_tmp9F.tag=Cyc_Core_Failure;_tmp9F.f1=
_tag_arr("Write failure",sizeof(unsigned char),14);_tmp9F;});_tmp9E;}));}}void
Cyc_Marshal_cycputint(int x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct
_tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(int),1)),sizeof(int),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpA0=_cycalloc(sizeof(*
_tmpA0));_tmpA0[0]=({struct Cyc_Core_Failure_struct _tmpA1;_tmpA1.tag=Cyc_Core_Failure;
_tmpA1.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmpA1;});_tmpA0;}));}}
void Cyc_Marshal_cycputfloat(float x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(float),1)),sizeof(float),
1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpA2=_cycalloc(
sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Core_Failure_struct _tmpA3;_tmpA3.tag=Cyc_Core_Failure;
_tmpA3.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmpA3;});_tmpA2;}));}}
void Cyc_Marshal_cycputdouble(double x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(double),1)),sizeof(
double),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpA4=
_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Core_Failure_struct _tmpA5;
_tmpA5.tag=Cyc_Core_Failure;_tmpA5.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmpA5;});_tmpA4;}));}}void Cyc_Marshal_cycputvarint(
unsigned int i,struct Cyc_Std___cycFILE*fp){int width=0;unsigned int mask=-256;
while((int)mask){if((int)(i & mask)){width ++;}mask <<=8;}Cyc_Std_fputc((int)(width
<< 6 | i & 63),fp);i >>=6;{int j=0;for(0;j < width;j ++){Cyc_Std_fputc((int)(i & 255),
fp);i >>=8;}}}unsigned char Cyc_Marshal_cycgetchar(struct Cyc_Std___cycFILE*fp){
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpAA=sizeof(
unsigned char);unsigned char*_tmpAB=(unsigned char*)_region_malloc(r,
_check_times(sizeof(unsigned char),_tmpAA));{unsigned int _tmpAC=_tmpAA;
unsigned int i;for(i=0;i < _tmpAC;i ++){_tmpAB[i]='\000';}};_tmpAB;}),sizeof(
unsigned char),sizeof(unsigned char));if(Cyc_Std_fread(x,sizeof(unsigned char),1,
fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpA7=_cycalloc(
sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Core_Failure_struct _tmpA8;_tmpA8.tag=Cyc_Core_Failure;
_tmpA8.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmpA8;});_tmpA7;}));}{
unsigned char*xx=((unsigned char*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));unsigned char
_tmpA9=*xx;_npop_handler(0);return _tmpA9;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpAD=_new_region("r");struct
_RegionHandle*r=& _tmpAD;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpB1=sizeof(short);unsigned char*_tmpB2=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmpB1));{unsigned int _tmpB3=
_tmpB1;unsigned int i;for(i=0;i < _tmpB3;i ++){_tmpB2[i]='\000';}};_tmpB2;}),
sizeof(unsigned char),sizeof(short));if(Cyc_Std_fread(x,sizeof(short),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE[0]=({struct Cyc_Core_Failure_struct _tmpAF;_tmpAF.tag=Cyc_Core_Failure;
_tmpAF.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmpAF;});_tmpAE;}));}{
short*xx=((short*(*)(unsigned char*x))unsafe_cast)((unsigned char*)_check_null(
_untag_arr(x,sizeof(unsigned char),1)));short _tmpB0=*xx;_npop_handler(0);return
_tmpB0;}};_pop_region(r);}int Cyc_Marshal_cycgetint(struct Cyc_Std___cycFILE*fp){
struct _RegionHandle _tmpB4=_new_region("r");struct _RegionHandle*r=& _tmpB4;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpB8=sizeof(int);
unsigned char*_tmpB9=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmpB8));{unsigned int _tmpBA=_tmpB8;unsigned int i;for(i=0;i < 
_tmpBA;i ++){_tmpB9[i]='\000';}};_tmpB9;}),sizeof(unsigned char),sizeof(int));if(
Cyc_Std_fread(x,sizeof(int),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Core_Failure_struct
_tmpB6;_tmpB6.tag=Cyc_Core_Failure;_tmpB6.f1=_tag_arr("Read failure",sizeof(
unsigned char),13);_tmpB6;});_tmpB5;}));}{int*xx=((int*(*)(unsigned char*x))
unsafe_cast)((unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));
int _tmpB7=*xx;_npop_handler(0);return _tmpB7;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpBB=_new_region("r");struct
_RegionHandle*r=& _tmpBB;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpBF=sizeof(float);unsigned char*_tmpC0=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmpBF));{unsigned int _tmpC1=
_tmpBF;unsigned int i;for(i=0;i < _tmpC1;i ++){_tmpC0[i]='\000';}};_tmpC0;}),
sizeof(unsigned char),sizeof(float));if(Cyc_Std_fread(x,sizeof(float),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpBC=_cycalloc(sizeof(*
_tmpBC));_tmpBC[0]=({struct Cyc_Core_Failure_struct _tmpBD;_tmpBD.tag=Cyc_Core_Failure;
_tmpBD.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmpBD;});_tmpBC;}));}{
float*xx=((float*(*)(unsigned char*x))unsafe_cast)((unsigned char*)_check_null(
_untag_arr(x,sizeof(unsigned char),1)));float _tmpBE=*xx;_npop_handler(0);return
_tmpBE;}};_pop_region(r);}double Cyc_Marshal_cycgetdouble(struct Cyc_Std___cycFILE*
fp){struct _RegionHandle _tmpC2=_new_region("r");struct _RegionHandle*r=& _tmpC2;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpC6=sizeof(double);
unsigned char*_tmpC7=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmpC6));{unsigned int _tmpC8=_tmpC6;unsigned int i;for(i=0;i < 
_tmpC8;i ++){_tmpC7[i]='\000';}};_tmpC7;}),sizeof(unsigned char),sizeof(double));
if(Cyc_Std_fread(x,sizeof(double),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Core_Failure_struct
_tmpC4;_tmpC4.tag=Cyc_Core_Failure;_tmpC4.f1=_tag_arr("Read failure",sizeof(
unsigned char),13);_tmpC4;});_tmpC3;}));}{double*xx=((double*(*)(unsigned char*x))
unsafe_cast)((unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));
double _tmpC5=*xx;_npop_handler(0);return _tmpC5;}};_pop_region(r);}unsigned int
Cyc_Marshal_cycgetvarint(struct Cyc_Std___cycFILE*fp){int i=0;int j=(int)Cyc_Marshal_cycgetchar(
fp);int width=(j & 192)>> 6;int pos=6;i +=j & 63;{int k=0;for(0;k < width;k ++){j=(int)
Cyc_Marshal_cycgetchar(fp);i |=j << pos;pos +=8;}}return(unsigned int)i;}struct
_tuple5 Cyc_Marshal_write_type_base(void*rep,struct _tuple5 env,struct Cyc_Std___cycFILE*
fp,void*val){{void*_tmpC9=rep;unsigned int _tmpCA;void*_tmpCB;unsigned int _tmpCC;
void*_tmpCD;struct _tagged_arr _tmpCE;struct _tagged_arr _tmpCF;struct _tagged_arr
_tmpD0;struct _tagged_arr _tmpD1;struct _tagged_arr _tmpD2;struct _tagged_arr _tmpD3;
int _tmpD4;int _tmpD5;_LL65: if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)== 0: 0){
_LL7D: _tmpCA=((struct Cyc_Typerep_Int_struct*)_tmpC9)->f2;goto _LL66;}else{goto
_LL67;}_LL67: if((int)_tmpC9 == 0){goto _LL68;}else{goto _LL69;}_LL69: if((int)_tmpC9
== 1){goto _LL6A;}else{goto _LL6B;}_LL6B: if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)
== 1: 0){_LL7F: _tmpCC=((struct Cyc_Typerep_ThinPtr_struct*)_tmpC9)->f1;goto _LL7E;
_LL7E: _tmpCB=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmpC9)->f2;goto _LL6C;}
else{goto _LL6D;}_LL6D: if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)== 2: 0){_LL80:
_tmpCD=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpC9)->f1;goto _LL6E;}else{
goto _LL6F;}_LL6F: if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)== 3: 0){_LL81: _tmpCE=((
struct Cyc_Typerep_Struct_struct*)_tmpC9)->f3;goto _LL70;}else{goto _LL71;}_LL71:
if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)== 4: 0){_LL82: _tmpCF=((struct Cyc_Typerep_Tuple_struct*)
_tmpC9)->f2;goto _LL72;}else{goto _LL73;}_LL73: if((unsigned int)_tmpC9 > 2?*((int*)
_tmpC9)== 6: 0){_LL83: _tmpD0=((struct Cyc_Typerep_TUnionField_struct*)_tmpC9)->f4;
goto _LL74;}else{goto _LL75;}_LL75: if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)== 5:
0){_LL85: _tmpD2=((struct Cyc_Typerep_TUnion_struct*)_tmpC9)->f2;goto _LL84;_LL84:
_tmpD1=((struct Cyc_Typerep_TUnion_struct*)_tmpC9)->f3;goto _LL76;}else{goto _LL77;}
_LL77: if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)== 7: 0){_LL86: _tmpD3=((struct Cyc_Typerep_XTUnion_struct*)
_tmpC9)->f2;goto _LL78;}else{goto _LL79;}_LL79: if((unsigned int)_tmpC9 > 2?*((int*)
_tmpC9)== 8: 0){_LL87: _tmpD4=((struct Cyc_Typerep_Union_struct*)_tmpC9)->f2;goto
_LL7A;}else{goto _LL7B;}_LL7B: if((unsigned int)_tmpC9 > 2?*((int*)_tmpC9)== 9: 0){
_LL88: _tmpD5=((struct Cyc_Typerep_Enum_struct*)_tmpC9)->f2;goto _LL7C;}else{goto
_LL64;}_LL66: switch(_tmpCA){case 8: _LL89: {unsigned char*x=((unsigned char*(*)(
void*x))unsafe_cast)(val);Cyc_Marshal_cycputchar(*x,fp);break;}case 16: _LL8A: {
short*x=((short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*x,fp);
break;}case 32: _LL8B: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*
x,fp);break;}default: _LL8C:({struct Cyc_Std_Int_pa_struct _tmpD7;_tmpD7.tag=1;
_tmpD7.f1=(int)_tmpCA;{void*_tmpD6[1]={& _tmpD7};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("write_type_base: illegal int size %d bits\n",sizeof(unsigned char),43),
_tag_arr(_tmpD6,sizeof(void*),1));}});exit(1);}goto _LL64;_LL68: {float*x=((float*(*)(
void*x))unsafe_cast)(val);Cyc_Marshal_cycputfloat(*x,fp);goto _LL64;}_LL6A: {
double*x=((double*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*x,fp);
goto _LL64;}_LL6C: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));int _tmpD8=Cyc_Marshal_index_lookup(env,(unsigned int)y);Cyc_Marshal_cycputvarint((
unsigned int)_tmpD8,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmpD8)){
env=Cyc_Marshal_index_insert(env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmpCB);int i=0;for(0;i < _tmpCC;i ++){env=Cyc_Marshal_write_type_base(_tmpCB,env,
fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}goto _LL64;}_LL6E: {
struct _tuple8*x=((struct _tuple8*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmpCD);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;unsigned int pos=(curr - base)/ typ_szb;
unsigned int sz=(last_plus_one - base)/ typ_szb;int _tmpD9=Cyc_Marshal_index_lookup(
env,base);Cyc_Marshal_cycputvarint((unsigned int)_tmpD9,fp);Cyc_Marshal_cycputvarint(
pos,fp);Cyc_Marshal_cycputvarint(sz,fp);if(! Cyc_Marshal_index_member(env,(
unsigned int)_tmpD9)){env=Cyc_Marshal_index_insert(env,base);{unsigned int i=base;
for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(_tmpCD,env,fp,(
void*)((void*(*)(unsigned int x))unsafe_cast)(i));}}}goto _LL64;}_LL70: {int x=((
int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpCE,sizeof(
struct _tuple0*));i ++){void*_tmpDB;unsigned int _tmpDC;struct _tuple0 _tmpDA=*((
struct _tuple0**)_tmpCE.curr)[i];_LL90: _tmpDC=_tmpDA.f1;goto _LL8F;_LL8F: _tmpDB=
_tmpDA.f3;goto _LL8E;_LL8E: {int x_ofs=(int)(x + _tmpDC);env=Cyc_Marshal_write_type_base(
_tmpDB,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL64;}_LL72: {
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpCF,
sizeof(struct _tuple1*));i ++){void*_tmpDE;unsigned int _tmpDF;struct _tuple1 _tmpDD=*((
struct _tuple1**)_tmpCF.curr)[i];_LL93: _tmpDF=_tmpDD.f1;goto _LL92;_LL92: _tmpDE=
_tmpDD.f2;goto _LL91;_LL91: {int x_ofs=(int)(x + _tmpDF);env=Cyc_Marshal_write_type_base(
_tmpDE,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL64;}_LL74: {
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpD0,
sizeof(struct _tuple1*));i ++){void*_tmpE1;unsigned int _tmpE2;struct _tuple1 _tmpE0=*((
struct _tuple1**)_tmpD0.curr)[i];_LL96: _tmpE2=_tmpE0.f1;goto _LL95;_LL95: _tmpE1=
_tmpE0.f2;goto _LL94;_LL94: {int x_ofs=(int)(x + _tmpE2);env=Cyc_Marshal_write_type_base(
_tmpE1,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL64;}_LL76: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024){
Cyc_Marshal_cycputvarint(*tagp,fp);}else{int _tmpE3=Cyc_Marshal_index_lookup(env,*
tagp);Cyc_Marshal_cycputvarint((unsigned int)(_tmpE3 + 1024),fp);if(! Cyc_Marshal_index_member(
env,(unsigned int)_tmpE3)){env=Cyc_Marshal_index_insert(env,*tagp);{unsigned int*
x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;Cyc_Marshal_cycputvarint(
tag,fp);{void*_tmpE5;int _tmpE6;struct _tuple4 _tmpE4=Cyc_Typerep_get_unionbranch(
tag,_tmpD1);_LL99: _tmpE6=_tmpE4.f1;goto _LL98;_LL98: _tmpE5=_tmpE4.f2;goto _LL97;
_LL97: {void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*tagp + _tmpE6);env=Cyc_Marshal_write_type_base(
_tmpE5,env,fp,xoff);}}}}}goto _LL64;}_LL78: {unsigned int*xtunionp=((unsigned int*(*)(
void*x))unsafe_cast)(val);int _tmpE7=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint((
unsigned int)_tmpE7,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmpE7)){
env=Cyc_Marshal_index_insert(env,*xtunionp);{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',
fp);{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtstructp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct
_tagged_arr)xtname;{void*_tmpE8[1]={& _tmpE9};Cyc_Std_fprintf(fp,_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpE8,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',
fp);{void*_tmpEB;int _tmpEC;struct _tuple4 _tmpEA=Cyc_Typerep_get_xtunionbranch(
xtname,_tmpD3);_LL9C: _tmpEC=_tmpEA.f1;goto _LL9B;_LL9B: _tmpEB=_tmpEA.f2;goto _LL9A;
_LL9A: {void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*xtunionp + _tmpEC);env=
Cyc_Marshal_write_type_base(_tmpEB,env,fp,xoff);}}}}else{Cyc_Marshal_cycputchar('\000',
fp);{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtunionp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct
_tagged_arr)xtname;{void*_tmpED[1]={& _tmpEE};Cyc_Std_fprintf(fp,_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpED,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',
fp);}}}}goto _LL64;}_LL7A: env=Cyc_Marshal_write_type_base((void*)({struct Cyc_Typerep_Tuple_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Typerep_Tuple_struct
_tmpF0;_tmpF0.tag=4;_tmpF0.f1=(unsigned int)_tmpD4;_tmpF0.f2=({unsigned int
_tmpF1=(unsigned int)_tmpD4;struct _tuple1**_tmpF2=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmpF1));struct _tagged_arr _tmpF7=_tag_arr(
_tmpF2,sizeof(struct _tuple1*),(unsigned int)_tmpD4);{unsigned int _tmpF3=_tmpF1;
unsigned int i;for(i=0;i < _tmpF3;i ++){_tmpF2[i]=({struct _tuple1*_tmpF4=_cycalloc(
sizeof(*_tmpF4));_tmpF4->f1=i;_tmpF4->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmpF5=_cycalloc_atomic(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Typerep_Int_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=0;_tmpF6.f2=8;_tmpF6;});_tmpF5;});_tmpF4;});}};
_tmpF7;});_tmpF0;});_tmpEF;}),env,fp,val);goto _LL64;_LL7C: env=Cyc_Marshal_write_type_base((
void*)({struct Cyc_Typerep_Int_struct*_tmpF8=_cycalloc_atomic(sizeof(*_tmpF8));
_tmpF8[0]=({struct Cyc_Typerep_Int_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=0;_tmpF9.f2=(
unsigned int)(_tmpD5 << 3);_tmpF9;});_tmpF8;}),env,fp,val);_LL64:;}return env;}
void Cyc_Marshal_write_type(void*ts,struct Cyc_Std___cycFILE*fp,void*val){struct
_handler_cons _tmpFA;_push_handler(& _tmpFA);{int _tmpFC=0;if(setjmp(_tmpFA.handler)){
_tmpFC=1;}if(! _tmpFC){Cyc_Marshal_write_type_base(ts,Cyc_Marshal_empty_addr_index(),
fp,val);;_pop_handler();}else{void*_tmpFB=(void*)_exn_thrown;void*_tmpFE=_tmpFB;
struct _tagged_arr _tmpFF;_LL9E: if(*((void**)_tmpFE)== Cyc_Core_Failure){_LLA2:
_tmpFF=((struct Cyc_Core_Failure_struct*)_tmpFE)->f1;goto _LL9F;}else{goto _LLA0;}
_LLA0: goto _LLA1;_LL9F:({struct Cyc_Std_String_pa_struct _tmp101;_tmp101.tag=0;
_tmp101.f1=(struct _tagged_arr)_tmpFF;{void*_tmp100[1]={& _tmp101};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(
_tmp100,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Core_Failure_struct
_tmp103;_tmp103.tag=Cyc_Core_Failure;_tmp103.f1=_tmpFF;_tmp103;});_tmp102;}));
_LLA1:(void)_throw(_tmpFE);_LL9D:;}}}void Cyc_Marshal_fscanntstring(struct Cyc_Std___cycFILE*
fp,struct _tagged_arr s){int c;int i=0;while(c=Cyc_Std_fgetc(fp)){if(c == - 1){(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp104=_cycalloc(sizeof(*_tmp104));
_tmp104[0]=({struct Cyc_Core_Failure_struct _tmp105;_tmp105.tag=Cyc_Core_Failure;
_tmp105.f1=_tag_arr("Read error",sizeof(unsigned char),11);_tmp105;});_tmp104;}));}((
unsigned char*)s.curr)[i]=(unsigned char)c;i ++;}((unsigned char*)s.curr)[i]='\000';}
struct _tuple6 Cyc_Marshal_read_type_base(void*rep,struct _tuple6 env,struct Cyc_Std___cycFILE*
fp,void*result){{void*_tmp106=rep;unsigned int _tmp107;void*_tmp108;unsigned int
_tmp109;void*_tmp10A;struct _tagged_arr _tmp10B;struct _tagged_arr _tmp10C;struct
_tagged_arr _tmp10D;struct _tagged_arr _tmp10E;struct _tagged_arr _tmp10F;struct
_tagged_arr _tmp110;int _tmp111;int _tmp112;_LLA4: if((unsigned int)_tmp106 > 2?*((
int*)_tmp106)== 0: 0){_LLBC: _tmp107=((struct Cyc_Typerep_Int_struct*)_tmp106)->f2;
goto _LLA5;}else{goto _LLA6;}_LLA6: if((int)_tmp106 == 0){goto _LLA7;}else{goto _LLA8;}
_LLA8: if((int)_tmp106 == 1){goto _LLA9;}else{goto _LLAA;}_LLAA: if((unsigned int)
_tmp106 > 2?*((int*)_tmp106)== 1: 0){_LLBE: _tmp109=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp106)->f1;goto _LLBD;_LLBD: _tmp108=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp106)->f2;goto _LLAB;}else{goto _LLAC;}_LLAC: if((unsigned int)_tmp106 > 2?*((int*)
_tmp106)== 2: 0){_LLBF: _tmp10A=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp106)->f1;
goto _LLAD;}else{goto _LLAE;}_LLAE: if((unsigned int)_tmp106 > 2?*((int*)_tmp106)== 
3: 0){_LLC0: _tmp10B=((struct Cyc_Typerep_Struct_struct*)_tmp106)->f3;goto _LLAF;}
else{goto _LLB0;}_LLB0: if((unsigned int)_tmp106 > 2?*((int*)_tmp106)== 4: 0){_LLC1:
_tmp10C=((struct Cyc_Typerep_Tuple_struct*)_tmp106)->f2;goto _LLB1;}else{goto _LLB2;}
_LLB2: if((unsigned int)_tmp106 > 2?*((int*)_tmp106)== 6: 0){_LLC2: _tmp10D=((struct
Cyc_Typerep_TUnionField_struct*)_tmp106)->f4;goto _LLB3;}else{goto _LLB4;}_LLB4:
if((unsigned int)_tmp106 > 2?*((int*)_tmp106)== 5: 0){_LLC4: _tmp10F=((struct Cyc_Typerep_TUnion_struct*)
_tmp106)->f2;goto _LLC3;_LLC3: _tmp10E=((struct Cyc_Typerep_TUnion_struct*)_tmp106)->f3;
goto _LLB5;}else{goto _LLB6;}_LLB6: if((unsigned int)_tmp106 > 2?*((int*)_tmp106)== 
7: 0){_LLC5: _tmp110=((struct Cyc_Typerep_XTUnion_struct*)_tmp106)->f2;goto _LLB7;}
else{goto _LLB8;}_LLB8: if((unsigned int)_tmp106 > 2?*((int*)_tmp106)== 8: 0){_LLC6:
_tmp111=((struct Cyc_Typerep_Union_struct*)_tmp106)->f2;goto _LLB9;}else{goto _LLBA;}
_LLBA: if((unsigned int)_tmp106 > 2?*((int*)_tmp106)== 9: 0){_LLC7: _tmp112=((struct
Cyc_Typerep_Enum_struct*)_tmp106)->f2;goto _LLBB;}else{goto _LLA3;}_LLA5: switch(
_tmp107){case 8: _LLC8: {unsigned char*x=((unsigned char*(*)(void*x))unsafe_cast)(
result);*x=Cyc_Marshal_cycgetchar(fp);break;}case 16: _LLC9: {short*x=((short*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetshort(fp);break;}case 32: _LLCA: {
int*x=((int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(fp);break;}
default: _LLCB:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Core_Failure_struct _tmp114;
_tmp114.tag=Cyc_Core_Failure;_tmp114.f1=_tag_arr("read_type_base: illegal int size bits",
sizeof(unsigned char),38);_tmp114;});_tmp113;}));}goto _LLA3;_LLA7: {float*y=((
float*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetfloat(fp);goto _LLA3;}
_LLA9: {double*y=((double*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(
fp);goto _LLA3;}_LLAB: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmp108);struct
_tagged_arr buf=({unsigned int _tmp115=_tmp109 * typ_szb;unsigned char*_tmp116=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp115));
struct _tagged_arr _tmp118=_tag_arr(_tmp116,sizeof(unsigned char),_tmp109 * typ_szb);{
unsigned int _tmp117=_tmp115;unsigned int i;for(i=0;i < _tmp117;i ++){_tmp116[i]='\000';}};
_tmp118;});unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(
_untag_arr(buf,sizeof(unsigned char),1)));unsigned int*x=((unsigned int*(*)(void*
x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(
int)bufptr;int i=0;for(0;i < _tmp109;i ++){env=Cyc_Marshal_read_type_base(_tmp108,
env,fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*
x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(
env,mem);}goto _LLA3;}_LLAD: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);int pos=(
int)Cyc_Marshal_cycgetvarint(fp);int sz=(int)Cyc_Marshal_cycgetvarint(fp);int
typ_szb=(int)Cyc_Typerep_size_type(_tmp10A);if(! Cyc_Marshal_table_member(env,(
unsigned int)mem)){struct _tagged_arr buf=({unsigned int _tmp119=(unsigned int)(sz * 
typ_szb);unsigned char*_tmp11A=(unsigned char*)_cycalloc_atomic(_check_times(
sizeof(unsigned char),_tmp119));struct _tagged_arr _tmp11C=_tag_arr(_tmp11A,
sizeof(unsigned char),(unsigned int)(sz * typ_szb));{unsigned int _tmp11B=_tmp119;
unsigned int i;for(i=0;i < _tmp11B;i ++){_tmp11A[i]='\000';}};_tmp11C;});
unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,
sizeof(unsigned char),1)));struct _tuple8*x=((struct _tuple8*(*)(void*x))
unsafe_cast)(result);(*x).f1=bufptr;(*x).f2=bufptr + pos * typ_szb;(*x).f3=bufptr + 
sz * typ_szb;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int i=0;
for(0;i < sz;i ++){env=Cyc_Marshal_read_type_base(_tmp10A,env,fp,(void*)((void*(*)(
int x))unsafe_cast)(y));y +=typ_szb;}}}else{struct _tuple8*x=((struct _tuple8*(*)(
void*x))unsafe_cast)(result);(*x).f1=Cyc_Marshal_table_lookup(env,mem);(*x).f2=(*
x).f1 + typ_szb * pos;(*x).f3=(*x).f1 + typ_szb * sz;}goto _LLA3;}_LLAF: {
unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < 
_get_arr_size(_tmp10B,sizeof(struct _tuple0*));i ++){void*_tmp11E;unsigned int
_tmp11F;struct _tuple0 _tmp11D=*((struct _tuple0**)_tmp10B.curr)[i];_LLCF: _tmp11F=
_tmp11D.f1;goto _LLCE;_LLCE: _tmp11E=_tmp11D.f3;goto _LLCD;_LLCD: {int x_ofs=(int)(x
+ _tmp11F);env=Cyc_Marshal_read_type_base(_tmp11E,env,fp,(void*)((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LLA3;}_LLB1: {unsigned int x=((unsigned int(*)(void*
x))unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmp10C,sizeof(struct
_tuple1*));i ++){void*_tmp121;unsigned int _tmp122;struct _tuple1 _tmp120=*((struct
_tuple1**)_tmp10C.curr)[i];_LLD2: _tmp122=_tmp120.f1;goto _LLD1;_LLD1: _tmp121=
_tmp120.f2;goto _LLD0;_LLD0: {int x_ofs=(int)(x + _tmp122);env=Cyc_Marshal_read_type_base(
_tmp121,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LLA3;}_LLB3: {
unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < 
_get_arr_size(_tmp10D,sizeof(struct _tuple1*));i ++){void*_tmp124;unsigned int
_tmp125;struct _tuple1 _tmp123=*((struct _tuple1**)_tmp10D.curr)[i];_LLD5: _tmp125=
_tmp123.f1;goto _LLD4;_LLD4: _tmp124=_tmp123.f2;goto _LLD3;_LLD3: {int x_ofs=(int)(x
+ _tmp125);env=Cyc_Marshal_read_type_base(_tmp124,env,fp,(void*)((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LLA3;}_LLB5: {int tagmem=(int)Cyc_Marshal_cycgetvarint(
fp);if(tagmem < 1024){int tag=tagmem;unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=(unsigned int)tag;}else{int mem=tagmem - 1024;if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){int tag=(int)Cyc_Marshal_cycgetvarint(fp);void*_tmp127;
int _tmp128;struct _tuple4 _tmp126=Cyc_Typerep_get_unionbranch((unsigned int)tag,
_tmp10E);_LLD8: _tmp128=_tmp126.f1;goto _LLD7;_LLD7: _tmp127=_tmp126.f2;goto _LLD6;
_LLD6: {struct _tagged_arr buf=({unsigned int _tmp129=_tmp128 + Cyc_Typerep_size_type(
_tmp127);unsigned char*_tmp12A=(unsigned char*)_cycalloc_atomic(_check_times(
sizeof(unsigned char),_tmp129));struct _tagged_arr _tmp12C=_tag_arr(_tmp12A,
sizeof(unsigned char),_tmp128 + Cyc_Typerep_size_type(_tmp127));{unsigned int
_tmp12B=_tmp129;unsigned int i;for(i=0;i < _tmp12B;i ++){_tmp12A[i]='\000';}};
_tmp12C;});unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(
_untag_arr(buf,sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(env,
bufptr);{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(
bufptr);*tagptr=(unsigned int)tag;env=Cyc_Marshal_read_type_base(_tmp127,env,fp,(
void*)((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp128));}}}}else{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(
env,mem);}}goto _LLA3;}_LLB7: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){unsigned char nonnull=(unsigned char)Cyc_Marshal_cycgetchar(
fp);struct _tagged_arr s=_tag_arr(({unsigned int _tmp134=(unsigned int)100;
unsigned char*_tmp135=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp134));{unsigned int _tmp136=_tmp134;unsigned int i;for(i=0;i < 
_tmp136;i ++){_tmp135[i]='\000';}};_tmp135;}),sizeof(unsigned char),(unsigned int)
100);struct _tagged_arr xtname=_tagged_arr_plus(s,sizeof(unsigned char),4);Cyc_Marshal_fscanntstring(
fp,xtname);if((int)nonnull){void*_tmp12E;int _tmp12F;struct _tuple4 _tmp12D=Cyc_Typerep_get_xtunionbranch((
struct _tagged_arr)xtname,_tmp110);_LLDB: _tmp12F=_tmp12D.f1;goto _LLDA;_LLDA:
_tmp12E=_tmp12D.f2;goto _LLD9;_LLD9: {struct _tagged_arr buf=({unsigned int _tmp130=
_tmp12F + Cyc_Typerep_size_type(_tmp12E);unsigned char*_tmp131=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp130));struct _tagged_arr
_tmp133=_tag_arr(_tmp131,sizeof(unsigned char),_tmp12F + Cyc_Typerep_size_type(
_tmp12E));{unsigned int _tmp132=_tmp130;unsigned int i;for(i=0;i < _tmp132;i ++){
_tmp131[i]='\000';}};_tmp133;});unsigned int bufptr=(unsigned int)((unsigned char*)
_check_null(_untag_arr(buf,sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(
env,bufptr);{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(
bufptr);*tagptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(s,sizeof(
unsigned char),1)));env=Cyc_Marshal_read_type_base(_tmp12E,env,fp,(void*)((void*(*)(
unsigned int x))unsafe_cast)(bufptr + _tmp12F));}}}}else{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=(unsigned int)((unsigned char*)
_check_null(_untag_arr(s,sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(
env,*x);}}else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
Cyc_Marshal_table_lookup(env,mem);}goto _LLA3;}_LLB9: env=Cyc_Marshal_read_type_base((
void*)({struct Cyc_Typerep_Tuple_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Typerep_Tuple_struct _tmp138;_tmp138.tag=4;_tmp138.f1=(
unsigned int)_tmp111;_tmp138.f2=({unsigned int _tmp139=(unsigned int)_tmp111;
struct _tuple1**_tmp13A=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct
_tuple1*),_tmp139));struct _tagged_arr _tmp13F=_tag_arr(_tmp13A,sizeof(struct
_tuple1*),(unsigned int)_tmp111);{unsigned int _tmp13B=_tmp139;unsigned int i;for(
i=0;i < _tmp13B;i ++){_tmp13A[i]=({struct _tuple1*_tmp13C=_cycalloc(sizeof(*_tmp13C));
_tmp13C->f1=i;_tmp13C->f2=(void*)({struct Cyc_Typerep_Int_struct*_tmp13D=
_cycalloc_atomic(sizeof(*_tmp13D));_tmp13D[0]=({struct Cyc_Typerep_Int_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=0;_tmp13E.f2=8;_tmp13E;});_tmp13D;});_tmp13C;});}};
_tmp13F;});_tmp138;});_tmp137;}),env,fp,result);goto _LLA3;_LLBB: env=Cyc_Marshal_read_type_base((
void*)({struct Cyc_Typerep_Int_struct*_tmp140=_cycalloc_atomic(sizeof(*_tmp140));
_tmp140[0]=({struct Cyc_Typerep_Int_struct _tmp141;_tmp141.tag=0;_tmp141.f1=0;
_tmp141.f2=(unsigned int)(_tmp112 << 3);_tmp141;});_tmp140;}),env,fp,result);
_LLA3:;}return env;}void*Cyc_Marshal_read_type(void*ts,struct Cyc_Std___cycFILE*fp){
struct _handler_cons _tmp142;_push_handler(& _tmp142);{int _tmp144=0;if(setjmp(
_tmp142.handler)){_tmp144=1;}if(! _tmp144){{struct _tagged_arr buf=({unsigned int
_tmp146=Cyc_Typerep_size_type(ts);unsigned char*_tmp147=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp146));struct _tagged_arr
_tmp149=_tag_arr(_tmp147,sizeof(unsigned char),Cyc_Typerep_size_type(ts));{
unsigned int _tmp148=_tmp146;unsigned int i;for(i=0;i < _tmp148;i ++){_tmp147[i]='\000';}};
_tmp149;});void*result=((void*(*)(unsigned char*x))unsafe_cast)((unsigned char*)
_check_null(_untag_arr(buf,sizeof(unsigned char),1)));Cyc_Marshal_read_type_base(
ts,Cyc_Marshal_empty_addr_table(),fp,result);{void*_tmp145=result;_npop_handler(
0);return _tmp145;}};_pop_handler();}else{void*_tmp143=(void*)_exn_thrown;void*
_tmp14B=_tmp143;struct _tagged_arr _tmp14C;_LLDD: if(*((void**)_tmp14B)== Cyc_Core_Failure){
_LLE1: _tmp14C=((struct Cyc_Core_Failure_struct*)_tmp14B)->f1;goto _LLDE;}else{goto
_LLDF;}_LLDF: goto _LLE0;_LLDE:({struct Cyc_Std_String_pa_struct _tmp14E;_tmp14E.tag=
0;_tmp14E.f1=(struct _tagged_arr)_tmp14C;{void*_tmp14D[1]={& _tmp14E};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(
_tmp14D,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=({struct Cyc_Core_Failure_struct
_tmp150;_tmp150.tag=Cyc_Core_Failure;_tmp150.f1=_tmp14C;_tmp150;});_tmp14F;}));
_LLE0:(void)_throw(_tmp14B);_LLDC:;}}}
