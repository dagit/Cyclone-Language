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
extern struct _tagged_arr Cyc_Core_new_string( int); extern struct _tagged_arr
Cyc_Core_rnew_string( struct _RegionHandle*, int); extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[
8u]; extern int isalnum( int); extern int isalpha( int); extern int isascii( int);
extern int iscntrl( int); extern int isdigit( int); extern int isgraph( int);
extern int islower( int); extern int isprint( int); extern int ispunct( int);
extern int isspace( int); extern int isupper( int); extern int isxdigit( int);
extern int toascii( int); extern int tolower( int); extern int toupper( int);
extern int _tolower( int); extern int _toupper( int); extern struct _tagged_arr
Cyc_Std_strerror( int); extern unsigned int Cyc_Std_strlen( struct _tagged_arr s);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2); extern
int Cyc_Std_strncmp( struct _tagged_arr s1, struct _tagged_arr s2, unsigned int
len); extern int Cyc_Std_zstrcmp( struct _tagged_arr, struct _tagged_arr);
extern int Cyc_Std_zstrncmp( struct _tagged_arr s1, struct _tagged_arr s2,
unsigned int n); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); extern int Cyc_Std_strcasecmp( struct _tagged_arr, struct
_tagged_arr); extern int Cyc_Std_strncasecmp( struct _tagged_arr s1, struct
_tagged_arr s2, unsigned int len); extern struct _tagged_arr Cyc_Std_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_rstrconcat(
struct _RegionHandle*, struct _tagged_arr, struct _tagged_arr); extern struct
_tagged_arr Cyc_Std_strconcat_l( struct Cyc_List_List*); extern struct
_tagged_arr Cyc_Std_rstrconcat_l( struct _RegionHandle*, struct Cyc_List_List*);
extern struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_rstr_sepstr( struct
_RegionHandle*, struct Cyc_List_List*, struct _tagged_arr); extern struct
_tagged_arr Cyc_Std_strcpy( struct _tagged_arr dest, struct _tagged_arr src);
extern struct _tagged_arr Cyc_Std_strncpy( struct _tagged_arr, struct
_tagged_arr, unsigned int); extern struct _tagged_arr Cyc_Std_zstrncpy( struct
_tagged_arr, struct _tagged_arr, unsigned int); extern struct _tagged_arr Cyc_Std_expand(
struct _tagged_arr s, unsigned int sz); extern struct _tagged_arr Cyc_Std_rexpand(
struct _RegionHandle*, struct _tagged_arr s, unsigned int sz); extern struct
_tagged_arr Cyc_Std_realloc_str( struct _tagged_arr str, unsigned int sz);
extern struct _tagged_arr Cyc_Std_rrealloc_str( struct _RegionHandle* r, struct
_tagged_arr str, unsigned int sz); extern struct _tagged_arr Cyc_Std_strdup(
struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_rstrdup( struct
_RegionHandle*, struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr, int ofs, unsigned int n); extern struct _tagged_arr Cyc_Std_rsubstring(
struct _RegionHandle*, struct _tagged_arr, int ofs, unsigned int n); extern
struct _tagged_arr Cyc_Std_replace_suffix( struct _tagged_arr, struct
_tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_rreplace_suffix(
struct _RegionHandle* r, struct _tagged_arr src, struct _tagged_arr curr_suffix,
struct _tagged_arr new_suffix); extern struct _tagged_arr Cyc_Std_strchr( struct
_tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_Std_mstrchr(
struct _tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_Std_mstrrchr(
struct _tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_Std_strrchr(
struct _tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_Std_mstrstr(
struct _tagged_arr haystack, struct _tagged_arr needle); extern struct
_tagged_arr Cyc_Std_strstr( struct _tagged_arr haystack, struct _tagged_arr
needle); extern struct _tagged_arr Cyc_Std_strpbrk( struct _tagged_arr s, struct
_tagged_arr accept); extern struct _tagged_arr Cyc_Std_mstrpbrk( struct
_tagged_arr s, struct _tagged_arr accept); extern unsigned int Cyc_Std_strspn(
struct _tagged_arr s, struct _tagged_arr accept); extern unsigned int Cyc_Std_strcspn(
struct _tagged_arr s, struct _tagged_arr accept); extern struct _tagged_arr Cyc_Std_strtok(
struct _tagged_arr s, struct _tagged_arr delim); extern struct Cyc_List_List*
Cyc_Std_explode( struct _tagged_arr s); extern struct Cyc_List_List* Cyc_Std_rexplode(
struct _RegionHandle*, struct _tagged_arr s); extern struct _tagged_arr Cyc_Std_implode(
struct Cyc_List_List* c); extern int Cyc_Std_strcasecmp( struct _tagged_arr,
struct _tagged_arr); extern int Cyc_Std_strncasecmp( struct _tagged_arr s1,
struct _tagged_arr s2, unsigned int len); extern unsigned char* strerror( int
errnum); struct _tagged_arr Cyc_Std_strerror( int errnum){ return( struct
_tagged_arr) wrap_Cstring_as_string( strerror( errnum), - 1);} unsigned int Cyc_Std_strlen(
struct _tagged_arr s){ unsigned int i; for( i= 0; i <  _get_arr_size( s, sizeof(
unsigned char)); i ++){ if(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char),( int) i)) == '\000'){ return i;}} return i;} static
unsigned int Cyc_Std_int_strleno( struct _tagged_arr s, struct _tagged_arr error){
int i; for( i= 0; i <  _get_arr_size( s, sizeof( unsigned char)); i ++){ if(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
== '\000'){ break;}} return( unsigned int) i;} static int Cyc_Std_case_cmp(
unsigned char c1, unsigned char c2){ return c1 -  c2;} static int Cyc_Std_cmp(
struct _tagged_arr s1, unsigned int len1, struct _tagged_arr s2, unsigned int
len2, int(* f)( unsigned char, unsigned char)){ unsigned int min_length= len1 < 
len2? len1: len2; int i= - 1; while( ++ i <  min_length) { int diff= f(*(( const
unsigned char*) _check_unknown_subscript( s1, sizeof( unsigned char), i)),*((
const unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i)));
if( diff !=  0){ return diff;}} return( int) len1 - ( int) len2;} int Cyc_Std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2){ if( s1.curr ==  s2.curr){ return
0;}{ unsigned int len1= Cyc_Std_int_strleno( s1, _tag_arr("Std::strcmp", sizeof(
unsigned char), 12u)); unsigned int len2= Cyc_Std_int_strleno( s2, _tag_arr("Std::strcmp",
sizeof( unsigned char), 12u)); return Cyc_Std_cmp( s1, len1, s2, len2, Cyc_Std_case_cmp);}}
int Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2){ return
Cyc_Std_strcmp(* s1,* s2);} static int Cyc_Std_ncmp( struct _tagged_arr s1,
unsigned int len1, struct _tagged_arr s2, unsigned int len2, unsigned int n, int(*
f)( unsigned char, unsigned char)){ if( n <=  0){ return 0;}{ unsigned int
min_len= len1 >  len2? len2: len1; unsigned int bound= min_len >  n? n: min_len;{
int i= 0; for( 0; i <  bound; i ++){ int retc; if(( retc= f(*(( const
unsigned char*) _check_unknown_subscript( s1, sizeof( unsigned char), i)),*((
const unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i))))
!=  0){ return retc;}}} if( len1 <  n? 1: len2 <  n){ return( int) len1 - ( int)
len2;} return 0;}} int Cyc_Std_strncmp( struct _tagged_arr s1, struct
_tagged_arr s2, unsigned int n){ unsigned int len1= Cyc_Std_int_strleno( s1,
_tag_arr("Std::strncmp", sizeof( unsigned char), 13u)); unsigned int len2= Cyc_Std_int_strleno(
s2, _tag_arr("Std::strncmp", sizeof( unsigned char), 13u)); return Cyc_Std_ncmp(
s1, len1, s2, len2, n, Cyc_Std_case_cmp);} int Cyc_Std_zstrcmp( struct
_tagged_arr a, struct _tagged_arr b){ if( a.curr ==  b.curr){ return 0;}{
unsigned int as= _get_arr_size( a, sizeof( unsigned char)); unsigned int bs=
_get_arr_size( b, sizeof( unsigned char)); unsigned int min_length= as <  bs? as:
bs; int i= - 1; while( ++ i <  min_length) { int diff=( int)*(( const
unsigned char*) _check_unknown_subscript( a, sizeof( unsigned char), i)) - ( int)*((
const unsigned char*) _check_unknown_subscript( b, sizeof( unsigned char), i));
if( diff !=  0){ return diff;}} return( int) as - ( int) bs;}} int Cyc_Std_zstrncmp(
struct _tagged_arr s1, struct _tagged_arr s2, unsigned int n){ if( n <=  0){
return 0;}{ unsigned int s1size= _get_arr_size( s1, sizeof( unsigned char));
unsigned int s2size= _get_arr_size( s2, sizeof( unsigned char)); unsigned int
min_size= s1size >  s2size? s2size: s1size; unsigned int bound= min_size >  n? n:
min_size;{ int i= 0; for( 0; i <  bound; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s1, sizeof( unsigned char), i)) < *(( const
unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i))){
return - 1;} else{ if(*(( const unsigned char*) _check_unknown_subscript( s2,
sizeof( unsigned char), i)) < *(( const unsigned char*) _check_unknown_subscript(
s1, sizeof( unsigned char), i))){ return 1;}}}} if( min_size <=  bound){ return
0;} if( s1size <  s2size){ return - 1;} else{ return 1;}}} int Cyc_Std_zstrptrcmp(
struct _tagged_arr* a, struct _tagged_arr* b){ return Cyc_Std_zstrcmp(* a,* b);}
static struct _tagged_arr Cyc_Std_int_strcato( struct _tagged_arr dest, struct
_tagged_arr src, struct _tagged_arr error){ int i; unsigned int dsize;
unsigned int slen; unsigned int dlen; dsize= _get_arr_size( dest, sizeof(
unsigned char)); dlen= Cyc_Std_strlen(( struct _tagged_arr) dest); slen= Cyc_Std_int_strleno(
src, error); if( slen +  dlen <=  dsize){ for( i= 0; i <  slen; i ++){*((
unsigned char*) _check_unknown_subscript( dest, sizeof( unsigned char),( int)( i
+  dlen)))=*(( const unsigned char*) _check_unknown_subscript( src, sizeof(
unsigned char), i));} if( i !=  dsize){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char),( int)( i +  dlen)))='\000';}}
else{( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp0=(
struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp0[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp1; _temp1.tag= Cyc_Core_Invalid_argument;
_temp1.f1= error; _temp1;}); _temp0;}));} return dest;} struct _tagged_arr Cyc_Std_strcat(
struct _tagged_arr dest, struct _tagged_arr src){ return Cyc_Std_int_strcato(
dest, src, _tag_arr("Std::strcat", sizeof( unsigned char), 12u));} struct
_tagged_arr Cyc_Std_rstrconcat( struct _RegionHandle* r, struct _tagged_arr a,
struct _tagged_arr b){ unsigned int _temp2= Cyc_Std_strlen( a); unsigned int
_temp3= Cyc_Std_strlen( b); struct _tagged_arr ans= Cyc_Core_rnew_string( r,(
int)(( _temp2 +  _temp3) +  1)); int i; int j; for( i= 0; i <  _temp2; ++ i){*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), i))=*((
const unsigned char*) _check_unknown_subscript( a, sizeof( unsigned char), i));}
for( j= 0; j <  _temp3; ++ j){*(( unsigned char*) _check_unknown_subscript( ans,
sizeof( unsigned char), i +  j))=*(( const unsigned char*)
_check_unknown_subscript( b, sizeof( unsigned char), j));} return ans;} struct
_tagged_arr Cyc_Std_strconcat( struct _tagged_arr a, struct _tagged_arr b){
return Cyc_Std_rstrconcat( Cyc_Core_heap_region, a, b);} struct _tagged_arr Cyc_Std_rstrconcat_l(
struct _RegionHandle* r, struct Cyc_List_List* strs){ unsigned int len;
unsigned int total_len= 0; struct _tagged_arr ans;{ struct _RegionHandle _temp4=
_new_region("temp"); struct _RegionHandle* temp=& _temp4; _push_region( temp);{
struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp7->hd=( void*)((
unsigned int) 0); _temp7->tl= 0; _temp7;}); struct Cyc_List_List* end= lens;{
struct Cyc_List_List* p= strs; for( 0; p !=  0; p= p->tl){ len= Cyc_Std_strlen(*((
struct _tagged_arr*) p->hd)); total_len += len;(( struct Cyc_List_List*)
_check_null( end))->tl=({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp5->hd=( void*) len;
_temp5->tl= 0; _temp5;}); end=(( struct Cyc_List_List*) _check_null( end))->tl;}}
lens= lens->tl; ans= Cyc_Core_rnew_string( r,( int)( total_len +  1));{
unsigned int i= 0; while( strs !=  0) { struct _tagged_arr _temp6=*(( struct
_tagged_arr*) strs->hd); len=( unsigned int)(( struct Cyc_List_List*)
_check_null( lens))->hd; Cyc_Std_strncpy( _tagged_arr_plus( ans, sizeof(
unsigned char),( int) i), _temp6, len); i += len; strs= strs->tl; lens= lens->tl;}}};
_pop_region( temp);} return ans;} struct _tagged_arr Cyc_Std_strconcat_l( struct
Cyc_List_List* strs){ return Cyc_Std_rstrconcat_l( Cyc_Core_heap_region, strs);}
struct _tagged_arr Cyc_Std_rstr_sepstr( struct _RegionHandle* r, struct Cyc_List_List*
strs, struct _tagged_arr separator){ if( strs ==  0){ return Cyc_Core_rnew_string(
r, 0);} if( strs->tl ==  0){ return Cyc_Std_rstrdup( r,*(( struct _tagged_arr*)
strs->hd));}{ struct Cyc_List_List* _temp8= strs; struct _RegionHandle _temp9=
_new_region("temp"); struct _RegionHandle* temp=& _temp9; _push_region( temp);{
struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp13->hd=( void*)((
unsigned int) 0); _temp13->tl= 0; _temp13;}); struct Cyc_List_List* end= lens;
unsigned int len= 0; unsigned int total_len= 0; unsigned int list_len= 0; for( 0;
_temp8 !=  0; _temp8= _temp8->tl){ len= Cyc_Std_strlen(*(( struct _tagged_arr*)
_temp8->hd)); total_len += len;(( struct Cyc_List_List*) _check_null( end))->tl=({
struct Cyc_List_List* _temp10=( struct Cyc_List_List*) _region_malloc( temp,
sizeof( struct Cyc_List_List)); _temp10->hd=( void*) len; _temp10->tl= 0;
_temp10;}); end=(( struct Cyc_List_List*) _check_null( end))->tl; ++ list_len;}
lens= lens->tl;{ unsigned int seplen= Cyc_Std_strlen( separator); total_len +=(
list_len -  1) *  seplen;{ struct _tagged_arr ans= Cyc_Core_rnew_string( r,( int)
total_len); unsigned int i= 0; while( strs->tl !=  0) { struct _tagged_arr
_temp11=*(( struct _tagged_arr*) strs->hd); len=( unsigned int)(( struct Cyc_List_List*)
_check_null( lens))->hd; Cyc_Std_strncpy( _tagged_arr_plus( ans, sizeof(
unsigned char),( int) i), _temp11, len); i += len; Cyc_Std_strncpy(
_tagged_arr_plus( ans, sizeof( unsigned char),( int) i), separator, seplen); i +=
seplen; strs= strs->tl; lens= lens->tl;} Cyc_Std_strncpy( _tagged_arr_plus( ans,
sizeof( unsigned char),( int) i),*(( struct _tagged_arr*) strs->hd),(
unsigned int)(( struct Cyc_List_List*) _check_null( lens))->hd);{ struct
_tagged_arr _temp12= ans; _npop_handler( 0u); return _temp12;}}}}; _pop_region(
temp);}} struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List* strs,
struct _tagged_arr separator){ return Cyc_Std_rstr_sepstr( Cyc_Core_heap_region,
strs, separator);} struct _tagged_arr Cyc_Std_strncpy( struct _tagged_arr dest,
struct _tagged_arr src, unsigned int n){ unsigned int _temp14= _get_arr_size(
dest, sizeof( unsigned char)); unsigned int _temp15= _get_arr_size( src, sizeof(
unsigned char)); if( n <=  0){ return dest;} if( n >  _temp14? 1: n >  _temp15){(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp16=( struct
Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp16[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp17; _temp17.tag= Cyc_Core_Invalid_argument;
_temp17.f1= _tag_arr("Std::strncpy", sizeof( unsigned char), 13u); _temp17;});
_temp16;}));}{ int i; for( i= 0; i <  n; i ++){ unsigned char _temp18=*(( const
unsigned char*) _check_unknown_subscript( src, sizeof( unsigned char), i)); if(
_temp18 == '\000'){ break;}*(( unsigned char*) _check_unknown_subscript( dest,
sizeof( unsigned char), i))= _temp18;} for( 0; i <  n; i ++){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char), i))='\000';} return dest;}}
struct _tagged_arr Cyc_Std_zstrncpy( struct _tagged_arr dest, struct _tagged_arr
src, unsigned int n){ unsigned int _temp19= _get_arr_size( dest, sizeof(
unsigned char)); unsigned int _temp20= _get_arr_size( src, sizeof( unsigned char));
if( n <=  0){ return dest;} if( n >  _temp19? 1: n >  _temp20){( int) _throw((
void*)({ struct Cyc_Core_Invalid_argument_struct* _temp21=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp21[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp22; _temp22.tag= Cyc_Core_Invalid_argument;
_temp22.f1= _tag_arr("Std::zstrncpy", sizeof( unsigned char), 14u); _temp22;});
_temp21;}));}{ int i; for( i= 0; i <  n; i ++){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char), i))=*(( const
unsigned char*) _check_unknown_subscript( src, sizeof( unsigned char), i));}
return dest;}} struct _tagged_arr Cyc_Std_strcpy( struct _tagged_arr dest,
struct _tagged_arr src){ unsigned int len= Cyc_Std_strlen( src); Cyc_Std_strncpy(
dest, src, len); if( len !=  _get_arr_size( dest, sizeof( unsigned char))){*((
unsigned char*) _check_unknown_subscript( dest, sizeof( unsigned char),( int)
len))='\000';} return dest;} struct _tagged_arr Cyc_Std_rstrdup( struct
_RegionHandle* r, struct _tagged_arr src){ unsigned int len; struct _tagged_arr
temp; len= Cyc_Std_strlen( src); temp= Cyc_Core_rnew_string( r,( int)( len +  1));
Cyc_Std_strncpy( temp, src, len); return temp;} struct _tagged_arr Cyc_Std_strdup(
struct _tagged_arr src){ return Cyc_Std_rstrdup( Cyc_Core_heap_region, src);}
struct _tagged_arr Cyc_Std_rexpand( struct _RegionHandle* r, struct _tagged_arr
s, unsigned int sz){ struct _tagged_arr temp; unsigned int slen; slen= Cyc_Std_strlen(
s); sz= sz >  slen? sz: slen; temp= Cyc_Core_rnew_string( r,( int) sz); Cyc_Std_strncpy(
temp, s, slen); if( slen !=  _get_arr_size( s, sizeof( unsigned char))){*((
unsigned char*) _check_unknown_subscript( temp, sizeof( unsigned char),( int)
slen))='\000';} return temp;} struct _tagged_arr Cyc_Std_expand( struct
_tagged_arr s, unsigned int sz){ return Cyc_Std_rexpand( Cyc_Core_heap_region, s,
sz);} struct _tagged_arr Cyc_Std_rrealloc_str( struct _RegionHandle* r, struct
_tagged_arr str, unsigned int sz){ unsigned int maxsizeP= _get_arr_size( str,
sizeof( unsigned char)); if( maxsizeP ==  0){ maxsizeP= 30 >  sz? 30: sz; str=
Cyc_Core_rnew_string( r,( int) maxsizeP);*(( unsigned char*)
_check_unknown_subscript( str, sizeof( unsigned char), 0))='\000';} else{ if( sz
>  maxsizeP){ maxsizeP= maxsizeP *  2 > ( sz *  5) /  4? maxsizeP *  2:( sz *  5)
/  4; str= Cyc_Std_rexpand( r,( struct _tagged_arr) str, maxsizeP);}} return str;}
struct _tagged_arr Cyc_Std_realloc_str( struct _tagged_arr str, unsigned int sz){
return Cyc_Std_rrealloc_str( Cyc_Core_heap_region, str, sz);} struct _tagged_arr
Cyc_Std_rsubstring( struct _RegionHandle* r, struct _tagged_arr s, int start,
unsigned int amt){ if(( start <  0? 1: amt <  0)? 1:( start + ( int) amt) -  1 > (
int) _get_arr_size( s, sizeof( unsigned char))){( int) _throw(( void*)({ struct
Cyc_Core_Invalid_argument_struct* _temp23=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp23[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp24; _temp24.tag= Cyc_Core_Invalid_argument;
_temp24.f1= _tag_arr("Std::substring", sizeof( unsigned char), 15u); _temp24;});
_temp23;}));}{ struct _tagged_arr ans= Cyc_Core_rnew_string( r,( int)( amt +  1));{
unsigned int i= 0; for( 0; i <  amt; ++ i){*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char),( int) i))=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),( int)(
start +  i)));}}*(( unsigned char*) _check_unknown_subscript( ans, sizeof(
unsigned char),( int) amt))='\000'; return ans;}} struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr s, int start, unsigned int amt){ return Cyc_Std_rsubstring(
Cyc_Core_heap_region, s, start, amt);} struct _tagged_arr Cyc_Std_rreplace_suffix(
struct _RegionHandle* r, struct _tagged_arr src, struct _tagged_arr curr_suffix,
struct _tagged_arr new_suffix){ unsigned int m= _get_arr_size( src, sizeof(
unsigned char)); unsigned int n= _get_arr_size( curr_suffix, sizeof(
unsigned char)); struct _tagged_arr err= _tag_arr("Std::replace_suffix", sizeof(
unsigned char), 20u); if( m <  n){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp25=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp25[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp26; _temp26.tag= Cyc_Core_Invalid_argument; _temp26.f1= err; _temp26;});
_temp25;}));}{ unsigned int i= 1; for( 0; i <=  n; ++ i){ if(*(( const
unsigned char*) _check_unknown_subscript( src, sizeof( unsigned char),( int)( m
-  i))) != *(( const unsigned char*) _check_unknown_subscript( curr_suffix,
sizeof( unsigned char),( int)( n -  i)))){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp27=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp27[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp28; _temp28.tag= Cyc_Core_Invalid_argument; _temp28.f1= err; _temp28;});
_temp27;}));}}}{ struct _tagged_arr ans= Cyc_Core_rnew_string( r,( int)(( m -  n)
+  _get_arr_size( new_suffix, sizeof( unsigned char)))); Cyc_Std_strncpy( ans,
src, m -  n); Cyc_Std_strncpy( _tagged_arr_plus( _tagged_arr_plus( ans, sizeof(
unsigned char),( int) m), sizeof( unsigned char), -( int) n), new_suffix,
_get_arr_size( new_suffix, sizeof( unsigned char))); return ans;}} struct
_tagged_arr Cyc_Std_replace_suffix( struct _tagged_arr src, struct _tagged_arr
curr_suffix, struct _tagged_arr new_suffix){ return Cyc_Std_rreplace_suffix( Cyc_Core_heap_region,
src, curr_suffix, new_suffix);} struct _tagged_arr Cyc_Std_strpbrk( struct
_tagged_arr s, struct _tagged_arr accept){ int len=( int) Cyc_Std_int_strleno( s,
_tag_arr("Std::strpbrk", sizeof( unsigned char), 13u)); unsigned char c='\000';
for( 0; len >  0?( c=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0u))) !=  0: 0;( len --, _tagged_arr_inplace_plus_post(&
s, sizeof( unsigned char), 1))){ int j= 0; for( 0; j <  _get_arr_size( accept,
sizeof( unsigned char)); j ++){ if( c == *(( const unsigned char*)
_check_unknown_subscript( accept, sizeof( unsigned char), j))){ return s;}}}
return( struct _tagged_arr) _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_Std_mstrpbrk(
struct _tagged_arr s, struct _tagged_arr accept){ int len=( int) Cyc_Std_int_strleno((
struct _tagged_arr) s, _tag_arr("Std::strpbrk", sizeof( unsigned char), 13u));
unsigned char c='\000'; for( 0; len >  0?( c=*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0u))) !=  0: 0;( len --,
_tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), 1))){ int j= 0; for(
0; j <  _get_arr_size( accept, sizeof( unsigned char)); j ++){ if( c == *((
const unsigned char*) _check_unknown_subscript( accept, sizeof( unsigned char),
j))){ return s;}}} return _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_Std_mstrchr(
struct _tagged_arr s, unsigned char c){ int len=( int) Cyc_Std_int_strleno((
struct _tagged_arr) s, _tag_arr("Std::strchr", sizeof( unsigned char), 12u));
unsigned char c2='\000'; for( 0; len >  0?( c2=*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0u))) !=  0: 0;( len --,
_tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), 1))){ if( c2 ==  c){
return s;}} return _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_Std_strchr(
struct _tagged_arr s, unsigned char c){ int len=( int) Cyc_Std_int_strleno( s,
_tag_arr("Std::strchr", sizeof( unsigned char), 12u)); unsigned char c2='\000';
for( 0; len >  0?( c2=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0u))) !=  0: 0;( len --, _tagged_arr_inplace_plus_post(&
s, sizeof( unsigned char), 1))){ if( c2 ==  c){ return s;}} return( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_Std_strrchr( struct
_tagged_arr s, unsigned char c){ int len=( int) Cyc_Std_int_strleno( s, _tag_arr("Std::strrchr",
sizeof( unsigned char), 13u)); int i= len -  1; _tagged_arr_inplace_plus(& s,
sizeof( unsigned char), i); for( 0; i >=  0;( i --,
_tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), -1))){ if(*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0u)) ==  c){
return s;}} return( struct _tagged_arr) _tag_arr( 0u, 0u, 0u);} struct
_tagged_arr Cyc_Std_mstrrchr( struct _tagged_arr s, unsigned char c){ int len=(
int) Cyc_Std_int_strleno(( struct _tagged_arr) s, _tag_arr("Std::mstrrchr",
sizeof( unsigned char), 14u)); int i= len -  1; _tagged_arr_inplace_plus(& s,
sizeof( unsigned char), i); for( 0; i >=  0;( i --,
_tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), -1))){ if(*((
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0u)) ==  c){
return s;}} return _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_Std_strstr(
struct _tagged_arr haystack, struct _tagged_arr needle){ if( !(( unsigned int)
haystack.curr)? 1: !(( unsigned int) needle.curr)){( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp29=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp29[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp30; _temp30.tag= Cyc_Core_Invalid_argument;
_temp30.f1= _tag_arr("Std::strstr", sizeof( unsigned char), 12u); _temp30;});
_temp29;}));} if(*(( const unsigned char*) _check_unknown_subscript( needle,
sizeof( unsigned char), 0u)) == '\000'){ return haystack;}{ int len=( int) Cyc_Std_int_strleno(
needle, _tag_arr("Std::strstr", sizeof( unsigned char), 12u));{ struct
_tagged_arr start= haystack; for( 0;( start= Cyc_Std_strchr( start,*(( const
unsigned char*) _check_unknown_subscript( needle, sizeof( unsigned char), 0u)))).curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr; start= Cyc_Std_strchr(
_tagged_arr_plus( start, sizeof( unsigned char), 1),*(( const unsigned char*)
_check_unknown_subscript( needle, sizeof( unsigned char), 0u)))){ if( Cyc_Std_strncmp(
start, needle,( unsigned int) len) ==  0){ return start;}}} return( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u);}} struct _tagged_arr Cyc_Std_mstrstr( struct
_tagged_arr haystack, struct _tagged_arr needle){ if( !(( unsigned int) haystack.curr)?
1: !(( unsigned int) needle.curr)){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp31=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp31[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp32; _temp32.tag= Cyc_Core_Invalid_argument; _temp32.f1= _tag_arr("Std::mstrstr",
sizeof( unsigned char), 13u); _temp32;}); _temp31;}));} if(*(( const
unsigned char*) _check_unknown_subscript( needle, sizeof( unsigned char), 0u))
== '\000'){ return haystack;}{ int len=( int) Cyc_Std_int_strleno( needle,
_tag_arr("Std::mstrstr", sizeof( unsigned char), 13u));{ struct _tagged_arr
start= haystack; for( 0;( start= Cyc_Std_mstrchr( start,*(( const unsigned char*)
_check_unknown_subscript( needle, sizeof( unsigned char), 0u)))).curr != (
_tag_arr( 0u, 0u, 0u)).curr; start= Cyc_Std_mstrchr( _tagged_arr_plus( start,
sizeof( unsigned char), 1),*(( const unsigned char*) _check_unknown_subscript(
needle, sizeof( unsigned char), 0u)))){ if( Cyc_Std_strncmp(( struct _tagged_arr)
start, needle,( unsigned int) len) ==  0){ return start;}}} return _tag_arr( 0u,
0u, 0u);}} unsigned int Cyc_Std_strspn( struct _tagged_arr s, struct _tagged_arr
accept){ unsigned int len= Cyc_Std_int_strleno( s, _tag_arr("Std::strspn",
sizeof( unsigned char), 12u));{ unsigned int i= 0; for( 0; i <  len; i ++){ int
j; for( j= 0; j <  _get_arr_size( accept, sizeof( unsigned char)); j ++){ if(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),( int)
i)) == *(( const unsigned char*) _check_unknown_subscript( accept, sizeof(
unsigned char), j))){ break;}} if( j ==  _get_arr_size( accept, sizeof(
unsigned char))){ return i;}}} return len;} unsigned int Cyc_Std_strcspn( struct
_tagged_arr s, struct _tagged_arr accept){ unsigned int len= Cyc_Std_int_strleno(
s, _tag_arr("Std::strspn", sizeof( unsigned char), 12u));{ unsigned int i= 0;
for( 0; i <  len; i ++){ int j; for( j= 0; j <  _get_arr_size( accept, sizeof(
unsigned char)); j ++){ if(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char),( int) i)) != *(( const unsigned char*)
_check_unknown_subscript( accept, sizeof( unsigned char), j))){ break;}} if( j
==  _get_arr_size( accept, sizeof( unsigned char))){ return i;}}} return len;}
struct _tagged_arr Cyc_Std_strtok( struct _tagged_arr s, struct _tagged_arr
delim){ static struct _tagged_arr olds={( void*) 0u,( void*) 0u,( void*)( 0u + 
0u)}; struct _tagged_arr token; if( s.curr == ( _tag_arr( 0u, 0u, 0u)).curr){
if( olds.curr == ( _tag_arr( 0u, 0u, 0u)).curr){ return _tag_arr( 0u, 0u, 0u);}
s= olds;}{ unsigned int inc= Cyc_Std_strspn(( struct _tagged_arr) s, delim); if(
inc >=  _get_arr_size( s, sizeof( unsigned char))? 1:*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_plus( s, sizeof( unsigned char),( int) inc),
sizeof( unsigned char), 0u)) == '\000'){ olds= _tag_arr( 0u, 0u, 0u); return
_tag_arr( 0u, 0u, 0u);} else{ _tagged_arr_inplace_plus(& s, sizeof(
unsigned char),( int) inc);} token= s; s= Cyc_Std_mstrpbrk( token, delim); if( s.curr
== ( _tag_arr( 0u, 0u, 0u)).curr){ olds= _tag_arr( 0u, 0u, 0u);} else{*((
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0u))='\000';
olds= _tagged_arr_plus( s, sizeof( unsigned char), 1);} return token;}} struct
Cyc_List_List* Cyc_Std_rexplode( struct _RegionHandle* r, struct _tagged_arr s){
struct Cyc_List_List* result= 0;{ int i=( int)( Cyc_Std_strlen( s) -  1); for( 0;
i >=  0; i --){ result=({ struct Cyc_List_List* _temp33=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp33->hd=( void*)(( int)*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)));
_temp33->tl= result; _temp33;});}} return result;} struct Cyc_List_List* Cyc_Std_explode(
struct _tagged_arr s){ return Cyc_Std_rexplode( Cyc_Core_heap_region, s);}
struct _tagged_arr Cyc_Std_implode( struct Cyc_List_List* chars){ struct
_tagged_arr s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
chars)); unsigned int i= 0; while( chars !=  0) {*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char),( int) i ++))=(
unsigned char)(( int) chars->hd); chars= chars->tl;} return s;} int Cyc_Std_strcasecmp(
struct _tagged_arr s1, struct _tagged_arr s2){ if( s1.curr ==  s2.curr){ return
0;}{ unsigned int len1= Cyc_Std_int_strleno( s1, _tag_arr("Std::strcasecmp",
sizeof( unsigned char), 16u)); unsigned int len2= Cyc_Std_int_strleno( s2,
_tag_arr("Std::strcasecmp", sizeof( unsigned char), 16u)); return Cyc_Std_cmp(
s1, len1, s2, len2, Cyc_Std_case_cmp);}} static int Cyc_Std_nocase_cmp(
unsigned char c1, unsigned char c2){ return Cyc_Std_case_cmp(( unsigned char)
toupper(( int) c1),( unsigned char) toupper(( int) c2));} int Cyc_Std_strncasecmp(
struct _tagged_arr s1, struct _tagged_arr s2, unsigned int n){ unsigned int len1=
Cyc_Std_int_strleno( s1, _tag_arr("Std::strncasecmp", sizeof( unsigned char), 17u));
unsigned int len2= Cyc_Std_int_strleno( s2, _tag_arr("Std::strncasecmp", sizeof(
unsigned char), 17u)); return Cyc_Std_ncmp( s1, len1, s2, len2, n, Cyc_Std_nocase_cmp);}
