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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};struct
_tagged_arr Cyc_Core_new_string(unsigned int);void*Cyc_Core_identity(void*);int
Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[
8];unsigned int Cyc_Std_strlen(struct _tagged_arr s);int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_create(
int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct
Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*
t,void*key);int Cyc_Hashtable_hash_stringptr(struct _tagged_arr*p);struct Cyc_Fn_Function{
void*(*f)(void*,void*);void*env;};struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(
void*,void*),void*x);struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*f)(void*));void*
Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc_Std___cycFILE*
f);struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct _tuple0{
struct _tagged_arr f1;struct Cyc_List_List*f2;};struct _tuple0*Cyc_PP_string_and_links(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_oline_doc();struct Cyc_PP_Doc*
Cyc_PP_text(struct _tagged_arr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct
_tagged_arr*p);struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _tagged_arr shrt,struct
_tagged_arr full);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*
Cyc_PP_cat(struct _tagged_arr);struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*
doclist);struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _tagged_arr docs);struct Cyc_PP_Doc*
Cyc_PP_doc_union(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);struct Cyc_PP_Doc*Cyc_PP_tab(
struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_seq(struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _tagged_arr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_groupl(struct _tagged_arr start,struct _tagged_arr stop,
struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(
struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_Xarray_Xarray{struct _tagged_arr elmts;int num_elmts;};int Cyc_Xarray_length(
struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);void
Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);struct Cyc_Xarray_Xarray*Cyc_Xarray_create(
int,void*);void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);struct Cyc_PP_Single_struct{
int tag;void*f1;};struct Cyc_PP_Append_struct{int tag;void*f1;void*f2;};void*Cyc_PP_append(
void*a1,void*a2){if(a1 == (void*)0)return a2;else{if(a2 == (void*)0)return a1;else{
return(void*)({struct Cyc_PP_Append_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[
0]=({struct Cyc_PP_Append_struct _tmp1;_tmp1.tag=1;_tmp1.f1=(void*)a1;_tmp1.f2=(
void*)a2;_tmp1;});_tmp0;});}}}struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,
struct Cyc_List_List*l){void*_tmp2=y;void*_tmp3;void*_tmp4;void*_tmp5;_LL1: if((
int)_tmp2 != 0)goto _LL3;_LL2: return l;_LL3: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 
0)goto _LL5;_tmp3=(void*)((struct Cyc_PP_Single_struct*)_tmp2)->f1;_LL4: return({
struct Cyc_List_List*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=(void*)_tmp3;_tmp6->tl=
l;_tmp6;});_LL5: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 1)goto _LL0;_tmp4=(void*)((
struct Cyc_PP_Append_struct*)_tmp2)->f1;_tmp5=(void*)((struct Cyc_PP_Append_struct*)
_tmp2)->f2;_LL6: return Cyc_PP_list_of_alist_f(_tmp4,Cyc_PP_list_of_alist_f(_tmp5,
l));_LL0:;}struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){return Cyc_PP_list_of_alist_f(
x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int
newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*
f;};static void Cyc_PP_dump_out(struct Cyc_Std___cycFILE*f,void*al){struct Cyc_Xarray_Xarray*
_tmp7=Cyc_Xarray_create(16,al);Cyc_Xarray_add(_tmp7,al);{int last=0;while(last >= 
0){void*_tmp8=Cyc_Xarray_get(_tmp7,last);struct _tagged_arr*_tmp9;void*_tmpA;void*
_tmpB;_LL8: if((int)_tmp8 != 0)goto _LLA;_LL9: -- last;goto _LL7;_LLA: if(_tmp8 <= (void*)
1?1:*((int*)_tmp8)!= 0)goto _LLC;_tmp9=(struct _tagged_arr*)((struct Cyc_PP_Single_struct*)
_tmp8)->f1;_LLB: -- last;({struct Cyc_Std_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(
struct _tagged_arr)*_tmp9;{void*_tmpC[1]={& _tmpD};Cyc_Std_fprintf(f,_tag_arr("%s",
sizeof(char),3),_tag_arr(_tmpC,sizeof(void*),1));}});goto _LL7;_LLC: if(_tmp8 <= (
void*)1?1:*((int*)_tmp8)!= 1)goto _LL7;_tmpA=(void*)((struct Cyc_PP_Append_struct*)
_tmp8)->f1;_tmpB=(void*)((struct Cyc_PP_Append_struct*)_tmp8)->f2;_LLD: Cyc_Xarray_set(
_tmp7,last,_tmpB);if(last == Cyc_Xarray_length(_tmp7)- 1)Cyc_Xarray_add(_tmp7,
_tmpA);else{Cyc_Xarray_set(_tmp7,last + 1,_tmpA);}++ last;goto _LL7;_LL7:;}}}void
Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc_Std___cycFILE*f){struct Cyc_PP_Out*
o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(
d->f,({struct Cyc_PP_Ppstate*_tmpE=_cycalloc_atomic(sizeof(*_tmpE));_tmpE->ci=0;
_tmpE->cc=0;_tmpE->cl=1;_tmpE->pw=w;_tmpE->epw=w;_tmpE;}));Cyc_PP_dump_out(f,(
void*)o->ppout);}struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*
x))Cyc_Fn_apply)(d->f,({struct Cyc_PP_Ppstate*_tmpF=_cycalloc_atomic(sizeof(*
_tmpF));_tmpF->ci=0;_tmpF->cc=0;_tmpF->cl=1;_tmpF->pw=w;_tmpF->epw=w;_tmpF;}));
return(struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(void*x))
Cyc_PP_list_of_alist)((void*)o->ppout),_tag_arr("",sizeof(char),1));}struct
_tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Out*o=((
struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(
d->f,({struct Cyc_PP_Ppstate*_tmp11=_cycalloc_atomic(sizeof(*_tmp11));_tmp11->ci=
0;_tmp11->cc=0;_tmp11->cl=1;_tmp11->pw=w;_tmp11->epw=w;_tmp11;}));return({struct
_tuple0*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->f1=(struct _tagged_arr)Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)((void*)o->ppout),_tag_arr("",
sizeof(char),1));_tmp10->f2=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)((
void*)o->links);_tmp10;});}static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;struct
_tagged_arr Cyc_PP_nlblanks(int i){if(Cyc_PP_bhashtbl == 0)Cyc_PP_bhashtbl=({struct
Cyc_Core_Opt*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->v=((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(
int(*)(int))Cyc_Core_identity);_tmp12;});if(i < 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_Core_Failure_struct
_tmp14;_tmp14.tag=Cyc_Core_Failure;_tmp14.f1=_tag_arr("nlblanks",sizeof(char),9);
_tmp14;});_tmp13;}));{struct _handler_cons _tmp15;_push_handler(& _tmp15);{int
_tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!_tmp17){{struct _tagged_arr _tmp18=*((
struct _tagged_arr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((
struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,
i);_npop_handler(0);return _tmp18;};_pop_handler();}else{void*_tmp16=(void*)
_exn_thrown;void*_tmp1A=_tmp16;_LLF: if(_tmp1A != Cyc_Core_Not_found)goto _LL11;
_LL10: {int _tmp1B=i / 8;int _tmp1C=i % 8;int _tmp1D=(1 + _tmp1B)+ _tmp1C;struct
_tagged_arr nlb=Cyc_Core_new_string((unsigned int)_tmp1D);*((char*)
_check_unknown_subscript(nlb,sizeof(char),0))='\n';{int j=0;for(0;j < _tmp1B;j ++){*((
char*)_check_unknown_subscript(nlb,sizeof(char),j + 1))='\t';}}{int j=0;for(0;j < 
_tmp1C;j ++){*((char*)_check_unknown_subscript(nlb,sizeof(char),(j + 1)+ _tmp1B))=' ';}}((
void(*)(struct Cyc_Hashtable_Table*t,int key,struct _tagged_arr*val))Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,
i,({struct _tagged_arr*_tmp1E=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp1E[0]=(
struct _tagged_arr)nlb;_tmp1E;}));return(struct _tagged_arr)nlb;}_LL11:;_LL12:(
void)_throw(_tmp1A);_LLE:;}}}}static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;int
Cyc_PP_infinity=9999999;struct _tuple1{int f1;struct _tagged_arr f2;};static struct
Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple1*clo,struct Cyc_PP_Ppstate*st){struct
_tuple1 _tmp20;int _tmp21;struct _tagged_arr _tmp22;struct _tuple1*_tmp1F=clo;_tmp20=*
_tmp1F;_tmp21=_tmp20.f1;_tmp22=_tmp20.f2;return({struct Cyc_PP_Out*_tmp23=
_cycalloc(sizeof(*_tmp23));_tmp23->newcc=st->cc + _tmp21;_tmp23->newcl=st->cl;
_tmp23->ppout=(void*)((void*)({struct Cyc_PP_Single_struct*_tmp24=_cycalloc(
sizeof(*_tmp24));_tmp24[0]=({struct Cyc_PP_Single_struct _tmp25;_tmp25.tag=0;
_tmp25.f1=(void*)({struct _tagged_arr*_tmp26=_cycalloc(sizeof(struct _tagged_arr)* 
1);_tmp26[0]=_tmp22;_tmp26;});_tmp25;});_tmp24;}));_tmp23->links=(void*)((void*)
0);_tmp23;});}static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _tagged_arr s){int slen=(
int)Cyc_Std_strlen(s);return({struct Cyc_PP_Doc*_tmp27=_cycalloc(sizeof(*_tmp27));
_tmp27->mwo=slen;_tmp27->mw=Cyc_PP_infinity;_tmp27->f=((struct Cyc_Fn_Function*(*)(
struct Cyc_PP_Out*(*f)(struct _tuple1*,struct Cyc_PP_Ppstate*),struct _tuple1*x))Cyc_Fn_make_fn)(
Cyc_PP_text_doc_f,({struct _tuple1*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->f1=
slen;_tmp28->f2=s;_tmp28;}));_tmp27;});}struct Cyc_PP_Doc*Cyc_PP_text(struct
_tagged_arr s){struct Cyc_Hashtable_Table*t;if(Cyc_PP_str_hashtbl == 0){t=((struct
Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),
int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(101,Cyc_Std_zstrptrcmp,Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl=({struct Cyc_Core_Opt*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->v=
t;_tmp29;});}else{t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_str_hashtbl))->v;}{struct _handler_cons _tmp2A;_push_handler(&
_tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!_tmp2C){{struct Cyc_PP_Doc*
_tmp2E=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))
Cyc_Hashtable_lookup)(t,({struct _tagged_arr*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D[0]=s;_tmp2D;}));_npop_handler(0);return _tmp2E;};_pop_handler();}else{void*
_tmp2B=(void*)_exn_thrown;void*_tmp30=_tmp2B;_LL14: if(_tmp30 != Cyc_Core_Not_found)
goto _LL16;_LL15: {struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,({struct
_tagged_arr*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=s;_tmp31;}),d);return d;}
_LL16:;_LL17:(void)_throw(_tmp30);_LL13:;}}}}struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _tagged_arr*s){return Cyc_PP_text(*s);}struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=
0;struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=
0;struct Cyc_PP_Doc*Cyc_PP_nil_doc(){if(Cyc_PP_nil_doc_opt == 0)Cyc_PP_nil_doc_opt=({
struct Cyc_Core_Opt*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->v=Cyc_PP_text(
_tag_arr("",sizeof(char),1));_tmp32;});return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_nil_doc_opt))->v;}struct Cyc_PP_Doc*Cyc_PP_blank_doc(){if(Cyc_PP_blank_doc_opt
== 0)Cyc_PP_blank_doc_opt=({struct Cyc_Core_Opt*_tmp33=_cycalloc(sizeof(*_tmp33));
_tmp33->v=Cyc_PP_text(_tag_arr("",sizeof(char),1));_tmp33;});return(struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple2{int f1;
struct _tagged_arr f2;struct _tagged_arr f3;};struct _tuple3{int f1;int f2;int f3;struct
_tagged_arr f4;};static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple2*clo,
struct Cyc_PP_Ppstate*st){struct _tuple2 _tmp35;int _tmp36;struct _tagged_arr _tmp37;
struct _tagged_arr _tmp38;struct _tuple2*_tmp34=clo;_tmp35=*_tmp34;_tmp36=_tmp35.f1;
_tmp37=_tmp35.f2;_tmp38=_tmp35.f3;return({struct Cyc_PP_Out*_tmp39=_cycalloc(
sizeof(*_tmp39));_tmp39->newcc=st->cc + _tmp36;_tmp39->newcl=st->cl;_tmp39->ppout=(
void*)((void*)({struct Cyc_PP_Single_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));
_tmp3D[0]=({struct Cyc_PP_Single_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(void*)({
struct _tagged_arr*_tmp3F=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp3F[0]=
_tmp37;_tmp3F;});_tmp3E;});_tmp3D;}));_tmp39->links=(void*)((void*)({struct Cyc_PP_Single_struct*
_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_PP_Single_struct _tmp3B;
_tmp3B.tag=0;_tmp3B.f1=(void*)({struct _tuple3*_tmp3C=_cycalloc(sizeof(*_tmp3C));
_tmp3C->f1=st->cl;_tmp3C->f2=st->cc;_tmp3C->f3=_tmp36;_tmp3C->f4=_tmp38;_tmp3C;});
_tmp3B;});_tmp3A;}));_tmp39;});}struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct
_tagged_arr shrt,struct _tagged_arr full){int slen=(int)Cyc_Std_strlen(shrt);return({
struct Cyc_PP_Doc*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->mwo=slen;_tmp40->mw=
Cyc_PP_infinity;_tmp40->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(
struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,({
struct _tuple2*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=slen;_tmp41->f2=shrt;
_tmp41->f3=full;_tmp41;}));_tmp40;});}static struct Cyc_PP_Out*Cyc_PP_line_f(
struct Cyc_PP_Ppstate*st){return({struct Cyc_PP_Out*_tmp42=_cycalloc(sizeof(*
_tmp42));_tmp42->newcc=st->ci;_tmp42->newcl=st->cl + 1;_tmp42->ppout=(void*)((
void*)({struct Cyc_PP_Single_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({
struct Cyc_PP_Single_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(void*)({struct
_tagged_arr*_tmp45=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp45[0]=Cyc_PP_nlblanks(
st->ci);_tmp45;});_tmp44;});_tmp43;}));_tmp42->links=(void*)((void*)0);_tmp42;});}
struct Cyc_PP_Doc*Cyc_PP_line_doc(){if(Cyc_PP_line_doc_opt == 0)Cyc_PP_line_doc_opt=({
struct Cyc_Core_Opt*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->v=({struct Cyc_PP_Doc*
_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->mwo=0;_tmp47->mw=0;_tmp47->f=((struct
Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(
Cyc_PP_line_f);_tmp47;});_tmp46;});return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple4{int f1;struct Cyc_PP_Doc*f2;};
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple4*clo,struct Cyc_PP_Ppstate*st){
struct _tuple4 _tmp49;int _tmp4A;struct Cyc_PP_Doc*_tmp4B;struct _tuple4*_tmp48=clo;
_tmp49=*_tmp48;_tmp4A=_tmp49.f1;_tmp4B=_tmp49.f2;{struct Cyc_PP_Ppstate*st2=({
struct Cyc_PP_Ppstate*_tmp4C=_cycalloc_atomic(sizeof(*_tmp4C));_tmp4C->ci=st->ci + 
_tmp4A;_tmp4C->cc=st->cc;_tmp4C->cl=st->cl;_tmp4C->pw=st->pw;_tmp4C->epw=st->epw;
_tmp4C;});return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*
x))Cyc_Fn_apply)(_tmp4B->f,st2);}}struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*
d){return({struct Cyc_PP_Doc*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->mwo=d->mwo;
_tmp4D->mw=d->mw;_tmp4D->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(
struct _tuple4*,struct Cyc_PP_Ppstate*),struct _tuple4*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,({
struct _tuple4*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=k;_tmp4E->f2=d;_tmp4E;}));
_tmp4D;});}int Cyc_PP_min(int x,int y){if(x <= y)return x;else{return y;}}int Cyc_PP_max(
int x,int y){if(x >= y)return x;else{return y;}}struct _tuple5{struct Cyc_PP_Doc*f1;
struct Cyc_PP_Doc*f2;};static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple5*clo,
struct Cyc_PP_Ppstate*st){struct _tuple5 _tmp50;struct Cyc_PP_Doc*_tmp51;struct Cyc_PP_Doc*
_tmp52;struct _tuple5*_tmp4F=clo;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;_tmp52=_tmp50.f2;{
int epw2=Cyc_PP_max(st->pw - _tmp52->mw,st->epw - _tmp51->mwo);struct Cyc_PP_Ppstate*
st2=({struct Cyc_PP_Ppstate*_tmp55=_cycalloc_atomic(sizeof(*_tmp55));_tmp55->ci=
st->ci;_tmp55->cc=st->cc;_tmp55->cl=st->cl;_tmp55->pw=st->pw;_tmp55->epw=epw2;
_tmp55;});struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,
struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp51->f,st2);struct Cyc_PP_Ppstate*st3=({
struct Cyc_PP_Ppstate*_tmp54=_cycalloc_atomic(sizeof(*_tmp54));_tmp54->ci=st->ci;
_tmp54->cc=o1->newcc;_tmp54->cl=o1->newcl;_tmp54->pw=st->pw;_tmp54->epw=epw2;
_tmp54;});struct Cyc_PP_Out*o2=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,
struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp52->f,st3);struct Cyc_PP_Out*o3=({
struct Cyc_PP_Out*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->newcc=o2->newcc;
_tmp53->newcl=o2->newcl;_tmp53->ppout=(void*)((void*(*)(void*a1,void*a2))Cyc_PP_append)((
void*)o1->ppout,(void*)o2->ppout);_tmp53->links=(void*)((void*(*)(void*a1,void*
a2))Cyc_PP_append)((void*)o1->links,(void*)o2->links);_tmp53;});return o3;}}
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->mwo=Cyc_PP_min(
d1->mw,d1->mwo + d2->mwo);_tmp56->mw=Cyc_PP_min(d1->mw,d1->mwo + d2->mw);_tmp56->f=((
struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),
struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,({struct _tuple5*_tmp57=
_cycalloc(sizeof(*_tmp57));_tmp57->f1=d1;_tmp57->f2=d2;_tmp57;}));_tmp56;});}
struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr l){struct Cyc_PP_Doc*_tmp58=Cyc_PP_nil_doc();{
int i=(int)(_get_arr_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;i --){
_tmp58=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_unknown_subscript(l,sizeof(
struct Cyc_PP_Doc*),i)),_tmp58);}}return _tmp58;}static struct Cyc_PP_Out*Cyc_PP_long_cats_f(
struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;{
struct Cyc_List_List*_tmp59=ds0;for(0;_tmp59 != 0;_tmp59=_tmp59->tl){struct Cyc_PP_Doc*
d=(struct Cyc_PP_Doc*)_tmp59->hd;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct
Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);st=({struct Cyc_PP_Ppstate*
_tmp5A=_cycalloc_atomic(sizeof(*_tmp5A));_tmp5A->ci=st->ci;_tmp5A->cc=o->newcc;
_tmp5A->cl=o->newcl;_tmp5A->pw=st->pw;_tmp5A->epw=st->epw - d->mwo;_tmp5A;});os=({
struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->hd=o;_tmp5B->tl=os;
_tmp5B;});}}{int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;void*s=(void*)0;void*links=(void*)0;
for(0;os != 0;os=os->tl){s=((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((
struct Cyc_PP_Out*)os->hd)->ppout,s);links=((void*(*)(void*a1,void*a2))Cyc_PP_append)((
void*)((struct Cyc_PP_Out*)os->hd)->links,links);}return({struct Cyc_PP_Out*_tmp5C=
_cycalloc(sizeof(*_tmp5C));_tmp5C->newcc=newcc;_tmp5C->newcl=newcl;_tmp5C->ppout=(
void*)s;_tmp5C->links=(void*)links;_tmp5C;});}}static struct Cyc_PP_Doc*Cyc_PP_long_cats(
struct Cyc_List_List*doclist){struct Cyc_List_List*orig=doclist;struct Cyc_PP_Doc*d=(
struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;doclist=
doclist->tl;{int mw=d->mw;int mwo=d->mw;{struct Cyc_List_List*_tmp5D=doclist;for(0;
_tmp5D != 0;_tmp5D=_tmp5D->tl){int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)
_tmp5D->hd)->mwo);int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp5D->hd)->mw);
mw=mw2;mwo=mwo2;}}return({struct Cyc_PP_Doc*_tmp5E=_cycalloc(sizeof(*_tmp5E));
_tmp5E->mwo=mw;_tmp5E->mw=mwo;_tmp5E->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*
f)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(
Cyc_PP_long_cats_f,orig);_tmp5E;});}}struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*
doclist){if(doclist == 0)return Cyc_PP_nil_doc();else{if(doclist->tl == 0)return(
struct Cyc_PP_Doc*)doclist->hd;else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
doclist)> 30)return Cyc_PP_long_cats(doclist);else{return Cyc_PP_concat((struct Cyc_PP_Doc*)
doclist->hd,Cyc_PP_cats(doclist->tl));}}}}static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(
struct _tagged_arr*docs_ptr,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;
struct _tagged_arr docs=*docs_ptr;int sz=(int)_get_arr_size(docs,sizeof(struct Cyc_PP_Doc*));{
int i=0;for(0;i < sz;++ i){struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)
_check_unknown_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));struct Cyc_PP_Out*o=((
struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(
d->f,st);st=({struct Cyc_PP_Ppstate*_tmp5F=_cycalloc_atomic(sizeof(*_tmp5F));
_tmp5F->ci=st->ci;_tmp5F->cc=o->newcc;_tmp5F->cl=o->newcl;_tmp5F->pw=st->pw;
_tmp5F->epw=st->epw - d->mwo;_tmp5F;});os=({struct Cyc_List_List*_tmp60=_cycalloc(
sizeof(*_tmp60));_tmp60->hd=o;_tmp60->tl=os;_tmp60;});}}{int newcc=((struct Cyc_PP_Out*)((
struct Cyc_List_List*)_check_null(os))->hd)->newcc;int newcl=((struct Cyc_PP_Out*)
os->hd)->newcl;void*s=(void*)0;void*links=(void*)0;for(0;os != 0;os=os->tl){s=((
void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((struct Cyc_PP_Out*)os->hd)->ppout,
s);links=((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((struct Cyc_PP_Out*)
os->hd)->links,links);}return({struct Cyc_PP_Out*_tmp61=_cycalloc(sizeof(*_tmp61));
_tmp61->newcc=newcc;_tmp61->newcl=newcl;_tmp61->ppout=(void*)s;_tmp61->links=(
void*)links;_tmp61;});}}struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _tagged_arr docs){
int sz=(int)_get_arr_size(docs,sizeof(struct Cyc_PP_Doc*));if(sz == 0)(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({
struct Cyc_Core_Failure_struct _tmp63;_tmp63.tag=Cyc_Core_Failure;_tmp63.f1=
_tag_arr("cats_arr -- size zero array",sizeof(char),28);_tmp63;});_tmp62;}));{
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_unknown_subscript(docs,sizeof(
struct Cyc_PP_Doc*),0));int mw=d->mw;int mwo=d->mw;{int i=1;for(0;i < sz;++ i){int mw2=
Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_unknown_subscript(docs,sizeof(
struct Cyc_PP_Doc*),i)))->mwo);int mwo2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)
_check_unknown_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mw);mw=mw2;mwo=
mwo2;}}return({struct Cyc_PP_Doc*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->mwo=mw;
_tmp64->mw=mwo;_tmp64->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(
struct _tagged_arr*,struct Cyc_PP_Ppstate*),struct _tagged_arr*x))Cyc_Fn_make_fn)(
Cyc_PP_cats_arr_f,({struct _tagged_arr*_tmp65=_cycalloc(sizeof(struct _tagged_arr)
* 1);_tmp65[0]=docs;_tmp65;}));_tmp64;});}}static struct Cyc_PP_Out*Cyc_PP_doc_union_f(
struct _tuple5*clo,struct Cyc_PP_Ppstate*st){struct _tuple5 _tmp67;struct Cyc_PP_Doc*
_tmp68;struct Cyc_PP_Doc*_tmp69;struct _tuple5*_tmp66=clo;_tmp67=*_tmp66;_tmp68=
_tmp67.f1;_tmp69=_tmp67.f2;{int dfits=st->cc + _tmp68->mwo <= st->epw?1: st->cc + 
_tmp68->mw <= st->pw;if(dfits)return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*
f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp68->f,st);else{return((struct Cyc_PP_Out*(*)(
struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp69->f,st);}}}
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){return({
struct Cyc_PP_Doc*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->mwo=Cyc_PP_min(d->mwo,
d2->mwo);_tmp6A->mw=Cyc_PP_min(d->mw,d2->mw);_tmp6A->f=((struct Cyc_Fn_Function*(*)(
struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({struct _tuple5*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->f1=
d;_tmp6B->f2=d2;_tmp6B;}));_tmp6A;});}struct Cyc_PP_Doc*Cyc_PP_oline_doc(){return
Cyc_PP_doc_union(Cyc_PP_nil_doc(),Cyc_PP_line_doc());}static struct Cyc_PP_Out*Cyc_PP_tab_f(
struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*
_tmp6C=_cycalloc_atomic(sizeof(*_tmp6C));_tmp6C->ci=st->cc;_tmp6C->cc=st->cc;
_tmp6C->cl=st->cl;_tmp6C->pw=st->pw;_tmp6C->epw=st->epw;_tmp6C;});return((struct
Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,
st2);}struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){struct Cyc_PP_Doc*d2=({
struct Cyc_PP_Doc*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->mwo=d->mwo;_tmp6D->mw=
d->mw;_tmp6D->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d);
_tmp6D;});return d2;}static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l){if(l == 0)return Cyc_PP_nil_doc();
else{if(l->tl == 0)return pp((void*)l->hd);else{return({struct Cyc_PP_Doc*_tmp6E[4];
_tmp6E[3]=Cyc_PP_ppseq_f(pp,sep,l->tl);_tmp6E[2]=Cyc_PP_oline_doc();_tmp6E[1]=
Cyc_PP_text(sep);_tmp6E[0]=pp((void*)l->hd);Cyc_PP_cat(_tag_arr(_tmp6E,sizeof(
struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseq_f(
pp,sep,l));}struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _tagged_arr sep,struct Cyc_List_List*
l){if(l == 0)return Cyc_PP_nil_doc();else{if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;
else{struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmp6F=l;while(_tmp6F->tl != 0){struct Cyc_List_List*_tmp70=
_tmp6F->tl;_tmp6F->tl=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));
_tmp71->hd=sep2;_tmp71->tl=({struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));
_tmp72->hd=oline;_tmp72->tl=_tmp70;_tmp72;});_tmp71;});_tmp6F=_tmp70;}return Cyc_PP_cats(
l);}}}struct Cyc_PP_Doc*Cyc_PP_seq(struct _tagged_arr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct
Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l){if(l == 0)
return Cyc_PP_nil_doc();else{if(l->tl == 0)return pp((void*)l->hd);else{return({
struct Cyc_PP_Doc*_tmp73[4];_tmp73[3]=Cyc_PP_ppseql_f(pp,sep,l->tl);_tmp73[2]=Cyc_PP_line_doc();
_tmp73[1]=Cyc_PP_text(sep);_tmp73[0]=pp((void*)l->hd);Cyc_PP_cat(_tag_arr(_tmp73,
sizeof(struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*
pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseql_f(
pp,sep,l));}static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _tagged_arr sep,struct Cyc_List_List*
l){if(l == 0)return Cyc_PP_nil_doc();else{if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;
else{return({struct Cyc_PP_Doc*_tmp74[4];_tmp74[3]=Cyc_PP_seql_f(sep,l->tl);
_tmp74[2]=Cyc_PP_line_doc();_tmp74[1]=Cyc_PP_text(sep);_tmp74[0]=(struct Cyc_PP_Doc*)
l->hd;Cyc_PP_cat(_tag_arr(_tmp74,sizeof(struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*
Cyc_PP_seql(struct _tagged_arr sep,struct Cyc_List_List*l0){return Cyc_PP_tab(Cyc_PP_seql_f(
sep,l0));}struct Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr
stop,struct _tagged_arr sep,struct Cyc_List_List*ss){return({struct Cyc_PP_Doc*
_tmp75[3];_tmp75[2]=Cyc_PP_text(stop);_tmp75[1]=Cyc_PP_seq(sep,ss);_tmp75[0]=Cyc_PP_text(
start);Cyc_PP_cat(_tag_arr(_tmp75,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_PP_egroup(struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,
struct Cyc_List_List*ss){if(ss == 0)return Cyc_PP_nil_doc();else{return({struct Cyc_PP_Doc*
_tmp76[3];_tmp76[2]=Cyc_PP_text(stop);_tmp76[1]=Cyc_PP_seq(sep,ss);_tmp76[0]=Cyc_PP_text(
start);Cyc_PP_cat(_tag_arr(_tmp76,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_PP_groupl(struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,
struct Cyc_List_List*ss){return({struct Cyc_PP_Doc*_tmp77[3];_tmp77[2]=Cyc_PP_text(
stop);_tmp77[1]=Cyc_PP_seql(sep,ss);_tmp77[0]=Cyc_PP_text(start);Cyc_PP_cat(
_tag_arr(_tmp77,sizeof(struct Cyc_PP_Doc*),3));});}
