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
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct
_tuple3{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;
struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;
struct _tagged_arr*f1;struct _tagged_arr f2;};struct _tuple4{int f1;void*f2;};extern
struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct _tagged_arr l);
extern struct _tagged_arr Cyc_Typerep_get_tagname(unsigned int tag,struct _tagged_arr
l);extern struct _tagged_arr Cyc_Typerep_get_tagname2(unsigned int tag,struct
_tagged_arr l);extern struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct _tagged_arr
tag,struct _tagged_arr l);unsigned int Cyc_Typerep_size_type(void*rep);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern int
Cyc_Std_fgetc(struct Cyc_Std___cycFILE*__stream);extern int Cyc_Std_fputc(int __c,
struct Cyc_Std___cycFILE*__stream);extern unsigned int Cyc_Std_fread(struct
_tagged_arr __ptr,unsigned int __size,unsigned int __n,struct Cyc_Std___cycFILE*
__stream);extern unsigned int Cyc_Std_fwrite(struct _tagged_arr __ptr,unsigned int
__size,unsigned int __n,struct Cyc_Std___cycFILE*__s);extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern int Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern int isprint(int);extern void Cyc_Marshal_print_type(void*
rep,void*val);extern void Cyc_Marshal_write_type(void*rep,struct Cyc_Std___cycFILE*
fp,void*val);extern void*Cyc_Marshal_read_type(void*rep,struct Cyc_Std___cycFILE*
fp);struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern
unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*
cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);extern void*unsafe_cast(void*x);struct _tuple5{struct Cyc_Dict_Dict*f1;
int f2;};struct _tuple6{struct _tagged_arr f1;int f2;};static int Cyc_Marshal_uint_cmp(
unsigned int a,unsigned int b){if(a == b){return 0;}else{if(a < b){return 1;}else{
return - 1;}}}struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set(){return((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(((struct Cyc_Set_Set*(*)(int(*
cmp)(unsigned int,unsigned int)))Cyc_Set_empty)(Cyc_Marshal_uint_cmp),0);}struct
_tuple7{int f1;struct Cyc_Set_Set*f2;};struct _tuple7*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val){return({struct _tuple7*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0->f1=((int(*)(struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_member)(
set,val);_tmp0->f2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,unsigned int elt))
Cyc_Set_insert)(set,val);_tmp0;});}inline int Cyc_Marshal_index_member(struct
_tuple5 idx,unsigned int m){int _tmp2;struct _tuple5 _tmp1=idx;_LL1: _tmp2=_tmp1.f2;
goto _LL0;_LL0: return m < _tmp2;}struct _tuple5 Cyc_Marshal_empty_addr_index(){return({
struct _tuple5 _tmp3;_tmp3.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);_tmp3.f2=
1;_tmp3;});}int Cyc_Marshal_index_lookup(struct _tuple5 index,unsigned int val){int
_tmp5;struct Cyc_Dict_Dict*_tmp6;struct _tuple5 _tmp4=index;_LL4: _tmp6=_tmp4.f1;
goto _LL3;_LL3: _tmp5=_tmp4.f2;goto _LL2;_LL2: {struct Cyc_Core_Opt*_tmp7=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup_opt)(_tmp6,
val);if(_tmp7 != 0){return(int)_tmp7->v;}else{return _tmp5;}}}struct _tuple5 Cyc_Marshal_index_insert(
struct _tuple5 index,unsigned int val){int _tmp9;struct Cyc_Dict_Dict*_tmpA;struct
_tuple5 _tmp8=index;_LL7: _tmpA=_tmp8.f1;goto _LL6;_LL6: _tmp9=_tmp8.f2;goto _LL5;
_LL5: return({struct _tuple5 _tmpB;_tmpB.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,unsigned int k,int v))Cyc_Dict_insert)(_tmpA,val,_tmp9);_tmpB.f2=_tmp9 + 1;_tmpB;});}
struct _tuple6 Cyc_Marshal_empty_addr_table(){return({struct _tuple6 _tmpC;_tmpC.f1=
_tag_arr(({unsigned int _tmpD=(unsigned int)1;unsigned int*_tmpE=(unsigned int*)
_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmpD));{unsigned int _tmpF=
_tmpD;unsigned int i;for(i=0;i < _tmpF;i ++){_tmpE[i]=0;}};_tmpE;}),sizeof(
unsigned int),(unsigned int)1);_tmpC.f2=1;_tmpC;});}inline int Cyc_Marshal_table_member(
struct _tuple6 table,unsigned int m){int _tmp11;struct _tuple6 _tmp10=table;_LL9:
_tmp11=_tmp10.f2;goto _LL8;_LL8: return m < _tmp11;}struct _tuple6 Cyc_Marshal_table_insert(
struct _tuple6 tbl,unsigned int i){int _tmp13;struct _tagged_arr _tmp14;struct _tuple6
_tmp12=tbl;_LLC: _tmp14=_tmp12.f1;goto _LLB;_LLB: _tmp13=_tmp12.f2;goto _LLA;_LLA:
if(_get_arr_size(_tmp14,sizeof(unsigned int))== _tmp13){_tmp14=({unsigned int
_tmp15=(unsigned int)(2 * _tmp13);unsigned int*_tmp16=(unsigned int*)
_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmp15));struct _tagged_arr
_tmp18=_tag_arr(_tmp16,sizeof(unsigned int),(unsigned int)(2 * _tmp13));{
unsigned int _tmp17=_tmp15;unsigned int j;for(j=0;j < _tmp17;j ++){_tmp16[j]=j < 
_tmp13?*((unsigned int*)_check_unknown_subscript(_tmp14,sizeof(unsigned int),(
int)j)): 0;}};_tmp18;});}((unsigned int*)_tmp14.curr)[_tmp13]=i;return({struct
_tuple6 _tmp19;_tmp19.f1=_tmp14;_tmp19.f2=_tmp13 + 1;_tmp19;});}unsigned int Cyc_Marshal_table_lookup(
struct _tuple6 tbl,int i){return*((unsigned int*)_check_unknown_subscript(tbl.f1,
sizeof(unsigned int),i));}void Cyc_Marshal_print_ref(unsigned int ref){if(ref == 0){({
void*_tmp1A[0]={};Cyc_Std_printf(_tag_arr("NULL",sizeof(unsigned char),5),
_tag_arr(_tmp1A,sizeof(void*),0));});}else{({struct Cyc_Std_Int_pa_struct _tmp1C;
_tmp1C.tag=1;_tmp1C.f1=ref;{void*_tmp1B[1]={& _tmp1C};Cyc_Std_printf(_tag_arr("(#%x)",
sizeof(unsigned char),6),_tag_arr(_tmp1B,sizeof(void*),1));}});}}struct _tuple8{
unsigned int f1;unsigned int f2;unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_print_type_base(
void*rep,struct Cyc_Set_Set*env,void*val){{void*_tmp1D=rep;unsigned int _tmp1E;int
_tmp1F;void*_tmp20;unsigned int _tmp21;void*_tmp22;struct _tagged_arr _tmp23;struct
_tagged_arr*_tmp24;struct _tagged_arr _tmp25;struct _tagged_arr _tmp26;struct
_tagged_arr _tmp27;struct _tagged_arr _tmp28;struct _tagged_arr _tmp29;struct
_tagged_arr _tmp2A;struct _tagged_arr _tmp2B;struct _tagged_arr*_tmp2C;_LLE: if((
unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 0: 0){_LL23: _tmp1F=((struct Cyc_Typerep_Int_struct*)
_tmp1D)->f1;goto _LL22;_LL22: _tmp1E=((struct Cyc_Typerep_Int_struct*)_tmp1D)->f2;
goto _LLF;}else{goto _LL10;}_LL10: if((int)_tmp1D == 0){goto _LL11;}else{goto _LL12;}
_LL12: if((int)_tmp1D == 1){goto _LL13;}else{goto _LL14;}_LL14: if((unsigned int)
_tmp1D > 2?*((int*)_tmp1D)== 1: 0){_LL25: _tmp21=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1D)->f1;goto _LL24;_LL24: _tmp20=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1D)->f2;goto _LL15;}else{goto _LL16;}_LL16: if((unsigned int)_tmp1D > 2?*((int*)
_tmp1D)== 2: 0){_LL26: _tmp22=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp1D)->f1;
goto _LL17;}else{goto _LL18;}_LL18: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 3:
0){_LL28: _tmp24=((struct Cyc_Typerep_Struct_struct*)_tmp1D)->f1;goto _LL27;_LL27:
_tmp23=((struct Cyc_Typerep_Struct_struct*)_tmp1D)->f3;goto _LL19;}else{goto _LL1A;}
_LL1A: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 4: 0){_LL29: _tmp25=((struct Cyc_Typerep_Tuple_struct*)
_tmp1D)->f2;goto _LL1B;}else{goto _LL1C;}_LL1C: if((unsigned int)_tmp1D > 2?*((int*)
_tmp1D)== 5: 0){_LL2C: _tmp28=((struct Cyc_Typerep_TUnion_struct*)_tmp1D)->f1;goto
_LL2B;_LL2B: _tmp27=((struct Cyc_Typerep_TUnion_struct*)_tmp1D)->f2;goto _LL2A;
_LL2A: _tmp26=((struct Cyc_Typerep_TUnion_struct*)_tmp1D)->f3;goto _LL1D;}else{goto
_LL1E;}_LL1E: if((unsigned int)_tmp1D > 2?*((int*)_tmp1D)== 6: 0){_LL2E: _tmp2A=((
struct Cyc_Typerep_XTUnion_struct*)_tmp1D)->f1;goto _LL2D;_LL2D: _tmp29=((struct Cyc_Typerep_XTUnion_struct*)
_tmp1D)->f2;goto _LL1F;}else{goto _LL20;}_LL20: if((unsigned int)_tmp1D > 2?*((int*)
_tmp1D)== 7: 0){_LL30: _tmp2C=((struct Cyc_Typerep_Union_struct*)_tmp1D)->f1;goto
_LL2F;_LL2F: _tmp2B=((struct Cyc_Typerep_Union_struct*)_tmp1D)->f2;goto _LL21;}
else{goto _LLD;}_LLF: switch(_tmp1E){case 8: _LL31: {unsigned char*x=((unsigned char*(*)(
void*x))unsafe_cast)(val);if(isprint((int)*x)){({struct Cyc_Std_Int_pa_struct
_tmp2E;_tmp2E.tag=1;_tmp2E.f1=(int)((unsigned int)((int)*x));{void*_tmp2D[1]={&
_tmp2E};Cyc_Std_printf(_tag_arr("'%c'",sizeof(unsigned char),5),_tag_arr(_tmp2D,
sizeof(void*),1));}});}else{({struct Cyc_Std_Int_pa_struct _tmp30;_tmp30.tag=1;
_tmp30.f1=(int)((unsigned int)((int)*x));{void*_tmp2F[1]={& _tmp30};Cyc_Std_printf(
_tag_arr("'\\%d'",sizeof(unsigned char),6),_tag_arr(_tmp2F,sizeof(void*),1));}});}
break;}case 16: _LL32: {short*x=((short*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp32;_tmp32.tag=1;_tmp32.f1=(unsigned int)((int)*x);{void*_tmp31[1]={& _tmp32};
Cyc_Std_printf(_tmp1F? _tag_arr("%d",sizeof(unsigned char),3): _tag_arr("%u",
sizeof(unsigned char),3),_tag_arr(_tmp31,sizeof(void*),1));}});break;}case 32:
_LL33: {int*x=((int*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp34;_tmp34.tag=1;_tmp34.f1=(unsigned int)*x;{void*_tmp33[1]={& _tmp34};Cyc_Std_printf(
_tmp1F? _tag_arr("%d",sizeof(unsigned char),3): _tag_arr("%u",sizeof(unsigned char),
3),_tag_arr(_tmp33,sizeof(void*),1));}});break;}default: _LL34:({struct Cyc_Std_Int_pa_struct
_tmp36;_tmp36.tag=1;_tmp36.f1=(int)_tmp1E;{void*_tmp35[1]={& _tmp36};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("print_type_base: illegal int size %d bits\n",sizeof(
unsigned char),43),_tag_arr(_tmp35,sizeof(void*),1));}});exit(1);}goto _LLD;_LL11: {
float*x=((float*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp38;_tmp38.tag=2;_tmp38.f1=(double)*x;{void*_tmp37[1]={& _tmp38};Cyc_Std_printf(
_tag_arr("%f",sizeof(unsigned char),3),_tag_arr(_tmp37,sizeof(void*),1));}});
goto _LLD;}_LL13: {double*x=((double*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp3A;_tmp3A.tag=2;_tmp3A.f1=*x;{void*_tmp39[1]={& _tmp3A};Cyc_Std_printf(
_tag_arr("%f",sizeof(unsigned char),3),_tag_arr(_tmp39,sizeof(void*),1));}});
goto _LLD;}_LL15: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));struct _tuple7 _tmp3C;struct Cyc_Set_Set*_tmp3D;int _tmp3E;struct
_tuple7*_tmp3B=Cyc_Marshal_member_insert(env,(unsigned int)y);_tmp3C=*_tmp3B;
_LL38: _tmp3E=_tmp3C.f1;goto _LL37;_LL37: _tmp3D=_tmp3C.f2;goto _LL36;_LL36: env=
_tmp3D;if(! _tmp3E){({struct Cyc_Std_Int_pa_struct _tmp40;_tmp40.tag=1;_tmp40.f1=(
unsigned int)y;{void*_tmp3F[1]={& _tmp40};Cyc_Std_printf(_tag_arr("{(@%x) ",
sizeof(unsigned char),8),_tag_arr(_tmp3F,sizeof(void*),1));}});if(y != 0){int
typ_szb=(int)Cyc_Typerep_size_type(_tmp20);int i=0;for(0;i < _tmp21;i ++){env=Cyc_Marshal_print_type_base(
_tmp20,env,(void*)((void*(*)(int x))unsafe_cast)(y));if(i != _tmp21 - 1){({void*
_tmp41[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(
_tmp41,sizeof(void*),0));});}y +=typ_szb;}}else{({void*_tmp42[0]={};Cyc_Std_printf(
_tag_arr("NULL",sizeof(unsigned char),5),_tag_arr(_tmp42,sizeof(void*),0));});}({
void*_tmp43[0]={};Cyc_Std_printf(_tag_arr(" }",sizeof(unsigned char),3),_tag_arr(
_tmp43,sizeof(void*),0));});}else{Cyc_Marshal_print_ref((unsigned int)y);}goto
_LLD;}_LL17: {struct _tuple8*x=((struct _tuple8*(*)(void*x))unsafe_cast)(val);
unsigned int typ_szb=Cyc_Typerep_size_type(_tmp22);unsigned int base=(*x).f1;
unsigned int curr=(*x).f2;unsigned int last_plus_one=(*x).f3;struct _tuple7 _tmp45;
struct Cyc_Set_Set*_tmp46;int _tmp47;struct _tuple7*_tmp44=Cyc_Marshal_member_insert(
env,base);_tmp45=*_tmp44;_LL3B: _tmp47=_tmp45.f1;goto _LL3A;_LL3A: _tmp46=_tmp45.f2;
goto _LL39;_LL39: env=_tmp46;if(! _tmp47){({struct Cyc_Std_Int_pa_struct _tmp49;
_tmp49.tag=1;_tmp49.f1=base;{void*_tmp48[1]={& _tmp49};Cyc_Std_printf(_tag_arr("[(@%x) ",
sizeof(unsigned char),8),_tag_arr(_tmp48,sizeof(void*),1));}});{unsigned int i=
base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_print_type_base(_tmp22,
env,(void*)((void*(*)(unsigned int x))unsafe_cast)(i));if(i + typ_szb < 
last_plus_one){({void*_tmp4A[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(
unsigned char),3),_tag_arr(_tmp4A,sizeof(void*),0));});}}}({void*_tmp4B[0]={};
Cyc_Std_printf(_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp4B,sizeof(
void*),0));});}else{Cyc_Marshal_print_ref(base);}goto _LLD;}_LL19: if(_tmp24 == 0){({
void*_tmp4C[0]={};Cyc_Std_printf(_tag_arr("{ ",sizeof(unsigned char),3),_tag_arr(
_tmp4C,sizeof(void*),0));});}else{({struct Cyc_Std_String_pa_struct _tmp4E;_tmp4E.tag=
0;_tmp4E.f1=(struct _tagged_arr)*_tmp24;{void*_tmp4D[1]={& _tmp4E};Cyc_Std_printf(
_tag_arr("%s{ ",sizeof(unsigned char),5),_tag_arr(_tmp4D,sizeof(void*),1));}});}{
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmp23,
sizeof(struct _tuple0*));i ++){void*_tmp50;struct _tagged_arr _tmp51;unsigned int
_tmp52;struct _tuple0 _tmp4F=*((struct _tuple0**)_tmp23.curr)[i];_LL3F: _tmp52=
_tmp4F.f1;goto _LL3E;_LL3E: _tmp51=_tmp4F.f2;goto _LL3D;_LL3D: _tmp50=_tmp4F.f3;goto
_LL3C;_LL3C:({struct Cyc_Std_String_pa_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(
struct _tagged_arr)_tmp51;{void*_tmp53[1]={& _tmp54};Cyc_Std_printf(_tag_arr("%s=",
sizeof(unsigned char),4),_tag_arr(_tmp53,sizeof(void*),1));}});{int x_ofs=(int)(x
+ _tmp52);env=Cyc_Marshal_print_type_base(_tmp50,env,(void*)((void*(*)(int x))
unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp23,sizeof(struct _tuple0*))- 1){({
void*_tmp55[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(
_tmp55,sizeof(void*),0));});}}}}({void*_tmp56[0]={};Cyc_Std_printf(_tag_arr(" }",
sizeof(unsigned char),3),_tag_arr(_tmp56,sizeof(void*),0));});goto _LLD;}_LL1B:({
void*_tmp57[0]={};Cyc_Std_printf(_tag_arr("$( ",sizeof(unsigned char),4),
_tag_arr(_tmp57,sizeof(void*),0));});{int x=((int(*)(void*x))unsafe_cast)(val);{
int i=0;for(0;i < _get_arr_size(_tmp25,sizeof(struct _tuple1*));i ++){void*_tmp59;
unsigned int _tmp5A;struct _tuple1 _tmp58=*((struct _tuple1**)_tmp25.curr)[i];_LL42:
_tmp5A=_tmp58.f1;goto _LL41;_LL41: _tmp59=_tmp58.f2;goto _LL40;_LL40: {int x_ofs=(
int)(x + _tmp5A);env=Cyc_Marshal_print_type_base(_tmp59,env,(void*)((void*(*)(int
x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp25,sizeof(struct _tuple1*))- 1){({
void*_tmp5B[0]={};Cyc_Std_printf(_tag_arr(", ",sizeof(unsigned char),3),_tag_arr(
_tmp5B,sizeof(void*),0));});}}}}({void*_tmp5C[0]={};Cyc_Std_printf(_tag_arr(" )",
sizeof(unsigned char),3),_tag_arr(_tmp5C,sizeof(void*),0));});goto _LLD;}_LL1D: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024){({
struct Cyc_Std_String_pa_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _tagged_arr)
Cyc_Typerep_get_tagname(*tagp,_tmp27);{void*_tmp5D[1]={& _tmp5E};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp5D,sizeof(void*),1));}});}
else{struct _tuple7 _tmp60;struct Cyc_Set_Set*_tmp61;int _tmp62;struct _tuple7*_tmp5F=
Cyc_Marshal_member_insert(env,*tagp);_tmp60=*_tmp5F;_LL45: _tmp62=_tmp60.f1;goto
_LL44;_LL44: _tmp61=_tmp60.f2;goto _LL43;_LL43: env=_tmp61;if(! _tmp62){({struct Cyc_Std_Int_pa_struct
_tmp64;_tmp64.tag=1;_tmp64.f1=*tagp;{void*_tmp63[1]={& _tmp64};Cyc_Std_printf(
_tag_arr("(@%x)",sizeof(unsigned char),6),_tag_arr(_tmp63,sizeof(void*),1));}});{
unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);
unsigned int tag=*x;void*_tmp66;int _tmp67;struct _tuple4 _tmp65=Cyc_Typerep_get_unionbranch(
tag,_tmp26);_LL48: _tmp67=_tmp65.f1;goto _LL47;_LL47: _tmp66=_tmp65.f2;goto _LL46;
_LL46: {void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*tagp + _tmp67);({struct
Cyc_Std_String_pa_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname2(
tag,_tmp26);{void*_tmp68[1]={& _tmp69};Cyc_Std_printf(_tag_arr("&%s[",sizeof(
unsigned char),5),_tag_arr(_tmp68,sizeof(void*),1));}});env=Cyc_Marshal_print_type_base(
_tmp66,env,xoff);({void*_tmp6A[0]={};Cyc_Std_printf(_tag_arr("]",sizeof(
unsigned char),2),_tag_arr(_tmp6A,sizeof(void*),0));});}}}else{Cyc_Marshal_print_ref(*
tagp);}}goto _LLD;}_LL1F: {unsigned int*xtunionp=((unsigned int*(*)(void*x))
unsafe_cast)(val);struct _tuple7 _tmp6C;struct Cyc_Set_Set*_tmp6D;int _tmp6E;struct
_tuple7*_tmp6B=Cyc_Marshal_member_insert(env,*xtunionp);_tmp6C=*_tmp6B;_LL4B:
_tmp6E=_tmp6C.f1;goto _LL4A;_LL4A: _tmp6D=_tmp6C.f2;goto _LL49;_LL49: env=_tmp6D;if(
! _tmp6E){({struct Cyc_Std_Int_pa_struct _tmp70;_tmp70.tag=1;_tmp70.f1=*xtunionp;{
void*_tmp6F[1]={& _tmp70};Cyc_Std_printf(_tag_arr("(@%x)",sizeof(unsigned char),6),
_tag_arr(_tmp6F,sizeof(void*),1));}});{unsigned int*xtstructp=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){unsigned char*xtnamec=((
unsigned char*(*)(unsigned int x))unsafe_cast)(*xtstructp + 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmp72;_tmp72.tag=0;_tmp72.f1=(struct _tagged_arr)xtname;{void*_tmp71[1]={& _tmp72};
Cyc_Std_printf(_tag_arr("&%s[( ",sizeof(unsigned char),7),_tag_arr(_tmp71,
sizeof(void*),1));}});{void*_tmp74;int _tmp75;struct _tuple4 _tmp73=Cyc_Typerep_get_xtunionbranch(
xtname,_tmp29);_LL4E: _tmp75=_tmp73.f1;goto _LL4D;_LL4D: _tmp74=_tmp73.f2;goto _LL4C;
_LL4C: {void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*xtunionp + _tmp75);env=
Cyc_Marshal_print_type_base(_tmp74,env,xoff);({void*_tmp76[0]={};Cyc_Std_printf(
_tag_arr(" )]",sizeof(unsigned char),4),_tag_arr(_tmp76,sizeof(void*),0));});}}}
else{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtunionp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(struct
_tagged_arr)xtname;{void*_tmp77[1]={& _tmp78};Cyc_Std_printf(_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmp77,sizeof(void*),1));}});}}}else{Cyc_Marshal_print_ref(*
xtunionp);}goto _LLD;}_LL21: if(_tmp2C == 0){({void*_tmp79[0]={};Cyc_Std_printf(
_tag_arr("{",sizeof(unsigned char),2),_tag_arr(_tmp79,sizeof(void*),0));});}
else{({struct Cyc_Std_String_pa_struct _tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct
_tagged_arr)*_tmp2C;{void*_tmp7A[1]={& _tmp7B};Cyc_Std_printf(_tag_arr("%s{",
sizeof(unsigned char),4),_tag_arr(_tmp7A,sizeof(void*),1));}});}{int i=0;for(0;i < 
_get_arr_size(_tmp2B,sizeof(struct _tuple3*));i ++){void*_tmp7D;struct _tagged_arr
_tmp7E;struct _tuple3 _tmp7C=*((struct _tuple3**)_tmp2B.curr)[i];_LL51: _tmp7E=
_tmp7C.f1;goto _LL50;_LL50: _tmp7D=_tmp7C.f2;goto _LL4F;_LL4F:({struct Cyc_Std_String_pa_struct
_tmp80;_tmp80.tag=0;_tmp80.f1=(struct _tagged_arr)_tmp7E;{void*_tmp7F[1]={& _tmp80};
Cyc_Std_printf(_tag_arr("%s=",sizeof(unsigned char),4),_tag_arr(_tmp7F,sizeof(
void*),1));}});Cyc_Marshal_print_type_base(_tmp7D,env,val);if(i < _get_arr_size(
_tmp2B,sizeof(struct _tuple3*))- 1){({void*_tmp81[0]={};Cyc_Std_printf(_tag_arr(" | ",
sizeof(unsigned char),4),_tag_arr(_tmp81,sizeof(void*),0));});}}}({void*_tmp82[0]={};
Cyc_Std_printf(_tag_arr("}",sizeof(unsigned char),2),_tag_arr(_tmp82,sizeof(void*),
0));});goto _LLD;_LLD:;}return env;}void Cyc_Marshal_print_type(void*ts,void*val){
struct _handler_cons _tmp83;_push_handler(& _tmp83);{int _tmp85=0;if(setjmp(_tmp83.handler)){
_tmp85=1;}if(! _tmp85){Cyc_Marshal_print_type_base(ts,Cyc_Marshal_empty_addr_set(),
val);;_pop_handler();}else{void*_tmp84=(void*)_exn_thrown;void*_tmp87=_tmp84;
struct _tagged_arr _tmp88;_LL53: if(*((void**)_tmp87)== Cyc_Core_Failure){_LL57:
_tmp88=((struct Cyc_Core_Failure_struct*)_tmp87)->f1;goto _LL54;}else{goto _LL55;}
_LL55: goto _LL56;_LL54:({struct Cyc_Std_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(
struct _tagged_arr)_tmp88;{void*_tmp89[1]={& _tmp8A};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(_tmp89,sizeof(
void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp8B=
_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Core_Failure_struct _tmp8C;
_tmp8C.tag=Cyc_Core_Failure;_tmp8C.f1=_tmp88;_tmp8C;});_tmp8B;}));_LL56:(void)
_throw(_tmp87);_LL52:;}}}void Cyc_Marshal_cycputchar(unsigned char x,struct Cyc_Std___cycFILE*
fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(
unsigned char),1)),sizeof(unsigned char),1,fp)< 1){(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Core_Failure_struct
_tmp8E;_tmp8E.tag=Cyc_Core_Failure;_tmp8E.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmp8E;});_tmp8D;}));}}void Cyc_Marshal_cycputshort(short x,
struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(short),1)),sizeof(short),1,fp)< 1){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({
struct Cyc_Core_Failure_struct _tmp90;_tmp90.tag=Cyc_Core_Failure;_tmp90.f1=
_tag_arr("Write failure",sizeof(unsigned char),14);_tmp90;});_tmp8F;}));}}void
Cyc_Marshal_cycputint(int x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct
_tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(int),1)),sizeof(int),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp91=_cycalloc(sizeof(*
_tmp91));_tmp91[0]=({struct Cyc_Core_Failure_struct _tmp92;_tmp92.tag=Cyc_Core_Failure;
_tmp92.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmp92;});_tmp91;}));}}
void Cyc_Marshal_cycputfloat(float x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(float),1)),sizeof(float),
1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp93=_cycalloc(
sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Core_Failure_struct _tmp94;_tmp94.tag=Cyc_Core_Failure;
_tmp94.f1=_tag_arr("Write failure",sizeof(unsigned char),14);_tmp94;});_tmp93;}));}}
void Cyc_Marshal_cycputdouble(double x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((
struct _tagged_arr)((struct _tagged_arr)_tag_arr(& x,sizeof(double),1)),sizeof(
double),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp95=
_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Core_Failure_struct _tmp96;
_tmp96.tag=Cyc_Core_Failure;_tmp96.f1=_tag_arr("Write failure",sizeof(
unsigned char),14);_tmp96;});_tmp95;}));}}void Cyc_Marshal_cycputvarint(
unsigned int i,struct Cyc_Std___cycFILE*fp){int width=0;unsigned int mask=-256;
while((int)mask){if((int)(i & mask)){width ++;}mask <<=8;}Cyc_Std_fputc((int)(width
<< 6 | i & 63),fp);i >>=6;{int j=0;for(0;j < width;j ++){Cyc_Std_fputc((int)(i & 255),
fp);i >>=8;}}}unsigned char Cyc_Marshal_cycgetchar(struct Cyc_Std___cycFILE*fp){
struct _RegionHandle _tmp97=_new_region("r");struct _RegionHandle*r=& _tmp97;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmp9B=sizeof(
unsigned char);unsigned char*_tmp9C=(unsigned char*)_region_malloc(r,
_check_times(sizeof(unsigned char),_tmp9B));{unsigned int _tmp9D=_tmp9B;
unsigned int i;for(i=0;i < _tmp9D;i ++){_tmp9C[i]='\000';}};_tmp9C;}),sizeof(
unsigned char),sizeof(unsigned char));if(Cyc_Std_fread(x,sizeof(unsigned char),1,
fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Core_Failure_struct _tmp99;_tmp99.tag=Cyc_Core_Failure;
_tmp99.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmp99;});_tmp98;}));}{
unsigned char*xx=((unsigned char*(*)(unsigned char*x))unsafe_cast)((
unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));unsigned char
_tmp9A=*xx;_npop_handler(0);return _tmp9A;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmp9E=_new_region("r");struct
_RegionHandle*r=& _tmp9E;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpA2=sizeof(short);unsigned char*_tmpA3=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmpA2));{unsigned int _tmpA4=
_tmpA2;unsigned int i;for(i=0;i < _tmpA4;i ++){_tmpA3[i]='\000';}};_tmpA3;}),
sizeof(unsigned char),sizeof(short));if(Cyc_Std_fread(x,sizeof(short),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F[0]=({struct Cyc_Core_Failure_struct _tmpA0;_tmpA0.tag=Cyc_Core_Failure;
_tmpA0.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmpA0;});_tmp9F;}));}{
short*xx=((short*(*)(unsigned char*x))unsafe_cast)((unsigned char*)_check_null(
_untag_arr(x,sizeof(unsigned char),1)));short _tmpA1=*xx;_npop_handler(0);return
_tmpA1;}};_pop_region(r);}int Cyc_Marshal_cycgetint(struct Cyc_Std___cycFILE*fp){
struct _RegionHandle _tmpA5=_new_region("r");struct _RegionHandle*r=& _tmpA5;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpA9=sizeof(int);
unsigned char*_tmpAA=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmpA9));{unsigned int _tmpAB=_tmpA9;unsigned int i;for(i=0;i < 
_tmpAB;i ++){_tmpAA[i]='\000';}};_tmpAA;}),sizeof(unsigned char),sizeof(int));if(
Cyc_Std_fread(x,sizeof(int),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Core_Failure_struct
_tmpA7;_tmpA7.tag=Cyc_Core_Failure;_tmpA7.f1=_tag_arr("Read failure",sizeof(
unsigned char),13);_tmpA7;});_tmpA6;}));}{int*xx=((int*(*)(unsigned char*x))
unsafe_cast)((unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));
int _tmpA8=*xx;_npop_handler(0);return _tmpA8;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpAC=_new_region("r");struct
_RegionHandle*r=& _tmpAC;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpB0=sizeof(float);unsigned char*_tmpB1=(unsigned char*)
_region_malloc(r,_check_times(sizeof(unsigned char),_tmpB0));{unsigned int _tmpB2=
_tmpB0;unsigned int i;for(i=0;i < _tmpB2;i ++){_tmpB1[i]='\000';}};_tmpB1;}),
sizeof(unsigned char),sizeof(float));if(Cyc_Std_fread(x,sizeof(float),1,fp)< 1){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD[0]=({struct Cyc_Core_Failure_struct _tmpAE;_tmpAE.tag=Cyc_Core_Failure;
_tmpAE.f1=_tag_arr("Read failure",sizeof(unsigned char),13);_tmpAE;});_tmpAD;}));}{
float*xx=((float*(*)(unsigned char*x))unsafe_cast)((unsigned char*)_check_null(
_untag_arr(x,sizeof(unsigned char),1)));float _tmpAF=*xx;_npop_handler(0);return
_tmpAF;}};_pop_region(r);}double Cyc_Marshal_cycgetdouble(struct Cyc_Std___cycFILE*
fp){struct _RegionHandle _tmpB3=_new_region("r");struct _RegionHandle*r=& _tmpB3;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpB7=sizeof(double);
unsigned char*_tmpB8=(unsigned char*)_region_malloc(r,_check_times(sizeof(
unsigned char),_tmpB7));{unsigned int _tmpB9=_tmpB7;unsigned int i;for(i=0;i < 
_tmpB9;i ++){_tmpB8[i]='\000';}};_tmpB8;}),sizeof(unsigned char),sizeof(double));
if(Cyc_Std_fread(x,sizeof(double),1,fp)< 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Core_Failure_struct
_tmpB5;_tmpB5.tag=Cyc_Core_Failure;_tmpB5.f1=_tag_arr("Read failure",sizeof(
unsigned char),13);_tmpB5;});_tmpB4;}));}{double*xx=((double*(*)(unsigned char*x))
unsafe_cast)((unsigned char*)_check_null(_untag_arr(x,sizeof(unsigned char),1)));
double _tmpB6=*xx;_npop_handler(0);return _tmpB6;}};_pop_region(r);}unsigned int
Cyc_Marshal_cycgetvarint(struct Cyc_Std___cycFILE*fp){int i=0;int j=(int)Cyc_Marshal_cycgetchar(
fp);int width=(j & 192)>> 6;int pos=6;i +=j & 63;{int k=0;for(0;k < width;k ++){j=(int)
Cyc_Marshal_cycgetchar(fp);i |=j << pos;pos +=8;}}return(unsigned int)i;}struct
_tuple5 Cyc_Marshal_write_type_base(void*rep,struct _tuple5 env,struct Cyc_Std___cycFILE*
fp,void*val){{void*_tmpBA=rep;unsigned int _tmpBB;void*_tmpBC;unsigned int _tmpBD;
void*_tmpBE;struct _tagged_arr _tmpBF;struct _tagged_arr _tmpC0;struct _tagged_arr
_tmpC1;struct _tagged_arr _tmpC2;struct _tagged_arr _tmpC3;_LL59: if((unsigned int)
_tmpBA > 2?*((int*)_tmpBA)== 0: 0){_LL6D: _tmpBB=((struct Cyc_Typerep_Int_struct*)
_tmpBA)->f2;goto _LL5A;}else{goto _LL5B;}_LL5B: if((int)_tmpBA == 0){goto _LL5C;}
else{goto _LL5D;}_LL5D: if((int)_tmpBA == 1){goto _LL5E;}else{goto _LL5F;}_LL5F: if((
unsigned int)_tmpBA > 2?*((int*)_tmpBA)== 1: 0){_LL6F: _tmpBD=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpBA)->f1;goto _LL6E;_LL6E: _tmpBC=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmpBA)->f2;goto _LL60;}else{goto _LL61;}_LL61: if((unsigned int)_tmpBA > 2?*((int*)
_tmpBA)== 2: 0){_LL70: _tmpBE=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpBA)->f1;
goto _LL62;}else{goto _LL63;}_LL63: if((unsigned int)_tmpBA > 2?*((int*)_tmpBA)== 3:
0){_LL71: _tmpBF=((struct Cyc_Typerep_Struct_struct*)_tmpBA)->f3;goto _LL64;}else{
goto _LL65;}_LL65: if((unsigned int)_tmpBA > 2?*((int*)_tmpBA)== 4: 0){_LL72: _tmpC0=((
struct Cyc_Typerep_Tuple_struct*)_tmpBA)->f2;goto _LL66;}else{goto _LL67;}_LL67: if((
unsigned int)_tmpBA > 2?*((int*)_tmpBA)== 5: 0){_LL74: _tmpC2=((struct Cyc_Typerep_TUnion_struct*)
_tmpBA)->f2;goto _LL73;_LL73: _tmpC1=((struct Cyc_Typerep_TUnion_struct*)_tmpBA)->f3;
goto _LL68;}else{goto _LL69;}_LL69: if((unsigned int)_tmpBA > 2?*((int*)_tmpBA)== 6:
0){_LL75: _tmpC3=((struct Cyc_Typerep_XTUnion_struct*)_tmpBA)->f2;goto _LL6A;}else{
goto _LL6B;}_LL6B: if((unsigned int)_tmpBA > 2?*((int*)_tmpBA)== 7: 0){goto _LL6C;}
else{goto _LL58;}_LL5A: switch(_tmpBB){case 8: _LL76: {unsigned char*x=((
unsigned char*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputchar(*x,fp);break;}
case 16: _LL77: {short*x=((short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*
x,fp);break;}case 32: _LL78: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*
x,fp);break;}default: _LL79:({struct Cyc_Std_Int_pa_struct _tmpC5;_tmpC5.tag=1;
_tmpC5.f1=(int)_tmpBB;{void*_tmpC4[1]={& _tmpC5};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("write_type_base: illegal int size %d bits\n",sizeof(unsigned char),43),
_tag_arr(_tmpC4,sizeof(void*),1));}});exit(1);}goto _LL58;_LL5C: {float*x=((float*(*)(
void*x))unsafe_cast)(val);Cyc_Marshal_cycputfloat(*x,fp);goto _LL58;}_LL5E: {
double*x=((double*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*x,fp);
goto _LL58;}_LL60: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));int _tmpC6=Cyc_Marshal_index_lookup(env,(unsigned int)y);Cyc_Marshal_cycputvarint((
unsigned int)_tmpC6,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmpC6)){
env=Cyc_Marshal_index_insert(env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmpBC);int i=0;for(0;i < _tmpBD;i ++){env=Cyc_Marshal_write_type_base(_tmpBC,env,
fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}goto _LL58;}_LL62: {
struct _tuple8*x=((struct _tuple8*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmpBE);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;unsigned int pos=(curr - base)/ typ_szb;
unsigned int sz=(last_plus_one - base)/ typ_szb;int _tmpC7=Cyc_Marshal_index_lookup(
env,base);Cyc_Marshal_cycputvarint((unsigned int)_tmpC7,fp);Cyc_Marshal_cycputvarint(
pos,fp);Cyc_Marshal_cycputvarint(sz,fp);if(! Cyc_Marshal_index_member(env,(
unsigned int)_tmpC7)){env=Cyc_Marshal_index_insert(env,base);{unsigned int i=base;
for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(_tmpBE,env,fp,(
void*)((void*(*)(unsigned int x))unsafe_cast)(i));}}}goto _LL58;}_LL64: {int x=((
int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpBF,sizeof(
struct _tuple0*));i ++){void*_tmpC9;unsigned int _tmpCA;struct _tuple0 _tmpC8=*((
struct _tuple0**)_tmpBF.curr)[i];_LL7D: _tmpCA=_tmpC8.f1;goto _LL7C;_LL7C: _tmpC9=
_tmpC8.f3;goto _LL7B;_LL7B: {int x_ofs=(int)(x + _tmpCA);env=Cyc_Marshal_write_type_base(
_tmpC9,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL58;}_LL66: {
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpC0,
sizeof(struct _tuple1*));i ++){void*_tmpCC;unsigned int _tmpCD;struct _tuple1 _tmpCB=*((
struct _tuple1**)_tmpC0.curr)[i];_LL80: _tmpCD=_tmpCB.f1;goto _LL7F;_LL7F: _tmpCC=
_tmpCB.f2;goto _LL7E;_LL7E: {int x_ofs=(int)(x + _tmpCD);env=Cyc_Marshal_write_type_base(
_tmpCC,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL58;}_LL68: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024){
Cyc_Marshal_cycputvarint(*tagp,fp);}else{int _tmpCE=Cyc_Marshal_index_lookup(env,*
tagp);Cyc_Marshal_cycputvarint((unsigned int)(_tmpCE + 1024),fp);if(! Cyc_Marshal_index_member(
env,(unsigned int)_tmpCE)){env=Cyc_Marshal_index_insert(env,*tagp);{unsigned int*
x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;Cyc_Marshal_cycputvarint(
tag,fp);{void*_tmpD0;int _tmpD1;struct _tuple4 _tmpCF=Cyc_Typerep_get_unionbranch(
tag,_tmpC1);_LL83: _tmpD1=_tmpCF.f1;goto _LL82;_LL82: _tmpD0=_tmpCF.f2;goto _LL81;
_LL81: {void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*tagp + _tmpD1);env=Cyc_Marshal_write_type_base(
_tmpD0,env,fp,xoff);}}}}}goto _LL58;}_LL6A: {unsigned int*xtunionp=((unsigned int*(*)(
void*x))unsafe_cast)(val);int _tmpD2=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint((
unsigned int)_tmpD2,fp);if(! Cyc_Marshal_index_member(env,(unsigned int)_tmpD2)){
env=Cyc_Marshal_index_insert(env,*xtunionp);{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',
fp);{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtstructp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmpD4;_tmpD4.tag=0;_tmpD4.f1=(struct
_tagged_arr)xtname;{void*_tmpD3[1]={& _tmpD4};Cyc_Std_fprintf(fp,_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpD3,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',
fp);{void*_tmpD6;int _tmpD7;struct _tuple4 _tmpD5=Cyc_Typerep_get_xtunionbranch(
xtname,_tmpC3);_LL86: _tmpD7=_tmpD5.f1;goto _LL85;_LL85: _tmpD6=_tmpD5.f2;goto _LL84;
_LL84: {void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*xtunionp + _tmpD7);env=
Cyc_Marshal_write_type_base(_tmpD6,env,fp,xoff);}}}}else{Cyc_Marshal_cycputchar('\000',
fp);{unsigned char*xtnamec=((unsigned char*(*)(unsigned int x))unsafe_cast)(*
xtunionp + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct
_tagged_arr)xtname;{void*_tmpD8[1]={& _tmpD9};Cyc_Std_fprintf(fp,_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpD8,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',
fp);}}}}goto _LL58;}_LL6C: {int sz=(int)Cyc_Typerep_size_type(rep);env=Cyc_Marshal_write_type_base((
void*)({struct Cyc_Typerep_Tuple_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[
0]=({struct Cyc_Typerep_Tuple_struct _tmpDB;_tmpDB.tag=4;_tmpDB.f1=(unsigned int)
sz;_tmpDB.f2=({unsigned int _tmpDC=(unsigned int)sz;struct _tuple1**_tmpDD=(struct
_tuple1**)_cycalloc(_check_times(sizeof(struct _tuple1*),_tmpDC));struct
_tagged_arr _tmpE2=_tag_arr(_tmpDD,sizeof(struct _tuple1*),(unsigned int)sz);{
unsigned int _tmpDE=_tmpDC;unsigned int i;for(i=0;i < _tmpDE;i ++){_tmpDD[i]=({
struct _tuple1*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->f1=i;_tmpDF->f2=(void*)({
struct Cyc_Typerep_Int_struct*_tmpE0=_cycalloc_atomic(sizeof(*_tmpE0));_tmpE0[0]=({
struct Cyc_Typerep_Int_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=0;_tmpE1.f2=8;_tmpE1;});
_tmpE0;});_tmpDF;});}};_tmpE2;});_tmpDB;});_tmpDA;}),env,fp,val);goto _LL58;}
_LL58:;}return env;}void Cyc_Marshal_write_type(void*ts,struct Cyc_Std___cycFILE*fp,
void*val){struct _handler_cons _tmpE3;_push_handler(& _tmpE3);{int _tmpE5=0;if(
setjmp(_tmpE3.handler)){_tmpE5=1;}if(! _tmpE5){Cyc_Marshal_write_type_base(ts,Cyc_Marshal_empty_addr_index(),
fp,val);;_pop_handler();}else{void*_tmpE4=(void*)_exn_thrown;void*_tmpE7=_tmpE4;
struct _tagged_arr _tmpE8;_LL88: if(*((void**)_tmpE7)== Cyc_Core_Failure){_LL8C:
_tmpE8=((struct Cyc_Core_Failure_struct*)_tmpE7)->f1;goto _LL89;}else{goto _LL8A;}
_LL8A: goto _LL8B;_LL89:({struct Cyc_Std_String_pa_struct _tmpEA;_tmpEA.tag=0;_tmpEA.f1=(
struct _tagged_arr)_tmpE8;{void*_tmpE9[1]={& _tmpEA};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(_tmpE9,sizeof(
void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpEB=
_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Core_Failure_struct _tmpEC;
_tmpEC.tag=Cyc_Core_Failure;_tmpEC.f1=_tmpE8;_tmpEC;});_tmpEB;}));_LL8B:(void)
_throw(_tmpE7);_LL87:;}}}void Cyc_Marshal_fscanntstring(struct Cyc_Std___cycFILE*
fp,struct _tagged_arr s){int c;int i=0;while(c=Cyc_Std_fgetc(fp)){if(c == - 1){(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpED=_cycalloc(sizeof(*_tmpED));
_tmpED[0]=({struct Cyc_Core_Failure_struct _tmpEE;_tmpEE.tag=Cyc_Core_Failure;
_tmpEE.f1=_tag_arr("Read error",sizeof(unsigned char),11);_tmpEE;});_tmpED;}));}((
unsigned char*)s.curr)[i]=(unsigned char)c;i ++;}((unsigned char*)s.curr)[i]='\000';}
struct _tuple6 Cyc_Marshal_read_type_base(void*rep,struct _tuple6 env,struct Cyc_Std___cycFILE*
fp,void*result){{void*_tmpEF=rep;unsigned int _tmpF0;void*_tmpF1;unsigned int
_tmpF2;void*_tmpF3;struct _tagged_arr _tmpF4;struct _tagged_arr _tmpF5;struct
_tagged_arr _tmpF6;struct _tagged_arr _tmpF7;struct _tagged_arr _tmpF8;_LL8E: if((
unsigned int)_tmpEF > 2?*((int*)_tmpEF)== 0: 0){_LLA2: _tmpF0=((struct Cyc_Typerep_Int_struct*)
_tmpEF)->f2;goto _LL8F;}else{goto _LL90;}_LL90: if((int)_tmpEF == 0){goto _LL91;}
else{goto _LL92;}_LL92: if((int)_tmpEF == 1){goto _LL93;}else{goto _LL94;}_LL94: if((
unsigned int)_tmpEF > 2?*((int*)_tmpEF)== 1: 0){_LLA4: _tmpF2=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpEF)->f1;goto _LLA3;_LLA3: _tmpF1=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmpEF)->f2;goto _LL95;}else{goto _LL96;}_LL96: if((unsigned int)_tmpEF > 2?*((int*)
_tmpEF)== 2: 0){_LLA5: _tmpF3=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpEF)->f1;
goto _LL97;}else{goto _LL98;}_LL98: if((unsigned int)_tmpEF > 2?*((int*)_tmpEF)== 3:
0){_LLA6: _tmpF4=((struct Cyc_Typerep_Struct_struct*)_tmpEF)->f3;goto _LL99;}else{
goto _LL9A;}_LL9A: if((unsigned int)_tmpEF > 2?*((int*)_tmpEF)== 4: 0){_LLA7: _tmpF5=((
struct Cyc_Typerep_Tuple_struct*)_tmpEF)->f2;goto _LL9B;}else{goto _LL9C;}_LL9C: if((
unsigned int)_tmpEF > 2?*((int*)_tmpEF)== 5: 0){_LLA9: _tmpF7=((struct Cyc_Typerep_TUnion_struct*)
_tmpEF)->f2;goto _LLA8;_LLA8: _tmpF6=((struct Cyc_Typerep_TUnion_struct*)_tmpEF)->f3;
goto _LL9D;}else{goto _LL9E;}_LL9E: if((unsigned int)_tmpEF > 2?*((int*)_tmpEF)== 6:
0){_LLAA: _tmpF8=((struct Cyc_Typerep_XTUnion_struct*)_tmpEF)->f2;goto _LL9F;}else{
goto _LLA0;}_LLA0: if((unsigned int)_tmpEF > 2?*((int*)_tmpEF)== 7: 0){goto _LLA1;}
else{goto _LL8D;}_LL8F: switch(_tmpF0){case 8: _LLAB: {unsigned char*x=((
unsigned char*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetchar(fp);
break;}case 16: _LLAC: {short*x=((short*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetshort(
fp);break;}case 32: _LLAD: {int*x=((int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(
fp);break;}default: _LLAE:(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Core_Failure_struct
_tmpFA;_tmpFA.tag=Cyc_Core_Failure;_tmpFA.f1=_tag_arr("read_type_base: illegal int size bits",
sizeof(unsigned char),38);_tmpFA;});_tmpF9;}));}goto _LL8D;_LL91: {float*y=((
float*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetfloat(fp);goto _LL8D;}
_LL93: {double*y=((double*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(
fp);goto _LL8D;}_LL95: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmpF1);struct
_tagged_arr buf=({unsigned int _tmpFB=_tmpF2 * typ_szb;unsigned char*_tmpFC=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmpFB));
struct _tagged_arr _tmpFE=_tag_arr(_tmpFC,sizeof(unsigned char),_tmpF2 * typ_szb);{
unsigned int _tmpFD=_tmpFB;unsigned int i;for(i=0;i < _tmpFD;i ++){_tmpFC[i]='\000';}};
_tmpFE;});unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(
_untag_arr(buf,sizeof(unsigned char),1)));unsigned int*x=((unsigned int*(*)(void*
x))unsafe_cast)(result);*x=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(
int)bufptr;int i=0;for(0;i < _tmpF2;i ++){env=Cyc_Marshal_read_type_base(_tmpF1,env,
fp,(void*)((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,
mem);}goto _LL8D;}_LL97: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);int pos=(int)
Cyc_Marshal_cycgetvarint(fp);int sz=(int)Cyc_Marshal_cycgetvarint(fp);int typ_szb=(
int)Cyc_Typerep_size_type(_tmpF3);if(! Cyc_Marshal_table_member(env,(unsigned int)
mem)){struct _tagged_arr buf=({unsigned int _tmpFF=(unsigned int)(sz * typ_szb);
unsigned char*_tmp100=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmpFF));struct _tagged_arr _tmp102=_tag_arr(_tmp100,sizeof(
unsigned char),(unsigned int)(sz * typ_szb));{unsigned int _tmp101=_tmpFF;
unsigned int i;for(i=0;i < _tmp101;i ++){_tmp100[i]='\000';}};_tmp102;});
unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,
sizeof(unsigned char),1)));struct _tuple8*x=((struct _tuple8*(*)(void*x))
unsafe_cast)(result);(*x).f1=bufptr;(*x).f2=bufptr + pos * typ_szb;(*x).f3=bufptr + 
sz * typ_szb;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int i=0;
for(0;i < sz;i ++){env=Cyc_Marshal_read_type_base(_tmpF3,env,fp,(void*)((void*(*)(
int x))unsafe_cast)(y));y +=typ_szb;}}}else{struct _tuple8*x=((struct _tuple8*(*)(
void*x))unsafe_cast)(result);(*x).f1=Cyc_Marshal_table_lookup(env,mem);(*x).f2=(*
x).f1 + typ_szb * pos;(*x).f3=(*x).f1 + typ_szb * sz;}goto _LL8D;}_LL99: {
unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < 
_get_arr_size(_tmpF4,sizeof(struct _tuple0*));i ++){void*_tmp104;unsigned int
_tmp105;struct _tuple0 _tmp103=*((struct _tuple0**)_tmpF4.curr)[i];_LLB2: _tmp105=
_tmp103.f1;goto _LLB1;_LLB1: _tmp104=_tmp103.f3;goto _LLB0;_LLB0: {int x_ofs=(int)(x
+ _tmp105);env=Cyc_Marshal_read_type_base(_tmp104,env,fp,(void*)((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LL8D;}_LL9B: {unsigned int x=((unsigned int(*)(void*
x))unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmpF5,sizeof(struct
_tuple1*));i ++){void*_tmp107;unsigned int _tmp108;struct _tuple1 _tmp106=*((struct
_tuple1**)_tmpF5.curr)[i];_LLB5: _tmp108=_tmp106.f1;goto _LLB4;_LLB4: _tmp107=
_tmp106.f2;goto _LLB3;_LLB3: {int x_ofs=(int)(x + _tmp108);env=Cyc_Marshal_read_type_base(
_tmp107,env,fp,(void*)((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL8D;}_LL9D: {
int tagmem=(int)Cyc_Marshal_cycgetvarint(fp);if(tagmem < 1024){int tag=tagmem;
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(unsigned int)
tag;}else{int mem=tagmem - 1024;if(! Cyc_Marshal_table_member(env,(unsigned int)mem)){
int tag=(int)Cyc_Marshal_cycgetvarint(fp);void*_tmp10A;int _tmp10B;struct _tuple4
_tmp109=Cyc_Typerep_get_unionbranch((unsigned int)tag,_tmpF6);_LLB8: _tmp10B=
_tmp109.f1;goto _LLB7;_LLB7: _tmp10A=_tmp109.f2;goto _LLB6;_LLB6: {struct
_tagged_arr buf=({unsigned int _tmp10C=_tmp10B + Cyc_Typerep_size_type(_tmp10A);
unsigned char*_tmp10D=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp10C));struct _tagged_arr _tmp10F=_tag_arr(_tmp10D,sizeof(
unsigned char),_tmp10B + Cyc_Typerep_size_type(_tmp10A));{unsigned int _tmp10E=
_tmp10C;unsigned int i;for(i=0;i < _tmp10E;i ++){_tmp10D[i]='\000';}};_tmp10F;});
unsigned int bufptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(buf,
sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(env,bufptr);{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;{
unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*
tagptr=(unsigned int)tag;env=Cyc_Marshal_read_type_base(_tmp10A,env,fp,(void*)((
void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp10B));}}}}else{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,
mem);}}goto _LL8D;}_LL9F: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(! Cyc_Marshal_table_member(
env,(unsigned int)mem)){unsigned char nonnull=(unsigned char)Cyc_Marshal_cycgetchar(
fp);struct _tagged_arr s=_tag_arr(({unsigned int _tmp117=(unsigned int)100;
unsigned char*_tmp118=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp117));{unsigned int _tmp119=_tmp117;unsigned int i;for(i=0;i < 
_tmp119;i ++){_tmp118[i]='\000';}};_tmp118;}),sizeof(unsigned char),(unsigned int)
100);struct _tagged_arr xtname=_tagged_arr_plus(s,sizeof(unsigned char),4);Cyc_Marshal_fscanntstring(
fp,xtname);if((int)nonnull){void*_tmp111;int _tmp112;struct _tuple4 _tmp110=Cyc_Typerep_get_xtunionbranch((
struct _tagged_arr)xtname,_tmpF8);_LLBB: _tmp112=_tmp110.f1;goto _LLBA;_LLBA:
_tmp111=_tmp110.f2;goto _LLB9;_LLB9: {struct _tagged_arr buf=({unsigned int _tmp113=
_tmp112 + Cyc_Typerep_size_type(_tmp111);unsigned char*_tmp114=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp113));struct _tagged_arr
_tmp116=_tag_arr(_tmp114,sizeof(unsigned char),_tmp112 + Cyc_Typerep_size_type(
_tmp111));{unsigned int _tmp115=_tmp113;unsigned int i;for(i=0;i < _tmp115;i ++){
_tmp114[i]='\000';}};_tmp116;});unsigned int bufptr=(unsigned int)((unsigned char*)
_check_null(_untag_arr(buf,sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(
env,bufptr);{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(
bufptr);*tagptr=(unsigned int)((unsigned char*)_check_null(_untag_arr(s,sizeof(
unsigned char),1)));env=Cyc_Marshal_read_type_base(_tmp111,env,fp,(void*)((void*(*)(
unsigned int x))unsafe_cast)(bufptr + _tmp112));}}}}else{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=(unsigned int)((unsigned char*)
_check_null(_untag_arr(s,sizeof(unsigned char),1)));env=Cyc_Marshal_table_insert(
env,*x);}}else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
Cyc_Marshal_table_lookup(env,mem);}goto _LL8D;}_LLA1: {int sz=(int)Cyc_Typerep_size_type(
rep);env=Cyc_Marshal_read_type_base((void*)({struct Cyc_Typerep_Tuple_struct*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Typerep_Tuple_struct
_tmp11B;_tmp11B.tag=4;_tmp11B.f1=(unsigned int)sz;_tmp11B.f2=({unsigned int
_tmp11C=(unsigned int)sz;struct _tuple1**_tmp11D=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp11C));struct _tagged_arr _tmp122=_tag_arr(
_tmp11D,sizeof(struct _tuple1*),(unsigned int)sz);{unsigned int _tmp11E=_tmp11C;
unsigned int i;for(i=0;i < _tmp11E;i ++){_tmp11D[i]=({struct _tuple1*_tmp11F=
_cycalloc(sizeof(*_tmp11F));_tmp11F->f1=i;_tmp11F->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp120=_cycalloc_atomic(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Typerep_Int_struct
_tmp121;_tmp121.tag=0;_tmp121.f1=0;_tmp121.f2=8;_tmp121;});_tmp120;});_tmp11F;});}};
_tmp122;});_tmp11B;});_tmp11A;}),env,fp,result);goto _LL8D;}_LL8D:;}return env;}
void*Cyc_Marshal_read_type(void*ts,struct Cyc_Std___cycFILE*fp){struct
_handler_cons _tmp123;_push_handler(& _tmp123);{int _tmp125=0;if(setjmp(_tmp123.handler)){
_tmp125=1;}if(! _tmp125){{struct _tagged_arr buf=({unsigned int _tmp127=Cyc_Typerep_size_type(
ts);unsigned char*_tmp128=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(
unsigned char),_tmp127));struct _tagged_arr _tmp12A=_tag_arr(_tmp128,sizeof(
unsigned char),Cyc_Typerep_size_type(ts));{unsigned int _tmp129=_tmp127;
unsigned int i;for(i=0;i < _tmp129;i ++){_tmp128[i]='\000';}};_tmp12A;});void*
result=((void*(*)(unsigned char*x))unsafe_cast)((unsigned char*)_check_null(
_untag_arr(buf,sizeof(unsigned char),1)));Cyc_Marshal_read_type_base(ts,Cyc_Marshal_empty_addr_table(),
fp,result);{void*_tmp126=result;_npop_handler(0);return _tmp126;}};_pop_handler();}
else{void*_tmp124=(void*)_exn_thrown;void*_tmp12C=_tmp124;struct _tagged_arr
_tmp12D;_LLBD: if(*((void**)_tmp12C)== Cyc_Core_Failure){_LLC1: _tmp12D=((struct
Cyc_Core_Failure_struct*)_tmp12C)->f1;goto _LLBE;}else{goto _LLBF;}_LLBF: goto _LLC0;
_LLBE:({struct Cyc_Std_String_pa_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct
_tagged_arr)_tmp12D;{void*_tmp12E[1]={& _tmp12F};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n",sizeof(unsigned char),19),_tag_arr(_tmp12E,
sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp130=
_cycalloc(sizeof(*_tmp130));_tmp130[0]=({struct Cyc_Core_Failure_struct _tmp131;
_tmp131.tag=Cyc_Core_Failure;_tmp131.f1=_tmp12D;_tmp131;});_tmp130;}));_LLC0:(
void)_throw(_tmp12C);_LLBC:;}}}
