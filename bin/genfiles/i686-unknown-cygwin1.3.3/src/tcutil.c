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
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
f3; } ; static const int Cyc_Absyn_Malloc_e= 32; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
33; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 34; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 35; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 36; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 18; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
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
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_SwitchC_clause{
struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment*
loc; } ; static const int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b= 1; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b= 2;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b= 3; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b= 4; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
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
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
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
struct Cyc_List_List* tvs){ for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){({ struct Cyc_Std_String_pa_struct _temp15; _temp15.tag=
Cyc_Std_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);{
struct Cyc_Std_String_pa_struct _temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=(
struct _tagged_arr) Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd);{ void* _temp13[ 2u]={& _temp14,&
_temp15}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::%s ", sizeof(
unsigned char), 8u), _tag_arr( _temp13, sizeof( void*), 2u));}}});}({ void*
_temp16[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp16, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs=
0; static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_arr fmt, struct _tagged_arr ap){
struct _tagged_arr msg=( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap); Cyc_Tcutil_warning_segs=({ struct Cyc_List_List* _temp17=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp17->hd=( void*) sg; _temp17->tl=
Cyc_Tcutil_warning_segs; _temp17;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List*
_temp18=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp18->hd=( void*)({ struct _tagged_arr* _temp19=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp19[ 0]= msg; _temp19;}); _temp18->tl=
Cyc_Tcutil_warning_msgs; _temp18;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs
==  0){ return;}({ void* _temp20[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("***Warnings***\n", sizeof( unsigned char), 16u), _tag_arr( _temp20,
sizeof( void*), 0u));});{ struct Cyc_List_List* _temp21= Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs);
while( Cyc_Tcutil_warning_msgs !=  0) {({ struct Cyc_Std_String_pa_struct
_temp24; _temp24.tag= Cyc_Std_String_pa; _temp24.f1=( struct _tagged_arr)*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Std_String_pa_struct _temp23; _temp23.tag= Cyc_Std_String_pa; _temp23.f1=(
struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
_temp21))->hd);{ void* _temp22[ 2u]={& _temp23,& _temp24}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp22, sizeof(
void*), 2u));}}}); _temp21=(( struct Cyc_List_List*) _check_null( _temp21))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}({
void* _temp25[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("**************\n",
sizeof( unsigned char), 16u), _tag_arr( _temp25, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) - *(( int*) _check_null( tv2->identity));}
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
_temp85); _temp87->f2= Cyc_Tcutil_copy_type( _temp83); _temp87;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp88= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp136; struct Cyc_Absyn_TunionInfo _temp138; void* _temp140; struct Cyc_List_List*
_temp142; void* _temp144; struct Cyc_Absyn_TunionFieldInfo _temp146; struct Cyc_List_List*
_temp148; void* _temp150; struct Cyc_Absyn_PtrInfo _temp152; struct Cyc_Absyn_Conref*
_temp154; struct Cyc_Absyn_Tqual _temp156; struct Cyc_Absyn_Conref* _temp158;
void* _temp160; void* _temp162; void* _temp164; void* _temp166; struct Cyc_Absyn_Exp*
_temp168; struct Cyc_Absyn_Tqual _temp170; void* _temp172; struct Cyc_Absyn_FnInfo
_temp174; struct Cyc_List_List* _temp176; struct Cyc_List_List* _temp178; struct
Cyc_Absyn_VarargInfo* _temp180; int _temp182; struct Cyc_List_List* _temp184;
void* _temp186; struct Cyc_Core_Opt* _temp188; struct Cyc_List_List* _temp190;
struct Cyc_List_List* _temp192; struct Cyc_List_List* _temp194; struct _tuple1*
_temp196; struct Cyc_List_List* _temp198; struct _tuple1* _temp200; struct Cyc_List_List*
_temp202; struct Cyc_List_List* _temp204; struct _tuple1* _temp206; void*
_temp208; struct Cyc_List_List* _temp210; struct _tuple1* _temp212; void*
_temp214; struct Cyc_List_List* _temp216; void* _temp218; _LL90: if( _temp88 == (
void*) Cyc_Absyn_VoidType){ goto _LL91;} else{ goto _LL92;} _LL92: if((
unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_Evar: 0){ goto
_LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp88 >  4u?*(( int*)
_temp88) ==  Cyc_Absyn_VarType: 0){ _LL137: _temp136=(( struct Cyc_Absyn_VarType_struct*)
_temp88)->f1; goto _LL95;} else{ goto _LL96;} _LL96: if(( unsigned int) _temp88
>  4u?*(( int*) _temp88) ==  Cyc_Absyn_TunionType: 0){ _LL139: _temp138=((
struct Cyc_Absyn_TunionType_struct*) _temp88)->f1; _LL145: _temp144=( void*)
_temp138.tunion_info; goto _LL143; _LL143: _temp142= _temp138.targs; goto _LL141;
_LL141: _temp140=( void*) _temp138.rgn; goto _LL97;} else{ goto _LL98;} _LL98:
if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_TunionFieldType:
0){ _LL147: _temp146=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp88)->f1;
_LL151: _temp150=( void*) _temp146.field_info; goto _LL149; _LL149: _temp148=
_temp146.targs; goto _LL99;} else{ goto _LL100;} _LL100: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_PointerType: 0){ _LL153: _temp152=((
struct Cyc_Absyn_PointerType_struct*) _temp88)->f1; _LL163: _temp162=( void*)
_temp152.elt_typ; goto _LL161; _LL161: _temp160=( void*) _temp152.rgn_typ; goto
_LL159; _LL159: _temp158= _temp152.nullable; goto _LL157; _LL157: _temp156=
_temp152.tq; goto _LL155; _LL155: _temp154= _temp152.bounds; goto _LL101;} else{
goto _LL102;} _LL102: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) == 
Cyc_Absyn_IntType: 0){ _LL167: _temp166=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp88)->f1; goto _LL165; _LL165: _temp164=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp88)->f2; goto _LL103;} else{ goto _LL104;} _LL104: if( _temp88 == ( void*)
Cyc_Absyn_FloatType){ goto _LL105;} else{ goto _LL106;} _LL106: if( _temp88 == (
void*) Cyc_Absyn_DoubleType){ goto _LL107;} else{ goto _LL108;} _LL108: if((
unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_ArrayType: 0){
_LL173: _temp172=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f1;
goto _LL171; _LL171: _temp170=(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f2;
goto _LL169; _LL169: _temp168=(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f3;
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp88 >  4u?*((
int*) _temp88) ==  Cyc_Absyn_FnType: 0){ _LL175: _temp174=(( struct Cyc_Absyn_FnType_struct*)
_temp88)->f1; _LL191: _temp190= _temp174.tvars; goto _LL189; _LL189: _temp188=
_temp174.effect; goto _LL187; _LL187: _temp186=( void*) _temp174.ret_typ; goto
_LL185; _LL185: _temp184= _temp174.args; goto _LL183; _LL183: _temp182= _temp174.c_varargs;
goto _LL181; _LL181: _temp180= _temp174.cyc_varargs; goto _LL179; _LL179:
_temp178= _temp174.rgn_po; goto _LL177; _LL177: _temp176= _temp174.attributes;
goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp88 >  4u?*((
int*) _temp88) ==  Cyc_Absyn_TupleType: 0){ _LL193: _temp192=(( struct Cyc_Absyn_TupleType_struct*)
_temp88)->f1; goto _LL113;} else{ goto _LL114;} _LL114: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_StructType: 0){ _LL197: _temp196=((
struct Cyc_Absyn_StructType_struct*) _temp88)->f1; goto _LL195; _LL195: _temp194=((
struct Cyc_Absyn_StructType_struct*) _temp88)->f2; goto _LL115;} else{ goto
_LL116;} _LL116: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_UnionType:
0){ _LL201: _temp200=(( struct Cyc_Absyn_UnionType_struct*) _temp88)->f1; goto
_LL199; _LL199: _temp198=(( struct Cyc_Absyn_UnionType_struct*) _temp88)->f2;
goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int) _temp88 >  4u?*((
int*) _temp88) ==  Cyc_Absyn_AnonStructType: 0){ _LL203: _temp202=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp88)->f1; goto _LL119;} else{ goto _LL120;} _LL120: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_AnonUnionType: 0){ _LL205:
_temp204=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp88)->f1; goto _LL121;}
else{ goto _LL122;} _LL122: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88)
==  Cyc_Absyn_EnumType: 0){ _LL207: _temp206=(( struct Cyc_Absyn_EnumType_struct*)
_temp88)->f1; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_RgnHandleType: 0){ _LL209:
_temp208=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp88)->f1; goto
_LL125;} else{ goto _LL126;} _LL126: if(( unsigned int) _temp88 >  4u?*(( int*)
_temp88) ==  Cyc_Absyn_TypedefType: 0){ _LL213: _temp212=(( struct Cyc_Absyn_TypedefType_struct*)
_temp88)->f1; goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_TypedefType_struct*)
_temp88)->f2; goto _LL127;} else{ goto _LL128;} _LL128: if( _temp88 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL129;} else{ goto _LL130;} _LL130: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_AccessEff: 0){ _LL215: _temp214=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp88)->f1; goto _LL131;} else{
goto _LL132;} _LL132: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) == 
Cyc_Absyn_JoinEff: 0){ _LL217: _temp216=(( struct Cyc_Absyn_JoinEff_struct*)
_temp88)->f1; goto _LL133;} else{ goto _LL134;} _LL134: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_RgnsEff: 0){ _LL219: _temp218=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp88)->f1; goto _LL135;} else{
goto _LL89;} _LL91: goto _LL93; _LL93: return t; _LL95: return( void*)({ struct
Cyc_Absyn_VarType_struct* _temp220=( struct Cyc_Absyn_VarType_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp220[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp221; _temp221.tag= Cyc_Absyn_VarType; _temp221.f1= Cyc_Tcutil_copy_tvar(
_temp136); _temp221;}); _temp220;}); _LL97: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp222=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp222[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp223; _temp223.tag= Cyc_Absyn_TunionType;
_temp223.f1=({ struct Cyc_Absyn_TunionInfo _temp224; _temp224.tunion_info=( void*)
_temp144; _temp224.targs= Cyc_Tcutil_copy_types( _temp142); _temp224.rgn=( void*)
Cyc_Tcutil_copy_type( _temp140); _temp224;}); _temp223;}); _temp222;}); _LL99:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp225=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp225[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp226; _temp226.tag= Cyc_Absyn_TunionFieldType;
_temp226.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp227; _temp227.field_info=(
void*) _temp150; _temp227.targs= Cyc_Tcutil_copy_types( _temp148); _temp227;});
_temp226;}); _temp225;}); _LL101: { void* _temp228= Cyc_Tcutil_copy_type(
_temp162); void* _temp229= Cyc_Tcutil_copy_type( _temp160); struct Cyc_Absyn_Conref*
_temp230=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp158); struct Cyc_Absyn_Tqual _temp231= _temp156; struct Cyc_Absyn_Conref*
_temp232= Cyc_Tcutil_copy_conref( _temp154); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp233=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp234; _temp234.tag= Cyc_Absyn_PointerType;
_temp234.f1=({ struct Cyc_Absyn_PtrInfo _temp235; _temp235.elt_typ=( void*)
_temp228; _temp235.rgn_typ=( void*) _temp229; _temp235.nullable= _temp230;
_temp235.tq= _temp231; _temp235.bounds= _temp232; _temp235;}); _temp234;});
_temp233;});} _LL103: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp236=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp236[ 0]=({ struct Cyc_Absyn_IntType_struct _temp237; _temp237.tag= Cyc_Absyn_IntType;
_temp237.f1=( void*) _temp166; _temp237.f2=( void*) _temp164; _temp237;});
_temp236;}); _LL105: goto _LL107; _LL107: return t; _LL109: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp238=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp238[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp239; _temp239.tag= Cyc_Absyn_ArrayType; _temp239.f1=(
void*) Cyc_Tcutil_copy_type( _temp172); _temp239.f2= _temp170; _temp239.f3=
_temp168; _temp239;}); _temp238;}); _LL111: { struct Cyc_List_List* _temp240=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp190); struct
Cyc_Core_Opt* _temp241= _temp188 ==  0? 0:({ struct Cyc_Core_Opt* _temp251=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp251->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp188))->v); _temp251;}); void* _temp242= Cyc_Tcutil_copy_type( _temp186);
struct Cyc_List_List* _temp243=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp184); int _temp244= _temp182; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp180 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp180); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp245=(
struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp245->name= cv->name; _temp245->tq= cv->tq; _temp245->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp245->inject= cv->inject; _temp245;});}{ struct Cyc_List_List*
_temp246=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp178);
struct Cyc_List_List* _temp247= _temp176; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp248=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp248[ 0]=({ struct Cyc_Absyn_FnType_struct _temp249; _temp249.tag= Cyc_Absyn_FnType;
_temp249.f1=({ struct Cyc_Absyn_FnInfo _temp250; _temp250.tvars= _temp240;
_temp250.effect= _temp241; _temp250.ret_typ=( void*) _temp242; _temp250.args=
_temp243; _temp250.c_varargs= _temp244; _temp250.cyc_varargs= cyc_varargs2;
_temp250.rgn_po= _temp246; _temp250.attributes= _temp247; _temp250;}); _temp249;});
_temp248;});}} _LL113: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp252=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp252[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp253; _temp253.tag= Cyc_Absyn_TupleType;
_temp253.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp192);
_temp253;}); _temp252;}); _LL115: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp254=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp254[ 0]=({ struct Cyc_Absyn_StructType_struct _temp255; _temp255.tag= Cyc_Absyn_StructType;
_temp255.f1= _temp196; _temp255.f2= Cyc_Tcutil_copy_types( _temp194); _temp255.f3=
0; _temp255;}); _temp254;}); _LL117: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp256=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp256[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp257; _temp257.tag= Cyc_Absyn_UnionType;
_temp257.f1= _temp200; _temp257.f2= Cyc_Tcutil_copy_types( _temp198); _temp257.f3=
0; _temp257;}); _temp256;}); _LL119: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp258=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp258[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp259; _temp259.tag= Cyc_Absyn_AnonStructType; _temp259.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp202); _temp259;}); _temp258;});
_LL121: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp260=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp260[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp261; _temp261.tag=
Cyc_Absyn_AnonUnionType; _temp261.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp204); _temp261;}); _temp260;}); _LL123: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp262=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp262[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp263; _temp263.tag= Cyc_Absyn_EnumType;
_temp263.f1= _temp206; _temp263.f2= 0; _temp263;}); _temp262;}); _LL125: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp264=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp264[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp265; _temp265.tag= Cyc_Absyn_RgnHandleType;
_temp265.f1=( void*) Cyc_Tcutil_copy_type( _temp208); _temp265;}); _temp264;});
_LL127: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp266=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp266[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp267; _temp267.tag= Cyc_Absyn_TypedefType;
_temp267.f1= _temp212; _temp267.f2= Cyc_Tcutil_copy_types( _temp210); _temp267.f3=
0; _temp267;}); _temp266;}); _LL129: return t; _LL131: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp268=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp268[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp269; _temp269.tag= Cyc_Absyn_AccessEff; _temp269.f1=(
void*) Cyc_Tcutil_copy_type( _temp214); _temp269;}); _temp268;}); _LL133: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp270=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp270[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp271; _temp271.tag= Cyc_Absyn_JoinEff; _temp271.f1= Cyc_Tcutil_copy_types(
_temp216); _temp271;}); _temp270;}); _LL135: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp272=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp272[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp273; _temp273.tag= Cyc_Absyn_RgnsEff;
_temp273.f1=( void*) Cyc_Tcutil_copy_type( _temp218); _temp273;}); _temp272;});
_LL89:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){
return 1;}{ struct _tuple8 _temp275=({ struct _tuple8 _temp274; _temp274.f1= k1;
_temp274.f2= k2; _temp274;}); void* _temp285; void* _temp287; void* _temp289;
void* _temp291; void* _temp293; void* _temp295; _LL277: _LL288: _temp287=
_temp275.f1; if( _temp287 == ( void*) Cyc_Absyn_BoxKind){ goto _LL286;} else{
goto _LL279;} _LL286: _temp285= _temp275.f2; if( _temp285 == ( void*) Cyc_Absyn_MemKind){
goto _LL278;} else{ goto _LL279;} _LL279: _LL292: _temp291= _temp275.f1; if(
_temp291 == ( void*) Cyc_Absyn_BoxKind){ goto _LL290;} else{ goto _LL281;}
_LL290: _temp289= _temp275.f2; if( _temp289 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL280;} else{ goto _LL281;} _LL281: _LL296: _temp295= _temp275.f1; if( _temp295
== ( void*) Cyc_Absyn_MemKind){ goto _LL294;} else{ goto _LL283;} _LL294:
_temp293= _temp275.f2; if( _temp293 == ( void*) Cyc_Absyn_AnyKind){ goto _LL282;}
else{ goto _LL283;} _LL283: goto _LL284; _LL278: goto _LL280; _LL280: goto
_LL282; _LL282: return 1; _LL284: return 0; _LL276:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp297= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp353;
struct Cyc_Core_Opt* _temp355; struct Cyc_Absyn_Tvar* _temp357; void* _temp359;
struct Cyc_Absyn_TunionFieldInfo _temp361; void* _temp363; struct Cyc_Absyn_Tunionfield*
_temp365; struct Cyc_Absyn_TunionFieldInfo _temp367; void* _temp369; struct Cyc_Absyn_Structdecl**
_temp371; struct Cyc_Absyn_Uniondecl** _temp373; struct Cyc_Absyn_Enumdecl*
_temp375; struct Cyc_Absyn_Enumdecl* _temp377; struct Cyc_Absyn_PtrInfo _temp379;
struct Cyc_Core_Opt* _temp381; _LL299: if(( unsigned int) _temp297 >  4u?*(( int*)
_temp297) ==  Cyc_Absyn_Evar: 0){ _LL356: _temp355=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f1; goto _LL354; _LL354: _temp353=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f2; goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_VarType: 0){ _LL358: _temp357=((
struct Cyc_Absyn_VarType_struct*) _temp297)->f1; goto _LL302;} else{ goto _LL303;}
_LL303: if( _temp297 == ( void*) Cyc_Absyn_VoidType){ goto _LL304;} else{ goto
_LL305;} _LL305: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_IntType:
0){ _LL360: _temp359=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp297)->f2;
goto _LL306;} else{ goto _LL307;} _LL307: if( _temp297 == ( void*) Cyc_Absyn_FloatType){
goto _LL308;} else{ goto _LL309;} _LL309: if( _temp297 == ( void*) Cyc_Absyn_DoubleType){
goto _LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_FnType: 0){ goto _LL312;} else{ goto _LL313;}
_LL313: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL314;} else{ goto _LL315;} _LL315: if( _temp297 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL316;} else{ goto _LL317;} _LL317: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_TunionType: 0){ goto _LL318;} else{ goto _LL319;}
_LL319: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_TunionFieldType:
0){ _LL362: _temp361=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp297)->f1;
_LL364: _temp363=( void*) _temp361.field_info; if(*(( int*) _temp363) ==  Cyc_Absyn_KnownTunionfield){
_LL366: _temp365=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp363)->f2;
goto _LL320;} else{ goto _LL321;}} else{ goto _LL321;} _LL321: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_TunionFieldType: 0){ _LL368:
_temp367=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp297)->f1; _LL370:
_temp369=( void*) _temp367.field_info; if(*(( int*) _temp369) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL322;} else{ goto _LL323;}} else{ goto _LL323;} _LL323: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_StructType: 0){ _LL372:
_temp371=(( struct Cyc_Absyn_StructType_struct*) _temp297)->f3; if( _temp371 == 
0){ goto _LL324;} else{ goto _LL325;}} else{ goto _LL325;} _LL325: if((
unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_UnionType: 0){
_LL374: _temp373=(( struct Cyc_Absyn_UnionType_struct*) _temp297)->f3; if(
_temp373 ==  0){ goto _LL326;} else{ goto _LL327;}} else{ goto _LL327;} _LL327:
if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_EnumType: 0){
_LL376: _temp375=(( struct Cyc_Absyn_EnumType_struct*) _temp297)->f2; if(
_temp375 ==  0){ goto _LL328;} else{ goto _LL329;}} else{ goto _LL329;} _LL329:
if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_StructType:
0){ goto _LL330;} else{ goto _LL331;} _LL331: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_UnionType: 0){ goto _LL332;} else{ goto _LL333;}
_LL333: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL334;} else{ goto _LL335;} _LL335: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL336;} else{ goto _LL337;}
_LL337: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_EnumType:
0){ _LL378: _temp377=(( struct Cyc_Absyn_EnumType_struct*) _temp297)->f2; goto
_LL338;} else{ goto _LL339;} _LL339: if(( unsigned int) _temp297 >  4u?*(( int*)
_temp297) ==  Cyc_Absyn_PointerType: 0){ _LL380: _temp379=(( struct Cyc_Absyn_PointerType_struct*)
_temp297)->f1; goto _LL340;} else{ goto _LL341;} _LL341: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_ArrayType: 0){ goto _LL342;}
else{ goto _LL343;} _LL343: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297)
==  Cyc_Absyn_TupleType: 0){ goto _LL344;} else{ goto _LL345;} _LL345: if((
unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_TypedefType: 0){
_LL382: _temp381=(( struct Cyc_Absyn_TypedefType_struct*) _temp297)->f3; goto
_LL346;} else{ goto _LL347;} _LL347: if(( unsigned int) _temp297 >  4u?*(( int*)
_temp297) ==  Cyc_Absyn_AccessEff: 0){ goto _LL348;} else{ goto _LL349;} _LL349:
if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_JoinEff: 0){
goto _LL350;} else{ goto _LL351;} _LL351: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL352;} else{ goto _LL298;}
_LL300: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp355))->v;
_LL302: return Cyc_Absyn_conref_val( _temp357->kind); _LL304: return( void*) Cyc_Absyn_MemKind;
_LL306: return _temp359 == ( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL308: goto _LL310; _LL310: goto _LL312; _LL312:
return( void*) Cyc_Absyn_MemKind; _LL314: return( void*) Cyc_Absyn_BoxKind;
_LL316: return( void*) Cyc_Absyn_RgnKind; _LL318: return( void*) Cyc_Absyn_BoxKind;
_LL320: if( _temp365->typs ==  0){ return( void*) Cyc_Absyn_BoxKind;} else{
return( void*) Cyc_Absyn_MemKind;} _LL322: return({ void* _temp383[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("typ_kind: Unresolved TunionFieldType", sizeof( unsigned char), 37u),
_tag_arr( _temp383, sizeof( void*), 0u));}); _LL324: goto _LL326; _LL326: goto
_LL328; _LL328: return( void*) Cyc_Absyn_AnyKind; _LL330: goto _LL332; _LL332:
goto _LL334; _LL334: goto _LL336; _LL336: return( void*) Cyc_Absyn_MemKind;
_LL338: if((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp377))->fields ==  0){
return( void*) Cyc_Absyn_AnyKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL340: { void* _temp384=( void*)( Cyc_Absyn_compress_conref( _temp379.bounds))->v;
void* _temp394; void* _temp396; _LL386: if(( unsigned int) _temp384 >  1u?*((
int*) _temp384) ==  Cyc_Absyn_Eq_constr: 0){ _LL395: _temp394=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp384)->f1; if( _temp394 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL387;} else{ goto _LL388;}} else{ goto _LL388;} _LL388: if(( unsigned int)
_temp384 >  1u?*(( int*) _temp384) ==  Cyc_Absyn_Eq_constr: 0){ _LL397: _temp396=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp384)->f1; if(( unsigned int)
_temp396 >  1u?*(( int*) _temp396) ==  Cyc_Absyn_Upper_b: 0){ goto _LL389;}
else{ goto _LL390;}} else{ goto _LL390;} _LL390: if( _temp384 == ( void*) Cyc_Absyn_No_constr){
goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int) _temp384 >  1u?*((
int*) _temp384) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL393;} else{ goto
_LL385;} _LL387: return( void*) Cyc_Absyn_MemKind; _LL389: return( void*) Cyc_Absyn_BoxKind;
_LL391: return( void*) Cyc_Absyn_MemKind; _LL393: return({ void* _temp398[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds", sizeof(
unsigned char), 43u), _tag_arr( _temp398, sizeof( void*), 0u));}); _LL385:;}
_LL342: goto _LL344; _LL344: return( void*) Cyc_Absyn_MemKind; _LL346: return({
struct Cyc_Std_String_pa_struct _temp400; _temp400.tag= Cyc_Std_String_pa;
_temp400.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp399[ 1u]={&
_temp400}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp399, sizeof( void*), 1u));}}); _LL348: goto
_LL350; _LL350: goto _LL352; _LL352: return( void*) Cyc_Absyn_EffKind; _LL298:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp401; _push_handler(& _temp401);{
int _temp403= 0; if( setjmp( _temp401.handler)){ _temp403= 1;} if( ! _temp403){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp404= 1; _npop_handler( 0u); return
_temp404;}; _pop_handler();} else{ void* _temp402=( void*) _exn_thrown; void*
_temp406= _temp402; _LL408: if( _temp406 ==  Cyc_Tcutil_Unify){ goto _LL409;}
else{ goto _LL410;} _LL410: goto _LL411; _LL409: return 0; _LL411:( void) _throw(
_temp406); _LL407:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp412= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp448; struct Cyc_Core_Opt* _temp450; struct Cyc_Core_Opt**
_temp452; struct Cyc_Core_Opt* _temp453; struct Cyc_Absyn_PtrInfo _temp455; void*
_temp457; struct Cyc_Absyn_FnInfo _temp459; struct Cyc_List_List* _temp461;
struct Cyc_List_List* _temp463; struct Cyc_Absyn_VarargInfo* _temp465; int
_temp467; struct Cyc_List_List* _temp469; void* _temp471; struct Cyc_Core_Opt*
_temp473; struct Cyc_List_List* _temp475; struct Cyc_List_List* _temp477; void*
_temp479; struct Cyc_Absyn_TunionInfo _temp481; void* _temp483; struct Cyc_List_List*
_temp485; struct Cyc_Core_Opt* _temp487; struct Cyc_List_List* _temp489; struct
Cyc_Absyn_TunionFieldInfo _temp491; struct Cyc_List_List* _temp493; struct Cyc_List_List*
_temp495; struct Cyc_List_List* _temp497; struct Cyc_List_List* _temp499; void*
_temp501; struct Cyc_List_List* _temp503; void* _temp505; _LL414: if((
unsigned int) _temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_VarType: 0){
_LL449: _temp448=(( struct Cyc_Absyn_VarType_struct*) _temp412)->f1; goto _LL415;}
else{ goto _LL416;} _LL416: if(( unsigned int) _temp412 >  4u?*(( int*) _temp412)
==  Cyc_Absyn_Evar: 0){ _LL454: _temp453=(( struct Cyc_Absyn_Evar_struct*)
_temp412)->f2; goto _LL451; _LL451: _temp450=(( struct Cyc_Absyn_Evar_struct*)
_temp412)->f4; _temp452=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp412)->f4; goto _LL417;} else{ goto _LL418;} _LL418: if(( unsigned int)
_temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_PointerType: 0){ _LL456:
_temp455=(( struct Cyc_Absyn_PointerType_struct*) _temp412)->f1; goto _LL419;}
else{ goto _LL420;} _LL420: if(( unsigned int) _temp412 >  4u?*(( int*) _temp412)
==  Cyc_Absyn_ArrayType: 0){ _LL458: _temp457=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp412)->f1; goto _LL421;} else{ goto _LL422;} _LL422: if(( unsigned int)
_temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_FnType: 0){ _LL460: _temp459=((
struct Cyc_Absyn_FnType_struct*) _temp412)->f1; _LL476: _temp475= _temp459.tvars;
goto _LL474; _LL474: _temp473= _temp459.effect; goto _LL472; _LL472: _temp471=(
void*) _temp459.ret_typ; goto _LL470; _LL470: _temp469= _temp459.args; goto
_LL468; _LL468: _temp467= _temp459.c_varargs; goto _LL466; _LL466: _temp465=
_temp459.cyc_varargs; goto _LL464; _LL464: _temp463= _temp459.rgn_po; goto
_LL462; _LL462: _temp461= _temp459.attributes; goto _LL423;} else{ goto _LL424;}
_LL424: if(( unsigned int) _temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_TupleType:
0){ _LL478: _temp477=(( struct Cyc_Absyn_TupleType_struct*) _temp412)->f1; goto
_LL425;} else{ goto _LL426;} _LL426: if(( unsigned int) _temp412 >  4u?*(( int*)
_temp412) ==  Cyc_Absyn_RgnHandleType: 0){ _LL480: _temp479=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp412)->f1; goto _LL427;} else{ goto _LL428;}
_LL428: if(( unsigned int) _temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_TunionType:
0){ _LL482: _temp481=(( struct Cyc_Absyn_TunionType_struct*) _temp412)->f1;
_LL486: _temp485= _temp481.targs; goto _LL484; _LL484: _temp483=( void*)
_temp481.rgn; goto _LL429;} else{ goto _LL430;} _LL430: if(( unsigned int)
_temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_TypedefType: 0){ _LL490:
_temp489=(( struct Cyc_Absyn_TypedefType_struct*) _temp412)->f2; goto _LL488;
_LL488: _temp487=(( struct Cyc_Absyn_TypedefType_struct*) _temp412)->f3; goto
_LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp412 >  4u?*(( int*)
_temp412) ==  Cyc_Absyn_TunionFieldType: 0){ _LL492: _temp491=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp412)->f1; _LL494: _temp493= _temp491.targs; goto _LL433;} else{ goto _LL434;}
_LL434: if(( unsigned int) _temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_StructType:
0){ _LL496: _temp495=(( struct Cyc_Absyn_StructType_struct*) _temp412)->f2; goto
_LL435;} else{ goto _LL436;} _LL436: if(( unsigned int) _temp412 >  4u?*(( int*)
_temp412) ==  Cyc_Absyn_AnonStructType: 0){ _LL498: _temp497=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp412)->f1; goto _LL437;} else{ goto _LL438;} _LL438: if(( unsigned int)
_temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_AnonUnionType: 0){ _LL500:
_temp499=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp412)->f1; goto _LL439;}
else{ goto _LL440;} _LL440: if(( unsigned int) _temp412 >  4u?*(( int*) _temp412)
==  Cyc_Absyn_AccessEff: 0){ _LL502: _temp501=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp412)->f1; goto _LL441;} else{ goto _LL442;} _LL442: if(( unsigned int)
_temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_JoinEff: 0){ _LL504: _temp503=((
struct Cyc_Absyn_JoinEff_struct*) _temp412)->f1; goto _LL443;} else{ goto _LL444;}
_LL444: if(( unsigned int) _temp412 >  4u?*(( int*) _temp412) ==  Cyc_Absyn_RgnsEff:
0){ _LL506: _temp505=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp412)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: goto _LL447; _LL415: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp448)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);} goto
_LL413; _LL417: if( t ==  evar){ Cyc_Tcutil_failure_reason= _tag_arr("(occurs check)",
sizeof( unsigned char), 15u);( int) _throw(( void*) Cyc_Tcutil_Unify);} else{
if( _temp453 !=  0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp453))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp452))->v; for(
0; s !=  0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp507= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp452))->v;
for( 0; s !=  0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp507=({ struct Cyc_List_List* _temp508=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp508->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp508->tl= _temp507; _temp508;});}}}*
_temp452=({ struct Cyc_Core_Opt* _temp509=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp509->v=( void*) _temp507; _temp509;});}}}
goto _LL413; _LL419: Cyc_Tcutil_occurs( evar, env,( void*) _temp455.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp455.rgn_typ); goto _LL413; _LL421:
Cyc_Tcutil_occurs( evar, env, _temp457); goto _LL413; _LL423: if( _temp473 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp473))->v);} Cyc_Tcutil_occurs( evar, env, _temp471); for( 0; _temp469 !=  0;
_temp469=(( struct Cyc_List_List*) _check_null( _temp469))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp469))->hd)).f3);}
if( _temp465 !=  0){ void* _temp512; struct Cyc_Absyn_VarargInfo _temp510=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp465)); _LL513: _temp512=( void*)
_temp510.type; goto _LL511; _LL511: Cyc_Tcutil_occurs( evar, env, _temp512);}
for( 0; _temp463 !=  0; _temp463=(( struct Cyc_List_List*) _check_null( _temp463))->tl){
struct _tuple8 _temp516; void* _temp517; void* _temp519; struct _tuple8*
_temp514=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp463))->hd;
_temp516=* _temp514; _LL520: _temp519= _temp516.f1; goto _LL518; _LL518:
_temp517= _temp516.f2; goto _LL515; _LL515: Cyc_Tcutil_occurs( evar, env,
_temp519); Cyc_Tcutil_occurs( evar, env, _temp517);} goto _LL413; _LL425: for( 0;
_temp477 !=  0; _temp477=(( struct Cyc_List_List*) _check_null( _temp477))->tl){
Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp477))->hd)).f2);} goto _LL413; _LL427: Cyc_Tcutil_occurs( evar,
env, _temp479); goto _LL413; _LL429: Cyc_Tcutil_occurs( evar, env, _temp483);
Cyc_Tcutil_occurslist( evar, env, _temp485); goto _LL413; _LL431: _temp493=
_temp489; goto _LL433; _LL433: _temp495= _temp493; goto _LL435; _LL435: Cyc_Tcutil_occurslist(
evar, env, _temp495); goto _LL413; _LL437: _temp499= _temp497; goto _LL439;
_LL439: for( 0; _temp499 !=  0; _temp499=(( struct Cyc_List_List*) _check_null(
_temp499))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp499))->hd)->type);} goto _LL413; _LL441:
Cyc_Tcutil_occurs( evar, env, _temp501); goto _LL413; _LL443: Cyc_Tcutil_occurslist(
evar, env, _temp503); goto _LL413; _LL445: Cyc_Tcutil_occurs( evar, env,
_temp505); goto _LL413; _LL447: goto _LL413; _LL413:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts != 
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ for( 0; t1 !=  0? t2 !=  0:
0;( t1=(( struct Cyc_List_List*) _check_null( t1))->tl, t2=(( struct Cyc_List_List*)
_check_null( t2))->tl)){ Cyc_Tcutil_unify_it(( void*)(( struct Cyc_List_List*)
_check_null( t1))->hd,( void*)(( struct Cyc_List_List*) _check_null( t2))->hd);}
if( t1 !=  0? 1: t2 !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);}} static
void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ if(( tq1.q_const !=  tq2.q_const? 1: tq1.q_volatile !=  tq2.q_volatile)? 1:
tq1.q_restrict !=  tq2.q_restrict){ Cyc_Tcutil_failure_reason= _tag_arr("(qualifiers don't match)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == 
tq2.q_const? tq1.q_volatile ==  tq2.q_volatile: 0)? tq1.q_restrict ==  tq2.q_restrict:
0;} static void Cyc_Tcutil_unify_it_conrefs( int(* cmp)( void*, void*), struct
Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y, struct _tagged_arr reason){ x=
Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x ==  y){
return;}{ void* _temp521=( void*) x->v; void* _temp529; _LL523: if( _temp521 == (
void*) Cyc_Absyn_No_constr){ goto _LL524;} else{ goto _LL525;} _LL525: if((
unsigned int) _temp521 >  1u?*(( int*) _temp521) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL526;} else{ goto _LL527;} _LL527: if(( unsigned int) _temp521 >  1u?*((
int*) _temp521) ==  Cyc_Absyn_Eq_constr: 0){ _LL530: _temp529=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp521)->f1; goto _LL528;} else{ goto _LL522;}
_LL524:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp531=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp531[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp532; _temp532.tag= Cyc_Absyn_Forward_constr; _temp532.f1= y; _temp532;});
_temp531;}))); return; _LL526:( int) _throw(({ void* _temp533[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp533, sizeof( void*), 0u));})); _LL528: { void* _temp534=( void*)
y->v; void* _temp542; _LL536: if( _temp534 == ( void*) Cyc_Absyn_No_constr){
goto _LL537;} else{ goto _LL538;} _LL538: if(( unsigned int) _temp534 >  1u?*((
int*) _temp534) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL539;} else{ goto
_LL540;} _LL540: if(( unsigned int) _temp534 >  1u?*(( int*) _temp534) ==  Cyc_Absyn_Eq_constr:
0){ _LL543: _temp542=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp534)->f1;
goto _LL541;} else{ goto _LL535;} _LL537:( void*)( y->v=( void*)(( void*) x->v));
return; _LL539:( int) _throw(({ void* _temp544[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp544, sizeof( void*), 0u));})); _LL541: if( cmp( _temp529,
_temp542) !=  0){ Cyc_Tcutil_failure_reason= reason;( int) _throw(( void*) Cyc_Tcutil_Unify);}
return; _LL535:;} _LL522:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp545; _push_handler(& _temp545);{ int _temp547= 0; if( setjmp(
_temp545.handler)){ _temp547= 1;} if( ! _temp547){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u));{ int _temp548= 1;
_npop_handler( 0u); return _temp548;}; _pop_handler();} else{ void* _temp546=(
void*) _exn_thrown; void* _temp550= _temp546; _LL552: if( _temp550 ==  Cyc_Tcutil_Unify){
goto _LL553;} else{ goto _LL554;} _LL554: goto _LL555; _LL553: return 0; _LL555:(
void) _throw( _temp550); _LL551:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp557=({ struct _tuple8 _temp556; _temp556.f1= b1;
_temp556.f2= b2; _temp556;}); void* _temp567; void* _temp569; void* _temp571;
void* _temp573; void* _temp575; struct Cyc_Absyn_Exp* _temp577; void* _temp579;
struct Cyc_Absyn_Exp* _temp581; _LL559: _LL570: _temp569= _temp557.f1; if(
_temp569 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL568;} else{ goto _LL561;}
_LL568: _temp567= _temp557.f2; if( _temp567 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL560;} else{ goto _LL561;} _LL561: _LL572: _temp571= _temp557.f1; if(
_temp571 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL562;} else{ goto _LL563;}
_LL563: _LL574: _temp573= _temp557.f2; if( _temp573 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL564;} else{ goto _LL565;} _LL565: _LL580: _temp579= _temp557.f1; if((
unsigned int) _temp579 >  1u?*(( int*) _temp579) ==  Cyc_Absyn_Upper_b: 0){
_LL582: _temp581=(( struct Cyc_Absyn_Upper_b_struct*) _temp579)->f1; goto _LL576;}
else{ goto _LL558;} _LL576: _temp575= _temp557.f2; if(( unsigned int) _temp575 > 
1u?*(( int*) _temp575) ==  Cyc_Absyn_Upper_b: 0){ _LL578: _temp577=(( struct Cyc_Absyn_Upper_b_struct*)
_temp575)->f1; goto _LL566;} else{ goto _LL558;} _LL560: return 0; _LL562:
return - 1; _LL564: return 1; _LL566: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp581); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp577); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL558:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp584=({
struct _tuple8 _temp583; _temp583.f1= a1; _temp583.f2= a2; _temp583;}); void*
_temp596; int _temp598; int _temp600; void* _temp602; void* _temp604; int
_temp606; int _temp608; void* _temp610; void* _temp612; int _temp614; void*
_temp616; int _temp618; void* _temp620; int _temp622; void* _temp624; int
_temp626; void* _temp628; struct _tagged_arr _temp630; void* _temp632; struct
_tagged_arr _temp634; _LL586: _LL605: _temp604= _temp584.f1; if(( unsigned int)
_temp604 >  16u?*(( int*) _temp604) ==  Cyc_Absyn_Format_att: 0){ _LL611:
_temp610=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp604)->f1; goto
_LL609; _LL609: _temp608=(( struct Cyc_Absyn_Format_att_struct*) _temp604)->f2;
goto _LL607; _LL607: _temp606=(( struct Cyc_Absyn_Format_att_struct*) _temp604)->f3;
goto _LL597;} else{ goto _LL588;} _LL597: _temp596= _temp584.f2; if((
unsigned int) _temp596 >  16u?*(( int*) _temp596) ==  Cyc_Absyn_Format_att: 0){
_LL603: _temp602=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp596)->f1;
goto _LL601; _LL601: _temp600=(( struct Cyc_Absyn_Format_att_struct*) _temp596)->f2;
goto _LL599; _LL599: _temp598=(( struct Cyc_Absyn_Format_att_struct*) _temp596)->f3;
goto _LL587;} else{ goto _LL588;} _LL588: _LL617: _temp616= _temp584.f1; if((
unsigned int) _temp616 >  16u?*(( int*) _temp616) ==  Cyc_Absyn_Regparm_att: 0){
_LL619: _temp618=(( struct Cyc_Absyn_Regparm_att_struct*) _temp616)->f1; goto
_LL613;} else{ goto _LL590;} _LL613: _temp612= _temp584.f2; if(( unsigned int)
_temp612 >  16u?*(( int*) _temp612) ==  Cyc_Absyn_Regparm_att: 0){ _LL615:
_temp614=(( struct Cyc_Absyn_Regparm_att_struct*) _temp612)->f1; goto _LL589;}
else{ goto _LL590;} _LL590: _LL625: _temp624= _temp584.f1; if(( unsigned int)
_temp624 >  16u?*(( int*) _temp624) ==  Cyc_Absyn_Aligned_att: 0){ _LL627:
_temp626=(( struct Cyc_Absyn_Aligned_att_struct*) _temp624)->f1; goto _LL621;}
else{ goto _LL592;} _LL621: _temp620= _temp584.f2; if(( unsigned int) _temp620 > 
16u?*(( int*) _temp620) ==  Cyc_Absyn_Aligned_att: 0){ _LL623: _temp622=((
struct Cyc_Absyn_Aligned_att_struct*) _temp620)->f1; goto _LL591;} else{ goto
_LL592;} _LL592: _LL633: _temp632= _temp584.f1; if(( unsigned int) _temp632 > 
16u?*(( int*) _temp632) ==  Cyc_Absyn_Section_att: 0){ _LL635: _temp634=((
struct Cyc_Absyn_Section_att_struct*) _temp632)->f1; goto _LL629;} else{ goto
_LL594;} _LL629: _temp628= _temp584.f2; if(( unsigned int) _temp628 >  16u?*((
int*) _temp628) ==  Cyc_Absyn_Section_att: 0){ _LL631: _temp630=(( struct Cyc_Absyn_Section_att_struct*)
_temp628)->f1; goto _LL593;} else{ goto _LL594;} _LL594: goto _LL595; _LL587:
return( _temp610 ==  _temp602? _temp608 ==  _temp600: 0)? _temp606 ==  _temp598:
0; _LL589: _temp626= _temp618; _temp622= _temp614; goto _LL591; _LL591: return
_temp626 ==  _temp622; _LL593: return Cyc_Std_strcmp( _temp634, _temp630) ==  0;
_LL595: return 0; _LL585:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,(
void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct
Cyc_List_List* a= a2; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null(
a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void* Cyc_Tcutil_normalize_effect(
void* e); static void* Cyc_Tcutil_rgns_of( void* t); static void* Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Structfield* sf){ return Cyc_Tcutil_rgns_of(( void*) sf->type);}
static struct _tuple5* Cyc_Tcutil_region_free_subst( struct Cyc_Absyn_Tvar* tv){
void* t;{ void* _temp636= Cyc_Absyn_conref_val( tv->kind); _LL638: if( _temp636
== ( void*) Cyc_Absyn_RgnKind){ goto _LL639;} else{ goto _LL640;} _LL640: if(
_temp636 == ( void*) Cyc_Absyn_EffKind){ goto _LL641;} else{ goto _LL642;}
_LL642: goto _LL643; _LL639: t=( void*) Cyc_Absyn_HeapRgn; goto _LL637; _LL641:
t= Cyc_Absyn_empty_effect; goto _LL637; _LL643: t= Cyc_Absyn_sint_t; goto _LL637;
_LL637:;} return({ struct _tuple5* _temp644=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp644->f1= tv; _temp644->f2= t; _temp644;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp645= Cyc_Tcutil_compress(
t); void* _temp693; struct Cyc_Absyn_TunionInfo _temp695; void* _temp697; struct
Cyc_List_List* _temp699; struct Cyc_Absyn_PtrInfo _temp701; void* _temp703; void*
_temp705; void* _temp707; struct Cyc_List_List* _temp709; struct Cyc_Absyn_TunionFieldInfo
_temp711; struct Cyc_List_List* _temp713; struct Cyc_List_List* _temp715; struct
Cyc_List_List* _temp717; struct Cyc_List_List* _temp719; struct Cyc_List_List*
_temp721; struct Cyc_Absyn_FnInfo _temp723; struct Cyc_List_List* _temp725;
struct Cyc_Absyn_VarargInfo* _temp727; struct Cyc_List_List* _temp729; void*
_temp731; struct Cyc_Core_Opt* _temp733; struct Cyc_List_List* _temp735; void*
_temp737; _LL647: if( _temp645 == ( void*) Cyc_Absyn_VoidType){ goto _LL648;}
else{ goto _LL649;} _LL649: if( _temp645 == ( void*) Cyc_Absyn_FloatType){ goto
_LL650;} else{ goto _LL651;} _LL651: if( _temp645 == ( void*) Cyc_Absyn_DoubleType){
goto _LL652;} else{ goto _LL653;} _LL653: if(( unsigned int) _temp645 >  4u?*((
int*) _temp645) ==  Cyc_Absyn_EnumType: 0){ goto _LL654;} else{ goto _LL655;}
_LL655: if(( unsigned int) _temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_IntType:
0){ goto _LL656;} else{ goto _LL657;} _LL657: if(( unsigned int) _temp645 >  4u?*((
int*) _temp645) ==  Cyc_Absyn_Evar: 0){ goto _LL658;} else{ goto _LL659;} _LL659:
if(( unsigned int) _temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_VarType: 0){
goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int) _temp645 >  4u?*((
int*) _temp645) ==  Cyc_Absyn_RgnHandleType: 0){ _LL694: _temp693=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp645)->f1; goto _LL662;} else{ goto
_LL663;} _LL663: if(( unsigned int) _temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_TunionType:
0){ _LL696: _temp695=(( struct Cyc_Absyn_TunionType_struct*) _temp645)->f1;
_LL700: _temp699= _temp695.targs; goto _LL698; _LL698: _temp697=( void*)
_temp695.rgn; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_PointerType: 0){ _LL702:
_temp701=(( struct Cyc_Absyn_PointerType_struct*) _temp645)->f1; _LL706:
_temp705=( void*) _temp701.elt_typ; goto _LL704; _LL704: _temp703=( void*)
_temp701.rgn_typ; goto _LL666;} else{ goto _LL667;} _LL667: if(( unsigned int)
_temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_ArrayType: 0){ _LL708: _temp707=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp645)->f1; goto _LL668;} else{
goto _LL669;} _LL669: if(( unsigned int) _temp645 >  4u?*(( int*) _temp645) == 
Cyc_Absyn_TupleType: 0){ _LL710: _temp709=(( struct Cyc_Absyn_TupleType_struct*)
_temp645)->f1; goto _LL670;} else{ goto _LL671;} _LL671: if(( unsigned int)
_temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_TunionFieldType: 0){ _LL712:
_temp711=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp645)->f1; _LL714:
_temp713= _temp711.targs; goto _LL672;} else{ goto _LL673;} _LL673: if((
unsigned int) _temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_StructType: 0){
_LL716: _temp715=(( struct Cyc_Absyn_StructType_struct*) _temp645)->f2; goto
_LL674;} else{ goto _LL675;} _LL675: if(( unsigned int) _temp645 >  4u?*(( int*)
_temp645) ==  Cyc_Absyn_UnionType: 0){ _LL718: _temp717=(( struct Cyc_Absyn_UnionType_struct*)
_temp645)->f2; goto _LL676;} else{ goto _LL677;} _LL677: if(( unsigned int)
_temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_AnonStructType: 0){ _LL720:
_temp719=(( struct Cyc_Absyn_AnonStructType_struct*) _temp645)->f1; goto _LL678;}
else{ goto _LL679;} _LL679: if(( unsigned int) _temp645 >  4u?*(( int*) _temp645)
==  Cyc_Absyn_AnonUnionType: 0){ _LL722: _temp721=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp645)->f1; goto _LL680;} else{ goto _LL681;} _LL681: if(( unsigned int)
_temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_FnType: 0){ _LL724: _temp723=((
struct Cyc_Absyn_FnType_struct*) _temp645)->f1; _LL736: _temp735= _temp723.tvars;
goto _LL734; _LL734: _temp733= _temp723.effect; goto _LL732; _LL732: _temp731=(
void*) _temp723.ret_typ; goto _LL730; _LL730: _temp729= _temp723.args; goto
_LL728; _LL728: _temp727= _temp723.cyc_varargs; goto _LL726; _LL726: _temp725=
_temp723.rgn_po; goto _LL682;} else{ goto _LL683;} _LL683: if( _temp645 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL684;} else{ goto _LL685;} _LL685: if((
unsigned int) _temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_AccessEff: 0){
goto _LL686;} else{ goto _LL687;} _LL687: if(( unsigned int) _temp645 >  4u?*((
int*) _temp645) ==  Cyc_Absyn_JoinEff: 0){ goto _LL688;} else{ goto _LL689;}
_LL689: if(( unsigned int) _temp645 >  4u?*(( int*) _temp645) ==  Cyc_Absyn_RgnsEff:
0){ _LL738: _temp737=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp645)->f1;
goto _LL690;} else{ goto _LL691;} _LL691: if(( unsigned int) _temp645 >  4u?*((
int*) _temp645) ==  Cyc_Absyn_TypedefType: 0){ goto _LL692;} else{ goto _LL646;}
_LL648: goto _LL650; _LL650: goto _LL652; _LL652: goto _LL654; _LL654: goto
_LL656; _LL656: return Cyc_Absyn_empty_effect; _LL658: goto _LL660; _LL660: {
void* _temp739= Cyc_Tcutil_typ_kind( t); _LL741: if( _temp739 == ( void*) Cyc_Absyn_RgnKind){
goto _LL742;} else{ goto _LL743;} _LL743: if( _temp739 == ( void*) Cyc_Absyn_EffKind){
goto _LL744;} else{ goto _LL745;} _LL745: goto _LL746; _LL742: return( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp747=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp747[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp748; _temp748.tag= Cyc_Absyn_AccessEff; _temp748.f1=(
void*) t; _temp748;}); _temp747;}); _LL744: return t; _LL746: return( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp749=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp749[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp750; _temp750.tag= Cyc_Absyn_RgnsEff; _temp750.f1=( void*) t; _temp750;});
_temp749;}); _LL740:;} _LL662: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp751=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp751[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp752; _temp752.tag= Cyc_Absyn_AccessEff;
_temp752.f1=( void*) _temp693; _temp752;}); _temp751;}); _LL664: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp755=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp755->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp756=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp756[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp757; _temp757.tag= Cyc_Absyn_AccessEff;
_temp757.f1=( void*) _temp697; _temp757;}); _temp756;})); _temp755->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp699); _temp755;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp753=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp753[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp754; _temp754.tag= Cyc_Absyn_JoinEff; _temp754.f1= ts; _temp754;});
_temp753;}));} _LL666: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp758=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp758[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp759; _temp759.tag= Cyc_Absyn_JoinEff;
_temp759.f1=({ void* _temp760[ 2u]; _temp760[ 1u]= Cyc_Tcutil_rgns_of( _temp705);
_temp760[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp761=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp761[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp762; _temp762.tag= Cyc_Absyn_AccessEff;
_temp762.f1=( void*) _temp703; _temp762;}); _temp761;}); Cyc_List_list( _tag_arr(
_temp760, sizeof( void*), 2u));}); _temp759;}); _temp758;})); _LL668: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp707)); _LL670: { struct Cyc_List_List* _temp763= 0;
for( 0; _temp709 !=  0; _temp709=(( struct Cyc_List_List*) _check_null( _temp709))->tl){
_temp763=({ struct Cyc_List_List* _temp764=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp764->hd=( void*)(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp709))->hd)).f2; _temp764->tl= _temp763;
_temp764;});} _temp713= _temp763; goto _LL672;} _LL672: _temp715= _temp713; goto
_LL674; _LL674: _temp717= _temp715; goto _LL676; _LL676: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp765=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp765[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp766; _temp766.tag= Cyc_Absyn_JoinEff; _temp766.f1= Cyc_List_map( Cyc_Tcutil_rgns_of,
_temp717); _temp766;}); _temp765;})); _LL678: _temp721= _temp719; goto _LL680;
_LL680: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp767=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp767[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp768; _temp768.tag= Cyc_Absyn_JoinEff;
_temp768.f1=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_rgns_of_field, _temp721);
_temp768;}); _temp767;})); _LL682: { void* _temp769= Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst, _temp735),( void*)((
struct Cyc_Core_Opt*) _check_null( _temp733))->v); return Cyc_Tcutil_normalize_effect(
_temp769);} _LL684: return Cyc_Absyn_empty_effect; _LL686: goto _LL688; _LL688:
return t; _LL690: return Cyc_Tcutil_rgns_of( _temp737); _LL692: return({ void*
_temp770[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp770, sizeof( void*), 0u));});
_LL646:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp771= e; struct Cyc_List_List* _temp779; struct Cyc_List_List**
_temp781; void* _temp782; _LL773: if(( unsigned int) _temp771 >  4u?*(( int*)
_temp771) ==  Cyc_Absyn_JoinEff: 0){ _LL780: _temp779=(( struct Cyc_Absyn_JoinEff_struct*)
_temp771)->f1; _temp781=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp771)->f1; goto _LL774;} else{ goto _LL775;} _LL775: if(( unsigned int)
_temp771 >  4u?*(( int*) _temp771) ==  Cyc_Absyn_RgnsEff: 0){ _LL783: _temp782=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp771)->f1; goto _LL776;} else{
goto _LL777;} _LL777: goto _LL778; _LL774: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp781; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp784=( void*)(( struct Cyc_List_List*) _check_null( effs))->hd;(
void*)((( struct Cyc_List_List*) _check_null( effs))->hd=( void*) Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect( _temp784)));{ void* _temp785=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; _LL787: if(( unsigned int) _temp785 >  4u?*(( int*)
_temp785) ==  Cyc_Absyn_JoinEff: 0){ goto _LL788;} else{ goto _LL789;} _LL789:
goto _LL790; _LL788: nested_join= 1; goto _LL786; _LL790: goto _LL786; _LL786:;}}}
if( ! nested_join){ return e;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp781; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp791= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp799; void* _temp801; _LL793:
if(( unsigned int) _temp791 >  4u?*(( int*) _temp791) ==  Cyc_Absyn_JoinEff: 0){
_LL800: _temp799=(( struct Cyc_Absyn_JoinEff_struct*) _temp791)->f1; goto _LL794;}
else{ goto _LL795;} _LL795: if(( unsigned int) _temp791 >  4u?*(( int*) _temp791)
==  Cyc_Absyn_AccessEff: 0){ _LL802: _temp801=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp791)->f1; if( _temp801 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL796;} else{
goto _LL797;}} else{ goto _LL797;} _LL797: goto _LL798; _LL794: effects= Cyc_List_revappend(
_temp799, effects); goto _LL792; _LL796: goto _LL792; _LL798: effects=({ struct
Cyc_List_List* _temp803=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp803->hd=( void*) _temp791; _temp803->tl= effects; _temp803;}); goto _LL792;
_LL792:;}}* _temp781= Cyc_List_imp_rev( effects); return e;}} _LL776: return Cyc_Tcutil_rgns_of(
_temp782); _LL778: return e; _LL772:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp804= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp812; struct Cyc_List_List _temp814; struct Cyc_List_List* _temp815; void*
_temp817; struct Cyc_Core_Opt* _temp819; struct Cyc_Core_Opt* _temp821; _LL806:
if(( unsigned int) _temp804 >  4u?*(( int*) _temp804) ==  Cyc_Absyn_JoinEff: 0){
_LL813: _temp812=(( struct Cyc_Absyn_JoinEff_struct*) _temp804)->f1; if(
_temp812 ==  0){ goto _LL808;} else{ _temp814=* _temp812; _LL818: _temp817=(
void*) _temp814.hd; goto _LL816; _LL816: _temp815= _temp814.tl; goto _LL807;}}
else{ goto _LL808;} _LL808: if(( unsigned int) _temp804 >  4u?*(( int*) _temp804)
==  Cyc_Absyn_Evar: 0){ _LL822: _temp821=(( struct Cyc_Absyn_Evar_struct*)
_temp804)->f1; goto _LL820; _LL820: _temp819=(( struct Cyc_Absyn_Evar_struct*)
_temp804)->f4; goto _LL809;} else{ goto _LL810;} _LL810: goto _LL811; _LL807: {
void* _temp823= Cyc_Tcutil_compress( _temp817); struct Cyc_Core_Opt* _temp829;
_LL825: if(( unsigned int) _temp823 >  4u?*(( int*) _temp823) ==  Cyc_Absyn_Evar:
0){ _LL830: _temp829=(( struct Cyc_Absyn_Evar_struct*) _temp823)->f4; goto
_LL826;} else{ goto _LL827;} _LL827: goto _LL828; _LL826: return({ struct
_tuple9* _temp831=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp831->f1= _temp817; _temp831->f2= _temp815; _temp831->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp829))->v; _temp831;}); _LL828: return 0;
_LL824:;} _LL809: if( _temp821 ==  0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp821))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp832[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp832, sizeof( void*), 0u));});} return({ struct _tuple9* _temp833=( struct
_tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp833->f1= t; _temp833->f2= 0;
_temp833->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp819))->v; _temp833;}); _LL811: return 0; _LL805:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp834=({ struct Cyc_Absyn_FnType_struct*
_temp835=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp835[ 0]=({ struct Cyc_Absyn_FnType_struct _temp836; _temp836.tag= Cyc_Absyn_FnType;
_temp836.f1=({ struct Cyc_Absyn_FnInfo _temp837; _temp837.tvars= 0; _temp837.effect=({
struct Cyc_Core_Opt* _temp838=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp838->v=( void*) eff; _temp838;}); _temp837.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp837.args= 0; _temp837.c_varargs= 0; _temp837.cyc_varargs=
0; _temp837.rgn_po= 0; _temp837.attributes= 0; _temp837;}); _temp836;});
_temp835;}); return Cyc_Absyn_atb_typ(( void*) _temp834,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp839= Cyc_Tcutil_compress( e); void*
_temp851; struct Cyc_List_List* _temp853; void* _temp855; struct Cyc_Core_Opt*
_temp857; struct Cyc_Core_Opt* _temp859; struct Cyc_Core_Opt** _temp861; struct
Cyc_Core_Opt* _temp862; _LL841: if(( unsigned int) _temp839 >  4u?*(( int*)
_temp839) ==  Cyc_Absyn_AccessEff: 0){ _LL852: _temp851=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp839)->f1; goto _LL842;} else{ goto _LL843;} _LL843: if(( unsigned int)
_temp839 >  4u?*(( int*) _temp839) ==  Cyc_Absyn_JoinEff: 0){ _LL854: _temp853=((
struct Cyc_Absyn_JoinEff_struct*) _temp839)->f1; goto _LL844;} else{ goto _LL845;}
_LL845: if(( unsigned int) _temp839 >  4u?*(( int*) _temp839) ==  Cyc_Absyn_RgnsEff:
0){ _LL856: _temp855=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp839)->f1;
goto _LL846;} else{ goto _LL847;} _LL847: if(( unsigned int) _temp839 >  4u?*((
int*) _temp839) ==  Cyc_Absyn_Evar: 0){ _LL863: _temp862=(( struct Cyc_Absyn_Evar_struct*)
_temp839)->f1; goto _LL860; _LL860: _temp859=(( struct Cyc_Absyn_Evar_struct*)
_temp839)->f2; _temp861=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp839)->f2; goto _LL858; _LL858: _temp857=(( struct Cyc_Absyn_Evar_struct*)
_temp839)->f4; goto _LL848;} else{ goto _LL849;} _LL849: goto _LL850; _LL842:
if( constrain){ return Cyc_Tcutil_unify( r, _temp851);} _temp851= Cyc_Tcutil_compress(
_temp851); if( r ==  _temp851){ return 1;}{ struct _tuple8 _temp865=({ struct
_tuple8 _temp864; _temp864.f1= r; _temp864.f2= _temp851; _temp864;}); void*
_temp871; struct Cyc_Absyn_Tvar* _temp873; void* _temp875; struct Cyc_Absyn_Tvar*
_temp877; _LL867: _LL876: _temp875= _temp865.f1; if(( unsigned int) _temp875 > 
4u?*(( int*) _temp875) ==  Cyc_Absyn_VarType: 0){ _LL878: _temp877=(( struct Cyc_Absyn_VarType_struct*)
_temp875)->f1; goto _LL872;} else{ goto _LL869;} _LL872: _temp871= _temp865.f2;
if(( unsigned int) _temp871 >  4u?*(( int*) _temp871) ==  Cyc_Absyn_VarType: 0){
_LL874: _temp873=(( struct Cyc_Absyn_VarType_struct*) _temp871)->f1; goto _LL868;}
else{ goto _LL869;} _LL869: goto _LL870; _LL868: return Cyc_Absyn_tvar_cmp(
_temp877, _temp873) ==  0; _LL870: return 0; _LL866:;} _LL844: for( 0; _temp853
!=  0; _temp853=(( struct Cyc_List_List*) _check_null( _temp853))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp853))->hd)){
return 1;}} return 0; _LL846: { void* _temp879= Cyc_Tcutil_rgns_of( _temp855);
void* _temp885; _LL881: if(( unsigned int) _temp879 >  4u?*(( int*) _temp879) == 
Cyc_Absyn_RgnsEff: 0){ _LL886: _temp885=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp879)->f1; goto _LL882;} else{ goto _LL883;} _LL883: goto _LL884; _LL882:
if( ! constrain){ return 0;}{ void* _temp887= Cyc_Tcutil_compress( _temp885);
struct Cyc_Core_Opt* _temp893; struct Cyc_Core_Opt* _temp895; struct Cyc_Core_Opt**
_temp897; struct Cyc_Core_Opt* _temp898; _LL889: if(( unsigned int) _temp887 > 
4u?*(( int*) _temp887) ==  Cyc_Absyn_Evar: 0){ _LL899: _temp898=(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f1; goto _LL896; _LL896: _temp895=(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f2; _temp897=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f2; goto _LL894; _LL894: _temp893=(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f4; goto _LL890;} else{ goto _LL891;} _LL891: goto _LL892; _LL890: {
void* _temp900= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp893); Cyc_Tcutil_occurs( _temp900,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp893))->v, r);{ void* _temp901= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp903=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp903[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp904; _temp904.tag= Cyc_Absyn_JoinEff; _temp904.f1=({ void* _temp905[ 2u];
_temp905[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp906=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp906[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp907; _temp907.tag= Cyc_Absyn_AccessEff;
_temp907.f1=( void*) r; _temp907;}); _temp906;}); _temp905[ 0u]= _temp900; Cyc_List_list(
_tag_arr( _temp905, sizeof( void*), 2u));}); _temp904;}); _temp903;}));*
_temp897=({ struct Cyc_Core_Opt* _temp902=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp902->v=( void*) _temp901; _temp902;});
return 1;}} _LL892: return 0; _LL888:;} _LL884: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp879); _LL880:;} _LL848: if( _temp862 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp862))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp908[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp908, sizeof( void*), 0u));});} if( ! constrain){ return 0;}{
void* _temp909= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp857); Cyc_Tcutil_occurs( _temp909,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp857))->v, r);{ struct Cyc_Absyn_JoinEff_struct* _temp910=({
struct Cyc_Absyn_JoinEff_struct* _temp912=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp912[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp913; _temp913.tag= Cyc_Absyn_JoinEff; _temp913.f1=({ struct Cyc_List_List*
_temp914=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp914->hd=( void*) _temp909; _temp914->tl=({ struct Cyc_List_List* _temp915=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp915->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp916=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp916[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp917; _temp917.tag= Cyc_Absyn_AccessEff; _temp917.f1=(
void*) r; _temp917;}); _temp916;})); _temp915->tl= 0; _temp915;}); _temp914;});
_temp913;}); _temp912;});* _temp861=({ struct Cyc_Core_Opt* _temp911=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp911->v=( void*)((
void*) _temp910); _temp911;}); return 1;}} _LL850: return 0; _LL840:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp918= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp930;
void* _temp932; struct Cyc_Core_Opt* _temp934; struct Cyc_Core_Opt* _temp936;
struct Cyc_Core_Opt** _temp938; struct Cyc_Core_Opt* _temp939; _LL920: if((
unsigned int) _temp918 >  4u?*(( int*) _temp918) ==  Cyc_Absyn_AccessEff: 0){
goto _LL921;} else{ goto _LL922;} _LL922: if(( unsigned int) _temp918 >  4u?*((
int*) _temp918) ==  Cyc_Absyn_JoinEff: 0){ _LL931: _temp930=(( struct Cyc_Absyn_JoinEff_struct*)
_temp918)->f1; goto _LL923;} else{ goto _LL924;} _LL924: if(( unsigned int)
_temp918 >  4u?*(( int*) _temp918) ==  Cyc_Absyn_RgnsEff: 0){ _LL933: _temp932=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp918)->f1; goto _LL925;} else{
goto _LL926;} _LL926: if(( unsigned int) _temp918 >  4u?*(( int*) _temp918) == 
Cyc_Absyn_Evar: 0){ _LL940: _temp939=(( struct Cyc_Absyn_Evar_struct*) _temp918)->f1;
goto _LL937; _LL937: _temp936=(( struct Cyc_Absyn_Evar_struct*) _temp918)->f2;
_temp938=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp918)->f2;
goto _LL935; _LL935: _temp934=(( struct Cyc_Absyn_Evar_struct*) _temp918)->f4;
goto _LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL921: return 0; _LL923:
for( 0; _temp930 !=  0; _temp930=(( struct Cyc_List_List*) _check_null( _temp930))->tl){
if( Cyc_Tcutil_type_in_effect( constrain, t,( void*)(( struct Cyc_List_List*)
_check_null( _temp930))->hd)){ return 1;}} return 0; _LL925: _temp932= Cyc_Tcutil_compress(
_temp932); if( t ==  _temp932){ return 1;} if( constrain){ return Cyc_Tcutil_unify(
t, _temp932);}{ void* _temp941= Cyc_Tcutil_rgns_of( t); void* _temp947; _LL943:
if(( unsigned int) _temp941 >  4u?*(( int*) _temp941) ==  Cyc_Absyn_RgnsEff: 0){
_LL948: _temp947=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp941)->f1;
goto _LL944;} else{ goto _LL945;} _LL945: goto _LL946; _LL944: { struct _tuple8
_temp950=({ struct _tuple8 _temp949; _temp949.f1= t; _temp949.f2= Cyc_Tcutil_compress(
_temp947); _temp949;}); void* _temp956; struct Cyc_Absyn_Tvar* _temp958; void*
_temp960; struct Cyc_Absyn_Tvar* _temp962; _LL952: _LL961: _temp960= _temp950.f1;
if(( unsigned int) _temp960 >  4u?*(( int*) _temp960) ==  Cyc_Absyn_VarType: 0){
_LL963: _temp962=(( struct Cyc_Absyn_VarType_struct*) _temp960)->f1; goto _LL957;}
else{ goto _LL954;} _LL957: _temp956= _temp950.f2; if(( unsigned int) _temp956 > 
4u?*(( int*) _temp956) ==  Cyc_Absyn_VarType: 0){ _LL959: _temp958=(( struct Cyc_Absyn_VarType_struct*)
_temp956)->f1; goto _LL953;} else{ goto _LL954;} _LL954: goto _LL955; _LL953:
return Cyc_Tcutil_unify( t, _temp947); _LL955: return t ==  _temp947; _LL951:;}
_LL946: return Cyc_Tcutil_type_in_effect( constrain, t, _temp941); _LL942:;}
_LL927: if( _temp939 ==  0? 1:( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp939))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp964[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp964, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp965= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp934); Cyc_Tcutil_occurs( _temp965,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp934))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp966=({ struct Cyc_Absyn_JoinEff_struct* _temp968=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp968[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp969; _temp969.tag= Cyc_Absyn_JoinEff; _temp969.f1=({ struct Cyc_List_List*
_temp970=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp970->hd=( void*) _temp965; _temp970->tl=({ struct Cyc_List_List* _temp971=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp971->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp972=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp972[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp973; _temp973.tag= Cyc_Absyn_RgnsEff; _temp973.f1=( void*) t; _temp973;});
_temp972;})); _temp971->tl= 0; _temp971;}); _temp970;}); _temp969;}); _temp968;});*
_temp938=({ struct Cyc_Core_Opt* _temp967=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp967->v=( void*)(( void*) _temp966); _temp967;});
return 1;}} _LL929: return 0; _LL919:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp974= e; struct Cyc_Absyn_Tvar* _temp986; struct Cyc_List_List*
_temp988; void* _temp990; struct Cyc_Core_Opt* _temp992; struct Cyc_Core_Opt*
_temp994; struct Cyc_Core_Opt** _temp996; struct Cyc_Core_Opt* _temp997; _LL976:
if(( unsigned int) _temp974 >  4u?*(( int*) _temp974) ==  Cyc_Absyn_VarType: 0){
_LL987: _temp986=(( struct Cyc_Absyn_VarType_struct*) _temp974)->f1; goto _LL977;}
else{ goto _LL978;} _LL978: if(( unsigned int) _temp974 >  4u?*(( int*) _temp974)
==  Cyc_Absyn_JoinEff: 0){ _LL989: _temp988=(( struct Cyc_Absyn_JoinEff_struct*)
_temp974)->f1; goto _LL979;} else{ goto _LL980;} _LL980: if(( unsigned int)
_temp974 >  4u?*(( int*) _temp974) ==  Cyc_Absyn_RgnsEff: 0){ _LL991: _temp990=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp974)->f1; goto _LL981;} else{
goto _LL982;} _LL982: if(( unsigned int) _temp974 >  4u?*(( int*) _temp974) == 
Cyc_Absyn_Evar: 0){ _LL998: _temp997=(( struct Cyc_Absyn_Evar_struct*) _temp974)->f1;
goto _LL995; _LL995: _temp994=(( struct Cyc_Absyn_Evar_struct*) _temp974)->f2;
_temp996=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp974)->f2;
goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_Evar_struct*) _temp974)->f4;
goto _LL983;} else{ goto _LL984;} _LL984: goto _LL985; _LL977: return Cyc_Absyn_tvar_cmp(
v, _temp986) ==  0; _LL979: for( 0; _temp988 !=  0; _temp988=(( struct Cyc_List_List*)
_check_null( _temp988))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp988))->hd)){ return 1;}}
return 0; _LL981: { void* _temp999= Cyc_Tcutil_rgns_of( _temp990); void*
_temp1005; _LL1001: if(( unsigned int) _temp999 >  4u?*(( int*) _temp999) == 
Cyc_Absyn_RgnsEff: 0){ _LL1006: _temp1005=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp999)->f1; goto _LL1002;} else{ goto _LL1003;} _LL1003: goto _LL1004;
_LL1002: if( ! constrain){ return 0;}{ void* _temp1007= Cyc_Tcutil_compress(
_temp1005); struct Cyc_Core_Opt* _temp1013; struct Cyc_Core_Opt* _temp1015;
struct Cyc_Core_Opt** _temp1017; struct Cyc_Core_Opt* _temp1018; _LL1009: if((
unsigned int) _temp1007 >  4u?*(( int*) _temp1007) ==  Cyc_Absyn_Evar: 0){
_LL1019: _temp1018=(( struct Cyc_Absyn_Evar_struct*) _temp1007)->f1; goto
_LL1016; _LL1016: _temp1015=(( struct Cyc_Absyn_Evar_struct*) _temp1007)->f2;
_temp1017=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1007)->f2;
goto _LL1014; _LL1014: _temp1013=(( struct Cyc_Absyn_Evar_struct*) _temp1007)->f4;
goto _LL1010;} else{ goto _LL1011;} _LL1011: goto _LL1012; _LL1010: { void*
_temp1020= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp1013);
if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1013))->v, v)){
return 0;}{ void* _temp1021= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1023=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1023[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1024; _temp1024.tag= Cyc_Absyn_JoinEff;
_temp1024.f1=({ void* _temp1025[ 2u]; _temp1025[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1026=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1026[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1027; _temp1027.tag= Cyc_Absyn_VarType;
_temp1027.f1= v; _temp1027;}); _temp1026;}); _temp1025[ 0u]= _temp1020; Cyc_List_list(
_tag_arr( _temp1025, sizeof( void*), 2u));}); _temp1024;}); _temp1023;}));*
_temp1017=({ struct Cyc_Core_Opt* _temp1022=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1022->v=( void*) _temp1021; _temp1022;});
return 1;}} _LL1012: return 0; _LL1008:;} _LL1004: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp999); _LL1000:;} _LL983: if( _temp997 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp997))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp1028[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp1028, sizeof( void*), 0u));});}{ void* _temp1029= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp992); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp992))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp1030=({ struct Cyc_Absyn_JoinEff_struct* _temp1032=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1032[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1033; _temp1033.tag= Cyc_Absyn_JoinEff; _temp1033.f1=({
struct Cyc_List_List* _temp1034=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1034->hd=( void*) _temp1029; _temp1034->tl=({
struct Cyc_List_List* _temp1035=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1035->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1036=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1036[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1037; _temp1037.tag= Cyc_Absyn_VarType;
_temp1037.f1= v; _temp1037;}); _temp1036;})); _temp1035->tl= 0; _temp1035;});
_temp1034;}); _temp1033;}); _temp1032;});* _temp996=({ struct Cyc_Core_Opt*
_temp1031=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1031->v=( void*)(( void*) _temp1030); _temp1031;}); return 1;}} _LL985:
return 0; _LL975:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp1038= e; struct Cyc_List_List* _temp1048;
void* _temp1050; _LL1040: if(( unsigned int) _temp1038 >  4u?*(( int*) _temp1038)
==  Cyc_Absyn_JoinEff: 0){ _LL1049: _temp1048=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1038)->f1; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(( unsigned int)
_temp1038 >  4u?*(( int*) _temp1038) ==  Cyc_Absyn_RgnsEff: 0){ _LL1051:
_temp1050=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1038)->f1; goto
_LL1043;} else{ goto _LL1044;} _LL1044: if(( unsigned int) _temp1038 >  4u?*((
int*) _temp1038) ==  Cyc_Absyn_Evar: 0){ goto _LL1045;} else{ goto _LL1046;}
_LL1046: goto _LL1047; _LL1041: for( 0; _temp1048 !=  0; _temp1048=(( struct Cyc_List_List*)
_check_null( _temp1048))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp1048))->hd)){ return 1;}} return 0;
_LL1043: { void* _temp1052= Cyc_Tcutil_rgns_of( _temp1050); void* _temp1058;
_LL1054: if(( unsigned int) _temp1052 >  4u?*(( int*) _temp1052) ==  Cyc_Absyn_RgnsEff:
0){ _LL1059: _temp1058=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1052)->f1;
goto _LL1055;} else{ goto _LL1056;} _LL1056: goto _LL1057; _LL1055: return 0;
_LL1057: return Cyc_Tcutil_evar_in_effect( evar, _temp1052); _LL1053:;} _LL1045:
return evar ==  e; _LL1047: return 0; _LL1039:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1060= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1074; void* _temp1076; struct Cyc_Absyn_Tvar*
_temp1078; void* _temp1080; struct Cyc_Core_Opt* _temp1082; struct Cyc_Core_Opt*
_temp1084; struct Cyc_Core_Opt** _temp1086; _LL1062: if(( unsigned int)
_temp1060 >  4u?*(( int*) _temp1060) ==  Cyc_Absyn_JoinEff: 0){ _LL1075:
_temp1074=(( struct Cyc_Absyn_JoinEff_struct*) _temp1060)->f1; goto _LL1063;}
else{ goto _LL1064;} _LL1064: if(( unsigned int) _temp1060 >  4u?*(( int*)
_temp1060) ==  Cyc_Absyn_AccessEff: 0){ _LL1077: _temp1076=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1060)->f1; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(( unsigned int)
_temp1060 >  4u?*(( int*) _temp1060) ==  Cyc_Absyn_VarType: 0){ _LL1079:
_temp1078=(( struct Cyc_Absyn_VarType_struct*) _temp1060)->f1; goto _LL1067;}
else{ goto _LL1068;} _LL1068: if(( unsigned int) _temp1060 >  4u?*(( int*)
_temp1060) ==  Cyc_Absyn_RgnsEff: 0){ _LL1081: _temp1080=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1060)->f1; goto _LL1069;} else{ goto _LL1070;} _LL1070: if(( unsigned int)
_temp1060 >  4u?*(( int*) _temp1060) ==  Cyc_Absyn_Evar: 0){ _LL1085: _temp1084=((
struct Cyc_Absyn_Evar_struct*) _temp1060)->f2; _temp1086=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1060)->f2; goto _LL1083; _LL1083: _temp1082=((
struct Cyc_Absyn_Evar_struct*) _temp1060)->f4; goto _LL1071;} else{ goto _LL1072;}
_LL1072: goto _LL1073; _LL1063: for( 0; _temp1074 !=  0; _temp1074=(( struct Cyc_List_List*)
_check_null( _temp1074))->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*)(( struct Cyc_List_List*) _check_null( _temp1074))->hd, e2)){ return 0;}}
return 1; _LL1065: return( Cyc_Tcutil_region_in_effect( 0, _temp1076, e2)? 1:
Cyc_Tcutil_region_in_effect( 1, _temp1076, e2))? 1: Cyc_Tcutil_unify( _temp1076,(
void*) Cyc_Absyn_HeapRgn); _LL1067: return Cyc_Tcutil_variable_in_effect( 0,
_temp1078, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1078, e2); _LL1069: {
void* _temp1087= Cyc_Tcutil_rgns_of( _temp1080); void* _temp1093; _LL1089: if((
unsigned int) _temp1087 >  4u?*(( int*) _temp1087) ==  Cyc_Absyn_RgnsEff: 0){
_LL1094: _temp1093=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1087)->f1;
goto _LL1090;} else{ goto _LL1091;} _LL1091: goto _LL1092; _LL1090: return( Cyc_Tcutil_type_in_effect(
0, _temp1093, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1093, e2))? 1: Cyc_Tcutil_unify(
_temp1093, Cyc_Absyn_sint_t); _LL1092: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1087, e2); _LL1088:;} _LL1071: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1086=({ struct Cyc_Core_Opt* _temp1095=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1095->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1096=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1096[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1097; _temp1097.tag= Cyc_Absyn_JoinEff; _temp1097.f1=
0; _temp1097;}); _temp1096;})); _temp1095;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1082))->v, e2);*
_temp1086=({ struct Cyc_Core_Opt* _temp1098=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1098->v=( void*) e2; _temp1098;});}} return
1; _LL1073: return({ struct Cyc_Std_String_pa_struct _temp1100; _temp1100.tag=
Cyc_Std_String_pa; _temp1100.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1099[ 1u]={& _temp1100};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1099, sizeof( void*), 1u));}});
_LL1061:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1102=({ struct
_tuple10 _temp1101; _temp1101.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1101.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1101;}); _LL1104: goto _LL1105; _LL1105:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1103:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8 _temp1108;
void* _temp1109; void* _temp1111; struct _tuple8* _temp1106=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( r1))->hd; _temp1108=* _temp1106; _LL1112:
_temp1111= _temp1108.f1; goto _LL1110; _LL1110: _temp1109= _temp1108.f2; goto
_LL1107; _LL1107: { int found= _temp1111 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2=(( struct Cyc_List_List*)
_check_null( r2))->tl){ struct _tuple8 _temp1115; void* _temp1116; void*
_temp1118; struct _tuple8* _temp1113=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( r2))->hd; _temp1115=* _temp1113; _LL1119: _temp1118= _temp1115.f1;
goto _LL1117; _LL1117: _temp1116= _temp1115.f2; goto _LL1114; _LL1114: if( Cyc_Tcutil_unify(
_temp1111, _temp1118)? Cyc_Tcutil_unify( _temp1109, _temp1116): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason=(
struct _tagged_arr) _tag_arr( 0u, 0u, 0u); t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2); if( t1 ==  t2){ return;}{ void* _temp1120= t1; struct Cyc_Core_Opt*
_temp1126; struct Cyc_Core_Opt* _temp1128; struct Cyc_Core_Opt** _temp1130;
struct Cyc_Core_Opt* _temp1131; _LL1122: if(( unsigned int) _temp1120 >  4u?*((
int*) _temp1120) ==  Cyc_Absyn_Evar: 0){ _LL1132: _temp1131=(( struct Cyc_Absyn_Evar_struct*)
_temp1120)->f1; goto _LL1129; _LL1129: _temp1128=(( struct Cyc_Absyn_Evar_struct*)
_temp1120)->f2; _temp1130=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1120)->f2; goto _LL1127; _LL1127: _temp1126=(( struct Cyc_Absyn_Evar_struct*)
_temp1120)->f4; goto _LL1123;} else{ goto _LL1124;} _LL1124: goto _LL1125;
_LL1123: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1126))->v, t2);{ void* _temp1133= Cyc_Tcutil_typ_kind( t2);
if( Cyc_Tcutil_kind_leq( _temp1133,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1131))->v)){* _temp1130=({ struct Cyc_Core_Opt* _temp1134=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1134->v=( void*) t2; _temp1134;});
return;} else{{ void* _temp1135= t2; struct Cyc_Core_Opt* _temp1143; struct Cyc_Core_Opt*
_temp1145; struct Cyc_Core_Opt** _temp1147; struct Cyc_Absyn_PtrInfo _temp1149;
_LL1137: if(( unsigned int) _temp1135 >  4u?*(( int*) _temp1135) ==  Cyc_Absyn_Evar:
0){ _LL1146: _temp1145=(( struct Cyc_Absyn_Evar_struct*) _temp1135)->f2;
_temp1147=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1135)->f2;
goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_Evar_struct*) _temp1135)->f4;
goto _LL1138;} else{ goto _LL1139;} _LL1139: if(( unsigned int) _temp1135 >  4u?*((
int*) _temp1135) ==  Cyc_Absyn_PointerType: 0){ _LL1150: _temp1149=(( struct Cyc_Absyn_PointerType_struct*)
_temp1135)->f1; goto _LL1148;} else{ goto _LL1141;} _LL1148: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1131))->v == ( void*) Cyc_Absyn_BoxKind){
goto _LL1140;} else{ goto _LL1141;} _LL1141: goto _LL1142; _LL1138: { struct Cyc_List_List*
_temp1151=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1126))->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1143))->v; for( 0; s2 !=  0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1151,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if(
Cyc_Tcutil_kind_leq(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1131))->v,
_temp1133)){* _temp1147=({ struct Cyc_Core_Opt* _temp1152=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1152->v=( void*) t1; _temp1152;});
return;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)", sizeof(
unsigned char), 25u); goto _LL1136;} _LL1140: { struct Cyc_Absyn_Conref*
_temp1153= Cyc_Absyn_compress_conref( _temp1149.bounds);{ void* _temp1154=( void*)
_temp1153->v; _LL1156: if( _temp1154 == ( void*) Cyc_Absyn_No_constr){ goto
_LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159; _LL1157:( void*)(
_temp1153->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1160=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1160[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp1161; _temp1161.tag= Cyc_Absyn_Eq_constr;
_temp1161.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1162=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1162[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1163; _temp1163.tag= Cyc_Absyn_Upper_b;
_temp1163.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1163;}); _temp1162;}));
_temp1161;}); _temp1160;})));* _temp1130=({ struct Cyc_Core_Opt* _temp1164=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1164->v=(
void*) t2; _temp1164;}); return; _LL1159: goto _LL1155; _LL1155:;} goto _LL1136;}
_LL1142: goto _LL1136; _LL1136:;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} _LL1125:
goto _LL1121; _LL1121:;}{ struct _tuple8 _temp1166=({ struct _tuple8 _temp1165;
_temp1165.f1= t2; _temp1165.f2= t1; _temp1165;}); void* _temp1220; void*
_temp1222; void* _temp1224; void* _temp1226; struct Cyc_Absyn_Tvar* _temp1228;
void* _temp1230; struct Cyc_Absyn_Tvar* _temp1232; void* _temp1234; struct Cyc_Absyn_Structdecl**
_temp1236; struct Cyc_List_List* _temp1238; struct _tuple1* _temp1240; void*
_temp1242; struct Cyc_Absyn_Structdecl** _temp1244; struct Cyc_List_List*
_temp1246; struct _tuple1* _temp1248; void* _temp1250; struct _tuple1* _temp1252;
void* _temp1254; struct _tuple1* _temp1256; void* _temp1258; struct Cyc_Absyn_Uniondecl**
_temp1260; struct Cyc_List_List* _temp1262; struct _tuple1* _temp1264; void*
_temp1266; struct Cyc_Absyn_Uniondecl** _temp1268; struct Cyc_List_List*
_temp1270; struct _tuple1* _temp1272; void* _temp1274; struct Cyc_Absyn_TunionInfo
_temp1276; void* _temp1278; struct Cyc_List_List* _temp1280; void* _temp1282;
struct Cyc_Absyn_Tuniondecl** _temp1284; struct Cyc_Absyn_Tuniondecl* _temp1286;
void* _temp1287; struct Cyc_Absyn_TunionInfo _temp1289; void* _temp1291; struct
Cyc_List_List* _temp1293; void* _temp1295; struct Cyc_Absyn_Tuniondecl**
_temp1297; struct Cyc_Absyn_Tuniondecl* _temp1299; void* _temp1300; struct Cyc_Absyn_TunionFieldInfo
_temp1302; struct Cyc_List_List* _temp1304; void* _temp1306; struct Cyc_Absyn_Tunionfield*
_temp1308; struct Cyc_Absyn_Tuniondecl* _temp1310; void* _temp1312; struct Cyc_Absyn_TunionFieldInfo
_temp1314; struct Cyc_List_List* _temp1316; void* _temp1318; struct Cyc_Absyn_Tunionfield*
_temp1320; struct Cyc_Absyn_Tuniondecl* _temp1322; void* _temp1324; struct Cyc_Absyn_PtrInfo
_temp1326; struct Cyc_Absyn_Conref* _temp1328; struct Cyc_Absyn_Tqual _temp1330;
struct Cyc_Absyn_Conref* _temp1332; void* _temp1334; void* _temp1336; void*
_temp1338; struct Cyc_Absyn_PtrInfo _temp1340; struct Cyc_Absyn_Conref*
_temp1342; struct Cyc_Absyn_Tqual _temp1344; struct Cyc_Absyn_Conref* _temp1346;
void* _temp1348; void* _temp1350; void* _temp1352; void* _temp1354; void*
_temp1356; void* _temp1358; void* _temp1360; void* _temp1362; void* _temp1364;
void* _temp1366; void* _temp1368; void* _temp1370; void* _temp1372; struct Cyc_Absyn_Exp*
_temp1374; struct Cyc_Absyn_Tqual _temp1376; void* _temp1378; void* _temp1380;
struct Cyc_Absyn_Exp* _temp1382; struct Cyc_Absyn_Tqual _temp1384; void*
_temp1386; void* _temp1388; struct Cyc_Absyn_FnInfo _temp1390; struct Cyc_List_List*
_temp1392; struct Cyc_List_List* _temp1394; struct Cyc_Absyn_VarargInfo*
_temp1396; int _temp1398; struct Cyc_List_List* _temp1400; void* _temp1402;
struct Cyc_Core_Opt* _temp1404; struct Cyc_List_List* _temp1406; void* _temp1408;
struct Cyc_Absyn_FnInfo _temp1410; struct Cyc_List_List* _temp1412; struct Cyc_List_List*
_temp1414; struct Cyc_Absyn_VarargInfo* _temp1416; int _temp1418; struct Cyc_List_List*
_temp1420; void* _temp1422; struct Cyc_Core_Opt* _temp1424; struct Cyc_List_List*
_temp1426; void* _temp1428; struct Cyc_List_List* _temp1430; void* _temp1432;
struct Cyc_List_List* _temp1434; void* _temp1436; struct Cyc_List_List*
_temp1438; void* _temp1440; struct Cyc_List_List* _temp1442; void* _temp1444;
struct Cyc_List_List* _temp1446; void* _temp1448; struct Cyc_List_List*
_temp1450; void* _temp1452; void* _temp1454; void* _temp1456; void* _temp1458;
void* _temp1460; void* _temp1462; void* _temp1464; void* _temp1466; void*
_temp1468; void* _temp1470; void* _temp1472; void* _temp1474; _LL1168: _LL1221:
_temp1220= _temp1166.f1; if(( unsigned int) _temp1220 >  4u?*(( int*) _temp1220)
==  Cyc_Absyn_Evar: 0){ goto _LL1169;} else{ goto _LL1170;} _LL1170: _LL1225:
_temp1224= _temp1166.f1; if( _temp1224 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1223;} else{ goto _LL1172;} _LL1223: _temp1222= _temp1166.f2; if( _temp1222
== ( void*) Cyc_Absyn_VoidType){ goto _LL1171;} else{ goto _LL1172;} _LL1172:
_LL1231: _temp1230= _temp1166.f1; if(( unsigned int) _temp1230 >  4u?*(( int*)
_temp1230) ==  Cyc_Absyn_VarType: 0){ _LL1233: _temp1232=(( struct Cyc_Absyn_VarType_struct*)
_temp1230)->f1; goto _LL1227;} else{ goto _LL1174;} _LL1227: _temp1226=
_temp1166.f2; if(( unsigned int) _temp1226 >  4u?*(( int*) _temp1226) ==  Cyc_Absyn_VarType:
0){ _LL1229: _temp1228=(( struct Cyc_Absyn_VarType_struct*) _temp1226)->f1; goto
_LL1173;} else{ goto _LL1174;} _LL1174: _LL1243: _temp1242= _temp1166.f1; if((
unsigned int) _temp1242 >  4u?*(( int*) _temp1242) ==  Cyc_Absyn_StructType: 0){
_LL1249: _temp1248=(( struct Cyc_Absyn_StructType_struct*) _temp1242)->f1; goto
_LL1247; _LL1247: _temp1246=(( struct Cyc_Absyn_StructType_struct*) _temp1242)->f2;
goto _LL1245; _LL1245: _temp1244=(( struct Cyc_Absyn_StructType_struct*)
_temp1242)->f3; goto _LL1235;} else{ goto _LL1176;} _LL1235: _temp1234=
_temp1166.f2; if(( unsigned int) _temp1234 >  4u?*(( int*) _temp1234) ==  Cyc_Absyn_StructType:
0){ _LL1241: _temp1240=(( struct Cyc_Absyn_StructType_struct*) _temp1234)->f1;
goto _LL1239; _LL1239: _temp1238=(( struct Cyc_Absyn_StructType_struct*)
_temp1234)->f2; goto _LL1237; _LL1237: _temp1236=(( struct Cyc_Absyn_StructType_struct*)
_temp1234)->f3; goto _LL1175;} else{ goto _LL1176;} _LL1176: _LL1255: _temp1254=
_temp1166.f1; if(( unsigned int) _temp1254 >  4u?*(( int*) _temp1254) ==  Cyc_Absyn_EnumType:
0){ _LL1257: _temp1256=(( struct Cyc_Absyn_EnumType_struct*) _temp1254)->f1;
goto _LL1251;} else{ goto _LL1178;} _LL1251: _temp1250= _temp1166.f2; if((
unsigned int) _temp1250 >  4u?*(( int*) _temp1250) ==  Cyc_Absyn_EnumType: 0){
_LL1253: _temp1252=(( struct Cyc_Absyn_EnumType_struct*) _temp1250)->f1; goto
_LL1177;} else{ goto _LL1178;} _LL1178: _LL1267: _temp1266= _temp1166.f1; if((
unsigned int) _temp1266 >  4u?*(( int*) _temp1266) ==  Cyc_Absyn_UnionType: 0){
_LL1273: _temp1272=(( struct Cyc_Absyn_UnionType_struct*) _temp1266)->f1; goto
_LL1271; _LL1271: _temp1270=(( struct Cyc_Absyn_UnionType_struct*) _temp1266)->f2;
goto _LL1269; _LL1269: _temp1268=(( struct Cyc_Absyn_UnionType_struct*)
_temp1266)->f3; goto _LL1259;} else{ goto _LL1180;} _LL1259: _temp1258=
_temp1166.f2; if(( unsigned int) _temp1258 >  4u?*(( int*) _temp1258) ==  Cyc_Absyn_UnionType:
0){ _LL1265: _temp1264=(( struct Cyc_Absyn_UnionType_struct*) _temp1258)->f1;
goto _LL1263; _LL1263: _temp1262=(( struct Cyc_Absyn_UnionType_struct*)
_temp1258)->f2; goto _LL1261; _LL1261: _temp1260=(( struct Cyc_Absyn_UnionType_struct*)
_temp1258)->f3; goto _LL1179;} else{ goto _LL1180;} _LL1180: _LL1288: _temp1287=
_temp1166.f1; if(( unsigned int) _temp1287 >  4u?*(( int*) _temp1287) ==  Cyc_Absyn_TunionType:
0){ _LL1290: _temp1289=(( struct Cyc_Absyn_TunionType_struct*) _temp1287)->f1;
_LL1296: _temp1295=( void*) _temp1289.tunion_info; if(*(( int*) _temp1295) == 
Cyc_Absyn_KnownTunion){ _LL1298: _temp1297=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1295)->f1; _temp1299=* _temp1297; goto _LL1294;} else{ goto _LL1182;}
_LL1294: _temp1293= _temp1289.targs; goto _LL1292; _LL1292: _temp1291=( void*)
_temp1289.rgn; goto _LL1275;} else{ goto _LL1182;} _LL1275: _temp1274= _temp1166.f2;
if(( unsigned int) _temp1274 >  4u?*(( int*) _temp1274) ==  Cyc_Absyn_TunionType:
0){ _LL1277: _temp1276=(( struct Cyc_Absyn_TunionType_struct*) _temp1274)->f1;
_LL1283: _temp1282=( void*) _temp1276.tunion_info; if(*(( int*) _temp1282) == 
Cyc_Absyn_KnownTunion){ _LL1285: _temp1284=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1282)->f1; _temp1286=* _temp1284; goto _LL1281;} else{ goto _LL1182;}
_LL1281: _temp1280= _temp1276.targs; goto _LL1279; _LL1279: _temp1278=( void*)
_temp1276.rgn; goto _LL1181;} else{ goto _LL1182;} _LL1182: _LL1313: _temp1312=
_temp1166.f1; if(( unsigned int) _temp1312 >  4u?*(( int*) _temp1312) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1315: _temp1314=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1312)->f1;
_LL1319: _temp1318=( void*) _temp1314.field_info; if(*(( int*) _temp1318) == 
Cyc_Absyn_KnownTunionfield){ _LL1323: _temp1322=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1318)->f1; goto _LL1321; _LL1321: _temp1320=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1318)->f2; goto _LL1317;} else{ goto _LL1184;} _LL1317: _temp1316=
_temp1314.targs; goto _LL1301;} else{ goto _LL1184;} _LL1301: _temp1300=
_temp1166.f2; if(( unsigned int) _temp1300 >  4u?*(( int*) _temp1300) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1303: _temp1302=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1300)->f1;
_LL1307: _temp1306=( void*) _temp1302.field_info; if(*(( int*) _temp1306) == 
Cyc_Absyn_KnownTunionfield){ _LL1311: _temp1310=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1306)->f1; goto _LL1309; _LL1309: _temp1308=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1306)->f2; goto _LL1305;} else{ goto _LL1184;} _LL1305: _temp1304=
_temp1302.targs; goto _LL1183;} else{ goto _LL1184;} _LL1184: _LL1339: _temp1338=
_temp1166.f1; if(( unsigned int) _temp1338 >  4u?*(( int*) _temp1338) ==  Cyc_Absyn_PointerType:
0){ _LL1341: _temp1340=(( struct Cyc_Absyn_PointerType_struct*) _temp1338)->f1;
_LL1351: _temp1350=( void*) _temp1340.elt_typ; goto _LL1349; _LL1349: _temp1348=(
void*) _temp1340.rgn_typ; goto _LL1347; _LL1347: _temp1346= _temp1340.nullable;
goto _LL1345; _LL1345: _temp1344= _temp1340.tq; goto _LL1343; _LL1343: _temp1342=
_temp1340.bounds; goto _LL1325;} else{ goto _LL1186;} _LL1325: _temp1324=
_temp1166.f2; if(( unsigned int) _temp1324 >  4u?*(( int*) _temp1324) ==  Cyc_Absyn_PointerType:
0){ _LL1327: _temp1326=(( struct Cyc_Absyn_PointerType_struct*) _temp1324)->f1;
_LL1337: _temp1336=( void*) _temp1326.elt_typ; goto _LL1335; _LL1335: _temp1334=(
void*) _temp1326.rgn_typ; goto _LL1333; _LL1333: _temp1332= _temp1326.nullable;
goto _LL1331; _LL1331: _temp1330= _temp1326.tq; goto _LL1329; _LL1329: _temp1328=
_temp1326.bounds; goto _LL1185;} else{ goto _LL1186;} _LL1186: _LL1359:
_temp1358= _temp1166.f1; if(( unsigned int) _temp1358 >  4u?*(( int*) _temp1358)
==  Cyc_Absyn_IntType: 0){ _LL1363: _temp1362=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1358)->f1; goto _LL1361; _LL1361: _temp1360=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1358)->f2; goto _LL1353;} else{ goto _LL1188;} _LL1353: _temp1352=
_temp1166.f2; if(( unsigned int) _temp1352 >  4u?*(( int*) _temp1352) ==  Cyc_Absyn_IntType:
0){ _LL1357: _temp1356=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1352)->f1;
goto _LL1355; _LL1355: _temp1354=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1352)->f2; goto _LL1187;} else{ goto _LL1188;} _LL1188: _LL1367: _temp1366=
_temp1166.f1; if( _temp1366 == ( void*) Cyc_Absyn_FloatType){ goto _LL1365;}
else{ goto _LL1190;} _LL1365: _temp1364= _temp1166.f2; if( _temp1364 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1189;} else{ goto _LL1190;} _LL1190: _LL1371:
_temp1370= _temp1166.f1; if( _temp1370 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1369;} else{ goto _LL1192;} _LL1369: _temp1368= _temp1166.f2; if( _temp1368
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1191;} else{ goto _LL1192;} _LL1192:
_LL1381: _temp1380= _temp1166.f1; if(( unsigned int) _temp1380 >  4u?*(( int*)
_temp1380) ==  Cyc_Absyn_ArrayType: 0){ _LL1387: _temp1386=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1380)->f1; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1380)->f2; goto _LL1383; _LL1383: _temp1382=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1380)->f3; goto _LL1373;} else{ goto _LL1194;} _LL1373: _temp1372=
_temp1166.f2; if(( unsigned int) _temp1372 >  4u?*(( int*) _temp1372) ==  Cyc_Absyn_ArrayType:
0){ _LL1379: _temp1378=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1372)->f1;
goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1372)->f2; goto _LL1375; _LL1375: _temp1374=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1372)->f3; goto _LL1193;} else{ goto _LL1194;} _LL1194: _LL1409: _temp1408=
_temp1166.f1; if(( unsigned int) _temp1408 >  4u?*(( int*) _temp1408) ==  Cyc_Absyn_FnType:
0){ _LL1411: _temp1410=(( struct Cyc_Absyn_FnType_struct*) _temp1408)->f1;
_LL1427: _temp1426= _temp1410.tvars; goto _LL1425; _LL1425: _temp1424= _temp1410.effect;
goto _LL1423; _LL1423: _temp1422=( void*) _temp1410.ret_typ; goto _LL1421;
_LL1421: _temp1420= _temp1410.args; goto _LL1419; _LL1419: _temp1418= _temp1410.c_varargs;
goto _LL1417; _LL1417: _temp1416= _temp1410.cyc_varargs; goto _LL1415; _LL1415:
_temp1414= _temp1410.rgn_po; goto _LL1413; _LL1413: _temp1412= _temp1410.attributes;
goto _LL1389;} else{ goto _LL1196;} _LL1389: _temp1388= _temp1166.f2; if((
unsigned int) _temp1388 >  4u?*(( int*) _temp1388) ==  Cyc_Absyn_FnType: 0){
_LL1391: _temp1390=(( struct Cyc_Absyn_FnType_struct*) _temp1388)->f1; _LL1407:
_temp1406= _temp1390.tvars; goto _LL1405; _LL1405: _temp1404= _temp1390.effect;
goto _LL1403; _LL1403: _temp1402=( void*) _temp1390.ret_typ; goto _LL1401;
_LL1401: _temp1400= _temp1390.args; goto _LL1399; _LL1399: _temp1398= _temp1390.c_varargs;
goto _LL1397; _LL1397: _temp1396= _temp1390.cyc_varargs; goto _LL1395; _LL1395:
_temp1394= _temp1390.rgn_po; goto _LL1393; _LL1393: _temp1392= _temp1390.attributes;
goto _LL1195;} else{ goto _LL1196;} _LL1196: _LL1433: _temp1432= _temp1166.f1;
if(( unsigned int) _temp1432 >  4u?*(( int*) _temp1432) ==  Cyc_Absyn_TupleType:
0){ _LL1435: _temp1434=(( struct Cyc_Absyn_TupleType_struct*) _temp1432)->f1;
goto _LL1429;} else{ goto _LL1198;} _LL1429: _temp1428= _temp1166.f2; if((
unsigned int) _temp1428 >  4u?*(( int*) _temp1428) ==  Cyc_Absyn_TupleType: 0){
_LL1431: _temp1430=(( struct Cyc_Absyn_TupleType_struct*) _temp1428)->f1; goto
_LL1197;} else{ goto _LL1198;} _LL1198: _LL1441: _temp1440= _temp1166.f1; if((
unsigned int) _temp1440 >  4u?*(( int*) _temp1440) ==  Cyc_Absyn_AnonStructType:
0){ _LL1443: _temp1442=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1440)->f1;
goto _LL1437;} else{ goto _LL1200;} _LL1437: _temp1436= _temp1166.f2; if((
unsigned int) _temp1436 >  4u?*(( int*) _temp1436) ==  Cyc_Absyn_AnonStructType:
0){ _LL1439: _temp1438=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1436)->f1;
goto _LL1199;} else{ goto _LL1200;} _LL1200: _LL1449: _temp1448= _temp1166.f1;
if(( unsigned int) _temp1448 >  4u?*(( int*) _temp1448) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1451: _temp1450=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1448)->f1;
goto _LL1445;} else{ goto _LL1202;} _LL1445: _temp1444= _temp1166.f2; if((
unsigned int) _temp1444 >  4u?*(( int*) _temp1444) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1447: _temp1446=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1444)->f1;
goto _LL1201;} else{ goto _LL1202;} _LL1202: _LL1455: _temp1454= _temp1166.f1;
if( _temp1454 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1453;} else{ goto _LL1204;}
_LL1453: _temp1452= _temp1166.f2; if( _temp1452 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1203;} else{ goto _LL1204;} _LL1204: _LL1461: _temp1460= _temp1166.f1;
if(( unsigned int) _temp1460 >  4u?*(( int*) _temp1460) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1463: _temp1462=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1460)->f1; goto _LL1457;} else{ goto _LL1206;} _LL1457: _temp1456=
_temp1166.f2; if(( unsigned int) _temp1456 >  4u?*(( int*) _temp1456) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1456)->f1; goto _LL1205;} else{ goto _LL1206;} _LL1206: _LL1465: _temp1464=
_temp1166.f1; if(( unsigned int) _temp1464 >  4u?*(( int*) _temp1464) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1207;} else{ goto _LL1208;} _LL1208: _LL1467: _temp1466= _temp1166.f2;
if(( unsigned int) _temp1466 >  4u?*(( int*) _temp1466) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1209;} else{ goto _LL1210;} _LL1210: _LL1469: _temp1468= _temp1166.f1;
if(( unsigned int) _temp1468 >  4u?*(( int*) _temp1468) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1211;} else{ goto _LL1212;} _LL1212: _LL1471: _temp1470= _temp1166.f1;
if(( unsigned int) _temp1470 >  4u?*(( int*) _temp1470) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1213;} else{ goto _LL1214;} _LL1214: _LL1473: _temp1472= _temp1166.f2;
if(( unsigned int) _temp1472 >  4u?*(( int*) _temp1472) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1215;} else{ goto _LL1216;} _LL1216: _LL1475: _temp1474= _temp1166.f2;
if(( unsigned int) _temp1474 >  4u?*(( int*) _temp1474) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1217;} else{ goto _LL1218;} _LL1218: goto _LL1219; _LL1169: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1171: return; _LL1173: { struct _tagged_arr* _temp1476=
_temp1232->name; struct _tagged_arr* _temp1477= _temp1228->name; int _temp1478=*((
int*) _check_null( _temp1232->identity)); int _temp1479=*(( int*) _check_null(
_temp1228->identity)); void* _temp1480= Cyc_Absyn_conref_val( _temp1232->kind);
void* _temp1481= Cyc_Absyn_conref_val( _temp1228->kind); if( _temp1479 == 
_temp1478? Cyc_Std_zstrptrcmp( _temp1476, _temp1477) ==  0: 0){ if( _temp1480 != 
_temp1481){({ struct Cyc_Std_String_pa_struct _temp1485; _temp1485.tag= Cyc_Std_String_pa;
_temp1485.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1481);{ struct
Cyc_Std_String_pa_struct _temp1484; _temp1484.tag= Cyc_Std_String_pa; _temp1484.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1480);{ struct Cyc_Std_String_pa_struct
_temp1483; _temp1483.tag= Cyc_Std_String_pa; _temp1483.f1=( struct _tagged_arr)*
_temp1476;{ void* _temp1482[ 3u]={& _temp1483,& _temp1484,& _temp1485};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1482, sizeof( void*), 3u));}}}});}
return;} Cyc_Tcutil_failure_reason= _tag_arr("(variable types are not the same)",
sizeof( unsigned char), 34u); goto _LL1167;} _LL1175: if((( _temp1240 !=  0?
_temp1248 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1240),( struct _tuple1*) _check_null( _temp1248)) ==  0: 0)? 1:( _temp1240
==  0? _temp1248 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1238, _temp1246);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different struct types)", sizeof(
unsigned char), 25u); goto _LL1167; _LL1177: if( Cyc_Absyn_qvar_cmp( _temp1256,
_temp1252) ==  0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different enum types)",
sizeof( unsigned char), 23u); goto _LL1167; _LL1179: if((( _temp1264 !=  0?
_temp1272 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1264),( struct _tuple1*) _check_null( _temp1272)) ==  0: 0)? 1:( _temp1264
==  0? _temp1272 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1262, _temp1270);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different union types)", sizeof(
unsigned char), 24u); goto _LL1167; _LL1181: if( _temp1299 ==  _temp1286? 1: Cyc_Absyn_qvar_cmp(
_temp1299->name, _temp1286->name) ==  0){ Cyc_Tcutil_unify_it( _temp1278,
_temp1291); Cyc_Tcutil_unify_list( _temp1280, _temp1293); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)", sizeof( unsigned char), 25u); goto _LL1167;
_LL1183: if(( _temp1322 ==  _temp1310? 1: Cyc_Absyn_qvar_cmp( _temp1322->name,
_temp1310->name) ==  0)? _temp1320 ==  _temp1308? 1: Cyc_Absyn_qvar_cmp(
_temp1320->name, _temp1308->name) ==  0: 0){ Cyc_Tcutil_unify_list( _temp1304,
_temp1316); return;} Cyc_Tcutil_failure_reason= _tag_arr("(different tunion field types)",
sizeof( unsigned char), 31u); goto _LL1167; _LL1185: Cyc_Tcutil_unify_it(
_temp1336, _temp1350); Cyc_Tcutil_unify_it( _temp1348, _temp1334); Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_unify_tqual( _temp1330, _temp1344);
Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp, _temp1328, _temp1342,
_tag_arr("(different pointer bounds)", sizeof( unsigned char), 27u));{ void*
_temp1486=( void*)( Cyc_Absyn_compress_conref( _temp1328))->v; void* _temp1492;
_LL1488: if(( unsigned int) _temp1486 >  1u?*(( int*) _temp1486) ==  Cyc_Absyn_Eq_constr:
0){ _LL1493: _temp1492=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1486)->f1;
if( _temp1492 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1489;} else{ goto
_LL1490;}} else{ goto _LL1490;} _LL1490: goto _LL1491; _LL1489: return; _LL1491:
goto _LL1487; _LL1487:;}(( void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y, struct _tagged_arr reason)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1332, _temp1346, _tag_arr("(different pointer types)",
sizeof( unsigned char), 26u)); return; _LL1187: if( _temp1356 ==  _temp1362?
_temp1354 ==  _temp1360: 0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different integral types)",
sizeof( unsigned char), 27u); goto _LL1167; _LL1189: return; _LL1191: return;
_LL1193: Cyc_Tcutil_unify_tqual( _temp1376, _temp1384); Cyc_Tcutil_unify_it(
_temp1378, _temp1386); if( _temp1382 ==  _temp1374){ return;} if( _temp1382 == 
0? 1: _temp1374 ==  0){ goto _LL1167;} if( Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1382)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1374))){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)", sizeof( unsigned char), 24u); goto _LL1167;
_LL1195: { int done= 0;{ struct _RegionHandle _temp1494= _new_region(); struct
_RegionHandle* rgn=& _temp1494; _push_region( rgn);{ struct Cyc_List_List* inst=
0; while( _temp1406 !=  0) { if( _temp1426 ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too few type variables)", sizeof(
unsigned char), 50u); break;} inst=({ struct Cyc_List_List* _temp1495=( struct
Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1495->hd=(
void*)({ struct _tuple5* _temp1496=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp1496->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1426))->hd; _temp1496->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1497=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1497[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1498; _temp1498.tag= Cyc_Absyn_VarType;
_temp1498.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1406))->hd; _temp1498;}); _temp1497;}); _temp1496;}); _temp1495->tl= inst;
_temp1495;}); _temp1406=(( struct Cyc_List_List*) _check_null( _temp1406))->tl;
_temp1426=(( struct Cyc_List_List*) _check_null( _temp1426))->tl;} if( _temp1426
!=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(second function type has too many type variables)",
sizeof( unsigned char), 51u); _npop_handler( 0u); goto _LL1167;} if( inst !=  0){
Cyc_Tcutil_unify_it(( void*)({ struct Cyc_Absyn_FnType_struct* _temp1499=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1499[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1500; _temp1500.tag= Cyc_Absyn_FnType;
_temp1500.f1=({ struct Cyc_Absyn_FnInfo _temp1501; _temp1501.tvars= 0; _temp1501.effect=
_temp1404; _temp1501.ret_typ=( void*) _temp1402; _temp1501.args= _temp1400;
_temp1501.c_varargs= _temp1398; _temp1501.cyc_varargs= _temp1396; _temp1501.rgn_po=
_temp1394; _temp1501.attributes= _temp1392; _temp1501;}); _temp1500;});
_temp1499;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1502=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1502[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1503; _temp1503.tag= Cyc_Absyn_FnType;
_temp1503.f1=({ struct Cyc_Absyn_FnInfo _temp1504; _temp1504.tvars= 0; _temp1504.effect=
_temp1424; _temp1504.ret_typ=( void*) _temp1422; _temp1504.args= _temp1420;
_temp1504.c_varargs= _temp1418; _temp1504.cyc_varargs= _temp1416; _temp1504.rgn_po=
_temp1414; _temp1504.attributes= _temp1412; _temp1504;}); _temp1503;});
_temp1502;}))); done= 1;}}; _pop_region( rgn);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp1402, _temp1422); for( 0; _temp1400 !=  0? _temp1420 !=  0: 0;( _temp1400=((
struct Cyc_List_List*) _check_null( _temp1400))->tl, _temp1420=(( struct Cyc_List_List*)
_check_null( _temp1420))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1400))->hd)).f2,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1420))->hd)).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1400))->hd)).f3,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1420))->hd)).f3);}
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; if( _temp1400 !=  0? 1:
_temp1420 !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different number of arguments)",
sizeof( unsigned char), 52u); goto _LL1167;} if( _temp1398 !=  _temp1418){ Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)", sizeof( unsigned char), 41u);
goto _LL1167;}{ int bad_cyc_vararg= 0;{ struct _tuple11 _temp1506=({ struct
_tuple11 _temp1505; _temp1505.f1= _temp1396; _temp1505.f2= _temp1416; _temp1505;});
struct Cyc_Absyn_VarargInfo* _temp1516; struct Cyc_Absyn_VarargInfo* _temp1518;
struct Cyc_Absyn_VarargInfo* _temp1520; struct Cyc_Absyn_VarargInfo* _temp1522;
struct Cyc_Absyn_VarargInfo* _temp1524; struct Cyc_Absyn_VarargInfo _temp1526;
int _temp1527; void* _temp1529; struct Cyc_Absyn_Tqual _temp1531; struct Cyc_Core_Opt*
_temp1533; struct Cyc_Absyn_VarargInfo* _temp1535; struct Cyc_Absyn_VarargInfo
_temp1537; int _temp1538; void* _temp1540; struct Cyc_Absyn_Tqual _temp1542;
struct Cyc_Core_Opt* _temp1544; _LL1508: _LL1519: _temp1518= _temp1506.f1; if(
_temp1518 ==  0){ goto _LL1517;} else{ goto _LL1510;} _LL1517: _temp1516=
_temp1506.f2; if( _temp1516 ==  0){ goto _LL1509;} else{ goto _LL1510;} _LL1510:
_LL1521: _temp1520= _temp1506.f1; if( _temp1520 ==  0){ goto _LL1511;} else{
goto _LL1512;} _LL1512: _LL1523: _temp1522= _temp1506.f2; if( _temp1522 ==  0){
goto _LL1513;} else{ goto _LL1514;} _LL1514: _LL1536: _temp1535= _temp1506.f1;
if( _temp1535 ==  0){ goto _LL1507;} else{ _temp1537=* _temp1535; _LL1545:
_temp1544= _temp1537.name; goto _LL1543; _LL1543: _temp1542= _temp1537.tq; goto
_LL1541; _LL1541: _temp1540=( void*) _temp1537.type; goto _LL1539; _LL1539:
_temp1538= _temp1537.inject; goto _LL1525;} _LL1525: _temp1524= _temp1506.f2;
if( _temp1524 ==  0){ goto _LL1507;} else{ _temp1526=* _temp1524; _LL1534:
_temp1533= _temp1526.name; goto _LL1532; _LL1532: _temp1531= _temp1526.tq; goto
_LL1530; _LL1530: _temp1529=( void*) _temp1526.type; goto _LL1528; _LL1528:
_temp1527= _temp1526.inject; goto _LL1515;} _LL1509: goto _LL1507; _LL1511: goto
_LL1513; _LL1513: bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type takes varargs)",
sizeof( unsigned char), 39u); goto _LL1507; _LL1515: Cyc_Tcutil_unify_tqual(
_temp1542, _temp1531); Cyc_Tcutil_unify_it( _temp1540, _temp1529); if( _temp1538
!=  _temp1527){ bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type injects varargs)",
sizeof( unsigned char), 41u);} goto _LL1507; _LL1507:;} if( bad_cyc_vararg){
goto _LL1167;}{ int bad_effect= 0;{ struct _tuple12 _temp1547=({ struct _tuple12
_temp1546; _temp1546.f1= _temp1404; _temp1546.f2= _temp1424; _temp1546;});
struct Cyc_Core_Opt* _temp1557; struct Cyc_Core_Opt* _temp1559; struct Cyc_Core_Opt*
_temp1561; struct Cyc_Core_Opt* _temp1563; _LL1549: _LL1560: _temp1559=
_temp1547.f1; if( _temp1559 ==  0){ goto _LL1558;} else{ goto _LL1551;} _LL1558:
_temp1557= _temp1547.f2; if( _temp1557 ==  0){ goto _LL1550;} else{ goto _LL1551;}
_LL1551: _LL1562: _temp1561= _temp1547.f1; if( _temp1561 ==  0){ goto _LL1552;}
else{ goto _LL1553;} _LL1553: _LL1564: _temp1563= _temp1547.f2; if( _temp1563 == 
0){ goto _LL1554;} else{ goto _LL1555;} _LL1555: goto _LL1556; _LL1550: goto
_LL1548; _LL1552: goto _LL1554; _LL1554: bad_effect= 1; goto _LL1548; _LL1556:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1404))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1424))->v); goto _LL1548; _LL1548:;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; if( bad_effect){ Cyc_Tcutil_failure_reason= _tag_arr("(function type effects do not unify)",
sizeof( unsigned char), 37u); goto _LL1167;} if( ! Cyc_Tcutil_same_atts(
_temp1412, _temp1392)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different attributes)",
sizeof( unsigned char), 43u); goto _LL1167;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1414, _temp1394)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different region lifetime orderings)",
sizeof( unsigned char), 58u); goto _LL1167;} return;}}} _LL1197: for( 0;
_temp1430 !=  0? _temp1434 !=  0: 0;( _temp1430=(( struct Cyc_List_List*)
_check_null( _temp1430))->tl, _temp1434=(( struct Cyc_List_List*) _check_null(
_temp1434))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1430))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1434))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1430))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1434))->hd)).f2);} if( _temp1430 ==  0?
_temp1434 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(tuple types have different numbers of components)",
sizeof( unsigned char), 51u); goto _LL1167; _LL1199: _temp1450= _temp1442;
_temp1446= _temp1438; goto _LL1201; _LL1201: for( 0; _temp1446 !=  0? _temp1450
!=  0: 0;( _temp1446=(( struct Cyc_List_List*) _check_null( _temp1446))->tl,
_temp1450=(( struct Cyc_List_List*) _check_null( _temp1450))->tl)){ struct Cyc_Absyn_Structfield*
_temp1565=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1446))->hd; struct Cyc_Absyn_Structfield* _temp1566=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1450))->hd; if( Cyc_Std_zstrptrcmp(
_temp1565->name, _temp1566->name) !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(different member names in anonymous structs/unions)",
sizeof( unsigned char), 53u);( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1565->tq, _temp1566->tq); Cyc_Tcutil_unify_it(( void*) _temp1565->type,(
void*) _temp1566->type); if( ! Cyc_Tcutil_same_atts( _temp1565->attributes,
_temp1566->attributes)){ Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2;
Cyc_Tcutil_failure_reason= _tag_arr("(different attributes on member)", sizeof(
unsigned char), 33u);( int) _throw(( void*) Cyc_Tcutil_Unify);} if((( _temp1565->width
!=  0? _temp1566->width ==  0: 0)? 1:( _temp1566->width !=  0? _temp1565->width
==  0: 0))? 1:(( _temp1565->width !=  0? _temp1566->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1565->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1566->width)): 0)){ Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason= _tag_arr("(different bitfield widths on member)",
sizeof( unsigned char), 38u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} if(
_temp1446 ==  0? _temp1450 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(different number of members)", sizeof(
unsigned char), 30u); goto _LL1167; _LL1203: return; _LL1205: Cyc_Tcutil_unify_it(
_temp1462, _temp1458); return; _LL1207: goto _LL1209; _LL1209: goto _LL1211;
_LL1211: goto _LL1213; _LL1213: goto _LL1215; _LL1215: goto _LL1217; _LL1217:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)", sizeof( unsigned char), 22u); goto _LL1167;
_LL1219: goto _LL1167; _LL1167:;}( int) _throw(( void*) Cyc_Tcutil_Unify);} int
Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1567= Cyc_Tcutil_compress( t);
_LL1569: if(( unsigned int) _temp1567 >  4u?*(( int*) _temp1567) ==  Cyc_Absyn_IntType:
0){ goto _LL1570;} else{ goto _LL1571;} _LL1571: if( _temp1567 == ( void*) Cyc_Absyn_FloatType){
goto _LL1572;} else{ goto _LL1573;} _LL1573: if( _temp1567 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1574;} else{ goto _LL1575;} _LL1575: if(( unsigned int) _temp1567 >  4u?*((
int*) _temp1567) ==  Cyc_Absyn_EnumType: 0){ goto _LL1576;} else{ goto _LL1577;}
_LL1577: goto _LL1578; _LL1570: goto _LL1572; _LL1572: goto _LL1574; _LL1574:
goto _LL1576; _LL1576: return 1; _LL1578: return 0; _LL1568:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1580=({ struct _tuple8 _temp1579; _temp1579.f1= t1;
_temp1579.f2= t2; _temp1579;}); void* _temp1602; void* _temp1604; void*
_temp1606; void* _temp1608; void* _temp1610; void* _temp1612; void* _temp1614;
void* _temp1616; void* _temp1618; void* _temp1620; void* _temp1622; void*
_temp1624; void* _temp1626; void* _temp1628; void* _temp1630; void* _temp1632;
void* _temp1634; void* _temp1636; void* _temp1638; void* _temp1640; void*
_temp1642; void* _temp1644; void* _temp1646; void* _temp1648; void* _temp1650;
void* _temp1652; void* _temp1654; _LL1582: _LL1605: _temp1604= _temp1580.f1; if(
_temp1604 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1603;} else{ goto _LL1584;}
_LL1603: _temp1602= _temp1580.f2; if( _temp1602 == ( void*) Cyc_Absyn_FloatType){
goto _LL1583;} else{ goto _LL1584;} _LL1584: _LL1609: _temp1608= _temp1580.f1;
if( _temp1608 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1607;} else{ goto
_LL1586;} _LL1607: _temp1606= _temp1580.f2; if(( unsigned int) _temp1606 >  4u?*((
int*) _temp1606) ==  Cyc_Absyn_IntType: 0){ goto _LL1585;} else{ goto _LL1586;}
_LL1586: _LL1613: _temp1612= _temp1580.f1; if( _temp1612 == ( void*) Cyc_Absyn_FloatType){
goto _LL1611;} else{ goto _LL1588;} _LL1611: _temp1610= _temp1580.f2; if((
unsigned int) _temp1610 >  4u?*(( int*) _temp1610) ==  Cyc_Absyn_IntType: 0){
goto _LL1587;} else{ goto _LL1588;} _LL1588: _LL1619: _temp1618= _temp1580.f1;
if(( unsigned int) _temp1618 >  4u?*(( int*) _temp1618) ==  Cyc_Absyn_IntType: 0){
_LL1621: _temp1620=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1618)->f2;
if( _temp1620 == ( void*) Cyc_Absyn_B8){ goto _LL1615;} else{ goto _LL1590;}}
else{ goto _LL1590;} _LL1615: _temp1614= _temp1580.f2; if(( unsigned int)
_temp1614 >  4u?*(( int*) _temp1614) ==  Cyc_Absyn_IntType: 0){ _LL1617:
_temp1616=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1614)->f2; if(
_temp1616 == ( void*) Cyc_Absyn_B8){ goto _LL1589;} else{ goto _LL1590;}} else{
goto _LL1590;} _LL1590: _LL1623: _temp1622= _temp1580.f1; if(( unsigned int)
_temp1622 >  4u?*(( int*) _temp1622) ==  Cyc_Absyn_IntType: 0){ _LL1625:
_temp1624=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1622)->f2; if(
_temp1624 == ( void*) Cyc_Absyn_B8){ goto _LL1591;} else{ goto _LL1592;}} else{
goto _LL1592;} _LL1592: _LL1629: _temp1628= _temp1580.f1; if(( unsigned int)
_temp1628 >  4u?*(( int*) _temp1628) ==  Cyc_Absyn_IntType: 0){ _LL1631:
_temp1630=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1628)->f2; if(
_temp1630 == ( void*) Cyc_Absyn_B4){ goto _LL1627;} else{ goto _LL1594;}} else{
goto _LL1594;} _LL1627: _temp1626= _temp1580.f2; if( _temp1626 == ( void*) Cyc_Absyn_FloatType){
goto _LL1593;} else{ goto _LL1594;} _LL1594: _LL1637: _temp1636= _temp1580.f1;
if(( unsigned int) _temp1636 >  4u?*(( int*) _temp1636) ==  Cyc_Absyn_IntType: 0){
_LL1639: _temp1638=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1636)->f2;
if( _temp1638 == ( void*) Cyc_Absyn_B4){ goto _LL1633;} else{ goto _LL1596;}}
else{ goto _LL1596;} _LL1633: _temp1632= _temp1580.f2; if(( unsigned int)
_temp1632 >  4u?*(( int*) _temp1632) ==  Cyc_Absyn_IntType: 0){ _LL1635:
_temp1634=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1632)->f2; if(
_temp1634 == ( void*) Cyc_Absyn_B2){ goto _LL1595;} else{ goto _LL1596;}} else{
goto _LL1596;} _LL1596: _LL1645: _temp1644= _temp1580.f1; if(( unsigned int)
_temp1644 >  4u?*(( int*) _temp1644) ==  Cyc_Absyn_IntType: 0){ _LL1647:
_temp1646=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1644)->f2; if(
_temp1646 == ( void*) Cyc_Absyn_B4){ goto _LL1641;} else{ goto _LL1598;}} else{
goto _LL1598;} _LL1641: _temp1640= _temp1580.f2; if(( unsigned int) _temp1640 > 
4u?*(( int*) _temp1640) ==  Cyc_Absyn_IntType: 0){ _LL1643: _temp1642=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1640)->f2; if( _temp1642 == ( void*) Cyc_Absyn_B1){
goto _LL1597;} else{ goto _LL1598;}} else{ goto _LL1598;} _LL1598: _LL1653:
_temp1652= _temp1580.f1; if(( unsigned int) _temp1652 >  4u?*(( int*) _temp1652)
==  Cyc_Absyn_IntType: 0){ _LL1655: _temp1654=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1652)->f2; if( _temp1654 == ( void*) Cyc_Absyn_B2){ goto _LL1649;} else{
goto _LL1600;}} else{ goto _LL1600;} _LL1649: _temp1648= _temp1580.f2; if((
unsigned int) _temp1648 >  4u?*(( int*) _temp1648) ==  Cyc_Absyn_IntType: 0){
_LL1651: _temp1650=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1648)->f2;
if( _temp1650 == ( void*) Cyc_Absyn_B1){ goto _LL1599;} else{ goto _LL1600;}}
else{ goto _LL1600;} _LL1600: goto _LL1601; _LL1583: goto _LL1585; _LL1585: goto
_LL1587; _LL1587: return 1; _LL1589: return 0; _LL1591: goto _LL1593; _LL1593:
goto _LL1595; _LL1595: goto _LL1597; _LL1597: goto _LL1599; _LL1599: return 1;
_LL1601: return 0; _LL1581:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1656=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1656->v=( void*)
t1; _temp1656;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify( t,(
void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp1659; _temp1659.tag= Cyc_Std_String_pa; _temp1659.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1658; _temp1658.tag= Cyc_Std_String_pa;
_temp1658.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1657[
2u]={& _temp1658,& _temp1659}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1657, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1660= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1662: if(( unsigned int) _temp1660 >  4u?*(( int*) _temp1660) ==  Cyc_Absyn_PointerType:
0){ goto _LL1663;} else{ goto _LL1664;} _LL1664: goto _LL1665; _LL1663: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1661; _LL1665: return 0; _LL1661:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1666= Cyc_Tcutil_compress(
t); _LL1668: if(( unsigned int) _temp1666 >  4u?*(( int*) _temp1666) ==  Cyc_Absyn_IntType:
0){ goto _LL1669;} else{ goto _LL1670;} _LL1670: if(( unsigned int) _temp1666 > 
4u?*(( int*) _temp1666) ==  Cyc_Absyn_EnumType: 0){ goto _LL1671;} else{ goto
_LL1672;} _LL1672: goto _LL1673; _LL1669: goto _LL1671; _LL1671: return 1;
_LL1673: return 0; _LL1667:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1674[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1674, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1675[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1675, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1677=({ struct _tuple8 _temp1676; _temp1676.f1= t1; _temp1676.f2= t2;
_temp1676;}); void* _temp1689; struct Cyc_Absyn_PtrInfo _temp1691; void*
_temp1693; struct Cyc_Absyn_PtrInfo _temp1695; void* _temp1697; struct Cyc_Absyn_Exp*
_temp1699; struct Cyc_Absyn_Tqual _temp1701; void* _temp1703; void* _temp1705;
struct Cyc_Absyn_Exp* _temp1707; struct Cyc_Absyn_Tqual _temp1709; void*
_temp1711; void* _temp1713; struct Cyc_Absyn_TunionInfo _temp1715; struct Cyc_List_List*
_temp1717; void* _temp1719; struct Cyc_Absyn_Tuniondecl** _temp1721; struct Cyc_Absyn_Tuniondecl*
_temp1723; void* _temp1724; struct Cyc_Absyn_TunionFieldInfo _temp1726; struct
Cyc_List_List* _temp1728; void* _temp1730; struct Cyc_Absyn_Tunionfield*
_temp1732; struct Cyc_Absyn_Tuniondecl* _temp1734; void* _temp1736; struct Cyc_Absyn_TunionInfo
_temp1738; void* _temp1740; struct Cyc_List_List* _temp1742; void* _temp1744;
struct Cyc_Absyn_Tuniondecl** _temp1746; struct Cyc_Absyn_Tuniondecl* _temp1748;
void* _temp1749; struct Cyc_Absyn_PtrInfo _temp1751; struct Cyc_Absyn_Conref*
_temp1753; struct Cyc_Absyn_Tqual _temp1755; struct Cyc_Absyn_Conref* _temp1757;
void* _temp1759; void* _temp1761; _LL1679: _LL1694: _temp1693= _temp1677.f1; if((
unsigned int) _temp1693 >  4u?*(( int*) _temp1693) ==  Cyc_Absyn_PointerType: 0){
_LL1696: _temp1695=(( struct Cyc_Absyn_PointerType_struct*) _temp1693)->f1; goto
_LL1690;} else{ goto _LL1681;} _LL1690: _temp1689= _temp1677.f2; if((
unsigned int) _temp1689 >  4u?*(( int*) _temp1689) ==  Cyc_Absyn_PointerType: 0){
_LL1692: _temp1691=(( struct Cyc_Absyn_PointerType_struct*) _temp1689)->f1; goto
_LL1680;} else{ goto _LL1681;} _LL1681: _LL1706: _temp1705= _temp1677.f1; if((
unsigned int) _temp1705 >  4u?*(( int*) _temp1705) ==  Cyc_Absyn_ArrayType: 0){
_LL1712: _temp1711=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1705)->f1;
goto _LL1710; _LL1710: _temp1709=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1705)->f2; goto _LL1708; _LL1708: _temp1707=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1705)->f3; goto _LL1698;} else{ goto _LL1683;} _LL1698: _temp1697=
_temp1677.f2; if(( unsigned int) _temp1697 >  4u?*(( int*) _temp1697) ==  Cyc_Absyn_ArrayType:
0){ _LL1704: _temp1703=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1697)->f1;
goto _LL1702; _LL1702: _temp1701=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1697)->f2; goto _LL1700; _LL1700: _temp1699=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1697)->f3; goto _LL1682;} else{ goto _LL1683;} _LL1683: _LL1725: _temp1724=
_temp1677.f1; if(( unsigned int) _temp1724 >  4u?*(( int*) _temp1724) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1727: _temp1726=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1724)->f1;
_LL1731: _temp1730=( void*) _temp1726.field_info; if(*(( int*) _temp1730) == 
Cyc_Absyn_KnownTunionfield){ _LL1735: _temp1734=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1730)->f1; goto _LL1733; _LL1733: _temp1732=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1730)->f2; goto _LL1729;} else{ goto _LL1685;} _LL1729: _temp1728=
_temp1726.targs; goto _LL1714;} else{ goto _LL1685;} _LL1714: _temp1713=
_temp1677.f2; if(( unsigned int) _temp1713 >  4u?*(( int*) _temp1713) ==  Cyc_Absyn_TunionType:
0){ _LL1716: _temp1715=(( struct Cyc_Absyn_TunionType_struct*) _temp1713)->f1;
_LL1720: _temp1719=( void*) _temp1715.tunion_info; if(*(( int*) _temp1719) == 
Cyc_Absyn_KnownTunion){ _LL1722: _temp1721=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1719)->f1; _temp1723=* _temp1721; goto _LL1718;} else{ goto _LL1685;}
_LL1718: _temp1717= _temp1715.targs; goto _LL1684;} else{ goto _LL1685;} _LL1685:
_LL1750: _temp1749= _temp1677.f1; if(( unsigned int) _temp1749 >  4u?*(( int*)
_temp1749) ==  Cyc_Absyn_PointerType: 0){ _LL1752: _temp1751=(( struct Cyc_Absyn_PointerType_struct*)
_temp1749)->f1; _LL1762: _temp1761=( void*) _temp1751.elt_typ; goto _LL1760;
_LL1760: _temp1759=( void*) _temp1751.rgn_typ; goto _LL1758; _LL1758: _temp1757=
_temp1751.nullable; goto _LL1756; _LL1756: _temp1755= _temp1751.tq; goto _LL1754;
_LL1754: _temp1753= _temp1751.bounds; goto _LL1737;} else{ goto _LL1687;}
_LL1737: _temp1736= _temp1677.f2; if(( unsigned int) _temp1736 >  4u?*(( int*)
_temp1736) ==  Cyc_Absyn_TunionType: 0){ _LL1739: _temp1738=(( struct Cyc_Absyn_TunionType_struct*)
_temp1736)->f1; _LL1745: _temp1744=( void*) _temp1738.tunion_info; if(*(( int*)
_temp1744) ==  Cyc_Absyn_KnownTunion){ _LL1747: _temp1746=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1744)->f1; _temp1748=* _temp1746; goto _LL1743;} else{ goto _LL1687;}
_LL1743: _temp1742= _temp1738.targs; goto _LL1741; _LL1741: _temp1740=( void*)
_temp1738.rgn; goto _LL1686;} else{ goto _LL1687;} _LL1687: goto _LL1688;
_LL1680: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1695.nullable, _temp1691.nullable)){ void* _temp1763=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1695.nullable))->v;
int _temp1769; _LL1765: if(( unsigned int) _temp1763 >  1u?*(( int*) _temp1763)
==  Cyc_Absyn_Eq_constr: 0){ _LL1770: _temp1769=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1763)->f1; goto _LL1766;} else{ goto _LL1767;} _LL1767: goto _LL1768;
_LL1766: okay= ! _temp1769; goto _LL1764; _LL1768:( int) _throw(({ void*
_temp1771[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp1771, sizeof( void*), 0u));}));
_LL1764:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1695.bounds,
_temp1691.bounds)){ struct _tuple8 _temp1773=({ struct _tuple8 _temp1772;
_temp1772.f1=( void*)( Cyc_Absyn_compress_conref( _temp1695.bounds))->v;
_temp1772.f2=( void*)( Cyc_Absyn_compress_conref( _temp1691.bounds))->v;
_temp1772;}); void* _temp1783; void* _temp1785; void* _temp1787; void* _temp1789;
void* _temp1791; void* _temp1793; struct Cyc_Absyn_Exp* _temp1795; void*
_temp1797; void* _temp1799; struct Cyc_Absyn_Exp* _temp1801; void* _temp1803;
void* _temp1805; void* _temp1807; void* _temp1809; _LL1775: _LL1788: _temp1787=
_temp1773.f1; if(( unsigned int) _temp1787 >  1u?*(( int*) _temp1787) ==  Cyc_Absyn_Eq_constr:
0){ _LL1790: _temp1789=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1787)->f1;
if(( unsigned int) _temp1789 >  1u?*(( int*) _temp1789) ==  Cyc_Absyn_Upper_b: 0){
goto _LL1784;} else{ goto _LL1777;}} else{ goto _LL1777;} _LL1784: _temp1783=
_temp1773.f2; if(( unsigned int) _temp1783 >  1u?*(( int*) _temp1783) ==  Cyc_Absyn_Eq_constr:
0){ _LL1786: _temp1785=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1783)->f1;
if( _temp1785 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1776;} else{ goto
_LL1777;}} else{ goto _LL1777;} _LL1777: _LL1798: _temp1797= _temp1773.f1; if((
unsigned int) _temp1797 >  1u?*(( int*) _temp1797) ==  Cyc_Absyn_Eq_constr: 0){
_LL1800: _temp1799=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1797)->f1;
if(( unsigned int) _temp1799 >  1u?*(( int*) _temp1799) ==  Cyc_Absyn_Upper_b: 0){
_LL1802: _temp1801=(( struct Cyc_Absyn_Upper_b_struct*) _temp1799)->f1; goto
_LL1792;} else{ goto _LL1779;}} else{ goto _LL1779;} _LL1792: _temp1791=
_temp1773.f2; if(( unsigned int) _temp1791 >  1u?*(( int*) _temp1791) ==  Cyc_Absyn_Eq_constr:
0){ _LL1794: _temp1793=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1791)->f1;
if(( unsigned int) _temp1793 >  1u?*(( int*) _temp1793) ==  Cyc_Absyn_Upper_b: 0){
_LL1796: _temp1795=(( struct Cyc_Absyn_Upper_b_struct*) _temp1793)->f1; goto
_LL1778;} else{ goto _LL1779;}} else{ goto _LL1779;} _LL1779: _LL1808: _temp1807=
_temp1773.f1; if(( unsigned int) _temp1807 >  1u?*(( int*) _temp1807) ==  Cyc_Absyn_Eq_constr:
0){ _LL1810: _temp1809=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1807)->f1;
if( _temp1809 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1804;} else{ goto
_LL1781;}} else{ goto _LL1781;} _LL1804: _temp1803= _temp1773.f2; if((
unsigned int) _temp1803 >  1u?*(( int*) _temp1803) ==  Cyc_Absyn_Eq_constr: 0){
_LL1806: _temp1805=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1803)->f1;
if( _temp1805 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1780;} else{ goto
_LL1781;}} else{ goto _LL1781;} _LL1781: goto _LL1782; _LL1776: okay= 1; goto
_LL1774; _LL1778: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1801) >=  Cyc_Evexp_eval_const_uint_exp(
_temp1795): 0;({ void* _temp1811[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1811, sizeof( void*), 0u));}); goto
_LL1774; _LL1780: okay= 1; goto _LL1774; _LL1782: okay= 0; goto _LL1774; _LL1774:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1695.elt_typ,( void*) _temp1691.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1695.rgn_typ,( void*) _temp1691.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1695.rgn_typ,( void*) _temp1691.rgn_typ):
0; okay= okay? !( _temp1695.tq).q_const? 1:( _temp1691.tq).q_const: 0; return
okay;} _LL1682: { int okay; okay=( _temp1707 !=  0? _temp1699 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1707)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1699)): 0; return( okay? Cyc_Tcutil_unify(
_temp1711, _temp1703): 0)? ! _temp1709.q_const? 1: _temp1701.q_const: 0;}
_LL1684: if(( _temp1734 ==  _temp1723? 1: Cyc_Absyn_qvar_cmp( _temp1734->name,
_temp1723->name) ==  0)? _temp1732->typs ==  0: 0){ for( 0; _temp1728 !=  0?
_temp1717 !=  0: 0;( _temp1728=(( struct Cyc_List_List*) _check_null( _temp1728))->tl,
_temp1717=(( struct Cyc_List_List*) _check_null( _temp1717))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1728))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1717))->hd)){ break;}} if( _temp1728 ==  0?
_temp1717 ==  0: 0){ return 1;}} return 0; _LL1686:{ void* _temp1812= Cyc_Tcutil_compress(
_temp1761); struct Cyc_Absyn_TunionFieldInfo _temp1818; struct Cyc_List_List*
_temp1820; void* _temp1822; struct Cyc_Absyn_Tunionfield* _temp1824; struct Cyc_Absyn_Tuniondecl*
_temp1826; _LL1814: if(( unsigned int) _temp1812 >  4u?*(( int*) _temp1812) == 
Cyc_Absyn_TunionFieldType: 0){ _LL1819: _temp1818=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1812)->f1; _LL1823: _temp1822=( void*) _temp1818.field_info; if(*(( int*)
_temp1822) ==  Cyc_Absyn_KnownTunionfield){ _LL1827: _temp1826=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1822)->f1; goto _LL1825; _LL1825: _temp1824=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1822)->f2; goto _LL1821;} else{ goto _LL1816;} _LL1821: _temp1820=
_temp1818.targs; goto _LL1815;} else{ goto _LL1816;} _LL1816: goto _LL1817;
_LL1815: if( ! Cyc_Tcutil_unify( _temp1759, _temp1740)? ! Cyc_Tcenv_region_outlives(
te, _temp1759, _temp1740): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1757,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1753,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1748->name, _temp1826->name) ==  0? _temp1824->typs !=  0: 0){ int okay= 1;
for( 0; _temp1820 !=  0? _temp1742 !=  0: 0;( _temp1820=(( struct Cyc_List_List*)
_check_null( _temp1820))->tl, _temp1742=(( struct Cyc_List_List*) _check_null(
_temp1742))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1820))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1742))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1820 !=  0)? 1:
_temp1742 !=  0){ return 0;} return 1;} goto _LL1813; _LL1817: goto _LL1813;
_LL1813:;} return 0; _LL1688: return Cyc_Tcutil_unify( t1, t2); _LL1678:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1828[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1828, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Std_String_pa_struct
_temp1831; _temp1831.tag= Cyc_Std_String_pa; _temp1831.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct _temp1830;
_temp1830.tag= Cyc_Std_String_pa; _temp1830.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1829[ 2u]={& _temp1830,& _temp1831}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1829, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} int Cyc_Tcutil_coerceable( void* t){ void* _temp1832= Cyc_Tcutil_compress(
t); _LL1834: if(( unsigned int) _temp1832 >  4u?*(( int*) _temp1832) ==  Cyc_Absyn_IntType:
0){ goto _LL1835;} else{ goto _LL1836;} _LL1836: if( _temp1832 == ( void*) Cyc_Absyn_FloatType){
goto _LL1837;} else{ goto _LL1838;} _LL1838: if( _temp1832 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1839;} else{ goto _LL1840;} _LL1840: goto _LL1841; _LL1835: goto _LL1837;
_LL1837: goto _LL1839; _LL1839: return 1; _LL1841: return 0; _LL1833:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1842=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp1842->f1= x->tq; _temp1842->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1842;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1843= t1; struct Cyc_List_List* _temp1855; struct Cyc_Absyn_Structdecl**
_temp1857; struct Cyc_List_List* _temp1859; struct _tuple1* _temp1861; struct
Cyc_List_List* _temp1863; _LL1845: if( _temp1843 == ( void*) Cyc_Absyn_VoidType){
goto _LL1846;} else{ goto _LL1847;} _LL1847: if(( unsigned int) _temp1843 >  4u?*((
int*) _temp1843) ==  Cyc_Absyn_TupleType: 0){ _LL1856: _temp1855=(( struct Cyc_Absyn_TupleType_struct*)
_temp1843)->f1; goto _LL1848;} else{ goto _LL1849;} _LL1849: if(( unsigned int)
_temp1843 >  4u?*(( int*) _temp1843) ==  Cyc_Absyn_StructType: 0){ _LL1862:
_temp1861=(( struct Cyc_Absyn_StructType_struct*) _temp1843)->f1; goto _LL1860;
_LL1860: _temp1859=(( struct Cyc_Absyn_StructType_struct*) _temp1843)->f2; goto
_LL1858; _LL1858: _temp1857=(( struct Cyc_Absyn_StructType_struct*) _temp1843)->f3;
goto _LL1850;} else{ goto _LL1851;} _LL1851: if(( unsigned int) _temp1843 >  4u?*((
int*) _temp1843) ==  Cyc_Absyn_AnonStructType: 0){ _LL1864: _temp1863=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1843)->f1; goto _LL1852;} else{ goto
_LL1853;} _LL1853: goto _LL1854; _LL1846: return 0; _LL1848: return _temp1855;
_LL1850: if( _temp1857 ==  0? 1: _temp1861 ==  0){ return({ struct Cyc_List_List*
_temp1865=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1865->hd=( void*)({ struct _tuple4* _temp1866=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1866->f1= Cyc_Absyn_empty_tqual(); _temp1866->f2=
t1; _temp1866;}); _temp1865->tl= 0; _temp1865;});}{ struct Cyc_Absyn_Structdecl*
_temp1867=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1857)); struct
_tuple1 _temp1868=*(( struct _tuple1*) _check_null( _temp1861)); struct Cyc_List_List*
_temp1869=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1867->tvs, _temp1859); if( _temp1867->fields ==  0){
return({ struct Cyc_List_List* _temp1870=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1870->hd=( void*)({ struct _tuple4*
_temp1871=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1871->f1=
Cyc_Absyn_empty_tqual(); _temp1871->f2= t1; _temp1871;}); _temp1870->tl= 0;
_temp1870;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1869,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1867->fields))->v);}
_LL1852: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1863); _LL1854: return({
struct Cyc_List_List* _temp1872=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1872->hd=( void*)({ struct _tuple4* _temp1873=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1873->f1= Cyc_Absyn_empty_tqual();
_temp1873->f2= t1; _temp1873;}); _temp1872->tl= 0; _temp1872;}); _LL1844:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1875=({ struct _tuple8 _temp1874; _temp1874.f1= t1; _temp1874.f2=
t2; _temp1874;}); void* _temp1881; struct Cyc_Absyn_PtrInfo _temp1883; struct
Cyc_Absyn_Conref* _temp1885; struct Cyc_Absyn_Tqual _temp1887; struct Cyc_Absyn_Conref*
_temp1889; void* _temp1891; void* _temp1893; void* _temp1895; struct Cyc_Absyn_PtrInfo
_temp1897; struct Cyc_Absyn_Conref* _temp1899; struct Cyc_Absyn_Tqual _temp1901;
struct Cyc_Absyn_Conref* _temp1903; void* _temp1905; void* _temp1907; _LL1877:
_LL1896: _temp1895= _temp1875.f1; if(( unsigned int) _temp1895 >  4u?*(( int*)
_temp1895) ==  Cyc_Absyn_PointerType: 0){ _LL1898: _temp1897=(( struct Cyc_Absyn_PointerType_struct*)
_temp1895)->f1; _LL1908: _temp1907=( void*) _temp1897.elt_typ; goto _LL1906;
_LL1906: _temp1905=( void*) _temp1897.rgn_typ; goto _LL1904; _LL1904: _temp1903=
_temp1897.nullable; goto _LL1902; _LL1902: _temp1901= _temp1897.tq; goto _LL1900;
_LL1900: _temp1899= _temp1897.bounds; goto _LL1882;} else{ goto _LL1879;}
_LL1882: _temp1881= _temp1875.f2; if(( unsigned int) _temp1881 >  4u?*(( int*)
_temp1881) ==  Cyc_Absyn_PointerType: 0){ _LL1884: _temp1883=(( struct Cyc_Absyn_PointerType_struct*)
_temp1881)->f1; _LL1894: _temp1893=( void*) _temp1883.elt_typ; goto _LL1892;
_LL1892: _temp1891=( void*) _temp1883.rgn_typ; goto _LL1890; _LL1890: _temp1889=
_temp1883.nullable; goto _LL1888; _LL1888: _temp1887= _temp1883.tq; goto _LL1886;
_LL1886: _temp1885= _temp1883.bounds; goto _LL1878;} else{ goto _LL1879;}
_LL1879: goto _LL1880; _LL1878: if( _temp1901.q_const? ! _temp1887.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1903, _temp1889)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1903): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1889): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1905, _temp1891)? ! Cyc_Tcenv_region_outlives(
te, _temp1905, _temp1891): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1899, _temp1885)){ struct _tuple8 _temp1910=({ struct _tuple8 _temp1909;
_temp1909.f1= Cyc_Absyn_conref_val( _temp1899); _temp1909.f2= Cyc_Absyn_conref_val(
_temp1885); _temp1909;}); void* _temp1918; void* _temp1920; void* _temp1922;
struct Cyc_Absyn_Exp* _temp1924; void* _temp1926; struct Cyc_Absyn_Exp*
_temp1928; _LL1912: _LL1921: _temp1920= _temp1910.f1; if(( unsigned int)
_temp1920 >  1u?*(( int*) _temp1920) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1919;}
else{ goto _LL1914;} _LL1919: _temp1918= _temp1910.f2; if( _temp1918 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1913;} else{ goto _LL1914;} _LL1914: _LL1927:
_temp1926= _temp1910.f1; if(( unsigned int) _temp1926 >  1u?*(( int*) _temp1926)
==  Cyc_Absyn_Upper_b: 0){ _LL1929: _temp1928=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1926)->f1; goto _LL1923;} else{ goto _LL1916;} _LL1923: _temp1922=
_temp1910.f2; if(( unsigned int) _temp1922 >  1u?*(( int*) _temp1922) ==  Cyc_Absyn_Upper_b:
0){ _LL1925: _temp1924=(( struct Cyc_Absyn_Upper_b_struct*) _temp1922)->f1; goto
_LL1915;} else{ goto _LL1916;} _LL1916: goto _LL1917; _LL1913: goto _LL1911;
_LL1915: if( Cyc_Evexp_eval_const_uint_exp( _temp1928) <  Cyc_Evexp_eval_const_uint_exp(
_temp1924)){ return 0;} goto _LL1911; _LL1917: return 0; _LL1911:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1930=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1930->hd=( void*)({ struct _tuple8*
_temp1931=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1931->f1=
t1; _temp1931->f2= t2; _temp1931;}); _temp1930->tl= assume; _temp1930;}),
_temp1907, _temp1893); _LL1880: return 0; _LL1876:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1933=({ struct _tuple8 _temp1932;
_temp1932.f1= Cyc_Tcutil_compress( t1); _temp1932.f2= Cyc_Tcutil_compress( t2);
_temp1932;}); void* _temp1939; void* _temp1941; void* _temp1943; void* _temp1945;
_LL1935: _LL1944: _temp1943= _temp1933.f1; if(( unsigned int) _temp1943 >  4u?*((
int*) _temp1943) ==  Cyc_Absyn_IntType: 0){ _LL1946: _temp1945=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1943)->f2; goto _LL1940;} else{ goto _LL1937;}
_LL1940: _temp1939= _temp1933.f2; if(( unsigned int) _temp1939 >  4u?*(( int*)
_temp1939) ==  Cyc_Absyn_IntType: 0){ _LL1942: _temp1941=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1939)->f2; goto _LL1936;} else{ goto _LL1937;} _LL1937: goto _LL1938;
_LL1936: return _temp1945 ==  _temp1941; _LL1938: return 0; _LL1934:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=((
struct Cyc_List_List*) _check_null( tqs1))->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1949; void* _temp1950; struct Cyc_Absyn_Tqual _temp1952;
struct _tuple4* _temp1947=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs1))->hd; _temp1949=* _temp1947; _LL1953: _temp1952= _temp1949.f1;
goto _LL1951; _LL1951: _temp1950= _temp1949.f2; goto _LL1948; _LL1948: { struct
_tuple4 _temp1956; void* _temp1957; struct Cyc_Absyn_Tqual _temp1959; struct
_tuple4* _temp1954=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs2))->hd;
_temp1956=* _temp1954; _LL1960: _temp1959= _temp1956.f1; goto _LL1958; _LL1958:
_temp1957= _temp1956.f2; goto _LL1955; _LL1955: if( _temp1959.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1950, _temp1957): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1950, _temp1957)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1950,
_temp1957)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1961= t2; struct Cyc_Absyn_Uniondecl** _temp1969; struct Cyc_List_List*
_temp1971; void* _temp1973; _LL1963: if(( unsigned int) _temp1961 >  4u?*(( int*)
_temp1961) ==  Cyc_Absyn_UnionType: 0){ _LL1972: _temp1971=(( struct Cyc_Absyn_UnionType_struct*)
_temp1961)->f2; goto _LL1970; _LL1970: _temp1969=(( struct Cyc_Absyn_UnionType_struct*)
_temp1961)->f3; goto _LL1964;} else{ goto _LL1965;} _LL1965: if(( unsigned int)
_temp1961 >  4u?*(( int*) _temp1961) ==  Cyc_Absyn_IntType: 0){ _LL1974:
_temp1973=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1961)->f2; if(
_temp1973 == ( void*) Cyc_Absyn_B4){ goto _LL1966;} else{ goto _LL1967;}} else{
goto _LL1967;} _LL1967: goto _LL1968; _LL1964: if( _temp1969 ==  0){ goto
_LL1962;}{ struct Cyc_Absyn_Uniondecl* _temp1975=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1969)); if( _temp1975->fields ==  0){ goto _LL1962;}{ struct
Cyc_List_List* _temp1976=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1975->tvs, _temp1971);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1975->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp1976,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1962;}} _LL1966: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1962; _LL1968: goto _LL1962; _LL1962:;}{ void* _temp1977= t1;
struct Cyc_Absyn_PtrInfo _temp1995; struct Cyc_Absyn_Conref* _temp1997; struct
Cyc_Absyn_Tqual _temp1999; struct Cyc_Absyn_Conref* _temp2001; void* _temp2003;
void* _temp2005; struct Cyc_Absyn_Exp* _temp2007; struct Cyc_Absyn_Tqual
_temp2009; void* _temp2011; struct Cyc_Absyn_Enumdecl* _temp2013; _LL1979: if((
unsigned int) _temp1977 >  4u?*(( int*) _temp1977) ==  Cyc_Absyn_PointerType: 0){
_LL1996: _temp1995=(( struct Cyc_Absyn_PointerType_struct*) _temp1977)->f1;
_LL2006: _temp2005=( void*) _temp1995.elt_typ; goto _LL2004; _LL2004: _temp2003=(
void*) _temp1995.rgn_typ; goto _LL2002; _LL2002: _temp2001= _temp1995.nullable;
goto _LL2000; _LL2000: _temp1999= _temp1995.tq; goto _LL1998; _LL1998: _temp1997=
_temp1995.bounds; goto _LL1980;} else{ goto _LL1981;} _LL1981: if(( unsigned int)
_temp1977 >  4u?*(( int*) _temp1977) ==  Cyc_Absyn_ArrayType: 0){ _LL2012:
_temp2011=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1977)->f1; goto
_LL2010; _LL2010: _temp2009=(( struct Cyc_Absyn_ArrayType_struct*) _temp1977)->f2;
goto _LL2008; _LL2008: _temp2007=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1977)->f3; goto _LL1982;} else{ goto _LL1983;} _LL1983: if(( unsigned int)
_temp1977 >  4u?*(( int*) _temp1977) ==  Cyc_Absyn_EnumType: 0){ _LL2014:
_temp2013=(( struct Cyc_Absyn_EnumType_struct*) _temp1977)->f2; goto _LL1984;}
else{ goto _LL1985;} _LL1985: if(( unsigned int) _temp1977 >  4u?*(( int*)
_temp1977) ==  Cyc_Absyn_IntType: 0){ goto _LL1986;} else{ goto _LL1987;}
_LL1987: if( _temp1977 == ( void*) Cyc_Absyn_FloatType){ goto _LL1988;} else{
goto _LL1989;} _LL1989: if( _temp1977 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1990;} else{ goto _LL1991;} _LL1991: if( _temp1977 == ( void*) Cyc_Absyn_VoidType){
goto _LL1992;} else{ goto _LL1993;} _LL1993: goto _LL1994; _LL1980:{ void*
_temp2015= t2; struct Cyc_Absyn_PtrInfo _temp2023; struct Cyc_Absyn_Conref*
_temp2025; struct Cyc_Absyn_Tqual _temp2027; struct Cyc_Absyn_Conref* _temp2029;
void* _temp2031; void* _temp2033; _LL2017: if(( unsigned int) _temp2015 >  4u?*((
int*) _temp2015) ==  Cyc_Absyn_PointerType: 0){ _LL2024: _temp2023=(( struct Cyc_Absyn_PointerType_struct*)
_temp2015)->f1; _LL2034: _temp2033=( void*) _temp2023.elt_typ; goto _LL2032;
_LL2032: _temp2031=( void*) _temp2023.rgn_typ; goto _LL2030; _LL2030: _temp2029=
_temp2023.nullable; goto _LL2028; _LL2028: _temp2027= _temp2023.tq; goto _LL2026;
_LL2026: _temp2025= _temp2023.bounds; goto _LL2018;} else{ goto _LL2019;}
_LL2019: if( _temp2015 == ( void*) Cyc_Absyn_VoidType){ goto _LL2020;} else{
goto _LL2021;} _LL2021: goto _LL2022; _LL2018: { struct Cyc_List_List* _temp2035=({
struct Cyc_List_List* _temp2052=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2052->hd=( void*)({ struct _tuple8* _temp2053=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2053->f1= t1;
_temp2053->f2= t2; _temp2053;}); _temp2052->tl= 0; _temp2052;}); int _temp2036=
Cyc_Tcutil_ptrsubtype( te, _temp2035, _temp2005, _temp2033)? ! _temp1999.q_const?
1: _temp2027.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1997, _temp2025); if( ! bounds_ok){ struct _tuple8 _temp2038=({ struct
_tuple8 _temp2037; _temp2037.f1= Cyc_Absyn_conref_val( _temp1997); _temp2037.f2=
Cyc_Absyn_conref_val( _temp2025); _temp2037;}); void* _temp2044; struct Cyc_Absyn_Exp*
_temp2046; void* _temp2048; struct Cyc_Absyn_Exp* _temp2050; _LL2040: _LL2049:
_temp2048= _temp2038.f1; if(( unsigned int) _temp2048 >  1u?*(( int*) _temp2048)
==  Cyc_Absyn_Upper_b: 0){ _LL2051: _temp2050=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2048)->f1; goto _LL2045;} else{ goto _LL2042;} _LL2045: _temp2044=
_temp2038.f2; if(( unsigned int) _temp2044 >  1u?*(( int*) _temp2044) ==  Cyc_Absyn_Upper_b:
0){ _LL2047: _temp2046=(( struct Cyc_Absyn_Upper_b_struct*) _temp2044)->f1; goto
_LL2041;} else{ goto _LL2042;} _LL2042: goto _LL2043; _LL2041: if( Cyc_Evexp_eval_const_uint_exp(
_temp2050) >=  Cyc_Evexp_eval_const_uint_exp( _temp2046)){ bounds_ok= 1;} goto
_LL2039; _LL2043: bounds_ok= 1; goto _LL2039; _LL2039:;} return( bounds_ok?
_temp2036: 0)? Cyc_Tcutil_unify( _temp2003, _temp2031)? 1: Cyc_Tcenv_region_outlives(
te, _temp2003, _temp2031): 0;} _LL2020: return 1; _LL2022: goto _LL2016; _LL2016:;}
return 0; _LL1982:{ void* _temp2054= t2; struct Cyc_Absyn_Exp* _temp2060; struct
Cyc_Absyn_Tqual _temp2062; void* _temp2064; _LL2056: if(( unsigned int)
_temp2054 >  4u?*(( int*) _temp2054) ==  Cyc_Absyn_ArrayType: 0){ _LL2065:
_temp2064=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2054)->f1; goto
_LL2063; _LL2063: _temp2062=(( struct Cyc_Absyn_ArrayType_struct*) _temp2054)->f2;
goto _LL2061; _LL2061: _temp2060=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2054)->f3; goto _LL2057;} else{ goto _LL2058;} _LL2058: goto _LL2059;
_LL2057: { int okay; okay=( _temp2007 !=  0? _temp2060 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2007)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2060)): 0; return( okay? Cyc_Tcutil_unify(
_temp2011, _temp2064): 0)? ! _temp2009.q_const? 1: _temp2062.q_const: 0;}
_LL2059: return 0; _LL2055:;} return 0; _LL1984:{ void* _temp2066= t2; struct
Cyc_Absyn_Enumdecl* _temp2072; _LL2068: if(( unsigned int) _temp2066 >  4u?*((
int*) _temp2066) ==  Cyc_Absyn_EnumType: 0){ _LL2073: _temp2072=(( struct Cyc_Absyn_EnumType_struct*)
_temp2066)->f2; goto _LL2069;} else{ goto _LL2070;} _LL2070: goto _LL2071;
_LL2069: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2013))->fields != 
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2072))->fields !=  0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp2013))->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2072))->fields))->v): 0){ return 1;} goto _LL2067; _LL2071:
goto _LL2067; _LL2067:;} goto _LL1986; _LL1986: goto _LL1988; _LL1988: goto
_LL1990; _LL1990: return Cyc_Tcutil_coerceable( t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType;
_LL1992: return t2 == ( void*) Cyc_Absyn_VoidType; _LL1994: return 0; _LL1978:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2074= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2075=(
struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2075[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2076; _temp2076.tag= Cyc_Absyn_Cast_e;
_temp2076.f1=( void*) t; _temp2076.f2= _temp2074; _temp2076;}); _temp2075;})));
e->topt=({ struct Cyc_Core_Opt* _temp2077=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2077->v=( void*) t; _temp2077;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2078= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2080: if(( unsigned int) _temp2078
>  4u?*(( int*) _temp2078) ==  Cyc_Absyn_EnumType: 0){ goto _LL2081;} else{ goto
_LL2082;} _LL2082: if(( unsigned int) _temp2078 >  4u?*(( int*) _temp2078) == 
Cyc_Absyn_IntType: 0){ goto _LL2083;} else{ goto _LL2084;} _LL2084: if((
unsigned int) _temp2078 >  4u?*(( int*) _temp2078) ==  Cyc_Absyn_Evar: 0){ goto
_LL2085;} else{ goto _LL2086;} _LL2086: goto _LL2087; _LL2081: goto _LL2083;
_LL2083: return 1; _LL2085: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2087: return 0; _LL2079:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2088= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2090: if( _temp2088 == ( void*) Cyc_Absyn_FloatType){
goto _LL2091;} else{ goto _LL2092;} _LL2092: if( _temp2088 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2093;} else{ goto _LL2094;} _LL2094: goto _LL2095; _LL2091: goto _LL2093;
_LL2093: return 1; _LL2095: return 0; _LL2089:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp2096= Cyc_Tcutil_compress( t); _LL2098: if(( unsigned int)
_temp2096 >  4u?*(( int*) _temp2096) ==  Cyc_Absyn_FnType: 0){ goto _LL2099;}
else{ goto _LL2100;} _LL2100: goto _LL2101; _LL2099: return 1; _LL2101: return 0;
_LL2097:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp2103=({ struct _tuple8 _temp2102; _temp2102.f1= t1; _temp2102.f2=
t2; _temp2102;}); void* _temp2127; void* _temp2129; void* _temp2131; void*
_temp2133; void* _temp2135; void* _temp2137; void* _temp2139; void* _temp2141;
void* _temp2143; void* _temp2145; void* _temp2147; void* _temp2149; void*
_temp2151; void* _temp2153; void* _temp2155; void* _temp2157; void* _temp2159;
void* _temp2161; void* _temp2163; void* _temp2165; void* _temp2167; void*
_temp2169; _LL2105: _LL2128: _temp2127= _temp2103.f1; if( _temp2127 == ( void*)
Cyc_Absyn_DoubleType){ goto _LL2106;} else{ goto _LL2107;} _LL2107: _LL2130:
_temp2129= _temp2103.f2; if( _temp2129 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2108;} else{ goto _LL2109;} _LL2109: _LL2132: _temp2131= _temp2103.f1; if(
_temp2131 == ( void*) Cyc_Absyn_FloatType){ goto _LL2110;} else{ goto _LL2111;}
_LL2111: _LL2134: _temp2133= _temp2103.f2; if( _temp2133 == ( void*) Cyc_Absyn_FloatType){
goto _LL2112;} else{ goto _LL2113;} _LL2113: _LL2136: _temp2135= _temp2103.f1;
if(( unsigned int) _temp2135 >  4u?*(( int*) _temp2135) ==  Cyc_Absyn_IntType: 0){
_LL2140: _temp2139=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2135)->f1;
if( _temp2139 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2138;} else{ goto _LL2115;}
_LL2138: _temp2137=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2135)->f2;
if( _temp2137 == ( void*) Cyc_Absyn_B8){ goto _LL2114;} else{ goto _LL2115;}}
else{ goto _LL2115;} _LL2115: _LL2142: _temp2141= _temp2103.f2; if((
unsigned int) _temp2141 >  4u?*(( int*) _temp2141) ==  Cyc_Absyn_IntType: 0){
_LL2146: _temp2145=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2141)->f1;
if( _temp2145 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2144;} else{ goto _LL2117;}
_LL2144: _temp2143=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2141)->f2;
if( _temp2143 == ( void*) Cyc_Absyn_B8){ goto _LL2116;} else{ goto _LL2117;}}
else{ goto _LL2117;} _LL2117: _LL2148: _temp2147= _temp2103.f1; if((
unsigned int) _temp2147 >  4u?*(( int*) _temp2147) ==  Cyc_Absyn_IntType: 0){
_LL2152: _temp2151=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2147)->f1;
if( _temp2151 == ( void*) Cyc_Absyn_Signed){ goto _LL2150;} else{ goto _LL2119;}
_LL2150: _temp2149=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2147)->f2;
if( _temp2149 == ( void*) Cyc_Absyn_B8){ goto _LL2118;} else{ goto _LL2119;}}
else{ goto _LL2119;} _LL2119: _LL2154: _temp2153= _temp2103.f2; if((
unsigned int) _temp2153 >  4u?*(( int*) _temp2153) ==  Cyc_Absyn_IntType: 0){
_LL2158: _temp2157=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2153)->f1;
if( _temp2157 == ( void*) Cyc_Absyn_Signed){ goto _LL2156;} else{ goto _LL2121;}
_LL2156: _temp2155=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2153)->f2;
if( _temp2155 == ( void*) Cyc_Absyn_B8){ goto _LL2120;} else{ goto _LL2121;}}
else{ goto _LL2121;} _LL2121: _LL2160: _temp2159= _temp2103.f1; if((
unsigned int) _temp2159 >  4u?*(( int*) _temp2159) ==  Cyc_Absyn_IntType: 0){
_LL2164: _temp2163=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2159)->f1;
if( _temp2163 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2162;} else{ goto _LL2123;}
_LL2162: _temp2161=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2159)->f2;
if( _temp2161 == ( void*) Cyc_Absyn_B4){ goto _LL2122;} else{ goto _LL2123;}}
else{ goto _LL2123;} _LL2123: _LL2166: _temp2165= _temp2103.f2; if((
unsigned int) _temp2165 >  4u?*(( int*) _temp2165) ==  Cyc_Absyn_IntType: 0){
_LL2170: _temp2169=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2165)->f1;
if( _temp2169 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2168;} else{ goto _LL2125;}
_LL2168: _temp2167=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2165)->f2;
if( _temp2167 == ( void*) Cyc_Absyn_B4){ goto _LL2124;} else{ goto _LL2125;}}
else{ goto _LL2125;} _LL2125: goto _LL2126; _LL2106: goto _LL2108; _LL2108:
return( void*) Cyc_Absyn_DoubleType; _LL2110: goto _LL2112; _LL2112: return(
void*) Cyc_Absyn_FloatType; _LL2114: goto _LL2116; _LL2116: return Cyc_Absyn_ulonglong_t;
_LL2118: goto _LL2120; _LL2120: return Cyc_Absyn_slonglong_t; _LL2122: goto
_LL2124; _LL2124: return Cyc_Absyn_uint_t; _LL2126: return Cyc_Absyn_sint_t;
_LL2104:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2171=( void*) e->r; struct Cyc_Core_Opt* _temp2177; _LL2173: if(*(( int*)
_temp2171) ==  Cyc_Absyn_AssignOp_e){ _LL2178: _temp2177=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2171)->f2; if( _temp2177 ==  0){ goto _LL2174;} else{ goto _LL2175;}} else{
goto _LL2175;} _LL2175: goto _LL2176; _LL2174:({ void* _temp2179[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2179, sizeof( void*), 0u));}); goto _LL2172; _LL2176: goto _LL2172; _LL2172:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2180=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2180[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2181; _temp2181.tag=
Cyc_Absyn_Forward_constr; _temp2181.f1= c2; _temp2181;}); _temp2180;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2182=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2182[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2183; _temp2183.tag= Cyc_Absyn_Forward_constr;
_temp2183.f1= c1; _temp2183;}); _temp2182;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2184=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2184[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2185; _temp2185.tag= Cyc_Absyn_Forward_constr; _temp2185.f1= c1; _temp2185;});
_temp2184;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2186=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2186[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2187; _temp2187.tag= Cyc_Absyn_Forward_constr;
_temp2187.f1= c2; _temp2187;}); _temp2186;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2188=( int*) _cycalloc_atomic( sizeof( int)); _temp2188[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2188;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2192;
_temp2192.tag= Cyc_Std_Int_pa; _temp2192.f1=( int)(( unsigned int) i);{ void*
_temp2191[ 1u]={& _temp2192}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2191, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2189=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2189->name=({ struct _tagged_arr* _temp2190=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2190[ 0]=
s; _temp2190;}); _temp2189->identity= 0; _temp2189->kind= Cyc_Absyn_new_conref(
k); _temp2189;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2193=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2193, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2195; _temp2195.tag= Cyc_Std_String_pa; _temp2195.f1=( struct _tagged_arr)*
t->name;{ void* _temp2194[ 1u]={& _temp2195}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2194, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2196= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2196, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2197=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2197[ 0]=( struct _tagged_arr) _temp2196; _temp2197;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2198=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2198->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2199=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2199->v=( void*)(*
x).f1; _temp2199;}); _temp2198->f2=(* x).f2; _temp2198->f3=(* x).f3; _temp2198;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2200= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2200=({ struct Cyc_List_List* _temp2201=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2201->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2201->tl= _temp2200; _temp2201;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2202=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2202[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2203; _temp2203.tag= Cyc_Absyn_FnType; _temp2203.f1=({ struct Cyc_Absyn_FnInfo
_temp2204; _temp2204.tvars= fd->tvs; _temp2204.effect= fd->effect; _temp2204.ret_typ=(
void*)(( void*) fd->ret_type); _temp2204.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2204.c_varargs= fd->c_varargs;
_temp2204.cyc_varargs= fd->cyc_varargs; _temp2204.rgn_po= fd->rgn_po; _temp2204.attributes=
_temp2200; _temp2204;}); _temp2203;}); _temp2202;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2205=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp2205->f1=(* pr).f1; _temp2205->f2= t; _temp2205;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2206=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2206->f1=({ struct _tuple15* _temp2207=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2207->f1=(* y).f1; _temp2207->f2=(* y).f2;
_temp2207;}); _temp2206->f2=(* y).f3; _temp2206;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2210; struct _tuple15* _temp2212; struct
_tuple16 _temp2208=* w; _LL2213: _temp2212= _temp2208.f1; goto _LL2211; _LL2211:
_temp2210= _temp2208.f2; goto _LL2209; _LL2209: { struct Cyc_Absyn_Tqual
_temp2216; struct Cyc_Core_Opt* _temp2218; struct _tuple15 _temp2214=* _temp2212;
_LL2219: _temp2218= _temp2214.f1; goto _LL2217; _LL2217: _temp2216= _temp2214.f2;
goto _LL2215; _LL2215: return({ struct _tuple2* _temp2220=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2220->f1= _temp2218; _temp2220->f2=
_temp2216; _temp2220->f3= _temp2210; _temp2220;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2221=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2221->name= f->name;
_temp2221->tq= f->tq; _temp2221->type=( void*) t; _temp2221->width= f->width;
_temp2221->attributes= f->attributes; _temp2221;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2222= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2270; struct Cyc_Absyn_Structdecl** _temp2272;
struct Cyc_List_List* _temp2274; struct _tuple1* _temp2276; struct Cyc_Absyn_Uniondecl**
_temp2278; struct Cyc_List_List* _temp2280; struct _tuple1* _temp2282; struct
Cyc_Absyn_TunionInfo _temp2284; void* _temp2286; struct Cyc_List_List* _temp2288;
void* _temp2290; struct Cyc_Absyn_TunionFieldInfo _temp2292; struct Cyc_List_List*
_temp2294; void* _temp2296; struct Cyc_Core_Opt* _temp2298; struct Cyc_List_List*
_temp2300; struct _tuple1* _temp2302; struct Cyc_Absyn_Exp* _temp2304; struct
Cyc_Absyn_Tqual _temp2306; void* _temp2308; struct Cyc_Absyn_PtrInfo _temp2310;
struct Cyc_Absyn_Conref* _temp2312; struct Cyc_Absyn_Tqual _temp2314; struct Cyc_Absyn_Conref*
_temp2316; void* _temp2318; void* _temp2320; struct Cyc_Absyn_FnInfo _temp2322;
struct Cyc_List_List* _temp2324; struct Cyc_List_List* _temp2326; struct Cyc_Absyn_VarargInfo*
_temp2328; int _temp2330; struct Cyc_List_List* _temp2332; void* _temp2334;
struct Cyc_Core_Opt* _temp2336; struct Cyc_List_List* _temp2338; struct Cyc_List_List*
_temp2340; struct Cyc_List_List* _temp2342; struct Cyc_List_List* _temp2344;
struct Cyc_Core_Opt* _temp2346; void* _temp2348; void* _temp2350; void*
_temp2352; struct Cyc_List_List* _temp2354; _LL2224: if(( unsigned int)
_temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_VarType: 0){ _LL2271:
_temp2270=(( struct Cyc_Absyn_VarType_struct*) _temp2222)->f1; goto _LL2225;}
else{ goto _LL2226;} _LL2226: if(( unsigned int) _temp2222 >  4u?*(( int*)
_temp2222) ==  Cyc_Absyn_StructType: 0){ _LL2277: _temp2276=(( struct Cyc_Absyn_StructType_struct*)
_temp2222)->f1; goto _LL2275; _LL2275: _temp2274=(( struct Cyc_Absyn_StructType_struct*)
_temp2222)->f2; goto _LL2273; _LL2273: _temp2272=(( struct Cyc_Absyn_StructType_struct*)
_temp2222)->f3; goto _LL2227;} else{ goto _LL2228;} _LL2228: if(( unsigned int)
_temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_UnionType: 0){ _LL2283:
_temp2282=(( struct Cyc_Absyn_UnionType_struct*) _temp2222)->f1; goto _LL2281;
_LL2281: _temp2280=(( struct Cyc_Absyn_UnionType_struct*) _temp2222)->f2; goto
_LL2279; _LL2279: _temp2278=(( struct Cyc_Absyn_UnionType_struct*) _temp2222)->f3;
goto _LL2229;} else{ goto _LL2230;} _LL2230: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_TunionType: 0){ _LL2285: _temp2284=(( struct Cyc_Absyn_TunionType_struct*)
_temp2222)->f1; _LL2291: _temp2290=( void*) _temp2284.tunion_info; goto _LL2289;
_LL2289: _temp2288= _temp2284.targs; goto _LL2287; _LL2287: _temp2286=( void*)
_temp2284.rgn; goto _LL2231;} else{ goto _LL2232;} _LL2232: if(( unsigned int)
_temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2293:
_temp2292=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2222)->f1; _LL2297:
_temp2296=( void*) _temp2292.field_info; goto _LL2295; _LL2295: _temp2294=
_temp2292.targs; goto _LL2233;} else{ goto _LL2234;} _LL2234: if(( unsigned int)
_temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_TypedefType: 0){ _LL2303:
_temp2302=(( struct Cyc_Absyn_TypedefType_struct*) _temp2222)->f1; goto _LL2301;
_LL2301: _temp2300=(( struct Cyc_Absyn_TypedefType_struct*) _temp2222)->f2; goto
_LL2299; _LL2299: _temp2298=(( struct Cyc_Absyn_TypedefType_struct*) _temp2222)->f3;
goto _LL2235;} else{ goto _LL2236;} _LL2236: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_ArrayType: 0){ _LL2309: _temp2308=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2222)->f1; goto _LL2307; _LL2307:
_temp2306=(( struct Cyc_Absyn_ArrayType_struct*) _temp2222)->f2; goto _LL2305;
_LL2305: _temp2304=(( struct Cyc_Absyn_ArrayType_struct*) _temp2222)->f3; goto
_LL2237;} else{ goto _LL2238;} _LL2238: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_PointerType: 0){ _LL2311: _temp2310=(( struct Cyc_Absyn_PointerType_struct*)
_temp2222)->f1; _LL2321: _temp2320=( void*) _temp2310.elt_typ; goto _LL2319;
_LL2319: _temp2318=( void*) _temp2310.rgn_typ; goto _LL2317; _LL2317: _temp2316=
_temp2310.nullable; goto _LL2315; _LL2315: _temp2314= _temp2310.tq; goto _LL2313;
_LL2313: _temp2312= _temp2310.bounds; goto _LL2239;} else{ goto _LL2240;}
_LL2240: if(( unsigned int) _temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_FnType:
0){ _LL2323: _temp2322=(( struct Cyc_Absyn_FnType_struct*) _temp2222)->f1;
_LL2339: _temp2338= _temp2322.tvars; goto _LL2337; _LL2337: _temp2336= _temp2322.effect;
goto _LL2335; _LL2335: _temp2334=( void*) _temp2322.ret_typ; goto _LL2333;
_LL2333: _temp2332= _temp2322.args; goto _LL2331; _LL2331: _temp2330= _temp2322.c_varargs;
goto _LL2329; _LL2329: _temp2328= _temp2322.cyc_varargs; goto _LL2327; _LL2327:
_temp2326= _temp2322.rgn_po; goto _LL2325; _LL2325: _temp2324= _temp2322.attributes;
goto _LL2241;} else{ goto _LL2242;} _LL2242: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_TupleType: 0){ _LL2341: _temp2340=(( struct Cyc_Absyn_TupleType_struct*)
_temp2222)->f1; goto _LL2243;} else{ goto _LL2244;} _LL2244: if(( unsigned int)
_temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_AnonStructType: 0){ _LL2343:
_temp2342=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2222)->f1; goto
_LL2245;} else{ goto _LL2246;} _LL2246: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2345: _temp2344=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2222)->f1; goto _LL2247;} else{ goto
_LL2248;} _LL2248: if(( unsigned int) _temp2222 >  4u?*(( int*) _temp2222) == 
Cyc_Absyn_Evar: 0){ _LL2347: _temp2346=(( struct Cyc_Absyn_Evar_struct*)
_temp2222)->f2; goto _LL2249;} else{ goto _LL2250;} _LL2250: if(( unsigned int)
_temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2349:
_temp2348=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2222)->f1;
goto _LL2251;} else{ goto _LL2252;} _LL2252: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_EnumType: 0){ goto _LL2253;} else{ goto _LL2254;}
_LL2254: if( _temp2222 == ( void*) Cyc_Absyn_VoidType){ goto _LL2255;} else{
goto _LL2256;} _LL2256: if(( unsigned int) _temp2222 >  4u?*(( int*) _temp2222)
==  Cyc_Absyn_IntType: 0){ goto _LL2257;} else{ goto _LL2258;} _LL2258: if(
_temp2222 == ( void*) Cyc_Absyn_FloatType){ goto _LL2259;} else{ goto _LL2260;}
_LL2260: if( _temp2222 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2261;} else{
goto _LL2262;} _LL2262: if( _temp2222 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL2263;} else{ goto _LL2264;} _LL2264: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_RgnsEff: 0){ _LL2351: _temp2350=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2222)->f1; goto _LL2265;} else{ goto _LL2266;}
_LL2266: if(( unsigned int) _temp2222 >  4u?*(( int*) _temp2222) ==  Cyc_Absyn_AccessEff:
0){ _LL2353: _temp2352=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2222)->f1;
goto _LL2267;} else{ goto _LL2268;} _LL2268: if(( unsigned int) _temp2222 >  4u?*((
int*) _temp2222) ==  Cyc_Absyn_JoinEff: 0){ _LL2355: _temp2354=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2222)->f1; goto _LL2269;} else{ goto _LL2223;} _LL2225: { struct
_handler_cons _temp2356; _push_handler(& _temp2356);{ int _temp2358= 0; if(
setjmp( _temp2356.handler)){ _temp2358= 1;} if( ! _temp2358){{ void* _temp2359=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2270); _npop_handler( 0u); return _temp2359;}; _pop_handler();} else{
void* _temp2357=( void*) _exn_thrown; void* _temp2361= _temp2357; _LL2363: if(
_temp2361 ==  Cyc_Core_Not_found){ goto _LL2364;} else{ goto _LL2365;} _LL2365:
goto _LL2366; _LL2364: return t; _LL2366:( void) _throw( _temp2361); _LL2362:;}}}
_LL2227: { struct Cyc_List_List* _temp2367= Cyc_Tcutil_substs( rgn, inst,
_temp2274); return _temp2367 ==  _temp2274? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2368=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2368[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2369; _temp2369.tag=
Cyc_Absyn_StructType; _temp2369.f1= _temp2276; _temp2369.f2= _temp2367;
_temp2369.f3= _temp2272; _temp2369;}); _temp2368;});} _LL2229: { struct Cyc_List_List*
_temp2370= Cyc_Tcutil_substs( rgn, inst, _temp2280); return _temp2370 == 
_temp2280? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2371=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2371[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2372; _temp2372.tag= Cyc_Absyn_UnionType;
_temp2372.f1= _temp2282; _temp2372.f2= _temp2370; _temp2372.f3= _temp2278;
_temp2372;}); _temp2371;});} _LL2231: { struct Cyc_List_List* _temp2373= Cyc_Tcutil_substs(
rgn, inst, _temp2288); void* _temp2374= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2286); return( _temp2373 ==  _temp2288? _temp2374 ==  _temp2286: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2375=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2375[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2376; _temp2376.tag= Cyc_Absyn_TunionType;
_temp2376.f1=({ struct Cyc_Absyn_TunionInfo _temp2377; _temp2377.tunion_info=(
void*) _temp2290; _temp2377.targs= _temp2373; _temp2377.rgn=( void*) _temp2374;
_temp2377;}); _temp2376;}); _temp2375;});} _LL2233: { struct Cyc_List_List*
_temp2378= Cyc_Tcutil_substs( rgn, inst, _temp2294); return _temp2378 == 
_temp2294? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2379=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2379[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2380; _temp2380.tag=
Cyc_Absyn_TunionFieldType; _temp2380.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2381; _temp2381.field_info=( void*) _temp2296; _temp2381.targs= _temp2378;
_temp2381;}); _temp2380;}); _temp2379;});} _LL2235: { struct Cyc_List_List*
_temp2382= Cyc_Tcutil_substs( rgn, inst, _temp2300); return _temp2382 == 
_temp2300? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2383=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2383[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2384; _temp2384.tag=
Cyc_Absyn_TypedefType; _temp2384.f1= _temp2302; _temp2384.f2= _temp2382;
_temp2384.f3= _temp2298; _temp2384;}); _temp2383;});} _LL2237: { void* _temp2385=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2308); return _temp2385 ==  _temp2308? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2386=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2386[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2387; _temp2387.tag= Cyc_Absyn_ArrayType;
_temp2387.f1=( void*) _temp2385; _temp2387.f2= _temp2306; _temp2387.f3=
_temp2304; _temp2387;}); _temp2386;});} _LL2239: { void* _temp2388= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2320); void* _temp2389= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2318); if( _temp2388 ==  _temp2320? _temp2389 ==  _temp2318: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2390=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2390[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2391; _temp2391.tag= Cyc_Absyn_PointerType;
_temp2391.f1=({ struct Cyc_Absyn_PtrInfo _temp2392; _temp2392.elt_typ=( void*)
_temp2388; _temp2392.rgn_typ=( void*) _temp2389; _temp2392.nullable= _temp2316;
_temp2392.tq= _temp2314; _temp2392.bounds= _temp2312; _temp2392;}); _temp2391;});
_temp2390;});} _LL2241:{ struct Cyc_List_List* _temp2393= _temp2338; for( 0;
_temp2393 !=  0; _temp2393=(( struct Cyc_List_List*) _check_null( _temp2393))->tl){
inst=({ struct Cyc_List_List* _temp2394=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2394->hd=( void*)({ struct _tuple5*
_temp2395=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2395->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2393))->hd; _temp2395->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2396=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2396[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2397; _temp2397.tag= Cyc_Absyn_VarType;
_temp2397.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2393))->hd; _temp2397;}); _temp2396;}); _temp2395;}); _temp2394->tl= inst;
_temp2394;});}}{ struct Cyc_List_List* _temp2400; struct Cyc_List_List*
_temp2402; struct _tuple0 _temp2398=(( struct _tuple0(*)( struct _RegionHandle*
r1, struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2332)); _LL2403:
_temp2402= _temp2398.f1; goto _LL2401; _LL2401: _temp2400= _temp2398.f2; goto
_LL2399; _LL2399: { struct Cyc_List_List* _temp2404= Cyc_Tcutil_substs( rgn,
inst, _temp2400); struct Cyc_List_List* _temp2405=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r1,
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2402, _temp2404)); struct Cyc_Core_Opt* eff2; if( _temp2336 ==  0){
eff2= 0;} else{ void* _temp2406= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2336))->v); if( _temp2406 == ( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2336))->v){ eff2= _temp2336;} else{ eff2=({
struct Cyc_Core_Opt* _temp2407=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2407->v=( void*) _temp2406; _temp2407;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2328 ==  0){ cyc_varargs2= 0;} else{ int _temp2410; void*
_temp2412; struct Cyc_Absyn_Tqual _temp2414; struct Cyc_Core_Opt* _temp2416;
struct Cyc_Absyn_VarargInfo _temp2408=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2328)); _LL2417: _temp2416= _temp2408.name; goto _LL2415;
_LL2415: _temp2414= _temp2408.tq; goto _LL2413; _LL2413: _temp2412=( void*)
_temp2408.type; goto _LL2411; _LL2411: _temp2410= _temp2408.inject; goto _LL2409;
_LL2409: { void* _temp2418= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2412); if(
_temp2418 ==  _temp2412){ cyc_varargs2= _temp2328;} else{ cyc_varargs2=({ struct
Cyc_Absyn_VarargInfo* _temp2419=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2419->name= _temp2416; _temp2419->tq=
_temp2414; _temp2419->type=( void*) _temp2418; _temp2419->inject= _temp2410;
_temp2419;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2422;
struct Cyc_List_List* _temp2424; struct _tuple0 _temp2420= Cyc_List_rsplit( rgn,
rgn, _temp2326); _LL2425: _temp2424= _temp2420.f1; goto _LL2423; _LL2423:
_temp2422= _temp2420.f2; goto _LL2421; _LL2421: { struct Cyc_List_List*
_temp2426= Cyc_Tcutil_substs( rgn, inst, _temp2424); struct Cyc_List_List*
_temp2427= Cyc_Tcutil_substs( rgn, inst, _temp2422); if( _temp2426 ==  _temp2424?
_temp2427 ==  _temp2422: 0){ rgn_po2= _temp2326;} else{ rgn_po2= Cyc_List_zip(
_temp2426, _temp2427);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2428=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2428[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2429; _temp2429.tag= Cyc_Absyn_FnType;
_temp2429.f1=({ struct Cyc_Absyn_FnInfo _temp2430; _temp2430.tvars= _temp2338;
_temp2430.effect= eff2; _temp2430.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2334); _temp2430.args= _temp2405; _temp2430.c_varargs= _temp2330;
_temp2430.cyc_varargs= cyc_varargs2; _temp2430.rgn_po= rgn_po2; _temp2430.attributes=
_temp2324; _temp2430;}); _temp2429;}); _temp2428;});}}}}} _LL2243: { struct Cyc_List_List*
_temp2431=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2340); struct Cyc_List_List* _temp2432= Cyc_Tcutil_substs( rgn, inst,
_temp2431); if( _temp2432 ==  _temp2431){ return t;}{ struct Cyc_List_List*
_temp2433=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2340, _temp2432); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2434=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2434[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2435; _temp2435.tag= Cyc_Absyn_TupleType;
_temp2435.f1= _temp2433; _temp2435;}); _temp2434;});}} _LL2245: { struct Cyc_List_List*
_temp2436=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2342); struct Cyc_List_List* _temp2437= Cyc_Tcutil_substs( rgn, inst,
_temp2436); if( _temp2437 ==  _temp2436){ return t;}{ struct Cyc_List_List*
_temp2438=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2342, _temp2437); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2439=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2439[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2440; _temp2440.tag= Cyc_Absyn_AnonStructType;
_temp2440.f1= _temp2438; _temp2440;}); _temp2439;});}} _LL2247: { struct Cyc_List_List*
_temp2441=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2344); struct Cyc_List_List* _temp2442= Cyc_Tcutil_substs( rgn, inst,
_temp2441); if( _temp2442 ==  _temp2441){ return t;}{ struct Cyc_List_List*
_temp2443=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2344, _temp2442); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2444=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2444[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2445; _temp2445.tag= Cyc_Absyn_AnonStructType;
_temp2445.f1= _temp2443; _temp2445;}); _temp2444;});}} _LL2249: if( _temp2346 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2346))->v);} else{ return t;} _LL2251: { void* _temp2446= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2348); return _temp2446 ==  _temp2348? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2447=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2447[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2448; _temp2448.tag= Cyc_Absyn_RgnHandleType; _temp2448.f1=( void*)
_temp2446; _temp2448;}); _temp2447;});} _LL2253: return t; _LL2255: return t;
_LL2257: return t; _LL2259: return t; _LL2261: return t; _LL2263: return t;
_LL2265: { void* _temp2449= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2350);
return _temp2449 ==  _temp2350? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2450=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2450[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2451; _temp2451.tag= Cyc_Absyn_RgnsEff;
_temp2451.f1=( void*) _temp2449; _temp2451;}); _temp2450;});} _LL2267: { void*
_temp2452= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2352); return _temp2452 == 
_temp2352? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2453=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2453[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2454; _temp2454.tag= Cyc_Absyn_AccessEff;
_temp2454.f1=( void*) _temp2452; _temp2454;}); _temp2453;});} _LL2269: { struct
Cyc_List_List* _temp2455= Cyc_Tcutil_substs( rgn, inst, _temp2354); return
_temp2455 ==  _temp2354? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2456=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2456[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2457; _temp2457.tag= Cyc_Absyn_JoinEff;
_temp2457.f1= _temp2455; _temp2457;}); _temp2456;});} _LL2223:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2458=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2459=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2460= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2458); struct Cyc_List_List* _temp2461= Cyc_Tcutil_substs( rgn,
inst, _temp2459); if( _temp2458 ==  _temp2460? _temp2459 ==  _temp2461: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2462=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2462->hd=(
void*) _temp2460; _temp2462->tl= _temp2461; _temp2462;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2507=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2507[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2508; _temp2508.tag= Cyc_Absyn_Const_e; _temp2508.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2508;}); _temp2507;}), loc);{ void*
_temp2463= Cyc_Tcutil_compress( t); void* _temp2477; void* _temp2479; void*
_temp2481; void* _temp2483; _LL2465: if(( unsigned int) _temp2463 >  4u?*(( int*)
_temp2463) ==  Cyc_Absyn_PointerType: 0){ goto _LL2466;} else{ goto _LL2467;}
_LL2467: if(( unsigned int) _temp2463 >  4u?*(( int*) _temp2463) ==  Cyc_Absyn_IntType:
0){ _LL2480: _temp2479=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2463)->f1;
goto _LL2478; _LL2478: _temp2477=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2463)->f2; if( _temp2477 == ( void*) Cyc_Absyn_B1){ goto _LL2468;} else{
goto _LL2469;}} else{ goto _LL2469;} _LL2469: if(( unsigned int) _temp2463 >  4u?*((
int*) _temp2463) ==  Cyc_Absyn_IntType: 0){ _LL2484: _temp2483=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2463)->f1; goto _LL2482; _LL2482: _temp2481=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2463)->f2; goto _LL2470;} else{
goto _LL2471;} _LL2471: if( _temp2463 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2472;} else{ goto _LL2473;} _LL2473: if( _temp2463 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2474;} else{ goto _LL2475;} _LL2475: goto _LL2476; _LL2466: goto _LL2464;
_LL2468:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2485=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2485[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2486; _temp2486.tag= Cyc_Absyn_Const_e;
_temp2486.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2487=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2487[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2488; _temp2488.tag= Cyc_Absyn_Char_c;
_temp2488.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2488.f2='\000'; _temp2488;});
_temp2487;})); _temp2486;}); _temp2485;}))); goto _LL2464; _LL2470:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2489=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2489[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2490; _temp2490.tag= Cyc_Absyn_Const_e; _temp2490.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2491=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2491[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2492; _temp2492.tag= Cyc_Absyn_Int_c; _temp2492.f1=( void*) _temp2483;
_temp2492.f2= 0; _temp2492;}); _temp2491;})); _temp2490;}); _temp2489;}))); if(
_temp2481 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2493=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2493[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2494; _temp2494.tag= Cyc_Absyn_Cast_e;
_temp2494.f1=( void*) t; _temp2494.f2= e; _temp2494;}); _temp2493;}), loc);}
goto _LL2464; _LL2472:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2495=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2495[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2496; _temp2496.tag= Cyc_Absyn_Const_e;
_temp2496.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2497=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2497[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2498; _temp2498.tag= Cyc_Absyn_Float_c;
_temp2498.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2498;});
_temp2497;})); _temp2496;}); _temp2495;}))); goto _LL2464; _LL2474:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2499=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2499[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2500; _temp2500.tag= Cyc_Absyn_Cast_e; _temp2500.f1=( void*) t; _temp2500.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2501=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2501[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2502; _temp2502.tag= Cyc_Absyn_Const_e;
_temp2502.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2503=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2503[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2504; _temp2504.tag= Cyc_Absyn_Float_c;
_temp2504.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2504;});
_temp2503;})); _temp2502;}); _temp2501;}), loc); _temp2500;}); _temp2499;})));
goto _LL2464; _LL2476:({ struct Cyc_Std_String_pa_struct _temp2506; _temp2506.tag=
Cyc_Std_String_pa; _temp2506.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2505[ 1u]={& _temp2506}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2505, sizeof( void*), 1u));}});
goto _LL2464; _LL2464:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2509=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp2509->f1= tv; _temp2509->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2510=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2510->v=( void*) k; _temp2510;}),({ struct Cyc_Core_Opt*
_temp2511=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2511->v=( void*) s; _temp2511;})); _temp2509;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2514;
struct _RegionHandle* _temp2515; struct Cyc_List_List* _temp2517; struct _tuple6*
_temp2512= env; _temp2514=* _temp2512; _LL2518: _temp2517= _temp2514.f1; goto
_LL2516; _LL2516: _temp2515= _temp2514.f2; goto _LL2513; _LL2513: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2519=( struct _tuple5*) _region_malloc(
_temp2515, sizeof( struct _tuple5)); _temp2519->f1= tv; _temp2519->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2520=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2520->v=( void*) k; _temp2520;}),({ struct Cyc_Core_Opt*
_temp2521=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2521->v=( void*) _temp2517; _temp2521;})); _temp2519;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_Std_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_Std_String_pa_struct _temp2525; _temp2525.tag= Cyc_Std_String_pa;
_temp2525.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct
_temp2524; _temp2524.tag= Cyc_Std_String_pa; _temp2524.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_Std_String_pa_struct _temp2523;
_temp2523.tag= Cyc_Std_String_pa; _temp2523.f1=( struct _tagged_arr)* tv->name;{
void* _temp2522[ 3u]={& _temp2523,& _temp2524,& _temp2525}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2522, sizeof( void*), 3u));}}}});} if( tv->identity
==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!= *(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2526[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2526, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2527=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2527->hd=( void*) tv; _temp2527->tl= tvs; _temp2527;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2528[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2528, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity ==  0){({ void* _temp2529[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2529, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2530=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2530->hd=( void*) tv; _temp2530->tl=
tvs; _temp2530;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp2532; _temp2532.tag= Cyc_Std_String_pa;
_temp2532.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2531[
1u]={& _temp2532};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2531, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2533=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2533->hd=(
void*) tv; _temp2533->tl= tvs; _temp2533;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2534= Cyc_Tcutil_compress( e);
int _temp2540; _LL2536: if(( unsigned int) _temp2534 >  4u?*(( int*) _temp2534)
==  Cyc_Absyn_Evar: 0){ _LL2541: _temp2540=(( struct Cyc_Absyn_Evar_struct*)
_temp2534)->f3; goto _LL2537;} else{ goto _LL2538;} _LL2538: goto _LL2539;
_LL2537:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2542= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2548; _LL2544: if((
unsigned int) _temp2542 >  4u?*(( int*) _temp2542) ==  Cyc_Absyn_Evar: 0){
_LL2549: _temp2548=(( struct Cyc_Absyn_Evar_struct*) _temp2542)->f3; goto
_LL2545;} else{ goto _LL2546;} _LL2546: goto _LL2547; _LL2545: if( _temp2540 == 
_temp2548){ return es;} goto _LL2543; _LL2547: goto _LL2543; _LL2543:;}} return({
struct Cyc_List_List* _temp2550=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2550->hd=( void*) e; _temp2550->tl= es; _temp2550;});
_LL2539: return es; _LL2535:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b !=  0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) == *(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2551=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2551->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2551->tl= r; _temp2551;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp2553; _temp2553.tag= Cyc_Std_String_pa; _temp2553.f1=( struct _tagged_arr)*
fn;{ void* _temp2552[ 1u]={& _temp2553}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2552, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2554= Cyc_Tcutil_compress( field_typ); void* _temp2560;
_LL2556: if(( unsigned int) _temp2554 >  4u?*(( int*) _temp2554) ==  Cyc_Absyn_IntType:
0){ _LL2561: _temp2560=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2554)->f2;
goto _LL2557;} else{ goto _LL2558;} _LL2558: goto _LL2559; _LL2557:{ void*
_temp2562= _temp2560; _LL2564: if( _temp2562 == ( void*) Cyc_Absyn_B1){ goto
_LL2565;} else{ goto _LL2566;} _LL2566: if( _temp2562 == ( void*) Cyc_Absyn_B2){
goto _LL2567;} else{ goto _LL2568;} _LL2568: if( _temp2562 == ( void*) Cyc_Absyn_B4){
goto _LL2569;} else{ goto _LL2570;} _LL2570: if( _temp2562 == ( void*) Cyc_Absyn_B8){
goto _LL2571;} else{ goto _LL2563;} _LL2565: if( w >  8){({ void* _temp2572[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2572, sizeof( void*), 0u));});} goto
_LL2563; _LL2567: if( w >  16){({ void* _temp2573[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2573, sizeof( void*), 0u));});} goto _LL2563; _LL2569: if( w >  32){({ void*
_temp2574[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2574, sizeof( void*), 0u));});}
goto _LL2563; _LL2571: if( w >  64){({ void* _temp2575[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2575, sizeof( void*), 0u));});} goto _LL2563; _LL2563:;} goto
_LL2555; _LL2559:({ struct Cyc_Std_String_pa_struct _temp2578; _temp2578.tag=
Cyc_Std_String_pa; _temp2578.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp2577; _temp2577.tag= Cyc_Std_String_pa;
_temp2577.f1=( struct _tagged_arr)* fn;{ void* _temp2576[ 2u]={& _temp2577,&
_temp2578}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2576, sizeof( void*), 2u));}}});
goto _LL2555; _LL2555:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp2579=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL2581: if( _temp2579 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL2582;} else{ goto _LL2583;} _LL2583: if((
unsigned int) _temp2579 >  16u?*(( int*) _temp2579) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2584;} else{ goto _LL2585;} _LL2585: goto _LL2586; _LL2582: continue;
_LL2584: continue; _LL2586:({ struct Cyc_Std_String_pa_struct _temp2589;
_temp2589.tag= Cyc_Std_String_pa; _temp2589.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp2588; _temp2588.tag= Cyc_Std_String_pa;
_temp2588.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp2587[ 2u]={& _temp2588,&
_temp2589}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2587, sizeof( void*), 2u));}}});
goto _LL2580; _LL2580:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv
cvtenv, void* expected_kind, void* t){{ void* _temp2590= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp2638; struct Cyc_Core_Opt** _temp2640; struct Cyc_Core_Opt*
_temp2641; struct Cyc_Core_Opt** _temp2643; struct Cyc_Absyn_Tvar* _temp2644;
struct Cyc_Absyn_Enumdecl* _temp2646; struct Cyc_Absyn_Enumdecl** _temp2648;
struct _tuple1* _temp2649; struct Cyc_Absyn_TunionInfo _temp2651; void*
_temp2653; struct Cyc_List_List* _temp2655; void* _temp2657; void** _temp2659;
struct Cyc_Absyn_TunionFieldInfo _temp2660; struct Cyc_List_List* _temp2662;
void* _temp2664; void** _temp2666; struct Cyc_Absyn_PtrInfo _temp2667; struct
Cyc_Absyn_Conref* _temp2669; struct Cyc_Absyn_Tqual _temp2671; struct Cyc_Absyn_Conref*
_temp2673; void* _temp2675; void* _temp2677; struct Cyc_Absyn_Exp* _temp2679;
struct Cyc_Absyn_Tqual _temp2681; void* _temp2683; struct Cyc_Absyn_FnInfo
_temp2685; struct Cyc_List_List* _temp2687; struct Cyc_List_List* _temp2689;
struct Cyc_Absyn_VarargInfo* _temp2691; int _temp2693; struct Cyc_List_List*
_temp2695; void* _temp2697; struct Cyc_Core_Opt* _temp2699; struct Cyc_Core_Opt**
_temp2701; struct Cyc_List_List* _temp2702; struct Cyc_List_List** _temp2704;
struct Cyc_List_List* _temp2705; struct Cyc_List_List* _temp2707; struct Cyc_List_List*
_temp2709; struct Cyc_Absyn_Structdecl** _temp2711; struct Cyc_Absyn_Structdecl***
_temp2713; struct Cyc_List_List* _temp2714; struct _tuple1* _temp2716; struct
Cyc_Absyn_Uniondecl** _temp2718; struct Cyc_Absyn_Uniondecl*** _temp2720; struct
Cyc_List_List* _temp2721; struct _tuple1* _temp2723; struct Cyc_Core_Opt*
_temp2725; struct Cyc_Core_Opt** _temp2727; struct Cyc_List_List* _temp2728;
struct Cyc_List_List** _temp2730; struct _tuple1* _temp2731; void* _temp2733;
void* _temp2735; void* _temp2737; struct Cyc_List_List* _temp2739; _LL2592: if(
_temp2590 == ( void*) Cyc_Absyn_VoidType){ goto _LL2593;} else{ goto _LL2594;}
_LL2594: if(( unsigned int) _temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_Evar:
0){ _LL2642: _temp2641=(( struct Cyc_Absyn_Evar_struct*) _temp2590)->f1;
_temp2643=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2590)->f1;
goto _LL2639; _LL2639: _temp2638=(( struct Cyc_Absyn_Evar_struct*) _temp2590)->f2;
_temp2640=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2590)->f2;
goto _LL2595;} else{ goto _LL2596;} _LL2596: if(( unsigned int) _temp2590 >  4u?*((
int*) _temp2590) ==  Cyc_Absyn_VarType: 0){ _LL2645: _temp2644=(( struct Cyc_Absyn_VarType_struct*)
_temp2590)->f1; goto _LL2597;} else{ goto _LL2598;} _LL2598: if(( unsigned int)
_temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_EnumType: 0){ _LL2650:
_temp2649=(( struct Cyc_Absyn_EnumType_struct*) _temp2590)->f1; goto _LL2647;
_LL2647: _temp2646=(( struct Cyc_Absyn_EnumType_struct*) _temp2590)->f2;
_temp2648=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2590)->f2; goto _LL2599;} else{ goto _LL2600;} _LL2600: if(( unsigned int)
_temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_TunionType: 0){ _LL2652:
_temp2651=(( struct Cyc_Absyn_TunionType_struct*) _temp2590)->f1; _LL2658:
_temp2657=( void*) _temp2651.tunion_info; _temp2659=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2590)->f1).tunion_info; goto _LL2656; _LL2656: _temp2655= _temp2651.targs;
goto _LL2654; _LL2654: _temp2653=( void*) _temp2651.rgn; goto _LL2601;} else{
goto _LL2602;} _LL2602: if(( unsigned int) _temp2590 >  4u?*(( int*) _temp2590)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2661: _temp2660=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2590)->f1; _LL2665: _temp2664=( void*) _temp2660.field_info; _temp2666=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2590)->f1).field_info;
goto _LL2663; _LL2663: _temp2662= _temp2660.targs; goto _LL2603;} else{ goto
_LL2604;} _LL2604: if(( unsigned int) _temp2590 >  4u?*(( int*) _temp2590) == 
Cyc_Absyn_PointerType: 0){ _LL2668: _temp2667=(( struct Cyc_Absyn_PointerType_struct*)
_temp2590)->f1; _LL2678: _temp2677=( void*) _temp2667.elt_typ; goto _LL2676;
_LL2676: _temp2675=( void*) _temp2667.rgn_typ; goto _LL2674; _LL2674: _temp2673=
_temp2667.nullable; goto _LL2672; _LL2672: _temp2671= _temp2667.tq; goto _LL2670;
_LL2670: _temp2669= _temp2667.bounds; goto _LL2605;} else{ goto _LL2606;}
_LL2606: if(( unsigned int) _temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_IntType:
0){ goto _LL2607;} else{ goto _LL2608;} _LL2608: if( _temp2590 == ( void*) Cyc_Absyn_FloatType){
goto _LL2609;} else{ goto _LL2610;} _LL2610: if( _temp2590 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2611;} else{ goto _LL2612;} _LL2612: if(( unsigned int) _temp2590 >  4u?*((
int*) _temp2590) ==  Cyc_Absyn_ArrayType: 0){ _LL2684: _temp2683=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2590)->f1; goto _LL2682; _LL2682:
_temp2681=(( struct Cyc_Absyn_ArrayType_struct*) _temp2590)->f2; goto _LL2680;
_LL2680: _temp2679=(( struct Cyc_Absyn_ArrayType_struct*) _temp2590)->f3; goto
_LL2613;} else{ goto _LL2614;} _LL2614: if(( unsigned int) _temp2590 >  4u?*((
int*) _temp2590) ==  Cyc_Absyn_FnType: 0){ _LL2686: _temp2685=(( struct Cyc_Absyn_FnType_struct*)
_temp2590)->f1; _LL2703: _temp2702= _temp2685.tvars; _temp2704=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2590)->f1).tvars; goto _LL2700; _LL2700:
_temp2699= _temp2685.effect; _temp2701=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2590)->f1).effect; goto _LL2698; _LL2698: _temp2697=( void*) _temp2685.ret_typ;
goto _LL2696; _LL2696: _temp2695= _temp2685.args; goto _LL2694; _LL2694:
_temp2693= _temp2685.c_varargs; goto _LL2692; _LL2692: _temp2691= _temp2685.cyc_varargs;
goto _LL2690; _LL2690: _temp2689= _temp2685.rgn_po; goto _LL2688; _LL2688:
_temp2687= _temp2685.attributes; goto _LL2615;} else{ goto _LL2616;} _LL2616:
if(( unsigned int) _temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_TupleType:
0){ _LL2706: _temp2705=(( struct Cyc_Absyn_TupleType_struct*) _temp2590)->f1;
goto _LL2617;} else{ goto _LL2618;} _LL2618: if(( unsigned int) _temp2590 >  4u?*((
int*) _temp2590) ==  Cyc_Absyn_AnonStructType: 0){ _LL2708: _temp2707=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2590)->f1; goto _LL2619;} else{ goto
_LL2620;} _LL2620: if(( unsigned int) _temp2590 >  4u?*(( int*) _temp2590) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2710: _temp2709=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2590)->f1; goto _LL2621;} else{ goto _LL2622;} _LL2622: if(( unsigned int)
_temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_StructType: 0){ _LL2717:
_temp2716=(( struct Cyc_Absyn_StructType_struct*) _temp2590)->f1; goto _LL2715;
_LL2715: _temp2714=(( struct Cyc_Absyn_StructType_struct*) _temp2590)->f2; goto
_LL2712; _LL2712: _temp2711=(( struct Cyc_Absyn_StructType_struct*) _temp2590)->f3;
_temp2713=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2590)->f3; goto _LL2623;} else{ goto _LL2624;} _LL2624: if(( unsigned int)
_temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_UnionType: 0){ _LL2724:
_temp2723=(( struct Cyc_Absyn_UnionType_struct*) _temp2590)->f1; goto _LL2722;
_LL2722: _temp2721=(( struct Cyc_Absyn_UnionType_struct*) _temp2590)->f2; goto
_LL2719; _LL2719: _temp2718=(( struct Cyc_Absyn_UnionType_struct*) _temp2590)->f3;
_temp2720=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2590)->f3; goto _LL2625;} else{ goto _LL2626;} _LL2626: if(( unsigned int)
_temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_TypedefType: 0){ _LL2732:
_temp2731=(( struct Cyc_Absyn_TypedefType_struct*) _temp2590)->f1; goto _LL2729;
_LL2729: _temp2728=(( struct Cyc_Absyn_TypedefType_struct*) _temp2590)->f2;
_temp2730=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2590)->f2; goto _LL2726; _LL2726: _temp2725=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2590)->f3; _temp2727=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2590)->f3; goto _LL2627;} else{ goto _LL2628;} _LL2628: if( _temp2590 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2629;} else{ goto _LL2630;} _LL2630: if((
unsigned int) _temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2734: _temp2733=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2590)->f1; goto _LL2631;} else{ goto _LL2632;} _LL2632: if(( unsigned int)
_temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_AccessEff: 0){ _LL2736:
_temp2735=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2590)->f1; goto
_LL2633;} else{ goto _LL2634;} _LL2634: if(( unsigned int) _temp2590 >  4u?*((
int*) _temp2590) ==  Cyc_Absyn_RgnsEff: 0){ _LL2738: _temp2737=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2590)->f1; goto _LL2635;} else{ goto _LL2636;}
_LL2636: if(( unsigned int) _temp2590 >  4u?*(( int*) _temp2590) ==  Cyc_Absyn_JoinEff:
0){ _LL2740: _temp2739=(( struct Cyc_Absyn_JoinEff_struct*) _temp2590)->f1; goto
_LL2637;} else{ goto _LL2591;} _LL2593: goto _LL2591; _LL2595: if(* _temp2643 == 
0){* _temp2643=({ struct Cyc_Core_Opt* _temp2741=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2741->v=( void*) expected_kind;
_temp2741;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2640=({ struct Cyc_Core_Opt* _temp2742=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2742->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2742;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2743= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2640=({ struct Cyc_Core_Opt* _temp2744=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2744->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2745=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2745[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2746; _temp2746.tag= Cyc_Absyn_VarType;
_temp2746.f1= _temp2743; _temp2746;}); _temp2745;})); _temp2744;}); _temp2644=
_temp2743; goto _LL2597;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2591; _LL2597: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2644->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2747=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2747[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2748; _temp2748.tag= Cyc_Absyn_Eq_constr;
_temp2748.f1=( void*) expected_kind; _temp2748;}); _temp2747;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2644); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2644); goto _LL2591;}
_LL2599: if(* _temp2648 ==  0){ struct _handler_cons _temp2749; _push_handler(&
_temp2749);{ int _temp2751= 0; if( setjmp( _temp2749.handler)){ _temp2751= 1;}
if( ! _temp2751){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2649);* _temp2648=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2750=( void*) _exn_thrown; void* _temp2753= _temp2750; _LL2755:
if( _temp2753 ==  Cyc_Dict_Absent){ goto _LL2756;} else{ goto _LL2757;} _LL2757:
goto _LL2758; _LL2756: { struct Cyc_Tcenv_Genv* _temp2759=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Enumdecl* _temp2760=({ struct Cyc_Absyn_Enumdecl* _temp2761=(
struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2761->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp2761->name= _temp2649;
_temp2761->fields= 0; _temp2761;}); Cyc_Tc_tcEnumdecl( te, _temp2759, loc,
_temp2760);{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc,
_temp2649);* _temp2648=( struct Cyc_Absyn_Enumdecl*)* ed; goto _LL2754;}}
_LL2758:( void) _throw( _temp2753); _LL2754:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2648);* _temp2649=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2591;} _LL2601:{ void*
_temp2762=* _temp2659; struct Cyc_Absyn_UnknownTunionInfo _temp2768; int
_temp2770; struct _tuple1* _temp2772; struct Cyc_Absyn_Tuniondecl** _temp2774;
struct Cyc_Absyn_Tuniondecl* _temp2776; _LL2764: if(*(( int*) _temp2762) ==  Cyc_Absyn_UnknownTunion){
_LL2769: _temp2768=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2762)->f1;
_LL2773: _temp2772= _temp2768.name; goto _LL2771; _LL2771: _temp2770= _temp2768.is_xtunion;
goto _LL2765;} else{ goto _LL2766;} _LL2766: if(*(( int*) _temp2762) ==  Cyc_Absyn_KnownTunion){
_LL2775: _temp2774=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2762)->f1;
_temp2776=* _temp2774; goto _LL2767;} else{ goto _LL2763;} _LL2765: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp2777; _push_handler(& _temp2777);{ int
_temp2779= 0; if( setjmp( _temp2777.handler)){ _temp2779= 1;} if( ! _temp2779){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp2772);; _pop_handler();} else{
void* _temp2778=( void*) _exn_thrown; void* _temp2781= _temp2778; _LL2783: if(
_temp2781 ==  Cyc_Dict_Absent){ goto _LL2784;} else{ goto _LL2785;} _LL2785:
goto _LL2786; _LL2784: { struct Cyc_Tcenv_Genv* _temp2787=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Tuniondecl* _temp2788=({ struct Cyc_Absyn_Tuniondecl* _temp2792=(
struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp2792->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp2792->name= _temp2772;
_temp2792->tvs= 0; _temp2792->fields= 0; _temp2792->is_xtunion= _temp2770;
_temp2792;}); Cyc_Tc_tcTuniondecl( te, _temp2787, loc, _temp2788); tudp= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2772); if( _temp2655 !=  0){({ struct Cyc_Std_String_pa_struct
_temp2791; _temp2791.tag= Cyc_Std_String_pa; _temp2791.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2772);{ struct Cyc_Std_String_pa_struct _temp2790;
_temp2790.tag= Cyc_Std_String_pa; _temp2790.f1=( struct _tagged_arr)( _temp2770?
_tag_arr("xtunion", sizeof( unsigned char), 8u): _tag_arr("tunion", sizeof(
unsigned char), 7u));{ void* _temp2789[ 2u]={& _temp2790,& _temp2791}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized %s %s before using", sizeof(
unsigned char), 48u), _tag_arr( _temp2789, sizeof( void*), 2u));}}}); return
cvtenv;} goto _LL2782;} _LL2786:( void) _throw( _temp2781); _LL2782:;}}} if((*
tudp)->is_xtunion !=  _temp2770){({ struct Cyc_Std_String_pa_struct _temp2794;
_temp2794.tag= Cyc_Std_String_pa; _temp2794.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2772);{ void* _temp2793[ 1u]={& _temp2794}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2793, sizeof( void*), 1u));}});}*
_temp2659=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2795=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2795[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2796; _temp2796.tag= Cyc_Absyn_KnownTunion;
_temp2796.f1= tudp; _temp2796;}); _temp2795;}); _temp2776=* tudp; goto _LL2767;}
_LL2767: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2653);{ struct Cyc_List_List* tvs= _temp2776->tvs; for( 0; _temp2655 !=  0?
tvs !=  0: 0;( _temp2655=(( struct Cyc_List_List*) _check_null( _temp2655))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2655))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2655
!=  0){({ struct Cyc_Std_String_pa_struct _temp2798; _temp2798.tag= Cyc_Std_String_pa;
_temp2798.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2776->name);{
void* _temp2797[ 1u]={& _temp2798}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2797, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2800; _temp2800.tag= Cyc_Std_String_pa;
_temp2800.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2776->name);{
void* _temp2799[ 1u]={& _temp2800}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2799, sizeof( void*), 1u));}});}
goto _LL2763;} _LL2763:;} goto _LL2591; _LL2603:{ void* _temp2801=* _temp2666;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2807; int _temp2809; struct _tuple1*
_temp2811; struct _tuple1* _temp2813; struct Cyc_Absyn_Tunionfield* _temp2815;
struct Cyc_Absyn_Tuniondecl* _temp2817; _LL2803: if(*(( int*) _temp2801) ==  Cyc_Absyn_UnknownTunionfield){
_LL2808: _temp2807=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2801)->f1;
_LL2814: _temp2813= _temp2807.tunion_name; goto _LL2812; _LL2812: _temp2811=
_temp2807.field_name; goto _LL2810; _LL2810: _temp2809= _temp2807.is_xtunion;
goto _LL2804;} else{ goto _LL2805;} _LL2805: if(*(( int*) _temp2801) ==  Cyc_Absyn_KnownTunionfield){
_LL2818: _temp2817=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2801)->f1;
goto _LL2816; _LL2816: _temp2815=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2801)->f2; goto _LL2806;} else{ goto _LL2802;} _LL2804: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2819;
_push_handler(& _temp2819);{ int _temp2821= 0; if( setjmp( _temp2819.handler)){
_temp2821= 1;} if( ! _temp2821){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2813);; _pop_handler();} else{ void* _temp2820=( void*) _exn_thrown; void*
_temp2823= _temp2820; _LL2825: if( _temp2823 ==  Cyc_Dict_Absent){ goto _LL2826;}
else{ goto _LL2827;} _LL2827: goto _LL2828; _LL2826:({ struct Cyc_Std_String_pa_struct
_temp2830; _temp2830.tag= Cyc_Std_String_pa; _temp2830.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2813);{ void* _temp2829[ 1u]={& _temp2830}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2829, sizeof( void*), 1u));}}); return cvtenv; _LL2828:( void) _throw(
_temp2823); _LL2824:;}}}{ struct _handler_cons _temp2831; _push_handler(&
_temp2831);{ int _temp2833= 0; if( setjmp( _temp2831.handler)){ _temp2833= 1;}
if( ! _temp2833){{ void* _temp2834= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2811); struct Cyc_Absyn_Tunionfield* _temp2840; struct Cyc_Absyn_Tuniondecl*
_temp2842; _LL2836: if(*(( int*) _temp2834) ==  Cyc_Tcenv_TunionRes){ _LL2843:
_temp2842=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2834)->f1; goto _LL2841;
_LL2841: _temp2840=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2834)->f2; goto
_LL2837;} else{ goto _LL2838;} _LL2838: goto _LL2839; _LL2837: tuf= _temp2840;
tud= _temp2842; if( tud->is_xtunion !=  _temp2809){({ struct Cyc_Std_String_pa_struct
_temp2845; _temp2845.tag= Cyc_Std_String_pa; _temp2845.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2813);{ void* _temp2844[ 1u]={& _temp2845}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2844, sizeof( void*), 1u));}});} goto
_LL2835; _LL2839:({ struct Cyc_Std_String_pa_struct _temp2848; _temp2848.tag=
Cyc_Std_String_pa; _temp2848.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2813);{ struct Cyc_Std_String_pa_struct _temp2847; _temp2847.tag= Cyc_Std_String_pa;
_temp2847.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2811);{ void*
_temp2846[ 2u]={& _temp2847,& _temp2848}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2846, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2849= cvtenv; _npop_handler( 0u); return _temp2849;}
_LL2835:;}; _pop_handler();} else{ void* _temp2832=( void*) _exn_thrown; void*
_temp2851= _temp2832; _LL2853: if( _temp2851 ==  Cyc_Dict_Absent){ goto _LL2854;}
else{ goto _LL2855;} _LL2855: goto _LL2856; _LL2854:({ struct Cyc_Std_String_pa_struct
_temp2859; _temp2859.tag= Cyc_Std_String_pa; _temp2859.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2813);{ struct Cyc_Std_String_pa_struct _temp2858;
_temp2858.tag= Cyc_Std_String_pa; _temp2858.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2811);{ void* _temp2857[ 2u]={& _temp2858,& _temp2859}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2857, sizeof( void*), 2u));}}}); return cvtenv; _LL2856:( void)
_throw( _temp2851); _LL2852:;}}}* _temp2666=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2860=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2860[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2861; _temp2861.tag= Cyc_Absyn_KnownTunionfield; _temp2861.f1= tud;
_temp2861.f2= tuf; _temp2861;}); _temp2860;}); _temp2817= tud; _temp2815= tuf;
goto _LL2806;} _LL2806: { struct Cyc_List_List* tvs= _temp2817->tvs; for( 0;
_temp2662 !=  0? tvs !=  0: 0;( _temp2662=(( struct Cyc_List_List*) _check_null(
_temp2662))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2662))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2662
!=  0){({ struct Cyc_Std_String_pa_struct _temp2864; _temp2864.tag= Cyc_Std_String_pa;
_temp2864.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2815->name);{
struct Cyc_Std_String_pa_struct _temp2863; _temp2863.tag= Cyc_Std_String_pa;
_temp2863.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2817->name);{
void* _temp2862[ 2u]={& _temp2863,& _temp2864}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2862, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2867; _temp2867.tag= Cyc_Std_String_pa;
_temp2867.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2815->name);{
struct Cyc_Std_String_pa_struct _temp2866; _temp2866.tag= Cyc_Std_String_pa;
_temp2866.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2817->name);{
void* _temp2865[ 2u]={& _temp2866,& _temp2867}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2865, sizeof( void*), 2u));}}});}
goto _LL2802;} _LL2802:;} goto _LL2591; _LL2605: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2677); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2675);{ void* _temp2868=( void*)(
Cyc_Absyn_compress_conref( _temp2669))->v; void* _temp2874; struct Cyc_Absyn_Exp*
_temp2876; _LL2870: if(( unsigned int) _temp2868 >  1u?*(( int*) _temp2868) == 
Cyc_Absyn_Eq_constr: 0){ _LL2875: _temp2874=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2868)->f1; if(( unsigned int) _temp2874 >  1u?*(( int*) _temp2874) ==  Cyc_Absyn_Upper_b:
0){ _LL2877: _temp2876=(( struct Cyc_Absyn_Upper_b_struct*) _temp2874)->f1; goto
_LL2871;} else{ goto _LL2872;}} else{ goto _LL2872;} _LL2872: goto _LL2873;
_LL2871: if( ! Cyc_Tcutil_is_const_exp( te, _temp2876)){({ void* _temp2878[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2878, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2876); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2876)){({ void*
_temp2879[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2879, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2876); goto _LL2869; _LL2873: goto _LL2869; _LL2869:;} goto _LL2591;
_LL2607: goto _LL2591; _LL2609: goto _LL2591; _LL2611: goto _LL2591; _LL2613:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2683); if( _temp2679 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2679))){({ void* _temp2880[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2880, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2679)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2679))){({ void* _temp2881[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2881, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2679)); goto _LL2591; _LL2615: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2687 !=  0; _temp2687=(( struct Cyc_List_List*)
_check_null( _temp2687))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2687))->hd)){({ struct Cyc_Std_String_pa_struct _temp2883;
_temp2883.tag= Cyc_Std_String_pa; _temp2883.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2687))->hd);{ void* _temp2882[
1u]={& _temp2883}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2882, sizeof( void*), 1u));}});}{
void* _temp2884=( void*)(( struct Cyc_List_List*) _check_null( _temp2687))->hd;
int _temp2896; int _temp2898; void* _temp2900; _LL2886: if( _temp2884 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2887;} else{ goto _LL2888;} _LL2888: if(
_temp2884 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL2889;} else{ goto _LL2890;}
_LL2890: if( _temp2884 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL2891;} else{
goto _LL2892;} _LL2892: if(( unsigned int) _temp2884 >  16u?*(( int*) _temp2884)
==  Cyc_Absyn_Format_att: 0){ _LL2901: _temp2900=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2884)->f1; goto _LL2899; _LL2899: _temp2898=(( struct Cyc_Absyn_Format_att_struct*)
_temp2884)->f2; goto _LL2897; _LL2897: _temp2896=(( struct Cyc_Absyn_Format_att_struct*)
_temp2884)->f3; goto _LL2893;} else{ goto _LL2894;} _LL2894: goto _LL2895;
_LL2887: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2885;
_LL2889: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2885; _LL2891:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2885; _LL2893:
if( ! seen_format){ seen_format= 1; ft= _temp2900; fmt_desc_arg= _temp2898;
fmt_arg_start= _temp2896;} else{({ void* _temp2902[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2902, sizeof( void*), 0u));});} goto _LL2885; _LL2895: goto
_LL2885; _LL2885:;}} if( num_convs >  1){({ void* _temp2903[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2903, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2704);{ struct Cyc_List_List* b=* _temp2704; for( 0; b !=  0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2904=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2910; _LL2906: if(( unsigned int) _temp2904 >  1u?*(( int*) _temp2904)
==  Cyc_Absyn_Eq_constr: 0){ _LL2911: _temp2910=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2904)->f1; if( _temp2910 == ( void*) Cyc_Absyn_MemKind){ goto _LL2907;}
else{ goto _LL2908;}} else{ goto _LL2908;} _LL2908: goto _LL2909; _LL2907:({
struct Cyc_Std_String_pa_struct _temp2913; _temp2913.tag= Cyc_Std_String_pa;
_temp2913.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2912[ 1u]={& _temp2913}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2912, sizeof( void*), 1u));}}); goto
_LL2905; _LL2909: goto _LL2905; _LL2905:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2914=({
struct Cyc_Tcutil_CVTEnv _temp3073; _temp3073.kind_env= cvtenv.kind_env;
_temp3073.free_vars= 0; _temp3073.free_evars= 0; _temp3073.generalize_evars=
cvtenv.generalize_evars; _temp3073.fn_result= 1; _temp3073;}); _temp2914= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2914,( void*) Cyc_Absyn_MemKind, _temp2697); _temp2914.fn_result=
0;{ struct Cyc_List_List* a= _temp2695; for( 0; a !=  0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ _temp2914= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2914,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( a))->hd)).f3);}} if( _temp2691 !=  0){ if( _temp2693){({ void*
_temp2915[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2915, sizeof( void*), 0u));});}{ int _temp2918; void* _temp2920; struct Cyc_Absyn_Tqual
_temp2922; struct Cyc_Core_Opt* _temp2924; struct Cyc_Absyn_VarargInfo _temp2916=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp2691)); _LL2925: _temp2924=
_temp2916.name; goto _LL2923; _LL2923: _temp2922= _temp2916.tq; goto _LL2921;
_LL2921: _temp2920=( void*) _temp2916.type; goto _LL2919; _LL2919: _temp2918=
_temp2916.inject; goto _LL2917; _LL2917: _temp2914= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2914,( void*) Cyc_Absyn_MemKind, _temp2920); if( _temp2918){ void*
_temp2926= Cyc_Tcutil_compress( _temp2920); struct Cyc_Absyn_TunionInfo
_temp2932; void* _temp2934; void* _temp2936; _LL2928: if(( unsigned int)
_temp2926 >  4u?*(( int*) _temp2926) ==  Cyc_Absyn_TunionType: 0){ _LL2933:
_temp2932=(( struct Cyc_Absyn_TunionType_struct*) _temp2926)->f1; _LL2937:
_temp2936=( void*) _temp2932.tunion_info; if(*(( int*) _temp2936) ==  Cyc_Absyn_KnownTunion){
goto _LL2935;} else{ goto _LL2930;} _LL2935: _temp2934=( void*) _temp2932.rgn;
goto _LL2929;} else{ goto _LL2930;} _LL2930: goto _LL2931; _LL2929: goto _LL2927;
_LL2931:({ void* _temp2938[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u), _tag_arr( _temp2938, sizeof( void*), 0u));}); goto
_LL2927; _LL2927:;}}} if( seen_format){ int _temp2939=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2695); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg > 
_temp2939)? 1: fmt_arg_start <  0)? 1:( _temp2691 ==  0? fmt_arg_start !=  0: 0))?
1:( _temp2691 !=  0? fmt_arg_start !=  _temp2939 +  1: 0)){({ void* _temp2940[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2940, sizeof( void*), 0u));});} else{ void* _temp2943;
struct _tuple2 _temp2941=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp2695, fmt_desc_arg -  1); _LL2944: _temp2943= _temp2941.f3;
goto _LL2942; _LL2942:{ void* _temp2945= Cyc_Tcutil_compress( _temp2943); struct
Cyc_Absyn_PtrInfo _temp2951; struct Cyc_Absyn_Conref* _temp2953; void* _temp2955;
_LL2947: if(( unsigned int) _temp2945 >  4u?*(( int*) _temp2945) ==  Cyc_Absyn_PointerType:
0){ _LL2952: _temp2951=(( struct Cyc_Absyn_PointerType_struct*) _temp2945)->f1;
_LL2956: _temp2955=( void*) _temp2951.elt_typ; goto _LL2954; _LL2954: _temp2953=
_temp2951.bounds; goto _LL2948;} else{ goto _LL2949;} _LL2949: goto _LL2950;
_LL2948:{ struct _tuple8 _temp2958=({ struct _tuple8 _temp2957; _temp2957.f1=
Cyc_Tcutil_compress( _temp2955); _temp2957.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2953); _temp2957;}); void* _temp2964; void* _temp2966; void* _temp2968;
void* _temp2970; _LL2960: _LL2967: _temp2966= _temp2958.f1; if(( unsigned int)
_temp2966 >  4u?*(( int*) _temp2966) ==  Cyc_Absyn_IntType: 0){ _LL2971:
_temp2970=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2966)->f1; if(
_temp2970 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2969;} else{ goto _LL2962;}
_LL2969: _temp2968=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2966)->f2;
if( _temp2968 == ( void*) Cyc_Absyn_B1){ goto _LL2965;} else{ goto _LL2962;}}
else{ goto _LL2962;} _LL2965: _temp2964= _temp2958.f2; if( _temp2964 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2961;} else{ goto _LL2962;} _LL2962: goto _LL2963;
_LL2961: goto _LL2959; _LL2963:({ void* _temp2972[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp2972, sizeof( void*), 0u));}); goto _LL2959; _LL2959:;} goto
_LL2946; _LL2950:({ void* _temp2973[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2973, sizeof( void*), 0u));}); goto
_LL2946; _LL2946:;} if( fmt_arg_start !=  0){ void* _temp2974= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2691))->type); int
problem;{ void* _temp2975= ft; _LL2977: if( _temp2975 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL2978;} else{ goto _LL2979;} _LL2979: if( _temp2975 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL2980;} else{ goto _LL2976;} _LL2978:{ void* _temp2981= _temp2974; struct
Cyc_Absyn_TunionInfo _temp2987; void* _temp2989; struct Cyc_Absyn_Tuniondecl**
_temp2991; struct Cyc_Absyn_Tuniondecl* _temp2993; _LL2983: if(( unsigned int)
_temp2981 >  4u?*(( int*) _temp2981) ==  Cyc_Absyn_TunionType: 0){ _LL2988:
_temp2987=(( struct Cyc_Absyn_TunionType_struct*) _temp2981)->f1; _LL2990:
_temp2989=( void*) _temp2987.tunion_info; if(*(( int*) _temp2989) ==  Cyc_Absyn_KnownTunion){
_LL2992: _temp2991=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2989)->f1;
_temp2993=* _temp2991; goto _LL2984;} else{ goto _LL2985;}} else{ goto _LL2985;}
_LL2985: goto _LL2986; _LL2984: problem= Cyc_Absyn_qvar_cmp( _temp2993->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL2982; _LL2986: problem= 1; goto
_LL2982; _LL2982:;} goto _LL2976; _LL2980:{ void* _temp2994= _temp2974; struct
Cyc_Absyn_TunionInfo _temp3000; void* _temp3002; struct Cyc_Absyn_Tuniondecl**
_temp3004; struct Cyc_Absyn_Tuniondecl* _temp3006; _LL2996: if(( unsigned int)
_temp2994 >  4u?*(( int*) _temp2994) ==  Cyc_Absyn_TunionType: 0){ _LL3001:
_temp3000=(( struct Cyc_Absyn_TunionType_struct*) _temp2994)->f1; _LL3003:
_temp3002=( void*) _temp3000.tunion_info; if(*(( int*) _temp3002) ==  Cyc_Absyn_KnownTunion){
_LL3005: _temp3004=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3002)->f1;
_temp3006=* _temp3004; goto _LL2997;} else{ goto _LL2998;}} else{ goto _LL2998;}
_LL2998: goto _LL2999; _LL2997: problem= Cyc_Absyn_qvar_cmp( _temp3006->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL2995; _LL2999: problem= 1; goto
_LL2995; _LL2995:;} goto _LL2976; _LL2976:;} if( problem){({ void* _temp3007[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3007, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2689; for( 0; rpo !=  0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp3010; void* _temp3011; void*
_temp3013; struct _tuple8* _temp3008=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp3010=* _temp3008; _LL3014: _temp3013= _temp3010.f1;
goto _LL3012; _LL3012: _temp3011= _temp3010.f2; goto _LL3009; _LL3009: _temp2914=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2914,( void*) Cyc_Absyn_RgnKind,
_temp3013); _temp2914= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2914,( void*)
Cyc_Absyn_RgnKind, _temp3011);}} if(* _temp2701 !=  0){ _temp2914= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2914,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2701))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2914.free_vars; for(
0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void*
_temp3015=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3023; void*
_temp3025; _LL3017: if(( unsigned int) _temp3015 >  1u?*(( int*) _temp3015) == 
Cyc_Absyn_Eq_constr: 0){ _LL3024: _temp3023=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3015)->f1; if( _temp3023 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3018;}
else{ goto _LL3019;}} else{ goto _LL3019;} _LL3019: if(( unsigned int) _temp3015
>  1u?*(( int*) _temp3015) ==  Cyc_Absyn_Eq_constr: 0){ _LL3026: _temp3025=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3015)->f1; if( _temp3025 == (
void*) Cyc_Absyn_EffKind){ goto _LL3020;} else{ goto _LL3021;}} else{ goto
_LL3021;} _LL3021: goto _LL3022; _LL3018: effect=({ struct Cyc_List_List*
_temp3027=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3027->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3028=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3028[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3029; _temp3029.tag= Cyc_Absyn_AccessEff;
_temp3029.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3030=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3030[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3031; _temp3031.tag= Cyc_Absyn_VarType;
_temp3031.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3031;}); _temp3030;})); _temp3029;}); _temp3028;})); _temp3027->tl= effect;
_temp3027;}); goto _LL3016; _LL3020: effect=({ struct Cyc_List_List* _temp3032=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3032->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3033=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3033[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3034; _temp3034.tag= Cyc_Absyn_VarType; _temp3034.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3034;}); _temp3033;})); _temp3032->tl= effect; _temp3032;}); goto _LL3016;
_LL3022: effect=({ struct Cyc_List_List* _temp3035=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3035->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3036=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3036[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3037; _temp3037.tag= Cyc_Absyn_RgnsEff; _temp3037.f1=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3038=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3038[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3039; _temp3039.tag= Cyc_Absyn_VarType; _temp3039.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3039;}); _temp3038;})); _temp3037;}); _temp3036;})); _temp3035->tl= effect;
_temp3035;}); goto _LL3016; _LL3016:;}} effect= Cyc_List_imp_rev( effect);{
struct Cyc_List_List* ts= _temp2914.free_evars; for( 0; ts !=  0; ts=(( struct
Cyc_List_List*) _check_null( ts))->tl){ void* _temp3040= Cyc_Tcutil_typ_kind((
void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _LL3042: if( _temp3040
== ( void*) Cyc_Absyn_RgnKind){ goto _LL3043;} else{ goto _LL3044;} _LL3044: if(
_temp3040 == ( void*) Cyc_Absyn_EffKind){ goto _LL3045;} else{ goto _LL3046;}
_LL3046: goto _LL3047; _LL3043: effect=({ struct Cyc_List_List* _temp3048=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3048->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3049=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3049[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3050; _temp3050.tag= Cyc_Absyn_AccessEff;
_temp3050.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3050;}); _temp3049;})); _temp3048->tl= effect; _temp3048;}); goto _LL3041;
_LL3045: effect=({ struct Cyc_List_List* _temp3051=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3051->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3051->tl= effect; _temp3051;});
goto _LL3041; _LL3047: effect=({ struct Cyc_List_List* _temp3052=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3052->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3053=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3053[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3054; _temp3054.tag= Cyc_Absyn_RgnsEff; _temp3054.f1=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _temp3054;});
_temp3053;})); _temp3052->tl= effect; _temp3052;}); goto _LL3041; _LL3041:;}}*
_temp2701=({ struct Cyc_Core_Opt* _temp3055=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3055->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3056=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3056[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3057; _temp3057.tag= Cyc_Absyn_JoinEff;
_temp3057.f1= Cyc_List_imp_rev( effect); _temp3057;}); _temp3056;})); _temp3055;});}}
if(* _temp2704 !=  0){ struct Cyc_List_List* bs=* _temp2704; for( 0; bs !=  0;
bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3058= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3059=( void*) _temp3058->v; void*
_temp3067; _LL3061: if( _temp3059 == ( void*) Cyc_Absyn_No_constr){ goto _LL3062;}
else{ goto _LL3063;} _LL3063: if(( unsigned int) _temp3059 >  1u?*(( int*)
_temp3059) ==  Cyc_Absyn_Eq_constr: 0){ _LL3068: _temp3067=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3059)->f1; if( _temp3067 == ( void*) Cyc_Absyn_MemKind){ goto _LL3064;}
else{ goto _LL3065;}} else{ goto _LL3065;} _LL3065: goto _LL3066; _LL3062:({
struct Cyc_Std_String_pa_struct _temp3070; _temp3070.tag= Cyc_Std_String_pa;
_temp3070.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp3069[ 1u]={& _temp3070}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3069, sizeof( void*), 1u));}}); goto
_LL3064; _LL3064:( void*)( _temp3058->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3071=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3071[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3072; _temp3072.tag= Cyc_Absyn_Eq_constr;
_temp3072.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3072;}); _temp3071;})));
goto _LL3060; _LL3066: goto _LL3060; _LL3060:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2914.kind_env,* _temp2704); _temp2914.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2914.free_vars,* _temp2704);{ struct Cyc_List_List* tvs= _temp2914.free_vars;
for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2914.free_evars; for( 0; evs !=  0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2591;}}
_LL2617: for( 0; _temp2705 !=  0; _temp2705=(( struct Cyc_List_List*)
_check_null( _temp2705))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp2705))->hd)).f2);} goto _LL2591; _LL2619:{ struct
_RegionHandle _temp3074= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp3074; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2707 !=  0; _temp2707=(( struct Cyc_List_List*) _check_null( _temp2707))->tl){
struct Cyc_Absyn_Structfield _temp3077; struct Cyc_List_List* _temp3078; struct
Cyc_Absyn_Exp* _temp3080; void* _temp3082; struct Cyc_Absyn_Tqual _temp3084;
struct _tagged_arr* _temp3086; struct Cyc_Absyn_Structfield* _temp3075=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2707))->hd;
_temp3077=* _temp3075; _LL3087: _temp3086= _temp3077.name; goto _LL3085; _LL3085:
_temp3084= _temp3077.tq; goto _LL3083; _LL3083: _temp3082=( void*) _temp3077.type;
goto _LL3081; _LL3081: _temp3080= _temp3077.width; goto _LL3079; _LL3079:
_temp3078= _temp3077.attributes; goto _LL3076; _LL3076: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3086)){({ struct Cyc_Std_String_pa_struct _temp3089; _temp3089.tag= Cyc_Std_String_pa;
_temp3089.f1=( struct _tagged_arr)* _temp3086;{ void* _temp3088[ 1u]={&
_temp3089}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3088, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3086, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3090=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3090->hd=( void*)
_temp3086; _temp3090->tl= prev_fields; _temp3090;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3082); Cyc_Tcutil_check_bitfield(
loc, te, _temp3082, _temp3080, _temp3086); Cyc_Tcutil_check_field_atts( loc,
_temp3086, _temp3078);}}; _pop_region( sprev_rgn);} goto _LL2591; _LL2621:{
struct _RegionHandle _temp3091= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3091; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2709 !=  0; _temp2709=(( struct Cyc_List_List*) _check_null( _temp2709))->tl){
struct Cyc_Absyn_Structfield _temp3094; struct Cyc_List_List* _temp3095; struct
Cyc_Absyn_Exp* _temp3097; void* _temp3099; struct Cyc_Absyn_Tqual _temp3101;
struct _tagged_arr* _temp3103; struct Cyc_Absyn_Structfield* _temp3092=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2709))->hd;
_temp3094=* _temp3092; _LL3104: _temp3103= _temp3094.name; goto _LL3102; _LL3102:
_temp3101= _temp3094.tq; goto _LL3100; _LL3100: _temp3099=( void*) _temp3094.type;
goto _LL3098; _LL3098: _temp3097= _temp3094.width; goto _LL3096; _LL3096:
_temp3095= _temp3094.attributes; goto _LL3093; _LL3093: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3103)){({ struct Cyc_Std_String_pa_struct _temp3106; _temp3106.tag= Cyc_Std_String_pa;
_temp3106.f1=( struct _tagged_arr)* _temp3103;{ void* _temp3105[ 1u]={&
_temp3106}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3105, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3103, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3107=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3107->hd=( void*)
_temp3103; _temp3107->tl= prev_fields; _temp3107;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3099); if( ! Cyc_Tcutil_bits_only(
_temp3099)){({ struct Cyc_Std_String_pa_struct _temp3109; _temp3109.tag= Cyc_Std_String_pa;
_temp3109.f1=( struct _tagged_arr)* _temp3103;{ void* _temp3108[ 1u]={&
_temp3109}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3108, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3099, _temp3097, _temp3103); Cyc_Tcutil_check_field_atts(
loc, _temp3103, _temp3095);}}; _pop_region( uprev_rgn);} goto _LL2591; _LL2623:
if( _temp2716 ==  0){ if(* _temp2713 ==  0){({ void* _temp3110[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3110, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2713));;}} if(*
_temp2713 ==  0){ struct _handler_cons _temp3111; _push_handler(& _temp3111);{
int _temp3113= 0; if( setjmp( _temp3111.handler)){ _temp3113= 1;} if( !
_temp3113){* _temp2713=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2716));; _pop_handler();} else{
void* _temp3112=( void*) _exn_thrown; void* _temp3115= _temp3112; _LL3117: if(
_temp3115 ==  Cyc_Dict_Absent){ goto _LL3118;} else{ goto _LL3119;} _LL3119:
goto _LL3120; _LL3118: { struct _tuple1* tdn=( struct _tuple1*) _check_null(
_temp2716); struct Cyc_Tcenv_Genv* _temp3121=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Structdecl* _temp3122=({ struct Cyc_Absyn_Structdecl* _temp3125=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp3125->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3125->name=({ struct Cyc_Core_Opt*
_temp3126=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3126->v=( void*) tdn; _temp3126;}); _temp3125->tvs= 0; _temp3125->fields= 0;
_temp3125->attributes= 0; _temp3125;}); Cyc_Tc_tcStructdecl( te, _temp3121, loc,
_temp3122);* _temp2713=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc, tdn); if( _temp2714 !=  0){({ struct Cyc_Std_String_pa_struct _temp3124;
_temp3124.tag= Cyc_Std_String_pa; _temp3124.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tdn);{ void* _temp3123[ 1u]={& _temp3124}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized struct %s before using",
sizeof( unsigned char), 52u), _tag_arr( _temp3123, sizeof( void*), 1u));}});
return cvtenv;} goto _LL3116;} _LL3120:( void) _throw( _temp3115); _LL3116:;}}}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp2713)); if( sd->name !=  0){*(( struct _tuple1*) _check_null( _temp2716))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2714); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3130; _temp3130.tag= Cyc_Std_Int_pa; _temp3130.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3129; _temp3129.tag= Cyc_Std_Int_pa;
_temp3129.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3128; _temp3128.tag= Cyc_Std_String_pa; _temp3128.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2716));{ void*
_temp3127[ 3u]={& _temp3128,& _temp3129,& _temp3130}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3127, sizeof( void*), 3u));}}}});} for( 0;
_temp2714 !=  0;( _temp2714=(( struct Cyc_List_List*) _check_null( _temp2714))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2714))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2591;}}
_LL2625: if( _temp2723 ==  0){({ void* _temp3131[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp3131,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2720 ==  0){ struct
_handler_cons _temp3132; _push_handler(& _temp3132);{ int _temp3134= 0; if(
setjmp( _temp3132.handler)){ _temp3134= 1;} if( ! _temp3134){* _temp2720=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2723));; _pop_handler();} else{ void* _temp3133=(
void*) _exn_thrown; void* _temp3136= _temp3133; _LL3138: if( _temp3136 ==  Cyc_Dict_Absent){
goto _LL3139;} else{ goto _LL3140;} _LL3140: goto _LL3141; _LL3139: { struct
_tuple1* _temp3142=( struct _tuple1*) _check_null( _temp2723); struct Cyc_Tcenv_Genv*
_temp3143=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Uniondecl* _temp3144=({
struct Cyc_Absyn_Uniondecl* _temp3147=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp3147->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp3147->name=({ struct Cyc_Core_Opt* _temp3148=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3148->v=( void*) _temp3142;
_temp3148;}); _temp3147->tvs= 0; _temp3147->fields= 0; _temp3147->attributes= 0;
_temp3147;}); Cyc_Tc_tcUniondecl( te, _temp3143, loc, _temp3144);* _temp2720=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc, _temp3142);
if( _temp2721 !=  0){({ struct Cyc_Std_String_pa_struct _temp3146; _temp3146.tag=
Cyc_Std_String_pa; _temp3146.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3142);{ void* _temp3145[ 1u]={& _temp3146}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized union %s before using",
sizeof( unsigned char), 51u), _tag_arr( _temp3145, sizeof( void*), 1u));}});
return cvtenv;} goto _LL3137;} _LL3141:( void) _throw( _temp3136); _LL3137:;}}}{
struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp2720)); if( ud->name !=  0){*(( struct _tuple1*) _check_null( _temp2723))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2721); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3152; _temp3152.tag= Cyc_Std_Int_pa; _temp3152.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3151; _temp3151.tag= Cyc_Std_Int_pa;
_temp3151.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3150; _temp3150.tag= Cyc_Std_String_pa; _temp3150.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2723));{ void*
_temp3149[ 3u]={& _temp3150,& _temp3151,& _temp3152}; Cyc_Tcutil_terr( loc,
_tag_arr("union %s expects %d type arguments but was given %d", sizeof(
unsigned char), 52u), _tag_arr( _temp3149, sizeof( void*), 3u));}}}});} for( 0;
_temp2721 !=  0;( _temp2721=(( struct Cyc_List_List*) _check_null( _temp2721))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2721))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2591;}}
_LL2627: { struct Cyc_List_List* targs=* _temp2730; struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3153; _push_handler(& _temp3153);{ int _temp3155=
0; if( setjmp( _temp3153.handler)){ _temp3155= 1;} if( ! _temp3155){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2731);; _pop_handler();} else{ void* _temp3154=( void*)
_exn_thrown; void* _temp3157= _temp3154; _LL3159: if( _temp3157 ==  Cyc_Dict_Absent){
goto _LL3160;} else{ goto _LL3161;} _LL3161: goto _LL3162; _LL3160:({ struct Cyc_Std_String_pa_struct
_temp3164; _temp3164.tag= Cyc_Std_String_pa; _temp3164.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2731);{ void* _temp3163[ 1u]={& _temp3164}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3163, sizeof( void*), 1u));}}); return cvtenv; _LL3162:( void) _throw(
_temp3157); _LL3158:;}}} _temp2731[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3165=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3165->hd=( void*)({ struct
_tuple5* _temp3166=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3166->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3166->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3166;}); _temp3165->tl= inst; _temp3165;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp3168; _temp3168.tag= Cyc_Std_String_pa; _temp3168.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2731);{ void* _temp3167[ 1u]={& _temp3168}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3167, sizeof( void*), 1u));}});} if( tvs !=  0){ struct Cyc_List_List*
hidden_ts= 0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({
struct Cyc_List_List* _temp3169=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3169->hd=( void*) e; _temp3169->tl= hidden_ts;
_temp3169;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k, e);
inst=({ struct Cyc_List_List* _temp3170=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3170->hd=( void*)({ struct _tuple5*
_temp3171=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp3171->f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3171->f2= e; _temp3171;}); _temp3170->tl= inst; _temp3170;});}* _temp2730=
Cyc_List_imp_append( targs, Cyc_List_imp_rev( hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute(
inst,( void*) td->defn);* _temp2727=({ struct Cyc_Core_Opt* _temp3172=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3172->v=( void*)
new_typ; _temp3172;}); goto _LL2591;}}} _LL2629: goto _LL2591; _LL2631:
_temp2735= _temp2733; goto _LL2633; _LL2633: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2735); goto _LL2591; _LL2635:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2737); goto _LL2591; _LL2637: for( 0; _temp2739 !=  0; _temp2739=(( struct
Cyc_List_List*) _check_null( _temp2739))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2739))->hd);} goto _LL2591; _LL2591:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){({ struct Cyc_Std_String_pa_struct
_temp3176; _temp3176.tag= Cyc_Std_String_pa; _temp3176.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( expected_kind);{ struct Cyc_Std_String_pa_struct
_temp3175; _temp3175.tag= Cyc_Std_String_pa; _temp3175.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ struct Cyc_Std_String_pa_struct
_temp3174; _temp3174.tag= Cyc_Std_String_pa; _temp3174.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp3173[ 3u]={& _temp3174,& _temp3175,&
_temp3176}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3173, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3177= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3192; _temp3192.kind_env= kind_env; _temp3192.free_vars= 0; _temp3192.free_evars=
0; _temp3192.generalize_evars= generalize_evars; _temp3192.fn_result= 0;
_temp3192;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3177.free_vars;
for( 0; vs !=  0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ _temp3177.kind_env=
Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( vs))->hd);}}{ struct Cyc_List_List* evs= _temp3177.free_evars; for(
0; evs !=  0; evs=(( struct Cyc_List_List*) _check_null( evs))->tl){ void*
_temp3178= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null(
evs))->hd); struct Cyc_Core_Opt* _temp3184; struct Cyc_Core_Opt** _temp3186;
_LL3180: if(( unsigned int) _temp3178 >  4u?*(( int*) _temp3178) ==  Cyc_Absyn_Evar:
0){ _LL3185: _temp3184=(( struct Cyc_Absyn_Evar_struct*) _temp3178)->f4;
_temp3186=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3178)->f4;
goto _LL3181;} else{ goto _LL3182;} _LL3182: goto _LL3183; _LL3181: if(*
_temp3186 ==  0){* _temp3186=({ struct Cyc_Core_Opt* _temp3187=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3187->v=( void*) kind_env;
_temp3187;});} else{ struct Cyc_List_List* _temp3188=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3186))->v; struct Cyc_List_List*
_temp3189= 0; for( 0; _temp3188 !=  0; _temp3188=(( struct Cyc_List_List*)
_check_null( _temp3188))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3188))->hd)){ _temp3189=({ struct Cyc_List_List* _temp3190=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3190->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3188))->hd);
_temp3190->tl= _temp3189; _temp3190;});}}* _temp3186=({ struct Cyc_Core_Opt*
_temp3191=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3191->v=( void*) _temp3189; _temp3191;});} goto _LL3179; _LL3183: goto
_LL3179; _LL3179:;}} return _temp3177;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ int
generalize_evars= Cyc_Tcutil_is_function_type( t); struct Cyc_Tcutil_CVTEnv
_temp3193= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
generalize_evars, t); struct Cyc_List_List* _temp3194= _temp3193.free_vars;
struct Cyc_List_List* _temp3195= _temp3193.free_evars;{ struct Cyc_List_List* x=
_temp3194; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3196=( void*) c->v;
void* _temp3204; _LL3198: if( _temp3196 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3199;} else{ goto _LL3200;} _LL3200: if(( unsigned int) _temp3196 >  1u?*((
int*) _temp3196) ==  Cyc_Absyn_Eq_constr: 0){ _LL3205: _temp3204=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3196)->f1; if( _temp3204 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3201;} else{ goto _LL3202;}} else{ goto _LL3202;}
_LL3202: goto _LL3203; _LL3199: goto _LL3201; _LL3201:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3206=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3206[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3207; _temp3207.tag= Cyc_Absyn_Eq_constr;
_temp3207.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3207;}); _temp3206;})));
goto _LL3197; _LL3203: goto _LL3197; _LL3197:;}} if( _temp3194 !=  0? 1:
_temp3195 !=  0){{ void* _temp3208= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3214; struct Cyc_List_List* _temp3216; struct Cyc_List_List* _temp3218;
struct Cyc_Absyn_VarargInfo* _temp3220; int _temp3222; struct Cyc_List_List*
_temp3224; void* _temp3226; struct Cyc_Core_Opt* _temp3228; struct Cyc_List_List*
_temp3230; struct Cyc_List_List** _temp3232; _LL3210: if(( unsigned int)
_temp3208 >  4u?*(( int*) _temp3208) ==  Cyc_Absyn_FnType: 0){ _LL3215:
_temp3214=(( struct Cyc_Absyn_FnType_struct*) _temp3208)->f1; _LL3231: _temp3230=
_temp3214.tvars; _temp3232=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3208)->f1).tvars; goto _LL3229; _LL3229: _temp3228= _temp3214.effect; goto
_LL3227; _LL3227: _temp3226=( void*) _temp3214.ret_typ; goto _LL3225; _LL3225:
_temp3224= _temp3214.args; goto _LL3223; _LL3223: _temp3222= _temp3214.c_varargs;
goto _LL3221; _LL3221: _temp3220= _temp3214.cyc_varargs; goto _LL3219; _LL3219:
_temp3218= _temp3214.rgn_po; goto _LL3217; _LL3217: _temp3216= _temp3214.attributes;
goto _LL3211;} else{ goto _LL3212;} _LL3212: goto _LL3213; _LL3211: if(*
_temp3232 ==  0){* _temp3232= _temp3194; _temp3194= 0;} goto _LL3209; _LL3213:
goto _LL3209; _LL3209:;} if( _temp3194 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3234; _temp3234.tag= Cyc_Std_String_pa; _temp3234.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3194))->hd)->name;{
void* _temp3233[ 1u]={& _temp3234}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp3233, sizeof( void*), 1u));}});}
if( _temp3195 !=  0){ for( 0; _temp3195 !=  0; _temp3195=(( struct Cyc_List_List*)
_check_null( _temp3195))->tl){ void* e=( void*)(( struct Cyc_List_List*)
_check_null( _temp3195))->hd; void* _temp3235= Cyc_Tcutil_typ_kind( e); _LL3237:
if( _temp3235 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3238;} else{ goto _LL3239;}
_LL3239: if( _temp3235 == ( void*) Cyc_Absyn_EffKind){ goto _LL3240;} else{ goto
_LL3241;} _LL3241: goto _LL3242; _LL3238: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp3243[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp3243, sizeof( void*), 0u));});} goto _LL3236; _LL3240: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp3244[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp3244, sizeof( void*), 0u));});} goto _LL3236; _LL3242:({ struct Cyc_Std_String_pa_struct
_temp3247; _temp3247.tag= Cyc_Std_String_pa; _temp3247.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3246;
_temp3246.tag= Cyc_Std_String_pa; _temp3246.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp3245[ 2u]={& _temp3246,& _temp3247}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3245, sizeof( void*), 2u));}}}); goto
_LL3236; _LL3236:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3248= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3254; struct Cyc_List_List* _temp3256; struct
Cyc_List_List* _temp3258; struct Cyc_Absyn_VarargInfo* _temp3260; int _temp3262;
struct Cyc_List_List* _temp3264; void* _temp3266; struct Cyc_Core_Opt* _temp3268;
struct Cyc_List_List* _temp3270; _LL3250: if(( unsigned int) _temp3248 >  4u?*((
int*) _temp3248) ==  Cyc_Absyn_FnType: 0){ _LL3255: _temp3254=(( struct Cyc_Absyn_FnType_struct*)
_temp3248)->f1; _LL3271: _temp3270= _temp3254.tvars; goto _LL3269; _LL3269:
_temp3268= _temp3254.effect; goto _LL3267; _LL3267: _temp3266=( void*) _temp3254.ret_typ;
goto _LL3265; _LL3265: _temp3264= _temp3254.args; goto _LL3263; _LL3263:
_temp3262= _temp3254.c_varargs; goto _LL3261; _LL3261: _temp3260= _temp3254.cyc_varargs;
goto _LL3259; _LL3259: _temp3258= _temp3254.rgn_po; goto _LL3257; _LL3257:
_temp3256= _temp3254.attributes; goto _LL3251;} else{ goto _LL3252;} _LL3252:
goto _LL3253; _LL3251: fd->tvs= _temp3270; fd->effect= _temp3268; goto _LL3249;
_LL3253:({ void* _temp3272[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3272, sizeof( void*), 0u));});
return; _LL3249:;}{ struct _RegionHandle _temp3273= _new_region(); struct
_RegionHandle* r=& _temp3273; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3274=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3274->v=(
void*) t; _temp3274;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3275=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3276= Cyc_Tcutil_remove_bound_tvars( _temp3275.free_vars,
bound_tvars); struct Cyc_List_List* _temp3277= _temp3275.free_evars;{ struct Cyc_List_List*
fs= _temp3276; for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3278=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name;({ struct Cyc_Std_String_pa_struct _temp3281;
_temp3281.tag= Cyc_Std_String_pa; _temp3281.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp3280; _temp3280.tag= Cyc_Std_String_pa;
_temp3280.f1=( struct _tagged_arr)* _temp3278;{ void* _temp3279[ 2u]={&
_temp3280,& _temp3281}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3279, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3277 !=  0: 0){ for( 0; _temp3277 !=  0; _temp3277=((
struct Cyc_List_List*) _check_null( _temp3277))->tl){ void* e=( void*)(( struct
Cyc_List_List*) _check_null( _temp3277))->hd; void* _temp3282= Cyc_Tcutil_typ_kind(
e); _LL3284: if( _temp3282 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3285;} else{
goto _LL3286;} _LL3286: if( _temp3282 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3287;} else{ goto _LL3288;} _LL3288: goto _LL3289; _LL3285: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3290[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3290, sizeof( void*), 0u));});}
goto _LL3283; _LL3287: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3291=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3291[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3292; _temp3292.tag= Cyc_Absyn_JoinEff;
_temp3292.f1= 0; _temp3292;}); _temp3291;}))){({ void* _temp3293[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3293, sizeof( void*), 0u));});}
goto _LL3283; _LL3289:({ struct Cyc_Std_String_pa_struct _temp3296; _temp3296.tag=
Cyc_Std_String_pa; _temp3296.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3295; _temp3295.tag= Cyc_Std_String_pa;
_temp3295.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3294[
2u]={& _temp3295,& _temp3296}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3294, sizeof( void*), 2u));}}});
goto _LL3283; _LL3283:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ struct Cyc_List_List* vs2=(( struct Cyc_List_List*)
_check_null( vs))->tl; for( 0; vs2 !=  0; vs2=(( struct Cyc_List_List*)
_check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*) _check_null(
vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd) ==  0){({
struct Cyc_Std_String_pa_struct _temp3299; _temp3299.tag= Cyc_Std_String_pa;
_temp3299.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_Std_String_pa_struct _temp3298; _temp3298.tag=
Cyc_Std_String_pa; _temp3298.f1=( struct _tagged_arr) msg;{ void* _temp3297[ 2u]={&
_temp3298,& _temp3299}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3297, sizeof( void*), 2u));}}});}}}} static
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
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields !=  0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_Std_strcmp(*(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd)->name, _tag_arr("", sizeof(
unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List* _temp3300=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3300->hd=( void*)({
struct _tuple17* _temp3301=( struct _tuple17*) _cycalloc( sizeof( struct
_tuple17)); _temp3301->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3301->f2= 0; _temp3301;}); _temp3300->tl=
fields; _temp3300;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3304;
void* _temp3305; struct Cyc_List_List* _temp3307; struct _tuple18* _temp3302=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3304=*
_temp3302; _LL3308: _temp3307= _temp3304.f1; goto _LL3306; _LL3306: _temp3305=
_temp3304.f2; goto _LL3303; _LL3303: if( _temp3307 ==  0){ struct Cyc_List_List*
_temp3309= fields; for( 0; _temp3309 !=  0; _temp3309=(( struct Cyc_List_List*)
_check_null( _temp3309))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3309))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3309))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3310=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3310->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3311=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3311[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3312; _temp3312.tag= Cyc_Absyn_FieldName;
_temp3312.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3309))->hd)).f1)->name; _temp3312;}); _temp3311;})); _temp3310->tl= 0;
_temp3310;}); ans=({ struct Cyc_List_List* _temp3313=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3313->hd=( void*)({
struct _tuple19* _temp3314=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3314->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3309))->hd)).f1; _temp3314->f2= _temp3305; _temp3314;});
_temp3313->tl= ans; _temp3313;}); break;}} if( _temp3309 ==  0){({ void*
_temp3315[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3315, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3307))->tl !=  0){({ void*
_temp3316[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3316, sizeof( void*), 0u));});}
else{ void* _temp3317=( void*)(( struct Cyc_List_List*) _check_null( _temp3307))->hd;
struct _tagged_arr* _temp3323; _LL3319: if(*(( int*) _temp3317) ==  Cyc_Absyn_ArrayElement){
goto _LL3320;} else{ goto _LL3321;} _LL3321: if(*(( int*) _temp3317) ==  Cyc_Absyn_FieldName){
_LL3324: _temp3323=(( struct Cyc_Absyn_FieldName_struct*) _temp3317)->f1; goto
_LL3322;} else{ goto _LL3318;} _LL3320:({ void* _temp3325[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3325, sizeof( void*), 0u));}); goto _LL3318;
_LL3322: { struct Cyc_List_List* _temp3326= fields; for( 0; _temp3326 !=  0;
_temp3326=(( struct Cyc_List_List*) _check_null( _temp3326))->tl){ if( Cyc_Std_zstrptrcmp(
_temp3323,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3326))->hd)).f1)->name) ==  0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3326))->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3328;
_temp3328.tag= Cyc_Std_String_pa; _temp3328.f1=( struct _tagged_arr)* _temp3323;{
void* _temp3327[ 1u]={& _temp3328}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3327, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3326))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3329=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3329->hd=( void*)({ struct _tuple19*
_temp3330=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3330->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3326))->hd)).f1; _temp3330->f2= _temp3305; _temp3330;}); _temp3329->tl= ans;
_temp3329;}); break;}} if( _temp3326 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3332; _temp3332.tag= Cyc_Std_String_pa; _temp3332.f1=( struct _tagged_arr)*
_temp3323;{ void* _temp3331[ 1u]={& _temp3332}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3331, sizeof( void*), 1u));}});}
goto _LL3318;} _LL3318:;}}} for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3333[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3333, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3334= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3340; struct Cyc_Absyn_Conref* _temp3342; void* _temp3344;
_LL3336: if(( unsigned int) _temp3334 >  4u?*(( int*) _temp3334) ==  Cyc_Absyn_PointerType:
0){ _LL3341: _temp3340=(( struct Cyc_Absyn_PointerType_struct*) _temp3334)->f1;
_LL3345: _temp3344=( void*) _temp3340.elt_typ; goto _LL3343; _LL3343: _temp3342=
_temp3340.bounds; goto _LL3337;} else{ goto _LL3338;} _LL3338: goto _LL3339;
_LL3337: { struct Cyc_Absyn_Conref* _temp3346= Cyc_Absyn_compress_conref(
_temp3342); void* _temp3347=( void*)( Cyc_Absyn_compress_conref( _temp3346))->v;
void* _temp3355; _LL3349: if(( unsigned int) _temp3347 >  1u?*(( int*) _temp3347)
==  Cyc_Absyn_Eq_constr: 0){ _LL3356: _temp3355=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3347)->f1; if( _temp3355 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3350;}
else{ goto _LL3351;}} else{ goto _LL3351;} _LL3351: if( _temp3347 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3352;} else{ goto _LL3353;} _LL3353: goto _LL3354;
_LL3350:* elt_typ_dest= _temp3344; return 1; _LL3352:( void*)( _temp3346->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3357=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3357[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3358; _temp3358.tag= Cyc_Absyn_Eq_constr;
_temp3358.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3358;}); _temp3357;})));*
elt_typ_dest= _temp3344; return 1; _LL3354: return 0; _LL3348:;} _LL3339: return
0; _LL3335:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3541;
_temp3541.f1= 0; _temp3541.f2=( void*) Cyc_Absyn_HeapRgn; _temp3541;}); void*
_temp3359=( void*) e->r; void* _temp3373; struct _tuple1* _temp3375; struct
_tagged_arr* _temp3377; struct Cyc_Absyn_Exp* _temp3379; struct _tagged_arr*
_temp3381; struct Cyc_Absyn_Exp* _temp3383; struct Cyc_Absyn_Exp* _temp3385;
struct Cyc_Absyn_Exp* _temp3387; struct Cyc_Absyn_Exp* _temp3389; _LL3361: if(*((
int*) _temp3359) ==  Cyc_Absyn_Var_e){ _LL3376: _temp3375=(( struct Cyc_Absyn_Var_e_struct*)
_temp3359)->f1; goto _LL3374; _LL3374: _temp3373=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3359)->f2; goto _LL3362;} else{ goto _LL3363;} _LL3363: if(*(( int*)
_temp3359) ==  Cyc_Absyn_StructMember_e){ _LL3380: _temp3379=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3359)->f1; goto _LL3378; _LL3378: _temp3377=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3359)->f2; goto _LL3364;} else{ goto _LL3365;} _LL3365: if(*(( int*)
_temp3359) ==  Cyc_Absyn_StructArrow_e){ _LL3384: _temp3383=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3359)->f1; goto _LL3382; _LL3382: _temp3381=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3359)->f2; goto _LL3366;} else{ goto _LL3367;} _LL3367: if(*(( int*)
_temp3359) ==  Cyc_Absyn_Deref_e){ _LL3386: _temp3385=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3359)->f1; goto _LL3368;} else{ goto _LL3369;} _LL3369: if(*(( int*)
_temp3359) ==  Cyc_Absyn_Subscript_e){ _LL3390: _temp3389=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3359)->f1; goto _LL3388; _LL3388: _temp3387=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3359)->f2; goto _LL3370;} else{ goto _LL3371;} _LL3371: goto _LL3372;
_LL3362: { void* _temp3391= _temp3373; struct Cyc_Absyn_Vardecl* _temp3405;
struct Cyc_Absyn_Vardecl* _temp3407; struct Cyc_Absyn_Vardecl* _temp3409; struct
Cyc_Absyn_Vardecl* _temp3411; _LL3393: if( _temp3391 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3394;} else{ goto _LL3395;} _LL3395: if(( unsigned int) _temp3391 >  1u?*((
int*) _temp3391) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3396;} else{ goto _LL3397;}
_LL3397: if(( unsigned int) _temp3391 >  1u?*(( int*) _temp3391) ==  Cyc_Absyn_Global_b:
0){ _LL3406: _temp3405=(( struct Cyc_Absyn_Global_b_struct*) _temp3391)->f1;
goto _LL3398;} else{ goto _LL3399;} _LL3399: if(( unsigned int) _temp3391 >  1u?*((
int*) _temp3391) ==  Cyc_Absyn_Local_b: 0){ _LL3408: _temp3407=(( struct Cyc_Absyn_Local_b_struct*)
_temp3391)->f1; goto _LL3400;} else{ goto _LL3401;} _LL3401: if(( unsigned int)
_temp3391 >  1u?*(( int*) _temp3391) ==  Cyc_Absyn_Pat_b: 0){ _LL3410: _temp3409=((
struct Cyc_Absyn_Pat_b_struct*) _temp3391)->f1; goto _LL3402;} else{ goto
_LL3403;} _LL3403: if(( unsigned int) _temp3391 >  1u?*(( int*) _temp3391) == 
Cyc_Absyn_Param_b: 0){ _LL3412: _temp3411=(( struct Cyc_Absyn_Param_b_struct*)
_temp3391)->f1; goto _LL3404;} else{ goto _LL3392;} _LL3394: return bogus_ans;
_LL3396: return({ struct _tuple7 _temp3413; _temp3413.f1= 0; _temp3413.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3413;}); _LL3398: { void* _temp3414= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3416: if((
unsigned int) _temp3414 >  4u?*(( int*) _temp3414) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3417;} else{ goto _LL3418;} _LL3418: goto _LL3419; _LL3417: return({
struct _tuple7 _temp3420; _temp3420.f1= 1; _temp3420.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3420;}); _LL3419: return({ struct _tuple7 _temp3421; _temp3421.f1=(
_temp3405->tq).q_const; _temp3421.f2=( void*) Cyc_Absyn_HeapRgn; _temp3421;});
_LL3415:;} _LL3400: { void* _temp3422= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3424: if(( unsigned int) _temp3422 >  4u?*(( int*)
_temp3422) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3425;} else{ goto _LL3426;}
_LL3426: goto _LL3427; _LL3425: return({ struct _tuple7 _temp3428; _temp3428.f1=
1; _temp3428.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3407->rgn))->v;
_temp3428;}); _LL3427: return({ struct _tuple7 _temp3429; _temp3429.f1=(
_temp3407->tq).q_const; _temp3429.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3407->rgn))->v; _temp3429;}); _LL3423:;} _LL3402: _temp3411=
_temp3409; goto _LL3404; _LL3404: return({ struct _tuple7 _temp3430; _temp3430.f1=(
_temp3411->tq).q_const; _temp3430.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3411->rgn))->v; _temp3430;}); _LL3392:;} _LL3364: { void*
_temp3431= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3379->topt))->v); struct Cyc_List_List* _temp3443; struct Cyc_List_List*
_temp3445; struct Cyc_Absyn_Structdecl** _temp3447; struct Cyc_Absyn_Structdecl*
_temp3449; struct Cyc_Absyn_Uniondecl** _temp3450; struct Cyc_Absyn_Uniondecl*
_temp3452; _LL3433: if(( unsigned int) _temp3431 >  4u?*(( int*) _temp3431) == 
Cyc_Absyn_AnonStructType: 0){ _LL3444: _temp3443=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3431)->f1; goto _LL3434;} else{ goto _LL3435;} _LL3435: if(( unsigned int)
_temp3431 >  4u?*(( int*) _temp3431) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3446:
_temp3445=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3431)->f1; goto
_LL3436;} else{ goto _LL3437;} _LL3437: if(( unsigned int) _temp3431 >  4u?*((
int*) _temp3431) ==  Cyc_Absyn_StructType: 0){ _LL3448: _temp3447=(( struct Cyc_Absyn_StructType_struct*)
_temp3431)->f3; if( _temp3447 ==  0){ goto _LL3439;} else{ _temp3449=* _temp3447;
goto _LL3438;}} else{ goto _LL3439;} _LL3439: if(( unsigned int) _temp3431 >  4u?*((
int*) _temp3431) ==  Cyc_Absyn_UnionType: 0){ _LL3451: _temp3450=(( struct Cyc_Absyn_UnionType_struct*)
_temp3431)->f3; if( _temp3450 ==  0){ goto _LL3441;} else{ _temp3452=* _temp3450;
goto _LL3440;}} else{ goto _LL3441;} _LL3441: goto _LL3442; _LL3434: _temp3445=
_temp3443; goto _LL3436; _LL3436: { struct Cyc_Absyn_Structfield* _temp3453= Cyc_Absyn_lookup_field(
_temp3445, _temp3377); if( _temp3453 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3453))->width !=  0: 0){ return({ struct _tuple7 _temp3454;
_temp3454.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3453))->tq).q_const;
_temp3454.f2=( Cyc_Tcutil_addressof_props( te, _temp3379)).f2; _temp3454;});}
return bogus_ans;} _LL3438: { struct Cyc_Absyn_Structfield* _temp3455= Cyc_Absyn_lookup_struct_field(
_temp3449, _temp3377); if( _temp3455 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3455))->width !=  0: 0){ return({ struct _tuple7 _temp3456;
_temp3456.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3455))->tq).q_const;
_temp3456.f2=( Cyc_Tcutil_addressof_props( te, _temp3379)).f2; _temp3456;});}
return bogus_ans;} _LL3440: { struct Cyc_Absyn_Structfield* _temp3457= Cyc_Absyn_lookup_union_field(
_temp3452, _temp3377); if( _temp3457 !=  0){ return({ struct _tuple7 _temp3458;
_temp3458.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3457))->tq).q_const;
_temp3458.f2=( Cyc_Tcutil_addressof_props( te, _temp3379)).f2; _temp3458;});}
goto _LL3442;} _LL3442: return bogus_ans; _LL3432:;} _LL3366: { void* _temp3459=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3383->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3465; void* _temp3467; void* _temp3469; _LL3461:
if(( unsigned int) _temp3459 >  4u?*(( int*) _temp3459) ==  Cyc_Absyn_PointerType:
0){ _LL3466: _temp3465=(( struct Cyc_Absyn_PointerType_struct*) _temp3459)->f1;
_LL3470: _temp3469=( void*) _temp3465.elt_typ; goto _LL3468; _LL3468: _temp3467=(
void*) _temp3465.rgn_typ; goto _LL3462;} else{ goto _LL3463;} _LL3463: goto
_LL3464; _LL3462: { void* _temp3471= Cyc_Tcutil_compress( _temp3469); struct Cyc_List_List*
_temp3483; struct Cyc_List_List* _temp3485; struct Cyc_Absyn_Structdecl**
_temp3487; struct Cyc_Absyn_Structdecl* _temp3489; struct Cyc_Absyn_Uniondecl**
_temp3490; struct Cyc_Absyn_Uniondecl* _temp3492; _LL3473: if(( unsigned int)
_temp3471 >  4u?*(( int*) _temp3471) ==  Cyc_Absyn_AnonStructType: 0){ _LL3484:
_temp3483=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3471)->f1; goto
_LL3474;} else{ goto _LL3475;} _LL3475: if(( unsigned int) _temp3471 >  4u?*((
int*) _temp3471) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3486: _temp3485=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3471)->f1; goto _LL3476;} else{ goto
_LL3477;} _LL3477: if(( unsigned int) _temp3471 >  4u?*(( int*) _temp3471) == 
Cyc_Absyn_StructType: 0){ _LL3488: _temp3487=(( struct Cyc_Absyn_StructType_struct*)
_temp3471)->f3; if( _temp3487 ==  0){ goto _LL3479;} else{ _temp3489=* _temp3487;
goto _LL3478;}} else{ goto _LL3479;} _LL3479: if(( unsigned int) _temp3471 >  4u?*((
int*) _temp3471) ==  Cyc_Absyn_UnionType: 0){ _LL3491: _temp3490=(( struct Cyc_Absyn_UnionType_struct*)
_temp3471)->f3; if( _temp3490 ==  0){ goto _LL3481;} else{ _temp3492=* _temp3490;
goto _LL3480;}} else{ goto _LL3481;} _LL3481: goto _LL3482; _LL3474: _temp3485=
_temp3483; goto _LL3476; _LL3476: { struct Cyc_Absyn_Structfield* _temp3493= Cyc_Absyn_lookup_field(
_temp3485, _temp3381); if( _temp3493 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3493))->width !=  0: 0){ return({ struct _tuple7 _temp3494;
_temp3494.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3493))->tq).q_const;
_temp3494.f2= _temp3467; _temp3494;});} return bogus_ans;} _LL3478: { struct Cyc_Absyn_Structfield*
_temp3495= Cyc_Absyn_lookup_struct_field( _temp3489, _temp3381); if( _temp3495
!=  0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3495))->width !=  0: 0){
return({ struct _tuple7 _temp3496; _temp3496.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3495))->tq).q_const; _temp3496.f2= _temp3467; _temp3496;});}
return bogus_ans;} _LL3480: { struct Cyc_Absyn_Structfield* _temp3497= Cyc_Absyn_lookup_union_field(
_temp3492, _temp3381); if( _temp3497 !=  0){ return({ struct _tuple7 _temp3498;
_temp3498.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3497))->tq).q_const;
_temp3498.f2= _temp3467; _temp3498;});} return bogus_ans;} _LL3482: return
bogus_ans; _LL3472:;} _LL3464: return bogus_ans; _LL3460:;} _LL3368: { void*
_temp3499= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3385->topt))->v); struct Cyc_Absyn_PtrInfo _temp3505; struct Cyc_Absyn_Tqual
_temp3507; void* _temp3509; _LL3501: if(( unsigned int) _temp3499 >  4u?*(( int*)
_temp3499) ==  Cyc_Absyn_PointerType: 0){ _LL3506: _temp3505=(( struct Cyc_Absyn_PointerType_struct*)
_temp3499)->f1; _LL3510: _temp3509=( void*) _temp3505.rgn_typ; goto _LL3508;
_LL3508: _temp3507= _temp3505.tq; goto _LL3502;} else{ goto _LL3503;} _LL3503:
goto _LL3504; _LL3502: return({ struct _tuple7 _temp3511; _temp3511.f1=
_temp3507.q_const; _temp3511.f2= _temp3509; _temp3511;}); _LL3504: return
bogus_ans; _LL3500:;} _LL3370: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3389->topt))->v); void* _temp3512= t; struct
Cyc_Absyn_Tqual _temp3522; struct Cyc_List_List* _temp3524; struct Cyc_Absyn_PtrInfo
_temp3526; struct Cyc_Absyn_Conref* _temp3528; struct Cyc_Absyn_Tqual _temp3530;
void* _temp3532; void* _temp3534; _LL3514: if(( unsigned int) _temp3512 >  4u?*((
int*) _temp3512) ==  Cyc_Absyn_ArrayType: 0){ _LL3523: _temp3522=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3512)->f2; goto _LL3515;} else{ goto _LL3516;} _LL3516: if(( unsigned int)
_temp3512 >  4u?*(( int*) _temp3512) ==  Cyc_Absyn_TupleType: 0){ _LL3525:
_temp3524=(( struct Cyc_Absyn_TupleType_struct*) _temp3512)->f1; goto _LL3517;}
else{ goto _LL3518;} _LL3518: if(( unsigned int) _temp3512 >  4u?*(( int*)
_temp3512) ==  Cyc_Absyn_PointerType: 0){ _LL3527: _temp3526=(( struct Cyc_Absyn_PointerType_struct*)
_temp3512)->f1; _LL3535: _temp3534=( void*) _temp3526.elt_typ; goto _LL3533;
_LL3533: _temp3532=( void*) _temp3526.rgn_typ; goto _LL3531; _LL3531: _temp3530=
_temp3526.tq; goto _LL3529; _LL3529: _temp3528= _temp3526.bounds; goto _LL3519;}
else{ goto _LL3520;} _LL3520: goto _LL3521; _LL3515: return({ struct _tuple7
_temp3536; _temp3536.f1= _temp3522.q_const; _temp3536.f2=( Cyc_Tcutil_addressof_props(
te, _temp3389)).f2; _temp3536;}); _LL3517: { struct _tuple4* _temp3537= Cyc_Absyn_lookup_tuple_field(
_temp3524,( int) Cyc_Evexp_eval_const_uint_exp( _temp3387)); if( _temp3537 !=  0){
return({ struct _tuple7 _temp3538; _temp3538.f1=((*(( struct _tuple4*)
_check_null( _temp3537))).f1).q_const; _temp3538.f2=( Cyc_Tcutil_addressof_props(
te, _temp3389)).f2; _temp3538;});} return bogus_ans;} _LL3519: return({ struct
_tuple7 _temp3539; _temp3539.f1= _temp3530.q_const; _temp3539.f2= _temp3532;
_temp3539;}); _LL3521: return bogus_ans; _LL3513:;} _LL3372:({ void* _temp3540[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3540, sizeof( void*), 0u));});
return bogus_ans; _LL3360:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3542= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3548; void* _temp3550; _LL3544: if((
unsigned int) _temp3542 >  4u?*(( int*) _temp3542) ==  Cyc_Absyn_ArrayType: 0){
_LL3551: _temp3550=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3542)->f1;
goto _LL3549; _LL3549: _temp3548=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3542)->f2; goto _LL3545;} else{ goto _LL3546;} _LL3546: goto _LL3547;
_LL3545: { void* _temp3554; struct _tuple7 _temp3552= Cyc_Tcutil_addressof_props(
te, e); _LL3555: _temp3554= _temp3552.f2; goto _LL3553; _LL3553: return Cyc_Absyn_atb_typ(
_temp3550, _temp3554, _temp3548,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3556=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3556[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3557; _temp3557.tag= Cyc_Absyn_Upper_b;
_temp3557.f1= e; _temp3557;}); _temp3556;}));} _LL3547: return e_typ; _LL3543:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3558=(
void*) b->v; void* _temp3566; void* _temp3568; struct Cyc_Absyn_Exp* _temp3570;
_LL3560: if(( unsigned int) _temp3558 >  1u?*(( int*) _temp3558) ==  Cyc_Absyn_Eq_constr:
0){ _LL3567: _temp3566=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3558)->f1;
if( _temp3566 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3561;} else{ goto
_LL3562;}} else{ goto _LL3562;} _LL3562: if(( unsigned int) _temp3558 >  1u?*((
int*) _temp3558) ==  Cyc_Absyn_Eq_constr: 0){ _LL3569: _temp3568=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3558)->f1; if(( unsigned int) _temp3568
>  1u?*(( int*) _temp3568) ==  Cyc_Absyn_Upper_b: 0){ _LL3571: _temp3570=((
struct Cyc_Absyn_Upper_b_struct*) _temp3568)->f1; goto _LL3563;} else{ goto
_LL3564;}} else{ goto _LL3564;} _LL3564: goto _LL3565; _LL3561: return; _LL3563:
if( Cyc_Evexp_eval_const_uint_exp( _temp3570) <=  i){({ void* _temp3572[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3572, sizeof( void*), 0u));});} return;
_LL3565:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3573=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3573[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3574; _temp3574.tag= Cyc_Absyn_Eq_constr;
_temp3574.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3575=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3575[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3576; _temp3576.tag= Cyc_Absyn_Upper_b;
_temp3576.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3576;}); _temp3575;}));
_temp3574;}); _temp3573;}))); return; _LL3559:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp3577=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp3583; struct
Cyc_Absyn_Exp* _temp3585; _LL3579: if(( unsigned int) _temp3577 >  1u?*(( int*)
_temp3577) ==  Cyc_Absyn_Eq_constr: 0){ _LL3584: _temp3583=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3577)->f1; if(( unsigned int) _temp3583 >  1u?*(( int*) _temp3583) ==  Cyc_Absyn_Upper_b:
0){ _LL3586: _temp3585=(( struct Cyc_Absyn_Upper_b_struct*) _temp3583)->f1; goto
_LL3580;} else{ goto _LL3581;}} else{ goto _LL3581;} _LL3581: goto _LL3582;
_LL3580: return Cyc_Evexp_eval_const_uint_exp( _temp3585) ==  1; _LL3582: return
0; _LL3578:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp3587= Cyc_Tcutil_compress(
t); void* _temp3613; struct Cyc_List_List* _temp3615; struct Cyc_Absyn_Structdecl**
_temp3617; struct Cyc_List_List* _temp3619; struct Cyc_Absyn_Uniondecl**
_temp3621; struct Cyc_List_List* _temp3623; struct Cyc_List_List* _temp3625;
struct Cyc_List_List* _temp3627; _LL3589: if( _temp3587 == ( void*) Cyc_Absyn_VoidType){
goto _LL3590;} else{ goto _LL3591;} _LL3591: if(( unsigned int) _temp3587 >  4u?*((
int*) _temp3587) ==  Cyc_Absyn_IntType: 0){ goto _LL3592;} else{ goto _LL3593;}
_LL3593: if( _temp3587 == ( void*) Cyc_Absyn_FloatType){ goto _LL3594;} else{
goto _LL3595;} _LL3595: if( _temp3587 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3596;} else{ goto _LL3597;} _LL3597: if(( unsigned int) _temp3587 >  4u?*((
int*) _temp3587) ==  Cyc_Absyn_EnumType: 0){ goto _LL3598;} else{ goto _LL3599;}
_LL3599: if(( unsigned int) _temp3587 >  4u?*(( int*) _temp3587) ==  Cyc_Absyn_ArrayType:
0){ _LL3614: _temp3613=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3587)->f1;
goto _LL3600;} else{ goto _LL3601;} _LL3601: if(( unsigned int) _temp3587 >  4u?*((
int*) _temp3587) ==  Cyc_Absyn_TupleType: 0){ _LL3616: _temp3615=(( struct Cyc_Absyn_TupleType_struct*)
_temp3587)->f1; goto _LL3602;} else{ goto _LL3603;} _LL3603: if(( unsigned int)
_temp3587 >  4u?*(( int*) _temp3587) ==  Cyc_Absyn_StructType: 0){ _LL3620:
_temp3619=(( struct Cyc_Absyn_StructType_struct*) _temp3587)->f2; goto _LL3618;
_LL3618: _temp3617=(( struct Cyc_Absyn_StructType_struct*) _temp3587)->f3; goto
_LL3604;} else{ goto _LL3605;} _LL3605: if(( unsigned int) _temp3587 >  4u?*((
int*) _temp3587) ==  Cyc_Absyn_UnionType: 0){ _LL3624: _temp3623=(( struct Cyc_Absyn_UnionType_struct*)
_temp3587)->f2; goto _LL3622; _LL3622: _temp3621=(( struct Cyc_Absyn_UnionType_struct*)
_temp3587)->f3; goto _LL3606;} else{ goto _LL3607;} _LL3607: if(( unsigned int)
_temp3587 >  4u?*(( int*) _temp3587) ==  Cyc_Absyn_AnonStructType: 0){ _LL3626:
_temp3625=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3587)->f1; goto
_LL3608;} else{ goto _LL3609;} _LL3609: if(( unsigned int) _temp3587 >  4u?*((
int*) _temp3587) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3628: _temp3627=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3587)->f1; goto _LL3610;} else{ goto
_LL3611;} _LL3611: goto _LL3612; _LL3590: goto _LL3592; _LL3592: goto _LL3594;
_LL3594: goto _LL3596; _LL3596: return 1; _LL3598: return 0; _LL3600: return Cyc_Tcutil_bits_only(
_temp3613); _LL3602: for( 0; _temp3615 !=  0; _temp3615=(( struct Cyc_List_List*)
_check_null( _temp3615))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3615))->hd)).f2)){ return 0;}} return 1;
_LL3604: if( _temp3617 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp3629=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp3617)); if(
_temp3629->fields ==  0){ return 0;}{ struct _RegionHandle _temp3630=
_new_region(); struct _RegionHandle* rgn=& _temp3630; _push_region( rgn);{
struct Cyc_List_List* _temp3631=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3629->tvs, _temp3619);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp3629->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp3631,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ int _temp3632= 0;
_npop_handler( 0u); return _temp3632;}}}{ int _temp3633= 1; _npop_handler( 0u);
return _temp3633;}}; _pop_region( rgn);}} _LL3606: if( _temp3621 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3634=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3621)); if( _temp3634->fields ==  0){ return 0;}{ struct
_RegionHandle _temp3635= _new_region(); struct _RegionHandle* rgn=& _temp3635;
_push_region( rgn);{ struct Cyc_List_List* _temp3636=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3634->tvs, _temp3623);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3634->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3636,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3637= 0; _npop_handler( 0u); return
_temp3637;}}}{ int _temp3638= 1; _npop_handler( 0u); return _temp3638;}};
_pop_region( rgn);}} _LL3608: _temp3627= _temp3625; goto _LL3610; _LL3610: for(
0; _temp3627 !=  0; _temp3627=(( struct Cyc_List_List*) _check_null( _temp3627))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3627))->hd)->type)){ return 0;}} return 1;
_LL3612: return 0; _LL3588:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3639=( void*) e->r; struct
_tuple1* _temp3683; struct Cyc_Absyn_Exp* _temp3685; struct Cyc_Absyn_Exp*
_temp3687; struct Cyc_Absyn_Exp* _temp3689; struct Cyc_Absyn_Exp* _temp3691;
struct Cyc_Absyn_Exp* _temp3693; struct Cyc_Absyn_Exp* _temp3695; struct Cyc_Absyn_Exp*
_temp3697; struct Cyc_Absyn_Exp* _temp3699; void* _temp3701; struct Cyc_Absyn_Exp*
_temp3703; struct Cyc_Absyn_Exp* _temp3705; struct Cyc_Absyn_Exp* _temp3707;
struct Cyc_List_List* _temp3709; struct Cyc_List_List* _temp3711; struct Cyc_List_List*
_temp3713; struct Cyc_List_List* _temp3715; void* _temp3717; struct Cyc_List_List*
_temp3719; struct Cyc_List_List* _temp3721; _LL3641: if(*(( int*) _temp3639) == 
Cyc_Absyn_Const_e){ goto _LL3642;} else{ goto _LL3643;} _LL3643: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3644;} else{ goto _LL3645;}
_LL3645: if(*(( int*) _temp3639) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3646;}
else{ goto _LL3647;} _LL3647: if(*(( int*) _temp3639) ==  Cyc_Absyn_Offsetof_e){
goto _LL3648;} else{ goto _LL3649;} _LL3649: if(*(( int*) _temp3639) ==  Cyc_Absyn_Gentyp_e){
goto _LL3650;} else{ goto _LL3651;} _LL3651: if(*(( int*) _temp3639) ==  Cyc_Absyn_Enum_e){
goto _LL3652;} else{ goto _LL3653;} _LL3653: if(*(( int*) _temp3639) ==  Cyc_Absyn_Var_e){
_LL3684: _temp3683=(( struct Cyc_Absyn_Var_e_struct*) _temp3639)->f1; goto
_LL3654;} else{ goto _LL3655;} _LL3655: if(*(( int*) _temp3639) ==  Cyc_Absyn_Conditional_e){
_LL3690: _temp3689=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3639)->f1;
goto _LL3688; _LL3688: _temp3687=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3639)->f2; goto _LL3686; _LL3686: _temp3685=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3639)->f3; goto _LL3656;} else{ goto _LL3657;} _LL3657: if(*(( int*)
_temp3639) ==  Cyc_Absyn_SeqExp_e){ _LL3694: _temp3693=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3639)->f1; goto _LL3692; _LL3692: _temp3691=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3639)->f2; goto _LL3658;} else{ goto _LL3659;} _LL3659: if(*(( int*)
_temp3639) ==  Cyc_Absyn_NoInstantiate_e){ _LL3696: _temp3695=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3639)->f1; goto _LL3660;} else{ goto _LL3661;} _LL3661: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Instantiate_e){ _LL3698: _temp3697=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3639)->f1; goto _LL3662;} else{ goto _LL3663;} _LL3663: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Cast_e){ _LL3702: _temp3701=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3639)->f1; goto _LL3700; _LL3700: _temp3699=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3639)->f2; goto _LL3664;} else{ goto _LL3665;} _LL3665: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Address_e){ _LL3704: _temp3703=(( struct Cyc_Absyn_Address_e_struct*)
_temp3639)->f1; goto _LL3666;} else{ goto _LL3667;} _LL3667: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Comprehension_e){ _LL3708: _temp3707=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3639)->f2; goto _LL3706; _LL3706: _temp3705=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3639)->f3; goto _LL3668;} else{ goto _LL3669;} _LL3669: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Array_e){ _LL3710: _temp3709=(( struct Cyc_Absyn_Array_e_struct*)
_temp3639)->f1; goto _LL3670;} else{ goto _LL3671;} _LL3671: if(*(( int*)
_temp3639) ==  Cyc_Absyn_AnonStruct_e){ _LL3712: _temp3711=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3639)->f2; goto _LL3672;} else{ goto _LL3673;} _LL3673: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Struct_e){ _LL3714: _temp3713=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3639)->f3; goto _LL3674;} else{ goto _LL3675;} _LL3675: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Primop_e){ _LL3718: _temp3717=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3639)->f1; goto _LL3716; _LL3716: _temp3715=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3639)->f2; goto _LL3676;} else{ goto _LL3677;} _LL3677: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Tuple_e){ _LL3720: _temp3719=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3639)->f1; goto _LL3678;} else{ goto _LL3679;} _LL3679: if(*(( int*)
_temp3639) ==  Cyc_Absyn_Tunion_e){ _LL3722: _temp3721=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3639)->f3; goto _LL3680;} else{ goto _LL3681;} _LL3681: goto _LL3682;
_LL3642: return 1; _LL3644: return 1; _LL3646: return 1; _LL3648: return 1;
_LL3650: return 1; _LL3652: return 1; _LL3654: { struct _handler_cons _temp3723;
_push_handler(& _temp3723);{ int _temp3725= 0; if( setjmp( _temp3723.handler)){
_temp3725= 1;} if( ! _temp3725){{ void* _temp3726= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3683); void* _temp3736; void* _temp3738; struct Cyc_Absyn_Vardecl*
_temp3740; void* _temp3742; _LL3728: if(*(( int*) _temp3726) ==  Cyc_Tcenv_VarRes){
_LL3737: _temp3736=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3726)->f1;
if(( unsigned int) _temp3736 >  1u?*(( int*) _temp3736) ==  Cyc_Absyn_Funname_b:
0){ goto _LL3729;} else{ goto _LL3730;}} else{ goto _LL3730;} _LL3730: if(*((
int*) _temp3726) ==  Cyc_Tcenv_VarRes){ _LL3739: _temp3738=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3726)->f1; if(( unsigned int) _temp3738 >  1u?*(( int*) _temp3738) ==  Cyc_Absyn_Global_b:
0){ _LL3741: _temp3740=(( struct Cyc_Absyn_Global_b_struct*) _temp3738)->f1;
goto _LL3731;} else{ goto _LL3732;}} else{ goto _LL3732;} _LL3732: if(*(( int*)
_temp3726) ==  Cyc_Tcenv_VarRes){ _LL3743: _temp3742=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3726)->f1; if( _temp3742 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL3733;}
else{ goto _LL3734;}} else{ goto _LL3734;} _LL3734: goto _LL3735; _LL3729: { int
_temp3744= 1; _npop_handler( 0u); return _temp3744;} _LL3731: { void* _temp3745=
Cyc_Tcutil_compress(( void*) _temp3740->type); _LL3747: if(( unsigned int)
_temp3745 >  4u?*(( int*) _temp3745) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3748;}
else{ goto _LL3749;} _LL3749: goto _LL3750; _LL3748: { int _temp3751= 1;
_npop_handler( 0u); return _temp3751;} _LL3750: { int _temp3752= var_okay;
_npop_handler( 0u); return _temp3752;} _LL3746:;} _LL3733: { int _temp3753= 0;
_npop_handler( 0u); return _temp3753;} _LL3735: { int _temp3754= var_okay;
_npop_handler( 0u); return _temp3754;} _LL3727:;}; _pop_handler();} else{ void*
_temp3724=( void*) _exn_thrown; void* _temp3756= _temp3724; _LL3758: if(
_temp3756 ==  Cyc_Dict_Absent){ goto _LL3759;} else{ goto _LL3760;} _LL3760:
goto _LL3761; _LL3759: return 0; _LL3761:( void) _throw( _temp3756); _LL3757:;}}}
_LL3656: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3689)? Cyc_Tcutil_cnst_exp( te,
0, _temp3687): 0)? Cyc_Tcutil_cnst_exp( te, 0, _temp3685): 0; _LL3658: return
Cyc_Tcutil_cnst_exp( te, 0, _temp3693)? Cyc_Tcutil_cnst_exp( te, 0, _temp3691):
0; _LL3660: _temp3697= _temp3695; goto _LL3662; _LL3662: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3697); _LL3664: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3699); _LL3666: return Cyc_Tcutil_cnst_exp( te, 1, _temp3703); _LL3668:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3707)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3705): 0; _LL3670: _temp3711= _temp3709; goto _LL3672; _LL3672: _temp3713=
_temp3711; goto _LL3674; _LL3674: for( 0; _temp3713 !=  0; _temp3713=(( struct
Cyc_List_List*) _check_null( _temp3713))->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*((
struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3713))->hd)).f2)){
return 0;}} return 1; _LL3676: _temp3719= _temp3715; goto _LL3678; _LL3678:
_temp3721= _temp3719; goto _LL3680; _LL3680: for( 0; _temp3721 !=  0; _temp3721=((
struct Cyc_List_List*) _check_null( _temp3721))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3721))->hd)){
return 0;}} return 1; _LL3682: return 0; _LL3640:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3762= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3790; struct Cyc_Absyn_Conref* _temp3792; struct Cyc_Absyn_Conref*
_temp3794; void* _temp3796; struct Cyc_List_List* _temp3798; struct Cyc_Absyn_Structdecl**
_temp3800; struct Cyc_List_List* _temp3802; struct Cyc_Absyn_Uniondecl**
_temp3804; struct Cyc_List_List* _temp3806; struct Cyc_List_List* _temp3808;
struct Cyc_List_List* _temp3810; _LL3764: if( _temp3762 == ( void*) Cyc_Absyn_VoidType){
goto _LL3765;} else{ goto _LL3766;} _LL3766: if(( unsigned int) _temp3762 >  4u?*((
int*) _temp3762) ==  Cyc_Absyn_IntType: 0){ goto _LL3767;} else{ goto _LL3768;}
_LL3768: if( _temp3762 == ( void*) Cyc_Absyn_FloatType){ goto _LL3769;} else{
goto _LL3770;} _LL3770: if( _temp3762 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3771;} else{ goto _LL3772;} _LL3772: if(( unsigned int) _temp3762 >  4u?*((
int*) _temp3762) ==  Cyc_Absyn_PointerType: 0){ _LL3791: _temp3790=(( struct Cyc_Absyn_PointerType_struct*)
_temp3762)->f1; _LL3795: _temp3794= _temp3790.nullable; goto _LL3793; _LL3793:
_temp3792= _temp3790.bounds; goto _LL3773;} else{ goto _LL3774;} _LL3774: if((
unsigned int) _temp3762 >  4u?*(( int*) _temp3762) ==  Cyc_Absyn_ArrayType: 0){
_LL3797: _temp3796=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3762)->f1;
goto _LL3775;} else{ goto _LL3776;} _LL3776: if(( unsigned int) _temp3762 >  4u?*((
int*) _temp3762) ==  Cyc_Absyn_TupleType: 0){ _LL3799: _temp3798=(( struct Cyc_Absyn_TupleType_struct*)
_temp3762)->f1; goto _LL3777;} else{ goto _LL3778;} _LL3778: if(( unsigned int)
_temp3762 >  4u?*(( int*) _temp3762) ==  Cyc_Absyn_StructType: 0){ _LL3803:
_temp3802=(( struct Cyc_Absyn_StructType_struct*) _temp3762)->f2; goto _LL3801;
_LL3801: _temp3800=(( struct Cyc_Absyn_StructType_struct*) _temp3762)->f3; goto
_LL3779;} else{ goto _LL3780;} _LL3780: if(( unsigned int) _temp3762 >  4u?*((
int*) _temp3762) ==  Cyc_Absyn_UnionType: 0){ _LL3807: _temp3806=(( struct Cyc_Absyn_UnionType_struct*)
_temp3762)->f2; goto _LL3805; _LL3805: _temp3804=(( struct Cyc_Absyn_UnionType_struct*)
_temp3762)->f3; goto _LL3781;} else{ goto _LL3782;} _LL3782: if(( unsigned int)
_temp3762 >  4u?*(( int*) _temp3762) ==  Cyc_Absyn_AnonStructType: 0){ _LL3809:
_temp3808=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3762)->f1; goto
_LL3783;} else{ goto _LL3784;} _LL3784: if(( unsigned int) _temp3762 >  4u?*((
int*) _temp3762) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3811: _temp3810=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3762)->f1; goto _LL3785;} else{ goto
_LL3786;} _LL3786: if(( unsigned int) _temp3762 >  4u?*(( int*) _temp3762) == 
Cyc_Absyn_EnumType: 0){ goto _LL3787;} else{ goto _LL3788;} _LL3788: goto
_LL3789; _LL3765: goto _LL3767; _LL3767: goto _LL3769; _LL3769: goto _LL3771;
_LL3771: return 1; _LL3773: { void* _temp3812=( void*)( Cyc_Absyn_compress_conref(
_temp3792))->v; void* _temp3820; void* _temp3822; _LL3814: if(( unsigned int)
_temp3812 >  1u?*(( int*) _temp3812) ==  Cyc_Absyn_Eq_constr: 0){ _LL3821:
_temp3820=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3812)->f1; if(
_temp3820 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3815;} else{ goto _LL3816;}}
else{ goto _LL3816;} _LL3816: if(( unsigned int) _temp3812 >  1u?*(( int*)
_temp3812) ==  Cyc_Absyn_Eq_constr: 0){ _LL3823: _temp3822=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3812)->f1; if(( unsigned int) _temp3822 >  1u?*(( int*) _temp3822) ==  Cyc_Absyn_Upper_b:
0){ goto _LL3817;} else{ goto _LL3818;}} else{ goto _LL3818;} _LL3818: goto
_LL3819; _LL3815: return 1; _LL3817: { void* _temp3824=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3794))->v; int
_temp3830; _LL3826: if(( unsigned int) _temp3824 >  1u?*(( int*) _temp3824) == 
Cyc_Absyn_Eq_constr: 0){ _LL3831: _temp3830=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3824)->f1; goto _LL3827;} else{ goto _LL3828;} _LL3828: goto _LL3829;
_LL3827: return _temp3830; _LL3829: return 0; _LL3825:;} _LL3819: return 0;
_LL3813:;} _LL3775: return Cyc_Tcutil_supports_default( _temp3796); _LL3777:
for( 0; _temp3798 !=  0; _temp3798=(( struct Cyc_List_List*) _check_null(
_temp3798))->tl){ if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3798))->hd)).f2)){ return 0;}} return 1;
_LL3779: if( _temp3800 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3800)); if( sd->fields ==  0){
return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs, _temp3802,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);} _LL3781:
if( _temp3804 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3804)); if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp3806,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL3783: _temp3810= _temp3808; goto _LL3785; _LL3785: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp3810); _LL3787: return 1; _LL3789:
return 0; _LL3763:;} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{ struct _RegionHandle
_temp3832= _new_region(); struct _RegionHandle* rgn=& _temp3832; _push_region(
rgn);{ struct Cyc_List_List* _temp3833=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3833,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3834= 0; _npop_handler( 0u);
return _temp3834;}}}; _pop_region( rgn);} return 1;}
