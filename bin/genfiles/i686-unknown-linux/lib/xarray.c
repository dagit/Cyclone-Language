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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Xarray_Xarray{
struct _tagged_arr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,
int,void*);struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);struct Cyc_Xarray_Xarray*
Cyc_Xarray_create_empty();struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int,void*);
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);int Cyc_Xarray_add_ind(struct
Cyc_Xarray_Xarray*,void*);struct _tagged_arr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*);
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _tagged_arr arr);struct Cyc_Xarray_Xarray*
Cyc_Xarray_append(struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);void Cyc_Xarray_app(
void*(*f)(void*),struct Cyc_Xarray_Xarray*);void Cyc_Xarray_app_c(void*(*f)(void*,
void*),void*,struct Cyc_Xarray_Xarray*);void Cyc_Xarray_iter(void(*f)(void*),
struct Cyc_Xarray_Xarray*);void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*,
struct Cyc_Xarray_Xarray*);struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),
struct Cyc_Xarray_Xarray*);struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(
void*,void*),void*,struct Cyc_Xarray_Xarray*);void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*
xarr);int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*xarr){return xarr->num_elmts;}
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*xarr,int i){if(i < 0?1: i >= xarr->num_elmts)(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Core_Invalid_argument_struct _tmp1;_tmp1.tag=
Cyc_Core_Invalid_argument;_tmp1.f1=_tag_arr("Xarray::get: bad index",sizeof(char),
23);_tmp1;});_tmp0;}));return*((void**)_check_unknown_subscript(xarr->elmts,
sizeof(void*),i));}void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*xarr,int i,void*a){
if(i < 0?1: i >= xarr->num_elmts)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp3;_tmp3.tag=Cyc_Core_Invalid_argument;_tmp3.f1=_tag_arr("Xarray::set: bad index",
sizeof(char),23);_tmp3;});_tmp2;}));*((void**)_check_unknown_subscript(xarr->elmts,
sizeof(void*),i))=a;}struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty(){struct
_tagged_arr x=_tag_arr(({void**_tmp5=_cycalloc(sizeof(void*)* 0);_tmp5;}),sizeof(
void*),0);return({struct Cyc_Xarray_Xarray*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->elmts=
x;_tmp4->num_elmts=0;_tmp4;});}struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int len,
void*a){if(len < 0)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp7;_tmp7.tag=Cyc_Core_Invalid_argument;_tmp7.f1=_tag_arr("xarrays must have a non-negative size buffer",
sizeof(char),45);_tmp7;});_tmp6;}));return({struct Cyc_Xarray_Xarray*_tmp8=
_cycalloc(sizeof(*_tmp8));_tmp8->elmts=({unsigned int _tmp9=(unsigned int)len;
void**_tmpA=(void**)_cycalloc(_check_times(sizeof(void*),_tmp9));struct
_tagged_arr _tmpC=_tag_arr(_tmpA,sizeof(void*),(unsigned int)len);{unsigned int
_tmpB=_tmp9;unsigned int i;for(i=0;i < _tmpB;i ++){_tmpA[i]=a;}}_tmpC;});_tmp8->num_elmts=
0;_tmp8;});}struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int len,void*a){if(len < 
1)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmpD=_cycalloc(
sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Core_Invalid_argument_struct _tmpE;_tmpE.tag=
Cyc_Core_Invalid_argument;_tmpE.f1=_tag_arr("singleton xarray must have size >=1",
sizeof(char),36);_tmpE;});_tmpD;}));{struct Cyc_Xarray_Xarray*x=Cyc_Xarray_create(
len,a);x->num_elmts=1;return x;}}void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,
void*a){if(xarr->num_elmts == _get_arr_size(xarr->elmts,sizeof(void*))){if(xarr->num_elmts
== 0)xarr->elmts=_tag_arr(({unsigned int _tmpF=(unsigned int)10;void**_tmp10=(
void**)_cycalloc(_check_times(sizeof(void*),_tmpF));{unsigned int _tmp11=_tmpF;
unsigned int i;for(i=0;i < _tmp11;i ++){_tmp10[i]=a;}}_tmp10;}),sizeof(void*),(
unsigned int)10);else{struct _tagged_arr newarr=({unsigned int _tmp12=(unsigned int)(
xarr->num_elmts * 2);void**_tmp13=(void**)_cycalloc(_check_times(sizeof(void*),
_tmp12));struct _tagged_arr _tmp15=_tag_arr(_tmp13,sizeof(void*),(unsigned int)(
xarr->num_elmts * 2));{unsigned int _tmp14=_tmp12;unsigned int i;for(i=0;i < _tmp14;
i ++){_tmp13[i]=*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),0));}}
_tmp15;});{int i=1;for(0;i < xarr->num_elmts;++ i){((void**)newarr.curr)[i]=*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),i));}}xarr->elmts=newarr;}}*((
void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),xarr->num_elmts ++))=a;}
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*xarr,void*a){Cyc_Xarray_add(xarr,a);
return xarr->num_elmts - 1;}struct _tagged_arr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*
xarr){if(xarr->num_elmts == 0)return _tag_arr(({void**_tmp16=_cycalloc(sizeof(void*)
* 0);_tmp16;}),sizeof(void*),0);{struct _tagged_arr ans=({unsigned int _tmp17=(
unsigned int)xarr->num_elmts;void**_tmp18=(void**)_cycalloc(_check_times(sizeof(
void*),_tmp17));struct _tagged_arr _tmp1A=_tag_arr(_tmp18,sizeof(void*),(
unsigned int)xarr->num_elmts);{unsigned int _tmp19=_tmp17;unsigned int i;for(i=0;i
< _tmp19;i ++){_tmp18[i]=*((void**)_check_unknown_subscript(xarr->elmts,sizeof(
void*),(int)i));}}_tmp1A;});return ans;}}struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(
struct _tagged_arr arr){if(_get_arr_size(arr,sizeof(void*))== 0)return Cyc_Xarray_create_empty();{
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp1B=_cycalloc(sizeof(*
_tmp1B));_tmp1B->elmts=({unsigned int _tmp1C=_get_arr_size(arr,sizeof(void*));
void**_tmp1D=(void**)_cycalloc(_check_times(sizeof(void*),_tmp1C));struct
_tagged_arr _tmp1F=_tag_arr(_tmp1D,sizeof(void*),_get_arr_size(arr,sizeof(void*)));{
unsigned int _tmp1E=_tmp1C;unsigned int i;for(i=0;i < _tmp1E;i ++){_tmp1D[i]=((void**)
arr.curr)[(int)i];}}_tmp1F;});_tmp1B->num_elmts=(int)_get_arr_size(arr,sizeof(
void*));_tmp1B;});return ans;}}struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct
Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){int newsz=(int)(
_get_arr_size(xarr1->elmts,sizeof(void*))+ _get_arr_size(xarr2->elmts,sizeof(
void*)));if(newsz == 0)return Cyc_Xarray_create_empty();{void*init=_get_arr_size(
xarr1->elmts,sizeof(void*))== 0?*((void**)_check_unknown_subscript(xarr2->elmts,
sizeof(void*),0)):*((void**)_check_unknown_subscript(xarr1->elmts,sizeof(void*),
0));struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp20=_cycalloc(
sizeof(*_tmp20));_tmp20->elmts=({unsigned int _tmp21=(unsigned int)newsz;void**
_tmp22=(void**)_cycalloc(_check_times(sizeof(void*),_tmp21));struct _tagged_arr
_tmp24=_tag_arr(_tmp22,sizeof(void*),(unsigned int)newsz);{unsigned int _tmp23=
_tmp21;unsigned int i;for(i=0;i < _tmp23;i ++){_tmp22[i]=init;}}_tmp24;});_tmp20->num_elmts=
0;_tmp20;});{int i=0;for(0;i < xarr1->num_elmts;++ i){Cyc_Xarray_add(ans,*((void**)
_check_unknown_subscript(xarr1->elmts,sizeof(void*),i)));}}{int i=0;for(0;i < 
xarr2->num_elmts;++ i){Cyc_Xarray_add(ans,*((void**)_check_unknown_subscript(
xarr2->elmts,sizeof(void*),i)));}}return ans;}}void Cyc_Xarray_app(void*(*f)(void*),
struct Cyc_Xarray_Xarray*xarr){int i=0;for(0;i < xarr->num_elmts;++ i){f(*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}void Cyc_Xarray_app_c(
void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){int i=0;for(0;i < 
xarr->num_elmts;++ i){f(env,*((void**)_check_unknown_subscript(xarr->elmts,
sizeof(void*),i)));}}void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*
xarr){int i=0;for(0;i < xarr->num_elmts;++ i){f(*((void**)_check_unknown_subscript(
xarr->elmts,sizeof(void*),i)));}}void Cyc_Xarray_iter_c(void(*f)(void*,void*),
void*env,struct Cyc_Xarray_Xarray*xarr){int i=0;for(0;i < xarr->num_elmts;++ i){f(
env,*((void**)_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}struct
Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_create_empty();{struct Cyc_Xarray_Xarray*
ans=({struct Cyc_Xarray_Xarray*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->elmts=({
unsigned int _tmp26=_get_arr_size(xarr->elmts,sizeof(void*));void**_tmp27=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp26));struct _tagged_arr _tmp29=_tag_arr(
_tmp27,sizeof(void*),_get_arr_size(xarr->elmts,sizeof(void*)));{unsigned int
_tmp28=_tmp26;unsigned int i;for(i=0;i < _tmp28;i ++){_tmp27[i]=f(*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),0)));}}_tmp29;});_tmp25->num_elmts=
xarr->num_elmts;_tmp25;});{int i=1;for(0;i < xarr->num_elmts;++ i){*((void**)
_check_unknown_subscript(ans->elmts,sizeof(void*),i))=f(*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}return ans;}}struct Cyc_Xarray_Xarray*
Cyc_Xarray_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_create_empty();{struct Cyc_Xarray_Xarray*
ans=({struct Cyc_Xarray_Xarray*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->elmts=({
unsigned int _tmp2B=_get_arr_size(xarr->elmts,sizeof(void*));void**_tmp2C=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp2B));struct _tagged_arr _tmp2E=_tag_arr(
_tmp2C,sizeof(void*),_get_arr_size(xarr->elmts,sizeof(void*)));{unsigned int
_tmp2D=_tmp2B;unsigned int i;for(i=0;i < _tmp2D;i ++){_tmp2C[i]=f(env,*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),0)));}}_tmp2E;});_tmp2A->num_elmts=
xarr->num_elmts;_tmp2A;});{int i=1;for(0;i < xarr->num_elmts;++ i){*((void**)
_check_unknown_subscript(ans->elmts,sizeof(void*),i))=f(env,*((void**)
_check_unknown_subscript(xarr->elmts,sizeof(void*),i)));}}return ans;}}void Cyc_Xarray_reuse(
struct Cyc_Xarray_Xarray*xarr){xarr->num_elmts=0;}
