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
struct Cyc_Splay_node*v;};static const int Cyc_Splay_Leaf=0;static const int Cyc_Splay_Node=
0;struct Cyc_Splay_Node_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{
void*key;void*data;void*left;void*right;};extern int Cyc_Splay_rsplay(struct
_RegionHandle*,int(*f)(void*,void*),void*,void*);extern int Cyc_Splay_splay(int(*f)(
void*,void*),void*,void*);static const int Cyc_Splay_LEFT=0;static const int Cyc_Splay_RIGHT=
1;static void Cyc_Splay_rotate_left(struct _RegionHandle*r,struct Cyc_Splay_noderef*
nr){struct Cyc_Splay_node*_tmp0=nr->v;void*_tmp1=(void*)_tmp0->left;struct Cyc_Splay_noderef*
_tmp7;_LL3: if((unsigned int)_tmp1 > 1?*((int*)_tmp1)== Cyc_Splay_Node: 0){_LL8:
_tmp7=((struct Cyc_Splay_Node_struct*)_tmp1)->f1;goto _LL4;}else{goto _LL5;}_LL5:
goto _LL6;_LL4: {struct Cyc_Splay_node*_tmp9=_tmp7->v;struct Cyc_Splay_Node_struct*
_tmpA=({struct Cyc_Splay_Node_struct*_tmpC=_region_malloc(r,sizeof(struct Cyc_Splay_Node_struct));
_tmpC[0]=({struct Cyc_Splay_Node_struct _tmpD;_tmpD.tag=Cyc_Splay_Node;_tmpD.f1=({
struct Cyc_Splay_noderef*_tmpE=_region_malloc(r,sizeof(struct Cyc_Splay_noderef));
_tmpE->v=({struct Cyc_Splay_node*_tmpF=_region_malloc(r,sizeof(struct Cyc_Splay_node));
_tmpF->key=(void*)((void*)_tmp0->key);_tmpF->data=(void*)((void*)_tmp0->data);
_tmpF->left=(void*)((void*)_tmp9->right);_tmpF->right=(void*)((void*)_tmp0->right);
_tmpF;});_tmpE;});_tmpD;});_tmpC;});nr->v=({struct Cyc_Splay_node*_tmpB=
_region_malloc(r,sizeof(struct Cyc_Splay_node));_tmpB->key=(void*)((void*)_tmp9->key);
_tmpB->data=(void*)((void*)_tmp9->data);_tmpB->left=(void*)((void*)_tmp9->left);
_tmpB->right=(void*)((void*)_tmpA);_tmpB;});goto _LL2;}_LL6:(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp10=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));
_tmp10[0]=({struct Cyc_Core_Invalid_argument_struct _tmp11;_tmp11.tag=Cyc_Core_Invalid_argument;
_tmp11.f1=_tag_arr("Splay::rotate_left",sizeof(unsigned char),19);_tmp11;});
_tmp10;}));_LL2:;}static void Cyc_Splay_rotate_right(struct _RegionHandle*r,struct
Cyc_Splay_noderef*nr){struct Cyc_Splay_node*_tmp12=nr->v;void*_tmp13=(void*)
_tmp12->right;struct Cyc_Splay_noderef*_tmp19;_LL21: if((unsigned int)_tmp13 > 1?*((
int*)_tmp13)== Cyc_Splay_Node: 0){_LL26: _tmp19=((struct Cyc_Splay_Node_struct*)
_tmp13)->f1;goto _LL22;}else{goto _LL23;}_LL23: goto _LL24;_LL22: {struct Cyc_Splay_node*
_tmp1B=_tmp19->v;struct Cyc_Splay_Node_struct*_tmp1C=({struct Cyc_Splay_Node_struct*
_tmp1E=_region_malloc(r,sizeof(struct Cyc_Splay_Node_struct));_tmp1E[0]=({struct
Cyc_Splay_Node_struct _tmp1F;_tmp1F.tag=Cyc_Splay_Node;_tmp1F.f1=({struct Cyc_Splay_noderef*
_tmp20=_region_malloc(r,sizeof(struct Cyc_Splay_noderef));_tmp20->v=({struct Cyc_Splay_node*
_tmp21=_region_malloc(r,sizeof(struct Cyc_Splay_node));_tmp21->key=(void*)((void*)
_tmp12->key);_tmp21->data=(void*)((void*)_tmp12->data);_tmp21->left=(void*)((
void*)_tmp12->left);_tmp21->right=(void*)((void*)_tmp1B->left);_tmp21;});_tmp20;});
_tmp1F;});_tmp1E;});nr->v=({struct Cyc_Splay_node*_tmp1D=_region_malloc(r,sizeof(
struct Cyc_Splay_node));_tmp1D->key=(void*)((void*)_tmp1B->key);_tmp1D->data=(
void*)((void*)_tmp1B->data);_tmp1D->left=(void*)((void*)_tmp1C);_tmp1D->right=(
void*)((void*)_tmp1B->right);_tmp1D;});goto _LL20;}_LL24:(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp22=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));
_tmp22[0]=({struct Cyc_Core_Invalid_argument_struct _tmp23;_tmp23.tag=Cyc_Core_Invalid_argument;
_tmp23.f1=_tag_arr("Splay::rotate_right",sizeof(unsigned char),20);_tmp23;});
_tmp22;}));_LL20:;}struct _tuple0{void*f1;struct Cyc_Splay_noderef*f2;};static void
Cyc_Splay_lift(struct _RegionHandle*r,struct Cyc_List_List*dnl){while(dnl != 0){if(
dnl->tl == 0){{struct _tuple0*_tmp24=(struct _tuple0*)dnl->hd;struct _tuple0 _tmp2A;
struct Cyc_Splay_noderef*_tmp2B;void*_tmp2D;struct _tuple0 _tmp2F;struct Cyc_Splay_noderef*
_tmp30;void*_tmp32;_LL38: _tmp2A=*_tmp24;_LL46: _tmp2D=_tmp2A.f1;if(_tmp2D == (void*)
Cyc_Splay_LEFT){goto _LL44;}else{goto _LL40;}_LL44: _tmp2B=_tmp2A.f2;goto _LL39;
_LL40: _tmp2F=*_tmp24;_LL51: _tmp32=_tmp2F.f1;if(_tmp32 == (void*)Cyc_Splay_RIGHT){
goto _LL49;}else{goto _LL37;}_LL49: _tmp30=_tmp2F.f2;goto _LL41;_LL39: Cyc_Splay_rotate_left(
r,_tmp2B);goto _LL37;_LL41: Cyc_Splay_rotate_right(r,_tmp30);goto _LL37;_LL37:;}
return;}{struct _tuple0 _tmp36;struct Cyc_Splay_noderef*_tmp37;void*_tmp39;struct
_tuple0*_tmp34=(struct _tuple0*)dnl->hd;_tmp36=*_tmp34;_LL58: _tmp39=_tmp36.f1;
goto _LL56;_LL56: _tmp37=_tmp36.f2;goto _LL53;_LL53: {struct _tuple0 _tmp3D;struct Cyc_Splay_noderef*
_tmp3E;void*_tmp40;struct _tuple0*_tmp3B=(struct _tuple0*)((struct Cyc_List_List*)
_check_null(dnl->tl))->hd;_tmp3D=*_tmp3B;_LL65: _tmp40=_tmp3D.f1;goto _LL63;_LL63:
_tmp3E=_tmp3D.f2;goto _LL60;_LL60: dnl=((struct Cyc_List_List*)_check_null(dnl->tl))->tl;{
void*_tmp42=_tmp39;_LL68: if(_tmp42 == (void*)Cyc_Splay_LEFT){goto _LL69;}else{goto
_LL70;}_LL70: if(_tmp42 == (void*)Cyc_Splay_RIGHT){goto _LL71;}else{goto _LL67;}
_LL69:{void*_tmp48=_tmp40;_LL74: if(_tmp48 == (void*)Cyc_Splay_LEFT){goto _LL75;}
else{goto _LL76;}_LL76: if(_tmp48 == (void*)Cyc_Splay_RIGHT){goto _LL77;}else{goto
_LL73;}_LL75: Cyc_Splay_rotate_left(r,_tmp3E);Cyc_Splay_rotate_left(r,_tmp3E);
goto _LL73;_LL77: Cyc_Splay_rotate_left(r,_tmp37);Cyc_Splay_rotate_right(r,_tmp3E);
goto _LL73;_LL73:;}goto _LL67;_LL71:{void*_tmp4E=_tmp40;_LL80: if(_tmp4E == (void*)
Cyc_Splay_LEFT){goto _LL81;}else{goto _LL82;}_LL82: if(_tmp4E == (void*)Cyc_Splay_RIGHT){
goto _LL83;}else{goto _LL79;}_LL81: Cyc_Splay_rotate_right(r,_tmp37);Cyc_Splay_rotate_left(
r,_tmp3E);goto _LL79;_LL83: Cyc_Splay_rotate_right(r,_tmp3E);Cyc_Splay_rotate_right(
r,_tmp3E);goto _LL79;_LL79:;}goto _LL67;_LL67:;}}}}}int Cyc_Splay_rsplay(struct
_RegionHandle*r,int(*reln)(void*,void*),void*reln_first_arg,void*tree){struct
_RegionHandle _tmp54=_new_region("temp");struct _RegionHandle*temp=& _tmp54;
_push_region(temp);{struct Cyc_List_List*path=0;while(1){void*_tmp55=tree;struct
Cyc_Splay_noderef*_tmp5B;_LL87: if((unsigned int)_tmp55 > 1?*((int*)_tmp55)== Cyc_Splay_Node:
0){_LL92: _tmp5B=((struct Cyc_Splay_Node_struct*)_tmp55)->f1;goto _LL88;}else{goto
_LL89;}_LL89: if(_tmp55 == (void*)Cyc_Splay_Leaf){goto _LL90;}else{goto _LL86;}_LL88: {
struct Cyc_Splay_node*_tmp5D=_tmp5B->v;int _tmp5E=reln(reln_first_arg,(void*)
_tmp5D->key);if(_tmp5E == 0){Cyc_Splay_lift(r,path);{int _tmp5F=1;_npop_handler(0);
return _tmp5F;}}else{if(_tmp5E < 0){path=({struct Cyc_List_List*_tmp60=
_region_malloc(temp,sizeof(struct Cyc_List_List));_tmp60->hd=({struct _tuple0*
_tmp61=_region_malloc(temp,sizeof(struct _tuple0));_tmp61->f1=(void*)Cyc_Splay_LEFT;
_tmp61->f2=_tmp5B;_tmp61;});_tmp60->tl=path;_tmp60;});tree=(void*)_tmp5D->left;}
else{path=({struct Cyc_List_List*_tmp62=_region_malloc(temp,sizeof(struct Cyc_List_List));
_tmp62->hd=({struct _tuple0*_tmp63=_region_malloc(temp,sizeof(struct _tuple0));
_tmp63->f1=(void*)Cyc_Splay_RIGHT;_tmp63->f2=_tmp5B;_tmp63;});_tmp62->tl=path;
_tmp62;});tree=(void*)_tmp5D->right;}}goto _LL86;}_LL90: if(path != 0){Cyc_Splay_lift(
r,((struct Cyc_List_List*)_check_null(path))->tl);}{int _tmp64=0;_npop_handler(0);
return _tmp64;}_LL86:;}};_pop_region(temp);}int Cyc_Splay_splay(int(*reln)(void*,
void*),void*reln_first_arg,void*tree){return Cyc_Splay_rsplay(Cyc_Core_heap_region,
reln,reln_first_arg,tree);}
