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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_remove( struct
_tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*); extern struct
Cyc_Std___sFILE* Cyc_Std_fopen( struct _tagged_arr __filename, struct
_tagged_arr __modes); extern int Cyc_Std_fputc( int __c, struct Cyc_Std___sFILE*
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
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
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
__z); struct Cyc_Std_option{ struct _tagged_arr name; int has_arg; int* flag;
int val; } ; extern unsigned int alarm( unsigned int seconds); extern int close(
int); extern void _exit( int); extern int getpid(); extern int getppid(); extern
int fork(); extern int fchdir( int); extern int fchown( int, unsigned int,
unsigned int); extern int fsync( int); extern int ftruncate( int, int); extern
int dup( int); extern int dup2( int, int); extern unsigned int getuid(); extern
int setuid( unsigned int uid); extern unsigned int geteuid(); extern int seteuid(
unsigned int euid); extern unsigned int getgid(); extern int setgid(
unsigned int gid); extern unsigned int getegid(); extern int setegid(
unsigned int egid); extern int nice( int); extern int pause(); extern int pipe(
int* filedes); extern int lseek( int filedes, int offset, int whence); extern
unsigned int sleep( unsigned int); extern int isatty( int); int Cyc_Std_access(
struct _tagged_arr, int); int Cyc_Std_chdir( struct _tagged_arr); int Cyc_Std_chown(
struct _tagged_arr, unsigned int, unsigned int); struct _tagged_arr Cyc_Std_getcwd(
struct _tagged_arr buf, unsigned int size); int Cyc_Std_execl( struct
_tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv); int Cyc_Std_execlp(
struct _tagged_arr file, struct _tagged_arr arg0, struct _tagged_arr argv); int
Cyc_Std_execve( struct _tagged_arr filename, struct _tagged_arr argv, struct
_tagged_arr envp); int Cyc_Std_link( struct _tagged_arr, struct _tagged_arr);
int Cyc_Std_read( int fd, struct _tagged_arr buf, unsigned int count); int Cyc_Std_rmdir(
struct _tagged_arr); int Cyc_Std_symlink( struct _tagged_arr, struct _tagged_arr);
int Cyc_Std_truncate( struct _tagged_arr, int); int Cyc_Std_write( int fd,
struct _tagged_arr buf, unsigned int count); int Cyc_Std_unlink( struct
_tagged_arr pathname); int Cyc_Std_gethostname( struct _tagged_arr, unsigned int);
int Cyc_Std_chroot( struct _tagged_arr); struct _tagged_arr Cyc_Std_getpass(
struct _tagged_arr prompt); extern int isalnum( int); extern int isalpha( int);
extern int isascii( int); extern int iscntrl( int); extern int isdigit( int);
extern int isgraph( int); extern int islower( int); extern int isprint( int);
extern int ispunct( int); extern int isspace( int); extern int isupper( int);
extern int isxdigit( int); extern int toascii( int); extern int tolower( int);
extern int toupper( int); extern int _tolower( int); extern int _toupper( int);
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1;
static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars;
struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args; int
c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
0; int Cyc_sizeof_unsignedlongint= - 1; int Cyc_sizeof_unsignedshortint= - 1;
int Cyc_sizeof_int= - 1; int Cyc_sizeof_fdmask= - 1; int Cyc_sizeof_uint32= - 1;
int Cyc_sizeof_sockaddr= - 1; int Cyc_sizeof_inport= - 1; int Cyc_sizeof_inaddr=
- 1; static struct _tagged_arr* Cyc_current_source= 0; static struct Cyc_List_List*
Cyc_current_args= 0; static struct Cyc_Set_Set** Cyc_current_targets= 0; static
void Cyc_add_target( struct _tagged_arr* sptr){ Cyc_current_targets=({ struct
Cyc_Set_Set** _temp0=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp0[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr*
elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets)),
sptr); _temp0;});} struct _tuple8{ struct _tagged_arr* f1; struct Cyc_Set_Set*
f2; } ; extern struct _tuple8* Cyc_line( struct Cyc_Lexing_lexbuf*); extern int
Cyc_macroname( struct Cyc_Lexing_lexbuf*); extern int Cyc_args( struct Cyc_Lexing_lexbuf*);
extern int Cyc_token( struct Cyc_Lexing_lexbuf*); extern int Cyc_string( struct
Cyc_Lexing_lexbuf*); struct Cyc_Std___sFILE* Cyc_slurp_out= 0; extern int Cyc_slurp(
struct Cyc_Lexing_lexbuf*); extern int Cyc_slurp_string( struct Cyc_Lexing_lexbuf*);
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
0; struct Cyc_Buffer_t* Cyc_specbuf= 0; int Cyc_braces_to_match= 0; int Cyc_numdef=
0; const int Cyc_lex_base[ 462u]=( const int[ 462u]){ 0, 0, 75, 192, 305, 285,
310, 27, 355, 28, 502, 616, 694, 290, 468, 91, - 3, 0, - 2, - 1, - 8, - 3, 1, -
1, 585, - 4, 2, 158, - 5, 438, 611, 166, - 6, - 7, - 3, 16, - 2, 29, 11, 776, -
3, 854, 13, - 14, 223, - 13, 107, 22, 25, 27, 23, - 12, 27, 31, 55, 64, 51, 68,
316, 968, 1083, 323, 66, 82, 81, 103, 109, 122, 154, 190, 213, 221, 213, 321,
356, 236, 230, 223, 241, 241, 336, 361, 238, 242, 240, 249, 237, 255, 349, 1197,
1312, 386, 241, 261, 246, 247, 247, 269, 366, 1426, 1541, - 9, 618, - 10, 255,
275, 487, 1655, 1770, 974, - 8, 511, 513, 1847, 1924, 642, 1999, 277, 277, 280,
278, 274, 284, 0, 4, 636, 5, 644, 2074, 2097, 940, - 25, 278, 280, 1, 280, 286,
270, 293, 290, 312, 1085, 1113, 321, 307, 303, 337, 334, 346, 377, 372, 382, 385,
721, 994, 387, 395, 390, 396, 397, 808, 999, 404, 400, 1202, 407, 415, 1097,
1111, 415, 411, 1314, 414, 412, 431, 415, 1204, 1228, 422, 422, 443, 437, 450,
462, 463, 450, 1428, 470, 496, 494, 496, 516, 1543, 537, 529, 1657, 531, 529,
528, 550, 531, 1772, 552, 556, 552, 548, 554, 8, 10, 576, 556, 2146, 578, 574,
603, 586, 595, 2148, 589, 605, 1212, - 13, 900, 600, 601, 608, 600, 612, 622,
610, 613, 618, 630, 637, 621, - 24, 630, 634, 630, 651, 659, 660, - 20, 641, 684,
676, 672, 683, 679, 710, 727, 728, 724, 720, 720, 742, 743, - 21, 734, 759, 752,
764, 760, 804, 782, 804, 817, 805, 809, 800, 800, - 23, 801, 804, 840, 848, 830,
832, 878, 885, 887, 1, 4, 6, 890, 891, 887, 891, 910, 910, 918, 920, 2, 21, 932,
933, 2109, 18, 19, 20, 21, 918, 916, 932, 964, 965, 23, 1020, 1021, - 19, 969,
1004, 1059, 1060, 1062, - 16, 1010, 1013, 1070, 1071, 1072, - 17, 1019, 1021,
1077, 1078, 1086, - 15, 1033, 1035, 1100, 1101, 1103, - 18, 1049, 1039, 1066,
1064, 1077, 4, 1080, 1071, 1101, 1108, 1121, 1191, - 11, - 7, - 8, 6, 1319, 2169,
7, 1394, 2181, 2204, 1402, 1339, - 49, 1325, - 2, 1163, - 4, 1165, 1194, 2173,
1166, 1196, 1197, 1509, 1170, 2242, 2285, 1175, 1228, 1174, 1269, 2365, 1179,
1230, - 36, - 42, - 37, 2440, - 28, 1195, - 40, 1200, - 27, - 45, - 39, - 48,
2515, 2544, 1626, 1241, 1262, 1738, 2554, 2584, 2323, 2152, 2617, 2648, 2686,
1255, 1266, 2756, 2794, 1291, 1364, 1400, 1410, 1402, 1412, - 6, - 34, 1247,
2726, - 47, - 30, - 32, - 46, - 29, - 31, - 33, 1300, 2834, 1302, 1306, 1528,
1308, 1311, 1317, 1320, 1336, 1364, 1365, 1366, 1367, 2907, 2991, - 22, 1757,
1420, - 41, - 38, - 35, - 26, 1452, 3073, 1430, 3156, 1424, 8, 1366, 1366, 1366,
1365, 1361, 1372, 1457}; const int Cyc_lex_backtrk[ 462u]=( const int[ 462u]){ -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
2, - 1, - 1, - 1, - 1, 2, - 1, 8, - 1, 3, 5, - 1, - 1, 6, 5, - 1, - 1, - 1, 6, -
1, 4, 1, 0, - 1, 0, 1, - 1, 10, - 1, 13, 13, 13, 13, 13, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 10, 1, 3, - 1, 1, 0, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 2, 8, 3, 5, - 1, 6, 5, - 1, 24, 24, 24, 24, 24, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 6, 1,
9, 2, 4, - 1, 5, 4, - 1, - 1, 2, - 1, 48, - 1, 48, 48, 48, 48, 48, 48, 48, 48, 5,
7, 48, 48, 48, 48, 0, 48, 48, - 1, - 1, - 1, 0, - 1, 43, - 1, 42, - 1, - 1, - 1,
- 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9, 5, 6, 5, 5, - 1, 4, 4, 4, 6, 6, 5,
5, - 1, - 1, - 1, 9, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 2, - 1, - 1, 2, 1, 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int
Cyc_lex_default[ 462u]=( const int[ 462u]){ - 1, - 1, - 1, 358, 347, 131, 20, -
1, - 1, 9, 25, 43, 28, 33, 20, 16, 0, - 1, 0, 0, 0, 0, - 1, 0, - 1, 0, - 1, - 1,
0, - 1, - 1, - 1, 0, 0, 0, - 1, 0, 37, - 1, - 1, 0, - 1, - 1, 0, - 1, 0, - 1, -
1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 0, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1,
- 1, 0, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, 0,
- 1, 0, - 1, - 1, 426, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 0, 0, 0, - 1, 0, - 1, 0, - 1, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, 0, 0, 0, 0, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_trans[
3413u]=( const int[ 3413u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 16, 25, 454, 16,
25, 110, 16, 18, 0, 42, 42, 42, 42, 42, 0, 42, 0, 0, 0, 0, 0, 19, 282, 293, 455,
51, 18, - 1, - 1, 18, - 1, - 1, 42, 283, 42, 284, 19, 452, 452, 452, 452, 452,
452, 452, 452, 452, 452, 25, 208, 209, 294, 117, 37, 307, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 328, 322, 316, 310, 452, 222, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
17, 82, 75, 62, 52, 53, 54, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
55, 56, 57, 58, 450, 63, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 64,
65, 66, 359, 360, 359, 359, 360, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32,
32, 32, 32, 32, 92, 67, 359, 361, 362, 68, 93, 363, 364, 365, 111, 111, 366, 367,
111, 368, 369, 370, 371, 372, 372, 372, 372, 372, 372, 372, 372, 372, 373, 69,
374, 375, 376, 111, 16, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377,
377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 16, -
1, - 1, 378, 377, 70, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377,
377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 348, 379,
132, 349, 19, 21, 71, 72, 124, 34, 59, 59, 133, 73, 59, 74, 74, 61, 61, 74, 76,
61, 35, 77, 350, 78, 79, 80, 83, 23, 81, 81, 18, 59, 81, 84, 85, 86, 74, 87, 61,
88, 104, 89, 89, 94, 95, 89, 96, 97, 74, 74, 98, 81, 74, 81, 81, 105, 106, 81,
99, 99, 118, 119, 99, 134, 89, 120, 121, 122, 123, 135, 340, 74, 334, 218, 137,
138, 81, 139, 91, 91, 351, 99, 91, 136, 140, 125, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 36, 141, 19, 202, 193, 91, 172, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 25, 147, 18, 148, 116, 149, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 21, 18, 150, 22, 151, 152, 19, 153, 30, 30, 30, 30, 30, 30,
30, 30, 30, 30, 107, 107, 165, 157, 107, 158, 23, 30, 30, 30, 30, 30, 30, 32,
159, 112, 112, 160, 163, 112, 164, 166, 167, 107, 111, 111, 115, 115, 111, 170,
115, 171, 173, 174, 175, 176, 187, 180, 112, 30, 30, 30, 30, 30, 30, 18, 181,
111, 182, 115, 32, 183, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 24, 101, 184, 185, 186, 188, 18, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 189, 190, 25, 191, 113, 26, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 44, 44, 102, 102, 44, 192, 102, 194, 27, 27, 27, 27, 27, 27, 27, 27,
196, 197, 198, 199, 200, 25, 201, 44, 126, 102, 115, 115, 195, 203, 115, 212,
205, 206, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 204, 207, 210, 211, 213, 115,
45, 30, 30, 30, 30, 30, 30, 28, 214, 127, 127, 127, 127, 127, 127, 127, 127, 130,
130, 130, 130, 130, 130, 130, 130, 215, 216, 217, 38, 38, 29, 219, 38, 30, 30,
30, 30, 30, 30, 220, 46, 272, 47, 258, 252, 243, 48, 236, 229, 18, 230, 38, 49,
50, 231, 154, 154, 232, 28, 154, 233, 234, 235, 237, 238, 239, 103, 39, 39, 39,
39, 39, 39, 39, 39, 39, 39, 240, 154, 241, 242, 128, 244, 16, 39, 39, 39, 39, 39,
39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
39, 245, 246, 247, 248, 39, 249, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 161, 161, 40, 250, 161,
251, 43, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 253, 254, 255, 256, 257, 259,
161, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
41, 41, 41, 41, 41, 41, 41, 260, 261, 262, 263, 41, 51, 41, 41, 41, 41, 41, 41,
41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
264, 265, 266, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 267, 268, 269, 270, 271,
273, 274, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
41, 41, 41, 41, 41, 41, 41, 41, 275, 276, 277, 278, 41, 25, 41, 41, 41, 41, 41,
41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
41, 59, 59, 279, 280, 59, 281, 109, 109, 285, 286, 109, 32, 32, 32, 32, 32, 32,
32, 32, 287, 223, 224, 225, 59, 226, 288, 154, 154, 227, 109, 154, 161, 161, 289,
290, 161, 291, 228, 292, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 154, 295, 296,
302, 303, 161, 304, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 305, 306, 308, 309, 60, 311, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 16, 61, 61, 141, 141, 61, 110, 141, 312, 313, 314, 155, 315, 162,
317, 168, 168, 318, 156, 168, 319, 320, 321, 323, 61, 324, 141, 325, 326, 168,
168, 142, 142, 168, 142, 142, 327, 329, 168, 330, 60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 331, 332, 168, 333, 142, 335, 336, 60, 60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 337, 338,
339, 341, 60, 342, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 89, 89, 143, 343, 89, 164, 164, 177,
177, 164, 169, 177, 144, 344, 345, 220, 220, 346, 448, 220, 447, 425, 145, 89,
146, 416, 445, 391, 164, 131, 177, 177, 177, 423, 382, 177, 420, 110, 220, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 446, 386, 424, 421, 422, 177, 390, 90, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
90, 90, 90, 90, 387, 388, 389, 380, 90, 419, 90, 90, 90, 90, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 28, 91, 91,
171, 171, 91, 110, 171, 357, 357, 384, 385, 357, 178, 449, 221, 449, 449, 110,
235, 32, 242, 32, 179, 91, 309, 171, 257, 357, 357, 333, 357, 357, 19, 381, 33,
346, 449, 110, 51, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 110, 357, 32, 19, 32,
221, 28, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, 90, 90, 90, 43, 327, 315, 321, 90, 28, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
90, 99, 99, 186, 186, 99, 28, 186, 356, 356, 356, 356, 356, 356, 356, 356, 28,
28, 28, 28, 28, 28, 28, 28, 99, 442, 186, 449, 19, 449, 449, 19, 456, 457, 458,
101, 459, 460, 28, 461, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 449,
33, 33, 415, 415, 19, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 33,
33, 415, 415, 100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 101, 102,
102, 192, 192, 102, 417, 192, 418, 418, 418, 418, 418, 418, 418, 418, 418, 418,
442, 0, 0, 0, 0, 0, 102, 0, 192, 443, 443, 443, 443, 443, 443, 443, 443, 103, 0,
0, 0, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 0, 0, 0, 0, 0, 0,
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 0, 0, 0, 100, 0, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100, 107, 107, 195, 195, 107, 397, 195, 397,
0, 0, 398, 398, 398, 398, 398, 398, 398, 398, 398, 398, 0, 0, 0, 107, 0, 195, 0,
0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 0, 0, 0, 0, 0, 0, 0, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 0, 0,
0, 0, 108, 0, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 33, 109, 109,
201, 201, 109, 0, 201, 398, 398, 398, 398, 398, 398, 398, 398, 398, 398, 442, 0,
0, 0, 0, 0, 109, 0, 201, 444, 444, 444, 444, 444, 444, 444, 444, 28, 0, 0, 0, 0,
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 0, 0, 0, 0, 0, 0, 0, 108, 108,
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 108, 108, 108, 108, 108, 108, 108, 0, 0, 0, 0, 108, 0, 108, 108, 108, 108,
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 108, 108, 108, 108, 108, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 19, 0, 0, 0, 0, 0, 0, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
0, 0, 0, 0, 114, 0, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 19, 0, 0, 0, 0, 0, 0, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 0, 0, 0, 0, 114, 0, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
0, 0, 0, 0, 0, 0, 0, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 0, 0, 0, 0,
116, 0, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 129, 129, 129, 129,
129, 129, 129, 129, 129, 129, 0, 0, 0, 0, 0, 0, 0, 129, 129, 129, 129, 129, 129,
129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 211, 211, 217, 217, 211, 0,
217, 129, 129, 129, 129, 129, 129, 0, 0, 0, 129, 129, 129, 129, 129, 129, 297,
211, 16, 217, 298, 352, - 1, 0, 0, 0, 25, 0, 16, 299, 0, 300, 0, 129, 129, 129,
129, 129, 129, 402, 402, 402, 402, 402, 402, 402, 402, 402, 402, 0, 0, - 1, 0, 0,
0, 0, 353, 353, 353, 353, 353, 353, 353, 353, 0, 0, 0, 301, 355, 355, 355, 355,
355, 355, 355, 355, 355, 355, 0, 0, 0, 0, 0, 0, 0, 355, 355, 355, 355, 355, 355,
355, 355, 355, 355, 355, 355, 355, 355, 355, 355, 0, 0, 0, 427, 25, 0, 0, 355,
355, 355, 355, 355, 355, 0, 0, 0, 355, 355, 355, 355, 355, 355, 0, 0, 0, 0, 392,
354, 403, 403, 403, 403, 403, 403, 403, 403, 404, 404, 0, 355, 355, 355, 355,
355, 355, 0, 0, 0, 0, 394, 0, 0, 0, 0, 0, 0, 405, 0, 0, 0, 0, 0, 0, 0, 0, 406, 0,
0, 407, 392, 0, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 394, 0, 0, 0,
0, 0, 0, 405, 0, 0, 0, 394, 0, 0, 0, 0, 406, 0, 395, 407, 0, 0, 0, 401, 0, 401,
0, 396, 402, 402, 402, 402, 402, 402, 402, 402, 402, 402, 0, 0, 0, 0, 0, 394, 0,
0, 0, 0, 0, 0, 395, 0, 0, 0, 0, 0, 0, 0, 0, 396, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 0, 0, 0, 0, 0, 0, - 1, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 0, 0, 0, 0, 383, 0, 383, 383, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383,
383, 0, 0, 0, 0, 0, 0, 0, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 0, 0,
0, 0, 383, 0, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 399, 399, 399,
399, 399, 399, 399, 399, 399, 399, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 400, 103, 0,
0, 0, 0, 392, 103, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 398, 398,
398, 398, 398, 398, 398, 398, 398, 398, 0, 394, 0, 0, 400, 103, 0, 0, 395, 0, 0,
103, 101, 0, 0, 0, 0, 396, 101, 0, 399, 399, 399, 399, 399, 399, 399, 399, 399,
399, 0, 0, 0, 394, 0, 0, 0, 0, 0, 0, 395, 400, 103, 0, 101, 0, 0, 0, 103, 396,
101, 0, 0, 402, 402, 402, 402, 402, 402, 402, 402, 402, 402, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 400, 103, 103, 0, 0, 0, 0, 103, 103, 392, 0, 403, 403, 403, 403, 403,
403, 403, 403, 404, 404, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 394, 0, 103, 0, 0, 0,
0, 413, 103, 0, 0, 0, 0, 0, 0, 392, 414, 404, 404, 404, 404, 404, 404, 404, 404,
404, 404, 0, 0, 0, 0, 0, 394, 0, 0, 0, 0, 0, 394, 413, 0, 0, 0, 0, 0, 411, 0, 0,
414, 0, 0, 0, 0, 0, 412, 0, 0, 418, 418, 418, 418, 418, 418, 418, 418, 418, 418,
0, 0, 0, 394, 0, 0, 0, 0, 0, 0, 411, 400, 103, 0, 0, 0, 0, 0, 103, 412, 408, 408,
408, 408, 408, 408, 408, 408, 408, 408, 0, 0, 0, 0, 0, 0, 0, 408, 408, 408, 408,
408, 408, 400, 103, 0, 0, 0, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 408, 408, 408, 408,
408, 408, 408, 408, 408, 408, 0, 408, 408, 408, 408, 408, 408, 408, 408, 408,
408, 408, 408, 0, 0, 0, 428, 0, 409, 0, 0, 429, 0, 0, 0, 0, 0, 410, 0, 0, 430,
430, 430, 430, 430, 430, 430, 430, 0, 408, 408, 408, 408, 408, 408, 431, 0, 0, 0,
0, 409, 0, 0, 0, 0, 0, 0, 0, 0, 410, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
432, 0, 0, 0, 0, 433, 434, 0, 0, 0, 435, 0, 0, 0, 0, 0, 0, 0, 436, 0, 0, 0, 437,
0, 438, 0, 439, 0, 440, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 0, 0,
0, 0, 0, 0, 0, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 0, 0, 0, 0, 0,
0, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 271, 0, 0, 0, 0, 0, 0, 0,
0, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 0, 0, 0, 0, 0, 0, 0, 441,
441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, 441, 441, 441, 0, 0, 0, 0, 0, 0, 441, 441, 441,
441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, 441, 18, 0, 0, 451, 0, 0, 0, 450, 450, 450, 450,
450, 450, 450, 450, 450, 450, 0, 0, 0, 0, 0, 0, 0, 450, 450, 450, 450, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
450, 450, 450, 450, 0, 0, 0, 0, 450, 0, 450, 450, 450, 450, 450, 450, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
450, 450, 453, 0, 0, 0, 0, 0, 0, 0, 452, 452, 452, 452, 452, 452, 452, 452, 452,
452, 0, 0, 0, 0, 0, 0, 0, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 0, 0,
0, 0, 452, 0, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; const int Cyc_lex_check[
3413u]=( const int[ 3413u]){ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
22, 26, 0, 124, 126, 349, 352, 454, - 1, 38, 38, 42, 42, 38, - 1, 42, - 1, - 1,
- 1, - 1, - 1, 123, 281, 292, 0, 339, 7, 9, 37, 7, 9, 37, 38, 282, 42, 283, 17,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 35, 207, 208, 293, 7, 35, 306, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 297, 298, 299, 300, 1,
134, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 15, 47, 48, 49, 50, 52, 53, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 54, 55, 56, 57, 2, 62,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 63,
64, 65, 3, 3, 3, 3, 3, 27, 27, 27, 27, 27, 27, 27, 27, 31, 31, 31, 31, 31, 31,
31, 31, 46, 66, 3, 3, 3, 67, 46, 3, 3, 3, 44, 44, 3, 3, 44, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 68, 3, 3, 3, 44, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 9, 37, 3, 3, 69, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 5, 4, 5, 6, 70,
71, 6, 13, 58, 58, 5, 72, 58, 73, 73, 61, 61, 73, 75, 61, 13, 76, 4, 77, 78, 79,
82, 6, 80, 80, 15, 58, 80, 83, 84, 85, 73, 86, 61, 87, 92, 88, 88, 93, 94, 88,
95, 96, 74, 74, 97, 80, 74, 81, 81, 104, 105, 81, 98, 98, 117, 118, 98, 5, 88,
119, 120, 121, 122, 5, 132, 74, 133, 135, 136, 137, 81, 138, 91, 91, 4, 98, 91,
5, 139, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 13, 140, 13, 143, 144, 91, 145, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 61, 146, 3,
147, 8, 148, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 14, 74, 149, 14, 150, 151, 81, 152, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 106, 106, 155, 156, 106, 157, 14, 29, 29, 29, 29, 29, 29, 91, 158, 10, 10,
159, 162, 10, 163, 165, 166, 106, 111, 111, 112, 112, 111, 169, 112, 170, 172,
173, 174, 175, 178, 179, 10, 29, 29, 29, 29, 29, 29, 5, 180, 111, 181, 112, 13,
182, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 14, 4, 183, 184, 185, 187,
6, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
10, 10, 10, 10, 10, 10, 10, 188, 189, 24, 190, 10, 24, 10, 10, 10, 10, 10, 10,
10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
11, 11, 102, 102, 11, 191, 102, 193, 24, 24, 24, 24, 24, 24, 24, 24, 194, 196,
197, 198, 199, 125, 200, 11, 125, 102, 115, 115, 193, 202, 115, 203, 204, 205,
30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 202, 206, 209, 210, 212, 115, 11, 30, 30,
30, 30, 30, 30, 24, 213, 125, 125, 125, 125, 125, 125, 125, 125, 127, 127, 127,
127, 127, 127, 127, 127, 214, 215, 216, 12, 12, 24, 218, 12, 30, 30, 30, 30, 30,
30, 219, 11, 223, 11, 224, 225, 226, 11, 227, 228, 14, 229, 12, 11, 11, 230, 153,
153, 231, 125, 153, 232, 233, 234, 236, 237, 238, 102, 12, 12, 12, 12, 12, 12,
12, 12, 12, 12, 239, 153, 240, 241, 125, 243, 10, 12, 12, 12, 12, 12, 12, 12, 12,
12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 244, 245,
246, 247, 12, 248, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 160, 160, 12, 249, 160, 250, 251, 39,
39, 39, 39, 39, 39, 39, 39, 39, 39, 252, 253, 254, 255, 256, 258, 160, 39, 39,
39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
39, 39, 39, 39, 259, 260, 261, 262, 39, 11, 39, 39, 39, 39, 39, 39, 39, 39, 39,
39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 263, 264,
265, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 266, 267, 268, 269, 270, 272, 273,
41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
41, 41, 41, 41, 41, 41, 274, 275, 276, 277, 41, 12, 41, 41, 41, 41, 41, 41, 41,
41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 59,
59, 278, 279, 59, 280, 109, 109, 284, 285, 109, 130, 130, 130, 130, 130, 130,
130, 130, 286, 222, 222, 222, 59, 222, 287, 154, 154, 222, 109, 154, 161, 161,
288, 289, 161, 290, 222, 291, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 154, 294,
295, 301, 302, 161, 303, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 304, 305, 307, 308, 59, 310, 59,
59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
59, 59, 59, 59, 59, 59, 60, 60, 141, 141, 60, 109, 141, 311, 312, 313, 154, 314,
161, 316, 167, 167, 317, 154, 167, 318, 319, 320, 322, 60, 323, 141, 324, 325,
168, 168, 142, 142, 168, 141, 142, 326, 328, 167, 329, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 330, 331, 168, 332, 142, 334, 335, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 336,
337, 338, 340, 60, 341, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 89, 89, 142, 342, 89, 164, 164,
176, 176, 164, 168, 176, 142, 343, 344, 220, 220, 345, 361, 220, 363, 366, 142,
89, 142, 370, 364, 373, 164, 375, 176, 177, 177, 367, 378, 177, 368, 164, 220,
89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 364, 385, 367, 368, 368, 177, 387, 89,
89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,
89, 89, 89, 89, 89, 374, 374, 374, 379, 89, 417, 89, 89, 89, 89, 89, 89, 89, 89,
89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 90,
90, 171, 171, 90, 395, 171, 350, 350, 376, 376, 350, 177, 359, 220, 359, 359,
396, 426, 405, 428, 406, 177, 90, 429, 171, 431, 357, 357, 432, 350, 357, 350,
379, 171, 433, 359, 395, 434, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 396, 357,
405, 357, 406, 435, 409, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 436, 437, 438, 439, 90, 409, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, 99, 99, 186, 186, 99, 410, 186, 353, 353, 353, 353, 353, 353,
353, 353, 356, 356, 356, 356, 356, 356, 356, 356, 99, 444, 186, 449, 451, 449,
449, 453, 455, 456, 457, 186, 458, 459, 410, 460, 99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 449, 411, 412, 413, 414, 461, - 1, 99, 99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 411, 412,
413, 414, 99, - 1, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 100, 100, 192, 192, 100, 369,
192, 369, 369, 369, 369, 369, 369, 369, 369, 369, 369, 430, - 1, - 1, - 1, - 1,
- 1, 100, - 1, 192, 430, 430, 430, 430, 430, 430, 430, 430, 192, - 1, - 1, - 1,
- 1, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, - 1, - 1, - 1, - 1,
100, - 1, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 107, 107, 195, 195,
107, 394, 195, 394, - 1, - 1, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394,
- 1, - 1, - 1, 107, - 1, 195, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 195, - 1,
- 1, - 1, - 1, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, - 1, - 1,
- 1, - 1, 107, - 1, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108,
108, 201, 201, 108, - 1, 201, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397,
443, - 1, - 1, - 1, - 1, - 1, 108, - 1, 201, 443, 443, 443, 443, 443, 443, 443,
443, 201, - 1, - 1, - 1, - 1, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, - 1, - 1, - 1, - 1, 108, - 1, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
108, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, - 1, - 1,
- 1, - 1, - 1, - 1, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, - 1, - 1,
- 1, - 1, 113, - 1, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113,
113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, - 1, - 1, - 1, - 1, - 1,
- 1, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, - 1, - 1, - 1, - 1, 114,
- 1, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114,
114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 114, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, - 1, - 1, - 1, - 1, 116, - 1, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
116, 116, 116, 116, 116, 116, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 128, 128, 128, 128, 128, 128, 129, 129, 129,
129, 129, 129, 129, 129, 129, 129, 211, 211, 217, 217, 211, - 1, 217, 129, 129,
129, 129, 129, 129, - 1, - 1, - 1, 128, 128, 128, 128, 128, 128, 296, 211, 351,
217, 296, 351, 365, - 1, - 1, - 1, 211, - 1, 217, 296, - 1, 296, - 1, 129, 129,
129, 129, 129, 129, 401, 401, 401, 401, 401, 401, 401, 401, 401, 401, - 1, - 1,
365, - 1, - 1, - 1, - 1, 351, 351, 351, 351, 351, 351, 351, 351, - 1, - 1, - 1,
296, 354, 354, 354, 354, 354, 354, 354, 354, 354, 354, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 354, 354, 354, 354, 354, 354, 355, 355, 355, 355, 355, 355, 355, 355,
355, 355, - 1, - 1, - 1, 365, 351, - 1, - 1, 355, 355, 355, 355, 355, 355, - 1,
- 1, - 1, 354, 354, 354, 354, 354, 354, - 1, - 1, - 1, - 1, 371, 351, 371, 371,
371, 371, 371, 371, 371, 371, 371, 371, - 1, 355, 355, 355, 355, 355, 355, - 1,
- 1, - 1, - 1, 371, - 1, - 1, - 1, - 1, - 1, - 1, 371, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 371, - 1, - 1, 371, 372, - 1, 372, 372, 372, 372, 372, 372, 372,
372, 372, 372, 371, - 1, - 1, - 1, - 1, - 1, - 1, 371, - 1, - 1, - 1, 372, - 1,
- 1, - 1, - 1, 371, - 1, 372, 371, - 1, - 1, - 1, 400, - 1, 400, - 1, 372, 400,
400, 400, 400, 400, 400, 400, 400, 400, 400, - 1, - 1, - 1, - 1, - 1, 372, - 1,
- 1, - 1, - 1, - 1, - 1, 372, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 372, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 377, 377, 377, 377, 377, 377, 377,
377, 377, 377, - 1, - 1, - 1, - 1, - 1, - 1, 365, 377, 377, 377, 377, 377, 377,
377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377,
377, 377, 377, 377, - 1, - 1, - 1, - 1, 377, - 1, 377, 377, 377, 377, 377, 377,
377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377, 377,
377, 377, 377, 377, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, - 1,
- 1, - 1, - 1, 383, - 1, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383,
383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 383, 392,
392, 392, 392, 392, 392, 392, 392, 392, 392, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 392, 392, - 1, - 1, - 1, - 1, 393, 392, 393, 393, 393, 393,
393, 393, 393, 393, 393, 393, 398, 398, 398, 398, 398, 398, 398, 398, 398, 398,
- 1, 393, - 1, - 1, 392, 392, - 1, - 1, 393, - 1, - 1, 392, 398, - 1, - 1, - 1,
- 1, 393, 398, - 1, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, - 1, - 1,
- 1, 393, - 1, - 1, - 1, - 1, - 1, - 1, 393, 399, 399, - 1, 398, - 1, - 1, - 1,
399, 393, 398, - 1, - 1, 402, 402, 402, 402, 402, 402, 402, 402, 402, 402, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 399, 399, 402, - 1, - 1, - 1, - 1,
399, 402, 403, - 1, 403, 403, 403, 403, 403, 403, 403, 403, 403, 403, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 403, - 1, 402, - 1, - 1, - 1, - 1,
403, 402, - 1, - 1, - 1, - 1, - 1, - 1, 404, 403, 404, 404, 404, 404, 404, 404,
404, 404, 404, 404, - 1, - 1, - 1, - 1, - 1, 403, - 1, - 1, - 1, - 1, - 1, 404,
403, - 1, - 1, - 1, - 1, - 1, 404, - 1, - 1, 403, - 1, - 1, - 1, - 1, - 1, 404,
- 1, - 1, 418, 418, 418, 418, 418, 418, 418, 418, 418, 418, - 1, - 1, - 1, 404,
- 1, - 1, - 1, - 1, - 1, - 1, 404, 418, 418, - 1, - 1, - 1, - 1, - 1, 418, 404,
407, 407, 407, 407, 407, 407, 407, 407, 407, 407, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 407, 407, 407, 407, 407, 407, 418, 418, - 1, - 1, - 1, - 1, - 1, 418, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 408, 408, 408, 408, 408, 408, 408, 408, 408, 408,
- 1, 407, 407, 407, 407, 407, 407, 408, 408, 408, 408, 408, 408, - 1, - 1, - 1,
427, - 1, 408, - 1, - 1, 427, - 1, - 1, - 1, - 1, - 1, 408, - 1, - 1, 427, 427,
427, 427, 427, 427, 427, 427, - 1, 408, 408, 408, 408, 408, 408, 427, - 1, - 1,
- 1, - 1, 408, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 408, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 427, - 1, - 1, - 1, - 1, 427,
427, - 1, - 1, - 1, 427, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 427, - 1, - 1, - 1,
427, - 1, 427, - 1, 427, - 1, 427, 440, 440, 440, 440, 440, 440, 440, 440, 440,
440, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 440, 440, 440, 440, 440, 440, 440, 440,
440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440,
440, 440, - 1, - 1, - 1, - 1, - 1, - 1, 440, 440, 440, 440, 440, 440, 440, 440,
440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440,
440, 440, 441, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, - 1, - 1, - 1, - 1, - 1, - 1, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441, 441,
441, 441, 441, 441, 441, 441, 450, - 1, - 1, 450, - 1, - 1, - 1, 450, 450, 450,
450, 450, 450, 450, 450, 450, 450, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, - 1, - 1, - 1, - 1, 450, - 1, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450,
450, 450, 450, 450, 450, 450, 450, 450, 452, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, - 1, - 1, - 1, - 1, 452,
- 1, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452,
452, 452, 452, 452, 452, 452, 452, 452, 452, 452, 452, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1}; int Cyc_lex_engine( int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int
state; int base; int backtrk; int c; state= start_state; if( state >=  0){ lbuf->lex_last_pos=(
lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{
state=( - state) -  1;} while( 1) { base= Cyc_lex_base[
_check_known_subscript_notnull( 462u, state)]; if( base <  0){ return( - base) - 
1;} backtrk= Cyc_lex_backtrk[ _check_known_subscript_notnull( 462u, state)]; if(
backtrk >=  0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >=  lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) -  1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c ==  - 1){ c= 256;}} if( Cyc_lex_check[
_check_known_subscript_notnull( 3413u, base +  c)] ==  state){ state= Cyc_lex_trans[
_check_known_subscript_notnull( 3413u, base +  c)];} else{ state= Cyc_lex_default[
_check_known_subscript_notnull( 462u, state)];} if( state <  0){ lbuf->lex_curr_pos=
lbuf->lex_last_pos; if( lbuf->lex_last_action ==  - 1){( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp1=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp1[ 0]=({ struct Cyc_Lexing_Error_struct
_temp2; _temp2.tag= Cyc_Lexing_Error; _temp2.f1= _tag_arr("empty token", sizeof(
unsigned char), 12u); _temp2;}); _temp1;}));} else{ return lbuf->lex_last_action;}}
else{ if( c ==  256){ lbuf->lex_eof_reached= 0;}}}} struct _tuple8* Cyc_line_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL3: Cyc_macroname( lexbuf); for(
0; Cyc_current_args !=  0; Cyc_current_args=(( struct Cyc_List_List*)
_check_null( Cyc_current_args))->tl){ Cyc_current_targets=({ struct Cyc_Set_Set**
_temp5=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*)); _temp5[
0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt))
Cyc_Set_delete)(*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets)),(
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_current_args))->hd);
_temp5;});} return({ struct _tuple8* _temp6=( struct _tuple8*) _cycalloc(
sizeof( struct _tuple8)); _temp6->f1=( struct _tagged_arr*) _check_null( Cyc_current_source);
_temp6->f2=*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets)); _temp6;});
case 1: _LL4: return Cyc_line( lexbuf); case 2: _LL7: return 0; default: _LL8:(
lexbuf->refill_buff)( lexbuf); return Cyc_line_rec( lexbuf, lexstate);}( int)
_throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp10=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp10[ 0]=({ struct Cyc_Lexing_Error_struct
_temp11; _temp11.tag= Cyc_Lexing_Error; _temp11.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp11;}); _temp10;}));} struct _tuple8* Cyc_line(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_line_rec( lexbuf, 0);} int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL12: Cyc_current_source=({
struct _tagged_arr* _temp14=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp14[ 0]=( struct _tagged_arr) Cyc_Std_substring(( struct
_tagged_arr) Cyc_Lexing_lexeme( lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end(
lexbuf) -  Cyc_Lexing_lexeme_start( lexbuf)) -  2)); _temp14;}); Cyc_current_args=
0; Cyc_current_targets=({ struct Cyc_Set_Set** _temp15=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp15[ 0]=(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_temp15;}); Cyc_token( lexbuf); return 0; case 1: _LL13: Cyc_current_source=({
struct _tagged_arr* _temp17=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp17[ 0]=( struct _tagged_arr) Cyc_Std_substring(( struct
_tagged_arr) Cyc_Lexing_lexeme( lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end(
lexbuf) -  Cyc_Lexing_lexeme_start( lexbuf)) -  1)); _temp17;}); Cyc_current_args=
0; Cyc_current_targets=({ struct Cyc_Set_Set** _temp18=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp18[ 0]=(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_temp18;}); Cyc_args( lexbuf); return 0; case 2: _LL16: Cyc_current_source=({
struct _tagged_arr* _temp20=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp20[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);
_temp20;}); Cyc_current_args= 0; Cyc_current_targets=({ struct Cyc_Set_Set**
_temp21=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp21[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); _temp21;}); Cyc_token(
lexbuf); return 0; default: _LL19:( lexbuf->refill_buff)( lexbuf); return Cyc_macroname_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp23=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp23[ 0]=({ struct Cyc_Lexing_Error_struct _temp24; _temp24.tag= Cyc_Lexing_Error;
_temp24.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp24;}); _temp23;}));} int Cyc_macroname( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_macroname_rec( lexbuf, 1);} int Cyc_args_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL25: { struct _tagged_arr* _temp27=({ struct _tagged_arr*
_temp29=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp29[
0]=( struct _tagged_arr) Cyc_Std_substring(( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end( lexbuf) -  Cyc_Lexing_lexeme_start(
lexbuf)) -  2)); _temp29;}); Cyc_current_args=({ struct Cyc_List_List* _temp28=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp28->hd=(
void*) _temp27; _temp28->tl= Cyc_current_args; _temp28;}); return Cyc_args(
lexbuf);} case 1: _LL26: { struct _tagged_arr* _temp31=({ struct _tagged_arr*
_temp33=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp33[
0]=( struct _tagged_arr) Cyc_Std_substring(( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf), 0,( unsigned int)(( Cyc_Lexing_lexeme_end( lexbuf) -  Cyc_Lexing_lexeme_start(
lexbuf)) -  1)); _temp33;}); Cyc_current_args=({ struct Cyc_List_List* _temp32=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp32->hd=(
void*) _temp31; _temp32->tl= Cyc_current_args; _temp32;}); return Cyc_token(
lexbuf);} default: _LL30:( lexbuf->refill_buff)( lexbuf); return Cyc_args_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp35=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp35[ 0]=({ struct Cyc_Lexing_Error_struct _temp36; _temp36.tag= Cyc_Lexing_Error;
_temp36.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp36;}); _temp35;}));} int Cyc_args( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_args_rec( lexbuf, 2);} int Cyc_token_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL37: Cyc_add_target(({ struct _tagged_arr* _temp39=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp39[ 0]=( struct
_tagged_arr) Cyc_Lexing_lexeme( lexbuf); _temp39;})); return Cyc_token( lexbuf);
case 1: _LL38: return 0; case 2: _LL40: return Cyc_token( lexbuf); case 3: _LL41:
Cyc_string( lexbuf); return Cyc_token( lexbuf); case 4: _LL42: return Cyc_token(
lexbuf); case 5: _LL43: return Cyc_token( lexbuf); case 6: _LL44: return Cyc_token(
lexbuf); case 7: _LL45: return Cyc_token( lexbuf); case 8: _LL46: return Cyc_token(
lexbuf); case 9: _LL47: return Cyc_token( lexbuf); case 10: _LL48: return Cyc_token(
lexbuf); case 11: _LL49: return Cyc_token( lexbuf); case 12: _LL50: return Cyc_token(
lexbuf); case 13: _LL51: return Cyc_token( lexbuf); case 14: _LL52: return Cyc_token(
lexbuf); case 15: _LL53: return Cyc_token( lexbuf); case 16: _LL54: return Cyc_token(
lexbuf); case 17: _LL55: return Cyc_token( lexbuf); case 18: _LL56: return Cyc_token(
lexbuf); case 19: _LL57: return Cyc_token( lexbuf); case 20: _LL58: return Cyc_token(
lexbuf); case 21: _LL59: return Cyc_token( lexbuf); case 22: _LL60: return Cyc_token(
lexbuf); case 23: _LL61: return Cyc_token( lexbuf); case 24: _LL62: return Cyc_token(
lexbuf); case 25: _LL63: return Cyc_token( lexbuf); case 26: _LL64: return Cyc_token(
lexbuf); case 27: _LL65: return Cyc_token( lexbuf); case 28: _LL66: return Cyc_token(
lexbuf); case 29: _LL67: return Cyc_token( lexbuf); case 30: _LL68: return Cyc_token(
lexbuf); case 31: _LL69: return Cyc_token( lexbuf); case 32: _LL70: return Cyc_token(
lexbuf); case 33: _LL71: return Cyc_token( lexbuf); case 34: _LL72: return Cyc_token(
lexbuf); case 35: _LL73: return Cyc_token( lexbuf); case 36: _LL74: return Cyc_token(
lexbuf); case 37: _LL75: return Cyc_token( lexbuf); case 38: _LL76: return Cyc_token(
lexbuf); case 39: _LL77: return Cyc_token( lexbuf); case 40: _LL78: return Cyc_token(
lexbuf); case 41: _LL79: return Cyc_token( lexbuf); case 42: _LL80: return Cyc_token(
lexbuf); case 43: _LL81: return Cyc_token( lexbuf); case 44: _LL82: return Cyc_token(
lexbuf); case 45: _LL83: return Cyc_token( lexbuf); case 46: _LL84: return Cyc_token(
lexbuf); case 47: _LL85: return Cyc_token( lexbuf); case 48: _LL86: return Cyc_token(
lexbuf); default: _LL87:( lexbuf->refill_buff)( lexbuf); return Cyc_token_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp89=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp89[ 0]=({ struct Cyc_Lexing_Error_struct _temp90; _temp90.tag= Cyc_Lexing_Error;
_temp90.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp90;}); _temp89;}));} int Cyc_token( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_token_rec( lexbuf, 3);} int Cyc_string_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL91: return Cyc_string( lexbuf); case 1: _LL92: return 0;
case 2: _LL93: return Cyc_string( lexbuf); case 3: _LL94: return Cyc_string(
lexbuf); case 4: _LL95: return Cyc_string( lexbuf); case 5: _LL96: return Cyc_string(
lexbuf); case 6: _LL97: return Cyc_string( lexbuf); case 7: _LL98: return 0;
case 8: _LL99: return 0; case 9: _LL100: return Cyc_string( lexbuf); default:
_LL101:( lexbuf->refill_buff)( lexbuf); return Cyc_string_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp103=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp103[ 0]=({ struct Cyc_Lexing_Error_struct
_temp104; _temp104.tag= Cyc_Lexing_Error; _temp104.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp104;}); _temp103;}));} int Cyc_string( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_string_rec( lexbuf, 4);} int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL105: Cyc_Std_fputc(( int)'"',(
struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); if( Cyc_slurp_string(
lexbuf)){ return 1;} else{ return Cyc_slurp( lexbuf);} case 1: _LL106: return 0;
case 2: _LL107:({ struct Cyc_Std_Int_pa_struct _temp110; _temp110.tag= Cyc_Std_Int_pa;
_temp110.f1=( int)(( unsigned int) Cyc_sizeof_fdmask);{ void* _temp109[ 1u]={&
_temp110}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp109, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 3: _LL108:({ struct Cyc_Std_Int_pa_struct
_temp113; _temp113.tag= Cyc_Std_Int_pa; _temp113.f1=( int)(( unsigned int) Cyc_sizeof_uint32);{
void* _temp112[ 1u]={& _temp113}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp112, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 4:
_LL111:({ struct Cyc_Std_Int_pa_struct _temp116; _temp116.tag= Cyc_Std_Int_pa;
_temp116.f1=( int)(( unsigned int) Cyc_sizeof_inport);{ void* _temp115[ 1u]={&
_temp116}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp115, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 5: _LL114:({ struct Cyc_Std_Int_pa_struct
_temp119; _temp119.tag= Cyc_Std_Int_pa; _temp119.f1=( int)(( unsigned int) Cyc_sizeof_int);{
void* _temp118[ 1u]={& _temp119}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp118, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 6:
_LL117:({ struct Cyc_Std_Int_pa_struct _temp122; _temp122.tag= Cyc_Std_Int_pa;
_temp122.f1=( int)(( unsigned int) Cyc_sizeof_unsignedlongint);{ void* _temp121[
1u]={& _temp122}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp121, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 7: _LL120:({ struct Cyc_Std_Int_pa_struct
_temp125; _temp125.tag= Cyc_Std_Int_pa; _temp125.f1=( int)(( unsigned int) Cyc_sizeof_unsignedshortint);{
void* _temp124[ 1u]={& _temp125}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp124, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 8:
_LL123:({ struct Cyc_Std_Int_pa_struct _temp128; _temp128.tag= Cyc_Std_Int_pa;
_temp128.f1=( int)(( unsigned int) Cyc_sizeof_sockaddr);{ void* _temp127[ 1u]={&
_temp128}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr(" %d ", sizeof( unsigned char), 5u), _tag_arr( _temp127, sizeof( void*),
1u));}}); return Cyc_slurp( lexbuf); case 9: _LL126:({ struct Cyc_Std_Int_pa_struct
_temp131; _temp131.tag= Cyc_Std_Int_pa; _temp131.f1=( int)(( unsigned int) Cyc_sizeof_inaddr);{
void* _temp130[ 1u]={& _temp131}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr(" %d ", sizeof( unsigned char), 5u),
_tag_arr( _temp130, sizeof( void*), 1u));}}); return Cyc_slurp( lexbuf); case 10:
_LL129: Cyc_Std_fputs( _tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC ", sizeof(
unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 11: _LL132: Cyc_Std_fputs( _tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC ",
sizeof( unsigned char), 30u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 12: _LL133:({ struct Cyc_Std_Int_pa_struct
_temp136; _temp136.tag= Cyc_Std_Int_pa; _temp136.f1=( int)(( unsigned int) Cyc_enum_hack
++);{ void* _temp135[ 1u]={& _temp136}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("enum __CYCLONE_ENUM_HACK_%d {", sizeof(
unsigned char), 30u), _tag_arr( _temp135, sizeof( void*), 1u));}}); return Cyc_slurp(
lexbuf); case 13: _LL134: return Cyc_slurp( lexbuf); case 14: _LL137: return Cyc_slurp(
lexbuf); case 15: _LL138: return Cyc_slurp( lexbuf); case 16: _LL139: return Cyc_slurp(
lexbuf); case 17: _LL140: return Cyc_slurp( lexbuf); case 18: _LL141: return Cyc_slurp(
lexbuf); case 19: _LL142: Cyc_Std_fputs( _tag_arr("inline", sizeof(
unsigned char), 7u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 20: _LL143: Cyc_Std_fputs( _tag_arr("const",
sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 21: _LL144: Cyc_Std_fputs( _tag_arr("const",
sizeof( unsigned char), 6u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 22: _LL145: Cyc_Std_fputs( _tag_arr("int",
sizeof( unsigned char), 4u),( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out));
return Cyc_slurp( lexbuf); case 23: _LL146: return Cyc_slurp( lexbuf); case 24:
_LL147: Cyc_Std_fputc(( int) Cyc_Lexing_lexeme_char( lexbuf, 0),( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out)); return Cyc_slurp( lexbuf); default: _LL148:(
lexbuf->refill_buff)( lexbuf); return Cyc_slurp_rec( lexbuf, lexstate);}( int)
_throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp150=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp150[ 0]=({ struct Cyc_Lexing_Error_struct
_temp151; _temp151.tag= Cyc_Lexing_Error; _temp151.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp151;}); _temp150;}));} int Cyc_slurp( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_slurp_rec( lexbuf, 5);} int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL152: Cyc_Std_fputc(( int)'"',(
struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); return 0; case 1: _LL153:
return 1; case 2: _LL154: return 1; case 3: _LL155:({ struct Cyc_Std_String_pa_struct
_temp158; _temp158.tag= Cyc_Std_String_pa; _temp158.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp157[ 1u]={& _temp158}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp157, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 4: _LL156:({ struct Cyc_Std_String_pa_struct _temp161; _temp161.tag= Cyc_Std_String_pa;
_temp161.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp160[ 1u]={&
_temp161}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp160, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 5: _LL159:({ struct Cyc_Std_String_pa_struct
_temp164; _temp164.tag= Cyc_Std_String_pa; _temp164.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp163[ 1u]={& _temp164}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp163, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 6: _LL162:({ struct Cyc_Std_String_pa_struct _temp167; _temp167.tag= Cyc_Std_String_pa;
_temp167.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp166[ 1u]={&
_temp167}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp166, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); case 7: _LL165:({ struct Cyc_Std_String_pa_struct
_temp170; _temp170.tag= Cyc_Std_String_pa; _temp170.f1=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf);{ void* _temp169[ 1u]={& _temp170}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_slurp_out), _tag_arr("%s", sizeof( unsigned char), 3u),
_tag_arr( _temp169, sizeof( void*), 1u));}}); return Cyc_slurp_string( lexbuf);
case 8: _LL168:({ struct Cyc_Std_String_pa_struct _temp173; _temp173.tag= Cyc_Std_String_pa;
_temp173.f1=( struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf);{ void* _temp172[ 1u]={&
_temp173}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out),
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp172, sizeof( void*),
1u));}}); return Cyc_slurp_string( lexbuf); default: _LL171:( lexbuf->refill_buff)(
lexbuf); return Cyc_slurp_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp175=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp175[ 0]=({ struct Cyc_Lexing_Error_struct
_temp176; _temp176.tag= Cyc_Lexing_Error; _temp176.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp176;}); _temp175;}));} int Cyc_slurp_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_slurp_string_rec( lexbuf, 6);}
struct _tuple9* Cyc_suck_line_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case
0: _LL177: Cyc_current_line= _tag_arr("#define ", sizeof( unsigned char), 9u);
Cyc_suck_macroname( lexbuf); return({ struct _tuple9* _temp179=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp179->f1= Cyc_current_line; _temp179->f2=(
struct _tagged_arr*) _check_null( Cyc_current_source); _temp179;}); case 1:
_LL178: return Cyc_suck_line( lexbuf); case 2: _LL180: return 0; default: _LL181:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_line_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp183=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp183[ 0]=({ struct Cyc_Lexing_Error_struct
_temp184; _temp184.tag= Cyc_Lexing_Error; _temp184.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp184;}); _temp183;}));} struct _tuple9* Cyc_suck_line(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_line_rec( lexbuf, 7);} int
Cyc_suck_macroname_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL185:
Cyc_current_source=({ struct _tagged_arr* _temp187=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp187[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp187;}); Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat(
Cyc_current_line,*(( struct _tagged_arr*) _check_null( Cyc_current_source)));
return Cyc_suck_restofline( lexbuf); default: _LL186:( lexbuf->refill_buff)(
lexbuf); return Cyc_suck_macroname_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp189=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp189[ 0]=({ struct Cyc_Lexing_Error_struct
_temp190; _temp190.tag= Cyc_Lexing_Error; _temp190.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp190;}); _temp189;}));} int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_macroname_rec( lexbuf, 8);}
int Cyc_suck_restofline_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL191:
Cyc_current_line=( struct _tagged_arr) Cyc_Std_strconcat( Cyc_current_line,(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return 0; default: _LL192:(
lexbuf->refill_buff)( lexbuf); return Cyc_suck_restofline_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp194=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp194[ 0]=({ struct Cyc_Lexing_Error_struct
_temp195; _temp195.tag= Cyc_Lexing_Error; _temp195.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp195;}); _temp194;}));} int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_suck_restofline_rec( lexbuf, 9);}
struct _tuple11* Cyc_spec_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL196:
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
_tuple11* _temp198=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11));
_temp198->f1= Cyc_current_header; _temp198->f2= Cyc_current_symbols; _temp198->f3=
Cyc_current_omit_symbols; _temp198->f4= Cyc_current_prologue; _temp198->f5= Cyc_current_epilogue;
_temp198->f6= Cyc_current_cstubs; _temp198->f7= Cyc_current_cycstubs; _temp198;});
case 1: _LL197: return Cyc_spec( lexbuf); case 2: _LL199: return 0; case 3:
_LL200:({ struct Cyc_Std_Int_pa_struct _temp203; _temp203.tag= Cyc_Std_Int_pa;
_temp203.f1=( int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{
void* _temp202[ 1u]={& _temp203}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof( unsigned char), 67u), _tag_arr( _temp202, sizeof( void*), 1u));}});
return 0; default: _LL201:( lexbuf->refill_buff)( lexbuf); return Cyc_spec_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp205=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp205[ 0]=({ struct Cyc_Lexing_Error_struct _temp206; _temp206.tag= Cyc_Lexing_Error;
_temp206.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp206;}); _temp205;}));} struct _tuple11* Cyc_spec( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_spec_rec( lexbuf, 10);} int Cyc_commands_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL207: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols( lexbuf)){
return 1;} Cyc_current_symbols=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols, Cyc_current_symbols);
return Cyc_commands( lexbuf); case 1: _LL208: Cyc_snarfed_symbols= 0; if( Cyc_snarfsymbols(
lexbuf)){ return 1;} Cyc_current_omit_symbols=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_snarfed_symbols,
Cyc_current_omit_symbols); return Cyc_commands( lexbuf); case 2: _LL209: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp212=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp212->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp212->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp212;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp211=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp211->hd=( void*) x; _temp211->tl= Cyc_current_prologue;
_temp211;}); return Cyc_commands( lexbuf);} case 3: _LL210: { struct _tagged_arr
_temp214= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp214,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("prologue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp214, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp214, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp214; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp215= Cyc_Std_substring(( struct _tagged_arr) _temp214, 0,(
unsigned int)(( t.curr -  _temp214.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp217=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp217->f1=( struct
_tagged_arr) _temp215; _temp217->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp217;}); Cyc_current_prologue=({
struct Cyc_List_List* _temp216=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp216->hd=( void*) x; _temp216->tl= Cyc_current_prologue;
_temp216;}); return Cyc_commands( lexbuf);}}}} case 4: _LL213: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp220=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp220->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp220->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp220;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp219=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp219->hd=( void*) x; _temp219->tl= Cyc_current_epilogue;
_temp219;}); return Cyc_commands( lexbuf);} case 5: _LL218: { struct _tagged_arr
_temp222= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp222,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("epilogue", sizeof(
unsigned char), 9u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp222, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp222, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp222; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp223= Cyc_Std_substring(( struct _tagged_arr) _temp222, 0,(
unsigned int)(( t.curr -  _temp222.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp225=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp225->f1=( struct
_tagged_arr) _temp223; _temp225->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp225;}); Cyc_current_epilogue=({
struct Cyc_List_List* _temp224=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp224->hd=( void*) x; _temp224->tl= Cyc_current_epilogue;
_temp224;}); return Cyc_commands( lexbuf);}}}} case 6: _LL221: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp228=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp228->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp228->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp228;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp227=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp227->hd=( void*) x; _temp227->tl= Cyc_current_cstubs;
_temp227;}); return Cyc_commands( lexbuf);} case 7: _LL226: { struct _tagged_arr
_temp230= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp230,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cstub", sizeof(
unsigned char), 6u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp230, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp230, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp230; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp231= Cyc_Std_substring(( struct _tagged_arr) _temp230, 0,(
unsigned int)(( t.curr -  _temp230.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp233=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp233->f1=( struct
_tagged_arr) _temp231; _temp233->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp233;}); Cyc_current_cstubs=({
struct Cyc_List_List* _temp232=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp232->hd=( void*) x; _temp232->tl= Cyc_current_cstubs;
_temp232;}); return Cyc_commands( lexbuf);}}}} case 8: _LL229: Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp236=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp236->f1=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); _temp236->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp236;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp235=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp235->hd=( void*) x; _temp235->tl= Cyc_current_cycstubs;
_temp235;}); return Cyc_commands( lexbuf);} case 9: _LL234: { struct _tagged_arr
_temp238= Cyc_Lexing_lexeme( lexbuf); _tagged_arr_inplace_plus(& _temp238,
sizeof( unsigned char),( int) Cyc_Std_strlen( _tag_arr("cycstub", sizeof(
unsigned char), 8u))); while( isspace(( int)*(( unsigned char*)
_check_unknown_subscript( _temp238, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& _temp238, sizeof( unsigned char), 1);}{ struct
_tagged_arr t= _temp238; while( ! isspace(( int)*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)))) {
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1);}{ struct
_tagged_arr _temp239= Cyc_Std_substring(( struct _tagged_arr) _temp238, 0,(
unsigned int)(( t.curr -  _temp238.curr) /  sizeof( unsigned char))); Cyc_braces_to_match=
1; Cyc_specbuf=( struct Cyc_Buffer_t*) Cyc_Buffer_create( 255); if( Cyc_block(
lexbuf)){ return 1;}{ struct _tuple10* x=({ struct _tuple10* _temp241=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp241->f1=( struct
_tagged_arr) _temp239; _temp241->f2=( struct _tagged_arr) Cyc_Buffer_contents((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf)); _temp241;}); Cyc_current_cycstubs=({
struct Cyc_List_List* _temp240=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp240->hd=( void*) x; _temp240->tl= Cyc_current_cycstubs;
_temp240;}); return Cyc_commands( lexbuf);}}}} case 10: _LL237: return Cyc_commands(
lexbuf); case 11: _LL242: return 0; case 12: _LL243: return 0; case 13: _LL244:({
struct Cyc_Std_Int_pa_struct _temp247; _temp247.tag= Cyc_Std_Int_pa; _temp247.f1=(
int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void*
_temp246[ 1u]={& _temp247}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof( unsigned char), 58u), _tag_arr( _temp246, sizeof( void*), 1u));}});
return 1; default: _LL245:( lexbuf->refill_buff)( lexbuf); return Cyc_commands_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp249=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp249[ 0]=({ struct Cyc_Lexing_Error_struct _temp250; _temp250.tag= Cyc_Lexing_Error;
_temp250.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp250;}); _temp249;}));} int Cyc_commands( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_commands_rec( lexbuf, 11);} int Cyc_snarfsymbols_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL251: Cyc_snarfed_symbols=({ struct Cyc_List_List* _temp253=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp253->hd=(
void*)({ struct _tagged_arr* _temp254=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp254[ 0]=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); _temp254;}); _temp253->tl= Cyc_snarfed_symbols; _temp253;}); return Cyc_snarfsymbols(
lexbuf); case 1: _LL252: return Cyc_snarfsymbols( lexbuf); case 2: _LL255:
return 0; case 3: _LL256:({ void* _temp258[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n", sizeof( unsigned char),
44u), _tag_arr( _temp258, sizeof( void*), 0u));}); return 1; case 4: _LL257:({
struct Cyc_Std_Int_pa_struct _temp261; _temp261.tag= Cyc_Std_Int_pa; _temp261.f1=(
int)(( unsigned int)(( int) Cyc_Lexing_lexeme_char( lexbuf, 0)));{ void*
_temp260[ 1u]={& _temp261}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof( unsigned char), 57u), _tag_arr( _temp260, sizeof( void*), 1u));}});
return 1; default: _LL259:( lexbuf->refill_buff)( lexbuf); return Cyc_snarfsymbols_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp263=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp263[ 0]=({ struct Cyc_Lexing_Error_struct _temp264; _temp264.tag= Cyc_Lexing_Error;
_temp264.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp264;}); _temp263;}));} int Cyc_snarfsymbols( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_snarfsymbols_rec( lexbuf, 12);} int Cyc_block_rec( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine( lexstate,
lexbuf); switch( lexstate){ case 0: _LL265: if( Cyc_braces_to_match ==  1){
return 0;} Cyc_braces_to_match --; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'}'); return Cyc_block( lexbuf); case 1: _LL266: Cyc_braces_to_match
++; Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'{');
return Cyc_block( lexbuf); case 2: _LL267: Cyc_Buffer_add_char(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf),'"'); Cyc_block_string( lexbuf); return Cyc_block(
lexbuf); case 3: _LL268: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*)
_check_null( Cyc_specbuf), _tag_arr("/*", sizeof( unsigned char), 3u)); Cyc_block_comment(
lexbuf); return Cyc_block( lexbuf); case 4: _LL269: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block( lexbuf); case 5: _LL270: return 0; case 6: _LL271:
Cyc_Buffer_add_char(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf), Cyc_Lexing_lexeme_char(
lexbuf, 0)); return Cyc_block( lexbuf); default: _LL272:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp274=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp274[ 0]=({ struct Cyc_Lexing_Error_struct
_temp275; _temp275.tag= Cyc_Lexing_Error; _temp275.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp275;}); _temp274;}));} int Cyc_block( struct
Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_rec( lexbuf, 13);} int Cyc_block_string_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL276: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),'"'); return 0; case 1: _LL277:
return 1; case 2: _LL278: return 1; case 3: _LL279: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 4: _LL280: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 5: _LL281: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 6: _LL282: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 7: _LL283: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); case 8: _LL284: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf)); return Cyc_block_string( lexbuf); default: _LL285:( lexbuf->refill_buff)(
lexbuf); return Cyc_block_string_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp287=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp287[ 0]=({ struct Cyc_Lexing_Error_struct
_temp288; _temp288.tag= Cyc_Lexing_Error; _temp288.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp288;}); _temp287;}));} int Cyc_block_string(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_block_string_rec( lexbuf, 14);}
int Cyc_block_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL289:
Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),
_tag_arr("*/", sizeof( unsigned char), 3u)); return 0; case 1: _LL290: return 1;
case 2: _LL291: Cyc_Buffer_add_string(( struct Cyc_Buffer_t*) _check_null( Cyc_specbuf),(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf)); return Cyc_block_comment(
lexbuf); default: _LL292:( lexbuf->refill_buff)( lexbuf); return Cyc_block_comment_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp294=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp294[ 0]=({ struct Cyc_Lexing_Error_struct _temp295; _temp295.tag= Cyc_Lexing_Error;
_temp295.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp295;}); _temp294;}));} int Cyc_block_comment( struct Cyc_Lexing_lexbuf*
lexbuf){ return Cyc_block_comment_rec( lexbuf, 15);} static void Cyc_pr( struct
_tagged_arr* sptr){({ struct Cyc_Std_String_pa_struct _temp297; _temp297.tag=
Cyc_Std_String_pa; _temp297.f1=( struct _tagged_arr)* sptr;{ void* _temp296[ 1u]={&
_temp297}; Cyc_Std_printf( _tag_arr(" %s\n", sizeof( unsigned char), 5u),
_tag_arr( _temp296, sizeof( void*), 1u));}});} extern void Cyc_scan_type( void*
t); struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
void Cyc_scan_exp( struct Cyc_Absyn_Exp* e){ void* _temp298=( void*)(( struct
Cyc_Absyn_Exp*) _check_null( e))->r; struct _tuple0* _temp376; struct _tuple0
_temp378; struct _tagged_arr* _temp379; struct _tuple0* _temp381; struct _tuple0
_temp383; struct _tagged_arr* _temp384; struct Cyc_List_List* _temp386; struct
Cyc_Absyn_Exp* _temp388; struct Cyc_Absyn_Exp* _temp390; struct Cyc_Absyn_Exp*
_temp392; struct Cyc_Absyn_Exp* _temp394; struct Cyc_Absyn_Exp* _temp396; struct
Cyc_Absyn_Exp* _temp398; struct Cyc_Absyn_Exp* _temp400; struct Cyc_Absyn_Exp*
_temp402; struct Cyc_Absyn_Exp* _temp404; struct Cyc_Absyn_Exp* _temp406; struct
Cyc_Absyn_Exp* _temp408; struct Cyc_Absyn_Exp* _temp410; struct Cyc_Absyn_Exp*
_temp412; struct Cyc_List_List* _temp414; struct Cyc_Absyn_Exp* _temp416; struct
Cyc_List_List* _temp418; struct Cyc_Absyn_Exp* _temp420; struct Cyc_Absyn_Exp*
_temp422; void* _temp424; void* _temp426; void* _temp428; struct _tagged_arr*
_temp430; struct Cyc_Absyn_Exp* _temp432; struct _tagged_arr* _temp434; struct
Cyc_Absyn_Exp* _temp436; void* _temp438; void* _temp440; struct Cyc_List_List*
_temp442; _LL300: if(*(( int*) _temp298) ==  Cyc_Absyn_Var_e){ _LL377: _temp376=((
struct Cyc_Absyn_Var_e_struct*) _temp298)->f1; _temp378=* _temp376; _LL380:
_temp379= _temp378.f2; goto _LL301;} else{ goto _LL302;} _LL302: if(*(( int*)
_temp298) ==  Cyc_Absyn_UnknownId_e){ _LL382: _temp381=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp298)->f1; _temp383=* _temp381; _LL385: _temp384= _temp383.f2; goto _LL303;}
else{ goto _LL304;} _LL304: if(*(( int*) _temp298) ==  Cyc_Absyn_Primop_e){
_LL387: _temp386=(( struct Cyc_Absyn_Primop_e_struct*) _temp298)->f2; goto
_LL305;} else{ goto _LL306;} _LL306: if(*(( int*) _temp298) ==  Cyc_Absyn_Subscript_e){
_LL391: _temp390=(( struct Cyc_Absyn_Subscript_e_struct*) _temp298)->f1; goto
_LL389; _LL389: _temp388=(( struct Cyc_Absyn_Subscript_e_struct*) _temp298)->f2;
goto _LL307;} else{ goto _LL308;} _LL308: if(*(( int*) _temp298) ==  Cyc_Absyn_SeqExp_e){
_LL395: _temp394=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp298)->f1; goto
_LL393; _LL393: _temp392=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp298)->f2;
goto _LL309;} else{ goto _LL310;} _LL310: if(*(( int*) _temp298) ==  Cyc_Absyn_AssignOp_e){
_LL399: _temp398=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp298)->f1; goto
_LL397; _LL397: _temp396=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp298)->f3;
goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp298) ==  Cyc_Absyn_Deref_e){
_LL401: _temp400=(( struct Cyc_Absyn_Deref_e_struct*) _temp298)->f1; goto _LL313;}
else{ goto _LL314;} _LL314: if(*(( int*) _temp298) ==  Cyc_Absyn_Sizeofexp_e){
_LL403: _temp402=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp298)->f1; goto
_LL315;} else{ goto _LL316;} _LL316: if(*(( int*) _temp298) ==  Cyc_Absyn_Address_e){
_LL405: _temp404=(( struct Cyc_Absyn_Address_e_struct*) _temp298)->f1; goto
_LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp298) ==  Cyc_Absyn_Increment_e){
_LL407: _temp406=(( struct Cyc_Absyn_Increment_e_struct*) _temp298)->f1; goto
_LL319;} else{ goto _LL320;} _LL320: if(*(( int*) _temp298) ==  Cyc_Absyn_Conditional_e){
_LL413: _temp412=(( struct Cyc_Absyn_Conditional_e_struct*) _temp298)->f1; goto
_LL411; _LL411: _temp410=(( struct Cyc_Absyn_Conditional_e_struct*) _temp298)->f2;
goto _LL409; _LL409: _temp408=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp298)->f3; goto _LL321;} else{ goto _LL322;} _LL322: if(*(( int*) _temp298)
==  Cyc_Absyn_FnCall_e){ _LL417: _temp416=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp298)->f1; goto _LL415; _LL415: _temp414=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp298)->f2; goto _LL323;} else{ goto _LL324;} _LL324: if(*(( int*) _temp298)
==  Cyc_Absyn_UnknownCall_e){ _LL421: _temp420=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp298)->f1; goto _LL419; _LL419: _temp418=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp298)->f2; goto _LL325;} else{ goto _LL326;} _LL326: if(*(( int*) _temp298)
==  Cyc_Absyn_Cast_e){ _LL425: _temp424=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp298)->f1; goto _LL423; _LL423: _temp422=(( struct Cyc_Absyn_Cast_e_struct*)
_temp298)->f2; goto _LL327;} else{ goto _LL328;} _LL328: if(*(( int*) _temp298)
==  Cyc_Absyn_Malloc_e){ _LL427: _temp426=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp298)->f2; goto _LL329;} else{ goto _LL330;} _LL330: if(*(( int*) _temp298)
==  Cyc_Absyn_Sizeoftyp_e){ _LL429: _temp428=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp298)->f1; goto _LL331;} else{ goto _LL332;} _LL332: if(*(( int*) _temp298)
==  Cyc_Absyn_StructMember_e){ _LL433: _temp432=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp298)->f1; goto _LL431; _LL431: _temp430=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp298)->f2; goto _LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp298)
==  Cyc_Absyn_StructArrow_e){ _LL437: _temp436=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp298)->f1; goto _LL435; _LL435: _temp434=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp298)->f2; goto _LL335;} else{ goto _LL336;} _LL336: if(*(( int*) _temp298)
==  Cyc_Absyn_Offsetof_e){ _LL441: _temp440=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp298)->f1; goto _LL439; _LL439: _temp438=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp298)->f2; goto _LL337;} else{ goto _LL338;} _LL338: if(*(( int*) _temp298)
==  Cyc_Absyn_Const_e){ goto _LL339;} else{ goto _LL340;} _LL340: if(*(( int*)
_temp298) ==  Cyc_Absyn_UnresolvedMem_e){ _LL443: _temp442=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp298)->f2; goto _LL341;} else{ goto _LL342;} _LL342: if(*(( int*) _temp298)
==  Cyc_Absyn_StmtExp_e){ goto _LL343;} else{ goto _LL344;} _LL344: if(*(( int*)
_temp298) ==  Cyc_Absyn_Throw_e){ goto _LL345;} else{ goto _LL346;} _LL346: if(*((
int*) _temp298) ==  Cyc_Absyn_NoInstantiate_e){ goto _LL347;} else{ goto _LL348;}
_LL348: if(*(( int*) _temp298) ==  Cyc_Absyn_Instantiate_e){ goto _LL349;} else{
goto _LL350;} _LL350: if(*(( int*) _temp298) ==  Cyc_Absyn_New_e){ goto _LL351;}
else{ goto _LL352;} _LL352: if(*(( int*) _temp298) ==  Cyc_Absyn_Gentyp_e){ goto
_LL353;} else{ goto _LL354;} _LL354: if(*(( int*) _temp298) ==  Cyc_Absyn_Tuple_e){
goto _LL355;} else{ goto _LL356;} _LL356: if(*(( int*) _temp298) ==  Cyc_Absyn_CompoundLit_e){
goto _LL357;} else{ goto _LL358;} _LL358: if(*(( int*) _temp298) ==  Cyc_Absyn_Array_e){
goto _LL359;} else{ goto _LL360;} _LL360: if(*(( int*) _temp298) ==  Cyc_Absyn_Comprehension_e){
goto _LL361;} else{ goto _LL362;} _LL362: if(*(( int*) _temp298) ==  Cyc_Absyn_Struct_e){
goto _LL363;} else{ goto _LL364;} _LL364: if(*(( int*) _temp298) ==  Cyc_Absyn_AnonStruct_e){
goto _LL365;} else{ goto _LL366;} _LL366: if(*(( int*) _temp298) ==  Cyc_Absyn_Tunion_e){
goto _LL367;} else{ goto _LL368;} _LL368: if(*(( int*) _temp298) ==  Cyc_Absyn_Enum_e){
goto _LL369;} else{ goto _LL370;} _LL370: if(*(( int*) _temp298) ==  Cyc_Absyn_AnonEnum_e){
goto _LL371;} else{ goto _LL372;} _LL372: if(*(( int*) _temp298) ==  Cyc_Absyn_Codegen_e){
goto _LL373;} else{ goto _LL374;} _LL374: if(*(( int*) _temp298) ==  Cyc_Absyn_Fill_e){
goto _LL375;} else{ goto _LL299;} _LL301: _temp384= _temp379; goto _LL303;
_LL303: Cyc_add_target( _temp384); return; _LL305: for( 0; _temp386 !=  0;
_temp386= _temp386->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_temp386->hd));} return; _LL307: _temp394= _temp390; _temp392= _temp388; goto
_LL309; _LL309: _temp398= _temp394; _temp396= _temp392; goto _LL311; _LL311: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp398); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp396);
return; _LL313: _temp402= _temp400; goto _LL315; _LL315: _temp404= _temp402;
goto _LL317; _LL317: _temp406= _temp404; goto _LL319; _LL319: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp406); return; _LL321: Cyc_scan_exp(( struct Cyc_Absyn_Exp*)
_temp412); Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp410); Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp408); return; _LL323: _temp420= _temp416; _temp418=
_temp414; goto _LL325; _LL325: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp420);
for( 0; _temp418 !=  0; _temp418= _temp418->tl){ Cyc_scan_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _temp418->hd));} return; _LL327: Cyc_scan_type( _temp424);
Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp422); return; _LL329: _temp428=
_temp426; goto _LL331; _LL331: Cyc_scan_type( _temp428); return; _LL333:
_temp436= _temp432; _temp434= _temp430; goto _LL335; _LL335: Cyc_scan_exp((
struct Cyc_Absyn_Exp*) _temp436); Cyc_add_target( _temp434); return; _LL337: Cyc_scan_type(
_temp440);{ void* _temp444= _temp438; struct _tagged_arr* _temp450; _LL446: if(*((
int*) _temp444) ==  Cyc_Absyn_StructField){ _LL451: _temp450=(( struct Cyc_Absyn_StructField_struct*)
_temp444)->f1; goto _LL447;} else{ goto _LL448;} _LL448: if(*(( int*) _temp444)
==  Cyc_Absyn_TupleIndex){ goto _LL449;} else{ goto _LL445;} _LL447: Cyc_add_target(
_temp450); goto _LL445; _LL449: goto _LL445; _LL445:;} return; _LL339: return;
_LL341: for( 0; _temp442 !=  0; _temp442= _temp442->tl){ struct _tuple12
_temp454; struct Cyc_Absyn_Exp* _temp455; struct _tuple12* _temp452=( struct
_tuple12*) _temp442->hd; _temp454=* _temp452; _LL456: _temp455= _temp454.f2;
goto _LL453; _LL453: Cyc_scan_exp(( struct Cyc_Absyn_Exp*) _temp455);} return;
_LL343:({ void* _temp457[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Stmt_e\n",
sizeof( unsigned char), 26u), _tag_arr( _temp457, sizeof( void*), 0u));}); exit(
1); return; _LL345:({ void* _temp458[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Throw_e\n", sizeof( unsigned char), 27u), _tag_arr(
_temp458, sizeof( void*), 0u));}); exit( 1); return; _LL347:({ void* _temp459[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected NoInstantiate_e\n",
sizeof( unsigned char), 35u), _tag_arr( _temp459, sizeof( void*), 0u));}); exit(
1); return; _LL349:({ void* _temp460[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Instantiate_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp460, sizeof( void*), 0u));}); exit( 1); return; _LL351:({ void*
_temp461[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected New_e\n",
sizeof( unsigned char), 25u), _tag_arr( _temp461, sizeof( void*), 0u));}); exit(
1); return; _LL353:({ void* _temp462[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Gentyp_e\n", sizeof( unsigned char), 28u), _tag_arr(
_temp462, sizeof( void*), 0u));}); exit( 1); return; _LL355:({ void* _temp463[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tuple_e\n", sizeof(
unsigned char), 27u), _tag_arr( _temp463, sizeof( void*), 0u));}); exit( 1);
return; _LL357:({ void* _temp464[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected CompoundLit_e\n", sizeof( unsigned char), 33u),
_tag_arr( _temp464, sizeof( void*), 0u));}); exit( 1); return; _LL359:({ void*
_temp465[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Array_e\n",
sizeof( unsigned char), 27u), _tag_arr( _temp465, sizeof( void*), 0u));}); exit(
1); return; _LL361:({ void* _temp466[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Comprehension_e\n", sizeof( unsigned char), 35u),
_tag_arr( _temp466, sizeof( void*), 0u));}); exit( 1); return; _LL363:({ void*
_temp467[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Struct_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp467, sizeof( void*), 0u));}); exit(
1); return; _LL365:({ void* _temp468[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected AnonStruct_e\n", sizeof( unsigned char), 32u),
_tag_arr( _temp468, sizeof( void*), 0u));}); exit( 1); return; _LL367:({ void*
_temp469[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Tunion_e\n",
sizeof( unsigned char), 28u), _tag_arr( _temp469, sizeof( void*), 0u));}); exit(
1); return; _LL369:({ void* _temp470[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Enum_e\n", sizeof( unsigned char), 26u), _tag_arr(
_temp470, sizeof( void*), 0u));}); exit( 1); return; _LL371:({ void* _temp471[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AnonEnum_e\n",
sizeof( unsigned char), 30u), _tag_arr( _temp471, sizeof( void*), 0u));}); exit(
1); return; _LL373:({ void* _temp472[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected Codegen_e\n", sizeof( unsigned char), 29u), _tag_arr(
_temp472, sizeof( void*), 0u));}); exit( 1); return; _LL375:({ void* _temp473[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected Fill_e\n", sizeof(
unsigned char), 26u), _tag_arr( _temp473, sizeof( void*), 0u));}); exit( 1);
return; _LL299:;} void Cyc_scan_exp_opt( struct Cyc_Absyn_Exp* eo){ if((
unsigned int) eo){ Cyc_scan_exp( eo);} return;} void Cyc_scan_type( void* t){
void* _temp474= t; struct Cyc_Absyn_PtrInfo _temp528; struct Cyc_Absyn_Exp*
_temp530; void* _temp532; struct Cyc_Absyn_FnInfo _temp534; struct Cyc_List_List*
_temp536; struct Cyc_List_List* _temp538; struct _tuple0* _temp540; struct
_tuple0* _temp542; struct _tuple0* _temp544; struct _tuple0 _temp546; struct
_tagged_arr* _temp547; struct _tuple0* _temp549; struct _tuple0 _temp551; struct
_tagged_arr* _temp552; struct _tuple0* _temp554; struct _tuple0 _temp556; struct
_tagged_arr* _temp557; struct _tuple0* _temp559; struct _tuple0 _temp561; struct
_tagged_arr* _temp562; _LL476: if( _temp474 == ( void*) Cyc_Absyn_VoidType){
goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp474 >  4u?*((
int*) _temp474) ==  Cyc_Absyn_IntType: 0){ goto _LL479;} else{ goto _LL480;}
_LL480: if( _temp474 == ( void*) Cyc_Absyn_FloatType){ goto _LL481;} else{ goto
_LL482;} _LL482: if( _temp474 == ( void*) Cyc_Absyn_DoubleType){ goto _LL483;}
else{ goto _LL484;} _LL484: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474)
==  Cyc_Absyn_PointerType: 0){ _LL529: _temp528=(( struct Cyc_Absyn_PointerType_struct*)
_temp474)->f1; goto _LL485;} else{ goto _LL486;} _LL486: if(( unsigned int)
_temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_ArrayType: 0){ _LL533: _temp532=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp474)->f1; goto _LL531; _LL531:
_temp530=(( struct Cyc_Absyn_ArrayType_struct*) _temp474)->f3; goto _LL487;}
else{ goto _LL488;} _LL488: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474)
==  Cyc_Absyn_FnType: 0){ _LL535: _temp534=(( struct Cyc_Absyn_FnType_struct*)
_temp474)->f1; goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int)
_temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_AnonStructType: 0){ _LL537:
_temp536=(( struct Cyc_Absyn_AnonStructType_struct*) _temp474)->f1; goto _LL491;}
else{ goto _LL492;} _LL492: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474)
==  Cyc_Absyn_AnonUnionType: 0){ _LL539: _temp538=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp474)->f1; goto _LL493;} else{ goto _LL494;} _LL494: if(( unsigned int)
_temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL495;}
else{ goto _LL496;} _LL496: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474)
==  Cyc_Absyn_StructType: 0){ _LL541: _temp540=(( struct Cyc_Absyn_StructType_struct*)
_temp474)->f1; if( _temp540 ==  0){ goto _LL497;} else{ goto _LL498;}} else{
goto _LL498;} _LL498: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474) == 
Cyc_Absyn_UnionType: 0){ _LL543: _temp542=(( struct Cyc_Absyn_UnionType_struct*)
_temp474)->f1; if( _temp542 ==  0){ goto _LL499;} else{ goto _LL500;}} else{
goto _LL500;} _LL500: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474) == 
Cyc_Absyn_StructType: 0){ _LL545: _temp544=(( struct Cyc_Absyn_StructType_struct*)
_temp474)->f1; if( _temp544 ==  0){ goto _LL502;} else{ _temp546=* _temp544;
_LL548: _temp547= _temp546.f2; goto _LL501;}} else{ goto _LL502;} _LL502: if((
unsigned int) _temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_UnionType: 0){
_LL550: _temp549=(( struct Cyc_Absyn_UnionType_struct*) _temp474)->f1; if(
_temp549 ==  0){ goto _LL504;} else{ _temp551=* _temp549; _LL553: _temp552=
_temp551.f2; goto _LL503;}} else{ goto _LL504;} _LL504: if(( unsigned int)
_temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_EnumType: 0){ _LL555: _temp554=((
struct Cyc_Absyn_EnumType_struct*) _temp474)->f1; _temp556=* _temp554; _LL558:
_temp557= _temp556.f2; goto _LL505;} else{ goto _LL506;} _LL506: if((
unsigned int) _temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_TypedefType: 0){
_LL560: _temp559=(( struct Cyc_Absyn_TypedefType_struct*) _temp474)->f1;
_temp561=* _temp559; _LL563: _temp562= _temp561.f2; goto _LL507;} else{ goto
_LL508;} _LL508: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_Evar:
0){ goto _LL509;} else{ goto _LL510;} _LL510: if(( unsigned int) _temp474 >  4u?*((
int*) _temp474) ==  Cyc_Absyn_VarType: 0){ goto _LL511;} else{ goto _LL512;}
_LL512: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_TunionType:
0){ goto _LL513;} else{ goto _LL514;} _LL514: if(( unsigned int) _temp474 >  4u?*((
int*) _temp474) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL515;} else{ goto
_LL516;} _LL516: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_TupleType:
0){ goto _LL517;} else{ goto _LL518;} _LL518: if(( unsigned int) _temp474 >  4u?*((
int*) _temp474) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL519;} else{ goto _LL520;}
_LL520: if( _temp474 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL521;} else{ goto
_LL522;} _LL522: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_AccessEff:
0){ goto _LL523;} else{ goto _LL524;} _LL524: if(( unsigned int) _temp474 >  4u?*((
int*) _temp474) ==  Cyc_Absyn_JoinEff: 0){ goto _LL525;} else{ goto _LL526;}
_LL526: if(( unsigned int) _temp474 >  4u?*(( int*) _temp474) ==  Cyc_Absyn_RgnsEff:
0){ goto _LL527;} else{ goto _LL475;} _LL477: goto _LL479; _LL479: goto _LL481;
_LL481: goto _LL483; _LL483: return; _LL485: Cyc_scan_type(( void*) _temp528.elt_typ);
return; _LL487: Cyc_scan_type( _temp532); Cyc_scan_exp_opt( _temp530); return;
_LL489: Cyc_scan_type(( void*) _temp534.ret_typ);{ struct Cyc_List_List*
_temp564= _temp534.args; for( 0; _temp564 !=  0; _temp564= _temp564->tl){ struct
_tuple1 _temp567; void* _temp568; struct _tuple1* _temp565=( struct _tuple1*)
_temp564->hd; _temp567=* _temp565; _LL569: _temp568= _temp567.f3; goto _LL566;
_LL566: Cyc_scan_type( _temp568);}} if( _temp534.cyc_varargs !=  0){ Cyc_scan_type((
void*)( _temp534.cyc_varargs)->type);} return; _LL491: _temp538= _temp536; goto
_LL493; _LL493: for( 0; _temp538 !=  0; _temp538= _temp538->tl){ Cyc_scan_type((
void*)(( struct Cyc_Absyn_Structfield*) _temp538->hd)->type); Cyc_scan_exp_opt(((
struct Cyc_Absyn_Structfield*) _temp538->hd)->width);} return; _LL495: return;
_LL497: return; _LL499: return; _LL501: _temp552= _temp547; goto _LL503; _LL503:
_temp557= _temp552; goto _LL505; _LL505: _temp562= _temp557; goto _LL507; _LL507:
Cyc_add_target( _temp562); return; _LL509:({ void* _temp570[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: unexpected Evar\n", sizeof( unsigned char), 24u),
_tag_arr( _temp570, sizeof( void*), 0u));}); exit( 1); return; _LL511:({ void*
_temp571[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected VarType\n",
sizeof( unsigned char), 27u), _tag_arr( _temp571, sizeof( void*), 0u));}); exit(
1); return; _LL513:({ void* _temp572[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionType\n", sizeof( unsigned char), 30u),
_tag_arr( _temp572, sizeof( void*), 0u));}); exit( 1); return; _LL515:({ void*
_temp573[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected TunionFieldType\n",
sizeof( unsigned char), 35u), _tag_arr( _temp573, sizeof( void*), 0u));}); exit(
1); return; _LL517:({ void* _temp574[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n", sizeof( unsigned char), 29u), _tag_arr(
_temp574, sizeof( void*), 0u));}); exit( 1); return; _LL519:({ void* _temp575[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnHandleType\n",
sizeof( unsigned char), 33u), _tag_arr( _temp575, sizeof( void*), 0u));}); exit(
1); return; _LL521:({ void* _temp576[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected HeapRgn\n", sizeof( unsigned char), 27u), _tag_arr(
_temp576, sizeof( void*), 0u));}); exit( 1); return; _LL523:({ void* _temp577[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected AccessEff\n",
sizeof( unsigned char), 29u), _tag_arr( _temp577, sizeof( void*), 0u));}); exit(
1); return; _LL525:({ void* _temp578[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected JoinEff\n", sizeof( unsigned char), 27u), _tag_arr(
_temp578, sizeof( void*), 0u));}); exit( 1); return; _LL527:({ void* _temp579[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected RgnsEff\n", sizeof(
unsigned char), 27u), _tag_arr( _temp579, sizeof( void*), 0u));}); exit( 1);
return; _LL475:;} struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; struct _tuple8* Cyc_scan_decl( struct Cyc_Absyn_Decl* d){ Cyc_current_targets=({
struct Cyc_Set_Set** _temp580=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp580[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_temp580;});{ void* _temp581=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp607;
struct Cyc_Absyn_Fndecl* _temp609; struct Cyc_Absyn_Structdecl* _temp611; struct
Cyc_Absyn_Uniondecl* _temp613; struct Cyc_Absyn_Enumdecl* _temp615; struct Cyc_Absyn_Typedefdecl*
_temp617; _LL583: if(*(( int*) _temp581) ==  Cyc_Absyn_Var_d){ _LL608: _temp607=((
struct Cyc_Absyn_Var_d_struct*) _temp581)->f1; goto _LL584;} else{ goto _LL585;}
_LL585: if(*(( int*) _temp581) ==  Cyc_Absyn_Fn_d){ _LL610: _temp609=(( struct
Cyc_Absyn_Fn_d_struct*) _temp581)->f1; goto _LL586;} else{ goto _LL587;} _LL587:
if(*(( int*) _temp581) ==  Cyc_Absyn_Struct_d){ _LL612: _temp611=(( struct Cyc_Absyn_Struct_d_struct*)
_temp581)->f1; goto _LL588;} else{ goto _LL589;} _LL589: if(*(( int*) _temp581)
==  Cyc_Absyn_Union_d){ _LL614: _temp613=(( struct Cyc_Absyn_Union_d_struct*)
_temp581)->f1; goto _LL590;} else{ goto _LL591;} _LL591: if(*(( int*) _temp581)
==  Cyc_Absyn_Enum_d){ _LL616: _temp615=(( struct Cyc_Absyn_Enum_d_struct*)
_temp581)->f1; goto _LL592;} else{ goto _LL593;} _LL593: if(*(( int*) _temp581)
==  Cyc_Absyn_Typedef_d){ _LL618: _temp617=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp581)->f1; goto _LL594;} else{ goto _LL595;} _LL595: if(*(( int*) _temp581)
==  Cyc_Absyn_Let_d){ goto _LL596;} else{ goto _LL597;} _LL597: if(*(( int*)
_temp581) ==  Cyc_Absyn_Tunion_d){ goto _LL598;} else{ goto _LL599;} _LL599: if(*((
int*) _temp581) ==  Cyc_Absyn_Letv_d){ goto _LL600;} else{ goto _LL601;} _LL601:
if(*(( int*) _temp581) ==  Cyc_Absyn_Namespace_d){ goto _LL602;} else{ goto
_LL603;} _LL603: if(*(( int*) _temp581) ==  Cyc_Absyn_Using_d){ goto _LL604;}
else{ goto _LL605;} _LL605: if(*(( int*) _temp581) ==  Cyc_Absyn_ExternC_d){
goto _LL606;} else{ goto _LL582;} _LL584: { struct _tuple0 _temp621; struct
_tagged_arr* _temp622; struct _tuple0* _temp619= _temp607->name; _temp621=*
_temp619; _LL623: _temp622= _temp621.f2; goto _LL620; _LL620: Cyc_current_source=(
struct _tagged_arr*) _temp622; Cyc_scan_type(( void*) _temp607->type); Cyc_scan_exp_opt(
_temp607->initializer); goto _LL582;} _LL586: { struct _tuple0 _temp626; struct
_tagged_arr* _temp627; struct _tuple0* _temp624= _temp609->name; _temp626=*
_temp624; _LL628: _temp627= _temp626.f2; goto _LL625; _LL625: Cyc_current_source=(
struct _tagged_arr*) _temp627; Cyc_scan_type(( void*) _temp609->ret_type);{
struct Cyc_List_List* _temp629= _temp609->args; for( 0; _temp629 !=  0; _temp629=
_temp629->tl){ struct _tuple13 _temp632; void* _temp633; struct _tuple13*
_temp630=( struct _tuple13*) _temp629->hd; _temp632=* _temp630; _LL634: _temp633=
_temp632.f3; goto _LL631; _LL631: Cyc_scan_type( _temp633);}} if( _temp609->cyc_varargs
!=  0){ Cyc_scan_type(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp609->cyc_varargs))->type);} if( _temp609->is_inline){({ void* _temp635[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: ignoring inline function\n",
sizeof( unsigned char), 35u), _tag_arr( _temp635, sizeof( void*), 0u));});} goto
_LL582;} _LL588: if( !(( unsigned int) _temp611->name)){ return 0;}{ struct
_tuple0 _temp638; struct _tagged_arr* _temp639; struct _tuple0* _temp636=(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp611->name))->v;
_temp638=* _temp636; _LL640: _temp639= _temp638.f2; goto _LL637; _LL637: Cyc_current_source=(
struct _tagged_arr*) _temp639; if(( unsigned int) _temp611->fields){{ struct Cyc_List_List*
_temp641=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp611->fields))->v;
for( 0; _temp641 !=  0; _temp641= _temp641->tl){ struct Cyc_Absyn_Structfield*
_temp642=( struct Cyc_Absyn_Structfield*) _temp641->hd; Cyc_scan_type(( void*)
_temp642->type); Cyc_scan_exp_opt( _temp642->width);}}{ struct Cyc_List_List*
_temp643=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp611->fields))->v;
for( 0; _temp643 !=  0; _temp643= _temp643->tl){;}}} goto _LL582;} _LL590: if( !((
unsigned int) _temp613->name)){ return 0;}{ struct _tuple0 _temp646; struct
_tagged_arr* _temp647; struct _tuple0* _temp644=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp613->name))->v; _temp646=* _temp644; _LL648: _temp647=
_temp646.f2; goto _LL645; _LL645: Cyc_current_source=( struct _tagged_arr*)
_temp647; if(( unsigned int) _temp613->fields){{ struct Cyc_List_List* _temp649=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp613->fields))->v;
for( 0; _temp649 !=  0; _temp649= _temp649->tl){ struct Cyc_Absyn_Structfield*
_temp650=( struct Cyc_Absyn_Structfield*) _temp649->hd; Cyc_add_target( _temp650->name);
Cyc_scan_type(( void*) _temp650->type); Cyc_scan_exp_opt( _temp650->width);}}{
struct Cyc_List_List* _temp651=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp613->fields))->v; for( 0; _temp651 !=  0; _temp651= _temp651->tl){;}}}
goto _LL582;} _LL592: { struct _tuple0 _temp654; struct _tagged_arr* _temp655;
struct _tuple0* _temp652= _temp615->name; _temp654=* _temp652; _LL656: _temp655=
_temp654.f2; goto _LL653; _LL653: Cyc_current_source=( struct _tagged_arr*)
_temp655; if(( unsigned int) _temp615->fields){{ struct Cyc_List_List* _temp657=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp615->fields))->v;
for( 0; _temp657 !=  0; _temp657= _temp657->tl){ struct Cyc_Absyn_Enumfield*
_temp658=( struct Cyc_Absyn_Enumfield*) _temp657->hd; Cyc_scan_exp_opt( _temp658->tag);}}{
struct Cyc_List_List* _temp659=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp615->fields))->v; for( 0; _temp659 !=  0; _temp659= _temp659->tl){;}}}
goto _LL582;} _LL594: { struct _tuple0 _temp662; struct _tagged_arr* _temp663;
struct _tuple0* _temp660= _temp617->name; _temp662=* _temp660; _LL664: _temp663=
_temp662.f2; goto _LL661; _LL661: Cyc_current_source=( struct _tagged_arr*)
_temp663; Cyc_scan_type(( void*) _temp617->defn); goto _LL582;} _LL596:({ void*
_temp665[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp665, sizeof( void*), 0u));}); exit(
1); return 0; _LL598:({ void* _temp666[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected tunion declaration\n", sizeof( unsigned char), 38u),
_tag_arr( _temp666, sizeof( void*), 0u));}); exit( 1); return 0; _LL600:({ void*
_temp667[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected let declaration\n",
sizeof( unsigned char), 35u), _tag_arr( _temp667, sizeof( void*), 0u));}); exit(
1); return 0; _LL602:({ void* _temp668[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected namespace declaration\n", sizeof( unsigned char), 41u),
_tag_arr( _temp668, sizeof( void*), 0u));}); exit( 1); return 0; _LL604:({ void*
_temp669[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: unexpected using declaration\n",
sizeof( unsigned char), 37u), _tag_arr( _temp669, sizeof( void*), 0u));}); exit(
1); return 0; _LL606:({ void* _temp670[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n", sizeof( unsigned char),
42u), _tag_arr( _temp670, sizeof( void*), 0u));}); exit( 1); return 0; _LL582:;}
return({ struct _tuple8* _temp671=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp671->f1=( struct _tagged_arr*) _check_null( Cyc_current_source);
_temp671->f2=*(( struct Cyc_Set_Set**) _check_null( Cyc_current_targets));
_temp671;});} struct Cyc_Hashtable_Table* Cyc_new_deps(){ return(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), int(* hash)(
struct _tagged_arr*))) Cyc_Hashtable_create)( 107, Cyc_Std_strptrcmp, Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set* Cyc_find( struct Cyc_Hashtable_Table* t, struct _tagged_arr*
x){ struct _handler_cons _temp672; _push_handler(& _temp672);{ int _temp674= 0;
if( setjmp( _temp672.handler)){ _temp674= 1;} if( ! _temp674){{ struct Cyc_Set_Set*
_temp675=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key)) Cyc_Hashtable_lookup)( t, x); _npop_handler( 0u); return
_temp675;}; _pop_handler();} else{ void* _temp673=( void*) _exn_thrown; void*
_temp677= _temp673; _LL679: if( _temp677 ==  Cyc_Core_Not_found){ goto _LL680;}
else{ goto _LL681;} _LL681: goto _LL682; _LL680: return(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);
_LL682:( void) _throw( _temp677); _LL678:;}}} struct Cyc_Set_Set* Cyc_reachable(
struct Cyc_List_List* init, struct Cyc_Hashtable_Table* t){ struct Cyc_Set_Set*
emptyset=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); struct Cyc_Set_Set* curr;
for( curr= emptyset; init !=  0; init= init->tl){ curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( curr,( struct
_tagged_arr*) init->hd);}{ struct Cyc_Set_Set* _temp683= curr; while((( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp683) >  0) { struct Cyc_Set_Set*
_temp684= emptyset;{ struct Cyc_List_List* _temp685=(( struct Cyc_List_List*(*)(
struct Cyc_Set_Set* s)) Cyc_Set_elements)( _temp683); for( 0; _temp685 !=  0;
_temp685= _temp685->tl){ _temp684=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( _temp684, Cyc_find( t,( struct
_tagged_arr*) _temp685->hd));}} _temp683=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_diff)( _temp684, curr); curr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( curr,
_temp683);} return curr;}} int Cyc_force_directory( struct _tagged_arr d){ int
_temp686=({ unsigned int _temp689[ 0u]; Cyc_Std_open( d, 0, _tag_arr( _temp689,
sizeof( unsigned int), 0u));}); if( _temp686 ==  - 1){ if( Cyc_Std_mkdir( d, 448)
==  - 1){({ struct Cyc_Std_String_pa_struct _temp688; _temp688.tag= Cyc_Std_String_pa;
_temp688.f1=( struct _tagged_arr) d;{ void* _temp687[ 1u]={& _temp688}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n", sizeof(
unsigned char), 38u), _tag_arr( _temp687, sizeof( void*), 1u));}}); return 1;}}
else{ close( _temp686);} return 0;} int Cyc_force_directory_prefixes( struct
_tagged_arr file){ struct _tagged_arr _temp690= Cyc_Std_strdup( file); struct
Cyc_List_List* _temp691= 0; while( 1) { _temp690= Cyc_Filename_dirname(( struct
_tagged_arr) _temp690); if( _get_arr_size( _temp690, sizeof( unsigned char)) == 
0){ break;} _temp691=({ struct Cyc_List_List* _temp692=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp692->hd=( void*)({ struct
_tagged_arr* _temp693=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp693[ 0]=( struct _tagged_arr) _temp690; _temp693;});
_temp692->tl= _temp691; _temp692;});} for( 0; _temp691 !=  0; _temp691= _temp691->tl){
if( Cyc_force_directory(*(( struct _tagged_arr*) _temp691->hd))){ return 1;}}
return 0;} int Cyc_process_file( struct _tagged_arr filename, struct _tagged_arr
dir, struct Cyc_List_List* start_symbols, struct Cyc_List_List* omit_symbols,
struct Cyc_List_List* prologue, struct Cyc_List_List* epilogue, struct Cyc_List_List*
cstubs, struct Cyc_List_List* cycstubs){ struct Cyc_Std___sFILE* maybe; struct
Cyc_Std___sFILE* in_file; struct Cyc_Std___sFILE* out_file;({ struct Cyc_Std_String_pa_struct
_temp695; _temp695.tag= Cyc_Std_String_pa; _temp695.f1=( struct _tagged_arr)
filename;{ void* _temp694[ 1u]={& _temp695}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("********************************* %s...\n", sizeof( unsigned char), 41u),
_tag_arr( _temp694, sizeof( void*), 1u));}});{ struct _tagged_arr buf= _tag_arr(({
unsigned int _temp997=( unsigned int) 1024; unsigned char* _temp998=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char), _temp997));{
unsigned int _temp999= _temp997; unsigned int i; for( i= 0; i <  _temp999; i ++){
_temp998[ i]='\000';}}; _temp998;}), sizeof( unsigned char),( unsigned int) 1024);
struct _tagged_arr _temp696= Cyc_Std_getcwd( buf, _get_arr_size( buf, sizeof(
unsigned char))); if( Cyc_Std_chdir( dir)){({ struct Cyc_Std_String_pa_struct
_temp698; _temp698.tag= Cyc_Std_String_pa; _temp698.f1=( struct _tagged_arr) dir;{
void* _temp697[ 1u]={& _temp698}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: can't change directory to %s\n",
sizeof( unsigned char), 37u), _tag_arr( _temp697, sizeof( void*), 1u));}});
return 1;}({ struct Cyc_Std_String_pa_struct _temp700; _temp700.tag= Cyc_Std_String_pa;
_temp700.f1=( struct _tagged_arr) filename;{ void* _temp699[ 1u]={& _temp700};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_log_file), _tag_arr("\n%s:\n",
sizeof( unsigned char), 6u), _tag_arr( _temp699, sizeof( void*), 1u));}});{
struct _tagged_arr _temp701= Cyc_Filename_basename( filename); struct
_tagged_arr _temp702= Cyc_Filename_chop_extension(( struct _tagged_arr) _temp701);
struct _tagged_arr _temp703= Cyc_Std_strconcat(( struct _tagged_arr) _temp702,
_tag_arr(".i", sizeof( unsigned char), 3u)); struct _tagged_arr _temp704= Cyc_Std_strconcat((
struct _tagged_arr) _temp702, _tag_arr(".iX", sizeof( unsigned char), 4u));
struct _tagged_arr _temp705= Cyc_Filename_dirname( filename); struct _tagged_arr
_temp706= _get_arr_size( _temp705, sizeof( unsigned char)) ==  0?({ struct Cyc_Std_String_pa_struct
_temp994; _temp994.tag= Cyc_Std_String_pa; _temp994.f1=( struct _tagged_arr)
_temp701;{ void* _temp993[ 1u]={& _temp994}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp993, sizeof( void*), 1u));}}): Cyc_Filename_concat((
struct _tagged_arr) _temp705,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp996; _temp996.tag= Cyc_Std_String_pa; _temp996.f1=( struct _tagged_arr)
_temp701;{ void* _temp995[ 1u]={& _temp996}; Cyc_Std_aprintf( _tag_arr("c%s",
sizeof( unsigned char), 4u), _tag_arr( _temp995, sizeof( void*), 1u));}})); if(
Cyc_force_directory_prefixes( filename)){ return 1;}{ struct Cyc_Hashtable_Table*
t= Cyc_new_deps(); struct _tagged_arr _temp707=({ struct Cyc_Std_String_pa_struct
_temp992; _temp992.tag= Cyc_Std_String_pa; _temp992.f1=( struct _tagged_arr)
_temp703;{ struct Cyc_Std_String_pa_struct _temp991; _temp991.tag= Cyc_Std_String_pa;
_temp991.f1=( struct _tagged_arr) filename;{ void* _temp990[ 2u]={& _temp991,&
_temp992}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp990, sizeof( void*), 2u));}}}); if(
Cyc_Std_system(( struct _tagged_arr) _temp707) !=  0){ maybe= Cyc_Std_fopen(
filename, _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int)
maybe)){({ struct Cyc_Std_String_pa_struct _temp709; _temp709.tag= Cyc_Std_String_pa;
_temp709.f1=( struct _tagged_arr) filename;{ void* _temp708[ 1u]={& _temp709};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create file %s\n",
sizeof( unsigned char), 33u), _tag_arr( _temp708, sizeof( void*), 1u));}});
return 1;} out_file=( struct Cyc_Std___sFILE*) _check_null( maybe);({ struct Cyc_Std_String_pa_struct
_temp711; _temp711.tag= Cyc_Std_String_pa; _temp711.f1=( struct _tagged_arr)
filename;{ void* _temp710[ 1u]={& _temp711}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp710, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp706, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){({ struct Cyc_Std_String_pa_struct
_temp713; _temp713.tag= Cyc_Std_String_pa; _temp713.f1=( struct _tagged_arr)
_temp706;{ void* _temp712[ 1u]={& _temp713}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n", sizeof( unsigned char), 33u),
_tag_arr( _temp712, sizeof( void*), 1u));}}); return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp715; _temp715.tag=
Cyc_Std_String_pa; _temp715.f1=( struct _tagged_arr) _temp706;{ void* _temp714[
1u]={& _temp715}; Cyc_Std_fprintf( out_file, _tag_arr("#error -- %s is not supported on this platform\n",
sizeof( unsigned char), 48u), _tag_arr( _temp714, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file);({ struct Cyc_Std_String_pa_struct _temp717; _temp717.tag= Cyc_Std_String_pa;
_temp717.f1=( struct _tagged_arr) filename;{ void* _temp716[ 1u]={& _temp717};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Warning: %s will not be supported on this platform\n",
sizeof( unsigned char), 52u), _tag_arr( _temp716, sizeof( void*), 1u));}});({
void* _temp718[ 0u]={}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null(
Cyc_log_file), _tag_arr("Not supported on this platform\n", sizeof(
unsigned char), 32u), _tag_arr( _temp718, sizeof( void*), 0u));}); if( Cyc_Std_chdir((
struct _tagged_arr) _temp696)){({ struct Cyc_Std_String_pa_struct _temp720;
_temp720.tag= Cyc_Std_String_pa; _temp720.f1=( struct _tagged_arr) _temp696;{
void* _temp719[ 1u]={& _temp720}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not change directory to %s\n",
sizeof( unsigned char), 41u), _tag_arr( _temp719, sizeof( void*), 1u));}});
return 1;} return 0;} maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp703,
_tag_arr("r", sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){
return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null( maybe);{ struct Cyc_Lexing_lexbuf*
_temp721= Cyc_Lexing_from_file( in_file); struct _tuple8* entry; while(( entry=((
struct _tuple8*(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_line)( _temp721)) != 
0) { struct _tuple8 _temp724; struct Cyc_Set_Set* _temp725; struct _tagged_arr*
_temp727; struct _tuple8* _temp722=( struct _tuple8*) _check_null( entry);
_temp724=* _temp722; _LL728: _temp727= _temp724.f1; goto _LL726; _LL726:
_temp725= _temp724.f2; goto _LL723; _LL723:(( void(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_arr* key, struct Cyc_Set_Set* val)) Cyc_Hashtable_insert)( t,
_temp727, _temp725);} Cyc_Std_fclose( in_file); _temp707=({ struct Cyc_Std_String_pa_struct
_temp731; _temp731.tag= Cyc_Std_String_pa; _temp731.f1=( struct _tagged_arr)
_temp703;{ struct Cyc_Std_String_pa_struct _temp730; _temp730.tag= Cyc_Std_String_pa;
_temp730.f1=( struct _tagged_arr) filename;{ void* _temp729[ 2u]={& _temp730,&
_temp731}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -o %s -",
sizeof( unsigned char), 38u), _tag_arr( _temp729, sizeof( void*), 2u));}}}); if(
Cyc_Std_system(( struct _tagged_arr) _temp707) !=  0){ return 1;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp703, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp721= Cyc_Lexing_from_file( in_file); Cyc_slurp_out= Cyc_Std_fopen((
struct _tagged_arr) _temp704, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) Cyc_slurp_out)){ return 1;} if((( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_slurp)( _temp721)){({ struct Cyc_Std_String_pa_struct _temp733;
_temp733.tag= Cyc_Std_String_pa; _temp733.f1=( struct _tagged_arr) filename;{
void* _temp732[ 1u]={& _temp733}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_log_file), _tag_arr("Error: Unclosed string in %s\n", sizeof(
unsigned char), 30u), _tag_arr( _temp732, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp735; _temp735.tag= Cyc_Std_String_pa; _temp735.f1=( struct _tagged_arr)
filename;{ void* _temp734[ 1u]={& _temp735}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: Unclosed string in %s\n", sizeof( unsigned char), 30u),
_tag_arr( _temp734, sizeof( void*), 1u));}}); Cyc_Std_fclose( in_file); Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_slurp_out)); Cyc_Std_remove(( struct
_tagged_arr) _temp703); Cyc_Std_remove(( struct _tagged_arr) _temp704); return 1;}
Cyc_Std_fclose( in_file); Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null(
Cyc_slurp_out)); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp704, _tag_arr("r",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} in_file=(
struct Cyc_Std___sFILE*) _check_null( maybe); Cyc_Position_reset_position((
struct _tagged_arr) _temp704); Cyc_Lex_lex_init();{ struct Cyc_List_List*
_temp736= Cyc_Parse_parse_file( in_file); Cyc_Lex_lex_init(); Cyc_Std_fclose(
in_file);{ struct Cyc_List_List* _temp737= _temp736; for( 0; _temp737 !=  0;
_temp737= _temp737->tl){ struct _tuple8* _temp738= Cyc_scan_decl(( struct Cyc_Absyn_Decl*)
_temp737->hd); if( _temp738 ==  0){ continue;}{ struct _tuple8 _temp741; struct
Cyc_Set_Set* _temp742; struct _tagged_arr* _temp744; struct _tuple8* _temp739=(
struct _tuple8*) _check_null( _temp738); _temp741=* _temp739; _LL745: _temp744=
_temp741.f1; goto _LL743; _LL743: _temp742= _temp741.f2; goto _LL740; _LL740: {
struct Cyc_Set_Set* old;{ struct _handler_cons _temp746; _push_handler(&
_temp746);{ int _temp748= 0; if( setjmp( _temp746.handler)){ _temp748= 1;} if( !
_temp748){ old=(( struct Cyc_Set_Set*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key)) Cyc_Hashtable_lookup)( t, _temp744);; _pop_handler();} else{
void* _temp747=( void*) _exn_thrown; void* _temp750= _temp747; _LL752: if(
_temp750 ==  Cyc_Core_Not_found){ goto _LL753;} else{ goto _LL754;} _LL754: goto
_LL755; _LL753: old=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp); goto _LL751; _LL755:(
void) _throw( _temp750); _LL751:;}}}(( void(*)( struct Cyc_Hashtable_Table* t,
struct _tagged_arr* key, struct Cyc_Set_Set* val)) Cyc_Hashtable_insert)( t,
_temp744,(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( _temp742, old));}}}}{ struct Cyc_Set_Set* _temp756= Cyc_reachable(
start_symbols, t); struct Cyc_List_List* _temp757= 0; struct Cyc_Set_Set*
defined_symbols=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_strptrcmp);{ struct Cyc_List_List*
_temp758= _temp736; for( 0; _temp758 !=  0; _temp758= _temp758->tl){ struct Cyc_Absyn_Decl*
_temp759=( struct Cyc_Absyn_Decl*) _temp758->hd; struct _tagged_arr* name;{ void*
_temp760=( void*) _temp759->r; struct Cyc_Absyn_Vardecl* _temp786; struct Cyc_Absyn_Fndecl*
_temp788; struct Cyc_Absyn_Structdecl* _temp790; struct Cyc_Absyn_Uniondecl*
_temp792; struct Cyc_Absyn_Enumdecl* _temp794; struct Cyc_Absyn_Typedefdecl*
_temp796; _LL762: if(*(( int*) _temp760) ==  Cyc_Absyn_Var_d){ _LL787: _temp786=((
struct Cyc_Absyn_Var_d_struct*) _temp760)->f1; goto _LL763;} else{ goto _LL764;}
_LL764: if(*(( int*) _temp760) ==  Cyc_Absyn_Fn_d){ _LL789: _temp788=(( struct
Cyc_Absyn_Fn_d_struct*) _temp760)->f1; goto _LL765;} else{ goto _LL766;} _LL766:
if(*(( int*) _temp760) ==  Cyc_Absyn_Struct_d){ _LL791: _temp790=(( struct Cyc_Absyn_Struct_d_struct*)
_temp760)->f1; goto _LL767;} else{ goto _LL768;} _LL768: if(*(( int*) _temp760)
==  Cyc_Absyn_Union_d){ _LL793: _temp792=(( struct Cyc_Absyn_Union_d_struct*)
_temp760)->f1; goto _LL769;} else{ goto _LL770;} _LL770: if(*(( int*) _temp760)
==  Cyc_Absyn_Enum_d){ _LL795: _temp794=(( struct Cyc_Absyn_Enum_d_struct*)
_temp760)->f1; goto _LL771;} else{ goto _LL772;} _LL772: if(*(( int*) _temp760)
==  Cyc_Absyn_Typedef_d){ _LL797: _temp796=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp760)->f1; goto _LL773;} else{ goto _LL774;} _LL774: if(*(( int*) _temp760)
==  Cyc_Absyn_Let_d){ goto _LL775;} else{ goto _LL776;} _LL776: if(*(( int*)
_temp760) ==  Cyc_Absyn_Tunion_d){ goto _LL777;} else{ goto _LL778;} _LL778: if(*((
int*) _temp760) ==  Cyc_Absyn_Letv_d){ goto _LL779;} else{ goto _LL780;} _LL780:
if(*(( int*) _temp760) ==  Cyc_Absyn_Namespace_d){ goto _LL781;} else{ goto
_LL782;} _LL782: if(*(( int*) _temp760) ==  Cyc_Absyn_Using_d){ goto _LL783;}
else{ goto _LL784;} _LL784: if(*(( int*) _temp760) ==  Cyc_Absyn_ExternC_d){
goto _LL785;} else{ goto _LL761;} _LL763: { struct _tuple0 _temp800; struct
_tagged_arr* _temp801; struct _tuple0* _temp798= _temp786->name; _temp800=*
_temp798; _LL802: _temp801= _temp800.f2; goto _LL799; _LL799: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp801); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp801)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp801;} goto _LL761;} _LL765: { struct _tuple0 _temp805; struct
_tagged_arr* _temp806; struct _tuple0* _temp803= _temp788->name; _temp805=*
_temp803; _LL807: _temp806= _temp805.f2; goto _LL804; _LL804: defined_symbols=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
defined_symbols, _temp806); if((( int(*)( int(* compare)( struct _tagged_arr*,
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_strptrcmp, omit_symbols, _temp806)){ name= 0;} else{ name=( struct
_tagged_arr*) _temp806;} goto _LL761;} _LL767: if( !(( unsigned int) _temp790->name)){
name= 0;}{ struct _tuple0 _temp810; struct _tagged_arr* _temp811; struct _tuple0*
_temp808=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp790->name))->v;
_temp810=* _temp808; _LL812: _temp811= _temp810.f2; goto _LL809; _LL809: name=(
struct _tagged_arr*) _temp811; goto _LL761;} _LL769: if( !(( unsigned int)
_temp792->name)){ name= 0;}{ struct _tuple0 _temp815; struct _tagged_arr*
_temp816; struct _tuple0* _temp813=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp792->name))->v; _temp815=* _temp813; _LL817: _temp816=
_temp815.f2; goto _LL814; _LL814: name=( struct _tagged_arr*) _temp816; goto
_LL761;} _LL771: { struct _tuple0 _temp820; struct _tagged_arr* _temp821; struct
_tuple0* _temp818= _temp794->name; _temp820=* _temp818; _LL822: _temp821=
_temp820.f2; goto _LL819; _LL819: name=( struct _tagged_arr*) _temp821; if( name
!=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp756,( struct _tagged_arr*) _check_null( name)): 0){ _temp757=({ struct Cyc_List_List*
_temp823=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp823->hd=( void*) _temp759; _temp823->tl= _temp757; _temp823;});} else{ if((
unsigned int) _temp794->fields){ struct Cyc_List_List* _temp824=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp794->fields))->v; for( 0; _temp824 !=  0;
_temp824= _temp824->tl){ struct Cyc_Absyn_Enumfield* _temp825=( struct Cyc_Absyn_Enumfield*)
_temp824->hd; struct _tuple0 _temp828; struct _tagged_arr* _temp829; struct
_tuple0* _temp826= _temp825->name; _temp828=* _temp826; _LL830: _temp829=
_temp828.f2; goto _LL827; _LL827: if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( _temp756, _temp829)){ _temp757=({ struct Cyc_List_List*
_temp831=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp831->hd=( void*) _temp759; _temp831->tl= _temp757; _temp831;}); break;}}}}
name= 0; goto _LL761;} _LL773: { struct _tuple0 _temp834; struct _tagged_arr*
_temp835; struct _tuple0* _temp832= _temp796->name; _temp834=* _temp832; _LL836:
_temp835= _temp834.f2; goto _LL833; _LL833: name=( struct _tagged_arr*) _temp835;
goto _LL761;} _LL775: goto _LL777; _LL777: goto _LL779; _LL779: goto _LL781;
_LL781: goto _LL783; _LL783: goto _LL785; _LL785: name= 0; goto _LL761; _LL761:;}
if( name !=  0?(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp756,( struct _tagged_arr*) _check_null( name)): 0){ _temp757=({ struct Cyc_List_List*
_temp837=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp837->hd=( void*) _temp759; _temp837->tl= _temp757; _temp837;});}}} _temp757=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp757);
maybe= Cyc_Std_fopen( filename, _tag_arr("w", sizeof( unsigned char), 2u)); if(
!(( unsigned int) maybe)){ return 1;} out_file=( struct Cyc_Std___sFILE*)
_check_null( maybe);({ struct Cyc_Std_String_pa_struct _temp839; _temp839.tag=
Cyc_Std_String_pa; _temp839.f1=( struct _tagged_arr) _temp706;{ void* _temp838[
1u]={& _temp839}; Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\nusing Std;\n",
sizeof( unsigned char), 26u), _tag_arr( _temp838, sizeof( void*), 1u));}}); Cyc_Std_fclose(
out_file); maybe= Cyc_Std_fopen(( struct _tagged_arr) _temp706, _tag_arr("w",
sizeof( unsigned char), 2u)); if( !(( unsigned int) maybe)){ return 1;} out_file=(
struct Cyc_Std___sFILE*) _check_null( maybe);{ struct _tagged_arr ifdefmacro=({
struct Cyc_Std_String_pa_struct _temp989; _temp989.tag= Cyc_Std_String_pa;
_temp989.f1=( struct _tagged_arr) filename;{ void* _temp988[ 1u]={& _temp989};
Cyc_Std_aprintf( _tag_arr("_%s_", sizeof( unsigned char), 5u), _tag_arr(
_temp988, sizeof( void*), 1u));}});{ int _temp840= 0; for( 0; _temp840 < 
_get_arr_size( ifdefmacro, sizeof( unsigned char)); _temp840 ++){ if(*((
unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof( unsigned char),
_temp840)) == '.'? 1:*(( unsigned char*) _check_unknown_subscript( ifdefmacro,
sizeof( unsigned char), _temp840)) == '/'){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp840))='_';}
else{ if(*(( unsigned char*) _check_unknown_subscript( ifdefmacro, sizeof(
unsigned char), _temp840)) != '_'?*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp840)) != '/': 0){*(( unsigned char*)
_check_unknown_subscript( ifdefmacro, sizeof( unsigned char), _temp840))=(
unsigned char) toupper(( int)*(( unsigned char*) _check_unknown_subscript(
ifdefmacro, sizeof( unsigned char), _temp840)));}}}}({ struct Cyc_Std_String_pa_struct
_temp843; _temp843.tag= Cyc_Std_String_pa; _temp843.f1=( struct _tagged_arr)
ifdefmacro;{ struct Cyc_Std_String_pa_struct _temp842; _temp842.tag= Cyc_Std_String_pa;
_temp842.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp841[ 2u]={& _temp842,&
_temp843}; Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n#define %s\n",
sizeof( unsigned char), 23u), _tag_arr( _temp841, sizeof( void*), 2u));}}}); if(
prologue !=  0){ struct Cyc_List_List* _temp844= prologue; for( 0; _temp844 != 
0; _temp844= _temp844->tl){ struct _tuple10 _temp847; struct _tagged_arr
_temp848; struct _tagged_arr _temp850; struct _tuple10* _temp845=( struct
_tuple10*) _temp844->hd; _temp847=* _temp845; _LL851: _temp850= _temp847.f1;
goto _LL849; _LL849: _temp848= _temp847.f2; goto _LL846; _LL846: if( _temp848.curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp850.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp852=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp852[ 0]= _temp850; _temp852;})): 0){ Cyc_Std_fputs( _temp848,
out_file);} else{({ struct Cyc_Std_String_pa_struct _temp854; _temp854.tag= Cyc_Std_String_pa;
_temp854.f1=( struct _tagged_arr) _temp850;{ void* _temp853[ 1u]={& _temp854};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_log_file), _tag_arr("%s is not supported on this platform\n",
sizeof( unsigned char), 38u), _tag_arr( _temp853, sizeof( void*), 1u));}});}}}({
void* _temp855[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("\nnamespace Std {\n",
sizeof( unsigned char), 18u), _tag_arr( _temp855, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp856= _temp757; for( 0; _temp856 !=  0; _temp856=
_temp856->tl){ struct Cyc_Absyn_Decl* _temp857=( struct Cyc_Absyn_Decl*)
_temp856->hd; int _temp858= 0; struct _tagged_arr* name;{ void* _temp859=( void*)
_temp857->r; struct Cyc_Absyn_Vardecl* _temp885; struct Cyc_Absyn_Fndecl*
_temp887; struct Cyc_Absyn_Structdecl* _temp889; struct Cyc_Absyn_Uniondecl*
_temp891; struct Cyc_Absyn_Enumdecl* _temp893; struct Cyc_Absyn_Typedefdecl*
_temp895; _LL861: if(*(( int*) _temp859) ==  Cyc_Absyn_Var_d){ _LL886: _temp885=((
struct Cyc_Absyn_Var_d_struct*) _temp859)->f1; goto _LL862;} else{ goto _LL863;}
_LL863: if(*(( int*) _temp859) ==  Cyc_Absyn_Fn_d){ _LL888: _temp887=(( struct
Cyc_Absyn_Fn_d_struct*) _temp859)->f1; goto _LL864;} else{ goto _LL865;} _LL865:
if(*(( int*) _temp859) ==  Cyc_Absyn_Struct_d){ _LL890: _temp889=(( struct Cyc_Absyn_Struct_d_struct*)
_temp859)->f1; goto _LL866;} else{ goto _LL867;} _LL867: if(*(( int*) _temp859)
==  Cyc_Absyn_Union_d){ _LL892: _temp891=(( struct Cyc_Absyn_Union_d_struct*)
_temp859)->f1; goto _LL868;} else{ goto _LL869;} _LL869: if(*(( int*) _temp859)
==  Cyc_Absyn_Enum_d){ _LL894: _temp893=(( struct Cyc_Absyn_Enum_d_struct*)
_temp859)->f1; goto _LL870;} else{ goto _LL871;} _LL871: if(*(( int*) _temp859)
==  Cyc_Absyn_Typedef_d){ _LL896: _temp895=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp859)->f1; goto _LL872;} else{ goto _LL873;} _LL873: if(*(( int*) _temp859)
==  Cyc_Absyn_Let_d){ goto _LL874;} else{ goto _LL875;} _LL875: if(*(( int*)
_temp859) ==  Cyc_Absyn_Tunion_d){ goto _LL876;} else{ goto _LL877;} _LL877: if(*((
int*) _temp859) ==  Cyc_Absyn_Letv_d){ goto _LL878;} else{ goto _LL879;} _LL879:
if(*(( int*) _temp859) ==  Cyc_Absyn_Namespace_d){ goto _LL880;} else{ goto
_LL881;} _LL881: if(*(( int*) _temp859) ==  Cyc_Absyn_Using_d){ goto _LL882;}
else{ goto _LL883;} _LL883: if(*(( int*) _temp859) ==  Cyc_Absyn_ExternC_d){
goto _LL884;} else{ goto _LL860;} _LL862: { struct _tuple0 _temp899; struct
_tagged_arr* _temp900; struct _tuple0* _temp897= _temp885->name; _temp899=*
_temp897; _LL901: _temp900= _temp899.f2; goto _LL898; _LL898: name=( struct
_tagged_arr*) _temp900; goto _LL860;} _LL864: { struct _tuple0 _temp904; struct
_tagged_arr* _temp905; struct _tuple0* _temp902= _temp887->name; _temp904=*
_temp902; _LL906: _temp905= _temp904.f2; goto _LL903; _LL903: name=( struct
_tagged_arr*) _temp905; goto _LL860;} _LL866: if( !(( unsigned int) _temp889->name)){
name= 0;}{ struct _tuple0 _temp909; struct _tagged_arr* _temp910; struct _tuple0*
_temp907=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp889->name))->v;
_temp909=* _temp907; _LL911: _temp910= _temp909.f2; goto _LL908; _LL908: name=(
struct _tagged_arr*) _temp910; goto _LL860;} _LL868: if( !(( unsigned int)
_temp891->name)){ name= 0;}{ struct _tuple0 _temp914; struct _tagged_arr*
_temp915; struct _tuple0* _temp912=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp891->name))->v; _temp914=* _temp912; _LL916: _temp915=
_temp914.f2; goto _LL913; _LL913: name=( struct _tagged_arr*) _temp915; goto
_LL860;} _LL870: { struct _tuple0 _temp919; struct _tagged_arr* _temp920; struct
_tuple0* _temp917= _temp893->name; _temp919=* _temp917; _LL921: _temp920=
_temp919.f2; goto _LL918; _LL918: name=( struct _tagged_arr*) _temp920; goto
_LL860;} _LL872: { struct _tuple0 _temp924; struct _tagged_arr* _temp925; struct
_tuple0* _temp922= _temp895->name; _temp924=* _temp922; _LL926: _temp925=
_temp924.f2; goto _LL923; _LL923: name=( struct _tagged_arr*) _temp925; goto
_LL860;} _LL874: goto _LL876; _LL876: goto _LL878; _LL878: goto _LL880; _LL880:
goto _LL882; _LL882: goto _LL884; _LL884: name= 0; goto _LL860; _LL860:;} if( !((
unsigned int) name)? ! _temp858: 0){ continue;} if(( unsigned int) name){
ifdefmacro=({ struct Cyc_Std_String_pa_struct _temp928; _temp928.tag= Cyc_Std_String_pa;
_temp928.f1=( struct _tagged_arr)* name;{ void* _temp927[ 1u]={& _temp928}; Cyc_Std_aprintf(
_tag_arr("_%s_def_", sizeof( unsigned char), 9u), _tag_arr( _temp927, sizeof(
void*), 1u));}});({ struct Cyc_Std_String_pa_struct _temp930; _temp930.tag= Cyc_Std_String_pa;
_temp930.f1=( struct _tagged_arr) ifdefmacro;{ void* _temp929[ 1u]={& _temp930};
Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof( unsigned char), 12u),
_tag_arr( _temp929, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp932; _temp932.tag= Cyc_Std_String_pa; _temp932.f1=( struct _tagged_arr)
ifdefmacro;{ void* _temp931[ 1u]={& _temp932}; Cyc_Std_fprintf( out_file,
_tag_arr("#define %s\n", sizeof( unsigned char), 12u), _tag_arr( _temp931,
sizeof( void*), 1u));}}); Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp933[ 1u]; _temp933[ 0u]= _temp857;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp933, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);({ void* _temp934[ 0u]={}; Cyc_Std_fprintf( out_file,
_tag_arr("#endif\n", sizeof( unsigned char), 8u), _tag_arr( _temp934, sizeof(
void*), 0u));});} else{ Cyc_Absynpp_decllist2file(({ struct Cyc_Absyn_Decl*
_temp935[ 1u]; _temp935[ 0u]= _temp857;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp935, sizeof( struct Cyc_Absyn_Decl*),
1u));}), out_file);}}} _temp707=({ struct Cyc_Std_String_pa_struct _temp938;
_temp938.tag= Cyc_Std_String_pa; _temp938.f1=( struct _tagged_arr) _temp703;{
struct Cyc_Std_String_pa_struct _temp937; _temp937.tag= Cyc_Std_String_pa;
_temp937.f1=( struct _tagged_arr) filename;{ void* _temp936[ 2u]={& _temp937,&
_temp938}; Cyc_Std_aprintf( _tag_arr("echo '#include <%s>' | gcc -E -dM -o %s -",
sizeof( unsigned char), 42u), _tag_arr( _temp936, sizeof( void*), 2u));}}}); if(
Cyc_Std_system(( struct _tagged_arr) _temp707) !=  0){ return 1;} maybe= Cyc_Std_fopen((
struct _tagged_arr) _temp703, _tag_arr("r", sizeof( unsigned char), 2u)); if( !((
unsigned int) maybe)){ return 1;} in_file=( struct Cyc_Std___sFILE*) _check_null(
maybe); _temp721= Cyc_Lexing_from_file( in_file);{ struct _tuple9* entry2;
while(( entry2=(( struct _tuple9*(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_suck_line)(
_temp721)) !=  0) { struct _tuple9 _temp941; struct _tagged_arr* _temp942;
struct _tagged_arr _temp944; struct _tuple9* _temp939=( struct _tuple9*)
_check_null( entry2); _temp941=* _temp939; _LL945: _temp944= _temp941.f1; goto
_LL943; _LL943: _temp942= _temp941.f2; goto _LL940; _LL940: if((( int(*)( struct
Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( _temp756, _temp942)){({
struct Cyc_Std_String_pa_struct _temp947; _temp947.tag= Cyc_Std_String_pa;
_temp947.f1=( struct _tagged_arr)* _temp942;{ void* _temp946[ 1u]={& _temp947};
Cyc_Std_fprintf( out_file, _tag_arr("#ifndef %s\n", sizeof( unsigned char), 12u),
_tag_arr( _temp946, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp949; _temp949.tag= Cyc_Std_String_pa; _temp949.f1=( struct _tagged_arr)
_temp944;{ void* _temp948[ 1u]={& _temp949}; Cyc_Std_fprintf( out_file, _tag_arr("%s\n",
sizeof( unsigned char), 4u), _tag_arr( _temp948, sizeof( void*), 1u));}});({
void* _temp950[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp950, sizeof( void*), 0u));});}} Cyc_Std_fclose(
in_file); Cyc_Std_remove(( struct _tagged_arr) _temp703); if( epilogue !=  0){
struct Cyc_List_List* _temp951= epilogue; for( 0; _temp951 !=  0; _temp951=
_temp951->tl){ struct _tuple10 _temp954; struct _tagged_arr _temp955; struct
_tagged_arr _temp957; struct _tuple10* _temp952=( struct _tuple10*) _temp951->hd;
_temp954=* _temp952; _LL958: _temp957= _temp954.f1; goto _LL956; _LL956:
_temp955= _temp954.f2; goto _LL953; _LL953: if( _temp955.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp957.curr == (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct _tagged_arr*
_temp959=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp959[ 0]= _temp957; _temp959;})): 0){ Cyc_Std_fputs( _temp955, out_file);}
else{({ struct Cyc_Std_String_pa_struct _temp961; _temp961.tag= Cyc_Std_String_pa;
_temp961.f1=( struct _tagged_arr) _temp957;{ void* _temp960[ 1u]={& _temp961};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_log_file), _tag_arr("%s is not supported on this platform\n",
sizeof( unsigned char), 38u), _tag_arr( _temp960, sizeof( void*), 1u));}});}}}({
void* _temp962[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n", sizeof(
unsigned char), 3u), _tag_arr( _temp962, sizeof( void*), 0u));});({ void*
_temp963[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("#endif\n", sizeof(
unsigned char), 8u), _tag_arr( _temp963, sizeof( void*), 0u));}); Cyc_Std_fclose(
out_file); if( cstubs !=  0){ out_file=( struct Cyc_Std___sFILE*) _check_null(
Cyc_cstubs_file);{ struct Cyc_List_List* _temp964= cstubs; for( 0; _temp964 != 
0; _temp964= _temp964->tl){ struct _tuple10 _temp967; struct _tagged_arr
_temp968; struct _tagged_arr _temp970; struct _tuple10* _temp965=( struct
_tuple10*) _temp964->hd; _temp967=* _temp965; _LL971: _temp970= _temp967.f1;
goto _LL969; _LL969: _temp968= _temp967.f2; goto _LL966; _LL966: if( _temp968.curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp970.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct
_tagged_arr* _temp972=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp972[ 0]= _temp970; _temp972;})): 0){ Cyc_Std_fputs( _temp968,
out_file);}}}} out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _temp974; _temp974.tag= Cyc_Std_String_pa;
_temp974.f1=( struct _tagged_arr) _temp706;{ void* _temp973[ 1u]={& _temp974};
Cyc_Std_fprintf( out_file, _tag_arr("#include <%s>\n\n", sizeof( unsigned char),
16u), _tag_arr( _temp973, sizeof( void*), 1u));}}); if( cycstubs !=  0){
out_file=( struct Cyc_Std___sFILE*) _check_null( Cyc_cycstubs_file);({ void*
_temp975[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("namespace Std {\n",
sizeof( unsigned char), 17u), _tag_arr( _temp975, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp976= cycstubs; for( 0; _temp976 !=  0; _temp976=
_temp976->tl){ struct _tuple10 _temp979; struct _tagged_arr _temp980; struct
_tagged_arr _temp982; struct _tuple10* _temp977=( struct _tuple10*) _temp976->hd;
_temp979=* _temp977; _LL983: _temp982= _temp979.f1; goto _LL981; _LL981:
_temp980= _temp979.f2; goto _LL978; _LL978: if( _temp980.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? _temp982.curr == (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr? 1:(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( defined_symbols,({ struct _tagged_arr*
_temp984=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp984[ 0]= _temp982; _temp984;})): 0){ Cyc_Std_fputs( _temp980, out_file);}}}({
void* _temp985[ 0u]={}; Cyc_Std_fprintf( out_file, _tag_arr("}\n\n", sizeof(
unsigned char), 4u), _tag_arr( _temp985, sizeof( void*), 0u));});} if( Cyc_Std_chdir((
struct _tagged_arr) _temp696)){({ struct Cyc_Std_String_pa_struct _temp987;
_temp987.tag= Cyc_Std_String_pa; _temp987.f1=( struct _tagged_arr) _temp696;{
void* _temp986[ 1u]={& _temp987}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not change directory to %s\n",
sizeof( unsigned char), 41u), _tag_arr( _temp986, sizeof( void*), 1u));}});
return 1;} return 0;}}}}}}}}} int Cyc_process_specfile( struct _tagged_arr file,
struct _tagged_arr dir){ struct Cyc_Std___sFILE* _temp1000= Cyc_Std_fopen( file,
_tag_arr("r", sizeof( unsigned char), 2u)); if( !(( unsigned int) _temp1000)){
return 1;}{ struct Cyc_Std___sFILE* _temp1001=( struct Cyc_Std___sFILE*)
_check_null( _temp1000); struct Cyc_Lexing_lexbuf* _temp1002= Cyc_Lexing_from_file(
_temp1001); struct _tuple11* entry; while(( entry=(( struct _tuple11*(*)( struct
Cyc_Lexing_lexbuf* lexbuf)) Cyc_spec)( _temp1002)) !=  0) { struct _tuple11
_temp1005; struct Cyc_List_List* _temp1006; struct Cyc_List_List* _temp1008;
struct Cyc_List_List* _temp1010; struct Cyc_List_List* _temp1012; struct Cyc_List_List*
_temp1014; struct Cyc_List_List* _temp1016; struct _tagged_arr _temp1018; struct
_tuple11* _temp1003=( struct _tuple11*) _check_null( entry); _temp1005=*
_temp1003; _LL1019: _temp1018= _temp1005.f1; goto _LL1017; _LL1017: _temp1016=
_temp1005.f2; goto _LL1015; _LL1015: _temp1014= _temp1005.f3; goto _LL1013;
_LL1013: _temp1012= _temp1005.f4; goto _LL1011; _LL1011: _temp1010= _temp1005.f5;
goto _LL1009; _LL1009: _temp1008= _temp1005.f6; goto _LL1007; _LL1007: _temp1006=
_temp1005.f7; goto _LL1004; _LL1004: if( Cyc_process_file( _temp1018, dir,
_temp1016, _temp1014, _temp1012, _temp1010, _temp1008, _temp1006)){ Cyc_Std_fclose(
_temp1001); return 1;}} Cyc_Std_fclose( _temp1001); return 0;}} int Cyc_getsize(
struct _tagged_arr dir, struct _tagged_arr includes, struct _tagged_arr type){
struct _tagged_arr _temp1020= Cyc_Filename_concat( dir, _tag_arr("getsize.c",
sizeof( unsigned char), 10u)); struct _tagged_arr _temp1021= Cyc_Filename_concat(
dir, _tag_arr("getsize", sizeof( unsigned char), 8u)); struct _tagged_arr
_temp1022= Cyc_Filename_concat( dir, _tag_arr("getsize.out", sizeof(
unsigned char), 12u)); struct Cyc_Std___sFILE* _temp1023= Cyc_Std_fopen(( struct
_tagged_arr) _temp1020, _tag_arr("w", sizeof( unsigned char), 2u)); if( !((
unsigned int) _temp1023)){({ struct Cyc_Std_String_pa_struct _temp1025;
_temp1025.tag= Cyc_Std_String_pa; _temp1025.f1=( struct _tagged_arr) _temp1020;{
void* _temp1024[ 1u]={& _temp1025}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create %s\n",
sizeof( unsigned char), 28u), _tag_arr( _temp1024, sizeof( void*), 1u));}});
return - 1;}({ struct Cyc_Std_String_pa_struct _temp1030; _temp1030.tag= Cyc_Std_String_pa;
_temp1030.f1=( struct _tagged_arr) type;{ struct Cyc_Std_String_pa_struct
_temp1029; _temp1029.tag= Cyc_Std_String_pa; _temp1029.f1=( struct _tagged_arr)
type;{ struct Cyc_Std_String_pa_struct _temp1028; _temp1028.tag= Cyc_Std_String_pa;
_temp1028.f1=( struct _tagged_arr) _temp1022;{ struct Cyc_Std_String_pa_struct
_temp1027; _temp1027.tag= Cyc_Std_String_pa; _temp1027.f1=( struct _tagged_arr)
includes;{ void* _temp1026[ 4u]={& _temp1027,& _temp1028,& _temp1029,& _temp1030};
Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( _temp1023), _tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof( unsigned char), 139u), _tag_arr( _temp1026, sizeof( void*), 4u));}}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1023));{ struct
_tagged_arr _temp1031=({ struct Cyc_Std_String_pa_struct _temp1047; _temp1047.tag=
Cyc_Std_String_pa; _temp1047.f1=( struct _tagged_arr) _temp1020;{ struct Cyc_Std_String_pa_struct
_temp1046; _temp1046.tag= Cyc_Std_String_pa; _temp1046.f1=( struct _tagged_arr)
_temp1021;{ void* _temp1045[ 2u]={& _temp1046,& _temp1047}; Cyc_Std_aprintf(
_tag_arr("gcc -o %s %s", sizeof( unsigned char), 13u), _tag_arr( _temp1045,
sizeof( void*), 2u));}}}); if( Cyc_Std_system(( struct _tagged_arr) _temp1031)
!=  0){({ struct Cyc_Std_String_pa_struct _temp1033; _temp1033.tag= Cyc_Std_String_pa;
_temp1033.f1=( struct _tagged_arr) _temp1021;{ void* _temp1032[ 1u]={& _temp1033};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: can't run %s\n", sizeof(
unsigned char), 21u), _tag_arr( _temp1032, sizeof( void*), 1u));}}); return - 1;}{
int _temp1034= Cyc_Std_system(( struct _tagged_arr) _temp1021);({ struct Cyc_Std_Int_pa_struct
_temp1037; _temp1037.tag= Cyc_Std_Int_pa; _temp1037.f1=( int)(( unsigned int)
_temp1034);{ struct Cyc_Std_String_pa_struct _temp1036; _temp1036.tag= Cyc_Std_String_pa;
_temp1036.f1=( struct _tagged_arr) type;{ void* _temp1035[ 2u]={& _temp1036,&
_temp1037}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*) _check_null( Cyc_log_file),
_tag_arr("size of %s returned by system is %d\n", sizeof( unsigned char), 37u),
_tag_arr( _temp1035, sizeof( void*), 2u));}}});{ struct Cyc_Std___sFILE*
_temp1038= Cyc_Std_fopen(( struct _tagged_arr) _temp1022, _tag_arr("r", sizeof(
unsigned char), 2u)); int w= - 1; if(({ struct Cyc_Std_IntPtr_sa_struct
_temp1040; _temp1040.tag= Cyc_Std_IntPtr_sa; _temp1040.f1=& w;{ void* _temp1039[
1u]={& _temp1040}; Cyc_Std_fscanf(( struct Cyc_Std___sFILE*) _check_null(
_temp1038), _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1039,
sizeof( void*), 1u));}}) !=  1){ return - 1;}({ struct Cyc_Std_Int_pa_struct
_temp1044; _temp1044.tag= Cyc_Std_Int_pa; _temp1044.f1=( int)(( unsigned int) w);{
struct Cyc_Std_String_pa_struct _temp1043; _temp1043.tag= Cyc_Std_String_pa;
_temp1043.f1=( struct _tagged_arr) _temp1022;{ struct Cyc_Std_String_pa_struct
_temp1042; _temp1042.tag= Cyc_Std_String_pa; _temp1042.f1=( struct _tagged_arr)
type;{ void* _temp1041[ 3u]={& _temp1042,& _temp1043,& _temp1044}; Cyc_Std_fprintf((
struct Cyc_Std___sFILE*) _check_null( Cyc_log_file), _tag_arr("size of %s read from file %s is %d\n",
sizeof( unsigned char), 36u), _tag_arr( _temp1041, sizeof( void*), 3u));}}}});
Cyc_Std_fclose(( struct Cyc_Std___sFILE*) _check_null( _temp1038)); Cyc_Std_remove((
struct _tagged_arr) _temp1022); Cyc_Std_remove(( struct _tagged_arr) _temp1020);
Cyc_Std_remove(( struct _tagged_arr) _temp1021); return w;}}}} int Cyc_main( int
argc, struct _tagged_arr argv){ if( argc <  3){({ void* _temp1048[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof( unsigned char), 105u), _tag_arr( _temp1048, sizeof( void*), 0u));});
return 1;}{ struct _tagged_arr _temp1049=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 1)); if( Cyc_force_directory_prefixes((
struct _tagged_arr) _temp1049)? 1: Cyc_force_directory(( struct _tagged_arr)
_temp1049)){({ struct Cyc_Std_String_pa_struct _temp1051; _temp1051.tag= Cyc_Std_String_pa;
_temp1051.f1=( struct _tagged_arr) _temp1049;{ void* _temp1050[ 1u]={& _temp1051};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create directory %s\n",
sizeof( unsigned char), 38u), _tag_arr( _temp1050, sizeof( void*), 1u));}});
return 1;} Cyc_log_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1049, _tag_arr("BUILDLIB.LOG", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_log_file)){({
struct Cyc_Std_String_pa_struct _temp1053; _temp1053.tag= Cyc_Std_String_pa;
_temp1053.f1=( struct _tagged_arr) _temp1049;{ void* _temp1052[ 1u]={& _temp1053};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create log file in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1052, sizeof( void*), 1u));}});
return 1;} Cyc_cstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1049, _tag_arr("cstubs.c", sizeof( unsigned char), 9u)),
_tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1055; _temp1055.tag= Cyc_Std_String_pa;
_temp1055.f1=( struct _tagged_arr) _temp1049;{ void* _temp1054[ 1u]={& _temp1055};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof( unsigned char), 50u), _tag_arr( _temp1054, sizeof( void*), 1u));}});
return 1;} Cyc_cycstubs_file= Cyc_Std_fopen(( struct _tagged_arr) Cyc_Filename_concat((
struct _tagged_arr) _temp1049, _tag_arr("cycstubs.cyc", sizeof( unsigned char),
13u)), _tag_arr("w", sizeof( unsigned char), 2u)); if( !(( unsigned int) Cyc_cycstubs_file)){({
struct Cyc_Std_String_pa_struct _temp1057; _temp1057.tag= Cyc_Std_String_pa;
_temp1057.f1=( struct _tagged_arr) _temp1049;{ void* _temp1056[ 1u]={& _temp1057};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof( unsigned char), 52u), _tag_arr( _temp1056, sizeof( void*), 1u));}});
return 1;}({ void* _temp1058[ 0u]={}; Cyc_Std_fprintf(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file), _tag_arr("#include <core.h>\nusing Core;\n\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1058, sizeof( void*), 0u));}); Cyc_sizeof_unsignedlongint=
Cyc_getsize(( struct _tagged_arr) _temp1049, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned long int", sizeof( unsigned char), 18u)); Cyc_sizeof_unsignedlongint=
Cyc_getsize(( struct _tagged_arr) _temp1049, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("unsigned short int", sizeof( unsigned char), 19u)); Cyc_sizeof_int=
Cyc_getsize(( struct _tagged_arr) _temp1049, _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("int", sizeof( unsigned char), 4u)); Cyc_sizeof_fdmask= Cyc_getsize((
struct _tagged_arr) _temp1049, _tag_arr("#include <sys/select.h>", sizeof(
unsigned char), 24u), _tag_arr("__fd_mask", sizeof( unsigned char), 10u)); Cyc_sizeof_uint32=
Cyc_getsize(( struct _tagged_arr) _temp1049, _tag_arr("#include <sys/types.h>",
sizeof( unsigned char), 23u), _tag_arr("__uint32_t", sizeof( unsigned char), 11u));
Cyc_sizeof_sockaddr= Cyc_getsize(( struct _tagged_arr) _temp1049, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("struct sockaddr", sizeof( unsigned char),
16u)); Cyc_sizeof_inport= Cyc_getsize(( struct _tagged_arr) _temp1049, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("in_port_t", sizeof( unsigned char), 10u));
Cyc_sizeof_inaddr= Cyc_getsize(( struct _tagged_arr) _temp1049, _tag_arr("#include <netinet/in.h>",
sizeof( unsigned char), 24u), _tag_arr("struct in_addr", sizeof( unsigned char),
15u));{ int i= 2; for( 0; i <  argc; i ++){ Cyc_process_specfile(( struct
_tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), i)),( struct _tagged_arr) _temp1049);}} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( Cyc_log_file)); Cyc_Std_fclose(( struct
Cyc_Std___sFILE*) _check_null( Cyc_cstubs_file)); Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( Cyc_cycstubs_file)); return 0;}}
