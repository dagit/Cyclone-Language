// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in
// the file lib/runtime_cyc.c

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
struct _tagged_string {  // delete after bootstrapping
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

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
//  extern struct _RegionHandle _new_region();
extern void * _region_malloc(struct _RegionHandle *, unsigned int);
extern void * _region_calloc(struct _RegionHandle *, unsigned int t,
                             unsigned int n);
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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; struct Cyc_Splay_node; struct Cyc_Splay_noderef{ struct Cyc_Splay_node*
v; } ; static const int Cyc_Splay_Leaf= 0; static const int Cyc_Splay_Node= 0;
struct Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef* f1; } ; struct
Cyc_Splay_node{ void* key; void* data; void* left; void* right; } ; extern int
Cyc_Splay_splay( int(* f)( void*, void*), void*, void*); struct Cyc_SlowDict_Dict;
extern unsigned char Cyc_SlowDict_Present[ 12u]; extern unsigned char Cyc_SlowDict_Absent[
11u]; extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* cmp)( void*,
void*)); extern int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d); extern
int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict* d, void* k); extern struct
Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void* k,
void* v); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new( struct Cyc_SlowDict_Dict*
d, void* k, void* v); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_inserts(
struct Cyc_SlowDict_Dict* d, struct Cyc_List_List* l); extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton( int(* cmp)( void*, void*), void* k, void* v); extern
void* Cyc_SlowDict_lookup( struct Cyc_SlowDict_Dict* d, void* k); extern struct
Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict* d, void* k);
extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict*
d, void* k); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* k); extern void* Cyc_SlowDict_fold( void*(* f)(
void*, void*, void*), struct Cyc_SlowDict_Dict* d, void* accum); extern void*
Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*, void*), void* env, struct
Cyc_SlowDict_Dict* d, void* accum); extern void Cyc_SlowDict_app( void*(* f)(
void*, void*), struct Cyc_SlowDict_Dict* d); extern void Cyc_SlowDict_app_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d);
extern void Cyc_SlowDict_iter( void(* f)( void*, void*), struct Cyc_SlowDict_Dict*
d); extern void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*), void* env,
struct Cyc_SlowDict_Dict* d); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_map(
void*(* f)( void*), struct Cyc_SlowDict_Dict* d); extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_SlowDict_Dict*
d); struct _tuple0{ void* f1; void* f2; } ; extern struct _tuple0* Cyc_SlowDict_choose(
struct Cyc_SlowDict_Dict* d); extern struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d); unsigned char Cyc_SlowDict_Absent[ 11u]="\000\000\000\000Absent";
unsigned char Cyc_SlowDict_Present[ 12u]="\000\000\000\000Present"; struct Cyc_SlowDict_Dict{
int(* reln)( void*, void*); void* tree; } ; struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty(
int(* comp)( void*, void*)){ void* t=( void*) Cyc_Splay_Leaf; return({ struct
Cyc_SlowDict_Dict* _temp0=( struct Cyc_SlowDict_Dict*) _cycalloc( sizeof( struct
Cyc_SlowDict_Dict)); _temp0->reln= comp; _temp0->tree=( void*) t; _temp0;});}
int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d){ void* _temp1=( void*) d->tree;
_LL3: if( _temp1 == ( void*) Cyc_Splay_Leaf){ goto _LL4;} else{ goto _LL5;} _LL5:
if(( unsigned int) _temp1 >  1u?*(( int*) _temp1) ==  Cyc_Splay_Node: 0){ goto
_LL6;} else{ goto _LL2;} _LL4: return 1; _LL6: return 0; _LL2:;} int Cyc_SlowDict_member(
struct Cyc_SlowDict_Dict* d, void* key){ return Cyc_Splay_splay( d->reln, key,(
void*) d->tree);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict*
d, void* key, void* data){ void* newleft=( void*) Cyc_Splay_Leaf; void* newright=(
void*) Cyc_Splay_Leaf; if( Cyc_Splay_splay( d->reln, key,( void*) d->tree)){
void* _temp7=( void*) d->tree; struct Cyc_Splay_noderef* _temp13; _LL9: if((
unsigned int) _temp7 >  1u?*(( int*) _temp7) ==  Cyc_Splay_Node: 0){ _LL14:
_temp13=(( struct Cyc_Splay_Node_struct*) _temp7)->f1; goto _LL10;} else{ goto
_LL11;} _LL11: goto _LL12; _LL10: newleft=( void*)( _temp13->v)->left; newright=(
void*)( _temp13->v)->right; goto _LL8; _LL12: goto _LL8; _LL8:;} else{ void*
_temp15=( void*) d->tree; struct Cyc_Splay_noderef* _temp21; _LL17: if((
unsigned int) _temp15 >  1u?*(( int*) _temp15) ==  Cyc_Splay_Node: 0){ _LL22:
_temp21=(( struct Cyc_Splay_Node_struct*) _temp15)->f1; goto _LL18;} else{ goto
_LL19;} _LL19: if( _temp15 == ( void*) Cyc_Splay_Leaf){ goto _LL20;} else{ goto
_LL16;} _LL18: { struct Cyc_Splay_node* _temp23= _temp21->v; if(( d->reln)( key,(
void*) _temp23->key) <  0){ newleft=( void*) _temp23->left; newright=( void*)({
struct Cyc_Splay_Node_struct* _temp24=( struct Cyc_Splay_Node_struct*) _cycalloc(
sizeof( struct Cyc_Splay_Node_struct)); _temp24[ 0]=({ struct Cyc_Splay_Node_struct
_temp25; _temp25.tag= Cyc_Splay_Node; _temp25.f1=({ struct Cyc_Splay_noderef*
_temp26=( struct Cyc_Splay_noderef*) _cycalloc( sizeof( struct Cyc_Splay_noderef));
_temp26->v=({ struct Cyc_Splay_node* _temp27=( struct Cyc_Splay_node*) _cycalloc(
sizeof( struct Cyc_Splay_node)); _temp27->key=( void*)(( void*) _temp23->key);
_temp27->data=( void*)(( void*) _temp23->data); _temp27->left=( void*)(( void*)
Cyc_Splay_Leaf); _temp27->right=( void*)(( void*) _temp23->right); _temp27;});
_temp26;}); _temp25;}); _temp24;});} else{ newleft=( void*)({ struct Cyc_Splay_Node_struct*
_temp28=( struct Cyc_Splay_Node_struct*) _cycalloc( sizeof( struct Cyc_Splay_Node_struct));
_temp28[ 0]=({ struct Cyc_Splay_Node_struct _temp29; _temp29.tag= Cyc_Splay_Node;
_temp29.f1=({ struct Cyc_Splay_noderef* _temp30=( struct Cyc_Splay_noderef*)
_cycalloc( sizeof( struct Cyc_Splay_noderef)); _temp30->v=({ struct Cyc_Splay_node*
_temp31=( struct Cyc_Splay_node*) _cycalloc( sizeof( struct Cyc_Splay_node));
_temp31->key=( void*)(( void*) _temp23->key); _temp31->data=( void*)(( void*)
_temp23->data); _temp31->left=( void*)(( void*) _temp23->left); _temp31->right=(
void*)(( void*) Cyc_Splay_Leaf); _temp31;}); _temp30;}); _temp29;}); _temp28;});
newright=( void*) _temp23->right;} goto _LL16;} _LL20: goto _LL16; _LL16:;}
return({ struct Cyc_SlowDict_Dict* _temp32=( struct Cyc_SlowDict_Dict*)
_cycalloc( sizeof( struct Cyc_SlowDict_Dict)); _temp32->reln= d->reln; _temp32->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp33=( struct Cyc_Splay_Node_struct*)
_cycalloc( sizeof( struct Cyc_Splay_Node_struct)); _temp33[ 0]=({ struct Cyc_Splay_Node_struct
_temp34; _temp34.tag= Cyc_Splay_Node; _temp34.f1=({ struct Cyc_Splay_noderef*
_temp35=( struct Cyc_Splay_noderef*) _cycalloc( sizeof( struct Cyc_Splay_noderef));
_temp35->v=({ struct Cyc_Splay_node* _temp36=( struct Cyc_Splay_node*) _cycalloc(
sizeof( struct Cyc_Splay_node)); _temp36->key=( void*) key; _temp36->data=( void*)
data; _temp36->left=( void*) newleft; _temp36->right=( void*) newright; _temp36;});
_temp35;}); _temp34;}); _temp33;})); _temp32;});} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ if( Cyc_Splay_splay( d->reln,
key,( void*) d->tree)){( int) _throw(( void*) Cyc_SlowDict_Present);} return Cyc_SlowDict_insert(
d, key, data);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_inserts( struct Cyc_SlowDict_Dict*
d, struct Cyc_List_List* kds){ for( 0; kds !=  0; kds= kds->tl){ d= Cyc_SlowDict_insert(
d,(*(( struct _tuple0*) kds->hd)).f1,(*(( struct _tuple0*) kds->hd)).f2);}
return d;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_singleton( int(* comp)( void*,
void*), void* key, void* data){ return({ struct Cyc_SlowDict_Dict* _temp37=(
struct Cyc_SlowDict_Dict*) _cycalloc( sizeof( struct Cyc_SlowDict_Dict));
_temp37->reln= comp; _temp37->tree=( void*)(( void*)({ struct Cyc_Splay_Node_struct*
_temp38=( struct Cyc_Splay_Node_struct*) _cycalloc( sizeof( struct Cyc_Splay_Node_struct));
_temp38[ 0]=({ struct Cyc_Splay_Node_struct _temp39; _temp39.tag= Cyc_Splay_Node;
_temp39.f1=({ struct Cyc_Splay_noderef* _temp40=( struct Cyc_Splay_noderef*)
_cycalloc( sizeof( struct Cyc_Splay_noderef)); _temp40->v=({ struct Cyc_Splay_node*
_temp41=( struct Cyc_Splay_node*) _cycalloc( sizeof( struct Cyc_Splay_node));
_temp41->key=( void*) key; _temp41->data=( void*) data; _temp41->left=( void*)((
void*) Cyc_Splay_Leaf); _temp41->right=( void*)(( void*) Cyc_Splay_Leaf);
_temp41;}); _temp40;}); _temp39;}); _temp38;})); _temp37;});} void* Cyc_SlowDict_lookup(
struct Cyc_SlowDict_Dict* d, void* key){ if( Cyc_Splay_splay( d->reln, key,(
void*) d->tree)){ void* _temp42=( void*) d->tree; struct Cyc_Splay_noderef*
_temp48; _LL44: if(( unsigned int) _temp42 >  1u?*(( int*) _temp42) ==  Cyc_Splay_Node:
0){ _LL49: _temp48=(( struct Cyc_Splay_Node_struct*) _temp42)->f1; goto _LL45;}
else{ goto _LL46;} _LL46: if( _temp42 == ( void*) Cyc_Splay_Leaf){ goto _LL47;}
else{ goto _LL43;} _LL45: return( void*)( _temp48->v)->data; _LL47:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp50=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp50[ 0]=({ struct
Cyc_Core_Impossible_struct _temp51; _temp51.tag= Cyc_Core_Impossible; _temp51.f1=
_tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp51;}); _temp50;}));
_LL43:;}( int) _throw(( void*) Cyc_SlowDict_Absent);} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt(
struct Cyc_SlowDict_Dict* d, void* key){ if( Cyc_Splay_splay( d->reln, key,(
void*) d->tree)){ void* _temp52=( void*) d->tree; struct Cyc_Splay_noderef*
_temp58; _LL54: if(( unsigned int) _temp52 >  1u?*(( int*) _temp52) ==  Cyc_Splay_Node:
0){ _LL59: _temp58=(( struct Cyc_Splay_Node_struct*) _temp52)->f1; goto _LL55;}
else{ goto _LL56;} _LL56: if( _temp52 == ( void*) Cyc_Splay_Leaf){ goto _LL57;}
else{ goto _LL53;} _LL55: return({ struct Cyc_Core_Opt* _temp60=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp60->v=( void*)(( void*)( _temp58->v)->data);
_temp60;}); _LL57:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp61=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp61[ 0]=({ struct Cyc_Core_Impossible_struct _temp62; _temp62.tag= Cyc_Core_Impossible;
_temp62.f1= _tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp62;});
_temp61;})); _LL53:;} return 0;} static int Cyc_SlowDict_get_largest( void* x,
void* y){ return 1;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict*
d, void* key){ if( Cyc_Splay_splay( d->reln, key,( void*) d->tree)){ void*
_temp63=( void*) d->tree; struct Cyc_Splay_noderef* _temp69; _LL65: if( _temp63
== ( void*) Cyc_Splay_Leaf){ goto _LL66;} else{ goto _LL67;} _LL67: if((
unsigned int) _temp63 >  1u?*(( int*) _temp63) ==  Cyc_Splay_Node: 0){ _LL70:
_temp69=(( struct Cyc_Splay_Node_struct*) _temp63)->f1; goto _LL68;} else{ goto
_LL64;} _LL66:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp71=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp71[ 0]=({ struct Cyc_Core_Impossible_struct _temp72; _temp72.tag= Cyc_Core_Impossible;
_temp72.f1= _tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp72;});
_temp71;})); _LL68: { struct Cyc_Splay_node* n= _temp69->v; void* _temp73=( void*)
n->left; struct Cyc_Splay_noderef* _temp79; _LL75: if( _temp73 == ( void*) Cyc_Splay_Leaf){
goto _LL76;} else{ goto _LL77;} _LL77: if(( unsigned int) _temp73 >  1u?*(( int*)
_temp73) ==  Cyc_Splay_Node: 0){ _LL80: _temp79=(( struct Cyc_Splay_Node_struct*)
_temp73)->f1; goto _LL78;} else{ goto _LL74;} _LL76: return({ struct Cyc_SlowDict_Dict*
_temp81=( struct Cyc_SlowDict_Dict*) _cycalloc( sizeof( struct Cyc_SlowDict_Dict));
_temp81->reln= d->reln; _temp81->tree=( void*)(( void*) n->right); _temp81;});
_LL78: { void* _temp82=( void*) n->right; struct Cyc_Splay_noderef* _temp88;
_LL84: if( _temp82 == ( void*) Cyc_Splay_Leaf){ goto _LL85;} else{ goto _LL86;}
_LL86: if(( unsigned int) _temp82 >  1u?*(( int*) _temp82) ==  Cyc_Splay_Node: 0){
_LL89: _temp88=(( struct Cyc_Splay_Node_struct*) _temp82)->f1; goto _LL87;}
else{ goto _LL83;} _LL85: return({ struct Cyc_SlowDict_Dict* _temp90=( struct
Cyc_SlowDict_Dict*) _cycalloc( sizeof( struct Cyc_SlowDict_Dict)); _temp90->reln=
d->reln; _temp90->tree=( void*)(( void*) n->left); _temp90;}); _LL87: Cyc_Splay_splay(
Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node* newtop=
_temp79->v; return({ struct Cyc_SlowDict_Dict* _temp91=( struct Cyc_SlowDict_Dict*)
_cycalloc( sizeof( struct Cyc_SlowDict_Dict)); _temp91->reln= d->reln; _temp91->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp92=( struct Cyc_Splay_Node_struct*)
_cycalloc( sizeof( struct Cyc_Splay_Node_struct)); _temp92[ 0]=({ struct Cyc_Splay_Node_struct
_temp93; _temp93.tag= Cyc_Splay_Node; _temp93.f1=({ struct Cyc_Splay_noderef*
_temp94=( struct Cyc_Splay_noderef*) _cycalloc( sizeof( struct Cyc_Splay_noderef));
_temp94->v=({ struct Cyc_Splay_node* _temp95=( struct Cyc_Splay_node*) _cycalloc(
sizeof( struct Cyc_Splay_node)); _temp95->key=( void*)(( void*) newtop->key);
_temp95->data=( void*)(( void*) newtop->data); _temp95->left=( void*)(( void*)
newtop->left); _temp95->right=( void*)(( void*) n->right); _temp95;}); _temp94;});
_temp93;}); _temp92;})); _temp91;});} _LL83:;} _LL74:;} _LL64:;} else{ return d;}}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present( struct Cyc_SlowDict_Dict*
d, void* key){ struct Cyc_SlowDict_Dict* _temp96= Cyc_SlowDict_delete( d, key);
if( d ==  _temp96){( int) _throw(( void*) Cyc_SlowDict_Absent);} return _temp96;}
static void* Cyc_SlowDict_fold_tree( void*(* f)( void*, void*, void*), void* t,
void* accum){ void* _temp97= t; struct Cyc_Splay_noderef* _temp103; _LL99: if(
_temp97 == ( void*) Cyc_Splay_Leaf){ goto _LL100;} else{ goto _LL101;} _LL101:
if(( unsigned int) _temp97 >  1u?*(( int*) _temp97) ==  Cyc_Splay_Node: 0){
_LL104: _temp103=(( struct Cyc_Splay_Node_struct*) _temp97)->f1; goto _LL102;}
else{ goto _LL98;} _LL100: return accum; _LL102: { struct Cyc_Splay_node* n=
_temp103->v; return f(( void*) n->key,( void*) n->data, Cyc_SlowDict_fold_tree(
f,( void*) n->left, Cyc_SlowDict_fold_tree( f,( void*) n->right, accum)));}
_LL98:;} void* Cyc_SlowDict_fold( void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict*
d, void* accum){ return Cyc_SlowDict_fold_tree( f,( void*) d->tree, accum);}
static void* Cyc_SlowDict_fold_tree_c( void*(* f)( void*, void*, void*, void*),
void* env, void* t, void* accum){ void* _temp105= t; struct Cyc_Splay_noderef*
_temp111; _LL107: if( _temp105 == ( void*) Cyc_Splay_Leaf){ goto _LL108;} else{
goto _LL109;} _LL109: if(( unsigned int) _temp105 >  1u?*(( int*) _temp105) == 
Cyc_Splay_Node: 0){ _LL112: _temp111=(( struct Cyc_Splay_Node_struct*) _temp105)->f1;
goto _LL110;} else{ goto _LL106;} _LL108: return accum; _LL110: { struct Cyc_Splay_node*
n= _temp111->v; return f( env,( void*) n->key,( void*) n->data, Cyc_SlowDict_fold_tree_c(
f, env,( void*) n->left, Cyc_SlowDict_fold_tree_c( f, env,( void*) n->right,
accum)));} _LL106:;} void* Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_SlowDict_Dict* dict, void* accum){ return Cyc_SlowDict_fold_tree_c(
f, env,( void*) dict->tree, accum);} static void Cyc_SlowDict_app_tree( void*(*
f)( void*, void*), void* t){ void* _temp113= t; struct Cyc_Splay_noderef*
_temp119; _LL115: if( _temp113 == ( void*) Cyc_Splay_Leaf){ goto _LL116;} else{
goto _LL117;} _LL117: if(( unsigned int) _temp113 >  1u?*(( int*) _temp113) == 
Cyc_Splay_Node: 0){ _LL120: _temp119=(( struct Cyc_Splay_Node_struct*) _temp113)->f1;
goto _LL118;} else{ goto _LL114;} _LL116: goto _LL114; _LL118: { struct Cyc_Splay_node*
_temp121= _temp119->v; Cyc_SlowDict_app_tree( f,( void*) _temp121->left); f((
void*) _temp121->key,( void*) _temp121->data); Cyc_SlowDict_app_tree( f,( void*)
_temp121->right); goto _LL114;} _LL114:;} void Cyc_SlowDict_app( void*(* f)(
void*, void*), struct Cyc_SlowDict_Dict* d){ Cyc_SlowDict_app_tree( f,( void*) d->tree);}
static void Cyc_SlowDict_iter_tree( void(* f)( void*, void*), void* t){ void*
_temp122= t; struct Cyc_Splay_noderef* _temp128; _LL124: if( _temp122 == ( void*)
Cyc_Splay_Leaf){ goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp122 >  1u?*(( int*) _temp122) ==  Cyc_Splay_Node: 0){ _LL129: _temp128=((
struct Cyc_Splay_Node_struct*) _temp122)->f1; goto _LL127;} else{ goto _LL123;}
_LL125: goto _LL123; _LL127: { struct Cyc_Splay_node* n= _temp128->v; Cyc_SlowDict_iter_tree(
f,( void*) n->left); f(( void*) n->key,( void*) n->data); Cyc_SlowDict_iter_tree(
f,( void*) n->right); goto _LL123;} _LL123:;} void Cyc_SlowDict_iter( void(* f)(
void*, void*), struct Cyc_SlowDict_Dict* d){ Cyc_SlowDict_iter_tree( f,( void*)
d->tree);} static void Cyc_SlowDict_app_tree_c( void*(* f)( void*, void*, void*),
void* env, void* t){ void* _temp130= t; struct Cyc_Splay_noderef* _temp136;
_LL132: if( _temp130 == ( void*) Cyc_Splay_Leaf){ goto _LL133;} else{ goto
_LL134;} _LL134: if(( unsigned int) _temp130 >  1u?*(( int*) _temp130) ==  Cyc_Splay_Node:
0){ _LL137: _temp136=(( struct Cyc_Splay_Node_struct*) _temp130)->f1; goto
_LL135;} else{ goto _LL131;} _LL133: goto _LL131; _LL135: { struct Cyc_Splay_node*
n= _temp136->v; Cyc_SlowDict_app_tree_c( f, env,( void*) n->left); f( env,( void*)
n->key,( void*) n->data); Cyc_SlowDict_app_tree_c( f, env,( void*) n->right);
goto _LL131;} _LL131:;} void Cyc_SlowDict_app_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ Cyc_SlowDict_app_tree_c( f, env,( void*)
d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*, void*, void*),
void* env, void* t){ void* _temp138= t; struct Cyc_Splay_noderef* _temp144;
_LL140: if( _temp138 == ( void*) Cyc_Splay_Leaf){ goto _LL141;} else{ goto
_LL142;} _LL142: if(( unsigned int) _temp138 >  1u?*(( int*) _temp138) ==  Cyc_Splay_Node:
0){ _LL145: _temp144=(( struct Cyc_Splay_Node_struct*) _temp138)->f1; goto
_LL143;} else{ goto _LL139;} _LL141: goto _LL139; _LL143: { struct Cyc_Splay_node*
n= _temp144->v; Cyc_SlowDict_iter_tree_c( f, env,( void*) n->left); f( env,(
void*) n->key,( void*) n->data); Cyc_SlowDict_iter_tree_c( f, env,( void*) n->right);
goto _LL139;} _LL139:;} void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ Cyc_SlowDict_iter_tree_c( f, env,( void*)
d->tree);} static void* Cyc_SlowDict_map_tree( void*(* f)( void*), void* t){
void* _temp146= t; struct Cyc_Splay_noderef* _temp152; _LL148: if( _temp146 == (
void*) Cyc_Splay_Leaf){ goto _LL149;} else{ goto _LL150;} _LL150: if((
unsigned int) _temp146 >  1u?*(( int*) _temp146) ==  Cyc_Splay_Node: 0){ _LL153:
_temp152=(( struct Cyc_Splay_Node_struct*) _temp146)->f1; goto _LL151;} else{
goto _LL147;} _LL149: return( void*) Cyc_Splay_Leaf; _LL151: { struct Cyc_Splay_node*
_temp154= _temp152->v; return( void*)({ struct Cyc_Splay_Node_struct* _temp155=(
struct Cyc_Splay_Node_struct*) _cycalloc( sizeof( struct Cyc_Splay_Node_struct));
_temp155[ 0]=({ struct Cyc_Splay_Node_struct _temp156; _temp156.tag= Cyc_Splay_Node;
_temp156.f1=({ struct Cyc_Splay_noderef* _temp157=( struct Cyc_Splay_noderef*)
_cycalloc( sizeof( struct Cyc_Splay_noderef)); _temp157->v=({ struct Cyc_Splay_node*
_temp158=( struct Cyc_Splay_node*) _cycalloc( sizeof( struct Cyc_Splay_node));
_temp158->key=( void*)(( void*) _temp154->key); _temp158->data=( void*) f(( void*)
_temp154->data); _temp158->left=( void*) Cyc_SlowDict_map_tree( f,( void*)
_temp154->left); _temp158->right=( void*) Cyc_SlowDict_map_tree( f,( void*)
_temp154->right); _temp158;}); _temp157;}); _temp156;}); _temp155;});} _LL147:;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_map( void*(* f)( void*), struct Cyc_SlowDict_Dict*
d){ return({ struct Cyc_SlowDict_Dict* _temp159=( struct Cyc_SlowDict_Dict*)
_cycalloc( sizeof( struct Cyc_SlowDict_Dict)); _temp159->reln= d->reln; _temp159->tree=(
void*) Cyc_SlowDict_map_tree( f,( void*) d->tree); _temp159;});} static void*
Cyc_SlowDict_map_tree_c( void*(* f)( void*, void*), void* env, void* t){ void*
_temp160= t; struct Cyc_Splay_noderef* _temp166; _LL162: if( _temp160 == ( void*)
Cyc_Splay_Leaf){ goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp160 >  1u?*(( int*) _temp160) ==  Cyc_Splay_Node: 0){ _LL167: _temp166=((
struct Cyc_Splay_Node_struct*) _temp160)->f1; goto _LL165;} else{ goto _LL161;}
_LL163: return( void*) Cyc_Splay_Leaf; _LL165: { struct Cyc_Splay_node* n=
_temp166->v; return( void*)({ struct Cyc_Splay_Node_struct* _temp168=( struct
Cyc_Splay_Node_struct*) _cycalloc( sizeof( struct Cyc_Splay_Node_struct));
_temp168[ 0]=({ struct Cyc_Splay_Node_struct _temp169; _temp169.tag= Cyc_Splay_Node;
_temp169.f1=({ struct Cyc_Splay_noderef* _temp170=( struct Cyc_Splay_noderef*)
_cycalloc( sizeof( struct Cyc_Splay_noderef)); _temp170->v=({ struct Cyc_Splay_node*
_temp171=( struct Cyc_Splay_node*) _cycalloc( sizeof( struct Cyc_Splay_node));
_temp171->key=( void*)(( void*) n->key); _temp171->data=( void*) f( env,( void*)
n->data); _temp171->left=( void*) Cyc_SlowDict_map_tree_c( f, env,( void*) n->left);
_temp171->right=( void*) Cyc_SlowDict_map_tree_c( f, env,( void*) n->right);
_temp171;}); _temp170;}); _temp169;}); _temp168;});} _LL161:;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){ return({ struct Cyc_SlowDict_Dict* _temp172=( struct Cyc_SlowDict_Dict*)
_cycalloc( sizeof( struct Cyc_SlowDict_Dict)); _temp172->reln= d->reln; _temp172->tree=(
void*) Cyc_SlowDict_map_tree_c( f, env,( void*) d->tree); _temp172;});} struct
_tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){ void* _temp173=(
void*) d->tree; struct Cyc_Splay_noderef* _temp179; _LL175: if( _temp173 == (
void*) Cyc_Splay_Leaf){ goto _LL176;} else{ goto _LL177;} _LL177: if((
unsigned int) _temp173 >  1u?*(( int*) _temp173) ==  Cyc_Splay_Node: 0){ _LL180:
_temp179=(( struct Cyc_Splay_Node_struct*) _temp173)->f1; goto _LL178;} else{
goto _LL174;} _LL176:( int) _throw(( void*) Cyc_SlowDict_Absent); _LL178: return({
struct _tuple0* _temp181=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp181->f1=( void*)( _temp179->v)->key; _temp181->f2=( void*)( _temp179->v)->data;
_temp181;}); _LL174:;} struct Cyc_List_List* Cyc_SlowDict_to_list_f( void* k,
void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp182=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp182->hd=(
void*)({ struct _tuple0* _temp183=( struct _tuple0*) _cycalloc( sizeof( struct
_tuple0)); _temp183->f1= k; _temp183->f2= v; _temp183;}); _temp182->tl= accum;
_temp182;});} struct Cyc_List_List* Cyc_SlowDict_to_list( struct Cyc_SlowDict_Dict*
d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( void*, void*,
struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct Cyc_List_List* accum))
Cyc_SlowDict_fold)( Cyc_SlowDict_to_list_f, d, 0);}
