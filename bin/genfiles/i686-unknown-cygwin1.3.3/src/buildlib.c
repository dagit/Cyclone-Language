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
__p, struct Cyc_Std_timezone* __z); extern int select( int n, struct Cyc_Std__types_fd_set*
readfds, struct Cyc_Std__types_fd_set* writefds, struct Cyc_Std__types_fd_set*
exceptfds, struct Cyc_Std_timeval* timeout); struct Cyc_Std_option{ struct
_tagged_arr name; int has_arg; int* flag; int val; } ; extern unsigned int alarm(
unsigned int seconds); extern int close( int); extern void _exit( int); extern
int getpid(); extern int getppid(); extern int fork(); extern int fchdir( int);
extern int fchown( int, unsigned short, unsigned short); extern int fsync( int);
extern int ftruncate( int, int); extern int dup( int); extern int dup2( int, int);
extern unsigned short getuid(); extern int setuid( unsigned short uid); extern
unsigned short geteuid(); extern int seteuid( unsigned short euid); extern
unsigned short getgid(); extern int setgid( unsigned short gid); extern
unsigned short getegid(); extern int setegid( unsigned short egid); extern int
nice( int); extern int pause(); extern int pipe( int* filedes); extern int lseek(
int filedes, int offset, int whence); extern unsigned int sleep( unsigned int);
extern int isatty( int); int Cyc_Std_access( struct _tagged_arr, int); int Cyc_Std_chdir(
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
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
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
const int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType= 6;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual f2;
struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_StructType= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 11; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_AnonStructType= 12; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 13;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 14; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
15; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 16; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 17;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
18; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 19; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
int Cyc_enum_hack= 0; struct _tuple9{ struct _tagged_arr f1; struct _tagged_arr*
f2; } ; extern struct _tuple9* Cyc_suck_line( struct Cyc_Lexing_lexbuf*); extern
int Cyc_suck_macroname( struct Cyc_Lexing_lexbuf*); extern int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf*); struct _tagged_arr Cyc_current_line=( struct
_tagged_arr){( void*) 0u,( void*) 0u,( void*)( 0u +  0u)}; struct _tuple10{
struct _tagged_arr f1; struct _tagged_arr f2; } ; struct _tuple11{ struct
_tagged_arr f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct Cyc_List_List*
f4; struct Cyc_List_List* f5; struct Cyc_List_List* f6; struct Cyc_List_List* f7;
} ; extern struct _tuple11* Cyc_spec( struct Cyc_Lexing_lexbuf*); extern int Cyc_commands(
struct Cyc_Lexing_lexbuf*); extern int Cyc_snarfsymbols( struct Cyc_Lexing_lexbuf*);
extern int Cyc_block( struct Cyc_Lexing_lexbuf*); extern int Cyc_block_string(
struct Cyc_Lexing_lexbuf*); extern int Cyc_block_comment( struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_header=( struct _tagged_arr){( void*) 0u,( void*)
0u,( void*)( 0u +  0u)}; struct Cyc_List_List* Cyc_snarfed_symbols= 0; struct
Cyc_List_List* Cyc_current_symbols= 0; struct Cyc_List_List* Cyc_current_cstubs=
0; struct Cyc_List_List* Cyc_current_cycstubs= 0; struct Cyc_List_List* Cyc_current_prologue=
0; struct Cyc_List_List* Cyc_current_epilogue= 0; struct Cyc_List_List* Cyc_current_omit_symbols=
0; struct Cyc_Buffer_t* Cyc_specbuf= 0; int Cyc_braces_to_match= 0; int Cyc_parens_to_match=
0; int Cyc_numdef= 0; const int Cyc_lex_base[ 494u]=( const int[ 494u]){ 0, 0,
75, 192, 305, 285, 310, 166, 311, 91, 27, 384, 28, 503, 617, 695, 288, 338, 93,
- 3, 0, - 2, - 1, - 8, - 3, 1, - 1, 586, - 4, 2, 166, - 5, 308, 612, 339, - 6, -
7, - 3, 16, - 2, 29, 11, 777, - 3, 855, 13, - 14, 223, - 13, 107, 22, 26, 28, 24,
- 12, 28, 58, 56, 65, 52, 70, 320, 969, 1084, 386, 93, 83, 90, 112, 96, 106, 115,
125, 119, 150, 173, 372, 410, 226, 218, 211, 231, 231, 415, 505, 229, 228, 226,
235, 223, 240, 498, 1198, 1313, 510, 226, 244, 231, 234, 236, 256, 515, 1427,
1542, - 9, 530, - 10, 238, 268, 638, 1656, 1771, 975, - 8, 662, 675, 1848, 1925,
683, 2000, 267, 267, 267, 265, 261, 278, 0, 12, 4, 2081, 5, 682, 2089, 2114, 941,
49, 388, 6, 2182, 7, 960, 2190, 2215, 1051, - 29, 274, 275, 317, 304, 310, 294,
316, 310, 324, 1200, 1229, 332, 329, 321, 406, 333, 329, 340, 343, 365, 375, 377,
688, 1110, 367, 376, 406, 407, 410, 809, 1112, 417, 413, 1319, 421, 429, 1098,
1339, 424, 420, 1429, 427, 431, 429, 449, 447, 1211, 1345, 454, 454, 496, 490,
532, 531, 532, 528, 1544, 548, 548, 546, 550, 538, 1658, - 13, 540, 539, 1773,
2264, 546, 541, 2164, - 11, 579, 546, 2288, 548, 559, 570, 592, 573, 2290, 595,
582, 602, 585, 595, 2298, 621, 603, 612, 609, 605, 9, 11, 629, 609, 2314, 631,
630, 643, 626, 635, 2324, 636, 645, 1213, - 16, 1130, 640, 640, 647, 666, 677,
687, 674, 675, 707, 718, 725, 719, - 27, 728, 732, 728, 738, 745, 773, - 23, 753,
769, 761, 785, 796, 791, 803, 819, 820, - 17, 806, 802, 802, 824, 851, - 24, 842,
840, 833, 875, 871, 888, 868, 890, 902, 890, 894, 885, 886, - 26, 894, 888, 891,
901, 918, 910, 912, 928, 935, 936, 1, 4, 6, 937, 938, 951, 950, 962, 962, 970,
1000, 2, 24, 1001, 1014, 2238, 19, 20, 21, 1037, 1001, 999, 1014, 1020, 1022,
1077, 1083, 1085, - 22, 1032, 1033, 1088, 1090, 1102, - 19, 1050, 1051, 1106,
1107, 1134, - 20, 1081, 1082, 1137, 1139, 1171, - 18, 1119, 1120, 1175, 1178,
1192, - 21, 1139, 1141, 1549, - 28, 1140, 1133, 1148, 1146, 1159, 4, - 15, 1162,
1152, 1154, 1178, 1191, 1259, - 7, - 8, 8, 1439, 2322, 9, 1414, 2330, 2355, 1515,
1569, - 49, 1442, - 2, 1231, - 4, 1233, 1289, 2137, 1264, 1292, 1295, 1624, 1269,
2412, 2455, 1275, 1277, 1280, 1281, 2525, 1286, 1348, - 36, - 42, - 37, 2600,
1298, - 40, - 25, 1311, - 45, - 39, - 48, 2675, 2704, 1742, 1288, 1298, 2169,
2714, 2744, 2493, 2422, 2777, 2808, 2846, 1290, 1379, 2916, 2954, 1371, 1381,
1373, 1410, 1406, 1480, - 6, - 34, 1330, 2886, - 47, - 30, - 32, - 46, - 31, -
33, 1365, 2994, 1402, 1404, 1643, 1406, 1407, 1408, 1446, 1480, 1481, 1482, 1521,
1522, 3067, 3151, 2299, 1532, - 41, - 38, - 35, 1797, 3233, 1540, 3316, 1532, 15,
1475, 1476, 1478, 1478, 1474, 1485, 1555}; const int Cyc_lex_backtrk[ 494u]=(
const int[ 494u]){ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, - 1, - 1, - 1, - 1, 2, - 1, 8, - 1, 3, 5,
- 1, - 1, 6, 5, - 1, - 1, - 1, 6, - 1, 4, 1, 0, - 1, 0, 1, - 1, 10, - 1, 13, 13,
13, 13, 13, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 10, 1, 3,
- 1, 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, 2, 8, 3, 5, - 1, 6, 5, 6, 4, 2,
8, 3, 5, - 1, 6, 5, - 1, 28, 28, 28, 28, 28, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 6, 1, 9, 2, 4, - 1, 5, 4, - 1, - 1, 2, - 1, 48, - 1, 48, 48,
48, 48, 48, 48, 48, 48, 5, 7, 48, 48, 48, 48, 0, 48, 48, - 1, - 1, - 1, 0, 43, -
1, - 1, 42, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9, 5, 6, 5, 5,
- 1, 4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, 9, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 2, - 1, - 1, 2, 1, 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1};
const int Cyc_lex_default[ 494u]=( const int[ 494u]){ - 1, - 1, - 1, 394, 383,
144, 23, 36, 23, 19, - 1, - 1, 12, 28, 46, 31, 36, 23, 19, 0, - 1, 0, 0, 0, 0, -
1, 0, - 1, 0, - 1, - 1, 0, - 1, - 1, - 1, 0, 0, 0, - 1, 0, 40, - 1, - 1, 0, - 1,
- 1, 0, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, 0, - 1, - 1, - 1, - 1, -
1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 136, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1,
- 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, -
1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 0, - 1, 0, - 1, 0, - 1, - 1, 460, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0, - 1, - 1, 0, 0, - 1, 0, 0, 0, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_trans[
3573u]=( const int[ 3573u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 19, 28, 486, 19,
28, 19, 28, 113, 19, 45, 45, 45, 45, 45, 21, 45, 0, 0, 0, 0, 0, 22, 314, 325,
487, 376, 21, - 1, - 1, 21, - 1, - 1, 45, 315, 45, 316, 22, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 28, 22, 238, 239, 120, 40, 326, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 28, 360, 354, 348, 484, 136, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
127, 85, 20, 78, 65, 55, 56, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
57, 58, 59, 60, 482, 61, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 66,
67, 37, 395, 396, 395, 395, 396, 39, 22, 68, 69, 70, 71, 72, 135, 34, 34, 34, 34,
34, 34, 34, 34, 95, 73, 395, 397, 398, 74, 96, 399, 400, 401, 114, 114, 402, 403,
114, 404, 405, 406, 407, 408, 408, 408, 408, 408, 408, 408, 408, 408, 409, 75,
410, 411, 412, 114, 19, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413,
413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 19, -
1, - 1, 414, 413, 76, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413,
413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 384, 415,
145, 385, 22, 24, 24, 37, 137, 128, 79, 80, 146, 81, 62, 62, 82, 83, 62, 86, 38,
87, 88, 89, 386, 90, 91, 107, 97, 26, 26, 98, 21, 24, 21, 99, 25, 62, 100, 101,
108, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 109, 121, 122, 123, 124, 125, 26,
33, 33, 33, 33, 33, 33, 126, 147, 77, 77, 377, 370, 77, 148, 35, 35, 35, 35, 35,
35, 35, 35, 64, 64, 387, - 1, 64, 149, - 1, 138, 129, 77, 33, 33, 33, 33, 33, 33,
39, 252, 22, 248, 150, 151, 152, 64, 77, 77, 153, 35, 77, 84, 84, 154, 232, 84,
226, 27, 217, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 77, 161, 162,
163, 164, 84, 21, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 165, 166,
167, 179, 119, 171, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 92, 92, 28,
186, 92, 115, 115, 84, 84, 115, 172, 84, 94, 94, 173, 187, 94, 102, 102, 174,
177, 102, 178, 92, 180, 181, 21, 184, 115, 185, 84, 209, 105, 105, 21, 94, 105,
35, 188, 189, 102, 190, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 104, 105, 191, 202, 195, 21, 21, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 21, 196, 28, 197, 116, 29, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 47, 47, 22, 198, 47, 199, 200, 35, 30, 30, 30, 30, 30, 30, 30, 30, 201, 203,
- 1, 204, 205, 110, 110, 47, 206, 110, 207, 106, 210, 211, 214, 215, 220, 221,
33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 110, 114, 114, 222, 218, 114, 48, 33, 33,
33, 33, 33, 33, 31, 118, 118, 223, 224, 118, 225, 227, 228, 118, 118, 114, 219,
118, 168, 168, 229, 230, 168, 231, 242, 41, 41, 32, 118, 41, 33, 33, 33, 33, 33,
33, 118, 49, 235, 50, 236, 168, 237, 51, 233, 240, 241, 243, 41, 52, 53, 134,
134, 134, 134, 134, 134, 134, 134, 234, 244, 245, 246, 247, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 249, 250, 303, 304, 289, 283, 19, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 273,
266, 259, 260, 42, 261, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 175, 175, 43, 262, 175, 263, 264,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 265, 267, 268, 269, 270, 271, 175, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 272, 274, 275, 276, 44, 54, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 277, 278,
279, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 280, 281, 282, 284, 285, 286, 287,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 288, 290, 291, 292, 44, 28, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 62,
62, 293, 294, 62, 295, 112, 112, 296, 297, 112, 35, 35, 35, 35, 35, 35, 35, 35,
298, 299, 300, 301, 62, 302, 366, 305, 306, 307, 112, 143, 143, 143, 143, 143,
143, 143, 143, 308, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 309, 310, 311, 312,
313, 317, 318, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 319, 320, 321, 322, 63, 323, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 19, 64, 64, 324, 327, 64, 113, 35, 35, 35, 35, 35, 35, 35, 35, 182,
182, 328, 342, 182, 334, 335, 336, 337, 64, 338, 339, 168, 168, 175, 175, 168,
340, 175, 341, 343, 344, 345, 182, 346, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
168, 347, 175, 349, 350, 351, 352, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 353, 355, 356, 357,
63, 358, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 92, 92, 154, 154, 92, 359, 154, 361, 362, 363,
176, 169, 364, 192, 192, 250, 250, 192, 170, 250, 253, 254, 255, 92, 256, 154,
365, 367, 257, 368, 371, 155, 155, 155, 372, 155, 192, 258, 250, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 373, 374, 375, 378, 379, 155, 380, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 381, 382, 46, 302, 93, 480, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 31, 94, 94, 156, 459, 94,
478, 178, 178, 451, 157, 178, 426, 158, 144, 251, 423, 265, 424, 455, 422, 369,
420, 159, 94, 160, 418, 182, 182, 479, 178, 182, 458, 192, 192, 456, 457, 192,
421, 106, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 182, 425, 113, 113, 35, 454,
192, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 288, 113, 113, 35, 93, 416, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
102, 102, 185, 185, 102, 353, 185, 347, 183, 365, 359, 216, 393, 393, 193, 481,
393, 481, 481, 35, 31, 31, 36, 102, 194, 185, 392, 392, 392, 392, 392, 392, 392,
392, 104, 393, 417, 22, 481, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
54, 36, 35, 31, 31, 36, 450, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
36, 208, 46, 376, 103, 450, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
104, 105, 105, 201, 201, 105, 450, 201, 368, 368, 251, 282, 368, 31, 31, 31, 31,
31, 31, 31, 31, 341, 22, 22, 105, 488, 201, 489, 393, 393, 490, 368, 393, 491,
492, 54, 493, 22, 450, 369, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 0,
393, 0, 22, 0, 0, 0, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 0, 0, 0, 0,
103, 0, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 110, 110, 207, 207,
110, 452, 207, 453, 453, 453, 453, 453, 453, 453, 453, 453, 453, 341, 0, 0, 0, 0,
0, 110, 0, 207, 476, 476, 476, 476, 476, 476, 476, 476, 208, 0, 0, 0, 0, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 0, 0, 0, 0, 0, 0, 0, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 0, 0, 0, 0, 111, 0, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 36, 112, 112, 212, 212, 112, 432, 212, 432, 0, 0, 433,
433, 433, 433, 433, 433, 433, 433, 433, 433, 0, 0, 0, 112, 0, 212, 481, 0, 481,
481, 0, 0, 0, 0, 113, 0, 0, 0, 0, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 481, 0, 0, 0, 0, 0, 0, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
0, 0, 0, 0, 111, 0, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 22, 0, 0, 0, 0, 0, 0, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 0, 0, 0, 0, 117, 0, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
117, 117, 22, 0, 0, 0, 0, 0, 0, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
0, 0, 0, 0, 117, 0, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 0, 0, 0, 0, 0, 0, 0, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 28, 0, 0, 130, 119, 0, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 0, 0, 0, 0, 0, 0, 131, 131, 131, 131, 131, 131, 131,
131, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, - 1, 0, 0, 0, 0, 0, 0,
133, 133, 133, 133, 133, 133, 0, 0, 133, 133, 133, 133, 133, 133, 133, 133, 133,
133, 0, 215, 215, 0, - 1, 215, 31, 133, 133, 133, 133, 133, 133, 0, 133, 133,
133, 133, 133, 133, 28, 0, 0, 139, 215, 0, 0, 0, 0, 132, 0, 0, 0, 216, 0, 0, 0,
0, 0, 133, 133, 133, 133, 133, 133, 433, 433, 433, 433, 433, 433, 433, 433, 433,
433, 0, 0, 461, 140, 140, 140, 140, 140, 140, 140, 140, 142, 142, 142, 142, 142,
142, 142, 142, 142, 142, 0, 0, 0, 0, 0, 0, 0, 142, 142, 142, 142, 142, 142, 0, 0,
142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 212, 212, 0, 0, 212, 0, 31,
142, 142, 142, 142, 142, 142, 0, 142, 142, 142, 142, 142, 142, 0, 0, 0, 212, 219,
219, 225, 225, 219, 141, 225, 0, 113, 329, 231, 231, 0, 330, 231, 142, 142, 142,
142, 142, 142, 0, 331, 219, 332, 225, 241, 241, 0, 0, 241, 0, 36, 231, 35, 19,
247, 247, 388, 0, 247, 341, 19, 0, 0, 0, 0, 0, 0, 241, 477, 477, 477, 477, 477,
477, 477, 477, 31, 247, 333, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 213, 389, 389,
389, 389, 389, 389, 389, 389, 391, 391, 391, 391, 391, 391, 391, 391, 391, 391,
0, 0, 0, 0, 0, - 1, 0, 391, 391, 391, 391, 391, 391, 0, 0, 391, 391, 391, 391,
391, 391, 391, 391, 391, 391, 0, 0, 0, 0, 0, 0, 28, 391, 391, 391, 391, 391, 391,
0, 391, 391, 391, 391, 391, 391, 0, 0, 0, 0, 0, 0, 0, 0, 0, 390, 0, 0, 0, 0, 0,
0, 0, 0, 0, 391, 391, 391, 391, 391, 391, 427, 0, 438, 438, 438, 438, 438, 438,
438, 438, 439, 439, 437, 437, 437, 437, 437, 437, 437, 437, 437, 437, 0, 429, 0,
0, 0, 0, 0, 0, 440, 0, 0, 0, 0, 0, 0, 0, 0, 441, 0, 0, 442, 427, 0, 428, 428,
428, 428, 428, 428, 428, 428, 428, 428, 429, 0, 0, 0, 0, 0, 0, 440, 0, 0, 0, 429,
0, 0, 0, 0, 441, 0, 430, 442, 0, 0, 0, 436, 0, 436, 0, 431, 437, 437, 437, 437,
437, 437, 437, 437, 437, 437, 0, 0, 0, 0, 0, 429, 0, 0, 0, 0, 0, 0, 430, 0, 0, 0,
0, 0, 0, 0, 0, 431, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 0, 0, 0, 0,
0, 0, 0, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 0, 0, 0, 0, 419, 0,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 419, 419, 0, 0, 0, 0, 0, 0, 0, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 0, 0, 0, 0, 419, 0, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
434, 434, 434, 434, 434, 434, 434, 434, 434, 434, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 435, 106, 0, 0, 0, 0, 427, 106, 428, 428, 428, 428, 428, 428, 428, 428, 428,
428, 433, 433, 433, 433, 433, 433, 433, 433, 433, 433, 0, 429, 0, 0, 435, 106, 0,
0, 430, 0, 0, 106, 104, 0, 0, 0, 0, 431, 104, 0, 434, 434, 434, 434, 434, 434,
434, 434, 434, 434, 0, 0, 0, 429, 0, 0, 0, 0, 0, 0, 430, 435, 106, 0, 104, 0, 0,
0, 106, 431, 104, 0, 0, 437, 437, 437, 437, 437, 437, 437, 437, 437, 437, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 435, 106, 106, 0, 0, 0, 0, 106, 106, 427, 0, 438, 438,
438, 438, 438, 438, 438, 438, 439, 439, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 429, 0,
106, 0, 0, 0, 0, 448, 106, 0, 0, 0, 0, 0, 0, 427, 449, 439, 439, 439, 439, 439,
439, 439, 439, 439, 439, 0, 0, 0, 0, 0, 429, 0, 0, 0, 0, 0, 429, 448, 0, 0, 0, 0,
0, 446, 0, 0, 449, 0, 0, 0, 0, 0, 447, 0, 0, 453, 453, 453, 453, 453, 453, 453,
453, 453, 453, 0, 0, 0, 429, 0, 0, 0, 0, 0, 0, 446, 435, 106, 0, 0, 0, 0, 0, 106,
447, 443, 443, 443, 443, 443, 443, 443, 443, 443, 443, 0, 0, 0, 0, 0, 0, 0, 443,
443, 443, 443, 443, 443, 435, 106, 0, 0, 0, 0, 0, 106, 0, 0, 0, 0, 0, 0, 0, 443,
443, 443, 443, 443, 443, 443, 443, 443, 443, 0, 443, 443, 443, 443, 443, 443,
443, 443, 443, 443, 443, 443, 0, 0, 0, 462, 0, 444, 0, 0, 463, 0, 0, 0, 0, 0,
445, 0, 0, 464, 464, 464, 464, 464, 464, 464, 464, 0, 443, 443, 443, 443, 443,
443, 465, 0, 0, 0, 0, 444, 0, 0, 0, 0, 0, 0, 0, 0, 445, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 466, 0, 0, 0, 0, 467, 468, 0, 0, 0, 469, 0, 0, 0, 0, 0, 0, 0,
470, 0, 0, 0, 471, 0, 472, 0, 473, 0, 474, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 0, 0, 0, 0, 0, 0, 0, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 0, 0, 0, 0, 0, 0, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 272,
0, 0, 0, 0, 0, 0, 0, 0, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 0, 0,
0, 0, 0, 0, 0, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 0, 0, 0, 0, 0,
0, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 21, 0, 0, 483, 0, 0, 0,
482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 0, 0, 0, 0, 0, 0, 0, 482, 482,
482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, 482, 482, 482, 482, 482, 0, 0, 0, 0, 482, 0, 482, 482, 482, 482,
482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, 482, 482, 482, 485, 0, 0, 0, 0, 0, 0, 0, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 0, 0, 0, 0, 0, 0, 0, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 0, 0, 0, 0, 484, 0, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0}; const int Cyc_lex_check[ 3573u]=( const int[ 3573u]){ - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 0, 25, 29, 0, 128, 130, 137, 139, 385, 388, 41, 41,
45, 45, 41, 486, 45, - 1, - 1, - 1, - 1, - 1, 126, 313, 324, 0, 375, 10, 12, 40,
10, 12, 40, 41, 314, 45, 315, 20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 38, 127, 237,
238, 10, 38, 325, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 135, 329, 330, 331, 1, 135, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 50, 18,
51, 52, 53, 55, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 56, 57, 58, 59, 2, 60, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 65, 66, 7, 3, 3, 3, 3, 3, 7, 7, 67, 68, 69, 70,
71, 7, 30, 30, 30, 30, 30, 30, 30, 30, 49, 72, 3, 3, 3, 73, 49, 3, 3, 3, 47, 47,
3, 3, 47, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 74, 3, 3, 3, 47, 0, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 12, 40,
3, 3, 75, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 4, 3, 5, 4, 5, 6, 8, 16, 6, 8, 78, 79, 5, 80, 61, 61, 81, 82, 61, 85, 16,
86, 87, 88, 4, 89, 90, 95, 96, 6, 8, 97, 9, 17, 18, 98, 17, 61, 99, 100, 107, 32,
32, 32, 32, 32, 32, 32, 32, 32, 32, 108, 120, 121, 122, 123, 124, 17, 32, 32, 32,
32, 32, 32, 125, 5, 76, 76, 145, 146, 76, 5, 34, 34, 34, 34, 34, 34, 34, 34, 64,
64, 4, 136, 64, 5, 136, 6, 8, 76, 32, 32, 32, 32, 32, 32, 16, 147, 16, 148, 149,
150, 151, 64, 77, 77, 152, 7, 77, 83, 83, 153, 156, 83, 157, 17, 158, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 77, 160, 161, 162, 163, 83, 3, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 164, 165, 166, 169, 11, 170, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 91, 91, 64, 159, 91, 13,
13, 84, 84, 13, 171, 84, 94, 94, 172, 159, 94, 101, 101, 173, 176, 101, 177, 91,
179, 180, 77, 183, 13, 184, 84, 186, 105, 105, 5, 94, 105, 16, 187, 188, 101,
189, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 4, 105, 190, 193, 194, 6, 8,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 17, 195, 27, 196, 13, 27, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14,
84, 197, 14, 198, 199, 94, 27, 27, 27, 27, 27, 27, 27, 27, 200, 202, 136, 203,
204, 109, 109, 14, 205, 109, 206, 105, 209, 210, 213, 214, 218, 220, 33, 33, 33,
33, 33, 33, 33, 33, 33, 33, 109, 114, 114, 221, 217, 114, 14, 33, 33, 33, 33, 33,
33, 27, 115, 115, 222, 223, 115, 224, 226, 227, 118, 118, 114, 217, 118, 167,
167, 228, 229, 167, 230, 233, 15, 15, 27, 115, 15, 33, 33, 33, 33, 33, 33, 118,
14, 234, 14, 235, 167, 236, 14, 232, 239, 240, 242, 15, 14, 14, 131, 131, 131,
131, 131, 131, 131, 131, 232, 243, 244, 245, 246, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 248, 249, 253, 253, 254, 255, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 256, 257, 258,
259, 15, 260, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 174, 174, 15, 261, 174, 262, 263, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 264, 266, 267, 268, 269, 270, 174, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 271, 273, 274, 275, 42, 14, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 276, 277, 278, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 279, 280, 281, 283, 284, 285, 286, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 287, 289, 290, 291, 44, 15, 44, 44, 44, 44, 44, 44, 44, 44, 44,
44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 62, 62, 292,
293, 62, 294, 112, 112, 295, 296, 112, 134, 134, 134, 134, 134, 134, 134, 134,
297, 298, 299, 300, 62, 301, 303, 304, 305, 306, 112, 140, 140, 140, 140, 140,
140, 140, 140, 307, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 308, 309, 310, 311,
312, 316, 317, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 318, 319, 320, 321, 62, 322, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 63, 63, 323, 326, 63, 112, 143, 143, 143, 143, 143, 143, 143,
143, 181, 181, 327, 332, 181, 333, 334, 335, 336, 63, 337, 338, 168, 168, 175,
175, 168, 339, 175, 340, 342, 343, 344, 181, 345, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 168, 346, 175, 348, 349, 350, 351, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 352, 354,
355, 356, 63, 357, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 92, 92, 154, 154, 92, 358, 154, 360,
361, 362, 175, 168, 363, 191, 191, 250, 250, 191, 168, 250, 252, 252, 252, 92,
252, 154, 364, 366, 252, 367, 370, 155, 155, 154, 371, 155, 191, 252, 250, 92,
92, 92, 92, 92, 92, 92, 92, 92, 92, 372, 373, 374, 377, 378, 155, 379, 92, 92,
92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,
92, 92, 92, 92, 380, 381, 382, 397, 92, 399, 92, 92, 92, 92, 92, 92, 92, 92, 92,
92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 93, 93,
155, 402, 93, 400, 178, 178, 406, 155, 178, 409, 155, 403, 250, 410, 410, 410,
404, 411, 412, 412, 155, 93, 155, 414, 182, 182, 400, 178, 182, 403, 192, 192,
404, 404, 192, 420, 178, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 182, 423, 430,
431, 440, 452, 192, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 460, 430, 431, 440, 93, 415, 93, 93,
93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
93, 93, 93, 93, 102, 102, 185, 185, 102, 462, 185, 463, 182, 465, 466, 467, 386,
386, 192, 395, 386, 395, 395, 441, 444, 445, 446, 102, 192, 185, 389, 389, 389,
389, 389, 389, 389, 389, 185, 386, 415, 386, 395, 102, 102, 102, 102, 102, 102,
102, 102, 102, 102, 468, 447, 441, 444, 445, 446, 448, 102, 102, 102, 102, 102,
102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102,
102, 102, 102, 102, 102, 447, 469, 470, 471, 102, 448, 102, 102, 102, 102, 102,
102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102,
102, 102, 102, 102, 102, 102, 103, 103, 201, 201, 103, 449, 201, 368, 368, 472,
473, 368, 392, 392, 392, 392, 392, 392, 392, 392, 477, 483, 485, 103, 487, 201,
488, 393, 393, 489, 368, 393, 490, 491, 201, 492, 493, 449, 368, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, - 1, 393, - 1, 393, - 1, - 1, - 1, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, - 1, - 1, - 1, - 1, 103, - 1, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
103, 103, 103, 103, 103, 103, 103, 103, 110, 110, 207, 207, 110, 405, 207, 405,
405, 405, 405, 405, 405, 405, 405, 405, 405, 464, - 1, - 1, - 1, - 1, - 1, 110,
- 1, 207, 464, 464, 464, 464, 464, 464, 464, 464, 207, - 1, - 1, - 1, - 1, 110,
110, 110, 110, 110, 110, 110, 110, 110, 110, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
110, 110, 110, 110, 110, 110, 110, 110, 110, 110, - 1, - 1, - 1, - 1, 110, - 1,
110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 111, 111, 211, 211, 111,
429, 211, 429, - 1, - 1, 429, 429, 429, 429, 429, 429, 429, 429, 429, 429, - 1,
- 1, - 1, 111, - 1, 211, 481, - 1, 481, 481, - 1, - 1, - 1, - 1, 211, - 1, - 1,
- 1, - 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 481, - 1, - 1, - 1,
- 1, - 1, - 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, - 1, - 1, - 1,
- 1, 111, - 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, - 1, - 1, - 1, - 1, - 1, - 1,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, - 1, - 1, - 1, - 1, 116, - 1,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 117, 117, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, - 1, - 1, - 1, - 1, - 1, - 1, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, - 1, - 1, - 1, - 1, 117, - 1, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117,
117, 117, 117, 117, 117, 117, 117, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 129, - 1, - 1, 129, 119, - 1, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
119, 119, - 1, - 1, - 1, - 1, - 1, - 1, 129, 129, 129, 129, 129, 129, 129, 129,
132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 401, - 1, - 1, - 1, - 1, - 1,
- 1, 132, 132, 132, 132, 132, 132, - 1, - 1, 133, 133, 133, 133, 133, 133, 133,
133, 133, 133, - 1, 215, 215, - 1, 401, 215, 129, 133, 133, 133, 133, 133, 133,
- 1, 132, 132, 132, 132, 132, 132, 138, - 1, - 1, 138, 215, - 1, - 1, - 1, - 1,
129, - 1, - 1, - 1, 215, - 1, - 1, - 1, - 1, - 1, 133, 133, 133, 133, 133, 133,
432, 432, 432, 432, 432, 432, 432, 432, 432, 432, - 1, - 1, 401, 138, 138, 138,
138, 138, 138, 138, 138, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 141, 141, 141, 141, 141, 141, - 1, - 1, 142, 142,
142, 142, 142, 142, 142, 142, 142, 142, 212, 212, - 1, - 1, 212, - 1, 138, 142,
142, 142, 142, 142, 142, - 1, 141, 141, 141, 141, 141, 141, - 1, - 1, - 1, 212,
219, 219, 225, 225, 219, 138, 225, - 1, 212, 328, 231, 231, - 1, 328, 231, 142,
142, 142, 142, 142, 142, - 1, 328, 219, 328, 225, 241, 241, - 1, - 1, 241, - 1,
219, 231, 225, 387, 247, 247, 387, - 1, 247, 476, 231, - 1, - 1, - 1, - 1, - 1,
- 1, 241, 476, 476, 476, 476, 476, 476, 476, 476, 241, 247, 328, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 247, - 1, - 1, - 1, 212, 387, 387, 387, 387, 387, 387, 387,
387, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, - 1, - 1, - 1, - 1, - 1,
401, - 1, 390, 390, 390, 390, 390, 390, - 1, - 1, 391, 391, 391, 391, 391, 391,
391, 391, 391, 391, - 1, - 1, - 1, - 1, - 1, - 1, 387, 391, 391, 391, 391, 391,
391, - 1, 390, 390, 390, 390, 390, 390, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 387, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 391, 391, 391, 391, 391,
391, 407, - 1, 407, 407, 407, 407, 407, 407, 407, 407, 407, 407, 436, 436, 436,
436, 436, 436, 436, 436, 436, 436, - 1, 407, - 1, - 1, - 1, - 1, - 1, - 1, 407,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 407, - 1, - 1, 407, 408, - 1, 408, 408,
408, 408, 408, 408, 408, 408, 408, 408, 407, - 1, - 1, - 1, - 1, - 1, - 1, 407,
- 1, - 1, - 1, 408, - 1, - 1, - 1, - 1, 407, - 1, 408, 407, - 1, - 1, - 1, 435,
- 1, 435, - 1, 408, 435, 435, 435, 435, 435, 435, 435, 435, 435, 435, - 1, - 1,
- 1, - 1, - 1, 408, - 1, - 1, - 1, - 1, - 1, - 1, 408, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 408, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413,
413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, - 1,
- 1, - 1, - 1, 413, - 1, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413,
413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 413, 419,
419, 419, 419, 419, 419, 419, 419, 419, 419, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, - 1, - 1, - 1, - 1, 419, - 1,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 419,
419, 419, 419, 419, 419, 419, 419, 419, 419, 419, 427, 427, 427, 427, 427, 427,
427, 427, 427, 427, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 427,
427, - 1, - 1, - 1, - 1, 428, 427, 428, 428, 428, 428, 428, 428, 428, 428, 428,
428, 433, 433, 433, 433, 433, 433, 433, 433, 433, 433, - 1, 428, - 1, - 1, 427,
427, - 1, - 1, 428, - 1, - 1, 427, 433, - 1, - 1, - 1, - 1, 428, 433, - 1, 434,
434, 434, 434, 434, 434, 434, 434, 434, 434, - 1, - 1, - 1, 428, - 1, - 1, - 1,
- 1, - 1, - 1, 428, 434, 434, - 1, 433, - 1, - 1, - 1, 434, 428, 433, - 1, - 1,
437, 437, 437, 437, 437, 437, 437, 437, 437, 437, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 434, 434, 437, - 1, - 1, - 1, - 1, 434, 437, 438, - 1, 438,
438, 438, 438, 438, 438, 438, 438, 438, 438, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 438, - 1, 437, - 1, - 1, - 1, - 1, 438, 437, - 1, - 1, - 1,
- 1, - 1, - 1, 439, 438, 439, 439, 439, 439, 439, 439, 439, 439, 439, 439, - 1,
- 1, - 1, - 1, - 1, 438, - 1, - 1, - 1, - 1, - 1, 439, 438, - 1, - 1, - 1, - 1,
- 1, 439, - 1, - 1, 438, - 1, - 1, - 1, - 1, - 1, 439, - 1, - 1, 453, 453, 453,
453, 453, 453, 453, 453, 453, 453, - 1, - 1, - 1, 439, - 1, - 1, - 1, - 1, - 1,
- 1, 439, 453, 453, - 1, - 1, - 1, - 1, - 1, 453, 439, 442, 442, 442, 442, 442,
442, 442, 442, 442, 442, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 442, 442, 442, 442,
442, 442, 453, 453, - 1, - 1, - 1, - 1, - 1, 453, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 443, 443, 443, 443, 443, 443, 443, 443, 443, 443, - 1, 442, 442, 442, 442,
442, 442, 443, 443, 443, 443, 443, 443, - 1, - 1, - 1, 461, - 1, 443, - 1, - 1,
461, - 1, - 1, - 1, - 1, - 1, 443, - 1, - 1, 461, 461, 461, 461, 461, 461, 461,
461, - 1, 443, 443, 443, 443, 443, 443, 461, - 1, - 1, - 1, - 1, 443, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 443, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 461, - 1, - 1, - 1, - 1, 461, 461, - 1, - 1, - 1, 461,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 461, - 1, - 1, - 1, 461, - 1, 461, - 1, 461,
- 1, 461, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474,
474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, - 1, - 1, - 1,
- 1, - 1, - 1, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474,
474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 474, 475, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, - 1, - 1, - 1, - 1, - 1, - 1, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475, 475,
475, 482, - 1, - 1, 482, - 1, - 1, - 1, 482, 482, 482, 482, 482, 482, 482, 482,
482, 482, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, - 1, - 1, - 1, - 1, 482, - 1, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
482, 482, 482, 484, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, - 1, - 1, - 1, - 1, 484, - 1, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484, 484,
484, 484, 484, 484, 484, 484, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int Cyc_lex_engine(
int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int state; int base; int
backtrk; int c; state= start_state; if( state >=  0){ lbuf->lex_last_pos=( lbuf->lex_start_pos=
lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{ state=( - state) -  1;}
while( 1) { base= Cyc_lex_base[ _check_known_subscript_notnull( 494u, state)];
if( base <  0){ return( - base) -  1;} backtrk= Cyc_lex_backtrk[
_check_known_subscript_notnull( 494u, state)]; if( backtrk >=  0){ lbuf->lex_last_pos=
lbuf->lex_curr_pos; lbuf->lex_last_action= backtrk;} if( lbuf->lex_curr_pos >= 
lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){ return( - state) -  1;}
else{ c= 256;}} else{ c=( int)*(( unsigned char*) _check_unknown_subscript( lbuf->lex_buffer,
sizeof( unsigned char), lbuf->lex_curr_pos ++)); if( c ==  - 1){ c= 256;}} if(
Cyc_lex_check[ _check_known_subscript_notnull( 3573u, base +  c)] ==  state){
state= Cyc_lex_trans[ _check_known_subscript_notnull( 3573u, base +  c)];} else{
state= Cyc_lex_default[ _check_known_subscript_notnull( 494u, state)];} if(
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
1u));}}); return Cyc_slurp( lexbuf); case 3: _LL109:({ struct Cyc_Std_Int_pa_struct
_temp114; _temp114.tag= Cyc_Std_Int_pa; _temp114.f1=( int)(( unsigned int) Cyc_sizeof___fdmask);{
void* _temp113[ 1u]={& _temp114}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp113, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 4:
_LL112:({ struct Cyc_Std_Int_pa_struct _temp117; _temp117.tag= Cyc_Std_Int_pa;
_temp117.f1=( int)(( unsigned int) Cyc_sizeof_uint32);{ void* _temp116[ 1u]={&
_temp117}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp116, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 5: _LL115:({ struct Cyc_Std_Int_pa_struct
_temp120; _temp120.tag= Cyc_Std_Int_pa; _temp120.f1=( int)(( unsigned int) Cyc_sizeof_inport);{
void* _temp119[ 1u]={& _temp120}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp119, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 6:
_LL118:({ struct Cyc_Std_Int_pa_struct _temp123; _temp123.tag= Cyc_Std_Int_pa;
_temp123.f1=( int)(( unsigned int) Cyc_sizeof_int);{ void* _temp122[ 1u]={&
_temp123}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp122, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 7: _LL121:({ struct Cyc_Std_Int_pa_struct
_temp126; _temp126.tag= Cyc_Std_Int_pa; _temp126.f1=( int)(( unsigned int) Cyc_sizeof_short);{
void* _temp125[ 1u]={& _temp126}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp125, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 8:
_LL124:({ struct Cyc_Std_Int_pa_struct _temp129; _temp129.tag= Cyc_Std_Int_pa;
_temp129.f1=( int)(( unsigned int) Cyc_sizeof_unsignedlongint);{ void* _temp128[
1u]={& _temp129}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp128, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 9: _LL127:({ struct Cyc_Std_Int_pa_struct
_temp132; _temp132.tag= Cyc_Std_Int_pa; _temp132.f1=( int)(( unsigned int) Cyc_sizeof_unsignedshortint);{
void* _temp131[ 1u]={& _temp132}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp131, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 10:
_LL130:({ struct Cyc_Std_Int_pa_struct _temp135; _temp135.tag= Cyc_Std_Int_pa;
_temp135.f1=( int)(( unsigned int) Cyc_sizeof_shortint);{ void* _temp134[ 1u]={&
_temp135}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp134, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 11: _LL133:({ struct Cyc_Std_Int_pa_struct
_temp138; _temp138.tag= Cyc_Std_Int_pa; _temp138.f1=( int)(( unsigned int) Cyc_sizeof_sockaddr);{
void* _temp137[ 1u]={& _temp138}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp137, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 12:
_LL136:({ struct Cyc_Std_Int_pa_struct _temp141; _temp141.tag= Cyc_Std_Int_pa;
_temp141.f1=( int)(( unsigned int) Cyc_sizeof_inaddr);{ void* _temp140[ 1u]={&
_temp141}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp140, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 13: _LL139: Cyc_Std_fputs( _tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC ",
sizeof( unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 14: _LL142: Cyc_Std_fputs( _tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC ",
sizeof( unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 15: _LL143:({ struct Cyc_Std_Int_pa_struct
_temp146; _temp146.tag= Cyc_Std_Int_pa; _temp146.f1=( int)(( unsigned int) Cyc_enum_hack
++);{ void* _temp145[ 1u]={& _temp146}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("enum __CYCLONE_ENUM_HACK_%d {", sizeof(
unsigned char), 30u), _tag_arr( _temp145, sizeof( void*), 1u));}}); return Cyc_slurp(
lexbuf); case 16: _LL144: return Cyc_slurp( lexbuf); case 17: _LL147: return Cyc_slurp(
lexbuf); case 18: _LL148: return Cyc_slurp( lexbuf); case 19: _LL149: return Cyc_slurp(
lexbuf); case 20: _LL150: return Cyc_slurp( lexbuf); case 21: _LL151: return Cyc_slurp(
lexbuf); case 22: _LL152: Cyc_Std_fputs( _tag_arr("inline", sizeof(
unsigned char), 7u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 23: _LL153: Cyc_Std_fputs( _tag_arr("const",
sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 24: _LL154: Cyc_Std_fputs( _tag_arr("const",
sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 25: _LL155: Cyc_Std_fputs( _tag_arr("int",
sizeof( unsigned char), 4u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 26: _LL156: return Cyc_slurp( lexbuf); case 27:
_LL157: Cyc_parens_to_match= 1; if( Cyc_asm( lexbuf)){ return 1;} Cyc_Std_fputs(
_tag_arr("0", sizeof( unsigned char), 2u),( struct Cyc_Std___sFILE*) _check_null(
Cyc_slurp_out)); return Cyc_slurp( lexbuf); case 28: _LL158: Cyc_Std_fputc(( int)
Cyc_Lexing_lexeme_char( lexbuf, 0),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); default: _LL159:( lexbuf->refill_buff)( lexbuf);
return Cyc_slurp_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp161=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp161[ 0]=({ struct Cyc_Lexing_Error_struct _temp162; _temp162.tag= Cyc_Lexing_Error;
_temp162.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp162;}); _temp161;}));} int Cyc_slurp( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_slurp_rec( lexbuf, 5);} int Cyc_slurp_string_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL163: Cyc_Std_fputc(( int)'"',( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return 0; case 1: _LL164: return 1; case 2: _LL165:
return 1; case 3: _LL166:({ struct Cyc_Std_String_pa_struct _temp169; _temp169.tag=
Cyc_Std_String_pa; _temp169.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{
void* _temp168[ 1u]={& _temp169}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp168, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 4: _LL167:({ struct Cyc_Std_String_pa_struct _temp172; _temp172.tag= Cyc_Std_String_pa;
_temp172.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp171[ 1u]={&
_temp172}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp171, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 5: _LL170:({ struct Cyc_Std_String_pa_struct
_temp175; _temp175.tag= Cyc_Std_String_pa; _temp175.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp174[ 1u]={& _temp175}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp174, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 6: _LL173:({ struct Cyc_Std_String_pa_struct _temp178; _temp178.tag= Cyc_Std_String_pa;
_temp178.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp177[ 1u]={&
_temp178}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp177, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 7: _LL176:({ struct Cyc_Std_String_pa_struct
_temp181; _temp181.tag= Cyc_Std_String_pa; _temp181.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp180[ 1u]={& _temp181}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp180, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 8: _LL179:({ struct Cyc_Std_String_pa_struct _temp184; _temp184.tag= Cyc_Std_String_pa;
_temp184.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp183[ 1u]={&
_temp184}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp183, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); default: _LL182:( lexbuf->refill_buff)(
lexbuf); return Cyc_slurp_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp186=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp186[ 0]=({ struct Cyc_Lexing_Error_struct
_temp187; _temp187.tag= Cyc_Lexing_Error; _temp187.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp187;}); _temp186;}));} int Cyc_slurp_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_slurp_string_rec( lexbuf, 6);} int
Cyc_asm_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL188: if( Cyc_parens_to_match == 
1){ return 0;} Cyc_parens_to_match --; return Cyc_asm( lexbuf); case 1: _LL189:
Cyc_parens_to_match ++; return Cyc_asm( lexbuf); case 2: _LL190: Cyc_asm_string(
lexbuf); return Cyc_asm( lexbuf); case 3: _LL191: Cyc_asm_comment( lexbuf);
return Cyc_asm( lexbuf); case 4: _LL192: return Cyc_asm( lexbuf); case 5: _LL193:
return 0; case 6: _LL194: return Cyc_asm( lexbuf); default: _LL195:( lexbuf->refill_buff)(
lexbuf); return Cyc_asm_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp197=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp197[ 0]=({ struct Cyc_Lexing_Error_struct
_temp198; _temp198.tag= Cyc_Lexing_Error; _temp198.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp198;}); _temp197;}));} int Cyc_asm( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_rec( lexbuf, 7);} int Cyc_asm_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL199: return 0; case 1: _LL200:
return 1; case 2: _LL201: return 1; case 3: _LL202: return Cyc_asm_string(
lexbuf); case 4: _LL203: return Cyc_asm_string( lexbuf); case 5: _LL204: return
Cyc_asm_string( lexbuf); case 6: _LL205: return Cyc_asm_string( lexbuf); case 7:
_LL206: return Cyc_asm_string( lexbuf); case 8: _LL207: return Cyc_asm_string(
lexbuf); default: _LL208:( lexbuf->refill_buff)( lexbuf); return Cyc_asm_string_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp210=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp210[ 0]=({ struct Cyc_Lexing_Error_struct _temp211; _temp211.tag= Cyc_Lexing_Error;
_temp211.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp211;}); _temp210;}));} int Cyc_asm_string( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_asm_string_rec( lexbuf, 8);} int Cyc_asm_comment_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL212: return 0; case 1: _LL213: return 1; case 2: _LL214:
return Cyc_asm_comment( lexbuf); default: _LL215:( lexbuf->refill_buff)( lexbuf);
return Cyc_asm_comment_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp217=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp217[ 0]=({ struct Cyc_Lexing_Error_struct
_temp218; _temp218.tag= Cyc_Lexing_Error; _temp218.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp218;}); _temp217;}));} int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_asm_comment_rec( lexbuf, 9);}
struct _tuple9* Cyc_suck_line_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case
0: _LL219: Cyc_current_line= _tag_arr("#define ", sizeof( unsigned char), 9u);
Cyc_suck_macroname( lexbuf); return({ struct _tuple9* _temp221=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp221->f1= Cyc_current_line; _temp221->f2=(
struct _tagged_arr*) _check_null( Cyc_current_source); _temp221;}); case 1:
_LL220: return Cyc_suck_line( lexbuf); case 2: _LL222: return 0; default: _LL223:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_line_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp225=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp225[ 0]=({ struct Cyc_Lexing_Error_struct
_temp226; _temp226.tag= Cyc_Lexing_Error; _temp226.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp226;}); _temp225;}));} struct _tuple9* Cyc_suck_line(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_line_rec( lexbuf, 10);} int
Cyc_suck_macroname_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL227:
Cyc_current_source=({ struct _tagged_arr* _temp229=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp229[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp229;}); Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat(
Cyc_current_line,*(( struct _tagged_arr*) _check_null( Cyc_current_source)));
return Cyc_suck_restofline( lexbuf); default: _LL228:( lexbuf->refill_buff)(
lexbuf); return Cyc_suck_macroname_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp231=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp231[ 0]=({ struct Cyc_Lexing_Error_struct
_temp232; _temp232.tag= Cyc_Lexing_Error; _temp232.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp232;}); _temp231;}));} int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_macroname_rec( lexbuf, 11);}
int Cyc_suck_restofline_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL233:
Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat( Cyc_current_line,(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return 0; default: _LL234:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_restofline_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp236=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp236[ 0]=({ struct Cyc_Lexing_Error_struct
_temp237; _temp237.tag= Cyc_Lexing_Error; _temp237.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp237;}); _temp236;}));} int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_restofline_rec( lexbuf, 12);}
struct _tuple11* Cyc_spec_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL238:
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
_tuple11* _temp240=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11));
_temp240->f1= Cyc_current_header; _temp240->f2= Cyc_current_symbols; _temp240->f3=
Cyc_current_omit_symbols; _temp240->f4= Cyc_current_prologue; _temp240->f5= Cyc_current_epilogue;
_temp240->f6= Cyc_current_cstubs; _temp240->f7= Cyc_current_cycstubs; _temp240;});
case 1: _LL239: return Cyc_spec( lexbuf); case 2: _LL241: return 0; case 3:
_LL242:({ struct Cyc_Std_Int_pa_struct _temp245; _temp245.tag= Cyc_Std_Int_pa;
_temp245.f1=( int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{
void* _temp244[ 1u]={& _temp245}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof( unsigned char), 67u), _tag_arr( _temp244, sizeof( void*), 1u));}});
return 0; default: _LL243:( lexbuf->refill_buff)( lexbuf); return Cyc_spec_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp247=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp247[ 0]=({ struct Cyc_Lexing_Error_struct _temp248; _temp248.tag= Cyc_Lexing_Error;
_temp248.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp248;}); _temp247;}));} struct _tuple11* Cyc_spec( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_spec_rec( lexbuf, 13);} int Cyc_commands_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL249: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols( lexbuf)){
return 1;} Cyc_current_symbols=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols, Cyc_current_symbols);
return Cyc_commands( lexbuf); case 1: _LL250: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols(
lexbuf)){ return 1;} Cyc_current_omit_symbols=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols,
Cyc_current_omit_symbols); return Cyc_commands( lexbuf); case 2: _LL251: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp254=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp254->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp254->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp254;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp253=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp253->hd=( void*) x; _temp253->tl= Cyc_current_prologue;
_temp253;}); return Cyc_commands( lexbuf);} case 3: _LL252: { struct _tagged_arr
_temp256= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp256,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("prologue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp256, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp256, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp256; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp257= Cyc_Std_substring(( struct _tagged_arr) _temp256, 0,(
unsigned int)(( t.curr -  _temp256.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp259=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp259->f1=( struct
_tagged_arr) _temp257; _temp259->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp259;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp258=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp258->hd=( void*) x; _temp258->tl= Cyc_current_prologue;
_temp258;}); return Cyc_commands( lexbuf);}}}} case 4: _LL255: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp262=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp262->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp262->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp262;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp261=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp261->hd=( void*) x; _temp261->tl= Cyc_current_epilogue;
_temp261;}); return Cyc_commands( lexbuf);} case 5: _LL260: { struct _tagged_arr
_temp264= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp264,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("epilogue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp264, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp264, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp264; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp265= Cyc_Std_substring(( struct _tagged_arr) _temp264, 0,(
unsigned int)(( t.curr -  _temp264.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp267=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp267->f1=( struct
_tagged_arr) _temp265; _temp267->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp267;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp266=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp266->hd=( void*) x; _temp266->tl= Cyc_current_epilogue;
_temp266;}); return Cyc_commands( lexbuf);}}}} case 6: _LL263: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp270=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp270->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp270->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp270;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp269=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp269->hd=( void*) x; _temp269->tl= Cyc_current_cstubs;
_temp269;}); return Cyc_commands( lexbuf);} case 7: _LL268: { struct _tagged_arr
_temp272= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp272,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cstub", sizeof(
unsigned char), 6u))); while( isspace(( int)*(( unsigned char*)
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
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp275;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp274=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp274->hd=( void*) x; _temp274->tl= Cyc_current_cstubs;
_temp274;}); return Cyc_commands( lexbuf);}}}} case 8: _LL271: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp278=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp278->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp278->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp278;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp277=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp277->hd=( void*) x; _temp277->tl= Cyc_current_cycstubs;
_temp277;}); return Cyc_commands( lexbuf);} case 9: _LL276: { struct _tagged_arr
_temp280= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp280,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cycstub", sizeof(
unsigned char), 8u))); while( isspace(( int)*(( unsigned char*)
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
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp283;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp282=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp282->hd=( void*) x; _temp282->tl= Cyc_current_cycstubs;
_temp282;}); return Cyc_commands( lexbuf);}}}} case 10: _LL279: return Cyc_commands(
lexbuf); case 11: _LL284: return 0; case 12: _LL285: return 0; case 13: _LL286:({
struct Cyc_Std_Int_pa_struct _temp289; _temp289.tag= Cyc_Std_Int_pa; _temp289.f1=(
int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void*
_temp288[ 1u]={& _temp289}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof( unsigned char), 58u), _tag_arr( _temp288, sizeof( void*), 1u));}});
return 1; default: _LL287:( lexbuf->refill_buff)( lexbuf); return Cyc_commands_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp291=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp291[ 0]=({ struct Cyc_Lexing_Error_struct _temp292; _temp292.tag= Cyc_Lexing_Error;
_temp292.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp292;}); _temp291;}));} int Cyc_commands( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_commands_rec( lexbuf, 14);} int Cyc_snarfsymbols_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL293: Cyc_snarfed_symbols=({ struct Cyc_List_List* _temp295=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp295->hd=(
void*)({ struct _tagged_arr* _temp296=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp296[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp296;}); _temp295->tl= Cyc_snarfed_symbols; _temp295;}); return Cyc_snarfsymbols(
lexbuf); case 1: _LL294: return Cyc_snarfsymbols( lexbuf); case 2: _LL297:
return 0; case 3: _LL298:({ void* _temp300[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n", sizeof( unsigned char),
44u), _tag_arr( _temp300, sizeof( void*), 0u));}); return 1; case 4: _LL299:({
struct Cyc_Std_Int_pa_struct _temp303; _temp303.tag= Cyc_Std_Int_pa; _temp303.f1=(
int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void*
_temp302[ 1u]={& _temp303}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof( unsigned char), 57u), _tag_arr( _temp302, sizeof( void*), 1u));}});
return 1; default: _LL301:( lexbuf->refill_buff)( lexbuf); return Cyc_snarfsymbols_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp305=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp305[ 0]=({ struct Cyc_Lexing_Error_struct _temp306; _temp306.tag= Cyc_Lexing_Error;
_temp306.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp306;}); _temp305;}));} int Cyc_snarfsymbols( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_snarfsymbols_rec( lexbuf, 15);} int Cyc_block_rec( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate,
lexbuf); switch( lexstate){ case 0: _LL307: if( Cyc_braces_to_match ==  1){
return 0;} Cyc_braces_to_match --; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'}'); return Cyc_block( lexbuf); case 1: _LL308: Cyc_braces_to_match
++; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'{');
return Cyc_block( lexbuf); case 2: _LL309: Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'"'); Cyc_block_string( lexbuf); return Cyc_block(
lexbuf); case 3: _LL310: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf), _tag_arr("/*", sizeof( unsigned char), 3u)); Cyc_block_comment(
lexbuf); return Cyc_block( lexbuf); case 4: _LL311: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block( lexbuf); case 5: _LL312: return 0; case 6: _LL313:
Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf), Cyc_Lexing_lexeme_char(
lexbuf, 0)); return Cyc_block( lexbuf); default: _LL314:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp316=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp316[ 0]=({ struct Cyc_Lexing_Error_struct
_temp317; _temp317.tag= Cyc_Lexing_Error; _temp317.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp317;}); _temp316;}));} int Cyc_block( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_rec( lexbuf, 16);} int Cyc_block_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL318: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'"'); return 0; case 1: _LL319:
return 1; case 2: _LL320: return 1; case 3: _LL321: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 4: _LL322: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 5: _LL323: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 6: _LL324: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 7: _LL325: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 8: _LL326: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); default: _LL327:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp329=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp329[ 0]=({ struct Cyc_Lexing_Error_struct
_temp330; _temp330.tag= Cyc_Lexing_Error; _temp330.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp330;}); _temp329;}));} int Cyc_block_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_string_rec( lexbuf, 17);}
int Cyc_block_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL331:
Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),
_tag_arr("*/", sizeof( unsigned char), 3u)); return 0; case 1: _LL332: return 1;
case 2: _LL333: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return Cyc_block_comment(
lexbuf); default: _LL334:( lexbuf->refill_buff)( lexbuf); return Cyc_block_comment_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp336=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp336[ 0]=({ struct Cyc_Lexing_Error_struct _temp337; _temp337.tag= Cyc_Lexing_Error;
_temp337.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp337;}); _temp336;}));} int Cyc_block_comment( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_block_comment_rec( lexbuf, 18);} static void Cyc_pr( struct
_tagged_arr* sptr){({ struct Cyc_Std_String_pa_struct _temp339; _temp339.tag=
Cyc_Std_String_pa; _temp339.f1=( struct _tagged_arr)* sptr;{ void* _temp338[ 1u]={&
_temp339}; Cyc_Std_printf( _tag_arr(" %s\n", sizeof( unsigned char), 5u),
_tag_arr( _temp338, sizeof( void*), 1u));}});} extern void Cyc_scan_type( void*
t); struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
void Cyc_scan_exp( struct Cyc_Absyn_Exp* e){ void* _temp340=( void*)(( struct
Cyc_Absyn_Exp*) _check_null( e))->r; struct _tuple0* _temp418; struct _tuple0
_temp420; struct _tagged_arr* _temp421; struct _tuple0* _temp423; struct _tuple0
_temp425; struct _tagged_arr* _temp426; struct Cyc_List_List* _temp428; struct
Cyc_Absyn_Exp* _temp430; struct Cyc_Absyn_Exp* _temp432; struct Cyc_Absyn_Exp*
_temp434; struct Cyc_Absyn_Exp* _temp436; struct Cyc_Absyn_Exp* _temp438; struct
Cyc_Absyn_Exp* _temp440; struct Cyc_Absyn_Exp* _temp442; struct Cyc_Absyn_Exp*
_temp444; struct Cyc_Absyn_Exp* _temp446; struct Cyc_Absyn_Exp* _temp448; struct
Cyc_Absyn_Exp* _temp450; struct Cyc_Absyn_Exp* _temp452; struct Cyc_Absyn_Exp*
_temp454; struct Cyc_List_List* _temp456; struct Cyc_Absyn_Exp* _temp458; struct
Cyc_List_List* _temp460; struct Cyc_Absyn_Exp* _temp462; struct Cyc_Absyn_Exp*
_temp464; void* _temp466; void* _temp468; void* _temp470; struct _tagged_arr*
_temp472; struct Cyc_Absyn_Exp* _temp474; struct _tagged_arr* _temp476; struct
Cyc_Absyn_Exp* _temp478; void* _temp480; void* _temp482; struct Cyc_List_List*
_temp484; _LL342: if(*(( int*) _temp340) ==  Cyc_Absyn_Var_e){ _LL419: _temp418=((
struct Cyc_Absyn_Var_e_struct*) _temp340)->f1; _temp420=* _temp418; _LL422:
_temp421= _temp420.f2; goto _LL343;} else{ goto _LL344;} _LL344: if(*(( int*)
_temp340) ==  Cyc_Absyn_UnknownId_e){ _LL424: _temp423=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp340)->f1; _temp425=* _temp423; _LL427: _temp426= _temp425.f2; goto _LL345;}
else{ goto _LL346;} _LL346: if(*(( int*) _temp340) ==  Cyc_Absyn_Primop_e){
_LL429: _temp428=(( struct Cyc_Absyn_Primop_e_struct*) _temp340)->f2; goto
_LL347;} else{ goto _LL348;} _LL348: if(*(( int*) _temp340) ==  Cyc_Absyn_Subscript_e){
_LL433: _temp432=(( struct Cyc_Absyn_Subscript_e_struct*) _temp340)->f1; goto
_LL431; _LL431: _temp430=(( struct Cyc_Absyn_Subscript_e_struct*) _temp340)->f2;
goto _LL349;} else{ goto _LL350;} _LL350: if(*(( int*) _temp340) ==  Cyc_Absyn_SeqExp_e){
_LL437: _temp436=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp340)->f1; goto
_LL435; _LL435: _temp434=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp340)->f2;
goto _LL351;} else{ goto _LL352;} _LL352: if(*(( int*) _temp340) ==  Cyc_Absyn_AssignOp_e){
_LL441: _temp440=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp340)->f1; goto
_LL439; _LL439: _temp438=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp340)->f3;
goto _LL353;} else{ goto _LL354;} _LL354: if(*(( int*) _temp340) ==  Cyc_Absyn_Deref_e){
_LL443: _temp442=(( struct Cyc_Absyn_Deref_e_struct*) _temp340)->f1; goto _LL355;}
else{ goto _LL356;} _LL356: if(*(( int*) _temp340) ==  Cyc_Absyn_Sizeofexp_e){
_LL445: _temp444=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp340)->f1; goto
_LL357;} else{ goto _LL358;} _LL358: if(*(( int*) _temp340) ==  Cyc_Absyn_Address_e){
_LL447: _temp446=(( struct Cyc_Absyn_Address_e_struct*) _temp340)->f1; goto
_LL359;} else{ goto _LL360;} _LL360: if(*(( int*) _temp340) ==  Cyc_Absyn_Increment_e){
_LL449: _temp448=(( struct Cyc_Absyn_Increment_e_struct*) _temp340)->f1; goto
_LL361;} else{ goto _LL362;} _LL362: if(*(( int*) _temp340) ==  Cyc_Absyn_Conditional_e){
_LL455: _temp454=(( struct Cyc_Absyn_Conditional_e_struct*) _temp340)->f1; goto
_LL453; _LL453: _temp452=(( struct Cyc_Absyn_Conditional_e_struct*) _temp340)->f2;
goto _LL451; _LL451: _temp450=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp340)->f3; goto _LL363;} else{ goto _LL364;} _LL364: if(*(( int*) _temp340)
==  Cyc_Absyn_FnCall_e){ _LL459: _temp458=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp340)->f1; goto _LL457; _LL457: _temp456=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp340)->f2; goto _LL365;} else{ goto _LL366;} _LL366: if(*(( int*) _temp340)
==  Cyc_Absyn_UnknownCall_e){ _LL463: _temp462=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp340)->f1; goto _LL461; _LL461: _temp460=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp340)->f2; goto _LL367;} else{ goto _LL368;} _LL368: if(*(( int*) _temp340)
==  Cyc_Absyn_Cast_e){ _LL467: _temp466=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp340)->f1; goto _LL465; _LL465: _temp464=(( struct Cyc_Absyn_Cast_e_struct*)
_temp340)->f2; goto _LL369;} else{ goto _LL370;} _LL370: if(*(( int*) _temp340)
==  Cyc_Absyn_Malloc_e){ _LL469: _temp468=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp340)->f2; goto _LL371;} else{ goto _LL372;} _LL372: if(*(( int*) _temp340)
==  Cyc_Absyn_Sizeoftyp_e){ _LL471: _temp470=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp340)->f1; goto _LL373;} else{ goto _LL374;} _LL374: if(*(( int*) _temp340)
==  Cyc_Absyn_StructMember_e){ _LL475: _temp474=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp340)->f1; goto _LL473; _LL473: _temp472=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp340)->f2; goto _LL375;} else{ goto _LL376;} _LL376: if(*(( int*) _temp340)
==  Cyc_Absyn_StructArrow_e){ _LL479: _temp478=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp340)->f1; goto _LL477; _LL477: _temp476=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp340)->f2; goto _LL377;} else{ goto _LL378;} _LL378: if(*(( int*) _temp340)
==  Cyc_Absyn_Offsetof_e){ _LL483: _temp482=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp340)->f1; goto _LL481; _LL481: _temp480=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp340)->f2; goto _LL379;} else{ goto _LL380;} _LL380: if(*(( int*) _temp340)
==  Cyc_Absyn_Const_e){ goto _LL381;} else{ goto _LL382;} _LL382: if(*(( int*)
_temp340) ==  Cyc_Absyn_UnresolvedMem_e){ _LL485: _temp484=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp340)->f2; goto _LL383;} else{ goto _LL384;} _LL384: if(*(( int*) _temp340)
==  Cyc_Absyn_StmtExp_e){ goto _LL385;} else{ goto _LL386;} _LL386: if(*(( int*)
_temp340) ==  Cyc_Absyn_Throw_e){ goto _LL387;} else{ goto _LL388;} _LL388: if(*((
int*) _temp340) ==  Cyc_Absyn_NoInstantiate_e){ goto _LL389;} else{ goto _LL390;}
_LL390: if(*(( int*) _temp340) ==  Cyc_Absyn_Instantiate_e){ goto _LL391;} else{
goto _LL392;} _LL392: if(*(( int*) _temp340) ==  Cyc_Absyn_New_e){ goto _LL393;}
else{ goto _LL394;} _LL394: if(*(( int*) _temp340) ==  Cyc_Absyn_Gentyp_e){ goto
_LL395;} else{ goto _LL396;} _LL396: if(*(( int*) _temp340) ==  Cyc_Absyn_Tuple_e){
goto _LL397;} else{ goto _LL398;} _LL398: if(*(( int*) _temp340) ==  Cyc_Absyn_CompoundLit_e){
goto _LL399;} else{ goto _LL400;} _LL400: if(*(( int*) _temp340) ==  Cyc_Absyn_Array_e){
goto _LL401;} else{ goto _LL402;} _LL402: if(*(( int*) _temp340) ==  Cyc_Absyn_Comprehension_e){
goto _LL403;} else{ goto _LL404;} _LL404: if(*(( int*) _temp340) ==  Cyc_Absyn_Struct_e){
goto _LL405;} else{ goto _LL406;} _LL406: if(*(( int*) _temp340) ==  Cyc_Absyn_AnonStruct_e){
goto _LL407;} else{ goto _LL408;} _LL408: if(*(( int*) _temp340) ==  Cyc_Absyn_Tunion_e){
goto _LL409;} else{ goto _LL410;} _LL410: if(*(( int*) _temp340) ==  Cyc_Absyn_Enum_e){
goto _LL411;} else{ goto _LL412;} _LL412: if(*(( int*) _temp340) ==  Cyc_Absyn_AnonEnum_e){
goto _LL413;} else{ goto _LL414;} _LL414: if(*(( int*) _temp340) ==  Cyc_Absyn_Codegen_e){
goto _LL415;} else{ goto _LL416;} _LL416: if(*(( int*) _temp340) ==  Cyc_Absyn_Fill_e){
goto _LL417;} else{ goto _LL341;} _LL343: _temp426= _temp421; goto _LL345;
_LL345: Cyc_add_target( _temp426); return; _LL347: for( 0; _temp428 !=  0;
_temp428= _temp428->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_temp428->hd));} return; _LL349: _temp436= _temp432; _temp434= _temp430; goto
_LL351; _LL351: _temp440= _temp436; _temp438= _temp434; goto _LL353; _LL353: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp440); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp438);
return; _LL355: _temp444= _temp442; goto _LL357; _LL357: _temp446= _temp444;
goto _LL359; _LL359: _temp448= _temp446; goto _LL361; _LL361: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp448); return; _LL363: Cyc_scan_exp(( struct Cyc_Absyn_Exp*)
_temp454); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp452); Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp450); return; _LL365: _temp462= _temp458; _temp460=
_temp456; goto _LL367; _LL367: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp462);
for( 0; _temp460 !=  0; _temp460= _temp460->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _temp460->hd));} return; _LL369: Cyc_scan_type( _temp466);
Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp464); return; _LL371: _temp470=
_temp468; goto _LL373; _LL373: Cyc_scan_type( _temp470); return; _LL375:
_temp478= _temp474; _temp476= _temp472; goto _LL377; _LL377: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp478); Cyc_add_target( _temp476); return; _LL379: Cyc_scan_type(
_temp482);{ void* _temp486= _temp480; struct _tagged_arr* _temp492; _LL488: if(*((
int*) _temp486) ==  Cyc_Absyn_StructField){ _LL493: _temp492=(( struct Cyc_Absyn_StructField_struct*)
_temp486)->f1; goto _LL489;} else{ goto _LL490;} _LL490: if(*(( int*) _temp486)
==  Cyc_Absyn_TupleIndex){ goto _LL491;} else{ goto _LL487;} _LL489: Cyc_add_target(
_temp492); goto _LL487; _LL491: goto _LL487; _LL487:;} return; _LL381: return;
_LL383: for( 0; _temp484 !=  0; _temp484= _temp484->tl){ struct _tuple12
_temp496; struct Cyc_Absyn_Exp* _temp497; struct _tuple12* _temp494=( struct
_tuple12*) _temp484->hd; _temp496=* _temp494; _LL498: _temp497= _temp496.f2;
goto _LL495; _LL495: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp497);} return;
_LL385:({ void* _temp499[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Stmt_e\n",
sizeof( unsigned char), 26u), _tag_arr( _temp499, sizeof( void*), 0u));}); exit(
1); return; _LL387:({ void* _temp500[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Throw_e\n", sizeof( unsigned char), 27u), _tag_arr(
_temp500, sizeof( void*), 0u));}); exit( 1); return; _LL389:({ void* _temp501[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected NoInstantiate_e\n",
sizeof( unsigned char), 35u), _tag_arr( _temp501, sizeof( void*), 0u));}); exit(
1); return; _LL391:({ void* _temp502[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Instantiate_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp502, sizeof( void*), 0u));}); exit( 1); return; _LL393:({ void*
_temp503[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected New_e\n",
sizeof( unsigned char), 25u), _tag_arr( _temp503, sizeof( void*), 0u));}); exit(
1); return; _LL395:({ void* _temp504[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Gentyp_e\n", sizeof( unsigned char), 28u), _tag_arr(
_temp504, sizeof( void*), 0u));}); exit( 1); return; _LL397:({ void* _temp505[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tuple_e\n", sizeof(
unsigned char), 27u), _tag_arr( _temp505, sizeof( void*), 0u));}); exit( 1);
return; _LL399:({ void* _temp506[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected CompoundLit_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp506, sizeof( void*), 0u));}); exit( 1); return; _LL401:({ void*
_temp507[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Array_e\n",
sizeof( unsigned char), 27u), _tag_arr( _temp507, sizeof( void*), 0u));}); exit(
1); return; _LL403:({ void* _temp508[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Comprehension_e\n", sizeof( unsigned char), 35u),
_tag_arr( _temp508, sizeof( void*), 0u));}); exit( 1); return; _LL405:({ void*
_temp509[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Struct_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp509, sizeof( void*), 0u));}); exit(
1); return; _LL407:({ void* _temp510[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonStruct_e\n", sizeof( unsigned char), 32u),
_tag_arr( _temp510, sizeof( void*), 0u));}); exit( 1); return; _LL409:({ void*
_temp511[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tunion_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp511, sizeof( void*), 0u));}); exit(
1); return; _LL411:({ void* _temp512[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Enum_e\n", sizeof( unsigned char), 26u), _tag_arr(
_temp512, sizeof( void*), 0u));}); exit( 1); return; _LL413:({ void* _temp513[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AnonEnum_e\n",
sizeof( unsigned char), 30u), _tag_arr( _temp513, sizeof( void*), 0u));}); exit(
1); return; _LL415:({ void* _temp514[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Codegen_e\n", sizeof( unsigned char), 29u), _tag_arr(
_temp514, sizeof( void*), 0u));}); exit( 1); return; _LL417:({ void* _temp515[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Fill_e\n", sizeof(
unsigned char), 26u), _tag_arr( _temp515, sizeof( void*), 0u));}); exit( 1);
return; _LL341:;} void Cyc_scan_exp_opt( struct Cyc_Absyn_Exp* eo){ if((
unsigned int) eo){ Cyc_scan_exp( eo);} return;} void Cyc_scan_type( void* t){
void* _temp516= t; struct Cyc_Absyn_PtrInfo _temp570; struct Cyc_Absyn_Exp*
_temp572; void* _temp574; struct Cyc_Absyn_FnInfo _temp576; struct Cyc_List_List*
_temp578; struct Cyc_List_List* _temp580; struct _tuple0* _temp582; struct
_tuple0* _temp584; struct _tuple0* _temp586; struct _tuple0 _temp588; struct
_tagged_arr* _temp589; struct _tuple0* _temp591; struct _tuple0 _temp593; struct
_tagged_arr* _temp594; struct _tuple0* _temp596; struct _tuple0 _temp598; struct
_tagged_arr* _temp599; struct _tuple0* _temp601; struct _tuple0 _temp603; struct
_tagged_arr* _temp604; _LL518: if( _temp516 == ( void*) Cyc_Absyn_VoidType){
goto _LL519;} else{ goto _LL520;} _LL520: if(( unsigned int) _temp516 >  4u?*((
int*) _temp516) ==  Cyc_Absyn_IntType: 0){ goto _LL521;} else{ goto _LL522;}
_LL522: if( _temp516 == ( void*) Cyc_Absyn_FloatType){ goto _LL523;} else{ goto
_LL524;} _LL524: if( _temp516 == ( void*) Cyc_Absyn_DoubleType){ goto _LL525;}
else{ goto _LL526;} _LL526: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516)
==  Cyc_Absyn_PointerType: 0){ _LL571: _temp570=(( struct Cyc_Absyn_PointerType_struct*)
_temp516)->f1; goto _LL527;} else{ goto _LL528;} _LL528: if(( unsigned int)
_temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_ArrayType: 0){ _LL575: _temp574=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp516)->f1; goto _LL573; _LL573:
_temp572=(( struct Cyc_Absyn_ArrayType_struct*) _temp516)->f3; goto _LL529;}
else{ goto _LL530;} _LL530: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516)
==  Cyc_Absyn_FnType: 0){ _LL577: _temp576=(( struct Cyc_Absyn_FnType_struct*)
_temp516)->f1; goto _LL531;} else{ goto _LL532;} _LL532: if(( unsigned int)
_temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_AnonStructType: 0){ _LL579:
_temp578=(( struct Cyc_Absyn_AnonStructType_struct*) _temp516)->f1; goto _LL533;}
else{ goto _LL534;} _LL534: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516)
==  Cyc_Absyn_AnonUnionType: 0){ _LL581: _temp580=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp516)->f1; goto _LL535;} else{ goto _LL536;} _LL536: if(( unsigned int)
_temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL537;}
else{ goto _LL538;} _LL538: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516)
==  Cyc_Absyn_StructType: 0){ _LL583: _temp582=(( struct Cyc_Absyn_StructType_struct*)
_temp516)->f1; if( _temp582 ==  0){ goto _LL539;} else{ goto _LL540;}} else{
goto _LL540;} _LL540: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516) == 
Cyc_Absyn_UnionType: 0){ _LL585: _temp584=(( struct Cyc_Absyn_UnionType_struct*)
_temp516)->f1; if( _temp584 ==  0){ goto _LL541;} else{ goto _LL542;}} else{
goto _LL542;} _LL542: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516) == 
Cyc_Absyn_StructType: 0){ _LL587: _temp586=(( struct Cyc_Absyn_StructType_struct*)
_temp516)->f1; if( _temp586 ==  0){ goto _LL544;} else{ _temp588=* _temp586;
_LL590: _temp589= _temp588.f2; goto _LL543;}} else{ goto _LL544;} _LL544: if((
unsigned int) _temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_UnionType: 0){
_LL592: _temp591=(( struct Cyc_Absyn_UnionType_struct*) _temp516)->f1; if(
_temp591 ==  0){ goto _LL546;} else{ _temp593=* _temp591; _LL595: _temp594=
_temp593.f2; goto _LL545;}} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_EnumType: 0){ _LL597: _temp596=((
struct Cyc_Absyn_EnumType_struct*) _temp516)->f1; _temp598=* _temp596; _LL600:
_temp599= _temp598.f2; goto _LL547;} else{ goto _LL548;} _LL548: if((
unsigned int) _temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_TypedefType: 0){
_LL602: _temp601=(( struct Cyc_Absyn_TypedefType_struct*) _temp516)->f1;
_temp603=* _temp601; _LL605: _temp604= _temp603.f2; goto _LL549;} else{ goto
_LL550;} _LL550: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_Evar:
0){ goto _LL551;} else{ goto _LL552;} _LL552: if(( unsigned int) _temp516 >  4u?*((
int*) _temp516) ==  Cyc_Absyn_VarType: 0){ goto _LL553;} else{ goto _LL554;}
_LL554: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_TunionType:
0){ goto _LL555;} else{ goto _LL556;} _LL556: if(( unsigned int) _temp516 >  4u?*((
int*) _temp516) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL557;} else{ goto
_LL558;} _LL558: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_TupleType:
0){ goto _LL559;} else{ goto _LL560;} _LL560: if(( unsigned int) _temp516 >  4u?*((
int*) _temp516) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL561;} else{ goto _LL562;}
_LL562: if( _temp516 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL563;} else{ goto
_LL564;} _LL564: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_AccessEff:
0){ goto _LL565;} else{ goto _LL566;} _LL566: if(( unsigned int) _temp516 >  4u?*((
int*) _temp516) ==  Cyc_Absyn_JoinEff: 0){ goto _LL567;} else{ goto _LL568;}
_LL568: if(( unsigned int) _temp516 >  4u?*(( int*) _temp516) ==  Cyc_Absyn_RgnsEff:
0){ goto _LL569;} else{ goto _LL517;} _LL519: goto _LL521; _LL521: goto _LL523;
_LL523: goto _LL525; _LL525: return; _LL527: Cyc_scan_type(( void*) _temp570.elt_typ);
return; _LL529: Cyc_scan_type( _temp574); Cyc_scan_exp_opt( _temp572); return;
_LL531: Cyc_scan_type(( void*) _temp576.ret_typ);{ struct Cyc_List_List*
_temp606= _temp576.args; for( 0; _temp606 !=  0; _temp606= _temp606->tl){ struct
_tuple1 _temp609; void* _temp610; struct _tuple1* _temp607=( struct _tuple1*)
_temp606->hd; _temp609=* _temp607; _LL611: _temp610= _temp609.f3; goto _LL608;
_LL608: Cyc_scan_type( _temp610);}} if( _temp576.cyc_varargs !=  0){ Cyc_scan_type((
void*)( _temp576.cyc_varargs)->type);} return; _LL533: _temp580= _temp578; goto
_LL535; _LL535: for( 0; _temp580 !=  0; _temp580= _temp580->tl){ Cyc_scan_type((
void*)(( struct Cyc_Absyn_Structfield*) _temp580->hd)->type); Cyc_scan_exp_opt(((
struct Cyc_Absyn_Structfield*) _temp580->hd)->width);} return; _LL537: return;
_LL539: return; _LL541: return; _LL543: _temp594= _temp589; goto _LL545; _LL545:
_temp599= _temp594; goto _LL547; _LL547: _temp604= _temp599; goto _LL549; _LL549:
Cyc_add_target( _temp604); return; _LL551:({ void* _temp612[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: unexpected Evar\n", sizeof( unsigned char), 24u),
_tag_arr( _temp612, sizeof( void*), 0u));}); exit( 1); return; _LL553:({ void*
_temp613[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected VarType\n",
sizeof( unsigned char), 27u), _tag_arr( _temp613, sizeof( void*), 0u));}); exit(
1); return; _LL555:({ void* _temp614[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionType\n", sizeof( unsigned char), 30u),
_tag_arr( _temp614, sizeof( void*), 0u));}); exit( 1); return; _LL557:({ void*
_temp615[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected TunionFieldType\n",
sizeof( unsigned char), 35u), _tag_arr( _temp615, sizeof( void*), 0u));}); exit(
1); return; _LL559:({ void* _temp616[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n", sizeof( unsigned char), 29u), _tag_arr(
_temp616, sizeof( void*), 0u));}); exit( 1); return; _LL561:({ void* _temp617[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnHandleType\n",
sizeof( unsigned char), 33u), _tag_arr( _temp617, sizeof( void*), 0u));}); exit(
1); return; _LL563:({ void* _temp618[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected HeapRgn\n", sizeof( unsigned char), 27u), _tag_arr(
_temp618, sizeof( void*), 0u));}); exit( 1); return; _LL565:({ void* _temp619[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AccessEff\n",
sizeof( unsigned char), 29u), _tag_arr( _temp619, sizeof( void*), 0u));}); exit(
1); return; _LL567:({ void* _temp620[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected JoinEff\n", sizeof( unsigned char), 27u), _tag_arr(
_temp620, sizeof( void*), 0u));}); exit( 1); return; _LL569:({ void* _temp621[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnsEff\n", sizeof(
unsigned char), 27u), _tag_arr( _temp621, sizeof( void*), 0u));}); exit( 1);
return; _LL517:;} struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; struct _tuple8* Cyc_scan_decl( struct Cyc_Absyn_Decl* d){ Cyc_current_targets=({
struct Cyc_Set_Set** _temp622=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp622[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_temp622;});{ void* _temp623=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp649;
struct Cyc_Absyn_Fndecl* _temp651; struct Cyc_Absyn_Structdecl* _temp653; struct
Cyc_Absyn_Uniondecl* _temp655; struct Cyc_Absyn_Enumdecl* _temp657; struct Cyc_Absyn_Typedefdecl*
_temp659; _LL625: if(*(( int*) _temp623) ==  Cyc_Absyn_Var_d){ _LL650: _temp649=((
struct Cyc_Absyn_Var_d_struct*) _temp623)->f1; goto _LL626;} else{ goto _LL627;}
_LL627: if(*(( int*) _temp623) ==  Cyc_Absyn_Fn_d){ _LL652: _temp651=(( struct
Cyc_Absyn_Fn_d_struct*) _temp623)->f1; goto _LL628;} else{ goto _LL629;} _LL629:
if(*(( int*) _temp623) ==  Cyc_Absyn_Struct_d){ _LL654: _temp653=(( struct Cyc_Absyn_Struct_d_struct*)
_temp623)->f1; goto _LL630;} else{ goto _LL631;} _LL631: if(*(( int*) _temp623)
==  Cyc_Absyn_Union_d){ _LL656: _temp655=(( struct Cyc_Absyn_Union_d_struct*)
_temp623)->f1; goto _LL632;} else{ goto _LL633;} _LL633: if(*(( int*) _temp623)
==  Cyc_Absyn_Enum_d){ _LL658: _temp657=(( struct Cyc_Absyn_Enum_d_struct*)
_temp623)->f1; goto _LL634;} else{ goto _LL635;} _LL635: if(*(( int*) _temp623)
==  Cyc_Absyn_Typedef_d){ _LL660: _temp659=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp623)->f1; goto _LL636;} else{ goto _LL637;} _LL637: if(*(( int*) _temp623)
==  Cyc_Absyn_Let_d){ goto _LL638;} else{ goto _LL639;} _LL639: if(*(( int*)
_temp623) ==  Cyc_Absyn_Tunion_d){ goto _LL640;} else{ goto _LL641;} _LL641: if(*((
int*) _temp623) ==  Cyc_Absyn_Letv_d){ goto _LL642;} else{ goto _LL643;} _LL643:
if(*(( int*) _temp623) ==  Cyc_Absyn_Namespace_d){ goto _LL644;} else{ goto
_LL645;} _LL645: if(*(( int*) _temp623) ==  Cyc_Absyn_Using_d){ goto _LL646;}
else{ goto _LL647;} _LL647: if(*(( int*) _temp623) ==  Cyc_Absyn_ExternC_d){
goto _LL648;} else{ goto _LL624;} _LL626: { struct _tuple0 _temp663; struct
_tagged_arr* _temp664; struct _tuple0* _temp661= _temp649->name; _temp663=*
_temp661; _LL665: _temp664= _temp663.f2; goto _LL662; _LL662: Cyc_current_source=(
struct _tagged_arr*) _temp664; Cyc_scan_type(( void*) _temp649->type); Cyc_scan_exp_opt(
_temp649->initializer); goto _LL624;} _LL628: { struct _tuple0 _temp668; struct
_tagged_arr* _temp669; struct _tuple0* _temp666= _temp651->name; _temp668=*
_temp666; _LL670: _temp669= _temp668.f2; goto _LL667; _LL667: Cyc_current_source=(
struct _tagged_arr*) _temp669; Cyc_scan_type(( void*) _temp651->ret_type);{
struct Cyc_List_List* _temp671= _temp651->args; for( 0; _temp671 !=  0; _temp671=
_temp671->tl){ struct _tuple13 _temp674; void* _temp675; struct _tuple13*
_temp672=( struct _tuple13*) _temp671->hd; _temp674=* _temp672; _LL676: _temp675=
_temp674.f3; goto _LL673; _LL673: Cyc_scan_type( _temp675);}} if( _temp651->cyc_varargs
!=  0){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp651->cyc_varargs))->type);} if( _temp651->is_inline){({ void* _temp677[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: ignoring inline function\n",
sizeof( unsigned char), 35u), _tag_arr( _temp677, sizeof( void*), 0u));});} goto
_LL624;} _LL630: if( !(( unsigned int) _temp653->name)){ return 0;}{ struct
_tuple0 _temp680; struct _tagged_arr* _temp681; struct _tuple0* _temp678=(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp653->name))->v;
_temp680=* _temp678; _LL682: _temp681= _temp680.f2; goto _LL679; _LL679: Cyc_current_source=(
struct _tagged_arr*) _temp681; if(( unsigned int) _temp653->fields){{ struct Cyc_List_List*
_temp683=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp653->fields))->v;
for( 0; _temp683 !=  0; _temp683= _temp683->tl){ struct Cyc_Absyn_Structfield*
_temp684=( struct Cyc_Absyn_Structfield*) _temp683->hd; Cyc_scan_type(( void*)
_temp684->type); Cyc_scan_exp_opt( _temp684->width);}}{ struct Cyc_List_List*
_temp685=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp653->fields))->v;
for( 0; _temp685 !=  0; _temp685= _temp685->tl){;}}} goto _LL624;} _LL632: if( !((
unsigned int) _temp655->name)){ return 0;}{ struct _tuple0 _temp688; struct
_tagged_arr* _temp689; struct _tuple0* _temp686=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp655->name))->v; _temp688=* _temp686; _LL690: _temp689=
_temp688.f2; goto _LL687; _LL687: Cyc_current_source=( struct _tagged_arr*)
_temp689; if(( unsigned int) _temp655->fields){{ struct Cyc_List_List* _temp691=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp655->fields))->v;
for( 0; _temp691 !=  0; _temp691= _temp691->tl){ struct Cyc_Absyn_Structfield*
_temp692=( struct Cyc_Absyn_Structfield*) _temp691->hd; Cyc_add_target( _temp692->name);
Cyc_scan_type(( void*) _temp692->type); Cyc_scan_exp_opt( _temp692->width);}}{
struct Cyc_List_List* _temp693=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp655->fields))->v; for( 0; _temp693 !=  0; _temp693= _temp693->tl){;}}}
goto _LL624;} _LL634: { struct _tuple0 _temp696; struct _tagged_arr* _temp697;
struct _tuple0* _temp694= _temp657->name; _temp696=* _temp694; _LL698: _temp697=
_temp696.f2; goto _LL695; _LL695: Cyc_current_source=( struct _tagged_arr*)
_temp697; if(( unsigned int) _temp657->fields){{ struct Cyc_List_List* _temp699=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp657->fields))->v;
for( 0; _temp699 !=  0; _temp699= _temp699->tl){ struct Cyc_Absyn_Enumfield*
_temp700=( struct Cyc_Absyn_Enumfield*) _temp699->hd; Cyc_scan_exp_opt( _temp700->tag);}}{
struct Cyc_List_List* _temp701=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp657->fields))->v; for( 0; _temp701 !=  0; _temp701= _temp701->tl){;}}}
goto _LL624;} _LL636: { struct _tuple0 _temp704; struct _tagged_arr* _temp705;
struct _tuple0* _temp702= _temp659->name; _temp704=* _temp702; _LL706: _temp705=
_temp704.f2; goto _LL703; _LL703: Cyc_current_source=( struct _tagged_arr*)
_temp705; Cyc_scan_type(( void*) _temp659->defn); goto _LL624;} _LL638:({ void*
_temp707[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp707, sizeof( void*), 0u));}); exit(
1); return 0; _LL640:({ void* _temp708[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected tunion declaration\n", sizeof( unsigned char), 38u),
_tag_arr( _temp708, sizeof( void*), 0u));}); exit( 1); return 0; _LL642:({ void*
_temp709[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp709, sizeof( void*), 0u));}); exit(
1); return 0; _LL644:({ void* _temp710[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected namespace declaration\n", sizeof( unsigned char), 41u),
_tag_arr( _temp710, sizeof( void*), 0u));}); exit( 1); return 0; _LL646:({ void*
_temp711[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected using declaration\n",
sizeof( unsigned char), 37u), _tag_arr( _temp711, sizeof( void*), 0u));}); exit(
1); return 0; _LL648:({ void* _temp712[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n", sizeof( unsigned char),
42u), _tag_arr( _temp712, sizeof( void*), 0u));}); exit( 1); return 0; _LL624:;}
return({ struct _tuple8* _temp713=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp713->f1=( struct _tagged_arr*) _check_null( Cyc_current_source);
_temp713->f2=*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets));
_temp713;});} struct Cyc_Hashtable_Table* Cyc_new_deps(){ return(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), int(* hash)(
struct _tagged_arr*))) Cyc_Hashtable_create)( 107, Cyc_Std_strptrcmp, Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set* Cyc_find( struct Cyc_Hashtable_Table* t, struct _tagged_arr*
x){ struct _handler_cons _temp714; _push_handler(& _temp714);{ int _temp716= 0;
if( setjmp( _temp714.handler)){ _temp716= 1;} if( ! _temp716){{ struct Cyc_Set_Set*
_temp717=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key)) Cyc_Hashtable_lookup)( t, x); _npop_handler( 0u); return
_temp717;}; _pop_handler();} else{ void* _temp715=( void*) _exn_thrown; void*
_temp719= _temp715; _LL721: if( _temp719 ==  Cyc_Core_Not_found){ goto _LL722;}
else{ goto _LL723;} _LL723: goto _LL724; _LL722: return(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_LL724:( void) _throw( _temp719); _LL720:;}}} struct Cyc_Set_Set* Cyc_reachable(
struct Cyc_List_List* init, struct Cyc_Hashtable_Table* t){ struct Cyc_Set_Set*
emptyset=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); struct Cyc_Set_Set* curr;
for( curr= emptyset; init !=  0; init= init->tl){ curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( curr,( struct
_tagged_arr*) init->hd);}{ struct Cyc_Set_Set* _temp725= curr; while((( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp725) >  0) { struct Cyc_Set_Set*
_temp726= emptyset;{ struct Cyc_List_List* _temp727=(( struct Cyc_List_List*(*)(
struct Cyc_Set_Set* s)) Cyc_Set_elements)( _temp725); for( 0; _temp727 !=  0;
_temp727= _temp727->tl){ _temp726=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( _temp726, Cyc_find( t,( struct
_tagged_arr*) _temp727->hd));}} _temp725=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_diff)( _temp726, curr); curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( curr,
_temp725);} return curr;}} int Cyc_force_directory( struct _tagged_arr d){ int
_temp728=({ int _temp731[ 0u]; Cyc_Std_open( d, 0, _tag_arr( _temp731, sizeof(
int), 0u));}); if( _temp728 ==  - 1){ if( Cyc_Std_mkdir( d, 448) ==  - 1){({
struct Cyc_Std_String_pa_struct _temp730; _temp730.tag= Cyc_Std_String_pa;
_temp730.f1=( struct _tagged_arr) d;{ void* _temp729[ 1u]={& _temp730}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n", sizeof(
unsigned char), 38u), _tag_arr( _temp729, sizeof( void*), 1u));}}); return 1;}}
else{ close( _temp728);} return 0;} int Cyc_force_directory_prefixes( struct
_tagged_arr file){ struct _tagged_arr _temp732= Cyc_Std_strdup( file); struct
Cyc_List_List* _temp733= 0; while( 1) { _temp732= Cyc_Filename_dirname(( struct
_tagged_arr) _temp732); if( _get_arr_size( _temp732, sizeof( unsigned char)) == 
0){ break;} _temp733=({ struct Cyc_List_List* _temp734=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp734->hd=( void*)({ struct
_tagged_arr* _temp735=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp735[ 0]=( struct _tagged_arr) _temp732; _temp735;});
_temp734->tl= _temp733; _temp734;});} for( 0; _temp733 !=  0; _temp733= _temp733->tl){
if( Cyc_force_directory(*(( struct _tagged_arr*) _temp733->hd))){ return 1;}}
return 0;} int Cyc_process_file( struct _tagged_arr filename, struct _tagged_arr
dir, struct Cyc_List_List* start_symbols, struct Cyc_List_List* omit_symbols,
struct Cyc_List_List* prologue, struct Cyc_List_List* epilogue, struct Cyc_List_List*
cstubs, struct Cyc_List_List* cycstubs){ struct Cyc_Std___sFILE* maybe; struct
Cyc_Std___sFILE* in_file; struct Cyc_Std___sFILE* out_file;({ struct Cyc_Std_String_pa_struct
_temp737; _temp737.tag= Cyc_Std_String_pa; _temp737.f1=( struct _tagged_arr)
filename;{ void* _temp736[ 1u]={& _temp737}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("********************************* %s...\n", sizeof( unsigned char), 41u),
_tag_arr( _temp736, sizeof( void*), 1u));}});{ struct _tagged_arr buf= _tag_arr(({
unsigned int _temp1039=( unsigned int) 1024; unsigned char* _temp1040=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char),
_temp1039));{ unsigned int _temp1041= _temp1039; unsigned int i; for( i= 0; i < 
_temp1041; i ++){ _temp1040[ i]='\000';}}; _temp1040;}), sizeof( unsigned char),(
unsigned int) 1024); struct _tagged_arr _temp738= Cyc_Std_getcwd( buf,
_get_arr_size( buf, sizeof( unsigned char))); if( Cyc_Std_chdir( dir)){({ struct
Cyc_Std_String_pa_struct _temp740; _temp740.tag= Cyc_Std_String_pa; _temp740.f1=(
struct _tagged_arr) dir;{ void* _temp739[ 1u]={& _temp740}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: can't change directory to %s\n", sizeof( unsigned char), 37u),
_tag_arr( _temp739, sizeof( void*), 1u));}}); return 1;}({ struct Cyc_Std_String_pa_struct
_temp742; _temp742.tag= Cyc_Std_String_pa; _temp742.f1=( struct _tagged_arr)
filename;{ void* _temp741[ 1u]={& _temp742}; Cyc_log( _tag_arr("\n%s:\n",
sizeof( unsigned char), 6u), _tag_arr( _temp741, sizeof( void*), 1u));}});{
struct _tagged_arr _temp743= Cyc_Filename_basename( filename); struct
_tagged_arr _temp744= Cyc_Filename_chop_extension(( struct _tagged_arr) _temp743);
struct _tagged_arr _temp745= Cyc_Std_strconcat(( struct _tagged_arr) _temp744,
_tag_arr(".i", sizeof( unsigned char), 3u)); struct _tagged_arr _temp746= Cyc_Std_strconcat((
struct _tagged_arr) _temp744, _tag_arr(".iX", sizeof( unsigned char), 4u));
struct _tagged_arr _temp747= Cyc_Filename_dirname( filename); struct _tagged_arr
_temp748= _get_arr_size( _temp747, sizeof( unsigned char)) ==  0?({ struct Cyc_Std_String_pa_struct
_temp1036; _temp1036.tag= Cyc_Std_String_pa; _temp1036.f1=( struct _tagged_arr)
_temp743;{ void* _temp1035[ 1u]={& _temp1036}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp1035, sizeof( void*), 1u));}}): Cyc_Filename_concat((
struct _tagged_arr) _temp747,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1038; _temp1038.tag= Cyc_Std_String_pa; _temp1038.f1=( struct _tagged_arr)
_temp743;{ void* _temp1037[ 1u]={& _temp1038}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp1037, sizeof( void*), 1u));}})); if(
Cyc_force_directory_prefixes( filename)){ return 1;}{ struct Cyc_Hashtable_Table*
t= Cyc_new_deps(); struct _tagged_arr _temp749=({ struct Cyc_Std_String_pa_struct
_temp1034; _temp1034.tag= Cyc_Std_String_pa; _temp1034.f1=( struct _tagged_arr)
_temp745;{ struct Cyc_Std_String_pa_struct _temp1033; _temp1033.tag= Cyc_Std_String_pa;
_temp1033.f1=( struct _tagged_arr) filename;{ void* _temp1032[ 2u]={& _temp1033,&
_temp1034}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp1032, sizeof( void*), 2u));}}});
if( Cyc_Std_system(( struct _tagged_arr) _temp749) !=  0){ maybe= Cyc_Std_fopen(
filename, _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int)
maybe)){({ struct Cyc_Std_String_pa_struct _temp751; _temp751.tag= Cyc_Std_String_pa;
_temp751.f1=( struct _tagged_arr) filename;{ void* _temp750[ 1u]={& _temp751};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create file %s\n",
sizeof( unsigned char), 33u), _tag_arr( _temp750, sizeof( void*), 1u));}});
return 1;} out_file=( struct Cyc_Std___sFILE*) _check_null( maybe);({ struct Cyc_Std_String_pa_struct
_temp753; _temp753.tag= Cyc_Std_String_pa; _temp753.f1=( struct _tagged_arr)
filename;{ void* _temp752[ 1u]={& _temp753}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp752, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp748, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){({ struct Cyc_Std_String_pa_struct
_temp755; _temp755.tag= Cyc_Std_String_pa; _temp755.f1=( struct _tagged_arr)
_temp748;{ void* _temp754[ 1u]={& _temp755}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n", sizeof( unsigned char), 33u),
_tag_arr( _temp754, sizeof( void*), 1u));}}); return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp757; _temp757.tag=
Cyc_Std_String_pa; _temp757.f1=( struct _tagged_arr) _temp748;{ void* _temp756[
1u]={& _temp757}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp756, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file);({ struct Cyc_Std_String_pa_struct _temp759; _temp759.tag= Cyc_Std_String_pa;
_temp759.f1=( struct _tagged_arr) filename;{ void* _temp758[ 1u]={& _temp759};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: %s will not be supported on this platform\n",
sizeof( unsigned char), 52u), _tag_arr( _temp758, sizeof( void*), 1u));}});({
void* _temp760[ 0u]={}; Cyc_log( _tag_arr("Not supported on this platform\n",
sizeof( unsigned char), 32u), _tag_arr( _temp760, sizeof( void*), 0u));}); if(
Cyc_Std_chdir(( struct _tagged_arr) _temp738)){({ struct Cyc_Std_String_pa_struct
_temp762; _temp762.tag= Cyc_Std_String_pa; _temp762.f1=( struct _tagged_arr)
_temp738;{ void* _temp761[ 1u]={& _temp762}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n", sizeof( unsigned char), 41u),
_tag_arr( _temp761, sizeof( void*), 1u));}}); return 1;} return 0;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp745, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe);{ struct Cyc_Lexing_lexbuf* _temp763= Cyc_Lexing_from_file( in_file);
struct _tuple8* entry; while(( entry=(( struct _tuple8*(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_line)( _temp763)) !=  0) { struct _tuple8 _temp766; struct Cyc_Set_Set*
_temp767; struct _tagged_arr* _temp769; struct _tuple8* _temp764=( struct
_tuple8*) _check_null( entry); _temp766=* _temp764; _LL770: _temp769= _temp766.f1;
goto _LL768; _LL768: _temp767= _temp766.f2; goto _LL765; _LL765:(( void(*)(
struct Cyc_Hashtable_Table* t, struct _tagged_arr* key, struct Cyc_Set_Set* val))
Cyc_Hashtable_insert)( t, _temp769, _temp767);} Cyc_Std_fclose( in_file);
_temp749=({ struct Cyc_Std_String_pa_struct _temp773; _temp773.tag= Cyc_Std_String_pa;
_temp773.f1=( struct _tagged_arr) _temp745;{ struct Cyc_Std_String_pa_struct
_temp772; _temp772.tag= Cyc_Std_String_pa; _temp772.f1=( struct _tagged_arr)
filename;{ void* _temp771[ 2u]={& _temp772,& _temp773}; Cyc_Std_aprintf(
_tag_arr("echo '#include <%s>' | gcc -E -o %s -", sizeof( unsigned char), 38u),
_tag_arr( _temp771, sizeof( void*), 2u));}}}); if( Cyc_Std_system(( struct
_tagged_arr) _temp749) !=  0){ return 1;} maybe= Cyc_Std_fopen(( struct
_tagged_arr) _temp745, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp763= Cyc_Lexing_from_file( in_file); Cyc_slurp_out= Cyc_Std_fopen((
struct _tagged_arr) _temp746, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) Cyc_slurp_out)){ return 1;} if((( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_slurp)( _temp763)){({ struct Cyc_Std_String_pa_struct _temp775;
_temp775.tag= Cyc_Std_String_pa; _temp775.f1=( struct _tagged_arr) filename;{
void* _temp774[ 1u]={& _temp775}; Cyc_log( _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp774, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp777; _temp777.tag= Cyc_Std_String_pa;
_temp777.f1=( struct _tagged_arr) filename;{ void* _temp776[ 1u]={& _temp777};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: Unclosed string in %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp776, sizeof( void*), 1u));}}); Cyc_Std_fclose(
in_file); Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
Cyc_Std_remove(( struct _tagged_arr) _temp745); Cyc_Std_remove(( struct
_tagged_arr) _temp746); return 1;} Cyc_Std_fclose( in_file); Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp746, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); Cyc_Position_reset_position(( struct _tagged_arr) _temp746); Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp778= Cyc_Parse_parse_file( in_file); Cyc_Lex_lex_init();
Cyc_Std_fclose( in_file);{ struct Cyc_List_List* _temp779= _temp778; for( 0;
_temp779 !=  0; _temp779= _temp779->tl){ struct _tuple8* _temp780= Cyc_scan_decl((
struct Cyc_Absyn_Decl*) _temp779->hd); if( _temp780 ==  0){ continue;}{ struct
_tuple8 _temp783; struct Cyc_Set_Set* _temp784; struct _tagged_arr* _temp786;
struct _tuple8* _temp781=( struct _tuple8*) _check_null( _temp780); _temp783=*
_temp781; _LL787: _temp786= _temp783.f1; goto _LL785; _LL785: _temp784= _temp783.f2;
goto _LL782; _LL782: { struct Cyc_Set_Set* old;{ struct _handler_cons _temp788;
_push_handler(& _temp788);{ int _temp790= 0; if( setjmp( _temp788.handler)){
_temp790= 1;} if( ! _temp790){ old=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_arr* key)) Cyc_Hashtable_lookup)( t, _temp786);; _pop_handler();}
else{ void* _temp789=( void*) _exn_thrown; void* _temp792= _temp789; _LL794: if(
_temp792 ==  Cyc_Core_Not_found){ goto _LL795;} else{ goto _LL796;} _LL796: goto
_LL797; _LL795: old=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); goto _LL793; _LL797:(
void) _throw( _temp792); _LL793:;}}}(( void(*)( struct Cyc_Hashtable_Table* t,
struct _tagged_arr* key, struct Cyc_Set_Set* val)) Cyc_Hashtable_insert)( t,
_temp786,(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( _temp784, old));}}}}{ struct Cyc_Set_Set* _temp798= Cyc_reachable(
start_symbols, t); struct Cyc_List_List* _temp799= 0; struct Cyc_Set_Set*
defined_symbols=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);{ struct Cyc_List_List*
_temp800= _temp778; for( 0; _temp800 !=  0; _temp800= _temp800->tl){ struct Cyc_Absyn_Decl*
_temp801=( struct Cyc_Absyn_Decl*) _temp800->hd; struct _tagged_arr* name;{ void*
_temp802=( void*) _temp801->r; struct Cyc_Absyn_Vardecl* _temp828; struct Cyc_Absyn_Fndecl*
_temp830; struct Cyc_Absyn_Structdecl* _temp832; struct Cyc_Absyn_Uniondecl*
_temp834; struct Cyc_Absyn_Enumdecl* _temp836; struct Cyc_Absyn_Typedefdecl*
_temp838; _LL804: if(*(( int*) _temp802) ==  Cyc_Absyn_Var_d){ _LL829: _temp828=((
struct Cyc_Absyn_Var_d_struct*) _temp802)->f1; goto _LL805;} else{ goto _LL806;}
_LL806: if(*(( int*) _temp802) ==  Cyc_Absyn_Fn_d){ _LL831: _temp830=(( struct
Cyc_Absyn_Fn_d_struct*) _temp802)->f1; goto _LL807;} else{ goto _LL808;} _LL808:
if(*(( int*) _temp802) ==  Cyc_Absyn_Struct_d){ _LL833: _temp832=(( struct Cyc_Absyn_Struct_d_struct*)
_temp802)->f1; goto _LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp802)
==  Cyc_Absyn_Union_d){ _LL835: _temp834=(( struct Cyc_Absyn_Union_d_struct*)
_temp802)->f1; goto _LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp802)
==  Cyc_Absyn_Enum_d){ _LL837: _temp836=(( struct Cyc_Absyn_Enum_d_struct*)
_temp802)->f1; goto _LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp802)
==  Cyc_Absyn_Typedef_d){ _LL839: _temp838=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp802)->f1; goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp802)
==  Cyc_Absyn_Let_d){ goto _LL817;} else{ goto _LL818;} _LL818: if(*(( int*)
_temp802) ==  Cyc_Absyn_Tunion_d){ goto _LL819;} else{ goto _LL820;} _LL820: if(*((
int*) _temp802) ==  Cyc_Absyn_Letv_d){ goto _LL821;} else{ goto _LL822;} _LL822:
if(*(( int*) _temp802) ==  Cyc_Absyn_Namespace_d){ goto _LL823;} else{ goto
_LL824;} _LL824: if(*(( int*) _temp802) ==  Cyc_Absyn_Using_d){ goto _LL825;}
else{ goto _LL826;} _LL826: if(*(( int*) _temp802) ==  Cyc_Absyn_ExternC_d){
goto _LL827;} else{ goto _LL803;} _LL805: { struct _tuple0 _temp842; struct
_tagged_arr* _temp843; struct _tuple0* _temp840= _temp828->name; _temp842=*
_temp840; _LL844: _temp843= _temp842.f2; goto _LL841; _LL841: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp843); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp843)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp843;} goto _LL803;} _LL807: { struct _tuple0 _temp847; struct
_tagged_arr* _temp848; struct _tuple0* _temp845= _temp830->name; _temp847=*
_temp845; _LL849: _temp848= _temp847.f2; goto _LL846; _LL846: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp848); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp848)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp848;} goto _LL803;} _LL809: if( !(( unsigned int) _temp832->name)){
name= 0;}{ struct _tuple0 _temp852; struct _tagged_arr* _temp853; struct _tuple0*
_temp850=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp832->name))->v;
_temp852=* _temp850; _LL854: _temp853= _temp852.f2; goto _LL851; _LL851: name=(
struct _tagged_arr*) _temp853; goto _LL803;} _LL811: if( !(( unsigned int)
_temp834->name)){ name= 0;}{ struct _tuple0 _temp857; struct _tagged_arr*
_temp858; struct _tuple0* _temp855=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp834->name))->v; _temp857=* _temp855; _LL859: _temp858=
_temp857.f2; goto _LL856; _LL856: name=( struct _tagged_arr*) _temp858; goto
_LL803;} _LL813: { struct _tuple0 _temp862; struct _tagged_arr* _temp863; struct
_tuple0* _temp860= _temp836->name; _temp862=* _temp860; _LL864: _temp863=
_temp862.f2; goto _LL861; _LL861: name=( struct _tagged_arr*) _temp863; if( name
!=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp798,( struct _tagged_arr*) _check_null( name)): 0){ _temp799=({ struct Cyc_List_List*
_temp865=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp865->hd=( void*) _temp801; _temp865->tl= _temp799; _temp865;});} else{ if((
unsigned int) _temp836->fields){ struct Cyc_List_List* _temp866=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp836->fields))->v; for( 0; _temp866 !=  0;
_temp866= _temp866->tl){ struct Cyc_Absyn_Enumfield* _temp867=( struct Cyc_Absyn_Enumfield*)
_temp866->hd; struct _tuple0 _temp870; struct _tagged_arr* _temp871; struct
_tuple0* _temp868= _temp867->name; _temp870=* _temp868; _LL872: _temp871=
_temp870.f2; goto _LL869; _LL869: if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( _temp798, _temp871)){ _temp799=({ struct Cyc_List_List*
_temp873=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp873->hd=( void*) _temp801; _temp873->tl= _temp799; _temp873;}); break;}}}}
name= 0; goto _LL803;} _LL815: { struct _tuple0 _temp876; struct _tagged_arr*
_temp877; struct _tuple0* _temp874= _temp838->name; _temp876=* _temp874; _LL878:
_temp877= _temp876.f2; goto _LL875; _LL875: name=( struct _tagged_arr*) _temp877;
goto _LL803;} _LL817: goto _LL819; _LL819: goto _LL821; _LL821: goto _LL823;
_LL823: goto _LL825; _LL825: goto _LL827; _LL827: name= 0; goto _LL803; _LL803:;}
if( name !=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp798,( struct _tagged_arr*) _check_null( name)): 0){ _temp799=({ struct Cyc_List_List*
_temp879=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp879->hd=( void*) _temp801; _temp879->tl= _temp799; _temp879;});}}} _temp799=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp799);
maybe= Cyc_Std_fopen( filename, _tag_arr("w", sizeof( unsigned char), 2u)); if(
!(( unsigned int) maybe)){ return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp881; _temp881.tag=
Cyc_Std_String_pa; _temp881.f1=( struct _tagged_arr) _temp748;{ void* _temp880[
1u]={& _temp881}; Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\nusing Std;\n",
sizeof( unsigned char), 26u), _tag_arr( _temp880, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp748, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} out_file=(
struct Cyc_Std___sFILE*) _check_null( maybe);{ struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _temp1031; _temp1031.tag= Cyc_Std_String_pa;
_temp1031.f1=( struct _tagged_arr) filename;{ void* _temp1030[ 1u]={& _temp1031};
Cyc_Std_aprintf( _tag_arr("_%s_", sizeof( unsigned char), 5u), _tag_arr(
_temp1030, sizeof( void*), 1u));}});{ int _temp882= 0; for( 0; _temp882 < 
_get_arr_size( ifdefmacro, sizeof( unsigned char)); _temp882 ++){ if(*((
unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof( unsigned char),
_temp882)) == '.'? 1:*(( unsigned char*) _check_unknown_subscript( ifdefmacro,
sizeof( unsigned char), _temp882)) == '/'){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp882))='_';}
else{ if(*(( unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof(
unsigned char), _temp882)) != '_'?*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp882)) != '/': 0){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp882))=(
unsigned char) toupper(( int)*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp882)));}}}}({ struct Cyc_Std_String_pa_struct
_temp885; _temp885.tag= Cyc_Std_String_pa; _temp885.f1=( struct _tagged_arr)
ifdefmacro;{ struct Cyc_Std_String_pa_struct _temp884; _temp884.tag= Cyc_Std_String_pa;
_temp884.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp883[ 2u]={& _temp884,&
_temp885}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n#define %s\n",
sizeof( unsigned char), 23u), _tag_arr( _temp883, sizeof( void*), 2u));}}}); if(
prologue !=  0){ struct Cyc_List_List* _temp886= prologue; for( 0; _temp886 != 
0; _temp886= _temp886->tl){ struct _tuple10 _temp889; struct _tagged_arr
_temp890; struct _tagged_arr _temp892; struct _tuple10* _temp887=( struct
_tuple10*) _temp886->hd; _temp889=* _temp887; _LL893: _temp892= _temp889.f1;
goto _LL891; _LL891: _temp890= _temp889.f2; goto _LL888; _LL888: if( _temp890.curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp892.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp894=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp894[ 0]= _temp892; _temp894;})): 0){ Cyc_Std_fputs( _temp890,
out_file);} else{({ struct Cyc_Std_String_pa_struct _temp896; _temp896.tag= Cyc_Std_String_pa;
_temp896.f1=( struct _tagged_arr) _temp892;{ void* _temp895[ 1u]={& _temp896};
Cyc_log( _tag_arr("%s is not supported on this platform\n", sizeof(
unsigned char), 38u), _tag_arr( _temp895, sizeof( void*), 1u));}});}}}({ void*
_temp897[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("\nnamespace Std {\n",
sizeof( unsigned char), 18u), _tag_arr( _temp897, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp898= _temp799; for( 0; _temp898 !=  0; _temp898=
_temp898->tl){ struct Cyc_Absyn_Decl* _temp899=( struct Cyc_Absyn_Decl*)
_temp898->hd; int _temp900= 0; struct _tagged_arr* name;{ void* _temp901=( void*)
_temp899->r; struct Cyc_Absyn_Vardecl* _temp927; struct Cyc_Absyn_Fndecl*
_temp929; struct Cyc_Absyn_Structdecl* _temp931; struct Cyc_Absyn_Uniondecl*
_temp933; struct Cyc_Absyn_Enumdecl* _temp935; struct Cyc_Absyn_Typedefdecl*
_temp937; _LL903: if(*(( int*) _temp901) ==  Cyc_Absyn_Var_d){ _LL928: _temp927=((
struct Cyc_Absyn_Var_d_struct*) _temp901)->f1; goto _LL904;} else{ goto _LL905;}
_LL905: if(*(( int*) _temp901) ==  Cyc_Absyn_Fn_d){ _LL930: _temp929=(( struct
Cyc_Absyn_Fn_d_struct*) _temp901)->f1; goto _LL906;} else{ goto _LL907;} _LL907:
if(*(( int*) _temp901) ==  Cyc_Absyn_Struct_d){ _LL932: _temp931=(( struct Cyc_Absyn_Struct_d_struct*)
_temp901)->f1; goto _LL908;} else{ goto _LL909;} _LL909: if(*(( int*) _temp901)
==  Cyc_Absyn_Union_d){ _LL934: _temp933=(( struct Cyc_Absyn_Union_d_struct*)
_temp901)->f1; goto _LL910;} else{ goto _LL911;} _LL911: if(*(( int*) _temp901)
==  Cyc_Absyn_Enum_d){ _LL936: _temp935=(( struct Cyc_Absyn_Enum_d_struct*)
_temp901)->f1; goto _LL912;} else{ goto _LL913;} _LL913: if(*(( int*) _temp901)
==  Cyc_Absyn_Typedef_d){ _LL938: _temp937=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp901)->f1; goto _LL914;} else{ goto _LL915;} _LL915: if(*(( int*) _temp901)
==  Cyc_Absyn_Let_d){ goto _LL916;} else{ goto _LL917;} _LL917: if(*(( int*)
_temp901) ==  Cyc_Absyn_Tunion_d){ goto _LL918;} else{ goto _LL919;} _LL919: if(*((
int*) _temp901) ==  Cyc_Absyn_Letv_d){ goto _LL920;} else{ goto _LL921;} _LL921:
if(*(( int*) _temp901) ==  Cyc_Absyn_Namespace_d){ goto _LL922;} else{ goto
_LL923;} _LL923: if(*(( int*) _temp901) ==  Cyc_Absyn_Using_d){ goto _LL924;}
else{ goto _LL925;} _LL925: if(*(( int*) _temp901) ==  Cyc_Absyn_ExternC_d){
goto _LL926;} else{ goto _LL902;} _LL904: { struct _tuple0 _temp941; struct
_tagged_arr* _temp942; struct _tuple0* _temp939= _temp927->name; _temp941=*
_temp939; _LL943: _temp942= _temp941.f2; goto _LL940; _LL940: name=( struct
_tagged_arr*) _temp942; goto _LL902;} _LL906: { struct _tuple0 _temp946; struct
_tagged_arr* _temp947; struct _tuple0* _temp944= _temp929->name; _temp946=*
_temp944; _LL948: _temp947= _temp946.f2; goto _LL945; _LL945: name=( struct
_tagged_arr*) _temp947; goto _LL902;} _LL908: if( !(( unsigned int) _temp931->name)){
name= 0;}{ struct _tuple0 _temp951; struct _tagged_arr* _temp952; struct _tuple0*
_temp949=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp931->name))->v;
_temp951=* _temp949; _LL953: _temp952= _temp951.f2; goto _LL950; _LL950: name=(
struct _tagged_arr*) _temp952; goto _LL902;} _LL910: if( !(( unsigned int)
_temp933->name)){ name= 0;}{ struct _tuple0 _temp956; struct _tagged_arr*
_temp957; struct _tuple0* _temp954=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp933->name))->v; _temp956=* _temp954; _LL958: _temp957=
_temp956.f2; goto _LL955; _LL955: name=( struct _tagged_arr*) _temp957; goto
_LL902;} _LL912: { struct _tuple0 _temp961; struct _tagged_arr* _temp962; struct
_tuple0* _temp959= _temp935->name; _temp961=* _temp959; _LL963: _temp962=
_temp961.f2; goto _LL960; _LL960: name=( struct _tagged_arr*) _temp962; goto
_LL902;} _LL914: { struct _tuple0 _temp966; struct _tagged_arr* _temp967; struct
_tuple0* _temp964= _temp937->name; _temp966=* _temp964; _LL968: _temp967=
_temp966.f2; goto _LL965; _LL965: name=( struct _tagged_arr*) _temp967; goto
_LL902;} _LL916: goto _LL918; _LL918: goto _LL920; _LL920: goto _LL922; _LL922:
goto _LL924; _LL924: goto _LL926; _LL926: name= 0; goto _LL902; _LL902:;} if( !((
unsigned int) name)? ! _temp900: 0){ continue;} if(( unsigned int) name){
ifdefmacro=({ struct Cyc_Std_String_pa_struct _temp970; _temp970.tag= Cyc_Std_String_pa;
_temp970.f1=( struct _tagged_arr)* name;{ void* _temp969[ 1u]={& _temp970}; Cyc_Std_aprintf(
_tag_arr("_%s_def_", sizeof( unsigned char), 9u), _tag_arr( _temp969, sizeof(
void*), 1u));}});({ struct Cyc_Std_String_pa_struct _temp972; _temp972.tag= Cyc_Std_String_pa;
_temp972.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp971[ 1u]={& _temp972};
Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof( unsigned char), 12u),
_tag_arr( _temp971, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp974; _temp974.tag= Cyc_Std_String_pa; _temp974.f1=( struct _tagged_arr)
ifdefmacro;{ void* _temp973[ 1u]={& _temp974}; Cyc_Std_fprintf( out_file,
_tag_arr("#define %s\n", sizeof( unsigned char), 12u), _tag_arr( _temp973,
sizeof( void*), 1u));}}); Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp975[ 1u]; _temp975[ 0u]= _temp899;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp975, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);({ void* _temp976[ 0u]={}; Cyc_Std_fprintf( out_file,
_tag_arr("#endif\n", sizeof( unsigned char), 8u), _tag_arr( _temp976, sizeof(
void*), 0u));});} else{ Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp977[ 1u]; _temp977[ 0u]= _temp899;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp977, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);}}} _temp749=({ struct Cyc_Std_String_pa_struct _temp980;
_temp980.tag= Cyc_Std_String_pa; _temp980.f1=( struct _tagged_arr) _temp745;{
struct Cyc_Std_String_pa_struct _temp979; _temp979.tag= Cyc_Std_String_pa;
_temp979.f1=( struct _tagged_arr) filename;{ void* _temp978[ 2u]={& _temp979,&
_temp980}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp978, sizeof( void*), 2u));}}}); if(
Cyc_Std_system(( struct _tagged_arr) _temp749) !=  0){ return 1;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp745, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp763= Cyc_Lexing_from_file( in_file);{ struct _tuple9* entry2;
while(( entry2=(( struct _tuple9*(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_suck_line)(
_temp763)) !=  0) { struct _tuple9 _temp983; struct _tagged_arr* _temp984;
struct _tagged_arr _temp986; struct _tuple9* _temp981=( struct _tuple9*)
_check_null( entry2); _temp983=* _temp981; _LL987: _temp986= _temp983.f1; goto
_LL985; _LL985: _temp984= _temp983.f2; goto _LL982; _LL982: if((( int(*)( struct
Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( _temp798, _temp984)){({
struct Cyc_Std_String_pa_struct _temp989; _temp989.tag= Cyc_Std_String_pa;
_temp989.f1=( struct _tagged_arr)* _temp984;{ void* _temp988[ 1u]={& _temp989};
Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof( unsigned char), 12u),
_tag_arr( _temp988, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp991; _temp991.tag= Cyc_Std_String_pa; _temp991.f1=( struct _tagged_arr)
_temp986;{ void* _temp990[ 1u]={& _temp991}; Cyc_Std_fprintf( out_file, _tag_arr("%s\n",
sizeof( unsigned char), 4u), _tag_arr( _temp990, sizeof( void*), 1u));}});({
void* _temp992[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp992, sizeof( void*), 0u));});}} Cyc_Std_fclose(
in_file); Cyc_Std_remove(( struct _tagged_arr) _temp745); if( epilogue !=  0){
struct Cyc_List_List* _temp993= epilogue; for( 0; _temp993 !=  0; _temp993=
_temp993->tl){ struct _tuple10 _temp996; struct _tagged_arr _temp997; struct
_tagged_arr _temp999; struct _tuple10* _temp994=( struct _tuple10*) _temp993->hd;
_temp996=* _temp994; _LL1000: _temp999= _temp996.f1; goto _LL998; _LL998:
_temp997= _temp996.f2; goto _LL995; _LL995: if( _temp997.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp999.curr == (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct _tagged_arr*
_temp1001=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp1001[ 0]= _temp999; _temp1001;})): 0){ Cyc_Std_fputs( _temp997, out_file);}
else{({ struct Cyc_Std_String_pa_struct _temp1003; _temp1003.tag= Cyc_Std_String_pa;
_temp1003.f1=( struct _tagged_arr) _temp999;{ void* _temp1002[ 1u]={& _temp1003};
Cyc_log( _tag_arr("%s is not supported on this platform\n", sizeof(
unsigned char), 38u), _tag_arr( _temp1002, sizeof( void*), 1u));}});}}}({ void*
_temp1004[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n", sizeof(
unsigned char), 3u), _tag_arr( _temp1004, sizeof( void*), 0u));});({ void*
_temp1005[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp1005, sizeof( void*), 0u));}); Cyc_Std_fclose(
out_file); if( cstubs !=  0){ out_file=( struct Cyc_Std___sFILE*) _check_null(
Cyc_cstubs_file);{ struct Cyc_List_List* _temp1006= cstubs; for( 0; _temp1006 != 
0; _temp1006= _temp1006->tl){ struct _tuple10 _temp1009; struct _tagged_arr
_temp1010; struct _tagged_arr _temp1012; struct _tuple10* _temp1007=( struct
_tuple10*) _temp1006->hd; _temp1009=* _temp1007; _LL1013: _temp1012= _temp1009.f1;
goto _LL1011; _LL1011: _temp1010= _temp1009.f2; goto _LL1008; _LL1008: if(
_temp1010.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1012.curr
== (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp1014=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1014[ 0]= _temp1012; _temp1014;})): 0){ Cyc_Std_fputs(
_temp1010, out_file);}}}} out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _temp1016; _temp1016.tag= Cyc_Std_String_pa;
_temp1016.f1=( struct _tagged_arr) _temp748;{ void* _temp1015[ 1u]={& _temp1016};
Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\n\n", sizeof( unsigned char),
16u), _tag_arr( _temp1015, sizeof( void*), 1u));}}); if( cycstubs !=  0){
out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({ void*
_temp1017[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("namespace Std {\n",
sizeof( unsigned char), 17u), _tag_arr( _temp1017, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp1018= cycstubs; for( 0; _temp1018 !=  0; _temp1018=
_temp1018->tl){ struct _tuple10 _temp1021; struct _tagged_arr _temp1022; struct
_tagged_arr _temp1024; struct _tuple10* _temp1019=( struct _tuple10*) _temp1018->hd;
_temp1021=* _temp1019; _LL1025: _temp1024= _temp1021.f1; goto _LL1023; _LL1023:
_temp1022= _temp1021.f2; goto _LL1020; _LL1020: if( _temp1022.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp1024.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct _tagged_arr*
_temp1026=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp1026[ 0]= _temp1024; _temp1026;})): 0){ Cyc_Std_fputs( _temp1022, out_file);}}}({
void* _temp1027[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n\n", sizeof(
unsigned char), 4u), _tag_arr( _temp1027, sizeof( void*), 0u));});} if( Cyc_Std_chdir((
struct _tagged_arr) _temp738)){({ struct Cyc_Std_String_pa_struct _temp1029;
_temp1029.tag= Cyc_Std_String_pa; _temp1029.f1=( struct _tagged_arr) _temp738;{
void* _temp1028[ 1u]={& _temp1029}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not change directory to %s\n",
sizeof( unsigned char), 41u), _tag_arr( _temp1028, sizeof( void*), 1u));}});
return 1;} return 0;}}}}}}}}} int Cyc_process_specfile( struct _tagged_arr file,
struct _tagged_arr dir){ struct Cyc_Std___sFILE* _temp1042= Cyc_Std_fopen( file,
_tag_arr("r", sizeof( unsigned char), 2u)); if( !(( unsigned int) _temp1042)){
return 1;}{ struct Cyc_Std___sFILE* _temp1043=( struct Cyc_Std___sFILE*)
_check_null( _temp1042); struct Cyc_Lexing_lexbuf* _temp1044= Cyc_Lexing_from_file(
_temp1043); struct _tuple11* entry; while(( entry=(( struct _tuple11*(*)( struct
Cyc_Lexing_lexbuf* lexbuf)) Cyc_spec)( _temp1044)) !=  0) { struct _tuple11
_temp1047; struct Cyc_List_List* _temp1048; struct Cyc_List_List* _temp1050;
struct Cyc_List_List* _temp1052; struct Cyc_List_List* _temp1054; struct Cyc_List_List*
_temp1056; struct Cyc_List_List* _temp1058; struct _tagged_arr _temp1060; struct
_tuple11* _temp1045=( struct _tuple11*) _check_null( entry); _temp1047=*
_temp1045; _LL1061: _temp1060= _temp1047.f1; goto _LL1059; _LL1059: _temp1058=
_temp1047.f2; goto _LL1057; _LL1057: _temp1056= _temp1047.f3; goto _LL1055;
_LL1055: _temp1054= _temp1047.f4; goto _LL1053; _LL1053: _temp1052= _temp1047.f5;
goto _LL1051; _LL1051: _temp1050= _temp1047.f6; goto _LL1049; _LL1049: _temp1048=
_temp1047.f7; goto _LL1046; _LL1046: if( Cyc_process_file( _temp1060, dir,
_temp1058, _temp1056, _temp1054, _temp1052, _temp1050, _temp1048)){ Cyc_Std_fclose(
_temp1043); return 1;}} Cyc_Std_fclose( _temp1043); return 0;}} int Cyc_getsize(
struct _tagged_arr dir, struct _tagged_arr includes, struct _tagged_arr type){
struct _tagged_arr _temp1062= Cyc_Filename_concat( dir, _tag_arr("getsize.c",
sizeof( unsigned char), 10u)); struct _tagged_arr _temp1063= Cyc_Filename_concat(
dir, _tag_arr("getsize", sizeof( unsigned char), 8u)); struct _tagged_arr
_temp1064= Cyc_Filename_concat( dir, _tag_arr("getsize.out", sizeof(
unsigned char), 12u)); struct Cyc_Std___sFILE* _temp1065= Cyc_Std_fopen(( struct
_tagged_arr) _temp1062, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) _temp1065)){({ struct Cyc_Std_String_pa_struct _temp1067;
_temp1067.tag= Cyc_Std_String_pa; _temp1067.f1=( struct _tagged_arr) _temp1062;{
void* _temp1066[ 1u]={& _temp1067}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create %s\n",
sizeof( unsigned char), 28u), _tag_arr( _temp1066, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1069; _temp1069.tag= Cyc_Std_String_pa;
_temp1069.f1=( struct _tagged_arr) type;{ void* _temp1068[ 1u]={& _temp1069};
Cyc_log( _tag_arr("Warning: could not find the size of %s\n", sizeof(
unsigned char), 40u), _tag_arr( _temp1068, sizeof( void*), 1u));}}); return - 1;}({
struct Cyc_Std_String_pa_struct _temp1074; _temp1074.tag= Cyc_Std_String_pa;
_temp1074.f1=( struct _tagged_arr) type;{ struct Cyc_Std_String_pa_struct
_temp1073; _temp1073.tag= Cyc_Std_String_pa; _temp1073.f1=( struct _tagged_arr)
type;{ struct Cyc_Std_String_pa_struct _temp1072; _temp1072.tag= Cyc_Std_String_pa;
_temp1072.f1=( struct _tagged_arr) _temp1064;{ struct Cyc_Std_String_pa_struct
_temp1071; _temp1071.tag= Cyc_Std_String_pa; _temp1071.f1=( struct _tagged_arr)
includes;{ void* _temp1070[ 4u]={& _temp1071,& _temp1072,& _temp1073,& _temp1074};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( _temp1065), _tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof( unsigned char), 139u), _tag_arr( _temp1070, sizeof( void*), 4u));}}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1065));{ struct
_tagged_arr _temp1075=({ struct Cyc_Std_String_pa_struct _temp1095; _temp1095.tag=
Cyc_Std_String_pa; _temp1095.f1=( struct _tagged_arr) _temp1062;{ struct Cyc_Std_String_pa_struct
_temp1094; _temp1094.tag= Cyc_Std_String_pa; _temp1094.f1=( struct _tagged_arr)
_temp1063;{ void* _temp1093[ 2u]={& _temp1094,& _temp1095}; Cyc_Std_aprintf(
_tag_arr("gcc -o %s %s", sizeof( unsigned char), 13u), _tag_arr( _temp1093,
sizeof( void*), 2u));}}}); if( Cyc_Std_system(( struct _tagged_arr) _temp1075)
!=  0){({ struct Cyc_Std_String_pa_struct _temp1077; _temp1077.tag= Cyc_Std_String_pa;
_temp1077.f1=( struct _tagged_arr) type;{ void* _temp1076[ 1u]={& _temp1077};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof( unsigned char), 59u), _tag_arr( _temp1076, sizeof( void*), 1u));}});({
struct Cyc_Std_String_pa_struct _temp1079; _temp1079.tag= Cyc_Std_String_pa;
_temp1079.f1=( struct _tagged_arr) type;{ void* _temp1078[ 1u]={& _temp1079};
Cyc_log( _tag_arr("Warning: could not find the size of %s\n", sizeof(
unsigned char), 40u), _tag_arr( _temp1078, sizeof( void*), 1u));}}); return - 1;}{
int _temp1080= Cyc_Std_system(( struct _tagged_arr) _temp1063);({ struct Cyc_Std_Int_pa_struct
_temp1083; _temp1083.tag= Cyc_Std_Int_pa; _temp1083.f1=( int)(( unsigned int)
_temp1080);{ struct Cyc_Std_String_pa_struct _temp1082; _temp1082.tag= Cyc_Std_String_pa;
_temp1082.f1=( struct _tagged_arr) type;{ void* _temp1081[ 2u]={& _temp1082,&
_temp1083}; Cyc_log( _tag_arr("size of %s returned by system is %d\n", sizeof(
unsigned char), 37u), _tag_arr( _temp1081, sizeof( void*), 2u));}}});{ struct
Cyc_Std___sFILE* _temp1084= Cyc_Std_fopen(( struct _tagged_arr) _temp1064,
_tag_arr("r", sizeof( unsigned char), 2u)); int w= - 1; if(({ struct Cyc_Std_IntPtr_sa_struct
_temp1086; _temp1086.tag= Cyc_Std_IntPtr_sa; _temp1086.f1=& w;{ void* _temp1085[
1u]={& _temp1086}; Cyc_Std_fscanf(( struct Cyc_Std___sFILE*) _check_null(
_temp1084), _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1085,
sizeof( void*), 1u));}}) !=  1){({ struct Cyc_Std_String_pa_struct _temp1088;
_temp1088.tag= Cyc_Std_String_pa; _temp1088.f1=( struct _tagged_arr) type;{ void*
_temp1087[ 1u]={& _temp1088}; Cyc_log( _tag_arr("Warning: could not find the size of %s\n",
sizeof( unsigned char), 40u), _tag_arr( _temp1087, sizeof( void*), 1u));}});
return - 1;}({ struct Cyc_Std_Int_pa_struct _temp1092; _temp1092.tag= Cyc_Std_Int_pa;
_temp1092.f1=( int)(( unsigned int) w);{ struct Cyc_Std_String_pa_struct
_temp1091; _temp1091.tag= Cyc_Std_String_pa; _temp1091.f1=( struct _tagged_arr)
_temp1064;{ struct Cyc_Std_String_pa_struct _temp1090; _temp1090.tag= Cyc_Std_String_pa;
_temp1090.f1=( struct _tagged_arr) type;{ void* _temp1089[ 3u]={& _temp1090,&
_temp1091,& _temp1092}; Cyc_log( _tag_arr("size of %s read from file %s is %d\n",
sizeof( unsigned char), 36u), _tag_arr( _temp1089, sizeof( void*), 3u));}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1084)); Cyc_Std_remove((
struct _tagged_arr) _temp1064); Cyc_Std_remove(( struct _tagged_arr) _temp1062);
Cyc_Std_remove(( struct _tagged_arr) _temp1063); return w;}}}} int Cyc_main( int
argc, struct _tagged_arr argv){ if( argc <  3){({ void* _temp1096[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof( unsigned char), 105u), _tag_arr( _temp1096, sizeof( void*), 0u));});
return 1;}{ struct _tagged_arr _temp1097=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 1)); if( Cyc_force_directory_prefixes((
struct _tagged_arr) _temp1097)? 1: Cyc_force_directory(( struct _tagged_arr)
_temp1097)){({ struct Cyc_Std_String_pa_struct _temp1099; _temp1099.tag= Cyc_Std_String_pa;
_temp1099.f1=( struct _tagged_arr) _temp1097;{ void* _temp1098[ 1u]={& _temp1099};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1098, sizeof( void*), 1u));}});
return 1;} Cyc_log_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1097, _tag_arr("BUILDLIB.LOG", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _temp1101; _temp1101.tag= Cyc_Std_String_pa;
_temp1101.f1=( struct _tagged_arr) _temp1097;{ void* _temp1100[ 1u]={& _temp1101};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create log file in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1100, sizeof( void*), 1u));}});
return 1;} Cyc_cstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1097, _tag_arr("cstubs.c", sizeof( unsigned char), 9u)),
_tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1103; _temp1103.tag= Cyc_Std_String_pa;
_temp1103.f1=( struct _tagged_arr) _temp1097;{ void* _temp1102[ 1u]={& _temp1103};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1102, sizeof( void*), 1u));}});
return 1;} Cyc_cycstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1097, _tag_arr("cycstubs.cyc", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1105; _temp1105.tag= Cyc_Std_String_pa;
_temp1105.f1=( struct _tagged_arr) _temp1097;{ void* _temp1104[ 1u]={& _temp1105};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof( unsigned char), 52u), _tag_arr( _temp1104, sizeof( void*), 1u));}});
return 1;}({ void* _temp1106[ 0u]={}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file), _tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1106, sizeof( void*), 0u));}); Cyc_sizeof_unsignedlongint=
Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned long int", sizeof( unsigned char), 18u)); Cyc_sizeof_unsignedshortint=
Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned short int", sizeof( unsigned char), 19u)); Cyc_sizeof_shortint=
Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("short int", sizeof( unsigned char), 10u)); Cyc_sizeof_int= Cyc_getsize((
struct _tagged_arr) _temp1097, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("int", sizeof( unsigned char), 4u)); Cyc_sizeof_short= Cyc_getsize((
struct _tagged_arr) _temp1097, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("short", sizeof( unsigned char), 6u)); Cyc_sizeof_fdmask= Cyc_getsize((
struct _tagged_arr) _temp1097, _tag_arr("#include <sys/select.h>", sizeof(
unsigned char), 24u), _tag_arr("fd_mask", sizeof( unsigned char), 8u)); Cyc_sizeof___fdmask=
Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("#include <sys/select.h>",
sizeof( unsigned char), 24u), _tag_arr("__fd_mask", sizeof( unsigned char), 10u));
Cyc_sizeof_uint32= Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("#include <sys/types.h>",
sizeof( unsigned char), 23u), _tag_arr("__uint32_t", sizeof( unsigned char), 11u));
Cyc_sizeof_sockaddr= Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("#include <sys/socket.h>",
sizeof( unsigned char), 24u), _tag_arr("struct sockaddr", sizeof( unsigned char),
16u)); Cyc_sizeof_inport= Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("in_port_t", sizeof( unsigned char), 10u));
Cyc_sizeof_inaddr= Cyc_getsize(( struct _tagged_arr) _temp1097, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("struct in_addr", sizeof( unsigned char),
15u));{ int i= 2; for( 0; i <  argc; i ++){ Cyc_process_specfile(( struct
_tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), i)),( struct _tagged_arr) _temp1097);}} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_log_file)); Cyc_Std_fclose(( struct
Cyc_Std___sFILE*) _check_null( Cyc_cstubs_file)); Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file)); return 0;}}
