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
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*); extern void*
Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*, void*);
extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void* e2);
extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e); extern
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_Position_Segment*);
struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5*
Cyc_Tcutil_make_inst_var( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*); struct
_tuple6{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; extern struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc, struct
Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn); extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
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
Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1592= Cyc_Tcutil_compress( t);
_LL1594: if(( unsigned int) _temp1592 >  4u?*(( int*) _temp1592) ==  Cyc_Absyn_IntType:
0){ goto _LL1595;} else{ goto _LL1596;} _LL1596: if( _temp1592 == ( void*) Cyc_Absyn_FloatType){
goto _LL1597;} else{ goto _LL1598;} _LL1598: if( _temp1592 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1599;} else{ goto _LL1600;} _LL1600: if(( unsigned int) _temp1592 >  4u?*((
int*) _temp1592) ==  Cyc_Absyn_EnumType: 0){ goto _LL1601;} else{ goto _LL1602;}
_LL1602: if(( unsigned int) _temp1592 >  4u?*(( int*) _temp1592) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL1603;} else{ goto _LL1604;} _LL1604: goto _LL1605; _LL1595: goto
_LL1597; _LL1597: goto _LL1599; _LL1599: goto _LL1601; _LL1601: return 1;
_LL1603: return 1; _LL1605: return 0; _LL1593:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1607=({ struct _tuple8 _temp1606; _temp1606.f1= t1;
_temp1606.f2= t2; _temp1606;}); void* _temp1629; void* _temp1631; void*
_temp1633; void* _temp1635; void* _temp1637; void* _temp1639; void* _temp1641;
void* _temp1643; void* _temp1645; void* _temp1647; void* _temp1649; void*
_temp1651; void* _temp1653; void* _temp1655; void* _temp1657; void* _temp1659;
void* _temp1661; void* _temp1663; void* _temp1665; void* _temp1667; void*
_temp1669; void* _temp1671; void* _temp1673; void* _temp1675; void* _temp1677;
void* _temp1679; void* _temp1681; _LL1609: _LL1632: _temp1631= _temp1607.f1; if(
_temp1631 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1630;} else{ goto _LL1611;}
_LL1630: _temp1629= _temp1607.f2; if( _temp1629 == ( void*) Cyc_Absyn_FloatType){
goto _LL1610;} else{ goto _LL1611;} _LL1611: _LL1636: _temp1635= _temp1607.f1;
if( _temp1635 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1634;} else{ goto
_LL1613;} _LL1634: _temp1633= _temp1607.f2; if(( unsigned int) _temp1633 >  4u?*((
int*) _temp1633) ==  Cyc_Absyn_IntType: 0){ goto _LL1612;} else{ goto _LL1613;}
_LL1613: _LL1640: _temp1639= _temp1607.f1; if( _temp1639 == ( void*) Cyc_Absyn_FloatType){
goto _LL1638;} else{ goto _LL1615;} _LL1638: _temp1637= _temp1607.f2; if((
unsigned int) _temp1637 >  4u?*(( int*) _temp1637) ==  Cyc_Absyn_IntType: 0){
goto _LL1614;} else{ goto _LL1615;} _LL1615: _LL1646: _temp1645= _temp1607.f1;
if(( unsigned int) _temp1645 >  4u?*(( int*) _temp1645) ==  Cyc_Absyn_IntType: 0){
_LL1648: _temp1647=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1645)->f2;
if( _temp1647 == ( void*) Cyc_Absyn_B8){ goto _LL1642;} else{ goto _LL1617;}}
else{ goto _LL1617;} _LL1642: _temp1641= _temp1607.f2; if(( unsigned int)
_temp1641 >  4u?*(( int*) _temp1641) ==  Cyc_Absyn_IntType: 0){ _LL1644:
_temp1643=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1641)->f2; if(
_temp1643 == ( void*) Cyc_Absyn_B8){ goto _LL1616;} else{ goto _LL1617;}} else{
goto _LL1617;} _LL1617: _LL1650: _temp1649= _temp1607.f1; if(( unsigned int)
_temp1649 >  4u?*(( int*) _temp1649) ==  Cyc_Absyn_IntType: 0){ _LL1652:
_temp1651=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1649)->f2; if(
_temp1651 == ( void*) Cyc_Absyn_B8){ goto _LL1618;} else{ goto _LL1619;}} else{
goto _LL1619;} _LL1619: _LL1656: _temp1655= _temp1607.f1; if(( unsigned int)
_temp1655 >  4u?*(( int*) _temp1655) ==  Cyc_Absyn_IntType: 0){ _LL1658:
_temp1657=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1655)->f2; if(
_temp1657 == ( void*) Cyc_Absyn_B4){ goto _LL1654;} else{ goto _LL1621;}} else{
goto _LL1621;} _LL1654: _temp1653= _temp1607.f2; if( _temp1653 == ( void*) Cyc_Absyn_FloatType){
goto _LL1620;} else{ goto _LL1621;} _LL1621: _LL1664: _temp1663= _temp1607.f1;
if(( unsigned int) _temp1663 >  4u?*(( int*) _temp1663) ==  Cyc_Absyn_IntType: 0){
_LL1666: _temp1665=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1663)->f2;
if( _temp1665 == ( void*) Cyc_Absyn_B4){ goto _LL1660;} else{ goto _LL1623;}}
else{ goto _LL1623;} _LL1660: _temp1659= _temp1607.f2; if(( unsigned int)
_temp1659 >  4u?*(( int*) _temp1659) ==  Cyc_Absyn_IntType: 0){ _LL1662:
_temp1661=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1659)->f2; if(
_temp1661 == ( void*) Cyc_Absyn_B2){ goto _LL1622;} else{ goto _LL1623;}} else{
goto _LL1623;} _LL1623: _LL1672: _temp1671= _temp1607.f1; if(( unsigned int)
_temp1671 >  4u?*(( int*) _temp1671) ==  Cyc_Absyn_IntType: 0){ _LL1674:
_temp1673=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1671)->f2; if(
_temp1673 == ( void*) Cyc_Absyn_B4){ goto _LL1668;} else{ goto _LL1625;}} else{
goto _LL1625;} _LL1668: _temp1667= _temp1607.f2; if(( unsigned int) _temp1667 > 
4u?*(( int*) _temp1667) ==  Cyc_Absyn_IntType: 0){ _LL1670: _temp1669=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1667)->f2; if( _temp1669 == ( void*) Cyc_Absyn_B1){
goto _LL1624;} else{ goto _LL1625;}} else{ goto _LL1625;} _LL1625: _LL1680:
_temp1679= _temp1607.f1; if(( unsigned int) _temp1679 >  4u?*(( int*) _temp1679)
==  Cyc_Absyn_IntType: 0){ _LL1682: _temp1681=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1679)->f2; if( _temp1681 == ( void*) Cyc_Absyn_B2){ goto _LL1676;} else{
goto _LL1627;}} else{ goto _LL1627;} _LL1676: _temp1675= _temp1607.f2; if((
unsigned int) _temp1675 >  4u?*(( int*) _temp1675) ==  Cyc_Absyn_IntType: 0){
_LL1678: _temp1677=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1675)->f2;
if( _temp1677 == ( void*) Cyc_Absyn_B1){ goto _LL1626;} else{ goto _LL1627;}}
else{ goto _LL1627;} _LL1627: goto _LL1628; _LL1610: goto _LL1612; _LL1612: goto
_LL1614; _LL1614: return 1; _LL1616: return 0; _LL1618: goto _LL1620; _LL1620:
goto _LL1622; _LL1622: goto _LL1624; _LL1624: goto _LL1626; _LL1626: return 1;
_LL1628: return 0; _LL1608:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) el->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*) max_arith_type->v)){ max_arith_type=( struct Cyc_Core_Opt*)({ struct
Cyc_Core_Opt* _temp1683=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1683->v=( void*) t1; _temp1683;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify(
t,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*) el->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp1686; _temp1686.tag= Cyc_Std_String_pa; _temp1686.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*) el->hd)->topt))->v);{ struct Cyc_Std_String_pa_struct _temp1685;
_temp1685.tag= Cyc_Std_String_pa; _temp1685.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1684[ 2u]={& _temp1685,& _temp1686}; Cyc_Tcutil_terr((( struct
Cyc_Absyn_Exp*) el->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1684, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1687= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1689: if(( unsigned int) _temp1687 >  4u?*(( int*) _temp1687) ==  Cyc_Absyn_PointerType:
0){ goto _LL1690;} else{ goto _LL1691;} _LL1691: goto _LL1692; _LL1690: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1688; _LL1692: return 0; _LL1688:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1693= Cyc_Tcutil_compress(
t); _LL1695: if(( unsigned int) _temp1693 >  4u?*(( int*) _temp1693) ==  Cyc_Absyn_IntType:
0){ goto _LL1696;} else{ goto _LL1697;} _LL1697: if(( unsigned int) _temp1693 > 
4u?*(( int*) _temp1693) ==  Cyc_Absyn_EnumType: 0){ goto _LL1698;} else{ goto
_LL1699;} _LL1699: if(( unsigned int) _temp1693 >  4u?*(( int*) _temp1693) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL1700;} else{ goto _LL1701;} _LL1701: goto
_LL1702; _LL1696: goto _LL1698; _LL1698: return 1; _LL1700: return 1; _LL1702:
return 0; _LL1694:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1703[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1703, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1704[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1704, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1706=({ struct _tuple8 _temp1705; _temp1705.f1= t1; _temp1705.f2= t2;
_temp1705;}); void* _temp1718; struct Cyc_Absyn_PtrInfo _temp1720; void*
_temp1722; struct Cyc_Absyn_PtrInfo _temp1724; void* _temp1726; struct Cyc_Absyn_Exp*
_temp1728; struct Cyc_Absyn_Tqual _temp1730; void* _temp1732; void* _temp1734;
struct Cyc_Absyn_Exp* _temp1736; struct Cyc_Absyn_Tqual _temp1738; void*
_temp1740; void* _temp1742; struct Cyc_Absyn_TunionInfo _temp1744; struct Cyc_List_List*
_temp1746; void* _temp1748; struct Cyc_Absyn_Tuniondecl** _temp1750; struct Cyc_Absyn_Tuniondecl*
_temp1752; void* _temp1753; struct Cyc_Absyn_TunionFieldInfo _temp1755; struct
Cyc_List_List* _temp1757; void* _temp1759; struct Cyc_Absyn_Tunionfield*
_temp1761; struct Cyc_Absyn_Tuniondecl* _temp1763; void* _temp1765; struct Cyc_Absyn_TunionInfo
_temp1767; void* _temp1769; struct Cyc_List_List* _temp1771; void* _temp1773;
struct Cyc_Absyn_Tuniondecl** _temp1775; struct Cyc_Absyn_Tuniondecl* _temp1777;
void* _temp1778; struct Cyc_Absyn_PtrInfo _temp1780; struct Cyc_Absyn_Conref*
_temp1782; struct Cyc_Absyn_Tqual _temp1784; struct Cyc_Absyn_Conref* _temp1786;
void* _temp1788; void* _temp1790; _LL1708: _LL1723: _temp1722= _temp1706.f1; if((
unsigned int) _temp1722 >  4u?*(( int*) _temp1722) ==  Cyc_Absyn_PointerType: 0){
_LL1725: _temp1724=(( struct Cyc_Absyn_PointerType_struct*) _temp1722)->f1; goto
_LL1719;} else{ goto _LL1710;} _LL1719: _temp1718= _temp1706.f2; if((
unsigned int) _temp1718 >  4u?*(( int*) _temp1718) ==  Cyc_Absyn_PointerType: 0){
_LL1721: _temp1720=(( struct Cyc_Absyn_PointerType_struct*) _temp1718)->f1; goto
_LL1709;} else{ goto _LL1710;} _LL1710: _LL1735: _temp1734= _temp1706.f1; if((
unsigned int) _temp1734 >  4u?*(( int*) _temp1734) ==  Cyc_Absyn_ArrayType: 0){
_LL1741: _temp1740=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1734)->f1;
goto _LL1739; _LL1739: _temp1738=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1734)->f2; goto _LL1737; _LL1737: _temp1736=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1734)->f3; goto _LL1727;} else{ goto _LL1712;} _LL1727: _temp1726=
_temp1706.f2; if(( unsigned int) _temp1726 >  4u?*(( int*) _temp1726) ==  Cyc_Absyn_ArrayType:
0){ _LL1733: _temp1732=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1726)->f1;
goto _LL1731; _LL1731: _temp1730=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1726)->f2; goto _LL1729; _LL1729: _temp1728=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1726)->f3; goto _LL1711;} else{ goto _LL1712;} _LL1712: _LL1754: _temp1753=
_temp1706.f1; if(( unsigned int) _temp1753 >  4u?*(( int*) _temp1753) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1756: _temp1755=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1753)->f1;
_LL1760: _temp1759=( void*) _temp1755.field_info; if(*(( int*) _temp1759) == 
Cyc_Absyn_KnownTunionfield){ _LL1764: _temp1763=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1759)->f1; goto _LL1762; _LL1762: _temp1761=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1759)->f2; goto _LL1758;} else{ goto _LL1714;} _LL1758: _temp1757=
_temp1755.targs; goto _LL1743;} else{ goto _LL1714;} _LL1743: _temp1742=
_temp1706.f2; if(( unsigned int) _temp1742 >  4u?*(( int*) _temp1742) ==  Cyc_Absyn_TunionType:
0){ _LL1745: _temp1744=(( struct Cyc_Absyn_TunionType_struct*) _temp1742)->f1;
_LL1749: _temp1748=( void*) _temp1744.tunion_info; if(*(( int*) _temp1748) == 
Cyc_Absyn_KnownTunion){ _LL1751: _temp1750=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1748)->f1; _temp1752=* _temp1750; goto _LL1747;} else{ goto _LL1714;}
_LL1747: _temp1746= _temp1744.targs; goto _LL1713;} else{ goto _LL1714;} _LL1714:
_LL1779: _temp1778= _temp1706.f1; if(( unsigned int) _temp1778 >  4u?*(( int*)
_temp1778) ==  Cyc_Absyn_PointerType: 0){ _LL1781: _temp1780=(( struct Cyc_Absyn_PointerType_struct*)
_temp1778)->f1; _LL1791: _temp1790=( void*) _temp1780.elt_typ; goto _LL1789;
_LL1789: _temp1788=( void*) _temp1780.rgn_typ; goto _LL1787; _LL1787: _temp1786=
_temp1780.nullable; goto _LL1785; _LL1785: _temp1784= _temp1780.tq; goto _LL1783;
_LL1783: _temp1782= _temp1780.bounds; goto _LL1766;} else{ goto _LL1716;}
_LL1766: _temp1765= _temp1706.f2; if(( unsigned int) _temp1765 >  4u?*(( int*)
_temp1765) ==  Cyc_Absyn_TunionType: 0){ _LL1768: _temp1767=(( struct Cyc_Absyn_TunionType_struct*)
_temp1765)->f1; _LL1774: _temp1773=( void*) _temp1767.tunion_info; if(*(( int*)
_temp1773) ==  Cyc_Absyn_KnownTunion){ _LL1776: _temp1775=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1773)->f1; _temp1777=* _temp1775; goto _LL1772;} else{ goto _LL1716;}
_LL1772: _temp1771= _temp1767.targs; goto _LL1770; _LL1770: _temp1769=( void*)
_temp1767.rgn; goto _LL1715;} else{ goto _LL1716;} _LL1716: goto _LL1717;
_LL1709: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1724.nullable, _temp1720.nullable)){ void* _temp1792=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1724.nullable))->v;
int _temp1798; _LL1794: if(( unsigned int) _temp1792 >  1u?*(( int*) _temp1792)
==  Cyc_Absyn_Eq_constr: 0){ _LL1799: _temp1798=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1792)->f1; goto _LL1795;} else{ goto _LL1796;} _LL1796: goto _LL1797;
_LL1795: okay= ! _temp1798; goto _LL1793; _LL1797:( int) _throw(({ void*
_temp1800[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp1800, sizeof( void*), 0u));}));
_LL1793:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1724.bounds,
_temp1720.bounds)){ struct _tuple8 _temp1802=({ struct _tuple8 _temp1801;
_temp1801.f1=( void*)( Cyc_Absyn_compress_conref( _temp1724.bounds))->v;
_temp1801.f2=( void*)( Cyc_Absyn_compress_conref( _temp1720.bounds))->v;
_temp1801;}); void* _temp1812; void* _temp1814; void* _temp1816; void* _temp1818;
void* _temp1820; void* _temp1822; struct Cyc_Absyn_Exp* _temp1824; void*
_temp1826; void* _temp1828; struct Cyc_Absyn_Exp* _temp1830; void* _temp1832;
void* _temp1834; void* _temp1836; void* _temp1838; _LL1804: _LL1817: _temp1816=
_temp1802.f1; if(( unsigned int) _temp1816 >  1u?*(( int*) _temp1816) ==  Cyc_Absyn_Eq_constr:
0){ _LL1819: _temp1818=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1816)->f1;
if(( unsigned int) _temp1818 >  1u?*(( int*) _temp1818) ==  Cyc_Absyn_Upper_b: 0){
goto _LL1813;} else{ goto _LL1806;}} else{ goto _LL1806;} _LL1813: _temp1812=
_temp1802.f2; if(( unsigned int) _temp1812 >  1u?*(( int*) _temp1812) ==  Cyc_Absyn_Eq_constr:
0){ _LL1815: _temp1814=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1812)->f1;
if( _temp1814 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1805;} else{ goto
_LL1806;}} else{ goto _LL1806;} _LL1806: _LL1827: _temp1826= _temp1802.f1; if((
unsigned int) _temp1826 >  1u?*(( int*) _temp1826) ==  Cyc_Absyn_Eq_constr: 0){
_LL1829: _temp1828=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1826)->f1;
if(( unsigned int) _temp1828 >  1u?*(( int*) _temp1828) ==  Cyc_Absyn_Upper_b: 0){
_LL1831: _temp1830=(( struct Cyc_Absyn_Upper_b_struct*) _temp1828)->f1; goto
_LL1821;} else{ goto _LL1808;}} else{ goto _LL1808;} _LL1821: _temp1820=
_temp1802.f2; if(( unsigned int) _temp1820 >  1u?*(( int*) _temp1820) ==  Cyc_Absyn_Eq_constr:
0){ _LL1823: _temp1822=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1820)->f1;
if(( unsigned int) _temp1822 >  1u?*(( int*) _temp1822) ==  Cyc_Absyn_Upper_b: 0){
_LL1825: _temp1824=(( struct Cyc_Absyn_Upper_b_struct*) _temp1822)->f1; goto
_LL1807;} else{ goto _LL1808;}} else{ goto _LL1808;} _LL1808: _LL1837: _temp1836=
_temp1802.f1; if(( unsigned int) _temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_Eq_constr:
0){ _LL1839: _temp1838=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1836)->f1;
if( _temp1838 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1833;} else{ goto
_LL1810;}} else{ goto _LL1810;} _LL1833: _temp1832= _temp1802.f2; if((
unsigned int) _temp1832 >  1u?*(( int*) _temp1832) ==  Cyc_Absyn_Eq_constr: 0){
_LL1835: _temp1834=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1832)->f1;
if( _temp1834 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1809;} else{ goto
_LL1810;}} else{ goto _LL1810;} _LL1810: goto _LL1811; _LL1805: okay= 1; goto
_LL1803; _LL1807: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1830) >=  Cyc_Evexp_eval_const_uint_exp(
_temp1824): 0;({ void* _temp1840[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1840, sizeof( void*), 0u));}); goto
_LL1803; _LL1809: okay= 1; goto _LL1803; _LL1811: okay= 0; goto _LL1803; _LL1803:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1724.elt_typ,( void*) _temp1720.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1724.rgn_typ,( void*) _temp1720.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1724.rgn_typ,( void*) _temp1720.rgn_typ):
0; okay= okay? !( _temp1724.tq).q_const? 1:( _temp1720.tq).q_const: 0; return
okay;} _LL1711: { int okay; okay=( _temp1736 !=  0? _temp1728 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1736)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1728)): 0; return( okay? Cyc_Tcutil_unify(
_temp1740, _temp1732): 0)? ! _temp1738.q_const? 1: _temp1730.q_const: 0;}
_LL1713: if(( _temp1763 ==  _temp1752? 1: Cyc_Absyn_qvar_cmp( _temp1763->name,
_temp1752->name) ==  0)? _temp1761->typs ==  0: 0){ for( 0; _temp1757 !=  0?
_temp1746 !=  0: 0;( _temp1757= _temp1757->tl, _temp1746= _temp1746->tl)){ if( !
Cyc_Tcutil_unify(( void*) _temp1757->hd,( void*) _temp1746->hd)){ break;}} if(
_temp1757 ==  0? _temp1746 ==  0: 0){ return 1;}} return 0; _LL1715:{ void*
_temp1841= Cyc_Tcutil_compress( _temp1790); struct Cyc_Absyn_TunionFieldInfo
_temp1847; struct Cyc_List_List* _temp1849; void* _temp1851; struct Cyc_Absyn_Tunionfield*
_temp1853; struct Cyc_Absyn_Tuniondecl* _temp1855; _LL1843: if(( unsigned int)
_temp1841 >  4u?*(( int*) _temp1841) ==  Cyc_Absyn_TunionFieldType: 0){ _LL1848:
_temp1847=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1841)->f1; _LL1852:
_temp1851=( void*) _temp1847.field_info; if(*(( int*) _temp1851) ==  Cyc_Absyn_KnownTunionfield){
_LL1856: _temp1855=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1851)->f1;
goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1851)->f2; goto _LL1850;} else{ goto _LL1845;} _LL1850: _temp1849=
_temp1847.targs; goto _LL1844;} else{ goto _LL1845;} _LL1845: goto _LL1846;
_LL1844: if( ! Cyc_Tcutil_unify( _temp1788, _temp1769)? ! Cyc_Tcenv_region_outlives(
te, _temp1788, _temp1769): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1786,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1782,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1777->name, _temp1855->name) ==  0? _temp1853->typs !=  0: 0){ int okay= 1;
for( 0; _temp1849 !=  0? _temp1771 !=  0: 0;( _temp1849= _temp1849->tl,
_temp1771= _temp1771->tl)){ if( ! Cyc_Tcutil_unify(( void*) _temp1849->hd,( void*)
_temp1771->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1849 !=  0)? 1:
_temp1771 !=  0){ return 0;} return 1;} goto _LL1842; _LL1846: goto _LL1842;
_LL1842:;} return 0; _LL1717: return Cyc_Tcutil_unify( t1, t2); _LL1707:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1857[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1857, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Std_String_pa_struct
_temp1860; _temp1860.tag= Cyc_Std_String_pa; _temp1860.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct _temp1859;
_temp1859.tag= Cyc_Std_String_pa; _temp1859.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1858[ 2u]={& _temp1859,& _temp1860}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1858, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} int Cyc_Tcutil_coerceable( void* t){ void* _temp1861= Cyc_Tcutil_compress(
t); _LL1863: if(( unsigned int) _temp1861 >  4u?*(( int*) _temp1861) ==  Cyc_Absyn_IntType:
0){ goto _LL1864;} else{ goto _LL1865;} _LL1865: if( _temp1861 == ( void*) Cyc_Absyn_FloatType){
goto _LL1866;} else{ goto _LL1867;} _LL1867: if( _temp1861 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1868;} else{ goto _LL1869;} _LL1869: goto _LL1870; _LL1864: goto _LL1866;
_LL1866: goto _LL1868; _LL1868: return 1; _LL1870: return 0; _LL1862:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1871=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp1871->f1= x->tq; _temp1871->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1871;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1872= t1; struct Cyc_List_List* _temp1884; struct Cyc_Absyn_Structdecl**
_temp1886; struct Cyc_List_List* _temp1888; struct _tuple1* _temp1890; struct
Cyc_List_List* _temp1892; _LL1874: if( _temp1872 == ( void*) Cyc_Absyn_VoidType){
goto _LL1875;} else{ goto _LL1876;} _LL1876: if(( unsigned int) _temp1872 >  4u?*((
int*) _temp1872) ==  Cyc_Absyn_TupleType: 0){ _LL1885: _temp1884=(( struct Cyc_Absyn_TupleType_struct*)
_temp1872)->f1; goto _LL1877;} else{ goto _LL1878;} _LL1878: if(( unsigned int)
_temp1872 >  4u?*(( int*) _temp1872) ==  Cyc_Absyn_StructType: 0){ _LL1891:
_temp1890=(( struct Cyc_Absyn_StructType_struct*) _temp1872)->f1; goto _LL1889;
_LL1889: _temp1888=(( struct Cyc_Absyn_StructType_struct*) _temp1872)->f2; goto
_LL1887; _LL1887: _temp1886=(( struct Cyc_Absyn_StructType_struct*) _temp1872)->f3;
goto _LL1879;} else{ goto _LL1880;} _LL1880: if(( unsigned int) _temp1872 >  4u?*((
int*) _temp1872) ==  Cyc_Absyn_AnonStructType: 0){ _LL1893: _temp1892=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1872)->f1; goto _LL1881;} else{ goto
_LL1882;} _LL1882: goto _LL1883; _LL1875: return 0; _LL1877: return _temp1884;
_LL1879: if( _temp1886 ==  0? 1: _temp1890 ==  0){ return({ struct Cyc_List_List*
_temp1894=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1894->hd=( void*)({ struct _tuple4* _temp1895=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1895->f1= Cyc_Absyn_empty_tqual(); _temp1895->f2=
t1; _temp1895;}); _temp1894->tl= 0; _temp1894;});}{ struct Cyc_Absyn_Structdecl*
_temp1896=* _temp1886; struct _tuple1 _temp1897=* _temp1890; struct Cyc_List_List*
_temp1898=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1896->tvs, _temp1888); if( _temp1896->fields ==  0){
return({ struct Cyc_List_List* _temp1899=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1899->hd=( void*)({ struct _tuple4*
_temp1900=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1900->f1=
Cyc_Absyn_empty_tqual(); _temp1900->f2= t1; _temp1900;}); _temp1899->tl= 0;
_temp1899;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1898,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1896->fields))->v);}
_LL1881: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1892); _LL1883: return({
struct Cyc_List_List* _temp1901=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1901->hd=( void*)({ struct _tuple4* _temp1902=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1902->f1= Cyc_Absyn_empty_tqual();
_temp1902->f2= t1; _temp1902;}); _temp1901->tl= 0; _temp1901;}); _LL1873:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a= a->tl){
if( Cyc_Tcutil_unify( t1,(*(( struct _tuple8*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*((
struct _tuple8*) a->hd)).f2): 0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1904=({ struct _tuple8 _temp1903;
_temp1903.f1= t1; _temp1903.f2= t2; _temp1903;}); void* _temp1910; struct Cyc_Absyn_PtrInfo
_temp1912; struct Cyc_Absyn_Conref* _temp1914; struct Cyc_Absyn_Tqual _temp1916;
struct Cyc_Absyn_Conref* _temp1918; void* _temp1920; void* _temp1922; void*
_temp1924; struct Cyc_Absyn_PtrInfo _temp1926; struct Cyc_Absyn_Conref*
_temp1928; struct Cyc_Absyn_Tqual _temp1930; struct Cyc_Absyn_Conref* _temp1932;
void* _temp1934; void* _temp1936; _LL1906: _LL1925: _temp1924= _temp1904.f1; if((
unsigned int) _temp1924 >  4u?*(( int*) _temp1924) ==  Cyc_Absyn_PointerType: 0){
_LL1927: _temp1926=(( struct Cyc_Absyn_PointerType_struct*) _temp1924)->f1;
_LL1937: _temp1936=( void*) _temp1926.elt_typ; goto _LL1935; _LL1935: _temp1934=(
void*) _temp1926.rgn_typ; goto _LL1933; _LL1933: _temp1932= _temp1926.nullable;
goto _LL1931; _LL1931: _temp1930= _temp1926.tq; goto _LL1929; _LL1929: _temp1928=
_temp1926.bounds; goto _LL1911;} else{ goto _LL1908;} _LL1911: _temp1910=
_temp1904.f2; if(( unsigned int) _temp1910 >  4u?*(( int*) _temp1910) ==  Cyc_Absyn_PointerType:
0){ _LL1913: _temp1912=(( struct Cyc_Absyn_PointerType_struct*) _temp1910)->f1;
_LL1923: _temp1922=( void*) _temp1912.elt_typ; goto _LL1921; _LL1921: _temp1920=(
void*) _temp1912.rgn_typ; goto _LL1919; _LL1919: _temp1918= _temp1912.nullable;
goto _LL1917; _LL1917: _temp1916= _temp1912.tq; goto _LL1915; _LL1915: _temp1914=
_temp1912.bounds; goto _LL1907;} else{ goto _LL1908;} _LL1908: goto _LL1909;
_LL1907: if( _temp1930.q_const? ! _temp1916.q_const: 0){ return 0;} if(( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1932, _temp1918)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1932): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1918): 0){ return 0;}
if( ! Cyc_Tcutil_unify( _temp1934, _temp1920)? ! Cyc_Tcenv_region_outlives( te,
_temp1934, _temp1920): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1928, _temp1914)){ struct _tuple8 _temp1939=({ struct _tuple8 _temp1938;
_temp1938.f1= Cyc_Absyn_conref_val( _temp1928); _temp1938.f2= Cyc_Absyn_conref_val(
_temp1914); _temp1938;}); void* _temp1947; void* _temp1949; void* _temp1951;
struct Cyc_Absyn_Exp* _temp1953; void* _temp1955; struct Cyc_Absyn_Exp*
_temp1957; _LL1941: _LL1950: _temp1949= _temp1939.f1; if(( unsigned int)
_temp1949 >  1u?*(( int*) _temp1949) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1948;}
else{ goto _LL1943;} _LL1948: _temp1947= _temp1939.f2; if( _temp1947 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1942;} else{ goto _LL1943;} _LL1943: _LL1956:
_temp1955= _temp1939.f1; if(( unsigned int) _temp1955 >  1u?*(( int*) _temp1955)
==  Cyc_Absyn_Upper_b: 0){ _LL1958: _temp1957=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1955)->f1; goto _LL1952;} else{ goto _LL1945;} _LL1952: _temp1951=
_temp1939.f2; if(( unsigned int) _temp1951 >  1u?*(( int*) _temp1951) ==  Cyc_Absyn_Upper_b:
0){ _LL1954: _temp1953=(( struct Cyc_Absyn_Upper_b_struct*) _temp1951)->f1; goto
_LL1944;} else{ goto _LL1945;} _LL1945: goto _LL1946; _LL1942: goto _LL1940;
_LL1944: if( Cyc_Evexp_eval_const_uint_exp( _temp1957) <  Cyc_Evexp_eval_const_uint_exp(
_temp1953)){ return 0;} goto _LL1940; _LL1946: return 0; _LL1940:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1959=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1959->hd=( void*)({ struct _tuple8*
_temp1960=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1960->f1=
t1; _temp1960->f2= t2; _temp1960;}); _temp1959->tl= assume; _temp1959;}),
_temp1936, _temp1922); _LL1909: return 0; _LL1905:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1962=({ struct _tuple8 _temp1961;
_temp1961.f1= Cyc_Tcutil_compress( t1); _temp1961.f2= Cyc_Tcutil_compress( t2);
_temp1961;}); void* _temp1968; void* _temp1970; void* _temp1972; void* _temp1974;
_LL1964: _LL1973: _temp1972= _temp1962.f1; if(( unsigned int) _temp1972 >  4u?*((
int*) _temp1972) ==  Cyc_Absyn_IntType: 0){ _LL1975: _temp1974=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1972)->f2; goto _LL1969;} else{ goto _LL1966;}
_LL1969: _temp1968= _temp1962.f2; if(( unsigned int) _temp1968 >  4u?*(( int*)
_temp1968) ==  Cyc_Absyn_IntType: 0){ _LL1971: _temp1970=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1968)->f2; goto _LL1965;} else{ goto _LL1966;} _LL1966: goto _LL1967;
_LL1965: return _temp1974 ==  _temp1970; _LL1967: return 0; _LL1963:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2= tqs2->tl, tqs1= tqs1->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1978; void* _temp1979; struct Cyc_Absyn_Tqual _temp1981;
struct _tuple4* _temp1976=( struct _tuple4*) tqs1->hd; _temp1978=* _temp1976;
_LL1982: _temp1981= _temp1978.f1; goto _LL1980; _LL1980: _temp1979= _temp1978.f2;
goto _LL1977; _LL1977: { struct _tuple4 _temp1985; void* _temp1986; struct Cyc_Absyn_Tqual
_temp1988; struct _tuple4* _temp1983=( struct _tuple4*) tqs2->hd; _temp1985=*
_temp1983; _LL1989: _temp1988= _temp1985.f1; goto _LL1987; _LL1987: _temp1986=
_temp1985.f2; goto _LL1984; _LL1984: if( _temp1988.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1979, _temp1986): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1979, _temp1986)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1979,
_temp1986)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1990= t2; struct Cyc_Absyn_Uniondecl** _temp1998; struct Cyc_List_List*
_temp2000; void* _temp2002; _LL1992: if(( unsigned int) _temp1990 >  4u?*(( int*)
_temp1990) ==  Cyc_Absyn_UnionType: 0){ _LL2001: _temp2000=(( struct Cyc_Absyn_UnionType_struct*)
_temp1990)->f2; goto _LL1999; _LL1999: _temp1998=(( struct Cyc_Absyn_UnionType_struct*)
_temp1990)->f3; goto _LL1993;} else{ goto _LL1994;} _LL1994: if(( unsigned int)
_temp1990 >  4u?*(( int*) _temp1990) ==  Cyc_Absyn_IntType: 0){ _LL2003:
_temp2002=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1990)->f2; if(
_temp2002 == ( void*) Cyc_Absyn_B4){ goto _LL1995;} else{ goto _LL1996;}} else{
goto _LL1996;} _LL1996: goto _LL1997; _LL1993: if( _temp1998 ==  0){ goto
_LL1991;}{ struct Cyc_Absyn_Uniondecl* _temp2004=* _temp1998; if( _temp2004->fields
==  0){ goto _LL1991;}{ struct Cyc_List_List* _temp2005=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2004->tvs,
_temp2000);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2004->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp2005,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ return 1;}}} goto _LL1991;}} _LL1995: if( Cyc_Tcutil_typ_kind(
t2) == ( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1991; _LL1997: goto
_LL1991; _LL1991:;}{ void* _temp2006= t1; struct Cyc_Absyn_PtrInfo _temp2024;
struct Cyc_Absyn_Conref* _temp2026; struct Cyc_Absyn_Tqual _temp2028; struct Cyc_Absyn_Conref*
_temp2030; void* _temp2032; void* _temp2034; struct Cyc_Absyn_Exp* _temp2036;
struct Cyc_Absyn_Tqual _temp2038; void* _temp2040; struct Cyc_Absyn_Enumdecl*
_temp2042; _LL2008: if(( unsigned int) _temp2006 >  4u?*(( int*) _temp2006) == 
Cyc_Absyn_PointerType: 0){ _LL2025: _temp2024=(( struct Cyc_Absyn_PointerType_struct*)
_temp2006)->f1; _LL2035: _temp2034=( void*) _temp2024.elt_typ; goto _LL2033;
_LL2033: _temp2032=( void*) _temp2024.rgn_typ; goto _LL2031; _LL2031: _temp2030=
_temp2024.nullable; goto _LL2029; _LL2029: _temp2028= _temp2024.tq; goto _LL2027;
_LL2027: _temp2026= _temp2024.bounds; goto _LL2009;} else{ goto _LL2010;}
_LL2010: if(( unsigned int) _temp2006 >  4u?*(( int*) _temp2006) ==  Cyc_Absyn_ArrayType:
0){ _LL2041: _temp2040=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2006)->f1;
goto _LL2039; _LL2039: _temp2038=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2006)->f2; goto _LL2037; _LL2037: _temp2036=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2006)->f3; goto _LL2011;} else{ goto _LL2012;} _LL2012: if(( unsigned int)
_temp2006 >  4u?*(( int*) _temp2006) ==  Cyc_Absyn_EnumType: 0){ _LL2043:
_temp2042=(( struct Cyc_Absyn_EnumType_struct*) _temp2006)->f2; goto _LL2013;}
else{ goto _LL2014;} _LL2014: if(( unsigned int) _temp2006 >  4u?*(( int*)
_temp2006) ==  Cyc_Absyn_IntType: 0){ goto _LL2015;} else{ goto _LL2016;}
_LL2016: if( _temp2006 == ( void*) Cyc_Absyn_FloatType){ goto _LL2017;} else{
goto _LL2018;} _LL2018: if( _temp2006 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2019;} else{ goto _LL2020;} _LL2020: if( _temp2006 == ( void*) Cyc_Absyn_VoidType){
goto _LL2021;} else{ goto _LL2022;} _LL2022: goto _LL2023; _LL2009:{ void*
_temp2044= t2; struct Cyc_Absyn_PtrInfo _temp2052; struct Cyc_Absyn_Conref*
_temp2054; struct Cyc_Absyn_Tqual _temp2056; struct Cyc_Absyn_Conref* _temp2058;
void* _temp2060; void* _temp2062; _LL2046: if(( unsigned int) _temp2044 >  4u?*((
int*) _temp2044) ==  Cyc_Absyn_PointerType: 0){ _LL2053: _temp2052=(( struct Cyc_Absyn_PointerType_struct*)
_temp2044)->f1; _LL2063: _temp2062=( void*) _temp2052.elt_typ; goto _LL2061;
_LL2061: _temp2060=( void*) _temp2052.rgn_typ; goto _LL2059; _LL2059: _temp2058=
_temp2052.nullable; goto _LL2057; _LL2057: _temp2056= _temp2052.tq; goto _LL2055;
_LL2055: _temp2054= _temp2052.bounds; goto _LL2047;} else{ goto _LL2048;}
_LL2048: if( _temp2044 == ( void*) Cyc_Absyn_VoidType){ goto _LL2049;} else{
goto _LL2050;} _LL2050: goto _LL2051; _LL2047: { struct Cyc_List_List* _temp2064=({
struct Cyc_List_List* _temp2081=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2081->hd=( void*)({ struct _tuple8* _temp2082=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2082->f1= t1;
_temp2082->f2= t2; _temp2082;}); _temp2081->tl= 0; _temp2081;}); int _temp2065=
Cyc_Tcutil_ptrsubtype( te, _temp2064, _temp2034, _temp2062)? ! _temp2028.q_const?
1: _temp2056.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2026, _temp2054); if( ! bounds_ok){ struct _tuple8 _temp2067=({ struct
_tuple8 _temp2066; _temp2066.f1= Cyc_Absyn_conref_val( _temp2026); _temp2066.f2=
Cyc_Absyn_conref_val( _temp2054); _temp2066;}); void* _temp2073; struct Cyc_Absyn_Exp*
_temp2075; void* _temp2077; struct Cyc_Absyn_Exp* _temp2079; _LL2069: _LL2078:
_temp2077= _temp2067.f1; if(( unsigned int) _temp2077 >  1u?*(( int*) _temp2077)
==  Cyc_Absyn_Upper_b: 0){ _LL2080: _temp2079=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2077)->f1; goto _LL2074;} else{ goto _LL2071;} _LL2074: _temp2073=
_temp2067.f2; if(( unsigned int) _temp2073 >  1u?*(( int*) _temp2073) ==  Cyc_Absyn_Upper_b:
0){ _LL2076: _temp2075=(( struct Cyc_Absyn_Upper_b_struct*) _temp2073)->f1; goto
_LL2070;} else{ goto _LL2071;} _LL2071: goto _LL2072; _LL2070: if( Cyc_Evexp_eval_const_uint_exp(
_temp2079) >=  Cyc_Evexp_eval_const_uint_exp( _temp2075)){ bounds_ok= 1;} goto
_LL2068; _LL2072: bounds_ok= 1; goto _LL2068; _LL2068:;} return( bounds_ok?
_temp2065: 0)? Cyc_Tcutil_unify( _temp2032, _temp2060)? 1: Cyc_Tcenv_region_outlives(
te, _temp2032, _temp2060): 0;} _LL2049: return 1; _LL2051: goto _LL2045; _LL2045:;}
return 0; _LL2011:{ void* _temp2083= t2; struct Cyc_Absyn_Exp* _temp2089; struct
Cyc_Absyn_Tqual _temp2091; void* _temp2093; _LL2085: if(( unsigned int)
_temp2083 >  4u?*(( int*) _temp2083) ==  Cyc_Absyn_ArrayType: 0){ _LL2094:
_temp2093=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2083)->f1; goto
_LL2092; _LL2092: _temp2091=(( struct Cyc_Absyn_ArrayType_struct*) _temp2083)->f2;
goto _LL2090; _LL2090: _temp2089=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2083)->f3; goto _LL2086;} else{ goto _LL2087;} _LL2087: goto _LL2088;
_LL2086: { int okay; okay=( _temp2036 !=  0? _temp2089 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2036)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2089)): 0; return( okay? Cyc_Tcutil_unify(
_temp2040, _temp2093): 0)? ! _temp2038.q_const? 1: _temp2091.q_const: 0;}
_LL2088: return 0; _LL2084:;} return 0; _LL2013:{ void* _temp2095= t2; struct
Cyc_Absyn_Enumdecl* _temp2101; _LL2097: if(( unsigned int) _temp2095 >  4u?*((
int*) _temp2095) ==  Cyc_Absyn_EnumType: 0){ _LL2102: _temp2101=(( struct Cyc_Absyn_EnumType_struct*)
_temp2095)->f2; goto _LL2098;} else{ goto _LL2099;} _LL2099: goto _LL2100;
_LL2098: if(( _temp2042->fields !=  0? _temp2101->fields !=  0: 0)?(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2042->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2101->fields))->v):
0){ return 1;} goto _LL2096; _LL2100: goto _LL2096; _LL2096:;} goto _LL2015;
_LL2015: goto _LL2017; _LL2017: goto _LL2019; _LL2019: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType; _LL2021: return t2 == ( void*) Cyc_Absyn_VoidType;
_LL2023: return 0; _LL2007:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2103=
Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2104=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2104[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2105; _temp2105.tag= Cyc_Absyn_Cast_e;
_temp2105.f1=( void*) t; _temp2105.f2= _temp2103; _temp2105;}); _temp2104;})));
e->topt=({ struct Cyc_Core_Opt* _temp2106=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2106->v=( void*) t; _temp2106;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2107= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2109: if(( unsigned int) _temp2107
>  4u?*(( int*) _temp2107) ==  Cyc_Absyn_EnumType: 0){ goto _LL2110;} else{ goto
_LL2111;} _LL2111: if(( unsigned int) _temp2107 >  4u?*(( int*) _temp2107) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2112;} else{ goto _LL2113;} _LL2113: if((
unsigned int) _temp2107 >  4u?*(( int*) _temp2107) ==  Cyc_Absyn_IntType: 0){
goto _LL2114;} else{ goto _LL2115;} _LL2115: if(( unsigned int) _temp2107 >  4u?*((
int*) _temp2107) ==  Cyc_Absyn_Evar: 0){ goto _LL2116;} else{ goto _LL2117;}
_LL2117: goto _LL2118; _LL2110: goto _LL2112; _LL2112: goto _LL2114; _LL2114:
return 1; _LL2116: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2118: return 0; _LL2108:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2119= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2121: if( _temp2119 == ( void*) Cyc_Absyn_FloatType){
goto _LL2122;} else{ goto _LL2123;} _LL2123: if( _temp2119 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2124;} else{ goto _LL2125;} _LL2125: goto _LL2126; _LL2122: goto _LL2124;
_LL2124: return 1; _LL2126: return 0; _LL2120:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp2127= Cyc_Tcutil_compress( t); _LL2129: if(( unsigned int)
_temp2127 >  4u?*(( int*) _temp2127) ==  Cyc_Absyn_FnType: 0){ goto _LL2130;}
else{ goto _LL2131;} _LL2131: goto _LL2132; _LL2130: return 1; _LL2132: return 0;
_LL2128:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp2134=({ struct _tuple8 _temp2133; _temp2133.f1= t1; _temp2133.f2=
t2; _temp2133;}); void* _temp2158; void* _temp2160; void* _temp2162; void*
_temp2164; void* _temp2166; void* _temp2168; void* _temp2170; void* _temp2172;
void* _temp2174; void* _temp2176; void* _temp2178; void* _temp2180; void*
_temp2182; void* _temp2184; void* _temp2186; void* _temp2188; void* _temp2190;
void* _temp2192; void* _temp2194; void* _temp2196; void* _temp2198; void*
_temp2200; _LL2136: _LL2159: _temp2158= _temp2134.f1; if( _temp2158 == ( void*)
Cyc_Absyn_DoubleType){ goto _LL2137;} else{ goto _LL2138;} _LL2138: _LL2161:
_temp2160= _temp2134.f2; if( _temp2160 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2139;} else{ goto _LL2140;} _LL2140: _LL2163: _temp2162= _temp2134.f1; if(
_temp2162 == ( void*) Cyc_Absyn_FloatType){ goto _LL2141;} else{ goto _LL2142;}
_LL2142: _LL2165: _temp2164= _temp2134.f2; if( _temp2164 == ( void*) Cyc_Absyn_FloatType){
goto _LL2143;} else{ goto _LL2144;} _LL2144: _LL2167: _temp2166= _temp2134.f1;
if(( unsigned int) _temp2166 >  4u?*(( int*) _temp2166) ==  Cyc_Absyn_IntType: 0){
_LL2171: _temp2170=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2166)->f1;
if( _temp2170 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2169;} else{ goto _LL2146;}
_LL2169: _temp2168=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2166)->f2;
if( _temp2168 == ( void*) Cyc_Absyn_B8){ goto _LL2145;} else{ goto _LL2146;}}
else{ goto _LL2146;} _LL2146: _LL2173: _temp2172= _temp2134.f2; if((
unsigned int) _temp2172 >  4u?*(( int*) _temp2172) ==  Cyc_Absyn_IntType: 0){
_LL2177: _temp2176=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2172)->f1;
if( _temp2176 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2175;} else{ goto _LL2148;}
_LL2175: _temp2174=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2172)->f2;
if( _temp2174 == ( void*) Cyc_Absyn_B8){ goto _LL2147;} else{ goto _LL2148;}}
else{ goto _LL2148;} _LL2148: _LL2179: _temp2178= _temp2134.f1; if((
unsigned int) _temp2178 >  4u?*(( int*) _temp2178) ==  Cyc_Absyn_IntType: 0){
_LL2183: _temp2182=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2178)->f1;
if( _temp2182 == ( void*) Cyc_Absyn_Signed){ goto _LL2181;} else{ goto _LL2150;}
_LL2181: _temp2180=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2178)->f2;
if( _temp2180 == ( void*) Cyc_Absyn_B8){ goto _LL2149;} else{ goto _LL2150;}}
else{ goto _LL2150;} _LL2150: _LL2185: _temp2184= _temp2134.f2; if((
unsigned int) _temp2184 >  4u?*(( int*) _temp2184) ==  Cyc_Absyn_IntType: 0){
_LL2189: _temp2188=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2184)->f1;
if( _temp2188 == ( void*) Cyc_Absyn_Signed){ goto _LL2187;} else{ goto _LL2152;}
_LL2187: _temp2186=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2184)->f2;
if( _temp2186 == ( void*) Cyc_Absyn_B8){ goto _LL2151;} else{ goto _LL2152;}}
else{ goto _LL2152;} _LL2152: _LL2191: _temp2190= _temp2134.f1; if((
unsigned int) _temp2190 >  4u?*(( int*) _temp2190) ==  Cyc_Absyn_IntType: 0){
_LL2195: _temp2194=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2190)->f1;
if( _temp2194 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2193;} else{ goto _LL2154;}
_LL2193: _temp2192=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2190)->f2;
if( _temp2192 == ( void*) Cyc_Absyn_B4){ goto _LL2153;} else{ goto _LL2154;}}
else{ goto _LL2154;} _LL2154: _LL2197: _temp2196= _temp2134.f2; if((
unsigned int) _temp2196 >  4u?*(( int*) _temp2196) ==  Cyc_Absyn_IntType: 0){
_LL2201: _temp2200=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2196)->f1;
if( _temp2200 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2199;} else{ goto _LL2156;}
_LL2199: _temp2198=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2196)->f2;
if( _temp2198 == ( void*) Cyc_Absyn_B4){ goto _LL2155;} else{ goto _LL2156;}}
else{ goto _LL2156;} _LL2156: goto _LL2157; _LL2137: goto _LL2139; _LL2139:
return( void*) Cyc_Absyn_DoubleType; _LL2141: goto _LL2143; _LL2143: return(
void*) Cyc_Absyn_FloatType; _LL2145: goto _LL2147; _LL2147: return Cyc_Absyn_ulonglong_t;
_LL2149: goto _LL2151; _LL2151: return Cyc_Absyn_slonglong_t; _LL2153: goto
_LL2155; _LL2155: return Cyc_Absyn_uint_t; _LL2157: return Cyc_Absyn_sint_t;
_LL2135:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2202=( void*) e->r; struct Cyc_Core_Opt* _temp2208; _LL2204: if(*(( int*)
_temp2202) ==  Cyc_Absyn_AssignOp_e){ _LL2209: _temp2208=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2202)->f2; if( _temp2208 ==  0){ goto _LL2205;} else{ goto _LL2206;}} else{
goto _LL2206;} _LL2206: goto _LL2207; _LL2205:({ void* _temp2210[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2210, sizeof( void*), 0u));}); goto _LL2203; _LL2207: goto _LL2203; _LL2203:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2211=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2211[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2212; _temp2212.tag=
Cyc_Absyn_Forward_constr; _temp2212.f1= c2; _temp2212;}); _temp2211;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2213=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2213[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2214; _temp2214.tag= Cyc_Absyn_Forward_constr;
_temp2214.f1= c1; _temp2214;}); _temp2213;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2215=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2215[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2216; _temp2216.tag= Cyc_Absyn_Forward_constr; _temp2216.f1= c1; _temp2216;});
_temp2215;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2217=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2217[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2218; _temp2218.tag= Cyc_Absyn_Forward_constr;
_temp2218.f1= c2; _temp2218;}); _temp2217;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2219=( int*) _cycalloc_atomic( sizeof( int)); _temp2219[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2219;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2223;
_temp2223.tag= Cyc_Std_Int_pa; _temp2223.f1=( int)(( unsigned int) i);{ void*
_temp2222[ 1u]={& _temp2223}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2222, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2220=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2220->name=({ struct _tagged_arr* _temp2221=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2221[ 0]=
s; _temp2221;}); _temp2220->identity= 0; _temp2220->kind= Cyc_Absyn_new_conref(
k); _temp2220;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2224=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2224, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2226; _temp2226.tag= Cyc_Std_String_pa; _temp2226.f1=( struct _tagged_arr)*
t->name;{ void* _temp2225[ 1u]={& _temp2226}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2225, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2227= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2227, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2228=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2228[ 0]=( struct _tagged_arr) _temp2227; _temp2228;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2229=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2229->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2230=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2230->v=( void*)(*
x).f1; _temp2230;}); _temp2229->f2=(* x).f2; _temp2229->f3=(* x).f3; _temp2229;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2231= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts= atts->tl){ if( Cyc_Absyn_fntype_att(( void*) atts->hd)){
_temp2231=({ struct Cyc_List_List* _temp2232=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2232->hd=( void*)(( void*) atts->hd);
_temp2232->tl= _temp2231; _temp2232;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2233=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2233[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2234; _temp2234.tag= Cyc_Absyn_FnType;
_temp2234.f1=({ struct Cyc_Absyn_FnInfo _temp2235; _temp2235.tvars= fd->tvs;
_temp2235.effect= fd->effect; _temp2235.ret_typ=( void*)(( void*) fd->ret_type);
_temp2235.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2235.c_varargs= fd->c_varargs; _temp2235.cyc_varargs= fd->cyc_varargs;
_temp2235.rgn_po= fd->rgn_po; _temp2235.attributes= _temp2231; _temp2235;});
_temp2234;}); _temp2233;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp2236=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp2236->f1=(* pr).f1; _temp2236->f2= t; _temp2236;});} struct _tuple15{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct
_tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2237=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2237->f1=({ struct _tuple15* _temp2238=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2238->f1=(* y).f1; _temp2238->f2=(* y).f2;
_temp2238;}); _temp2237->f2=(* y).f3; _temp2237;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2241; struct _tuple15* _temp2243; struct
_tuple16 _temp2239=* w; _LL2244: _temp2243= _temp2239.f1; goto _LL2242; _LL2242:
_temp2241= _temp2239.f2; goto _LL2240; _LL2240: { struct Cyc_Absyn_Tqual
_temp2247; struct Cyc_Core_Opt* _temp2249; struct _tuple15 _temp2245=* _temp2243;
_LL2250: _temp2249= _temp2245.f1; goto _LL2248; _LL2248: _temp2247= _temp2245.f2;
goto _LL2246; _LL2246: return({ struct _tuple2* _temp2251=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2251->f1= _temp2249; _temp2251->f2=
_temp2247; _temp2251->f3= _temp2241; _temp2251;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2252=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2252->name= f->name;
_temp2252->tq= f->tq; _temp2252->type=( void*) t; _temp2252->width= f->width;
_temp2252->attributes= f->attributes; _temp2252;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2253= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2303; struct Cyc_Absyn_Structdecl** _temp2305;
struct Cyc_List_List* _temp2307; struct _tuple1* _temp2309; struct Cyc_Absyn_Uniondecl**
_temp2311; struct Cyc_List_List* _temp2313; struct _tuple1* _temp2315; struct
Cyc_Absyn_TunionInfo _temp2317; void* _temp2319; struct Cyc_List_List* _temp2321;
void* _temp2323; struct Cyc_Absyn_TunionFieldInfo _temp2325; struct Cyc_List_List*
_temp2327; void* _temp2329; struct Cyc_Core_Opt* _temp2331; struct Cyc_List_List*
_temp2333; struct _tuple1* _temp2335; struct Cyc_Absyn_Exp* _temp2337; struct
Cyc_Absyn_Tqual _temp2339; void* _temp2341; struct Cyc_Absyn_PtrInfo _temp2343;
struct Cyc_Absyn_Conref* _temp2345; struct Cyc_Absyn_Tqual _temp2347; struct Cyc_Absyn_Conref*
_temp2349; void* _temp2351; void* _temp2353; struct Cyc_Absyn_FnInfo _temp2355;
struct Cyc_List_List* _temp2357; struct Cyc_List_List* _temp2359; struct Cyc_Absyn_VarargInfo*
_temp2361; int _temp2363; struct Cyc_List_List* _temp2365; void* _temp2367;
struct Cyc_Core_Opt* _temp2369; struct Cyc_List_List* _temp2371; struct Cyc_List_List*
_temp2373; struct Cyc_List_List* _temp2375; struct Cyc_List_List* _temp2377;
struct Cyc_Core_Opt* _temp2379; void* _temp2381; void* _temp2383; void*
_temp2385; struct Cyc_List_List* _temp2387; _LL2255: if(( unsigned int)
_temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_VarType: 0){ _LL2304:
_temp2303=(( struct Cyc_Absyn_VarType_struct*) _temp2253)->f1; goto _LL2256;}
else{ goto _LL2257;} _LL2257: if(( unsigned int) _temp2253 >  4u?*(( int*)
_temp2253) ==  Cyc_Absyn_StructType: 0){ _LL2310: _temp2309=(( struct Cyc_Absyn_StructType_struct*)
_temp2253)->f1; goto _LL2308; _LL2308: _temp2307=(( struct Cyc_Absyn_StructType_struct*)
_temp2253)->f2; goto _LL2306; _LL2306: _temp2305=(( struct Cyc_Absyn_StructType_struct*)
_temp2253)->f3; goto _LL2258;} else{ goto _LL2259;} _LL2259: if(( unsigned int)
_temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_UnionType: 0){ _LL2316:
_temp2315=(( struct Cyc_Absyn_UnionType_struct*) _temp2253)->f1; goto _LL2314;
_LL2314: _temp2313=(( struct Cyc_Absyn_UnionType_struct*) _temp2253)->f2; goto
_LL2312; _LL2312: _temp2311=(( struct Cyc_Absyn_UnionType_struct*) _temp2253)->f3;
goto _LL2260;} else{ goto _LL2261;} _LL2261: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_TunionType: 0){ _LL2318: _temp2317=(( struct Cyc_Absyn_TunionType_struct*)
_temp2253)->f1; _LL2324: _temp2323=( void*) _temp2317.tunion_info; goto _LL2322;
_LL2322: _temp2321= _temp2317.targs; goto _LL2320; _LL2320: _temp2319=( void*)
_temp2317.rgn; goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int)
_temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2326:
_temp2325=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2253)->f1; _LL2330:
_temp2329=( void*) _temp2325.field_info; goto _LL2328; _LL2328: _temp2327=
_temp2325.targs; goto _LL2264;} else{ goto _LL2265;} _LL2265: if(( unsigned int)
_temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_TypedefType: 0){ _LL2336:
_temp2335=(( struct Cyc_Absyn_TypedefType_struct*) _temp2253)->f1; goto _LL2334;
_LL2334: _temp2333=(( struct Cyc_Absyn_TypedefType_struct*) _temp2253)->f2; goto
_LL2332; _LL2332: _temp2331=(( struct Cyc_Absyn_TypedefType_struct*) _temp2253)->f3;
goto _LL2266;} else{ goto _LL2267;} _LL2267: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_ArrayType: 0){ _LL2342: _temp2341=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2253)->f1; goto _LL2340; _LL2340:
_temp2339=(( struct Cyc_Absyn_ArrayType_struct*) _temp2253)->f2; goto _LL2338;
_LL2338: _temp2337=(( struct Cyc_Absyn_ArrayType_struct*) _temp2253)->f3; goto
_LL2268;} else{ goto _LL2269;} _LL2269: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_PointerType: 0){ _LL2344: _temp2343=(( struct Cyc_Absyn_PointerType_struct*)
_temp2253)->f1; _LL2354: _temp2353=( void*) _temp2343.elt_typ; goto _LL2352;
_LL2352: _temp2351=( void*) _temp2343.rgn_typ; goto _LL2350; _LL2350: _temp2349=
_temp2343.nullable; goto _LL2348; _LL2348: _temp2347= _temp2343.tq; goto _LL2346;
_LL2346: _temp2345= _temp2343.bounds; goto _LL2270;} else{ goto _LL2271;}
_LL2271: if(( unsigned int) _temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_FnType:
0){ _LL2356: _temp2355=(( struct Cyc_Absyn_FnType_struct*) _temp2253)->f1;
_LL2372: _temp2371= _temp2355.tvars; goto _LL2370; _LL2370: _temp2369= _temp2355.effect;
goto _LL2368; _LL2368: _temp2367=( void*) _temp2355.ret_typ; goto _LL2366;
_LL2366: _temp2365= _temp2355.args; goto _LL2364; _LL2364: _temp2363= _temp2355.c_varargs;
goto _LL2362; _LL2362: _temp2361= _temp2355.cyc_varargs; goto _LL2360; _LL2360:
_temp2359= _temp2355.rgn_po; goto _LL2358; _LL2358: _temp2357= _temp2355.attributes;
goto _LL2272;} else{ goto _LL2273;} _LL2273: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_TupleType: 0){ _LL2374: _temp2373=(( struct Cyc_Absyn_TupleType_struct*)
_temp2253)->f1; goto _LL2274;} else{ goto _LL2275;} _LL2275: if(( unsigned int)
_temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_AnonStructType: 0){ _LL2376:
_temp2375=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2253)->f1; goto
_LL2276;} else{ goto _LL2277;} _LL2277: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2378: _temp2377=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2253)->f1; goto _LL2278;} else{ goto
_LL2279;} _LL2279: if(( unsigned int) _temp2253 >  4u?*(( int*) _temp2253) == 
Cyc_Absyn_Evar: 0){ _LL2380: _temp2379=(( struct Cyc_Absyn_Evar_struct*)
_temp2253)->f2; goto _LL2280;} else{ goto _LL2281;} _LL2281: if(( unsigned int)
_temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2382:
_temp2381=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2253)->f1;
goto _LL2282;} else{ goto _LL2283;} _LL2283: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_EnumType: 0){ goto _LL2284;} else{ goto _LL2285;}
_LL2285: if(( unsigned int) _temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL2286;} else{ goto _LL2287;} _LL2287: if( _temp2253 == ( void*) Cyc_Absyn_VoidType){
goto _LL2288;} else{ goto _LL2289;} _LL2289: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_IntType: 0){ goto _LL2290;} else{ goto _LL2291;}
_LL2291: if( _temp2253 == ( void*) Cyc_Absyn_FloatType){ goto _LL2292;} else{
goto _LL2293;} _LL2293: if( _temp2253 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2294;} else{ goto _LL2295;} _LL2295: if( _temp2253 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL2296;} else{ goto _LL2297;} _LL2297: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_RgnsEff: 0){ _LL2384: _temp2383=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2253)->f1; goto _LL2298;} else{ goto _LL2299;}
_LL2299: if(( unsigned int) _temp2253 >  4u?*(( int*) _temp2253) ==  Cyc_Absyn_AccessEff:
0){ _LL2386: _temp2385=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2253)->f1;
goto _LL2300;} else{ goto _LL2301;} _LL2301: if(( unsigned int) _temp2253 >  4u?*((
int*) _temp2253) ==  Cyc_Absyn_JoinEff: 0){ _LL2388: _temp2387=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2253)->f1; goto _LL2302;} else{ goto _LL2254;} _LL2256: { struct
_handler_cons _temp2389; _push_handler(& _temp2389);{ int _temp2391= 0; if(
setjmp( _temp2389.handler)){ _temp2391= 1;} if( ! _temp2391){{ void* _temp2392=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2303); _npop_handler( 0u); return _temp2392;}; _pop_handler();} else{
void* _temp2390=( void*) _exn_thrown; void* _temp2394= _temp2390; _LL2396: if(
_temp2394 ==  Cyc_Core_Not_found){ goto _LL2397;} else{ goto _LL2398;} _LL2398:
goto _LL2399; _LL2397: return t; _LL2399:( void) _throw( _temp2394); _LL2395:;}}}
_LL2258: { struct Cyc_List_List* _temp2400= Cyc_Tcutil_substs( rgn, inst,
_temp2307); return _temp2400 ==  _temp2307? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2401=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2401[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2402; _temp2402.tag=
Cyc_Absyn_StructType; _temp2402.f1= _temp2309; _temp2402.f2= _temp2400;
_temp2402.f3= _temp2305; _temp2402;}); _temp2401;});} _LL2260: { struct Cyc_List_List*
_temp2403= Cyc_Tcutil_substs( rgn, inst, _temp2313); return _temp2403 == 
_temp2313? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2404=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2404[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2405; _temp2405.tag= Cyc_Absyn_UnionType;
_temp2405.f1= _temp2315; _temp2405.f2= _temp2403; _temp2405.f3= _temp2311;
_temp2405;}); _temp2404;});} _LL2262: { struct Cyc_List_List* _temp2406= Cyc_Tcutil_substs(
rgn, inst, _temp2321); void* _temp2407= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2319); return( _temp2406 ==  _temp2321? _temp2407 ==  _temp2319: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2408=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2408[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2409; _temp2409.tag= Cyc_Absyn_TunionType;
_temp2409.f1=({ struct Cyc_Absyn_TunionInfo _temp2410; _temp2410.tunion_info=(
void*) _temp2323; _temp2410.targs= _temp2406; _temp2410.rgn=( void*) _temp2407;
_temp2410;}); _temp2409;}); _temp2408;});} _LL2264: { struct Cyc_List_List*
_temp2411= Cyc_Tcutil_substs( rgn, inst, _temp2327); return _temp2411 == 
_temp2327? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2412=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2412[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2413; _temp2413.tag=
Cyc_Absyn_TunionFieldType; _temp2413.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2414; _temp2414.field_info=( void*) _temp2329; _temp2414.targs= _temp2411;
_temp2414;}); _temp2413;}); _temp2412;});} _LL2266: { struct Cyc_List_List*
_temp2415= Cyc_Tcutil_substs( rgn, inst, _temp2333); return _temp2415 == 
_temp2333? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2416=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2416[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2417; _temp2417.tag=
Cyc_Absyn_TypedefType; _temp2417.f1= _temp2335; _temp2417.f2= _temp2415;
_temp2417.f3= _temp2331; _temp2417;}); _temp2416;});} _LL2268: { void* _temp2418=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2341); return _temp2418 ==  _temp2341? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2419=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2419[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2420; _temp2420.tag= Cyc_Absyn_ArrayType;
_temp2420.f1=( void*) _temp2418; _temp2420.f2= _temp2339; _temp2420.f3=
_temp2337; _temp2420;}); _temp2419;});} _LL2270: { void* _temp2421= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2353); void* _temp2422= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2351); if( _temp2421 ==  _temp2353? _temp2422 ==  _temp2351: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2423=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2423[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2424; _temp2424.tag= Cyc_Absyn_PointerType;
_temp2424.f1=({ struct Cyc_Absyn_PtrInfo _temp2425; _temp2425.elt_typ=( void*)
_temp2421; _temp2425.rgn_typ=( void*) _temp2422; _temp2425.nullable= _temp2349;
_temp2425.tq= _temp2347; _temp2425.bounds= _temp2345; _temp2425;}); _temp2424;});
_temp2423;});} _LL2272:{ struct Cyc_List_List* _temp2426= _temp2371; for( 0;
_temp2426 !=  0; _temp2426= _temp2426->tl){ inst=({ struct Cyc_List_List*
_temp2427=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2427->hd=( void*)({ struct _tuple5* _temp2428=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2428->f1=( struct Cyc_Absyn_Tvar*)
_temp2426->hd; _temp2428->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2429=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2429[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2430; _temp2430.tag= Cyc_Absyn_VarType;
_temp2430.f1=( struct Cyc_Absyn_Tvar*) _temp2426->hd; _temp2430;}); _temp2429;});
_temp2428;}); _temp2427->tl= inst; _temp2427;});}}{ struct Cyc_List_List*
_temp2433; struct Cyc_List_List* _temp2435; struct _tuple0 _temp2431=(( struct
_tuple0(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp2365)); _LL2436: _temp2435= _temp2431.f1; goto _LL2434; _LL2434:
_temp2433= _temp2431.f2; goto _LL2432; _LL2432: { struct Cyc_List_List*
_temp2437= Cyc_Tcutil_substs( rgn, inst, _temp2433); struct Cyc_List_List*
_temp2438=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple16*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp2435, _temp2437));
struct Cyc_Core_Opt* eff2; if( _temp2369 ==  0){ eff2= 0;} else{ void* _temp2439=
Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp2369->v); if( _temp2439 == (
void*) _temp2369->v){ eff2= _temp2369;} else{ eff2=({ struct Cyc_Core_Opt*
_temp2440=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2440->v=( void*) _temp2439; _temp2440;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2361 ==  0){ cyc_varargs2= 0;} else{ int _temp2443; void*
_temp2445; struct Cyc_Absyn_Tqual _temp2447; struct Cyc_Core_Opt* _temp2449;
struct Cyc_Absyn_VarargInfo _temp2441=* _temp2361; _LL2450: _temp2449= _temp2441.name;
goto _LL2448; _LL2448: _temp2447= _temp2441.tq; goto _LL2446; _LL2446: _temp2445=(
void*) _temp2441.type; goto _LL2444; _LL2444: _temp2443= _temp2441.inject; goto
_LL2442; _LL2442: { void* _temp2451= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2445); if( _temp2451 ==  _temp2445){ cyc_varargs2= _temp2361;} else{
cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp2452=( struct Cyc_Absyn_VarargInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2452->name= _temp2449;
_temp2452->tq= _temp2447; _temp2452->type=( void*) _temp2451; _temp2452->inject=
_temp2443; _temp2452;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List*
_temp2455; struct Cyc_List_List* _temp2457; struct _tuple0 _temp2453= Cyc_List_rsplit(
rgn, rgn, _temp2359); _LL2458: _temp2457= _temp2453.f1; goto _LL2456; _LL2456:
_temp2455= _temp2453.f2; goto _LL2454; _LL2454: { struct Cyc_List_List*
_temp2459= Cyc_Tcutil_substs( rgn, inst, _temp2457); struct Cyc_List_List*
_temp2460= Cyc_Tcutil_substs( rgn, inst, _temp2455); if( _temp2459 ==  _temp2457?
_temp2460 ==  _temp2455: 0){ rgn_po2= _temp2359;} else{ rgn_po2= Cyc_List_zip(
_temp2459, _temp2460);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2461=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2461[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2462; _temp2462.tag= Cyc_Absyn_FnType;
_temp2462.f1=({ struct Cyc_Absyn_FnInfo _temp2463; _temp2463.tvars= _temp2371;
_temp2463.effect= eff2; _temp2463.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2367); _temp2463.args= _temp2438; _temp2463.c_varargs= _temp2363;
_temp2463.cyc_varargs= cyc_varargs2; _temp2463.rgn_po= rgn_po2; _temp2463.attributes=
_temp2357; _temp2463;}); _temp2462;}); _temp2461;});}}}}} _LL2274: { struct Cyc_List_List*
_temp2464=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2373); struct Cyc_List_List* _temp2465= Cyc_Tcutil_substs( rgn, inst,
_temp2464); if( _temp2465 ==  _temp2464){ return t;}{ struct Cyc_List_List*
_temp2466=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2373, _temp2465); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2467=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2467[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2468; _temp2468.tag= Cyc_Absyn_TupleType;
_temp2468.f1= _temp2466; _temp2468;}); _temp2467;});}} _LL2276: { struct Cyc_List_List*
_temp2469=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2375); struct Cyc_List_List* _temp2470= Cyc_Tcutil_substs( rgn, inst,
_temp2469); if( _temp2470 ==  _temp2469){ return t;}{ struct Cyc_List_List*
_temp2471=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2375, _temp2470); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2472=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2472[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2473; _temp2473.tag= Cyc_Absyn_AnonStructType;
_temp2473.f1= _temp2471; _temp2473;}); _temp2472;});}} _LL2278: { struct Cyc_List_List*
_temp2474=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2377); struct Cyc_List_List* _temp2475= Cyc_Tcutil_substs( rgn, inst,
_temp2474); if( _temp2475 ==  _temp2474){ return t;}{ struct Cyc_List_List*
_temp2476=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2377, _temp2475); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2477=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2477[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2478; _temp2478.tag= Cyc_Absyn_AnonStructType;
_temp2478.f1= _temp2476; _temp2478;}); _temp2477;});}} _LL2280: if( _temp2379 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp2379->v);} else{
return t;} _LL2282: { void* _temp2479= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2381); return _temp2479 ==  _temp2381? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2480=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2480[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2481; _temp2481.tag= Cyc_Absyn_RgnHandleType; _temp2481.f1=( void*)
_temp2479; _temp2481;}); _temp2480;});} _LL2284: return t; _LL2286: return t;
_LL2288: return t; _LL2290: return t; _LL2292: return t; _LL2294: return t;
_LL2296: return t; _LL2298: { void* _temp2482= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2383); return _temp2482 ==  _temp2383? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2483=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2483[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2484; _temp2484.tag= Cyc_Absyn_RgnsEff;
_temp2484.f1=( void*) _temp2482; _temp2484;}); _temp2483;});} _LL2300: { void*
_temp2485= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2385); return _temp2485 == 
_temp2385? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2486=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2486[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2487; _temp2487.tag= Cyc_Absyn_AccessEff;
_temp2487.f1=( void*) _temp2485; _temp2487;}); _temp2486;});} _LL2302: { struct
Cyc_List_List* _temp2488= Cyc_Tcutil_substs( rgn, inst, _temp2387); return
_temp2488 ==  _temp2387? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2489=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2489[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2490; _temp2490.tag= Cyc_Absyn_JoinEff;
_temp2490.f1= _temp2488; _temp2490;}); _temp2489;});} _LL2254:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2491=(
void*) ts->hd; struct Cyc_List_List* _temp2492= ts->tl; void* _temp2493= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2491); struct Cyc_List_List* _temp2494= Cyc_Tcutil_substs( rgn,
inst, _temp2492); if( _temp2491 ==  _temp2493? _temp2492 ==  _temp2494: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2495=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2495->hd=(
void*) _temp2493; _temp2495->tl= _temp2494; _temp2495;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2540=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2540[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2541; _temp2541.tag= Cyc_Absyn_Const_e; _temp2541.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2541;}); _temp2540;}), loc);{ void*
_temp2496= Cyc_Tcutil_compress( t); void* _temp2510; void* _temp2512; void*
_temp2514; void* _temp2516; _LL2498: if(( unsigned int) _temp2496 >  4u?*(( int*)
_temp2496) ==  Cyc_Absyn_PointerType: 0){ goto _LL2499;} else{ goto _LL2500;}
_LL2500: if(( unsigned int) _temp2496 >  4u?*(( int*) _temp2496) ==  Cyc_Absyn_IntType:
0){ _LL2513: _temp2512=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2496)->f1;
goto _LL2511; _LL2511: _temp2510=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2496)->f2; if( _temp2510 == ( void*) Cyc_Absyn_B1){ goto _LL2501;} else{
goto _LL2502;}} else{ goto _LL2502;} _LL2502: if(( unsigned int) _temp2496 >  4u?*((
int*) _temp2496) ==  Cyc_Absyn_IntType: 0){ _LL2517: _temp2516=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2496)->f1; goto _LL2515; _LL2515: _temp2514=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2496)->f2; goto _LL2503;} else{
goto _LL2504;} _LL2504: if( _temp2496 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2505;} else{ goto _LL2506;} _LL2506: if( _temp2496 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2507;} else{ goto _LL2508;} _LL2508: goto _LL2509; _LL2499: goto _LL2497;
_LL2501:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2518=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2518[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2519; _temp2519.tag= Cyc_Absyn_Const_e;
_temp2519.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2520=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2520[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2521; _temp2521.tag= Cyc_Absyn_Char_c;
_temp2521.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2521.f2='\000'; _temp2521;});
_temp2520;})); _temp2519;}); _temp2518;}))); goto _LL2497; _LL2503:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2522=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2522[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2523; _temp2523.tag= Cyc_Absyn_Const_e; _temp2523.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2524=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2524[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2525; _temp2525.tag= Cyc_Absyn_Int_c; _temp2525.f1=( void*) _temp2516;
_temp2525.f2= 0; _temp2525;}); _temp2524;})); _temp2523;}); _temp2522;}))); if(
_temp2514 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2526=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2526[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2527; _temp2527.tag= Cyc_Absyn_Cast_e;
_temp2527.f1=( void*) t; _temp2527.f2= e; _temp2527;}); _temp2526;}), loc);}
goto _LL2497; _LL2505:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2528=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2528[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2529; _temp2529.tag= Cyc_Absyn_Const_e;
_temp2529.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2530=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2530[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2531; _temp2531.tag= Cyc_Absyn_Float_c;
_temp2531.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2531;});
_temp2530;})); _temp2529;}); _temp2528;}))); goto _LL2497; _LL2507:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2532=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2532[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2533; _temp2533.tag= Cyc_Absyn_Cast_e; _temp2533.f1=( void*) t; _temp2533.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2534=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2534[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2535; _temp2535.tag= Cyc_Absyn_Const_e;
_temp2535.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2536=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2536[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2537; _temp2537.tag= Cyc_Absyn_Float_c;
_temp2537.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2537;});
_temp2536;})); _temp2535;}); _temp2534;}), loc); _temp2533;}); _temp2532;})));
goto _LL2497; _LL2509:({ struct Cyc_Std_String_pa_struct _temp2539; _temp2539.tag=
Cyc_Std_String_pa; _temp2539.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2538[ 1u]={& _temp2539}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2538, sizeof( void*), 1u));}});
goto _LL2497; _LL2497:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2542=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp2542->f1= tv; _temp2542->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2543=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2543->v=( void*) k; _temp2543;}),({ struct Cyc_Core_Opt*
_temp2544=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2544->v=( void*) s; _temp2544;})); _temp2542;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2547;
struct _RegionHandle* _temp2548; struct Cyc_List_List* _temp2550; struct _tuple6*
_temp2545= env; _temp2547=* _temp2545; _LL2551: _temp2550= _temp2547.f1; goto
_LL2549; _LL2549: _temp2548= _temp2547.f2; goto _LL2546; _LL2546: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2552=( struct _tuple5*) _region_malloc(
_temp2548, sizeof( struct _tuple5)); _temp2552->f1= tv; _temp2552->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2553=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2553->v=( void*) k; _temp2553;}),({ struct Cyc_Core_Opt*
_temp2554=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2554->v=( void*) _temp2550; _temp2554;})); _temp2552;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if( Cyc_Std_zstrptrcmp((( struct
Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind; struct Cyc_Absyn_Conref* k2= tv->kind;
if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({ struct Cyc_Std_String_pa_struct
_temp2558; _temp2558.tag= Cyc_Std_String_pa; _temp2558.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct _temp2557;
_temp2557.tag= Cyc_Std_String_pa; _temp2557.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
k1);{ struct Cyc_Std_String_pa_struct _temp2556; _temp2556.tag= Cyc_Std_String_pa;
_temp2556.f1=( struct _tagged_arr)* tv->name;{ void* _temp2555[ 3u]={& _temp2556,&
_temp2557,& _temp2558}; Cyc_Tcutil_terr( loc, _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u), _tag_arr( _temp2555, sizeof( void*), 3u));}}}});}
if( tv->identity ==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) != *(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity))){({ void* _temp2559[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2559, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2560=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2560->hd=( void*) tv; _temp2560->tl= tvs; _temp2560;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2561[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2561, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity ==  0){({ void* _temp2562[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u),
_tag_arr( _temp2562, sizeof( void*), 0u));});} if(*(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2563=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2563->hd=( void*) tv; _temp2563->tl=
tvs; _temp2563;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp2565; _temp2565.tag= Cyc_Std_String_pa;
_temp2565.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2564[
1u]={& _temp2565};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2564, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2566=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2566->hd=(
void*) tv; _temp2566->tl= tvs; _temp2566;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2567= Cyc_Tcutil_compress( e);
int _temp2573; _LL2569: if(( unsigned int) _temp2567 >  4u?*(( int*) _temp2567)
==  Cyc_Absyn_Evar: 0){ _LL2574: _temp2573=(( struct Cyc_Absyn_Evar_struct*)
_temp2567)->f3; goto _LL2570;} else{ goto _LL2571;} _LL2571: goto _LL2572;
_LL2570:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2= es2->tl){ void*
_temp2575= Cyc_Tcutil_compress(( void*) es2->hd); int _temp2581; _LL2577: if((
unsigned int) _temp2575 >  4u?*(( int*) _temp2575) ==  Cyc_Absyn_Evar: 0){
_LL2582: _temp2581=(( struct Cyc_Absyn_Evar_struct*) _temp2575)->f3; goto
_LL2578;} else{ goto _LL2579;} _LL2579: goto _LL2580; _LL2578: if( _temp2573 == 
_temp2581){ return es;} goto _LL2576; _LL2580: goto _LL2576; _LL2576:;}} return({
struct Cyc_List_List* _temp2583=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2583->hd=( void*) e; _temp2583->tl= es; _temp2583;});
_LL2572: return es; _LL2568:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs= tvs->tl){ int present= 0;{ struct Cyc_List_List* b=
btvs; for( 0; b !=  0; b= b->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)
tvs->hd)->identity)) == *(( int*) _check_null((( struct Cyc_Absyn_Tvar*) b->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2584=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2584->hd=(
void*)(( struct Cyc_Absyn_Tvar*) tvs->hd); _temp2584->tl= r; _temp2584;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp2586; _temp2586.tag= Cyc_Std_String_pa; _temp2586.f1=( struct _tagged_arr)*
fn;{ void* _temp2585[ 1u]={& _temp2586}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2585, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2587= Cyc_Tcutil_compress( field_typ); void* _temp2593;
_LL2589: if(( unsigned int) _temp2587 >  4u?*(( int*) _temp2587) ==  Cyc_Absyn_IntType:
0){ _LL2594: _temp2593=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2587)->f2;
goto _LL2590;} else{ goto _LL2591;} _LL2591: goto _LL2592; _LL2590:{ void*
_temp2595= _temp2593; _LL2597: if( _temp2595 == ( void*) Cyc_Absyn_B1){ goto
_LL2598;} else{ goto _LL2599;} _LL2599: if( _temp2595 == ( void*) Cyc_Absyn_B2){
goto _LL2600;} else{ goto _LL2601;} _LL2601: if( _temp2595 == ( void*) Cyc_Absyn_B4){
goto _LL2602;} else{ goto _LL2603;} _LL2603: if( _temp2595 == ( void*) Cyc_Absyn_B8){
goto _LL2604;} else{ goto _LL2596;} _LL2598: if( w >  8){({ void* _temp2605[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2605, sizeof( void*), 0u));});} goto
_LL2596; _LL2600: if( w >  16){({ void* _temp2606[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2606, sizeof( void*), 0u));});} goto _LL2596; _LL2602: if( w >  32){({ void*
_temp2607[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2607, sizeof( void*), 0u));});}
goto _LL2596; _LL2604: if( w >  64){({ void* _temp2608[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2608, sizeof( void*), 0u));});} goto _LL2596; _LL2596:;} goto
_LL2588; _LL2592:({ struct Cyc_Std_String_pa_struct _temp2611; _temp2611.tag=
Cyc_Std_String_pa; _temp2611.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp2610; _temp2610.tag= Cyc_Std_String_pa;
_temp2610.f1=( struct _tagged_arr)* fn;{ void* _temp2609[ 2u]={& _temp2610,&
_temp2611}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2609, sizeof( void*), 2u));}}});
goto _LL2588; _LL2588:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts= atts->tl){ void* _temp2612=( void*) atts->hd; _LL2614: if( _temp2612 == (
void*) Cyc_Absyn_Packed_att){ goto _LL2615;} else{ goto _LL2616;} _LL2616: if((
unsigned int) _temp2612 >  16u?*(( int*) _temp2612) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2617;} else{ goto _LL2618;} _LL2618: goto _LL2619; _LL2615: continue;
_LL2617: continue; _LL2619:({ struct Cyc_Std_String_pa_struct _temp2622;
_temp2622.tag= Cyc_Std_String_pa; _temp2622.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp2621; _temp2621.tag= Cyc_Std_String_pa;
_temp2621.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp2620[ 2u]={& _temp2621,& _temp2622}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2620, sizeof( void*), 2u));}}});
goto _LL2613; _LL2613:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; struct _tuple17{ void* f1; int f2; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp2623= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp2673; struct Cyc_Core_Opt** _temp2675; struct Cyc_Core_Opt* _temp2676;
struct Cyc_Core_Opt** _temp2678; struct Cyc_Absyn_Tvar* _temp2679; struct Cyc_List_List*
_temp2681; struct Cyc_Absyn_Enumdecl* _temp2683; struct Cyc_Absyn_Enumdecl**
_temp2685; struct _tuple1* _temp2686; struct Cyc_Absyn_TunionInfo _temp2688;
void* _temp2690; struct Cyc_List_List* _temp2692; void* _temp2694; void**
_temp2696; struct Cyc_Absyn_TunionFieldInfo _temp2697; struct Cyc_List_List*
_temp2699; void* _temp2701; void** _temp2703; struct Cyc_Absyn_PtrInfo _temp2704;
struct Cyc_Absyn_Conref* _temp2706; struct Cyc_Absyn_Tqual _temp2708; struct Cyc_Absyn_Conref*
_temp2710; void* _temp2712; void* _temp2714; struct Cyc_Absyn_Exp* _temp2716;
struct Cyc_Absyn_Tqual _temp2718; void* _temp2720; struct Cyc_Absyn_FnInfo
_temp2722; struct Cyc_List_List* _temp2724; struct Cyc_List_List* _temp2726;
struct Cyc_Absyn_VarargInfo* _temp2728; int _temp2730; struct Cyc_List_List*
_temp2732; void* _temp2734; struct Cyc_Core_Opt* _temp2736; struct Cyc_Core_Opt**
_temp2738; struct Cyc_List_List* _temp2739; struct Cyc_List_List** _temp2741;
struct Cyc_List_List* _temp2742; struct Cyc_List_List* _temp2744; struct Cyc_List_List*
_temp2746; struct Cyc_Absyn_Structdecl** _temp2748; struct Cyc_Absyn_Structdecl***
_temp2750; struct Cyc_List_List* _temp2751; struct _tuple1* _temp2753; struct
Cyc_Absyn_Uniondecl** _temp2755; struct Cyc_Absyn_Uniondecl*** _temp2757; struct
Cyc_List_List* _temp2758; struct _tuple1* _temp2760; struct Cyc_Core_Opt*
_temp2762; struct Cyc_Core_Opt** _temp2764; struct Cyc_List_List* _temp2765;
struct Cyc_List_List** _temp2767; struct _tuple1* _temp2768; void* _temp2770;
void* _temp2772; void* _temp2774; struct Cyc_List_List* _temp2776; _LL2625: if(
_temp2623 == ( void*) Cyc_Absyn_VoidType){ goto _LL2626;} else{ goto _LL2627;}
_LL2627: if(( unsigned int) _temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_Evar:
0){ _LL2677: _temp2676=(( struct Cyc_Absyn_Evar_struct*) _temp2623)->f1;
_temp2678=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2623)->f1;
goto _LL2674; _LL2674: _temp2673=(( struct Cyc_Absyn_Evar_struct*) _temp2623)->f2;
_temp2675=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2623)->f2;
goto _LL2628;} else{ goto _LL2629;} _LL2629: if(( unsigned int) _temp2623 >  4u?*((
int*) _temp2623) ==  Cyc_Absyn_VarType: 0){ _LL2680: _temp2679=(( struct Cyc_Absyn_VarType_struct*)
_temp2623)->f1; goto _LL2630;} else{ goto _LL2631;} _LL2631: if(( unsigned int)
_temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_AnonEnumType: 0){ _LL2682:
_temp2681=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp2623)->f1; goto _LL2632;}
else{ goto _LL2633;} _LL2633: if(( unsigned int) _temp2623 >  4u?*(( int*)
_temp2623) ==  Cyc_Absyn_EnumType: 0){ _LL2687: _temp2686=(( struct Cyc_Absyn_EnumType_struct*)
_temp2623)->f1; goto _LL2684; _LL2684: _temp2683=(( struct Cyc_Absyn_EnumType_struct*)
_temp2623)->f2; _temp2685=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2623)->f2; goto _LL2634;} else{ goto _LL2635;} _LL2635: if(( unsigned int)
_temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_TunionType: 0){ _LL2689:
_temp2688=(( struct Cyc_Absyn_TunionType_struct*) _temp2623)->f1; _LL2695:
_temp2694=( void*) _temp2688.tunion_info; _temp2696=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2623)->f1).tunion_info; goto _LL2693; _LL2693: _temp2692= _temp2688.targs;
goto _LL2691; _LL2691: _temp2690=( void*) _temp2688.rgn; goto _LL2636;} else{
goto _LL2637;} _LL2637: if(( unsigned int) _temp2623 >  4u?*(( int*) _temp2623)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2698: _temp2697=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2623)->f1; _LL2702: _temp2701=( void*) _temp2697.field_info; _temp2703=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2623)->f1).field_info;
goto _LL2700; _LL2700: _temp2699= _temp2697.targs; goto _LL2638;} else{ goto
_LL2639;} _LL2639: if(( unsigned int) _temp2623 >  4u?*(( int*) _temp2623) == 
Cyc_Absyn_PointerType: 0){ _LL2705: _temp2704=(( struct Cyc_Absyn_PointerType_struct*)
_temp2623)->f1; _LL2715: _temp2714=( void*) _temp2704.elt_typ; goto _LL2713;
_LL2713: _temp2712=( void*) _temp2704.rgn_typ; goto _LL2711; _LL2711: _temp2710=
_temp2704.nullable; goto _LL2709; _LL2709: _temp2708= _temp2704.tq; goto _LL2707;
_LL2707: _temp2706= _temp2704.bounds; goto _LL2640;} else{ goto _LL2641;}
_LL2641: if(( unsigned int) _temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_IntType:
0){ goto _LL2642;} else{ goto _LL2643;} _LL2643: if( _temp2623 == ( void*) Cyc_Absyn_FloatType){
goto _LL2644;} else{ goto _LL2645;} _LL2645: if( _temp2623 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2646;} else{ goto _LL2647;} _LL2647: if(( unsigned int) _temp2623 >  4u?*((
int*) _temp2623) ==  Cyc_Absyn_ArrayType: 0){ _LL2721: _temp2720=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2623)->f1; goto _LL2719; _LL2719:
_temp2718=(( struct Cyc_Absyn_ArrayType_struct*) _temp2623)->f2; goto _LL2717;
_LL2717: _temp2716=(( struct Cyc_Absyn_ArrayType_struct*) _temp2623)->f3; goto
_LL2648;} else{ goto _LL2649;} _LL2649: if(( unsigned int) _temp2623 >  4u?*((
int*) _temp2623) ==  Cyc_Absyn_FnType: 0){ _LL2723: _temp2722=(( struct Cyc_Absyn_FnType_struct*)
_temp2623)->f1; _LL2740: _temp2739= _temp2722.tvars; _temp2741=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2623)->f1).tvars; goto _LL2737; _LL2737:
_temp2736= _temp2722.effect; _temp2738=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2623)->f1).effect; goto _LL2735; _LL2735: _temp2734=( void*) _temp2722.ret_typ;
goto _LL2733; _LL2733: _temp2732= _temp2722.args; goto _LL2731; _LL2731:
_temp2730= _temp2722.c_varargs; goto _LL2729; _LL2729: _temp2728= _temp2722.cyc_varargs;
goto _LL2727; _LL2727: _temp2726= _temp2722.rgn_po; goto _LL2725; _LL2725:
_temp2724= _temp2722.attributes; goto _LL2650;} else{ goto _LL2651;} _LL2651:
if(( unsigned int) _temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_TupleType:
0){ _LL2743: _temp2742=(( struct Cyc_Absyn_TupleType_struct*) _temp2623)->f1;
goto _LL2652;} else{ goto _LL2653;} _LL2653: if(( unsigned int) _temp2623 >  4u?*((
int*) _temp2623) ==  Cyc_Absyn_AnonStructType: 0){ _LL2745: _temp2744=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2623)->f1; goto _LL2654;} else{ goto
_LL2655;} _LL2655: if(( unsigned int) _temp2623 >  4u?*(( int*) _temp2623) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2747: _temp2746=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2623)->f1; goto _LL2656;} else{ goto _LL2657;} _LL2657: if(( unsigned int)
_temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_StructType: 0){ _LL2754:
_temp2753=(( struct Cyc_Absyn_StructType_struct*) _temp2623)->f1; goto _LL2752;
_LL2752: _temp2751=(( struct Cyc_Absyn_StructType_struct*) _temp2623)->f2; goto
_LL2749; _LL2749: _temp2748=(( struct Cyc_Absyn_StructType_struct*) _temp2623)->f3;
_temp2750=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2623)->f3; goto _LL2658;} else{ goto _LL2659;} _LL2659: if(( unsigned int)
_temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_UnionType: 0){ _LL2761:
_temp2760=(( struct Cyc_Absyn_UnionType_struct*) _temp2623)->f1; goto _LL2759;
_LL2759: _temp2758=(( struct Cyc_Absyn_UnionType_struct*) _temp2623)->f2; goto
_LL2756; _LL2756: _temp2755=(( struct Cyc_Absyn_UnionType_struct*) _temp2623)->f3;
_temp2757=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2623)->f3; goto _LL2660;} else{ goto _LL2661;} _LL2661: if(( unsigned int)
_temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_TypedefType: 0){ _LL2769:
_temp2768=(( struct Cyc_Absyn_TypedefType_struct*) _temp2623)->f1; goto _LL2766;
_LL2766: _temp2765=(( struct Cyc_Absyn_TypedefType_struct*) _temp2623)->f2;
_temp2767=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2623)->f2; goto _LL2763; _LL2763: _temp2762=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2623)->f3; _temp2764=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2623)->f3; goto _LL2662;} else{ goto _LL2663;} _LL2663: if( _temp2623 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2664;} else{ goto _LL2665;} _LL2665: if((
unsigned int) _temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2771: _temp2770=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2623)->f1; goto _LL2666;} else{ goto _LL2667;} _LL2667: if(( unsigned int)
_temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_AccessEff: 0){ _LL2773:
_temp2772=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2623)->f1; goto
_LL2668;} else{ goto _LL2669;} _LL2669: if(( unsigned int) _temp2623 >  4u?*((
int*) _temp2623) ==  Cyc_Absyn_RgnsEff: 0){ _LL2775: _temp2774=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2623)->f1; goto _LL2670;} else{ goto _LL2671;}
_LL2671: if(( unsigned int) _temp2623 >  4u?*(( int*) _temp2623) ==  Cyc_Absyn_JoinEff:
0){ _LL2777: _temp2776=(( struct Cyc_Absyn_JoinEff_struct*) _temp2623)->f1; goto
_LL2672;} else{ goto _LL2624;} _LL2626: goto _LL2624; _LL2628: if(* _temp2678 == 
0){* _temp2678=({ struct Cyc_Core_Opt* _temp2778=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2778->v=( void*) expected_kind;
_temp2778;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2675=({ struct Cyc_Core_Opt* _temp2779=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2779->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2779;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2780= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2675=({ struct Cyc_Core_Opt* _temp2781=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2781->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2782=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2782[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2783; _temp2783.tag= Cyc_Absyn_VarType;
_temp2783.f1= _temp2780; _temp2783;}); _temp2782;})); _temp2781;}); _temp2679=
_temp2780; goto _LL2630;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2624; _LL2630: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2679->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2784=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2784[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2785; _temp2785.tag= Cyc_Absyn_Eq_constr;
_temp2785.f1=( void*) expected_kind; _temp2785;}); _temp2784;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2679); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2679); goto _LL2624;}
_LL2632: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);{ struct
_RegionHandle _temp2786= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp2786; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; for( 0; _temp2681 !=  0; _temp2681= _temp2681->tl){
struct Cyc_Absyn_Enumfield* _temp2787=( struct Cyc_Absyn_Enumfield*) _temp2681->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp,
prev_fields,(* _temp2787->name).f2)){({ struct Cyc_Std_String_pa_struct
_temp2789; _temp2789.tag= Cyc_Std_String_pa; _temp2789.f1=( struct _tagged_arr)*(*
_temp2787->name).f2;{ void* _temp2788[ 1u]={& _temp2789}; Cyc_Tcutil_terr(
_temp2787->loc, _tag_arr("duplicate enum field name %s", sizeof( unsigned char),
29u), _tag_arr( _temp2788, sizeof( void*), 1u));}});} else{ prev_fields=({
struct Cyc_List_List* _temp2790=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp2790->hd=( void*)(* _temp2787->name).f2;
_temp2790->tl= prev_fields; _temp2790;});} if( _temp2787->tag ==  0){ _temp2787->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp2787->loc);} else{
if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null(
_temp2787->tag))){({ struct Cyc_Std_String_pa_struct _temp2792; _temp2792.tag=
Cyc_Std_String_pa; _temp2792.f1=( struct _tagged_arr)*(* _temp2787->name).f2;{
void* _temp2791[ 1u]={& _temp2792}; Cyc_Tcutil_terr( loc, _tag_arr("enum field %s: expression is not constant",
sizeof( unsigned char), 42u), _tag_arr( _temp2791, sizeof( void*), 1u));}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp2787->tag)); tag_count= t1 +  1;(* _temp2787->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp2793=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp2793[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp2794; _temp2794.tag= Cyc_Absyn_Abs_n; _temp2794.f1= te->ns; _temp2794;});
_temp2793;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple17* v)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp2787->name).f2,({ struct _tuple17* _temp2795=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp2795->f1=( void*)({ struct Cyc_Tcenv_AnonEnumRes_struct*
_temp2796=( struct Cyc_Tcenv_AnonEnumRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_AnonEnumRes_struct));
_temp2796[ 0]=({ struct Cyc_Tcenv_AnonEnumRes_struct _temp2797; _temp2797.tag=
Cyc_Tcenv_AnonEnumRes; _temp2797.f1=( void*) t; _temp2797.f2= _temp2787;
_temp2797;}); _temp2796;}); _temp2795->f2= 1; _temp2795;}));}}}; _pop_region(
uprev_rgn);} goto _LL2624;} _LL2634: if(* _temp2685 ==  0){ struct _handler_cons
_temp2798; _push_handler(& _temp2798);{ int _temp2800= 0; if( setjmp( _temp2798.handler)){
_temp2800= 1;} if( ! _temp2800){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp2686);* _temp2685=( struct Cyc_Absyn_Enumdecl*)* ed;};
_pop_handler();} else{ void* _temp2799=( void*) _exn_thrown; void* _temp2802=
_temp2799; _LL2804: if( _temp2802 ==  Cyc_Dict_Absent){ goto _LL2805;} else{
goto _LL2806;} _LL2806: goto _LL2807; _LL2805: { struct Cyc_Tcenv_Genv*
_temp2808=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Enumdecl* _temp2809=({
struct Cyc_Absyn_Enumdecl* _temp2810=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp2810->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp2810->name= _temp2686; _temp2810->fields= 0; _temp2810;}); Cyc_Tc_tcEnumdecl(
te, _temp2808, loc, _temp2809);{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp2686);* _temp2685=( struct Cyc_Absyn_Enumdecl*)* ed; goto _LL2803;}}
_LL2807:( void) _throw( _temp2802); _LL2803:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2685);* _temp2686=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2624;} _LL2636:{ void*
_temp2811=* _temp2696; struct Cyc_Absyn_UnknownTunionInfo _temp2817; int
_temp2819; struct _tuple1* _temp2821; struct Cyc_Absyn_Tuniondecl** _temp2823;
struct Cyc_Absyn_Tuniondecl* _temp2825; _LL2813: if(*(( int*) _temp2811) ==  Cyc_Absyn_UnknownTunion){
_LL2818: _temp2817=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2811)->f1;
_LL2822: _temp2821= _temp2817.name; goto _LL2820; _LL2820: _temp2819= _temp2817.is_xtunion;
goto _LL2814;} else{ goto _LL2815;} _LL2815: if(*(( int*) _temp2811) ==  Cyc_Absyn_KnownTunion){
_LL2824: _temp2823=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2811)->f1;
_temp2825=* _temp2823; goto _LL2816;} else{ goto _LL2812;} _LL2814: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp2826; _push_handler(& _temp2826);{ int
_temp2828= 0; if( setjmp( _temp2826.handler)){ _temp2828= 1;} if( ! _temp2828){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp2821);; _pop_handler();} else{
void* _temp2827=( void*) _exn_thrown; void* _temp2830= _temp2827; _LL2832: if(
_temp2830 ==  Cyc_Dict_Absent){ goto _LL2833;} else{ goto _LL2834;} _LL2834:
goto _LL2835; _LL2833: { struct Cyc_Tcenv_Genv* _temp2836=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Tuniondecl* _temp2837=({ struct Cyc_Absyn_Tuniondecl* _temp2841=(
struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp2841->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp2841->name= _temp2821;
_temp2841->tvs= 0; _temp2841->fields= 0; _temp2841->is_xtunion= _temp2819;
_temp2841;}); Cyc_Tc_tcTuniondecl( te, _temp2836, loc, _temp2837); tudp= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2821); if( _temp2692 !=  0){({ struct Cyc_Std_String_pa_struct
_temp2840; _temp2840.tag= Cyc_Std_String_pa; _temp2840.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2821);{ struct Cyc_Std_String_pa_struct _temp2839;
_temp2839.tag= Cyc_Std_String_pa; _temp2839.f1=( struct _tagged_arr)( _temp2819?
_tag_arr("xtunion", sizeof( unsigned char), 8u): _tag_arr("tunion", sizeof(
unsigned char), 7u));{ void* _temp2838[ 2u]={& _temp2839,& _temp2840}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized %s %s before using", sizeof(
unsigned char), 48u), _tag_arr( _temp2838, sizeof( void*), 2u));}}}); return
cvtenv;} goto _LL2831;} _LL2835:( void) _throw( _temp2830); _LL2831:;}}} if((*
tudp)->is_xtunion !=  _temp2819){({ struct Cyc_Std_String_pa_struct _temp2843;
_temp2843.tag= Cyc_Std_String_pa; _temp2843.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2821);{ void* _temp2842[ 1u]={& _temp2843}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2842, sizeof( void*), 1u));}});}*
_temp2696=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2844=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2844[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2845; _temp2845.tag= Cyc_Absyn_KnownTunion;
_temp2845.f1= tudp; _temp2845;}); _temp2844;}); _temp2825=* tudp; goto _LL2816;}
_LL2816: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2690);{ struct Cyc_List_List* tvs= _temp2825->tvs; for( 0; _temp2692 !=  0?
tvs !=  0: 0;( _temp2692= _temp2692->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp2692->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*) tvs->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2692
!=  0){({ struct Cyc_Std_String_pa_struct _temp2847; _temp2847.tag= Cyc_Std_String_pa;
_temp2847.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2825->name);{
void* _temp2846[ 1u]={& _temp2847}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2846, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2849; _temp2849.tag= Cyc_Std_String_pa;
_temp2849.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2825->name);{
void* _temp2848[ 1u]={& _temp2849}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2848, sizeof( void*), 1u));}});}
goto _LL2812;} _LL2812:;} goto _LL2624; _LL2638:{ void* _temp2850=* _temp2703;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2856; int _temp2858; struct _tuple1*
_temp2860; struct _tuple1* _temp2862; struct Cyc_Absyn_Tunionfield* _temp2864;
struct Cyc_Absyn_Tuniondecl* _temp2866; _LL2852: if(*(( int*) _temp2850) ==  Cyc_Absyn_UnknownTunionfield){
_LL2857: _temp2856=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2850)->f1;
_LL2863: _temp2862= _temp2856.tunion_name; goto _LL2861; _LL2861: _temp2860=
_temp2856.field_name; goto _LL2859; _LL2859: _temp2858= _temp2856.is_xtunion;
goto _LL2853;} else{ goto _LL2854;} _LL2854: if(*(( int*) _temp2850) ==  Cyc_Absyn_KnownTunionfield){
_LL2867: _temp2866=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2850)->f1;
goto _LL2865; _LL2865: _temp2864=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2850)->f2; goto _LL2855;} else{ goto _LL2851;} _LL2853: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2868;
_push_handler(& _temp2868);{ int _temp2870= 0; if( setjmp( _temp2868.handler)){
_temp2870= 1;} if( ! _temp2870){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2862);; _pop_handler();} else{ void* _temp2869=( void*) _exn_thrown; void*
_temp2872= _temp2869; _LL2874: if( _temp2872 ==  Cyc_Dict_Absent){ goto _LL2875;}
else{ goto _LL2876;} _LL2876: goto _LL2877; _LL2875:({ struct Cyc_Std_String_pa_struct
_temp2879; _temp2879.tag= Cyc_Std_String_pa; _temp2879.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2862);{ void* _temp2878[ 1u]={& _temp2879}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2878, sizeof( void*), 1u));}}); return cvtenv; _LL2877:( void) _throw(
_temp2872); _LL2873:;}}}{ struct _handler_cons _temp2880; _push_handler(&
_temp2880);{ int _temp2882= 0; if( setjmp( _temp2880.handler)){ _temp2882= 1;}
if( ! _temp2882){{ void* _temp2883= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2860); struct Cyc_Absyn_Tunionfield* _temp2889; struct Cyc_Absyn_Tuniondecl*
_temp2891; _LL2885: if(*(( int*) _temp2883) ==  Cyc_Tcenv_TunionRes){ _LL2892:
_temp2891=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2883)->f1; goto _LL2890;
_LL2890: _temp2889=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2883)->f2; goto
_LL2886;} else{ goto _LL2887;} _LL2887: goto _LL2888; _LL2886: tuf= _temp2889;
tud= _temp2891; if( tud->is_xtunion !=  _temp2858){({ struct Cyc_Std_String_pa_struct
_temp2894; _temp2894.tag= Cyc_Std_String_pa; _temp2894.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2862);{ void* _temp2893[ 1u]={& _temp2894}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2893, sizeof( void*), 1u));}});} goto
_LL2884; _LL2888:({ struct Cyc_Std_String_pa_struct _temp2897; _temp2897.tag=
Cyc_Std_String_pa; _temp2897.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2862);{ struct Cyc_Std_String_pa_struct _temp2896; _temp2896.tag= Cyc_Std_String_pa;
_temp2896.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2860);{ void*
_temp2895[ 2u]={& _temp2896,& _temp2897}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2895, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2898= cvtenv; _npop_handler( 0u); return _temp2898;}
_LL2884:;}; _pop_handler();} else{ void* _temp2881=( void*) _exn_thrown; void*
_temp2900= _temp2881; _LL2902: if( _temp2900 ==  Cyc_Dict_Absent){ goto _LL2903;}
else{ goto _LL2904;} _LL2904: goto _LL2905; _LL2903:({ struct Cyc_Std_String_pa_struct
_temp2908; _temp2908.tag= Cyc_Std_String_pa; _temp2908.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2862);{ struct Cyc_Std_String_pa_struct _temp2907;
_temp2907.tag= Cyc_Std_String_pa; _temp2907.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2860);{ void* _temp2906[ 2u]={& _temp2907,& _temp2908}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2906, sizeof( void*), 2u));}}}); return cvtenv; _LL2905:( void)
_throw( _temp2900); _LL2901:;}}}* _temp2703=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2909=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2909[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2910; _temp2910.tag= Cyc_Absyn_KnownTunionfield; _temp2910.f1= tud;
_temp2910.f2= tuf; _temp2910;}); _temp2909;}); _temp2866= tud; _temp2864= tuf;
goto _LL2855;} _LL2855: { struct Cyc_List_List* tvs= _temp2866->tvs; for( 0;
_temp2699 !=  0? tvs !=  0: 0;( _temp2699= _temp2699->tl, tvs= tvs->tl)){ void*
t1=( void*) _temp2699->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)
tvs->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);}
if( _temp2699 !=  0){({ struct Cyc_Std_String_pa_struct _temp2913; _temp2913.tag=
Cyc_Std_String_pa; _temp2913.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2864->name);{ struct Cyc_Std_String_pa_struct _temp2912; _temp2912.tag= Cyc_Std_String_pa;
_temp2912.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2866->name);{
void* _temp2911[ 2u]={& _temp2912,& _temp2913}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2911, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2916; _temp2916.tag= Cyc_Std_String_pa;
_temp2916.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2864->name);{
struct Cyc_Std_String_pa_struct _temp2915; _temp2915.tag= Cyc_Std_String_pa;
_temp2915.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2866->name);{
void* _temp2914[ 2u]={& _temp2915,& _temp2916}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2914, sizeof( void*), 2u));}}});}
goto _LL2851;} _LL2851:;} goto _LL2624; _LL2640: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2714); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2712);{ void* _temp2917=( void*)(
Cyc_Absyn_compress_conref( _temp2706))->v; void* _temp2923; struct Cyc_Absyn_Exp*
_temp2925; _LL2919: if(( unsigned int) _temp2917 >  1u?*(( int*) _temp2917) == 
Cyc_Absyn_Eq_constr: 0){ _LL2924: _temp2923=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2917)->f1; if(( unsigned int) _temp2923 >  1u?*(( int*) _temp2923) ==  Cyc_Absyn_Upper_b:
0){ _LL2926: _temp2925=(( struct Cyc_Absyn_Upper_b_struct*) _temp2923)->f1; goto
_LL2920;} else{ goto _LL2921;}} else{ goto _LL2921;} _LL2921: goto _LL2922;
_LL2920: if( ! Cyc_Tcutil_is_const_exp( te, _temp2925)){({ void* _temp2927[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2927, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2925); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2925)){({ void*
_temp2928[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2928, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2925); goto _LL2918; _LL2922: goto _LL2918; _LL2918:;} goto _LL2624;
_LL2642: goto _LL2624; _LL2644: goto _LL2624; _LL2646: goto _LL2624; _LL2648:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2720); if( _temp2716 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2716))){({ void* _temp2929[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2929, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2716)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2716))){({ void* _temp2930[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2930, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2716)); goto _LL2624; _LL2650: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2724 !=  0; _temp2724= _temp2724->tl){ if(
! Cyc_Absyn_fntype_att(( void*) _temp2724->hd)){({ struct Cyc_Std_String_pa_struct
_temp2932; _temp2932.tag= Cyc_Std_String_pa; _temp2932.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*) _temp2724->hd);{ void* _temp2931[ 1u]={&
_temp2932}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2931, sizeof( void*), 1u));}});}{
void* _temp2933=( void*) _temp2724->hd; int _temp2945; int _temp2947; void*
_temp2949; _LL2935: if( _temp2933 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL2936;} else{ goto _LL2937;} _LL2937: if( _temp2933 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL2938;} else{ goto _LL2939;} _LL2939: if( _temp2933 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL2940;} else{ goto _LL2941;} _LL2941: if(( unsigned int) _temp2933 >  16u?*((
int*) _temp2933) ==  Cyc_Absyn_Format_att: 0){ _LL2950: _temp2949=( void*)((
struct Cyc_Absyn_Format_att_struct*) _temp2933)->f1; goto _LL2948; _LL2948:
_temp2947=(( struct Cyc_Absyn_Format_att_struct*) _temp2933)->f2; goto _LL2946;
_LL2946: _temp2945=(( struct Cyc_Absyn_Format_att_struct*) _temp2933)->f3; goto
_LL2942;} else{ goto _LL2943;} _LL2943: goto _LL2944; _LL2936: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2934; _LL2938: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2934; _LL2940: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2934; _LL2942: if( !
seen_format){ seen_format= 1; ft= _temp2949; fmt_desc_arg= _temp2947;
fmt_arg_start= _temp2945;} else{({ void* _temp2951[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2951, sizeof( void*), 0u));});} goto _LL2934; _LL2944: goto
_LL2934; _LL2934:;}} if( num_convs >  1){({ void* _temp2952[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2952, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2741);{ struct Cyc_List_List* b=* _temp2741; for( 0; b !=  0; b= b->tl){((
struct Cyc_Absyn_Tvar*) b->hd)->identity= Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env=
Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*) b->hd);{
void* _temp2953=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*) b->hd)->kind))->v;
void* _temp2959; _LL2955: if(( unsigned int) _temp2953 >  1u?*(( int*) _temp2953)
==  Cyc_Absyn_Eq_constr: 0){ _LL2960: _temp2959=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2953)->f1; if( _temp2959 == ( void*) Cyc_Absyn_MemKind){ goto _LL2956;}
else{ goto _LL2957;}} else{ goto _LL2957;} _LL2957: goto _LL2958; _LL2956:({
struct Cyc_Std_String_pa_struct _temp2962; _temp2962.tag= Cyc_Std_String_pa;
_temp2962.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) b->hd)->name;{
void* _temp2961[ 1u]={& _temp2962}; Cyc_Tcutil_terr( loc, _tag_arr("function attempts to abstract Mem type variable %s",
sizeof( unsigned char), 51u), _tag_arr( _temp2961, sizeof( void*), 1u));}});
goto _LL2954; _LL2958: goto _LL2954; _LL2954:;}}}{ struct Cyc_Tcutil_CVTEnv
_temp2963=({ struct Cyc_Tcutil_CVTEnv _temp3122; _temp3122.kind_env= cvtenv.kind_env;
_temp3122.free_vars= 0; _temp3122.free_evars= 0; _temp3122.generalize_evars=
cvtenv.generalize_evars; _temp3122.fn_result= 1; _temp3122;}); _temp2963= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2963,( void*) Cyc_Absyn_MemKind, _temp2734); _temp2963.fn_result=
0;{ struct Cyc_List_List* a= _temp2732; for( 0; a !=  0; a= a->tl){ _temp2963=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2963,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*) a->hd)).f3);}} if( _temp2728 !=  0){ if( _temp2730){({ void*
_temp2964[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2964, sizeof( void*), 0u));});}{ int _temp2967; void* _temp2969; struct Cyc_Absyn_Tqual
_temp2971; struct Cyc_Core_Opt* _temp2973; struct Cyc_Absyn_VarargInfo _temp2965=*
_temp2728; _LL2974: _temp2973= _temp2965.name; goto _LL2972; _LL2972: _temp2971=
_temp2965.tq; goto _LL2970; _LL2970: _temp2969=( void*) _temp2965.type; goto
_LL2968; _LL2968: _temp2967= _temp2965.inject; goto _LL2966; _LL2966: _temp2963=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2963,( void*) Cyc_Absyn_MemKind,
_temp2969); if( _temp2967){ void* _temp2975= Cyc_Tcutil_compress( _temp2969);
struct Cyc_Absyn_TunionInfo _temp2981; void* _temp2983; void* _temp2985; _LL2977:
if(( unsigned int) _temp2975 >  4u?*(( int*) _temp2975) ==  Cyc_Absyn_TunionType:
0){ _LL2982: _temp2981=(( struct Cyc_Absyn_TunionType_struct*) _temp2975)->f1;
_LL2986: _temp2985=( void*) _temp2981.tunion_info; if(*(( int*) _temp2985) == 
Cyc_Absyn_KnownTunion){ goto _LL2984;} else{ goto _LL2979;} _LL2984: _temp2983=(
void*) _temp2981.rgn; goto _LL2978;} else{ goto _LL2979;} _LL2979: goto _LL2980;
_LL2978: goto _LL2976; _LL2980:({ void* _temp2987[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("can't inject a non-[x]tunion type", sizeof( unsigned char), 34u),
_tag_arr( _temp2987, sizeof( void*), 0u));}); goto _LL2976; _LL2976:;}}} if(
seen_format){ int _temp2988=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2732); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg >  _temp2988)? 1:
fmt_arg_start <  0)? 1:( _temp2728 ==  0? fmt_arg_start !=  0: 0))? 1:(
_temp2728 !=  0? fmt_arg_start !=  _temp2988 +  1: 0)){({ void* _temp2989[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2989, sizeof( void*), 0u));});} else{ void* _temp2992;
struct _tuple2 _temp2990=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp2732, fmt_desc_arg -  1); _LL2993: _temp2992= _temp2990.f3;
goto _LL2991; _LL2991:{ void* _temp2994= Cyc_Tcutil_compress( _temp2992); struct
Cyc_Absyn_PtrInfo _temp3000; struct Cyc_Absyn_Conref* _temp3002; void* _temp3004;
_LL2996: if(( unsigned int) _temp2994 >  4u?*(( int*) _temp2994) ==  Cyc_Absyn_PointerType:
0){ _LL3001: _temp3000=(( struct Cyc_Absyn_PointerType_struct*) _temp2994)->f1;
_LL3005: _temp3004=( void*) _temp3000.elt_typ; goto _LL3003; _LL3003: _temp3002=
_temp3000.bounds; goto _LL2997;} else{ goto _LL2998;} _LL2998: goto _LL2999;
_LL2997:{ struct _tuple8 _temp3007=({ struct _tuple8 _temp3006; _temp3006.f1=
Cyc_Tcutil_compress( _temp3004); _temp3006.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp3002); _temp3006;}); void* _temp3013; void* _temp3015; void* _temp3017;
void* _temp3019; _LL3009: _LL3016: _temp3015= _temp3007.f1; if(( unsigned int)
_temp3015 >  4u?*(( int*) _temp3015) ==  Cyc_Absyn_IntType: 0){ _LL3020:
_temp3019=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3015)->f1; if(
_temp3019 == ( void*) Cyc_Absyn_Unsigned){ goto _LL3018;} else{ goto _LL3011;}
_LL3018: _temp3017=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3015)->f2;
if( _temp3017 == ( void*) Cyc_Absyn_B1){ goto _LL3014;} else{ goto _LL3011;}}
else{ goto _LL3011;} _LL3014: _temp3013= _temp3007.f2; if( _temp3013 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL3010;} else{ goto _LL3011;} _LL3011: goto _LL3012;
_LL3010: goto _LL3008; _LL3012:({ void* _temp3021[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp3021, sizeof( void*), 0u));}); goto _LL3008; _LL3008:;} goto
_LL2995; _LL2999:({ void* _temp3022[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp3022, sizeof( void*), 0u));}); goto
_LL2995; _LL2995:;} if( fmt_arg_start !=  0){ void* _temp3023= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2728))->type); int
problem;{ void* _temp3024= ft; _LL3026: if( _temp3024 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL3027;} else{ goto _LL3028;} _LL3028: if( _temp3024 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL3029;} else{ goto _LL3025;} _LL3027:{ void* _temp3030= _temp3023; struct
Cyc_Absyn_TunionInfo _temp3036; void* _temp3038; struct Cyc_Absyn_Tuniondecl**
_temp3040; struct Cyc_Absyn_Tuniondecl* _temp3042; _LL3032: if(( unsigned int)
_temp3030 >  4u?*(( int*) _temp3030) ==  Cyc_Absyn_TunionType: 0){ _LL3037:
_temp3036=(( struct Cyc_Absyn_TunionType_struct*) _temp3030)->f1; _LL3039:
_temp3038=( void*) _temp3036.tunion_info; if(*(( int*) _temp3038) ==  Cyc_Absyn_KnownTunion){
_LL3041: _temp3040=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3038)->f1;
_temp3042=* _temp3040; goto _LL3033;} else{ goto _LL3034;}} else{ goto _LL3034;}
_LL3034: goto _LL3035; _LL3033: problem= Cyc_Absyn_qvar_cmp( _temp3042->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL3031; _LL3035: problem= 1; goto
_LL3031; _LL3031:;} goto _LL3025; _LL3029:{ void* _temp3043= _temp3023; struct
Cyc_Absyn_TunionInfo _temp3049; void* _temp3051; struct Cyc_Absyn_Tuniondecl**
_temp3053; struct Cyc_Absyn_Tuniondecl* _temp3055; _LL3045: if(( unsigned int)
_temp3043 >  4u?*(( int*) _temp3043) ==  Cyc_Absyn_TunionType: 0){ _LL3050:
_temp3049=(( struct Cyc_Absyn_TunionType_struct*) _temp3043)->f1; _LL3052:
_temp3051=( void*) _temp3049.tunion_info; if(*(( int*) _temp3051) ==  Cyc_Absyn_KnownTunion){
_LL3054: _temp3053=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3051)->f1;
_temp3055=* _temp3053; goto _LL3046;} else{ goto _LL3047;}} else{ goto _LL3047;}
_LL3047: goto _LL3048; _LL3046: problem= Cyc_Absyn_qvar_cmp( _temp3055->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL3044; _LL3048: problem= 1; goto
_LL3044; _LL3044:;} goto _LL3025; _LL3025:;} if( problem){({ void* _temp3056[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3056, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2726; for( 0; rpo !=  0; rpo= rpo->tl){ struct
_tuple8 _temp3059; void* _temp3060; void* _temp3062; struct _tuple8* _temp3057=(
struct _tuple8*) rpo->hd; _temp3059=* _temp3057; _LL3063: _temp3062= _temp3059.f1;
goto _LL3061; _LL3061: _temp3060= _temp3059.f2; goto _LL3058; _LL3058: _temp2963=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2963,( void*) Cyc_Absyn_RgnKind,
_temp3062); _temp2963= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2963,( void*)
Cyc_Absyn_RgnKind, _temp3060);}} if(* _temp2738 !=  0){ _temp2963= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2963,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2738))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2963.free_vars; for(
0; tvs !=  0; tvs= tvs->tl){ void* _temp3064=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind))->v; void* _temp3072; void* _temp3074;
_LL3066: if(( unsigned int) _temp3064 >  1u?*(( int*) _temp3064) ==  Cyc_Absyn_Eq_constr:
0){ _LL3073: _temp3072=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3064)->f1;
if( _temp3072 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3067;} else{ goto _LL3068;}}
else{ goto _LL3068;} _LL3068: if(( unsigned int) _temp3064 >  1u?*(( int*)
_temp3064) ==  Cyc_Absyn_Eq_constr: 0){ _LL3075: _temp3074=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3064)->f1; if( _temp3074 == ( void*) Cyc_Absyn_EffKind){ goto _LL3069;}
else{ goto _LL3070;}} else{ goto _LL3070;} _LL3070: goto _LL3071; _LL3067:
effect=({ struct Cyc_List_List* _temp3076=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3076->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3077=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3077[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3078; _temp3078.tag= Cyc_Absyn_AccessEff;
_temp3078.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3079=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3079[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3080; _temp3080.tag= Cyc_Absyn_VarType;
_temp3080.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3080;}); _temp3079;}));
_temp3078;}); _temp3077;})); _temp3076->tl= effect; _temp3076;}); goto _LL3065;
_LL3069: effect=({ struct Cyc_List_List* _temp3081=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3081->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp3082=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3082[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3083; _temp3083.tag= Cyc_Absyn_VarType; _temp3083.f1=(
struct Cyc_Absyn_Tvar*) tvs->hd; _temp3083;}); _temp3082;})); _temp3081->tl=
effect; _temp3081;}); goto _LL3065; _LL3071: effect=({ struct Cyc_List_List*
_temp3084=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3084->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp3085=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3085[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3086; _temp3086.tag= Cyc_Absyn_RgnsEff;
_temp3086.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3087=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3087[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3088; _temp3088.tag= Cyc_Absyn_VarType;
_temp3088.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3088;}); _temp3087;}));
_temp3086;}); _temp3085;})); _temp3084->tl= effect; _temp3084;}); goto _LL3065;
_LL3065:;}} effect= Cyc_List_imp_rev( effect);{ struct Cyc_List_List* ts=
_temp2963.free_evars; for( 0; ts !=  0; ts= ts->tl){ void* _temp3089= Cyc_Tcutil_typ_kind((
void*) ts->hd); _LL3091: if( _temp3089 == ( void*) Cyc_Absyn_RgnKind){ goto
_LL3092;} else{ goto _LL3093;} _LL3093: if( _temp3089 == ( void*) Cyc_Absyn_EffKind){
goto _LL3094;} else{ goto _LL3095;} _LL3095: goto _LL3096; _LL3092: effect=({
struct Cyc_List_List* _temp3097=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3097->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3098=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3098[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3099; _temp3099.tag= Cyc_Absyn_AccessEff;
_temp3099.f1=( void*)(( void*) ts->hd); _temp3099;}); _temp3098;})); _temp3097->tl=
effect; _temp3097;}); goto _LL3090; _LL3094: effect=({ struct Cyc_List_List*
_temp3100=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3100->hd=( void*)(( void*) ts->hd); _temp3100->tl= effect; _temp3100;});
goto _LL3090; _LL3096: effect=({ struct Cyc_List_List* _temp3101=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3101->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3102=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3102[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3103; _temp3103.tag= Cyc_Absyn_RgnsEff; _temp3103.f1=(
void*)(( void*) ts->hd); _temp3103;}); _temp3102;})); _temp3101->tl= effect;
_temp3101;}); goto _LL3090; _LL3090:;}}* _temp2738=({ struct Cyc_Core_Opt*
_temp3104=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3104->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3105=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3105[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3106; _temp3106.tag= Cyc_Absyn_JoinEff;
_temp3106.f1= Cyc_List_imp_rev( effect); _temp3106;}); _temp3105;})); _temp3104;});}}
if(* _temp2741 !=  0){ struct Cyc_List_List* bs=* _temp2741; for( 0; bs !=  0;
bs= bs->tl){ struct Cyc_Absyn_Conref* _temp3107= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*) bs->hd)->kind); void* _temp3108=( void*) _temp3107->v;
void* _temp3116; _LL3110: if( _temp3108 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3111;} else{ goto _LL3112;} _LL3112: if(( unsigned int) _temp3108 >  1u?*((
int*) _temp3108) ==  Cyc_Absyn_Eq_constr: 0){ _LL3117: _temp3116=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3108)->f1; if( _temp3116 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3113;} else{ goto _LL3114;}} else{ goto _LL3114;}
_LL3114: goto _LL3115; _LL3111:({ struct Cyc_Std_String_pa_struct _temp3119;
_temp3119.tag= Cyc_Std_String_pa; _temp3119.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*) bs->hd)->name;{ void* _temp3118[ 1u]={& _temp3119}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3118, sizeof( void*), 1u));}}); goto
_LL3113; _LL3113:( void*)( _temp3107->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3120=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3120[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3121; _temp3121.tag= Cyc_Absyn_Eq_constr;
_temp3121.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3121;}); _temp3120;})));
goto _LL3109; _LL3115: goto _LL3109; _LL3109:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2963.kind_env,* _temp2741); _temp2963.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2963.free_vars,* _temp2741);{ struct Cyc_List_List* tvs= _temp2963.free_vars;
for( 0; tvs !=  0; tvs= tvs->tl){ cvtenv.free_vars= Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,( struct Cyc_Absyn_Tvar*) tvs->hd);}}{ struct Cyc_List_List*
evs= _temp2963.free_evars; for( 0; evs !=  0; evs= evs->tl){ cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars,( void*) evs->hd);}} goto _LL2624;}}
_LL2652: for( 0; _temp2742 !=  0; _temp2742= _temp2742->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp2742->hd)).f2);}
goto _LL2624; _LL2654:{ struct _RegionHandle _temp3123= _new_region(); struct
_RegionHandle* sprev_rgn=& _temp3123; _push_region( sprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2744 !=  0; _temp2744= _temp2744->tl){ struct Cyc_Absyn_Structfield
_temp3126; struct Cyc_List_List* _temp3127; struct Cyc_Absyn_Exp* _temp3129;
void* _temp3131; struct Cyc_Absyn_Tqual _temp3133; struct _tagged_arr* _temp3135;
struct Cyc_Absyn_Structfield* _temp3124=( struct Cyc_Absyn_Structfield*)
_temp2744->hd; _temp3126=* _temp3124; _LL3136: _temp3135= _temp3126.name; goto
_LL3134; _LL3134: _temp3133= _temp3126.tq; goto _LL3132; _LL3132: _temp3131=(
void*) _temp3126.type; goto _LL3130; _LL3130: _temp3129= _temp3126.width; goto
_LL3128; _LL3128: _temp3127= _temp3126.attributes; goto _LL3125; _LL3125: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3135)){({ struct Cyc_Std_String_pa_struct _temp3138; _temp3138.tag= Cyc_Std_String_pa;
_temp3138.f1=( struct _tagged_arr)* _temp3135;{ void* _temp3137[ 1u]={&
_temp3138}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3137, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3135, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3139=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3139->hd=( void*)
_temp3135; _temp3139->tl= prev_fields; _temp3139;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3131); Cyc_Tcutil_check_bitfield(
loc, te, _temp3131, _temp3129, _temp3135); Cyc_Tcutil_check_field_atts( loc,
_temp3135, _temp3127);}}; _pop_region( sprev_rgn);} goto _LL2624; _LL2656:{
struct _RegionHandle _temp3140= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3140; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2746 !=  0; _temp2746= _temp2746->tl){ struct Cyc_Absyn_Structfield
_temp3143; struct Cyc_List_List* _temp3144; struct Cyc_Absyn_Exp* _temp3146;
void* _temp3148; struct Cyc_Absyn_Tqual _temp3150; struct _tagged_arr* _temp3152;
struct Cyc_Absyn_Structfield* _temp3141=( struct Cyc_Absyn_Structfield*)
_temp2746->hd; _temp3143=* _temp3141; _LL3153: _temp3152= _temp3143.name; goto
_LL3151; _LL3151: _temp3150= _temp3143.tq; goto _LL3149; _LL3149: _temp3148=(
void*) _temp3143.type; goto _LL3147; _LL3147: _temp3146= _temp3143.width; goto
_LL3145; _LL3145: _temp3144= _temp3143.attributes; goto _LL3142; _LL3142: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3152)){({ struct Cyc_Std_String_pa_struct _temp3155; _temp3155.tag= Cyc_Std_String_pa;
_temp3155.f1=( struct _tagged_arr)* _temp3152;{ void* _temp3154[ 1u]={&
_temp3155}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3154, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3152, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3156=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3156->hd=( void*)
_temp3152; _temp3156->tl= prev_fields; _temp3156;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3148); if( ! Cyc_Tcutil_bits_only(
_temp3148)){({ struct Cyc_Std_String_pa_struct _temp3158; _temp3158.tag= Cyc_Std_String_pa;
_temp3158.f1=( struct _tagged_arr)* _temp3152;{ void* _temp3157[ 1u]={&
_temp3158}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3157, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3148, _temp3146, _temp3152); Cyc_Tcutil_check_field_atts(
loc, _temp3152, _temp3144);}}; _pop_region( uprev_rgn);} goto _LL2624; _LL2658:
if( _temp2753 ==  0){ if(* _temp2750 ==  0){({ void* _temp3159[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3159, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2750));;}} if(*
_temp2750 ==  0){ struct _handler_cons _temp3160; _push_handler(& _temp3160);{
int _temp3162= 0; if( setjmp( _temp3160.handler)){ _temp3162= 1;} if( !
_temp3162){* _temp2750=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2753));; _pop_handler();} else{
void* _temp3161=( void*) _exn_thrown; void* _temp3164= _temp3161; _LL3166: if(
_temp3164 ==  Cyc_Dict_Absent){ goto _LL3167;} else{ goto _LL3168;} _LL3168:
goto _LL3169; _LL3167: { struct _tuple1* tdn=( struct _tuple1*) _check_null(
_temp2753); struct Cyc_Tcenv_Genv* _temp3170=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Structdecl* _temp3171=({ struct Cyc_Absyn_Structdecl* _temp3174=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp3174->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3174->name=({ struct Cyc_Core_Opt*
_temp3175=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3175->v=( void*) tdn; _temp3175;}); _temp3174->tvs= 0; _temp3174->fields= 0;
_temp3174->attributes= 0; _temp3174;}); Cyc_Tc_tcStructdecl( te, _temp3170, loc,
_temp3171);* _temp2750=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc, tdn); if( _temp2751 !=  0){({ struct Cyc_Std_String_pa_struct _temp3173;
_temp3173.tag= Cyc_Std_String_pa; _temp3173.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tdn);{ void* _temp3172[ 1u]={& _temp3173}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized struct %s before using",
sizeof( unsigned char), 52u), _tag_arr( _temp3172, sizeof( void*), 1u));}});
return cvtenv;} goto _LL3165;} _LL3169:( void) _throw( _temp3164); _LL3165:;}}}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp2750)); if( sd->name !=  0){*(( struct _tuple1*) _check_null( _temp2753))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2751); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3179; _temp3179.tag= Cyc_Std_Int_pa; _temp3179.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3178; _temp3178.tag= Cyc_Std_Int_pa;
_temp3178.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3177; _temp3177.tag= Cyc_Std_String_pa; _temp3177.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2753));{ void*
_temp3176[ 3u]={& _temp3177,& _temp3178,& _temp3179}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3176, sizeof( void*), 3u));}}}});} for( 0;
_temp2751 !=  0;( _temp2751= _temp2751->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp2751->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2624;}} _LL2660: if( _temp2760 ==  0){({ void*
_temp3180[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("missing union name", sizeof(
unsigned char), 19u), _tag_arr( _temp3180, sizeof( void*), 0u));}); return
cvtenv;} if(* _temp2757 ==  0){ struct _handler_cons _temp3181; _push_handler(&
_temp3181);{ int _temp3183= 0; if( setjmp( _temp3181.handler)){ _temp3183= 1;}
if( ! _temp3183){* _temp2757=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2760));; _pop_handler();} else{
void* _temp3182=( void*) _exn_thrown; void* _temp3185= _temp3182; _LL3187: if(
_temp3185 ==  Cyc_Dict_Absent){ goto _LL3188;} else{ goto _LL3189;} _LL3189:
goto _LL3190; _LL3188: { struct _tuple1* _temp3191=( struct _tuple1*)
_check_null( _temp2760); struct Cyc_Tcenv_Genv* _temp3192=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Uniondecl* _temp3193=({ struct Cyc_Absyn_Uniondecl* _temp3196=(
struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp3196->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3196->name=({ struct Cyc_Core_Opt*
_temp3197=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3197->v=( void*) _temp3191; _temp3197;}); _temp3196->tvs= 0; _temp3196->fields=
0; _temp3196->attributes= 0; _temp3196;}); Cyc_Tc_tcUniondecl( te, _temp3192,
loc, _temp3193);* _temp2757=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc, _temp3191); if( _temp2758 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3195; _temp3195.tag= Cyc_Std_String_pa; _temp3195.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3191);{ void* _temp3194[ 1u]={& _temp3195}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized union %s before using", sizeof(
unsigned char), 51u), _tag_arr( _temp3194, sizeof( void*), 1u));}}); return
cvtenv;} goto _LL3186;} _LL3190:( void) _throw( _temp3185); _LL3186:;}}}{ struct
Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp2757)); if( ud->name !=  0){* _temp2760=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2758); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3201; _temp3201.tag= Cyc_Std_Int_pa; _temp3201.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3200; _temp3200.tag= Cyc_Std_Int_pa;
_temp3200.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3199; _temp3199.tag= Cyc_Std_String_pa; _temp3199.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2760));{ void*
_temp3198[ 3u]={& _temp3199,& _temp3200,& _temp3201}; Cyc_Tcutil_terr( loc,
_tag_arr("union %s expects %d type arguments but was given %d", sizeof(
unsigned char), 52u), _tag_arr( _temp3198, sizeof( void*), 3u));}}}});} for( 0;
_temp2758 !=  0;( _temp2758= _temp2758->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp2758->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2624;}} _LL2662: { struct Cyc_List_List*
targs=* _temp2767; struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons
_temp3202; _push_handler(& _temp3202);{ int _temp3204= 0; if( setjmp( _temp3202.handler)){
_temp3204= 1;} if( ! _temp3204){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp2768);; _pop_handler();} else{ void* _temp3203=( void*) _exn_thrown; void*
_temp3206= _temp3203; _LL3208: if( _temp3206 ==  Cyc_Dict_Absent){ goto _LL3209;}
else{ goto _LL3210;} _LL3210: goto _LL3211; _LL3209:({ struct Cyc_Std_String_pa_struct
_temp3213; _temp3213.tag= Cyc_Std_String_pa; _temp3213.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2768);{ void* _temp3212[ 1u]={& _temp3213}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3212, sizeof( void*), 1u));}}); return cvtenv; _LL3211:( void) _throw(
_temp3206); _LL3207:;}}} _temp2768[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts= ts->tl, tvs= tvs->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp3214=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3214->hd=(
void*)({ struct _tuple5* _temp3215=( struct _tuple5*) _cycalloc( sizeof( struct
_tuple5)); _temp3215->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3215->f2=( void*)
ts->hd; _temp3215;}); _temp3214->tl= inst; _temp3214;});} if( ts !=  0){({
struct Cyc_Std_String_pa_struct _temp3217; _temp3217.tag= Cyc_Std_String_pa;
_temp3217.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2768);{ void*
_temp3216[ 1u]={& _temp3217}; Cyc_Tcutil_terr( loc, _tag_arr("too many parameters for typedef %s",
sizeof( unsigned char), 35u), _tag_arr( _temp3216, sizeof( void*), 1u));}});}
if( tvs !=  0){ struct Cyc_List_List* hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*) tvs->hd)->kind); void*
e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({ struct Cyc_List_List* _temp3218=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3218->hd=(
void*) e; _temp3218->tl= hidden_ts; _temp3218;}); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k, e); inst=({ struct Cyc_List_List* _temp3219=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3219->hd=( void*)({ struct
_tuple5* _temp3220=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3220->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3220->f2= e; _temp3220;});
_temp3219->tl= inst; _temp3219;});}* _temp2767= Cyc_List_imp_append( targs, Cyc_List_imp_rev(
hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);*
_temp2764=({ struct Cyc_Core_Opt* _temp3221=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3221->v=( void*) new_typ; _temp3221;}); goto
_LL2624;}}} _LL2664: goto _LL2624; _LL2666: _temp2772= _temp2770; goto _LL2668;
_LL2668: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2772); goto _LL2624; _LL2670: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2774); goto _LL2624; _LL2672: for( 0;
_temp2776 !=  0; _temp2776= _temp2776->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*) _temp2776->hd);} goto
_LL2624; _LL2624:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){({ struct Cyc_Std_String_pa_struct _temp3225; _temp3225.tag= Cyc_Std_String_pa;
_temp3225.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_Std_String_pa_struct _temp3224; _temp3224.tag= Cyc_Std_String_pa;
_temp3224.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_Std_String_pa_struct _temp3223; _temp3223.tag= Cyc_Std_String_pa;
_temp3223.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3222[
3u]={& _temp3223,& _temp3224,& _temp3225}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3222, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3226= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3241; _temp3241.kind_env= kind_env; _temp3241.free_vars= 0; _temp3241.free_evars=
0; _temp3241.generalize_evars= generalize_evars; _temp3241.fn_result= 0;
_temp3241;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3226.free_vars;
for( 0; vs !=  0; vs= vs->tl){ _temp3226.kind_env= Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*) vs->hd);}}{ struct Cyc_List_List* evs=
_temp3226.free_evars; for( 0; evs !=  0; evs= evs->tl){ void* _temp3227= Cyc_Tcutil_compress((
void*) evs->hd); struct Cyc_Core_Opt* _temp3233; struct Cyc_Core_Opt** _temp3235;
_LL3229: if(( unsigned int) _temp3227 >  4u?*(( int*) _temp3227) ==  Cyc_Absyn_Evar:
0){ _LL3234: _temp3233=(( struct Cyc_Absyn_Evar_struct*) _temp3227)->f4;
_temp3235=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3227)->f4;
goto _LL3230;} else{ goto _LL3231;} _LL3231: goto _LL3232; _LL3230: if(*
_temp3235 ==  0){* _temp3235=({ struct Cyc_Core_Opt* _temp3236=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3236->v=( void*) kind_env;
_temp3236;});} else{ struct Cyc_List_List* _temp3237=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3235))->v; struct Cyc_List_List*
_temp3238= 0; for( 0; _temp3237 !=  0; _temp3237= _temp3237->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, kind_env,(
struct Cyc_Absyn_Tvar*) _temp3237->hd)){ _temp3238=({ struct Cyc_List_List*
_temp3239=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3239->hd=( void*)(( struct Cyc_Absyn_Tvar*) _temp3237->hd); _temp3239->tl=
_temp3238; _temp3239;});}}* _temp3235=({ struct Cyc_Core_Opt* _temp3240=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3240->v=( void*)
_temp3238; _temp3240;});} goto _LL3228; _LL3232: goto _LL3228; _LL3228:;}}
return _temp3226;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ int generalize_evars= Cyc_Tcutil_is_function_type(
t); struct Cyc_Tcutil_CVTEnv _temp3242= Cyc_Tcutil_check_valid_type( loc, te, 0,(
void*) Cyc_Absyn_MemKind, generalize_evars, t); struct Cyc_List_List* _temp3243=
_temp3242.free_vars; struct Cyc_List_List* _temp3244= _temp3242.free_evars;{
struct Cyc_List_List* x= _temp3243; for( 0; x !=  0; x= x->tl){ struct Cyc_Absyn_Conref*
c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*) x->hd)->kind); void*
_temp3245=( void*) c->v; void* _temp3253; _LL3247: if( _temp3245 == ( void*) Cyc_Absyn_No_constr){
goto _LL3248;} else{ goto _LL3249;} _LL3249: if(( unsigned int) _temp3245 >  1u?*((
int*) _temp3245) ==  Cyc_Absyn_Eq_constr: 0){ _LL3254: _temp3253=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3245)->f1; if( _temp3253 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3250;} else{ goto _LL3251;}} else{ goto _LL3251;}
_LL3251: goto _LL3252; _LL3248: goto _LL3250; _LL3250:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3255=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3255[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3256; _temp3256.tag= Cyc_Absyn_Eq_constr;
_temp3256.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3256;}); _temp3255;})));
goto _LL3246; _LL3252: goto _LL3246; _LL3246:;}} if( _temp3243 !=  0? 1:
_temp3244 !=  0){{ void* _temp3257= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3263; struct Cyc_List_List* _temp3265; struct Cyc_List_List* _temp3267;
struct Cyc_Absyn_VarargInfo* _temp3269; int _temp3271; struct Cyc_List_List*
_temp3273; void* _temp3275; struct Cyc_Core_Opt* _temp3277; struct Cyc_List_List*
_temp3279; struct Cyc_List_List** _temp3281; _LL3259: if(( unsigned int)
_temp3257 >  4u?*(( int*) _temp3257) ==  Cyc_Absyn_FnType: 0){ _LL3264:
_temp3263=(( struct Cyc_Absyn_FnType_struct*) _temp3257)->f1; _LL3280: _temp3279=
_temp3263.tvars; _temp3281=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3257)->f1).tvars; goto _LL3278; _LL3278: _temp3277= _temp3263.effect; goto
_LL3276; _LL3276: _temp3275=( void*) _temp3263.ret_typ; goto _LL3274; _LL3274:
_temp3273= _temp3263.args; goto _LL3272; _LL3272: _temp3271= _temp3263.c_varargs;
goto _LL3270; _LL3270: _temp3269= _temp3263.cyc_varargs; goto _LL3268; _LL3268:
_temp3267= _temp3263.rgn_po; goto _LL3266; _LL3266: _temp3265= _temp3263.attributes;
goto _LL3260;} else{ goto _LL3261;} _LL3261: goto _LL3262; _LL3260: if(*
_temp3281 ==  0){* _temp3281= _temp3243; _temp3243= 0;} goto _LL3258; _LL3262:
goto _LL3258; _LL3258:;} if( _temp3243 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3283; _temp3283.tag= Cyc_Std_String_pa; _temp3283.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*) _temp3243->hd)->name;{ void* _temp3282[ 1u]={& _temp3283};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s", sizeof( unsigned char),
25u), _tag_arr( _temp3282, sizeof( void*), 1u));}});} if( _temp3244 !=  0){ for(
0; _temp3244 !=  0; _temp3244= _temp3244->tl){ void* e=( void*) _temp3244->hd;
void* _temp3284= Cyc_Tcutil_typ_kind( e); _LL3286: if( _temp3284 == ( void*) Cyc_Absyn_RgnKind){
goto _LL3287;} else{ goto _LL3288;} _LL3288: if( _temp3284 == ( void*) Cyc_Absyn_EffKind){
goto _LL3289;} else{ goto _LL3290;} _LL3290: goto _LL3291; _LL3287: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3292[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3292, sizeof( void*), 0u));});}
goto _LL3285; _LL3289: if( ! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({
void* _temp3293[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!", sizeof( unsigned char),
26u), _tag_arr( _temp3293, sizeof( void*), 0u));});} goto _LL3285; _LL3291:({
struct Cyc_Std_String_pa_struct _temp3296; _temp3296.tag= Cyc_Std_String_pa;
_temp3296.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp3295; _temp3295.tag= Cyc_Std_String_pa; _temp3295.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( e);{ void* _temp3294[ 2u]={& _temp3295,& _temp3296}; Cyc_Tcutil_terr(
loc, _tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3294, sizeof( void*), 2u));}}}); goto
_LL3285; _LL3285:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3297= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3303; struct Cyc_List_List* _temp3305; struct
Cyc_List_List* _temp3307; struct Cyc_Absyn_VarargInfo* _temp3309; int _temp3311;
struct Cyc_List_List* _temp3313; void* _temp3315; struct Cyc_Core_Opt* _temp3317;
struct Cyc_List_List* _temp3319; _LL3299: if(( unsigned int) _temp3297 >  4u?*((
int*) _temp3297) ==  Cyc_Absyn_FnType: 0){ _LL3304: _temp3303=(( struct Cyc_Absyn_FnType_struct*)
_temp3297)->f1; _LL3320: _temp3319= _temp3303.tvars; goto _LL3318; _LL3318:
_temp3317= _temp3303.effect; goto _LL3316; _LL3316: _temp3315=( void*) _temp3303.ret_typ;
goto _LL3314; _LL3314: _temp3313= _temp3303.args; goto _LL3312; _LL3312:
_temp3311= _temp3303.c_varargs; goto _LL3310; _LL3310: _temp3309= _temp3303.cyc_varargs;
goto _LL3308; _LL3308: _temp3307= _temp3303.rgn_po; goto _LL3306; _LL3306:
_temp3305= _temp3303.attributes; goto _LL3300;} else{ goto _LL3301;} _LL3301:
goto _LL3302; _LL3300: fd->tvs= _temp3319; fd->effect= _temp3317; goto _LL3298;
_LL3302:({ void* _temp3321[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3321, sizeof( void*), 0u));});
return; _LL3298:;}{ struct _RegionHandle _temp3322= _new_region(); struct
_RegionHandle* r=& _temp3322; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3323=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3323->v=(
void*) t; _temp3323;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3324=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3325= Cyc_Tcutil_remove_bound_tvars( _temp3324.free_vars,
bound_tvars); struct Cyc_List_List* _temp3326= _temp3324.free_evars;{ struct Cyc_List_List*
fs= _temp3325; for( 0; fs !=  0; fs= fs->tl){ struct _tagged_arr* _temp3327=((
struct Cyc_Absyn_Tvar*) fs->hd)->name;({ struct Cyc_Std_String_pa_struct
_temp3330; _temp3330.tag= Cyc_Std_String_pa; _temp3330.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3329;
_temp3329.tag= Cyc_Std_String_pa; _temp3329.f1=( struct _tagged_arr)* _temp3327;{
void* _temp3328[ 2u]={& _temp3329,& _temp3330}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3328, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3326 !=  0: 0){ for( 0; _temp3326 !=  0; _temp3326=
_temp3326->tl){ void* e=( void*) _temp3326->hd; void* _temp3331= Cyc_Tcutil_typ_kind(
e); _LL3333: if( _temp3331 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3334;} else{
goto _LL3335;} _LL3335: if( _temp3331 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3336;} else{ goto _LL3337;} _LL3337: goto _LL3338; _LL3334: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3339[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3339, sizeof( void*), 0u));});}
goto _LL3332; _LL3336: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3340=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3340[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3341; _temp3341.tag= Cyc_Absyn_JoinEff;
_temp3341.f1= 0; _temp3341;}); _temp3340;}))){({ void* _temp3342[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3342, sizeof( void*), 0u));});}
goto _LL3332; _LL3338:({ struct Cyc_Std_String_pa_struct _temp3345; _temp3345.tag=
Cyc_Std_String_pa; _temp3345.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3344; _temp3344.tag= Cyc_Std_String_pa;
_temp3344.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3343[
2u]={& _temp3344,& _temp3345}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3343, sizeof( void*), 2u));}}});
goto _LL3332; _LL3332:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs= vs->tl){
struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 !=  0; vs2= vs2->tl){ if( cmp((
void*) vs->hd,( void*) vs2->hd) ==  0){({ struct Cyc_Std_String_pa_struct
_temp3348; _temp3348.tag= Cyc_Std_String_pa; _temp3348.f1=( struct _tagged_arr)
a2string(( void*) vs->hd);{ struct Cyc_Std_String_pa_struct _temp3347; _temp3347.tag=
Cyc_Std_String_pa; _temp3347.f1=( struct _tagged_arr) msg;{ void* _temp3346[ 2u]={&
_temp3347,& _temp3348}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3346, sizeof( void*), 2u));}}});}}}} static
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
_temp3349=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3349->hd=( void*)({ struct _tuple18* _temp3350=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp3350->f1=( struct Cyc_Absyn_Structfield*)
sd_fields->hd; _temp3350->f2= 0; _temp3350;}); _temp3349->tl= fields; _temp3349;});}}}
fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0; des= des->tl){ struct
_tuple19 _temp3353; void* _temp3354; struct Cyc_List_List* _temp3356; struct
_tuple19* _temp3351=( struct _tuple19*) des->hd; _temp3353=* _temp3351; _LL3357:
_temp3356= _temp3353.f1; goto _LL3355; _LL3355: _temp3354= _temp3353.f2; goto
_LL3352; _LL3352: if( _temp3356 ==  0){ struct Cyc_List_List* _temp3358= fields;
for( 0; _temp3358 !=  0; _temp3358= _temp3358->tl){ if( !(*(( struct _tuple18*)
_temp3358->hd)).f2){(*(( struct _tuple18*) _temp3358->hd)).f2= 1;(*(( struct
_tuple19*) des->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3359=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3359->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3360=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3360[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3361; _temp3361.tag= Cyc_Absyn_FieldName;
_temp3361.f1=((*(( struct _tuple18*) _temp3358->hd)).f1)->name; _temp3361;});
_temp3360;})); _temp3359->tl= 0; _temp3359;}); ans=({ struct Cyc_List_List*
_temp3362=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3362->hd=( void*)({ struct _tuple20* _temp3363=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp3363->f1=(*(( struct
_tuple18*) _temp3358->hd)).f1; _temp3363->f2= _temp3354; _temp3363;}); _temp3362->tl=
ans; _temp3362;}); break;}} if( _temp3358 ==  0){({ void* _temp3364[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u),
_tag_arr( _temp3364, sizeof( void*), 0u));});}} else{ if( _temp3356->tl !=  0){({
void* _temp3365[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3365, sizeof( void*), 0u));});}
else{ void* _temp3366=( void*) _temp3356->hd; struct _tagged_arr* _temp3372;
_LL3368: if(*(( int*) _temp3366) ==  Cyc_Absyn_ArrayElement){ goto _LL3369;}
else{ goto _LL3370;} _LL3370: if(*(( int*) _temp3366) ==  Cyc_Absyn_FieldName){
_LL3373: _temp3372=(( struct Cyc_Absyn_FieldName_struct*) _temp3366)->f1; goto
_LL3371;} else{ goto _LL3367;} _LL3369:({ void* _temp3374[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3374, sizeof( void*), 0u));}); goto _LL3367;
_LL3371: { struct Cyc_List_List* _temp3375= fields; for( 0; _temp3375 !=  0;
_temp3375= _temp3375->tl){ if( Cyc_Std_zstrptrcmp( _temp3372,((*(( struct
_tuple18*) _temp3375->hd)).f1)->name) ==  0){ if((*(( struct _tuple18*)
_temp3375->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3377; _temp3377.tag=
Cyc_Std_String_pa; _temp3377.f1=( struct _tagged_arr)* _temp3372;{ void*
_temp3376[ 1u]={& _temp3377}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3376, sizeof( void*), 1u));}});}(*((
struct _tuple18*) _temp3375->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp3378=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3378->hd=( void*)({ struct _tuple20* _temp3379=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp3379->f1=(*(( struct
_tuple18*) _temp3375->hd)).f1; _temp3379->f2= _temp3354; _temp3379;}); _temp3378->tl=
ans; _temp3378;}); break;}} if( _temp3375 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3381; _temp3381.tag= Cyc_Std_String_pa; _temp3381.f1=( struct _tagged_arr)*
_temp3372;{ void* _temp3380[ 1u]={& _temp3381}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3380, sizeof( void*), 1u));}});}
goto _LL3367;} _LL3367:;}}} for( 0; fields !=  0; fields= fields->tl){ if( !(*((
struct _tuple18*) fields->hd)).f2){({ void* _temp3382[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments to struct", sizeof( unsigned char), 28u),
_tag_arr( _temp3382, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3383= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3389; struct Cyc_Absyn_Conref* _temp3391; void* _temp3393;
_LL3385: if(( unsigned int) _temp3383 >  4u?*(( int*) _temp3383) ==  Cyc_Absyn_PointerType:
0){ _LL3390: _temp3389=(( struct Cyc_Absyn_PointerType_struct*) _temp3383)->f1;
_LL3394: _temp3393=( void*) _temp3389.elt_typ; goto _LL3392; _LL3392: _temp3391=
_temp3389.bounds; goto _LL3386;} else{ goto _LL3387;} _LL3387: goto _LL3388;
_LL3386: { struct Cyc_Absyn_Conref* _temp3395= Cyc_Absyn_compress_conref(
_temp3391); void* _temp3396=( void*)( Cyc_Absyn_compress_conref( _temp3395))->v;
void* _temp3404; _LL3398: if(( unsigned int) _temp3396 >  1u?*(( int*) _temp3396)
==  Cyc_Absyn_Eq_constr: 0){ _LL3405: _temp3404=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3396)->f1; if( _temp3404 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3399;}
else{ goto _LL3400;}} else{ goto _LL3400;} _LL3400: if( _temp3396 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3401;} else{ goto _LL3402;} _LL3402: goto _LL3403;
_LL3399:* elt_typ_dest= _temp3393; return 1; _LL3401:( void*)( _temp3395->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3406=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3406[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3407; _temp3407.tag= Cyc_Absyn_Eq_constr;
_temp3407.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3407;}); _temp3406;})));*
elt_typ_dest= _temp3393; return 1; _LL3403: return 0; _LL3397:;} _LL3388: return
0; _LL3384:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3590;
_temp3590.f1= 0; _temp3590.f2=( void*) Cyc_Absyn_HeapRgn; _temp3590;}); void*
_temp3408=( void*) e->r; void* _temp3422; struct _tuple1* _temp3424; struct
_tagged_arr* _temp3426; struct Cyc_Absyn_Exp* _temp3428; struct _tagged_arr*
_temp3430; struct Cyc_Absyn_Exp* _temp3432; struct Cyc_Absyn_Exp* _temp3434;
struct Cyc_Absyn_Exp* _temp3436; struct Cyc_Absyn_Exp* _temp3438; _LL3410: if(*((
int*) _temp3408) ==  Cyc_Absyn_Var_e){ _LL3425: _temp3424=(( struct Cyc_Absyn_Var_e_struct*)
_temp3408)->f1; goto _LL3423; _LL3423: _temp3422=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3408)->f2; goto _LL3411;} else{ goto _LL3412;} _LL3412: if(*(( int*)
_temp3408) ==  Cyc_Absyn_StructMember_e){ _LL3429: _temp3428=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3408)->f1; goto _LL3427; _LL3427: _temp3426=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3408)->f2; goto _LL3413;} else{ goto _LL3414;} _LL3414: if(*(( int*)
_temp3408) ==  Cyc_Absyn_StructArrow_e){ _LL3433: _temp3432=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3408)->f1; goto _LL3431; _LL3431: _temp3430=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3408)->f2; goto _LL3415;} else{ goto _LL3416;} _LL3416: if(*(( int*)
_temp3408) ==  Cyc_Absyn_Deref_e){ _LL3435: _temp3434=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3408)->f1; goto _LL3417;} else{ goto _LL3418;} _LL3418: if(*(( int*)
_temp3408) ==  Cyc_Absyn_Subscript_e){ _LL3439: _temp3438=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3408)->f1; goto _LL3437; _LL3437: _temp3436=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3408)->f2; goto _LL3419;} else{ goto _LL3420;} _LL3420: goto _LL3421;
_LL3411: { void* _temp3440= _temp3422; struct Cyc_Absyn_Vardecl* _temp3454;
struct Cyc_Absyn_Vardecl* _temp3456; struct Cyc_Absyn_Vardecl* _temp3458; struct
Cyc_Absyn_Vardecl* _temp3460; _LL3442: if( _temp3440 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3443;} else{ goto _LL3444;} _LL3444: if(( unsigned int) _temp3440 >  1u?*((
int*) _temp3440) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3445;} else{ goto _LL3446;}
_LL3446: if(( unsigned int) _temp3440 >  1u?*(( int*) _temp3440) ==  Cyc_Absyn_Global_b:
0){ _LL3455: _temp3454=(( struct Cyc_Absyn_Global_b_struct*) _temp3440)->f1;
goto _LL3447;} else{ goto _LL3448;} _LL3448: if(( unsigned int) _temp3440 >  1u?*((
int*) _temp3440) ==  Cyc_Absyn_Local_b: 0){ _LL3457: _temp3456=(( struct Cyc_Absyn_Local_b_struct*)
_temp3440)->f1; goto _LL3449;} else{ goto _LL3450;} _LL3450: if(( unsigned int)
_temp3440 >  1u?*(( int*) _temp3440) ==  Cyc_Absyn_Pat_b: 0){ _LL3459: _temp3458=((
struct Cyc_Absyn_Pat_b_struct*) _temp3440)->f1; goto _LL3451;} else{ goto
_LL3452;} _LL3452: if(( unsigned int) _temp3440 >  1u?*(( int*) _temp3440) == 
Cyc_Absyn_Param_b: 0){ _LL3461: _temp3460=(( struct Cyc_Absyn_Param_b_struct*)
_temp3440)->f1; goto _LL3453;} else{ goto _LL3441;} _LL3443: return bogus_ans;
_LL3445: return({ struct _tuple7 _temp3462; _temp3462.f1= 0; _temp3462.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3462;}); _LL3447: { void* _temp3463= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3465: if((
unsigned int) _temp3463 >  4u?*(( int*) _temp3463) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3466;} else{ goto _LL3467;} _LL3467: goto _LL3468; _LL3466: return({
struct _tuple7 _temp3469; _temp3469.f1= 1; _temp3469.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3469;}); _LL3468: return({ struct _tuple7 _temp3470; _temp3470.f1=(
_temp3454->tq).q_const; _temp3470.f2=( void*) Cyc_Absyn_HeapRgn; _temp3470;});
_LL3464:;} _LL3449: { void* _temp3471= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3473: if(( unsigned int) _temp3471 >  4u?*(( int*)
_temp3471) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3474;} else{ goto _LL3475;}
_LL3475: goto _LL3476; _LL3474: return({ struct _tuple7 _temp3477; _temp3477.f1=
1; _temp3477.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3456->rgn))->v;
_temp3477;}); _LL3476: return({ struct _tuple7 _temp3478; _temp3478.f1=(
_temp3456->tq).q_const; _temp3478.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3456->rgn))->v; _temp3478;}); _LL3472:;} _LL3451: _temp3460=
_temp3458; goto _LL3453; _LL3453: return({ struct _tuple7 _temp3479; _temp3479.f1=(
_temp3460->tq).q_const; _temp3479.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3460->rgn))->v; _temp3479;}); _LL3441:;} _LL3413: { void*
_temp3480= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3428->topt))->v); struct Cyc_List_List* _temp3492; struct Cyc_List_List*
_temp3494; struct Cyc_Absyn_Structdecl** _temp3496; struct Cyc_Absyn_Structdecl*
_temp3498; struct Cyc_Absyn_Uniondecl** _temp3499; struct Cyc_Absyn_Uniondecl*
_temp3501; _LL3482: if(( unsigned int) _temp3480 >  4u?*(( int*) _temp3480) == 
Cyc_Absyn_AnonStructType: 0){ _LL3493: _temp3492=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3480)->f1; goto _LL3483;} else{ goto _LL3484;} _LL3484: if(( unsigned int)
_temp3480 >  4u?*(( int*) _temp3480) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3495:
_temp3494=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3480)->f1; goto
_LL3485;} else{ goto _LL3486;} _LL3486: if(( unsigned int) _temp3480 >  4u?*((
int*) _temp3480) ==  Cyc_Absyn_StructType: 0){ _LL3497: _temp3496=(( struct Cyc_Absyn_StructType_struct*)
_temp3480)->f3; if( _temp3496 ==  0){ goto _LL3488;} else{ _temp3498=* _temp3496;
goto _LL3487;}} else{ goto _LL3488;} _LL3488: if(( unsigned int) _temp3480 >  4u?*((
int*) _temp3480) ==  Cyc_Absyn_UnionType: 0){ _LL3500: _temp3499=(( struct Cyc_Absyn_UnionType_struct*)
_temp3480)->f3; if( _temp3499 ==  0){ goto _LL3490;} else{ _temp3501=* _temp3499;
goto _LL3489;}} else{ goto _LL3490;} _LL3490: goto _LL3491; _LL3483: _temp3494=
_temp3492; goto _LL3485; _LL3485: { struct Cyc_Absyn_Structfield* _temp3502= Cyc_Absyn_lookup_field(
_temp3494, _temp3426); if( _temp3502 !=  0? _temp3502->width !=  0: 0){ return({
struct _tuple7 _temp3503; _temp3503.f1=( _temp3502->tq).q_const; _temp3503.f2=(
Cyc_Tcutil_addressof_props( te, _temp3428)).f2; _temp3503;});} return bogus_ans;}
_LL3487: { struct Cyc_Absyn_Structfield* _temp3504= Cyc_Absyn_lookup_struct_field(
_temp3498, _temp3426); if( _temp3504 !=  0? _temp3504->width !=  0: 0){ return({
struct _tuple7 _temp3505; _temp3505.f1=( _temp3504->tq).q_const; _temp3505.f2=(
Cyc_Tcutil_addressof_props( te, _temp3428)).f2; _temp3505;});} return bogus_ans;}
_LL3489: { struct Cyc_Absyn_Structfield* _temp3506= Cyc_Absyn_lookup_union_field(
_temp3501, _temp3426); if( _temp3506 !=  0){ return({ struct _tuple7 _temp3507;
_temp3507.f1=( _temp3506->tq).q_const; _temp3507.f2=( Cyc_Tcutil_addressof_props(
te, _temp3428)).f2; _temp3507;});} goto _LL3491;} _LL3491: return bogus_ans;
_LL3481:;} _LL3415: { void* _temp3508= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3432->topt))->v); struct Cyc_Absyn_PtrInfo _temp3514; void*
_temp3516; void* _temp3518; _LL3510: if(( unsigned int) _temp3508 >  4u?*(( int*)
_temp3508) ==  Cyc_Absyn_PointerType: 0){ _LL3515: _temp3514=(( struct Cyc_Absyn_PointerType_struct*)
_temp3508)->f1; _LL3519: _temp3518=( void*) _temp3514.elt_typ; goto _LL3517;
_LL3517: _temp3516=( void*) _temp3514.rgn_typ; goto _LL3511;} else{ goto _LL3512;}
_LL3512: goto _LL3513; _LL3511: { void* _temp3520= Cyc_Tcutil_compress(
_temp3518); struct Cyc_List_List* _temp3532; struct Cyc_List_List* _temp3534;
struct Cyc_Absyn_Structdecl** _temp3536; struct Cyc_Absyn_Structdecl* _temp3538;
struct Cyc_Absyn_Uniondecl** _temp3539; struct Cyc_Absyn_Uniondecl* _temp3541;
_LL3522: if(( unsigned int) _temp3520 >  4u?*(( int*) _temp3520) ==  Cyc_Absyn_AnonStructType:
0){ _LL3533: _temp3532=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3520)->f1;
goto _LL3523;} else{ goto _LL3524;} _LL3524: if(( unsigned int) _temp3520 >  4u?*((
int*) _temp3520) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3535: _temp3534=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3520)->f1; goto _LL3525;} else{ goto
_LL3526;} _LL3526: if(( unsigned int) _temp3520 >  4u?*(( int*) _temp3520) == 
Cyc_Absyn_StructType: 0){ _LL3537: _temp3536=(( struct Cyc_Absyn_StructType_struct*)
_temp3520)->f3; if( _temp3536 ==  0){ goto _LL3528;} else{ _temp3538=* _temp3536;
goto _LL3527;}} else{ goto _LL3528;} _LL3528: if(( unsigned int) _temp3520 >  4u?*((
int*) _temp3520) ==  Cyc_Absyn_UnionType: 0){ _LL3540: _temp3539=(( struct Cyc_Absyn_UnionType_struct*)
_temp3520)->f3; if( _temp3539 ==  0){ goto _LL3530;} else{ _temp3541=* _temp3539;
goto _LL3529;}} else{ goto _LL3530;} _LL3530: goto _LL3531; _LL3523: _temp3534=
_temp3532; goto _LL3525; _LL3525: { struct Cyc_Absyn_Structfield* _temp3542= Cyc_Absyn_lookup_field(
_temp3534, _temp3430); if( _temp3542 !=  0? _temp3542->width !=  0: 0){ return({
struct _tuple7 _temp3543; _temp3543.f1=( _temp3542->tq).q_const; _temp3543.f2=
_temp3516; _temp3543;});} return bogus_ans;} _LL3527: { struct Cyc_Absyn_Structfield*
_temp3544= Cyc_Absyn_lookup_struct_field( _temp3538, _temp3430); if( _temp3544
!=  0? _temp3544->width !=  0: 0){ return({ struct _tuple7 _temp3545; _temp3545.f1=(
_temp3544->tq).q_const; _temp3545.f2= _temp3516; _temp3545;});} return bogus_ans;}
_LL3529: { struct Cyc_Absyn_Structfield* _temp3546= Cyc_Absyn_lookup_union_field(
_temp3541, _temp3430); if( _temp3546 !=  0){ return({ struct _tuple7 _temp3547;
_temp3547.f1=( _temp3546->tq).q_const; _temp3547.f2= _temp3516; _temp3547;});}
return bogus_ans;} _LL3531: return bogus_ans; _LL3521:;} _LL3513: return
bogus_ans; _LL3509:;} _LL3417: { void* _temp3548= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3434->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3554; struct Cyc_Absyn_Tqual _temp3556; void* _temp3558; _LL3550: if((
unsigned int) _temp3548 >  4u?*(( int*) _temp3548) ==  Cyc_Absyn_PointerType: 0){
_LL3555: _temp3554=(( struct Cyc_Absyn_PointerType_struct*) _temp3548)->f1;
_LL3559: _temp3558=( void*) _temp3554.rgn_typ; goto _LL3557; _LL3557: _temp3556=
_temp3554.tq; goto _LL3551;} else{ goto _LL3552;} _LL3552: goto _LL3553; _LL3551:
return({ struct _tuple7 _temp3560; _temp3560.f1= _temp3556.q_const; _temp3560.f2=
_temp3558; _temp3560;}); _LL3553: return bogus_ans; _LL3549:;} _LL3419: { void*
t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3438->topt))->v);
void* _temp3561= t; struct Cyc_Absyn_Tqual _temp3571; struct Cyc_List_List*
_temp3573; struct Cyc_Absyn_PtrInfo _temp3575; struct Cyc_Absyn_Conref*
_temp3577; struct Cyc_Absyn_Tqual _temp3579; void* _temp3581; void* _temp3583;
_LL3563: if(( unsigned int) _temp3561 >  4u?*(( int*) _temp3561) ==  Cyc_Absyn_ArrayType:
0){ _LL3572: _temp3571=(( struct Cyc_Absyn_ArrayType_struct*) _temp3561)->f2;
goto _LL3564;} else{ goto _LL3565;} _LL3565: if(( unsigned int) _temp3561 >  4u?*((
int*) _temp3561) ==  Cyc_Absyn_TupleType: 0){ _LL3574: _temp3573=(( struct Cyc_Absyn_TupleType_struct*)
_temp3561)->f1; goto _LL3566;} else{ goto _LL3567;} _LL3567: if(( unsigned int)
_temp3561 >  4u?*(( int*) _temp3561) ==  Cyc_Absyn_PointerType: 0){ _LL3576:
_temp3575=(( struct Cyc_Absyn_PointerType_struct*) _temp3561)->f1; _LL3584:
_temp3583=( void*) _temp3575.elt_typ; goto _LL3582; _LL3582: _temp3581=( void*)
_temp3575.rgn_typ; goto _LL3580; _LL3580: _temp3579= _temp3575.tq; goto _LL3578;
_LL3578: _temp3577= _temp3575.bounds; goto _LL3568;} else{ goto _LL3569;}
_LL3569: goto _LL3570; _LL3564: return({ struct _tuple7 _temp3585; _temp3585.f1=
_temp3571.q_const; _temp3585.f2=( Cyc_Tcutil_addressof_props( te, _temp3438)).f2;
_temp3585;}); _LL3566: { struct _tuple4* _temp3586= Cyc_Absyn_lookup_tuple_field(
_temp3573,( int) Cyc_Evexp_eval_const_uint_exp( _temp3436)); if( _temp3586 !=  0){
return({ struct _tuple7 _temp3587; _temp3587.f1=((* _temp3586).f1).q_const;
_temp3587.f2=( Cyc_Tcutil_addressof_props( te, _temp3438)).f2; _temp3587;});}
return bogus_ans;} _LL3568: return({ struct _tuple7 _temp3588; _temp3588.f1=
_temp3579.q_const; _temp3588.f2= _temp3581; _temp3588;}); _LL3570: return
bogus_ans; _LL3562:;} _LL3421:({ void* _temp3589[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u), _tag_arr(
_temp3589, sizeof( void*), 0u));}); return bogus_ans; _LL3409:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3591= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Tqual _temp3597; void*
_temp3599; _LL3593: if(( unsigned int) _temp3591 >  4u?*(( int*) _temp3591) == 
Cyc_Absyn_ArrayType: 0){ _LL3600: _temp3599=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3591)->f1; goto _LL3598; _LL3598: _temp3597=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3591)->f2; goto _LL3594;} else{ goto _LL3595;} _LL3595: goto _LL3596;
_LL3594: { void* _temp3603; struct _tuple7 _temp3601= Cyc_Tcutil_addressof_props(
te, e); _LL3604: _temp3603= _temp3601.f2; goto _LL3602; _LL3602: return Cyc_Absyn_atb_typ(
_temp3599, _temp3603, _temp3597,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3605=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3605[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3606; _temp3606.tag= Cyc_Absyn_Upper_b;
_temp3606.f1= e; _temp3606;}); _temp3605;}));} _LL3596: return e_typ; _LL3592:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3607=(
void*) b->v; void* _temp3615; void* _temp3617; struct Cyc_Absyn_Exp* _temp3619;
_LL3609: if(( unsigned int) _temp3607 >  1u?*(( int*) _temp3607) ==  Cyc_Absyn_Eq_constr:
0){ _LL3616: _temp3615=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3607)->f1;
if( _temp3615 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3610;} else{ goto
_LL3611;}} else{ goto _LL3611;} _LL3611: if(( unsigned int) _temp3607 >  1u?*((
int*) _temp3607) ==  Cyc_Absyn_Eq_constr: 0){ _LL3618: _temp3617=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3607)->f1; if(( unsigned int) _temp3617
>  1u?*(( int*) _temp3617) ==  Cyc_Absyn_Upper_b: 0){ _LL3620: _temp3619=((
struct Cyc_Absyn_Upper_b_struct*) _temp3617)->f1; goto _LL3612;} else{ goto
_LL3613;}} else{ goto _LL3613;} _LL3613: goto _LL3614; _LL3610: return; _LL3612:
if( Cyc_Evexp_eval_const_uint_exp( _temp3619) <=  i){({ void* _temp3621[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3621, sizeof( void*), 0u));});} return;
_LL3614:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3622=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3622[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3623; _temp3623.tag= Cyc_Absyn_Eq_constr;
_temp3623.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3624=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3624[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3625; _temp3625.tag= Cyc_Absyn_Upper_b;
_temp3625.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3625;}); _temp3624;}));
_temp3623;}); _temp3622;}))); return; _LL3608:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp3626=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp3632; struct
Cyc_Absyn_Exp* _temp3634; _LL3628: if(( unsigned int) _temp3626 >  1u?*(( int*)
_temp3626) ==  Cyc_Absyn_Eq_constr: 0){ _LL3633: _temp3632=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3626)->f1; if(( unsigned int) _temp3632 >  1u?*(( int*) _temp3632) ==  Cyc_Absyn_Upper_b:
0){ _LL3635: _temp3634=(( struct Cyc_Absyn_Upper_b_struct*) _temp3632)->f1; goto
_LL3629;} else{ goto _LL3630;}} else{ goto _LL3630;} _LL3630: goto _LL3631;
_LL3629: return Cyc_Evexp_eval_const_uint_exp( _temp3634) ==  1; _LL3631: return
0; _LL3627:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp3636= Cyc_Tcutil_compress(
t); void* _temp3664; struct Cyc_List_List* _temp3666; struct Cyc_Absyn_Structdecl**
_temp3668; struct Cyc_List_List* _temp3670; struct Cyc_Absyn_Uniondecl**
_temp3672; struct Cyc_List_List* _temp3674; struct Cyc_List_List* _temp3676;
struct Cyc_List_List* _temp3678; _LL3638: if( _temp3636 == ( void*) Cyc_Absyn_VoidType){
goto _LL3639;} else{ goto _LL3640;} _LL3640: if(( unsigned int) _temp3636 >  4u?*((
int*) _temp3636) ==  Cyc_Absyn_IntType: 0){ goto _LL3641;} else{ goto _LL3642;}
_LL3642: if( _temp3636 == ( void*) Cyc_Absyn_FloatType){ goto _LL3643;} else{
goto _LL3644;} _LL3644: if( _temp3636 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3645;} else{ goto _LL3646;} _LL3646: if(( unsigned int) _temp3636 >  4u?*((
int*) _temp3636) ==  Cyc_Absyn_EnumType: 0){ goto _LL3647;} else{ goto _LL3648;}
_LL3648: if(( unsigned int) _temp3636 >  4u?*(( int*) _temp3636) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL3649;} else{ goto _LL3650;} _LL3650: if(( unsigned int) _temp3636 > 
4u?*(( int*) _temp3636) ==  Cyc_Absyn_ArrayType: 0){ _LL3665: _temp3664=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3636)->f1; goto _LL3651;} else{ goto
_LL3652;} _LL3652: if(( unsigned int) _temp3636 >  4u?*(( int*) _temp3636) == 
Cyc_Absyn_TupleType: 0){ _LL3667: _temp3666=(( struct Cyc_Absyn_TupleType_struct*)
_temp3636)->f1; goto _LL3653;} else{ goto _LL3654;} _LL3654: if(( unsigned int)
_temp3636 >  4u?*(( int*) _temp3636) ==  Cyc_Absyn_StructType: 0){ _LL3671:
_temp3670=(( struct Cyc_Absyn_StructType_struct*) _temp3636)->f2; goto _LL3669;
_LL3669: _temp3668=(( struct Cyc_Absyn_StructType_struct*) _temp3636)->f3; goto
_LL3655;} else{ goto _LL3656;} _LL3656: if(( unsigned int) _temp3636 >  4u?*((
int*) _temp3636) ==  Cyc_Absyn_UnionType: 0){ _LL3675: _temp3674=(( struct Cyc_Absyn_UnionType_struct*)
_temp3636)->f2; goto _LL3673; _LL3673: _temp3672=(( struct Cyc_Absyn_UnionType_struct*)
_temp3636)->f3; goto _LL3657;} else{ goto _LL3658;} _LL3658: if(( unsigned int)
_temp3636 >  4u?*(( int*) _temp3636) ==  Cyc_Absyn_AnonStructType: 0){ _LL3677:
_temp3676=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3636)->f1; goto
_LL3659;} else{ goto _LL3660;} _LL3660: if(( unsigned int) _temp3636 >  4u?*((
int*) _temp3636) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3679: _temp3678=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3636)->f1; goto _LL3661;} else{ goto
_LL3662;} _LL3662: goto _LL3663; _LL3639: goto _LL3641; _LL3641: goto _LL3643;
_LL3643: goto _LL3645; _LL3645: return 1; _LL3647: return 0; _LL3649: return 0;
_LL3651: return Cyc_Tcutil_bits_only( _temp3664); _LL3653: for( 0; _temp3666 != 
0; _temp3666= _temp3666->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)
_temp3666->hd)).f2)){ return 0;}} return 1; _LL3655: if( _temp3668 ==  0){
return 0;}{ struct Cyc_Absyn_Structdecl* _temp3680=* _temp3668; if( _temp3680->fields
==  0){ return 0;}{ struct _RegionHandle _temp3681= _new_region(); struct
_RegionHandle* rgn=& _temp3681; _push_region( rgn);{ struct Cyc_List_List*
_temp3682=(( struct Cyc_List_List*(*)( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3680->tvs, _temp3670);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3680->fields))->v; for( 0; fs !=  0; fs=
fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn, _temp3682,(
void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ int _temp3683= 0;
_npop_handler( 0u); return _temp3683;}}}{ int _temp3684= 1; _npop_handler( 0u);
return _temp3684;}}; _pop_region( rgn);}} _LL3657: if( _temp3672 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3685=* _temp3672; if( _temp3685->fields == 
0){ return 0;}{ struct _RegionHandle _temp3686= _new_region(); struct
_RegionHandle* rgn=& _temp3686; _push_region( rgn);{ struct Cyc_List_List*
_temp3687=(( struct Cyc_List_List*(*)( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3685->tvs, _temp3674);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3685->fields))->v; for( 0; fs !=  0; fs=
fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn, _temp3687,(
void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ int _temp3688= 0;
_npop_handler( 0u); return _temp3688;}}}{ int _temp3689= 1; _npop_handler( 0u);
return _temp3689;}}; _pop_region( rgn);}} _LL3659: _temp3678= _temp3676; goto
_LL3661; _LL3661: for( 0; _temp3678 !=  0; _temp3678= _temp3678->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*) _temp3678->hd)->type)){ return 0;}}
return 1; _LL3663: return 0; _LL3637:;} struct _tuple21{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3690=( void*) e->r;
struct _tuple1* _temp3736; struct Cyc_Absyn_Exp* _temp3738; struct Cyc_Absyn_Exp*
_temp3740; struct Cyc_Absyn_Exp* _temp3742; struct Cyc_Absyn_Exp* _temp3744;
struct Cyc_Absyn_Exp* _temp3746; struct Cyc_Absyn_Exp* _temp3748; struct Cyc_Absyn_Exp*
_temp3750; struct Cyc_Absyn_Exp* _temp3752; void* _temp3754; struct Cyc_Absyn_Exp*
_temp3756; struct Cyc_Absyn_Exp* _temp3758; struct Cyc_Absyn_Exp* _temp3760;
struct Cyc_List_List* _temp3762; struct Cyc_List_List* _temp3764; struct Cyc_List_List*
_temp3766; struct Cyc_List_List* _temp3768; void* _temp3770; struct Cyc_List_List*
_temp3772; struct Cyc_List_List* _temp3774; _LL3692: if(*(( int*) _temp3690) == 
Cyc_Absyn_Const_e){ goto _LL3693;} else{ goto _LL3694;} _LL3694: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3695;} else{ goto _LL3696;}
_LL3696: if(*(( int*) _temp3690) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3697;}
else{ goto _LL3698;} _LL3698: if(*(( int*) _temp3690) ==  Cyc_Absyn_Offsetof_e){
goto _LL3699;} else{ goto _LL3700;} _LL3700: if(*(( int*) _temp3690) ==  Cyc_Absyn_Gentyp_e){
goto _LL3701;} else{ goto _LL3702;} _LL3702: if(*(( int*) _temp3690) ==  Cyc_Absyn_Enum_e){
goto _LL3703;} else{ goto _LL3704;} _LL3704: if(*(( int*) _temp3690) ==  Cyc_Absyn_AnonEnum_e){
goto _LL3705;} else{ goto _LL3706;} _LL3706: if(*(( int*) _temp3690) ==  Cyc_Absyn_Var_e){
_LL3737: _temp3736=(( struct Cyc_Absyn_Var_e_struct*) _temp3690)->f1; goto
_LL3707;} else{ goto _LL3708;} _LL3708: if(*(( int*) _temp3690) ==  Cyc_Absyn_Conditional_e){
_LL3743: _temp3742=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3690)->f1;
goto _LL3741; _LL3741: _temp3740=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3690)->f2; goto _LL3739; _LL3739: _temp3738=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3690)->f3; goto _LL3709;} else{ goto _LL3710;} _LL3710: if(*(( int*)
_temp3690) ==  Cyc_Absyn_SeqExp_e){ _LL3747: _temp3746=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3690)->f1; goto _LL3745; _LL3745: _temp3744=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3690)->f2; goto _LL3711;} else{ goto _LL3712;} _LL3712: if(*(( int*)
_temp3690) ==  Cyc_Absyn_NoInstantiate_e){ _LL3749: _temp3748=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3690)->f1; goto _LL3713;} else{ goto _LL3714;} _LL3714: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Instantiate_e){ _LL3751: _temp3750=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3690)->f1; goto _LL3715;} else{ goto _LL3716;} _LL3716: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Cast_e){ _LL3755: _temp3754=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3690)->f1; goto _LL3753; _LL3753: _temp3752=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3690)->f2; goto _LL3717;} else{ goto _LL3718;} _LL3718: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Address_e){ _LL3757: _temp3756=(( struct Cyc_Absyn_Address_e_struct*)
_temp3690)->f1; goto _LL3719;} else{ goto _LL3720;} _LL3720: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Comprehension_e){ _LL3761: _temp3760=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3690)->f2; goto _LL3759; _LL3759: _temp3758=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3690)->f3; goto _LL3721;} else{ goto _LL3722;} _LL3722: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Array_e){ _LL3763: _temp3762=(( struct Cyc_Absyn_Array_e_struct*)
_temp3690)->f1; goto _LL3723;} else{ goto _LL3724;} _LL3724: if(*(( int*)
_temp3690) ==  Cyc_Absyn_AnonStruct_e){ _LL3765: _temp3764=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3690)->f2; goto _LL3725;} else{ goto _LL3726;} _LL3726: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Struct_e){ _LL3767: _temp3766=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3690)->f3; goto _LL3727;} else{ goto _LL3728;} _LL3728: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Primop_e){ _LL3771: _temp3770=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3690)->f1; goto _LL3769; _LL3769: _temp3768=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3690)->f2; goto _LL3729;} else{ goto _LL3730;} _LL3730: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Tuple_e){ _LL3773: _temp3772=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3690)->f1; goto _LL3731;} else{ goto _LL3732;} _LL3732: if(*(( int*)
_temp3690) ==  Cyc_Absyn_Tunion_e){ _LL3775: _temp3774=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3690)->f3; goto _LL3733;} else{ goto _LL3734;} _LL3734: goto _LL3735;
_LL3693: return 1; _LL3695: return 1; _LL3697: return 1; _LL3699: return 1;
_LL3701: return 1; _LL3703: return 1; _LL3705: return 1; _LL3707: { struct
_handler_cons _temp3776; _push_handler(& _temp3776);{ int _temp3778= 0; if(
setjmp( _temp3776.handler)){ _temp3778= 1;} if( ! _temp3778){{ void* _temp3779=
Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3736); void* _temp3789; void*
_temp3791; struct Cyc_Absyn_Vardecl* _temp3793; void* _temp3795; _LL3781: if(*((
int*) _temp3779) ==  Cyc_Tcenv_VarRes){ _LL3790: _temp3789=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3779)->f1; if(( unsigned int) _temp3789 >  1u?*(( int*) _temp3789) ==  Cyc_Absyn_Funname_b:
0){ goto _LL3782;} else{ goto _LL3783;}} else{ goto _LL3783;} _LL3783: if(*((
int*) _temp3779) ==  Cyc_Tcenv_VarRes){ _LL3792: _temp3791=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3779)->f1; if(( unsigned int) _temp3791 >  1u?*(( int*) _temp3791) ==  Cyc_Absyn_Global_b:
0){ _LL3794: _temp3793=(( struct Cyc_Absyn_Global_b_struct*) _temp3791)->f1;
goto _LL3784;} else{ goto _LL3785;}} else{ goto _LL3785;} _LL3785: if(*(( int*)
_temp3779) ==  Cyc_Tcenv_VarRes){ _LL3796: _temp3795=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3779)->f1; if( _temp3795 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL3786;}
else{ goto _LL3787;}} else{ goto _LL3787;} _LL3787: goto _LL3788; _LL3782: { int
_temp3797= 1; _npop_handler( 0u); return _temp3797;} _LL3784: { void* _temp3798=
Cyc_Tcutil_compress(( void*) _temp3793->type); _LL3800: if(( unsigned int)
_temp3798 >  4u?*(( int*) _temp3798) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3801;}
else{ goto _LL3802;} _LL3802: goto _LL3803; _LL3801: { int _temp3804= 1;
_npop_handler( 0u); return _temp3804;} _LL3803: { int _temp3805= var_okay;
_npop_handler( 0u); return _temp3805;} _LL3799:;} _LL3786: { int _temp3806= 0;
_npop_handler( 0u); return _temp3806;} _LL3788: { int _temp3807= var_okay;
_npop_handler( 0u); return _temp3807;} _LL3780:;}; _pop_handler();} else{ void*
_temp3777=( void*) _exn_thrown; void* _temp3809= _temp3777; _LL3811: if(
_temp3809 ==  Cyc_Dict_Absent){ goto _LL3812;} else{ goto _LL3813;} _LL3813:
goto _LL3814; _LL3812: return 0; _LL3814:( void) _throw( _temp3809); _LL3810:;}}}
_LL3709: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3742)? Cyc_Tcutil_cnst_exp( te,
0, _temp3740): 0)? Cyc_Tcutil_cnst_exp( te, 0, _temp3738): 0; _LL3711: return
Cyc_Tcutil_cnst_exp( te, 0, _temp3746)? Cyc_Tcutil_cnst_exp( te, 0, _temp3744):
0; _LL3713: _temp3750= _temp3748; goto _LL3715; _LL3715: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3750); _LL3717: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3752); _LL3719: return Cyc_Tcutil_cnst_exp( te, 1, _temp3756); _LL3721:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3760)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3758): 0; _LL3723: _temp3764= _temp3762; goto _LL3725; _LL3725: _temp3766=
_temp3764; goto _LL3727; _LL3727: for( 0; _temp3766 !=  0; _temp3766= _temp3766->tl){
if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple21*) _temp3766->hd)).f2)){
return 0;}} return 1; _LL3729: _temp3772= _temp3768; goto _LL3731; _LL3731:
_temp3774= _temp3772; goto _LL3733; _LL3733: for( 0; _temp3774 !=  0; _temp3774=
_temp3774->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp3774->hd)){ return 0;}} return 1; _LL3735: return 0; _LL3691:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3815= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3845; struct Cyc_Absyn_Conref* _temp3847; struct Cyc_Absyn_Conref*
_temp3849; void* _temp3851; struct Cyc_List_List* _temp3853; struct Cyc_Absyn_Structdecl**
_temp3855; struct Cyc_List_List* _temp3857; struct Cyc_Absyn_Uniondecl**
_temp3859; struct Cyc_List_List* _temp3861; struct Cyc_List_List* _temp3863;
struct Cyc_List_List* _temp3865; _LL3817: if( _temp3815 == ( void*) Cyc_Absyn_VoidType){
goto _LL3818;} else{ goto _LL3819;} _LL3819: if(( unsigned int) _temp3815 >  4u?*((
int*) _temp3815) ==  Cyc_Absyn_IntType: 0){ goto _LL3820;} else{ goto _LL3821;}
_LL3821: if( _temp3815 == ( void*) Cyc_Absyn_FloatType){ goto _LL3822;} else{
goto _LL3823;} _LL3823: if( _temp3815 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3824;} else{ goto _LL3825;} _LL3825: if(( unsigned int) _temp3815 >  4u?*((
int*) _temp3815) ==  Cyc_Absyn_PointerType: 0){ _LL3846: _temp3845=(( struct Cyc_Absyn_PointerType_struct*)
_temp3815)->f1; _LL3850: _temp3849= _temp3845.nullable; goto _LL3848; _LL3848:
_temp3847= _temp3845.bounds; goto _LL3826;} else{ goto _LL3827;} _LL3827: if((
unsigned int) _temp3815 >  4u?*(( int*) _temp3815) ==  Cyc_Absyn_ArrayType: 0){
_LL3852: _temp3851=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3815)->f1;
goto _LL3828;} else{ goto _LL3829;} _LL3829: if(( unsigned int) _temp3815 >  4u?*((
int*) _temp3815) ==  Cyc_Absyn_TupleType: 0){ _LL3854: _temp3853=(( struct Cyc_Absyn_TupleType_struct*)
_temp3815)->f1; goto _LL3830;} else{ goto _LL3831;} _LL3831: if(( unsigned int)
_temp3815 >  4u?*(( int*) _temp3815) ==  Cyc_Absyn_StructType: 0){ _LL3858:
_temp3857=(( struct Cyc_Absyn_StructType_struct*) _temp3815)->f2; goto _LL3856;
_LL3856: _temp3855=(( struct Cyc_Absyn_StructType_struct*) _temp3815)->f3; goto
_LL3832;} else{ goto _LL3833;} _LL3833: if(( unsigned int) _temp3815 >  4u?*((
int*) _temp3815) ==  Cyc_Absyn_UnionType: 0){ _LL3862: _temp3861=(( struct Cyc_Absyn_UnionType_struct*)
_temp3815)->f2; goto _LL3860; _LL3860: _temp3859=(( struct Cyc_Absyn_UnionType_struct*)
_temp3815)->f3; goto _LL3834;} else{ goto _LL3835;} _LL3835: if(( unsigned int)
_temp3815 >  4u?*(( int*) _temp3815) ==  Cyc_Absyn_AnonStructType: 0){ _LL3864:
_temp3863=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3815)->f1; goto
_LL3836;} else{ goto _LL3837;} _LL3837: if(( unsigned int) _temp3815 >  4u?*((
int*) _temp3815) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3866: _temp3865=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3815)->f1; goto _LL3838;} else{ goto
_LL3839;} _LL3839: if(( unsigned int) _temp3815 >  4u?*(( int*) _temp3815) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL3840;} else{ goto _LL3841;} _LL3841: if((
unsigned int) _temp3815 >  4u?*(( int*) _temp3815) ==  Cyc_Absyn_EnumType: 0){
goto _LL3842;} else{ goto _LL3843;} _LL3843: goto _LL3844; _LL3818: goto _LL3820;
_LL3820: goto _LL3822; _LL3822: goto _LL3824; _LL3824: return 1; _LL3826: { void*
_temp3867=( void*)( Cyc_Absyn_compress_conref( _temp3847))->v; void* _temp3875;
void* _temp3877; _LL3869: if(( unsigned int) _temp3867 >  1u?*(( int*) _temp3867)
==  Cyc_Absyn_Eq_constr: 0){ _LL3876: _temp3875=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3867)->f1; if( _temp3875 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3870;}
else{ goto _LL3871;}} else{ goto _LL3871;} _LL3871: if(( unsigned int) _temp3867
>  1u?*(( int*) _temp3867) ==  Cyc_Absyn_Eq_constr: 0){ _LL3878: _temp3877=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3867)->f1; if(( unsigned int)
_temp3877 >  1u?*(( int*) _temp3877) ==  Cyc_Absyn_Upper_b: 0){ goto _LL3872;}
else{ goto _LL3873;}} else{ goto _LL3873;} _LL3873: goto _LL3874; _LL3870:
return 1; _LL3872: { void* _temp3879=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3849))->v; int
_temp3885; _LL3881: if(( unsigned int) _temp3879 >  1u?*(( int*) _temp3879) == 
Cyc_Absyn_Eq_constr: 0){ _LL3886: _temp3885=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3879)->f1; goto _LL3882;} else{ goto _LL3883;} _LL3883: goto _LL3884;
_LL3882: return _temp3885; _LL3884: return 0; _LL3880:;} _LL3874: return 0;
_LL3868:;} _LL3828: return Cyc_Tcutil_supports_default( _temp3851); _LL3830:
for( 0; _temp3853 !=  0; _temp3853= _temp3853->tl){ if( ! Cyc_Tcutil_supports_default((*((
struct _tuple4*) _temp3853->hd)).f2)){ return 0;}} return 1; _LL3832: if(
_temp3855 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=* _temp3855; if(
sd->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs,
_temp3857,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);}
_LL3834: if( _temp3859 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*
_temp3859; if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp3861,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL3836: _temp3865= _temp3863; goto _LL3838; _LL3838: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp3865); _LL3840: goto _LL3842;
_LL3842: return 1; _LL3844: return 0; _LL3816:;} static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List* tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{
struct _RegionHandle _temp3887= _new_region(); struct _RegionHandle* rgn=&
_temp3887; _push_region( rgn);{ struct Cyc_List_List* _temp3888=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0;
fs= fs->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3888,( void*)(( struct
Cyc_Absyn_Structfield*) fs->hd)->type); if( ! Cyc_Tcutil_supports_default( t)){
int _temp3889= 0; _npop_handler( 0u); return _temp3889;}}}; _pop_region( rgn);}
return 1;}
