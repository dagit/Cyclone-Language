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
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_vrprintf(
struct _RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static
const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int tag;
short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern int Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_exists_c( int(* pred)( void*, void*), void* env, struct
Cyc_List_List* x); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1; static
const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3; static
const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1; int
q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0; static
const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const int
Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl* base; struct Cyc_Absyn_Tunionfield* field; } ;
extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg1, struct _tagged_arr fmt, struct _tagged_arr ap); struct
_tuple3{ void* f1; int f2; } ; extern struct _tuple3 Cyc_Tcdecl_merge_scope(
void* s0, void* s1, struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern void* Cyc_Tcdecl_merge_binding( void* d0,
void* d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern
struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl( struct
Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct
Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f, int* res, struct
_tagged_arr* v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg);
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern int Cyc_Std_strptrcmp( struct
_tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_Std_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct
Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2); extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*,
struct Cyc_List_List*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_arr Cyc_Absynpp_scope2string( void* sc); unsigned char Cyc_Tcdecl_Incompatible[
17u]="\000\000\000\000Incompatible"; void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg1, struct _tagged_arr fmt, struct _tagged_arr ap){
if( msg1 ==  0){( int) _throw(( void*) Cyc_Tcdecl_Incompatible);}{ struct
_tagged_arr fmt2=( struct _tagged_arr) Cyc_Std_strconcat( _tag_arr("%s ",
sizeof( unsigned char), 4u), fmt); struct _tagged_arr ap2=({ unsigned int _temp0=
_get_arr_size( ap, sizeof( void*)) +  1; void** _temp1=( void**) _cycalloc(
_check_times( sizeof( void*), _temp0)); struct _tagged_arr _temp5= _tag_arr(
_temp1, sizeof( void*), _get_arr_size( ap, sizeof( void*)) +  1);{ unsigned int
_temp2= _temp0; unsigned int i; for( i= 0; i <  _temp2; i ++){ _temp1[ i]= i == 
0?( void*)({ struct Cyc_Std_String_pa_struct* _temp3=( struct Cyc_Std_String_pa_struct*)
_cycalloc( sizeof( struct Cyc_Std_String_pa_struct)); _temp3[ 0]=({ struct Cyc_Std_String_pa_struct
_temp4; _temp4.tag= Cyc_Std_String_pa; _temp4.f1=( struct _tagged_arr)* msg1;
_temp4;}); _temp3;}):*(( void**) _check_unknown_subscript( ap, sizeof( void*),(
int)( i -  1)));}}; _temp5;}); Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc,( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt2, ap2)));}}
static void Cyc_Tcdecl_merge_scope_err( void* s0, void* s1, struct _tagged_arr t,
struct _tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){({
struct Cyc_Std_String_pa_struct _temp10; _temp10.tag= Cyc_Std_String_pa; _temp10.f1=(
struct _tagged_arr) Cyc_Absynpp_scope2string( s0);{ struct Cyc_Std_String_pa_struct
_temp9; _temp9.tag= Cyc_Std_String_pa; _temp9.f1=( struct _tagged_arr) Cyc_Absynpp_scope2string(
s1);{ struct Cyc_Std_String_pa_struct _temp8; _temp8.tag= Cyc_Std_String_pa;
_temp8.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp7;
_temp7.tag= Cyc_Std_String_pa; _temp7.f1=( struct _tagged_arr) t;{ void* _temp6[
4u]={& _temp7,& _temp8,& _temp9,& _temp10}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s is %s whereas expected scope is %s",
sizeof( unsigned char), 41u), _tag_arr( _temp6, sizeof( void*), 4u));}}}}});}
struct _tuple4{ void* f1; void* f2; } ; struct _tuple3 Cyc_Tcdecl_merge_scope(
void* s0, void* s1, struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){{ struct _tuple4 _temp12=({ struct _tuple4 _temp11;
_temp11.f1= s0; _temp11.f2= s1; _temp11;}); void* _temp32; void* _temp34; void*
_temp36; void* _temp38; void* _temp40; void* _temp42; void* _temp44; void*
_temp46; void* _temp48; void* _temp50; void* _temp52; void* _temp54; _LL14:
_LL35: _temp34= _temp12.f1; if( _temp34 == ( void*) Cyc_Absyn_ExternC){ goto
_LL33;} else{ goto _LL16;} _LL33: _temp32= _temp12.f2; if( _temp32 == ( void*)
Cyc_Absyn_ExternC){ goto _LL15;} else{ goto _LL16;} _LL16: _LL37: _temp36=
_temp12.f1; if( _temp36 == ( void*) Cyc_Absyn_ExternC){ goto _LL17;} else{ goto
_LL18;} _LL18: _LL39: _temp38= _temp12.f2; if( _temp38 == ( void*) Cyc_Absyn_ExternC){
goto _LL19;} else{ goto _LL20;} _LL20: _LL41: _temp40= _temp12.f2; if( _temp40
== ( void*) Cyc_Absyn_Extern){ goto _LL21;} else{ goto _LL22;} _LL22: _LL43:
_temp42= _temp12.f1; if( _temp42 == ( void*) Cyc_Absyn_Extern){ goto _LL23;}
else{ goto _LL24;} _LL24: _LL47: _temp46= _temp12.f1; if( _temp46 == ( void*)
Cyc_Absyn_Static){ goto _LL45;} else{ goto _LL26;} _LL45: _temp44= _temp12.f2;
if( _temp44 == ( void*) Cyc_Absyn_Static){ goto _LL25;} else{ goto _LL26;} _LL26:
_LL51: _temp50= _temp12.f1; if( _temp50 == ( void*) Cyc_Absyn_Public){ goto
_LL49;} else{ goto _LL28;} _LL49: _temp48= _temp12.f2; if( _temp48 == ( void*)
Cyc_Absyn_Public){ goto _LL27;} else{ goto _LL28;} _LL28: _LL55: _temp54=
_temp12.f1; if( _temp54 == ( void*) Cyc_Absyn_Abstract){ goto _LL53;} else{ goto
_LL30;} _LL53: _temp52= _temp12.f2; if( _temp52 == ( void*) Cyc_Absyn_Abstract){
goto _LL29;} else{ goto _LL30;} _LL30: goto _LL31; _LL15: goto _LL13; _LL17:
goto _LL19; _LL19: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp56; _temp56.f1= s1; _temp56.f2= 0; _temp56;}); _LL21: s1= s0;
goto _LL13; _LL23: goto _LL13; _LL25: goto _LL27; _LL27: goto _LL29; _LL29: goto
_LL13; _LL31: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp57; _temp57.f1= s1; _temp57.f2= 0; _temp57;}); _LL13:;}
return({ struct _tuple3 _temp58; _temp58.f1= s1; _temp58.f2= 1; _temp58;});}
static int Cyc_Tcdecl_check_type( void* t0, void* t1){ return Cyc_Tcutil_unify(
t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value( struct Cyc_Absyn_Exp*
e){ void* _temp59=( void*) e->r; void* _temp65; int _temp67; _LL61: if(*(( int*)
_temp59) ==  Cyc_Absyn_Const_e){ _LL66: _temp65=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp59)->f1; if(( unsigned int) _temp65 >  1u?*(( int*) _temp65) ==  Cyc_Absyn_Int_c:
0){ _LL68: _temp67=(( struct Cyc_Absyn_Int_c_struct*) _temp65)->f2; goto _LL62;}
else{ goto _LL63;}} else{ goto _LL63;} _LL63: goto _LL64; _LL62: return(
unsigned int) _temp67; _LL64:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp69=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp69[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp70; _temp70.tag= Cyc_Core_Invalid_argument; _temp70.f1= _tag_arr("Tcdecl::get_uint_const_value",
sizeof( unsigned char), 29u); _temp70;}); _temp69;})); _LL60:;} inline static
int Cyc_Tcdecl_check_tvs( struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs0) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs1)){({ struct
Cyc_Std_String_pa_struct _temp73; _temp73.tag= Cyc_Std_String_pa; _temp73.f1=(
struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp72; _temp72.tag=
Cyc_Std_String_pa; _temp72.f1=( struct _tagged_arr) t;{ void* _temp71[ 2u]={&
_temp72,& _temp73}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s has a different number of type parameters",
sizeof( unsigned char), 48u), _tag_arr( _temp71, sizeof( void*), 2u));}}});
return 0;}{ struct Cyc_List_List* _temp74= tvs0; struct Cyc_List_List* _temp75=
tvs1; for( 0; _temp74 !=  0;( _temp74= _temp74->tl, _temp75= _temp75->tl)){
struct Cyc_Absyn_Conref* _temp76= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)
_temp74->hd)->kind); struct Cyc_Absyn_Conref* _temp77= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp75))->hd)->kind);
if( Cyc_Absyn_conref_val( _temp76) !=  Cyc_Absyn_conref_val( _temp77)){({ struct
Cyc_Std_String_pa_struct _temp83; _temp83.tag= Cyc_Std_String_pa; _temp83.f1=(
struct _tagged_arr) Cyc_Absynpp_ckind2string( _temp77);{ struct Cyc_Std_String_pa_struct
_temp82; _temp82.tag= Cyc_Std_String_pa; _temp82.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*) _temp74->hd)->name;{ struct Cyc_Std_String_pa_struct
_temp81; _temp81.tag= Cyc_Std_String_pa; _temp81.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
_temp76);{ struct Cyc_Std_String_pa_struct _temp80; _temp80.tag= Cyc_Std_String_pa;
_temp80.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp79;
_temp79.tag= Cyc_Std_String_pa; _temp79.f1=( struct _tagged_arr) t;{ void*
_temp78[ 5u]={& _temp79,& _temp80,& _temp81,& _temp82,& _temp83}; Cyc_Tcdecl_merr(
loc, msg, _tag_arr("%s %s has a different kind (%s) for type parameter %s (%s)",
sizeof( unsigned char), 59u), _tag_arr( _temp78, sizeof( void*), 5u));}}}}}});
return 0;}} return 1;}} static int Cyc_Tcdecl_check_atts( struct Cyc_List_List*
atts0, struct Cyc_List_List* atts1, struct _tagged_arr t, struct _tagged_arr v,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){({ struct Cyc_Std_String_pa_struct _temp86; _temp86.tag= Cyc_Std_String_pa;
_temp86.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp85;
_temp85.tag= Cyc_Std_String_pa; _temp85.f1=( struct _tagged_arr) t;{ void*
_temp84[ 2u]={& _temp85,& _temp86}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s has different attributes",
sizeof( unsigned char), 31u), _tag_arr( _temp84, sizeof( void*), 2u));}}});
return 0;} return 1;} struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List* _temp87= 0; for( 0;
tvs0 !=  0;( tvs0= tvs0->tl, tvs1= tvs1->tl)){ _temp87=({ struct Cyc_List_List*
_temp88=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp88->hd=( void*)({ struct _tuple5* _temp89=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp89->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs1))->hd; _temp89->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp90=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp90[ 0]=({ struct Cyc_Absyn_VarType_struct _temp91; _temp91.tag= Cyc_Absyn_VarType;
_temp91.f1=( struct Cyc_Absyn_Tvar*) tvs0->hd; _temp91;}); _temp90;}); _temp89;});
_temp88->tl= _temp87; _temp88;});} return _temp87;} struct _tuple6{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_arr t, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tagged_arr _temp92= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( d0->name))->v); int _temp93= 1; if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp92, loc, msg)){ return 0;}{ int _temp96; void* _temp98;
struct _tuple3 _temp94= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc,
t, _temp92, loc, msg); _LL99: _temp98= _temp94.f1; goto _LL97; _LL97: _temp96=
_temp94.f2; goto _LL95; _LL95: if( ! _temp96){ _temp93= 0;} if( ! Cyc_Tcdecl_check_atts(
d0->attributes, d1->attributes, t, _temp92, loc, msg)){ _temp93= 0;}{ struct Cyc_Absyn_Structdecl*
d2;{ struct _tuple6 _temp101=({ struct _tuple6 _temp100; _temp100.f1= d0->fields;
_temp100.f2= d1->fields; _temp100;}); struct Cyc_Core_Opt* _temp109; struct Cyc_Core_Opt*
_temp111; struct Cyc_Core_Opt* _temp113; struct Cyc_Core_Opt _temp115; struct
Cyc_List_List* _temp116; struct Cyc_Core_Opt* _temp118; struct Cyc_Core_Opt
_temp120; struct Cyc_List_List* _temp121; _LL103: _LL110: _temp109= _temp101.f2;
if( _temp109 ==  0){ goto _LL104;} else{ goto _LL105;} _LL105: _LL112: _temp111=
_temp101.f1; if( _temp111 ==  0){ goto _LL106;} else{ goto _LL107;} _LL107:
_LL119: _temp118= _temp101.f1; if( _temp118 ==  0){ goto _LL102;} else{ _temp120=*
_temp118; _LL122: _temp121=( struct Cyc_List_List*) _temp120.v; goto _LL114;}
_LL114: _temp113= _temp101.f2; if( _temp113 ==  0){ goto _LL102;} else{ _temp115=*
_temp113; _LL117: _temp116=( struct Cyc_List_List*) _temp115.v; goto _LL108;}
_LL104: d2= d0; goto _LL102; _LL106: d2= d1; goto _LL102; _LL108: { struct Cyc_List_List*
_temp123= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); for( 0; _temp121 !=  0?
_temp116 !=  0: 0;( _temp121= _temp121->tl, _temp116= _temp116->tl)){ struct Cyc_Absyn_Structfield
_temp126; struct Cyc_List_List* _temp127; struct Cyc_Absyn_Exp* _temp129; void*
_temp131; struct Cyc_Absyn_Tqual _temp133; struct _tagged_arr* _temp135; struct
Cyc_Absyn_Structfield* _temp124=( struct Cyc_Absyn_Structfield*) _temp121->hd;
_temp126=* _temp124; _LL136: _temp135= _temp126.name; goto _LL134; _LL134:
_temp133= _temp126.tq; goto _LL132; _LL132: _temp131=( void*) _temp126.type;
goto _LL130; _LL130: _temp129= _temp126.width; goto _LL128; _LL128: _temp127=
_temp126.attributes; goto _LL125; _LL125: { struct Cyc_Absyn_Structfield
_temp139; struct Cyc_List_List* _temp140; struct Cyc_Absyn_Exp* _temp142; void*
_temp144; struct Cyc_Absyn_Tqual _temp146; struct _tagged_arr* _temp148; struct
Cyc_Absyn_Structfield* _temp137=( struct Cyc_Absyn_Structfield*) _temp116->hd;
_temp139=* _temp137; _LL149: _temp148= _temp139.name; goto _LL147; _LL147:
_temp146= _temp139.tq; goto _LL145; _LL145: _temp144=( void*) _temp139.type;
goto _LL143; _LL143: _temp142= _temp139.width; goto _LL141; _LL141: _temp140=
_temp139.attributes; goto _LL138; _LL138: if( Cyc_Std_zstrptrcmp( _temp135,
_temp148) !=  0){({ struct Cyc_Std_String_pa_struct _temp154; _temp154.tag= Cyc_Std_String_pa;
_temp154.f1=( struct _tagged_arr)* _temp148;{ struct Cyc_Std_String_pa_struct
_temp153; _temp153.tag= Cyc_Std_String_pa; _temp153.f1=( struct _tagged_arr)*
_temp135;{ struct Cyc_Std_String_pa_struct _temp152; _temp152.tag= Cyc_Std_String_pa;
_temp152.f1=( struct _tagged_arr) _temp92;{ struct Cyc_Std_String_pa_struct
_temp151; _temp151.tag= Cyc_Std_String_pa; _temp151.f1=( struct _tagged_arr) t;{
void* _temp150[ 4u]={& _temp151,& _temp152,& _temp153,& _temp154}; Cyc_Tcdecl_merr(
loc, msg, _tag_arr("%s %s : field name mismatch %s != %s", sizeof( unsigned char),
37u), _tag_arr( _temp150, sizeof( void*), 4u));}}}}}); return 0;} if( ! Cyc_Tcutil_same_atts(
_temp127, _temp140)){({ struct Cyc_Std_String_pa_struct _temp158; _temp158.tag=
Cyc_Std_String_pa; _temp158.f1=( struct _tagged_arr)* _temp135;{ struct Cyc_Std_String_pa_struct
_temp157; _temp157.tag= Cyc_Std_String_pa; _temp157.f1=( struct _tagged_arr)
_temp92;{ struct Cyc_Std_String_pa_struct _temp156; _temp156.tag= Cyc_Std_String_pa;
_temp156.f1=( struct _tagged_arr) t;{ void* _temp155[ 3u]={& _temp156,& _temp157,&
_temp158}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s : attribute mismatch on field %s",
sizeof( unsigned char), 39u), _tag_arr( _temp155, sizeof( void*), 3u));}}}});
_temp93= 0;} if( ! Cyc_Tcutil_equal_tqual( _temp133, _temp146)){({ struct Cyc_Std_String_pa_struct
_temp162; _temp162.tag= Cyc_Std_String_pa; _temp162.f1=( struct _tagged_arr)*
_temp135;{ struct Cyc_Std_String_pa_struct _temp161; _temp161.tag= Cyc_Std_String_pa;
_temp161.f1=( struct _tagged_arr) _temp92;{ struct Cyc_Std_String_pa_struct
_temp160; _temp160.tag= Cyc_Std_String_pa; _temp160.f1=( struct _tagged_arr) t;{
void* _temp159[ 3u]={& _temp160,& _temp161,& _temp162}; Cyc_Tcdecl_merr( loc,
msg, _tag_arr("%s %s : qualifier mismatch on field %s", sizeof( unsigned char),
39u), _tag_arr( _temp159, sizeof( void*), 3u));}}}}); _temp93= 0;} if((((
_temp129 !=  0? _temp142 !=  0: 0)? Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp129)) !=  Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp142)): 0)? 1:( _temp129 ==  0? _temp142 !=  0: 0))? 1:(
_temp129 !=  0? _temp142 ==  0: 0)){({ struct Cyc_Std_String_pa_struct _temp166;
_temp166.tag= Cyc_Std_String_pa; _temp166.f1=( struct _tagged_arr)* _temp135;{
struct Cyc_Std_String_pa_struct _temp165; _temp165.tag= Cyc_Std_String_pa;
_temp165.f1=( struct _tagged_arr) _temp92;{ struct Cyc_Std_String_pa_struct
_temp164; _temp164.tag= Cyc_Std_String_pa; _temp164.f1=( struct _tagged_arr) t;{
void* _temp163[ 3u]={& _temp164,& _temp165,& _temp166}; Cyc_Tcdecl_merr( loc,
msg, _tag_arr("%s %s : bitfield mismatch on field %s", sizeof( unsigned char),
38u), _tag_arr( _temp163, sizeof( void*), 3u));}}}}); _temp93= 0;}{ void*
subst_t1= Cyc_Tcutil_substitute( _temp123, _temp144); if( ! Cyc_Tcdecl_check_type(
_temp131, subst_t1)){({ struct Cyc_Std_String_pa_struct _temp172; _temp172.tag=
Cyc_Std_String_pa; _temp172.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
subst_t1);{ struct Cyc_Std_String_pa_struct _temp171; _temp171.tag= Cyc_Std_String_pa;
_temp171.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp131);{ struct Cyc_Std_String_pa_struct
_temp170; _temp170.tag= Cyc_Std_String_pa; _temp170.f1=( struct _tagged_arr)*
_temp135;{ struct Cyc_Std_String_pa_struct _temp169; _temp169.tag= Cyc_Std_String_pa;
_temp169.f1=( struct _tagged_arr) _temp92;{ struct Cyc_Std_String_pa_struct
_temp168; _temp168.tag= Cyc_Std_String_pa; _temp168.f1=( struct _tagged_arr) t;{
void* _temp167[ 5u]={& _temp168,& _temp169,& _temp170,& _temp171,& _temp172};
Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s : type mismatch on field %s: %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp167, sizeof( void*), 5u));}}}}}});
Cyc_Tcutil_explain_failure(); _temp93= 0;}}}} if( _temp121 !=  0){({ struct Cyc_Std_String_pa_struct
_temp176; _temp176.tag= Cyc_Std_String_pa; _temp176.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Structfield*) _temp121->hd)->name;{ struct Cyc_Std_String_pa_struct
_temp175; _temp175.tag= Cyc_Std_String_pa; _temp175.f1=( struct _tagged_arr)
_temp92;{ struct Cyc_Std_String_pa_struct _temp174; _temp174.tag= Cyc_Std_String_pa;
_temp174.f1=( struct _tagged_arr) t;{ void* _temp173[ 3u]={& _temp174,& _temp175,&
_temp176}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s is missing field %s",
sizeof( unsigned char), 26u), _tag_arr( _temp173, sizeof( void*), 3u));}}}});
_temp93= 0;} if( _temp116 !=  0){({ struct Cyc_Std_String_pa_struct _temp180;
_temp180.tag= Cyc_Std_String_pa; _temp180.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)
_temp116->hd)->name;{ struct Cyc_Std_String_pa_struct _temp179; _temp179.tag=
Cyc_Std_String_pa; _temp179.f1=( struct _tagged_arr) _temp92;{ struct Cyc_Std_String_pa_struct
_temp178; _temp178.tag= Cyc_Std_String_pa; _temp178.f1=( struct _tagged_arr) t;{
void* _temp177[ 3u]={& _temp178,& _temp179,& _temp180}; Cyc_Tcdecl_merr( loc,
msg, _tag_arr("%s %s has extra field %s", sizeof( unsigned char), 25u), _tag_arr(
_temp177, sizeof( void*), 3u));}}}}); _temp93= 0;} d2= d0; goto _LL102;} _LL102:;}
if( ! _temp93){ return 0;} if( _temp98 == ( void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*)
d2;} else{ d2=({ struct Cyc_Absyn_Structdecl* _temp181=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp181[ 0]=* d2; _temp181;});(
void*)( d2->sc=( void*) _temp98); return( struct Cyc_Absyn_Structdecl*) d2;}}}}
struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ return Cyc_Tcdecl_merge_struct_or_union_decl( d0, d1,
_tag_arr("struct", sizeof( unsigned char), 7u), loc, msg);} struct Cyc_Absyn_Uniondecl*
Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ return( struct
Cyc_Absyn_Uniondecl*) Cyc_Tcdecl_merge_struct_or_union_decl(( struct Cyc_Absyn_Structdecl*)
d0,( struct Cyc_Absyn_Structdecl*) d1, _tag_arr("union", sizeof( unsigned char),
6u), loc, msg);} inline static struct _tagged_arr Cyc_Tcdecl_is_x2string( int
is_x){ return is_x? _tag_arr("xtunion", sizeof( unsigned char), 8u): _tag_arr("tunion",
sizeof( unsigned char), 7u);} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_merge_tunionfield(
struct Cyc_Absyn_Tunionfield* f0, struct Cyc_Absyn_Tunionfield* f1, struct Cyc_List_List*
inst, struct _tagged_arr t, struct _tagged_arr v, struct _tagged_arr* msg){
struct Cyc_Position_Segment* loc= f1->loc; if( Cyc_Std_zstrptrcmp((* f0->name).f2,(*
f1->name).f2) !=  0){({ struct Cyc_Std_String_pa_struct _temp186; _temp186.tag=
Cyc_Std_String_pa; _temp186.f1=( struct _tagged_arr)*(* f0->name).f2;{ struct
Cyc_Std_String_pa_struct _temp185; _temp185.tag= Cyc_Std_String_pa; _temp185.f1=(
struct _tagged_arr)*(* f1->name).f2;{ struct Cyc_Std_String_pa_struct _temp184;
_temp184.tag= Cyc_Std_String_pa; _temp184.f1=( struct _tagged_arr) v;{ struct
Cyc_Std_String_pa_struct _temp183; _temp183.tag= Cyc_Std_String_pa; _temp183.f1=(
struct _tagged_arr) t;{ void* _temp182[ 4u]={& _temp183,& _temp184,& _temp185,&
_temp186}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s: field name mismatch %s != %s",
sizeof( unsigned char), 36u), _tag_arr( _temp182, sizeof( void*), 4u));}}}}});
return 0;}{ struct _tagged_arr _temp187=*(* f0->name).f2; int _temp193; void*
_temp195; struct _tuple3 _temp191= Cyc_Tcdecl_merge_scope(( void*) f0->sc,( void*)
f1->sc,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp190;
_temp190.tag= Cyc_Std_String_pa; _temp190.f1=( struct _tagged_arr) v;{ struct
Cyc_Std_String_pa_struct _temp189; _temp189.tag= Cyc_Std_String_pa; _temp189.f1=(
struct _tagged_arr) t;{ void* _temp188[ 2u]={& _temp189,& _temp190}; Cyc_Std_aprintf(
_tag_arr("in %s %s, field", sizeof( unsigned char), 16u), _tag_arr( _temp188,
sizeof( void*), 2u));}}}), _temp187, loc, msg); _LL196: _temp195= _temp191.f1;
goto _LL194; _LL194: _temp193= _temp191.f2; goto _LL192; _LL192: { struct Cyc_List_List*
_temp197= f0->tvs; struct Cyc_List_List* _temp198= f1->tvs; if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp197) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp198)){({ struct Cyc_Std_String_pa_struct _temp202;
_temp202.tag= Cyc_Std_String_pa; _temp202.f1=( struct _tagged_arr) _temp187;{
struct Cyc_Std_String_pa_struct _temp201; _temp201.tag= Cyc_Std_String_pa;
_temp201.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp200;
_temp200.tag= Cyc_Std_String_pa; _temp200.f1=( struct _tagged_arr) t;{ void*
_temp199[ 3u]={& _temp200,& _temp201,& _temp202}; Cyc_Tcdecl_merr( loc, msg,
_tag_arr("%s %s, field %s: type parameter number mismatch", sizeof(
unsigned char), 48u), _tag_arr( _temp199, sizeof( void*), 3u));}}}}); return 0;}{
struct Cyc_List_List* _temp203= Cyc_Tcdecl_build_tvs_map( _temp197, _temp198);
struct Cyc_List_List* _temp204= f0->typs; struct Cyc_List_List* _temp205= f1->typs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp204) != (( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp205)){({ struct Cyc_Std_String_pa_struct
_temp209; _temp209.tag= Cyc_Std_String_pa; _temp209.f1=( struct _tagged_arr)
_temp187;{ struct Cyc_Std_String_pa_struct _temp208; _temp208.tag= Cyc_Std_String_pa;
_temp208.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp207;
_temp207.tag= Cyc_Std_String_pa; _temp207.f1=( struct _tagged_arr) t;{ void*
_temp206[ 3u]={& _temp207,& _temp208,& _temp209}; Cyc_Tcdecl_merr( loc, msg,
_tag_arr("%s %s, field %s: parameter number mismatch", sizeof( unsigned char),
43u), _tag_arr( _temp206, sizeof( void*), 3u));}}}}); _temp193= 0;} for( 0;
_temp204 !=  0;( _temp204= _temp204->tl, _temp205= _temp205->tl)){ if( ! Cyc_Tcutil_equal_tqual((*((
struct _tuple7*) _temp204->hd)).f1,(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp205))->hd)).f1)){({ struct Cyc_Std_String_pa_struct _temp213;
_temp213.tag= Cyc_Std_String_pa; _temp213.f1=( struct _tagged_arr) _temp187;{
struct Cyc_Std_String_pa_struct _temp212; _temp212.tag= Cyc_Std_String_pa;
_temp212.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp211;
_temp211.tag= Cyc_Std_String_pa; _temp211.f1=( struct _tagged_arr) t;{ void*
_temp210[ 3u]={& _temp211,& _temp212,& _temp213}; Cyc_Tcdecl_merr( loc, msg,
_tag_arr("%s %s, field %s: parameter qualifier", sizeof( unsigned char), 37u),
_tag_arr( _temp210, sizeof( void*), 3u));}}}}); _temp193= 0;}{ void* subst_t1=
Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp203, inst),(*(( struct _tuple7*)
_temp205->hd)).f2); if( ! Cyc_Tcdecl_check_type((*(( struct _tuple7*) _temp204->hd)).f2,
subst_t1)){({ struct Cyc_Std_String_pa_struct _temp219; _temp219.tag= Cyc_Std_String_pa;
_temp219.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( subst_t1);{ struct Cyc_Std_String_pa_struct
_temp218; _temp218.tag= Cyc_Std_String_pa; _temp218.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((*((
struct _tuple7*) _temp204->hd)).f2);{ struct Cyc_Std_String_pa_struct _temp217;
_temp217.tag= Cyc_Std_String_pa; _temp217.f1=( struct _tagged_arr) _temp187;{
struct Cyc_Std_String_pa_struct _temp216; _temp216.tag= Cyc_Std_String_pa;
_temp216.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp215;
_temp215.tag= Cyc_Std_String_pa; _temp215.f1=( struct _tagged_arr) t;{ void*
_temp214[ 5u]={& _temp215,& _temp216,& _temp217,& _temp218,& _temp219}; Cyc_Tcdecl_merr(
loc, msg, _tag_arr("%s %s, field %s: parameter type mismatch %s != %s", sizeof(
unsigned char), 50u), _tag_arr( _temp214, sizeof( void*), 5u));}}}}}}); Cyc_Tcutil_explain_failure();
_temp193= 0;}}} if( ! _temp193){ return 0;} if(( void*) f0->sc !=  _temp195){
struct Cyc_Absyn_Tunionfield* _temp220=({ struct Cyc_Absyn_Tunionfield* _temp221=(
struct Cyc_Absyn_Tunionfield*) _cycalloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp221[ 0]=* f0; _temp221;});( void*)((( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp220))->sc=( void*) _temp195); return _temp220;} else{ return(
struct Cyc_Absyn_Tunionfield*) f0;}}}}} static int Cyc_Tcdecl_substitute_tunionfield_f1(
struct _tagged_arr* name, struct Cyc_Absyn_Tvar* x){ return Cyc_Std_strptrcmp( x->name,
name) ==  0;} static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2(
struct Cyc_List_List* inst, struct _tuple7* x){ struct _tuple7 _temp224; void*
_temp225; struct Cyc_Absyn_Tqual _temp227; struct _tuple7* _temp222= x; _temp224=*
_temp222; _LL228: _temp227= _temp224.f1; goto _LL226; _LL226: _temp225= _temp224.f2;
goto _LL223; _LL223: return({ struct _tuple7* _temp229=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7)); _temp229->f1= _temp227; _temp229->f2= Cyc_Tcutil_substitute(
inst, _temp225); _temp229;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp230=({
struct Cyc_Absyn_Tunionfield* _temp240=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp240[ 0]=* f1; _temp240;});
struct Cyc_List_List* _temp231= 0;{ struct Cyc_List_List* _temp232= f1->tvs;
for( 0; _temp232 !=  0; _temp232= _temp232->tl){ struct Cyc_Absyn_Tvar* tv=(
struct Cyc_Absyn_Tvar*) _temp232->hd; if((( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct Cyc_List_List*
x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)?
1:(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct
_tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)){ int i= 0; struct _tagged_arr base=* tv->name; struct
_tagged_arr* new_name; do { new_name=({ struct _tagged_arr* _temp233=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp233[ 0]=( struct
_tagged_arr) Cyc_Std_strconcat( base,( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp235; _temp235.tag= Cyc_Std_Int_pa; _temp235.f1=( int)(( unsigned int) i);{
void* _temp234[ 1u]={& _temp235}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp234, sizeof( void*), 1u));}})); _temp233;});
i ++;} while ((( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp236=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp236[ 0]=* tv; _temp236;}); tv->name=
new_name;} _temp231=({ struct Cyc_List_List* _temp237=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp237->hd=( void*) tv; _temp237->tl=
_temp231; _temp237;});}} _temp231=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp231); _temp230->tvs= _temp231;{ struct Cyc_List_List*
_temp238= Cyc_Tcdecl_build_tvs_map( _temp231, f1->tvs); struct Cyc_List_List*
_temp239=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp238, inst1); _temp230->typs=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_List_List*, struct _tuple7*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2,
_temp239, f1->typs); return _temp230;}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp249=( struct Cyc_List_List**) _cycalloc( sizeof( struct Cyc_List_List*));
_temp249[ 0]= 0; _temp249;}); struct Cyc_List_List** _temp241= f2sp; int cmp= -
1; for( 0; f0s !=  0? f1s !=  0: 0; f1s= f1s->tl){ while( f0s !=  0?( cmp= Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*) f0s->hd)->name,(( struct Cyc_Absyn_Tunionfield*)
f1s->hd)->name)) <  0: 0) { struct Cyc_List_List* _temp242=({ struct Cyc_List_List*
_temp243=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp243->hd=( void*)(( struct Cyc_Absyn_Tunionfield*) f0s->hd); _temp243->tl= 0;
_temp243;});* _temp241= _temp242; _temp241=&(( struct Cyc_List_List*)
_check_null( _temp242))->tl; f0s= f0s->tl;} if( f0s ==  0? 1: cmp >  0){* incl=
0;{ struct Cyc_List_List* _temp244=({ struct Cyc_List_List* _temp245=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp245->hd=( void*)
Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,( struct Cyc_Absyn_Tunionfield*)
f1s->hd); _temp245->tl= 0; _temp245;});* _temp241= _temp244; _temp241=&(( struct
Cyc_List_List*) _check_null( _temp244))->tl;}} else{ struct Cyc_Absyn_Tunionfield*
_temp246= Cyc_Tcdecl_merge_tunionfield(( struct Cyc_Absyn_Tunionfield*) f0s->hd,(
struct Cyc_Absyn_Tunionfield*) f1s->hd, inst, t, v, msg); if( _temp246 !=  0){
if( _temp246 != ( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)
f0s->hd)){* incl= 0;}{ struct Cyc_List_List* _temp247=({ struct Cyc_List_List*
_temp248=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp248->hd=( void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp246));
_temp248->tl= 0; _temp248;});* _temp241= _temp247; _temp241=&(( struct Cyc_List_List*)
_check_null( _temp247))->tl;}} else{* res= 0;} f0s= f0s->tl;}} if( f1s !=  0){*
incl= 0;* _temp241= f1s;} else{* _temp241= f0s;} return* f2sp;} struct _tuple8{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct _tuple8
Cyc_Tcdecl_split( struct Cyc_List_List* f){ if( f ==  0){ return({ struct
_tuple8 _temp250; _temp250.f1= 0; _temp250.f2= 0; _temp250;});} if( f->tl ==  0){
return({ struct _tuple8 _temp251; _temp251.f1= f; _temp251.f2= 0; _temp251;});}{
struct Cyc_List_List* _temp254; struct Cyc_List_List* _temp256; struct _tuple8
_temp252= Cyc_Tcdecl_split((( struct Cyc_List_List*) _check_null( f->tl))->tl);
_LL257: _temp256= _temp252.f1; goto _LL255; _LL255: _temp254= _temp252.f2; goto
_LL253; _LL253: return({ struct _tuple8 _temp258; _temp258.f1=({ struct Cyc_List_List*
_temp260=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp260->hd=( void*)(( void*) f->hd); _temp260->tl= _temp256; _temp260;});
_temp258.f2=({ struct Cyc_List_List* _temp259=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp259->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( f->tl))->hd); _temp259->tl= _temp254; _temp259;}); _temp258;});}}
struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f,
int* res, struct _tagged_arr* v, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ struct Cyc_List_List* _temp263; struct Cyc_List_List*
_temp265; struct _tuple8 _temp261=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL266: _temp265= _temp261.f1; goto _LL264; _LL264:
_temp263= _temp261.f2; goto _LL262; _LL262: if( _temp265 !=  0? _temp265->tl != 
0: 0){ _temp265= Cyc_Tcdecl_sort_xtunion_fields( _temp265, res, v, loc, msg);}
if( _temp263 !=  0? _temp263->tl !=  0: 0){ _temp263= Cyc_Tcdecl_sort_xtunion_fields(
_temp263, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp265,
_temp263, 0, 0, 0, res,({ int* _temp267=( int*) _cycalloc_atomic( sizeof( int));
_temp267[ 0]= 1; _temp267;}), _tag_arr("xtunion", sizeof( unsigned char), 8u),*
v, loc, msg);} struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl( struct
Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp268= Cyc_Absynpp_qvar2string(
d0->name); struct _tagged_arr t= _tag_arr("[x]tunion", sizeof( unsigned char),
10u); int _temp269= 1; if( d0->is_xtunion !=  d1->is_xtunion){({ struct Cyc_Std_String_pa_struct
_temp273; _temp273.tag= Cyc_Std_String_pa; _temp273.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string(
d1->is_xtunion);{ struct Cyc_Std_String_pa_struct _temp272; _temp272.tag= Cyc_Std_String_pa;
_temp272.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string( d0->is_xtunion);{
struct Cyc_Std_String_pa_struct _temp271; _temp271.tag= Cyc_Std_String_pa;
_temp271.f1=( struct _tagged_arr) _temp268;{ void* _temp270[ 3u]={& _temp271,&
_temp272,& _temp273}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("expected %s to be a %s instead of a %s",
sizeof( unsigned char), 39u), _tag_arr( _temp270, sizeof( void*), 3u));}}}});
_temp269= 0;} else{ t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp268, loc, msg)){ return 0;}{ int _temp276; void*
_temp278; struct _tuple3 _temp274= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*)
d1->sc, t, _temp268, loc, msg); _LL279: _temp278= _temp274.f1; goto _LL277;
_LL277: _temp276= _temp274.f2; goto _LL275; _LL275: if( ! _temp276){ _temp269= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp281=({ struct _tuple6
_temp280; _temp280.f1= d0->fields; _temp280.f2= d1->fields; _temp280;}); struct
Cyc_Core_Opt* _temp289; struct Cyc_Core_Opt* _temp291; struct Cyc_Core_Opt*
_temp293; struct Cyc_Core_Opt _temp295; struct Cyc_List_List* _temp296; struct
Cyc_Core_Opt* _temp298; struct Cyc_Core_Opt _temp300; struct Cyc_List_List*
_temp301; _LL283: _LL290: _temp289= _temp281.f2; if( _temp289 ==  0){ goto
_LL284;} else{ goto _LL285;} _LL285: _LL292: _temp291= _temp281.f1; if( _temp291
==  0){ goto _LL286;} else{ goto _LL287;} _LL287: _LL299: _temp298= _temp281.f1;
if( _temp298 ==  0){ goto _LL282;} else{ _temp300=* _temp298; _LL302: _temp301=(
struct Cyc_List_List*) _temp300.v; goto _LL294;} _LL294: _temp293= _temp281.f2;
if( _temp293 ==  0){ goto _LL282;} else{ _temp295=* _temp293; _LL297: _temp296=(
struct Cyc_List_List*) _temp295.v; goto _LL288;} _LL284: d2= d0; goto _LL282;
_LL286: d2= d1; goto _LL282; _LL288: { struct Cyc_List_List* _temp303= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); if( d0->is_xtunion){ int _temp304= 1; struct Cyc_List_List*
_temp305= Cyc_Tcdecl_merge_xtunion_fields( _temp301, _temp296, _temp303, d0->tvs,
d1->tvs,& _temp269,& _temp304, t, _temp268, loc, msg); if( _temp304){ d2= d0;}
else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp306=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp306[ 0]=* d0; _temp306;});(
void*)( d2->sc=( void*) _temp278); d2->fields=({ struct Cyc_Core_Opt* _temp307=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp307->v=(
void*) _temp305; _temp307;});}} else{ for( 0; _temp301 !=  0? _temp296 !=  0: 0;(
_temp301= _temp301->tl, _temp296= _temp296->tl)){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*) _temp301->hd,( struct Cyc_Absyn_Tunionfield*)
_temp296->hd, _temp303, t, _temp268, msg);} if( _temp301 !=  0){({ struct Cyc_Std_String_pa_struct
_temp311; _temp311.tag= Cyc_Std_String_pa; _temp311.f1=( struct _tagged_arr)*(*((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp296))->hd)->name).f2;{
struct Cyc_Std_String_pa_struct _temp310; _temp310.tag= Cyc_Std_String_pa;
_temp310.f1=( struct _tagged_arr) _temp268;{ struct Cyc_Std_String_pa_struct
_temp309; _temp309.tag= Cyc_Std_String_pa; _temp309.f1=( struct _tagged_arr) t;{
void* _temp308[ 3u]={& _temp309,& _temp310,& _temp311}; Cyc_Tcdecl_merr( loc,
msg, _tag_arr("%s %s has extra field %s", sizeof( unsigned char), 25u), _tag_arr(
_temp308, sizeof( void*), 3u));}}}}); _temp269= 0;} if( _temp296 !=  0){({
struct Cyc_Std_String_pa_struct _temp315; _temp315.tag= Cyc_Std_String_pa;
_temp315.f1=( struct _tagged_arr)*(*(( struct Cyc_Absyn_Tunionfield*) _temp296->hd)->name).f2;{
struct Cyc_Std_String_pa_struct _temp314; _temp314.tag= Cyc_Std_String_pa;
_temp314.f1=( struct _tagged_arr) _temp268;{ struct Cyc_Std_String_pa_struct
_temp313; _temp313.tag= Cyc_Std_String_pa; _temp313.f1=( struct _tagged_arr) t;{
void* _temp312[ 3u]={& _temp313,& _temp314,& _temp315}; Cyc_Tcdecl_merr( loc,
msg, _tag_arr("%s %s is missing field %s", sizeof( unsigned char), 26u),
_tag_arr( _temp312, sizeof( void*), 3u));}}}}); _temp269= 0;} d2= d0;} goto
_LL282;} _LL282:;} if( ! _temp269){ return 0;} if( _temp278 == ( void*) d2->sc){
return( struct Cyc_Absyn_Tuniondecl*) d2;} else{ d2=({ struct Cyc_Absyn_Tuniondecl*
_temp316=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp316[ 0]=* d2; _temp316;});( void*)( d2->sc=( void*) _temp278); return(
struct Cyc_Absyn_Tuniondecl*) d2;}}}} struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp317= Cyc_Absynpp_qvar2string(
d0->name); int _temp318= 1; int _temp321; void* _temp323; struct _tuple3
_temp319= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc, _tag_arr("enum",
sizeof( unsigned char), 5u), _temp317, loc, msg); _LL324: _temp323= _temp319.f1;
goto _LL322; _LL322: _temp321= _temp319.f2; goto _LL320; _LL320: if( ! _temp321){
_temp318= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{ struct _tuple6 _temp326=({ struct
_tuple6 _temp325; _temp325.f1= d0->fields; _temp325.f2= d1->fields; _temp325;});
struct Cyc_Core_Opt* _temp334; struct Cyc_Core_Opt* _temp336; struct Cyc_Core_Opt*
_temp338; struct Cyc_Core_Opt _temp340; struct Cyc_List_List* _temp341; struct
Cyc_Core_Opt* _temp343; struct Cyc_Core_Opt _temp345; struct Cyc_List_List*
_temp346; _LL328: _LL335: _temp334= _temp326.f2; if( _temp334 ==  0){ goto
_LL329;} else{ goto _LL330;} _LL330: _LL337: _temp336= _temp326.f1; if( _temp336
==  0){ goto _LL331;} else{ goto _LL332;} _LL332: _LL344: _temp343= _temp326.f1;
if( _temp343 ==  0){ goto _LL327;} else{ _temp345=* _temp343; _LL347: _temp346=(
struct Cyc_List_List*) _temp345.v; goto _LL339;} _LL339: _temp338= _temp326.f2;
if( _temp338 ==  0){ goto _LL327;} else{ _temp340=* _temp338; _LL342: _temp341=(
struct Cyc_List_List*) _temp340.v; goto _LL333;} _LL329: d2= d0; goto _LL327;
_LL331: d2= d1; goto _LL327; _LL333: for( 0; _temp346 !=  0? _temp341 !=  0: 0;(
_temp346= _temp346->tl, _temp341= _temp341->tl)){ struct Cyc_Absyn_Enumfield
_temp350; struct Cyc_Position_Segment* _temp351; struct Cyc_Absyn_Exp* _temp353;
struct _tuple0* _temp355; struct _tuple0 _temp357; struct _tagged_arr* _temp358;
struct Cyc_Absyn_Enumfield* _temp348=( struct Cyc_Absyn_Enumfield*) _temp346->hd;
_temp350=* _temp348; _LL356: _temp355= _temp350.name; _temp357=* _temp355;
_LL359: _temp358= _temp357.f2; goto _LL354; _LL354: _temp353= _temp350.tag; goto
_LL352; _LL352: _temp351= _temp350.loc; goto _LL349; _LL349: { struct Cyc_Absyn_Enumfield
_temp362; struct Cyc_Position_Segment* _temp363; struct Cyc_Absyn_Exp* _temp365;
struct _tuple0* _temp367; struct _tuple0 _temp369; struct _tagged_arr* _temp370;
struct Cyc_Absyn_Enumfield* _temp360=( struct Cyc_Absyn_Enumfield*) _temp341->hd;
_temp362=* _temp360; _LL368: _temp367= _temp362.name; _temp369=* _temp367;
_LL371: _temp370= _temp369.f2; goto _LL366; _LL366: _temp365= _temp362.tag; goto
_LL364; _LL364: _temp363= _temp362.loc; goto _LL361; _LL361: if( Cyc_Std_zstrptrcmp(
_temp358, _temp370) !=  0){({ struct Cyc_Std_String_pa_struct _temp375; _temp375.tag=
Cyc_Std_String_pa; _temp375.f1=( struct _tagged_arr)* _temp370;{ struct Cyc_Std_String_pa_struct
_temp374; _temp374.tag= Cyc_Std_String_pa; _temp374.f1=( struct _tagged_arr)*
_temp358;{ struct Cyc_Std_String_pa_struct _temp373; _temp373.tag= Cyc_Std_String_pa;
_temp373.f1=( struct _tagged_arr) _temp317;{ void* _temp372[ 3u]={& _temp373,&
_temp374,& _temp375}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("enum %s: field name mismatch %s != %s",
sizeof( unsigned char), 38u), _tag_arr( _temp372, sizeof( void*), 3u));}}}});
_temp318= 0;} if( Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp353)) !=  Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp365))){({ struct Cyc_Std_String_pa_struct _temp378; _temp378.tag=
Cyc_Std_String_pa; _temp378.f1=( struct _tagged_arr)* _temp370;{ struct Cyc_Std_String_pa_struct
_temp377; _temp377.tag= Cyc_Std_String_pa; _temp377.f1=( struct _tagged_arr)
_temp317;{ void* _temp376[ 2u]={& _temp377,& _temp378}; Cyc_Tcdecl_merr( loc,
msg, _tag_arr("enum %s, field %s, value mismatch", sizeof( unsigned char), 34u),
_tag_arr( _temp376, sizeof( void*), 2u));}}}); _temp318= 0;}}} d2= d0; goto
_LL327; _LL327:;} if( ! _temp318){ return 0;} if(( void*) d2->sc ==  _temp323){
return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Enumdecl*
_temp379=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp379[ 0]=* d2; _temp379;});( void*)( d2->sc=( void*) _temp323); return(
struct Cyc_Absyn_Enumdecl*) d2;}}} inline static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void* sc0, void* t0, struct Cyc_Absyn_Tqual tq0, struct Cyc_List_List* atts0,
void* sc1, void* t1, struct Cyc_Absyn_Tqual tq1, struct Cyc_List_List* atts1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ int _temp380= 1; int _temp383; void* _temp385; struct
_tuple3 _temp381= Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL386:
_temp385= _temp381.f1; goto _LL384; _LL384: _temp383= _temp381.f2; goto _LL382;
_LL382: if( ! _temp383){ _temp380= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){({
struct Cyc_Std_String_pa_struct _temp391; _temp391.tag= Cyc_Std_String_pa;
_temp391.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t0);{ struct Cyc_Std_String_pa_struct
_temp390; _temp390.tag= Cyc_Std_String_pa; _temp390.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ struct Cyc_Std_String_pa_struct _temp389; _temp389.tag= Cyc_Std_String_pa;
_temp389.f1=( struct _tagged_arr) v;{ struct Cyc_Std_String_pa_struct _temp388;
_temp388.tag= Cyc_Std_String_pa; _temp388.f1=( struct _tagged_arr) t;{ void*
_temp387[ 4u]={& _temp388,& _temp389,& _temp390,& _temp391}; Cyc_Tcdecl_merr(
loc, msg, _tag_arr("%s %s has type \n%s\n instead of \n%s\n", sizeof(
unsigned char), 36u), _tag_arr( _temp387, sizeof( void*), 4u));}}}}}); Cyc_Tcutil_explain_failure();
_temp380= 0;} if( ! Cyc_Tcutil_equal_tqual( tq0, tq1)){({ struct Cyc_Std_String_pa_struct
_temp394; _temp394.tag= Cyc_Std_String_pa; _temp394.f1=( struct _tagged_arr) v;{
struct Cyc_Std_String_pa_struct _temp393; _temp393.tag= Cyc_Std_String_pa;
_temp393.f1=( struct _tagged_arr) t;{ void* _temp392[ 2u]={& _temp393,& _temp394};
Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s has different type qualifiers",
sizeof( unsigned char), 36u), _tag_arr( _temp392, sizeof( void*), 2u));}}});
_temp380= 0;} if( ! Cyc_Tcutil_same_atts( atts0, atts1)){({ struct Cyc_Std_String_pa_struct
_temp397; _temp397.tag= Cyc_Std_String_pa; _temp397.f1=( struct _tagged_arr) v;{
struct Cyc_Std_String_pa_struct _temp396; _temp396.tag= Cyc_Std_String_pa;
_temp396.f1=( struct _tagged_arr) t;{ void* _temp395[ 2u]={& _temp396,& _temp397};
Cyc_Tcdecl_merr( loc, msg, _tag_arr("%s %s has different attributes", sizeof(
unsigned char), 31u), _tag_arr( _temp395, sizeof( void*), 2u));}}});({ void*
_temp398[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("previous attributes: ",
sizeof( unsigned char), 22u), _tag_arr( _temp398, sizeof( void*), 0u));}); for(
0; atts0 !=  0; atts0= atts0->tl){({ struct Cyc_Std_String_pa_struct _temp400;
_temp400.tag= Cyc_Std_String_pa; _temp400.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts0->hd);{ void* _temp399[ 1u]={& _temp400}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s ", sizeof( unsigned char), 4u), _tag_arr( _temp399, sizeof( void*),
1u));}});}({ void* _temp401[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\ncurrent attributes: ",
sizeof( unsigned char), 22u), _tag_arr( _temp401, sizeof( void*), 0u));}); for(
0; atts1 !=  0; atts1= atts1->tl){({ struct Cyc_Std_String_pa_struct _temp403;
_temp403.tag= Cyc_Std_String_pa; _temp403.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts1->hd);{ void* _temp402[ 1u]={& _temp403}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s ", sizeof( unsigned char), 4u), _tag_arr( _temp402, sizeof( void*),
1u));}});}({ void* _temp404[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n",
sizeof( unsigned char), 2u), _tag_arr( _temp404, sizeof( void*), 0u));});
_temp380= 0;} return({ struct _tuple3 _temp405; _temp405.f1= _temp385; _temp405.f2=
_temp380; _temp405;});} struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp406= Cyc_Absynpp_qvar2string(
d0->name); int _temp409; void* _temp411; struct _tuple3 _temp407= Cyc_Tcdecl_check_var_or_fn_decl((
void*) d0->sc,( void*) d0->type, d0->tq, d0->attributes,( void*) d1->sc,( void*)
d1->type, d1->tq, d1->attributes, _tag_arr("variable", sizeof( unsigned char), 9u),
_temp406, loc, msg); _LL412: _temp411= _temp407.f1; goto _LL410; _LL410:
_temp409= _temp407.f2; goto _LL408; _LL408: if( ! _temp409){ return 0;} if((
void*) d0->sc ==  _temp411){ return( struct Cyc_Absyn_Vardecl*) d0;} else{
struct Cyc_Absyn_Vardecl* _temp413=({ struct Cyc_Absyn_Vardecl* _temp414=(
struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp414[ 0]=* d0; _temp414;});( void*)((( struct Cyc_Absyn_Vardecl*)
_check_null( _temp413))->sc=( void*) _temp411); return _temp413;}} struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ struct
_tagged_arr _temp415= Cyc_Absynpp_qvar2string( d0->name); if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, _tag_arr("typedef", sizeof( unsigned char), 8u), _temp415, loc,
msg)){ return 0;}{ struct Cyc_List_List* _temp416= Cyc_Tcdecl_build_tvs_map( d0->tvs,
d1->tvs); void* subst_defn1= Cyc_Tcutil_substitute( _temp416,( void*) d1->defn);
if( ! Cyc_Tcdecl_check_type(( void*) d0->defn, subst_defn1)){({ struct Cyc_Std_String_pa_struct
_temp420; _temp420.tag= Cyc_Std_String_pa; _temp420.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*) d0->defn);{ struct Cyc_Std_String_pa_struct _temp419; _temp419.tag= Cyc_Std_String_pa;
_temp419.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( subst_defn1);{ struct
Cyc_Std_String_pa_struct _temp418; _temp418.tag= Cyc_Std_String_pa; _temp418.f1=(
struct _tagged_arr) _temp415;{ void* _temp417[ 3u]={& _temp418,& _temp419,&
_temp420}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("typedef %s does not refer to the same type: %s != %s",
sizeof( unsigned char), 53u), _tag_arr( _temp417, sizeof( void*), 3u));}}}});
return 0;} return( struct Cyc_Absyn_Typedefdecl*) d0;}} void* Cyc_Tcdecl_merge_binding(
void* b0, void* b1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tuple4 _temp422=({ struct _tuple4 _temp421; _temp421.f1= b0; _temp421.f2=
b1; _temp421;}); void* _temp436; void* _temp438; void* _temp440; struct Cyc_Absyn_Vardecl*
_temp442; void* _temp444; struct Cyc_Absyn_Vardecl* _temp446; void* _temp448;
struct Cyc_Absyn_Fndecl* _temp450; void* _temp452; struct Cyc_Absyn_Vardecl*
_temp454; void* _temp456; struct Cyc_Absyn_Fndecl* _temp458; void* _temp460;
void* _temp462; struct Cyc_Absyn_Vardecl* _temp464; void* _temp466; struct Cyc_Absyn_Fndecl*
_temp468; _LL424: _LL439: _temp438= _temp422.f1; if( _temp438 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL437;} else{ goto _LL426;} _LL437: _temp436= _temp422.f2; if( _temp436 == (
void*) Cyc_Absyn_Unresolved_b){ goto _LL425;} else{ goto _LL426;} _LL426: _LL445:
_temp444= _temp422.f1; if(( unsigned int) _temp444 >  1u?*(( int*) _temp444) == 
Cyc_Absyn_Global_b: 0){ _LL447: _temp446=(( struct Cyc_Absyn_Global_b_struct*)
_temp444)->f1; goto _LL441;} else{ goto _LL428;} _LL441: _temp440= _temp422.f2;
if(( unsigned int) _temp440 >  1u?*(( int*) _temp440) ==  Cyc_Absyn_Global_b: 0){
_LL443: _temp442=(( struct Cyc_Absyn_Global_b_struct*) _temp440)->f1; goto
_LL427;} else{ goto _LL428;} _LL428: _LL453: _temp452= _temp422.f1; if((
unsigned int) _temp452 >  1u?*(( int*) _temp452) ==  Cyc_Absyn_Global_b: 0){
_LL455: _temp454=(( struct Cyc_Absyn_Global_b_struct*) _temp452)->f1; goto
_LL449;} else{ goto _LL430;} _LL449: _temp448= _temp422.f2; if(( unsigned int)
_temp448 >  1u?*(( int*) _temp448) ==  Cyc_Absyn_Funname_b: 0){ _LL451: _temp450=((
struct Cyc_Absyn_Funname_b_struct*) _temp448)->f1; goto _LL429;} else{ goto
_LL430;} _LL430: _LL461: _temp460= _temp422.f1; if(( unsigned int) _temp460 >  1u?*((
int*) _temp460) ==  Cyc_Absyn_Funname_b: 0){ goto _LL457;} else{ goto _LL432;}
_LL457: _temp456= _temp422.f2; if(( unsigned int) _temp456 >  1u?*(( int*)
_temp456) ==  Cyc_Absyn_Funname_b: 0){ _LL459: _temp458=(( struct Cyc_Absyn_Funname_b_struct*)
_temp456)->f1; goto _LL431;} else{ goto _LL432;} _LL432: _LL467: _temp466=
_temp422.f1; if(( unsigned int) _temp466 >  1u?*(( int*) _temp466) ==  Cyc_Absyn_Funname_b:
0){ _LL469: _temp468=(( struct Cyc_Absyn_Funname_b_struct*) _temp466)->f1; goto
_LL463;} else{ goto _LL434;} _LL463: _temp462= _temp422.f2; if(( unsigned int)
_temp462 >  1u?*(( int*) _temp462) ==  Cyc_Absyn_Global_b: 0){ _LL465: _temp464=((
struct Cyc_Absyn_Global_b_struct*) _temp462)->f1; goto _LL433;} else{ goto
_LL434;} _LL434: goto _LL435; _LL425: return( void*) Cyc_Absyn_Unresolved_b;
_LL427: { struct Cyc_Absyn_Vardecl* _temp470= Cyc_Tcdecl_merge_vardecl( _temp446,
_temp442, loc, msg); if( _temp470 ==  0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp446 ==  _temp470){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp442 ==  _temp470){ return b1;} else{ return(
void*)({ struct Cyc_Absyn_Global_b_struct* _temp471=( struct Cyc_Absyn_Global_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp471[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp472; _temp472.tag= Cyc_Absyn_Global_b; _temp472.f1=(
struct Cyc_Absyn_Vardecl*) _check_null( _temp470); _temp472;}); _temp471;});}}}
_LL429: { int _temp475; struct _tuple3 _temp473= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp454->sc,( void*) _temp454->type, _temp454->tq, _temp454->attributes,(
void*) _temp450->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp450->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp450->attributes, _tag_arr("function", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp454->name), loc, msg); _LL476:
_temp475= _temp473.f2; goto _LL474; _LL474: return _temp475? b1:( void*) Cyc_Absyn_Unresolved_b;}
_LL431:({ struct Cyc_Std_String_pa_struct _temp478; _temp478.tag= Cyc_Std_String_pa;
_temp478.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp458->name);{
void* _temp477[ 1u]={& _temp478}; Cyc_Tcdecl_merr( loc, msg, _tag_arr("redefinition of function %s",
sizeof( unsigned char), 28u), _tag_arr( _temp477, sizeof( void*), 1u));}});
return( void*) Cyc_Absyn_Unresolved_b; _LL433: { int _temp481; struct _tuple3
_temp479= Cyc_Tcdecl_check_var_or_fn_decl(( void*) _temp468->sc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp468->cached_typ))->v, Cyc_Absyn_empty_tqual(),
_temp468->attributes,( void*) _temp464->sc,( void*) _temp464->type, _temp464->tq,
_temp464->attributes, _tag_arr("variable", sizeof( unsigned char), 9u), Cyc_Absynpp_qvar2string(
_temp468->name), loc, msg); _LL482: _temp481= _temp479.f2; goto _LL480; _LL480:
return _temp481? b0:( void*) Cyc_Absyn_Unresolved_b;} _LL435:( int) _throw((
void*)({ struct Cyc_Core_Invalid_argument_struct* _temp483=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp483[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp484; _temp484.tag= Cyc_Core_Invalid_argument;
_temp484.f1= _tag_arr("Tcdecl::merge_binding", sizeof( unsigned char), 22u);
_temp484;}); _temp483;})); _LL423:;} struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl
_temp487; struct Cyc_Absyn_Tunionfield* _temp488; struct Cyc_Absyn_Tuniondecl*
_temp490; struct Cyc_Tcdecl_Xtunionfielddecl* _temp485= d0; _temp487=* _temp485;
_LL491: _temp490= _temp487.base; goto _LL489; _LL489: _temp488= _temp487.field;
goto _LL486; _LL486: { struct Cyc_Tcdecl_Xtunionfielddecl _temp494; struct Cyc_Absyn_Tunionfield*
_temp495; struct Cyc_Absyn_Tuniondecl* _temp497; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp492= d1; _temp494=* _temp492; _LL498: _temp497= _temp494.base; goto _LL496;
_LL496: _temp495= _temp494.field; goto _LL493; _LL493: { struct _tagged_arr
_temp499= Cyc_Absynpp_qvar2string( _temp488->name); if( Cyc_Absyn_qvar_cmp(
_temp490->name, _temp497->name) !=  0){({ struct Cyc_Std_String_pa_struct
_temp503; _temp503.tag= Cyc_Std_String_pa; _temp503.f1=( struct _tagged_arr)
_temp499;{ struct Cyc_Std_String_pa_struct _temp502; _temp502.tag= Cyc_Std_String_pa;
_temp502.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp497->name);{
struct Cyc_Std_String_pa_struct _temp501; _temp501.tag= Cyc_Std_String_pa;
_temp501.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp490->name);{
void* _temp500[ 3u]={& _temp501,& _temp502,& _temp503}; Cyc_Tcdecl_merr( loc,
msg, _tag_arr("xtunions %s and %s have both a field named %s", sizeof(
unsigned char), 46u), _tag_arr( _temp500, sizeof( void*), 3u));}}}}); return 0;}
if( ! Cyc_Tcdecl_check_tvs( _temp490->tvs, _temp497->tvs, _tag_arr("xtunion",
sizeof( unsigned char), 8u), Cyc_Absynpp_qvar2string( _temp490->name), loc, msg)){
return 0;}{ struct Cyc_List_List* _temp504= Cyc_Tcdecl_build_tvs_map( _temp490->tvs,
_temp497->tvs); struct Cyc_Absyn_Tunionfield* _temp505= Cyc_Tcdecl_merge_tunionfield(
_temp488, _temp495, _temp504, _tag_arr("xtunionfield", sizeof( unsigned char),
13u), _temp499, msg); if( _temp505 ==  0){ return 0;} if( _temp505 == ( struct
Cyc_Absyn_Tunionfield*) _temp488){ return( struct Cyc_Tcdecl_Xtunionfielddecl*)
d0;} else{ return({ struct Cyc_Tcdecl_Xtunionfielddecl* _temp506=( struct Cyc_Tcdecl_Xtunionfielddecl*)
_cycalloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp506->base=
_temp490; _temp506->field=( struct Cyc_Absyn_Tunionfield*) _check_null( _temp505);
_temp506;});}}}}}
