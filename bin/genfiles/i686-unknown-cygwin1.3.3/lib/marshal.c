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

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct _tagged_arr _tagged_ptr_decrease_size(struct _tagged_arr x,
                                                    unsigned int sz,
                                                    unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

// Add i to zero-terminated pointer x.  Checks for x being null and
// ensures that x[0..i-1] are not 0.
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

// Does in-place addition of a zero-terminated pointer (x += e and ++x).  
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

// Does in-place increment of a zero-terminated pointer (e.g., x++).
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  
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
_tagged_arr l);unsigned int Cyc_Typerep_size_type(void*rep);struct Cyc_Std___cycFILE;
extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*
f1;};int Cyc_Std_fgetc(struct Cyc_Std___cycFILE*);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fputc(int,struct Cyc_Std___cycFILE*);
unsigned int Cyc_Std_fread(struct _tagged_arr,unsigned int,unsigned int,struct Cyc_Std___cycFILE*);
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Std_CharPtr_sa_struct{
int tag;struct _tagged_arr f1;};extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};int Cyc_Std_strcmp(
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
unsigned int),_tmp15);{unsigned int _tmp17=_tmp15;unsigned int j;for(j=0;j < _tmp17;
j ++){_tmp16[j]=j < _tmp14?*((unsigned int*)_check_unknown_subscript(_tmp13,
sizeof(unsigned int),(int)j)): 0;}}_tmp18;});*((unsigned int*)
_check_unknown_subscript(_tmp13,sizeof(unsigned int),_tmp14))=i;return({struct
_tuple7 _tmp19;_tmp19.f1=_tmp13;_tmp19.f2=_tmp14 + 1;_tmp19;});}static unsigned int
Cyc_Marshal_table_lookup(struct _tuple7 tbl,int i){return*((unsigned int*)
_check_unknown_subscript(tbl.f1,sizeof(unsigned int),i));}static void Cyc_Marshal_fprint_ref(
struct Cyc_Std___cycFILE*fp,unsigned int ref){if(ref == 0)({void*_tmp1A[0]={};Cyc_Std_fprintf(
fp,({const char*_tmp1B="NULL";_tag_arr(_tmp1B,sizeof(char),_get_zero_arr_size(
_tmp1B,5));}),_tag_arr(_tmp1A,sizeof(void*),0));});else{({struct Cyc_Std_Int_pa_struct
_tmp1E;_tmp1E.tag=1;_tmp1E.f1=ref;{void*_tmp1C[1]={& _tmp1E};Cyc_Std_fprintf(fp,({
const char*_tmp1D="(#%x)";_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,
6));}),_tag_arr(_tmp1C,sizeof(void*),1));}});}}struct _tuple9{unsigned int f1;
unsigned int f2;unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_fprint_type_base(
void*rep,struct Cyc_Set_Set*env,struct Cyc_Std___cycFILE*fp,void*val){{void*_tmp1F=
rep;int _tmp20;unsigned int _tmp21;unsigned int _tmp22;void*_tmp23;void*_tmp24;
struct _tagged_arr*_tmp25;struct _tagged_arr _tmp26;struct _tagged_arr _tmp27;struct
_tagged_arr _tmp28;struct _tagged_arr _tmp29;struct _tagged_arr _tmp2A;struct
_tagged_arr _tmp2B;struct _tagged_arr _tmp2C;struct _tagged_arr _tmp2D;struct
_tagged_arr _tmp2E;struct _tagged_arr _tmp2F;struct _tagged_arr*_tmp30;struct
_tagged_arr _tmp31;int _tmp32;struct _tagged_arr _tmp33;_LL1: if(_tmp1F <= (void*)2?1:*((
int*)_tmp1F)!= 0)goto _LL3;_tmp20=((struct Cyc_Typerep_Int_struct*)_tmp1F)->f1;
_tmp21=((struct Cyc_Typerep_Int_struct*)_tmp1F)->f2;_LL2: switch(_tmp21){case 8:
_LL19: {char*x=((char*(*)(void*x))unsafe_cast)(val);if(isprint((int)*((char*)x)))({
struct Cyc_Std_Int_pa_struct _tmp36;_tmp36.tag=1;_tmp36.f1=(int)((unsigned int)((
int)*((char*)x)));{void*_tmp34[1]={& _tmp36};Cyc_Std_fprintf(fp,({const char*
_tmp35="'%c'";_tag_arr(_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,5));}),
_tag_arr(_tmp34,sizeof(void*),1));}});else{({struct Cyc_Std_Int_pa_struct _tmp39;
_tmp39.tag=1;_tmp39.f1=(int)((unsigned int)((int)*((char*)x)));{void*_tmp37[1]={&
_tmp39};Cyc_Std_fprintf(fp,({const char*_tmp38="'\\%d'";_tag_arr(_tmp38,sizeof(
char),_get_zero_arr_size(_tmp38,6));}),_tag_arr(_tmp37,sizeof(void*),1));}});}
break;}case 16: _LL1A: {short*x=((short*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Int_pa_struct
_tmp3D;_tmp3D.tag=1;_tmp3D.f1=(unsigned int)((int)*((short*)x));{void*_tmp3A[1]={&
_tmp3D};Cyc_Std_fprintf(fp,_tmp20?({const char*_tmp3B="%d";_tag_arr(_tmp3B,
sizeof(char),_get_zero_arr_size(_tmp3B,3));}):({const char*_tmp3C="%u";_tag_arr(
_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,3));}),_tag_arr(_tmp3A,sizeof(void*),
1));}});break;}case 32: _LL1B: {int*x=((int*(*)(void*x))unsafe_cast)(val);({struct
Cyc_Std_Int_pa_struct _tmp41;_tmp41.tag=1;_tmp41.f1=(unsigned int)*((int*)x);{
void*_tmp3E[1]={& _tmp41};Cyc_Std_fprintf(fp,_tmp20?({const char*_tmp3F="%d";
_tag_arr(_tmp3F,sizeof(char),_get_zero_arr_size(_tmp3F,3));}):({const char*_tmp40="%u";
_tag_arr(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,3));}),_tag_arr(_tmp3E,
sizeof(void*),1));}});break;}default: _LL1C:({struct Cyc_Std_Int_pa_struct _tmp44;
_tmp44.tag=1;_tmp44.f1=(int)_tmp21;{void*_tmp42[1]={& _tmp44};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp43="fprint_type_base: illegal int size %d bits\n";_tag_arr(_tmp43,
sizeof(char),_get_zero_arr_size(_tmp43,44));}),_tag_arr(_tmp42,sizeof(void*),1));}});
exit(1);}goto _LL0;_LL3: if((int)_tmp1F != 0)goto _LL5;_LL4: {float*x=((float*(*)(
void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct _tmp47;_tmp47.tag=2;
_tmp47.f1=(double)*((float*)x);{void*_tmp45[1]={& _tmp47};Cyc_Std_fprintf(fp,({
const char*_tmp46="%f";_tag_arr(_tmp46,sizeof(char),_get_zero_arr_size(_tmp46,3));}),
_tag_arr(_tmp45,sizeof(void*),1));}});goto _LL0;}_LL5: if((int)_tmp1F != 1)goto _LL7;
_LL6: {double*x=((double*(*)(void*x))unsafe_cast)(val);({struct Cyc_Std_Double_pa_struct
_tmp4A;_tmp4A.tag=2;_tmp4A.f1=*((double*)x);{void*_tmp48[1]={& _tmp4A};Cyc_Std_fprintf(
fp,({const char*_tmp49="%f";_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(
_tmp49,3));}),_tag_arr(_tmp48,sizeof(void*),1));}});goto _LL0;}_LL7: if(_tmp1F <= (
void*)2?1:*((int*)_tmp1F)!= 1)goto _LL9;_tmp22=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1F)->f1;_tmp23=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1F)->f2;_LL8: {
int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));struct
_tuple8 _tmp4C;int _tmp4D;struct Cyc_Set_Set*_tmp4E;struct _tuple8*_tmp4B=Cyc_Marshal_member_insert(
env,(unsigned int)y);_tmp4C=*_tmp4B;_tmp4D=_tmp4C.f1;_tmp4E=_tmp4C.f2;env=_tmp4E;
if(!_tmp4D){({struct Cyc_Std_Int_pa_struct _tmp51;_tmp51.tag=1;_tmp51.f1=(
unsigned int)y;{void*_tmp4F[1]={& _tmp51};Cyc_Std_fprintf(fp,({const char*_tmp50="{(@%x) ";
_tag_arr(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,8));}),_tag_arr(_tmp4F,
sizeof(void*),1));}});if(y != 0){int typ_szb=(int)Cyc_Typerep_size_type(_tmp23);
int i=0;for(0;i < _tmp22;i ++){env=Cyc_Marshal_fprint_type_base(_tmp23,env,fp,((
void*(*)(int x))unsafe_cast)(y));if(i != _tmp22 - 1)({void*_tmp52[0]={};Cyc_Std_fprintf(
fp,({const char*_tmp53=", ";_tag_arr(_tmp53,sizeof(char),_get_zero_arr_size(
_tmp53,3));}),_tag_arr(_tmp52,sizeof(void*),0));});y +=typ_szb;}}else{({void*
_tmp54[0]={};Cyc_Std_fprintf(fp,({const char*_tmp55="NULL";_tag_arr(_tmp55,
sizeof(char),_get_zero_arr_size(_tmp55,5));}),_tag_arr(_tmp54,sizeof(void*),0));});}({
void*_tmp56[0]={};Cyc_Std_fprintf(fp,({const char*_tmp57=" }";_tag_arr(_tmp57,
sizeof(char),_get_zero_arr_size(_tmp57,3));}),_tag_arr(_tmp56,sizeof(void*),0));});}
else{Cyc_Marshal_fprint_ref(fp,(unsigned int)y);}goto _LL0;}_LL9: if(_tmp1F <= (
void*)2?1:*((int*)_tmp1F)!= 2)goto _LLB;_tmp24=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp1F)->f1;_LLA: {struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);
unsigned int typ_szb=Cyc_Typerep_size_type(_tmp24);unsigned int base=(*((struct
_tuple9*)x)).f1;unsigned int curr=(*((struct _tuple9*)x)).f2;unsigned int
last_plus_one=(*((struct _tuple9*)x)).f3;struct _tuple8 _tmp59;int _tmp5A;struct Cyc_Set_Set*
_tmp5B;struct _tuple8*_tmp58=Cyc_Marshal_member_insert(env,base);_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;env=_tmp5B;if(!_tmp5A){({struct Cyc_Std_Int_pa_struct
_tmp5E;_tmp5E.tag=1;_tmp5E.f1=base;{void*_tmp5C[1]={& _tmp5E};Cyc_Std_fprintf(fp,({
const char*_tmp5D="[(@%x) ";_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(
_tmp5D,8));}),_tag_arr(_tmp5C,sizeof(void*),1));}});{unsigned int i=base;for(0;i < 
last_plus_one;i +=typ_szb){env=Cyc_Marshal_fprint_type_base(_tmp24,env,fp,((void*(*)(
unsigned int x))unsafe_cast)(i));if(i + typ_szb < last_plus_one)({void*_tmp5F[0]={};
Cyc_Std_fprintf(fp,({const char*_tmp60=", ";_tag_arr(_tmp60,sizeof(char),
_get_zero_arr_size(_tmp60,3));}),_tag_arr(_tmp5F,sizeof(void*),0));});}}({void*
_tmp61[0]={};Cyc_Std_fprintf(fp,({const char*_tmp62=" ]";_tag_arr(_tmp62,sizeof(
char),_get_zero_arr_size(_tmp62,3));}),_tag_arr(_tmp61,sizeof(void*),0));});}
else{Cyc_Marshal_fprint_ref(fp,base);}goto _LL0;}_LLB: if(_tmp1F <= (void*)2?1:*((
int*)_tmp1F)!= 3)goto _LLD;_tmp25=((struct Cyc_Typerep_Struct_struct*)_tmp1F)->f1;
_tmp26=((struct Cyc_Typerep_Struct_struct*)_tmp1F)->f3;_LLC: if(_tmp25 == 0)({void*
_tmp63[0]={};Cyc_Std_fprintf(fp,({const char*_tmp64="{ ";_tag_arr(_tmp64,sizeof(
char),_get_zero_arr_size(_tmp64,3));}),_tag_arr(_tmp63,sizeof(void*),0));});
else{({struct Cyc_Std_String_pa_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct
_tagged_arr)*((struct _tagged_arr*)_tmp25);{void*_tmp65[1]={& _tmp67};Cyc_Std_fprintf(
fp,({const char*_tmp66="%s{ ";_tag_arr(_tmp66,sizeof(char),_get_zero_arr_size(
_tmp66,5));}),_tag_arr(_tmp65,sizeof(void*),1));}});}{int x=((int(*)(void*x))
unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmp26,sizeof(struct _tuple0*));
i ++){unsigned int _tmp69;struct _tagged_arr _tmp6A;void*_tmp6B;struct _tuple0 _tmp68=*((
struct _tuple0*)((struct _tuple0**)_tmp26.curr)[i]);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
_tmp6B=_tmp68.f3;({struct Cyc_Std_String_pa_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(
struct _tagged_arr)_tmp6A;{void*_tmp6C[1]={& _tmp6E};Cyc_Std_fprintf(fp,({const
char*_tmp6D="%s=";_tag_arr(_tmp6D,sizeof(char),_get_zero_arr_size(_tmp6D,4));}),
_tag_arr(_tmp6C,sizeof(void*),1));}});{int x_ofs=(int)(x + _tmp69);env=Cyc_Marshal_fprint_type_base(
_tmp6B,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp26,
sizeof(struct _tuple0*))- 1)({void*_tmp6F[0]={};Cyc_Std_fprintf(fp,({const char*
_tmp70=", ";_tag_arr(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,3));}),
_tag_arr(_tmp6F,sizeof(void*),0));});}}}({void*_tmp71[0]={};Cyc_Std_fprintf(fp,({
const char*_tmp72=" }";_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,3));}),
_tag_arr(_tmp71,sizeof(void*),0));});goto _LL0;}_LLD: if(_tmp1F <= (void*)2?1:*((
int*)_tmp1F)!= 4)goto _LLF;_tmp27=((struct Cyc_Typerep_Tuple_struct*)_tmp1F)->f2;
_LLE:({void*_tmp73[0]={};Cyc_Std_fprintf(fp,({const char*_tmp74="$( ";_tag_arr(
_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,4));}),_tag_arr(_tmp73,sizeof(void*),
0));});{int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(
_tmp27,sizeof(struct _tuple1*));i ++){unsigned int _tmp76;void*_tmp77;struct _tuple1
_tmp75=*((struct _tuple1*)((struct _tuple1**)_tmp27.curr)[i]);_tmp76=_tmp75.f1;
_tmp77=_tmp75.f2;{int x_ofs=(int)(x + _tmp76);env=Cyc_Marshal_fprint_type_base(
_tmp77,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp27,
sizeof(struct _tuple1*))- 1)({void*_tmp78[0]={};Cyc_Std_fprintf(fp,({const char*
_tmp79=", ";_tag_arr(_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,3));}),
_tag_arr(_tmp78,sizeof(void*),0));});}}}({void*_tmp7A[0]={};Cyc_Std_fprintf(fp,({
const char*_tmp7B=" )";_tag_arr(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,3));}),
_tag_arr(_tmp7A,sizeof(void*),0));});goto _LL0;}_LLF: if(_tmp1F <= (void*)2?1:*((
int*)_tmp1F)!= 6)goto _LL11;_tmp28=((struct Cyc_Typerep_TUnionField_struct*)_tmp1F)->f1;
_tmp29=((struct Cyc_Typerep_TUnionField_struct*)_tmp1F)->f2;_tmp2A=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1F)->f4;_LL10:({struct Cyc_Std_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(
struct _tagged_arr)_tmp29;{struct Cyc_Std_String_pa_struct _tmp7E;_tmp7E.tag=0;
_tmp7E.f1=(struct _tagged_arr)_tmp28;{void*_tmp7C[2]={& _tmp7E,& _tmp7F};Cyc_Std_fprintf(
fp,({const char*_tmp7D="%s.%s(";_tag_arr(_tmp7D,sizeof(char),_get_zero_arr_size(
_tmp7D,7));}),_tag_arr(_tmp7C,sizeof(void*),2));}}});{int x=((int(*)(void*x))
unsafe_cast)(val);{int i=1;for(0;i < _get_arr_size(_tmp2A,sizeof(struct _tuple1*));
i ++){unsigned int _tmp81;void*_tmp82;struct _tuple1 _tmp80=*((struct _tuple1*)((
struct _tuple1**)_tmp2A.curr)[i]);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{int x_ofs=(
int)(x + _tmp81);env=Cyc_Marshal_fprint_type_base(_tmp82,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));if(i != _get_arr_size(_tmp2A,sizeof(struct _tuple1*))- 1)({
void*_tmp83[0]={};Cyc_Std_fprintf(fp,({const char*_tmp84=", ";_tag_arr(_tmp84,
sizeof(char),_get_zero_arr_size(_tmp84,3));}),_tag_arr(_tmp83,sizeof(void*),0));});}}}({
void*_tmp85[0]={};Cyc_Std_fprintf(fp,({const char*_tmp86=" )";_tag_arr(_tmp86,
sizeof(char),_get_zero_arr_size(_tmp86,3));}),_tag_arr(_tmp85,sizeof(void*),0));});
goto _LL0;}_LL11: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 5)goto _LL13;_tmp2B=((
struct Cyc_Typerep_TUnion_struct*)_tmp1F)->f1;_tmp2C=((struct Cyc_Typerep_TUnion_struct*)
_tmp1F)->f2;_tmp2D=((struct Cyc_Typerep_TUnion_struct*)_tmp1F)->f3;_LL12: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*((
unsigned int*)tagp)< 1024)({struct Cyc_Std_String_pa_struct _tmp89;_tmp89.tag=0;
_tmp89.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname(*((unsigned int*)tagp),
_tmp2C);{void*_tmp87[1]={& _tmp89};Cyc_Std_fprintf(fp,({const char*_tmp88="%s";
_tag_arr(_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,3));}),_tag_arr(_tmp87,
sizeof(void*),1));}});else{struct _tuple8 _tmp8B;int _tmp8C;struct Cyc_Set_Set*
_tmp8D;struct _tuple8*_tmp8A=Cyc_Marshal_member_insert(env,*((unsigned int*)tagp));
_tmp8B=*_tmp8A;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;env=_tmp8D;if(!_tmp8C){({struct
Cyc_Std_Int_pa_struct _tmp90;_tmp90.tag=1;_tmp90.f1=*((unsigned int*)tagp);{void*
_tmp8E[1]={& _tmp90};Cyc_Std_fprintf(fp,({const char*_tmp8F="(@%x)";_tag_arr(
_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,6));}),_tag_arr(_tmp8E,sizeof(void*),
1));}});{unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*((
unsigned int*)tagp));unsigned int tag=*((unsigned int*)x);int _tmp92;void*_tmp93;
struct _tuple4 _tmp91=Cyc_Typerep_get_unionbranch(tag,_tmp2D);_tmp92=_tmp91.f1;
_tmp93=_tmp91.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*((
unsigned int*)tagp)+ _tmp92);({struct Cyc_Std_String_pa_struct _tmp96;_tmp96.tag=0;
_tmp96.f1=(struct _tagged_arr)Cyc_Typerep_get_tagname2(tag,_tmp2D);{void*_tmp94[1]={&
_tmp96};Cyc_Std_fprintf(fp,({const char*_tmp95="&%s[";_tag_arr(_tmp95,sizeof(char),
_get_zero_arr_size(_tmp95,5));}),_tag_arr(_tmp94,sizeof(void*),1));}});env=Cyc_Marshal_fprint_type_base(
_tmp93,env,fp,xoff);({void*_tmp97[0]={};Cyc_Std_fprintf(fp,({const char*_tmp98="]";
_tag_arr(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,2));}),_tag_arr(_tmp97,
sizeof(void*),0));});}}}else{Cyc_Marshal_fprint_ref(fp,*((unsigned int*)tagp));}}
goto _LL0;}_LL13: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 7)goto _LL15;_tmp2E=((
struct Cyc_Typerep_XTUnion_struct*)_tmp1F)->f1;_tmp2F=((struct Cyc_Typerep_XTUnion_struct*)
_tmp1F)->f2;_LL14: {unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(
val);struct _tuple8 _tmp9A;int _tmp9B;struct Cyc_Set_Set*_tmp9C;struct _tuple8*_tmp99=
Cyc_Marshal_member_insert(env,*((unsigned int*)xtunionp));_tmp9A=*_tmp99;_tmp9B=
_tmp9A.f1;_tmp9C=_tmp9A.f2;env=_tmp9C;if(!_tmp9B){({struct Cyc_Std_Int_pa_struct
_tmp9F;_tmp9F.tag=1;_tmp9F.f1=*((unsigned int*)xtunionp);{void*_tmp9D[1]={&
_tmp9F};Cyc_Std_fprintf(fp,({const char*_tmp9E="(@%x)";_tag_arr(_tmp9E,sizeof(
char),_get_zero_arr_size(_tmp9E,6));}),_tag_arr(_tmp9D,sizeof(void*),1));}});{
unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*((
unsigned int*)xtunionp));if(*((unsigned int*)xtstructp)!= 0){char*xtnamec=((char*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)xtstructp)+ 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _tagged_arr)xtname;{void*_tmpA0[1]={& _tmpA2};
Cyc_Std_fprintf(fp,({const char*_tmpA1="&%s[( ";_tag_arr(_tmpA1,sizeof(char),
_get_zero_arr_size(_tmpA1,7));}),_tag_arr(_tmpA0,sizeof(void*),1));}});{int
_tmpA4;void*_tmpA5;struct _tuple4 _tmpA3=Cyc_Typerep_get_xtunionbranch(xtname,
_tmp2F);_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;{void*xoff=((void*(*)(unsigned int x))
unsafe_cast)(*((unsigned int*)xtunionp)+ _tmpA4);env=Cyc_Marshal_fprint_type_base(
_tmpA5,env,fp,xoff);({void*_tmpA6[0]={};Cyc_Std_fprintf(fp,({const char*_tmpA7=" )]";
_tag_arr(_tmpA7,sizeof(char),_get_zero_arr_size(_tmpA7,4));}),_tag_arr(_tmpA6,
sizeof(void*),0));});}}}else{char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*((
unsigned int*)xtunionp)+ 4);struct _tagged_arr xtname=(struct _tagged_arr)
Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct _tmpAA;_tmpAA.tag=0;
_tmpAA.f1=(struct _tagged_arr)xtname;{void*_tmpA8[1]={& _tmpAA};Cyc_Std_fprintf(fp,({
const char*_tmpA9="%s";_tag_arr(_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,3));}),
_tag_arr(_tmpA8,sizeof(void*),1));}});}}}else{Cyc_Marshal_fprint_ref(fp,*((
unsigned int*)xtunionp));}goto _LL0;}_LL15: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)
!= 8)goto _LL17;_tmp30=((struct Cyc_Typerep_Union_struct*)_tmp1F)->f1;_tmp31=((
struct Cyc_Typerep_Union_struct*)_tmp1F)->f3;_LL16: if(_tmp30 == 0)({void*_tmpAB[0]={};
Cyc_Std_fprintf(fp,({const char*_tmpAC="{";_tag_arr(_tmpAC,sizeof(char),
_get_zero_arr_size(_tmpAC,2));}),_tag_arr(_tmpAB,sizeof(void*),0));});else{({
struct Cyc_Std_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp30);{void*_tmpAD[1]={& _tmpAF};Cyc_Std_fprintf(fp,({const
char*_tmpAE="%s{";_tag_arr(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,4));}),
_tag_arr(_tmpAD,sizeof(void*),1));}});}{int i=0;for(0;i < _get_arr_size(_tmp31,
sizeof(struct _tuple3*));i ++){struct _tagged_arr _tmpB1;void*_tmpB2;struct _tuple3
_tmpB0=*((struct _tuple3*)((struct _tuple3**)_tmp31.curr)[i]);_tmpB1=_tmpB0.f1;
_tmpB2=_tmpB0.f2;({struct Cyc_Std_String_pa_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(
struct _tagged_arr)_tmpB1;{void*_tmpB3[1]={& _tmpB5};Cyc_Std_fprintf(fp,({const
char*_tmpB4="%s=";_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,4));}),
_tag_arr(_tmpB3,sizeof(void*),1));}});Cyc_Marshal_fprint_type_base(_tmpB2,env,fp,
val);if(i < _get_arr_size(_tmp31,sizeof(struct _tuple3*))- 1)({void*_tmpB6[0]={};
Cyc_Std_fprintf(fp,({const char*_tmpB7=" | ";_tag_arr(_tmpB7,sizeof(char),
_get_zero_arr_size(_tmpB7,4));}),_tag_arr(_tmpB6,sizeof(void*),0));});}}({void*
_tmpB8[0]={};Cyc_Std_fprintf(fp,({const char*_tmpB9="}";_tag_arr(_tmpB9,sizeof(
char),_get_zero_arr_size(_tmpB9,2));}),_tag_arr(_tmpB8,sizeof(void*),0));});goto
_LL0;_LL17: if(_tmp1F <= (void*)2?1:*((int*)_tmp1F)!= 9)goto _LL0;_tmp32=((struct
Cyc_Typerep_Enum_struct*)_tmp1F)->f2;_tmp33=((struct Cyc_Typerep_Enum_struct*)
_tmp1F)->f3;_LL18: {unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(
val);({struct Cyc_Std_String_pa_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct
_tagged_arr)Cyc_Typerep_get_tagname(*((unsigned int*)tagp),_tmp33);{void*_tmpBA[
1]={& _tmpBC};Cyc_Std_fprintf(fp,({const char*_tmpBB="%s";_tag_arr(_tmpBB,sizeof(
char),_get_zero_arr_size(_tmpBB,3));}),_tag_arr(_tmpBA,sizeof(void*),1));}});
goto _LL0;}_LL0:;}return env;}void Cyc_Marshal_fprint_type(void*ts,struct Cyc_Std___cycFILE*
fp,void*val){struct _handler_cons _tmpBD;_push_handler(& _tmpBD);{int _tmpBF=0;if(
setjmp(_tmpBD.handler))_tmpBF=1;if(!_tmpBF){Cyc_Marshal_fprint_type_base(ts,Cyc_Marshal_empty_addr_set(),
fp,val);;_pop_handler();}else{void*_tmpBE=(void*)_exn_thrown;void*_tmpC1=_tmpBE;
struct _tagged_arr _tmpC2;_LL1F: if(*((void**)_tmpC1)!= Cyc_Core_Failure)goto _LL21;
_tmpC2=((struct Cyc_Core_Failure_struct*)_tmpC1)->f1;_LL20:({struct Cyc_Std_String_pa_struct
_tmpC5;_tmpC5.tag=0;_tmpC5.f1=(struct _tagged_arr)_tmpC2;{void*_tmpC3[1]={& _tmpC5};
Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmpC4="Core::Failure(%s)\n";_tag_arr(
_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,19));}),_tag_arr(_tmpC3,sizeof(
void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Core_Failure_struct _tmpC7;
_tmpC7.tag=Cyc_Core_Failure;_tmpC7.f1=_tmpC2;_tmpC7;});_tmpC6;}));_LL21:;_LL22:(
void)_throw(_tmpC1);_LL1E:;}}}void Cyc_Marshal_print_type(void*ts,void*val){Cyc_Marshal_fprint_type(
ts,Cyc_Std_stdout,val);}void Cyc_Marshal_cycputchar(char x,struct Cyc_Std___cycFILE*
fp){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpC8=_cycalloc(sizeof(*
_tmpC8));_tmpC8[0]=({struct Cyc_Core_Failure_struct _tmpC9;_tmpC9.tag=Cyc_Core_Failure;
_tmpC9.f1=({const char*_tmpCA="Write failure";_tag_arr(_tmpCA,sizeof(char),
_get_zero_arr_size(_tmpCA,14));});_tmpC9;});_tmpC8;}));}void Cyc_Marshal_cycputshort(
short x,struct Cyc_Std___cycFILE*fp){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Core_Failure_struct
_tmpCC;_tmpCC.tag=Cyc_Core_Failure;_tmpCC.f1=({const char*_tmpCD="Write failure";
_tag_arr(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,14));});_tmpCC;});_tmpCB;}));}
void Cyc_Marshal_cycputint(int x,struct Cyc_Std___cycFILE*fp){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({
struct Cyc_Core_Failure_struct _tmpCF;_tmpCF.tag=Cyc_Core_Failure;_tmpCF.f1=({
const char*_tmpD0="Write failure";_tag_arr(_tmpD0,sizeof(char),_get_zero_arr_size(
_tmpD0,14));});_tmpCF;});_tmpCE;}));}void Cyc_Marshal_cycputfloat(float x,struct
Cyc_Std___cycFILE*fp){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpD1=
_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Core_Failure_struct _tmpD2;
_tmpD2.tag=Cyc_Core_Failure;_tmpD2.f1=({const char*_tmpD3="Write failure";
_tag_arr(_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,14));});_tmpD2;});_tmpD1;}));}
void Cyc_Marshal_cycputdouble(double x,struct Cyc_Std___cycFILE*fp){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({
struct Cyc_Core_Failure_struct _tmpD5;_tmpD5.tag=Cyc_Core_Failure;_tmpD5.f1=({
const char*_tmpD6="Write failure";_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(
_tmpD6,14));});_tmpD5;});_tmpD4;}));}void Cyc_Marshal_cycputvarint(unsigned int i,
struct Cyc_Std___cycFILE*fp){int width=0;unsigned int mask=-256;while((int)mask){
if((int)(i & mask))width ++;mask <<=8;}Cyc_Std_fputc((int)(width << 6 | i & 63),fp);i
>>=6;{int j=0;for(0;j < width;j ++){Cyc_Std_fputc((int)(i & 255),fp);i >>=8;}}}char
Cyc_Marshal_cycgetchar(struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpD7=
_new_region("r");struct _RegionHandle*r=& _tmpD7;_push_region(r);{struct
_tagged_arr x=({char*_tmpDF=({unsigned int _tmpDC=sizeof(char);char*_tmpDD=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmpDC + 1));{unsigned int _tmpDE=
_tmpDC;unsigned int i;for(i=0;i < _tmpDE;i ++){_tmpDD[i]='\000';}_tmpDD[_tmpDE]=(
char)0;}_tmpDD;});_tag_arr(_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,sizeof(
char)+ 1));});if(Cyc_Std_fread(x,sizeof(char),1,fp)< 1)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({
struct Cyc_Core_Failure_struct _tmpD9;_tmpD9.tag=Cyc_Core_Failure;_tmpD9.f1=({
const char*_tmpDA="Read failure";_tag_arr(_tmpDA,sizeof(char),_get_zero_arr_size(
_tmpDA,13));});_tmpD9;});_tmpD8;}));{char*xx=((char*(*)(char*x))unsafe_cast)((
char*)_check_null(_untag_arr(x,sizeof(char),1 + 1)));char _tmpDB=*((char*)xx);
_npop_handler(0);return _tmpDB;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpE0=_new_region("r");struct
_RegionHandle*r=& _tmpE0;_push_region(r);{struct _tagged_arr x=({char*_tmpE8=({
unsigned int _tmpE5=sizeof(short);char*_tmpE6=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmpE5 + 1));{unsigned int _tmpE7=_tmpE5;unsigned int i;
for(i=0;i < _tmpE7;i ++){_tmpE6[i]='\000';}_tmpE6[_tmpE7]=(char)0;}_tmpE6;});
_tag_arr(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,sizeof(short)+ 1));});if(
Cyc_Std_fread(x,sizeof(short),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Core_Failure_struct
_tmpE2;_tmpE2.tag=Cyc_Core_Failure;_tmpE2.f1=({const char*_tmpE3="Read failure";
_tag_arr(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,13));});_tmpE2;});_tmpE1;}));{
short*xx=((short*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,
sizeof(char),1 + 1)));short _tmpE4=*((short*)xx);_npop_handler(0);return _tmpE4;}};
_pop_region(r);}int Cyc_Marshal_cycgetint(struct Cyc_Std___cycFILE*fp){struct
_RegionHandle _tmpE9=_new_region("r");struct _RegionHandle*r=& _tmpE9;_push_region(
r);{struct _tagged_arr x=({char*_tmpF1=({unsigned int _tmpEE=sizeof(int);char*
_tmpEF=(char*)_region_malloc(r,_check_times(sizeof(char),_tmpEE + 1));{
unsigned int _tmpF0=_tmpEE;unsigned int i;for(i=0;i < _tmpF0;i ++){_tmpEF[i]='\000';}
_tmpEF[_tmpF0]=(char)0;}_tmpEF;});_tag_arr(_tmpF1,sizeof(char),
_get_zero_arr_size(_tmpF1,sizeof(int)+ 1));});if(Cyc_Std_fread(x,sizeof(int),1,
fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpEA=_cycalloc(
sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Core_Failure_struct _tmpEB;_tmpEB.tag=Cyc_Core_Failure;
_tmpEB.f1=({const char*_tmpEC="Read failure";_tag_arr(_tmpEC,sizeof(char),
_get_zero_arr_size(_tmpEC,13));});_tmpEB;});_tmpEA;}));{int*xx=((int*(*)(char*x))
unsafe_cast)((char*)_check_null(_untag_arr(x,sizeof(char),1 + 1)));int _tmpED=*((
int*)xx);_npop_handler(0);return _tmpED;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(
struct Cyc_Std___cycFILE*fp){struct _RegionHandle _tmpF2=_new_region("r");struct
_RegionHandle*r=& _tmpF2;_push_region(r);{struct _tagged_arr x=({char*_tmpFA=({
unsigned int _tmpF7=sizeof(float);char*_tmpF8=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmpF7 + 1));{unsigned int _tmpF9=_tmpF7;unsigned int i;
for(i=0;i < _tmpF9;i ++){_tmpF8[i]='\000';}_tmpF8[_tmpF9]=(char)0;}_tmpF8;});
_tag_arr(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,sizeof(float)+ 1));});if(
Cyc_Std_fread(x,sizeof(float),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Core_Failure_struct
_tmpF4;_tmpF4.tag=Cyc_Core_Failure;_tmpF4.f1=({const char*_tmpF5="Read failure";
_tag_arr(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,13));});_tmpF4;});_tmpF3;}));{
float*xx=((float*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,
sizeof(char),1 + 1)));float _tmpF6=*((float*)xx);_npop_handler(0);return _tmpF6;}};
_pop_region(r);}double Cyc_Marshal_cycgetdouble(struct Cyc_Std___cycFILE*fp){
struct _RegionHandle _tmpFB=_new_region("r");struct _RegionHandle*r=& _tmpFB;
_push_region(r);{struct _tagged_arr x=({char*_tmp103=({unsigned int _tmp100=sizeof(
double);char*_tmp101=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp100 + 
1));{unsigned int _tmp102=_tmp100;unsigned int i;for(i=0;i < _tmp102;i ++){_tmp101[i]='\000';}
_tmp101[_tmp102]=(char)0;}_tmp101;});_tag_arr(_tmp103,sizeof(char),
_get_zero_arr_size(_tmp103,sizeof(double)+ 1));});if(Cyc_Std_fread(x,sizeof(
double),1,fp)< 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpFC=
_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Core_Failure_struct _tmpFD;
_tmpFD.tag=Cyc_Core_Failure;_tmpFD.f1=({const char*_tmpFE="Read failure";_tag_arr(
_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,13));});_tmpFD;});_tmpFC;}));{
double*xx=((double*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_arr(x,
sizeof(char),1 + 1)));double _tmpFF=*((double*)xx);_npop_handler(0);return _tmpFF;}};
_pop_region(r);}unsigned int Cyc_Marshal_cycgetvarint(struct Cyc_Std___cycFILE*fp){
int i=0;int j=(int)Cyc_Marshal_cycgetchar(fp);int width=(j & 192)>> 6;int pos=6;i +=j & 
63;{int k=0;for(0;k < width;k ++){j=(int)Cyc_Marshal_cycgetchar(fp);i |=j << pos;pos +=
8;}}return(unsigned int)i;}struct _tuple6 Cyc_Marshal_write_type_base(void*rep,
struct _tuple6 env,struct Cyc_Std___cycFILE*fp,void*val){{void*_tmp104=rep;
unsigned int _tmp105;unsigned int _tmp106;void*_tmp107;void*_tmp108;struct
_tagged_arr _tmp109;struct _tagged_arr _tmp10A;struct _tagged_arr _tmp10B;struct
_tagged_arr _tmp10C;struct _tagged_arr _tmp10D;struct _tagged_arr _tmp10E;int _tmp10F;
int _tmp110;_LL24: if(_tmp104 <= (void*)2?1:*((int*)_tmp104)!= 0)goto _LL26;_tmp105=((
struct Cyc_Typerep_Int_struct*)_tmp104)->f2;_LL25: switch(_tmp105){case 8: _LL3C: {
char*x=((char*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputchar(*((char*)x),
fp);break;}case 16: _LL3D: {short*x=((short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*((
short*)x),fp);break;}case 32: _LL3E: {int*x=((int*(*)(void*x))unsafe_cast)(val);
Cyc_Marshal_cycputint(*((int*)x),fp);break;}default: _LL3F:({struct Cyc_Std_Int_pa_struct
_tmp113;_tmp113.tag=1;_tmp113.f1=(int)_tmp105;{void*_tmp111[1]={& _tmp113};Cyc_Std_fprintf(
Cyc_Std_stderr,({const char*_tmp112="write_type_base: illegal int size %d bits\n";
_tag_arr(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,43));}),_tag_arr(
_tmp111,sizeof(void*),1));}});exit(1);}goto _LL23;_LL26: if((int)_tmp104 != 0)goto
_LL28;_LL27: {float*x=((float*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputfloat(*((
float*)x),fp);goto _LL23;}_LL28: if((int)_tmp104 != 1)goto _LL2A;_LL29: {double*x=((
double*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*((double*)x),fp);
goto _LL23;}_LL2A: if(_tmp104 <= (void*)2?1:*((int*)_tmp104)!= 1)goto _LL2C;_tmp106=((
struct Cyc_Typerep_ThinPtr_struct*)_tmp104)->f1;_tmp107=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp104)->f2;_LL2B: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));unsigned int _tmp114=Cyc_Marshal_index_lookup(env,(unsigned int)y);
Cyc_Marshal_cycputvarint(_tmp114,fp);if(!Cyc_Marshal_index_member(env,_tmp114)){
env=Cyc_Marshal_index_insert(env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp107);int i=0;for(0;i < _tmp106;i ++){env=Cyc_Marshal_write_type_base(_tmp107,
env,fp,((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}goto _LL23;}_LL2C: if(
_tmp104 <= (void*)2?1:*((int*)_tmp104)!= 2)goto _LL2E;_tmp108=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp104)->f1;_LL2D: {struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(
val);unsigned int typ_szb=Cyc_Typerep_size_type(_tmp108);unsigned int base=(*((
struct _tuple9*)x)).f1;unsigned int curr=(*((struct _tuple9*)x)).f2;unsigned int
last_plus_one=(*((struct _tuple9*)x)).f3;unsigned int pos=(curr - base)/ typ_szb;
unsigned int sz=(last_plus_one - base)/ typ_szb;unsigned int _tmp115=Cyc_Marshal_index_lookup(
env,base);Cyc_Marshal_cycputvarint(_tmp115,fp);Cyc_Marshal_cycputvarint(pos,fp);
Cyc_Marshal_cycputvarint(sz,fp);if(!Cyc_Marshal_index_member(env,_tmp115)){env=
Cyc_Marshal_index_insert(env,base);{unsigned int i=base;for(0;i < last_plus_one;i +=
typ_szb){env=Cyc_Marshal_write_type_base(_tmp108,env,fp,((void*(*)(unsigned int x))
unsafe_cast)(i));}}}goto _LL23;}_LL2E: if(_tmp104 <= (void*)2?1:*((int*)_tmp104)!= 
3)goto _LL30;_tmp109=((struct Cyc_Typerep_Struct_struct*)_tmp104)->f3;_LL2F: {int x=((
int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_arr_size(_tmp109,sizeof(
struct _tuple0*));i ++){unsigned int _tmp117;void*_tmp118;struct _tuple0 _tmp116=*((
struct _tuple0*)((struct _tuple0**)_tmp109.curr)[i]);_tmp117=_tmp116.f1;_tmp118=
_tmp116.f3;{int x_ofs=(int)(x + _tmp117);env=Cyc_Marshal_write_type_base(_tmp118,
env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL30: if(_tmp104 <= (
void*)2?1:*((int*)_tmp104)!= 4)goto _LL32;_tmp10A=((struct Cyc_Typerep_Tuple_struct*)
_tmp104)->f2;_LL31: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_arr_size(_tmp10A,sizeof(struct _tuple1*));i ++){unsigned int _tmp11A;void*
_tmp11B;struct _tuple1 _tmp119=*((struct _tuple1*)((struct _tuple1**)_tmp10A.curr)[i]);
_tmp11A=_tmp119.f1;_tmp11B=_tmp119.f2;{int x_ofs=(int)(x + _tmp11A);env=Cyc_Marshal_write_type_base(
_tmp11B,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL32: if(
_tmp104 <= (void*)2?1:*((int*)_tmp104)!= 6)goto _LL34;_tmp10B=((struct Cyc_Typerep_TUnionField_struct*)
_tmp104)->f4;_LL33: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_arr_size(_tmp10B,sizeof(struct _tuple1*));i ++){unsigned int _tmp11D;void*
_tmp11E;struct _tuple1 _tmp11C=*((struct _tuple1*)((struct _tuple1**)_tmp10B.curr)[i]);
_tmp11D=_tmp11C.f1;_tmp11E=_tmp11C.f2;{int x_ofs=(int)(x + _tmp11D);env=Cyc_Marshal_write_type_base(
_tmp11E,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL34: if(
_tmp104 <= (void*)2?1:*((int*)_tmp104)!= 5)goto _LL36;_tmp10C=((struct Cyc_Typerep_TUnion_struct*)
_tmp104)->f2;_tmp10D=((struct Cyc_Typerep_TUnion_struct*)_tmp104)->f3;_LL35: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*((
unsigned int*)tagp)< 1024)Cyc_Marshal_cycputvarint(*((unsigned int*)tagp),fp);
else{unsigned int _tmp11F=Cyc_Marshal_index_lookup(env,*((unsigned int*)tagp));
Cyc_Marshal_cycputvarint(_tmp11F + 1024,fp);if(!Cyc_Marshal_index_member(env,
_tmp11F)){env=Cyc_Marshal_index_insert(env,*((unsigned int*)tagp));{unsigned int*
x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)tagp));
unsigned int tag=*((unsigned int*)x);Cyc_Marshal_cycputvarint(tag,fp);{int _tmp121;
void*_tmp122;struct _tuple4 _tmp120=Cyc_Typerep_get_unionbranch(tag,_tmp10D);
_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;{void*xoff=((void*(*)(unsigned int x))
unsafe_cast)(*((unsigned int*)tagp)+ _tmp121);env=Cyc_Marshal_write_type_base(
_tmp122,env,fp,xoff);}}}}}goto _LL23;}_LL36: if(_tmp104 <= (void*)2?1:*((int*)
_tmp104)!= 7)goto _LL38;_tmp10E=((struct Cyc_Typerep_XTUnion_struct*)_tmp104)->f2;
_LL37: {unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(val);
unsigned int _tmp123=Cyc_Marshal_index_lookup(env,*((unsigned int*)xtunionp));Cyc_Marshal_cycputvarint(
_tmp123,fp);if(!Cyc_Marshal_index_member(env,_tmp123)){env=Cyc_Marshal_index_insert(
env,*((unsigned int*)xtunionp));{unsigned int*xtstructp=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)xtunionp));if(*((unsigned int*)
xtstructp)!= 0){Cyc_Marshal_cycputchar('\001',fp);{char*xtnamec=((char*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)xtstructp)+ 4);struct _tagged_arr
xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);({struct Cyc_Std_String_pa_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=(struct _tagged_arr)xtname;{void*_tmp124[1]={&
_tmp126};Cyc_Std_fprintf(fp,({const char*_tmp125="%s";_tag_arr(_tmp125,sizeof(
char),_get_zero_arr_size(_tmp125,3));}),_tag_arr(_tmp124,sizeof(void*),1));}});
Cyc_Marshal_cycputchar('\000',fp);{int _tmp128;void*_tmp129;struct _tuple4 _tmp127=
Cyc_Typerep_get_xtunionbranch(xtname,_tmp10E);_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;{
void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)xtunionp)+ 
_tmp128);env=Cyc_Marshal_write_type_base(_tmp129,env,fp,xoff);}}}}else{Cyc_Marshal_cycputchar('\000',
fp);{char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)
xtunionp)+ 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(
xtnamec);({struct Cyc_Std_String_pa_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(
struct _tagged_arr)xtname;{void*_tmp12A[1]={& _tmp12C};Cyc_Std_fprintf(fp,({const
char*_tmp12B="%s";_tag_arr(_tmp12B,sizeof(char),_get_zero_arr_size(_tmp12B,3));}),
_tag_arr(_tmp12A,sizeof(void*),1));}});Cyc_Marshal_cycputchar('\000',fp);}}}}
goto _LL23;}_LL38: if(_tmp104 <= (void*)2?1:*((int*)_tmp104)!= 8)goto _LL3A;_tmp10F=((
struct Cyc_Typerep_Union_struct*)_tmp104)->f2;_LL39: env=Cyc_Marshal_write_type_base((
void*)({struct Cyc_Typerep_Tuple_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));
_tmp12D[0]=({struct Cyc_Typerep_Tuple_struct _tmp12E;_tmp12E.tag=4;_tmp12E.f1=(
unsigned int)_tmp10F;_tmp12E.f2=({unsigned int _tmp12F=(unsigned int)_tmp10F;
struct _tuple1**_tmp130=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct
_tuple1*),_tmp12F));struct _tagged_arr _tmp135=_tag_arr(_tmp130,sizeof(struct
_tuple1*),_tmp12F);{unsigned int _tmp131=_tmp12F;unsigned int i;for(i=0;i < _tmp131;
i ++){_tmp130[i]=({struct _tuple1*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->f1=
i;_tmp132->f2=(void*)({struct Cyc_Typerep_Int_struct*_tmp133=_cycalloc_atomic(
sizeof(*_tmp133));_tmp133[0]=({struct Cyc_Typerep_Int_struct _tmp134;_tmp134.tag=0;
_tmp134.f1=0;_tmp134.f2=8;_tmp134;});_tmp133;});_tmp132;});}}_tmp135;});_tmp12E;});
_tmp12D;}),env,fp,val);goto _LL23;_LL3A: if(_tmp104 <= (void*)2?1:*((int*)_tmp104)
!= 9)goto _LL23;_tmp110=((struct Cyc_Typerep_Enum_struct*)_tmp104)->f2;_LL3B: env=
Cyc_Marshal_write_type_base((void*)({struct Cyc_Typerep_Int_struct*_tmp136=
_cycalloc_atomic(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Typerep_Int_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=0;_tmp137.f2=(unsigned int)(_tmp110 << 3);
_tmp137;});_tmp136;}),env,fp,val);_LL23:;}return env;}void Cyc_Marshal_write_type(
void*ts,struct Cyc_Std___cycFILE*fp,void*val){struct _handler_cons _tmp138;
_push_handler(& _tmp138);{int _tmp13A=0;if(setjmp(_tmp138.handler))_tmp13A=1;if(!
_tmp13A){Cyc_Marshal_write_type_base(ts,Cyc_Marshal_empty_addr_index(),fp,val);;
_pop_handler();}else{void*_tmp139=(void*)_exn_thrown;void*_tmp13C=_tmp139;struct
_tagged_arr _tmp13D;_LL42: if(*((void**)_tmp13C)!= Cyc_Core_Failure)goto _LL44;
_tmp13D=((struct Cyc_Core_Failure_struct*)_tmp13C)->f1;_LL43:({struct Cyc_Std_String_pa_struct
_tmp140;_tmp140.tag=0;_tmp140.f1=(struct _tagged_arr)_tmp13D;{void*_tmp13E[1]={&
_tmp140};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp13F="Core::Failure(%s)\n";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,19));}),_tag_arr(
_tmp13E,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141[0]=({struct Cyc_Core_Failure_struct
_tmp142;_tmp142.tag=Cyc_Core_Failure;_tmp142.f1=_tmp13D;_tmp142;});_tmp141;}));
_LL44:;_LL45:(void)_throw(_tmp13C);_LL41:;}}}void Cyc_Marshal_fscanntstring(
struct Cyc_Std___cycFILE*fp,struct _tagged_arr s){int c;int i=0;while((c=Cyc_Std_fgetc(
fp))!= '\000'){if(c == - 1)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Core_Failure_struct
_tmp144;_tmp144.tag=Cyc_Core_Failure;_tmp144.f1=({const char*_tmp145="Read error";
_tag_arr(_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,11));});_tmp144;});
_tmp143;}));({struct _tagged_arr _tmp146=_tagged_arr_plus(s,sizeof(char),i);char
_tmp147=*((char*)_check_unknown_subscript(_tmp146,sizeof(char),0));char _tmp148=(
char)c;if(_get_arr_size(_tmp146,sizeof(char))== 1?_tmp147 == '\000'?_tmp148 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp146.curr)=_tmp148;});i ++;}({struct
_tagged_arr _tmp149=_tagged_arr_plus(s,sizeof(char),i);char _tmp14A=*((char*)
_check_unknown_subscript(_tmp149,sizeof(char),0));char _tmp14B='\000';if(
_get_arr_size(_tmp149,sizeof(char))== 1?_tmp14A == '\000'?_tmp14B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp149.curr)=_tmp14B;});}struct _tuple7 Cyc_Marshal_rread_type_base(
struct _RegionHandle*r,void*rep,struct _tuple7 env,struct Cyc_Std___cycFILE*fp,void*
result){{void*_tmp14C=rep;unsigned int _tmp14D;unsigned int _tmp14E;void*_tmp14F;
void*_tmp150;struct _tagged_arr _tmp151;struct _tagged_arr _tmp152;struct _tagged_arr
_tmp153;struct _tagged_arr _tmp154;struct _tagged_arr _tmp155;struct _tagged_arr
_tmp156;int _tmp157;int _tmp158;_LL47: if(_tmp14C <= (void*)2?1:*((int*)_tmp14C)!= 0)
goto _LL49;_tmp14D=((struct Cyc_Typerep_Int_struct*)_tmp14C)->f2;_LL48: switch(
_tmp14D){case 8: _LL5F: {char*x=((char*(*)(void*x))unsafe_cast)(result);({struct
_tagged_arr _tmp15A=({char*_tmp159=x;_tag_arr(_tmp159,sizeof(char),
_get_zero_arr_size(_tmp159,1));});char _tmp15B=*((char*)_check_unknown_subscript(
_tmp15A,sizeof(char),0));char _tmp15C=Cyc_Marshal_cycgetchar(fp);if(_get_arr_size(
_tmp15A,sizeof(char))== 1?_tmp15B == '\000'?_tmp15C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp15A.curr)=_tmp15C;});break;}case 16: _LL60: {
short*x=((short*(*)(void*x))unsafe_cast)(result);*((short*)x)=Cyc_Marshal_cycgetshort(
fp);break;}case 32: _LL61: {int*x=((int*(*)(void*x))unsafe_cast)(result);*((int*)x)=
Cyc_Marshal_cycgetint(fp);break;}default: _LL62:(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D[0]=({struct Cyc_Core_Failure_struct
_tmp15E;_tmp15E.tag=Cyc_Core_Failure;_tmp15E.f1=({const char*_tmp15F="read_type_base: illegal int size bits";
_tag_arr(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,38));});_tmp15E;});
_tmp15D;}));}goto _LL46;_LL49: if((int)_tmp14C != 0)goto _LL4B;_LL4A: {float*y=((
float*(*)(void*x))unsafe_cast)(result);*((float*)y)=Cyc_Marshal_cycgetfloat(fp);
goto _LL46;}_LL4B: if((int)_tmp14C != 1)goto _LL4D;_LL4C: {double*y=((double*(*)(
void*x))unsafe_cast)(result);*((double*)y)=Cyc_Marshal_cycgetdouble(fp);goto
_LL46;}_LL4D: if(_tmp14C <= (void*)2?1:*((int*)_tmp14C)!= 1)goto _LL4F;_tmp14E=((
struct Cyc_Typerep_ThinPtr_struct*)_tmp14C)->f1;_tmp14F=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp14C)->f2;_LL4E: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmp14F);struct
_tagged_arr buf=({unsigned int _tmp160=_tmp14E * typ_szb;char*_tmp161=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp160 + 1));struct _tagged_arr _tmp163=
_tag_arr(_tmp161,sizeof(char),_tmp160 + 1);{unsigned int _tmp162=_tmp160;
unsigned int i;for(i=0;i < _tmp162;i ++){_tmp161[i]='\000';}_tmp161[_tmp162]=(char)
0;}_tmp163;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(
result);*((unsigned int*)x)=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int
y=(int)bufptr;int i=0;for(0;i < _tmp14E;i ++){env=Cyc_Marshal_rread_type_base(r,
_tmp14F,env,fp,((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*
x=((unsigned int*(*)(void*x))unsafe_cast)(result);*((unsigned int*)x)=Cyc_Marshal_table_lookup(
env,mem);}goto _LL46;}_LL4F: if(_tmp14C <= (void*)2?1:*((int*)_tmp14C)!= 2)goto
_LL51;_tmp150=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp14C)->f1;_LL50: {int
mem=(int)Cyc_Marshal_cycgetvarint(fp);int pos=(int)Cyc_Marshal_cycgetvarint(fp);
int sz=(int)Cyc_Marshal_cycgetvarint(fp);int typ_szb=(int)Cyc_Typerep_size_type(
_tmp150);if(!Cyc_Marshal_table_member(env,(unsigned int)mem)){struct _tagged_arr
buf=({unsigned int _tmp164=(unsigned int)(sz * typ_szb);char*_tmp165=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp164 + 1));struct _tagged_arr _tmp167=
_tag_arr(_tmp165,sizeof(char),_tmp164 + 1);{unsigned int _tmp166=_tmp164;
unsigned int i;for(i=0;i < _tmp166;i ++){_tmp165[i]='\000';}_tmp165[_tmp166]=(char)
0;}_tmp167;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(
result);(*((struct _tuple9*)x)).f1=bufptr;(*((struct _tuple9*)x)).f2=bufptr + pos * 
typ_szb;(*((struct _tuple9*)x)).f3=bufptr + sz * typ_szb;env=Cyc_Marshal_table_insert(
env,bufptr);{int y=(int)bufptr;int i=0;for(0;i < sz;i ++){env=Cyc_Marshal_rread_type_base(
r,_tmp150,env,fp,((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{struct
_tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(result);(*((struct _tuple9*)x)).f1=
Cyc_Marshal_table_lookup(env,mem);(*((struct _tuple9*)x)).f2=(*((struct _tuple9*)x)).f1
+ typ_szb * pos;(*((struct _tuple9*)x)).f3=(*((struct _tuple9*)x)).f1 + typ_szb * sz;}
goto _LL46;}_LL51: if(_tmp14C <= (void*)2?1:*((int*)_tmp14C)!= 3)goto _LL53;_tmp151=((
struct Cyc_Typerep_Struct_struct*)_tmp14C)->f3;_LL52: {unsigned int x=((
unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(
_tmp151,sizeof(struct _tuple0*));i ++){unsigned int _tmp169;void*_tmp16A;struct
_tuple0 _tmp168=*((struct _tuple0*)((struct _tuple0**)_tmp151.curr)[i]);_tmp169=
_tmp168.f1;_tmp16A=_tmp168.f3;{int x_ofs=(int)(x + _tmp169);env=Cyc_Marshal_rread_type_base(
r,_tmp16A,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL53: if(
_tmp14C <= (void*)2?1:*((int*)_tmp14C)!= 4)goto _LL55;_tmp152=((struct Cyc_Typerep_Tuple_struct*)
_tmp14C)->f2;_LL54: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{
int i=0;for(0;i < _get_arr_size(_tmp152,sizeof(struct _tuple1*));i ++){unsigned int
_tmp16C;void*_tmp16D;struct _tuple1 _tmp16B=*((struct _tuple1*)((struct _tuple1**)
_tmp152.curr)[i]);_tmp16C=_tmp16B.f1;_tmp16D=_tmp16B.f2;{int x_ofs=(int)(x + 
_tmp16C);env=Cyc_Marshal_rread_type_base(r,_tmp16D,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LL46;}_LL55: if(_tmp14C <= (void*)2?1:*((int*)_tmp14C)
!= 6)goto _LL57;_tmp153=((struct Cyc_Typerep_TUnionField_struct*)_tmp14C)->f4;
_LL56: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;
for(0;i < _get_arr_size(_tmp153,sizeof(struct _tuple1*));i ++){unsigned int _tmp16F;
void*_tmp170;struct _tuple1 _tmp16E=*((struct _tuple1*)((struct _tuple1**)_tmp153.curr)[
i]);_tmp16F=_tmp16E.f1;_tmp170=_tmp16E.f2;{int x_ofs=(int)(x + _tmp16F);env=Cyc_Marshal_rread_type_base(
r,_tmp170,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL57: if(
_tmp14C <= (void*)2?1:*((int*)_tmp14C)!= 5)goto _LL59;_tmp154=((struct Cyc_Typerep_TUnion_struct*)
_tmp14C)->f2;_tmp155=((struct Cyc_Typerep_TUnion_struct*)_tmp14C)->f3;_LL58: {int
tagmem=(int)Cyc_Marshal_cycgetvarint(fp);if(tagmem < 1024){int tag=tagmem;
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*((unsigned int*)
x)=(unsigned int)tag;}else{int mem=tagmem - 1024;if(!Cyc_Marshal_table_member(env,(
unsigned int)mem)){int tag=(int)Cyc_Marshal_cycgetvarint(fp);int _tmp172;void*
_tmp173;struct _tuple4 _tmp171=Cyc_Typerep_get_unionbranch((unsigned int)tag,
_tmp155);_tmp172=_tmp171.f1;_tmp173=_tmp171.f2;{struct _tagged_arr buf=({
unsigned int _tmp174=_tmp172 + Cyc_Typerep_size_type(_tmp173);char*_tmp175=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp174 + 1));struct _tagged_arr _tmp177=
_tag_arr(_tmp175,sizeof(char),_tmp174 + 1);{unsigned int _tmp176=_tmp174;
unsigned int i;for(i=0;i < _tmp176;i ++){_tmp175[i]='\000';}_tmp175[_tmp176]=(char)
0;}_tmp177;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*
x=((unsigned int*(*)(void*x))unsafe_cast)(result);*((unsigned int*)x)=bufptr;{
unsigned int*tagptr=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*((
unsigned int*)tagptr)=(unsigned int)tag;env=Cyc_Marshal_rread_type_base(r,
_tmp173,env,fp,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp172));}}}}
else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*((
unsigned int*)x)=Cyc_Marshal_table_lookup(env,mem);}}goto _LL46;}_LL59: if(_tmp14C
<= (void*)2?1:*((int*)_tmp14C)!= 7)goto _LL5B;_tmp156=((struct Cyc_Typerep_XTUnion_struct*)
_tmp14C)->f2;_LL5A: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){char nonnull=(char)Cyc_Marshal_cycgetchar(fp);struct
_tagged_arr s=({char*_tmp182=({unsigned int _tmp17F=(unsigned int)100;char*_tmp180=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp17F + 1));{unsigned int
_tmp181=_tmp17F;unsigned int i;for(i=0;i < _tmp181;i ++){_tmp180[i]='\000';}_tmp180[
_tmp181]=(char)0;}_tmp180;});_tag_arr(_tmp182,sizeof(char),_get_zero_arr_size(
_tmp182,(unsigned int)100 + 1));});struct _tagged_arr xtname=_tagged_arr_plus(s,
sizeof(char),4);Cyc_Marshal_fscanntstring(fp,xtname);if((int)nonnull){int _tmp179;
void*_tmp17A;struct _tuple4 _tmp178=Cyc_Typerep_get_xtunionbranch((struct
_tagged_arr)xtname,_tmp156);_tmp179=_tmp178.f1;_tmp17A=_tmp178.f2;{struct
_tagged_arr buf=({unsigned int _tmp17B=_tmp179 + Cyc_Typerep_size_type(_tmp17A);
char*_tmp17C=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp17B + 1));
struct _tagged_arr _tmp17E=_tag_arr(_tmp17C,sizeof(char),_tmp17B + 1);{unsigned int
_tmp17D=_tmp17B;unsigned int i;for(i=0;i < _tmp17D;i ++){_tmp17C[i]='\000';}_tmp17C[
_tmp17D]=(char)0;}_tmp17E;});unsigned int bufptr=(unsigned int)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,
bufptr);{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*((
unsigned int*)x)=bufptr;{unsigned int*tagptr=((unsigned int*(*)(unsigned int x))
unsafe_cast)(bufptr);*((unsigned int*)tagptr)=(unsigned int)((char*)_check_null(
_untag_arr(s,sizeof(char),1 + 1)));env=Cyc_Marshal_rread_type_base(r,_tmp17A,env,
fp,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp179));}}}}else{
unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*((unsigned int*)
x)=(unsigned int)((char*)_check_null(_untag_arr(s,sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(
env,*((unsigned int*)x));}}else{unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*((unsigned int*)x)=Cyc_Marshal_table_lookup(env,mem);}goto
_LL46;}_LL5B: if(_tmp14C <= (void*)2?1:*((int*)_tmp14C)!= 8)goto _LL5D;_tmp157=((
struct Cyc_Typerep_Union_struct*)_tmp14C)->f2;_LL5C: env=Cyc_Marshal_rread_type_base(
r,(void*)({struct Cyc_Typerep_Tuple_struct*_tmp183=_cycalloc(sizeof(*_tmp183));
_tmp183[0]=({struct Cyc_Typerep_Tuple_struct _tmp184;_tmp184.tag=4;_tmp184.f1=(
unsigned int)_tmp157;_tmp184.f2=({unsigned int _tmp185=(unsigned int)_tmp157;
struct _tuple1**_tmp186=(struct _tuple1**)_cycalloc(_check_times(sizeof(struct
_tuple1*),_tmp185));struct _tagged_arr _tmp18B=_tag_arr(_tmp186,sizeof(struct
_tuple1*),_tmp185);{unsigned int _tmp187=_tmp185;unsigned int i;for(i=0;i < _tmp187;
i ++){_tmp186[i]=({struct _tuple1*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->f1=
i;_tmp188->f2=(void*)({struct Cyc_Typerep_Int_struct*_tmp189=_cycalloc_atomic(
sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Typerep_Int_struct _tmp18A;_tmp18A.tag=0;
_tmp18A.f1=0;_tmp18A.f2=8;_tmp18A;});_tmp189;});_tmp188;});}}_tmp18B;});_tmp184;});
_tmp183;}),env,fp,result);goto _LL46;_LL5D: if(_tmp14C <= (void*)2?1:*((int*)
_tmp14C)!= 9)goto _LL46;_tmp158=((struct Cyc_Typerep_Enum_struct*)_tmp14C)->f2;
_LL5E: env=Cyc_Marshal_rread_type_base(r,(void*)({struct Cyc_Typerep_Int_struct*
_tmp18C=_cycalloc_atomic(sizeof(*_tmp18C));_tmp18C[0]=({struct Cyc_Typerep_Int_struct
_tmp18D;_tmp18D.tag=0;_tmp18D.f1=0;_tmp18D.f2=(unsigned int)(_tmp158 << 3);
_tmp18D;});_tmp18C;}),env,fp,result);_LL46:;}return env;}void*Cyc_Marshal_rread_type(
struct _RegionHandle*r,void*ts,struct Cyc_Std___cycFILE*fp){struct _handler_cons
_tmp18E;_push_handler(& _tmp18E);{int _tmp190=0;if(setjmp(_tmp18E.handler))_tmp190=
1;if(!_tmp190){{struct _tagged_arr buf=({unsigned int _tmp192=Cyc_Typerep_size_type(
ts);char*_tmp193=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp192 + 1));
struct _tagged_arr _tmp195=_tag_arr(_tmp193,sizeof(char),_tmp192 + 1);{unsigned int
_tmp194=_tmp192;unsigned int i;for(i=0;i < _tmp194;i ++){_tmp193[i]='\000';}_tmp193[
_tmp194]=(char)0;}_tmp195;});void*result=((void*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));Cyc_Marshal_rread_type_base(r,ts,
Cyc_Marshal_empty_addr_table(),fp,result);{void*_tmp191=result;_npop_handler(0);
return _tmp191;}};_pop_handler();}else{void*_tmp18F=(void*)_exn_thrown;void*
_tmp197=_tmp18F;struct _tagged_arr _tmp198;_LL65: if(*((void**)_tmp197)!= Cyc_Core_Failure)
goto _LL67;_tmp198=((struct Cyc_Core_Failure_struct*)_tmp197)->f1;_LL66:({struct
Cyc_Std_String_pa_struct _tmp19B;_tmp19B.tag=0;_tmp19B.f1=(struct _tagged_arr)
_tmp198;{void*_tmp199[1]={& _tmp19B};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp19A="Core::Failure(%s)\n";_tag_arr(_tmp19A,sizeof(char),_get_zero_arr_size(
_tmp19A,19));}),_tag_arr(_tmp199,sizeof(void*),1));}});(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=({
struct Cyc_Core_Failure_struct _tmp19D;_tmp19D.tag=Cyc_Core_Failure;_tmp19D.f1=
_tmp198;_tmp19D;});_tmp19C;}));_LL67:;_LL68:(void)_throw(_tmp197);_LL64:;}}}void*
Cyc_Marshal_read_type(void*ts,struct Cyc_Std___cycFILE*fp){return Cyc_Marshal_rread_type(
Cyc_Core_heap_region,ts,fp);}static struct Cyc_Dict_Dict*Cyc_Marshal_empty_addr_map(){
return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,unsigned int k,unsigned int
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(unsigned int,
unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);}struct Cyc_Dict_Dict*
Cyc_Marshal_rcopy_type_base(struct _RegionHandle*r,void*rep,struct Cyc_Dict_Dict*
env,void*val,void*result){{void*_tmp19E=rep;unsigned int _tmp19F;unsigned int
_tmp1A0;unsigned int _tmp1A1;unsigned int _tmp1A2;unsigned int _tmp1A3;void*_tmp1A4;
void*_tmp1A5;struct _tagged_arr _tmp1A6;struct _tagged_arr _tmp1A7;struct _tagged_arr
_tmp1A8;struct _tagged_arr _tmp1A9;struct _tagged_arr _tmp1AA;struct _tagged_arr
_tmp1AB;int _tmp1AC;int _tmp1AD;_LL6A: if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 0)
goto _LL6C;_tmp19F=((struct Cyc_Typerep_Int_struct*)_tmp19E)->f2;if(_tmp19F != 8)
goto _LL6C;_LL6B: {char*val0=((char*(*)(void*x))unsafe_cast)(val);char*result0=((
char*(*)(void*x))unsafe_cast)(result);({struct _tagged_arr _tmp1AF=({char*_tmp1AE=
result0;_tag_arr(_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,1));});char
_tmp1B0=*((char*)_check_unknown_subscript(_tmp1AF,sizeof(char),0));char _tmp1B1=*((
char*)val0);if(_get_arr_size(_tmp1AF,sizeof(char))== 1?_tmp1B0 == '\000'?_tmp1B1
!= '\000': 0: 0)_throw_arraybounds();*((char*)_tmp1AF.curr)=_tmp1B1;});goto _LL69;}
_LL6C: if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 0)goto _LL6E;_tmp1A0=((struct Cyc_Typerep_Int_struct*)
_tmp19E)->f2;if(_tmp1A0 != 16)goto _LL6E;_LL6D: {short*val0=((short*(*)(void*x))
unsafe_cast)(val);short*result0=((short*(*)(void*x))unsafe_cast)(result);*((
short*)result0)=*((short*)val0);goto _LL69;}_LL6E: if(_tmp19E <= (void*)2?1:*((int*)
_tmp19E)!= 0)goto _LL70;_tmp1A1=((struct Cyc_Typerep_Int_struct*)_tmp19E)->f2;if(
_tmp1A1 != 32)goto _LL70;_LL6F: {int*val0=((int*(*)(void*x))unsafe_cast)(val);int*
result0=((int*(*)(void*x))unsafe_cast)(result);*((int*)result0)=*((int*)val0);
goto _LL69;}_LL70: if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 0)goto _LL72;_tmp1A2=((
struct Cyc_Typerep_Int_struct*)_tmp19E)->f2;if(_tmp1A2 != 64)goto _LL72;_LL71: {
long long*val0=((long long*(*)(void*x))unsafe_cast)(val);long long*result0=((
long long*(*)(void*x))unsafe_cast)(result);*((long long*)result0)=*((long long*)
val0);goto _LL69;}_LL72: if((int)_tmp19E != 0)goto _LL74;_LL73: {float*val0=((float*(*)(
void*x))unsafe_cast)(val);float*result0=((float*(*)(void*x))unsafe_cast)(result);*((
float*)result0)=*((float*)val0);goto _LL69;}_LL74: if((int)_tmp19E != 1)goto _LL76;
_LL75: {double*val0=((double*(*)(void*x))unsafe_cast)(val);double*result0=((
double*(*)(void*x))unsafe_cast)(result);*((double*)result0)=*((double*)val0);
goto _LL69;}_LL76: if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 1)goto _LL78;_tmp1A3=((
struct Cyc_Typerep_ThinPtr_struct*)_tmp19E)->f1;_tmp1A4=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp19E)->f2;_LL77: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(
val);unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);int
addr=(int)*((unsigned int*)val0);if(((int(*)(struct Cyc_Dict_Dict*d,unsigned int k))
Cyc_Dict_member)(env,(unsigned int)addr))*((unsigned int*)result0)=((
unsigned int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup)(env,(
unsigned int)addr);else{int typ_szb=(int)Cyc_Typerep_size_type(_tmp1A4);struct
_tagged_arr buf=({unsigned int _tmp1B2=_tmp1A3 * typ_szb;char*_tmp1B3=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp1B2 + 1));struct _tagged_arr _tmp1B5=
_tag_arr(_tmp1B3,sizeof(char),_tmp1B2 + 1);{unsigned int _tmp1B4=_tmp1B2;
unsigned int i;for(i=0;i < _tmp1B4;i ++){_tmp1B3[i]='\000';}_tmp1B3[_tmp1B4]=(char)
0;}_tmp1B5;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));*((unsigned int*)result0)=bufptr;env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,(
unsigned int)addr,bufptr);{int x=addr;int y=(int)bufptr;int i=0;for(0;i < _tmp1A3;i
++){env=Cyc_Marshal_rcopy_type_base(r,_tmp1A4,env,((void*(*)(int x))unsafe_cast)(
x),((void*(*)(int x))unsafe_cast)(y));x +=typ_szb;y +=typ_szb;}}}goto _LL69;}_LL78:
if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 2)goto _LL7A;_tmp1A5=(void*)((struct
Cyc_Typerep_FatPtr_struct*)_tmp19E)->f1;_LL79: {struct _tuple9*val0=((struct
_tuple9*(*)(void*x))unsafe_cast)(val);struct _tuple9*result0=((struct _tuple9*(*)(
void*x))unsafe_cast)(result);unsigned int _tmp1B7;unsigned int _tmp1B8;
unsigned int _tmp1B9;struct _tuple9 _tmp1B6=*((struct _tuple9*)val0);_tmp1B7=_tmp1B6.f1;
_tmp1B8=_tmp1B6.f2;_tmp1B9=_tmp1B6.f3;{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp1A5);int size=(int)((_tmp1B9 - _tmp1B7)/ typ_szb);int pos=(int)((_tmp1B8 - 
_tmp1B7)/ typ_szb);if(((int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(
env,_tmp1B7)){unsigned int _tmp1BA=((unsigned int(*)(struct Cyc_Dict_Dict*d,
unsigned int k))Cyc_Dict_lookup)(env,_tmp1B7);*((struct _tuple9*)result0)=({struct
_tuple9 _tmp1BB;_tmp1BB.f1=_tmp1BA;_tmp1BB.f2=_tmp1BA + pos * typ_szb;_tmp1BB.f3=
_tmp1BA + size * typ_szb;_tmp1BB;});}else{struct _tagged_arr buf=({unsigned int
_tmp1BD=_tmp1B9 - _tmp1B7;char*_tmp1BE=(char*)_region_malloc(r,_check_times(
sizeof(char),_tmp1BD + 1));struct _tagged_arr _tmp1C0=_tag_arr(_tmp1BE,sizeof(char),
_tmp1BD + 1);{unsigned int _tmp1BF=_tmp1BD;unsigned int i;for(i=0;i < _tmp1BF;i ++){
_tmp1BE[i]='\000';}_tmp1BE[_tmp1BF]=(char)0;}_tmp1C0;});unsigned int bufptr=(
unsigned int)((char*)_check_null(_untag_arr(buf,sizeof(char),1 + 1)));*((struct
_tuple9*)result0)=({struct _tuple9 _tmp1BC;_tmp1BC.f1=bufptr;_tmp1BC.f2=bufptr + 
pos * typ_szb;_tmp1BC.f3=bufptr + size * typ_szb;_tmp1BC;});env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,_tmp1B7,
bufptr);{int y=(int)_tmp1B7;int z=(int)bufptr;int i=0;for(0;i < size;i ++){env=Cyc_Marshal_rcopy_type_base(
r,_tmp1A5,env,((void*(*)(int x))unsafe_cast)(y),((void*(*)(int x))unsafe_cast)(z));
y +=typ_szb;z +=typ_szb;}}}goto _LL69;}}_LL7A: if(_tmp19E <= (void*)2?1:*((int*)
_tmp19E)!= 3)goto _LL7C;_tmp1A6=((struct Cyc_Typerep_Struct_struct*)_tmp19E)->f3;
_LL7B: {int x=((int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*x))
unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmp1A6,sizeof(struct _tuple0*));
i ++){unsigned int _tmp1C2;void*_tmp1C3;struct _tuple0 _tmp1C1=*((struct _tuple0*)((
struct _tuple0**)_tmp1A6.curr)[i]);_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f3;{int
x_ofs=(int)(x + _tmp1C2);int y_ofs=(int)(y + _tmp1C2);env=Cyc_Marshal_rcopy_type_base(
r,_tmp1C3,env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(
y_ofs));}}}goto _LL69;}_LL7C: if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 4)goto
_LL7E;_tmp1A7=((struct Cyc_Typerep_Tuple_struct*)_tmp19E)->f2;_LL7D: {int x=((int(*)(
void*x))unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int i=0;
for(0;i < _get_arr_size(_tmp1A7,sizeof(struct _tuple1*));i ++){unsigned int _tmp1C5;
void*_tmp1C6;struct _tuple1 _tmp1C4=*((struct _tuple1*)((struct _tuple1**)_tmp1A7.curr)[
i]);_tmp1C5=_tmp1C4.f1;_tmp1C6=_tmp1C4.f2;{int x_ofs=(int)(x + _tmp1C5);int y_ofs=(
int)(y + _tmp1C5);env=Cyc_Marshal_rcopy_type_base(r,_tmp1C6,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL7E:
if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 6)goto _LL80;_tmp1A8=((struct Cyc_Typerep_TUnionField_struct*)
_tmp19E)->f4;_LL7F: {int x=((int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*
x))unsafe_cast)(result);{int i=0;for(0;i < _get_arr_size(_tmp1A8,sizeof(struct
_tuple1*));i ++){unsigned int _tmp1C8;void*_tmp1C9;struct _tuple1 _tmp1C7=*((struct
_tuple1*)((struct _tuple1**)_tmp1A8.curr)[i]);_tmp1C8=_tmp1C7.f1;_tmp1C9=_tmp1C7.f2;{
int x_ofs=(int)(x + _tmp1C8);int y_ofs=(int)(y + _tmp1C8);env=Cyc_Marshal_rcopy_type_base(
r,_tmp1C9,env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(
y_ofs));}}}goto _LL69;}_LL80: if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 5)goto
_LL82;_tmp1A9=((struct Cyc_Typerep_TUnion_struct*)_tmp19E)->f2;_tmp1AA=((struct
Cyc_Typerep_TUnion_struct*)_tmp19E)->f3;_LL81: {unsigned int*val0=((unsigned int*(*)(
void*x))unsafe_cast)(val);unsigned int*result0=((unsigned int*(*)(void*x))
unsafe_cast)(result);if(*((unsigned int*)val0)< 1024)*((unsigned int*)result0)=*((
unsigned int*)val0);else{unsigned int addr=*((unsigned int*)val0);if(((int(*)(
struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_member)(env,addr))*((unsigned int*)
result0)=((unsigned int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup)(
env,addr);else{unsigned int*tufp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*((
unsigned int*)val0));unsigned int tag=*((unsigned int*)tufp);int _tmp1CB;void*
_tmp1CC;struct _tuple4 _tmp1CA=Cyc_Typerep_get_unionbranch(tag,_tmp1AA);_tmp1CB=
_tmp1CA.f1;_tmp1CC=_tmp1CA.f2;{struct _tagged_arr buf=({unsigned int _tmp1CD=
_tmp1CB + Cyc_Typerep_size_type(_tmp1CC);char*_tmp1CE=(char*)_region_malloc(r,
_check_times(sizeof(char),_tmp1CD + 1));struct _tagged_arr _tmp1D0=_tag_arr(_tmp1CE,
sizeof(char),_tmp1CD + 1);{unsigned int _tmp1CF=_tmp1CD;unsigned int i;for(i=0;i < 
_tmp1CF;i ++){_tmp1CE[i]='\000';}_tmp1CE[_tmp1CF]=(char)0;}_tmp1D0;});
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(buf,sizeof(char),
1 + 1)));env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,unsigned int k,
unsigned int v))Cyc_Dict_insert)(env,addr,bufptr);*((unsigned int*)result0)=
bufptr;{unsigned int*newtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(
bufptr);*((unsigned int*)newtagp)=tag;env=Cyc_Marshal_rcopy_type_base(r,_tmp1CC,
env,((void*(*)(unsigned int x))unsafe_cast)(addr + _tmp1CB),((void*(*)(
unsigned int x))unsafe_cast)(bufptr + _tmp1CB));}}}}goto _LL69;}_LL82: if(_tmp19E <= (
void*)2?1:*((int*)_tmp19E)!= 7)goto _LL84;_tmp1AB=((struct Cyc_Typerep_XTUnion_struct*)
_tmp19E)->f2;_LL83: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(
val);unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);
unsigned int addr=*((unsigned int*)val0);if(((int(*)(struct Cyc_Dict_Dict*d,
unsigned int k))Cyc_Dict_member)(env,addr))*((unsigned int*)result0)=((
unsigned int(*)(struct Cyc_Dict_Dict*d,unsigned int k))Cyc_Dict_lookup)(env,addr);
else{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*((
unsigned int*)val0));if(*((unsigned int*)xtstructp)!= 0){unsigned int xtnamep=*((
unsigned int*)xtstructp);char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(
xtnamep + 4);struct _tagged_arr xtname=(struct _tagged_arr)Cstring_to_string(xtnamec);
int _tmp1D2;void*_tmp1D3;struct _tuple4 _tmp1D1=Cyc_Typerep_get_xtunionbranch(
xtname,_tmp1AB);_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;{struct _tagged_arr buf=({
unsigned int _tmp1D4=_tmp1D2 + Cyc_Typerep_size_type(_tmp1D3);char*_tmp1D5=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp1D4 + 1));struct _tagged_arr _tmp1D7=
_tag_arr(_tmp1D5,sizeof(char),_tmp1D4 + 1);{unsigned int _tmp1D6=_tmp1D4;
unsigned int i;for(i=0;i < _tmp1D6;i ++){_tmp1D5[i]='\000';}_tmp1D5[_tmp1D6]=(char)
0;}_tmp1D7;});unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_arr(
buf,sizeof(char),1 + 1)));*((unsigned int*)result0)=bufptr;{unsigned int*tagptr=((
unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*((unsigned int*)tagptr)=
xtnamep;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)val0)
+ _tmp1D2);env=Cyc_Marshal_rcopy_type_base(r,_tmp1D3,env,xoff,((void*(*)(
unsigned int x))unsafe_cast)(bufptr + _tmp1D2));env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,addr,
bufptr);}}}}else{*((unsigned int*)result0)=*((unsigned int*)val0);}}goto _LL69;}
_LL84: if(_tmp19E <= (void*)2?1:*((int*)_tmp19E)!= 8)goto _LL86;_tmp1AC=((struct Cyc_Typerep_Union_struct*)
_tmp19E)->f2;_LL85: env=Cyc_Marshal_rcopy_type_base(r,(void*)({struct Cyc_Typerep_Tuple_struct*
_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8[0]=({struct Cyc_Typerep_Tuple_struct
_tmp1D9;_tmp1D9.tag=4;_tmp1D9.f1=(unsigned int)_tmp1AC;_tmp1D9.f2=({unsigned int
_tmp1DA=(unsigned int)_tmp1AC;struct _tuple1**_tmp1DB=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp1DA));struct _tagged_arr _tmp1E0=_tag_arr(
_tmp1DB,sizeof(struct _tuple1*),_tmp1DA);{unsigned int _tmp1DC=_tmp1DA;
unsigned int i;for(i=0;i < _tmp1DC;i ++){_tmp1DB[i]=({struct _tuple1*_tmp1DD=
_cycalloc(sizeof(*_tmp1DD));_tmp1DD->f1=i;_tmp1DD->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp1DE=_cycalloc_atomic(sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_Typerep_Int_struct
_tmp1DF;_tmp1DF.tag=0;_tmp1DF.f1=0;_tmp1DF.f2=8;_tmp1DF;});_tmp1DE;});_tmp1DD;});}}
_tmp1E0;});_tmp1D9;});_tmp1D8;}),env,val,result);goto _LL69;_LL86: if(_tmp19E <= (
void*)2?1:*((int*)_tmp19E)!= 9)goto _LL88;_tmp1AD=((struct Cyc_Typerep_Enum_struct*)
_tmp19E)->f2;_LL87: env=Cyc_Marshal_rcopy_type_base(r,(void*)({struct Cyc_Typerep_Int_struct*
_tmp1E1=_cycalloc_atomic(sizeof(*_tmp1E1));_tmp1E1[0]=({struct Cyc_Typerep_Int_struct
_tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=0;_tmp1E2.f2=(unsigned int)(_tmp1AD << 3);
_tmp1E2;});_tmp1E1;}),env,val,result);goto _LL69;_LL88:;_LL89:(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({
struct Cyc_Core_Failure_struct _tmp1E4;_tmp1E4.tag=Cyc_Core_Failure;_tmp1E4.f1=({
const char*_tmp1E5="Unhandled case in copy_type_base";_tag_arr(_tmp1E5,sizeof(
char),_get_zero_arr_size(_tmp1E5,33));});_tmp1E4;});_tmp1E3;}));_LL69:;}return
env;}void*Cyc_Marshal_rcopy_type(struct _RegionHandle*r,void*ts,void*val){struct
_handler_cons _tmp1E6;_push_handler(& _tmp1E6);{int _tmp1E8=0;if(setjmp(_tmp1E6.handler))
_tmp1E8=1;if(!_tmp1E8){{struct _tagged_arr buf=({unsigned int _tmp1EA=Cyc_Typerep_size_type(
ts);char*_tmp1EB=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp1EA + 1));
struct _tagged_arr _tmp1ED=_tag_arr(_tmp1EB,sizeof(char),_tmp1EA + 1);{unsigned int
_tmp1EC=_tmp1EA;unsigned int i;for(i=0;i < _tmp1EC;i ++){_tmp1EB[i]='\000';}_tmp1EB[
_tmp1EC]=(char)0;}_tmp1ED;});void*result=((void*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_arr(buf,sizeof(char),1 + 1)));Cyc_Marshal_rcopy_type_base(r,ts,
Cyc_Marshal_empty_addr_map(),val,result);{void*_tmp1E9=result;_npop_handler(0);
return _tmp1E9;}};_pop_handler();}else{void*_tmp1E7=(void*)_exn_thrown;void*
_tmp1EF=_tmp1E7;struct _tagged_arr _tmp1F0;_LL8B: if(*((void**)_tmp1EF)!= Cyc_Core_Failure)
goto _LL8D;_tmp1F0=((struct Cyc_Core_Failure_struct*)_tmp1EF)->f1;_LL8C:({struct
Cyc_Std_String_pa_struct _tmp1F3;_tmp1F3.tag=0;_tmp1F3.f1=(struct _tagged_arr)
_tmp1F0;{void*_tmp1F1[1]={& _tmp1F3};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp1F2="Core::Failure(%s)\n";_tag_arr(_tmp1F2,sizeof(char),_get_zero_arr_size(
_tmp1F2,19));}),_tag_arr(_tmp1F1,sizeof(void*),1));}});(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({
struct Cyc_Core_Failure_struct _tmp1F5;_tmp1F5.tag=Cyc_Core_Failure;_tmp1F5.f1=
_tmp1F0;_tmp1F5;});_tmp1F4;}));_LL8D:;_LL8E:(void)_throw(_tmp1EF);_LL8A:;}}}void*
Cyc_Marshal_copy_type(void*ts,void*val){return Cyc_Marshal_rcopy_type(Cyc_Core_heap_region,
ts,val);}void*Cyc_Marshal_order_lex(void*ord1,void*ord2){if(ord1 != (void*)1)
return ord1;else{return ord2;}}struct _tuple10{unsigned int f1;unsigned int f2;};
static int Cyc_Marshal_addrp_cmp(struct _tuple10*addrp1,struct _tuple10*addrp2){if((*((
struct _tuple10*)addrp1)).f1 < (*((struct _tuple10*)addrp2)).f1)return 1;else{if((*((
struct _tuple10*)addrp1)).f1 == (*((struct _tuple10*)addrp2)).f1)return Cyc_Marshal_uint_cmp((*((
struct _tuple10*)addrp1)).f2,(*((struct _tuple10*)addrp2)).f2);else{return - 1;}}}
struct Cyc_Dict_Dict*Cyc_Marshal_empty_eq_dict(){return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple10*,struct _tuple10*)))Cyc_Dict_empty)(Cyc_Marshal_addrp_cmp),({
struct _tuple10*_tmp1F6=_cycalloc_atomic(sizeof(*_tmp1F6));_tmp1F6->f1=0;_tmp1F6->f2=
0;_tmp1F6;}),(void*)1);}struct _tuple11{void*f1;struct Cyc_Dict_Dict*f2;};struct
_tuple11 Cyc_Marshal_return_update(struct _tuple10*pair,struct _tuple11 ordenv){void*
_tmp1F8;struct Cyc_Dict_Dict*_tmp1F9;struct _tuple11 _tmp1F7=ordenv;_tmp1F8=_tmp1F7.f1;
_tmp1F9=_tmp1F7.f2;_tmp1F9=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple10*k,void*v))Cyc_Dict_insert)(_tmp1F9,pair,_tmp1F8);return({struct
_tuple11 _tmp1FA;_tmp1FA.f1=_tmp1F8;_tmp1FA.f2=_tmp1F9;_tmp1FA;});}struct _tuple11
Cyc_Marshal_leq_eq_type(void*rep,struct Cyc_Dict_Dict*env,void*x,void*y){if(x == y)
return({struct _tuple11 _tmp1FB;_tmp1FB.f1=(void*)1;_tmp1FB.f2=env;_tmp1FB;});{
void*_tmp1FC=rep;int _tmp1FD;unsigned int _tmp1FE;int _tmp1FF;unsigned int _tmp200;
int _tmp201;unsigned int _tmp202;int _tmp203;unsigned int _tmp204;int _tmp205;
unsigned int _tmp206;int _tmp207;unsigned int _tmp208;int _tmp209;unsigned int
_tmp20A;int _tmp20B;unsigned int _tmp20C;unsigned int _tmp20D;void*_tmp20E;void*
_tmp20F;struct _tagged_arr _tmp210;struct _tagged_arr _tmp211;struct _tagged_arr
_tmp212;struct _tagged_arr _tmp213;struct _tagged_arr _tmp214;struct _tagged_arr
_tmp215;int _tmp216;struct _tagged_arr _tmp217;int _tmp218;_LL90: if(_tmp1FC <= (void*)
2?1:*((int*)_tmp1FC)!= 0)goto _LL92;_tmp1FD=((struct Cyc_Typerep_Int_struct*)
_tmp1FC)->f1;if(_tmp1FD != 0)goto _LL92;_tmp1FE=((struct Cyc_Typerep_Int_struct*)
_tmp1FC)->f2;if(_tmp1FE != 8)goto _LL92;_LL91: {unsigned char*x0=((unsigned char*(*)(
void*x))unsafe_cast)(x);unsigned char*y0=((unsigned char*(*)(void*x))unsafe_cast)(
y);return({struct _tuple11 _tmp219;_tmp219.f1=*((unsigned char*)x0)== *((
unsigned char*)y0)?(void*)1:(*((unsigned char*)x0)<= *((unsigned char*)y0)?(void*)
2:(void*)0);_tmp219.f2=env;_tmp219;});}_LL92: if(_tmp1FC <= (void*)2?1:*((int*)
_tmp1FC)!= 0)goto _LL94;_tmp1FF=((struct Cyc_Typerep_Int_struct*)_tmp1FC)->f1;if(
_tmp1FF != 1)goto _LL94;_tmp200=((struct Cyc_Typerep_Int_struct*)_tmp1FC)->f2;if(
_tmp200 != 8)goto _LL94;_LL93: {signed char*x0=((signed char*(*)(void*x))
unsafe_cast)(x);signed char*y0=((signed char*(*)(void*x))unsafe_cast)(y);return({
struct _tuple11 _tmp21A;_tmp21A.f1=*((signed char*)x0)== *((signed char*)y0)?(void*)
1:(*((signed char*)x0)<= *((signed char*)y0)?(void*)2:(void*)0);_tmp21A.f2=env;
_tmp21A;});}_LL94: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 0)goto _LL96;_tmp201=((
struct Cyc_Typerep_Int_struct*)_tmp1FC)->f1;if(_tmp201 != 1)goto _LL96;_tmp202=((
struct Cyc_Typerep_Int_struct*)_tmp1FC)->f2;if(_tmp202 != 16)goto _LL96;_LL95: {
short*x0=((short*(*)(void*x))unsafe_cast)(x);short*y0=((short*(*)(void*x))
unsafe_cast)(y);return({struct _tuple11 _tmp21B;_tmp21B.f1=*((short*)x0)== *((
short*)y0)?(void*)1:(*((short*)x0)<= *((short*)y0)?(void*)2:(void*)0);_tmp21B.f2=
env;_tmp21B;});}_LL96: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 0)goto _LL98;
_tmp203=((struct Cyc_Typerep_Int_struct*)_tmp1FC)->f1;if(_tmp203 != 0)goto _LL98;
_tmp204=((struct Cyc_Typerep_Int_struct*)_tmp1FC)->f2;if(_tmp204 != 16)goto _LL98;
_LL97: {unsigned short*x0=((unsigned short*(*)(void*x))unsafe_cast)(x);
unsigned short*y0=((unsigned short*(*)(void*x))unsafe_cast)(y);return({struct
_tuple11 _tmp21C;_tmp21C.f1=*((unsigned short*)x0)== *((unsigned short*)y0)?(void*)
1:(*((unsigned short*)x0)<= *((unsigned short*)y0)?(void*)2:(void*)0);_tmp21C.f2=
env;_tmp21C;});}_LL98: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 0)goto _LL9A;
_tmp205=((struct Cyc_Typerep_Int_struct*)_tmp1FC)->f1;if(_tmp205 != 1)goto _LL9A;
_tmp206=((struct Cyc_Typerep_Int_struct*)_tmp1FC)->f2;if(_tmp206 != 32)goto _LL9A;
_LL99: {int*x0=((int*(*)(void*x))unsafe_cast)(x);int*y0=((int*(*)(void*x))
unsafe_cast)(y);return({struct _tuple11 _tmp21D;_tmp21D.f1=*((int*)x0)== *((int*)
y0)?(void*)1:(*((int*)x0)<= *((int*)y0)?(void*)2:(void*)0);_tmp21D.f2=env;
_tmp21D;});}_LL9A: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 0)goto _LL9C;_tmp207=((
struct Cyc_Typerep_Int_struct*)_tmp1FC)->f1;if(_tmp207 != 0)goto _LL9C;_tmp208=((
struct Cyc_Typerep_Int_struct*)_tmp1FC)->f2;if(_tmp208 != 32)goto _LL9C;_LL9B: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);return({struct _tuple11 _tmp21E;_tmp21E.f1=*((
unsigned int*)x0)== *((unsigned int*)y0)?(void*)1:(*((unsigned int*)x0)<= *((
unsigned int*)y0)?(void*)2:(void*)0);_tmp21E.f2=env;_tmp21E;});}_LL9C: if(_tmp1FC
<= (void*)2?1:*((int*)_tmp1FC)!= 0)goto _LL9E;_tmp209=((struct Cyc_Typerep_Int_struct*)
_tmp1FC)->f1;if(_tmp209 != 1)goto _LL9E;_tmp20A=((struct Cyc_Typerep_Int_struct*)
_tmp1FC)->f2;if(_tmp20A != 64)goto _LL9E;_LL9D: {long long*x0=((long long*(*)(void*
x))unsafe_cast)(x);long long*y0=((long long*(*)(void*x))unsafe_cast)(y);return({
struct _tuple11 _tmp21F;_tmp21F.f1=*((long long*)x0)== *((long long*)y0)?(void*)1:(*((
long long*)x0)<= *((long long*)y0)?(void*)2:(void*)0);_tmp21F.f2=env;_tmp21F;});}
_LL9E: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 0)goto _LLA0;_tmp20B=((struct Cyc_Typerep_Int_struct*)
_tmp1FC)->f1;if(_tmp20B != 0)goto _LLA0;_tmp20C=((struct Cyc_Typerep_Int_struct*)
_tmp1FC)->f2;if(_tmp20C != 64)goto _LLA0;_LL9F: {unsigned long long*x0=((
unsigned long long*(*)(void*x))unsafe_cast)(x);unsigned long long*y0=((
unsigned long long*(*)(void*x))unsafe_cast)(y);return({struct _tuple11 _tmp220;
_tmp220.f1=*((unsigned long long*)x0)== *((unsigned long long*)y0)?(void*)1:(*((
unsigned long long*)x0)<= *((unsigned long long*)y0)?(void*)2:(void*)0);_tmp220.f2=
env;_tmp220;});}_LLA0: if((int)_tmp1FC != 0)goto _LLA2;_LLA1: {float*x0=((float*(*)(
void*x))unsafe_cast)(x);float*y0=((float*(*)(void*x))unsafe_cast)(y);return({
struct _tuple11 _tmp221;_tmp221.f1=*((float*)x0)== *((float*)y0)?(void*)1:(*((
float*)x0)<= *((float*)y0)?(void*)2:(void*)0);_tmp221.f2=env;_tmp221;});}_LLA2:
if((int)_tmp1FC != 1)goto _LLA4;_LLA3: {double*x0=((double*(*)(void*x))unsafe_cast)(
x);double*y0=((double*(*)(void*x))unsafe_cast)(y);return({struct _tuple11 _tmp222;
_tmp222.f1=*((double*)x0)== *((double*)y0)?(void*)1:(*((double*)x0)<= *((double*)
y0)?(void*)2:(void*)0);_tmp222.f2=env;_tmp222;});}_LLA4: if(_tmp1FC <= (void*)2?1:*((
int*)_tmp1FC)!= 1)goto _LLA6;_tmp20D=((struct Cyc_Typerep_ThinPtr_struct*)_tmp1FC)->f1;
_tmp20E=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1FC)->f2;_LLA5: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);unsigned int addrx=*((unsigned int*)x0);
unsigned int addry=*((unsigned int*)y0);struct _tuple10*_tmp223=({struct _tuple10*
_tmp22B=_cycalloc_atomic(sizeof(*_tmp22B));_tmp22B->f1=addrx;_tmp22B->f2=addry;
_tmp22B;});if(((int(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp223))return({struct _tuple11 _tmp224;_tmp224.f1=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp223);_tmp224.f2=env;_tmp224;});
else{if(addrx == 0)return({struct _tuple11 _tmp225;_tmp225.f1=(void*)2;_tmp225.f2=
env;_tmp225;});else{if(addry == 0)return({struct _tuple11 _tmp226;_tmp226.f1=(void*)
0;_tmp226.f2=env;_tmp226;});else{env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp223,(void*)1);{int typ_szb=(
int)Cyc_Typerep_size_type(_tmp20E);int xa=(int)addrx;int ya=(int)addry;void*ord=(
void*)1;{int i=0;for(0;i < _tmp20D;i ++){void*_tmp228;struct Cyc_Dict_Dict*_tmp229;
struct _tuple11 _tmp227=Cyc_Marshal_leq_eq_type(_tmp20E,env,((void*(*)(int x))
unsafe_cast)(xa),((void*(*)(int x))unsafe_cast)(ya));_tmp228=_tmp227.f1;_tmp229=
_tmp227.f2;ord=Cyc_Marshal_order_lex(ord,_tmp228);env=_tmp229;xa +=typ_szb;ya +=
typ_szb;}}return Cyc_Marshal_return_update(_tmp223,({struct _tuple11 _tmp22A;
_tmp22A.f1=ord;_tmp22A.f2=env;_tmp22A;}));}}}}}_LLA6: if(_tmp1FC <= (void*)2?1:*((
int*)_tmp1FC)!= 2)goto _LLA8;_tmp20F=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp1FC)->f1;_LLA7: {struct _tuple9*x0=((struct _tuple9*(*)(void*x))unsafe_cast)(x);
struct _tuple9*y0=((struct _tuple9*(*)(void*x))unsafe_cast)(y);unsigned int _tmp22D;
unsigned int _tmp22E;unsigned int _tmp22F;struct _tuple9 _tmp22C=*((struct _tuple9*)
x0);_tmp22D=_tmp22C.f1;_tmp22E=_tmp22C.f2;_tmp22F=_tmp22C.f3;{unsigned int
_tmp231;unsigned int _tmp232;unsigned int _tmp233;struct _tuple9 _tmp230=*((struct
_tuple9*)y0);_tmp231=_tmp230.f1;_tmp232=_tmp230.f2;_tmp233=_tmp230.f3;{int
typ_szb=(int)Cyc_Typerep_size_type(_tmp20F);int xpos=(int)((_tmp22E - _tmp22D)/ 
typ_szb);int ypos=(int)((_tmp232 - _tmp231)/ typ_szb);if(xpos > ypos)return({struct
_tuple11 _tmp234;_tmp234.f1=(void*)0;_tmp234.f2=env;_tmp234;});{int xsize=(int)((
_tmp22F - _tmp22D)/ typ_szb);int ysize=(int)((_tmp233 - _tmp231)/ typ_szb);if(xsize
> ysize)return({struct _tuple11 _tmp235;_tmp235.f1=(void*)0;_tmp235.f2=env;_tmp235;});{
struct _tuple10*_tmp236=({struct _tuple10*_tmp23E=_cycalloc_atomic(sizeof(*_tmp23E));
_tmp23E->f1=_tmp22D;_tmp23E->f2=_tmp231;_tmp23E;});if(((int(*)(struct Cyc_Dict_Dict*
d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp236))return({struct _tuple11 _tmp237;
_tmp237.f1=((void*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_lookup)(
env,_tmp236);_tmp237.f2=env;_tmp237;});else{if(_tmp22D == 0)return({struct
_tuple11 _tmp238;_tmp238.f1=(void*)2;_tmp238.f2=env;_tmp238;});else{if(_tmp231 == 
0)return({struct _tuple11 _tmp239;_tmp239.f1=(void*)0;_tmp239.f2=env;_tmp239;});
else{env=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))
Cyc_Dict_insert)(env,_tmp236,(void*)1);{int xb=(int)_tmp22D;int yb=(int)_tmp231;
void*ord=(void*)1;{int i=0;for(0;i < xsize;i ++){void*_tmp23B;struct Cyc_Dict_Dict*
_tmp23C;struct _tuple11 _tmp23A=Cyc_Marshal_leq_eq_type(_tmp20F,env,((void*(*)(int
x))unsafe_cast)(xb),((void*(*)(int x))unsafe_cast)(yb));_tmp23B=_tmp23A.f1;
_tmp23C=_tmp23A.f2;ord=Cyc_Marshal_order_lex(ord,_tmp23B);env=_tmp23C;xb +=
typ_szb;yb +=typ_szb;}}if(ord == (void*)1?xsize < ysize: 0)ord=(void*)2;return Cyc_Marshal_return_update(
_tmp236,({struct _tuple11 _tmp23D;_tmp23D.f1=ord;_tmp23D.f2=env;_tmp23D;}));}}}}}}}}}
_LLA8: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 4)goto _LLAA;_tmp210=((struct Cyc_Typerep_Tuple_struct*)
_tmp1FC)->f2;_LLA9: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=0;for(0;i < _get_arr_size(_tmp210,
sizeof(struct _tuple1*));i ++){unsigned int _tmp240;void*_tmp241;struct _tuple1
_tmp23F=*((struct _tuple1*)((struct _tuple1**)_tmp210.curr)[i]);_tmp240=_tmp23F.f1;
_tmp241=_tmp23F.f2;{int x_ofs=(int)(x0 + _tmp240);int y_ofs=(int)(y0 + _tmp240);void*
_tmp243;struct Cyc_Dict_Dict*_tmp244;struct _tuple11 _tmp242=Cyc_Marshal_leq_eq_type(
_tmp241,env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(
y_ofs));_tmp243=_tmp242.f1;_tmp244=_tmp242.f2;ord=Cyc_Marshal_order_lex(ord,
_tmp243);env=_tmp244;}}}return({struct _tuple11 _tmp245;_tmp245.f1=ord;_tmp245.f2=
env;_tmp245;});}_LLAA: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 3)goto _LLAC;
_tmp211=((struct Cyc_Typerep_Struct_struct*)_tmp1FC)->f3;_LLAB: {int x0=((int(*)(
void*x))unsafe_cast)(x);int y0=((int(*)(void*x))unsafe_cast)(y);void*ord=(void*)1;{
int i=0;for(0;i < _get_arr_size(_tmp211,sizeof(struct _tuple0*));i ++){unsigned int
_tmp247;void*_tmp248;struct _tuple0 _tmp246=*((struct _tuple0*)((struct _tuple0**)
_tmp211.curr)[i]);_tmp247=_tmp246.f1;_tmp248=_tmp246.f3;{int x_ofs=(int)(x0 + 
_tmp247);int y_ofs=(int)(y0 + _tmp247);void*_tmp24A;struct Cyc_Dict_Dict*_tmp24B;
struct _tuple11 _tmp249=Cyc_Marshal_leq_eq_type(_tmp248,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp24A=_tmp249.f1;
_tmp24B=_tmp249.f2;ord=Cyc_Marshal_order_lex(ord,_tmp24A);env=_tmp24B;}}}return({
struct _tuple11 _tmp24C;_tmp24C.f1=ord;_tmp24C.f2=env;_tmp24C;});}_LLAC: if(_tmp1FC
<= (void*)2?1:*((int*)_tmp1FC)!= 6)goto _LLAE;_tmp212=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1FC)->f4;_LLAD: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=1;for(0;i < _get_arr_size(_tmp212,
sizeof(struct _tuple1*));i ++){unsigned int _tmp24E;void*_tmp24F;struct _tuple1
_tmp24D=*((struct _tuple1*)((struct _tuple1**)_tmp212.curr)[i]);_tmp24E=_tmp24D.f1;
_tmp24F=_tmp24D.f2;{int x_ofs=(int)(x0 + _tmp24E);int y_ofs=(int)(y0 + _tmp24E);void*
_tmp251;struct Cyc_Dict_Dict*_tmp252;struct _tuple11 _tmp250=Cyc_Marshal_leq_eq_type(
_tmp24F,env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(
y_ofs));_tmp251=_tmp250.f1;_tmp252=_tmp250.f2;ord=Cyc_Marshal_order_lex(ord,
_tmp251);env=_tmp252;}}}return({struct _tuple11 _tmp253;_tmp253.f1=ord;_tmp253.f2=
env;_tmp253;});}_LLAE: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 5)goto _LLB0;
_tmp213=((struct Cyc_Typerep_TUnion_struct*)_tmp1FC)->f2;_tmp214=((struct Cyc_Typerep_TUnion_struct*)
_tmp1FC)->f3;_LLAF: {unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);
unsigned int*y0=((unsigned int*(*)(void*x))unsafe_cast)(y);if(*((unsigned int*)
x0)< 1024?*((unsigned int*)y0)< 1024: 0)return({struct _tuple11 _tmp254;_tmp254.f1=*((
unsigned int*)x0)== *((unsigned int*)y0)?(void*)1:(*((unsigned int*)x0)<= *((
unsigned int*)y0)?(void*)2:(void*)0);_tmp254.f2=env;_tmp254;});else{if(*((
unsigned int*)x0)< 1024)return({struct _tuple11 _tmp255;_tmp255.f1=(void*)2;
_tmp255.f2=env;_tmp255;});else{if(*((unsigned int*)y0)< 1024)return({struct
_tuple11 _tmp256;_tmp256.f1=(void*)0;_tmp256.f2=env;_tmp256;});else{struct
_tuple10*_tmp257=({struct _tuple10*_tmp25D=_cycalloc_atomic(sizeof(*_tmp25D));
_tmp25D->f1=*((unsigned int*)x0);_tmp25D->f2=*((unsigned int*)y0);_tmp25D;});if(((
int(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp257))
return({struct _tuple11 _tmp258;_tmp258.f1=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tuple10*k))Cyc_Dict_lookup)(env,_tmp257);_tmp258.f2=env;_tmp258;});else{env=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(
env,_tmp257,(void*)1);{unsigned int*xtagp=((unsigned int*(*)(unsigned int x))
unsafe_cast)(*((unsigned int*)x0));unsigned int*ytagp=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)y0));if(*((unsigned int*)xtagp)!= *((
unsigned int*)ytagp))return Cyc_Marshal_return_update(_tmp257,({struct _tuple11
_tmp259;_tmp259.f1=*((unsigned int*)xtagp)== *((unsigned int*)ytagp)?(void*)1:(*((
unsigned int*)xtagp)<= *((unsigned int*)ytagp)?(void*)2:(void*)0);_tmp259.f2=env;
_tmp259;}));else{int _tmp25B;void*_tmp25C;struct _tuple4 _tmp25A=Cyc_Typerep_get_unionbranch(*((
unsigned int*)xtagp),_tmp214);_tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;{void*xoff=((
void*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)x0)+ _tmp25B);void*yoff=((
void*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)y0)+ _tmp25B);return Cyc_Marshal_return_update(
_tmp257,Cyc_Marshal_leq_eq_type(_tmp25C,env,xoff,yoff));}}}}}}}}_LLB0: if(_tmp1FC
<= (void*)2?1:*((int*)_tmp1FC)!= 7)goto _LLB2;_tmp215=((struct Cyc_Typerep_XTUnion_struct*)
_tmp1FC)->f2;_LLB1: {unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);
unsigned int*y0=((unsigned int*(*)(void*x))unsafe_cast)(y);struct _tuple10*
_tmp25E=({struct _tuple10*_tmp26B=_cycalloc_atomic(sizeof(*_tmp26B));_tmp26B->f1=*((
unsigned int*)x0);_tmp26B->f2=*((unsigned int*)y0);_tmp26B;});if(((int(*)(struct
Cyc_Dict_Dict*d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp25E))return({struct
_tuple11 _tmp25F;_tmp25F.f1=((void*(*)(struct Cyc_Dict_Dict*d,struct _tuple10*k))
Cyc_Dict_lookup)(env,_tmp25E);_tmp25F.f2=env;_tmp25F;});else{env=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,({struct
_tuple10*_tmp260=_cycalloc_atomic(sizeof(*_tmp260));_tmp260->f1=*((unsigned int*)
x0);_tmp260->f2=*((unsigned int*)y0);_tmp260;}),(void*)1);{unsigned int*xp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)x0));unsigned int*
yp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*((unsigned int*)y0));if(*((
unsigned int*)xp)== 0?*((unsigned int*)yp)== 0: 0){char*xnamec=((char*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)x0)+ 4);char*ynamec=((char*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)y0)+ 4);struct _tagged_arr xname=(
struct _tagged_arr)Cstring_to_string(xnamec);struct _tagged_arr yname=(struct
_tagged_arr)Cstring_to_string(ynamec);int cmp=Cyc_Std_strcmp((struct _tagged_arr)
xname,(struct _tagged_arr)yname);if(cmp < 0)return Cyc_Marshal_return_update(
_tmp25E,({struct _tuple11 _tmp261;_tmp261.f1=(void*)0;_tmp261.f2=env;_tmp261;}));
else{if(cmp == 0)return Cyc_Marshal_return_update(_tmp25E,({struct _tuple11 _tmp262;
_tmp262.f1=(void*)1;_tmp262.f2=env;_tmp262;}));else{return Cyc_Marshal_return_update(
_tmp25E,({struct _tuple11 _tmp263;_tmp263.f1=(void*)2;_tmp263.f2=env;_tmp263;}));}}}
else{if(*((unsigned int*)xp)== 0)return Cyc_Marshal_return_update(_tmp25E,({
struct _tuple11 _tmp264;_tmp264.f1=(void*)2;_tmp264.f2=env;_tmp264;}));else{if(*((
unsigned int*)yp)== 0)return Cyc_Marshal_return_update(_tmp25E,({struct _tuple11
_tmp265;_tmp265.f1=(void*)0;_tmp265.f2=env;_tmp265;}));else{char*xnamec=((char*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)xp)+ 4);char*ynamec=((char*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)yp)+ 4);struct _tagged_arr xname=(
struct _tagged_arr)Cstring_to_string(xnamec);struct _tagged_arr yname=(struct
_tagged_arr)Cstring_to_string(ynamec);int cmp=Cyc_Std_strcmp((struct _tagged_arr)
xname,(struct _tagged_arr)yname);if(cmp < 0)return Cyc_Marshal_return_update(
_tmp25E,({struct _tuple11 _tmp266;_tmp266.f1=(void*)0;_tmp266.f2=env;_tmp266;}));
else{if(cmp == 0){int _tmp268;void*_tmp269;struct _tuple4 _tmp267=Cyc_Typerep_get_xtunionbranch(
xname,_tmp215);_tmp268=_tmp267.f1;_tmp269=_tmp267.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)x0)+ _tmp268);void*yoff=((void*(*)(
unsigned int x))unsafe_cast)(*((unsigned int*)y0)+ _tmp268);return Cyc_Marshal_return_update(
_tmp25E,Cyc_Marshal_leq_eq_type(_tmp269,env,xoff,yoff));}}else{return Cyc_Marshal_return_update(
_tmp25E,({struct _tuple11 _tmp26A;_tmp26A.f1=(void*)2;_tmp26A.f2=env;_tmp26A;}));}}}}}}}}
_LLB2: if(_tmp1FC <= (void*)2?1:*((int*)_tmp1FC)!= 8)goto _LLB4;_tmp216=((struct Cyc_Typerep_Union_struct*)
_tmp1FC)->f2;_tmp217=((struct Cyc_Typerep_Union_struct*)_tmp1FC)->f3;_LLB3: return
Cyc_Marshal_leq_eq_type((void*)({struct Cyc_Typerep_Tuple_struct*_tmp26C=
_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=({struct Cyc_Typerep_Tuple_struct _tmp26D;
_tmp26D.tag=4;_tmp26D.f1=(unsigned int)_tmp216;_tmp26D.f2=({unsigned int _tmp26E=(
unsigned int)_tmp216;struct _tuple1**_tmp26F=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp26E));struct _tagged_arr _tmp274=_tag_arr(
_tmp26F,sizeof(struct _tuple1*),_tmp26E);{unsigned int _tmp270=_tmp26E;
unsigned int i;for(i=0;i < _tmp270;i ++){_tmp26F[i]=({struct _tuple1*_tmp271=
_cycalloc(sizeof(*_tmp271));_tmp271->f1=i;_tmp271->f2=(void*)({struct Cyc_Typerep_Int_struct*
_tmp272=_cycalloc_atomic(sizeof(*_tmp272));_tmp272[0]=({struct Cyc_Typerep_Int_struct
_tmp273;_tmp273.tag=0;_tmp273.f1=0;_tmp273.f2=8;_tmp273;});_tmp272;});_tmp271;});}}
_tmp274;});_tmp26D;});_tmp26C;}),env,x,y);_LLB4: if(_tmp1FC <= (void*)2?1:*((int*)
_tmp1FC)!= 9)goto _LLB6;_tmp218=((struct Cyc_Typerep_Enum_struct*)_tmp1FC)->f2;
_LLB5: return Cyc_Marshal_leq_eq_type((void*)({struct Cyc_Typerep_Int_struct*
_tmp275=_cycalloc_atomic(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Typerep_Int_struct
_tmp276;_tmp276.tag=0;_tmp276.f1=0;_tmp276.f2=(unsigned int)(_tmp218 << 3);
_tmp276;});_tmp275;}),env,x,y);_LLB6:;_LLB7:(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277[0]=({struct Cyc_Core_Failure_struct
_tmp278;_tmp278.tag=Cyc_Core_Failure;_tmp278.f1=({const char*_tmp279="leq_eq_type case unmatched";
_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,27));});_tmp278;});
_tmp277;}));_LL8F:;}}int Cyc_Marshal_eq_type(void*ts,void*x,void*y){struct
_handler_cons _tmp27A;_push_handler(& _tmp27A);{int _tmp27C=0;if(setjmp(_tmp27A.handler))
_tmp27C=1;if(!_tmp27C){{void*_tmp27E;struct _tuple11 _tmp27D=Cyc_Marshal_leq_eq_type(
ts,Cyc_Marshal_empty_eq_dict(),x,y);_tmp27E=_tmp27D.f1;{int _tmp27F=_tmp27E == (
void*)1;_npop_handler(0);return _tmp27F;}};_pop_handler();}else{void*_tmp27B=(
void*)_exn_thrown;void*_tmp281=_tmp27B;struct _tagged_arr _tmp282;_LLB9: if(*((void**)
_tmp281)!= Cyc_Core_Failure)goto _LLBB;_tmp282=((struct Cyc_Core_Failure_struct*)
_tmp281)->f1;_LLBA:({struct Cyc_Std_String_pa_struct _tmp285;_tmp285.tag=0;_tmp285.f1=(
struct _tagged_arr)_tmp282;{void*_tmp283[1]={& _tmp285};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp284="Core::Failure(%s)\n";_tag_arr(_tmp284,sizeof(char),
_get_zero_arr_size(_tmp284,19));}),_tag_arr(_tmp283,sizeof(void*),1));}});(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp286=_cycalloc(sizeof(*_tmp286));
_tmp286[0]=({struct Cyc_Core_Failure_struct _tmp287;_tmp287.tag=Cyc_Core_Failure;
_tmp287.f1=_tmp282;_tmp287;});_tmp286;}));_LLBB:;_LLBC:(void)_throw(_tmp281);
_LLB8:;}}}int Cyc_Marshal_leq_type(void*ts,void*x,void*y){struct _handler_cons
_tmp288;_push_handler(& _tmp288);{int _tmp28A=0;if(setjmp(_tmp288.handler))_tmp28A=
1;if(!_tmp28A){{void*_tmp28C;struct _tuple11 _tmp28B=Cyc_Marshal_leq_eq_type(ts,
Cyc_Marshal_empty_eq_dict(),x,y);_tmp28C=_tmp28B.f1;{int _tmp28D=_tmp28C != (void*)
0;_npop_handler(0);return _tmp28D;}};_pop_handler();}else{void*_tmp289=(void*)
_exn_thrown;void*_tmp28F=_tmp289;struct _tagged_arr _tmp290;_LLBE: if(*((void**)
_tmp28F)!= Cyc_Core_Failure)goto _LLC0;_tmp290=((struct Cyc_Core_Failure_struct*)
_tmp28F)->f1;_LLBF:({struct Cyc_Std_String_pa_struct _tmp293;_tmp293.tag=0;_tmp293.f1=(
struct _tagged_arr)_tmp290;{void*_tmp291[1]={& _tmp293};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp292="Core::Failure(%s)\n";_tag_arr(_tmp292,sizeof(char),
_get_zero_arr_size(_tmp292,19));}),_tag_arr(_tmp291,sizeof(void*),1));}});(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp294=_cycalloc(sizeof(*_tmp294));
_tmp294[0]=({struct Cyc_Core_Failure_struct _tmp295;_tmp295.tag=Cyc_Core_Failure;
_tmp295.f1=_tmp290;_tmp295;});_tmp294;}));_LLC0:;_LLC1:(void)_throw(_tmp28F);
_LLBD:;}}}int Cyc_Marshal_cmp_type(void*ts,void*x,void*y){struct _handler_cons
_tmp296;_push_handler(& _tmp296);{int _tmp298=0;if(setjmp(_tmp296.handler))_tmp298=
1;if(!_tmp298){{void*_tmp29A;struct _tuple11 _tmp299=Cyc_Marshal_leq_eq_type(ts,
Cyc_Marshal_empty_eq_dict(),x,y);_tmp29A=_tmp299.f1;{void*_tmp29B=_tmp29A;_LLC3:
if((int)_tmp29B != 0)goto _LLC5;_LLC4: {int _tmp29C=- 1;_npop_handler(0);return
_tmp29C;}_LLC5: if((int)_tmp29B != 1)goto _LLC7;_LLC6: {int _tmp29D=0;_npop_handler(
0);return _tmp29D;}_LLC7: if((int)_tmp29B != 2)goto _LLC2;_LLC8: {int _tmp29E=1;
_npop_handler(0);return _tmp29E;}_LLC2:;}};_pop_handler();}else{void*_tmp297=(
void*)_exn_thrown;void*_tmp2A0=_tmp297;struct _tagged_arr _tmp2A1;_LLCA: if(*((void**)
_tmp2A0)!= Cyc_Core_Failure)goto _LLCC;_tmp2A1=((struct Cyc_Core_Failure_struct*)
_tmp2A0)->f1;_LLCB:({struct Cyc_Std_String_pa_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(
struct _tagged_arr)_tmp2A1;{void*_tmp2A2[1]={& _tmp2A4};Cyc_Std_fprintf(Cyc_Std_stderr,({
const char*_tmp2A3="Core::Failure(%s)\n";_tag_arr(_tmp2A3,sizeof(char),
_get_zero_arr_size(_tmp2A3,19));}),_tag_arr(_tmp2A2,sizeof(void*),1));}});(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));
_tmp2A5[0]=({struct Cyc_Core_Failure_struct _tmp2A6;_tmp2A6.tag=Cyc_Core_Failure;
_tmp2A6.f1=_tmp2A1;_tmp2A6;});_tmp2A5;}));_LLCC:;_LLCD:(void)_throw(_tmp2A0);
_LLC9:;}}}
