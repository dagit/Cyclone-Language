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
  if(!ptr)
    _throw_null();
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
  if(!ptr || index >= bound)
    _throw_null();
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
  if(index >= bound)
    _throw_arraybounds();
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
  if(!arr.base) 
    _throw_null();
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
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
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.last_plus_one - arr.curr) / elt_sz;
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

static inline void * _cycalloc(int n) {
  void * ans = GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = GC_malloc(n);
  if(!ans)
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
#define _new_region(n) _profile_new_region(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; struct Cyc_Splay_node; struct Cyc_Splay_noderef{ struct Cyc_Splay_node*
v; } ; static const int Cyc_Splay_Leaf= 0; static const int Cyc_Splay_Node= 0;
struct Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef* f1; } ; struct
Cyc_Splay_node{ void* key; void* data; void* left; void* right; } ; extern int
Cyc_Splay_rsplay( struct _RegionHandle*, int(* f)( void*, void*), void*, void*);
extern int Cyc_Splay_splay( int(* f)( void*, void*), void*, void*); static const
int Cyc_Splay_LEFT= 0; static const int Cyc_Splay_RIGHT= 1; static void Cyc_Splay_rotate_left(
struct _RegionHandle* r, struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node*
_temp0= nr->v; void* _temp1=( void*) _temp0->left; struct Cyc_Splay_noderef*
_temp7; _LL3: if(( unsigned int) _temp1 >  1u?*(( int*) _temp1) ==  Cyc_Splay_Node:
0){ _LL8: _temp7=(( struct Cyc_Splay_Node_struct*) _temp1)->f1; goto _LL4;}
else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_Splay_node* _temp9= _temp7->v;
struct Cyc_Splay_Node_struct* _temp10=({ struct Cyc_Splay_Node_struct* _temp12=(
struct Cyc_Splay_Node_struct*) _region_malloc( r, sizeof( struct Cyc_Splay_Node_struct));
_temp12[ 0]=({ struct Cyc_Splay_Node_struct _temp13; _temp13.tag= Cyc_Splay_Node;
_temp13.f1=({ struct Cyc_Splay_noderef* _temp14=( struct Cyc_Splay_noderef*)
_region_malloc( r, sizeof( struct Cyc_Splay_noderef)); _temp14->v=({ struct Cyc_Splay_node*
_temp15=( struct Cyc_Splay_node*) _region_malloc( r, sizeof( struct Cyc_Splay_node));
_temp15->key=( void*)(( void*) _temp0->key); _temp15->data=( void*)(( void*)
_temp0->data); _temp15->left=( void*)(( void*) _temp9->right); _temp15->right=(
void*)(( void*) _temp0->right); _temp15;}); _temp14;}); _temp13;}); _temp12;});
nr->v=({ struct Cyc_Splay_node* _temp11=( struct Cyc_Splay_node*) _region_malloc(
r, sizeof( struct Cyc_Splay_node)); _temp11->key=( void*)(( void*) _temp9->key);
_temp11->data=( void*)(( void*) _temp9->data); _temp11->left=( void*)(( void*)
_temp9->left); _temp11->right=( void*)(( void*) _temp10); _temp11;}); goto _LL2;}
_LL6:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp16=(
struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp16[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp17; _temp17.tag= Cyc_Core_Invalid_argument;
_temp17.f1= _tag_arr("Splay::rotate_left", sizeof( unsigned char), 19u); _temp17;});
_temp16;})); _LL2:;} static void Cyc_Splay_rotate_right( struct _RegionHandle* r,
struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* _temp18= nr->v; void*
_temp19=( void*) _temp18->right; struct Cyc_Splay_noderef* _temp25; _LL21: if((
unsigned int) _temp19 >  1u?*(( int*) _temp19) ==  Cyc_Splay_Node: 0){ _LL26:
_temp25=(( struct Cyc_Splay_Node_struct*) _temp19)->f1; goto _LL22;} else{ goto
_LL23;} _LL23: goto _LL24; _LL22: { struct Cyc_Splay_node* _temp27= _temp25->v;
struct Cyc_Splay_Node_struct* _temp28=({ struct Cyc_Splay_Node_struct* _temp30=(
struct Cyc_Splay_Node_struct*) _region_malloc( r, sizeof( struct Cyc_Splay_Node_struct));
_temp30[ 0]=({ struct Cyc_Splay_Node_struct _temp31; _temp31.tag= Cyc_Splay_Node;
_temp31.f1=({ struct Cyc_Splay_noderef* _temp32=( struct Cyc_Splay_noderef*)
_region_malloc( r, sizeof( struct Cyc_Splay_noderef)); _temp32->v=({ struct Cyc_Splay_node*
_temp33=( struct Cyc_Splay_node*) _region_malloc( r, sizeof( struct Cyc_Splay_node));
_temp33->key=( void*)(( void*) _temp18->key); _temp33->data=( void*)(( void*)
_temp18->data); _temp33->left=( void*)(( void*) _temp18->left); _temp33->right=(
void*)(( void*) _temp27->left); _temp33;}); _temp32;}); _temp31;}); _temp30;});
nr->v=({ struct Cyc_Splay_node* _temp29=( struct Cyc_Splay_node*) _region_malloc(
r, sizeof( struct Cyc_Splay_node)); _temp29->key=( void*)(( void*) _temp27->key);
_temp29->data=( void*)(( void*) _temp27->data); _temp29->left=( void*)(( void*)
_temp28); _temp29->right=( void*)(( void*) _temp27->right); _temp29;}); goto
_LL20;} _LL24:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp34=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp34[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp35; _temp35.tag= Cyc_Core_Invalid_argument; _temp35.f1= _tag_arr("Splay::rotate_right",
sizeof( unsigned char), 20u); _temp35;}); _temp34;})); _LL20:;} struct _tuple0{
void* f1; struct Cyc_Splay_noderef* f2; } ; static void Cyc_Splay_lift( struct
_RegionHandle* r, struct Cyc_List_List* dnl){ while( dnl !=  0) { if( dnl->tl == 
0){{ struct _tuple0* _temp36=( struct _tuple0*) dnl->hd; struct _tuple0 _temp42;
struct Cyc_Splay_noderef* _temp43; void* _temp45; struct _tuple0 _temp47; struct
Cyc_Splay_noderef* _temp48; void* _temp50; _LL38: _temp42=* _temp36; _LL46:
_temp45= _temp42.f1; if( _temp45 == ( void*) Cyc_Splay_LEFT){ goto _LL44;} else{
goto _LL40;} _LL44: _temp43= _temp42.f2; goto _LL39; _LL40: _temp47=* _temp36;
_LL51: _temp50= _temp47.f1; if( _temp50 == ( void*) Cyc_Splay_RIGHT){ goto _LL49;}
else{ goto _LL37;} _LL49: _temp48= _temp47.f2; goto _LL41; _LL39: Cyc_Splay_rotate_left(
r, _temp43); goto _LL37; _LL41: Cyc_Splay_rotate_right( r, _temp48); goto _LL37;
_LL37:;} return;}{ struct _tuple0 _temp54; struct Cyc_Splay_noderef* _temp55;
void* _temp57; struct _tuple0* _temp52=( struct _tuple0*) dnl->hd; _temp54=*
_temp52; _LL58: _temp57= _temp54.f1; goto _LL56; _LL56: _temp55= _temp54.f2;
goto _LL53; _LL53: { struct _tuple0 _temp61; struct Cyc_Splay_noderef* _temp62;
void* _temp64; struct _tuple0* _temp59=( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( dnl->tl))->hd; _temp61=* _temp59; _LL65: _temp64= _temp61.f1; goto
_LL63; _LL63: _temp62= _temp61.f2; goto _LL60; _LL60: dnl=(( struct Cyc_List_List*)
_check_null( dnl->tl))->tl;{ void* _temp66= _temp57; _LL68: if( _temp66 == (
void*) Cyc_Splay_LEFT){ goto _LL69;} else{ goto _LL70;} _LL70: if( _temp66 == (
void*) Cyc_Splay_RIGHT){ goto _LL71;} else{ goto _LL67;} _LL69:{ void* _temp72=
_temp64; _LL74: if( _temp72 == ( void*) Cyc_Splay_LEFT){ goto _LL75;} else{ goto
_LL76;} _LL76: if( _temp72 == ( void*) Cyc_Splay_RIGHT){ goto _LL77;} else{ goto
_LL73;} _LL75: Cyc_Splay_rotate_left( r, _temp62); Cyc_Splay_rotate_left( r,
_temp62); goto _LL73; _LL77: Cyc_Splay_rotate_left( r, _temp55); Cyc_Splay_rotate_right(
r, _temp62); goto _LL73; _LL73:;} goto _LL67; _LL71:{ void* _temp78= _temp64;
_LL80: if( _temp78 == ( void*) Cyc_Splay_LEFT){ goto _LL81;} else{ goto _LL82;}
_LL82: if( _temp78 == ( void*) Cyc_Splay_RIGHT){ goto _LL83;} else{ goto _LL79;}
_LL81: Cyc_Splay_rotate_right( r, _temp55); Cyc_Splay_rotate_left( r, _temp62);
goto _LL79; _LL83: Cyc_Splay_rotate_right( r, _temp62); Cyc_Splay_rotate_right(
r, _temp62); goto _LL79; _LL79:;} goto _LL67; _LL67:;}}}}} int Cyc_Splay_rsplay(
struct _RegionHandle* r, int(* reln)( void*, void*), void* reln_first_arg, void*
tree){ struct _RegionHandle _temp84= _new_region("temp"); struct _RegionHandle*
temp=& _temp84; _push_region( temp);{ struct Cyc_List_List* path= 0; while( 1) {
void* _temp85= tree; struct Cyc_Splay_noderef* _temp91; _LL87: if(( unsigned int)
_temp85 >  1u?*(( int*) _temp85) ==  Cyc_Splay_Node: 0){ _LL92: _temp91=((
struct Cyc_Splay_Node_struct*) _temp85)->f1; goto _LL88;} else{ goto _LL89;}
_LL89: if( _temp85 == ( void*) Cyc_Splay_Leaf){ goto _LL90;} else{ goto _LL86;}
_LL88: { struct Cyc_Splay_node* _temp93= _temp91->v; int _temp94= reln(
reln_first_arg,( void*) _temp93->key); if( _temp94 ==  0){ Cyc_Splay_lift( r,
path);{ int _temp95= 1; _npop_handler( 0u); return _temp95;}} else{ if( _temp94
<  0){ path=({ struct Cyc_List_List* _temp96=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp96->hd=( void*)({
struct _tuple0* _temp97=( struct _tuple0*) _region_malloc( temp, sizeof( struct
_tuple0)); _temp97->f1=( void*) Cyc_Splay_LEFT; _temp97->f2= _temp91; _temp97;});
_temp96->tl= path; _temp96;}); tree=( void*) _temp93->left;} else{ path=({
struct Cyc_List_List* _temp98=( struct Cyc_List_List*) _region_malloc( temp,
sizeof( struct Cyc_List_List)); _temp98->hd=( void*)({ struct _tuple0* _temp99=(
struct _tuple0*) _region_malloc( temp, sizeof( struct _tuple0)); _temp99->f1=(
void*) Cyc_Splay_RIGHT; _temp99->f2= _temp91; _temp99;}); _temp98->tl= path;
_temp98;}); tree=( void*) _temp93->right;}} goto _LL86;} _LL90: if( path !=  0){
Cyc_Splay_lift( r,(( struct Cyc_List_List*) _check_null( path))->tl);}{ int
_temp100= 0; _npop_handler( 0u); return _temp100;} _LL86:;}}; _pop_region( temp);}
int Cyc_Splay_splay( int(* reln)( void*, void*), void* reln_first_arg, void*
tree){ return Cyc_Splay_rsplay( Cyc_Core_heap_region, reln, reln_first_arg, tree);}
