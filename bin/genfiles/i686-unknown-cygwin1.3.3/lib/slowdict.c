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
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[
8];struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};static
const int Cyc_Splay_Leaf=0;static const int Cyc_Splay_Node=0;struct Cyc_Splay_Node_struct{
int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*
left;void*right;};extern int Cyc_Splay_splay(int(*f)(void*,void*),void*,void*);
struct Cyc_SlowDict_Dict;extern unsigned char Cyc_SlowDict_Present[12];extern
unsigned char Cyc_SlowDict_Absent[11];extern struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(
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
comp)(void*,void*)){void*t=(void*)Cyc_Splay_Leaf;return({struct Cyc_SlowDict_Dict*
_tmp0=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_tmp0->reln=comp;_tmp0->tree=(
void*)t;_tmp0;});}int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d){void*_tmp1=(
void*)d->tree;_LL3: if(_tmp1 == (void*)Cyc_Splay_Leaf){goto _LL4;}else{goto _LL5;}
_LL5: if((unsigned int)_tmp1 > 1?*((int*)_tmp1)== Cyc_Splay_Node: 0){goto _LL6;}
else{goto _LL2;}_LL4: return 1;_LL6: return 0;_LL2:;}int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*
d,void*key){return Cyc_Splay_splay(d->reln,key,(void*)d->tree);}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,void*data){void*newleft=(
void*)Cyc_Splay_Leaf;void*newright=(void*)Cyc_Splay_Leaf;if(Cyc_Splay_splay(d->reln,
key,(void*)d->tree)){void*_tmp7=(void*)d->tree;struct Cyc_Splay_noderef*_tmpD;
_LL9: if((unsigned int)_tmp7 > 1?*((int*)_tmp7)== Cyc_Splay_Node: 0){_LL14: _tmpD=((
struct Cyc_Splay_Node_struct*)_tmp7)->f1;goto _LL10;}else{goto _LL11;}_LL11: goto
_LL12;_LL10: newleft=(void*)(_tmpD->v)->left;newright=(void*)(_tmpD->v)->right;
goto _LL8;_LL12: goto _LL8;_LL8:;}else{void*_tmpF=(void*)d->tree;struct Cyc_Splay_noderef*
_tmp15;_LL17: if((unsigned int)_tmpF > 1?*((int*)_tmpF)== Cyc_Splay_Node: 0){_LL22:
_tmp15=((struct Cyc_Splay_Node_struct*)_tmpF)->f1;goto _LL18;}else{goto _LL19;}
_LL19: if(_tmpF == (void*)Cyc_Splay_Leaf){goto _LL20;}else{goto _LL16;}_LL18: {
struct Cyc_Splay_node*_tmp17=_tmp15->v;if((d->reln)(key,(void*)_tmp17->key)< 0){
newleft=(void*)_tmp17->left;newright=(void*)({struct Cyc_Splay_Node_struct*_tmp18=
_cycalloc(sizeof(struct Cyc_Splay_Node_struct));_tmp18[0]=({struct Cyc_Splay_Node_struct
_tmp19;_tmp19.tag=Cyc_Splay_Node;_tmp19.f1=({struct Cyc_Splay_noderef*_tmp1A=
_cycalloc(sizeof(struct Cyc_Splay_noderef));_tmp1A->v=({struct Cyc_Splay_node*
_tmp1B=_cycalloc(sizeof(struct Cyc_Splay_node));_tmp1B->key=(void*)((void*)_tmp17->key);
_tmp1B->data=(void*)((void*)_tmp17->data);_tmp1B->left=(void*)((void*)Cyc_Splay_Leaf);
_tmp1B->right=(void*)((void*)_tmp17->right);_tmp1B;});_tmp1A;});_tmp19;});_tmp18;});}
else{newleft=(void*)({struct Cyc_Splay_Node_struct*_tmp1C=_cycalloc(sizeof(struct
Cyc_Splay_Node_struct));_tmp1C[0]=({struct Cyc_Splay_Node_struct _tmp1D;_tmp1D.tag=
Cyc_Splay_Node;_tmp1D.f1=({struct Cyc_Splay_noderef*_tmp1E=_cycalloc(sizeof(
struct Cyc_Splay_noderef));_tmp1E->v=({struct Cyc_Splay_node*_tmp1F=_cycalloc(
sizeof(struct Cyc_Splay_node));_tmp1F->key=(void*)((void*)_tmp17->key);_tmp1F->data=(
void*)((void*)_tmp17->data);_tmp1F->left=(void*)((void*)_tmp17->left);_tmp1F->right=(
void*)((void*)Cyc_Splay_Leaf);_tmp1F;});_tmp1E;});_tmp1D;});_tmp1C;});newright=(
void*)_tmp17->right;}goto _LL16;}_LL20: goto _LL16;_LL16:;}return({struct Cyc_SlowDict_Dict*
_tmp20=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_tmp20->reln=d->reln;_tmp20->tree=(
void*)((void*)({struct Cyc_Splay_Node_struct*_tmp21=_cycalloc(sizeof(struct Cyc_Splay_Node_struct));
_tmp21[0]=({struct Cyc_Splay_Node_struct _tmp22;_tmp22.tag=Cyc_Splay_Node;_tmp22.f1=({
struct Cyc_Splay_noderef*_tmp23=_cycalloc(sizeof(struct Cyc_Splay_noderef));_tmp23->v=({
struct Cyc_Splay_node*_tmp24=_cycalloc(sizeof(struct Cyc_Splay_node));_tmp24->key=(
void*)key;_tmp24->data=(void*)data;_tmp24->left=(void*)newleft;_tmp24->right=(
void*)newright;_tmp24;});_tmp23;});_tmp22;});_tmp21;}));_tmp20;});}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*key,void*data){if(Cyc_Splay_splay(
d->reln,key,(void*)d->tree)){(int)_throw((void*)Cyc_SlowDict_Present);}return Cyc_SlowDict_insert(
d,key,data);}struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*
d,struct Cyc_List_List*kds){for(0;kds != 0;kds=kds->tl){d=Cyc_SlowDict_insert(d,(*((
struct _tuple0*)kds->hd)).f1,(*((struct _tuple0*)kds->hd)).f2);}return d;}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){return({
struct Cyc_SlowDict_Dict*_tmp25=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_tmp25->reln=
comp;_tmp25->tree=(void*)((void*)({struct Cyc_Splay_Node_struct*_tmp26=_cycalloc(
sizeof(struct Cyc_Splay_Node_struct));_tmp26[0]=({struct Cyc_Splay_Node_struct
_tmp27;_tmp27.tag=Cyc_Splay_Node;_tmp27.f1=({struct Cyc_Splay_noderef*_tmp28=
_cycalloc(sizeof(struct Cyc_Splay_noderef));_tmp28->v=({struct Cyc_Splay_node*
_tmp29=_cycalloc(sizeof(struct Cyc_Splay_node));_tmp29->key=(void*)key;_tmp29->data=(
void*)data;_tmp29->left=(void*)((void*)Cyc_Splay_Leaf);_tmp29->right=(void*)((
void*)Cyc_Splay_Leaf);_tmp29;});_tmp28;});_tmp27;});_tmp26;}));_tmp25;});}void*
Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*key){if(Cyc_Splay_splay(d->reln,
key,(void*)d->tree)){void*_tmp2A=(void*)d->tree;struct Cyc_Splay_noderef*_tmp30;
_LL44: if((unsigned int)_tmp2A > 1?*((int*)_tmp2A)== Cyc_Splay_Node: 0){_LL49:
_tmp30=((struct Cyc_Splay_Node_struct*)_tmp2A)->f1;goto _LL45;}else{goto _LL46;}
_LL46: if(_tmp2A == (void*)Cyc_Splay_Leaf){goto _LL47;}else{goto _LL43;}_LL45: return(
void*)(_tmp30->v)->data;_LL47:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp32[0]=({struct Cyc_Core_Impossible_struct
_tmp33;_tmp33.tag=Cyc_Core_Impossible;_tmp33.f1=_tag_arr("Dict::lookup",sizeof(
unsigned char),13);_tmp33;});_tmp32;}));_LL43:;}(int)_throw((void*)Cyc_SlowDict_Absent);}
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){void*_tmp34=(void*)d->tree;
struct Cyc_Splay_noderef*_tmp3A;_LL54: if((unsigned int)_tmp34 > 1?*((int*)_tmp34)
== Cyc_Splay_Node: 0){_LL59: _tmp3A=((struct Cyc_Splay_Node_struct*)_tmp34)->f1;
goto _LL55;}else{goto _LL56;}_LL56: if(_tmp34 == (void*)Cyc_Splay_Leaf){goto _LL57;}
else{goto _LL53;}_LL55: return({struct Cyc_Core_Opt*_tmp3C=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp3C->v=(void*)((void*)(_tmp3A->v)->data);_tmp3C;});_LL57:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3D=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmp3D[0]=({struct Cyc_Core_Impossible_struct _tmp3E;
_tmp3E.tag=Cyc_Core_Impossible;_tmp3E.f1=_tag_arr("Dict::lookup",sizeof(
unsigned char),13);_tmp3E;});_tmp3D;}));_LL53:;}return 0;}static int Cyc_SlowDict_get_largest(
void*x,void*y){return 1;}struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*
d,void*key){if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){void*_tmp3F=(void*)d->tree;
struct Cyc_Splay_noderef*_tmp45;_LL65: if(_tmp3F == (void*)Cyc_Splay_Leaf){goto
_LL66;}else{goto _LL67;}_LL67: if((unsigned int)_tmp3F > 1?*((int*)_tmp3F)== Cyc_Splay_Node:
0){_LL70: _tmp45=((struct Cyc_Splay_Node_struct*)_tmp3F)->f1;goto _LL68;}else{goto
_LL64;}_LL66:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp47=
_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp47[0]=({struct Cyc_Core_Impossible_struct
_tmp48;_tmp48.tag=Cyc_Core_Impossible;_tmp48.f1=_tag_arr("Dict::lookup",sizeof(
unsigned char),13);_tmp48;});_tmp47;}));_LL68: {struct Cyc_Splay_node*n=_tmp45->v;
void*_tmp49=(void*)n->left;struct Cyc_Splay_noderef*_tmp4F;_LL75: if(_tmp49 == (
void*)Cyc_Splay_Leaf){goto _LL76;}else{goto _LL77;}_LL77: if((unsigned int)_tmp49 > 
1?*((int*)_tmp49)== Cyc_Splay_Node: 0){_LL80: _tmp4F=((struct Cyc_Splay_Node_struct*)
_tmp49)->f1;goto _LL78;}else{goto _LL74;}_LL76: return({struct Cyc_SlowDict_Dict*
_tmp51=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_tmp51->reln=d->reln;_tmp51->tree=(
void*)((void*)n->right);_tmp51;});_LL78: {void*_tmp52=(void*)n->right;struct Cyc_Splay_noderef*
_tmp58;_LL84: if(_tmp52 == (void*)Cyc_Splay_Leaf){goto _LL85;}else{goto _LL86;}_LL86:
if((unsigned int)_tmp52 > 1?*((int*)_tmp52)== Cyc_Splay_Node: 0){_LL89: _tmp58=((
struct Cyc_Splay_Node_struct*)_tmp52)->f1;goto _LL87;}else{goto _LL83;}_LL85: return({
struct Cyc_SlowDict_Dict*_tmp5A=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_tmp5A->reln=
d->reln;_tmp5A->tree=(void*)((void*)n->left);_tmp5A;});_LL87: Cyc_Splay_splay(Cyc_SlowDict_get_largest,
key,(void*)n->left);{struct Cyc_Splay_node*newtop=_tmp4F->v;return({struct Cyc_SlowDict_Dict*
_tmp5B=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_tmp5B->reln=d->reln;_tmp5B->tree=(
void*)((void*)({struct Cyc_Splay_Node_struct*_tmp5C=_cycalloc(sizeof(struct Cyc_Splay_Node_struct));
_tmp5C[0]=({struct Cyc_Splay_Node_struct _tmp5D;_tmp5D.tag=Cyc_Splay_Node;_tmp5D.f1=({
struct Cyc_Splay_noderef*_tmp5E=_cycalloc(sizeof(struct Cyc_Splay_noderef));_tmp5E->v=({
struct Cyc_Splay_node*_tmp5F=_cycalloc(sizeof(struct Cyc_Splay_node));_tmp5F->key=(
void*)((void*)newtop->key);_tmp5F->data=(void*)((void*)newtop->data);_tmp5F->left=(
void*)((void*)newtop->left);_tmp5F->right=(void*)((void*)n->right);_tmp5F;});
_tmp5E;});_tmp5D;});_tmp5C;}));_tmp5B;});}_LL83:;}_LL74:;}_LL64:;}else{return d;}}
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,
void*key){struct Cyc_SlowDict_Dict*_tmp60=Cyc_SlowDict_delete(d,key);if(d == 
_tmp60){(int)_throw((void*)Cyc_SlowDict_Absent);}return _tmp60;}static void*Cyc_SlowDict_fold_tree(
void*(*f)(void*,void*,void*),void*t,void*accum){void*_tmp61=t;struct Cyc_Splay_noderef*
_tmp67;_LL99: if(_tmp61 == (void*)Cyc_Splay_Leaf){goto _LL100;}else{goto _LL101;}
_LL101: if((unsigned int)_tmp61 > 1?*((int*)_tmp61)== Cyc_Splay_Node: 0){_LL104:
_tmp67=((struct Cyc_Splay_Node_struct*)_tmp61)->f1;goto _LL102;}else{goto _LL98;}
_LL100: return accum;_LL102: {struct Cyc_Splay_node*n=_tmp67->v;return f((void*)n->key,(
void*)n->data,Cyc_SlowDict_fold_tree(f,(void*)n->left,Cyc_SlowDict_fold_tree(f,(
void*)n->right,accum)));}_LL98:;}void*Cyc_SlowDict_fold(void*(*f)(void*,void*,
void*),struct Cyc_SlowDict_Dict*d,void*accum){return Cyc_SlowDict_fold_tree(f,(
void*)d->tree,accum);}static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,
void*,void*),void*env,void*t,void*accum){void*_tmp69=t;struct Cyc_Splay_noderef*
_tmp6F;_LL107: if(_tmp69 == (void*)Cyc_Splay_Leaf){goto _LL108;}else{goto _LL109;}
_LL109: if((unsigned int)_tmp69 > 1?*((int*)_tmp69)== Cyc_Splay_Node: 0){_LL112:
_tmp6F=((struct Cyc_Splay_Node_struct*)_tmp69)->f1;goto _LL110;}else{goto _LL106;}
_LL108: return accum;_LL110: {struct Cyc_Splay_node*n=_tmp6F->v;return f(env,(void*)
n->key,(void*)n->data,Cyc_SlowDict_fold_tree_c(f,env,(void*)n->left,Cyc_SlowDict_fold_tree_c(
f,env,(void*)n->right,accum)));}_LL106:;}void*Cyc_SlowDict_fold_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){return Cyc_SlowDict_fold_tree_c(
f,env,(void*)dict->tree,accum);}static void Cyc_SlowDict_app_tree(void*(*f)(void*,
void*),void*t){void*_tmp71=t;struct Cyc_Splay_noderef*_tmp77;_LL115: if(_tmp71 == (
void*)Cyc_Splay_Leaf){goto _LL116;}else{goto _LL117;}_LL117: if((unsigned int)
_tmp71 > 1?*((int*)_tmp71)== Cyc_Splay_Node: 0){_LL120: _tmp77=((struct Cyc_Splay_Node_struct*)
_tmp71)->f1;goto _LL118;}else{goto _LL114;}_LL116: goto _LL114;_LL118: {struct Cyc_Splay_node*
_tmp79=_tmp77->v;Cyc_SlowDict_app_tree(f,(void*)_tmp79->left);f((void*)_tmp79->key,(
void*)_tmp79->data);Cyc_SlowDict_app_tree(f,(void*)_tmp79->right);goto _LL114;}
_LL114:;}void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree(f,(void*)d->tree);}static void Cyc_SlowDict_iter_tree(void(*
f)(void*,void*),void*t){void*_tmp7A=t;struct Cyc_Splay_noderef*_tmp80;_LL124: if(
_tmp7A == (void*)Cyc_Splay_Leaf){goto _LL125;}else{goto _LL126;}_LL126: if((
unsigned int)_tmp7A > 1?*((int*)_tmp7A)== Cyc_Splay_Node: 0){_LL129: _tmp80=((
struct Cyc_Splay_Node_struct*)_tmp7A)->f1;goto _LL127;}else{goto _LL123;}_LL125:
goto _LL123;_LL127: {struct Cyc_Splay_node*n=_tmp80->v;Cyc_SlowDict_iter_tree(f,(
void*)n->left);f((void*)n->key,(void*)n->data);Cyc_SlowDict_iter_tree(f,(void*)n->right);
goto _LL123;}_LL123:;}void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*
d){Cyc_SlowDict_iter_tree(f,(void*)d->tree);}static void Cyc_SlowDict_app_tree_c(
void*(*f)(void*,void*,void*),void*env,void*t){void*_tmp82=t;struct Cyc_Splay_noderef*
_tmp88;_LL132: if(_tmp82 == (void*)Cyc_Splay_Leaf){goto _LL133;}else{goto _LL134;}
_LL134: if((unsigned int)_tmp82 > 1?*((int*)_tmp82)== Cyc_Splay_Node: 0){_LL137:
_tmp88=((struct Cyc_Splay_Node_struct*)_tmp82)->f1;goto _LL135;}else{goto _LL131;}
_LL133: goto _LL131;_LL135: {struct Cyc_Splay_node*n=_tmp88->v;Cyc_SlowDict_app_tree_c(
f,env,(void*)n->left);f(env,(void*)n->key,(void*)n->data);Cyc_SlowDict_app_tree_c(
f,env,(void*)n->right);goto _LL131;}_LL131:;}void Cyc_SlowDict_app_c(void*(*f)(
void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){Cyc_SlowDict_app_tree_c(f,
env,(void*)d->tree);}static void Cyc_SlowDict_iter_tree_c(void(*f)(void*,void*,
void*),void*env,void*t){void*_tmp8A=t;struct Cyc_Splay_noderef*_tmp90;_LL140: if(
_tmp8A == (void*)Cyc_Splay_Leaf){goto _LL141;}else{goto _LL142;}_LL142: if((
unsigned int)_tmp8A > 1?*((int*)_tmp8A)== Cyc_Splay_Node: 0){_LL145: _tmp90=((
struct Cyc_Splay_Node_struct*)_tmp8A)->f1;goto _LL143;}else{goto _LL139;}_LL141:
goto _LL139;_LL143: {struct Cyc_Splay_node*n=_tmp90->v;Cyc_SlowDict_iter_tree_c(f,
env,(void*)n->left);f(env,(void*)n->key,(void*)n->data);Cyc_SlowDict_iter_tree_c(
f,env,(void*)n->right);goto _LL139;}_LL139:;}void Cyc_SlowDict_iter_c(void(*f)(
void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){Cyc_SlowDict_iter_tree_c(
f,env,(void*)d->tree);}static void*Cyc_SlowDict_map_tree(void*(*f)(void*),void*t){
void*_tmp92=t;struct Cyc_Splay_noderef*_tmp98;_LL148: if(_tmp92 == (void*)Cyc_Splay_Leaf){
goto _LL149;}else{goto _LL150;}_LL150: if((unsigned int)_tmp92 > 1?*((int*)_tmp92)== 
Cyc_Splay_Node: 0){_LL153: _tmp98=((struct Cyc_Splay_Node_struct*)_tmp92)->f1;goto
_LL151;}else{goto _LL147;}_LL149: return(void*)Cyc_Splay_Leaf;_LL151: {struct Cyc_Splay_node*
_tmp9A=_tmp98->v;return(void*)({struct Cyc_Splay_Node_struct*_tmp9B=_cycalloc(
sizeof(struct Cyc_Splay_Node_struct));_tmp9B[0]=({struct Cyc_Splay_Node_struct
_tmp9C;_tmp9C.tag=Cyc_Splay_Node;_tmp9C.f1=({struct Cyc_Splay_noderef*_tmp9D=
_cycalloc(sizeof(struct Cyc_Splay_noderef));_tmp9D->v=({struct Cyc_Splay_node*
_tmp9E=_cycalloc(sizeof(struct Cyc_Splay_node));_tmp9E->key=(void*)((void*)_tmp9A->key);
_tmp9E->data=(void*)f((void*)_tmp9A->data);_tmp9E->left=(void*)Cyc_SlowDict_map_tree(
f,(void*)_tmp9A->left);_tmp9E->right=(void*)Cyc_SlowDict_map_tree(f,(void*)
_tmp9A->right);_tmp9E;});_tmp9D;});_tmp9C;});_tmp9B;});}_LL147:;}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d){return({struct Cyc_SlowDict_Dict*
_tmp9F=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_tmp9F->reln=d->reln;_tmp9F->tree=(
void*)Cyc_SlowDict_map_tree(f,(void*)d->tree);_tmp9F;});}static void*Cyc_SlowDict_map_tree_c(
void*(*f)(void*,void*),void*env,void*t){void*_tmpA0=t;struct Cyc_Splay_noderef*
_tmpA6;_LL162: if(_tmpA0 == (void*)Cyc_Splay_Leaf){goto _LL163;}else{goto _LL164;}
_LL164: if((unsigned int)_tmpA0 > 1?*((int*)_tmpA0)== Cyc_Splay_Node: 0){_LL167:
_tmpA6=((struct Cyc_Splay_Node_struct*)_tmpA0)->f1;goto _LL165;}else{goto _LL161;}
_LL163: return(void*)Cyc_Splay_Leaf;_LL165: {struct Cyc_Splay_node*n=_tmpA6->v;
return(void*)({struct Cyc_Splay_Node_struct*_tmpA8=_cycalloc(sizeof(struct Cyc_Splay_Node_struct));
_tmpA8[0]=({struct Cyc_Splay_Node_struct _tmpA9;_tmpA9.tag=Cyc_Splay_Node;_tmpA9.f1=({
struct Cyc_Splay_noderef*_tmpAA=_cycalloc(sizeof(struct Cyc_Splay_noderef));_tmpAA->v=({
struct Cyc_Splay_node*_tmpAB=_cycalloc(sizeof(struct Cyc_Splay_node));_tmpAB->key=(
void*)((void*)n->key);_tmpAB->data=(void*)f(env,(void*)n->data);_tmpAB->left=(
void*)Cyc_SlowDict_map_tree_c(f,env,(void*)n->left);_tmpAB->right=(void*)Cyc_SlowDict_map_tree_c(
f,env,(void*)n->right);_tmpAB;});_tmpAA;});_tmpA9;});_tmpA8;});}_LL161:;}struct
Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*
d){return({struct Cyc_SlowDict_Dict*_tmpAC=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));
_tmpAC->reln=d->reln;_tmpAC->tree=(void*)Cyc_SlowDict_map_tree_c(f,env,(void*)d->tree);
_tmpAC;});}struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){void*
_tmpAD=(void*)d->tree;struct Cyc_Splay_noderef*_tmpB3;_LL175: if(_tmpAD == (void*)
Cyc_Splay_Leaf){goto _LL176;}else{goto _LL177;}_LL177: if((unsigned int)_tmpAD > 1?*((
int*)_tmpAD)== Cyc_Splay_Node: 0){_LL180: _tmpB3=((struct Cyc_Splay_Node_struct*)
_tmpAD)->f1;goto _LL178;}else{goto _LL174;}_LL176:(int)_throw((void*)Cyc_SlowDict_Absent);
_LL178: return({struct _tuple0*_tmpB5=_cycalloc(sizeof(struct _tuple0));_tmpB5->f1=(
void*)(_tmpB3->v)->key;_tmpB5->f2=(void*)(_tmpB3->v)->data;_tmpB5;});_LL174:;}
struct Cyc_List_List*Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*
accum){return({struct Cyc_List_List*_tmpB6=_cycalloc(sizeof(struct Cyc_List_List));
_tmpB6->hd=({struct _tuple0*_tmpB7=_cycalloc(sizeof(struct _tuple0));_tmpB7->f1=k;
_tmpB7->f2=v;_tmpB7;});_tmpB6->tl=accum;_tmpB6;});}struct Cyc_List_List*Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict*d){return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*
accum))Cyc_SlowDict_fold)(Cyc_SlowDict_to_list_f,d,0);}
