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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_remove( struct
_tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*); extern int
Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern struct Cyc_Std___sFILE* Cyc_Std_fopen(
struct _tagged_arr __filename, struct _tagged_arr __modes); extern int Cyc_Std_fputc(
int __c, struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_fputs( struct
_tagged_arr __s, struct Cyc_Std___sFILE* __stream); extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_vfprintf( struct
Cyc_Std___sFILE*, struct _tagged_arr fmt, struct _tagged_arr ap); static const
int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short*
f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
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
extern int _timezone; extern int daylight; struct Cyc_Std_stat_t{ short st_dev;
unsigned int st_ino; int st_mode; unsigned short st_nlink; unsigned short st_uid;
unsigned short st_gid; short st_rdev; int st_size; int st_atime; int st_spare1;
int st_mtime; int st_spare2; int st_ctime; int st_spare3; int st_blksize; int
st_blocks; int st_spare4[ 2u]; } ; extern int fstat( int fd, struct Cyc_Std_stat_t*
buf); extern int umask( int mask); extern int Cyc_Std_mkdir( struct _tagged_arr
pathname, int mode); extern int fchmod( int fd, int mode); struct Cyc_Std_flock{
short l_type; int l_start; short l_whence; int l_len; int l_pid; } ; static
const int Cyc_Std_Flock= 0; struct Cyc_Std_Flock_struct{ int tag; struct Cyc_Std_flock*
f1; } ; static const int Cyc_Std_Long= 1; struct Cyc_Std_Long_struct{ int tag;
int f1; } ; extern int Cyc_Std_open( struct _tagged_arr, int, struct _tagged_arr);
struct Cyc_Std_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_Std_timezone{
int tz_minuteswest; int tz_dsttime; } ; extern int gettimeofday( struct Cyc_Std_timeval*
__p, struct Cyc_Std_timezone* __z); struct Cyc_Std_itimerval{ struct Cyc_Std_timeval
it_interval; struct Cyc_Std_timeval it_value; } ; extern int getitimer( int,
struct Cyc_Std_itimerval*); extern int setitimer( int, const struct Cyc_Std_itimerval*,
struct Cyc_Std_itimerval*); typedef struct { unsigned int __val[ 64u]; } Cyc_Std___sigset_t;
typedef Cyc_Std___sigset_t Cyc_Std_sigset_t; struct Cyc_Std_timespec{ int tv_sec;
int tv_nsec; } ; struct Cyc_Std_timeval; extern int select( int, struct Cyc_Std__types_fd_set*,
struct Cyc_Std__types_fd_set*, struct Cyc_Std__types_fd_set*, struct Cyc_Std_timeval*);
struct Cyc_Std_option{ struct _tagged_arr name; int has_arg; int* flag; int val;
} ; extern unsigned int alarm( unsigned int seconds); extern int close( int);
extern void _exit( int); extern int getpid(); extern int getppid(); extern int
fork(); extern int fchdir( int); extern int fchown( int, unsigned short,
unsigned short); extern int fsync( int); extern int ftruncate( int, int); extern
int dup( int); extern int dup2( int, int); extern int setsid(); extern int
getsid( int pid); extern unsigned short getuid(); extern int setuid(
unsigned short uid); extern unsigned short geteuid(); extern int seteuid(
unsigned short euid); extern unsigned short getgid(); extern int setgid(
unsigned short gid); extern unsigned short getegid(); extern int setegid(
unsigned short egid); extern int nice( int); extern int pause(); extern int pipe(
int* filedes); extern int lseek( int filedes, int offset, int whence); extern
unsigned int sleep( unsigned int); extern int isatty( int); extern int daemon(
int nochdir, int noclose); int Cyc_Std_access( struct _tagged_arr, int); int Cyc_Std_chdir(
struct _tagged_arr); int Cyc_Std_chown( struct _tagged_arr, unsigned short,
unsigned short); struct _tagged_arr Cyc_Std_getcwd( struct _tagged_arr buf,
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
} ; extern void Cyc_Lex_lex_init(); extern unsigned char* Ccomp; struct Cyc_Std___sFILE*
Cyc_log_file= 0; struct Cyc_Std___sFILE* Cyc_cstubs_file= 0; struct Cyc_Std___sFILE*
Cyc_cycstubs_file= 0; int Cyc_log( struct _tagged_arr fmt, struct _tagged_arr ap){
int _temp0= Cyc_Std_vfprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_log_file),
fmt, ap); Cyc_Std_fflush(( struct Cyc_Std___sFILE*)(( struct Cyc_Std___sFILE*)
_check_null( Cyc_log_file))); return _temp0;} int Cyc_sizeof_unsignedlongint= -
1; int Cyc_sizeof_unsignedshortint= - 1; int Cyc_sizeof_shortint= - 1; int Cyc_sizeof_int=
- 1; int Cyc_sizeof_short= - 1; int Cyc_sizeof_fdmask= - 1; int Cyc_sizeof___fdmask=
- 1; int Cyc_sizeof_uint32= - 1; int Cyc_sizeof_sockaddr= - 1; int Cyc_sizeof_inport=
- 1; int Cyc_sizeof_inaddr= - 1; static struct _tagged_arr* Cyc_current_source=
0; static struct Cyc_List_List* Cyc_current_args= 0; static struct Cyc_Set_Set**
Cyc_current_targets= 0; static void Cyc_add_target( struct _tagged_arr* sptr){
Cyc_current_targets=({ struct Cyc_Set_Set** _temp1=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp1[ 0]=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**)
_check_null( Cyc_current_targets)), sptr); _temp1;});} struct _tuple8{ struct
_tagged_arr* f1; struct Cyc_Set_Set* f2; } ; extern struct _tuple8* Cyc_line(
struct Cyc_Lexing_lexbuf*); extern int Cyc_macroname( struct Cyc_Lexing_lexbuf*);
extern int Cyc_args( struct Cyc_Lexing_lexbuf*); extern int Cyc_token( struct
Cyc_Lexing_lexbuf*); extern int Cyc_string( struct Cyc_Lexing_lexbuf*); struct
Cyc_Std___sFILE* Cyc_slurp_out= 0; extern int Cyc_slurp( struct Cyc_Lexing_lexbuf*);
extern int Cyc_slurp_string( struct Cyc_Lexing_lexbuf*); extern int Cyc_asm(
struct Cyc_Lexing_lexbuf*); extern int Cyc_asm_string( struct Cyc_Lexing_lexbuf*);
extern int Cyc_asm_comment( struct Cyc_Lexing_lexbuf*); struct _tuple9{ struct
_tagged_arr f1; struct _tagged_arr* f2; } ; extern struct _tuple9* Cyc_suck_line(
struct Cyc_Lexing_lexbuf*); extern int Cyc_suck_macroname( struct Cyc_Lexing_lexbuf*);
extern int Cyc_suck_restofline( struct Cyc_Lexing_lexbuf*); struct _tagged_arr
Cyc_current_line=( struct _tagged_arr){( void*) 0u,( void*) 0u,( void*)( 0u +  0u)};
struct _tuple10{ struct _tagged_arr f1; struct _tagged_arr f2; } ; struct
_tuple11{ struct _tagged_arr f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; struct Cyc_List_List* f6;
struct Cyc_List_List* f7; } ; extern struct _tuple11* Cyc_spec( struct Cyc_Lexing_lexbuf*);
extern int Cyc_commands( struct Cyc_Lexing_lexbuf*); extern int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*); extern int Cyc_block( struct Cyc_Lexing_lexbuf*);
extern int Cyc_block_string( struct Cyc_Lexing_lexbuf*); extern int Cyc_block_comment(
struct Cyc_Lexing_lexbuf*); struct _tagged_arr Cyc_current_header=( struct
_tagged_arr){( void*) 0u,( void*) 0u,( void*)( 0u +  0u)}; struct Cyc_List_List*
Cyc_snarfed_symbols= 0; struct Cyc_List_List* Cyc_current_symbols= 0; struct Cyc_List_List*
Cyc_current_cstubs= 0; struct Cyc_List_List* Cyc_current_cycstubs= 0; struct Cyc_List_List*
Cyc_current_prologue= 0; struct Cyc_List_List* Cyc_current_epilogue= 0; struct
Cyc_List_List* Cyc_current_omit_symbols= 0; struct Cyc_List_List* Cyc_current_cpp=
0; struct Cyc_Buffer_t* Cyc_specbuf= 0; int Cyc_braces_to_match= 0; int Cyc_parens_to_match=
0; int Cyc_numdef= 0; const int Cyc_lex_base[ 515u]=( const int[ 515u]){ 0, 0,
75, 192, 305, 285, 310, 166, 311, 91, 27, 384, 28, 503, 617, 695, 288, 338, 93,
- 3, 0, - 2, - 1, - 8, - 3, 1, - 1, 586, - 4, 2, 166, - 5, 308, 777, 333, - 6, -
7, - 3, 16, - 2, 29, 11, 815, - 3, 890, 13, - 16, 223, 12, - 15, 213, 22, 26, 28,
24, - 14, 28, 58, 56, 65, 52, 70, 320, 1004, 1119, 357, 93, 83, 90, 112, 96, 106,
115, 124, 115, 126, 113, 382, 410, 152, 180, 209, 231, 231, 415, 505, 231, 229,
227, 237, 224, 241, 498, 1233, 1348, 510, 231, 230, 251, 238, 238, 238, 270, 515,
1462, 1577, - 9, 530, - 10, 252, 273, 633, 1691, 1806, 635, - 8, 664, - 11, 391,
646, 648, 1883, 1960, 2037, 2118, 416, 433, 653, 2193, 279, 289, 291, 289, 286,
297, 0, 13, 4, 2274, 5, 682, 2282, 2347, 800, 49, 465, 6, 2308, 7, 970, 2370,
2408, 978, - 32, 318, 418, 304, 307, 293, 315, 307, 319, 677, 1035, 336, 344,
335, 405, 366, 362, 405, 419, 416, 428, 431, 682, 1030, 423, 432, 427, 431, 430,
688, 1145, 438, 447, 1235, 454, 462, 809, 1147, 485, 481, 1350, 518, 517, 515,
551, 535, 826, 1259, 542, 543, 561, 561, 572, 570, 571, 561, 1464, - 12, 584,
584, 582, 584, 574, 1579, - 13, 579, 578, 1693, 2470, 589, 584, 1808, 607, 591,
2332, 596, 596, 595, 617, 597, 2472, 620, 610, 627, 610, 619, 2480, 636, 639,
635, 631, 626, 10, 14, 659, 639, 2482, 661, 648, 689, 673, 682, 2487, 1156, 708,
674, 710, 717, 728, 739, 741, 741, 745, 755, 762, 746, - 30, 798, 802, 798, 808,
816, 853, - 25, 833, 849, 842, 838, 849, 870, 872, 888, 889, - 19, 876, 900, 900,
939, 940, - 27, 932, 930, 925, 937, 936, 952, 931, 953, 956, 955, 959, 950, 950,
- 29, 959, 979, 982, 992, 1000, 983, 1015, 1032, 1039, 1040, 1, 4, 6, 1041, 1044,
1032, 1031, 1043, 1043, 1051, 1052, 2, 52, 1053, 1054, 2272, 20, 21, 1080, 1086,
1050, 1048, 1063, 1069, 1070, 1125, 1137, 1139, - 24, 1086, 1087, 1142, 1169,
1170, - 21, 1117, 1118, 1174, 1206, 1208, - 22, 1156, 1161, 1217, 1218, 1219, -
20, 1167, 1168, 1223, 1225, 1230, - 23, 1178, 1179, 1469, - 31, 1178, 1180, 1177,
1176, 1175, 1181, 1183, - 18, 1186, 1187, 1185, 1198, 4, - 15, 1216, 1217, 1215,
1228, 1297, - 17, 1265, 1268, 1258, 1259, 1257, 1270, 1338, 1263, 1264, 1262,
1276, 1344, - 7, - 8, 8, 1374, 2488, 9, 1435, 2496, 2561, 1545, 1489, - 49, 1377,
- 2, 1316, - 4, 1317, 1442, 2090, 1318, 1349, 1350, 1659, 1320, 2588, 2631, 1327,
1379, 1329, 1332, 2701, 1346, 1383, - 36, - 42, - 37, 2776, - 28, 1381, - 40,
1415, - 45, - 39, - 48, 2851, 2880, 1777, 1407, 1417, 2598, 2890, 2920, 2669,
2861, 2953, 2984, 3022, 1437, 1450, 3092, 3130, 1506, 1527, 1519, 1529, 1521,
1531, - 6, - 34, 1435, 3062, - 47, - 46, - 33, 1452, 3170, 1456, 1458, 1678,
1461, 1465, 1467, 1469, 1481, 1514, 1516, 1517, 1562, 3243, 3327, 2519, 1563, -
41, - 38, - 35, - 26, 1832, 3409, 1576, 3492, 1569, 15, 1512, 1512, 1512, 1510,
1506, 1516, 1586}; const int Cyc_lex_backtrk[ 515u]=( const int[ 515u]){ - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 2, - 1, - 1, - 1, - 1, 2, - 1, 8, - 1, 3, 5, - 1, - 1, 6, 5, - 1, - 1,
- 1, 6, - 1, 4, 1, 0, - 1, 0, 1, - 1, 12, 15, - 1, 15, 15, 15, 15, 15, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 11, 12, 2, 4, 4,
- 1, 1, 1, 0, 2, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, 2, 8, 3, 5, - 1, 6, 5,
6, 4, 2, 8, 3, 5, - 1, 6, 5, - 1, 31, 31, 31, 31, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 25, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 27, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 6, 1, 9, 2, 4, - 1, 5, 4, - 1, - 1, 2, - 1, 48, - 1, 48, 48, 48,
48, 48, 48, 48, 48, 5, 7, 48, 48, 48, 48, 0, 48, 48, - 1, - 1, - 1, 0, - 1, 43,
- 1, 42, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9, 5, 6, 5, 5, - 1,
4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, 9, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 2, - 1, - 1, 2, 1, 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_default[
515u]=( const int[ 515u]){ - 1, - 1, - 1, 417, 406, 153, 23, 36, 23, 19, - 1, -
1, 12, 31, 46, 31, 36, 23, 19, 0, - 1, 0, 0, 0, 0, - 1, 0, - 1, 0, - 1, - 1, 0,
- 1, - 1, - 1, 0, 0, 0, - 1, 0, 40, - 1, - 1, 0, - 1, - 1, 0, - 1, - 1, 0, - 1,
- 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1,
0, 118, - 1, - 1, - 1, - 1, - 1, 125, 125, 125, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 145, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, -
1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1,
0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 0, - 1, 0, - 1, 0, - 1, - 1, 480, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0, - 1, 0, - 1, 0, - 1, 0, 0, 0, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 0, 0, - 1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0, 0, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_trans[
3749u]=( const int[ 3749u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 19, 28, 507, 19,
28, 19, 28, 115, 19, 45, 45, 45, 45, 45, 21, 45, 0, 0, 0, 0, 0, 22, 318, 329,
508, 387, 21, - 1, - 1, 21, - 1, - 1, 45, 319, 45, 320, 22, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 28, 118, 22, 246, 129, 40, 247, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 28, 330, 364, 358, 505, 145, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
136, 86, 20, 79, 66, 56, 57, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
58, 59, 60, 61, 503, 62, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 67,
68, 37, 418, 419, 418, 418, 419, 39, 22, 69, 70, 71, 72, 73, 144, 34, 34, 34, 34,
34, 34, 34, 34, 74, 75, 418, 420, 421, 76, 77, 422, 423, 424, 119, 119, 425, 426,
119, 427, 428, 429, 430, 431, 431, 431, 431, 431, 431, 431, 431, 431, 432, 80,
433, 434, 435, 119, 19, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436,
436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 19, -
1, - 1, 437, 436, 81, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436,
436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 407, 438,
154, 408, 22, 24, 24, 37, 146, 137, 96, 82, 155, 97, 63, 63, 83, 84, 63, 98, 38,
87, 88, 89, 409, 90, 91, 92, 116, 26, 26, 109, 21, 24, 21, 99, 25, 63, 100, 101,
102, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 65, 65, 103, 110, 65, 111, 26, 33,
33, 33, 33, 33, 33, 130, 156, 35, 35, 35, 35, 35, 35, 35, 35, 65, 131, 78, 78,
132, 133, 78, 134, 410, 135, 256, 157, - 1, 147, 138, - 1, 33, 33, 33, 33, 33,
33, 39, 158, 22, 78, 159, 160, 394, 161, 78, 78, 162, 35, 78, 85, 85, - 1, 395,
85, - 1, 27, 240, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 78, - 1, 234,
225, - 1, 85, 21, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, - 1, 169,
170, - 1, 128, 28, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 93, 93,
194, 171, 93, 120, 120, 85, 85, 120, 374, 85, 95, 95, 195, 172, 95, 104, 104,
173, 375, 104, 174, 93, 175, 376, 21, 187, 120, 179, 85, 180, 107, 107, 21, 95,
107, 35, 181, 182, 104, 185, 121, 122, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 106, 107, 186, 188, 189, 21, 21, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 21, 192, 28, 193, 121, 29, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 47, 47, 22, 218, 47, 196, 197, 35, 30, 30, 30, 30, 30, 30, 30, 30, 112,
112, 114, 114, 112, - 1, 114, 47, 198, 199, 211, 108, 203, 119, 119, 127, 127,
119, 204, 127, 127, 127, 48, 112, 127, 114, 205, 206, 207, 208, - 1, 116, 116,
209, 49, 116, 119, 212, 127, 213, 214, 31, 215, 127, 162, 162, 216, - 1, 162,
176, 176, 219, 220, 176, 116, 183, 183, 223, 224, 183, 226, 228, 41, 41, 32, 229,
41, 162, 230, 231, 232, 233, 176, 235, 50, 163, 51, 236, 183, - 1, 52, 227, 237,
238, 239, 41, 53, 54, 143, 143, 143, 143, 143, 143, 143, 143, 241, 250, 243, 244,
245, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 242, 248, 249, 251, 252, 115, 28,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 253, 117, 254, 255, 42, 293, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 190,
190, 43, 287, 190, 307, 308, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 200, 200,
277, 270, 200, 263, 190, 33, 33, 33, 33, 33, 33, 35, 35, 35, 35, 35, 35, 35, 35,
264, 265, 200, 266, 267, 268, 269, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 55,
33, 33, 33, 33, 33, 33, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 271, 272, 273, 274, 44, 275, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 276, 278, 279, 28,
280, 281, 282, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 283, 284, 285, 286, 44, 288, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 63, 63, 289, 290, 63, 152, 152, 152, 152, 152, 152, 152, 152, 35, 35,
35, 35, 35, 35, 35, 35, 291, 292, 63, 294, 295, 176, 176, 296, 297, 176, 163,
163, 298, 299, 163, 300, 301, 302, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 176,
303, 304, 305, 306, 163, 370, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 309, 310, 311, 312, 64, 313,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 19, 65, 65, 164, 314, 65, 315, 316, 317, 321, 165, 177,
322, 166, 323, 324, 325, 326, 178, 327, 328, 331, 332, 167, 65, 168, 352, 183,
183, 190, 190, 183, 346, 190, 338, 339, 340, 341, 342, 343, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 183, 344, 190, 345, 347, 348, 349, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
350, 351, 353, 354, 64, 355, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 93, 93, 186, 186, 93, 356,
186, 357, 184, 359, 191, 257, 258, 259, 360, 260, 361, 362, 363, 261, 365, 366,
367, 93, 368, 186, 200, 200, 262, 369, 200, 371, 372, 388, 108, 382, 377, 378,
379, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 200, 380, 381, 383, 384, 385, 386,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 389, 390, 391, 392, 94, 393, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 31,
95, 95, 193, 193, 95, 401, 193, 201, 396, 397, 398, 399, 400, 55, 402, 403, 404,
202, 405, 46, 501, 500, 479, 95, 474, 193, 416, 416, 449, 502, 416, 502, 502,
276, 106, 306, 443, 444, 269, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 416, 441,
22, 502, 373, 153, 478, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 446, 292, 447, 445, 94, 439, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 104, 104, 209, 209, 104, 448, 209, 372, 372, 498, 477, 372,
415, 415, 415, 415, 415, 415, 415, 415, 345, 115, 115, 104, 363, 209, 286, 416,
416, 351, 372, 416, 499, 381, 210, 117, 440, 210, 373, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 217, 416, 35, 22, 115, 115, 35, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 55, 35, 387, 46, 105, 35, 105, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, 105, 106, 107, 107, 216, 216, 107, 31, 216, 31, 31, 31, 31, 31,
31, 31, 31, 393, 357, 31, 36, 36, 473, 473, 22, 107, 22, 216, 509, 510, 511, 512,
513, 514, 22, 0, 217, 0, 0, 31, 0, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 31, 36, 36, 473, 473, 0, 0, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 0, 0, 0, 0, 105, 0, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 112,
112, 221, 221, 112, 475, 221, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476,
357, 0, 0, 0, 0, 0, 112, 0, 221, 496, 496, 496, 496, 496, 496, 496, 496, 115, 0,
0, 0, 0, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 0, 0, 0, 0, 0, 0, 0,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 0, 0, 0, 0, 113, 0, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 36, 114, 114, 224, 224, 114, 455, 224,
455, 0, 0, 456, 456, 456, 456, 456, 456, 456, 456, 456, 456, 0, 0, 0, 114, 0,
224, 502, 0, 502, 502, 0, 0, 0, 0, 117, 0, 0, 0, 0, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 502, 0, 0, 0, 0, 0, 0, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 0, 0, 0, 0, 113, 0, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 22, 0, 0, 0, 0,
0, 0, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 0, 0, 0, 0, 123, 0, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 124, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 22, 0, 0, 0, 0, 0, 0, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 0, 0, 0, 0, 123, 0, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 22, 0, 0, 0, 0,
- 1, 0, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, - 1, - 1, 0, - 1,
123, 0, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 0, 0, 0, 0, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 126, 0, 0, 0, 0, 0, 481, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 0, 0, 0, 0, 124, 0, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 28, 0,
0, 139, 128, 0, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0, 0, 28, 0, 0,
148, 140, 140, 140, 140, 140, 140, 140, 140, 142, 142, 142, 142, 142, 142, 142,
142, 142, 142, 333, 227, 227, 0, 334, 227, - 1, 142, 142, 142, 142, 142, 142,
335, 0, 336, 149, 149, 149, 149, 149, 149, 149, 149, 227, 0, 0, 0, 0, 0, 0, 31,
0, 36, - 1, 0, 0, 0, 0, 142, 142, 142, 142, 142, 142, 0, 0, 0, 0, 0, 0, 337, 0,
0, 141, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 31, 0, 0, 0, 0, 0, 0,
142, 142, 142, 142, 142, 142, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151,
150, 0, 0, 0, 0, 0, 0, 151, 151, 151, 151, 151, 151, 0, 0, 0, 142, 142, 142, 142,
142, 142, 0, 0, 0, 0, 0, 0, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 0,
151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 221, 221, 233, 233,
221, 0, 233, 0, 0, 0, 239, 239, 249, 249, 239, 0, 249, 255, 255, 19, 0, 255, 411,
221, 0, 233, 151, 151, 151, 151, 151, 151, 115, 239, 35, 249, 0, 0, 0, 0, 255, 0,
19, 0, 31, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 412, 412, 412, 412, 412, 412,
412, 412, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 0, 0, 0, 0, 357, 0,
0, 414, 414, 414, 414, 414, 414, 497, 497, 497, 497, 497, 497, 497, 497, 222, 0,
0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 414, 414, 414, 414, 414, 414, 0,
0, 0, 0, 0, 0, 0, 0, 0, 413, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 0,
0, 0, 0, 0, 0, 0, 414, 414, 414, 414, 414, 414, 0, 0, 450, 0, 461, 461, 461, 461,
461, 461, 461, 461, 462, 462, 456, 456, 456, 456, 456, 456, 456, 456, 456, 456,
0, 452, 414, 414, 414, 414, 414, 414, 463, 0, 0, 0, 0, 0, 0, 0, 0, 464, 0, 0,
465, 450, 0, 451, 451, 451, 451, 451, 451, 451, 451, 451, 451, 452, 0, 0, 0, 0,
0, 0, 463, 0, 0, 0, 452, 0, 0, 0, 0, 464, 0, 453, 465, 0, 0, 0, 459, 0, 459, 0,
454, 460, 460, 460, 460, 460, 460, 460, 460, 460, 460, 0, 0, 0, 0, 0, 452, 0, 0,
0, 0, 0, 0, 453, 0, 0, 0, 0, 0, 0, 0, 0, 454, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 0, 0, 0, 0, 0, 0, 0, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 0, 0, 0, 0, 442, 0, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 0, 0, 0, 0, 0, 0, 0, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 0, 0, 0, 0, 442, 0, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 457, 457, 457, 457, 457, 457, 457, 457, 457, 457, 460,
460, 460, 460, 460, 460, 460, 460, 460, 460, 0, 458, 108, 0, 0, 0, 0, 450, 108,
451, 451, 451, 451, 451, 451, 451, 451, 451, 451, 456, 456, 456, 456, 456, 456,
456, 456, 456, 456, 0, 452, 0, 0, 458, 108, 0, 0, 453, 0, 0, 108, 106, 0, 0, 0,
0, 454, 106, 0, 457, 457, 457, 457, 457, 457, 457, 457, 457, 457, 0, 0, 0, 452,
0, 0, 0, 0, 0, 0, 453, 458, 108, 0, 106, 0, 0, 0, 108, 454, 106, 0, 0, 460, 460,
460, 460, 460, 460, 460, 460, 460, 460, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 458, 108,
108, 0, 0, 0, 0, 108, 108, 450, 0, 461, 461, 461, 461, 461, 461, 461, 461, 462,
462, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 452, 0, 108, 0, 0, 0, 0, 471, 108, 0, 0, 0,
0, 0, 0, 450, 472, 462, 462, 462, 462, 462, 462, 462, 462, 462, 462, 0, 0, 0, 0,
0, 452, 0, 0, 0, 0, 0, 452, 471, 0, 0, 0, 0, 0, 469, 0, 0, 472, 0, 0, 0, 0, 0,
470, 0, 0, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 0, 0, 0, 452, 0, 0,
0, 0, 0, 0, 469, 458, 108, 0, 0, 0, 0, 0, 108, 470, 466, 466, 466, 466, 466, 466,
466, 466, 466, 466, 0, 0, 0, 0, 0, 0, 0, 466, 466, 466, 466, 466, 466, 458, 108,
0, 0, 0, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0, 466, 466, 466, 466, 466, 466, 466, 466,
466, 466, 0, 466, 466, 466, 466, 466, 466, 466, 466, 466, 466, 466, 466, 0, 0, 0,
482, 0, 467, 0, 0, 483, 0, 0, 0, 0, 0, 468, 0, 0, 484, 484, 484, 484, 484, 484,
484, 484, 0, 466, 466, 466, 466, 466, 466, 485, 0, 0, 0, 0, 467, 0, 0, 0, 0, 0,
0, 0, 0, 468, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 486, 0, 0, 0, 0, 487,
488, 0, 0, 0, 489, 0, 0, 0, 0, 0, 0, 0, 490, 0, 0, 0, 491, 0, 492, 0, 493, 0,
494, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 0, 0, 0, 0, 0, 0, 0, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 0, 0, 0, 0, 0, 0, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 369, 0, 0, 0, 0, 0, 0, 0, 0, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 0, 0, 0, 0, 0, 0, 0, 495, 495, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495,
495, 495, 495, 495, 495, 0, 0, 0, 0, 0, 0, 495, 495, 495, 495, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495,
495, 495, 495, 21, 0, 0, 504, 0, 0, 0, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 0, 0, 0, 0, 0, 0, 0, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
0, 0, 0, 0, 503, 0, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 506, 0, 0,
0, 0, 0, 0, 0, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 0, 0, 0, 0, 0,
0, 0, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 0, 0, 0, 0, 505, 0, 505,
505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 505, 505, 505, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; const int Cyc_lex_check[ 3749u]=(
const int[ 3749u]){ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 25, 29,
0, 137, 139, 146, 148, 408, 411, 41, 41, 45, 45, 41, 507, 45, - 1, - 1, - 1, - 1,
- 1, 135, 317, 328, 0, 386, 10, 12, 40, 10, 12, 40, 41, 318, 45, 319, 20, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 38, 48, 136, 245, 10, 38, 246, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 144, 329, 333, 334, 1, 144, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 9, 51, 18, 52, 53, 54, 56, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 57, 58, 59, 60, 2, 61, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 66, 67, 7,
3, 3, 3, 3, 3, 7, 7, 68, 69, 70, 71, 72, 7, 30, 30, 30, 30, 30, 30, 30, 30, 73,
74, 3, 3, 3, 75, 76, 3, 3, 3, 47, 47, 3, 3, 47, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 79, 3, 3, 3, 47, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 12, 40, 3, 3, 80, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 5, 4, 5, 6, 8, 16, 6, 8, 50,
81, 5, 50, 62, 62, 82, 83, 62, 50, 16, 86, 87, 88, 4, 89, 90, 91, 96, 6, 8, 97,
9, 17, 18, 98, 17, 62, 99, 100, 101, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 65,
65, 102, 109, 65, 110, 17, 32, 32, 32, 32, 32, 32, 129, 5, 34, 34, 34, 34, 34,
34, 34, 34, 65, 130, 77, 77, 131, 132, 77, 133, 4, 134, 156, 5, 118, 6, 8, 118,
32, 32, 32, 32, 32, 32, 16, 157, 16, 77, 158, 159, 154, 160, 78, 78, 161, 7, 78,
84, 84, 125, 154, 84, 125, 17, 164, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 78,
126, 165, 166, 126, 84, 3, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 145, 168, 169, 145, 11, 65,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 92, 92, 167, 170, 92, 13, 13, 85, 85, 13, 155, 85, 95,
95, 167, 171, 95, 103, 103, 172, 155, 103, 173, 92, 174, 155, 78, 177, 13, 178,
85, 179, 107, 107, 5, 95, 107, 16, 180, 181, 103, 184, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 4, 107, 185, 187, 188, 6, 8, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 17,
191, 27, 192, 13, 27, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 85, 194, 14, 195, 196, 95,
27, 27, 27, 27, 27, 27, 27, 27, 111, 111, 114, 114, 111, 118, 114, 14, 197, 198,
201, 107, 202, 119, 119, 120, 120, 119, 203, 120, 127, 127, 14, 111, 127, 114,
204, 205, 206, 207, 125, 116, 116, 208, 14, 116, 119, 211, 120, 212, 213, 27,
214, 127, 162, 162, 215, 126, 162, 175, 175, 218, 219, 175, 116, 182, 182, 222,
223, 182, 225, 226, 15, 15, 27, 228, 15, 162, 229, 230, 231, 232, 175, 234, 14,
162, 14, 235, 182, 145, 14, 225, 236, 237, 238, 15, 14, 14, 140, 140, 140, 140,
140, 140, 140, 140, 240, 241, 242, 243, 244, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 240, 247, 248, 250, 251, 114, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 252, 116, 253, 254,
15, 258, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 189, 189, 15, 259, 189, 257, 257, 33, 33, 33, 33,
33, 33, 33, 33, 33, 33, 199, 199, 260, 261, 199, 262, 189, 33, 33, 33, 33, 33,
33, 143, 143, 143, 143, 143, 143, 143, 143, 263, 264, 199, 265, 266, 267, 268,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 14, 33, 33, 33, 33, 33, 33, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 270, 271, 272, 273, 42, 274, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 275, 277, 278, 15, 279, 280, 281, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
282, 283, 284, 285, 44, 287, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 63, 63, 288, 289, 63, 149,
149, 149, 149, 149, 149, 149, 149, 152, 152, 152, 152, 152, 152, 152, 152, 290,
291, 63, 293, 294, 176, 176, 295, 296, 176, 163, 163, 297, 298, 163, 299, 300,
301, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 176, 302, 303, 304, 305, 163, 307,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 308, 309, 310, 311, 63, 312, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
64, 64, 163, 313, 64, 314, 315, 316, 320, 163, 176, 321, 163, 322, 323, 324, 325,
176, 326, 327, 330, 331, 163, 64, 163, 335, 183, 183, 190, 190, 183, 336, 190,
337, 338, 339, 340, 341, 342, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 183, 343,
190, 344, 346, 347, 348, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 349, 350, 352, 353, 64, 354, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 93, 93, 186, 186, 93, 355, 186, 356, 183, 358, 190, 256, 256,
256, 359, 256, 360, 361, 362, 256, 364, 365, 366, 93, 367, 186, 200, 200, 256,
368, 200, 370, 371, 374, 186, 375, 376, 377, 378, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 200, 379, 380, 382, 383, 384, 385, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 388, 389,
390, 391, 93, 392, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 94, 94, 193, 193, 94, 394, 193,
200, 395, 396, 397, 398, 399, 400, 401, 402, 403, 200, 404, 405, 420, 422, 425,
94, 429, 193, 409, 409, 432, 418, 409, 418, 418, 434, 193, 426, 435, 435, 427,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 409, 437, 409, 418, 426, 427, 427, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 433, 433, 433, 444, 94, 438, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 104, 104,
209, 209, 104, 446, 209, 372, 372, 423, 475, 372, 412, 412, 412, 412, 412, 412,
412, 412, 480, 453, 454, 104, 482, 209, 483, 416, 416, 485, 372, 416, 423, 486,
209, 487, 438, 488, 372, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 489,
416, 463, 416, 453, 454, 464, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
490, 463, 491, 492, 104, 464, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 105, 105, 216, 216, 105, 467, 216, 415, 415, 415, 415, 415, 415, 415, 415,
493, 497, 468, 469, 470, 471, 472, 504, 105, 506, 216, 508, 509, 510, 511, 512,
513, 514, - 1, 216, - 1, - 1, 467, - 1, 105, 105, 105, 105, 105, 105, 105, 105,
105, 105, 468, 469, 470, 471, 472, - 1, - 1, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, - 1, - 1, - 1, - 1, 105, - 1, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
105, 105, 105, 112, 112, 220, 220, 112, 428, 220, 428, 428, 428, 428, 428, 428,
428, 428, 428, 428, 484, - 1, - 1, - 1, - 1, - 1, 112, - 1, 220, 484, 484, 484,
484, 484, 484, 484, 484, 220, - 1, - 1, - 1, - 1, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, - 1, - 1, - 1, - 1, 112, - 1, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 113, 113, 224, 224, 113, 452, 224, 452, - 1, - 1,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, - 1, - 1, - 1, 113, - 1, 224,
502, - 1, 502, 502, - 1, - 1, - 1, - 1, 224, - 1, - 1, - 1, - 1, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 502, - 1, - 1, - 1, - 1, - 1, - 1, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, - 1, - 1, - 1, - 1, 113, - 1, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, - 1, - 1, - 1, - 1, - 1, - 1, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, - 1, - 1, - 1, - 1, 121, - 1, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
122, 122, - 1, - 1, - 1, - 1, - 1, - 1, 122, 122, 122, 122, 122, 122, 122, 122,
122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
122, 122, - 1, - 1, - 1, - 1, 122, - 1, 122, 122, 122, 122, 122, 122, 122, 122,
122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
122, 122, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, - 1,
- 1, - 1, - 1, 424, - 1, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 124,
424, - 1, 124, 123, - 1, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, - 1,
- 1, - 1, - 1, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
- 1, - 1, - 1, - 1, - 1, 424, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
- 1, - 1, - 1, - 1, 124, - 1, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 138, - 1, - 1, 138, 128,
- 1, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, - 1, - 1, 147, - 1, - 1,
147, 138, 138, 138, 138, 138, 138, 138, 138, 141, 141, 141, 141, 141, 141, 141,
141, 141, 141, 332, 227, 227, - 1, 332, 227, 424, 141, 141, 141, 141, 141, 141,
332, - 1, 332, 147, 147, 147, 147, 147, 147, 147, 147, 227, - 1, - 1, - 1, - 1,
- 1, - 1, 138, - 1, 227, 124, - 1, - 1, - 1, - 1, 141, 141, 141, 141, 141, 141,
- 1, - 1, - 1, - 1, - 1, - 1, 332, - 1, - 1, 138, 142, 142, 142, 142, 142, 142,
142, 142, 142, 142, 147, - 1, - 1, - 1, - 1, - 1, - 1, 142, 142, 142, 142, 142,
142, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 147, - 1, - 1, - 1, - 1,
- 1, - 1, 150, 150, 150, 150, 150, 150, - 1, - 1, - 1, 142, 142, 142, 142, 142,
142, - 1, - 1, - 1, - 1, - 1, - 1, 151, 151, 151, 151, 151, 151, 151, 151, 151,
151, - 1, 150, 150, 150, 150, 150, 150, 151, 151, 151, 151, 151, 151, 221, 221,
233, 233, 221, - 1, 233, - 1, - 1, - 1, 239, 239, 249, 249, 239, - 1, 249, 255,
255, 410, - 1, 255, 410, 221, - 1, 233, 151, 151, 151, 151, 151, 151, 221, 239,
233, 249, - 1, - 1, - 1, - 1, 255, - 1, 239, - 1, 249, - 1, - 1, - 1, - 1, 255,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 410, 410, 410, 410, 410, 410, 410, 410, 413,
413, 413, 413, 413, 413, 413, 413, 413, 413, - 1, - 1, - 1, - 1, 496, - 1, - 1,
413, 413, 413, 413, 413, 413, 496, 496, 496, 496, 496, 496, 496, 496, 221, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 410, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
413, 413, 413, 413, 413, 413, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 410,
414, 414, 414, 414, 414, 414, 414, 414, 414, 414, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 414, 414, 414, 414, 414, 414, - 1, - 1, 430, - 1, 430, 430, 430, 430, 430,
430, 430, 430, 430, 430, 455, 455, 455, 455, 455, 455, 455, 455, 455, 455, - 1,
430, 414, 414, 414, 414, 414, 414, 430, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
430, - 1, - 1, 430, 431, - 1, 431, 431, 431, 431, 431, 431, 431, 431, 431, 431,
430, - 1, - 1, - 1, - 1, - 1, - 1, 430, - 1, - 1, - 1, 431, - 1, - 1, - 1, - 1,
430, - 1, 431, 430, - 1, - 1, - 1, 458, - 1, 458, - 1, 431, 458, 458, 458, 458,
458, 458, 458, 458, 458, 458, - 1, - 1, - 1, - 1, - 1, 431, - 1, - 1, - 1, - 1,
- 1, - 1, 431, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 431, 436, 436, 436, 436,
436, 436, 436, 436, 436, 436, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 436, 436, 436,
436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436,
436, 436, 436, 436, 436, 436, 436, - 1, - 1, - 1, - 1, 436, - 1, 436, 436, 436,
436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436,
436, 436, 436, 436, 436, 436, 436, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, - 1, - 1, - 1, - 1, 442, - 1, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442, 442,
442, 442, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 459, 459, 459, 459,
459, 459, 459, 459, 459, 459, - 1, 450, 450, - 1, - 1, - 1, - 1, 451, 450, 451,
451, 451, 451, 451, 451, 451, 451, 451, 451, 456, 456, 456, 456, 456, 456, 456,
456, 456, 456, - 1, 451, - 1, - 1, 450, 450, - 1, - 1, 451, - 1, - 1, 450, 456,
- 1, - 1, - 1, - 1, 451, 456, - 1, 457, 457, 457, 457, 457, 457, 457, 457, 457,
457, - 1, - 1, - 1, 451, - 1, - 1, - 1, - 1, - 1, - 1, 451, 457, 457, - 1, 456,
- 1, - 1, - 1, 457, 451, 456, - 1, - 1, 460, 460, 460, 460, 460, 460, 460, 460,
460, 460, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 457, 457, 460, - 1,
- 1, - 1, - 1, 457, 460, 461, - 1, 461, 461, 461, 461, 461, 461, 461, 461, 461,
461, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 461, - 1, 460, - 1,
- 1, - 1, - 1, 461, 460, - 1, - 1, - 1, - 1, - 1, - 1, 462, 461, 462, 462, 462,
462, 462, 462, 462, 462, 462, 462, - 1, - 1, - 1, - 1, - 1, 461, - 1, - 1, - 1,
- 1, - 1, 462, 461, - 1, - 1, - 1, - 1, - 1, 462, - 1, - 1, 461, - 1, - 1, - 1,
- 1, - 1, 462, - 1, - 1, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, - 1,
- 1, - 1, 462, - 1, - 1, - 1, - 1, - 1, - 1, 462, 476, 476, - 1, - 1, - 1, - 1,
- 1, 476, 462, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 465, 465, 465, 465, 465, 465, 476, 476, - 1, - 1, - 1, - 1,
- 1, 476, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 466, 466, 466, 466, 466, 466, 466,
466, 466, 466, - 1, 465, 465, 465, 465, 465, 465, 466, 466, 466, 466, 466, 466,
- 1, - 1, - 1, 481, - 1, 466, - 1, - 1, 481, - 1, - 1, - 1, - 1, - 1, 466, - 1,
- 1, 481, 481, 481, 481, 481, 481, 481, 481, - 1, 466, 466, 466, 466, 466, 466,
481, - 1, - 1, - 1, - 1, 466, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 466, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 481, - 1, - 1,
- 1, - 1, 481, 481, - 1, - 1, - 1, 481, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 481,
- 1, - 1, - 1, 481, - 1, 481, - 1, 481, - 1, 481, 494, 494, 494, 494, 494, 494,
494, 494, 494, 494, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 494, 494, 494, 494, 494,
494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494,
494, 494, 494, 494, 494, - 1, - 1, - 1, - 1, - 1, - 1, 494, 494, 494, 494, 494,
494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494, 494,
494, 494, 494, 494, 494, 495, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 495, 495,
495, 495, 495, 495, 495, 495, 495, 495, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, - 1, - 1, - 1, - 1, - 1, - 1, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495, 495,
495, 495, 495, 495, 495, 495, 495, 495, 495, 503, - 1, - 1, 503, - 1, - 1, - 1,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, - 1, - 1, - 1, - 1, 503,
- 1, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503,
503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 503, 505, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, - 1, - 1,
- 1, - 1, 505, - 1, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505,
505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, 505, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1}; int Cyc_lex_engine( int start_state, struct Cyc_Lexing_lexbuf*
lbuf){ int state; int base; int backtrk; int c; state= start_state; if( state >= 
0){ lbuf->lex_last_pos=( lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action=
- 1;} else{ state=( - state) -  1;} while( 1) { base= Cyc_lex_base[
_check_known_subscript_notnull( 515u, state)]; if( base <  0){ return( - base) - 
1;} backtrk= Cyc_lex_backtrk[ _check_known_subscript_notnull( 515u, state)]; if(
backtrk >=  0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >=  lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) -  1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c ==  - 1){ c= 256;}} if( Cyc_lex_check[
_check_known_subscript_notnull( 3749u, base +  c)] ==  state){ state= Cyc_lex_trans[
_check_known_subscript_notnull( 3749u, base +  c)];} else{ state= Cyc_lex_default[
_check_known_subscript_notnull( 515u, state)];} if( state <  0){ lbuf->lex_curr_pos=
lbuf->lex_last_pos; if( lbuf->lex_last_action ==  - 1){( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp2=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp2[ 0]=({ struct Cyc_Lexing_Error_struct
_temp3; _temp3.tag= Cyc_Lexing_Error; _temp3.f1= _tag_arr("empty token", sizeof(
unsigned char), 12u); _temp3;}); _temp2;}));} else{ return lbuf->lex_last_action;}}
else{ if( c ==  256){ lbuf->lex_eof_reached= 0;}}}} struct _tuple8* Cyc_line_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL4: Cyc_macroname( lexbuf); for(
0; Cyc_current_args !=  0; Cyc_current_args=(( struct Cyc_List_List*)
_check_null( Cyc_current_args))->tl){ Cyc_current_targets=({ struct Cyc_Set_Set**
_temp6=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*)); _temp6[
0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt))
Cyc_Set_delete)(*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets)),(
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_current_args))->hd);
_temp6;});} return({ struct _tuple8* _temp7=( struct _tuple8*) _cycalloc(
sizeof( struct _tuple8)); _temp7->f1=( struct _tagged_arr*) _check_null( Cyc_current_source);
_temp7->f2=*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets)); _temp7;});
case 1: _LL5: return Cyc_line( lexbuf); case 2: _LL8: return 0; default: _LL9:(
lexbuf->refill_buff)( lexbuf); return Cyc_line_rec( lexbuf, lexstate);}( int)
_throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp11=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp11[ 0]=({ struct Cyc_Lexing_Error_struct
_temp12; _temp12.tag= Cyc_Lexing_Error; _temp12.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp12;}); _temp11;}));} struct _tuple8* Cyc_line(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_line_rec( lexbuf, 0);} int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL13: Cyc_current_source=({
struct _tagged_arr* _temp15=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp15[ 0]=( struct _tagged_arr) Cyc_Std_substring(( struct
_tagged_arr) Cyc_Lexing_lexeme( lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end(
lexbuf) -  Cyc_Lexing_lexeme_start( lexbuf)) -  2)); _temp15;}); Cyc_current_args=
0; Cyc_current_targets=({ struct Cyc_Set_Set** _temp16=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp16[ 0]=(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_temp16;}); Cyc_token( lexbuf); return 0; case 1: _LL14: Cyc_current_source=({
struct _tagged_arr* _temp18=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp18[ 0]=( struct _tagged_arr) Cyc_Std_substring(( struct
_tagged_arr) Cyc_Lexing_lexeme( lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end(
lexbuf) -  Cyc_Lexing_lexeme_start( lexbuf)) -  1)); _temp18;}); Cyc_current_args=
0; Cyc_current_targets=({ struct Cyc_Set_Set** _temp19=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp19[ 0]=(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_temp19;}); Cyc_args( lexbuf); return 0; case 2: _LL17: Cyc_current_source=({
struct _tagged_arr* _temp21=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp21[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);
_temp21;}); Cyc_current_args= 0; Cyc_current_targets=({ struct Cyc_Set_Set**
_temp22=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp22[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); _temp22;}); Cyc_token(
lexbuf); return 0; default: _LL20:( lexbuf->refill_buff)( lexbuf); return Cyc_macroname_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp24=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp24[ 0]=({ struct Cyc_Lexing_Error_struct _temp25; _temp25.tag= Cyc_Lexing_Error;
_temp25.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp25;}); _temp24;}));} int Cyc_macroname( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_macroname_rec( lexbuf, 1);} int Cyc_args_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL26: { struct _tagged_arr* _temp28=({ struct _tagged_arr*
_temp30=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp30[
0]=( struct _tagged_arr) Cyc_Std_substring(( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end( lexbuf) -  Cyc_Lexing_lexeme_start(
lexbuf)) -  2)); _temp30;}); Cyc_current_args=({ struct Cyc_List_List* _temp29=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp29->hd=(
void*) _temp28; _temp29->tl= Cyc_current_args; _temp29;}); return Cyc_args(
lexbuf);} case 1: _LL27: { struct _tagged_arr* _temp32=({ struct _tagged_arr*
_temp34=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp34[
0]=( struct _tagged_arr) Cyc_Std_substring(( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end( lexbuf) -  Cyc_Lexing_lexeme_start(
lexbuf)) -  1)); _temp34;}); Cyc_current_args=({ struct Cyc_List_List* _temp33=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp33->hd=(
void*) _temp32; _temp33->tl= Cyc_current_args; _temp33;}); return Cyc_token(
lexbuf);} default: _LL31:( lexbuf->refill_buff)( lexbuf); return Cyc_args_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp36=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp36[ 0]=({ struct Cyc_Lexing_Error_struct _temp37; _temp37.tag= Cyc_Lexing_Error;
_temp37.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp37;}); _temp36;}));} int Cyc_args( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_args_rec( lexbuf, 2);} int Cyc_token_rec( struct Cyc_Lexing_lexbuf*
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
return Cyc_slurp( lexbuf); case 17: _LL159: Cyc_Std_fputs( _tag_arr("__region",
sizeof( unsigned char), 9u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));({
void* _temp162[ 0u]={}; Cyc_log( _tag_arr("Warning: use of region sidestepped\n",
sizeof( unsigned char), 36u), _tag_arr( _temp162, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 18: _LL161: return Cyc_slurp( lexbuf); case 19:
_LL163: return Cyc_slurp( lexbuf); case 20: _LL164: return Cyc_slurp( lexbuf);
case 21: _LL165: return Cyc_slurp( lexbuf); case 22: _LL166: return Cyc_slurp(
lexbuf); case 23: _LL167: return Cyc_slurp( lexbuf); case 24: _LL168: Cyc_Std_fputs(
_tag_arr("inline", sizeof( unsigned char), 7u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 25: _LL169: Cyc_Std_fputs(
_tag_arr("inline", sizeof( unsigned char), 7u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 26: _LL170: Cyc_Std_fputs(
_tag_arr("const", sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 27: _LL171: Cyc_Std_fputs(
_tag_arr("const", sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 28: _LL172: Cyc_Std_fputs(
_tag_arr("int", sizeof( unsigned char), 4u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 29: _LL173: return
Cyc_slurp( lexbuf); case 30: _LL174: Cyc_parens_to_match= 1; if( Cyc_asm( lexbuf)){
return 1;} Cyc_Std_fputs( _tag_arr("0", sizeof( unsigned char), 2u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out));({ void* _temp176[ 0u]={}; Cyc_log( _tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof( unsigned char), 42u), _tag_arr( _temp176, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 31: _LL175: Cyc_Std_fputc(( int) Cyc_Lexing_lexeme_char(
lexbuf, 0),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); return Cyc_slurp(
lexbuf); default: _LL177:( lexbuf->refill_buff)( lexbuf); return Cyc_slurp_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp179=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp179[ 0]=({ struct Cyc_Lexing_Error_struct _temp180; _temp180.tag= Cyc_Lexing_Error;
_temp180.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp180;}); _temp179;}));} int Cyc_slurp( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_slurp_rec( lexbuf, 5);} int Cyc_slurp_string_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL181: Cyc_Std_fputc(( int)'"',( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return 0; case 1: _LL182: return 1; case 2: _LL183:
return 1; case 3: _LL184:({ struct Cyc_Std_String_pa_struct _temp187; _temp187.tag=
Cyc_Std_String_pa; _temp187.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{
void* _temp186[ 1u]={& _temp187}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp186, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 4: _LL185:({ struct Cyc_Std_String_pa_struct _temp190; _temp190.tag= Cyc_Std_String_pa;
_temp190.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp189[ 1u]={&
_temp190}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp189, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 5: _LL188:({ struct Cyc_Std_String_pa_struct
_temp193; _temp193.tag= Cyc_Std_String_pa; _temp193.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp192[ 1u]={& _temp193}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp192, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 6: _LL191:({ struct Cyc_Std_String_pa_struct _temp196; _temp196.tag= Cyc_Std_String_pa;
_temp196.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp195[ 1u]={&
_temp196}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp195, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 7: _LL194:({ struct Cyc_Std_String_pa_struct
_temp199; _temp199.tag= Cyc_Std_String_pa; _temp199.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp198[ 1u]={& _temp199}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp198, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 8: _LL197:({ struct Cyc_Std_String_pa_struct _temp202; _temp202.tag= Cyc_Std_String_pa;
_temp202.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp201[ 1u]={&
_temp202}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp201, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); default: _LL200:( lexbuf->refill_buff)(
lexbuf); return Cyc_slurp_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp204=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp204[ 0]=({ struct Cyc_Lexing_Error_struct
_temp205; _temp205.tag= Cyc_Lexing_Error; _temp205.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp205;}); _temp204;}));} int Cyc_slurp_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_slurp_string_rec( lexbuf, 6);} int
Cyc_asm_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL206: if( Cyc_parens_to_match == 
1){ return 0;} Cyc_parens_to_match --; return Cyc_asm( lexbuf); case 1: _LL207:
Cyc_parens_to_match ++; return Cyc_asm( lexbuf); case 2: _LL208: Cyc_asm_string(
lexbuf); return Cyc_asm( lexbuf); case 3: _LL209: Cyc_asm_comment( lexbuf);
return Cyc_asm( lexbuf); case 4: _LL210: return Cyc_asm( lexbuf); case 5: _LL211:
return 0; case 6: _LL212: return Cyc_asm( lexbuf); default: _LL213:( lexbuf->refill_buff)(
lexbuf); return Cyc_asm_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp215=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp215[ 0]=({ struct Cyc_Lexing_Error_struct
_temp216; _temp216.tag= Cyc_Lexing_Error; _temp216.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp216;}); _temp215;}));} int Cyc_asm( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_rec( lexbuf, 7);} int Cyc_asm_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL217: return 0; case 1: _LL218:
return 1; case 2: _LL219: return 1; case 3: _LL220: return Cyc_asm_string(
lexbuf); case 4: _LL221: return Cyc_asm_string( lexbuf); case 5: _LL222: return
Cyc_asm_string( lexbuf); case 6: _LL223: return Cyc_asm_string( lexbuf); case 7:
_LL224: return Cyc_asm_string( lexbuf); case 8: _LL225: return Cyc_asm_string(
lexbuf); default: _LL226:( lexbuf->refill_buff)( lexbuf); return Cyc_asm_string_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp228=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp228[ 0]=({ struct Cyc_Lexing_Error_struct _temp229; _temp229.tag= Cyc_Lexing_Error;
_temp229.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp229;}); _temp228;}));} int Cyc_asm_string( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_asm_string_rec( lexbuf, 8);} int Cyc_asm_comment_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL230: return 0; case 1: _LL231: return 1; case 2: _LL232:
return Cyc_asm_comment( lexbuf); default: _LL233:( lexbuf->refill_buff)( lexbuf);
return Cyc_asm_comment_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp235=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp235[ 0]=({ struct Cyc_Lexing_Error_struct
_temp236; _temp236.tag= Cyc_Lexing_Error; _temp236.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp236;}); _temp235;}));} int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_comment_rec( lexbuf, 9);}
struct _tuple9* Cyc_suck_line_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case
0: _LL237: Cyc_current_line= _tag_arr("#define ", sizeof( unsigned char), 9u);
Cyc_suck_macroname( lexbuf); return({ struct _tuple9* _temp239=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp239->f1= Cyc_current_line; _temp239->f2=(
struct _tagged_arr*) _check_null( Cyc_current_source); _temp239;}); case 1:
_LL238: return Cyc_suck_line( lexbuf); case 2: _LL240: return 0; default: _LL241:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_line_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp243=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp243[ 0]=({ struct Cyc_Lexing_Error_struct
_temp244; _temp244.tag= Cyc_Lexing_Error; _temp244.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp244;}); _temp243;}));} struct _tuple9* Cyc_suck_line(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_line_rec( lexbuf, 10);} int
Cyc_suck_macroname_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL245:
Cyc_current_source=({ struct _tagged_arr* _temp247=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp247[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp247;}); Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat(
Cyc_current_line,*(( struct _tagged_arr*) _check_null( Cyc_current_source)));
return Cyc_suck_restofline( lexbuf); default: _LL246:( lexbuf->refill_buff)(
lexbuf); return Cyc_suck_macroname_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp249=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp249[ 0]=({ struct Cyc_Lexing_Error_struct
_temp250; _temp250.tag= Cyc_Lexing_Error; _temp250.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp250;}); _temp249;}));} int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_macroname_rec( lexbuf, 11);}
int Cyc_suck_restofline_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL251:
Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat( Cyc_current_line,(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return 0; default: _LL252:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_restofline_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp254=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp254[ 0]=({ struct Cyc_Lexing_Error_struct
_temp255; _temp255.tag= Cyc_Lexing_Error; _temp255.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp255;}); _temp254;}));} int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_restofline_rec( lexbuf, 12);}
struct _tuple11* Cyc_spec_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL256:
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
_tuple11* _temp258=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11));
_temp258->f1= Cyc_current_header; _temp258->f2= Cyc_current_symbols; _temp258->f3=
Cyc_current_omit_symbols; _temp258->f4= Cyc_current_prologue; _temp258->f5= Cyc_current_epilogue;
_temp258->f6= Cyc_current_cstubs; _temp258->f7= Cyc_current_cycstubs; _temp258;});
case 1: _LL257: return Cyc_spec( lexbuf); case 2: _LL259: return Cyc_spec(
lexbuf); case 3: _LL260: return 0; case 4: _LL261:({ struct Cyc_Std_Int_pa_struct
_temp264; _temp264.tag= Cyc_Std_Int_pa; _temp264.f1=( int)(( unsigned int)(( int)
Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void* _temp263[ 1u]={& _temp264}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof( unsigned char), 67u), _tag_arr( _temp263, sizeof( void*), 1u));}});
return 0; default: _LL262:( lexbuf->refill_buff)( lexbuf); return Cyc_spec_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp266=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp266[ 0]=({ struct Cyc_Lexing_Error_struct _temp267; _temp267.tag= Cyc_Lexing_Error;
_temp267.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp267;}); _temp266;}));} struct _tuple11* Cyc_spec( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_spec_rec( lexbuf, 13);} int Cyc_commands_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL268: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols( lexbuf)){
return 1;} Cyc_current_symbols=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols, Cyc_current_symbols);
return Cyc_commands( lexbuf); case 1: _LL269: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols(
lexbuf)){ return 1;} Cyc_current_omit_symbols=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols,
Cyc_current_omit_symbols); return Cyc_commands( lexbuf); case 2: _LL270: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp273=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp273->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp273->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp273;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp272=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp272->hd=( void*) x; _temp272->tl= Cyc_current_prologue;
_temp272;}); return Cyc_commands( lexbuf);} case 3: _LL271: { struct _tagged_arr
_temp275= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp275,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("prologue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp275, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp275, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp275; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp276= Cyc_Std_substring(( struct _tagged_arr) _temp275, 0,(
unsigned int)(( t.curr -  _temp275.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp278=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp278->f1=( struct
_tagged_arr) _temp276; _temp278->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp278;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp277=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp277->hd=( void*) x; _temp277->tl= Cyc_current_prologue;
_temp277;}); return Cyc_commands( lexbuf);}}}} case 4: _LL274: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp281=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp281->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp281->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp281;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp280=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp280->hd=( void*) x; _temp280->tl= Cyc_current_epilogue;
_temp280;}); return Cyc_commands( lexbuf);} case 5: _LL279: { struct _tagged_arr
_temp283= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp283,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("epilogue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp283, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp283, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp283; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp284= Cyc_Std_substring(( struct _tagged_arr) _temp283, 0,(
unsigned int)(( t.curr -  _temp283.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp286=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp286->f1=( struct
_tagged_arr) _temp284; _temp286->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp286;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp285=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp285->hd=( void*) x; _temp285->tl= Cyc_current_epilogue;
_temp285;}); return Cyc_commands( lexbuf);}}}} case 6: _LL282: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp289=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp289->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp289->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp289;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp288=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp288->hd=( void*) x; _temp288->tl= Cyc_current_cstubs;
_temp288;}); return Cyc_commands( lexbuf);} case 7: _LL287: { struct _tagged_arr
_temp291= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp291,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cstub", sizeof(
unsigned char), 6u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp291, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp291, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp291; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp292= Cyc_Std_substring(( struct _tagged_arr) _temp291, 0,(
unsigned int)(( t.curr -  _temp291.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp294=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp294->f1=( struct
_tagged_arr) _temp292; _temp294->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp294;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp293=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp293->hd=( void*) x; _temp293->tl= Cyc_current_cstubs;
_temp293;}); return Cyc_commands( lexbuf);}}}} case 8: _LL290: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp297=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp297->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp297->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp297;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp296=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp296->hd=( void*) x; _temp296->tl= Cyc_current_cycstubs;
_temp296;}); return Cyc_commands( lexbuf);} case 9: _LL295: { struct _tagged_arr
_temp299= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp299,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cycstub", sizeof(
unsigned char), 8u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp299, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp299, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp299; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp300= Cyc_Std_substring(( struct _tagged_arr) _temp299, 0,(
unsigned int)(( t.curr -  _temp299.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp302=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp302->f1=( struct
_tagged_arr) _temp300; _temp302->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp302;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp301=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp301->hd=( void*) x; _temp301->tl= Cyc_current_cycstubs;
_temp301;}); return Cyc_commands( lexbuf);}}}} case 10: _LL298: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tagged_arr* x=({ struct _tagged_arr* _temp305=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp305[ 0]=(
struct _tagged_arr) Cyc_Buffer_contents(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf));
_temp305;}); Cyc_current_cpp=({ struct Cyc_List_List* _temp304=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp304->hd=( void*) x; _temp304->tl=
Cyc_current_cpp; _temp304;}); return Cyc_commands( lexbuf);} case 11: _LL303:
return Cyc_commands( lexbuf); case 12: _LL306: return Cyc_commands( lexbuf);
case 13: _LL307: return 0; case 14: _LL308: return 0; case 15: _LL309:({ struct
Cyc_Std_Int_pa_struct _temp312; _temp312.tag= Cyc_Std_Int_pa; _temp312.f1=( int)((
unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void* _temp311[ 1u]={&
_temp312}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof( unsigned char), 58u), _tag_arr( _temp311, sizeof( void*), 1u));}});
return 1; default: _LL310:( lexbuf->refill_buff)( lexbuf); return Cyc_commands_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp314=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp314[ 0]=({ struct Cyc_Lexing_Error_struct _temp315; _temp315.tag= Cyc_Lexing_Error;
_temp315.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp315;}); _temp314;}));} int Cyc_commands( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_commands_rec( lexbuf, 14);} int Cyc_snarfsymbols_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL316: Cyc_snarfed_symbols=({ struct Cyc_List_List* _temp318=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp318->hd=(
void*)({ struct _tagged_arr* _temp319=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp319[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp319;}); _temp318->tl= Cyc_snarfed_symbols; _temp318;}); return Cyc_snarfsymbols(
lexbuf); case 1: _LL317: return Cyc_snarfsymbols( lexbuf); case 2: _LL320:
return 0; case 3: _LL321:({ void* _temp323[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n", sizeof( unsigned char),
44u), _tag_arr( _temp323, sizeof( void*), 0u));}); return 1; case 4: _LL322:({
struct Cyc_Std_Int_pa_struct _temp326; _temp326.tag= Cyc_Std_Int_pa; _temp326.f1=(
int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void*
_temp325[ 1u]={& _temp326}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof( unsigned char), 57u), _tag_arr( _temp325, sizeof( void*), 1u));}});
return 1; default: _LL324:( lexbuf->refill_buff)( lexbuf); return Cyc_snarfsymbols_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp328=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp328[ 0]=({ struct Cyc_Lexing_Error_struct _temp329; _temp329.tag= Cyc_Lexing_Error;
_temp329.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp329;}); _temp328;}));} int Cyc_snarfsymbols( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_snarfsymbols_rec( lexbuf, 15);} int Cyc_block_rec( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate,
lexbuf); switch( lexstate){ case 0: _LL330: if( Cyc_braces_to_match ==  1){
return 0;} Cyc_braces_to_match --; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'}'); return Cyc_block( lexbuf); case 1: _LL331: Cyc_braces_to_match
++; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'{');
return Cyc_block( lexbuf); case 2: _LL332: Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'"'); Cyc_block_string( lexbuf); return Cyc_block(
lexbuf); case 3: _LL333: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf), _tag_arr("/*", sizeof( unsigned char), 3u)); Cyc_block_comment(
lexbuf); return Cyc_block( lexbuf); case 4: _LL334: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block( lexbuf); case 5: _LL335: return 0; case 6: _LL336:
Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf), Cyc_Lexing_lexeme_char(
lexbuf, 0)); return Cyc_block( lexbuf); default: _LL337:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp339=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp339[ 0]=({ struct Cyc_Lexing_Error_struct
_temp340; _temp340.tag= Cyc_Lexing_Error; _temp340.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp340;}); _temp339;}));} int Cyc_block( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_rec( lexbuf, 16);} int Cyc_block_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL341: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'"'); return 0; case 1: _LL342:
return 1; case 2: _LL343: return 1; case 3: _LL344: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 4: _LL345: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 5: _LL346: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 6: _LL347: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 7: _LL348: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 8: _LL349: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); default: _LL350:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp352=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp352[ 0]=({ struct Cyc_Lexing_Error_struct
_temp353; _temp353.tag= Cyc_Lexing_Error; _temp353.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp353;}); _temp352;}));} int Cyc_block_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_string_rec( lexbuf, 17);}
int Cyc_block_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL354:
Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),
_tag_arr("*/", sizeof( unsigned char), 3u)); return 0; case 1: _LL355: return 1;
case 2: _LL356: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return Cyc_block_comment(
lexbuf); default: _LL357:( lexbuf->refill_buff)( lexbuf); return Cyc_block_comment_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp359=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp359[ 0]=({ struct Cyc_Lexing_Error_struct _temp360; _temp360.tag= Cyc_Lexing_Error;
_temp360.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp360;}); _temp359;}));} int Cyc_block_comment( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_block_comment_rec( lexbuf, 18);} static void Cyc_pr( struct
_tagged_arr* sptr){({ struct Cyc_Std_String_pa_struct _temp362; _temp362.tag=
Cyc_Std_String_pa; _temp362.f1=( struct _tagged_arr)* sptr;{ void* _temp361[ 1u]={&
_temp362}; Cyc_Std_printf( _tag_arr(" %s\n", sizeof( unsigned char), 5u),
_tag_arr( _temp361, sizeof( void*), 1u));}});} extern void Cyc_scan_type( void*
t); struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
void Cyc_scan_exp( struct Cyc_Absyn_Exp* e){ void* _temp363=( void*)(( struct
Cyc_Absyn_Exp*) _check_null( e))->r; struct _tuple0* _temp441; struct _tuple0
_temp443; struct _tagged_arr* _temp444; struct _tuple0* _temp446; struct _tuple0
_temp448; struct _tagged_arr* _temp449; struct Cyc_List_List* _temp451; struct
Cyc_Absyn_Exp* _temp453; struct Cyc_Absyn_Exp* _temp455; struct Cyc_Absyn_Exp*
_temp457; struct Cyc_Absyn_Exp* _temp459; struct Cyc_Absyn_Exp* _temp461; struct
Cyc_Absyn_Exp* _temp463; struct Cyc_Absyn_Exp* _temp465; struct Cyc_Absyn_Exp*
_temp467; struct Cyc_Absyn_Exp* _temp469; struct Cyc_Absyn_Exp* _temp471; struct
Cyc_Absyn_Exp* _temp473; struct Cyc_Absyn_Exp* _temp475; struct Cyc_Absyn_Exp*
_temp477; struct Cyc_List_List* _temp479; struct Cyc_Absyn_Exp* _temp481; struct
Cyc_List_List* _temp483; struct Cyc_Absyn_Exp* _temp485; struct Cyc_Absyn_Exp*
_temp487; void* _temp489; struct Cyc_Absyn_MallocInfo _temp491; struct Cyc_Absyn_Exp*
_temp493; void** _temp495; struct Cyc_Absyn_Exp* _temp497; int _temp499; void*
_temp501; struct _tagged_arr* _temp503; struct Cyc_Absyn_Exp* _temp505; struct
_tagged_arr* _temp507; struct Cyc_Absyn_Exp* _temp509; void* _temp511; void*
_temp513; struct Cyc_List_List* _temp515; _LL365: if(*(( int*) _temp363) ==  Cyc_Absyn_Var_e){
_LL442: _temp441=(( struct Cyc_Absyn_Var_e_struct*) _temp363)->f1; _temp443=*
_temp441; _LL445: _temp444= _temp443.f2; goto _LL366;} else{ goto _LL367;}
_LL367: if(*(( int*) _temp363) ==  Cyc_Absyn_UnknownId_e){ _LL447: _temp446=((
struct Cyc_Absyn_UnknownId_e_struct*) _temp363)->f1; _temp448=* _temp446; _LL450:
_temp449= _temp448.f2; goto _LL368;} else{ goto _LL369;} _LL369: if(*(( int*)
_temp363) ==  Cyc_Absyn_Primop_e){ _LL452: _temp451=(( struct Cyc_Absyn_Primop_e_struct*)
_temp363)->f2; goto _LL370;} else{ goto _LL371;} _LL371: if(*(( int*) _temp363)
==  Cyc_Absyn_Subscript_e){ _LL456: _temp455=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp363)->f1; goto _LL454; _LL454: _temp453=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp363)->f2; goto _LL372;} else{ goto _LL373;} _LL373: if(*(( int*) _temp363)
==  Cyc_Absyn_SeqExp_e){ _LL460: _temp459=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp363)->f1; goto _LL458; _LL458: _temp457=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp363)->f2; goto _LL374;} else{ goto _LL375;} _LL375: if(*(( int*) _temp363)
==  Cyc_Absyn_AssignOp_e){ _LL464: _temp463=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp363)->f1; goto _LL462; _LL462: _temp461=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp363)->f3; goto _LL376;} else{ goto _LL377;} _LL377: if(*(( int*) _temp363)
==  Cyc_Absyn_Deref_e){ _LL466: _temp465=(( struct Cyc_Absyn_Deref_e_struct*)
_temp363)->f1; goto _LL378;} else{ goto _LL379;} _LL379: if(*(( int*) _temp363)
==  Cyc_Absyn_Sizeofexp_e){ _LL468: _temp467=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp363)->f1; goto _LL380;} else{ goto _LL381;} _LL381: if(*(( int*) _temp363)
==  Cyc_Absyn_Address_e){ _LL470: _temp469=(( struct Cyc_Absyn_Address_e_struct*)
_temp363)->f1; goto _LL382;} else{ goto _LL383;} _LL383: if(*(( int*) _temp363)
==  Cyc_Absyn_Increment_e){ _LL472: _temp471=(( struct Cyc_Absyn_Increment_e_struct*)
_temp363)->f1; goto _LL384;} else{ goto _LL385;} _LL385: if(*(( int*) _temp363)
==  Cyc_Absyn_Conditional_e){ _LL478: _temp477=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp363)->f1; goto _LL476; _LL476: _temp475=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp363)->f2; goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp363)->f3; goto _LL386;} else{ goto _LL387;} _LL387: if(*(( int*) _temp363)
==  Cyc_Absyn_FnCall_e){ _LL482: _temp481=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp363)->f1; goto _LL480; _LL480: _temp479=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp363)->f2; goto _LL388;} else{ goto _LL389;} _LL389: if(*(( int*) _temp363)
==  Cyc_Absyn_UnknownCall_e){ _LL486: _temp485=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp363)->f1; goto _LL484; _LL484: _temp483=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp363)->f2; goto _LL390;} else{ goto _LL391;} _LL391: if(*(( int*) _temp363)
==  Cyc_Absyn_Cast_e){ _LL490: _temp489=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp363)->f1; goto _LL488; _LL488: _temp487=(( struct Cyc_Absyn_Cast_e_struct*)
_temp363)->f2; goto _LL392;} else{ goto _LL393;} _LL393: if(*(( int*) _temp363)
==  Cyc_Absyn_Malloc_e){ _LL492: _temp491=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp363)->f1; _LL500: _temp499= _temp491.is_calloc; goto _LL498; _LL498:
_temp497= _temp491.rgn; goto _LL496; _LL496: _temp495= _temp491.elt_type; goto
_LL494; _LL494: _temp493= _temp491.num_elts; goto _LL394;} else{ goto _LL395;}
_LL395: if(*(( int*) _temp363) ==  Cyc_Absyn_Sizeoftyp_e){ _LL502: _temp501=(
void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp363)->f1; goto _LL396;}
else{ goto _LL397;} _LL397: if(*(( int*) _temp363) ==  Cyc_Absyn_StructMember_e){
_LL506: _temp505=(( struct Cyc_Absyn_StructMember_e_struct*) _temp363)->f1; goto
_LL504; _LL504: _temp503=(( struct Cyc_Absyn_StructMember_e_struct*) _temp363)->f2;
goto _LL398;} else{ goto _LL399;} _LL399: if(*(( int*) _temp363) ==  Cyc_Absyn_StructArrow_e){
_LL510: _temp509=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp363)->f1; goto
_LL508; _LL508: _temp507=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp363)->f2;
goto _LL400;} else{ goto _LL401;} _LL401: if(*(( int*) _temp363) ==  Cyc_Absyn_Offsetof_e){
_LL514: _temp513=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp363)->f1;
goto _LL512; _LL512: _temp511=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp363)->f2; goto _LL402;} else{ goto _LL403;} _LL403: if(*(( int*) _temp363)
==  Cyc_Absyn_Const_e){ goto _LL404;} else{ goto _LL405;} _LL405: if(*(( int*)
_temp363) ==  Cyc_Absyn_UnresolvedMem_e){ _LL516: _temp515=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp363)->f2; goto _LL406;} else{ goto _LL407;} _LL407: if(*(( int*) _temp363)
==  Cyc_Absyn_StmtExp_e){ goto _LL408;} else{ goto _LL409;} _LL409: if(*(( int*)
_temp363) ==  Cyc_Absyn_Throw_e){ goto _LL410;} else{ goto _LL411;} _LL411: if(*((
int*) _temp363) ==  Cyc_Absyn_NoInstantiate_e){ goto _LL412;} else{ goto _LL413;}
_LL413: if(*(( int*) _temp363) ==  Cyc_Absyn_Instantiate_e){ goto _LL414;} else{
goto _LL415;} _LL415: if(*(( int*) _temp363) ==  Cyc_Absyn_New_e){ goto _LL416;}
else{ goto _LL417;} _LL417: if(*(( int*) _temp363) ==  Cyc_Absyn_Gentyp_e){ goto
_LL418;} else{ goto _LL419;} _LL419: if(*(( int*) _temp363) ==  Cyc_Absyn_Tuple_e){
goto _LL420;} else{ goto _LL421;} _LL421: if(*(( int*) _temp363) ==  Cyc_Absyn_CompoundLit_e){
goto _LL422;} else{ goto _LL423;} _LL423: if(*(( int*) _temp363) ==  Cyc_Absyn_Array_e){
goto _LL424;} else{ goto _LL425;} _LL425: if(*(( int*) _temp363) ==  Cyc_Absyn_Comprehension_e){
goto _LL426;} else{ goto _LL427;} _LL427: if(*(( int*) _temp363) ==  Cyc_Absyn_Struct_e){
goto _LL428;} else{ goto _LL429;} _LL429: if(*(( int*) _temp363) ==  Cyc_Absyn_AnonStruct_e){
goto _LL430;} else{ goto _LL431;} _LL431: if(*(( int*) _temp363) ==  Cyc_Absyn_Tunion_e){
goto _LL432;} else{ goto _LL433;} _LL433: if(*(( int*) _temp363) ==  Cyc_Absyn_Enum_e){
goto _LL434;} else{ goto _LL435;} _LL435: if(*(( int*) _temp363) ==  Cyc_Absyn_AnonEnum_e){
goto _LL436;} else{ goto _LL437;} _LL437: if(*(( int*) _temp363) ==  Cyc_Absyn_Codegen_e){
goto _LL438;} else{ goto _LL439;} _LL439: if(*(( int*) _temp363) ==  Cyc_Absyn_Fill_e){
goto _LL440;} else{ goto _LL364;} _LL366: _temp449= _temp444; goto _LL368;
_LL368: Cyc_add_target( _temp449); return; _LL370: for( 0; _temp451 !=  0;
_temp451= _temp451->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_temp451->hd));} return; _LL372: _temp459= _temp455; _temp457= _temp453; goto
_LL374; _LL374: _temp463= _temp459; _temp461= _temp457; goto _LL376; _LL376: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp463); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp461);
return; _LL378: _temp467= _temp465; goto _LL380; _LL380: _temp469= _temp467;
goto _LL382; _LL382: _temp471= _temp469; goto _LL384; _LL384: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp471); return; _LL386: Cyc_scan_exp(( struct Cyc_Absyn_Exp*)
_temp477); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp475); Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp473); return; _LL388: _temp485= _temp481; _temp483=
_temp479; goto _LL390; _LL390: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp485);
for( 0; _temp483 !=  0; _temp483= _temp483->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _temp483->hd));} return; _LL392: Cyc_scan_type( _temp489);
Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp487); return; _LL394: if( _temp497 != 
0){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp497)));} if( _temp495 !=  0){ Cyc_scan_type(* _temp495);} Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp493); return; _LL396: Cyc_scan_type( _temp501);
return; _LL398: _temp509= _temp505; _temp507= _temp503; goto _LL400; _LL400: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp509); Cyc_add_target( _temp507); return; _LL402: Cyc_scan_type(
_temp513);{ void* _temp517= _temp511; struct _tagged_arr* _temp523; _LL519: if(*((
int*) _temp517) ==  Cyc_Absyn_StructField){ _LL524: _temp523=(( struct Cyc_Absyn_StructField_struct*)
_temp517)->f1; goto _LL520;} else{ goto _LL521;} _LL521: if(*(( int*) _temp517)
==  Cyc_Absyn_TupleIndex){ goto _LL522;} else{ goto _LL518;} _LL520: Cyc_add_target(
_temp523); goto _LL518; _LL522: goto _LL518; _LL518:;} return; _LL404: return;
_LL406: for( 0; _temp515 !=  0; _temp515= _temp515->tl){ struct _tuple12
_temp527; struct Cyc_Absyn_Exp* _temp528; struct _tuple12* _temp525=( struct
_tuple12*) _temp515->hd; _temp527=* _temp525; _LL529: _temp528= _temp527.f2;
goto _LL526; _LL526: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp528);} return;
_LL408:({ void* _temp530[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Stmt_e\n",
sizeof( unsigned char), 26u), _tag_arr( _temp530, sizeof( void*), 0u));}); exit(
1); return; _LL410:({ void* _temp531[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Throw_e\n", sizeof( unsigned char), 27u), _tag_arr(
_temp531, sizeof( void*), 0u));}); exit( 1); return; _LL412:({ void* _temp532[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected NoInstantiate_e\n",
sizeof( unsigned char), 35u), _tag_arr( _temp532, sizeof( void*), 0u));}); exit(
1); return; _LL414:({ void* _temp533[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Instantiate_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp533, sizeof( void*), 0u));}); exit( 1); return; _LL416:({ void*
_temp534[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected New_e\n",
sizeof( unsigned char), 25u), _tag_arr( _temp534, sizeof( void*), 0u));}); exit(
1); return; _LL418:({ void* _temp535[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Gentyp_e\n", sizeof( unsigned char), 28u), _tag_arr(
_temp535, sizeof( void*), 0u));}); exit( 1); return; _LL420:({ void* _temp536[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tuple_e\n", sizeof(
unsigned char), 27u), _tag_arr( _temp536, sizeof( void*), 0u));}); exit( 1);
return; _LL422:({ void* _temp537[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected CompoundLit_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp537, sizeof( void*), 0u));}); exit( 1); return; _LL424:({ void*
_temp538[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Array_e\n",
sizeof( unsigned char), 27u), _tag_arr( _temp538, sizeof( void*), 0u));}); exit(
1); return; _LL426:({ void* _temp539[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Comprehension_e\n", sizeof( unsigned char), 35u),
_tag_arr( _temp539, sizeof( void*), 0u));}); exit( 1); return; _LL428:({ void*
_temp540[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Struct_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp540, sizeof( void*), 0u));}); exit(
1); return; _LL430:({ void* _temp541[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonStruct_e\n", sizeof( unsigned char), 32u),
_tag_arr( _temp541, sizeof( void*), 0u));}); exit( 1); return; _LL432:({ void*
_temp542[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tunion_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp542, sizeof( void*), 0u));}); exit(
1); return; _LL434:({ void* _temp543[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Enum_e\n", sizeof( unsigned char), 26u), _tag_arr(
_temp543, sizeof( void*), 0u));}); exit( 1); return; _LL436:({ void* _temp544[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AnonEnum_e\n",
sizeof( unsigned char), 30u), _tag_arr( _temp544, sizeof( void*), 0u));}); exit(
1); return; _LL438:({ void* _temp545[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Codegen_e\n", sizeof( unsigned char), 29u), _tag_arr(
_temp545, sizeof( void*), 0u));}); exit( 1); return; _LL440:({ void* _temp546[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Fill_e\n", sizeof(
unsigned char), 26u), _tag_arr( _temp546, sizeof( void*), 0u));}); exit( 1);
return; _LL364:;} void Cyc_scan_exp_opt( struct Cyc_Absyn_Exp* eo){ if((
unsigned int) eo){ Cyc_scan_exp( eo);} return;} void Cyc_scan_type( void* t){
void* _temp547= t; struct Cyc_Absyn_PtrInfo _temp603; struct Cyc_Absyn_Exp*
_temp605; void* _temp607; struct Cyc_Absyn_FnInfo _temp609; struct Cyc_List_List*
_temp611; struct Cyc_List_List* _temp613; struct _tuple0* _temp615; struct
_tuple0* _temp617; struct _tuple0* _temp619; struct _tuple0 _temp621; struct
_tagged_arr* _temp622; struct _tuple0* _temp624; struct _tuple0 _temp626; struct
_tagged_arr* _temp627; struct _tuple0* _temp629; struct _tuple0 _temp631; struct
_tagged_arr* _temp632; struct _tuple0* _temp634; struct _tuple0 _temp636; struct
_tagged_arr* _temp637; _LL549: if( _temp547 == ( void*) Cyc_Absyn_VoidType){
goto _LL550;} else{ goto _LL551;} _LL551: if(( unsigned int) _temp547 >  3u?*((
int*) _temp547) ==  Cyc_Absyn_IntType: 0){ goto _LL552;} else{ goto _LL553;}
_LL553: if( _temp547 == ( void*) Cyc_Absyn_FloatType){ goto _LL554;} else{ goto
_LL555;} _LL555: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_DoubleType:
0){ goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int) _temp547 >  3u?*((
int*) _temp547) ==  Cyc_Absyn_PointerType: 0){ _LL604: _temp603=(( struct Cyc_Absyn_PointerType_struct*)
_temp547)->f1; goto _LL558;} else{ goto _LL559;} _LL559: if(( unsigned int)
_temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_ArrayType: 0){ _LL608: _temp607=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp547)->f1; goto _LL606; _LL606:
_temp605=(( struct Cyc_Absyn_ArrayType_struct*) _temp547)->f3; goto _LL560;}
else{ goto _LL561;} _LL561: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547)
==  Cyc_Absyn_FnType: 0){ _LL610: _temp609=(( struct Cyc_Absyn_FnType_struct*)
_temp547)->f1; goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int)
_temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_AnonStructType: 0){ _LL612:
_temp611=(( struct Cyc_Absyn_AnonStructType_struct*) _temp547)->f1; goto _LL564;}
else{ goto _LL565;} _LL565: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547)
==  Cyc_Absyn_AnonUnionType: 0){ _LL614: _temp613=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp547)->f1; goto _LL566;} else{ goto _LL567;} _LL567: if(( unsigned int)
_temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL568;}
else{ goto _LL569;} _LL569: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547)
==  Cyc_Absyn_StructType: 0){ _LL616: _temp615=(( struct Cyc_Absyn_StructType_struct*)
_temp547)->f1; if( _temp615 ==  0){ goto _LL570;} else{ goto _LL571;}} else{
goto _LL571;} _LL571: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) == 
Cyc_Absyn_UnionType: 0){ _LL618: _temp617=(( struct Cyc_Absyn_UnionType_struct*)
_temp547)->f1; if( _temp617 ==  0){ goto _LL572;} else{ goto _LL573;}} else{
goto _LL573;} _LL573: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) == 
Cyc_Absyn_StructType: 0){ _LL620: _temp619=(( struct Cyc_Absyn_StructType_struct*)
_temp547)->f1; if( _temp619 ==  0){ goto _LL575;} else{ _temp621=* _temp619;
_LL623: _temp622= _temp621.f2; goto _LL574;}} else{ goto _LL575;} _LL575: if((
unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_UnionType: 0){
_LL625: _temp624=(( struct Cyc_Absyn_UnionType_struct*) _temp547)->f1; if(
_temp624 ==  0){ goto _LL577;} else{ _temp626=* _temp624; _LL628: _temp627=
_temp626.f2; goto _LL576;}} else{ goto _LL577;} _LL577: if(( unsigned int)
_temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_EnumType: 0){ _LL630: _temp629=((
struct Cyc_Absyn_EnumType_struct*) _temp547)->f1; _temp631=* _temp629; _LL633:
_temp632= _temp631.f2; goto _LL578;} else{ goto _LL579;} _LL579: if((
unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_TypedefType: 0){
_LL635: _temp634=(( struct Cyc_Absyn_TypedefType_struct*) _temp547)->f1;
_temp636=* _temp634; _LL638: _temp637= _temp636.f2; goto _LL580;} else{ goto
_LL581;} _LL581: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_Evar:
0){ goto _LL582;} else{ goto _LL583;} _LL583: if(( unsigned int) _temp547 >  3u?*((
int*) _temp547) ==  Cyc_Absyn_VarType: 0){ goto _LL584;} else{ goto _LL585;}
_LL585: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_TunionType:
0){ goto _LL586;} else{ goto _LL587;} _LL587: if(( unsigned int) _temp547 >  3u?*((
int*) _temp547) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL588;} else{ goto
_LL589;} _LL589: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_TupleType:
0){ goto _LL590;} else{ goto _LL591;} _LL591: if(( unsigned int) _temp547 >  3u?*((
int*) _temp547) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL592;} else{ goto _LL593;}
_LL593: if( _temp547 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL594;} else{ goto
_LL595;} _LL595: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_AccessEff:
0){ goto _LL596;} else{ goto _LL597;} _LL597: if(( unsigned int) _temp547 >  3u?*((
int*) _temp547) ==  Cyc_Absyn_JoinEff: 0){ goto _LL598;} else{ goto _LL599;}
_LL599: if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_Absyn_RgnsEff:
0){ goto _LL600;} else{ goto _LL601;} _LL601: if(( unsigned int) _temp547 >  3u?*((
int*) _temp547) ==  Cyc_Absyn_SizeofType: 0){ goto _LL602;} else{ goto _LL548;}
_LL550: goto _LL552; _LL552: goto _LL554; _LL554: goto _LL556; _LL556: return;
_LL558: Cyc_scan_type(( void*) _temp603.elt_typ); return; _LL560: Cyc_scan_type(
_temp607); Cyc_scan_exp_opt( _temp605); return; _LL562: Cyc_scan_type(( void*)
_temp609.ret_typ);{ struct Cyc_List_List* _temp639= _temp609.args; for( 0;
_temp639 !=  0; _temp639= _temp639->tl){ struct _tuple1 _temp642; void* _temp643;
struct _tuple1* _temp640=( struct _tuple1*) _temp639->hd; _temp642=* _temp640;
_LL644: _temp643= _temp642.f3; goto _LL641; _LL641: Cyc_scan_type( _temp643);}}
if( _temp609.cyc_varargs !=  0){ Cyc_scan_type(( void*)( _temp609.cyc_varargs)->type);}
return; _LL564: _temp613= _temp611; goto _LL566; _LL566: for( 0; _temp613 !=  0;
_temp613= _temp613->tl){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_Structfield*)
_temp613->hd)->type); Cyc_scan_exp_opt((( struct Cyc_Absyn_Structfield*)
_temp613->hd)->width);} return; _LL568: return; _LL570: return; _LL572: return;
_LL574: _temp627= _temp622; goto _LL576; _LL576: _temp632= _temp627; goto _LL578;
_LL578: _temp637= _temp632; goto _LL580; _LL580: Cyc_add_target( _temp637);
return; _LL582:({ void* _temp645[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Evar\n", sizeof( unsigned char), 24u), _tag_arr(
_temp645, sizeof( void*), 0u));}); exit( 1); return; _LL584:({ void* _temp646[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected VarType\n", sizeof(
unsigned char), 27u), _tag_arr( _temp646, sizeof( void*), 0u));}); exit( 1);
return; _LL586:({ void* _temp647[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionType\n", sizeof( unsigned char), 30u),
_tag_arr( _temp647, sizeof( void*), 0u));}); exit( 1); return; _LL588:({ void*
_temp648[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected TunionFieldType\n",
sizeof( unsigned char), 35u), _tag_arr( _temp648, sizeof( void*), 0u));}); exit(
1); return; _LL590:({ void* _temp649[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n", sizeof( unsigned char), 29u), _tag_arr(
_temp649, sizeof( void*), 0u));}); exit( 1); return; _LL592:({ void* _temp650[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnHandleType\n",
sizeof( unsigned char), 33u), _tag_arr( _temp650, sizeof( void*), 0u));}); exit(
1); return; _LL594:({ void* _temp651[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected HeapRgn\n", sizeof( unsigned char), 27u), _tag_arr(
_temp651, sizeof( void*), 0u));}); exit( 1); return; _LL596:({ void* _temp652[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AccessEff\n",
sizeof( unsigned char), 29u), _tag_arr( _temp652, sizeof( void*), 0u));}); exit(
1); return; _LL598:({ void* _temp653[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected JoinEff\n", sizeof( unsigned char), 27u), _tag_arr(
_temp653, sizeof( void*), 0u));}); exit( 1); return; _LL600:({ void* _temp654[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnsEff\n", sizeof(
unsigned char), 27u), _tag_arr( _temp654, sizeof( void*), 0u));}); exit( 1);
return; _LL602:({ void* _temp655[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected sizeof_t\n", sizeof( unsigned char), 28u), _tag_arr(
_temp655, sizeof( void*), 0u));}); exit( 1); return; _LL548:;} struct _tuple13{
struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct _tuple8*
Cyc_scan_decl( struct Cyc_Absyn_Decl* d){ Cyc_current_targets=({ struct Cyc_Set_Set**
_temp656=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp656[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); _temp656;});{ void* _temp657=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp683; struct Cyc_Absyn_Fndecl*
_temp685; struct Cyc_Absyn_Structdecl* _temp687; struct Cyc_Absyn_Uniondecl*
_temp689; struct Cyc_Absyn_Enumdecl* _temp691; struct Cyc_Absyn_Typedefdecl*
_temp693; _LL659: if(*(( int*) _temp657) ==  Cyc_Absyn_Var_d){ _LL684: _temp683=((
struct Cyc_Absyn_Var_d_struct*) _temp657)->f1; goto _LL660;} else{ goto _LL661;}
_LL661: if(*(( int*) _temp657) ==  Cyc_Absyn_Fn_d){ _LL686: _temp685=(( struct
Cyc_Absyn_Fn_d_struct*) _temp657)->f1; goto _LL662;} else{ goto _LL663;} _LL663:
if(*(( int*) _temp657) ==  Cyc_Absyn_Struct_d){ _LL688: _temp687=(( struct Cyc_Absyn_Struct_d_struct*)
_temp657)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp657)
==  Cyc_Absyn_Union_d){ _LL690: _temp689=(( struct Cyc_Absyn_Union_d_struct*)
_temp657)->f1; goto _LL666;} else{ goto _LL667;} _LL667: if(*(( int*) _temp657)
==  Cyc_Absyn_Enum_d){ _LL692: _temp691=(( struct Cyc_Absyn_Enum_d_struct*)
_temp657)->f1; goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp657)
==  Cyc_Absyn_Typedef_d){ _LL694: _temp693=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp657)->f1; goto _LL670;} else{ goto _LL671;} _LL671: if(*(( int*) _temp657)
==  Cyc_Absyn_Let_d){ goto _LL672;} else{ goto _LL673;} _LL673: if(*(( int*)
_temp657) ==  Cyc_Absyn_Tunion_d){ goto _LL674;} else{ goto _LL675;} _LL675: if(*((
int*) _temp657) ==  Cyc_Absyn_Letv_d){ goto _LL676;} else{ goto _LL677;} _LL677:
if(*(( int*) _temp657) ==  Cyc_Absyn_Namespace_d){ goto _LL678;} else{ goto
_LL679;} _LL679: if(*(( int*) _temp657) ==  Cyc_Absyn_Using_d){ goto _LL680;}
else{ goto _LL681;} _LL681: if(*(( int*) _temp657) ==  Cyc_Absyn_ExternC_d){
goto _LL682;} else{ goto _LL658;} _LL660: { struct _tuple0 _temp697; struct
_tagged_arr* _temp698; struct _tuple0* _temp695= _temp683->name; _temp697=*
_temp695; _LL699: _temp698= _temp697.f2; goto _LL696; _LL696: Cyc_current_source=(
struct _tagged_arr*) _temp698; Cyc_scan_type(( void*) _temp683->type); Cyc_scan_exp_opt(
_temp683->initializer); goto _LL658;} _LL662: { struct _tuple0 _temp702; struct
_tagged_arr* _temp703; struct _tuple0* _temp700= _temp685->name; _temp702=*
_temp700; _LL704: _temp703= _temp702.f2; goto _LL701; _LL701: Cyc_current_source=(
struct _tagged_arr*) _temp703; Cyc_scan_type(( void*) _temp685->ret_type);{
struct Cyc_List_List* _temp705= _temp685->args; for( 0; _temp705 !=  0; _temp705=
_temp705->tl){ struct _tuple13 _temp708; void* _temp709; struct _tuple13*
_temp706=( struct _tuple13*) _temp705->hd; _temp708=* _temp706; _LL710: _temp709=
_temp708.f3; goto _LL707; _LL707: Cyc_scan_type( _temp709);}} if( _temp685->cyc_varargs
!=  0){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp685->cyc_varargs))->type);} if( _temp685->is_inline){({ void* _temp711[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: ignoring inline function\n",
sizeof( unsigned char), 35u), _tag_arr( _temp711, sizeof( void*), 0u));});} goto
_LL658;} _LL664: if( !(( unsigned int) _temp687->name)){ return 0;}{ struct
_tuple0 _temp714; struct _tagged_arr* _temp715; struct _tuple0* _temp712=(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp687->name))->v;
_temp714=* _temp712; _LL716: _temp715= _temp714.f2; goto _LL713; _LL713: Cyc_current_source=(
struct _tagged_arr*) _temp715; if(( unsigned int) _temp687->fields){{ struct Cyc_List_List*
_temp717=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp687->fields))->v;
for( 0; _temp717 !=  0; _temp717= _temp717->tl){ struct Cyc_Absyn_Structfield*
_temp718=( struct Cyc_Absyn_Structfield*) _temp717->hd; Cyc_scan_type(( void*)
_temp718->type); Cyc_scan_exp_opt( _temp718->width);}}{ struct Cyc_List_List*
_temp719=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp687->fields))->v;
for( 0; _temp719 !=  0; _temp719= _temp719->tl){;}}} goto _LL658;} _LL666: if( !((
unsigned int) _temp689->name)){ return 0;}{ struct _tuple0 _temp722; struct
_tagged_arr* _temp723; struct _tuple0* _temp720=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp689->name))->v; _temp722=* _temp720; _LL724: _temp723=
_temp722.f2; goto _LL721; _LL721: Cyc_current_source=( struct _tagged_arr*)
_temp723; if(( unsigned int) _temp689->fields){{ struct Cyc_List_List* _temp725=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp689->fields))->v;
for( 0; _temp725 !=  0; _temp725= _temp725->tl){ struct Cyc_Absyn_Structfield*
_temp726=( struct Cyc_Absyn_Structfield*) _temp725->hd; Cyc_add_target( _temp726->name);
Cyc_scan_type(( void*) _temp726->type); Cyc_scan_exp_opt( _temp726->width);}}{
struct Cyc_List_List* _temp727=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp689->fields))->v; for( 0; _temp727 !=  0; _temp727= _temp727->tl){;}}}
goto _LL658;} _LL668: { struct _tuple0 _temp730; struct _tagged_arr* _temp731;
struct _tuple0* _temp728= _temp691->name; _temp730=* _temp728; _LL732: _temp731=
_temp730.f2; goto _LL729; _LL729: Cyc_current_source=( struct _tagged_arr*)
_temp731; if(( unsigned int) _temp691->fields){{ struct Cyc_List_List* _temp733=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp691->fields))->v;
for( 0; _temp733 !=  0; _temp733= _temp733->tl){ struct Cyc_Absyn_Enumfield*
_temp734=( struct Cyc_Absyn_Enumfield*) _temp733->hd; Cyc_scan_exp_opt( _temp734->tag);}}{
struct Cyc_List_List* _temp735=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp691->fields))->v; for( 0; _temp735 !=  0; _temp735= _temp735->tl){;}}}
goto _LL658;} _LL670: { struct _tuple0 _temp738; struct _tagged_arr* _temp739;
struct _tuple0* _temp736= _temp693->name; _temp738=* _temp736; _LL740: _temp739=
_temp738.f2; goto _LL737; _LL737: Cyc_current_source=( struct _tagged_arr*)
_temp739; Cyc_scan_type(( void*) _temp693->defn); goto _LL658;} _LL672:({ void*
_temp741[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp741, sizeof( void*), 0u));}); exit(
1); return 0; _LL674:({ void* _temp742[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected tunion declaration\n", sizeof( unsigned char), 38u),
_tag_arr( _temp742, sizeof( void*), 0u));}); exit( 1); return 0; _LL676:({ void*
_temp743[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp743, sizeof( void*), 0u));}); exit(
1); return 0; _LL678:({ void* _temp744[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected namespace declaration\n", sizeof( unsigned char), 41u),
_tag_arr( _temp744, sizeof( void*), 0u));}); exit( 1); return 0; _LL680:({ void*
_temp745[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected using declaration\n",
sizeof( unsigned char), 37u), _tag_arr( _temp745, sizeof( void*), 0u));}); exit(
1); return 0; _LL682:({ void* _temp746[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n", sizeof( unsigned char),
42u), _tag_arr( _temp746, sizeof( void*), 0u));}); exit( 1); return 0; _LL658:;}
return({ struct _tuple8* _temp747=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp747->f1=( struct _tagged_arr*) _check_null( Cyc_current_source);
_temp747->f2=*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets));
_temp747;});} struct Cyc_Hashtable_Table* Cyc_new_deps(){ return(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), int(* hash)(
struct _tagged_arr*))) Cyc_Hashtable_create)( 107, Cyc_Std_strptrcmp, Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set* Cyc_find( struct Cyc_Hashtable_Table* t, struct _tagged_arr*
x){ struct _handler_cons _temp748; _push_handler(& _temp748);{ int _temp750= 0;
if( setjmp( _temp748.handler)){ _temp750= 1;} if( ! _temp750){{ struct Cyc_Set_Set*
_temp751=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key)) Cyc_Hashtable_lookup)( t, x); _npop_handler( 0u); return
_temp751;}; _pop_handler();} else{ void* _temp749=( void*) _exn_thrown; void*
_temp753= _temp749; _LL755: if( _temp753 ==  Cyc_Core_Not_found){ goto _LL756;}
else{ goto _LL757;} _LL757: goto _LL758; _LL756: return(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_LL758:( void) _throw( _temp753); _LL754:;}}} struct Cyc_Set_Set* Cyc_reachable(
struct Cyc_List_List* init, struct Cyc_Hashtable_Table* t){ struct Cyc_Set_Set*
emptyset=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); struct Cyc_Set_Set* curr;
for( curr= emptyset; init !=  0; init= init->tl){ curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( curr,( struct
_tagged_arr*) init->hd);}{ struct Cyc_Set_Set* _temp759= curr; while((( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp759) >  0) { struct Cyc_Set_Set*
_temp760= emptyset;{ struct Cyc_List_List* _temp761=(( struct Cyc_List_List*(*)(
struct Cyc_Set_Set* s)) Cyc_Set_elements)( _temp759); for( 0; _temp761 !=  0;
_temp761= _temp761->tl){ _temp760=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( _temp760, Cyc_find( t,( struct
_tagged_arr*) _temp761->hd));}} _temp759=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_diff)( _temp760, curr); curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( curr,
_temp759);} return curr;}} int Cyc_force_directory( struct _tagged_arr d){ int
_temp762=({ int _temp765[ 0u]; Cyc_Std_open( d, 0, _tag_arr( _temp765, sizeof(
int), 0u));}); if( _temp762 ==  - 1){ if( Cyc_Std_mkdir( d, 448) ==  - 1){({
struct Cyc_Std_String_pa_struct _temp764; _temp764.tag= Cyc_Std_String_pa;
_temp764.f1=( struct _tagged_arr) d;{ void* _temp763[ 1u]={& _temp764}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n", sizeof(
unsigned char), 38u), _tag_arr( _temp763, sizeof( void*), 1u));}}); return 1;}}
else{ close( _temp762);} return 0;} int Cyc_force_directory_prefixes( struct
_tagged_arr file){ struct _tagged_arr _temp766= Cyc_Std_strdup( file); struct
Cyc_List_List* _temp767= 0; while( 1) { _temp766= Cyc_Filename_dirname(( struct
_tagged_arr) _temp766); if( _get_arr_size( _temp766, sizeof( unsigned char)) == 
0){ break;} _temp767=({ struct Cyc_List_List* _temp768=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp768->hd=( void*)({ struct
_tagged_arr* _temp769=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp769[ 0]=( struct _tagged_arr) _temp766; _temp769;});
_temp768->tl= _temp767; _temp768;});} for( 0; _temp767 !=  0; _temp767= _temp767->tl){
if( Cyc_force_directory(*(( struct _tagged_arr*) _temp767->hd))){ return 1;}}
return 0;} int Cyc_process_file( struct _tagged_arr filename, struct _tagged_arr
dir, struct Cyc_List_List* start_symbols, struct Cyc_List_List* omit_symbols,
struct Cyc_List_List* prologue, struct Cyc_List_List* epilogue, struct Cyc_List_List*
cstubs, struct Cyc_List_List* cycstubs){ struct Cyc_Std___sFILE* maybe; struct
Cyc_Std___sFILE* in_file; struct Cyc_Std___sFILE* out_file;({ struct Cyc_Std_String_pa_struct
_temp771; _temp771.tag= Cyc_Std_String_pa; _temp771.f1=( struct _tagged_arr)
filename;{ void* _temp770[ 1u]={& _temp771}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("********************************* %s...\n", sizeof( unsigned char), 41u),
_tag_arr( _temp770, sizeof( void*), 1u));}});{ struct _tagged_arr buf= _tag_arr(({
unsigned int _temp1083=( unsigned int) 1024; unsigned char* _temp1084=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char),
_temp1083));{ unsigned int _temp1085= _temp1083; unsigned int i; for( i= 0; i < 
_temp1085; i ++){ _temp1084[ i]='\000';}}; _temp1084;}), sizeof( unsigned char),(
unsigned int) 1024); struct _tagged_arr _temp772= Cyc_Std_getcwd( buf,
_get_arr_size( buf, sizeof( unsigned char))); if( Cyc_Std_chdir( dir)){({ struct
Cyc_Std_String_pa_struct _temp774; _temp774.tag= Cyc_Std_String_pa; _temp774.f1=(
struct _tagged_arr) dir;{ void* _temp773[ 1u]={& _temp774}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: can't change directory to %s\n", sizeof( unsigned char), 37u),
_tag_arr( _temp773, sizeof( void*), 1u));}}); return 1;}({ struct Cyc_Std_String_pa_struct
_temp776; _temp776.tag= Cyc_Std_String_pa; _temp776.f1=( struct _tagged_arr)
filename;{ void* _temp775[ 1u]={& _temp776}; Cyc_log( _tag_arr("\n%s:\n",
sizeof( unsigned char), 6u), _tag_arr( _temp775, sizeof( void*), 1u));}});{
struct _tagged_arr _temp777= Cyc_Filename_basename( filename); struct
_tagged_arr _temp778= Cyc_Filename_chop_extension(( struct _tagged_arr) _temp777);
struct _tagged_arr _temp779= Cyc_Std_strconcat(( struct _tagged_arr) _temp778,
_tag_arr(".iI", sizeof( unsigned char), 4u)); struct _tagged_arr _temp780= Cyc_Std_strconcat((
struct _tagged_arr) _temp778, _tag_arr(".i", sizeof( unsigned char), 3u));
struct _tagged_arr _temp781= Cyc_Std_strconcat(( struct _tagged_arr) _temp778,
_tag_arr(".iX", sizeof( unsigned char), 4u)); struct _tagged_arr _temp782= Cyc_Filename_dirname(
filename); struct _tagged_arr _temp783= _get_arr_size( _temp782, sizeof(
unsigned char)) ==  0?({ struct Cyc_Std_String_pa_struct _temp1080; _temp1080.tag=
Cyc_Std_String_pa; _temp1080.f1=( struct _tagged_arr) _temp777;{ void* _temp1079[
1u]={& _temp1080}; Cyc_Std_aprintf( _tag_arr("c%s", sizeof( unsigned char), 4u),
_tag_arr( _temp1079, sizeof( void*), 1u));}}): Cyc_Filename_concat(( struct
_tagged_arr) _temp782,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1082; _temp1082.tag= Cyc_Std_String_pa; _temp1082.f1=( struct _tagged_arr)
_temp777;{ void* _temp1081[ 1u]={& _temp1082}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp1081, sizeof( void*), 1u));}})); if(
Cyc_force_directory_prefixes( filename)){ return 1;}{ struct Cyc_Hashtable_Table*
t= Cyc_new_deps(); struct _tagged_arr _temp784= Cstring_to_string( Ccomp); maybe=
Cyc_Std_fopen(( struct _tagged_arr) _temp779, _tag_arr("w", sizeof(
unsigned char), 2u)); if( !(( unsigned int) maybe)){({ struct Cyc_Std_String_pa_struct
_temp786; _temp786.tag= Cyc_Std_String_pa; _temp786.f1=( struct _tagged_arr)
_temp779;{ void* _temp785[ 1u]={& _temp786}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n", sizeof( unsigned char), 33u),
_tag_arr( _temp785, sizeof( void*), 1u));}}); return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe); Cyc_current_cpp=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_current_cpp);{ struct Cyc_List_List* _temp787= Cyc_current_cpp;
for( 0; _temp787 !=  0; _temp787= _temp787->tl){ Cyc_Std_fputs(*(( struct
_tagged_arr*) _temp787->hd), out_file);}}({ struct Cyc_Std_String_pa_struct
_temp789; _temp789.tag= Cyc_Std_String_pa; _temp789.f1=( struct _tagged_arr)
filename;{ void* _temp788[ 1u]={& _temp789}; Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\n",
sizeof( unsigned char), 15u), _tag_arr( _temp788, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file);{ struct _tagged_arr _temp790=({ struct Cyc_Std_String_pa_struct
_temp1078; _temp1078.tag= Cyc_Std_String_pa; _temp1078.f1=( struct _tagged_arr)
_temp779;{ struct Cyc_Std_String_pa_struct _temp1077; _temp1077.tag= Cyc_Std_String_pa;
_temp1077.f1=( struct _tagged_arr) _temp780;{ struct Cyc_Std_String_pa_struct
_temp1076; _temp1076.tag= Cyc_Std_String_pa; _temp1076.f1=( struct _tagged_arr)
_temp784;{ void* _temp1075[ 3u]={& _temp1076,& _temp1077,& _temp1078}; Cyc_Std_aprintf(
_tag_arr("%s -E -dM -o %s %s", sizeof( unsigned char), 19u), _tag_arr( _temp1075,
sizeof( void*), 3u));}}}}); if( Cyc_Std_system(( struct _tagged_arr) _temp790)
!=  0){ Cyc_Std_remove(( struct _tagged_arr) _temp779); maybe= Cyc_Std_fopen(
filename, _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int)
maybe)){({ struct Cyc_Std_String_pa_struct _temp792; _temp792.tag= Cyc_Std_String_pa;
_temp792.f1=( struct _tagged_arr) filename;{ void* _temp791[ 1u]={& _temp792};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create file %s\n",
sizeof( unsigned char), 33u), _tag_arr( _temp791, sizeof( void*), 1u));}});
return 1;} out_file=( struct Cyc_Std___sFILE*) _check_null( maybe);({ struct Cyc_Std_String_pa_struct
_temp794; _temp794.tag= Cyc_Std_String_pa; _temp794.f1=( struct _tagged_arr)
filename;{ void* _temp793[ 1u]={& _temp794}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp793, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp783, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){({ struct Cyc_Std_String_pa_struct
_temp796; _temp796.tag= Cyc_Std_String_pa; _temp796.f1=( struct _tagged_arr)
_temp783;{ void* _temp795[ 1u]={& _temp796}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n", sizeof( unsigned char), 33u),
_tag_arr( _temp795, sizeof( void*), 1u));}}); return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp798; _temp798.tag=
Cyc_Std_String_pa; _temp798.f1=( struct _tagged_arr) _temp783;{ void* _temp797[
1u]={& _temp798}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp797, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file);({ struct Cyc_Std_String_pa_struct _temp800; _temp800.tag= Cyc_Std_String_pa;
_temp800.f1=( struct _tagged_arr) filename;{ void* _temp799[ 1u]={& _temp800};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: %s will not be supported on this platform\n",
sizeof( unsigned char), 52u), _tag_arr( _temp799, sizeof( void*), 1u));}});({
void* _temp801[ 0u]={}; Cyc_log( _tag_arr("Not supported on this platform\n",
sizeof( unsigned char), 32u), _tag_arr( _temp801, sizeof( void*), 0u));}); if(
Cyc_Std_chdir(( struct _tagged_arr) _temp772)){({ struct Cyc_Std_String_pa_struct
_temp803; _temp803.tag= Cyc_Std_String_pa; _temp803.f1=( struct _tagged_arr)
_temp772;{ void* _temp802[ 1u]={& _temp803}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n", sizeof( unsigned char), 41u),
_tag_arr( _temp802, sizeof( void*), 1u));}}); return 1;} return 0;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp780, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe);{ struct Cyc_Lexing_lexbuf* _temp804= Cyc_Lexing_from_file( in_file);
struct _tuple8* entry; while(( entry=(( struct _tuple8*(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_line)( _temp804)) !=  0) { struct _tuple8 _temp807; struct Cyc_Set_Set*
_temp808; struct _tagged_arr* _temp810; struct _tuple8* _temp805=( struct
_tuple8*) _check_null( entry); _temp807=* _temp805; _LL811: _temp810= _temp807.f1;
goto _LL809; _LL809: _temp808= _temp807.f2; goto _LL806; _LL806:(( void(*)(
struct Cyc_Hashtable_Table* t, struct _tagged_arr* key, struct Cyc_Set_Set* val))
Cyc_Hashtable_insert)( t, _temp810, _temp808);} Cyc_Std_fclose( in_file);
_temp790=({ struct Cyc_Std_String_pa_struct _temp815; _temp815.tag= Cyc_Std_String_pa;
_temp815.f1=( struct _tagged_arr) _temp779;{ struct Cyc_Std_String_pa_struct
_temp814; _temp814.tag= Cyc_Std_String_pa; _temp814.f1=( struct _tagged_arr)
_temp780;{ struct Cyc_Std_String_pa_struct _temp813; _temp813.tag= Cyc_Std_String_pa;
_temp813.f1=( struct _tagged_arr) _temp784;{ void* _temp812[ 3u]={& _temp813,&
_temp814,& _temp815}; Cyc_Std_aprintf( _tag_arr("%s -E -o %s %s", sizeof(
unsigned char), 15u), _tag_arr( _temp812, sizeof( void*), 3u));}}}}); if( Cyc_Std_system((
struct _tagged_arr) _temp790) !=  0){ return 1;} maybe= Cyc_Std_fopen(( struct
_tagged_arr) _temp780, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp804= Cyc_Lexing_from_file( in_file); Cyc_slurp_out= Cyc_Std_fopen((
struct _tagged_arr) _temp781, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) Cyc_slurp_out)){ return 1;} if((( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_slurp)( _temp804)){({ struct Cyc_Std_String_pa_struct _temp817;
_temp817.tag= Cyc_Std_String_pa; _temp817.f1=( struct _tagged_arr) filename;{
void* _temp816[ 1u]={& _temp817}; Cyc_log( _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp816, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp819; _temp819.tag= Cyc_Std_String_pa;
_temp819.f1=( struct _tagged_arr) filename;{ void* _temp818[ 1u]={& _temp819};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp818, sizeof( void*), 1u));}}); Cyc_Std_fclose(
in_file); Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
Cyc_Std_remove(( struct _tagged_arr) _temp779); Cyc_Std_remove(( struct
_tagged_arr) _temp780); Cyc_Std_remove(( struct _tagged_arr) _temp781); return 1;}
Cyc_Std_fclose( in_file); Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null(
Cyc_slurp_out)); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp781, _tag_arr("r",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} in_file=(
struct Cyc_Std___sFILE*) _check_null( maybe); Cyc_Position_reset_position((
struct _tagged_arr) _temp781); Cyc_Lex_lex_init();{ struct Cyc_List_List*
_temp820= Cyc_Parse_parse_file( in_file); Cyc_Lex_lex_init(); Cyc_Std_fclose(
in_file); Cyc_Std_remove(( struct _tagged_arr) _temp781);{ struct Cyc_List_List*
_temp821= _temp820; for( 0; _temp821 !=  0; _temp821= _temp821->tl){ struct
_tuple8* _temp822= Cyc_scan_decl(( struct Cyc_Absyn_Decl*) _temp821->hd); if(
_temp822 ==  0){ continue;}{ struct _tuple8 _temp825; struct Cyc_Set_Set*
_temp826; struct _tagged_arr* _temp828; struct _tuple8* _temp823=( struct
_tuple8*) _check_null( _temp822); _temp825=* _temp823; _LL829: _temp828=
_temp825.f1; goto _LL827; _LL827: _temp826= _temp825.f2; goto _LL824; _LL824: {
struct Cyc_Set_Set* old;{ struct _handler_cons _temp830; _push_handler(&
_temp830);{ int _temp832= 0; if( setjmp( _temp830.handler)){ _temp832= 1;} if( !
_temp832){ old=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key)) Cyc_Hashtable_lookup)( t, _temp828);; _pop_handler();} else{
void* _temp831=( void*) _exn_thrown; void* _temp834= _temp831; _LL836: if(
_temp834 ==  Cyc_Core_Not_found){ goto _LL837;} else{ goto _LL838;} _LL838: goto
_LL839; _LL837: old=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); goto _LL835; _LL839:(
void) _throw( _temp834); _LL835:;}}}(( void(*)( struct Cyc_Hashtable_Table* t,
struct _tagged_arr* key, struct Cyc_Set_Set* val)) Cyc_Hashtable_insert)( t,
_temp828,(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( _temp826, old));}}}}{ struct Cyc_Set_Set* _temp840= Cyc_reachable(
start_symbols, t); struct Cyc_List_List* _temp841= 0; struct Cyc_Set_Set*
defined_symbols=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);{ struct Cyc_List_List*
_temp842= _temp820; for( 0; _temp842 !=  0; _temp842= _temp842->tl){ struct Cyc_Absyn_Decl*
_temp843=( struct Cyc_Absyn_Decl*) _temp842->hd; struct _tagged_arr* name;{ void*
_temp844=( void*) _temp843->r; struct Cyc_Absyn_Vardecl* _temp870; struct Cyc_Absyn_Fndecl*
_temp872; struct Cyc_Absyn_Structdecl* _temp874; struct Cyc_Absyn_Uniondecl*
_temp876; struct Cyc_Absyn_Enumdecl* _temp878; struct Cyc_Absyn_Typedefdecl*
_temp880; _LL846: if(*(( int*) _temp844) ==  Cyc_Absyn_Var_d){ _LL871: _temp870=((
struct Cyc_Absyn_Var_d_struct*) _temp844)->f1; goto _LL847;} else{ goto _LL848;}
_LL848: if(*(( int*) _temp844) ==  Cyc_Absyn_Fn_d){ _LL873: _temp872=(( struct
Cyc_Absyn_Fn_d_struct*) _temp844)->f1; goto _LL849;} else{ goto _LL850;} _LL850:
if(*(( int*) _temp844) ==  Cyc_Absyn_Struct_d){ _LL875: _temp874=(( struct Cyc_Absyn_Struct_d_struct*)
_temp844)->f1; goto _LL851;} else{ goto _LL852;} _LL852: if(*(( int*) _temp844)
==  Cyc_Absyn_Union_d){ _LL877: _temp876=(( struct Cyc_Absyn_Union_d_struct*)
_temp844)->f1; goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp844)
==  Cyc_Absyn_Enum_d){ _LL879: _temp878=(( struct Cyc_Absyn_Enum_d_struct*)
_temp844)->f1; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp844)
==  Cyc_Absyn_Typedef_d){ _LL881: _temp880=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp844)->f1; goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp844)
==  Cyc_Absyn_Let_d){ goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*)
_temp844) ==  Cyc_Absyn_Tunion_d){ goto _LL861;} else{ goto _LL862;} _LL862: if(*((
int*) _temp844) ==  Cyc_Absyn_Letv_d){ goto _LL863;} else{ goto _LL864;} _LL864:
if(*(( int*) _temp844) ==  Cyc_Absyn_Namespace_d){ goto _LL865;} else{ goto
_LL866;} _LL866: if(*(( int*) _temp844) ==  Cyc_Absyn_Using_d){ goto _LL867;}
else{ goto _LL868;} _LL868: if(*(( int*) _temp844) ==  Cyc_Absyn_ExternC_d){
goto _LL869;} else{ goto _LL845;} _LL847: { struct _tuple0 _temp884; struct
_tagged_arr* _temp885; struct _tuple0* _temp882= _temp870->name; _temp884=*
_temp882; _LL886: _temp885= _temp884.f2; goto _LL883; _LL883: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp885); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp885)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp885;} goto _LL845;} _LL849: { struct _tuple0 _temp889; struct
_tagged_arr* _temp890; struct _tuple0* _temp887= _temp872->name; _temp889=*
_temp887; _LL891: _temp890= _temp889.f2; goto _LL888; _LL888: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp890); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp890)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp890;} goto _LL845;} _LL851: if( !(( unsigned int) _temp874->name)){
name= 0;}{ struct _tuple0 _temp894; struct _tagged_arr* _temp895; struct _tuple0*
_temp892=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp874->name))->v;
_temp894=* _temp892; _LL896: _temp895= _temp894.f2; goto _LL893; _LL893: name=(
struct _tagged_arr*) _temp895; goto _LL845;} _LL853: if( !(( unsigned int)
_temp876->name)){ name= 0;}{ struct _tuple0 _temp899; struct _tagged_arr*
_temp900; struct _tuple0* _temp897=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp876->name))->v; _temp899=* _temp897; _LL901: _temp900=
_temp899.f2; goto _LL898; _LL898: name=( struct _tagged_arr*) _temp900; goto
_LL845;} _LL855: { struct _tuple0 _temp904; struct _tagged_arr* _temp905; struct
_tuple0* _temp902= _temp878->name; _temp904=* _temp902; _LL906: _temp905=
_temp904.f2; goto _LL903; _LL903: name=( struct _tagged_arr*) _temp905; if( name
!=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp840,( struct _tagged_arr*) _check_null( name)): 0){ _temp841=({ struct Cyc_List_List*
_temp907=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp907->hd=( void*) _temp843; _temp907->tl= _temp841; _temp907;});} else{ if((
unsigned int) _temp878->fields){ struct Cyc_List_List* _temp908=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp878->fields))->v; for( 0; _temp908 !=  0;
_temp908= _temp908->tl){ struct Cyc_Absyn_Enumfield* _temp909=( struct Cyc_Absyn_Enumfield*)
_temp908->hd; struct _tuple0 _temp912; struct _tagged_arr* _temp913; struct
_tuple0* _temp910= _temp909->name; _temp912=* _temp910; _LL914: _temp913=
_temp912.f2; goto _LL911; _LL911: if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( _temp840, _temp913)){ _temp841=({ struct Cyc_List_List*
_temp915=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp915->hd=( void*) _temp843; _temp915->tl= _temp841; _temp915;}); break;}}}}
name= 0; goto _LL845;} _LL857: { struct _tuple0 _temp918; struct _tagged_arr*
_temp919; struct _tuple0* _temp916= _temp880->name; _temp918=* _temp916; _LL920:
_temp919= _temp918.f2; goto _LL917; _LL917: name=( struct _tagged_arr*) _temp919;
goto _LL845;} _LL859: goto _LL861; _LL861: goto _LL863; _LL863: goto _LL865;
_LL865: goto _LL867; _LL867: goto _LL869; _LL869: name= 0; goto _LL845; _LL845:;}
if( name !=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp840,( struct _tagged_arr*) _check_null( name)): 0){ _temp841=({ struct Cyc_List_List*
_temp921=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp921->hd=( void*) _temp843; _temp921->tl= _temp841; _temp921;});}}} _temp841=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp841);
maybe= Cyc_Std_fopen( filename, _tag_arr("w", sizeof( unsigned char), 2u)); if(
!(( unsigned int) maybe)){ return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp923; _temp923.tag=
Cyc_Std_String_pa; _temp923.f1=( struct _tagged_arr) _temp783;{ void* _temp922[
1u]={& _temp923}; Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\nusing Std;\n",
sizeof( unsigned char), 26u), _tag_arr( _temp922, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp783, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} out_file=(
struct Cyc_Std___sFILE*) _check_null( maybe);{ struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _temp1074; _temp1074.tag= Cyc_Std_String_pa;
_temp1074.f1=( struct _tagged_arr) filename;{ void* _temp1073[ 1u]={& _temp1074};
Cyc_Std_aprintf( _tag_arr("_%s_", sizeof( unsigned char), 5u), _tag_arr(
_temp1073, sizeof( void*), 1u));}});{ int _temp924= 0; for( 0; _temp924 < 
_get_arr_size( ifdefmacro, sizeof( unsigned char)); _temp924 ++){ if(*((
unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof( unsigned char),
_temp924)) == '.'? 1:*(( unsigned char*) _check_unknown_subscript( ifdefmacro,
sizeof( unsigned char), _temp924)) == '/'){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp924))='_';}
else{ if(*(( unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof(
unsigned char), _temp924)) != '_'?*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp924)) != '/': 0){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp924))=(
unsigned char) toupper(( int)*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp924)));}}}}({ struct Cyc_Std_String_pa_struct
_temp927; _temp927.tag= Cyc_Std_String_pa; _temp927.f1=( struct _tagged_arr)
ifdefmacro;{ struct Cyc_Std_String_pa_struct _temp926; _temp926.tag= Cyc_Std_String_pa;
_temp926.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp925[ 2u]={& _temp926,&
_temp927}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n#define %s\n",
sizeof( unsigned char), 23u), _tag_arr( _temp925, sizeof( void*), 2u));}}}); if(
prologue !=  0){ struct Cyc_List_List* _temp928= prologue; for( 0; _temp928 != 
0; _temp928= _temp928->tl){ struct _tuple10 _temp931; struct _tagged_arr
_temp932; struct _tagged_arr _temp934; struct _tuple10* _temp929=( struct
_tuple10*) _temp928->hd; _temp931=* _temp929; _LL935: _temp934= _temp931.f1;
goto _LL933; _LL933: _temp932= _temp931.f2; goto _LL930; _LL930: if( _temp932.curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp934.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp936=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp936[ 0]= _temp934; _temp936;})): 0){ Cyc_Std_fputs( _temp932,
out_file);} else{({ struct Cyc_Std_String_pa_struct _temp938; _temp938.tag= Cyc_Std_String_pa;
_temp938.f1=( struct _tagged_arr) _temp934;{ void* _temp937[ 1u]={& _temp938};
Cyc_log( _tag_arr("%s is not supported on this platform\n", sizeof(
unsigned char), 38u), _tag_arr( _temp937, sizeof( void*), 1u));}});}}}({ void*
_temp939[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("\nnamespace Std {\n",
sizeof( unsigned char), 18u), _tag_arr( _temp939, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp940= _temp841; for( 0; _temp940 !=  0; _temp940=
_temp940->tl){ struct Cyc_Absyn_Decl* _temp941=( struct Cyc_Absyn_Decl*)
_temp940->hd; int _temp942= 0; struct _tagged_arr* name;{ void* _temp943=( void*)
_temp941->r; struct Cyc_Absyn_Vardecl* _temp969; struct Cyc_Absyn_Fndecl*
_temp971; struct Cyc_Absyn_Structdecl* _temp973; struct Cyc_Absyn_Uniondecl*
_temp975; struct Cyc_Absyn_Enumdecl* _temp977; struct Cyc_Absyn_Typedefdecl*
_temp979; _LL945: if(*(( int*) _temp943) ==  Cyc_Absyn_Var_d){ _LL970: _temp969=((
struct Cyc_Absyn_Var_d_struct*) _temp943)->f1; goto _LL946;} else{ goto _LL947;}
_LL947: if(*(( int*) _temp943) ==  Cyc_Absyn_Fn_d){ _LL972: _temp971=(( struct
Cyc_Absyn_Fn_d_struct*) _temp943)->f1; goto _LL948;} else{ goto _LL949;} _LL949:
if(*(( int*) _temp943) ==  Cyc_Absyn_Struct_d){ _LL974: _temp973=(( struct Cyc_Absyn_Struct_d_struct*)
_temp943)->f1; goto _LL950;} else{ goto _LL951;} _LL951: if(*(( int*) _temp943)
==  Cyc_Absyn_Union_d){ _LL976: _temp975=(( struct Cyc_Absyn_Union_d_struct*)
_temp943)->f1; goto _LL952;} else{ goto _LL953;} _LL953: if(*(( int*) _temp943)
==  Cyc_Absyn_Enum_d){ _LL978: _temp977=(( struct Cyc_Absyn_Enum_d_struct*)
_temp943)->f1; goto _LL954;} else{ goto _LL955;} _LL955: if(*(( int*) _temp943)
==  Cyc_Absyn_Typedef_d){ _LL980: _temp979=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp943)->f1; goto _LL956;} else{ goto _LL957;} _LL957: if(*(( int*) _temp943)
==  Cyc_Absyn_Let_d){ goto _LL958;} else{ goto _LL959;} _LL959: if(*(( int*)
_temp943) ==  Cyc_Absyn_Tunion_d){ goto _LL960;} else{ goto _LL961;} _LL961: if(*((
int*) _temp943) ==  Cyc_Absyn_Letv_d){ goto _LL962;} else{ goto _LL963;} _LL963:
if(*(( int*) _temp943) ==  Cyc_Absyn_Namespace_d){ goto _LL964;} else{ goto
_LL965;} _LL965: if(*(( int*) _temp943) ==  Cyc_Absyn_Using_d){ goto _LL966;}
else{ goto _LL967;} _LL967: if(*(( int*) _temp943) ==  Cyc_Absyn_ExternC_d){
goto _LL968;} else{ goto _LL944;} _LL946: { struct _tuple0 _temp983; struct
_tagged_arr* _temp984; struct _tuple0* _temp981= _temp969->name; _temp983=*
_temp981; _LL985: _temp984= _temp983.f2; goto _LL982; _LL982: name=( struct
_tagged_arr*) _temp984; goto _LL944;} _LL948: { struct _tuple0 _temp988; struct
_tagged_arr* _temp989; struct _tuple0* _temp986= _temp971->name; _temp988=*
_temp986; _LL990: _temp989= _temp988.f2; goto _LL987; _LL987: name=( struct
_tagged_arr*) _temp989; goto _LL944;} _LL950: if( !(( unsigned int) _temp973->name)){
name= 0;}{ struct _tuple0 _temp993; struct _tagged_arr* _temp994; struct _tuple0*
_temp991=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp973->name))->v;
_temp993=* _temp991; _LL995: _temp994= _temp993.f2; goto _LL992; _LL992: name=(
struct _tagged_arr*) _temp994; goto _LL944;} _LL952: if( !(( unsigned int)
_temp975->name)){ name= 0;}{ struct _tuple0 _temp998; struct _tagged_arr*
_temp999; struct _tuple0* _temp996=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp975->name))->v; _temp998=* _temp996; _LL1000: _temp999=
_temp998.f2; goto _LL997; _LL997: name=( struct _tagged_arr*) _temp999; goto
_LL944;} _LL954: { struct _tuple0 _temp1003; struct _tagged_arr* _temp1004;
struct _tuple0* _temp1001= _temp977->name; _temp1003=* _temp1001; _LL1005:
_temp1004= _temp1003.f2; goto _LL1002; _LL1002: name=( struct _tagged_arr*)
_temp1004; goto _LL944;} _LL956: { struct _tuple0 _temp1008; struct _tagged_arr*
_temp1009; struct _tuple0* _temp1006= _temp979->name; _temp1008=* _temp1006;
_LL1010: _temp1009= _temp1008.f2; goto _LL1007; _LL1007: name=( struct
_tagged_arr*) _temp1009; goto _LL944;} _LL958: goto _LL960; _LL960: goto _LL962;
_LL962: goto _LL964; _LL964: goto _LL966; _LL966: goto _LL968; _LL968: name= 0;
goto _LL944; _LL944:;} if( !(( unsigned int) name)? ! _temp942: 0){ continue;}
if(( unsigned int) name){ ifdefmacro=({ struct Cyc_Std_String_pa_struct
_temp1012; _temp1012.tag= Cyc_Std_String_pa; _temp1012.f1=( struct _tagged_arr)*
name;{ void* _temp1011[ 1u]={& _temp1012}; Cyc_Std_aprintf( _tag_arr("_%s_def_",
sizeof( unsigned char), 9u), _tag_arr( _temp1011, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1014; _temp1014.tag= Cyc_Std_String_pa;
_temp1014.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp1013[ 1u]={&
_temp1014}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof(
unsigned char), 12u), _tag_arr( _temp1013, sizeof( void*), 1u));}});({ struct
Cyc_Std_String_pa_struct _temp1016; _temp1016.tag= Cyc_Std_String_pa; _temp1016.f1=(
struct _tagged_arr) ifdefmacro;{ void* _temp1015[ 1u]={& _temp1016}; Cyc_Std_fprintf(
out_file, _tag_arr("#define %s\n", sizeof( unsigned char), 12u), _tag_arr(
_temp1015, sizeof( void*), 1u));}}); Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp1017[ 1u]; _temp1017[ 0u]= _temp941;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1017, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);({ void* _temp1018[ 0u]={}; Cyc_Std_fprintf( out_file,
_tag_arr("#endif\n", sizeof( unsigned char), 8u), _tag_arr( _temp1018, sizeof(
void*), 0u));});} else{ Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp1019[ 1u]; _temp1019[ 0u]= _temp941;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1019, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);}}} _temp790=({ struct Cyc_Std_String_pa_struct _temp1023;
_temp1023.tag= Cyc_Std_String_pa; _temp1023.f1=( struct _tagged_arr) _temp779;{
struct Cyc_Std_String_pa_struct _temp1022; _temp1022.tag= Cyc_Std_String_pa;
_temp1022.f1=( struct _tagged_arr) _temp780;{ struct Cyc_Std_String_pa_struct
_temp1021; _temp1021.tag= Cyc_Std_String_pa; _temp1021.f1=( struct _tagged_arr)
_temp784;{ void* _temp1020[ 3u]={& _temp1021,& _temp1022,& _temp1023}; Cyc_Std_aprintf(
_tag_arr("%s -E -dM -o %s %s", sizeof( unsigned char), 19u), _tag_arr( _temp1020,
sizeof( void*), 3u));}}}}); if( Cyc_Std_system(( struct _tagged_arr) _temp790)
!=  0){ return 1;} maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp780, _tag_arr("r",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} in_file=(
struct Cyc_Std___sFILE*) _check_null( maybe); _temp804= Cyc_Lexing_from_file(
in_file);{ struct _tuple9* entry2; while(( entry2=(( struct _tuple9*(*)( struct
Cyc_Lexing_lexbuf* lexbuf)) Cyc_suck_line)( _temp804)) !=  0) { struct _tuple9
_temp1026; struct _tagged_arr* _temp1027; struct _tagged_arr _temp1029; struct
_tuple9* _temp1024=( struct _tuple9*) _check_null( entry2); _temp1026=*
_temp1024; _LL1030: _temp1029= _temp1026.f1; goto _LL1028; _LL1028: _temp1027=
_temp1026.f2; goto _LL1025; _LL1025: if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( _temp840, _temp1027)){({ struct Cyc_Std_String_pa_struct
_temp1032; _temp1032.tag= Cyc_Std_String_pa; _temp1032.f1=( struct _tagged_arr)*
_temp1027;{ void* _temp1031[ 1u]={& _temp1032}; Cyc_Std_fprintf( out_file,
_tag_arr("#ifndef %s\n", sizeof( unsigned char), 12u), _tag_arr( _temp1031,
sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct _temp1034; _temp1034.tag=
Cyc_Std_String_pa; _temp1034.f1=( struct _tagged_arr) _temp1029;{ void*
_temp1033[ 1u]={& _temp1034}; Cyc_Std_fprintf( out_file, _tag_arr("%s\n",
sizeof( unsigned char), 4u), _tag_arr( _temp1033, sizeof( void*), 1u));}});({
void* _temp1035[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp1035, sizeof( void*), 0u));});}} Cyc_Std_fclose(
in_file); Cyc_Std_remove(( struct _tagged_arr) _temp779); Cyc_Std_remove((
struct _tagged_arr) _temp780); if( epilogue !=  0){ struct Cyc_List_List*
_temp1036= epilogue; for( 0; _temp1036 !=  0; _temp1036= _temp1036->tl){ struct
_tuple10 _temp1039; struct _tagged_arr _temp1040; struct _tagged_arr _temp1042;
struct _tuple10* _temp1037=( struct _tuple10*) _temp1036->hd; _temp1039=*
_temp1037; _LL1043: _temp1042= _temp1039.f1; goto _LL1041; _LL1041: _temp1040=
_temp1039.f2; goto _LL1038; _LL1038: if( _temp1040.curr != (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr? _temp1042.curr == (( struct _tagged_arr) _tag_arr(
0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt))
Cyc_Set_member)( defined_symbols,({ struct _tagged_arr* _temp1044=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1044[ 0]= _temp1042;
_temp1044;})): 0){ Cyc_Std_fputs( _temp1040, out_file);} else{({ struct Cyc_Std_String_pa_struct
_temp1046; _temp1046.tag= Cyc_Std_String_pa; _temp1046.f1=( struct _tagged_arr)
_temp1042;{ void* _temp1045[ 1u]={& _temp1046}; Cyc_log( _tag_arr("%s is not supported on this platform\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1045, sizeof( void*), 1u));}});}}}({
void* _temp1047[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n", sizeof(
unsigned char), 3u), _tag_arr( _temp1047, sizeof( void*), 0u));});({ void*
_temp1048[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp1048, sizeof( void*), 0u));}); Cyc_Std_fclose(
out_file); if( cstubs !=  0){ out_file=( struct Cyc_Std___sFILE*) _check_null(
Cyc_cstubs_file);{ struct Cyc_List_List* _temp1049= cstubs; for( 0; _temp1049 != 
0; _temp1049= _temp1049->tl){ struct _tuple10 _temp1052; struct _tagged_arr
_temp1053; struct _tagged_arr _temp1055; struct _tuple10* _temp1050=( struct
_tuple10*) _temp1049->hd; _temp1052=* _temp1050; _LL1056: _temp1055= _temp1052.f1;
goto _LL1054; _LL1054: _temp1053= _temp1052.f2; goto _LL1051; _LL1051: if(
_temp1053.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1055.curr
== (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp1057=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1057[ 0]= _temp1055; _temp1057;})): 0){ Cyc_Std_fputs(
_temp1053, out_file);}}}} out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _temp1059; _temp1059.tag= Cyc_Std_String_pa;
_temp1059.f1=( struct _tagged_arr) _temp783;{ void* _temp1058[ 1u]={& _temp1059};
Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\n\n", sizeof( unsigned char),
16u), _tag_arr( _temp1058, sizeof( void*), 1u));}}); if( cycstubs !=  0){
out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({ void*
_temp1060[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("namespace Std {\n",
sizeof( unsigned char), 17u), _tag_arr( _temp1060, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp1061= cycstubs; for( 0; _temp1061 !=  0; _temp1061=
_temp1061->tl){ struct _tuple10 _temp1064; struct _tagged_arr _temp1065; struct
_tagged_arr _temp1067; struct _tuple10* _temp1062=( struct _tuple10*) _temp1061->hd;
_temp1064=* _temp1062; _LL1068: _temp1067= _temp1064.f1; goto _LL1066; _LL1066:
_temp1065= _temp1064.f2; goto _LL1063; _LL1063: if( _temp1065.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1067.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct _tagged_arr*
_temp1069=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp1069[ 0]= _temp1067; _temp1069;})): 0){ Cyc_Std_fputs( _temp1065, out_file);}}}({
void* _temp1070[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n\n", sizeof(
unsigned char), 4u), _tag_arr( _temp1070, sizeof( void*), 0u));});} if( Cyc_Std_chdir((
struct _tagged_arr) _temp772)){({ struct Cyc_Std_String_pa_struct _temp1072;
_temp1072.tag= Cyc_Std_String_pa; _temp1072.f1=( struct _tagged_arr) _temp772;{
void* _temp1071[ 1u]={& _temp1072}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not change directory to %s\n",
sizeof( unsigned char), 41u), _tag_arr( _temp1071, sizeof( void*), 1u));}});
return 1;} return 0;}}}}}}}}}} int Cyc_process_specfile( struct _tagged_arr file,
struct _tagged_arr dir){ struct Cyc_Std___sFILE* _temp1086= Cyc_Std_fopen( file,
_tag_arr("r", sizeof( unsigned char), 2u)); if( !(( unsigned int) _temp1086)){
return 1;}{ struct Cyc_Std___sFILE* _temp1087=( struct Cyc_Std___sFILE*)
_check_null( _temp1086); struct Cyc_Lexing_lexbuf* _temp1088= Cyc_Lexing_from_file(
_temp1087); struct _tuple11* entry; while(( entry=(( struct _tuple11*(*)( struct
Cyc_Lexing_lexbuf* lexbuf)) Cyc_spec)( _temp1088)) !=  0) { struct _tuple11
_temp1091; struct Cyc_List_List* _temp1092; struct Cyc_List_List* _temp1094;
struct Cyc_List_List* _temp1096; struct Cyc_List_List* _temp1098; struct Cyc_List_List*
_temp1100; struct Cyc_List_List* _temp1102; struct _tagged_arr _temp1104; struct
_tuple11* _temp1089=( struct _tuple11*) _check_null( entry); _temp1091=*
_temp1089; _LL1105: _temp1104= _temp1091.f1; goto _LL1103; _LL1103: _temp1102=
_temp1091.f2; goto _LL1101; _LL1101: _temp1100= _temp1091.f3; goto _LL1099;
_LL1099: _temp1098= _temp1091.f4; goto _LL1097; _LL1097: _temp1096= _temp1091.f5;
goto _LL1095; _LL1095: _temp1094= _temp1091.f6; goto _LL1093; _LL1093: _temp1092=
_temp1091.f7; goto _LL1090; _LL1090: if( Cyc_process_file( _temp1104, dir,
_temp1102, _temp1100, _temp1098, _temp1096, _temp1094, _temp1092)){ Cyc_Std_fclose(
_temp1087); return 1;}} Cyc_Std_fclose( _temp1087); return 0;}} int Cyc_getsize(
struct _tagged_arr dir, struct _tagged_arr includes, struct _tagged_arr type){
struct _tagged_arr _temp1106= Cyc_Filename_concat( dir, _tag_arr("getsize.c",
sizeof( unsigned char), 10u)); struct _tagged_arr _temp1107= Cyc_Filename_concat(
dir, _tag_arr("getsize", sizeof( unsigned char), 8u)); struct _tagged_arr
_temp1108= Cyc_Filename_concat( dir, _tag_arr("getsize.out", sizeof(
unsigned char), 12u)); struct Cyc_Std___sFILE* _temp1109= Cyc_Std_fopen(( struct
_tagged_arr) _temp1106, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) _temp1109)){({ struct Cyc_Std_String_pa_struct _temp1111;
_temp1111.tag= Cyc_Std_String_pa; _temp1111.f1=( struct _tagged_arr) _temp1106;{
void* _temp1110[ 1u]={& _temp1111}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create %s\n",
sizeof( unsigned char), 28u), _tag_arr( _temp1110, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1113; _temp1113.tag= Cyc_Std_String_pa;
_temp1113.f1=( struct _tagged_arr) type;{ void* _temp1112[ 1u]={& _temp1113};
Cyc_log( _tag_arr("Warning: could not find the size of %s\n", sizeof(
unsigned char), 40u), _tag_arr( _temp1112, sizeof( void*), 1u));}}); return - 1;}({
struct Cyc_Std_String_pa_struct _temp1118; _temp1118.tag= Cyc_Std_String_pa;
_temp1118.f1=( struct _tagged_arr) type;{ struct Cyc_Std_String_pa_struct
_temp1117; _temp1117.tag= Cyc_Std_String_pa; _temp1117.f1=( struct _tagged_arr)
type;{ struct Cyc_Std_String_pa_struct _temp1116; _temp1116.tag= Cyc_Std_String_pa;
_temp1116.f1=( struct _tagged_arr) _temp1108;{ struct Cyc_Std_String_pa_struct
_temp1115; _temp1115.tag= Cyc_Std_String_pa; _temp1115.f1=( struct _tagged_arr)
includes;{ void* _temp1114[ 4u]={& _temp1115,& _temp1116,& _temp1117,& _temp1118};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( _temp1109), _tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof( unsigned char), 139u), _tag_arr( _temp1114, sizeof( void*), 4u));}}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1109));{ struct
_tagged_arr _temp1119= Cstring_to_string( Ccomp); struct _tagged_arr _temp1120=({
struct Cyc_Std_String_pa_struct _temp1141; _temp1141.tag= Cyc_Std_String_pa;
_temp1141.f1=( struct _tagged_arr) _temp1106;{ struct Cyc_Std_String_pa_struct
_temp1140; _temp1140.tag= Cyc_Std_String_pa; _temp1140.f1=( struct _tagged_arr)
_temp1107;{ struct Cyc_Std_String_pa_struct _temp1139; _temp1139.tag= Cyc_Std_String_pa;
_temp1139.f1=( struct _tagged_arr) _temp1119;{ void* _temp1138[ 3u]={& _temp1139,&
_temp1140,& _temp1141}; Cyc_Std_aprintf( _tag_arr("%s -o %s %s", sizeof(
unsigned char), 12u), _tag_arr( _temp1138, sizeof( void*), 3u));}}}}); if( Cyc_Std_system((
struct _tagged_arr) _temp1120) !=  0){({ struct Cyc_Std_String_pa_struct
_temp1122; _temp1122.tag= Cyc_Std_String_pa; _temp1122.f1=( struct _tagged_arr)
type;{ void* _temp1121[ 1u]={& _temp1122}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Warning: could not find the size of %s; continuing anyway\n", sizeof(
unsigned char), 59u), _tag_arr( _temp1121, sizeof( void*), 1u));}});({ struct
Cyc_Std_String_pa_struct _temp1124; _temp1124.tag= Cyc_Std_String_pa; _temp1124.f1=(
struct _tagged_arr) type;{ void* _temp1123[ 1u]={& _temp1124}; Cyc_log( _tag_arr("Warning: could not find the size of %s\n",
sizeof( unsigned char), 40u), _tag_arr( _temp1123, sizeof( void*), 1u));}});
return - 1;}{ int _temp1125= Cyc_Std_system(( struct _tagged_arr) _temp1107);({
struct Cyc_Std_Int_pa_struct _temp1128; _temp1128.tag= Cyc_Std_Int_pa; _temp1128.f1=(
int)(( unsigned int) _temp1125);{ struct Cyc_Std_String_pa_struct _temp1127;
_temp1127.tag= Cyc_Std_String_pa; _temp1127.f1=( struct _tagged_arr) type;{ void*
_temp1126[ 2u]={& _temp1127,& _temp1128}; Cyc_log( _tag_arr("size of %s returned by system is %d\n",
sizeof( unsigned char), 37u), _tag_arr( _temp1126, sizeof( void*), 2u));}}});{
struct Cyc_Std___sFILE* _temp1129= Cyc_Std_fopen(( struct _tagged_arr) _temp1108,
_tag_arr("r", sizeof( unsigned char), 2u)); int w= - 1; if(({ struct Cyc_Std_IntPtr_sa_struct
_temp1131; _temp1131.tag= Cyc_Std_IntPtr_sa; _temp1131.f1=& w;{ void* _temp1130[
1u]={& _temp1131}; Cyc_Std_fscanf(( struct Cyc_Std___sFILE*) _check_null(
_temp1129), _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1130,
sizeof( void*), 1u));}}) !=  1){({ struct Cyc_Std_String_pa_struct _temp1133;
_temp1133.tag= Cyc_Std_String_pa; _temp1133.f1=( struct _tagged_arr) type;{ void*
_temp1132[ 1u]={& _temp1133}; Cyc_log( _tag_arr("Warning: could not find the size of %s\n",
sizeof( unsigned char), 40u), _tag_arr( _temp1132, sizeof( void*), 1u));}});
return - 1;}({ struct Cyc_Std_Int_pa_struct _temp1137; _temp1137.tag= Cyc_Std_Int_pa;
_temp1137.f1=( int)(( unsigned int) w);{ struct Cyc_Std_String_pa_struct
_temp1136; _temp1136.tag= Cyc_Std_String_pa; _temp1136.f1=( struct _tagged_arr)
_temp1108;{ struct Cyc_Std_String_pa_struct _temp1135; _temp1135.tag= Cyc_Std_String_pa;
_temp1135.f1=( struct _tagged_arr) type;{ void* _temp1134[ 3u]={& _temp1135,&
_temp1136,& _temp1137}; Cyc_log( _tag_arr("size of %s read from file %s is %d\n",
sizeof( unsigned char), 36u), _tag_arr( _temp1134, sizeof( void*), 3u));}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1129)); Cyc_Std_remove((
struct _tagged_arr) _temp1108); Cyc_Std_remove(( struct _tagged_arr) _temp1106);
Cyc_Std_remove(( struct _tagged_arr) _temp1107); return w;}}}} extern void
GC_blacklist_warn_clear(); int Cyc_main( int argc, struct _tagged_arr argv){
GC_blacklist_warn_clear(); if( argc <  3){({ void* _temp1142[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof( unsigned char), 105u), _tag_arr( _temp1142, sizeof( void*), 0u));});
return 1;}{ struct _tagged_arr _temp1143=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 1)); if( Cyc_force_directory_prefixes((
struct _tagged_arr) _temp1143)? 1: Cyc_force_directory(( struct _tagged_arr)
_temp1143)){({ struct Cyc_Std_String_pa_struct _temp1145; _temp1145.tag= Cyc_Std_String_pa;
_temp1145.f1=( struct _tagged_arr) _temp1143;{ void* _temp1144[ 1u]={& _temp1145};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1144, sizeof( void*), 1u));}});
return 1;} Cyc_log_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1143, _tag_arr("BUILDLIB.LOG", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _temp1147; _temp1147.tag= Cyc_Std_String_pa;
_temp1147.f1=( struct _tagged_arr) _temp1143;{ void* _temp1146[ 1u]={& _temp1147};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create log file in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1146, sizeof( void*), 1u));}});
return 1;} Cyc_cstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1143, _tag_arr("cstubs.c", sizeof( unsigned char), 9u)),
_tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1149; _temp1149.tag= Cyc_Std_String_pa;
_temp1149.f1=( struct _tagged_arr) _temp1143;{ void* _temp1148[ 1u]={& _temp1149};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1148, sizeof( void*), 1u));}});
return 1;} Cyc_cycstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1143, _tag_arr("cycstubs.cyc", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1151; _temp1151.tag= Cyc_Std_String_pa;
_temp1151.f1=( struct _tagged_arr) _temp1143;{ void* _temp1150[ 1u]={& _temp1151};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof( unsigned char), 52u), _tag_arr( _temp1150, sizeof( void*), 1u));}});
return 1;}({ void* _temp1152[ 0u]={}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file), _tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1152, sizeof( void*), 0u));}); Cyc_sizeof_unsignedlongint=
Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned long int", sizeof( unsigned char), 18u)); Cyc_sizeof_unsignedshortint=
Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned short int", sizeof( unsigned char), 19u)); Cyc_sizeof_shortint=
Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("short int", sizeof( unsigned char), 10u)); Cyc_sizeof_int= Cyc_getsize((
struct _tagged_arr) _temp1143, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("int", sizeof( unsigned char), 4u)); Cyc_sizeof_short= Cyc_getsize((
struct _tagged_arr) _temp1143, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("short", sizeof( unsigned char), 6u)); Cyc_sizeof_fdmask= Cyc_getsize((
struct _tagged_arr) _temp1143, _tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof( unsigned char), 47u), _tag_arr("fd_mask", sizeof( unsigned char), 8u));
Cyc_sizeof___fdmask= Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof( unsigned char), 47u), _tag_arr("__fd_mask", sizeof( unsigned char), 10u));
Cyc_sizeof_uint32= Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("#include <sys/types.h>",
sizeof( unsigned char), 23u), _tag_arr("__uint32_t", sizeof( unsigned char), 11u));
Cyc_sizeof_sockaddr= Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("#include <sys/types.h>\n#include <sys/socket.h>",
sizeof( unsigned char), 47u), _tag_arr("struct sockaddr", sizeof( unsigned char),
16u)); Cyc_sizeof_inport= Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof( unsigned char), 47u), _tag_arr("in_port_t", sizeof( unsigned char), 10u));
Cyc_sizeof_inaddr= Cyc_getsize(( struct _tagged_arr) _temp1143, _tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof( unsigned char), 47u), _tag_arr("struct in_addr", sizeof( unsigned char),
15u));{ int i= 2; for( 0; i <  argc; i ++){ Cyc_process_specfile(( struct
_tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), i)),( struct _tagged_arr) _temp1143);}} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_log_file)); Cyc_Std_fclose(( struct
Cyc_Std___sFILE*) _check_null( Cyc_cstubs_file)); Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file)); return 0;}}
