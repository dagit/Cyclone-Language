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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[8];struct Cyc_Splay_node;
struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};struct Cyc_Splay_Node_struct{int
tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*
left;void*right;};int Cyc_Splay_rsplay(struct _RegionHandle*,int(*f)(void*,void*),
void*,void*);int Cyc_Splay_splay(int(*f)(void*,void*),void*,void*);static void Cyc_Splay_rotate_left(
struct _RegionHandle*r,struct Cyc_Splay_noderef*nr){struct Cyc_Splay_node*_tmp0=nr->v;
void*_tmp1=(void*)_tmp0->left;struct Cyc_Splay_noderef*_tmp2;_LL1: if(_tmp1 <= (
void*)1?1:*((int*)_tmp1)!= 0)goto _LL3;_tmp2=((struct Cyc_Splay_Node_struct*)_tmp1)->f1;
_LL2: {struct Cyc_Splay_node*_tmp3=_tmp2->v;struct Cyc_Splay_Node_struct*_tmp4=({
struct Cyc_Splay_Node_struct*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6[0]=({
struct Cyc_Splay_Node_struct _tmp7;_tmp7.tag=0;_tmp7.f1=({struct Cyc_Splay_noderef*
_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->v=({struct Cyc_Splay_node*_tmp9=
_region_malloc(r,sizeof(*_tmp9));_tmp9->key=(void*)((void*)_tmp0->key);_tmp9->data=(
void*)((void*)_tmp0->data);_tmp9->left=(void*)((void*)_tmp3->right);_tmp9->right=(
void*)((void*)_tmp0->right);_tmp9;});_tmp8;});_tmp7;});_tmp6;});nr->v=({struct
Cyc_Splay_node*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->key=(void*)((void*)
_tmp3->key);_tmp5->data=(void*)((void*)_tmp3->data);_tmp5->left=(void*)((void*)
_tmp3->left);_tmp5->right=(void*)((void*)_tmp4);_tmp5;});goto _LL0;}_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmpA=_cycalloc(
sizeof(*_tmpA));_tmpA[0]=({struct Cyc_Core_Invalid_argument_struct _tmpB;_tmpB.tag=
Cyc_Core_Invalid_argument;_tmpB.f1=_tag_arr("Splay::rotate_left",sizeof(char),19);
_tmpB;});_tmpA;}));_LL0:;}static void Cyc_Splay_rotate_right(struct _RegionHandle*r,
struct Cyc_Splay_noderef*nr){struct Cyc_Splay_node*_tmpC=nr->v;void*_tmpD=(void*)
_tmpC->right;struct Cyc_Splay_noderef*_tmpE;_LL6: if(_tmpD <= (void*)1?1:*((int*)
_tmpD)!= 0)goto _LL8;_tmpE=((struct Cyc_Splay_Node_struct*)_tmpD)->f1;_LL7: {
struct Cyc_Splay_node*_tmpF=_tmpE->v;struct Cyc_Splay_Node_struct*_tmp10=({struct
Cyc_Splay_Node_struct*_tmp12=_region_malloc(r,sizeof(*_tmp12));_tmp12[0]=({
struct Cyc_Splay_Node_struct _tmp13;_tmp13.tag=0;_tmp13.f1=({struct Cyc_Splay_noderef*
_tmp14=_region_malloc(r,sizeof(*_tmp14));_tmp14->v=({struct Cyc_Splay_node*_tmp15=
_region_malloc(r,sizeof(*_tmp15));_tmp15->key=(void*)((void*)_tmpC->key);_tmp15->data=(
void*)((void*)_tmpC->data);_tmp15->left=(void*)((void*)_tmpC->left);_tmp15->right=(
void*)((void*)_tmpF->left);_tmp15;});_tmp14;});_tmp13;});_tmp12;});nr->v=({
struct Cyc_Splay_node*_tmp11=_region_malloc(r,sizeof(*_tmp11));_tmp11->key=(void*)((
void*)_tmpF->key);_tmp11->data=(void*)((void*)_tmpF->data);_tmp11->left=(void*)((
void*)_tmp10);_tmp11->right=(void*)((void*)_tmpF->right);_tmp11;});goto _LL5;}
_LL8:;_LL9:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp16=
_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp17;_tmp17.tag=Cyc_Core_Invalid_argument;_tmp17.f1=_tag_arr("Splay::rotate_right",
sizeof(char),20);_tmp17;});_tmp16;}));_LL5:;}struct _tuple0{void*f1;struct Cyc_Splay_noderef*
f2;};static void Cyc_Splay_lift(struct _RegionHandle*r,struct Cyc_List_List*dnl){
while(dnl != 0){if(dnl->tl == 0){{struct _tuple0*_tmp18=(struct _tuple0*)dnl->hd;
struct _tuple0 _tmp19;void*_tmp1A;struct Cyc_Splay_noderef*_tmp1B;struct _tuple0
_tmp1C;void*_tmp1D;struct Cyc_Splay_noderef*_tmp1E;_LLB: _tmp19=*_tmp18;_tmp1A=
_tmp19.f1;if((int)_tmp1A != 0)goto _LLD;_tmp1B=_tmp19.f2;_LLC: Cyc_Splay_rotate_left(
r,_tmp1B);goto _LLA;_LLD: _tmp1C=*_tmp18;_tmp1D=_tmp1C.f1;if((int)_tmp1D != 1)goto
_LLA;_tmp1E=_tmp1C.f2;_LLE: Cyc_Splay_rotate_right(r,_tmp1E);goto _LLA;_LLA:;}
return;}{struct _tuple0 _tmp20;void*_tmp21;struct Cyc_Splay_noderef*_tmp22;struct
_tuple0*_tmp1F=(struct _tuple0*)dnl->hd;_tmp20=*_tmp1F;_tmp21=_tmp20.f1;_tmp22=
_tmp20.f2;{struct _tuple0 _tmp24;void*_tmp25;struct Cyc_Splay_noderef*_tmp26;struct
_tuple0*_tmp23=(struct _tuple0*)((struct Cyc_List_List*)_check_null(dnl->tl))->hd;
_tmp24=*_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;dnl=((struct Cyc_List_List*)
_check_null(dnl->tl))->tl;{void*_tmp27=_tmp21;_LL10: if((int)_tmp27 != 0)goto _LL12;
_LL11:{void*_tmp28=_tmp25;_LL15: if((int)_tmp28 != 0)goto _LL17;_LL16: Cyc_Splay_rotate_left(
r,_tmp26);Cyc_Splay_rotate_left(r,_tmp26);goto _LL14;_LL17: if((int)_tmp28 != 1)
goto _LL14;_LL18: Cyc_Splay_rotate_left(r,_tmp22);Cyc_Splay_rotate_right(r,_tmp26);
goto _LL14;_LL14:;}goto _LLF;_LL12: if((int)_tmp27 != 1)goto _LLF;_LL13:{void*_tmp29=
_tmp25;_LL1A: if((int)_tmp29 != 0)goto _LL1C;_LL1B: Cyc_Splay_rotate_right(r,_tmp22);
Cyc_Splay_rotate_left(r,_tmp26);goto _LL19;_LL1C: if((int)_tmp29 != 1)goto _LL19;
_LL1D: Cyc_Splay_rotate_right(r,_tmp26);Cyc_Splay_rotate_right(r,_tmp26);goto
_LL19;_LL19:;}goto _LLF;_LLF:;}}}}}int Cyc_Splay_rsplay(struct _RegionHandle*r,int(*
reln)(void*,void*),void*reln_first_arg,void*tree){struct _RegionHandle _tmp2A=
_new_region("temp");struct _RegionHandle*temp=& _tmp2A;_push_region(temp);{struct
Cyc_List_List*path=0;while(1){void*_tmp2B=tree;struct Cyc_Splay_noderef*_tmp2C;
_LL1F: if(_tmp2B <= (void*)1?1:*((int*)_tmp2B)!= 0)goto _LL21;_tmp2C=((struct Cyc_Splay_Node_struct*)
_tmp2B)->f1;_LL20: {struct Cyc_Splay_node*_tmp2D=_tmp2C->v;int _tmp2E=reln(
reln_first_arg,(void*)_tmp2D->key);if(_tmp2E == 0){Cyc_Splay_lift(r,path);{int
_tmp2F=1;_npop_handler(0);return _tmp2F;}}else{if(_tmp2E < 0){path=({struct Cyc_List_List*
_tmp30=_region_malloc(temp,sizeof(*_tmp30));_tmp30->hd=({struct _tuple0*_tmp31=
_region_malloc(temp,sizeof(*_tmp31));_tmp31->f1=(void*)0;_tmp31->f2=_tmp2C;
_tmp31;});_tmp30->tl=path;_tmp30;});tree=(void*)_tmp2D->left;}else{path=({struct
Cyc_List_List*_tmp32=_region_malloc(temp,sizeof(*_tmp32));_tmp32->hd=({struct
_tuple0*_tmp33=_region_malloc(temp,sizeof(*_tmp33));_tmp33->f1=(void*)1;_tmp33->f2=
_tmp2C;_tmp33;});_tmp32->tl=path;_tmp32;});tree=(void*)_tmp2D->right;}}goto _LL1E;}
_LL21: if((int)_tmp2B != 0)goto _LL1E;_LL22: if(path != 0)Cyc_Splay_lift(r,((struct
Cyc_List_List*)_check_null(path))->tl);{int _tmp34=0;_npop_handler(0);return
_tmp34;}_LL1E:;}};_pop_region(temp);}int Cyc_Splay_splay(int(*reln)(void*,void*),
void*reln_first_arg,void*tree){return Cyc_Splay_rsplay(Cyc_Core_heap_region,reln,
reln_first_arg,tree);}
