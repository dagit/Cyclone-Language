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
static inline 
void * _check_null(void * ptr) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
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
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[
18];extern unsigned char Cyc_List_Nth[8];struct Cyc_Splay_node;struct Cyc_Splay_noderef{
struct Cyc_Splay_node*v;};struct Cyc_Splay_Node_struct{int tag;struct Cyc_Splay_noderef*
f1;};struct Cyc_Splay_node{void*key;void*data;void*left;void*right;};extern int Cyc_Splay_rsplay(
struct _RegionHandle*,int(*f)(void*,void*),void*,void*);extern int Cyc_Splay_splay(
int(*f)(void*,void*),void*,void*);static void Cyc_Splay_rotate_left(struct
_RegionHandle*r,struct Cyc_Splay_noderef*nr){struct Cyc_Splay_node*_tmp0=nr->v;
void*_tmp1=(void*)_tmp0->left;struct Cyc_Splay_noderef*_tmp2;_LL1: if((
unsigned int)_tmp1 > 1?*((int*)_tmp1)== 0: 0){_LL5: _tmp2=((struct Cyc_Splay_Node_struct*)
_tmp1)->f1;goto _LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2: {struct Cyc_Splay_node*
_tmp3=_tmp2->v;struct Cyc_Splay_Node_struct*_tmp4=({struct Cyc_Splay_Node_struct*
_tmp6=_region_malloc(r,sizeof(struct Cyc_Splay_Node_struct));_tmp6[0]=({struct Cyc_Splay_Node_struct
_tmp7;_tmp7.tag=0;_tmp7.f1=({struct Cyc_Splay_noderef*_tmp8=_region_malloc(r,
sizeof(struct Cyc_Splay_noderef));_tmp8->v=({struct Cyc_Splay_node*_tmp9=
_region_malloc(r,sizeof(struct Cyc_Splay_node));_tmp9->key=(void*)((void*)_tmp0->key);
_tmp9->data=(void*)((void*)_tmp0->data);_tmp9->left=(void*)((void*)_tmp3->right);
_tmp9->right=(void*)((void*)_tmp0->right);_tmp9;});_tmp8;});_tmp7;});_tmp6;});nr->v=({
struct Cyc_Splay_node*_tmp5=_region_malloc(r,sizeof(struct Cyc_Splay_node));_tmp5->key=(
void*)((void*)_tmp3->key);_tmp5->data=(void*)((void*)_tmp3->data);_tmp5->left=(
void*)((void*)_tmp3->left);_tmp5->right=(void*)((void*)_tmp4);_tmp5;});goto _LL0;}
_LL4:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmpA=_cycalloc(
sizeof(struct Cyc_Core_Invalid_argument_struct));_tmpA[0]=({struct Cyc_Core_Invalid_argument_struct
_tmpB;_tmpB.tag=Cyc_Core_Invalid_argument;_tmpB.f1=_tag_arr("Splay::rotate_left",
sizeof(unsigned char),19);_tmpB;});_tmpA;}));_LL0:;}static void Cyc_Splay_rotate_right(
struct _RegionHandle*r,struct Cyc_Splay_noderef*nr){struct Cyc_Splay_node*_tmpC=nr->v;
void*_tmpD=(void*)_tmpC->right;struct Cyc_Splay_noderef*_tmpE;_LL7: if((
unsigned int)_tmpD > 1?*((int*)_tmpD)== 0: 0){_LLB: _tmpE=((struct Cyc_Splay_Node_struct*)
_tmpD)->f1;goto _LL8;}else{goto _LL9;}_LL9: goto _LLA;_LL8: {struct Cyc_Splay_node*
_tmpF=_tmpE->v;struct Cyc_Splay_Node_struct*_tmp10=({struct Cyc_Splay_Node_struct*
_tmp12=_region_malloc(r,sizeof(struct Cyc_Splay_Node_struct));_tmp12[0]=({struct
Cyc_Splay_Node_struct _tmp13;_tmp13.tag=0;_tmp13.f1=({struct Cyc_Splay_noderef*
_tmp14=_region_malloc(r,sizeof(struct Cyc_Splay_noderef));_tmp14->v=({struct Cyc_Splay_node*
_tmp15=_region_malloc(r,sizeof(struct Cyc_Splay_node));_tmp15->key=(void*)((void*)
_tmpC->key);_tmp15->data=(void*)((void*)_tmpC->data);_tmp15->left=(void*)((void*)
_tmpC->left);_tmp15->right=(void*)((void*)_tmpF->left);_tmp15;});_tmp14;});
_tmp13;});_tmp12;});nr->v=({struct Cyc_Splay_node*_tmp11=_region_malloc(r,sizeof(
struct Cyc_Splay_node));_tmp11->key=(void*)((void*)_tmpF->key);_tmp11->data=(void*)((
void*)_tmpF->data);_tmp11->left=(void*)((void*)_tmp10);_tmp11->right=(void*)((
void*)_tmpF->right);_tmp11;});goto _LL6;}_LLA:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp16=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp16[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp17;_tmp17.tag=Cyc_Core_Invalid_argument;
_tmp17.f1=_tag_arr("Splay::rotate_right",sizeof(unsigned char),20);_tmp17;});
_tmp16;}));_LL6:;}struct _tuple0{void*f1;struct Cyc_Splay_noderef*f2;};static void
Cyc_Splay_lift(struct _RegionHandle*r,struct Cyc_List_List*dnl){while(dnl != 0){if(
dnl->tl == 0){{struct _tuple0*_tmp18=(struct _tuple0*)dnl->hd;struct _tuple0 _tmp19;
struct Cyc_Splay_noderef*_tmp1A;void*_tmp1B;struct _tuple0 _tmp1C;struct Cyc_Splay_noderef*
_tmp1D;void*_tmp1E;_LLD: _tmp19=*_tmp18;_LL12: _tmp1B=_tmp19.f1;if((int)_tmp1B == 0){
goto _LL11;}else{goto _LLF;}_LL11: _tmp1A=_tmp19.f2;goto _LLE;_LLF: _tmp1C=*_tmp18;
_LL14: _tmp1E=_tmp1C.f1;if((int)_tmp1E == 1){goto _LL13;}else{goto _LLC;}_LL13:
_tmp1D=_tmp1C.f2;goto _LL10;_LLE: Cyc_Splay_rotate_left(r,_tmp1A);goto _LLC;_LL10:
Cyc_Splay_rotate_right(r,_tmp1D);goto _LLC;_LLC:;}return;}{struct _tuple0 _tmp20;
struct Cyc_Splay_noderef*_tmp21;void*_tmp22;struct _tuple0*_tmp1F=(struct _tuple0*)
dnl->hd;_tmp20=*_tmp1F;_LL17: _tmp22=_tmp20.f1;goto _LL16;_LL16: _tmp21=_tmp20.f2;
goto _LL15;_LL15: {struct _tuple0 _tmp24;struct Cyc_Splay_noderef*_tmp25;void*_tmp26;
struct _tuple0*_tmp23=(struct _tuple0*)((struct Cyc_List_List*)_check_null(dnl->tl))->hd;
_tmp24=*_tmp23;_LL1A: _tmp26=_tmp24.f1;goto _LL19;_LL19: _tmp25=_tmp24.f2;goto _LL18;
_LL18: dnl=((struct Cyc_List_List*)_check_null(dnl->tl))->tl;{void*_tmp27=_tmp22;
_LL1C: if((int)_tmp27 == 0){goto _LL1D;}else{goto _LL1E;}_LL1E: if((int)_tmp27 == 1){
goto _LL1F;}else{goto _LL1B;}_LL1D:{void*_tmp28=_tmp26;_LL21: if((int)_tmp28 == 0){
goto _LL22;}else{goto _LL23;}_LL23: if((int)_tmp28 == 1){goto _LL24;}else{goto _LL20;}
_LL22: Cyc_Splay_rotate_left(r,_tmp25);Cyc_Splay_rotate_left(r,_tmp25);goto _LL20;
_LL24: Cyc_Splay_rotate_left(r,_tmp21);Cyc_Splay_rotate_right(r,_tmp25);goto _LL20;
_LL20:;}goto _LL1B;_LL1F:{void*_tmp29=_tmp26;_LL26: if((int)_tmp29 == 0){goto _LL27;}
else{goto _LL28;}_LL28: if((int)_tmp29 == 1){goto _LL29;}else{goto _LL25;}_LL27: Cyc_Splay_rotate_right(
r,_tmp21);Cyc_Splay_rotate_left(r,_tmp25);goto _LL25;_LL29: Cyc_Splay_rotate_right(
r,_tmp25);Cyc_Splay_rotate_right(r,_tmp25);goto _LL25;_LL25:;}goto _LL1B;_LL1B:;}}}}}
int Cyc_Splay_rsplay(struct _RegionHandle*r,int(*reln)(void*,void*),void*
reln_first_arg,void*tree){struct _RegionHandle _tmp2A=_new_region("temp");struct
_RegionHandle*temp=& _tmp2A;_push_region(temp);{struct Cyc_List_List*path=0;while(
1){void*_tmp2B=tree;struct Cyc_Splay_noderef*_tmp2C;_LL2B: if((unsigned int)_tmp2B
> 1?*((int*)_tmp2B)== 0: 0){_LL2F: _tmp2C=((struct Cyc_Splay_Node_struct*)_tmp2B)->f1;
goto _LL2C;}else{goto _LL2D;}_LL2D: if((int)_tmp2B == 0){goto _LL2E;}else{goto _LL2A;}
_LL2C: {struct Cyc_Splay_node*_tmp2D=_tmp2C->v;int _tmp2E=reln(reln_first_arg,(
void*)_tmp2D->key);if(_tmp2E == 0){Cyc_Splay_lift(r,path);{int _tmp2F=1;
_npop_handler(0);return _tmp2F;}}else{if(_tmp2E < 0){path=({struct Cyc_List_List*
_tmp30=_region_malloc(temp,sizeof(struct Cyc_List_List));_tmp30->hd=({struct
_tuple0*_tmp31=_region_malloc(temp,sizeof(struct _tuple0));_tmp31->f1=(void*)0;
_tmp31->f2=_tmp2C;_tmp31;});_tmp30->tl=path;_tmp30;});tree=(void*)_tmp2D->left;}
else{path=({struct Cyc_List_List*_tmp32=_region_malloc(temp,sizeof(struct Cyc_List_List));
_tmp32->hd=({struct _tuple0*_tmp33=_region_malloc(temp,sizeof(struct _tuple0));
_tmp33->f1=(void*)1;_tmp33->f2=_tmp2C;_tmp33;});_tmp32->tl=path;_tmp32;});tree=(
void*)_tmp2D->right;}}goto _LL2A;}_LL2E: if(path != 0){Cyc_Splay_lift(r,((struct Cyc_List_List*)
_check_null(path))->tl);}{int _tmp34=0;_npop_handler(0);return _tmp34;}_LL2A:;}};
_pop_region(temp);}int Cyc_Splay_splay(int(*reln)(void*,void*),void*
reln_first_arg,void*tree){return Cyc_Splay_rsplay(Cyc_Core_heap_region,reln,
reln_first_arg,tree);}
