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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_filter(
int(* f)( void*), struct Cyc_List_List* x); extern unsigned char Cyc_Arg_Bad[ 8u];
struct Cyc_Arg_Bad_struct{ unsigned char* tag; struct _tagged_arr f1; } ; extern
unsigned char Cyc_Arg_Error[ 10u]; static const int Cyc_Arg_Unit_spec= 0; struct
Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)(); } ; static const int Cyc_Arg_Flag_spec=
1; struct Cyc_Arg_Flag_spec_struct{ int tag; void(* f1)( struct _tagged_arr); }
; static const int Cyc_Arg_FlagString_spec= 2; struct Cyc_Arg_FlagString_spec_struct{
int tag; void(* f1)( struct _tagged_arr, struct _tagged_arr); } ; static const
int Cyc_Arg_Set_spec= 3; struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ;
static const int Cyc_Arg_Clear_spec= 4; struct Cyc_Arg_Clear_spec_struct{ int
tag; int* f1; } ; static const int Cyc_Arg_String_spec= 5; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_Int_spec=
6; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct Cyc_List_List* specs,
void(* anonfun)( struct _tagged_arr), struct _tagged_arr errmsg, struct
_tagged_arr args); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stdout;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_remove( struct
_tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*); extern int
Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern struct Cyc_Std___sFILE* Cyc_Std_fopen(
struct _tagged_arr __filename, struct _tagged_arr __modes); extern unsigned int
Cyc_Std_fread( struct _tagged_arr __ptr, unsigned int __size, unsigned int __n,
struct Cyc_Std___sFILE* __stream); extern unsigned int Cyc_Std_fwrite( struct
_tagged_arr __ptr, unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE*
__s); extern int Cyc_Std_feof( struct Cyc_Std___sFILE* __stream); extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern struct Cyc_Std___sFILE* Cyc_Std_file_open( struct
_tagged_arr fname, struct _tagged_arr mode); extern void Cyc_Std_file_close(
struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Std_printf( struct _tagged_arr fmt, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt,
struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
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
static unsigned char _temp0[ 6u]="0.1.2"; static struct _tagged_arr Cyc_version={
_temp0, _temp0, _temp0 +  6u}; extern void Cyc_Lex_lex_init(); static int Cyc_pp_r=
0; static int Cyc_noexpand_r= 0; static int Cyc_noshake_r= 0; static int Cyc_stop_after_cpp_r=
0; static int Cyc_parseonly_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0;
static int Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r= 0; static int
Cyc_stop_after_asmfile_r= 0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0;
static int Cyc_save_temps_r= 0; static int Cyc_save_c_r= 0; static int Cyc_nogc_r=
0; static int Cyc_pa_r= 0; static int Cyc_add_cyc_namespace_r= 1; static int Cyc_generate_line_directives_r=
0; static int Cyc_print_full_evars_r= 0; static int Cyc_print_all_tvars_r= 0;
static int Cyc_print_all_kinds_r= 0; static struct _tagged_arr* Cyc_output_file=
0; static void Cyc_set_output_file( struct _tagged_arr s){ Cyc_output_file=({
struct _tagged_arr* _temp1=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1[ 0]= s; _temp1;});} extern unsigned char* Cdef_inc_path;
extern unsigned char* Cdef_lib_path; extern unsigned char* Carch; extern
unsigned char* Ccomp; extern unsigned char* Ccflags; static unsigned char _temp2[
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
_temp10; _temp10.tag= Cyc_Std_String_pa; _temp10.f1=( struct _tagged_arr)
Cstring_to_string( Ccflags);{ struct Cyc_Std_String_pa_struct _temp9; _temp9.tag=
Cyc_Std_String_pa; _temp9.f1=( struct _tagged_arr) Cstring_to_string( Ccomp);{
struct Cyc_Std_String_pa_struct _temp8; _temp8.tag= Cyc_Std_String_pa; _temp8.f1=(
struct _tagged_arr) Cstring_to_string( Carch);{ void* _temp7[ 3u]={& _temp8,&
_temp9,& _temp10}; Cyc_Std_printf( _tag_arr("Compiled for architecture: %s CC=\"%s %s\"\n",
sizeof( unsigned char), 42u), _tag_arr( _temp7, sizeof( void*), 3u));}}}});({
struct Cyc_Std_String_pa_struct _temp12; _temp12.tag= Cyc_Std_String_pa; _temp12.f1=(
struct _tagged_arr) Cstring_to_string( Cdef_lib_path);{ void* _temp11[ 1u]={&
_temp12}; Cyc_Std_printf( _tag_arr("Standard library directory: %s\n", sizeof(
unsigned char), 32u), _tag_arr( _temp11, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path);{ void* _temp13[ 1u]={& _temp14}; Cyc_Std_printf(
_tag_arr("Standard include directory: %s\n", sizeof( unsigned char), 32u),
_tag_arr( _temp13, sizeof( void*), 1u));}}); exit( 0);} static int Cyc_is_cyclone_sourcefile(
struct _tagged_arr s){ unsigned int _temp15= Cyc_Std_strlen( s); if( _temp15 <= 
4){ return 0;} else{ return Cyc_Std_strcmp( _tagged_arr_plus( s, sizeof(
unsigned char),( int)( _temp15 -  4)), _tag_arr(".cyc", sizeof( unsigned char),
5u)) ==  0;}} static struct Cyc_List_List* Cyc_ccargs= 0; static void Cyc_add_ccarg(
struct _tagged_arr s){ Cyc_ccargs=({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp16->hd=( void*)({ struct
_tagged_arr* _temp17=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp17[ 0]= s; _temp17;}); _temp16->tl= Cyc_ccargs; _temp16;});}
static void Cyc_add_ccarg2( struct _tagged_arr flag, struct _tagged_arr arg){
Cyc_ccargs=({ struct Cyc_List_List* _temp18=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp18->hd=( void*)({ struct _tagged_arr*
_temp21=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp21[
0]= arg; _temp21;}); _temp18->tl=({ struct Cyc_List_List* _temp19=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp19->hd=( void*)({ struct
_tagged_arr* _temp20=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp20[ 0]= flag; _temp20;}); _temp19->tl= Cyc_ccargs; _temp19;});
_temp18;});} static struct Cyc_List_List* Cyc_cyclone_lib_path= 0; static void
Cyc_add_cyclone_lib_path( struct _tagged_arr s){ unsigned int _temp22= Cyc_Std_strlen(
s); if( _temp22 <=  2){ return;}{ struct _tagged_arr _temp23=( struct
_tagged_arr) Cyc_Std_substring( s, 2, _temp22 -  2); Cyc_cyclone_lib_path=({
struct Cyc_List_List* _temp24=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp24->hd=( void*)({ struct _tagged_arr* _temp25=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp25[ 0]= _temp23;
_temp25;}); _temp24->tl= Cyc_cyclone_lib_path; _temp24;});}} static struct Cyc_List_List*
Cyc_libargs= 0; static void Cyc_add_libarg( struct _tagged_arr s){ if( Cyc_Std_strcmp(
s, _tag_arr("-lxml", sizeof( unsigned char), 6u)) ==  0){ Cyc_add_ccarg( s);}
else{ Cyc_libargs=({ struct Cyc_List_List* _temp26=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp26->hd=( void*)({ struct
_tagged_arr* _temp27=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp27[ 0]= s; _temp27;}); _temp26->tl= Cyc_libargs; _temp26;});}}
static void Cyc_set_save_temps(){ Cyc_save_temps_r= 1; Cyc_add_ccarg( _tag_arr("-save-temps",
sizeof( unsigned char), 12u));} static int Cyc_produce_dependencies= 0; static
void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r= 1; Cyc_produce_dependencies=
1; Cyc_add_cpparg( _tag_arr("-M", sizeof( unsigned char), 3u));} static struct
_tagged_arr* Cyc_dependencies_target= 0; static void Cyc_set_dependencies_target(
struct _tagged_arr s){ Cyc_dependencies_target=({ struct _tagged_arr* _temp28=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp28[ 0]= s;
_temp28;});} static void Cyc_set_stop_after_objectfile(){ Cyc_stop_after_objectfile_r=
1; Cyc_add_ccarg( _tag_arr("-c", sizeof( unsigned char), 3u));} static void Cyc_set_nocppprecomp(){
Cyc_add_cpparg( _tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u)); Cyc_add_ccarg(
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u));} static void Cyc_set_lineno(){
Cyc_generate_line_directives_r= 1; Cyc_set_save_temps();} static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r= 0; Cyc_add_ccarg( _tag_arr("-DNO_CYC_PREFIX", sizeof(
unsigned char), 16u));} static void Cyc_set_pa(){ Cyc_pa_r= 1; Cyc_add_ccarg(
_tag_arr("-DCYC_REGION_PROFILE", sizeof( unsigned char), 21u));} static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r= 1; Cyc_add_ccarg( _tag_arr("-S", sizeof( unsigned char),
3u));} static const int Cyc_DEFAULTINPUT= 0; static const int Cyc_CYCLONEFILE= 1;
static void* Cyc_intype=( void*) 0u; static void Cyc_set_inputtype( struct
_tagged_arr s){ if( Cyc_Std_strcmp( s, _tag_arr("cyc", sizeof( unsigned char), 4u))
==  0){ Cyc_intype=( void*) Cyc_CYCLONEFILE;} else{ if( Cyc_Std_strcmp( s,
_tag_arr("none", sizeof( unsigned char), 5u)) ==  0){ Cyc_intype=( void*) Cyc_DEFAULTINPUT;}
else{({ struct Cyc_Std_String_pa_struct _temp30; _temp30.tag= Cyc_Std_String_pa;
_temp30.f1=( struct _tagged_arr) s;{ void* _temp29[ 1u]={& _temp30}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Input type '%s' not supported\n", sizeof(
unsigned char), 31u), _tag_arr( _temp29, sizeof( void*), 1u));}});}}} static
struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other( struct
_tagged_arr s){ if( Cyc_intype == ( void*) Cyc_CYCLONEFILE? 1: Cyc_is_cyclone_sourcefile(
s)){ Cyc_cyclone_files=({ struct Cyc_List_List* _temp31=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp31->hd=( void*)({ struct
_tagged_arr* _temp32=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp32[ 0]= s; _temp32;}); _temp31->tl= Cyc_cyclone_files;
_temp31;});{ struct _tagged_arr _temp33= Cyc_Filename_chop_extension( s); struct
_tagged_arr _temp34= Cyc_Std_strconcat(( struct _tagged_arr) _temp33, _tag_arr(".c",
sizeof( unsigned char), 3u)); Cyc_add_ccarg(( struct _tagged_arr) _temp34);}}
else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct _tagged_arr s){
if( Cyc_save_temps_r){ return;} else{ Cyc_Std_remove( s);}} int Cyc_compile_failure=
0; struct Cyc_Std___sFILE* Cyc_try_file_open( struct _tagged_arr filename,
struct _tagged_arr mode, struct _tagged_arr msg_part){ struct _handler_cons
_temp35; _push_handler(& _temp35);{ int _temp37= 0; if( setjmp( _temp35.handler)){
_temp37= 1;} if( ! _temp37){{ struct Cyc_Std___sFILE* _temp38=( struct Cyc_Std___sFILE*)
Cyc_Std_file_open( filename, mode); _npop_handler( 0u); return _temp38;};
_pop_handler();} else{ void* _temp36=( void*) _exn_thrown; void* _temp40=
_temp36; _LL42: goto _LL43; _LL44: goto _LL45; _LL43: Cyc_compile_failure= 1;({
struct Cyc_Std_String_pa_struct _temp48; _temp48.tag= Cyc_Std_String_pa; _temp48.f1=(
struct _tagged_arr) filename;{ struct Cyc_Std_String_pa_struct _temp47; _temp47.tag=
Cyc_Std_String_pa; _temp47.f1=( struct _tagged_arr) msg_part;{ void* _temp46[ 2u]={&
_temp47,& _temp48}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: couldn't open %s %s\n",
sizeof( unsigned char), 29u), _tag_arr( _temp46, sizeof( void*), 2u));}}}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return 0; _LL45:( void) _throw(
_temp40); _LL41:;}}} struct Cyc_List_List* Cyc_do_stage( struct _tagged_arr
stage_name, struct Cyc_List_List*(* f)( void*, struct Cyc_List_List*), void* env,
struct Cyc_List_List* tds){ struct _tagged_arr exn_string= _tag_arr("", sizeof(
unsigned char), 1u); struct _tagged_arr explain_string= _tag_arr("", sizeof(
unsigned char), 1u); int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct*
_temp81=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp81[ 0]=({ struct Cyc_Core_Impossible_struct _temp82; _temp82.tag= Cyc_Core_Impossible;
_temp82.f1= _tag_arr("", sizeof( unsigned char), 1u); _temp82;}); _temp81;});
struct Cyc_List_List* _temp49= 0;{ struct _handler_cons _temp50; _push_handler(&
_temp50);{ int _temp52= 0; if( setjmp( _temp50.handler)){ _temp52= 1;} if( !
_temp52){ _temp49= f( env, tds);; _pop_handler();} else{ void* _temp51=( void*)
_exn_thrown; void* _temp54= _temp51; struct _tagged_arr _temp68; struct
_tagged_arr _temp70; struct _tagged_arr _temp72; _LL56: if(*(( void**) _temp54)
==  Cyc_Core_Impossible){ _LL69: _temp68=(( struct Cyc_Core_Impossible_struct*)
_temp54)->f1; goto _LL57;} else{ goto _LL58;} _LL58: if( _temp54 ==  Cyc_Dict_Absent){
goto _LL59;} else{ goto _LL60;} _LL60: if(*(( void**) _temp54) ==  Cyc_Core_Invalid_argument){
_LL71: _temp70=(( struct Cyc_Core_Invalid_argument_struct*) _temp54)->f1; goto
_LL61;} else{ goto _LL62;} _LL62: if(*(( void**) _temp54) ==  Cyc_Core_Failure){
_LL73: _temp72=(( struct Cyc_Core_Failure_struct*) _temp54)->f1; goto _LL63;}
else{ goto _LL64;} _LL64: goto _LL65; _LL66: goto _LL67; _LL57: exn_string=
_tag_arr("Exception Core::Impossible", sizeof( unsigned char), 27u);
explain_string= _temp68; goto _LL55; _LL59: exn_string= _tag_arr("Exception Dict::Absent",
sizeof( unsigned char), 23u); goto _LL55; _LL61: exn_string= _tag_arr("Exception Core::Invalid_argument",
sizeof( unsigned char), 33u); explain_string= _temp70; goto _LL55; _LL63:
exn_string= _tag_arr("Exception Core::Failure", sizeof( unsigned char), 24u);
explain_string= _temp72; goto _LL55; _LL65: ex= _temp54; other_exn= 1;
exn_string= _tag_arr("Uncaught exception", sizeof( unsigned char), 19u); goto
_LL55; _LL67:( void) _throw( _temp54); _LL55:;}}} if( Cyc_Position_error_p()){
Cyc_compile_failure= 1;} if( Cyc_Std_strcmp( exn_string, _tag_arr("", sizeof(
unsigned char), 1u)) !=  0){ Cyc_compile_failure= 1;({ struct Cyc_Std_String_pa_struct
_temp77; _temp77.tag= Cyc_Std_String_pa; _temp77.f1=( struct _tagged_arr)
explain_string;{ struct Cyc_Std_String_pa_struct _temp76; _temp76.tag= Cyc_Std_String_pa;
_temp76.f1=( struct _tagged_arr) stage_name;{ struct Cyc_Std_String_pa_struct
_temp75; _temp75.tag= Cyc_Std_String_pa; _temp75.f1=( struct _tagged_arr)
exn_string;{ void* _temp74[ 3u]={& _temp75,& _temp76,& _temp77}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n%s thrown during %s: %s", sizeof( unsigned char), 25u),
_tag_arr( _temp74, sizeof( void*), 3u));}}}});} if( Cyc_compile_failure){({ void*
_temp78[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nCOMPILATION FAILED!\n",
sizeof( unsigned char), 22u), _tag_arr( _temp78, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); if( other_exn){( int) _throw( ex);}
return _temp49;} else{ if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp80;
_temp80.tag= Cyc_Std_String_pa; _temp80.f1=( struct _tagged_arr) stage_name;{
void* _temp79[ 1u]={& _temp80}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s completed.\n",
sizeof( unsigned char), 15u), _tag_arr( _temp79, sizeof( void*), 1u));}}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return _temp49;}} return _temp49;}
struct Cyc_List_List* Cyc_do_parse( struct Cyc_Std___sFILE* f, struct Cyc_List_List*
ignore){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp83= Cyc_Parse_parse_file(
f); Cyc_Lex_lex_init(); return _temp83;}} struct Cyc_List_List* Cyc_do_typecheck(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds);
if( ! Cyc_noshake_r){ tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List*
Cyc_do_cfcheck( int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check(
tds); return tds;} struct _tuple9{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Std___sFILE*
f2; struct Cyc_Std___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple9* params, struct Cyc_List_List* tds){ struct _tuple9 _temp86;
struct Cyc_Std___sFILE* _temp87; struct Cyc_Std___sFILE* _temp89; struct Cyc_Tcenv_Tenv*
_temp91; struct _tuple9* _temp84= params; _temp86=* _temp84; _LL92: _temp91=
_temp86.f1; goto _LL90; _LL90: _temp89= _temp86.f2; goto _LL88; _LL88: _temp87=
_temp86.f3; goto _LL85; _LL85: { struct Cyc_Interface_I* _temp93= Cyc_Interface_extract(
_temp91->ae); if( _temp89 ==  0){ Cyc_Interface_save( _temp93, _temp87);} else{
struct Cyc_Interface_I* _temp94= Cyc_Interface_parse(( struct Cyc_Std___sFILE*)
_check_null( _temp89)); if( ! Cyc_Interface_is_subinterface( _temp94, _temp93,({
struct _tuple8* _temp95=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp95->f1= _tag_arr("written interface", sizeof( unsigned char), 18u); _temp95->f2=
_tag_arr("maximal interface", sizeof( unsigned char), 18u); _temp95;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp94, _temp87);}} return tds;}} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_tovc( int ignore, struct Cyc_List_List* tds){
return Cyc_Tovc_tovc( tds);} static unsigned char _temp96[ 1u]=""; static struct
_tagged_arr Cyc_cyc_include={ _temp96, _temp96, _temp96 +  1u}; struct Cyc_List_List*
Cyc_do_print( struct Cyc_Std___sFILE* out_file, struct Cyc_List_List* tds){
struct Cyc_Absynpp_Params params_r; if( Cyc_tc_r){ params_r= Cyc_Absynpp_cyc_params_r;}
else{ params_r= Cyc_Absynpp_c_params_r;} params_r.expand_typedefs= ! Cyc_noexpand_r;
params_r.to_VC= Cyc_tovc_r; params_r.add_cyc_prefix= Cyc_add_cyc_namespace_r;
params_r.generate_line_directives= Cyc_generate_line_directives_r; params_r.print_full_evars=
Cyc_print_full_evars_r; params_r.print_all_tvars= Cyc_print_all_tvars_r;
params_r.print_all_kinds= Cyc_print_all_kinds_r;{ struct Cyc_Std___sFILE*
_temp97= Cyc_try_file_open( Cyc_cyc_include, _tag_arr("r", sizeof( unsigned char),
2u), _tag_arr("internal compiler file", sizeof( unsigned char), 23u)); if(
_temp97 ==  0){ return tds;}{ struct Cyc_Std___sFILE* _temp98=( struct Cyc_Std___sFILE*)
_check_null( _temp97); unsigned int n_read= 1024; unsigned int n_written;
unsigned char buf[ 1024u];{ unsigned int _temp101= 1024u; unsigned int i; for( i=
0; i <  _temp101; i ++){ buf[ i]='\000';}} while( n_read ==  1024) { n_read= Cyc_Std_fread(
_tag_arr( buf, sizeof( unsigned char), 1024u), 1, 1024, _temp98); if( n_read != 
1024? ! Cyc_Std_feof( _temp98): 0){ Cyc_compile_failure= 1;({ void* _temp99[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: problem copying cyc_include.h\n",
sizeof( unsigned char), 39u), _tag_arr( _temp99, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return tds;} n_written= Cyc_Std_fwrite(
_tag_arr( buf, sizeof( unsigned char), 1024u), 1, n_read, out_file); if( n_read
!=  n_written){ Cyc_compile_failure= 1;({ void* _temp100[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\nError: problem copying cyc_include.h\n", sizeof(
unsigned char), 39u), _tag_arr( _temp100, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return tds;}} Cyc_Std_fclose( _temp98);
if( Cyc_pp_r){ Cyc_Absynpp_set_params(& params_r); Cyc_Absynpp_decllist2file(
tds, out_file);} else{ Cyc_Absyndump_set_params(& params_r); Cyc_Absyndump_dumpdecllist2file(
tds, out_file);} Cyc_Std_fflush(( struct Cyc_Std___sFILE*) out_file); return tds;}}}
static struct Cyc_List_List* Cyc_cfiles= 0; static void Cyc_remove_cfiles(){ if(
! Cyc_save_c_r){ for( 0; Cyc_cfiles !=  0; Cyc_cfiles=(( struct Cyc_List_List*)
_check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}} static struct Cyc_List_List*
Cyc_split_by_char( struct _tagged_arr s, unsigned char c){ if( s.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ return 0;}{ struct Cyc_List_List*
_temp102= 0; unsigned int _temp103= Cyc_Std_strlen( s); while( _temp103 >  0) {
struct _tagged_arr _temp104= Cyc_Std_strchr( s, c); if( _temp104.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ _temp102=({ struct Cyc_List_List*
_temp105=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp105->hd=( void*)({ struct _tagged_arr* _temp106=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp106[ 0]= s; _temp106;}); _temp105->tl=
_temp102; _temp105;}); break;} else{ _temp102=({ struct Cyc_List_List* _temp107=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp107->hd=(
void*)({ struct _tagged_arr* _temp108=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp108[ 0]=( struct _tagged_arr) Cyc_Std_substring( s, 0,(
unsigned int)(((( struct _tagged_arr) _temp104).curr -  s.curr) /  sizeof(
unsigned char))); _temp108;}); _temp107->tl= _temp102; _temp107;}); _temp103 -=(((
struct _tagged_arr) _temp104).curr -  s.curr) /  sizeof( unsigned char); s=
_tagged_arr_plus( _temp104, sizeof( unsigned char), 1);}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp102);}} static int Cyc_file_exists(
struct _tagged_arr file){ struct Cyc_Std___sFILE* f= 0;{ struct _handler_cons
_temp109; _push_handler(& _temp109);{ int _temp111= 0; if( setjmp( _temp109.handler)){
_temp111= 1;} if( ! _temp111){ f=( struct Cyc_Std___sFILE*) Cyc_Std_file_open(
file, _tag_arr("r", sizeof( unsigned char), 2u));; _pop_handler();} else{ void*
_temp110=( void*) _exn_thrown; void* _temp113= _temp110; _LL115: goto _LL116;
_LL117: goto _LL118; _LL116: goto _LL114; _LL118:( void) _throw( _temp113);
_LL114:;}}} if( f ==  0){ return 0;} else{ Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( f)); return 1;}} static struct _tagged_arr Cyc_sprint_list( struct
Cyc_List_List* dirs){ struct _tagged_arr tmp= _tag_arr("", sizeof( unsigned char),
1u); for( 0; dirs !=  0; dirs= dirs->tl){ struct _tagged_arr _temp119=*(( struct
_tagged_arr*) dirs->hd); if( _temp119.curr == (( struct _tagged_arr) _tag_arr( 0u,
0u, 0u)).curr? 1: Cyc_Std_strlen( _temp119) ==  0){ continue;} _temp119=( struct
_tagged_arr) Cyc_Std_strconcat( _temp119, _tag_arr(":", sizeof( unsigned char),
2u)); tmp=( struct _tagged_arr) Cyc_Std_strconcat( _temp119, tmp);} return tmp;}
static struct _tagged_arr* Cyc_find( struct Cyc_List_List* dirs, struct
_tagged_arr file){ if( file.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
return 0;} for( 0; dirs !=  0; dirs= dirs->tl){ struct _tagged_arr _temp120=*((
struct _tagged_arr*) dirs->hd); if( _temp120.curr == (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr? 1: Cyc_Std_strlen( _temp120) ==  0){ continue;}{
struct _tagged_arr s=( struct _tagged_arr) Cyc_Filename_concat( _temp120, file);
if( Cyc_file_exists( s)){ return({ struct _tagged_arr* _temp121=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp121[ 0]= s; _temp121;});}}}
return 0;} static struct _tagged_arr Cyc_do_find( struct Cyc_List_List* dirs,
struct _tagged_arr file){ struct _tagged_arr* _temp122= Cyc_find( dirs, file);
if( _temp122 ==  0){({ struct Cyc_Std_String_pa_struct _temp125; _temp125.tag=
Cyc_Std_String_pa; _temp125.f1=( struct _tagged_arr) Cyc_sprint_list( dirs);{
struct Cyc_Std_String_pa_struct _temp124; _temp124.tag= Cyc_Std_String_pa;
_temp124.f1=( struct _tagged_arr) file;{ void* _temp123[ 2u]={& _temp124,&
_temp125}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: can't find internal compiler file %s in path %s\n",
sizeof( unsigned char), 56u), _tag_arr( _temp123, sizeof( void*), 2u));}}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); exit( 1);} return*(( struct _tagged_arr*) _check_null(
_temp122));} static int Cyc_is_other_special( unsigned char c){ switch( c){ case
'\\': _LL126: goto _LL127; case '"': _LL127: goto _LL128; case ';': _LL128: goto
_LL129; case '&': _LL129: goto _LL130; case '(': _LL130: goto _LL131; case ')':
_LL131: goto _LL132; case '|': _LL132: goto _LL133; case '^': _LL133: goto
_LL134; case '<': _LL134: goto _LL135; case '>': _LL135: goto _LL136; case ' ':
_LL136: goto _LL137; case '\n': _LL137: goto _LL138; case '\t': _LL138: return 1;
default: _LL139: return 0;}} static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){ unsigned int _temp141= Cyc_Std_strlen( s); int _temp142=
0; int _temp143= 0;{ int i= 0; for( 0; i <  _temp141; i ++){ unsigned char
_temp144=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp144 == '\''){ _temp142 ++;} else{ if( Cyc_is_other_special(
_temp144)){ _temp143 ++;}}}} if( _temp142 ==  0? _temp143 ==  0: 0){ return s;}
if( _temp142 ==  0){ return( struct _tagged_arr) Cyc_Std_strconcat_l(({ struct
_tagged_arr* _temp145[ 3u]; _temp145[ 2u]= _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"'", sizeof( unsigned char), 2u);
_temp145[ 1u]=({ struct _tagged_arr* _temp146=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp146[ 0]=( struct _tagged_arr) s; _temp146;});
_temp145[ 0u]= _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"'", sizeof( unsigned char), 2u);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp145, sizeof( struct
_tagged_arr*), 3u));}));}{ unsigned int _temp147=( _temp141 +  _temp142) + 
_temp143; struct _tagged_arr _temp148=({ unsigned int _temp152= _temp147 +  1;
unsigned char* _temp153=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp152)); struct _tagged_arr _temp155= _tag_arr(
_temp153, sizeof( unsigned char), _temp147 +  1);{ unsigned int _temp154=
_temp152; unsigned int i; for( i= 0; i <  _temp154; i ++){ _temp153[ i]='\000';}};
_temp155;}); int _temp149= 0; int _temp150= 0; for( 0; _temp149 <  _temp141;
_temp149 ++){ unsigned char _temp151=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), _temp149)); if( _temp151 == '\''?
1: Cyc_is_other_special( _temp151)){*(( unsigned char*) _check_unknown_subscript(
_temp148, sizeof( unsigned char), _temp150 ++))='\\';}*(( unsigned char*)
_check_unknown_subscript( _temp148, sizeof( unsigned char), _temp150 ++))=
_temp151;} return( struct _tagged_arr) _temp148;}} static struct _tagged_arr*
Cyc_sh_escape_stringptr( struct _tagged_arr* sp){ return({ struct _tagged_arr*
_temp156=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp156[ 0]= Cyc_sh_escape_string(* sp); _temp156;});} static void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp157= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp158= Cyc_Std_strconcat(( struct _tagged_arr)
_temp157, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct _tagged_arr
_temp159= Cyc_Std_strconcat(( struct _tagged_arr) _temp157, _tag_arr(".cyci",
sizeof( unsigned char), 6u)); struct _tagged_arr _temp160= Cyc_Std_strconcat((
struct _tagged_arr) _temp157, _tag_arr(".cycio", sizeof( unsigned char), 7u));
struct _tagged_arr _temp161= Cyc_Std_strconcat(( struct _tagged_arr) _temp157,
_tag_arr(".c", sizeof( unsigned char), 3u)); if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct
_temp163; _temp163.tag= Cyc_Std_String_pa; _temp163.f1=( struct _tagged_arr)
filename;{ void* _temp162[ 1u]={& _temp163}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Compiling %s\n", sizeof( unsigned char), 14u), _tag_arr( _temp162,
sizeof( void*), 1u));}});}{ struct Cyc_Std___sFILE* f0= Cyc_try_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("input file",
sizeof( unsigned char), 11u)); if( Cyc_compile_failure){ return;} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( f0));{ struct _tagged_arr _temp164= Cyc_Std_str_sepstr(({
struct Cyc_List_List* _temp218=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp218->hd=( void*) _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp218->tl=(( struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct
_tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs));
_temp218;}), _tag_arr(" ", sizeof( unsigned char), 2u)); struct _tagged_arr
stdinc_string; struct _tagged_arr def_inc_path=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path); if( Cyc_Std_strlen( def_inc_path) >  0){
stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp166;
_temp166.tag= Cyc_Std_String_pa; _temp166.f1=( struct _tagged_arr) def_inc_path;{
void* _temp165[ 1u]={& _temp166}; Cyc_Std_aprintf( _tag_arr(" -I%s", sizeof(
unsigned char), 6u), _tag_arr( _temp165, sizeof( void*), 1u));}});} else{
stdinc_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct _tagged_arr
_temp167= Cyc_Std_getenv( _tag_arr("CYCLONE_INCLUDE_PATH", sizeof( unsigned char),
21u)); if( _temp167.curr != ( _tag_arr( 0u, 0u, 0u)).curr){ struct Cyc_List_List*
_temp168= Cyc_split_by_char(( struct _tagged_arr) _temp167,':'); struct
_tagged_arr _temp169= Cyc_Std_str_sepstr(({ struct Cyc_List_List* _temp173=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp173->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"", sizeof( unsigned char), 1u); _temp173->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, _temp168); _temp173;}), _tag_arr(" -I", sizeof(
unsigned char), 4u)); stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp172; _temp172.tag= Cyc_Std_String_pa; _temp172.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp171; _temp171.tag= Cyc_Std_String_pa;
_temp171.f1=( struct _tagged_arr) _temp169;{ void* _temp170[ 2u]={& _temp171,&
_temp172}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp170, sizeof( void*), 2u));}}});}{ struct _tagged_arr ofile_string;
if( Cyc_stop_after_cpp_r){ if( Cyc_output_file !=  0){ ofile_string=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp175; _temp175.tag= Cyc_Std_String_pa;
_temp175.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_output_file));{
void* _temp174[ 1u]={& _temp175}; Cyc_Std_aprintf( _tag_arr(" > %s", sizeof(
unsigned char), 6u), _tag_arr( _temp174, sizeof( void*), 1u));}});} else{
ofile_string= _tag_arr("", sizeof( unsigned char), 1u);}} else{ ofile_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp177; _temp177.tag=
Cyc_Std_String_pa; _temp177.f1=( struct _tagged_arr) Cyc_sh_escape_string((
struct _tagged_arr) _temp158);{ void* _temp176[ 1u]={& _temp177}; Cyc_Std_aprintf(
_tag_arr(" > %s", sizeof( unsigned char), 6u), _tag_arr( _temp176, sizeof( void*),
1u));}});}{ struct _tagged_arr fixup_string; if( Cyc_produce_dependencies){ if(
Cyc_dependencies_target ==  0){ fixup_string= _tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof( unsigned char), 35u);} else{ fixup_string=( struct _tagged_arr)({ struct
Cyc_Std_String_pa_struct _temp179; _temp179.tag= Cyc_Std_String_pa; _temp179.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_dependencies_target));{
void* _temp178[ 1u]={& _temp179}; Cyc_Std_aprintf( _tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",
sizeof( unsigned char), 29u), _tag_arr( _temp178, sizeof( void*), 1u));}});}}
else{ fixup_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct
_tagged_arr arch=( struct _tagged_arr) Cstring_to_string( Carch); struct
_tagged_arr _temp180=({ struct Cyc_Std_String_pa_struct _temp217; _temp217.tag=
Cyc_Std_String_pa; _temp217.f1=( struct _tagged_arr) ofile_string;{ struct Cyc_Std_String_pa_struct
_temp216; _temp216.tag= Cyc_Std_String_pa; _temp216.f1=( struct _tagged_arr)
fixup_string;{ struct Cyc_Std_String_pa_struct _temp215; _temp215.tag= Cyc_Std_String_pa;
_temp215.f1=( struct _tagged_arr) Cyc_sh_escape_string( filename);{ struct Cyc_Std_String_pa_struct
_temp214; _temp214.tag= Cyc_Std_String_pa; _temp214.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp213; _temp213.tag= Cyc_Std_String_pa;
_temp213.f1=( struct _tagged_arr) _temp164;{ struct Cyc_Std_String_pa_struct
_temp212; _temp212.tag= Cyc_Std_String_pa; _temp212.f1=( struct _tagged_arr)
arch;{ struct Cyc_Std_String_pa_struct _temp211; _temp211.tag= Cyc_Std_String_pa;
_temp211.f1=( struct _tagged_arr) Cyc_cpp;{ void* _temp210[ 7u]={& _temp211,&
_temp212,& _temp213,& _temp214,& _temp215,& _temp216,& _temp217}; Cyc_Std_aprintf(
_tag_arr("%s '-D_PLATFORM_INCLUDES_=\"arch/%s.h\"'%s%s %s%s%s", sizeof(
unsigned char), 50u), _tag_arr( _temp210, sizeof( void*), 7u));}}}}}}}}); if(
Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp182; _temp182.tag= Cyc_Std_String_pa;
_temp182.f1=( struct _tagged_arr) _temp180;{ void* _temp181[ 1u]={& _temp182};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp181, sizeof( void*), 1u));}});} if( Cyc_Std_system(( struct
_tagged_arr) _temp180) !=  0){ Cyc_compile_failure= 1;({ void* _temp183[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: preprocessing\n", sizeof(
unsigned char), 23u), _tag_arr( _temp183, sizeof( void*), 0u));}); return;} if(
Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position(( struct _tagged_arr)
_temp158);{ struct Cyc_Std___sFILE* in_file= Cyc_try_file_open(( struct
_tagged_arr) _temp158, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("file",
sizeof( unsigned char), 5u)); if( Cyc_compile_failure){ return;}{ struct Cyc_List_List*
tds= 0;{ struct _handler_cons _temp184; _push_handler(& _temp184);{ int _temp186=
0; if( setjmp( _temp184.handler)){ _temp186= 1;} if( ! _temp186){ tds=(( struct
Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(* f)(
struct Cyc_Std___sFILE*, struct Cyc_List_List*), struct Cyc_Std___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("parsing", sizeof(
unsigned char), 8u), Cyc_do_parse,( struct Cyc_Std___sFILE*) _check_null(
in_file), tds);; _pop_handler();} else{ void* _temp185=( void*) _exn_thrown;
void* _temp188= _temp185; _LL190: goto _LL191; _LL192: goto _LL193; _LL191: Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( in_file)); Cyc_remove_file(( struct
_tagged_arr) _temp158);( int) _throw( _temp188); _LL193:( void) _throw( _temp188);
_LL189:;}}} Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( in_file));
if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr) _temp158);
return;}{ struct Cyc_Tcenv_Tenv* _temp194= Cyc_Tcenv_tc_init(); if( Cyc_parseonly_r){
goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("type checking",
sizeof( unsigned char), 14u), Cyc_do_typecheck, _temp194, tds); if( Cyc_compile_failure){
Cyc_remove_file(( struct _tagged_arr) _temp158); return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("control-flow checking",
sizeof( unsigned char), 22u), Cyc_do_cfcheck, 1, tds); if( Cyc_compile_failure){
Cyc_remove_file(( struct _tagged_arr) _temp158); return;} if( Cyc_ic_r){ struct
Cyc_Std___sFILE* inter_file= Cyc_Std_fopen(( struct _tagged_arr) _temp159,
_tag_arr("r", sizeof( unsigned char), 2u)); struct Cyc_Std___sFILE*
inter_objfile= Cyc_try_file_open(( struct _tagged_arr) _temp160, _tag_arr("w",
sizeof( unsigned char), 2u), _tag_arr("interface object file", sizeof(
unsigned char), 22u)); if( inter_objfile ==  0){ Cyc_compile_failure= 1; return;}
Cyc_Position_reset_position(( struct _tagged_arr) _temp159); tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( struct _tuple9*,
struct Cyc_List_List*), struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("interface checking", sizeof( unsigned char), 19u), Cyc_do_interface,({
struct _tuple9* _temp195=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp195->f1= _temp194; _temp195->f2= inter_file; _temp195->f3=( struct Cyc_Std___sFILE*)
_check_null( inter_objfile); _temp195;}), tds); if( inter_file !=  0){ Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( inter_file));} Cyc_Std_file_close(( struct
Cyc_Std___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto PRINTC;}
tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("translation to C", sizeof( unsigned char), 17u), Cyc_do_translate, 1,
tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr) _temp158);
return;} if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();} Cyc_remove_file((
struct _tagged_arr) _temp158); if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){
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
struct _tagged_arr) _temp161, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));}} if( Cyc_compile_failure){
return;}{ struct _handler_cons _temp196; _push_handler(& _temp196);{ int
_temp198= 0; if( setjmp( _temp196.handler)){ _temp198= 1;} if( ! _temp198){ tds=((
struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*), struct Cyc_Std___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("printing", sizeof(
unsigned char), 9u), Cyc_do_print,( struct Cyc_Std___sFILE*) _check_null(
out_file), tds);; _pop_handler();} else{ void* _temp197=( void*) _exn_thrown;
void* _temp200= _temp197; _LL202: goto _LL203; _LL204: goto _LL205; _LL203: Cyc_compile_failure=
1; Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({
struct Cyc_List_List* _temp206=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp206->hd=( void*)({ struct _tagged_arr* _temp207=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp207[ 0]=(
struct _tagged_arr) _temp161; _temp207;}); _temp206->tl= Cyc_cfiles; _temp206;});(
int) _throw( _temp200); _LL205:( void) _throw( _temp200); _LL201:;}}} Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List*
_temp208=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp208->hd=( void*)({ struct _tagged_arr* _temp209=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp209[ 0]=( struct _tagged_arr)
_temp161; _temp209;}); _temp208->tl= Cyc_cfiles; _temp208;});}}}}}}}}}}} static
unsigned char _temp219[ 8u]="<final>"; static struct _tagged_arr Cyc_final_str={
_temp219, _temp219, _temp219 +  8u}; static struct _tagged_arr* Cyc_final_strptr=&
Cyc_final_str; static struct Cyc_Interface_I* Cyc_read_cycio( struct _tagged_arr*
n){ if( n == ( struct _tagged_arr*) Cyc_final_strptr){ return Cyc_Interface_final();}{
struct _tagged_arr basename;{ struct _handler_cons _temp220; _push_handler(&
_temp220);{ int _temp222= 0; if( setjmp( _temp220.handler)){ _temp222= 1;} if( !
_temp222){ basename=( struct _tagged_arr) Cyc_Filename_chop_extension(* n);;
_pop_handler();} else{ void* _temp221=( void*) _exn_thrown; void* _temp224=
_temp221; _LL226: if(*(( void**) _temp224) ==  Cyc_Core_Invalid_argument){ goto
_LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL227: basename=* n; goto
_LL225; _LL229:( void) _throw( _temp224); _LL225:;}}}{ struct _tagged_arr
_temp230= Cyc_Std_strconcat( basename, _tag_arr(".cycio", sizeof( unsigned char),
7u)); struct Cyc_Std___sFILE* _temp231= Cyc_try_file_open(( struct _tagged_arr)
_temp230, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp231 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} Cyc_Position_reset_position(( struct _tagged_arr) _temp230);{ struct
Cyc_Interface_I* _temp232= Cyc_Interface_load(( struct Cyc_Std___sFILE*)
_check_null( _temp231)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp231)); return _temp232;}}}} static int Cyc_is_cfile( struct
_tagged_arr* n){ return*(( const unsigned char*) _check_unknown_subscript(* n,
sizeof( unsigned char), 0)) != '-';} struct _tuple10{ struct _tagged_arr f1; int
f2; struct _tagged_arr f3; void* f4; struct _tagged_arr f5; } ; int Cyc_main(
int argc, struct _tagged_arr argv){ struct _tagged_arr comp=( struct _tagged_arr)
Cstring_to_string( Ccomp); Cyc_set_cpp(( struct _tagged_arr) Cyc_Std_strconcat(
comp, _tag_arr(" -x c -E -U__GNUC__ -nostdinc", sizeof( unsigned char), 30u)));{
struct _tagged_arr cflags=( struct _tagged_arr) Cstring_to_string( Ccflags); if(
Cyc_Std_strlen( cflags) !=  0){ Cyc_add_ccarg( cflags); Cyc_add_cpparg( cflags);}{
struct Cyc_List_List* options=({ struct _tuple10* _temp306[ 42u]; _temp306[ 41u]=({
struct _tuple10* _temp430=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp430->f1= _tag_arr("-printfullevars", sizeof( unsigned char), 16u); _temp430->f2=
0; _temp430->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp430->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp431=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp431[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp432; _temp432.tag= Cyc_Arg_Set_spec; _temp432.f1=& Cyc_print_full_evars_r;
_temp432;}); _temp431;}); _temp430->f5= _tag_arr("Print full information for evars (type debugging)",
sizeof( unsigned char), 50u); _temp430;}); _temp306[ 40u]=({ struct _tuple10*
_temp427=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp427->f1=
_tag_arr("-printallkinds", sizeof( unsigned char), 15u); _temp427->f2= 0;
_temp427->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp427->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp428=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp428[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp429; _temp429.tag= Cyc_Arg_Set_spec; _temp429.f1=& Cyc_print_all_kinds_r;
_temp429;}); _temp428;}); _temp427->f5= _tag_arr("Always print kinds of type variables",
sizeof( unsigned char), 37u); _temp427;}); _temp306[ 39u]=({ struct _tuple10*
_temp424=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp424->f1=
_tag_arr("-printalltvars", sizeof( unsigned char), 15u); _temp424->f2= 0;
_temp424->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp424->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp425=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp425[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp426; _temp426.tag= Cyc_Arg_Set_spec; _temp426.f1=& Cyc_print_all_tvars_r;
_temp426;}); _temp425;}); _temp424->f5= _tag_arr("Print all type variables (even implicit default effects)",
sizeof( unsigned char), 57u); _temp424;}); _temp306[ 38u]=({ struct _tuple10*
_temp421=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp421->f1=
_tag_arr("-noexpandtypedefs", sizeof( unsigned char), 18u); _temp421->f2= 0;
_temp421->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp421->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp422=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp422[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp423; _temp423.tag= Cyc_Arg_Set_spec; _temp423.f1=& Cyc_noexpand_r; _temp423;});
_temp422;}); _temp421->f5= _tag_arr("Don't expand typedefs in pretty printing",
sizeof( unsigned char), 41u); _temp421;}); _temp306[ 37u]=({ struct _tuple10*
_temp418=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp418->f1=
_tag_arr("-noremoveunused", sizeof( unsigned char), 16u); _temp418->f2= 0;
_temp418->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp418->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp419=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp419[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp420; _temp420.tag= Cyc_Arg_Set_spec; _temp420.f1=& Cyc_noshake_r; _temp420;});
_temp419;}); _temp418->f5= _tag_arr("Don't remove externed variables that aren't used",
sizeof( unsigned char), 49u); _temp418;}); _temp306[ 36u]=({ struct _tuple10*
_temp415=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp415->f1=
_tag_arr("-nogc", sizeof( unsigned char), 6u); _temp415->f2= 0; _temp415->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp415->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp416=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp416[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp417; _temp417.tag= Cyc_Arg_Set_spec;
_temp417.f1=& Cyc_nogc_r; _temp417;}); _temp416;}); _temp415->f5= _tag_arr("Don't link in the garbage collector",
sizeof( unsigned char), 36u); _temp415;}); _temp306[ 35u]=({ struct _tuple10*
_temp412=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp412->f1=
_tag_arr("-nocyc", sizeof( unsigned char), 7u); _temp412->f2= 0; _temp412->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp412->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp413=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp413[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp414; _temp414.tag= Cyc_Arg_Unit_spec;
_temp414.f1= Cyc_set_nocyc; _temp414;}); _temp413;}); _temp412->f5= _tag_arr("Don't add implicit namespace Cyc",
sizeof( unsigned char), 33u); _temp412;}); _temp306[ 34u]=({ struct _tuple10*
_temp409=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp409->f1=
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u); _temp409->f2= 0;
_temp409->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp409->f4=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp410=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp410[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp411; _temp411.tag= Cyc_Arg_Unit_spec; _temp411.f1= Cyc_set_nocppprecomp;
_temp411;}); _temp410;}); _temp409->f5= _tag_arr("Don't do smart preprocessing (mac only)",
sizeof( unsigned char), 40u); _temp409;}); _temp306[ 33u]=({ struct _tuple10*
_temp406=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp406->f1=
_tag_arr("-use-cpp", sizeof( unsigned char), 9u); _temp406->f2= 0; _temp406->f3=
_tag_arr("<path>", sizeof( unsigned char), 7u); _temp406->f4=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp407=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp407[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp408; _temp408.tag= Cyc_Arg_String_spec; _temp408.f1=
Cyc_set_cpp; _temp408;}); _temp407;}); _temp406->f5= _tag_arr("Indicate which preprocessor to use",
sizeof( unsigned char), 35u); _temp406;}); _temp306[ 32u]=({ struct _tuple10*
_temp403=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp403->f1=
_tag_arr("--lineno", sizeof( unsigned char), 9u); _temp403->f2= 0; _temp403->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp403->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp404=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp404[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp405; _temp405.tag= Cyc_Arg_Unit_spec;
_temp405.f1= Cyc_set_lineno; _temp405;}); _temp404;}); _temp403->f5= _tag_arr("Generate line numbers for debugging",
sizeof( unsigned char), 36u); _temp403;}); _temp306[ 31u]=({ struct _tuple10*
_temp400=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp400->f1=
_tag_arr("-save-c", sizeof( unsigned char), 8u); _temp400->f2= 0; _temp400->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp400->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp401=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp401[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp402; _temp402.tag= Cyc_Arg_Set_spec;
_temp402.f1=& Cyc_save_c_r; _temp402;}); _temp401;}); _temp400->f5= _tag_arr("Don't delete temporary C files",
sizeof( unsigned char), 31u); _temp400;}); _temp306[ 30u]=({ struct _tuple10*
_temp397=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp397->f1=
_tag_arr("-save-temps", sizeof( unsigned char), 12u); _temp397->f2= 0; _temp397->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp397->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp398=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp398[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp399; _temp399.tag= Cyc_Arg_Unit_spec;
_temp399.f1= Cyc_set_save_temps; _temp399;}); _temp398;}); _temp397->f5=
_tag_arr("Don't delete temporary files", sizeof( unsigned char), 29u); _temp397;});
_temp306[ 29u]=({ struct _tuple10* _temp394=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp394->f1= _tag_arr("-tovc", sizeof( unsigned char),
6u); _temp394->f2= 0; _temp394->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp394->f4=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp395=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp395[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp396; _temp396.tag= Cyc_Arg_Set_spec; _temp396.f1=& Cyc_tovc_r; _temp396;});
_temp395;}); _temp394->f5= _tag_arr("Avoid gcc extensions in C output", sizeof(
unsigned char), 33u); _temp394;}); _temp306[ 28u]=({ struct _tuple10* _temp391=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp391->f1= _tag_arr("-up",
sizeof( unsigned char), 4u); _temp391->f2= 0; _temp391->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp391->f4=( void*)({ struct Cyc_Arg_Clear_spec_struct*
_temp392=( struct Cyc_Arg_Clear_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Clear_spec_struct));
_temp392[ 0]=({ struct Cyc_Arg_Clear_spec_struct _temp393; _temp393.tag= Cyc_Arg_Clear_spec;
_temp393.f1=& Cyc_pp_r; _temp393;}); _temp392;}); _temp391->f5= _tag_arr("Ugly print",
sizeof( unsigned char), 11u); _temp391;}); _temp306[ 27u]=({ struct _tuple10*
_temp388=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp388->f1=
_tag_arr("-pp", sizeof( unsigned char), 4u); _temp388->f2= 0; _temp388->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp388->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp389=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp389[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp390; _temp390.tag= Cyc_Arg_Set_spec;
_temp390.f1=& Cyc_pp_r; _temp390;}); _temp389;}); _temp388->f5= _tag_arr("Pretty print",
sizeof( unsigned char), 13u); _temp388;}); _temp306[ 26u]=({ struct _tuple10*
_temp385=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp385->f1=
_tag_arr("-ic", sizeof( unsigned char), 4u); _temp385->f2= 0; _temp385->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp385->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp386=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp386[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp387; _temp387.tag= Cyc_Arg_Set_spec;
_temp387.f1=& Cyc_ic_r; _temp387;}); _temp386;}); _temp385->f5= _tag_arr("Activate the link-checker",
sizeof( unsigned char), 26u); _temp385;}); _temp306[ 25u]=({ struct _tuple10*
_temp382=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp382->f1=
_tag_arr("-stopafter-toc", sizeof( unsigned char), 15u); _temp382->f2= 0;
_temp382->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp382->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp383=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp383[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp384; _temp384.tag= Cyc_Arg_Set_spec; _temp384.f1=& Cyc_toc_r; _temp384;});
_temp383;}); _temp382->f5= _tag_arr("Stop after translation to C", sizeof(
unsigned char), 28u); _temp382;}); _temp306[ 24u]=({ struct _tuple10* _temp379=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp379->f1= _tag_arr("-stopafter-tc",
sizeof( unsigned char), 14u); _temp379->f2= 0; _temp379->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp379->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp380=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp380[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp381; _temp381.tag= Cyc_Arg_Set_spec;
_temp381.f1=& Cyc_tc_r; _temp381;}); _temp380;}); _temp379->f5= _tag_arr("Stop after type checking",
sizeof( unsigned char), 25u); _temp379;}); _temp306[ 23u]=({ struct _tuple10*
_temp376=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp376->f1=
_tag_arr("-stopafter-parse", sizeof( unsigned char), 17u); _temp376->f2= 0;
_temp376->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp376->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp377=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp377[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp378; _temp378.tag= Cyc_Arg_Set_spec; _temp378.f1=& Cyc_parseonly_r;
_temp378;}); _temp377;}); _temp376->f5= _tag_arr("Stop after parsing", sizeof(
unsigned char), 19u); _temp376;}); _temp306[ 22u]=({ struct _tuple10* _temp373=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp373->f1= _tag_arr("-E",
sizeof( unsigned char), 3u); _temp373->f2= 0; _temp373->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp373->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp374=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp374[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp375; _temp375.tag= Cyc_Arg_Set_spec;
_temp375.f1=& Cyc_stop_after_cpp_r; _temp375;}); _temp374;}); _temp373->f5=
_tag_arr("Stop after preprocessing", sizeof( unsigned char), 25u); _temp373;});
_temp306[ 21u]=({ struct _tuple10* _temp370=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp370->f1= _tag_arr("-MT", sizeof( unsigned char),
4u); _temp370->f2= 0; _temp370->f3= _tag_arr(" <target>", sizeof( unsigned char),
10u); _temp370->f4=( void*)({ struct Cyc_Arg_String_spec_struct* _temp371=(
struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp371[ 0]=({ struct Cyc_Arg_String_spec_struct _temp372; _temp372.tag= Cyc_Arg_String_spec;
_temp372.f1= Cyc_set_dependencies_target; _temp372;}); _temp371;}); _temp370->f5=
_tag_arr("Give target for dependencies", sizeof( unsigned char), 29u); _temp370;});
_temp306[ 20u]=({ struct _tuple10* _temp367=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp367->f1= _tag_arr("-MG", sizeof( unsigned char),
4u); _temp367->f2= 0; _temp367->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp367->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct* _temp368=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp368[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp369; _temp369.tag= Cyc_Arg_Flag_spec; _temp369.f1= Cyc_add_cpparg; _temp369;});
_temp368;}); _temp367->f5= _tag_arr("When producing dependencies assume that missing files are generated",
sizeof( unsigned char), 68u); _temp367;}); _temp306[ 19u]=({ struct _tuple10*
_temp364=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp364->f1=
_tag_arr("-M", sizeof( unsigned char), 3u); _temp364->f2= 0; _temp364->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp364->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp365=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp365[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp366; _temp366.tag= Cyc_Arg_Unit_spec;
_temp366.f1= Cyc_set_produce_dependencies; _temp366;}); _temp365;}); _temp364->f5=
_tag_arr("Produce dependencies", sizeof( unsigned char), 21u); _temp364;});
_temp306[ 18u]=({ struct _tuple10* _temp361=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp361->f1= _tag_arr("-S", sizeof( unsigned char),
3u); _temp361->f2= 0; _temp361->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp361->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct* _temp362=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp362[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp363; _temp363.tag= Cyc_Arg_Unit_spec; _temp363.f1= Cyc_set_stop_after_asmfile;
_temp363;}); _temp362;}); _temp361->f5= _tag_arr("Stop after producing assembly code",
sizeof( unsigned char), 35u); _temp361;}); _temp306[ 17u]=({ struct _tuple10*
_temp358=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp358->f1=
_tag_arr("-pa", sizeof( unsigned char), 4u); _temp358->f2= 0; _temp358->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp358->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp359=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp359[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp360; _temp360.tag= Cyc_Arg_Unit_spec;
_temp360.f1= Cyc_set_pa; _temp360;}); _temp359;}); _temp358->f5= _tag_arr("Compile for profiling with aprof",
sizeof( unsigned char), 33u); _temp358;}); _temp306[ 16u]=({ struct _tuple10*
_temp355=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp355->f1=
_tag_arr("-pg", sizeof( unsigned char), 4u); _temp355->f2= 0; _temp355->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp355->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp356=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp356[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp357; _temp357.tag= Cyc_Arg_Flag_spec;
_temp357.f1= Cyc_add_ccarg; _temp357;}); _temp356;}); _temp355->f5= _tag_arr("Compile for profiling with gprof",
sizeof( unsigned char), 33u); _temp355;}); _temp306[ 15u]=({ struct _tuple10*
_temp352=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp352->f1=
_tag_arr("-p", sizeof( unsigned char), 3u); _temp352->f2= 0; _temp352->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp352->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp353=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp353[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp354; _temp354.tag= Cyc_Arg_Flag_spec;
_temp354.f1= Cyc_add_ccarg; _temp354;}); _temp353;}); _temp352->f5= _tag_arr("Compile for profiling with prof",
sizeof( unsigned char), 32u); _temp352;}); _temp306[ 14u]=({ struct _tuple10*
_temp349=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp349->f1=
_tag_arr("-g", sizeof( unsigned char), 3u); _temp349->f2= 0; _temp349->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp349->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp350=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp350[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp351; _temp351.tag= Cyc_Arg_Flag_spec;
_temp351.f1= Cyc_add_ccarg; _temp351;}); _temp350;}); _temp349->f5= _tag_arr("Compile for debugging",
sizeof( unsigned char), 22u); _temp349;}); _temp306[ 13u]=({ struct _tuple10*
_temp346=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp346->f1=
_tag_arr("-O3", sizeof( unsigned char), 4u); _temp346->f2= 0; _temp346->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp346->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp347=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp347[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp348; _temp348.tag= Cyc_Arg_Flag_spec;
_temp348.f1= Cyc_add_ccarg; _temp348;}); _temp347;}); _temp346->f5= _tag_arr("Even more optimization",
sizeof( unsigned char), 23u); _temp346;}); _temp306[ 12u]=({ struct _tuple10*
_temp343=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp343->f1=
_tag_arr("-O2", sizeof( unsigned char), 4u); _temp343->f2= 0; _temp343->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp343->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp344=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp344[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp345; _temp345.tag= Cyc_Arg_Flag_spec;
_temp345.f1= Cyc_add_ccarg; _temp345;}); _temp344;}); _temp343->f5= _tag_arr("A higher level of optimization",
sizeof( unsigned char), 31u); _temp343;}); _temp306[ 11u]=({ struct _tuple10*
_temp340=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp340->f1=
_tag_arr("-O", sizeof( unsigned char), 3u); _temp340->f2= 0; _temp340->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp340->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp341=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp341[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp342; _temp342.tag= Cyc_Arg_Flag_spec;
_temp342.f1= Cyc_add_ccarg; _temp342;}); _temp341;}); _temp340->f5= _tag_arr("Optimize",
sizeof( unsigned char), 9u); _temp340;}); _temp306[ 10u]=({ struct _tuple10*
_temp337=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp337->f1=
_tag_arr("-s", sizeof( unsigned char), 3u); _temp337->f2= 0; _temp337->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp337->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp338=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp338[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp339; _temp339.tag= Cyc_Arg_Flag_spec;
_temp339.f1= Cyc_add_ccarg; _temp339;}); _temp338;}); _temp337->f5= _tag_arr("Remove all symbol table and relocation info from executable",
sizeof( unsigned char), 60u); _temp337;}); _temp306[ 9u]=({ struct _tuple10*
_temp334=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp334->f1=
_tag_arr("-x", sizeof( unsigned char), 3u); _temp334->f2= 0; _temp334->f3=
_tag_arr(" <language>", sizeof( unsigned char), 12u); _temp334->f4=( void*)({
struct Cyc_Arg_String_spec_struct* _temp335=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp335[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp336; _temp336.tag= Cyc_Arg_String_spec; _temp336.f1=
Cyc_set_inputtype; _temp336;}); _temp335;}); _temp334->f5= _tag_arr("Specify <language> for the following input files",
sizeof( unsigned char), 49u); _temp334;}); _temp306[ 8u]=({ struct _tuple10*
_temp331=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp331->f1=
_tag_arr("-c", sizeof( unsigned char), 3u); _temp331->f2= 0; _temp331->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp331->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp332=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp332[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp333; _temp333.tag= Cyc_Arg_Unit_spec;
_temp333.f1= Cyc_set_stop_after_objectfile; _temp333;}); _temp332;}); _temp331->f5=
_tag_arr("Produce an object file instead of an executable; do not link", sizeof(
unsigned char), 61u); _temp331;}); _temp306[ 7u]=({ struct _tuple10* _temp328=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp328->f1= _tag_arr("-l",
sizeof( unsigned char), 3u); _temp328->f2= 1; _temp328->f3= _tag_arr("<file>",
sizeof( unsigned char), 7u); _temp328->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp329=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp329[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp330; _temp330.tag= Cyc_Arg_Flag_spec;
_temp330.f1= Cyc_add_libarg; _temp330;}); _temp329;}); _temp328->f5= _tag_arr("Library file",
sizeof( unsigned char), 13u); _temp328;}); _temp306[ 6u]=({ struct _tuple10*
_temp325=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp325->f1=
_tag_arr("-L", sizeof( unsigned char), 3u); _temp325->f2= 1; _temp325->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp325->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp326=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp326[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp327; _temp327.tag= Cyc_Arg_Flag_spec; _temp327.f1= Cyc_add_cpparg; _temp327;});
_temp326;}); _temp325->f5= _tag_arr("Add to the list of directories for -l",
sizeof( unsigned char), 38u); _temp325;}); _temp306[ 5u]=({ struct _tuple10*
_temp322=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp322->f1=
_tag_arr("-I", sizeof( unsigned char), 3u); _temp322->f2= 1; _temp322->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp322->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp323=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp323[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp324; _temp324.tag= Cyc_Arg_Flag_spec; _temp324.f1= Cyc_add_cpparg; _temp324;});
_temp323;}); _temp322->f5= _tag_arr("Add to the list of directories to search for include files",
sizeof( unsigned char), 59u); _temp322;}); _temp306[ 4u]=({ struct _tuple10*
_temp319=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp319->f1=
_tag_arr("-B", sizeof( unsigned char), 3u); _temp319->f2= 1; _temp319->f3=
_tag_arr("<file>", sizeof( unsigned char), 7u); _temp319->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp320=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp320[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp321; _temp321.tag= Cyc_Arg_Flag_spec; _temp321.f1= Cyc_add_cyclone_lib_path;
_temp321;}); _temp320;}); _temp319->f5= _tag_arr("Add to the list of directories to search for compiler files",
sizeof( unsigned char), 60u); _temp319;}); _temp306[ 3u]=({ struct _tuple10*
_temp316=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp316->f1=
_tag_arr("-D", sizeof( unsigned char), 3u); _temp316->f2= 1; _temp316->f3=
_tag_arr("<name>[=<value>]", sizeof( unsigned char), 17u); _temp316->f4=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp317=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp317[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp318; _temp318.tag= Cyc_Arg_Flag_spec; _temp318.f1= Cyc_add_cpparg; _temp318;});
_temp317;}); _temp316->f5= _tag_arr("Pass definition to preprocessor", sizeof(
unsigned char), 32u); _temp316;}); _temp306[ 2u]=({ struct _tuple10* _temp313=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp313->f1= _tag_arr("-o",
sizeof( unsigned char), 3u); _temp313->f2= 0; _temp313->f3= _tag_arr(" <file>",
sizeof( unsigned char), 8u); _temp313->f4=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp314=( struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp314[ 0]=({ struct Cyc_Arg_String_spec_struct _temp315; _temp315.tag= Cyc_Arg_String_spec;
_temp315.f1= Cyc_set_output_file; _temp315;}); _temp314;}); _temp313->f5=
_tag_arr("Set the output file name to <file>", sizeof( unsigned char), 35u);
_temp313;}); _temp306[ 1u]=({ struct _tuple10* _temp310=( struct _tuple10*)
_cycalloc( sizeof( struct _tuple10)); _temp310->f1= _tag_arr("--version",
sizeof( unsigned char), 10u); _temp310->f2= 0; _temp310->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp310->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp311=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp311[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp312; _temp312.tag= Cyc_Arg_Unit_spec;
_temp312.f1= Cyc_print_version; _temp312;}); _temp311;}); _temp310->f5= _tag_arr("Print version number and exit",
sizeof( unsigned char), 30u); _temp310;}); _temp306[ 0u]=({ struct _tuple10*
_temp307=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp307->f1=
_tag_arr("-v", sizeof( unsigned char), 3u); _temp307->f2= 0; _temp307->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp307->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp308=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp308[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp309; _temp309.tag= Cyc_Arg_Set_spec;
_temp309.f1=& Cyc_v_r; _temp309;}); _temp308;}); _temp307->f5= _tag_arr("Print compilation stages verbosely",
sizeof( unsigned char), 35u); _temp307;});(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp306, sizeof( struct _tuple10*), 42u));});
Cyc_Arg_parse( options, Cyc_add_other, _tag_arr("Options:", sizeof(
unsigned char), 9u), argv);{ struct _tagged_arr cyclone_exec_prefix=( struct
_tagged_arr) Cyc_Std_getenv( _tag_arr("CYCLONE_EXEC_PREFIX", sizeof(
unsigned char), 20u)); if( cyclone_exec_prefix.curr != (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr){ Cyc_cyclone_lib_path=({ struct Cyc_List_List*
_temp233=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp233->hd=( void*)({ struct _tagged_arr* _temp234=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp234[ 0]= cyclone_exec_prefix;
_temp234;}); _temp233->tl= Cyc_cyclone_lib_path; _temp233;});}{ struct
_tagged_arr def_lib_path=( struct _tagged_arr) Cstring_to_string( Cdef_lib_path);
if( Cyc_Std_strlen( def_lib_path) >  0){ Cyc_cyclone_lib_path=({ struct Cyc_List_List*
_temp235=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp235->hd=( void*)({ struct _tagged_arr* _temp236=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp236[ 0]= def_lib_path; _temp236;});
_temp235->tl= Cyc_cyclone_lib_path; _temp235;});} if( Cyc_cyclone_lib_path !=  0){
Cyc_add_ccarg2( _tag_arr("-L", sizeof( unsigned char), 3u),*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_cyclone_lib_path))->hd));}
Cyc_cyclone_lib_path=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_cyclone_lib_path); Cyc_cyc_include= Cyc_do_find( Cyc_cyclone_lib_path,
_tag_arr("include/cyc_include.h", sizeof( unsigned char), 22u));{ struct Cyc_List_List*
_temp237=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
Cyc_cyclone_files); for( 0; _temp237 !=  0; _temp237= _temp237->tl){ Cyc_process_file(*((
struct _tagged_arr*) _temp237->hd)); if( Cyc_compile_failure){ return 1;}}} if(((
Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)? 1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;}
if( Cyc_ccargs ==  0){ return 0;} Cyc_ccargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{ struct _tagged_arr _temp238= Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tagged_arr*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr, Cyc_ccargs), _tag_arr(" ",
sizeof( unsigned char), 2u)); Cyc_libargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_libargs);{ struct _tagged_arr _temp239=
Cyc_Std_str_sepstr(({ struct Cyc_List_List* _temp305=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp305->hd=( void*) _init_tag_arr((
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"", sizeof(
unsigned char), 1u); _temp305->tl=(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_libargs); _temp305;}), _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_List_List* stdlib; struct _tagged_arr
stdlib_string; int _temp240=(( Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".a", sizeof( unsigned char), 3u)): 0))?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".lib", sizeof( unsigned char), 5u)): 0);
if( _temp240){ stdlib= 0; stdlib_string= _tag_arr("", sizeof( unsigned char), 1u);}
else{ struct _tagged_arr libcyc_filename= Cyc_pa_r? _tag_arr("libcyc_a.a",
sizeof( unsigned char), 11u): _tag_arr("libcyc.a", sizeof( unsigned char), 9u);
struct _tagged_arr nogc_filename= Cyc_pa_r? _tag_arr("nogc_a.a", sizeof(
unsigned char), 9u): _tag_arr("nogc.a", sizeof( unsigned char), 7u); struct
_tagged_arr gc_filename= _tag_arr("gc.a", sizeof( unsigned char), 5u); struct
_tagged_arr _temp241= Cyc_do_find( Cyc_cyclone_lib_path, libcyc_filename);
struct _tagged_arr _temp242= Cyc_nogc_r? Cyc_do_find( Cyc_cyclone_lib_path,
nogc_filename): Cyc_do_find( Cyc_cyclone_lib_path, gc_filename); stdlib=({
struct _tagged_arr* _temp243[ 1u]; _temp243[ 0u]=({ struct _tagged_arr* _temp244=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp244[ 0]=
_temp241; _temp244;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp243, sizeof( struct _tagged_arr*), 1u));}); stdlib_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp247; _temp247.tag=
Cyc_Std_String_pa; _temp247.f1=( struct _tagged_arr) _temp242;{ struct Cyc_Std_String_pa_struct
_temp246; _temp246.tag= Cyc_Std_String_pa; _temp246.f1=( struct _tagged_arr)
_temp241;{ void* _temp245[ 2u]={& _temp246,& _temp247}; Cyc_Std_aprintf(
_tag_arr(" %s %s", sizeof( unsigned char), 7u), _tag_arr( _temp245, sizeof( void*),
2u));}}});} if( Cyc_ic_r){ struct _handler_cons _temp248; _push_handler(&
_temp248);{ int _temp250= 0; if( setjmp( _temp248.handler)){ _temp250= 1;} if( !
_temp250){ Cyc_ccargs=(( struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*),
struct Cyc_List_List* x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=((
struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List*
_temp251=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( stdlib,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_ccargs, Cyc_libargs)); if( !
_temp240){ _temp251=({ struct Cyc_List_List* _temp252=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp252->hd=( void*) Cyc_final_strptr;
_temp252->tl= _temp251; _temp252;});}{ struct Cyc_Interface_I* _temp253=((
struct Cyc_Interface_I*(*)( struct Cyc_Interface_I*(* get)( struct _tagged_arr*),
struct Cyc_List_List* la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio, _temp251, _temp251); if( _temp253 ==  0){({ void* _temp254[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: interfaces incompatible\n",
sizeof( unsigned char), 32u), _tag_arr( _temp254, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp255= 1; _npop_handler( 0u); return _temp255;}}
if( _temp240){ if( Cyc_output_file !=  0){ struct _tagged_arr _temp256=({ struct
Cyc_Std_String_pa_struct _temp260; _temp260.tag= Cyc_Std_String_pa; _temp260.f1=(
struct _tagged_arr) Cyc_Filename_chop_extension(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp259[ 1u]={& _temp260}; Cyc_Std_aprintf(
_tag_arr("%s.cycio", sizeof( unsigned char), 9u), _tag_arr( _temp259, sizeof(
void*), 1u));}}); struct Cyc_Std___sFILE* _temp257= Cyc_try_file_open(( struct
_tagged_arr) _temp256, _tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp257 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{
int _temp258= 1; _npop_handler( 0u); return _temp258;}} Cyc_Interface_save((
struct Cyc_Interface_I*) _check_null( _temp253),( struct Cyc_Std___sFILE*)
_check_null( _temp257)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp257));}} else{ if( ! Cyc_Interface_is_subinterface( Cyc_Interface_empty(),(
struct Cyc_Interface_I*) _check_null( _temp253),({ struct _tuple8* _temp261=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp261->f1= _tag_arr("empty interface",
sizeof( unsigned char), 16u); _temp261->f2= _tag_arr("global interface", sizeof(
unsigned char), 17u); _temp261;}))){({ void* _temp262[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: some objects are still undefined\n", sizeof(
unsigned char), 41u), _tag_arr( _temp262, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp263= 1; _npop_handler( 0u); return _temp263;}}}}};
_pop_handler();} else{ void* _temp249=( void*) _exn_thrown; void* _temp265=
_temp249; struct _tagged_arr _temp279; struct _tagged_arr _temp281; struct
_tagged_arr _temp283; _LL267: if(*(( void**) _temp265) ==  Cyc_Core_Failure){
_LL280: _temp279=(( struct Cyc_Core_Failure_struct*) _temp265)->f1; goto _LL268;}
else{ goto _LL269;} _LL269: if(*(( void**) _temp265) ==  Cyc_Core_Impossible){
_LL282: _temp281=(( struct Cyc_Core_Impossible_struct*) _temp265)->f1; goto
_LL270;} else{ goto _LL271;} _LL271: if( _temp265 ==  Cyc_Dict_Absent){ goto
_LL272;} else{ goto _LL273;} _LL273: if(*(( void**) _temp265) ==  Cyc_Core_Invalid_argument){
_LL284: _temp283=(( struct Cyc_Core_Invalid_argument_struct*) _temp265)->f1;
goto _LL274;} else{ goto _LL275;} _LL275: goto _LL276; _LL277: goto _LL278;
_LL268:({ struct Cyc_Std_String_pa_struct _temp286; _temp286.tag= Cyc_Std_String_pa;
_temp286.f1=( struct _tagged_arr) _temp279;{ void* _temp285[ 1u]={& _temp286};
Cyc_Std_printf( _tag_arr("Exception Core::Failure %s\n", sizeof( unsigned char),
28u), _tag_arr( _temp285, sizeof( void*), 1u));}}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL270:({ struct Cyc_Std_String_pa_struct _temp288; _temp288.tag= Cyc_Std_String_pa;
_temp288.f1=( struct _tagged_arr) _temp281;{ void* _temp287[ 1u]={& _temp288};
Cyc_Std_printf( _tag_arr("Exception Core::Impossible %s\n", sizeof(
unsigned char), 31u), _tag_arr( _temp287, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL272:({ void* _temp289[ 0u]={}; Cyc_Std_printf(
_tag_arr("Exception Dict::Absent\n", sizeof( unsigned char), 24u), _tag_arr(
_temp289, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL274:({ struct Cyc_Std_String_pa_struct _temp291; _temp291.tag= Cyc_Std_String_pa;
_temp291.f1=( struct _tagged_arr) _temp283;{ void* _temp290[ 1u]={& _temp291};
Cyc_Std_printf( _tag_arr("Exception Core::Invalid_argument %s\n", sizeof(
unsigned char), 37u), _tag_arr( _temp290, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL276:({ void* _temp292[ 0u]={}; Cyc_Std_printf(
_tag_arr("Uncaught exception\n", sizeof( unsigned char), 20u), _tag_arr(
_temp292, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL278:( void) _throw( _temp265); _LL266:;}}}{ struct _tagged_arr
_temp293=({ struct Cyc_Std_String_pa_struct _temp302; _temp302.tag= Cyc_Std_String_pa;
_temp302.f1=( struct _tagged_arr) _temp239;{ struct Cyc_Std_String_pa_struct
_temp301; _temp301.tag= Cyc_Std_String_pa; _temp301.f1=( struct _tagged_arr)
stdlib_string;{ struct Cyc_Std_String_pa_struct _temp300; _temp300.tag= Cyc_Std_String_pa;
_temp300.f1=( struct _tagged_arr) _temp238;{ struct Cyc_Std_String_pa_struct
_temp299; _temp299.tag= Cyc_Std_String_pa; _temp299.f1=( struct _tagged_arr)(
Cyc_output_file ==  0? _tag_arr("", sizeof( unsigned char), 1u):( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp304; _temp304.tag= Cyc_Std_String_pa;
_temp304.f1=( struct _tagged_arr) Cyc_sh_escape_string(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp303[ 1u]={& _temp304}; Cyc_Std_aprintf(
_tag_arr(" -o %s", sizeof( unsigned char), 7u), _tag_arr( _temp303, sizeof( void*),
1u));}}));{ struct Cyc_Std_String_pa_struct _temp298; _temp298.tag= Cyc_Std_String_pa;
_temp298.f1=( struct _tagged_arr) comp;{ void* _temp297[ 5u]={& _temp298,&
_temp299,& _temp300,& _temp301,& _temp302}; Cyc_Std_aprintf( _tag_arr("%s %s %s%s%s",
sizeof( unsigned char), 13u), _tag_arr( _temp297, sizeof( void*), 5u));}}}}}});
if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp295; _temp295.tag= Cyc_Std_String_pa;
_temp295.f1=( struct _tagged_arr) _temp293;{ void* _temp294[ 1u]={& _temp295};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp294, sizeof( void*), 1u));}});} if( Cyc_Std_system(( struct
_tagged_arr) _temp293) !=  0){({ void* _temp296[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Error: C compiler failed\n", sizeof( unsigned char), 26u), _tag_arr(
_temp296, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1;} Cyc_remove_cfiles(); return Cyc_compile_failure? 1: 0;}}}}}}}}
