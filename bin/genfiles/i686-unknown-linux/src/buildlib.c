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
  unsigned total_bytes;
  unsigned free_bytes;
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
extern void* _region_malloc(struct _RegionHandle *, unsigned);
extern void* _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void  _free_region(struct _RegionHandle *);

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
extern int _throw(void* e);
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
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
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
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
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
 void exit(int);struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _RegionHandle*Cyc_Core_heap_region;struct _tagged_arr
Cstring_to_string(unsigned char*);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;int Cyc_Std_remove(struct _tagged_arr);
int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);
struct Cyc_Std___cycFILE*Cyc_Std_fopen(struct _tagged_arr __filename,struct
_tagged_arr __modes);int Cyc_Std_fputc(int __c,struct Cyc_Std___cycFILE*__stream);
int Cyc_Std_fputs(struct _tagged_arr __s,struct Cyc_Std___cycFILE*__stream);extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);int Cyc_Std_printf(struct _tagged_arr fmt,struct
_tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct
_tagged_arr);int Cyc_Std_vfprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,
struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};int Cyc_Std_fscanf(
struct Cyc_Std___cycFILE*stream,struct _tagged_arr fmt,struct _tagged_arr);extern
unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{unsigned char*
tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _tagged_arr lex_buffer;int lex_buffer_len;int lex_abs_pos;
int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct
_tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr lex_default;
struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);struct _tagged_arr Cyc_Lexing_lexeme(
struct Cyc_Lexing_lexbuf*);unsigned char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,
int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern unsigned char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Iter_Iter Cyc_Set_make_iter(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned int Cyc_Std_strlen(struct
_tagged_arr s);int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Std_strdup(struct _tagged_arr src);struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_tagged_arr*p);struct _tagged_arr Cyc_Filename_concat(struct _tagged_arr,struct
_tagged_arr);struct _tagged_arr Cyc_Filename_chop_extension(struct _tagged_arr);
struct _tagged_arr Cyc_Filename_dirname(struct _tagged_arr);struct _tagged_arr Cyc_Filename_basename(
struct _tagged_arr);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];void
Cyc_Position_reset_position(struct _tagged_arr);struct Cyc_Position_Segment;struct
Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*
kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
extern unsigned char Cyc_Array_Array_mismatch[19];struct Cyc_Std__Div{int quot;int
rem;};struct Cyc_Std__Ldiv{int quot;int rem;};int Cyc_Std_system(struct _tagged_arr);
struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);struct
_tagged_arr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void Cyc_Buffer_add_char(
struct Cyc_Buffer_t*,unsigned char);void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,
struct _tagged_arr);struct Cyc_Std_tm{int tm_sec;int tm_min;int tm_hour;int tm_mday;
int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;};extern int timezone;
struct Cyc_Std_stat_t{unsigned long long st_dev;unsigned short __pad1;unsigned int
st_ino;unsigned int st_mode;unsigned int st_nlink;unsigned int st_uid;unsigned int
st_gid;unsigned long long st_rdev;unsigned short __pad2;int st_size;int st_blksize;
int st_blocks;int st_atime;unsigned int __unused1;int st_mtime;unsigned int __unused2;
int st_ctime;unsigned int __unused3;unsigned int __unused4;unsigned int __unused5;};
int Cyc_Std_mkdir(struct _tagged_arr pathname,unsigned int mode);struct Cyc_Std_flock{
short l_type;int l_start;short l_whence;int l_len;int l_pid;};struct Cyc_Std_Flock_struct{
int tag;struct Cyc_Std_flock*f1;};struct Cyc_Std_Long_struct{int tag;int f1;};int Cyc_Std_open(
struct _tagged_arr,int,struct _tagged_arr);struct Cyc_Std_timeval{int tv_sec;int
tv_usec;};struct Cyc_Std_timezone{int tz_minuteswest;int tz_dsttime;};struct Cyc_Std_itimerval{
struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval it_value;};typedef struct{
unsigned int __val[64];}Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t Cyc_Std_sigset_t;
struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;struct Cyc_Std___fd_set{
int __fds_bits[32];};struct Cyc_Std_option{struct _tagged_arr name;int has_arg;int*
flag;int val;};int close(int);int Cyc_Std_access(struct _tagged_arr,int);int Cyc_Std_chdir(
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
_tagged_arr prompt);int isspace(int);int toupper(int);struct Cyc_Absyn_Rel_n_struct{
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
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
f3;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
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
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
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
15];struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*
f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;int Cyc_yyparse();extern
unsigned char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
unsigned char*tag;struct Cyc_Abstractdeclarator*f1;};extern unsigned char Cyc_AggrFieldDeclListList_tok[
30];struct Cyc_AggrFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_AggrFieldDeclList_tok[26];struct Cyc_AggrFieldDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{
unsigned char*tag;int f1;};extern unsigned char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{
unsigned char*tag;unsigned char f1;};extern unsigned char Cyc_DeclList_tok[17];
struct Cyc_DeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{unsigned char*
tag;struct Cyc_Declaration_spec*f1;};extern unsigned char Cyc_Declarator_tok[19];
struct Cyc_Declarator_tok_struct{unsigned char*tag;struct Cyc_Declarator*f1;};
extern unsigned char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Designator_tok[
19];struct Cyc_Designator_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Enumfield_tok[
18];struct Cyc_Enumfield_tok_struct{unsigned char*tag;struct Cyc_Absyn_Enumfield*
f1;};extern unsigned char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Exp_tok[12];
struct Cyc_Exp_tok_struct{unsigned char*tag;struct Cyc_Absyn_Exp*f1;};extern
unsigned char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
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
unsigned char*tag;void*f1;};int Cyc_yyparse();extern unsigned char Cyc_YY1[8];
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};void Cyc_Lex_lex_init();extern unsigned char*Ccomp;struct Cyc_Std___cycFILE*
Cyc_log_file=0;struct Cyc_Std___cycFILE*Cyc_cstubs_file=0;struct Cyc_Std___cycFILE*
Cyc_cycstubs_file=0;int Cyc_log(struct _tagged_arr fmt,struct _tagged_arr ap){int
_tmp0=Cyc_Std_vfprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_log_file),fmt,
ap);Cyc_Std_fflush((struct Cyc_Std___cycFILE*)((struct Cyc_Std___cycFILE*)
_check_null(Cyc_log_file)));return _tmp0;}int Cyc_sizeof_unsignedlongint=- 1;int Cyc_sizeof_unsignedshortint=
- 1;int Cyc_sizeof_shortint=- 1;int Cyc_sizeof_int=- 1;int Cyc_sizeof_short=- 1;int Cyc_sizeof_fdmask=
- 1;int Cyc_sizeof___fdmask=- 1;int Cyc_sizeof_uint32=- 1;int Cyc_sizeof_sockaddr=- 1;
int Cyc_sizeof_inport=- 1;int Cyc_sizeof_inaddr=- 1;static struct _tagged_arr*Cyc_current_source=
0;static struct Cyc_List_List*Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=
0;static void Cyc_add_target(struct _tagged_arr*sptr){Cyc_current_targets=({struct
Cyc_Set_Set**_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),sptr);_tmp1;});}struct _tuple9{struct
_tagged_arr*f1;struct Cyc_Set_Set*f2;};struct _tuple9*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);int Cyc_string(struct Cyc_Lexing_lexbuf*);
struct Cyc_Std___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);
struct _tuple10{struct _tagged_arr f1;struct _tagged_arr*f2;};struct _tuple10*Cyc_suck_line(
struct Cyc_Lexing_lexbuf*);int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf*);struct _tagged_arr Cyc_current_line=(struct _tagged_arr){(
void*)0,(void*)0,(void*)(0 + 0)};struct _tuple11{struct _tagged_arr f1;struct
_tagged_arr f2;};struct _tuple12{struct _tagged_arr f1;struct Cyc_List_List*f2;struct
Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*
f6;struct Cyc_List_List*f7;};struct _tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);struct _tagged_arr Cyc_current_header=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=
0;struct Cyc_List_List*Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=
0;struct Cyc_List_List*Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_prologue=
0;struct Cyc_List_List*Cyc_current_epilogue=0;struct Cyc_List_List*Cyc_current_omit_symbols=
0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*Cyc_specbuf=0;int Cyc_braces_to_match=
0;int Cyc_parens_to_match=0;int Cyc_numdef=0;const int Cyc_lex_base[503]=(const int[
503]){0,0,75,192,305,283,309,166,310,91,27,384,28,502,616,694,287,338,93,- 3,0,- 1,
- 2,- 8,- 3,1,- 2,585,- 4,2,166,- 5,776,799,278,- 6,- 7,- 4,16,- 3,29,11,854,- 3,929,13,- 16,
223,12,- 2,225,22,26,28,24,28,58,56,65,52,70,332,1043,1158,352,93,83,90,112,96,
106,115,124,115,126,113,343,357,152,180,207,235,235,345,386,233,242,246,256,243,
262,372,1272,1387,396,- 8,256,253,272,258,258,259,281,381,1501,1616,- 11,410,- 12,
263,285,504,1730,1845,- 9,528,- 10,618,- 13,390,512,514,1922,1999,2076,2157,398,404,
632,2232,286,286,286,287,283,306,0,13,4,2313,5,617,2321,2386,635,49,497,6,2347,7,
643,2409,2447,681,- 30,1070,320,311,299,323,314,324,668,1188,332,329,321,404,333,
329,340,343,337,375,377,648,1072,367,412,407,412,412,808,882,419,414,848,421,429,
850,1186,425,424,1045,431,431,430,463,447,1052,1298,454,482,497,523,535,543,544,
533,1389,554,555,553,555,542,1503,545,546,1618,2509,563,558,1732,604,564,1847,
568,587,586,611,593,2371,615,602,615,598,607,2511,620,616,613,609,607,10,14,629,
609,2519,643,630,643,637,647,2521,973,640,640,674,666,677,687,675,704,708,718,
735,719,- 28,728,732,728,738,767,775,- 23,755,778,771,768,779,774,776,792,793,- 17,
780,778,778,817,818,- 25,810,808,801,813,835,851,829,851,893,881,885,876,876,- 27,
884,904,907,917,925,908,941,958,965,968,1,4,6,969,971,958,957,969,972,981,993,2,
52,994,995,1102,20,21,1028,1030,994,992,1007,1039,1040,1095,1096,1098,- 22,1077,
1078,1134,1135,1136,- 19,1083,1086,1141,1145,1147,- 20,1094,1096,1151,1152,1153,-
18,1105,1107,1162,1163,1164,- 21,1121,1122,1508,- 29,1416,1122,1125,1148,1147,1146,
1141,1144,1176,1178,1176,1189,1623,- 14,1181,1184,1183,1196,1737,- 15,- 7,- 8,8,1300,
2523,9,1356,2531,2556,1589,1513,- 49,1287,- 2,1236,- 4,1239,1274,2129,1240,1272,
1356,1703,1241,2613,2656,1248,1256,1253,1302,2726,1270,1304,- 36,- 42,- 37,2801,
1275,- 40,1305,- 45,- 39,- 48,2876,2905,1816,1329,1339,2623,2915,2945,2694,2886,2978,
3009,3047,1364,1375,3117,3155,1442,1452,1444,1454,1446,1456,- 6,- 34,1322,3087,- 47,
- 32,- 46,- 31,- 33,1373,3195,1377,1381,2376,1383,1384,1385,1388,1392,1393,1394,1395,
1406,3268,3352,2515,1411,- 24,- 41,- 38,- 35,- 26,1642,3434,4,3517,1437,15,1379,1379,
1417,1415,1414,1433,1504};const int Cyc_lex_backtrk[503]=(const int[503]){- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,
- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,14,15,- 1,15,15,15,15,15,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,13,14,2,4,4,- 1,1,1,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,
8,3,5,- 1,6,5,- 1,29,29,29,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,23,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,25,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,
0,48,48,- 1,- 1,- 1,0,43,- 1,42,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,
6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
503]=(const int[503]){- 1,- 1,- 1,403,392,154,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,
0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,119,- 1,- 1,- 1,- 1,- 1,126,126,126,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,
0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,
- 1,0,- 1,- 1,467,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,0,0,0,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,
0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3774]=(const int[3774]){0,0,
0,0,0,0,0,0,0,0,22,19,28,495,19,28,19,28,95,19,45,45,45,45,45,22,45,0,0,0,0,0,21,
316,327,496,21,22,- 1,- 1,22,- 1,- 1,45,317,45,318,22,493,493,493,493,493,493,493,
493,493,493,31,119,22,244,130,40,245,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,31,328,362,356,493,
146,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,491,491,491,491,491,491,491,491,491,491,137,85,20,78,
65,55,56,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,57,58,59,60,491,61,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,66,
67,37,404,405,404,404,405,39,22,68,69,70,71,72,145,34,34,34,34,34,34,34,34,73,74,
404,406,407,75,76,408,409,410,120,120,411,412,120,413,414,415,416,417,417,417,
417,417,417,417,417,417,418,79,419,420,421,120,19,422,422,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,19,-
1,- 1,423,422,80,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,422,393,424,22,394,24,24,37,147,138,81,155,
35,35,35,35,35,35,35,35,38,82,83,96,86,395,97,62,62,26,26,62,98,21,24,21,87,25,
77,77,84,84,77,88,84,89,90,64,64,91,62,64,77,77,117,109,77,99,26,100,101,77,102,
84,156,103,110,92,92,111,64,92,131,132,133,77,104,104,134,135,104,84,84,396,157,
84,- 1,148,139,- 1,92,94,94,136,- 1,94,39,- 1,22,104,- 1,254,158,- 1,84,107,107,159,21,
107,160,161,162,238,94,232,27,223,129,129,129,129,129,129,129,129,129,129,107,
169,170,171,172,173,22,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,35,174,175,187,129,28,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,- 1,194,19,- 1,121,121,112,112,121,179,112,180,95,195,120,120,128,
128,120,181,128,182,185,186,188,189,108,121,192,112,115,115,21,193,115,216,21,
120,196,128,197,122,123,122,122,122,122,122,122,122,122,122,122,115,114,198,199,
210,21,21,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,203,21,28,204,122,29,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
47,47,117,117,47,205,117,206,30,30,30,30,30,30,30,30,128,128,207,208,128,- 1,209,
47,211,117,116,212,213,- 1,214,215,176,176,217,- 1,176,218,48,128,144,144,144,144,
144,144,144,144,221,222,49,226,162,162,227,176,162,31,35,35,35,35,35,35,35,35,
153,153,153,153,153,153,153,153,224,162,228,229,41,41,32,230,41,163,231,233,234,
235,236,237,50,248,51,241,242,225,52,239,243,246,247,41,53,54,35,35,35,35,35,35,
35,35,240,249,250,251,118,42,42,42,42,42,42,42,42,42,42,252,- 1,253,305,306,291,
28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
285,275,268,261,42,262,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,183,183,43,263,183,264,265,33,33,33,33,33,33,33,33,33,33,
266,267,269,270,271,272,183,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,186,
186,190,190,186,273,190,33,33,33,33,33,33,274,276,21,33,33,33,33,33,33,277,186,
278,190,279,280,281,282,283,284,116,286,183,183,287,288,183,33,33,33,33,33,33,44,
44,44,44,44,44,44,44,44,44,289,290,183,292,293,294,295,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,296,297,298,299,44,28,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,184,300,301,302,303,304,368,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,307,308,309,310,44,311,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,
193,193,62,312,193,313,314,200,200,315,319,200,320,321,322,323,255,256,257,324,
258,62,325,193,259,372,372,176,176,372,200,176,114,260,326,329,330,63,63,63,63,
63,63,63,63,63,63,350,372,344,176,336,337,338,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,339,340,341,342,63,343,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,31,64,64,373,
331,64,345,346,332,347,348,349,351,374,177,352,353,333,375,334,354,178,355,357,
64,358,359,360,361,190,190,163,163,190,363,163,364,365,366,367,63,63,63,63,63,63,
63,63,63,63,369,370,190,386,163,335,380,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,376,377,378,379,63,46,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,92,92,164,381,92,382,
383,384,387,165,191,388,166,389,390,490,489,490,490,488,466,459,167,92,168,434,
200,200,402,402,200,486,402,290,371,431,304,432,490,93,93,93,93,93,93,93,93,93,
93,200,427,402,465,21,487,430,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,267,429,425,433,93,462,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,36,94,94,209,209,94,154,209,201,
401,401,401,401,401,401,401,401,485,202,95,95,355,463,464,94,349,209,367,361,106,
372,372,108,426,372,108,118,385,391,46,93,93,93,93,93,93,93,93,93,93,284,95,95,
372,35,343,35,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,21,497,498,35,93,35,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,104,104,215,215,104,373,215,370,370,499,500,370,
402,402,501,374,402,31,31,36,36,458,458,104,502,215,21,0,0,0,370,0,0,0,118,402,0,
21,371,105,105,105,105,105,105,105,105,105,105,31,31,36,36,458,458,0,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,0,0,0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,106,107,107,219,219,107,0,
219,384,384,0,0,384,31,31,31,31,31,31,31,31,0,0,0,107,0,219,490,0,490,490,384,0,
0,0,95,0,0,0,385,105,105,105,105,105,105,105,105,105,105,490,0,0,0,0,0,0,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,0,0,0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,112,112,222,222,112,0,
222,390,390,0,460,390,461,461,461,461,461,461,461,461,461,461,0,112,0,222,0,0,0,
0,390,0,0,0,106,0,0,0,391,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,114,115,115,225,
225,115,440,225,440,0,0,441,441,441,441,441,441,441,441,441,441,0,0,0,115,0,225,
0,0,0,0,0,0,0,0,36,0,0,0,0,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,124,124,124,124,
124,124,124,124,124,124,124,124,21,0,0,0,0,0,0,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,
124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,125,124,124,124,124,124,124,124,124,124,124,
21,0,0,0,0,0,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,0,0,0,0,124,0,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,21,0,0,0,0,- 1,0,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,- 1,- 1,0,- 1,124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,125,125,125,125,125,125,
125,125,125,125,125,125,127,0,0,0,0,0,468,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,0,0,0,0,125,
0,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,129,129,129,129,129,129,129,129,129,129,0,0,0,0,0,0,
0,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,28,0,0,140,129,0,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,0,0,28,0,0,
149,141,141,141,141,141,141,141,141,143,143,143,143,143,143,143,143,143,143,0,
231,231,0,0,231,- 1,143,143,143,143,143,143,0,0,0,150,150,150,150,150,150,150,150,
231,0,0,0,0,0,0,31,0,35,- 1,0,343,0,0,143,143,143,143,143,143,483,483,483,483,483,
483,483,483,0,142,143,143,143,143,143,143,143,143,143,143,31,0,0,0,0,0,0,143,143,
143,143,143,143,152,152,152,152,152,152,152,152,152,152,151,0,0,0,0,0,0,152,152,
152,152,152,152,0,0,0,143,143,143,143,143,143,0,0,0,0,0,0,152,152,152,152,152,
152,152,152,152,152,0,152,152,152,152,152,152,152,152,152,152,152,152,219,219,
237,237,219,0,237,0,0,0,247,247,253,253,247,19,253,0,397,0,0,0,0,219,0,237,152,
152,152,152,152,152,95,247,19,253,343,0,0,0,0,0,31,0,28,484,484,484,484,484,484,
484,484,398,398,398,398,398,398,398,398,400,400,400,400,400,400,400,400,400,400,
0,0,0,0,0,0,0,400,400,400,400,400,400,0,0,400,400,400,400,400,400,400,400,400,
400,220,0,0,0,0,0,28,400,400,400,400,400,400,0,400,400,400,400,400,400,0,0,0,0,0,
0,0,0,0,399,0,0,0,0,0,0,0,0,0,400,400,400,400,400,400,435,0,446,446,446,446,446,
446,446,446,447,447,441,441,441,441,441,441,441,441,441,441,0,437,0,0,0,0,0,0,
448,0,0,0,0,0,0,0,0,449,0,0,450,435,0,436,436,436,436,436,436,436,436,436,436,
437,0,0,0,0,0,0,448,0,0,0,437,0,0,0,0,449,0,438,450,0,0,0,444,0,444,0,439,445,
445,445,445,445,445,445,445,445,445,0,0,0,0,0,437,0,0,0,0,0,0,438,0,0,0,0,0,0,0,
0,439,428,428,428,428,428,428,428,428,428,428,0,0,0,0,0,0,0,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,0,0,0,0,428,0,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,0,0,0,0,0,0,0,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,0,0,0,0,428,0,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,442,442,442,442,442,442,442,442,442,442,445,445,445,445,445,445,445,445,445,
445,0,443,116,0,0,0,0,435,116,436,436,436,436,436,436,436,436,436,436,441,441,
441,441,441,441,441,441,441,441,0,437,0,0,443,116,0,0,438,0,0,116,114,0,0,0,0,
439,114,0,442,442,442,442,442,442,442,442,442,442,0,0,0,437,0,0,0,0,0,0,438,443,
116,0,114,0,0,0,116,439,114,0,0,445,445,445,445,445,445,445,445,445,445,0,0,0,0,
0,0,0,0,0,0,443,116,116,0,0,0,0,116,116,435,0,446,446,446,446,446,446,446,446,
447,447,0,0,0,0,0,0,0,0,0,0,0,437,0,116,0,0,0,0,456,116,0,0,0,0,0,0,435,457,447,
447,447,447,447,447,447,447,447,447,0,0,0,0,0,437,0,0,0,0,0,437,456,0,0,0,0,0,
454,0,0,457,0,0,0,0,0,455,0,0,461,461,461,461,461,461,461,461,461,461,0,0,0,437,
0,0,0,0,0,0,454,443,116,0,0,0,0,0,116,455,451,451,451,451,451,451,451,451,451,
451,0,0,0,0,0,0,0,451,451,451,451,451,451,443,116,0,0,0,0,0,116,0,0,0,0,0,0,0,
451,451,451,451,451,451,451,451,451,451,0,451,451,451,451,451,451,451,451,451,
451,451,451,0,0,0,469,0,452,0,0,470,0,0,0,0,0,453,0,0,471,471,471,471,471,471,
471,471,0,451,451,451,451,451,451,472,0,0,0,0,452,0,0,0,0,0,0,0,0,453,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,473,0,0,0,0,474,475,0,0,0,476,0,0,0,0,0,0,0,477,0,0,0,478,0,
479,0,480,0,481,482,482,482,482,482,482,482,482,482,482,0,0,0,0,0,0,0,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,0,0,0,0,0,0,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,274,0,0,0,0,0,0,0,0,482,482,
482,482,482,482,482,482,482,482,0,0,0,0,0,0,0,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,0,0,0,0,
0,0,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,22,0,0,492,0,0,0,491,491,491,491,491,491,491,491,491,
491,0,0,0,0,0,0,0,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,0,0,0,0,491,0,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,494,0,0,0,0,0,0,0,493,493,493,493,493,493,493,493,493,493,0,0,0,0,0,0,0,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,0,0,0,0,493,0,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0};const int Cyc_lex_check[3774]=(const int[3774]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,25,29,0,138,140,147,149,394,397,41,41,45,45,41,495,45,- 1,- 1,- 1,- 1,- 1,136,
315,326,0,492,10,12,40,10,12,40,41,316,45,317,20,1,1,1,1,1,1,1,1,1,1,38,48,137,
243,10,38,244,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,145,327,331,
332,1,145,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
2,9,51,18,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,
58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,65,66,7,3,3,3,3,3,
7,7,67,68,69,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,75,3,3,3,47,47,3,3,
47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,10,12,40,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,4,3,5,4,6,8,16,6,8,80,5,34,34,34,34,34,34,34,34,16,81,82,50,85,4,50,61,61,6,
8,61,50,9,17,18,86,17,76,76,83,83,76,87,83,88,89,64,64,90,61,64,77,77,96,97,77,
98,17,99,100,76,101,83,5,102,109,91,91,110,64,91,130,131,132,77,103,103,133,134,
103,84,84,4,5,84,119,6,8,119,91,94,94,135,126,94,16,126,16,103,127,156,157,127,
84,107,107,158,7,107,159,160,161,164,94,165,17,166,11,11,11,11,11,11,11,11,11,11,
107,168,169,170,171,172,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,64,173,174,177,11,77,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,146,167,84,146,13,13,111,111,13,178,
111,179,94,167,120,120,121,121,120,180,121,181,184,185,187,188,107,13,191,111,
115,115,5,192,115,194,16,120,195,121,196,13,13,13,13,13,13,13,13,13,13,13,13,115,
4,197,198,201,6,8,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,202,17,27,203,13,27,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,14,14,117,117,14,204,117,205,27,27,27,27,27,27,
27,27,128,128,206,207,128,119,208,14,210,117,115,211,212,126,213,214,175,175,216,
127,175,217,14,128,141,141,141,141,141,141,141,141,220,221,14,224,162,162,226,
175,162,27,144,144,144,144,144,144,144,144,150,150,150,150,150,150,150,150,223,
162,227,228,15,15,27,229,15,162,230,232,233,234,235,236,14,239,14,240,241,223,14,
238,242,245,246,15,14,14,153,153,153,153,153,153,153,153,238,248,249,250,117,15,
15,15,15,15,15,15,15,15,15,251,146,252,255,255,256,13,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,257,258,259,260,15,261,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,182,182,
15,262,182,263,264,32,32,32,32,32,32,32,32,32,32,265,266,268,269,270,271,182,32,
32,32,32,32,32,33,33,33,33,33,33,33,33,33,33,186,186,189,189,186,272,189,33,33,
33,33,33,33,273,275,14,32,32,32,32,32,32,276,186,277,189,278,279,280,281,282,283,
186,285,183,183,286,287,183,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,288,
289,183,291,292,293,294,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,295,296,297,298,42,15,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,183,299,
300,301,302,303,305,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,306,307,308,309,44,310,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,193,193,62,311,193,312,313,199,199,
314,318,199,319,320,321,322,254,254,254,323,254,62,324,193,254,155,155,176,176,
155,199,176,193,254,325,328,329,62,62,62,62,62,62,62,62,62,62,333,155,334,176,
335,336,337,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,338,339,340,341,62,342,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,63,63,155,330,63,344,345,330,346,347,348,350,
155,176,351,352,330,155,330,353,176,354,356,63,357,358,359,360,190,190,163,163,
190,362,163,363,364,365,366,63,63,63,63,63,63,63,63,63,63,368,369,190,373,163,
330,374,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,375,376,377,378,63,379,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,92,92,163,380,92,381,382,383,386,163,190,387,163,388,
389,404,406,404,404,408,411,415,163,92,163,418,200,200,395,395,200,409,395,420,
412,419,419,419,404,92,92,92,92,92,92,92,92,92,92,200,423,395,412,395,409,429,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,421,
421,424,431,92,460,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,93,93,209,209,93,413,209,200,398,398,398,398,398,398,398,
398,467,200,438,439,469,413,413,93,470,209,472,473,474,372,372,475,424,372,209,
476,477,478,479,93,93,93,93,93,93,93,93,93,93,480,438,439,372,448,484,449,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,494,496,
497,448,93,449,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,104,104,215,215,104,372,215,370,370,498,499,370,402,402,500,372,402,
452,453,454,455,456,457,104,501,215,502,- 1,- 1,- 1,370,- 1,- 1,- 1,215,402,- 1,402,370,
104,104,104,104,104,104,104,104,104,104,452,453,454,455,456,457,- 1,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,- 1,- 1,- 1,- 1,104,- 1,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,105,105,218,218,105,
- 1,218,384,384,- 1,- 1,384,401,401,401,401,401,401,401,401,- 1,- 1,- 1,105,- 1,218,490,
- 1,490,490,384,- 1,- 1,- 1,218,- 1,- 1,- 1,384,105,105,105,105,105,105,105,105,105,105,
490,- 1,- 1,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,105,- 1,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,112,112,222,222,112,- 1,222,390,390,- 1,414,390,414,414,414,414,414,
414,414,414,414,414,- 1,112,- 1,222,- 1,- 1,- 1,- 1,390,- 1,- 1,- 1,222,- 1,- 1,- 1,390,112,
112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,113,113,225,225,113,437,225,437,
- 1,- 1,437,437,437,437,437,437,437,437,437,437,- 1,- 1,- 1,113,- 1,225,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,225,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,122,
122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,- 1,- 1,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,123,123,123,123,123,
123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,
- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,124,124,124,124,124,124,124,124,124,124,124,
124,124,- 1,- 1,- 1,- 1,410,- 1,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,125,410,- 1,125,124,- 1,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,125,125,125,125,
125,- 1,- 1,- 1,- 1,- 1,410,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,125,- 1,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,139,- 1,- 1,139,129,- 1,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,- 1,- 1,148,- 1,- 1,148,
139,139,139,139,139,139,139,139,142,142,142,142,142,142,142,142,142,142,- 1,231,
231,- 1,- 1,231,410,142,142,142,142,142,142,- 1,- 1,- 1,148,148,148,148,148,148,148,
148,231,- 1,- 1,- 1,- 1,- 1,- 1,139,- 1,231,125,- 1,471,- 1,- 1,142,142,142,142,142,142,
471,471,471,471,471,471,471,471,- 1,139,143,143,143,143,143,143,143,143,143,143,
148,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,143,143,143,151,151,151,151,151,151,151,151,
151,151,148,- 1,- 1,- 1,- 1,- 1,- 1,151,151,151,151,151,151,- 1,- 1,- 1,143,143,143,143,
143,143,- 1,- 1,- 1,- 1,- 1,- 1,152,152,152,152,152,152,152,152,152,152,- 1,151,151,151,
151,151,151,152,152,152,152,152,152,219,219,237,237,219,- 1,237,- 1,- 1,- 1,247,247,
253,253,247,396,253,- 1,396,- 1,- 1,- 1,- 1,219,- 1,237,152,152,152,152,152,152,219,
247,237,253,483,- 1,- 1,- 1,- 1,- 1,247,- 1,253,483,483,483,483,483,483,483,483,396,
396,396,396,396,396,396,396,399,399,399,399,399,399,399,399,399,399,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,399,399,399,399,399,399,- 1,- 1,400,400,400,400,400,400,400,400,400,400,
219,- 1,- 1,- 1,- 1,- 1,396,400,400,400,400,400,400,- 1,399,399,399,399,399,399,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,416,
- 1,416,416,416,416,416,416,416,416,416,416,440,440,440,440,440,440,440,440,440,
440,- 1,416,- 1,- 1,- 1,- 1,- 1,- 1,416,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,416,- 1,- 1,416,417,- 1,
417,417,417,417,417,417,417,417,417,417,416,- 1,- 1,- 1,- 1,- 1,- 1,416,- 1,- 1,- 1,417,-
1,- 1,- 1,- 1,416,- 1,417,416,- 1,- 1,- 1,443,- 1,443,- 1,417,443,443,443,443,443,443,443,
443,443,443,- 1,- 1,- 1,- 1,- 1,417,- 1,- 1,- 1,- 1,- 1,- 1,417,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,417,
422,422,422,422,422,422,422,422,422,422,- 1,- 1,- 1,- 1,- 1,- 1,- 1,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,
422,- 1,- 1,- 1,- 1,422,- 1,422,422,422,422,422,422,422,422,422,422,422,422,422,422,
422,422,422,422,422,422,422,422,422,422,422,422,428,428,428,428,428,428,428,428,
428,428,- 1,- 1,- 1,- 1,- 1,- 1,- 1,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,- 1,- 1,- 1,- 1,428,- 1,428,428,
428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,428,
428,428,428,428,435,435,435,435,435,435,435,435,435,435,444,444,444,444,444,444,
444,444,444,444,- 1,435,435,- 1,- 1,- 1,- 1,436,435,436,436,436,436,436,436,436,436,
436,436,441,441,441,441,441,441,441,441,441,441,- 1,436,- 1,- 1,435,435,- 1,- 1,436,-
1,- 1,435,441,- 1,- 1,- 1,- 1,436,441,- 1,442,442,442,442,442,442,442,442,442,442,- 1,-
1,- 1,436,- 1,- 1,- 1,- 1,- 1,- 1,436,442,442,- 1,441,- 1,- 1,- 1,442,436,441,- 1,- 1,445,445,
445,445,445,445,445,445,445,445,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,442,442,445,- 1,- 1,
- 1,- 1,442,445,446,- 1,446,446,446,446,446,446,446,446,446,446,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,446,- 1,445,- 1,- 1,- 1,- 1,446,445,- 1,- 1,- 1,- 1,- 1,- 1,447,446,447,447,
447,447,447,447,447,447,447,447,- 1,- 1,- 1,- 1,- 1,446,- 1,- 1,- 1,- 1,- 1,447,446,- 1,- 1,
- 1,- 1,- 1,447,- 1,- 1,446,- 1,- 1,- 1,- 1,- 1,447,- 1,- 1,461,461,461,461,461,461,461,461,
461,461,- 1,- 1,- 1,447,- 1,- 1,- 1,- 1,- 1,- 1,447,461,461,- 1,- 1,- 1,- 1,- 1,461,447,450,
450,450,450,450,450,450,450,450,450,- 1,- 1,- 1,- 1,- 1,- 1,- 1,450,450,450,450,450,450,
461,461,- 1,- 1,- 1,- 1,- 1,461,- 1,- 1,- 1,- 1,- 1,- 1,- 1,451,451,451,451,451,451,451,451,
451,451,- 1,450,450,450,450,450,450,451,451,451,451,451,451,- 1,- 1,- 1,468,- 1,451,-
1,- 1,468,- 1,- 1,- 1,- 1,- 1,451,- 1,- 1,468,468,468,468,468,468,468,468,- 1,451,451,451,
451,451,451,468,- 1,- 1,- 1,- 1,451,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,451,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,468,- 1,- 1,- 1,- 1,468,468,- 1,- 1,- 1,468,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
468,- 1,- 1,- 1,468,- 1,468,- 1,468,- 1,468,481,481,481,481,481,481,481,481,481,481,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,
481,481,481,481,481,481,481,481,481,481,481,- 1,- 1,- 1,- 1,- 1,- 1,481,481,481,481,
481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,481,
481,481,482,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,482,482,482,482,482,482,482,482,482,482,- 1,-
1,- 1,- 1,- 1,- 1,- 1,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,- 1,- 1,- 1,- 1,- 1,- 1,482,482,482,482,482,
482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,482,
482,491,- 1,- 1,491,- 1,- 1,- 1,491,491,491,491,491,491,491,491,491,491,- 1,- 1,- 1,- 1,-
1,- 1,- 1,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,- 1,- 1,- 1,- 1,491,- 1,491,491,491,491,491,491,491,
491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,491,493,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,493,493,493,493,493,493,493,493,493,493,- 1,- 1,- 1,- 1,- 1,- 1,-
1,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,- 1,- 1,- 1,- 1,493,- 1,493,493,493,493,493,493,493,493,
493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,493,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
state];if(base < 0)return(- base)- 1;backtrk=Cyc_lex_backtrk[state];if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos
>= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}
else{c=(int)*((unsigned char*)_check_unknown_subscript(lbuf->lex_buffer,sizeof(
unsigned char),lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_lex_check[
_check_known_subscript_notnull(3774,base + c)]== state)state=Cyc_lex_trans[
_check_known_subscript_notnull(3774,base + c)];else{state=Cyc_lex_default[state];}
if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1)(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp2=_cycalloc(sizeof(*_tmp2));
_tmp2[0]=({struct Cyc_Lexing_Error_struct _tmp3;_tmp3.tag=Cyc_Lexing_Error;_tmp3.f1=
_tag_arr("empty token",sizeof(unsigned char),12);_tmp3;});_tmp2;}));else{return
lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=0;}}}struct _tuple9*
Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args
!= 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4[0]=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _tagged_arr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);_tmp4;});}return({
struct _tuple9*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->f1=(struct _tagged_arr*)
_check_null(Cyc_current_source);_tmp5->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));
_tmp5;});case 1: _LL1: return Cyc_line(lexbuf);case 2: _LL2: return 0;default: _LL3:(
lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({
struct Cyc_Lexing_Error_struct _tmp7;_tmp7.tag=Cyc_Lexing_Error;_tmp7.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp7;});_tmp6;}));}struct _tuple9*Cyc_line(struct Cyc_Lexing_lexbuf*
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
case 0: _LL4D: return 0;case 1: _LL4E: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}return 1;case 2:
_LL4F:({struct Cyc_Std_Int_pa_struct _tmp1E;_tmp1E.tag=1;_tmp1E.f1=(int)((
unsigned int)Cyc_sizeof_fdmask);{void*_tmp1D[1]={& _tmp1E};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp1D,sizeof(void*),1));}});({void*_tmp1F[0]={};Cyc_log(
_tag_arr("Warning: sizeof(fd_mask) inlined\n",sizeof(unsigned char),34),_tag_arr(
_tmp1F,sizeof(void*),0));});return 1;case 3: _LL50:({struct Cyc_Std_Int_pa_struct
_tmp21;_tmp21.tag=1;_tmp21.f1=(int)((unsigned int)Cyc_sizeof___fdmask);{void*
_tmp20[1]={& _tmp21};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp20,sizeof(void*),1));}});({
void*_tmp22[0]={};Cyc_log(_tag_arr("Warning: sizeof(__fd_mask) inlined\n",
sizeof(unsigned char),36),_tag_arr(_tmp22,sizeof(void*),0));});return 1;case 4:
_LL51:({struct Cyc_Std_Int_pa_struct _tmp24;_tmp24.tag=1;_tmp24.f1=(int)((
unsigned int)Cyc_sizeof_uint32);{void*_tmp23[1]={& _tmp24};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp23,sizeof(void*),1));}});({void*_tmp25[0]={};Cyc_log(
_tag_arr("Warning: sizeof(__uint32_t) inlined\n",sizeof(unsigned char),37),
_tag_arr(_tmp25,sizeof(void*),0));});return 1;case 5: _LL52:({struct Cyc_Std_Int_pa_struct
_tmp27;_tmp27.tag=1;_tmp27.f1=(int)((unsigned int)Cyc_sizeof_inport);{void*
_tmp26[1]={& _tmp27};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp26,sizeof(void*),1));}});({
void*_tmp28[0]={};Cyc_log(_tag_arr("Warning: sizeof(__in_port_t) inlined\n",
sizeof(unsigned char),38),_tag_arr(_tmp28,sizeof(void*),0));});return 1;case 6:
_LL53:({struct Cyc_Std_Int_pa_struct _tmp2A;_tmp2A.tag=1;_tmp2A.f1=(int)((
unsigned int)Cyc_sizeof_int);{void*_tmp29[1]={& _tmp2A};Cyc_Std_fprintf((struct
Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp29,sizeof(void*),1));}});({void*_tmp2B[0]={};Cyc_log(
_tag_arr("Warning: sizeof(int) inlined\n",sizeof(unsigned char),30),_tag_arr(
_tmp2B,sizeof(void*),0));});return 1;case 7: _LL54:({struct Cyc_Std_Int_pa_struct
_tmp2D;_tmp2D.tag=1;_tmp2D.f1=(int)((unsigned int)Cyc_sizeof_short);{void*_tmp2C[
1]={& _tmp2D};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp2C,sizeof(void*),1));}});({
void*_tmp2E[0]={};Cyc_log(_tag_arr("Warning: sizeof(__short) inlined\n",sizeof(
unsigned char),34),_tag_arr(_tmp2E,sizeof(void*),0));});return 1;case 8: _LL55:({
struct Cyc_Std_Int_pa_struct _tmp30;_tmp30.tag=1;_tmp30.f1=(int)((unsigned int)Cyc_sizeof_unsignedlongint);{
void*_tmp2F[1]={& _tmp30};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out),_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp2F,sizeof(
void*),1));}});({void*_tmp31[0]={};Cyc_log(_tag_arr("Warning: sizeof(unsigned long int) inlined\n",
sizeof(unsigned char),44),_tag_arr(_tmp31,sizeof(void*),0));});return 1;case 9:
_LL56:({struct Cyc_Std_Int_pa_struct _tmp33;_tmp33.tag=1;_tmp33.f1=(int)((
unsigned int)Cyc_sizeof_unsignedshortint);{void*_tmp32[1]={& _tmp33};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp32,sizeof(void*),1));}});({void*_tmp34[0]={};Cyc_log(
_tag_arr("Warning: sizeof(unsigned short int) inlined\n",sizeof(unsigned char),
45),_tag_arr(_tmp34,sizeof(void*),0));});return 1;case 10: _LL57:({struct Cyc_Std_Int_pa_struct
_tmp36;_tmp36.tag=1;_tmp36.f1=(int)((unsigned int)Cyc_sizeof_shortint);{void*
_tmp35[1]={& _tmp36};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp35,sizeof(void*),1));}});({
void*_tmp37[0]={};Cyc_log(_tag_arr("Warning: sizeof(short int) inlined\n",
sizeof(unsigned char),36),_tag_arr(_tmp37,sizeof(void*),0));});return 1;case 11:
_LL58:({struct Cyc_Std_Int_pa_struct _tmp39;_tmp39.tag=1;_tmp39.f1=(int)((
unsigned int)Cyc_sizeof_sockaddr);{void*_tmp38[1]={& _tmp39};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr(" %d ",sizeof(
unsigned char),5),_tag_arr(_tmp38,sizeof(void*),1));}});({void*_tmp3A[0]={};Cyc_log(
_tag_arr("Warning: sizeof(struct sockaddr) inlined\n",sizeof(unsigned char),42),
_tag_arr(_tmp3A,sizeof(void*),0));});return 1;case 12: _LL59:({struct Cyc_Std_Int_pa_struct
_tmp3C;_tmp3C.tag=1;_tmp3C.f1=(int)((unsigned int)Cyc_sizeof_inaddr);{void*
_tmp3B[1]={& _tmp3C};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr(" %d ",sizeof(unsigned char),5),_tag_arr(_tmp3B,sizeof(void*),1));}});({
void*_tmp3D[0]={};Cyc_log(_tag_arr("Warning: sizeof(struct inaddr) inlined\n",
sizeof(unsigned char),40),_tag_arr(_tmp3D,sizeof(void*),0));});return 1;case 13:
_LL5A: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC(",sizeof(
unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*
_tmp3E[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp3E,sizeof(void*),0));});return 1;case 14:
_LL5B: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC(",sizeof(
unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*
_tmp3F[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp3F,sizeof(void*),0));});return 1;case 15:
_LL5C: Cyc_Std_fputs(_tag_arr("__region",sizeof(unsigned char),9),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp40[0]={};Cyc_log(_tag_arr("Warning: use of region sidestepped\n",
sizeof(unsigned char),36),_tag_arr(_tmp40,sizeof(void*),0));});return 1;case 16:
_LL5D: return 1;case 17: _LL5E: return 1;case 18: _LL5F: return 1;case 19: _LL60: return 1;
case 20: _LL61: return 1;case 21: _LL62: return 1;case 22: _LL63: Cyc_Std_fputs(_tag_arr("inline",
sizeof(unsigned char),7),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 23: _LL64: Cyc_Std_fputs(_tag_arr("inline",sizeof(unsigned char),7),(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 24: _LL65: Cyc_Std_fputs(
_tag_arr("const",sizeof(unsigned char),6),(struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out));return 1;case 25: _LL66: Cyc_Std_fputs(_tag_arr("const",sizeof(
unsigned char),6),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 26: _LL67: Cyc_Std_fputs(_tag_arr("int",sizeof(unsigned char),4),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 27: _LL68: return 1;case 28: _LL69: Cyc_parens_to_match=
1;while(Cyc_asm(lexbuf)){;}Cyc_Std_fputs(_tag_arr("0",sizeof(unsigned char),2),(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp41[0]={};Cyc_log(
_tag_arr("Warning: replacing use of __asm__ with 0\n",sizeof(unsigned char),42),
_tag_arr(_tmp41,sizeof(void*),0));});return 1;case 29: _LL6A: Cyc_Std_fputc((int)Cyc_Lexing_lexeme_char(
lexbuf,0),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;default:
_LL6B:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp42=_cycalloc(sizeof(*_tmp42));
_tmp42[0]=({struct Cyc_Lexing_Error_struct _tmp43;_tmp43.tag=Cyc_Lexing_Error;
_tmp43.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp43;});
_tmp42;}));}int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(
lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6D: return 0;
case 1: _LL6E: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 0;case 2: _LL6F:({void*_tmp44[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(unsigned char),26),_tag_arr(_tmp44,sizeof(void*),0));});({struct Cyc_Std_String_pa_struct
_tmp46;_tmp46.tag=0;_tmp46.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp45[1]={& _tmp46};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp45,sizeof(void*),1));}});
return 1;case 3: _LL70:({struct Cyc_Std_String_pa_struct _tmp48;_tmp48.tag=0;_tmp48.f1=(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp47[1]={& _tmp48};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp47,sizeof(void*),1));}});return 1;case 4: _LL71:({
struct Cyc_Std_String_pa_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf);{void*_tmp49[1]={& _tmp4A};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp49,sizeof(void*),1));}});return 1;case 5: _LL72:({struct Cyc_Std_String_pa_struct
_tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4B[1]={& _tmp4C};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4B,sizeof(void*),1));}});
return 1;case 6: _LL73:({struct Cyc_Std_String_pa_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp4D[1]={& _tmp4E};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp4D,sizeof(void*),1));}});return 1;case 7: _LL74:({
struct Cyc_Std_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf);{void*_tmp4F[1]={& _tmp50};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp4F,sizeof(void*),1));}});return 1;case 8: _LL75:({struct Cyc_Std_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp51[1]={& _tmp52};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp51,sizeof(void*),1));}});
return 1;default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp53=
_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Lexing_Error_struct _tmp54;
_tmp54.tag=Cyc_Lexing_Error;_tmp54.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp54;});_tmp53;}));}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL78: return 0;case 1: _LL79: if(Cyc_parens_to_match == 1)return 0;Cyc_parens_to_match
--;return 1;case 2: _LL7A: Cyc_parens_to_match ++;return 1;case 3: _LL7B: while(Cyc_asm_string(
lexbuf)){;}return 1;case 4: _LL7C: while(Cyc_asm_comment(lexbuf)){;}return 1;case 5:
_LL7D: return 1;case 6: _LL7E: return 1;default: _LL7F:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=({struct Cyc_Lexing_Error_struct
_tmp56;_tmp56.tag=Cyc_Lexing_Error;_tmp56.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp56;});_tmp55;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL81:({void*_tmp57[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(unsigned char),26),_tag_arr(_tmp57,sizeof(void*),0));});return 0;case 1:
_LL82: return 0;case 2: _LL83:({void*_tmp58[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(unsigned char),26),_tag_arr(_tmp58,sizeof(void*),0));});return 1;case 3:
_LL84: return 1;case 4: _LL85: return 1;case 5: _LL86: return 1;case 6: _LL87: return 1;case 7:
_LL88: return 1;case 8: _LL89: return 1;default: _LL8A:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Lexing_Error_struct
_tmp5A;_tmp5A.tag=Cyc_Lexing_Error;_tmp5A.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp5A;});_tmp59;}));}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL8C:({void*_tmp5B[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",
sizeof(unsigned char),27),_tag_arr(_tmp5B,sizeof(void*),0));});return 0;case 1:
_LL8D: return 0;case 2: _LL8E: return 1;default: _LL8F:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Lexing_Error_struct
_tmp5D;_tmp5D.tag=Cyc_Lexing_Error;_tmp5D.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp5D;});_tmp5C;}));}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple10*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL91: Cyc_current_line=_tag_arr("#define ",
sizeof(unsigned char),9);Cyc_suck_macroname(lexbuf);return({struct _tuple10*
_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->f1=Cyc_current_line;_tmp5E->f2=(struct
_tagged_arr*)_check_null(Cyc_current_source);_tmp5E;});case 1: _LL92: return Cyc_suck_line(
lexbuf);case 2: _LL93: return 0;default: _LL94:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Lexing_Error_struct
_tmp60;_tmp60.tag=Cyc_Lexing_Error;_tmp60.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp60;});_tmp5F;}));}struct _tuple10*Cyc_suck_line(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL96: Cyc_current_source=({struct _tagged_arr*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp61;});Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,*((
struct _tagged_arr*)_check_null(Cyc_current_source)));return Cyc_suck_restofline(
lexbuf);default: _LL97:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp62=
_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Lexing_Error_struct _tmp63;
_tmp63.tag=Cyc_Lexing_Error;_tmp63.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp63;});_tmp62;}));}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL99: Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(
Cyc_current_line,(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default:
_LL9A:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp64=_cycalloc(
sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Lexing_Error_struct _tmp65;_tmp65.tag=Cyc_Lexing_Error;
_tmp65.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp65;});
_tmp64;}));}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple12*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL9C:
Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=0;Cyc_current_cstubs=
0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_prologue=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);Cyc_current_cstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cycstubs);return({struct _tuple12*_tmp66=_cycalloc(sizeof(*_tmp66));
_tmp66->f1=Cyc_current_header;_tmp66->f2=Cyc_current_symbols;_tmp66->f3=Cyc_current_omit_symbols;
_tmp66->f4=Cyc_current_prologue;_tmp66->f5=Cyc_current_epilogue;_tmp66->f6=Cyc_current_cstubs;
_tmp66->f7=Cyc_current_cycstubs;_tmp66;});case 1: _LL9D: return Cyc_spec(lexbuf);
case 2: _LL9E: return Cyc_spec(lexbuf);case 3: _LL9F: return 0;case 4: _LLA0:({struct Cyc_Std_Int_pa_struct
_tmp68;_tmp68.tag=1;_tmp68.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp67[1]={& _tmp68};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof(unsigned char),67),_tag_arr(_tmp67,sizeof(void*),1));}});return 0;default:
_LLA1:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp69=_cycalloc(sizeof(*_tmp69));
_tmp69[0]=({struct Cyc_Lexing_Error_struct _tmp6A;_tmp6A.tag=Cyc_Lexing_Error;
_tmp6A.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp6A;});
_tmp69;}));}struct _tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(
lexbuf,13);}int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLA3: return 0;
case 1: _LLA4: return 0;case 2: _LLA5: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(
lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LLA6: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LLA7: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp6C->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp6C;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->hd=x;_tmp6B->tl=
Cyc_current_prologue;_tmp6B;});return 1;}case 5: _LLA8: {struct _tagged_arr _tmp6D=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp6D,sizeof(unsigned char),(
int)Cyc_Std_strlen(_tag_arr("prologue",sizeof(unsigned char),9)));while(isspace((
int)*((unsigned char*)_check_unknown_subscript(_tmp6D,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp6D,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp6D;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp6E=Cyc_Std_substring((struct _tagged_arr)_tmp6D,0,(unsigned int)((t.curr - 
_tmp6D.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=(struct
_tagged_arr)_tmp6E;_tmp70->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp70;});Cyc_current_prologue=({struct Cyc_List_List*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->hd=x;_tmp6F->tl=Cyc_current_prologue;
_tmp6F;});return 1;}}}}case 6: _LLA9: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*x=({struct
_tuple11*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp72->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp72;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=x;_tmp71->tl=Cyc_current_epilogue;
_tmp71;});return 1;}case 7: _LLAA: {struct _tagged_arr _tmp73=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp73,sizeof(unsigned char),(int)Cyc_Std_strlen(
_tag_arr("epilogue",sizeof(unsigned char),9)));while(isspace((int)*((
unsigned char*)_check_unknown_subscript(_tmp73,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp73,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp73;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp74=Cyc_Std_substring((struct _tagged_arr)_tmp73,0,(unsigned int)((t.curr - 
_tmp73.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=(struct
_tagged_arr)_tmp74;_tmp76->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp76;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->hd=x;_tmp75->tl=Cyc_current_epilogue;
_tmp75;});return 1;}}}}case 8: _LLAB: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*x=({struct
_tuple11*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp78->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp78;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->hd=x;_tmp77->tl=Cyc_current_cstubs;
_tmp77;});return 1;}case 9: _LLAC: {struct _tagged_arr _tmp79=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp79,sizeof(unsigned char),(int)Cyc_Std_strlen(
_tag_arr("cstub",sizeof(unsigned char),6)));while(isspace((int)*((unsigned char*)
_check_unknown_subscript(_tmp79,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp79,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp79;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp7A=Cyc_Std_substring((struct _tagged_arr)_tmp79,0,(unsigned int)((t.curr - 
_tmp79.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=(struct
_tagged_arr)_tmp7A;_tmp7C->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp7C;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->hd=x;_tmp7B->tl=Cyc_current_cstubs;
_tmp7B;});return 1;}}}}case 10: _LLAD: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*x=({struct
_tuple11*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp7E->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp7E;});Cyc_current_cycstubs=({struct Cyc_List_List*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=x;_tmp7D->tl=Cyc_current_cycstubs;
_tmp7D;});return 1;}case 11: _LLAE: {struct _tagged_arr _tmp7F=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp7F,sizeof(unsigned char),(int)Cyc_Std_strlen(
_tag_arr("cycstub",sizeof(unsigned char),8)));while(isspace((int)*((
unsigned char*)_check_unknown_subscript(_tmp7F,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp7F,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp7F;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp80=Cyc_Std_substring((struct _tagged_arr)_tmp7F,0,(unsigned int)((t.curr - 
_tmp7F.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(struct
_tagged_arr)_tmp80;_tmp82->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp82;});Cyc_current_cycstubs=({struct Cyc_List_List*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->hd=x;_tmp81->tl=Cyc_current_cycstubs;
_tmp81;});return 1;}}}}case 12: _LLAF: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tagged_arr*x=({struct
_tagged_arr*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp84;});Cyc_current_cpp=({struct
Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=x;_tmp83->tl=Cyc_current_cpp;
_tmp83;});return 1;}case 13: _LLB0: return 1;case 14: _LLB1: return 1;case 15: _LLB2:({
struct Cyc_Std_Int_pa_struct _tmp86;_tmp86.tag=1;_tmp86.f1=(int)((unsigned int)((
int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp85[1]={& _tmp86};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof(unsigned char),58),_tag_arr(_tmp85,sizeof(void*),1));}});return 0;default:
_LLB3:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87[0]=({struct Cyc_Lexing_Error_struct _tmp88;_tmp88.tag=Cyc_Lexing_Error;
_tmp88.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp88;});
_tmp87;}));}int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(
lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB5: Cyc_snarfed_symbols=({
struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=({struct
_tagged_arr*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp8A;});_tmp89->tl=Cyc_snarfed_symbols;_tmp89;});return 1;case 1: _LLB6:
return 1;case 2: _LLB7: return 0;case 3: _LLB8:({void*_tmp8B[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n",sizeof(unsigned char),44),
_tag_arr(_tmp8B,sizeof(void*),0));});return 0;case 4: _LLB9:({struct Cyc_Std_Int_pa_struct
_tmp8D;_tmp8D.tag=1;_tmp8D.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp8C[1]={& _tmp8D};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof(unsigned char),57),_tag_arr(_tmp8C,sizeof(void*),1));}});return 0;default:
_LLBA:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp8E=_cycalloc(sizeof(*
_tmp8E));_tmp8E[0]=({struct Cyc_Lexing_Error_struct _tmp8F;_tmp8F.tag=Cyc_Lexing_Error;
_tmp8F.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp8F;});
_tmp8E;}));}int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBC:({void*
_tmp90[0]={};Cyc_log(_tag_arr("Warning: unclosed brace\n",sizeof(unsigned char),
25),_tag_arr(_tmp90,sizeof(void*),0));});return 0;case 1: _LLBD: if(Cyc_braces_to_match
== 1)return 0;Cyc_braces_to_match --;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLBE: Cyc_braces_to_match ++;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLBF: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
_tag_arr("/*",sizeof(unsigned char),3));while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC2: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLC3:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp91=_cycalloc(
sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Lexing_Error_struct _tmp92;_tmp92.tag=Cyc_Lexing_Error;
_tmp92.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp92;});
_tmp91;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(
lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC5:({void*
_tmp93[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(unsigned char),
26),_tag_arr(_tmp93,sizeof(void*),0));});return 0;case 1: _LLC6: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLC7:({void*
_tmp94[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(unsigned char),
26),_tag_arr(_tmp94,sizeof(void*),0));});Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 3: _LLC8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 4: _LLC9: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 5: _LLCA: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 6: _LLCB: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 7: _LLCC: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 8: _LLCD: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
default: _LLCE:(lexbuf->refill_buff)(lexbuf);return Cyc_block_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp95=_cycalloc(
sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Lexing_Error_struct _tmp96;_tmp96.tag=Cyc_Lexing_Error;
_tmp96.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp96;});
_tmp95;}));}int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLD0:({void*
_tmp97[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(unsigned char),
27),_tag_arr(_tmp97,sizeof(void*),0));});return 0;case 1: _LLD1: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tag_arr("*/",sizeof(unsigned char),
3));return 0;case 2: _LLD2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(
Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLD3:(
lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp98=_cycalloc(sizeof(*_tmp98));
_tmp98[0]=({struct Cyc_Lexing_Error_struct _tmp99;_tmp99.tag=Cyc_Lexing_Error;
_tmp99.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp99;});
_tmp98;}));}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}static void Cyc_pr(struct _tagged_arr*sptr){({struct Cyc_Std_String_pa_struct
_tmp9B;_tmp9B.tag=0;_tmp9B.f1=(struct _tagged_arr)*sptr;{void*_tmp9A[1]={& _tmp9B};
Cyc_Std_printf(_tag_arr(" %s\n",sizeof(unsigned char),5),_tag_arr(_tmp9A,sizeof(
void*),1));}});}void Cyc_scan_type(void*t);struct _tuple13{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmp9C=(
void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmp9D;struct
_tuple0 _tmp9E;struct _tagged_arr*_tmp9F;struct _tuple0*_tmpA0;struct _tuple0 _tmpA1;
struct _tagged_arr*_tmpA2;struct Cyc_List_List*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;
struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA7;
struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpAA;
struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAD;
struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Exp*_tmpB0;
struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_List_List*_tmpB2;struct Cyc_Absyn_Exp*_tmpB3;
struct Cyc_List_List*_tmpB4;void*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_MallocInfo
_tmpB7;int _tmpB8;struct Cyc_Absyn_Exp*_tmpB9;void**_tmpBA;struct Cyc_Absyn_Exp*
_tmpBB;void*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct _tagged_arr*_tmpBE;struct Cyc_Absyn_Exp*
_tmpBF;struct _tagged_arr*_tmpC0;void*_tmpC1;void*_tmpC2;struct Cyc_List_List*
_tmpC3;_LLD6: if(*((int*)_tmp9C)!= 1)goto _LLD8;_tmp9D=((struct Cyc_Absyn_Var_e_struct*)
_tmp9C)->f1;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f2;_LLD7: _tmpA2=_tmp9F;goto _LLD9;_LLD8:
if(*((int*)_tmp9C)!= 2)goto _LLDA;_tmpA0=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp9C)->f1;_tmpA1=*_tmpA0;_tmpA2=_tmpA1.f2;_LLD9: Cyc_add_target(_tmpA2);return;
_LLDA: if(*((int*)_tmp9C)!= 3)goto _LLDC;_tmpA3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9C)->f2;_LLDB: for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpA3->hd));}return;_LLDC: if(*((int*)_tmp9C)!= 23)goto
_LLDE;_tmpA4=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9C)->f1;_tmpA5=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp9C)->f2;_LLDD: _tmpA6=_tmpA4;_tmpA7=_tmpA5;goto
_LLDF;_LLDE: if(*((int*)_tmp9C)!= 7)goto _LLE0;_tmpA6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9C)->f1;_tmpA7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9C)->f2;_LLDF: _tmpA8=
_tmpA6;_tmpA9=_tmpA7;goto _LLE1;_LLE0: if(*((int*)_tmp9C)!= 4)goto _LLE2;_tmpA8=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp9C)->f1;_tmpA9=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9C)->f3;_LLE1: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpA8);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpA9);return;_LLE2: if(*((int*)_tmp9C)!= 20)goto _LLE4;
_tmpAA=((struct Cyc_Absyn_Deref_e_struct*)_tmp9C)->f1;_LLE3: _tmpAB=_tmpAA;goto
_LLE5;_LLE4: if(*((int*)_tmp9C)!= 17)goto _LLE6;_tmpAB=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9C)->f1;_LLE5: _tmpAC=_tmpAB;goto _LLE7;_LLE6: if(*((int*)_tmp9C)!= 14)goto
_LLE8;_tmpAC=((struct Cyc_Absyn_Address_e_struct*)_tmp9C)->f1;_LLE7: _tmpAD=_tmpAC;
goto _LLE9;_LLE8: if(*((int*)_tmp9C)!= 5)goto _LLEA;_tmpAD=((struct Cyc_Absyn_Increment_e_struct*)
_tmp9C)->f1;_LLE9: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAD);return;_LLEA: if(*((
int*)_tmp9C)!= 6)goto _LLEC;_tmpAE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9C)->f1;
_tmpAF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9C)->f2;_tmpB0=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9C)->f3;_LLEB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAE);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpAF);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB0);return;
_LLEC: if(*((int*)_tmp9C)!= 9)goto _LLEE;_tmpB1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp9C)->f1;_tmpB2=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9C)->f2;_LLED: _tmpB3=
_tmpB1;_tmpB4=_tmpB2;goto _LLEF;_LLEE: if(*((int*)_tmp9C)!= 8)goto _LLF0;_tmpB3=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9C)->f1;_tmpB4=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp9C)->f2;_LLEF: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB3);for(0;_tmpB4 != 0;
_tmpB4=_tmpB4->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmpB4->hd));}return;_LLF0: if(*((int*)_tmp9C)!= 13)goto _LLF2;_tmpB5=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9C)->f1;_tmpB6=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9C)->f2;_LLF1: Cyc_scan_type(_tmpB5);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpB6);return;_LLF2: if(*((int*)_tmp9C)!= 33)goto _LLF4;_tmpB7=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp9C)->f1;_tmpB8=_tmpB7.is_calloc;_tmpB9=_tmpB7.rgn;_tmpBA=_tmpB7.elt_type;
_tmpBB=_tmpB7.num_elts;_LLF3: if(_tmpB9 != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmpB9)));if(_tmpBA != 0)Cyc_scan_type(*_tmpBA);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBB);return;_LLF4: if(*((int*)_tmp9C)!= 16)
goto _LLF6;_tmpBC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp9C)->f1;_LLF5:
Cyc_scan_type(_tmpBC);return;_LLF6: if(*((int*)_tmp9C)!= 21)goto _LLF8;_tmpBD=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9C)->f1;_tmpBE=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9C)->f2;_LLF7: _tmpBF=_tmpBD;_tmpC0=_tmpBE;goto _LLF9;_LLF8: if(*((int*)_tmp9C)
!= 22)goto _LLFA;_tmpBF=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9C)->f1;_tmpC0=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9C)->f2;_LLF9: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpBF);Cyc_add_target(_tmpC0);return;_LLFA: if(*((int*)_tmp9C)!= 18)goto _LLFC;
_tmpC1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp9C)->f1;_tmpC2=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp9C)->f2;_LLFB: Cyc_scan_type(_tmpC1);{void*
_tmpC4=_tmpC2;struct _tagged_arr*_tmpC5;_LL123: if(*((int*)_tmpC4)!= 0)goto _LL125;
_tmpC5=((struct Cyc_Absyn_StructField_struct*)_tmpC4)->f1;_LL124: Cyc_add_target(
_tmpC5);goto _LL122;_LL125: if(*((int*)_tmpC4)!= 1)goto _LL122;_LL126: goto _LL122;
_LL122:;}return;_LLFC: if(*((int*)_tmp9C)!= 0)goto _LLFE;_LLFD: return;_LLFE: if(*((
int*)_tmp9C)!= 34)goto _LL100;_tmpC3=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp9C)->f2;_LLFF: for(0;_tmpC3 != 0;_tmpC3=_tmpC3->tl){struct _tuple13 _tmpC7;
struct Cyc_Absyn_Exp*_tmpC8;struct _tuple13*_tmpC6=(struct _tuple13*)_tmpC3->hd;
_tmpC7=*_tmpC6;_tmpC8=_tmpC7.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC8);}
return;_LL100: if(*((int*)_tmp9C)!= 35)goto _LL102;_LL101:({void*_tmpC9[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Stmt_e\n",sizeof(unsigned char),26),
_tag_arr(_tmpC9,sizeof(void*),0));});exit(1);return;_LL102: if(*((int*)_tmp9C)!= 
10)goto _LL104;_LL103:({void*_tmpCA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Throw_e\n",
sizeof(unsigned char),27),_tag_arr(_tmpCA,sizeof(void*),0));});exit(1);return;
_LL104: if(*((int*)_tmp9C)!= 11)goto _LL106;_LL105:({void*_tmpCB[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected NoInstantiate_e\n",sizeof(
unsigned char),35),_tag_arr(_tmpCB,sizeof(void*),0));});exit(1);return;_LL106:
if(*((int*)_tmp9C)!= 12)goto _LL108;_LL107:({void*_tmpCC[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Instantiate_e\n",sizeof(unsigned char),
33),_tag_arr(_tmpCC,sizeof(void*),0));});exit(1);return;_LL108: if(*((int*)_tmp9C)
!= 15)goto _LL10A;_LL109:({void*_tmpCD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected New_e\n",sizeof(unsigned char),25),_tag_arr(_tmpCD,
sizeof(void*),0));});exit(1);return;_LL10A: if(*((int*)_tmp9C)!= 19)goto _LL10C;
_LL10B:({void*_tmpCE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Gentyp_e\n",
sizeof(unsigned char),28),_tag_arr(_tmpCE,sizeof(void*),0));});exit(1);return;
_LL10C: if(*((int*)_tmp9C)!= 24)goto _LL10E;_LL10D:({void*_tmpCF[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Tuple_e\n",sizeof(unsigned char),27),
_tag_arr(_tmpCF,sizeof(void*),0));});exit(1);return;_LL10E: if(*((int*)_tmp9C)!= 
25)goto _LL110;_LL10F:({void*_tmpD0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected CompoundLit_e\n",
sizeof(unsigned char),33),_tag_arr(_tmpD0,sizeof(void*),0));});exit(1);return;
_LL110: if(*((int*)_tmp9C)!= 26)goto _LL112;_LL111:({void*_tmpD1[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Array_e\n",sizeof(unsigned char),27),
_tag_arr(_tmpD1,sizeof(void*),0));});exit(1);return;_LL112: if(*((int*)_tmp9C)!= 
27)goto _LL114;_LL113:({void*_tmpD2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Comprehension_e\n",
sizeof(unsigned char),35),_tag_arr(_tmpD2,sizeof(void*),0));});exit(1);return;
_LL114: if(*((int*)_tmp9C)!= 28)goto _LL116;_LL115:({void*_tmpD3[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Struct_e\n",sizeof(unsigned char),28),
_tag_arr(_tmpD3,sizeof(void*),0));});exit(1);return;_LL116: if(*((int*)_tmp9C)!= 
29)goto _LL118;_LL117:({void*_tmpD4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonStruct_e\n",
sizeof(unsigned char),32),_tag_arr(_tmpD4,sizeof(void*),0));});exit(1);return;
_LL118: if(*((int*)_tmp9C)!= 30)goto _LL11A;_LL119:({void*_tmpD5[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Tunion_e\n",sizeof(unsigned char),28),
_tag_arr(_tmpD5,sizeof(void*),0));});exit(1);return;_LL11A: if(*((int*)_tmp9C)!= 
31)goto _LL11C;_LL11B:({void*_tmpD6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Enum_e\n",
sizeof(unsigned char),26),_tag_arr(_tmpD6,sizeof(void*),0));});exit(1);return;
_LL11C: if(*((int*)_tmp9C)!= 32)goto _LL11E;_LL11D:({void*_tmpD7[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected AnonEnum_e\n",sizeof(unsigned char),
30),_tag_arr(_tmpD7,sizeof(void*),0));});exit(1);return;_LL11E: if(*((int*)_tmp9C)
!= 36)goto _LL120;_LL11F:({void*_tmpD8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Codegen_e\n",sizeof(unsigned char),29),_tag_arr(
_tmpD8,sizeof(void*),0));});exit(1);return;_LL120: if(*((int*)_tmp9C)!= 37)goto
_LLD5;_LL121:({void*_tmpD9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Fill_e\n",
sizeof(unsigned char),26),_tag_arr(_tmpD9,sizeof(void*),0));});exit(1);return;
_LLD5:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(
eo);return;}void Cyc_scan_type(void*t){void*_tmpDA=t;struct Cyc_Absyn_PtrInfo
_tmpDB;void*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_FnInfo _tmpDE;
struct Cyc_List_List*_tmpDF;struct Cyc_Absyn_AggrInfo _tmpE0;void*_tmpE1;struct
_tuple0*_tmpE2;struct _tuple0 _tmpE3;struct _tagged_arr*_tmpE4;struct _tuple0*_tmpE5;
struct _tuple0 _tmpE6;struct _tagged_arr*_tmpE7;_LL128: if((int)_tmpDA != 0)goto
_LL12A;_LL129: goto _LL12B;_LL12A: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 5)goto
_LL12C;_LL12B: goto _LL12D;_LL12C: if((int)_tmpDA != 1)goto _LL12E;_LL12D: goto _LL12F;
_LL12E: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 6)goto _LL130;_LL12F: return;
_LL130: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 4)goto _LL132;_tmpDB=((struct Cyc_Absyn_PointerType_struct*)
_tmpDA)->f1;_LL131: Cyc_scan_type((void*)_tmpDB.elt_typ);return;_LL132: if(_tmpDA
<= (void*)3?1:*((int*)_tmpDA)!= 7)goto _LL134;_tmpDC=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpDA)->f1;_tmpDD=((struct Cyc_Absyn_ArrayType_struct*)_tmpDA)->f3;_LL133: Cyc_scan_type(
_tmpDC);Cyc_scan_exp_opt(_tmpDD);return;_LL134: if(_tmpDA <= (void*)3?1:*((int*)
_tmpDA)!= 8)goto _LL136;_tmpDE=((struct Cyc_Absyn_FnType_struct*)_tmpDA)->f1;
_LL135: Cyc_scan_type((void*)_tmpDE.ret_typ);{struct Cyc_List_List*_tmpE8=_tmpDE.args;
for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){struct _tuple1 _tmpEA;void*_tmpEB;struct
_tuple1*_tmpE9=(struct _tuple1*)_tmpE8->hd;_tmpEA=*_tmpE9;_tmpEB=_tmpEA.f3;Cyc_scan_type(
_tmpEB);}}if(_tmpDE.cyc_varargs != 0)Cyc_scan_type((void*)(_tmpDE.cyc_varargs)->type);
return;_LL136: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 11)goto _LL138;_tmpDF=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpDA)->f2;_LL137: for(0;_tmpDF != 0;_tmpDF=
_tmpDF->tl){Cyc_scan_type((void*)((struct Cyc_Absyn_Aggrfield*)_tmpDF->hd)->type);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmpDF->hd)->width);}return;_LL138:
if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 13)goto _LL13A;_LL139: return;_LL13A: if(
_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 10)goto _LL13C;_tmpE0=((struct Cyc_Absyn_AggrType_struct*)
_tmpDA)->f1;_tmpE1=(void*)_tmpE0.aggr_info;_LL13B: {struct _tuple0*_tmpED;struct
_tuple0 _tmpEE;struct _tagged_arr*_tmpEF;struct _tuple3 _tmpEC=Cyc_Absyn_aggr_kinded_name(
_tmpE1);_tmpED=_tmpEC.f2;_tmpEE=*_tmpED;_tmpEF=_tmpEE.f2;_tmpE4=_tmpEF;goto
_LL13D;}_LL13C: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 12)goto _LL13E;_tmpE2=((
struct Cyc_Absyn_EnumType_struct*)_tmpDA)->f1;_tmpE3=*_tmpE2;_tmpE4=_tmpE3.f2;
_LL13D: _tmpE7=_tmpE4;goto _LL13F;_LL13E: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 
16)goto _LL140;_tmpE5=((struct Cyc_Absyn_TypedefType_struct*)_tmpDA)->f1;_tmpE6=*
_tmpE5;_tmpE7=_tmpE6.f2;_LL13F: Cyc_add_target(_tmpE7);return;_LL140: if(_tmpDA <= (
void*)3?1:*((int*)_tmpDA)!= 0)goto _LL142;_LL141:({void*_tmpF0[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Evar\n",sizeof(unsigned char),24),
_tag_arr(_tmpF0,sizeof(void*),0));});exit(1);return;_LL142: if(_tmpDA <= (void*)3?
1:*((int*)_tmpDA)!= 1)goto _LL144;_LL143:({void*_tmpF1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected VarType\n",sizeof(unsigned char),27),_tag_arr(_tmpF1,
sizeof(void*),0));});exit(1);return;_LL144: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)
!= 2)goto _LL146;_LL145:({void*_tmpF2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionType\n",sizeof(unsigned char),30),_tag_arr(
_tmpF2,sizeof(void*),0));});exit(1);return;_LL146: if(_tmpDA <= (void*)3?1:*((int*)
_tmpDA)!= 3)goto _LL148;_LL147:({void*_tmpF3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionFieldType\n",sizeof(unsigned char),35),
_tag_arr(_tmpF3,sizeof(void*),0));});exit(1);return;_LL148: if(_tmpDA <= (void*)3?
1:*((int*)_tmpDA)!= 9)goto _LL14A;_LL149:({void*_tmpF4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n",sizeof(unsigned char),29),_tag_arr(
_tmpF4,sizeof(void*),0));});exit(1);return;_LL14A: if(_tmpDA <= (void*)3?1:*((int*)
_tmpDA)!= 15)goto _LL14C;_LL14B:({void*_tmpF5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnHandleType\n",sizeof(unsigned char),33),_tag_arr(
_tmpF5,sizeof(void*),0));});exit(1);return;_LL14C: if((int)_tmpDA != 2)goto _LL14E;
_LL14D:({void*_tmpF6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected HeapRgn\n",
sizeof(unsigned char),27),_tag_arr(_tmpF6,sizeof(void*),0));});exit(1);return;
_LL14E: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 17)goto _LL150;_LL14F:({void*
_tmpF7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AccessEff\n",
sizeof(unsigned char),29),_tag_arr(_tmpF7,sizeof(void*),0));});exit(1);return;
_LL150: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 18)goto _LL152;_LL151:({void*
_tmpF8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected JoinEff\n",
sizeof(unsigned char),27),_tag_arr(_tmpF8,sizeof(void*),0));});exit(1);return;
_LL152: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 19)goto _LL154;_LL153:({void*
_tmpF9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected RgnsEff\n",
sizeof(unsigned char),27),_tag_arr(_tmpF9,sizeof(void*),0));});exit(1);return;
_LL154: if(_tmpDA <= (void*)3?1:*((int*)_tmpDA)!= 14)goto _LL127;_LL155:({void*
_tmpFA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected sizeof_t\n",
sizeof(unsigned char),28),_tag_arr(_tmpFA,sizeof(void*),0));});exit(1);return;
_LL127:;}struct _tuple14{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct _tuple9*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct
Cyc_Set_Set**_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
_tmpFB;});{void*_tmpFC=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmpFD;struct Cyc_Absyn_Fndecl*
_tmpFE;struct Cyc_Absyn_Aggrdecl*_tmpFF;struct Cyc_Absyn_Enumdecl*_tmp100;struct
Cyc_Absyn_Typedefdecl*_tmp101;_LL157: if(*((int*)_tmpFC)!= 0)goto _LL159;_tmpFD=((
struct Cyc_Absyn_Var_d_struct*)_tmpFC)->f1;_LL158: {struct _tuple0 _tmp103;struct
_tagged_arr*_tmp104;struct _tuple0*_tmp102=_tmpFD->name;_tmp103=*_tmp102;_tmp104=
_tmp103.f2;Cyc_current_source=(struct _tagged_arr*)_tmp104;Cyc_scan_type((void*)
_tmpFD->type);Cyc_scan_exp_opt(_tmpFD->initializer);goto _LL156;}_LL159: if(*((int*)
_tmpFC)!= 1)goto _LL15B;_tmpFE=((struct Cyc_Absyn_Fn_d_struct*)_tmpFC)->f1;_LL15A: {
struct _tuple0 _tmp106;struct _tagged_arr*_tmp107;struct _tuple0*_tmp105=_tmpFE->name;
_tmp106=*_tmp105;_tmp107=_tmp106.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp107;Cyc_scan_type((void*)_tmpFE->ret_type);{struct Cyc_List_List*_tmp108=
_tmpFE->args;for(0;_tmp108 != 0;_tmp108=_tmp108->tl){struct _tuple14 _tmp10A;void*
_tmp10B;struct _tuple14*_tmp109=(struct _tuple14*)_tmp108->hd;_tmp10A=*_tmp109;
_tmp10B=_tmp10A.f3;Cyc_scan_type(_tmp10B);}}if(_tmpFE->cyc_varargs != 0)Cyc_scan_type((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpFE->cyc_varargs))->type);if(
_tmpFE->is_inline)({void*_tmp10C[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: ignoring inline function\n",
sizeof(unsigned char),35),_tag_arr(_tmp10C,sizeof(void*),0));});goto _LL156;}
_LL15B: if(*((int*)_tmpFC)!= 4)goto _LL15D;_tmpFF=((struct Cyc_Absyn_Aggr_d_struct*)
_tmpFC)->f1;_LL15C: {struct _tuple0 _tmp10E;struct _tagged_arr*_tmp10F;struct
_tuple0*_tmp10D=_tmpFF->name;_tmp10E=*_tmp10D;_tmp10F=_tmp10E.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp10F;if((unsigned int)_tmpFF->fields){{struct Cyc_List_List*
_tmp110=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpFF->fields))->v;
for(0;_tmp110 != 0;_tmp110=_tmp110->tl){struct Cyc_Absyn_Aggrfield*_tmp111=(struct
Cyc_Absyn_Aggrfield*)_tmp110->hd;Cyc_scan_type((void*)_tmp111->type);Cyc_scan_exp_opt(
_tmp111->width);}}{struct Cyc_List_List*_tmp112=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpFF->fields))->v;for(0;_tmp112 != 0;_tmp112=_tmp112->tl){;}}}goto
_LL156;}_LL15D: if(*((int*)_tmpFC)!= 6)goto _LL15F;_tmp100=((struct Cyc_Absyn_Enum_d_struct*)
_tmpFC)->f1;_LL15E: {struct _tuple0 _tmp114;struct _tagged_arr*_tmp115;struct
_tuple0*_tmp113=_tmp100->name;_tmp114=*_tmp113;_tmp115=_tmp114.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp115;if((unsigned int)_tmp100->fields){{struct Cyc_List_List*
_tmp116=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp100->fields))->v;
for(0;_tmp116 != 0;_tmp116=_tmp116->tl){struct Cyc_Absyn_Enumfield*_tmp117=(struct
Cyc_Absyn_Enumfield*)_tmp116->hd;Cyc_scan_exp_opt(_tmp117->tag);}}{struct Cyc_List_List*
_tmp118=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp100->fields))->v;
for(0;_tmp118 != 0;_tmp118=_tmp118->tl){;}}}goto _LL156;}_LL15F: if(*((int*)_tmpFC)
!= 7)goto _LL161;_tmp101=((struct Cyc_Absyn_Typedef_d_struct*)_tmpFC)->f1;_LL160: {
struct _tuple0 _tmp11A;struct _tagged_arr*_tmp11B;struct _tuple0*_tmp119=_tmp101->name;
_tmp11A=*_tmp119;_tmp11B=_tmp11A.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp11B;Cyc_scan_type((void*)_tmp101->defn);goto _LL156;}_LL161: if(*((int*)_tmpFC)
!= 2)goto _LL163;_LL162:({void*_tmp11C[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(unsigned char),35),
_tag_arr(_tmp11C,sizeof(void*),0));});exit(1);return 0;_LL163: if(*((int*)_tmpFC)
!= 5)goto _LL165;_LL164:({void*_tmp11D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected tunion declaration\n",sizeof(unsigned char),38),
_tag_arr(_tmp11D,sizeof(void*),0));});exit(1);return 0;_LL165: if(*((int*)_tmpFC)
!= 3)goto _LL167;_LL166:({void*_tmp11E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(unsigned char),35),
_tag_arr(_tmp11E,sizeof(void*),0));});exit(1);return 0;_LL167: if(*((int*)_tmpFC)
!= 8)goto _LL169;_LL168:({void*_tmp11F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected namespace declaration\n",sizeof(unsigned char),41),
_tag_arr(_tmp11F,sizeof(void*),0));});exit(1);return 0;_LL169: if(*((int*)_tmpFC)
!= 9)goto _LL16B;_LL16A:({void*_tmp120[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected using declaration\n",sizeof(unsigned char),37),
_tag_arr(_tmp120,sizeof(void*),0));});exit(1);return 0;_LL16B: if(*((int*)_tmpFC)
!= 10)goto _LL156;_LL16C:({void*_tmp121[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n",sizeof(unsigned char),42),
_tag_arr(_tmp121,sizeof(void*),0));});exit(1);return 0;_LL156:;}return({struct
_tuple9*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=(struct _tagged_arr*)
_check_null(Cyc_current_source);_tmp122->f2=*((struct Cyc_Set_Set**)_check_null(
Cyc_current_targets));_tmp122;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,
Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*x){struct _handler_cons _tmp123;_push_handler(& _tmp123);{int
_tmp125=0;if(setjmp(_tmp123.handler))_tmp125=1;if(!_tmp125){{struct Cyc_Set_Set*
_tmp126=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*
key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp126;};_pop_handler();}
else{void*_tmp124=(void*)_exn_thrown;void*_tmp128=_tmp124;_LL16E: if(_tmp128 != 
Cyc_Core_Not_found)goto _LL170;_LL16F: return((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_LL170:;
_LL171:(void)_throw(_tmp128);_LL16D:;}}}struct Cyc_Set_Set*Cyc_reachable(struct
Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
curr,(struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp129=curr;struct
_tagged_arr*_tmp12A=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(unsigned char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp129)> 0){struct Cyc_Set_Set*_tmp12B=emptyset;struct Cyc_Iter_Iter _tmp12C=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(
Cyc_Core_heap_region,_tmp129);while(((int(*)(struct Cyc_Iter_Iter,struct
_tagged_arr**))Cyc_Iter_next)(_tmp12C,& _tmp12A)){_tmp12B=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp12B,Cyc_find(t,
_tmp12A));}_tmp129=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_diff)(_tmp12B,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp129);}return curr;}}int Cyc_force_directory(
struct _tagged_arr d){int _tmp12E=({unsigned int _tmp131[0];Cyc_Std_open(d,0,
_tag_arr(_tmp131,sizeof(unsigned int),0));});if(_tmp12E == - 1){if(Cyc_Std_mkdir(d,
448)== - 1){({struct Cyc_Std_String_pa_struct _tmp130;_tmp130.tag=0;_tmp130.f1=(
struct _tagged_arr)d;{void*_tmp12F[1]={& _tmp130};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create directory %s\n",sizeof(unsigned char),38),
_tag_arr(_tmp12F,sizeof(void*),1));}});return 1;}}else{close(_tmp12E);}return 0;}
int Cyc_force_directory_prefixes(struct _tagged_arr file){struct _tagged_arr _tmp132=
Cyc_Std_strdup(file);struct Cyc_List_List*_tmp133=0;while(1){_tmp132=Cyc_Filename_dirname((
struct _tagged_arr)_tmp132);if(_get_arr_size(_tmp132,sizeof(unsigned char))== 0)
break;_tmp133=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=({
struct _tagged_arr*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135[0]=(struct
_tagged_arr)_tmp132;_tmp135;});_tmp134->tl=_tmp133;_tmp134;});}for(0;_tmp133 != 0;
_tmp133=_tmp133->tl){if(Cyc_force_directory(*((struct _tagged_arr*)_tmp133->hd)))
return 1;}return 0;}int Cyc_process_file(struct _tagged_arr filename,struct
_tagged_arr dir,struct Cyc_List_List*start_symbols,struct Cyc_List_List*
omit_symbols,struct Cyc_List_List*prologue,struct Cyc_List_List*epilogue,struct Cyc_List_List*
cstubs,struct Cyc_List_List*cycstubs){struct Cyc_Std___cycFILE*maybe;struct Cyc_Std___cycFILE*
in_file;struct Cyc_Std___cycFILE*out_file;({struct Cyc_Std_String_pa_struct _tmp137;
_tmp137.tag=0;_tmp137.f1=(struct _tagged_arr)filename;{void*_tmp136[1]={& _tmp137};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("********************************* %s...\n",
sizeof(unsigned char),41),_tag_arr(_tmp136,sizeof(void*),1));}});{struct
_tagged_arr buf=_tag_arr(({unsigned int _tmp1F1=(unsigned int)1024;unsigned char*
_tmp1F2=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),
_tmp1F1));{unsigned int _tmp1F3=_tmp1F1;unsigned int i;for(i=0;i < _tmp1F3;i ++){
_tmp1F2[i]='\000';}}_tmp1F2;}),sizeof(unsigned char),(unsigned int)1024);struct
_tagged_arr _tmp138=Cyc_Std_getcwd(buf,_get_arr_size(buf,sizeof(unsigned char)));
if(Cyc_Std_chdir(dir)){({struct Cyc_Std_String_pa_struct _tmp13A;_tmp13A.tag=0;
_tmp13A.f1=(struct _tagged_arr)dir;{void*_tmp139[1]={& _tmp13A};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: can't change directory to %s\n",sizeof(
unsigned char),37),_tag_arr(_tmp139,sizeof(void*),1));}});return 1;}({struct Cyc_Std_String_pa_struct
_tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct _tagged_arr)filename;{void*_tmp13B[1]={&
_tmp13C};Cyc_log(_tag_arr("\n%s:\n",sizeof(unsigned char),6),_tag_arr(_tmp13B,
sizeof(void*),1));}});{struct _tagged_arr _tmp13D=Cyc_Filename_basename(filename);
struct _tagged_arr _tmp13E=Cyc_Filename_chop_extension((struct _tagged_arr)_tmp13D);
struct _tagged_arr _tmp13F=Cyc_Std_strconcat((struct _tagged_arr)_tmp13E,_tag_arr(".iA",
sizeof(unsigned char),4));struct _tagged_arr _tmp140=Cyc_Std_strconcat((struct
_tagged_arr)_tmp13E,_tag_arr(".i",sizeof(unsigned char),3));struct _tagged_arr
_tmp141=Cyc_Std_strconcat((struct _tagged_arr)_tmp13E,_tag_arr(".iB",sizeof(
unsigned char),4));struct _tagged_arr _tmp142=Cyc_Filename_dirname(filename);
struct _tagged_arr _tmp143=_get_arr_size(_tmp142,sizeof(unsigned char))== 0?({
struct Cyc_Std_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct
_tagged_arr)_tmp13D;{void*_tmp1ED[1]={& _tmp1EE};Cyc_Std_aprintf(_tag_arr("c%s",
sizeof(unsigned char),4),_tag_arr(_tmp1ED,sizeof(void*),1));}}): Cyc_Filename_concat((
struct _tagged_arr)_tmp142,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1F0;_tmp1F0.tag=0;_tmp1F0.f1=(struct _tagged_arr)_tmp13D;{void*_tmp1EF[1]={&
_tmp1F0};Cyc_Std_aprintf(_tag_arr("c%s",sizeof(unsigned char),4),_tag_arr(
_tmp1EF,sizeof(void*),1));}}));if(Cyc_force_directory_prefixes(filename))return 1;{
struct Cyc_Hashtable_Table*t=Cyc_new_deps();struct _tagged_arr _tmp144=
Cstring_to_string(Ccomp);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp13F,_tag_arr("w",
sizeof(unsigned char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp146;_tmp146.tag=0;_tmp146.f1=(struct _tagged_arr)_tmp13F;{void*_tmp145[1]={&
_tmp146};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(unsigned char),33),_tag_arr(_tmp145,sizeof(void*),1));}});return 1;}
out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);Cyc_current_cpp=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);{
struct Cyc_List_List*_tmp147=Cyc_current_cpp;for(0;_tmp147 != 0;_tmp147=_tmp147->tl){
Cyc_Std_fputs(*((struct _tagged_arr*)_tmp147->hd),out_file);}}({struct Cyc_Std_String_pa_struct
_tmp149;_tmp149.tag=0;_tmp149.f1=(struct _tagged_arr)filename;{void*_tmp148[1]={&
_tmp149};Cyc_Std_fprintf(out_file,_tag_arr("#include <%s>\n",sizeof(
unsigned char),15),_tag_arr(_tmp148,sizeof(void*),1));}});Cyc_Std_fclose(
out_file);{struct _tagged_arr _tmp14A=({struct Cyc_Std_String_pa_struct _tmp1EC;
_tmp1EC.tag=0;_tmp1EC.f1=(struct _tagged_arr)_tmp13F;{struct Cyc_Std_String_pa_struct
_tmp1EB;_tmp1EB.tag=0;_tmp1EB.f1=(struct _tagged_arr)_tmp140;{struct Cyc_Std_String_pa_struct
_tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _tagged_arr)_tmp144;{void*_tmp1E9[3]={&
_tmp1EA,& _tmp1EB,& _tmp1EC};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",
sizeof(unsigned char),24),_tag_arr(_tmp1E9,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp14A)!= 0){Cyc_Std_remove((struct _tagged_arr)_tmp13F);maybe=
Cyc_Std_fopen(filename,_tag_arr("w",sizeof(unsigned char),2));if(!((unsigned int)
maybe)){({struct Cyc_Std_String_pa_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct
_tagged_arr)filename;{void*_tmp14B[1]={& _tmp14C};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),33),_tag_arr(
_tmp14B,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(
struct _tagged_arr)filename;{void*_tmp14D[1]={& _tmp14E};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(unsigned char),
48),_tag_arr(_tmp14D,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp143,_tag_arr("w",sizeof(unsigned char),2));if(!((
unsigned int)maybe)){({struct Cyc_Std_String_pa_struct _tmp150;_tmp150.tag=0;
_tmp150.f1=(struct _tagged_arr)_tmp143;{void*_tmp14F[1]={& _tmp150};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),
33),_tag_arr(_tmp14F,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(
struct _tagged_arr)_tmp143;{void*_tmp151[1]={& _tmp152};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(unsigned char),
48),_tag_arr(_tmp151,sizeof(void*),1));}});Cyc_Std_fclose(out_file);({struct Cyc_Std_String_pa_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(struct _tagged_arr)filename;{void*_tmp153[1]={&
_tmp154};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: %s will not be supported on this platform\n",
sizeof(unsigned char),52),_tag_arr(_tmp153,sizeof(void*),1));}});({void*_tmp155[
0]={};Cyc_log(_tag_arr("Not supported on this platform\n",sizeof(unsigned char),
32),_tag_arr(_tmp155,sizeof(void*),0));});if(Cyc_Std_chdir((struct _tagged_arr)
_tmp138)){({struct Cyc_Std_String_pa_struct _tmp157;_tmp157.tag=0;_tmp157.f1=(
struct _tagged_arr)_tmp138;{void*_tmp156[1]={& _tmp157};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n",sizeof(unsigned char),41),
_tag_arr(_tmp156,sizeof(void*),1));}});return 1;}return 0;}maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp140,_tag_arr("r",sizeof(unsigned char),2));if(!((
unsigned int)maybe))return 1;in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);{
struct Cyc_Lexing_lexbuf*_tmp158=Cyc_Lexing_from_file(in_file);struct _tuple9*
entry;while((entry=((struct _tuple9*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(
_tmp158))!= 0){struct _tuple9 _tmp15A;struct _tagged_arr*_tmp15B;struct Cyc_Set_Set*
_tmp15C;struct _tuple9*_tmp159=(struct _tuple9*)_check_null(entry);_tmp15A=*
_tmp159;_tmp15B=_tmp15A.f1;_tmp15C=_tmp15A.f2;((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp15B,
_tmp15C);}Cyc_Std_fclose(in_file);_tmp14A=({struct Cyc_Std_String_pa_struct
_tmp160;_tmp160.tag=0;_tmp160.f1=(struct _tagged_arr)_tmp13F;{struct Cyc_Std_String_pa_struct
_tmp15F;_tmp15F.tag=0;_tmp15F.f1=(struct _tagged_arr)_tmp140;{struct Cyc_Std_String_pa_struct
_tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct _tagged_arr)_tmp144;{void*_tmp15D[3]={&
_tmp15E,& _tmp15F,& _tmp160};Cyc_Std_aprintf(_tag_arr("%s -E -o %s -x c %s",
sizeof(unsigned char),20),_tag_arr(_tmp15D,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp14A)!= 0)return 1;maybe=Cyc_Std_fopen((struct _tagged_arr)
_tmp140,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)maybe))return 1;
in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);_tmp158=Cyc_Lexing_from_file(
in_file);Cyc_slurp_out=Cyc_Std_fopen((struct _tagged_arr)_tmp141,_tag_arr("w",
sizeof(unsigned char),2));if(!((unsigned int)Cyc_slurp_out))return 1;while(((int(*)(
struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp158)){;}Cyc_Std_fclose(in_file);
Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp141,_tag_arr("r",sizeof(unsigned char),2));if(!((
unsigned int)maybe))return 1;in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);
Cyc_Position_reset_position((struct _tagged_arr)_tmp141);Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp161=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(
in_file);Cyc_Std_remove((struct _tagged_arr)_tmp141);{struct Cyc_List_List*_tmp162=
_tmp161;for(0;_tmp162 != 0;_tmp162=_tmp162->tl){struct _tuple9*_tmp163=Cyc_scan_decl((
struct Cyc_Absyn_Decl*)_tmp162->hd);if(_tmp163 == 0)continue;{struct _tuple9 _tmp165;
struct _tagged_arr*_tmp166;struct Cyc_Set_Set*_tmp167;struct _tuple9*_tmp164=(
struct _tuple9*)_check_null(_tmp163);_tmp165=*_tmp164;_tmp166=_tmp165.f1;_tmp167=
_tmp165.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp168;_push_handler(&
_tmp168);{int _tmp16A=0;if(setjmp(_tmp168.handler))_tmp16A=1;if(!_tmp16A){old=((
struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(
t,_tmp166);;_pop_handler();}else{void*_tmp169=(void*)_exn_thrown;void*_tmp16C=
_tmp169;_LL173: if(_tmp16C != Cyc_Core_Not_found)goto _LL175;_LL174: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL172;_LL175:;_LL176:(void)_throw(_tmp16C);_LL172:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp166,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp167,old));}}}}{struct Cyc_Set_Set*_tmp16D=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp16E=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp16F=_tmp161;for(0;_tmp16F != 0;_tmp16F=_tmp16F->tl){
struct Cyc_Absyn_Decl*_tmp170=(struct Cyc_Absyn_Decl*)_tmp16F->hd;struct
_tagged_arr*name;{void*_tmp171=(void*)_tmp170->r;struct Cyc_Absyn_Vardecl*_tmp172;
struct Cyc_Absyn_Fndecl*_tmp173;struct Cyc_Absyn_Aggrdecl*_tmp174;struct Cyc_Absyn_Enumdecl*
_tmp175;struct Cyc_Absyn_Typedefdecl*_tmp176;_LL178: if(*((int*)_tmp171)!= 0)goto
_LL17A;_tmp172=((struct Cyc_Absyn_Var_d_struct*)_tmp171)->f1;_LL179: {struct
_tuple0 _tmp178;struct _tagged_arr*_tmp179;struct _tuple0*_tmp177=_tmp172->name;
_tmp178=*_tmp177;_tmp179=_tmp178.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp179);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,
omit_symbols,_tmp179))name=0;else{name=(struct _tagged_arr*)_tmp179;}goto _LL177;}
_LL17A: if(*((int*)_tmp171)!= 1)goto _LL17C;_tmp173=((struct Cyc_Absyn_Fn_d_struct*)
_tmp171)->f1;_LL17B: {struct _tuple0 _tmp17B;struct _tagged_arr*_tmp17C;struct
_tuple0*_tmp17A=_tmp173->name;_tmp17B=*_tmp17A;_tmp17C=_tmp17B.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp17C);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp17C))name=0;else{name=(struct _tagged_arr*)
_tmp17C;}goto _LL177;}_LL17C: if(*((int*)_tmp171)!= 4)goto _LL17E;_tmp174=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp171)->f1;_LL17D: {struct _tuple0 _tmp17E;struct
_tagged_arr*_tmp17F;struct _tuple0*_tmp17D=_tmp174->name;_tmp17E=*_tmp17D;_tmp17F=
_tmp17E.f2;name=(struct _tagged_arr*)_tmp17F;goto _LL177;}_LL17E: if(*((int*)
_tmp171)!= 6)goto _LL180;_tmp175=((struct Cyc_Absyn_Enum_d_struct*)_tmp171)->f1;
_LL17F: {struct _tuple0 _tmp181;struct _tagged_arr*_tmp182;struct _tuple0*_tmp180=
_tmp175->name;_tmp181=*_tmp180;_tmp182=_tmp181.f2;name=(struct _tagged_arr*)
_tmp182;if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp16D,(struct _tagged_arr*)_check_null(name)): 0)_tmp16E=({struct Cyc_List_List*
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->hd=_tmp170;_tmp183->tl=_tmp16E;
_tmp183;});else{if((unsigned int)_tmp175->fields){struct Cyc_List_List*_tmp184=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp175->fields))->v;for(
0;_tmp184 != 0;_tmp184=_tmp184->tl){struct Cyc_Absyn_Enumfield*_tmp185=(struct Cyc_Absyn_Enumfield*)
_tmp184->hd;struct _tuple0 _tmp187;struct _tagged_arr*_tmp188;struct _tuple0*_tmp186=
_tmp185->name;_tmp187=*_tmp186;_tmp188=_tmp187.f2;if(((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp16D,_tmp188)){_tmp16E=({struct Cyc_List_List*
_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->hd=_tmp170;_tmp189->tl=_tmp16E;
_tmp189;});break;}}}}name=0;goto _LL177;}_LL180: if(*((int*)_tmp171)!= 7)goto
_LL182;_tmp176=((struct Cyc_Absyn_Typedef_d_struct*)_tmp171)->f1;_LL181: {struct
_tuple0 _tmp18B;struct _tagged_arr*_tmp18C;struct _tuple0*_tmp18A=_tmp176->name;
_tmp18B=*_tmp18A;_tmp18C=_tmp18B.f2;name=(struct _tagged_arr*)_tmp18C;goto _LL177;}
_LL182: if(*((int*)_tmp171)!= 2)goto _LL184;_LL183: goto _LL185;_LL184: if(*((int*)
_tmp171)!= 5)goto _LL186;_LL185: goto _LL187;_LL186: if(*((int*)_tmp171)!= 3)goto
_LL188;_LL187: goto _LL189;_LL188: if(*((int*)_tmp171)!= 8)goto _LL18A;_LL189: goto
_LL18B;_LL18A: if(*((int*)_tmp171)!= 9)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(*((
int*)_tmp171)!= 10)goto _LL177;_LL18D: name=0;goto _LL177;_LL177:;}if(name != 0?((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp16D,(
struct _tagged_arr*)_check_null(name)): 0)_tmp16E=({struct Cyc_List_List*_tmp18D=
_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=_tmp170;_tmp18D->tl=_tmp16E;_tmp18D;});}}
_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp16E);maybe=Cyc_Std_fopen(filename,_tag_arr("w",sizeof(unsigned char),2));if(
!((unsigned int)maybe))return 1;out_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);({struct Cyc_Std_String_pa_struct _tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct
_tagged_arr)_tmp143;{void*_tmp18E[1]={& _tmp18F};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\nusing Std;\n",sizeof(unsigned char),26),_tag_arr(
_tmp18E,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp143,_tag_arr("w",sizeof(unsigned char),2));if(!((
unsigned int)maybe))return 1;out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);{
struct _tagged_arr ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1E8;_tmp1E8.tag=
0;_tmp1E8.f1=(struct _tagged_arr)filename;{void*_tmp1E7[1]={& _tmp1E8};Cyc_Std_aprintf(
_tag_arr("_%s_",sizeof(unsigned char),5),_tag_arr(_tmp1E7,sizeof(void*),1));}});{
int _tmp190=0;for(0;_tmp190 < _get_arr_size(ifdefmacro,sizeof(unsigned char));
_tmp190 ++){if(((unsigned char*)ifdefmacro.curr)[_tmp190]== '.'?1:((unsigned char*)
ifdefmacro.curr)[_tmp190]== '/')((unsigned char*)ifdefmacro.curr)[_tmp190]='_';
else{if(((unsigned char*)ifdefmacro.curr)[_tmp190]!= '_'?((unsigned char*)
ifdefmacro.curr)[_tmp190]!= '/': 0)((unsigned char*)ifdefmacro.curr)[_tmp190]=(
unsigned char)toupper((int)((unsigned char*)ifdefmacro.curr)[_tmp190]);}}}({
struct Cyc_Std_String_pa_struct _tmp193;_tmp193.tag=0;_tmp193.f1=(struct
_tagged_arr)ifdefmacro;{struct Cyc_Std_String_pa_struct _tmp192;_tmp192.tag=0;
_tmp192.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp191[2]={& _tmp192,& _tmp193};
Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n#define %s\n",sizeof(
unsigned char),23),_tag_arr(_tmp191,sizeof(void*),2));}}});if(prologue != 0){
struct Cyc_List_List*_tmp194=prologue;for(0;_tmp194 != 0;_tmp194=_tmp194->tl){
struct _tuple11 _tmp196;struct _tagged_arr _tmp197;struct _tagged_arr _tmp198;struct
_tuple11*_tmp195=(struct _tuple11*)_tmp194->hd;_tmp196=*_tmp195;_tmp197=_tmp196.f1;
_tmp198=_tmp196.f2;if(_tmp198.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
_tmp197.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199[0]=_tmp197;_tmp199;})): 0)Cyc_Std_fputs(
_tmp198,out_file);else{({struct Cyc_Std_String_pa_struct _tmp19B;_tmp19B.tag=0;
_tmp19B.f1=(struct _tagged_arr)_tmp197;{void*_tmp19A[1]={& _tmp19B};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(unsigned char),38),
_tag_arr(_tmp19A,sizeof(void*),1));}});}}}({void*_tmp19C[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("\nnamespace Std {\n",sizeof(unsigned char),18),_tag_arr(
_tmp19C,sizeof(void*),0));});{struct Cyc_List_List*_tmp19D=_tmp16E;for(0;_tmp19D
!= 0;_tmp19D=_tmp19D->tl){struct Cyc_Absyn_Decl*_tmp19E=(struct Cyc_Absyn_Decl*)
_tmp19D->hd;int _tmp19F=0;struct _tagged_arr*name;{void*_tmp1A0=(void*)_tmp19E->r;
struct Cyc_Absyn_Vardecl*_tmp1A1;struct Cyc_Absyn_Fndecl*_tmp1A2;struct Cyc_Absyn_Aggrdecl*
_tmp1A3;struct Cyc_Absyn_Enumdecl*_tmp1A4;struct Cyc_Absyn_Typedefdecl*_tmp1A5;
_LL18F: if(*((int*)_tmp1A0)!= 0)goto _LL191;_tmp1A1=((struct Cyc_Absyn_Var_d_struct*)
_tmp1A0)->f1;_LL190: {struct _tuple0 _tmp1A7;struct _tagged_arr*_tmp1A8;struct
_tuple0*_tmp1A6=_tmp1A1->name;_tmp1A7=*_tmp1A6;_tmp1A8=_tmp1A7.f2;name=(struct
_tagged_arr*)_tmp1A8;goto _LL18E;}_LL191: if(*((int*)_tmp1A0)!= 1)goto _LL193;
_tmp1A2=((struct Cyc_Absyn_Fn_d_struct*)_tmp1A0)->f1;_LL192: {struct _tuple0
_tmp1AA;struct _tagged_arr*_tmp1AB;struct _tuple0*_tmp1A9=_tmp1A2->name;_tmp1AA=*
_tmp1A9;_tmp1AB=_tmp1AA.f2;name=(struct _tagged_arr*)_tmp1AB;goto _LL18E;}_LL193:
if(*((int*)_tmp1A0)!= 4)goto _LL195;_tmp1A3=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp1A0)->f1;_LL194: {struct _tuple0 _tmp1AD;struct _tagged_arr*_tmp1AE;struct
_tuple0*_tmp1AC=_tmp1A3->name;_tmp1AD=*_tmp1AC;_tmp1AE=_tmp1AD.f2;name=(struct
_tagged_arr*)_tmp1AE;goto _LL18E;}_LL195: if(*((int*)_tmp1A0)!= 6)goto _LL197;
_tmp1A4=((struct Cyc_Absyn_Enum_d_struct*)_tmp1A0)->f1;_LL196: {struct _tuple0
_tmp1B0;struct _tagged_arr*_tmp1B1;struct _tuple0*_tmp1AF=_tmp1A4->name;_tmp1B0=*
_tmp1AF;_tmp1B1=_tmp1B0.f2;name=(struct _tagged_arr*)_tmp1B1;goto _LL18E;}_LL197:
if(*((int*)_tmp1A0)!= 7)goto _LL199;_tmp1A5=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1A0)->f1;_LL198: {struct _tuple0 _tmp1B3;struct _tagged_arr*_tmp1B4;struct
_tuple0*_tmp1B2=_tmp1A5->name;_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.f2;name=(struct
_tagged_arr*)_tmp1B4;goto _LL18E;}_LL199: if(*((int*)_tmp1A0)!= 2)goto _LL19B;
_LL19A: goto _LL19C;_LL19B: if(*((int*)_tmp1A0)!= 5)goto _LL19D;_LL19C: goto _LL19E;
_LL19D: if(*((int*)_tmp1A0)!= 3)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)
_tmp1A0)!= 8)goto _LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if(*((int*)_tmp1A0)!= 9)goto
_LL1A3;_LL1A2: goto _LL1A4;_LL1A3: if(*((int*)_tmp1A0)!= 10)goto _LL18E;_LL1A4: name=
0;goto _LL18E;_LL18E:;}if(!((unsigned int)name)?!_tmp19F: 0)continue;if((
unsigned int)name){ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1B6;_tmp1B6.tag=
0;_tmp1B6.f1=(struct _tagged_arr)*name;{void*_tmp1B5[1]={& _tmp1B6};Cyc_Std_aprintf(
_tag_arr("_%s_def_",sizeof(unsigned char),9),_tag_arr(_tmp1B5,sizeof(void*),1));}});({
struct Cyc_Std_String_pa_struct _tmp1B8;_tmp1B8.tag=0;_tmp1B8.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp1B7[1]={& _tmp1B8};Cyc_Std_fprintf(out_file,
_tag_arr("#ifndef %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1B7,sizeof(void*),
1));}});({struct Cyc_Std_String_pa_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp1B9[1]={& _tmp1BA};Cyc_Std_fprintf(out_file,
_tag_arr("#define %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1B9,sizeof(void*),
1));}});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp1BB[1];_tmp1BB[0]=
_tmp19E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp1BB,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp1BC[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(_tmp1BC,sizeof(
void*),0));});}else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp1BD[1];
_tmp1BD[0]=_tmp19E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1BD,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}_tmp14A=({
struct Cyc_Std_String_pa_struct _tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=(struct
_tagged_arr)_tmp13F;{struct Cyc_Std_String_pa_struct _tmp1C0;_tmp1C0.tag=0;_tmp1C0.f1=(
struct _tagged_arr)_tmp140;{struct Cyc_Std_String_pa_struct _tmp1BF;_tmp1BF.tag=0;
_tmp1BF.f1=(struct _tagged_arr)_tmp144;{void*_tmp1BE[3]={& _tmp1BF,& _tmp1C0,&
_tmp1C1};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",sizeof(unsigned char),
24),_tag_arr(_tmp1BE,sizeof(void*),3));}}}});if(Cyc_Std_system((struct
_tagged_arr)_tmp14A)!= 0)return 1;maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp140,
_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)maybe))return 1;in_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);_tmp158=Cyc_Lexing_from_file(in_file);{
struct _tuple10*entry2;while((entry2=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp158))!= 0){struct _tuple10 _tmp1C3;struct _tagged_arr
_tmp1C4;struct _tagged_arr*_tmp1C5;struct _tuple10*_tmp1C2=(struct _tuple10*)
_check_null(entry2);_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp16D,
_tmp1C5)){({struct Cyc_Std_String_pa_struct _tmp1C7;_tmp1C7.tag=0;_tmp1C7.f1=(
struct _tagged_arr)*_tmp1C5;{void*_tmp1C6[1]={& _tmp1C7};Cyc_Std_fprintf(out_file,
_tag_arr("#ifndef %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1C6,sizeof(void*),
1));}});({struct Cyc_Std_String_pa_struct _tmp1C9;_tmp1C9.tag=0;_tmp1C9.f1=(struct
_tagged_arr)_tmp1C4;{void*_tmp1C8[1]={& _tmp1C9};Cyc_Std_fprintf(out_file,
_tag_arr("%s\n",sizeof(unsigned char),4),_tag_arr(_tmp1C8,sizeof(void*),1));}});({
void*_tmp1CA[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(
unsigned char),8),_tag_arr(_tmp1CA,sizeof(void*),0));});}}Cyc_Std_fclose(in_file);
Cyc_Std_remove((struct _tagged_arr)_tmp13F);Cyc_Std_remove((struct _tagged_arr)
_tmp140);if(epilogue != 0){struct Cyc_List_List*_tmp1CB=epilogue;for(0;_tmp1CB != 0;
_tmp1CB=_tmp1CB->tl){struct _tuple11 _tmp1CD;struct _tagged_arr _tmp1CE;struct
_tagged_arr _tmp1CF;struct _tuple11*_tmp1CC=(struct _tuple11*)_tmp1CB->hd;_tmp1CD=*
_tmp1CC;_tmp1CE=_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;if(_tmp1CF.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?_tmp1CE.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0[
0]=_tmp1CE;_tmp1D0;})): 0)Cyc_Std_fputs(_tmp1CF,out_file);else{({struct Cyc_Std_String_pa_struct
_tmp1D2;_tmp1D2.tag=0;_tmp1D2.f1=(struct _tagged_arr)_tmp1CE;{void*_tmp1D1[1]={&
_tmp1D2};Cyc_log(_tag_arr("%s is not supported on this platform\n",sizeof(
unsigned char),38),_tag_arr(_tmp1D1,sizeof(void*),1));}});}}}({void*_tmp1D3[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("}\n",sizeof(unsigned char),3),_tag_arr(
_tmp1D3,sizeof(void*),0));});({void*_tmp1D4[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(_tmp1D4,sizeof(void*),0));});
Cyc_Std_fclose(out_file);if(cstubs != 0){out_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_cstubs_file);{struct Cyc_List_List*_tmp1D5=cstubs;for(0;_tmp1D5 != 
0;_tmp1D5=_tmp1D5->tl){struct _tuple11 _tmp1D7;struct _tagged_arr _tmp1D8;struct
_tagged_arr _tmp1D9;struct _tuple11*_tmp1D6=(struct _tuple11*)_tmp1D5->hd;_tmp1D7=*
_tmp1D6;_tmp1D8=_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;if(_tmp1D9.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?_tmp1D8.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[
0]=_tmp1D8;_tmp1DA;})): 0)Cyc_Std_fputs(_tmp1D9,out_file);}}}out_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file);({struct Cyc_Std_String_pa_struct _tmp1DC;_tmp1DC.tag=
0;_tmp1DC.f1=(struct _tagged_arr)_tmp143;{void*_tmp1DB[1]={& _tmp1DC};Cyc_Std_fprintf(
out_file,_tag_arr("#include <%s>\n\n",sizeof(unsigned char),16),_tag_arr(_tmp1DB,
sizeof(void*),1));}});if(cycstubs != 0){out_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file);({void*_tmp1DD[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("namespace Std {\n",sizeof(unsigned char),17),_tag_arr(_tmp1DD,sizeof(
void*),0));});{struct Cyc_List_List*_tmp1DE=cycstubs;for(0;_tmp1DE != 0;_tmp1DE=
_tmp1DE->tl){struct _tuple11 _tmp1E0;struct _tagged_arr _tmp1E1;struct _tagged_arr
_tmp1E2;struct _tuple11*_tmp1DF=(struct _tuple11*)_tmp1DE->hd;_tmp1E0=*_tmp1DF;
_tmp1E1=_tmp1E0.f1;_tmp1E2=_tmp1E0.f2;if(_tmp1E2.curr != ((struct _tagged_arr)
_tag_arr(0,0,0)).curr?_tmp1E1.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[
0]=_tmp1E1;_tmp1E3;})): 0)Cyc_Std_fputs(_tmp1E2,out_file);}}({void*_tmp1E4[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("}\n\n",sizeof(unsigned char),4),_tag_arr(
_tmp1E4,sizeof(void*),0));});}if(Cyc_Std_chdir((struct _tagged_arr)_tmp138)){({
struct Cyc_Std_String_pa_struct _tmp1E6;_tmp1E6.tag=0;_tmp1E6.f1=(struct
_tagged_arr)_tmp138;{void*_tmp1E5[1]={& _tmp1E6};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n",sizeof(unsigned char),41),
_tag_arr(_tmp1E5,sizeof(void*),1));}});return 1;}return 0;}}}}}}}}}}void Cyc_process_specfile(
struct _tagged_arr file,struct _tagged_arr dir){struct Cyc_Std___cycFILE*_tmp1F4=Cyc_Std_fopen(
file,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)_tmp1F4)){({
struct Cyc_Std_String_pa_struct _tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct
_tagged_arr)file;{void*_tmp1F5[1]={& _tmp1F6};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not open %s\n",sizeof(unsigned char),26),_tag_arr(_tmp1F5,
sizeof(void*),1));}});return;}{struct Cyc_Std___cycFILE*_tmp1F7=(struct Cyc_Std___cycFILE*)
_check_null(_tmp1F4);struct Cyc_Lexing_lexbuf*_tmp1F8=Cyc_Lexing_from_file(
_tmp1F7);struct _tuple12*entry;while((entry=((struct _tuple12*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_spec)(_tmp1F8))!= 0){struct _tuple12 _tmp1FA;struct _tagged_arr _tmp1FB;
struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*
_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_List_List*
_tmp201;struct _tuple12*_tmp1F9=(struct _tuple12*)_check_null(entry);_tmp1FA=*
_tmp1F9;_tmp1FB=_tmp1FA.f1;_tmp1FC=_tmp1FA.f2;_tmp1FD=_tmp1FA.f3;_tmp1FE=_tmp1FA.f4;
_tmp1FF=_tmp1FA.f5;_tmp200=_tmp1FA.f6;_tmp201=_tmp1FA.f7;Cyc_process_file(
_tmp1FB,dir,_tmp1FC,_tmp1FD,_tmp1FE,_tmp1FF,_tmp200,_tmp201);}Cyc_Std_fclose(
_tmp1F7);}}int Cyc_getsize(struct _tagged_arr dir,struct _tagged_arr includes,struct
_tagged_arr type){struct _tagged_arr _tmp202=Cyc_Filename_concat(dir,_tag_arr("getsize.c",
sizeof(unsigned char),10));struct _tagged_arr _tmp203=Cyc_Filename_concat(dir,
_tag_arr("getsize",sizeof(unsigned char),8));struct _tagged_arr _tmp204=Cyc_Filename_concat(
dir,_tag_arr("getsize.out",sizeof(unsigned char),12));struct Cyc_Std___cycFILE*
_tmp205=Cyc_Std_fopen((struct _tagged_arr)_tmp202,_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)_tmp205)){({struct Cyc_Std_String_pa_struct
_tmp207;_tmp207.tag=0;_tmp207.f1=(struct _tagged_arr)_tmp202;{void*_tmp206[1]={&
_tmp207};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create %s\n",
sizeof(unsigned char),28),_tag_arr(_tmp206,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp209;_tmp209.tag=0;_tmp209.f1=(struct _tagged_arr)type;{void*_tmp208[1]={&
_tmp209};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(
unsigned char),40),_tag_arr(_tmp208,sizeof(void*),1));}});return - 1;}({struct Cyc_Std_String_pa_struct
_tmp20E;_tmp20E.tag=0;_tmp20E.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp20D;_tmp20D.tag=0;_tmp20D.f1=(struct _tagged_arr)type;{struct Cyc_Std_String_pa_struct
_tmp20C;_tmp20C.tag=0;_tmp20C.f1=(struct _tagged_arr)_tmp204;{struct Cyc_Std_String_pa_struct
_tmp20B;_tmp20B.tag=0;_tmp20B.f1=(struct _tagged_arr)includes;{void*_tmp20A[4]={&
_tmp20B,& _tmp20C,& _tmp20D,& _tmp20E};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(_tmp205),_tag_arr("%s\n#include <stdio.h>\nint main() {\n  FILE *f = fopen(\"%s\",\"w\");\n  fprintf(f,\"%%d\\n\",sizeof(%s));\n  fclose(f);\n  return (int)sizeof(%s);\n}\n",
sizeof(unsigned char),139),_tag_arr(_tmp20A,sizeof(void*),4));}}}}});Cyc_Std_fclose((
struct Cyc_Std___cycFILE*)_check_null(_tmp205));{struct _tagged_arr _tmp20F=
Cstring_to_string(Ccomp);struct _tagged_arr _tmp210=({struct Cyc_Std_String_pa_struct
_tmp225;_tmp225.tag=0;_tmp225.f1=(struct _tagged_arr)_tmp202;{struct Cyc_Std_String_pa_struct
_tmp224;_tmp224.tag=0;_tmp224.f1=(struct _tagged_arr)_tmp203;{struct Cyc_Std_String_pa_struct
_tmp223;_tmp223.tag=0;_tmp223.f1=(struct _tagged_arr)_tmp20F;{void*_tmp222[3]={&
_tmp223,& _tmp224,& _tmp225};Cyc_Std_aprintf(_tag_arr("%s -o %s -x c %s",sizeof(
unsigned char),17),_tag_arr(_tmp222,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp210)!= 0){({struct Cyc_Std_String_pa_struct _tmp212;_tmp212.tag=
0;_tmp212.f1=(struct _tagged_arr)type;{void*_tmp211[1]={& _tmp212};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Warning: could not find the size of %s; continuing anyway\n",
sizeof(unsigned char),59),_tag_arr(_tmp211,sizeof(void*),1));}});({struct Cyc_Std_String_pa_struct
_tmp214;_tmp214.tag=0;_tmp214.f1=(struct _tagged_arr)type;{void*_tmp213[1]={&
_tmp214};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",sizeof(
unsigned char),40),_tag_arr(_tmp213,sizeof(void*),1));}});return - 1;}{int _tmp215=
Cyc_Std_system((struct _tagged_arr)_tmp203);({struct Cyc_Std_Int_pa_struct _tmp218;
_tmp218.tag=1;_tmp218.f1=(int)((unsigned int)_tmp215);{struct Cyc_Std_String_pa_struct
_tmp217;_tmp217.tag=0;_tmp217.f1=(struct _tagged_arr)type;{void*_tmp216[2]={&
_tmp217,& _tmp218};Cyc_log(_tag_arr("size of %s returned by system is %d\n",
sizeof(unsigned char),37),_tag_arr(_tmp216,sizeof(void*),2));}}});{struct Cyc_Std___cycFILE*
_tmp219=Cyc_Std_fopen((struct _tagged_arr)_tmp204,_tag_arr("r",sizeof(
unsigned char),2));int w=- 1;if(({struct Cyc_Std_IntPtr_sa_struct _tmp21B;_tmp21B.tag=
2;_tmp21B.f1=& w;{void*_tmp21A[1]={& _tmp21B};Cyc_Std_fscanf((struct Cyc_Std___cycFILE*)
_check_null(_tmp219),_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp21A,
sizeof(void*),1));}})!= 1){({struct Cyc_Std_String_pa_struct _tmp21D;_tmp21D.tag=0;
_tmp21D.f1=(struct _tagged_arr)type;{void*_tmp21C[1]={& _tmp21D};Cyc_log(_tag_arr("Warning: could not find the size of %s\n",
sizeof(unsigned char),40),_tag_arr(_tmp21C,sizeof(void*),1));}});return - 1;}({
struct Cyc_Std_Int_pa_struct _tmp221;_tmp221.tag=1;_tmp221.f1=(int)((unsigned int)
w);{struct Cyc_Std_String_pa_struct _tmp220;_tmp220.tag=0;_tmp220.f1=(struct
_tagged_arr)_tmp204;{struct Cyc_Std_String_pa_struct _tmp21F;_tmp21F.tag=0;_tmp21F.f1=(
struct _tagged_arr)type;{void*_tmp21E[3]={& _tmp21F,& _tmp220,& _tmp221};Cyc_log(
_tag_arr("size of %s read from file %s is %d\n",sizeof(unsigned char),36),
_tag_arr(_tmp21E,sizeof(void*),3));}}}});Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(_tmp219));Cyc_Std_remove((struct _tagged_arr)_tmp204);Cyc_Std_remove((
struct _tagged_arr)_tmp202);Cyc_Std_remove((struct _tagged_arr)_tmp203);return w;}}}}
void GC_blacklist_warn_clear();int Cyc_main(int argc,struct _tagged_arr argv){
GC_blacklist_warn_clear();if(argc < 3){({void*_tmp226[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Usage: buildlib <dir> <spec>\n where <dir> the output directory and <spec> is a .cys specification file.\n",
sizeof(unsigned char),105),_tag_arr(_tmp226,sizeof(void*),0));});return 1;}{
struct _tagged_arr _tmp227=*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),1));if(Cyc_force_directory_prefixes((struct _tagged_arr)
_tmp227)?1: Cyc_force_directory((struct _tagged_arr)_tmp227)){({struct Cyc_Std_String_pa_struct
_tmp229;_tmp229.tag=0;_tmp229.f1=(struct _tagged_arr)_tmp227;{void*_tmp228[1]={&
_tmp229};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create directory %s\n",
sizeof(unsigned char),38),_tag_arr(_tmp228,sizeof(void*),1));}});return 1;}Cyc_log_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp227,
_tag_arr("BUILDLIB.LOG",sizeof(unsigned char),13)),_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)Cyc_log_file)){({struct Cyc_Std_String_pa_struct
_tmp22B;_tmp22B.tag=0;_tmp22B.f1=(struct _tagged_arr)_tmp227;{void*_tmp22A[1]={&
_tmp22B};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create log file in directory %s\n",
sizeof(unsigned char),50),_tag_arr(_tmp22A,sizeof(void*),1));}});return 1;}Cyc_cstubs_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp227,
_tag_arr("cstubs.c",sizeof(unsigned char),9)),_tag_arr("w",sizeof(unsigned char),
2));if(!((unsigned int)Cyc_cstubs_file)){({struct Cyc_Std_String_pa_struct _tmp22D;
_tmp22D.tag=0;_tmp22D.f1=(struct _tagged_arr)_tmp227;{void*_tmp22C[1]={& _tmp22D};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create cstubs.c in directory %s\n",
sizeof(unsigned char),50),_tag_arr(_tmp22C,sizeof(void*),1));}});return 1;}Cyc_cycstubs_file=
Cyc_Std_fopen((struct _tagged_arr)Cyc_Filename_concat((struct _tagged_arr)_tmp227,
_tag_arr("cycstubs.cyc",sizeof(unsigned char),13)),_tag_arr("w",sizeof(
unsigned char),2));if(!((unsigned int)Cyc_cycstubs_file)){({struct Cyc_Std_String_pa_struct
_tmp22F;_tmp22F.tag=0;_tmp22F.f1=(struct _tagged_arr)_tmp227;{void*_tmp22E[1]={&
_tmp22F};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create cycstubs.c in directory %s\n",
sizeof(unsigned char),52),_tag_arr(_tmp22E,sizeof(void*),1));}});return 1;}({void*
_tmp230[0]={};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_cycstubs_file),
_tag_arr("#include <core.h>\nusing Core;\n\n",sizeof(unsigned char),32),_tag_arr(
_tmp230,sizeof(void*),0));});Cyc_sizeof_unsignedlongint=Cyc_getsize((struct
_tagged_arr)_tmp227,_tag_arr("",sizeof(unsigned char),1),_tag_arr("unsigned long int",
sizeof(unsigned char),18));Cyc_sizeof_unsignedshortint=Cyc_getsize((struct
_tagged_arr)_tmp227,_tag_arr("",sizeof(unsigned char),1),_tag_arr("unsigned short int",
sizeof(unsigned char),19));Cyc_sizeof_shortint=Cyc_getsize((struct _tagged_arr)
_tmp227,_tag_arr("",sizeof(unsigned char),1),_tag_arr("short int",sizeof(
unsigned char),10));Cyc_sizeof_int=Cyc_getsize((struct _tagged_arr)_tmp227,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("int",sizeof(unsigned char),4));
Cyc_sizeof_short=Cyc_getsize((struct _tagged_arr)_tmp227,_tag_arr("",sizeof(
unsigned char),1),_tag_arr("short",sizeof(unsigned char),6));Cyc_sizeof_fdmask=
Cyc_getsize((struct _tagged_arr)_tmp227,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(unsigned char),47),_tag_arr("fd_mask",sizeof(unsigned char),8));Cyc_sizeof___fdmask=
Cyc_getsize((struct _tagged_arr)_tmp227,_tag_arr("#include <sys/types.h>\n#include <sys/select.h>",
sizeof(unsigned char),47),_tag_arr("__fd_mask",sizeof(unsigned char),10));Cyc_sizeof_uint32=
Cyc_getsize((struct _tagged_arr)_tmp227,_tag_arr("#include <sys/types.h>",sizeof(
unsigned char),23),_tag_arr("__uint32_t",sizeof(unsigned char),11));Cyc_sizeof_sockaddr=
Cyc_getsize((struct _tagged_arr)_tmp227,_tag_arr("#include <sys/types.h>\n#include <sys/socket.h>",
sizeof(unsigned char),47),_tag_arr("struct sockaddr",sizeof(unsigned char),16));
Cyc_sizeof_inport=Cyc_getsize((struct _tagged_arr)_tmp227,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(unsigned char),47),_tag_arr("in_port_t",sizeof(unsigned char),10));Cyc_sizeof_inaddr=
Cyc_getsize((struct _tagged_arr)_tmp227,_tag_arr("#include <sys/types.h>\n#include <netinet/in.h>",
sizeof(unsigned char),47),_tag_arr("struct in_addr",sizeof(unsigned char),15));{
int i=2;for(0;i < argc;i ++){Cyc_process_specfile((struct _tagged_arr)((struct
_tagged_arr*)argv.curr)[i],(struct _tagged_arr)_tmp227);}}Cyc_Std_fclose((struct
Cyc_Std___cycFILE*)_check_null(Cyc_log_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cstubs_file));Cyc_Std_fclose((struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file));return 0;}}
