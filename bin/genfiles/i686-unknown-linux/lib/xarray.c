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
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Xarray_Xarray{
struct _tagged_arr elmts; int num_elmts; } ; extern int Cyc_Xarray_length(
struct Cyc_Xarray_Xarray*); extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*,
int); extern void Cyc_Xarray_set( struct Cyc_Xarray_Xarray*, int, void*); extern
struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int, void*); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_create_empty(); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton(
int, void*); extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*);
extern int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*, void*); extern struct
_tagged_arr Cyc_Xarray_to_array( struct Cyc_Xarray_Xarray*); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_from_array( struct _tagged_arr arr); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_append( struct Cyc_Xarray_Xarray*, struct Cyc_Xarray_Xarray*); extern
void Cyc_Xarray_app( void*(* f)( void*), struct Cyc_Xarray_Xarray*); extern void
Cyc_Xarray_app_c( void*(* f)( void*, void*), void*, struct Cyc_Xarray_Xarray*);
extern void Cyc_Xarray_iter( void(* f)( void*), struct Cyc_Xarray_Xarray*);
extern void Cyc_Xarray_iter_c( void(* f)( void*, void*), void*, struct Cyc_Xarray_Xarray*);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_map( void*(* f)( void*), struct Cyc_Xarray_Xarray*);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c( void*(* f)( void*, void*),
void*, struct Cyc_Xarray_Xarray*); extern void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray*
xarr); int Cyc_Xarray_length( struct Cyc_Xarray_Xarray* xarr){ return xarr->num_elmts;}
void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray* xarr, int i){ if( i <  0? 1: i
>=  xarr->num_elmts){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp0=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp0[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp1; _temp1.tag= Cyc_Core_Invalid_argument;
_temp1.f1= _tag_arr("Xarray::get: bad index", sizeof( unsigned char), 23u);
_temp1;}); _temp0;}));} return*(( void**) _check_unknown_subscript( xarr->elmts,
sizeof( void*), i));} void Cyc_Xarray_set( struct Cyc_Xarray_Xarray* xarr, int i,
void* a){ if( i <  0? 1: i >=  xarr->num_elmts){( int) _throw(( void*)({ struct
Cyc_Core_Invalid_argument_struct* _temp2=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp2[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp3; _temp3.tag= Cyc_Core_Invalid_argument;
_temp3.f1= _tag_arr("Xarray::set: bad index", sizeof( unsigned char), 23u);
_temp3;}); _temp2;}));}*(( void**) _check_unknown_subscript( xarr->elmts,
sizeof( void*), i))= a;} struct Cyc_Xarray_Xarray* Cyc_Xarray_create_empty(){
struct _tagged_arr x= _tag_arr(({ void** _temp5=( void**) _cycalloc( sizeof(
void*) *  0); _temp5;}), sizeof( void*), 0u); return({ struct Cyc_Xarray_Xarray*
_temp4=( struct Cyc_Xarray_Xarray*) _cycalloc( sizeof( struct Cyc_Xarray_Xarray));
_temp4->elmts= x; _temp4->num_elmts= 0; _temp4;});} struct Cyc_Xarray_Xarray*
Cyc_Xarray_create( int len, void* a){ if( len <  0){( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp6=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp6[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp7; _temp7.tag= Cyc_Core_Invalid_argument;
_temp7.f1= _tag_arr("xarrays must have a non-negative size buffer", sizeof(
unsigned char), 45u); _temp7;}); _temp6;}));} return({ struct Cyc_Xarray_Xarray*
_temp8=( struct Cyc_Xarray_Xarray*) _cycalloc( sizeof( struct Cyc_Xarray_Xarray));
_temp8->elmts=({ unsigned int _temp9=( unsigned int) len; void** _temp10=( void**)
_cycalloc( _check_times( sizeof( void*), _temp9)); struct _tagged_arr _temp12=
_tag_arr( _temp10, sizeof( void*),( unsigned int) len);{ unsigned int _temp11=
_temp9; unsigned int i; for( i= 0; i <  _temp11; i ++){ _temp10[ i]= a;}};
_temp12;}); _temp8->num_elmts= 0; _temp8;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton(
int len, void* a){ if( len <  1){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp13=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp13[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp14; _temp14.tag= Cyc_Core_Invalid_argument; _temp14.f1= _tag_arr("singleton xarray must have size >=1",
sizeof( unsigned char), 36u); _temp14;}); _temp13;}));}{ struct Cyc_Xarray_Xarray*
x= Cyc_Xarray_create( len, a); x->num_elmts= 1; return x;}} void Cyc_Xarray_add(
struct Cyc_Xarray_Xarray* xarr, void* a){ if( xarr->num_elmts ==  _get_arr_size(
xarr->elmts, sizeof( void*))){ if( xarr->num_elmts ==  0){ xarr->elmts= _tag_arr(({
unsigned int _temp15=( unsigned int) 10; void** _temp16=( void**) _cycalloc(
_check_times( sizeof( void*), _temp15));{ unsigned int _temp17= _temp15;
unsigned int i; for( i= 0; i <  _temp17; i ++){ _temp16[ i]= a;}}; _temp16;}),
sizeof( void*),( unsigned int) 10);} else{ struct _tagged_arr newarr=({
unsigned int _temp18=( unsigned int)( xarr->num_elmts *  2); void** _temp19=(
void**) _cycalloc( _check_times( sizeof( void*), _temp18)); struct _tagged_arr
_temp21= _tag_arr( _temp19, sizeof( void*),( unsigned int)( xarr->num_elmts *  2));{
unsigned int _temp20= _temp18; unsigned int i; for( i= 0; i <  _temp20; i ++){
_temp19[ i]=*(( void**) _check_unknown_subscript( xarr->elmts, sizeof( void*), 0));}};
_temp21;});{ int i= 1; for( 0; i <  xarr->num_elmts; ++ i){*(( void**)
_check_unknown_subscript( newarr, sizeof( void*), i))=*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), i));}} xarr->elmts=
newarr;}}*(( void**) _check_unknown_subscript( xarr->elmts, sizeof( void*), xarr->num_elmts
++))= a;} int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray* xarr, void* a){ Cyc_Xarray_add(
xarr, a); return xarr->num_elmts -  1;} struct _tagged_arr Cyc_Xarray_to_array(
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts ==  0){ return _tag_arr(({
void** _temp22=( void**) _cycalloc( sizeof( void*) *  0); _temp22;}), sizeof(
void*), 0u);}{ struct _tagged_arr ans=({ unsigned int _temp23=( unsigned int)
xarr->num_elmts; void** _temp24=( void**) _cycalloc( _check_times( sizeof( void*),
_temp23)); struct _tagged_arr _temp26= _tag_arr( _temp24, sizeof( void*),(
unsigned int) xarr->num_elmts);{ unsigned int _temp25= _temp23; unsigned int i;
for( i= 0; i <  _temp25; i ++){ _temp24[ i]=*(( void**) _check_unknown_subscript(
xarr->elmts, sizeof( void*),( int) i));}}; _temp26;}); return ans;}} struct Cyc_Xarray_Xarray*
Cyc_Xarray_from_array( struct _tagged_arr arr){ if( _get_arr_size( arr, sizeof(
void*)) ==  0){ return Cyc_Xarray_create_empty();}{ struct Cyc_Xarray_Xarray*
ans=({ struct Cyc_Xarray_Xarray* _temp27=( struct Cyc_Xarray_Xarray*) _cycalloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp27->elmts=({ unsigned int _temp28=
_get_arr_size( arr, sizeof( void*)); void** _temp29=( void**) _cycalloc(
_check_times( sizeof( void*), _temp28)); struct _tagged_arr _temp31= _tag_arr(
_temp29, sizeof( void*), _get_arr_size( arr, sizeof( void*)));{ unsigned int
_temp30= _temp28; unsigned int i; for( i= 0; i <  _temp30; i ++){ _temp29[ i]=*((
void**) _check_unknown_subscript( arr, sizeof( void*),( int) i));}}; _temp31;});
_temp27->num_elmts=( int) _get_arr_size( arr, sizeof( void*)); _temp27;});
return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_append( struct Cyc_Xarray_Xarray*
xarr1, struct Cyc_Xarray_Xarray* xarr2){ int newsz=( int)( _get_arr_size( xarr1->elmts,
sizeof( void*)) +  _get_arr_size( xarr2->elmts, sizeof( void*))); if( newsz == 
0){ return Cyc_Xarray_create_empty();}{ void* init= _get_arr_size( xarr1->elmts,
sizeof( void*)) ==  0?*(( void**) _check_unknown_subscript( xarr2->elmts,
sizeof( void*), 0)):*(( void**) _check_unknown_subscript( xarr1->elmts, sizeof(
void*), 0)); struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp32=(
struct Cyc_Xarray_Xarray*) _cycalloc( sizeof( struct Cyc_Xarray_Xarray));
_temp32->elmts=({ unsigned int _temp33=( unsigned int) newsz; void** _temp34=(
void**) _cycalloc( _check_times( sizeof( void*), _temp33)); struct _tagged_arr
_temp36= _tag_arr( _temp34, sizeof( void*),( unsigned int) newsz);{ unsigned int
_temp35= _temp33; unsigned int i; for( i= 0; i <  _temp35; i ++){ _temp34[ i]=
init;}}; _temp36;}); _temp32->num_elmts= 0; _temp32;});{ int i= 0; for( 0; i < 
xarr1->num_elmts; ++ i){ Cyc_Xarray_add( ans,*(( void**)
_check_unknown_subscript( xarr1->elmts, sizeof( void*), i)));}}{ int i= 0; for(
0; i <  xarr2->num_elmts; ++ i){ Cyc_Xarray_add( ans,*(( void**)
_check_unknown_subscript( xarr2->elmts, sizeof( void*), i)));}} return ans;}}
void Cyc_Xarray_app( void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ int i=
0; for( 0; i <  xarr->num_elmts; ++ i){ f(*(( void**) _check_unknown_subscript(
xarr->elmts, sizeof( void*), i)));}} void Cyc_Xarray_app_c( void*(* f)( void*,
void*), void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i <  xarr->num_elmts;
++ i){ f( env,*(( void**) _check_unknown_subscript( xarr->elmts, sizeof( void*),
i)));}} void Cyc_Xarray_iter( void(* f)( void*), struct Cyc_Xarray_Xarray* xarr){
int i= 0; for( 0; i <  xarr->num_elmts; ++ i){ f(*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), i)));}} void Cyc_Xarray_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0;
for( 0; i <  xarr->num_elmts; ++ i){ f( env,*(( void**) _check_unknown_subscript(
xarr->elmts, sizeof( void*), i)));}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map(
void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts ==  0){
return Cyc_Xarray_create_empty();}{ struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray*
_temp37=( struct Cyc_Xarray_Xarray*) _cycalloc( sizeof( struct Cyc_Xarray_Xarray));
_temp37->elmts=({ unsigned int _temp38= _get_arr_size( xarr->elmts, sizeof( void*));
void** _temp39=( void**) _cycalloc( _check_times( sizeof( void*), _temp38));
struct _tagged_arr _temp41= _tag_arr( _temp39, sizeof( void*), _get_arr_size(
xarr->elmts, sizeof( void*)));{ unsigned int _temp40= _temp38; unsigned int i;
for( i= 0; i <  _temp40; i ++){ _temp39[ i]= f(*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), 0)));}}; _temp41;});
_temp37->num_elmts= xarr->num_elmts; _temp37;});{ int i= 1; for( 0; i <  xarr->num_elmts;
++ i){*(( void**) _check_unknown_subscript( ans->elmts, sizeof( void*), i))= f(*((
void**) _check_unknown_subscript( xarr->elmts, sizeof( void*), i)));}} return
ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts ==  0){ return
Cyc_Xarray_create_empty();}{ struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray*
_temp42=( struct Cyc_Xarray_Xarray*) _cycalloc( sizeof( struct Cyc_Xarray_Xarray));
_temp42->elmts=({ unsigned int _temp43= _get_arr_size( xarr->elmts, sizeof( void*));
void** _temp44=( void**) _cycalloc( _check_times( sizeof( void*), _temp43));
struct _tagged_arr _temp46= _tag_arr( _temp44, sizeof( void*), _get_arr_size(
xarr->elmts, sizeof( void*)));{ unsigned int _temp45= _temp43; unsigned int i;
for( i= 0; i <  _temp45; i ++){ _temp44[ i]= f( env,*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), 0)));}}; _temp46;});
_temp42->num_elmts= xarr->num_elmts; _temp42;});{ int i= 1; for( 0; i <  xarr->num_elmts;
++ i){*(( void**) _check_unknown_subscript( ans->elmts, sizeof( void*), i))= f(
env,*(( void**) _check_unknown_subscript( xarr->elmts, sizeof( void*), i)));}}
return ans;}} void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray* xarr){ xarr->num_elmts=
0;}
