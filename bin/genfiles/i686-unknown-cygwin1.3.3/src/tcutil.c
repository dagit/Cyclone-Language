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
extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_Invalid_argument[
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
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_fflush( struct
Cyc_Std___sFILE*); extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
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
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int
Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y); extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern void*
Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_exists_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct
Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct _RegionHandle* r2,
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_rsplit( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x); extern
int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l, void*
x); extern void* Cyc_List_assoc_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l, void* k); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List*); static const int Cyc_Position_Lex= 0; static const int
Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
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
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl** f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_AnonEnumType=
14; struct Cyc_Absyn_AnonEnumType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnHandleType= 15; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 16; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
17; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 18; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_RgnsEff= 19; struct Cyc_Absyn_RgnsEff_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
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
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 25; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
26; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 27; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 28; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 29;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 30; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple1* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
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
4; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
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
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s=
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
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_UnknownCall_p= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_sint_t; extern void*
Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_empty_effect; extern struct
_tuple1* Cyc_Absyn_tunion_print_arg_qvar; extern struct _tuple1* Cyc_Absyn_tunion_scanf_arg_qvar;
extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_atb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq, void* b); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct _tagged_arr*); extern
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*); struct _tuple4{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple4* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*);
extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k,
void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k);
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Tcenv_AnonEnumRes= 4; struct
Cyc_Tcenv_AnonEnumRes_struct{ int tag; void* f1; struct Cyc_Absyn_Enumfield* f2;
} ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict* tuniondecls;
struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables; } ; struct Cyc_Tcenv_Fenv; static
const int Cyc_Tcenv_NotLoop_j= 0; static const int Cyc_Tcenv_CaseEnd_j= 1;
static const int Cyc_Tcenv_FnEnd_j= 2; static const int Cyc_Tcenv_Stmt_j= 0;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct
Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*, void* r1, void* r2); extern int Cyc_Std_strcmp( struct
_tagged_arr s1, struct _tagged_arr s2); extern int Cyc_Std_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_copy_type(
void* t); extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern
void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress( void* t);
extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*); extern int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern
void Cyc_Tcutil_explain_failure(); extern int Cyc_Tcutil_unify( void*, void*);
extern int Cyc_Tcutil_typecmp( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*); struct _tuple6{ struct Cyc_List_List* f1; struct
_RegionHandle* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_bitfield( struct
Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct
Cyc_Absyn_Exp* width, struct _tagged_arr* fn); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_valid_toplevel_type( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr err_msg); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*, unsigned int i, struct Cyc_Absyn_Conref*); extern
int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*,
void* t, struct Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern
struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int* Cyc_Tcutil_new_tvar_id();
extern void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*); extern void
Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_snd_tqt(
struct _tuple4*); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct
Cyc_Position_Segment*, struct Cyc_Absyn_Structdecl*); extern void Cyc_Tc_tcUniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*,
struct Cyc_Absyn_Uniondecl*); extern void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*, struct Cyc_Absyn_Tuniondecl*);
extern void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*, struct Cyc_Absyn_Enumdecl*); unsigned char Cyc_Tcutil_TypeErr[
12u]="\000\000\000\000TypeErr"; extern void Cyc_Tcutil_unify_it( void* t1, void*
t2); void* Cyc_Tcutil_t1_failure=( void*) 0u; void* Cyc_Tcutil_t2_failure=( void*)
0u; struct _tagged_arr Cyc_Tcutil_failure_reason=( struct _tagged_arr){( void*)
0u,( void*) 0u,( void*)( 0u +  0u)}; void Cyc_Tcutil_explain_failure(){ Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);{ struct _tagged_arr s1= Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure); struct _tagged_arr s2= Cyc_Absynpp_typ2string( Cyc_Tcutil_t2_failure);
int pos= 8;({ struct Cyc_Std_String_pa_struct _temp1; _temp1.tag= Cyc_Std_String_pa;
_temp1.f1=( struct _tagged_arr) s1;{ void* _temp0[ 1u]={& _temp1}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\t%s and ", sizeof( unsigned char), 9u), _tag_arr(
_temp0, sizeof( void*), 1u));}}); pos += _get_arr_size( s1, sizeof(
unsigned char)) +  5; if( pos >=  80){({ void* _temp2[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n\t", sizeof( unsigned char), 3u), _tag_arr( _temp2,
sizeof( void*), 0u));}); pos= 8;}({ struct Cyc_Std_String_pa_struct _temp4;
_temp4.tag= Cyc_Std_String_pa; _temp4.f1=( struct _tagged_arr) s2;{ void* _temp3[
1u]={& _temp4}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s ", sizeof(
unsigned char), 4u), _tag_arr( _temp3, sizeof( void*), 1u));}}); pos +=
_get_arr_size( s2, sizeof( unsigned char)) +  1; if( pos >=  80){({ void* _temp5[
0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n\t", sizeof( unsigned char),
3u), _tag_arr( _temp5, sizeof( void*), 0u));}); pos= 8;}({ void* _temp6[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("failed to unify. ", sizeof(
unsigned char), 18u), _tag_arr( _temp6, sizeof( void*), 0u));}); pos += 17; if(
Cyc_Tcutil_failure_reason.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
if( pos >=  80){({ void* _temp7[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\n\t", sizeof( unsigned char), 3u), _tag_arr( _temp7, sizeof( void*),
0u));}); pos= 8;}({ struct Cyc_Std_String_pa_struct _temp9; _temp9.tag= Cyc_Std_String_pa;
_temp9.f1=( struct _tagged_arr) Cyc_Tcutil_failure_reason;{ void* _temp8[ 1u]={&
_temp9}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp8, sizeof( void*), 1u));}});}({ void* _temp10[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp10,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}}
void Cyc_Tcutil_terr( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap){ Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap)));} void*
Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap){ struct
_tagged_arr msg=( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap);({ struct Cyc_Std_String_pa_struct _temp12; _temp12.tag= Cyc_Std_String_pa;
_temp12.f1=( struct _tagged_arr) msg;{ void* _temp11[ 1u]={& _temp12}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp11, sizeof( void*), 1u));}}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);(
int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs !=  0; tvs= tvs->tl){({ struct Cyc_Std_String_pa_struct
_temp15; _temp15.tag= Cyc_Std_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind);{ struct Cyc_Std_String_pa_struct
_temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*) tvs->hd);{ void* _temp13[ 2u]={& _temp14,& _temp15}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s::%s ", sizeof( unsigned char), 8u), _tag_arr(
_temp13, sizeof( void*), 2u));}}});}({ void* _temp16[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp16,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_arr fmt, struct _tagged_arr ap){ struct _tagged_arr msg=(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap); Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp17=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp17->hd=( void*) sg; _temp17->tl= Cyc_Tcutil_warning_segs;
_temp17;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp18=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp18->hd=( void*)({
struct _tagged_arr* _temp19=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp19[ 0]= msg; _temp19;}); _temp18->tl= Cyc_Tcutil_warning_msgs;
_temp18;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs ==  0){
return;}({ void* _temp20[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("***Warnings***\n",
sizeof( unsigned char), 16u), _tag_arr( _temp20, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp21= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs !=  0) {({ struct Cyc_Std_String_pa_struct _temp24;
_temp24.tag= Cyc_Std_String_pa; _temp24.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Std_String_pa_struct _temp23; _temp23.tag= Cyc_Std_String_pa; _temp23.f1=(
struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
_temp21))->hd);{ void* _temp22[ 2u]={& _temp23,& _temp24}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp22, sizeof(
void*), 2u));}}}); _temp21= _temp21->tl; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->tl;}({ void* _temp25[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("**************\n", sizeof( unsigned char), 16u),
_tag_arr( _temp25, sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*)
Cyc_Std_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; static int
Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar* tv2){
return*(( int*) _check_null( tv1->identity)) - *(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp26= t; struct Cyc_Core_Opt*
_temp38; struct Cyc_Core_Opt* _temp40; struct Cyc_Core_Opt** _temp42; struct Cyc_Core_Opt*
_temp43; struct Cyc_Core_Opt* _temp45; struct Cyc_Core_Opt** _temp47; _LL28: if((
unsigned int) _temp26 >  4u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL39:
_temp38=(( struct Cyc_Absyn_Evar_struct*) _temp26)->f2; if( _temp38 ==  0){ goto
_LL29;} else{ goto _LL30;}} else{ goto _LL30;} _LL30: if(( unsigned int) _temp26
>  4u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL41: _temp40=(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; _temp42=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; goto _LL31;} else{ goto _LL32;} _LL32: if(( unsigned int) _temp26
>  4u?*(( int*) _temp26) ==  Cyc_Absyn_TypedefType: 0){ _LL44: _temp43=(( struct
Cyc_Absyn_TypedefType_struct*) _temp26)->f3; if( _temp43 ==  0){ goto _LL33;}
else{ goto _LL34;}} else{ goto _LL34;} _LL34: if(( unsigned int) _temp26 >  4u?*((
int*) _temp26) ==  Cyc_Absyn_TypedefType: 0){ _LL46: _temp45=(( struct Cyc_Absyn_TypedefType_struct*)
_temp26)->f3; _temp47=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp26)->f3; goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37; _LL29: return t;
_LL31: { void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp42))->v); if( t2 != ( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp42))->v){* _temp42=({ struct Cyc_Core_Opt* _temp48=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp48->v=( void*) t2;
_temp48;});} return t2;} _LL33: return t; _LL35: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp47))->v); if( t2 != ( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp47))->v){* _temp47=({ struct Cyc_Core_Opt*
_temp49=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp49->v=( void*) t2; _temp49;});} return t2;} _LL37: return t; _LL27:;} void*
Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp50=( void*) c->v; void* _temp58; struct Cyc_Absyn_Conref* _temp60;
_LL52: if( _temp50 == ( void*) Cyc_Absyn_No_constr){ goto _LL53;} else{ goto
_LL54;} _LL54: if(( unsigned int) _temp50 >  1u?*(( int*) _temp50) ==  Cyc_Absyn_Eq_constr:
0){ _LL59: _temp58=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp50)->f1;
goto _LL55;} else{ goto _LL56;} _LL56: if(( unsigned int) _temp50 >  1u?*(( int*)
_temp50) ==  Cyc_Absyn_Forward_constr: 0){ _LL61: _temp60=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp50)->f1; goto _LL57;} else{ goto _LL51;} _LL53: return Cyc_Absyn_empty_conref();
_LL55: return Cyc_Absyn_new_conref( _temp58); _LL57: return Cyc_Tcutil_copy_conref(
_temp60); _LL51:;} static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_Absyn_Tvar* _temp62=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp62->name= tv->name; _temp62->identity=
0; _temp62->kind= Cyc_Tcutil_copy_conref( tv->kind); _temp62;});} static struct
_tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp65; struct Cyc_Absyn_Tqual
_temp67; struct Cyc_Core_Opt* _temp69; struct _tuple2 _temp63=* arg; _LL70:
_temp69= _temp63.f1; goto _LL68; _LL68: _temp67= _temp63.f2; goto _LL66; _LL66:
_temp65= _temp63.f3; goto _LL64; _LL64: return({ struct _tuple2* _temp71=(
struct _tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp71->f1= _temp69;
_temp71->f2= _temp67; _temp71->f3= Cyc_Tcutil_copy_type( _temp65); _temp71;});}
static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){ void* _temp74;
struct Cyc_Absyn_Tqual _temp76; struct _tuple4 _temp72=* arg; _LL77: _temp76=
_temp72.f1; goto _LL75; _LL75: _temp74= _temp72.f2; goto _LL73; _LL73: return({
struct _tuple4* _temp78=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp78->f1= _temp76; _temp78->f2= Cyc_Tcutil_copy_type( _temp74); _temp78;});}
static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field( struct Cyc_Absyn_Structfield*
f){ return({ struct Cyc_Absyn_Structfield* _temp79=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp79->name= f->name;
_temp79->tq= f->tq; _temp79->type=( void*) Cyc_Tcutil_copy_type(( void*) f->type);
_temp79->width= f->width; _temp79->attributes= f->attributes; _temp79;});}
struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp82; void* _temp83; void* _temp85; struct
_tuple8* _temp80= x; _temp82=* _temp80; _LL86: _temp85= _temp82.f1; goto _LL84;
_LL84: _temp83= _temp82.f2; goto _LL81; _LL81: return({ struct _tuple8* _temp87=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp87->f1= Cyc_Tcutil_copy_type(
_temp85); _temp87->f2= Cyc_Tcutil_copy_type( _temp83); _temp87;});} static
struct Cyc_Absyn_Enumfield* Cyc_Tcutil_copy_enumfield( struct Cyc_Absyn_Enumfield*
f){ return({ struct Cyc_Absyn_Enumfield* _temp88=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp88->name= f->name; _temp88->tag=
f->tag; _temp88->loc= f->loc; _temp88;});} void* Cyc_Tcutil_copy_type( void* t){
void* _temp89= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp139; struct
Cyc_Absyn_TunionInfo _temp141; void* _temp143; struct Cyc_List_List* _temp145;
void* _temp147; struct Cyc_Absyn_TunionFieldInfo _temp149; struct Cyc_List_List*
_temp151; void* _temp153; struct Cyc_Absyn_PtrInfo _temp155; struct Cyc_Absyn_Conref*
_temp157; struct Cyc_Absyn_Tqual _temp159; struct Cyc_Absyn_Conref* _temp161;
void* _temp163; void* _temp165; void* _temp167; void* _temp169; struct Cyc_Absyn_Exp*
_temp171; struct Cyc_Absyn_Tqual _temp173; void* _temp175; struct Cyc_Absyn_FnInfo
_temp177; struct Cyc_List_List* _temp179; struct Cyc_List_List* _temp181; struct
Cyc_Absyn_VarargInfo* _temp183; int _temp185; struct Cyc_List_List* _temp187;
void* _temp189; struct Cyc_Core_Opt* _temp191; struct Cyc_List_List* _temp193;
struct Cyc_List_List* _temp195; struct Cyc_List_List* _temp197; struct _tuple1*
_temp199; struct Cyc_List_List* _temp201; struct _tuple1* _temp203; struct Cyc_List_List*
_temp205; struct Cyc_List_List* _temp207; struct Cyc_Absyn_Enumdecl* _temp209;
struct _tuple1* _temp211; struct Cyc_List_List* _temp213; void* _temp215; struct
Cyc_List_List* _temp217; struct _tuple1* _temp219; void* _temp221; struct Cyc_List_List*
_temp223; void* _temp225; _LL91: if( _temp89 == ( void*) Cyc_Absyn_VoidType){
goto _LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp89 >  4u?*(( int*)
_temp89) ==  Cyc_Absyn_Evar: 0){ goto _LL94;} else{ goto _LL95;} _LL95: if((
unsigned int) _temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_VarType: 0){ _LL140:
_temp139=(( struct Cyc_Absyn_VarType_struct*) _temp89)->f1; goto _LL96;} else{
goto _LL97;} _LL97: if(( unsigned int) _temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_TunionType:
0){ _LL142: _temp141=(( struct Cyc_Absyn_TunionType_struct*) _temp89)->f1;
_LL148: _temp147=( void*) _temp141.tunion_info; goto _LL146; _LL146: _temp145=
_temp141.targs; goto _LL144; _LL144: _temp143=( void*) _temp141.rgn; goto _LL98;}
else{ goto _LL99;} _LL99: if(( unsigned int) _temp89 >  4u?*(( int*) _temp89) == 
Cyc_Absyn_TunionFieldType: 0){ _LL150: _temp149=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp89)->f1; _LL154: _temp153=( void*) _temp149.field_info; goto _LL152; _LL152:
_temp151= _temp149.targs; goto _LL100;} else{ goto _LL101;} _LL101: if((
unsigned int) _temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_PointerType: 0){
_LL156: _temp155=(( struct Cyc_Absyn_PointerType_struct*) _temp89)->f1; _LL166:
_temp165=( void*) _temp155.elt_typ; goto _LL164; _LL164: _temp163=( void*)
_temp155.rgn_typ; goto _LL162; _LL162: _temp161= _temp155.nullable; goto _LL160;
_LL160: _temp159= _temp155.tq; goto _LL158; _LL158: _temp157= _temp155.bounds;
goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int) _temp89 >  4u?*((
int*) _temp89) ==  Cyc_Absyn_IntType: 0){ _LL170: _temp169=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp89)->f1; goto _LL168; _LL168: _temp167=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp89)->f2; goto _LL104;} else{ goto _LL105;} _LL105: if( _temp89 == ( void*)
Cyc_Absyn_FloatType){ goto _LL106;} else{ goto _LL107;} _LL107: if( _temp89 == (
void*) Cyc_Absyn_DoubleType){ goto _LL108;} else{ goto _LL109;} _LL109: if((
unsigned int) _temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_ArrayType: 0){
_LL176: _temp175=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp89)->f1;
goto _LL174; _LL174: _temp173=(( struct Cyc_Absyn_ArrayType_struct*) _temp89)->f2;
goto _LL172; _LL172: _temp171=(( struct Cyc_Absyn_ArrayType_struct*) _temp89)->f3;
goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int) _temp89 >  4u?*((
int*) _temp89) ==  Cyc_Absyn_FnType: 0){ _LL178: _temp177=(( struct Cyc_Absyn_FnType_struct*)
_temp89)->f1; _LL194: _temp193= _temp177.tvars; goto _LL192; _LL192: _temp191=
_temp177.effect; goto _LL190; _LL190: _temp189=( void*) _temp177.ret_typ; goto
_LL188; _LL188: _temp187= _temp177.args; goto _LL186; _LL186: _temp185= _temp177.c_varargs;
goto _LL184; _LL184: _temp183= _temp177.cyc_varargs; goto _LL182; _LL182:
_temp181= _temp177.rgn_po; goto _LL180; _LL180: _temp179= _temp177.attributes;
goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp89 >  4u?*((
int*) _temp89) ==  Cyc_Absyn_TupleType: 0){ _LL196: _temp195=(( struct Cyc_Absyn_TupleType_struct*)
_temp89)->f1; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_StructType: 0){ _LL200: _temp199=((
struct Cyc_Absyn_StructType_struct*) _temp89)->f1; goto _LL198; _LL198: _temp197=((
struct Cyc_Absyn_StructType_struct*) _temp89)->f2; goto _LL116;} else{ goto
_LL117;} _LL117: if(( unsigned int) _temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_UnionType:
0){ _LL204: _temp203=(( struct Cyc_Absyn_UnionType_struct*) _temp89)->f1; goto
_LL202; _LL202: _temp201=(( struct Cyc_Absyn_UnionType_struct*) _temp89)->f2;
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp89 >  4u?*((
int*) _temp89) ==  Cyc_Absyn_AnonStructType: 0){ _LL206: _temp205=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp89)->f1; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_AnonUnionType: 0){ _LL208:
_temp207=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp89)->f1; goto _LL122;}
else{ goto _LL123;} _LL123: if(( unsigned int) _temp89 >  4u?*(( int*) _temp89)
==  Cyc_Absyn_EnumType: 0){ _LL212: _temp211=(( struct Cyc_Absyn_EnumType_struct*)
_temp89)->f1; goto _LL210; _LL210: _temp209=(( struct Cyc_Absyn_EnumType_struct*)
_temp89)->f2; goto _LL124;} else{ goto _LL125;} _LL125: if(( unsigned int)
_temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_AnonEnumType: 0){ _LL214:
_temp213=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp89)->f1; goto _LL126;}
else{ goto _LL127;} _LL127: if(( unsigned int) _temp89 >  4u?*(( int*) _temp89)
==  Cyc_Absyn_RgnHandleType: 0){ _LL216: _temp215=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp89)->f1; goto _LL128;} else{ goto _LL129;} _LL129: if(( unsigned int)
_temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_TypedefType: 0){ _LL220: _temp219=((
struct Cyc_Absyn_TypedefType_struct*) _temp89)->f1; goto _LL218; _LL218:
_temp217=(( struct Cyc_Absyn_TypedefType_struct*) _temp89)->f2; goto _LL130;}
else{ goto _LL131;} _LL131: if( _temp89 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL132;} else{ goto _LL133;} _LL133: if(( unsigned int) _temp89 >  4u?*(( int*)
_temp89) ==  Cyc_Absyn_AccessEff: 0){ _LL222: _temp221=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp89)->f1; goto _LL134;} else{ goto _LL135;} _LL135: if(( unsigned int)
_temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_JoinEff: 0){ _LL224: _temp223=((
struct Cyc_Absyn_JoinEff_struct*) _temp89)->f1; goto _LL136;} else{ goto _LL137;}
_LL137: if(( unsigned int) _temp89 >  4u?*(( int*) _temp89) ==  Cyc_Absyn_RgnsEff:
0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp89)->f1;
goto _LL138;} else{ goto _LL90;} _LL92: goto _LL94; _LL94: return t; _LL96:
return( void*)({ struct Cyc_Absyn_VarType_struct* _temp227=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp227[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp228; _temp228.tag= Cyc_Absyn_VarType; _temp228.f1= Cyc_Tcutil_copy_tvar(
_temp139); _temp228;}); _temp227;}); _LL98: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp229=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp229[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp230; _temp230.tag= Cyc_Absyn_TunionType;
_temp230.f1=({ struct Cyc_Absyn_TunionInfo _temp231; _temp231.tunion_info=( void*)
_temp147; _temp231.targs= Cyc_Tcutil_copy_types( _temp145); _temp231.rgn=( void*)
Cyc_Tcutil_copy_type( _temp143); _temp231;}); _temp230;}); _temp229;}); _LL100:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp232=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp232[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp233; _temp233.tag= Cyc_Absyn_TunionFieldType;
_temp233.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp234; _temp234.field_info=(
void*) _temp153; _temp234.targs= Cyc_Tcutil_copy_types( _temp151); _temp234;});
_temp233;}); _temp232;}); _LL102: { void* _temp235= Cyc_Tcutil_copy_type(
_temp165); void* _temp236= Cyc_Tcutil_copy_type( _temp163); struct Cyc_Absyn_Conref*
_temp237=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp161); struct Cyc_Absyn_Tqual _temp238= _temp159; struct Cyc_Absyn_Conref*
_temp239= Cyc_Tcutil_copy_conref( _temp157); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp240=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp240[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp241; _temp241.tag= Cyc_Absyn_PointerType;
_temp241.f1=({ struct Cyc_Absyn_PtrInfo _temp242; _temp242.elt_typ=( void*)
_temp235; _temp242.rgn_typ=( void*) _temp236; _temp242.nullable= _temp237;
_temp242.tq= _temp238; _temp242.bounds= _temp239; _temp242;}); _temp241;});
_temp240;});} _LL104: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp243=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp243[ 0]=({ struct Cyc_Absyn_IntType_struct _temp244; _temp244.tag= Cyc_Absyn_IntType;
_temp244.f1=( void*) _temp169; _temp244.f2=( void*) _temp167; _temp244;});
_temp243;}); _LL106: goto _LL108; _LL108: return t; _LL110: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp245=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp245[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp246; _temp246.tag= Cyc_Absyn_ArrayType; _temp246.f1=(
void*) Cyc_Tcutil_copy_type( _temp175); _temp246.f2= _temp173; _temp246.f3=
_temp171; _temp246;}); _temp245;}); _LL112: { struct Cyc_List_List* _temp247=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp193); struct
Cyc_Core_Opt* _temp248= _temp191 ==  0? 0:({ struct Cyc_Core_Opt* _temp258=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp258->v=(
void*) Cyc_Tcutil_copy_type(( void*) _temp191->v); _temp258;}); void* _temp249=
Cyc_Tcutil_copy_type( _temp189); struct Cyc_List_List* _temp250=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_copy_arg, _temp187); int _temp251= _temp185; struct Cyc_Absyn_VarargInfo*
cyc_varargs2= 0; if( _temp183 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp183); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp252=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp252->name= cv->name; _temp252->tq= cv->tq; _temp252->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp252->inject= cv->inject; _temp252;});}{ struct Cyc_List_List*
_temp253=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp181);
struct Cyc_List_List* _temp254= _temp179; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp255=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp255[ 0]=({ struct Cyc_Absyn_FnType_struct _temp256; _temp256.tag= Cyc_Absyn_FnType;
_temp256.f1=({ struct Cyc_Absyn_FnInfo _temp257; _temp257.tvars= _temp247;
_temp257.effect= _temp248; _temp257.ret_typ=( void*) _temp249; _temp257.args=
_temp250; _temp257.c_varargs= _temp251; _temp257.cyc_varargs= cyc_varargs2;
_temp257.rgn_po= _temp253; _temp257.attributes= _temp254; _temp257;}); _temp256;});
_temp255;});}} _LL114: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp259=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp259[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp260; _temp260.tag= Cyc_Absyn_TupleType;
_temp260.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp195);
_temp260;}); _temp259;}); _LL116: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp261=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp261[ 0]=({ struct Cyc_Absyn_StructType_struct _temp262; _temp262.tag= Cyc_Absyn_StructType;
_temp262.f1= _temp199; _temp262.f2= Cyc_Tcutil_copy_types( _temp197); _temp262.f3=
0; _temp262;}); _temp261;}); _LL118: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp263=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp263[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp264; _temp264.tag= Cyc_Absyn_UnionType;
_temp264.f1= _temp203; _temp264.f2= Cyc_Tcutil_copy_types( _temp201); _temp264.f3=
0; _temp264;}); _temp263;}); _LL120: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp265=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp265[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp266; _temp266.tag= Cyc_Absyn_AnonStructType; _temp266.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp205); _temp266;}); _temp265;});
_LL122: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp267=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp267[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp268; _temp268.tag=
Cyc_Absyn_AnonUnionType; _temp268.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp207); _temp268;}); _temp267;}); _LL124: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp269=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp269[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp270; _temp270.tag= Cyc_Absyn_EnumType;
_temp270.f1= _temp211; _temp270.f2= _temp209; _temp270;}); _temp269;}); _LL126:
return( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp271=( struct Cyc_Absyn_AnonEnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct)); _temp271[ 0]=({
struct Cyc_Absyn_AnonEnumType_struct _temp272; _temp272.tag= Cyc_Absyn_AnonEnumType;
_temp272.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Enumfield*(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_enumfield,
_temp213); _temp272;}); _temp271;}); _LL128: return( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp273=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp273[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp274; _temp274.tag=
Cyc_Absyn_RgnHandleType; _temp274.f1=( void*) Cyc_Tcutil_copy_type( _temp215);
_temp274;}); _temp273;}); _LL130: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp275=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp275[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp276; _temp276.tag= Cyc_Absyn_TypedefType;
_temp276.f1= _temp219; _temp276.f2= Cyc_Tcutil_copy_types( _temp217); _temp276.f3=
0; _temp276;}); _temp275;}); _LL132: return t; _LL134: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp277=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp277[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp278; _temp278.tag= Cyc_Absyn_AccessEff; _temp278.f1=(
void*) Cyc_Tcutil_copy_type( _temp221); _temp278;}); _temp277;}); _LL136: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp279=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp279[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp280; _temp280.tag= Cyc_Absyn_JoinEff; _temp280.f1= Cyc_Tcutil_copy_types(
_temp223); _temp280;}); _temp279;}); _LL138: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp281=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp281[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp282; _temp282.tag= Cyc_Absyn_RgnsEff;
_temp282.f1=( void*) Cyc_Tcutil_copy_type( _temp225); _temp282;}); _temp281;});
_LL90:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){
return 1;}{ struct _tuple8 _temp284=({ struct _tuple8 _temp283; _temp283.f1= k1;
_temp283.f2= k2; _temp283;}); void* _temp294; void* _temp296; void* _temp298;
void* _temp300; void* _temp302; void* _temp304; _LL286: _LL297: _temp296=
_temp284.f1; if( _temp296 == ( void*) Cyc_Absyn_BoxKind){ goto _LL295;} else{
goto _LL288;} _LL295: _temp294= _temp284.f2; if( _temp294 == ( void*) Cyc_Absyn_MemKind){
goto _LL287;} else{ goto _LL288;} _LL288: _LL301: _temp300= _temp284.f1; if(
_temp300 == ( void*) Cyc_Absyn_BoxKind){ goto _LL299;} else{ goto _LL290;}
_LL299: _temp298= _temp284.f2; if( _temp298 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL289;} else{ goto _LL290;} _LL290: _LL305: _temp304= _temp284.f1; if( _temp304
== ( void*) Cyc_Absyn_MemKind){ goto _LL303;} else{ goto _LL292;} _LL303:
_temp302= _temp284.f2; if( _temp302 == ( void*) Cyc_Absyn_AnyKind){ goto _LL291;}
else{ goto _LL292;} _LL292: goto _LL293; _LL287: goto _LL289; _LL289: goto
_LL291; _LL291: return 1; _LL293: return 0; _LL285:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp306= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp364;
struct Cyc_Core_Opt* _temp366; struct Cyc_Absyn_Tvar* _temp368; void* _temp370;
struct Cyc_Absyn_TunionFieldInfo _temp372; void* _temp374; struct Cyc_Absyn_Tunionfield*
_temp376; struct Cyc_Absyn_TunionFieldInfo _temp378; void* _temp380; struct Cyc_Absyn_Structdecl**
_temp382; struct Cyc_Absyn_Uniondecl** _temp384; struct Cyc_Absyn_Enumdecl*
_temp386; struct Cyc_Absyn_Enumdecl* _temp388; struct Cyc_Absyn_PtrInfo _temp390;
struct Cyc_Core_Opt* _temp392; _LL308: if(( unsigned int) _temp306 >  4u?*(( int*)
_temp306) ==  Cyc_Absyn_Evar: 0){ _LL367: _temp366=(( struct Cyc_Absyn_Evar_struct*)
_temp306)->f1; goto _LL365; _LL365: _temp364=(( struct Cyc_Absyn_Evar_struct*)
_temp306)->f2; goto _LL309;} else{ goto _LL310;} _LL310: if(( unsigned int)
_temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_VarType: 0){ _LL369: _temp368=((
struct Cyc_Absyn_VarType_struct*) _temp306)->f1; goto _LL311;} else{ goto _LL312;}
_LL312: if( _temp306 == ( void*) Cyc_Absyn_VoidType){ goto _LL313;} else{ goto
_LL314;} _LL314: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_IntType:
0){ _LL371: _temp370=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp306)->f2;
goto _LL315;} else{ goto _LL316;} _LL316: if( _temp306 == ( void*) Cyc_Absyn_FloatType){
goto _LL317;} else{ goto _LL318;} _LL318: if( _temp306 == ( void*) Cyc_Absyn_DoubleType){
goto _LL319;} else{ goto _LL320;} _LL320: if(( unsigned int) _temp306 >  4u?*((
int*) _temp306) ==  Cyc_Absyn_FnType: 0){ goto _LL321;} else{ goto _LL322;}
_LL322: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL323;} else{ goto _LL324;} _LL324: if( _temp306 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int) _temp306 >  4u?*((
int*) _temp306) ==  Cyc_Absyn_TunionType: 0){ goto _LL327;} else{ goto _LL328;}
_LL328: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_TunionFieldType:
0){ _LL373: _temp372=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp306)->f1;
_LL375: _temp374=( void*) _temp372.field_info; if(*(( int*) _temp374) ==  Cyc_Absyn_KnownTunionfield){
_LL377: _temp376=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp374)->f2;
goto _LL329;} else{ goto _LL330;}} else{ goto _LL330;} _LL330: if(( unsigned int)
_temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_TunionFieldType: 0){ _LL379:
_temp378=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp306)->f1; _LL381:
_temp380=( void*) _temp378.field_info; if(*(( int*) _temp380) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL331;} else{ goto _LL332;}} else{ goto _LL332;} _LL332: if(( unsigned int)
_temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_StructType: 0){ _LL383:
_temp382=(( struct Cyc_Absyn_StructType_struct*) _temp306)->f3; if( _temp382 == 
0){ goto _LL333;} else{ goto _LL334;}} else{ goto _LL334;} _LL334: if((
unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_UnionType: 0){
_LL385: _temp384=(( struct Cyc_Absyn_UnionType_struct*) _temp306)->f3; if(
_temp384 ==  0){ goto _LL335;} else{ goto _LL336;}} else{ goto _LL336;} _LL336:
if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_EnumType: 0){
_LL387: _temp386=(( struct Cyc_Absyn_EnumType_struct*) _temp306)->f2; if(
_temp386 ==  0){ goto _LL337;} else{ goto _LL338;}} else{ goto _LL338;} _LL338:
if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_StructType:
0){ goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int) _temp306 >  4u?*((
int*) _temp306) ==  Cyc_Absyn_UnionType: 0){ goto _LL341;} else{ goto _LL342;}
_LL342: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int) _temp306 >  4u?*((
int*) _temp306) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL345;} else{ goto _LL346;}
_LL346: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp306 >  4u?*((
int*) _temp306) ==  Cyc_Absyn_EnumType: 0){ _LL389: _temp388=(( struct Cyc_Absyn_EnumType_struct*)
_temp306)->f2; goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int)
_temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_PointerType: 0){ _LL391:
_temp390=(( struct Cyc_Absyn_PointerType_struct*) _temp306)->f1; goto _LL351;}
else{ goto _LL352;} _LL352: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306)
==  Cyc_Absyn_ArrayType: 0){ goto _LL353;} else{ goto _LL354;} _LL354: if((
unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_TupleType: 0){
goto _LL355;} else{ goto _LL356;} _LL356: if(( unsigned int) _temp306 >  4u?*((
int*) _temp306) ==  Cyc_Absyn_TypedefType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_TypedefType_struct*)
_temp306)->f3; goto _LL357;} else{ goto _LL358;} _LL358: if(( unsigned int)
_temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_AccessEff: 0){ goto _LL359;}
else{ goto _LL360;} _LL360: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306)
==  Cyc_Absyn_JoinEff: 0){ goto _LL361;} else{ goto _LL362;} _LL362: if((
unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL363;} else{ goto _LL307;} _LL309: return( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp366))->v; _LL311: return Cyc_Absyn_conref_val( _temp368->kind);
_LL313: return( void*) Cyc_Absyn_MemKind; _LL315: return _temp370 == ( void*)
Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL317: goto
_LL319; _LL319: goto _LL321; _LL321: return( void*) Cyc_Absyn_MemKind; _LL323:
return( void*) Cyc_Absyn_BoxKind; _LL325: return( void*) Cyc_Absyn_RgnKind;
_LL327: return( void*) Cyc_Absyn_BoxKind; _LL329: if( _temp376->typs ==  0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL331: return({ void* _temp394[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u), _tag_arr( _temp394, sizeof( void*), 0u));});
_LL333: goto _LL335; _LL335: goto _LL337; _LL337: return( void*) Cyc_Absyn_AnyKind;
_LL339: goto _LL341; _LL341: goto _LL343; _LL343: goto _LL345; _LL345: goto
_LL347; _LL347: return( void*) Cyc_Absyn_MemKind; _LL349: if( _temp388->fields
==  0){ return( void*) Cyc_Absyn_AnyKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL351: { void* _temp395=( void*)( Cyc_Absyn_compress_conref( _temp390.bounds))->v;
void* _temp405; void* _temp407; _LL397: if(( unsigned int) _temp395 >  1u?*((
int*) _temp395) ==  Cyc_Absyn_Eq_constr: 0){ _LL406: _temp405=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp395)->f1; if( _temp405 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL398;} else{ goto _LL399;}} else{ goto _LL399;} _LL399: if(( unsigned int)
_temp395 >  1u?*(( int*) _temp395) ==  Cyc_Absyn_Eq_constr: 0){ _LL408: _temp407=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp395)->f1; if(( unsigned int)
_temp407 >  1u?*(( int*) _temp407) ==  Cyc_Absyn_Upper_b: 0){ goto _LL400;}
else{ goto _LL401;}} else{ goto _LL401;} _LL401: if( _temp395 == ( void*) Cyc_Absyn_No_constr){
goto _LL402;} else{ goto _LL403;} _LL403: if(( unsigned int) _temp395 >  1u?*((
int*) _temp395) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL404;} else{ goto
_LL396;} _LL398: return( void*) Cyc_Absyn_MemKind; _LL400: return( void*) Cyc_Absyn_BoxKind;
_LL402: return( void*) Cyc_Absyn_MemKind; _LL404: return({ void* _temp409[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds", sizeof(
unsigned char), 43u), _tag_arr( _temp409, sizeof( void*), 0u));}); _LL396:;}
_LL353: goto _LL355; _LL355: return( void*) Cyc_Absyn_MemKind; _LL357: return({
struct Cyc_Std_String_pa_struct _temp411; _temp411.tag= Cyc_Std_String_pa;
_temp411.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp410[ 1u]={&
_temp411}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp410, sizeof( void*), 1u));}}); _LL359: goto
_LL361; _LL361: goto _LL363; _LL363: return( void*) Cyc_Absyn_EffKind; _LL307:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp412; _push_handler(& _temp412);{
int _temp414= 0; if( setjmp( _temp412.handler)){ _temp414= 1;} if( ! _temp414){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp415= 1; _npop_handler( 0u); return
_temp415;}; _pop_handler();} else{ void* _temp413=( void*) _exn_thrown; void*
_temp417= _temp413; _LL419: if( _temp417 ==  Cyc_Tcutil_Unify){ goto _LL420;}
else{ goto _LL421;} _LL421: goto _LL422; _LL420: return 0; _LL422:( void) _throw(
_temp417); _LL418:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp423= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp459; struct Cyc_Core_Opt* _temp461; struct Cyc_Core_Opt**
_temp463; struct Cyc_Core_Opt* _temp464; struct Cyc_Absyn_PtrInfo _temp466; void*
_temp468; struct Cyc_Absyn_FnInfo _temp470; struct Cyc_List_List* _temp472;
struct Cyc_List_List* _temp474; struct Cyc_Absyn_VarargInfo* _temp476; int
_temp478; struct Cyc_List_List* _temp480; void* _temp482; struct Cyc_Core_Opt*
_temp484; struct Cyc_List_List* _temp486; struct Cyc_List_List* _temp488; void*
_temp490; struct Cyc_Absyn_TunionInfo _temp492; void* _temp494; struct Cyc_List_List*
_temp496; struct Cyc_Core_Opt* _temp498; struct Cyc_List_List* _temp500; struct
Cyc_Absyn_TunionFieldInfo _temp502; struct Cyc_List_List* _temp504; struct Cyc_List_List*
_temp506; struct Cyc_List_List* _temp508; struct Cyc_List_List* _temp510; void*
_temp512; struct Cyc_List_List* _temp514; void* _temp516; _LL425: if((
unsigned int) _temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_VarType: 0){
_LL460: _temp459=(( struct Cyc_Absyn_VarType_struct*) _temp423)->f1; goto _LL426;}
else{ goto _LL427;} _LL427: if(( unsigned int) _temp423 >  4u?*(( int*) _temp423)
==  Cyc_Absyn_Evar: 0){ _LL465: _temp464=(( struct Cyc_Absyn_Evar_struct*)
_temp423)->f2; goto _LL462; _LL462: _temp461=(( struct Cyc_Absyn_Evar_struct*)
_temp423)->f4; _temp463=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp423)->f4; goto _LL428;} else{ goto _LL429;} _LL429: if(( unsigned int)
_temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_PointerType: 0){ _LL467:
_temp466=(( struct Cyc_Absyn_PointerType_struct*) _temp423)->f1; goto _LL430;}
else{ goto _LL431;} _LL431: if(( unsigned int) _temp423 >  4u?*(( int*) _temp423)
==  Cyc_Absyn_ArrayType: 0){ _LL469: _temp468=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp423)->f1; goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int)
_temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_FnType: 0){ _LL471: _temp470=((
struct Cyc_Absyn_FnType_struct*) _temp423)->f1; _LL487: _temp486= _temp470.tvars;
goto _LL485; _LL485: _temp484= _temp470.effect; goto _LL483; _LL483: _temp482=(
void*) _temp470.ret_typ; goto _LL481; _LL481: _temp480= _temp470.args; goto
_LL479; _LL479: _temp478= _temp470.c_varargs; goto _LL477; _LL477: _temp476=
_temp470.cyc_varargs; goto _LL475; _LL475: _temp474= _temp470.rgn_po; goto
_LL473; _LL473: _temp472= _temp470.attributes; goto _LL434;} else{ goto _LL435;}
_LL435: if(( unsigned int) _temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_TupleType:
0){ _LL489: _temp488=(( struct Cyc_Absyn_TupleType_struct*) _temp423)->f1; goto
_LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp423 >  4u?*(( int*)
_temp423) ==  Cyc_Absyn_RgnHandleType: 0){ _LL491: _temp490=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp423)->f1; goto _LL438;} else{ goto _LL439;}
_LL439: if(( unsigned int) _temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_TunionType:
0){ _LL493: _temp492=(( struct Cyc_Absyn_TunionType_struct*) _temp423)->f1;
_LL497: _temp496= _temp492.targs; goto _LL495; _LL495: _temp494=( void*)
_temp492.rgn; goto _LL440;} else{ goto _LL441;} _LL441: if(( unsigned int)
_temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_TypedefType: 0){ _LL501:
_temp500=(( struct Cyc_Absyn_TypedefType_struct*) _temp423)->f2; goto _LL499;
_LL499: _temp498=(( struct Cyc_Absyn_TypedefType_struct*) _temp423)->f3; goto
_LL442;} else{ goto _LL443;} _LL443: if(( unsigned int) _temp423 >  4u?*(( int*)
_temp423) ==  Cyc_Absyn_TunionFieldType: 0){ _LL503: _temp502=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp423)->f1; _LL505: _temp504= _temp502.targs; goto _LL444;} else{ goto _LL445;}
_LL445: if(( unsigned int) _temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_StructType:
0){ _LL507: _temp506=(( struct Cyc_Absyn_StructType_struct*) _temp423)->f2; goto
_LL446;} else{ goto _LL447;} _LL447: if(( unsigned int) _temp423 >  4u?*(( int*)
_temp423) ==  Cyc_Absyn_AnonStructType: 0){ _LL509: _temp508=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp423)->f1; goto _LL448;} else{ goto _LL449;} _LL449: if(( unsigned int)
_temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_AnonUnionType: 0){ _LL511:
_temp510=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp423)->f1; goto _LL450;}
else{ goto _LL451;} _LL451: if(( unsigned int) _temp423 >  4u?*(( int*) _temp423)
==  Cyc_Absyn_AccessEff: 0){ _LL513: _temp512=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp423)->f1; goto _LL452;} else{ goto _LL453;} _LL453: if(( unsigned int)
_temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_JoinEff: 0){ _LL515: _temp514=((
struct Cyc_Absyn_JoinEff_struct*) _temp423)->f1; goto _LL454;} else{ goto _LL455;}
_LL455: if(( unsigned int) _temp423 >  4u?*(( int*) _temp423) ==  Cyc_Absyn_RgnsEff:
0){ _LL517: _temp516=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp423)->f1;
goto _LL456;} else{ goto _LL457;} _LL457: goto _LL458; _LL426: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp459)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);} goto
_LL424; _LL428: if( t ==  evar){ Cyc_Tcutil_failure_reason= _tag_arr("(occurs check)",
sizeof( unsigned char), 15u);( int) _throw(( void*) Cyc_Tcutil_Unify);} else{
if( _temp464 !=  0){ Cyc_Tcutil_occurs( evar, env,( void*) _temp464->v);} else{
int problem= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp463))->v; for( 0; s !=  0; s= s->tl){ if( !(( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*) s->hd)){ problem= 1; break;}}} if( problem){ struct Cyc_List_List*
_temp518= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp463))->v; for( 0; s !=  0; s= s->tl){ if((( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*) s->hd)){ _temp518=({ struct Cyc_List_List* _temp519=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp519->hd=(
void*)(( struct Cyc_Absyn_Tvar*) s->hd); _temp519->tl= _temp518; _temp519;});}}}*
_temp463=({ struct Cyc_Core_Opt* _temp520=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp520->v=( void*) _temp518; _temp520;});}}}
goto _LL424; _LL430: Cyc_Tcutil_occurs( evar, env,( void*) _temp466.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp466.rgn_typ); goto _LL424; _LL432:
Cyc_Tcutil_occurs( evar, env, _temp468); goto _LL424; _LL434: if( _temp484 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*) _temp484->v);} Cyc_Tcutil_occurs( evar,
env, _temp482); for( 0; _temp480 !=  0; _temp480= _temp480->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*) _temp480->hd)).f3);} if( _temp476 !=  0){ void*
_temp523; struct Cyc_Absyn_VarargInfo _temp521=* _temp476; _LL524: _temp523=(
void*) _temp521.type; goto _LL522; _LL522: Cyc_Tcutil_occurs( evar, env,
_temp523);} for( 0; _temp474 !=  0; _temp474= _temp474->tl){ struct _tuple8
_temp527; void* _temp528; void* _temp530; struct _tuple8* _temp525=( struct
_tuple8*) _temp474->hd; _temp527=* _temp525; _LL531: _temp530= _temp527.f1; goto
_LL529; _LL529: _temp528= _temp527.f2; goto _LL526; _LL526: Cyc_Tcutil_occurs(
evar, env, _temp530); Cyc_Tcutil_occurs( evar, env, _temp528);} goto _LL424;
_LL436: for( 0; _temp488 !=  0; _temp488= _temp488->tl){ Cyc_Tcutil_occurs( evar,
env,(*(( struct _tuple4*) _temp488->hd)).f2);} goto _LL424; _LL438: Cyc_Tcutil_occurs(
evar, env, _temp490); goto _LL424; _LL440: Cyc_Tcutil_occurs( evar, env,
_temp494); Cyc_Tcutil_occurslist( evar, env, _temp496); goto _LL424; _LL442:
_temp504= _temp500; goto _LL444; _LL444: _temp506= _temp504; goto _LL446; _LL446:
Cyc_Tcutil_occurslist( evar, env, _temp506); goto _LL424; _LL448: _temp510=
_temp508; goto _LL450; _LL450: for( 0; _temp510 !=  0; _temp510= _temp510->tl){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*) _temp510->hd)->type);}
goto _LL424; _LL452: Cyc_Tcutil_occurs( evar, env, _temp512); goto _LL424;
_LL454: Cyc_Tcutil_occurslist( evar, env, _temp514); goto _LL424; _LL456: Cyc_Tcutil_occurs(
evar, env, _temp516); goto _LL424; _LL458: goto _LL424; _LL424:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts !=  0; ts= ts->tl){ Cyc_Tcutil_occurs( evar, env,( void*) ts->hd);}}
static void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List*
t2){ for( 0; t1 !=  0? t2 !=  0: 0;( t1= t1->tl, t2= t2->tl)){ Cyc_Tcutil_unify_it((
void*) t1->hd,( void*) t2->hd);} if( t1 !=  0? 1: t2 !=  0){( int) _throw(( void*)
Cyc_Tcutil_Unify);}} static void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const !=  tq2.q_const? 1: tq1.q_volatile
!=  tq2.q_volatile)? 1: tq1.q_restrict !=  tq2.q_restrict){ Cyc_Tcutil_failure_reason=
_tag_arr("(qualifiers don't match)", sizeof( unsigned char), 25u);( int) _throw((
void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const ==  tq2.q_const? tq1.q_volatile
==  tq2.q_volatile: 0)? tq1.q_restrict ==  tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y, struct _tagged_arr reason){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref(
y); if( x ==  y){ return;}{ void* _temp532=( void*) x->v; void* _temp540; _LL534:
if( _temp532 == ( void*) Cyc_Absyn_No_constr){ goto _LL535;} else{ goto _LL536;}
_LL536: if(( unsigned int) _temp532 >  1u?*(( int*) _temp532) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL537;} else{ goto _LL538;} _LL538: if(( unsigned int) _temp532 >  1u?*((
int*) _temp532) ==  Cyc_Absyn_Eq_constr: 0){ _LL541: _temp540=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp532)->f1; goto _LL539;} else{ goto _LL533;}
_LL535:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp542=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp542[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp543; _temp543.tag= Cyc_Absyn_Forward_constr; _temp543.f1= y; _temp543;});
_temp542;}))); return; _LL537:( int) _throw(({ void* _temp544[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp544, sizeof( void*), 0u));})); _LL539: { void* _temp545=( void*)
y->v; void* _temp553; _LL547: if( _temp545 == ( void*) Cyc_Absyn_No_constr){
goto _LL548;} else{ goto _LL549;} _LL549: if(( unsigned int) _temp545 >  1u?*((
int*) _temp545) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL550;} else{ goto
_LL551;} _LL551: if(( unsigned int) _temp545 >  1u?*(( int*) _temp545) ==  Cyc_Absyn_Eq_constr:
0){ _LL554: _temp553=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp545)->f1;
goto _LL552;} else{ goto _LL546;} _LL548:( void*)( y->v=( void*)(( void*) x->v));
return; _LL550:( int) _throw(({ void* _temp555[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp555, sizeof( void*), 0u));})); _LL552: if( cmp( _temp540,
_temp553) !=  0){ Cyc_Tcutil_failure_reason= reason;( int) _throw(( void*) Cyc_Tcutil_Unify);}
return; _LL546:;} _LL533:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp556; _push_handler(& _temp556);{ int _temp558= 0; if( setjmp(
_temp556.handler)){ _temp558= 1;} if( ! _temp558){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u));{ int _temp559= 1;
_npop_handler( 0u); return _temp559;}; _pop_handler();} else{ void* _temp557=(
void*) _exn_thrown; void* _temp561= _temp557; _LL563: if( _temp561 ==  Cyc_Tcutil_Unify){
goto _LL564;} else{ goto _LL565;} _LL565: goto _LL566; _LL564: return 0; _LL566:(
void) _throw( _temp561); _LL562:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp568=({ struct _tuple8 _temp567; _temp567.f1= b1;
_temp567.f2= b2; _temp567;}); void* _temp578; void* _temp580; void* _temp582;
void* _temp584; void* _temp586; struct Cyc_Absyn_Exp* _temp588; void* _temp590;
struct Cyc_Absyn_Exp* _temp592; _LL570: _LL581: _temp580= _temp568.f1; if(
_temp580 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL579;} else{ goto _LL572;}
_LL579: _temp578= _temp568.f2; if( _temp578 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL571;} else{ goto _LL572;} _LL572: _LL583: _temp582= _temp568.f1; if(
_temp582 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL573;} else{ goto _LL574;}
_LL574: _LL585: _temp584= _temp568.f2; if( _temp584 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL575;} else{ goto _LL576;} _LL576: _LL591: _temp590= _temp568.f1; if((
unsigned int) _temp590 >  1u?*(( int*) _temp590) ==  Cyc_Absyn_Upper_b: 0){
_LL593: _temp592=(( struct Cyc_Absyn_Upper_b_struct*) _temp590)->f1; goto _LL587;}
else{ goto _LL569;} _LL587: _temp586= _temp568.f2; if(( unsigned int) _temp586 > 
1u?*(( int*) _temp586) ==  Cyc_Absyn_Upper_b: 0){ _LL589: _temp588=(( struct Cyc_Absyn_Upper_b_struct*)
_temp586)->f1; goto _LL577;} else{ goto _LL569;} _LL571: return 0; _LL573:
return - 1; _LL575: return 1; _LL577: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp592); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp588); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL569:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp595=({
struct _tuple8 _temp594; _temp594.f1= a1; _temp594.f2= a2; _temp594;}); void*
_temp607; int _temp609; int _temp611; void* _temp613; void* _temp615; int
_temp617; int _temp619; void* _temp621; void* _temp623; int _temp625; void*
_temp627; int _temp629; void* _temp631; int _temp633; void* _temp635; int
_temp637; void* _temp639; struct _tagged_arr _temp641; void* _temp643; struct
_tagged_arr _temp645; _LL597: _LL616: _temp615= _temp595.f1; if(( unsigned int)
_temp615 >  16u?*(( int*) _temp615) ==  Cyc_Absyn_Format_att: 0){ _LL622:
_temp621=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp615)->f1; goto
_LL620; _LL620: _temp619=(( struct Cyc_Absyn_Format_att_struct*) _temp615)->f2;
goto _LL618; _LL618: _temp617=(( struct Cyc_Absyn_Format_att_struct*) _temp615)->f3;
goto _LL608;} else{ goto _LL599;} _LL608: _temp607= _temp595.f2; if((
unsigned int) _temp607 >  16u?*(( int*) _temp607) ==  Cyc_Absyn_Format_att: 0){
_LL614: _temp613=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp607)->f1;
goto _LL612; _LL612: _temp611=(( struct Cyc_Absyn_Format_att_struct*) _temp607)->f2;
goto _LL610; _LL610: _temp609=(( struct Cyc_Absyn_Format_att_struct*) _temp607)->f3;
goto _LL598;} else{ goto _LL599;} _LL599: _LL628: _temp627= _temp595.f1; if((
unsigned int) _temp627 >  16u?*(( int*) _temp627) ==  Cyc_Absyn_Regparm_att: 0){
_LL630: _temp629=(( struct Cyc_Absyn_Regparm_att_struct*) _temp627)->f1; goto
_LL624;} else{ goto _LL601;} _LL624: _temp623= _temp595.f2; if(( unsigned int)
_temp623 >  16u?*(( int*) _temp623) ==  Cyc_Absyn_Regparm_att: 0){ _LL626:
_temp625=(( struct Cyc_Absyn_Regparm_att_struct*) _temp623)->f1; goto _LL600;}
else{ goto _LL601;} _LL601: _LL636: _temp635= _temp595.f1; if(( unsigned int)
_temp635 >  16u?*(( int*) _temp635) ==  Cyc_Absyn_Aligned_att: 0){ _LL638:
_temp637=(( struct Cyc_Absyn_Aligned_att_struct*) _temp635)->f1; goto _LL632;}
else{ goto _LL603;} _LL632: _temp631= _temp595.f2; if(( unsigned int) _temp631 > 
16u?*(( int*) _temp631) ==  Cyc_Absyn_Aligned_att: 0){ _LL634: _temp633=((
struct Cyc_Absyn_Aligned_att_struct*) _temp631)->f1; goto _LL602;} else{ goto
_LL603;} _LL603: _LL644: _temp643= _temp595.f1; if(( unsigned int) _temp643 > 
16u?*(( int*) _temp643) ==  Cyc_Absyn_Section_att: 0){ _LL646: _temp645=((
struct Cyc_Absyn_Section_att_struct*) _temp643)->f1; goto _LL640;} else{ goto
_LL605;} _LL640: _temp639= _temp595.f2; if(( unsigned int) _temp639 >  16u?*((
int*) _temp639) ==  Cyc_Absyn_Section_att: 0){ _LL642: _temp641=(( struct Cyc_Absyn_Section_att_struct*)
_temp639)->f1; goto _LL604;} else{ goto _LL605;} _LL605: goto _LL606; _LL598:
return( _temp621 ==  _temp613? _temp619 ==  _temp611: 0)? _temp617 ==  _temp609:
0; _LL600: _temp637= _temp629; _temp633= _temp625; goto _LL602; _LL602: return
_temp637 ==  _temp633; _LL604: return Cyc_Std_strcmp( _temp645, _temp641) ==  0;
_LL606: return 0; _LL596:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a= a->tl){
if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*) a->hd, a2)){ return 0;}}}{
struct Cyc_List_List* a= a2; for( 0; a !=  0; a= a->tl){ if( ! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,( void*) a->hd, a1)){ return 0;}}} return 1;} static void*
Cyc_Tcutil_normalize_effect( void* e); static void* Cyc_Tcutil_rgns_of( void* t);
static void* Cyc_Tcutil_rgns_of_field( struct Cyc_Absyn_Structfield* sf){ return
Cyc_Tcutil_rgns_of(( void*) sf->type);} static struct _tuple5* Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar* tv){ void* t;{ void* _temp647= Cyc_Absyn_conref_val( tv->kind);
_LL649: if( _temp647 == ( void*) Cyc_Absyn_RgnKind){ goto _LL650;} else{ goto
_LL651;} _LL651: if( _temp647 == ( void*) Cyc_Absyn_EffKind){ goto _LL652;}
else{ goto _LL653;} _LL653: goto _LL654; _LL650: t=( void*) Cyc_Absyn_HeapRgn;
goto _LL648; _LL652: t= Cyc_Absyn_empty_effect; goto _LL648; _LL654: t= Cyc_Absyn_sint_t;
goto _LL648; _LL648:;} return({ struct _tuple5* _temp655=( struct _tuple5*)
_cycalloc( sizeof( struct _tuple5)); _temp655->f1= tv; _temp655->f2= t; _temp655;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp656= Cyc_Tcutil_compress(
t); void* _temp706; struct Cyc_Absyn_TunionInfo _temp708; void* _temp710; struct
Cyc_List_List* _temp712; struct Cyc_Absyn_PtrInfo _temp714; void* _temp716; void*
_temp718; void* _temp720; struct Cyc_List_List* _temp722; struct Cyc_Absyn_TunionFieldInfo
_temp724; struct Cyc_List_List* _temp726; struct Cyc_List_List* _temp728; struct
Cyc_List_List* _temp730; struct Cyc_List_List* _temp732; struct Cyc_List_List*
_temp734; struct Cyc_Absyn_FnInfo _temp736; struct Cyc_List_List* _temp738;
struct Cyc_Absyn_VarargInfo* _temp740; struct Cyc_List_List* _temp742; void*
_temp744; struct Cyc_Core_Opt* _temp746; struct Cyc_List_List* _temp748; void*
_temp750; _LL658: if( _temp656 == ( void*) Cyc_Absyn_VoidType){ goto _LL659;}
else{ goto _LL660;} _LL660: if( _temp656 == ( void*) Cyc_Absyn_FloatType){ goto
_LL661;} else{ goto _LL662;} _LL662: if( _temp656 == ( void*) Cyc_Absyn_DoubleType){
goto _LL663;} else{ goto _LL664;} _LL664: if(( unsigned int) _temp656 >  4u?*((
int*) _temp656) ==  Cyc_Absyn_EnumType: 0){ goto _LL665;} else{ goto _LL666;}
_LL666: if(( unsigned int) _temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL667;} else{ goto _LL668;} _LL668: if(( unsigned int) _temp656 >  4u?*((
int*) _temp656) ==  Cyc_Absyn_IntType: 0){ goto _LL669;} else{ goto _LL670;}
_LL670: if(( unsigned int) _temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_Evar:
0){ goto _LL671;} else{ goto _LL672;} _LL672: if(( unsigned int) _temp656 >  4u?*((
int*) _temp656) ==  Cyc_Absyn_VarType: 0){ goto _LL673;} else{ goto _LL674;}
_LL674: if(( unsigned int) _temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_RgnHandleType:
0){ _LL707: _temp706=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp656)->f1;
goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int) _temp656 >  4u?*((
int*) _temp656) ==  Cyc_Absyn_TunionType: 0){ _LL709: _temp708=(( struct Cyc_Absyn_TunionType_struct*)
_temp656)->f1; _LL713: _temp712= _temp708.targs; goto _LL711; _LL711: _temp710=(
void*) _temp708.rgn; goto _LL677;} else{ goto _LL678;} _LL678: if(( unsigned int)
_temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_PointerType: 0){ _LL715:
_temp714=(( struct Cyc_Absyn_PointerType_struct*) _temp656)->f1; _LL719:
_temp718=( void*) _temp714.elt_typ; goto _LL717; _LL717: _temp716=( void*)
_temp714.rgn_typ; goto _LL679;} else{ goto _LL680;} _LL680: if(( unsigned int)
_temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_ArrayType: 0){ _LL721: _temp720=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp656)->f1; goto _LL681;} else{
goto _LL682;} _LL682: if(( unsigned int) _temp656 >  4u?*(( int*) _temp656) == 
Cyc_Absyn_TupleType: 0){ _LL723: _temp722=(( struct Cyc_Absyn_TupleType_struct*)
_temp656)->f1; goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_TunionFieldType: 0){ _LL725:
_temp724=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp656)->f1; _LL727:
_temp726= _temp724.targs; goto _LL685;} else{ goto _LL686;} _LL686: if((
unsigned int) _temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_StructType: 0){
_LL729: _temp728=(( struct Cyc_Absyn_StructType_struct*) _temp656)->f2; goto
_LL687;} else{ goto _LL688;} _LL688: if(( unsigned int) _temp656 >  4u?*(( int*)
_temp656) ==  Cyc_Absyn_UnionType: 0){ _LL731: _temp730=(( struct Cyc_Absyn_UnionType_struct*)
_temp656)->f2; goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int)
_temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_AnonStructType: 0){ _LL733:
_temp732=(( struct Cyc_Absyn_AnonStructType_struct*) _temp656)->f1; goto _LL691;}
else{ goto _LL692;} _LL692: if(( unsigned int) _temp656 >  4u?*(( int*) _temp656)
==  Cyc_Absyn_AnonUnionType: 0){ _LL735: _temp734=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp656)->f1; goto _LL693;} else{ goto _LL694;} _LL694: if(( unsigned int)
_temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_FnType: 0){ _LL737: _temp736=((
struct Cyc_Absyn_FnType_struct*) _temp656)->f1; _LL749: _temp748= _temp736.tvars;
goto _LL747; _LL747: _temp746= _temp736.effect; goto _LL745; _LL745: _temp744=(
void*) _temp736.ret_typ; goto _LL743; _LL743: _temp742= _temp736.args; goto
_LL741; _LL741: _temp740= _temp736.cyc_varargs; goto _LL739; _LL739: _temp738=
_temp736.rgn_po; goto _LL695;} else{ goto _LL696;} _LL696: if( _temp656 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL697;} else{ goto _LL698;} _LL698: if((
unsigned int) _temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_AccessEff: 0){
goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int) _temp656 >  4u?*((
int*) _temp656) ==  Cyc_Absyn_JoinEff: 0){ goto _LL701;} else{ goto _LL702;}
_LL702: if(( unsigned int) _temp656 >  4u?*(( int*) _temp656) ==  Cyc_Absyn_RgnsEff:
0){ _LL751: _temp750=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp656)->f1;
goto _LL703;} else{ goto _LL704;} _LL704: if(( unsigned int) _temp656 >  4u?*((
int*) _temp656) ==  Cyc_Absyn_TypedefType: 0){ goto _LL705;} else{ goto _LL657;}
_LL659: goto _LL661; _LL661: goto _LL663; _LL663: goto _LL665; _LL665: goto
_LL667; _LL667: goto _LL669; _LL669: return Cyc_Absyn_empty_effect; _LL671: goto
_LL673; _LL673: { void* _temp752= Cyc_Tcutil_typ_kind( t); _LL754: if( _temp752
== ( void*) Cyc_Absyn_RgnKind){ goto _LL755;} else{ goto _LL756;} _LL756: if(
_temp752 == ( void*) Cyc_Absyn_EffKind){ goto _LL757;} else{ goto _LL758;}
_LL758: goto _LL759; _LL755: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp760=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp760[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp761; _temp761.tag= Cyc_Absyn_AccessEff;
_temp761.f1=( void*) t; _temp761;}); _temp760;}); _LL757: return t; _LL759:
return( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp762=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp762[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp763; _temp763.tag= Cyc_Absyn_RgnsEff; _temp763.f1=( void*) t; _temp763;});
_temp762;}); _LL753:;} _LL675: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp764=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp764[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp765; _temp765.tag= Cyc_Absyn_AccessEff;
_temp765.f1=( void*) _temp706; _temp765;}); _temp764;}); _LL677: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp768=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp768->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp769=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp769[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp770; _temp770.tag= Cyc_Absyn_AccessEff;
_temp770.f1=( void*) _temp710; _temp770;}); _temp769;})); _temp768->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp712); _temp768;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp766=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp766[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp767; _temp767.tag= Cyc_Absyn_JoinEff; _temp767.f1= ts; _temp767;});
_temp766;}));} _LL679: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp771=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp771[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp772; _temp772.tag= Cyc_Absyn_JoinEff;
_temp772.f1=({ void* _temp773[ 2u]; _temp773[ 1u]= Cyc_Tcutil_rgns_of( _temp718);
_temp773[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp774=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp774[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp775; _temp775.tag= Cyc_Absyn_AccessEff;
_temp775.f1=( void*) _temp716; _temp775;}); _temp774;}); Cyc_List_list( _tag_arr(
_temp773, sizeof( void*), 2u));}); _temp772;}); _temp771;})); _LL681: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp720)); _LL683: { struct Cyc_List_List* _temp776= 0;
for( 0; _temp722 !=  0; _temp722= _temp722->tl){ _temp776=({ struct Cyc_List_List*
_temp777=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp777->hd=( void*)(*(( struct _tuple4*) _temp722->hd)).f2; _temp777->tl=
_temp776; _temp777;});} _temp726= _temp776; goto _LL685;} _LL685: _temp728=
_temp726; goto _LL687; _LL687: _temp730= _temp728; goto _LL689; _LL689: return
Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp778=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp778[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp779; _temp779.tag= Cyc_Absyn_JoinEff;
_temp779.f1= Cyc_List_map( Cyc_Tcutil_rgns_of, _temp730); _temp779;}); _temp778;}));
_LL691: _temp734= _temp732; goto _LL693; _LL693: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp780=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp780[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp781; _temp781.tag= Cyc_Absyn_JoinEff; _temp781.f1=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_rgns_of_field, _temp734); _temp781;}); _temp780;})); _LL695: { void*
_temp782= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)( struct _tuple5*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst,
_temp748),( void*)(( struct Cyc_Core_Opt*) _check_null( _temp746))->v); return
Cyc_Tcutil_normalize_effect( _temp782);} _LL697: return Cyc_Absyn_empty_effect;
_LL699: goto _LL701; _LL701: return t; _LL703: return Cyc_Tcutil_rgns_of(
_temp750); _LL705: return({ void* _temp783[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp783, sizeof( void*), 0u));});
_LL657:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp784= e; struct Cyc_List_List* _temp792; struct Cyc_List_List**
_temp794; void* _temp795; _LL786: if(( unsigned int) _temp784 >  4u?*(( int*)
_temp784) ==  Cyc_Absyn_JoinEff: 0){ _LL793: _temp792=(( struct Cyc_Absyn_JoinEff_struct*)
_temp784)->f1; _temp794=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp784)->f1; goto _LL787;} else{ goto _LL788;} _LL788: if(( unsigned int)
_temp784 >  4u?*(( int*) _temp784) ==  Cyc_Absyn_RgnsEff: 0){ _LL796: _temp795=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp784)->f1; goto _LL789;} else{
goto _LL790;} _LL790: goto _LL791; _LL787: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp794; for( 0; effs !=  0; effs= effs->tl){ void* _temp797=( void*)
effs->hd;( void*)( effs->hd=( void*) Cyc_Tcutil_compress( Cyc_Tcutil_normalize_effect(
_temp797)));{ void* _temp798=( void*) effs->hd; _LL800: if(( unsigned int)
_temp798 >  4u?*(( int*) _temp798) ==  Cyc_Absyn_JoinEff: 0){ goto _LL801;}
else{ goto _LL802;} _LL802: goto _LL803; _LL801: nested_join= 1; goto _LL799;
_LL803: goto _LL799; _LL799:;}}} if( ! nested_join){ return e;}{ struct Cyc_List_List*
effects= 0;{ struct Cyc_List_List* effs=* _temp794; for( 0; effs !=  0; effs=
effs->tl){ void* _temp804= Cyc_Tcutil_compress(( void*) effs->hd); struct Cyc_List_List*
_temp812; void* _temp814; _LL806: if(( unsigned int) _temp804 >  4u?*(( int*)
_temp804) ==  Cyc_Absyn_JoinEff: 0){ _LL813: _temp812=(( struct Cyc_Absyn_JoinEff_struct*)
_temp804)->f1; goto _LL807;} else{ goto _LL808;} _LL808: if(( unsigned int)
_temp804 >  4u?*(( int*) _temp804) ==  Cyc_Absyn_AccessEff: 0){ _LL815: _temp814=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp804)->f1; if( _temp814 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL809;} else{ goto _LL810;}} else{ goto _LL810;}
_LL810: goto _LL811; _LL807: effects= Cyc_List_revappend( _temp812, effects);
goto _LL805; _LL809: goto _LL805; _LL811: effects=({ struct Cyc_List_List*
_temp816=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp816->hd=( void*) _temp804; _temp816->tl= effects; _temp816;}); goto _LL805;
_LL805:;}}* _temp794= Cyc_List_imp_rev( effects); return e;}} _LL789: return Cyc_Tcutil_rgns_of(
_temp795); _LL791: return e; _LL785:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp817= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp825; struct Cyc_List_List _temp827; struct Cyc_List_List* _temp828; void*
_temp830; struct Cyc_Core_Opt* _temp832; struct Cyc_Core_Opt* _temp834; _LL819:
if(( unsigned int) _temp817 >  4u?*(( int*) _temp817) ==  Cyc_Absyn_JoinEff: 0){
_LL826: _temp825=(( struct Cyc_Absyn_JoinEff_struct*) _temp817)->f1; if(
_temp825 ==  0){ goto _LL821;} else{ _temp827=* _temp825; _LL831: _temp830=(
void*) _temp827.hd; goto _LL829; _LL829: _temp828= _temp827.tl; goto _LL820;}}
else{ goto _LL821;} _LL821: if(( unsigned int) _temp817 >  4u?*(( int*) _temp817)
==  Cyc_Absyn_Evar: 0){ _LL835: _temp834=(( struct Cyc_Absyn_Evar_struct*)
_temp817)->f1; goto _LL833; _LL833: _temp832=(( struct Cyc_Absyn_Evar_struct*)
_temp817)->f4; goto _LL822;} else{ goto _LL823;} _LL823: goto _LL824; _LL820: {
void* _temp836= Cyc_Tcutil_compress( _temp830); struct Cyc_Core_Opt* _temp842;
_LL838: if(( unsigned int) _temp836 >  4u?*(( int*) _temp836) ==  Cyc_Absyn_Evar:
0){ _LL843: _temp842=(( struct Cyc_Absyn_Evar_struct*) _temp836)->f4; goto
_LL839;} else{ goto _LL840;} _LL840: goto _LL841; _LL839: return({ struct
_tuple9* _temp844=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp844->f1= _temp830; _temp844->f2= _temp828; _temp844->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp842))->v; _temp844;}); _LL841: return 0;
_LL837:;} _LL822: if( _temp834 ==  0? 1:( void*) _temp834->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp845[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp845, sizeof( void*), 0u));});} return({ struct _tuple9*
_temp846=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp846->f1= t;
_temp846->f2= 0; _temp846->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp832))->v; _temp846;}); _LL824: return 0; _LL818:;} static
struct Cyc_Core_Opt Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp847=({ struct Cyc_Absyn_FnType_struct*
_temp848=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp848[ 0]=({ struct Cyc_Absyn_FnType_struct _temp849; _temp849.tag= Cyc_Absyn_FnType;
_temp849.f1=({ struct Cyc_Absyn_FnInfo _temp850; _temp850.tvars= 0; _temp850.effect=({
struct Cyc_Core_Opt* _temp851=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp851->v=( void*) eff; _temp851;}); _temp850.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp850.args= 0; _temp850.c_varargs= 0; _temp850.cyc_varargs=
0; _temp850.rgn_po= 0; _temp850.attributes= 0; _temp850;}); _temp849;});
_temp848;}); return Cyc_Absyn_atb_typ(( void*) _temp847,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp852= Cyc_Tcutil_compress( e); void*
_temp864; struct Cyc_List_List* _temp866; void* _temp868; struct Cyc_Core_Opt*
_temp870; struct Cyc_Core_Opt* _temp872; struct Cyc_Core_Opt** _temp874; struct
Cyc_Core_Opt* _temp875; _LL854: if(( unsigned int) _temp852 >  4u?*(( int*)
_temp852) ==  Cyc_Absyn_AccessEff: 0){ _LL865: _temp864=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp852)->f1; goto _LL855;} else{ goto _LL856;} _LL856: if(( unsigned int)
_temp852 >  4u?*(( int*) _temp852) ==  Cyc_Absyn_JoinEff: 0){ _LL867: _temp866=((
struct Cyc_Absyn_JoinEff_struct*) _temp852)->f1; goto _LL857;} else{ goto _LL858;}
_LL858: if(( unsigned int) _temp852 >  4u?*(( int*) _temp852) ==  Cyc_Absyn_RgnsEff:
0){ _LL869: _temp868=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp852)->f1;
goto _LL859;} else{ goto _LL860;} _LL860: if(( unsigned int) _temp852 >  4u?*((
int*) _temp852) ==  Cyc_Absyn_Evar: 0){ _LL876: _temp875=(( struct Cyc_Absyn_Evar_struct*)
_temp852)->f1; goto _LL873; _LL873: _temp872=(( struct Cyc_Absyn_Evar_struct*)
_temp852)->f2; _temp874=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp852)->f2; goto _LL871; _LL871: _temp870=(( struct Cyc_Absyn_Evar_struct*)
_temp852)->f4; goto _LL861;} else{ goto _LL862;} _LL862: goto _LL863; _LL855:
if( constrain){ return Cyc_Tcutil_unify( r, _temp864);} _temp864= Cyc_Tcutil_compress(
_temp864); if( r ==  _temp864){ return 1;}{ struct _tuple8 _temp878=({ struct
_tuple8 _temp877; _temp877.f1= r; _temp877.f2= _temp864; _temp877;}); void*
_temp884; struct Cyc_Absyn_Tvar* _temp886; void* _temp888; struct Cyc_Absyn_Tvar*
_temp890; _LL880: _LL889: _temp888= _temp878.f1; if(( unsigned int) _temp888 > 
4u?*(( int*) _temp888) ==  Cyc_Absyn_VarType: 0){ _LL891: _temp890=(( struct Cyc_Absyn_VarType_struct*)
_temp888)->f1; goto _LL885;} else{ goto _LL882;} _LL885: _temp884= _temp878.f2;
if(( unsigned int) _temp884 >  4u?*(( int*) _temp884) ==  Cyc_Absyn_VarType: 0){
_LL887: _temp886=(( struct Cyc_Absyn_VarType_struct*) _temp884)->f1; goto _LL881;}
else{ goto _LL882;} _LL882: goto _LL883; _LL881: return Cyc_Absyn_tvar_cmp(
_temp890, _temp886) ==  0; _LL883: return 0; _LL879:;} _LL857: for( 0; _temp866
!=  0; _temp866= _temp866->tl){ if( Cyc_Tcutil_region_in_effect( constrain, r,(
void*) _temp866->hd)){ return 1;}} return 0; _LL859: { void* _temp892= Cyc_Tcutil_rgns_of(
_temp868); void* _temp898; _LL894: if(( unsigned int) _temp892 >  4u?*(( int*)
_temp892) ==  Cyc_Absyn_RgnsEff: 0){ _LL899: _temp898=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp892)->f1; goto _LL895;} else{ goto _LL896;} _LL896: goto _LL897; _LL895:
if( ! constrain){ return 0;}{ void* _temp900= Cyc_Tcutil_compress( _temp898);
struct Cyc_Core_Opt* _temp906; struct Cyc_Core_Opt* _temp908; struct Cyc_Core_Opt**
_temp910; struct Cyc_Core_Opt* _temp911; _LL902: if(( unsigned int) _temp900 > 
4u?*(( int*) _temp900) ==  Cyc_Absyn_Evar: 0){ _LL912: _temp911=(( struct Cyc_Absyn_Evar_struct*)
_temp900)->f1; goto _LL909; _LL909: _temp908=(( struct Cyc_Absyn_Evar_struct*)
_temp900)->f2; _temp910=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp900)->f2; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_Evar_struct*)
_temp900)->f4; goto _LL903;} else{ goto _LL904;} _LL904: goto _LL905; _LL903: {
void* _temp913= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp906); Cyc_Tcutil_occurs( _temp913,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp906))->v, r);{ void* _temp914= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp916=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp916[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp917; _temp917.tag= Cyc_Absyn_JoinEff; _temp917.f1=({ void* _temp918[ 2u];
_temp918[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp919=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp919[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp920; _temp920.tag= Cyc_Absyn_AccessEff;
_temp920.f1=( void*) r; _temp920;}); _temp919;}); _temp918[ 0u]= _temp913; Cyc_List_list(
_tag_arr( _temp918, sizeof( void*), 2u));}); _temp917;}); _temp916;}));*
_temp910=({ struct Cyc_Core_Opt* _temp915=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp915->v=( void*) _temp914; _temp915;});
return 1;}} _LL905: return 0; _LL901:;} _LL897: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp892); _LL893:;} _LL861: if( _temp875 ==  0? 1:( void*)
_temp875->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp921[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp921, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp922= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp870); Cyc_Tcutil_occurs( _temp922,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp870))->v, r);{ struct Cyc_Absyn_JoinEff_struct*
_temp923=({ struct Cyc_Absyn_JoinEff_struct* _temp925=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp925[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp926; _temp926.tag= Cyc_Absyn_JoinEff; _temp926.f1=({ struct Cyc_List_List*
_temp927=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp927->hd=( void*) _temp922; _temp927->tl=({ struct Cyc_List_List* _temp928=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp928->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp929=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp929[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp930; _temp930.tag= Cyc_Absyn_AccessEff; _temp930.f1=(
void*) r; _temp930;}); _temp929;})); _temp928->tl= 0; _temp928;}); _temp927;});
_temp926;}); _temp925;});* _temp874=({ struct Cyc_Core_Opt* _temp924=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp924->v=( void*)((
void*) _temp923); _temp924;}); return 1;}} _LL863: return 0; _LL853:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp931= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp943;
void* _temp945; struct Cyc_Core_Opt* _temp947; struct Cyc_Core_Opt* _temp949;
struct Cyc_Core_Opt** _temp951; struct Cyc_Core_Opt* _temp952; _LL933: if((
unsigned int) _temp931 >  4u?*(( int*) _temp931) ==  Cyc_Absyn_AccessEff: 0){
goto _LL934;} else{ goto _LL935;} _LL935: if(( unsigned int) _temp931 >  4u?*((
int*) _temp931) ==  Cyc_Absyn_JoinEff: 0){ _LL944: _temp943=(( struct Cyc_Absyn_JoinEff_struct*)
_temp931)->f1; goto _LL936;} else{ goto _LL937;} _LL937: if(( unsigned int)
_temp931 >  4u?*(( int*) _temp931) ==  Cyc_Absyn_RgnsEff: 0){ _LL946: _temp945=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp931)->f1; goto _LL938;} else{
goto _LL939;} _LL939: if(( unsigned int) _temp931 >  4u?*(( int*) _temp931) == 
Cyc_Absyn_Evar: 0){ _LL953: _temp952=(( struct Cyc_Absyn_Evar_struct*) _temp931)->f1;
goto _LL950; _LL950: _temp949=(( struct Cyc_Absyn_Evar_struct*) _temp931)->f2;
_temp951=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp931)->f2;
goto _LL948; _LL948: _temp947=(( struct Cyc_Absyn_Evar_struct*) _temp931)->f4;
goto _LL940;} else{ goto _LL941;} _LL941: goto _LL942; _LL934: return 0; _LL936:
for( 0; _temp943 !=  0; _temp943= _temp943->tl){ if( Cyc_Tcutil_type_in_effect(
constrain, t,( void*) _temp943->hd)){ return 1;}} return 0; _LL938: _temp945=
Cyc_Tcutil_compress( _temp945); if( t ==  _temp945){ return 1;} if( constrain){
return Cyc_Tcutil_unify( t, _temp945);}{ void* _temp954= Cyc_Tcutil_rgns_of( t);
void* _temp960; _LL956: if(( unsigned int) _temp954 >  4u?*(( int*) _temp954) == 
Cyc_Absyn_RgnsEff: 0){ _LL961: _temp960=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp954)->f1; goto _LL957;} else{ goto _LL958;} _LL958: goto _LL959; _LL957: {
struct _tuple8 _temp963=({ struct _tuple8 _temp962; _temp962.f1= t; _temp962.f2=
Cyc_Tcutil_compress( _temp960); _temp962;}); void* _temp969; struct Cyc_Absyn_Tvar*
_temp971; void* _temp973; struct Cyc_Absyn_Tvar* _temp975; _LL965: _LL974:
_temp973= _temp963.f1; if(( unsigned int) _temp973 >  4u?*(( int*) _temp973) == 
Cyc_Absyn_VarType: 0){ _LL976: _temp975=(( struct Cyc_Absyn_VarType_struct*)
_temp973)->f1; goto _LL970;} else{ goto _LL967;} _LL970: _temp969= _temp963.f2;
if(( unsigned int) _temp969 >  4u?*(( int*) _temp969) ==  Cyc_Absyn_VarType: 0){
_LL972: _temp971=(( struct Cyc_Absyn_VarType_struct*) _temp969)->f1; goto _LL966;}
else{ goto _LL967;} _LL967: goto _LL968; _LL966: return Cyc_Tcutil_unify( t,
_temp960); _LL968: return t ==  _temp960; _LL964:;} _LL959: return Cyc_Tcutil_type_in_effect(
constrain, t, _temp954); _LL955:;} _LL940: if( _temp952 ==  0? 1:( void*)
_temp952->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp977[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp977, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp978= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp947); Cyc_Tcutil_occurs( _temp978,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp947))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp979=({ struct Cyc_Absyn_JoinEff_struct* _temp981=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp981[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp982; _temp982.tag= Cyc_Absyn_JoinEff; _temp982.f1=({ struct Cyc_List_List*
_temp983=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp983->hd=( void*) _temp978; _temp983->tl=({ struct Cyc_List_List* _temp984=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp984->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp985=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp985[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp986; _temp986.tag= Cyc_Absyn_RgnsEff; _temp986.f1=( void*) t; _temp986;});
_temp985;})); _temp984->tl= 0; _temp984;}); _temp983;}); _temp982;}); _temp981;});*
_temp951=({ struct Cyc_Core_Opt* _temp980=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp980->v=( void*)(( void*) _temp979); _temp980;});
return 1;}} _LL942: return 0; _LL932:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp987= e; struct Cyc_Absyn_Tvar* _temp999; struct Cyc_List_List*
_temp1001; void* _temp1003; struct Cyc_Core_Opt* _temp1005; struct Cyc_Core_Opt*
_temp1007; struct Cyc_Core_Opt** _temp1009; struct Cyc_Core_Opt* _temp1010;
_LL989: if(( unsigned int) _temp987 >  4u?*(( int*) _temp987) ==  Cyc_Absyn_VarType:
0){ _LL1000: _temp999=(( struct Cyc_Absyn_VarType_struct*) _temp987)->f1; goto
_LL990;} else{ goto _LL991;} _LL991: if(( unsigned int) _temp987 >  4u?*(( int*)
_temp987) ==  Cyc_Absyn_JoinEff: 0){ _LL1002: _temp1001=(( struct Cyc_Absyn_JoinEff_struct*)
_temp987)->f1; goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp987 >  4u?*(( int*) _temp987) ==  Cyc_Absyn_RgnsEff: 0){ _LL1004: _temp1003=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp987)->f1; goto _LL994;} else{
goto _LL995;} _LL995: if(( unsigned int) _temp987 >  4u?*(( int*) _temp987) == 
Cyc_Absyn_Evar: 0){ _LL1011: _temp1010=(( struct Cyc_Absyn_Evar_struct*)
_temp987)->f1; goto _LL1008; _LL1008: _temp1007=(( struct Cyc_Absyn_Evar_struct*)
_temp987)->f2; _temp1009=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp987)->f2; goto _LL1006; _LL1006: _temp1005=(( struct Cyc_Absyn_Evar_struct*)
_temp987)->f4; goto _LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL990:
return Cyc_Absyn_tvar_cmp( v, _temp999) ==  0; _LL992: for( 0; _temp1001 !=  0;
_temp1001= _temp1001->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*) _temp1001->hd)){ return 1;}} return 0; _LL994: { void* _temp1012= Cyc_Tcutil_rgns_of(
_temp1003); void* _temp1018; _LL1014: if(( unsigned int) _temp1012 >  4u?*(( int*)
_temp1012) ==  Cyc_Absyn_RgnsEff: 0){ _LL1019: _temp1018=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1012)->f1; goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017;
_LL1015: if( ! constrain){ return 0;}{ void* _temp1020= Cyc_Tcutil_compress(
_temp1018); struct Cyc_Core_Opt* _temp1026; struct Cyc_Core_Opt* _temp1028;
struct Cyc_Core_Opt** _temp1030; struct Cyc_Core_Opt* _temp1031; _LL1022: if((
unsigned int) _temp1020 >  4u?*(( int*) _temp1020) ==  Cyc_Absyn_Evar: 0){
_LL1032: _temp1031=(( struct Cyc_Absyn_Evar_struct*) _temp1020)->f1; goto
_LL1029; _LL1029: _temp1028=(( struct Cyc_Absyn_Evar_struct*) _temp1020)->f2;
_temp1030=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1020)->f2;
goto _LL1027; _LL1027: _temp1026=(( struct Cyc_Absyn_Evar_struct*) _temp1020)->f4;
goto _LL1023;} else{ goto _LL1024;} _LL1024: goto _LL1025; _LL1023: { void*
_temp1033= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp1026);
if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1026))->v, v)){
return 0;}{ void* _temp1034= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1036=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1036[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1037; _temp1037.tag= Cyc_Absyn_JoinEff;
_temp1037.f1=({ void* _temp1038[ 2u]; _temp1038[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1039=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1039[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1040; _temp1040.tag= Cyc_Absyn_VarType;
_temp1040.f1= v; _temp1040;}); _temp1039;}); _temp1038[ 0u]= _temp1033; Cyc_List_list(
_tag_arr( _temp1038, sizeof( void*), 2u));}); _temp1037;}); _temp1036;}));*
_temp1030=({ struct Cyc_Core_Opt* _temp1035=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1035->v=( void*) _temp1034; _temp1035;});
return 1;}} _LL1025: return 0; _LL1021:;} _LL1017: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp1012); _LL1013:;} _LL996: if( _temp1010 ==  0? 1:( void*)
_temp1010->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp1041[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp1041, sizeof( void*), 0u));});}{
void* _temp1042= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp1005); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1005))->v, v)){
return 0;}{ struct Cyc_Absyn_JoinEff_struct* _temp1043=({ struct Cyc_Absyn_JoinEff_struct*
_temp1045=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1045[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1046; _temp1046.tag= Cyc_Absyn_JoinEff;
_temp1046.f1=({ struct Cyc_List_List* _temp1047=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1047->hd=( void*) _temp1042;
_temp1047->tl=({ struct Cyc_List_List* _temp1048=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1048->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp1049=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1049[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1050; _temp1050.tag= Cyc_Absyn_VarType; _temp1050.f1=
v; _temp1050;}); _temp1049;})); _temp1048->tl= 0; _temp1048;}); _temp1047;});
_temp1046;}); _temp1045;});* _temp1009=({ struct Cyc_Core_Opt* _temp1044=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1044->v=(
void*)(( void*) _temp1043); _temp1044;}); return 1;}} _LL998: return 0; _LL988:;}}
static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp1051= e; struct Cyc_List_List* _temp1061; void* _temp1063;
_LL1053: if(( unsigned int) _temp1051 >  4u?*(( int*) _temp1051) ==  Cyc_Absyn_JoinEff:
0){ _LL1062: _temp1061=(( struct Cyc_Absyn_JoinEff_struct*) _temp1051)->f1; goto
_LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int) _temp1051 >  4u?*((
int*) _temp1051) ==  Cyc_Absyn_RgnsEff: 0){ _LL1064: _temp1063=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp1051)->f1; goto _LL1056;} else{ goto _LL1057;}
_LL1057: if(( unsigned int) _temp1051 >  4u?*(( int*) _temp1051) ==  Cyc_Absyn_Evar:
0){ goto _LL1058;} else{ goto _LL1059;} _LL1059: goto _LL1060; _LL1054: for( 0;
_temp1061 !=  0; _temp1061= _temp1061->tl){ if( Cyc_Tcutil_evar_in_effect( evar,(
void*) _temp1061->hd)){ return 1;}} return 0; _LL1056: { void* _temp1065= Cyc_Tcutil_rgns_of(
_temp1063); void* _temp1071; _LL1067: if(( unsigned int) _temp1065 >  4u?*(( int*)
_temp1065) ==  Cyc_Absyn_RgnsEff: 0){ _LL1072: _temp1071=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1065)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: goto _LL1070;
_LL1068: return 0; _LL1070: return Cyc_Tcutil_evar_in_effect( evar, _temp1065);
_LL1066:;} _LL1058: return evar ==  e; _LL1060: return 0; _LL1052:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1073= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1087; void* _temp1089; struct Cyc_Absyn_Tvar*
_temp1091; void* _temp1093; struct Cyc_Core_Opt* _temp1095; struct Cyc_Core_Opt*
_temp1097; struct Cyc_Core_Opt** _temp1099; _LL1075: if(( unsigned int)
_temp1073 >  4u?*(( int*) _temp1073) ==  Cyc_Absyn_JoinEff: 0){ _LL1088:
_temp1087=(( struct Cyc_Absyn_JoinEff_struct*) _temp1073)->f1; goto _LL1076;}
else{ goto _LL1077;} _LL1077: if(( unsigned int) _temp1073 >  4u?*(( int*)
_temp1073) ==  Cyc_Absyn_AccessEff: 0){ _LL1090: _temp1089=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1073)->f1; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(( unsigned int)
_temp1073 >  4u?*(( int*) _temp1073) ==  Cyc_Absyn_VarType: 0){ _LL1092:
_temp1091=(( struct Cyc_Absyn_VarType_struct*) _temp1073)->f1; goto _LL1080;}
else{ goto _LL1081;} _LL1081: if(( unsigned int) _temp1073 >  4u?*(( int*)
_temp1073) ==  Cyc_Absyn_RgnsEff: 0){ _LL1094: _temp1093=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1073)->f1; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(( unsigned int)
_temp1073 >  4u?*(( int*) _temp1073) ==  Cyc_Absyn_Evar: 0){ _LL1098: _temp1097=((
struct Cyc_Absyn_Evar_struct*) _temp1073)->f2; _temp1099=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1073)->f2; goto _LL1096; _LL1096: _temp1095=((
struct Cyc_Absyn_Evar_struct*) _temp1073)->f4; goto _LL1084;} else{ goto _LL1085;}
_LL1085: goto _LL1086; _LL1076: for( 0; _temp1087 !=  0; _temp1087= _temp1087->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*) _temp1087->hd, e2)){
return 0;}} return 1; _LL1078: return( Cyc_Tcutil_region_in_effect( 0, _temp1089,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp1089, e2))? 1: Cyc_Tcutil_unify(
_temp1089,( void*) Cyc_Absyn_HeapRgn); _LL1080: return Cyc_Tcutil_variable_in_effect(
0, _temp1091, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1091, e2); _LL1082: {
void* _temp1100= Cyc_Tcutil_rgns_of( _temp1093); void* _temp1106; _LL1102: if((
unsigned int) _temp1100 >  4u?*(( int*) _temp1100) ==  Cyc_Absyn_RgnsEff: 0){
_LL1107: _temp1106=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1100)->f1;
goto _LL1103;} else{ goto _LL1104;} _LL1104: goto _LL1105; _LL1103: return( Cyc_Tcutil_type_in_effect(
0, _temp1106, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1106, e2))? 1: Cyc_Tcutil_unify(
_temp1106, Cyc_Absyn_sint_t); _LL1105: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1100, e2); _LL1101:;} _LL1084: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1099=({ struct Cyc_Core_Opt* _temp1108=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1108->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1109=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1109[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1110; _temp1110.tag= Cyc_Absyn_JoinEff; _temp1110.f1=
0; _temp1110;}); _temp1109;})); _temp1108;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1095))->v, e2);*
_temp1099=({ struct Cyc_Core_Opt* _temp1111=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1111->v=( void*) e2; _temp1111;});}} return
1; _LL1086: return({ struct Cyc_Std_String_pa_struct _temp1113; _temp1113.tag=
Cyc_Std_String_pa; _temp1113.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1112[ 1u]={& _temp1113};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1112, sizeof( void*), 1u));}});
_LL1074:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1115=({ struct
_tuple10 _temp1114; _temp1114.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1114.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1114;}); _LL1117: goto _LL1118; _LL1118:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1116:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1= r1->tl){ struct _tuple8 _temp1121; void* _temp1122; void* _temp1124; struct
_tuple8* _temp1119=( struct _tuple8*) r1->hd; _temp1121=* _temp1119; _LL1125:
_temp1124= _temp1121.f1; goto _LL1123; _LL1123: _temp1122= _temp1121.f2; goto
_LL1120; _LL1120: { int found= _temp1124 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2= r2->tl){ struct
_tuple8 _temp1128; void* _temp1129; void* _temp1131; struct _tuple8* _temp1126=(
struct _tuple8*) r2->hd; _temp1128=* _temp1126; _LL1132: _temp1131= _temp1128.f1;
goto _LL1130; _LL1130: _temp1129= _temp1128.f2; goto _LL1127; _LL1127: if( Cyc_Tcutil_unify(
_temp1124, _temp1131)? Cyc_Tcutil_unify( _temp1122, _temp1129): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason=(
struct _tagged_arr) _tag_arr( 0u, 0u, 0u); t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2); if( t1 ==  t2){ return;}{ void* _temp1133= t1; struct Cyc_Core_Opt*
_temp1139; struct Cyc_Core_Opt* _temp1141; struct Cyc_Core_Opt** _temp1143;
struct Cyc_Core_Opt* _temp1144; _LL1135: if(( unsigned int) _temp1133 >  4u?*((
int*) _temp1133) ==  Cyc_Absyn_Evar: 0){ _LL1145: _temp1144=(( struct Cyc_Absyn_Evar_struct*)
_temp1133)->f1; goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_Evar_struct*)
_temp1133)->f2; _temp1143=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1133)->f2; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_Evar_struct*)
_temp1133)->f4; goto _LL1136;} else{ goto _LL1137;} _LL1137: goto _LL1138;
_LL1136: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1139))->v, t2);{ void* _temp1146= Cyc_Tcutil_typ_kind( t2);
if( Cyc_Tcutil_kind_leq( _temp1146,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1144))->v)){* _temp1143=({ struct Cyc_Core_Opt* _temp1147=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1147->v=( void*) t2; _temp1147;});
return;} else{{ void* _temp1148= t2; struct Cyc_Core_Opt* _temp1156; struct Cyc_Core_Opt*
_temp1158; struct Cyc_Core_Opt** _temp1160; struct Cyc_Absyn_PtrInfo _temp1162;
_LL1150: if(( unsigned int) _temp1148 >  4u?*(( int*) _temp1148) ==  Cyc_Absyn_Evar:
0){ _LL1159: _temp1158=(( struct Cyc_Absyn_Evar_struct*) _temp1148)->f2;
_temp1160=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1148)->f2;
goto _LL1157; _LL1157: _temp1156=(( struct Cyc_Absyn_Evar_struct*) _temp1148)->f4;
goto _LL1151;} else{ goto _LL1152;} _LL1152: if(( unsigned int) _temp1148 >  4u?*((
int*) _temp1148) ==  Cyc_Absyn_PointerType: 0){ _LL1163: _temp1162=(( struct Cyc_Absyn_PointerType_struct*)
_temp1148)->f1; goto _LL1161;} else{ goto _LL1154;} _LL1161: if(( void*)
_temp1144->v == ( void*) Cyc_Absyn_BoxKind){ goto _LL1153;} else{ goto _LL1154;}
_LL1154: goto _LL1155; _LL1151: { struct Cyc_List_List* _temp1164=( struct Cyc_List_List*)
_temp1139->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1156))->v; for( 0; s2 !=  0; s2= s2->tl){ if( !(( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp1164,(
struct Cyc_Absyn_Tvar*) s2->hd)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if(
Cyc_Tcutil_kind_leq(( void*) _temp1144->v, _temp1146)){* _temp1160=({ struct Cyc_Core_Opt*
_temp1165=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1165->v=( void*) t1; _temp1165;}); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)", sizeof( unsigned char), 25u); goto _LL1149;}
_LL1153: { struct Cyc_Absyn_Conref* _temp1166= Cyc_Absyn_compress_conref(
_temp1162.bounds);{ void* _temp1167=( void*) _temp1166->v; _LL1169: if(
_temp1167 == ( void*) Cyc_Absyn_No_constr){ goto _LL1170;} else{ goto _LL1171;}
_LL1171: goto _LL1172; _LL1170:( void*)( _temp1166->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp1173=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1173[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1174; _temp1174.tag= Cyc_Absyn_Eq_constr;
_temp1174.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1175=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1175[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1176; _temp1176.tag= Cyc_Absyn_Upper_b;
_temp1176.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1176;}); _temp1175;}));
_temp1174;}); _temp1173;})));* _temp1143=({ struct Cyc_Core_Opt* _temp1177=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1177->v=(
void*) t2; _temp1177;}); return; _LL1172: goto _LL1168; _LL1168:;} goto _LL1149;}
_LL1155: goto _LL1149; _LL1149:;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} _LL1138:
goto _LL1134; _LL1134:;}{ struct _tuple8 _temp1179=({ struct _tuple8 _temp1178;
_temp1178.f1= t2; _temp1178.f2= t1; _temp1178;}); void* _temp1235; void*
_temp1237; void* _temp1239; void* _temp1241; struct Cyc_Absyn_Tvar* _temp1243;
void* _temp1245; struct Cyc_Absyn_Tvar* _temp1247; void* _temp1249; struct Cyc_Absyn_Structdecl**
_temp1251; struct Cyc_List_List* _temp1253; struct _tuple1* _temp1255; void*
_temp1257; struct Cyc_Absyn_Structdecl** _temp1259; struct Cyc_List_List*
_temp1261; struct _tuple1* _temp1263; void* _temp1265; struct _tuple1* _temp1267;
void* _temp1269; struct _tuple1* _temp1271; void* _temp1273; struct Cyc_List_List*
_temp1275; void* _temp1277; struct Cyc_List_List* _temp1279; void* _temp1281;
struct Cyc_Absyn_Uniondecl** _temp1283; struct Cyc_List_List* _temp1285; struct
_tuple1* _temp1287; void* _temp1289; struct Cyc_Absyn_Uniondecl** _temp1291;
struct Cyc_List_List* _temp1293; struct _tuple1* _temp1295; void* _temp1297;
struct Cyc_Absyn_TunionInfo _temp1299; void* _temp1301; struct Cyc_List_List*
_temp1303; void* _temp1305; struct Cyc_Absyn_Tuniondecl** _temp1307; struct Cyc_Absyn_Tuniondecl*
_temp1309; void* _temp1310; struct Cyc_Absyn_TunionInfo _temp1312; void*
_temp1314; struct Cyc_List_List* _temp1316; void* _temp1318; struct Cyc_Absyn_Tuniondecl**
_temp1320; struct Cyc_Absyn_Tuniondecl* _temp1322; void* _temp1323; struct Cyc_Absyn_TunionFieldInfo
_temp1325; struct Cyc_List_List* _temp1327; void* _temp1329; struct Cyc_Absyn_Tunionfield*
_temp1331; struct Cyc_Absyn_Tuniondecl* _temp1333; void* _temp1335; struct Cyc_Absyn_TunionFieldInfo
_temp1337; struct Cyc_List_List* _temp1339; void* _temp1341; struct Cyc_Absyn_Tunionfield*
_temp1343; struct Cyc_Absyn_Tuniondecl* _temp1345; void* _temp1347; struct Cyc_Absyn_PtrInfo
_temp1349; struct Cyc_Absyn_Conref* _temp1351; struct Cyc_Absyn_Tqual _temp1353;
struct Cyc_Absyn_Conref* _temp1355; void* _temp1357; void* _temp1359; void*
_temp1361; struct Cyc_Absyn_PtrInfo _temp1363; struct Cyc_Absyn_Conref*
_temp1365; struct Cyc_Absyn_Tqual _temp1367; struct Cyc_Absyn_Conref* _temp1369;
void* _temp1371; void* _temp1373; void* _temp1375; void* _temp1377; void*
_temp1379; void* _temp1381; void* _temp1383; void* _temp1385; void* _temp1387;
void* _temp1389; void* _temp1391; void* _temp1393; void* _temp1395; struct Cyc_Absyn_Exp*
_temp1397; struct Cyc_Absyn_Tqual _temp1399; void* _temp1401; void* _temp1403;
struct Cyc_Absyn_Exp* _temp1405; struct Cyc_Absyn_Tqual _temp1407; void*
_temp1409; void* _temp1411; struct Cyc_Absyn_FnInfo _temp1413; struct Cyc_List_List*
_temp1415; struct Cyc_List_List* _temp1417; struct Cyc_Absyn_VarargInfo*
_temp1419; int _temp1421; struct Cyc_List_List* _temp1423; void* _temp1425;
struct Cyc_Core_Opt* _temp1427; struct Cyc_List_List* _temp1429; void* _temp1431;
struct Cyc_Absyn_FnInfo _temp1433; struct Cyc_List_List* _temp1435; struct Cyc_List_List*
_temp1437; struct Cyc_Absyn_VarargInfo* _temp1439; int _temp1441; struct Cyc_List_List*
_temp1443; void* _temp1445; struct Cyc_Core_Opt* _temp1447; struct Cyc_List_List*
_temp1449; void* _temp1451; struct Cyc_List_List* _temp1453; void* _temp1455;
struct Cyc_List_List* _temp1457; void* _temp1459; struct Cyc_List_List*
_temp1461; void* _temp1463; struct Cyc_List_List* _temp1465; void* _temp1467;
struct Cyc_List_List* _temp1469; void* _temp1471; struct Cyc_List_List*
_temp1473; void* _temp1475; void* _temp1477; void* _temp1479; void* _temp1481;
void* _temp1483; void* _temp1485; void* _temp1487; void* _temp1489; void*
_temp1491; void* _temp1493; void* _temp1495; void* _temp1497; _LL1181: _LL1236:
_temp1235= _temp1179.f1; if(( unsigned int) _temp1235 >  4u?*(( int*) _temp1235)
==  Cyc_Absyn_Evar: 0){ goto _LL1182;} else{ goto _LL1183;} _LL1183: _LL1240:
_temp1239= _temp1179.f1; if( _temp1239 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1238;} else{ goto _LL1185;} _LL1238: _temp1237= _temp1179.f2; if( _temp1237
== ( void*) Cyc_Absyn_VoidType){ goto _LL1184;} else{ goto _LL1185;} _LL1185:
_LL1246: _temp1245= _temp1179.f1; if(( unsigned int) _temp1245 >  4u?*(( int*)
_temp1245) ==  Cyc_Absyn_VarType: 0){ _LL1248: _temp1247=(( struct Cyc_Absyn_VarType_struct*)
_temp1245)->f1; goto _LL1242;} else{ goto _LL1187;} _LL1242: _temp1241=
_temp1179.f2; if(( unsigned int) _temp1241 >  4u?*(( int*) _temp1241) ==  Cyc_Absyn_VarType:
0){ _LL1244: _temp1243=(( struct Cyc_Absyn_VarType_struct*) _temp1241)->f1; goto
_LL1186;} else{ goto _LL1187;} _LL1187: _LL1258: _temp1257= _temp1179.f1; if((
unsigned int) _temp1257 >  4u?*(( int*) _temp1257) ==  Cyc_Absyn_StructType: 0){
_LL1264: _temp1263=(( struct Cyc_Absyn_StructType_struct*) _temp1257)->f1; goto
_LL1262; _LL1262: _temp1261=(( struct Cyc_Absyn_StructType_struct*) _temp1257)->f2;
goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_StructType_struct*)
_temp1257)->f3; goto _LL1250;} else{ goto _LL1189;} _LL1250: _temp1249=
_temp1179.f2; if(( unsigned int) _temp1249 >  4u?*(( int*) _temp1249) ==  Cyc_Absyn_StructType:
0){ _LL1256: _temp1255=(( struct Cyc_Absyn_StructType_struct*) _temp1249)->f1;
goto _LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_StructType_struct*)
_temp1249)->f2; goto _LL1252; _LL1252: _temp1251=(( struct Cyc_Absyn_StructType_struct*)
_temp1249)->f3; goto _LL1188;} else{ goto _LL1189;} _LL1189: _LL1270: _temp1269=
_temp1179.f1; if(( unsigned int) _temp1269 >  4u?*(( int*) _temp1269) ==  Cyc_Absyn_EnumType:
0){ _LL1272: _temp1271=(( struct Cyc_Absyn_EnumType_struct*) _temp1269)->f1;
goto _LL1266;} else{ goto _LL1191;} _LL1266: _temp1265= _temp1179.f2; if((
unsigned int) _temp1265 >  4u?*(( int*) _temp1265) ==  Cyc_Absyn_EnumType: 0){
_LL1268: _temp1267=(( struct Cyc_Absyn_EnumType_struct*) _temp1265)->f1; goto
_LL1190;} else{ goto _LL1191;} _LL1191: _LL1278: _temp1277= _temp1179.f1; if((
unsigned int) _temp1277 >  4u?*(( int*) _temp1277) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1280: _temp1279=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1277)->f1;
goto _LL1274;} else{ goto _LL1193;} _LL1274: _temp1273= _temp1179.f2; if((
unsigned int) _temp1273 >  4u?*(( int*) _temp1273) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1276: _temp1275=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1273)->f1;
goto _LL1192;} else{ goto _LL1193;} _LL1193: _LL1290: _temp1289= _temp1179.f1;
if(( unsigned int) _temp1289 >  4u?*(( int*) _temp1289) ==  Cyc_Absyn_UnionType:
0){ _LL1296: _temp1295=(( struct Cyc_Absyn_UnionType_struct*) _temp1289)->f1;
goto _LL1294; _LL1294: _temp1293=(( struct Cyc_Absyn_UnionType_struct*)
_temp1289)->f2; goto _LL1292; _LL1292: _temp1291=(( struct Cyc_Absyn_UnionType_struct*)
_temp1289)->f3; goto _LL1282;} else{ goto _LL1195;} _LL1282: _temp1281=
_temp1179.f2; if(( unsigned int) _temp1281 >  4u?*(( int*) _temp1281) ==  Cyc_Absyn_UnionType:
0){ _LL1288: _temp1287=(( struct Cyc_Absyn_UnionType_struct*) _temp1281)->f1;
goto _LL1286; _LL1286: _temp1285=(( struct Cyc_Absyn_UnionType_struct*)
_temp1281)->f2; goto _LL1284; _LL1284: _temp1283=(( struct Cyc_Absyn_UnionType_struct*)
_temp1281)->f3; goto _LL1194;} else{ goto _LL1195;} _LL1195: _LL1311: _temp1310=
_temp1179.f1; if(( unsigned int) _temp1310 >  4u?*(( int*) _temp1310) ==  Cyc_Absyn_TunionType:
0){ _LL1313: _temp1312=(( struct Cyc_Absyn_TunionType_struct*) _temp1310)->f1;
_LL1319: _temp1318=( void*) _temp1312.tunion_info; if(*(( int*) _temp1318) == 
Cyc_Absyn_KnownTunion){ _LL1321: _temp1320=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1318)->f1; _temp1322=* _temp1320; goto _LL1317;} else{ goto _LL1197;}
_LL1317: _temp1316= _temp1312.targs; goto _LL1315; _LL1315: _temp1314=( void*)
_temp1312.rgn; goto _LL1298;} else{ goto _LL1197;} _LL1298: _temp1297= _temp1179.f2;
if(( unsigned int) _temp1297 >  4u?*(( int*) _temp1297) ==  Cyc_Absyn_TunionType:
0){ _LL1300: _temp1299=(( struct Cyc_Absyn_TunionType_struct*) _temp1297)->f1;
_LL1306: _temp1305=( void*) _temp1299.tunion_info; if(*(( int*) _temp1305) == 
Cyc_Absyn_KnownTunion){ _LL1308: _temp1307=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1305)->f1; _temp1309=* _temp1307; goto _LL1304;} else{ goto _LL1197;}
_LL1304: _temp1303= _temp1299.targs; goto _LL1302; _LL1302: _temp1301=( void*)
_temp1299.rgn; goto _LL1196;} else{ goto _LL1197;} _LL1197: _LL1336: _temp1335=
_temp1179.f1; if(( unsigned int) _temp1335 >  4u?*(( int*) _temp1335) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1338: _temp1337=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1335)->f1;
_LL1342: _temp1341=( void*) _temp1337.field_info; if(*(( int*) _temp1341) == 
Cyc_Absyn_KnownTunionfield){ _LL1346: _temp1345=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1341)->f1; goto _LL1344; _LL1344: _temp1343=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1341)->f2; goto _LL1340;} else{ goto _LL1199;} _LL1340: _temp1339=
_temp1337.targs; goto _LL1324;} else{ goto _LL1199;} _LL1324: _temp1323=
_temp1179.f2; if(( unsigned int) _temp1323 >  4u?*(( int*) _temp1323) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1326: _temp1325=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1323)->f1;
_LL1330: _temp1329=( void*) _temp1325.field_info; if(*(( int*) _temp1329) == 
Cyc_Absyn_KnownTunionfield){ _LL1334: _temp1333=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1329)->f1; goto _LL1332; _LL1332: _temp1331=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1329)->f2; goto _LL1328;} else{ goto _LL1199;} _LL1328: _temp1327=
_temp1325.targs; goto _LL1198;} else{ goto _LL1199;} _LL1199: _LL1362: _temp1361=
_temp1179.f1; if(( unsigned int) _temp1361 >  4u?*(( int*) _temp1361) ==  Cyc_Absyn_PointerType:
0){ _LL1364: _temp1363=(( struct Cyc_Absyn_PointerType_struct*) _temp1361)->f1;
_LL1374: _temp1373=( void*) _temp1363.elt_typ; goto _LL1372; _LL1372: _temp1371=(
void*) _temp1363.rgn_typ; goto _LL1370; _LL1370: _temp1369= _temp1363.nullable;
goto _LL1368; _LL1368: _temp1367= _temp1363.tq; goto _LL1366; _LL1366: _temp1365=
_temp1363.bounds; goto _LL1348;} else{ goto _LL1201;} _LL1348: _temp1347=
_temp1179.f2; if(( unsigned int) _temp1347 >  4u?*(( int*) _temp1347) ==  Cyc_Absyn_PointerType:
0){ _LL1350: _temp1349=(( struct Cyc_Absyn_PointerType_struct*) _temp1347)->f1;
_LL1360: _temp1359=( void*) _temp1349.elt_typ; goto _LL1358; _LL1358: _temp1357=(
void*) _temp1349.rgn_typ; goto _LL1356; _LL1356: _temp1355= _temp1349.nullable;
goto _LL1354; _LL1354: _temp1353= _temp1349.tq; goto _LL1352; _LL1352: _temp1351=
_temp1349.bounds; goto _LL1200;} else{ goto _LL1201;} _LL1201: _LL1382:
_temp1381= _temp1179.f1; if(( unsigned int) _temp1381 >  4u?*(( int*) _temp1381)
==  Cyc_Absyn_IntType: 0){ _LL1386: _temp1385=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1381)->f1; goto _LL1384; _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1381)->f2; goto _LL1376;} else{ goto _LL1203;} _LL1376: _temp1375=
_temp1179.f2; if(( unsigned int) _temp1375 >  4u?*(( int*) _temp1375) ==  Cyc_Absyn_IntType:
0){ _LL1380: _temp1379=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1375)->f1;
goto _LL1378; _LL1378: _temp1377=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1375)->f2; goto _LL1202;} else{ goto _LL1203;} _LL1203: _LL1390: _temp1389=
_temp1179.f1; if( _temp1389 == ( void*) Cyc_Absyn_FloatType){ goto _LL1388;}
else{ goto _LL1205;} _LL1388: _temp1387= _temp1179.f2; if( _temp1387 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1204;} else{ goto _LL1205;} _LL1205: _LL1394:
_temp1393= _temp1179.f1; if( _temp1393 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1392;} else{ goto _LL1207;} _LL1392: _temp1391= _temp1179.f2; if( _temp1391
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1206;} else{ goto _LL1207;} _LL1207:
_LL1404: _temp1403= _temp1179.f1; if(( unsigned int) _temp1403 >  4u?*(( int*)
_temp1403) ==  Cyc_Absyn_ArrayType: 0){ _LL1410: _temp1409=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1403)->f1; goto _LL1408; _LL1408: _temp1407=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1403)->f2; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1403)->f3; goto _LL1396;} else{ goto _LL1209;} _LL1396: _temp1395=
_temp1179.f2; if(( unsigned int) _temp1395 >  4u?*(( int*) _temp1395) ==  Cyc_Absyn_ArrayType:
0){ _LL1402: _temp1401=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1395)->f1;
goto _LL1400; _LL1400: _temp1399=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1395)->f2; goto _LL1398; _LL1398: _temp1397=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1395)->f3; goto _LL1208;} else{ goto _LL1209;} _LL1209: _LL1432: _temp1431=
_temp1179.f1; if(( unsigned int) _temp1431 >  4u?*(( int*) _temp1431) ==  Cyc_Absyn_FnType:
0){ _LL1434: _temp1433=(( struct Cyc_Absyn_FnType_struct*) _temp1431)->f1;
_LL1450: _temp1449= _temp1433.tvars; goto _LL1448; _LL1448: _temp1447= _temp1433.effect;
goto _LL1446; _LL1446: _temp1445=( void*) _temp1433.ret_typ; goto _LL1444;
_LL1444: _temp1443= _temp1433.args; goto _LL1442; _LL1442: _temp1441= _temp1433.c_varargs;
goto _LL1440; _LL1440: _temp1439= _temp1433.cyc_varargs; goto _LL1438; _LL1438:
_temp1437= _temp1433.rgn_po; goto _LL1436; _LL1436: _temp1435= _temp1433.attributes;
goto _LL1412;} else{ goto _LL1211;} _LL1412: _temp1411= _temp1179.f2; if((
unsigned int) _temp1411 >  4u?*(( int*) _temp1411) ==  Cyc_Absyn_FnType: 0){
_LL1414: _temp1413=(( struct Cyc_Absyn_FnType_struct*) _temp1411)->f1; _LL1430:
_temp1429= _temp1413.tvars; goto _LL1428; _LL1428: _temp1427= _temp1413.effect;
goto _LL1426; _LL1426: _temp1425=( void*) _temp1413.ret_typ; goto _LL1424;
_LL1424: _temp1423= _temp1413.args; goto _LL1422; _LL1422: _temp1421= _temp1413.c_varargs;
goto _LL1420; _LL1420: _temp1419= _temp1413.cyc_varargs; goto _LL1418; _LL1418:
_temp1417= _temp1413.rgn_po; goto _LL1416; _LL1416: _temp1415= _temp1413.attributes;
goto _LL1210;} else{ goto _LL1211;} _LL1211: _LL1456: _temp1455= _temp1179.f1;
if(( unsigned int) _temp1455 >  4u?*(( int*) _temp1455) ==  Cyc_Absyn_TupleType:
0){ _LL1458: _temp1457=(( struct Cyc_Absyn_TupleType_struct*) _temp1455)->f1;
goto _LL1452;} else{ goto _LL1213;} _LL1452: _temp1451= _temp1179.f2; if((
unsigned int) _temp1451 >  4u?*(( int*) _temp1451) ==  Cyc_Absyn_TupleType: 0){
_LL1454: _temp1453=(( struct Cyc_Absyn_TupleType_struct*) _temp1451)->f1; goto
_LL1212;} else{ goto _LL1213;} _LL1213: _LL1464: _temp1463= _temp1179.f1; if((
unsigned int) _temp1463 >  4u?*(( int*) _temp1463) ==  Cyc_Absyn_AnonStructType:
0){ _LL1466: _temp1465=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1463)->f1;
goto _LL1460;} else{ goto _LL1215;} _LL1460: _temp1459= _temp1179.f2; if((
unsigned int) _temp1459 >  4u?*(( int*) _temp1459) ==  Cyc_Absyn_AnonStructType:
0){ _LL1462: _temp1461=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1459)->f1;
goto _LL1214;} else{ goto _LL1215;} _LL1215: _LL1472: _temp1471= _temp1179.f1;
if(( unsigned int) _temp1471 >  4u?*(( int*) _temp1471) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1474: _temp1473=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1471)->f1;
goto _LL1468;} else{ goto _LL1217;} _LL1468: _temp1467= _temp1179.f2; if((
unsigned int) _temp1467 >  4u?*(( int*) _temp1467) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1470: _temp1469=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1467)->f1;
goto _LL1216;} else{ goto _LL1217;} _LL1217: _LL1478: _temp1477= _temp1179.f1;
if( _temp1477 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1476;} else{ goto _LL1219;}
_LL1476: _temp1475= _temp1179.f2; if( _temp1475 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1218;} else{ goto _LL1219;} _LL1219: _LL1484: _temp1483= _temp1179.f1;
if(( unsigned int) _temp1483 >  4u?*(( int*) _temp1483) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1486: _temp1485=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1483)->f1; goto _LL1480;} else{ goto _LL1221;} _LL1480: _temp1479=
_temp1179.f2; if(( unsigned int) _temp1479 >  4u?*(( int*) _temp1479) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1482: _temp1481=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1479)->f1; goto _LL1220;} else{ goto _LL1221;} _LL1221: _LL1488: _temp1487=
_temp1179.f1; if(( unsigned int) _temp1487 >  4u?*(( int*) _temp1487) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1222;} else{ goto _LL1223;} _LL1223: _LL1490: _temp1489= _temp1179.f2;
if(( unsigned int) _temp1489 >  4u?*(( int*) _temp1489) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1224;} else{ goto _LL1225;} _LL1225: _LL1492: _temp1491= _temp1179.f1;
if(( unsigned int) _temp1491 >  4u?*(( int*) _temp1491) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1226;} else{ goto _LL1227;} _LL1227: _LL1494: _temp1493= _temp1179.f1;
if(( unsigned int) _temp1493 >  4u?*(( int*) _temp1493) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1228;} else{ goto _LL1229;} _LL1229: _LL1496: _temp1495= _temp1179.f2;
if(( unsigned int) _temp1495 >  4u?*(( int*) _temp1495) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1230;} else{ goto _LL1231;} _LL1231: _LL1498: _temp1497= _temp1179.f2;
if(( unsigned int) _temp1497 >  4u?*(( int*) _temp1497) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1232;} else{ goto _LL1233;} _LL1233: goto _LL1234; _LL1182: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1184: return; _LL1186: { struct _tagged_arr* _temp1499=
_temp1247->name; struct _tagged_arr* _temp1500= _temp1243->name; int _temp1501=*((
int*) _check_null( _temp1247->identity)); int _temp1502=*(( int*) _check_null(
_temp1243->identity)); void* _temp1503= Cyc_Absyn_conref_val( _temp1247->kind);
void* _temp1504= Cyc_Absyn_conref_val( _temp1243->kind); if( _temp1502 == 
_temp1501? Cyc_Std_zstrptrcmp( _temp1499, _temp1500) ==  0: 0){ if( _temp1503 != 
_temp1504){({ struct Cyc_Std_String_pa_struct _temp1508; _temp1508.tag= Cyc_Std_String_pa;
_temp1508.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1504);{ struct
Cyc_Std_String_pa_struct _temp1507; _temp1507.tag= Cyc_Std_String_pa; _temp1507.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1503);{ struct Cyc_Std_String_pa_struct
_temp1506; _temp1506.tag= Cyc_Std_String_pa; _temp1506.f1=( struct _tagged_arr)*
_temp1499;{ void* _temp1505[ 3u]={& _temp1506,& _temp1507,& _temp1508};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1505, sizeof( void*), 3u));}}}});}
return;} Cyc_Tcutil_failure_reason= _tag_arr("(variable types are not the same)",
sizeof( unsigned char), 34u); goto _LL1180;} _LL1188: if((( _temp1255 !=  0?
_temp1263 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1255),( struct _tuple1*) _check_null( _temp1263)) ==  0: 0)? 1:( _temp1255
==  0? _temp1263 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1253, _temp1261);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different struct types)", sizeof(
unsigned char), 25u); goto _LL1180; _LL1190: if( Cyc_Absyn_qvar_cmp( _temp1271,
_temp1267) ==  0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different enum types)",
sizeof( unsigned char), 23u); goto _LL1180; _LL1192: { int bad= 0; for( 0;
_temp1279 !=  0? _temp1275 !=  0: 0;( _temp1279= _temp1279->tl, _temp1275=
_temp1275->tl)){ struct Cyc_Absyn_Enumfield* _temp1509=( struct Cyc_Absyn_Enumfield*)
_temp1279->hd; struct Cyc_Absyn_Enumfield* _temp1510=( struct Cyc_Absyn_Enumfield*)
_temp1275->hd; if( Cyc_Absyn_qvar_cmp( _temp1509->name, _temp1510->name) !=  0){
Cyc_Tcutil_failure_reason= _tag_arr("(different names for enum fields)", sizeof(
unsigned char), 34u); bad= 1; break;} if( _temp1509->tag ==  _temp1510->tag){
continue;} if( _temp1509->tag ==  0? 1: _temp1510->tag ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)", sizeof( unsigned char), 39u);
bad= 1; break;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1509->tag)) !=  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1510->tag))){ Cyc_Tcutil_failure_reason= _tag_arr("(different tag values for enum fields)",
sizeof( unsigned char), 39u); bad= 1; break;}} if( bad){ goto _LL1180;} if(
_temp1279 ==  0? _temp1275 ==  0: 0){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)", sizeof( unsigned char), 39u);
goto _LL1180;} _LL1194: if((( _temp1287 !=  0? _temp1295 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1287),( struct _tuple1*) _check_null(
_temp1295)) ==  0: 0)? 1:( _temp1287 ==  0? _temp1295 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1285, _temp1293); return;} Cyc_Tcutil_failure_reason= _tag_arr("(different union types)",
sizeof( unsigned char), 24u); goto _LL1180; _LL1196: if( _temp1322 ==  _temp1309?
1: Cyc_Absyn_qvar_cmp( _temp1322->name, _temp1309->name) ==  0){ Cyc_Tcutil_unify_it(
_temp1301, _temp1314); Cyc_Tcutil_unify_list( _temp1303, _temp1316); return;}
Cyc_Tcutil_failure_reason= _tag_arr("(different tunion types)", sizeof(
unsigned char), 25u); goto _LL1180; _LL1198: if(( _temp1345 ==  _temp1333? 1:
Cyc_Absyn_qvar_cmp( _temp1345->name, _temp1333->name) ==  0)? _temp1343 == 
_temp1331? 1: Cyc_Absyn_qvar_cmp( _temp1343->name, _temp1331->name) ==  0: 0){
Cyc_Tcutil_unify_list( _temp1327, _temp1339); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)", sizeof( unsigned char), 31u); goto
_LL1180; _LL1200: Cyc_Tcutil_unify_it( _temp1359, _temp1373); Cyc_Tcutil_unify_it(
_temp1371, _temp1357); Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_unify_tqual(
_temp1353, _temp1367); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1351, _temp1365, _tag_arr("(different pointer bounds)", sizeof(
unsigned char), 27u));{ void* _temp1511=( void*)( Cyc_Absyn_compress_conref(
_temp1351))->v; void* _temp1517; _LL1513: if(( unsigned int) _temp1511 >  1u?*((
int*) _temp1511) ==  Cyc_Absyn_Eq_constr: 0){ _LL1518: _temp1517=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1511)->f1; if( _temp1517 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1514;} else{ goto _LL1515;}} else{ goto _LL1515;}
_LL1515: goto _LL1516; _LL1514: return; _LL1516: goto _LL1512; _LL1512:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y,
struct _tagged_arr reason)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp1355, _temp1369, _tag_arr("(different pointer types)", sizeof(
unsigned char), 26u)); return; _LL1202: if( _temp1379 ==  _temp1385? _temp1377
==  _temp1383: 0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different integral types)",
sizeof( unsigned char), 27u); goto _LL1180; _LL1204: return; _LL1206: return;
_LL1208: Cyc_Tcutil_unify_tqual( _temp1399, _temp1407); Cyc_Tcutil_unify_it(
_temp1401, _temp1409); if( _temp1405 ==  _temp1397){ return;} if( _temp1405 == 
0? 1: _temp1397 ==  0){ goto _LL1180;} if( Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1405)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1397))){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)", sizeof( unsigned char), 24u); goto _LL1180;
_LL1210: { int done= 0;{ struct _RegionHandle _temp1519= _new_region(); struct
_RegionHandle* rgn=& _temp1519; _push_region( rgn);{ struct Cyc_List_List* inst=
0; while( _temp1429 !=  0) { if( _temp1449 ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too few type variables)", sizeof(
unsigned char), 50u); break;} inst=({ struct Cyc_List_List* _temp1520=( struct
Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1520->hd=(
void*)({ struct _tuple5* _temp1521=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp1521->f1=( struct Cyc_Absyn_Tvar*) _temp1449->hd;
_temp1521->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp1522=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1522[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1523; _temp1523.tag= Cyc_Absyn_VarType; _temp1523.f1=(
struct Cyc_Absyn_Tvar*) _temp1429->hd; _temp1523;}); _temp1522;}); _temp1521;});
_temp1520->tl= inst; _temp1520;}); _temp1429= _temp1429->tl; _temp1449=
_temp1449->tl;} if( _temp1449 !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(second function type has too many type variables)",
sizeof( unsigned char), 51u); _npop_handler( 0u); goto _LL1180;} if( inst !=  0){
Cyc_Tcutil_unify_it(( void*)({ struct Cyc_Absyn_FnType_struct* _temp1524=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1524[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1525; _temp1525.tag= Cyc_Absyn_FnType;
_temp1525.f1=({ struct Cyc_Absyn_FnInfo _temp1526; _temp1526.tvars= 0; _temp1526.effect=
_temp1427; _temp1526.ret_typ=( void*) _temp1425; _temp1526.args= _temp1423;
_temp1526.c_varargs= _temp1421; _temp1526.cyc_varargs= _temp1419; _temp1526.rgn_po=
_temp1417; _temp1526.attributes= _temp1415; _temp1526;}); _temp1525;});
_temp1524;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1527=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1527[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1528; _temp1528.tag= Cyc_Absyn_FnType;
_temp1528.f1=({ struct Cyc_Absyn_FnInfo _temp1529; _temp1529.tvars= 0; _temp1529.effect=
_temp1447; _temp1529.ret_typ=( void*) _temp1445; _temp1529.args= _temp1443;
_temp1529.c_varargs= _temp1441; _temp1529.cyc_varargs= _temp1439; _temp1529.rgn_po=
_temp1437; _temp1529.attributes= _temp1435; _temp1529;}); _temp1528;});
_temp1527;}))); done= 1;}}; _pop_region( rgn);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp1425, _temp1445); for( 0; _temp1423 !=  0? _temp1443 !=  0: 0;( _temp1423=
_temp1423->tl, _temp1443= _temp1443->tl)){ Cyc_Tcutil_unify_tqual((*(( struct
_tuple2*) _temp1423->hd)).f2,(*(( struct _tuple2*) _temp1443->hd)).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*) _temp1423->hd)).f3,(*(( struct _tuple2*) _temp1443->hd)).f3);}
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; if( _temp1423 !=  0? 1:
_temp1443 !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different number of arguments)",
sizeof( unsigned char), 52u); goto _LL1180;} if( _temp1421 !=  _temp1441){ Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)", sizeof( unsigned char), 41u);
goto _LL1180;}{ int bad_cyc_vararg= 0;{ struct _tuple11 _temp1531=({ struct
_tuple11 _temp1530; _temp1530.f1= _temp1419; _temp1530.f2= _temp1439; _temp1530;});
struct Cyc_Absyn_VarargInfo* _temp1541; struct Cyc_Absyn_VarargInfo* _temp1543;
struct Cyc_Absyn_VarargInfo* _temp1545; struct Cyc_Absyn_VarargInfo* _temp1547;
struct Cyc_Absyn_VarargInfo* _temp1549; struct Cyc_Absyn_VarargInfo _temp1551;
int _temp1552; void* _temp1554; struct Cyc_Absyn_Tqual _temp1556; struct Cyc_Core_Opt*
_temp1558; struct Cyc_Absyn_VarargInfo* _temp1560; struct Cyc_Absyn_VarargInfo
_temp1562; int _temp1563; void* _temp1565; struct Cyc_Absyn_Tqual _temp1567;
struct Cyc_Core_Opt* _temp1569; _LL1533: _LL1544: _temp1543= _temp1531.f1; if(
_temp1543 ==  0){ goto _LL1542;} else{ goto _LL1535;} _LL1542: _temp1541=
_temp1531.f2; if( _temp1541 ==  0){ goto _LL1534;} else{ goto _LL1535;} _LL1535:
_LL1546: _temp1545= _temp1531.f1; if( _temp1545 ==  0){ goto _LL1536;} else{
goto _LL1537;} _LL1537: _LL1548: _temp1547= _temp1531.f2; if( _temp1547 ==  0){
goto _LL1538;} else{ goto _LL1539;} _LL1539: _LL1561: _temp1560= _temp1531.f1;
if( _temp1560 ==  0){ goto _LL1532;} else{ _temp1562=* _temp1560; _LL1570:
_temp1569= _temp1562.name; goto _LL1568; _LL1568: _temp1567= _temp1562.tq; goto
_LL1566; _LL1566: _temp1565=( void*) _temp1562.type; goto _LL1564; _LL1564:
_temp1563= _temp1562.inject; goto _LL1550;} _LL1550: _temp1549= _temp1531.f2;
if( _temp1549 ==  0){ goto _LL1532;} else{ _temp1551=* _temp1549; _LL1559:
_temp1558= _temp1551.name; goto _LL1557; _LL1557: _temp1556= _temp1551.tq; goto
_LL1555; _LL1555: _temp1554=( void*) _temp1551.type; goto _LL1553; _LL1553:
_temp1552= _temp1551.inject; goto _LL1540;} _LL1534: goto _LL1532; _LL1536: goto
_LL1538; _LL1538: bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type takes varargs)",
sizeof( unsigned char), 39u); goto _LL1532; _LL1540: Cyc_Tcutil_unify_tqual(
_temp1567, _temp1556); Cyc_Tcutil_unify_it( _temp1565, _temp1554); if( _temp1563
!=  _temp1552){ bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type injects varargs)",
sizeof( unsigned char), 41u);} goto _LL1532; _LL1532:;} if( bad_cyc_vararg){
goto _LL1180;}{ int bad_effect= 0;{ struct _tuple12 _temp1572=({ struct _tuple12
_temp1571; _temp1571.f1= _temp1427; _temp1571.f2= _temp1447; _temp1571;});
struct Cyc_Core_Opt* _temp1582; struct Cyc_Core_Opt* _temp1584; struct Cyc_Core_Opt*
_temp1586; struct Cyc_Core_Opt* _temp1588; _LL1574: _LL1585: _temp1584=
_temp1572.f1; if( _temp1584 ==  0){ goto _LL1583;} else{ goto _LL1576;} _LL1583:
_temp1582= _temp1572.f2; if( _temp1582 ==  0){ goto _LL1575;} else{ goto _LL1576;}
_LL1576: _LL1587: _temp1586= _temp1572.f1; if( _temp1586 ==  0){ goto _LL1577;}
else{ goto _LL1578;} _LL1578: _LL1589: _temp1588= _temp1572.f2; if( _temp1588 == 
0){ goto _LL1579;} else{ goto _LL1580;} _LL1580: goto _LL1581; _LL1575: goto
_LL1573; _LL1577: goto _LL1579; _LL1579: bad_effect= 1; goto _LL1573; _LL1581:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1427))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1447))->v); goto _LL1573; _LL1573:;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; if( bad_effect){ Cyc_Tcutil_failure_reason= _tag_arr("(function type effects do not unify)",
sizeof( unsigned char), 37u); goto _LL1180;} if( ! Cyc_Tcutil_same_atts(
_temp1435, _temp1415)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different attributes)",
sizeof( unsigned char), 43u); goto _LL1180;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1437, _temp1417)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different region lifetime orderings)",
sizeof( unsigned char), 58u); goto _LL1180;} return;}}} _LL1212: for( 0;
_temp1453 !=  0? _temp1457 !=  0: 0;( _temp1453= _temp1453->tl, _temp1457=
_temp1457->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*) _temp1453->hd)).f1,(*((
struct _tuple4*) _temp1457->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)
_temp1453->hd)).f2,(*(( struct _tuple4*) _temp1457->hd)).f2);} if( _temp1453 == 
0? _temp1457 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(tuple types have different numbers of components)",
sizeof( unsigned char), 51u); goto _LL1180; _LL1214: _temp1473= _temp1465;
_temp1469= _temp1461; goto _LL1216; _LL1216: for( 0; _temp1469 !=  0? _temp1473
!=  0: 0;( _temp1469= _temp1469->tl, _temp1473= _temp1473->tl)){ struct Cyc_Absyn_Structfield*
_temp1590=( struct Cyc_Absyn_Structfield*) _temp1469->hd; struct Cyc_Absyn_Structfield*
_temp1591=( struct Cyc_Absyn_Structfield*) _temp1473->hd; if( Cyc_Std_zstrptrcmp(
_temp1590->name, _temp1591->name) !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(different member names in anonymous structs/unions)",
sizeof( unsigned char), 53u);( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1590->tq, _temp1591->tq); Cyc_Tcutil_unify_it(( void*) _temp1590->type,(
void*) _temp1591->type); if( ! Cyc_Tcutil_same_atts( _temp1590->attributes,
_temp1591->attributes)){ Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2;
Cyc_Tcutil_failure_reason= _tag_arr("(different attributes on member)", sizeof(
unsigned char), 33u);( int) _throw(( void*) Cyc_Tcutil_Unify);} if((( _temp1590->width
!=  0? _temp1591->width ==  0: 0)? 1:( _temp1591->width !=  0? _temp1590->width
==  0: 0))? 1:(( _temp1590->width !=  0? _temp1591->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1590->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1591->width)): 0)){ Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason= _tag_arr("(different bitfield widths on member)",
sizeof( unsigned char), 38u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} if(
_temp1469 ==  0? _temp1473 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(different number of members)", sizeof(
unsigned char), 30u); goto _LL1180; _LL1218: return; _LL1220: Cyc_Tcutil_unify_it(
_temp1485, _temp1481); return; _LL1222: goto _LL1224; _LL1224: goto _LL1226;
_LL1226: goto _LL1228; _LL1228: goto _LL1230; _LL1230: goto _LL1232; _LL1232:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)", sizeof( unsigned char), 22u); goto _LL1180;
_LL1234: goto _LL1180; _LL1180:;}( int) _throw(( void*) Cyc_Tcutil_Unify);} int
Cyc_Tcutil_opt_cmp( int(* cmp)( void*, void*), struct Cyc_Core_Opt* a1, struct
Cyc_Core_Opt* a2){ if( a1 ==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return
- 1;} if( a1 !=  0? a2 ==  0: 0){ return 1;} return cmp(( void*)(( struct Cyc_Core_Opt*)
_check_null( a1))->v,( void*)(( struct Cyc_Core_Opt*) _check_null( a2))->v);}
int Cyc_Tcutil_star_cmp( int(* cmp)( void**, void**), void** a1, void** a2){ if(
a1 ==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return - 1;} if( a1 !=  0? a2
==  0: 0){ return 1;} return cmp(( void**) _check_null( a1),( void**)
_check_null( a2));} int Cyc_Tcutil_list_cmp( int(* cmp)( void*, void*), struct
Cyc_List_List* l1, struct Cyc_List_List* l2){ if( l1 ==  l2){ return 0;} for( 0;
l1 !=  0? l2 !=  0: 0;( l1= l1->tl, l2= l2->tl)){ int _temp1592= cmp(( void*) l1->hd,(
void*) l2->hd); if( _temp1592 !=  0){ return _temp1592;}} if( l2 !=  0){ return
- 1;} if( l1 !=  0){ return 1;} return 0;} static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ int _temp1593=( tq1.q_const
+ ( tq1.q_volatile <<  1)) + ( tq1.q_restrict <<  2); int _temp1594=( tq2.q_const
+ ( tq2.q_volatile <<  1)) + ( tq2.q_restrict <<  2); return Cyc_Core_intcmp(
_temp1593, _temp1594);} static int Cyc_Tcutil_conrefs_cmp( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ x= Cyc_Absyn_compress_conref(
x); y= Cyc_Absyn_compress_conref( y); if( x ==  y){ return 0;}{ void* _temp1595=(
void*) x->v; void* _temp1603; _LL1597: if( _temp1595 == ( void*) Cyc_Absyn_No_constr){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if(( unsigned int) _temp1595 >  1u?*((
int*) _temp1595) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL1600;} else{ goto
_LL1601;} _LL1601: if(( unsigned int) _temp1595 >  1u?*(( int*) _temp1595) == 
Cyc_Absyn_Eq_constr: 0){ _LL1604: _temp1603=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1595)->f1; goto _LL1602;} else{ goto _LL1596;} _LL1598: return - 1; _LL1600:(
int) _throw(({ void* _temp1605[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress",
sizeof( unsigned char), 37u), _tag_arr( _temp1605, sizeof( void*), 0u));}));
_LL1602: { void* _temp1606=( void*) y->v; void* _temp1614; _LL1608: if(
_temp1606 == ( void*) Cyc_Absyn_No_constr){ goto _LL1609;} else{ goto _LL1610;}
_LL1610: if(( unsigned int) _temp1606 >  1u?*(( int*) _temp1606) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL1611;} else{ goto _LL1612;} _LL1612: if(( unsigned int) _temp1606 > 
1u?*(( int*) _temp1606) ==  Cyc_Absyn_Eq_constr: 0){ _LL1615: _temp1614=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1606)->f1; goto _LL1613;} else{ goto
_LL1607;} _LL1609: return 1; _LL1611:( int) _throw(({ void* _temp1616[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress(2)", sizeof(
unsigned char), 40u), _tag_arr( _temp1616, sizeof( void*), 0u));})); _LL1613:
return cmp( _temp1603, _temp1614); _LL1607:;} _LL1596:;}} static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4* tqt1, struct _tuple4* tqt2){ struct _tuple4 _temp1619; void*
_temp1620; struct Cyc_Absyn_Tqual _temp1622; struct _tuple4* _temp1617= tqt1;
_temp1619=* _temp1617; _LL1623: _temp1622= _temp1619.f1; goto _LL1621; _LL1621:
_temp1620= _temp1619.f2; goto _LL1618; _LL1618: { struct _tuple4 _temp1626; void*
_temp1627; struct Cyc_Absyn_Tqual _temp1629; struct _tuple4* _temp1624= tqt2;
_temp1626=* _temp1624; _LL1630: _temp1629= _temp1626.f1; goto _LL1628; _LL1628:
_temp1627= _temp1626.f2; goto _LL1625; _LL1625: { int _temp1631= Cyc_Tcutil_tqual_cmp(
_temp1622, _temp1629); if( _temp1631 !=  0){ return _temp1631;} return Cyc_Tcutil_typecmp(
_temp1620, _temp1627);}}} static int Cyc_Tcutil_const_uint_exp_cmp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ return Cyc_Core_intcmp(( int) Cyc_Evexp_eval_const_uint_exp(
e1),( int) Cyc_Evexp_eval_const_uint_exp( e2));} static int Cyc_Tcutil_attribute_case_number(
void* att){ void* _temp1632= att; _LL1634: if(( unsigned int) _temp1632 >  16u?*((
int*) _temp1632) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL1635;} else{ goto
_LL1636;} _LL1636: if( _temp1632 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1637;} else{ goto _LL1638;} _LL1638: if( _temp1632 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1639;} else{ goto _LL1640;} _LL1640: if( _temp1632 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1641;} else{ goto _LL1642;} _LL1642: if( _temp1632 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL1643;} else{ goto _LL1644;} _LL1644: if( _temp1632 == ( void*) Cyc_Absyn_Const_att){
goto _LL1645;} else{ goto _LL1646;} _LL1646: if(( unsigned int) _temp1632 >  16u?*((
int*) _temp1632) ==  Cyc_Absyn_Aligned_att: 0){ goto _LL1647;} else{ goto
_LL1648;} _LL1648: if( _temp1632 == ( void*) Cyc_Absyn_Packed_att){ goto _LL1649;}
else{ goto _LL1650;} _LL1650: if(( unsigned int) _temp1632 >  16u?*(( int*)
_temp1632) ==  Cyc_Absyn_Section_att: 0){ goto _LL1651;} else{ goto _LL1652;}
_LL1652: if( _temp1632 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL1653;} else{
goto _LL1654;} _LL1654: if( _temp1632 == ( void*) Cyc_Absyn_Shared_att){ goto
_LL1655;} else{ goto _LL1656;} _LL1656: if( _temp1632 == ( void*) Cyc_Absyn_Unused_att){
goto _LL1657;} else{ goto _LL1658;} _LL1658: if( _temp1632 == ( void*) Cyc_Absyn_Weak_att){
goto _LL1659;} else{ goto _LL1660;} _LL1660: if( _temp1632 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL1661;} else{ goto _LL1662;} _LL1662: if( _temp1632 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL1663;} else{ goto _LL1664;} _LL1664: if( _temp1632 == ( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL1665;} else{ goto _LL1666;} _LL1666: if( _temp1632 == ( void*) Cyc_Absyn_Constructor_att){
goto _LL1667;} else{ goto _LL1668;} _LL1668: if( _temp1632 == ( void*) Cyc_Absyn_Destructor_att){
goto _LL1669;} else{ goto _LL1670;} _LL1670: if( _temp1632 == ( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL1671;} else{ goto _LL1672;} _LL1672: if(( unsigned int) _temp1632 >  16u?*((
int*) _temp1632) ==  Cyc_Absyn_Format_att: 0){ goto _LL1673;} else{ goto _LL1633;}
_LL1635: return 0; _LL1637: return 1; _LL1639: return 2; _LL1641: return 3;
_LL1643: return 4; _LL1645: return 5; _LL1647: return 6; _LL1649: return 7;
_LL1651: return 8; _LL1653: return 9; _LL1655: return 10; _LL1657: return 11;
_LL1659: return 12; _LL1661: return 13; _LL1663: return 14; _LL1665: return 15;
_LL1667: return 16; _LL1669: return 17; _LL1671: return 18; _LL1673: return 19;
_LL1633:;} static int Cyc_Tcutil_attribute_cmp( void* att1, void* att2){ struct
_tuple8 _temp1675=({ struct _tuple8 _temp1674; _temp1674.f1= att1; _temp1674.f2=
att2; _temp1674;}); void* _temp1687; int _temp1689; void* _temp1691; int
_temp1693; void* _temp1695; int _temp1697; void* _temp1699; int _temp1701; void*
_temp1703; struct _tagged_arr _temp1705; void* _temp1707; struct _tagged_arr
_temp1709; void* _temp1711; int _temp1713; int _temp1715; void* _temp1717; void*
_temp1719; int _temp1721; int _temp1723; void* _temp1725; _LL1677: _LL1692:
_temp1691= _temp1675.f1; if(( unsigned int) _temp1691 >  16u?*(( int*) _temp1691)
==  Cyc_Absyn_Regparm_att: 0){ _LL1694: _temp1693=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp1691)->f1; goto _LL1688;} else{ goto _LL1679;} _LL1688: _temp1687=
_temp1675.f2; if(( unsigned int) _temp1687 >  16u?*(( int*) _temp1687) ==  Cyc_Absyn_Regparm_att:
0){ _LL1690: _temp1689=(( struct Cyc_Absyn_Regparm_att_struct*) _temp1687)->f1;
goto _LL1678;} else{ goto _LL1679;} _LL1679: _LL1700: _temp1699= _temp1675.f1;
if(( unsigned int) _temp1699 >  16u?*(( int*) _temp1699) ==  Cyc_Absyn_Aligned_att:
0){ _LL1702: _temp1701=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1699)->f1;
goto _LL1696;} else{ goto _LL1681;} _LL1696: _temp1695= _temp1675.f2; if((
unsigned int) _temp1695 >  16u?*(( int*) _temp1695) ==  Cyc_Absyn_Aligned_att: 0){
_LL1698: _temp1697=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1695)->f1; goto
_LL1680;} else{ goto _LL1681;} _LL1681: _LL1708: _temp1707= _temp1675.f1; if((
unsigned int) _temp1707 >  16u?*(( int*) _temp1707) ==  Cyc_Absyn_Section_att: 0){
_LL1710: _temp1709=(( struct Cyc_Absyn_Section_att_struct*) _temp1707)->f1; goto
_LL1704;} else{ goto _LL1683;} _LL1704: _temp1703= _temp1675.f2; if((
unsigned int) _temp1703 >  16u?*(( int*) _temp1703) ==  Cyc_Absyn_Section_att: 0){
_LL1706: _temp1705=(( struct Cyc_Absyn_Section_att_struct*) _temp1703)->f1; goto
_LL1682;} else{ goto _LL1683;} _LL1683: _LL1720: _temp1719= _temp1675.f1; if((
unsigned int) _temp1719 >  16u?*(( int*) _temp1719) ==  Cyc_Absyn_Format_att: 0){
_LL1726: _temp1725=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1719)->f1;
goto _LL1724; _LL1724: _temp1723=(( struct Cyc_Absyn_Format_att_struct*)
_temp1719)->f2; goto _LL1722; _LL1722: _temp1721=(( struct Cyc_Absyn_Format_att_struct*)
_temp1719)->f3; goto _LL1712;} else{ goto _LL1685;} _LL1712: _temp1711=
_temp1675.f2; if(( unsigned int) _temp1711 >  16u?*(( int*) _temp1711) ==  Cyc_Absyn_Format_att:
0){ _LL1718: _temp1717=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1711)->f1;
goto _LL1716; _LL1716: _temp1715=(( struct Cyc_Absyn_Format_att_struct*)
_temp1711)->f2; goto _LL1714; _LL1714: _temp1713=(( struct Cyc_Absyn_Format_att_struct*)
_temp1711)->f3; goto _LL1684;} else{ goto _LL1685;} _LL1685: goto _LL1686;
_LL1678: _temp1701= _temp1693; _temp1697= _temp1689; goto _LL1680; _LL1680:
return Cyc_Core_intcmp( _temp1701, _temp1697); _LL1682: return Cyc_Std_strcmp(
_temp1709, _temp1705); _LL1684: { int _temp1727= Cyc_Core_intcmp(( int)((
unsigned int) _temp1725),( int)(( unsigned int) _temp1717)); if( _temp1727 !=  0){
return _temp1727;}{ int _temp1728= Cyc_Core_intcmp( _temp1723, _temp1715); if(
_temp1728 !=  0){ return _temp1728;} return Cyc_Core_intcmp( _temp1721,
_temp1713);}} _LL1686: { int _temp1729= Cyc_Tcutil_attribute_case_number( att1);
int _temp1730= Cyc_Tcutil_attribute_case_number( att2); return Cyc_Core_intcmp(
_temp1729, _temp1730);} _LL1676:;} static int Cyc_Tcutil_structfield_cmp( struct
Cyc_Absyn_Structfield* f1, struct Cyc_Absyn_Structfield* f2){ int _temp1731= Cyc_Std_zstrptrcmp(
f1->name, f2->name); if( _temp1731 !=  0){ return _temp1731;}{ int _temp1732=
Cyc_Tcutil_tqual_cmp( f1->tq, f2->tq); if( _temp1732 !=  0){ return _temp1732;}{
int _temp1733= Cyc_Tcutil_typecmp(( void*) f1->type,( void*) f2->type); if(
_temp1733 !=  0){ return _temp1733;}{ int _temp1734= Cyc_Tcutil_list_cmp( Cyc_Tcutil_attribute_cmp,
f1->attributes, f2->attributes); if( _temp1734 !=  0){ return _temp1734;} return((
int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp*
a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
f1->width, f2->width);}}}} static int Cyc_Tcutil_enumfield_cmp( struct Cyc_Absyn_Enumfield*
e1, struct Cyc_Absyn_Enumfield* e2){ int _temp1735= Cyc_Absyn_qvar_cmp( e1->name,
e2->name); if( _temp1735 !=  0){ return _temp1735;} return(( int(*)( int(* cmp)(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp* a1, struct
Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp, e1->tag,
e2->tag);} extern int Cyc_Tcutil_typecmp( void* t1, void* t2); int Cyc_Tcutil_type_case_number(
void* t){ void* _temp1736= t; _LL1738: if( _temp1736 == ( void*) Cyc_Absyn_VoidType){
goto _LL1739;} else{ goto _LL1740;} _LL1740: if(( unsigned int) _temp1736 >  4u?*((
int*) _temp1736) ==  Cyc_Absyn_Evar: 0){ goto _LL1741;} else{ goto _LL1742;}
_LL1742: if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_VarType:
0){ goto _LL1743;} else{ goto _LL1744;} _LL1744: if(( unsigned int) _temp1736 > 
4u?*(( int*) _temp1736) ==  Cyc_Absyn_TunionType: 0){ goto _LL1745;} else{ goto
_LL1746;} _LL1746: if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1747;} else{ goto _LL1748;} _LL1748: if((
unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_PointerType: 0){
goto _LL1749;} else{ goto _LL1750;} _LL1750: if(( unsigned int) _temp1736 >  4u?*((
int*) _temp1736) ==  Cyc_Absyn_IntType: 0){ goto _LL1751;} else{ goto _LL1752;}
_LL1752: if( _temp1736 == ( void*) Cyc_Absyn_FloatType){ goto _LL1753;} else{
goto _LL1754;} _LL1754: if( _temp1736 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1755;} else{ goto _LL1756;} _LL1756: if(( unsigned int) _temp1736 >  4u?*((
int*) _temp1736) ==  Cyc_Absyn_ArrayType: 0){ goto _LL1757;} else{ goto _LL1758;}
_LL1758: if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_FnType:
0){ goto _LL1759;} else{ goto _LL1760;} _LL1760: if(( unsigned int) _temp1736 > 
4u?*(( int*) _temp1736) ==  Cyc_Absyn_TupleType: 0){ goto _LL1761;} else{ goto
_LL1762;} _LL1762: if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) == 
Cyc_Absyn_StructType: 0){ goto _LL1763;} else{ goto _LL1764;} _LL1764: if((
unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_UnionType: 0){
goto _LL1765;} else{ goto _LL1766;} _LL1766: if(( unsigned int) _temp1736 >  4u?*((
int*) _temp1736) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL1767;} else{ goto
_LL1768;} _LL1768: if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) == 
Cyc_Absyn_AnonUnionType: 0){ goto _LL1769;} else{ goto _LL1770;} _LL1770: if((
unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_EnumType: 0){
goto _LL1771;} else{ goto _LL1772;} _LL1772: if(( unsigned int) _temp1736 >  4u?*((
int*) _temp1736) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL1773;} else{ goto
_LL1774;} _LL1774: if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) == 
Cyc_Absyn_RgnHandleType: 0){ goto _LL1775;} else{ goto _LL1776;} _LL1776: if((
unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_TypedefType: 0){
goto _LL1777;} else{ goto _LL1778;} _LL1778: if( _temp1736 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1779;} else{ goto _LL1780;} _LL1780: if(( unsigned int) _temp1736 >  4u?*((
int*) _temp1736) ==  Cyc_Absyn_AccessEff: 0){ goto _LL1781;} else{ goto _LL1782;}
_LL1782: if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int) _temp1736 > 
4u?*(( int*) _temp1736) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL1785;} else{ goto
_LL1737;} _LL1739: return 0; _LL1741: return 1; _LL1743: return 2; _LL1745:
return 3; _LL1747: return 4; _LL1749: return 5; _LL1751: return 6; _LL1753:
return 7; _LL1755: return 8; _LL1757: return 9; _LL1759: return 10; _LL1761:
return 11; _LL1763: return 12; _LL1765: return 13; _LL1767: return 14; _LL1769:
return 15; _LL1771: return 16; _LL1773: return 17; _LL1775: return 18; _LL1777:
return 19; _LL1779: return 20; _LL1781: return 21; _LL1783: return 22; _LL1785:
return 23; _LL1737:;} int Cyc_Tcutil_shallow_typecmp_it( void* t1, void* t2){
return Cyc_Core_intcmp( Cyc_Tcutil_type_case_number( t1), Cyc_Tcutil_type_case_number(
t2));} int Cyc_Tcutil_typecmp( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1);
t2= Cyc_Tcutil_compress( t2); if( t1 ==  t2){ return 0;}{ int _temp1786= Cyc_Tcutil_shallow_typecmp_it(
t1, t2); if( _temp1786 !=  0){ return _temp1786;}{ struct _tuple8 _temp1788=({
struct _tuple8 _temp1787; _temp1787.f1= t2; _temp1787.f2= t1; _temp1787;}); void*
_temp1844; void* _temp1846; void* _temp1848; void* _temp1850; void* _temp1852;
struct Cyc_Absyn_Tvar* _temp1854; void* _temp1856; struct Cyc_Absyn_Tvar*
_temp1858; void* _temp1860; struct Cyc_List_List* _temp1862; struct _tuple1*
_temp1864; void* _temp1866; struct Cyc_List_List* _temp1868; struct _tuple1*
_temp1870; void* _temp1872; struct _tuple1* _temp1874; void* _temp1876; struct
_tuple1* _temp1878; void* _temp1880; struct Cyc_List_List* _temp1882; void*
_temp1884; struct Cyc_List_List* _temp1886; void* _temp1888; struct Cyc_Absyn_Uniondecl**
_temp1890; struct Cyc_List_List* _temp1892; struct _tuple1* _temp1894; void*
_temp1896; struct Cyc_Absyn_Uniondecl** _temp1898; struct Cyc_List_List*
_temp1900; struct _tuple1* _temp1902; void* _temp1904; struct Cyc_Absyn_TunionInfo
_temp1906; void* _temp1908; struct Cyc_List_List* _temp1910; void* _temp1912;
struct Cyc_Absyn_Tuniondecl** _temp1914; struct Cyc_Absyn_Tuniondecl* _temp1916;
void* _temp1917; struct Cyc_Absyn_TunionInfo _temp1919; void* _temp1921; struct
Cyc_List_List* _temp1923; void* _temp1925; struct Cyc_Absyn_Tuniondecl**
_temp1927; struct Cyc_Absyn_Tuniondecl* _temp1929; void* _temp1930; struct Cyc_Absyn_TunionFieldInfo
_temp1932; struct Cyc_List_List* _temp1934; void* _temp1936; struct Cyc_Absyn_Tunionfield*
_temp1938; struct Cyc_Absyn_Tuniondecl* _temp1940; void* _temp1942; struct Cyc_Absyn_TunionFieldInfo
_temp1944; struct Cyc_List_List* _temp1946; void* _temp1948; struct Cyc_Absyn_Tunionfield*
_temp1950; struct Cyc_Absyn_Tuniondecl* _temp1952; void* _temp1954; struct Cyc_Absyn_PtrInfo
_temp1956; struct Cyc_Absyn_Conref* _temp1958; struct Cyc_Absyn_Tqual _temp1960;
struct Cyc_Absyn_Conref* _temp1962; void* _temp1964; void* _temp1966; void*
_temp1968; struct Cyc_Absyn_PtrInfo _temp1970; struct Cyc_Absyn_Conref*
_temp1972; struct Cyc_Absyn_Tqual _temp1974; struct Cyc_Absyn_Conref* _temp1976;
void* _temp1978; void* _temp1980; void* _temp1982; void* _temp1984; void*
_temp1986; void* _temp1988; void* _temp1990; void* _temp1992; void* _temp1994;
void* _temp1996; void* _temp1998; void* _temp2000; void* _temp2002; struct Cyc_Absyn_Exp*
_temp2004; struct Cyc_Absyn_Tqual _temp2006; void* _temp2008; void* _temp2010;
struct Cyc_Absyn_Exp* _temp2012; struct Cyc_Absyn_Tqual _temp2014; void*
_temp2016; void* _temp2018; struct Cyc_Absyn_FnInfo _temp2020; struct Cyc_List_List*
_temp2022; struct Cyc_List_List* _temp2024; struct Cyc_Absyn_VarargInfo*
_temp2026; int _temp2028; struct Cyc_List_List* _temp2030; void* _temp2032;
struct Cyc_Core_Opt* _temp2034; struct Cyc_List_List* _temp2036; void* _temp2038;
struct Cyc_Absyn_FnInfo _temp2040; struct Cyc_List_List* _temp2042; struct Cyc_List_List*
_temp2044; struct Cyc_Absyn_VarargInfo* _temp2046; int _temp2048; struct Cyc_List_List*
_temp2050; void* _temp2052; struct Cyc_Core_Opt* _temp2054; struct Cyc_List_List*
_temp2056; void* _temp2058; struct Cyc_List_List* _temp2060; void* _temp2062;
struct Cyc_List_List* _temp2064; void* _temp2066; struct Cyc_List_List*
_temp2068; void* _temp2070; struct Cyc_List_List* _temp2072; void* _temp2074;
struct Cyc_List_List* _temp2076; void* _temp2078; struct Cyc_List_List*
_temp2080; void* _temp2082; void* _temp2084; void* _temp2086; void* _temp2088;
void* _temp2090; void* _temp2092; void* _temp2094; void* _temp2096; void*
_temp2098; void* _temp2100; void* _temp2102; void* _temp2104; _LL1790: _LL1847:
_temp1846= _temp1788.f1; if(( unsigned int) _temp1846 >  4u?*(( int*) _temp1846)
==  Cyc_Absyn_Evar: 0){ goto _LL1845;} else{ goto _LL1792;} _LL1845: _temp1844=
_temp1788.f2; if(( unsigned int) _temp1844 >  4u?*(( int*) _temp1844) ==  Cyc_Absyn_Evar:
0){ goto _LL1791;} else{ goto _LL1792;} _LL1792: _LL1851: _temp1850= _temp1788.f1;
if( _temp1850 == ( void*) Cyc_Absyn_VoidType){ goto _LL1849;} else{ goto _LL1794;}
_LL1849: _temp1848= _temp1788.f2; if( _temp1848 == ( void*) Cyc_Absyn_VoidType){
goto _LL1793;} else{ goto _LL1794;} _LL1794: _LL1857: _temp1856= _temp1788.f1;
if(( unsigned int) _temp1856 >  4u?*(( int*) _temp1856) ==  Cyc_Absyn_VarType: 0){
_LL1859: _temp1858=(( struct Cyc_Absyn_VarType_struct*) _temp1856)->f1; goto
_LL1853;} else{ goto _LL1796;} _LL1853: _temp1852= _temp1788.f2; if((
unsigned int) _temp1852 >  4u?*(( int*) _temp1852) ==  Cyc_Absyn_VarType: 0){
_LL1855: _temp1854=(( struct Cyc_Absyn_VarType_struct*) _temp1852)->f1; goto
_LL1795;} else{ goto _LL1796;} _LL1796: _LL1867: _temp1866= _temp1788.f1; if((
unsigned int) _temp1866 >  4u?*(( int*) _temp1866) ==  Cyc_Absyn_StructType: 0){
_LL1871: _temp1870=(( struct Cyc_Absyn_StructType_struct*) _temp1866)->f1; goto
_LL1869; _LL1869: _temp1868=(( struct Cyc_Absyn_StructType_struct*) _temp1866)->f2;
goto _LL1861;} else{ goto _LL1798;} _LL1861: _temp1860= _temp1788.f2; if((
unsigned int) _temp1860 >  4u?*(( int*) _temp1860) ==  Cyc_Absyn_StructType: 0){
_LL1865: _temp1864=(( struct Cyc_Absyn_StructType_struct*) _temp1860)->f1; goto
_LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_StructType_struct*) _temp1860)->f2;
goto _LL1797;} else{ goto _LL1798;} _LL1798: _LL1877: _temp1876= _temp1788.f1;
if(( unsigned int) _temp1876 >  4u?*(( int*) _temp1876) ==  Cyc_Absyn_EnumType:
0){ _LL1879: _temp1878=(( struct Cyc_Absyn_EnumType_struct*) _temp1876)->f1;
goto _LL1873;} else{ goto _LL1800;} _LL1873: _temp1872= _temp1788.f2; if((
unsigned int) _temp1872 >  4u?*(( int*) _temp1872) ==  Cyc_Absyn_EnumType: 0){
_LL1875: _temp1874=(( struct Cyc_Absyn_EnumType_struct*) _temp1872)->f1; goto
_LL1799;} else{ goto _LL1800;} _LL1800: _LL1885: _temp1884= _temp1788.f1; if((
unsigned int) _temp1884 >  4u?*(( int*) _temp1884) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1887: _temp1886=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1884)->f1;
goto _LL1881;} else{ goto _LL1802;} _LL1881: _temp1880= _temp1788.f2; if((
unsigned int) _temp1880 >  4u?*(( int*) _temp1880) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1883: _temp1882=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1880)->f1;
goto _LL1801;} else{ goto _LL1802;} _LL1802: _LL1897: _temp1896= _temp1788.f1;
if(( unsigned int) _temp1896 >  4u?*(( int*) _temp1896) ==  Cyc_Absyn_UnionType:
0){ _LL1903: _temp1902=(( struct Cyc_Absyn_UnionType_struct*) _temp1896)->f1;
goto _LL1901; _LL1901: _temp1900=(( struct Cyc_Absyn_UnionType_struct*)
_temp1896)->f2; goto _LL1899; _LL1899: _temp1898=(( struct Cyc_Absyn_UnionType_struct*)
_temp1896)->f3; goto _LL1889;} else{ goto _LL1804;} _LL1889: _temp1888=
_temp1788.f2; if(( unsigned int) _temp1888 >  4u?*(( int*) _temp1888) ==  Cyc_Absyn_UnionType:
0){ _LL1895: _temp1894=(( struct Cyc_Absyn_UnionType_struct*) _temp1888)->f1;
goto _LL1893; _LL1893: _temp1892=(( struct Cyc_Absyn_UnionType_struct*)
_temp1888)->f2; goto _LL1891; _LL1891: _temp1890=(( struct Cyc_Absyn_UnionType_struct*)
_temp1888)->f3; goto _LL1803;} else{ goto _LL1804;} _LL1804: _LL1918: _temp1917=
_temp1788.f1; if(( unsigned int) _temp1917 >  4u?*(( int*) _temp1917) ==  Cyc_Absyn_TunionType:
0){ _LL1920: _temp1919=(( struct Cyc_Absyn_TunionType_struct*) _temp1917)->f1;
_LL1926: _temp1925=( void*) _temp1919.tunion_info; if(*(( int*) _temp1925) == 
Cyc_Absyn_KnownTunion){ _LL1928: _temp1927=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1925)->f1; _temp1929=* _temp1927; goto _LL1924;} else{ goto _LL1806;}
_LL1924: _temp1923= _temp1919.targs; goto _LL1922; _LL1922: _temp1921=( void*)
_temp1919.rgn; goto _LL1905;} else{ goto _LL1806;} _LL1905: _temp1904= _temp1788.f2;
if(( unsigned int) _temp1904 >  4u?*(( int*) _temp1904) ==  Cyc_Absyn_TunionType:
0){ _LL1907: _temp1906=(( struct Cyc_Absyn_TunionType_struct*) _temp1904)->f1;
_LL1913: _temp1912=( void*) _temp1906.tunion_info; if(*(( int*) _temp1912) == 
Cyc_Absyn_KnownTunion){ _LL1915: _temp1914=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1912)->f1; _temp1916=* _temp1914; goto _LL1911;} else{ goto _LL1806;}
_LL1911: _temp1910= _temp1906.targs; goto _LL1909; _LL1909: _temp1908=( void*)
_temp1906.rgn; goto _LL1805;} else{ goto _LL1806;} _LL1806: _LL1943: _temp1942=
_temp1788.f1; if(( unsigned int) _temp1942 >  4u?*(( int*) _temp1942) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1945: _temp1944=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1942)->f1;
_LL1949: _temp1948=( void*) _temp1944.field_info; if(*(( int*) _temp1948) == 
Cyc_Absyn_KnownTunionfield){ _LL1953: _temp1952=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1948)->f1; goto _LL1951; _LL1951: _temp1950=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1948)->f2; goto _LL1947;} else{ goto _LL1808;} _LL1947: _temp1946=
_temp1944.targs; goto _LL1931;} else{ goto _LL1808;} _LL1931: _temp1930=
_temp1788.f2; if(( unsigned int) _temp1930 >  4u?*(( int*) _temp1930) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1933: _temp1932=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1930)->f1;
_LL1937: _temp1936=( void*) _temp1932.field_info; if(*(( int*) _temp1936) == 
Cyc_Absyn_KnownTunionfield){ _LL1941: _temp1940=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1936)->f1; goto _LL1939; _LL1939: _temp1938=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1936)->f2; goto _LL1935;} else{ goto _LL1808;} _LL1935: _temp1934=
_temp1932.targs; goto _LL1807;} else{ goto _LL1808;} _LL1808: _LL1969: _temp1968=
_temp1788.f1; if(( unsigned int) _temp1968 >  4u?*(( int*) _temp1968) ==  Cyc_Absyn_PointerType:
0){ _LL1971: _temp1970=(( struct Cyc_Absyn_PointerType_struct*) _temp1968)->f1;
_LL1981: _temp1980=( void*) _temp1970.elt_typ; goto _LL1979; _LL1979: _temp1978=(
void*) _temp1970.rgn_typ; goto _LL1977; _LL1977: _temp1976= _temp1970.nullable;
goto _LL1975; _LL1975: _temp1974= _temp1970.tq; goto _LL1973; _LL1973: _temp1972=
_temp1970.bounds; goto _LL1955;} else{ goto _LL1810;} _LL1955: _temp1954=
_temp1788.f2; if(( unsigned int) _temp1954 >  4u?*(( int*) _temp1954) ==  Cyc_Absyn_PointerType:
0){ _LL1957: _temp1956=(( struct Cyc_Absyn_PointerType_struct*) _temp1954)->f1;
_LL1967: _temp1966=( void*) _temp1956.elt_typ; goto _LL1965; _LL1965: _temp1964=(
void*) _temp1956.rgn_typ; goto _LL1963; _LL1963: _temp1962= _temp1956.nullable;
goto _LL1961; _LL1961: _temp1960= _temp1956.tq; goto _LL1959; _LL1959: _temp1958=
_temp1956.bounds; goto _LL1809;} else{ goto _LL1810;} _LL1810: _LL1989:
_temp1988= _temp1788.f1; if(( unsigned int) _temp1988 >  4u?*(( int*) _temp1988)
==  Cyc_Absyn_IntType: 0){ _LL1993: _temp1992=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1988)->f1; goto _LL1991; _LL1991: _temp1990=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1988)->f2; goto _LL1983;} else{ goto _LL1812;} _LL1983: _temp1982=
_temp1788.f2; if(( unsigned int) _temp1982 >  4u?*(( int*) _temp1982) ==  Cyc_Absyn_IntType:
0){ _LL1987: _temp1986=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1982)->f1;
goto _LL1985; _LL1985: _temp1984=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1982)->f2; goto _LL1811;} else{ goto _LL1812;} _LL1812: _LL1997: _temp1996=
_temp1788.f1; if( _temp1996 == ( void*) Cyc_Absyn_FloatType){ goto _LL1995;}
else{ goto _LL1814;} _LL1995: _temp1994= _temp1788.f2; if( _temp1994 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1813;} else{ goto _LL1814;} _LL1814: _LL2001:
_temp2000= _temp1788.f1; if( _temp2000 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1999;} else{ goto _LL1816;} _LL1999: _temp1998= _temp1788.f2; if( _temp1998
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1815;} else{ goto _LL1816;} _LL1816:
_LL2011: _temp2010= _temp1788.f1; if(( unsigned int) _temp2010 >  4u?*(( int*)
_temp2010) ==  Cyc_Absyn_ArrayType: 0){ _LL2017: _temp2016=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2010)->f1; goto _LL2015; _LL2015: _temp2014=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2010)->f2; goto _LL2013; _LL2013: _temp2012=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2010)->f3; goto _LL2003;} else{ goto _LL1818;} _LL2003: _temp2002=
_temp1788.f2; if(( unsigned int) _temp2002 >  4u?*(( int*) _temp2002) ==  Cyc_Absyn_ArrayType:
0){ _LL2009: _temp2008=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2002)->f1;
goto _LL2007; _LL2007: _temp2006=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2002)->f2; goto _LL2005; _LL2005: _temp2004=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2002)->f3; goto _LL1817;} else{ goto _LL1818;} _LL1818: _LL2039: _temp2038=
_temp1788.f1; if(( unsigned int) _temp2038 >  4u?*(( int*) _temp2038) ==  Cyc_Absyn_FnType:
0){ _LL2041: _temp2040=(( struct Cyc_Absyn_FnType_struct*) _temp2038)->f1;
_LL2057: _temp2056= _temp2040.tvars; goto _LL2055; _LL2055: _temp2054= _temp2040.effect;
goto _LL2053; _LL2053: _temp2052=( void*) _temp2040.ret_typ; goto _LL2051;
_LL2051: _temp2050= _temp2040.args; goto _LL2049; _LL2049: _temp2048= _temp2040.c_varargs;
goto _LL2047; _LL2047: _temp2046= _temp2040.cyc_varargs; goto _LL2045; _LL2045:
_temp2044= _temp2040.rgn_po; goto _LL2043; _LL2043: _temp2042= _temp2040.attributes;
goto _LL2019;} else{ goto _LL1820;} _LL2019: _temp2018= _temp1788.f2; if((
unsigned int) _temp2018 >  4u?*(( int*) _temp2018) ==  Cyc_Absyn_FnType: 0){
_LL2021: _temp2020=(( struct Cyc_Absyn_FnType_struct*) _temp2018)->f1; _LL2037:
_temp2036= _temp2020.tvars; goto _LL2035; _LL2035: _temp2034= _temp2020.effect;
goto _LL2033; _LL2033: _temp2032=( void*) _temp2020.ret_typ; goto _LL2031;
_LL2031: _temp2030= _temp2020.args; goto _LL2029; _LL2029: _temp2028= _temp2020.c_varargs;
goto _LL2027; _LL2027: _temp2026= _temp2020.cyc_varargs; goto _LL2025; _LL2025:
_temp2024= _temp2020.rgn_po; goto _LL2023; _LL2023: _temp2022= _temp2020.attributes;
goto _LL1819;} else{ goto _LL1820;} _LL1820: _LL2063: _temp2062= _temp1788.f1;
if(( unsigned int) _temp2062 >  4u?*(( int*) _temp2062) ==  Cyc_Absyn_TupleType:
0){ _LL2065: _temp2064=(( struct Cyc_Absyn_TupleType_struct*) _temp2062)->f1;
goto _LL2059;} else{ goto _LL1822;} _LL2059: _temp2058= _temp1788.f2; if((
unsigned int) _temp2058 >  4u?*(( int*) _temp2058) ==  Cyc_Absyn_TupleType: 0){
_LL2061: _temp2060=(( struct Cyc_Absyn_TupleType_struct*) _temp2058)->f1; goto
_LL1821;} else{ goto _LL1822;} _LL1822: _LL2071: _temp2070= _temp1788.f1; if((
unsigned int) _temp2070 >  4u?*(( int*) _temp2070) ==  Cyc_Absyn_AnonStructType:
0){ _LL2073: _temp2072=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2070)->f1;
goto _LL2067;} else{ goto _LL1824;} _LL2067: _temp2066= _temp1788.f2; if((
unsigned int) _temp2066 >  4u?*(( int*) _temp2066) ==  Cyc_Absyn_AnonStructType:
0){ _LL2069: _temp2068=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2066)->f1;
goto _LL1823;} else{ goto _LL1824;} _LL1824: _LL2079: _temp2078= _temp1788.f1;
if(( unsigned int) _temp2078 >  4u?*(( int*) _temp2078) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2081: _temp2080=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2078)->f1;
goto _LL2075;} else{ goto _LL1826;} _LL2075: _temp2074= _temp1788.f2; if((
unsigned int) _temp2074 >  4u?*(( int*) _temp2074) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2077: _temp2076=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2074)->f1;
goto _LL1825;} else{ goto _LL1826;} _LL1826: _LL2085: _temp2084= _temp1788.f1;
if( _temp2084 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL2083;} else{ goto _LL1828;}
_LL2083: _temp2082= _temp1788.f2; if( _temp2082 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1827;} else{ goto _LL1828;} _LL1828: _LL2091: _temp2090= _temp1788.f1;
if(( unsigned int) _temp2090 >  4u?*(( int*) _temp2090) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2093: _temp2092=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2090)->f1; goto _LL2087;} else{ goto _LL1830;} _LL2087: _temp2086=
_temp1788.f2; if(( unsigned int) _temp2086 >  4u?*(( int*) _temp2086) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2089: _temp2088=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2086)->f1; goto _LL1829;} else{ goto _LL1830;} _LL1830: _LL2095: _temp2094=
_temp1788.f1; if(( unsigned int) _temp2094 >  4u?*(( int*) _temp2094) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1831;} else{ goto _LL1832;} _LL1832: _LL2097: _temp2096= _temp1788.f2;
if(( unsigned int) _temp2096 >  4u?*(( int*) _temp2096) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1833;} else{ goto _LL1834;} _LL1834: _LL2099: _temp2098= _temp1788.f1;
if(( unsigned int) _temp2098 >  4u?*(( int*) _temp2098) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1835;} else{ goto _LL1836;} _LL1836: _LL2101: _temp2100= _temp1788.f1;
if(( unsigned int) _temp2100 >  4u?*(( int*) _temp2100) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1837;} else{ goto _LL1838;} _LL1838: _LL2103: _temp2102= _temp1788.f2;
if(( unsigned int) _temp2102 >  4u?*(( int*) _temp2102) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1839;} else{ goto _LL1840;} _LL1840: _LL2105: _temp2104= _temp1788.f2;
if(( unsigned int) _temp2104 >  4u?*(( int*) _temp2104) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1841;} else{ goto _LL1842;} _LL1842: goto _LL1843; _LL1791:({ void*
_temp2106[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types", sizeof( unsigned char), 39u),
_tag_arr( _temp2106, sizeof( void*), 0u));}); return 0; _LL1793: return 0;
_LL1795: { struct _tagged_arr* _temp2107= _temp1854->name; struct _tagged_arr*
_temp2108= _temp1858->name; int _temp2109=*(( int*) _check_null( _temp1854->identity));
int _temp2110=*(( int*) _check_null( _temp1858->identity)); return Cyc_Core_intcmp(
_temp2109, _temp2110);} _LL1797: { int _temp2111=(( int(*)( int(* cmp)( struct
_tuple1*, struct _tuple1*), struct _tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)(
Cyc_Absyn_qvar_cmp, _temp1864, _temp1870); if( _temp2111 !=  0){ return
_temp2111;} else{ return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp1862,
_temp1868);}} _LL1799: { int _temp2112= Cyc_Absyn_qvar_cmp( _temp1878, _temp1874);
return _temp2112;} _LL1801: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l1, struct Cyc_List_List* l2))
Cyc_Tcutil_list_cmp)( Cyc_Tcutil_enumfield_cmp, _temp1886, _temp1882); _LL1803: {
int _temp2113=(( int(*)( int(* cmp)( struct _tuple1*, struct _tuple1*), struct
_tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)( Cyc_Absyn_qvar_cmp,
_temp1894, _temp1902); if( _temp2113 !=  0){ return _temp2113;} else{ return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1892, _temp1900);}} _LL1805: if( _temp1916 == 
_temp1929){ return 0;}{ int _temp2114= Cyc_Absyn_qvar_cmp( _temp1916->name,
_temp1929->name); if( _temp2114 !=  0){ return _temp2114;}{ int _temp2115= Cyc_Tcutil_typecmp(
_temp1908, _temp1921); if( _temp2115 !=  0){ return _temp2115;} return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1910, _temp1923);}} _LL1807: if( _temp1940 == 
_temp1952){ return 0;}{ int _temp2116= Cyc_Absyn_qvar_cmp( _temp1952->name,
_temp1940->name); if( _temp2116 !=  0){ return _temp2116;}{ int _temp2117= Cyc_Absyn_qvar_cmp(
_temp1950->name, _temp1938->name); if( _temp2117 !=  0){ return _temp2117;}
return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp1934, _temp1946);}} _LL1809: {
int _temp2118= Cyc_Tcutil_typecmp( _temp1966, _temp1980); if( _temp2118 !=  0){
return _temp2118;}{ int _temp2119= Cyc_Tcutil_typecmp( _temp1964, _temp1978);
if( _temp2119 !=  0){ return _temp2119;}{ int _temp2120= Cyc_Tcutil_tqual_cmp(
_temp1960, _temp1974); if( _temp2120 !=  0){ return _temp2120;}{ int _temp2121=
Cyc_Tcutil_conrefs_cmp( Cyc_Tcutil_boundscmp, _temp1958, _temp1972); if(
_temp2121 !=  0){ return _temp2121;}{ void* _temp2122=( void*)( Cyc_Absyn_compress_conref(
_temp1958))->v; void* _temp2128; _LL2124: if(( unsigned int) _temp2122 >  1u?*((
int*) _temp2122) ==  Cyc_Absyn_Eq_constr: 0){ _LL2129: _temp2128=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2122)->f1; if( _temp2128 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2125;} else{ goto _LL2126;}} else{ goto _LL2126;}
_LL2126: goto _LL2127; _LL2125: return 0; _LL2127: goto _LL2123; _LL2123:;}
return(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_conrefs_cmp)( Cyc_Core_intcmp, _temp1962, _temp1976);}}}} _LL1811:
if( _temp1986 !=  _temp1992){ return Cyc_Core_intcmp(( int)(( unsigned int)
_temp1986),( int)(( unsigned int) _temp1992));} if( _temp1984 !=  _temp1990){
return Cyc_Core_intcmp(( int)(( unsigned int) _temp1984),( int)(( unsigned int)
_temp1990));} return 0; _LL1813: return 0; _LL1815: return 0; _LL1817: { int
_temp2130= Cyc_Tcutil_tqual_cmp( _temp2006, _temp2014); if( _temp2130 !=  0){
return _temp2130;}{ int _temp2131= Cyc_Tcutil_typecmp( _temp2008, _temp2016);
if( _temp2131 !=  0){ return _temp2131;} if( _temp2012 ==  _temp2004){ return 0;}
if( _temp2012 ==  0? 1: _temp2004 ==  0){({ void* _temp2132[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("missing expression in array index",
sizeof( unsigned char), 34u), _tag_arr( _temp2132, sizeof( void*), 0u));});}
return(( int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp* a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
_temp2012, _temp2004);}} _LL1819:({ void* _temp2133[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: function types not handled",
sizeof( unsigned char), 36u), _tag_arr( _temp2133, sizeof( void*), 0u));}); goto
_LL1789; _LL1821: return(( int(*)( int(* cmp)( struct _tuple4*, struct _tuple4*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_tqual_type_cmp,
_temp2060, _temp2064); _LL1823: _temp2080= _temp2072; _temp2076= _temp2068; goto
_LL1825; _LL1825: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Structfield*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_structfield_cmp, _temp2076, _temp2080);
_LL1827: return 0; _LL1829: return Cyc_Tcutil_typecmp( _temp2092, _temp2088);
_LL1831: goto _LL1833; _LL1833: goto _LL1835; _LL1835: goto _LL1837; _LL1837:
goto _LL1839; _LL1839: goto _LL1841; _LL1841:({ void* _temp2134[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: effects not handled",
sizeof( unsigned char), 29u), _tag_arr( _temp2134, sizeof( void*), 0u));}); goto
_LL1789; _LL1843: goto _LL1789; _LL1789:;} return({ void* _temp2135[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("Unmatched case in typecmp", sizeof( unsigned char), 26u), _tag_arr(
_temp2135, sizeof( void*), 0u));});}} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp2136= Cyc_Tcutil_compress( t); _LL2138: if(( unsigned int) _temp2136
>  4u?*(( int*) _temp2136) ==  Cyc_Absyn_IntType: 0){ goto _LL2139;} else{ goto
_LL2140;} _LL2140: if( _temp2136 == ( void*) Cyc_Absyn_FloatType){ goto _LL2141;}
else{ goto _LL2142;} _LL2142: if( _temp2136 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2143;} else{ goto _LL2144;} _LL2144: if(( unsigned int) _temp2136 >  4u?*((
int*) _temp2136) ==  Cyc_Absyn_EnumType: 0){ goto _LL2145;} else{ goto _LL2146;}
_LL2146: if(( unsigned int) _temp2136 >  4u?*(( int*) _temp2136) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL2147;} else{ goto _LL2148;} _LL2148: goto _LL2149; _LL2139: goto
_LL2141; _LL2141: goto _LL2143; _LL2143: goto _LL2145; _LL2145: return 1;
_LL2147: return 1; _LL2149: return 0; _LL2137:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp2151=({ struct _tuple8 _temp2150; _temp2150.f1= t1;
_temp2150.f2= t2; _temp2150;}); void* _temp2173; void* _temp2175; void*
_temp2177; void* _temp2179; void* _temp2181; void* _temp2183; void* _temp2185;
void* _temp2187; void* _temp2189; void* _temp2191; void* _temp2193; void*
_temp2195; void* _temp2197; void* _temp2199; void* _temp2201; void* _temp2203;
void* _temp2205; void* _temp2207; void* _temp2209; void* _temp2211; void*
_temp2213; void* _temp2215; void* _temp2217; void* _temp2219; void* _temp2221;
void* _temp2223; void* _temp2225; _LL2153: _LL2176: _temp2175= _temp2151.f1; if(
_temp2175 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2174;} else{ goto _LL2155;}
_LL2174: _temp2173= _temp2151.f2; if( _temp2173 == ( void*) Cyc_Absyn_FloatType){
goto _LL2154;} else{ goto _LL2155;} _LL2155: _LL2180: _temp2179= _temp2151.f1;
if( _temp2179 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2178;} else{ goto
_LL2157;} _LL2178: _temp2177= _temp2151.f2; if(( unsigned int) _temp2177 >  4u?*((
int*) _temp2177) ==  Cyc_Absyn_IntType: 0){ goto _LL2156;} else{ goto _LL2157;}
_LL2157: _LL2184: _temp2183= _temp2151.f1; if( _temp2183 == ( void*) Cyc_Absyn_FloatType){
goto _LL2182;} else{ goto _LL2159;} _LL2182: _temp2181= _temp2151.f2; if((
unsigned int) _temp2181 >  4u?*(( int*) _temp2181) ==  Cyc_Absyn_IntType: 0){
goto _LL2158;} else{ goto _LL2159;} _LL2159: _LL2190: _temp2189= _temp2151.f1;
if(( unsigned int) _temp2189 >  4u?*(( int*) _temp2189) ==  Cyc_Absyn_IntType: 0){
_LL2192: _temp2191=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2189)->f2;
if( _temp2191 == ( void*) Cyc_Absyn_B8){ goto _LL2186;} else{ goto _LL2161;}}
else{ goto _LL2161;} _LL2186: _temp2185= _temp2151.f2; if(( unsigned int)
_temp2185 >  4u?*(( int*) _temp2185) ==  Cyc_Absyn_IntType: 0){ _LL2188:
_temp2187=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2185)->f2; if(
_temp2187 == ( void*) Cyc_Absyn_B8){ goto _LL2160;} else{ goto _LL2161;}} else{
goto _LL2161;} _LL2161: _LL2194: _temp2193= _temp2151.f1; if(( unsigned int)
_temp2193 >  4u?*(( int*) _temp2193) ==  Cyc_Absyn_IntType: 0){ _LL2196:
_temp2195=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2193)->f2; if(
_temp2195 == ( void*) Cyc_Absyn_B8){ goto _LL2162;} else{ goto _LL2163;}} else{
goto _LL2163;} _LL2163: _LL2200: _temp2199= _temp2151.f1; if(( unsigned int)
_temp2199 >  4u?*(( int*) _temp2199) ==  Cyc_Absyn_IntType: 0){ _LL2202:
_temp2201=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2199)->f2; if(
_temp2201 == ( void*) Cyc_Absyn_B4){ goto _LL2198;} else{ goto _LL2165;}} else{
goto _LL2165;} _LL2198: _temp2197= _temp2151.f2; if( _temp2197 == ( void*) Cyc_Absyn_FloatType){
goto _LL2164;} else{ goto _LL2165;} _LL2165: _LL2208: _temp2207= _temp2151.f1;
if(( unsigned int) _temp2207 >  4u?*(( int*) _temp2207) ==  Cyc_Absyn_IntType: 0){
_LL2210: _temp2209=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2207)->f2;
if( _temp2209 == ( void*) Cyc_Absyn_B4){ goto _LL2204;} else{ goto _LL2167;}}
else{ goto _LL2167;} _LL2204: _temp2203= _temp2151.f2; if(( unsigned int)
_temp2203 >  4u?*(( int*) _temp2203) ==  Cyc_Absyn_IntType: 0){ _LL2206:
_temp2205=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2203)->f2; if(
_temp2205 == ( void*) Cyc_Absyn_B2){ goto _LL2166;} else{ goto _LL2167;}} else{
goto _LL2167;} _LL2167: _LL2216: _temp2215= _temp2151.f1; if(( unsigned int)
_temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_IntType: 0){ _LL2218:
_temp2217=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2215)->f2; if(
_temp2217 == ( void*) Cyc_Absyn_B4){ goto _LL2212;} else{ goto _LL2169;}} else{
goto _LL2169;} _LL2212: _temp2211= _temp2151.f2; if(( unsigned int) _temp2211 > 
4u?*(( int*) _temp2211) ==  Cyc_Absyn_IntType: 0){ _LL2214: _temp2213=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2211)->f2; if( _temp2213 == ( void*) Cyc_Absyn_B1){
goto _LL2168;} else{ goto _LL2169;}} else{ goto _LL2169;} _LL2169: _LL2224:
_temp2223= _temp2151.f1; if(( unsigned int) _temp2223 >  4u?*(( int*) _temp2223)
==  Cyc_Absyn_IntType: 0){ _LL2226: _temp2225=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2223)->f2; if( _temp2225 == ( void*) Cyc_Absyn_B2){ goto _LL2220;} else{
goto _LL2171;}} else{ goto _LL2171;} _LL2220: _temp2219= _temp2151.f2; if((
unsigned int) _temp2219 >  4u?*(( int*) _temp2219) ==  Cyc_Absyn_IntType: 0){
_LL2222: _temp2221=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2219)->f2;
if( _temp2221 == ( void*) Cyc_Absyn_B1){ goto _LL2170;} else{ goto _LL2171;}}
else{ goto _LL2171;} _LL2171: goto _LL2172; _LL2154: goto _LL2156; _LL2156: goto
_LL2158; _LL2158: return 1; _LL2160: return 0; _LL2162: goto _LL2164; _LL2164:
goto _LL2166; _LL2166: goto _LL2168; _LL2168: goto _LL2170; _LL2170: return 1;
_LL2172: return 0; _LL2152:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) el->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*) max_arith_type->v)){ max_arith_type=( struct Cyc_Core_Opt*)({ struct
Cyc_Core_Opt* _temp2227=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2227->v=( void*) t1; _temp2227;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify(
t,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*) el->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp2230; _temp2230.tag= Cyc_Std_String_pa; _temp2230.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*) el->hd)->topt))->v);{ struct Cyc_Std_String_pa_struct _temp2229;
_temp2229.tag= Cyc_Std_String_pa; _temp2229.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2228[ 2u]={& _temp2229,& _temp2230}; Cyc_Tcutil_terr((( struct
Cyc_Absyn_Exp*) el->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp2228, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp2231= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL2233: if(( unsigned int) _temp2231 >  4u?*(( int*) _temp2231) ==  Cyc_Absyn_PointerType:
0){ goto _LL2234;} else{ goto _LL2235;} _LL2235: goto _LL2236; _LL2234: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL2232; _LL2236: return 0; _LL2232:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp2237= Cyc_Tcutil_compress(
t); _LL2239: if(( unsigned int) _temp2237 >  4u?*(( int*) _temp2237) ==  Cyc_Absyn_IntType:
0){ goto _LL2240;} else{ goto _LL2241;} _LL2241: if(( unsigned int) _temp2237 > 
4u?*(( int*) _temp2237) ==  Cyc_Absyn_EnumType: 0){ goto _LL2242;} else{ goto
_LL2243;} _LL2243: if(( unsigned int) _temp2237 >  4u?*(( int*) _temp2237) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2244;} else{ goto _LL2245;} _LL2245: goto
_LL2246; _LL2240: goto _LL2242; _LL2242: return 1; _LL2244: return 1; _LL2246:
return 0; _LL2238:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp2247[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2247, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp2248[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp2248, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp2250=({ struct _tuple8 _temp2249; _temp2249.f1= t1; _temp2249.f2= t2;
_temp2249;}); void* _temp2262; struct Cyc_Absyn_PtrInfo _temp2264; void*
_temp2266; struct Cyc_Absyn_PtrInfo _temp2268; void* _temp2270; struct Cyc_Absyn_Exp*
_temp2272; struct Cyc_Absyn_Tqual _temp2274; void* _temp2276; void* _temp2278;
struct Cyc_Absyn_Exp* _temp2280; struct Cyc_Absyn_Tqual _temp2282; void*
_temp2284; void* _temp2286; struct Cyc_Absyn_TunionInfo _temp2288; struct Cyc_List_List*
_temp2290; void* _temp2292; struct Cyc_Absyn_Tuniondecl** _temp2294; struct Cyc_Absyn_Tuniondecl*
_temp2296; void* _temp2297; struct Cyc_Absyn_TunionFieldInfo _temp2299; struct
Cyc_List_List* _temp2301; void* _temp2303; struct Cyc_Absyn_Tunionfield*
_temp2305; struct Cyc_Absyn_Tuniondecl* _temp2307; void* _temp2309; struct Cyc_Absyn_TunionInfo
_temp2311; void* _temp2313; struct Cyc_List_List* _temp2315; void* _temp2317;
struct Cyc_Absyn_Tuniondecl** _temp2319; struct Cyc_Absyn_Tuniondecl* _temp2321;
void* _temp2322; struct Cyc_Absyn_PtrInfo _temp2324; struct Cyc_Absyn_Conref*
_temp2326; struct Cyc_Absyn_Tqual _temp2328; struct Cyc_Absyn_Conref* _temp2330;
void* _temp2332; void* _temp2334; _LL2252: _LL2267: _temp2266= _temp2250.f1; if((
unsigned int) _temp2266 >  4u?*(( int*) _temp2266) ==  Cyc_Absyn_PointerType: 0){
_LL2269: _temp2268=(( struct Cyc_Absyn_PointerType_struct*) _temp2266)->f1; goto
_LL2263;} else{ goto _LL2254;} _LL2263: _temp2262= _temp2250.f2; if((
unsigned int) _temp2262 >  4u?*(( int*) _temp2262) ==  Cyc_Absyn_PointerType: 0){
_LL2265: _temp2264=(( struct Cyc_Absyn_PointerType_struct*) _temp2262)->f1; goto
_LL2253;} else{ goto _LL2254;} _LL2254: _LL2279: _temp2278= _temp2250.f1; if((
unsigned int) _temp2278 >  4u?*(( int*) _temp2278) ==  Cyc_Absyn_ArrayType: 0){
_LL2285: _temp2284=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2278)->f1;
goto _LL2283; _LL2283: _temp2282=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2278)->f2; goto _LL2281; _LL2281: _temp2280=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2278)->f3; goto _LL2271;} else{ goto _LL2256;} _LL2271: _temp2270=
_temp2250.f2; if(( unsigned int) _temp2270 >  4u?*(( int*) _temp2270) ==  Cyc_Absyn_ArrayType:
0){ _LL2277: _temp2276=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2270)->f1;
goto _LL2275; _LL2275: _temp2274=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2270)->f2; goto _LL2273; _LL2273: _temp2272=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2270)->f3; goto _LL2255;} else{ goto _LL2256;} _LL2256: _LL2298: _temp2297=
_temp2250.f1; if(( unsigned int) _temp2297 >  4u?*(( int*) _temp2297) ==  Cyc_Absyn_TunionFieldType:
0){ _LL2300: _temp2299=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2297)->f1;
_LL2304: _temp2303=( void*) _temp2299.field_info; if(*(( int*) _temp2303) == 
Cyc_Absyn_KnownTunionfield){ _LL2308: _temp2307=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2303)->f1; goto _LL2306; _LL2306: _temp2305=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2303)->f2; goto _LL2302;} else{ goto _LL2258;} _LL2302: _temp2301=
_temp2299.targs; goto _LL2287;} else{ goto _LL2258;} _LL2287: _temp2286=
_temp2250.f2; if(( unsigned int) _temp2286 >  4u?*(( int*) _temp2286) ==  Cyc_Absyn_TunionType:
0){ _LL2289: _temp2288=(( struct Cyc_Absyn_TunionType_struct*) _temp2286)->f1;
_LL2293: _temp2292=( void*) _temp2288.tunion_info; if(*(( int*) _temp2292) == 
Cyc_Absyn_KnownTunion){ _LL2295: _temp2294=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2292)->f1; _temp2296=* _temp2294; goto _LL2291;} else{ goto _LL2258;}
_LL2291: _temp2290= _temp2288.targs; goto _LL2257;} else{ goto _LL2258;} _LL2258:
_LL2323: _temp2322= _temp2250.f1; if(( unsigned int) _temp2322 >  4u?*(( int*)
_temp2322) ==  Cyc_Absyn_PointerType: 0){ _LL2325: _temp2324=(( struct Cyc_Absyn_PointerType_struct*)
_temp2322)->f1; _LL2335: _temp2334=( void*) _temp2324.elt_typ; goto _LL2333;
_LL2333: _temp2332=( void*) _temp2324.rgn_typ; goto _LL2331; _LL2331: _temp2330=
_temp2324.nullable; goto _LL2329; _LL2329: _temp2328= _temp2324.tq; goto _LL2327;
_LL2327: _temp2326= _temp2324.bounds; goto _LL2310;} else{ goto _LL2260;}
_LL2310: _temp2309= _temp2250.f2; if(( unsigned int) _temp2309 >  4u?*(( int*)
_temp2309) ==  Cyc_Absyn_TunionType: 0){ _LL2312: _temp2311=(( struct Cyc_Absyn_TunionType_struct*)
_temp2309)->f1; _LL2318: _temp2317=( void*) _temp2311.tunion_info; if(*(( int*)
_temp2317) ==  Cyc_Absyn_KnownTunion){ _LL2320: _temp2319=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2317)->f1; _temp2321=* _temp2319; goto _LL2316;} else{ goto _LL2260;}
_LL2316: _temp2315= _temp2311.targs; goto _LL2314; _LL2314: _temp2313=( void*)
_temp2311.rgn; goto _LL2259;} else{ goto _LL2260;} _LL2260: goto _LL2261;
_LL2253: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp2268.nullable, _temp2264.nullable)){ void* _temp2336=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2268.nullable))->v;
int _temp2342; _LL2338: if(( unsigned int) _temp2336 >  1u?*(( int*) _temp2336)
==  Cyc_Absyn_Eq_constr: 0){ _LL2343: _temp2342=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2336)->f1; goto _LL2339;} else{ goto _LL2340;} _LL2340: goto _LL2341;
_LL2339: okay= ! _temp2342; goto _LL2337; _LL2341:( int) _throw(({ void*
_temp2344[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp2344, sizeof( void*), 0u));}));
_LL2337:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2268.bounds,
_temp2264.bounds)){ struct _tuple8 _temp2346=({ struct _tuple8 _temp2345;
_temp2345.f1=( void*)( Cyc_Absyn_compress_conref( _temp2268.bounds))->v;
_temp2345.f2=( void*)( Cyc_Absyn_compress_conref( _temp2264.bounds))->v;
_temp2345;}); void* _temp2356; void* _temp2358; void* _temp2360; void* _temp2362;
void* _temp2364; void* _temp2366; struct Cyc_Absyn_Exp* _temp2368; void*
_temp2370; void* _temp2372; struct Cyc_Absyn_Exp* _temp2374; void* _temp2376;
void* _temp2378; void* _temp2380; void* _temp2382; _LL2348: _LL2361: _temp2360=
_temp2346.f1; if(( unsigned int) _temp2360 >  1u?*(( int*) _temp2360) ==  Cyc_Absyn_Eq_constr:
0){ _LL2363: _temp2362=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2360)->f1;
if(( unsigned int) _temp2362 >  1u?*(( int*) _temp2362) ==  Cyc_Absyn_Upper_b: 0){
goto _LL2357;} else{ goto _LL2350;}} else{ goto _LL2350;} _LL2357: _temp2356=
_temp2346.f2; if(( unsigned int) _temp2356 >  1u?*(( int*) _temp2356) ==  Cyc_Absyn_Eq_constr:
0){ _LL2359: _temp2358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2356)->f1;
if( _temp2358 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2349;} else{ goto
_LL2350;}} else{ goto _LL2350;} _LL2350: _LL2371: _temp2370= _temp2346.f1; if((
unsigned int) _temp2370 >  1u?*(( int*) _temp2370) ==  Cyc_Absyn_Eq_constr: 0){
_LL2373: _temp2372=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2370)->f1;
if(( unsigned int) _temp2372 >  1u?*(( int*) _temp2372) ==  Cyc_Absyn_Upper_b: 0){
_LL2375: _temp2374=(( struct Cyc_Absyn_Upper_b_struct*) _temp2372)->f1; goto
_LL2365;} else{ goto _LL2352;}} else{ goto _LL2352;} _LL2365: _temp2364=
_temp2346.f2; if(( unsigned int) _temp2364 >  1u?*(( int*) _temp2364) ==  Cyc_Absyn_Eq_constr:
0){ _LL2367: _temp2366=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2364)->f1;
if(( unsigned int) _temp2366 >  1u?*(( int*) _temp2366) ==  Cyc_Absyn_Upper_b: 0){
_LL2369: _temp2368=(( struct Cyc_Absyn_Upper_b_struct*) _temp2366)->f1; goto
_LL2351;} else{ goto _LL2352;}} else{ goto _LL2352;} _LL2352: _LL2381: _temp2380=
_temp2346.f1; if(( unsigned int) _temp2380 >  1u?*(( int*) _temp2380) ==  Cyc_Absyn_Eq_constr:
0){ _LL2383: _temp2382=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2380)->f1;
if( _temp2382 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2377;} else{ goto
_LL2354;}} else{ goto _LL2354;} _LL2377: _temp2376= _temp2346.f2; if((
unsigned int) _temp2376 >  1u?*(( int*) _temp2376) ==  Cyc_Absyn_Eq_constr: 0){
_LL2379: _temp2378=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2376)->f1;
if( _temp2378 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2353;} else{ goto
_LL2354;}} else{ goto _LL2354;} _LL2354: goto _LL2355; _LL2349: okay= 1; goto
_LL2347; _LL2351: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp2374) >=  Cyc_Evexp_eval_const_uint_exp(
_temp2368): 0;({ void* _temp2384[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp2384, sizeof( void*), 0u));}); goto
_LL2347; _LL2353: okay= 1; goto _LL2347; _LL2355: okay= 0; goto _LL2347; _LL2347:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp2268.elt_typ,( void*) _temp2264.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp2268.rgn_typ,( void*) _temp2264.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp2268.rgn_typ,( void*) _temp2264.rgn_typ):
0; okay= okay? !( _temp2268.tq).q_const? 1:( _temp2264.tq).q_const: 0; return
okay;} _LL2255: { int okay; okay=( _temp2280 !=  0? _temp2272 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2280)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2272)): 0; return( okay? Cyc_Tcutil_unify(
_temp2284, _temp2276): 0)? ! _temp2282.q_const? 1: _temp2274.q_const: 0;}
_LL2257: if(( _temp2307 ==  _temp2296? 1: Cyc_Absyn_qvar_cmp( _temp2307->name,
_temp2296->name) ==  0)? _temp2305->typs ==  0: 0){ for( 0; _temp2301 !=  0?
_temp2290 !=  0: 0;( _temp2301= _temp2301->tl, _temp2290= _temp2290->tl)){ if( !
Cyc_Tcutil_unify(( void*) _temp2301->hd,( void*) _temp2290->hd)){ break;}} if(
_temp2301 ==  0? _temp2290 ==  0: 0){ return 1;}} return 0; _LL2259:{ void*
_temp2385= Cyc_Tcutil_compress( _temp2334); struct Cyc_Absyn_TunionFieldInfo
_temp2391; struct Cyc_List_List* _temp2393; void* _temp2395; struct Cyc_Absyn_Tunionfield*
_temp2397; struct Cyc_Absyn_Tuniondecl* _temp2399; _LL2387: if(( unsigned int)
_temp2385 >  4u?*(( int*) _temp2385) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2392:
_temp2391=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2385)->f1; _LL2396:
_temp2395=( void*) _temp2391.field_info; if(*(( int*) _temp2395) ==  Cyc_Absyn_KnownTunionfield){
_LL2400: _temp2399=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2395)->f1;
goto _LL2398; _LL2398: _temp2397=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2395)->f2; goto _LL2394;} else{ goto _LL2389;} _LL2394: _temp2393=
_temp2391.targs; goto _LL2388;} else{ goto _LL2389;} _LL2389: goto _LL2390;
_LL2388: if( ! Cyc_Tcutil_unify( _temp2332, _temp2313)? ! Cyc_Tcenv_region_outlives(
te, _temp2332, _temp2313): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp2330,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2326,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp2321->name, _temp2399->name) ==  0? _temp2397->typs !=  0: 0){ int okay= 1;
for( 0; _temp2393 !=  0? _temp2315 !=  0: 0;( _temp2393= _temp2393->tl,
_temp2315= _temp2315->tl)){ if( ! Cyc_Tcutil_unify(( void*) _temp2393->hd,( void*)
_temp2315->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp2393 !=  0)? 1:
_temp2315 !=  0){ return 0;} return 1;} goto _LL2386; _LL2390: goto _LL2386;
_LL2386:;} return 0; _LL2261: return Cyc_Tcutil_unify( t1, t2); _LL2251:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp2401[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2401, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Std_String_pa_struct
_temp2404; _temp2404.tag= Cyc_Std_String_pa; _temp2404.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct _temp2403;
_temp2403.tag= Cyc_Std_String_pa; _temp2403.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp2402[ 2u]={& _temp2403,& _temp2404}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp2402, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} int Cyc_Tcutil_coerceable( void* t){ void* _temp2405= Cyc_Tcutil_compress(
t); _LL2407: if(( unsigned int) _temp2405 >  4u?*(( int*) _temp2405) ==  Cyc_Absyn_IntType:
0){ goto _LL2408;} else{ goto _LL2409;} _LL2409: if( _temp2405 == ( void*) Cyc_Absyn_FloatType){
goto _LL2410;} else{ goto _LL2411;} _LL2411: if( _temp2405 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2412;} else{ goto _LL2413;} _LL2413: goto _LL2414; _LL2408: goto _LL2410;
_LL2410: goto _LL2412; _LL2412: return 1; _LL2414: return 0; _LL2406:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp2415=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp2415->f1= x->tq; _temp2415->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp2415;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp2416= t1; struct Cyc_List_List* _temp2428; struct Cyc_Absyn_Structdecl**
_temp2430; struct Cyc_List_List* _temp2432; struct _tuple1* _temp2434; struct
Cyc_List_List* _temp2436; _LL2418: if( _temp2416 == ( void*) Cyc_Absyn_VoidType){
goto _LL2419;} else{ goto _LL2420;} _LL2420: if(( unsigned int) _temp2416 >  4u?*((
int*) _temp2416) ==  Cyc_Absyn_TupleType: 0){ _LL2429: _temp2428=(( struct Cyc_Absyn_TupleType_struct*)
_temp2416)->f1; goto _LL2421;} else{ goto _LL2422;} _LL2422: if(( unsigned int)
_temp2416 >  4u?*(( int*) _temp2416) ==  Cyc_Absyn_StructType: 0){ _LL2435:
_temp2434=(( struct Cyc_Absyn_StructType_struct*) _temp2416)->f1; goto _LL2433;
_LL2433: _temp2432=(( struct Cyc_Absyn_StructType_struct*) _temp2416)->f2; goto
_LL2431; _LL2431: _temp2430=(( struct Cyc_Absyn_StructType_struct*) _temp2416)->f3;
goto _LL2423;} else{ goto _LL2424;} _LL2424: if(( unsigned int) _temp2416 >  4u?*((
int*) _temp2416) ==  Cyc_Absyn_AnonStructType: 0){ _LL2437: _temp2436=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2416)->f1; goto _LL2425;} else{ goto
_LL2426;} _LL2426: goto _LL2427; _LL2419: return 0; _LL2421: return _temp2428;
_LL2423: if( _temp2430 ==  0? 1: _temp2434 ==  0){ return({ struct Cyc_List_List*
_temp2438=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2438->hd=( void*)({ struct _tuple4* _temp2439=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp2439->f1= Cyc_Absyn_empty_tqual(); _temp2439->f2=
t1; _temp2439;}); _temp2438->tl= 0; _temp2438;});}{ struct Cyc_Absyn_Structdecl*
_temp2440=* _temp2430; struct _tuple1 _temp2441=* _temp2434; struct Cyc_List_List*
_temp2442=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp2440->tvs, _temp2432); if( _temp2440->fields ==  0){
return({ struct Cyc_List_List* _temp2443=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2443->hd=( void*)({ struct _tuple4*
_temp2444=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2444->f1=
Cyc_Absyn_empty_tqual(); _temp2444->f2= t1; _temp2444;}); _temp2443->tl= 0;
_temp2443;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp2442,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2440->fields))->v);}
_LL2425: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp2436); _LL2427: return({
struct Cyc_List_List* _temp2445=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2445->hd=( void*)({ struct _tuple4* _temp2446=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2446->f1= Cyc_Absyn_empty_tqual();
_temp2446->f2= t1; _temp2446;}); _temp2445->tl= 0; _temp2445;}); _LL2417:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a= a->tl){
if( Cyc_Tcutil_unify( t1,(*(( struct _tuple8*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*((
struct _tuple8*) a->hd)).f2): 0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp2448=({ struct _tuple8 _temp2447;
_temp2447.f1= t1; _temp2447.f2= t2; _temp2447;}); void* _temp2454; struct Cyc_Absyn_PtrInfo
_temp2456; struct Cyc_Absyn_Conref* _temp2458; struct Cyc_Absyn_Tqual _temp2460;
struct Cyc_Absyn_Conref* _temp2462; void* _temp2464; void* _temp2466; void*
_temp2468; struct Cyc_Absyn_PtrInfo _temp2470; struct Cyc_Absyn_Conref*
_temp2472; struct Cyc_Absyn_Tqual _temp2474; struct Cyc_Absyn_Conref* _temp2476;
void* _temp2478; void* _temp2480; _LL2450: _LL2469: _temp2468= _temp2448.f1; if((
unsigned int) _temp2468 >  4u?*(( int*) _temp2468) ==  Cyc_Absyn_PointerType: 0){
_LL2471: _temp2470=(( struct Cyc_Absyn_PointerType_struct*) _temp2468)->f1;
_LL2481: _temp2480=( void*) _temp2470.elt_typ; goto _LL2479; _LL2479: _temp2478=(
void*) _temp2470.rgn_typ; goto _LL2477; _LL2477: _temp2476= _temp2470.nullable;
goto _LL2475; _LL2475: _temp2474= _temp2470.tq; goto _LL2473; _LL2473: _temp2472=
_temp2470.bounds; goto _LL2455;} else{ goto _LL2452;} _LL2455: _temp2454=
_temp2448.f2; if(( unsigned int) _temp2454 >  4u?*(( int*) _temp2454) ==  Cyc_Absyn_PointerType:
0){ _LL2457: _temp2456=(( struct Cyc_Absyn_PointerType_struct*) _temp2454)->f1;
_LL2467: _temp2466=( void*) _temp2456.elt_typ; goto _LL2465; _LL2465: _temp2464=(
void*) _temp2456.rgn_typ; goto _LL2463; _LL2463: _temp2462= _temp2456.nullable;
goto _LL2461; _LL2461: _temp2460= _temp2456.tq; goto _LL2459; _LL2459: _temp2458=
_temp2456.bounds; goto _LL2451;} else{ goto _LL2452;} _LL2452: goto _LL2453;
_LL2451: if( _temp2474.q_const? ! _temp2460.q_const: 0){ return 0;} if(( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp2476, _temp2462)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2476): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2462): 0){ return 0;}
if( ! Cyc_Tcutil_unify( _temp2478, _temp2464)? ! Cyc_Tcenv_region_outlives( te,
_temp2478, _temp2464): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2472, _temp2458)){ struct _tuple8 _temp2483=({ struct _tuple8 _temp2482;
_temp2482.f1= Cyc_Absyn_conref_val( _temp2472); _temp2482.f2= Cyc_Absyn_conref_val(
_temp2458); _temp2482;}); void* _temp2491; void* _temp2493; void* _temp2495;
struct Cyc_Absyn_Exp* _temp2497; void* _temp2499; struct Cyc_Absyn_Exp*
_temp2501; _LL2485: _LL2494: _temp2493= _temp2483.f1; if(( unsigned int)
_temp2493 >  1u?*(( int*) _temp2493) ==  Cyc_Absyn_Upper_b: 0){ goto _LL2492;}
else{ goto _LL2487;} _LL2492: _temp2491= _temp2483.f2; if( _temp2491 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2486;} else{ goto _LL2487;} _LL2487: _LL2500:
_temp2499= _temp2483.f1; if(( unsigned int) _temp2499 >  1u?*(( int*) _temp2499)
==  Cyc_Absyn_Upper_b: 0){ _LL2502: _temp2501=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2499)->f1; goto _LL2496;} else{ goto _LL2489;} _LL2496: _temp2495=
_temp2483.f2; if(( unsigned int) _temp2495 >  1u?*(( int*) _temp2495) ==  Cyc_Absyn_Upper_b:
0){ _LL2498: _temp2497=(( struct Cyc_Absyn_Upper_b_struct*) _temp2495)->f1; goto
_LL2488;} else{ goto _LL2489;} _LL2489: goto _LL2490; _LL2486: goto _LL2484;
_LL2488: if( Cyc_Evexp_eval_const_uint_exp( _temp2501) <  Cyc_Evexp_eval_const_uint_exp(
_temp2497)){ return 0;} goto _LL2484; _LL2490: return 0; _LL2484:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp2503=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2503->hd=( void*)({ struct _tuple8*
_temp2504=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2504->f1=
t1; _temp2504->f2= t2; _temp2504;}); _temp2503->tl= assume; _temp2503;}),
_temp2480, _temp2466); _LL2453: return 0; _LL2449:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp2506=({ struct _tuple8 _temp2505;
_temp2505.f1= Cyc_Tcutil_compress( t1); _temp2505.f2= Cyc_Tcutil_compress( t2);
_temp2505;}); void* _temp2512; void* _temp2514; void* _temp2516; void* _temp2518;
_LL2508: _LL2517: _temp2516= _temp2506.f1; if(( unsigned int) _temp2516 >  4u?*((
int*) _temp2516) ==  Cyc_Absyn_IntType: 0){ _LL2519: _temp2518=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2516)->f2; goto _LL2513;} else{ goto _LL2510;}
_LL2513: _temp2512= _temp2506.f2; if(( unsigned int) _temp2512 >  4u?*(( int*)
_temp2512) ==  Cyc_Absyn_IntType: 0){ _LL2515: _temp2514=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2512)->f2; goto _LL2509;} else{ goto _LL2510;} _LL2510: goto _LL2511;
_LL2509: return _temp2518 ==  _temp2514; _LL2511: return 0; _LL2507:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2= tqs2->tl, tqs1= tqs1->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp2522; void* _temp2523; struct Cyc_Absyn_Tqual _temp2525;
struct _tuple4* _temp2520=( struct _tuple4*) tqs1->hd; _temp2522=* _temp2520;
_LL2526: _temp2525= _temp2522.f1; goto _LL2524; _LL2524: _temp2523= _temp2522.f2;
goto _LL2521; _LL2521: { struct _tuple4 _temp2529; void* _temp2530; struct Cyc_Absyn_Tqual
_temp2532; struct _tuple4* _temp2527=( struct _tuple4*) tqs2->hd; _temp2529=*
_temp2527; _LL2533: _temp2532= _temp2529.f1; goto _LL2531; _LL2531: _temp2530=
_temp2529.f2; goto _LL2528; _LL2528: if( _temp2532.q_const? Cyc_Tcutil_subtype(
te, assume, _temp2523, _temp2530): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp2523, _temp2530)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp2523,
_temp2530)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp2534= t2; struct Cyc_Absyn_Uniondecl** _temp2542; struct Cyc_List_List*
_temp2544; void* _temp2546; _LL2536: if(( unsigned int) _temp2534 >  4u?*(( int*)
_temp2534) ==  Cyc_Absyn_UnionType: 0){ _LL2545: _temp2544=(( struct Cyc_Absyn_UnionType_struct*)
_temp2534)->f2; goto _LL2543; _LL2543: _temp2542=(( struct Cyc_Absyn_UnionType_struct*)
_temp2534)->f3; goto _LL2537;} else{ goto _LL2538;} _LL2538: if(( unsigned int)
_temp2534 >  4u?*(( int*) _temp2534) ==  Cyc_Absyn_IntType: 0){ _LL2547:
_temp2546=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2534)->f2; if(
_temp2546 == ( void*) Cyc_Absyn_B4){ goto _LL2539;} else{ goto _LL2540;}} else{
goto _LL2540;} _LL2540: goto _LL2541; _LL2537: if( _temp2542 ==  0){ goto
_LL2535;}{ struct Cyc_Absyn_Uniondecl* _temp2548=* _temp2542; if( _temp2548->fields
==  0){ goto _LL2535;}{ struct Cyc_List_List* _temp2549=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2548->tvs,
_temp2544);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2548->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp2549,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ return 1;}}} goto _LL2535;}} _LL2539: if( Cyc_Tcutil_typ_kind(
t2) == ( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL2535; _LL2541: goto
_LL2535; _LL2535:;}{ void* _temp2550= t1; struct Cyc_Absyn_PtrInfo _temp2568;
struct Cyc_Absyn_Conref* _temp2570; struct Cyc_Absyn_Tqual _temp2572; struct Cyc_Absyn_Conref*
_temp2574; void* _temp2576; void* _temp2578; struct Cyc_Absyn_Exp* _temp2580;
struct Cyc_Absyn_Tqual _temp2582; void* _temp2584; struct Cyc_Absyn_Enumdecl*
_temp2586; _LL2552: if(( unsigned int) _temp2550 >  4u?*(( int*) _temp2550) == 
Cyc_Absyn_PointerType: 0){ _LL2569: _temp2568=(( struct Cyc_Absyn_PointerType_struct*)
_temp2550)->f1; _LL2579: _temp2578=( void*) _temp2568.elt_typ; goto _LL2577;
_LL2577: _temp2576=( void*) _temp2568.rgn_typ; goto _LL2575; _LL2575: _temp2574=
_temp2568.nullable; goto _LL2573; _LL2573: _temp2572= _temp2568.tq; goto _LL2571;
_LL2571: _temp2570= _temp2568.bounds; goto _LL2553;} else{ goto _LL2554;}
_LL2554: if(( unsigned int) _temp2550 >  4u?*(( int*) _temp2550) ==  Cyc_Absyn_ArrayType:
0){ _LL2585: _temp2584=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2550)->f1;
goto _LL2583; _LL2583: _temp2582=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2550)->f2; goto _LL2581; _LL2581: _temp2580=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2550)->f3; goto _LL2555;} else{ goto _LL2556;} _LL2556: if(( unsigned int)
_temp2550 >  4u?*(( int*) _temp2550) ==  Cyc_Absyn_EnumType: 0){ _LL2587:
_temp2586=(( struct Cyc_Absyn_EnumType_struct*) _temp2550)->f2; goto _LL2557;}
else{ goto _LL2558;} _LL2558: if(( unsigned int) _temp2550 >  4u?*(( int*)
_temp2550) ==  Cyc_Absyn_IntType: 0){ goto _LL2559;} else{ goto _LL2560;}
_LL2560: if( _temp2550 == ( void*) Cyc_Absyn_FloatType){ goto _LL2561;} else{
goto _LL2562;} _LL2562: if( _temp2550 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2563;} else{ goto _LL2564;} _LL2564: if( _temp2550 == ( void*) Cyc_Absyn_VoidType){
goto _LL2565;} else{ goto _LL2566;} _LL2566: goto _LL2567; _LL2553:{ void*
_temp2588= t2; struct Cyc_Absyn_PtrInfo _temp2596; struct Cyc_Absyn_Conref*
_temp2598; struct Cyc_Absyn_Tqual _temp2600; struct Cyc_Absyn_Conref* _temp2602;
void* _temp2604; void* _temp2606; _LL2590: if(( unsigned int) _temp2588 >  4u?*((
int*) _temp2588) ==  Cyc_Absyn_PointerType: 0){ _LL2597: _temp2596=(( struct Cyc_Absyn_PointerType_struct*)
_temp2588)->f1; _LL2607: _temp2606=( void*) _temp2596.elt_typ; goto _LL2605;
_LL2605: _temp2604=( void*) _temp2596.rgn_typ; goto _LL2603; _LL2603: _temp2602=
_temp2596.nullable; goto _LL2601; _LL2601: _temp2600= _temp2596.tq; goto _LL2599;
_LL2599: _temp2598= _temp2596.bounds; goto _LL2591;} else{ goto _LL2592;}
_LL2592: if( _temp2588 == ( void*) Cyc_Absyn_VoidType){ goto _LL2593;} else{
goto _LL2594;} _LL2594: goto _LL2595; _LL2591: { struct Cyc_List_List* _temp2608=({
struct Cyc_List_List* _temp2625=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2625->hd=( void*)({ struct _tuple8* _temp2626=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2626->f1= t1;
_temp2626->f2= t2; _temp2626;}); _temp2625->tl= 0; _temp2625;}); int _temp2609=
Cyc_Tcutil_ptrsubtype( te, _temp2608, _temp2578, _temp2606)? ! _temp2572.q_const?
1: _temp2600.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2570, _temp2598); if( ! bounds_ok){ struct _tuple8 _temp2611=({ struct
_tuple8 _temp2610; _temp2610.f1= Cyc_Absyn_conref_val( _temp2570); _temp2610.f2=
Cyc_Absyn_conref_val( _temp2598); _temp2610;}); void* _temp2617; struct Cyc_Absyn_Exp*
_temp2619; void* _temp2621; struct Cyc_Absyn_Exp* _temp2623; _LL2613: _LL2622:
_temp2621= _temp2611.f1; if(( unsigned int) _temp2621 >  1u?*(( int*) _temp2621)
==  Cyc_Absyn_Upper_b: 0){ _LL2624: _temp2623=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2621)->f1; goto _LL2618;} else{ goto _LL2615;} _LL2618: _temp2617=
_temp2611.f2; if(( unsigned int) _temp2617 >  1u?*(( int*) _temp2617) ==  Cyc_Absyn_Upper_b:
0){ _LL2620: _temp2619=(( struct Cyc_Absyn_Upper_b_struct*) _temp2617)->f1; goto
_LL2614;} else{ goto _LL2615;} _LL2615: goto _LL2616; _LL2614: if( Cyc_Evexp_eval_const_uint_exp(
_temp2623) >=  Cyc_Evexp_eval_const_uint_exp( _temp2619)){ bounds_ok= 1;} goto
_LL2612; _LL2616: bounds_ok= 1; goto _LL2612; _LL2612:;} return( bounds_ok?
_temp2609: 0)? Cyc_Tcutil_unify( _temp2576, _temp2604)? 1: Cyc_Tcenv_region_outlives(
te, _temp2576, _temp2604): 0;} _LL2593: return 1; _LL2595: goto _LL2589; _LL2589:;}
return 0; _LL2555:{ void* _temp2627= t2; struct Cyc_Absyn_Exp* _temp2633; struct
Cyc_Absyn_Tqual _temp2635; void* _temp2637; _LL2629: if(( unsigned int)
_temp2627 >  4u?*(( int*) _temp2627) ==  Cyc_Absyn_ArrayType: 0){ _LL2638:
_temp2637=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2627)->f1; goto
_LL2636; _LL2636: _temp2635=(( struct Cyc_Absyn_ArrayType_struct*) _temp2627)->f2;
goto _LL2634; _LL2634: _temp2633=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2627)->f3; goto _LL2630;} else{ goto _LL2631;} _LL2631: goto _LL2632;
_LL2630: { int okay; okay=( _temp2580 !=  0? _temp2633 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2580)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2633)): 0; return( okay? Cyc_Tcutil_unify(
_temp2584, _temp2637): 0)? ! _temp2582.q_const? 1: _temp2635.q_const: 0;}
_LL2632: return 0; _LL2628:;} return 0; _LL2557:{ void* _temp2639= t2; struct
Cyc_Absyn_Enumdecl* _temp2645; _LL2641: if(( unsigned int) _temp2639 >  4u?*((
int*) _temp2639) ==  Cyc_Absyn_EnumType: 0){ _LL2646: _temp2645=(( struct Cyc_Absyn_EnumType_struct*)
_temp2639)->f2; goto _LL2642;} else{ goto _LL2643;} _LL2643: goto _LL2644;
_LL2642: if(( _temp2586->fields !=  0? _temp2645->fields !=  0: 0)?(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2586->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2645->fields))->v):
0){ return 1;} goto _LL2640; _LL2644: goto _LL2640; _LL2640:;} goto _LL2559;
_LL2559: goto _LL2561; _LL2561: goto _LL2563; _LL2563: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType; _LL2565: return t2 == ( void*) Cyc_Absyn_VoidType;
_LL2567: return 0; _LL2551:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2647=
Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2648=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2648[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2649; _temp2649.tag= Cyc_Absyn_Cast_e;
_temp2649.f1=( void*) t; _temp2649.f2= _temp2647; _temp2649;}); _temp2648;})));
e->topt=({ struct Cyc_Core_Opt* _temp2650=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2650->v=( void*) t; _temp2650;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2651= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2653: if(( unsigned int) _temp2651
>  4u?*(( int*) _temp2651) ==  Cyc_Absyn_EnumType: 0){ goto _LL2654;} else{ goto
_LL2655;} _LL2655: if(( unsigned int) _temp2651 >  4u?*(( int*) _temp2651) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2656;} else{ goto _LL2657;} _LL2657: if((
unsigned int) _temp2651 >  4u?*(( int*) _temp2651) ==  Cyc_Absyn_IntType: 0){
goto _LL2658;} else{ goto _LL2659;} _LL2659: if(( unsigned int) _temp2651 >  4u?*((
int*) _temp2651) ==  Cyc_Absyn_Evar: 0){ goto _LL2660;} else{ goto _LL2661;}
_LL2661: goto _LL2662; _LL2654: goto _LL2656; _LL2656: goto _LL2658; _LL2658:
return 1; _LL2660: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2662: return 0; _LL2652:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2663= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2665: if( _temp2663 == ( void*) Cyc_Absyn_FloatType){
goto _LL2666;} else{ goto _LL2667;} _LL2667: if( _temp2663 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2668;} else{ goto _LL2669;} _LL2669: goto _LL2670; _LL2666: goto _LL2668;
_LL2668: return 1; _LL2670: return 0; _LL2664:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp2671= Cyc_Tcutil_compress( t); _LL2673: if(( unsigned int)
_temp2671 >  4u?*(( int*) _temp2671) ==  Cyc_Absyn_FnType: 0){ goto _LL2674;}
else{ goto _LL2675;} _LL2675: goto _LL2676; _LL2674: return 1; _LL2676: return 0;
_LL2672:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp2678=({ struct _tuple8 _temp2677; _temp2677.f1= t1; _temp2677.f2=
t2; _temp2677;}); void* _temp2702; void* _temp2704; void* _temp2706; void*
_temp2708; void* _temp2710; void* _temp2712; void* _temp2714; void* _temp2716;
void* _temp2718; void* _temp2720; void* _temp2722; void* _temp2724; void*
_temp2726; void* _temp2728; void* _temp2730; void* _temp2732; void* _temp2734;
void* _temp2736; void* _temp2738; void* _temp2740; void* _temp2742; void*
_temp2744; _LL2680: _LL2703: _temp2702= _temp2678.f1; if( _temp2702 == ( void*)
Cyc_Absyn_DoubleType){ goto _LL2681;} else{ goto _LL2682;} _LL2682: _LL2705:
_temp2704= _temp2678.f2; if( _temp2704 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2683;} else{ goto _LL2684;} _LL2684: _LL2707: _temp2706= _temp2678.f1; if(
_temp2706 == ( void*) Cyc_Absyn_FloatType){ goto _LL2685;} else{ goto _LL2686;}
_LL2686: _LL2709: _temp2708= _temp2678.f2; if( _temp2708 == ( void*) Cyc_Absyn_FloatType){
goto _LL2687;} else{ goto _LL2688;} _LL2688: _LL2711: _temp2710= _temp2678.f1;
if(( unsigned int) _temp2710 >  4u?*(( int*) _temp2710) ==  Cyc_Absyn_IntType: 0){
_LL2715: _temp2714=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2710)->f1;
if( _temp2714 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2713;} else{ goto _LL2690;}
_LL2713: _temp2712=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2710)->f2;
if( _temp2712 == ( void*) Cyc_Absyn_B8){ goto _LL2689;} else{ goto _LL2690;}}
else{ goto _LL2690;} _LL2690: _LL2717: _temp2716= _temp2678.f2; if((
unsigned int) _temp2716 >  4u?*(( int*) _temp2716) ==  Cyc_Absyn_IntType: 0){
_LL2721: _temp2720=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2716)->f1;
if( _temp2720 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2719;} else{ goto _LL2692;}
_LL2719: _temp2718=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2716)->f2;
if( _temp2718 == ( void*) Cyc_Absyn_B8){ goto _LL2691;} else{ goto _LL2692;}}
else{ goto _LL2692;} _LL2692: _LL2723: _temp2722= _temp2678.f1; if((
unsigned int) _temp2722 >  4u?*(( int*) _temp2722) ==  Cyc_Absyn_IntType: 0){
_LL2727: _temp2726=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2722)->f1;
if( _temp2726 == ( void*) Cyc_Absyn_Signed){ goto _LL2725;} else{ goto _LL2694;}
_LL2725: _temp2724=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2722)->f2;
if( _temp2724 == ( void*) Cyc_Absyn_B8){ goto _LL2693;} else{ goto _LL2694;}}
else{ goto _LL2694;} _LL2694: _LL2729: _temp2728= _temp2678.f2; if((
unsigned int) _temp2728 >  4u?*(( int*) _temp2728) ==  Cyc_Absyn_IntType: 0){
_LL2733: _temp2732=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2728)->f1;
if( _temp2732 == ( void*) Cyc_Absyn_Signed){ goto _LL2731;} else{ goto _LL2696;}
_LL2731: _temp2730=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2728)->f2;
if( _temp2730 == ( void*) Cyc_Absyn_B8){ goto _LL2695;} else{ goto _LL2696;}}
else{ goto _LL2696;} _LL2696: _LL2735: _temp2734= _temp2678.f1; if((
unsigned int) _temp2734 >  4u?*(( int*) _temp2734) ==  Cyc_Absyn_IntType: 0){
_LL2739: _temp2738=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2734)->f1;
if( _temp2738 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2737;} else{ goto _LL2698;}
_LL2737: _temp2736=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2734)->f2;
if( _temp2736 == ( void*) Cyc_Absyn_B4){ goto _LL2697;} else{ goto _LL2698;}}
else{ goto _LL2698;} _LL2698: _LL2741: _temp2740= _temp2678.f2; if((
unsigned int) _temp2740 >  4u?*(( int*) _temp2740) ==  Cyc_Absyn_IntType: 0){
_LL2745: _temp2744=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2740)->f1;
if( _temp2744 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2743;} else{ goto _LL2700;}
_LL2743: _temp2742=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2740)->f2;
if( _temp2742 == ( void*) Cyc_Absyn_B4){ goto _LL2699;} else{ goto _LL2700;}}
else{ goto _LL2700;} _LL2700: goto _LL2701; _LL2681: goto _LL2683; _LL2683:
return( void*) Cyc_Absyn_DoubleType; _LL2685: goto _LL2687; _LL2687: return(
void*) Cyc_Absyn_FloatType; _LL2689: goto _LL2691; _LL2691: return Cyc_Absyn_ulonglong_t;
_LL2693: goto _LL2695; _LL2695: return Cyc_Absyn_slonglong_t; _LL2697: goto
_LL2699; _LL2699: return Cyc_Absyn_uint_t; _LL2701: return Cyc_Absyn_sint_t;
_LL2679:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2746=( void*) e->r; struct Cyc_Core_Opt* _temp2752; _LL2748: if(*(( int*)
_temp2746) ==  Cyc_Absyn_AssignOp_e){ _LL2753: _temp2752=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2746)->f2; if( _temp2752 ==  0){ goto _LL2749;} else{ goto _LL2750;}} else{
goto _LL2750;} _LL2750: goto _LL2751; _LL2749:({ void* _temp2754[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2754, sizeof( void*), 0u));}); goto _LL2747; _LL2751: goto _LL2747; _LL2747:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2755=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2755[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2756; _temp2756.tag=
Cyc_Absyn_Forward_constr; _temp2756.f1= c2; _temp2756;}); _temp2755;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2757=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2757[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2758; _temp2758.tag= Cyc_Absyn_Forward_constr;
_temp2758.f1= c1; _temp2758;}); _temp2757;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2759=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2759[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2760; _temp2760.tag= Cyc_Absyn_Forward_constr; _temp2760.f1= c1; _temp2760;});
_temp2759;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2761=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2761[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2762; _temp2762.tag= Cyc_Absyn_Forward_constr;
_temp2762.f1= c2; _temp2762;}); _temp2761;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2763=( int*) _cycalloc_atomic( sizeof( int)); _temp2763[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2763;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2767;
_temp2767.tag= Cyc_Std_Int_pa; _temp2767.f1=( int)(( unsigned int) i);{ void*
_temp2766[ 1u]={& _temp2767}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2766, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2764=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2764->name=({ struct _tagged_arr* _temp2765=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2765[ 0]=
s; _temp2765;}); _temp2764->identity= 0; _temp2764->kind= Cyc_Absyn_new_conref(
k); _temp2764;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2768=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2768, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2770; _temp2770.tag= Cyc_Std_String_pa; _temp2770.f1=( struct _tagged_arr)*
t->name;{ void* _temp2769[ 1u]={& _temp2770}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2769, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2771= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2771, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2772=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2772[ 0]=( struct _tagged_arr) _temp2771; _temp2772;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2773=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2773->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2774=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2774->v=( void*)(*
x).f1; _temp2774;}); _temp2773->f2=(* x).f2; _temp2773->f3=(* x).f3; _temp2773;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2775= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts= atts->tl){ if( Cyc_Absyn_fntype_att(( void*) atts->hd)){
_temp2775=({ struct Cyc_List_List* _temp2776=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2776->hd=( void*)(( void*) atts->hd);
_temp2776->tl= _temp2775; _temp2776;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2777=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2777[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2778; _temp2778.tag= Cyc_Absyn_FnType;
_temp2778.f1=({ struct Cyc_Absyn_FnInfo _temp2779; _temp2779.tvars= fd->tvs;
_temp2779.effect= fd->effect; _temp2779.ret_typ=( void*)(( void*) fd->ret_type);
_temp2779.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2779.c_varargs= fd->c_varargs; _temp2779.cyc_varargs= fd->cyc_varargs;
_temp2779.rgn_po= fd->rgn_po; _temp2779.attributes= _temp2775; _temp2779;});
_temp2778;}); _temp2777;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp2780=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp2780->f1=(* pr).f1; _temp2780->f2= t; _temp2780;});} struct _tuple15{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct
_tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2781=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2781->f1=({ struct _tuple15* _temp2782=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2782->f1=(* y).f1; _temp2782->f2=(* y).f2;
_temp2782;}); _temp2781->f2=(* y).f3; _temp2781;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2785; struct _tuple15* _temp2787; struct
_tuple16 _temp2783=* w; _LL2788: _temp2787= _temp2783.f1; goto _LL2786; _LL2786:
_temp2785= _temp2783.f2; goto _LL2784; _LL2784: { struct Cyc_Absyn_Tqual
_temp2791; struct Cyc_Core_Opt* _temp2793; struct _tuple15 _temp2789=* _temp2787;
_LL2794: _temp2793= _temp2789.f1; goto _LL2792; _LL2792: _temp2791= _temp2789.f2;
goto _LL2790; _LL2790: return({ struct _tuple2* _temp2795=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2795->f1= _temp2793; _temp2795->f2=
_temp2791; _temp2795->f3= _temp2785; _temp2795;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2796=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2796->name= f->name;
_temp2796->tq= f->tq; _temp2796->type=( void*) t; _temp2796->width= f->width;
_temp2796->attributes= f->attributes; _temp2796;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2797= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2847; struct Cyc_Absyn_Structdecl** _temp2849;
struct Cyc_List_List* _temp2851; struct _tuple1* _temp2853; struct Cyc_Absyn_Uniondecl**
_temp2855; struct Cyc_List_List* _temp2857; struct _tuple1* _temp2859; struct
Cyc_Absyn_TunionInfo _temp2861; void* _temp2863; struct Cyc_List_List* _temp2865;
void* _temp2867; struct Cyc_Absyn_TunionFieldInfo _temp2869; struct Cyc_List_List*
_temp2871; void* _temp2873; struct Cyc_Core_Opt* _temp2875; struct Cyc_List_List*
_temp2877; struct _tuple1* _temp2879; struct Cyc_Absyn_Exp* _temp2881; struct
Cyc_Absyn_Tqual _temp2883; void* _temp2885; struct Cyc_Absyn_PtrInfo _temp2887;
struct Cyc_Absyn_Conref* _temp2889; struct Cyc_Absyn_Tqual _temp2891; struct Cyc_Absyn_Conref*
_temp2893; void* _temp2895; void* _temp2897; struct Cyc_Absyn_FnInfo _temp2899;
struct Cyc_List_List* _temp2901; struct Cyc_List_List* _temp2903; struct Cyc_Absyn_VarargInfo*
_temp2905; int _temp2907; struct Cyc_List_List* _temp2909; void* _temp2911;
struct Cyc_Core_Opt* _temp2913; struct Cyc_List_List* _temp2915; struct Cyc_List_List*
_temp2917; struct Cyc_List_List* _temp2919; struct Cyc_List_List* _temp2921;
struct Cyc_Core_Opt* _temp2923; void* _temp2925; void* _temp2927; void*
_temp2929; struct Cyc_List_List* _temp2931; _LL2799: if(( unsigned int)
_temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_VarType: 0){ _LL2848:
_temp2847=(( struct Cyc_Absyn_VarType_struct*) _temp2797)->f1; goto _LL2800;}
else{ goto _LL2801;} _LL2801: if(( unsigned int) _temp2797 >  4u?*(( int*)
_temp2797) ==  Cyc_Absyn_StructType: 0){ _LL2854: _temp2853=(( struct Cyc_Absyn_StructType_struct*)
_temp2797)->f1; goto _LL2852; _LL2852: _temp2851=(( struct Cyc_Absyn_StructType_struct*)
_temp2797)->f2; goto _LL2850; _LL2850: _temp2849=(( struct Cyc_Absyn_StructType_struct*)
_temp2797)->f3; goto _LL2802;} else{ goto _LL2803;} _LL2803: if(( unsigned int)
_temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_UnionType: 0){ _LL2860:
_temp2859=(( struct Cyc_Absyn_UnionType_struct*) _temp2797)->f1; goto _LL2858;
_LL2858: _temp2857=(( struct Cyc_Absyn_UnionType_struct*) _temp2797)->f2; goto
_LL2856; _LL2856: _temp2855=(( struct Cyc_Absyn_UnionType_struct*) _temp2797)->f3;
goto _LL2804;} else{ goto _LL2805;} _LL2805: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_TunionType: 0){ _LL2862: _temp2861=(( struct Cyc_Absyn_TunionType_struct*)
_temp2797)->f1; _LL2868: _temp2867=( void*) _temp2861.tunion_info; goto _LL2866;
_LL2866: _temp2865= _temp2861.targs; goto _LL2864; _LL2864: _temp2863=( void*)
_temp2861.rgn; goto _LL2806;} else{ goto _LL2807;} _LL2807: if(( unsigned int)
_temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2870:
_temp2869=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2797)->f1; _LL2874:
_temp2873=( void*) _temp2869.field_info; goto _LL2872; _LL2872: _temp2871=
_temp2869.targs; goto _LL2808;} else{ goto _LL2809;} _LL2809: if(( unsigned int)
_temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_TypedefType: 0){ _LL2880:
_temp2879=(( struct Cyc_Absyn_TypedefType_struct*) _temp2797)->f1; goto _LL2878;
_LL2878: _temp2877=(( struct Cyc_Absyn_TypedefType_struct*) _temp2797)->f2; goto
_LL2876; _LL2876: _temp2875=(( struct Cyc_Absyn_TypedefType_struct*) _temp2797)->f3;
goto _LL2810;} else{ goto _LL2811;} _LL2811: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_ArrayType: 0){ _LL2886: _temp2885=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2797)->f1; goto _LL2884; _LL2884:
_temp2883=(( struct Cyc_Absyn_ArrayType_struct*) _temp2797)->f2; goto _LL2882;
_LL2882: _temp2881=(( struct Cyc_Absyn_ArrayType_struct*) _temp2797)->f3; goto
_LL2812;} else{ goto _LL2813;} _LL2813: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_PointerType: 0){ _LL2888: _temp2887=(( struct Cyc_Absyn_PointerType_struct*)
_temp2797)->f1; _LL2898: _temp2897=( void*) _temp2887.elt_typ; goto _LL2896;
_LL2896: _temp2895=( void*) _temp2887.rgn_typ; goto _LL2894; _LL2894: _temp2893=
_temp2887.nullable; goto _LL2892; _LL2892: _temp2891= _temp2887.tq; goto _LL2890;
_LL2890: _temp2889= _temp2887.bounds; goto _LL2814;} else{ goto _LL2815;}
_LL2815: if(( unsigned int) _temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_FnType:
0){ _LL2900: _temp2899=(( struct Cyc_Absyn_FnType_struct*) _temp2797)->f1;
_LL2916: _temp2915= _temp2899.tvars; goto _LL2914; _LL2914: _temp2913= _temp2899.effect;
goto _LL2912; _LL2912: _temp2911=( void*) _temp2899.ret_typ; goto _LL2910;
_LL2910: _temp2909= _temp2899.args; goto _LL2908; _LL2908: _temp2907= _temp2899.c_varargs;
goto _LL2906; _LL2906: _temp2905= _temp2899.cyc_varargs; goto _LL2904; _LL2904:
_temp2903= _temp2899.rgn_po; goto _LL2902; _LL2902: _temp2901= _temp2899.attributes;
goto _LL2816;} else{ goto _LL2817;} _LL2817: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_TupleType: 0){ _LL2918: _temp2917=(( struct Cyc_Absyn_TupleType_struct*)
_temp2797)->f1; goto _LL2818;} else{ goto _LL2819;} _LL2819: if(( unsigned int)
_temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_AnonStructType: 0){ _LL2920:
_temp2919=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2797)->f1; goto
_LL2820;} else{ goto _LL2821;} _LL2821: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2922: _temp2921=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2797)->f1; goto _LL2822;} else{ goto
_LL2823;} _LL2823: if(( unsigned int) _temp2797 >  4u?*(( int*) _temp2797) == 
Cyc_Absyn_Evar: 0){ _LL2924: _temp2923=(( struct Cyc_Absyn_Evar_struct*)
_temp2797)->f2; goto _LL2824;} else{ goto _LL2825;} _LL2825: if(( unsigned int)
_temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2926:
_temp2925=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2797)->f1;
goto _LL2826;} else{ goto _LL2827;} _LL2827: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_EnumType: 0){ goto _LL2828;} else{ goto _LL2829;}
_LL2829: if(( unsigned int) _temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL2830;} else{ goto _LL2831;} _LL2831: if( _temp2797 == ( void*) Cyc_Absyn_VoidType){
goto _LL2832;} else{ goto _LL2833;} _LL2833: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_IntType: 0){ goto _LL2834;} else{ goto _LL2835;}
_LL2835: if( _temp2797 == ( void*) Cyc_Absyn_FloatType){ goto _LL2836;} else{
goto _LL2837;} _LL2837: if( _temp2797 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2838;} else{ goto _LL2839;} _LL2839: if( _temp2797 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL2840;} else{ goto _LL2841;} _LL2841: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_RgnsEff: 0){ _LL2928: _temp2927=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2797)->f1; goto _LL2842;} else{ goto _LL2843;}
_LL2843: if(( unsigned int) _temp2797 >  4u?*(( int*) _temp2797) ==  Cyc_Absyn_AccessEff:
0){ _LL2930: _temp2929=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2797)->f1;
goto _LL2844;} else{ goto _LL2845;} _LL2845: if(( unsigned int) _temp2797 >  4u?*((
int*) _temp2797) ==  Cyc_Absyn_JoinEff: 0){ _LL2932: _temp2931=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2797)->f1; goto _LL2846;} else{ goto _LL2798;} _LL2800: { struct
_handler_cons _temp2933; _push_handler(& _temp2933);{ int _temp2935= 0; if(
setjmp( _temp2933.handler)){ _temp2935= 1;} if( ! _temp2935){{ void* _temp2936=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2847); _npop_handler( 0u); return _temp2936;}; _pop_handler();} else{
void* _temp2934=( void*) _exn_thrown; void* _temp2938= _temp2934; _LL2940: if(
_temp2938 ==  Cyc_Core_Not_found){ goto _LL2941;} else{ goto _LL2942;} _LL2942:
goto _LL2943; _LL2941: return t; _LL2943:( void) _throw( _temp2938); _LL2939:;}}}
_LL2802: { struct Cyc_List_List* _temp2944= Cyc_Tcutil_substs( rgn, inst,
_temp2851); return _temp2944 ==  _temp2851? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2945=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2945[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2946; _temp2946.tag=
Cyc_Absyn_StructType; _temp2946.f1= _temp2853; _temp2946.f2= _temp2944;
_temp2946.f3= _temp2849; _temp2946;}); _temp2945;});} _LL2804: { struct Cyc_List_List*
_temp2947= Cyc_Tcutil_substs( rgn, inst, _temp2857); return _temp2947 == 
_temp2857? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2948=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2948[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2949; _temp2949.tag= Cyc_Absyn_UnionType;
_temp2949.f1= _temp2859; _temp2949.f2= _temp2947; _temp2949.f3= _temp2855;
_temp2949;}); _temp2948;});} _LL2806: { struct Cyc_List_List* _temp2950= Cyc_Tcutil_substs(
rgn, inst, _temp2865); void* _temp2951= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2863); return( _temp2950 ==  _temp2865? _temp2951 ==  _temp2863: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2952=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2952[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2953; _temp2953.tag= Cyc_Absyn_TunionType;
_temp2953.f1=({ struct Cyc_Absyn_TunionInfo _temp2954; _temp2954.tunion_info=(
void*) _temp2867; _temp2954.targs= _temp2950; _temp2954.rgn=( void*) _temp2951;
_temp2954;}); _temp2953;}); _temp2952;});} _LL2808: { struct Cyc_List_List*
_temp2955= Cyc_Tcutil_substs( rgn, inst, _temp2871); return _temp2955 == 
_temp2871? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2956=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2956[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2957; _temp2957.tag=
Cyc_Absyn_TunionFieldType; _temp2957.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2958; _temp2958.field_info=( void*) _temp2873; _temp2958.targs= _temp2955;
_temp2958;}); _temp2957;}); _temp2956;});} _LL2810: { struct Cyc_List_List*
_temp2959= Cyc_Tcutil_substs( rgn, inst, _temp2877); return _temp2959 == 
_temp2877? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2960=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2960[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2961; _temp2961.tag=
Cyc_Absyn_TypedefType; _temp2961.f1= _temp2879; _temp2961.f2= _temp2959;
_temp2961.f3= _temp2875; _temp2961;}); _temp2960;});} _LL2812: { void* _temp2962=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2885); return _temp2962 ==  _temp2885? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2963=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2963[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2964; _temp2964.tag= Cyc_Absyn_ArrayType;
_temp2964.f1=( void*) _temp2962; _temp2964.f2= _temp2883; _temp2964.f3=
_temp2881; _temp2964;}); _temp2963;});} _LL2814: { void* _temp2965= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2897); void* _temp2966= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2895); if( _temp2965 ==  _temp2897? _temp2966 ==  _temp2895: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2967=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2967[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2968; _temp2968.tag= Cyc_Absyn_PointerType;
_temp2968.f1=({ struct Cyc_Absyn_PtrInfo _temp2969; _temp2969.elt_typ=( void*)
_temp2965; _temp2969.rgn_typ=( void*) _temp2966; _temp2969.nullable= _temp2893;
_temp2969.tq= _temp2891; _temp2969.bounds= _temp2889; _temp2969;}); _temp2968;});
_temp2967;});} _LL2816:{ struct Cyc_List_List* _temp2970= _temp2915; for( 0;
_temp2970 !=  0; _temp2970= _temp2970->tl){ inst=({ struct Cyc_List_List*
_temp2971=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2971->hd=( void*)({ struct _tuple5* _temp2972=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2972->f1=( struct Cyc_Absyn_Tvar*)
_temp2970->hd; _temp2972->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2973=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2973[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2974; _temp2974.tag= Cyc_Absyn_VarType;
_temp2974.f1=( struct Cyc_Absyn_Tvar*) _temp2970->hd; _temp2974;}); _temp2973;});
_temp2972;}); _temp2971->tl= inst; _temp2971;});}}{ struct Cyc_List_List*
_temp2977; struct Cyc_List_List* _temp2979; struct _tuple0 _temp2975=(( struct
_tuple0(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp2909)); _LL2980: _temp2979= _temp2975.f1; goto _LL2978; _LL2978:
_temp2977= _temp2975.f2; goto _LL2976; _LL2976: { struct Cyc_List_List*
_temp2981= Cyc_Tcutil_substs( rgn, inst, _temp2977); struct Cyc_List_List*
_temp2982=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple16*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp2979, _temp2981));
struct Cyc_Core_Opt* eff2; if( _temp2913 ==  0){ eff2= 0;} else{ void* _temp2983=
Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp2913->v); if( _temp2983 == (
void*) _temp2913->v){ eff2= _temp2913;} else{ eff2=({ struct Cyc_Core_Opt*
_temp2984=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2984->v=( void*) _temp2983; _temp2984;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2905 ==  0){ cyc_varargs2= 0;} else{ int _temp2987; void*
_temp2989; struct Cyc_Absyn_Tqual _temp2991; struct Cyc_Core_Opt* _temp2993;
struct Cyc_Absyn_VarargInfo _temp2985=* _temp2905; _LL2994: _temp2993= _temp2985.name;
goto _LL2992; _LL2992: _temp2991= _temp2985.tq; goto _LL2990; _LL2990: _temp2989=(
void*) _temp2985.type; goto _LL2988; _LL2988: _temp2987= _temp2985.inject; goto
_LL2986; _LL2986: { void* _temp2995= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2989); if( _temp2995 ==  _temp2989){ cyc_varargs2= _temp2905;} else{
cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp2996=( struct Cyc_Absyn_VarargInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2996->name= _temp2993;
_temp2996->tq= _temp2991; _temp2996->type=( void*) _temp2995; _temp2996->inject=
_temp2987; _temp2996;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List*
_temp2999; struct Cyc_List_List* _temp3001; struct _tuple0 _temp2997= Cyc_List_rsplit(
rgn, rgn, _temp2903); _LL3002: _temp3001= _temp2997.f1; goto _LL3000; _LL3000:
_temp2999= _temp2997.f2; goto _LL2998; _LL2998: { struct Cyc_List_List*
_temp3003= Cyc_Tcutil_substs( rgn, inst, _temp3001); struct Cyc_List_List*
_temp3004= Cyc_Tcutil_substs( rgn, inst, _temp2999); if( _temp3003 ==  _temp3001?
_temp3004 ==  _temp2999: 0){ rgn_po2= _temp2903;} else{ rgn_po2= Cyc_List_zip(
_temp3003, _temp3004);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp3005=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp3005[ 0]=({ struct Cyc_Absyn_FnType_struct _temp3006; _temp3006.tag= Cyc_Absyn_FnType;
_temp3006.f1=({ struct Cyc_Absyn_FnInfo _temp3007; _temp3007.tvars= _temp2915;
_temp3007.effect= eff2; _temp3007.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2911); _temp3007.args= _temp2982; _temp3007.c_varargs= _temp2907;
_temp3007.cyc_varargs= cyc_varargs2; _temp3007.rgn_po= rgn_po2; _temp3007.attributes=
_temp2901; _temp3007;}); _temp3006;}); _temp3005;});}}}}} _LL2818: { struct Cyc_List_List*
_temp3008=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2917); struct Cyc_List_List* _temp3009= Cyc_Tcutil_substs( rgn, inst,
_temp3008); if( _temp3009 ==  _temp3008){ return t;}{ struct Cyc_List_List*
_temp3010=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2917, _temp3009); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3011=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3011[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3012; _temp3012.tag= Cyc_Absyn_TupleType;
_temp3012.f1= _temp3010; _temp3012;}); _temp3011;});}} _LL2820: { struct Cyc_List_List*
_temp3013=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2919); struct Cyc_List_List* _temp3014= Cyc_Tcutil_substs( rgn, inst,
_temp3013); if( _temp3014 ==  _temp3013){ return t;}{ struct Cyc_List_List*
_temp3015=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2919, _temp3014); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3016=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3016[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3017; _temp3017.tag= Cyc_Absyn_AnonStructType;
_temp3017.f1= _temp3015; _temp3017;}); _temp3016;});}} _LL2822: { struct Cyc_List_List*
_temp3018=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2921); struct Cyc_List_List* _temp3019= Cyc_Tcutil_substs( rgn, inst,
_temp3018); if( _temp3019 ==  _temp3018){ return t;}{ struct Cyc_List_List*
_temp3020=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2921, _temp3019); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3021=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3021[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3022; _temp3022.tag= Cyc_Absyn_AnonStructType;
_temp3022.f1= _temp3020; _temp3022;}); _temp3021;});}} _LL2824: if( _temp2923 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp2923->v);} else{
return t;} _LL2826: { void* _temp3023= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2925); return _temp3023 ==  _temp2925? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3024=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3024[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3025; _temp3025.tag= Cyc_Absyn_RgnHandleType; _temp3025.f1=( void*)
_temp3023; _temp3025;}); _temp3024;});} _LL2828: return t; _LL2830: return t;
_LL2832: return t; _LL2834: return t; _LL2836: return t; _LL2838: return t;
_LL2840: return t; _LL2842: { void* _temp3026= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2927); return _temp3026 ==  _temp2927? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp3027=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3027[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3028; _temp3028.tag= Cyc_Absyn_RgnsEff;
_temp3028.f1=( void*) _temp3026; _temp3028;}); _temp3027;});} _LL2844: { void*
_temp3029= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2929); return _temp3029 == 
_temp2929? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3030=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3030[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3031; _temp3031.tag= Cyc_Absyn_AccessEff;
_temp3031.f1=( void*) _temp3029; _temp3031;}); _temp3030;});} _LL2846: { struct
Cyc_List_List* _temp3032= Cyc_Tcutil_substs( rgn, inst, _temp2931); return
_temp3032 ==  _temp2931? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3033=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3033[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3034; _temp3034.tag= Cyc_Absyn_JoinEff;
_temp3034.f1= _temp3032; _temp3034;}); _temp3033;});} _LL2798:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp3035=(
void*) ts->hd; struct Cyc_List_List* _temp3036= ts->tl; void* _temp3037= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3035); struct Cyc_List_List* _temp3038= Cyc_Tcutil_substs( rgn,
inst, _temp3036); if( _temp3035 ==  _temp3037? _temp3036 ==  _temp3038: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp3039=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3039->hd=(
void*) _temp3037; _temp3039->tl= _temp3038; _temp3039;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp3084=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3084[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3085; _temp3085.tag= Cyc_Absyn_Const_e; _temp3085.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp3085;}); _temp3084;}), loc);{ void*
_temp3040= Cyc_Tcutil_compress( t); void* _temp3054; void* _temp3056; void*
_temp3058; void* _temp3060; _LL3042: if(( unsigned int) _temp3040 >  4u?*(( int*)
_temp3040) ==  Cyc_Absyn_PointerType: 0){ goto _LL3043;} else{ goto _LL3044;}
_LL3044: if(( unsigned int) _temp3040 >  4u?*(( int*) _temp3040) ==  Cyc_Absyn_IntType:
0){ _LL3057: _temp3056=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3040)->f1;
goto _LL3055; _LL3055: _temp3054=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3040)->f2; if( _temp3054 == ( void*) Cyc_Absyn_B1){ goto _LL3045;} else{
goto _LL3046;}} else{ goto _LL3046;} _LL3046: if(( unsigned int) _temp3040 >  4u?*((
int*) _temp3040) ==  Cyc_Absyn_IntType: 0){ _LL3061: _temp3060=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp3040)->f1; goto _LL3059; _LL3059: _temp3058=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3040)->f2; goto _LL3047;} else{
goto _LL3048;} _LL3048: if( _temp3040 == ( void*) Cyc_Absyn_FloatType){ goto
_LL3049;} else{ goto _LL3050;} _LL3050: if( _temp3040 == ( void*) Cyc_Absyn_DoubleType){
goto _LL3051;} else{ goto _LL3052;} _LL3052: goto _LL3053; _LL3043: goto _LL3041;
_LL3045:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp3062=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3062[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3063; _temp3063.tag= Cyc_Absyn_Const_e;
_temp3063.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp3064=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp3064[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp3065; _temp3065.tag= Cyc_Absyn_Char_c;
_temp3065.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp3065.f2='\000'; _temp3065;});
_temp3064;})); _temp3063;}); _temp3062;}))); goto _LL3041; _LL3047:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3066=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3066[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3067; _temp3067.tag= Cyc_Absyn_Const_e; _temp3067.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp3068=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp3068[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp3069; _temp3069.tag= Cyc_Absyn_Int_c; _temp3069.f1=( void*) _temp3060;
_temp3069.f2= 0; _temp3069;}); _temp3068;})); _temp3067;}); _temp3066;}))); if(
_temp3058 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp3070=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp3070[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp3071; _temp3071.tag= Cyc_Absyn_Cast_e;
_temp3071.f1=( void*) t; _temp3071.f2= e; _temp3071;}); _temp3070;}), loc);}
goto _LL3041; _LL3049:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp3072=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3072[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3073; _temp3073.tag= Cyc_Absyn_Const_e;
_temp3073.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3074=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp3074[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp3075; _temp3075.tag= Cyc_Absyn_Float_c;
_temp3075.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp3075;});
_temp3074;})); _temp3073;}); _temp3072;}))); goto _LL3041; _LL3051:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp3076=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp3076[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp3077; _temp3077.tag= Cyc_Absyn_Cast_e; _temp3077.f1=( void*) t; _temp3077.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3078=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3078[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3079; _temp3079.tag= Cyc_Absyn_Const_e;
_temp3079.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3080=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp3080[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp3081; _temp3081.tag= Cyc_Absyn_Float_c;
_temp3081.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp3081;});
_temp3080;})); _temp3079;}); _temp3078;}), loc); _temp3077;}); _temp3076;})));
goto _LL3041; _LL3053:({ struct Cyc_Std_String_pa_struct _temp3083; _temp3083.tag=
Cyc_Std_String_pa; _temp3083.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp3082[ 1u]={& _temp3083}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp3082, sizeof( void*), 1u));}});
goto _LL3041; _LL3041:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp3086=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp3086->f1= tv; _temp3086->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3087=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3087->v=( void*) k; _temp3087;}),({ struct Cyc_Core_Opt*
_temp3088=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3088->v=( void*) s; _temp3088;})); _temp3086;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp3091;
struct _RegionHandle* _temp3092; struct Cyc_List_List* _temp3094; struct _tuple6*
_temp3089= env; _temp3091=* _temp3089; _LL3095: _temp3094= _temp3091.f1; goto
_LL3093; _LL3093: _temp3092= _temp3091.f2; goto _LL3090; _LL3090: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp3096=( struct _tuple5*) _region_malloc(
_temp3092, sizeof( struct _tuple5)); _temp3096->f1= tv; _temp3096->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3097=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3097->v=( void*) k; _temp3097;}),({ struct Cyc_Core_Opt*
_temp3098=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3098->v=( void*) _temp3094; _temp3098;})); _temp3096;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if( Cyc_Std_zstrptrcmp((( struct
Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind; struct Cyc_Absyn_Conref* k2= tv->kind;
if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({ struct Cyc_Std_String_pa_struct
_temp3102; _temp3102.tag= Cyc_Std_String_pa; _temp3102.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct _temp3101;
_temp3101.tag= Cyc_Std_String_pa; _temp3101.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
k1);{ struct Cyc_Std_String_pa_struct _temp3100; _temp3100.tag= Cyc_Std_String_pa;
_temp3100.f1=( struct _tagged_arr)* tv->name;{ void* _temp3099[ 3u]={& _temp3100,&
_temp3101,& _temp3102}; Cyc_Tcutil_terr( loc, _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u), _tag_arr( _temp3099, sizeof( void*), 3u));}}}});}
if( tv->identity ==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) != *(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity))){({ void* _temp3103[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp3103, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp3104=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3104->hd=( void*) tv; _temp3104->tl= tvs; _temp3104;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp3105[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp3105, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity ==  0){({ void* _temp3106[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u),
_tag_arr( _temp3106, sizeof( void*), 0u));});} if(*(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp3107=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3107->hd=( void*) tv; _temp3107->tl=
tvs; _temp3107;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp3109; _temp3109.tag= Cyc_Std_String_pa;
_temp3109.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp3108[
1u]={& _temp3109};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp3108, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp3110=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3110->hd=(
void*) tv; _temp3110->tl= tvs; _temp3110;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp3111= Cyc_Tcutil_compress( e);
int _temp3117; _LL3113: if(( unsigned int) _temp3111 >  4u?*(( int*) _temp3111)
==  Cyc_Absyn_Evar: 0){ _LL3118: _temp3117=(( struct Cyc_Absyn_Evar_struct*)
_temp3111)->f3; goto _LL3114;} else{ goto _LL3115;} _LL3115: goto _LL3116;
_LL3114:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2= es2->tl){ void*
_temp3119= Cyc_Tcutil_compress(( void*) es2->hd); int _temp3125; _LL3121: if((
unsigned int) _temp3119 >  4u?*(( int*) _temp3119) ==  Cyc_Absyn_Evar: 0){
_LL3126: _temp3125=(( struct Cyc_Absyn_Evar_struct*) _temp3119)->f3; goto
_LL3122;} else{ goto _LL3123;} _LL3123: goto _LL3124; _LL3122: if( _temp3117 == 
_temp3125){ return es;} goto _LL3120; _LL3124: goto _LL3120; _LL3120:;}} return({
struct Cyc_List_List* _temp3127=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3127->hd=( void*) e; _temp3127->tl= es; _temp3127;});
_LL3116: return es; _LL3112:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs= tvs->tl){ int present= 0;{ struct Cyc_List_List* b=
btvs; for( 0; b !=  0; b= b->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)
tvs->hd)->identity)) == *(( int*) _check_null((( struct Cyc_Absyn_Tvar*) b->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp3128=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3128->hd=(
void*)(( struct Cyc_Absyn_Tvar*) tvs->hd); _temp3128->tl= r; _temp3128;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp3130; _temp3130.tag= Cyc_Std_String_pa; _temp3130.f1=( struct _tagged_arr)*
fn;{ void* _temp3129[ 1u]={& _temp3130}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp3129, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp3131= Cyc_Tcutil_compress( field_typ); void* _temp3137;
_LL3133: if(( unsigned int) _temp3131 >  4u?*(( int*) _temp3131) ==  Cyc_Absyn_IntType:
0){ _LL3138: _temp3137=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3131)->f2;
goto _LL3134;} else{ goto _LL3135;} _LL3135: goto _LL3136; _LL3134:{ void*
_temp3139= _temp3137; _LL3141: if( _temp3139 == ( void*) Cyc_Absyn_B1){ goto
_LL3142;} else{ goto _LL3143;} _LL3143: if( _temp3139 == ( void*) Cyc_Absyn_B2){
goto _LL3144;} else{ goto _LL3145;} _LL3145: if( _temp3139 == ( void*) Cyc_Absyn_B4){
goto _LL3146;} else{ goto _LL3147;} _LL3147: if( _temp3139 == ( void*) Cyc_Absyn_B8){
goto _LL3148;} else{ goto _LL3140;} _LL3142: if( w >  8){({ void* _temp3149[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp3149, sizeof( void*), 0u));});} goto
_LL3140; _LL3144: if( w >  16){({ void* _temp3150[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp3150, sizeof( void*), 0u));});} goto _LL3140; _LL3146: if( w >  32){({ void*
_temp3151[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp3151, sizeof( void*), 0u));});}
goto _LL3140; _LL3148: if( w >  64){({ void* _temp3152[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp3152, sizeof( void*), 0u));});} goto _LL3140; _LL3140:;} goto
_LL3132; _LL3136:({ struct Cyc_Std_String_pa_struct _temp3155; _temp3155.tag=
Cyc_Std_String_pa; _temp3155.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp3154; _temp3154.tag= Cyc_Std_String_pa;
_temp3154.f1=( struct _tagged_arr)* fn;{ void* _temp3153[ 2u]={& _temp3154,&
_temp3155}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3153, sizeof( void*), 2u));}}});
goto _LL3132; _LL3132:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts= atts->tl){ void* _temp3156=( void*) atts->hd; _LL3158: if( _temp3156 == (
void*) Cyc_Absyn_Packed_att){ goto _LL3159;} else{ goto _LL3160;} _LL3160: if((
unsigned int) _temp3156 >  16u?*(( int*) _temp3156) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL3161;} else{ goto _LL3162;} _LL3162: goto _LL3163; _LL3159: continue;
_LL3161: continue; _LL3163:({ struct Cyc_Std_String_pa_struct _temp3166;
_temp3166.tag= Cyc_Std_String_pa; _temp3166.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp3165; _temp3165.tag= Cyc_Std_String_pa;
_temp3165.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp3164[ 2u]={& _temp3165,& _temp3166}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp3164, sizeof( void*), 2u));}}});
goto _LL3157; _LL3157:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; struct _tuple17{ void* f1; int f2; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp3167= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3217; struct Cyc_Core_Opt** _temp3219; struct Cyc_Core_Opt* _temp3220;
struct Cyc_Core_Opt** _temp3222; struct Cyc_Absyn_Tvar* _temp3223; struct Cyc_List_List*
_temp3225; struct Cyc_Absyn_Enumdecl* _temp3227; struct Cyc_Absyn_Enumdecl**
_temp3229; struct _tuple1* _temp3230; struct Cyc_Absyn_TunionInfo _temp3232;
void* _temp3234; struct Cyc_List_List* _temp3236; void* _temp3238; void**
_temp3240; struct Cyc_Absyn_TunionFieldInfo _temp3241; struct Cyc_List_List*
_temp3243; void* _temp3245; void** _temp3247; struct Cyc_Absyn_PtrInfo _temp3248;
struct Cyc_Absyn_Conref* _temp3250; struct Cyc_Absyn_Tqual _temp3252; struct Cyc_Absyn_Conref*
_temp3254; void* _temp3256; void* _temp3258; struct Cyc_Absyn_Exp* _temp3260;
struct Cyc_Absyn_Tqual _temp3262; void* _temp3264; struct Cyc_Absyn_FnInfo
_temp3266; struct Cyc_List_List* _temp3268; struct Cyc_List_List* _temp3270;
struct Cyc_Absyn_VarargInfo* _temp3272; int _temp3274; struct Cyc_List_List*
_temp3276; void* _temp3278; struct Cyc_Core_Opt* _temp3280; struct Cyc_Core_Opt**
_temp3282; struct Cyc_List_List* _temp3283; struct Cyc_List_List** _temp3285;
struct Cyc_List_List* _temp3286; struct Cyc_List_List* _temp3288; struct Cyc_List_List*
_temp3290; struct Cyc_Absyn_Structdecl** _temp3292; struct Cyc_Absyn_Structdecl***
_temp3294; struct Cyc_List_List* _temp3295; struct _tuple1* _temp3297; struct
Cyc_Absyn_Uniondecl** _temp3299; struct Cyc_Absyn_Uniondecl*** _temp3301; struct
Cyc_List_List* _temp3302; struct _tuple1* _temp3304; struct Cyc_Core_Opt*
_temp3306; struct Cyc_Core_Opt** _temp3308; struct Cyc_List_List* _temp3309;
struct Cyc_List_List** _temp3311; struct _tuple1* _temp3312; void* _temp3314;
void* _temp3316; void* _temp3318; struct Cyc_List_List* _temp3320; _LL3169: if(
_temp3167 == ( void*) Cyc_Absyn_VoidType){ goto _LL3170;} else{ goto _LL3171;}
_LL3171: if(( unsigned int) _temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_Evar:
0){ _LL3221: _temp3220=(( struct Cyc_Absyn_Evar_struct*) _temp3167)->f1;
_temp3222=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3167)->f1;
goto _LL3218; _LL3218: _temp3217=(( struct Cyc_Absyn_Evar_struct*) _temp3167)->f2;
_temp3219=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3167)->f2;
goto _LL3172;} else{ goto _LL3173;} _LL3173: if(( unsigned int) _temp3167 >  4u?*((
int*) _temp3167) ==  Cyc_Absyn_VarType: 0){ _LL3224: _temp3223=(( struct Cyc_Absyn_VarType_struct*)
_temp3167)->f1; goto _LL3174;} else{ goto _LL3175;} _LL3175: if(( unsigned int)
_temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_AnonEnumType: 0){ _LL3226:
_temp3225=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp3167)->f1; goto _LL3176;}
else{ goto _LL3177;} _LL3177: if(( unsigned int) _temp3167 >  4u?*(( int*)
_temp3167) ==  Cyc_Absyn_EnumType: 0){ _LL3231: _temp3230=(( struct Cyc_Absyn_EnumType_struct*)
_temp3167)->f1; goto _LL3228; _LL3228: _temp3227=(( struct Cyc_Absyn_EnumType_struct*)
_temp3167)->f2; _temp3229=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp3167)->f2; goto _LL3178;} else{ goto _LL3179;} _LL3179: if(( unsigned int)
_temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_TunionType: 0){ _LL3233:
_temp3232=(( struct Cyc_Absyn_TunionType_struct*) _temp3167)->f1; _LL3239:
_temp3238=( void*) _temp3232.tunion_info; _temp3240=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp3167)->f1).tunion_info; goto _LL3237; _LL3237: _temp3236= _temp3232.targs;
goto _LL3235; _LL3235: _temp3234=( void*) _temp3232.rgn; goto _LL3180;} else{
goto _LL3181;} _LL3181: if(( unsigned int) _temp3167 >  4u?*(( int*) _temp3167)
==  Cyc_Absyn_TunionFieldType: 0){ _LL3242: _temp3241=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp3167)->f1; _LL3246: _temp3245=( void*) _temp3241.field_info; _temp3247=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp3167)->f1).field_info;
goto _LL3244; _LL3244: _temp3243= _temp3241.targs; goto _LL3182;} else{ goto
_LL3183;} _LL3183: if(( unsigned int) _temp3167 >  4u?*(( int*) _temp3167) == 
Cyc_Absyn_PointerType: 0){ _LL3249: _temp3248=(( struct Cyc_Absyn_PointerType_struct*)
_temp3167)->f1; _LL3259: _temp3258=( void*) _temp3248.elt_typ; goto _LL3257;
_LL3257: _temp3256=( void*) _temp3248.rgn_typ; goto _LL3255; _LL3255: _temp3254=
_temp3248.nullable; goto _LL3253; _LL3253: _temp3252= _temp3248.tq; goto _LL3251;
_LL3251: _temp3250= _temp3248.bounds; goto _LL3184;} else{ goto _LL3185;}
_LL3185: if(( unsigned int) _temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_IntType:
0){ goto _LL3186;} else{ goto _LL3187;} _LL3187: if( _temp3167 == ( void*) Cyc_Absyn_FloatType){
goto _LL3188;} else{ goto _LL3189;} _LL3189: if( _temp3167 == ( void*) Cyc_Absyn_DoubleType){
goto _LL3190;} else{ goto _LL3191;} _LL3191: if(( unsigned int) _temp3167 >  4u?*((
int*) _temp3167) ==  Cyc_Absyn_ArrayType: 0){ _LL3265: _temp3264=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3167)->f1; goto _LL3263; _LL3263:
_temp3262=(( struct Cyc_Absyn_ArrayType_struct*) _temp3167)->f2; goto _LL3261;
_LL3261: _temp3260=(( struct Cyc_Absyn_ArrayType_struct*) _temp3167)->f3; goto
_LL3192;} else{ goto _LL3193;} _LL3193: if(( unsigned int) _temp3167 >  4u?*((
int*) _temp3167) ==  Cyc_Absyn_FnType: 0){ _LL3267: _temp3266=(( struct Cyc_Absyn_FnType_struct*)
_temp3167)->f1; _LL3284: _temp3283= _temp3266.tvars; _temp3285=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp3167)->f1).tvars; goto _LL3281; _LL3281:
_temp3280= _temp3266.effect; _temp3282=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3167)->f1).effect; goto _LL3279; _LL3279: _temp3278=( void*) _temp3266.ret_typ;
goto _LL3277; _LL3277: _temp3276= _temp3266.args; goto _LL3275; _LL3275:
_temp3274= _temp3266.c_varargs; goto _LL3273; _LL3273: _temp3272= _temp3266.cyc_varargs;
goto _LL3271; _LL3271: _temp3270= _temp3266.rgn_po; goto _LL3269; _LL3269:
_temp3268= _temp3266.attributes; goto _LL3194;} else{ goto _LL3195;} _LL3195:
if(( unsigned int) _temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_TupleType:
0){ _LL3287: _temp3286=(( struct Cyc_Absyn_TupleType_struct*) _temp3167)->f1;
goto _LL3196;} else{ goto _LL3197;} _LL3197: if(( unsigned int) _temp3167 >  4u?*((
int*) _temp3167) ==  Cyc_Absyn_AnonStructType: 0){ _LL3289: _temp3288=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3167)->f1; goto _LL3198;} else{ goto
_LL3199;} _LL3199: if(( unsigned int) _temp3167 >  4u?*(( int*) _temp3167) == 
Cyc_Absyn_AnonUnionType: 0){ _LL3291: _temp3290=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3167)->f1; goto _LL3200;} else{ goto _LL3201;} _LL3201: if(( unsigned int)
_temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_StructType: 0){ _LL3298:
_temp3297=(( struct Cyc_Absyn_StructType_struct*) _temp3167)->f1; goto _LL3296;
_LL3296: _temp3295=(( struct Cyc_Absyn_StructType_struct*) _temp3167)->f2; goto
_LL3293; _LL3293: _temp3292=(( struct Cyc_Absyn_StructType_struct*) _temp3167)->f3;
_temp3294=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp3167)->f3; goto _LL3202;} else{ goto _LL3203;} _LL3203: if(( unsigned int)
_temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_UnionType: 0){ _LL3305:
_temp3304=(( struct Cyc_Absyn_UnionType_struct*) _temp3167)->f1; goto _LL3303;
_LL3303: _temp3302=(( struct Cyc_Absyn_UnionType_struct*) _temp3167)->f2; goto
_LL3300; _LL3300: _temp3299=(( struct Cyc_Absyn_UnionType_struct*) _temp3167)->f3;
_temp3301=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp3167)->f3; goto _LL3204;} else{ goto _LL3205;} _LL3205: if(( unsigned int)
_temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_TypedefType: 0){ _LL3313:
_temp3312=(( struct Cyc_Absyn_TypedefType_struct*) _temp3167)->f1; goto _LL3310;
_LL3310: _temp3309=(( struct Cyc_Absyn_TypedefType_struct*) _temp3167)->f2;
_temp3311=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3167)->f2; goto _LL3307; _LL3307: _temp3306=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3167)->f3; _temp3308=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3167)->f3; goto _LL3206;} else{ goto _LL3207;} _LL3207: if( _temp3167 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL3208;} else{ goto _LL3209;} _LL3209: if((
unsigned int) _temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_RgnHandleType:
0){ _LL3315: _temp3314=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp3167)->f1; goto _LL3210;} else{ goto _LL3211;} _LL3211: if(( unsigned int)
_temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_AccessEff: 0){ _LL3317:
_temp3316=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp3167)->f1; goto
_LL3212;} else{ goto _LL3213;} _LL3213: if(( unsigned int) _temp3167 >  4u?*((
int*) _temp3167) ==  Cyc_Absyn_RgnsEff: 0){ _LL3319: _temp3318=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp3167)->f1; goto _LL3214;} else{ goto _LL3215;}
_LL3215: if(( unsigned int) _temp3167 >  4u?*(( int*) _temp3167) ==  Cyc_Absyn_JoinEff:
0){ _LL3321: _temp3320=(( struct Cyc_Absyn_JoinEff_struct*) _temp3167)->f1; goto
_LL3216;} else{ goto _LL3168;} _LL3170: goto _LL3168; _LL3172: if(* _temp3222 == 
0){* _temp3222=({ struct Cyc_Core_Opt* _temp3322=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3322->v=( void*) expected_kind;
_temp3322;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp3219=({ struct Cyc_Core_Opt* _temp3323=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3323->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp3323;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp3324= Cyc_Tcutil_new_tvar( expected_kind);*
_temp3219=({ struct Cyc_Core_Opt* _temp3325=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3325->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3326=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3326[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3327; _temp3327.tag= Cyc_Absyn_VarType;
_temp3327.f1= _temp3324; _temp3327;}); _temp3326;})); _temp3325;}); _temp3223=
_temp3324; goto _LL3174;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL3168; _LL3174: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp3223->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3328=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3328[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3329; _temp3329.tag= Cyc_Absyn_Eq_constr;
_temp3329.f1=( void*) expected_kind; _temp3329;}); _temp3328;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp3223); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp3223); goto _LL3168;}
_LL3176: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);{ struct
_RegionHandle _temp3330= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3330; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; for( 0; _temp3225 !=  0; _temp3225= _temp3225->tl){
struct Cyc_Absyn_Enumfield* _temp3331=( struct Cyc_Absyn_Enumfield*) _temp3225->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp,
prev_fields,(* _temp3331->name).f2)){({ struct Cyc_Std_String_pa_struct
_temp3333; _temp3333.tag= Cyc_Std_String_pa; _temp3333.f1=( struct _tagged_arr)*(*
_temp3331->name).f2;{ void* _temp3332[ 1u]={& _temp3333}; Cyc_Tcutil_terr(
_temp3331->loc, _tag_arr("duplicate enum field name %s", sizeof( unsigned char),
29u), _tag_arr( _temp3332, sizeof( void*), 1u));}});} else{ prev_fields=({
struct Cyc_List_List* _temp3334=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp3334->hd=( void*)(* _temp3331->name).f2;
_temp3334->tl= prev_fields; _temp3334;});} if( _temp3331->tag ==  0){ _temp3331->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp3331->loc);} else{
if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null(
_temp3331->tag))){({ struct Cyc_Std_String_pa_struct _temp3336; _temp3336.tag=
Cyc_Std_String_pa; _temp3336.f1=( struct _tagged_arr)*(* _temp3331->name).f2;{
void* _temp3335[ 1u]={& _temp3336}; Cyc_Tcutil_terr( loc, _tag_arr("enum field %s: expression is not constant",
sizeof( unsigned char), 42u), _tag_arr( _temp3335, sizeof( void*), 1u));}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp3331->tag)); tag_count= t1 +  1;(* _temp3331->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp3337=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp3337[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp3338; _temp3338.tag= Cyc_Absyn_Abs_n; _temp3338.f1= te->ns; _temp3338;});
_temp3337;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple17* v)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp3331->name).f2,({ struct _tuple17* _temp3339=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp3339->f1=( void*)({ struct Cyc_Tcenv_AnonEnumRes_struct*
_temp3340=( struct Cyc_Tcenv_AnonEnumRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_AnonEnumRes_struct));
_temp3340[ 0]=({ struct Cyc_Tcenv_AnonEnumRes_struct _temp3341; _temp3341.tag=
Cyc_Tcenv_AnonEnumRes; _temp3341.f1=( void*) t; _temp3341.f2= _temp3331;
_temp3341;}); _temp3340;}); _temp3339->f2= 1; _temp3339;}));}}}; _pop_region(
uprev_rgn);} goto _LL3168;} _LL3178: if(* _temp3229 ==  0){ struct _handler_cons
_temp3342; _push_handler(& _temp3342);{ int _temp3344= 0; if( setjmp( _temp3342.handler)){
_temp3344= 1;} if( ! _temp3344){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3230);* _temp3229=( struct Cyc_Absyn_Enumdecl*)* ed;};
_pop_handler();} else{ void* _temp3343=( void*) _exn_thrown; void* _temp3346=
_temp3343; _LL3348: if( _temp3346 ==  Cyc_Dict_Absent){ goto _LL3349;} else{
goto _LL3350;} _LL3350: goto _LL3351; _LL3349: { struct Cyc_Tcenv_Genv*
_temp3352=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Enumdecl* _temp3353=({
struct Cyc_Absyn_Enumdecl* _temp3354=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp3354->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp3354->name= _temp3230; _temp3354->fields= 0; _temp3354;}); Cyc_Tc_tcEnumdecl(
te, _temp3352, loc, _temp3353);{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3230);* _temp3229=( struct Cyc_Absyn_Enumdecl*)* ed; goto _LL3347;}}
_LL3351:( void) _throw( _temp3346); _LL3347:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp3229);* _temp3230=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL3168;} _LL3180:{ void*
_temp3355=* _temp3240; struct Cyc_Absyn_UnknownTunionInfo _temp3361; int
_temp3363; struct _tuple1* _temp3365; struct Cyc_Absyn_Tuniondecl** _temp3367;
struct Cyc_Absyn_Tuniondecl* _temp3369; _LL3357: if(*(( int*) _temp3355) ==  Cyc_Absyn_UnknownTunion){
_LL3362: _temp3361=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp3355)->f1;
_LL3366: _temp3365= _temp3361.name; goto _LL3364; _LL3364: _temp3363= _temp3361.is_xtunion;
goto _LL3358;} else{ goto _LL3359;} _LL3359: if(*(( int*) _temp3355) ==  Cyc_Absyn_KnownTunion){
_LL3368: _temp3367=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3355)->f1;
_temp3369=* _temp3367; goto _LL3360;} else{ goto _LL3356;} _LL3358: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp3370; _push_handler(& _temp3370);{ int
_temp3372= 0; if( setjmp( _temp3370.handler)){ _temp3372= 1;} if( ! _temp3372){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp3365);; _pop_handler();} else{
void* _temp3371=( void*) _exn_thrown; void* _temp3374= _temp3371; _LL3376: if(
_temp3374 ==  Cyc_Dict_Absent){ goto _LL3377;} else{ goto _LL3378;} _LL3378:
goto _LL3379; _LL3377: { struct Cyc_Tcenv_Genv* _temp3380=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Tuniondecl* _temp3381=({ struct Cyc_Absyn_Tuniondecl* _temp3385=(
struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp3385->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3385->name= _temp3365;
_temp3385->tvs= 0; _temp3385->fields= 0; _temp3385->is_xtunion= _temp3363;
_temp3385;}); Cyc_Tc_tcTuniondecl( te, _temp3380, loc, _temp3381); tudp= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp3365); if( _temp3236 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3384; _temp3384.tag= Cyc_Std_String_pa; _temp3384.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3365);{ struct Cyc_Std_String_pa_struct _temp3383;
_temp3383.tag= Cyc_Std_String_pa; _temp3383.f1=( struct _tagged_arr)( _temp3363?
_tag_arr("xtunion", sizeof( unsigned char), 8u): _tag_arr("tunion", sizeof(
unsigned char), 7u));{ void* _temp3382[ 2u]={& _temp3383,& _temp3384}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized %s %s before using", sizeof(
unsigned char), 48u), _tag_arr( _temp3382, sizeof( void*), 2u));}}}); return
cvtenv;} goto _LL3375;} _LL3379:( void) _throw( _temp3374); _LL3375:;}}} if((*
tudp)->is_xtunion !=  _temp3363){({ struct Cyc_Std_String_pa_struct _temp3387;
_temp3387.tag= Cyc_Std_String_pa; _temp3387.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3365);{ void* _temp3386[ 1u]={& _temp3387}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp3386, sizeof( void*), 1u));}});}*
_temp3240=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp3388=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp3388[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp3389; _temp3389.tag= Cyc_Absyn_KnownTunion;
_temp3389.f1= tudp; _temp3389;}); _temp3388;}); _temp3369=* tudp; goto _LL3360;}
_LL3360: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3234);{ struct Cyc_List_List* tvs= _temp3369->tvs; for( 0; _temp3236 !=  0?
tvs !=  0: 0;( _temp3236= _temp3236->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3236->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*) tvs->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp3236
!=  0){({ struct Cyc_Std_String_pa_struct _temp3391; _temp3391.tag= Cyc_Std_String_pa;
_temp3391.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3369->name);{
void* _temp3390[ 1u]={& _temp3391}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp3390, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp3393; _temp3393.tag= Cyc_Std_String_pa;
_temp3393.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3369->name);{
void* _temp3392[ 1u]={& _temp3393}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp3392, sizeof( void*), 1u));}});}
goto _LL3356;} _LL3356:;} goto _LL3168; _LL3182:{ void* _temp3394=* _temp3247;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp3400; int _temp3402; struct _tuple1*
_temp3404; struct _tuple1* _temp3406; struct Cyc_Absyn_Tunionfield* _temp3408;
struct Cyc_Absyn_Tuniondecl* _temp3410; _LL3396: if(*(( int*) _temp3394) ==  Cyc_Absyn_UnknownTunionfield){
_LL3401: _temp3400=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp3394)->f1;
_LL3407: _temp3406= _temp3400.tunion_name; goto _LL3405; _LL3405: _temp3404=
_temp3400.field_name; goto _LL3403; _LL3403: _temp3402= _temp3400.is_xtunion;
goto _LL3397;} else{ goto _LL3398;} _LL3398: if(*(( int*) _temp3394) ==  Cyc_Absyn_KnownTunionfield){
_LL3411: _temp3410=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp3394)->f1;
goto _LL3409; _LL3409: _temp3408=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp3394)->f2; goto _LL3399;} else{ goto _LL3395;} _LL3397: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp3412;
_push_handler(& _temp3412);{ int _temp3414= 0; if( setjmp( _temp3412.handler)){
_temp3414= 1;} if( ! _temp3414){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp3406);; _pop_handler();} else{ void* _temp3413=( void*) _exn_thrown; void*
_temp3416= _temp3413; _LL3418: if( _temp3416 ==  Cyc_Dict_Absent){ goto _LL3419;}
else{ goto _LL3420;} _LL3420: goto _LL3421; _LL3419:({ struct Cyc_Std_String_pa_struct
_temp3423; _temp3423.tag= Cyc_Std_String_pa; _temp3423.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3406);{ void* _temp3422[ 1u]={& _temp3423}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp3422, sizeof( void*), 1u));}}); return cvtenv; _LL3421:( void) _throw(
_temp3416); _LL3417:;}}}{ struct _handler_cons _temp3424; _push_handler(&
_temp3424);{ int _temp3426= 0; if( setjmp( _temp3424.handler)){ _temp3426= 1;}
if( ! _temp3426){{ void* _temp3427= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp3404); struct Cyc_Absyn_Tunionfield* _temp3433; struct Cyc_Absyn_Tuniondecl*
_temp3435; _LL3429: if(*(( int*) _temp3427) ==  Cyc_Tcenv_TunionRes){ _LL3436:
_temp3435=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3427)->f1; goto _LL3434;
_LL3434: _temp3433=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3427)->f2; goto
_LL3430;} else{ goto _LL3431;} _LL3431: goto _LL3432; _LL3430: tuf= _temp3433;
tud= _temp3435; if( tud->is_xtunion !=  _temp3402){({ struct Cyc_Std_String_pa_struct
_temp3438; _temp3438.tag= Cyc_Std_String_pa; _temp3438.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3406);{ void* _temp3437[ 1u]={& _temp3438}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp3437, sizeof( void*), 1u));}});} goto
_LL3428; _LL3432:({ struct Cyc_Std_String_pa_struct _temp3441; _temp3441.tag=
Cyc_Std_String_pa; _temp3441.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3406);{ struct Cyc_Std_String_pa_struct _temp3440; _temp3440.tag= Cyc_Std_String_pa;
_temp3440.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3404);{ void*
_temp3439[ 2u]={& _temp3440,& _temp3441}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp3439, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp3442= cvtenv; _npop_handler( 0u); return _temp3442;}
_LL3428:;}; _pop_handler();} else{ void* _temp3425=( void*) _exn_thrown; void*
_temp3444= _temp3425; _LL3446: if( _temp3444 ==  Cyc_Dict_Absent){ goto _LL3447;}
else{ goto _LL3448;} _LL3448: goto _LL3449; _LL3447:({ struct Cyc_Std_String_pa_struct
_temp3452; _temp3452.tag= Cyc_Std_String_pa; _temp3452.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3406);{ struct Cyc_Std_String_pa_struct _temp3451;
_temp3451.tag= Cyc_Std_String_pa; _temp3451.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3404);{ void* _temp3450[ 2u]={& _temp3451,& _temp3452}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3450, sizeof( void*), 2u));}}}); return cvtenv; _LL3449:( void)
_throw( _temp3444); _LL3445:;}}}* _temp3247=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp3453=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp3453[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp3454; _temp3454.tag= Cyc_Absyn_KnownTunionfield; _temp3454.f1= tud;
_temp3454.f2= tuf; _temp3454;}); _temp3453;}); _temp3410= tud; _temp3408= tuf;
goto _LL3399;} _LL3399: { struct Cyc_List_List* tvs= _temp3410->tvs; for( 0;
_temp3243 !=  0? tvs !=  0: 0;( _temp3243= _temp3243->tl, tvs= tvs->tl)){ void*
t1=( void*) _temp3243->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)
tvs->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);}
if( _temp3243 !=  0){({ struct Cyc_Std_String_pa_struct _temp3457; _temp3457.tag=
Cyc_Std_String_pa; _temp3457.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3408->name);{ struct Cyc_Std_String_pa_struct _temp3456; _temp3456.tag= Cyc_Std_String_pa;
_temp3456.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3410->name);{
void* _temp3455[ 2u]={& _temp3456,& _temp3457}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp3455, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp3460; _temp3460.tag= Cyc_Std_String_pa;
_temp3460.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3408->name);{
struct Cyc_Std_String_pa_struct _temp3459; _temp3459.tag= Cyc_Std_String_pa;
_temp3459.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3410->name);{
void* _temp3458[ 2u]={& _temp3459,& _temp3460}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp3458, sizeof( void*), 2u));}}});}
goto _LL3395;} _LL3395:;} goto _LL3168; _LL3184: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3258); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp3256);{ void* _temp3461=( void*)(
Cyc_Absyn_compress_conref( _temp3250))->v; void* _temp3467; struct Cyc_Absyn_Exp*
_temp3469; _LL3463: if(( unsigned int) _temp3461 >  1u?*(( int*) _temp3461) == 
Cyc_Absyn_Eq_constr: 0){ _LL3468: _temp3467=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3461)->f1; if(( unsigned int) _temp3467 >  1u?*(( int*) _temp3467) ==  Cyc_Absyn_Upper_b:
0){ _LL3470: _temp3469=(( struct Cyc_Absyn_Upper_b_struct*) _temp3467)->f1; goto
_LL3464;} else{ goto _LL3465;}} else{ goto _LL3465;} _LL3465: goto _LL3466;
_LL3464: if( ! Cyc_Tcutil_is_const_exp( te, _temp3469)){({ void* _temp3471[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp3471, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp3469); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp3469)){({ void*
_temp3472[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp3472, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp3469); goto _LL3462; _LL3466: goto _LL3462; _LL3462:;} goto _LL3168;
_LL3186: goto _LL3168; _LL3188: goto _LL3168; _LL3190: goto _LL3168; _LL3192:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3264); if( _temp3260 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp3260))){({ void* _temp3473[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp3473, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp3260)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp3260))){({ void* _temp3474[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp3474, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp3260)); goto _LL3168; _LL3194: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp3268 !=  0; _temp3268= _temp3268->tl){ if(
! Cyc_Absyn_fntype_att(( void*) _temp3268->hd)){({ struct Cyc_Std_String_pa_struct
_temp3476; _temp3476.tag= Cyc_Std_String_pa; _temp3476.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*) _temp3268->hd);{ void* _temp3475[ 1u]={&
_temp3476}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp3475, sizeof( void*), 1u));}});}{
void* _temp3477=( void*) _temp3268->hd; int _temp3489; int _temp3491; void*
_temp3493; _LL3479: if( _temp3477 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL3480;} else{ goto _LL3481;} _LL3481: if( _temp3477 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL3482;} else{ goto _LL3483;} _LL3483: if( _temp3477 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL3484;} else{ goto _LL3485;} _LL3485: if(( unsigned int) _temp3477 >  16u?*((
int*) _temp3477) ==  Cyc_Absyn_Format_att: 0){ _LL3494: _temp3493=( void*)((
struct Cyc_Absyn_Format_att_struct*) _temp3477)->f1; goto _LL3492; _LL3492:
_temp3491=(( struct Cyc_Absyn_Format_att_struct*) _temp3477)->f2; goto _LL3490;
_LL3490: _temp3489=(( struct Cyc_Absyn_Format_att_struct*) _temp3477)->f3; goto
_LL3486;} else{ goto _LL3487;} _LL3487: goto _LL3488; _LL3480: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL3478; _LL3482: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL3478; _LL3484: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL3478; _LL3486: if( !
seen_format){ seen_format= 1; ft= _temp3493; fmt_desc_arg= _temp3491;
fmt_arg_start= _temp3489;} else{({ void* _temp3495[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp3495, sizeof( void*), 0u));});} goto _LL3478; _LL3488: goto
_LL3478; _LL3478:;}} if( num_convs >  1){({ void* _temp3496[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp3496, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp3285);{ struct Cyc_List_List* b=* _temp3285; for( 0; b !=  0; b= b->tl){((
struct Cyc_Absyn_Tvar*) b->hd)->identity= Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env=
Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*) b->hd);{
void* _temp3497=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*) b->hd)->kind))->v;
void* _temp3503; _LL3499: if(( unsigned int) _temp3497 >  1u?*(( int*) _temp3497)
==  Cyc_Absyn_Eq_constr: 0){ _LL3504: _temp3503=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3497)->f1; if( _temp3503 == ( void*) Cyc_Absyn_MemKind){ goto _LL3500;}
else{ goto _LL3501;}} else{ goto _LL3501;} _LL3501: goto _LL3502; _LL3500:({
struct Cyc_Std_String_pa_struct _temp3506; _temp3506.tag= Cyc_Std_String_pa;
_temp3506.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) b->hd)->name;{
void* _temp3505[ 1u]={& _temp3506}; Cyc_Tcutil_terr( loc, _tag_arr("function attempts to abstract Mem type variable %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3505, sizeof( void*), 1u));}});
goto _LL3498; _LL3502: goto _LL3498; _LL3498:;}}}{ struct Cyc_Tcutil_CVTEnv
_temp3507=({ struct Cyc_Tcutil_CVTEnv _temp3666; _temp3666.kind_env= cvtenv.kind_env;
_temp3666.free_vars= 0; _temp3666.free_evars= 0; _temp3666.generalize_evars=
cvtenv.generalize_evars; _temp3666.fn_result= 1; _temp3666;}); _temp3507= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3507,( void*) Cyc_Absyn_MemKind, _temp3278); _temp3507.fn_result=
0;{ struct Cyc_List_List* a= _temp3276; for( 0; a !=  0; a= a->tl){ _temp3507=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3507,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*) a->hd)).f3);}} if( _temp3272 !=  0){ if( _temp3274){({ void*
_temp3508[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp3508, sizeof( void*), 0u));});}{ int _temp3511; void* _temp3513; struct Cyc_Absyn_Tqual
_temp3515; struct Cyc_Core_Opt* _temp3517; struct Cyc_Absyn_VarargInfo _temp3509=*
_temp3272; _LL3518: _temp3517= _temp3509.name; goto _LL3516; _LL3516: _temp3515=
_temp3509.tq; goto _LL3514; _LL3514: _temp3513=( void*) _temp3509.type; goto
_LL3512; _LL3512: _temp3511= _temp3509.inject; goto _LL3510; _LL3510: _temp3507=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3507,( void*) Cyc_Absyn_MemKind,
_temp3513); if( _temp3511){ void* _temp3519= Cyc_Tcutil_compress( _temp3513);
struct Cyc_Absyn_TunionInfo _temp3525; void* _temp3527; void* _temp3529; _LL3521:
if(( unsigned int) _temp3519 >  4u?*(( int*) _temp3519) ==  Cyc_Absyn_TunionType:
0){ _LL3526: _temp3525=(( struct Cyc_Absyn_TunionType_struct*) _temp3519)->f1;
_LL3530: _temp3529=( void*) _temp3525.tunion_info; if(*(( int*) _temp3529) == 
Cyc_Absyn_KnownTunion){ goto _LL3528;} else{ goto _LL3523;} _LL3528: _temp3527=(
void*) _temp3525.rgn; goto _LL3522;} else{ goto _LL3523;} _LL3523: goto _LL3524;
_LL3522: goto _LL3520; _LL3524:({ void* _temp3531[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("can't inject a non-[x]tunion type", sizeof( unsigned char), 34u),
_tag_arr( _temp3531, sizeof( void*), 0u));}); goto _LL3520; _LL3520:;}}} if(
seen_format){ int _temp3532=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp3276); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg >  _temp3532)? 1:
fmt_arg_start <  0)? 1:( _temp3272 ==  0? fmt_arg_start !=  0: 0))? 1:(
_temp3272 !=  0? fmt_arg_start !=  _temp3532 +  1: 0)){({ void* _temp3533[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp3533, sizeof( void*), 0u));});} else{ void* _temp3536;
struct _tuple2 _temp3534=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp3276, fmt_desc_arg -  1); _LL3537: _temp3536= _temp3534.f3;
goto _LL3535; _LL3535:{ void* _temp3538= Cyc_Tcutil_compress( _temp3536); struct
Cyc_Absyn_PtrInfo _temp3544; struct Cyc_Absyn_Conref* _temp3546; void* _temp3548;
_LL3540: if(( unsigned int) _temp3538 >  4u?*(( int*) _temp3538) ==  Cyc_Absyn_PointerType:
0){ _LL3545: _temp3544=(( struct Cyc_Absyn_PointerType_struct*) _temp3538)->f1;
_LL3549: _temp3548=( void*) _temp3544.elt_typ; goto _LL3547; _LL3547: _temp3546=
_temp3544.bounds; goto _LL3541;} else{ goto _LL3542;} _LL3542: goto _LL3543;
_LL3541:{ struct _tuple8 _temp3551=({ struct _tuple8 _temp3550; _temp3550.f1=
Cyc_Tcutil_compress( _temp3548); _temp3550.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp3546); _temp3550;}); void* _temp3557; void* _temp3559; void* _temp3561;
void* _temp3563; _LL3553: _LL3560: _temp3559= _temp3551.f1; if(( unsigned int)
_temp3559 >  4u?*(( int*) _temp3559) ==  Cyc_Absyn_IntType: 0){ _LL3564:
_temp3563=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3559)->f1; if(
_temp3563 == ( void*) Cyc_Absyn_Unsigned){ goto _LL3562;} else{ goto _LL3555;}
_LL3562: _temp3561=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3559)->f2;
if( _temp3561 == ( void*) Cyc_Absyn_B1){ goto _LL3558;} else{ goto _LL3555;}}
else{ goto _LL3555;} _LL3558: _temp3557= _temp3551.f2; if( _temp3557 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL3554;} else{ goto _LL3555;} _LL3555: goto _LL3556;
_LL3554: goto _LL3552; _LL3556:({ void* _temp3565[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp3565, sizeof( void*), 0u));}); goto _LL3552; _LL3552:;} goto
_LL3539; _LL3543:({ void* _temp3566[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp3566, sizeof( void*), 0u));}); goto
_LL3539; _LL3539:;} if( fmt_arg_start !=  0){ void* _temp3567= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp3272))->type); int
problem;{ void* _temp3568= ft; _LL3570: if( _temp3568 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL3571;} else{ goto _LL3572;} _LL3572: if( _temp3568 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL3573;} else{ goto _LL3569;} _LL3571:{ void* _temp3574= _temp3567; struct
Cyc_Absyn_TunionInfo _temp3580; void* _temp3582; struct Cyc_Absyn_Tuniondecl**
_temp3584; struct Cyc_Absyn_Tuniondecl* _temp3586; _LL3576: if(( unsigned int)
_temp3574 >  4u?*(( int*) _temp3574) ==  Cyc_Absyn_TunionType: 0){ _LL3581:
_temp3580=(( struct Cyc_Absyn_TunionType_struct*) _temp3574)->f1; _LL3583:
_temp3582=( void*) _temp3580.tunion_info; if(*(( int*) _temp3582) ==  Cyc_Absyn_KnownTunion){
_LL3585: _temp3584=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3582)->f1;
_temp3586=* _temp3584; goto _LL3577;} else{ goto _LL3578;}} else{ goto _LL3578;}
_LL3578: goto _LL3579; _LL3577: problem= Cyc_Absyn_qvar_cmp( _temp3586->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL3575; _LL3579: problem= 1; goto
_LL3575; _LL3575:;} goto _LL3569; _LL3573:{ void* _temp3587= _temp3567; struct
Cyc_Absyn_TunionInfo _temp3593; void* _temp3595; struct Cyc_Absyn_Tuniondecl**
_temp3597; struct Cyc_Absyn_Tuniondecl* _temp3599; _LL3589: if(( unsigned int)
_temp3587 >  4u?*(( int*) _temp3587) ==  Cyc_Absyn_TunionType: 0){ _LL3594:
_temp3593=(( struct Cyc_Absyn_TunionType_struct*) _temp3587)->f1; _LL3596:
_temp3595=( void*) _temp3593.tunion_info; if(*(( int*) _temp3595) ==  Cyc_Absyn_KnownTunion){
_LL3598: _temp3597=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3595)->f1;
_temp3599=* _temp3597; goto _LL3590;} else{ goto _LL3591;}} else{ goto _LL3591;}
_LL3591: goto _LL3592; _LL3590: problem= Cyc_Absyn_qvar_cmp( _temp3599->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL3588; _LL3592: problem= 1; goto
_LL3588; _LL3588:;} goto _LL3569; _LL3569:;} if( problem){({ void* _temp3600[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3600, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp3270; for( 0; rpo !=  0; rpo= rpo->tl){ struct
_tuple8 _temp3603; void* _temp3604; void* _temp3606; struct _tuple8* _temp3601=(
struct _tuple8*) rpo->hd; _temp3603=* _temp3601; _LL3607: _temp3606= _temp3603.f1;
goto _LL3605; _LL3605: _temp3604= _temp3603.f2; goto _LL3602; _LL3602: _temp3507=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3507,( void*) Cyc_Absyn_RgnKind,
_temp3606); _temp3507= Cyc_Tcutil_i_check_valid_type( loc, te, _temp3507,( void*)
Cyc_Absyn_RgnKind, _temp3604);}} if(* _temp3282 !=  0){ _temp3507= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3507,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp3282))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp3507.free_vars; for(
0; tvs !=  0; tvs= tvs->tl){ void* _temp3608=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind))->v; void* _temp3616; void* _temp3618;
_LL3610: if(( unsigned int) _temp3608 >  1u?*(( int*) _temp3608) ==  Cyc_Absyn_Eq_constr:
0){ _LL3617: _temp3616=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3608)->f1;
if( _temp3616 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3611;} else{ goto _LL3612;}}
else{ goto _LL3612;} _LL3612: if(( unsigned int) _temp3608 >  1u?*(( int*)
_temp3608) ==  Cyc_Absyn_Eq_constr: 0){ _LL3619: _temp3618=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3608)->f1; if( _temp3618 == ( void*) Cyc_Absyn_EffKind){ goto _LL3613;}
else{ goto _LL3614;}} else{ goto _LL3614;} _LL3614: goto _LL3615; _LL3611:
effect=({ struct Cyc_List_List* _temp3620=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3620->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3621=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3621[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3622; _temp3622.tag= Cyc_Absyn_AccessEff;
_temp3622.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3623=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3623[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3624; _temp3624.tag= Cyc_Absyn_VarType;
_temp3624.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3624;}); _temp3623;}));
_temp3622;}); _temp3621;})); _temp3620->tl= effect; _temp3620;}); goto _LL3609;
_LL3613: effect=({ struct Cyc_List_List* _temp3625=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3625->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp3626=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3626[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3627; _temp3627.tag= Cyc_Absyn_VarType; _temp3627.f1=(
struct Cyc_Absyn_Tvar*) tvs->hd; _temp3627;}); _temp3626;})); _temp3625->tl=
effect; _temp3625;}); goto _LL3609; _LL3615: effect=({ struct Cyc_List_List*
_temp3628=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3628->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp3629=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3629[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3630; _temp3630.tag= Cyc_Absyn_RgnsEff;
_temp3630.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3631=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3631[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3632; _temp3632.tag= Cyc_Absyn_VarType;
_temp3632.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3632;}); _temp3631;}));
_temp3630;}); _temp3629;})); _temp3628->tl= effect; _temp3628;}); goto _LL3609;
_LL3609:;}} effect= Cyc_List_imp_rev( effect);{ struct Cyc_List_List* ts=
_temp3507.free_evars; for( 0; ts !=  0; ts= ts->tl){ void* _temp3633= Cyc_Tcutil_typ_kind((
void*) ts->hd); _LL3635: if( _temp3633 == ( void*) Cyc_Absyn_RgnKind){ goto
_LL3636;} else{ goto _LL3637;} _LL3637: if( _temp3633 == ( void*) Cyc_Absyn_EffKind){
goto _LL3638;} else{ goto _LL3639;} _LL3639: goto _LL3640; _LL3636: effect=({
struct Cyc_List_List* _temp3641=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3641->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3642=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3642[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3643; _temp3643.tag= Cyc_Absyn_AccessEff;
_temp3643.f1=( void*)(( void*) ts->hd); _temp3643;}); _temp3642;})); _temp3641->tl=
effect; _temp3641;}); goto _LL3634; _LL3638: effect=({ struct Cyc_List_List*
_temp3644=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3644->hd=( void*)(( void*) ts->hd); _temp3644->tl= effect; _temp3644;});
goto _LL3634; _LL3640: effect=({ struct Cyc_List_List* _temp3645=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3645->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3646=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3646[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3647; _temp3647.tag= Cyc_Absyn_RgnsEff; _temp3647.f1=(
void*)(( void*) ts->hd); _temp3647;}); _temp3646;})); _temp3645->tl= effect;
_temp3645;}); goto _LL3634; _LL3634:;}}* _temp3282=({ struct Cyc_Core_Opt*
_temp3648=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3648->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3649=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3649[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3650; _temp3650.tag= Cyc_Absyn_JoinEff;
_temp3650.f1= Cyc_List_imp_rev( effect); _temp3650;}); _temp3649;})); _temp3648;});}}
if(* _temp3285 !=  0){ struct Cyc_List_List* bs=* _temp3285; for( 0; bs !=  0;
bs= bs->tl){ struct Cyc_Absyn_Conref* _temp3651= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*) bs->hd)->kind); void* _temp3652=( void*) _temp3651->v;
void* _temp3660; _LL3654: if( _temp3652 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3655;} else{ goto _LL3656;} _LL3656: if(( unsigned int) _temp3652 >  1u?*((
int*) _temp3652) ==  Cyc_Absyn_Eq_constr: 0){ _LL3661: _temp3660=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3652)->f1; if( _temp3660 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3657;} else{ goto _LL3658;}} else{ goto _LL3658;}
_LL3658: goto _LL3659; _LL3655:({ struct Cyc_Std_String_pa_struct _temp3663;
_temp3663.tag= Cyc_Std_String_pa; _temp3663.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*) bs->hd)->name;{ void* _temp3662[ 1u]={& _temp3663}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3662, sizeof( void*), 1u));}}); goto
_LL3657; _LL3657:( void*)( _temp3651->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3664=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3664[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3665; _temp3665.tag= Cyc_Absyn_Eq_constr;
_temp3665.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3665;}); _temp3664;})));
goto _LL3653; _LL3659: goto _LL3653; _LL3653:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp3507.kind_env,* _temp3285); _temp3507.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp3507.free_vars,* _temp3285);{ struct Cyc_List_List* tvs= _temp3507.free_vars;
for( 0; tvs !=  0; tvs= tvs->tl){ cvtenv.free_vars= Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,( struct Cyc_Absyn_Tvar*) tvs->hd);}}{ struct Cyc_List_List*
evs= _temp3507.free_evars; for( 0; evs !=  0; evs= evs->tl){ cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars,( void*) evs->hd);}} goto _LL3168;}}
_LL3196: for( 0; _temp3286 !=  0; _temp3286= _temp3286->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp3286->hd)).f2);}
goto _LL3168; _LL3198:{ struct _RegionHandle _temp3667= _new_region(); struct
_RegionHandle* sprev_rgn=& _temp3667; _push_region( sprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp3288 !=  0; _temp3288= _temp3288->tl){ struct Cyc_Absyn_Structfield
_temp3670; struct Cyc_List_List* _temp3671; struct Cyc_Absyn_Exp* _temp3673;
void* _temp3675; struct Cyc_Absyn_Tqual _temp3677; struct _tagged_arr* _temp3679;
struct Cyc_Absyn_Structfield* _temp3668=( struct Cyc_Absyn_Structfield*)
_temp3288->hd; _temp3670=* _temp3668; _LL3680: _temp3679= _temp3670.name; goto
_LL3678; _LL3678: _temp3677= _temp3670.tq; goto _LL3676; _LL3676: _temp3675=(
void*) _temp3670.type; goto _LL3674; _LL3674: _temp3673= _temp3670.width; goto
_LL3672; _LL3672: _temp3671= _temp3670.attributes; goto _LL3669; _LL3669: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3679)){({ struct Cyc_Std_String_pa_struct _temp3682; _temp3682.tag= Cyc_Std_String_pa;
_temp3682.f1=( struct _tagged_arr)* _temp3679;{ void* _temp3681[ 1u]={&
_temp3682}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3681, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3679, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3683=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3683->hd=( void*)
_temp3679; _temp3683->tl= prev_fields; _temp3683;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3675); Cyc_Tcutil_check_bitfield(
loc, te, _temp3675, _temp3673, _temp3679); Cyc_Tcutil_check_field_atts( loc,
_temp3679, _temp3671);}}; _pop_region( sprev_rgn);} goto _LL3168; _LL3200:{
struct _RegionHandle _temp3684= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3684; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp3290 !=  0; _temp3290= _temp3290->tl){ struct Cyc_Absyn_Structfield
_temp3687; struct Cyc_List_List* _temp3688; struct Cyc_Absyn_Exp* _temp3690;
void* _temp3692; struct Cyc_Absyn_Tqual _temp3694; struct _tagged_arr* _temp3696;
struct Cyc_Absyn_Structfield* _temp3685=( struct Cyc_Absyn_Structfield*)
_temp3290->hd; _temp3687=* _temp3685; _LL3697: _temp3696= _temp3687.name; goto
_LL3695; _LL3695: _temp3694= _temp3687.tq; goto _LL3693; _LL3693: _temp3692=(
void*) _temp3687.type; goto _LL3691; _LL3691: _temp3690= _temp3687.width; goto
_LL3689; _LL3689: _temp3688= _temp3687.attributes; goto _LL3686; _LL3686: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3696)){({ struct Cyc_Std_String_pa_struct _temp3699; _temp3699.tag= Cyc_Std_String_pa;
_temp3699.f1=( struct _tagged_arr)* _temp3696;{ void* _temp3698[ 1u]={&
_temp3699}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3698, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3696, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3700=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3700->hd=( void*)
_temp3696; _temp3700->tl= prev_fields; _temp3700;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3692); if( ! Cyc_Tcutil_bits_only(
_temp3692)){({ struct Cyc_Std_String_pa_struct _temp3702; _temp3702.tag= Cyc_Std_String_pa;
_temp3702.f1=( struct _tagged_arr)* _temp3696;{ void* _temp3701[ 1u]={&
_temp3702}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3701, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3692, _temp3690, _temp3696); Cyc_Tcutil_check_field_atts(
loc, _temp3696, _temp3688);}}; _pop_region( uprev_rgn);} goto _LL3168; _LL3202:
if( _temp3297 ==  0){ if(* _temp3294 ==  0){({ void* _temp3703[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3703, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp3294));;}} if(*
_temp3294 ==  0){ struct _handler_cons _temp3704; _push_handler(& _temp3704);{
int _temp3706= 0; if( setjmp( _temp3704.handler)){ _temp3706= 1;} if( !
_temp3706){* _temp3294=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp3297));; _pop_handler();} else{
void* _temp3705=( void*) _exn_thrown; void* _temp3708= _temp3705; _LL3710: if(
_temp3708 ==  Cyc_Dict_Absent){ goto _LL3711;} else{ goto _LL3712;} _LL3712:
goto _LL3713; _LL3711: { struct _tuple1* tdn=( struct _tuple1*) _check_null(
_temp3297); struct Cyc_Tcenv_Genv* _temp3714=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Structdecl* _temp3715=({ struct Cyc_Absyn_Structdecl* _temp3718=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp3718->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3718->name=({ struct Cyc_Core_Opt*
_temp3719=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3719->v=( void*) tdn; _temp3719;}); _temp3718->tvs= 0; _temp3718->fields= 0;
_temp3718->attributes= 0; _temp3718;}); Cyc_Tc_tcStructdecl( te, _temp3714, loc,
_temp3715);* _temp3294=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc, tdn); if( _temp3295 !=  0){({ struct Cyc_Std_String_pa_struct _temp3717;
_temp3717.tag= Cyc_Std_String_pa; _temp3717.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tdn);{ void* _temp3716[ 1u]={& _temp3717}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized struct %s before using",
sizeof( unsigned char), 52u), _tag_arr( _temp3716, sizeof( void*), 1u));}});
return cvtenv;} goto _LL3709;} _LL3713:( void) _throw( _temp3708); _LL3709:;}}}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp3294)); if( sd->name !=  0){*(( struct _tuple1*) _check_null( _temp3297))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp3295); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3723; _temp3723.tag= Cyc_Std_Int_pa; _temp3723.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3722; _temp3722.tag= Cyc_Std_Int_pa;
_temp3722.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3721; _temp3721.tag= Cyc_Std_String_pa; _temp3721.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp3297));{ void*
_temp3720[ 3u]={& _temp3721,& _temp3722,& _temp3723}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3720, sizeof( void*), 3u));}}}});} for( 0;
_temp3295 !=  0;( _temp3295= _temp3295->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3295->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL3168;}} _LL3204: if( _temp3304 ==  0){({ void*
_temp3724[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("missing union name", sizeof(
unsigned char), 19u), _tag_arr( _temp3724, sizeof( void*), 0u));}); return
cvtenv;} if(* _temp3301 ==  0){ struct _handler_cons _temp3725; _push_handler(&
_temp3725);{ int _temp3727= 0; if( setjmp( _temp3725.handler)){ _temp3727= 1;}
if( ! _temp3727){* _temp3301=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp3304));; _pop_handler();} else{
void* _temp3726=( void*) _exn_thrown; void* _temp3729= _temp3726; _LL3731: if(
_temp3729 ==  Cyc_Dict_Absent){ goto _LL3732;} else{ goto _LL3733;} _LL3733:
goto _LL3734; _LL3732: { struct _tuple1* _temp3735=( struct _tuple1*)
_check_null( _temp3304); struct Cyc_Tcenv_Genv* _temp3736=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Uniondecl* _temp3737=({ struct Cyc_Absyn_Uniondecl* _temp3740=(
struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp3740->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3740->name=({ struct Cyc_Core_Opt*
_temp3741=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3741->v=( void*) _temp3735; _temp3741;}); _temp3740->tvs= 0; _temp3740->fields=
0; _temp3740->attributes= 0; _temp3740;}); Cyc_Tc_tcUniondecl( te, _temp3736,
loc, _temp3737);* _temp3301=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc, _temp3735); if( _temp3302 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3739; _temp3739.tag= Cyc_Std_String_pa; _temp3739.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3735);{ void* _temp3738[ 1u]={& _temp3739}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized union %s before using", sizeof(
unsigned char), 51u), _tag_arr( _temp3738, sizeof( void*), 1u));}}); return
cvtenv;} goto _LL3730;} _LL3734:( void) _throw( _temp3729); _LL3730:;}}}{ struct
Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp3301)); if( ud->name !=  0){* _temp3304=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp3302); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3745; _temp3745.tag= Cyc_Std_Int_pa; _temp3745.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3744; _temp3744.tag= Cyc_Std_Int_pa;
_temp3744.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3743; _temp3743.tag= Cyc_Std_String_pa; _temp3743.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp3304));{ void*
_temp3742[ 3u]={& _temp3743,& _temp3744,& _temp3745}; Cyc_Tcutil_terr( loc,
_tag_arr("union %s expects %d type arguments but was given %d", sizeof(
unsigned char), 52u), _tag_arr( _temp3742, sizeof( void*), 3u));}}}});} for( 0;
_temp3302 !=  0;( _temp3302= _temp3302->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3302->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL3168;}} _LL3206: { struct Cyc_List_List*
targs=* _temp3311; struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons
_temp3746; _push_handler(& _temp3746);{ int _temp3748= 0; if( setjmp( _temp3746.handler)){
_temp3748= 1;} if( ! _temp3748){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp3312);; _pop_handler();} else{ void* _temp3747=( void*) _exn_thrown; void*
_temp3750= _temp3747; _LL3752: if( _temp3750 ==  Cyc_Dict_Absent){ goto _LL3753;}
else{ goto _LL3754;} _LL3754: goto _LL3755; _LL3753:({ struct Cyc_Std_String_pa_struct
_temp3757; _temp3757.tag= Cyc_Std_String_pa; _temp3757.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3312);{ void* _temp3756[ 1u]={& _temp3757}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3756, sizeof( void*), 1u));}}); return cvtenv; _LL3755:( void) _throw(
_temp3750); _LL3751:;}}} _temp3312[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts= ts->tl, tvs= tvs->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp3758=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3758->hd=(
void*)({ struct _tuple5* _temp3759=( struct _tuple5*) _cycalloc( sizeof( struct
_tuple5)); _temp3759->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3759->f2=( void*)
ts->hd; _temp3759;}); _temp3758->tl= inst; _temp3758;});} if( ts !=  0){({
struct Cyc_Std_String_pa_struct _temp3761; _temp3761.tag= Cyc_Std_String_pa;
_temp3761.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3312);{ void*
_temp3760[ 1u]={& _temp3761}; Cyc_Tcutil_terr( loc, _tag_arr("too many parameters for typedef %s",
sizeof( unsigned char), 35u), _tag_arr( _temp3760, sizeof( void*), 1u));}});}
if( tvs !=  0){ struct Cyc_List_List* hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*) tvs->hd)->kind); void*
e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({ struct Cyc_List_List* _temp3762=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3762->hd=(
void*) e; _temp3762->tl= hidden_ts; _temp3762;}); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k, e); inst=({ struct Cyc_List_List* _temp3763=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3763->hd=( void*)({ struct
_tuple5* _temp3764=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3764->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3764->f2= e; _temp3764;});
_temp3763->tl= inst; _temp3763;});}* _temp3311= Cyc_List_imp_append( targs, Cyc_List_imp_rev(
hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);*
_temp3308=({ struct Cyc_Core_Opt* _temp3765=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3765->v=( void*) new_typ; _temp3765;}); goto
_LL3168;}}} _LL3208: goto _LL3168; _LL3210: _temp3316= _temp3314; goto _LL3212;
_LL3212: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3316); goto _LL3168; _LL3214: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3318); goto _LL3168; _LL3216: for( 0;
_temp3320 !=  0; _temp3320= _temp3320->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*) _temp3320->hd);} goto
_LL3168; _LL3168:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){({ struct Cyc_Std_String_pa_struct _temp3769; _temp3769.tag= Cyc_Std_String_pa;
_temp3769.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_Std_String_pa_struct _temp3768; _temp3768.tag= Cyc_Std_String_pa;
_temp3768.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_Std_String_pa_struct _temp3767; _temp3767.tag= Cyc_Std_String_pa;
_temp3767.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3766[
3u]={& _temp3767,& _temp3768,& _temp3769}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3766, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3770= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3785; _temp3785.kind_env= kind_env; _temp3785.free_vars= 0; _temp3785.free_evars=
0; _temp3785.generalize_evars= generalize_evars; _temp3785.fn_result= 0;
_temp3785;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3770.free_vars;
for( 0; vs !=  0; vs= vs->tl){ _temp3770.kind_env= Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*) vs->hd);}}{ struct Cyc_List_List* evs=
_temp3770.free_evars; for( 0; evs !=  0; evs= evs->tl){ void* _temp3771= Cyc_Tcutil_compress((
void*) evs->hd); struct Cyc_Core_Opt* _temp3777; struct Cyc_Core_Opt** _temp3779;
_LL3773: if(( unsigned int) _temp3771 >  4u?*(( int*) _temp3771) ==  Cyc_Absyn_Evar:
0){ _LL3778: _temp3777=(( struct Cyc_Absyn_Evar_struct*) _temp3771)->f4;
_temp3779=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3771)->f4;
goto _LL3774;} else{ goto _LL3775;} _LL3775: goto _LL3776; _LL3774: if(*
_temp3779 ==  0){* _temp3779=({ struct Cyc_Core_Opt* _temp3780=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3780->v=( void*) kind_env;
_temp3780;});} else{ struct Cyc_List_List* _temp3781=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3779))->v; struct Cyc_List_List*
_temp3782= 0; for( 0; _temp3781 !=  0; _temp3781= _temp3781->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, kind_env,(
struct Cyc_Absyn_Tvar*) _temp3781->hd)){ _temp3782=({ struct Cyc_List_List*
_temp3783=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3783->hd=( void*)(( struct Cyc_Absyn_Tvar*) _temp3781->hd); _temp3783->tl=
_temp3782; _temp3783;});}}* _temp3779=({ struct Cyc_Core_Opt* _temp3784=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3784->v=( void*)
_temp3782; _temp3784;});} goto _LL3772; _LL3776: goto _LL3772; _LL3772:;}}
return _temp3770;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ int generalize_evars= Cyc_Tcutil_is_function_type(
t); struct Cyc_Tcutil_CVTEnv _temp3786= Cyc_Tcutil_check_valid_type( loc, te, 0,(
void*) Cyc_Absyn_MemKind, generalize_evars, t); struct Cyc_List_List* _temp3787=
_temp3786.free_vars; struct Cyc_List_List* _temp3788= _temp3786.free_evars;{
struct Cyc_List_List* x= _temp3787; for( 0; x !=  0; x= x->tl){ struct Cyc_Absyn_Conref*
c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*) x->hd)->kind); void*
_temp3789=( void*) c->v; void* _temp3797; _LL3791: if( _temp3789 == ( void*) Cyc_Absyn_No_constr){
goto _LL3792;} else{ goto _LL3793;} _LL3793: if(( unsigned int) _temp3789 >  1u?*((
int*) _temp3789) ==  Cyc_Absyn_Eq_constr: 0){ _LL3798: _temp3797=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3789)->f1; if( _temp3797 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3794;} else{ goto _LL3795;}} else{ goto _LL3795;}
_LL3795: goto _LL3796; _LL3792: goto _LL3794; _LL3794:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3799=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3799[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3800; _temp3800.tag= Cyc_Absyn_Eq_constr;
_temp3800.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3800;}); _temp3799;})));
goto _LL3790; _LL3796: goto _LL3790; _LL3790:;}} if( _temp3787 !=  0? 1:
_temp3788 !=  0){{ void* _temp3801= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3807; struct Cyc_List_List* _temp3809; struct Cyc_List_List* _temp3811;
struct Cyc_Absyn_VarargInfo* _temp3813; int _temp3815; struct Cyc_List_List*
_temp3817; void* _temp3819; struct Cyc_Core_Opt* _temp3821; struct Cyc_List_List*
_temp3823; struct Cyc_List_List** _temp3825; _LL3803: if(( unsigned int)
_temp3801 >  4u?*(( int*) _temp3801) ==  Cyc_Absyn_FnType: 0){ _LL3808:
_temp3807=(( struct Cyc_Absyn_FnType_struct*) _temp3801)->f1; _LL3824: _temp3823=
_temp3807.tvars; _temp3825=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3801)->f1).tvars; goto _LL3822; _LL3822: _temp3821= _temp3807.effect; goto
_LL3820; _LL3820: _temp3819=( void*) _temp3807.ret_typ; goto _LL3818; _LL3818:
_temp3817= _temp3807.args; goto _LL3816; _LL3816: _temp3815= _temp3807.c_varargs;
goto _LL3814; _LL3814: _temp3813= _temp3807.cyc_varargs; goto _LL3812; _LL3812:
_temp3811= _temp3807.rgn_po; goto _LL3810; _LL3810: _temp3809= _temp3807.attributes;
goto _LL3804;} else{ goto _LL3805;} _LL3805: goto _LL3806; _LL3804: if(*
_temp3825 ==  0){* _temp3825= _temp3787; _temp3787= 0;} goto _LL3802; _LL3806:
goto _LL3802; _LL3802:;} if( _temp3787 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3827; _temp3827.tag= Cyc_Std_String_pa; _temp3827.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*) _temp3787->hd)->name;{ void* _temp3826[ 1u]={& _temp3827};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s", sizeof( unsigned char),
25u), _tag_arr( _temp3826, sizeof( void*), 1u));}});} if( _temp3788 !=  0){ for(
0; _temp3788 !=  0; _temp3788= _temp3788->tl){ void* e=( void*) _temp3788->hd;
void* _temp3828= Cyc_Tcutil_typ_kind( e); _LL3830: if( _temp3828 == ( void*) Cyc_Absyn_RgnKind){
goto _LL3831;} else{ goto _LL3832;} _LL3832: if( _temp3828 == ( void*) Cyc_Absyn_EffKind){
goto _LL3833;} else{ goto _LL3834;} _LL3834: goto _LL3835; _LL3831: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3836[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3836, sizeof( void*), 0u));});}
goto _LL3829; _LL3833: if( ! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({
void* _temp3837[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!", sizeof( unsigned char),
26u), _tag_arr( _temp3837, sizeof( void*), 0u));});} goto _LL3829; _LL3835:({
struct Cyc_Std_String_pa_struct _temp3840; _temp3840.tag= Cyc_Std_String_pa;
_temp3840.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp3839; _temp3839.tag= Cyc_Std_String_pa; _temp3839.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( e);{ void* _temp3838[ 2u]={& _temp3839,& _temp3840}; Cyc_Tcutil_terr(
loc, _tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3838, sizeof( void*), 2u));}}}); goto
_LL3829; _LL3829:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3841= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3847; struct Cyc_List_List* _temp3849; struct
Cyc_List_List* _temp3851; struct Cyc_Absyn_VarargInfo* _temp3853; int _temp3855;
struct Cyc_List_List* _temp3857; void* _temp3859; struct Cyc_Core_Opt* _temp3861;
struct Cyc_List_List* _temp3863; _LL3843: if(( unsigned int) _temp3841 >  4u?*((
int*) _temp3841) ==  Cyc_Absyn_FnType: 0){ _LL3848: _temp3847=(( struct Cyc_Absyn_FnType_struct*)
_temp3841)->f1; _LL3864: _temp3863= _temp3847.tvars; goto _LL3862; _LL3862:
_temp3861= _temp3847.effect; goto _LL3860; _LL3860: _temp3859=( void*) _temp3847.ret_typ;
goto _LL3858; _LL3858: _temp3857= _temp3847.args; goto _LL3856; _LL3856:
_temp3855= _temp3847.c_varargs; goto _LL3854; _LL3854: _temp3853= _temp3847.cyc_varargs;
goto _LL3852; _LL3852: _temp3851= _temp3847.rgn_po; goto _LL3850; _LL3850:
_temp3849= _temp3847.attributes; goto _LL3844;} else{ goto _LL3845;} _LL3845:
goto _LL3846; _LL3844: fd->tvs= _temp3863; fd->effect= _temp3861; goto _LL3842;
_LL3846:({ void* _temp3865[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3865, sizeof( void*), 0u));});
return; _LL3842:;}{ struct _RegionHandle _temp3866= _new_region(); struct
_RegionHandle* r=& _temp3866; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3867=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3867->v=(
void*) t; _temp3867;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3868=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3869= Cyc_Tcutil_remove_bound_tvars( _temp3868.free_vars,
bound_tvars); struct Cyc_List_List* _temp3870= _temp3868.free_evars;{ struct Cyc_List_List*
fs= _temp3869; for( 0; fs !=  0; fs= fs->tl){ struct _tagged_arr* _temp3871=((
struct Cyc_Absyn_Tvar*) fs->hd)->name;({ struct Cyc_Std_String_pa_struct
_temp3874; _temp3874.tag= Cyc_Std_String_pa; _temp3874.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3873;
_temp3873.tag= Cyc_Std_String_pa; _temp3873.f1=( struct _tagged_arr)* _temp3871;{
void* _temp3872[ 2u]={& _temp3873,& _temp3874}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3872, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3870 !=  0: 0){ for( 0; _temp3870 !=  0; _temp3870=
_temp3870->tl){ void* e=( void*) _temp3870->hd; void* _temp3875= Cyc_Tcutil_typ_kind(
e); _LL3877: if( _temp3875 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3878;} else{
goto _LL3879;} _LL3879: if( _temp3875 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3880;} else{ goto _LL3881;} _LL3881: goto _LL3882; _LL3878: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3883[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3883, sizeof( void*), 0u));});}
goto _LL3876; _LL3880: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3884=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3884[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3885; _temp3885.tag= Cyc_Absyn_JoinEff;
_temp3885.f1= 0; _temp3885;}); _temp3884;}))){({ void* _temp3886[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3886, sizeof( void*), 0u));});}
goto _LL3876; _LL3882:({ struct Cyc_Std_String_pa_struct _temp3889; _temp3889.tag=
Cyc_Std_String_pa; _temp3889.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3888; _temp3888.tag= Cyc_Std_String_pa;
_temp3888.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3887[
2u]={& _temp3888,& _temp3889}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3887, sizeof( void*), 2u));}}});
goto _LL3876; _LL3876:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs= vs->tl){
struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 !=  0; vs2= vs2->tl){ if( cmp((
void*) vs->hd,( void*) vs2->hd) ==  0){({ struct Cyc_Std_String_pa_struct
_temp3892; _temp3892.tag= Cyc_Std_String_pa; _temp3892.f1=( struct _tagged_arr)
a2string(( void*) vs->hd);{ struct Cyc_Std_String_pa_struct _temp3891; _temp3891.tag=
Cyc_Std_String_pa; _temp3891.f1=( struct _tagged_arr) msg;{ void* _temp3890[ 2u]={&
_temp3891,& _temp3892}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3890, sizeof( void*), 2u));}}});}}}} static
struct _tagged_arr Cyc_Tcutil_strptr2string( struct _tagged_arr* s){ return* s;}
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){(( void(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( struct _tagged_arr*), struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)( Cyc_Std_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string,
msg);} void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs){(( void(*)( int(* cmp)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( struct Cyc_Absyn_Tvar*), struct _tagged_arr
msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp, tvs, loc, Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable", sizeof( unsigned char), 24u));} struct
_tuple18{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple20{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields !=  0; sd_fields=
sd_fields->tl){ if( Cyc_Std_strcmp(*(( struct Cyc_Absyn_Structfield*) sd_fields->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List*
_temp3893=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3893->hd=( void*)({ struct _tuple18* _temp3894=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp3894->f1=( struct Cyc_Absyn_Structfield*)
sd_fields->hd; _temp3894->f2= 0; _temp3894;}); _temp3893->tl= fields; _temp3893;});}}}
fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0; des= des->tl){ struct
_tuple19 _temp3897; void* _temp3898; struct Cyc_List_List* _temp3900; struct
_tuple19* _temp3895=( struct _tuple19*) des->hd; _temp3897=* _temp3895; _LL3901:
_temp3900= _temp3897.f1; goto _LL3899; _LL3899: _temp3898= _temp3897.f2; goto
_LL3896; _LL3896: if( _temp3900 ==  0){ struct Cyc_List_List* _temp3902= fields;
for( 0; _temp3902 !=  0; _temp3902= _temp3902->tl){ if( !(*(( struct _tuple18*)
_temp3902->hd)).f2){(*(( struct _tuple18*) _temp3902->hd)).f2= 1;(*(( struct
_tuple19*) des->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3903=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3903->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3904=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3904[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3905; _temp3905.tag= Cyc_Absyn_FieldName;
_temp3905.f1=((*(( struct _tuple18*) _temp3902->hd)).f1)->name; _temp3905;});
_temp3904;})); _temp3903->tl= 0; _temp3903;}); ans=({ struct Cyc_List_List*
_temp3906=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3906->hd=( void*)({ struct _tuple20* _temp3907=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp3907->f1=(*(( struct
_tuple18*) _temp3902->hd)).f1; _temp3907->f2= _temp3898; _temp3907;}); _temp3906->tl=
ans; _temp3906;}); break;}} if( _temp3902 ==  0){({ void* _temp3908[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u),
_tag_arr( _temp3908, sizeof( void*), 0u));});}} else{ if( _temp3900->tl !=  0){({
void* _temp3909[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3909, sizeof( void*), 0u));});}
else{ void* _temp3910=( void*) _temp3900->hd; struct _tagged_arr* _temp3916;
_LL3912: if(*(( int*) _temp3910) ==  Cyc_Absyn_ArrayElement){ goto _LL3913;}
else{ goto _LL3914;} _LL3914: if(*(( int*) _temp3910) ==  Cyc_Absyn_FieldName){
_LL3917: _temp3916=(( struct Cyc_Absyn_FieldName_struct*) _temp3910)->f1; goto
_LL3915;} else{ goto _LL3911;} _LL3913:({ void* _temp3918[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3918, sizeof( void*), 0u));}); goto _LL3911;
_LL3915: { struct Cyc_List_List* _temp3919= fields; for( 0; _temp3919 !=  0;
_temp3919= _temp3919->tl){ if( Cyc_Std_zstrptrcmp( _temp3916,((*(( struct
_tuple18*) _temp3919->hd)).f1)->name) ==  0){ if((*(( struct _tuple18*)
_temp3919->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3921; _temp3921.tag=
Cyc_Std_String_pa; _temp3921.f1=( struct _tagged_arr)* _temp3916;{ void*
_temp3920[ 1u]={& _temp3921}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3920, sizeof( void*), 1u));}});}(*((
struct _tuple18*) _temp3919->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp3922=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3922->hd=( void*)({ struct _tuple20* _temp3923=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp3923->f1=(*(( struct
_tuple18*) _temp3919->hd)).f1; _temp3923->f2= _temp3898; _temp3923;}); _temp3922->tl=
ans; _temp3922;}); break;}} if( _temp3919 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3925; _temp3925.tag= Cyc_Std_String_pa; _temp3925.f1=( struct _tagged_arr)*
_temp3916;{ void* _temp3924[ 1u]={& _temp3925}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3924, sizeof( void*), 1u));}});}
goto _LL3911;} _LL3911:;}}} for( 0; fields !=  0; fields= fields->tl){ if( !(*((
struct _tuple18*) fields->hd)).f2){({ void* _temp3926[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments to struct", sizeof( unsigned char), 28u),
_tag_arr( _temp3926, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3927= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3933; struct Cyc_Absyn_Conref* _temp3935; void* _temp3937;
_LL3929: if(( unsigned int) _temp3927 >  4u?*(( int*) _temp3927) ==  Cyc_Absyn_PointerType:
0){ _LL3934: _temp3933=(( struct Cyc_Absyn_PointerType_struct*) _temp3927)->f1;
_LL3938: _temp3937=( void*) _temp3933.elt_typ; goto _LL3936; _LL3936: _temp3935=
_temp3933.bounds; goto _LL3930;} else{ goto _LL3931;} _LL3931: goto _LL3932;
_LL3930: { struct Cyc_Absyn_Conref* _temp3939= Cyc_Absyn_compress_conref(
_temp3935); void* _temp3940=( void*)( Cyc_Absyn_compress_conref( _temp3939))->v;
void* _temp3948; _LL3942: if(( unsigned int) _temp3940 >  1u?*(( int*) _temp3940)
==  Cyc_Absyn_Eq_constr: 0){ _LL3949: _temp3948=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3940)->f1; if( _temp3948 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3943;}
else{ goto _LL3944;}} else{ goto _LL3944;} _LL3944: if( _temp3940 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3945;} else{ goto _LL3946;} _LL3946: goto _LL3947;
_LL3943:* elt_typ_dest= _temp3937; return 1; _LL3945:( void*)( _temp3939->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3950=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3950[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3951; _temp3951.tag= Cyc_Absyn_Eq_constr;
_temp3951.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3951;}); _temp3950;})));*
elt_typ_dest= _temp3937; return 1; _LL3947: return 0; _LL3941:;} _LL3932: return
0; _LL3928:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp4134;
_temp4134.f1= 0; _temp4134.f2=( void*) Cyc_Absyn_HeapRgn; _temp4134;}); void*
_temp3952=( void*) e->r; void* _temp3966; struct _tuple1* _temp3968; struct
_tagged_arr* _temp3970; struct Cyc_Absyn_Exp* _temp3972; struct _tagged_arr*
_temp3974; struct Cyc_Absyn_Exp* _temp3976; struct Cyc_Absyn_Exp* _temp3978;
struct Cyc_Absyn_Exp* _temp3980; struct Cyc_Absyn_Exp* _temp3982; _LL3954: if(*((
int*) _temp3952) ==  Cyc_Absyn_Var_e){ _LL3969: _temp3968=(( struct Cyc_Absyn_Var_e_struct*)
_temp3952)->f1; goto _LL3967; _LL3967: _temp3966=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3952)->f2; goto _LL3955;} else{ goto _LL3956;} _LL3956: if(*(( int*)
_temp3952) ==  Cyc_Absyn_StructMember_e){ _LL3973: _temp3972=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3952)->f1; goto _LL3971; _LL3971: _temp3970=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3952)->f2; goto _LL3957;} else{ goto _LL3958;} _LL3958: if(*(( int*)
_temp3952) ==  Cyc_Absyn_StructArrow_e){ _LL3977: _temp3976=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3952)->f1; goto _LL3975; _LL3975: _temp3974=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3952)->f2; goto _LL3959;} else{ goto _LL3960;} _LL3960: if(*(( int*)
_temp3952) ==  Cyc_Absyn_Deref_e){ _LL3979: _temp3978=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3952)->f1; goto _LL3961;} else{ goto _LL3962;} _LL3962: if(*(( int*)
_temp3952) ==  Cyc_Absyn_Subscript_e){ _LL3983: _temp3982=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3952)->f1; goto _LL3981; _LL3981: _temp3980=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3952)->f2; goto _LL3963;} else{ goto _LL3964;} _LL3964: goto _LL3965;
_LL3955: { void* _temp3984= _temp3966; struct Cyc_Absyn_Vardecl* _temp3998;
struct Cyc_Absyn_Vardecl* _temp4000; struct Cyc_Absyn_Vardecl* _temp4002; struct
Cyc_Absyn_Vardecl* _temp4004; _LL3986: if( _temp3984 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3987;} else{ goto _LL3988;} _LL3988: if(( unsigned int) _temp3984 >  1u?*((
int*) _temp3984) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3989;} else{ goto _LL3990;}
_LL3990: if(( unsigned int) _temp3984 >  1u?*(( int*) _temp3984) ==  Cyc_Absyn_Global_b:
0){ _LL3999: _temp3998=(( struct Cyc_Absyn_Global_b_struct*) _temp3984)->f1;
goto _LL3991;} else{ goto _LL3992;} _LL3992: if(( unsigned int) _temp3984 >  1u?*((
int*) _temp3984) ==  Cyc_Absyn_Local_b: 0){ _LL4001: _temp4000=(( struct Cyc_Absyn_Local_b_struct*)
_temp3984)->f1; goto _LL3993;} else{ goto _LL3994;} _LL3994: if(( unsigned int)
_temp3984 >  1u?*(( int*) _temp3984) ==  Cyc_Absyn_Pat_b: 0){ _LL4003: _temp4002=((
struct Cyc_Absyn_Pat_b_struct*) _temp3984)->f1; goto _LL3995;} else{ goto
_LL3996;} _LL3996: if(( unsigned int) _temp3984 >  1u?*(( int*) _temp3984) == 
Cyc_Absyn_Param_b: 0){ _LL4005: _temp4004=(( struct Cyc_Absyn_Param_b_struct*)
_temp3984)->f1; goto _LL3997;} else{ goto _LL3985;} _LL3987: return bogus_ans;
_LL3989: return({ struct _tuple7 _temp4006; _temp4006.f1= 0; _temp4006.f2=( void*)
Cyc_Absyn_HeapRgn; _temp4006;}); _LL3991: { void* _temp4007= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL4009: if((
unsigned int) _temp4007 >  4u?*(( int*) _temp4007) ==  Cyc_Absyn_ArrayType: 0){
goto _LL4010;} else{ goto _LL4011;} _LL4011: goto _LL4012; _LL4010: return({
struct _tuple7 _temp4013; _temp4013.f1= 1; _temp4013.f2=( void*) Cyc_Absyn_HeapRgn;
_temp4013;}); _LL4012: return({ struct _tuple7 _temp4014; _temp4014.f1=(
_temp3998->tq).q_const; _temp4014.f2=( void*) Cyc_Absyn_HeapRgn; _temp4014;});
_LL4008:;} _LL3993: { void* _temp4015= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL4017: if(( unsigned int) _temp4015 >  4u?*(( int*)
_temp4015) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4018;} else{ goto _LL4019;}
_LL4019: goto _LL4020; _LL4018: return({ struct _tuple7 _temp4021; _temp4021.f1=
1; _temp4021.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp4000->rgn))->v;
_temp4021;}); _LL4020: return({ struct _tuple7 _temp4022; _temp4022.f1=(
_temp4000->tq).q_const; _temp4022.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4000->rgn))->v; _temp4022;}); _LL4016:;} _LL3995: _temp4004=
_temp4002; goto _LL3997; _LL3997: return({ struct _tuple7 _temp4023; _temp4023.f1=(
_temp4004->tq).q_const; _temp4023.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4004->rgn))->v; _temp4023;}); _LL3985:;} _LL3957: { void*
_temp4024= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3972->topt))->v); struct Cyc_List_List* _temp4036; struct Cyc_List_List*
_temp4038; struct Cyc_Absyn_Structdecl** _temp4040; struct Cyc_Absyn_Structdecl*
_temp4042; struct Cyc_Absyn_Uniondecl** _temp4043; struct Cyc_Absyn_Uniondecl*
_temp4045; _LL4026: if(( unsigned int) _temp4024 >  4u?*(( int*) _temp4024) == 
Cyc_Absyn_AnonStructType: 0){ _LL4037: _temp4036=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp4024)->f1; goto _LL4027;} else{ goto _LL4028;} _LL4028: if(( unsigned int)
_temp4024 >  4u?*(( int*) _temp4024) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4039:
_temp4038=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp4024)->f1; goto
_LL4029;} else{ goto _LL4030;} _LL4030: if(( unsigned int) _temp4024 >  4u?*((
int*) _temp4024) ==  Cyc_Absyn_StructType: 0){ _LL4041: _temp4040=(( struct Cyc_Absyn_StructType_struct*)
_temp4024)->f3; if( _temp4040 ==  0){ goto _LL4032;} else{ _temp4042=* _temp4040;
goto _LL4031;}} else{ goto _LL4032;} _LL4032: if(( unsigned int) _temp4024 >  4u?*((
int*) _temp4024) ==  Cyc_Absyn_UnionType: 0){ _LL4044: _temp4043=(( struct Cyc_Absyn_UnionType_struct*)
_temp4024)->f3; if( _temp4043 ==  0){ goto _LL4034;} else{ _temp4045=* _temp4043;
goto _LL4033;}} else{ goto _LL4034;} _LL4034: goto _LL4035; _LL4027: _temp4038=
_temp4036; goto _LL4029; _LL4029: { struct Cyc_Absyn_Structfield* _temp4046= Cyc_Absyn_lookup_field(
_temp4038, _temp3970); if( _temp4046 !=  0? _temp4046->width !=  0: 0){ return({
struct _tuple7 _temp4047; _temp4047.f1=( _temp4046->tq).q_const; _temp4047.f2=(
Cyc_Tcutil_addressof_props( te, _temp3972)).f2; _temp4047;});} return bogus_ans;}
_LL4031: { struct Cyc_Absyn_Structfield* _temp4048= Cyc_Absyn_lookup_struct_field(
_temp4042, _temp3970); if( _temp4048 !=  0? _temp4048->width !=  0: 0){ return({
struct _tuple7 _temp4049; _temp4049.f1=( _temp4048->tq).q_const; _temp4049.f2=(
Cyc_Tcutil_addressof_props( te, _temp3972)).f2; _temp4049;});} return bogus_ans;}
_LL4033: { struct Cyc_Absyn_Structfield* _temp4050= Cyc_Absyn_lookup_union_field(
_temp4045, _temp3970); if( _temp4050 !=  0){ return({ struct _tuple7 _temp4051;
_temp4051.f1=( _temp4050->tq).q_const; _temp4051.f2=( Cyc_Tcutil_addressof_props(
te, _temp3972)).f2; _temp4051;});} goto _LL4035;} _LL4035: return bogus_ans;
_LL4025:;} _LL3959: { void* _temp4052= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3976->topt))->v); struct Cyc_Absyn_PtrInfo _temp4058; void*
_temp4060; void* _temp4062; _LL4054: if(( unsigned int) _temp4052 >  4u?*(( int*)
_temp4052) ==  Cyc_Absyn_PointerType: 0){ _LL4059: _temp4058=(( struct Cyc_Absyn_PointerType_struct*)
_temp4052)->f1; _LL4063: _temp4062=( void*) _temp4058.elt_typ; goto _LL4061;
_LL4061: _temp4060=( void*) _temp4058.rgn_typ; goto _LL4055;} else{ goto _LL4056;}
_LL4056: goto _LL4057; _LL4055: { void* _temp4064= Cyc_Tcutil_compress(
_temp4062); struct Cyc_List_List* _temp4076; struct Cyc_List_List* _temp4078;
struct Cyc_Absyn_Structdecl** _temp4080; struct Cyc_Absyn_Structdecl* _temp4082;
struct Cyc_Absyn_Uniondecl** _temp4083; struct Cyc_Absyn_Uniondecl* _temp4085;
_LL4066: if(( unsigned int) _temp4064 >  4u?*(( int*) _temp4064) ==  Cyc_Absyn_AnonStructType:
0){ _LL4077: _temp4076=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4064)->f1;
goto _LL4067;} else{ goto _LL4068;} _LL4068: if(( unsigned int) _temp4064 >  4u?*((
int*) _temp4064) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4079: _temp4078=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4064)->f1; goto _LL4069;} else{ goto
_LL4070;} _LL4070: if(( unsigned int) _temp4064 >  4u?*(( int*) _temp4064) == 
Cyc_Absyn_StructType: 0){ _LL4081: _temp4080=(( struct Cyc_Absyn_StructType_struct*)
_temp4064)->f3; if( _temp4080 ==  0){ goto _LL4072;} else{ _temp4082=* _temp4080;
goto _LL4071;}} else{ goto _LL4072;} _LL4072: if(( unsigned int) _temp4064 >  4u?*((
int*) _temp4064) ==  Cyc_Absyn_UnionType: 0){ _LL4084: _temp4083=(( struct Cyc_Absyn_UnionType_struct*)
_temp4064)->f3; if( _temp4083 ==  0){ goto _LL4074;} else{ _temp4085=* _temp4083;
goto _LL4073;}} else{ goto _LL4074;} _LL4074: goto _LL4075; _LL4067: _temp4078=
_temp4076; goto _LL4069; _LL4069: { struct Cyc_Absyn_Structfield* _temp4086= Cyc_Absyn_lookup_field(
_temp4078, _temp3974); if( _temp4086 !=  0? _temp4086->width !=  0: 0){ return({
struct _tuple7 _temp4087; _temp4087.f1=( _temp4086->tq).q_const; _temp4087.f2=
_temp4060; _temp4087;});} return bogus_ans;} _LL4071: { struct Cyc_Absyn_Structfield*
_temp4088= Cyc_Absyn_lookup_struct_field( _temp4082, _temp3974); if( _temp4088
!=  0? _temp4088->width !=  0: 0){ return({ struct _tuple7 _temp4089; _temp4089.f1=(
_temp4088->tq).q_const; _temp4089.f2= _temp4060; _temp4089;});} return bogus_ans;}
_LL4073: { struct Cyc_Absyn_Structfield* _temp4090= Cyc_Absyn_lookup_union_field(
_temp4085, _temp3974); if( _temp4090 !=  0){ return({ struct _tuple7 _temp4091;
_temp4091.f1=( _temp4090->tq).q_const; _temp4091.f2= _temp4060; _temp4091;});}
return bogus_ans;} _LL4075: return bogus_ans; _LL4065:;} _LL4057: return
bogus_ans; _LL4053:;} _LL3961: { void* _temp4092= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3978->topt))->v); struct Cyc_Absyn_PtrInfo
_temp4098; struct Cyc_Absyn_Tqual _temp4100; void* _temp4102; _LL4094: if((
unsigned int) _temp4092 >  4u?*(( int*) _temp4092) ==  Cyc_Absyn_PointerType: 0){
_LL4099: _temp4098=(( struct Cyc_Absyn_PointerType_struct*) _temp4092)->f1;
_LL4103: _temp4102=( void*) _temp4098.rgn_typ; goto _LL4101; _LL4101: _temp4100=
_temp4098.tq; goto _LL4095;} else{ goto _LL4096;} _LL4096: goto _LL4097; _LL4095:
return({ struct _tuple7 _temp4104; _temp4104.f1= _temp4100.q_const; _temp4104.f2=
_temp4102; _temp4104;}); _LL4097: return bogus_ans; _LL4093:;} _LL3963: { void*
t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3982->topt))->v);
void* _temp4105= t; struct Cyc_Absyn_Tqual _temp4115; struct Cyc_List_List*
_temp4117; struct Cyc_Absyn_PtrInfo _temp4119; struct Cyc_Absyn_Conref*
_temp4121; struct Cyc_Absyn_Tqual _temp4123; void* _temp4125; void* _temp4127;
_LL4107: if(( unsigned int) _temp4105 >  4u?*(( int*) _temp4105) ==  Cyc_Absyn_ArrayType:
0){ _LL4116: _temp4115=(( struct Cyc_Absyn_ArrayType_struct*) _temp4105)->f2;
goto _LL4108;} else{ goto _LL4109;} _LL4109: if(( unsigned int) _temp4105 >  4u?*((
int*) _temp4105) ==  Cyc_Absyn_TupleType: 0){ _LL4118: _temp4117=(( struct Cyc_Absyn_TupleType_struct*)
_temp4105)->f1; goto _LL4110;} else{ goto _LL4111;} _LL4111: if(( unsigned int)
_temp4105 >  4u?*(( int*) _temp4105) ==  Cyc_Absyn_PointerType: 0){ _LL4120:
_temp4119=(( struct Cyc_Absyn_PointerType_struct*) _temp4105)->f1; _LL4128:
_temp4127=( void*) _temp4119.elt_typ; goto _LL4126; _LL4126: _temp4125=( void*)
_temp4119.rgn_typ; goto _LL4124; _LL4124: _temp4123= _temp4119.tq; goto _LL4122;
_LL4122: _temp4121= _temp4119.bounds; goto _LL4112;} else{ goto _LL4113;}
_LL4113: goto _LL4114; _LL4108: return({ struct _tuple7 _temp4129; _temp4129.f1=
_temp4115.q_const; _temp4129.f2=( Cyc_Tcutil_addressof_props( te, _temp3982)).f2;
_temp4129;}); _LL4110: { struct _tuple4* _temp4130= Cyc_Absyn_lookup_tuple_field(
_temp4117,( int) Cyc_Evexp_eval_const_uint_exp( _temp3980)); if( _temp4130 !=  0){
return({ struct _tuple7 _temp4131; _temp4131.f1=((* _temp4130).f1).q_const;
_temp4131.f2=( Cyc_Tcutil_addressof_props( te, _temp3982)).f2; _temp4131;});}
return bogus_ans;} _LL4112: return({ struct _tuple7 _temp4132; _temp4132.f1=
_temp4123.q_const; _temp4132.f2= _temp4125; _temp4132;}); _LL4114: return
bogus_ans; _LL4106:;} _LL3965:({ void* _temp4133[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u), _tag_arr(
_temp4133, sizeof( void*), 0u));}); return bogus_ans; _LL3953:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp4135= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Tqual _temp4141; void*
_temp4143; _LL4137: if(( unsigned int) _temp4135 >  4u?*(( int*) _temp4135) == 
Cyc_Absyn_ArrayType: 0){ _LL4144: _temp4143=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp4135)->f1; goto _LL4142; _LL4142: _temp4141=(( struct Cyc_Absyn_ArrayType_struct*)
_temp4135)->f2; goto _LL4138;} else{ goto _LL4139;} _LL4139: goto _LL4140;
_LL4138: { void* _temp4147; struct _tuple7 _temp4145= Cyc_Tcutil_addressof_props(
te, e); _LL4148: _temp4147= _temp4145.f2; goto _LL4146; _LL4146: return Cyc_Absyn_atb_typ(
_temp4143, _temp4147, _temp4141,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp4149=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4149[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4150; _temp4150.tag= Cyc_Absyn_Upper_b;
_temp4150.f1= e; _temp4150;}); _temp4149;}));} _LL4140: return e_typ; _LL4136:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp4151=(
void*) b->v; void* _temp4159; void* _temp4161; struct Cyc_Absyn_Exp* _temp4163;
_LL4153: if(( unsigned int) _temp4151 >  1u?*(( int*) _temp4151) ==  Cyc_Absyn_Eq_constr:
0){ _LL4160: _temp4159=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4151)->f1;
if( _temp4159 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4154;} else{ goto
_LL4155;}} else{ goto _LL4155;} _LL4155: if(( unsigned int) _temp4151 >  1u?*((
int*) _temp4151) ==  Cyc_Absyn_Eq_constr: 0){ _LL4162: _temp4161=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp4151)->f1; if(( unsigned int) _temp4161
>  1u?*(( int*) _temp4161) ==  Cyc_Absyn_Upper_b: 0){ _LL4164: _temp4163=((
struct Cyc_Absyn_Upper_b_struct*) _temp4161)->f1; goto _LL4156;} else{ goto
_LL4157;}} else{ goto _LL4157;} _LL4157: goto _LL4158; _LL4154: return; _LL4156:
if( Cyc_Evexp_eval_const_uint_exp( _temp4163) <=  i){({ void* _temp4165[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp4165, sizeof( void*), 0u));});} return;
_LL4158:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp4166=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp4166[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp4167; _temp4167.tag= Cyc_Absyn_Eq_constr;
_temp4167.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp4168=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4168[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4169; _temp4169.tag= Cyc_Absyn_Upper_b;
_temp4169.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp4169;}); _temp4168;}));
_temp4167;}); _temp4166;}))); return; _LL4152:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp4170=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp4176; struct
Cyc_Absyn_Exp* _temp4178; _LL4172: if(( unsigned int) _temp4170 >  1u?*(( int*)
_temp4170) ==  Cyc_Absyn_Eq_constr: 0){ _LL4177: _temp4176=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4170)->f1; if(( unsigned int) _temp4176 >  1u?*(( int*) _temp4176) ==  Cyc_Absyn_Upper_b:
0){ _LL4179: _temp4178=(( struct Cyc_Absyn_Upper_b_struct*) _temp4176)->f1; goto
_LL4173;} else{ goto _LL4174;}} else{ goto _LL4174;} _LL4174: goto _LL4175;
_LL4173: return Cyc_Evexp_eval_const_uint_exp( _temp4178) ==  1; _LL4175: return
0; _LL4171:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp4180= Cyc_Tcutil_compress(
t); void* _temp4208; struct Cyc_List_List* _temp4210; struct Cyc_Absyn_Structdecl**
_temp4212; struct Cyc_List_List* _temp4214; struct Cyc_Absyn_Uniondecl**
_temp4216; struct Cyc_List_List* _temp4218; struct Cyc_List_List* _temp4220;
struct Cyc_List_List* _temp4222; _LL4182: if( _temp4180 == ( void*) Cyc_Absyn_VoidType){
goto _LL4183;} else{ goto _LL4184;} _LL4184: if(( unsigned int) _temp4180 >  4u?*((
int*) _temp4180) ==  Cyc_Absyn_IntType: 0){ goto _LL4185;} else{ goto _LL4186;}
_LL4186: if( _temp4180 == ( void*) Cyc_Absyn_FloatType){ goto _LL4187;} else{
goto _LL4188;} _LL4188: if( _temp4180 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL4189;} else{ goto _LL4190;} _LL4190: if(( unsigned int) _temp4180 >  4u?*((
int*) _temp4180) ==  Cyc_Absyn_EnumType: 0){ goto _LL4191;} else{ goto _LL4192;}
_LL4192: if(( unsigned int) _temp4180 >  4u?*(( int*) _temp4180) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL4193;} else{ goto _LL4194;} _LL4194: if(( unsigned int) _temp4180 > 
4u?*(( int*) _temp4180) ==  Cyc_Absyn_ArrayType: 0){ _LL4209: _temp4208=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp4180)->f1; goto _LL4195;} else{ goto
_LL4196;} _LL4196: if(( unsigned int) _temp4180 >  4u?*(( int*) _temp4180) == 
Cyc_Absyn_TupleType: 0){ _LL4211: _temp4210=(( struct Cyc_Absyn_TupleType_struct*)
_temp4180)->f1; goto _LL4197;} else{ goto _LL4198;} _LL4198: if(( unsigned int)
_temp4180 >  4u?*(( int*) _temp4180) ==  Cyc_Absyn_StructType: 0){ _LL4215:
_temp4214=(( struct Cyc_Absyn_StructType_struct*) _temp4180)->f2; goto _LL4213;
_LL4213: _temp4212=(( struct Cyc_Absyn_StructType_struct*) _temp4180)->f3; goto
_LL4199;} else{ goto _LL4200;} _LL4200: if(( unsigned int) _temp4180 >  4u?*((
int*) _temp4180) ==  Cyc_Absyn_UnionType: 0){ _LL4219: _temp4218=(( struct Cyc_Absyn_UnionType_struct*)
_temp4180)->f2; goto _LL4217; _LL4217: _temp4216=(( struct Cyc_Absyn_UnionType_struct*)
_temp4180)->f3; goto _LL4201;} else{ goto _LL4202;} _LL4202: if(( unsigned int)
_temp4180 >  4u?*(( int*) _temp4180) ==  Cyc_Absyn_AnonStructType: 0){ _LL4221:
_temp4220=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4180)->f1; goto
_LL4203;} else{ goto _LL4204;} _LL4204: if(( unsigned int) _temp4180 >  4u?*((
int*) _temp4180) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4223: _temp4222=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4180)->f1; goto _LL4205;} else{ goto
_LL4206;} _LL4206: goto _LL4207; _LL4183: goto _LL4185; _LL4185: goto _LL4187;
_LL4187: goto _LL4189; _LL4189: return 1; _LL4191: return 0; _LL4193: return 0;
_LL4195: return Cyc_Tcutil_bits_only( _temp4208); _LL4197: for( 0; _temp4210 != 
0; _temp4210= _temp4210->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)
_temp4210->hd)).f2)){ return 0;}} return 1; _LL4199: if( _temp4212 ==  0){
return 0;}{ struct Cyc_Absyn_Structdecl* _temp4224=* _temp4212; if( _temp4224->fields
==  0){ return 0;}{ struct _RegionHandle _temp4225= _new_region(); struct
_RegionHandle* rgn=& _temp4225; _push_region( rgn);{ struct Cyc_List_List*
_temp4226=(( struct Cyc_List_List*(*)( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp4224->tvs, _temp4214);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp4224->fields))->v; for( 0; fs !=  0; fs=
fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn, _temp4226,(
void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ int _temp4227= 0;
_npop_handler( 0u); return _temp4227;}}}{ int _temp4228= 1; _npop_handler( 0u);
return _temp4228;}}; _pop_region( rgn);}} _LL4201: if( _temp4216 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp4229=* _temp4216; if( _temp4229->fields == 
0){ return 0;}{ struct _RegionHandle _temp4230= _new_region(); struct
_RegionHandle* rgn=& _temp4230; _push_region( rgn);{ struct Cyc_List_List*
_temp4231=(( struct Cyc_List_List*(*)( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp4229->tvs, _temp4218);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp4229->fields))->v; for( 0; fs !=  0; fs=
fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn, _temp4231,(
void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ int _temp4232= 0;
_npop_handler( 0u); return _temp4232;}}}{ int _temp4233= 1; _npop_handler( 0u);
return _temp4233;}}; _pop_region( rgn);}} _LL4203: _temp4222= _temp4220; goto
_LL4205; _LL4205: for( 0; _temp4222 !=  0; _temp4222= _temp4222->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*) _temp4222->hd)->type)){ return 0;}}
return 1; _LL4207: return 0; _LL4181:;} struct _tuple21{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp4234=( void*) e->r;
struct _tuple1* _temp4280; struct Cyc_Absyn_Exp* _temp4282; struct Cyc_Absyn_Exp*
_temp4284; struct Cyc_Absyn_Exp* _temp4286; struct Cyc_Absyn_Exp* _temp4288;
struct Cyc_Absyn_Exp* _temp4290; struct Cyc_Absyn_Exp* _temp4292; struct Cyc_Absyn_Exp*
_temp4294; struct Cyc_Absyn_Exp* _temp4296; void* _temp4298; struct Cyc_Absyn_Exp*
_temp4300; struct Cyc_Absyn_Exp* _temp4302; struct Cyc_Absyn_Exp* _temp4304;
struct Cyc_List_List* _temp4306; struct Cyc_List_List* _temp4308; struct Cyc_List_List*
_temp4310; struct Cyc_List_List* _temp4312; void* _temp4314; struct Cyc_List_List*
_temp4316; struct Cyc_List_List* _temp4318; _LL4236: if(*(( int*) _temp4234) == 
Cyc_Absyn_Const_e){ goto _LL4237;} else{ goto _LL4238;} _LL4238: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL4239;} else{ goto _LL4240;}
_LL4240: if(*(( int*) _temp4234) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL4241;}
else{ goto _LL4242;} _LL4242: if(*(( int*) _temp4234) ==  Cyc_Absyn_Offsetof_e){
goto _LL4243;} else{ goto _LL4244;} _LL4244: if(*(( int*) _temp4234) ==  Cyc_Absyn_Gentyp_e){
goto _LL4245;} else{ goto _LL4246;} _LL4246: if(*(( int*) _temp4234) ==  Cyc_Absyn_Enum_e){
goto _LL4247;} else{ goto _LL4248;} _LL4248: if(*(( int*) _temp4234) ==  Cyc_Absyn_AnonEnum_e){
goto _LL4249;} else{ goto _LL4250;} _LL4250: if(*(( int*) _temp4234) ==  Cyc_Absyn_Var_e){
_LL4281: _temp4280=(( struct Cyc_Absyn_Var_e_struct*) _temp4234)->f1; goto
_LL4251;} else{ goto _LL4252;} _LL4252: if(*(( int*) _temp4234) ==  Cyc_Absyn_Conditional_e){
_LL4287: _temp4286=(( struct Cyc_Absyn_Conditional_e_struct*) _temp4234)->f1;
goto _LL4285; _LL4285: _temp4284=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4234)->f2; goto _LL4283; _LL4283: _temp4282=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4234)->f3; goto _LL4253;} else{ goto _LL4254;} _LL4254: if(*(( int*)
_temp4234) ==  Cyc_Absyn_SeqExp_e){ _LL4291: _temp4290=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4234)->f1; goto _LL4289; _LL4289: _temp4288=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4234)->f2; goto _LL4255;} else{ goto _LL4256;} _LL4256: if(*(( int*)
_temp4234) ==  Cyc_Absyn_NoInstantiate_e){ _LL4293: _temp4292=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp4234)->f1; goto _LL4257;} else{ goto _LL4258;} _LL4258: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Instantiate_e){ _LL4295: _temp4294=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp4234)->f1; goto _LL4259;} else{ goto _LL4260;} _LL4260: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Cast_e){ _LL4299: _temp4298=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp4234)->f1; goto _LL4297; _LL4297: _temp4296=(( struct Cyc_Absyn_Cast_e_struct*)
_temp4234)->f2; goto _LL4261;} else{ goto _LL4262;} _LL4262: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Address_e){ _LL4301: _temp4300=(( struct Cyc_Absyn_Address_e_struct*)
_temp4234)->f1; goto _LL4263;} else{ goto _LL4264;} _LL4264: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Comprehension_e){ _LL4305: _temp4304=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4234)->f2; goto _LL4303; _LL4303: _temp4302=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4234)->f3; goto _LL4265;} else{ goto _LL4266;} _LL4266: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Array_e){ _LL4307: _temp4306=(( struct Cyc_Absyn_Array_e_struct*)
_temp4234)->f1; goto _LL4267;} else{ goto _LL4268;} _LL4268: if(*(( int*)
_temp4234) ==  Cyc_Absyn_AnonStruct_e){ _LL4309: _temp4308=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp4234)->f2; goto _LL4269;} else{ goto _LL4270;} _LL4270: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Struct_e){ _LL4311: _temp4310=(( struct Cyc_Absyn_Struct_e_struct*)
_temp4234)->f3; goto _LL4271;} else{ goto _LL4272;} _LL4272: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Primop_e){ _LL4315: _temp4314=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp4234)->f1; goto _LL4313; _LL4313: _temp4312=(( struct Cyc_Absyn_Primop_e_struct*)
_temp4234)->f2; goto _LL4273;} else{ goto _LL4274;} _LL4274: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Tuple_e){ _LL4317: _temp4316=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp4234)->f1; goto _LL4275;} else{ goto _LL4276;} _LL4276: if(*(( int*)
_temp4234) ==  Cyc_Absyn_Tunion_e){ _LL4319: _temp4318=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp4234)->f3; goto _LL4277;} else{ goto _LL4278;} _LL4278: goto _LL4279;
_LL4237: return 1; _LL4239: return 1; _LL4241: return 1; _LL4243: return 1;
_LL4245: return 1; _LL4247: return 1; _LL4249: return 1; _LL4251: { struct
_handler_cons _temp4320; _push_handler(& _temp4320);{ int _temp4322= 0; if(
setjmp( _temp4320.handler)){ _temp4322= 1;} if( ! _temp4322){{ void* _temp4323=
Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp4280); void* _temp4333; void*
_temp4335; struct Cyc_Absyn_Vardecl* _temp4337; void* _temp4339; _LL4325: if(*((
int*) _temp4323) ==  Cyc_Tcenv_VarRes){ _LL4334: _temp4333=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp4323)->f1; if(( unsigned int) _temp4333 >  1u?*(( int*) _temp4333) ==  Cyc_Absyn_Funname_b:
0){ goto _LL4326;} else{ goto _LL4327;}} else{ goto _LL4327;} _LL4327: if(*((
int*) _temp4323) ==  Cyc_Tcenv_VarRes){ _LL4336: _temp4335=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp4323)->f1; if(( unsigned int) _temp4335 >  1u?*(( int*) _temp4335) ==  Cyc_Absyn_Global_b:
0){ _LL4338: _temp4337=(( struct Cyc_Absyn_Global_b_struct*) _temp4335)->f1;
goto _LL4328;} else{ goto _LL4329;}} else{ goto _LL4329;} _LL4329: if(*(( int*)
_temp4323) ==  Cyc_Tcenv_VarRes){ _LL4340: _temp4339=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp4323)->f1; if( _temp4339 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL4330;}
else{ goto _LL4331;}} else{ goto _LL4331;} _LL4331: goto _LL4332; _LL4326: { int
_temp4341= 1; _npop_handler( 0u); return _temp4341;} _LL4328: { void* _temp4342=
Cyc_Tcutil_compress(( void*) _temp4337->type); _LL4344: if(( unsigned int)
_temp4342 >  4u?*(( int*) _temp4342) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4345;}
else{ goto _LL4346;} _LL4346: goto _LL4347; _LL4345: { int _temp4348= 1;
_npop_handler( 0u); return _temp4348;} _LL4347: { int _temp4349= var_okay;
_npop_handler( 0u); return _temp4349;} _LL4343:;} _LL4330: { int _temp4350= 0;
_npop_handler( 0u); return _temp4350;} _LL4332: { int _temp4351= var_okay;
_npop_handler( 0u); return _temp4351;} _LL4324:;}; _pop_handler();} else{ void*
_temp4321=( void*) _exn_thrown; void* _temp4353= _temp4321; _LL4355: if(
_temp4353 ==  Cyc_Dict_Absent){ goto _LL4356;} else{ goto _LL4357;} _LL4357:
goto _LL4358; _LL4356: return 0; _LL4358:( void) _throw( _temp4353); _LL4354:;}}}
_LL4253: return( Cyc_Tcutil_cnst_exp( te, 0, _temp4286)? Cyc_Tcutil_cnst_exp( te,
0, _temp4284): 0)? Cyc_Tcutil_cnst_exp( te, 0, _temp4282): 0; _LL4255: return
Cyc_Tcutil_cnst_exp( te, 0, _temp4290)? Cyc_Tcutil_cnst_exp( te, 0, _temp4288):
0; _LL4257: _temp4294= _temp4292; goto _LL4259; _LL4259: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp4294); _LL4261: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp4296); _LL4263: return Cyc_Tcutil_cnst_exp( te, 1, _temp4300); _LL4265:
return Cyc_Tcutil_cnst_exp( te, 0, _temp4304)? Cyc_Tcutil_cnst_exp( te, 0,
_temp4302): 0; _LL4267: _temp4308= _temp4306; goto _LL4269; _LL4269: _temp4310=
_temp4308; goto _LL4271; _LL4271: for( 0; _temp4310 !=  0; _temp4310= _temp4310->tl){
if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple21*) _temp4310->hd)).f2)){
return 0;}} return 1; _LL4273: _temp4316= _temp4312; goto _LL4275; _LL4275:
_temp4318= _temp4316; goto _LL4277; _LL4277: for( 0; _temp4318 !=  0; _temp4318=
_temp4318->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp4318->hd)){ return 0;}} return 1; _LL4279: return 0; _LL4235:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp4359= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp4389; struct Cyc_Absyn_Conref* _temp4391; struct Cyc_Absyn_Conref*
_temp4393; void* _temp4395; struct Cyc_List_List* _temp4397; struct Cyc_Absyn_Structdecl**
_temp4399; struct Cyc_List_List* _temp4401; struct Cyc_Absyn_Uniondecl**
_temp4403; struct Cyc_List_List* _temp4405; struct Cyc_List_List* _temp4407;
struct Cyc_List_List* _temp4409; _LL4361: if( _temp4359 == ( void*) Cyc_Absyn_VoidType){
goto _LL4362;} else{ goto _LL4363;} _LL4363: if(( unsigned int) _temp4359 >  4u?*((
int*) _temp4359) ==  Cyc_Absyn_IntType: 0){ goto _LL4364;} else{ goto _LL4365;}
_LL4365: if( _temp4359 == ( void*) Cyc_Absyn_FloatType){ goto _LL4366;} else{
goto _LL4367;} _LL4367: if( _temp4359 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL4368;} else{ goto _LL4369;} _LL4369: if(( unsigned int) _temp4359 >  4u?*((
int*) _temp4359) ==  Cyc_Absyn_PointerType: 0){ _LL4390: _temp4389=(( struct Cyc_Absyn_PointerType_struct*)
_temp4359)->f1; _LL4394: _temp4393= _temp4389.nullable; goto _LL4392; _LL4392:
_temp4391= _temp4389.bounds; goto _LL4370;} else{ goto _LL4371;} _LL4371: if((
unsigned int) _temp4359 >  4u?*(( int*) _temp4359) ==  Cyc_Absyn_ArrayType: 0){
_LL4396: _temp4395=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp4359)->f1;
goto _LL4372;} else{ goto _LL4373;} _LL4373: if(( unsigned int) _temp4359 >  4u?*((
int*) _temp4359) ==  Cyc_Absyn_TupleType: 0){ _LL4398: _temp4397=(( struct Cyc_Absyn_TupleType_struct*)
_temp4359)->f1; goto _LL4374;} else{ goto _LL4375;} _LL4375: if(( unsigned int)
_temp4359 >  4u?*(( int*) _temp4359) ==  Cyc_Absyn_StructType: 0){ _LL4402:
_temp4401=(( struct Cyc_Absyn_StructType_struct*) _temp4359)->f2; goto _LL4400;
_LL4400: _temp4399=(( struct Cyc_Absyn_StructType_struct*) _temp4359)->f3; goto
_LL4376;} else{ goto _LL4377;} _LL4377: if(( unsigned int) _temp4359 >  4u?*((
int*) _temp4359) ==  Cyc_Absyn_UnionType: 0){ _LL4406: _temp4405=(( struct Cyc_Absyn_UnionType_struct*)
_temp4359)->f2; goto _LL4404; _LL4404: _temp4403=(( struct Cyc_Absyn_UnionType_struct*)
_temp4359)->f3; goto _LL4378;} else{ goto _LL4379;} _LL4379: if(( unsigned int)
_temp4359 >  4u?*(( int*) _temp4359) ==  Cyc_Absyn_AnonStructType: 0){ _LL4408:
_temp4407=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4359)->f1; goto
_LL4380;} else{ goto _LL4381;} _LL4381: if(( unsigned int) _temp4359 >  4u?*((
int*) _temp4359) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4410: _temp4409=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4359)->f1; goto _LL4382;} else{ goto
_LL4383;} _LL4383: if(( unsigned int) _temp4359 >  4u?*(( int*) _temp4359) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL4384;} else{ goto _LL4385;} _LL4385: if((
unsigned int) _temp4359 >  4u?*(( int*) _temp4359) ==  Cyc_Absyn_EnumType: 0){
goto _LL4386;} else{ goto _LL4387;} _LL4387: goto _LL4388; _LL4362: goto _LL4364;
_LL4364: goto _LL4366; _LL4366: goto _LL4368; _LL4368: return 1; _LL4370: { void*
_temp4411=( void*)( Cyc_Absyn_compress_conref( _temp4391))->v; void* _temp4419;
void* _temp4421; _LL4413: if(( unsigned int) _temp4411 >  1u?*(( int*) _temp4411)
==  Cyc_Absyn_Eq_constr: 0){ _LL4420: _temp4419=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4411)->f1; if( _temp4419 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4414;}
else{ goto _LL4415;}} else{ goto _LL4415;} _LL4415: if(( unsigned int) _temp4411
>  1u?*(( int*) _temp4411) ==  Cyc_Absyn_Eq_constr: 0){ _LL4422: _temp4421=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4411)->f1; if(( unsigned int)
_temp4421 >  1u?*(( int*) _temp4421) ==  Cyc_Absyn_Upper_b: 0){ goto _LL4416;}
else{ goto _LL4417;}} else{ goto _LL4417;} _LL4417: goto _LL4418; _LL4414:
return 1; _LL4416: { void* _temp4423=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp4393))->v; int
_temp4429; _LL4425: if(( unsigned int) _temp4423 >  1u?*(( int*) _temp4423) == 
Cyc_Absyn_Eq_constr: 0){ _LL4430: _temp4429=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4423)->f1; goto _LL4426;} else{ goto _LL4427;} _LL4427: goto _LL4428;
_LL4426: return _temp4429; _LL4428: return 0; _LL4424:;} _LL4418: return 0;
_LL4412:;} _LL4372: return Cyc_Tcutil_supports_default( _temp4395); _LL4374:
for( 0; _temp4397 !=  0; _temp4397= _temp4397->tl){ if( ! Cyc_Tcutil_supports_default((*((
struct _tuple4*) _temp4397->hd)).f2)){ return 0;}} return 1; _LL4376: if(
_temp4399 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=* _temp4399; if(
sd->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs,
_temp4401,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);}
_LL4378: if( _temp4403 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*
_temp4403; if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp4405,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL4380: _temp4409= _temp4407; goto _LL4382; _LL4382: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp4409); _LL4384: goto _LL4386;
_LL4386: return 1; _LL4388: return 0; _LL4360:;} static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List* tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{
struct _RegionHandle _temp4431= _new_region(); struct _RegionHandle* rgn=&
_temp4431; _push_region( rgn);{ struct Cyc_List_List* _temp4432=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0;
fs= fs->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4432,( void*)(( struct
Cyc_Absyn_Structfield*) fs->hd)->type); if( ! Cyc_Tcutil_supports_default( t)){
int _temp4433= 0; _npop_handler( 0u); return _temp4433;}}}; _pop_region( rgn);}
return 1;}
