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
 extern void exit(int);struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{
void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _tagged_arr Cstring_to_string(
unsigned char*);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*
Cyc_Std_stderr;extern int Cyc_Std_remove(struct _tagged_arr);extern int Cyc_Std_fclose(
struct Cyc_Std___sFILE*);extern int Cyc_Std_fflush(struct Cyc_Std___sFILE*);extern
struct Cyc_Std___sFILE*Cyc_Std_fopen(struct _tagged_arr __filename,struct
_tagged_arr __modes);extern int Cyc_Std_fputc(int __c,struct Cyc_Std___sFILE*__stream);
extern int Cyc_Std_fputs(struct _tagged_arr __s,struct Cyc_Std___sFILE*__stream);
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(struct _tagged_arr fmt,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,
struct _tagged_arr);extern int Cyc_Std_vfprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr ap);static const int Cyc_Std_ShortPtr_sa=0;struct
Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};extern int Cyc_Std_fscanf(struct Cyc_Std___sFILE*stream,struct
_tagged_arr fmt,struct _tagged_arr);extern unsigned char Cyc_Lexing_Error[10];struct
Cyc_Lexing_Error_struct{unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc_Std___sFILE*);extern struct
_tagged_arr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);extern unsigned char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);extern unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);extern
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
extern struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);extern int
Cyc_Set_cardinality(struct Cyc_Set_Set*s);extern int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern struct Cyc_List_List*Cyc_Set_elements(struct Cyc_Set_Set*s);
extern unsigned char Cyc_Set_Absent[11];extern unsigned int Cyc_Std_strlen(struct
_tagged_arr s);extern int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*
s2);extern struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_strdup(struct _tagged_arr src);extern struct
_tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Hashtable_Table;
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),
int(*hash)(void*));extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,
void*key,void*val);extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,
void*key);extern int Cyc_Hashtable_hash_stringptr(struct _tagged_arr*p);extern
struct _tagged_arr Cyc_Filename_concat(struct _tagged_arr,struct _tagged_arr);extern
struct _tagged_arr Cyc_Filename_chop_extension(struct _tagged_arr);extern struct
_tagged_arr Cyc_Filename_dirname(struct _tagged_arr);extern struct _tagged_arr Cyc_Filename_basename(
struct _tagged_arr);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
extern void Cyc_Position_reset_position(struct _tagged_arr);struct Cyc_Position_Segment;
static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=1;static const
int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr source;struct
Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern unsigned char
Cyc_Position_Nocontext[14];extern unsigned char Cyc_Array_Array_mismatch[19];
struct Cyc_Std__Div{int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};extern
int abs(int __x);extern int atexit(void(*__func)());extern struct Cyc_Std__Div div(int
__numer,int __denom);extern struct Cyc_Std__Ldiv ldiv(int __numer,int __denom);extern
int random();extern void srandom(unsigned int __seed);extern int rand();extern void
srand(unsigned int __seed);extern int rand_r(unsigned int*__seed);extern int grantpt(
int __fd);extern int unlockpt(int __fd);extern int Cyc_Std_system(struct _tagged_arr);
struct Cyc_Buffer_t;extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
extern struct _tagged_arr Cyc_Buffer_contents(struct Cyc_Buffer_t*);extern void Cyc_Buffer_add_char(
struct Cyc_Buffer_t*,unsigned char);extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,
struct _tagged_arr);struct Cyc_Std_tm{int tm_sec;int tm_min;int tm_hour;int tm_mday;
int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;};struct Cyc_Std_stat_t{
short st_dev;unsigned int st_ino;int st_mode;unsigned short st_nlink;unsigned short
st_uid;unsigned short st_gid;short st_rdev;int st_size;int st_atime;int st_spare1;int
st_mtime;int st_spare2;int st_ctime;int st_spare3;int st_blksize;int st_blocks;int
st_spare4[2];};extern int Cyc_Std_mkdir(struct _tagged_arr pathname,int mode);struct
Cyc_Std_flock{short l_type;int l_start;short l_whence;int l_len;int l_pid;};static
const int Cyc_Std_Flock=0;struct Cyc_Std_Flock_struct{int tag;struct Cyc_Std_flock*f1;
};static const int Cyc_Std_Long=1;struct Cyc_Std_Long_struct{int tag;int f1;};extern
int Cyc_Std_open(struct _tagged_arr,int,struct _tagged_arr);struct Cyc_Std_timeval{
int tv_sec;int tv_usec;};struct Cyc_Std_timezone{int tz_minuteswest;int tz_dsttime;};
struct Cyc_Std_itimerval{struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval
it_value;};typedef struct {unsigned int __val[64];}Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t
Cyc_Std_sigset_t;struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;
struct Cyc_Std_option{struct _tagged_arr name;int has_arg;int*flag;int val;};extern
unsigned int alarm(unsigned int seconds);extern int close(int);extern void _exit(int);
extern int getpid();extern int getppid();extern int fork();extern int fchdir(int);extern
int fchown(int,unsigned short,unsigned short);extern int fsync(int);extern int
ftruncate(int,int);extern int dup(int);extern int dup2(int,int);extern int setsid();
extern int getsid(int pid);extern unsigned short getuid();extern int setuid(
unsigned short uid);extern unsigned short geteuid();extern int seteuid(unsigned short
euid);extern unsigned short getgid();extern int setgid(unsigned short gid);extern
unsigned short getegid();extern int setegid(unsigned short egid);extern int nice(int);
extern int pause();extern int pipe(int*filedes);extern int lseek(int filedes,int offset,
int whence);extern unsigned int sleep(unsigned int);extern int isatty(int);extern int
daemon(int nochdir,int noclose);int Cyc_Std_access(struct _tagged_arr,int);int Cyc_Std_chdir(
struct _tagged_arr);int Cyc_Std_chown(struct _tagged_arr,unsigned short,
unsigned short);struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,
unsigned int size);int Cyc_Std_execl(struct _tagged_arr path,struct _tagged_arr arg0,
struct _tagged_arr argv);int Cyc_Std_execlp(struct _tagged_arr file,struct _tagged_arr
arg0,struct _tagged_arr argv);int Cyc_Std_execve(struct _tagged_arr filename,struct
_tagged_arr argv,struct _tagged_arr envp);int Cyc_Std_link(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_read(int fd,struct _tagged_arr buf,unsigned int count);int
Cyc_Std_rmdir(struct _tagged_arr);int Cyc_Std_symlink(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_truncate(struct _tagged_arr,int);int Cyc_Std_write(int fd,
struct _tagged_arr buf,unsigned int count);int Cyc_Std_unlink(struct _tagged_arr
pathname);int Cyc_Std_gethostname(struct _tagged_arr,unsigned int);int Cyc_Std_chroot(
struct _tagged_arr);struct _tagged_arr Cyc_Std_getpass(struct _tagged_arr prompt);
extern int isalnum(int);extern int isalpha(int);extern int isascii(int);extern int
iscntrl(int);extern int isdigit(int);extern int isgraph(int);extern int islower(int);
extern int isprint(int);extern int ispunct(int);extern int isspace(int);extern int
isupper(int);extern int isxdigit(int);extern int toascii(int);extern int tolower(int);
extern int toupper(int);extern int _tolower(int);extern int _toupper(int);static const
int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;static const int Cyc_Absyn_Static=0;static const int Cyc_Absyn_Abstract=
1;static const int Cyc_Absyn_Public=2;static const int Cyc_Absyn_Extern=3;static const
int Cyc_Absyn_ExternC=4;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};static const int Cyc_Absyn_B1=0;static const int Cyc_Absyn_B2=1;static
const int Cyc_Absyn_B4=2;static const int Cyc_Absyn_B8=3;static const int Cyc_Absyn_AnyKind=
0;static const int Cyc_Absyn_MemKind=1;static const int Cyc_Absyn_BoxKind=2;static
const int Cyc_Absyn_RgnKind=3;static const int Cyc_Absyn_EffKind=4;static const int Cyc_Absyn_Signed=
0;static const int Cyc_Absyn_Unsigned=1;struct Cyc_Absyn_Conref{void*v;};static const
int Cyc_Absyn_Eq_constr=0;struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};
static const int Cyc_Absyn_Forward_constr=1;struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};static const int Cyc_Absyn_No_constr=0;static
const int Cyc_Absyn_Eq_kb=0;struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};static
const int Cyc_Absyn_Unknown_kb=1;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct
Cyc_Core_Opt*f1;};static const int Cyc_Absyn_Less_kb=2;struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};static const int Cyc_Absyn_Unknown_b=0;static const int
Cyc_Absyn_Upper_b=0;struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunion=0;struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};static const int Cyc_Absyn_KnownTunion=
1;struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct _tuple0*
field_name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunionfield=0;struct
Cyc_Absyn_UnknownTunionfield_struct{int tag;struct Cyc_Absyn_UnknownTunionFieldInfo
f1;};static const int Cyc_Absyn_KnownTunionfield=1;struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};static const int Cyc_Absyn_VoidType=0;
static const int Cyc_Absyn_Evar=0;struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*
f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};static const int Cyc_Absyn_VarType=
1;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};static const int
Cyc_Absyn_TunionType=2;struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo
f1;};static const int Cyc_Absyn_TunionFieldType=3;struct Cyc_Absyn_TunionFieldType_struct{
int tag;struct Cyc_Absyn_TunionFieldInfo f1;};static const int Cyc_Absyn_PointerType=4;
struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};static
const int Cyc_Absyn_IntType=5;struct Cyc_Absyn_IntType_struct{int tag;void*f1;void*
f2;};static const int Cyc_Absyn_FloatType=1;static const int Cyc_Absyn_DoubleType=6;
struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};static const int Cyc_Absyn_ArrayType=
7;struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct
Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_FnType=8;struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};static const int Cyc_Absyn_TupleType=9;struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_StructType=10;struct Cyc_Absyn_StructType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};
static const int Cyc_Absyn_UnionType=11;struct Cyc_Absyn_UnionType_struct{int tag;
struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};static
const int Cyc_Absyn_EnumType=12;struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};static const int Cyc_Absyn_SizeofType=13;
struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};static const int Cyc_Absyn_AnonStructType=
14;struct Cyc_Absyn_AnonStructType_struct{int tag;struct Cyc_List_List*f1;};static
const int Cyc_Absyn_AnonUnionType=15;struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_AnonEnumType=16;struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_RgnHandleType=17;struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};static const int Cyc_Absyn_TypedefType=
18;struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_Core_Opt*f3;};static const int Cyc_Absyn_HeapRgn=2;static const int Cyc_Absyn_AccessEff=
19;struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};static const int Cyc_Absyn_JoinEff=
20;struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_RgnsEff=21;struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};static const
int Cyc_Absyn_NoTypes=0;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};static const int Cyc_Absyn_WithTypes=1;struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};static const int Cyc_Absyn_NonNullable_ps=0;struct Cyc_Absyn_NonNullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Nullable_ps=1;struct Cyc_Absyn_Nullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_TaggedArray_ps=0;static
const int Cyc_Absyn_Printf_ft=0;static const int Cyc_Absyn_Scanf_ft=1;static const int
Cyc_Absyn_Regparm_att=0;struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};static
const int Cyc_Absyn_Stdcall_att=0;static const int Cyc_Absyn_Cdecl_att=1;static const
int Cyc_Absyn_Fastcall_att=2;static const int Cyc_Absyn_Noreturn_att=3;static const
int Cyc_Absyn_Const_att=4;static const int Cyc_Absyn_Aligned_att=1;struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};static const int Cyc_Absyn_Packed_att=5;static const int Cyc_Absyn_Section_att=
2;struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Absyn_Nocommon_att=6;static const int Cyc_Absyn_Shared_att=7;static const int Cyc_Absyn_Unused_att=
8;static const int Cyc_Absyn_Weak_att=9;static const int Cyc_Absyn_Dllimport_att=10;
static const int Cyc_Absyn_Dllexport_att=11;static const int Cyc_Absyn_No_instrument_function_att=
12;static const int Cyc_Absyn_Constructor_att=13;static const int Cyc_Absyn_Destructor_att=
14;static const int Cyc_Absyn_No_check_memory_usage_att=15;static const int Cyc_Absyn_Format_att=
3;struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};static const int
Cyc_Absyn_Carray_mod=0;static const int Cyc_Absyn_ConstArray_mod=0;struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Pointer_mod=1;struct Cyc_Absyn_Pointer_mod_struct{
int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual f3;};static const int Cyc_Absyn_Function_mod=
2;struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};static const int Cyc_Absyn_TypeParams_mod=
3;struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};static const int Cyc_Absyn_Attributes_mod=4;struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Char_c=
0;struct Cyc_Absyn_Char_c_struct{int tag;void*f1;unsigned char f2;};static const int
Cyc_Absyn_Short_c=1;struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};
static const int Cyc_Absyn_Int_c=2;struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int
f2;};static const int Cyc_Absyn_LongLong_c=3;struct Cyc_Absyn_LongLong_c_struct{int
tag;void*f1;long long f2;};static const int Cyc_Absyn_Float_c=4;struct Cyc_Absyn_Float_c_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Absyn_String_c=5;struct Cyc_Absyn_String_c_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Absyn_Null_c=0;static const int Cyc_Absyn_Plus=
0;static const int Cyc_Absyn_Times=1;static const int Cyc_Absyn_Minus=2;static const int
Cyc_Absyn_Div=3;static const int Cyc_Absyn_Mod=4;static const int Cyc_Absyn_Eq=5;
static const int Cyc_Absyn_Neq=6;static const int Cyc_Absyn_Gt=7;static const int Cyc_Absyn_Lt=
8;static const int Cyc_Absyn_Gte=9;static const int Cyc_Absyn_Lte=10;static const int Cyc_Absyn_Not=
11;static const int Cyc_Absyn_Bitnot=12;static const int Cyc_Absyn_Bitand=13;static
const int Cyc_Absyn_Bitor=14;static const int Cyc_Absyn_Bitxor=15;static const int Cyc_Absyn_Bitlshift=
16;static const int Cyc_Absyn_Bitlrshift=17;static const int Cyc_Absyn_Bitarshift=18;
static const int Cyc_Absyn_Size=19;static const int Cyc_Absyn_PreInc=0;static const int
Cyc_Absyn_PostInc=1;static const int Cyc_Absyn_PreDec=2;static const int Cyc_Absyn_PostDec=
3;struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};static const int Cyc_Absyn_StructField=0;struct Cyc_Absyn_StructField_struct{
int tag;struct _tagged_arr*f1;};static const int Cyc_Absyn_TupleIndex=1;struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};static const int
Cyc_Absyn_Const_e=0;struct Cyc_Absyn_Const_e_struct{int tag;void*f1;};static const
int Cyc_Absyn_Var_e=1;struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*
f2;};static const int Cyc_Absyn_UnknownId_e=2;struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple0*f1;};static const int Cyc_Absyn_Primop_e=3;struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_AssignOp_e=4;
struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_Increment_e=5;struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};static const int Cyc_Absyn_Conditional_e=6;
struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_SeqExp_e=7;struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_UnknownCall_e=
8;struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_FnCall_e=9;struct Cyc_Absyn_FnCall_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;
};static const int Cyc_Absyn_Throw_e=10;struct Cyc_Absyn_Throw_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_NoInstantiate_e=11;struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Instantiate_e=12;struct
Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_Cast_e=13;struct Cyc_Absyn_Cast_e_struct{int tag;void*
f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_Address_e=14;struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_New_e=15;struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_Sizeoftyp_e=
16;struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};static const int Cyc_Absyn_Sizeofexp_e=
17;struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};static
const int Cyc_Absyn_Offsetof_e=18;struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};static const int Cyc_Absyn_Gentyp_e=19;struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};static const int Cyc_Absyn_Deref_e=20;
struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_StructMember_e=
21;struct Cyc_Absyn_StructMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_tagged_arr*f2;};static const int Cyc_Absyn_StructArrow_e=22;struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};static const int Cyc_Absyn_Subscript_e=
23;struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};static const int Cyc_Absyn_Tuple_e=24;struct Cyc_Absyn_Tuple_e_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_CompoundLit_e=25;struct _tuple1{
struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Array_e=
26;struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_Comprehension_e=27;struct Cyc_Absyn_Comprehension_e_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
static const int Cyc_Absyn_Struct_e=28;struct Cyc_Absyn_Struct_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};static const int Cyc_Absyn_AnonStruct_e=29;struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Tunion_e=30;
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};static const int Cyc_Absyn_Enum_e=31;struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
static const int Cyc_Absyn_AnonEnum_e=32;struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static const int Cyc_Absyn_Malloc_e=
33;struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};static
const int Cyc_Absyn_UnresolvedMem_e=34;struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_StmtExp_e=
35;struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const
int Cyc_Absyn_Codegen_e=36;struct Cyc_Absyn_Codegen_e_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};static const int Cyc_Absyn_Fill_e=37;struct Cyc_Absyn_Fill_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;
struct Cyc_Position_Segment*loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;
struct _tuple2 condition;struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};static
const int Cyc_Absyn_Skip_s=0;static const int Cyc_Absyn_Exp_s=0;struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Seq_s=1;struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Return_s=
2;struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int
Cyc_Absyn_IfThenElse_s=3;struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_While_s=
4;struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*f2;
};static const int Cyc_Absyn_Break_s=5;struct Cyc_Absyn_Break_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Continue_s=6;struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Goto_s=7;struct Cyc_Absyn_Goto_s_struct{
int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_For_s=
8;struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};static const int Cyc_Absyn_Switch_s=9;
struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_SwitchC_s=10;struct Cyc_Absyn_SwitchC_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Fallthru_s=
11;struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};static const int Cyc_Absyn_Decl_s=12;struct Cyc_Absyn_Decl_s_struct{int tag;
struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Cut_s=
13;struct Cyc_Absyn_Cut_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const int
Cyc_Absyn_Splice_s=14;struct Cyc_Absyn_Splice_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};static const int Cyc_Absyn_Label_s=15;struct Cyc_Absyn_Label_s_struct{int tag;
struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Do_s=16;
struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;};
static const int Cyc_Absyn_TryCatch_s=17;struct Cyc_Absyn_TryCatch_s_struct{int tag;
struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Region_s=
18;struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_ForArray_s=19;struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};static const int
Cyc_Absyn_Wild_p=0;static const int Cyc_Absyn_Var_p=0;struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Null_p=1;static const
int Cyc_Absyn_Int_p=1;struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};static
const int Cyc_Absyn_Char_p=2;struct Cyc_Absyn_Char_p_struct{int tag;unsigned char f1;
};static const int Cyc_Absyn_Float_p=3;struct Cyc_Absyn_Float_p_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Absyn_Tuple_p=4;struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Pointer_p=5;struct Cyc_Absyn_Pointer_p_struct{
int tag;struct Cyc_Absyn_Pat*f1;};static const int Cyc_Absyn_Reference_p=6;struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Struct_p=7;struct Cyc_Absyn_Struct_p_struct{
int tag;struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};static const int Cyc_Absyn_Tunion_p=8;struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};static const int Cyc_Absyn_Enum_p=9;struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};static const int
Cyc_Absyn_AnonEnum_p=10;struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct
Cyc_Absyn_Enumfield*f2;};static const int Cyc_Absyn_UnknownId_p=11;struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};static const int Cyc_Absyn_UnknownCall_p=12;struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static
const int Cyc_Absyn_UnknownFields_p=13;struct Cyc_Absyn_UnknownFields_p_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{
void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};static const int Cyc_Absyn_Unresolved_b=0;static
const int Cyc_Absyn_Global_b=0;struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};static const int Cyc_Absyn_Funname_b=1;struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};static const int Cyc_Absyn_Param_b=2;struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Local_b=3;struct Cyc_Absyn_Local_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Pat_b=4;struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Structfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{
void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_List_List*tvs;void*defn;};static const int Cyc_Absyn_Var_d=0;
struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static const int
Cyc_Absyn_Fn_d=1;struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};
static const int Cyc_Absyn_Let_d=2;struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};
static const int Cyc_Absyn_Letv_d=3;struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};static const int Cyc_Absyn_Struct_d=4;struct Cyc_Absyn_Struct_d_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;};static const int Cyc_Absyn_Union_d=5;struct Cyc_Absyn_Union_d_struct{
int tag;struct Cyc_Absyn_Uniondecl*f1;};static const int Cyc_Absyn_Tunion_d=6;struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};static const int
Cyc_Absyn_Enum_d=7;struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};static const int Cyc_Absyn_Typedef_d=8;struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};static const int Cyc_Absyn_Namespace_d=9;struct Cyc_Absyn_Namespace_d_struct{
int tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Using_d=
10;struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;
};static const int Cyc_Absyn_ExternC_d=11;struct Cyc_Absyn_ExternC_d_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};static const int Cyc_Absyn_ArrayElement=0;struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_FieldName=1;struct Cyc_Absyn_FieldName_struct{
int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc_Std___sFILE*f);extern struct Cyc_List_List*Cyc_Parse_parse_file(struct
Cyc_Std___sFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
extern int Cyc_yyparse();extern unsigned char Cyc_AbstractDeclarator_tok[27];struct
Cyc_AbstractDeclarator_tok_struct{unsigned char*tag;struct Cyc_Abstractdeclarator*
f1;};extern unsigned char Cyc_AttributeList_tok[22];struct Cyc_AttributeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Attribute_tok[
18];struct Cyc_Attribute_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{unsigned char*tag;int f1;
};extern unsigned char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{unsigned char*
tag;unsigned char f1;};extern unsigned char Cyc_DeclList_tok[17];struct Cyc_DeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_DeclSpec_tok[
17];struct Cyc_DeclSpec_tok_struct{unsigned char*tag;struct Cyc_Declaration_spec*
f1;};extern unsigned char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{
unsigned char*tag;struct Cyc_Declarator*f1;};extern unsigned char Cyc_DesignatorList_tok[
23];struct Cyc_DesignatorList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_Designator_tok[19];struct Cyc_Designator_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_EnumfieldList_tok[22];struct
Cyc_EnumfieldList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{unsigned char*
tag;struct Cyc_Absyn_Enumfield*f1;};extern unsigned char Cyc_ExpList_tok[16];struct
Cyc_ExpList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{unsigned char*tag;struct
Cyc_Absyn_Exp*f1;};extern unsigned char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_FieldPattern_tok[
21];struct _tuple3{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple3*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple4{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple4*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple5{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple6{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple6*f1;}
;extern unsigned char Cyc_ParamDeclList_tok[22];struct Cyc_ParamDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_ParamDecl_tok[
18];struct Cyc_ParamDecl_tok_struct{unsigned char*tag;struct _tuple1*f1;};extern
unsigned char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Pattern_tok[16];
struct Cyc_Pattern_tok_struct{unsigned char*tag;struct Cyc_Absyn_Pat*f1;};extern
unsigned char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_QualId_tok[15];
struct Cyc_QualId_tok_struct{unsigned char*tag;struct _tuple0*f1;};extern
unsigned char Cyc_QualSpecList_tok[21];struct _tuple7{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
struct Cyc_Rgnorder_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{unsigned char*tag;void*
f1;};extern unsigned char Cyc_Short_tok[14];struct Cyc_Short_tok_struct{
unsigned char*tag;short f1;};extern unsigned char Cyc_Stmt_tok[13];struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};extern unsigned char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_String_tok[15];struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Stringopt_tok[18];struct Cyc_Stringopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_StructFieldDeclListList_tok[
32];struct Cyc_StructFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_StructFieldDeclList_tok[28];struct Cyc_StructFieldDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_StructOrUnion_tok[
22];struct Cyc_StructOrUnion_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_SwitchClauseList_tok[
25];struct Cyc_SwitchClauseList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TunionFieldList_tok[24];struct Cyc_TunionFieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TunionField_tok[
20];struct Cyc_TunionField_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tunionfield*
f1;};extern unsigned char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TypeModifierList_tok[
25];struct Cyc_TypeModifierList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TypeOpt_tok[16];struct Cyc_TypeOpt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_TypeQual_tok[17];
struct Cyc_TypeQual_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tqual f1;};extern
unsigned char Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{
unsigned char*tag;void*f1;};extern int Cyc_yyparse();extern unsigned char Cyc_YY1[8];
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};extern void Cyc_Lex_lex_init();extern unsigned char*Ccomp;struct
Cyc_Std___sFILE*Cyc_log_file=0;struct Cyc_Std___sFILE*Cyc_cstubs_file=0;struct Cyc_Std___sFILE*
Cyc_cycstubs_file=0;int Cyc_log(struct _tagged_arr fmt,struct _tagged_arr ap){int
_tmp0=Cyc_Std_vfprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_Std_fflush((struct Cyc_Std___sFILE*)((struct Cyc_Std___sFILE*)_check_null(Cyc_log_file)));
return _tmp0;}int Cyc_sizeof_unsignedlongint=- 1;int Cyc_sizeof_unsignedshortint=- 1;
int Cyc_sizeof_shortint=- 1;int Cyc_sizeof_int=- 1;int Cyc_sizeof_short=- 1;int Cyc_sizeof_fdmask=
- 1;int Cyc_sizeof___fdmask=- 1;int Cyc_sizeof_uint32=- 1;int Cyc_sizeof_sockaddr=- 1;
int Cyc_sizeof_inport=- 1;int Cyc_sizeof_inaddr=- 1;static struct _tagged_arr*Cyc_current_source=
0;static struct Cyc_List_List*Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=
0;static void Cyc_add_target(struct _tagged_arr*sptr){Cyc_current_targets=({struct
Cyc_Set_Set**_tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp1[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),sptr);_tmp1;});}struct _tuple8{struct
_tagged_arr*f1;struct Cyc_Set_Set*f2;};extern struct _tuple8*Cyc_line(struct Cyc_Lexing_lexbuf*);
extern int Cyc_macroname(struct Cyc_Lexing_lexbuf*);extern int Cyc_args(struct Cyc_Lexing_lexbuf*);
extern int Cyc_token(struct Cyc_Lexing_lexbuf*);extern int Cyc_string(struct Cyc_Lexing_lexbuf*);
struct Cyc_Std___sFILE*Cyc_slurp_out=0;extern int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
extern int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);extern int Cyc_asm(struct Cyc_Lexing_lexbuf*);
extern int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);extern int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple9{struct _tagged_arr f1;struct _tagged_arr*f2;
};extern struct _tuple9*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);extern int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf*);extern int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_line=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};struct _tuple10{struct _tagged_arr f1;struct _tagged_arr f2;};struct _tuple11{
struct _tagged_arr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};extern
struct _tuple11*Cyc_spec(struct Cyc_Lexing_lexbuf*);extern int Cyc_commands(struct
Cyc_Lexing_lexbuf*);extern int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);extern
int Cyc_block(struct Cyc_Lexing_lexbuf*);extern int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
extern int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);struct _tagged_arr Cyc_current_header=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=
0;struct Cyc_List_List*Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=
0;struct Cyc_List_List*Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_prologue=
0;struct Cyc_List_List*Cyc_current_epilogue=0;struct Cyc_List_List*Cyc_current_omit_symbols=
0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*Cyc_specbuf=0;int Cyc_braces_to_match=
0;int Cyc_parens_to_match=0;int Cyc_numdef=0;const int Cyc_lex_base[515]=(const int[
515]){0,0,75,192,305,285,310,166,311,91,27,384,28,503,617,695,288,338,93,- 3,0,- 2,
- 1,- 8,- 3,1,- 1,586,- 4,2,166,- 5,308,777,333,- 6,- 7,- 3,16,- 2,29,11,815,- 3,890,13,- 16,
223,12,- 15,213,22,26,28,24,- 14,28,58,56,65,52,70,320,1004,1119,357,93,83,90,112,
96,106,115,124,115,126,113,382,410,152,180,209,231,231,415,505,231,229,227,237,
224,241,498,1233,1348,510,231,230,251,238,238,238,270,515,1462,1577,- 9,530,- 10,
252,273,633,1691,1806,635,- 8,664,- 11,391,646,648,1883,1960,2037,2118,416,433,653,
2193,279,289,291,289,286,297,0,13,4,2274,5,682,2282,2347,800,49,465,6,2308,7,970,
2370,2408,978,- 32,318,418,304,307,293,315,307,319,677,1035,336,344,335,405,366,
362,405,419,416,428,431,682,1030,423,432,427,431,430,688,1145,438,447,1235,454,
462,809,1147,485,481,1350,518,517,515,551,535,826,1259,542,543,561,561,572,570,
571,561,1464,- 12,584,584,582,584,574,1579,- 13,579,578,1693,2470,589,584,1808,607,
591,2332,596,596,595,617,597,2472,620,610,627,610,619,2480,636,639,635,631,626,
10,14,659,639,2482,661,648,689,673,682,2487,1156,708,674,710,717,728,739,741,741,
745,755,762,746,- 30,798,802,798,808,816,853,- 25,833,849,842,838,849,870,872,888,
889,- 19,876,900,900,939,940,- 27,932,930,925,937,936,952,931,953,956,955,959,950,
950,- 29,959,979,982,992,1000,983,1015,1032,1039,1040,1,4,6,1041,1044,1032,1031,
1043,1043,1051,1052,2,52,1053,1054,2272,20,21,1080,1086,1050,1048,1063,1069,1070,
1125,1137,1139,- 24,1086,1087,1142,1169,1170,- 21,1117,1118,1174,1206,1208,- 22,
1156,1161,1217,1218,1219,- 20,1167,1168,1223,1225,1230,- 23,1178,1179,1469,- 31,
1178,1180,1177,1176,1175,1181,1183,- 18,1186,1187,1185,1198,4,- 15,1216,1217,1215,
1228,1297,- 17,1265,1268,1258,1259,1257,1270,1338,1263,1264,1262,1276,1344,- 7,- 8,
8,1374,2488,9,1435,2496,2561,1545,1489,- 49,1377,- 2,1316,- 4,1317,1442,2090,1318,
1349,1350,1659,1320,2588,2631,1327,1379,1329,1332,2701,1346,1383,- 36,- 42,- 37,
2776,- 28,1381,- 40,1415,- 45,- 39,- 48,2851,2880,1777,1407,1417,2598,2890,2920,2669,
2861,2953,2984,3022,1437,1450,3092,3130,1506,1527,1519,1529,1521,1531,- 6,- 34,
1435,3062,- 47,- 46,- 33,1452,3170,1456,1458,1678,1461,1465,1467,1469,1481,1514,
1516,1517,1562,3243,3327,2519,1563,- 41,- 38,- 35,- 26,1832,3409,1576,3492,1569,15,
1512,1512,1512,1510,1506,1516,1586};const int Cyc_lex_backtrk[515]=(const int[515]){
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,
- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,4,1,0,- 1,0,1,- 1,12,15,- 1,15,15,15,15,15,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,1,1,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,
6,4,2,8,3,5,- 1,6,5,- 1,31,31,31,31,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,27,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,
2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,
42,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[515]=(const int[515]){- 1,- 1,- 1,
417,406,153,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,
0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,0,118,- 1,- 1,- 1,- 1,- 1,125,125,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,-
1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,0,- 1,0,- 1,- 1,480,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,
0,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
0,0,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3749]=(const int[3749]){
0,0,0,0,0,0,0,0,0,0,21,19,28,507,19,28,19,28,115,19,45,45,45,45,45,21,45,0,0,0,0,
0,22,318,329,508,387,21,- 1,- 1,21,- 1,- 1,45,319,45,320,22,505,505,505,505,505,505,
505,505,505,505,28,118,22,246,129,40,247,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,28,330,364,358,
505,145,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,503,503,503,503,503,503,503,503,503,503,136,86,
20,79,66,56,57,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,503,503,503,503,503,503,503,58,59,60,61,503,62,503,503,503,503,503,
503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,67,68,37,418,419,418,418,419,39,22,69,70,71,72,73,144,34,34,34,34,34,34,34,
34,74,75,418,420,421,76,77,422,423,424,119,119,425,426,119,427,428,429,430,431,
431,431,431,431,431,431,431,431,432,80,433,434,435,119,19,436,436,436,436,436,
436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,
436,19,- 1,- 1,437,436,81,436,436,436,436,436,436,436,436,436,436,436,436,436,436,
436,436,436,436,436,436,436,436,436,436,436,436,407,438,154,408,22,24,24,37,146,
137,96,82,155,97,63,63,83,84,63,98,38,87,88,89,409,90,91,92,116,26,26,109,21,24,
21,99,25,63,100,101,102,33,33,33,33,33,33,33,33,33,33,65,65,103,110,65,111,26,33,
33,33,33,33,33,130,156,35,35,35,35,35,35,35,35,65,131,78,78,132,133,78,134,410,
135,256,157,- 1,147,138,- 1,33,33,33,33,33,33,39,158,22,78,159,160,394,161,78,78,
162,35,78,85,85,- 1,395,85,- 1,27,240,128,128,128,128,128,128,128,128,128,128,78,-
1,234,225,- 1,85,21,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,128,128,128,- 1,169,170,- 1,128,28,128,128,128,128,
128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
128,128,93,93,194,171,93,120,120,85,85,120,374,85,95,95,195,172,95,104,104,173,
375,104,174,93,175,376,21,187,120,179,85,180,107,107,21,95,107,35,181,182,104,
185,121,122,121,121,121,121,121,121,121,121,121,121,106,107,186,188,189,21,21,
121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,
121,121,121,121,121,121,21,192,28,193,121,29,121,121,121,121,121,121,121,121,121,
121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,47,47,22,218,
47,196,197,35,30,30,30,30,30,30,30,30,112,112,114,114,112,- 1,114,47,198,199,211,
108,203,119,119,127,127,119,204,127,127,127,48,112,127,114,205,206,207,208,- 1,
116,116,209,49,116,119,212,127,213,214,31,215,127,162,162,216,- 1,162,176,176,219,
220,176,116,183,183,223,224,183,226,228,41,41,32,229,41,162,230,231,232,233,176,
235,50,163,51,236,183,- 1,52,227,237,238,239,41,53,54,143,143,143,143,143,143,143,
143,241,250,243,244,245,42,42,42,42,42,42,42,42,42,42,242,248,249,251,252,115,28,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
253,117,254,255,42,293,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,190,190,43,287,190,307,308,33,33,33,33,33,33,33,33,33,33,
200,200,277,270,200,263,190,33,33,33,33,33,33,35,35,35,35,35,35,35,35,264,265,
200,266,267,268,269,44,44,44,44,44,44,44,44,44,44,55,33,33,33,33,33,33,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,271,272,273,
274,44,275,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,276,278,279,28,280,281,282,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,283,284,285,286,
44,288,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,63,63,289,290,63,152,152,152,152,152,152,152,152,35,35,35,35,35,35,35,35,
291,292,63,294,295,176,176,296,297,176,163,163,298,299,163,300,301,302,64,64,64,
64,64,64,64,64,64,64,176,303,304,305,306,163,370,64,64,64,64,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,309,310,311,312,64,313,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,19,65,65,
164,314,65,315,316,317,321,165,177,322,166,323,324,325,326,178,327,328,331,332,
167,65,168,352,183,183,190,190,183,346,190,338,339,340,341,342,343,64,64,64,64,
64,64,64,64,64,64,183,344,190,345,347,348,349,64,64,64,64,64,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,350,351,353,354,64,355,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,93,93,186,186,
93,356,186,357,184,359,191,257,258,259,360,260,361,362,363,261,365,366,367,93,
368,186,200,200,262,369,200,371,372,388,108,382,377,378,379,94,94,94,94,94,94,94,
94,94,94,200,380,381,383,384,385,386,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,94,94,94,94,94,389,390,391,392,94,393,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,31,95,95,193,193,95,401,
193,201,396,397,398,399,400,55,402,403,404,202,405,46,501,500,479,95,474,193,416,
416,449,502,416,502,502,276,106,306,443,444,269,94,94,94,94,94,94,94,94,94,94,
416,441,22,502,373,153,478,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,94,446,292,447,445,94,439,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,104,104,209,209,104,448,209,372,372,
498,477,372,415,415,415,415,415,415,415,415,345,115,115,104,363,209,286,416,416,
351,372,416,499,381,210,117,440,210,373,105,105,105,105,105,105,105,105,105,105,
217,416,35,22,115,115,35,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,55,35,387,46,105,35,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,106,107,107,216,216,107,31,216,31,31,31,31,31,31,31,31,393,357,31,36,
36,473,473,22,107,22,216,509,510,511,512,513,514,22,0,217,0,0,31,0,105,105,105,
105,105,105,105,105,105,105,31,36,36,473,473,0,0,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,0,0,0,0,
105,0,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,112,112,221,221,112,475,221,476,476,476,476,476,
476,476,476,476,476,357,0,0,0,0,0,112,0,221,496,496,496,496,496,496,496,496,115,
0,0,0,0,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,36,114,114,224,224,114,455,224,
455,0,0,456,456,456,456,456,456,456,456,456,456,0,0,0,114,0,224,502,0,502,502,0,
0,0,0,117,0,0,0,0,113,113,113,113,113,113,113,113,113,113,502,0,0,0,0,0,0,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,123,123,123,123,123,
123,123,123,123,123,123,123,22,0,0,0,0,0,0,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,0,0,0,0,123,
0,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,124,123,123,123,123,123,123,123,123,123,123,22,0,
0,0,0,0,0,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,0,0,0,0,123,0,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,22,0,0,0,0,- 1,0,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,- 1,- 1,0,- 1,123,0,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,0,0,0,0,124,124,124,124,124,124,124,
124,124,124,124,124,126,0,0,0,0,0,481,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,124,0,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,128,128,128,128,128,128,128,128,128,128,0,0,0,0,0,0,0,
128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
128,128,128,128,128,128,28,0,0,139,128,0,128,128,128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,28,0,0,148,
140,140,140,140,140,140,140,140,142,142,142,142,142,142,142,142,142,142,333,227,
227,0,334,227,- 1,142,142,142,142,142,142,335,0,336,149,149,149,149,149,149,149,
149,227,0,0,0,0,0,0,31,0,36,- 1,0,0,0,0,142,142,142,142,142,142,0,0,0,0,0,0,337,0,
0,141,142,142,142,142,142,142,142,142,142,142,31,0,0,0,0,0,0,142,142,142,142,142,
142,151,151,151,151,151,151,151,151,151,151,150,0,0,0,0,0,0,151,151,151,151,151,
151,0,0,0,142,142,142,142,142,142,0,0,0,0,0,0,151,151,151,151,151,151,151,151,
151,151,0,151,151,151,151,151,151,151,151,151,151,151,151,221,221,233,233,221,0,
233,0,0,0,239,239,249,249,239,0,249,255,255,19,0,255,411,221,0,233,151,151,151,
151,151,151,115,239,35,249,0,0,0,0,255,0,19,0,31,0,0,0,0,28,0,0,0,0,0,0,0,412,
412,412,412,412,412,412,412,414,414,414,414,414,414,414,414,414,414,0,0,0,0,357,
0,0,414,414,414,414,414,414,497,497,497,497,497,497,497,497,222,0,0,0,0,0,0,0,0,
0,28,0,0,0,0,0,0,0,414,414,414,414,414,414,0,0,0,0,0,0,0,0,0,413,414,414,414,414,
414,414,414,414,414,414,0,0,0,0,0,0,0,414,414,414,414,414,414,0,0,450,0,461,461,
461,461,461,461,461,461,462,462,456,456,456,456,456,456,456,456,456,456,0,452,
414,414,414,414,414,414,463,0,0,0,0,0,0,0,0,464,0,0,465,450,0,451,451,451,451,
451,451,451,451,451,451,452,0,0,0,0,0,0,463,0,0,0,452,0,0,0,0,464,0,453,465,0,0,
0,459,0,459,0,454,460,460,460,460,460,460,460,460,460,460,0,0,0,0,0,452,0,0,0,0,
0,0,453,0,0,0,0,0,0,0,0,454,442,442,442,442,442,442,442,442,442,442,0,0,0,0,0,0,
0,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,0,0,0,0,442,0,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,0,0,0,0,0,0,0,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,0,0,0,0,442,
0,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,457,457,457,457,457,457,457,457,457,457,460,460,460,
460,460,460,460,460,460,460,0,458,108,0,0,0,0,450,108,451,451,451,451,451,451,
451,451,451,451,456,456,456,456,456,456,456,456,456,456,0,452,0,0,458,108,0,0,
453,0,0,108,106,0,0,0,0,454,106,0,457,457,457,457,457,457,457,457,457,457,0,0,0,
452,0,0,0,0,0,0,453,458,108,0,106,0,0,0,108,454,106,0,0,460,460,460,460,460,460,
460,460,460,460,0,0,0,0,0,0,0,0,0,0,458,108,108,0,0,0,0,108,108,450,0,461,461,
461,461,461,461,461,461,462,462,0,0,0,0,0,0,0,0,0,0,0,452,0,108,0,0,0,0,471,108,
0,0,0,0,0,0,450,472,462,462,462,462,462,462,462,462,462,462,0,0,0,0,0,452,0,0,0,
0,0,452,471,0,0,0,0,0,469,0,0,472,0,0,0,0,0,470,0,0,476,476,476,476,476,476,476,
476,476,476,0,0,0,452,0,0,0,0,0,0,469,458,108,0,0,0,0,0,108,470,466,466,466,466,
466,466,466,466,466,466,0,0,0,0,0,0,0,466,466,466,466,466,466,458,108,0,0,0,0,0,
108,0,0,0,0,0,0,0,466,466,466,466,466,466,466,466,466,466,0,466,466,466,466,466,
466,466,466,466,466,466,466,0,0,0,482,0,467,0,0,483,0,0,0,0,0,468,0,0,484,484,
484,484,484,484,484,484,0,466,466,466,466,466,466,485,0,0,0,0,467,0,0,0,0,0,0,0,
0,468,0,0,0,0,0,0,0,0,0,0,0,0,0,0,486,0,0,0,0,487,488,0,0,0,489,0,0,0,0,0,0,0,
490,0,0,0,491,0,492,0,493,0,494,495,495,495,495,495,495,495,495,495,495,0,0,0,0,
0,0,0,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,495,495,495,495,495,495,495,0,0,0,0,0,0,495,495,495,495,495,495,495,495,495,
495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,369,0,0,0,0,
0,0,0,0,495,495,495,495,495,495,495,495,495,495,0,0,0,0,0,0,0,495,495,495,495,
495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,495,0,0,0,0,0,0,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,495,495,495,495,495,495,495,495,495,495,21,0,0,504,0,0,0,503,503,503,503,503,
503,503,503,503,503,0,0,0,0,0,0,0,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,0,0,0,0,503,0,503,
503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,503,503,506,0,0,0,0,0,0,0,505,505,505,505,505,505,505,505,505,505,0,
0,0,0,0,0,0,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,0,0,0,0,505,0,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_lex_check[3749]=(const int[3749]){- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,137,139,146,148,408,411,41,41,45,45,41,507,45,- 1,-
1,- 1,- 1,- 1,135,317,328,0,386,10,12,40,10,12,40,41,318,45,319,20,1,1,1,1,1,1,1,1,
1,1,38,48,136,245,10,38,246,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
144,329,333,334,1,144,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,
2,2,2,2,2,2,2,9,51,18,52,53,54,56,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,57,58,59,60,2,61,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,66,67,
7,3,3,3,3,3,7,7,68,69,70,71,72,7,30,30,30,30,30,30,30,30,73,74,3,3,3,75,76,3,3,3,
47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,79,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,80,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,4,3,5,4,5,6,8,16,6,8,50,81,5,50,62,62,82,83,62,50,16,86,87,88,4,89,
90,91,96,6,8,97,9,17,18,98,17,62,99,100,101,32,32,32,32,32,32,32,32,32,32,65,65,
102,109,65,110,17,32,32,32,32,32,32,129,5,34,34,34,34,34,34,34,34,65,130,77,77,
131,132,77,133,4,134,156,5,118,6,8,118,32,32,32,32,32,32,16,157,16,77,158,159,
154,160,78,78,161,7,78,84,84,125,154,84,125,17,164,11,11,11,11,11,11,11,11,11,11,
78,126,165,166,126,84,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,145,168,169,145,11,65,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,92,92,167,170,92,13,13,85,85,13,155,85,
95,95,167,171,95,103,103,172,155,103,173,92,174,155,78,177,13,178,85,179,107,107,
5,95,107,16,180,181,103,184,13,13,13,13,13,13,13,13,13,13,13,13,4,107,185,187,
188,6,8,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,17,191,27,192,13,27,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,14,14,85,194,14,195,196,95,27,27,27,27,27,27,27,27,111,
111,114,114,111,118,114,14,197,198,201,107,202,119,119,120,120,119,203,120,127,
127,14,111,127,114,204,205,206,207,125,116,116,208,14,116,119,211,120,212,213,27,
214,127,162,162,215,126,162,175,175,218,219,175,116,182,182,222,223,182,225,226,
15,15,27,228,15,162,229,230,231,232,175,234,14,162,14,235,182,145,14,225,236,237,
238,15,14,14,140,140,140,140,140,140,140,140,240,241,242,243,244,15,15,15,15,15,
15,15,15,15,15,240,247,248,250,251,114,13,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,252,116,253,254,15,258,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,189,189,15,259,189,
257,257,33,33,33,33,33,33,33,33,33,33,199,199,260,261,199,262,189,33,33,33,33,33,
33,143,143,143,143,143,143,143,143,263,264,199,265,266,267,268,42,42,42,42,42,42,
42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,270,271,272,273,42,274,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,
275,277,278,15,279,280,281,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,282,283,284,285,44,287,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,63,63,288,289,63,149,149,149,149,
149,149,149,149,152,152,152,152,152,152,152,152,290,291,63,293,294,176,176,295,
296,176,163,163,297,298,163,299,300,301,63,63,63,63,63,63,63,63,63,63,176,302,
303,304,305,163,307,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,308,309,310,311,63,312,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,64,64,163,313,64,314,315,316,320,163,
176,321,163,322,323,324,325,176,326,327,330,331,163,64,163,335,183,183,190,190,
183,336,190,337,338,339,340,341,342,64,64,64,64,64,64,64,64,64,64,183,343,190,
344,346,347,348,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
64,64,64,64,64,349,350,352,353,64,354,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,93,93,186,186,93,355,186,356,183,358,190,256,
256,256,359,256,360,361,362,256,364,365,366,93,367,186,200,200,256,368,200,370,
371,374,186,375,376,377,378,93,93,93,93,93,93,93,93,93,93,200,379,380,382,383,
384,385,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,388,389,390,391,93,392,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,94,94,193,193,94,394,193,200,395,396,397,398,399,
400,401,402,403,200,404,405,420,422,425,94,429,193,409,409,432,418,409,418,418,
434,193,426,435,435,427,94,94,94,94,94,94,94,94,94,94,409,437,409,418,426,427,
427,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,433,433,433,444,94,438,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,94,104,104,209,209,104,446,209,372,372,423,475,372,412,412,
412,412,412,412,412,412,480,453,454,104,482,209,483,416,416,485,372,416,423,486,
209,487,438,488,372,104,104,104,104,104,104,104,104,104,104,489,416,463,416,453,
454,464,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,490,463,491,492,104,464,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,105,105,216,216,105,467,216,415,415,415,415,415,415,415,415,493,497,468,469,
470,471,472,504,105,506,216,508,509,510,511,512,513,514,- 1,216,- 1,- 1,467,- 1,105,
105,105,105,105,105,105,105,105,105,468,469,470,471,472,- 1,- 1,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,- 1,- 1,- 1,- 1,105,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,112,112,220,220,112,428,220,
428,428,428,428,428,428,428,428,428,428,484,- 1,- 1,- 1,- 1,- 1,112,- 1,220,484,484,
484,484,484,484,484,484,220,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,113,113,224,224,113,452,224,452,- 1,- 1,452,452,452,452,452,452,452,
452,452,452,- 1,- 1,- 1,113,- 1,224,502,- 1,502,502,- 1,- 1,- 1,- 1,224,- 1,- 1,- 1,- 1,113,
113,113,113,113,113,113,113,113,113,502,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,121,121,121,121,121,121,121,121,
121,121,121,121,121,- 1,- 1,- 1,- 1,- 1,- 1,121,121,121,121,121,121,121,121,121,121,
121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,121,
- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,
121,121,121,121,121,121,121,122,122,122,122,122,122,122,122,122,122,122,122,122,
- 1,- 1,- 1,- 1,- 1,- 1,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,424,- 1,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,124,424,- 1,124,123,- 1,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,
- 1,124,124,124,124,124,124,124,124,124,124,124,124,124,- 1,- 1,- 1,- 1,- 1,424,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,- 1,- 1,- 1,- 1,124,- 1,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,128,128,128,128,
128,128,128,128,128,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,138,- 1,- 1,
138,128,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,128,- 1,- 1,147,- 1,- 1,147,138,138,138,138,138,138,
138,138,141,141,141,141,141,141,141,141,141,141,332,227,227,- 1,332,227,424,141,
141,141,141,141,141,332,- 1,332,147,147,147,147,147,147,147,147,227,- 1,- 1,- 1,- 1,-
1,- 1,138,- 1,227,124,- 1,- 1,- 1,- 1,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,332,- 1,
- 1,138,142,142,142,142,142,142,142,142,142,142,147,- 1,- 1,- 1,- 1,- 1,- 1,142,142,142,
142,142,142,150,150,150,150,150,150,150,150,150,150,147,- 1,- 1,- 1,- 1,- 1,- 1,150,
150,150,150,150,150,- 1,- 1,- 1,142,142,142,142,142,142,- 1,- 1,- 1,- 1,- 1,- 1,151,151,
151,151,151,151,151,151,151,151,- 1,150,150,150,150,150,150,151,151,151,151,151,
151,221,221,233,233,221,- 1,233,- 1,- 1,- 1,239,239,249,249,239,- 1,249,255,255,410,-
1,255,410,221,- 1,233,151,151,151,151,151,151,221,239,233,249,- 1,- 1,- 1,- 1,255,- 1,
239,- 1,249,- 1,- 1,- 1,- 1,255,- 1,- 1,- 1,- 1,- 1,- 1,- 1,410,410,410,410,410,410,410,410,
413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,496,- 1,- 1,413,413,413,413,
413,413,496,496,496,496,496,496,496,496,221,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,410,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,413,413,413,413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,410,414,414,
414,414,414,414,414,414,414,414,- 1,- 1,- 1,- 1,- 1,- 1,- 1,414,414,414,414,414,414,- 1,
- 1,430,- 1,430,430,430,430,430,430,430,430,430,430,455,455,455,455,455,455,455,
455,455,455,- 1,430,414,414,414,414,414,414,430,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,430,- 1,- 1,
430,431,- 1,431,431,431,431,431,431,431,431,431,431,430,- 1,- 1,- 1,- 1,- 1,- 1,430,- 1,
- 1,- 1,431,- 1,- 1,- 1,- 1,430,- 1,431,430,- 1,- 1,- 1,458,- 1,458,- 1,431,458,458,458,458,
458,458,458,458,458,458,- 1,- 1,- 1,- 1,- 1,431,- 1,- 1,- 1,- 1,- 1,- 1,431,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,431,436,436,436,436,436,436,436,436,436,436,- 1,- 1,- 1,- 1,- 1,- 1,- 1,436,
436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,
436,436,436,436,436,- 1,- 1,- 1,- 1,436,- 1,436,436,436,436,436,436,436,436,436,436,
436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,442,442,442,442,
442,442,442,442,442,442,- 1,- 1,- 1,- 1,- 1,- 1,- 1,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,- 1,- 1,- 1,- 1,
442,- 1,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,442,450,450,450,450,450,450,450,450,450,450,459,459,
459,459,459,459,459,459,459,459,- 1,450,450,- 1,- 1,- 1,- 1,451,450,451,451,451,451,
451,451,451,451,451,451,456,456,456,456,456,456,456,456,456,456,- 1,451,- 1,- 1,450,
450,- 1,- 1,451,- 1,- 1,450,456,- 1,- 1,- 1,- 1,451,456,- 1,457,457,457,457,457,457,457,
457,457,457,- 1,- 1,- 1,451,- 1,- 1,- 1,- 1,- 1,- 1,451,457,457,- 1,456,- 1,- 1,- 1,457,451,
456,- 1,- 1,460,460,460,460,460,460,460,460,460,460,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
457,457,460,- 1,- 1,- 1,- 1,457,460,461,- 1,461,461,461,461,461,461,461,461,461,461,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,461,- 1,460,- 1,- 1,- 1,- 1,461,460,- 1,- 1,- 1,- 1,- 1,- 1,
462,461,462,462,462,462,462,462,462,462,462,462,- 1,- 1,- 1,- 1,- 1,461,- 1,- 1,- 1,- 1,-
1,462,461,- 1,- 1,- 1,- 1,- 1,462,- 1,- 1,461,- 1,- 1,- 1,- 1,- 1,462,- 1,- 1,476,476,476,476,
476,476,476,476,476,476,- 1,- 1,- 1,462,- 1,- 1,- 1,- 1,- 1,- 1,462,476,476,- 1,- 1,- 1,- 1,-
1,476,462,465,465,465,465,465,465,465,465,465,465,- 1,- 1,- 1,- 1,- 1,- 1,- 1,465,465,
465,465,465,465,476,476,- 1,- 1,- 1,- 1,- 1,476,- 1,- 1,- 1,- 1,- 1,- 1,- 1,466,466,466,466,
466,466,466,466,466,466,- 1,465,465,465,465,465,465,466,466,466,466,466,466,- 1,- 1,
- 1,481,- 1,466,- 1,- 1,481,- 1,- 1,- 1,- 1,- 1,466,- 1,- 1,481,481,481,481,481,481,481,481,
- 1,466,466,466,466,466,466,481,- 1,- 1,- 1,- 1,466,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,466,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,481,- 1,- 1,- 1,- 1,481,481,- 1,- 1,- 1,481,- 1,- 1,-
1,- 1,- 1,- 1,- 1,481,- 1,- 1,- 1,481,- 1,481,- 1,481,- 1,481,494,494,494,494,494,494,494,
494,494,494,- 1,- 1,- 1,- 1,- 1,- 1,- 1,494,494,494,494,494,494,494,494,494,494,494,494,
494,494,494,494,494,494,494,494,494,494,494,494,494,494,- 1,- 1,- 1,- 1,- 1,- 1,494,
494,494,494,494,494,494,494,494,494,494,494,494,494,494,494,494,494,494,494,494,
494,494,494,494,494,495,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,495,495,495,495,495,495,495,495,
495,495,- 1,- 1,- 1,- 1,- 1,- 1,- 1,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,495,495,495,495,495,495,495,495,495,495,495,495,- 1,- 1,- 1,- 1,- 1,- 1,495,495,
495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,495,495,495,503,- 1,- 1,503,- 1,- 1,- 1,503,503,503,503,503,503,503,503,503,503,-
1,- 1,- 1,- 1,- 1,- 1,- 1,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,503,503,503,503,503,503,503,503,- 1,- 1,- 1,- 1,503,- 1,503,503,503,503,
503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,505,- 1,- 1,- 1,- 1,- 1,- 1,- 1,505,505,505,505,505,505,505,505,505,505,- 1,- 1,-
1,- 1,- 1,- 1,- 1,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,- 1,- 1,- 1,- 1,505,- 1,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int
base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=
lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){
base=Cyc_lex_base[state];if(base < 0){return(- base)- 1;}backtrk=Cyc_lex_backtrk[
state];if(backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=
backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(! lbuf->lex_eof_reached){
return(- state)- 1;}else{c=256;}}else{c=(int)*((unsigned char*)
_check_unknown_subscript(lbuf->lex_buffer,sizeof(unsigned char),lbuf->lex_curr_pos
++));if(c == - 1){c=256;}}if(Cyc_lex_check[_check_known_subscript_notnull(3749,
base + c)]== state){state=Cyc_lex_trans[_check_known_subscript_notnull(3749,base + 
c)];}else{state=Cyc_lex_default[state];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp2=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmp2[0]=({struct Cyc_Lexing_Error_struct
_tmp3;_tmp3.tag=Cyc_Lexing_Error;_tmp3.f1=_tag_arr("empty token",sizeof(
unsigned char),12);_tmp3;});_tmp2;}));}else{return lbuf->lex_last_action;}}else{
if(c == 256){lbuf->lex_eof_reached=0;}}}}struct _tuple8*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL4: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((
struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){Cyc_current_targets=({
struct Cyc_Set_Set**_tmp6=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp6[0]=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _tagged_arr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);_tmp6;});}return({
struct _tuple8*_tmp7=_cycalloc(sizeof(struct _tuple8));_tmp7->f1=(struct
_tagged_arr*)_check_null(Cyc_current_source);_tmp7->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets));_tmp7;});case 1: _LL5: return Cyc_line(lexbuf);
case 2: _LL8: return 0;default: _LL9:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpB=
_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmpB[0]=({struct Cyc_Lexing_Error_struct
_tmpC;_tmpC.tag=Cyc_Lexing_Error;_tmpC.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpC;});_tmpB;}));}struct _tuple8*Cyc_line(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL13: Cyc_current_source=({struct _tagged_arr*_tmpF=_cycalloc(sizeof(struct
_tagged_arr));_tmpF[0]=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 2));_tmpF;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmp10=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp10[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
_tmp10;});Cyc_token(lexbuf);return 0;case 1: _LL14: Cyc_current_source=({struct
_tagged_arr*_tmp12=_cycalloc(sizeof(struct _tagged_arr));_tmp12[0]=(struct
_tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
1));_tmp12;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**_tmp13=
_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp13[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmp13;});
Cyc_args(lexbuf);return 0;case 2: _LL17: Cyc_current_source=({struct _tagged_arr*
_tmp15=_cycalloc(sizeof(struct _tagged_arr));_tmp15[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp15;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmp16=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp16[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
_tmp16;});Cyc_token(lexbuf);return 0;default: _LL20:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp18=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmp18[0]=({struct Cyc_Lexing_Error_struct
_tmp19;_tmp19.tag=Cyc_Lexing_Error;_tmp19.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp19;});_tmp18;}));}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_macroname_rec(lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL26: {struct _tagged_arr*_tmp1C=({struct _tagged_arr*_tmp1E=_cycalloc(
sizeof(struct _tagged_arr));_tmp1E[0]=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp1E;});Cyc_current_args=({
struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(struct Cyc_List_List));_tmp1D->hd=
_tmp1C;_tmp1D->tl=Cyc_current_args;_tmp1D;});return Cyc_args(lexbuf);}case 1: _LL27: {
struct _tagged_arr*_tmp20=({struct _tagged_arr*_tmp22=_cycalloc(sizeof(struct
_tagged_arr));_tmp22[0]=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));_tmp22;});Cyc_current_args=({struct Cyc_List_List*_tmp21=_cycalloc(
sizeof(struct Cyc_List_List));_tmp21->hd=_tmp20;_tmp21->tl=Cyc_current_args;
_tmp21;});return Cyc_token(lexbuf);}default: _LL31:(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp24=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmp24[0]=({struct Cyc_Lexing_Error_struct
_tmp25;_tmp25.tag=Cyc_Lexing_Error;_tmp25.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp25;});_tmp24;}));}int Cyc_args(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL38: Cyc_add_target(({struct _tagged_arr*_tmp28=_cycalloc(sizeof(struct
_tagged_arr));_tmp28[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);_tmp28;}));
return Cyc_token(lexbuf);case 1: _LL39: return 0;case 2: _LL41: return Cyc_token(lexbuf);
case 3: _LL42: Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL43: return Cyc_token(
lexbuf);case 5: _LL44: return Cyc_token(lexbuf);case 6: _LL45: return Cyc_token(lexbuf);
case 7: _LL46: return Cyc_token(lexbuf);case 8: _LL47: return Cyc_token(lexbuf);case 9:
_LL48: return Cyc_token(lexbuf);case 10: _LL49: return Cyc_token(lexbuf);case 11: _LL50:
return Cyc_token(lexbuf);case 12: _LL51: return Cyc_token(lexbuf);case 13: _LL52: return
Cyc_token(lexbuf);case 14: _LL53: return Cyc_token(lexbuf);case 15: _LL54: return Cyc_token(
lexbuf);case 16: _LL55: return Cyc_token(lexbuf);case 17: _LL56: return Cyc_token(lexbuf);
case 18: _LL57: return Cyc_token(lexbuf);case 19: _LL58: return Cyc_token(lexbuf);case 20:
_LL59: return Cyc_token(lexbuf);case 21: _LL60: return Cyc_token(lexbuf);case 22: _LL61:
return Cyc_token(lexbuf);case 23: _LL62: return Cyc_token(lexbuf);case 24: _LL63: return
Cyc_token(lexbuf);case 25: _LL64: return Cyc_token(lexbuf);case 26: _LL65: return Cyc_token(
lexbuf);case 27: _LL66: return Cyc_token(lexbuf);case 28: _LL67: return Cyc_token(lexbuf);
case 29: _LL68: return Cyc_token(lexbuf);case 30: _LL69: return Cyc_token(lexbuf);case 31:
_LL70: return Cyc_token(lexbuf);case 32: _LL71: return Cyc_token(lexbuf);case 33: _LL72:
return Cyc_token(lexbuf);case 34: _LL73: return Cyc_token(lexbuf);case 35: _LL74: return
Cyc_token(lexbuf);case 36: _LL75: return Cyc_token(lexbuf);case 37: _LL76: return Cyc_token(
lexbuf);case 38: _LL77: return Cyc_token(lexbuf);case 39: _LL78: return Cyc_token(lexbuf);
case 40: _LL79: return Cyc_token(lexbuf);case 41: _LL80: return Cyc_token(lexbuf);case 42:
_LL81: return Cyc_token(lexbuf);case 43: _LL82: return Cyc_token(lexbuf);case 44: _LL83:
return Cyc_token(lexbuf);case 45: _LL84: return Cyc_token(lexbuf);case 46: _LL85: return
Cyc_token(lexbuf);case 47: _LL86: return Cyc_token(lexbuf);case 48: _LL87: return Cyc_token(
lexbuf);default: _LL88:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5A=_cycalloc(
sizeof(struct Cyc_Lexing_Error_struct));_tmp5A[0]=({struct Cyc_Lexing_Error_struct
_tmp5B;_tmp5B.tag=Cyc_Lexing_Error;_tmp5B.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp5B;});_tmp5A;}));}int Cyc_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL92: return Cyc_string(lexbuf);case 1: _LL93: return 0;case 2: _LL94: return Cyc_string(
lexbuf);case 3: _LL95: return Cyc_string(lexbuf);case 4: _LL96: return Cyc_string(lexbuf);
case 5: _LL97: return Cyc_string(lexbuf);case 6: _LL98: return Cyc_string(lexbuf);case 7:
_LL99: return 0;case 8: _LL100: return 0;case 9: _LL101: return Cyc_string(lexbuf);
default: _LL102:(lexbuf->refill_buff)(lexbuf);return Cyc_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp68=_cycalloc(
sizeof(struct Cyc_Lexing_Error_struct));_tmp68[0]=({struct Cyc_Lexing_Error_struct
_tmp69;_tmp69.tag=Cyc_Lexing_Error;_tmp69.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp69;});_tmp68;}));}int Cyc_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_string_rec(lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL106: Cyc_Std_fputc((int)'"',(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));
if(Cyc_slurp_string(lexbuf)){return 1;}else{return Cyc_slurp(lexbuf);}case 1: _LL107:
return 0;case 2: _LL108:({struct Cyc_Std_Int_pa_struct _tmp6F;_tmp6F.tag=Cyc_Std_Int_pa;
_tmp6F.f1=(int)((unsigned int)Cyc_sizeof_fdmask);{void*_tmp6E[1]={& _tmp6F};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp6E,sizeof(void*),1));}});({void*_tmp70[0]={};Cyc_log(
_tag_arr("Warning: sizeof(fd_mask) inlined\n",sizeof(unsigned char),34),_tag_arr(
_tmp70,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 3: _LL109:({struct Cyc_Std_Int_pa_struct
_tmp73;_tmp73.tag=Cyc_Std_Int_pa;_tmp73.f1=(int)((unsigned int)Cyc_sizeof___fdmask);{
void*_tmp72[1]={& _tmp73};Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp72,sizeof(void*),1));}});({
void*_tmp74[0]={};Cyc_log(_tag_arr("Warning: sizeof(__fd_mask) inlined\n",
sizeof(unsigned char),36),_tag_arr(_tmp74,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 4: _LL113:({struct Cyc_Std_Int_pa_struct _tmp77;_tmp77.tag=Cyc_Std_Int_pa;
_tmp77.f1=(int)((unsigned int)Cyc_sizeof_uint32);{void*_tmp76[1]={& _tmp77};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp76,sizeof(void*),1));}});({void*_tmp78[0]={};Cyc_log(
_tag_arr("Warning: sizeof(__uint32_t) inlined\n",sizeof(unsigned char),37),
_tag_arr(_tmp78,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 5: _LL117:({
struct Cyc_Std_Int_pa_struct _tmp7B;_tmp7B.tag=Cyc_Std_Int_pa;_tmp7B.f1=(int)((
unsigned int)Cyc_sizeof_inport);{void*_tmp7A[1]={& _tmp7B};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp7A,sizeof(void*),1));}});({void*_tmp7C[0]={};Cyc_log(
_tag_arr("Warning: sizeof(__in_port_t) inlined\n",sizeof(unsigned char),38),
_tag_arr(_tmp7C,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 6: _LL121:({
struct Cyc_Std_Int_pa_struct _tmp7F;_tmp7F.tag=Cyc_Std_Int_pa;_tmp7F.f1=(int)((
unsigned int)Cyc_sizeof_int);{void*_tmp7E[1]={& _tmp7F};Cyc_Std_fprintf((struct
Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(unsigned char),
5),_tag_arr(_tmp7E,sizeof(void*),1));}});({void*_tmp80[0]={};Cyc_log(_tag_arr("Warning: sizeof(int) inlined\n",
sizeof(unsigned char),30),_tag_arr(_tmp80,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 7: _LL125:({struct Cyc_Std_Int_pa_struct _tmp83;_tmp83.tag=Cyc_Std_Int_pa;
_tmp83.f1=(int)((unsigned int)Cyc_sizeof_short);{void*_tmp82[1]={& _tmp83};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp82,sizeof(void*),1));}});({void*_tmp84[0]={};Cyc_log(
_tag_arr("Warning: sizeof(__short) inlined\n",sizeof(unsigned char),34),_tag_arr(
_tmp84,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 8: _LL129:({struct Cyc_Std_Int_pa_struct
_tmp87;_tmp87.tag=Cyc_Std_Int_pa;_tmp87.f1=(int)((unsigned int)Cyc_sizeof_unsignedlongint);{
void*_tmp86[1]={& _tmp87};Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp86,sizeof(void*),1));}});({
void*_tmp88[0]={};Cyc_log(_tag_arr("Warning: sizeof(unsigned long int) inlined\n",
sizeof(unsigned char),44),_tag_arr(_tmp88,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 9: _LL133:({struct Cyc_Std_Int_pa_struct _tmp8B;_tmp8B.tag=Cyc_Std_Int_pa;
_tmp8B.f1=(int)((unsigned int)Cyc_sizeof_unsignedshortint);{void*_tmp8A[1]={&
_tmp8B};Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp8A,sizeof(void*),1));}});({
void*_tmp8C[0]={};Cyc_log(_tag_arr("Warning: sizeof(unsigned short int) inlined\n",
sizeof(unsigned char),45),_tag_arr(_tmp8C,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 10: _LL137:({struct Cyc_Std_Int_pa_struct _tmp8F;_tmp8F.tag=Cyc_Std_Int_pa;
_tmp8F.f1=(int)((unsigned int)Cyc_sizeof_shortint);{void*_tmp8E[1]={& _tmp8F};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp8E,sizeof(void*),1));}});({void*_tmp90[0]={};Cyc_log(
_tag_arr("Warning: sizeof(short int) inlined\n",sizeof(unsigned char),36),
_tag_arr(_tmp90,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 11: _LL141:({
struct Cyc_Std_Int_pa_struct _tmp93;_tmp93.tag=Cyc_Std_Int_pa;_tmp93.f1=(int)((
unsigned int)Cyc_sizeof_sockaddr);{void*_tmp92[1]={& _tmp93};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp92,sizeof(void*),1));}});({void*_tmp94[0]={};Cyc_log(
_tag_arr("Warning: sizeof(struct sockaddr) inlined\n",sizeof(unsigned char),42),
_tag_arr(_tmp94,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 12: _LL145:({
struct Cyc_Std_Int_pa_struct _tmp97;_tmp97.tag=Cyc_Std_Int_pa;_tmp97.f1=(int)((
unsigned int)Cyc_sizeof_inaddr);{void*_tmp96[1]={& _tmp97};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp96,sizeof(void*),1));}});({void*_tmp98[0]={};Cyc_log(
_tag_arr("Warning: sizeof(struct inaddr) inlined\n",sizeof(unsigned char),40),
_tag_arr(_tmp98,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 13: _LL149: Cyc_Std_fputs(
_tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC ",sizeof(unsigned char),30),(struct Cyc_Std___sFILE*)
_check_null(Cyc_slurp_out));({void*_tmp9A[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp9A,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 14: _LL153: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC ",
sizeof(unsigned char),30),(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));({
void*_tmp9C[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp9C,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 15: _LL155: Cyc_Std_fputs(_tag_arr(" __IGNORE_FOR_CYCLONE_CALLOC ",
sizeof(unsigned char),30),(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));({
void*_tmp9E[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp9E,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 16: _LL157: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC ",
sizeof(unsigned char),30),(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));({
void*_tmpA0[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmpA0,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 17: _LL159: Cyc_Std_fputs(_tag_arr("__region",sizeof(unsigned char),9),(
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));({void*_tmpA2[0]={};Cyc_log(
_tag_arr("Warning: use of region sidestepped\n",sizeof(unsigned char),36),
_tag_arr(_tmpA2,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 18: _LL161:
return Cyc_slurp(lexbuf);case 19: _LL163: return Cyc_slurp(lexbuf);case 20: _LL164:
return Cyc_slurp(lexbuf);case 21: _LL165: return Cyc_slurp(lexbuf);case 22: _LL166:
return Cyc_slurp(lexbuf);case 23: _LL167: return Cyc_slurp(lexbuf);case 24: _LL168: Cyc_Std_fputs(
_tag_arr("inline",sizeof(unsigned char),7),(struct Cyc_Std___sFILE*)_check_null(
Cyc_slurp_out));return Cyc_slurp(lexbuf);case 25: _LL169: Cyc_Std_fputs(_tag_arr("inline",
sizeof(unsigned char),7),(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));
return Cyc_slurp(lexbuf);case 26: _LL170: Cyc_Std_fputs(_tag_arr("const",sizeof(
unsigned char),6),(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));return Cyc_slurp(
lexbuf);case 27: _LL171: Cyc_Std_fputs(_tag_arr("const",sizeof(unsigned char),6),(
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));return Cyc_slurp(lexbuf);case
28: _LL172: Cyc_Std_fputs(_tag_arr("int",sizeof(unsigned char),4),(struct Cyc_Std___sFILE*)
_check_null(Cyc_slurp_out));return Cyc_slurp(lexbuf);case 29: _LL173: return Cyc_slurp(
lexbuf);case 30: _LL174: Cyc_parens_to_match=1;if(Cyc_asm(lexbuf)){return 1;}Cyc_Std_fputs(
_tag_arr("0",sizeof(unsigned char),2),(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));({
void*_tmpB0[0]={};Cyc_log(_tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof(unsigned char),42),_tag_arr(_tmpB0,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 31: _LL175: Cyc_Std_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));return Cyc_slurp(lexbuf);
default: _LL177:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpB3=_cycalloc(sizeof(struct
Cyc_Lexing_Error_struct));_tmpB3[0]=({struct Cyc_Lexing_Error_struct _tmpB4;_tmpB4.tag=
Cyc_Lexing_Error;_tmpB4.f1=_tag_arr("some action didn't return!",sizeof(
unsigned char),27);_tmpB4;});_tmpB3;}));}int Cyc_slurp(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_rec(lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL181: Cyc_Std_fputc((int)'"',(struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));
return 0;case 1: _LL182: return 1;case 2: _LL183: return 1;case 3: _LL184:({struct Cyc_Std_String_pa_struct
_tmpBB;_tmpBB.tag=Cyc_Std_String_pa;_tmpBB.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);{void*_tmpBA[1]={& _tmpBB};Cyc_Std_fprintf((struct Cyc_Std___sFILE*)
_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmpBA,sizeof(void*),1));}});return Cyc_slurp_string(lexbuf);case 4: _LL185:({
struct Cyc_Std_String_pa_struct _tmpBE;_tmpBE.tag=Cyc_Std_String_pa;_tmpBE.f1=(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmpBD[1]={& _tmpBE};Cyc_Std_fprintf((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmpBD,sizeof(void*),1));}});return Cyc_slurp_string(
lexbuf);case 5: _LL188:({struct Cyc_Std_String_pa_struct _tmpC1;_tmpC1.tag=Cyc_Std_String_pa;
_tmpC1.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmpC0[1]={& _tmpC1};
Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpC0,sizeof(void*),1));}});return Cyc_slurp_string(
lexbuf);case 6: _LL191:({struct Cyc_Std_String_pa_struct _tmpC4;_tmpC4.tag=Cyc_Std_String_pa;
_tmpC4.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmpC3[1]={& _tmpC4};
Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpC3,sizeof(void*),1));}});return Cyc_slurp_string(
lexbuf);case 7: _LL194:({struct Cyc_Std_String_pa_struct _tmpC7;_tmpC7.tag=Cyc_Std_String_pa;
_tmpC7.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmpC6[1]={& _tmpC7};
Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpC6,sizeof(void*),1));}});return Cyc_slurp_string(
lexbuf);case 8: _LL197:({struct Cyc_Std_String_pa_struct _tmpCA;_tmpCA.tag=Cyc_Std_String_pa;
_tmpCA.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmpC9[1]={& _tmpCA};
Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpC9,sizeof(void*),1));}});return Cyc_slurp_string(
lexbuf);default: _LL200:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpCC=
_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmpCC[0]=({struct Cyc_Lexing_Error_struct
_tmpCD;_tmpCD.tag=Cyc_Lexing_Error;_tmpCD.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpCD;});_tmpCC;}));}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL206: if(Cyc_parens_to_match == 1){return 0;}Cyc_parens_to_match --;return
Cyc_asm(lexbuf);case 1: _LL207: Cyc_parens_to_match ++;return Cyc_asm(lexbuf);case 2:
_LL208: Cyc_asm_string(lexbuf);return Cyc_asm(lexbuf);case 3: _LL209: Cyc_asm_comment(
lexbuf);return Cyc_asm(lexbuf);case 4: _LL210: return Cyc_asm(lexbuf);case 5: _LL211:
return 0;case 6: _LL212: return Cyc_asm(lexbuf);default: _LL213:(lexbuf->refill_buff)(
lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpD7=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmpD7[0]=({struct Cyc_Lexing_Error_struct
_tmpD8;_tmpD8.tag=Cyc_Lexing_Error;_tmpD8.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpD8;});_tmpD7;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL217: return 0;case 1: _LL218: return 1;case 2: _LL219: return 1;case 3: _LL220:
return Cyc_asm_string(lexbuf);case 4: _LL221: return Cyc_asm_string(lexbuf);case 5:
_LL222: return Cyc_asm_string(lexbuf);case 6: _LL223: return Cyc_asm_string(lexbuf);
case 7: _LL224: return Cyc_asm_string(lexbuf);case 8: _LL225: return Cyc_asm_string(
lexbuf);default: _LL226:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpE4=
_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmpE4[0]=({struct Cyc_Lexing_Error_struct
_tmpE5;_tmpE5.tag=Cyc_Lexing_Error;_tmpE5.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpE5;});_tmpE4;}));}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL230: return 0;case 1: _LL231: return 1;case 2: _LL232: return Cyc_asm_comment(
lexbuf);default: _LL233:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpEB=
_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmpEB[0]=({struct Cyc_Lexing_Error_struct
_tmpEC;_tmpEC.tag=Cyc_Lexing_Error;_tmpEC.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpEC;});_tmpEB;}));}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple9*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL237: Cyc_current_line=_tag_arr("#define ",
sizeof(unsigned char),9);Cyc_suck_macroname(lexbuf);return({struct _tuple9*_tmpEF=
_cycalloc(sizeof(struct _tuple9));_tmpEF->f1=Cyc_current_line;_tmpEF->f2=(struct
_tagged_arr*)_check_null(Cyc_current_source);_tmpEF;});case 1: _LL238: return Cyc_suck_line(
lexbuf);case 2: _LL240: return 0;default: _LL241:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpF3=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmpF3[0]=({struct Cyc_Lexing_Error_struct
_tmpF4;_tmpF4.tag=Cyc_Lexing_Error;_tmpF4.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpF4;});_tmpF3;}));}struct _tuple9*Cyc_suck_line(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL245: Cyc_current_source=({struct _tagged_arr*
_tmpF7=_cycalloc(sizeof(struct _tagged_arr));_tmpF7[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmpF7;});Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,*((
struct _tagged_arr*)_check_null(Cyc_current_source)));return Cyc_suck_restofline(
lexbuf);default: _LL246:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpF9=
_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));_tmpF9[0]=({struct Cyc_Lexing_Error_struct
_tmpFA;_tmpFA.tag=Cyc_Lexing_Error;_tmpFA.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpFA;});_tmpF9;}));}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL251: Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(
Cyc_current_line,(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default:
_LL252:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpFE=_cycalloc(
sizeof(struct Cyc_Lexing_Error_struct));_tmpFE[0]=({struct Cyc_Lexing_Error_struct
_tmpFF;_tmpFF.tag=Cyc_Lexing_Error;_tmpFF.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmpFF;});_tmpFE;}));}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}struct _tuple11*Cyc_spec_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL256: Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=
0;if(Cyc_commands(lexbuf)){return 0;}Cyc_current_prologue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cstubs);Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_cycstubs);return({struct _tuple11*_tmp102=
_cycalloc(sizeof(struct _tuple11));_tmp102->f1=Cyc_current_header;_tmp102->f2=Cyc_current_symbols;
_tmp102->f3=Cyc_current_omit_symbols;_tmp102->f4=Cyc_current_prologue;_tmp102->f5=
Cyc_current_epilogue;_tmp102->f6=Cyc_current_cstubs;_tmp102->f7=Cyc_current_cycstubs;
_tmp102;});case 1: _LL257: return Cyc_spec(lexbuf);case 2: _LL259: return Cyc_spec(
lexbuf);case 3: _LL260: return 0;case 4: _LL261:({struct Cyc_Std_Int_pa_struct _tmp108;
_tmp108.tag=Cyc_Std_Int_pa;_tmp108.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp107[1]={& _tmp108};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof(unsigned char),67),_tag_arr(_tmp107,sizeof(void*),1));}});return 0;
default: _LL262:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp10A=_cycalloc(sizeof(
struct Cyc_Lexing_Error_struct));_tmp10A[0]=({struct Cyc_Lexing_Error_struct
_tmp10B;_tmp10B.tag=Cyc_Lexing_Error;_tmp10B.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp10B;});_tmp10A;}));}struct _tuple11*Cyc_spec(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}int Cyc_commands_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL268: Cyc_snarfed_symbols=0;if(Cyc_snarfsymbols(
lexbuf)){return 1;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return Cyc_commands(lexbuf);case 1: _LL269: Cyc_snarfed_symbols=0;if(Cyc_snarfsymbols(
lexbuf)){return 1;}Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return Cyc_commands(lexbuf);case 2: _LL270: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{
struct _tuple10*x=({struct _tuple10*_tmp111=_cycalloc(sizeof(struct _tuple10));
_tmp111->f1=(struct _tagged_arr)_tag_arr(0,0,0);_tmp111->f2=(struct _tagged_arr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp111;});
Cyc_current_prologue=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(struct Cyc_List_List));
_tmp110->hd=x;_tmp110->tl=Cyc_current_prologue;_tmp110;});return Cyc_commands(
lexbuf);}case 3: _LL271: {struct _tagged_arr _tmp113=Cyc_Lexing_lexeme(lexbuf);
_tagged_arr_inplace_plus(& _tmp113,sizeof(unsigned char),(int)Cyc_Std_strlen(
_tag_arr("prologue",sizeof(unsigned char),9)));while(isspace((int)*((
unsigned char*)_check_unknown_subscript(_tmp113,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp113,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp113;while(! isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp114=Cyc_Std_substring((struct _tagged_arr)_tmp113,0,(unsigned int)((t.curr - 
_tmp113.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple10*x=({struct _tuple10*_tmp116=_cycalloc(sizeof(struct _tuple10));_tmp116->f1=(
struct _tagged_arr)_tmp114;_tmp116->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp116;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp115=_cycalloc(sizeof(struct Cyc_List_List));_tmp115->hd=x;
_tmp115->tl=Cyc_current_prologue;_tmp115;});return Cyc_commands(lexbuf);}}}}case 4:
_LL274: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);if(Cyc_block(lexbuf)){return 1;}{struct _tuple10*x=({struct _tuple10*_tmp119=
_cycalloc(sizeof(struct _tuple10));_tmp119->f1=(struct _tagged_arr)_tag_arr(0,0,0);
_tmp119->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp119;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp118=_cycalloc(sizeof(struct Cyc_List_List));_tmp118->hd=x;_tmp118->tl=Cyc_current_epilogue;
_tmp118;});return Cyc_commands(lexbuf);}case 5: _LL279: {struct _tagged_arr _tmp11B=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp11B,sizeof(unsigned char),(
int)Cyc_Std_strlen(_tag_arr("epilogue",sizeof(unsigned char),9)));while(isspace((
int)*((unsigned char*)_check_unknown_subscript(_tmp11B,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp11B,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp11B;while(! isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp11C=Cyc_Std_substring((struct _tagged_arr)_tmp11B,0,(unsigned int)((t.curr - 
_tmp11B.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple10*x=({struct _tuple10*_tmp11E=_cycalloc(sizeof(struct _tuple10));_tmp11E->f1=(
struct _tagged_arr)_tmp11C;_tmp11E->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp11E;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(struct Cyc_List_List));_tmp11D->hd=x;
_tmp11D->tl=Cyc_current_epilogue;_tmp11D;});return Cyc_commands(lexbuf);}}}}case 6:
_LL282: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);if(Cyc_block(lexbuf)){return 1;}{struct _tuple10*x=({struct _tuple10*_tmp121=
_cycalloc(sizeof(struct _tuple10));_tmp121->f1=(struct _tagged_arr)_tag_arr(0,0,0);
_tmp121->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp121;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp120=_cycalloc(sizeof(struct Cyc_List_List));_tmp120->hd=x;_tmp120->tl=Cyc_current_cstubs;
_tmp120;});return Cyc_commands(lexbuf);}case 7: _LL287: {struct _tagged_arr _tmp123=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp123,sizeof(unsigned char),(
int)Cyc_Std_strlen(_tag_arr("cstub",sizeof(unsigned char),6)));while(isspace((
int)*((unsigned char*)_check_unknown_subscript(_tmp123,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp123,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp123;while(! isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp124=Cyc_Std_substring((struct _tagged_arr)_tmp123,0,(unsigned int)((t.curr - 
_tmp123.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple10*x=({struct _tuple10*_tmp126=_cycalloc(sizeof(struct _tuple10));_tmp126->f1=(
struct _tagged_arr)_tmp124;_tmp126->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp126;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp125=_cycalloc(sizeof(struct Cyc_List_List));_tmp125->hd=x;
_tmp125->tl=Cyc_current_cstubs;_tmp125;});return Cyc_commands(lexbuf);}}}}case 8:
_LL290: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);if(Cyc_block(lexbuf)){return 1;}{struct _tuple10*x=({struct _tuple10*_tmp129=
_cycalloc(sizeof(struct _tuple10));_tmp129->f1=(struct _tagged_arr)_tag_arr(0,0,0);
_tmp129->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp129;});Cyc_current_cycstubs=({struct Cyc_List_List*
_tmp128=_cycalloc(sizeof(struct Cyc_List_List));_tmp128->hd=x;_tmp128->tl=Cyc_current_cycstubs;
_tmp128;});return Cyc_commands(lexbuf);}case 9: _LL295: {struct _tagged_arr _tmp12B=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp12B,sizeof(unsigned char),(
int)Cyc_Std_strlen(_tag_arr("cycstub",sizeof(unsigned char),8)));while(isspace((
int)*((unsigned char*)_check_unknown_subscript(_tmp12B,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp12B,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp12B;while(! isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp12C=Cyc_Std_substring((struct _tagged_arr)_tmp12B,0,(unsigned int)((t.curr - 
_tmp12B.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple10*x=({struct _tuple10*_tmp12E=_cycalloc(sizeof(struct _tuple10));_tmp12E->f1=(
struct _tagged_arr)_tmp12C;_tmp12E->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp12E;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(struct Cyc_List_List));_tmp12D->hd=x;
_tmp12D->tl=Cyc_current_cycstubs;_tmp12D;});return Cyc_commands(lexbuf);}}}}case
10: _LL298: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);if(Cyc_block(lexbuf)){return 1;}{struct _tagged_arr*x=({struct _tagged_arr*
_tmp131=_cycalloc(sizeof(struct _tagged_arr));_tmp131[0]=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp131;});Cyc_current_cpp=({
struct Cyc_List_List*_tmp130=_cycalloc(sizeof(struct Cyc_List_List));_tmp130->hd=x;
_tmp130->tl=Cyc_current_cpp;_tmp130;});return Cyc_commands(lexbuf);}case 11: _LL303:
return Cyc_commands(lexbuf);case 12: _LL306: return Cyc_commands(lexbuf);case 13:
_LL307: return 0;case 14: _LL308: return 0;case 15: _LL309:({struct Cyc_Std_Int_pa_struct
_tmp138;_tmp138.tag=Cyc_Std_Int_pa;_tmp138.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp137[1]={& _tmp138};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof(unsigned char),58),_tag_arr(_tmp137,sizeof(void*),1));}});return 1;
default: _LL310:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp13A=_cycalloc(
sizeof(struct Cyc_Lexing_Error_struct));_tmp13A[0]=({struct Cyc_Lexing_Error_struct
_tmp13B;_tmp13B.tag=Cyc_Lexing_Error;_tmp13B.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp13B;});_tmp13A;}));}int Cyc_commands(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL316: Cyc_snarfed_symbols=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(
struct Cyc_List_List));_tmp13E->hd=({struct _tagged_arr*_tmp13F=_cycalloc(sizeof(
struct _tagged_arr));_tmp13F[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);
_tmp13F;});_tmp13E->tl=Cyc_snarfed_symbols;_tmp13E;});return Cyc_snarfsymbols(
lexbuf);case 1: _LL317: return Cyc_snarfsymbols(lexbuf);case 2: _LL320: return 0;case 3:
_LL321:({void*_tmp143[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: unexpected end-of-file\n",
sizeof(unsigned char),44),_tag_arr(_tmp143,sizeof(void*),0));});return 1;case 4:
_LL322:({struct Cyc_Std_Int_pa_struct _tmp146;_tmp146.tag=Cyc_Std_Int_pa;_tmp146.f1=(
int)((unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp145[1]={&
_tmp146};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof(unsigned char),57),_tag_arr(_tmp145,sizeof(void*),1));}});return 1;
default: _LL324:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp148=_cycalloc(
sizeof(struct Cyc_Lexing_Error_struct));_tmp148[0]=({struct Cyc_Lexing_Error_struct
_tmp149;_tmp149.tag=Cyc_Lexing_Error;_tmp149.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp149;});_tmp148;}));}int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL330: if(Cyc_braces_to_match == 1){return 0;}Cyc_braces_to_match --;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return Cyc_block(lexbuf);case 1:
_LL331: Cyc_braces_to_match ++;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'{');return Cyc_block(lexbuf);case 2: _LL332: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');Cyc_block_string(lexbuf);
return Cyc_block(lexbuf);case 3: _LL333: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),_tag_arr("/*",sizeof(unsigned char),3));Cyc_block_comment(
lexbuf);return Cyc_block(lexbuf);case 4: _LL334: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return Cyc_block(
lexbuf);case 5: _LL335: return 0;case 6: _LL336: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));return Cyc_block(
lexbuf);default: _LL337:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp153=_cycalloc(
sizeof(struct Cyc_Lexing_Error_struct));_tmp153[0]=({struct Cyc_Lexing_Error_struct
_tmp154;_tmp154.tag=Cyc_Lexing_Error;_tmp154.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp154;});_tmp153;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL341: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 1: _LL342: return 1;case 2: _LL343: return 1;case 3: _LL344: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 4: _LL345: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 5: _LL346: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 6: _LL347: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 7: _LL348: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 8: _LL349: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);default: _LL350:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp160=_cycalloc(sizeof(struct Cyc_Lexing_Error_struct));
_tmp160[0]=({struct Cyc_Lexing_Error_struct _tmp161;_tmp161.tag=Cyc_Lexing_Error;
_tmp161.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);
_tmp161;});_tmp160;}));}int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_block_string_rec(lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL354: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
_tag_arr("*/",sizeof(unsigned char),3));return 0;case 1: _LL355: return 1;case 2:
_LL356: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return Cyc_block_comment(lexbuf);
default: _LL357:(lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp167=_cycalloc(
sizeof(struct Cyc_Lexing_Error_struct));_tmp167[0]=({struct Cyc_Lexing_Error_struct
_tmp168;_tmp168.tag=Cyc_Lexing_Error;_tmp168.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp168;});_tmp167;}));}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_comment_rec(lexbuf,18);}static void Cyc_pr(struct
_tagged_arr*sptr){({struct Cyc_Std_String_pa_struct _tmp16A;_tmp16A.tag=Cyc_Std_String_pa;
_tmp16A.f1=(struct _tagged_arr)*sptr;{void*_tmp169[1]={& _tmp16A};Cyc_Std_printf(
_tag_arr(" %s\n",sizeof(unsigned char),5),_tag_arr(_tmp169,sizeof(void*),1));}});}
extern void Cyc_scan_type(void*t);struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmp16B=(void*)((struct Cyc_Absyn_Exp*)
_check_null(e))->r;struct _tuple0*_tmp1B9;struct _tuple0 _tmp1BB;struct _tagged_arr*
_tmp1BC;struct _tuple0*_tmp1BE;struct _tuple0 _tmp1C0;struct _tagged_arr*_tmp1C1;
struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Absyn_Exp*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*
_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*
_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*
_tmp1D9;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_List_List*
_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*
_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E7;void*_tmp1E9;struct Cyc_Absyn_MallocInfo
_tmp1EB;struct Cyc_Absyn_Exp*_tmp1ED;void**_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;
int _tmp1F3;void*_tmp1F5;struct _tagged_arr*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F9;
struct _tagged_arr*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FD;void*_tmp1FF;void*_tmp201;
struct Cyc_List_List*_tmp203;_LL365: if(*((int*)_tmp16B)== Cyc_Absyn_Var_e){_LL442:
_tmp1B9=((struct Cyc_Absyn_Var_e_struct*)_tmp16B)->f1;_tmp1BB=*_tmp1B9;_LL445:
_tmp1BC=_tmp1BB.f2;goto _LL366;}else{goto _LL367;}_LL367: if(*((int*)_tmp16B)== Cyc_Absyn_UnknownId_e){
_LL447: _tmp1BE=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp16B)->f1;_tmp1C0=*
_tmp1BE;_LL450: _tmp1C1=_tmp1C0.f2;goto _LL368;}else{goto _LL369;}_LL369: if(*((int*)
_tmp16B)== Cyc_Absyn_Primop_e){_LL452: _tmp1C3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp16B)->f2;goto _LL370;}else{goto _LL371;}_LL371: if(*((int*)_tmp16B)== Cyc_Absyn_Subscript_e){
_LL456: _tmp1C7=((struct Cyc_Absyn_Subscript_e_struct*)_tmp16B)->f1;goto _LL454;
_LL454: _tmp1C5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp16B)->f2;goto _LL372;}
else{goto _LL373;}_LL373: if(*((int*)_tmp16B)== Cyc_Absyn_SeqExp_e){_LL460: _tmp1CB=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp16B)->f1;goto _LL458;_LL458: _tmp1C9=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp16B)->f2;goto _LL374;}else{goto _LL375;}_LL375: if(*((
int*)_tmp16B)== Cyc_Absyn_AssignOp_e){_LL464: _tmp1CF=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp16B)->f1;goto _LL462;_LL462: _tmp1CD=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp16B)->f3;goto _LL376;}else{goto _LL377;}_LL377: if(*((int*)_tmp16B)== Cyc_Absyn_Deref_e){
_LL466: _tmp1D1=((struct Cyc_Absyn_Deref_e_struct*)_tmp16B)->f1;goto _LL378;}else{
goto _LL379;}_LL379: if(*((int*)_tmp16B)== Cyc_Absyn_Sizeofexp_e){_LL468: _tmp1D3=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp16B)->f1;goto _LL380;}else{goto _LL381;}
_LL381: if(*((int*)_tmp16B)== Cyc_Absyn_Address_e){_LL470: _tmp1D5=((struct Cyc_Absyn_Address_e_struct*)
_tmp16B)->f1;goto _LL382;}else{goto _LL383;}_LL383: if(*((int*)_tmp16B)== Cyc_Absyn_Increment_e){
_LL472: _tmp1D7=((struct Cyc_Absyn_Increment_e_struct*)_tmp16B)->f1;goto _LL384;}
else{goto _LL385;}_LL385: if(*((int*)_tmp16B)== Cyc_Absyn_Conditional_e){_LL478:
_tmp1DD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp16B)->f1;goto _LL476;_LL476:
_tmp1DB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp16B)->f2;goto _LL474;_LL474:
_tmp1D9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp16B)->f3;goto _LL386;}else{
goto _LL387;}_LL387: if(*((int*)_tmp16B)== Cyc_Absyn_FnCall_e){_LL482: _tmp1E1=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp16B)->f1;goto _LL480;_LL480: _tmp1DF=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp16B)->f2;goto _LL388;}else{goto _LL389;}_LL389: if(*((
int*)_tmp16B)== Cyc_Absyn_UnknownCall_e){_LL486: _tmp1E5=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp16B)->f1;goto _LL484;_LL484: _tmp1E3=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp16B)->f2;goto _LL390;}else{goto _LL391;}_LL391: if(*((int*)_tmp16B)== Cyc_Absyn_Cast_e){
_LL490: _tmp1E9=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp16B)->f1;goto _LL488;
_LL488: _tmp1E7=((struct Cyc_Absyn_Cast_e_struct*)_tmp16B)->f2;goto _LL392;}else{
goto _LL393;}_LL393: if(*((int*)_tmp16B)== Cyc_Absyn_Malloc_e){_LL492: _tmp1EB=((
struct Cyc_Absyn_Malloc_e_struct*)_tmp16B)->f1;_LL500: _tmp1F3=_tmp1EB.is_calloc;
goto _LL498;_LL498: _tmp1F1=_tmp1EB.rgn;goto _LL496;_LL496: _tmp1EF=_tmp1EB.elt_type;
goto _LL494;_LL494: _tmp1ED=_tmp1EB.num_elts;goto _LL394;}else{goto _LL395;}_LL395:
if(*((int*)_tmp16B)== Cyc_Absyn_Sizeoftyp_e){_LL502: _tmp1F5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp16B)->f1;goto _LL396;}else{goto _LL397;}_LL397: if(*((int*)_tmp16B)== Cyc_Absyn_StructMember_e){
_LL506: _tmp1F9=((struct Cyc_Absyn_StructMember_e_struct*)_tmp16B)->f1;goto _LL504;
_LL504: _tmp1F7=((struct Cyc_Absyn_StructMember_e_struct*)_tmp16B)->f2;goto _LL398;}
else{goto _LL399;}_LL399: if(*((int*)_tmp16B)== Cyc_Absyn_StructArrow_e){_LL510:
_tmp1FD=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp16B)->f1;goto _LL508;_LL508:
_tmp1FB=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp16B)->f2;goto _LL400;}else{
goto _LL401;}_LL401: if(*((int*)_tmp16B)== Cyc_Absyn_Offsetof_e){_LL514: _tmp201=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp16B)->f1;goto _LL512;_LL512:
_tmp1FF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp16B)->f2;goto _LL402;}
else{goto _LL403;}_LL403: if(*((int*)_tmp16B)== Cyc_Absyn_Const_e){goto _LL404;}
else{goto _LL405;}_LL405: if(*((int*)_tmp16B)== Cyc_Absyn_UnresolvedMem_e){_LL516:
_tmp203=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp16B)->f2;goto _LL406;}else{
goto _LL407;}_LL407: if(*((int*)_tmp16B)== Cyc_Absyn_StmtExp_e){goto _LL408;}else{
goto _LL409;}_LL409: if(*((int*)_tmp16B)== Cyc_Absyn_Throw_e){goto _LL410;}else{
goto _LL411;}_LL411: if(*((int*)_tmp16B)== Cyc_Absyn_NoInstantiate_e){goto _LL412;}
else{goto _LL413;}_LL413: if(*((int*)_tmp16B)== Cyc_Absyn_Instantiate_e){goto
_LL414;}else{goto _LL415;}_LL415: if(*((int*)_tmp16B)== Cyc_Absyn_New_e){goto
_LL416;}else{goto _LL417;}_LL417: if(*((int*)_tmp16B)== Cyc_Absyn_Gentyp_e){goto
_LL418;}else{goto _LL419;}_LL419: if(*((int*)_tmp16B)== Cyc_Absyn_Tuple_e){goto
_LL420;}else{goto _LL421;}_LL421: if(*((int*)_tmp16B)== Cyc_Absyn_CompoundLit_e){
goto _LL422;}else{goto _LL423;}_LL423: if(*((int*)_tmp16B)== Cyc_Absyn_Array_e){
goto _LL424;}else{goto _LL425;}_LL425: if(*((int*)_tmp16B)== Cyc_Absyn_Comprehension_e){
goto _LL426;}else{goto _LL427;}_LL427: if(*((int*)_tmp16B)== Cyc_Absyn_Struct_e){
goto _LL428;}else{goto _LL429;}_LL429: if(*((int*)_tmp16B)== Cyc_Absyn_AnonStruct_e){
goto _LL430;}else{goto _LL431;}_LL431: if(*((int*)_tmp16B)== Cyc_Absyn_Tunion_e){
goto _LL432;}else{goto _LL433;}_LL433: if(*((int*)_tmp16B)== Cyc_Absyn_Enum_e){goto
_LL434;}else{goto _LL435;}_LL435: if(*((int*)_tmp16B)== Cyc_Absyn_AnonEnum_e){goto
_LL436;}else{goto _LL437;}_LL437: if(*((int*)_tmp16B)== Cyc_Absyn_Codegen_e){goto
_LL438;}else{goto _LL439;}_LL439: if(*((int*)_tmp16B)== Cyc_Absyn_Fill_e){goto
_LL440;}else{goto _LL364;}_LL366: _tmp1C1=_tmp1BC;goto _LL368;_LL368: Cyc_add_target(
_tmp1C1);return;_LL370: for(0;_tmp1C3 != 0;_tmp1C3=_tmp1C3->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmp1C3->hd));}return;_LL372: _tmp1CB=
_tmp1C7;_tmp1C9=_tmp1C5;goto _LL374;_LL374: _tmp1CF=_tmp1CB;_tmp1CD=_tmp1C9;goto
_LL376;_LL376: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1CF);Cyc_scan_exp((struct
Cyc_Absyn_Exp*)_tmp1CD);return;_LL378: _tmp1D3=_tmp1D1;goto _LL380;_LL380: _tmp1D5=
_tmp1D3;goto _LL382;_LL382: _tmp1D7=_tmp1D5;goto _LL384;_LL384: Cyc_scan_exp((struct
Cyc_Absyn_Exp*)_tmp1D7);return;_LL386: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1DD);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1DB);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmp1D9);return;_LL388: _tmp1E5=_tmp1E1;_tmp1E3=_tmp1DF;goto _LL390;_LL390: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmp1E5);for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmp1E3->hd));}return;_LL392: Cyc_scan_type(
_tmp1E9);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1E7);return;_LL394: if(_tmp1F1 != 
0){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_check_null(_tmp1F1)));}
if(_tmp1EF != 0){Cyc_scan_type(*_tmp1EF);}Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmp1ED);return;_LL396: Cyc_scan_type(_tmp1F5);return;_LL398: _tmp1FD=_tmp1F9;
_tmp1FB=_tmp1F7;goto _LL400;_LL400: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1FD);
Cyc_add_target(_tmp1FB);return;_LL402: Cyc_scan_type(_tmp201);{void*_tmp205=
_tmp1FF;struct _tagged_arr*_tmp20B;_LL519: if(*((int*)_tmp205)== Cyc_Absyn_StructField){
_LL524: _tmp20B=((struct Cyc_Absyn_StructField_struct*)_tmp205)->f1;goto _LL520;}
else{goto _LL521;}_LL521: if(*((int*)_tmp205)== Cyc_Absyn_TupleIndex){goto _LL522;}
else{goto _LL518;}_LL520: Cyc_add_target(_tmp20B);goto _LL518;_LL522: goto _LL518;
_LL518:;}return;_LL404: return;_LL406: for(0;_tmp203 != 0;_tmp203=_tmp203->tl){
struct _tuple12 _tmp20F;struct Cyc_Absyn_Exp*_tmp210;struct _tuple12*_tmp20D=(struct
_tuple12*)_tmp203->hd;_tmp20F=*_tmp20D;_LL529: _tmp210=_tmp20F.f2;goto _LL526;
_LL526: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp210);}return;_LL408:({void*_tmp212[
0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Stmt_e\n",
sizeof(unsigned char),26),_tag_arr(_tmp212,sizeof(void*),0));});exit(1);return;
_LL410:({void*_tmp213[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Throw_e\n",
sizeof(unsigned char),27),_tag_arr(_tmp213,sizeof(void*),0));});exit(1);return;
_LL412:({void*_tmp214[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected NoInstantiate_e\n",
sizeof(unsigned char),35),_tag_arr(_tmp214,sizeof(void*),0));});exit(1);return;
_LL414:({void*_tmp215[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Instantiate_e\n",
sizeof(unsigned char),33),_tag_arr(_tmp215,sizeof(void*),0));});exit(1);return;
_LL416:({void*_tmp216[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected New_e\n",
sizeof(unsigned char),25),_tag_arr(_tmp216,sizeof(void*),0));});exit(1);return;
_LL418:({void*_tmp217[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Gentyp_e\n",
sizeof(unsigned char),28),_tag_arr(_tmp217,sizeof(void*),0));});exit(1);return;
_LL420:({void*_tmp218[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Tuple_e\n",
sizeof(unsigned char),27),_tag_arr(_tmp218,sizeof(void*),0));});exit(1);return;
_LL422:({void*_tmp219[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected CompoundLit_e\n",
sizeof(unsigned char),33),_tag_arr(_tmp219,sizeof(void*),0));});exit(1);return;
_LL424:({void*_tmp21A[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Array_e\n",
sizeof(unsigned char),27),_tag_arr(_tmp21A,sizeof(void*),0));});exit(1);return;
_LL426:({void*_tmp21B[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Comprehension_e\n",
sizeof(unsigned char),35),_tag_arr(_tmp21B,sizeof(void*),0));});exit(1);return;
_LL428:({void*_tmp21C[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Struct_e\n",
sizeof(unsigned char),28),_tag_arr(_tmp21C,sizeof(void*),0));});exit(1);return;
_LL430:({void*_tmp21D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonStruct_e\n",
sizeof(unsigned char),32),_tag_arr(_tmp21D,sizeof(void*),0));});exit(1);return;
_LL432:({void*_tmp21E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Tunion_e\n",
sizeof(unsigned char),28),_tag_arr(_tmp21E,sizeof(void*),0));});exit(1);return;
_LL434:({void*_tmp21F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Enum_e\n",
sizeof(unsigned char),26),_tag_arr(_tmp21F,sizeof(void*),0));});exit(1);return;
_LL436:({void*_tmp220[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonEnum_e\n",
sizeof(unsigned char),30),_tag_arr(_tmp220,sizeof(void*),0));});exit(1);return;
_LL438:({void*_tmp221[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Codegen_e\n",
sizeof(unsigned char),29),_tag_arr(_tmp221,sizeof(void*),0));});exit(1);return;
_LL440:({void*_tmp222[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Fill_e\n",
sizeof(unsigned char),26),_tag_arr(_tmp222,sizeof(void*),0));});exit(1);return;
_LL364:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((unsigned int)eo){Cyc_scan_exp(
eo);}return;}void Cyc_scan_type(void*t){void*_tmp223=t;struct Cyc_Absyn_PtrInfo
_tmp25B;struct Cyc_Absyn_Exp*_tmp25D;void*_tmp25F;struct Cyc_Absyn_FnInfo _tmp261;
struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp265;struct _tuple0*_tmp267;
struct _tuple0*_tmp269;struct _tuple0*_tmp26B;struct _tuple0 _tmp26D;struct
_tagged_arr*_tmp26E;struct _tuple0*_tmp270;struct _tuple0 _tmp272;struct _tagged_arr*
_tmp273;struct _tuple0*_tmp275;struct _tuple0 _tmp277;struct _tagged_arr*_tmp278;
struct _tuple0*_tmp27A;struct _tuple0 _tmp27C;struct _tagged_arr*_tmp27D;_LL549: if(
_tmp223 == (void*)Cyc_Absyn_VoidType){goto _LL550;}else{goto _LL551;}_LL551: if((
unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_IntType: 0){goto _LL552;}
else{goto _LL553;}_LL553: if(_tmp223 == (void*)Cyc_Absyn_FloatType){goto _LL554;}
else{goto _LL555;}_LL555: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_DoubleType:
0){goto _LL556;}else{goto _LL557;}_LL557: if((unsigned int)_tmp223 > 3?*((int*)
_tmp223)== Cyc_Absyn_PointerType: 0){_LL604: _tmp25B=((struct Cyc_Absyn_PointerType_struct*)
_tmp223)->f1;goto _LL558;}else{goto _LL559;}_LL559: if((unsigned int)_tmp223 > 3?*((
int*)_tmp223)== Cyc_Absyn_ArrayType: 0){_LL608: _tmp25F=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp223)->f1;goto _LL606;_LL606: _tmp25D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp223)->f3;goto _LL560;}else{goto _LL561;}_LL561: if((unsigned int)_tmp223 > 3?*((
int*)_tmp223)== Cyc_Absyn_FnType: 0){_LL610: _tmp261=((struct Cyc_Absyn_FnType_struct*)
_tmp223)->f1;goto _LL562;}else{goto _LL563;}_LL563: if((unsigned int)_tmp223 > 3?*((
int*)_tmp223)== Cyc_Absyn_AnonStructType: 0){_LL612: _tmp263=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp223)->f1;goto _LL564;}else{goto _LL565;}_LL565: if((unsigned int)_tmp223 > 3?*((
int*)_tmp223)== Cyc_Absyn_AnonUnionType: 0){_LL614: _tmp265=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp223)->f1;goto _LL566;}else{goto _LL567;}_LL567: if((unsigned int)_tmp223 > 3?*((
int*)_tmp223)== Cyc_Absyn_AnonEnumType: 0){goto _LL568;}else{goto _LL569;}_LL569:
if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_StructType: 0){_LL616:
_tmp267=((struct Cyc_Absyn_StructType_struct*)_tmp223)->f1;if(_tmp267 == 0){goto
_LL570;}else{goto _LL571;}}else{goto _LL571;}_LL571: if((unsigned int)_tmp223 > 3?*((
int*)_tmp223)== Cyc_Absyn_UnionType: 0){_LL618: _tmp269=((struct Cyc_Absyn_UnionType_struct*)
_tmp223)->f1;if(_tmp269 == 0){goto _LL572;}else{goto _LL573;}}else{goto _LL573;}
_LL573: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_StructType: 0){
_LL620: _tmp26B=((struct Cyc_Absyn_StructType_struct*)_tmp223)->f1;if(_tmp26B == 0){
goto _LL575;}else{_tmp26D=*_tmp26B;_LL623: _tmp26E=_tmp26D.f2;goto _LL574;}}else{
goto _LL575;}_LL575: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_UnionType:
0){_LL625: _tmp270=((struct Cyc_Absyn_UnionType_struct*)_tmp223)->f1;if(_tmp270 == 
0){goto _LL577;}else{_tmp272=*_tmp270;_LL628: _tmp273=_tmp272.f2;goto _LL576;}}
else{goto _LL577;}_LL577: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_EnumType:
0){_LL630: _tmp275=((struct Cyc_Absyn_EnumType_struct*)_tmp223)->f1;_tmp277=*
_tmp275;_LL633: _tmp278=_tmp277.f2;goto _LL578;}else{goto _LL579;}_LL579: if((
unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_TypedefType: 0){_LL635:
_tmp27A=((struct Cyc_Absyn_TypedefType_struct*)_tmp223)->f1;_tmp27C=*_tmp27A;
_LL638: _tmp27D=_tmp27C.f2;goto _LL580;}else{goto _LL581;}_LL581: if((unsigned int)
_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_Evar: 0){goto _LL582;}else{goto _LL583;}
_LL583: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_VarType: 0){goto
_LL584;}else{goto _LL585;}_LL585: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== 
Cyc_Absyn_TunionType: 0){goto _LL586;}else{goto _LL587;}_LL587: if((unsigned int)
_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_TunionFieldType: 0){goto _LL588;}else{goto
_LL589;}_LL589: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_TupleType:
0){goto _LL590;}else{goto _LL591;}_LL591: if((unsigned int)_tmp223 > 3?*((int*)
_tmp223)== Cyc_Absyn_RgnHandleType: 0){goto _LL592;}else{goto _LL593;}_LL593: if(
_tmp223 == (void*)Cyc_Absyn_HeapRgn){goto _LL594;}else{goto _LL595;}_LL595: if((
unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_AccessEff: 0){goto _LL596;}
else{goto _LL597;}_LL597: if((unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_JoinEff:
0){goto _LL598;}else{goto _LL599;}_LL599: if((unsigned int)_tmp223 > 3?*((int*)
_tmp223)== Cyc_Absyn_RgnsEff: 0){goto _LL600;}else{goto _LL601;}_LL601: if((
unsigned int)_tmp223 > 3?*((int*)_tmp223)== Cyc_Absyn_SizeofType: 0){goto _LL602;}
else{goto _LL548;}_LL550: goto _LL552;_LL552: goto _LL554;_LL554: goto _LL556;_LL556:
return;_LL558: Cyc_scan_type((void*)_tmp25B.elt_typ);return;_LL560: Cyc_scan_type(
_tmp25F);Cyc_scan_exp_opt(_tmp25D);return;_LL562: Cyc_scan_type((void*)_tmp261.ret_typ);{
struct Cyc_List_List*_tmp27F=_tmp261.args;for(0;_tmp27F != 0;_tmp27F=_tmp27F->tl){
struct _tuple1 _tmp282;void*_tmp283;struct _tuple1*_tmp280=(struct _tuple1*)_tmp27F->hd;
_tmp282=*_tmp280;_LL644: _tmp283=_tmp282.f3;goto _LL641;_LL641: Cyc_scan_type(
_tmp283);}}if(_tmp261.cyc_varargs != 0){Cyc_scan_type((void*)(_tmp261.cyc_varargs)->type);}
return;_LL564: _tmp265=_tmp263;goto _LL566;_LL566: for(0;_tmp265 != 0;_tmp265=
_tmp265->tl){Cyc_scan_type((void*)((struct Cyc_Absyn_Structfield*)_tmp265->hd)->type);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Structfield*)_tmp265->hd)->width);}return;
_LL568: return;_LL570: return;_LL572: return;_LL574: _tmp273=_tmp26E;goto _LL576;
_LL576: _tmp278=_tmp273;goto _LL578;_LL578: _tmp27D=_tmp278;goto _LL580;_LL580: Cyc_add_target(
_tmp27D);return;_LL582:({void*_tmp285[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Evar\n",sizeof(unsigned char),24),_tag_arr(_tmp285,
sizeof(void*),0));});exit(1);return;_LL584:({void*_tmp286[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected VarType\n",sizeof(unsigned char),27),
_tag_arr(_tmp286,sizeof(void*),0));});exit(1);return;_LL586:({void*_tmp287[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected TunionType\n",sizeof(
unsigned char),30),_tag_arr(_tmp287,sizeof(void*),0));});exit(1);return;_LL588:({
void*_tmp288[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected TunionFieldType\n",
sizeof(unsigned char),35),_tag_arr(_tmp288,sizeof(void*),0));});exit(1);return;
_LL590:({void*_tmp289[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected TupleType\n",
sizeof(unsigned char),29),_tag_arr(_tmp289,sizeof(void*),0));});exit(1);return;
_LL592:({void*_tmp28A[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected RgnHandleType\n",
sizeof(unsigned char),33),_tag_arr(_tmp28A,sizeof(void*),0));});exit(1);return;
_LL594:({void*_tmp28B[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected HeapRgn\n",
sizeof(unsigned char),27),_tag_arr(_tmp28B,sizeof(void*),0));});exit(1);return;
_LL596:({void*_tmp28C[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AccessEff\n",
sizeof(unsigned char),29),_tag_arr(_tmp28C,sizeof(void*),0));});exit(1);return;
_LL598:({void*_tmp28D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected JoinEff\n",
sizeof(unsigned char),27),_tag_arr(_tmp28D,sizeof(void*),0));});exit(1);return;
_LL600:({void*_tmp28E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected RgnsEff\n",
sizeof(unsigned char),27),_tag_arr(_tmp28E,sizeof(void*),0));});exit(1);return;
_LL602:({void*_tmp28F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected sizeof_t\n",
sizeof(unsigned char),28),_tag_arr(_tmp28F,sizeof(void*),0));});exit(1);return;
_LL548:;}struct _tuple13{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct _tuple8*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct
Cyc_Set_Set**_tmp290=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp290[0]=((struct
Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);_tmp290;});{void*_tmp291=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp2AB;struct Cyc_Absyn_Fndecl*_tmp2AD;struct Cyc_Absyn_Structdecl*_tmp2AF;struct
Cyc_Absyn_Uniondecl*_tmp2B1;struct Cyc_Absyn_Enumdecl*_tmp2B3;struct Cyc_Absyn_Typedefdecl*
_tmp2B5;_LL659: if(*((int*)_tmp291)== Cyc_Absyn_Var_d){_LL684: _tmp2AB=((struct Cyc_Absyn_Var_d_struct*)
_tmp291)->f1;goto _LL660;}else{goto _LL661;}_LL661: if(*((int*)_tmp291)== Cyc_Absyn_Fn_d){
_LL686: _tmp2AD=((struct Cyc_Absyn_Fn_d_struct*)_tmp291)->f1;goto _LL662;}else{goto
_LL663;}_LL663: if(*((int*)_tmp291)== Cyc_Absyn_Struct_d){_LL688: _tmp2AF=((struct
Cyc_Absyn_Struct_d_struct*)_tmp291)->f1;goto _LL664;}else{goto _LL665;}_LL665: if(*((
int*)_tmp291)== Cyc_Absyn_Union_d){_LL690: _tmp2B1=((struct Cyc_Absyn_Union_d_struct*)
_tmp291)->f1;goto _LL666;}else{goto _LL667;}_LL667: if(*((int*)_tmp291)== Cyc_Absyn_Enum_d){
_LL692: _tmp2B3=((struct Cyc_Absyn_Enum_d_struct*)_tmp291)->f1;goto _LL668;}else{
goto _LL669;}_LL669: if(*((int*)_tmp291)== Cyc_Absyn_Typedef_d){_LL694: _tmp2B5=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp291)->f1;goto _LL670;}else{goto _LL671;}
_LL671: if(*((int*)_tmp291)== Cyc_Absyn_Let_d){goto _LL672;}else{goto _LL673;}
_LL673: if(*((int*)_tmp291)== Cyc_Absyn_Tunion_d){goto _LL674;}else{goto _LL675;}
_LL675: if(*((int*)_tmp291)== Cyc_Absyn_Letv_d){goto _LL676;}else{goto _LL677;}
_LL677: if(*((int*)_tmp291)== Cyc_Absyn_Namespace_d){goto _LL678;}else{goto _LL679;}
_LL679: if(*((int*)_tmp291)== Cyc_Absyn_Using_d){goto _LL680;}else{goto _LL681;}
_LL681: if(*((int*)_tmp291)== Cyc_Absyn_ExternC_d){goto _LL682;}else{goto _LL658;}
_LL660: {struct _tuple0 _tmp2B9;struct _tagged_arr*_tmp2BA;struct _tuple0*_tmp2B7=
_tmp2AB->name;_tmp2B9=*_tmp2B7;_LL699: _tmp2BA=_tmp2B9.f2;goto _LL696;_LL696: Cyc_current_source=(
struct _tagged_arr*)_tmp2BA;Cyc_scan_type((void*)_tmp2AB->type);Cyc_scan_exp_opt(
_tmp2AB->initializer);goto _LL658;}_LL662: {struct _tuple0 _tmp2BE;struct
_tagged_arr*_tmp2BF;struct _tuple0*_tmp2BC=_tmp2AD->name;_tmp2BE=*_tmp2BC;_LL704:
_tmp2BF=_tmp2BE.f2;goto _LL701;_LL701: Cyc_current_source=(struct _tagged_arr*)
_tmp2BF;Cyc_scan_type((void*)_tmp2AD->ret_type);{struct Cyc_List_List*_tmp2C1=
_tmp2AD->args;for(0;_tmp2C1 != 0;_tmp2C1=_tmp2C1->tl){struct _tuple13 _tmp2C4;void*
_tmp2C5;struct _tuple13*_tmp2C2=(struct _tuple13*)_tmp2C1->hd;_tmp2C4=*_tmp2C2;
_LL710: _tmp2C5=_tmp2C4.f3;goto _LL707;_LL707: Cyc_scan_type(_tmp2C5);}}if(_tmp2AD->cyc_varargs
!= 0){Cyc_scan_type((void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp2AD->cyc_varargs))->type);}
if(_tmp2AD->is_inline){({void*_tmp2C7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Warning: ignoring inline function\n",sizeof(unsigned char),35),
_tag_arr(_tmp2C7,sizeof(void*),0));});}goto _LL658;}_LL664: if(!((unsigned int)
_tmp2AF->name)){return 0;}{struct _tuple0 _tmp2CA;struct _tagged_arr*_tmp2CB;struct
_tuple0*_tmp2C8=(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp2AF->name))->v;
_tmp2CA=*_tmp2C8;_LL716: _tmp2CB=_tmp2CA.f2;goto _LL713;_LL713: Cyc_current_source=(
struct _tagged_arr*)_tmp2CB;if((unsigned int)_tmp2AF->fields){{struct Cyc_List_List*
_tmp2CD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2AF->fields))->v;
for(0;_tmp2CD != 0;_tmp2CD=_tmp2CD->tl){struct Cyc_Absyn_Structfield*_tmp2CE=(
struct Cyc_Absyn_Structfield*)_tmp2CD->hd;Cyc_scan_type((void*)_tmp2CE->type);Cyc_scan_exp_opt(
_tmp2CE->width);}}{struct Cyc_List_List*_tmp2CF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2AF->fields))->v;for(0;_tmp2CF != 0;_tmp2CF=_tmp2CF->tl){;}}}goto
_LL658;}_LL666: if(!((unsigned int)_tmp2B1->name)){return 0;}{struct _tuple0 _tmp2D2;
struct _tagged_arr*_tmp2D3;struct _tuple0*_tmp2D0=(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B1->name))->v;_tmp2D2=*_tmp2D0;_LL724: _tmp2D3=_tmp2D2.f2;goto
_LL721;_LL721: Cyc_current_source=(struct _tagged_arr*)_tmp2D3;if((unsigned int)
_tmp2B1->fields){{struct Cyc_List_List*_tmp2D5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B1->fields))->v;for(0;_tmp2D5 != 0;_tmp2D5=_tmp2D5->tl){struct
Cyc_Absyn_Structfield*_tmp2D6=(struct Cyc_Absyn_Structfield*)_tmp2D5->hd;Cyc_add_target(
_tmp2D6->name);Cyc_scan_type((void*)_tmp2D6->type);Cyc_scan_exp_opt(_tmp2D6->width);}}{
struct Cyc_List_List*_tmp2D7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B1->fields))->v;for(0;_tmp2D7 != 0;_tmp2D7=_tmp2D7->tl){;}}}goto
_LL658;}_LL668: {struct _tuple0 _tmp2DA;struct _tagged_arr*_tmp2DB;struct _tuple0*
_tmp2D8=_tmp2B3->name;_tmp2DA=*_tmp2D8;_LL732: _tmp2DB=_tmp2DA.f2;goto _LL729;
_LL729: Cyc_current_source=(struct _tagged_arr*)_tmp2DB;if((unsigned int)_tmp2B3->fields){{
struct Cyc_List_List*_tmp2DD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B3->fields))->v;for(0;_tmp2DD != 0;_tmp2DD=_tmp2DD->tl){struct
Cyc_Absyn_Enumfield*_tmp2DE=(struct Cyc_Absyn_Enumfield*)_tmp2DD->hd;Cyc_scan_exp_opt(
_tmp2DE->tag);}}{struct Cyc_List_List*_tmp2DF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B3->fields))->v;for(0;_tmp2DF != 0;_tmp2DF=_tmp2DF->tl){;}}}goto
_LL658;}_LL670: {struct _tuple0 _tmp2E2;struct _tagged_arr*_tmp2E3;struct _tuple0*
_tmp2E0=_tmp2B5->name;_tmp2E2=*_tmp2E0;_LL740: _tmp2E3=_tmp2E2.f2;goto _LL737;
_LL737: Cyc_current_source=(struct _tagged_arr*)_tmp2E3;Cyc_scan_type((void*)
_tmp2B5->defn);goto _LL658;}_LL672:({void*_tmp2E5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(unsigned char),35),
_tag_arr(_tmp2E5,sizeof(void*),0));});exit(1);return 0;_LL674:({void*_tmp2E6[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected tunion declaration\n",
sizeof(unsigned char),38),_tag_arr(_tmp2E6,sizeof(void*),0));});exit(1);return 0;
_LL676:({void*_tmp2E7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected let declaration\n",
sizeof(unsigned char),35),_tag_arr(_tmp2E7,sizeof(void*),0));});exit(1);return 0;
_LL678:({void*_tmp2E8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected namespace declaration\n",
sizeof(unsigned char),41),_tag_arr(_tmp2E8,sizeof(void*),0));});exit(1);return 0;
_LL680:({void*_tmp2E9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected using declaration\n",
sizeof(unsigned char),37),_tag_arr(_tmp2E9,sizeof(void*),0));});exit(1);return 0;
_LL682:({void*_tmp2EA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected extern \"C\" declaration\n",
sizeof(unsigned char),42),_tag_arr(_tmp2EA,sizeof(void*),0));});exit(1);return 0;
_LL658:;}return({struct _tuple8*_tmp2EB=_cycalloc(sizeof(struct _tuple8));_tmp2EB->f1=(
struct _tagged_arr*)_check_null(Cyc_current_source);_tmp2EB->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets));_tmp2EB;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,
Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*x){struct _handler_cons _tmp2EC;_push_handler(& _tmp2EC);{int
_tmp2EE=0;if(setjmp(_tmp2EC.handler)){_tmp2EE=1;}if(! _tmp2EE){{struct Cyc_Set_Set*
_tmp2EF=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*
key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp2EF;};_pop_handler();}
else{void*_tmp2ED=(void*)_exn_thrown;void*_tmp2F1=_tmp2ED;_LL755: if(_tmp2F1 == 
Cyc_Core_Not_found){goto _LL756;}else{goto _LL757;}_LL757: goto _LL758;_LL756: return((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);_LL758:(void)_throw(_tmp2F1);_LL754:;}}}struct Cyc_Set_Set*Cyc_reachable(
struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
curr,(struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp2F7=curr;while(((int(*)(
struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2F7)> 0){struct Cyc_Set_Set*_tmp2F8=
emptyset;{struct Cyc_List_List*_tmp2F9=((struct Cyc_List_List*(*)(struct Cyc_Set_Set*
s))Cyc_Set_elements)(_tmp2F7);for(0;_tmp2F9 != 0;_tmp2F9=_tmp2F9->tl){_tmp2F8=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp2F8,Cyc_find(t,(struct _tagged_arr*)_tmp2F9->hd));}}_tmp2F7=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp2F8,curr);curr=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
curr,_tmp2F7);}return curr;}}int Cyc_force_directory(struct _tagged_arr d){int
_tmp2FA=({int _tmp2FD[0];Cyc_Std_open(d,0,_tag_arr(_tmp2FD,sizeof(int),0));});if(
_tmp2FA == - 1){if(Cyc_Std_mkdir(d,448)== - 1){({struct Cyc_Std_String_pa_struct
_tmp2FC;_tmp2FC.tag=Cyc_Std_String_pa;_tmp2FC.f1=(struct _tagged_arr)d;{void*
_tmp2FB[1]={& _tmp2FC};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create directory %s\n",
sizeof(unsigned char),38),_tag_arr(_tmp2FB,sizeof(void*),1));}});return 1;}}else{
close(_tmp2FA);}return 0;}int Cyc_force_directory_prefixes(struct _tagged_arr file){
struct _tagged_arr _tmp2FE=Cyc_Std_strdup(file);struct Cyc_List_List*_tmp2FF=0;
while(1){_tmp2FE=Cyc_Filename_dirname((struct _tagged_arr)_tmp2FE);if(
_get_arr_size(_tmp2FE,sizeof(unsigned char))== 0){break;}_tmp2FF=({struct Cyc_List_List*
_tmp300=_cycalloc(sizeof(struct Cyc_List_List));_tmp300->hd=({struct _tagged_arr*
_tmp301=_cycalloc(sizeof(struct _tagged_arr));_tmp301[0]=(struct _tagged_arr)
_tmp2FE;_tmp301;});_tmp300->tl=_tmp2FF;_tmp300;});}for(0;_tmp2FF != 0;_tmp2FF=
_tmp2FF->tl){if(Cyc_force_directory(*((struct _tagged_arr*)_tmp2FF->hd))){return 1;}}
return 0;}int Cyc_process_file(struct _tagged_arr filename,struct _tagged_arr dir,
struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*
prologue,struct Cyc_List_List*epilogue,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs){struct Cyc_Std___sFILE*maybe;struct Cyc_Std___sFILE*in_file;struct Cyc_Std___sFILE*
out_file;({struct Cyc_Std_String_pa_struct _tmp303;_tmp303.tag=Cyc_Std_String_pa;
_tmp303.f1=(struct _tagged_arr)filename;{void*_tmp302[1]={& _tmp303};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("********************************* %s...\n",sizeof(
unsigned char),41),_tag_arr(_tmp302,sizeof(void*),1));}});{struct _tagged_arr buf=
_tag_arr(({unsigned int _tmp43B=(unsigned int)1024;unsigned char*_tmp43C=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp43B));{
unsigned int _tmp43D=_tmp43B;unsigned int i;for(i=0;i < _tmp43D;i ++){_tmp43C[i]='\000';}};
_tmp43C;}),sizeof(unsigned char),(unsigned int)1024);struct _tagged_arr _tmp304=
Cyc_Std_getcwd(buf,_get_arr_size(buf,sizeof(unsigned char)));if(Cyc_Std_chdir(
dir)){({struct Cyc_Std_String_pa_struct _tmp306;_tmp306.tag=Cyc_Std_String_pa;
_tmp306.f1=(struct _tagged_arr)dir;{void*_tmp305[1]={& _tmp306};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: can't change directory to %s\n",sizeof(
unsigned char),37),_tag_arr(_tmp305,sizeof(void*),1));}});return 1;}({struct Cyc_Std_String_pa_struct
_tmp308;_tmp308.tag=Cyc_Std_String_pa;_tmp308.f1=(struct _tagged_arr)filename;{
void*_tmp307[1]={& _tmp308};Cyc_log(_tag_arr("\n%s:\n",sizeof(unsigned char),6),
_tag_arr(_tmp307,sizeof(void*),1));}});{struct _tagged_arr _tmp309=Cyc_Filename_basename(
filename);struct _tagged_arr _tmp30A=Cyc_Filename_chop_extension((struct
_tagged_arr)_tmp309);struct _tagged_arr _tmp30B=Cyc_Std_strconcat((struct
_tagged_arr)_tmp30A,_tag_arr(".iI",sizeof(unsigned char),4));struct _tagged_arr
_tmp30C=Cyc_Std_strconcat((struct _tagged_arr)_tmp30A,_tag_arr(".i",sizeof(
unsigned char),3));struct _tagged_arr _tmp30D=Cyc_Std_strconcat((struct _tagged_arr)
_tmp30A,_tag_arr(".iX",sizeof(unsigned char),4));struct _tagged_arr _tmp30E=Cyc_Filename_dirname(
filename);struct _tagged_arr _tmp30F=_get_arr_size(_tmp30E,sizeof(unsigned char))
== 0?({struct Cyc_Std_String_pa_struct _tmp438;_tmp438.tag=Cyc_Std_String_pa;
_tmp438.f1=(struct _tagged_arr)_tmp309;{void*_tmp437[1]={& _tmp438};Cyc_Std_aprintf(
_tag_arr("c%s",sizeof(unsigned char),4),_tag_arr(_tmp437,sizeof(void*),1));}}):
Cyc_Filename_concat((struct _tagged_arr)_tmp30E,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp43A;_tmp43A.tag=Cyc_Std_String_pa;_tmp43A.f1=(struct _tagged_arr)_tmp309;{
void*_tmp439[1]={& _tmp43A};Cyc_Std_aprintf(_tag_arr("c%s",sizeof(unsigned char),
4),_tag_arr(_tmp439,sizeof(void*),1));}}));if(Cyc_force_directory_prefixes(
filename)){return 1;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();struct
_tagged_arr _tmp310=Cstring_to_string(Ccomp);maybe=Cyc_Std_fopen((struct
_tagged_arr)_tmp30B,_tag_arr("w",sizeof(unsigned char),2));if(!((unsigned int)
maybe)){({struct Cyc_Std_String_pa_struct _tmp312;_tmp312.tag=Cyc_Std_String_pa;
_tmp312.f1=(struct _tagged_arr)_tmp30B;{void*_tmp311[1]={& _tmp312};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),
33),_tag_arr(_tmp311,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___sFILE*)
_check_null(maybe);Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_cpp);{struct Cyc_List_List*_tmp313=Cyc_current_cpp;
for(0;_tmp313 != 0;_tmp313=_tmp313->tl){Cyc_Std_fputs(*((struct _tagged_arr*)
_tmp313->hd),out_file);}}({struct Cyc_Std_String_pa_struct _tmp315;_tmp315.tag=Cyc_Std_String_pa;
_tmp315.f1=(struct _tagged_arr)filename;{void*_tmp314[1]={& _tmp315};Cyc_Std_fprintf(
out_file,_tag_arr("#include <%s>\n",sizeof(unsigned char),15),_tag_arr(_tmp314,
sizeof(void*),1));}});Cyc_Std_fclose(out_file);{struct _tagged_arr _tmp316=({
struct Cyc_Std_String_pa_struct _tmp436;_tmp436.tag=Cyc_Std_String_pa;_tmp436.f1=(
struct _tagged_arr)_tmp30B;{struct Cyc_Std_String_pa_struct _tmp435;_tmp435.tag=Cyc_Std_String_pa;
_tmp435.f1=(struct _tagged_arr)_tmp30C;{struct Cyc_Std_String_pa_struct _tmp434;
_tmp434.tag=Cyc_Std_String_pa;_tmp434.f1=(struct _tagged_arr)_tmp310;{void*
_tmp433[3]={& _tmp434,& _tmp435,& _tmp436};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s %s",
sizeof(unsigned char),19),_tag_arr(_tmp433,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp316)!= 0){Cyc_Std_remove((struct _tagged_arr)_tmp30B);maybe=
Cyc_Std_fopen(filename,_tag_arr("w",sizeof(unsigned char),2));if(!((unsigned int)
maybe)){({struct Cyc_Std_String_pa_struct _tmp318;_tmp318.tag=Cyc_Std_String_pa;
_tmp318.f1=(struct _tagged_arr)filename;{void*_tmp317[1]={& _tmp318};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),
33),_tag_arr(_tmp317,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___sFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp31A;_tmp31A.tag=Cyc_Std_String_pa;
_tmp31A.f1=(struct _tagged_arr)filename;{void*_tmp319[1]={& _tmp31A};Cyc_Std_fprintf(
out_file,_tag_arr("#error -- %s is not supported on this platform\n",sizeof(
unsigned char),48),_tag_arr(_tmp319,sizeof(void*),1));}});Cyc_Std_fclose(
out_file);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp30F,_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp31C;_tmp31C.tag=Cyc_Std_String_pa;_tmp31C.f1=(struct _tagged_arr)_tmp30F;{
void*_tmp31B[1]={& _tmp31C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(unsigned char),33),_tag_arr(_tmp31B,sizeof(void*),1));}});return 1;}
out_file=(struct Cyc_Std___sFILE*)_check_null(maybe);({struct Cyc_Std_String_pa_struct
_tmp31E;_tmp31E.tag=Cyc_Std_String_pa;_tmp31E.f1=(struct _tagged_arr)_tmp30F;{
void*_tmp31D[1]={& _tmp31E};Cyc_Std_fprintf(out_file,_tag_arr("#error -- %s is not supported on this platform\n",
sizeof(unsigned char),48),_tag_arr(_tmp31D,sizeof(void*),1));}});Cyc_Std_fclose(
out_file);({struct Cyc_Std_String_pa_struct _tmp320;_tmp320.tag=Cyc_Std_String_pa;
_tmp320.f1=(struct _tagged_arr)filename;{void*_tmp31F[1]={& _tmp320};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Warning: %s will not be supported on this platform\n",
sizeof(unsigned char),52),_tag_arr(_tmp31F,sizeof(void*),1));}});({void*_tmp321[
0]={};Cyc_log(_tag_arr("Not supported on this platform\n",sizeof(unsigned char),
32),_tag_arr(_tmp321,sizeof(void*),0));});if(Cyc_Std_chdir((struct _tagged_arr)
_tmp304)){({struct Cyc_Std_String_pa_struct _tmp323;_tmp323.tag=Cyc_Std_String_pa;
_tmp323.f1=(struct _tagged_arr)_tmp304;{void*_tmp322[1]={& _tmp323};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not change directory to %s\n",sizeof(
unsigned char),41),_tag_arr(_tmp322,sizeof(void*),1));}});return 1;}return 0;}
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp30C,_tag_arr("r",sizeof(unsigned char),
2));if(!((unsigned int)maybe)){return 1;}in_file=(struct Cyc_Std___sFILE*)
_check_null(maybe);{struct Cyc_Lexing_lexbuf*_tmp324=Cyc_Lexing_from_file(in_file);
struct _tuple8*entry;while((entry=((struct _tuple8*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_line)(_tmp324))!= 0){struct _tuple8 _tmp327;struct Cyc_Set_Set*_tmp328;
struct _tagged_arr*_tmp32A;struct _tuple8*_tmp325=(struct _tuple8*)_check_null(
entry);_tmp327=*_tmp325;_LL811: _tmp32A=_tmp327.f1;goto _LL809;_LL809: _tmp328=
_tmp327.f2;goto _LL806;_LL806:((void(*)(struct Cyc_Hashtable_Table*t,struct
_tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp32A,_tmp328);}
Cyc_Std_fclose(in_file);_tmp316=({struct Cyc_Std_String_pa_struct _tmp32F;_tmp32F.tag=
Cyc_Std_String_pa;_tmp32F.f1=(struct _tagged_arr)_tmp30B;{struct Cyc_Std_String_pa_struct
_tmp32E;_tmp32E.tag=Cyc_Std_String_pa;_tmp32E.f1=(struct _tagged_arr)_tmp30C;{
struct Cyc_Std_String_pa_struct _tmp32D;_tmp32D.tag=Cyc_Std_String_pa;_tmp32D.f1=(
struct _tagged_arr)_tmp310;{void*_tmp32C[3]={& _tmp32D,& _tmp32E,& _tmp32F};Cyc_Std_aprintf(
_tag_arr("%s -E -o %s %s",sizeof(unsigned char),15),_tag_arr(_tmp32C,sizeof(void*),
3));}}}});if(Cyc_Std_system((struct _tagged_arr)_tmp316)!= 0){return 1;}maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp30C,_tag_arr("r",sizeof(unsigned char),2));if(!((
unsigned int)maybe)){return 1;}in_file=(struct Cyc_Std___sFILE*)_check_null(maybe);
_tmp324=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=Cyc_Std_fopen((struct
_tagged_arr)_tmp30D,_tag_arr("w",sizeof(unsigned char),2));if(!((unsigned int)
Cyc_slurp_out)){return 1;}if(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(
_tmp324)){({struct Cyc_Std_String_pa_struct _tmp331;_tmp331.tag=Cyc_Std_String_pa;
_tmp331.f1=(struct _tagged_arr)filename;{void*_tmp330[1]={& _tmp331};Cyc_log(
_tag_arr("Error: Unclosed string in %s\n",sizeof(unsigned char),30),_tag_arr(
_tmp330,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp333;_tmp333.tag=
Cyc_Std_String_pa;_tmp333.f1=(struct _tagged_arr)filename;{void*_tmp332[1]={&
_tmp333};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: Unclosed string in %s\n",
sizeof(unsigned char),30),_tag_arr(_tmp332,sizeof(void*),1));}});Cyc_Std_fclose(
in_file);Cyc_Std_fclose((struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));Cyc_Std_remove((
struct _tagged_arr)_tmp30B);Cyc_Std_remove((struct _tagged_arr)_tmp30C);Cyc_Std_remove((
struct _tagged_arr)_tmp30D);return 1;}Cyc_Std_fclose(in_file);Cyc_Std_fclose((
struct Cyc_Std___sFILE*)_check_null(Cyc_slurp_out));maybe=Cyc_Std_fopen((struct
_tagged_arr)_tmp30D,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)
maybe)){return 1;}in_file=(struct Cyc_Std___sFILE*)_check_null(maybe);Cyc_Position_reset_position((
struct _tagged_arr)_tmp30D);Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp334=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(in_file);Cyc_Std_remove((struct
_tagged_arr)_tmp30D);{struct Cyc_List_List*_tmp335=_tmp334;for(0;_tmp335 != 0;
_tmp335=_tmp335->tl){struct _tuple8*_tmp336=Cyc_scan_decl((struct Cyc_Absyn_Decl*)
_tmp335->hd);if(_tmp336 == 0){continue;}{struct _tuple8 _tmp339;struct Cyc_Set_Set*
_tmp33A;struct _tagged_arr*_tmp33C;struct _tuple8*_tmp337=(struct _tuple8*)
_check_null(_tmp336);_tmp339=*_tmp337;_LL829: _tmp33C=_tmp339.f1;goto _LL827;
_LL827: _tmp33A=_tmp339.f2;goto _LL824;_LL824: {struct Cyc_Set_Set*old;{struct
_handler_cons _tmp33E;_push_handler(& _tmp33E);{int _tmp340=0;if(setjmp(_tmp33E.handler)){
_tmp340=1;}if(! _tmp340){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,
struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,_tmp33C);;_pop_handler();}else{
void*_tmp33F=(void*)_exn_thrown;void*_tmp342=_tmp33F;_LL836: if(_tmp342 == Cyc_Core_Not_found){
goto _LL837;}else{goto _LL838;}_LL838: goto _LL839;_LL837: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL835;_LL839:(void)_throw(_tmp342);_LL835:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp33C,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp33A,old));}}}}{struct Cyc_Set_Set*_tmp348=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp349=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp34A=_tmp334;for(0;_tmp34A != 0;_tmp34A=_tmp34A->tl){
struct Cyc_Absyn_Decl*_tmp34B=(struct Cyc_Absyn_Decl*)_tmp34A->hd;struct
_tagged_arr*name;{void*_tmp34C=(void*)_tmp34B->r;struct Cyc_Absyn_Vardecl*_tmp366;
struct Cyc_Absyn_Fndecl*_tmp368;struct Cyc_Absyn_Structdecl*_tmp36A;struct Cyc_Absyn_Uniondecl*
_tmp36C;struct Cyc_Absyn_Enumdecl*_tmp36E;struct Cyc_Absyn_Typedefdecl*_tmp370;
_LL846: if(*((int*)_tmp34C)== Cyc_Absyn_Var_d){_LL871: _tmp366=((struct Cyc_Absyn_Var_d_struct*)
_tmp34C)->f1;goto _LL847;}else{goto _LL848;}_LL848: if(*((int*)_tmp34C)== Cyc_Absyn_Fn_d){
_LL873: _tmp368=((struct Cyc_Absyn_Fn_d_struct*)_tmp34C)->f1;goto _LL849;}else{goto
_LL850;}_LL850: if(*((int*)_tmp34C)== Cyc_Absyn_Struct_d){_LL875: _tmp36A=((struct
Cyc_Absyn_Struct_d_struct*)_tmp34C)->f1;goto _LL851;}else{goto _LL852;}_LL852: if(*((
int*)_tmp34C)== Cyc_Absyn_Union_d){_LL877: _tmp36C=((struct Cyc_Absyn_Union_d_struct*)
_tmp34C)->f1;goto _LL853;}else{goto _LL854;}_LL854: if(*((int*)_tmp34C)== Cyc_Absyn_Enum_d){
_LL879: _tmp36E=((struct Cyc_Absyn_Enum_d_struct*)_tmp34C)->f1;goto _LL855;}else{
goto _LL856;}_LL856: if(*((int*)_tmp34C)== Cyc_Absyn_Typedef_d){_LL881: _tmp370=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp34C)->f1;goto _LL857;}else{goto _LL858;}
_LL858: if(*((int*)_tmp34C)== Cyc_Absyn_Let_d){goto _LL859;}else{goto _LL860;}
_LL860: if(*((int*)_tmp34C)== Cyc_Absyn_Tunion_d){goto _LL861;}else{goto _LL862;}
_LL862: if(*((int*)_tmp34C)== Cyc_Absyn_Letv_d){goto _LL863;}else{goto _LL864;}
_LL864: if(*((int*)_tmp34C)== Cyc_Absyn_Namespace_d){goto _LL865;}else{goto _LL866;}
_LL866: if(*((int*)_tmp34C)== Cyc_Absyn_Using_d){goto _LL867;}else{goto _LL868;}
_LL868: if(*((int*)_tmp34C)== Cyc_Absyn_ExternC_d){goto _LL869;}else{goto _LL845;}
_LL847: {struct _tuple0 _tmp374;struct _tagged_arr*_tmp375;struct _tuple0*_tmp372=
_tmp366->name;_tmp374=*_tmp372;_LL886: _tmp375=_tmp374.f2;goto _LL883;_LL883:
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp375);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp375)){name=0;}else{name=(struct _tagged_arr*)
_tmp375;}goto _LL845;}_LL849: {struct _tuple0 _tmp379;struct _tagged_arr*_tmp37A;
struct _tuple0*_tmp377=_tmp368->name;_tmp379=*_tmp377;_LL891: _tmp37A=_tmp379.f2;
goto _LL888;_LL888: defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,_tmp37A);if(((int(*)(int(*
compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct
_tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,omit_symbols,_tmp37A)){name=0;}
else{name=(struct _tagged_arr*)_tmp37A;}goto _LL845;}_LL851: if(!((unsigned int)
_tmp36A->name)){name=0;}{struct _tuple0 _tmp37E;struct _tagged_arr*_tmp37F;struct
_tuple0*_tmp37C=(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp36A->name))->v;
_tmp37E=*_tmp37C;_LL896: _tmp37F=_tmp37E.f2;goto _LL893;_LL893: name=(struct
_tagged_arr*)_tmp37F;goto _LL845;}_LL853: if(!((unsigned int)_tmp36C->name)){name=
0;}{struct _tuple0 _tmp383;struct _tagged_arr*_tmp384;struct _tuple0*_tmp381=(struct
_tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp36C->name))->v;_tmp383=*_tmp381;
_LL901: _tmp384=_tmp383.f2;goto _LL898;_LL898: name=(struct _tagged_arr*)_tmp384;
goto _LL845;}_LL855: {struct _tuple0 _tmp388;struct _tagged_arr*_tmp389;struct
_tuple0*_tmp386=_tmp36E->name;_tmp388=*_tmp386;_LL906: _tmp389=_tmp388.f2;goto
_LL903;_LL903: name=(struct _tagged_arr*)_tmp389;if(name != 0?((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp348,(struct _tagged_arr*)_check_null(
name)): 0){_tmp349=({struct Cyc_List_List*_tmp38B=_cycalloc(sizeof(struct Cyc_List_List));
_tmp38B->hd=_tmp34B;_tmp38B->tl=_tmp349;_tmp38B;});}else{if((unsigned int)
_tmp36E->fields){struct Cyc_List_List*_tmp38C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp36E->fields))->v;for(0;_tmp38C != 0;_tmp38C=_tmp38C->tl){struct
Cyc_Absyn_Enumfield*_tmp38D=(struct Cyc_Absyn_Enumfield*)_tmp38C->hd;struct
_tuple0 _tmp390;struct _tagged_arr*_tmp391;struct _tuple0*_tmp38E=_tmp38D->name;
_tmp390=*_tmp38E;_LL914: _tmp391=_tmp390.f2;goto _LL911;_LL911: if(((int(*)(struct
Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp348,_tmp391)){_tmp349=({
struct Cyc_List_List*_tmp393=_cycalloc(sizeof(struct Cyc_List_List));_tmp393->hd=
_tmp34B;_tmp393->tl=_tmp349;_tmp393;});break;}}}}name=0;goto _LL845;}_LL857: {
struct _tuple0 _tmp396;struct _tagged_arr*_tmp397;struct _tuple0*_tmp394=_tmp370->name;
_tmp396=*_tmp394;_LL920: _tmp397=_tmp396.f2;goto _LL917;_LL917: name=(struct
_tagged_arr*)_tmp397;goto _LL845;}_LL859: goto _LL861;_LL861: goto _LL863;_LL863: goto
_LL865;_LL865: goto _LL867;_LL867: goto _LL869;_LL869: name=0;goto _LL845;_LL845:;}if(
name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp348,(struct _tagged_arr*)_check_null(name)): 0){_tmp349=({struct Cyc_List_List*
_tmp399=_cycalloc(sizeof(struct Cyc_List_List));_tmp399->hd=_tmp34B;_tmp399->tl=
_tmp349;_tmp399;});}}}_tmp349=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp349);maybe=Cyc_Std_fopen(filename,_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)maybe)){return 1;}out_file=(struct Cyc_Std___sFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp39B;_tmp39B.tag=Cyc_Std_String_pa;
_tmp39B.f1=(struct _tagged_arr)_tmp30F;{void*_tmp39A[1]={& _tmp39B};Cyc_Std_fprintf(
out_file,_tag_arr("#include <%s>\nusing Std;\n",sizeof(unsigned char),26),
_tag_arr(_tmp39A,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp30F,_tag_arr("w",sizeof(unsigned char),2));if(!((
unsigned int)maybe)){return 1;}out_file=(struct Cyc_Std___sFILE*)_check_null(maybe);{
struct _tagged_arr ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp432;_tmp432.tag=
Cyc_Std_String_pa;_tmp432.f1=(struct _tagged_arr)filename;{void*_tmp431[1]={&
_tmp432};Cyc_Std_aprintf(_tag_arr("_%s_",sizeof(unsigned char),5),_tag_arr(
_tmp431,sizeof(void*),1));}});{int _tmp39C=0;for(0;_tmp39C < _get_arr_size(
ifdefmacro,sizeof(unsigned char));_tmp39C ++){if(((unsigned char*)ifdefmacro.curr)[
_tmp39C]== '.'? 1:((unsigned char*)ifdefmacro.curr)[_tmp39C]== '/'){((
unsigned char*)ifdefmacro.curr)[_tmp39C]='_';}else{if(((unsigned char*)
ifdefmacro.curr)[_tmp39C]!= '_'?((unsigned char*)ifdefmacro.curr)[_tmp39C]!= '/':
0){((unsigned char*)ifdefmacro.curr)[_tmp39C]=(unsigned char)toupper((int)((
unsigned char*)ifdefmacro.curr)[_tmp39C]);}}}}({struct Cyc_Std_String_pa_struct
_tmp39F;_tmp39F.tag=Cyc_Std_String_pa;_tmp39F.f1=(struct _tagged_arr)ifdefmacro;{
struct Cyc_Std_String_pa_struct _tmp39E;_tmp39E.tag=Cyc_Std_String_pa;_tmp39E.f1=(
struct _tagged_arr)ifdefmacro;{void*_tmp39D[2]={& _tmp39E,& _tmp39F};Cyc_Std_fprintf(
out_file,_tag_arr("#ifndef %s\n#define %s\n",sizeof(unsigned char),23),_tag_arr(
_tmp39D,sizeof(void*),2));}}});if(prologue != 0){struct Cyc_List_List*_tmp3A0=
prologue;for(0;_tmp3A0 != 0;_tmp3A0=_tmp3A0->tl){struct _tuple10 _tmp3A3;struct
_tagged_arr _tmp3A4;struct _tagged_arr _tmp3A6;struct _tuple10*_tmp3A1=(struct
_tuple10*)_tmp3A0->hd;_tmp3A3=*_tmp3A1;_LL935: _tmp3A6=_tmp3A3.f1;goto _LL933;
_LL933: _tmp3A4=_tmp3A3.f2;goto _LL930;_LL930: if(_tmp3A4.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr? _tmp3A6.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp3A8=_cycalloc(sizeof(struct _tagged_arr));
_tmp3A8[0]=_tmp3A6;_tmp3A8;})): 0){Cyc_Std_fputs(_tmp3A4,out_file);}else{({struct
Cyc_Std_String_pa_struct _tmp3AA;_tmp3AA.tag=Cyc_Std_String_pa;_tmp3AA.f1=(struct
_tagged_arr)_tmp3A6;{void*_tmp3A9[1]={& _tmp3AA};Cyc_log(_tag_arr("%s is not supported on this platform\n",
sizeof(unsigned char),38),_tag_arr(_tmp3A9,sizeof(void*),1));}});}}}({void*
_tmp3AB[0]={};Cyc_Std_fprintf(out_file,_tag_arr("\nnamespace Std {\n",sizeof(
unsigned char),18),_tag_arr(_tmp3AB,sizeof(void*),0));});{struct Cyc_List_List*
_tmp3AC=_tmp349;for(0;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){struct Cyc_Absyn_Decl*
_tmp3AD=(struct Cyc_Absyn_Decl*)_tmp3AC->hd;int _tmp3AE=0;struct _tagged_arr*name;{
void*_tmp3AF=(void*)_tmp3AD->r;struct Cyc_Absyn_Vardecl*_tmp3C9;struct Cyc_Absyn_Fndecl*
_tmp3CB;struct Cyc_Absyn_Structdecl*_tmp3CD;struct Cyc_Absyn_Uniondecl*_tmp3CF;
struct Cyc_Absyn_Enumdecl*_tmp3D1;struct Cyc_Absyn_Typedefdecl*_tmp3D3;_LL945: if(*((
int*)_tmp3AF)== Cyc_Absyn_Var_d){_LL970: _tmp3C9=((struct Cyc_Absyn_Var_d_struct*)
_tmp3AF)->f1;goto _LL946;}else{goto _LL947;}_LL947: if(*((int*)_tmp3AF)== Cyc_Absyn_Fn_d){
_LL972: _tmp3CB=((struct Cyc_Absyn_Fn_d_struct*)_tmp3AF)->f1;goto _LL948;}else{goto
_LL949;}_LL949: if(*((int*)_tmp3AF)== Cyc_Absyn_Struct_d){_LL974: _tmp3CD=((struct
Cyc_Absyn_Struct_d_struct*)_tmp3AF)->f1;goto _LL950;}else{goto _LL951;}_LL951: if(*((
int*)_tmp3AF)== Cyc_Absyn_Union_d){_LL976: _tmp3CF=((struct Cyc_Absyn_Union_d_struct*)
_tmp3AF)->f1;goto _LL952;}else{goto _LL953;}_LL953: if(*((int*)_tmp3AF)== Cyc_Absyn_Enum_d){
_LL978: _tmp3D1=((struct Cyc_Absyn_Enum_d_struct*)_tmp3AF)->f1;goto _LL954;}else{
goto _LL955;}_LL955: if(*((int*)_tmp3AF)== Cyc_Absyn_Typedef_d){_LL980: _tmp3D3=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp3AF)->f1;goto _LL956;}else{goto _LL957;}
_LL957: if(*((int*)_tmp3AF)== Cyc_Absyn_Let_d){goto _LL958;}else{goto _LL959;}
_LL959: if(*((int*)_tmp3AF)== Cyc_Absyn_Tunion_d){goto _LL960;}else{goto _LL961;}
_LL961: if(*((int*)_tmp3AF)== Cyc_Absyn_Letv_d){goto _LL962;}else{goto _LL963;}
_LL963: if(*((int*)_tmp3AF)== Cyc_Absyn_Namespace_d){goto _LL964;}else{goto _LL965;}
_LL965: if(*((int*)_tmp3AF)== Cyc_Absyn_Using_d){goto _LL966;}else{goto _LL967;}
_LL967: if(*((int*)_tmp3AF)== Cyc_Absyn_ExternC_d){goto _LL968;}else{goto _LL944;}
_LL946: {struct _tuple0 _tmp3D7;struct _tagged_arr*_tmp3D8;struct _tuple0*_tmp3D5=
_tmp3C9->name;_tmp3D7=*_tmp3D5;_LL985: _tmp3D8=_tmp3D7.f2;goto _LL982;_LL982: name=(
struct _tagged_arr*)_tmp3D8;goto _LL944;}_LL948: {struct _tuple0 _tmp3DC;struct
_tagged_arr*_tmp3DD;struct _tuple0*_tmp3DA=_tmp3CB->name;_tmp3DC=*_tmp3DA;_LL990:
_tmp3DD=_tmp3DC.f2;goto _LL987;_LL987: name=(struct _tagged_arr*)_tmp3DD;goto _LL944;}
_LL950: if(!((unsigned int)_tmp3CD->name)){name=0;}{struct _tuple0 _tmp3E1;struct
_tagged_arr*_tmp3E2;struct _tuple0*_tmp3DF=(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp3CD->name))->v;_tmp3E1=*_tmp3DF;_LL995: _tmp3E2=_tmp3E1.f2;goto
_LL992;_LL992: name=(struct _tagged_arr*)_tmp3E2;goto _LL944;}_LL952: if(!((
unsigned int)_tmp3CF->name)){name=0;}{struct _tuple0 _tmp3E6;struct _tagged_arr*
_tmp3E7;struct _tuple0*_tmp3E4=(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(
_tmp3CF->name))->v;_tmp3E6=*_tmp3E4;_LL1000: _tmp3E7=_tmp3E6.f2;goto _LL997;_LL997:
name=(struct _tagged_arr*)_tmp3E7;goto _LL944;}_LL954: {struct _tuple0 _tmp3EB;
struct _tagged_arr*_tmp3EC;struct _tuple0*_tmp3E9=_tmp3D1->name;_tmp3EB=*_tmp3E9;
_LL1005: _tmp3EC=_tmp3EB.f2;goto _LL1002;_LL1002: name=(struct _tagged_arr*)_tmp3EC;
goto _LL944;}_LL956: {struct _tuple0 _tmp3F0;struct _tagged_arr*_tmp3F1;struct
_tuple0*_tmp3EE=_tmp3D3->name;_tmp3F0=*_tmp3EE;_LL1010: _tmp3F1=_tmp3F0.f2;goto
_LL1007;_LL1007: name=(struct _tagged_arr*)_tmp3F1;goto _LL944;}_LL958: goto _LL960;
_LL960: goto _LL962;_LL962: goto _LL964;_LL964: goto _LL966;_LL966: goto _LL968;_LL968:
name=0;goto _LL944;_LL944:;}if(!((unsigned int)name)? ! _tmp3AE: 0){continue;}if((
unsigned int)name){ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp3F4;_tmp3F4.tag=
Cyc_Std_String_pa;_tmp3F4.f1=(struct _tagged_arr)*name;{void*_tmp3F3[1]={& _tmp3F4};
Cyc_Std_aprintf(_tag_arr("_%s_def_",sizeof(unsigned char),9),_tag_arr(_tmp3F3,
sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp3F6;_tmp3F6.tag=Cyc_Std_String_pa;
_tmp3F6.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp3F5[1]={& _tmp3F6};Cyc_Std_fprintf(
out_file,_tag_arr("#ifndef %s\n",sizeof(unsigned char),12),_tag_arr(_tmp3F5,
sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp3F8;_tmp3F8.tag=Cyc_Std_String_pa;
_tmp3F8.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp3F7[1]={& _tmp3F8};Cyc_Std_fprintf(
out_file,_tag_arr("#define %s\n",sizeof(unsigned char),12),_tag_arr(_tmp3F7,
sizeof(void*),1));}});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp3F9[1];
_tmp3F9[0]=_tmp3AD;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp3F9,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp3FA[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(
_tmp3FA,sizeof(void*),0));});}else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp3FB[1];_tmp3FB[0]=_tmp3AD;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp3FB,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}_tmp316=({
struct Cyc_Std_String_pa_struct _tmp3FF;_tmp3FF.tag=Cyc_Std_String_pa;_tmp3FF.f1=(
struct _tagged_arr)_tmp30B;{struct Cyc_Std_String_pa_struct _tmp3FE;_tmp3FE.tag=Cyc_Std_String_pa;
_tmp3FE.f1=(struct _tagged_arr)_tmp30C;{struct Cyc_Std_String_pa_struct _tmp3FD;
_tmp3FD.tag=Cyc_Std_String_pa;_tmp3FD.f1=(struct _tagged_arr)_tmp310;{void*
_tmp3FC[3]={& _tmp3FD,& _tmp3FE,& _tmp3FF};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s %s",
sizeof(unsigned char),19),_tag_arr(_tmp3FC,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp316)!= 0){return 1;}maybe=Cyc_Std_fopen((struct _tagged_arr)
_tmp30C,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)maybe)){return
1;}in_file=(struct Cyc_Std___sFILE*)_check_null(maybe);_tmp324=Cyc_Lexing_from_file(
in_file);{struct _tuple9*entry2;while((entry2=((struct _tuple9*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp324))!= 0){struct _tuple9 _tmp402;struct _tagged_arr*
_tmp403;struct _tagged_arr _tmp405;struct _tuple9*_tmp400=(struct _tuple9*)
_check_null(entry2);_tmp402=*_tmp400;_LL1030: _tmp405=_tmp402.f1;goto _LL1028;
_LL1028: _tmp403=_tmp402.f2;goto _LL1025;_LL1025: if(((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(_tmp348,_tmp403)){({struct Cyc_Std_String_pa_struct
_tmp408;_tmp408.tag=Cyc_Std_String_pa;_tmp408.f1=(struct _tagged_arr)*_tmp403;{
void*_tmp407[1]={& _tmp408};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n",
sizeof(unsigned char),12),_tag_arr(_tmp407,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp40A;_tmp40A.tag=Cyc_Std_String_pa;_tmp40A.f1=(struct _tagged_arr)_tmp405;{
void*_tmp409[1]={& _tmp40A};Cyc_Std_fprintf(out_file,_tag_arr("%s\n",sizeof(
unsigned char),4),_tag_arr(_tmp409,sizeof(void*),1));}});({void*_tmp40B[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(
_tmp40B,sizeof(void*),0));});}}Cyc_Std_fclose(in_file);Cyc_Std_remove((struct
_tagged_arr)_tmp30B);Cyc_Std_remove((struct _tagged_arr)_tmp30C);if(epilogue != 0){
struct Cyc_List_List*_tmp40C=epilogue;for(0;_tmp40C != 0;_tmp40C=_tmp40C->tl){
struct _tuple10 _tmp40F;struct _tagged_arr _tmp410;struct _tagged_arr _tmp412;struct
_tuple10*_tmp40D=(struct _tuple10*)_tmp40C->hd;_tmp40F=*_tmp40D;_LL1043: _tmp412=
_tmp40F.f1;goto _LL1041;_LL1041: _tmp410=_tmp40F.f2;goto _LL1038;_LL1038: if(_tmp410.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr? _tmp412.curr == ((struct _tagged_arr)
_tag_arr(0,0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp414=_cycalloc(sizeof(struct _tagged_arr));
_tmp414[0]=_tmp412;_tmp414;})): 0){Cyc_Std_fputs(_tmp410,out_file);}else{({struct
Cyc_Std_String_pa_struct _tmp416;_tmp416.tag=Cyc_Std_String_pa;_tmp416.f1=(struct
_tagged_arr)_tmp412;{void*_tmp415[1]={& _tmp416};Cyc_log(_tag_arr("%s is not supported on this platform\n",
sizeof(unsigned char),38),_tag_arr(_tmp415,sizeof(void*),1));}});}}}({void*
_tmp417[0]={};Cyc_Std_fprintf(out_file,_tag_arr("}\n",sizeof(unsigned char),3),
_tag_arr(_tmp417,sizeof(void*),0));});({void*_tmp418[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(_tmp418,sizeof(
void*),0));});Cyc_Std_fclose(out_file);if(cstubs != 0){out_file=(struct Cyc_Std___sFILE*)
_check_null(Cyc_cstubs_file);{struct Cyc_List_List*_tmp419=cstubs;for(0;_tmp419 != 
0;_tmp419=_tmp419->tl){struct _tuple10 _tmp41C;struct _tagged_arr _tmp41D;struct
_tagged_arr _tmp41F;struct _tuple10*_tmp41A=(struct _tuple10*)_tmp419->hd;_tmp41C=*
_tmp41A;_LL1056: _tmp41F=_tmp41C.f1;goto _LL1054;_LL1054: _tmp41D=_tmp41C.f2;goto
_LL1051;_LL1051: if(_tmp41D.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
_tmp41F.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp421=_cycalloc(sizeof(struct _tagged_arr));_tmp421[0]=_tmp41F;_tmp421;})): 0){
Cyc_Std_fputs(_tmp41D,out_file);}}}}out_file=(struct Cyc_Std___sFILE*)_check_null(
Cyc_cycstubs_file);({struct Cyc_Std_String_pa_struct _tmp423;_tmp423.tag=Cyc_Std_String_pa;
_tmp423.f1=(struct _tagged_arr)_tmp30F;{void*_tmp422[1]={& _tmp423};Cyc_Std_fprintf(
out_file,_tag_arr("#include <%s>\n\n",sizeof(unsigned char),16),_tag_arr(_tmp422,
sizeof(void*),1));}});if(cycstubs != 0){out_file=(struct Cyc_Std___sFILE*)
_check_null(Cyc_cycstubs_file);({void*_tmp424[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("namespace Std {\n",sizeof(unsigned char),17),_tag_arr(_tmp424,sizeof(
void*),0));});{struct Cyc_List_List*_tmp425=cycstubs;for(0;_tmp425 != 0;_tmp425=
_tmp425->tl){struct _tuple10 _tmp428;struct _tagged_arr _tmp429;struct _tagged_arr
_tmp42B;struct _tuple10*_tmp426=(struct _tuple10*)_tmp425->hd;_tmp428=*_tmp426;
_LL1068: _tmp42B=_tmp428.f1;goto _LL1066;_LL1066: _tmp429=_tmp428.f2;goto _LL1063;
_LL1063: if(_tmp429.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr? _tmp42B.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp42D=_cycalloc(sizeof(struct _tagged_arr));_tmp42D[0]=_tmp42B;_tmp42D;})): 0){
Cyc_Std_fputs(_tmp429,out_file);}}}({void*_tmp42E[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("}\n\n",sizeof(unsigned char),4),_tag_arr(_tmp42E,sizeof(void*),0));});}
if(Cyc_Std_chdir((struct _tagged_arr)_tmp304)){({struct Cyc_Std_String_pa_struct
_tmp430;_tmp430.tag=Cyc_Std_String_pa;_tmp430.f1=(struct _tagged_arr)_tmp304;{
void*_tmp42F[1]={& _tmp430};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not change directory to %s\n",
sizeof(unsigned char),41),_tag_arr(_tmp42F,sizeof(void*),1));}});return 1;}return
0;}}}}}}}}}}int Cyc_process_specfile(struct _tagged_arr file,struct _tagged_arr dir){
struct Cyc_Std___sFILE*_tmp43E=Cyc_Std_fopen(file,_tag_arr("r",sizeof(
unsigned char),2));if(!((unsigned int)_tmp43E)){return 1;}{struct Cyc_Std___sFILE*
_tmp43F=(struct Cyc_Std___sFILE*)_check_null(_tmp43E);struct Cyc_Lexing_lexbuf*
_tmp440=Cyc_Lexing_from_file(_tmp43F);struct _tuple11*entry;while((entry=((struct
_tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp440))!= 0){struct
_tuple11 _tmp443;struct Cyc_List_List*_tmp444;struct Cyc_List_List*_tmp446;struct
Cyc_List_List*_tmp448;struct Cyc_List_List*_tmp44A;struct Cyc_List_List*_tmp44C;
struct Cyc_List_List*_tmp44E;struct _tagged_arr _tmp450;struct _tuple11*_tmp441=(
struct _tuple11*)_check_null(entry);_tmp443=*_tmp441;_LL1105: _tmp450=_tmp443.f1;
goto _LL1103;_LL1103: _tmp44E=_tmp443.f2;goto _LL1101;_LL1101: _tmp44C=_tmp443.f3;
goto _LL1099;_LL1099: _tmp44A=_tmp443.f4;goto _LL1097;_LL1097: _tmp448=_tmp443.f5;
goto _LL1095;_LL1095: _tmp446=_tmp443.f6;goto _LL1093;_LL1093: _tmp444=_tmp443.f7;
goto _LL1090;_LL1090: if(Cyc_process_file(_tmp450,dir,_tmp44E,_tmp44C,_tmp44A,
_tmp448,_tmp446,_tmp444)){Cyc_Std_fclose(_tmp43F);return 1;}}Cyc_Std_fclose(
_tmp43F);return 0;}}int Cyc_getsize(struct _tagged_arr dir,struct _tagged_arr includes,
struct _tagged_arr type){struct _tagged_arr _tmp452=Cyc_Filename_concat(dir,_tag_arr("getsize.c",
sizeof(unsigned char),10));struct _tagged_arr _tmp453=Cyc_Filename_concat(dir,
_tag_arr("getsize",sizeof(unsigned char),8));struct _tagged_arr _tmp454=Cyc_Filename_concat(
dir,_tag_arr("getsize.out",sizeof(unsigned char),12));struct Cyc_Std___sFILE*
_tmp455=Cyc_Std_fopen((struct _tagged_arr)_tmp452,_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)_tmp455)){({struct Cyc_Std_String_pa_struct
_tmp457;_tmp457.tag=Cyc_Std_String_pa;_tmp457.f1=(struct _tagged_arr)_tmp452;{
void*_tmp456[1]={& _tmp457};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create %s\n",
sizeof(unsigned char),28),_tag_arr(_tmp456,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp459;_tmp459.tag=Cyc_Std_String_pa;_tmp459.f1=(struct _tagged_arr)type;{void*
_tmp458[1]={& _tmp459};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",
sizeof(unsigned char),40),_tag_arr(_tmp458,sizeof(void*),1));}});return - 1;}({
struct Cyc_Std_String_pa_struct _tmp45E;_tmp45E.tag=Cyc_Std_String_pa;_tmp45E.f1=(
struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct _tmp45D;_tmp45D.tag=Cyc_Std_String_pa;
_tmp45D.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct _tmp45C;
_tmp45C.tag=Cyc_Std_String_pa;_tmp45C.f1=(struct _tagged_arr)_tmp454;{struct Cyc_Std_String_pa_struct
_tmp45B;_tmp45B.tag=Cyc_Std_String_pa;_tmp45B.f1=(struct _tagged_arr)includes;{
void*_tmp45A[4]={& _tmp45B,& _tmp45C,& _tmp45D,& _tmp45E};Cyc_Std_fprintf((struct Cyc_Std___sFILE*)
_check_null(_tmp455),_tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof(unsigned char),139),_tag_arr(_tmp45A,sizeof(void*),4));}}}}});Cyc_Std_fclose((
struct Cyc_Std___sFILE*)_check_null(_tmp455));{struct _tagged_arr _tmp45F=
Cstring_to_string(Ccomp);struct _tagged_arr _tmp460=({struct Cyc_Std_String_pa_struct
_tmp475;_tmp475.tag=Cyc_Std_String_pa;_tmp475.f1=(struct _tagged_arr)_tmp452;{
struct Cyc_Std_String_pa_struct _tmp474;_tmp474.tag=Cyc_Std_String_pa;_tmp474.f1=(
struct _tagged_arr)_tmp453;{struct Cyc_Std_String_pa_struct _tmp473;_tmp473.tag=Cyc_Std_String_pa;
_tmp473.f1=(struct _tagged_arr)_tmp45F;{void*_tmp472[3]={& _tmp473,& _tmp474,&
_tmp475};Cyc_Std_aprintf(_tag_arr("%s -o %s %s",sizeof(unsigned char),12),
_tag_arr(_tmp472,sizeof(void*),3));}}}});if(Cyc_Std_system((struct _tagged_arr)
_tmp460)!= 0){({struct Cyc_Std_String_pa_struct _tmp462;_tmp462.tag=Cyc_Std_String_pa;
_tmp462.f1=(struct _tagged_arr)type;{void*_tmp461[1]={& _tmp462};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof(unsigned char),59),_tag_arr(_tmp461,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp464;_tmp464.tag=Cyc_Std_String_pa;_tmp464.f1=(struct _tagged_arr)type;{void*
_tmp463[1]={& _tmp464};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",
sizeof(unsigned char),40),_tag_arr(_tmp463,sizeof(void*),1));}});return - 1;}{int
_tmp465=Cyc_Std_system((struct _tagged_arr)_tmp453);({struct Cyc_Std_Int_pa_struct
_tmp468;_tmp468.tag=Cyc_Std_Int_pa;_tmp468.f1=(int)((unsigned int)_tmp465);{
struct Cyc_Std_String_pa_struct _tmp467;_tmp467.tag=Cyc_Std_String_pa;_tmp467.f1=(
struct _tagged_arr)type;{void*_tmp466[2]={& _tmp467,& _tmp468};Cyc_log(_tag_arr("size of %s returned by system is %d\n",
sizeof(unsigned char),37),_tag_arr(_tmp466,sizeof(void*),2));}}});{struct Cyc_Std___sFILE*
_tmp469=Cyc_Std_fopen((struct _tagged_arr)_tmp454,_tag_arr("r",sizeof(
unsigned char),2));int w=- 1;if(({struct Cyc_Std_IntPtr_sa_struct _tmp46B;_tmp46B.tag=
Cyc_Std_IntPtr_sa;_tmp46B.f1=& w;{void*_tmp46A[1]={& _tmp46B};Cyc_Std_fscanf((
struct Cyc_Std___sFILE*)_check_null(_tmp469),_tag_arr("%d",sizeof(unsigned char),
3),_tag_arr(_tmp46A,sizeof(void*),1));}})!= 1){({struct Cyc_Std_String_pa_struct
_tmp46D;_tmp46D.tag=Cyc_Std_String_pa;_tmp46D.f1=(struct _tagged_arr)type;{void*
_tmp46C[1]={& _tmp46D};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",
sizeof(unsigned char),40),_tag_arr(_tmp46C,sizeof(void*),1));}});return - 1;}({
struct Cyc_Std_Int_pa_struct _tmp471;_tmp471.tag=Cyc_Std_Int_pa;_tmp471.f1=(int)((
unsigned int)w);{struct Cyc_Std_String_pa_struct _tmp470;_tmp470.tag=Cyc_Std_String_pa;
_tmp470.f1=(struct _tagged_arr)_tmp454;{struct Cyc_Std_String_pa_struct _tmp46F;
_tmp46F.tag=Cyc_Std_String_pa;_tmp46F.f1=(struct _tagged_arr)type;{void*_tmp46E[3]={&
_tmp46F,& _tmp470,& _tmp471};Cyc_log(_tag_arr("size of %s read from file %s is %d\n",
sizeof(unsigned char),36),_tag_arr(_tmp46E,sizeof(void*),3));}}}});Cyc_Std_fclose((
struct Cyc_Std___sFILE*)_check_null(_tmp469));Cyc_Std_remove((struct _tagged_arr)
_tmp454);Cyc_Std_remove((struct _tagged_arr)_tmp452);Cyc_Std_remove((struct
_tagged_arr)_tmp453);return w;}}}}extern void GC_blacklist_warn_clear();int Cyc_main(
int argc,struct _tagged_arr argv){GC_blacklist_warn_clear();if(argc < 3){({void*
_tmp476[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof(unsigned char),105),_tag_arr(_tmp476,sizeof(void*),0));});return 1;}{
struct _tagged_arr _tmp477=*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),1));if(Cyc_force_directory_prefixes((struct _tagged_arr)
_tmp477)? 1: Cyc_force_directory((struct _tagged_arr)_tmp477)){({struct Cyc_Std_String_pa_struct
_tmp479;_tmp479.tag=Cyc_Std_String_pa;_tmp479.f1=(struct _tagged_arr)_tmp477;{
void*_tmp478[1]={& _tmp479};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create directory %s\n",
sizeof(unsigned char),38),_tag_arr(_tmp478,sizeof(void*),1));}});return 1;}Cyc_log_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp477,
_tag_arr("BUILDLIB.LOG",sizeof(unsigned char),13)),_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)Cyc_log_file)){({struct Cyc_Std_String_pa_struct
_tmp47B;_tmp47B.tag=Cyc_Std_String_pa;_tmp47B.f1=(struct _tagged_arr)_tmp477;{
void*_tmp47A[1]={& _tmp47B};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create log file in directory %s\n",
sizeof(unsigned char),50),_tag_arr(_tmp47A,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp477,
_tag_arr("cstubs.c",sizeof(unsigned char),9)),_tag_arr("w",sizeof(unsigned char),
2));if(!((unsigned int)Cyc_cstubs_file)){({struct Cyc_Std_String_pa_struct _tmp47D;
_tmp47D.tag=Cyc_Std_String_pa;_tmp47D.f1=(struct _tagged_arr)_tmp477;{void*
_tmp47C[1]={& _tmp47D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof(unsigned char),50),_tag_arr(_tmp47C,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp477,
_tag_arr("cycstubs.cyc",sizeof(unsigned char),13)),_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)Cyc_cycstubs_file)){({struct Cyc_Std_String_pa_struct
_tmp47F;_tmp47F.tag=Cyc_Std_String_pa;_tmp47F.f1=(struct _tagged_arr)_tmp477;{
void*_tmp47E[1]={& _tmp47F};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof(unsigned char),52),_tag_arr(_tmp47E,sizeof(void*),1));}});return 1;}({void*
_tmp480[0]={};Cyc_Std_fprintf((struct Cyc_Std___sFILE*)_check_null(Cyc_cycstubs_file),
_tag_arr("#include <core.h>\nusing Core;\n\n",sizeof(unsigned char),32),_tag_arr(
_tmp480,sizeof(void*),0));});Cyc_sizeof_unsignedlongint=Cyc_getsize((struct
_tagged_arr)_tmp477,_tag_arr("",sizeof(unsigned char),1),_tag_arr("unsigned long int",
sizeof(unsigned char),18));Cyc_sizeof_unsignedshortint=Cyc_getsize((struct
_tagged_arr)_tmp477,_tag_arr("",sizeof(unsigned char),1),_tag_arr("unsigned short int",
sizeof(unsigned char),19));Cyc_sizeof_shortint=Cyc_getsize((struct _tagged_arr)
_tmp477,_tag_arr("",sizeof(unsigned char),1),_tag_arr("short int",sizeof(
unsigned char),10));Cyc_sizeof_int=Cyc_getsize((struct _tagged_arr)_tmp477,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("int",sizeof(unsigned char),4));
Cyc_sizeof_short=Cyc_getsize((struct _tagged_arr)_tmp477,_tag_arr("",sizeof(
unsigned char),1),_tag_arr("short",sizeof(unsigned char),6));Cyc_sizeof_fdmask=
Cyc_getsize((struct _tagged_arr)_tmp477,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(unsigned char),47),_tag_arr("fd_mask",sizeof(unsigned char),8));Cyc_sizeof___fdmask=
Cyc_getsize((struct _tagged_arr)_tmp477,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(unsigned char),47),_tag_arr("__fd_mask",sizeof(unsigned char),10));Cyc_sizeof_uint32=
Cyc_getsize((struct _tagged_arr)_tmp477,_tag_arr("#include <sys/types.h>",sizeof(
unsigned char),23),_tag_arr("__uint32_t",sizeof(unsigned char),11));Cyc_sizeof_sockaddr=
Cyc_getsize((struct _tagged_arr)_tmp477,_tag_arr("#include <sys/types.h>\n#include <sys/socket.h>",
sizeof(unsigned char),47),_tag_arr("struct sockaddr",sizeof(unsigned char),16));
Cyc_sizeof_inport=Cyc_getsize((struct _tagged_arr)_tmp477,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(unsigned char),47),_tag_arr("in_port_t",sizeof(unsigned char),10));Cyc_sizeof_inaddr=
Cyc_getsize((struct _tagged_arr)_tmp477,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(unsigned char),47),_tag_arr("struct in_addr",sizeof(unsigned char),15));{
int i=2;for(0;i < argc;i ++){Cyc_process_specfile((struct _tagged_arr)((struct
_tagged_arr*)argv.curr)[i],(struct _tagged_arr)_tmp477);}}Cyc_Std_fclose((struct
Cyc_Std___sFILE*)_check_null(Cyc_log_file));Cyc_Std_fclose((struct Cyc_Std___sFILE*)
_check_null(Cyc_cstubs_file));Cyc_Std_fclose((struct Cyc_Std___sFILE*)_check_null(
Cyc_cycstubs_file));return 0;}}
