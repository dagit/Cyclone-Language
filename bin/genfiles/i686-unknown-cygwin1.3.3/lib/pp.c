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
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
void*Cyc_Core_identity(void*);int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;}
;int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct
Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;
unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];unsigned int Cyc_strlen(struct _tagged_arr s);int Cyc_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_create(
int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct
Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*
t,void*key);int Cyc_Hashtable_hash_stringptr(struct _tagged_arr*p);struct Cyc_Fn_Function{
void*(*f)(void*,void*);void*env;};struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(
void*,void*),void*x);struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*f)(void*));void*
Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);extern int Cyc_PP_tex_output;struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct
Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct _tuple0{struct _tagged_arr f1;struct Cyc_List_List*
f2;};struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_oline_doc();struct Cyc_PP_Doc*Cyc_PP_text(struct
_tagged_arr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct _tagged_arr*p);struct Cyc_PP_Doc*
Cyc_PP_text_width(struct _tagged_arr s,int w);struct Cyc_PP_Doc*Cyc_PP_hyperlink(
struct _tagged_arr shrt,struct _tagged_arr full);struct Cyc_PP_Doc*Cyc_PP_nest(int k,
struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr);struct Cyc_PP_Doc*
Cyc_PP_cats(struct Cyc_List_List*doclist);struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct
_tagged_arr docs);struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*
d2);struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct
Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*
Cyc_PP_seql(struct _tagged_arr sep,struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(
struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct
Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr stop,struct
_tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_groupl(struct
_tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _tagged_arr start,struct _tagged_arr stop,
struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct
_tagged_arr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,
int,void*);struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);void Cyc_Xarray_add(
struct Cyc_Xarray_Xarray*,void*);struct Cyc_PP_Single_struct{int tag;void*f1;};
struct Cyc_PP_Append_struct{int tag;void*f1;void*f2;};void*Cyc_PP_append(void*a1,
void*a2){if(a1 == (void*)0)return a2;else{if(a2 == (void*)0)return a1;else{return(
void*)({struct Cyc_PP_Append_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_PP_Append_struct _tmp1;_tmp1.tag=1;_tmp1.f1=(void*)a1;_tmp1.f2=(void*)
a2;_tmp1;});_tmp0;});}}}struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct
Cyc_List_List*l){void*_tmp2=y;void*_tmp3;void*_tmp4;void*_tmp5;_LL1: if((int)
_tmp2 != 0)goto _LL3;_LL2: return l;_LL3: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 0)
goto _LL5;_tmp3=(void*)((struct Cyc_PP_Single_struct*)_tmp2)->f1;_LL4: return({
struct Cyc_List_List*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=(void*)_tmp3;_tmp6->tl=
l;_tmp6;});_LL5: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 1)goto _LL0;_tmp4=(void*)((
struct Cyc_PP_Append_struct*)_tmp2)->f1;_tmp5=(void*)((struct Cyc_PP_Append_struct*)
_tmp2)->f2;_LL6: return Cyc_PP_list_of_alist_f(_tmp4,Cyc_PP_list_of_alist_f(_tmp5,
l));_LL0:;}struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){return Cyc_PP_list_of_alist_f(
x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int
newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*
f;};static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){struct Cyc_Xarray_Xarray*
_tmp7=Cyc_Xarray_create(16,al);Cyc_Xarray_add(_tmp7,al);{int last=0;while(last >= 
0){void*_tmp8=Cyc_Xarray_get(_tmp7,last);struct _tagged_arr*_tmp9;void*_tmpA;void*
_tmpB;_LL8: if((int)_tmp8 != 0)goto _LLA;_LL9: -- last;goto _LL7;_LLA: if(_tmp8 <= (void*)
1?1:*((int*)_tmp8)!= 0)goto _LLC;_tmp9=(struct _tagged_arr*)((struct Cyc_PP_Single_struct*)
_tmp8)->f1;_LLB: -- last;({struct Cyc_String_pa_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_tmp9);{void*_tmpC[1]={& _tmpE};Cyc_fprintf(
f,({const char*_tmpD="%s";_tag_arr(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,3));}),
_tag_arr(_tmpC,sizeof(void*),1));}});goto _LL7;_LLC: if(_tmp8 <= (void*)1?1:*((int*)
_tmp8)!= 1)goto _LL7;_tmpA=(void*)((struct Cyc_PP_Append_struct*)_tmp8)->f1;_tmpB=(
void*)((struct Cyc_PP_Append_struct*)_tmp8)->f2;_LLD: Cyc_Xarray_set(_tmp7,last,
_tmpB);if(last == Cyc_Xarray_length(_tmp7)- 1)Cyc_Xarray_add(_tmp7,_tmpA);else{
Cyc_Xarray_set(_tmp7,last + 1,_tmpA);}++ last;goto _LL7;_LL7:;}}}void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(
struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,({struct Cyc_PP_Ppstate*
_tmpF=_cycalloc_atomic(sizeof(*_tmpF));_tmpF->ci=0;_tmpF->cc=0;_tmpF->cl=1;_tmpF->pw=
w;_tmpF->epw=w;_tmpF;}));Cyc_PP_dump_out(f,(void*)o->ppout);}struct _tagged_arr
Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(
struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,({struct Cyc_PP_Ppstate*
_tmp11=_cycalloc_atomic(sizeof(*_tmp11));_tmp11->ci=0;_tmp11->cc=0;_tmp11->cl=1;
_tmp11->pw=w;_tmp11->epw=w;_tmp11;}));return(struct _tagged_arr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)((void*)o->ppout),({const
char*_tmp10="";_tag_arr(_tmp10,sizeof(char),_get_zero_arr_size(_tmp10,1));}));}
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Out*
o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(
d->f,({struct Cyc_PP_Ppstate*_tmp14=_cycalloc_atomic(sizeof(*_tmp14));_tmp14->ci=
0;_tmp14->cc=0;_tmp14->cl=1;_tmp14->pw=w;_tmp14->epw=w;_tmp14;}));return({struct
_tuple0*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->f1=(struct _tagged_arr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)((void*)o->ppout),({const
char*_tmp13="";_tag_arr(_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,1));}));
_tmp12->f2=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)((void*)o->links);
_tmp12;});}static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;int Cyc_PP_tex_output=0;
struct _tagged_arr Cyc_PP_nlblanks(int i){if(Cyc_PP_bhashtbl == 0)Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->v=((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(
int(*)(int))Cyc_Core_identity);_tmp15;});if(i < 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Core_Failure_struct
_tmp17;_tmp17.tag=Cyc_Core_Failure;_tmp17.f1=({const char*_tmp18="nlblanks";
_tag_arr(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,9));});_tmp17;});_tmp16;}));{
struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))
_tmp1B=1;if(!_tmp1B){{struct _tagged_arr _tmp1C=*((struct _tagged_arr*)((struct
_tagged_arr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((
struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,
i));_npop_handler(0);return _tmp1C;};_pop_handler();}else{void*_tmp1A=(void*)
_exn_thrown;void*_tmp1E=_tmp1A;_LLF: if(_tmp1E != Cyc_Core_Not_found)goto _LL11;
_LL10: if(!Cyc_PP_tex_output){int _tmp1F=i / 8;int _tmp20=i % 8;int _tmp21=(2 + _tmp1F)
+ _tmp20;struct _tagged_arr nlb=Cyc_Core_new_string((unsigned int)_tmp21);({struct
_tagged_arr _tmp22=_tagged_arr_plus(nlb,sizeof(char),0);char _tmp23=*((char*)
_check_unknown_subscript(_tmp22,sizeof(char),0));char _tmp24='\n';if(
_get_arr_size(_tmp22,sizeof(char))== 1?_tmp23 == '\000'?_tmp24 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});{int j=0;for(0;j < _tmp1F;j ++){({
struct _tagged_arr _tmp25=_tagged_arr_plus(nlb,sizeof(char),j + 1);char _tmp26=*((
char*)_check_unknown_subscript(_tmp25,sizeof(char),0));char _tmp27='\t';if(
_get_arr_size(_tmp25,sizeof(char))== 1?_tmp26 == '\000'?_tmp27 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});}}{int j=0;for(0;j < _tmp20;j
++){({struct _tagged_arr _tmp28=_tagged_arr_plus(nlb,sizeof(char),(j + 1)+ _tmp1F);
char _tmp29=*((char*)_check_unknown_subscript(_tmp28,sizeof(char),0));char _tmp2A=' ';
if(_get_arr_size(_tmp28,sizeof(char))== 1?_tmp29 == '\000'?_tmp2A != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});}}((void(*)(struct Cyc_Hashtable_Table*
t,int key,struct _tagged_arr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,({struct _tagged_arr*
_tmp2B=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp2B[0]=(struct _tagged_arr)nlb;
_tmp2B;}));return(struct _tagged_arr)nlb;}else{int _tmp2C=3 + i;struct _tagged_arr
nlb=Cyc_Core_new_string((unsigned int)(_tmp2C + 1));({struct _tagged_arr _tmp2D=
_tagged_arr_plus(nlb,sizeof(char),0);char _tmp2E=*((char*)
_check_unknown_subscript(_tmp2D,sizeof(char),0));char _tmp2F='\\';if(
_get_arr_size(_tmp2D,sizeof(char))== 1?_tmp2E == '\000'?_tmp2F != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2D.curr)=_tmp2F;});({struct _tagged_arr _tmp30=
_tagged_arr_plus(nlb,sizeof(char),1);char _tmp31=*((char*)
_check_unknown_subscript(_tmp30,sizeof(char),0));char _tmp32='\\';if(
_get_arr_size(_tmp30,sizeof(char))== 1?_tmp31 == '\000'?_tmp32 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp30.curr)=_tmp32;});({struct _tagged_arr _tmp33=
_tagged_arr_plus(nlb,sizeof(char),2);char _tmp34=*((char*)
_check_unknown_subscript(_tmp33,sizeof(char),0));char _tmp35='\n';if(
_get_arr_size(_tmp33,sizeof(char))== 1?_tmp34 == '\000'?_tmp35 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp33.curr)=_tmp35;});{int j=3;for(0;j < _tmp2C;j ++){({
struct _tagged_arr _tmp36=_tagged_arr_plus(nlb,sizeof(char),j);char _tmp37=*((char*)
_check_unknown_subscript(_tmp36,sizeof(char),0));char _tmp38='~';if(_get_arr_size(
_tmp36,sizeof(char))== 1?_tmp37 == '\000'?_tmp38 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp36.curr)=_tmp38;});}}((void(*)(struct Cyc_Hashtable_Table*t,int key,
struct _tagged_arr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,({struct _tagged_arr*
_tmp39=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp39[0]=(struct _tagged_arr)nlb;
_tmp39;}));return(struct _tagged_arr)nlb;}_LL11:;_LL12:(void)_throw(_tmp1E);_LLE:;}}}}
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;int Cyc_PP_infinity=9999999;struct
_tuple1{int f1;struct _tagged_arr f2;};static struct Cyc_PP_Out*Cyc_PP_text_doc_f(
struct _tuple1*clo,struct Cyc_PP_Ppstate*st){struct _tuple1 _tmp3B;int _tmp3C;struct
_tagged_arr _tmp3D;struct _tuple1*_tmp3A=clo;_tmp3B=*_tmp3A;_tmp3C=_tmp3B.f1;
_tmp3D=_tmp3B.f2;return({struct Cyc_PP_Out*_tmp3E=_cycalloc(sizeof(*_tmp3E));
_tmp3E->newcc=st->cc + _tmp3C;_tmp3E->newcl=st->cl;_tmp3E->ppout=(void*)((void*)({
struct Cyc_PP_Single_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct
Cyc_PP_Single_struct _tmp40;_tmp40.tag=0;_tmp40.f1=(void*)({struct _tagged_arr*
_tmp41=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp41[0]=_tmp3D;_tmp41;});_tmp40;});
_tmp3F;}));_tmp3E->links=(void*)((void*)0);_tmp3E;});}static struct Cyc_PP_Doc*Cyc_PP_text_doc(
struct _tagged_arr s){int slen=(int)Cyc_strlen((struct _tagged_arr)s);return({struct
Cyc_PP_Doc*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->mwo=slen;_tmp42->mw=Cyc_PP_infinity;
_tmp42->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple1*,
struct Cyc_PP_Ppstate*),struct _tuple1*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,({
struct _tuple1*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=slen;_tmp43->f2=s;
_tmp43;}));_tmp42;});}struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s){struct Cyc_Hashtable_Table*
t;if(Cyc_PP_str_hashtbl == 0){t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(
101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);Cyc_PP_str_hashtbl=({struct Cyc_Core_Opt*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->v=t;_tmp44;});}else{t=(struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{struct _handler_cons
_tmp45;_push_handler(& _tmp45);{int _tmp47=0;if(setjmp(_tmp45.handler))_tmp47=1;
if(!_tmp47){{struct Cyc_PP_Doc*_tmp49=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,({struct _tagged_arr*_tmp48=
_cycalloc(sizeof(*_tmp48));_tmp48[0]=s;_tmp48;}));_npop_handler(0);return _tmp49;};
_pop_handler();}else{void*_tmp46=(void*)_exn_thrown;void*_tmp4B=_tmp46;_LL14: if(
_tmp4B != Cyc_Core_Not_found)goto _LL16;_LL15: {struct Cyc_PP_Doc*d=Cyc_PP_text_doc(
s);((void(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*key,struct Cyc_PP_Doc*
val))Cyc_Hashtable_insert)(t,({struct _tagged_arr*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C[0]=s;_tmp4C;}),d);return d;}_LL16:;_LL17:(void)_throw(_tmp4B);_LL13:;}}}}
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _tagged_arr*s){return Cyc_PP_text(*((struct
_tagged_arr*)s));}static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _tagged_arr
s,int slen){return({struct Cyc_PP_Doc*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->mwo=
slen;_tmp4D->mw=Cyc_PP_infinity;_tmp4D->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*
f)(struct _tuple1*,struct Cyc_PP_Ppstate*),struct _tuple1*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,({
struct _tuple1*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=slen;_tmp4E->f2=s;
_tmp4E;}));_tmp4D;});}static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;struct Cyc_PP_Doc*
Cyc_PP_text_width(struct _tagged_arr s,int slen){struct Cyc_Hashtable_Table*t;if(Cyc_PP_str_hashtbl2
== 0){t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(101,
Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);Cyc_PP_str_hashtbl2=({struct Cyc_Core_Opt*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->v=t;_tmp4F;});}else{t=(struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{struct _handler_cons
_tmp50;_push_handler(& _tmp50);{int _tmp52=0;if(setjmp(_tmp50.handler))_tmp52=1;
if(!_tmp52){{struct Cyc_PP_Doc*_tmp54=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,({struct _tagged_arr*_tmp53=
_cycalloc(sizeof(*_tmp53));_tmp53[0]=s;_tmp53;}));_npop_handler(0);return _tmp54;};
_pop_handler();}else{void*_tmp51=(void*)_exn_thrown;void*_tmp56=_tmp51;_LL19: if(
_tmp56 != Cyc_Core_Not_found)goto _LL1B;_LL1A: {struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(
s,slen);((void(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*key,struct Cyc_PP_Doc*
val))Cyc_Hashtable_insert)(t,({struct _tagged_arr*_tmp57=_cycalloc(sizeof(*_tmp57));
_tmp57[0]=s;_tmp57;}),d);return d;}_LL1B:;_LL1C:(void)_throw(_tmp56);_LL18:;}}}}
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=
0;struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;struct Cyc_PP_Doc*Cyc_PP_nil_doc(){if(
Cyc_PP_nil_doc_opt == 0)Cyc_PP_nil_doc_opt=({struct Cyc_Core_Opt*_tmp58=_cycalloc(
sizeof(*_tmp58));_tmp58->v=Cyc_PP_text(({const char*_tmp59="";_tag_arr(_tmp59,
sizeof(char),_get_zero_arr_size(_tmp59,1));}));_tmp58;});return(struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}struct Cyc_PP_Doc*Cyc_PP_blank_doc(){
if(Cyc_PP_blank_doc_opt == 0)Cyc_PP_blank_doc_opt=({struct Cyc_Core_Opt*_tmp5A=
_cycalloc(sizeof(*_tmp5A));_tmp5A->v=Cyc_PP_text(({const char*_tmp5B="";_tag_arr(
_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,1));}));_tmp5A;});return(struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple2{int f1;
struct _tagged_arr f2;struct _tagged_arr f3;};struct _tuple3{int f1;int f2;int f3;struct
_tagged_arr f4;};static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple2*clo,
struct Cyc_PP_Ppstate*st){struct _tuple2 _tmp5D;int _tmp5E;struct _tagged_arr _tmp5F;
struct _tagged_arr _tmp60;struct _tuple2*_tmp5C=clo;_tmp5D=*_tmp5C;_tmp5E=_tmp5D.f1;
_tmp5F=_tmp5D.f2;_tmp60=_tmp5D.f3;return({struct Cyc_PP_Out*_tmp61=_cycalloc(
sizeof(*_tmp61));_tmp61->newcc=st->cc + _tmp5E;_tmp61->newcl=st->cl;_tmp61->ppout=(
void*)((void*)({struct Cyc_PP_Single_struct*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65[0]=({struct Cyc_PP_Single_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(void*)({
struct _tagged_arr*_tmp67=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp67[0]=
_tmp5F;_tmp67;});_tmp66;});_tmp65;}));_tmp61->links=(void*)((void*)({struct Cyc_PP_Single_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_PP_Single_struct _tmp63;
_tmp63.tag=0;_tmp63.f1=(void*)({struct _tuple3*_tmp64=_cycalloc(sizeof(*_tmp64));
_tmp64->f1=st->cl;_tmp64->f2=st->cc;_tmp64->f3=_tmp5E;_tmp64->f4=_tmp60;_tmp64;});
_tmp63;});_tmp62;}));_tmp61;});}struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct
_tagged_arr shrt,struct _tagged_arr full){int slen=(int)Cyc_strlen((struct
_tagged_arr)shrt);return({struct Cyc_PP_Doc*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68->mwo=slen;_tmp68->mw=Cyc_PP_infinity;_tmp68->f=((struct Cyc_Fn_Function*(*)(
struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(
Cyc_PP_hyperlink_f,({struct _tuple2*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->f1=
slen;_tmp69->f2=shrt;_tmp69->f3=full;_tmp69;}));_tmp68;});}static struct Cyc_PP_Out*
Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){return({struct Cyc_PP_Out*_tmp6A=_cycalloc(
sizeof(*_tmp6A));_tmp6A->newcc=st->ci;_tmp6A->newcl=st->cl + 1;_tmp6A->ppout=(
void*)((void*)({struct Cyc_PP_Single_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));
_tmp6B[0]=({struct Cyc_PP_Single_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(void*)({
struct _tagged_arr*_tmp6D=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp6D[0]=Cyc_PP_nlblanks(
st->ci);_tmp6D;});_tmp6C;});_tmp6B;}));_tmp6A->links=(void*)((void*)0);_tmp6A;});}
struct Cyc_PP_Doc*Cyc_PP_line_doc(){if(Cyc_PP_line_doc_opt == 0)Cyc_PP_line_doc_opt=({
struct Cyc_Core_Opt*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->v=({struct Cyc_PP_Doc*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->mwo=0;_tmp6F->mw=0;_tmp6F->f=((struct
Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(
Cyc_PP_line_f);_tmp6F;});_tmp6E;});return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple4{int f1;struct Cyc_PP_Doc*f2;};
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple4*clo,struct Cyc_PP_Ppstate*st){
struct _tuple4 _tmp71;int _tmp72;struct Cyc_PP_Doc*_tmp73;struct _tuple4*_tmp70=clo;
_tmp71=*_tmp70;_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;{struct Cyc_PP_Ppstate*st2=({
struct Cyc_PP_Ppstate*_tmp74=_cycalloc_atomic(sizeof(*_tmp74));_tmp74->ci=st->ci + 
_tmp72;_tmp74->cc=st->cc;_tmp74->cl=st->cl;_tmp74->pw=st->pw;_tmp74->epw=st->epw;
_tmp74;});return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*
x))Cyc_Fn_apply)(_tmp73->f,st2);}}struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*
d){return({struct Cyc_PP_Doc*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->mwo=d->mwo;
_tmp75->mw=d->mw;_tmp75->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(
struct _tuple4*,struct Cyc_PP_Ppstate*),struct _tuple4*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,({
struct _tuple4*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=k;_tmp76->f2=d;_tmp76;}));
_tmp75;});}int Cyc_PP_min(int x,int y){if(x <= y)return x;else{return y;}}int Cyc_PP_max(
int x,int y){if(x >= y)return x;else{return y;}}struct _tuple5{struct Cyc_PP_Doc*f1;
struct Cyc_PP_Doc*f2;};static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple5*clo,
struct Cyc_PP_Ppstate*st){struct _tuple5 _tmp78;struct Cyc_PP_Doc*_tmp79;struct Cyc_PP_Doc*
_tmp7A;struct _tuple5*_tmp77=clo;_tmp78=*_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;{
int epw2=Cyc_PP_max(st->pw - _tmp7A->mw,st->epw - _tmp79->mwo);struct Cyc_PP_Ppstate*
st2=({struct Cyc_PP_Ppstate*_tmp7D=_cycalloc_atomic(sizeof(*_tmp7D));_tmp7D->ci=
st->ci;_tmp7D->cc=st->cc;_tmp7D->cl=st->cl;_tmp7D->pw=st->pw;_tmp7D->epw=epw2;
_tmp7D;});struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,
struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp79->f,st2);struct Cyc_PP_Ppstate*st3=({
struct Cyc_PP_Ppstate*_tmp7C=_cycalloc_atomic(sizeof(*_tmp7C));_tmp7C->ci=st->ci;
_tmp7C->cc=o1->newcc;_tmp7C->cl=o1->newcl;_tmp7C->pw=st->pw;_tmp7C->epw=epw2;
_tmp7C;});struct Cyc_PP_Out*o2=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,
struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp7A->f,st3);struct Cyc_PP_Out*o3=({
struct Cyc_PP_Out*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->newcc=o2->newcc;
_tmp7B->newcl=o2->newcl;_tmp7B->ppout=(void*)((void*(*)(void*a1,void*a2))Cyc_PP_append)((
void*)o1->ppout,(void*)o2->ppout);_tmp7B->links=(void*)((void*(*)(void*a1,void*
a2))Cyc_PP_append)((void*)o1->links,(void*)o2->links);_tmp7B;});return o3;}}
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->mwo=Cyc_PP_min(
d1->mw,d1->mwo + d2->mwo);_tmp7E->mw=Cyc_PP_min(d1->mw,d1->mwo + d2->mw);_tmp7E->f=((
struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),
struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,({struct _tuple5*_tmp7F=
_cycalloc(sizeof(*_tmp7F));_tmp7F->f1=d1;_tmp7F->f2=d2;_tmp7F;}));_tmp7E;});}
struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr l){struct Cyc_PP_Doc*_tmp80=Cyc_PP_nil_doc();{
int i=(int)(_get_arr_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;i --){
_tmp80=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_unknown_subscript(l,sizeof(
struct Cyc_PP_Doc*),i)),_tmp80);}}return _tmp80;}static struct Cyc_PP_Out*Cyc_PP_long_cats_f(
struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;{
struct Cyc_List_List*_tmp81=ds0;for(0;_tmp81 != 0;_tmp81=_tmp81->tl){struct Cyc_PP_Doc*
d=(struct Cyc_PP_Doc*)_tmp81->hd;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct
Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);st=({struct Cyc_PP_Ppstate*
_tmp82=_cycalloc_atomic(sizeof(*_tmp82));_tmp82->ci=st->ci;_tmp82->cc=o->newcc;
_tmp82->cl=o->newcl;_tmp82->pw=st->pw;_tmp82->epw=st->epw - d->mwo;_tmp82;});os=({
struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=o;_tmp83->tl=os;
_tmp83;});}}{int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;void*s=(void*)0;void*links=(void*)0;
for(0;os != 0;os=os->tl){s=((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((
struct Cyc_PP_Out*)os->hd)->ppout,s);links=((void*(*)(void*a1,void*a2))Cyc_PP_append)((
void*)((struct Cyc_PP_Out*)os->hd)->links,links);}return({struct Cyc_PP_Out*_tmp84=
_cycalloc(sizeof(*_tmp84));_tmp84->newcc=newcc;_tmp84->newcl=newcl;_tmp84->ppout=(
void*)s;_tmp84->links=(void*)links;_tmp84;});}}static struct Cyc_PP_Doc*Cyc_PP_long_cats(
struct Cyc_List_List*doclist){struct Cyc_List_List*orig=doclist;struct Cyc_PP_Doc*d=(
struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;doclist=
doclist->tl;{int mw=d->mw;int mwo=d->mw;{struct Cyc_List_List*_tmp85=doclist;for(0;
_tmp85 != 0;_tmp85=_tmp85->tl){int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)
_tmp85->hd)->mwo);int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp85->hd)->mw);
mw=mw2;mwo=mwo2;}}return({struct Cyc_PP_Doc*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86->mwo=mw;_tmp86->mw=mwo;_tmp86->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*
f)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(
Cyc_PP_long_cats_f,orig);_tmp86;});}}struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*
doclist){if(doclist == 0)return Cyc_PP_nil_doc();else{if(doclist->tl == 0)return(
struct Cyc_PP_Doc*)doclist->hd;else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
doclist)> 30)return Cyc_PP_long_cats(doclist);else{return Cyc_PP_concat((struct Cyc_PP_Doc*)
doclist->hd,Cyc_PP_cats(doclist->tl));}}}}static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(
struct _tagged_arr*docs_ptr,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;
struct _tagged_arr docs=*((struct _tagged_arr*)docs_ptr);int sz=(int)_get_arr_size(
docs,sizeof(struct Cyc_PP_Doc*));{int i=0;for(0;i < sz;++ i){struct Cyc_PP_Doc*d=*((
struct Cyc_PP_Doc**)_check_unknown_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*
x))Cyc_Fn_apply)(d->f,st);st=({struct Cyc_PP_Ppstate*_tmp87=_cycalloc_atomic(
sizeof(*_tmp87));_tmp87->ci=st->ci;_tmp87->cc=o->newcc;_tmp87->cl=o->newcl;
_tmp87->pw=st->pw;_tmp87->epw=st->epw - d->mwo;_tmp87;});os=({struct Cyc_List_List*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->hd=o;_tmp88->tl=os;_tmp88;});}}{int
newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;void*s=(void*)0;void*links=(void*)0;
for(0;os != 0;os=os->tl){s=((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((
struct Cyc_PP_Out*)os->hd)->ppout,s);links=((void*(*)(void*a1,void*a2))Cyc_PP_append)((
void*)((struct Cyc_PP_Out*)os->hd)->links,links);}return({struct Cyc_PP_Out*_tmp89=
_cycalloc(sizeof(*_tmp89));_tmp89->newcc=newcc;_tmp89->newcl=newcl;_tmp89->ppout=(
void*)s;_tmp89->links=(void*)links;_tmp89;});}}struct Cyc_PP_Doc*Cyc_PP_cats_arr(
struct _tagged_arr docs){int sz=(int)_get_arr_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp8A=_cycalloc(
sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Core_Failure_struct _tmp8B;_tmp8B.tag=Cyc_Core_Failure;
_tmp8B.f1=({const char*_tmp8C="cats_arr -- size zero array";_tag_arr(_tmp8C,
sizeof(char),_get_zero_arr_size(_tmp8C,28));});_tmp8B;});_tmp8A;}));{struct Cyc_PP_Doc*
d=*((struct Cyc_PP_Doc**)_check_unknown_subscript(docs,sizeof(struct Cyc_PP_Doc*),
0));int mw=d->mw;int mwo=d->mw;{int i=1;for(0;i < sz;++ i){int mw2=Cyc_PP_min(mw,mwo + (*((
struct Cyc_PP_Doc**)_check_unknown_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);
int mwo2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_unknown_subscript(docs,
sizeof(struct Cyc_PP_Doc*),i)))->mw);mw=mw2;mwo=mwo2;}}return({struct Cyc_PP_Doc*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->mwo=mw;_tmp8D->mw=mwo;_tmp8D->f=((
struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tagged_arr*,struct Cyc_PP_Ppstate*),
struct _tagged_arr*x))Cyc_Fn_make_fn)(Cyc_PP_cats_arr_f,({struct _tagged_arr*
_tmp8E=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp8E[0]=docs;_tmp8E;}));_tmp8D;});}}
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*
st){struct _tuple5 _tmp90;struct Cyc_PP_Doc*_tmp91;struct Cyc_PP_Doc*_tmp92;struct
_tuple5*_tmp8F=clo;_tmp90=*_tmp8F;_tmp91=_tmp90.f1;_tmp92=_tmp90.f2;{int dfits=st->cc
+ _tmp91->mwo <= st->epw?1: st->cc + _tmp91->mw <= st->pw;if(dfits)return((struct Cyc_PP_Out*(*)(
struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp91->f,st);
else{return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))
Cyc_Fn_apply)(_tmp92->f,st);}}}struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*
d,struct Cyc_PP_Doc*d2){return({struct Cyc_PP_Doc*_tmp93=_cycalloc(sizeof(*_tmp93));
_tmp93->mwo=Cyc_PP_min(d->mwo,d2->mwo);_tmp93->mw=Cyc_PP_min(d->mw,d2->mw);
_tmp93->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,
struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,({
struct _tuple5*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=d;_tmp94->f2=d2;
_tmp94;}));_tmp93;});}struct Cyc_PP_Doc*Cyc_PP_oline_doc(){return Cyc_PP_doc_union(
Cyc_PP_nil_doc(),Cyc_PP_line_doc());}static struct Cyc_PP_Out*Cyc_PP_tab_f(struct
Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*
_tmp95=_cycalloc_atomic(sizeof(*_tmp95));_tmp95->ci=st->cc;_tmp95->cc=st->cc;
_tmp95->cl=st->cl;_tmp95->pw=st->pw;_tmp95->epw=st->epw;_tmp95;});return((struct
Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,
st2);}struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){struct Cyc_PP_Doc*d2=({
struct Cyc_PP_Doc*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->mwo=d->mwo;_tmp96->mw=
d->mw;_tmp96->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d);
_tmp96;});return d2;}static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l){if(l == 0)return Cyc_PP_nil_doc();
else{if(l->tl == 0)return pp((void*)l->hd);else{return({struct Cyc_PP_Doc*_tmp97[4];
_tmp97[3]=Cyc_PP_ppseq_f(pp,sep,l->tl);_tmp97[2]=Cyc_PP_oline_doc();_tmp97[1]=
Cyc_PP_text(sep);_tmp97[0]=pp((void*)l->hd);Cyc_PP_cat(_tag_arr(_tmp97,sizeof(
struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseq_f(
pp,sep,l));}struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _tagged_arr sep,struct Cyc_List_List*
l){if(l == 0)return Cyc_PP_nil_doc();else{if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;
else{struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmp98=l;while(_tmp98->tl != 0){struct Cyc_List_List*_tmp99=
_tmp98->tl;_tmp98->tl=({struct Cyc_List_List*_tmp9A=_cycalloc(sizeof(*_tmp9A));
_tmp9A->hd=sep2;_tmp9A->tl=({struct Cyc_List_List*_tmp9B=_cycalloc(sizeof(*_tmp9B));
_tmp9B->hd=oline;_tmp9B->tl=_tmp99;_tmp9B;});_tmp9A;});_tmp98=_tmp99;}return Cyc_PP_cats(
l);}}}struct Cyc_PP_Doc*Cyc_PP_seq(struct _tagged_arr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct
Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l){if(l == 0)
return Cyc_PP_nil_doc();else{if(l->tl == 0)return pp((void*)l->hd);else{return({
struct Cyc_PP_Doc*_tmp9C[4];_tmp9C[3]=Cyc_PP_ppseql_f(pp,sep,l->tl);_tmp9C[2]=Cyc_PP_line_doc();
_tmp9C[1]=Cyc_PP_text(sep);_tmp9C[0]=pp((void*)l->hd);Cyc_PP_cat(_tag_arr(_tmp9C,
sizeof(struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*
pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseql_f(
pp,sep,l));}static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _tagged_arr sep,struct Cyc_List_List*
l){if(l == 0)return Cyc_PP_nil_doc();else{if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;
else{return({struct Cyc_PP_Doc*_tmp9D[4];_tmp9D[3]=Cyc_PP_seql_f(sep,l->tl);
_tmp9D[2]=Cyc_PP_line_doc();_tmp9D[1]=Cyc_PP_text(sep);_tmp9D[0]=(struct Cyc_PP_Doc*)
l->hd;Cyc_PP_cat(_tag_arr(_tmp9D,sizeof(struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*
Cyc_PP_seql(struct _tagged_arr sep,struct Cyc_List_List*l0){return Cyc_PP_tab(Cyc_PP_seql_f(
sep,l0));}struct Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr
stop,struct _tagged_arr sep,struct Cyc_List_List*ss){return({struct Cyc_PP_Doc*
_tmp9E[3];_tmp9E[2]=Cyc_PP_text(stop);_tmp9E[1]=Cyc_PP_seq(sep,ss);_tmp9E[0]=Cyc_PP_text(
start);Cyc_PP_cat(_tag_arr(_tmp9E,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_PP_egroup(struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,
struct Cyc_List_List*ss){if(ss == 0)return Cyc_PP_nil_doc();else{return({struct Cyc_PP_Doc*
_tmp9F[3];_tmp9F[2]=Cyc_PP_text(stop);_tmp9F[1]=Cyc_PP_seq(sep,ss);_tmp9F[0]=Cyc_PP_text(
start);Cyc_PP_cat(_tag_arr(_tmp9F,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_PP_groupl(struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,
struct Cyc_List_List*ss){return({struct Cyc_PP_Doc*_tmpA0[3];_tmpA0[2]=Cyc_PP_text(
stop);_tmpA0[1]=Cyc_PP_seql(sep,ss);_tmpA0[0]=Cyc_PP_text(start);Cyc_PP_cat(
_tag_arr(_tmpA0,sizeof(struct Cyc_PP_Doc*),3));});}
