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
 void exit(int);struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{
void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct _tagged_arr Cstring_to_string(unsigned char*);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;int Cyc_Std_remove(
struct _tagged_arr);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*);int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fopen(struct
_tagged_arr __filename,struct _tagged_arr __modes);int Cyc_Std_fputc(int __c,struct
Cyc_Std___cycFILE*__stream);int Cyc_Std_fputs(struct _tagged_arr __s,struct Cyc_Std___cycFILE*
__stream);extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
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
int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;};struct Cyc_Std_stat_t{
short st_dev;unsigned int st_ino;int st_mode;unsigned short st_nlink;unsigned short
st_uid;unsigned short st_gid;short st_rdev;int st_size;int st_atime;int st_spare1;int
st_mtime;int st_spare2;int st_ctime;int st_spare3;int st_blksize;int st_blocks;int
st_spare4[2];};int Cyc_Std_mkdir(struct _tagged_arr pathname,int mode);struct Cyc_Std_flock{
short l_type;int l_start;short l_whence;int l_len;int l_pid;};struct Cyc_Std_Flock_struct{
int tag;struct Cyc_Std_flock*f1;};struct Cyc_Std_Long_struct{int tag;int f1;};int Cyc_Std_open(
struct _tagged_arr,int,struct _tagged_arr);struct Cyc_Std_timeval{int tv_sec;int
tv_usec;};struct Cyc_Std_timezone{int tz_minuteswest;int tz_dsttime;};struct Cyc_Std_itimerval{
struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval it_value;};typedef struct{
unsigned int __val[64];}Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t Cyc_Std_sigset_t;
struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;struct Cyc_Std_option{
struct _tagged_arr name;int has_arg;int*flag;int val;};int close(int);int Cyc_Std_access(
struct _tagged_arr,int);int Cyc_Std_chdir(struct _tagged_arr);int Cyc_Std_chown(
struct _tagged_arr,unsigned short,unsigned short);struct _tagged_arr Cyc_Std_getcwd(
struct _tagged_arr buf,unsigned int size);int Cyc_Std_execl(struct _tagged_arr path,
struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execlp(struct _tagged_arr
file,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execve(struct
_tagged_arr filename,struct _tagged_arr argv,struct _tagged_arr envp);int Cyc_Std_link(
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
0;int Cyc_parens_to_match=0;int Cyc_numdef=0;const int Cyc_lex_base[515]=(const int[
515]){0,0,75,192,305,285,310,166,311,91,27,384,28,503,617,695,288,338,93,- 3,0,- 1,
- 2,- 8,- 3,1,- 2,586,- 4,2,166,- 5,308,777,333,- 6,- 7,- 4,16,- 3,29,11,815,- 3,890,13,- 16,
223,12,- 2,213,22,26,28,24,28,58,56,65,52,70,320,1004,1119,357,93,83,90,112,96,
106,115,124,115,126,113,382,410,152,180,209,231,231,415,505,231,229,227,237,224,
241,498,1233,1348,510,- 8,231,230,251,238,238,238,270,515,1462,1577,- 11,530,- 12,
252,273,633,1691,1806,- 9,635,- 10,664,- 13,391,646,648,1883,1960,2037,2118,416,433,
653,2193,279,289,291,289,286,297,0,13,4,2274,5,682,2282,2347,800,49,465,6,2308,7,
970,2370,2408,978,- 32,318,418,304,307,293,315,307,319,677,1035,336,344,335,405,
366,362,405,419,416,428,431,682,1030,423,432,427,431,430,688,1145,438,447,1235,
454,462,809,1147,485,481,1350,518,517,515,551,535,826,1259,542,543,561,561,572,
570,571,561,1464,584,584,582,584,574,1579,579,578,1693,2470,589,584,1808,607,591,
2332,596,596,595,617,597,2472,620,610,627,610,619,2480,636,639,635,631,626,10,14,
659,639,2482,661,648,689,673,682,2487,1156,708,674,710,717,728,739,741,741,745,
755,762,746,- 30,798,802,798,808,816,853,- 25,833,849,842,838,849,870,872,888,889,
- 19,876,900,900,939,940,- 27,932,930,925,937,936,952,931,953,956,955,959,950,950,
- 29,959,979,982,992,1000,983,1015,1032,1039,1040,1,4,6,1041,1044,1032,1031,1043,
1043,1051,1052,2,52,1053,1054,2272,20,21,1080,1086,1050,1048,1063,1069,1070,1125,
1137,1139,- 24,1086,1087,1142,1169,1170,- 21,1117,1118,1174,1206,1208,- 22,1156,
1161,1217,1218,1219,- 20,1167,1168,1223,1225,1230,- 23,1178,1179,1469,- 31,1178,
1180,1177,1176,1175,1181,1183,- 18,1186,1187,1185,1198,4,- 15,1216,1217,1215,1228,
1297,- 17,1265,1268,1258,1259,1257,1270,1338,- 14,1263,1264,1262,1276,1344,- 7,- 8,8,
1374,2488,9,1435,2496,2561,1545,1489,- 49,1377,- 2,1316,- 4,1317,1442,2090,1318,
1349,1350,1659,1320,2588,2631,1327,1379,1329,1332,2701,1346,1383,- 36,- 42,- 37,
2776,- 28,1381,- 40,1415,- 45,- 39,- 48,2851,2880,1777,1407,1417,2598,2890,2920,2669,
2861,2953,2984,3022,1437,1450,3092,3130,1506,1527,1519,1529,1521,1531,- 6,- 34,
1435,3062,- 47,- 46,- 33,1452,3170,1456,1458,1678,1461,1465,1467,1469,1481,1514,
1516,1517,1562,3243,3327,2519,1563,- 41,- 38,- 35,- 26,1832,3409,1576,3492,1569,15,
1512,1512,1512,1510,1506,1516,1586};const int Cyc_lex_backtrk[515]=(const int[515]){
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,
- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,14,15,- 1,15,15,15,15,15,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,13,14,2,4,4,- 1,1,1,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,
6,5,6,5,2,8,3,5,- 1,6,5,- 1,31,31,31,31,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,27,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,
2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,
42,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[515]=(const int[515]){- 1,- 1,- 1,
417,406,154,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,
0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,
- 1,0,- 1,0,119,- 1,- 1,- 1,- 1,- 1,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,-
1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,0,- 1,0,- 1,0,- 1,- 1,480,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,
- 1,0,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,0,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3749]=(const int[
3749]){0,0,0,0,0,0,0,0,0,0,22,19,28,507,19,28,19,28,95,19,45,45,45,45,45,22,45,0,
0,0,0,0,21,317,328,508,386,22,- 1,- 1,22,- 1,- 1,45,318,45,319,22,505,505,505,505,
505,505,505,505,505,505,31,119,22,245,130,40,246,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,31,329,
363,357,505,146,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,503,503,503,503,503,503,503,503,503,503,
137,85,20,78,65,55,56,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,503,503,503,503,503,503,503,503,503,57,58,59,60,503,61,503,503,503,
503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,66,67,37,418,419,418,418,419,39,22,68,69,70,71,72,145,34,34,34,34,34,
34,34,34,73,74,418,420,421,75,76,422,423,424,120,120,425,426,120,427,428,429,430,
431,431,431,431,431,431,431,431,431,432,79,433,434,435,120,19,436,436,436,436,
436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,436,
436,436,19,- 1,- 1,437,436,80,436,436,436,436,436,436,436,436,436,436,436,436,436,
436,436,436,436,436,436,436,436,436,436,436,436,436,407,438,155,408,22,24,24,37,
147,138,96,81,156,97,62,62,82,83,62,98,38,86,87,88,409,89,90,91,117,26,26,109,21,
24,21,99,25,62,100,101,102,33,33,33,33,33,33,33,33,33,33,64,64,103,110,64,111,26,
33,33,33,33,33,33,131,157,35,35,35,35,35,35,35,35,64,132,77,77,133,134,77,135,
410,136,255,158,- 1,148,139,- 1,33,33,33,33,33,33,39,159,22,77,160,161,393,162,77,
77,163,21,77,84,84,- 1,394,84,- 1,27,239,129,129,129,129,129,129,129,129,129,129,
77,- 1,233,224,- 1,84,22,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,- 1,170,171,- 1,129,35,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,92,92,195,172,92,121,121,84,84,121,373,84,94,94,196,173,94,104,104,
174,374,104,175,92,176,375,28,188,121,180,84,181,107,107,21,94,107,21,182,183,
104,186,122,123,122,122,122,122,122,122,122,122,122,122,114,107,187,189,190,21,
21,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,21,193,28,194,122,29,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,47,47,19,
217,47,197,198,95,30,30,30,30,30,30,30,30,112,112,115,115,112,- 1,115,47,199,200,
211,108,204,120,120,128,128,120,205,128,128,128,48,112,128,115,206,207,208,209,-
1,117,117,210,49,117,120,212,128,213,214,31,215,128,163,163,216,- 1,163,177,177,
218,219,177,117,184,184,222,223,184,225,227,41,41,32,228,41,163,229,230,231,232,
177,234,50,164,51,235,184,- 1,52,226,236,237,238,41,53,54,144,144,144,144,144,144,
144,144,240,249,242,243,244,42,42,42,42,42,42,42,42,42,42,241,247,248,250,251,
116,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,252,118,253,254,42,292,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,191,191,43,286,191,306,307,33,33,33,33,33,33,33,33,33,
33,201,201,276,269,201,262,191,33,33,33,33,33,33,35,35,35,35,35,35,35,35,263,264,
201,265,266,267,268,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,270,271,272,
273,44,274,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,275,277,278,28,279,280,281,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,282,283,284,285,
44,287,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,62,62,288,289,62,153,153,153,153,153,153,153,153,35,35,35,35,35,35,35,35,
290,291,62,293,294,177,177,295,296,177,164,164,297,298,164,299,300,301,63,63,63,
63,63,63,63,63,63,63,177,302,303,304,305,164,369,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,308,309,310,311,63,312,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,31,64,64,
165,313,64,314,315,316,320,166,178,321,167,322,323,324,325,179,326,327,330,331,
168,64,169,351,184,184,191,191,184,345,191,337,338,339,340,341,342,63,63,63,63,
63,63,63,63,63,63,184,343,191,344,346,347,348,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,349,350,352,353,63,354,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,92,92,187,187,
92,355,187,356,185,358,192,256,257,258,359,259,360,361,362,260,364,365,366,92,
367,187,201,201,261,368,201,370,371,387,116,381,376,377,378,93,93,93,93,93,93,93,
93,93,93,201,379,380,382,383,384,385,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,388,389,390,391,93,392,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,36,94,94,194,194,94,401,
194,202,395,396,397,398,399,400,402,403,404,203,405,46,501,500,479,94,474,194,
416,416,449,502,416,502,502,275,114,305,443,444,268,93,93,93,93,93,93,93,93,93,
93,416,441,21,502,372,154,478,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,446,291,447,445,93,439,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,104,104,210,210,104,448,210,371,
371,498,477,371,415,415,415,415,415,415,415,415,344,95,95,104,362,210,285,416,
416,350,371,416,499,380,108,106,440,108,372,105,105,105,105,105,105,105,105,105,
105,118,416,35,21,95,95,35,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,400,35,386,46,105,35,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,106,107,107,216,216,107,31,216,31,31,31,31,31,31,31,31,392,356,
31,36,36,473,473,21,107,21,216,509,510,511,512,513,514,21,0,118,0,0,31,0,105,105,
105,105,105,105,105,105,105,105,31,36,36,473,473,0,0,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,0,0,
0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,112,112,220,220,112,475,220,476,476,476,476,
476,476,476,476,476,476,356,0,0,0,0,0,112,0,220,496,496,496,496,496,496,496,496,
95,0,0,0,0,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,114,115,115,223,223,115,455,223,
455,0,0,456,456,456,456,456,456,456,456,456,456,0,0,0,115,0,223,502,0,502,502,0,
0,0,0,106,0,0,0,0,113,113,113,113,113,113,113,113,113,113,502,0,0,0,0,0,0,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,0,0,0,0,113,0,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,124,124,124,124,124,
124,124,124,124,124,124,124,21,0,0,0,0,0,0,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,124,
0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,125,124,124,124,124,124,124,124,124,124,124,21,0,
0,0,0,0,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,0,0,0,0,124,0,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,21,0,0,0,0,- 1,0,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,- 1,- 1,0,- 1,124,0,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,0,0,0,0,125,125,125,125,125,125,125,
125,125,125,125,125,127,0,0,0,0,0,481,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,0,0,0,0,125,0,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,129,129,129,129,129,129,129,129,129,129,0,0,0,0,0,0,0,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,28,0,0,140,129,0,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,0,0,28,0,0,149,
141,141,141,141,141,141,141,141,143,143,143,143,143,143,143,143,143,143,332,226,
226,0,333,226,- 1,143,143,143,143,143,143,334,0,335,150,150,150,150,150,150,150,
150,226,0,0,0,0,0,0,31,0,36,- 1,0,0,0,0,143,143,143,143,143,143,0,0,0,0,0,0,336,0,
0,142,143,143,143,143,143,143,143,143,143,143,31,0,0,0,0,0,0,143,143,143,143,143,
143,152,152,152,152,152,152,152,152,152,152,151,0,0,0,0,0,0,152,152,152,152,152,
152,0,0,0,143,143,143,143,143,143,0,0,0,0,0,0,152,152,152,152,152,152,152,152,
152,152,0,152,152,152,152,152,152,152,152,152,152,152,152,220,220,232,232,220,0,
232,0,0,0,238,238,248,248,238,0,248,254,254,19,0,254,411,220,0,232,152,152,152,
152,152,152,95,238,35,248,0,0,0,0,254,0,19,0,31,0,0,0,0,28,0,0,0,0,0,0,0,412,412,
412,412,412,412,412,412,414,414,414,414,414,414,414,414,414,414,0,0,0,0,356,0,0,
414,414,414,414,414,414,497,497,497,497,497,497,497,497,221,0,0,0,0,0,0,0,0,0,28,
0,0,0,0,0,0,0,414,414,414,414,414,414,0,0,0,0,0,0,0,0,0,413,414,414,414,414,414,
414,414,414,414,414,0,0,0,0,0,0,0,414,414,414,414,414,414,0,0,450,0,461,461,461,
461,461,461,461,461,462,462,456,456,456,456,456,456,456,456,456,456,0,452,414,
414,414,414,414,414,463,0,0,0,0,0,0,0,0,464,0,0,465,450,0,451,451,451,451,451,
451,451,451,451,451,452,0,0,0,0,0,0,463,0,0,0,452,0,0,0,0,464,0,453,465,0,0,0,
459,0,459,0,454,460,460,460,460,460,460,460,460,460,460,0,0,0,0,0,452,0,0,0,0,0,
0,453,0,0,0,0,0,0,0,0,454,442,442,442,442,442,442,442,442,442,442,0,0,0,0,0,0,0,
442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,0,0,0,0,442,0,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,0,0,0,0,0,0,0,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,0,0,0,0,442,0,
442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,442,
442,442,442,442,442,442,457,457,457,457,457,457,457,457,457,457,460,460,460,460,
460,460,460,460,460,460,0,458,116,0,0,0,0,450,116,451,451,451,451,451,451,451,
451,451,451,456,456,456,456,456,456,456,456,456,456,0,452,0,0,458,116,0,0,453,0,
0,116,114,0,0,0,0,454,114,0,457,457,457,457,457,457,457,457,457,457,0,0,0,452,0,
0,0,0,0,0,453,458,116,0,114,0,0,0,116,454,114,0,0,460,460,460,460,460,460,460,
460,460,460,0,0,0,0,0,0,0,0,0,0,458,116,116,0,0,0,0,116,116,450,0,461,461,461,
461,461,461,461,461,462,462,0,0,0,0,0,0,0,0,0,0,0,452,0,116,0,0,0,0,471,116,0,0,
0,0,0,0,450,472,462,462,462,462,462,462,462,462,462,462,0,0,0,0,0,452,0,0,0,0,0,
452,471,0,0,0,0,0,469,0,0,472,0,0,0,0,0,470,0,0,476,476,476,476,476,476,476,476,
476,476,0,0,0,452,0,0,0,0,0,0,469,458,116,0,0,0,0,0,116,470,466,466,466,466,466,
466,466,466,466,466,0,0,0,0,0,0,0,466,466,466,466,466,466,458,116,0,0,0,0,0,116,
0,0,0,0,0,0,0,466,466,466,466,466,466,466,466,466,466,0,466,466,466,466,466,466,
466,466,466,466,466,466,0,0,0,482,0,467,0,0,483,0,0,0,0,0,468,0,0,484,484,484,
484,484,484,484,484,0,466,466,466,466,466,466,485,0,0,0,0,467,0,0,0,0,0,0,0,0,
468,0,0,0,0,0,0,0,0,0,0,0,0,0,0,486,0,0,0,0,487,488,0,0,0,489,0,0,0,0,0,0,0,490,
0,0,0,491,0,492,0,493,0,494,495,495,495,495,495,495,495,495,495,495,0,0,0,0,0,0,
0,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,495,495,495,495,495,495,0,0,0,0,0,0,495,495,495,495,495,495,495,495,495,495,
495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,368,0,0,0,0,0,0,
0,0,495,495,495,495,495,495,495,495,495,495,0,0,0,0,0,0,0,495,495,495,495,495,
495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,0,0,0,0,0,0,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,495,
495,495,495,495,495,495,495,495,495,495,22,0,0,504,0,0,0,503,503,503,503,503,503,
503,503,503,503,0,0,0,0,0,0,0,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,503,503,503,503,503,503,503,503,503,503,503,0,0,0,0,503,0,503,503,
503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,
503,503,503,503,506,0,0,0,0,0,0,0,505,505,505,505,505,505,505,505,505,505,0,0,0,
0,0,0,0,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,0,0,0,0,505,0,505,505,505,505,505,505,505,505,
505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,505,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};const int Cyc_lex_check[3749]=(const int[3749]){- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,0,25,29,0,138,140,147,149,408,411,41,41,45,45,41,507,45,- 1,- 1,- 1,
- 1,- 1,136,316,327,0,385,10,12,40,10,12,40,41,317,45,318,20,1,1,1,1,1,1,1,1,1,1,
38,48,137,244,10,38,245,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,145,
328,332,333,1,145,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,
2,2,2,2,2,9,51,18,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,65,66,7,3,
3,3,3,3,7,7,67,68,69,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,75,3,3,3,47,
47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,4,3,5,4,5,6,8,16,6,8,50,80,5,50,61,61,81,82,61,50,16,85,86,87,4,88,
89,90,96,6,8,97,9,17,18,98,17,61,99,100,101,32,32,32,32,32,32,32,32,32,32,64,64,
102,109,64,110,17,32,32,32,32,32,32,130,5,34,34,34,34,34,34,34,34,64,131,76,76,
132,133,76,134,4,135,157,5,119,6,8,119,32,32,32,32,32,32,16,158,16,76,159,160,
155,161,77,77,162,7,77,83,83,126,155,83,126,17,165,11,11,11,11,11,11,11,11,11,11,
77,127,166,167,127,83,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,146,169,170,146,11,64,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,168,171,91,13,13,84,84,13,156,84,
94,94,168,172,94,103,103,173,156,103,174,91,175,156,77,178,13,179,84,180,107,107,
5,94,107,16,181,182,103,185,13,13,13,13,13,13,13,13,13,13,13,13,4,107,186,188,
189,6,8,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,17,192,27,193,13,27,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,14,14,84,195,14,196,197,94,27,27,27,27,27,27,27,27,111,
111,115,115,111,119,115,14,198,199,202,107,203,120,120,121,121,120,204,121,128,
128,14,111,128,115,205,206,207,208,126,117,117,209,14,117,120,211,121,212,213,27,
214,128,163,163,215,127,163,176,176,217,218,176,117,183,183,221,222,183,224,225,
15,15,27,227,15,163,228,229,230,231,176,233,14,163,14,234,183,146,14,224,235,236,
237,15,14,14,141,141,141,141,141,141,141,141,239,240,241,242,243,15,15,15,15,15,
15,15,15,15,15,239,246,247,249,250,115,13,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,251,117,252,253,15,257,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,190,190,15,258,190,
256,256,33,33,33,33,33,33,33,33,33,33,200,200,259,260,200,261,190,33,33,33,33,33,
33,144,144,144,144,144,144,144,144,262,263,200,264,265,266,267,42,42,42,42,42,42,
42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,269,270,271,272,42,273,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,
274,276,277,15,278,279,280,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,281,282,283,284,44,286,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,62,62,287,288,62,150,150,150,150,
150,150,150,150,153,153,153,153,153,153,153,153,289,290,62,292,293,177,177,294,
295,177,164,164,296,297,164,298,299,300,62,62,62,62,62,62,62,62,62,62,177,301,
302,303,304,164,306,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,307,308,309,310,62,311,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,63,63,164,312,63,313,314,315,319,164,
177,320,164,321,322,323,324,177,325,326,329,330,164,63,164,334,184,184,191,191,
184,335,191,336,337,338,339,340,341,63,63,63,63,63,63,63,63,63,63,184,342,191,
343,345,346,347,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,348,349,351,352,63,353,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,92,92,187,187,92,354,187,355,184,357,191,255,
255,255,358,255,359,360,361,255,363,364,365,92,366,187,201,201,255,367,201,369,
370,373,187,374,375,376,377,92,92,92,92,92,92,92,92,92,92,201,378,379,381,382,
383,384,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,387,388,389,390,92,391,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,93,93,194,194,93,393,194,201,394,395,396,397,398,
399,401,402,403,201,404,405,420,422,425,93,429,194,409,409,432,418,409,418,418,
434,194,426,435,435,427,93,93,93,93,93,93,93,93,93,93,409,437,409,418,426,427,
427,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,433,433,433,444,93,438,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
93,93,93,93,93,93,93,93,104,104,210,210,104,446,210,371,371,423,475,371,412,412,
412,412,412,412,412,412,480,453,454,104,482,210,483,416,416,485,371,416,423,486,
210,487,438,488,371,104,104,104,104,104,104,104,104,104,104,489,416,463,416,453,
454,464,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,490,463,491,492,104,464,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,105,105,216,216,105,467,216,415,415,415,415,415,415,415,415,493,497,468,469,
470,471,472,504,105,506,216,508,509,510,511,512,513,514,- 1,216,- 1,- 1,467,- 1,105,
105,105,105,105,105,105,105,105,105,468,469,470,471,472,- 1,- 1,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,- 1,- 1,- 1,- 1,105,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,112,112,219,219,112,428,219,
428,428,428,428,428,428,428,428,428,428,484,- 1,- 1,- 1,- 1,- 1,112,- 1,219,484,484,
484,484,484,484,484,484,219,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,113,113,223,223,113,452,223,452,- 1,- 1,452,452,452,452,452,452,452,
452,452,452,- 1,- 1,- 1,113,- 1,223,502,- 1,502,502,- 1,- 1,- 1,- 1,223,- 1,- 1,- 1,- 1,113,
113,113,113,113,113,113,113,113,113,502,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,122,122,122,122,122,122,122,122,
122,122,122,122,122,- 1,- 1,- 1,- 1,- 1,- 1,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,
- 1,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,
122,122,122,122,122,122,122,123,123,123,123,123,123,123,123,123,123,123,123,123,
- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,124,124,124,124,124,124,124,124,124,124,124,124,124,- 1,- 1,- 1,- 1,424,- 1,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,125,424,- 1,125,124,- 1,124,124,124,124,124,124,124,124,
124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,- 1,- 1,- 1,
- 1,125,125,125,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,- 1,424,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,- 1,- 1,- 1,- 1,125,- 1,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,129,129,129,129,
129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,139,- 1,- 1,
139,129,- 1,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,- 1,- 1,148,- 1,- 1,148,139,139,139,139,139,139,
139,139,142,142,142,142,142,142,142,142,142,142,331,226,226,- 1,331,226,424,142,
142,142,142,142,142,331,- 1,331,148,148,148,148,148,148,148,148,226,- 1,- 1,- 1,- 1,-
1,- 1,139,- 1,226,125,- 1,- 1,- 1,- 1,142,142,142,142,142,142,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,
- 1,139,143,143,143,143,143,143,143,143,143,143,148,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,
143,143,143,151,151,151,151,151,151,151,151,151,151,148,- 1,- 1,- 1,- 1,- 1,- 1,151,
151,151,151,151,151,- 1,- 1,- 1,143,143,143,143,143,143,- 1,- 1,- 1,- 1,- 1,- 1,152,152,
152,152,152,152,152,152,152,152,- 1,151,151,151,151,151,151,152,152,152,152,152,
152,220,220,232,232,220,- 1,232,- 1,- 1,- 1,238,238,248,248,238,- 1,248,254,254,410,-
1,254,410,220,- 1,232,152,152,152,152,152,152,220,238,232,248,- 1,- 1,- 1,- 1,254,- 1,
238,- 1,248,- 1,- 1,- 1,- 1,254,- 1,- 1,- 1,- 1,- 1,- 1,- 1,410,410,410,410,410,410,410,410,
413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,496,- 1,- 1,413,413,413,413,
413,413,496,496,496,496,496,496,496,496,220,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,410,- 1,- 1,
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
base=Cyc_lex_base[state];if(base < 0)return(- base)- 1;backtrk=Cyc_lex_backtrk[
state];if(backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=
backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)
return(- state)- 1;else{c=256;}}else{c=(int)*((unsigned char*)
_check_unknown_subscript(lbuf->lex_buffer,sizeof(unsigned char),lbuf->lex_curr_pos
++));if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3749,base + 
c)]== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3749,base + c)];
else{state=Cyc_lex_default[state];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2[0]=({struct Cyc_Lexing_Error_struct _tmp3;
_tmp3.tag=Cyc_Lexing_Error;_tmp3.f1=_tag_arr("empty token",sizeof(unsigned char),
12);_tmp3;});_tmp2;}));else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=
0;}}}struct _tuple9*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
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
_LL5A: Cyc_Std_fputs(_tag_arr(" __IGNORE_FOR_CYCLONE_MALLOC ",sizeof(
unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*
_tmp3E[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp3E,sizeof(void*),0));});return 1;case 14:
_LL5B: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_MALLOC ",sizeof(
unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*
_tmp3F[0]={};Cyc_log(_tag_arr("Warning: declaration of malloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp3F,sizeof(void*),0));});return 1;case 15:
_LL5C: Cyc_Std_fputs(_tag_arr(" __IGNORE_FOR_CYCLONE_CALLOC ",sizeof(
unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*
_tmp40[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp40,sizeof(void*),0));});return 1;case 16:
_LL5D: Cyc_Std_fputs(_tag_arr("*__IGNORE_FOR_CYCLONE_CALLOC ",sizeof(
unsigned char),30),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*
_tmp41[0]={};Cyc_log(_tag_arr("Warning: declaration of calloc sidestepped\n",
sizeof(unsigned char),44),_tag_arr(_tmp41,sizeof(void*),0));});return 1;case 17:
_LL5E: Cyc_Std_fputs(_tag_arr("__region",sizeof(unsigned char),9),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp42[0]={};Cyc_log(_tag_arr("Warning: use of region sidestepped\n",
sizeof(unsigned char),36),_tag_arr(_tmp42,sizeof(void*),0));});return 1;case 18:
_LL5F: return 1;case 19: _LL60: return 1;case 20: _LL61: return 1;case 21: _LL62: return 1;
case 22: _LL63: return 1;case 23: _LL64: return 1;case 24: _LL65: Cyc_Std_fputs(_tag_arr("inline",
sizeof(unsigned char),7),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 25: _LL66: Cyc_Std_fputs(_tag_arr("inline",sizeof(unsigned char),7),(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 26: _LL67: Cyc_Std_fputs(
_tag_arr("const",sizeof(unsigned char),6),(struct Cyc_Std___cycFILE*)_check_null(
Cyc_slurp_out));return 1;case 27: _LL68: Cyc_Std_fputs(_tag_arr("const",sizeof(
unsigned char),6),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 28: _LL69: Cyc_Std_fputs(_tag_arr("int",sizeof(unsigned char),4),(struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 29: _LL6A: return 1;case 30: _LL6B: Cyc_parens_to_match=
1;while(Cyc_asm(lexbuf)){;}Cyc_Std_fputs(_tag_arr("0",sizeof(unsigned char),2),(
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp43[0]={};Cyc_log(
_tag_arr("Warning: replacing use of __asm__ with 0\n",sizeof(unsigned char),42),
_tag_arr(_tmp43,sizeof(void*),0));});return 1;case 31: _LL6C: Cyc_Std_fputc((int)Cyc_Lexing_lexeme_char(
lexbuf,0),(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));return 1;default:
_LL6D:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp44=_cycalloc(sizeof(*_tmp44));
_tmp44[0]=({struct Cyc_Lexing_Error_struct _tmp45;_tmp45.tag=Cyc_Lexing_Error;
_tmp45.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp45;});
_tmp44;}));}int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(
lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL6F: return 0;
case 1: _LL70: Cyc_Std_fputc((int)'"',(struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));
return 0;case 2: _LL71:({void*_tmp46[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(unsigned char),26),_tag_arr(_tmp46,sizeof(void*),0));});({struct Cyc_Std_String_pa_struct
_tmp48;_tmp48.tag=0;_tmp48.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp47[1]={& _tmp48};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp47,sizeof(void*),1));}});
return 1;case 3: _LL72:({struct Cyc_Std_String_pa_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp49[1]={& _tmp4A};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp49,sizeof(void*),1));}});return 1;case 4: _LL73:({
struct Cyc_Std_String_pa_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf);{void*_tmp4B[1]={& _tmp4C};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp4B,sizeof(void*),1));}});return 1;case 5: _LL74:({struct Cyc_Std_String_pa_struct
_tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp4D[1]={& _tmp4E};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4D,sizeof(void*),1));}});
return 1;case 6: _LL75:({struct Cyc_Std_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*_tmp4F[1]={& _tmp50};Cyc_Std_fprintf((
struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp4F,sizeof(void*),1));}});return 1;case 7: _LL76:({
struct Cyc_Std_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)
Cyc_Lexing_lexeme(lexbuf);{void*_tmp51[1]={& _tmp52};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)
_check_null(Cyc_slurp_out),_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp51,sizeof(void*),1));}});return 1;case 8: _LL77:({struct Cyc_Std_String_pa_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf);{void*
_tmp53[1]={& _tmp54};Cyc_Std_fprintf((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out),
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp53,sizeof(void*),1));}});
return 1;default: _LL78:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp55=
_cycalloc(sizeof(*_tmp55));_tmp55[0]=({struct Cyc_Lexing_Error_struct _tmp56;
_tmp56.tag=Cyc_Lexing_Error;_tmp56.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp56;});_tmp55;}));}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL7A: return 0;case 1: _LL7B: if(Cyc_parens_to_match == 1)return 0;Cyc_parens_to_match
--;return 1;case 2: _LL7C: Cyc_parens_to_match ++;return 1;case 3: _LL7D: while(Cyc_asm_string(
lexbuf)){;}return 1;case 4: _LL7E: while(Cyc_asm_comment(lexbuf)){;}return 1;case 5:
_LL7F: return 1;case 6: _LL80: return 1;default: _LL81:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_Lexing_Error_struct
_tmp58;_tmp58.tag=Cyc_Lexing_Error;_tmp58.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp58;});_tmp57;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL83:({void*_tmp59[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(unsigned char),26),_tag_arr(_tmp59,sizeof(void*),0));});return 0;case 1:
_LL84: return 0;case 2: _LL85:({void*_tmp5A[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",
sizeof(unsigned char),26),_tag_arr(_tmp5A,sizeof(void*),0));});return 1;case 3:
_LL86: return 1;case 4: _LL87: return 1;case 5: _LL88: return 1;case 6: _LL89: return 1;case 7:
_LL8A: return 1;case 8: _LL8B: return 1;default: _LL8C:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Lexing_Error_struct
_tmp5C;_tmp5C.tag=Cyc_Lexing_Error;_tmp5C.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp5C;});_tmp5B;}));}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL8E:({void*_tmp5D[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",
sizeof(unsigned char),27),_tag_arr(_tmp5D,sizeof(void*),0));});return 0;case 1:
_LL8F: return 0;case 2: _LL90: return 1;default: _LL91:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Lexing_Error_struct
_tmp5F;_tmp5F.tag=Cyc_Lexing_Error;_tmp5F.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp5F;});_tmp5E;}));}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple10*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL93: Cyc_current_line=_tag_arr("#define ",
sizeof(unsigned char),9);Cyc_suck_macroname(lexbuf);return({struct _tuple10*
_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->f1=Cyc_current_line;_tmp60->f2=(struct
_tagged_arr*)_check_null(Cyc_current_source);_tmp60;});case 1: _LL94: return Cyc_suck_line(
lexbuf);case 2: _LL95: return 0;default: _LL96:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Lexing_Error_struct
_tmp62;_tmp62.tag=Cyc_Lexing_Error;_tmp62.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp62;});_tmp61;}));}struct _tuple10*Cyc_suck_line(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL98: Cyc_current_source=({struct _tagged_arr*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp63;});Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(Cyc_current_line,*((
struct _tagged_arr*)_check_null(Cyc_current_source)));return Cyc_suck_restofline(
lexbuf);default: _LL99:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp64=
_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Lexing_Error_struct _tmp65;
_tmp65.tag=Cyc_Lexing_Error;_tmp65.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp65;});_tmp64;}));}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL9B: Cyc_current_line=(struct _tagged_arr)Cyc_Std_strconcat(
Cyc_current_line,(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 0;default:
_LL9C:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp66=_cycalloc(
sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Lexing_Error_struct _tmp67;_tmp67.tag=Cyc_Lexing_Error;
_tmp67.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp67;});
_tmp66;}));}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple12*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL9E:
Cyc_current_header=(struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=0;Cyc_current_cstubs=
0;Cyc_current_cycstubs=0;Cyc_current_prologue=0;Cyc_current_epilogue=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_prologue=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_prologue);Cyc_current_epilogue=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_epilogue);Cyc_current_cstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cycstubs);return({struct _tuple12*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68->f1=Cyc_current_header;_tmp68->f2=Cyc_current_symbols;_tmp68->f3=Cyc_current_omit_symbols;
_tmp68->f4=Cyc_current_prologue;_tmp68->f5=Cyc_current_epilogue;_tmp68->f6=Cyc_current_cstubs;
_tmp68->f7=Cyc_current_cycstubs;_tmp68;});case 1: _LL9F: return Cyc_spec(lexbuf);
case 2: _LLA0: return Cyc_spec(lexbuf);case 3: _LLA1: return 0;case 4: _LLA2:({struct Cyc_Std_Int_pa_struct
_tmp6A;_tmp6A.tag=1;_tmp6A.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp69[1]={& _tmp6A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected header file name, found '%c' instead\n",
sizeof(unsigned char),67),_tag_arr(_tmp69,sizeof(void*),1));}});return 0;default:
_LLA3:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));
_tmp6B[0]=({struct Cyc_Lexing_Error_struct _tmp6C;_tmp6C.tag=Cyc_Lexing_Error;
_tmp6C.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp6C;});
_tmp6B;}));}struct _tuple12*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(
lexbuf,13);}int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLA5: return 0;
case 1: _LLA6: return 0;case 2: _LLA7: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(
lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LLA8: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LLA9: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple11*x=({struct _tuple11*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=(
struct _tagged_arr)_tag_arr(0,0,0);_tmp6E->f2=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp6E;});Cyc_current_prologue=({
struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=x;_tmp6D->tl=
Cyc_current_prologue;_tmp6D;});return 1;}case 5: _LLAA: {struct _tagged_arr _tmp6F=
Cyc_Lexing_lexeme(lexbuf);_tagged_arr_inplace_plus(& _tmp6F,sizeof(unsigned char),(
int)Cyc_Std_strlen(_tag_arr("prologue",sizeof(unsigned char),9)));while(isspace((
int)*((unsigned char*)_check_unknown_subscript(_tmp6F,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp6F,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp6F;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp70=Cyc_Std_substring((struct _tagged_arr)_tmp6F,0,(unsigned int)((t.curr - 
_tmp6F.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->f1=(struct
_tagged_arr)_tmp70;_tmp72->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp72;});Cyc_current_prologue=({struct Cyc_List_List*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=x;_tmp71->tl=Cyc_current_prologue;
_tmp71;});return 1;}}}}case 6: _LLAB: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*x=({struct
_tuple11*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp74->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp74;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=x;_tmp73->tl=Cyc_current_epilogue;
_tmp73;});return 1;}case 7: _LLAC: {struct _tagged_arr _tmp75=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp75,sizeof(unsigned char),(int)Cyc_Std_strlen(
_tag_arr("epilogue",sizeof(unsigned char),9)));while(isspace((int)*((
unsigned char*)_check_unknown_subscript(_tmp75,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp75,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp75;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp76=Cyc_Std_substring((struct _tagged_arr)_tmp75,0,(unsigned int)((t.curr - 
_tmp75.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->f1=(struct
_tagged_arr)_tmp76;_tmp78->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp78;});Cyc_current_epilogue=({struct Cyc_List_List*
_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->hd=x;_tmp77->tl=Cyc_current_epilogue;
_tmp77;});return 1;}}}}case 8: _LLAD: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*x=({struct
_tuple11*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp7A->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp7A;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->hd=x;_tmp79->tl=Cyc_current_cstubs;
_tmp79;});return 1;}case 9: _LLAE: {struct _tagged_arr _tmp7B=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp7B,sizeof(unsigned char),(int)Cyc_Std_strlen(
_tag_arr("cstub",sizeof(unsigned char),6)));while(isspace((int)*((unsigned char*)
_check_unknown_subscript(_tmp7B,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp7B,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp7B;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp7C=Cyc_Std_substring((struct _tagged_arr)_tmp7B,0,(unsigned int)((t.curr - 
_tmp7B.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->f1=(struct
_tagged_arr)_tmp7C;_tmp7E->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp7E;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=x;_tmp7D->tl=Cyc_current_cstubs;
_tmp7D;});return 1;}}}}case 10: _LLAF: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*x=({struct
_tuple11*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->f1=(struct _tagged_arr)
_tag_arr(0,0,0);_tmp80->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp80;});Cyc_current_cycstubs=({struct Cyc_List_List*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->hd=x;_tmp7F->tl=Cyc_current_cycstubs;
_tmp7F;});return 1;}case 11: _LLB0: {struct _tagged_arr _tmp81=Cyc_Lexing_lexeme(
lexbuf);_tagged_arr_inplace_plus(& _tmp81,sizeof(unsigned char),(int)Cyc_Std_strlen(
_tag_arr("cycstub",sizeof(unsigned char),8)));while(isspace((int)*((
unsigned char*)_check_unknown_subscript(_tmp81,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& _tmp81,sizeof(unsigned char),1);}{struct
_tagged_arr t=_tmp81;while(!isspace((int)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),0)))){
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1);}{struct _tagged_arr
_tmp82=Cyc_Std_substring((struct _tagged_arr)_tmp81,0,(unsigned int)((t.curr - 
_tmp81.curr)/ sizeof(unsigned char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct
Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple11*
x=({struct _tuple11*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=(struct
_tagged_arr)_tmp82;_tmp84->f2=(struct _tagged_arr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp84;});Cyc_current_cycstubs=({struct Cyc_List_List*
_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=x;_tmp83->tl=Cyc_current_cycstubs;
_tmp83;});return 1;}}}}case 12: _LLB1: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tagged_arr*x=({struct
_tagged_arr*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=(struct _tagged_arr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp86;});Cyc_current_cpp=({struct
Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=x;_tmp85->tl=Cyc_current_cpp;
_tmp85;});return 1;}case 13: _LLB2: return 1;case 14: _LLB3: return 1;case 15: _LLB4:({
struct Cyc_Std_Int_pa_struct _tmp88;_tmp88.tag=1;_tmp88.f1=(int)((unsigned int)((
int)Cyc_Lexing_lexeme_char(lexbuf,0)));{void*_tmp87[1]={& _tmp88};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error in .cys file: expected command, found '%c' instead\n",
sizeof(unsigned char),58),_tag_arr(_tmp87,sizeof(void*),1));}});return 0;default:
_LLB5:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp89=_cycalloc(sizeof(*
_tmp89));_tmp89[0]=({struct Cyc_Lexing_Error_struct _tmp8A;_tmp8A.tag=Cyc_Lexing_Error;
_tmp8A.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp8A;});
_tmp89;}));}int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(
lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB7: Cyc_snarfed_symbols=({
struct Cyc_List_List*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->hd=({struct
_tagged_arr*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf);_tmp8C;});_tmp8B->tl=Cyc_snarfed_symbols;_tmp8B;});return 1;case 1: _LLB8:
return 1;case 2: _LLB9: return 0;case 3: _LLBA:({void*_tmp8D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error in .cys file: unexpected end-of-file\n",sizeof(unsigned char),44),
_tag_arr(_tmp8D,sizeof(void*),0));});return 0;case 4: _LLBB:({struct Cyc_Std_Int_pa_struct
_tmp8F;_tmp8F.tag=1;_tmp8F.f1=(int)((unsigned int)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)));{void*_tmp8E[1]={& _tmp8F};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error in .cys file: expected symbol, found '%c' instead\n",
sizeof(unsigned char),57),_tag_arr(_tmp8E,sizeof(void*),1));}});return 0;default:
_LLBC:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90[0]=({struct Cyc_Lexing_Error_struct _tmp91;_tmp91.tag=Cyc_Lexing_Error;
_tmp91.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp91;});
_tmp90;}));}int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBE:({void*
_tmp92[0]={};Cyc_log(_tag_arr("Warning: unclosed brace\n",sizeof(unsigned char),
25),_tag_arr(_tmp92,sizeof(void*),0));});return 0;case 1: _LLBF: if(Cyc_braces_to_match
== 1)return 0;Cyc_braces_to_match --;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLC0: Cyc_braces_to_match ++;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLC1: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),
_tag_arr("/*",sizeof(unsigned char),3));while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LLC3: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC4: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLC5:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp93=_cycalloc(
sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Lexing_Error_struct _tmp94;_tmp94.tag=Cyc_Lexing_Error;
_tmp94.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp94;});
_tmp93;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(
lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC7:({void*
_tmp95[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(unsigned char),
26),_tag_arr(_tmp95,sizeof(void*),0));});return 0;case 1: _LLC8: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLC9:({void*
_tmp96[0]={};Cyc_log(_tag_arr("Warning: unclosed string\n",sizeof(unsigned char),
26),_tag_arr(_tmp96,sizeof(void*),0));});Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 3: _LLCA: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 4: _LLCB: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 5: _LLCC: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
case 6: _LLCD: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;case 7: _LLCE: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 8: _LLCF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;
default: _LLD0:(lexbuf->refill_buff)(lexbuf);return Cyc_block_string_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp97=_cycalloc(
sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Lexing_Error_struct _tmp98;_tmp98.tag=Cyc_Lexing_Error;
_tmp98.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp98;});
_tmp97;}));}int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLD2:({void*
_tmp99[0]={};Cyc_log(_tag_arr("Warning: unclosed comment\n",sizeof(unsigned char),
27),_tag_arr(_tmp99,sizeof(void*),0));});return 0;case 1: _LLD3: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tag_arr("*/",sizeof(unsigned char),
3));return 0;case 2: _LLD4: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(
Cyc_specbuf),(struct _tagged_arr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLD5:(
lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));
_tmp9A[0]=({struct Cyc_Lexing_Error_struct _tmp9B;_tmp9B.tag=Cyc_Lexing_Error;
_tmp9B.f1=_tag_arr("some action didn't return!",sizeof(unsigned char),27);_tmp9B;});
_tmp9A;}));}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}static void Cyc_pr(struct _tagged_arr*sptr){({struct Cyc_Std_String_pa_struct
_tmp9D;_tmp9D.tag=0;_tmp9D.f1=(struct _tagged_arr)*sptr;{void*_tmp9C[1]={& _tmp9D};
Cyc_Std_printf(_tag_arr(" %s\n",sizeof(unsigned char),5),_tag_arr(_tmp9C,sizeof(
void*),1));}});}void Cyc_scan_type(void*t);struct _tuple13{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e){void*_tmp9E=(
void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmp9F;struct
_tuple0 _tmpA0;struct _tagged_arr*_tmpA1;struct _tuple0*_tmpA2;struct _tuple0 _tmpA3;
struct _tagged_arr*_tmpA4;struct Cyc_List_List*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;
struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA9;
struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;
struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;
struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;
struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_List_List*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;
struct Cyc_List_List*_tmpB6;void*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_MallocInfo
_tmpB9;int _tmpBA;struct Cyc_Absyn_Exp*_tmpBB;void**_tmpBC;struct Cyc_Absyn_Exp*
_tmpBD;void*_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;struct _tagged_arr*_tmpC0;struct Cyc_Absyn_Exp*
_tmpC1;struct _tagged_arr*_tmpC2;void*_tmpC3;void*_tmpC4;struct Cyc_List_List*
_tmpC5;_LLD8: if(*((int*)_tmp9E)!= 1)goto _LLDA;_tmp9F=((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f1;_tmpA0=*_tmp9F;_tmpA1=_tmpA0.f2;_LLD9: _tmpA4=_tmpA1;goto _LLDB;_LLDA:
if(*((int*)_tmp9E)!= 2)goto _LLDC;_tmpA2=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp9E)->f1;_tmpA3=*_tmpA2;_tmpA4=_tmpA3.f2;_LLDB: Cyc_add_target(_tmpA4);return;
_LLDC: if(*((int*)_tmp9E)!= 3)goto _LLDE;_tmpA5=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9E)->f2;_LLDD: for(0;_tmpA5 != 0;_tmpA5=_tmpA5->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpA5->hd));}return;_LLDE: if(*((int*)_tmp9E)!= 23)goto
_LLE0;_tmpA6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9E)->f1;_tmpA7=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp9E)->f2;_LLDF: _tmpA8=_tmpA6;_tmpA9=_tmpA7;goto
_LLE1;_LLE0: if(*((int*)_tmp9E)!= 7)goto _LLE2;_tmpA8=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9E)->f1;_tmpA9=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E)->f2;_LLE1: _tmpAA=
_tmpA8;_tmpAB=_tmpA9;goto _LLE3;_LLE2: if(*((int*)_tmp9E)!= 4)goto _LLE4;_tmpAA=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f1;_tmpAB=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9E)->f3;_LLE3: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAA);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpAB);return;_LLE4: if(*((int*)_tmp9E)!= 20)goto _LLE6;
_tmpAC=((struct Cyc_Absyn_Deref_e_struct*)_tmp9E)->f1;_LLE5: _tmpAD=_tmpAC;goto
_LLE7;_LLE6: if(*((int*)_tmp9E)!= 17)goto _LLE8;_tmpAD=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9E)->f1;_LLE7: _tmpAE=_tmpAD;goto _LLE9;_LLE8: if(*((int*)_tmp9E)!= 14)goto
_LLEA;_tmpAE=((struct Cyc_Absyn_Address_e_struct*)_tmp9E)->f1;_LLE9: _tmpAF=_tmpAE;
goto _LLEB;_LLEA: if(*((int*)_tmp9E)!= 5)goto _LLEC;_tmpAF=((struct Cyc_Absyn_Increment_e_struct*)
_tmp9E)->f1;_LLEB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpAF);return;_LLEC: if(*((
int*)_tmp9E)!= 6)goto _LLEE;_tmpB0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f1;
_tmpB1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f2;_tmpB2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9E)->f3;_LLED: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB0);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpB1);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB2);return;
_LLEE: if(*((int*)_tmp9E)!= 9)goto _LLF0;_tmpB3=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp9E)->f1;_tmpB4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9E)->f2;_LLEF: _tmpB5=
_tmpB3;_tmpB6=_tmpB4;goto _LLF1;_LLF0: if(*((int*)_tmp9E)!= 8)goto _LLF2;_tmpB5=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9E)->f1;_tmpB6=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp9E)->f2;_LLF1: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB5);for(0;_tmpB6 != 0;
_tmpB6=_tmpB6->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmpB6->hd));}return;_LLF2: if(*((int*)_tmp9E)!= 13)goto _LLF4;_tmpB7=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9E)->f1;_tmpB8=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9E)->f2;_LLF3: Cyc_scan_type(_tmpB7);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpB8);return;_LLF4: if(*((int*)_tmp9E)!= 33)goto _LLF6;_tmpB9=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp9E)->f1;_tmpBA=_tmpB9.is_calloc;_tmpBB=_tmpB9.rgn;_tmpBC=_tmpB9.elt_type;
_tmpBD=_tmpB9.num_elts;_LLF5: if(_tmpBB != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_check_null(_tmpBB)));if(_tmpBC != 0)Cyc_scan_type(*_tmpBC);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBD);return;_LLF6: if(*((int*)_tmp9E)!= 16)
goto _LLF8;_tmpBE=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp9E)->f1;_LLF7:
Cyc_scan_type(_tmpBE);return;_LLF8: if(*((int*)_tmp9E)!= 21)goto _LLFA;_tmpBF=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9E)->f1;_tmpC0=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9E)->f2;_LLF9: _tmpC1=_tmpBF;_tmpC2=_tmpC0;goto _LLFB;_LLFA: if(*((int*)_tmp9E)
!= 22)goto _LLFC;_tmpC1=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9E)->f1;_tmpC2=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9E)->f2;_LLFB: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpC1);Cyc_add_target(_tmpC2);return;_LLFC: if(*((int*)_tmp9E)!= 18)goto _LLFE;
_tmpC3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp9E)->f1;_tmpC4=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp9E)->f2;_LLFD: Cyc_scan_type(_tmpC3);{void*
_tmpC6=_tmpC4;struct _tagged_arr*_tmpC7;_LL125: if(*((int*)_tmpC6)!= 0)goto _LL127;
_tmpC7=((struct Cyc_Absyn_StructField_struct*)_tmpC6)->f1;_LL126: Cyc_add_target(
_tmpC7);goto _LL124;_LL127: if(*((int*)_tmpC6)!= 1)goto _LL124;_LL128: goto _LL124;
_LL124:;}return;_LLFE: if(*((int*)_tmp9E)!= 0)goto _LL100;_LLFF: return;_LL100: if(*((
int*)_tmp9E)!= 34)goto _LL102;_tmpC5=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp9E)->f2;_LL101: for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){struct _tuple13 _tmpC9;
struct Cyc_Absyn_Exp*_tmpCA;struct _tuple13*_tmpC8=(struct _tuple13*)_tmpC5->hd;
_tmpC9=*_tmpC8;_tmpCA=_tmpC9.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCA);}
return;_LL102: if(*((int*)_tmp9E)!= 35)goto _LL104;_LL103:({void*_tmpCB[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Stmt_e\n",sizeof(unsigned char),26),
_tag_arr(_tmpCB,sizeof(void*),0));});exit(1);return;_LL104: if(*((int*)_tmp9E)!= 
10)goto _LL106;_LL105:({void*_tmpCC[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Throw_e\n",
sizeof(unsigned char),27),_tag_arr(_tmpCC,sizeof(void*),0));});exit(1);return;
_LL106: if(*((int*)_tmp9E)!= 11)goto _LL108;_LL107:({void*_tmpCD[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected NoInstantiate_e\n",sizeof(
unsigned char),35),_tag_arr(_tmpCD,sizeof(void*),0));});exit(1);return;_LL108:
if(*((int*)_tmp9E)!= 12)goto _LL10A;_LL109:({void*_tmpCE[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Instantiate_e\n",sizeof(unsigned char),
33),_tag_arr(_tmpCE,sizeof(void*),0));});exit(1);return;_LL10A: if(*((int*)_tmp9E)
!= 15)goto _LL10C;_LL10B:({void*_tmpCF[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected New_e\n",sizeof(unsigned char),25),_tag_arr(_tmpCF,
sizeof(void*),0));});exit(1);return;_LL10C: if(*((int*)_tmp9E)!= 19)goto _LL10E;
_LL10D:({void*_tmpD0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Gentyp_e\n",
sizeof(unsigned char),28),_tag_arr(_tmpD0,sizeof(void*),0));});exit(1);return;
_LL10E: if(*((int*)_tmp9E)!= 24)goto _LL110;_LL10F:({void*_tmpD1[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Tuple_e\n",sizeof(unsigned char),27),
_tag_arr(_tmpD1,sizeof(void*),0));});exit(1);return;_LL110: if(*((int*)_tmp9E)!= 
25)goto _LL112;_LL111:({void*_tmpD2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected CompoundLit_e\n",
sizeof(unsigned char),33),_tag_arr(_tmpD2,sizeof(void*),0));});exit(1);return;
_LL112: if(*((int*)_tmp9E)!= 26)goto _LL114;_LL113:({void*_tmpD3[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Array_e\n",sizeof(unsigned char),27),
_tag_arr(_tmpD3,sizeof(void*),0));});exit(1);return;_LL114: if(*((int*)_tmp9E)!= 
27)goto _LL116;_LL115:({void*_tmpD4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Comprehension_e\n",
sizeof(unsigned char),35),_tag_arr(_tmpD4,sizeof(void*),0));});exit(1);return;
_LL116: if(*((int*)_tmp9E)!= 28)goto _LL118;_LL117:({void*_tmpD5[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Struct_e\n",sizeof(unsigned char),28),
_tag_arr(_tmpD5,sizeof(void*),0));});exit(1);return;_LL118: if(*((int*)_tmp9E)!= 
29)goto _LL11A;_LL119:({void*_tmpD6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AnonStruct_e\n",
sizeof(unsigned char),32),_tag_arr(_tmpD6,sizeof(void*),0));});exit(1);return;
_LL11A: if(*((int*)_tmp9E)!= 30)goto _LL11C;_LL11B:({void*_tmpD7[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Tunion_e\n",sizeof(unsigned char),28),
_tag_arr(_tmpD7,sizeof(void*),0));});exit(1);return;_LL11C: if(*((int*)_tmp9E)!= 
31)goto _LL11E;_LL11D:({void*_tmpD8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Enum_e\n",
sizeof(unsigned char),26),_tag_arr(_tmpD8,sizeof(void*),0));});exit(1);return;
_LL11E: if(*((int*)_tmp9E)!= 32)goto _LL120;_LL11F:({void*_tmpD9[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected AnonEnum_e\n",sizeof(unsigned char),
30),_tag_arr(_tmpD9,sizeof(void*),0));});exit(1);return;_LL120: if(*((int*)_tmp9E)
!= 36)goto _LL122;_LL121:({void*_tmpDA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected Codegen_e\n",sizeof(unsigned char),29),_tag_arr(
_tmpDA,sizeof(void*),0));});exit(1);return;_LL122: if(*((int*)_tmp9E)!= 37)goto
_LLD7;_LL123:({void*_tmpDB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected Fill_e\n",
sizeof(unsigned char),26),_tag_arr(_tmpDB,sizeof(void*),0));});exit(1);return;
_LLD7:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(
eo);return;}void Cyc_scan_type(void*t){void*_tmpDC=t;struct Cyc_Absyn_PtrInfo
_tmpDD;void*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_FnInfo _tmpE0;
struct Cyc_List_List*_tmpE1;struct Cyc_Absyn_AggrInfo _tmpE2;void*_tmpE3;struct
_tuple0*_tmpE4;struct _tuple0 _tmpE5;struct _tagged_arr*_tmpE6;struct _tuple0*_tmpE7;
struct _tuple0 _tmpE8;struct _tagged_arr*_tmpE9;_LL12A: if((int)_tmpDC != 0)goto
_LL12C;_LL12B: goto _LL12D;_LL12C: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 5)goto
_LL12E;_LL12D: goto _LL12F;_LL12E: if((int)_tmpDC != 1)goto _LL130;_LL12F: goto _LL131;
_LL130: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 6)goto _LL132;_LL131: return;
_LL132: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 4)goto _LL134;_tmpDD=((struct Cyc_Absyn_PointerType_struct*)
_tmpDC)->f1;_LL133: Cyc_scan_type((void*)_tmpDD.elt_typ);return;_LL134: if(_tmpDC
<= (void*)3?1:*((int*)_tmpDC)!= 7)goto _LL136;_tmpDE=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpDC)->f1;_tmpDF=((struct Cyc_Absyn_ArrayType_struct*)_tmpDC)->f3;_LL135: Cyc_scan_type(
_tmpDE);Cyc_scan_exp_opt(_tmpDF);return;_LL136: if(_tmpDC <= (void*)3?1:*((int*)
_tmpDC)!= 8)goto _LL138;_tmpE0=((struct Cyc_Absyn_FnType_struct*)_tmpDC)->f1;
_LL137: Cyc_scan_type((void*)_tmpE0.ret_typ);{struct Cyc_List_List*_tmpEA=_tmpE0.args;
for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){struct _tuple1 _tmpEC;void*_tmpED;struct
_tuple1*_tmpEB=(struct _tuple1*)_tmpEA->hd;_tmpEC=*_tmpEB;_tmpED=_tmpEC.f3;Cyc_scan_type(
_tmpED);}}if(_tmpE0.cyc_varargs != 0)Cyc_scan_type((void*)(_tmpE0.cyc_varargs)->type);
return;_LL138: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 11)goto _LL13A;_tmpE1=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpDC)->f2;_LL139: for(0;_tmpE1 != 0;_tmpE1=
_tmpE1->tl){Cyc_scan_type((void*)((struct Cyc_Absyn_Aggrfield*)_tmpE1->hd)->type);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmpE1->hd)->width);}return;_LL13A:
if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 13)goto _LL13C;_LL13B: return;_LL13C: if(
_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 10)goto _LL13E;_tmpE2=((struct Cyc_Absyn_AggrType_struct*)
_tmpDC)->f1;_tmpE3=(void*)_tmpE2.aggr_info;_LL13D: {struct _tuple0*_tmpEF;struct
_tuple0 _tmpF0;struct _tagged_arr*_tmpF1;struct _tuple3 _tmpEE=Cyc_Absyn_aggr_kinded_name(
_tmpE3);_tmpEF=_tmpEE.f2;_tmpF0=*_tmpEF;_tmpF1=_tmpF0.f2;_tmpE6=_tmpF1;goto
_LL13F;}_LL13E: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 12)goto _LL140;_tmpE4=((
struct Cyc_Absyn_EnumType_struct*)_tmpDC)->f1;_tmpE5=*_tmpE4;_tmpE6=_tmpE5.f2;
_LL13F: _tmpE9=_tmpE6;goto _LL141;_LL140: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 
16)goto _LL142;_tmpE7=((struct Cyc_Absyn_TypedefType_struct*)_tmpDC)->f1;_tmpE8=*
_tmpE7;_tmpE9=_tmpE8.f2;_LL141: Cyc_add_target(_tmpE9);return;_LL142: if(_tmpDC <= (
void*)3?1:*((int*)_tmpDC)!= 0)goto _LL144;_LL143:({void*_tmpF2[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: unexpected Evar\n",sizeof(unsigned char),24),
_tag_arr(_tmpF2,sizeof(void*),0));});exit(1);return;_LL144: if(_tmpDC <= (void*)3?
1:*((int*)_tmpDC)!= 1)goto _LL146;_LL145:({void*_tmpF3[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected VarType\n",sizeof(unsigned char),27),_tag_arr(_tmpF3,
sizeof(void*),0));});exit(1);return;_LL146: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)
!= 2)goto _LL148;_LL147:({void*_tmpF4[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionType\n",sizeof(unsigned char),30),_tag_arr(
_tmpF4,sizeof(void*),0));});exit(1);return;_LL148: if(_tmpDC <= (void*)3?1:*((int*)
_tmpDC)!= 3)goto _LL14A;_LL149:({void*_tmpF5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TunionFieldType\n",sizeof(unsigned char),35),
_tag_arr(_tmpF5,sizeof(void*),0));});exit(1);return;_LL14A: if(_tmpDC <= (void*)3?
1:*((int*)_tmpDC)!= 9)goto _LL14C;_LL14B:({void*_tmpF6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected TupleType\n",sizeof(unsigned char),29),_tag_arr(
_tmpF6,sizeof(void*),0));});exit(1);return;_LL14C: if(_tmpDC <= (void*)3?1:*((int*)
_tmpDC)!= 15)goto _LL14E;_LL14D:({void*_tmpF7[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected RgnHandleType\n",sizeof(unsigned char),33),_tag_arr(
_tmpF7,sizeof(void*),0));});exit(1);return;_LL14E: if((int)_tmpDC != 2)goto _LL150;
_LL14F:({void*_tmpF8[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected HeapRgn\n",
sizeof(unsigned char),27),_tag_arr(_tmpF8,sizeof(void*),0));});exit(1);return;
_LL150: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 17)goto _LL152;_LL151:({void*
_tmpF9[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected AccessEff\n",
sizeof(unsigned char),29),_tag_arr(_tmpF9,sizeof(void*),0));});exit(1);return;
_LL152: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 18)goto _LL154;_LL153:({void*
_tmpFA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected JoinEff\n",
sizeof(unsigned char),27),_tag_arr(_tmpFA,sizeof(void*),0));});exit(1);return;
_LL154: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 19)goto _LL156;_LL155:({void*
_tmpFB[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected RgnsEff\n",
sizeof(unsigned char),27),_tag_arr(_tmpFB,sizeof(void*),0));});exit(1);return;
_LL156: if(_tmpDC <= (void*)3?1:*((int*)_tmpDC)!= 14)goto _LL129;_LL157:({void*
_tmpFC[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: unexpected sizeof_t\n",
sizeof(unsigned char),28),_tag_arr(_tmpFC,sizeof(void*),0));});exit(1);return;
_LL129:;}struct _tuple14{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct _tuple9*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct
Cyc_Set_Set**_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
_tmpFD;});{void*_tmpFE=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmpFF;struct Cyc_Absyn_Fndecl*
_tmp100;struct Cyc_Absyn_Aggrdecl*_tmp101;struct Cyc_Absyn_Enumdecl*_tmp102;struct
Cyc_Absyn_Typedefdecl*_tmp103;_LL159: if(*((int*)_tmpFE)!= 0)goto _LL15B;_tmpFF=((
struct Cyc_Absyn_Var_d_struct*)_tmpFE)->f1;_LL15A: {struct _tuple0 _tmp105;struct
_tagged_arr*_tmp106;struct _tuple0*_tmp104=_tmpFF->name;_tmp105=*_tmp104;_tmp106=
_tmp105.f2;Cyc_current_source=(struct _tagged_arr*)_tmp106;Cyc_scan_type((void*)
_tmpFF->type);Cyc_scan_exp_opt(_tmpFF->initializer);goto _LL158;}_LL15B: if(*((int*)
_tmpFE)!= 1)goto _LL15D;_tmp100=((struct Cyc_Absyn_Fn_d_struct*)_tmpFE)->f1;_LL15C: {
struct _tuple0 _tmp108;struct _tagged_arr*_tmp109;struct _tuple0*_tmp107=_tmp100->name;
_tmp108=*_tmp107;_tmp109=_tmp108.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp109;Cyc_scan_type((void*)_tmp100->ret_type);{struct Cyc_List_List*_tmp10A=
_tmp100->args;for(0;_tmp10A != 0;_tmp10A=_tmp10A->tl){struct _tuple14 _tmp10C;void*
_tmp10D;struct _tuple14*_tmp10B=(struct _tuple14*)_tmp10A->hd;_tmp10C=*_tmp10B;
_tmp10D=_tmp10C.f3;Cyc_scan_type(_tmp10D);}}if(_tmp100->cyc_varargs != 0)Cyc_scan_type((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp100->cyc_varargs))->type);
if(_tmp100->is_inline)({void*_tmp10E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Warning: ignoring inline function\n",sizeof(unsigned char),35),
_tag_arr(_tmp10E,sizeof(void*),0));});goto _LL158;}_LL15D: if(*((int*)_tmpFE)!= 4)
goto _LL15F;_tmp101=((struct Cyc_Absyn_Aggr_d_struct*)_tmpFE)->f1;_LL15E: {struct
_tuple0 _tmp110;struct _tagged_arr*_tmp111;struct _tuple0*_tmp10F=_tmp101->name;
_tmp110=*_tmp10F;_tmp111=_tmp110.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp111;if((unsigned int)_tmp101->fields){{struct Cyc_List_List*_tmp112=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp101->fields))->v;for(0;
_tmp112 != 0;_tmp112=_tmp112->tl){struct Cyc_Absyn_Aggrfield*_tmp113=(struct Cyc_Absyn_Aggrfield*)
_tmp112->hd;Cyc_scan_type((void*)_tmp113->type);Cyc_scan_exp_opt(_tmp113->width);}}{
struct Cyc_List_List*_tmp114=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp101->fields))->v;for(0;_tmp114 != 0;_tmp114=_tmp114->tl){;}}}goto
_LL158;}_LL15F: if(*((int*)_tmpFE)!= 6)goto _LL161;_tmp102=((struct Cyc_Absyn_Enum_d_struct*)
_tmpFE)->f1;_LL160: {struct _tuple0 _tmp116;struct _tagged_arr*_tmp117;struct
_tuple0*_tmp115=_tmp102->name;_tmp116=*_tmp115;_tmp117=_tmp116.f2;Cyc_current_source=(
struct _tagged_arr*)_tmp117;if((unsigned int)_tmp102->fields){{struct Cyc_List_List*
_tmp118=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp102->fields))->v;
for(0;_tmp118 != 0;_tmp118=_tmp118->tl){struct Cyc_Absyn_Enumfield*_tmp119=(struct
Cyc_Absyn_Enumfield*)_tmp118->hd;Cyc_scan_exp_opt(_tmp119->tag);}}{struct Cyc_List_List*
_tmp11A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp102->fields))->v;
for(0;_tmp11A != 0;_tmp11A=_tmp11A->tl){;}}}goto _LL158;}_LL161: if(*((int*)_tmpFE)
!= 7)goto _LL163;_tmp103=((struct Cyc_Absyn_Typedef_d_struct*)_tmpFE)->f1;_LL162: {
struct _tuple0 _tmp11C;struct _tagged_arr*_tmp11D;struct _tuple0*_tmp11B=_tmp103->name;
_tmp11C=*_tmp11B;_tmp11D=_tmp11C.f2;Cyc_current_source=(struct _tagged_arr*)
_tmp11D;Cyc_scan_type((void*)_tmp103->defn);goto _LL158;}_LL163: if(*((int*)_tmpFE)
!= 2)goto _LL165;_LL164:({void*_tmp11E[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(unsigned char),35),
_tag_arr(_tmp11E,sizeof(void*),0));});exit(1);return 0;_LL165: if(*((int*)_tmpFE)
!= 5)goto _LL167;_LL166:({void*_tmp11F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected tunion declaration\n",sizeof(unsigned char),38),
_tag_arr(_tmp11F,sizeof(void*),0));});exit(1);return 0;_LL167: if(*((int*)_tmpFE)
!= 3)goto _LL169;_LL168:({void*_tmp120[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected let declaration\n",sizeof(unsigned char),35),
_tag_arr(_tmp120,sizeof(void*),0));});exit(1);return 0;_LL169: if(*((int*)_tmpFE)
!= 8)goto _LL16B;_LL16A:({void*_tmp121[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected namespace declaration\n",sizeof(unsigned char),41),
_tag_arr(_tmp121,sizeof(void*),0));});exit(1);return 0;_LL16B: if(*((int*)_tmpFE)
!= 9)goto _LL16D;_LL16C:({void*_tmp122[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected using declaration\n",sizeof(unsigned char),37),
_tag_arr(_tmp122,sizeof(void*),0));});exit(1);return 0;_LL16D: if(*((int*)_tmpFE)
!= 10)goto _LL158;_LL16E:({void*_tmp123[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: unexpected extern \"C\" declaration\n",sizeof(unsigned char),42),
_tag_arr(_tmp123,sizeof(void*),0));});exit(1);return 0;_LL158:;}return({struct
_tuple9*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->f1=(struct _tagged_arr*)
_check_null(Cyc_current_source);_tmp124->f2=*((struct Cyc_Set_Set**)_check_null(
Cyc_current_targets));_tmp124;});}struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),int(*hash)(struct _tagged_arr*)))Cyc_Hashtable_create)(107,Cyc_Std_strptrcmp,
Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*x){struct _handler_cons _tmp125;_push_handler(& _tmp125);{int
_tmp127=0;if(setjmp(_tmp125.handler))_tmp127=1;if(!_tmp127){{struct Cyc_Set_Set*
_tmp128=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*
key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp128;};_pop_handler();}
else{void*_tmp126=(void*)_exn_thrown;void*_tmp12A=_tmp126;_LL170: if(_tmp12A != 
Cyc_Core_Not_found)goto _LL172;_LL171: return((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);_LL172:;
_LL173:(void)_throw(_tmp12A);_LL16F:;}}}struct Cyc_Set_Set*Cyc_reachable(struct
Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
curr,(struct _tagged_arr*)init->hd);}{struct Cyc_Set_Set*_tmp12B=curr;struct
_tagged_arr*_tmp12C=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"",
sizeof(unsigned char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp12B)> 0){struct Cyc_Set_Set*_tmp12D=emptyset;struct Cyc_Iter_Iter _tmp12E=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(
Cyc_Core_heap_region,_tmp12B);while(((int(*)(struct Cyc_Iter_Iter,struct
_tagged_arr**))Cyc_Iter_next)(_tmp12E,& _tmp12C)){_tmp12D=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp12D,Cyc_find(t,
_tmp12C));}_tmp12B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_diff)(_tmp12D,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp12B);}return curr;}}int Cyc_force_directory(
struct _tagged_arr d){int _tmp130=({int _tmp133[0];Cyc_Std_open(d,0,_tag_arr(_tmp133,
sizeof(int),0));});if(_tmp130 == - 1){if(Cyc_Std_mkdir(d,448)== - 1){({struct Cyc_Std_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _tagged_arr)d;{void*_tmp131[1]={& _tmp132};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create directory %s\n",
sizeof(unsigned char),38),_tag_arr(_tmp131,sizeof(void*),1));}});return 1;}}else{
close(_tmp130);}return 0;}int Cyc_force_directory_prefixes(struct _tagged_arr file){
struct _tagged_arr _tmp134=Cyc_Std_strdup(file);struct Cyc_List_List*_tmp135=0;
while(1){_tmp134=Cyc_Filename_dirname((struct _tagged_arr)_tmp134);if(
_get_arr_size(_tmp134,sizeof(unsigned char))== 0)break;_tmp135=({struct Cyc_List_List*
_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=({struct _tagged_arr*_tmp137=
_cycalloc(sizeof(*_tmp137));_tmp137[0]=(struct _tagged_arr)_tmp134;_tmp137;});
_tmp136->tl=_tmp135;_tmp136;});}for(0;_tmp135 != 0;_tmp135=_tmp135->tl){if(Cyc_force_directory(*((
struct _tagged_arr*)_tmp135->hd)))return 1;}return 0;}int Cyc_process_file(struct
_tagged_arr filename,struct _tagged_arr dir,struct Cyc_List_List*start_symbols,
struct Cyc_List_List*omit_symbols,struct Cyc_List_List*prologue,struct Cyc_List_List*
epilogue,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc_Std___cycFILE*
maybe;struct Cyc_Std___cycFILE*in_file;struct Cyc_Std___cycFILE*out_file;({struct
Cyc_Std_String_pa_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(struct _tagged_arr)
filename;{void*_tmp138[1]={& _tmp139};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("********************************* %s...\n",
sizeof(unsigned char),41),_tag_arr(_tmp138,sizeof(void*),1));}});{struct
_tagged_arr buf=_tag_arr(({unsigned int _tmp1F3=(unsigned int)1024;unsigned char*
_tmp1F4=(unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),
_tmp1F3));{unsigned int _tmp1F5=_tmp1F3;unsigned int i;for(i=0;i < _tmp1F5;i ++){
_tmp1F4[i]='\000';}}_tmp1F4;}),sizeof(unsigned char),(unsigned int)1024);struct
_tagged_arr _tmp13A=Cyc_Std_getcwd(buf,_get_arr_size(buf,sizeof(unsigned char)));
if(Cyc_Std_chdir(dir)){({struct Cyc_Std_String_pa_struct _tmp13C;_tmp13C.tag=0;
_tmp13C.f1=(struct _tagged_arr)dir;{void*_tmp13B[1]={& _tmp13C};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: can't change directory to %s\n",sizeof(
unsigned char),37),_tag_arr(_tmp13B,sizeof(void*),1));}});return 1;}({struct Cyc_Std_String_pa_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _tagged_arr)filename;{void*_tmp13D[1]={&
_tmp13E};Cyc_log(_tag_arr("\n%s:\n",sizeof(unsigned char),6),_tag_arr(_tmp13D,
sizeof(void*),1));}});{struct _tagged_arr _tmp13F=Cyc_Filename_basename(filename);
struct _tagged_arr _tmp140=Cyc_Filename_chop_extension((struct _tagged_arr)_tmp13F);
struct _tagged_arr _tmp141=Cyc_Std_strconcat((struct _tagged_arr)_tmp140,_tag_arr(".iA",
sizeof(unsigned char),4));struct _tagged_arr _tmp142=Cyc_Std_strconcat((struct
_tagged_arr)_tmp140,_tag_arr(".i",sizeof(unsigned char),3));struct _tagged_arr
_tmp143=Cyc_Std_strconcat((struct _tagged_arr)_tmp140,_tag_arr(".iB",sizeof(
unsigned char),4));struct _tagged_arr _tmp144=Cyc_Filename_dirname(filename);
struct _tagged_arr _tmp145=_get_arr_size(_tmp144,sizeof(unsigned char))== 0?({
struct Cyc_Std_String_pa_struct _tmp1F0;_tmp1F0.tag=0;_tmp1F0.f1=(struct
_tagged_arr)_tmp13F;{void*_tmp1EF[1]={& _tmp1F0};Cyc_Std_aprintf(_tag_arr("c%s",
sizeof(unsigned char),4),_tag_arr(_tmp1EF,sizeof(void*),1));}}): Cyc_Filename_concat((
struct _tagged_arr)_tmp144,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=(struct _tagged_arr)_tmp13F;{void*_tmp1F1[1]={&
_tmp1F2};Cyc_Std_aprintf(_tag_arr("c%s",sizeof(unsigned char),4),_tag_arr(
_tmp1F1,sizeof(void*),1));}}));if(Cyc_force_directory_prefixes(filename))return 1;{
struct Cyc_Hashtable_Table*t=Cyc_new_deps();struct _tagged_arr _tmp146=
Cstring_to_string(Ccomp);maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp141,_tag_arr("w",
sizeof(unsigned char),2));if(!((unsigned int)maybe)){({struct Cyc_Std_String_pa_struct
_tmp148;_tmp148.tag=0;_tmp148.f1=(struct _tagged_arr)_tmp141;{void*_tmp147[1]={&
_tmp148};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",
sizeof(unsigned char),33),_tag_arr(_tmp147,sizeof(void*),1));}});return 1;}
out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);Cyc_current_cpp=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);{
struct Cyc_List_List*_tmp149=Cyc_current_cpp;for(0;_tmp149 != 0;_tmp149=_tmp149->tl){
Cyc_Std_fputs(*((struct _tagged_arr*)_tmp149->hd),out_file);}}({struct Cyc_Std_String_pa_struct
_tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _tagged_arr)filename;{void*_tmp14A[1]={&
_tmp14B};Cyc_Std_fprintf(out_file,_tag_arr("#include <%s>\n",sizeof(
unsigned char),15),_tag_arr(_tmp14A,sizeof(void*),1));}});Cyc_Std_fclose(
out_file);{struct _tagged_arr _tmp14C=({struct Cyc_Std_String_pa_struct _tmp1EE;
_tmp1EE.tag=0;_tmp1EE.f1=(struct _tagged_arr)_tmp141;{struct Cyc_Std_String_pa_struct
_tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(struct _tagged_arr)_tmp142;{struct Cyc_Std_String_pa_struct
_tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=(struct _tagged_arr)_tmp146;{void*_tmp1EB[3]={&
_tmp1EC,& _tmp1ED,& _tmp1EE};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",
sizeof(unsigned char),24),_tag_arr(_tmp1EB,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp14C)!= 0){Cyc_Std_remove((struct _tagged_arr)_tmp141);maybe=
Cyc_Std_fopen(filename,_tag_arr("w",sizeof(unsigned char),2));if(!((unsigned int)
maybe)){({struct Cyc_Std_String_pa_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct
_tagged_arr)filename;{void*_tmp14D[1]={& _tmp14E};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),33),_tag_arr(
_tmp14D,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(
struct _tagged_arr)filename;{void*_tmp14F[1]={& _tmp150};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(unsigned char),
48),_tag_arr(_tmp14F,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp145,_tag_arr("w",sizeof(unsigned char),2));if(!((
unsigned int)maybe)){({struct Cyc_Std_String_pa_struct _tmp152;_tmp152.tag=0;
_tmp152.f1=(struct _tagged_arr)_tmp145;{void*_tmp151[1]={& _tmp152};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: could not create file %s\n",sizeof(unsigned char),
33),_tag_arr(_tmp151,sizeof(void*),1));}});return 1;}out_file=(struct Cyc_Std___cycFILE*)
_check_null(maybe);({struct Cyc_Std_String_pa_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(
struct _tagged_arr)_tmp145;{void*_tmp153[1]={& _tmp154};Cyc_Std_fprintf(out_file,
_tag_arr("#error -- %s is not supported on this platform\n",sizeof(unsigned char),
48),_tag_arr(_tmp153,sizeof(void*),1));}});Cyc_Std_fclose(out_file);({struct Cyc_Std_String_pa_struct
_tmp156;_tmp156.tag=0;_tmp156.f1=(struct _tagged_arr)filename;{void*_tmp155[1]={&
_tmp156};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("Warning: %s will not be supported on this platform\n",
sizeof(unsigned char),52),_tag_arr(_tmp155,sizeof(void*),1));}});({void*_tmp157[
0]={};Cyc_log(_tag_arr("Not supported on this platform\n",sizeof(unsigned char),
32),_tag_arr(_tmp157,sizeof(void*),0));});if(Cyc_Std_chdir((struct _tagged_arr)
_tmp13A)){({struct Cyc_Std_String_pa_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(
struct _tagged_arr)_tmp13A;{void*_tmp158[1]={& _tmp159};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n",sizeof(unsigned char),41),
_tag_arr(_tmp158,sizeof(void*),1));}});return 1;}return 0;}maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp142,_tag_arr("r",sizeof(unsigned char),2));if(!((
unsigned int)maybe))return 1;in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);{
struct Cyc_Lexing_lexbuf*_tmp15A=Cyc_Lexing_from_file(in_file);struct _tuple9*
entry;while((entry=((struct _tuple9*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(
_tmp15A))!= 0){struct _tuple9 _tmp15C;struct _tagged_arr*_tmp15D;struct Cyc_Set_Set*
_tmp15E;struct _tuple9*_tmp15B=(struct _tuple9*)_check_null(entry);_tmp15C=*
_tmp15B;_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp15D,
_tmp15E);}Cyc_Std_fclose(in_file);_tmp14C=({struct Cyc_Std_String_pa_struct
_tmp162;_tmp162.tag=0;_tmp162.f1=(struct _tagged_arr)_tmp141;{struct Cyc_Std_String_pa_struct
_tmp161;_tmp161.tag=0;_tmp161.f1=(struct _tagged_arr)_tmp142;{struct Cyc_Std_String_pa_struct
_tmp160;_tmp160.tag=0;_tmp160.f1=(struct _tagged_arr)_tmp146;{void*_tmp15F[3]={&
_tmp160,& _tmp161,& _tmp162};Cyc_Std_aprintf(_tag_arr("%s -E -o %s -x c %s",
sizeof(unsigned char),20),_tag_arr(_tmp15F,sizeof(void*),3));}}}});if(Cyc_Std_system((
struct _tagged_arr)_tmp14C)!= 0)return 1;maybe=Cyc_Std_fopen((struct _tagged_arr)
_tmp142,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)maybe))return 1;
in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);_tmp15A=Cyc_Lexing_from_file(
in_file);Cyc_slurp_out=Cyc_Std_fopen((struct _tagged_arr)_tmp143,_tag_arr("w",
sizeof(unsigned char),2));if(!((unsigned int)Cyc_slurp_out))return 1;while(((int(*)(
struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp15A)){;}Cyc_Std_fclose(in_file);
Cyc_Std_fclose((struct Cyc_Std___cycFILE*)_check_null(Cyc_slurp_out));maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp143,_tag_arr("r",sizeof(unsigned char),2));if(!((
unsigned int)maybe))return 1;in_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);
Cyc_Position_reset_position((struct _tagged_arr)_tmp143);Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp163=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init();Cyc_Std_fclose(
in_file);Cyc_Std_remove((struct _tagged_arr)_tmp143);{struct Cyc_List_List*_tmp164=
_tmp163;for(0;_tmp164 != 0;_tmp164=_tmp164->tl){struct _tuple9*_tmp165=Cyc_scan_decl((
struct Cyc_Absyn_Decl*)_tmp164->hd);if(_tmp165 == 0)continue;{struct _tuple9 _tmp167;
struct _tagged_arr*_tmp168;struct Cyc_Set_Set*_tmp169;struct _tuple9*_tmp166=(
struct _tuple9*)_check_null(_tmp165);_tmp167=*_tmp166;_tmp168=_tmp167.f1;_tmp169=
_tmp167.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp16A;_push_handler(&
_tmp16A);{int _tmp16C=0;if(setjmp(_tmp16A.handler))_tmp16C=1;if(!_tmp16C){old=((
struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))Cyc_Hashtable_lookup)(
t,_tmp168);;_pop_handler();}else{void*_tmp16B=(void*)_exn_thrown;void*_tmp16E=
_tmp16B;_LL175: if(_tmp16E != Cyc_Core_Not_found)goto _LL177;_LL176: old=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);
goto _LL174;_LL177:;_LL178:(void)_throw(_tmp16E);_LL174:;}}}((void(*)(struct Cyc_Hashtable_Table*
t,struct _tagged_arr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp168,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp169,old));}}}}{struct Cyc_Set_Set*_tmp16F=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp170=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_strptrcmp);{
struct Cyc_List_List*_tmp171=_tmp163;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){
struct Cyc_Absyn_Decl*_tmp172=(struct Cyc_Absyn_Decl*)_tmp171->hd;struct
_tagged_arr*name;{void*_tmp173=(void*)_tmp172->r;struct Cyc_Absyn_Vardecl*_tmp174;
struct Cyc_Absyn_Fndecl*_tmp175;struct Cyc_Absyn_Aggrdecl*_tmp176;struct Cyc_Absyn_Enumdecl*
_tmp177;struct Cyc_Absyn_Typedefdecl*_tmp178;_LL17A: if(*((int*)_tmp173)!= 0)goto
_LL17C;_tmp174=((struct Cyc_Absyn_Var_d_struct*)_tmp173)->f1;_LL17B: {struct
_tuple0 _tmp17A;struct _tagged_arr*_tmp17B;struct _tuple0*_tmp179=_tmp174->name;
_tmp17A=*_tmp179;_tmp17B=_tmp17A.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(defined_symbols,
_tmp17B);if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_strptrcmp,
omit_symbols,_tmp17B))name=0;else{name=(struct _tagged_arr*)_tmp17B;}goto _LL179;}
_LL17C: if(*((int*)_tmp173)!= 1)goto _LL17E;_tmp175=((struct Cyc_Absyn_Fn_d_struct*)
_tmp173)->f1;_LL17D: {struct _tuple0 _tmp17D;struct _tagged_arr*_tmp17E;struct
_tuple0*_tmp17C=_tmp175->name;_tmp17D=*_tmp17C;_tmp17E=_tmp17D.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*
elt))Cyc_Set_insert)(defined_symbols,_tmp17E);if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_strptrcmp,omit_symbols,_tmp17E))name=0;else{name=(struct _tagged_arr*)
_tmp17E;}goto _LL179;}_LL17E: if(*((int*)_tmp173)!= 4)goto _LL180;_tmp176=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp173)->f1;_LL17F: {struct _tuple0 _tmp180;struct
_tagged_arr*_tmp181;struct _tuple0*_tmp17F=_tmp176->name;_tmp180=*_tmp17F;_tmp181=
_tmp180.f2;name=(struct _tagged_arr*)_tmp181;goto _LL179;}_LL180: if(*((int*)
_tmp173)!= 6)goto _LL182;_tmp177=((struct Cyc_Absyn_Enum_d_struct*)_tmp173)->f1;
_LL181: {struct _tuple0 _tmp183;struct _tagged_arr*_tmp184;struct _tuple0*_tmp182=
_tmp177->name;_tmp183=*_tmp182;_tmp184=_tmp183.f2;name=(struct _tagged_arr*)
_tmp184;if(name != 0?((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
_tmp16F,(struct _tagged_arr*)_check_null(name)): 0)_tmp170=({struct Cyc_List_List*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->hd=_tmp172;_tmp185->tl=_tmp170;
_tmp185;});else{if((unsigned int)_tmp177->fields){struct Cyc_List_List*_tmp186=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp177->fields))->v;for(
0;_tmp186 != 0;_tmp186=_tmp186->tl){struct Cyc_Absyn_Enumfield*_tmp187=(struct Cyc_Absyn_Enumfield*)
_tmp186->hd;struct _tuple0 _tmp189;struct _tagged_arr*_tmp18A;struct _tuple0*_tmp188=
_tmp187->name;_tmp189=*_tmp188;_tmp18A=_tmp189.f2;if(((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp16F,_tmp18A)){_tmp170=({struct Cyc_List_List*
_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=_tmp172;_tmp18B->tl=_tmp170;
_tmp18B;});break;}}}}name=0;goto _LL179;}_LL182: if(*((int*)_tmp173)!= 7)goto
_LL184;_tmp178=((struct Cyc_Absyn_Typedef_d_struct*)_tmp173)->f1;_LL183: {struct
_tuple0 _tmp18D;struct _tagged_arr*_tmp18E;struct _tuple0*_tmp18C=_tmp178->name;
_tmp18D=*_tmp18C;_tmp18E=_tmp18D.f2;name=(struct _tagged_arr*)_tmp18E;goto _LL179;}
_LL184: if(*((int*)_tmp173)!= 2)goto _LL186;_LL185: goto _LL187;_LL186: if(*((int*)
_tmp173)!= 5)goto _LL188;_LL187: goto _LL189;_LL188: if(*((int*)_tmp173)!= 3)goto
_LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp173)!= 8)goto _LL18C;_LL18B: goto
_LL18D;_LL18C: if(*((int*)_tmp173)!= 9)goto _LL18E;_LL18D: goto _LL18F;_LL18E: if(*((
int*)_tmp173)!= 10)goto _LL179;_LL18F: name=0;goto _LL179;_LL179:;}if(name != 0?((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp16F,(
struct _tagged_arr*)_check_null(name)): 0)_tmp170=({struct Cyc_List_List*_tmp18F=
_cycalloc(sizeof(*_tmp18F));_tmp18F->hd=_tmp172;_tmp18F->tl=_tmp170;_tmp18F;});}}
_tmp170=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp170);maybe=Cyc_Std_fopen(filename,_tag_arr("w",sizeof(unsigned char),2));if(
!((unsigned int)maybe))return 1;out_file=(struct Cyc_Std___cycFILE*)_check_null(
maybe);({struct Cyc_Std_String_pa_struct _tmp191;_tmp191.tag=0;_tmp191.f1=(struct
_tagged_arr)_tmp145;{void*_tmp190[1]={& _tmp191};Cyc_Std_fprintf(out_file,
_tag_arr("#include <%s>\nusing Std;\n",sizeof(unsigned char),26),_tag_arr(
_tmp190,sizeof(void*),1));}});Cyc_Std_fclose(out_file);maybe=Cyc_Std_fopen((
struct _tagged_arr)_tmp145,_tag_arr("w",sizeof(unsigned char),2));if(!((
unsigned int)maybe))return 1;out_file=(struct Cyc_Std___cycFILE*)_check_null(maybe);{
struct _tagged_arr ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1EA;_tmp1EA.tag=
0;_tmp1EA.f1=(struct _tagged_arr)filename;{void*_tmp1E9[1]={& _tmp1EA};Cyc_Std_aprintf(
_tag_arr("_%s_",sizeof(unsigned char),5),_tag_arr(_tmp1E9,sizeof(void*),1));}});{
int _tmp192=0;for(0;_tmp192 < _get_arr_size(ifdefmacro,sizeof(unsigned char));
_tmp192 ++){if(((unsigned char*)ifdefmacro.curr)[_tmp192]== '.'?1:((unsigned char*)
ifdefmacro.curr)[_tmp192]== '/')((unsigned char*)ifdefmacro.curr)[_tmp192]='_';
else{if(((unsigned char*)ifdefmacro.curr)[_tmp192]!= '_'?((unsigned char*)
ifdefmacro.curr)[_tmp192]!= '/': 0)((unsigned char*)ifdefmacro.curr)[_tmp192]=(
unsigned char)toupper((int)((unsigned char*)ifdefmacro.curr)[_tmp192]);}}}({
struct Cyc_Std_String_pa_struct _tmp195;_tmp195.tag=0;_tmp195.f1=(struct
_tagged_arr)ifdefmacro;{struct Cyc_Std_String_pa_struct _tmp194;_tmp194.tag=0;
_tmp194.f1=(struct _tagged_arr)ifdefmacro;{void*_tmp193[2]={& _tmp194,& _tmp195};
Cyc_Std_fprintf(out_file,_tag_arr("#ifndef %s\n#define %s\n",sizeof(
unsigned char),23),_tag_arr(_tmp193,sizeof(void*),2));}}});if(prologue != 0){
struct Cyc_List_List*_tmp196=prologue;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){
struct _tuple11 _tmp198;struct _tagged_arr _tmp199;struct _tagged_arr _tmp19A;struct
_tuple11*_tmp197=(struct _tuple11*)_tmp196->hd;_tmp198=*_tmp197;_tmp199=_tmp198.f1;
_tmp19A=_tmp198.f2;if(_tmp19A.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
_tmp199.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(defined_symbols,({struct _tagged_arr*
_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=_tmp199;_tmp19B;})): 0)Cyc_Std_fputs(
_tmp19A,out_file);else{({struct Cyc_Std_String_pa_struct _tmp19D;_tmp19D.tag=0;
_tmp19D.f1=(struct _tagged_arr)_tmp199;{void*_tmp19C[1]={& _tmp19D};Cyc_log(
_tag_arr("%s is not supported on this platform\n",sizeof(unsigned char),38),
_tag_arr(_tmp19C,sizeof(void*),1));}});}}}({void*_tmp19E[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("\nnamespace Std {\n",sizeof(unsigned char),18),_tag_arr(
_tmp19E,sizeof(void*),0));});{struct Cyc_List_List*_tmp19F=_tmp170;for(0;_tmp19F
!= 0;_tmp19F=_tmp19F->tl){struct Cyc_Absyn_Decl*_tmp1A0=(struct Cyc_Absyn_Decl*)
_tmp19F->hd;int _tmp1A1=0;struct _tagged_arr*name;{void*_tmp1A2=(void*)_tmp1A0->r;
struct Cyc_Absyn_Vardecl*_tmp1A3;struct Cyc_Absyn_Fndecl*_tmp1A4;struct Cyc_Absyn_Aggrdecl*
_tmp1A5;struct Cyc_Absyn_Enumdecl*_tmp1A6;struct Cyc_Absyn_Typedefdecl*_tmp1A7;
_LL191: if(*((int*)_tmp1A2)!= 0)goto _LL193;_tmp1A3=((struct Cyc_Absyn_Var_d_struct*)
_tmp1A2)->f1;_LL192: {struct _tuple0 _tmp1A9;struct _tagged_arr*_tmp1AA;struct
_tuple0*_tmp1A8=_tmp1A3->name;_tmp1A9=*_tmp1A8;_tmp1AA=_tmp1A9.f2;name=(struct
_tagged_arr*)_tmp1AA;goto _LL190;}_LL193: if(*((int*)_tmp1A2)!= 1)goto _LL195;
_tmp1A4=((struct Cyc_Absyn_Fn_d_struct*)_tmp1A2)->f1;_LL194: {struct _tuple0
_tmp1AC;struct _tagged_arr*_tmp1AD;struct _tuple0*_tmp1AB=_tmp1A4->name;_tmp1AC=*
_tmp1AB;_tmp1AD=_tmp1AC.f2;name=(struct _tagged_arr*)_tmp1AD;goto _LL190;}_LL195:
if(*((int*)_tmp1A2)!= 4)goto _LL197;_tmp1A5=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp1A2)->f1;_LL196: {struct _tuple0 _tmp1AF;struct _tagged_arr*_tmp1B0;struct
_tuple0*_tmp1AE=_tmp1A5->name;_tmp1AF=*_tmp1AE;_tmp1B0=_tmp1AF.f2;name=(struct
_tagged_arr*)_tmp1B0;goto _LL190;}_LL197: if(*((int*)_tmp1A2)!= 6)goto _LL199;
_tmp1A6=((struct Cyc_Absyn_Enum_d_struct*)_tmp1A2)->f1;_LL198: {struct _tuple0
_tmp1B2;struct _tagged_arr*_tmp1B3;struct _tuple0*_tmp1B1=_tmp1A6->name;_tmp1B2=*
_tmp1B1;_tmp1B3=_tmp1B2.f2;name=(struct _tagged_arr*)_tmp1B3;goto _LL190;}_LL199:
if(*((int*)_tmp1A2)!= 7)goto _LL19B;_tmp1A7=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1A2)->f1;_LL19A: {struct _tuple0 _tmp1B5;struct _tagged_arr*_tmp1B6;struct
_tuple0*_tmp1B4=_tmp1A7->name;_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.f2;name=(struct
_tagged_arr*)_tmp1B6;goto _LL190;}_LL19B: if(*((int*)_tmp1A2)!= 2)goto _LL19D;
_LL19C: goto _LL19E;_LL19D: if(*((int*)_tmp1A2)!= 5)goto _LL19F;_LL19E: goto _LL1A0;
_LL19F: if(*((int*)_tmp1A2)!= 3)goto _LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if(*((int*)
_tmp1A2)!= 8)goto _LL1A3;_LL1A2: goto _LL1A4;_LL1A3: if(*((int*)_tmp1A2)!= 9)goto
_LL1A5;_LL1A4: goto _LL1A6;_LL1A5: if(*((int*)_tmp1A2)!= 10)goto _LL190;_LL1A6: name=
0;goto _LL190;_LL190:;}if(!((unsigned int)name)?!_tmp1A1: 0)continue;if((
unsigned int)name){ifdefmacro=({struct Cyc_Std_String_pa_struct _tmp1B8;_tmp1B8.tag=
0;_tmp1B8.f1=(struct _tagged_arr)*name;{void*_tmp1B7[1]={& _tmp1B8};Cyc_Std_aprintf(
_tag_arr("_%s_def_",sizeof(unsigned char),9),_tag_arr(_tmp1B7,sizeof(void*),1));}});({
struct Cyc_Std_String_pa_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp1B9[1]={& _tmp1BA};Cyc_Std_fprintf(out_file,
_tag_arr("#ifndef %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1B9,sizeof(void*),
1));}});({struct Cyc_Std_String_pa_struct _tmp1BC;_tmp1BC.tag=0;_tmp1BC.f1=(struct
_tagged_arr)ifdefmacro;{void*_tmp1BB[1]={& _tmp1BC};Cyc_Std_fprintf(out_file,
_tag_arr("#define %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1BB,sizeof(void*),
1));}});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp1BD[1];_tmp1BD[0]=
_tmp1A0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp1BD,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);({void*_tmp1BE[0]={};Cyc_Std_fprintf(
out_file,_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(_tmp1BE,sizeof(
void*),0));});}else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp1BF[1];
_tmp1BF[0]=_tmp1A0;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp1BF,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}_tmp14C=({
struct Cyc_Std_String_pa_struct _tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=(struct
_tagged_arr)_tmp141;{struct Cyc_Std_String_pa_struct _tmp1C2;_tmp1C2.tag=0;_tmp1C2.f1=(
struct _tagged_arr)_tmp142;{struct Cyc_Std_String_pa_struct _tmp1C1;_tmp1C1.tag=0;
_tmp1C1.f1=(struct _tagged_arr)_tmp146;{void*_tmp1C0[3]={& _tmp1C1,& _tmp1C2,&
_tmp1C3};Cyc_Std_aprintf(_tag_arr("%s -E -dM -o %s -x c %s",sizeof(unsigned char),
24),_tag_arr(_tmp1C0,sizeof(void*),3));}}}});if(Cyc_Std_system((struct
_tagged_arr)_tmp14C)!= 0)return 1;maybe=Cyc_Std_fopen((struct _tagged_arr)_tmp142,
_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)maybe))return 1;in_file=(
struct Cyc_Std___cycFILE*)_check_null(maybe);_tmp15A=Cyc_Lexing_from_file(in_file);{
struct _tuple10*entry2;while((entry2=((struct _tuple10*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp15A))!= 0){struct _tuple10 _tmp1C5;struct _tagged_arr
_tmp1C6;struct _tagged_arr*_tmp1C7;struct _tuple10*_tmp1C4=(struct _tuple10*)
_check_null(entry2);_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.f1;_tmp1C7=_tmp1C5.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp16F,
_tmp1C7)){({struct Cyc_Std_String_pa_struct _tmp1C9;_tmp1C9.tag=0;_tmp1C9.f1=(
struct _tagged_arr)*_tmp1C7;{void*_tmp1C8[1]={& _tmp1C9};Cyc_Std_fprintf(out_file,
_tag_arr("#ifndef %s\n",sizeof(unsigned char),12),_tag_arr(_tmp1C8,sizeof(void*),
1));}});({struct Cyc_Std_String_pa_struct _tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(struct
_tagged_arr)_tmp1C6;{void*_tmp1CA[1]={& _tmp1CB};Cyc_Std_fprintf(out_file,
_tag_arr("%s\n",sizeof(unsigned char),4),_tag_arr(_tmp1CA,sizeof(void*),1));}});({
void*_tmp1CC[0]={};Cyc_Std_fprintf(out_file,_tag_arr("#endif\n",sizeof(
unsigned char),8),_tag_arr(_tmp1CC,sizeof(void*),0));});}}Cyc_Std_fclose(in_file);
Cyc_Std_remove((struct _tagged_arr)_tmp141);Cyc_Std_remove((struct _tagged_arr)
_tmp142);if(epilogue != 0){struct Cyc_List_List*_tmp1CD=epilogue;for(0;_tmp1CD != 0;
_tmp1CD=_tmp1CD->tl){struct _tuple11 _tmp1CF;struct _tagged_arr _tmp1D0;struct
_tagged_arr _tmp1D1;struct _tuple11*_tmp1CE=(struct _tuple11*)_tmp1CD->hd;_tmp1CF=*
_tmp1CE;_tmp1D0=_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;if(_tmp1D1.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?_tmp1D0.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[
0]=_tmp1D0;_tmp1D2;})): 0)Cyc_Std_fputs(_tmp1D1,out_file);else{({struct Cyc_Std_String_pa_struct
_tmp1D4;_tmp1D4.tag=0;_tmp1D4.f1=(struct _tagged_arr)_tmp1D0;{void*_tmp1D3[1]={&
_tmp1D4};Cyc_log(_tag_arr("%s is not supported on this platform\n",sizeof(
unsigned char),38),_tag_arr(_tmp1D3,sizeof(void*),1));}});}}}({void*_tmp1D5[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("}\n",sizeof(unsigned char),3),_tag_arr(
_tmp1D5,sizeof(void*),0));});({void*_tmp1D6[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("#endif\n",sizeof(unsigned char),8),_tag_arr(_tmp1D6,sizeof(void*),0));});
Cyc_Std_fclose(out_file);if(cstubs != 0){out_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_cstubs_file);{struct Cyc_List_List*_tmp1D7=cstubs;for(0;_tmp1D7 != 
0;_tmp1D7=_tmp1D7->tl){struct _tuple11 _tmp1D9;struct _tagged_arr _tmp1DA;struct
_tagged_arr _tmp1DB;struct _tuple11*_tmp1D8=(struct _tuple11*)_tmp1D7->hd;_tmp1D9=*
_tmp1D8;_tmp1DA=_tmp1D9.f1;_tmp1DB=_tmp1D9.f2;if(_tmp1DB.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?_tmp1DA.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr?1:((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[
0]=_tmp1DA;_tmp1DC;})): 0)Cyc_Std_fputs(_tmp1DB,out_file);}}}out_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file);({struct Cyc_Std_String_pa_struct _tmp1DE;_tmp1DE.tag=
0;_tmp1DE.f1=(struct _tagged_arr)_tmp145;{void*_tmp1DD[1]={& _tmp1DE};Cyc_Std_fprintf(
out_file,_tag_arr("#include <%s>\n\n",sizeof(unsigned char),16),_tag_arr(_tmp1DD,
sizeof(void*),1));}});if(cycstubs != 0){out_file=(struct Cyc_Std___cycFILE*)
_check_null(Cyc_cycstubs_file);({void*_tmp1DF[0]={};Cyc_Std_fprintf(out_file,
_tag_arr("namespace Std {\n",sizeof(unsigned char),17),_tag_arr(_tmp1DF,sizeof(
void*),0));});{struct Cyc_List_List*_tmp1E0=cycstubs;for(0;_tmp1E0 != 0;_tmp1E0=
_tmp1E0->tl){struct _tuple11 _tmp1E2;struct _tagged_arr _tmp1E3;struct _tagged_arr
_tmp1E4;struct _tuple11*_tmp1E1=(struct _tuple11*)_tmp1E0->hd;_tmp1E2=*_tmp1E1;
_tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;if(_tmp1E4.curr != ((struct _tagged_arr)
_tag_arr(0,0,0)).curr?_tmp1E3.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
defined_symbols,({struct _tagged_arr*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5[
0]=_tmp1E3;_tmp1E5;})): 0)Cyc_Std_fputs(_tmp1E4,out_file);}}({void*_tmp1E6[0]={};
Cyc_Std_fprintf(out_file,_tag_arr("}\n\n",sizeof(unsigned char),4),_tag_arr(
_tmp1E6,sizeof(void*),0));});}if(Cyc_Std_chdir((struct _tagged_arr)_tmp13A)){({
struct Cyc_Std_String_pa_struct _tmp1E8;_tmp1E8.tag=0;_tmp1E8.f1=(struct
_tagged_arr)_tmp13A;{void*_tmp1E7[1]={& _tmp1E8};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: could not change directory to %s\n",sizeof(unsigned char),41),
_tag_arr(_tmp1E7,sizeof(void*),1));}});return 1;}return 0;}}}}}}}}}}int Cyc_process_specfile(
struct _tagged_arr file,struct _tagged_arr dir){struct Cyc_Std___cycFILE*_tmp1F6=Cyc_Std_fopen(
file,_tag_arr("r",sizeof(unsigned char),2));if(!((unsigned int)_tmp1F6))return 1;{
struct Cyc_Std___cycFILE*_tmp1F7=(struct Cyc_Std___cycFILE*)_check_null(_tmp1F6);
struct Cyc_Lexing_lexbuf*_tmp1F8=Cyc_Lexing_from_file(_tmp1F7);struct _tuple12*
entry;while((entry=((struct _tuple12*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp1F8))!= 0){struct _tuple12 _tmp1FA;struct _tagged_arr _tmp1FB;struct Cyc_List_List*
_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*
_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp201;struct _tuple12*
_tmp1F9=(struct _tuple12*)_check_null(entry);_tmp1FA=*_tmp1F9;_tmp1FB=_tmp1FA.f1;
_tmp1FC=_tmp1FA.f2;_tmp1FD=_tmp1FA.f3;_tmp1FE=_tmp1FA.f4;_tmp1FF=_tmp1FA.f5;
_tmp200=_tmp1FA.f6;_tmp201=_tmp1FA.f7;if(Cyc_process_file(_tmp1FB,dir,_tmp1FC,
_tmp1FD,_tmp1FE,_tmp1FF,_tmp200,_tmp201)){Cyc_Std_fclose(_tmp1F7);return 1;}}Cyc_Std_fclose(
_tmp1F7);return 0;}}int Cyc_getsize(struct _tagged_arr dir,struct _tagged_arr includes,
struct _tagged_arr type){struct _tagged_arr _tmp202=Cyc_Filename_concat(dir,_tag_arr("getsize.c",
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
