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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

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
 extern void exit(int);struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _tagged_arr Cstring_to_string(unsigned char*);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern int
Cyc_Std_remove(struct _tagged_arr);extern int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);
extern int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);extern struct Cyc_Std___cycFILE*
Cyc_Std_fopen(struct _tagged_arr __filename,struct _tagged_arr __modes);extern int Cyc_Std_fputc(
int __c,struct Cyc_Std___cycFILE*__stream);extern int Cyc_Std_fputs(struct
_tagged_arr __s,struct Cyc_Std___cycFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern int Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);extern struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_vfprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern int Cyc_Std_fscanf(struct Cyc_Std___cycFILE*stream,struct
_tagged_arr fmt,struct _tagged_arr);extern unsigned char Cyc_Lexing_Error[10];struct
Cyc_Lexing_Error_struct{unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);extern struct
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
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
extern unsigned char Cyc_Array_Array_mismatch[19];struct Cyc_Std__Div{int quot;int
rem;};struct Cyc_Std__Ldiv{int quot;int rem;};extern int Cyc_Std_system(struct
_tagged_arr);struct Cyc_Buffer_t;extern struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);extern struct _tagged_arr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
extern void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,unsigned char);extern void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*,struct _tagged_arr);struct Cyc_Std_tm{int tm_sec;int tm_min;int
tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;};
extern int timezone;struct Cyc_Std_stat_t{unsigned long long st_dev;unsigned short
__pad1;unsigned int st_ino;unsigned int st_mode;unsigned int st_nlink;unsigned int
st_uid;unsigned int st_gid;unsigned long long st_rdev;unsigned short __pad2;int
st_size;int st_blksize;int st_blocks;int st_atime;unsigned int __unused1;int st_mtime;
unsigned int __unused2;int st_ctime;unsigned int __unused3;unsigned int __unused4;
unsigned int __unused5;};extern int Cyc_Std_mkdir(struct _tagged_arr pathname,
unsigned int mode);struct Cyc_Std_flock{short l_type;int l_start;short l_whence;int
l_len;int l_pid;};struct Cyc_Std_Flock_struct{int tag;struct Cyc_Std_flock*f1;};
struct Cyc_Std_Long_struct{int tag;int f1;};extern int Cyc_Std_open(struct _tagged_arr,
int,struct _tagged_arr);struct Cyc_Std_timeval{int tv_sec;int tv_usec;};struct Cyc_Std_timezone{
int tz_minuteswest;int tz_dsttime;};struct Cyc_Std_itimerval{struct Cyc_Std_timeval
it_interval;struct Cyc_Std_timeval it_value;};typedef struct{unsigned int __val[64];}
Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t Cyc_Std_sigset_t;struct Cyc_Std_timespec{
int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;struct Cyc_Std___fd_set{int
__fds_bits[32];};struct Cyc_Std_option{struct _tagged_arr name;int has_arg;int*flag;
int val;};extern int close(int);int Cyc_Std_access(struct _tagged_arr,int);int Cyc_Std_chdir(
struct _tagged_arr);int Cyc_Std_chown(struct _tagged_arr,unsigned int,unsigned int);
struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,unsigned int size);int Cyc_Std_execl(
struct _tagged_arr path,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execlp(
struct _tagged_arr file,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execve(
struct _tagged_arr filename,struct _tagged_arr argv,struct _tagged_arr envp);int Cyc_Std_link(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_read(int fd,struct _tagged_arr buf,
unsigned int count);int Cyc_Std_rmdir(struct _tagged_arr);int Cyc_Std_symlink(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_truncate(struct _tagged_arr,int);int Cyc_Std_write(
int fd,struct _tagged_arr buf,unsigned int count);int Cyc_Std_unlink(struct
_tagged_arr pathname);int Cyc_Std_gethostname(struct _tagged_arr,unsigned int);int
Cyc_Std_chroot(struct _tagged_arr);struct _tagged_arr Cyc_Std_getpass(struct
_tagged_arr prompt);extern int isspace(int);extern int toupper(int);struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*
tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
Cyc_Absyn_Aggrdecl**f1;};struct Cyc_Absyn_AggrInfo{void*aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo
f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;
void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*
f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{int tag;
void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int
tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
int tag;void*f1;unsigned char f2;};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;
short f2;};struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{
int tag;void*f1;long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Absyn_String_c_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{int tag;
struct _tagged_arr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;unsigned int f1;}
;struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;
struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{int
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*f1;void*f2;
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Aggrdecl*f3;};
struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple2 condition;
struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;unsigned char f1;};struct Cyc_Absyn_Float_p_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct
Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];struct _tuple3{void*f1;struct _tuple0*f2;};extern struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc_Std___cycFILE*f);extern struct Cyc_List_List*Cyc_Parse_parse_file(struct
Cyc_Std___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
extern int Cyc_yyparse();extern unsigned char Cyc_AbstractDeclarator_tok[27];struct
Cyc_AbstractDeclarator_tok_struct{unsigned char*tag;struct Cyc_Abstractdeclarator*
f1;};extern unsigned char Cyc_AggrFieldDeclListList_tok[30];struct Cyc_AggrFieldDeclListList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_AggrFieldDeclList_tok[
26];struct Cyc_AggrFieldDeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*
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
21];struct _tuple4{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple4*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple5{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple6{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple7{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple7*f1;}
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
unsigned char Cyc_QualSpecList_tok[21];struct _tuple8{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple8*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
struct Cyc_Rgnorder_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{unsigned char*tag;void*
f1;};extern unsigned char Cyc_Short_tok[14];struct Cyc_Short_tok_struct{
unsigned char*tag;short f1;};extern unsigned char Cyc_Stmt_tok[13];struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};extern unsigned char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_String_tok[15];struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Stringopt_tok[18];struct Cyc_Stringopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_StructOrUnion_tok[
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
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};extern void Cyc_Lex_lex_init();extern unsigned char*Ccomp;struct
Cyc_Std___cycFILE*Cyc_log_file=0;struct Cyc_Std___cycFILE*Cyc_cstubs_file=0;
struct Cyc_Std___cycFILE*Cyc_cycstubs_file=0;int Cyc_log(struct _tagged_arr fmt,
struct _tagged_arr ap){int _tmp0=Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_log_file),fmt,ap);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)((
struct Cyc_Std___cycFILE*)_check_null(Cyc_log_file)));return _tmp0;}int Cyc_sizeof_unsignedlongint=
- 1;int Cyc_sizeof_unsignedshortint=- 1;int Cyc_sizeof_shortint=- 1;int Cyc_sizeof_int=
- 1;int Cyc_sizeof_short=- 1;int Cyc_sizeof_fdmask=- 1;int Cyc_sizeof___fdmask=- 1;int
Cyc_sizeof_uint32=- 1;int Cyc_sizeof_sockaddr=- 1;int Cyc_sizeof_inport=- 1;int Cyc_sizeof_inaddr=
- 1;static struct _tagged_arr*Cyc_current_source=0;static struct Cyc_List_List*Cyc_current_args=
0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(struct
_tagged_arr*sptr){Cyc_current_targets=({struct Cyc_Set_Set**_tmp1=_cycalloc(
sizeof(*_tmp1));_tmp1[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr);_tmp1;});}struct _tuple9{struct _tagged_arr*f1;struct Cyc_Set_Set*f2;};extern
struct _tuple9*Cyc_line(struct Cyc_Lexing_lexbuf*);extern int Cyc_macroname(struct
Cyc_Lexing_lexbuf*);extern int Cyc_args(struct Cyc_Lexing_lexbuf*);extern int Cyc_token(
struct Cyc_Lexing_lexbuf*);extern int Cyc_string(struct Cyc_Lexing_lexbuf*);struct
Cyc_Std___cycFILE*Cyc_slurp_out=0;extern int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
extern int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);extern int Cyc_asm(struct Cyc_Lexing_lexbuf*);
extern int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);extern int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple10{struct _tagged_arr f1;struct _tagged_arr*
f2;};extern struct _tuple10*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);extern int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf*);extern int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _tagged_arr Cyc_current_line=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};struct _tuple11{struct _tagged_arr f1;struct _tagged_arr f2;};struct _tuple12{
struct _tagged_arr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};extern
struct _tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*);extern int Cyc_commands(struct
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
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2[0]=({struct Cyc_Lexing_Error_struct _tmp3;
_tmp3.tag=Cyc_Lexing_Error;_tmp3.f1=_tag_arr("empty token",sizeof(unsigned char),
12);_tmp3;});_tmp2;}));}else{return lbuf->lex_last_action;}}else{if(c == 256){lbuf->lex_eof_reached=
0;}}}}struct _tuple9*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL0: Cyc_macroname(
lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)
_check_null(Cyc_current_args))->tl){Cyc_current_targets=({struct Cyc_Set_Set**
_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(
struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);
_tmp4;});}return({struct _tuple9*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->f1=(
struct _tagged_arr*)_check_null(Cyc_current_source);_tmp5->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets));_tmp5;});case 1: _LL1: return Cyc_line(lexbuf);
case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp6=
_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Lexing_Error_struct _tmp7;_tmp7.tag=
Cyc_Lexing_Error;_tmp7.f1=_tag_arr("some action didn't return!",sizeof(
unsigned char),27);_tmp7;});_tmp6;}));}struct _tuple9*Cyc_line(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL5: Cyc_current_source=({struct _tagged_arr*_tmp8=_cycalloc(sizeof(*_tmp8));
_tmp8[0]=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 2));_tmp8;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmp9;});
Cyc_token(lexbuf);return 0;case 1: _LL6: Cyc_current_source=({struct _tagged_arr*
_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=(struct _tagged_arr)Cyc_Std_substring((
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmpA;});Cyc_current_args=0;Cyc_current_targets=({
struct Cyc_Set_Set**_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
_tmpB;});Cyc_args(lexbuf);return 0;case 2: _LL7: Cyc_current_source=({struct
_tagged_arr*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmpC;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**
_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_tmpD;});
Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);return Cyc_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpE=
_cycalloc(sizeof(*_tmpE));_tmpE[0]=({struct Cyc_Lexing_Error_struct _tmpF;_tmpF.tag=
Cyc_Lexing_Error;_tmpF.f1=_tag_arr("some action didn't return!",sizeof(
unsigned char),27);_tmpF;});_tmpE;}));}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_macroname_rec(lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLA: {struct _tagged_arr*_tmp10=({struct _tagged_arr*_tmp12=_cycalloc(
sizeof(*_tmp12));_tmp12[0]=(struct _tagged_arr)Cyc_Std_substring((struct
_tagged_arr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp12;});Cyc_current_args=({
struct Cyc_List_List*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->hd=_tmp10;_tmp11->tl=
Cyc_current_args;_tmp11;});return Cyc_args(lexbuf);}case 1: _LLB: {struct
_tagged_arr*_tmp13=({struct _tagged_arr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[
0]=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));_tmp15;});Cyc_current_args=({struct Cyc_List_List*_tmp14=_cycalloc(
sizeof(*_tmp14));_tmp14->hd=_tmp13;_tmp14->tl=Cyc_current_args;_tmp14;});return
Cyc_token(lexbuf);}default: _LLC:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp16=
_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Lexing_Error_struct _tmp17;
_tmp17.tag=Cyc_Lexing_Error;_tmp17.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp17;});_tmp16;}));}int Cyc_args(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLE: Cyc_add_target(({struct _tagged_arr*_tmp18=_cycalloc(sizeof(*_tmp18));
_tmp18[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);_tmp18;}));return Cyc_token(
lexbuf);case 1: _LLF: return 0;case 2: _LL10: return Cyc_token(lexbuf);case 3: _LL11: Cyc_string(
lexbuf);return Cyc_token(lexbuf);case 4: _LL12: return Cyc_token(lexbuf);case 5: _LL13:
return Cyc_token(lexbuf);case 6: _LL14: return Cyc_token(lexbuf);case 7: _LL15: return
Cyc_token(lexbuf);case 8: _LL16: return Cyc_token(lexbuf);case 9: _LL17: return Cyc_token(
lexbuf);case 10: _LL18: return Cyc_token(lexbuf);case 11: _LL19: return Cyc_token(lexbuf);
case 12: _LL1A: return Cyc_token(lexbuf);case 13: _LL1B: return Cyc_token(lexbuf);case 14:
_LL1C: return Cyc_token(lexbuf);case 15: _LL1D: return Cyc_token(lexbuf);case 16: _LL1E:
return Cyc_token(lexbuf);case 17: _LL1F: return Cyc_token(lexbuf);case 18: _LL20: return
Cyc_token(lexbuf);case 19: _LL21: return Cyc_token(lexbuf);case 20: _LL22: return Cyc_token(
lexbuf);case 21: _LL23: return Cyc_token(lexbuf);case 22: _LL24: return Cyc_token(lexbuf);
case 23: _LL25: return Cyc_token(lexbuf);case 24: _LL26: return Cyc_token(lexbuf);case 25:
_LL27: return Cyc_token(lexbuf);case 26: _LL28: return Cyc_token(lexbuf);case 27: _LL29:
return Cyc_token(lexbuf);case 28: _LL2A: return Cyc_token(lexbuf);case 29: _LL2B: return
Cyc_token(lexbuf);case 30: _LL2C: return Cyc_token(lexbuf);case 31: _LL2D: return Cyc_token(
lexbuf);case 32: _LL2E: return Cyc_token(lexbuf);case 33: _LL2F: return Cyc_token(lexbuf);
case 34: _LL30: return Cyc_token(lexbuf);case 35: _LL31: return Cyc_token(lexbuf);case 36:
_LL32: return Cyc_token(lexbuf);case 37: _LL33: return Cyc_token(lexbuf);case 38: _LL34:
return Cyc_token(lexbuf);case 39: _LL35: return Cyc_token(lexbuf);case 40: _LL36: return
Cyc_token(lexbuf);case 41: _LL37: return Cyc_token(lexbuf);case 42: _LL38: return Cyc_token(
lexbuf);case 43: _LL39: return Cyc_token(lexbuf);case 44: _LL3A: return Cyc_token(lexbuf);
case 45: _LL3B: return Cyc_token(lexbuf);case 46: _LL3C: return Cyc_token(lexbuf);case 47:
_LL3D: return Cyc_token(lexbuf);case 48: _LL3E: return Cyc_token(lexbuf);default: _LL3F:(
lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({
struct Cyc_Lexing_Error_struct _tmp1A;_tmp1A.tag=Cyc_Lexing_Error;_tmp1A.f1=
_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp1A;});_tmp19;}));}
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}int
Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL41: return Cyc_string(lexbuf);case 1:
_LL42: return 0;case 2: _LL43: return Cyc_string(lexbuf);case 3: _LL44: return Cyc_string(
lexbuf);case 4: _LL45: return Cyc_string(lexbuf);case 5: _LL46: return Cyc_string(lexbuf);
case 6: _LL47: return Cyc_string(lexbuf);case 7: _LL48: return 0;case 8: _LL49: return 0;
case 9: _LL4A: return Cyc_string(lexbuf);default: _LL4B:(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Lexing_Error_struct
_tmp1C;_tmp1C.tag=Cyc_Lexing_Error;_tmp1C.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp1C;});_tmp1B;}));}int Cyc_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_string_rec(lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL4D: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_slurp_string(lexbuf)){return 1;}else{return Cyc_slurp(lexbuf);}case 1: _LL4E:
return 0;case 2: _LL4F:({struct Cyc_Std_Int_pa_struct _tmp1E;_tmp1E.tag=1;_tmp1E.f1=(
int)((unsigned int)Cyc_sizeof_fdmask);{void*_tmp1D[1]={& _tmp1E};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp1D,sizeof(void*),1));}});({void*_tmp1F[0]={};Cyc_log(
_tag_arr("Warning: sizeof(fd_mask) inlined\n",sizeof(unsigned char),34),_tag_arr(
_tmp1F,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 3: _LL50:({struct Cyc_Std_Int_pa_struct
_tmp21;_tmp21.tag=1;_tmp21.f1=(int)((unsigned int)Cyc_sizeof___fdmask);{void*
_tmp20[1]={& _tmp21};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp20,sizeof(void*),1));}});({
void*_tmp22[0]={};Cyc_log(_tag_arr("Warning: sizeof(__fd_mask) inlined\n",
sizeof(unsigned char),36),_tag_arr(_tmp22,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 4: _LL51:({struct Cyc_Std_Int_pa_struct _tmp24;_tmp24.tag=1;_tmp24.f1=(
int)((unsigned int)Cyc_sizeof_uint32);{void*_tmp23[1]={& _tmp24};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp23,sizeof(void*),1));}});({void*_tmp25[0]={};Cyc_log(
_tag_arr("Warning: sizeof(__uint32_t) inlined\n",sizeof(unsigned char),37),
_tag_arr(_tmp25,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 5: _LL52:({
struct Cyc_Std_Int_pa_struct _tmp27;_tmp27.tag=1;_tmp27.f1=(int)((unsigned int)Cyc_sizeof_inport);{
void*_tmp26[1]={& _tmp27};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp26,sizeof(
void*),1));}});({void*_tmp28[0]={};Cyc_log(_tag_arr("Warning: sizeof(__in_port_t) inlined\n",
sizeof(unsigned char),38),_tag_arr(_tmp28,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 6: _LL53:({struct Cyc_Std_Int_pa_struct _tmp2A;_tmp2A.tag=1;_tmp2A.f1=(
int)((unsigned int)Cyc_sizeof_int);{void*_tmp29[1]={& _tmp2A};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp29,sizeof(void*),1));}});({void*_tmp2B[0]={};Cyc_log(
_tag_arr("Warning: sizeof(int) inlined\n",sizeof(unsigned char),30),_tag_arr(
_tmp2B,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 7: _LL54:({struct Cyc_Std_Int_pa_struct
_tmp2D;_tmp2D.tag=1;_tmp2D.f1=(int)((unsigned int)Cyc_sizeof_short);{void*_tmp2C[
1]={& _tmp2D};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp2C,sizeof(void*),1));}});({
void*_tmp2E[0]={};Cyc_log(_tag_arr("Warning: sizeof(__short) inlined\n",sizeof(
unsigned char),34),_tag_arr(_tmp2E,sizeof(void*),0));});return Cyc_slurp(lexbuf);
case 8: _LL55:({struct Cyc_Std_Int_pa_struct _tmp30;_tmp30.tag=1;_tmp30.f1=(int)((
unsigned int)Cyc_sizeof_unsignedlongint);{void*_tmp2F[1]={& _tmp30};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp2F,sizeof(void*),1));}});({void*_tmp31[0]={};Cyc_log(
_tag_arr("Warning: sizeof(unsigned long int) inlined\n",sizeof(unsigned char),44),
_tag_arr(_tmp31,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 9: _LL56:({
struct Cyc_Std_Int_pa_struct _tmp33;_tmp33.tag=1;_tmp33.f1=(int)((unsigned int)Cyc_sizeof_unsignedshortint);{
void*_tmp32[1]={& _tmp33};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp32,sizeof(
void*),1));}});({void*_tmp34[0]={};Cyc_log(_tag_arr("Warning: sizeof(unsigned short int) inlined\n",
sizeof(unsigned char),45),_tag_arr(_tmp34,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 10: _LL57:({struct Cyc_Std_Int_pa_struct _tmp36;_tmp36.tag=1;_tmp36.f1=(
int)((unsigned int)Cyc_sizeof_shortint);{void*_tmp35[1]={& _tmp36};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp35,sizeof(void*),1));}});({void*_tmp37[0]={};Cyc_log(
_tag_arr("Warning: sizeof(short int) inlined\n",sizeof(unsigned char),36),
_tag_arr(_tmp37,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 11: _LL58:({
struct Cyc_Std_Int_pa_struct _tmp39;_tmp39.tag=1;_tmp39.f1=(int)((unsigned int)Cyc_sizeof_sockaddr);{
void*_tmp38[1]={& _tmp39};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp38,sizeof(
void*),1));}});({void*_tmp3A[0]={};Cyc_log(_tag_arr("Warning: sizeof(struct sockaddr) inlined\n",
sizeof(unsigned char),42),_tag_arr(_tmp3A,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 12: _LL59:({struct Cyc_Std_Int_pa_struct _tmp3C;_tmp3C.tag=1;_tmp3C.f1=(
int)((unsigned int)Cyc_sizeof_inaddr);{void*_tmp3B[1]={& _tmp3C};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp3B,sizeof(void*),1));}});({void*_tmp3D[0]={};Cyc_log(
_tag_arr("Warning: sizeof(struct inaddr) inlined\n",sizeof(unsigned char),40),
_tag_arr(_tmp3D,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 13: _LL5A: Cyc_Std_fputs(
_tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC ",sizeof(unsigned char),30),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp3E[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp3E,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 14: _LL5B: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC ",
sizeof(unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp3F[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp3F,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 15: _LL5C: Cyc_Std_fputs(_tag_arr(" __IGNORE_FOR_CYCLONE_CALLOC ",
sizeof(unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp40[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp40,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 16: _LL5D: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC ",
sizeof(unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp41[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp41,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 17: _LL5E: Cyc_Std_fputs(_tag_arr("__region",sizeof(unsigned char),9),(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp42[0]={};Cyc_log(
_tag_arr("Warning: use of region sidestepped\n",sizeof(unsigned char),36),
_tag_arr(_tmp42,sizeof(void*),0));});return Cyc_slurp(lexbuf);case 18: _LL5F: return
Cyc_slurp(lexbuf);case 19: _LL60: return Cyc_slurp(lexbuf);case 20: _LL61: return Cyc_slurp(
lexbuf);case 21: _LL62: return Cyc_slurp(lexbuf);case 22: _LL63: return Cyc_slurp(lexbuf);
case 23: _LL64: return Cyc_slurp(lexbuf);case 24: _LL65: Cyc_Std_fputs(_tag_arr("inline",
sizeof(unsigned char),7),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return Cyc_slurp(lexbuf);case 25: _LL66: Cyc_Std_fputs(_tag_arr("inline",sizeof(
unsigned char),7),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return
Cyc_slurp(lexbuf);case 26: _LL67: Cyc_Std_fputs(_tag_arr("const",sizeof(
unsigned char),6),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return
Cyc_slurp(lexbuf);case 27: _LL68: Cyc_Std_fputs(_tag_arr("const",sizeof(
unsigned char),6),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return
Cyc_slurp(lexbuf);case 28: _LL69: Cyc_Std_fputs(_tag_arr("int",sizeof(unsigned char),
4),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return Cyc_slurp(lexbuf);
case 29: _LL6A: return Cyc_slurp(lexbuf);case 30: _LL6B: Cyc_parens_to_match=1;if(Cyc_asm(
lexbuf)){return 1;}Cyc_Std_fputs(_tag_arr("0",sizeof(unsigned char),2),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp43[0]={};Cyc_log(_tag_arr("Warning: replacing use of __asm__ with 0\n",
sizeof(unsigned char),42),_tag_arr(_tmp43,sizeof(void*),0));});return Cyc_slurp(
lexbuf);case 31: _LL6C: Cyc_Std_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct
Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return Cyc_slurp(lexbuf);default:
_LL6D:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp44=_cycalloc(sizeof(*_tmp44));
_tmp44[0]=({struct Cyc_Lexing_Error_struct _tmp45;_tmp45.tag=Cyc_Lexing_Error;
_tmp45.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp45;});
_tmp44;}));}int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(
lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6F: Cyc_Std_fputc((
int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 1:
_LL70: return 1;case 2: _LL71: return 1;case 3: _LL72:({struct Cyc_Std_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp46[1]={& _tmp47};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp46,sizeof(void*),1));}});
return Cyc_slurp_string(lexbuf);case 4: _LL73:({struct Cyc_Std_String_pa_struct
_tmp49;_tmp49.tag=0;_tmp49.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp48[1]={& _tmp49};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp48,sizeof(void*),1));}});
return Cyc_slurp_string(lexbuf);case 5: _LL74:({struct Cyc_Std_String_pa_struct
_tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4A[1]={& _tmp4B};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4A,sizeof(void*),1));}});
return Cyc_slurp_string(lexbuf);case 6: _LL75:({struct Cyc_Std_String_pa_struct
_tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4C[1]={& _tmp4D};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4C,sizeof(void*),1));}});
return Cyc_slurp_string(lexbuf);case 7: _LL76:({struct Cyc_Std_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4E[1]={& _tmp4F};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4E,sizeof(void*),1));}});
return Cyc_slurp_string(lexbuf);case 8: _LL77:({struct Cyc_Std_String_pa_struct
_tmp51;_tmp51.tag=0;_tmp51.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp50[1]={& _tmp51};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp50,sizeof(void*),1));}});
return Cyc_slurp_string(lexbuf);default: _LL78:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Lexing_Error_struct
_tmp53;_tmp53.tag=Cyc_Lexing_Error;_tmp53.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp53;});_tmp52;}));}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL7A: if(Cyc_parens_to_match == 1){return 0;}Cyc_parens_to_match --;return Cyc_asm(
lexbuf);case 1: _LL7B: Cyc_parens_to_match ++;return Cyc_asm(lexbuf);case 2: _LL7C: Cyc_asm_string(
lexbuf);return Cyc_asm(lexbuf);case 3: _LL7D: Cyc_asm_comment(lexbuf);return Cyc_asm(
lexbuf);case 4: _LL7E: return Cyc_asm(lexbuf);case 5: _LL7F: return 0;case 6: _LL80: return
Cyc_asm(lexbuf);default: _LL81:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp54=
_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Lexing_Error_struct _tmp55;
_tmp55.tag=Cyc_Lexing_Error;_tmp55.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp55;});_tmp54;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL83: return 0;case 1: _LL84: return 1;case 2: _LL85: return 1;case 3: _LL86: return
Cyc_asm_string(lexbuf);case 4: _LL87: return Cyc_asm_string(lexbuf);case 5: _LL88:
return Cyc_asm_string(lexbuf);case 6: _LL89: return Cyc_asm_string(lexbuf);case 7:
_LL8A: return Cyc_asm_string(lexbuf);case 8: _LL8B: return Cyc_asm_string(lexbuf);
default: _LL8C:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp56=_cycalloc(
sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Lexing_Error_struct _tmp57;_tmp57.tag=Cyc_Lexing_Error;
_tmp57.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp57;});
_tmp56;}));}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8E: return 0;
case 1: _LL8F: return 1;case 2: _LL90: return Cyc_asm_comment(lexbuf);default: _LL91:(
lexbuf->refill_buff)(lexbuf);return Cyc_asm_comment_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp58=_cycalloc(sizeof(*_tmp58));
_tmp58[0]=({struct Cyc_Lexing_Error_struct _tmp59;_tmp59.tag=Cyc_Lexing_Error;
_tmp59.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp59;});
_tmp58;}));}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}struct _tuple10*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL93:
Cyc_current_line=_tag_arr("#define ",sizeof(unsigned char),9);Cyc_suck_macroname(
lexbuf);return({struct _tuple10*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->f1=Cyc_current_line;
_tmp5A->f2=(struct _tagged_arr*)_check_null(Cyc_current_source);_tmp5A;});case 1:
_LL94: return Cyc_suck_line(lexbuf);case 2: _LL95: return 0;default: _LL96:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Lexing_Error_struct
_tmp5C;_tmp5C.tag=Cyc_Lexing_Error;_tmp5C.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp5C;});_tmp5B;}));}struct _tuple10*Cyc_suck_line(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL98: Cyc_current_source=({struct _tagged_arr*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp5D;});Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,*((
struct _tagged_arr*)_check_null(Cyc_current_source)));return Cyc_suck_restofline(
lexbuf);default: _LL99:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5E=
_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Lexing_Error_struct _tmp5F;
_tmp5F.tag=Cyc_Lexing_Error;_tmp5F.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp5F;});_tmp5E;}));}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL9B: Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(
Cyc_current_line,(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default:
_LL9C:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp60=_cycalloc(
sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Lexing_Error_struct _tmp61;_tmp61.tag=Cyc_Lexing_Error;
_tmp61.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp61;});
_tmp60;}));}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple12*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL9E:
Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=0;Cyc_current_cstubs=
0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=0;if(Cyc_commands(
lexbuf)){return 0;}Cyc_current_prologue=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);Cyc_current_cstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cycstubs);return({struct _tuple12*_tmp62=_cycalloc(sizeof(*_tmp62));
_tmp62->f1=Cyc_current_header;_tmp62->f2=Cyc_current_symbols;_tmp62->f3=Cyc_current_omit_symbols;
_tmp62->f4=Cyc_current_prologue;_tmp62->f5=Cyc_current_epilogue;_tmp62->f6=Cyc_current_cstubs;
_tmp62->f7=Cyc_current_cycstubs;_tmp62;});case 1: _LL9F: return Cyc_spec(lexbuf);
case 2: _LLA0: return Cyc_spec(lexbuf);case 3: _LLA1: return 0;case 4: _LLA2:({struct Cyc_Std_Int_pa_struct
_tmp64;_tmp64.tag=1;_tmp64.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp63[1]={& _tmp64};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof(unsigned char),67),_tag_arr(_tmp63,sizeof(void*),1));}});return 0;default:
_LLA3:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65[0]=({struct Cyc_Lexing_Error_struct _tmp66;_tmp66.tag=Cyc_Lexing_Error;
_tmp66.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp66;});
_tmp65;}));}struct _tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(
lexbuf,13);}int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLA5: Cyc_snarfed_symbols=
0;if(Cyc_snarfsymbols(lexbuf)){return 1;}Cyc_current_symbols=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,
Cyc_current_symbols);return Cyc_commands(lexbuf);case 1: _LLA6: Cyc_snarfed_symbols=
0;if(Cyc_snarfsymbols(lexbuf)){return 1;}Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,
Cyc_current_omit_symbols);return Cyc_commands(lexbuf);case 2: _LLA7: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){
return 1;}{struct _tuple11*x=({struct _tuple11*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68->f1=(struct _tagged_arr)_tag_arr(0,0,0);_tmp68->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp68;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=x;_tmp67->tl=
Cyc_current_prologue;_tmp67;});return Cyc_commands(lexbuf);}case 3: _LLA8: {struct
_tagged_arr _tmp69=Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp69,
sizeof(unsigned char),(int)Cyc_Std_strlen(_tag_arr("prologue",sizeof(
unsigned char),9)));while(isspace((int)*((unsigned char*)
_check_unknown_subscript(_tmp69,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp69,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp69;while(! isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp6A=Cyc_Std_substring((struct _tagged_arr)_tmp69,0,(unsigned int)((t.curr - 
_tmp69.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple11*x=({struct _tuple11*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->f1=(struct
_tagged_arr)_tmp6A;_tmp6C->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp6C;});Cyc_current_prologue=({struct Cyc_List_List*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->hd=x;_tmp6B->tl=Cyc_current_prologue;
_tmp6B;});return Cyc_commands(lexbuf);}}}}case 4: _LLA9: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{
struct _tuple11*x=({struct _tuple11*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp6E->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp6E;});Cyc_current_epilogue=({
struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=x;_tmp6D->tl=
Cyc_current_epilogue;_tmp6D;});return Cyc_commands(lexbuf);}case 5: _LLAA: {struct
_tagged_arr _tmp6F=Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp6F,
sizeof(unsigned char),(int)Cyc_Std_strlen(_tag_arr("epilogue",sizeof(
unsigned char),9)));while(isspace((int)*((unsigned char*)
_check_unknown_subscript(_tmp6F,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp6F,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp6F;while(! isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp70=Cyc_Std_substring((struct _tagged_arr)_tmp6F,0,(unsigned int)((t.curr - 
_tmp6F.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple11*x=({struct _tuple11*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->f1=(struct
_tagged_arr)_tmp70;_tmp72->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp72;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=x;_tmp71->tl=Cyc_current_epilogue;
_tmp71;});return Cyc_commands(lexbuf);}}}}case 6: _LLAB: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{
struct _tuple11*x=({struct _tuple11*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp74->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp74;});Cyc_current_cstubs=({
struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=x;_tmp73->tl=
Cyc_current_cstubs;_tmp73;});return Cyc_commands(lexbuf);}case 7: _LLAC: {struct
_tagged_arr _tmp75=Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp75,
sizeof(unsigned char),(int)Cyc_Std_strlen(_tag_arr("cstub",sizeof(unsigned char),
6)));while(isspace((int)*((unsigned char*)_check_unknown_subscript(_tmp75,
sizeof(unsigned char),0)))){_tagged_arr_inplace_plus_post(& _tmp75,sizeof(
unsigned char),1);}{struct _tagged_arr t=_tmp75;while(! isspace((int)*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp76=Cyc_Std_substring((struct _tagged_arr)_tmp75,0,(unsigned int)((t.curr - 
_tmp75.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple11*x=({struct _tuple11*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->f1=(struct
_tagged_arr)_tmp76;_tmp78->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp78;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->hd=x;_tmp77->tl=Cyc_current_cstubs;
_tmp77;});return Cyc_commands(lexbuf);}}}}case 8: _LLAD: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{
struct _tuple11*x=({struct _tuple11*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp7A->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp7A;});Cyc_current_cycstubs=({
struct Cyc_List_List*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->hd=x;_tmp79->tl=
Cyc_current_cycstubs;_tmp79;});return Cyc_commands(lexbuf);}case 9: _LLAE: {struct
_tagged_arr _tmp7B=Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp7B,
sizeof(unsigned char),(int)Cyc_Std_strlen(_tag_arr("cycstub",sizeof(
unsigned char),8)));while(isspace((int)*((unsigned char*)
_check_unknown_subscript(_tmp7B,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp7B,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp7B;while(! isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp7C=Cyc_Std_substring((struct _tagged_arr)_tmp7B,0,(unsigned int)((t.curr - 
_tmp7B.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{struct
_tuple11*x=({struct _tuple11*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->f1=(struct
_tagged_arr)_tmp7C;_tmp7E->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp7E;});Cyc_current_cycstubs=({struct Cyc_List_List*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=x;_tmp7D->tl=Cyc_current_cycstubs;
_tmp7D;});return Cyc_commands(lexbuf);}}}}case 10: _LLAF: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);if(Cyc_block(lexbuf)){return 1;}{
struct _tagged_arr*x=({struct _tagged_arr*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[
0]=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp80;});Cyc_current_cpp=({struct Cyc_List_List*_tmp7F=_cycalloc(sizeof(*_tmp7F));
_tmp7F->hd=x;_tmp7F->tl=Cyc_current_cpp;_tmp7F;});return Cyc_commands(lexbuf);}
case 11: _LLB0: return Cyc_commands(lexbuf);case 12: _LLB1: return Cyc_commands(lexbuf);
case 13: _LLB2: return 0;case 14: _LLB3: return 0;case 15: _LLB4:({struct Cyc_Std_Int_pa_struct
_tmp82;_tmp82.tag=1;_tmp82.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp81[1]={& _tmp82};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof(unsigned char),58),_tag_arr(_tmp81,sizeof(void*),1));}});return 1;default:
_LLB5:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp83=_cycalloc(sizeof(*
_tmp83));_tmp83[0]=({struct Cyc_Lexing_Error_struct _tmp84;_tmp84.tag=Cyc_Lexing_Error;
_tmp84.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp84;});
_tmp83;}));}int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(
lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB7: Cyc_snarfed_symbols=({
struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=({struct
_tagged_arr*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp86;});_tmp85->tl=Cyc_snarfed_symbols;_tmp85;});return Cyc_snarfsymbols(
lexbuf);case 1: _LLB8: return Cyc_snarfsymbols(lexbuf);case 2: _LLB9: return 0;case 3:
_LLBA:({void*_tmp87[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: unexpected end-of-file\n",
sizeof(unsigned char),44),_tag_arr(_tmp87,sizeof(void*),0));});return 1;case 4:
_LLBB:({struct Cyc_Std_Int_pa_struct _tmp89;_tmp89.tag=1;_tmp89.f1=(int)((
unsigned int)((int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp88[1]={& _tmp89};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof(unsigned char),57),_tag_arr(_tmp88,sizeof(void*),1));}});return 1;default:
_LLBC:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A[0]=({struct Cyc_Lexing_Error_struct _tmp8B;_tmp8B.tag=Cyc_Lexing_Error;
_tmp8B.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp8B;});
_tmp8A;}));}int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBE: if(Cyc_braces_to_match
== 1){return 0;}Cyc_braces_to_match --;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return Cyc_block(lexbuf);case 1: _LLBF: Cyc_braces_to_match
++;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return
Cyc_block(lexbuf);case 2: _LLC0: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'"');Cyc_block_string(lexbuf);return Cyc_block(lexbuf);
case 3: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
_tag_arr("/*",sizeof(unsigned char),3));Cyc_block_comment(lexbuf);return Cyc_block(
lexbuf);case 4: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return Cyc_block(lexbuf);case 5: _LLC3:
return 0;case 6: _LLC4: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
Cyc_Lexing_lexeme_char(lexbuf,0));return Cyc_block(lexbuf);default: _LLC5:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Lexing_Error_struct
_tmp8D;_tmp8D.tag=Cyc_Lexing_Error;_tmp8D.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp8D;});_tmp8C;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLC7: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 1: _LLC8: return 1;case 2: _LLC9: return 1;case 3: _LLCA: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 4: _LLCB: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 5: _LLCC: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 6: _LLCD: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 7: _LLCE: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);case 8: _LLCF: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return Cyc_block_string(lexbuf);default: _LLD0:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_Lexing_Error_struct
_tmp8F;_tmp8F.tag=Cyc_Lexing_Error;_tmp8F.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp8F;});_tmp8E;}));}int Cyc_block_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_string_rec(lexbuf,17);}int Cyc_block_comment_rec(struct
Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLD2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),_tag_arr("*/",sizeof(unsigned char),3));return 0;case 1:
_LLD3: return 1;case 2: _LLD4: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(
Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return Cyc_block_comment(
lexbuf);default: _LLD5:(lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp90=
_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Lexing_Error_struct _tmp91;
_tmp91.tag=Cyc_Lexing_Error;_tmp91.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp91;});_tmp90;}));}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_comment_rec(lexbuf,18);}static void Cyc_pr(struct
_tagged_arr*sptr){({struct Cyc_Std_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(
struct _tagged_arr)*sptr;{void*_tmp92[1]={& _tmp93};Cyc_Std_printf(_tag_arr(" %s\n",
sizeof(unsigned char),5),_tag_arr(_tmp92,sizeof(void*),1));}});}extern void Cyc_scan_type(
void*t);struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(
struct Cyc_Absyn_Exp*e){void*_tmp94=(void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;
struct _tuple0*_tmp95;struct _tuple0 _tmp96;struct _tagged_arr*_tmp97;struct _tuple0*
_tmp98;struct _tuple0 _tmp99;struct _tagged_arr*_tmp9A;struct Cyc_List_List*_tmp9B;
struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;
struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;
struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;
struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA7;
struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_List_List*_tmpA9;struct Cyc_Absyn_Exp*_tmpAA;
struct Cyc_List_List*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAD;
void*_tmpAE;struct Cyc_Absyn_MallocInfo _tmpAF;struct Cyc_Absyn_Exp*_tmpB0;void**
_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;int _tmpB3;void*_tmpB4;struct _tagged_arr*_tmpB5;
struct Cyc_Absyn_Exp*_tmpB6;struct _tagged_arr*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;
void*_tmpB9;void*_tmpBA;struct Cyc_List_List*_tmpBB;_LLD8: if(*((int*)_tmp94)== 1){
_LL124: _tmp95=((struct Cyc_Absyn_Var_e_struct*)_tmp94)->f1;_tmp96=*_tmp95;_LL125:
_tmp97=_tmp96.f2;goto _LLD9;}else{goto _LLDA;}_LLDA: if(*((int*)_tmp94)== 2){_LL126:
_tmp98=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp94)->f1;_tmp99=*_tmp98;_LL127:
_tmp9A=_tmp99.f2;goto _LLDB;}else{goto _LLDC;}_LLDC: if(*((int*)_tmp94)== 3){_LL128:
_tmp9B=((struct Cyc_Absyn_Primop_e_struct*)_tmp94)->f2;goto _LLDD;}else{goto _LLDE;}
_LLDE: if(*((int*)_tmp94)== 23){_LL12A: _tmp9D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp94)->f1;goto _LL129;_LL129: _tmp9C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp94)->f2;goto _LLDF;}else{goto _LLE0;}_LLE0: if(*((int*)_tmp94)== 7){_LL12C:
_tmp9F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp94)->f1;goto _LL12B;_LL12B: _tmp9E=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp94)->f2;goto _LLE1;}else{goto _LLE2;}_LLE2:
if(*((int*)_tmp94)== 4){_LL12E: _tmpA1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp94)->f1;goto _LL12D;_LL12D: _tmpA0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp94)->f3;
goto _LLE3;}else{goto _LLE4;}_LLE4: if(*((int*)_tmp94)== 20){_LL12F: _tmpA2=((struct
Cyc_Absyn_Deref_e_struct*)_tmp94)->f1;goto _LLE5;}else{goto _LLE6;}_LLE6: if(*((int*)
_tmp94)== 17){_LL130: _tmpA3=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp94)->f1;
goto _LLE7;}else{goto _LLE8;}_LLE8: if(*((int*)_tmp94)== 14){_LL131: _tmpA4=((struct
Cyc_Absyn_Address_e_struct*)_tmp94)->f1;goto _LLE9;}else{goto _LLEA;}_LLEA: if(*((
int*)_tmp94)== 5){_LL132: _tmpA5=((struct Cyc_Absyn_Increment_e_struct*)_tmp94)->f1;
goto _LLEB;}else{goto _LLEC;}_LLEC: if(*((int*)_tmp94)== 6){_LL135: _tmpA8=((struct
Cyc_Absyn_Conditional_e_struct*)_tmp94)->f1;goto _LL134;_LL134: _tmpA7=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp94)->f2;goto _LL133;_LL133: _tmpA6=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp94)->f3;goto _LLED;}else{goto _LLEE;}_LLEE: if(*((int*)_tmp94)== 9){_LL137:
_tmpAA=((struct Cyc_Absyn_FnCall_e_struct*)_tmp94)->f1;goto _LL136;_LL136: _tmpA9=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp94)->f2;goto _LLEF;}else{goto _LLF0;}_LLF0:
if(*((int*)_tmp94)== 8){_LL139: _tmpAC=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp94)->f1;goto _LL138;_LL138: _tmpAB=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp94)->f2;goto _LLF1;}else{goto _LLF2;}_LLF2: if(*((int*)_tmp94)== 13){_LL13B:
_tmpAE=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp94)->f1;goto _LL13A;_LL13A:
_tmpAD=((struct Cyc_Absyn_Cast_e_struct*)_tmp94)->f2;goto _LLF3;}else{goto _LLF4;}
_LLF4: if(*((int*)_tmp94)== 33){_LL13C: _tmpAF=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp94)->f1;_LL140: _tmpB3=_tmpAF.is_calloc;goto _LL13F;_LL13F: _tmpB2=_tmpAF.rgn;
goto _LL13E;_LL13E: _tmpB1=_tmpAF.elt_type;goto _LL13D;_LL13D: _tmpB0=_tmpAF.num_elts;
goto _LLF5;}else{goto _LLF6;}_LLF6: if(*((int*)_tmp94)== 16){_LL141: _tmpB4=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp94)->f1;goto _LLF7;}else{goto _LLF8;}_LLF8:
if(*((int*)_tmp94)== 21){_LL143: _tmpB6=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp94)->f1;goto _LL142;_LL142: _tmpB5=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp94)->f2;goto _LLF9;}else{goto _LLFA;}_LLFA: if(*((int*)_tmp94)== 22){_LL145:
_tmpB8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp94)->f1;goto _LL144;_LL144:
_tmpB7=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp94)->f2;goto _LLFB;}else{goto
_LLFC;}_LLFC: if(*((int*)_tmp94)== 18){_LL147: _tmpBA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp94)->f1;goto _LL146;_LL146: _tmpB9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp94)->f2;goto _LLFD;}else{goto _LLFE;}_LLFE: if(*((int*)_tmp94)== 0){goto _LLFF;}
else{goto _LL100;}_LL100: if(*((int*)_tmp94)== 34){_LL148: _tmpBB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp94)->f2;goto _LL101;}else{goto _LL102;}_LL102: if(*((int*)_tmp94)== 35){goto
_LL103;}else{goto _LL104;}_LL104: if(*((int*)_tmp94)== 10){goto _LL105;}else{goto
_LL106;}_LL106: if(*((int*)_tmp94)== 11){goto _LL107;}else{goto _LL108;}_LL108: if(*((
int*)_tmp94)== 12){goto _LL109;}else{goto _LL10A;}_LL10A: if(*((int*)_tmp94)== 15){
goto _LL10B;}else{goto _LL10C;}_LL10C: if(*((int*)_tmp94)== 19){goto _LL10D;}else{
goto _LL10E;}_LL10E: if(*((int*)_tmp94)== 24){goto _LL10F;}else{goto _LL110;}_LL110:
if(*((int*)_tmp94)== 25){goto _LL111;}else{goto _LL112;}_LL112: if(*((int*)_tmp94)
== 26){goto _LL113;}else{goto _LL114;}_LL114: if(*((int*)_tmp94)== 27){goto _LL115;}
else{goto _LL116;}_LL116: if(*((int*)_tmp94)== 28){goto _LL117;}else{goto _LL118;}
_LL118: if(*((int*)_tmp94)== 29){goto _LL119;}else{goto _LL11A;}_LL11A: if(*((int*)
_tmp94)== 30){goto _LL11B;}else{goto _LL11C;}_LL11C: if(*((int*)_tmp94)== 31){goto
_LL11D;}else{goto _LL11E;}_LL11E: if(*((int*)_tmp94)== 32){goto _LL11F;}else{goto
_LL120;}_LL120: if(*((int*)_tmp94)== 36){goto _LL121;}else{goto _LL122;}_LL122: if(*((
int*)_tmp94)== 37){goto _LL123;}else{goto _LLD7;}_LLD9: _tmp9A=_tmp97;goto _LLDB;
_LLDB: Cyc_add_target(_tmp9A);return;_LLDD: for(0;_tmp9B != 0;_tmp9B=_tmp9B->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmp9B->hd));}return;
_LLDF: _tmp9F=_tmp9D;_tmp9E=_tmp9C;goto _LLE1;_LLE1: _tmpA1=_tmp9F;_tmpA0=_tmp9E;
goto _LLE3;_LLE3: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpA1);Cyc_scan_exp((struct
Cyc_Absyn_Exp*)_tmpA0);return;_LLE5: _tmpA3=_tmpA2;goto _LLE7;_LLE7: _tmpA4=_tmpA3;
goto _LLE9;_LLE9: _tmpA5=_tmpA4;goto _LLEB;_LLEB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpA5);return;_LLED: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpA8);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpA7);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpA6);return;
_LLEF: _tmpAC=_tmpAA;_tmpAB=_tmpA9;goto _LLF1;_LLF1: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpAC);for(0;_tmpAB != 0;_tmpAB=_tmpAB->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpAB->hd));}return;_LLF3: Cyc_scan_type(_tmpAE);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpAD);return;_LLF5: if(_tmpB2 != 0){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmpB2)));}if(_tmpB1 != 0){Cyc_scan_type(*_tmpB1);}
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB0);return;_LLF7: Cyc_scan_type(_tmpB4);
return;_LLF9: _tmpB8=_tmpB6;_tmpB7=_tmpB5;goto _LLFB;_LLFB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpB8);Cyc_add_target(_tmpB7);return;_LLFD: Cyc_scan_type(_tmpBA);{void*_tmpBC=
_tmpB9;struct _tagged_arr*_tmpBD;_LL14A: if(*((int*)_tmpBC)== 0){_LL14E: _tmpBD=((
struct Cyc_Absyn_StructField_struct*)_tmpBC)->f1;goto _LL14B;}else{goto _LL14C;}
_LL14C: if(*((int*)_tmpBC)== 1){goto _LL14D;}else{goto _LL149;}_LL14B: Cyc_add_target(
_tmpBD);goto _LL149;_LL14D: goto _LL149;_LL149:;}return;_LLFF: return;_LL101: for(0;
_tmpBB != 0;_tmpBB=_tmpBB->tl){struct _tuple13 _tmpBF;struct Cyc_Absyn_Exp*_tmpC0;
struct _tuple13*_tmpBE=(struct _tuple13*)_tmpBB->hd;_tmpBF=*_tmpBE;_LL150: _tmpC0=
_tmpBF.f2;goto _LL14F;_LL14F: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC0);}return;
_LL103:({void*_tmpC1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Stmt_e\n",
sizeof(unsigned char),26),_tag_arr(_tmpC1,sizeof(void*),0));});exit(1);return;
_LL105:({void*_tmpC2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Throw_e\n",
sizeof(unsigned char),27),_tag_arr(_tmpC2,sizeof(void*),0));});exit(1);return;
_LL107:({void*_tmpC3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected NoInstantiate_e\n",
sizeof(unsigned char),35),_tag_arr(_tmpC3,sizeof(void*),0));});exit(1);return;
_LL109:({void*_tmpC4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Instantiate_e\n",
sizeof(unsigned char),33),_tag_arr(_tmpC4,sizeof(void*),0));});exit(1);return;
_LL10B:({void*_tmpC5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected New_e\n",
sizeof(unsigned char),25),_tag_arr(_tmpC5,sizeof(void*),0));});exit(1);return;
_LL10D:({void*_tmpC6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Gentyp_e\n",
sizeof(unsigned char),28),_tag_arr(_tmpC6,sizeof(void*),0));});exit(1);return;
_LL10F:({void*_tmpC7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Tuple_e\n",
sizeof(unsigned char),27),_tag_arr(_tmpC7,sizeof(void*),0));});exit(1);return;
_LL111:({void*_tmpC8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected CompoundLit_e\n",
sizeof(unsigned char),33),_tag_arr(_tmpC8,sizeof(void*),0));});exit(1);return;
_LL113:({void*_tmpC9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Array_e\n",
sizeof(unsigned char),27),_tag_arr(_tmpC9,sizeof(void*),0));});exit(1);return;
_LL115:({void*_tmpCA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Comprehension_e\n",
sizeof(unsigned char),35),_tag_arr(_tmpCA,sizeof(void*),0));});exit(1);return;
_LL117:({void*_tmpCB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Struct_e\n",
sizeof(unsigned char),28),_tag_arr(_tmpCB,sizeof(void*),0));});exit(1);return;
_LL119:({void*_tmpCC[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonStruct_e\n",
sizeof(unsigned char),32),_tag_arr(_tmpCC,sizeof(void*),0));});exit(1);return;
_LL11B:({void*_tmpCD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Tunion_e\n",
sizeof(unsigned char),28),_tag_arr(_tmpCD,sizeof(void*),0));});exit(1);return;
_LL11D:({void*_tmpCE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Enum_e\n",
sizeof(unsigned char),26),_tag_arr(_tmpCE,sizeof(void*),0));});exit(1);return;
_LL11F:({void*_tmpCF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonEnum_e\n",
sizeof(unsigned char),30),_tag_arr(_tmpCF,sizeof(void*),0));});exit(1);return;
_LL121:({void*_tmpD0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Codegen_e\n",
sizeof(unsigned char),29),_tag_arr(_tmpD0,sizeof(void*),0));});exit(1);return;
_LL123:({void*_tmpD1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Fill_e\n",
sizeof(unsigned char),26),_tag_arr(_tmpD1,sizeof(void*),0));});exit(1);return;
_LLD7:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((unsigned int)eo){Cyc_scan_exp(
eo);}return;}void Cyc_scan_type(void*t){void*_tmpD2=t;struct Cyc_Absyn_PtrInfo
_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;void*_tmpD5;struct Cyc_Absyn_FnInfo _tmpD6;
struct Cyc_List_List*_tmpD7;struct Cyc_Absyn_AggrInfo _tmpD8;void*_tmpD9;struct
_tuple0*_tmpDA;struct _tuple0 _tmpDB;struct _tagged_arr*_tmpDC;struct _tuple0*_tmpDD;
struct _tuple0 _tmpDE;struct _tagged_arr*_tmpDF;_LL152: if((int)_tmpD2 == 0){goto
_LL153;}else{goto _LL154;}_LL154: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 5: 0){
goto _LL155;}else{goto _LL156;}_LL156: if((int)_tmpD2 == 1){goto _LL157;}else{goto
_LL158;}_LL158: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 6: 0){goto _LL159;}
else{goto _LL15A;}_LL15A: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 4: 0){_LL180:
_tmpD3=((struct Cyc_Absyn_PointerType_struct*)_tmpD2)->f1;goto _LL15B;}else{goto
_LL15C;}_LL15C: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 7: 0){_LL182: _tmpD5=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD2)->f1;goto _LL181;_LL181: _tmpD4=((
struct Cyc_Absyn_ArrayType_struct*)_tmpD2)->f3;goto _LL15D;}else{goto _LL15E;}
_LL15E: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 8: 0){_LL183: _tmpD6=((struct
Cyc_Absyn_FnType_struct*)_tmpD2)->f1;goto _LL15F;}else{goto _LL160;}_LL160: if((
unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 11: 0){_LL184: _tmpD7=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpD2)->f2;goto _LL161;}else{goto _LL162;}_LL162: if((unsigned int)_tmpD2 > 3?*((
int*)_tmpD2)== 13: 0){goto _LL163;}else{goto _LL164;}_LL164: if((unsigned int)_tmpD2
> 3?*((int*)_tmpD2)== 10: 0){_LL185: _tmpD8=((struct Cyc_Absyn_AggrType_struct*)
_tmpD2)->f1;_LL186: _tmpD9=(void*)_tmpD8.aggr_info;goto _LL165;}else{goto _LL166;}
_LL166: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 12: 0){_LL187: _tmpDA=((struct
Cyc_Absyn_EnumType_struct*)_tmpD2)->f1;_tmpDB=*_tmpDA;_LL188: _tmpDC=_tmpDB.f2;
goto _LL167;}else{goto _LL168;}_LL168: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 
16: 0){_LL189: _tmpDD=((struct Cyc_Absyn_TypedefType_struct*)_tmpD2)->f1;_tmpDE=*
_tmpDD;_LL18A: _tmpDF=_tmpDE.f2;goto _LL169;}else{goto _LL16A;}_LL16A: if((
unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 0: 0){goto _LL16B;}else{goto _LL16C;}
_LL16C: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 1: 0){goto _LL16D;}else{goto
_LL16E;}_LL16E: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 2: 0){goto _LL16F;}
else{goto _LL170;}_LL170: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 3: 0){goto
_LL171;}else{goto _LL172;}_LL172: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 9: 0){
goto _LL173;}else{goto _LL174;}_LL174: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 
15: 0){goto _LL175;}else{goto _LL176;}_LL176: if((int)_tmpD2 == 2){goto _LL177;}else{
goto _LL178;}_LL178: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 17: 0){goto _LL179;}
else{goto _LL17A;}_LL17A: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 18: 0){goto
_LL17B;}else{goto _LL17C;}_LL17C: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 19:
0){goto _LL17D;}else{goto _LL17E;}_LL17E: if((unsigned int)_tmpD2 > 3?*((int*)_tmpD2)
== 14: 0){goto _LL17F;}else{goto _LL151;}_LL153: goto _LL155;_LL155: goto _LL157;_LL157:
goto _LL159;_LL159: return;_LL15B: Cyc_scan_type((void*)_tmpD3.elt_typ);return;
_LL15D: Cyc_scan_type(_tmpD5);Cyc_scan_exp_opt(_tmpD4);return;_LL15F: Cyc_scan_type((
void*)_tmpD6.ret_typ);{struct Cyc_List_List*_tmpE0=_tmpD6.args;for(0;_tmpE0 != 0;
_tmpE0=_tmpE0->tl){struct _tuple1 _tmpE2;void*_tmpE3;struct _tuple1*_tmpE1=(struct
_tuple1*)_tmpE0->hd;_tmpE2=*_tmpE1;_LL18C: _tmpE3=_tmpE2.f3;goto _LL18B;_LL18B: Cyc_scan_type(
_tmpE3);}}if(_tmpD6.cyc_varargs != 0){Cyc_scan_type((void*)(_tmpD6.cyc_varargs)->type);}
return;_LL161: for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){Cyc_scan_type((void*)((struct
Cyc_Absyn_Aggrfield*)_tmpD7->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)
_tmpD7->hd)->width);}return;_LL163: return;_LL165: {struct _tuple0*_tmpE5;struct
_tuple0 _tmpE6;struct _tagged_arr*_tmpE7;struct _tuple3 _tmpE4=Cyc_Absyn_aggr_kinded_name(
_tmpD9);_LL18E: _tmpE5=_tmpE4.f2;_tmpE6=*_tmpE5;_LL18F: _tmpE7=_tmpE6.f2;goto
_LL18D;_LL18D: _tmpDC=_tmpE7;goto _LL167;}_LL167: _tmpDF=_tmpDC;goto _LL169;_LL169:
Cyc_add_target(_tmpDF);return;_LL16B:({void*_tmpE8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Evar\n",sizeof(unsigned char),24),_tag_arr(_tmpE8,
sizeof(void*),0));});exit(1);return;_LL16D:({void*_tmpE9[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected VarType\n",sizeof(unsigned char),27),
_tag_arr(_tmpE9,sizeof(void*),0));});exit(1);return;_LL16F:({void*_tmpEA[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected TunionType\n",sizeof(
unsigned char),30),_tag_arr(_tmpEA,sizeof(void*),0));});exit(1);return;_LL171:({
void*_tmpEB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected TunionFieldType\n",
sizeof(unsigned char),35),_tag_arr(_tmpEB,sizeof(void*),0));});exit(1);return;
_LL173:({void*_tmpEC[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected TupleType\n",
sizeof(unsigned char),29),_tag_arr(_tmpEC,sizeof(void*),0));});exit(1);return;
_LL175:({void*_tmpED[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected RgnHandleType\n",
sizeof(unsigned char),33),_tag_arr(_tmpED,sizeof(void*),0));});exit(1);return;
_LL177:({void*_tmpEE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected HeapRgn\n",
sizeof(unsigned char),27),_tag_arr(_tmpEE,sizeof(void*),0));});exit(1);return;
_LL179:({void*_tmpEF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AccessEff\n",
sizeof(unsigned char),29),_tag_arr(_tmpEF,sizeof(void*),0));});exit(1);return;
_LL17B:({void*_tmpF0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected JoinEff\n",
sizeof(unsigned char),27),_tag_arr(_tmpF0,sizeof(void*),0));});exit(1);return;
_LL17D:({void*_tmpF1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected RgnsEff\n",
sizeof(unsigned char),27),_tag_arr(_tmpF1,sizeof(void*),0));});exit(1);return;
_LL17F:({void*_tmpF2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected sizeof_t\n",
sizeof(unsigned char),28),_tag_arr(_tmpF2,sizeof(void*),0));});exit(1);return;
_LL151:;}struct _tuple14{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct _tuple9*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct
Cyc_Set_Set**_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
_tmpF3;});{void*_tmpF4=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmpF5;struct Cyc_Absyn_Fndecl*
_tmpF6;struct Cyc_Absyn_Aggrdecl*_tmpF7;struct Cyc_Absyn_Enumdecl*_tmpF8;struct Cyc_Absyn_Typedefdecl*
_tmpF9;_LL191: if(*((int*)_tmpF4)== 0){_LL1A7: _tmpF5=((struct Cyc_Absyn_Var_d_struct*)
_tmpF4)->f1;goto _LL192;}else{goto _LL193;}_LL193: if(*((int*)_tmpF4)== 1){_LL1A8:
_tmpF6=((struct Cyc_Absyn_Fn_d_struct*)_tmpF4)->f1;goto _LL194;}else{goto _LL195;}
_LL195: if(*((int*)_tmpF4)== 4){_LL1A9: _tmpF7=((struct Cyc_Absyn_Aggr_d_struct*)
_tmpF4)->f1;goto _LL196;}else{goto _LL197;}_LL197: if(*((int*)_tmpF4)== 6){_LL1AA:
_tmpF8=((struct Cyc_Absyn_Enum_d_struct*)_tmpF4)->f1;goto _LL198;}else{goto _LL199;}
_LL199: if(*((int*)_tmpF4)== 7){_LL1AB: _tmpF9=((struct Cyc_Absyn_Typedef_d_struct*)
_tmpF4)->f1;goto _LL19A;}else{goto _LL19B;}_LL19B: if(*((int*)_tmpF4)== 2){goto
_LL19C;}else{goto _LL19D;}_LL19D: if(*((int*)_tmpF4)== 5){goto _LL19E;}else{goto
_LL19F;}_LL19F: if(*((int*)_tmpF4)== 3){goto _LL1A0;}else{goto _LL1A1;}_LL1A1: if(*((
int*)_tmpF4)== 8){goto _LL1A2;}else{goto _LL1A3;}_LL1A3: if(*((int*)_tmpF4)== 9){
goto _LL1A4;}else{goto _LL1A5;}_LL1A5: if(*((int*)_tmpF4)== 10){goto _LL1A6;}else{
goto _LL190;}_LL192: {struct _tuple0 _tmpFB;struct _tagged_arr*_tmpFC;struct _tuple0*
_tmpFA=_tmpF5->name;_tmpFB=*_tmpFA;_LL1AD: _tmpFC=_tmpFB.f2;goto _LL1AC;_LL1AC: Cyc_current_source=(
struct _tagged_arr*)_tmpFC;Cyc_scan_type((void*)_tmpF5->type);Cyc_scan_exp_opt(
_tmpF5->initializer);goto _LL190;}_LL194: {struct _tuple0 _tmpFE;struct _tagged_arr*
_tmpFF;struct _tuple0*_tmpFD=_tmpF6->name;_tmpFE=*_tmpFD;_LL1AF: _tmpFF=_tmpFE.f2;
goto _LL1AE;_LL1AE: Cyc_current_source=(struct _tagged_arr*)_tmpFF;Cyc_scan_type((
void*)_tmpF6->ret_type);{struct Cyc_List_List*_tmp100=_tmpF6->args;for(0;_tmp100
!= 0;_tmp100=_tmp100->tl){struct _tuple14 _tmp102;void*_tmp103;struct _tuple14*
_tmp101=(struct _tuple14*)_tmp100->hd;_tmp102=*_tmp101;_LL1B1: _tmp103=_tmp102.f3;
goto _LL1B0;_LL1B0: Cyc_scan_type(_tmp103);}}if(_tmpF6->cyc_varargs != 0){Cyc_scan_type((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpF6->cyc_varargs))->type);}
if(_tmpF6->is_inline){({void*_tmp104[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Warning: ignoring inline function\n",sizeof(unsigned char),35),
_tag_arr(_tmp104,sizeof(void*),0));});}goto _LL190;}_LL196: {struct _tuple0 _tmp106;
struct _tagged_arr*_tmp107;struct _tuple0*_tmp105=_tmpF7->name;_tmp106=*_tmp105;
_LL1B3: _tmp107=_tmp106.f2;goto _LL1B2;_LL1B2: Cyc_current_source=(struct
_tagged_arr*)_tmp107;if((unsigned int)_tmpF7->fields){{struct Cyc_List_List*
_tmp108=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpF7->fields))->v;
for(0;_tmp108 != 0;_tmp108=_tmp108->tl){struct Cyc_Absyn_Aggrfield*_tmp109=(struct
Cyc_Absyn_Aggrfield*)_tmp108->hd;Cyc_scan_type((void*)_tmp109->type);Cyc_scan_exp_opt(
_tmp109->width);}}{struct Cyc_List_List*_tmp10A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpF7->fields))->v;for(0;_tmp10A != 0;_tmp10A=_tmp10A->tl){;}}}goto
_LL190;}_LL198: {struct _tuple0 _tmp10C;struct _tagged_arr*_tmp10D;struct _tuple0*
_tmp10B=_tmpF8->name;_tmp10C=*_tmp10B;_LL1B5: _tmp10D=_tmp10C.f2;goto _LL1B4;
_LL1B4: Cyc_current_source=(struct _tagged_arr*)_tmp10D;if((unsigned int)_tmpF8->fields){{
struct Cyc_List_List*_tmp10E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpF8->fields))->v;for(0;_tmp10E != 0;_tmp10E=_tmp10E->tl){struct Cyc_Absyn_Enumfield*
_tmp10F=(struct Cyc_Absyn_Enumfield*)_tmp10E->hd;Cyc_scan_exp_opt(_tmp10F->tag);}}{
struct Cyc_List_List*_tmp110=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpF8->fields))->v;for(0;_tmp110 != 0;_tmp110=_tmp110->tl){;}}}goto
_LL190;}_LL19A: {struct _tuple0 _tmp112;struct _tagged_arr*_tmp113;struct _tuple0*
_tmp111=_tmpF9->name;_tmp112=*_tmp111;_LL1B7: _tmp113=_tmp112.f2;goto _LL1B6;
_LL1B6: Cyc_current_source=(struct _tagged_arr*)_tmp113;Cyc_scan_type((void*)
_tmpF9->defn);goto _LL190;}_LL19C:({void*_tmp114[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(unsigned char),35),
_tag_arr(_tmp114,sizeof(void*),0));});exit(1);return 0;_LL19E:({void*_tmp115[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected tunion declaration\n",
sizeof(unsigned char),38),_tag_arr(_tmp115,sizeof(void*),0));});exit(1);return 0;
_LL1A0:({void*_tmp116[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected let declaration\n",
sizeof(unsigned char),35),_tag_arr(_tmp116,sizeof(void*),0));});exit(1);return 0;
_LL1A2:({void*_tmp117[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected namespace declaration\n",
sizeof(unsigned char),41),_tag_arr(_tmp117,sizeof(void*),0));});exit(1);return 0;
_LL1A4:({void*_tmp118[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected using declaration\n",
sizeof(unsigned char),37),_tag_arr(_tmp118,sizeof(void*),0));});exit(1);return 0;
_LL1A6:({void*_tmp119[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected extern \"C\" declaration\n",
sizeof(unsigned char),42),_tag_arr(_tmp119,sizeof(void*),0));});exit(1);return 0;
_LL190:;}return({struct _tuple9*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->f1=(
struct _tagged_arr*)_check_null(Cyc_current_source);_tmp11A->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets));_tmp11A;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,
Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*x){struct _handler_cons _tmp11B;_push_handler(& _tmp11B);{int
_tmp11D=0;if(setjmp(_tmp11B.handler)){_tmp11D=1;}if(! _tmp11D){{struct Cyc_Set_Set*
_tmp11E=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*
key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp11E;};_pop_handler();}
else{void*_tmp11C=(void*)_exn_thrown;void*_tmp120=_tmp11C;_LL1B9: if(_tmp120 == 
Cyc_Core_Not_found){goto _LL1BA;}else{goto _LL1BB;}_LL1BB: goto _LL1BC;_LL1BA: return((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);_LL1BC:(void)_throw(_tmp120);_LL1B8:;}}}struct Cyc_Set_Set*Cyc_reachable(
struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
curr,(struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp121=curr;while(((int(*)(
struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp121)> 0){struct Cyc_Set_Set*_tmp122=
emptyset;{struct Cyc_List_List*_tmp123=((struct Cyc_List_List*(*)(struct Cyc_Set_Set*
s))Cyc_Set_elements)(_tmp121);for(0;_tmp123 != 0;_tmp123=_tmp123->tl){_tmp122=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp122,Cyc_find(t,(struct _tagged_arr*)_tmp123->hd));}}_tmp121=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp122,curr);curr=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
curr,_tmp121);}return curr;}}int Cyc_force_directory(struct _tagged_arr d){int
_tmp124=({unsigned int _tmp127[0];Cyc_Std_open(d,0,_tag_arr(_tmp127,sizeof(
unsigned int),0));});if(_tmp124 == - 1){if(Cyc_Std_mkdir(d,448)== - 1){({struct Cyc_Std_String_pa_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=(struct _tagged_arr)d;{void*_tmp125[1]={& _tmp126};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create directory %s\n",
sizeof(unsigned char),38),_tag_arr(_tmp125,sizeof(void*),1));}});return 1;}}else{
close(_tmp124);}return 0;}int Cyc_force_directory_prefixes(struct _tagged_arr file){
struct _tagged_arr _tmp128=Cyc_Std_strdup(file);struct Cyc_List_List*_tmp129=0;
while(1){_tmp128=Cyc_Filename_dirname((struct _tagged_arr)_tmp128);if(
_get_arr_size(_tmp128,sizeof(unsigned char))== 0){break;}_tmp129=({struct Cyc_List_List*
_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->hd=({struct _tagged_arr*_tmp12B=
_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=(struct _tagged_arr)_tmp128;_tmp12B;});
_tmp12A->tl=_tmp129;_tmp12A;});}for(0;_tmp129 != 0;_tmp129=_tmp129->tl){if(Cyc_force_directory(*((
struct _tagged_arr*)_tmp129->hd))){return 1;}}return 0;}int Cyc_process_file(struct
_tagged_arr filename,struct _tagged_arr dir,struct Cyc_List_List*start_symbols,
struct Cyc_List_List*omit_symbols,struct Cyc_List_List*prologue,struct Cyc_List_List*
epilogue,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc_Std___cycFILE*
maybe;struct Cyc_Std___cycFILE*in_file;struct Cyc_Std___cycFILE*out_file;({struct
Cyc_Std_String_pa_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct _tagged_arr)
filename;{void*_tmp12C[1]={& _tmp12D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("********************************* %s...\n",
sizeof(unsigned char),41),_tag_arr(_tmp12C,sizeof(void*),1));}});{struct
_tagged_arr buf=_tag_arr(({unsigned int _tmp1EB=(unsigned int)1024;unsigned char*
_tmp1EC=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),
_tmp1EB));{unsigned int _tmp1ED=_tmp1EB;unsigned int i;for(i=0;i < _tmp1ED;i ++){
_tmp1EC[i]='\000';}};_tmp1EC;}),sizeof(unsigned char),(unsigned int)1024);struct
_tagged_arr _tmp12E=Cyc_Std_getcwd(buf,_get_arr_size(buf,sizeof(unsigned char)));
if(Cyc_Std_chdir(dir)){({struct Cyc_Std_String_pa_struct _tmp130;_tmp130.tag=0;
_tmp130.f1=(struct _tagged_arr)dir;{void*_tmp12F[1]={& _tmp130};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: can't change directory to %s\n",sizeof(
unsigned char),37),_tag_arr(_tmp12F,sizeof(void*),1));}});return 1;}({struct Cyc_Std_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _tagged_arr)filename;{void*_tmp131[1]={&
_tmp132};Cyc_log(_tag_arr("\n%s:\n",sizeof(unsigned char),6),_tag_arr(_tmp131,
sizeof(void*),1));}});{struct _tagged_arr _tmp133=Cyc_Filename_basename(filename);
struct _tagged_arr _tmp134=Cyc_Filename_chop_extension((struct _tagged_arr)_tmp133);
struct _tagged_arr _tmp135=Cyc_Std_strconcat((struct _tagged_arr)_tmp134,_tag_arr(".iA",
sizeof(unsigned char),4));struct _tagged_arr _tmp136=Cyc_Std_strconcat((struct
_tagged_arr)_tmp134,_tag_arr(".i",sizeof(unsigned char),3));struct _tagged_arr
_tmp137=Cyc_Std_strconcat((struct _tagged_arr)_tmp134,_tag_arr(".iB",sizeof(
unsigned char),4));struct _tagged_arr _tmp138=Cyc_Filename_dirname(filename);
struct _tagged_arr _tmp139=_get_arr_size(_tmp138,sizeof(unsigned char))== 0?({
struct Cyc_Std_String_pa_struct _tmp1E8;_tmp1E8.tag=0;_tmp1E8.f1=(struct
_tagged_arr)_tmp133;{void*_tmp1E7[1]={& _tmp1E8};Cyc_Std_aprintf(_tag_arr("c%s",
sizeof(unsigned char),4),_tag_arr(_tmp1E7,sizeof(void*),1));}}): Cyc_Filename_concat((
struct _tagged_arr)_tmp138,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _tagged_arr)_tmp133;{void*_tmp1E9[1]={&
_tmp1EA};Cyc_Std_aprintf(_tag_arr("c%s",sizeof(unsigned char),4),_tag_arr(
_tmp1E9,sizeof(void*),1));}}));if(Cyc_force_directory_prefixes(filename)){return
1;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();struct _tagged_arr _tmp13A=
Cstring_to_string(Ccomp);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp135,_tag_arr("w",
sizeof(unsigned char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct _tagged_arr)_tmp135;{void*_tmp13B[1]={&
_tmp13C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(unsigned char),33),_tag_arr(_tmp13B,sizeof(void*),1));}});return 1;}
out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);Cyc_current_cpp=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);{
struct Cyc_List_List*_tmp13D=Cyc_current_cpp;for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
Cyc_Std_fputs(*((struct _tagged_arr*)_tmp13D->hd),out_file);}}({struct Cyc_Std_String_pa_struct
_tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct _tagged_arr)filename;{void*_tmp13E[1]={&
_tmp13F};Cyc_Std_fprintf(out_file,_tag_arr("#include <%s>\n",sizeof(
unsigned char),15),_tag_arr(_tmp13E,sizeof(void*),1));}});Cyc_Std_fclose(
out_file);{struct _tagged_arr _tmp140=({struct Cyc_Std_String_pa_struct _tmp1E6;
_tmp1E6.tag=0;_tmp1E6.f1=(struct _tagged_arr)_tmp135;{struct Cyc_Std_String_pa_struct
_tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(struct _tagged_arr)_tmp136;{struct Cyc_Std_String_pa_struct
_tmp1E4;_tmp1E4.tag=0;_tmp1E4.f1=(struct _tagged_arr)_tmp13A;{void*_tmp1E3[3]={&
_tmp1E4,& _tmp1E5,& _tmp1E6};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",
sizeof(unsigned char),24),_tag_arr(_tmp1E3,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp140)!= 0){Cyc_Std_remove((struct _tagged_arr)_tmp135);maybe=
Cyc_Std_fopen(filename,_tag_arr("w",sizeof(unsigned char),2));if(!((unsigned int)
maybe)){({struct Cyc_Std_String_pa_struct _tmp142;_tmp142.tag=0;_tmp142.f1=(struct
_tagged_arr)filename;{void*_tmp141[1]={& _tmp142};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),33),_tag_arr(
_tmp141,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp144;_tmp144.tag=0;_tmp144.f1=(
struct _tagged_arr)filename;{void*_tmp143[1]={& _tmp144};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(unsigned char),
48),_tag_arr(_tmp143,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp139,_tag_arr("w",sizeof(unsigned char),2));if(!((
unsigned int)maybe)){({struct Cyc_Std_String_pa_struct _tmp146;_tmp146.tag=0;
_tmp146.f1=(struct _tagged_arr)_tmp139;{void*_tmp145[1]={& _tmp146};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),
33),_tag_arr(_tmp145,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(
struct _tagged_arr)_tmp139;{void*_tmp147[1]={& _tmp148};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(unsigned char),
48),_tag_arr(_tmp147,sizeof(void*),1));}});Cyc_Std_fclose(out_file);({struct Cyc_Std_String_pa_struct
_tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _tagged_arr)filename;{void*_tmp149[1]={&
_tmp14A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: %s will not be supported on this platform\n",
sizeof(unsigned char),52),_tag_arr(_tmp149,sizeof(void*),1));}});({void*_tmp14B[
0]={};Cyc_log(_tag_arr("Not supported on this platform\n",sizeof(unsigned char),
32),_tag_arr(_tmp14B,sizeof(void*),0));});if(Cyc_Std_chdir((struct _tagged_arr)
_tmp12E)){({struct Cyc_Std_String_pa_struct _tmp14D;_tmp14D.tag=0;_tmp14D.f1=(
struct _tagged_arr)_tmp12E;{void*_tmp14C[1]={& _tmp14D};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n",sizeof(unsigned char),41),
_tag_arr(_tmp14C,sizeof(void*),1));}});return 1;}return 0;}maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp136,_tag_arr("r",sizeof(unsigned char),2));if(!((
unsigned int)maybe)){return 1;}in_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);{struct Cyc_Lexing_lexbuf*_tmp14E=Cyc_Lexing_from_file(in_file);struct
_tuple9*entry;while((entry=((struct _tuple9*(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_line)(_tmp14E))!= 0){struct _tuple9 _tmp150;struct Cyc_Set_Set*_tmp151;struct
_tagged_arr*_tmp152;struct _tuple9*_tmp14F=(struct _tuple9*)_check_null(entry);
_tmp150=*_tmp14F;_LL1BF: _tmp152=_tmp150.f1;goto _LL1BE;_LL1BE: _tmp151=_tmp150.f2;
goto _LL1BD;_LL1BD:((void(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*key,
struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp152,_tmp151);}Cyc_Std_fclose(
in_file);_tmp140=({struct Cyc_Std_String_pa_struct _tmp156;_tmp156.tag=0;_tmp156.f1=(
struct _tagged_arr)_tmp135;{struct Cyc_Std_String_pa_struct _tmp155;_tmp155.tag=0;
_tmp155.f1=(struct _tagged_arr)_tmp136;{struct Cyc_Std_String_pa_struct _tmp154;
_tmp154.tag=0;_tmp154.f1=(struct _tagged_arr)_tmp13A;{void*_tmp153[3]={& _tmp154,&
_tmp155,& _tmp156};Cyc_Std_aprintf(_tag_arr("%s -E -o %s -x c %s",sizeof(
unsigned char),20),_tag_arr(_tmp153,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp140)!= 0){return 1;}maybe=Cyc_Std_fopen((struct _tagged_arr)
_tmp136,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)maybe)){return
1;}in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);_tmp14E=Cyc_Lexing_from_file(
in_file);Cyc_slurp_out=Cyc_Std_fopen((struct _tagged_arr)_tmp137,_tag_arr("w",
sizeof(unsigned char),2));if(!((unsigned int)Cyc_slurp_out)){return 1;}if(((int(*)(
struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp14E)){({struct Cyc_Std_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)filename;{void*_tmp157[1]={&
_tmp158};Cyc_log(_tag_arr("Error: Unclosed string in %s\n",sizeof(unsigned char),
30),_tag_arr(_tmp157,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _tagged_arr)filename;{void*_tmp159[1]={&
_tmp15A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: Unclosed string in %s\n",
sizeof(unsigned char),30),_tag_arr(_tmp159,sizeof(void*),1));}});Cyc_Std_fclose(
in_file);Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
Cyc_Std_remove((struct _tagged_arr)_tmp135);Cyc_Std_remove((struct _tagged_arr)
_tmp136);Cyc_Std_remove((struct _tagged_arr)_tmp137);return 1;}Cyc_Std_fclose(
in_file);Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp137,_tag_arr("r",sizeof(unsigned char),
2));if(!((unsigned int)maybe)){return 1;}in_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);Cyc_Position_reset_position((struct _tagged_arr)_tmp137);Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp15B=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(
in_file);Cyc_Std_remove((struct _tagged_arr)_tmp137);{struct Cyc_List_List*_tmp15C=
_tmp15B;for(0;_tmp15C != 0;_tmp15C=_tmp15C->tl){struct _tuple9*_tmp15D=Cyc_scan_decl((
struct Cyc_Absyn_Decl*)_tmp15C->hd);if(_tmp15D == 0){continue;}{struct _tuple9
_tmp15F;struct Cyc_Set_Set*_tmp160;struct _tagged_arr*_tmp161;struct _tuple9*
_tmp15E=(struct _tuple9*)_check_null(_tmp15D);_tmp15F=*_tmp15E;_LL1C2: _tmp161=
_tmp15F.f1;goto _LL1C1;_LL1C1: _tmp160=_tmp15F.f2;goto _LL1C0;_LL1C0: {struct Cyc_Set_Set*
old;{struct _handler_cons _tmp162;_push_handler(& _tmp162);{int _tmp164=0;if(setjmp(
_tmp162.handler)){_tmp164=1;}if(! _tmp164){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(t,_tmp161);;_pop_handler();}else{
void*_tmp163=(void*)_exn_thrown;void*_tmp166=_tmp163;_LL1C4: if(_tmp166 == Cyc_Core_Not_found){
goto _LL1C5;}else{goto _LL1C6;}_LL1C6: goto _LL1C7;_LL1C5: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL1C3;_LL1C7:(void)_throw(_tmp166);_LL1C3:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp161,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp160,old));}}}}{struct Cyc_Set_Set*_tmp167=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp168=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp169=_tmp15B;for(0;_tmp169 != 0;_tmp169=_tmp169->tl){
struct Cyc_Absyn_Decl*_tmp16A=(struct Cyc_Absyn_Decl*)_tmp169->hd;struct
_tagged_arr*name;{void*_tmp16B=(void*)_tmp16A->r;struct Cyc_Absyn_Vardecl*_tmp16C;
struct Cyc_Absyn_Fndecl*_tmp16D;struct Cyc_Absyn_Aggrdecl*_tmp16E;struct Cyc_Absyn_Enumdecl*
_tmp16F;struct Cyc_Absyn_Typedefdecl*_tmp170;_LL1C9: if(*((int*)_tmp16B)== 0){
_LL1DF: _tmp16C=((struct Cyc_Absyn_Var_d_struct*)_tmp16B)->f1;goto _LL1CA;}else{
goto _LL1CB;}_LL1CB: if(*((int*)_tmp16B)== 1){_LL1E0: _tmp16D=((struct Cyc_Absyn_Fn_d_struct*)
_tmp16B)->f1;goto _LL1CC;}else{goto _LL1CD;}_LL1CD: if(*((int*)_tmp16B)== 4){_LL1E1:
_tmp16E=((struct Cyc_Absyn_Aggr_d_struct*)_tmp16B)->f1;goto _LL1CE;}else{goto
_LL1CF;}_LL1CF: if(*((int*)_tmp16B)== 6){_LL1E2: _tmp16F=((struct Cyc_Absyn_Enum_d_struct*)
_tmp16B)->f1;goto _LL1D0;}else{goto _LL1D1;}_LL1D1: if(*((int*)_tmp16B)== 7){_LL1E3:
_tmp170=((struct Cyc_Absyn_Typedef_d_struct*)_tmp16B)->f1;goto _LL1D2;}else{goto
_LL1D3;}_LL1D3: if(*((int*)_tmp16B)== 2){goto _LL1D4;}else{goto _LL1D5;}_LL1D5: if(*((
int*)_tmp16B)== 5){goto _LL1D6;}else{goto _LL1D7;}_LL1D7: if(*((int*)_tmp16B)== 3){
goto _LL1D8;}else{goto _LL1D9;}_LL1D9: if(*((int*)_tmp16B)== 8){goto _LL1DA;}else{
goto _LL1DB;}_LL1DB: if(*((int*)_tmp16B)== 9){goto _LL1DC;}else{goto _LL1DD;}_LL1DD:
if(*((int*)_tmp16B)== 10){goto _LL1DE;}else{goto _LL1C8;}_LL1CA: {struct _tuple0
_tmp172;struct _tagged_arr*_tmp173;struct _tuple0*_tmp171=_tmp16C->name;_tmp172=*
_tmp171;_LL1E5: _tmp173=_tmp172.f2;goto _LL1E4;_LL1E4: defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp173);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,
omit_symbols,_tmp173)){name=0;}else{name=(struct _tagged_arr*)_tmp173;}goto _LL1C8;}
_LL1CC: {struct _tuple0 _tmp175;struct _tagged_arr*_tmp176;struct _tuple0*_tmp174=
_tmp16D->name;_tmp175=*_tmp174;_LL1E7: _tmp176=_tmp175.f2;goto _LL1E6;_LL1E6:
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp176);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp176)){name=0;}else{name=(struct _tagged_arr*)
_tmp176;}goto _LL1C8;}_LL1CE: {struct _tuple0 _tmp178;struct _tagged_arr*_tmp179;
struct _tuple0*_tmp177=_tmp16E->name;_tmp178=*_tmp177;_LL1E9: _tmp179=_tmp178.f2;
goto _LL1E8;_LL1E8: name=(struct _tagged_arr*)_tmp179;goto _LL1C8;}_LL1D0: {struct
_tuple0 _tmp17B;struct _tagged_arr*_tmp17C;struct _tuple0*_tmp17A=_tmp16F->name;
_tmp17B=*_tmp17A;_LL1EB: _tmp17C=_tmp17B.f2;goto _LL1EA;_LL1EA: name=(struct
_tagged_arr*)_tmp17C;if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_member)(_tmp167,(struct _tagged_arr*)_check_null(name)): 0){_tmp168=({
struct Cyc_List_List*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->hd=_tmp16A;
_tmp17D->tl=_tmp168;_tmp17D;});}else{if((unsigned int)_tmp16F->fields){struct Cyc_List_List*
_tmp17E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp16F->fields))->v;
for(0;_tmp17E != 0;_tmp17E=_tmp17E->tl){struct Cyc_Absyn_Enumfield*_tmp17F=(struct
Cyc_Absyn_Enumfield*)_tmp17E->hd;struct _tuple0 _tmp181;struct _tagged_arr*_tmp182;
struct _tuple0*_tmp180=_tmp17F->name;_tmp181=*_tmp180;_LL1ED: _tmp182=_tmp181.f2;
goto _LL1EC;_LL1EC: if(((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp167,_tmp182)){_tmp168=({struct Cyc_List_List*_tmp183=_cycalloc(sizeof(*
_tmp183));_tmp183->hd=_tmp16A;_tmp183->tl=_tmp168;_tmp183;});break;}}}}name=0;
goto _LL1C8;}_LL1D2: {struct _tuple0 _tmp185;struct _tagged_arr*_tmp186;struct
_tuple0*_tmp184=_tmp170->name;_tmp185=*_tmp184;_LL1EF: _tmp186=_tmp185.f2;goto
_LL1EE;_LL1EE: name=(struct _tagged_arr*)_tmp186;goto _LL1C8;}_LL1D4: goto _LL1D6;
_LL1D6: goto _LL1D8;_LL1D8: goto _LL1DA;_LL1DA: goto _LL1DC;_LL1DC: goto _LL1DE;_LL1DE:
name=0;goto _LL1C8;_LL1C8:;}if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(_tmp167,(struct _tagged_arr*)_check_null(name)):
0){_tmp168=({struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->hd=
_tmp16A;_tmp187->tl=_tmp168;_tmp187;});}}}_tmp168=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp168);maybe=Cyc_Std_fopen(filename,
_tag_arr("w",sizeof(unsigned char),2));if(!((unsigned int)maybe)){return 1;}
out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);({struct Cyc_Std_String_pa_struct
_tmp189;_tmp189.tag=0;_tmp189.f1=(struct _tagged_arr)_tmp139;{void*_tmp188[1]={&
_tmp189};Cyc_Std_fprintf(out_file,_tag_arr("#include <%s>\nusing Std;\n",sizeof(
unsigned char),26),_tag_arr(_tmp188,sizeof(void*),1));}});Cyc_Std_fclose(
out_file);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp139,_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)maybe)){return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);{struct _tagged_arr ifdefmacro=({struct Cyc_Std_String_pa_struct
_tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=(struct _tagged_arr)filename;{void*_tmp1E1[1]={&
_tmp1E2};Cyc_Std_aprintf(_tag_arr("_%s_",sizeof(unsigned char),5),_tag_arr(
_tmp1E1,sizeof(void*),1));}});{int _tmp18A=0;for(0;_tmp18A < _get_arr_size(
ifdefmacro,sizeof(unsigned char));_tmp18A ++){if(((unsigned char*)ifdefmacro.curr)[
_tmp18A]== '.'? 1:((unsigned char*)ifdefmacro.curr)[_tmp18A]== '/'){((
unsigned char*)ifdefmacro.curr)[_tmp18A]='_';}else{if(((unsigned char*)
ifdefmacro.curr)[_tmp18A]!= '_'?((unsigned char*)ifdefmacro.curr)[_tmp18A]!= '/':
0){((unsigned char*)ifdefmacro.curr)[_tmp18A]=(unsigned char)toupper((int)((
unsigned char*)ifdefmacro.curr)[_tmp18A]);}}}}({struct Cyc_Std_String_pa_struct
_tmp18D;_tmp18D.tag=0;_tmp18D.f1=(struct _tagged_arr)ifdefmacro;{struct Cyc_Std_String_pa_struct
_tmp18C;_tmp18C.tag=0;_tmp18C.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp18B[2]={&
_tmp18C,& _tmp18D};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n#define %s\n",
sizeof(unsigned char),23),_tag_arr(_tmp18B,sizeof(void*),2));}}});if(prologue != 
0){struct Cyc_List_List*_tmp18E=prologue;for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){
struct _tuple11 _tmp190;struct _tagged_arr _tmp191;struct _tagged_arr _tmp192;struct
_tuple11*_tmp18F=(struct _tuple11*)_tmp18E->hd;_tmp190=*_tmp18F;_LL1F2: _tmp192=
_tmp190.f1;goto _LL1F1;_LL1F1: _tmp191=_tmp190.f2;goto _LL1F0;_LL1F0: if(_tmp191.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr? _tmp192.curr == ((struct _tagged_arr)
_tag_arr(0,0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193[
0]=_tmp192;_tmp193;})): 0){Cyc_Std_fputs(_tmp191,out_file);}else{({struct Cyc_Std_String_pa_struct
_tmp195;_tmp195.tag=0;_tmp195.f1=(struct _tagged_arr)_tmp192;{void*_tmp194[1]={&
_tmp195};Cyc_log(_tag_arr("%s is not supported on this platform\n",sizeof(
unsigned char),38),_tag_arr(_tmp194,sizeof(void*),1));}});}}}({void*_tmp196[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("\nnamespace Std {\n",sizeof(unsigned char),18),
_tag_arr(_tmp196,sizeof(void*),0));});{struct Cyc_List_List*_tmp197=_tmp168;for(0;
_tmp197 != 0;_tmp197=_tmp197->tl){struct Cyc_Absyn_Decl*_tmp198=(struct Cyc_Absyn_Decl*)
_tmp197->hd;int _tmp199=0;struct _tagged_arr*name;{void*_tmp19A=(void*)_tmp198->r;
struct Cyc_Absyn_Vardecl*_tmp19B;struct Cyc_Absyn_Fndecl*_tmp19C;struct Cyc_Absyn_Aggrdecl*
_tmp19D;struct Cyc_Absyn_Enumdecl*_tmp19E;struct Cyc_Absyn_Typedefdecl*_tmp19F;
_LL1F4: if(*((int*)_tmp19A)== 0){_LL20A: _tmp19B=((struct Cyc_Absyn_Var_d_struct*)
_tmp19A)->f1;goto _LL1F5;}else{goto _LL1F6;}_LL1F6: if(*((int*)_tmp19A)== 1){_LL20B:
_tmp19C=((struct Cyc_Absyn_Fn_d_struct*)_tmp19A)->f1;goto _LL1F7;}else{goto _LL1F8;}
_LL1F8: if(*((int*)_tmp19A)== 4){_LL20C: _tmp19D=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp19A)->f1;goto _LL1F9;}else{goto _LL1FA;}_LL1FA: if(*((int*)_tmp19A)== 6){_LL20D:
_tmp19E=((struct Cyc_Absyn_Enum_d_struct*)_tmp19A)->f1;goto _LL1FB;}else{goto
_LL1FC;}_LL1FC: if(*((int*)_tmp19A)== 7){_LL20E: _tmp19F=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp19A)->f1;goto _LL1FD;}else{goto _LL1FE;}_LL1FE: if(*((int*)_tmp19A)== 2){goto
_LL1FF;}else{goto _LL200;}_LL200: if(*((int*)_tmp19A)== 5){goto _LL201;}else{goto
_LL202;}_LL202: if(*((int*)_tmp19A)== 3){goto _LL203;}else{goto _LL204;}_LL204: if(*((
int*)_tmp19A)== 8){goto _LL205;}else{goto _LL206;}_LL206: if(*((int*)_tmp19A)== 9){
goto _LL207;}else{goto _LL208;}_LL208: if(*((int*)_tmp19A)== 10){goto _LL209;}else{
goto _LL1F3;}_LL1F5: {struct _tuple0 _tmp1A1;struct _tagged_arr*_tmp1A2;struct
_tuple0*_tmp1A0=_tmp19B->name;_tmp1A1=*_tmp1A0;_LL210: _tmp1A2=_tmp1A1.f2;goto
_LL20F;_LL20F: name=(struct _tagged_arr*)_tmp1A2;goto _LL1F3;}_LL1F7: {struct
_tuple0 _tmp1A4;struct _tagged_arr*_tmp1A5;struct _tuple0*_tmp1A3=_tmp19C->name;
_tmp1A4=*_tmp1A3;_LL212: _tmp1A5=_tmp1A4.f2;goto _LL211;_LL211: name=(struct
_tagged_arr*)_tmp1A5;goto _LL1F3;}_LL1F9: {struct _tuple0 _tmp1A7;struct _tagged_arr*
_tmp1A8;struct _tuple0*_tmp1A6=_tmp19D->name;_tmp1A7=*_tmp1A6;_LL214: _tmp1A8=
_tmp1A7.f2;goto _LL213;_LL213: name=(struct _tagged_arr*)_tmp1A8;goto _LL1F3;}_LL1FB: {
struct _tuple0 _tmp1AA;struct _tagged_arr*_tmp1AB;struct _tuple0*_tmp1A9=_tmp19E->name;
_tmp1AA=*_tmp1A9;_LL216: _tmp1AB=_tmp1AA.f2;goto _LL215;_LL215: name=(struct
_tagged_arr*)_tmp1AB;goto _LL1F3;}_LL1FD: {struct _tuple0 _tmp1AD;struct _tagged_arr*
_tmp1AE;struct _tuple0*_tmp1AC=_tmp19F->name;_tmp1AD=*_tmp1AC;_LL218: _tmp1AE=
_tmp1AD.f2;goto _LL217;_LL217: name=(struct _tagged_arr*)_tmp1AE;goto _LL1F3;}_LL1FF:
goto _LL201;_LL201: goto _LL203;_LL203: goto _LL205;_LL205: goto _LL207;_LL207: goto
_LL209;_LL209: name=0;goto _LL1F3;_LL1F3:;}if(!((unsigned int)name)? ! _tmp199: 0){
continue;}if((unsigned int)name){ifdefmacro=({struct Cyc_Std_String_pa_struct
_tmp1B0;_tmp1B0.tag=0;_tmp1B0.f1=(struct _tagged_arr)*name;{void*_tmp1AF[1]={&
_tmp1B0};Cyc_Std_aprintf(_tag_arr("_%s_def_",sizeof(unsigned char),9),_tag_arr(
_tmp1AF,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp1B2;_tmp1B2.tag=
0;_tmp1B2.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1B1[1]={& _tmp1B2};Cyc_Std_fprintf(
out_file,_tag_arr("#ifndef %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1B1,
sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct _tmp1B4;_tmp1B4.tag=0;
_tmp1B4.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp1B3[1]={& _tmp1B4};Cyc_Std_fprintf(
out_file,_tag_arr("#define %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1B3,
sizeof(void*),1));}});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp1B5[1];
_tmp1B5[0]=_tmp198;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1B5,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp1B6[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(
_tmp1B6,sizeof(void*),0));});}else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp1B7[1];_tmp1B7[0]=_tmp198;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1B7,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}_tmp140=({
struct Cyc_Std_String_pa_struct _tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=(struct
_tagged_arr)_tmp135;{struct Cyc_Std_String_pa_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(
struct _tagged_arr)_tmp136;{struct Cyc_Std_String_pa_struct _tmp1B9;_tmp1B9.tag=0;
_tmp1B9.f1=(struct _tagged_arr)_tmp13A;{void*_tmp1B8[3]={& _tmp1B9,& _tmp1BA,&
_tmp1BB};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",sizeof(unsigned char),
24),_tag_arr(_tmp1B8,sizeof(void*),3));}}}});if(Cyc_Std_system((struct
_tagged_arr)_tmp140)!= 0){return 1;}maybe=Cyc_Std_fopen((struct _tagged_arr)
_tmp136,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)maybe)){return
1;}in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);_tmp14E=Cyc_Lexing_from_file(
in_file);{struct _tuple10*entry2;while((entry2=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp14E))!= 0){struct _tuple10 _tmp1BD;struct _tagged_arr*
_tmp1BE;struct _tagged_arr _tmp1BF;struct _tuple10*_tmp1BC=(struct _tuple10*)
_check_null(entry2);_tmp1BD=*_tmp1BC;_LL21B: _tmp1BF=_tmp1BD.f1;goto _LL21A;_LL21A:
_tmp1BE=_tmp1BD.f2;goto _LL219;_LL219: if(((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(_tmp167,_tmp1BE)){({struct Cyc_Std_String_pa_struct
_tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=(struct _tagged_arr)*_tmp1BE;{void*_tmp1C0[1]={&
_tmp1C1};Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n",sizeof(unsigned char),
12),_tag_arr(_tmp1C0,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=(struct _tagged_arr)_tmp1BF;{void*_tmp1C2[1]={&
_tmp1C3};Cyc_Std_fprintf(out_file,_tag_arr("%s\n",sizeof(unsigned char),4),
_tag_arr(_tmp1C2,sizeof(void*),1));}});({void*_tmp1C4[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(_tmp1C4,sizeof(
void*),0));});}}Cyc_Std_fclose(in_file);Cyc_Std_remove((struct _tagged_arr)
_tmp135);Cyc_Std_remove((struct _tagged_arr)_tmp136);if(epilogue != 0){struct Cyc_List_List*
_tmp1C5=epilogue;for(0;_tmp1C5 != 0;_tmp1C5=_tmp1C5->tl){struct _tuple11 _tmp1C7;
struct _tagged_arr _tmp1C8;struct _tagged_arr _tmp1C9;struct _tuple11*_tmp1C6=(struct
_tuple11*)_tmp1C5->hd;_tmp1C7=*_tmp1C6;_LL21E: _tmp1C9=_tmp1C7.f1;goto _LL21D;
_LL21D: _tmp1C8=_tmp1C7.f2;goto _LL21C;_LL21C: if(_tmp1C8.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr? _tmp1C9.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[
0]=_tmp1C9;_tmp1CA;})): 0){Cyc_Std_fputs(_tmp1C8,out_file);}else{({struct Cyc_Std_String_pa_struct
_tmp1CC;_tmp1CC.tag=0;_tmp1CC.f1=(struct _tagged_arr)_tmp1C9;{void*_tmp1CB[1]={&
_tmp1CC};Cyc_log(_tag_arr("%s is not supported on this platform\n",sizeof(
unsigned char),38),_tag_arr(_tmp1CB,sizeof(void*),1));}});}}}({void*_tmp1CD[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("}\n",sizeof(unsigned char),3),_tag_arr(
_tmp1CD,sizeof(void*),0));});({void*_tmp1CE[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(_tmp1CE,sizeof(void*),0));});
Cyc_Std_fclose(out_file);if(cstubs != 0){out_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_cstubs_file);{struct Cyc_List_List*_tmp1CF=cstubs;for(0;_tmp1CF != 
0;_tmp1CF=_tmp1CF->tl){struct _tuple11 _tmp1D1;struct _tagged_arr _tmp1D2;struct
_tagged_arr _tmp1D3;struct _tuple11*_tmp1D0=(struct _tuple11*)_tmp1CF->hd;_tmp1D1=*
_tmp1D0;_LL221: _tmp1D3=_tmp1D1.f1;goto _LL220;_LL220: _tmp1D2=_tmp1D1.f2;goto
_LL21F;_LL21F: if(_tmp1D2.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
_tmp1D3.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4[0]=_tmp1D3;_tmp1D4;})): 0){Cyc_Std_fputs(
_tmp1D2,out_file);}}}}out_file=(struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file);({
struct Cyc_Std_String_pa_struct _tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=(struct
_tagged_arr)_tmp139;{void*_tmp1D5[1]={& _tmp1D6};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\n\n",sizeof(unsigned char),16),_tag_arr(_tmp1D5,sizeof(
void*),1));}});if(cycstubs != 0){out_file=(struct Cyc_Std___cycFILE*)_check_null(
Cyc_cycstubs_file);({void*_tmp1D7[0]={};Cyc_Std_fprintf(out_file,_tag_arr("namespace Std {\n",
sizeof(unsigned char),17),_tag_arr(_tmp1D7,sizeof(void*),0));});{struct Cyc_List_List*
_tmp1D8=cycstubs;for(0;_tmp1D8 != 0;_tmp1D8=_tmp1D8->tl){struct _tuple11 _tmp1DA;
struct _tagged_arr _tmp1DB;struct _tagged_arr _tmp1DC;struct _tuple11*_tmp1D9=(struct
_tuple11*)_tmp1D8->hd;_tmp1DA=*_tmp1D9;_LL224: _tmp1DC=_tmp1DA.f1;goto _LL223;
_LL223: _tmp1DB=_tmp1DA.f2;goto _LL222;_LL222: if(_tmp1DB.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr? _tmp1DC.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr? 1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[
0]=_tmp1DC;_tmp1DD;})): 0){Cyc_Std_fputs(_tmp1DB,out_file);}}}({void*_tmp1DE[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("}\n\n",sizeof(unsigned char),4),_tag_arr(
_tmp1DE,sizeof(void*),0));});}if(Cyc_Std_chdir((struct _tagged_arr)_tmp12E)){({
struct Cyc_Std_String_pa_struct _tmp1E0;_tmp1E0.tag=0;_tmp1E0.f1=(struct
_tagged_arr)_tmp12E;{void*_tmp1DF[1]={& _tmp1E0};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n",sizeof(unsigned char),41),
_tag_arr(_tmp1DF,sizeof(void*),1));}});return 1;}return 0;}}}}}}}}}}int Cyc_process_specfile(
struct _tagged_arr file,struct _tagged_arr dir){struct Cyc_Std___cycFILE*_tmp1EE=Cyc_Std_fopen(
file,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)_tmp1EE)){return 1;}{
struct Cyc_Std___cycFILE*_tmp1EF=(struct Cyc_Std___cycFILE*)_check_null(_tmp1EE);
struct Cyc_Lexing_lexbuf*_tmp1F0=Cyc_Lexing_from_file(_tmp1EF);struct _tuple12*
entry;while((entry=((struct _tuple12*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp1F0))!= 0){struct _tuple12 _tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List*
_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*
_tmp1F7;struct Cyc_List_List*_tmp1F8;struct _tagged_arr _tmp1F9;struct _tuple12*
_tmp1F1=(struct _tuple12*)_check_null(entry);_tmp1F2=*_tmp1F1;_LL22C: _tmp1F9=
_tmp1F2.f1;goto _LL22B;_LL22B: _tmp1F8=_tmp1F2.f2;goto _LL22A;_LL22A: _tmp1F7=
_tmp1F2.f3;goto _LL229;_LL229: _tmp1F6=_tmp1F2.f4;goto _LL228;_LL228: _tmp1F5=
_tmp1F2.f5;goto _LL227;_LL227: _tmp1F4=_tmp1F2.f6;goto _LL226;_LL226: _tmp1F3=
_tmp1F2.f7;goto _LL225;_LL225: if(Cyc_process_file(_tmp1F9,dir,_tmp1F8,_tmp1F7,
_tmp1F6,_tmp1F5,_tmp1F4,_tmp1F3)){Cyc_Std_fclose(_tmp1EF);return 1;}}Cyc_Std_fclose(
_tmp1EF);return 0;}}int Cyc_getsize(struct _tagged_arr dir,struct _tagged_arr includes,
struct _tagged_arr type){struct _tagged_arr _tmp1FA=Cyc_Filename_concat(dir,_tag_arr("getsize.c",
sizeof(unsigned char),10));struct _tagged_arr _tmp1FB=Cyc_Filename_concat(dir,
_tag_arr("getsize",sizeof(unsigned char),8));struct _tagged_arr _tmp1FC=Cyc_Filename_concat(
dir,_tag_arr("getsize.out",sizeof(unsigned char),12));struct Cyc_Std___cycFILE*
_tmp1FD=Cyc_Std_fopen((struct _tagged_arr)_tmp1FA,_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)_tmp1FD)){({struct Cyc_Std_String_pa_struct
_tmp1FF;_tmp1FF.tag=0;_tmp1FF.f1=(struct _tagged_arr)_tmp1FA;{void*_tmp1FE[1]={&
_tmp1FF};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create %s\n",
sizeof(unsigned char),28),_tag_arr(_tmp1FE,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp201;_tmp201.tag=0;_tmp201.f1=(struct _tagged_arr)type;{void*_tmp200[1]={&
_tmp201};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(
unsigned char),40),_tag_arr(_tmp200,sizeof(void*),1));}});return - 1;}({struct Cyc_Std_String_pa_struct
_tmp206;_tmp206.tag=0;_tmp206.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp205;_tmp205.tag=0;_tmp205.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp204;_tmp204.tag=0;_tmp204.f1=(struct _tagged_arr)_tmp1FC;{struct Cyc_Std_String_pa_struct
_tmp203;_tmp203.tag=0;_tmp203.f1=(struct _tagged_arr)includes;{void*_tmp202[4]={&
_tmp203,& _tmp204,& _tmp205,& _tmp206};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(_tmp1FD),_tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof(unsigned char),139),_tag_arr(_tmp202,sizeof(void*),4));}}}}});Cyc_Std_fclose((
struct Cyc_Std___cycFILE*)_check_null(_tmp1FD));{struct _tagged_arr _tmp207=
Cstring_to_string(Ccomp);struct _tagged_arr _tmp208=({struct Cyc_Std_String_pa_struct
_tmp21D;_tmp21D.tag=0;_tmp21D.f1=(struct _tagged_arr)_tmp1FA;{struct Cyc_Std_String_pa_struct
_tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct _tagged_arr)_tmp1FB;{struct Cyc_Std_String_pa_struct
_tmp21B;_tmp21B.tag=0;_tmp21B.f1=(struct _tagged_arr)_tmp207;{void*_tmp21A[3]={&
_tmp21B,& _tmp21C,& _tmp21D};Cyc_Std_aprintf(_tag_arr("%s -o %s -x c %s",sizeof(
unsigned char),17),_tag_arr(_tmp21A,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp208)!= 0){({struct Cyc_Std_String_pa_struct _tmp20A;_tmp20A.tag=
0;_tmp20A.f1=(struct _tagged_arr)type;{void*_tmp209[1]={& _tmp20A};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof(unsigned char),59),_tag_arr(_tmp209,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp20C;_tmp20C.tag=0;_tmp20C.f1=(struct _tagged_arr)type;{void*_tmp20B[1]={&
_tmp20C};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(
unsigned char),40),_tag_arr(_tmp20B,sizeof(void*),1));}});return - 1;}{int _tmp20D=
Cyc_Std_system((struct _tagged_arr)_tmp1FB);({struct Cyc_Std_Int_pa_struct _tmp210;
_tmp210.tag=1;_tmp210.f1=(int)((unsigned int)_tmp20D);{struct Cyc_Std_String_pa_struct
_tmp20F;_tmp20F.tag=0;_tmp20F.f1=(struct _tagged_arr)type;{void*_tmp20E[2]={&
_tmp20F,& _tmp210};Cyc_log(_tag_arr("size of %s returned by system is %d\n",
sizeof(unsigned char),37),_tag_arr(_tmp20E,sizeof(void*),2));}}});{struct Cyc_Std___cycFILE*
_tmp211=Cyc_Std_fopen((struct _tagged_arr)_tmp1FC,_tag_arr("r",sizeof(
unsigned char),2));int w=- 1;if(({struct Cyc_Std_IntPtr_sa_struct _tmp213;_tmp213.tag=
2;_tmp213.f1=& w;{void*_tmp212[1]={& _tmp213};Cyc_Std_fscanf((struct Cyc_Std___cycFILE*)
_check_null(_tmp211),_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp212,
sizeof(void*),1));}})!= 1){({struct Cyc_Std_String_pa_struct _tmp215;_tmp215.tag=0;
_tmp215.f1=(struct _tagged_arr)type;{void*_tmp214[1]={& _tmp215};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",
sizeof(unsigned char),40),_tag_arr(_tmp214,sizeof(void*),1));}});return - 1;}({
struct Cyc_Std_Int_pa_struct _tmp219;_tmp219.tag=1;_tmp219.f1=(int)((unsigned int)
w);{struct Cyc_Std_String_pa_struct _tmp218;_tmp218.tag=0;_tmp218.f1=(struct
_tagged_arr)_tmp1FC;{struct Cyc_Std_String_pa_struct _tmp217;_tmp217.tag=0;_tmp217.f1=(
struct _tagged_arr)type;{void*_tmp216[3]={& _tmp217,& _tmp218,& _tmp219};Cyc_log(
_tag_arr("size of %s read from file %s is %d\n",sizeof(unsigned char),36),
_tag_arr(_tmp216,sizeof(void*),3));}}}});Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(_tmp211));Cyc_Std_remove((struct _tagged_arr)_tmp1FC);Cyc_Std_remove((
struct _tagged_arr)_tmp1FA);Cyc_Std_remove((struct _tagged_arr)_tmp1FB);return w;}}}}
extern void GC_blacklist_warn_clear();int Cyc_main(int argc,struct _tagged_arr argv){
GC_blacklist_warn_clear();if(argc < 3){({void*_tmp21E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof(unsigned char),105),_tag_arr(_tmp21E,sizeof(void*),0));});return 1;}{
struct _tagged_arr _tmp21F=*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),1));if(Cyc_force_directory_prefixes((struct _tagged_arr)
_tmp21F)? 1: Cyc_force_directory((struct _tagged_arr)_tmp21F)){({struct Cyc_Std_String_pa_struct
_tmp221;_tmp221.tag=0;_tmp221.f1=(struct _tagged_arr)_tmp21F;{void*_tmp220[1]={&
_tmp221};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create directory %s\n",
sizeof(unsigned char),38),_tag_arr(_tmp220,sizeof(void*),1));}});return 1;}Cyc_log_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp21F,
_tag_arr("BUILDLIB.LOG",sizeof(unsigned char),13)),_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)Cyc_log_file)){({struct Cyc_Std_String_pa_struct
_tmp223;_tmp223.tag=0;_tmp223.f1=(struct _tagged_arr)_tmp21F;{void*_tmp222[1]={&
_tmp223};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create log file in directory %s\n",
sizeof(unsigned char),50),_tag_arr(_tmp222,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp21F,
_tag_arr("cstubs.c",sizeof(unsigned char),9)),_tag_arr("w",sizeof(unsigned char),
2));if(!((unsigned int)Cyc_cstubs_file)){({struct Cyc_Std_String_pa_struct _tmp225;
_tmp225.tag=0;_tmp225.f1=(struct _tagged_arr)_tmp21F;{void*_tmp224[1]={& _tmp225};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof(unsigned char),50),_tag_arr(_tmp224,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp21F,
_tag_arr("cycstubs.cyc",sizeof(unsigned char),13)),_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)Cyc_cycstubs_file)){({struct Cyc_Std_String_pa_struct
_tmp227;_tmp227.tag=0;_tmp227.f1=(struct _tagged_arr)_tmp21F;{void*_tmp226[1]={&
_tmp227};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof(unsigned char),52),_tag_arr(_tmp226,sizeof(void*),1));}});return 1;}({void*
_tmp228[0]={};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file),
_tag_arr("#include <core.h>\nusing Core;\n\n",sizeof(unsigned char),32),_tag_arr(
_tmp228,sizeof(void*),0));});Cyc_sizeof_unsignedlongint=Cyc_getsize((struct
_tagged_arr)_tmp21F,_tag_arr("",sizeof(unsigned char),1),_tag_arr("unsigned long int",
sizeof(unsigned char),18));Cyc_sizeof_unsignedshortint=Cyc_getsize((struct
_tagged_arr)_tmp21F,_tag_arr("",sizeof(unsigned char),1),_tag_arr("unsigned short int",
sizeof(unsigned char),19));Cyc_sizeof_shortint=Cyc_getsize((struct _tagged_arr)
_tmp21F,_tag_arr("",sizeof(unsigned char),1),_tag_arr("short int",sizeof(
unsigned char),10));Cyc_sizeof_int=Cyc_getsize((struct _tagged_arr)_tmp21F,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("int",sizeof(unsigned char),4));
Cyc_sizeof_short=Cyc_getsize((struct _tagged_arr)_tmp21F,_tag_arr("",sizeof(
unsigned char),1),_tag_arr("short",sizeof(unsigned char),6));Cyc_sizeof_fdmask=
Cyc_getsize((struct _tagged_arr)_tmp21F,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(unsigned char),47),_tag_arr("fd_mask",sizeof(unsigned char),8));Cyc_sizeof___fdmask=
Cyc_getsize((struct _tagged_arr)_tmp21F,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(unsigned char),47),_tag_arr("__fd_mask",sizeof(unsigned char),10));Cyc_sizeof_uint32=
Cyc_getsize((struct _tagged_arr)_tmp21F,_tag_arr("#include <sys/types.h>",sizeof(
unsigned char),23),_tag_arr("__uint32_t",sizeof(unsigned char),11));Cyc_sizeof_sockaddr=
Cyc_getsize((struct _tagged_arr)_tmp21F,_tag_arr("#include <sys/types.h>\n#include <sys/socket.h>",
sizeof(unsigned char),47),_tag_arr("struct sockaddr",sizeof(unsigned char),16));
Cyc_sizeof_inport=Cyc_getsize((struct _tagged_arr)_tmp21F,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(unsigned char),47),_tag_arr("in_port_t",sizeof(unsigned char),10));Cyc_sizeof_inaddr=
Cyc_getsize((struct _tagged_arr)_tmp21F,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(unsigned char),47),_tag_arr("struct in_addr",sizeof(unsigned char),15));{
int i=2;for(0;i < argc;i ++){Cyc_process_specfile((struct _tagged_arr)((struct
_tagged_arr*)argv.curr)[i],(struct _tagged_arr)_tmp21F);}}Cyc_Std_fclose((struct
Cyc_Std___cycFILE*)_check_null(Cyc_log_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cstubs_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file));return 0;}}
