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
  unsigned total_bytes;
  unsigned free_bytes;
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
extern void   _reset_region(struct _RegionHandle *);

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
extern int _throw(void* e);
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
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
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
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
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
 void exit(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern char
Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct _tagged_arr f1;
};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{char*tag;
struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct _tagged_arr Cstring_to_string(char*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[11];struct Cyc_Typerep_Int_struct{int tag;int f1;
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
f2;};struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct _tagged_arr l);
struct _tagged_arr Cyc_Typerep_get_tagname(unsigned int tag,struct _tagged_arr l);
struct _tagged_arr Cyc_Typerep_get_tagname2(unsigned int tag,struct _tagged_arr l);
struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct _tagged_arr tag,struct
_tagged_arr l);unsigned int Cyc_Typerep_size_type(void*rep);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern
struct Cyc_Std___cycFILE*Cyc_Std_stderr;int Cyc_Std_fgetc(struct Cyc_Std___cycFILE*
__stream);int Cyc_Std_fputc(int __c,struct Cyc_Std___cycFILE*__stream);unsigned int
Cyc_Std_fread(struct _tagged_arr __ptr,unsigned int __size,unsigned int __n,struct
Cyc_Std___cycFILE*__stream);unsigned int Cyc_Std_fwrite(struct _tagged_arr __ptr,
unsigned int __size,unsigned int __n,struct Cyc_Std___cycFILE*__s);extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);int isprint(int);struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);void Cyc_Marshal_fprint_type(void*rep,
struct Cyc_Std___cycFILE*fp,void*val);void Cyc_Marshal_print_type(void*rep,void*
val);void Cyc_Marshal_write_type(void*rep,struct Cyc_Std___cycFILE*fp,void*val);
struct _tuple6{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple7{struct _tagged_arr f1;
int f2;};struct _tuple6 Cyc_Marshal_empty_addr_index();struct _tuple6 Cyc_Marshal_write_type_base(
void*rep,struct _tuple6 env,struct Cyc_Std___cycFILE*fp,void*val);void*Cyc_Marshal_rread_type(
struct _RegionHandle*r,void*rep,struct Cyc_Std___cycFILE*fp);void*Cyc_Marshal_read_type(
void*rep,struct Cyc_Std___cycFILE*fp);void*Cyc_Marshal_rcopy_type(struct
_RegionHandle*,void*rep,void*val);void*Cyc_Marshal_copy_type(void*rep,void*val);
int Cyc_Marshal_leq_type(void*rep,void*x,void*y);int Cyc_Marshal_eq_type(void*rep,
void*x,void*y);int Cyc_Marshal_cmp_type(void*rep,void*x,void*y);void*unsafe_cast(
void*x);static int Cyc_Marshal_uint_cmp(unsigned int a,unsigned int b){if(a == b)
return 0;else{if(a < b)return 1;else{return - 1;}}}static struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set(){
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(((
struct Cyc_Set_Set*(*)(int(*cmp)(unsigned int,unsigned int)))Cyc_Set_empty)(Cyc_Marshal_uint_cmp),
0);}struct _tuple8{int f1;struct Cyc_Set_Set*f2;};static struct _tuple8*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val){return({struct _tuple8*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0->f1=((int(*)(struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_member)(
set,val);_tmp0->f2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,unsigned int elt))
Cyc_Set_insert)(set,val);_tmp0;});}inline static int Cyc_Marshal_index_member(
struct _tuple6 idx,unsigned int m){int _tmp2;struct _tuple6 _tmp1=idx;_tmp2=_tmp1.f2;
return m < _tmp2;}struct _tuple6 Cyc_Marshal_empty_addr_index(){return({struct
_tuple6 _tmp3;_tmp3.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);_tmp3.f2=
1;_tmp3;});}static unsigned int Cyc_Marshal_index_lookup(struct _tuple6 index,
unsigned int val){struct Cyc_Dict_Dict*_tmp5;int _tmp6;struct _tuple6 _tmp4=index;
_tmp5=_tmp4.f1;_tmp6=_tmp4.f2;{struct Cyc_Core_Opt*_tmp7=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup_opt)(_tmp5,val);if(_tmp7 != 
0)return(unsigned int)((int)_tmp7->v);else{return(unsigned int)_tmp6;}}}static
struct _tuple6 Cyc_Marshal_index_insert(struct _tuple6 index,unsigned int val){struct
Cyc_Dict_Dict*_tmp9;int _tmpA;struct _tuple6 _tmp8=index;_tmp9=_tmp8.f1;_tmpA=_tmp8.f2;
return({struct _tuple6 _tmpB;_tmpB.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,unsigned int k,int v))Cyc_Dict_insert)(_tmp9,val,_tmpA);_tmpB.f2=_tmpA + 1;_tmpB;});}
static struct _tuple7 Cyc_Marshal_empty_addr_table(){return({struct _tuple7 _tmpC;
_tmpC.f1=_tag_arr(({unsigned int _tmpD=(unsigned int)1;unsigned int*_tmpE=(
unsigned int*)_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmpD));{
unsigned int _tmpF=_tmpD;unsigned int i;for(i=0;i < _tmpF;i ++){_tmpE[i]=0;}}_tmpE;}),
sizeof(unsigned int),(unsigned int)1);_tmpC.f2=1;_tmpC;});}inline static int Cyc_Marshal_table_member(
struct _tuple7 table,unsigned int m){int _tmp11;struct _tuple7 _tmp10=table;_tmp11=
_tmp10.f2;return m < _tmp11;}static struct _tuple7 Cyc_Marshal_table_insert(struct
_tuple7 tbl,unsigned int i){struct _tagged_arr _tmp13;int _tmp14;struct _tuple7 _tmp12=
tbl;_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;if(_get_arr_size(_tmp13,sizeof(
unsigned int))== _tmp14)_tmp13=({unsigned int _tmp15=(unsigned int)(2 * _tmp14);
unsigned int*_tmp16=(unsigned int*)_cycalloc_atomic(_check_times(sizeof(
unsigned int),_tmp15));struct _tagged_arr _tmp18=_tag_arr(_tmp16,sizeof(
unsigned int),(unsigned int)(2 * _tmp14));{unsigned int _tmp17=_tmp15;unsigned int
j;for(j=0;j < _tmp17;j ++){_tmp16[j]=j < _tmp14?*((unsigned int*)
_check_unknown_subscript(_tmp13,sizeof(unsigned int),(int)j)): 0;}}_tmp18;});((
unsigned int*)_tmp13.curr)[_tmp14]=i;return({struct _tuple7 _tmp19;_tmp19.f1=
_tmp13;_tmp19.f2=_tmp14 + 1;_tmp19;});}static unsigned int Cyc_Marshal_table_lookup(
struct _tuple7 tbl,int i){return*((unsigned int*)_check_unknown_subscript(tbl.f1,
sizeof(unsigned int),i));}static void Cyc_Marshal_fprint_ref(struct Cyc_Std___cycFILE*
fp,unsigned int ref){if(ref == 0)({void*_tmp1A[0]={};Cyc_Std_fprintf(fp,_tag_arr("NULL",
sizeof(char),5),_tag_arr(_tmp1A,sizeof(void*),0));});else{({struct Cyc_Std_Int_pa_struct
_tmp1C;_tmp1C.tag=1;_tmp1C.f1=ref;{void*_tmp1B[1]={& _tmp1C};Cyc_Std_fprintf(fp,
_tag_arr("(#%x)",sizeof(char),6),_tag_arr(_tmp1B,sizeof(void*),1));}});}}struct
_tuple9{unsigned int f1;unsigned int f2;unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_fprint_type_base(
void*rep,struct Cyc_Set_Set*env,struct Cyc_Std___cycFILE*fp,void*val){{void*_tmp1D=
rep;int _tmp1E;unsigned int _tmp1F;unsigned int _tmp20;void*_tmp21;void*_tmp22;
struct _tagged_arr*_tmp23;struct _tagged_arr _tmp24;struct _tagged_arr _tmp25;struct
_tagged_arr _tmp26;struct _tagged_arr _tmp27;struct _tagged_arr _tmp28;struct
_tagged_arr _tmp29;struct _tagged_arr _tmp2A;struct _tagged_arr _tmp2B;struct
_tagged_arr _tmp2C;struct _tagged_arr _tmp2D;struct _tagged_arr*_tmp2E;struct
_tagged_arr _tmp2F;int _tmp30;struct _tagged_arr _tmp31;_LL1: if(_tmp1D <= (void*)2?1:*((
int*)_tmp1D)!= 0)goto _LL3;_tmp1E=((struct Cyc_Typerep_Int_struct*)_tmp1D)->f1;
_tmp1F=((struct Cyc_Typerep_Int_struct*)_tmp1D)->f2;_LL2: switch(_tmp1F){case 8:
_LL19: {char*x=((char*(*)(void*x))unsafe_cast)(val);if(isprint((int)*x))({struct
Cyc_Std_Int_pa_struct _tmp33;_tmp33.tag=1;_tmp33.f1=(int)((unsigned int)((int)*x));{
void*_tmp32[1]={& _tmp33};Cyc_Std_fprintf(fp,_tag_arr("'%c'",sizeof(char),5),
_tag_arr(_tmp32,sizeof(void*),1));}});else{({struct Cyc_Std_Int_pa_struct _tmp35;
_tmp35.tag=1;_tmp35.f1=(int)((unsigned int)((int)*x));{void*_tmp34[1]={& _tmp35};
Cyc_Std_fprintf(fp,_tag_arr("'\\%d'",sizeof(char),6),_tag_arr(_tmp34,sizeof(void*),
1));}});}break;}case 16: _LL1A: {short*x=((short*(*)(void*x))unsafe_cast)(val);({
struct Cyc_Std_Int_pa_struct _tmp37;_tmp37.tag=1;_tmp37.f1=(unsigned int)((int)*x);{
void*_tmp36[1]={& _tmp37};Cyc_Std_fprintf(fp,_tmp1E?_tag_arr("%d",sizeof(char),3):
_tag_arr("%u",sizeof(char),3),_tag_arr(_tmp36,sizeof(void*),1));}});break;}case
32: _LL1B: {int*x=((int*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp39;_tmp39.tag=1;_tmp39.f1=(unsigned int)*x;{void*_tmp38[1]={& _tmp39};Cyc_Std_fprintf(
fp,_tmp1E?_tag_arr("%d",sizeof(char),3): _tag_arr("%u",sizeof(char),3),_tag_arr(
_tmp38,sizeof(void*),1));}});break;}default: _LL1C:({struct Cyc_Std_Int_pa_struct
_tmp3B;_tmp3B.tag=1;_tmp3B.f1=(int)_tmp1F;{void*_tmp3A[1]={& _tmp3B};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("fprint_type_base: illegal int size %d bits\n",sizeof(
char),44),_tag_arr(_tmp3A,sizeof(void*),1));}});exit(1);}goto _LL0;_LL3: if((int)
_tmp1D != 0)goto _LL5;_LL4: {float*x=((float*(*)(void*x))unsafe_cast)(val);({
struct Cyc_Std_Double_pa_struct _tmp3D;_tmp3D.tag=2;_tmp3D.f1=(double)*x;{void*
_tmp3C[1]={& _tmp3D};Cyc_Std_fprintf(fp,_tag_arr("%f",sizeof(char),3),_tag_arr(
_tmp3C,sizeof(void*),1));}});goto _LL0;}_LL5: if((int)_tmp1D != 1)goto _LL7;_LL6: {
double*x=((double*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp3F;_tmp3F.tag=2;_tmp3F.f1=*x;{void*_tmp3E[1]={& _tmp3F};Cyc_Std_fprintf(fp,
_tag_arr("%f",sizeof(char),3),_tag_arr(_tmp3E,sizeof(void*),1));}});goto _LL0;}
_LL7: if(_tmp1D <= (void*)2?1:*((int*)_tmp1D)!= 1)goto _LL9;_tmp20=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1D)->f1;_tmp21=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1D)->f2;_LL8: {
int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));struct
_tuple8 _tmp41;int _tmp42;struct Cyc_Set_Set*_tmp43;struct _tuple8*_tmp40=Cyc_Marshal_member_insert(
env,(unsigned int)y);_tmp41=*_tmp40;_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;env=_tmp43;
if(!_tmp42){({struct Cyc_Std_Int_pa_struct _tmp45;_tmp45.tag=1;_tmp45.f1=(
unsigned int)y;{void*_tmp44[1]={& _tmp45};Cyc_Std_fprintf(fp,_tag_arr("{(@%x) ",
sizeof(char),8),_tag_arr(_tmp44,sizeof(void*),1));}});if(y != 0){int typ_szb=(int)
Cyc_Typerep_size_type(_tmp21);int i=0;for(0;i < _tmp20;i ++){env=Cyc_Marshal_fprint_type_base(
_tmp21,env,fp,((void*(*)(int x))unsafe_cast)(y));if(i != _tmp20 - 1)({void*_tmp46[0]={};
Cyc_Std_fprintf(fp,_tag_arr(", ",sizeof(char),3),_tag_arr(_tmp46,sizeof(void*),0));});
y +=typ_szb;}}else{({void*_tmp47[0]={};Cyc_Std_fprintf(fp,_tag_arr("NULL",sizeof(
char),5),_tag_arr(_tmp47,sizeof(void*),0));});}({void*_tmp48[0]={};Cyc_Std_fprintf(
fp,_tag_arr(" }",sizeof(char),3),_tag_arr(_tmp48,sizeof(void*),0));});}else{Cyc_Marshal_fprint_ref(
fp,(unsigned int)y);}goto _LL0;}_LL9: if(_tmp1D <= (void*)2?1:*((int*)_tmp1D)!= 2)
goto _LLB;_tmp22=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp1D)->f1;_LLA: {
struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);unsigned int
typ_szb=Cyc_Typerep_size_type(_tmp22);unsigned int base=(*x).f1;unsigned int curr=(*
x).f2;unsigned int last_plus_one=(*x).f3;struct _tuple8 _tmp4A;int _tmp4B;struct Cyc_Set_Set*
_tmp4C;struct _tuple8*_tmp49=Cyc_Marshal_member_insert(env,base);_tmp4A=*_tmp49;
_tmp4B=_tmp4A.f1;_tmp4C=_tmp4A.f2;env=_tmp4C;if(!_tmp4B){({struct Cyc_Std_Int_pa_struct
_tmp4E;_tmp4E.tag=1;_tmp4E.f1=base;{void*_tmp4D[1]={& _tmp4E};Cyc_Std_fprintf(fp,
_tag_arr("[(@%x) ",sizeof(char),8),_tag_arr(_tmp4D,sizeof(void*),1));}});{
unsigned int i=base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_fprint_type_base(
_tmp22,env,fp,((void*(*)(unsigned int x))unsafe_cast)(i));if(i + typ_szb < 
last_plus_one)({void*_tmp4F[0]={};Cyc_Std_fprintf(fp,_tag_arr(", ",sizeof(char),
3),_tag_arr(_tmp4F,sizeof(void*),0));});}}({void*_tmp50[0]={};Cyc_Std_fprintf(fp,
_tag_arr(" ]",sizeof(char),3),_tag_arr(_tmp50,sizeof(void*),0));});}else{Cyc_Marshal_fprint_ref(
fp,base);}goto _LL0;}_LLB: if(_tmp1D <= (void*)2?1:*((int*)_tmp1D)!= 3)goto _LLD;
_tmp23=((struct Cyc_Typerep_Struct_struct*)_tmp1D)->f1;_tmp24=((struct Cyc_Typerep_Struct_struct*)
_tmp1D)->f3;_LLC: if(_tmp23 == 0)({void*_tmp51[0]={};Cyc_Std_fprintf(fp,_tag_arr("{ ",
sizeof(char),3),_tag_arr(_tmp51,sizeof(void*),0));});else{({struct Cyc_Std_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)*_tmp23;{void*_tmp52[1]={&
_tmp53};Cyc_Std_fprintf(fp,_tag_arr("%s{ ",sizeof(char),5),_tag_arr(_tmp52,
sizeof(void*),1));}});}{int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_arr_size(_tmp24,sizeof(struct _tuple0*));i ++){unsigned int _tmp55;struct
_tagged_arr _tmp56;void*_tmp57;struct _tuple0 _tmp54=*((struct _tuple0**)_tmp24.curr)[
i];_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;_tmp57=_tmp54.f3;({struct Cyc_Std_String_pa_struct
_tmp59;_tmp59.tag=0;_tmp59.f1=(struct _tagged_arr)_tmp56;{void*_tmp58[1]={& _tmp59};
Cyc_Std_fprintf(fp,_tag_arr("%s=",sizeof(char),4),_tag_arr(_tmp58,sizeof(void*),
1));}});{int x_ofs=(int)(x + _tmp55);env=Cyc_Marshal_fprint_type_base(_tmp57,env,
fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp24,sizeof(
struct _tuple0*))- 1)({void*_tmp5A[0]={};Cyc_Std_fprintf(fp,_tag_arr(", ",sizeof(
char),3),_tag_arr(_tmp5A,sizeof(void*),0));});}}}({void*_tmp5B[0]={};Cyc_Std_fprintf(
fp,_tag_arr(" }",sizeof(char),3),_tag_arr(_tmp5B,sizeof(void*),0));});goto _LL0;}
_LLD: if(_tmp1D <= (void*)2?1:*((int*)_tmp1D)!= 4)goto _LLF;_tmp25=((struct Cyc_Typerep_Tuple_struct*)
_tmp1D)->f2;_LLE:({void*_tmp5C[0]={};Cyc_Std_fprintf(fp,_tag_arr("$( ",sizeof(
char),4),_tag_arr(_tmp5C,sizeof(void*),0));});{int x=((int(*)(void*x))unsafe_cast)(
val);{int i=0;for(0;i < _get_arr_size(_tmp25,sizeof(struct _tuple1*));i ++){
unsigned int _tmp5E;void*_tmp5F;struct _tuple1 _tmp5D=*((struct _tuple1**)_tmp25.curr)[
i];_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{int x_ofs=(int)(x + _tmp5E);env=Cyc_Marshal_fprint_type_base(
_tmp5F,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp25,
sizeof(struct _tuple1*))- 1)({void*_tmp60[0]={};Cyc_Std_fprintf(fp,_tag_arr(", ",
sizeof(char),3),_tag_arr(_tmp60,sizeof(void*),0));});}}}({void*_tmp61[0]={};Cyc_Std_fprintf(
fp,_tag_arr(" )",sizeof(char),3),_tag_arr(_tmp61,sizeof(void*),0));});goto _LL0;}
_LLF: if(_tmp1D <= (void*)2?1:*((int*)_tmp1D)!= 6)goto _LL11;_tmp26=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1D)->f1;_tmp27=((struct Cyc_Typerep_TUnionField_struct*)_tmp1D)->f2;_tmp28=((
struct Cyc_Typerep_TUnionField_struct*)_tmp1D)->f4;_LL10:({struct Cyc_Std_String_pa_struct
_tmp64;_tmp64.tag=0;_tmp64.f1=(struct _tagged_arr)_tmp27;{struct Cyc_Std_String_pa_struct
_tmp63;_tmp63.tag=0;_tmp63.f1=(struct _tagged_arr)_tmp26;{void*_tmp62[2]={& _tmp63,&
_tmp64};Cyc_Std_fprintf(fp,_tag_arr("%s.%s(",sizeof(char),7),_tag_arr(_tmp62,
sizeof(void*),2));}}});{int x=((int(*)(void*x))unsafe_cast)(val);{int i=1;for(0;i < 
_get_arr_size(_tmp28,sizeof(struct _tuple1*));i ++){unsigned int _tmp66;void*_tmp67;
struct _tuple1 _tmp65=*((struct _tuple1**)_tmp28.curr)[i];_tmp66=_tmp65.f1;_tmp67=
_tmp65.f2;{int x_ofs=(int)(x + _tmp66);env=Cyc_Marshal_fprint_type_base(_tmp67,env,
fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp28,sizeof(
struct _tuple1*))- 1)({void*_tmp68[0]={};Cyc_Std_fprintf(fp,_tag_arr(", ",sizeof(
char),3),_tag_arr(_tmp68,sizeof(void*),0));});}}}({void*_tmp69[0]={};Cyc_Std_fprintf(
fp,_tag_arr(" )",sizeof(char),3),_tag_arr(_tmp69,sizeof(void*),0));});goto _LL0;}
_LL11: if(_tmp1D <= (void*)2?1:*((int*)_tmp1D)!= 5)goto _LL13;_tmp29=((struct Cyc_Typerep_TUnion_struct*)
_tmp1D)->f1;_tmp2A=((struct Cyc_Typerep_TUnion_struct*)_tmp1D)->f2;_tmp2B=((
struct Cyc_Typerep_TUnion_struct*)_tmp1D)->f3;_LL12: {unsigned int*tagp=((
unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024)({struct Cyc_Std_String_pa_struct
_tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname(*tagp,
_tmp2A);{void*_tmp6A[1]={& _tmp6B};Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(char),
3),_tag_arr(_tmp6A,sizeof(void*),1));}});else{struct _tuple8 _tmp6D;int _tmp6E;
struct Cyc_Set_Set*_tmp6F;struct _tuple8*_tmp6C=Cyc_Marshal_member_insert(env,*
tagp);_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;env=_tmp6F;if(!_tmp6E){({
struct Cyc_Std_Int_pa_struct _tmp71;_tmp71.tag=1;_tmp71.f1=*tagp;{void*_tmp70[1]={&
_tmp71};Cyc_Std_fprintf(fp,_tag_arr("(@%x)",sizeof(char),6),_tag_arr(_tmp70,
sizeof(void*),1));}});{unsigned int*x=((unsigned int*(*)(unsigned int x))
unsafe_cast)(*tagp);unsigned int tag=*x;int _tmp73;void*_tmp74;struct _tuple4 _tmp72=
Cyc_Typerep_get_unionbranch(tag,_tmp2B);_tmp73=_tmp72.f1;_tmp74=_tmp72.f2;{void*
xoff=((void*(*)(unsigned int x))unsafe_cast)(*tagp + _tmp73);({struct Cyc_Std_String_pa_struct
_tmp76;_tmp76.tag=0;_tmp76.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname2(tag,
_tmp2B);{void*_tmp75[1]={& _tmp76};Cyc_Std_fprintf(fp,_tag_arr("&%s[",sizeof(char),
5),_tag_arr(_tmp75,sizeof(void*),1));}});env=Cyc_Marshal_fprint_type_base(_tmp74,
env,fp,xoff);({void*_tmp77[0]={};Cyc_Std_fprintf(fp,_tag_arr("]",sizeof(char),2),
_tag_arr(_tmp77,sizeof(void*),0));});}}}else{Cyc_Marshal_fprint_ref(fp,*tagp);}}
goto _LL0;}_LL13: if(_tmp1D <= (void*)2?1:*((int*)_tmp1D)!= 7)goto _LL15;_tmp2C=((
struct Cyc_Typerep_XTUnion_struct*)_tmp1D)->f1;_tmp2D=((struct Cyc_Typerep_XTUnion_struct*)
_tmp1D)->f2;_LL14: {unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(
val);struct _tuple8 _tmp79;int _tmp7A;struct Cyc_Set_Set*_tmp7B;struct _tuple8*_tmp78=
Cyc_Marshal_member_insert(env,*xtunionp);_tmp79=*_tmp78;_tmp7A=_tmp79.f1;_tmp7B=
_tmp79.f2;env=_tmp7B;if(!_tmp7A){({struct Cyc_Std_Int_pa_struct _tmp7D;_tmp7D.tag=
1;_tmp7D.f1=*xtunionp;{void*_tmp7C[1]={& _tmp7D};Cyc_Std_fprintf(fp,_tag_arr("(@%x)",
sizeof(char),6),_tag_arr(_tmp7C,sizeof(void*),1));}});{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);if(*xtstructp != 0){char*
xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*xtstructp + 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct _tagged_arr)xtname;{void*_tmp7E[1]={& _tmp7F};
Cyc_Std_fprintf(fp,_tag_arr("&%s[( ",sizeof(char),7),_tag_arr(_tmp7E,sizeof(void*),
1));}});{int _tmp81;void*_tmp82;struct _tuple4 _tmp80=Cyc_Typerep_get_xtunionbranch(
xtname,_tmp2D);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*xtunionp + _tmp81);env=Cyc_Marshal_fprint_type_base(
_tmp82,env,fp,xoff);({void*_tmp83[0]={};Cyc_Std_fprintf(fp,_tag_arr(" )]",
sizeof(char),4),_tag_arr(_tmp83,sizeof(void*),0));});}}}else{char*xtnamec=((char*(*)(
unsigned int x))unsafe_cast)(*xtunionp + 4);struct _tagged_arr xtname=(struct
_tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct _tmp85;
_tmp85.tag=0;_tmp85.f1=(struct _tagged_arr)xtname;{void*_tmp84[1]={& _tmp85};Cyc_Std_fprintf(
fp,_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp84,sizeof(void*),1));}});}}}else{
Cyc_Marshal_fprint_ref(fp,*xtunionp);}goto _LL0;}_LL15: if(_tmp1D <= (void*)2?1:*((
int*)_tmp1D)!= 8)goto _LL17;_tmp2E=((struct Cyc_Typerep_Union_struct*)_tmp1D)->f1;
_tmp2F=((struct Cyc_Typerep_Union_struct*)_tmp1D)->f3;_LL16: if(_tmp2E == 0)({void*
_tmp86[0]={};Cyc_Std_fprintf(fp,_tag_arr("{",sizeof(char),2),_tag_arr(_tmp86,
sizeof(void*),0));});else{({struct Cyc_Std_String_pa_struct _tmp88;_tmp88.tag=0;
_tmp88.f1=(struct _tagged_arr)*_tmp2E;{void*_tmp87[1]={& _tmp88};Cyc_Std_fprintf(
fp,_tag_arr("%s{",sizeof(char),4),_tag_arr(_tmp87,sizeof(void*),1));}});}{int i=0;
for(0;i < _get_arr_size(_tmp2F,sizeof(struct _tuple3*));i ++){struct _tagged_arr
_tmp8A;void*_tmp8B;struct _tuple3 _tmp89=*((struct _tuple3**)_tmp2F.curr)[i];_tmp8A=
_tmp89.f1;_tmp8B=_tmp89.f2;({struct Cyc_Std_String_pa_struct _tmp8D;_tmp8D.tag=0;
_tmp8D.f1=(struct _tagged_arr)_tmp8A;{void*_tmp8C[1]={& _tmp8D};Cyc_Std_fprintf(fp,
_tag_arr("%s=",sizeof(char),4),_tag_arr(_tmp8C,sizeof(void*),1));}});Cyc_Marshal_fprint_type_base(
_tmp8B,env,fp,val);if(i < _get_arr_size(_tmp2F,sizeof(struct _tuple3*))- 1)({void*
_tmp8E[0]={};Cyc_Std_fprintf(fp,_tag_arr(" | ",sizeof(char),4),_tag_arr(_tmp8E,
sizeof(void*),0));});}}({void*_tmp8F[0]={};Cyc_Std_fprintf(fp,_tag_arr("}",
sizeof(char),2),_tag_arr(_tmp8F,sizeof(void*),0));});goto _LL0;_LL17: if(_tmp1D <= (
void*)2?1:*((int*)_tmp1D)!= 9)goto _LL0;_tmp30=((struct Cyc_Typerep_Enum_struct*)
_tmp1D)->f2;_tmp31=((struct Cyc_Typerep_Enum_struct*)_tmp1D)->f3;_LL18: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_String_pa_struct
_tmp91;_tmp91.tag=0;_tmp91.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname(*tagp,
_tmp31);{void*_tmp90[1]={& _tmp91};Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(char),
3),_tag_arr(_tmp90,sizeof(void*),1));}});goto _LL0;}_LL0:;}return env;}void Cyc_Marshal_fprint_type(
void*ts,struct Cyc_Std___cycFILE*fp,void*val){struct _handler_cons _tmp92;
_push_handler(& _tmp92);{int _tmp94=0;if(setjmp(_tmp92.handler))_tmp94=1;if(!
_tmp94){Cyc_Marshal_fprint_type_base(ts,Cyc_Marshal_empty_addr_set(),fp,val);;
_pop_handler();}else{void*_tmp93=(void*)_exn_thrown;void*_tmp96=_tmp93;struct
_tagged_arr _tmp97;_LL1F: if(*((void**)_tmp96)!= Cyc_Core_Failure)goto _LL21;_tmp97=((
struct Cyc_Core_Failure_struct*)_tmp96)->f1;_LL20:({struct Cyc_Std_String_pa_struct
_tmp99;_tmp99.tag=0;_tmp99.f1=(struct _tagged_arr)_tmp97;{void*_tmp98[1]={& _tmp99};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(char),19),
_tag_arr(_tmp98,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Core_Failure_struct
_tmp9B;_tmp9B.tag=Cyc_Core_Failure;_tmp9B.f1=_tmp97;_tmp9B;});_tmp9A;}));_LL21:;
_LL22:(void)_throw(_tmp96);_LL1E:;}}}void Cyc_Marshal_print_type(void*ts,void*val){
Cyc_Marshal_fprint_type(ts,Cyc_Std_stdout,val);}void Cyc_Marshal_cycputchar(char x,
struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(char),1)),sizeof(char),1,fp)< 1)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({
struct Cyc_Core_Failure_struct _tmp9D;_tmp9D.tag=Cyc_Core_Failure;_tmp9D.f1=
_tag_arr("Write failure",sizeof(char),14);_tmp9D;});_tmp9C;}));}void Cyc_Marshal_cycputshort(
short x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(short),1)),sizeof(short),1,fp)< 1)(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_Core_Failure_struct _tmp9F;_tmp9F.tag=Cyc_Core_Failure;_tmp9F.f1=
_tag_arr("Write failure",sizeof(char),14);_tmp9F;});_tmp9E;}));}void Cyc_Marshal_cycputint(
int x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(int),1)),sizeof(int),1,fp)< 1)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({
struct Cyc_Core_Failure_struct _tmpA1;_tmpA1.tag=Cyc_Core_Failure;_tmpA1.f1=
_tag_arr("Write failure",sizeof(char),14);_tmpA1;});_tmpA0;}));}void Cyc_Marshal_cycputfloat(
float x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((struct
_tagged_arr)_tag_arr(& x,sizeof(float),1)),sizeof(float),1,fp)< 1)(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({
struct Cyc_Core_Failure_struct _tmpA3;_tmpA3.tag=Cyc_Core_Failure;_tmpA3.f1=
_tag_arr("Write failure",sizeof(char),14);_tmpA3;});_tmpA2;}));}void Cyc_Marshal_cycputdouble(
double x,struct Cyc_Std___cycFILE*fp){if(Cyc_Std_fwrite((struct _tagged_arr)((
struct _tagged_arr)_tag_arr(& x,sizeof(double),1)),sizeof(double),1,fp)< 1)(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));
_tmpA4[0]=({struct Cyc_Core_Failure_struct _tmpA5;_tmpA5.tag=Cyc_Core_Failure;
_tmpA5.f1=_tag_arr("Write failure",sizeof(char),14);_tmpA5;});_tmpA4;}));}void
Cyc_Marshal_cycputvarint(unsigned int i,struct Cyc_Std___cycFILE*fp){int width=0;
unsigned int mask=-256;while((int)mask){if((int)(i & mask))width ++;mask <<=8;}Cyc_Std_fputc((
int)(width << 6 | i & 63),fp);i >>=6;{int j=0;for(0;j < width;j ++){Cyc_Std_fputc((int)(
i & 255),fp);i >>=8;}}}char Cyc_Marshal_cycgetchar(struct Cyc_Std___cycFILE*fp){
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpAA=sizeof(char);
char*_tmpAB=(char*)_region_malloc(r,_check_times(sizeof(char),_tmpAA));{
unsigned int _tmpAC=_tmpAA;unsigned int i;for(i=0;i < _tmpAC;i ++){_tmpAB[i]='\000';}}
_tmpAB;}),sizeof(char),sizeof(char));if(Cyc_Std_fread(x,sizeof(char),1,fp)< 1)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpA7=_cycalloc(sizeof(*
_tmpA7));_tmpA7[0]=({struct Cyc_Core_Failure_struct _tmpA8;_tmpA8.tag=Cyc_Core_Failure;
_tmpA8.f1=_tag_arr("Read failure",sizeof(char),13);_tmpA8;});_tmpA7;}));{char*xx=((
char*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,sizeof(char),1)));
char _tmpA9=*xx;_npop_handler(0);return _tmpA9;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpAD=_new_region("r");struct
_RegionHandle*r=& _tmpAD;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpB1=sizeof(short);char*_tmpB2=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmpB1));{unsigned int _tmpB3=_tmpB1;unsigned int i;for(
i=0;i < _tmpB3;i ++){_tmpB2[i]='\000';}}_tmpB2;}),sizeof(char),sizeof(short));if(
Cyc_Std_fread(x,sizeof(short),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Core_Failure_struct
_tmpAF;_tmpAF.tag=Cyc_Core_Failure;_tmpAF.f1=_tag_arr("Read failure",sizeof(char),
13);_tmpAF;});_tmpAE;}));{short*xx=((short*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_arr(x,sizeof(char),1)));short _tmpB0=*xx;_npop_handler(0);
return _tmpB0;}};_pop_region(r);}int Cyc_Marshal_cycgetint(struct Cyc_Std___cycFILE*
fp){struct _RegionHandle _tmpB4=_new_region("r");struct _RegionHandle*r=& _tmpB4;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpB8=sizeof(int);
char*_tmpB9=(char*)_region_malloc(r,_check_times(sizeof(char),_tmpB8));{
unsigned int _tmpBA=_tmpB8;unsigned int i;for(i=0;i < _tmpBA;i ++){_tmpB9[i]='\000';}}
_tmpB9;}),sizeof(char),sizeof(int));if(Cyc_Std_fread(x,sizeof(int),1,fp)< 1)(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));
_tmpB5[0]=({struct Cyc_Core_Failure_struct _tmpB6;_tmpB6.tag=Cyc_Core_Failure;
_tmpB6.f1=_tag_arr("Read failure",sizeof(char),13);_tmpB6;});_tmpB5;}));{int*xx=((
int*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,sizeof(char),1)));
int _tmpB7=*xx;_npop_handler(0);return _tmpB7;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpBB=_new_region("r");struct
_RegionHandle*r=& _tmpBB;_push_region(r);{struct _tagged_arr x=_tag_arr(({
unsigned int _tmpBF=sizeof(float);char*_tmpC0=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmpBF));{unsigned int _tmpC1=_tmpBF;unsigned int i;for(
i=0;i < _tmpC1;i ++){_tmpC0[i]='\000';}}_tmpC0;}),sizeof(char),sizeof(float));if(
Cyc_Std_fread(x,sizeof(float),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Core_Failure_struct
_tmpBD;_tmpBD.tag=Cyc_Core_Failure;_tmpBD.f1=_tag_arr("Read failure",sizeof(char),
13);_tmpBD;});_tmpBC;}));{float*xx=((float*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_arr(x,sizeof(char),1)));float _tmpBE=*xx;_npop_handler(0);
return _tmpBE;}};_pop_region(r);}double Cyc_Marshal_cycgetdouble(struct Cyc_Std___cycFILE*
fp){struct _RegionHandle _tmpC2=_new_region("r");struct _RegionHandle*r=& _tmpC2;
_push_region(r);{struct _tagged_arr x=_tag_arr(({unsigned int _tmpC6=sizeof(double);
char*_tmpC7=(char*)_region_malloc(r,_check_times(sizeof(char),_tmpC6));{
unsigned int _tmpC8=_tmpC6;unsigned int i;for(i=0;i < _tmpC8;i ++){_tmpC7[i]='\000';}}
_tmpC7;}),sizeof(char),sizeof(double));if(Cyc_Std_fread(x,sizeof(double),1,fp)< 
1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpC3=_cycalloc(sizeof(*
_tmpC3));_tmpC3[0]=({struct Cyc_Core_Failure_struct _tmpC4;_tmpC4.tag=Cyc_Core_Failure;
_tmpC4.f1=_tag_arr("Read failure",sizeof(char),13);_tmpC4;});_tmpC3;}));{double*
xx=((double*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,sizeof(char),
1)));double _tmpC5=*xx;_npop_handler(0);return _tmpC5;}};_pop_region(r);}
unsigned int Cyc_Marshal_cycgetvarint(struct Cyc_Std___cycFILE*fp){int i=0;int j=(
int)Cyc_Marshal_cycgetchar(fp);int width=(j & 192)>> 6;int pos=6;i +=j & 63;{int k=0;
for(0;k < width;k ++){j=(int)Cyc_Marshal_cycgetchar(fp);i |=j << pos;pos +=8;}}return(
unsigned int)i;}struct _tuple6 Cyc_Marshal_write_type_base(void*rep,struct _tuple6
env,struct Cyc_Std___cycFILE*fp,void*val){{void*_tmpC9=rep;unsigned int _tmpCA;
unsigned int _tmpCB;void*_tmpCC;void*_tmpCD;struct _tagged_arr _tmpCE;struct
_tagged_arr _tmpCF;struct _tagged_arr _tmpD0;struct _tagged_arr _tmpD1;struct
_tagged_arr _tmpD2;struct _tagged_arr _tmpD3;int _tmpD4;int _tmpD5;_LL24: if(_tmpC9 <= (
void*)2?1:*((int*)_tmpC9)!= 0)goto _LL26;_tmpCA=((struct Cyc_Typerep_Int_struct*)
_tmpC9)->f2;_LL25: switch(_tmpCA){case 8: _LL3C: {char*x=((char*(*)(void*x))
unsafe_cast)(val);Cyc_Marshal_cycputchar(*x,fp);break;}case 16: _LL3D: {short*x=((
short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*x,fp);break;}case 32:
_LL3E: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*x,fp);
break;}default: _LL3F:({struct Cyc_Std_Int_pa_struct _tmpD7;_tmpD7.tag=1;_tmpD7.f1=(
int)_tmpCA;{void*_tmpD6[1]={& _tmpD7};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("write_type_base: illegal int size %d bits\n",
sizeof(char),43),_tag_arr(_tmpD6,sizeof(void*),1));}});exit(1);}goto _LL23;_LL26:
if((int)_tmpC9 != 0)goto _LL28;_LL27: {float*x=((float*(*)(void*x))unsafe_cast)(
val);Cyc_Marshal_cycputfloat(*x,fp);goto _LL23;}_LL28: if((int)_tmpC9 != 1)goto
_LL2A;_LL29: {double*x=((double*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*
x,fp);goto _LL23;}_LL2A: if(_tmpC9 <= (void*)2?1:*((int*)_tmpC9)!= 1)goto _LL2C;
_tmpCB=((struct Cyc_Typerep_ThinPtr_struct*)_tmpC9)->f1;_tmpCC=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmpC9)->f2;_LL2B: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));unsigned int _tmpD8=Cyc_Marshal_index_lookup(env,(unsigned int)y);
Cyc_Marshal_cycputvarint(_tmpD8,fp);if(!Cyc_Marshal_index_member(env,_tmpD8)){
env=Cyc_Marshal_index_insert(env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmpCC);int i=0;for(0;i < _tmpCB;i ++){env=Cyc_Marshal_write_type_base(_tmpCC,env,
fp,((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}goto _LL23;}_LL2C: if(_tmpC9 <= (
void*)2?1:*((int*)_tmpC9)!= 2)goto _LL2E;_tmpCD=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmpC9)->f1;_LL2D: {struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);
unsigned int typ_szb=Cyc_Typerep_size_type(_tmpCD);unsigned int base=(*x).f1;
unsigned int curr=(*x).f2;unsigned int last_plus_one=(*x).f3;unsigned int pos=(curr
- base)/ typ_szb;unsigned int sz=(last_plus_one - base)/ typ_szb;unsigned int
_tmpD9=Cyc_Marshal_index_lookup(env,base);Cyc_Marshal_cycputvarint(_tmpD9,fp);
Cyc_Marshal_cycputvarint(pos,fp);Cyc_Marshal_cycputvarint(sz,fp);if(!Cyc_Marshal_index_member(
env,_tmpD9)){env=Cyc_Marshal_index_insert(env,base);{unsigned int i=base;for(0;i < 
last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(_tmpCD,env,fp,((void*(*)(
unsigned int x))unsafe_cast)(i));}}}goto _LL23;}_LL2E: if(_tmpC9 <= (void*)2?1:*((
int*)_tmpC9)!= 3)goto _LL30;_tmpCE=((struct Cyc_Typerep_Struct_struct*)_tmpC9)->f3;
_LL2F: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(
_tmpCE,sizeof(struct _tuple0*));i ++){unsigned int _tmpDB;void*_tmpDC;struct _tuple0
_tmpDA=*((struct _tuple0**)_tmpCE.curr)[i];_tmpDB=_tmpDA.f1;_tmpDC=_tmpDA.f3;{int
x_ofs=(int)(x + _tmpDB);env=Cyc_Marshal_write_type_base(_tmpDC,env,fp,((void*(*)(
int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL30: if(_tmpC9 <= (void*)2?1:*((int*)
_tmpC9)!= 4)goto _LL32;_tmpCF=((struct Cyc_Typerep_Tuple_struct*)_tmpC9)->f2;_LL31: {
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpCF,
sizeof(struct _tuple1*));i ++){unsigned int _tmpDE;void*_tmpDF;struct _tuple1 _tmpDD=*((
struct _tuple1**)_tmpCF.curr)[i];_tmpDE=_tmpDD.f1;_tmpDF=_tmpDD.f2;{int x_ofs=(int)(
x + _tmpDE);env=Cyc_Marshal_write_type_base(_tmpDF,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LL23;}_LL32: if(_tmpC9 <= (void*)2?1:*((int*)_tmpC9)
!= 6)goto _LL34;_tmpD0=((struct Cyc_Typerep_TUnionField_struct*)_tmpC9)->f4;_LL33: {
int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmpD0,
sizeof(struct _tuple1*));i ++){unsigned int _tmpE1;void*_tmpE2;struct _tuple1 _tmpE0=*((
struct _tuple1**)_tmpD0.curr)[i];_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;{int x_ofs=(int)(
x + _tmpE1);env=Cyc_Marshal_write_type_base(_tmpE2,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LL23;}_LL34: if(_tmpC9 <= (void*)2?1:*((int*)_tmpC9)
!= 5)goto _LL36;_tmpD1=((struct Cyc_Typerep_TUnion_struct*)_tmpC9)->f2;_tmpD2=((
struct Cyc_Typerep_TUnion_struct*)_tmpC9)->f3;_LL35: {unsigned int*tagp=((
unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024)Cyc_Marshal_cycputvarint(*
tagp,fp);else{unsigned int _tmpE3=Cyc_Marshal_index_lookup(env,*tagp);Cyc_Marshal_cycputvarint(
_tmpE3 + 1024,fp);if(!Cyc_Marshal_index_member(env,_tmpE3)){env=Cyc_Marshal_index_insert(
env,*tagp);{unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);
unsigned int tag=*x;Cyc_Marshal_cycputvarint(tag,fp);{int _tmpE5;void*_tmpE6;
struct _tuple4 _tmpE4=Cyc_Typerep_get_unionbranch(tag,_tmpD2);_tmpE5=_tmpE4.f1;
_tmpE6=_tmpE4.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*tagp + _tmpE5);
env=Cyc_Marshal_write_type_base(_tmpE6,env,fp,xoff);}}}}}goto _LL23;}_LL36: if(
_tmpC9 <= (void*)2?1:*((int*)_tmpC9)!= 7)goto _LL38;_tmpD3=((struct Cyc_Typerep_XTUnion_struct*)
_tmpC9)->f2;_LL37: {unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(
val);unsigned int _tmpE7=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint(
_tmpE7,fp);if(!Cyc_Marshal_index_member(env,_tmpE7)){env=Cyc_Marshal_index_insert(
env,*xtunionp);{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))
unsafe_cast)(*xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',fp);{
char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*xtstructp + 4);struct
_tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _tagged_arr)xtname;{void*_tmpE8[1]={& _tmpE9};
Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(char),3),_tag_arr(_tmpE8,sizeof(void*),1));}});
Cyc_Marshal_cycputchar('\000',fp);{int _tmpEB;void*_tmpEC;struct _tuple4 _tmpEA=Cyc_Typerep_get_xtunionbranch(
xtname,_tmpD3);_tmpEB=_tmpEA.f1;_tmpEC=_tmpEA.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*xtunionp + _tmpEB);env=Cyc_Marshal_write_type_base(
_tmpEC,env,fp,xoff);}}}}else{Cyc_Marshal_cycputchar('\000',fp);{char*xtnamec=((
char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct _tagged_arr xtname=(
struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _tagged_arr)xtname;{void*_tmpED[1]={& _tmpEE};
Cyc_Std_fprintf(fp,_tag_arr("%s",sizeof(char),3),_tag_arr(_tmpED,sizeof(void*),1));}});
Cyc_Marshal_cycputchar('\000',fp);}}}}goto _LL23;}_LL38: if(_tmpC9 <= (void*)2?1:*((
int*)_tmpC9)!= 8)goto _LL3A;_tmpD4=((struct Cyc_Typerep_Union_struct*)_tmpC9)->f2;
_LL39: env=Cyc_Marshal_write_type_base((void*)({struct Cyc_Typerep_Tuple_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Typerep_Tuple_struct
_tmpF0;_tmpF0.tag=4;_tmpF0.f1=(unsigned int)_tmpD4;_tmpF0.f2=({unsigned int
_tmpF1=(unsigned int)_tmpD4;struct _tuple1**_tmpF2=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmpF1));struct _tagged_arr _tmpF7=_tag_arr(
_tmpF2,sizeof(struct _tuple1*),(unsigned int)_tmpD4);{unsigned int _tmpF3=_tmpF1;
unsigned int i;for(i=0;i < _tmpF3;i ++){_tmpF2[i]=({struct _tuple1*_tmpF4=_cycalloc(
sizeof(*_tmpF4));_tmpF4->f1=i;_tmpF4->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmpF5=_cycalloc_atomic(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Typerep_Int_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=0;_tmpF6.f2=8;_tmpF6;});_tmpF5;});_tmpF4;});}}
_tmpF7;});_tmpF0;});_tmpEF;}),env,fp,val);goto _LL23;_LL3A: if(_tmpC9 <= (void*)2?1:*((
int*)_tmpC9)!= 9)goto _LL23;_tmpD5=((struct Cyc_Typerep_Enum_struct*)_tmpC9)->f2;
_LL3B: env=Cyc_Marshal_write_type_base((void*)({struct Cyc_Typerep_Int_struct*
_tmpF8=_cycalloc_atomic(sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Typerep_Int_struct
_tmpF9;_tmpF9.tag=0;_tmpF9.f1=0;_tmpF9.f2=(unsigned int)(_tmpD5 << 3);_tmpF9;});
_tmpF8;}),env,fp,val);_LL23:;}return env;}void Cyc_Marshal_write_type(void*ts,
struct Cyc_Std___cycFILE*fp,void*val){struct _handler_cons _tmpFA;_push_handler(&
_tmpFA);{int _tmpFC=0;if(setjmp(_tmpFA.handler))_tmpFC=1;if(!_tmpFC){Cyc_Marshal_write_type_base(
ts,Cyc_Marshal_empty_addr_index(),fp,val);;_pop_handler();}else{void*_tmpFB=(
void*)_exn_thrown;void*_tmpFE=_tmpFB;struct _tagged_arr _tmpFF;_LL42: if(*((void**)
_tmpFE)!= Cyc_Core_Failure)goto _LL44;_tmpFF=((struct Cyc_Core_Failure_struct*)
_tmpFE)->f1;_LL43:({struct Cyc_Std_String_pa_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(
struct _tagged_arr)_tmpFF;{void*_tmp100[1]={& _tmp101};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n",sizeof(char),19),_tag_arr(_tmp100,sizeof(void*),1));}});(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp102=_cycalloc(sizeof(*
_tmp102));_tmp102[0]=({struct Cyc_Core_Failure_struct _tmp103;_tmp103.tag=Cyc_Core_Failure;
_tmp103.f1=_tmpFF;_tmp103;});_tmp102;}));_LL44:;_LL45:(void)_throw(_tmpFE);_LL41:;}}}
void Cyc_Marshal_fscanntstring(struct Cyc_Std___cycFILE*fp,struct _tagged_arr s){int
c;int i=0;while(c=Cyc_Std_fgetc(fp)){if(c == - 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_Core_Failure_struct
_tmp105;_tmp105.tag=Cyc_Core_Failure;_tmp105.f1=_tag_arr("Read error",sizeof(
char),11);_tmp105;});_tmp104;}));((char*)s.curr)[i]=(char)c;i ++;}((char*)s.curr)[
i]='\000';}struct _tuple7 Cyc_Marshal_rread_type_base(struct _RegionHandle*r,void*
rep,struct _tuple7 env,struct Cyc_Std___cycFILE*fp,void*result){{void*_tmp106=rep;
unsigned int _tmp107;unsigned int _tmp108;void*_tmp109;void*_tmp10A;struct
_tagged_arr _tmp10B;struct _tagged_arr _tmp10C;struct _tagged_arr _tmp10D;struct
_tagged_arr _tmp10E;struct _tagged_arr _tmp10F;struct _tagged_arr _tmp110;int _tmp111;
int _tmp112;_LL47: if(_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 0)goto _LL49;_tmp107=((
struct Cyc_Typerep_Int_struct*)_tmp106)->f2;_LL48: switch(_tmp107){case 8: _LL5F: {
char*x=((char*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetchar(fp);
break;}case 16: _LL60: {short*x=((short*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetshort(
fp);break;}case 32: _LL61: {int*x=((int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(
fp);break;}default: _LL62:(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Core_Failure_struct
_tmp114;_tmp114.tag=Cyc_Core_Failure;_tmp114.f1=_tag_arr("read_type_base: illegal int size bits",
sizeof(char),38);_tmp114;});_tmp113;}));}goto _LL46;_LL49: if((int)_tmp106 != 0)
goto _LL4B;_LL4A: {float*y=((float*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetfloat(
fp);goto _LL46;}_LL4B: if((int)_tmp106 != 1)goto _LL4D;_LL4C: {double*y=((double*(*)(
void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(fp);goto _LL46;}_LL4D:
if(_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 1)goto _LL4F;_tmp108=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp106)->f1;_tmp109=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp106)->f2;
_LL4E: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmp109);struct
_tagged_arr buf=({unsigned int _tmp115=_tmp108 * typ_szb;char*_tmp116=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp115));struct _tagged_arr _tmp118=
_tag_arr(_tmp116,sizeof(char),_tmp108 * typ_szb);{unsigned int _tmp117=_tmp115;
unsigned int i;for(i=0;i < _tmp117;i ++){_tmp116[i]='\000';}}_tmp118;});
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,sizeof(char),
1)));unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;
env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int i=0;for(0;i < 
_tmp108;i ++){env=Cyc_Marshal_rread_type_base(r,_tmp109,env,fp,((void*(*)(int x))
unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL46;}_LL4F: if(
_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 2)goto _LL51;_tmp10A=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp106)->f1;_LL50: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);int pos=(int)Cyc_Marshal_cycgetvarint(
fp);int sz=(int)Cyc_Marshal_cycgetvarint(fp);int typ_szb=(int)Cyc_Typerep_size_type(
_tmp10A);if(!Cyc_Marshal_table_member(env,(unsigned int)mem)){struct _tagged_arr
buf=({unsigned int _tmp119=(unsigned int)(sz * typ_szb);char*_tmp11A=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp119));struct _tagged_arr _tmp11C=
_tag_arr(_tmp11A,sizeof(char),(unsigned int)(sz * typ_szb));{unsigned int _tmp11B=
_tmp119;unsigned int i;for(i=0;i < _tmp11B;i ++){_tmp11A[i]='\000';}}_tmp11C;});
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,sizeof(char),
1)));struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(result);(*x).f1=
bufptr;(*x).f2=bufptr + pos * typ_szb;(*x).f3=bufptr + sz * typ_szb;env=Cyc_Marshal_table_insert(
env,bufptr);{int y=(int)bufptr;int i=0;for(0;i < sz;i ++){env=Cyc_Marshal_rread_type_base(
r,_tmp10A,env,fp,((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{struct
_tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(result);(*x).f1=Cyc_Marshal_table_lookup(
env,mem);(*x).f2=(*x).f1 + typ_szb * pos;(*x).f3=(*x).f1 + typ_szb * sz;}goto _LL46;}
_LL51: if(_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 3)goto _LL53;_tmp10B=((struct Cyc_Typerep_Struct_struct*)
_tmp106)->f3;_LL52: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{
int i=0;for(0;i < _get_arr_size(_tmp10B,sizeof(struct _tuple0*));i ++){unsigned int
_tmp11E;void*_tmp11F;struct _tuple0 _tmp11D=*((struct _tuple0**)_tmp10B.curr)[i];
_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f3;{int x_ofs=(int)(x + _tmp11E);env=Cyc_Marshal_rread_type_base(
r,_tmp11F,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL53: if(
_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 4)goto _LL55;_tmp10C=((struct Cyc_Typerep_Tuple_struct*)
_tmp106)->f2;_LL54: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{
int i=0;for(0;i < _get_arr_size(_tmp10C,sizeof(struct _tuple1*));i ++){unsigned int
_tmp121;void*_tmp122;struct _tuple1 _tmp120=*((struct _tuple1**)_tmp10C.curr)[i];
_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;{int x_ofs=(int)(x + _tmp121);env=Cyc_Marshal_rread_type_base(
r,_tmp122,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL55: if(
_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 6)goto _LL57;_tmp10D=((struct Cyc_Typerep_TUnionField_struct*)
_tmp106)->f4;_LL56: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{
int i=0;for(0;i < _get_arr_size(_tmp10D,sizeof(struct _tuple1*));i ++){unsigned int
_tmp124;void*_tmp125;struct _tuple1 _tmp123=*((struct _tuple1**)_tmp10D.curr)[i];
_tmp124=_tmp123.f1;_tmp125=_tmp123.f2;{int x_ofs=(int)(x + _tmp124);env=Cyc_Marshal_rread_type_base(
r,_tmp125,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL57: if(
_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 5)goto _LL59;_tmp10E=((struct Cyc_Typerep_TUnion_struct*)
_tmp106)->f2;_tmp10F=((struct Cyc_Typerep_TUnion_struct*)_tmp106)->f3;_LL58: {int
tagmem=(int)Cyc_Marshal_cycgetvarint(fp);if(tagmem < 1024){int tag=tagmem;
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(unsigned int)
tag;}else{int mem=tagmem - 1024;if(!Cyc_Marshal_table_member(env,(unsigned int)mem)){
int tag=(int)Cyc_Marshal_cycgetvarint(fp);int _tmp127;void*_tmp128;struct _tuple4
_tmp126=Cyc_Typerep_get_unionbranch((unsigned int)tag,_tmp10F);_tmp127=_tmp126.f1;
_tmp128=_tmp126.f2;{struct _tagged_arr buf=({unsigned int _tmp129=_tmp127 + Cyc_Typerep_size_type(
_tmp128);char*_tmp12A=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp129));
struct _tagged_arr _tmp12C=_tag_arr(_tmp12A,sizeof(char),_tmp127 + Cyc_Typerep_size_type(
_tmp128));{unsigned int _tmp12B=_tmp129;unsigned int i;for(i=0;i < _tmp12B;i ++){
_tmp12A[i]='\000';}}_tmp12C;});unsigned int bufptr=(unsigned int)((char*)
_check_null(_untag_arr(buf,sizeof(char),1)));env=Cyc_Marshal_table_insert(env,
bufptr);{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(
bufptr);*tagptr=(unsigned int)tag;env=Cyc_Marshal_rread_type_base(r,_tmp128,env,
fp,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp127));}}}}else{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(
env,mem);}}goto _LL46;}_LL59: if(_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 7)goto
_LL5B;_tmp110=((struct Cyc_Typerep_XTUnion_struct*)_tmp106)->f2;_LL5A: {int mem=(
int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(env,(unsigned int)
mem)){char nonnull=(char)Cyc_Marshal_cycgetchar(fp);struct _tagged_arr s=_tag_arr(({
unsigned int _tmp134=(unsigned int)100;char*_tmp135=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp134));{unsigned int _tmp136=_tmp134;unsigned int i;
for(i=0;i < _tmp136;i ++){_tmp135[i]='\000';}}_tmp135;}),sizeof(char),(
unsigned int)100);struct _tagged_arr xtname=_tagged_arr_plus(s,sizeof(char),4);Cyc_Marshal_fscanntstring(
fp,xtname);if((int)nonnull){int _tmp12E;void*_tmp12F;struct _tuple4 _tmp12D=Cyc_Typerep_get_xtunionbranch((
struct _tagged_arr)xtname,_tmp110);_tmp12E=_tmp12D.f1;_tmp12F=_tmp12D.f2;{struct
_tagged_arr buf=({unsigned int _tmp130=_tmp12E + Cyc_Typerep_size_type(_tmp12F);
char*_tmp131=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp130));struct
_tagged_arr _tmp133=_tag_arr(_tmp131,sizeof(char),_tmp12E + Cyc_Typerep_size_type(
_tmp12F));{unsigned int _tmp132=_tmp130;unsigned int i;for(i=0;i < _tmp132;i ++){
_tmp131[i]='\000';}}_tmp133;});unsigned int bufptr=(unsigned int)((char*)
_check_null(_untag_arr(buf,sizeof(char),1)));env=Cyc_Marshal_table_insert(env,
bufptr);{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=
bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(
bufptr);*tagptr=(unsigned int)((char*)_check_null(_untag_arr(s,sizeof(char),1)));
env=Cyc_Marshal_rread_type_base(r,_tmp12F,env,fp,((void*(*)(unsigned int x))
unsafe_cast)(bufptr + _tmp12E));}}}}else{unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=(unsigned int)((char*)_check_null(_untag_arr(s,sizeof(
char),1)));env=Cyc_Marshal_table_insert(env,*x);}}else{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,
mem);}goto _LL46;}_LL5B: if(_tmp106 <= (void*)2?1:*((int*)_tmp106)!= 8)goto _LL5D;
_tmp111=((struct Cyc_Typerep_Union_struct*)_tmp106)->f2;_LL5C: env=Cyc_Marshal_rread_type_base(
r,(void*)({struct Cyc_Typerep_Tuple_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Typerep_Tuple_struct _tmp138;_tmp138.tag=4;_tmp138.f1=(
unsigned int)_tmp111;_tmp138.f2=({unsigned int _tmp139=(unsigned int)_tmp111;
struct _tuple1**_tmp13A=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct
_tuple1*),_tmp139));struct _tagged_arr _tmp13F=_tag_arr(_tmp13A,sizeof(struct
_tuple1*),(unsigned int)_tmp111);{unsigned int _tmp13B=_tmp139;unsigned int i;for(
i=0;i < _tmp13B;i ++){_tmp13A[i]=({struct _tuple1*_tmp13C=_cycalloc(sizeof(*_tmp13C));
_tmp13C->f1=i;_tmp13C->f2=(void*)({struct Cyc_Typerep_Int_struct*_tmp13D=
_cycalloc_atomic(sizeof(*_tmp13D));_tmp13D[0]=({struct Cyc_Typerep_Int_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=0;_tmp13E.f2=8;_tmp13E;});_tmp13D;});_tmp13C;});}}
_tmp13F;});_tmp138;});_tmp137;}),env,fp,result);goto _LL46;_LL5D: if(_tmp106 <= (
void*)2?1:*((int*)_tmp106)!= 9)goto _LL46;_tmp112=((struct Cyc_Typerep_Enum_struct*)
_tmp106)->f2;_LL5E: env=Cyc_Marshal_rread_type_base(r,(void*)({struct Cyc_Typerep_Int_struct*
_tmp140=_cycalloc_atomic(sizeof(*_tmp140));_tmp140[0]=({struct Cyc_Typerep_Int_struct
_tmp141;_tmp141.tag=0;_tmp141.f1=0;_tmp141.f2=(unsigned int)(_tmp112 << 3);
_tmp141;});_tmp140;}),env,fp,result);_LL46:;}return env;}void*Cyc_Marshal_rread_type(
struct _RegionHandle*r,void*ts,struct Cyc_Std___cycFILE*fp){struct _handler_cons
_tmp142;_push_handler(& _tmp142);{int _tmp144=0;if(setjmp(_tmp142.handler))_tmp144=
1;if(!_tmp144){{struct _tagged_arr buf=({unsigned int _tmp146=Cyc_Typerep_size_type(
ts);char*_tmp147=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp146));
struct _tagged_arr _tmp149=_tag_arr(_tmp147,sizeof(char),Cyc_Typerep_size_type(ts));{
unsigned int _tmp148=_tmp146;unsigned int i;for(i=0;i < _tmp148;i ++){_tmp147[i]='\000';}}
_tmp149;});void*result=((void*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_arr(buf,sizeof(char),1)));Cyc_Marshal_rread_type_base(r,ts,Cyc_Marshal_empty_addr_table(),
fp,result);{void*_tmp145=result;_npop_handler(0);return _tmp145;}};_pop_handler();}
else{void*_tmp143=(void*)_exn_thrown;void*_tmp14B=_tmp143;struct _tagged_arr
_tmp14C;_LL65: if(*((void**)_tmp14B)!= Cyc_Core_Failure)goto _LL67;_tmp14C=((
struct Cyc_Core_Failure_struct*)_tmp14B)->f1;_LL66:({struct Cyc_Std_String_pa_struct
_tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _tagged_arr)_tmp14C;{void*_tmp14D[1]={&
_tmp14E};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(
char),19),_tag_arr(_tmp14D,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=({struct Cyc_Core_Failure_struct
_tmp150;_tmp150.tag=Cyc_Core_Failure;_tmp150.f1=_tmp14C;_tmp150;});_tmp14F;}));
_LL67:;_LL68:(void)_throw(_tmp14B);_LL64:;}}}void*Cyc_Marshal_read_type(void*ts,
struct Cyc_Std___cycFILE*fp){return Cyc_Marshal_rread_type(Cyc_Core_heap_region,ts,
fp);}static struct Cyc_Dict_Dict*Cyc_Marshal_empty_addr_map(){return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(
int(*cmp)(unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);}
struct Cyc_Dict_Dict*Cyc_Marshal_rcopy_type_base(struct _RegionHandle*r,void*rep,
struct Cyc_Dict_Dict*env,void*val,void*result){{void*_tmp151=rep;unsigned int
_tmp152;unsigned int _tmp153;unsigned int _tmp154;unsigned int _tmp155;unsigned int
_tmp156;void*_tmp157;void*_tmp158;struct _tagged_arr _tmp159;struct _tagged_arr
_tmp15A;struct _tagged_arr _tmp15B;struct _tagged_arr _tmp15C;struct _tagged_arr
_tmp15D;struct _tagged_arr _tmp15E;int _tmp15F;int _tmp160;_LL6A: if(_tmp151 <= (void*)
2?1:*((int*)_tmp151)!= 0)goto _LL6C;_tmp152=((struct Cyc_Typerep_Int_struct*)
_tmp151)->f2;if(_tmp152 != 8)goto _LL6C;_LL6B: {char*val0=((char*(*)(void*x))
unsafe_cast)(val);char*result0=((char*(*)(void*x))unsafe_cast)(result);*result0=*
val0;goto _LL69;}_LL6C: if(_tmp151 <= (void*)2?1:*((int*)_tmp151)!= 0)goto _LL6E;
_tmp153=((struct Cyc_Typerep_Int_struct*)_tmp151)->f2;if(_tmp153 != 16)goto _LL6E;
_LL6D: {short*val0=((short*(*)(void*x))unsafe_cast)(val);short*result0=((short*(*)(
void*x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL6E: if(_tmp151 <= (void*)
2?1:*((int*)_tmp151)!= 0)goto _LL70;_tmp154=((struct Cyc_Typerep_Int_struct*)
_tmp151)->f2;if(_tmp154 != 32)goto _LL70;_LL6F: {int*val0=((int*(*)(void*x))
unsafe_cast)(val);int*result0=((int*(*)(void*x))unsafe_cast)(result);*result0=*
val0;goto _LL69;}_LL70: if(_tmp151 <= (void*)2?1:*((int*)_tmp151)!= 0)goto _LL72;
_tmp155=((struct Cyc_Typerep_Int_struct*)_tmp151)->f2;if(_tmp155 != 64)goto _LL72;
_LL71: {long long*val0=((long long*(*)(void*x))unsafe_cast)(val);long long*
result0=((long long*(*)(void*x))unsafe_cast)(result);*result0=*val0;goto _LL69;}
_LL72: if((int)_tmp151 != 0)goto _LL74;_LL73: {float*val0=((float*(*)(void*x))
unsafe_cast)(val);float*result0=((float*(*)(void*x))unsafe_cast)(result);*
result0=*val0;goto _LL69;}_LL74: if((int)_tmp151 != 1)goto _LL76;_LL75: {double*val0=((
double*(*)(void*x))unsafe_cast)(val);double*result0=((double*(*)(void*x))
unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL76: if(_tmp151 <= (void*)2?1:*((
int*)_tmp151)!= 1)goto _LL78;_tmp156=((struct Cyc_Typerep_ThinPtr_struct*)_tmp151)->f1;
_tmp157=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp151)->f2;_LL77: {
unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(val);unsigned int*
result0=((unsigned int*(*)(void*x))unsafe_cast)(result);int addr=(int)*val0;if(((
int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(env,(unsigned int)
addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup)(
env,(unsigned int)addr);else{int typ_szb=(int)Cyc_Typerep_size_type(_tmp157);
struct _tagged_arr buf=({unsigned int _tmp161=_tmp156 * typ_szb;char*_tmp162=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp161));struct _tagged_arr _tmp164=
_tag_arr(_tmp162,sizeof(char),_tmp156 * typ_szb);{unsigned int _tmp163=_tmp161;
unsigned int i;for(i=0;i < _tmp163;i ++){_tmp162[i]='\000';}}_tmp164;});
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,sizeof(char),
1)));*result0=bufptr;env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(env,(unsigned int)addr,bufptr);{
int x=addr;int y=(int)bufptr;int i=0;for(0;i < _tmp156;i ++){env=Cyc_Marshal_rcopy_type_base(
r,_tmp157,env,((void*(*)(int x))unsafe_cast)(x),((void*(*)(int x))unsafe_cast)(y));
x +=typ_szb;y +=typ_szb;}}}goto _LL69;}_LL78: if(_tmp151 <= (void*)2?1:*((int*)
_tmp151)!= 2)goto _LL7A;_tmp158=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp151)->f1;
_LL79: {struct _tuple9*val0=((struct _tuple9*(*)(void*x))unsafe_cast)(val);struct
_tuple9*result0=((struct _tuple9*(*)(void*x))unsafe_cast)(result);unsigned int
_tmp166;unsigned int _tmp167;unsigned int _tmp168;struct _tuple9 _tmp165=*val0;
_tmp166=_tmp165.f1;_tmp167=_tmp165.f2;_tmp168=_tmp165.f3;{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp158);int size=(int)((_tmp168 - _tmp166)/ typ_szb);int pos=(int)((_tmp167 - 
_tmp166)/ typ_szb);if(((int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(
env,_tmp166)){unsigned int _tmp169=((unsigned int(*)(struct Cyc_Dict_Dict*d,
unsigned int k))Cyc_Dict_lookup)(env,_tmp166);*result0=({struct _tuple9 _tmp16A;
_tmp16A.f1=_tmp169;_tmp16A.f2=_tmp169 + pos * typ_szb;_tmp16A.f3=_tmp169 + size * 
typ_szb;_tmp16A;});}else{struct _tagged_arr buf=({unsigned int _tmp16C=_tmp168 - 
_tmp166;char*_tmp16D=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp16C));
struct _tagged_arr _tmp16F=_tag_arr(_tmp16D,sizeof(char),_tmp168 - _tmp166);{
unsigned int _tmp16E=_tmp16C;unsigned int i;for(i=0;i < _tmp16E;i ++){_tmp16D[i]='\000';}}
_tmp16F;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,
sizeof(char),1)));*result0=({struct _tuple9 _tmp16B;_tmp16B.f1=bufptr;_tmp16B.f2=
bufptr + pos * typ_szb;_tmp16B.f3=bufptr + size * typ_szb;_tmp16B;});env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,_tmp166,
bufptr);{int y=(int)_tmp166;int z=(int)bufptr;int i=0;for(0;i < size;i ++){env=Cyc_Marshal_rcopy_type_base(
r,_tmp158,env,((void*(*)(int x))unsafe_cast)(y),((void*(*)(int x))unsafe_cast)(z));
y +=typ_szb;z +=typ_szb;}}}goto _LL69;}}_LL7A: if(_tmp151 <= (void*)2?1:*((int*)
_tmp151)!= 3)goto _LL7C;_tmp159=((struct Cyc_Typerep_Struct_struct*)_tmp151)->f3;
_LL7B: {int x=((int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*x))
unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmp159,sizeof(struct _tuple0*));
i ++){unsigned int _tmp171;void*_tmp172;struct _tuple0 _tmp170=*((struct _tuple0**)
_tmp159.curr)[i];_tmp171=_tmp170.f1;_tmp172=_tmp170.f3;{int x_ofs=(int)(x + 
_tmp171);int y_ofs=(int)(y + _tmp171);env=Cyc_Marshal_rcopy_type_base(r,_tmp172,
env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}
goto _LL69;}_LL7C: if(_tmp151 <= (void*)2?1:*((int*)_tmp151)!= 4)goto _LL7E;_tmp15A=((
struct Cyc_Typerep_Tuple_struct*)_tmp151)->f2;_LL7D: {int x=((int(*)(void*x))
unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < 
_get_arr_size(_tmp15A,sizeof(struct _tuple1*));i ++){unsigned int _tmp174;void*
_tmp175;struct _tuple1 _tmp173=*((struct _tuple1**)_tmp15A.curr)[i];_tmp174=_tmp173.f1;
_tmp175=_tmp173.f2;{int x_ofs=(int)(x + _tmp174);int y_ofs=(int)(y + _tmp174);env=
Cyc_Marshal_rcopy_type_base(r,_tmp175,env,((void*(*)(int x))unsafe_cast)(x_ofs),((
void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL7E: if(_tmp151 <= (void*)2?1:*((
int*)_tmp151)!= 6)goto _LL80;_tmp15B=((struct Cyc_Typerep_TUnionField_struct*)
_tmp151)->f4;_LL7F: {int x=((int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*
x))unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmp15B,sizeof(struct
_tuple1*));i ++){unsigned int _tmp177;void*_tmp178;struct _tuple1 _tmp176=*((struct
_tuple1**)_tmp15B.curr)[i];_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;{int x_ofs=(int)(
x + _tmp177);int y_ofs=(int)(y + _tmp177);env=Cyc_Marshal_rcopy_type_base(r,_tmp178,
env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}
goto _LL69;}_LL80: if(_tmp151 <= (void*)2?1:*((int*)_tmp151)!= 5)goto _LL82;_tmp15C=((
struct Cyc_Typerep_TUnion_struct*)_tmp151)->f2;_tmp15D=((struct Cyc_Typerep_TUnion_struct*)
_tmp151)->f3;_LL81: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(
val);unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);if(*
val0 < 1024)*result0=*val0;else{unsigned int addr=*val0;if(((int(*)(struct Cyc_Dict_Dict*
d,unsigned int k))Cyc_Dict_member)(env,addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict*
d,unsigned int k))Cyc_Dict_lookup)(env,addr);else{unsigned int*tufp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*val0);unsigned int tag=*tufp;int
_tmp17A;void*_tmp17B;struct _tuple4 _tmp179=Cyc_Typerep_get_unionbranch(tag,
_tmp15D);_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{struct _tagged_arr buf=({
unsigned int _tmp17C=_tmp17A + Cyc_Typerep_size_type(_tmp17B);char*_tmp17D=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp17C));struct _tagged_arr _tmp17F=
_tag_arr(_tmp17D,sizeof(char),_tmp17A + Cyc_Typerep_size_type(_tmp17B));{
unsigned int _tmp17E=_tmp17C;unsigned int i;for(i=0;i < _tmp17E;i ++){_tmp17D[i]='\000';}}
_tmp17F;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,
sizeof(char),1)));env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(env,addr,bufptr);*result0=bufptr;{
unsigned int*newtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*
newtagp=tag;env=Cyc_Marshal_rcopy_type_base(r,_tmp17B,env,((void*(*)(
unsigned int x))unsafe_cast)(addr + _tmp17A),((void*(*)(unsigned int x))unsafe_cast)(
bufptr + _tmp17A));}}}}goto _LL69;}_LL82: if(_tmp151 <= (void*)2?1:*((int*)_tmp151)
!= 7)goto _LL84;_tmp15E=((struct Cyc_Typerep_XTUnion_struct*)_tmp151)->f2;_LL83: {
unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(val);unsigned int*
result0=((unsigned int*(*)(void*x))unsafe_cast)(result);unsigned int addr=*val0;
if(((int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(env,addr))*
result0=((unsigned int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup)(
env,addr);else{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))
unsafe_cast)(*val0);if(*xtstructp != 0){unsigned int xtnamep=*xtstructp;char*
xtnamec=((char*(*)(unsigned int x))unsafe_cast)(xtnamep + 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);int _tmp181;void*_tmp182;
struct _tuple4 _tmp180=Cyc_Typerep_get_xtunionbranch(xtname,_tmp15E);_tmp181=
_tmp180.f1;_tmp182=_tmp180.f2;{struct _tagged_arr buf=({unsigned int _tmp183=
_tmp181 + Cyc_Typerep_size_type(_tmp182);char*_tmp184=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmp183));struct _tagged_arr _tmp186=_tag_arr(_tmp184,
sizeof(char),_tmp181 + Cyc_Typerep_size_type(_tmp182));{unsigned int _tmp185=
_tmp183;unsigned int i;for(i=0;i < _tmp185;i ++){_tmp184[i]='\000';}}_tmp186;});
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,sizeof(char),
1)));*result0=bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))
unsafe_cast)(bufptr);*tagptr=xtnamep;{void*xoff=((void*(*)(unsigned int x))
unsafe_cast)(*val0 + _tmp181);env=Cyc_Marshal_rcopy_type_base(r,_tmp182,env,xoff,((
void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp181));env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,addr,
bufptr);}}}}else{*result0=*val0;}}goto _LL69;}_LL84: if(_tmp151 <= (void*)2?1:*((
int*)_tmp151)!= 8)goto _LL86;_tmp15F=((struct Cyc_Typerep_Union_struct*)_tmp151)->f2;
_LL85: env=Cyc_Marshal_rcopy_type_base(r,(void*)({struct Cyc_Typerep_Tuple_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Typerep_Tuple_struct
_tmp188;_tmp188.tag=4;_tmp188.f1=(unsigned int)_tmp15F;_tmp188.f2=({unsigned int
_tmp189=(unsigned int)_tmp15F;struct _tuple1**_tmp18A=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp189));struct _tagged_arr _tmp18F=_tag_arr(
_tmp18A,sizeof(struct _tuple1*),(unsigned int)_tmp15F);{unsigned int _tmp18B=
_tmp189;unsigned int i;for(i=0;i < _tmp18B;i ++){_tmp18A[i]=({struct _tuple1*_tmp18C=
_cycalloc(sizeof(*_tmp18C));_tmp18C->f1=i;_tmp18C->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp18D=_cycalloc_atomic(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Typerep_Int_struct
_tmp18E;_tmp18E.tag=0;_tmp18E.f1=0;_tmp18E.f2=8;_tmp18E;});_tmp18D;});_tmp18C;});}}
_tmp18F;});_tmp188;});_tmp187;}),env,val,result);goto _LL69;_LL86: if(_tmp151 <= (
void*)2?1:*((int*)_tmp151)!= 9)goto _LL88;_tmp160=((struct Cyc_Typerep_Enum_struct*)
_tmp151)->f2;_LL87: env=Cyc_Marshal_rcopy_type_base(r,(void*)({struct Cyc_Typerep_Int_struct*
_tmp190=_cycalloc_atomic(sizeof(*_tmp190));_tmp190[0]=({struct Cyc_Typerep_Int_struct
_tmp191;_tmp191.tag=0;_tmp191.f1=0;_tmp191.f2=(unsigned int)(_tmp160 << 3);
_tmp191;});_tmp190;}),env,val,result);goto _LL69;_LL88:;_LL89:(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192[0]=({
struct Cyc_Core_Failure_struct _tmp193;_tmp193.tag=Cyc_Core_Failure;_tmp193.f1=
_tag_arr("Unhandled case in copy_type_base",sizeof(char),33);_tmp193;});_tmp192;}));
_LL69:;}return env;}void*Cyc_Marshal_rcopy_type(struct _RegionHandle*r,void*ts,
void*val){struct _handler_cons _tmp194;_push_handler(& _tmp194);{int _tmp196=0;if(
setjmp(_tmp194.handler))_tmp196=1;if(!_tmp196){{struct _tagged_arr buf=({
unsigned int _tmp198=Cyc_Typerep_size_type(ts);char*_tmp199=(char*)_region_malloc(
r,_check_times(sizeof(char),_tmp198));struct _tagged_arr _tmp19B=_tag_arr(_tmp199,
sizeof(char),Cyc_Typerep_size_type(ts));{unsigned int _tmp19A=_tmp198;
unsigned int i;for(i=0;i < _tmp19A;i ++){_tmp199[i]='\000';}}_tmp19B;});void*result=((
void*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(buf,sizeof(char),1)));
Cyc_Marshal_rcopy_type_base(r,ts,Cyc_Marshal_empty_addr_map(),val,result);{void*
_tmp197=result;_npop_handler(0);return _tmp197;}};_pop_handler();}else{void*
_tmp195=(void*)_exn_thrown;void*_tmp19D=_tmp195;struct _tagged_arr _tmp19E;_LL8B:
if(*((void**)_tmp19D)!= Cyc_Core_Failure)goto _LL8D;_tmp19E=((struct Cyc_Core_Failure_struct*)
_tmp19D)->f1;_LL8C:({struct Cyc_Std_String_pa_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(
struct _tagged_arr)_tmp19E;{void*_tmp19F[1]={& _tmp1A0};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n",sizeof(char),19),_tag_arr(_tmp19F,sizeof(void*),1));}});(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp1A1=_cycalloc(sizeof(*
_tmp1A1));_tmp1A1[0]=({struct Cyc_Core_Failure_struct _tmp1A2;_tmp1A2.tag=Cyc_Core_Failure;
_tmp1A2.f1=_tmp19E;_tmp1A2;});_tmp1A1;}));_LL8D:;_LL8E:(void)_throw(_tmp19D);
_LL8A:;}}}void*Cyc_Marshal_copy_type(void*ts,void*val){return Cyc_Marshal_rcopy_type(
Cyc_Core_heap_region,ts,val);}void*Cyc_Marshal_order_lex(void*ord1,void*ord2){
if(ord1 != (void*)1)return ord1;else{return ord2;}}struct _tuple10{unsigned int f1;
unsigned int f2;};static int Cyc_Marshal_addrp_cmp(struct _tuple10*addrp1,struct
_tuple10*addrp2){if((*addrp1).f1 < (*addrp2).f1)return 1;else{if((*addrp1).f1 == (*
addrp2).f1)return Cyc_Marshal_uint_cmp((*addrp1).f2,(*addrp2).f2);else{return - 1;}}}
struct Cyc_Dict_Dict*Cyc_Marshal_empty_eq_dict(){return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple10*,struct _tuple10*)))Cyc_Dict_empty)(Cyc_Marshal_addrp_cmp),({
struct _tuple10*_tmp1A3=_cycalloc_atomic(sizeof(*_tmp1A3));_tmp1A3->f1=0;_tmp1A3->f2=
0;_tmp1A3;}),(void*)1);}struct _tuple11{void*f1;struct Cyc_Dict_Dict*f2;};struct
_tuple11 Cyc_Marshal_return_update(struct _tuple10*pair,struct _tuple11 ordenv){void*
_tmp1A5;struct Cyc_Dict_Dict*_tmp1A6;struct _tuple11 _tmp1A4=ordenv;_tmp1A5=_tmp1A4.f1;
_tmp1A6=_tmp1A4.f2;_tmp1A6=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple10*k,void*v))Cyc_Dict_insert)(_tmp1A6,pair,_tmp1A5);return({struct
_tuple11 _tmp1A7;_tmp1A7.f1=_tmp1A5;_tmp1A7.f2=_tmp1A6;_tmp1A7;});}struct _tuple11
Cyc_Marshal_leq_eq_type(void*rep,struct Cyc_Dict_Dict*env,void*x,void*y){if(x == y)
return({struct _tuple11 _tmp1A8;_tmp1A8.f1=(void*)1;_tmp1A8.f2=env;_tmp1A8;});{
void*_tmp1A9=rep;int _tmp1AA;unsigned int _tmp1AB;int _tmp1AC;unsigned int _tmp1AD;
int _tmp1AE;unsigned int _tmp1AF;int _tmp1B0;unsigned int _tmp1B1;int _tmp1B2;
unsigned int _tmp1B3;int _tmp1B4;unsigned int _tmp1B5;int _tmp1B6;unsigned int
_tmp1B7;int _tmp1B8;unsigned int _tmp1B9;unsigned int _tmp1BA;void*_tmp1BB;void*
_tmp1BC;struct _tagged_arr _tmp1BD;struct _tagged_arr _tmp1BE;struct _tagged_arr
_tmp1BF;struct _tagged_arr _tmp1C0;struct _tagged_arr _tmp1C1;struct _tagged_arr
_tmp1C2;int _tmp1C3;struct _tagged_arr _tmp1C4;int _tmp1C5;_LL90: if(_tmp1A9 <= (void*)
2?1:*((int*)_tmp1A9)!= 0)goto _LL92;_tmp1AA=((struct Cyc_Typerep_Int_struct*)
_tmp1A9)->f1;if(_tmp1AA != 0)goto _LL92;_tmp1AB=((struct Cyc_Typerep_Int_struct*)
_tmp1A9)->f2;if(_tmp1AB != 8)goto _LL92;_LL91: {char*x0=((char*(*)(void*x))
unsafe_cast)(x);char*y0=((char*(*)(void*x))unsafe_cast)(y);return({struct
_tuple11 _tmp1C6;_tmp1C6.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);
_tmp1C6.f2=env;_tmp1C6;});}_LL92: if(_tmp1A9 <= (void*)2?1:*((int*)_tmp1A9)!= 0)
goto _LL94;_tmp1AC=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f1;if(_tmp1AC != 1)
goto _LL94;_tmp1AD=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f2;if(_tmp1AD != 8)
goto _LL94;_LL93: {signed char*x0=((signed char*(*)(void*x))unsafe_cast)(x);
signed char*y0=((signed char*(*)(void*x))unsafe_cast)(y);return({struct _tuple11
_tmp1C7;_tmp1C7.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);_tmp1C7.f2=
env;_tmp1C7;});}_LL94: if(_tmp1A9 <= (void*)2?1:*((int*)_tmp1A9)!= 0)goto _LL96;
_tmp1AE=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f1;if(_tmp1AE != 1)goto _LL96;
_tmp1AF=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f2;if(_tmp1AF != 16)goto _LL96;
_LL95: {short*x0=((short*(*)(void*x))unsafe_cast)(x);short*y0=((short*(*)(void*x))
unsafe_cast)(y);return({struct _tuple11 _tmp1C8;_tmp1C8.f1=*x0 == *y0?(void*)1:(*x0
<= *y0?(void*)2:(void*)0);_tmp1C8.f2=env;_tmp1C8;});}_LL96: if(_tmp1A9 <= (void*)2?
1:*((int*)_tmp1A9)!= 0)goto _LL98;_tmp1B0=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f1;
if(_tmp1B0 != 0)goto _LL98;_tmp1B1=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f2;
if(_tmp1B1 != 16)goto _LL98;_LL97: {unsigned short*x0=((unsigned short*(*)(void*x))
unsafe_cast)(x);unsigned short*y0=((unsigned short*(*)(void*x))unsafe_cast)(y);
return({struct _tuple11 _tmp1C9;_tmp1C9.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(
void*)0);_tmp1C9.f2=env;_tmp1C9;});}_LL98: if(_tmp1A9 <= (void*)2?1:*((int*)
_tmp1A9)!= 0)goto _LL9A;_tmp1B2=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f1;if(
_tmp1B2 != 1)goto _LL9A;_tmp1B3=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f2;if(
_tmp1B3 != 32)goto _LL9A;_LL99: {int*x0=((int*(*)(void*x))unsafe_cast)(x);int*y0=((
int*(*)(void*x))unsafe_cast)(y);return({struct _tuple11 _tmp1CA;_tmp1CA.f1=*x0 == *
y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);_tmp1CA.f2=env;_tmp1CA;});}_LL9A: if(
_tmp1A9 <= (void*)2?1:*((int*)_tmp1A9)!= 0)goto _LL9C;_tmp1B4=((struct Cyc_Typerep_Int_struct*)
_tmp1A9)->f1;if(_tmp1B4 != 0)goto _LL9C;_tmp1B5=((struct Cyc_Typerep_Int_struct*)
_tmp1A9)->f2;if(_tmp1B5 != 32)goto _LL9C;_LL9B: {unsigned int*x0=((unsigned int*(*)(
void*x))unsafe_cast)(x);unsigned int*y0=((unsigned int*(*)(void*x))unsafe_cast)(
y);return({struct _tuple11 _tmp1CB;_tmp1CB.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)
2:(void*)0);_tmp1CB.f2=env;_tmp1CB;});}_LL9C: if(_tmp1A9 <= (void*)2?1:*((int*)
_tmp1A9)!= 0)goto _LL9E;_tmp1B6=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f1;if(
_tmp1B6 != 1)goto _LL9E;_tmp1B7=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f2;if(
_tmp1B7 != 64)goto _LL9E;_LL9D: {long long*x0=((long long*(*)(void*x))unsafe_cast)(
x);long long*y0=((long long*(*)(void*x))unsafe_cast)(y);return({struct _tuple11
_tmp1CC;_tmp1CC.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);_tmp1CC.f2=
env;_tmp1CC;});}_LL9E: if(_tmp1A9 <= (void*)2?1:*((int*)_tmp1A9)!= 0)goto _LLA0;
_tmp1B8=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f1;if(_tmp1B8 != 0)goto _LLA0;
_tmp1B9=((struct Cyc_Typerep_Int_struct*)_tmp1A9)->f2;if(_tmp1B9 != 64)goto _LLA0;
_LL9F: {unsigned long long*x0=((unsigned long long*(*)(void*x))unsafe_cast)(x);
unsigned long long*y0=((unsigned long long*(*)(void*x))unsafe_cast)(y);return({
struct _tuple11 _tmp1CD;_tmp1CD.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);
_tmp1CD.f2=env;_tmp1CD;});}_LLA0: if((int)_tmp1A9 != 0)goto _LLA2;_LLA1: {float*x0=((
float*(*)(void*x))unsafe_cast)(x);float*y0=((float*(*)(void*x))unsafe_cast)(y);
return({struct _tuple11 _tmp1CE;_tmp1CE.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(
void*)0);_tmp1CE.f2=env;_tmp1CE;});}_LLA2: if((int)_tmp1A9 != 1)goto _LLA4;_LLA3: {
double*x0=((double*(*)(void*x))unsafe_cast)(x);double*y0=((double*(*)(void*x))
unsafe_cast)(y);return({struct _tuple11 _tmp1CF;_tmp1CF.f1=*x0 == *y0?(void*)1:(*x0
<= *y0?(void*)2:(void*)0);_tmp1CF.f2=env;_tmp1CF;});}_LLA4: if(_tmp1A9 <= (void*)2?
1:*((int*)_tmp1A9)!= 1)goto _LLA6;_tmp1BA=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1A9)->f1;_tmp1BB=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1A9)->f2;
_LLA5: {unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*
y0=((unsigned int*(*)(void*x))unsafe_cast)(y);unsigned int addrx=*x0;unsigned int
addry=*y0;struct _tuple10*_tmp1D0=({struct _tuple10*_tmp1D8=_cycalloc_atomic(
sizeof(*_tmp1D8));_tmp1D8->f1=addrx;_tmp1D8->f2=addry;_tmp1D8;});if(((int(*)(
struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp1D0))return({
struct _tuple11 _tmp1D1;_tmp1D1.f1=((void*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*
k))Cyc_Dict_lookup)(env,_tmp1D0);_tmp1D1.f2=env;_tmp1D1;});else{if(addrx == 0)
return({struct _tuple11 _tmp1D2;_tmp1D2.f1=(void*)2;_tmp1D2.f2=env;_tmp1D2;});
else{if(addry == 0)return({struct _tuple11 _tmp1D3;_tmp1D3.f1=(void*)0;_tmp1D3.f2=
env;_tmp1D3;});else{env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp1D0,(void*)1);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp1BB);int xa=(int)addrx;int ya=(int)addry;void*ord=(void*)1;{int i=0;for(0;i < 
_tmp1BA;i ++){void*_tmp1D5;struct Cyc_Dict_Dict*_tmp1D6;struct _tuple11 _tmp1D4=Cyc_Marshal_leq_eq_type(
_tmp1BB,env,((void*(*)(int x))unsafe_cast)(xa),((void*(*)(int x))unsafe_cast)(ya));
_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;ord=Cyc_Marshal_order_lex(ord,_tmp1D5);env=
_tmp1D6;xa +=typ_szb;ya +=typ_szb;}}return Cyc_Marshal_return_update(_tmp1D0,({
struct _tuple11 _tmp1D7;_tmp1D7.f1=ord;_tmp1D7.f2=env;_tmp1D7;}));}}}}}_LLA6: if(
_tmp1A9 <= (void*)2?1:*((int*)_tmp1A9)!= 2)goto _LLA8;_tmp1BC=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp1A9)->f1;_LLA7: {struct _tuple9*x0=((struct _tuple9*(*)(void*x))unsafe_cast)(x);
struct _tuple9*y0=((struct _tuple9*(*)(void*x))unsafe_cast)(y);unsigned int _tmp1DA;
unsigned int _tmp1DB;unsigned int _tmp1DC;struct _tuple9 _tmp1D9=*x0;_tmp1DA=_tmp1D9.f1;
_tmp1DB=_tmp1D9.f2;_tmp1DC=_tmp1D9.f3;{unsigned int _tmp1DE;unsigned int _tmp1DF;
unsigned int _tmp1E0;struct _tuple9 _tmp1DD=*y0;_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;
_tmp1E0=_tmp1DD.f3;{int typ_szb=(int)Cyc_Typerep_size_type(_tmp1BC);int xpos=(int)((
_tmp1DB - _tmp1DA)/ typ_szb);int ypos=(int)((_tmp1DF - _tmp1DE)/ typ_szb);if(xpos > 
ypos)return({struct _tuple11 _tmp1E1;_tmp1E1.f1=(void*)0;_tmp1E1.f2=env;_tmp1E1;});{
int xsize=(int)((_tmp1DC - _tmp1DA)/ typ_szb);int ysize=(int)((_tmp1E0 - _tmp1DE)/ 
typ_szb);if(xsize > ysize)return({struct _tuple11 _tmp1E2;_tmp1E2.f1=(void*)0;
_tmp1E2.f2=env;_tmp1E2;});{struct _tuple10*_tmp1E3=({struct _tuple10*_tmp1EB=
_cycalloc_atomic(sizeof(*_tmp1EB));_tmp1EB->f1=_tmp1DA;_tmp1EB->f2=_tmp1DE;
_tmp1EB;});if(((int(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp1E3))return({struct _tuple11 _tmp1E4;_tmp1E4.f1=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp1E3);_tmp1E4.f2=env;_tmp1E4;});
else{if(_tmp1DA == 0)return({struct _tuple11 _tmp1E5;_tmp1E5.f1=(void*)2;_tmp1E5.f2=
env;_tmp1E5;});else{if(_tmp1DE == 0)return({struct _tuple11 _tmp1E6;_tmp1E6.f1=(
void*)0;_tmp1E6.f2=env;_tmp1E6;});else{env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp1E3,(void*)1);{int xb=(int)
_tmp1DA;int yb=(int)_tmp1DE;void*ord=(void*)1;{int i=0;for(0;i < xsize;i ++){void*
_tmp1E8;struct Cyc_Dict_Dict*_tmp1E9;struct _tuple11 _tmp1E7=Cyc_Marshal_leq_eq_type(
_tmp1BC,env,((void*(*)(int x))unsafe_cast)(xb),((void*(*)(int x))unsafe_cast)(yb));
_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;ord=Cyc_Marshal_order_lex(ord,_tmp1E8);env=
_tmp1E9;xb +=typ_szb;yb +=typ_szb;}}if(ord == (void*)1?xsize < ysize: 0)ord=(void*)2;
return Cyc_Marshal_return_update(_tmp1E3,({struct _tuple11 _tmp1EA;_tmp1EA.f1=ord;
_tmp1EA.f2=env;_tmp1EA;}));}}}}}}}}}_LLA8: if(_tmp1A9 <= (void*)2?1:*((int*)
_tmp1A9)!= 4)goto _LLAA;_tmp1BD=((struct Cyc_Typerep_Tuple_struct*)_tmp1A9)->f2;
_LLA9: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*x))
unsafe_cast)(y);void*ord=(void*)1;{int i=0;for(0;i < _get_arr_size(_tmp1BD,sizeof(
struct _tuple1*));i ++){unsigned int _tmp1ED;void*_tmp1EE;struct _tuple1 _tmp1EC=*((
struct _tuple1**)_tmp1BD.curr)[i];_tmp1ED=_tmp1EC.f1;_tmp1EE=_tmp1EC.f2;{int x_ofs=(
int)(x0 + _tmp1ED);int y_ofs=(int)(y0 + _tmp1ED);void*_tmp1F0;struct Cyc_Dict_Dict*
_tmp1F1;struct _tuple11 _tmp1EF=Cyc_Marshal_leq_eq_type(_tmp1EE,env,((void*(*)(int
x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp1F0=_tmp1EF.f1;
_tmp1F1=_tmp1EF.f2;ord=Cyc_Marshal_order_lex(ord,_tmp1F0);env=_tmp1F1;}}}return({
struct _tuple11 _tmp1F2;_tmp1F2.f1=ord;_tmp1F2.f2=env;_tmp1F2;});}_LLAA: if(_tmp1A9
<= (void*)2?1:*((int*)_tmp1A9)!= 3)goto _LLAC;_tmp1BE=((struct Cyc_Typerep_Struct_struct*)
_tmp1A9)->f3;_LLAB: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=0;for(0;i < _get_arr_size(_tmp1BE,
sizeof(struct _tuple0*));i ++){unsigned int _tmp1F4;void*_tmp1F5;struct _tuple0
_tmp1F3=*((struct _tuple0**)_tmp1BE.curr)[i];_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f3;{
int x_ofs=(int)(x0 + _tmp1F4);int y_ofs=(int)(y0 + _tmp1F4);void*_tmp1F7;struct Cyc_Dict_Dict*
_tmp1F8;struct _tuple11 _tmp1F6=Cyc_Marshal_leq_eq_type(_tmp1F5,env,((void*(*)(int
x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp1F7=_tmp1F6.f1;
_tmp1F8=_tmp1F6.f2;ord=Cyc_Marshal_order_lex(ord,_tmp1F7);env=_tmp1F8;}}}return({
struct _tuple11 _tmp1F9;_tmp1F9.f1=ord;_tmp1F9.f2=env;_tmp1F9;});}_LLAC: if(_tmp1A9
<= (void*)2?1:*((int*)_tmp1A9)!= 6)goto _LLAE;_tmp1BF=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1A9)->f4;_LLAD: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=1;for(0;i < _get_arr_size(_tmp1BF,
sizeof(struct _tuple1*));i ++){unsigned int _tmp1FB;void*_tmp1FC;struct _tuple1
_tmp1FA=*((struct _tuple1**)_tmp1BF.curr)[i];_tmp1FB=_tmp1FA.f1;_tmp1FC=_tmp1FA.f2;{
int x_ofs=(int)(x0 + _tmp1FB);int y_ofs=(int)(y0 + _tmp1FB);void*_tmp1FE;struct Cyc_Dict_Dict*
_tmp1FF;struct _tuple11 _tmp1FD=Cyc_Marshal_leq_eq_type(_tmp1FC,env,((void*(*)(int
x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp1FE=_tmp1FD.f1;
_tmp1FF=_tmp1FD.f2;ord=Cyc_Marshal_order_lex(ord,_tmp1FE);env=_tmp1FF;}}}return({
struct _tuple11 _tmp200;_tmp200.f1=ord;_tmp200.f2=env;_tmp200;});}_LLAE: if(_tmp1A9
<= (void*)2?1:*((int*)_tmp1A9)!= 5)goto _LLB0;_tmp1C0=((struct Cyc_Typerep_TUnion_struct*)
_tmp1A9)->f2;_tmp1C1=((struct Cyc_Typerep_TUnion_struct*)_tmp1A9)->f3;_LLAF: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);if(*x0 < 1024?*y0 < 1024: 0)return({struct
_tuple11 _tmp201;_tmp201.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0);
_tmp201.f2=env;_tmp201;});else{if(*x0 < 1024)return({struct _tuple11 _tmp202;
_tmp202.f1=(void*)2;_tmp202.f2=env;_tmp202;});else{if(*y0 < 1024)return({struct
_tuple11 _tmp203;_tmp203.f1=(void*)0;_tmp203.f2=env;_tmp203;});else{struct
_tuple10*_tmp204=({struct _tuple10*_tmp20A=_cycalloc_atomic(sizeof(*_tmp20A));
_tmp20A->f1=*x0;_tmp20A->f2=*y0;_tmp20A;});if(((int(*)(struct Cyc_Dict_Dict*d,
struct _tuple10*k))Cyc_Dict_member)(env,_tmp204))return({struct _tuple11 _tmp205;
_tmp205.f1=((void*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_lookup)(
env,_tmp204);_tmp205.f2=env;_tmp205;});else{env=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp204,(void*)1);{
unsigned int*xtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*x0);
unsigned int*ytagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*y0);if(*xtagp
!= *ytagp)return Cyc_Marshal_return_update(_tmp204,({struct _tuple11 _tmp206;
_tmp206.f1=*xtagp == *ytagp?(void*)1:(*xtagp <= *ytagp?(void*)2:(void*)0);_tmp206.f2=
env;_tmp206;}));else{int _tmp208;void*_tmp209;struct _tuple4 _tmp207=Cyc_Typerep_get_unionbranch(*
xtagp,_tmp1C1);_tmp208=_tmp207.f1;_tmp209=_tmp207.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*x0 + _tmp208);void*yoff=((void*(*)(unsigned int x))
unsafe_cast)(*y0 + _tmp208);return Cyc_Marshal_return_update(_tmp204,Cyc_Marshal_leq_eq_type(
_tmp209,env,xoff,yoff));}}}}}}}}_LLB0: if(_tmp1A9 <= (void*)2?1:*((int*)_tmp1A9)!= 
7)goto _LLB2;_tmp1C2=((struct Cyc_Typerep_XTUnion_struct*)_tmp1A9)->f2;_LLB1: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);struct _tuple10*_tmp20B=({struct _tuple10*
_tmp218=_cycalloc_atomic(sizeof(*_tmp218));_tmp218->f1=*x0;_tmp218->f2=*y0;
_tmp218;});if(((int(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp20B))return({struct _tuple11 _tmp20C;_tmp20C.f1=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp20B);_tmp20C.f2=env;_tmp20C;});
else{env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))
Cyc_Dict_insert)(env,({struct _tuple10*_tmp20D=_cycalloc_atomic(sizeof(*_tmp20D));
_tmp20D->f1=*x0;_tmp20D->f2=*y0;_tmp20D;}),(void*)1);{unsigned int*xp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*x0);unsigned int*yp=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*y0);if(*xp == 0?*yp == 0: 0){char*xnamec=((char*(*)(
unsigned int x))unsafe_cast)(*x0 + 4);char*ynamec=((char*(*)(unsigned int x))
unsafe_cast)(*y0 + 4);struct _tagged_arr xname=(struct _tagged_arr)Cstring_to_string(
xnamec);struct _tagged_arr yname=(struct _tagged_arr)Cstring_to_string(ynamec);int
cmp=Cyc_Std_strcmp(xname,yname);if(cmp < 0)return Cyc_Marshal_return_update(
_tmp20B,({struct _tuple11 _tmp20E;_tmp20E.f1=(void*)0;_tmp20E.f2=env;_tmp20E;}));
else{if(cmp == 0)return Cyc_Marshal_return_update(_tmp20B,({struct _tuple11 _tmp20F;
_tmp20F.f1=(void*)1;_tmp20F.f2=env;_tmp20F;}));else{return Cyc_Marshal_return_update(
_tmp20B,({struct _tuple11 _tmp210;_tmp210.f1=(void*)2;_tmp210.f2=env;_tmp210;}));}}}
else{if(*xp == 0)return Cyc_Marshal_return_update(_tmp20B,({struct _tuple11 _tmp211;
_tmp211.f1=(void*)2;_tmp211.f2=env;_tmp211;}));else{if(*yp == 0)return Cyc_Marshal_return_update(
_tmp20B,({struct _tuple11 _tmp212;_tmp212.f1=(void*)0;_tmp212.f2=env;_tmp212;}));
else{char*xnamec=((char*(*)(unsigned int x))unsafe_cast)(*xp + 4);char*ynamec=((
char*(*)(unsigned int x))unsafe_cast)(*yp + 4);struct _tagged_arr xname=(struct
_tagged_arr)Cstring_to_string(xnamec);struct _tagged_arr yname=(struct _tagged_arr)
Cstring_to_string(ynamec);int cmp=Cyc_Std_strcmp(xname,yname);if(cmp < 0)return Cyc_Marshal_return_update(
_tmp20B,({struct _tuple11 _tmp213;_tmp213.f1=(void*)0;_tmp213.f2=env;_tmp213;}));
else{if(cmp == 0){int _tmp215;void*_tmp216;struct _tuple4 _tmp214=Cyc_Typerep_get_xtunionbranch(
xname,_tmp1C2);_tmp215=_tmp214.f1;_tmp216=_tmp214.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*x0 + _tmp215);void*yoff=((void*(*)(unsigned int x))
unsafe_cast)(*y0 + _tmp215);return Cyc_Marshal_return_update(_tmp20B,Cyc_Marshal_leq_eq_type(
_tmp216,env,xoff,yoff));}}else{return Cyc_Marshal_return_update(_tmp20B,({struct
_tuple11 _tmp217;_tmp217.f1=(void*)2;_tmp217.f2=env;_tmp217;}));}}}}}}}}_LLB2: if(
_tmp1A9 <= (void*)2?1:*((int*)_tmp1A9)!= 8)goto _LLB4;_tmp1C3=((struct Cyc_Typerep_Union_struct*)
_tmp1A9)->f2;_tmp1C4=((struct Cyc_Typerep_Union_struct*)_tmp1A9)->f3;_LLB3: return
Cyc_Marshal_leq_eq_type((void*)({struct Cyc_Typerep_Tuple_struct*_tmp219=
_cycalloc(sizeof(*_tmp219));_tmp219[0]=({struct Cyc_Typerep_Tuple_struct _tmp21A;
_tmp21A.tag=4;_tmp21A.f1=(unsigned int)_tmp1C3;_tmp21A.f2=({unsigned int _tmp21B=(
unsigned int)_tmp1C3;struct _tuple1**_tmp21C=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp21B));struct _tagged_arr _tmp221=_tag_arr(
_tmp21C,sizeof(struct _tuple1*),(unsigned int)_tmp1C3);{unsigned int _tmp21D=
_tmp21B;unsigned int i;for(i=0;i < _tmp21D;i ++){_tmp21C[i]=({struct _tuple1*_tmp21E=
_cycalloc(sizeof(*_tmp21E));_tmp21E->f1=i;_tmp21E->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp21F=_cycalloc_atomic(sizeof(*_tmp21F));_tmp21F[0]=({struct Cyc_Typerep_Int_struct
_tmp220;_tmp220.tag=0;_tmp220.f1=0;_tmp220.f2=8;_tmp220;});_tmp21F;});_tmp21E;});}}
_tmp221;});_tmp21A;});_tmp219;}),env,x,y);_LLB4: if(_tmp1A9 <= (void*)2?1:*((int*)
_tmp1A9)!= 9)goto _LLB6;_tmp1C5=((struct Cyc_Typerep_Enum_struct*)_tmp1A9)->f2;
_LLB5: return Cyc_Marshal_leq_eq_type((void*)({struct Cyc_Typerep_Int_struct*
_tmp222=_cycalloc_atomic(sizeof(*_tmp222));_tmp222[0]=({struct Cyc_Typerep_Int_struct
_tmp223;_tmp223.tag=0;_tmp223.f1=0;_tmp223.f2=(unsigned int)(_tmp1C5 << 3);
_tmp223;});_tmp222;}),env,x,y);_LLB6:;_LLB7:(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224[0]=({struct Cyc_Core_Failure_struct
_tmp225;_tmp225.tag=Cyc_Core_Failure;_tmp225.f1=_tag_arr("leq_eq_type case unmatched",
sizeof(char),27);_tmp225;});_tmp224;}));_LL8F:;}}int Cyc_Marshal_eq_type(void*ts,
void*x,void*y){struct _handler_cons _tmp226;_push_handler(& _tmp226);{int _tmp228=0;
if(setjmp(_tmp226.handler))_tmp228=1;if(!_tmp228){{void*_tmp22A;struct _tuple11
_tmp229=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),x,y);_tmp22A=
_tmp229.f1;{int _tmp22B=_tmp22A == (void*)1;_npop_handler(0);return _tmp22B;}};
_pop_handler();}else{void*_tmp227=(void*)_exn_thrown;void*_tmp22D=_tmp227;struct
_tagged_arr _tmp22E;_LLB9: if(*((void**)_tmp22D)!= Cyc_Core_Failure)goto _LLBB;
_tmp22E=((struct Cyc_Core_Failure_struct*)_tmp22D)->f1;_LLBA:({struct Cyc_Std_String_pa_struct
_tmp230;_tmp230.tag=0;_tmp230.f1=(struct _tagged_arr)_tmp22E;{void*_tmp22F[1]={&
_tmp230};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(
char),19),_tag_arr(_tmp22F,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Core_Failure_struct
_tmp232;_tmp232.tag=Cyc_Core_Failure;_tmp232.f1=_tmp22E;_tmp232;});_tmp231;}));
_LLBB:;_LLBC:(void)_throw(_tmp22D);_LLB8:;}}}int Cyc_Marshal_leq_type(void*ts,
void*x,void*y){struct _handler_cons _tmp233;_push_handler(& _tmp233);{int _tmp235=0;
if(setjmp(_tmp233.handler))_tmp235=1;if(!_tmp235){{void*_tmp237;struct _tuple11
_tmp236=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),x,y);_tmp237=
_tmp236.f1;{int _tmp238=_tmp237 != (void*)0;_npop_handler(0);return _tmp238;}};
_pop_handler();}else{void*_tmp234=(void*)_exn_thrown;void*_tmp23A=_tmp234;struct
_tagged_arr _tmp23B;_LLBE: if(*((void**)_tmp23A)!= Cyc_Core_Failure)goto _LLC0;
_tmp23B=((struct Cyc_Core_Failure_struct*)_tmp23A)->f1;_LLBF:({struct Cyc_Std_String_pa_struct
_tmp23D;_tmp23D.tag=0;_tmp23D.f1=(struct _tagged_arr)_tmp23B;{void*_tmp23C[1]={&
_tmp23D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(
char),19),_tag_arr(_tmp23C,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Core_Failure_struct
_tmp23F;_tmp23F.tag=Cyc_Core_Failure;_tmp23F.f1=_tmp23B;_tmp23F;});_tmp23E;}));
_LLC0:;_LLC1:(void)_throw(_tmp23A);_LLBD:;}}}int Cyc_Marshal_cmp_type(void*ts,
void*x,void*y){struct _handler_cons _tmp240;_push_handler(& _tmp240);{int _tmp242=0;
if(setjmp(_tmp240.handler))_tmp242=1;if(!_tmp242){{void*_tmp244;struct _tuple11
_tmp243=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),x,y);_tmp244=
_tmp243.f1;{void*_tmp245=_tmp244;_LLC3: if((int)_tmp245 != 0)goto _LLC5;_LLC4: {int
_tmp246=- 1;_npop_handler(0);return _tmp246;}_LLC5: if((int)_tmp245 != 1)goto _LLC7;
_LLC6: {int _tmp247=0;_npop_handler(0);return _tmp247;}_LLC7: if((int)_tmp245 != 2)
goto _LLC2;_LLC8: {int _tmp248=1;_npop_handler(0);return _tmp248;}_LLC2:;}};
_pop_handler();}else{void*_tmp241=(void*)_exn_thrown;void*_tmp24A=_tmp241;struct
_tagged_arr _tmp24B;_LLCA: if(*((void**)_tmp24A)!= Cyc_Core_Failure)goto _LLCC;
_tmp24B=((struct Cyc_Core_Failure_struct*)_tmp24A)->f1;_LLCB:({struct Cyc_Std_String_pa_struct
_tmp24D;_tmp24D.tag=0;_tmp24D.f1=(struct _tagged_arr)_tmp24B;{void*_tmp24C[1]={&
_tmp24D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Core::Failure(%s)\n",sizeof(
char),19),_tag_arr(_tmp24C,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E[0]=({struct Cyc_Core_Failure_struct
_tmp24F;_tmp24F.tag=Cyc_Core_Failure;_tmp24F.f1=_tmp24B;_tmp24F;});_tmp24E;}));
_LLCC:;_LLCD:(void)_throw(_tmp24A);_LLC9:;}}}
