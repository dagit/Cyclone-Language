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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[
8];extern struct _tagged_arr Cyc_List_to_array(struct Cyc_List_List*x);extern struct
Cyc_List_List*Cyc_List_from_array(struct _tagged_arr arr);extern void Cyc_Array_qsort(
int(*)(void**,void**),struct _tagged_arr x,int len);extern void Cyc_Array_msort(int(*)(
void**,void**),struct _tagged_arr x,int len);extern struct _tagged_arr Cyc_Array_from_list(
struct Cyc_List_List*l);extern struct Cyc_List_List*Cyc_Array_to_list(struct
_tagged_arr x);extern struct _tagged_arr Cyc_Array_copy(struct _tagged_arr x);extern
struct _tagged_arr Cyc_Array_map(void*(*f)(void*),struct _tagged_arr x);extern struct
_tagged_arr Cyc_Array_map_c(void*(*f)(void*,void*),void*env,struct _tagged_arr x);
extern void Cyc_Array_imp_map(void*(*f)(void*),struct _tagged_arr x);extern void Cyc_Array_imp_map_c(
void*(*f)(void*,void*),void*env,struct _tagged_arr x);extern unsigned char Cyc_Array_Array_mismatch[
19];extern struct _tagged_arr Cyc_Array_map2(void*(*f)(void*,void*),struct
_tagged_arr x,struct _tagged_arr y);extern void Cyc_Array_app(void*(*f)(void*),struct
_tagged_arr x);extern void Cyc_Array_app_c(void*(*f)(void*,void*),void*env,struct
_tagged_arr x);extern void Cyc_Array_iter(void(*f)(void*),struct _tagged_arr x);
extern void Cyc_Array_iter_c(void(*f)(void*,void*),void*env,struct _tagged_arr x);
extern void Cyc_Array_app2(void*(*f)(void*,void*),struct _tagged_arr x,struct
_tagged_arr y);extern void Cyc_Array_app2_c(void*(*f)(void*,void*,void*),void*env,
struct _tagged_arr x,struct _tagged_arr y);extern void Cyc_Array_iter2(void(*f)(void*,
void*),struct _tagged_arr x,struct _tagged_arr y);extern void Cyc_Array_iter2_c(void(*
f)(void*,void*,void*),void*env,struct _tagged_arr x,struct _tagged_arr y);extern void*
Cyc_Array_fold_left(void*(*f)(void*,void*),void*accum,struct _tagged_arr x);extern
void*Cyc_Array_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*accum,
struct _tagged_arr x);extern void*Cyc_Array_fold_right(void*(*f)(void*,void*),
struct _tagged_arr x,void*accum);extern void*Cyc_Array_fold_right_c(void*(*f)(void*,
void*,void*),void*env,struct _tagged_arr x,void*accum);extern struct _tagged_arr Cyc_Array_rev_copy(
struct _tagged_arr x);extern void Cyc_Array_imp_rev(struct _tagged_arr x);extern int Cyc_Array_forall(
int(*pred)(void*),struct _tagged_arr x);extern int Cyc_Array_forall_c(int(*pred)(
void*,void*),void*env,struct _tagged_arr x);extern int Cyc_Array_exists(int(*pred)(
void*),struct _tagged_arr x);extern int Cyc_Array_exists_c(int(*pred)(void*,void*),
void*env,struct _tagged_arr x);struct _tuple0{void*f1;void*f2;};extern struct
_tagged_arr Cyc_Array_zip(struct _tagged_arr x,struct _tagged_arr y);struct _tuple1{
struct _tagged_arr f1;struct _tagged_arr f2;};extern struct _tuple1 Cyc_Array_split(
struct _tagged_arr x);extern int Cyc_Array_memq(struct _tagged_arr l,void*x);extern int
Cyc_Array_mem(int(*cmp)(void*,void*),struct _tagged_arr l,void*x);extern struct
_tagged_arr Cyc_Array_extract(struct _tagged_arr x,int start,int*len_opt);void Cyc_Array_qsort(
int(*less_eq)(void**,void**),struct _tagged_arr arr,int len){int base_ofs=0;void*
temp;int sp[40];int sp_ofs;int i;int j;int limit_ofs;if((base_ofs < 0? 1: base_ofs + len > 
_get_arr_size(arr,sizeof(void*)))? 1: len < 0){(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp1;_tmp1.tag=Cyc_Core_Invalid_argument;_tmp1.f1=_tag_arr("Array::qsort",
sizeof(unsigned char),13);_tmp1;});_tmp0;}));}limit_ofs=base_ofs + len;sp_ofs=0;
for(0;1;0){if(limit_ofs - base_ofs > 3){temp=*((void**)_check_unknown_subscript(
arr,sizeof(void*),(limit_ofs - base_ofs)/ 2 + base_ofs));*((void**)
_check_unknown_subscript(arr,sizeof(void*),(limit_ofs - base_ofs)/ 2 + base_ofs))=((
void**)arr.curr)[base_ofs];((void**)arr.curr)[base_ofs]=temp;i=base_ofs + 1;j=
limit_ofs - 1;if(less_eq((void**)_untag_arr(_tagged_arr_plus(arr,sizeof(void*),i),
sizeof(void*),1),(void**)_untag_arr(_tagged_arr_plus(arr,sizeof(void*),j),
sizeof(void*),1))> 0){temp=((void**)arr.curr)[i];((void**)arr.curr)[i]=((void**)
arr.curr)[j];((void**)arr.curr)[j]=temp;}if(less_eq((void**)_untag_arr(
_tagged_arr_plus(arr,sizeof(void*),base_ofs),sizeof(void*),1),(void**)_untag_arr(
_tagged_arr_plus(arr,sizeof(void*),j),sizeof(void*),1))> 0){temp=((void**)arr.curr)[
base_ofs];((void**)arr.curr)[base_ofs]=((void**)arr.curr)[j];((void**)arr.curr)[
j]=temp;}if(less_eq((void**)_untag_arr(_tagged_arr_plus(arr,sizeof(void*),i),
sizeof(void*),1),(void**)_untag_arr(_tagged_arr_plus(arr,sizeof(void*),base_ofs),
sizeof(void*),1))> 0){temp=((void**)arr.curr)[i];((void**)arr.curr)[i]=((void**)
arr.curr)[base_ofs];((void**)arr.curr)[base_ofs]=temp;}for(0;1;0){do {i ++;} while (
less_eq((void**)_untag_arr(_tagged_arr_plus(arr,sizeof(void*),i),sizeof(void*),1),(
void**)_untag_arr(_tagged_arr_plus(arr,sizeof(void*),base_ofs),sizeof(void*),1))
< 0);do {j --;} while (less_eq((void**)_untag_arr(_tagged_arr_plus(arr,sizeof(
void*),j),sizeof(void*),1),(void**)_untag_arr(_tagged_arr_plus(arr,sizeof(void*),
base_ofs),sizeof(void*),1))> 0);if(i > j){break;}temp=((void**)arr.curr)[i];((
void**)arr.curr)[i]=((void**)arr.curr)[j];((void**)arr.curr)[j]=temp;;}temp=((
void**)arr.curr)[base_ofs];((void**)arr.curr)[base_ofs]=((void**)arr.curr)[j];((
void**)arr.curr)[j]=temp;if(j - base_ofs > limit_ofs - i){sp[sp_ofs]=base_ofs;sp[
_check_known_subscript_notnull(40,sp_ofs + 1)]=j;base_ofs=i;}else{sp[sp_ofs]=i;sp[
_check_known_subscript_notnull(40,sp_ofs + 1)]=limit_ofs;limit_ofs=j;}sp_ofs +=2;}
else{for((j=base_ofs,i=j + 1);i < limit_ofs;(j=i,i ++)){for(0;less_eq((void**)
_untag_arr(_tagged_arr_plus(arr,sizeof(void*),j),sizeof(void*),1),(void**)
_untag_arr(_tagged_arr_plus(_tagged_arr_plus(arr,sizeof(void*),j),sizeof(void*),
1),sizeof(void*),1))> 0;j --){temp=((void**)arr.curr)[j];((void**)arr.curr)[j]=*((
void**)_check_unknown_subscript(arr,sizeof(void*),j + 1));*((void**)
_check_unknown_subscript(arr,sizeof(void*),j + 1))=temp;if(j == base_ofs){break;}}}
if(sp_ofs != 0){sp_ofs -=2;base_ofs=sp[sp_ofs];limit_ofs=sp[
_check_known_subscript_notnull(40,sp_ofs + 1)];}else{break;}}}}void Cyc_Array_msort(
int(*less_eq)(void**,void**),struct _tagged_arr arr,int len){if(len > _get_arr_size(
arr,sizeof(void*))? 1: len < 0){(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp3;_tmp3.tag=Cyc_Core_Invalid_argument;_tmp3.f1=_tag_arr("Array::msort",
sizeof(unsigned char),13);_tmp3;});_tmp2;}));}{struct _tagged_arr from=({
unsigned int _tmp4=(unsigned int)len;void**_tmp5=(void**)_cycalloc(_check_times(
sizeof(void*),_tmp4));struct _tagged_arr _tmp7=_tag_arr(_tmp5,sizeof(void*),(
unsigned int)len);{unsigned int _tmp6=_tmp4;unsigned int i;for(i=0;i < _tmp6;i ++){
_tmp5[i]=*((void**)_check_unknown_subscript(arr,sizeof(void*),0));}};_tmp7;});
struct _tagged_arr to=arr;struct _tagged_arr swap;int swapped=0;int stepsize;int start;
int lstart;int lend;int rstart;int rend;int dest;for(stepsize=1;stepsize < len;
stepsize=stepsize * 2){swap=from;from=to;to=swap;dest=0;for(start=0;start < len;
start=start + stepsize * 2){lstart=start;rstart=start + stepsize < len? start + 
stepsize: len;lend=rstart;rend=start + stepsize * 2 < len? start + stepsize * 2: len;
while(lstart < lend? rstart < rend: 0){if(less_eq((void**)_untag_arr(
_tagged_arr_plus(from,sizeof(void*),lstart),sizeof(void*),1),(void**)_untag_arr(
_tagged_arr_plus(from,sizeof(void*),rstart),sizeof(void*),1))<= 0){*((void**)
_check_unknown_subscript(to,sizeof(void*),dest ++))=*((void**)
_check_unknown_subscript(from,sizeof(void*),lstart ++));}else{*((void**)
_check_unknown_subscript(to,sizeof(void*),dest ++))=*((void**)
_check_unknown_subscript(from,sizeof(void*),rstart ++));}}while(lstart < lend){*((
void**)_check_unknown_subscript(to,sizeof(void*),dest ++))=*((void**)
_check_unknown_subscript(from,sizeof(void*),lstart ++));}while(rstart < rend){*((
void**)_check_unknown_subscript(to,sizeof(void*),dest ++))=*((void**)
_check_unknown_subscript(from,sizeof(void*),rstart ++));}}}if(swapped){int i=0;
for(0;i < len;i ++){*((void**)_check_unknown_subscript(from,sizeof(void*),i))=*((
void**)_check_unknown_subscript(to,sizeof(void*),i));}}}}struct _tagged_arr Cyc_Array_from_list(
struct Cyc_List_List*x){return Cyc_List_to_array(x);}struct Cyc_List_List*Cyc_Array_to_list(
struct _tagged_arr x){return Cyc_List_from_array(x);}struct _tagged_arr Cyc_Array_copy(
struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));return({
unsigned int _tmp8=(unsigned int)sx;void**_tmp9=(void**)_cycalloc(_check_times(
sizeof(void*),_tmp8));struct _tagged_arr _tmpB=_tag_arr(_tmp9,sizeof(void*),(
unsigned int)sx);{unsigned int _tmpA=_tmp8;unsigned int i;for(i=0;i < _tmpA;i ++){
_tmp9[i]=((void**)x.curr)[(int)i];}};_tmpB;});}struct _tagged_arr Cyc_Array_map(
void*(*f)(void*),struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));
return({unsigned int _tmpC=(unsigned int)sx;void**_tmpD=(void**)_cycalloc(
_check_times(sizeof(void*),_tmpC));struct _tagged_arr _tmpF=_tag_arr(_tmpD,sizeof(
void*),(unsigned int)sx);{unsigned int _tmpE=_tmpC;unsigned int i;for(i=0;i < _tmpE;
i ++){_tmpD[i]=f(((void**)x.curr)[(int)i]);}};_tmpF;});}struct _tagged_arr Cyc_Array_map_c(
void*(*f)(void*,void*),void*env,struct _tagged_arr x){int sx=(int)_get_arr_size(x,
sizeof(void*));return({unsigned int _tmp10=(unsigned int)sx;void**_tmp11=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp10));struct _tagged_arr _tmp13=_tag_arr(
_tmp11,sizeof(void*),(unsigned int)sx);{unsigned int _tmp12=_tmp10;unsigned int i;
for(i=0;i < _tmp12;i ++){_tmp11[i]=f(env,((void**)x.curr)[(int)i]);}};_tmp13;});}
void Cyc_Array_imp_map(void*(*f)(void*),struct _tagged_arr x){int sx=(int)
_get_arr_size(x,sizeof(void*));int i=0;for(0;i < sx;i ++){((void**)x.curr)[i]=f(((
void**)x.curr)[i]);}}void Cyc_Array_imp_map_c(void*(*f)(void*,void*),void*env,
struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));int i=0;for(0;i < sx;
i ++){((void**)x.curr)[i]=f(env,((void**)x.curr)[i]);}}unsigned char Cyc_Array_Array_mismatch[
19]="\000\000\000\000Array_mismatch";struct _tagged_arr Cyc_Array_map2(void*(*f)(
void*,void*),struct _tagged_arr x,struct _tagged_arr y){int sx=(int)_get_arr_size(x,
sizeof(void*));if(sx != _get_arr_size(y,sizeof(void*))){(int)_throw((void*)Cyc_Array_Array_mismatch);}
return({unsigned int _tmp14=(unsigned int)sx;void**_tmp15=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp14));struct _tagged_arr _tmp17=_tag_arr(_tmp15,
sizeof(void*),(unsigned int)sx);{unsigned int _tmp16=_tmp14;unsigned int i;for(i=0;
i < _tmp16;i ++){_tmp15[i]=f(((void**)x.curr)[(int)i],*((void**)
_check_unknown_subscript(y,sizeof(void*),(int)i)));}};_tmp17;});}void Cyc_Array_app(
void*(*f)(void*),struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));
int i=0;for(0;i < sx;i ++){f(((void**)x.curr)[i]);}}void Cyc_Array_app_c(void*(*f)(
void*,void*),void*env,struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));
int i=0;for(0;i < sx;i ++){f(env,((void**)x.curr)[i]);}}void Cyc_Array_iter(void(*f)(
void*),struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));int i=0;for(
0;i < sx;i ++){f(((void**)x.curr)[i]);}}void Cyc_Array_iter_c(void(*f)(void*,void*),
void*env,struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));int i=0;
for(0;i < sx;i ++){f(env,((void**)x.curr)[i]);}}void Cyc_Array_app2(void*(*f)(void*,
void*),struct _tagged_arr x,struct _tagged_arr y){int sx=(int)_get_arr_size(x,sizeof(
void*));if(sx != _get_arr_size(y,sizeof(void*))){(int)_throw((void*)Cyc_Array_Array_mismatch);}{
int i=0;for(0;i < sx;i ++){f(((void**)x.curr)[i],((void**)y.curr)[i]);}}}void Cyc_Array_app2_c(
void*(*f)(void*,void*,void*),void*env,struct _tagged_arr x,struct _tagged_arr y){int
sx=(int)_get_arr_size(x,sizeof(void*));if(sx != _get_arr_size(y,sizeof(void*))){(
int)_throw((void*)Cyc_Array_Array_mismatch);}{int i=0;for(0;i < sx;i ++){f(env,((
void**)x.curr)[i],((void**)y.curr)[i]);}}}void Cyc_Array_iter2(void(*f)(void*,
void*),struct _tagged_arr x,struct _tagged_arr y){int sx=(int)_get_arr_size(x,sizeof(
void*));if(sx != _get_arr_size(y,sizeof(void*))){(int)_throw((void*)Cyc_Array_Array_mismatch);}{
int i=0;for(0;i < sx;i ++){f(((void**)x.curr)[i],((void**)y.curr)[i]);}}}void Cyc_Array_iter2_c(
void(*f)(void*,void*,void*),void*env,struct _tagged_arr x,struct _tagged_arr y){int
sx=(int)_get_arr_size(x,sizeof(void*));if(sx != _get_arr_size(y,sizeof(void*))){(
int)_throw((void*)Cyc_Array_Array_mismatch);}{int i=0;for(0;i < sx;i ++){f(env,((
void**)x.curr)[i],((void**)y.curr)[i]);}}}void*Cyc_Array_fold_left(void*(*f)(
void*,void*),void*accum,struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(
void*));{int i=0;for(0;i < sx;i ++){accum=f(accum,((void**)x.curr)[i]);}}return
accum;}void*Cyc_Array_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*
accum,struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));{int i=0;for(
0;i < sx;i ++){accum=f(env,accum,((void**)x.curr)[i]);}}return accum;}void*Cyc_Array_fold_right(
void*(*f)(void*,void*),struct _tagged_arr x,void*accum){{int i=(int)(_get_arr_size(
x,sizeof(void*))- 1);for(0;i >= 0;i --){accum=f(((void**)x.curr)[i],accum);}}
return accum;}void*Cyc_Array_fold_right_c(void*(*f)(void*,void*,void*),void*env,
struct _tagged_arr x,void*accum){{int i=(int)(_get_arr_size(x,sizeof(void*))- 1);
for(0;i >= 0;i --){accum=f(env,((void**)x.curr)[i],accum);}}return accum;}struct
_tagged_arr Cyc_Array_rev_copy(struct _tagged_arr x){int sx=(int)_get_arr_size(x,
sizeof(void*));int n=sx - 1;return({unsigned int _tmp18=(unsigned int)sx;void**
_tmp19=(void**)_cycalloc(_check_times(sizeof(void*),_tmp18));struct _tagged_arr
_tmp1B=_tag_arr(_tmp19,sizeof(void*),(unsigned int)sx);{unsigned int _tmp1A=
_tmp18;unsigned int i;for(i=0;i < _tmp1A;i ++){_tmp19[i]=*((void**)
_check_unknown_subscript(x,sizeof(void*),(int)(n - i)));}};_tmp1B;});}void Cyc_Array_imp_rev(
struct _tagged_arr x){void*temp;int i=0;int j=(int)(_get_arr_size(x,sizeof(void*))- 
1);while(i < j){temp=((void**)x.curr)[i];((void**)x.curr)[i]=((void**)x.curr)[j];((
void**)x.curr)[j]=temp;i ++;j --;}}int Cyc_Array_forall(int(*pred)(void*),struct
_tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));{int i=0;for(0;i < sx;i ++){
if(! pred(((void**)x.curr)[i])){return 0;}}}return 1;}int Cyc_Array_forall_c(int(*
pred)(void*,void*),void*env,struct _tagged_arr x){int sx=(int)_get_arr_size(x,
sizeof(void*));{int i=0;for(0;i < sx;i ++){if(! pred(env,((void**)x.curr)[i])){
return 0;}}}return 1;}int Cyc_Array_exists(int(*pred)(void*),struct _tagged_arr x){
int sx=(int)_get_arr_size(x,sizeof(void*));{int i=0;for(0;i < sx;i ++){if(pred(((
void**)x.curr)[i])){return 1;}}}return 0;}int Cyc_Array_exists_c(int(*pred)(void*,
void*),void*env,struct _tagged_arr x){int sx=(int)_get_arr_size(x,sizeof(void*));{
int i=0;for(0;i < sx;i ++){if(pred(env,((void**)x.curr)[i])){return 1;}}}return 0;}
struct _tagged_arr Cyc_Array_zip(struct _tagged_arr x,struct _tagged_arr y){int sx=(int)
_get_arr_size(x,sizeof(void*));if(sx != _get_arr_size(y,sizeof(void*))){(int)
_throw((void*)Cyc_Array_Array_mismatch);}return({unsigned int _tmp1C=(
unsigned int)sx;struct _tuple0*_tmp1D=(struct _tuple0*)_cycalloc(_check_times(
sizeof(struct _tuple0),_tmp1C));struct _tagged_arr _tmp20=_tag_arr(_tmp1D,sizeof(
struct _tuple0),(unsigned int)sx);{unsigned int _tmp1E=_tmp1C;unsigned int i;for(i=
0;i < _tmp1E;i ++){_tmp1D[i]=({struct _tuple0 _tmp1F;_tmp1F.f1=((void**)x.curr)[(int)
i];_tmp1F.f2=*((void**)_check_unknown_subscript(y,sizeof(void*),(int)i));_tmp1F;});}};
_tmp20;});}struct _tuple1 Cyc_Array_split(struct _tagged_arr x){int sx=(int)
_get_arr_size(x,sizeof(struct _tuple0));return({struct _tuple1 _tmp21;_tmp21.f1=({
unsigned int _tmp26=(unsigned int)sx;void**_tmp27=(void**)_cycalloc(_check_times(
sizeof(void*),_tmp26));struct _tagged_arr _tmp29=_tag_arr(_tmp27,sizeof(void*),(
unsigned int)sx);{unsigned int _tmp28=_tmp26;unsigned int i;for(i=0;i < _tmp28;i ++){
_tmp27[i]=(((struct _tuple0*)x.curr)[(int)i]).f1;}};_tmp29;});_tmp21.f2=({
unsigned int _tmp22=(unsigned int)sx;void**_tmp23=(void**)_cycalloc(_check_times(
sizeof(void*),_tmp22));struct _tagged_arr _tmp25=_tag_arr(_tmp23,sizeof(void*),(
unsigned int)sx);{unsigned int _tmp24=_tmp22;unsigned int i;for(i=0;i < _tmp24;i ++){
_tmp23[i]=(((struct _tuple0*)x.curr)[(int)i]).f2;}};_tmp25;});_tmp21;});}int Cyc_Array_memq(
struct _tagged_arr l,void*x){int s=(int)_get_arr_size(l,sizeof(void*));{int i=0;for(
0;i < s;i ++){if(((void**)l.curr)[i]== x){return 1;}}}return 0;}int Cyc_Array_mem(int(*
compare)(void*,void*),struct _tagged_arr l,void*x){int s=(int)_get_arr_size(l,
sizeof(void*));{int i=0;for(0;i < s;i ++){if(0 == compare(((void**)l.curr)[i],x)){
return 1;}}}return 0;}struct _tagged_arr Cyc_Array_extract(struct _tagged_arr x,int
start,int*n_opt){int sx=(int)_get_arr_size(x,sizeof(void*));int n=n_opt == 0? sx - 
start:*n_opt;if((start < 0? 1: n <= 0)? 1: start + (n_opt == 0? 0: n)> sx){(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));
_tmp2A[0]=({struct Cyc_Core_Invalid_argument_struct _tmp2B;_tmp2B.tag=Cyc_Core_Invalid_argument;
_tmp2B.f1=_tag_arr("Array::extract",sizeof(unsigned char),15);_tmp2B;});_tmp2A;}));}
return({unsigned int _tmp2C=(unsigned int)n;void**_tmp2D=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp2C));struct _tagged_arr _tmp2F=_tag_arr(_tmp2D,
sizeof(void*),(unsigned int)n);{unsigned int _tmp2E=_tmp2C;unsigned int i;for(i=0;
i < _tmp2E;i ++){_tmp2D[i]=*((void**)_check_unknown_subscript(x,sizeof(void*),(int)(
start + i)));}};_tmp2F;});}
