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
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern
int Cyc_Std_remove( struct _tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*);
extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern struct Cyc_Std___sFILE*
Cyc_Std_fopen( struct _tagged_arr __filename, struct _tagged_arr __modes);
extern int Cyc_Std_fputc( int __c, struct Cyc_Std___sFILE* __stream); extern int
Cyc_Std_fputs( struct _tagged_arr __s, struct Cyc_Std___sFILE* __stream); extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Std_printf( struct _tagged_arr fmt, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Std_vfprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr ap); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_Std_fscanf( struct Cyc_Std___sFILE*
stream, struct _tagged_arr fmt, struct _tagged_arr); extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_arr lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_arr, int, void*); void* read_fun_state; } ;
struct Cyc_Lexing_lex_tables{ struct _tagged_arr lex_base; struct _tagged_arr
lex_backtrk; struct _tagged_arr lex_default; struct _tagged_arr lex_trans;
struct _tagged_arr lex_check; } ; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Std___sFILE*); extern struct _tagged_arr Cyc_Lexing_lexeme( struct
Cyc_Lexing_lexbuf*); extern unsigned char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*,
int); extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); extern int
Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Set_Set; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set*
Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct Cyc_Set_Set*
Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2); extern
struct Cyc_Set_Set* Cyc_Set_diff( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2);
extern struct Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt);
extern int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern struct Cyc_List_List* Cyc_Set_elements(
struct Cyc_Set_Set* s); extern unsigned char Cyc_Set_Absent[ 11u]; extern
unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern int Cyc_Std_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_strdup(
struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_substring( struct
_tagged_arr, int ofs, unsigned int n); struct Cyc_Hashtable_Table; extern struct
Cyc_Hashtable_Table* Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*),
int(* hash)( void*)); extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table*
t, void* key, void* val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table*
t, void* key); extern int Cyc_Hashtable_hash_stringptr( struct _tagged_arr* p);
extern struct _tagged_arr Cyc_Filename_concat( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension( struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_dirname( struct _tagged_arr);
extern struct _tagged_arr Cyc_Filename_basename( struct _tagged_arr); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
struct _tagged_arr); struct Cyc_Position_Segment; static const int Cyc_Position_Lex=
0; static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
struct Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; extern unsigned char Cyc_Array_Array_mismatch[ 19u]; struct Cyc_Std__Div{
int quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int
abs( int __x); extern int atexit( void(* __func)()); extern struct Cyc_Std__Div
div( int __numer, int __denom); extern struct Cyc_Std__Ldiv ldiv( int __numer,
int __denom); extern int random(); extern void srandom( unsigned int __seed);
extern int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern int Cyc_Std_system( struct _tagged_arr); struct Cyc_Buffer_t;
extern struct Cyc_Buffer_t* Cyc_Buffer_create( unsigned int n); extern struct
_tagged_arr Cyc_Buffer_contents( struct Cyc_Buffer_t*); extern void Cyc_Buffer_add_char(
struct Cyc_Buffer_t*, unsigned char); extern void Cyc_Buffer_add_string( struct
Cyc_Buffer_t*, struct _tagged_arr); struct Cyc_Std_tm{ int tm_sec; int tm_min;
int tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int tm_yday; int
tm_isdst; } ; extern void tzset(); extern int time( int* t); extern struct Cyc_Std_tm*
gmtime( const int* timep); extern struct Cyc_Std_tm* localtime( const int* timep);
extern int timezone; extern int daylight; struct Cyc_Std_stat_t{
unsigned long long st_dev; unsigned short __pad1; unsigned int st_ino;
unsigned int st_mode; unsigned int st_nlink; unsigned int st_uid; unsigned int
st_gid; unsigned long long st_rdev; unsigned short __pad2; int st_size; int
st_blksize; int st_blocks; int st_atime; unsigned int __unused1; int st_mtime;
unsigned int __unused2; int st_ctime; unsigned int __unused3; unsigned int
__unused4; unsigned int __unused5; } ; extern int fstat( int fd, struct Cyc_Std_stat_t*
buf); extern unsigned int umask( unsigned int mask); extern int Cyc_Std_mkdir(
struct _tagged_arr pathname, unsigned int mode); extern int fchmod( int fd,
unsigned int mode); struct Cyc_Std_flock{ short l_type; int l_start; short
l_whence; int l_len; int l_pid; } ; static const int Cyc_Std_Flock= 0; struct
Cyc_Std_Flock_struct{ int tag; struct Cyc_Std_flock* f1; } ; static const int
Cyc_Std_Long= 1; struct Cyc_Std_Long_struct{ int tag; int f1; } ; extern int Cyc_Std_open(
struct _tagged_arr, int, struct _tagged_arr); struct Cyc_Std_timeval{ int tv_sec;
int tv_usec; } ; struct Cyc_Std_timezone{ int tz_minuteswest; int tz_dsttime; }
; extern int gettimeofday( struct Cyc_Std_timeval* __p, struct Cyc_Std_timezone*
__z); struct Cyc_Std_itimerval{ struct Cyc_Std_timeval it_interval; struct Cyc_Std_timeval
it_value; } ; extern int getitimer( int, struct Cyc_Std_itimerval*); extern int
setitimer( int, const struct Cyc_Std_itimerval*, struct Cyc_Std_itimerval*);
typedef struct { unsigned int __val[ 64u]; } Cyc_Std___sigset_t; typedef Cyc_Std___sigset_t
Cyc_Std_sigset_t; struct Cyc_Std_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_Std_timeval; struct Cyc_Std___fd_set{ int __fds_bits[ 32u]; } ; extern int
select( int, struct Cyc_Std___fd_set*, struct Cyc_Std___fd_set*, struct Cyc_Std___fd_set*,
struct Cyc_Std_timeval*); struct Cyc_Std_option{ struct _tagged_arr name; int
has_arg; int* flag; int val; } ; extern unsigned int alarm( unsigned int seconds);
extern int close( int); extern void _exit( int); extern int getpid(); extern int
getppid(); extern int fork(); extern int fchdir( int); extern int fchown( int,
unsigned int, unsigned int); extern int fsync( int); extern int ftruncate( int,
int); extern int dup( int); extern int dup2( int, int); extern int setsid();
extern int getsid( int pid); extern unsigned int getuid(); extern int setuid(
unsigned int uid); extern unsigned int geteuid(); extern int seteuid(
unsigned int euid); extern unsigned int getgid(); extern int setgid(
unsigned int gid); extern unsigned int getegid(); extern int setegid(
unsigned int egid); extern int nice( int); extern int pause(); extern int pipe(
int* filedes); extern int lseek( int filedes, int offset, int whence); extern
unsigned int sleep( unsigned int); extern int isatty( int); extern int daemon(
int nochdir, int noclose); int Cyc_Std_access( struct _tagged_arr, int); int Cyc_Std_chdir(
struct _tagged_arr); int Cyc_Std_chown( struct _tagged_arr, unsigned int,
unsigned int); struct _tagged_arr Cyc_Std_getcwd( struct _tagged_arr buf,
unsigned int size); int Cyc_Std_execl( struct _tagged_arr path, struct
_tagged_arr arg0, struct _tagged_arr argv); int Cyc_Std_execlp( struct
_tagged_arr file, struct _tagged_arr arg0, struct _tagged_arr argv); int Cyc_Std_execve(
struct _tagged_arr filename, struct _tagged_arr argv, struct _tagged_arr envp);
int Cyc_Std_link( struct _tagged_arr, struct _tagged_arr); int Cyc_Std_read( int
fd, struct _tagged_arr buf, unsigned int count); int Cyc_Std_rmdir( struct
_tagged_arr); int Cyc_Std_symlink( struct _tagged_arr, struct _tagged_arr); int
Cyc_Std_truncate( struct _tagged_arr, int); int Cyc_Std_write( int fd, struct
_tagged_arr buf, unsigned int count); int Cyc_Std_unlink( struct _tagged_arr
pathname); int Cyc_Std_gethostname( struct _tagged_arr, unsigned int); int Cyc_Std_chroot(
struct _tagged_arr); struct _tagged_arr Cyc_Std_getpass( struct _tagged_arr
prompt); extern int isalnum( int); extern int isalpha( int); extern int isascii(
int); extern int iscntrl( int); extern int isdigit( int); extern int isgraph(
int); extern int islower( int); extern int isprint( int); extern int ispunct(
int); extern int isspace( int); extern int isupper( int); extern int isxdigit(
int); extern int toascii( int); extern int tolower( int); extern int toupper(
int); extern int _tolower( int); extern int _toupper( int); static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; struct _tuple0{ void* f1; struct
_tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static const int Cyc_Absyn_Static=
0; static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; static const int Cyc_Absyn_Eq_kb= 0;
struct Cyc_Absyn_Eq_kb_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Unknown_kb=
1; struct Cyc_Absyn_Unknown_kb_struct{ int tag; struct Cyc_Core_Opt* f1; } ;
static const int Cyc_Absyn_Less_kb= 2; struct Cyc_Absyn_Less_kb_struct{ int tag;
struct Cyc_Core_Opt* f1; void* f2; } ; struct Cyc_Absyn_Tvar{ struct _tagged_arr*
name; int* identity; void* kind; } ; static const int Cyc_Absyn_Unknown_b= 0;
static const int Cyc_Absyn_Upper_b= 0; struct Cyc_Absyn_Upper_b_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; int inject; } ; struct Cyc_Absyn_FnInfo{
struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct
Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs;
struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl** f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{
int tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; int f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_VarType= 1; struct Cyc_Absyn_VarType_struct{
int tag; struct Cyc_Absyn_Tvar* f1; } ; static const int Cyc_Absyn_TunionType= 2;
struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1; } ;
static const int Cyc_Absyn_TunionFieldType= 3; struct Cyc_Absyn_TunionFieldType_struct{
int tag; struct Cyc_Absyn_TunionFieldInfo f1; } ; static const int Cyc_Absyn_PointerType=
4; struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; }
; static const int Cyc_Absyn_IntType= 5; struct Cyc_Absyn_IntType_struct{ int
tag; void* f1; void* f2; } ; static const int Cyc_Absyn_FloatType= 1; static
const int Cyc_Absyn_DoubleType= 6; struct Cyc_Absyn_DoubleType_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_ArrayType= 7; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ;
static const int Cyc_Absyn_FnType= 8; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 9; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static const
int Cyc_Absyn_StructType= 10; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 19;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
20; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 21; struct Cyc_Absyn_RgnsEff_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; struct Cyc_List_List* f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Printf_ft= 0; static const int Cyc_Absyn_Scanf_ft=
1; static const int Cyc_Absyn_Regparm_att= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att= 0; static const int
Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att= 2; static const
int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att= 4; static
const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Absyn_Nocommon_att= 6; static const int Cyc_Absyn_Shared_att=
7; static const int Cyc_Absyn_Unused_att= 8; static const int Cyc_Absyn_Weak_att=
9; static const int Cyc_Absyn_Dllimport_att= 10; static const int Cyc_Absyn_Dllexport_att=
11; static const int Cyc_Absyn_No_instrument_function_att= 12; static const int
Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att= 14;
static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int Cyc_Absyn_Format_att=
3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1; int f2; int f3; } ;
static const int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Null_c= 0; static
const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static const
int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const int Cyc_Absyn_Mod=
4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq= 6; static
const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static const int
Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int Cyc_Absyn_Not=
11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand= 13;
static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_StructField=
0; struct Cyc_Absyn_StructField_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_Absyn_TupleIndex= 1; struct Cyc_Absyn_TupleIndex_struct{
int tag; unsigned int f1; } ; struct Cyc_Absyn_MallocInfo{ int is_calloc; struct
Cyc_Absyn_Exp* rgn; void** elt_type; struct Cyc_Absyn_Exp* num_elts; int
fat_result; } ; static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Var_e= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_Primop_e= 3; struct Cyc_Absyn_Primop_e_struct{ int tag; void*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e= 4;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_FnCall_e=
9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static const int Cyc_Absyn_Throw_e=
10; struct Cyc_Absyn_Throw_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_NoInstantiate_e= 11; struct Cyc_Absyn_NoInstantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Instantiate_e=
12; struct Cyc_Absyn_Instantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e=
14; struct Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e=
16; struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_Sizeofexp_e= 17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Absyn_Gentyp_e= 19; struct
Cyc_Absyn_Gentyp_e_struct{ int tag; struct Cyc_List_List* f1; void* f2; } ;
static const int Cyc_Absyn_Deref_e= 20; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 21;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_arr* f2; } ; static const int Cyc_Absyn_StructArrow_e= 22; struct
Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e= 23; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 24; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 25; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
26; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 27; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 28; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 29;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 30; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple0* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
static const int Cyc_Absyn_Malloc_e= 33; struct Cyc_Absyn_Malloc_e_struct{ int
tag; struct Cyc_Absyn_MallocInfo f1; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_ForArrayInfo{
struct Cyc_List_List* defns; struct _tuple2 condition; struct _tuple2 delta;
struct Cyc_Absyn_Stmt* body; } ; static const int Cyc_Absyn_Skip_s= 0; static
const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int
Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_SwitchC_s= 10; struct Cyc_Absyn_SwitchC_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Fallthru_s=
11; struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Absyn_Switch_clause** f2; } ; static const int Cyc_Absyn_Decl_s= 12;
struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Cut_s= 13; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Splice_s= 14;
struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Absyn_Label_s= 15; struct Cyc_Absyn_Label_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
16; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct
_tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 18; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
static const int Cyc_Absyn_ForArray_s= 19; struct Cyc_Absyn_ForArray_s_struct{
int tag; struct Cyc_Absyn_ForArrayInfo f1; } ; struct Cyc_Absyn_Stmt{ void* r;
struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds; int
try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p= 0; static const
int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p=
1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{ int tag; unsigned char
f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p= 6; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Struct_p= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p= 8; struct
Cyc_Absyn_Tunion_p_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Enum_p=
9; struct Cyc_Absyn_Enum_p_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_AnonEnum_p= 10;
struct Cyc_Absyn_AnonEnum_p_struct{ int tag; void* f1; struct Cyc_Absyn_Enumfield*
f2; } ; static const int Cyc_Absyn_UnknownId_p= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_UnknownCall_p= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
struct Cyc_Absyn_SwitchC_clause{ struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Unresolved_b=
0; static const int Cyc_Absyn_Global_b= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Funname_b= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
static const int Cyc_Absyn_Param_b= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Local_b= 3; struct
Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Pat_b= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
tvs; void* defn; } ; static const int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Fn_d= 1;
struct Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ; static
const int Cyc_Absyn_Let_d= 2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat*
f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4;
int f5; } ; static const int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Struct_d= 4;
struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
static const int Cyc_Absyn_Union_d= 5; struct Cyc_Absyn_Union_d_struct{ int tag;
struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d= 6;
struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; } ;
static const int Cyc_Absyn_Enum_d= 7; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 8;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 9; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_arr* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int generate_line_directives: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); extern struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Std___sFILE* f); struct Cyc_Declaration_spec;
struct Cyc_Declarator; struct Cyc_Abstractdeclarator; extern int Cyc_yyparse();
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{ unsigned char* tag;
int f1; } ; extern unsigned char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
unsigned char* tag; unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Declaration_spec* f1; } ; extern unsigned char
Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{ unsigned char* tag;
struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[ 23u];
struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple3* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple4{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple4* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple5{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple5* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple6{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple6* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple7{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple7* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
17u]; struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u];
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; extern
unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; extern int Cyc_yyparse();
extern unsigned char Cyc_YY1[ 8u]; struct Cyc_YY1_struct{ unsigned char* tag;
struct _tuple6* f1; } ; extern unsigned char Cyc_YYINITIALSVAL[ 18u]; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; extern void Cyc_Lex_lex_init(); struct Cyc_Std___sFILE* Cyc_log_file= 0;
struct Cyc_Std___sFILE* Cyc_cstubs_file= 0; struct Cyc_Std___sFILE* Cyc_cycstubs_file=
0; int Cyc_log( struct _tagged_arr fmt, struct _tagged_arr ap){ int _temp0= Cyc_Std_vfprintf((
struct Cyc_Std___sFILE*) _check_null( Cyc_log_file), fmt, ap); Cyc_Std_fflush((
struct Cyc_Std___sFILE*)(( struct Cyc_Std___sFILE*) _check_null( Cyc_log_file)));
return _temp0;} int Cyc_sizeof_unsignedlongint= - 1; int Cyc_sizeof_unsignedshortint=
- 1; int Cyc_sizeof_shortint= - 1; int Cyc_sizeof_int= - 1; int Cyc_sizeof_short=
- 1; int Cyc_sizeof_fdmask= - 1; int Cyc_sizeof___fdmask= - 1; int Cyc_sizeof_uint32=
- 1; int Cyc_sizeof_sockaddr= - 1; int Cyc_sizeof_inport= - 1; int Cyc_sizeof_inaddr=
- 1; static struct _tagged_arr* Cyc_current_source= 0; static struct Cyc_List_List*
Cyc_current_args= 0; static struct Cyc_Set_Set** Cyc_current_targets= 0; static
void Cyc_add_target( struct _tagged_arr* sptr){ Cyc_current_targets=({ struct
Cyc_Set_Set** _temp1=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp1[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr*
elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets)),
sptr); _temp1;});} struct _tuple8{ struct _tagged_arr* f1; struct Cyc_Set_Set*
f2; } ; extern struct _tuple8* Cyc_line( struct Cyc_Lexing_lexbuf*); extern int
Cyc_macroname( struct Cyc_Lexing_lexbuf*); extern int Cyc_args( struct Cyc_Lexing_lexbuf*);
extern int Cyc_token( struct Cyc_Lexing_lexbuf*); extern int Cyc_string( struct
Cyc_Lexing_lexbuf*); struct Cyc_Std___sFILE* Cyc_slurp_out= 0; extern int Cyc_slurp(
struct Cyc_Lexing_lexbuf*); extern int Cyc_slurp_string( struct Cyc_Lexing_lexbuf*);
extern int Cyc_asm( struct Cyc_Lexing_lexbuf*); extern int Cyc_asm_string(
struct Cyc_Lexing_lexbuf*); extern int Cyc_asm_comment( struct Cyc_Lexing_lexbuf*);
struct _tuple9{ struct _tagged_arr f1; struct _tagged_arr* f2; } ; extern struct
_tuple9* Cyc_suck_line( struct Cyc_Lexing_lexbuf*); extern int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf*); extern int Cyc_suck_restofline( struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_line=( struct _tagged_arr){( void*) 0u,( void*) 0u,(
void*)( 0u +  0u)}; struct _tuple10{ struct _tagged_arr f1; struct _tagged_arr
f2; } ; struct _tuple11{ struct _tagged_arr f1; struct Cyc_List_List* f2; struct
Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; struct
Cyc_List_List* f6; struct Cyc_List_List* f7; } ; extern struct _tuple11* Cyc_spec(
struct Cyc_Lexing_lexbuf*); extern int Cyc_commands( struct Cyc_Lexing_lexbuf*);
extern int Cyc_snarfsymbols( struct Cyc_Lexing_lexbuf*); extern int Cyc_block(
struct Cyc_Lexing_lexbuf*); extern int Cyc_block_string( struct Cyc_Lexing_lexbuf*);
extern int Cyc_block_comment( struct Cyc_Lexing_lexbuf*); struct _tagged_arr Cyc_current_header=(
struct _tagged_arr){( void*) 0u,( void*) 0u,( void*)( 0u +  0u)}; struct Cyc_List_List*
Cyc_snarfed_symbols= 0; struct Cyc_List_List* Cyc_current_symbols= 0; struct Cyc_List_List*
Cyc_current_cstubs= 0; struct Cyc_List_List* Cyc_current_cycstubs= 0; struct Cyc_List_List*
Cyc_current_prologue= 0; struct Cyc_List_List* Cyc_current_epilogue= 0; struct
Cyc_List_List* Cyc_current_omit_symbols= 0; struct Cyc_Buffer_t* Cyc_specbuf= 0;
int Cyc_braces_to_match= 0; int Cyc_parens_to_match= 0; int Cyc_numdef= 0; const
int Cyc_lex_base[ 508u]=( const int[ 508u]){ 0, 0, 75, 192, 305, 285, 310, 166,
311, 91, 27, 384, 28, 503, 617, 695, 288, 338, 93, - 3, 0, - 2, - 1, - 8, - 3, 1,
- 1, 586, - 4, 2, 166, - 5, 308, 777, 333, - 6, - 7, - 3, 16, - 2, 29, 11, 815,
- 3, 890, 13, - 15, 223, 12, - 14, 107, 22, 26, 28, 24, - 13, 28, 58, 56, 65, 52,
70, 320, 1004, 1119, 357, 93, 83, 90, 112, 96, 106, 115, 125, 119, 150, 173, 382,
410, 226, 218, 211, 231, 231, 415, 505, 229, 228, 226, 235, 223, 240, 498, 1233,
1348, 510, 226, 244, 231, 234, 236, 256, 515, 1462, 1577, - 9, 530, - 10, 238,
270, 633, 1691, 1806, 635, - 8, 391, 641, 646, 1883, 1960, 2037, 2118, 402, 416,
653, 2193, 269, 270, 277, 285, 283, 293, 0, 13, 4, 2274, 5, 638, 2282, 2347, 646,
49, 465, 6, 2308, 7, 682, 2370, 2408, 800, - 30, 318, 322, 301, 293, 277, 315,
307, 341, 671, 1035, 349, 345, 336, 405, 366, 362, 405, 414, 412, 425, 427, 710,
1030, 418, 427, 421, 420, 420, 809, 1145, 428, 429, 1006, 436, 445, 826, 1147,
453, 448, 1235, 454, 481, 480, 530, 515, 1014, 1259, 522, 541, 558, 553, 564,
568, 569, 556, 1350, - 12, 576, 577, 575, 577, 565, 1464, 568, 586, 1579, 2470,
597, 593, 1693, - 11, 622, 598, 1808, 601, 599, 598, 620, 608, 2332, 630, 617,
642, 625, 634, 2472, 721, 653, 649, 645, 640, 10, 14, 662, 670, 2480, 692, 679,
692, 676, 714, 2482, 936, 741, 707, 726, 720, 749, 759, 746, 746, 792, 802, 809,
793, - 28, 803, 843, 839, 849, 857, 858, - 24, 838, 880, 872, 868, 879, 875, 908,
925, 926, - 18, 912, 910, 910, 933, 934, - 25, 925, 923, 916, 937, 939, 956, 945,
967, 970, 958, 963, 980, 980, - 27, 988, 982, 986, 1026, 1035, 1017, 1019, 1035,
1044, 1046, 1, 4, 6, 1047, 1048, 1035, 1035, 1047, 1047, 1054, 1058, 2, 52, 1064,
1066, 2272, 20, 21, 1089, 1090, 1053, 1051, 1066, 1083, 1085, 1140, 1141, 1142,
- 23, 1115, 1116, 1171, 1172, 1174, - 20, 1152, 1154, 1210, 1212, 1213, - 21,
1160, 1161, 1216, 1217, 1218, - 19, 1165, 1166, 1221, 1222, 1223, - 22, 1171,
1175, 1469, - 29, 1174, 1176, 1166, 1167, 1166, 1179, 4, 1171, 1172, 1181, 1194,
1262, - 17, 1198, 1199, 1189, 1216, 1214, 1227, 1295, - 14, 1221, 1254, 1254,
1267, 1335, - 16, - 7, - 8, 8, 1360, 2484, 9, 1335, 2492, 2517, 1435, 1489, - 49,
1582, - 2, 1307, - 4, 1310, 1334, 2090, 1314, 1350, 1431, 1549, 1315, 2574, 2617,
1319, 1346, 1317, 1348, 2687, 1318, 1320, - 36, - 42, - 37, 2762, 1351, - 40,
1378, - 45, - 39, - 48, 2837, 2866, 1662, 1437, 1448, 1774, 2876, 2906, 2655,
2584, 2939, 2970, 3008, 1441, 1531, 3078, 3116, 1523, 1560, 1552, 1562, 1556,
1644, - 6, - 34, 1394, 3048, - 47, - 32, - 46, - 31, - 33, 1402, 3156, 1403,
1441, 1793, 1442, 1452, 1456, 1458, 1461, 1464, 1465, 1467, 1468, 3229, 3313,
2476, 1469, - 41, - 38, - 35, - 26, 1717, 3395, 1488, 3478, 1484, 15, 1453, 1454,
1494, 1505, 1502, 1512, 1583}; const int Cyc_lex_backtrk[ 508u]=( const int[ 508u]){
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 2, - 1, - 1, - 1, - 1, 2, - 1, 8, - 1, 3, 5, - 1, - 1, 6, 5, -
1, - 1, - 1, 6, - 1, 4, 1, 0, - 1, 0, 1, - 1, 11, 14, - 1, 14, 14, 14, 14, 14, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 10, 11, 2, 4, 4, - 1,
1, 1, 0, 2, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, 2, 8, 3, 5, - 1, 6, 5, 6, 4,
2, 8, 3, 5, - 1, 6, 5, - 1, 29, 29, 29, 29, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 25, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 6, 1, 9, 2, 4,
- 1, 5, 4, - 1, - 1, 2, - 1, 48, - 1, 48, 48, 48, 48, 48, 48, 48, 48, 5, 7, 48,
48, 48, 48, 0, 48, 48, - 1, - 1, - 1, 0, 43, - 1, 42, - 1, - 1, - 1, 9, 7, - 1,
7, 7, - 1, 8, 9, - 1, - 1, 9, 5, 6, 5, 5, - 1, 4, 4, 4, 6, 6, 5, 5, - 1, - 1, -
1, 9, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, - 1, - 1, 2, 1, 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_default[ 508u]=( const int[
508u]){ - 1, - 1, - 1, 409, 398, 150, 23, 36, 23, 19, - 1, - 1, 12, 31, 46, 31,
36, 23, 19, 0, - 1, 0, 0, 0, 0, - 1, 0, - 1, 0, - 1, - 1, 0, - 1, - 1, - 1, 0, 0,
0, - 1, 0, 40, - 1, - 1, 0, - 1, - 1, 0, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 0, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, 115, - 1, - 1, - 1, - 1, - 1,
122, 122, 122, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 142, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1,
- 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, -
1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, 0, 0, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, 0, - 1, 0, - 1, - 1, 473, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0, - 1, - 1, 0,
- 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1}; const int Cyc_lex_trans[ 3735u]=( const int[ 3735u]){ 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 21, 19, 28, 500, 19, 28, 19, 28, 114, 19, 45, 45, 45, 45, 45, 21, 45, 0,
0, 0, 0, 0, 22, 315, 326, 501, 46, 21, - 1, - 1, 21, - 1, - 1, 45, 316, 45, 317,
22, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 28, 115, 22, 243, 126, 40,
244, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 28, 327, 361, 355, 498,
142, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 133, 86, 20, 79, 66, 56, 57, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 58, 59, 60, 61, 496, 62, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 67, 68, 37, 410, 411, 410, 410, 411, 39, 22, 69, 70, 71, 72,
73, 141, 34, 34, 34, 34, 34, 34, 34, 34, 96, 74, 410, 412, 413, 75, 97, 414, 415,
416, 116, 116, 417, 418, 116, 419, 420, 421, 422, 423, 423, 423, 423, 423, 423,
423, 423, 423, 424, 76, 425, 426, 427, 116, 19, 428, 428, 428, 428, 428, 428,
428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428,
428, 428, 428, 428, 19, - 1, - 1, 429, 428, 77, 428, 428, 428, 428, 428, 428,
428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428,
428, 428, 428, 428, 399, 430, 151, 400, 22, 24, 24, 37, 143, 134, 80, 81, 152,
82, 63, 63, 83, 84, 63, 87, 38, 88, 89, 90, 401, 91, 92, 108, 98, 26, 26, 99, 21,
24, 21, 100, 25, 63, 101, 102, 109, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 65,
65, 110, 127, 65, 128, 26, 33, 33, 33, 33, 33, 33, 129, 153, 35, 35, 35, 35, 35,
35, 35, 35, 65, 130, 78, 78, 131, 132, 78, 253, 402, 155, 156, 154, - 1, 144,
135, - 1, 33, 33, 33, 33, 33, 33, 39, - 1, 22, 78, - 1, 157, 384, 158, 78, 78,
371, 35, 78, 85, 85, - 1, 385, 85, - 1, 27, 372, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 78, 159, 237, 231, 222, 85, 21, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, - 1, 166, 167, - 1, 125, 28, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 93, 93, 191, 168, 93, 117, 117, 85, 85, 117, 169, 85, 95, 95,
192, 170, 95, 103, 103, 171, 172, 103, 184, 93, 176, 177, 21, 178, 117, 179, 85,
182, 106, 106, 21, 95, 106, 35, 183, 185, 103, 186, 118, 119, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 105, 106, 189, 190, 214, 21, 21, 118, 118, 118,
118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 118, 21, 193, 28, 194, 118, 29, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 47, 47, 22, 195, 47, 196, 208, 35, 30, 30, 30, 30,
30, 30, 30, 30, 111, 111, 113, 113, 111, - 1, 113, 47, 116, 116, 200, 107, 116,
124, 124, 201, - 1, 124, 202, 203, 124, 124, 48, 111, 124, 113, 204, 205, 206,
209, - 1, 116, 210, 211, 49, 212, 124, 213, 159, 159, 215, 31, 159, 124, 140,
140, 140, 140, 140, 140, 140, 140, 35, 35, 35, 35, 35, 35, 35, 35, 216, 159, 41,
41, 32, 219, 41, 220, 225, 160, 226, 227, 228, 229, 50, 223, 51, 173, 173, - 1,
52, 173, 230, 232, 233, 41, 53, 54, 149, 149, 149, 149, 149, 149, 149, 149, 224,
234, 235, 236, 173, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 247, 240, 241, 242,
245, 114, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 246, 248, 249, 250, 42, 251, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 180, 180, 43, 252, 180, 238, 290, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
187, 187, 284, 239, 187, 274, 180, 33, 33, 33, 33, 33, 33, 35, 35, 35, 35, 35,
35, 35, 35, 304, 305, 187, 267, 260, 261, 262, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 55, 33, 33, 33, 33, 33, 33, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 263, 264, 265, 266,
44, 268, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 269, 270,
271, 28, 272, 273, 275, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 276, 277, 278, 279, 44, 280, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 63, 63, 183, 183, 63, 281, 183, 282, 283, 285, 197, 197, 286,
287, 197, 288, 289, 291, 292, 293, 254, 255, 256, 63, 257, 183, 173, 173, 258,
294, 173, 160, 160, 197, 107, 160, 295, 259, 296, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 173, 297, 298, 299, 300, 160, 301, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 302, 303,
367, 306, 64, 307, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 19, 65, 65, 161, 308, 65, 309, 310,
311, 312, 162, 174, 313, 163, 314, 318, 319, 320, 175, 321, 322, 323, 324, 164,
65, 165, 325, 180, 180, 187, 187, 180, 328, 187, 329, 349, 343, 335, 336, 337,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 180, 338, 187, 339, 340, 341, 342, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 344, 345, 346, 347, 64, 348, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 93, 93,
190, 190, 93, 350, 190, 351, 181, 352, 188, 353, 354, 356, 357, 358, 359, 360,
362, 363, 364, 365, 366, 93, 368, 190, 197, 197, 369, 378, 197, 373, 374, 375,
105, 376, 377, 379, 380, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 197, 381, 382,
383, 392, 386, 387, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 388, 389, 390, 391, 94, 393, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 31, 95, 95, 206, 206, 95, 394, 206, 198, 395, 396, 397, 494, 408,
408, 493, 491, 408, 199, 472, 465, 440, 289, 433, 95, 431, 206, 407, 407, 407,
407, 407, 407, 407, 407, 207, 408, 370, 22, 492, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 437, 303, 438, 266, 435, 471, 436, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 439, 468,
273, 348, 94, 432, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 103, 103, 213, 213, 103, 150, 213,
369, 369, 360, 354, 369, 31, 31, 31, 31, 31, 31, 31, 31, 283, 469, 470, 103, 221,
213, 207, 408, 408, 55, 369, 408, 391, 46, 55, 397, 383, 366, 370, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 22, 408, 114, 22, 114, 22, 35, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 502, 114, 503, 114, 104, 35, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 105, 106, 106, 217, 217, 106, 495, 217, 495,
495, 466, 504, 467, 467, 467, 467, 467, 467, 467, 467, 467, 467, 35, 31, 106,
505, 217, 506, 507, 495, 22, 0, 0, 0, 0, 114, 0, 0, 0, 0, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 0, 31, 36, 36, 35, 31, 464, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 31, 36, 36, 0, 104, 464, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 111, 111, 220, 220, 111, 446, 220, 446, 0, 0, 447, 447,
447, 447, 447, 447, 447, 447, 447, 447, 464, 0, 0, 111, 0, 220, 495, 0, 495, 495,
0, 0, 0, 0, 221, 0, 0, 0, 0, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
495, 0, 0, 464, 0, 0, 0, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 0, 0,
0, 0, 112, 0, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 36, 113, 113,
224, 224, 113, 0, 224, 447, 447, 447, 447, 447, 447, 447, 447, 447, 447, 366, 0,
0, 0, 0, 0, 113, 0, 224, 489, 489, 489, 489, 489, 489, 489, 489, 36, 0, 0, 0, 0,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 0, 0, 0, 0, 0, 0, 0, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 0, 0, 0, 0, 112, 0, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 22, 0, 0, 0, 0, 0, 0, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
0, 0, 0, 0, 120, 0, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 121,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22, 0, 0, 0, 0, 0, 0, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 0, 0, 0, 0, 120, 0, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 22, 0, 0, 0, 0, - 1, 0, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, - 1, - 1, 0, - 1, 120, 0, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
0, 0, 0, 0, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 123, 0,
0, 0, 0, 0, 474, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 0, 0, 0, 0, 121,
0, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 28, 0, 0, 136, 125, 0, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 0, 0, 28, 0, 0, 145, 137, 137, 137, 137, 137, 137, 137, 137, 139, 139,
139, 139, 139, 139, 139, 139, 139, 139, 330, 230, 230, 0, 331, 230, - 1, 139,
139, 139, 139, 139, 139, 332, 0, 333, 146, 146, 146, 146, 146, 146, 146, 146,
230, 0, 0, 0, 0, 0, 0, 31, 0, 35, - 1, 0, 0, 0, 0, 139, 139, 139, 139, 139, 139,
0, 0, 0, 0, 0, 0, 334, 0, 0, 138, 139, 139, 139, 139, 139, 139, 139, 139, 139,
139, 31, 0, 0, 0, 0, 0, 0, 139, 139, 139, 139, 139, 139, 148, 148, 148, 148, 148,
148, 148, 148, 148, 148, 147, 0, 0, 0, 0, 0, 0, 148, 148, 148, 148, 148, 148, 0,
0, 0, 139, 139, 139, 139, 139, 139, 0, 0, 0, 0, 0, 0, 148, 148, 148, 148, 148,
148, 148, 148, 148, 148, 0, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148,
148, 148, 217, 217, 236, 236, 217, 0, 236, 0, 0, 0, 246, 246, 252, 252, 246, 19,
252, 0, 403, 0, 0, 0, 0, 217, 0, 236, 148, 148, 148, 148, 148, 148, 114, 246, 19,
252, 366, 0, 0, 0, 0, 0, 31, 0, 28, 490, 490, 490, 490, 490, 490, 490, 490, 404,
404, 404, 404, 404, 404, 404, 404, 406, 406, 406, 406, 406, 406, 406, 406, 406,
406, 0, 0, 0, 0, 0, 0, 0, 406, 406, 406, 406, 406, 406, 0, 0, 406, 406, 406, 406,
406, 406, 406, 406, 406, 406, 218, 0, 0, 0, 0, 0, 28, 406, 406, 406, 406, 406,
406, 0, 406, 406, 406, 406, 406, 406, 0, 0, 0, 0, 0, 0, 0, 0, 0, 405, 0, 0, 0, 0,
0, 0, 0, 0, 0, 406, 406, 406, 406, 406, 406, 441, 0, 452, 452, 452, 452, 452,
452, 452, 452, 453, 453, 451, 451, 451, 451, 451, 451, 451, 451, 451, 451, 0,
443, 0, 0, 0, 0, 0, 0, 454, 0, 0, 0, 0, 0, 0, 0, 0, 455, 0, 0, 456, 441, 0, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 443, 0, 0, 0, 0, 0, 0, 454, 0, 0, 0,
443, 0, 0, 0, 0, 455, 0, 444, 456, 0, 0, 0, 450, 0, 450, 0, 445, 451, 451, 451,
451, 451, 451, 451, 451, 451, 451, 0, 0, 0, 0, 0, 443, 0, 0, 0, 0, 0, 0, 444, 0,
0, 0, 0, 0, 0, 0, 0, 445, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 0, 0,
0, 0, 0, 0, 0, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 0, 0, 0, 0, 434,
0, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 434, 0, 0, 0, 0, 0, 0, 0, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 0, 0, 0, 0, 434, 0, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 448, 448, 448, 448, 448, 448, 448, 448, 448, 448, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 449, 107, 0, 0, 0, 0, 441, 107, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 447, 447, 447, 447, 447, 447, 447, 447, 447, 447, 0, 443, 0, 0, 449,
107, 0, 0, 444, 0, 0, 107, 105, 0, 0, 0, 0, 445, 105, 0, 448, 448, 448, 448, 448,
448, 448, 448, 448, 448, 0, 0, 0, 443, 0, 0, 0, 0, 0, 0, 444, 449, 107, 0, 105,
0, 0, 0, 107, 445, 105, 0, 0, 451, 451, 451, 451, 451, 451, 451, 451, 451, 451,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 449, 107, 107, 0, 0, 0, 0, 107, 107, 441, 0, 452,
452, 452, 452, 452, 452, 452, 452, 453, 453, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
443, 0, 107, 0, 0, 0, 0, 462, 107, 0, 0, 0, 0, 0, 0, 441, 463, 453, 453, 453,
453, 453, 453, 453, 453, 453, 453, 0, 0, 0, 0, 0, 443, 0, 0, 0, 0, 0, 443, 462,
0, 0, 0, 0, 0, 460, 0, 0, 463, 0, 0, 0, 0, 0, 461, 0, 0, 467, 467, 467, 467, 467,
467, 467, 467, 467, 467, 0, 0, 0, 443, 0, 0, 0, 0, 0, 0, 460, 449, 107, 0, 0, 0,
0, 0, 107, 461, 457, 457, 457, 457, 457, 457, 457, 457, 457, 457, 0, 0, 0, 0, 0,
0, 0, 457, 457, 457, 457, 457, 457, 449, 107, 0, 0, 0, 0, 0, 107, 0, 0, 0, 0, 0,
0, 0, 457, 457, 457, 457, 457, 457, 457, 457, 457, 457, 0, 457, 457, 457, 457,
457, 457, 457, 457, 457, 457, 457, 457, 0, 0, 0, 475, 0, 458, 0, 0, 476, 0, 0, 0,
0, 0, 459, 0, 0, 477, 477, 477, 477, 477, 477, 477, 477, 0, 457, 457, 457, 457,
457, 457, 478, 0, 0, 0, 0, 458, 0, 0, 0, 0, 0, 0, 0, 0, 459, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 479, 0, 0, 0, 0, 480, 481, 0, 0, 0, 482, 0, 0, 0, 0, 0, 0,
0, 483, 0, 0, 0, 484, 0, 485, 0, 486, 0, 487, 488, 488, 488, 488, 488, 488, 488,
488, 488, 488, 0, 0, 0, 0, 0, 0, 0, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, 0, 0, 0, 0, 0, 0, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 342,
0, 0, 0, 0, 0, 0, 0, 0, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 0, 0,
0, 0, 0, 0, 0, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 0, 0, 0, 0, 0,
0, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 21, 0, 0, 497, 0, 0, 0,
496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 0, 0, 0, 0, 0, 0, 0, 496, 496,
496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 496, 496, 496, 0, 0, 0, 0, 496, 0, 496, 496, 496, 496,
496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 496, 499, 0, 0, 0, 0, 0, 0, 0, 498, 498, 498, 498, 498,
498, 498, 498, 498, 498, 0, 0, 0, 0, 0, 0, 0, 498, 498, 498, 498, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498,
498, 498, 498, 0, 0, 0, 0, 498, 0, 498, 498, 498, 498, 498, 498, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498,
498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0}; const int Cyc_lex_check[ 3735u]=( const int[ 3735u]){ - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 0, 25, 29, 0, 134, 136, 143, 145, 400, 403, 41, 41,
45, 45, 41, 500, 45, - 1, - 1, - 1, - 1, - 1, 132, 314, 325, 0, 377, 10, 12, 40,
10, 12, 40, 41, 315, 45, 316, 20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 38, 48, 133, 242,
10, 38, 243, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 141, 326, 330, 331, 1, 141, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 51, 18, 52, 53,
54, 56, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 57, 58, 59, 60, 2, 61, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 66, 67, 7, 3, 3, 3, 3, 3, 7, 7, 68, 69, 70, 71, 72, 7,
30, 30, 30, 30, 30, 30, 30, 30, 50, 73, 3, 3, 3, 74, 50, 3, 3, 3, 47, 47, 3, 3,
47, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 75, 3, 3, 3, 47, 0, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 12, 40, 3, 3,
76, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
4, 3, 5, 4, 5, 6, 8, 16, 6, 8, 79, 80, 5, 81, 62, 62, 82, 83, 62, 86, 16, 87, 88,
89, 4, 90, 91, 96, 97, 6, 8, 98, 9, 17, 18, 99, 17, 62, 100, 101, 108, 32, 32,
32, 32, 32, 32, 32, 32, 32, 32, 65, 65, 109, 126, 65, 127, 17, 32, 32, 32, 32,
32, 32, 128, 5, 34, 34, 34, 34, 34, 34, 34, 34, 65, 129, 77, 77, 130, 131, 77,
153, 4, 154, 155, 5, 115, 6, 8, 115, 32, 32, 32, 32, 32, 32, 16, 122, 16, 77,
122, 156, 151, 157, 78, 78, 152, 7, 78, 84, 84, 123, 151, 84, 123, 17, 152, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 78, 158, 161, 162, 163, 84, 3, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 142, 165, 166, 142, 11, 65, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 92, 92, 164, 167,
92, 13, 13, 85, 85, 13, 168, 85, 95, 95, 164, 169, 95, 102, 102, 170, 171, 102,
174, 92, 175, 176, 78, 177, 13, 178, 85, 181, 106, 106, 5, 95, 106, 16, 182, 184,
102, 185, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 4, 106, 188, 189, 191,
6, 8, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 17, 192, 27, 193, 13, 27, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14,
14, 85, 194, 14, 195, 198, 95, 27, 27, 27, 27, 27, 27, 27, 27, 110, 110, 113,
113, 110, 115, 113, 14, 116, 116, 199, 106, 116, 117, 117, 200, 122, 117, 201,
202, 124, 124, 14, 110, 124, 113, 203, 204, 205, 208, 123, 116, 209, 210, 14,
211, 117, 212, 159, 159, 214, 27, 159, 124, 137, 137, 137, 137, 137, 137, 137,
137, 140, 140, 140, 140, 140, 140, 140, 140, 215, 159, 15, 15, 27, 218, 15, 219,
223, 159, 225, 226, 227, 228, 14, 222, 14, 172, 172, 142, 14, 172, 229, 231, 232,
15, 14, 14, 146, 146, 146, 146, 146, 146, 146, 146, 222, 233, 234, 235, 172, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 238, 239, 240, 241, 244, 113, 13, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 245, 247, 248, 249, 15, 250, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 179, 179, 15,
251, 179, 237, 255, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 186, 186, 256, 237,
186, 257, 179, 33, 33, 33, 33, 33, 33, 149, 149, 149, 149, 149, 149, 149, 149,
254, 254, 186, 258, 259, 260, 261, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 14,
33, 33, 33, 33, 33, 33, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 262, 263, 264, 265, 42, 267, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 268, 269, 270, 15,
271, 272, 274, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 275, 276, 277, 278, 44, 279, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 63, 63, 183, 183, 63, 280, 183, 281, 282, 284, 196, 196, 285, 286,
196, 287, 288, 290, 291, 292, 253, 253, 253, 63, 253, 183, 173, 173, 253, 293,
173, 160, 160, 196, 183, 160, 294, 253, 295, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 173, 296, 297, 298, 299, 160, 300, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 301, 302, 304,
305, 63, 306, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64, 160, 307, 64, 308, 309, 310, 311,
160, 173, 312, 160, 313, 317, 318, 319, 173, 320, 321, 322, 323, 160, 64, 160,
324, 180, 180, 187, 187, 180, 327, 187, 328, 332, 333, 334, 335, 336, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 180, 337, 187, 338, 339, 340, 341, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 343, 344, 345, 346, 64, 347, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 93, 93, 190, 190, 93,
349, 190, 350, 180, 351, 187, 352, 353, 355, 356, 357, 358, 359, 361, 362, 363,
364, 365, 93, 367, 190, 197, 197, 368, 371, 197, 372, 373, 374, 190, 375, 376,
378, 379, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 197, 380, 381, 382, 384, 385,
386, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 387, 388, 389, 390, 93, 392, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 94, 94, 206, 206, 94, 393, 206, 197, 394, 395, 396, 412, 401, 401, 414, 415,
401, 197, 417, 421, 424, 426, 429, 94, 430, 206, 404, 404, 404, 404, 404, 404,
404, 404, 206, 401, 418, 401, 415, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 425,
425, 425, 427, 427, 418, 435, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 437, 466, 473, 475, 94, 430,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 103, 103, 213, 213, 103, 419, 213, 369, 369, 476, 478,
369, 407, 407, 407, 407, 407, 407, 407, 407, 479, 419, 419, 103, 480, 213, 481,
408, 408, 482, 369, 408, 483, 484, 213, 485, 486, 490, 369, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 497, 408, 444, 408, 445, 499, 454, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 501, 444, 502, 445, 103, 454, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 104, 104, 216, 216, 104, 410, 216, 410,
410, 420, 503, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 455, 458, 104,
504, 216, 505, 506, 410, 507, - 1, - 1, - 1, - 1, 216, - 1, - 1, - 1, - 1, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, - 1, 459, 460, 461, 455, 458, 462,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 459, 460, 461, - 1, 104, 462,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 111, 111, 220, 220, 111, 443,
220, 443, - 1, - 1, 443, 443, 443, 443, 443, 443, 443, 443, 443, 443, 463, - 1,
- 1, 111, - 1, 220, 495, - 1, 495, 495, - 1, - 1, - 1, - 1, 220, - 1, - 1, - 1,
- 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 495, - 1, - 1, 463, - 1,
- 1, - 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, - 1, - 1, - 1, - 1,
111, - 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 112, 112, 224,
224, 112, - 1, 224, 446, 446, 446, 446, 446, 446, 446, 446, 446, 446, 477, - 1,
- 1, - 1, - 1, - 1, 112, - 1, 224, 477, 477, 477, 477, 477, 477, 477, 477, 224,
- 1, - 1, - 1, - 1, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, - 1,
- 1, - 1, - 1, 112, - 1, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 118,
118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, - 1, - 1, - 1, - 1,
- 1, - 1, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, - 1, - 1, - 1, - 1,
118, - 1, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, - 1, - 1, - 1, - 1, - 1, - 1, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, - 1, - 1, - 1, - 1, 119, - 1, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, - 1, - 1, - 1, - 1, 416, - 1, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 121, 416, - 1, 121, 120, - 1, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, - 1, - 1, - 1, - 1, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, - 1, - 1, - 1, - 1, - 1, 416, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, - 1, - 1, - 1, - 1, 121, - 1, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 135, - 1, - 1, 135, 125, - 1, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, - 1, - 1, 144, - 1, - 1, 144, 135, 135, 135, 135, 135, 135, 135, 135,
138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 329, 230, 230, - 1, 329, 230,
416, 138, 138, 138, 138, 138, 138, 329, - 1, 329, 144, 144, 144, 144, 144, 144,
144, 144, 230, - 1, - 1, - 1, - 1, - 1, - 1, 135, - 1, 230, 121, - 1, - 1, - 1,
- 1, 138, 138, 138, 138, 138, 138, - 1, - 1, - 1, - 1, - 1, - 1, 329, - 1, - 1,
135, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 144, - 1, - 1, - 1, - 1,
- 1, - 1, 139, 139, 139, 139, 139, 139, 147, 147, 147, 147, 147, 147, 147, 147,
147, 147, 144, - 1, - 1, - 1, - 1, - 1, - 1, 147, 147, 147, 147, 147, 147, - 1,
- 1, - 1, 139, 139, 139, 139, 139, 139, - 1, - 1, - 1, - 1, - 1, - 1, 148, 148,
148, 148, 148, 148, 148, 148, 148, 148, - 1, 147, 147, 147, 147, 147, 147, 148,
148, 148, 148, 148, 148, 217, 217, 236, 236, 217, - 1, 236, - 1, - 1, - 1, 246,
246, 252, 252, 246, 402, 252, - 1, 402, - 1, - 1, - 1, - 1, 217, - 1, 236, 148,
148, 148, 148, 148, 148, 217, 246, 236, 252, 489, - 1, - 1, - 1, - 1, - 1, 246,
- 1, 252, 489, 489, 489, 489, 489, 489, 489, 489, 402, 402, 402, 402, 402, 402,
402, 402, 405, 405, 405, 405, 405, 405, 405, 405, 405, 405, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 405, 405, 405, 405, 405, 405, - 1, - 1, 406, 406, 406, 406, 406,
406, 406, 406, 406, 406, 217, - 1, - 1, - 1, - 1, - 1, 402, 406, 406, 406, 406,
406, 406, - 1, 405, 405, 405, 405, 405, 405, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 402, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 406, 406, 406, 406,
406, 406, 422, - 1, 422, 422, 422, 422, 422, 422, 422, 422, 422, 422, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, - 1, 422, - 1, - 1, - 1, - 1, - 1, - 1,
422, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 422, - 1, - 1, 422, 423, - 1, 423,
423, 423, 423, 423, 423, 423, 423, 423, 423, 422, - 1, - 1, - 1, - 1, - 1, - 1,
422, - 1, - 1, - 1, 423, - 1, - 1, - 1, - 1, 422, - 1, 423, 422, - 1, - 1, - 1,
449, - 1, 449, - 1, 423, 449, 449, 449, 449, 449, 449, 449, 449, 449, 449, - 1,
- 1, - 1, - 1, - 1, 423, - 1, - 1, - 1, - 1, - 1, - 1, 423, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 423, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428,
428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428,
- 1, - 1, - 1, - 1, 428, - 1, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428,
428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, - 1, - 1, - 1, - 1, 434,
- 1, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
441, 441, - 1, - 1, - 1, - 1, 442, 441, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 447, 447, 447, 447, 447, 447, 447, 447, 447, 447, - 1, 442, - 1, - 1,
441, 441, - 1, - 1, 442, - 1, - 1, 441, 447, - 1, - 1, - 1, - 1, 442, 447, - 1,
448, 448, 448, 448, 448, 448, 448, 448, 448, 448, - 1, - 1, - 1, 442, - 1, - 1,
- 1, - 1, - 1, - 1, 442, 448, 448, - 1, 447, - 1, - 1, - 1, 448, 442, 447, - 1,
- 1, 451, 451, 451, 451, 451, 451, 451, 451, 451, 451, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 448, 448, 451, - 1, - 1, - 1, - 1, 448, 451, 452, - 1,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 452, - 1, 451, - 1, - 1, - 1, - 1, 452, 451, - 1, - 1,
- 1, - 1, - 1, - 1, 453, 452, 453, 453, 453, 453, 453, 453, 453, 453, 453, 453,
- 1, - 1, - 1, - 1, - 1, 452, - 1, - 1, - 1, - 1, - 1, 453, 452, - 1, - 1, - 1,
- 1, - 1, 453, - 1, - 1, 452, - 1, - 1, - 1, - 1, - 1, 453, - 1, - 1, 467, 467,
467, 467, 467, 467, 467, 467, 467, 467, - 1, - 1, - 1, 453, - 1, - 1, - 1, - 1,
- 1, - 1, 453, 467, 467, - 1, - 1, - 1, - 1, - 1, 467, 453, 456, 456, 456, 456,
456, 456, 456, 456, 456, 456, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 456, 456, 456,
456, 456, 456, 467, 467, - 1, - 1, - 1, - 1, - 1, 467, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 457, 457, 457, 457, 457, 457, 457, 457, 457, 457, - 1, 456, 456, 456,
456, 456, 456, 457, 457, 457, 457, 457, 457, - 1, - 1, - 1, 474, - 1, 457, - 1,
- 1, 474, - 1, - 1, - 1, - 1, - 1, 457, - 1, - 1, 474, 474, 474, 474, 474, 474,
474, 474, - 1, 457, 457, 457, 457, 457, 457, 474, - 1, - 1, - 1, - 1, 457, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 457, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 474, - 1, - 1, - 1, - 1, 474, 474, - 1, - 1, - 1,
474, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 474, - 1, - 1, - 1, 474, - 1, 474, - 1,
474, - 1, 474, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487,
487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, - 1, - 1,
- 1, - 1, - 1, - 1, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487,
487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 487, 488, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, - 1, - 1, - 1, - 1, - 1, - 1, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488, 488,
488, 488, 496, - 1, - 1, 496, - 1, - 1, - 1, 496, 496, 496, 496, 496, 496, 496,
496, 496, 496, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, - 1, - 1, - 1, - 1, 496, - 1, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496,
496, 496, 496, 496, 498, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 498, 498, 498, 498,
498, 498, 498, 498, 498, 498, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, - 1, - 1, - 1, - 1, 498, - 1, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498, 498,
498, 498, 498, 498, 498, 498, 498, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int Cyc_lex_engine(
int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int state; int base; int
backtrk; int c; state= start_state; if( state >=  0){ lbuf->lex_last_pos=( lbuf->lex_start_pos=
lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{ state=( - state) -  1;}
while( 1) { base= Cyc_lex_base[ _check_known_subscript_notnull( 508u, state)];
if( base <  0){ return( - base) -  1;} backtrk= Cyc_lex_backtrk[
_check_known_subscript_notnull( 508u, state)]; if( backtrk >=  0){ lbuf->lex_last_pos=
lbuf->lex_curr_pos; lbuf->lex_last_action= backtrk;} if( lbuf->lex_curr_pos >= 
lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){ return( - state) -  1;}
else{ c= 256;}} else{ c=( int)*(( unsigned char*) _check_unknown_subscript( lbuf->lex_buffer,
sizeof( unsigned char), lbuf->lex_curr_pos ++)); if( c ==  - 1){ c= 256;}} if(
Cyc_lex_check[ _check_known_subscript_notnull( 3735u, base +  c)] ==  state){
state= Cyc_lex_trans[ _check_known_subscript_notnull( 3735u, base +  c)];} else{
state= Cyc_lex_default[ _check_known_subscript_notnull( 508u, state)];} if(
state <  0){ lbuf->lex_curr_pos= lbuf->lex_last_pos; if( lbuf->lex_last_action
==  - 1){( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp2=(
struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp2[ 0]=({ struct Cyc_Lexing_Error_struct _temp3; _temp3.tag= Cyc_Lexing_Error;
_temp3.f1= _tag_arr("empty token", sizeof( unsigned char), 12u); _temp3;});
_temp2;}));} else{ return lbuf->lex_last_action;}} else{ if( c ==  256){ lbuf->lex_eof_reached=
0;}}}} struct _tuple8* Cyc_line_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case
0: _LL4: Cyc_macroname( lexbuf); for( 0; Cyc_current_args !=  0; Cyc_current_args=((
struct Cyc_List_List*) _check_null( Cyc_current_args))->tl){ Cyc_current_targets=({
struct Cyc_Set_Set** _temp6=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp6[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_delete)(*(( struct Cyc_Set_Set**) _check_null(
Cyc_current_targets)),( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( Cyc_current_args))->hd); _temp6;});} return({ struct _tuple8*
_temp7=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp7->f1=(
struct _tagged_arr*) _check_null( Cyc_current_source); _temp7->f2=*(( struct Cyc_Set_Set**)
_check_null( Cyc_current_targets)); _temp7;}); case 1: _LL5: return Cyc_line(
lexbuf); case 2: _LL8: return 0; default: _LL9:( lexbuf->refill_buff)( lexbuf);
return Cyc_line_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp11=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp11[ 0]=({ struct Cyc_Lexing_Error_struct _temp12; _temp12.tag= Cyc_Lexing_Error;
_temp12.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp12;}); _temp11;}));} struct _tuple8* Cyc_line( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_line_rec( lexbuf, 0);} int Cyc_macroname_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL13: Cyc_current_source=({ struct _tagged_arr* _temp15=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp15[ 0]=(
struct _tagged_arr) Cyc_Std_substring(( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end( lexbuf) -  Cyc_Lexing_lexeme_start(
lexbuf)) -  2)); _temp15;}); Cyc_current_args= 0; Cyc_current_targets=({ struct
Cyc_Set_Set** _temp16=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp16[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); _temp16;}); Cyc_token(
lexbuf); return 0; case 1: _LL14: Cyc_current_source=({ struct _tagged_arr*
_temp18=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp18[
0]=( struct _tagged_arr) Cyc_Std_substring(( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end( lexbuf) -  Cyc_Lexing_lexeme_start(
lexbuf)) -  1)); _temp18;}); Cyc_current_args= 0; Cyc_current_targets=({ struct
Cyc_Set_Set** _temp19=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp19[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); _temp19;}); Cyc_args( lexbuf);
return 0; case 2: _LL17: Cyc_current_source=({ struct _tagged_arr* _temp21=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp21[ 0]=(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf); _temp21;}); Cyc_current_args= 0;
Cyc_current_targets=({ struct Cyc_Set_Set** _temp22=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp22[ 0]=(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_temp22;}); Cyc_token( lexbuf); return 0; default: _LL20:( lexbuf->refill_buff)(
lexbuf); return Cyc_macroname_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp24=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp24[ 0]=({ struct Cyc_Lexing_Error_struct
_temp25; _temp25.tag= Cyc_Lexing_Error; _temp25.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp25;}); _temp24;}));} int Cyc_macroname(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_macroname_rec( lexbuf, 1);} int
Cyc_args_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL26: { struct _tagged_arr*
_temp28=({ struct _tagged_arr* _temp30=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp30[ 0]=( struct _tagged_arr) Cyc_Std_substring((
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end(
lexbuf) -  Cyc_Lexing_lexeme_start( lexbuf)) -  2)); _temp30;}); Cyc_current_args=({
struct Cyc_List_List* _temp29=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp29->hd=( void*) _temp28; _temp29->tl= Cyc_current_args;
_temp29;}); return Cyc_args( lexbuf);} case 1: _LL27: { struct _tagged_arr*
_temp32=({ struct _tagged_arr* _temp34=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp34[ 0]=( struct _tagged_arr) Cyc_Std_substring((
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end(
lexbuf) -  Cyc_Lexing_lexeme_start( lexbuf)) -  1)); _temp34;}); Cyc_current_args=({
struct Cyc_List_List* _temp33=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp33->hd=( void*) _temp32; _temp33->tl= Cyc_current_args;
_temp33;}); return Cyc_token( lexbuf);} default: _LL31:( lexbuf->refill_buff)(
lexbuf); return Cyc_args_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp36=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp36[ 0]=({ struct Cyc_Lexing_Error_struct
_temp37; _temp37.tag= Cyc_Lexing_Error; _temp37.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp37;}); _temp36;}));} int Cyc_args( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_args_rec( lexbuf, 2);} int Cyc_token_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL38: Cyc_add_target(({ struct _tagged_arr* _temp40=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp40[ 0]=( struct
_tagged_arr) Cyc_Lexing_lexeme( lexbuf); _temp40;})); return Cyc_token( lexbuf);
case 1: _LL39: return 0; case 2: _LL41: return Cyc_token( lexbuf); case 3: _LL42:
Cyc_string( lexbuf); return Cyc_token( lexbuf); case 4: _LL43: return Cyc_token(
lexbuf); case 5: _LL44: return Cyc_token( lexbuf); case 6: _LL45: return Cyc_token(
lexbuf); case 7: _LL46: return Cyc_token( lexbuf); case 8: _LL47: return Cyc_token(
lexbuf); case 9: _LL48: return Cyc_token( lexbuf); case 10: _LL49: return Cyc_token(
lexbuf); case 11: _LL50: return Cyc_token( lexbuf); case 12: _LL51: return Cyc_token(
lexbuf); case 13: _LL52: return Cyc_token( lexbuf); case 14: _LL53: return Cyc_token(
lexbuf); case 15: _LL54: return Cyc_token( lexbuf); case 16: _LL55: return Cyc_token(
lexbuf); case 17: _LL56: return Cyc_token( lexbuf); case 18: _LL57: return Cyc_token(
lexbuf); case 19: _LL58: return Cyc_token( lexbuf); case 20: _LL59: return Cyc_token(
lexbuf); case 21: _LL60: return Cyc_token( lexbuf); case 22: _LL61: return Cyc_token(
lexbuf); case 23: _LL62: return Cyc_token( lexbuf); case 24: _LL63: return Cyc_token(
lexbuf); case 25: _LL64: return Cyc_token( lexbuf); case 26: _LL65: return Cyc_token(
lexbuf); case 27: _LL66: return Cyc_token( lexbuf); case 28: _LL67: return Cyc_token(
lexbuf); case 29: _LL68: return Cyc_token( lexbuf); case 30: _LL69: return Cyc_token(
lexbuf); case 31: _LL70: return Cyc_token( lexbuf); case 32: _LL71: return Cyc_token(
lexbuf); case 33: _LL72: return Cyc_token( lexbuf); case 34: _LL73: return Cyc_token(
lexbuf); case 35: _LL74: return Cyc_token( lexbuf); case 36: _LL75: return Cyc_token(
lexbuf); case 37: _LL76: return Cyc_token( lexbuf); case 38: _LL77: return Cyc_token(
lexbuf); case 39: _LL78: return Cyc_token( lexbuf); case 40: _LL79: return Cyc_token(
lexbuf); case 41: _LL80: return Cyc_token( lexbuf); case 42: _LL81: return Cyc_token(
lexbuf); case 43: _LL82: return Cyc_token( lexbuf); case 44: _LL83: return Cyc_token(
lexbuf); case 45: _LL84: return Cyc_token( lexbuf); case 46: _LL85: return Cyc_token(
lexbuf); case 47: _LL86: return Cyc_token( lexbuf); case 48: _LL87: return Cyc_token(
lexbuf); default: _LL88:( lexbuf->refill_buff)( lexbuf); return Cyc_token_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp90=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp90[ 0]=({ struct Cyc_Lexing_Error_struct _temp91; _temp91.tag= Cyc_Lexing_Error;
_temp91.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp91;}); _temp90;}));} int Cyc_token( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_token_rec( lexbuf, 3);} int Cyc_string_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL92: return Cyc_string( lexbuf); case 1: _LL93: return 0;
case 2: _LL94: return Cyc_string( lexbuf); case 3: _LL95: return Cyc_string(
lexbuf); case 4: _LL96: return Cyc_string( lexbuf); case 5: _LL97: return Cyc_string(
lexbuf); case 6: _LL98: return Cyc_string( lexbuf); case 7: _LL99: return 0;
case 8: _LL100: return 0; case 9: _LL101: return Cyc_string( lexbuf); default:
_LL102:( lexbuf->refill_buff)( lexbuf); return Cyc_string_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp104=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp104[ 0]=({ struct Cyc_Lexing_Error_struct
_temp105; _temp105.tag= Cyc_Lexing_Error; _temp105.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp105;}); _temp104;}));} int Cyc_string( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_string_rec( lexbuf, 4);} int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL106: Cyc_Std_fputc(( int)'"',(
struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); if( Cyc_slurp_string(
lexbuf)){ return 1;} else{ return Cyc_slurp( lexbuf);} case 1: _LL107: return 0;
case 2: _LL108:({ struct Cyc_Std_Int_pa_struct _temp111; _temp111.tag= Cyc_Std_Int_pa;
_temp111.f1=( int)(( unsigned int) Cyc_sizeof_fdmask);{ void* _temp110[ 1u]={&
_temp111}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp110, sizeof( void*),
1u));}});({ void* _temp112[ 0u]={}; Cyc_log( _tag_arr("Warning: sizeof(fd_mask) inlined\n",
sizeof( unsigned char), 34u), _tag_arr( _temp112, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 3: _LL109:({ struct Cyc_Std_Int_pa_struct
_temp115; _temp115.tag= Cyc_Std_Int_pa; _temp115.f1=( int)(( unsigned int) Cyc_sizeof___fdmask);{
void* _temp114[ 1u]={& _temp115}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp114, sizeof( void*), 1u));}});({ void* _temp116[ 0u]={}; Cyc_log(
_tag_arr("Warning: sizeof(__fd_mask) inlined\n", sizeof( unsigned char), 36u),
_tag_arr( _temp116, sizeof( void*), 0u));}); return Cyc_slurp( lexbuf); case 4:
_LL113:({ struct Cyc_Std_Int_pa_struct _temp119; _temp119.tag= Cyc_Std_Int_pa;
_temp119.f1=( int)(( unsigned int) Cyc_sizeof_uint32);{ void* _temp118[ 1u]={&
_temp119}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp118, sizeof( void*),
1u));}});({ void* _temp120[ 0u]={}; Cyc_log( _tag_arr("Warning: sizeof(__uint32_t) inlined\n",
sizeof( unsigned char), 37u), _tag_arr( _temp120, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 5: _LL117:({ struct Cyc_Std_Int_pa_struct
_temp123; _temp123.tag= Cyc_Std_Int_pa; _temp123.f1=( int)(( unsigned int) Cyc_sizeof_inport);{
void* _temp122[ 1u]={& _temp123}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp122, sizeof( void*), 1u));}});({ void* _temp124[ 0u]={}; Cyc_log(
_tag_arr("Warning: sizeof(__in_port_t) inlined\n", sizeof( unsigned char), 38u),
_tag_arr( _temp124, sizeof( void*), 0u));}); return Cyc_slurp( lexbuf); case 6:
_LL121:({ struct Cyc_Std_Int_pa_struct _temp127; _temp127.tag= Cyc_Std_Int_pa;
_temp127.f1=( int)(( unsigned int) Cyc_sizeof_int);{ void* _temp126[ 1u]={&
_temp127}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp126, sizeof( void*),
1u));}});({ void* _temp128[ 0u]={}; Cyc_log( _tag_arr("Warning: sizeof(int) inlined\n",
sizeof( unsigned char), 30u), _tag_arr( _temp128, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 7: _LL125:({ struct Cyc_Std_Int_pa_struct
_temp131; _temp131.tag= Cyc_Std_Int_pa; _temp131.f1=( int)(( unsigned int) Cyc_sizeof_short);{
void* _temp130[ 1u]={& _temp131}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp130, sizeof( void*), 1u));}});({ void* _temp132[ 0u]={}; Cyc_log(
_tag_arr("Warning: sizeof(__short) inlined\n", sizeof( unsigned char), 34u),
_tag_arr( _temp132, sizeof( void*), 0u));}); return Cyc_slurp( lexbuf); case 8:
_LL129:({ struct Cyc_Std_Int_pa_struct _temp135; _temp135.tag= Cyc_Std_Int_pa;
_temp135.f1=( int)(( unsigned int) Cyc_sizeof_unsignedlongint);{ void* _temp134[
1u]={& _temp135}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp134, sizeof( void*),
1u));}});({ void* _temp136[ 0u]={}; Cyc_log( _tag_arr("Warning: sizeof(unsigned long int) inlined\n",
sizeof( unsigned char), 44u), _tag_arr( _temp136, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 9: _LL133:({ struct Cyc_Std_Int_pa_struct
_temp139; _temp139.tag= Cyc_Std_Int_pa; _temp139.f1=( int)(( unsigned int) Cyc_sizeof_unsignedshortint);{
void* _temp138[ 1u]={& _temp139}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp138, sizeof( void*), 1u));}});({ void* _temp140[ 0u]={}; Cyc_log(
_tag_arr("Warning: sizeof(unsigned short int) inlined\n", sizeof( unsigned char),
45u), _tag_arr( _temp140, sizeof( void*), 0u));}); return Cyc_slurp( lexbuf);
case 10: _LL137:({ struct Cyc_Std_Int_pa_struct _temp143; _temp143.tag= Cyc_Std_Int_pa;
_temp143.f1=( int)(( unsigned int) Cyc_sizeof_shortint);{ void* _temp142[ 1u]={&
_temp143}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp142, sizeof( void*),
1u));}});({ void* _temp144[ 0u]={}; Cyc_log( _tag_arr("Warning: sizeof(short int) inlined\n",
sizeof( unsigned char), 36u), _tag_arr( _temp144, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 11: _LL141:({ struct Cyc_Std_Int_pa_struct
_temp147; _temp147.tag= Cyc_Std_Int_pa; _temp147.f1=( int)(( unsigned int) Cyc_sizeof_sockaddr);{
void* _temp146[ 1u]={& _temp147}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp146, sizeof( void*), 1u));}});({ void* _temp148[ 0u]={}; Cyc_log(
_tag_arr("Warning: sizeof(struct sockaddr) inlined\n", sizeof( unsigned char),
42u), _tag_arr( _temp148, sizeof( void*), 0u));}); return Cyc_slurp( lexbuf);
case 12: _LL145:({ struct Cyc_Std_Int_pa_struct _temp151; _temp151.tag= Cyc_Std_Int_pa;
_temp151.f1=( int)(( unsigned int) Cyc_sizeof_inaddr);{ void* _temp150[ 1u]={&
_temp151}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp150, sizeof( void*),
1u));}});({ void* _temp152[ 0u]={}; Cyc_log( _tag_arr("Warning: sizeof(struct inaddr) inlined\n",
sizeof( unsigned char), 40u), _tag_arr( _temp152, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 13: _LL149: Cyc_Std_fputs( _tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC ",
sizeof( unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));({
void* _temp154[ 0u]={}; Cyc_log( _tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof( unsigned char), 44u), _tag_arr( _temp154, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 14: _LL153: Cyc_Std_fputs( _tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC ",
sizeof( unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));({
void* _temp156[ 0u]={}; Cyc_log( _tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof( unsigned char), 44u), _tag_arr( _temp156, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 15: _LL155: Cyc_Std_fputs( _tag_arr(" __IGNORE_FOR_CYCLONE_CALLOC ",
sizeof( unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));({
void* _temp158[ 0u]={}; Cyc_log( _tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof( unsigned char), 44u), _tag_arr( _temp158, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 16: _LL157: Cyc_Std_fputs( _tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC ",
sizeof( unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));({
void* _temp160[ 0u]={}; Cyc_log( _tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof( unsigned char), 44u), _tag_arr( _temp160, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 17: _LL159: return Cyc_slurp( lexbuf); case 18:
_LL161: return Cyc_slurp( lexbuf); case 19: _LL162: return Cyc_slurp( lexbuf);
case 20: _LL163: return Cyc_slurp( lexbuf); case 21: _LL164: return Cyc_slurp(
lexbuf); case 22: _LL165: return Cyc_slurp( lexbuf); case 23: _LL166: Cyc_Std_fputs(
_tag_arr("inline", sizeof( unsigned char), 7u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 24: _LL167: Cyc_Std_fputs(
_tag_arr("const", sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 25: _LL168: Cyc_Std_fputs(
_tag_arr("const", sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 26: _LL169: Cyc_Std_fputs(
_tag_arr("int", sizeof( unsigned char), 4u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 27: _LL170: return
Cyc_slurp( lexbuf); case 28: _LL171: Cyc_parens_to_match= 1; if( Cyc_asm( lexbuf)){
return 1;} Cyc_Std_fputs( _tag_arr("0", sizeof( unsigned char), 2u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out));({ void* _temp173[ 0u]={}; Cyc_log( _tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof( unsigned char), 42u), _tag_arr( _temp173, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 29: _LL172: Cyc_Std_fputc(( int) Cyc_Lexing_lexeme_char(
lexbuf, 0),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); return Cyc_slurp(
lexbuf); default: _LL174:( lexbuf->refill_buff)( lexbuf); return Cyc_slurp_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp176=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp176[ 0]=({ struct Cyc_Lexing_Error_struct _temp177; _temp177.tag= Cyc_Lexing_Error;
_temp177.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp177;}); _temp176;}));} int Cyc_slurp( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_slurp_rec( lexbuf, 5);} int Cyc_slurp_string_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL178: Cyc_Std_fputc(( int)'"',( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return 0; case 1: _LL179: return 1; case 2: _LL180:
return 1; case 3: _LL181:({ struct Cyc_Std_String_pa_struct _temp184; _temp184.tag=
Cyc_Std_String_pa; _temp184.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{
void* _temp183[ 1u]={& _temp184}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp183, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 4: _LL182:({ struct Cyc_Std_String_pa_struct _temp187; _temp187.tag= Cyc_Std_String_pa;
_temp187.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp186[ 1u]={&
_temp187}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp186, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 5: _LL185:({ struct Cyc_Std_String_pa_struct
_temp190; _temp190.tag= Cyc_Std_String_pa; _temp190.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp189[ 1u]={& _temp190}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp189, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 6: _LL188:({ struct Cyc_Std_String_pa_struct _temp193; _temp193.tag= Cyc_Std_String_pa;
_temp193.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp192[ 1u]={&
_temp193}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp192, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 7: _LL191:({ struct Cyc_Std_String_pa_struct
_temp196; _temp196.tag= Cyc_Std_String_pa; _temp196.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp195[ 1u]={& _temp196}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp195, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 8: _LL194:({ struct Cyc_Std_String_pa_struct _temp199; _temp199.tag= Cyc_Std_String_pa;
_temp199.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp198[ 1u]={&
_temp199}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp198, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); default: _LL197:( lexbuf->refill_buff)(
lexbuf); return Cyc_slurp_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp201=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp201[ 0]=({ struct Cyc_Lexing_Error_struct
_temp202; _temp202.tag= Cyc_Lexing_Error; _temp202.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp202;}); _temp201;}));} int Cyc_slurp_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_slurp_string_rec( lexbuf, 6);} int
Cyc_asm_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL203: if( Cyc_parens_to_match == 
1){ return 0;} Cyc_parens_to_match --; return Cyc_asm( lexbuf); case 1: _LL204:
Cyc_parens_to_match ++; return Cyc_asm( lexbuf); case 2: _LL205: Cyc_asm_string(
lexbuf); return Cyc_asm( lexbuf); case 3: _LL206: Cyc_asm_comment( lexbuf);
return Cyc_asm( lexbuf); case 4: _LL207: return Cyc_asm( lexbuf); case 5: _LL208:
return 0; case 6: _LL209: return Cyc_asm( lexbuf); default: _LL210:( lexbuf->refill_buff)(
lexbuf); return Cyc_asm_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp212=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp212[ 0]=({ struct Cyc_Lexing_Error_struct
_temp213; _temp213.tag= Cyc_Lexing_Error; _temp213.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp213;}); _temp212;}));} int Cyc_asm( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_rec( lexbuf, 7);} int Cyc_asm_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL214: return 0; case 1: _LL215:
return 1; case 2: _LL216: return 1; case 3: _LL217: return Cyc_asm_string(
lexbuf); case 4: _LL218: return Cyc_asm_string( lexbuf); case 5: _LL219: return
Cyc_asm_string( lexbuf); case 6: _LL220: return Cyc_asm_string( lexbuf); case 7:
_LL221: return Cyc_asm_string( lexbuf); case 8: _LL222: return Cyc_asm_string(
lexbuf); default: _LL223:( lexbuf->refill_buff)( lexbuf); return Cyc_asm_string_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp225=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp225[ 0]=({ struct Cyc_Lexing_Error_struct _temp226; _temp226.tag= Cyc_Lexing_Error;
_temp226.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp226;}); _temp225;}));} int Cyc_asm_string( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_asm_string_rec( lexbuf, 8);} int Cyc_asm_comment_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL227: return 0; case 1: _LL228: return 1; case 2: _LL229:
return Cyc_asm_comment( lexbuf); default: _LL230:( lexbuf->refill_buff)( lexbuf);
return Cyc_asm_comment_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp232=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp232[ 0]=({ struct Cyc_Lexing_Error_struct
_temp233; _temp233.tag= Cyc_Lexing_Error; _temp233.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp233;}); _temp232;}));} int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_comment_rec( lexbuf, 9);}
struct _tuple9* Cyc_suck_line_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case
0: _LL234: Cyc_current_line= _tag_arr("#define ", sizeof( unsigned char), 9u);
Cyc_suck_macroname( lexbuf); return({ struct _tuple9* _temp236=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp236->f1= Cyc_current_line; _temp236->f2=(
struct _tagged_arr*) _check_null( Cyc_current_source); _temp236;}); case 1:
_LL235: return Cyc_suck_line( lexbuf); case 2: _LL237: return 0; default: _LL238:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_line_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp240=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp240[ 0]=({ struct Cyc_Lexing_Error_struct
_temp241; _temp241.tag= Cyc_Lexing_Error; _temp241.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp241;}); _temp240;}));} struct _tuple9* Cyc_suck_line(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_line_rec( lexbuf, 10);} int
Cyc_suck_macroname_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL242:
Cyc_current_source=({ struct _tagged_arr* _temp244=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp244[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp244;}); Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat(
Cyc_current_line,*(( struct _tagged_arr*) _check_null( Cyc_current_source)));
return Cyc_suck_restofline( lexbuf); default: _LL243:( lexbuf->refill_buff)(
lexbuf); return Cyc_suck_macroname_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp246=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp246[ 0]=({ struct Cyc_Lexing_Error_struct
_temp247; _temp247.tag= Cyc_Lexing_Error; _temp247.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp247;}); _temp246;}));} int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_macroname_rec( lexbuf, 11);}
int Cyc_suck_restofline_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL248:
Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat( Cyc_current_line,(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return 0; default: _LL249:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_restofline_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp251=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp251[ 0]=({ struct Cyc_Lexing_Error_struct
_temp252; _temp252.tag= Cyc_Lexing_Error; _temp252.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp252;}); _temp251;}));} int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_restofline_rec( lexbuf, 12);}
struct _tuple11* Cyc_spec_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL253:
Cyc_current_header=( struct _tagged_arr) Cyc_Std_substring(( struct _tagged_arr)
Cyc_Lexing_lexeme( lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end( lexbuf) - 
Cyc_Lexing_lexeme_start( lexbuf)) -  1)); Cyc_current_symbols= 0; Cyc_current_omit_symbols=
0; Cyc_current_cstubs= 0; Cyc_current_cycstubs= 0; Cyc_current_prologue= 0; Cyc_current_epilogue=
0; if( Cyc_commands( lexbuf)){ return 0;} Cyc_current_prologue=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_current_prologue); Cyc_current_epilogue=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_current_epilogue);
Cyc_current_cstubs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_current_cstubs); Cyc_current_cycstubs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_current_cycstubs); return({ struct
_tuple11* _temp255=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11));
_temp255->f1= Cyc_current_header; _temp255->f2= Cyc_current_symbols; _temp255->f3=
Cyc_current_omit_symbols; _temp255->f4= Cyc_current_prologue; _temp255->f5= Cyc_current_epilogue;
_temp255->f6= Cyc_current_cstubs; _temp255->f7= Cyc_current_cycstubs; _temp255;});
case 1: _LL254: return Cyc_spec( lexbuf); case 2: _LL256: return Cyc_spec(
lexbuf); case 3: _LL257: return 0; case 4: _LL258:({ struct Cyc_Std_Int_pa_struct
_temp261; _temp261.tag= Cyc_Std_Int_pa; _temp261.f1=( int)(( unsigned int)(( int)
Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void* _temp260[ 1u]={& _temp261}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof( unsigned char), 67u), _tag_arr( _temp260, sizeof( void*), 1u));}});
return 0; default: _LL259:( lexbuf->refill_buff)( lexbuf); return Cyc_spec_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp263=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp263[ 0]=({ struct Cyc_Lexing_Error_struct _temp264; _temp264.tag= Cyc_Lexing_Error;
_temp264.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp264;}); _temp263;}));} struct _tuple11* Cyc_spec( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_spec_rec( lexbuf, 13);} int Cyc_commands_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL265: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols( lexbuf)){
return 1;} Cyc_current_symbols=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols, Cyc_current_symbols);
return Cyc_commands( lexbuf); case 1: _LL266: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols(
lexbuf)){ return 1;} Cyc_current_omit_symbols=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols,
Cyc_current_omit_symbols); return Cyc_commands( lexbuf); case 2: _LL267: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp270=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp270->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp270->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp270;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp269=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp269->hd=( void*) x; _temp269->tl= Cyc_current_prologue;
_temp269;}); return Cyc_commands( lexbuf);} case 3: _LL268: { struct _tagged_arr
_temp272= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp272,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("prologue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp272, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp272, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp272; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp273= Cyc_Std_substring(( struct _tagged_arr) _temp272, 0,(
unsigned int)(( t.curr -  _temp272.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp275=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp275->f1=( struct
_tagged_arr) _temp273; _temp275->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp275;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp274=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp274->hd=( void*) x; _temp274->tl= Cyc_current_prologue;
_temp274;}); return Cyc_commands( lexbuf);}}}} case 4: _LL271: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp278=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp278->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp278->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp278;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp277=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp277->hd=( void*) x; _temp277->tl= Cyc_current_epilogue;
_temp277;}); return Cyc_commands( lexbuf);} case 5: _LL276: { struct _tagged_arr
_temp280= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp280,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("epilogue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp280, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp280, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp280; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp281= Cyc_Std_substring(( struct _tagged_arr) _temp280, 0,(
unsigned int)(( t.curr -  _temp280.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp283=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp283->f1=( struct
_tagged_arr) _temp281; _temp283->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp283;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp282=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp282->hd=( void*) x; _temp282->tl= Cyc_current_epilogue;
_temp282;}); return Cyc_commands( lexbuf);}}}} case 6: _LL279: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp286=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp286->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp286->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp286;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp285=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp285->hd=( void*) x; _temp285->tl= Cyc_current_cstubs;
_temp285;}); return Cyc_commands( lexbuf);} case 7: _LL284: { struct _tagged_arr
_temp288= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp288,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cstub", sizeof(
unsigned char), 6u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp288, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp288, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp288; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp289= Cyc_Std_substring(( struct _tagged_arr) _temp288, 0,(
unsigned int)(( t.curr -  _temp288.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp291=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp291->f1=( struct
_tagged_arr) _temp289; _temp291->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp291;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp290=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp290->hd=( void*) x; _temp290->tl= Cyc_current_cstubs;
_temp290;}); return Cyc_commands( lexbuf);}}}} case 8: _LL287: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp294=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp294->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp294->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp294;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp293=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp293->hd=( void*) x; _temp293->tl= Cyc_current_cycstubs;
_temp293;}); return Cyc_commands( lexbuf);} case 9: _LL292: { struct _tagged_arr
_temp296= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp296,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cycstub", sizeof(
unsigned char), 8u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp296, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp296, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp296; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp297= Cyc_Std_substring(( struct _tagged_arr) _temp296, 0,(
unsigned int)(( t.curr -  _temp296.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp299=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp299->f1=( struct
_tagged_arr) _temp297; _temp299->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp299;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp298=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp298->hd=( void*) x; _temp298->tl= Cyc_current_cycstubs;
_temp298;}); return Cyc_commands( lexbuf);}}}} case 10: _LL295: return Cyc_commands(
lexbuf); case 11: _LL300: return Cyc_commands( lexbuf); case 12: _LL301: return
0; case 13: _LL302: return 0; case 14: _LL303:({ struct Cyc_Std_Int_pa_struct
_temp306; _temp306.tag= Cyc_Std_Int_pa; _temp306.f1=( int)(( unsigned int)(( int)
Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void* _temp305[ 1u]={& _temp306}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof( unsigned char), 58u), _tag_arr( _temp305, sizeof( void*), 1u));}});
return 1; default: _LL304:( lexbuf->refill_buff)( lexbuf); return Cyc_commands_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp308=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp308[ 0]=({ struct Cyc_Lexing_Error_struct _temp309; _temp309.tag= Cyc_Lexing_Error;
_temp309.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp309;}); _temp308;}));} int Cyc_commands( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_commands_rec( lexbuf, 14);} int Cyc_snarfsymbols_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL310: Cyc_snarfed_symbols=({ struct Cyc_List_List* _temp312=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp312->hd=(
void*)({ struct _tagged_arr* _temp313=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp313[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp313;}); _temp312->tl= Cyc_snarfed_symbols; _temp312;}); return Cyc_snarfsymbols(
lexbuf); case 1: _LL311: return Cyc_snarfsymbols( lexbuf); case 2: _LL314:
return 0; case 3: _LL315:({ void* _temp317[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n", sizeof( unsigned char),
44u), _tag_arr( _temp317, sizeof( void*), 0u));}); return 1; case 4: _LL316:({
struct Cyc_Std_Int_pa_struct _temp320; _temp320.tag= Cyc_Std_Int_pa; _temp320.f1=(
int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void*
_temp319[ 1u]={& _temp320}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof( unsigned char), 57u), _tag_arr( _temp319, sizeof( void*), 1u));}});
return 1; default: _LL318:( lexbuf->refill_buff)( lexbuf); return Cyc_snarfsymbols_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp322=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp322[ 0]=({ struct Cyc_Lexing_Error_struct _temp323; _temp323.tag= Cyc_Lexing_Error;
_temp323.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp323;}); _temp322;}));} int Cyc_snarfsymbols( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_snarfsymbols_rec( lexbuf, 15);} int Cyc_block_rec( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate,
lexbuf); switch( lexstate){ case 0: _LL324: if( Cyc_braces_to_match ==  1){
return 0;} Cyc_braces_to_match --; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'}'); return Cyc_block( lexbuf); case 1: _LL325: Cyc_braces_to_match
++; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'{');
return Cyc_block( lexbuf); case 2: _LL326: Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'"'); Cyc_block_string( lexbuf); return Cyc_block(
lexbuf); case 3: _LL327: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf), _tag_arr("/*", sizeof( unsigned char), 3u)); Cyc_block_comment(
lexbuf); return Cyc_block( lexbuf); case 4: _LL328: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block( lexbuf); case 5: _LL329: return 0; case 6: _LL330:
Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf), Cyc_Lexing_lexeme_char(
lexbuf, 0)); return Cyc_block( lexbuf); default: _LL331:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp333=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp333[ 0]=({ struct Cyc_Lexing_Error_struct
_temp334; _temp334.tag= Cyc_Lexing_Error; _temp334.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp334;}); _temp333;}));} int Cyc_block( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_rec( lexbuf, 16);} int Cyc_block_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL335: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'"'); return 0; case 1: _LL336:
return 1; case 2: _LL337: return 1; case 3: _LL338: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 4: _LL339: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 5: _LL340: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 6: _LL341: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 7: _LL342: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 8: _LL343: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); default: _LL344:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp346=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp346[ 0]=({ struct Cyc_Lexing_Error_struct
_temp347; _temp347.tag= Cyc_Lexing_Error; _temp347.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp347;}); _temp346;}));} int Cyc_block_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_string_rec( lexbuf, 17);}
int Cyc_block_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL348:
Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),
_tag_arr("*/", sizeof( unsigned char), 3u)); return 0; case 1: _LL349: return 1;
case 2: _LL350: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return Cyc_block_comment(
lexbuf); default: _LL351:( lexbuf->refill_buff)( lexbuf); return Cyc_block_comment_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp353=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp353[ 0]=({ struct Cyc_Lexing_Error_struct _temp354; _temp354.tag= Cyc_Lexing_Error;
_temp354.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp354;}); _temp353;}));} int Cyc_block_comment( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_block_comment_rec( lexbuf, 18);} static void Cyc_pr( struct
_tagged_arr* sptr){({ struct Cyc_Std_String_pa_struct _temp356; _temp356.tag=
Cyc_Std_String_pa; _temp356.f1=( struct _tagged_arr)* sptr;{ void* _temp355[ 1u]={&
_temp356}; Cyc_Std_printf( _tag_arr(" %s\n", sizeof( unsigned char), 5u),
_tag_arr( _temp355, sizeof( void*), 1u));}});} extern void Cyc_scan_type( void*
t); struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
void Cyc_scan_exp( struct Cyc_Absyn_Exp* e){ void* _temp357=( void*)(( struct
Cyc_Absyn_Exp*) _check_null( e))->r; struct _tuple0* _temp435; struct _tuple0
_temp437; struct _tagged_arr* _temp438; struct _tuple0* _temp440; struct _tuple0
_temp442; struct _tagged_arr* _temp443; struct Cyc_List_List* _temp445; struct
Cyc_Absyn_Exp* _temp447; struct Cyc_Absyn_Exp* _temp449; struct Cyc_Absyn_Exp*
_temp451; struct Cyc_Absyn_Exp* _temp453; struct Cyc_Absyn_Exp* _temp455; struct
Cyc_Absyn_Exp* _temp457; struct Cyc_Absyn_Exp* _temp459; struct Cyc_Absyn_Exp*
_temp461; struct Cyc_Absyn_Exp* _temp463; struct Cyc_Absyn_Exp* _temp465; struct
Cyc_Absyn_Exp* _temp467; struct Cyc_Absyn_Exp* _temp469; struct Cyc_Absyn_Exp*
_temp471; struct Cyc_List_List* _temp473; struct Cyc_Absyn_Exp* _temp475; struct
Cyc_List_List* _temp477; struct Cyc_Absyn_Exp* _temp479; struct Cyc_Absyn_Exp*
_temp481; void* _temp483; struct Cyc_Absyn_MallocInfo _temp485; struct Cyc_Absyn_Exp*
_temp487; void** _temp489; struct Cyc_Absyn_Exp* _temp491; int _temp493; void*
_temp495; struct _tagged_arr* _temp497; struct Cyc_Absyn_Exp* _temp499; struct
_tagged_arr* _temp501; struct Cyc_Absyn_Exp* _temp503; void* _temp505; void*
_temp507; struct Cyc_List_List* _temp509; _LL359: if(*(( int*) _temp357) ==  Cyc_Absyn_Var_e){
_LL436: _temp435=(( struct Cyc_Absyn_Var_e_struct*) _temp357)->f1; _temp437=*
_temp435; _LL439: _temp438= _temp437.f2; goto _LL360;} else{ goto _LL361;}
_LL361: if(*(( int*) _temp357) ==  Cyc_Absyn_UnknownId_e){ _LL441: _temp440=((
struct Cyc_Absyn_UnknownId_e_struct*) _temp357)->f1; _temp442=* _temp440; _LL444:
_temp443= _temp442.f2; goto _LL362;} else{ goto _LL363;} _LL363: if(*(( int*)
_temp357) ==  Cyc_Absyn_Primop_e){ _LL446: _temp445=(( struct Cyc_Absyn_Primop_e_struct*)
_temp357)->f2; goto _LL364;} else{ goto _LL365;} _LL365: if(*(( int*) _temp357)
==  Cyc_Absyn_Subscript_e){ _LL450: _temp449=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp357)->f1; goto _LL448; _LL448: _temp447=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp357)->f2; goto _LL366;} else{ goto _LL367;} _LL367: if(*(( int*) _temp357)
==  Cyc_Absyn_SeqExp_e){ _LL454: _temp453=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp357)->f1; goto _LL452; _LL452: _temp451=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp357)->f2; goto _LL368;} else{ goto _LL369;} _LL369: if(*(( int*) _temp357)
==  Cyc_Absyn_AssignOp_e){ _LL458: _temp457=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp357)->f1; goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp357)->f3; goto _LL370;} else{ goto _LL371;} _LL371: if(*(( int*) _temp357)
==  Cyc_Absyn_Deref_e){ _LL460: _temp459=(( struct Cyc_Absyn_Deref_e_struct*)
_temp357)->f1; goto _LL372;} else{ goto _LL373;} _LL373: if(*(( int*) _temp357)
==  Cyc_Absyn_Sizeofexp_e){ _LL462: _temp461=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp357)->f1; goto _LL374;} else{ goto _LL375;} _LL375: if(*(( int*) _temp357)
==  Cyc_Absyn_Address_e){ _LL464: _temp463=(( struct Cyc_Absyn_Address_e_struct*)
_temp357)->f1; goto _LL376;} else{ goto _LL377;} _LL377: if(*(( int*) _temp357)
==  Cyc_Absyn_Increment_e){ _LL466: _temp465=(( struct Cyc_Absyn_Increment_e_struct*)
_temp357)->f1; goto _LL378;} else{ goto _LL379;} _LL379: if(*(( int*) _temp357)
==  Cyc_Absyn_Conditional_e){ _LL472: _temp471=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp357)->f1; goto _LL470; _LL470: _temp469=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp357)->f2; goto _LL468; _LL468: _temp467=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp357)->f3; goto _LL380;} else{ goto _LL381;} _LL381: if(*(( int*) _temp357)
==  Cyc_Absyn_FnCall_e){ _LL476: _temp475=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp357)->f1; goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp357)->f2; goto _LL382;} else{ goto _LL383;} _LL383: if(*(( int*) _temp357)
==  Cyc_Absyn_UnknownCall_e){ _LL480: _temp479=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp357)->f1; goto _LL478; _LL478: _temp477=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp357)->f2; goto _LL384;} else{ goto _LL385;} _LL385: if(*(( int*) _temp357)
==  Cyc_Absyn_Cast_e){ _LL484: _temp483=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp357)->f1; goto _LL482; _LL482: _temp481=(( struct Cyc_Absyn_Cast_e_struct*)
_temp357)->f2; goto _LL386;} else{ goto _LL387;} _LL387: if(*(( int*) _temp357)
==  Cyc_Absyn_Malloc_e){ _LL486: _temp485=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp357)->f1; _LL494: _temp493= _temp485.is_calloc; goto _LL492; _LL492:
_temp491= _temp485.rgn; goto _LL490; _LL490: _temp489= _temp485.elt_type; goto
_LL488; _LL488: _temp487= _temp485.num_elts; goto _LL388;} else{ goto _LL389;}
_LL389: if(*(( int*) _temp357) ==  Cyc_Absyn_Sizeoftyp_e){ _LL496: _temp495=(
void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp357)->f1; goto _LL390;}
else{ goto _LL391;} _LL391: if(*(( int*) _temp357) ==  Cyc_Absyn_StructMember_e){
_LL500: _temp499=(( struct Cyc_Absyn_StructMember_e_struct*) _temp357)->f1; goto
_LL498; _LL498: _temp497=(( struct Cyc_Absyn_StructMember_e_struct*) _temp357)->f2;
goto _LL392;} else{ goto _LL393;} _LL393: if(*(( int*) _temp357) ==  Cyc_Absyn_StructArrow_e){
_LL504: _temp503=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp357)->f1; goto
_LL502; _LL502: _temp501=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp357)->f2;
goto _LL394;} else{ goto _LL395;} _LL395: if(*(( int*) _temp357) ==  Cyc_Absyn_Offsetof_e){
_LL508: _temp507=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp357)->f1;
goto _LL506; _LL506: _temp505=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp357)->f2; goto _LL396;} else{ goto _LL397;} _LL397: if(*(( int*) _temp357)
==  Cyc_Absyn_Const_e){ goto _LL398;} else{ goto _LL399;} _LL399: if(*(( int*)
_temp357) ==  Cyc_Absyn_UnresolvedMem_e){ _LL510: _temp509=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp357)->f2; goto _LL400;} else{ goto _LL401;} _LL401: if(*(( int*) _temp357)
==  Cyc_Absyn_StmtExp_e){ goto _LL402;} else{ goto _LL403;} _LL403: if(*(( int*)
_temp357) ==  Cyc_Absyn_Throw_e){ goto _LL404;} else{ goto _LL405;} _LL405: if(*((
int*) _temp357) ==  Cyc_Absyn_NoInstantiate_e){ goto _LL406;} else{ goto _LL407;}
_LL407: if(*(( int*) _temp357) ==  Cyc_Absyn_Instantiate_e){ goto _LL408;} else{
goto _LL409;} _LL409: if(*(( int*) _temp357) ==  Cyc_Absyn_New_e){ goto _LL410;}
else{ goto _LL411;} _LL411: if(*(( int*) _temp357) ==  Cyc_Absyn_Gentyp_e){ goto
_LL412;} else{ goto _LL413;} _LL413: if(*(( int*) _temp357) ==  Cyc_Absyn_Tuple_e){
goto _LL414;} else{ goto _LL415;} _LL415: if(*(( int*) _temp357) ==  Cyc_Absyn_CompoundLit_e){
goto _LL416;} else{ goto _LL417;} _LL417: if(*(( int*) _temp357) ==  Cyc_Absyn_Array_e){
goto _LL418;} else{ goto _LL419;} _LL419: if(*(( int*) _temp357) ==  Cyc_Absyn_Comprehension_e){
goto _LL420;} else{ goto _LL421;} _LL421: if(*(( int*) _temp357) ==  Cyc_Absyn_Struct_e){
goto _LL422;} else{ goto _LL423;} _LL423: if(*(( int*) _temp357) ==  Cyc_Absyn_AnonStruct_e){
goto _LL424;} else{ goto _LL425;} _LL425: if(*(( int*) _temp357) ==  Cyc_Absyn_Tunion_e){
goto _LL426;} else{ goto _LL427;} _LL427: if(*(( int*) _temp357) ==  Cyc_Absyn_Enum_e){
goto _LL428;} else{ goto _LL429;} _LL429: if(*(( int*) _temp357) ==  Cyc_Absyn_AnonEnum_e){
goto _LL430;} else{ goto _LL431;} _LL431: if(*(( int*) _temp357) ==  Cyc_Absyn_Codegen_e){
goto _LL432;} else{ goto _LL433;} _LL433: if(*(( int*) _temp357) ==  Cyc_Absyn_Fill_e){
goto _LL434;} else{ goto _LL358;} _LL360: _temp443= _temp438; goto _LL362;
_LL362: Cyc_add_target( _temp443); return; _LL364: for( 0; _temp445 !=  0;
_temp445= _temp445->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_temp445->hd));} return; _LL366: _temp453= _temp449; _temp451= _temp447; goto
_LL368; _LL368: _temp457= _temp453; _temp455= _temp451; goto _LL370; _LL370: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp457); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp455);
return; _LL372: _temp461= _temp459; goto _LL374; _LL374: _temp463= _temp461;
goto _LL376; _LL376: _temp465= _temp463; goto _LL378; _LL378: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp465); return; _LL380: Cyc_scan_exp(( struct Cyc_Absyn_Exp*)
_temp471); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp469); Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp467); return; _LL382: _temp479= _temp475; _temp477=
_temp473; goto _LL384; _LL384: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp479);
for( 0; _temp477 !=  0; _temp477= _temp477->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _temp477->hd));} return; _LL386: Cyc_scan_type( _temp483);
Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp481); return; _LL388: if( _temp491 != 
0){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp491)));} if( _temp489 !=  0){ Cyc_scan_type(* _temp489);} Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp487); return; _LL390: Cyc_scan_type( _temp495);
return; _LL392: _temp503= _temp499; _temp501= _temp497; goto _LL394; _LL394: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp503); Cyc_add_target( _temp501); return; _LL396: Cyc_scan_type(
_temp507);{ void* _temp511= _temp505; struct _tagged_arr* _temp517; _LL513: if(*((
int*) _temp511) ==  Cyc_Absyn_StructField){ _LL518: _temp517=(( struct Cyc_Absyn_StructField_struct*)
_temp511)->f1; goto _LL514;} else{ goto _LL515;} _LL515: if(*(( int*) _temp511)
==  Cyc_Absyn_TupleIndex){ goto _LL516;} else{ goto _LL512;} _LL514: Cyc_add_target(
_temp517); goto _LL512; _LL516: goto _LL512; _LL512:;} return; _LL398: return;
_LL400: for( 0; _temp509 !=  0; _temp509= _temp509->tl){ struct _tuple12
_temp521; struct Cyc_Absyn_Exp* _temp522; struct _tuple12* _temp519=( struct
_tuple12*) _temp509->hd; _temp521=* _temp519; _LL523: _temp522= _temp521.f2;
goto _LL520; _LL520: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp522);} return;
_LL402:({ void* _temp524[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Stmt_e\n",
sizeof( unsigned char), 26u), _tag_arr( _temp524, sizeof( void*), 0u));}); exit(
1); return; _LL404:({ void* _temp525[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Throw_e\n", sizeof( unsigned char), 27u), _tag_arr(
_temp525, sizeof( void*), 0u));}); exit( 1); return; _LL406:({ void* _temp526[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected NoInstantiate_e\n",
sizeof( unsigned char), 35u), _tag_arr( _temp526, sizeof( void*), 0u));}); exit(
1); return; _LL408:({ void* _temp527[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Instantiate_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp527, sizeof( void*), 0u));}); exit( 1); return; _LL410:({ void*
_temp528[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected New_e\n",
sizeof( unsigned char), 25u), _tag_arr( _temp528, sizeof( void*), 0u));}); exit(
1); return; _LL412:({ void* _temp529[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Gentyp_e\n", sizeof( unsigned char), 28u), _tag_arr(
_temp529, sizeof( void*), 0u));}); exit( 1); return; _LL414:({ void* _temp530[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tuple_e\n", sizeof(
unsigned char), 27u), _tag_arr( _temp530, sizeof( void*), 0u));}); exit( 1);
return; _LL416:({ void* _temp531[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected CompoundLit_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp531, sizeof( void*), 0u));}); exit( 1); return; _LL418:({ void*
_temp532[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Array_e\n",
sizeof( unsigned char), 27u), _tag_arr( _temp532, sizeof( void*), 0u));}); exit(
1); return; _LL420:({ void* _temp533[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Comprehension_e\n", sizeof( unsigned char), 35u),
_tag_arr( _temp533, sizeof( void*), 0u));}); exit( 1); return; _LL422:({ void*
_temp534[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Struct_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp534, sizeof( void*), 0u));}); exit(
1); return; _LL424:({ void* _temp535[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonStruct_e\n", sizeof( unsigned char), 32u),
_tag_arr( _temp535, sizeof( void*), 0u));}); exit( 1); return; _LL426:({ void*
_temp536[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tunion_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp536, sizeof( void*), 0u));}); exit(
1); return; _LL428:({ void* _temp537[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Enum_e\n", sizeof( unsigned char), 26u), _tag_arr(
_temp537, sizeof( void*), 0u));}); exit( 1); return; _LL430:({ void* _temp538[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AnonEnum_e\n",
sizeof( unsigned char), 30u), _tag_arr( _temp538, sizeof( void*), 0u));}); exit(
1); return; _LL432:({ void* _temp539[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Codegen_e\n", sizeof( unsigned char), 29u), _tag_arr(
_temp539, sizeof( void*), 0u));}); exit( 1); return; _LL434:({ void* _temp540[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Fill_e\n", sizeof(
unsigned char), 26u), _tag_arr( _temp540, sizeof( void*), 0u));}); exit( 1);
return; _LL358:;} void Cyc_scan_exp_opt( struct Cyc_Absyn_Exp* eo){ if((
unsigned int) eo){ Cyc_scan_exp( eo);} return;} void Cyc_scan_type( void* t){
void* _temp541= t; struct Cyc_Absyn_PtrInfo _temp597; struct Cyc_Absyn_Exp*
_temp599; void* _temp601; struct Cyc_Absyn_FnInfo _temp603; struct Cyc_List_List*
_temp605; struct Cyc_List_List* _temp607; struct _tuple0* _temp609; struct
_tuple0* _temp611; struct _tuple0* _temp613; struct _tuple0 _temp615; struct
_tagged_arr* _temp616; struct _tuple0* _temp618; struct _tuple0 _temp620; struct
_tagged_arr* _temp621; struct _tuple0* _temp623; struct _tuple0 _temp625; struct
_tagged_arr* _temp626; struct _tuple0* _temp628; struct _tuple0 _temp630; struct
_tagged_arr* _temp631; _LL543: if( _temp541 == ( void*) Cyc_Absyn_VoidType){
goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_IntType: 0){ goto _LL546;} else{ goto _LL547;}
_LL547: if( _temp541 == ( void*) Cyc_Absyn_FloatType){ goto _LL548;} else{ goto
_LL549;} _LL549: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_DoubleType:
0){ goto _LL550;} else{ goto _LL551;} _LL551: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_PointerType: 0){ _LL598: _temp597=(( struct Cyc_Absyn_PointerType_struct*)
_temp541)->f1; goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int)
_temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_ArrayType: 0){ _LL602: _temp601=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp541)->f1; goto _LL600; _LL600:
_temp599=(( struct Cyc_Absyn_ArrayType_struct*) _temp541)->f3; goto _LL554;}
else{ goto _LL555;} _LL555: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541)
==  Cyc_Absyn_FnType: 0){ _LL604: _temp603=(( struct Cyc_Absyn_FnType_struct*)
_temp541)->f1; goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int)
_temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_AnonStructType: 0){ _LL606:
_temp605=(( struct Cyc_Absyn_AnonStructType_struct*) _temp541)->f1; goto _LL558;}
else{ goto _LL559;} _LL559: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541)
==  Cyc_Absyn_AnonUnionType: 0){ _LL608: _temp607=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp541)->f1; goto _LL560;} else{ goto _LL561;} _LL561: if(( unsigned int)
_temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL562;}
else{ goto _LL563;} _LL563: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541)
==  Cyc_Absyn_StructType: 0){ _LL610: _temp609=(( struct Cyc_Absyn_StructType_struct*)
_temp541)->f1; if( _temp609 ==  0){ goto _LL564;} else{ goto _LL565;}} else{
goto _LL565;} _LL565: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) == 
Cyc_Absyn_UnionType: 0){ _LL612: _temp611=(( struct Cyc_Absyn_UnionType_struct*)
_temp541)->f1; if( _temp611 ==  0){ goto _LL566;} else{ goto _LL567;}} else{
goto _LL567;} _LL567: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) == 
Cyc_Absyn_StructType: 0){ _LL614: _temp613=(( struct Cyc_Absyn_StructType_struct*)
_temp541)->f1; if( _temp613 ==  0){ goto _LL569;} else{ _temp615=* _temp613;
_LL617: _temp616= _temp615.f2; goto _LL568;}} else{ goto _LL569;} _LL569: if((
unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_UnionType: 0){
_LL619: _temp618=(( struct Cyc_Absyn_UnionType_struct*) _temp541)->f1; if(
_temp618 ==  0){ goto _LL571;} else{ _temp620=* _temp618; _LL622: _temp621=
_temp620.f2; goto _LL570;}} else{ goto _LL571;} _LL571: if(( unsigned int)
_temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_EnumType: 0){ _LL624: _temp623=((
struct Cyc_Absyn_EnumType_struct*) _temp541)->f1; _temp625=* _temp623; _LL627:
_temp626= _temp625.f2; goto _LL572;} else{ goto _LL573;} _LL573: if((
unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_TypedefType: 0){
_LL629: _temp628=(( struct Cyc_Absyn_TypedefType_struct*) _temp541)->f1;
_temp630=* _temp628; _LL632: _temp631= _temp630.f2; goto _LL574;} else{ goto
_LL575;} _LL575: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_Evar:
0){ goto _LL576;} else{ goto _LL577;} _LL577: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_VarType: 0){ goto _LL578;} else{ goto _LL579;}
_LL579: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_TunionType:
0){ goto _LL580;} else{ goto _LL581;} _LL581: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL582;} else{ goto
_LL583;} _LL583: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_TupleType:
0){ goto _LL584;} else{ goto _LL585;} _LL585: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL586;} else{ goto _LL587;}
_LL587: if( _temp541 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL588;} else{ goto
_LL589;} _LL589: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_AccessEff:
0){ goto _LL590;} else{ goto _LL591;} _LL591: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_JoinEff: 0){ goto _LL592;} else{ goto _LL593;}
_LL593: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_RgnsEff:
0){ goto _LL594;} else{ goto _LL595;} _LL595: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_SizeofType: 0){ goto _LL596;} else{ goto _LL542;}
_LL544: goto _LL546; _LL546: goto _LL548; _LL548: goto _LL550; _LL550: return;
_LL552: Cyc_scan_type(( void*) _temp597.elt_typ); return; _LL554: Cyc_scan_type(
_temp601); Cyc_scan_exp_opt( _temp599); return; _LL556: Cyc_scan_type(( void*)
_temp603.ret_typ);{ struct Cyc_List_List* _temp633= _temp603.args; for( 0;
_temp633 !=  0; _temp633= _temp633->tl){ struct _tuple1 _temp636; void* _temp637;
struct _tuple1* _temp634=( struct _tuple1*) _temp633->hd; _temp636=* _temp634;
_LL638: _temp637= _temp636.f3; goto _LL635; _LL635: Cyc_scan_type( _temp637);}}
if( _temp603.cyc_varargs !=  0){ Cyc_scan_type(( void*)( _temp603.cyc_varargs)->type);}
return; _LL558: _temp607= _temp605; goto _LL560; _LL560: for( 0; _temp607 !=  0;
_temp607= _temp607->tl){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_Structfield*)
_temp607->hd)->type); Cyc_scan_exp_opt((( struct Cyc_Absyn_Structfield*)
_temp607->hd)->width);} return; _LL562: return; _LL564: return; _LL566: return;
_LL568: _temp621= _temp616; goto _LL570; _LL570: _temp626= _temp621; goto _LL572;
_LL572: _temp631= _temp626; goto _LL574; _LL574: Cyc_add_target( _temp631);
return; _LL576:({ void* _temp639[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Evar\n", sizeof( unsigned char), 24u), _tag_arr(
_temp639, sizeof( void*), 0u));}); exit( 1); return; _LL578:({ void* _temp640[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected VarType\n", sizeof(
unsigned char), 27u), _tag_arr( _temp640, sizeof( void*), 0u));}); exit( 1);
return; _LL580:({ void* _temp641[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionType\n", sizeof( unsigned char), 30u),
_tag_arr( _temp641, sizeof( void*), 0u));}); exit( 1); return; _LL582:({ void*
_temp642[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected TunionFieldType\n",
sizeof( unsigned char), 35u), _tag_arr( _temp642, sizeof( void*), 0u));}); exit(
1); return; _LL584:({ void* _temp643[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n", sizeof( unsigned char), 29u), _tag_arr(
_temp643, sizeof( void*), 0u));}); exit( 1); return; _LL586:({ void* _temp644[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnHandleType\n",
sizeof( unsigned char), 33u), _tag_arr( _temp644, sizeof( void*), 0u));}); exit(
1); return; _LL588:({ void* _temp645[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected HeapRgn\n", sizeof( unsigned char), 27u), _tag_arr(
_temp645, sizeof( void*), 0u));}); exit( 1); return; _LL590:({ void* _temp646[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AccessEff\n",
sizeof( unsigned char), 29u), _tag_arr( _temp646, sizeof( void*), 0u));}); exit(
1); return; _LL592:({ void* _temp647[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected JoinEff\n", sizeof( unsigned char), 27u), _tag_arr(
_temp647, sizeof( void*), 0u));}); exit( 1); return; _LL594:({ void* _temp648[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnsEff\n", sizeof(
unsigned char), 27u), _tag_arr( _temp648, sizeof( void*), 0u));}); exit( 1);
return; _LL596:({ void* _temp649[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected sizeof_t\n", sizeof( unsigned char), 28u), _tag_arr(
_temp649, sizeof( void*), 0u));}); exit( 1); return; _LL542:;} struct _tuple13{
struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct _tuple8*
Cyc_scan_decl( struct Cyc_Absyn_Decl* d){ Cyc_current_targets=({ struct Cyc_Set_Set**
_temp650=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp650[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); _temp650;});{ void* _temp651=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp677; struct Cyc_Absyn_Fndecl*
_temp679; struct Cyc_Absyn_Structdecl* _temp681; struct Cyc_Absyn_Uniondecl*
_temp683; struct Cyc_Absyn_Enumdecl* _temp685; struct Cyc_Absyn_Typedefdecl*
_temp687; _LL653: if(*(( int*) _temp651) ==  Cyc_Absyn_Var_d){ _LL678: _temp677=((
struct Cyc_Absyn_Var_d_struct*) _temp651)->f1; goto _LL654;} else{ goto _LL655;}
_LL655: if(*(( int*) _temp651) ==  Cyc_Absyn_Fn_d){ _LL680: _temp679=(( struct
Cyc_Absyn_Fn_d_struct*) _temp651)->f1; goto _LL656;} else{ goto _LL657;} _LL657:
if(*(( int*) _temp651) ==  Cyc_Absyn_Struct_d){ _LL682: _temp681=(( struct Cyc_Absyn_Struct_d_struct*)
_temp651)->f1; goto _LL658;} else{ goto _LL659;} _LL659: if(*(( int*) _temp651)
==  Cyc_Absyn_Union_d){ _LL684: _temp683=(( struct Cyc_Absyn_Union_d_struct*)
_temp651)->f1; goto _LL660;} else{ goto _LL661;} _LL661: if(*(( int*) _temp651)
==  Cyc_Absyn_Enum_d){ _LL686: _temp685=(( struct Cyc_Absyn_Enum_d_struct*)
_temp651)->f1; goto _LL662;} else{ goto _LL663;} _LL663: if(*(( int*) _temp651)
==  Cyc_Absyn_Typedef_d){ _LL688: _temp687=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp651)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp651)
==  Cyc_Absyn_Let_d){ goto _LL666;} else{ goto _LL667;} _LL667: if(*(( int*)
_temp651) ==  Cyc_Absyn_Tunion_d){ goto _LL668;} else{ goto _LL669;} _LL669: if(*((
int*) _temp651) ==  Cyc_Absyn_Letv_d){ goto _LL670;} else{ goto _LL671;} _LL671:
if(*(( int*) _temp651) ==  Cyc_Absyn_Namespace_d){ goto _LL672;} else{ goto
_LL673;} _LL673: if(*(( int*) _temp651) ==  Cyc_Absyn_Using_d){ goto _LL674;}
else{ goto _LL675;} _LL675: if(*(( int*) _temp651) ==  Cyc_Absyn_ExternC_d){
goto _LL676;} else{ goto _LL652;} _LL654: { struct _tuple0 _temp691; struct
_tagged_arr* _temp692; struct _tuple0* _temp689= _temp677->name; _temp691=*
_temp689; _LL693: _temp692= _temp691.f2; goto _LL690; _LL690: Cyc_current_source=(
struct _tagged_arr*) _temp692; Cyc_scan_type(( void*) _temp677->type); Cyc_scan_exp_opt(
_temp677->initializer); goto _LL652;} _LL656: { struct _tuple0 _temp696; struct
_tagged_arr* _temp697; struct _tuple0* _temp694= _temp679->name; _temp696=*
_temp694; _LL698: _temp697= _temp696.f2; goto _LL695; _LL695: Cyc_current_source=(
struct _tagged_arr*) _temp697; Cyc_scan_type(( void*) _temp679->ret_type);{
struct Cyc_List_List* _temp699= _temp679->args; for( 0; _temp699 !=  0; _temp699=
_temp699->tl){ struct _tuple13 _temp702; void* _temp703; struct _tuple13*
_temp700=( struct _tuple13*) _temp699->hd; _temp702=* _temp700; _LL704: _temp703=
_temp702.f3; goto _LL701; _LL701: Cyc_scan_type( _temp703);}} if( _temp679->cyc_varargs
!=  0){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp679->cyc_varargs))->type);} if( _temp679->is_inline){({ void* _temp705[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: ignoring inline function\n",
sizeof( unsigned char), 35u), _tag_arr( _temp705, sizeof( void*), 0u));});} goto
_LL652;} _LL658: if( !(( unsigned int) _temp681->name)){ return 0;}{ struct
_tuple0 _temp708; struct _tagged_arr* _temp709; struct _tuple0* _temp706=(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp681->name))->v;
_temp708=* _temp706; _LL710: _temp709= _temp708.f2; goto _LL707; _LL707: Cyc_current_source=(
struct _tagged_arr*) _temp709; if(( unsigned int) _temp681->fields){{ struct Cyc_List_List*
_temp711=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp681->fields))->v;
for( 0; _temp711 !=  0; _temp711= _temp711->tl){ struct Cyc_Absyn_Structfield*
_temp712=( struct Cyc_Absyn_Structfield*) _temp711->hd; Cyc_scan_type(( void*)
_temp712->type); Cyc_scan_exp_opt( _temp712->width);}}{ struct Cyc_List_List*
_temp713=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp681->fields))->v;
for( 0; _temp713 !=  0; _temp713= _temp713->tl){;}}} goto _LL652;} _LL660: if( !((
unsigned int) _temp683->name)){ return 0;}{ struct _tuple0 _temp716; struct
_tagged_arr* _temp717; struct _tuple0* _temp714=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp683->name))->v; _temp716=* _temp714; _LL718: _temp717=
_temp716.f2; goto _LL715; _LL715: Cyc_current_source=( struct _tagged_arr*)
_temp717; if(( unsigned int) _temp683->fields){{ struct Cyc_List_List* _temp719=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp683->fields))->v;
for( 0; _temp719 !=  0; _temp719= _temp719->tl){ struct Cyc_Absyn_Structfield*
_temp720=( struct Cyc_Absyn_Structfield*) _temp719->hd; Cyc_add_target( _temp720->name);
Cyc_scan_type(( void*) _temp720->type); Cyc_scan_exp_opt( _temp720->width);}}{
struct Cyc_List_List* _temp721=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp683->fields))->v; for( 0; _temp721 !=  0; _temp721= _temp721->tl){;}}}
goto _LL652;} _LL662: { struct _tuple0 _temp724; struct _tagged_arr* _temp725;
struct _tuple0* _temp722= _temp685->name; _temp724=* _temp722; _LL726: _temp725=
_temp724.f2; goto _LL723; _LL723: Cyc_current_source=( struct _tagged_arr*)
_temp725; if(( unsigned int) _temp685->fields){{ struct Cyc_List_List* _temp727=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp685->fields))->v;
for( 0; _temp727 !=  0; _temp727= _temp727->tl){ struct Cyc_Absyn_Enumfield*
_temp728=( struct Cyc_Absyn_Enumfield*) _temp727->hd; Cyc_scan_exp_opt( _temp728->tag);}}{
struct Cyc_List_List* _temp729=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp685->fields))->v; for( 0; _temp729 !=  0; _temp729= _temp729->tl){;}}}
goto _LL652;} _LL664: { struct _tuple0 _temp732; struct _tagged_arr* _temp733;
struct _tuple0* _temp730= _temp687->name; _temp732=* _temp730; _LL734: _temp733=
_temp732.f2; goto _LL731; _LL731: Cyc_current_source=( struct _tagged_arr*)
_temp733; Cyc_scan_type(( void*) _temp687->defn); goto _LL652;} _LL666:({ void*
_temp735[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp735, sizeof( void*), 0u));}); exit(
1); return 0; _LL668:({ void* _temp736[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected tunion declaration\n", sizeof( unsigned char), 38u),
_tag_arr( _temp736, sizeof( void*), 0u));}); exit( 1); return 0; _LL670:({ void*
_temp737[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp737, sizeof( void*), 0u));}); exit(
1); return 0; _LL672:({ void* _temp738[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected namespace declaration\n", sizeof( unsigned char), 41u),
_tag_arr( _temp738, sizeof( void*), 0u));}); exit( 1); return 0; _LL674:({ void*
_temp739[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected using declaration\n",
sizeof( unsigned char), 37u), _tag_arr( _temp739, sizeof( void*), 0u));}); exit(
1); return 0; _LL676:({ void* _temp740[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n", sizeof( unsigned char),
42u), _tag_arr( _temp740, sizeof( void*), 0u));}); exit( 1); return 0; _LL652:;}
return({ struct _tuple8* _temp741=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp741->f1=( struct _tagged_arr*) _check_null( Cyc_current_source);
_temp741->f2=*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets));
_temp741;});} struct Cyc_Hashtable_Table* Cyc_new_deps(){ return(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), int(* hash)(
struct _tagged_arr*))) Cyc_Hashtable_create)( 107, Cyc_Std_strptrcmp, Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set* Cyc_find( struct Cyc_Hashtable_Table* t, struct _tagged_arr*
x){ struct _handler_cons _temp742; _push_handler(& _temp742);{ int _temp744= 0;
if( setjmp( _temp742.handler)){ _temp744= 1;} if( ! _temp744){{ struct Cyc_Set_Set*
_temp745=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key)) Cyc_Hashtable_lookup)( t, x); _npop_handler( 0u); return
_temp745;}; _pop_handler();} else{ void* _temp743=( void*) _exn_thrown; void*
_temp747= _temp743; _LL749: if( _temp747 ==  Cyc_Core_Not_found){ goto _LL750;}
else{ goto _LL751;} _LL751: goto _LL752; _LL750: return(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_LL752:( void) _throw( _temp747); _LL748:;}}} struct Cyc_Set_Set* Cyc_reachable(
struct Cyc_List_List* init, struct Cyc_Hashtable_Table* t){ struct Cyc_Set_Set*
emptyset=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); struct Cyc_Set_Set* curr;
for( curr= emptyset; init !=  0; init= init->tl){ curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( curr,( struct
_tagged_arr*) init->hd);}{ struct Cyc_Set_Set* _temp753= curr; while((( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp753) >  0) { struct Cyc_Set_Set*
_temp754= emptyset;{ struct Cyc_List_List* _temp755=(( struct Cyc_List_List*(*)(
struct Cyc_Set_Set* s)) Cyc_Set_elements)( _temp753); for( 0; _temp755 !=  0;
_temp755= _temp755->tl){ _temp754=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( _temp754, Cyc_find( t,( struct
_tagged_arr*) _temp755->hd));}} _temp753=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_diff)( _temp754, curr); curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( curr,
_temp753);} return curr;}} int Cyc_force_directory( struct _tagged_arr d){ int
_temp756=({ unsigned int _temp759[ 0u]; Cyc_Std_open( d, 0, _tag_arr( _temp759,
sizeof( unsigned int), 0u));}); if( _temp756 ==  - 1){ if( Cyc_Std_mkdir( d, 448)
==  - 1){({ struct Cyc_Std_String_pa_struct _temp758; _temp758.tag= Cyc_Std_String_pa;
_temp758.f1=( struct _tagged_arr) d;{ void* _temp757[ 1u]={& _temp758}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n", sizeof(
unsigned char), 38u), _tag_arr( _temp757, sizeof( void*), 1u));}}); return 1;}}
else{ close( _temp756);} return 0;} int Cyc_force_directory_prefixes( struct
_tagged_arr file){ struct _tagged_arr _temp760= Cyc_Std_strdup( file); struct
Cyc_List_List* _temp761= 0; while( 1) { _temp760= Cyc_Filename_dirname(( struct
_tagged_arr) _temp760); if( _get_arr_size( _temp760, sizeof( unsigned char)) == 
0){ break;} _temp761=({ struct Cyc_List_List* _temp762=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp762->hd=( void*)({ struct
_tagged_arr* _temp763=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp763[ 0]=( struct _tagged_arr) _temp760; _temp763;});
_temp762->tl= _temp761; _temp762;});} for( 0; _temp761 !=  0; _temp761= _temp761->tl){
if( Cyc_force_directory(*(( struct _tagged_arr*) _temp761->hd))){ return 1;}}
return 0;} int Cyc_process_file( struct _tagged_arr filename, struct _tagged_arr
dir, struct Cyc_List_List* start_symbols, struct Cyc_List_List* omit_symbols,
struct Cyc_List_List* prologue, struct Cyc_List_List* epilogue, struct Cyc_List_List*
cstubs, struct Cyc_List_List* cycstubs){ struct Cyc_Std___sFILE* maybe; struct
Cyc_Std___sFILE* in_file; struct Cyc_Std___sFILE* out_file;({ struct Cyc_Std_String_pa_struct
_temp765; _temp765.tag= Cyc_Std_String_pa; _temp765.f1=( struct _tagged_arr)
filename;{ void* _temp764[ 1u]={& _temp765}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("********************************* %s...\n", sizeof( unsigned char), 41u),
_tag_arr( _temp764, sizeof( void*), 1u));}});{ struct _tagged_arr buf= _tag_arr(({
unsigned int _temp1067=( unsigned int) 1024; unsigned char* _temp1068=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char),
_temp1067));{ unsigned int _temp1069= _temp1067; unsigned int i; for( i= 0; i < 
_temp1069; i ++){ _temp1068[ i]='\000';}}; _temp1068;}), sizeof( unsigned char),(
unsigned int) 1024); struct _tagged_arr _temp766= Cyc_Std_getcwd( buf,
_get_arr_size( buf, sizeof( unsigned char))); if( Cyc_Std_chdir( dir)){({ struct
Cyc_Std_String_pa_struct _temp768; _temp768.tag= Cyc_Std_String_pa; _temp768.f1=(
struct _tagged_arr) dir;{ void* _temp767[ 1u]={& _temp768}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: can't change directory to %s\n", sizeof( unsigned char), 37u),
_tag_arr( _temp767, sizeof( void*), 1u));}}); return 1;}({ struct Cyc_Std_String_pa_struct
_temp770; _temp770.tag= Cyc_Std_String_pa; _temp770.f1=( struct _tagged_arr)
filename;{ void* _temp769[ 1u]={& _temp770}; Cyc_log( _tag_arr("\n%s:\n",
sizeof( unsigned char), 6u), _tag_arr( _temp769, sizeof( void*), 1u));}});{
struct _tagged_arr _temp771= Cyc_Filename_basename( filename); struct
_tagged_arr _temp772= Cyc_Filename_chop_extension(( struct _tagged_arr) _temp771);
struct _tagged_arr _temp773= Cyc_Std_strconcat(( struct _tagged_arr) _temp772,
_tag_arr(".i", sizeof( unsigned char), 3u)); struct _tagged_arr _temp774= Cyc_Std_strconcat((
struct _tagged_arr) _temp772, _tag_arr(".iX", sizeof( unsigned char), 4u));
struct _tagged_arr _temp775= Cyc_Filename_dirname( filename); struct _tagged_arr
_temp776= _get_arr_size( _temp775, sizeof( unsigned char)) ==  0?({ struct Cyc_Std_String_pa_struct
_temp1064; _temp1064.tag= Cyc_Std_String_pa; _temp1064.f1=( struct _tagged_arr)
_temp771;{ void* _temp1063[ 1u]={& _temp1064}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp1063, sizeof( void*), 1u));}}): Cyc_Filename_concat((
struct _tagged_arr) _temp775,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1066; _temp1066.tag= Cyc_Std_String_pa; _temp1066.f1=( struct _tagged_arr)
_temp771;{ void* _temp1065[ 1u]={& _temp1066}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp1065, sizeof( void*), 1u));}})); if(
Cyc_force_directory_prefixes( filename)){ return 1;}{ struct Cyc_Hashtable_Table*
t= Cyc_new_deps(); struct _tagged_arr _temp777=({ struct Cyc_Std_String_pa_struct
_temp1062; _temp1062.tag= Cyc_Std_String_pa; _temp1062.f1=( struct _tagged_arr)
_temp773;{ struct Cyc_Std_String_pa_struct _temp1061; _temp1061.tag= Cyc_Std_String_pa;
_temp1061.f1=( struct _tagged_arr) filename;{ void* _temp1060[ 2u]={& _temp1061,&
_temp1062}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp1060, sizeof( void*), 2u));}}});
if( Cyc_Std_system(( struct _tagged_arr) _temp777) !=  0){ maybe= Cyc_Std_fopen(
filename, _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int)
maybe)){({ struct Cyc_Std_String_pa_struct _temp779; _temp779.tag= Cyc_Std_String_pa;
_temp779.f1=( struct _tagged_arr) filename;{ void* _temp778[ 1u]={& _temp779};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create file %s\n",
sizeof( unsigned char), 33u), _tag_arr( _temp778, sizeof( void*), 1u));}});
return 1;} out_file=( struct Cyc_Std___sFILE*) _check_null( maybe);({ struct Cyc_Std_String_pa_struct
_temp781; _temp781.tag= Cyc_Std_String_pa; _temp781.f1=( struct _tagged_arr)
filename;{ void* _temp780[ 1u]={& _temp781}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp780, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp776, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){({ struct Cyc_Std_String_pa_struct
_temp783; _temp783.tag= Cyc_Std_String_pa; _temp783.f1=( struct _tagged_arr)
_temp776;{ void* _temp782[ 1u]={& _temp783}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n", sizeof( unsigned char), 33u),
_tag_arr( _temp782, sizeof( void*), 1u));}}); return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp785; _temp785.tag=
Cyc_Std_String_pa; _temp785.f1=( struct _tagged_arr) _temp776;{ void* _temp784[
1u]={& _temp785}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp784, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file);({ struct Cyc_Std_String_pa_struct _temp787; _temp787.tag= Cyc_Std_String_pa;
_temp787.f1=( struct _tagged_arr) filename;{ void* _temp786[ 1u]={& _temp787};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: %s will not be supported on this platform\n",
sizeof( unsigned char), 52u), _tag_arr( _temp786, sizeof( void*), 1u));}});({
void* _temp788[ 0u]={}; Cyc_log( _tag_arr("Not supported on this platform\n",
sizeof( unsigned char), 32u), _tag_arr( _temp788, sizeof( void*), 0u));}); if(
Cyc_Std_chdir(( struct _tagged_arr) _temp766)){({ struct Cyc_Std_String_pa_struct
_temp790; _temp790.tag= Cyc_Std_String_pa; _temp790.f1=( struct _tagged_arr)
_temp766;{ void* _temp789[ 1u]={& _temp790}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n", sizeof( unsigned char), 41u),
_tag_arr( _temp789, sizeof( void*), 1u));}}); return 1;} return 0;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp773, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe);{ struct Cyc_Lexing_lexbuf* _temp791= Cyc_Lexing_from_file( in_file);
struct _tuple8* entry; while(( entry=(( struct _tuple8*(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_line)( _temp791)) !=  0) { struct _tuple8 _temp794; struct Cyc_Set_Set*
_temp795; struct _tagged_arr* _temp797; struct _tuple8* _temp792=( struct
_tuple8*) _check_null( entry); _temp794=* _temp792; _LL798: _temp797= _temp794.f1;
goto _LL796; _LL796: _temp795= _temp794.f2; goto _LL793; _LL793:(( void(*)(
struct Cyc_Hashtable_Table* t, struct _tagged_arr* key, struct Cyc_Set_Set* val))
Cyc_Hashtable_insert)( t, _temp797, _temp795);} Cyc_Std_fclose( in_file);
_temp777=({ struct Cyc_Std_String_pa_struct _temp801; _temp801.tag= Cyc_Std_String_pa;
_temp801.f1=( struct _tagged_arr) _temp773;{ struct Cyc_Std_String_pa_struct
_temp800; _temp800.tag= Cyc_Std_String_pa; _temp800.f1=( struct _tagged_arr)
filename;{ void* _temp799[ 2u]={& _temp800,& _temp801}; Cyc_Std_aprintf(
_tag_arr("echo '#include <%s>' | gcc -E -o %s -", sizeof( unsigned char), 38u),
_tag_arr( _temp799, sizeof( void*), 2u));}}}); if( Cyc_Std_system(( struct
_tagged_arr) _temp777) !=  0){ return 1;} maybe= Cyc_Std_fopen(( struct
_tagged_arr) _temp773, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp791= Cyc_Lexing_from_file( in_file); Cyc_slurp_out= Cyc_Std_fopen((
struct _tagged_arr) _temp774, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) Cyc_slurp_out)){ return 1;} if((( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_slurp)( _temp791)){({ struct Cyc_Std_String_pa_struct _temp803;
_temp803.tag= Cyc_Std_String_pa; _temp803.f1=( struct _tagged_arr) filename;{
void* _temp802[ 1u]={& _temp803}; Cyc_log( _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp802, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp805; _temp805.tag= Cyc_Std_String_pa;
_temp805.f1=( struct _tagged_arr) filename;{ void* _temp804[ 1u]={& _temp805};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp804, sizeof( void*), 1u));}}); Cyc_Std_fclose(
in_file); Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
Cyc_Std_remove(( struct _tagged_arr) _temp773); Cyc_Std_remove(( struct
_tagged_arr) _temp774); return 1;} Cyc_Std_fclose( in_file); Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp774, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); Cyc_Position_reset_position(( struct _tagged_arr) _temp774); Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp806= Cyc_Parse_parse_file( in_file); Cyc_Lex_lex_init();
Cyc_Std_fclose( in_file);{ struct Cyc_List_List* _temp807= _temp806; for( 0;
_temp807 !=  0; _temp807= _temp807->tl){ struct _tuple8* _temp808= Cyc_scan_decl((
struct Cyc_Absyn_Decl*) _temp807->hd); if( _temp808 ==  0){ continue;}{ struct
_tuple8 _temp811; struct Cyc_Set_Set* _temp812; struct _tagged_arr* _temp814;
struct _tuple8* _temp809=( struct _tuple8*) _check_null( _temp808); _temp811=*
_temp809; _LL815: _temp814= _temp811.f1; goto _LL813; _LL813: _temp812= _temp811.f2;
goto _LL810; _LL810: { struct Cyc_Set_Set* old;{ struct _handler_cons _temp816;
_push_handler(& _temp816);{ int _temp818= 0; if( setjmp( _temp816.handler)){
_temp818= 1;} if( ! _temp818){ old=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_arr* key)) Cyc_Hashtable_lookup)( t, _temp814);; _pop_handler();}
else{ void* _temp817=( void*) _exn_thrown; void* _temp820= _temp817; _LL822: if(
_temp820 ==  Cyc_Core_Not_found){ goto _LL823;} else{ goto _LL824;} _LL824: goto
_LL825; _LL823: old=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); goto _LL821; _LL825:(
void) _throw( _temp820); _LL821:;}}}(( void(*)( struct Cyc_Hashtable_Table* t,
struct _tagged_arr* key, struct Cyc_Set_Set* val)) Cyc_Hashtable_insert)( t,
_temp814,(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( _temp812, old));}}}}{ struct Cyc_Set_Set* _temp826= Cyc_reachable(
start_symbols, t); struct Cyc_List_List* _temp827= 0; struct Cyc_Set_Set*
defined_symbols=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);{ struct Cyc_List_List*
_temp828= _temp806; for( 0; _temp828 !=  0; _temp828= _temp828->tl){ struct Cyc_Absyn_Decl*
_temp829=( struct Cyc_Absyn_Decl*) _temp828->hd; struct _tagged_arr* name;{ void*
_temp830=( void*) _temp829->r; struct Cyc_Absyn_Vardecl* _temp856; struct Cyc_Absyn_Fndecl*
_temp858; struct Cyc_Absyn_Structdecl* _temp860; struct Cyc_Absyn_Uniondecl*
_temp862; struct Cyc_Absyn_Enumdecl* _temp864; struct Cyc_Absyn_Typedefdecl*
_temp866; _LL832: if(*(( int*) _temp830) ==  Cyc_Absyn_Var_d){ _LL857: _temp856=((
struct Cyc_Absyn_Var_d_struct*) _temp830)->f1; goto _LL833;} else{ goto _LL834;}
_LL834: if(*(( int*) _temp830) ==  Cyc_Absyn_Fn_d){ _LL859: _temp858=(( struct
Cyc_Absyn_Fn_d_struct*) _temp830)->f1; goto _LL835;} else{ goto _LL836;} _LL836:
if(*(( int*) _temp830) ==  Cyc_Absyn_Struct_d){ _LL861: _temp860=(( struct Cyc_Absyn_Struct_d_struct*)
_temp830)->f1; goto _LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp830)
==  Cyc_Absyn_Union_d){ _LL863: _temp862=(( struct Cyc_Absyn_Union_d_struct*)
_temp830)->f1; goto _LL839;} else{ goto _LL840;} _LL840: if(*(( int*) _temp830)
==  Cyc_Absyn_Enum_d){ _LL865: _temp864=(( struct Cyc_Absyn_Enum_d_struct*)
_temp830)->f1; goto _LL841;} else{ goto _LL842;} _LL842: if(*(( int*) _temp830)
==  Cyc_Absyn_Typedef_d){ _LL867: _temp866=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp830)->f1; goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp830)
==  Cyc_Absyn_Let_d){ goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*)
_temp830) ==  Cyc_Absyn_Tunion_d){ goto _LL847;} else{ goto _LL848;} _LL848: if(*((
int*) _temp830) ==  Cyc_Absyn_Letv_d){ goto _LL849;} else{ goto _LL850;} _LL850:
if(*(( int*) _temp830) ==  Cyc_Absyn_Namespace_d){ goto _LL851;} else{ goto
_LL852;} _LL852: if(*(( int*) _temp830) ==  Cyc_Absyn_Using_d){ goto _LL853;}
else{ goto _LL854;} _LL854: if(*(( int*) _temp830) ==  Cyc_Absyn_ExternC_d){
goto _LL855;} else{ goto _LL831;} _LL833: { struct _tuple0 _temp870; struct
_tagged_arr* _temp871; struct _tuple0* _temp868= _temp856->name; _temp870=*
_temp868; _LL872: _temp871= _temp870.f2; goto _LL869; _LL869: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp871); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp871)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp871;} goto _LL831;} _LL835: { struct _tuple0 _temp875; struct
_tagged_arr* _temp876; struct _tuple0* _temp873= _temp858->name; _temp875=*
_temp873; _LL877: _temp876= _temp875.f2; goto _LL874; _LL874: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp876); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp876)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp876;} goto _LL831;} _LL837: if( !(( unsigned int) _temp860->name)){
name= 0;}{ struct _tuple0 _temp880; struct _tagged_arr* _temp881; struct _tuple0*
_temp878=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp860->name))->v;
_temp880=* _temp878; _LL882: _temp881= _temp880.f2; goto _LL879; _LL879: name=(
struct _tagged_arr*) _temp881; goto _LL831;} _LL839: if( !(( unsigned int)
_temp862->name)){ name= 0;}{ struct _tuple0 _temp885; struct _tagged_arr*
_temp886; struct _tuple0* _temp883=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp862->name))->v; _temp885=* _temp883; _LL887: _temp886=
_temp885.f2; goto _LL884; _LL884: name=( struct _tagged_arr*) _temp886; goto
_LL831;} _LL841: { struct _tuple0 _temp890; struct _tagged_arr* _temp891; struct
_tuple0* _temp888= _temp864->name; _temp890=* _temp888; _LL892: _temp891=
_temp890.f2; goto _LL889; _LL889: name=( struct _tagged_arr*) _temp891; if( name
!=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp826,( struct _tagged_arr*) _check_null( name)): 0){ _temp827=({ struct Cyc_List_List*
_temp893=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp893->hd=( void*) _temp829; _temp893->tl= _temp827; _temp893;});} else{ if((
unsigned int) _temp864->fields){ struct Cyc_List_List* _temp894=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp864->fields))->v; for( 0; _temp894 !=  0;
_temp894= _temp894->tl){ struct Cyc_Absyn_Enumfield* _temp895=( struct Cyc_Absyn_Enumfield*)
_temp894->hd; struct _tuple0 _temp898; struct _tagged_arr* _temp899; struct
_tuple0* _temp896= _temp895->name; _temp898=* _temp896; _LL900: _temp899=
_temp898.f2; goto _LL897; _LL897: if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( _temp826, _temp899)){ _temp827=({ struct Cyc_List_List*
_temp901=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp901->hd=( void*) _temp829; _temp901->tl= _temp827; _temp901;}); break;}}}}
name= 0; goto _LL831;} _LL843: { struct _tuple0 _temp904; struct _tagged_arr*
_temp905; struct _tuple0* _temp902= _temp866->name; _temp904=* _temp902; _LL906:
_temp905= _temp904.f2; goto _LL903; _LL903: name=( struct _tagged_arr*) _temp905;
goto _LL831;} _LL845: goto _LL847; _LL847: goto _LL849; _LL849: goto _LL851;
_LL851: goto _LL853; _LL853: goto _LL855; _LL855: name= 0; goto _LL831; _LL831:;}
if( name !=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp826,( struct _tagged_arr*) _check_null( name)): 0){ _temp827=({ struct Cyc_List_List*
_temp907=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp907->hd=( void*) _temp829; _temp907->tl= _temp827; _temp907;});}}} _temp827=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp827);
maybe= Cyc_Std_fopen( filename, _tag_arr("w", sizeof( unsigned char), 2u)); if(
!(( unsigned int) maybe)){ return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp909; _temp909.tag=
Cyc_Std_String_pa; _temp909.f1=( struct _tagged_arr) _temp776;{ void* _temp908[
1u]={& _temp909}; Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\nusing Std;\n",
sizeof( unsigned char), 26u), _tag_arr( _temp908, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp776, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} out_file=(
struct Cyc_Std___sFILE*) _check_null( maybe);{ struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _temp1059; _temp1059.tag= Cyc_Std_String_pa;
_temp1059.f1=( struct _tagged_arr) filename;{ void* _temp1058[ 1u]={& _temp1059};
Cyc_Std_aprintf( _tag_arr("_%s_", sizeof( unsigned char), 5u), _tag_arr(
_temp1058, sizeof( void*), 1u));}});{ int _temp910= 0; for( 0; _temp910 < 
_get_arr_size( ifdefmacro, sizeof( unsigned char)); _temp910 ++){ if(*((
unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof( unsigned char),
_temp910)) == '.'? 1:*(( unsigned char*) _check_unknown_subscript( ifdefmacro,
sizeof( unsigned char), _temp910)) == '/'){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp910))='_';}
else{ if(*(( unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof(
unsigned char), _temp910)) != '_'?*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp910)) != '/': 0){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp910))=(
unsigned char) toupper(( int)*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp910)));}}}}({ struct Cyc_Std_String_pa_struct
_temp913; _temp913.tag= Cyc_Std_String_pa; _temp913.f1=( struct _tagged_arr)
ifdefmacro;{ struct Cyc_Std_String_pa_struct _temp912; _temp912.tag= Cyc_Std_String_pa;
_temp912.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp911[ 2u]={& _temp912,&
_temp913}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n#define %s\n",
sizeof( unsigned char), 23u), _tag_arr( _temp911, sizeof( void*), 2u));}}}); if(
prologue !=  0){ struct Cyc_List_List* _temp914= prologue; for( 0; _temp914 != 
0; _temp914= _temp914->tl){ struct _tuple10 _temp917; struct _tagged_arr
_temp918; struct _tagged_arr _temp920; struct _tuple10* _temp915=( struct
_tuple10*) _temp914->hd; _temp917=* _temp915; _LL921: _temp920= _temp917.f1;
goto _LL919; _LL919: _temp918= _temp917.f2; goto _LL916; _LL916: if( _temp918.curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp920.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp922=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp922[ 0]= _temp920; _temp922;})): 0){ Cyc_Std_fputs( _temp918,
out_file);} else{({ struct Cyc_Std_String_pa_struct _temp924; _temp924.tag= Cyc_Std_String_pa;
_temp924.f1=( struct _tagged_arr) _temp920;{ void* _temp923[ 1u]={& _temp924};
Cyc_log( _tag_arr("%s is not supported on this platform\n", sizeof(
unsigned char), 38u), _tag_arr( _temp923, sizeof( void*), 1u));}});}}}({ void*
_temp925[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("\nnamespace Std {\n",
sizeof( unsigned char), 18u), _tag_arr( _temp925, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp926= _temp827; for( 0; _temp926 !=  0; _temp926=
_temp926->tl){ struct Cyc_Absyn_Decl* _temp927=( struct Cyc_Absyn_Decl*)
_temp926->hd; int _temp928= 0; struct _tagged_arr* name;{ void* _temp929=( void*)
_temp927->r; struct Cyc_Absyn_Vardecl* _temp955; struct Cyc_Absyn_Fndecl*
_temp957; struct Cyc_Absyn_Structdecl* _temp959; struct Cyc_Absyn_Uniondecl*
_temp961; struct Cyc_Absyn_Enumdecl* _temp963; struct Cyc_Absyn_Typedefdecl*
_temp965; _LL931: if(*(( int*) _temp929) ==  Cyc_Absyn_Var_d){ _LL956: _temp955=((
struct Cyc_Absyn_Var_d_struct*) _temp929)->f1; goto _LL932;} else{ goto _LL933;}
_LL933: if(*(( int*) _temp929) ==  Cyc_Absyn_Fn_d){ _LL958: _temp957=(( struct
Cyc_Absyn_Fn_d_struct*) _temp929)->f1; goto _LL934;} else{ goto _LL935;} _LL935:
if(*(( int*) _temp929) ==  Cyc_Absyn_Struct_d){ _LL960: _temp959=(( struct Cyc_Absyn_Struct_d_struct*)
_temp929)->f1; goto _LL936;} else{ goto _LL937;} _LL937: if(*(( int*) _temp929)
==  Cyc_Absyn_Union_d){ _LL962: _temp961=(( struct Cyc_Absyn_Union_d_struct*)
_temp929)->f1; goto _LL938;} else{ goto _LL939;} _LL939: if(*(( int*) _temp929)
==  Cyc_Absyn_Enum_d){ _LL964: _temp963=(( struct Cyc_Absyn_Enum_d_struct*)
_temp929)->f1; goto _LL940;} else{ goto _LL941;} _LL941: if(*(( int*) _temp929)
==  Cyc_Absyn_Typedef_d){ _LL966: _temp965=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp929)->f1; goto _LL942;} else{ goto _LL943;} _LL943: if(*(( int*) _temp929)
==  Cyc_Absyn_Let_d){ goto _LL944;} else{ goto _LL945;} _LL945: if(*(( int*)
_temp929) ==  Cyc_Absyn_Tunion_d){ goto _LL946;} else{ goto _LL947;} _LL947: if(*((
int*) _temp929) ==  Cyc_Absyn_Letv_d){ goto _LL948;} else{ goto _LL949;} _LL949:
if(*(( int*) _temp929) ==  Cyc_Absyn_Namespace_d){ goto _LL950;} else{ goto
_LL951;} _LL951: if(*(( int*) _temp929) ==  Cyc_Absyn_Using_d){ goto _LL952;}
else{ goto _LL953;} _LL953: if(*(( int*) _temp929) ==  Cyc_Absyn_ExternC_d){
goto _LL954;} else{ goto _LL930;} _LL932: { struct _tuple0 _temp969; struct
_tagged_arr* _temp970; struct _tuple0* _temp967= _temp955->name; _temp969=*
_temp967; _LL971: _temp970= _temp969.f2; goto _LL968; _LL968: name=( struct
_tagged_arr*) _temp970; goto _LL930;} _LL934: { struct _tuple0 _temp974; struct
_tagged_arr* _temp975; struct _tuple0* _temp972= _temp957->name; _temp974=*
_temp972; _LL976: _temp975= _temp974.f2; goto _LL973; _LL973: name=( struct
_tagged_arr*) _temp975; goto _LL930;} _LL936: if( !(( unsigned int) _temp959->name)){
name= 0;}{ struct _tuple0 _temp979; struct _tagged_arr* _temp980; struct _tuple0*
_temp977=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp959->name))->v;
_temp979=* _temp977; _LL981: _temp980= _temp979.f2; goto _LL978; _LL978: name=(
struct _tagged_arr*) _temp980; goto _LL930;} _LL938: if( !(( unsigned int)
_temp961->name)){ name= 0;}{ struct _tuple0 _temp984; struct _tagged_arr*
_temp985; struct _tuple0* _temp982=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp961->name))->v; _temp984=* _temp982; _LL986: _temp985=
_temp984.f2; goto _LL983; _LL983: name=( struct _tagged_arr*) _temp985; goto
_LL930;} _LL940: { struct _tuple0 _temp989; struct _tagged_arr* _temp990; struct
_tuple0* _temp987= _temp963->name; _temp989=* _temp987; _LL991: _temp990=
_temp989.f2; goto _LL988; _LL988: name=( struct _tagged_arr*) _temp990; goto
_LL930;} _LL942: { struct _tuple0 _temp994; struct _tagged_arr* _temp995; struct
_tuple0* _temp992= _temp965->name; _temp994=* _temp992; _LL996: _temp995=
_temp994.f2; goto _LL993; _LL993: name=( struct _tagged_arr*) _temp995; goto
_LL930;} _LL944: goto _LL946; _LL946: goto _LL948; _LL948: goto _LL950; _LL950:
goto _LL952; _LL952: goto _LL954; _LL954: name= 0; goto _LL930; _LL930:;} if( !((
unsigned int) name)? ! _temp928: 0){ continue;} if(( unsigned int) name){
ifdefmacro=({ struct Cyc_Std_String_pa_struct _temp998; _temp998.tag= Cyc_Std_String_pa;
_temp998.f1=( struct _tagged_arr)* name;{ void* _temp997[ 1u]={& _temp998}; Cyc_Std_aprintf(
_tag_arr("_%s_def_", sizeof( unsigned char), 9u), _tag_arr( _temp997, sizeof(
void*), 1u));}});({ struct Cyc_Std_String_pa_struct _temp1000; _temp1000.tag=
Cyc_Std_String_pa; _temp1000.f1=( struct _tagged_arr) ifdefmacro;{ void*
_temp999[ 1u]={& _temp1000}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n",
sizeof( unsigned char), 12u), _tag_arr( _temp999, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1002; _temp1002.tag= Cyc_Std_String_pa;
_temp1002.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp1001[ 1u]={&
_temp1002}; Cyc_Std_fprintf( out_file, _tag_arr("#define %s\n", sizeof(
unsigned char), 12u), _tag_arr( _temp1001, sizeof( void*), 1u));}}); Cyc_Absynpp_decllist2file(({
struct Cyc_Absyn_Decl* _temp1003[ 1u]; _temp1003[ 0u]= _temp927;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1003, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);({ void* _temp1004[ 0u]={}; Cyc_Std_fprintf( out_file,
_tag_arr("#endif\n", sizeof( unsigned char), 8u), _tag_arr( _temp1004, sizeof(
void*), 0u));});} else{ Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp1005[ 1u]; _temp1005[ 0u]= _temp927;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1005, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);}}} _temp777=({ struct Cyc_Std_String_pa_struct _temp1008;
_temp1008.tag= Cyc_Std_String_pa; _temp1008.f1=( struct _tagged_arr) _temp773;{
struct Cyc_Std_String_pa_struct _temp1007; _temp1007.tag= Cyc_Std_String_pa;
_temp1007.f1=( struct _tagged_arr) filename;{ void* _temp1006[ 2u]={& _temp1007,&
_temp1008}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp1006, sizeof( void*), 2u));}}});
if( Cyc_Std_system(( struct _tagged_arr) _temp777) !=  0){ return 1;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp773, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp791= Cyc_Lexing_from_file( in_file);{ struct _tuple9* entry2;
while(( entry2=(( struct _tuple9*(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_suck_line)(
_temp791)) !=  0) { struct _tuple9 _temp1011; struct _tagged_arr* _temp1012;
struct _tagged_arr _temp1014; struct _tuple9* _temp1009=( struct _tuple9*)
_check_null( entry2); _temp1011=* _temp1009; _LL1015: _temp1014= _temp1011.f1;
goto _LL1013; _LL1013: _temp1012= _temp1011.f2; goto _LL1010; _LL1010: if((( int(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( _temp826,
_temp1012)){({ struct Cyc_Std_String_pa_struct _temp1017; _temp1017.tag= Cyc_Std_String_pa;
_temp1017.f1=( struct _tagged_arr)* _temp1012;{ void* _temp1016[ 1u]={&
_temp1017}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof(
unsigned char), 12u), _tag_arr( _temp1016, sizeof( void*), 1u));}});({ struct
Cyc_Std_String_pa_struct _temp1019; _temp1019.tag= Cyc_Std_String_pa; _temp1019.f1=(
struct _tagged_arr) _temp1014;{ void* _temp1018[ 1u]={& _temp1019}; Cyc_Std_fprintf(
out_file, _tag_arr("%s\n", sizeof( unsigned char), 4u), _tag_arr( _temp1018,
sizeof( void*), 1u));}});({ void* _temp1020[ 0u]={}; Cyc_Std_fprintf( out_file,
_tag_arr("#endif\n", sizeof( unsigned char), 8u), _tag_arr( _temp1020, sizeof(
void*), 0u));});}} Cyc_Std_fclose( in_file); Cyc_Std_remove(( struct _tagged_arr)
_temp773); if( epilogue !=  0){ struct Cyc_List_List* _temp1021= epilogue; for(
0; _temp1021 !=  0; _temp1021= _temp1021->tl){ struct _tuple10 _temp1024; struct
_tagged_arr _temp1025; struct _tagged_arr _temp1027; struct _tuple10* _temp1022=(
struct _tuple10*) _temp1021->hd; _temp1024=* _temp1022; _LL1028: _temp1027=
_temp1024.f1; goto _LL1026; _LL1026: _temp1025= _temp1024.f2; goto _LL1023;
_LL1023: if( _temp1025.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?
_temp1027.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
defined_symbols,({ struct _tagged_arr* _temp1029=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp1029[ 0]= _temp1027; _temp1029;})):
0){ Cyc_Std_fputs( _temp1025, out_file);} else{({ struct Cyc_Std_String_pa_struct
_temp1031; _temp1031.tag= Cyc_Std_String_pa; _temp1031.f1=( struct _tagged_arr)
_temp1027;{ void* _temp1030[ 1u]={& _temp1031}; Cyc_log( _tag_arr("%s is not supported on this platform\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1030, sizeof( void*), 1u));}});}}}({
void* _temp1032[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n", sizeof(
unsigned char), 3u), _tag_arr( _temp1032, sizeof( void*), 0u));});({ void*
_temp1033[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp1033, sizeof( void*), 0u));}); Cyc_Std_fclose(
out_file); if( cstubs !=  0){ out_file=( struct Cyc_Std___sFILE*) _check_null(
Cyc_cstubs_file);{ struct Cyc_List_List* _temp1034= cstubs; for( 0; _temp1034 != 
0; _temp1034= _temp1034->tl){ struct _tuple10 _temp1037; struct _tagged_arr
_temp1038; struct _tagged_arr _temp1040; struct _tuple10* _temp1035=( struct
_tuple10*) _temp1034->hd; _temp1037=* _temp1035; _LL1041: _temp1040= _temp1037.f1;
goto _LL1039; _LL1039: _temp1038= _temp1037.f2; goto _LL1036; _LL1036: if(
_temp1038.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1040.curr
== (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp1042=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1042[ 0]= _temp1040; _temp1042;})): 0){ Cyc_Std_fputs(
_temp1038, out_file);}}}} out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _temp1044; _temp1044.tag= Cyc_Std_String_pa;
_temp1044.f1=( struct _tagged_arr) _temp776;{ void* _temp1043[ 1u]={& _temp1044};
Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\n\n", sizeof( unsigned char),
16u), _tag_arr( _temp1043, sizeof( void*), 1u));}}); if( cycstubs !=  0){
out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({ void*
_temp1045[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("namespace Std {\n",
sizeof( unsigned char), 17u), _tag_arr( _temp1045, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp1046= cycstubs; for( 0; _temp1046 !=  0; _temp1046=
_temp1046->tl){ struct _tuple10 _temp1049; struct _tagged_arr _temp1050; struct
_tagged_arr _temp1052; struct _tuple10* _temp1047=( struct _tuple10*) _temp1046->hd;
_temp1049=* _temp1047; _LL1053: _temp1052= _temp1049.f1; goto _LL1051; _LL1051:
_temp1050= _temp1049.f2; goto _LL1048; _LL1048: if( _temp1050.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1052.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct _tagged_arr*
_temp1054=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp1054[ 0]= _temp1052; _temp1054;})): 0){ Cyc_Std_fputs( _temp1050, out_file);}}}({
void* _temp1055[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n\n", sizeof(
unsigned char), 4u), _tag_arr( _temp1055, sizeof( void*), 0u));});} if( Cyc_Std_chdir((
struct _tagged_arr) _temp766)){({ struct Cyc_Std_String_pa_struct _temp1057;
_temp1057.tag= Cyc_Std_String_pa; _temp1057.f1=( struct _tagged_arr) _temp766;{
void* _temp1056[ 1u]={& _temp1057}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not change directory to %s\n",
sizeof( unsigned char), 41u), _tag_arr( _temp1056, sizeof( void*), 1u));}});
return 1;} return 0;}}}}}}}}} int Cyc_process_specfile( struct _tagged_arr file,
struct _tagged_arr dir){ struct Cyc_Std___sFILE* _temp1070= Cyc_Std_fopen( file,
_tag_arr("r", sizeof( unsigned char), 2u)); if( !(( unsigned int) _temp1070)){
return 1;}{ struct Cyc_Std___sFILE* _temp1071=( struct Cyc_Std___sFILE*)
_check_null( _temp1070); struct Cyc_Lexing_lexbuf* _temp1072= Cyc_Lexing_from_file(
_temp1071); struct _tuple11* entry; while(( entry=(( struct _tuple11*(*)( struct
Cyc_Lexing_lexbuf* lexbuf)) Cyc_spec)( _temp1072)) !=  0) { struct _tuple11
_temp1075; struct Cyc_List_List* _temp1076; struct Cyc_List_List* _temp1078;
struct Cyc_List_List* _temp1080; struct Cyc_List_List* _temp1082; struct Cyc_List_List*
_temp1084; struct Cyc_List_List* _temp1086; struct _tagged_arr _temp1088; struct
_tuple11* _temp1073=( struct _tuple11*) _check_null( entry); _temp1075=*
_temp1073; _LL1089: _temp1088= _temp1075.f1; goto _LL1087; _LL1087: _temp1086=
_temp1075.f2; goto _LL1085; _LL1085: _temp1084= _temp1075.f3; goto _LL1083;
_LL1083: _temp1082= _temp1075.f4; goto _LL1081; _LL1081: _temp1080= _temp1075.f5;
goto _LL1079; _LL1079: _temp1078= _temp1075.f6; goto _LL1077; _LL1077: _temp1076=
_temp1075.f7; goto _LL1074; _LL1074: if( Cyc_process_file( _temp1088, dir,
_temp1086, _temp1084, _temp1082, _temp1080, _temp1078, _temp1076)){ Cyc_Std_fclose(
_temp1071); return 1;}} Cyc_Std_fclose( _temp1071); return 0;}} int Cyc_getsize(
struct _tagged_arr dir, struct _tagged_arr includes, struct _tagged_arr type){
struct _tagged_arr _temp1090= Cyc_Filename_concat( dir, _tag_arr("getsize.c",
sizeof( unsigned char), 10u)); struct _tagged_arr _temp1091= Cyc_Filename_concat(
dir, _tag_arr("getsize", sizeof( unsigned char), 8u)); struct _tagged_arr
_temp1092= Cyc_Filename_concat( dir, _tag_arr("getsize.out", sizeof(
unsigned char), 12u)); struct Cyc_Std___sFILE* _temp1093= Cyc_Std_fopen(( struct
_tagged_arr) _temp1090, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) _temp1093)){({ struct Cyc_Std_String_pa_struct _temp1095;
_temp1095.tag= Cyc_Std_String_pa; _temp1095.f1=( struct _tagged_arr) _temp1090;{
void* _temp1094[ 1u]={& _temp1095}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create %s\n",
sizeof( unsigned char), 28u), _tag_arr( _temp1094, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1097; _temp1097.tag= Cyc_Std_String_pa;
_temp1097.f1=( struct _tagged_arr) type;{ void* _temp1096[ 1u]={& _temp1097};
Cyc_log( _tag_arr("Warning: could not find the size of %s\n", sizeof(
unsigned char), 40u), _tag_arr( _temp1096, sizeof( void*), 1u));}}); return - 1;}({
struct Cyc_Std_String_pa_struct _temp1102; _temp1102.tag= Cyc_Std_String_pa;
_temp1102.f1=( struct _tagged_arr) type;{ struct Cyc_Std_String_pa_struct
_temp1101; _temp1101.tag= Cyc_Std_String_pa; _temp1101.f1=( struct _tagged_arr)
type;{ struct Cyc_Std_String_pa_struct _temp1100; _temp1100.tag= Cyc_Std_String_pa;
_temp1100.f1=( struct _tagged_arr) _temp1092;{ struct Cyc_Std_String_pa_struct
_temp1099; _temp1099.tag= Cyc_Std_String_pa; _temp1099.f1=( struct _tagged_arr)
includes;{ void* _temp1098[ 4u]={& _temp1099,& _temp1100,& _temp1101,& _temp1102};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( _temp1093), _tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof( unsigned char), 139u), _tag_arr( _temp1098, sizeof( void*), 4u));}}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1093));{ struct
_tagged_arr _temp1103=({ struct Cyc_Std_String_pa_struct _temp1123; _temp1123.tag=
Cyc_Std_String_pa; _temp1123.f1=( struct _tagged_arr) _temp1090;{ struct Cyc_Std_String_pa_struct
_temp1122; _temp1122.tag= Cyc_Std_String_pa; _temp1122.f1=( struct _tagged_arr)
_temp1091;{ void* _temp1121[ 2u]={& _temp1122,& _temp1123}; Cyc_Std_aprintf(
_tag_arr("gcc -o %s %s", sizeof( unsigned char), 13u), _tag_arr( _temp1121,
sizeof( void*), 2u));}}}); if( Cyc_Std_system(( struct _tagged_arr) _temp1103)
!=  0){({ struct Cyc_Std_String_pa_struct _temp1105; _temp1105.tag= Cyc_Std_String_pa;
_temp1105.f1=( struct _tagged_arr) type;{ void* _temp1104[ 1u]={& _temp1105};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof( unsigned char), 59u), _tag_arr( _temp1104, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1107; _temp1107.tag= Cyc_Std_String_pa;
_temp1107.f1=( struct _tagged_arr) type;{ void* _temp1106[ 1u]={& _temp1107};
Cyc_log( _tag_arr("Warning: could not find the size of %s\n", sizeof(
unsigned char), 40u), _tag_arr( _temp1106, sizeof( void*), 1u));}}); return - 1;}{
int _temp1108= Cyc_Std_system(( struct _tagged_arr) _temp1091);({ struct Cyc_Std_Int_pa_struct
_temp1111; _temp1111.tag= Cyc_Std_Int_pa; _temp1111.f1=( int)(( unsigned int)
_temp1108);{ struct Cyc_Std_String_pa_struct _temp1110; _temp1110.tag= Cyc_Std_String_pa;
_temp1110.f1=( struct _tagged_arr) type;{ void* _temp1109[ 2u]={& _temp1110,&
_temp1111}; Cyc_log( _tag_arr("size of %s returned by system is %d\n", sizeof(
unsigned char), 37u), _tag_arr( _temp1109, sizeof( void*), 2u));}}});{ struct
Cyc_Std___sFILE* _temp1112= Cyc_Std_fopen(( struct _tagged_arr) _temp1092,
_tag_arr("r", sizeof( unsigned char), 2u)); int w= - 1; if(({ struct Cyc_Std_IntPtr_sa_struct
_temp1114; _temp1114.tag= Cyc_Std_IntPtr_sa; _temp1114.f1=& w;{ void* _temp1113[
1u]={& _temp1114}; Cyc_Std_fscanf(( struct Cyc_Std___sFILE*) _check_null(
_temp1112), _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1113,
sizeof( void*), 1u));}}) !=  1){({ struct Cyc_Std_String_pa_struct _temp1116;
_temp1116.tag= Cyc_Std_String_pa; _temp1116.f1=( struct _tagged_arr) type;{ void*
_temp1115[ 1u]={& _temp1116}; Cyc_log( _tag_arr("Warning: could not find the size of %s\n",
sizeof( unsigned char), 40u), _tag_arr( _temp1115, sizeof( void*), 1u));}});
return - 1;}({ struct Cyc_Std_Int_pa_struct _temp1120; _temp1120.tag= Cyc_Std_Int_pa;
_temp1120.f1=( int)(( unsigned int) w);{ struct Cyc_Std_String_pa_struct
_temp1119; _temp1119.tag= Cyc_Std_String_pa; _temp1119.f1=( struct _tagged_arr)
_temp1092;{ struct Cyc_Std_String_pa_struct _temp1118; _temp1118.tag= Cyc_Std_String_pa;
_temp1118.f1=( struct _tagged_arr) type;{ void* _temp1117[ 3u]={& _temp1118,&
_temp1119,& _temp1120}; Cyc_log( _tag_arr("size of %s read from file %s is %d\n",
sizeof( unsigned char), 36u), _tag_arr( _temp1117, sizeof( void*), 3u));}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1112)); Cyc_Std_remove((
struct _tagged_arr) _temp1092); Cyc_Std_remove(( struct _tagged_arr) _temp1090);
Cyc_Std_remove(( struct _tagged_arr) _temp1091); return w;}}}} int Cyc_main( int
argc, struct _tagged_arr argv){ if( argc <  3){({ void* _temp1124[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof( unsigned char), 105u), _tag_arr( _temp1124, sizeof( void*), 0u));});
return 1;}{ struct _tagged_arr _temp1125=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 1)); if( Cyc_force_directory_prefixes((
struct _tagged_arr) _temp1125)? 1: Cyc_force_directory(( struct _tagged_arr)
_temp1125)){({ struct Cyc_Std_String_pa_struct _temp1127; _temp1127.tag= Cyc_Std_String_pa;
_temp1127.f1=( struct _tagged_arr) _temp1125;{ void* _temp1126[ 1u]={& _temp1127};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1126, sizeof( void*), 1u));}});
return 1;} Cyc_log_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1125, _tag_arr("BUILDLIB.LOG", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _temp1129; _temp1129.tag= Cyc_Std_String_pa;
_temp1129.f1=( struct _tagged_arr) _temp1125;{ void* _temp1128[ 1u]={& _temp1129};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create log file in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1128, sizeof( void*), 1u));}});
return 1;} Cyc_cstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1125, _tag_arr("cstubs.c", sizeof( unsigned char), 9u)),
_tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1131; _temp1131.tag= Cyc_Std_String_pa;
_temp1131.f1=( struct _tagged_arr) _temp1125;{ void* _temp1130[ 1u]={& _temp1131};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1130, sizeof( void*), 1u));}});
return 1;} Cyc_cycstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1125, _tag_arr("cycstubs.cyc", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1133; _temp1133.tag= Cyc_Std_String_pa;
_temp1133.f1=( struct _tagged_arr) _temp1125;{ void* _temp1132[ 1u]={& _temp1133};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof( unsigned char), 52u), _tag_arr( _temp1132, sizeof( void*), 1u));}});
return 1;}({ void* _temp1134[ 0u]={}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file), _tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1134, sizeof( void*), 0u));}); Cyc_sizeof_unsignedlongint=
Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned long int", sizeof( unsigned char), 18u)); Cyc_sizeof_unsignedshortint=
Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned short int", sizeof( unsigned char), 19u)); Cyc_sizeof_shortint=
Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("short int", sizeof( unsigned char), 10u)); Cyc_sizeof_int= Cyc_getsize((
struct _tagged_arr) _temp1125, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("int", sizeof( unsigned char), 4u)); Cyc_sizeof_short= Cyc_getsize((
struct _tagged_arr) _temp1125, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("short", sizeof( unsigned char), 6u)); Cyc_sizeof_fdmask= Cyc_getsize((
struct _tagged_arr) _temp1125, _tag_arr("#include <sys/select.h>", sizeof(
unsigned char), 24u), _tag_arr("fd_mask", sizeof( unsigned char), 8u)); Cyc_sizeof___fdmask=
Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("#include <sys/select.h>",
sizeof( unsigned char), 24u), _tag_arr("__fd_mask", sizeof( unsigned char), 10u));
Cyc_sizeof_uint32= Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("#include <sys/types.h>",
sizeof( unsigned char), 23u), _tag_arr("__uint32_t", sizeof( unsigned char), 11u));
Cyc_sizeof_sockaddr= Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("#include <sys/socket.h>",
sizeof( unsigned char), 24u), _tag_arr("struct sockaddr", sizeof( unsigned char),
16u)); Cyc_sizeof_inport= Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("in_port_t", sizeof( unsigned char), 10u));
Cyc_sizeof_inaddr= Cyc_getsize(( struct _tagged_arr) _temp1125, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("struct in_addr", sizeof( unsigned char),
15u));{ int i= 2; for( 0; i <  argc; i ++){ Cyc_process_specfile(( struct
_tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), i)),( struct _tagged_arr) _temp1125);}} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_log_file)); Cyc_Std_fclose(( struct
Cyc_Std___sFILE*) _check_null( Cyc_cstubs_file)); Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file)); return 0;}}
