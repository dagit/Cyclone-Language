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
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern struct Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List*
x); extern unsigned char Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Arg_Error[
10u]; static const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; static const int Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_FlagString_spec=
2; struct Cyc_Arg_FlagString_spec_struct{ int tag; void(* f1)( struct
_tagged_arr, struct _tagged_arr); } ; static const int Cyc_Arg_Set_spec= 3;
struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec=
4; struct Cyc_Arg_Clear_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_String_spec=
5; struct Cyc_Arg_String_spec_struct{ int tag; void(* f1)( struct _tagged_arr);
} ; static const int Cyc_Arg_Int_spec= 6; struct Cyc_Arg_Int_spec_struct{ int
tag; void(* f1)( int); } ; static const int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct
Cyc_List_List* specs, void(* anonfun)( struct _tagged_arr), struct _tagged_arr
errmsg, struct _tagged_arr args); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stdout; extern struct Cyc_Std___sFILE*
Cyc_Std_stderr; extern int Cyc_Std_remove( struct _tagged_arr); extern int Cyc_Std_fclose(
struct Cyc_Std___sFILE*); extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*);
extern struct Cyc_Std___sFILE* Cyc_Std_fopen( struct _tagged_arr __filename,
struct _tagged_arr __modes); extern unsigned int Cyc_Std_fread( struct
_tagged_arr __ptr, unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE*
__stream); extern unsigned int Cyc_Std_fwrite( struct _tagged_arr __ptr,
unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE* __s); extern int
Cyc_Std_feof( struct Cyc_Std___sFILE* __stream); extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct Cyc_Std___sFILE*
Cyc_Std_file_open( struct _tagged_arr fname, struct _tagged_arr mode); extern
void Cyc_Std_file_close( struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa=
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
int tag; float* f1; } ; extern unsigned char Cyc_Array_Array_mismatch[ 19u];
struct Cyc_Std__Div{ int quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot; int
rem; } ; extern int abs( int __x); extern int atexit( void(* __func)()); extern
struct Cyc_Std__Div div( int __numer, int __denom); extern struct Cyc_Std__Ldiv
ldiv( int __numer, int __denom); extern int random(); extern void srandom(
unsigned int __seed); extern int rand(); extern void srand( unsigned int __seed);
extern int rand_r( unsigned int* __seed); extern int grantpt( int __fd); extern
int unlockpt( int __fd); extern struct _tagged_arr Cyc_Std_getenv( struct
_tagged_arr); extern int Cyc_Std_system( struct _tagged_arr); extern
unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern int Cyc_Std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_strconcat_l(
struct Cyc_List_List*); extern struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_substring( struct
_tagged_arr, int ofs, unsigned int n); extern struct _tagged_arr Cyc_Std_strchr(
struct _tagged_arr s, unsigned char c); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct _tagged_arr Cyc_Filename_concat( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension( struct
_tagged_arr); extern int Cyc_Filename_check_suffix( struct _tagged_arr, struct
_tagged_arr); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; extern void Cyc_Position_reset_position( struct _tagged_arr); struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_error_p(); static const int Cyc_Absyn_Loc_n= 0; static const
int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int
tag; struct Cyc_List_List* f1; } ; struct _tuple0{ void* f1; struct _tagged_arr*
f2; } ; struct Cyc_Absyn_Conref; static const int Cyc_Absyn_Static= 0; static
const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2; static
const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{
int q_const: 1; int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1=
0; static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static
const int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const
int Cyc_Absyn_MemKind= 1; static const int Cyc_Absyn_BoxKind= 2; static const
int Cyc_Absyn_RgnKind= 3; static const int Cyc_Absyn_EffKind= 4; static const
int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{
void* v; } ; static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; static const int Cyc_Absyn_Eq_kb= 0; struct Cyc_Absyn_Eq_kb_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Unknown_kb= 1; struct Cyc_Absyn_Unknown_kb_struct{
int tag; struct Cyc_Core_Opt* f1; } ; static const int Cyc_Absyn_Less_kb= 2;
struct Cyc_Absyn_Less_kb_struct{ int tag; struct Cyc_Core_Opt* f1; void* f2; } ;
struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; void* kind; } ;
static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
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
15u]; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern int Cyc_yyparse(); extern unsigned char Cyc_AbstractDeclarator_tok[ 27u];
struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern unsigned char Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[
18u]; struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
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
} ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int generate_line_directives: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); extern void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params* fs); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; static const
int Cyc_Tcenv_AnonEnumRes= 4; struct Cyc_Tcenv_AnonEnumRes_struct{ int tag; void*
f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict*
typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; }
; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0; static const
int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2; static const
int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct
Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern unsigned char Cyc_Tcutil_TypeErr[
12u]; extern void Cyc_Tcutil_flush_warnings(); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int var_default_init, struct Cyc_List_List* ds); extern struct Cyc_List_List*
Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); extern
struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List* ds); extern struct Cyc_List_List*
Cyc_Tovc_tovc( struct Cyc_List_List* decls); static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_CfFlowInfo_Place{ void* root;
struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_NoneIL= 0;
static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; extern unsigned char Cyc_CfFlowInfo_IsZero[ 11u]; extern unsigned char Cyc_CfFlowInfo_NotZero[
12u]; extern unsigned char Cyc_CfFlowInfo_UnknownZ[ 13u]; static const int Cyc_CfFlowInfo_PlaceL=
0; struct Cyc_CfFlowInfo_PlaceL_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1;
} ; static const int Cyc_CfFlowInfo_UnknownL= 0; static const int Cyc_CfFlowInfo_Zero=
0; static const int Cyc_CfFlowInfo_NotZeroAll= 1; static const int Cyc_CfFlowInfo_NotZeroThis=
2; static const int Cyc_CfFlowInfo_UnknownR= 0; struct Cyc_CfFlowInfo_UnknownR_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
} ; extern void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds); struct
Cyc_Interface_I; extern struct Cyc_Interface_I* Cyc_Interface_empty(); extern
struct Cyc_Interface_I* Cyc_Interface_final(); extern struct Cyc_Interface_I*
Cyc_Interface_extract( struct Cyc_Dict_Dict* ae); struct _tuple8{ struct
_tagged_arr f1; struct _tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple8* info);
extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo); extern
struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Std___sFILE*); extern
void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Std___sFILE*);
static unsigned char _temp0[ 4u]="0.2"; static struct _tagged_arr Cyc_version={
_temp0, _temp0, _temp0 +  4u}; extern void Cyc_Lex_lex_init(); static int Cyc_pp_r=
0; static int Cyc_noexpand_r= 0; static int Cyc_noshake_r= 0; static int Cyc_stop_after_cpp_r=
0; static int Cyc_parseonly_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0;
static int Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r= 0; static int
Cyc_stop_after_asmfile_r= 0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0;
static int Cyc_save_temps_r= 0; static int Cyc_save_c_r= 0; static int Cyc_nogc_r=
0; static int Cyc_pa_r= 0; static int Cyc_nocheck_r= 0; static int Cyc_add_cyc_namespace_r=
1; static int Cyc_generate_line_directives_r= 0; static int Cyc_print_full_evars_r=
0; static int Cyc_print_all_tvars_r= 0; static int Cyc_print_all_kinds_r= 0;
static struct _tagged_arr* Cyc_output_file= 0; static void Cyc_set_output_file(
struct _tagged_arr s){ Cyc_output_file=({ struct _tagged_arr* _temp1=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1[ 0]= s; _temp1;});}
extern unsigned char* Cdef_inc_path; extern unsigned char* Cdef_lib_path; extern
unsigned char* Carch; extern unsigned char* Ccomp; static unsigned char _temp2[
1u]=""; static struct _tagged_arr Cyc_cpp={ _temp2, _temp2, _temp2 +  1u};
static void Cyc_set_cpp( struct _tagged_arr s){ Cyc_cpp= s;} static struct Cyc_List_List*
Cyc_cppargs= 0; static void Cyc_add_cpparg( struct _tagged_arr s){ Cyc_cppargs=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*)({ struct _tagged_arr* _temp4=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp4[ 0]= s; _temp4;});
_temp3->tl= Cyc_cppargs; _temp3;});} static void Cyc_print_version(){({ struct
Cyc_Std_String_pa_struct _temp6; _temp6.tag= Cyc_Std_String_pa; _temp6.f1=(
struct _tagged_arr) Cyc_version;{ void* _temp5[ 1u]={& _temp6}; Cyc_Std_printf(
_tag_arr("The Cyclone compiler, version %s\n", sizeof( unsigned char), 34u),
_tag_arr( _temp5, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp9; _temp9.tag= Cyc_Std_String_pa; _temp9.f1=( struct _tagged_arr)
Cstring_to_string( Ccomp);{ struct Cyc_Std_String_pa_struct _temp8; _temp8.tag=
Cyc_Std_String_pa; _temp8.f1=( struct _tagged_arr) Cstring_to_string( Carch);{
void* _temp7[ 2u]={& _temp8,& _temp9}; Cyc_Std_printf( _tag_arr("Compiled for architecture: %s CC=\"%s\"\n",
sizeof( unsigned char), 39u), _tag_arr( _temp7, sizeof( void*), 2u));}}});({
struct Cyc_Std_String_pa_struct _temp11; _temp11.tag= Cyc_Std_String_pa; _temp11.f1=(
struct _tagged_arr) Cstring_to_string( Cdef_lib_path);{ void* _temp10[ 1u]={&
_temp11}; Cyc_Std_printf( _tag_arr("Standard library directory: %s\n", sizeof(
unsigned char), 32u), _tag_arr( _temp10, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp13; _temp13.tag= Cyc_Std_String_pa; _temp13.f1=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path);{ void* _temp12[ 1u]={& _temp13}; Cyc_Std_printf(
_tag_arr("Standard include directory: %s\n", sizeof( unsigned char), 32u),
_tag_arr( _temp12, sizeof( void*), 1u));}}); exit( 0);} static int Cyc_is_cyclone_sourcefile(
struct _tagged_arr s){ unsigned int _temp14= Cyc_Std_strlen( s); if( _temp14 <= 
4){ return 0;} else{ return Cyc_Std_strcmp( _tagged_arr_plus( s, sizeof(
unsigned char),( int)( _temp14 -  4)), _tag_arr(".cyc", sizeof( unsigned char),
5u)) ==  0;}} static struct Cyc_List_List* Cyc_ccargs= 0; static void Cyc_add_ccarg(
struct _tagged_arr s){ Cyc_ccargs=({ struct Cyc_List_List* _temp15=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp15->hd=( void*)({ struct
_tagged_arr* _temp16=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp16[ 0]= s; _temp16;}); _temp15->tl= Cyc_ccargs; _temp15;});}
static struct Cyc_List_List* Cyc_cyclone_lib_path= 0; static void Cyc_add_cyclone_lib_path(
struct _tagged_arr s){ unsigned int _temp17= Cyc_Std_strlen( s); if( _temp17 <= 
2){ return;}{ struct _tagged_arr _temp18=( struct _tagged_arr) Cyc_Std_substring(
s, 2, _temp17 -  2); Cyc_cyclone_lib_path=({ struct Cyc_List_List* _temp19=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp19->hd=(
void*)({ struct _tagged_arr* _temp20=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp20[ 0]= _temp18; _temp20;}); _temp19->tl= Cyc_cyclone_lib_path;
_temp19;});}} static struct Cyc_List_List* Cyc_libargs= 0; static void Cyc_add_libarg(
struct _tagged_arr s){ if( Cyc_Std_strcmp( s, _tag_arr("-lxml", sizeof(
unsigned char), 6u)) ==  0){ Cyc_add_ccarg( s);} else{ Cyc_libargs=({ struct Cyc_List_List*
_temp21=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp21->hd=( void*)({ struct _tagged_arr* _temp22=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp22[ 0]= s; _temp22;}); _temp21->tl=
Cyc_libargs; _temp21;});}} static void Cyc_set_save_temps(){ Cyc_save_temps_r= 1;
Cyc_add_ccarg( _tag_arr("-save-temps", sizeof( unsigned char), 12u));} static
int Cyc_produce_dependencies= 0; static void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r=
1; Cyc_produce_dependencies= 1; Cyc_add_cpparg( _tag_arr("-M", sizeof(
unsigned char), 3u));} static struct _tagged_arr* Cyc_dependencies_target= 0;
static void Cyc_set_dependencies_target( struct _tagged_arr s){ Cyc_dependencies_target=({
struct _tagged_arr* _temp23=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp23[ 0]= s; _temp23;});} static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r= 1; Cyc_add_ccarg( _tag_arr("-c", sizeof(
unsigned char), 3u));} static void Cyc_set_nocppprecomp(){ Cyc_add_cpparg(
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u)); Cyc_add_ccarg(
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u));} static void Cyc_set_lineno(){
Cyc_generate_line_directives_r= 1; Cyc_set_save_temps();} static void Cyc_set_noboundschecks(){
Cyc_add_ccarg( _tag_arr("-DNO_CYC_BOUNDS_CHECKS", sizeof( unsigned char), 23u));}
static void Cyc_set_nonullchecks(){ Cyc_add_ccarg( _tag_arr("-DNO_CYC_NULL_CHECKS",
sizeof( unsigned char), 21u));} static void Cyc_set_nochecks(){ Cyc_set_noboundschecks();
Cyc_set_nonullchecks(); Cyc_nocheck_r= 1;} static void Cyc_set_nocyc(){ Cyc_add_cyc_namespace_r=
0; Cyc_add_ccarg( _tag_arr("-DNO_CYC_PREFIX", sizeof( unsigned char), 16u));}
static void Cyc_set_pa(){ Cyc_pa_r= 1; Cyc_add_ccarg( _tag_arr("-DCYC_REGION_PROFILE",
sizeof( unsigned char), 21u));} static void Cyc_set_stop_after_asmfile(){ Cyc_stop_after_asmfile_r=
1; Cyc_add_ccarg( _tag_arr("-S", sizeof( unsigned char), 3u));} static const int
Cyc_DEFAULTINPUT= 0; static const int Cyc_CYCLONEFILE= 1; static void* Cyc_intype=(
void*) 0u; static void Cyc_set_inputtype( struct _tagged_arr s){ if( Cyc_Std_strcmp(
s, _tag_arr("cyc", sizeof( unsigned char), 4u)) ==  0){ Cyc_intype=( void*) Cyc_CYCLONEFILE;}
else{ if( Cyc_Std_strcmp( s, _tag_arr("none", sizeof( unsigned char), 5u)) ==  0){
Cyc_intype=( void*) Cyc_DEFAULTINPUT;} else{({ struct Cyc_Std_String_pa_struct
_temp25; _temp25.tag= Cyc_Std_String_pa; _temp25.f1=( struct _tagged_arr) s;{
void* _temp24[ 1u]={& _temp25}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Input type '%s' not supported\n",
sizeof( unsigned char), 31u), _tag_arr( _temp24, sizeof( void*), 1u));}});}}}
static struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other(
struct _tagged_arr s){ if( Cyc_intype == ( void*) Cyc_CYCLONEFILE? 1: Cyc_is_cyclone_sourcefile(
s)){ Cyc_cyclone_files=({ struct Cyc_List_List* _temp26=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp26->hd=( void*)({ struct
_tagged_arr* _temp27=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp27[ 0]= s; _temp27;}); _temp26->tl= Cyc_cyclone_files;
_temp26;});{ struct _tagged_arr _temp28= Cyc_Filename_chop_extension( s); struct
_tagged_arr _temp29= Cyc_Std_strconcat(( struct _tagged_arr) _temp28, _tag_arr(".c",
sizeof( unsigned char), 3u)); Cyc_add_ccarg(( struct _tagged_arr) _temp29);}}
else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct _tagged_arr s){
if( Cyc_save_temps_r){ return;} else{ Cyc_Std_remove( s);}} int Cyc_compile_failure=
0; struct Cyc_Std___sFILE* Cyc_try_file_open( struct _tagged_arr filename,
struct _tagged_arr mode, struct _tagged_arr msg_part){ struct _handler_cons
_temp30; _push_handler(& _temp30);{ int _temp32= 0; if( setjmp( _temp30.handler)){
_temp32= 1;} if( ! _temp32){{ struct Cyc_Std___sFILE* _temp33=( struct Cyc_Std___sFILE*)
Cyc_Std_file_open( filename, mode); _npop_handler( 0u); return _temp33;};
_pop_handler();} else{ void* _temp31=( void*) _exn_thrown; void* _temp35=
_temp31; _LL37: goto _LL38; _LL39: goto _LL40; _LL38: Cyc_compile_failure= 1;({
struct Cyc_Std_String_pa_struct _temp43; _temp43.tag= Cyc_Std_String_pa; _temp43.f1=(
struct _tagged_arr) filename;{ struct Cyc_Std_String_pa_struct _temp42; _temp42.tag=
Cyc_Std_String_pa; _temp42.f1=( struct _tagged_arr) msg_part;{ void* _temp41[ 2u]={&
_temp42,& _temp43}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: couldn't open %s %s\n",
sizeof( unsigned char), 29u), _tag_arr( _temp41, sizeof( void*), 2u));}}}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return 0; _LL40:( void) _throw(
_temp35); _LL36:;}}} struct Cyc_List_List* Cyc_do_stage( struct _tagged_arr
stage_name, struct Cyc_List_List*(* f)( void*, struct Cyc_List_List*), void* env,
struct Cyc_List_List* tds){ struct _tagged_arr exn_string= _tag_arr("", sizeof(
unsigned char), 1u); struct _tagged_arr explain_string= _tag_arr("", sizeof(
unsigned char), 1u); int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct*
_temp76=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp76[ 0]=({ struct Cyc_Core_Impossible_struct _temp77; _temp77.tag= Cyc_Core_Impossible;
_temp77.f1= _tag_arr("", sizeof( unsigned char), 1u); _temp77;}); _temp76;});
struct Cyc_List_List* _temp44= 0;{ struct _handler_cons _temp45; _push_handler(&
_temp45);{ int _temp47= 0; if( setjmp( _temp45.handler)){ _temp47= 1;} if( !
_temp47){ _temp44= f( env, tds);; _pop_handler();} else{ void* _temp46=( void*)
_exn_thrown; void* _temp49= _temp46; struct _tagged_arr _temp63; struct
_tagged_arr _temp65; struct _tagged_arr _temp67; _LL51: if(*(( void**) _temp49)
==  Cyc_Core_Impossible){ _LL64: _temp63=(( struct Cyc_Core_Impossible_struct*)
_temp49)->f1; goto _LL52;} else{ goto _LL53;} _LL53: if( _temp49 ==  Cyc_Dict_Absent){
goto _LL54;} else{ goto _LL55;} _LL55: if(*(( void**) _temp49) ==  Cyc_Core_Invalid_argument){
_LL66: _temp65=(( struct Cyc_Core_Invalid_argument_struct*) _temp49)->f1; goto
_LL56;} else{ goto _LL57;} _LL57: if(*(( void**) _temp49) ==  Cyc_Core_Failure){
_LL68: _temp67=(( struct Cyc_Core_Failure_struct*) _temp49)->f1; goto _LL58;}
else{ goto _LL59;} _LL59: goto _LL60; _LL61: goto _LL62; _LL52: exn_string=
_tag_arr("Exception Core::Impossible", sizeof( unsigned char), 27u);
explain_string= _temp63; goto _LL50; _LL54: exn_string= _tag_arr("Exception Dict::Absent",
sizeof( unsigned char), 23u); goto _LL50; _LL56: exn_string= _tag_arr("Exception Core::Invalid_argument",
sizeof( unsigned char), 33u); explain_string= _temp65; goto _LL50; _LL58:
exn_string= _tag_arr("Exception Core::Failure", sizeof( unsigned char), 24u);
explain_string= _temp67; goto _LL50; _LL60: ex= _temp49; other_exn= 1;
exn_string= _tag_arr("Uncaught exception", sizeof( unsigned char), 19u); goto
_LL50; _LL62:( void) _throw( _temp49); _LL50:;}}} if( Cyc_Position_error_p()){
Cyc_compile_failure= 1;} if( Cyc_Std_strcmp( exn_string, _tag_arr("", sizeof(
unsigned char), 1u)) !=  0){ Cyc_compile_failure= 1;({ struct Cyc_Std_String_pa_struct
_temp72; _temp72.tag= Cyc_Std_String_pa; _temp72.f1=( struct _tagged_arr)
explain_string;{ struct Cyc_Std_String_pa_struct _temp71; _temp71.tag= Cyc_Std_String_pa;
_temp71.f1=( struct _tagged_arr) stage_name;{ struct Cyc_Std_String_pa_struct
_temp70; _temp70.tag= Cyc_Std_String_pa; _temp70.f1=( struct _tagged_arr)
exn_string;{ void* _temp69[ 3u]={& _temp70,& _temp71,& _temp72}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n%s thrown during %s: %s", sizeof( unsigned char), 25u),
_tag_arr( _temp69, sizeof( void*), 3u));}}}});} if( Cyc_compile_failure){({ void*
_temp73[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nCOMPILATION FAILED!\n",
sizeof( unsigned char), 22u), _tag_arr( _temp73, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); if( other_exn){( int) _throw( ex);}
return _temp44;} else{ if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp75;
_temp75.tag= Cyc_Std_String_pa; _temp75.f1=( struct _tagged_arr) stage_name;{
void* _temp74[ 1u]={& _temp75}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s completed.\n",
sizeof( unsigned char), 15u), _tag_arr( _temp74, sizeof( void*), 1u));}}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return _temp44;}} return _temp44;}
struct Cyc_List_List* Cyc_do_parse( struct Cyc_Std___sFILE* f, struct Cyc_List_List*
ignore){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp78= Cyc_Parse_parse_file(
f); Cyc_Lex_lex_init(); return _temp78;}} struct Cyc_List_List* Cyc_do_typecheck(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds);
if( ! Cyc_noshake_r){ tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List*
Cyc_do_cfcheck( int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check(
tds); return tds;} struct _tuple9{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Std___sFILE*
f2; struct Cyc_Std___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple9* params, struct Cyc_List_List* tds){ struct _tuple9 _temp81;
struct Cyc_Std___sFILE* _temp82; struct Cyc_Std___sFILE* _temp84; struct Cyc_Tcenv_Tenv*
_temp86; struct _tuple9* _temp79= params; _temp81=* _temp79; _LL87: _temp86=
_temp81.f1; goto _LL85; _LL85: _temp84= _temp81.f2; goto _LL83; _LL83: _temp82=
_temp81.f3; goto _LL80; _LL80: { struct Cyc_Interface_I* _temp88= Cyc_Interface_extract(
_temp86->ae); if( _temp84 ==  0){ Cyc_Interface_save( _temp88, _temp82);} else{
struct Cyc_Interface_I* _temp89= Cyc_Interface_parse(( struct Cyc_Std___sFILE*)
_check_null( _temp84)); if( ! Cyc_Interface_is_subinterface( _temp89, _temp88,({
struct _tuple8* _temp90=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp90->f1= _tag_arr("written interface", sizeof( unsigned char), 18u); _temp90->f2=
_tag_arr("maximal interface", sizeof( unsigned char), 18u); _temp90;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp89, _temp82);}} return tds;}} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_tovc( int ignore, struct Cyc_List_List* tds){
return Cyc_Tovc_tovc( tds);} static unsigned char _temp91[ 1u]=""; static struct
_tagged_arr Cyc_cyc_include={ _temp91, _temp91, _temp91 +  1u}; struct Cyc_List_List*
Cyc_do_print( struct Cyc_Std___sFILE* out_file, struct Cyc_List_List* tds){
struct Cyc_Absynpp_Params params_r; if( Cyc_tc_r){ params_r= Cyc_Absynpp_cyc_params_r;}
else{ params_r= Cyc_Absynpp_c_params_r;} params_r.expand_typedefs= ! Cyc_noexpand_r;
params_r.to_VC= Cyc_tovc_r; params_r.add_cyc_prefix= Cyc_add_cyc_namespace_r;
params_r.generate_line_directives= Cyc_generate_line_directives_r; params_r.print_full_evars=
Cyc_print_full_evars_r; params_r.print_all_tvars= Cyc_print_all_tvars_r;
params_r.print_all_kinds= Cyc_print_all_kinds_r;{ struct Cyc_Std___sFILE*
_temp92= Cyc_try_file_open( Cyc_cyc_include, _tag_arr("r", sizeof( unsigned char),
2u), _tag_arr("internal compiler file", sizeof( unsigned char), 23u)); if(
_temp92 ==  0){ return tds;}{ struct Cyc_Std___sFILE* _temp93=( struct Cyc_Std___sFILE*)
_check_null( _temp92); unsigned int n_read= 1024; unsigned int n_written;
unsigned char buf[ 1024u];{ unsigned int _temp96= 1024u; unsigned int i; for( i=
0; i <  _temp96; i ++){ buf[ i]='\000';}} while( n_read ==  1024) { n_read= Cyc_Std_fread(
_tag_arr( buf, sizeof( unsigned char), 1024u), 1, 1024, _temp93); if( n_read != 
1024? ! Cyc_Std_feof( _temp93): 0){ Cyc_compile_failure= 1;({ void* _temp94[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: problem copying cyc_include.h\n",
sizeof( unsigned char), 39u), _tag_arr( _temp94, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return tds;} n_written= Cyc_Std_fwrite(
_tag_arr( buf, sizeof( unsigned char), 1024u), 1, n_read, out_file); if( n_read
!=  n_written){ Cyc_compile_failure= 1;({ void* _temp95[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\nError: problem copying cyc_include.h\n", sizeof(
unsigned char), 39u), _tag_arr( _temp95, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return tds;}} Cyc_Std_fclose( _temp93);
if( Cyc_pp_r){ Cyc_Absynpp_set_params(& params_r); Cyc_Absynpp_decllist2file(
tds, out_file);} else{ Cyc_Absyndump_set_params(& params_r); Cyc_Absyndump_dumpdecllist2file(
tds, out_file);} Cyc_Std_fflush(( struct Cyc_Std___sFILE*) out_file); return tds;}}}
static struct Cyc_List_List* Cyc_cfiles= 0; static void Cyc_remove_cfiles(){ if(
! Cyc_save_c_r){ for( 0; Cyc_cfiles !=  0; Cyc_cfiles=(( struct Cyc_List_List*)
_check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}} static struct Cyc_List_List*
Cyc_split_by_char( struct _tagged_arr s, unsigned char c){ if( s.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ return 0;}{ struct Cyc_List_List*
_temp97= 0; unsigned int _temp98= Cyc_Std_strlen( s); while( _temp98 >  0) {
struct _tagged_arr _temp99= Cyc_Std_strchr( s, c); if( _temp99.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ _temp97=({ struct Cyc_List_List*
_temp100=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp100->hd=( void*)({ struct _tagged_arr* _temp101=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp101[ 0]= s; _temp101;}); _temp100->tl=
_temp97; _temp100;}); break;} else{ _temp97=({ struct Cyc_List_List* _temp102=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp102->hd=(
void*)({ struct _tagged_arr* _temp103=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp103[ 0]=( struct _tagged_arr) Cyc_Std_substring( s, 0,(
unsigned int)(((( struct _tagged_arr) _temp99).curr -  s.curr) /  sizeof(
unsigned char))); _temp103;}); _temp102->tl= _temp97; _temp102;}); _temp98 -=(((
struct _tagged_arr) _temp99).curr -  s.curr) /  sizeof( unsigned char); s=
_tagged_arr_plus( _temp99, sizeof( unsigned char), 1);}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp97);}} static int Cyc_file_exists(
struct _tagged_arr file){ struct Cyc_Std___sFILE* f= 0;{ struct _handler_cons
_temp104; _push_handler(& _temp104);{ int _temp106= 0; if( setjmp( _temp104.handler)){
_temp106= 1;} if( ! _temp106){ f=( struct Cyc_Std___sFILE*) Cyc_Std_file_open(
file, _tag_arr("r", sizeof( unsigned char), 2u));; _pop_handler();} else{ void*
_temp105=( void*) _exn_thrown; void* _temp108= _temp105; _LL110: goto _LL111;
_LL112: goto _LL113; _LL111: goto _LL109; _LL113:( void) _throw( _temp108);
_LL109:;}}} if( f ==  0){ return 0;} else{ Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( f)); return 1;}} static struct _tagged_arr Cyc_sprint_list( struct
Cyc_List_List* dirs){ struct _tagged_arr tmp= _tag_arr("", sizeof( unsigned char),
1u); for( 0; dirs !=  0; dirs= dirs->tl){ struct _tagged_arr _temp114=*(( struct
_tagged_arr*) dirs->hd); if( _temp114.curr == (( struct _tagged_arr) _tag_arr( 0u,
0u, 0u)).curr? 1: Cyc_Std_strlen( _temp114) ==  0){ continue;} _temp114=( struct
_tagged_arr) Cyc_Std_strconcat( _temp114, _tag_arr(":", sizeof( unsigned char),
2u)); tmp=( struct _tagged_arr) Cyc_Std_strconcat( _temp114, tmp);} return tmp;}
static struct _tagged_arr* Cyc_find( struct Cyc_List_List* dirs, struct
_tagged_arr file){ if( file.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
return 0;} for( 0; dirs !=  0; dirs= dirs->tl){ struct _tagged_arr _temp115=*((
struct _tagged_arr*) dirs->hd); if( _temp115.curr == (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr? 1: Cyc_Std_strlen( _temp115) ==  0){ continue;}{
struct _tagged_arr s=( struct _tagged_arr) Cyc_Filename_concat( _temp115, file);
if( Cyc_file_exists( s)){ return({ struct _tagged_arr* _temp116=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp116[ 0]= s; _temp116;});}}}
return 0;} static struct _tagged_arr Cyc_do_find( struct Cyc_List_List* dirs,
struct _tagged_arr file){ struct _tagged_arr* _temp117= Cyc_find( dirs, file);
if( _temp117 ==  0){({ struct Cyc_Std_String_pa_struct _temp120; _temp120.tag=
Cyc_Std_String_pa; _temp120.f1=( struct _tagged_arr) Cyc_sprint_list( dirs);{
struct Cyc_Std_String_pa_struct _temp119; _temp119.tag= Cyc_Std_String_pa;
_temp119.f1=( struct _tagged_arr) file;{ void* _temp118[ 2u]={& _temp119,&
_temp120}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: can't find internal compiler file %s in path %s\n",
sizeof( unsigned char), 56u), _tag_arr( _temp118, sizeof( void*), 2u));}}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); exit( 1);} return*(( struct _tagged_arr*) _check_null(
_temp117));} static int Cyc_is_other_special( unsigned char c){ switch( c){ case
'\\': _LL121: goto _LL122; case '"': _LL122: goto _LL123; case ';': _LL123: goto
_LL124; case '&': _LL124: goto _LL125; case '(': _LL125: goto _LL126; case ')':
_LL126: goto _LL127; case '|': _LL127: goto _LL128; case '^': _LL128: goto
_LL129; case '<': _LL129: goto _LL130; case '>': _LL130: goto _LL131; case ' ':
_LL131: goto _LL132; case '\n': _LL132: goto _LL133; case '\t': _LL133: return 1;
default: _LL134: return 0;}} static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){ unsigned int _temp136= Cyc_Std_strlen( s); int _temp137=
0; int _temp138= 0;{ int i= 0; for( 0; i <  _temp136; i ++){ unsigned char
_temp139=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp139 == '\''){ _temp137 ++;} else{ if( Cyc_is_other_special(
_temp139)){ _temp138 ++;}}}} if( _temp137 ==  0? _temp138 ==  0: 0){ return s;}
if( _temp137 ==  0){ return( struct _tagged_arr) Cyc_Std_strconcat_l(({ struct
_tagged_arr* _temp140[ 3u]; _temp140[ 2u]= _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"'", sizeof( unsigned char), 2u);
_temp140[ 1u]=({ struct _tagged_arr* _temp141=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp141[ 0]=( struct _tagged_arr) s; _temp141;});
_temp140[ 0u]= _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"'", sizeof( unsigned char), 2u);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp140, sizeof( struct
_tagged_arr*), 3u));}));}{ unsigned int _temp142=( _temp136 +  _temp137) + 
_temp138; struct _tagged_arr _temp143=({ unsigned int _temp147= _temp142 +  1;
unsigned char* _temp148=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp147)); struct _tagged_arr _temp150= _tag_arr(
_temp148, sizeof( unsigned char), _temp142 +  1);{ unsigned int _temp149=
_temp147; unsigned int i; for( i= 0; i <  _temp149; i ++){ _temp148[ i]='\000';}};
_temp150;}); int _temp144= 0; int _temp145= 0; for( 0; _temp144 <  _temp136;
_temp144 ++){ unsigned char _temp146=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), _temp144)); if( _temp146 == '\''?
1: Cyc_is_other_special( _temp146)){*(( unsigned char*) _check_unknown_subscript(
_temp143, sizeof( unsigned char), _temp145 ++))='\\';}*(( unsigned char*)
_check_unknown_subscript( _temp143, sizeof( unsigned char), _temp145 ++))=
_temp146;} return( struct _tagged_arr) _temp143;}} static struct _tagged_arr*
Cyc_sh_escape_stringptr( struct _tagged_arr* sp){ return({ struct _tagged_arr*
_temp151=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp151[ 0]= Cyc_sh_escape_string(* sp); _temp151;});} static void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp152= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp153= Cyc_Std_strconcat(( struct _tagged_arr)
_temp152, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct _tagged_arr
_temp154= Cyc_Std_strconcat(( struct _tagged_arr) _temp152, _tag_arr(".cyci",
sizeof( unsigned char), 6u)); struct _tagged_arr _temp155= Cyc_Std_strconcat((
struct _tagged_arr) _temp152, _tag_arr(".cycio", sizeof( unsigned char), 7u));
struct _tagged_arr _temp156= Cyc_Std_strconcat(( struct _tagged_arr) _temp152,
_tag_arr(".c", sizeof( unsigned char), 3u)); if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct
_temp158; _temp158.tag= Cyc_Std_String_pa; _temp158.f1=( struct _tagged_arr)
filename;{ void* _temp157[ 1u]={& _temp158}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Compiling %s\n", sizeof( unsigned char), 14u), _tag_arr( _temp157,
sizeof( void*), 1u));}});}{ struct Cyc_Std___sFILE* f0= Cyc_try_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("input file",
sizeof( unsigned char), 11u)); if( Cyc_compile_failure){ return;} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( f0));{ struct _tagged_arr _temp159= Cyc_Std_str_sepstr(({
struct Cyc_List_List* _temp213=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp213->hd=( void*) _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp213->tl=(( struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct
_tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs));
_temp213;}), _tag_arr(" ", sizeof( unsigned char), 2u)); struct _tagged_arr
stdinc_string; struct _tagged_arr def_inc_path=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path); if( Cyc_Std_strlen( def_inc_path) >  0){
stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp161;
_temp161.tag= Cyc_Std_String_pa; _temp161.f1=( struct _tagged_arr) def_inc_path;{
void* _temp160[ 1u]={& _temp161}; Cyc_Std_aprintf( _tag_arr(" -I%s", sizeof(
unsigned char), 6u), _tag_arr( _temp160, sizeof( void*), 1u));}});} else{
stdinc_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct _tagged_arr
_temp162= Cyc_Std_getenv( _tag_arr("CYCLONE_INCLUDE_PATH", sizeof( unsigned char),
21u)); if( _temp162.curr != ( _tag_arr( 0u, 0u, 0u)).curr){ struct Cyc_List_List*
_temp163= Cyc_split_by_char(( struct _tagged_arr) _temp162,':'); struct
_tagged_arr _temp164= Cyc_Std_str_sepstr(({ struct Cyc_List_List* _temp168=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp168->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"", sizeof( unsigned char), 1u); _temp168->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, _temp163); _temp168;}), _tag_arr(" -I", sizeof(
unsigned char), 4u)); stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp167; _temp167.tag= Cyc_Std_String_pa; _temp167.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp166; _temp166.tag= Cyc_Std_String_pa;
_temp166.f1=( struct _tagged_arr) _temp164;{ void* _temp165[ 2u]={& _temp166,&
_temp167}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp165, sizeof( void*), 2u));}}});}{ struct _tagged_arr ofile_string;
if( Cyc_stop_after_cpp_r){ if( Cyc_output_file !=  0){ ofile_string=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp170; _temp170.tag= Cyc_Std_String_pa;
_temp170.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_output_file));{
void* _temp169[ 1u]={& _temp170}; Cyc_Std_aprintf( _tag_arr(" > %s", sizeof(
unsigned char), 6u), _tag_arr( _temp169, sizeof( void*), 1u));}});} else{
ofile_string= _tag_arr("", sizeof( unsigned char), 1u);}} else{ ofile_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp172; _temp172.tag=
Cyc_Std_String_pa; _temp172.f1=( struct _tagged_arr) Cyc_sh_escape_string((
struct _tagged_arr) _temp153);{ void* _temp171[ 1u]={& _temp172}; Cyc_Std_aprintf(
_tag_arr(" > %s", sizeof( unsigned char), 6u), _tag_arr( _temp171, sizeof( void*),
1u));}});}{ struct _tagged_arr fixup_string; if( Cyc_produce_dependencies){ if(
Cyc_dependencies_target ==  0){ fixup_string= _tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof( unsigned char), 35u);} else{ fixup_string=( struct _tagged_arr)({ struct
Cyc_Std_String_pa_struct _temp174; _temp174.tag= Cyc_Std_String_pa; _temp174.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_dependencies_target));{
void* _temp173[ 1u]={& _temp174}; Cyc_Std_aprintf( _tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",
sizeof( unsigned char), 29u), _tag_arr( _temp173, sizeof( void*), 1u));}});}}
else{ fixup_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct
_tagged_arr arch=( struct _tagged_arr) Cstring_to_string( Carch); struct
_tagged_arr _temp175=({ struct Cyc_Std_String_pa_struct _temp212; _temp212.tag=
Cyc_Std_String_pa; _temp212.f1=( struct _tagged_arr) ofile_string;{ struct Cyc_Std_String_pa_struct
_temp211; _temp211.tag= Cyc_Std_String_pa; _temp211.f1=( struct _tagged_arr)
fixup_string;{ struct Cyc_Std_String_pa_struct _temp210; _temp210.tag= Cyc_Std_String_pa;
_temp210.f1=( struct _tagged_arr) Cyc_sh_escape_string( filename);{ struct Cyc_Std_String_pa_struct
_temp209; _temp209.tag= Cyc_Std_String_pa; _temp209.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp208; _temp208.tag= Cyc_Std_String_pa;
_temp208.f1=( struct _tagged_arr) _temp159;{ struct Cyc_Std_String_pa_struct
_temp207; _temp207.tag= Cyc_Std_String_pa; _temp207.f1=( struct _tagged_arr)
arch;{ struct Cyc_Std_String_pa_struct _temp206; _temp206.tag= Cyc_Std_String_pa;
_temp206.f1=( struct _tagged_arr) Cyc_cpp;{ void* _temp205[ 7u]={& _temp206,&
_temp207,& _temp208,& _temp209,& _temp210,& _temp211,& _temp212}; Cyc_Std_aprintf(
_tag_arr("%s '-D_PLATFORM_INCLUDES_=\"arch/%s.h\"'%s%s %s%s%s", sizeof(
unsigned char), 50u), _tag_arr( _temp205, sizeof( void*), 7u));}}}}}}}}); if(
Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp177; _temp177.tag= Cyc_Std_String_pa;
_temp177.f1=( struct _tagged_arr) _temp175;{ void* _temp176[ 1u]={& _temp177};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp176, sizeof( void*), 1u));}});} if( Cyc_Std_system(( struct
_tagged_arr) _temp175) !=  0){ Cyc_compile_failure= 1;({ void* _temp178[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: preprocessing\n", sizeof(
unsigned char), 23u), _tag_arr( _temp178, sizeof( void*), 0u));}); return;} if(
Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position(( struct _tagged_arr)
_temp153);{ struct Cyc_Std___sFILE* in_file= Cyc_try_file_open(( struct
_tagged_arr) _temp153, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("file",
sizeof( unsigned char), 5u)); if( Cyc_compile_failure){ return;}{ struct Cyc_List_List*
tds= 0;{ struct _handler_cons _temp179; _push_handler(& _temp179);{ int _temp181=
0; if( setjmp( _temp179.handler)){ _temp181= 1;} if( ! _temp181){ tds=(( struct
Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(* f)(
struct Cyc_Std___sFILE*, struct Cyc_List_List*), struct Cyc_Std___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("parsing", sizeof(
unsigned char), 8u), Cyc_do_parse,( struct Cyc_Std___sFILE*) _check_null(
in_file), tds);; _pop_handler();} else{ void* _temp180=( void*) _exn_thrown;
void* _temp183= _temp180; _LL185: goto _LL186; _LL187: goto _LL188; _LL186: Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( in_file)); Cyc_remove_file(( struct
_tagged_arr) _temp153);( int) _throw( _temp183); _LL188:( void) _throw( _temp183);
_LL184:;}}} Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( in_file));
if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr) _temp153);
return;}{ struct Cyc_Tcenv_Tenv* _temp189= Cyc_Tcenv_tc_init(); if( Cyc_parseonly_r){
goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("type checking",
sizeof( unsigned char), 14u), Cyc_do_typecheck, _temp189, tds); if( Cyc_compile_failure){
Cyc_remove_file(( struct _tagged_arr) _temp153); return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("control-flow checking",
sizeof( unsigned char), 22u), Cyc_do_cfcheck, 1, tds); if( Cyc_compile_failure){
Cyc_remove_file(( struct _tagged_arr) _temp153); return;} if( Cyc_ic_r){ struct
Cyc_Std___sFILE* inter_file= Cyc_Std_fopen(( struct _tagged_arr) _temp154,
_tag_arr("r", sizeof( unsigned char), 2u)); struct Cyc_Std___sFILE*
inter_objfile= Cyc_try_file_open(( struct _tagged_arr) _temp155, _tag_arr("w",
sizeof( unsigned char), 2u), _tag_arr("interface object file", sizeof(
unsigned char), 22u)); if( inter_objfile ==  0){ Cyc_compile_failure= 1; return;}
Cyc_Position_reset_position(( struct _tagged_arr) _temp154); tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( struct _tuple9*,
struct Cyc_List_List*), struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("interface checking", sizeof( unsigned char), 19u), Cyc_do_interface,({
struct _tuple9* _temp190=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp190->f1= _temp189; _temp190->f2= inter_file; _temp190->f3=( struct Cyc_Std___sFILE*)
_check_null( inter_objfile); _temp190;}), tds); if( inter_file !=  0){ Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( inter_file));} Cyc_Std_file_close(( struct
Cyc_Std___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto PRINTC;}
tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("translation to C", sizeof( unsigned char), 17u), Cyc_do_translate, 1,
tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr) _temp153);
return;} if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();} Cyc_remove_file((
struct _tagged_arr) _temp153); if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){
tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("post-pass to VC", sizeof( unsigned char), 16u), Cyc_do_tovc, 1, tds);}
if( Cyc_compile_failure){ return;} PRINTC: { struct Cyc_Std___sFILE* out_file;
if( Cyc_parseonly_r? 1: Cyc_tc_r){ if( Cyc_output_file !=  0){ out_file= Cyc_try_file_open(*((
struct _tagged_arr*) _check_null( Cyc_output_file)), _tag_arr("w", sizeof(
unsigned char), 2u), _tag_arr("output file", sizeof( unsigned char), 12u));}
else{ out_file=( struct Cyc_Std___sFILE*) Cyc_Std_stdout;}} else{ if( Cyc_toc_r?
Cyc_output_file !=  0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));} else{ out_file= Cyc_try_file_open((
struct _tagged_arr) _temp156, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));}} if( Cyc_compile_failure){
return;}{ struct _handler_cons _temp191; _push_handler(& _temp191);{ int
_temp193= 0; if( setjmp( _temp191.handler)){ _temp193= 1;} if( ! _temp193){ tds=((
struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*), struct Cyc_Std___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("printing", sizeof(
unsigned char), 9u), Cyc_do_print,( struct Cyc_Std___sFILE*) _check_null(
out_file), tds);; _pop_handler();} else{ void* _temp192=( void*) _exn_thrown;
void* _temp195= _temp192; _LL197: goto _LL198; _LL199: goto _LL200; _LL198: Cyc_compile_failure=
1; Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({
struct Cyc_List_List* _temp201=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp201->hd=( void*)({ struct _tagged_arr* _temp202=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp202[ 0]=(
struct _tagged_arr) _temp156; _temp202;}); _temp201->tl= Cyc_cfiles; _temp201;});(
int) _throw( _temp195); _LL200:( void) _throw( _temp195); _LL196:;}}} Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List*
_temp203=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp203->hd=( void*)({ struct _tagged_arr* _temp204=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp204[ 0]=( struct _tagged_arr)
_temp156; _temp204;}); _temp203->tl= Cyc_cfiles; _temp203;});}}}}}}}}}}} static
unsigned char _temp214[ 8u]="<final>"; static struct _tagged_arr Cyc_final_str={
_temp214, _temp214, _temp214 +  8u}; static struct _tagged_arr* Cyc_final_strptr=&
Cyc_final_str; static struct Cyc_Interface_I* Cyc_read_cycio( struct _tagged_arr*
n){ if( n == ( struct _tagged_arr*) Cyc_final_strptr){ return Cyc_Interface_final();}{
struct _tagged_arr basename;{ struct _handler_cons _temp215; _push_handler(&
_temp215);{ int _temp217= 0; if( setjmp( _temp215.handler)){ _temp217= 1;} if( !
_temp217){ basename=( struct _tagged_arr) Cyc_Filename_chop_extension(* n);;
_pop_handler();} else{ void* _temp216=( void*) _exn_thrown; void* _temp219=
_temp216; _LL221: if(*(( void**) _temp219) ==  Cyc_Core_Invalid_argument){ goto
_LL222;} else{ goto _LL223;} _LL223: goto _LL224; _LL222: basename=* n; goto
_LL220; _LL224:( void) _throw( _temp219); _LL220:;}}}{ struct _tagged_arr
_temp225= Cyc_Std_strconcat( basename, _tag_arr(".cycio", sizeof( unsigned char),
7u)); struct Cyc_Std___sFILE* _temp226= Cyc_try_file_open(( struct _tagged_arr)
_temp225, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp226 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} Cyc_Position_reset_position(( struct _tagged_arr) _temp225);{ struct
Cyc_Interface_I* _temp227= Cyc_Interface_load(( struct Cyc_Std___sFILE*)
_check_null( _temp226)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp226)); return _temp227;}}}} static int Cyc_is_cfile( struct
_tagged_arr* n){ return*(( const unsigned char*) _check_unknown_subscript(* n,
sizeof( unsigned char), 0)) != '-';} struct _tuple10{ struct _tagged_arr f1; int
f2; struct _tagged_arr f3; void* f4; struct _tagged_arr f5; } ; int Cyc_main(
int argc, struct _tagged_arr argv){ struct _tagged_arr comp=( struct _tagged_arr)
Cstring_to_string( Ccomp); Cyc_set_cpp(( struct _tagged_arr) Cyc_Std_strconcat(
comp, _tag_arr(" -x c -E -U__GNUC__ -nostdinc", sizeof( unsigned char), 30u)));{
struct Cyc_List_List* options=({ struct _tuple10* _temp301[ 46u]; _temp301[ 45u]=({
struct _tuple10* _temp437=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp437->f1= _tag_arr("-printfullevars", sizeof( unsigned char), 16u); _temp437->f2=
0; _temp437->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp437->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp438=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp438[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp439; _temp439.tag= Cyc_Arg_Set_spec; _temp439.f1=& Cyc_print_full_evars_r;
_temp439;}); _temp438;}); _temp437->f5= _tag_arr("Print full information for evars (type debugging)",
sizeof( unsigned char), 50u); _temp437;}); _temp301[ 44u]=({ struct _tuple10*
_temp434=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp434->f1=
_tag_arr("-printallkinds", sizeof( unsigned char), 15u); _temp434->f2= 0;
_temp434->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp434->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp435=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp435[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp436; _temp436.tag= Cyc_Arg_Set_spec; _temp436.f1=& Cyc_print_all_kinds_r;
_temp436;}); _temp435;}); _temp434->f5= _tag_arr("Always print kinds of type variables",
sizeof( unsigned char), 37u); _temp434;}); _temp301[ 43u]=({ struct _tuple10*
_temp431=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp431->f1=
_tag_arr("-printalltvars", sizeof( unsigned char), 15u); _temp431->f2= 0;
_temp431->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp431->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp432=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp432[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp433; _temp433.tag= Cyc_Arg_Set_spec; _temp433.f1=& Cyc_print_all_tvars_r;
_temp433;}); _temp432;}); _temp431->f5= _tag_arr("Print all type variables (even implicit default effects)",
sizeof( unsigned char), 57u); _temp431;}); _temp301[ 42u]=({ struct _tuple10*
_temp428=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp428->f1=
_tag_arr("-noexpandtypedefs", sizeof( unsigned char), 18u); _temp428->f2= 0;
_temp428->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp428->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp429=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp429[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp430; _temp430.tag= Cyc_Arg_Set_spec; _temp430.f1=& Cyc_noexpand_r; _temp430;});
_temp429;}); _temp428->f5= _tag_arr("Don't expand typedefs in pretty printing",
sizeof( unsigned char), 41u); _temp428;}); _temp301[ 41u]=({ struct _tuple10*
_temp425=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp425->f1=
_tag_arr("-noremoveunused", sizeof( unsigned char), 16u); _temp425->f2= 0;
_temp425->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp425->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp426=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp426[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp427; _temp427.tag= Cyc_Arg_Set_spec; _temp427.f1=& Cyc_noshake_r; _temp427;});
_temp426;}); _temp425->f5= _tag_arr("Don't remove externed variables that aren't used",
sizeof( unsigned char), 49u); _temp425;}); _temp301[ 40u]=({ struct _tuple10*
_temp422=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp422->f1=
_tag_arr("-nogc", sizeof( unsigned char), 6u); _temp422->f2= 0; _temp422->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp422->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp423=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp423[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp424; _temp424.tag= Cyc_Arg_Set_spec;
_temp424.f1=& Cyc_nogc_r; _temp424;}); _temp423;}); _temp422->f5= _tag_arr("Don't link in the garbage collector",
sizeof( unsigned char), 36u); _temp422;}); _temp301[ 39u]=({ struct _tuple10*
_temp419=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp419->f1=
_tag_arr("-nocyc", sizeof( unsigned char), 7u); _temp419->f2= 0; _temp419->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp419->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp420=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp420[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp421; _temp421.tag= Cyc_Arg_Unit_spec;
_temp421.f1= Cyc_set_nocyc; _temp421;}); _temp420;}); _temp419->f5= _tag_arr("Don't add implicit namespace Cyc",
sizeof( unsigned char), 33u); _temp419;}); _temp301[ 38u]=({ struct _tuple10*
_temp416=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp416->f1=
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u); _temp416->f2= 0;
_temp416->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp416->f4=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp417=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp417[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp418; _temp418.tag= Cyc_Arg_Unit_spec; _temp418.f1= Cyc_set_nocppprecomp;
_temp418;}); _temp417;}); _temp416->f5= _tag_arr("Don't do smart preprocessing (mac only)",
sizeof( unsigned char), 40u); _temp416;}); _temp301[ 37u]=({ struct _tuple10*
_temp413=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp413->f1=
_tag_arr("-use-cpp", sizeof( unsigned char), 9u); _temp413->f2= 0; _temp413->f3=
_tag_arr("<path>", sizeof( unsigned char), 7u); _temp413->f4=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp414=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp414[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp415; _temp415.tag= Cyc_Arg_String_spec; _temp415.f1=
Cyc_set_cpp; _temp415;}); _temp414;}); _temp413->f5= _tag_arr("Indicate which preprocessor to use",
sizeof( unsigned char), 35u); _temp413;}); _temp301[ 36u]=({ struct _tuple10*
_temp410=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp410->f1=
_tag_arr("--noboundschecks", sizeof( unsigned char), 17u); _temp410->f2= 0;
_temp410->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp410->f4=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp411=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp411[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp412; _temp412.tag= Cyc_Arg_Unit_spec; _temp412.f1= Cyc_set_noboundschecks;
_temp412;}); _temp411;}); _temp410->f5= _tag_arr("Disable bounds checks",
sizeof( unsigned char), 22u); _temp410;}); _temp301[ 35u]=({ struct _tuple10*
_temp407=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp407->f1=
_tag_arr("--nonullchecks", sizeof( unsigned char), 15u); _temp407->f2= 0;
_temp407->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp407->f4=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp408=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp408[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp409; _temp409.tag= Cyc_Arg_Unit_spec; _temp409.f1= Cyc_set_nonullchecks;
_temp409;}); _temp408;}); _temp407->f5= _tag_arr("Disable null checks", sizeof(
unsigned char), 20u); _temp407;}); _temp301[ 34u]=({ struct _tuple10* _temp404=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp404->f1= _tag_arr("--nochecks",
sizeof( unsigned char), 11u); _temp404->f2= 0; _temp404->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp404->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp405=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp405[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp406; _temp406.tag= Cyc_Arg_Unit_spec;
_temp406.f1= Cyc_set_nochecks; _temp406;}); _temp405;}); _temp404->f5= _tag_arr("Disable bounds/null checks",
sizeof( unsigned char), 27u); _temp404;}); _temp301[ 33u]=({ struct _tuple10*
_temp401=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp401->f1=
_tag_arr("--lineno", sizeof( unsigned char), 9u); _temp401->f2= 0; _temp401->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp401->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp402=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp402[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp403; _temp403.tag= Cyc_Arg_Unit_spec;
_temp403.f1= Cyc_set_lineno; _temp403;}); _temp402;}); _temp401->f5= _tag_arr("Generate line numbers for debugging",
sizeof( unsigned char), 36u); _temp401;}); _temp301[ 32u]=({ struct _tuple10*
_temp398=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp398->f1=
_tag_arr("-save-c", sizeof( unsigned char), 8u); _temp398->f2= 0; _temp398->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp398->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp399=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp399[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp400; _temp400.tag= Cyc_Arg_Set_spec;
_temp400.f1=& Cyc_save_c_r; _temp400;}); _temp399;}); _temp398->f5= _tag_arr("Don't delete temporary C files",
sizeof( unsigned char), 31u); _temp398;}); _temp301[ 31u]=({ struct _tuple10*
_temp395=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp395->f1=
_tag_arr("-save-temps", sizeof( unsigned char), 12u); _temp395->f2= 0; _temp395->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp395->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp396=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp396[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp397; _temp397.tag= Cyc_Arg_Unit_spec;
_temp397.f1= Cyc_set_save_temps; _temp397;}); _temp396;}); _temp395->f5=
_tag_arr("Don't delete temporary files", sizeof( unsigned char), 29u); _temp395;});
_temp301[ 30u]=({ struct _tuple10* _temp392=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp392->f1= _tag_arr("-tovc", sizeof( unsigned char),
6u); _temp392->f2= 0; _temp392->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp392->f4=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp393=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp393[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp394; _temp394.tag= Cyc_Arg_Set_spec; _temp394.f1=& Cyc_tovc_r; _temp394;});
_temp393;}); _temp392->f5= _tag_arr("Avoid gcc extensions in C output", sizeof(
unsigned char), 33u); _temp392;}); _temp301[ 29u]=({ struct _tuple10* _temp389=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp389->f1= _tag_arr("-up",
sizeof( unsigned char), 4u); _temp389->f2= 0; _temp389->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp389->f4=( void*)({ struct Cyc_Arg_Clear_spec_struct*
_temp390=( struct Cyc_Arg_Clear_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Clear_spec_struct));
_temp390[ 0]=({ struct Cyc_Arg_Clear_spec_struct _temp391; _temp391.tag= Cyc_Arg_Clear_spec;
_temp391.f1=& Cyc_pp_r; _temp391;}); _temp390;}); _temp389->f5= _tag_arr("Ugly print",
sizeof( unsigned char), 11u); _temp389;}); _temp301[ 28u]=({ struct _tuple10*
_temp386=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp386->f1=
_tag_arr("-pp", sizeof( unsigned char), 4u); _temp386->f2= 0; _temp386->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp386->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp387=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp387[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp388; _temp388.tag= Cyc_Arg_Set_spec;
_temp388.f1=& Cyc_pp_r; _temp388;}); _temp387;}); _temp386->f5= _tag_arr("Pretty print",
sizeof( unsigned char), 13u); _temp386;}); _temp301[ 27u]=({ struct _tuple10*
_temp383=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp383->f1=
_tag_arr("-ic", sizeof( unsigned char), 4u); _temp383->f2= 0; _temp383->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp383->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp384=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp384[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp385; _temp385.tag= Cyc_Arg_Set_spec;
_temp385.f1=& Cyc_ic_r; _temp385;}); _temp384;}); _temp383->f5= _tag_arr("Activate the link-checker",
sizeof( unsigned char), 26u); _temp383;}); _temp301[ 26u]=({ struct _tuple10*
_temp380=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp380->f1=
_tag_arr("-stopafter-toc", sizeof( unsigned char), 15u); _temp380->f2= 0;
_temp380->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp380->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp381=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp381[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp382; _temp382.tag= Cyc_Arg_Set_spec; _temp382.f1=& Cyc_toc_r; _temp382;});
_temp381;}); _temp380->f5= _tag_arr("Stop after translation to C", sizeof(
unsigned char), 28u); _temp380;}); _temp301[ 25u]=({ struct _tuple10* _temp377=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp377->f1= _tag_arr("-stopafter-tc",
sizeof( unsigned char), 14u); _temp377->f2= 0; _temp377->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp377->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp378=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp378[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp379; _temp379.tag= Cyc_Arg_Set_spec;
_temp379.f1=& Cyc_tc_r; _temp379;}); _temp378;}); _temp377->f5= _tag_arr("Stop after type checking",
sizeof( unsigned char), 25u); _temp377;}); _temp301[ 24u]=({ struct _tuple10*
_temp374=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp374->f1=
_tag_arr("-stopafter-parse", sizeof( unsigned char), 17u); _temp374->f2= 0;
_temp374->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp374->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp375=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp375[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp376; _temp376.tag= Cyc_Arg_Set_spec; _temp376.f1=& Cyc_parseonly_r;
_temp376;}); _temp375;}); _temp374->f5= _tag_arr("Stop after parsing", sizeof(
unsigned char), 19u); _temp374;}); _temp301[ 23u]=({ struct _tuple10* _temp371=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp371->f1= _tag_arr("-E",
sizeof( unsigned char), 3u); _temp371->f2= 0; _temp371->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp371->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp372=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp372[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp373; _temp373.tag= Cyc_Arg_Set_spec;
_temp373.f1=& Cyc_stop_after_cpp_r; _temp373;}); _temp372;}); _temp371->f5=
_tag_arr("Stop after preprocessing", sizeof( unsigned char), 25u); _temp371;});
_temp301[ 22u]=({ struct _tuple10* _temp368=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp368->f1= _tag_arr("-MT", sizeof( unsigned char),
4u); _temp368->f2= 0; _temp368->f3= _tag_arr(" <target>", sizeof( unsigned char),
10u); _temp368->f4=( void*)({ struct Cyc_Arg_String_spec_struct* _temp369=(
struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp369[ 0]=({ struct Cyc_Arg_String_spec_struct _temp370; _temp370.tag= Cyc_Arg_String_spec;
_temp370.f1= Cyc_set_dependencies_target; _temp370;}); _temp369;}); _temp368->f5=
_tag_arr("Give target for dependencies", sizeof( unsigned char), 29u); _temp368;});
_temp301[ 21u]=({ struct _tuple10* _temp365=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp365->f1= _tag_arr("-MG", sizeof( unsigned char),
4u); _temp365->f2= 0; _temp365->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp365->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct* _temp366=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp366[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp367; _temp367.tag= Cyc_Arg_Flag_spec; _temp367.f1= Cyc_add_cpparg; _temp367;});
_temp366;}); _temp365->f5= _tag_arr("When producing dependencies assume that missing files are generated",
sizeof( unsigned char), 68u); _temp365;}); _temp301[ 20u]=({ struct _tuple10*
_temp362=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp362->f1=
_tag_arr("-M", sizeof( unsigned char), 3u); _temp362->f2= 0; _temp362->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp362->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp363=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp363[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp364; _temp364.tag= Cyc_Arg_Unit_spec;
_temp364.f1= Cyc_set_produce_dependencies; _temp364;}); _temp363;}); _temp362->f5=
_tag_arr("Produce dependencies", sizeof( unsigned char), 21u); _temp362;});
_temp301[ 19u]=({ struct _tuple10* _temp359=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp359->f1= _tag_arr("-S", sizeof( unsigned char),
3u); _temp359->f2= 0; _temp359->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp359->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct* _temp360=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp360[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp361; _temp361.tag= Cyc_Arg_Unit_spec; _temp361.f1= Cyc_set_stop_after_asmfile;
_temp361;}); _temp360;}); _temp359->f5= _tag_arr("Stop after producing assembly code",
sizeof( unsigned char), 35u); _temp359;}); _temp301[ 18u]=({ struct _tuple10*
_temp356=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp356->f1=
_tag_arr("-pa", sizeof( unsigned char), 4u); _temp356->f2= 0; _temp356->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp356->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp357=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp357[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp358; _temp358.tag= Cyc_Arg_Unit_spec;
_temp358.f1= Cyc_set_pa; _temp358;}); _temp357;}); _temp356->f5= _tag_arr("Compile for profiling with aprof",
sizeof( unsigned char), 33u); _temp356;}); _temp301[ 17u]=({ struct _tuple10*
_temp353=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp353->f1=
_tag_arr("-pg", sizeof( unsigned char), 4u); _temp353->f2= 0; _temp353->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp353->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp354=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp354[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp355; _temp355.tag= Cyc_Arg_Flag_spec;
_temp355.f1= Cyc_add_ccarg; _temp355;}); _temp354;}); _temp353->f5= _tag_arr("Compile for profiling with gprof",
sizeof( unsigned char), 33u); _temp353;}); _temp301[ 16u]=({ struct _tuple10*
_temp350=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp350->f1=
_tag_arr("-p", sizeof( unsigned char), 3u); _temp350->f2= 0; _temp350->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp350->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp351=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp351[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp352; _temp352.tag= Cyc_Arg_Flag_spec;
_temp352.f1= Cyc_add_ccarg; _temp352;}); _temp351;}); _temp350->f5= _tag_arr("Compile for profiling with prof",
sizeof( unsigned char), 32u); _temp350;}); _temp301[ 15u]=({ struct _tuple10*
_temp347=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp347->f1=
_tag_arr("-g", sizeof( unsigned char), 3u); _temp347->f2= 0; _temp347->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp347->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp348=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp348[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp349; _temp349.tag= Cyc_Arg_Flag_spec;
_temp349.f1= Cyc_add_ccarg; _temp349;}); _temp348;}); _temp347->f5= _tag_arr("Compile for debugging",
sizeof( unsigned char), 22u); _temp347;}); _temp301[ 14u]=({ struct _tuple10*
_temp344=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp344->f1=
_tag_arr("-O3", sizeof( unsigned char), 4u); _temp344->f2= 0; _temp344->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp344->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp345=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp345[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp346; _temp346.tag= Cyc_Arg_Flag_spec;
_temp346.f1= Cyc_add_ccarg; _temp346;}); _temp345;}); _temp344->f5= _tag_arr("Even more optimization",
sizeof( unsigned char), 23u); _temp344;}); _temp301[ 13u]=({ struct _tuple10*
_temp341=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp341->f1=
_tag_arr("-O2", sizeof( unsigned char), 4u); _temp341->f2= 0; _temp341->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp341->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp342=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp342[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp343; _temp343.tag= Cyc_Arg_Flag_spec;
_temp343.f1= Cyc_add_ccarg; _temp343;}); _temp342;}); _temp341->f5= _tag_arr("A higher level of optimization",
sizeof( unsigned char), 31u); _temp341;}); _temp301[ 12u]=({ struct _tuple10*
_temp338=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp338->f1=
_tag_arr("-O", sizeof( unsigned char), 3u); _temp338->f2= 0; _temp338->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp338->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp339=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp339[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp340; _temp340.tag= Cyc_Arg_Flag_spec;
_temp340.f1= Cyc_add_ccarg; _temp340;}); _temp339;}); _temp338->f5= _tag_arr("Optimize",
sizeof( unsigned char), 9u); _temp338;}); _temp301[ 11u]=({ struct _tuple10*
_temp335=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp335->f1=
_tag_arr("-O0", sizeof( unsigned char), 4u); _temp335->f2= 0; _temp335->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp335->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp336=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp336[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp337; _temp337.tag= Cyc_Arg_Flag_spec;
_temp337.f1= Cyc_add_ccarg; _temp337;}); _temp336;}); _temp335->f5= _tag_arr("Don't optimize",
sizeof( unsigned char), 15u); _temp335;}); _temp301[ 10u]=({ struct _tuple10*
_temp332=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp332->f1=
_tag_arr("-s", sizeof( unsigned char), 3u); _temp332->f2= 0; _temp332->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp332->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp333=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp333[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp334; _temp334.tag= Cyc_Arg_Flag_spec;
_temp334.f1= Cyc_add_ccarg; _temp334;}); _temp333;}); _temp332->f5= _tag_arr("Remove all symbol table and relocation info from executable",
sizeof( unsigned char), 60u); _temp332;}); _temp301[ 9u]=({ struct _tuple10*
_temp329=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp329->f1=
_tag_arr("-x", sizeof( unsigned char), 3u); _temp329->f2= 0; _temp329->f3=
_tag_arr(" <language>", sizeof( unsigned char), 12u); _temp329->f4=( void*)({
struct Cyc_Arg_String_spec_struct* _temp330=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp330[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp331; _temp331.tag= Cyc_Arg_String_spec; _temp331.f1=
Cyc_set_inputtype; _temp331;}); _temp330;}); _temp329->f5= _tag_arr("Specify <language> for the following input files",
sizeof( unsigned char), 49u); _temp329;}); _temp301[ 8u]=({ struct _tuple10*
_temp326=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp326->f1=
_tag_arr("-c", sizeof( unsigned char), 3u); _temp326->f2= 0; _temp326->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp326->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp327=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp327[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp328; _temp328.tag= Cyc_Arg_Unit_spec;
_temp328.f1= Cyc_set_stop_after_objectfile; _temp328;}); _temp327;}); _temp326->f5=
_tag_arr("Produce an object file instead of an executable; do not link", sizeof(
unsigned char), 61u); _temp326;}); _temp301[ 7u]=({ struct _tuple10* _temp323=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp323->f1= _tag_arr("-l",
sizeof( unsigned char), 3u); _temp323->f2= 1; _temp323->f3= _tag_arr("<file>",
sizeof( unsigned char), 7u); _temp323->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp324=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp324[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp325; _temp325.tag= Cyc_Arg_Flag_spec;
_temp325.f1= Cyc_add_libarg; _temp325;}); _temp324;}); _temp323->f5= _tag_arr("Library file",
sizeof( unsigned char), 13u); _temp323;}); _temp301[ 6u]=({ struct _tuple10*
_temp320=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp320->f1=
_tag_arr("-L", sizeof( unsigned char), 3u); _temp320->f2= 1; _temp320->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp320->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp321=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp321[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp322; _temp322.tag= Cyc_Arg_Flag_spec; _temp322.f1= Cyc_add_cpparg; _temp322;});
_temp321;}); _temp320->f5= _tag_arr("Add to the list of directories for -l",
sizeof( unsigned char), 38u); _temp320;}); _temp301[ 5u]=({ struct _tuple10*
_temp317=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp317->f1=
_tag_arr("-I", sizeof( unsigned char), 3u); _temp317->f2= 1; _temp317->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp317->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp318=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp318[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp319; _temp319.tag= Cyc_Arg_Flag_spec; _temp319.f1= Cyc_add_cpparg; _temp319;});
_temp318;}); _temp317->f5= _tag_arr("Add to the list of directories to search for include files",
sizeof( unsigned char), 59u); _temp317;}); _temp301[ 4u]=({ struct _tuple10*
_temp314=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp314->f1=
_tag_arr("-B", sizeof( unsigned char), 3u); _temp314->f2= 1; _temp314->f3=
_tag_arr("<file>", sizeof( unsigned char), 7u); _temp314->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp315=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp315[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp316; _temp316.tag= Cyc_Arg_Flag_spec; _temp316.f1= Cyc_add_cyclone_lib_path;
_temp316;}); _temp315;}); _temp314->f5= _tag_arr("Add to the list of directories to search for compiler files",
sizeof( unsigned char), 60u); _temp314;}); _temp301[ 3u]=({ struct _tuple10*
_temp311=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp311->f1=
_tag_arr("-D", sizeof( unsigned char), 3u); _temp311->f2= 1; _temp311->f3=
_tag_arr("<name>[=<value>]", sizeof( unsigned char), 17u); _temp311->f4=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp312=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp312[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp313; _temp313.tag= Cyc_Arg_Flag_spec; _temp313.f1= Cyc_add_cpparg; _temp313;});
_temp312;}); _temp311->f5= _tag_arr("Pass definition to preprocessor", sizeof(
unsigned char), 32u); _temp311;}); _temp301[ 2u]=({ struct _tuple10* _temp308=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp308->f1= _tag_arr("-o",
sizeof( unsigned char), 3u); _temp308->f2= 0; _temp308->f3= _tag_arr(" <file>",
sizeof( unsigned char), 8u); _temp308->f4=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp309=( struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp309[ 0]=({ struct Cyc_Arg_String_spec_struct _temp310; _temp310.tag= Cyc_Arg_String_spec;
_temp310.f1= Cyc_set_output_file; _temp310;}); _temp309;}); _temp308->f5=
_tag_arr("Set the output file name to <file>", sizeof( unsigned char), 35u);
_temp308;}); _temp301[ 1u]=({ struct _tuple10* _temp305=( struct _tuple10*)
_cycalloc( sizeof( struct _tuple10)); _temp305->f1= _tag_arr("--version",
sizeof( unsigned char), 10u); _temp305->f2= 0; _temp305->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp305->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp306=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp306[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp307; _temp307.tag= Cyc_Arg_Unit_spec;
_temp307.f1= Cyc_print_version; _temp307;}); _temp306;}); _temp305->f5= _tag_arr("Print version number and exit",
sizeof( unsigned char), 30u); _temp305;}); _temp301[ 0u]=({ struct _tuple10*
_temp302=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp302->f1=
_tag_arr("-v", sizeof( unsigned char), 3u); _temp302->f2= 0; _temp302->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp302->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp303=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp303[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp304; _temp304.tag= Cyc_Arg_Set_spec;
_temp304.f1=& Cyc_v_r; _temp304;}); _temp303;}); _temp302->f5= _tag_arr("Print compilation stages verbosely",
sizeof( unsigned char), 35u); _temp302;});(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp301, sizeof( struct _tuple10*), 46u));});
Cyc_Arg_parse( options, Cyc_add_other, _tag_arr("Options:", sizeof(
unsigned char), 9u), argv);{ struct _tagged_arr cyclone_exec_prefix=( struct
_tagged_arr) Cyc_Std_getenv( _tag_arr("CYCLONE_EXEC_PREFIX", sizeof(
unsigned char), 20u)); if( cyclone_exec_prefix.curr != (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr){ Cyc_cyclone_lib_path=({ struct Cyc_List_List*
_temp228=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp228->hd=( void*)({ struct _tagged_arr* _temp229=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp229[ 0]= cyclone_exec_prefix;
_temp229;}); _temp228->tl= Cyc_cyclone_lib_path; _temp228;});}{ struct
_tagged_arr def_lib_path=( struct _tagged_arr) Cstring_to_string( Cdef_lib_path);
if( Cyc_Std_strlen( def_lib_path) >  0){ Cyc_cyclone_lib_path=({ struct Cyc_List_List*
_temp230=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp230->hd=( void*)({ struct _tagged_arr* _temp231=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp231[ 0]= def_lib_path; _temp231;});
_temp230->tl= Cyc_cyclone_lib_path; _temp230;});} if( Cyc_cyclone_lib_path !=  0){
Cyc_add_ccarg(( struct _tagged_arr) Cyc_Std_strconcat( _tag_arr("-L", sizeof(
unsigned char), 3u),*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( Cyc_cyclone_lib_path))->hd)));} Cyc_cyclone_lib_path=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_lib_path); Cyc_cyc_include=
Cyc_do_find( Cyc_cyclone_lib_path, _tag_arr("include/cyc_include.h", sizeof(
unsigned char), 22u));{ struct Cyc_List_List* _temp232=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); for( 0; _temp232 != 
0; _temp232= _temp232->tl){ Cyc_process_file(*(( struct _tagged_arr*) _temp232->hd));
if( Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)?
1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs ==  0){ return 0;} Cyc_ccargs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{
struct _tagged_arr _temp233= Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_ccargs), _tag_arr(" ", sizeof( unsigned char), 2u));
Cyc_libargs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
Cyc_libargs);{ struct _tagged_arr _temp234= Cyc_Std_str_sepstr(({ struct Cyc_List_List*
_temp300=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp300->hd=( void*) _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)),"", sizeof( unsigned char), 1u); _temp300->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_libargs); _temp300;}), _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_List_List* stdlib; struct _tagged_arr
stdlib_string; int _temp235=(( Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".a", sizeof( unsigned char), 3u)): 0))?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".lib", sizeof( unsigned char), 5u)): 0);
if( _temp235){ stdlib= 0; stdlib_string= _tag_arr("", sizeof( unsigned char), 1u);}
else{ struct _tagged_arr libcyc_filename; struct _tagged_arr nogc_filename;
struct _tagged_arr gc_filename; if( Cyc_pa_r){ libcyc_filename= _tag_arr("libcyc_a.a",
sizeof( unsigned char), 11u); nogc_filename= _tag_arr("nogc_a.a", sizeof(
unsigned char), 9u);} else{ if( Cyc_nocheck_r){ libcyc_filename= _tag_arr("libcyc_nocheck.a",
sizeof( unsigned char), 17u); nogc_filename= _tag_arr("nogc_nocheck.a", sizeof(
unsigned char), 15u);} else{ libcyc_filename= _tag_arr("libcyc.a", sizeof(
unsigned char), 9u); nogc_filename= _tag_arr("nogc.a", sizeof( unsigned char), 7u);}}
gc_filename= _tag_arr("gc.a", sizeof( unsigned char), 5u);{ struct _tagged_arr
_temp236= Cyc_do_find( Cyc_cyclone_lib_path, libcyc_filename); struct
_tagged_arr _temp237= Cyc_nogc_r? Cyc_do_find( Cyc_cyclone_lib_path,
nogc_filename): Cyc_do_find( Cyc_cyclone_lib_path, gc_filename); stdlib=({
struct _tagged_arr* _temp238[ 1u]; _temp238[ 0u]=({ struct _tagged_arr* _temp239=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp239[ 0]=
_temp236; _temp239;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp238, sizeof( struct _tagged_arr*), 1u));}); stdlib_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp242; _temp242.tag=
Cyc_Std_String_pa; _temp242.f1=( struct _tagged_arr) _temp237;{ struct Cyc_Std_String_pa_struct
_temp241; _temp241.tag= Cyc_Std_String_pa; _temp241.f1=( struct _tagged_arr)
_temp236;{ void* _temp240[ 2u]={& _temp241,& _temp242}; Cyc_Std_aprintf(
_tag_arr(" %s %s", sizeof( unsigned char), 7u), _tag_arr( _temp240, sizeof( void*),
2u));}}});}} if( Cyc_ic_r){ struct _handler_cons _temp243; _push_handler(&
_temp243);{ int _temp245= 0; if( setjmp( _temp243.handler)){ _temp245= 1;} if( !
_temp245){ Cyc_ccargs=(( struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*),
struct Cyc_List_List* x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=((
struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List*
_temp246=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( stdlib,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_ccargs, Cyc_libargs)); if( !
_temp235){ _temp246=({ struct Cyc_List_List* _temp247=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp247->hd=( void*) Cyc_final_strptr;
_temp247->tl= _temp246; _temp247;});}{ struct Cyc_Interface_I* _temp248=((
struct Cyc_Interface_I*(*)( struct Cyc_Interface_I*(* get)( struct _tagged_arr*),
struct Cyc_List_List* la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio, _temp246, _temp246); if( _temp248 ==  0){({ void* _temp249[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: interfaces incompatible\n",
sizeof( unsigned char), 32u), _tag_arr( _temp249, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp250= 1; _npop_handler( 0u); return _temp250;}}
if( _temp235){ if( Cyc_output_file !=  0){ struct _tagged_arr _temp251=({ struct
Cyc_Std_String_pa_struct _temp255; _temp255.tag= Cyc_Std_String_pa; _temp255.f1=(
struct _tagged_arr) Cyc_Filename_chop_extension(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp254[ 1u]={& _temp255}; Cyc_Std_aprintf(
_tag_arr("%s.cycio", sizeof( unsigned char), 9u), _tag_arr( _temp254, sizeof(
void*), 1u));}}); struct Cyc_Std___sFILE* _temp252= Cyc_try_file_open(( struct
_tagged_arr) _temp251, _tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp252 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{
int _temp253= 1; _npop_handler( 0u); return _temp253;}} Cyc_Interface_save((
struct Cyc_Interface_I*) _check_null( _temp248),( struct Cyc_Std___sFILE*)
_check_null( _temp252)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp252));}} else{ if( ! Cyc_Interface_is_subinterface( Cyc_Interface_empty(),(
struct Cyc_Interface_I*) _check_null( _temp248),({ struct _tuple8* _temp256=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp256->f1= _tag_arr("empty interface",
sizeof( unsigned char), 16u); _temp256->f2= _tag_arr("global interface", sizeof(
unsigned char), 17u); _temp256;}))){({ void* _temp257[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: some objects are still undefined\n", sizeof(
unsigned char), 41u), _tag_arr( _temp257, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp258= 1; _npop_handler( 0u); return _temp258;}}}}};
_pop_handler();} else{ void* _temp244=( void*) _exn_thrown; void* _temp260=
_temp244; struct _tagged_arr _temp274; struct _tagged_arr _temp276; struct
_tagged_arr _temp278; _LL262: if(*(( void**) _temp260) ==  Cyc_Core_Failure){
_LL275: _temp274=(( struct Cyc_Core_Failure_struct*) _temp260)->f1; goto _LL263;}
else{ goto _LL264;} _LL264: if(*(( void**) _temp260) ==  Cyc_Core_Impossible){
_LL277: _temp276=(( struct Cyc_Core_Impossible_struct*) _temp260)->f1; goto
_LL265;} else{ goto _LL266;} _LL266: if( _temp260 ==  Cyc_Dict_Absent){ goto
_LL267;} else{ goto _LL268;} _LL268: if(*(( void**) _temp260) ==  Cyc_Core_Invalid_argument){
_LL279: _temp278=(( struct Cyc_Core_Invalid_argument_struct*) _temp260)->f1;
goto _LL269;} else{ goto _LL270;} _LL270: goto _LL271; _LL272: goto _LL273;
_LL263:({ struct Cyc_Std_String_pa_struct _temp281; _temp281.tag= Cyc_Std_String_pa;
_temp281.f1=( struct _tagged_arr) _temp274;{ void* _temp280[ 1u]={& _temp281};
Cyc_Std_printf( _tag_arr("Exception Core::Failure %s\n", sizeof( unsigned char),
28u), _tag_arr( _temp280, sizeof( void*), 1u));}}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL265:({ struct Cyc_Std_String_pa_struct _temp283; _temp283.tag= Cyc_Std_String_pa;
_temp283.f1=( struct _tagged_arr) _temp276;{ void* _temp282[ 1u]={& _temp283};
Cyc_Std_printf( _tag_arr("Exception Core::Impossible %s\n", sizeof(
unsigned char), 31u), _tag_arr( _temp282, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL267:({ void* _temp284[ 0u]={}; Cyc_Std_printf(
_tag_arr("Exception Dict::Absent\n", sizeof( unsigned char), 24u), _tag_arr(
_temp284, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL269:({ struct Cyc_Std_String_pa_struct _temp286; _temp286.tag= Cyc_Std_String_pa;
_temp286.f1=( struct _tagged_arr) _temp278;{ void* _temp285[ 1u]={& _temp286};
Cyc_Std_printf( _tag_arr("Exception Core::Invalid_argument %s\n", sizeof(
unsigned char), 37u), _tag_arr( _temp285, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL271:({ void* _temp287[ 0u]={}; Cyc_Std_printf(
_tag_arr("Uncaught exception\n", sizeof( unsigned char), 20u), _tag_arr(
_temp287, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL273:( void) _throw( _temp260); _LL261:;}}}{ struct _tagged_arr
_temp288=({ struct Cyc_Std_String_pa_struct _temp297; _temp297.tag= Cyc_Std_String_pa;
_temp297.f1=( struct _tagged_arr) _temp234;{ struct Cyc_Std_String_pa_struct
_temp296; _temp296.tag= Cyc_Std_String_pa; _temp296.f1=( struct _tagged_arr)
stdlib_string;{ struct Cyc_Std_String_pa_struct _temp295; _temp295.tag= Cyc_Std_String_pa;
_temp295.f1=( struct _tagged_arr) _temp233;{ struct Cyc_Std_String_pa_struct
_temp294; _temp294.tag= Cyc_Std_String_pa; _temp294.f1=( struct _tagged_arr)(
Cyc_output_file ==  0? _tag_arr("", sizeof( unsigned char), 1u):( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp299; _temp299.tag= Cyc_Std_String_pa;
_temp299.f1=( struct _tagged_arr) Cyc_sh_escape_string(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp298[ 1u]={& _temp299}; Cyc_Std_aprintf(
_tag_arr(" -o %s", sizeof( unsigned char), 7u), _tag_arr( _temp298, sizeof( void*),
1u));}}));{ struct Cyc_Std_String_pa_struct _temp293; _temp293.tag= Cyc_Std_String_pa;
_temp293.f1=( struct _tagged_arr) comp;{ void* _temp292[ 5u]={& _temp293,&
_temp294,& _temp295,& _temp296,& _temp297}; Cyc_Std_aprintf( _tag_arr("%s %s %s%s%s",
sizeof( unsigned char), 13u), _tag_arr( _temp292, sizeof( void*), 5u));}}}}}});
if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp290; _temp290.tag= Cyc_Std_String_pa;
_temp290.f1=( struct _tagged_arr) _temp288;{ void* _temp289[ 1u]={& _temp290};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp289, sizeof( void*), 1u));}});} if( Cyc_Std_system(( struct
_tagged_arr) _temp288) !=  0){({ void* _temp291[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: C compiler failed\n", sizeof( unsigned char), 26u), _tag_arr(
_temp291, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1;} Cyc_remove_cfiles(); return Cyc_compile_failure? 1: 0;}}}}}}}
