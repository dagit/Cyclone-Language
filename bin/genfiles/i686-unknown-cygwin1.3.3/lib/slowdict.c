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
8];struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};struct
Cyc_Splay_Node_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{
void*key;void*data;void*left;void*right;};extern int Cyc_Splay_splay(int(*f)(void*,
void*),void*,void*);struct Cyc_SlowDict_Dict;extern unsigned char Cyc_SlowDict_Present[
12];extern unsigned char Cyc_SlowDict_Absent[11];extern struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(
int(*cmp)(void*,void*));extern int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d);
extern int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*k);extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*k,void*v);extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*k,void*v);extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*l);extern
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*cmp)(void*,void*),void*k,
void*v);extern void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*k);extern
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*k);
extern struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*
k);extern struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*
d,void*k);extern void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*
d,void*accum);extern void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_SlowDict_Dict*d,void*accum);extern void Cyc_SlowDict_app(void*(*
f)(void*,void*),struct Cyc_SlowDict_Dict*d);extern void Cyc_SlowDict_app_c(void*(*f)(
void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);extern void Cyc_SlowDict_iter(
void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d);extern void Cyc_SlowDict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d);extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d);
struct _tuple0{void*f1;void*f2;};extern struct _tuple0*Cyc_SlowDict_choose(struct
Cyc_SlowDict_Dict*d);extern struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*
d);unsigned char Cyc_SlowDict_Absent[11]="\000\000\000\000Absent";unsigned char
Cyc_SlowDict_Present[12]="\000\000\000\000Present";struct Cyc_SlowDict_Dict{int(*
reln)(void*,void*);void*tree;};struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*
comp)(void*,void*)){void*t=(void*)0;return({struct Cyc_SlowDict_Dict*_tmp0=
_cycalloc(sizeof(*_tmp0));_tmp0->reln=comp;_tmp0->tree=(void*)t;_tmp0;});}int Cyc_SlowDict_is_empty(
struct Cyc_SlowDict_Dict*d){void*_tmp1=(void*)d->tree;_LL1: if((int)_tmp1 == 0){
goto _LL2;}else{goto _LL3;}_LL3: if((unsigned int)_tmp1 > 1?*((int*)_tmp1)== 0: 0){
goto _LL4;}else{goto _LL0;}_LL2: return 1;_LL4: return 0;_LL0:;}int Cyc_SlowDict_member(
struct Cyc_SlowDict_Dict*d,void*key){return Cyc_Splay_splay(d->reln,key,(void*)d->tree);}
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,
void*data){void*newleft=(void*)0;void*newright=(void*)0;if(Cyc_Splay_splay(d->reln,
key,(void*)d->tree)){void*_tmp2=(void*)d->tree;struct Cyc_Splay_noderef*_tmp3;
_LL6: if((unsigned int)_tmp2 > 1?*((int*)_tmp2)== 0: 0){_LLA: _tmp3=((struct Cyc_Splay_Node_struct*)
_tmp2)->f1;goto _LL7;}else{goto _LL8;}_LL8: goto _LL9;_LL7: newleft=(void*)(_tmp3->v)->left;
newright=(void*)(_tmp3->v)->right;goto _LL5;_LL9: goto _LL5;_LL5:;}else{void*_tmp4=(
void*)d->tree;struct Cyc_Splay_noderef*_tmp5;_LLC: if((unsigned int)_tmp4 > 1?*((
int*)_tmp4)== 0: 0){_LL10: _tmp5=((struct Cyc_Splay_Node_struct*)_tmp4)->f1;goto
_LLD;}else{goto _LLE;}_LLE: if((int)_tmp4 == 0){goto _LLF;}else{goto _LLB;}_LLD: {
struct Cyc_Splay_node*_tmp6=_tmp5->v;if((d->reln)(key,(void*)_tmp6->key)< 0){
newleft=(void*)_tmp6->left;newright=(void*)({struct Cyc_Splay_Node_struct*_tmp7=
_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Splay_Node_struct _tmp8;_tmp8.tag=
0;_tmp8.f1=({struct Cyc_Splay_noderef*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->v=({
struct Cyc_Splay_node*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->key=(void*)((void*)
_tmp6->key);_tmpA->data=(void*)((void*)_tmp6->data);_tmpA->left=(void*)((void*)0);
_tmpA->right=(void*)((void*)_tmp6->right);_tmpA;});_tmp9;});_tmp8;});_tmp7;});}
else{newleft=(void*)({struct Cyc_Splay_Node_struct*_tmpB=_cycalloc(sizeof(*_tmpB));
_tmpB[0]=({struct Cyc_Splay_Node_struct _tmpC;_tmpC.tag=0;_tmpC.f1=({struct Cyc_Splay_noderef*
_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->v=({struct Cyc_Splay_node*_tmpE=_cycalloc(
sizeof(*_tmpE));_tmpE->key=(void*)((void*)_tmp6->key);_tmpE->data=(void*)((void*)
_tmp6->data);_tmpE->left=(void*)((void*)_tmp6->left);_tmpE->right=(void*)((void*)
0);_tmpE;});_tmpD;});_tmpC;});_tmpB;});newright=(void*)_tmp6->right;}goto _LLB;}
_LLF: goto _LLB;_LLB:;}return({struct Cyc_SlowDict_Dict*_tmpF=_cycalloc(sizeof(*
_tmpF));_tmpF->reln=d->reln;_tmpF->tree=(void*)((void*)({struct Cyc_Splay_Node_struct*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({struct Cyc_Splay_Node_struct _tmp11;
_tmp11.tag=0;_tmp11.f1=({struct Cyc_Splay_noderef*_tmp12=_cycalloc(sizeof(*_tmp12));
_tmp12->v=({struct Cyc_Splay_node*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->key=(
void*)key;_tmp13->data=(void*)data;_tmp13->left=(void*)newleft;_tmp13->right=(
void*)newright;_tmp13;});_tmp12;});_tmp11;});_tmp10;}));_tmpF;});}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*key,void*data){if(Cyc_Splay_splay(
d->reln,key,(void*)d->tree)){(int)_throw((void*)Cyc_SlowDict_Present);}return Cyc_SlowDict_insert(
d,key,data);}struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*
d,struct Cyc_List_List*kds){for(0;kds != 0;kds=kds->tl){d=Cyc_SlowDict_insert(d,(*((
struct _tuple0*)kds->hd)).f1,(*((struct _tuple0*)kds->hd)).f2);}return d;}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){return({
struct Cyc_SlowDict_Dict*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->reln=comp;
_tmp14->tree=(void*)((void*)({struct Cyc_Splay_Node_struct*_tmp15=_cycalloc(
sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Splay_Node_struct _tmp16;_tmp16.tag=0;
_tmp16.f1=({struct Cyc_Splay_noderef*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->v=({
struct Cyc_Splay_node*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->key=(void*)key;
_tmp18->data=(void*)data;_tmp18->left=(void*)((void*)0);_tmp18->right=(void*)((
void*)0);_tmp18;});_tmp17;});_tmp16;});_tmp15;}));_tmp14;});}void*Cyc_SlowDict_lookup(
struct Cyc_SlowDict_Dict*d,void*key){if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){
void*_tmp19=(void*)d->tree;struct Cyc_Splay_noderef*_tmp1A;_LL12: if((unsigned int)
_tmp19 > 1?*((int*)_tmp19)== 0: 0){_LL16: _tmp1A=((struct Cyc_Splay_Node_struct*)
_tmp19)->f1;goto _LL13;}else{goto _LL14;}_LL14: if((int)_tmp19 == 0){goto _LL15;}
else{goto _LL11;}_LL13: return(void*)(_tmp1A->v)->data;_LL15:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({
struct Cyc_Core_Impossible_struct _tmp1C;_tmp1C.tag=Cyc_Core_Impossible;_tmp1C.f1=
_tag_arr("Dict::lookup",sizeof(unsigned char),13);_tmp1C;});_tmp1B;}));_LL11:;}(
int)_throw((void*)Cyc_SlowDict_Absent);}struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(
struct Cyc_SlowDict_Dict*d,void*key){if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){
void*_tmp1D=(void*)d->tree;struct Cyc_Splay_noderef*_tmp1E;_LL18: if((unsigned int)
_tmp1D > 1?*((int*)_tmp1D)== 0: 0){_LL1C: _tmp1E=((struct Cyc_Splay_Node_struct*)
_tmp1D)->f1;goto _LL19;}else{goto _LL1A;}_LL1A: if((int)_tmp1D == 0){goto _LL1B;}
else{goto _LL17;}_LL19: return({struct Cyc_Core_Opt*_tmp1F=_cycalloc(sizeof(*_tmp1F));
_tmp1F->v=(void*)((void*)(_tmp1E->v)->data);_tmp1F;});_LL1B:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({
struct Cyc_Core_Impossible_struct _tmp21;_tmp21.tag=Cyc_Core_Impossible;_tmp21.f1=
_tag_arr("Dict::lookup",sizeof(unsigned char),13);_tmp21;});_tmp20;}));_LL17:;}
return 0;}static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){if(Cyc_Splay_splay(d->reln,
key,(void*)d->tree)){void*_tmp22=(void*)d->tree;struct Cyc_Splay_noderef*_tmp23;
_LL1E: if((int)_tmp22 == 0){goto _LL1F;}else{goto _LL20;}_LL20: if((unsigned int)
_tmp22 > 1?*((int*)_tmp22)== 0: 0){_LL22: _tmp23=((struct Cyc_Splay_Node_struct*)
_tmp22)->f1;goto _LL21;}else{goto _LL1D;}_LL1F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Core_Impossible_struct
_tmp25;_tmp25.tag=Cyc_Core_Impossible;_tmp25.f1=_tag_arr("Dict::lookup",sizeof(
unsigned char),13);_tmp25;});_tmp24;}));_LL21: {struct Cyc_Splay_node*n=_tmp23->v;
void*_tmp26=(void*)n->left;struct Cyc_Splay_noderef*_tmp27;_LL24: if((int)_tmp26 == 
0){goto _LL25;}else{goto _LL26;}_LL26: if((unsigned int)_tmp26 > 1?*((int*)_tmp26)== 
0: 0){_LL28: _tmp27=((struct Cyc_Splay_Node_struct*)_tmp26)->f1;goto _LL27;}else{
goto _LL23;}_LL25: return({struct Cyc_SlowDict_Dict*_tmp28=_cycalloc(sizeof(*_tmp28));
_tmp28->reln=d->reln;_tmp28->tree=(void*)((void*)n->right);_tmp28;});_LL27: {
void*_tmp29=(void*)n->right;struct Cyc_Splay_noderef*_tmp2A;_LL2A: if((int)_tmp29
== 0){goto _LL2B;}else{goto _LL2C;}_LL2C: if((unsigned int)_tmp29 > 1?*((int*)_tmp29)
== 0: 0){_LL2E: _tmp2A=((struct Cyc_Splay_Node_struct*)_tmp29)->f1;goto _LL2D;}else{
goto _LL29;}_LL2B: return({struct Cyc_SlowDict_Dict*_tmp2B=_cycalloc(sizeof(*_tmp2B));
_tmp2B->reln=d->reln;_tmp2B->tree=(void*)((void*)n->left);_tmp2B;});_LL2D: Cyc_Splay_splay(
Cyc_SlowDict_get_largest,key,(void*)n->left);{struct Cyc_Splay_node*newtop=_tmp27->v;
return({struct Cyc_SlowDict_Dict*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->reln=d->reln;
_tmp2C->tree=(void*)((void*)({struct Cyc_Splay_Node_struct*_tmp2D=_cycalloc(
sizeof(*_tmp2D));_tmp2D[0]=({struct Cyc_Splay_Node_struct _tmp2E;_tmp2E.tag=0;
_tmp2E.f1=({struct Cyc_Splay_noderef*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->v=({
struct Cyc_Splay_node*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->key=(void*)((void*)
newtop->key);_tmp30->data=(void*)((void*)newtop->data);_tmp30->left=(void*)((
void*)newtop->left);_tmp30->right=(void*)((void*)n->right);_tmp30;});_tmp2F;});
_tmp2E;});_tmp2D;}));_tmp2C;});}_LL29:;}_LL23:;}_LL1D:;}else{return d;}}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*key){struct Cyc_SlowDict_Dict*
_tmp31=Cyc_SlowDict_delete(d,key);if(d == _tmp31){(int)_throw((void*)Cyc_SlowDict_Absent);}
return _tmp31;}static void*Cyc_SlowDict_fold_tree(void*(*f)(void*,void*,void*),
void*t,void*accum){void*_tmp32=t;struct Cyc_Splay_noderef*_tmp33;_LL30: if((int)
_tmp32 == 0){goto _LL31;}else{goto _LL32;}_LL32: if((unsigned int)_tmp32 > 1?*((int*)
_tmp32)== 0: 0){_LL34: _tmp33=((struct Cyc_Splay_Node_struct*)_tmp32)->f1;goto _LL33;}
else{goto _LL2F;}_LL31: return accum;_LL33: {struct Cyc_Splay_node*n=_tmp33->v;
return f((void*)n->key,(void*)n->data,Cyc_SlowDict_fold_tree(f,(void*)n->left,Cyc_SlowDict_fold_tree(
f,(void*)n->right,accum)));}_LL2F:;}void*Cyc_SlowDict_fold(void*(*f)(void*,void*,
void*),struct Cyc_SlowDict_Dict*d,void*accum){return Cyc_SlowDict_fold_tree(f,(
void*)d->tree,accum);}static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,
void*,void*),void*env,void*t,void*accum){void*_tmp34=t;struct Cyc_Splay_noderef*
_tmp35;_LL36: if((int)_tmp34 == 0){goto _LL37;}else{goto _LL38;}_LL38: if((
unsigned int)_tmp34 > 1?*((int*)_tmp34)== 0: 0){_LL3A: _tmp35=((struct Cyc_Splay_Node_struct*)
_tmp34)->f1;goto _LL39;}else{goto _LL35;}_LL37: return accum;_LL39: {struct Cyc_Splay_node*
n=_tmp35->v;return f(env,(void*)n->key,(void*)n->data,Cyc_SlowDict_fold_tree_c(f,
env,(void*)n->left,Cyc_SlowDict_fold_tree_c(f,env,(void*)n->right,accum)));}
_LL35:;}void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,
struct Cyc_SlowDict_Dict*dict,void*accum){return Cyc_SlowDict_fold_tree_c(f,env,(
void*)dict->tree,accum);}static void Cyc_SlowDict_app_tree(void*(*f)(void*,void*),
void*t){void*_tmp36=t;struct Cyc_Splay_noderef*_tmp37;_LL3C: if((int)_tmp36 == 0){
goto _LL3D;}else{goto _LL3E;}_LL3E: if((unsigned int)_tmp36 > 1?*((int*)_tmp36)== 0:
0){_LL40: _tmp37=((struct Cyc_Splay_Node_struct*)_tmp36)->f1;goto _LL3F;}else{goto
_LL3B;}_LL3D: goto _LL3B;_LL3F: {struct Cyc_Splay_node*_tmp38=_tmp37->v;Cyc_SlowDict_app_tree(
f,(void*)_tmp38->left);f((void*)_tmp38->key,(void*)_tmp38->data);Cyc_SlowDict_app_tree(
f,(void*)_tmp38->right);goto _LL3B;}_LL3B:;}void Cyc_SlowDict_app(void*(*f)(void*,
void*),struct Cyc_SlowDict_Dict*d){Cyc_SlowDict_app_tree(f,(void*)d->tree);}
static void Cyc_SlowDict_iter_tree(void(*f)(void*,void*),void*t){void*_tmp39=t;
struct Cyc_Splay_noderef*_tmp3A;_LL42: if((int)_tmp39 == 0){goto _LL43;}else{goto
_LL44;}_LL44: if((unsigned int)_tmp39 > 1?*((int*)_tmp39)== 0: 0){_LL46: _tmp3A=((
struct Cyc_Splay_Node_struct*)_tmp39)->f1;goto _LL45;}else{goto _LL41;}_LL43: goto
_LL41;_LL45: {struct Cyc_Splay_node*n=_tmp3A->v;Cyc_SlowDict_iter_tree(f,(void*)n->left);
f((void*)n->key,(void*)n->data);Cyc_SlowDict_iter_tree(f,(void*)n->right);goto
_LL41;}_LL41:;}void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*
d){Cyc_SlowDict_iter_tree(f,(void*)d->tree);}static void Cyc_SlowDict_app_tree_c(
void*(*f)(void*,void*,void*),void*env,void*t){void*_tmp3B=t;struct Cyc_Splay_noderef*
_tmp3C;_LL48: if((int)_tmp3B == 0){goto _LL49;}else{goto _LL4A;}_LL4A: if((
unsigned int)_tmp3B > 1?*((int*)_tmp3B)== 0: 0){_LL4C: _tmp3C=((struct Cyc_Splay_Node_struct*)
_tmp3B)->f1;goto _LL4B;}else{goto _LL47;}_LL49: goto _LL47;_LL4B: {struct Cyc_Splay_node*
n=_tmp3C->v;Cyc_SlowDict_app_tree_c(f,env,(void*)n->left);f(env,(void*)n->key,(
void*)n->data);Cyc_SlowDict_app_tree_c(f,env,(void*)n->right);goto _LL47;}_LL47:;}
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*
d){Cyc_SlowDict_app_tree_c(f,env,(void*)d->tree);}static void Cyc_SlowDict_iter_tree_c(
void(*f)(void*,void*,void*),void*env,void*t){void*_tmp3D=t;struct Cyc_Splay_noderef*
_tmp3E;_LL4E: if((int)_tmp3D == 0){goto _LL4F;}else{goto _LL50;}_LL50: if((
unsigned int)_tmp3D > 1?*((int*)_tmp3D)== 0: 0){_LL52: _tmp3E=((struct Cyc_Splay_Node_struct*)
_tmp3D)->f1;goto _LL51;}else{goto _LL4D;}_LL4F: goto _LL4D;_LL51: {struct Cyc_Splay_node*
n=_tmp3E->v;Cyc_SlowDict_iter_tree_c(f,env,(void*)n->left);f(env,(void*)n->key,(
void*)n->data);Cyc_SlowDict_iter_tree_c(f,env,(void*)n->right);goto _LL4D;}_LL4D:;}
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*
d){Cyc_SlowDict_iter_tree_c(f,env,(void*)d->tree);}static void*Cyc_SlowDict_map_tree(
void*(*f)(void*),void*t){void*_tmp3F=t;struct Cyc_Splay_noderef*_tmp40;_LL54: if((
int)_tmp3F == 0){goto _LL55;}else{goto _LL56;}_LL56: if((unsigned int)_tmp3F > 1?*((
int*)_tmp3F)== 0: 0){_LL58: _tmp40=((struct Cyc_Splay_Node_struct*)_tmp3F)->f1;goto
_LL57;}else{goto _LL53;}_LL55: return(void*)0;_LL57: {struct Cyc_Splay_node*_tmp41=
_tmp40->v;return(void*)({struct Cyc_Splay_Node_struct*_tmp42=_cycalloc(sizeof(*
_tmp42));_tmp42[0]=({struct Cyc_Splay_Node_struct _tmp43;_tmp43.tag=0;_tmp43.f1=({
struct Cyc_Splay_noderef*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->v=({struct Cyc_Splay_node*
_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->key=(void*)((void*)_tmp41->key);_tmp45->data=(
void*)f((void*)_tmp41->data);_tmp45->left=(void*)Cyc_SlowDict_map_tree(f,(void*)
_tmp41->left);_tmp45->right=(void*)Cyc_SlowDict_map_tree(f,(void*)_tmp41->right);
_tmp45;});_tmp44;});_tmp43;});_tmp42;});}_LL53:;}struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(
void*(*f)(void*),struct Cyc_SlowDict_Dict*d){return({struct Cyc_SlowDict_Dict*
_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->reln=d->reln;_tmp46->tree=(void*)Cyc_SlowDict_map_tree(
f,(void*)d->tree);_tmp46;});}static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,
void*),void*env,void*t){void*_tmp47=t;struct Cyc_Splay_noderef*_tmp48;_LL5A: if((
int)_tmp47 == 0){goto _LL5B;}else{goto _LL5C;}_LL5C: if((unsigned int)_tmp47 > 1?*((
int*)_tmp47)== 0: 0){_LL5E: _tmp48=((struct Cyc_Splay_Node_struct*)_tmp47)->f1;goto
_LL5D;}else{goto _LL59;}_LL5B: return(void*)0;_LL5D: {struct Cyc_Splay_node*n=
_tmp48->v;return(void*)({struct Cyc_Splay_Node_struct*_tmp49=_cycalloc(sizeof(*
_tmp49));_tmp49[0]=({struct Cyc_Splay_Node_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=({
struct Cyc_Splay_noderef*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->v=({struct Cyc_Splay_node*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->key=(void*)((void*)n->key);_tmp4C->data=(
void*)f(env,(void*)n->data);_tmp4C->left=(void*)Cyc_SlowDict_map_tree_c(f,env,(
void*)n->left);_tmp4C->right=(void*)Cyc_SlowDict_map_tree_c(f,env,(void*)n->right);
_tmp4C;});_tmp4B;});_tmp4A;});_tmp49;});}_LL59:;}struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d){return({struct Cyc_SlowDict_Dict*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->reln=d->reln;_tmp4D->tree=(void*)Cyc_SlowDict_map_tree_c(
f,env,(void*)d->tree);_tmp4D;});}struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*
d){void*_tmp4E=(void*)d->tree;struct Cyc_Splay_noderef*_tmp4F;_LL60: if((int)
_tmp4E == 0){goto _LL61;}else{goto _LL62;}_LL62: if((unsigned int)_tmp4E > 1?*((int*)
_tmp4E)== 0: 0){_LL64: _tmp4F=((struct Cyc_Splay_Node_struct*)_tmp4E)->f1;goto _LL63;}
else{goto _LL5F;}_LL61:(int)_throw((void*)Cyc_SlowDict_Absent);_LL63: return({
struct _tuple0*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->f1=(void*)(_tmp4F->v)->key;
_tmp50->f2=(void*)(_tmp4F->v)->data;_tmp50;});_LL5F:;}struct Cyc_List_List*Cyc_SlowDict_to_list_f(
void*k,void*v,struct Cyc_List_List*accum){return({struct Cyc_List_List*_tmp51=
_cycalloc(sizeof(*_tmp51));_tmp51->hd=({struct _tuple0*_tmp52=_cycalloc(sizeof(*
_tmp52));_tmp52->f1=k;_tmp52->f2=v;_tmp52;});_tmp51->tl=accum;_tmp51;});}struct
Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*
d,struct Cyc_List_List*accum))Cyc_SlowDict_fold)(Cyc_SlowDict_to_list_f,d,0);}
