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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stderr; extern int Cyc_Std_remove( struct _tagged_arr); extern int Cyc_Std_fclose(
struct Cyc_Std___sFILE*); extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*);
extern struct Cyc_Std___sFILE* Cyc_Std_fopen( struct _tagged_arr __filename,
struct _tagged_arr __modes); extern int Cyc_Std_fputc( int __c, struct Cyc_Std___sFILE*
__stream); extern int Cyc_Std_fputs( struct _tagged_arr __s, struct Cyc_Std___sFILE*
__stream); extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
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
__p, struct Cyc_Std_timezone* __z); typedef struct { unsigned int __val[ 64u]; }
Cyc_Std___sigset_t; typedef Cyc_Std___sigset_t Cyc_Std_sigset_t; struct Cyc_Std_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_Std_timeval; extern int select( int,
struct Cyc_Std__types_fd_set*, struct Cyc_Std__types_fd_set*, struct Cyc_Std__types_fd_set*,
struct Cyc_Std_timeval*); struct Cyc_Std_option{ struct _tagged_arr name; int
has_arg; int* flag; int val; } ; extern unsigned int alarm( unsigned int seconds);
extern int close( int); extern void _exit( int); extern int getpid(); extern int
getppid(); extern int fork(); extern int fchdir( int); extern int fchown( int,
unsigned short, unsigned short); extern int fsync( int); extern int ftruncate(
int, int); extern int dup( int); extern int dup2( int, int); extern int setsid();
extern int getsid( int pid); extern unsigned short getuid(); extern int setuid(
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
int tag; unsigned int f1; } ; static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{
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
tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; static const
int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_IfThenElse_s=
3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; static const int Cyc_Absyn_While_s=
4; struct _tuple2{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_SwitchC_s= 10;
struct Cyc_Absyn_SwitchC_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Fallthru_s= 11; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ;
static const int Cyc_Absyn_Decl_s= 12; struct Cyc_Absyn_Decl_s_struct{ int tag;
struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s=
13; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Splice_s= 14; struct Cyc_Absyn_Splice_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Label_s= 15;
struct Cyc_Absyn_Label_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Do_s= 16; struct Cyc_Absyn_Do_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s=
17; struct Cyc_Absyn_TryCatch_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Region_s= 18; struct
Cyc_Absyn_Region_s_struct{ int tag; struct Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
loc; struct Cyc_List_List* non_local_preds; int try_depth; void* annot; } ;
static const int Cyc_Absyn_Wild_p= 0; static const int Cyc_Absyn_Var_p= 0;
struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Std___sFILE* f); extern struct Cyc_List_List* Cyc_Parse_parse_file(
struct Cyc_Std___sFILE* f); struct Cyc_Declaration_spec; struct Cyc_Declarator;
struct Cyc_Abstractdeclarator; extern int Cyc_yyparse(); extern unsigned char
Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
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
int Cyc_lex_base[ 496u]=( const int[ 496u]){ 0, 0, 75, 192, 305, 285, 310, 166,
311, 91, 27, 384, 28, 503, 617, 695, 288, 338, 93, - 3, 0, - 2, - 1, - 8, - 3, 1,
- 1, 586, - 4, 2, 166, - 5, 308, 777, 333, - 6, - 7, - 3, 16, - 2, 29, 11, 815,
- 3, 890, 13, - 15, 223, 12, - 14, 107, 22, 26, 28, 24, - 13, 28, 58, 56, 65, 52,
70, 320, 1004, 1119, 357, 93, 83, 90, 112, 96, 106, 115, 125, 119, 150, 173, 382,
410, 226, 218, 211, 231, 231, 415, 505, 229, 228, 226, 235, 223, 240, 498, 1233,
1348, 510, 226, 244, 231, 234, 236, 256, 515, 1462, 1577, - 9, 530, - 10, 238,
270, 633, 1691, 1806, 635, - 8, 391, 641, 646, 1883, 1960, 2037, 2118, 402, 416,
653, 2193, 269, 270, 277, 285, 283, 293, 0, 13, 4, 2274, 5, 638, 2282, 2347, 646,
49, 465, 6, 2308, 7, 682, 2370, 2408, 800, - 28, 287, 289, 304, 311, 295, 317,
310, 325, 671, 1035, 336, 343, 334, 405, 335, 331, 371, 374, 400, 416, 422, 710,
1030, 415, 423, 418, 417, 416, 809, 1145, 424, 420, 1006, 428, 442, 826, 1147,
436, 432, 1235, 452, 450, 448, 496, 481, 1014, 1259, 519, 520, 533, 545, 560,
560, 561, 554, 1350, - 12, 574, 573, 571, 574, 561, 1464, 563, 563, 1579, 2470,
572, 586, 1693, - 11, 622, 595, 1808, 598, 596, 596, 618, 598, 2332, 620, 615,
628, 611, 632, 2472, 638, 641, 648, 644, 640, 10, 14, 662, 670, 2480, 692, 679,
692, 676, 714, 2482, 936, 708, 720, 727, 720, 746, 756, 744, 744, 747, 757, 807,
791, - 26, 800, 804, 801, 847, 854, 855, - 22, 836, 852, 844, 866, 877, 872, 874,
891, 923, - 16, 910, 906, 906, 930, 931, - 23, 923, 921, 914, 926, 922, 947, 931,
954, 968, 956, 960, 951, 952, - 25, 986, 980, 983, 993, 1002, 1014, 1017, 1033,
1040, 1041, 1, 4, 6, 1044, 1046, 1033, 1032, 1044, 1045, 1052, 1053, 2, 52, 1054,
1058, 2272, 20, 21, 1086, 1088, 1051, 1049, 1064, 1070, 1071, 1137, 1139, 1140,
- 21, 1087, 1088, 1169, 1170, 1171, - 18, 1118, 1120, 1206, 1208, 1210, - 19,
1158, 1159, 1214, 1215, 1216, - 17, 1163, 1164, 1219, 1220, 1221, - 20, 1168,
1169, 1469, - 27, 1169, 1162, 1163, 1162, 1175, 4, 1178, 1169, 1170, 1168, 1181,
1260, - 14, - 7, - 8, 8, 1356, 2484, 9, 1435, 2492, 2517, 1545, 1374, - 49, 1361,
- 2, 1232, - 4, 1233, 1324, 2090, 1234, 1325, 1333, 1659, 1235, 2574, 2617, 1239,
1264, 1266, 1314, 2687, 1268, 1316, - 36, - 42, - 37, 2762, 1306, - 40, 1310, -
45, - 39, - 48, 2837, 2866, 1777, 1294, 1331, 2584, 2876, 2906, 2655, 2847, 2939,
2970, 3008, 1324, 1334, 3078, 3116, 1327, 1400, 1406, 1416, 1408, 1445, - 6, -
34, 1335, 3048, - 47, - 30, - 32, - 46, - 29, - 31, - 33, 1350, 3156, 1353, 1441,
1564, 1442, 1456, 1458, 1459, 1460, 1461, 1463, 1464, 1465, 3229, 3313, 1678,
1467, - 24, - 41, - 38, - 35, 1832, 3395, 1475, 3478, 1479, 15, 1422, 1425, 1452,
1450, 1446, 1457, 1559}; const int Cyc_lex_backtrk[ 496u]=( const int[ 496u]){ -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 2, - 1, - 1, - 1, - 1, 2, - 1, 8, - 1, 3, 5, - 1, - 1, 6, 5, - 1,
- 1, - 1, 6, - 1, 4, 1, 0, - 1, 0, 1, - 1, 11, 14, - 1, 14, 14, 14, 14, 14, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 10, 11, 2, 4, 4, - 1, 1,
1, 0, 2, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, 2, 8, 3, 5, - 1, 6, 5, 6, 4, 2,
8, 3, 5, - 1, 6, 5, - 1, 27, 27, 27, 27, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 23, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 6, 1, 9,
2, 4, - 1, 5, 4, - 1, - 1, 2, - 1, 48, - 1, 48, 48, 48, 48, 48, 48, 48, 48, 5, 7,
48, 48, 48, 48, 0, 48, 48, - 1, - 1, - 1, 0, 43, - 1, 42, - 1, - 1, - 1, 9, 7, -
1, 7, 7, - 1, 8, 9, - 1, - 1, 9, 5, 6, 5, 5, - 1, 4, 4, 4, 6, 6, 5, 5, - 1, - 1,
- 1, 9, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, - 1,
- 1, 2, 1, 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_default[ 496u]=(
const int[ 496u]){ - 1, - 1, - 1, 395, 384, 150, 23, 36, 23, 19, - 1, - 1, 12,
31, 46, 31, 36, 23, 19, 0, - 1, 0, 0, 0, 0, - 1, 0, - 1, 0, - 1, - 1, 0, - 1, -
1, - 1, 0, 0, 0, - 1, 0, 40, - 1, - 1, 0, - 1, - 1, 0, - 1, - 1, 0, - 1, - 1, -
1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 0, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, 115, - 1, - 1, -
1, - 1, - 1, 122, 122, 122, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 142, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1,
- 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, -
1, - 1, 0, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, 0, - 1, 0,
- 1, - 1, 461, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 0, 0, 0, - 1, - 1, 0, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
- 1, - 1, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_trans[ 3735u]=( const int[
3735u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 19, 28, 488, 19, 28, 19, 28, 114, 19,
45, 45, 45, 45, 45, 21, 45, 0, 0, 0, 0, 0, 22, 315, 326, 489, 46, 21, - 1, - 1,
21, - 1, - 1, 45, 316, 45, 317, 22, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 28, 115, 22, 243, 126, 40, 244, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 28, 327, 361, 355, 486, 142, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 133, 86, 20, 79, 66, 56,
57, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 58, 59, 60, 61, 484, 62,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 67, 68, 37, 396, 397, 396, 396,
397, 39, 22, 69, 70, 71, 72, 73, 141, 34, 34, 34, 34, 34, 34, 34, 34, 96, 74,
396, 398, 399, 75, 97, 400, 401, 402, 116, 116, 403, 404, 116, 405, 406, 407,
408, 409, 409, 409, 409, 409, 409, 409, 409, 409, 410, 76, 411, 412, 413, 116,
19, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414,
414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 19, - 1, - 1, 415, 414,
77, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414,
414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 385, 416, 151, 386, 22,
24, 24, 37, 143, 134, 80, 81, 152, 82, 63, 63, 83, 84, 63, 87, 38, 88, 89, 90,
387, 91, 92, 108, 98, 26, 26, 99, 21, 24, 21, 100, 25, 63, 101, 102, 109, 33, 33,
33, 33, 33, 33, 33, 33, 33, 33, 65, 65, 110, 127, 65, 128, 26, 33, 33, 33, 33,
33, 33, 129, 153, 35, 35, 35, 35, 35, 35, 35, 35, 65, 130, 78, 78, 131, 132, 78,
377, 388, 371, 253, 154, - 1, 144, 135, - 1, 33, 33, 33, 33, 33, 33, 39, - 1, 22,
78, - 1, 155, 156, 157, 78, 78, 158, 35, 78, 85, 85, - 1, 159, 85, - 1, 27, 237,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 78, 231, 222, 166, 167, 85, 21,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, 168, 169, - 1, 125, 28,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 93, 93, 191, 170, 93, 117, 117,
85, 85, 117, 171, 85, 95, 95, 192, 172, 95, 103, 103, 184, 176, 103, 177, 93,
178, 179, 21, 182, 117, 183, 85, 185, 106, 106, 21, 95, 106, 35, 186, 189, 103,
190, 118, 119, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 105, 106, 214,
193, 194, 21, 21, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 21, 195,
28, 196, 118, 29, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118,
118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 47, 47, 22,
208, 47, 200, 201, 35, 30, 30, 30, 30, 30, 30, 30, 30, 111, 111, 113, 113, 111,
- 1, 113, 47, 116, 116, 202, 107, 116, 124, 124, 203, - 1, 124, 204, 205, 124,
124, 48, 111, 124, 113, 206, 209, 210, 211, - 1, 116, 212, 213, 49, 215, 124,
216, 159, 159, 219, 31, 159, 124, 140, 140, 140, 140, 140, 140, 140, 140, 35, 35,
35, 35, 35, 35, 35, 35, 220, 159, 41, 41, 32, 225, 41, 226, 227, 160, 228, 229,
230, 232, 50, 223, 51, 173, 173, - 1, 52, 173, 233, 234, 235, 41, 53, 54, 149,
149, 149, 149, 149, 149, 149, 149, 224, 236, 238, 247, 173, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 240, 241, 239, 242, 245, 114, 28, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 246,
248, 249, 250, 42, 251, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 180, 180, 43, 252, 180, 304, 305,
33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 187, 187, 290, 284, 187, 274, 180, 33,
33, 33, 33, 33, 33, 35, 35, 35, 35, 35, 35, 35, 35, 267, 260, 187, 261, 262, 263,
264, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 55, 33, 33, 33, 33, 33, 33, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 265, 266, 268, 269, 44, 270, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 271, 272, 273, 28, 275, 276, 277, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 278, 279, 280, 281, 44, 282, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 63, 63, 183, 183, 63,
283, 183, 285, 286, 287, 197, 197, 288, 289, 197, 291, 292, 293, 294, 295, 254,
255, 256, 63, 257, 183, 173, 173, 258, 296, 173, 160, 160, 197, 107, 160, 297,
259, 298, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 173, 299, 300, 301, 302, 160,
303, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 367, 306, 307, 308, 64, 309, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
19, 65, 65, 161, 310, 65, 311, 312, 313, 314, 162, 174, 318, 163, 319, 320, 321,
322, 175, 323, 324, 325, 328, 164, 65, 165, 329, 180, 180, 187, 187, 180, 349,
187, 343, 335, 336, 337, 338, 339, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 180,
340, 187, 341, 342, 344, 345, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 346, 347, 348, 350, 64, 351,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 93, 93, 190, 190, 93, 352, 190, 353, 181, 354, 188, 356,
357, 358, 359, 360, 362, 363, 364, 365, 366, 368, 369, 93, 372, 190, 197, 197,
373, 374, 197, 375, 376, 378, 105, 379, 380, 381, 382, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 197, 383, 266, 482, 460, 451, 426, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 423,
370, 424, 303, 94, 419, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 31, 95, 95, 206, 206, 95, 480,
206, 198, 394, 394, 422, 458, 394, 483, 425, 483, 483, 199, 150, 421, 417, 455,
114, 95, 454, 206, 394, 394, 481, 459, 394, 394, 479, 22, 207, 366, 483, 456,
457, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 394, 114, 22, 35, 35, 114, 31, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 114, 418, 35, 35, 94, 31, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 103, 103,
213, 213, 103, 31, 213, 369, 369, 354, 342, 369, 393, 393, 393, 393, 393, 393,
393, 393, 36, 36, 450, 103, 348, 213, 221, 207, 55, 383, 369, 46, 283, 360, 55,
273, 22, 31, 370, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 22, 450, 490,
36, 36, 450, 491, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 450, 492,
493, 494, 104, 495, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 105, 106,
106, 217, 217, 106, 22, 217, 31, 31, 31, 31, 31, 31, 31, 31, 0, 0, 273, 0, 0, 0,
0, 0, 106, 0, 217, 477, 477, 477, 477, 477, 477, 477, 477, 114, 0, 0, 0, 0, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 0, 0, 0, 0, 0, 0, 0, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 0, 0, 0, 0, 104, 0, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 111, 111, 220, 220, 111, 452, 220, 453, 453, 453, 453,
453, 453, 453, 453, 453, 453, 273, 0, 0, 0, 0, 0, 111, 0, 220, 478, 478, 478,
478, 478, 478, 478, 478, 221, 0, 0, 0, 0, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 0, 0, 0, 0, 0, 0, 0, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
0, 0, 0, 0, 112, 0, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 36, 113,
113, 224, 224, 113, 432, 224, 432, 0, 0, 433, 433, 433, 433, 433, 433, 433, 433,
433, 433, 0, 0, 0, 113, 0, 224, 483, 0, 483, 483, 0, 0, 0, 0, 36, 0, 0, 0, 0,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 483, 0, 0, 0, 0, 0, 0, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 0, 0, 0, 0, 112, 0, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
112, 112, 112, 112, 112, 112, 112, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 22, 0, 0, 0, 0, 0, 0, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 0, 0, 0, 0, 120, 0, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
121, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22, 0, 0, 0, 0, 0, 0, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 0, 0, 0, 0, 120, 0, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 22, 0, 0, 0, 0, - 1, 0, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, - 1, - 1, 0, - 1, 120, 0, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 0, 0, 0, 0, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 123,
0, 0, 0, 0, 0, 462, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 0, 0, 0, 0,
121, 0, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 28, 0, 0, 136, 125, 0, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 0, 0, 28, 0, 0, 145, 137, 137, 137, 137, 137, 137, 137, 137, 139,
139, 139, 139, 139, 139, 139, 139, 139, 139, 330, 230, 230, 0, 331, 230, - 1,
139, 139, 139, 139, 139, 139, 332, 0, 333, 146, 146, 146, 146, 146, 146, 146,
146, 230, 0, 0, 0, 0, 0, 0, 31, 0, 35, - 1, 0, 0, 0, 0, 139, 139, 139, 139, 139,
139, 0, 0, 0, 0, 0, 0, 334, 0, 0, 138, 139, 139, 139, 139, 139, 139, 139, 139,
139, 139, 31, 0, 0, 0, 0, 0, 0, 139, 139, 139, 139, 139, 139, 148, 148, 148, 148,
148, 148, 148, 148, 148, 148, 147, 0, 0, 0, 0, 0, 0, 148, 148, 148, 148, 148,
148, 0, 0, 0, 139, 139, 139, 139, 139, 139, 0, 0, 0, 0, 0, 0, 148, 148, 148, 148,
148, 148, 148, 148, 148, 148, 0, 148, 148, 148, 148, 148, 148, 148, 148, 148,
148, 148, 148, 217, 217, 236, 236, 217, 0, 236, 0, 0, 0, 246, 246, 252, 252, 246,
19, 252, 0, 389, 0, 0, 0, 0, 217, 0, 236, 148, 148, 148, 148, 148, 148, 114, 246,
19, 252, 0, 0, 0, 0, 0, 0, 31, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 390, 390, 390, 390,
390, 390, 390, 390, 392, 392, 392, 392, 392, 392, 392, 392, 392, 392, 0, 0, 0, 0,
0, 0, 0, 392, 392, 392, 392, 392, 392, 0, 0, 392, 392, 392, 392, 392, 392, 392,
392, 392, 392, 218, 0, 0, 0, 0, 0, 28, 392, 392, 392, 392, 392, 392, 0, 392, 392,
392, 392, 392, 392, 0, 0, 0, 0, 0, 0, 0, 0, 0, 391, 0, 0, 0, 0, 0, 0, 0, 0, 0,
392, 392, 392, 392, 392, 392, 427, 0, 438, 438, 438, 438, 438, 438, 438, 438,
439, 439, 433, 433, 433, 433, 433, 433, 433, 433, 433, 433, 0, 429, 0, 0, 0, 0,
0, 0, 440, 0, 0, 0, 0, 0, 0, 0, 0, 441, 0, 0, 442, 427, 0, 428, 428, 428, 428,
428, 428, 428, 428, 428, 428, 429, 0, 0, 0, 0, 0, 0, 440, 0, 0, 0, 429, 0, 0, 0,
0, 441, 0, 430, 442, 0, 0, 0, 436, 0, 436, 0, 431, 437, 437, 437, 437, 437, 437,
437, 437, 437, 437, 0, 0, 0, 0, 0, 429, 0, 0, 0, 0, 0, 0, 430, 0, 0, 0, 0, 0, 0,
0, 0, 431, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 0, 0, 0, 0, 0, 0, 0,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 0, 0, 0, 0, 420, 0, 420, 420,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
420, 420, 0, 0, 0, 0, 0, 0, 0, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
0, 0, 0, 0, 420, 0, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 434, 434,
434, 434, 434, 434, 434, 434, 434, 434, 437, 437, 437, 437, 437, 437, 437, 437,
437, 437, 0, 435, 107, 0, 0, 0, 0, 427, 107, 428, 428, 428, 428, 428, 428, 428,
428, 428, 428, 433, 433, 433, 433, 433, 433, 433, 433, 433, 433, 0, 429, 0, 0,
435, 107, 0, 0, 430, 0, 0, 107, 105, 0, 0, 0, 0, 431, 105, 0, 434, 434, 434, 434,
434, 434, 434, 434, 434, 434, 0, 0, 0, 429, 0, 0, 0, 0, 0, 0, 430, 435, 107, 0,
105, 0, 0, 0, 107, 431, 105, 0, 0, 437, 437, 437, 437, 437, 437, 437, 437, 437,
437, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 435, 107, 107, 0, 0, 0, 0, 107, 107, 427, 0,
438, 438, 438, 438, 438, 438, 438, 438, 439, 439, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 429, 0, 107, 0, 0, 0, 0, 448, 107, 0, 0, 0, 0, 0, 0, 427, 449, 439, 439, 439,
439, 439, 439, 439, 439, 439, 439, 0, 0, 0, 0, 0, 429, 0, 0, 0, 0, 0, 429, 448,
0, 0, 0, 0, 0, 446, 0, 0, 449, 0, 0, 0, 0, 0, 447, 0, 0, 453, 453, 453, 453, 453,
453, 453, 453, 453, 453, 0, 0, 0, 429, 0, 0, 0, 0, 0, 0, 446, 435, 107, 0, 0, 0,
0, 0, 107, 447, 443, 443, 443, 443, 443, 443, 443, 443, 443, 443, 0, 0, 0, 0, 0,
0, 0, 443, 443, 443, 443, 443, 443, 435, 107, 0, 0, 0, 0, 0, 107, 0, 0, 0, 0, 0,
0, 0, 443, 443, 443, 443, 443, 443, 443, 443, 443, 443, 0, 443, 443, 443, 443,
443, 443, 443, 443, 443, 443, 443, 443, 0, 0, 0, 463, 0, 444, 0, 0, 464, 0, 0, 0,
0, 0, 445, 0, 0, 465, 465, 465, 465, 465, 465, 465, 465, 0, 443, 443, 443, 443,
443, 443, 466, 0, 0, 0, 0, 444, 0, 0, 0, 0, 0, 0, 0, 0, 445, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 467, 0, 0, 0, 0, 468, 469, 0, 0, 0, 470, 0, 0, 0, 0, 0, 0,
0, 471, 0, 0, 0, 472, 0, 473, 0, 474, 0, 475, 476, 476, 476, 476, 476, 476, 476,
476, 476, 476, 0, 0, 0, 0, 0, 0, 0, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, 0, 0, 0, 0, 0, 0, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 289,
0, 0, 0, 0, 0, 0, 0, 0, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 0, 0,
0, 0, 0, 0, 0, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 0, 0, 0, 0, 0,
0, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 21, 0, 0, 485, 0, 0, 0,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 0, 0, 0, 0, 0, 0, 0, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 0, 0, 0, 0, 484, 0, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 487, 0, 0, 0, 0, 0, 0, 0, 486, 486, 486, 486, 486,
486, 486, 486, 486, 486, 0, 0, 0, 0, 0, 0, 0, 486, 486, 486, 486, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 486, 486, 0, 0, 0, 0, 486, 0, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0}; const int Cyc_lex_check[ 3735u]=( const int[ 3735u]){ - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 0, 25, 29, 0, 134, 136, 143, 145, 386, 389, 41, 41,
45, 45, 41, 488, 45, - 1, - 1, - 1, - 1, - 1, 132, 314, 325, 0, 376, 10, 12, 40,
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
151, 4, 152, 153, 5, 115, 6, 8, 115, 32, 32, 32, 32, 32, 32, 16, 122, 16, 77,
122, 154, 155, 156, 78, 78, 157, 7, 78, 84, 84, 123, 158, 84, 123, 17, 161, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 78, 162, 163, 165, 166, 84, 3, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 142, 167, 168, 142, 11, 65, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 92, 92, 164, 169,
92, 13, 13, 85, 85, 13, 170, 85, 95, 95, 164, 171, 95, 102, 102, 174, 175, 102,
176, 92, 177, 178, 78, 181, 13, 182, 85, 184, 106, 106, 5, 95, 106, 16, 185, 188,
102, 189, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 4, 106, 191, 192, 193,
6, 8, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 17, 194, 27, 195, 13, 27, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14,
14, 85, 198, 14, 199, 200, 95, 27, 27, 27, 27, 27, 27, 27, 27, 110, 110, 113,
113, 110, 115, 113, 14, 116, 116, 201, 106, 116, 117, 117, 202, 122, 117, 203,
204, 124, 124, 14, 110, 124, 113, 205, 208, 209, 210, 123, 116, 211, 212, 14,
214, 117, 215, 159, 159, 218, 27, 159, 124, 137, 137, 137, 137, 137, 137, 137,
137, 140, 140, 140, 140, 140, 140, 140, 140, 219, 159, 15, 15, 27, 223, 15, 225,
226, 159, 227, 228, 229, 231, 14, 222, 14, 172, 172, 142, 14, 172, 232, 233, 234,
15, 14, 14, 146, 146, 146, 146, 146, 146, 146, 146, 222, 235, 237, 238, 172, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 239, 240, 237, 241, 244, 113, 13, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 245, 247, 248, 249, 15, 250, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 179, 179, 15,
251, 179, 254, 254, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 186, 186, 255, 256,
186, 257, 179, 33, 33, 33, 33, 33, 33, 149, 149, 149, 149, 149, 149, 149, 149,
258, 259, 186, 260, 261, 262, 263, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 14,
33, 33, 33, 33, 33, 33, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 264, 265, 267, 268, 42, 269, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 270, 271, 272, 15,
274, 275, 276, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 277, 278, 279, 280, 44, 281, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 63, 63, 183, 183, 63, 282, 183, 284, 285, 286, 196, 196, 287, 288,
196, 290, 291, 292, 293, 294, 253, 253, 253, 63, 253, 183, 173, 173, 253, 295,
173, 160, 160, 196, 183, 160, 296, 253, 297, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 173, 298, 299, 300, 301, 160, 302, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 304, 305, 306,
307, 63, 308, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64, 160, 309, 64, 310, 311, 312, 313,
160, 173, 317, 160, 318, 319, 320, 321, 173, 322, 323, 324, 327, 160, 64, 160,
328, 180, 180, 187, 187, 180, 332, 187, 333, 334, 335, 336, 337, 338, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 180, 339, 187, 340, 341, 343, 344, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 345, 346, 347, 349, 64, 350, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 93, 93, 190, 190, 93,
351, 190, 352, 180, 353, 187, 355, 356, 357, 358, 359, 361, 362, 363, 364, 365,
367, 368, 93, 371, 190, 197, 197, 372, 373, 197, 374, 375, 377, 190, 378, 379,
380, 381, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 197, 382, 398, 400, 403, 407,
410, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 411, 411, 411, 412, 93, 415, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 94, 94, 206, 206, 94, 401, 206, 197, 387, 387, 421, 404, 387, 396, 423, 396,
396, 197, 413, 413, 416, 405, 430, 94, 452, 206, 394, 394, 401, 404, 394, 387,
461, 387, 206, 463, 396, 405, 405, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 394,
431, 394, 440, 441, 430, 444, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 431, 416, 440, 441, 94, 444,
94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,
94, 94, 94, 94, 94, 94, 103, 103, 213, 213, 103, 445, 213, 369, 369, 464, 466,
369, 390, 390, 390, 390, 390, 390, 390, 390, 446, 447, 448, 103, 467, 213, 468,
469, 470, 471, 369, 472, 473, 474, 213, 478, 485, 445, 369, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 487, 449, 489, 446, 447, 448, 490, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 449, 491, 492, 493, 103, 494, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 104, 104, 216, 216, 104, 495, 216, 393,
393, 393, 393, 393, 393, 393, 393, - 1, - 1, 465, - 1, - 1, - 1, - 1, - 1, 104,
- 1, 216, 465, 465, 465, 465, 465, 465, 465, 465, 216, - 1, - 1, - 1, - 1, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, - 1, - 1, - 1, - 1, 104, - 1,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 111, 111, 220, 220, 111, 406,
220, 406, 406, 406, 406, 406, 406, 406, 406, 406, 406, 477, - 1, - 1, - 1, - 1,
- 1, 111, - 1, 220, 477, 477, 477, 477, 477, 477, 477, 477, 220, - 1, - 1, - 1,
- 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, - 1, - 1, - 1, - 1,
111, - 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 112, 112, 224,
224, 112, 429, 224, 429, - 1, - 1, 429, 429, 429, 429, 429, 429, 429, 429, 429,
429, - 1, - 1, - 1, 112, - 1, 224, 483, - 1, 483, 483, - 1, - 1, - 1, - 1, 224,
- 1, - 1, - 1, - 1, 112, 112, 112, 112, 112, 112, 112, 112, 112, 112, 483, - 1,
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
120, 120, 120, 120, 120, 120, - 1, - 1, - 1, - 1, 402, - 1, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 121, 402, - 1, 121, 120, - 1, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
120, 120, 120, 120, 120, 120, - 1, - 1, - 1, - 1, 121, 121, 121, 121, 121, 121,
121, 121, 121, 121, 121, 121, 121, - 1, - 1, - 1, - 1, - 1, 402, 121, 121, 121,
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
402, 138, 138, 138, 138, 138, 138, 329, - 1, 329, 144, 144, 144, 144, 144, 144,
144, 144, 230, - 1, - 1, - 1, - 1, - 1, - 1, 135, - 1, 230, 121, - 1, - 1, - 1,
- 1, 138, 138, 138, 138, 138, 138, - 1, - 1, - 1, - 1, - 1, - 1, 329, - 1, - 1,
135, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 144, - 1, - 1, - 1, - 1,
- 1, - 1, 139, 139, 139, 139, 139, 139, 147, 147, 147, 147, 147, 147, 147, 147,
147, 147, 144, - 1, - 1, - 1, - 1, - 1, - 1, 147, 147, 147, 147, 147, 147, - 1,
- 1, - 1, 139, 139, 139, 139, 139, 139, - 1, - 1, - 1, - 1, - 1, - 1, 148, 148,
148, 148, 148, 148, 148, 148, 148, 148, - 1, 147, 147, 147, 147, 147, 147, 148,
148, 148, 148, 148, 148, 217, 217, 236, 236, 217, - 1, 236, - 1, - 1, - 1, 246,
246, 252, 252, 246, 388, 252, - 1, 388, - 1, - 1, - 1, - 1, 217, - 1, 236, 148,
148, 148, 148, 148, 148, 217, 246, 236, 252, - 1, - 1, - 1, - 1, - 1, - 1, 246,
- 1, 252, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 388, 388, 388, 388, 388, 388,
388, 388, 391, 391, 391, 391, 391, 391, 391, 391, 391, 391, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 391, 391, 391, 391, 391, 391, - 1, - 1, 392, 392, 392, 392, 392,
392, 392, 392, 392, 392, 217, - 1, - 1, - 1, - 1, - 1, 388, 392, 392, 392, 392,
392, 392, - 1, 391, 391, 391, 391, 391, 391, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 388, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 392, 392, 392, 392,
392, 392, 408, - 1, 408, 408, 408, 408, 408, 408, 408, 408, 408, 408, 432, 432,
432, 432, 432, 432, 432, 432, 432, 432, - 1, 408, - 1, - 1, - 1, - 1, - 1, - 1,
408, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 408, - 1, - 1, 408, 409, - 1, 409,
409, 409, 409, 409, 409, 409, 409, 409, 409, 408, - 1, - 1, - 1, - 1, - 1, - 1,
408, - 1, - 1, - 1, 409, - 1, - 1, - 1, - 1, 408, - 1, 409, 408, - 1, - 1, - 1,
435, - 1, 435, - 1, 409, 435, 435, 435, 435, 435, 435, 435, 435, 435, 435, - 1,
- 1, - 1, - 1, - 1, 409, - 1, - 1, - 1, - 1, - 1, - 1, 409, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 409, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414,
414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414,
- 1, - 1, - 1, - 1, 414, - 1, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414,
414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414, 414,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, - 1, - 1, - 1, - 1, 420,
- 1, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420,
420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 420, 427, 427, 427, 427, 427,
427, 427, 427, 427, 427, 436, 436, 436, 436, 436, 436, 436, 436, 436, 436, - 1,
427, 427, - 1, - 1, - 1, - 1, 428, 427, 428, 428, 428, 428, 428, 428, 428, 428,
428, 428, 433, 433, 433, 433, 433, 433, 433, 433, 433, 433, - 1, 428, - 1, - 1,
427, 427, - 1, - 1, 428, - 1, - 1, 427, 433, - 1, - 1, - 1, - 1, 428, 433, - 1,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, - 1, - 1, - 1, 428, - 1, - 1,
- 1, - 1, - 1, - 1, 428, 434, 434, - 1, 433, - 1, - 1, - 1, 434, 428, 433, - 1,
- 1, 437, 437, 437, 437, 437, 437, 437, 437, 437, 437, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 434, 434, 437, - 1, - 1, - 1, - 1, 434, 437, 438, - 1,
438, 438, 438, 438, 438, 438, 438, 438, 438, 438, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 438, - 1, 437, - 1, - 1, - 1, - 1, 438, 437, - 1, - 1,
- 1, - 1, - 1, - 1, 439, 438, 439, 439, 439, 439, 439, 439, 439, 439, 439, 439,
- 1, - 1, - 1, - 1, - 1, 438, - 1, - 1, - 1, - 1, - 1, 439, 438, - 1, - 1, - 1,
- 1, - 1, 439, - 1, - 1, 438, - 1, - 1, - 1, - 1, - 1, 439, - 1, - 1, 453, 453,
453, 453, 453, 453, 453, 453, 453, 453, - 1, - 1, - 1, 439, - 1, - 1, - 1, - 1,
- 1, - 1, 439, 453, 453, - 1, - 1, - 1, - 1, - 1, 453, 439, 442, 442, 442, 442,
442, 442, 442, 442, 442, 442, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 442, 442, 442,
442, 442, 442, 453, 453, - 1, - 1, - 1, - 1, - 1, 453, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 443, 443, 443, 443, 443, 443, 443, 443, 443, 443, - 1, 442, 442, 442,
442, 442, 442, 443, 443, 443, 443, 443, 443, - 1, - 1, - 1, 462, - 1, 443, - 1,
- 1, 462, - 1, - 1, - 1, - 1, - 1, 443, - 1, - 1, 462, 462, 462, 462, 462, 462,
462, 462, - 1, 443, 443, 443, 443, 443, 443, 462, - 1, - 1, - 1, - 1, 443, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 443, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 462, - 1, - 1, - 1, - 1, 462, 462, - 1, - 1, - 1,
462, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 462, - 1, - 1, - 1, 462, - 1, 462, - 1,
462, - 1, 462, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, - 1, - 1,
- 1, - 1, - 1, - 1, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 476, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, - 1, - 1, - 1, - 1, - 1, - 1, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476, 476,
476, 476, 484, - 1, - 1, 484, - 1, - 1, - 1, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, - 1, - 1, - 1, - 1, 484, - 1, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 486, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 486, 486, 486, 486,
486, 486, 486, 486, 486, 486, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, - 1, - 1, - 1, - 1, 486, - 1, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
486, 486, 486, 486, 486, 486, 486, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
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
while( 1) { base= Cyc_lex_base[ _check_known_subscript_notnull( 496u, state)];
if( base <  0){ return( - base) -  1;} backtrk= Cyc_lex_backtrk[
_check_known_subscript_notnull( 496u, state)]; if( backtrk >=  0){ lbuf->lex_last_pos=
lbuf->lex_curr_pos; lbuf->lex_last_action= backtrk;} if( lbuf->lex_curr_pos >= 
lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){ return( - state) -  1;}
else{ c= 256;}} else{ c=( int)*(( unsigned char*) _check_unknown_subscript( lbuf->lex_buffer,
sizeof( unsigned char), lbuf->lex_curr_pos ++)); if( c ==  - 1){ c= 256;}} if(
Cyc_lex_check[ _check_known_subscript_notnull( 3735u, base +  c)] ==  state){
state= Cyc_lex_trans[ _check_known_subscript_notnull( 3735u, base +  c)];} else{
state= Cyc_lex_default[ _check_known_subscript_notnull( 496u, state)];} if(
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
return Cyc_slurp( lexbuf); case 15: _LL155: return Cyc_slurp( lexbuf); case 16:
_LL157: return Cyc_slurp( lexbuf); case 17: _LL158: return Cyc_slurp( lexbuf);
case 18: _LL159: return Cyc_slurp( lexbuf); case 19: _LL160: return Cyc_slurp(
lexbuf); case 20: _LL161: return Cyc_slurp( lexbuf); case 21: _LL162: Cyc_Std_fputs(
_tag_arr("inline", sizeof( unsigned char), 7u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 22: _LL163: Cyc_Std_fputs(
_tag_arr("const", sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 23: _LL164: Cyc_Std_fputs(
_tag_arr("const", sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 24: _LL165: Cyc_Std_fputs(
_tag_arr("int", sizeof( unsigned char), 4u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 25: _LL166: return
Cyc_slurp( lexbuf); case 26: _LL167: Cyc_parens_to_match= 1; if( Cyc_asm( lexbuf)){
return 1;} Cyc_Std_fputs( _tag_arr("0", sizeof( unsigned char), 2u),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out));({ void* _temp169[ 0u]={}; Cyc_log( _tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof( unsigned char), 42u), _tag_arr( _temp169, sizeof( void*), 0u));});
return Cyc_slurp( lexbuf); case 27: _LL168: Cyc_Std_fputc(( int) Cyc_Lexing_lexeme_char(
lexbuf, 0),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); return Cyc_slurp(
lexbuf); default: _LL170:( lexbuf->refill_buff)( lexbuf); return Cyc_slurp_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp172=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp172[ 0]=({ struct Cyc_Lexing_Error_struct _temp173; _temp173.tag= Cyc_Lexing_Error;
_temp173.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp173;}); _temp172;}));} int Cyc_slurp( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_slurp_rec( lexbuf, 5);} int Cyc_slurp_string_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL174: Cyc_Std_fputc(( int)'"',( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return 0; case 1: _LL175: return 1; case 2: _LL176:
return 1; case 3: _LL177:({ struct Cyc_Std_String_pa_struct _temp180; _temp180.tag=
Cyc_Std_String_pa; _temp180.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{
void* _temp179[ 1u]={& _temp180}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp179, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 4: _LL178:({ struct Cyc_Std_String_pa_struct _temp183; _temp183.tag= Cyc_Std_String_pa;
_temp183.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp182[ 1u]={&
_temp183}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp182, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 5: _LL181:({ struct Cyc_Std_String_pa_struct
_temp186; _temp186.tag= Cyc_Std_String_pa; _temp186.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp185[ 1u]={& _temp186}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp185, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 6: _LL184:({ struct Cyc_Std_String_pa_struct _temp189; _temp189.tag= Cyc_Std_String_pa;
_temp189.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp188[ 1u]={&
_temp189}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp188, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 7: _LL187:({ struct Cyc_Std_String_pa_struct
_temp192; _temp192.tag= Cyc_Std_String_pa; _temp192.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp191[ 1u]={& _temp192}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp191, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 8: _LL190:({ struct Cyc_Std_String_pa_struct _temp195; _temp195.tag= Cyc_Std_String_pa;
_temp195.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp194[ 1u]={&
_temp195}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp194, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); default: _LL193:( lexbuf->refill_buff)(
lexbuf); return Cyc_slurp_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp197=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp197[ 0]=({ struct Cyc_Lexing_Error_struct
_temp198; _temp198.tag= Cyc_Lexing_Error; _temp198.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp198;}); _temp197;}));} int Cyc_slurp_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_slurp_string_rec( lexbuf, 6);} int
Cyc_asm_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL199: if( Cyc_parens_to_match == 
1){ return 0;} Cyc_parens_to_match --; return Cyc_asm( lexbuf); case 1: _LL200:
Cyc_parens_to_match ++; return Cyc_asm( lexbuf); case 2: _LL201: Cyc_asm_string(
lexbuf); return Cyc_asm( lexbuf); case 3: _LL202: Cyc_asm_comment( lexbuf);
return Cyc_asm( lexbuf); case 4: _LL203: return Cyc_asm( lexbuf); case 5: _LL204:
return 0; case 6: _LL205: return Cyc_asm( lexbuf); default: _LL206:( lexbuf->refill_buff)(
lexbuf); return Cyc_asm_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp208=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp208[ 0]=({ struct Cyc_Lexing_Error_struct
_temp209; _temp209.tag= Cyc_Lexing_Error; _temp209.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp209;}); _temp208;}));} int Cyc_asm( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_rec( lexbuf, 7);} int Cyc_asm_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL210: return 0; case 1: _LL211:
return 1; case 2: _LL212: return 1; case 3: _LL213: return Cyc_asm_string(
lexbuf); case 4: _LL214: return Cyc_asm_string( lexbuf); case 5: _LL215: return
Cyc_asm_string( lexbuf); case 6: _LL216: return Cyc_asm_string( lexbuf); case 7:
_LL217: return Cyc_asm_string( lexbuf); case 8: _LL218: return Cyc_asm_string(
lexbuf); default: _LL219:( lexbuf->refill_buff)( lexbuf); return Cyc_asm_string_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp221=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp221[ 0]=({ struct Cyc_Lexing_Error_struct _temp222; _temp222.tag= Cyc_Lexing_Error;
_temp222.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp222;}); _temp221;}));} int Cyc_asm_string( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_asm_string_rec( lexbuf, 8);} int Cyc_asm_comment_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL223: return 0; case 1: _LL224: return 1; case 2: _LL225:
return Cyc_asm_comment( lexbuf); default: _LL226:( lexbuf->refill_buff)( lexbuf);
return Cyc_asm_comment_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp228=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp228[ 0]=({ struct Cyc_Lexing_Error_struct
_temp229; _temp229.tag= Cyc_Lexing_Error; _temp229.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp229;}); _temp228;}));} int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_comment_rec( lexbuf, 9);}
struct _tuple9* Cyc_suck_line_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case
0: _LL230: Cyc_current_line= _tag_arr("#define ", sizeof( unsigned char), 9u);
Cyc_suck_macroname( lexbuf); return({ struct _tuple9* _temp232=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp232->f1= Cyc_current_line; _temp232->f2=(
struct _tagged_arr*) _check_null( Cyc_current_source); _temp232;}); case 1:
_LL231: return Cyc_suck_line( lexbuf); case 2: _LL233: return 0; default: _LL234:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_line_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp236=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp236[ 0]=({ struct Cyc_Lexing_Error_struct
_temp237; _temp237.tag= Cyc_Lexing_Error; _temp237.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp237;}); _temp236;}));} struct _tuple9* Cyc_suck_line(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_line_rec( lexbuf, 10);} int
Cyc_suck_macroname_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL238:
Cyc_current_source=({ struct _tagged_arr* _temp240=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp240[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp240;}); Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat(
Cyc_current_line,*(( struct _tagged_arr*) _check_null( Cyc_current_source)));
return Cyc_suck_restofline( lexbuf); default: _LL239:( lexbuf->refill_buff)(
lexbuf); return Cyc_suck_macroname_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp242=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp242[ 0]=({ struct Cyc_Lexing_Error_struct
_temp243; _temp243.tag= Cyc_Lexing_Error; _temp243.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp243;}); _temp242;}));} int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_macroname_rec( lexbuf, 11);}
int Cyc_suck_restofline_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL244:
Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat( Cyc_current_line,(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return 0; default: _LL245:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_restofline_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp247=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp247[ 0]=({ struct Cyc_Lexing_Error_struct
_temp248; _temp248.tag= Cyc_Lexing_Error; _temp248.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp248;}); _temp247;}));} int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_restofline_rec( lexbuf, 12);}
struct _tuple11* Cyc_spec_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL249:
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
_tuple11* _temp251=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11));
_temp251->f1= Cyc_current_header; _temp251->f2= Cyc_current_symbols; _temp251->f3=
Cyc_current_omit_symbols; _temp251->f4= Cyc_current_prologue; _temp251->f5= Cyc_current_epilogue;
_temp251->f6= Cyc_current_cstubs; _temp251->f7= Cyc_current_cycstubs; _temp251;});
case 1: _LL250: return Cyc_spec( lexbuf); case 2: _LL252: return Cyc_spec(
lexbuf); case 3: _LL253: return 0; case 4: _LL254:({ struct Cyc_Std_Int_pa_struct
_temp257; _temp257.tag= Cyc_Std_Int_pa; _temp257.f1=( int)(( unsigned int)(( int)
Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void* _temp256[ 1u]={& _temp257}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof( unsigned char), 67u), _tag_arr( _temp256, sizeof( void*), 1u));}});
return 0; default: _LL255:( lexbuf->refill_buff)( lexbuf); return Cyc_spec_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp259=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp259[ 0]=({ struct Cyc_Lexing_Error_struct _temp260; _temp260.tag= Cyc_Lexing_Error;
_temp260.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp260;}); _temp259;}));} struct _tuple11* Cyc_spec( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_spec_rec( lexbuf, 13);} int Cyc_commands_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL261: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols( lexbuf)){
return 1;} Cyc_current_symbols=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols, Cyc_current_symbols);
return Cyc_commands( lexbuf); case 1: _LL262: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols(
lexbuf)){ return 1;} Cyc_current_omit_symbols=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols,
Cyc_current_omit_symbols); return Cyc_commands( lexbuf); case 2: _LL263: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp266=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp266->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp266->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp266;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp265=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp265->hd=( void*) x; _temp265->tl= Cyc_current_prologue;
_temp265;}); return Cyc_commands( lexbuf);} case 3: _LL264: { struct _tagged_arr
_temp268= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp268,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("prologue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp268, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp268, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp268; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp269= Cyc_Std_substring(( struct _tagged_arr) _temp268, 0,(
unsigned int)(( t.curr -  _temp268.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp271=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp271->f1=( struct
_tagged_arr) _temp269; _temp271->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp271;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp270=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp270->hd=( void*) x; _temp270->tl= Cyc_current_prologue;
_temp270;}); return Cyc_commands( lexbuf);}}}} case 4: _LL267: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp274=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp274->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp274->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp274;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp273=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp273->hd=( void*) x; _temp273->tl= Cyc_current_epilogue;
_temp273;}); return Cyc_commands( lexbuf);} case 5: _LL272: { struct _tagged_arr
_temp276= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp276,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("epilogue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp276, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp276, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp276; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp277= Cyc_Std_substring(( struct _tagged_arr) _temp276, 0,(
unsigned int)(( t.curr -  _temp276.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp279=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp279->f1=( struct
_tagged_arr) _temp277; _temp279->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp279;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp278=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp278->hd=( void*) x; _temp278->tl= Cyc_current_epilogue;
_temp278;}); return Cyc_commands( lexbuf);}}}} case 6: _LL275: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp282=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp282->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp282->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp282;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp281=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp281->hd=( void*) x; _temp281->tl= Cyc_current_cstubs;
_temp281;}); return Cyc_commands( lexbuf);} case 7: _LL280: { struct _tagged_arr
_temp284= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp284,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cstub", sizeof(
unsigned char), 6u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp284, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp284, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp284; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp285= Cyc_Std_substring(( struct _tagged_arr) _temp284, 0,(
unsigned int)(( t.curr -  _temp284.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp287=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp287->f1=( struct
_tagged_arr) _temp285; _temp287->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp287;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp286=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp286->hd=( void*) x; _temp286->tl= Cyc_current_cstubs;
_temp286;}); return Cyc_commands( lexbuf);}}}} case 8: _LL283: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp290=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp290->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp290->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp290;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp289=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp289->hd=( void*) x; _temp289->tl= Cyc_current_cycstubs;
_temp289;}); return Cyc_commands( lexbuf);} case 9: _LL288: { struct _tagged_arr
_temp292= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp292,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cycstub", sizeof(
unsigned char), 8u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp292, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp292, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp292; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp293= Cyc_Std_substring(( struct _tagged_arr) _temp292, 0,(
unsigned int)(( t.curr -  _temp292.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp295=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp295->f1=( struct
_tagged_arr) _temp293; _temp295->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp295;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp294=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp294->hd=( void*) x; _temp294->tl= Cyc_current_cycstubs;
_temp294;}); return Cyc_commands( lexbuf);}}}} case 10: _LL291: return Cyc_commands(
lexbuf); case 11: _LL296: return Cyc_commands( lexbuf); case 12: _LL297: return
0; case 13: _LL298: return 0; case 14: _LL299:({ struct Cyc_Std_Int_pa_struct
_temp302; _temp302.tag= Cyc_Std_Int_pa; _temp302.f1=( int)(( unsigned int)(( int)
Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void* _temp301[ 1u]={& _temp302}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof( unsigned char), 58u), _tag_arr( _temp301, sizeof( void*), 1u));}});
return 1; default: _LL300:( lexbuf->refill_buff)( lexbuf); return Cyc_commands_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp304=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp304[ 0]=({ struct Cyc_Lexing_Error_struct _temp305; _temp305.tag= Cyc_Lexing_Error;
_temp305.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp305;}); _temp304;}));} int Cyc_commands( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_commands_rec( lexbuf, 14);} int Cyc_snarfsymbols_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL306: Cyc_snarfed_symbols=({ struct Cyc_List_List* _temp308=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp308->hd=(
void*)({ struct _tagged_arr* _temp309=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp309[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp309;}); _temp308->tl= Cyc_snarfed_symbols; _temp308;}); return Cyc_snarfsymbols(
lexbuf); case 1: _LL307: return Cyc_snarfsymbols( lexbuf); case 2: _LL310:
return 0; case 3: _LL311:({ void* _temp313[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n", sizeof( unsigned char),
44u), _tag_arr( _temp313, sizeof( void*), 0u));}); return 1; case 4: _LL312:({
struct Cyc_Std_Int_pa_struct _temp316; _temp316.tag= Cyc_Std_Int_pa; _temp316.f1=(
int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void*
_temp315[ 1u]={& _temp316}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof( unsigned char), 57u), _tag_arr( _temp315, sizeof( void*), 1u));}});
return 1; default: _LL314:( lexbuf->refill_buff)( lexbuf); return Cyc_snarfsymbols_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp318=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp318[ 0]=({ struct Cyc_Lexing_Error_struct _temp319; _temp319.tag= Cyc_Lexing_Error;
_temp319.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp319;}); _temp318;}));} int Cyc_snarfsymbols( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_snarfsymbols_rec( lexbuf, 15);} int Cyc_block_rec( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate,
lexbuf); switch( lexstate){ case 0: _LL320: if( Cyc_braces_to_match ==  1){
return 0;} Cyc_braces_to_match --; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'}'); return Cyc_block( lexbuf); case 1: _LL321: Cyc_braces_to_match
++; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'{');
return Cyc_block( lexbuf); case 2: _LL322: Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'"'); Cyc_block_string( lexbuf); return Cyc_block(
lexbuf); case 3: _LL323: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf), _tag_arr("/*", sizeof( unsigned char), 3u)); Cyc_block_comment(
lexbuf); return Cyc_block( lexbuf); case 4: _LL324: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block( lexbuf); case 5: _LL325: return 0; case 6: _LL326:
Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf), Cyc_Lexing_lexeme_char(
lexbuf, 0)); return Cyc_block( lexbuf); default: _LL327:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp329=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp329[ 0]=({ struct Cyc_Lexing_Error_struct
_temp330; _temp330.tag= Cyc_Lexing_Error; _temp330.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp330;}); _temp329;}));} int Cyc_block( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_rec( lexbuf, 16);} int Cyc_block_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL331: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'"'); return 0; case 1: _LL332:
return 1; case 2: _LL333: return 1; case 3: _LL334: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 4: _LL335: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 5: _LL336: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 6: _LL337: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 7: _LL338: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 8: _LL339: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); default: _LL340:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp342=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp342[ 0]=({ struct Cyc_Lexing_Error_struct
_temp343; _temp343.tag= Cyc_Lexing_Error; _temp343.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp343;}); _temp342;}));} int Cyc_block_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_string_rec( lexbuf, 17);}
int Cyc_block_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL344:
Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),
_tag_arr("*/", sizeof( unsigned char), 3u)); return 0; case 1: _LL345: return 1;
case 2: _LL346: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return Cyc_block_comment(
lexbuf); default: _LL347:( lexbuf->refill_buff)( lexbuf); return Cyc_block_comment_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp349=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp349[ 0]=({ struct Cyc_Lexing_Error_struct _temp350; _temp350.tag= Cyc_Lexing_Error;
_temp350.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp350;}); _temp349;}));} int Cyc_block_comment( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_block_comment_rec( lexbuf, 18);} static void Cyc_pr( struct
_tagged_arr* sptr){({ struct Cyc_Std_String_pa_struct _temp352; _temp352.tag=
Cyc_Std_String_pa; _temp352.f1=( struct _tagged_arr)* sptr;{ void* _temp351[ 1u]={&
_temp352}; Cyc_Std_printf( _tag_arr(" %s\n", sizeof( unsigned char), 5u),
_tag_arr( _temp351, sizeof( void*), 1u));}});} extern void Cyc_scan_type( void*
t); struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
void Cyc_scan_exp( struct Cyc_Absyn_Exp* e){ void* _temp353=( void*)(( struct
Cyc_Absyn_Exp*) _check_null( e))->r; struct _tuple0* _temp431; struct _tuple0
_temp433; struct _tagged_arr* _temp434; struct _tuple0* _temp436; struct _tuple0
_temp438; struct _tagged_arr* _temp439; struct Cyc_List_List* _temp441; struct
Cyc_Absyn_Exp* _temp443; struct Cyc_Absyn_Exp* _temp445; struct Cyc_Absyn_Exp*
_temp447; struct Cyc_Absyn_Exp* _temp449; struct Cyc_Absyn_Exp* _temp451; struct
Cyc_Absyn_Exp* _temp453; struct Cyc_Absyn_Exp* _temp455; struct Cyc_Absyn_Exp*
_temp457; struct Cyc_Absyn_Exp* _temp459; struct Cyc_Absyn_Exp* _temp461; struct
Cyc_Absyn_Exp* _temp463; struct Cyc_Absyn_Exp* _temp465; struct Cyc_Absyn_Exp*
_temp467; struct Cyc_List_List* _temp469; struct Cyc_Absyn_Exp* _temp471; struct
Cyc_List_List* _temp473; struct Cyc_Absyn_Exp* _temp475; struct Cyc_Absyn_Exp*
_temp477; void* _temp479; void* _temp481; void* _temp483; struct _tagged_arr*
_temp485; struct Cyc_Absyn_Exp* _temp487; struct _tagged_arr* _temp489; struct
Cyc_Absyn_Exp* _temp491; void* _temp493; void* _temp495; struct Cyc_List_List*
_temp497; _LL355: if(*(( int*) _temp353) ==  Cyc_Absyn_Var_e){ _LL432: _temp431=((
struct Cyc_Absyn_Var_e_struct*) _temp353)->f1; _temp433=* _temp431; _LL435:
_temp434= _temp433.f2; goto _LL356;} else{ goto _LL357;} _LL357: if(*(( int*)
_temp353) ==  Cyc_Absyn_UnknownId_e){ _LL437: _temp436=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp353)->f1; _temp438=* _temp436; _LL440: _temp439= _temp438.f2; goto _LL358;}
else{ goto _LL359;} _LL359: if(*(( int*) _temp353) ==  Cyc_Absyn_Primop_e){
_LL442: _temp441=(( struct Cyc_Absyn_Primop_e_struct*) _temp353)->f2; goto
_LL360;} else{ goto _LL361;} _LL361: if(*(( int*) _temp353) ==  Cyc_Absyn_Subscript_e){
_LL446: _temp445=(( struct Cyc_Absyn_Subscript_e_struct*) _temp353)->f1; goto
_LL444; _LL444: _temp443=(( struct Cyc_Absyn_Subscript_e_struct*) _temp353)->f2;
goto _LL362;} else{ goto _LL363;} _LL363: if(*(( int*) _temp353) ==  Cyc_Absyn_SeqExp_e){
_LL450: _temp449=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp353)->f1; goto
_LL448; _LL448: _temp447=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp353)->f2;
goto _LL364;} else{ goto _LL365;} _LL365: if(*(( int*) _temp353) ==  Cyc_Absyn_AssignOp_e){
_LL454: _temp453=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp353)->f1; goto
_LL452; _LL452: _temp451=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp353)->f3;
goto _LL366;} else{ goto _LL367;} _LL367: if(*(( int*) _temp353) ==  Cyc_Absyn_Deref_e){
_LL456: _temp455=(( struct Cyc_Absyn_Deref_e_struct*) _temp353)->f1; goto _LL368;}
else{ goto _LL369;} _LL369: if(*(( int*) _temp353) ==  Cyc_Absyn_Sizeofexp_e){
_LL458: _temp457=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp353)->f1; goto
_LL370;} else{ goto _LL371;} _LL371: if(*(( int*) _temp353) ==  Cyc_Absyn_Address_e){
_LL460: _temp459=(( struct Cyc_Absyn_Address_e_struct*) _temp353)->f1; goto
_LL372;} else{ goto _LL373;} _LL373: if(*(( int*) _temp353) ==  Cyc_Absyn_Increment_e){
_LL462: _temp461=(( struct Cyc_Absyn_Increment_e_struct*) _temp353)->f1; goto
_LL374;} else{ goto _LL375;} _LL375: if(*(( int*) _temp353) ==  Cyc_Absyn_Conditional_e){
_LL468: _temp467=(( struct Cyc_Absyn_Conditional_e_struct*) _temp353)->f1; goto
_LL466; _LL466: _temp465=(( struct Cyc_Absyn_Conditional_e_struct*) _temp353)->f2;
goto _LL464; _LL464: _temp463=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp353)->f3; goto _LL376;} else{ goto _LL377;} _LL377: if(*(( int*) _temp353)
==  Cyc_Absyn_FnCall_e){ _LL472: _temp471=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp353)->f1; goto _LL470; _LL470: _temp469=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp353)->f2; goto _LL378;} else{ goto _LL379;} _LL379: if(*(( int*) _temp353)
==  Cyc_Absyn_UnknownCall_e){ _LL476: _temp475=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp353)->f1; goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp353)->f2; goto _LL380;} else{ goto _LL381;} _LL381: if(*(( int*) _temp353)
==  Cyc_Absyn_Cast_e){ _LL480: _temp479=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp353)->f1; goto _LL478; _LL478: _temp477=(( struct Cyc_Absyn_Cast_e_struct*)
_temp353)->f2; goto _LL382;} else{ goto _LL383;} _LL383: if(*(( int*) _temp353)
==  Cyc_Absyn_Malloc_e){ _LL482: _temp481=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp353)->f2; goto _LL384;} else{ goto _LL385;} _LL385: if(*(( int*) _temp353)
==  Cyc_Absyn_Sizeoftyp_e){ _LL484: _temp483=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp353)->f1; goto _LL386;} else{ goto _LL387;} _LL387: if(*(( int*) _temp353)
==  Cyc_Absyn_StructMember_e){ _LL488: _temp487=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp353)->f1; goto _LL486; _LL486: _temp485=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp353)->f2; goto _LL388;} else{ goto _LL389;} _LL389: if(*(( int*) _temp353)
==  Cyc_Absyn_StructArrow_e){ _LL492: _temp491=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp353)->f1; goto _LL490; _LL490: _temp489=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp353)->f2; goto _LL390;} else{ goto _LL391;} _LL391: if(*(( int*) _temp353)
==  Cyc_Absyn_Offsetof_e){ _LL496: _temp495=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp353)->f1; goto _LL494; _LL494: _temp493=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp353)->f2; goto _LL392;} else{ goto _LL393;} _LL393: if(*(( int*) _temp353)
==  Cyc_Absyn_Const_e){ goto _LL394;} else{ goto _LL395;} _LL395: if(*(( int*)
_temp353) ==  Cyc_Absyn_UnresolvedMem_e){ _LL498: _temp497=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp353)->f2; goto _LL396;} else{ goto _LL397;} _LL397: if(*(( int*) _temp353)
==  Cyc_Absyn_StmtExp_e){ goto _LL398;} else{ goto _LL399;} _LL399: if(*(( int*)
_temp353) ==  Cyc_Absyn_Throw_e){ goto _LL400;} else{ goto _LL401;} _LL401: if(*((
int*) _temp353) ==  Cyc_Absyn_NoInstantiate_e){ goto _LL402;} else{ goto _LL403;}
_LL403: if(*(( int*) _temp353) ==  Cyc_Absyn_Instantiate_e){ goto _LL404;} else{
goto _LL405;} _LL405: if(*(( int*) _temp353) ==  Cyc_Absyn_New_e){ goto _LL406;}
else{ goto _LL407;} _LL407: if(*(( int*) _temp353) ==  Cyc_Absyn_Gentyp_e){ goto
_LL408;} else{ goto _LL409;} _LL409: if(*(( int*) _temp353) ==  Cyc_Absyn_Tuple_e){
goto _LL410;} else{ goto _LL411;} _LL411: if(*(( int*) _temp353) ==  Cyc_Absyn_CompoundLit_e){
goto _LL412;} else{ goto _LL413;} _LL413: if(*(( int*) _temp353) ==  Cyc_Absyn_Array_e){
goto _LL414;} else{ goto _LL415;} _LL415: if(*(( int*) _temp353) ==  Cyc_Absyn_Comprehension_e){
goto _LL416;} else{ goto _LL417;} _LL417: if(*(( int*) _temp353) ==  Cyc_Absyn_Struct_e){
goto _LL418;} else{ goto _LL419;} _LL419: if(*(( int*) _temp353) ==  Cyc_Absyn_AnonStruct_e){
goto _LL420;} else{ goto _LL421;} _LL421: if(*(( int*) _temp353) ==  Cyc_Absyn_Tunion_e){
goto _LL422;} else{ goto _LL423;} _LL423: if(*(( int*) _temp353) ==  Cyc_Absyn_Enum_e){
goto _LL424;} else{ goto _LL425;} _LL425: if(*(( int*) _temp353) ==  Cyc_Absyn_AnonEnum_e){
goto _LL426;} else{ goto _LL427;} _LL427: if(*(( int*) _temp353) ==  Cyc_Absyn_Codegen_e){
goto _LL428;} else{ goto _LL429;} _LL429: if(*(( int*) _temp353) ==  Cyc_Absyn_Fill_e){
goto _LL430;} else{ goto _LL354;} _LL356: _temp439= _temp434; goto _LL358;
_LL358: Cyc_add_target( _temp439); return; _LL360: for( 0; _temp441 !=  0;
_temp441= _temp441->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_temp441->hd));} return; _LL362: _temp449= _temp445; _temp447= _temp443; goto
_LL364; _LL364: _temp453= _temp449; _temp451= _temp447; goto _LL366; _LL366: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp453); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp451);
return; _LL368: _temp457= _temp455; goto _LL370; _LL370: _temp459= _temp457;
goto _LL372; _LL372: _temp461= _temp459; goto _LL374; _LL374: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp461); return; _LL376: Cyc_scan_exp(( struct Cyc_Absyn_Exp*)
_temp467); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp465); Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp463); return; _LL378: _temp475= _temp471; _temp473=
_temp469; goto _LL380; _LL380: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp475);
for( 0; _temp473 !=  0; _temp473= _temp473->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _temp473->hd));} return; _LL382: Cyc_scan_type( _temp479);
Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp477); return; _LL384: _temp483=
_temp481; goto _LL386; _LL386: Cyc_scan_type( _temp483); return; _LL388:
_temp491= _temp487; _temp489= _temp485; goto _LL390; _LL390: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp491); Cyc_add_target( _temp489); return; _LL392: Cyc_scan_type(
_temp495);{ void* _temp499= _temp493; struct _tagged_arr* _temp505; _LL501: if(*((
int*) _temp499) ==  Cyc_Absyn_StructField){ _LL506: _temp505=(( struct Cyc_Absyn_StructField_struct*)
_temp499)->f1; goto _LL502;} else{ goto _LL503;} _LL503: if(*(( int*) _temp499)
==  Cyc_Absyn_TupleIndex){ goto _LL504;} else{ goto _LL500;} _LL502: Cyc_add_target(
_temp505); goto _LL500; _LL504: goto _LL500; _LL500:;} return; _LL394: return;
_LL396: for( 0; _temp497 !=  0; _temp497= _temp497->tl){ struct _tuple12
_temp509; struct Cyc_Absyn_Exp* _temp510; struct _tuple12* _temp507=( struct
_tuple12*) _temp497->hd; _temp509=* _temp507; _LL511: _temp510= _temp509.f2;
goto _LL508; _LL508: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp510);} return;
_LL398:({ void* _temp512[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Stmt_e\n",
sizeof( unsigned char), 26u), _tag_arr( _temp512, sizeof( void*), 0u));}); exit(
1); return; _LL400:({ void* _temp513[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Throw_e\n", sizeof( unsigned char), 27u), _tag_arr(
_temp513, sizeof( void*), 0u));}); exit( 1); return; _LL402:({ void* _temp514[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected NoInstantiate_e\n",
sizeof( unsigned char), 35u), _tag_arr( _temp514, sizeof( void*), 0u));}); exit(
1); return; _LL404:({ void* _temp515[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Instantiate_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp515, sizeof( void*), 0u));}); exit( 1); return; _LL406:({ void*
_temp516[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected New_e\n",
sizeof( unsigned char), 25u), _tag_arr( _temp516, sizeof( void*), 0u));}); exit(
1); return; _LL408:({ void* _temp517[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Gentyp_e\n", sizeof( unsigned char), 28u), _tag_arr(
_temp517, sizeof( void*), 0u));}); exit( 1); return; _LL410:({ void* _temp518[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tuple_e\n", sizeof(
unsigned char), 27u), _tag_arr( _temp518, sizeof( void*), 0u));}); exit( 1);
return; _LL412:({ void* _temp519[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected CompoundLit_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp519, sizeof( void*), 0u));}); exit( 1); return; _LL414:({ void*
_temp520[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Array_e\n",
sizeof( unsigned char), 27u), _tag_arr( _temp520, sizeof( void*), 0u));}); exit(
1); return; _LL416:({ void* _temp521[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Comprehension_e\n", sizeof( unsigned char), 35u),
_tag_arr( _temp521, sizeof( void*), 0u));}); exit( 1); return; _LL418:({ void*
_temp522[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Struct_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp522, sizeof( void*), 0u));}); exit(
1); return; _LL420:({ void* _temp523[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonStruct_e\n", sizeof( unsigned char), 32u),
_tag_arr( _temp523, sizeof( void*), 0u));}); exit( 1); return; _LL422:({ void*
_temp524[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tunion_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp524, sizeof( void*), 0u));}); exit(
1); return; _LL424:({ void* _temp525[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Enum_e\n", sizeof( unsigned char), 26u), _tag_arr(
_temp525, sizeof( void*), 0u));}); exit( 1); return; _LL426:({ void* _temp526[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AnonEnum_e\n",
sizeof( unsigned char), 30u), _tag_arr( _temp526, sizeof( void*), 0u));}); exit(
1); return; _LL428:({ void* _temp527[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Codegen_e\n", sizeof( unsigned char), 29u), _tag_arr(
_temp527, sizeof( void*), 0u));}); exit( 1); return; _LL430:({ void* _temp528[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Fill_e\n", sizeof(
unsigned char), 26u), _tag_arr( _temp528, sizeof( void*), 0u));}); exit( 1);
return; _LL354:;} void Cyc_scan_exp_opt( struct Cyc_Absyn_Exp* eo){ if((
unsigned int) eo){ Cyc_scan_exp( eo);} return;} void Cyc_scan_type( void* t){
void* _temp529= t; struct Cyc_Absyn_PtrInfo _temp585; struct Cyc_Absyn_Exp*
_temp587; void* _temp589; struct Cyc_Absyn_FnInfo _temp591; struct Cyc_List_List*
_temp593; struct Cyc_List_List* _temp595; struct _tuple0* _temp597; struct
_tuple0* _temp599; struct _tuple0* _temp601; struct _tuple0 _temp603; struct
_tagged_arr* _temp604; struct _tuple0* _temp606; struct _tuple0 _temp608; struct
_tagged_arr* _temp609; struct _tuple0* _temp611; struct _tuple0 _temp613; struct
_tagged_arr* _temp614; struct _tuple0* _temp616; struct _tuple0 _temp618; struct
_tagged_arr* _temp619; _LL531: if( _temp529 == ( void*) Cyc_Absyn_VoidType){
goto _LL532;} else{ goto _LL533;} _LL533: if(( unsigned int) _temp529 >  3u?*((
int*) _temp529) ==  Cyc_Absyn_IntType: 0){ goto _LL534;} else{ goto _LL535;}
_LL535: if( _temp529 == ( void*) Cyc_Absyn_FloatType){ goto _LL536;} else{ goto
_LL537;} _LL537: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_DoubleType:
0){ goto _LL538;} else{ goto _LL539;} _LL539: if(( unsigned int) _temp529 >  3u?*((
int*) _temp529) ==  Cyc_Absyn_PointerType: 0){ _LL586: _temp585=(( struct Cyc_Absyn_PointerType_struct*)
_temp529)->f1; goto _LL540;} else{ goto _LL541;} _LL541: if(( unsigned int)
_temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_ArrayType: 0){ _LL590: _temp589=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp529)->f1; goto _LL588; _LL588:
_temp587=(( struct Cyc_Absyn_ArrayType_struct*) _temp529)->f3; goto _LL542;}
else{ goto _LL543;} _LL543: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529)
==  Cyc_Absyn_FnType: 0){ _LL592: _temp591=(( struct Cyc_Absyn_FnType_struct*)
_temp529)->f1; goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int)
_temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_AnonStructType: 0){ _LL594:
_temp593=(( struct Cyc_Absyn_AnonStructType_struct*) _temp529)->f1; goto _LL546;}
else{ goto _LL547;} _LL547: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529)
==  Cyc_Absyn_AnonUnionType: 0){ _LL596: _temp595=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp529)->f1; goto _LL548;} else{ goto _LL549;} _LL549: if(( unsigned int)
_temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL550;}
else{ goto _LL551;} _LL551: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529)
==  Cyc_Absyn_StructType: 0){ _LL598: _temp597=(( struct Cyc_Absyn_StructType_struct*)
_temp529)->f1; if( _temp597 ==  0){ goto _LL552;} else{ goto _LL553;}} else{
goto _LL553;} _LL553: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) == 
Cyc_Absyn_UnionType: 0){ _LL600: _temp599=(( struct Cyc_Absyn_UnionType_struct*)
_temp529)->f1; if( _temp599 ==  0){ goto _LL554;} else{ goto _LL555;}} else{
goto _LL555;} _LL555: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) == 
Cyc_Absyn_StructType: 0){ _LL602: _temp601=(( struct Cyc_Absyn_StructType_struct*)
_temp529)->f1; if( _temp601 ==  0){ goto _LL557;} else{ _temp603=* _temp601;
_LL605: _temp604= _temp603.f2; goto _LL556;}} else{ goto _LL557;} _LL557: if((
unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_UnionType: 0){
_LL607: _temp606=(( struct Cyc_Absyn_UnionType_struct*) _temp529)->f1; if(
_temp606 ==  0){ goto _LL559;} else{ _temp608=* _temp606; _LL610: _temp609=
_temp608.f2; goto _LL558;}} else{ goto _LL559;} _LL559: if(( unsigned int)
_temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_EnumType: 0){ _LL612: _temp611=((
struct Cyc_Absyn_EnumType_struct*) _temp529)->f1; _temp613=* _temp611; _LL615:
_temp614= _temp613.f2; goto _LL560;} else{ goto _LL561;} _LL561: if((
unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_TypedefType: 0){
_LL617: _temp616=(( struct Cyc_Absyn_TypedefType_struct*) _temp529)->f1;
_temp618=* _temp616; _LL620: _temp619= _temp618.f2; goto _LL562;} else{ goto
_LL563;} _LL563: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_Evar:
0){ goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int) _temp529 >  3u?*((
int*) _temp529) ==  Cyc_Absyn_VarType: 0){ goto _LL566;} else{ goto _LL567;}
_LL567: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_TunionType:
0){ goto _LL568;} else{ goto _LL569;} _LL569: if(( unsigned int) _temp529 >  3u?*((
int*) _temp529) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL570;} else{ goto
_LL571;} _LL571: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_TupleType:
0){ goto _LL572;} else{ goto _LL573;} _LL573: if(( unsigned int) _temp529 >  3u?*((
int*) _temp529) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL574;} else{ goto _LL575;}
_LL575: if( _temp529 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL576;} else{ goto
_LL577;} _LL577: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_AccessEff:
0){ goto _LL578;} else{ goto _LL579;} _LL579: if(( unsigned int) _temp529 >  3u?*((
int*) _temp529) ==  Cyc_Absyn_JoinEff: 0){ goto _LL580;} else{ goto _LL581;}
_LL581: if(( unsigned int) _temp529 >  3u?*(( int*) _temp529) ==  Cyc_Absyn_RgnsEff:
0){ goto _LL582;} else{ goto _LL583;} _LL583: if(( unsigned int) _temp529 >  3u?*((
int*) _temp529) ==  Cyc_Absyn_SizeofType: 0){ goto _LL584;} else{ goto _LL530;}
_LL532: goto _LL534; _LL534: goto _LL536; _LL536: goto _LL538; _LL538: return;
_LL540: Cyc_scan_type(( void*) _temp585.elt_typ); return; _LL542: Cyc_scan_type(
_temp589); Cyc_scan_exp_opt( _temp587); return; _LL544: Cyc_scan_type(( void*)
_temp591.ret_typ);{ struct Cyc_List_List* _temp621= _temp591.args; for( 0;
_temp621 !=  0; _temp621= _temp621->tl){ struct _tuple1 _temp624; void* _temp625;
struct _tuple1* _temp622=( struct _tuple1*) _temp621->hd; _temp624=* _temp622;
_LL626: _temp625= _temp624.f3; goto _LL623; _LL623: Cyc_scan_type( _temp625);}}
if( _temp591.cyc_varargs !=  0){ Cyc_scan_type(( void*)( _temp591.cyc_varargs)->type);}
return; _LL546: _temp595= _temp593; goto _LL548; _LL548: for( 0; _temp595 !=  0;
_temp595= _temp595->tl){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_Structfield*)
_temp595->hd)->type); Cyc_scan_exp_opt((( struct Cyc_Absyn_Structfield*)
_temp595->hd)->width);} return; _LL550: return; _LL552: return; _LL554: return;
_LL556: _temp609= _temp604; goto _LL558; _LL558: _temp614= _temp609; goto _LL560;
_LL560: _temp619= _temp614; goto _LL562; _LL562: Cyc_add_target( _temp619);
return; _LL564:({ void* _temp627[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Evar\n", sizeof( unsigned char), 24u), _tag_arr(
_temp627, sizeof( void*), 0u));}); exit( 1); return; _LL566:({ void* _temp628[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected VarType\n", sizeof(
unsigned char), 27u), _tag_arr( _temp628, sizeof( void*), 0u));}); exit( 1);
return; _LL568:({ void* _temp629[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionType\n", sizeof( unsigned char), 30u),
_tag_arr( _temp629, sizeof( void*), 0u));}); exit( 1); return; _LL570:({ void*
_temp630[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected TunionFieldType\n",
sizeof( unsigned char), 35u), _tag_arr( _temp630, sizeof( void*), 0u));}); exit(
1); return; _LL572:({ void* _temp631[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n", sizeof( unsigned char), 29u), _tag_arr(
_temp631, sizeof( void*), 0u));}); exit( 1); return; _LL574:({ void* _temp632[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnHandleType\n",
sizeof( unsigned char), 33u), _tag_arr( _temp632, sizeof( void*), 0u));}); exit(
1); return; _LL576:({ void* _temp633[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected HeapRgn\n", sizeof( unsigned char), 27u), _tag_arr(
_temp633, sizeof( void*), 0u));}); exit( 1); return; _LL578:({ void* _temp634[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AccessEff\n",
sizeof( unsigned char), 29u), _tag_arr( _temp634, sizeof( void*), 0u));}); exit(
1); return; _LL580:({ void* _temp635[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected JoinEff\n", sizeof( unsigned char), 27u), _tag_arr(
_temp635, sizeof( void*), 0u));}); exit( 1); return; _LL582:({ void* _temp636[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnsEff\n", sizeof(
unsigned char), 27u), _tag_arr( _temp636, sizeof( void*), 0u));}); exit( 1);
return; _LL584:({ void* _temp637[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected sizeof_t\n", sizeof( unsigned char), 28u), _tag_arr(
_temp637, sizeof( void*), 0u));}); exit( 1); return; _LL530:;} struct _tuple13{
struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct _tuple8*
Cyc_scan_decl( struct Cyc_Absyn_Decl* d){ Cyc_current_targets=({ struct Cyc_Set_Set**
_temp638=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp638[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); _temp638;});{ void* _temp639=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp665; struct Cyc_Absyn_Fndecl*
_temp667; struct Cyc_Absyn_Structdecl* _temp669; struct Cyc_Absyn_Uniondecl*
_temp671; struct Cyc_Absyn_Enumdecl* _temp673; struct Cyc_Absyn_Typedefdecl*
_temp675; _LL641: if(*(( int*) _temp639) ==  Cyc_Absyn_Var_d){ _LL666: _temp665=((
struct Cyc_Absyn_Var_d_struct*) _temp639)->f1; goto _LL642;} else{ goto _LL643;}
_LL643: if(*(( int*) _temp639) ==  Cyc_Absyn_Fn_d){ _LL668: _temp667=(( struct
Cyc_Absyn_Fn_d_struct*) _temp639)->f1; goto _LL644;} else{ goto _LL645;} _LL645:
if(*(( int*) _temp639) ==  Cyc_Absyn_Struct_d){ _LL670: _temp669=(( struct Cyc_Absyn_Struct_d_struct*)
_temp639)->f1; goto _LL646;} else{ goto _LL647;} _LL647: if(*(( int*) _temp639)
==  Cyc_Absyn_Union_d){ _LL672: _temp671=(( struct Cyc_Absyn_Union_d_struct*)
_temp639)->f1; goto _LL648;} else{ goto _LL649;} _LL649: if(*(( int*) _temp639)
==  Cyc_Absyn_Enum_d){ _LL674: _temp673=(( struct Cyc_Absyn_Enum_d_struct*)
_temp639)->f1; goto _LL650;} else{ goto _LL651;} _LL651: if(*(( int*) _temp639)
==  Cyc_Absyn_Typedef_d){ _LL676: _temp675=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp639)->f1; goto _LL652;} else{ goto _LL653;} _LL653: if(*(( int*) _temp639)
==  Cyc_Absyn_Let_d){ goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*)
_temp639) ==  Cyc_Absyn_Tunion_d){ goto _LL656;} else{ goto _LL657;} _LL657: if(*((
int*) _temp639) ==  Cyc_Absyn_Letv_d){ goto _LL658;} else{ goto _LL659;} _LL659:
if(*(( int*) _temp639) ==  Cyc_Absyn_Namespace_d){ goto _LL660;} else{ goto
_LL661;} _LL661: if(*(( int*) _temp639) ==  Cyc_Absyn_Using_d){ goto _LL662;}
else{ goto _LL663;} _LL663: if(*(( int*) _temp639) ==  Cyc_Absyn_ExternC_d){
goto _LL664;} else{ goto _LL640;} _LL642: { struct _tuple0 _temp679; struct
_tagged_arr* _temp680; struct _tuple0* _temp677= _temp665->name; _temp679=*
_temp677; _LL681: _temp680= _temp679.f2; goto _LL678; _LL678: Cyc_current_source=(
struct _tagged_arr*) _temp680; Cyc_scan_type(( void*) _temp665->type); Cyc_scan_exp_opt(
_temp665->initializer); goto _LL640;} _LL644: { struct _tuple0 _temp684; struct
_tagged_arr* _temp685; struct _tuple0* _temp682= _temp667->name; _temp684=*
_temp682; _LL686: _temp685= _temp684.f2; goto _LL683; _LL683: Cyc_current_source=(
struct _tagged_arr*) _temp685; Cyc_scan_type(( void*) _temp667->ret_type);{
struct Cyc_List_List* _temp687= _temp667->args; for( 0; _temp687 !=  0; _temp687=
_temp687->tl){ struct _tuple13 _temp690; void* _temp691; struct _tuple13*
_temp688=( struct _tuple13*) _temp687->hd; _temp690=* _temp688; _LL692: _temp691=
_temp690.f3; goto _LL689; _LL689: Cyc_scan_type( _temp691);}} if( _temp667->cyc_varargs
!=  0){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp667->cyc_varargs))->type);} if( _temp667->is_inline){({ void* _temp693[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: ignoring inline function\n",
sizeof( unsigned char), 35u), _tag_arr( _temp693, sizeof( void*), 0u));});} goto
_LL640;} _LL646: if( !(( unsigned int) _temp669->name)){ return 0;}{ struct
_tuple0 _temp696; struct _tagged_arr* _temp697; struct _tuple0* _temp694=(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp669->name))->v;
_temp696=* _temp694; _LL698: _temp697= _temp696.f2; goto _LL695; _LL695: Cyc_current_source=(
struct _tagged_arr*) _temp697; if(( unsigned int) _temp669->fields){{ struct Cyc_List_List*
_temp699=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp669->fields))->v;
for( 0; _temp699 !=  0; _temp699= _temp699->tl){ struct Cyc_Absyn_Structfield*
_temp700=( struct Cyc_Absyn_Structfield*) _temp699->hd; Cyc_scan_type(( void*)
_temp700->type); Cyc_scan_exp_opt( _temp700->width);}}{ struct Cyc_List_List*
_temp701=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp669->fields))->v;
for( 0; _temp701 !=  0; _temp701= _temp701->tl){;}}} goto _LL640;} _LL648: if( !((
unsigned int) _temp671->name)){ return 0;}{ struct _tuple0 _temp704; struct
_tagged_arr* _temp705; struct _tuple0* _temp702=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp671->name))->v; _temp704=* _temp702; _LL706: _temp705=
_temp704.f2; goto _LL703; _LL703: Cyc_current_source=( struct _tagged_arr*)
_temp705; if(( unsigned int) _temp671->fields){{ struct Cyc_List_List* _temp707=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp671->fields))->v;
for( 0; _temp707 !=  0; _temp707= _temp707->tl){ struct Cyc_Absyn_Structfield*
_temp708=( struct Cyc_Absyn_Structfield*) _temp707->hd; Cyc_add_target( _temp708->name);
Cyc_scan_type(( void*) _temp708->type); Cyc_scan_exp_opt( _temp708->width);}}{
struct Cyc_List_List* _temp709=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp671->fields))->v; for( 0; _temp709 !=  0; _temp709= _temp709->tl){;}}}
goto _LL640;} _LL650: { struct _tuple0 _temp712; struct _tagged_arr* _temp713;
struct _tuple0* _temp710= _temp673->name; _temp712=* _temp710; _LL714: _temp713=
_temp712.f2; goto _LL711; _LL711: Cyc_current_source=( struct _tagged_arr*)
_temp713; if(( unsigned int) _temp673->fields){{ struct Cyc_List_List* _temp715=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp673->fields))->v;
for( 0; _temp715 !=  0; _temp715= _temp715->tl){ struct Cyc_Absyn_Enumfield*
_temp716=( struct Cyc_Absyn_Enumfield*) _temp715->hd; Cyc_scan_exp_opt( _temp716->tag);}}{
struct Cyc_List_List* _temp717=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp673->fields))->v; for( 0; _temp717 !=  0; _temp717= _temp717->tl){;}}}
goto _LL640;} _LL652: { struct _tuple0 _temp720; struct _tagged_arr* _temp721;
struct _tuple0* _temp718= _temp675->name; _temp720=* _temp718; _LL722: _temp721=
_temp720.f2; goto _LL719; _LL719: Cyc_current_source=( struct _tagged_arr*)
_temp721; Cyc_scan_type(( void*) _temp675->defn); goto _LL640;} _LL654:({ void*
_temp723[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp723, sizeof( void*), 0u));}); exit(
1); return 0; _LL656:({ void* _temp724[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected tunion declaration\n", sizeof( unsigned char), 38u),
_tag_arr( _temp724, sizeof( void*), 0u));}); exit( 1); return 0; _LL658:({ void*
_temp725[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp725, sizeof( void*), 0u));}); exit(
1); return 0; _LL660:({ void* _temp726[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected namespace declaration\n", sizeof( unsigned char), 41u),
_tag_arr( _temp726, sizeof( void*), 0u));}); exit( 1); return 0; _LL662:({ void*
_temp727[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected using declaration\n",
sizeof( unsigned char), 37u), _tag_arr( _temp727, sizeof( void*), 0u));}); exit(
1); return 0; _LL664:({ void* _temp728[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n", sizeof( unsigned char),
42u), _tag_arr( _temp728, sizeof( void*), 0u));}); exit( 1); return 0; _LL640:;}
return({ struct _tuple8* _temp729=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp729->f1=( struct _tagged_arr*) _check_null( Cyc_current_source);
_temp729->f2=*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets));
_temp729;});} struct Cyc_Hashtable_Table* Cyc_new_deps(){ return(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), int(* hash)(
struct _tagged_arr*))) Cyc_Hashtable_create)( 107, Cyc_Std_strptrcmp, Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set* Cyc_find( struct Cyc_Hashtable_Table* t, struct _tagged_arr*
x){ struct _handler_cons _temp730; _push_handler(& _temp730);{ int _temp732= 0;
if( setjmp( _temp730.handler)){ _temp732= 1;} if( ! _temp732){{ struct Cyc_Set_Set*
_temp733=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key)) Cyc_Hashtable_lookup)( t, x); _npop_handler( 0u); return
_temp733;}; _pop_handler();} else{ void* _temp731=( void*) _exn_thrown; void*
_temp735= _temp731; _LL737: if( _temp735 ==  Cyc_Core_Not_found){ goto _LL738;}
else{ goto _LL739;} _LL739: goto _LL740; _LL738: return(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_LL740:( void) _throw( _temp735); _LL736:;}}} struct Cyc_Set_Set* Cyc_reachable(
struct Cyc_List_List* init, struct Cyc_Hashtable_Table* t){ struct Cyc_Set_Set*
emptyset=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); struct Cyc_Set_Set* curr;
for( curr= emptyset; init !=  0; init= init->tl){ curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( curr,( struct
_tagged_arr*) init->hd);}{ struct Cyc_Set_Set* _temp741= curr; while((( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp741) >  0) { struct Cyc_Set_Set*
_temp742= emptyset;{ struct Cyc_List_List* _temp743=(( struct Cyc_List_List*(*)(
struct Cyc_Set_Set* s)) Cyc_Set_elements)( _temp741); for( 0; _temp743 !=  0;
_temp743= _temp743->tl){ _temp742=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( _temp742, Cyc_find( t,( struct
_tagged_arr*) _temp743->hd));}} _temp741=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_diff)( _temp742, curr); curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( curr,
_temp741);} return curr;}} int Cyc_force_directory( struct _tagged_arr d){ int
_temp744=({ int _temp747[ 0u]; Cyc_Std_open( d, 0, _tag_arr( _temp747, sizeof(
int), 0u));}); if( _temp744 ==  - 1){ if( Cyc_Std_mkdir( d, 448) ==  - 1){({
struct Cyc_Std_String_pa_struct _temp746; _temp746.tag= Cyc_Std_String_pa;
_temp746.f1=( struct _tagged_arr) d;{ void* _temp745[ 1u]={& _temp746}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n", sizeof(
unsigned char), 38u), _tag_arr( _temp745, sizeof( void*), 1u));}}); return 1;}}
else{ close( _temp744);} return 0;} int Cyc_force_directory_prefixes( struct
_tagged_arr file){ struct _tagged_arr _temp748= Cyc_Std_strdup( file); struct
Cyc_List_List* _temp749= 0; while( 1) { _temp748= Cyc_Filename_dirname(( struct
_tagged_arr) _temp748); if( _get_arr_size( _temp748, sizeof( unsigned char)) == 
0){ break;} _temp749=({ struct Cyc_List_List* _temp750=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp750->hd=( void*)({ struct
_tagged_arr* _temp751=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp751[ 0]=( struct _tagged_arr) _temp748; _temp751;});
_temp750->tl= _temp749; _temp750;});} for( 0; _temp749 !=  0; _temp749= _temp749->tl){
if( Cyc_force_directory(*(( struct _tagged_arr*) _temp749->hd))){ return 1;}}
return 0;} int Cyc_process_file( struct _tagged_arr filename, struct _tagged_arr
dir, struct Cyc_List_List* start_symbols, struct Cyc_List_List* omit_symbols,
struct Cyc_List_List* prologue, struct Cyc_List_List* epilogue, struct Cyc_List_List*
cstubs, struct Cyc_List_List* cycstubs){ struct Cyc_Std___sFILE* maybe; struct
Cyc_Std___sFILE* in_file; struct Cyc_Std___sFILE* out_file;({ struct Cyc_Std_String_pa_struct
_temp753; _temp753.tag= Cyc_Std_String_pa; _temp753.f1=( struct _tagged_arr)
filename;{ void* _temp752[ 1u]={& _temp753}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("********************************* %s...\n", sizeof( unsigned char), 41u),
_tag_arr( _temp752, sizeof( void*), 1u));}});{ struct _tagged_arr buf= _tag_arr(({
unsigned int _temp1055=( unsigned int) 1024; unsigned char* _temp1056=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char),
_temp1055));{ unsigned int _temp1057= _temp1055; unsigned int i; for( i= 0; i < 
_temp1057; i ++){ _temp1056[ i]='\000';}}; _temp1056;}), sizeof( unsigned char),(
unsigned int) 1024); struct _tagged_arr _temp754= Cyc_Std_getcwd( buf,
_get_arr_size( buf, sizeof( unsigned char))); if( Cyc_Std_chdir( dir)){({ struct
Cyc_Std_String_pa_struct _temp756; _temp756.tag= Cyc_Std_String_pa; _temp756.f1=(
struct _tagged_arr) dir;{ void* _temp755[ 1u]={& _temp756}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: can't change directory to %s\n", sizeof( unsigned char), 37u),
_tag_arr( _temp755, sizeof( void*), 1u));}}); return 1;}({ struct Cyc_Std_String_pa_struct
_temp758; _temp758.tag= Cyc_Std_String_pa; _temp758.f1=( struct _tagged_arr)
filename;{ void* _temp757[ 1u]={& _temp758}; Cyc_log( _tag_arr("\n%s:\n",
sizeof( unsigned char), 6u), _tag_arr( _temp757, sizeof( void*), 1u));}});{
struct _tagged_arr _temp759= Cyc_Filename_basename( filename); struct
_tagged_arr _temp760= Cyc_Filename_chop_extension(( struct _tagged_arr) _temp759);
struct _tagged_arr _temp761= Cyc_Std_strconcat(( struct _tagged_arr) _temp760,
_tag_arr(".i", sizeof( unsigned char), 3u)); struct _tagged_arr _temp762= Cyc_Std_strconcat((
struct _tagged_arr) _temp760, _tag_arr(".iX", sizeof( unsigned char), 4u));
struct _tagged_arr _temp763= Cyc_Filename_dirname( filename); struct _tagged_arr
_temp764= _get_arr_size( _temp763, sizeof( unsigned char)) ==  0?({ struct Cyc_Std_String_pa_struct
_temp1052; _temp1052.tag= Cyc_Std_String_pa; _temp1052.f1=( struct _tagged_arr)
_temp759;{ void* _temp1051[ 1u]={& _temp1052}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp1051, sizeof( void*), 1u));}}): Cyc_Filename_concat((
struct _tagged_arr) _temp763,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1054; _temp1054.tag= Cyc_Std_String_pa; _temp1054.f1=( struct _tagged_arr)
_temp759;{ void* _temp1053[ 1u]={& _temp1054}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp1053, sizeof( void*), 1u));}})); if(
Cyc_force_directory_prefixes( filename)){ return 1;}{ struct Cyc_Hashtable_Table*
t= Cyc_new_deps(); struct _tagged_arr _temp765=({ struct Cyc_Std_String_pa_struct
_temp1050; _temp1050.tag= Cyc_Std_String_pa; _temp1050.f1=( struct _tagged_arr)
_temp761;{ struct Cyc_Std_String_pa_struct _temp1049; _temp1049.tag= Cyc_Std_String_pa;
_temp1049.f1=( struct _tagged_arr) filename;{ void* _temp1048[ 2u]={& _temp1049,&
_temp1050}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp1048, sizeof( void*), 2u));}}});
if( Cyc_Std_system(( struct _tagged_arr) _temp765) !=  0){ maybe= Cyc_Std_fopen(
filename, _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int)
maybe)){({ struct Cyc_Std_String_pa_struct _temp767; _temp767.tag= Cyc_Std_String_pa;
_temp767.f1=( struct _tagged_arr) filename;{ void* _temp766[ 1u]={& _temp767};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create file %s\n",
sizeof( unsigned char), 33u), _tag_arr( _temp766, sizeof( void*), 1u));}});
return 1;} out_file=( struct Cyc_Std___sFILE*) _check_null( maybe);({ struct Cyc_Std_String_pa_struct
_temp769; _temp769.tag= Cyc_Std_String_pa; _temp769.f1=( struct _tagged_arr)
filename;{ void* _temp768[ 1u]={& _temp769}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp768, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp764, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){({ struct Cyc_Std_String_pa_struct
_temp771; _temp771.tag= Cyc_Std_String_pa; _temp771.f1=( struct _tagged_arr)
_temp764;{ void* _temp770[ 1u]={& _temp771}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n", sizeof( unsigned char), 33u),
_tag_arr( _temp770, sizeof( void*), 1u));}}); return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp773; _temp773.tag=
Cyc_Std_String_pa; _temp773.f1=( struct _tagged_arr) _temp764;{ void* _temp772[
1u]={& _temp773}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp772, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file);({ struct Cyc_Std_String_pa_struct _temp775; _temp775.tag= Cyc_Std_String_pa;
_temp775.f1=( struct _tagged_arr) filename;{ void* _temp774[ 1u]={& _temp775};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: %s will not be supported on this platform\n",
sizeof( unsigned char), 52u), _tag_arr( _temp774, sizeof( void*), 1u));}});({
void* _temp776[ 0u]={}; Cyc_log( _tag_arr("Not supported on this platform\n",
sizeof( unsigned char), 32u), _tag_arr( _temp776, sizeof( void*), 0u));}); if(
Cyc_Std_chdir(( struct _tagged_arr) _temp754)){({ struct Cyc_Std_String_pa_struct
_temp778; _temp778.tag= Cyc_Std_String_pa; _temp778.f1=( struct _tagged_arr)
_temp754;{ void* _temp777[ 1u]={& _temp778}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n", sizeof( unsigned char), 41u),
_tag_arr( _temp777, sizeof( void*), 1u));}}); return 1;} return 0;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp761, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe);{ struct Cyc_Lexing_lexbuf* _temp779= Cyc_Lexing_from_file( in_file);
struct _tuple8* entry; while(( entry=(( struct _tuple8*(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_line)( _temp779)) !=  0) { struct _tuple8 _temp782; struct Cyc_Set_Set*
_temp783; struct _tagged_arr* _temp785; struct _tuple8* _temp780=( struct
_tuple8*) _check_null( entry); _temp782=* _temp780; _LL786: _temp785= _temp782.f1;
goto _LL784; _LL784: _temp783= _temp782.f2; goto _LL781; _LL781:(( void(*)(
struct Cyc_Hashtable_Table* t, struct _tagged_arr* key, struct Cyc_Set_Set* val))
Cyc_Hashtable_insert)( t, _temp785, _temp783);} Cyc_Std_fclose( in_file);
_temp765=({ struct Cyc_Std_String_pa_struct _temp789; _temp789.tag= Cyc_Std_String_pa;
_temp789.f1=( struct _tagged_arr) _temp761;{ struct Cyc_Std_String_pa_struct
_temp788; _temp788.tag= Cyc_Std_String_pa; _temp788.f1=( struct _tagged_arr)
filename;{ void* _temp787[ 2u]={& _temp788,& _temp789}; Cyc_Std_aprintf(
_tag_arr("echo '#include <%s>' | gcc -E -o %s -", sizeof( unsigned char), 38u),
_tag_arr( _temp787, sizeof( void*), 2u));}}}); if( Cyc_Std_system(( struct
_tagged_arr) _temp765) !=  0){ return 1;} maybe= Cyc_Std_fopen(( struct
_tagged_arr) _temp761, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp779= Cyc_Lexing_from_file( in_file); Cyc_slurp_out= Cyc_Std_fopen((
struct _tagged_arr) _temp762, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) Cyc_slurp_out)){ return 1;} if((( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_slurp)( _temp779)){({ struct Cyc_Std_String_pa_struct _temp791;
_temp791.tag= Cyc_Std_String_pa; _temp791.f1=( struct _tagged_arr) filename;{
void* _temp790[ 1u]={& _temp791}; Cyc_log( _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp790, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp793; _temp793.tag= Cyc_Std_String_pa;
_temp793.f1=( struct _tagged_arr) filename;{ void* _temp792[ 1u]={& _temp793};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp792, sizeof( void*), 1u));}}); Cyc_Std_fclose(
in_file); Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
Cyc_Std_remove(( struct _tagged_arr) _temp761); Cyc_Std_remove(( struct
_tagged_arr) _temp762); return 1;} Cyc_Std_fclose( in_file); Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp762, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); Cyc_Position_reset_position(( struct _tagged_arr) _temp762); Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp794= Cyc_Parse_parse_file( in_file); Cyc_Lex_lex_init();
Cyc_Std_fclose( in_file);{ struct Cyc_List_List* _temp795= _temp794; for( 0;
_temp795 !=  0; _temp795= _temp795->tl){ struct _tuple8* _temp796= Cyc_scan_decl((
struct Cyc_Absyn_Decl*) _temp795->hd); if( _temp796 ==  0){ continue;}{ struct
_tuple8 _temp799; struct Cyc_Set_Set* _temp800; struct _tagged_arr* _temp802;
struct _tuple8* _temp797=( struct _tuple8*) _check_null( _temp796); _temp799=*
_temp797; _LL803: _temp802= _temp799.f1; goto _LL801; _LL801: _temp800= _temp799.f2;
goto _LL798; _LL798: { struct Cyc_Set_Set* old;{ struct _handler_cons _temp804;
_push_handler(& _temp804);{ int _temp806= 0; if( setjmp( _temp804.handler)){
_temp806= 1;} if( ! _temp806){ old=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_arr* key)) Cyc_Hashtable_lookup)( t, _temp802);; _pop_handler();}
else{ void* _temp805=( void*) _exn_thrown; void* _temp808= _temp805; _LL810: if(
_temp808 ==  Cyc_Core_Not_found){ goto _LL811;} else{ goto _LL812;} _LL812: goto
_LL813; _LL811: old=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); goto _LL809; _LL813:(
void) _throw( _temp808); _LL809:;}}}(( void(*)( struct Cyc_Hashtable_Table* t,
struct _tagged_arr* key, struct Cyc_Set_Set* val)) Cyc_Hashtable_insert)( t,
_temp802,(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( _temp800, old));}}}}{ struct Cyc_Set_Set* _temp814= Cyc_reachable(
start_symbols, t); struct Cyc_List_List* _temp815= 0; struct Cyc_Set_Set*
defined_symbols=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);{ struct Cyc_List_List*
_temp816= _temp794; for( 0; _temp816 !=  0; _temp816= _temp816->tl){ struct Cyc_Absyn_Decl*
_temp817=( struct Cyc_Absyn_Decl*) _temp816->hd; struct _tagged_arr* name;{ void*
_temp818=( void*) _temp817->r; struct Cyc_Absyn_Vardecl* _temp844; struct Cyc_Absyn_Fndecl*
_temp846; struct Cyc_Absyn_Structdecl* _temp848; struct Cyc_Absyn_Uniondecl*
_temp850; struct Cyc_Absyn_Enumdecl* _temp852; struct Cyc_Absyn_Typedefdecl*
_temp854; _LL820: if(*(( int*) _temp818) ==  Cyc_Absyn_Var_d){ _LL845: _temp844=((
struct Cyc_Absyn_Var_d_struct*) _temp818)->f1; goto _LL821;} else{ goto _LL822;}
_LL822: if(*(( int*) _temp818) ==  Cyc_Absyn_Fn_d){ _LL847: _temp846=(( struct
Cyc_Absyn_Fn_d_struct*) _temp818)->f1; goto _LL823;} else{ goto _LL824;} _LL824:
if(*(( int*) _temp818) ==  Cyc_Absyn_Struct_d){ _LL849: _temp848=(( struct Cyc_Absyn_Struct_d_struct*)
_temp818)->f1; goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp818)
==  Cyc_Absyn_Union_d){ _LL851: _temp850=(( struct Cyc_Absyn_Union_d_struct*)
_temp818)->f1; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp818)
==  Cyc_Absyn_Enum_d){ _LL853: _temp852=(( struct Cyc_Absyn_Enum_d_struct*)
_temp818)->f1; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp818)
==  Cyc_Absyn_Typedef_d){ _LL855: _temp854=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp818)->f1; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp818)
==  Cyc_Absyn_Let_d){ goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*)
_temp818) ==  Cyc_Absyn_Tunion_d){ goto _LL835;} else{ goto _LL836;} _LL836: if(*((
int*) _temp818) ==  Cyc_Absyn_Letv_d){ goto _LL837;} else{ goto _LL838;} _LL838:
if(*(( int*) _temp818) ==  Cyc_Absyn_Namespace_d){ goto _LL839;} else{ goto
_LL840;} _LL840: if(*(( int*) _temp818) ==  Cyc_Absyn_Using_d){ goto _LL841;}
else{ goto _LL842;} _LL842: if(*(( int*) _temp818) ==  Cyc_Absyn_ExternC_d){
goto _LL843;} else{ goto _LL819;} _LL821: { struct _tuple0 _temp858; struct
_tagged_arr* _temp859; struct _tuple0* _temp856= _temp844->name; _temp858=*
_temp856; _LL860: _temp859= _temp858.f2; goto _LL857; _LL857: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp859); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp859)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp859;} goto _LL819;} _LL823: { struct _tuple0 _temp863; struct
_tagged_arr* _temp864; struct _tuple0* _temp861= _temp846->name; _temp863=*
_temp861; _LL865: _temp864= _temp863.f2; goto _LL862; _LL862: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp864); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp864)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp864;} goto _LL819;} _LL825: if( !(( unsigned int) _temp848->name)){
name= 0;}{ struct _tuple0 _temp868; struct _tagged_arr* _temp869; struct _tuple0*
_temp866=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp848->name))->v;
_temp868=* _temp866; _LL870: _temp869= _temp868.f2; goto _LL867; _LL867: name=(
struct _tagged_arr*) _temp869; goto _LL819;} _LL827: if( !(( unsigned int)
_temp850->name)){ name= 0;}{ struct _tuple0 _temp873; struct _tagged_arr*
_temp874; struct _tuple0* _temp871=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp850->name))->v; _temp873=* _temp871; _LL875: _temp874=
_temp873.f2; goto _LL872; _LL872: name=( struct _tagged_arr*) _temp874; goto
_LL819;} _LL829: { struct _tuple0 _temp878; struct _tagged_arr* _temp879; struct
_tuple0* _temp876= _temp852->name; _temp878=* _temp876; _LL880: _temp879=
_temp878.f2; goto _LL877; _LL877: name=( struct _tagged_arr*) _temp879; if( name
!=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp814,( struct _tagged_arr*) _check_null( name)): 0){ _temp815=({ struct Cyc_List_List*
_temp881=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp881->hd=( void*) _temp817; _temp881->tl= _temp815; _temp881;});} else{ if((
unsigned int) _temp852->fields){ struct Cyc_List_List* _temp882=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp852->fields))->v; for( 0; _temp882 !=  0;
_temp882= _temp882->tl){ struct Cyc_Absyn_Enumfield* _temp883=( struct Cyc_Absyn_Enumfield*)
_temp882->hd; struct _tuple0 _temp886; struct _tagged_arr* _temp887; struct
_tuple0* _temp884= _temp883->name; _temp886=* _temp884; _LL888: _temp887=
_temp886.f2; goto _LL885; _LL885: if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( _temp814, _temp887)){ _temp815=({ struct Cyc_List_List*
_temp889=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp889->hd=( void*) _temp817; _temp889->tl= _temp815; _temp889;}); break;}}}}
name= 0; goto _LL819;} _LL831: { struct _tuple0 _temp892; struct _tagged_arr*
_temp893; struct _tuple0* _temp890= _temp854->name; _temp892=* _temp890; _LL894:
_temp893= _temp892.f2; goto _LL891; _LL891: name=( struct _tagged_arr*) _temp893;
goto _LL819;} _LL833: goto _LL835; _LL835: goto _LL837; _LL837: goto _LL839;
_LL839: goto _LL841; _LL841: goto _LL843; _LL843: name= 0; goto _LL819; _LL819:;}
if( name !=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp814,( struct _tagged_arr*) _check_null( name)): 0){ _temp815=({ struct Cyc_List_List*
_temp895=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp895->hd=( void*) _temp817; _temp895->tl= _temp815; _temp895;});}}} _temp815=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp815);
maybe= Cyc_Std_fopen( filename, _tag_arr("w", sizeof( unsigned char), 2u)); if(
!(( unsigned int) maybe)){ return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp897; _temp897.tag=
Cyc_Std_String_pa; _temp897.f1=( struct _tagged_arr) _temp764;{ void* _temp896[
1u]={& _temp897}; Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\nusing Std;\n",
sizeof( unsigned char), 26u), _tag_arr( _temp896, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp764, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} out_file=(
struct Cyc_Std___sFILE*) _check_null( maybe);{ struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _temp1047; _temp1047.tag= Cyc_Std_String_pa;
_temp1047.f1=( struct _tagged_arr) filename;{ void* _temp1046[ 1u]={& _temp1047};
Cyc_Std_aprintf( _tag_arr("_%s_", sizeof( unsigned char), 5u), _tag_arr(
_temp1046, sizeof( void*), 1u));}});{ int _temp898= 0; for( 0; _temp898 < 
_get_arr_size( ifdefmacro, sizeof( unsigned char)); _temp898 ++){ if(*((
unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof( unsigned char),
_temp898)) == '.'? 1:*(( unsigned char*) _check_unknown_subscript( ifdefmacro,
sizeof( unsigned char), _temp898)) == '/'){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp898))='_';}
else{ if(*(( unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof(
unsigned char), _temp898)) != '_'?*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp898)) != '/': 0){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp898))=(
unsigned char) toupper(( int)*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp898)));}}}}({ struct Cyc_Std_String_pa_struct
_temp901; _temp901.tag= Cyc_Std_String_pa; _temp901.f1=( struct _tagged_arr)
ifdefmacro;{ struct Cyc_Std_String_pa_struct _temp900; _temp900.tag= Cyc_Std_String_pa;
_temp900.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp899[ 2u]={& _temp900,&
_temp901}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n#define %s\n",
sizeof( unsigned char), 23u), _tag_arr( _temp899, sizeof( void*), 2u));}}}); if(
prologue !=  0){ struct Cyc_List_List* _temp902= prologue; for( 0; _temp902 != 
0; _temp902= _temp902->tl){ struct _tuple10 _temp905; struct _tagged_arr
_temp906; struct _tagged_arr _temp908; struct _tuple10* _temp903=( struct
_tuple10*) _temp902->hd; _temp905=* _temp903; _LL909: _temp908= _temp905.f1;
goto _LL907; _LL907: _temp906= _temp905.f2; goto _LL904; _LL904: if( _temp906.curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp908.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp910=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp910[ 0]= _temp908; _temp910;})): 0){ Cyc_Std_fputs( _temp906,
out_file);} else{({ struct Cyc_Std_String_pa_struct _temp912; _temp912.tag= Cyc_Std_String_pa;
_temp912.f1=( struct _tagged_arr) _temp908;{ void* _temp911[ 1u]={& _temp912};
Cyc_log( _tag_arr("%s is not supported on this platform\n", sizeof(
unsigned char), 38u), _tag_arr( _temp911, sizeof( void*), 1u));}});}}}({ void*
_temp913[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("\nnamespace Std {\n",
sizeof( unsigned char), 18u), _tag_arr( _temp913, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp914= _temp815; for( 0; _temp914 !=  0; _temp914=
_temp914->tl){ struct Cyc_Absyn_Decl* _temp915=( struct Cyc_Absyn_Decl*)
_temp914->hd; int _temp916= 0; struct _tagged_arr* name;{ void* _temp917=( void*)
_temp915->r; struct Cyc_Absyn_Vardecl* _temp943; struct Cyc_Absyn_Fndecl*
_temp945; struct Cyc_Absyn_Structdecl* _temp947; struct Cyc_Absyn_Uniondecl*
_temp949; struct Cyc_Absyn_Enumdecl* _temp951; struct Cyc_Absyn_Typedefdecl*
_temp953; _LL919: if(*(( int*) _temp917) ==  Cyc_Absyn_Var_d){ _LL944: _temp943=((
struct Cyc_Absyn_Var_d_struct*) _temp917)->f1; goto _LL920;} else{ goto _LL921;}
_LL921: if(*(( int*) _temp917) ==  Cyc_Absyn_Fn_d){ _LL946: _temp945=(( struct
Cyc_Absyn_Fn_d_struct*) _temp917)->f1; goto _LL922;} else{ goto _LL923;} _LL923:
if(*(( int*) _temp917) ==  Cyc_Absyn_Struct_d){ _LL948: _temp947=(( struct Cyc_Absyn_Struct_d_struct*)
_temp917)->f1; goto _LL924;} else{ goto _LL925;} _LL925: if(*(( int*) _temp917)
==  Cyc_Absyn_Union_d){ _LL950: _temp949=(( struct Cyc_Absyn_Union_d_struct*)
_temp917)->f1; goto _LL926;} else{ goto _LL927;} _LL927: if(*(( int*) _temp917)
==  Cyc_Absyn_Enum_d){ _LL952: _temp951=(( struct Cyc_Absyn_Enum_d_struct*)
_temp917)->f1; goto _LL928;} else{ goto _LL929;} _LL929: if(*(( int*) _temp917)
==  Cyc_Absyn_Typedef_d){ _LL954: _temp953=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp917)->f1; goto _LL930;} else{ goto _LL931;} _LL931: if(*(( int*) _temp917)
==  Cyc_Absyn_Let_d){ goto _LL932;} else{ goto _LL933;} _LL933: if(*(( int*)
_temp917) ==  Cyc_Absyn_Tunion_d){ goto _LL934;} else{ goto _LL935;} _LL935: if(*((
int*) _temp917) ==  Cyc_Absyn_Letv_d){ goto _LL936;} else{ goto _LL937;} _LL937:
if(*(( int*) _temp917) ==  Cyc_Absyn_Namespace_d){ goto _LL938;} else{ goto
_LL939;} _LL939: if(*(( int*) _temp917) ==  Cyc_Absyn_Using_d){ goto _LL940;}
else{ goto _LL941;} _LL941: if(*(( int*) _temp917) ==  Cyc_Absyn_ExternC_d){
goto _LL942;} else{ goto _LL918;} _LL920: { struct _tuple0 _temp957; struct
_tagged_arr* _temp958; struct _tuple0* _temp955= _temp943->name; _temp957=*
_temp955; _LL959: _temp958= _temp957.f2; goto _LL956; _LL956: name=( struct
_tagged_arr*) _temp958; goto _LL918;} _LL922: { struct _tuple0 _temp962; struct
_tagged_arr* _temp963; struct _tuple0* _temp960= _temp945->name; _temp962=*
_temp960; _LL964: _temp963= _temp962.f2; goto _LL961; _LL961: name=( struct
_tagged_arr*) _temp963; goto _LL918;} _LL924: if( !(( unsigned int) _temp947->name)){
name= 0;}{ struct _tuple0 _temp967; struct _tagged_arr* _temp968; struct _tuple0*
_temp965=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp947->name))->v;
_temp967=* _temp965; _LL969: _temp968= _temp967.f2; goto _LL966; _LL966: name=(
struct _tagged_arr*) _temp968; goto _LL918;} _LL926: if( !(( unsigned int)
_temp949->name)){ name= 0;}{ struct _tuple0 _temp972; struct _tagged_arr*
_temp973; struct _tuple0* _temp970=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp949->name))->v; _temp972=* _temp970; _LL974: _temp973=
_temp972.f2; goto _LL971; _LL971: name=( struct _tagged_arr*) _temp973; goto
_LL918;} _LL928: { struct _tuple0 _temp977; struct _tagged_arr* _temp978; struct
_tuple0* _temp975= _temp951->name; _temp977=* _temp975; _LL979: _temp978=
_temp977.f2; goto _LL976; _LL976: name=( struct _tagged_arr*) _temp978; goto
_LL918;} _LL930: { struct _tuple0 _temp982; struct _tagged_arr* _temp983; struct
_tuple0* _temp980= _temp953->name; _temp982=* _temp980; _LL984: _temp983=
_temp982.f2; goto _LL981; _LL981: name=( struct _tagged_arr*) _temp983; goto
_LL918;} _LL932: goto _LL934; _LL934: goto _LL936; _LL936: goto _LL938; _LL938:
goto _LL940; _LL940: goto _LL942; _LL942: name= 0; goto _LL918; _LL918:;} if( !((
unsigned int) name)? ! _temp916: 0){ continue;} if(( unsigned int) name){
ifdefmacro=({ struct Cyc_Std_String_pa_struct _temp986; _temp986.tag= Cyc_Std_String_pa;
_temp986.f1=( struct _tagged_arr)* name;{ void* _temp985[ 1u]={& _temp986}; Cyc_Std_aprintf(
_tag_arr("_%s_def_", sizeof( unsigned char), 9u), _tag_arr( _temp985, sizeof(
void*), 1u));}});({ struct Cyc_Std_String_pa_struct _temp988; _temp988.tag= Cyc_Std_String_pa;
_temp988.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp987[ 1u]={& _temp988};
Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof( unsigned char), 12u),
_tag_arr( _temp987, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp990; _temp990.tag= Cyc_Std_String_pa; _temp990.f1=( struct _tagged_arr)
ifdefmacro;{ void* _temp989[ 1u]={& _temp990}; Cyc_Std_fprintf( out_file,
_tag_arr("#define %s\n", sizeof( unsigned char), 12u), _tag_arr( _temp989,
sizeof( void*), 1u));}}); Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp991[ 1u]; _temp991[ 0u]= _temp915;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp991, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);({ void* _temp992[ 0u]={}; Cyc_Std_fprintf( out_file,
_tag_arr("#endif\n", sizeof( unsigned char), 8u), _tag_arr( _temp992, sizeof(
void*), 0u));});} else{ Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp993[ 1u]; _temp993[ 0u]= _temp915;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp993, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);}}} _temp765=({ struct Cyc_Std_String_pa_struct _temp996;
_temp996.tag= Cyc_Std_String_pa; _temp996.f1=( struct _tagged_arr) _temp761;{
struct Cyc_Std_String_pa_struct _temp995; _temp995.tag= Cyc_Std_String_pa;
_temp995.f1=( struct _tagged_arr) filename;{ void* _temp994[ 2u]={& _temp995,&
_temp996}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp994, sizeof( void*), 2u));}}}); if(
Cyc_Std_system(( struct _tagged_arr) _temp765) !=  0){ return 1;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp761, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp779= Cyc_Lexing_from_file( in_file);{ struct _tuple9* entry2;
while(( entry2=(( struct _tuple9*(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_suck_line)(
_temp779)) !=  0) { struct _tuple9 _temp999; struct _tagged_arr* _temp1000;
struct _tagged_arr _temp1002; struct _tuple9* _temp997=( struct _tuple9*)
_check_null( entry2); _temp999=* _temp997; _LL1003: _temp1002= _temp999.f1; goto
_LL1001; _LL1001: _temp1000= _temp999.f2; goto _LL998; _LL998: if((( int(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( _temp814,
_temp1000)){({ struct Cyc_Std_String_pa_struct _temp1005; _temp1005.tag= Cyc_Std_String_pa;
_temp1005.f1=( struct _tagged_arr)* _temp1000;{ void* _temp1004[ 1u]={&
_temp1005}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof(
unsigned char), 12u), _tag_arr( _temp1004, sizeof( void*), 1u));}});({ struct
Cyc_Std_String_pa_struct _temp1007; _temp1007.tag= Cyc_Std_String_pa; _temp1007.f1=(
struct _tagged_arr) _temp1002;{ void* _temp1006[ 1u]={& _temp1007}; Cyc_Std_fprintf(
out_file, _tag_arr("%s\n", sizeof( unsigned char), 4u), _tag_arr( _temp1006,
sizeof( void*), 1u));}});({ void* _temp1008[ 0u]={}; Cyc_Std_fprintf( out_file,
_tag_arr("#endif\n", sizeof( unsigned char), 8u), _tag_arr( _temp1008, sizeof(
void*), 0u));});}} Cyc_Std_fclose( in_file); Cyc_Std_remove(( struct _tagged_arr)
_temp761); if( epilogue !=  0){ struct Cyc_List_List* _temp1009= epilogue; for(
0; _temp1009 !=  0; _temp1009= _temp1009->tl){ struct _tuple10 _temp1012; struct
_tagged_arr _temp1013; struct _tagged_arr _temp1015; struct _tuple10* _temp1010=(
struct _tuple10*) _temp1009->hd; _temp1012=* _temp1010; _LL1016: _temp1015=
_temp1012.f1; goto _LL1014; _LL1014: _temp1013= _temp1012.f2; goto _LL1011;
_LL1011: if( _temp1013.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?
_temp1015.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
defined_symbols,({ struct _tagged_arr* _temp1017=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp1017[ 0]= _temp1015; _temp1017;})):
0){ Cyc_Std_fputs( _temp1013, out_file);} else{({ struct Cyc_Std_String_pa_struct
_temp1019; _temp1019.tag= Cyc_Std_String_pa; _temp1019.f1=( struct _tagged_arr)
_temp1015;{ void* _temp1018[ 1u]={& _temp1019}; Cyc_log( _tag_arr("%s is not supported on this platform\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1018, sizeof( void*), 1u));}});}}}({
void* _temp1020[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n", sizeof(
unsigned char), 3u), _tag_arr( _temp1020, sizeof( void*), 0u));});({ void*
_temp1021[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp1021, sizeof( void*), 0u));}); Cyc_Std_fclose(
out_file); if( cstubs !=  0){ out_file=( struct Cyc_Std___sFILE*) _check_null(
Cyc_cstubs_file);{ struct Cyc_List_List* _temp1022= cstubs; for( 0; _temp1022 != 
0; _temp1022= _temp1022->tl){ struct _tuple10 _temp1025; struct _tagged_arr
_temp1026; struct _tagged_arr _temp1028; struct _tuple10* _temp1023=( struct
_tuple10*) _temp1022->hd; _temp1025=* _temp1023; _LL1029: _temp1028= _temp1025.f1;
goto _LL1027; _LL1027: _temp1026= _temp1025.f2; goto _LL1024; _LL1024: if(
_temp1026.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1028.curr
== (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp1030=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1030[ 0]= _temp1028; _temp1030;})): 0){ Cyc_Std_fputs(
_temp1026, out_file);}}}} out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _temp1032; _temp1032.tag= Cyc_Std_String_pa;
_temp1032.f1=( struct _tagged_arr) _temp764;{ void* _temp1031[ 1u]={& _temp1032};
Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\n\n", sizeof( unsigned char),
16u), _tag_arr( _temp1031, sizeof( void*), 1u));}}); if( cycstubs !=  0){
out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({ void*
_temp1033[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("namespace Std {\n",
sizeof( unsigned char), 17u), _tag_arr( _temp1033, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp1034= cycstubs; for( 0; _temp1034 !=  0; _temp1034=
_temp1034->tl){ struct _tuple10 _temp1037; struct _tagged_arr _temp1038; struct
_tagged_arr _temp1040; struct _tuple10* _temp1035=( struct _tuple10*) _temp1034->hd;
_temp1037=* _temp1035; _LL1041: _temp1040= _temp1037.f1; goto _LL1039; _LL1039:
_temp1038= _temp1037.f2; goto _LL1036; _LL1036: if( _temp1038.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1040.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct _tagged_arr*
_temp1042=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp1042[ 0]= _temp1040; _temp1042;})): 0){ Cyc_Std_fputs( _temp1038, out_file);}}}({
void* _temp1043[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n\n", sizeof(
unsigned char), 4u), _tag_arr( _temp1043, sizeof( void*), 0u));});} if( Cyc_Std_chdir((
struct _tagged_arr) _temp754)){({ struct Cyc_Std_String_pa_struct _temp1045;
_temp1045.tag= Cyc_Std_String_pa; _temp1045.f1=( struct _tagged_arr) _temp754;{
void* _temp1044[ 1u]={& _temp1045}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not change directory to %s\n",
sizeof( unsigned char), 41u), _tag_arr( _temp1044, sizeof( void*), 1u));}});
return 1;} return 0;}}}}}}}}} int Cyc_process_specfile( struct _tagged_arr file,
struct _tagged_arr dir){ struct Cyc_Std___sFILE* _temp1058= Cyc_Std_fopen( file,
_tag_arr("r", sizeof( unsigned char), 2u)); if( !(( unsigned int) _temp1058)){
return 1;}{ struct Cyc_Std___sFILE* _temp1059=( struct Cyc_Std___sFILE*)
_check_null( _temp1058); struct Cyc_Lexing_lexbuf* _temp1060= Cyc_Lexing_from_file(
_temp1059); struct _tuple11* entry; while(( entry=(( struct _tuple11*(*)( struct
Cyc_Lexing_lexbuf* lexbuf)) Cyc_spec)( _temp1060)) !=  0) { struct _tuple11
_temp1063; struct Cyc_List_List* _temp1064; struct Cyc_List_List* _temp1066;
struct Cyc_List_List* _temp1068; struct Cyc_List_List* _temp1070; struct Cyc_List_List*
_temp1072; struct Cyc_List_List* _temp1074; struct _tagged_arr _temp1076; struct
_tuple11* _temp1061=( struct _tuple11*) _check_null( entry); _temp1063=*
_temp1061; _LL1077: _temp1076= _temp1063.f1; goto _LL1075; _LL1075: _temp1074=
_temp1063.f2; goto _LL1073; _LL1073: _temp1072= _temp1063.f3; goto _LL1071;
_LL1071: _temp1070= _temp1063.f4; goto _LL1069; _LL1069: _temp1068= _temp1063.f5;
goto _LL1067; _LL1067: _temp1066= _temp1063.f6; goto _LL1065; _LL1065: _temp1064=
_temp1063.f7; goto _LL1062; _LL1062: if( Cyc_process_file( _temp1076, dir,
_temp1074, _temp1072, _temp1070, _temp1068, _temp1066, _temp1064)){ Cyc_Std_fclose(
_temp1059); return 1;}} Cyc_Std_fclose( _temp1059); return 0;}} int Cyc_getsize(
struct _tagged_arr dir, struct _tagged_arr includes, struct _tagged_arr type){
struct _tagged_arr _temp1078= Cyc_Filename_concat( dir, _tag_arr("getsize.c",
sizeof( unsigned char), 10u)); struct _tagged_arr _temp1079= Cyc_Filename_concat(
dir, _tag_arr("getsize", sizeof( unsigned char), 8u)); struct _tagged_arr
_temp1080= Cyc_Filename_concat( dir, _tag_arr("getsize.out", sizeof(
unsigned char), 12u)); struct Cyc_Std___sFILE* _temp1081= Cyc_Std_fopen(( struct
_tagged_arr) _temp1078, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) _temp1081)){({ struct Cyc_Std_String_pa_struct _temp1083;
_temp1083.tag= Cyc_Std_String_pa; _temp1083.f1=( struct _tagged_arr) _temp1078;{
void* _temp1082[ 1u]={& _temp1083}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create %s\n",
sizeof( unsigned char), 28u), _tag_arr( _temp1082, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1085; _temp1085.tag= Cyc_Std_String_pa;
_temp1085.f1=( struct _tagged_arr) type;{ void* _temp1084[ 1u]={& _temp1085};
Cyc_log( _tag_arr("Warning: could not find the size of %s\n", sizeof(
unsigned char), 40u), _tag_arr( _temp1084, sizeof( void*), 1u));}}); return - 1;}({
struct Cyc_Std_String_pa_struct _temp1090; _temp1090.tag= Cyc_Std_String_pa;
_temp1090.f1=( struct _tagged_arr) type;{ struct Cyc_Std_String_pa_struct
_temp1089; _temp1089.tag= Cyc_Std_String_pa; _temp1089.f1=( struct _tagged_arr)
type;{ struct Cyc_Std_String_pa_struct _temp1088; _temp1088.tag= Cyc_Std_String_pa;
_temp1088.f1=( struct _tagged_arr) _temp1080;{ struct Cyc_Std_String_pa_struct
_temp1087; _temp1087.tag= Cyc_Std_String_pa; _temp1087.f1=( struct _tagged_arr)
includes;{ void* _temp1086[ 4u]={& _temp1087,& _temp1088,& _temp1089,& _temp1090};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( _temp1081), _tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof( unsigned char), 139u), _tag_arr( _temp1086, sizeof( void*), 4u));}}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1081));{ struct
_tagged_arr _temp1091=({ struct Cyc_Std_String_pa_struct _temp1111; _temp1111.tag=
Cyc_Std_String_pa; _temp1111.f1=( struct _tagged_arr) _temp1078;{ struct Cyc_Std_String_pa_struct
_temp1110; _temp1110.tag= Cyc_Std_String_pa; _temp1110.f1=( struct _tagged_arr)
_temp1079;{ void* _temp1109[ 2u]={& _temp1110,& _temp1111}; Cyc_Std_aprintf(
_tag_arr("gcc -o %s %s", sizeof( unsigned char), 13u), _tag_arr( _temp1109,
sizeof( void*), 2u));}}}); if( Cyc_Std_system(( struct _tagged_arr) _temp1091)
!=  0){({ struct Cyc_Std_String_pa_struct _temp1093; _temp1093.tag= Cyc_Std_String_pa;
_temp1093.f1=( struct _tagged_arr) type;{ void* _temp1092[ 1u]={& _temp1093};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof( unsigned char), 59u), _tag_arr( _temp1092, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1095; _temp1095.tag= Cyc_Std_String_pa;
_temp1095.f1=( struct _tagged_arr) type;{ void* _temp1094[ 1u]={& _temp1095};
Cyc_log( _tag_arr("Warning: could not find the size of %s\n", sizeof(
unsigned char), 40u), _tag_arr( _temp1094, sizeof( void*), 1u));}}); return - 1;}{
int _temp1096= Cyc_Std_system(( struct _tagged_arr) _temp1079);({ struct Cyc_Std_Int_pa_struct
_temp1099; _temp1099.tag= Cyc_Std_Int_pa; _temp1099.f1=( int)(( unsigned int)
_temp1096);{ struct Cyc_Std_String_pa_struct _temp1098; _temp1098.tag= Cyc_Std_String_pa;
_temp1098.f1=( struct _tagged_arr) type;{ void* _temp1097[ 2u]={& _temp1098,&
_temp1099}; Cyc_log( _tag_arr("size of %s returned by system is %d\n", sizeof(
unsigned char), 37u), _tag_arr( _temp1097, sizeof( void*), 2u));}}});{ struct
Cyc_Std___sFILE* _temp1100= Cyc_Std_fopen(( struct _tagged_arr) _temp1080,
_tag_arr("r", sizeof( unsigned char), 2u)); int w= - 1; if(({ struct Cyc_Std_IntPtr_sa_struct
_temp1102; _temp1102.tag= Cyc_Std_IntPtr_sa; _temp1102.f1=& w;{ void* _temp1101[
1u]={& _temp1102}; Cyc_Std_fscanf(( struct Cyc_Std___sFILE*) _check_null(
_temp1100), _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1101,
sizeof( void*), 1u));}}) !=  1){({ struct Cyc_Std_String_pa_struct _temp1104;
_temp1104.tag= Cyc_Std_String_pa; _temp1104.f1=( struct _tagged_arr) type;{ void*
_temp1103[ 1u]={& _temp1104}; Cyc_log( _tag_arr("Warning: could not find the size of %s\n",
sizeof( unsigned char), 40u), _tag_arr( _temp1103, sizeof( void*), 1u));}});
return - 1;}({ struct Cyc_Std_Int_pa_struct _temp1108; _temp1108.tag= Cyc_Std_Int_pa;
_temp1108.f1=( int)(( unsigned int) w);{ struct Cyc_Std_String_pa_struct
_temp1107; _temp1107.tag= Cyc_Std_String_pa; _temp1107.f1=( struct _tagged_arr)
_temp1080;{ struct Cyc_Std_String_pa_struct _temp1106; _temp1106.tag= Cyc_Std_String_pa;
_temp1106.f1=( struct _tagged_arr) type;{ void* _temp1105[ 3u]={& _temp1106,&
_temp1107,& _temp1108}; Cyc_log( _tag_arr("size of %s read from file %s is %d\n",
sizeof( unsigned char), 36u), _tag_arr( _temp1105, sizeof( void*), 3u));}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1100)); Cyc_Std_remove((
struct _tagged_arr) _temp1080); Cyc_Std_remove(( struct _tagged_arr) _temp1078);
Cyc_Std_remove(( struct _tagged_arr) _temp1079); return w;}}}} int Cyc_main( int
argc, struct _tagged_arr argv){ if( argc <  3){({ void* _temp1112[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof( unsigned char), 105u), _tag_arr( _temp1112, sizeof( void*), 0u));});
return 1;}{ struct _tagged_arr _temp1113=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 1)); if( Cyc_force_directory_prefixes((
struct _tagged_arr) _temp1113)? 1: Cyc_force_directory(( struct _tagged_arr)
_temp1113)){({ struct Cyc_Std_String_pa_struct _temp1115; _temp1115.tag= Cyc_Std_String_pa;
_temp1115.f1=( struct _tagged_arr) _temp1113;{ void* _temp1114[ 1u]={& _temp1115};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1114, sizeof( void*), 1u));}});
return 1;} Cyc_log_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1113, _tag_arr("BUILDLIB.LOG", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _temp1117; _temp1117.tag= Cyc_Std_String_pa;
_temp1117.f1=( struct _tagged_arr) _temp1113;{ void* _temp1116[ 1u]={& _temp1117};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create log file in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1116, sizeof( void*), 1u));}});
return 1;} Cyc_cstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1113, _tag_arr("cstubs.c", sizeof( unsigned char), 9u)),
_tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1119; _temp1119.tag= Cyc_Std_String_pa;
_temp1119.f1=( struct _tagged_arr) _temp1113;{ void* _temp1118[ 1u]={& _temp1119};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1118, sizeof( void*), 1u));}});
return 1;} Cyc_cycstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1113, _tag_arr("cycstubs.cyc", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1121; _temp1121.tag= Cyc_Std_String_pa;
_temp1121.f1=( struct _tagged_arr) _temp1113;{ void* _temp1120[ 1u]={& _temp1121};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof( unsigned char), 52u), _tag_arr( _temp1120, sizeof( void*), 1u));}});
return 1;}({ void* _temp1122[ 0u]={}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file), _tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1122, sizeof( void*), 0u));}); Cyc_sizeof_unsignedlongint=
Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned long int", sizeof( unsigned char), 18u)); Cyc_sizeof_unsignedshortint=
Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned short int", sizeof( unsigned char), 19u)); Cyc_sizeof_shortint=
Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("short int", sizeof( unsigned char), 10u)); Cyc_sizeof_int= Cyc_getsize((
struct _tagged_arr) _temp1113, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("int", sizeof( unsigned char), 4u)); Cyc_sizeof_short= Cyc_getsize((
struct _tagged_arr) _temp1113, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("short", sizeof( unsigned char), 6u)); Cyc_sizeof_fdmask= Cyc_getsize((
struct _tagged_arr) _temp1113, _tag_arr("#include <sys/select.h>", sizeof(
unsigned char), 24u), _tag_arr("fd_mask", sizeof( unsigned char), 8u)); Cyc_sizeof___fdmask=
Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("#include <sys/select.h>",
sizeof( unsigned char), 24u), _tag_arr("__fd_mask", sizeof( unsigned char), 10u));
Cyc_sizeof_uint32= Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("#include <sys/types.h>",
sizeof( unsigned char), 23u), _tag_arr("__uint32_t", sizeof( unsigned char), 11u));
Cyc_sizeof_sockaddr= Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("#include <sys/socket.h>",
sizeof( unsigned char), 24u), _tag_arr("struct sockaddr", sizeof( unsigned char),
16u)); Cyc_sizeof_inport= Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("in_port_t", sizeof( unsigned char), 10u));
Cyc_sizeof_inaddr= Cyc_getsize(( struct _tagged_arr) _temp1113, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("struct in_addr", sizeof( unsigned char),
15u));{ int i= 2; for( 0; i <  argc; i ++){ Cyc_process_specfile(( struct
_tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), i)),( struct _tagged_arr) _temp1113);}} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_log_file)); Cyc_Std_fclose(( struct
Cyc_Std___sFILE*) _check_null( Cyc_cstubs_file)); Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file)); return 0;}}
