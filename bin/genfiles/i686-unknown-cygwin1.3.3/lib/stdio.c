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
  struct _RegionPage *next;
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
};

extern struct _RegionHandle _new_region();
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

#ifdef CYC_REGION_PROFILE
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
#define _cycalloc(n) _profile_cycalloc(n,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_cycalloc_atomic(n,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FUNCTION__,__LINE__)
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern int Cyc_Std_remove(
struct _tagged_arr); extern int Cyc_Std_rename( struct _tagged_arr, struct
_tagged_arr); extern struct Cyc_Std___sFILE* Cyc_Std_tmpfile(); extern int Cyc_Std_fclose(
struct Cyc_Std___sFILE*); extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*);
extern struct Cyc_Std___sFILE* Cyc_Std_fopen( struct _tagged_arr __filename,
struct _tagged_arr __modes); extern struct Cyc_Std___sFILE* Cyc_Std_freopen(
struct _tagged_arr, struct _tagged_arr, struct Cyc_Std___sFILE*); extern void
Cyc_Std_setbuf( struct Cyc_Std___sFILE* __stream, struct _tagged_arr __buf);
extern int Cyc_Std_setvbuf( struct Cyc_Std___sFILE* __stream, struct _tagged_arr
__buf, int __modes, unsigned int __n); extern int Cyc_Std_fgetc( struct Cyc_Std___sFILE*
__stream); extern int Cyc_Std_getc( struct Cyc_Std___sFILE* __stream); extern
struct _tagged_arr Cyc_Std_fgets( struct _tagged_arr __s, int __n, struct Cyc_Std___sFILE*
__stream); extern int Cyc_Std_fputc( int __c, struct Cyc_Std___sFILE* __stream);
extern int Cyc_Std_putc( int __c, struct Cyc_Std___sFILE* __stream); extern int
Cyc_Std_fputs( struct _tagged_arr __s, struct Cyc_Std___sFILE* __stream); extern
int Cyc_Std_puts( struct _tagged_arr __s); extern int Cyc_Std_ungetc( int __c,
struct Cyc_Std___sFILE* __stream); extern unsigned int Cyc_Std_fread( struct
_tagged_arr __ptr, unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE*
__stream); extern unsigned int Cyc_Std_fwrite( struct _tagged_arr __ptr,
unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE* __s); extern int
Cyc_Std_fseek( struct Cyc_Std___sFILE* __stream, int __off, int __whence);
extern int Cyc_Std_ftell( struct Cyc_Std___sFILE* __stream); extern void Cyc_Std_rewind(
struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_fgetpos( struct Cyc_Std___sFILE*
__stream, int* __pos); extern int Cyc_Std_fsetpos( struct Cyc_Std___sFILE*
__stream, int* __pos); extern void Cyc_Std_clearerr( struct Cyc_Std___sFILE*
__stream); extern int Cyc_Std_feof( struct Cyc_Std___sFILE* __stream); extern
int Cyc_Std_ferror( struct Cyc_Std___sFILE* __stream); extern void Cyc_Std_perror(
struct _tagged_arr __s); extern struct Cyc_Std___sFILE* Cyc_Std_fdopen( int __fd,
struct _tagged_arr __modes); extern int Cyc_Std_fileno( struct Cyc_Std___sFILE*
__stream); extern int Cyc_Std_getw( struct Cyc_Std___sFILE* __stream); extern
int Cyc_Std_putw( int __w, struct Cyc_Std___sFILE* __stream); extern void Cyc_Std_setbuffer(
struct Cyc_Std___sFILE* __stream, struct _tagged_arr __buf, unsigned int __size);
extern void Cyc_Std_setlinebuf( struct Cyc_Std___sFILE* __stream); extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern struct Cyc_Std___sFILE* Cyc_Std_file_open( struct
_tagged_arr fname, struct _tagged_arr mode); extern void Cyc_Std_file_close(
struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[
8u]; struct Cyc_Cstdio___sFILE; extern int remove( unsigned char*); extern int
rename( unsigned char*, unsigned char*); extern struct Cyc_Cstdio___sFILE*
tmpfile(); extern int fclose( struct Cyc_Cstdio___sFILE*); extern int fflush(
struct Cyc_Cstdio___sFILE*); extern struct Cyc_Cstdio___sFILE* fopen(
unsigned char* __filename, unsigned char* __modes); extern struct Cyc_Cstdio___sFILE*
freopen( unsigned char* __filename, unsigned char* __modes, struct Cyc_Cstdio___sFILE*
__stream); extern struct Cyc_Cstdio___sFILE* fdopen( int __fd, unsigned char*
__modes); extern int setvbuf( struct Cyc_Cstdio___sFILE* __stream, unsigned char*
__buf, int __modes, unsigned int __n); extern void setbuffer( struct Cyc_Cstdio___sFILE*
__stream, unsigned char* __buf, unsigned int __size); extern void setlinebuf(
struct Cyc_Cstdio___sFILE* __stream); extern int fgetc( struct Cyc_Cstdio___sFILE*
__stream); extern int getc( struct Cyc_Cstdio___sFILE* __stream); extern int
fputc( int __c, struct Cyc_Cstdio___sFILE* __stream); extern int putc( int __c,
struct Cyc_Cstdio___sFILE* __stream); extern int getw( struct Cyc_Cstdio___sFILE*
__stream); extern int putw( int __w, struct Cyc_Cstdio___sFILE* __stream);
extern unsigned char* fgets( unsigned char* __s, int __n, struct Cyc_Cstdio___sFILE*
__stream); extern unsigned char* gets( unsigned char* __s); extern int fputs(
unsigned char* __s, struct Cyc_Cstdio___sFILE* __stream); extern int puts(
unsigned char* __s); extern int ungetc( int __c, struct Cyc_Cstdio___sFILE*
__stream); extern unsigned int fread( unsigned char* __ptr, unsigned int __size,
unsigned int __n, struct Cyc_Cstdio___sFILE* __stream); extern unsigned int
fwrite( unsigned char* __ptr, unsigned int __size, unsigned int __n, struct Cyc_Cstdio___sFILE*
__s); extern int fseek( struct Cyc_Cstdio___sFILE* __stream, int __off, int
__whence); extern int ftell( struct Cyc_Cstdio___sFILE* __stream); extern void
rewind( struct Cyc_Cstdio___sFILE* __stream); extern int fgetpos( struct Cyc_Cstdio___sFILE*
__stream, int* __pos); extern int fsetpos( struct Cyc_Cstdio___sFILE* __stream,
int* __pos); extern void clearerr( struct Cyc_Cstdio___sFILE* __stream); extern
int feof( struct Cyc_Cstdio___sFILE* __stream); extern int ferror( struct Cyc_Cstdio___sFILE*
__stream); extern void perror( unsigned char* __s); extern int fileno( struct
Cyc_Cstdio___sFILE* __stream); struct Cyc_Std___sFILE{ struct Cyc_Cstdio___sFILE*
file; } ; int Cyc_Std_remove( struct _tagged_arr filename){ return remove(
string_to_Cstring( filename));} int Cyc_Std_rename( struct _tagged_arr
old_filename, struct _tagged_arr new_filename){ return rename( string_to_Cstring(
old_filename), string_to_Cstring( new_filename));} struct Cyc_Std___sFILE* Cyc_Std_tmpfile(){
struct Cyc_Cstdio___sFILE* cf= tmpfile(); return( unsigned int) cf?({ struct Cyc_Std___sFILE*
_temp0=( struct Cyc_Std___sFILE*) _cycalloc( sizeof( struct Cyc_Std___sFILE));
_temp0->file= cf; _temp0;}): 0;} int Cyc_Std_fclose( struct Cyc_Std___sFILE* f){
int r= fclose(( struct Cyc_Cstdio___sFILE*) _check_null( f->file)); if( r ==  0){
f->file= 0;} return r;} int Cyc_Std_fflush( struct Cyc_Std___sFILE* f){ return(
unsigned int) f? fflush( f->file): fflush( 0);} struct Cyc_Std___sFILE* Cyc_Std_freopen(
struct _tagged_arr x, struct _tagged_arr y, struct Cyc_Std___sFILE* f){ struct
Cyc_Cstdio___sFILE* cf= freopen( string_to_Cstring( x), string_to_Cstring( y),(
struct Cyc_Cstdio___sFILE*) _check_null( f->file)); return( unsigned int) cf?({
struct Cyc_Std___sFILE* _temp1=( struct Cyc_Std___sFILE*) _cycalloc( sizeof(
struct Cyc_Std___sFILE)); _temp1->file= cf; _temp1;}): 0;} void Cyc_Std_setbuf(
struct Cyc_Std___sFILE* f, struct _tagged_arr buf){ Cyc_Std_setvbuf( f, buf,(
unsigned int) buf.curr? 0: 2, 1024);} void Cyc_Std_setbuffer( struct Cyc_Std___sFILE*
f, struct _tagged_arr buf, unsigned int size){ Cyc_Std_setvbuf( f, buf,(
unsigned int) buf.curr? 0: 2, size);} void Cyc_Std_setlinebuf( struct Cyc_Std___sFILE*
f){ Cyc_Std_setvbuf( f, _tag_arr( 0u, 0u, 0u), 1, 0);} int Cyc_Std_setvbuf(
struct Cyc_Std___sFILE* f, struct _tagged_arr buf, int mode, unsigned int size){
if( _get_arr_size( buf, sizeof( unsigned char)) <  size){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp2=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp2[ 0]=({ struct Cyc_Core_Failure_struct
_temp3; _temp3.tag= Cyc_Core_Failure; _temp3.f1= _tag_arr("setvbuf: buffer insufficient",
sizeof( unsigned char), 29u); _temp3;}); _temp2;}));} return setvbuf(( struct
Cyc_Cstdio___sFILE*) _check_null( f->file), underlying_Cstring(( struct
_tagged_arr) buf), mode, size);} int Cyc_Std_fgetc( struct Cyc_Std___sFILE* f){
return fgetc(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} struct
_tagged_arr Cyc_Std_fgets( struct _tagged_arr s, int n, struct Cyc_Std___sFILE*
f){ unsigned char* result; unsigned char* buffer= underlying_Cstring(( struct
_tagged_arr) s); unsigned int len= _get_arr_size( s, sizeof( unsigned char)); n=(
int)( len <  n? len:( unsigned int) n); result= fgets( buffer, n,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file)); if( result ==  0){ return _tag_arr( 0u, 0u, 0u);} else{
return s;}} int Cyc_Std_fputc( int i, struct Cyc_Std___sFILE* f){ return fputc(
i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_fputs(
struct _tagged_arr s, struct Cyc_Std___sFILE* f){ return fputs(
string_to_Cstring( s),( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int
Cyc_Std_getc( struct Cyc_Std___sFILE* f){ return getc(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Std_putc( int i, struct Cyc_Std___sFILE* f){
return putc( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_puts(
struct _tagged_arr s){ return puts( string_to_Cstring( s));} int Cyc_Std_ungetc(
int i, struct Cyc_Std___sFILE* f){ return ungetc( i,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} unsigned int Cyc_Std_fread( struct _tagged_arr ptr,
unsigned int size, unsigned int nmemb, struct Cyc_Std___sFILE* f){ if( size * 
nmemb >  _get_arr_size( ptr, sizeof( unsigned char))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp4=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp4[ 0]=({ struct Cyc_Core_Failure_struct
_temp5; _temp5.tag= Cyc_Core_Failure; _temp5.f1= _tag_arr("fread: buffer insufficient",
sizeof( unsigned char), 27u); _temp5;}); _temp4;}));} return fread(
underlying_Cstring(( struct _tagged_arr) ptr), size, nmemb,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} unsigned int Cyc_Std_fwrite( struct _tagged_arr ptr,
unsigned int size, unsigned int nmemb, struct Cyc_Std___sFILE* f){ if( size * 
nmemb >  _get_arr_size( ptr, sizeof( unsigned char))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp6=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp6[ 0]=({ struct Cyc_Core_Failure_struct
_temp7; _temp7.tag= Cyc_Core_Failure; _temp7.f1= _tag_arr("fwrite: buffer insufficient",
sizeof( unsigned char), 28u); _temp7;}); _temp6;}));} return fwrite(
underlying_Cstring( ptr), size, nmemb,( struct Cyc_Cstdio___sFILE*) _check_null(
f->file));} int Cyc_Std_fgetpos( struct Cyc_Std___sFILE* f, int* x){ return
fgetpos(( struct Cyc_Cstdio___sFILE*) _check_null( f->file), x);} int Cyc_Std_fseek(
struct Cyc_Std___sFILE* f, int offset, int whence){ return fseek(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file), offset, whence);} int Cyc_Std_fsetpos( struct Cyc_Std___sFILE*
f, int* x){ return fsetpos(( struct Cyc_Cstdio___sFILE*) _check_null( f->file),
x);} int Cyc_Std_ftell( struct Cyc_Std___sFILE* f){ return ftell(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} void Cyc_Std_rewind( struct Cyc_Std___sFILE* f){ rewind((
struct Cyc_Cstdio___sFILE*) _check_null( f->file));} void Cyc_Std_clearerr(
struct Cyc_Std___sFILE* f){ clearerr(( struct Cyc_Cstdio___sFILE*) _check_null(
f->file));} int Cyc_Std_feof( struct Cyc_Std___sFILE* f){ return feof(( struct
Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_ferror( struct Cyc_Std___sFILE*
f){ return ferror(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} void
Cyc_Std_perror( struct _tagged_arr s){ perror( string_to_Cstring( s));} struct
Cyc_Std___sFILE* Cyc_Std_fopen( struct _tagged_arr name, struct _tagged_arr type){
struct Cyc_Cstdio___sFILE* cf= fopen( string_to_Cstring( name),
string_to_Cstring( type)); return( unsigned int) cf?({ struct Cyc_Std___sFILE*
_temp8=( struct Cyc_Std___sFILE*) _cycalloc( sizeof( struct Cyc_Std___sFILE));
_temp8->file= cf; _temp8;}): 0;} struct Cyc_Std___sFILE* Cyc_Std_fdopen( int i,
struct _tagged_arr s){ struct Cyc_Cstdio___sFILE* cf= fdopen( i,
string_to_Cstring( s)); return( unsigned int) cf?({ struct Cyc_Std___sFILE*
_temp9=( struct Cyc_Std___sFILE*) _cycalloc( sizeof( struct Cyc_Std___sFILE));
_temp9->file= cf; _temp9;}): 0;} int Cyc_Std_fileno( struct Cyc_Std___sFILE* f){
return fileno(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_getw(
struct Cyc_Std___sFILE* f){ return getw(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Std_putw( int i, struct Cyc_Std___sFILE* f){
return putw( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));}
unsigned char Cyc_Std_FileCloseError[ 19u]="\000\000\000\000FileCloseError";
unsigned char Cyc_Std_FileOpenError[ 18u]="\000\000\000\000FileOpenError";
struct Cyc_Std___sFILE* Cyc_Std_file_open( struct _tagged_arr fname, struct
_tagged_arr mode){ struct Cyc_Std___sFILE* f= Cyc_Std_fopen( fname, mode); if( f
==  0){ struct _tagged_arr fn=({ unsigned int _temp12= _get_arr_size( fname,
sizeof( unsigned char)); unsigned char* _temp13=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp12)); struct
_tagged_arr _temp15= _tag_arr( _temp13, sizeof( unsigned char), _get_arr_size(
fname, sizeof( unsigned char)));{ unsigned int _temp14= _temp12; unsigned int i;
for( i= 0; i <  _temp14; i ++){ _temp13[ i]=*(( const unsigned char*)
_check_unknown_subscript( fname, sizeof( unsigned char),( int) i));}}; _temp15;});(
int) _throw(( void*)({ struct Cyc_Std_FileOpenError_struct* _temp10=( struct Cyc_Std_FileOpenError_struct*)
_cycalloc( sizeof( struct Cyc_Std_FileOpenError_struct)); _temp10[ 0]=({ struct
Cyc_Std_FileOpenError_struct _temp11; _temp11.tag= Cyc_Std_FileOpenError;
_temp11.f1= fn; _temp11;}); _temp10;}));} return( struct Cyc_Std___sFILE*)
_check_null( f);} void Cyc_Std_file_close( struct Cyc_Std___sFILE* f){ if( Cyc_Std_fclose(
f) !=  0){( int) _throw(( void*) Cyc_Std_FileCloseError);}}
