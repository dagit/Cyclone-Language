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
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern int Cyc_Core_intcmp(
int, int); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stderr; extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Std_printf( struct _tagged_arr fmt, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_vrprintf( struct
_RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static const
int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short*
f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
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
l, void* x); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List*); static const int Cyc_Position_Lex= 0; static const int
Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
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
const int Cyc_Absyn_DoubleType= 6; struct Cyc_Absyn_DoubleType_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_ArrayType= 7; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ;
static const int Cyc_Absyn_FnType= 8; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 9; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static const
int Cyc_Absyn_StructType= 10; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_AnonStructType= 13; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 14;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 15; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
16; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 17; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 18;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
19; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 20; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern int Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv*,
void* r1, void* r2); extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat( struct _tagged_arr,
struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct
Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void*
Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap); extern void
Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
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
void* t); extern int Cyc_Tcutil_is_pointer_type( void* t); extern int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp* e); extern struct Cyc_Core_Opt Cyc_Tcutil_rk; extern
struct Cyc_Core_Opt Cyc_Tcutil_ak; extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk; extern int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*, void* t, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern void Cyc_Tcutil_explain_failure(); extern int Cyc_Tcutil_unify(
void*, void*); extern int Cyc_Tcutil_typecmp( void*, void*); extern void* Cyc_Tcutil_substitute(
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
unsigned int) _temp26 >  3u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL39:
_temp38=(( struct Cyc_Absyn_Evar_struct*) _temp26)->f2; if( _temp38 ==  0){ goto
_LL29;} else{ goto _LL30;}} else{ goto _LL30;} _LL30: if(( unsigned int) _temp26
>  3u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL41: _temp40=(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; _temp42=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; goto _LL31;} else{ goto _LL32;} _LL32: if(( unsigned int) _temp26
>  3u?*(( int*) _temp26) ==  Cyc_Absyn_TypedefType: 0){ _LL44: _temp43=(( struct
Cyc_Absyn_TypedefType_struct*) _temp26)->f3; if( _temp43 ==  0){ goto _LL33;}
else{ goto _LL34;}} else{ goto _LL34;} _LL34: if(( unsigned int) _temp26 >  3u?*((
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
void* _temp163; void* _temp165; void* _temp167; void* _temp169; int _temp171;
struct Cyc_Absyn_Exp* _temp173; struct Cyc_Absyn_Tqual _temp175; void* _temp177;
struct Cyc_Absyn_FnInfo _temp179; struct Cyc_List_List* _temp181; struct Cyc_List_List*
_temp183; struct Cyc_Absyn_VarargInfo* _temp185; int _temp187; struct Cyc_List_List*
_temp189; void* _temp191; struct Cyc_Core_Opt* _temp193; struct Cyc_List_List*
_temp195; struct Cyc_List_List* _temp197; struct Cyc_List_List* _temp199; struct
_tuple1* _temp201; struct Cyc_List_List* _temp203; struct _tuple1* _temp205;
struct Cyc_List_List* _temp207; struct Cyc_List_List* _temp209; struct Cyc_Absyn_Enumdecl*
_temp211; struct _tuple1* _temp213; struct Cyc_List_List* _temp215; void*
_temp217; struct Cyc_List_List* _temp219; struct _tuple1* _temp221; void*
_temp223; struct Cyc_List_List* _temp225; void* _temp227; _LL91: if( _temp89 == (
void*) Cyc_Absyn_VoidType){ goto _LL92;} else{ goto _LL93;} _LL93: if((
unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_Evar: 0){ goto
_LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp89 >  3u?*(( int*)
_temp89) ==  Cyc_Absyn_VarType: 0){ _LL140: _temp139=(( struct Cyc_Absyn_VarType_struct*)
_temp89)->f1; goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp89
>  3u?*(( int*) _temp89) ==  Cyc_Absyn_TunionType: 0){ _LL142: _temp141=((
struct Cyc_Absyn_TunionType_struct*) _temp89)->f1; _LL148: _temp147=( void*)
_temp141.tunion_info; goto _LL146; _LL146: _temp145= _temp141.targs; goto _LL144;
_LL144: _temp143=( void*) _temp141.rgn; goto _LL98;} else{ goto _LL99;} _LL99:
if(( unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_TunionFieldType:
0){ _LL150: _temp149=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp89)->f1;
_LL154: _temp153=( void*) _temp149.field_info; goto _LL152; _LL152: _temp151=
_temp149.targs; goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int)
_temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_PointerType: 0){ _LL156: _temp155=((
struct Cyc_Absyn_PointerType_struct*) _temp89)->f1; _LL166: _temp165=( void*)
_temp155.elt_typ; goto _LL164; _LL164: _temp163=( void*) _temp155.rgn_typ; goto
_LL162; _LL162: _temp161= _temp155.nullable; goto _LL160; _LL160: _temp159=
_temp155.tq; goto _LL158; _LL158: _temp157= _temp155.bounds; goto _LL102;} else{
goto _LL103;} _LL103: if(( unsigned int) _temp89 >  3u?*(( int*) _temp89) == 
Cyc_Absyn_IntType: 0){ _LL170: _temp169=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp89)->f1; goto _LL168; _LL168: _temp167=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp89)->f2; goto _LL104;} else{ goto _LL105;} _LL105: if( _temp89 == ( void*)
Cyc_Absyn_FloatType){ goto _LL106;} else{ goto _LL107;} _LL107: if((
unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_DoubleType: 0){
_LL172: _temp171=(( struct Cyc_Absyn_DoubleType_struct*) _temp89)->f1; goto
_LL108;} else{ goto _LL109;} _LL109: if(( unsigned int) _temp89 >  3u?*(( int*)
_temp89) ==  Cyc_Absyn_ArrayType: 0){ _LL178: _temp177=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp89)->f1; goto _LL176; _LL176: _temp175=(( struct Cyc_Absyn_ArrayType_struct*)
_temp89)->f2; goto _LL174; _LL174: _temp173=(( struct Cyc_Absyn_ArrayType_struct*)
_temp89)->f3; goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int)
_temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_FnType: 0){ _LL180: _temp179=((
struct Cyc_Absyn_FnType_struct*) _temp89)->f1; _LL196: _temp195= _temp179.tvars;
goto _LL194; _LL194: _temp193= _temp179.effect; goto _LL192; _LL192: _temp191=(
void*) _temp179.ret_typ; goto _LL190; _LL190: _temp189= _temp179.args; goto
_LL188; _LL188: _temp187= _temp179.c_varargs; goto _LL186; _LL186: _temp185=
_temp179.cyc_varargs; goto _LL184; _LL184: _temp183= _temp179.rgn_po; goto
_LL182; _LL182: _temp181= _temp179.attributes; goto _LL112;} else{ goto _LL113;}
_LL113: if(( unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_TupleType:
0){ _LL198: _temp197=(( struct Cyc_Absyn_TupleType_struct*) _temp89)->f1; goto
_LL114;} else{ goto _LL115;} _LL115: if(( unsigned int) _temp89 >  3u?*(( int*)
_temp89) ==  Cyc_Absyn_StructType: 0){ _LL202: _temp201=(( struct Cyc_Absyn_StructType_struct*)
_temp89)->f1; goto _LL200; _LL200: _temp199=(( struct Cyc_Absyn_StructType_struct*)
_temp89)->f2; goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int)
_temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_UnionType: 0){ _LL206: _temp205=((
struct Cyc_Absyn_UnionType_struct*) _temp89)->f1; goto _LL204; _LL204: _temp203=((
struct Cyc_Absyn_UnionType_struct*) _temp89)->f2; goto _LL118;} else{ goto
_LL119;} _LL119: if(( unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_AnonStructType:
0){ _LL208: _temp207=(( struct Cyc_Absyn_AnonStructType_struct*) _temp89)->f1;
goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int) _temp89 >  3u?*((
int*) _temp89) ==  Cyc_Absyn_AnonUnionType: 0){ _LL210: _temp209=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp89)->f1; goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int)
_temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_EnumType: 0){ _LL214: _temp213=((
struct Cyc_Absyn_EnumType_struct*) _temp89)->f1; goto _LL212; _LL212: _temp211=((
struct Cyc_Absyn_EnumType_struct*) _temp89)->f2; goto _LL124;} else{ goto _LL125;}
_LL125: if(( unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_AnonEnumType:
0){ _LL216: _temp215=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp89)->f1;
goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp89 >  3u?*((
int*) _temp89) ==  Cyc_Absyn_RgnHandleType: 0){ _LL218: _temp217=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp89)->f1; goto _LL128;} else{ goto
_LL129;} _LL129: if(( unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_TypedefType:
0){ _LL222: _temp221=(( struct Cyc_Absyn_TypedefType_struct*) _temp89)->f1; goto
_LL220; _LL220: _temp219=(( struct Cyc_Absyn_TypedefType_struct*) _temp89)->f2;
goto _LL130;} else{ goto _LL131;} _LL131: if( _temp89 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int) _temp89 >  3u?*((
int*) _temp89) ==  Cyc_Absyn_AccessEff: 0){ _LL224: _temp223=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp89)->f1; goto _LL134;} else{ goto _LL135;}
_LL135: if(( unsigned int) _temp89 >  3u?*(( int*) _temp89) ==  Cyc_Absyn_JoinEff:
0){ _LL226: _temp225=(( struct Cyc_Absyn_JoinEff_struct*) _temp89)->f1; goto
_LL136;} else{ goto _LL137;} _LL137: if(( unsigned int) _temp89 >  3u?*(( int*)
_temp89) ==  Cyc_Absyn_RgnsEff: 0){ _LL228: _temp227=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp89)->f1; goto _LL138;} else{ goto _LL90;} _LL92: goto _LL94; _LL94: return
t; _LL96: return( void*)({ struct Cyc_Absyn_VarType_struct* _temp229=( struct
Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp229[ 0]=({ struct Cyc_Absyn_VarType_struct _temp230; _temp230.tag= Cyc_Absyn_VarType;
_temp230.f1= Cyc_Tcutil_copy_tvar( _temp139); _temp230;}); _temp229;}); _LL98:
return( void*)({ struct Cyc_Absyn_TunionType_struct* _temp231=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp231[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp232; _temp232.tag= Cyc_Absyn_TunionType;
_temp232.f1=({ struct Cyc_Absyn_TunionInfo _temp233; _temp233.tunion_info=( void*)
_temp147; _temp233.targs= Cyc_Tcutil_copy_types( _temp145); _temp233.rgn=( void*)
Cyc_Tcutil_copy_type( _temp143); _temp233;}); _temp232;}); _temp231;}); _LL100:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp234=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp234[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp235; _temp235.tag= Cyc_Absyn_TunionFieldType;
_temp235.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp236; _temp236.field_info=(
void*) _temp153; _temp236.targs= Cyc_Tcutil_copy_types( _temp151); _temp236;});
_temp235;}); _temp234;}); _LL102: { void* _temp237= Cyc_Tcutil_copy_type(
_temp165); void* _temp238= Cyc_Tcutil_copy_type( _temp163); struct Cyc_Absyn_Conref*
_temp239=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp161); struct Cyc_Absyn_Tqual _temp240= _temp159; struct Cyc_Absyn_Conref*
_temp241= Cyc_Tcutil_copy_conref( _temp157); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp242=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp242[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp243; _temp243.tag= Cyc_Absyn_PointerType;
_temp243.f1=({ struct Cyc_Absyn_PtrInfo _temp244; _temp244.elt_typ=( void*)
_temp237; _temp244.rgn_typ=( void*) _temp238; _temp244.nullable= _temp239;
_temp244.tq= _temp240; _temp244.bounds= _temp241; _temp244;}); _temp243;});
_temp242;});} _LL104: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp245=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp245[ 0]=({ struct Cyc_Absyn_IntType_struct _temp246; _temp246.tag= Cyc_Absyn_IntType;
_temp246.f1=( void*) _temp169; _temp246.f2=( void*) _temp167; _temp246;});
_temp245;}); _LL106: return t; _LL108: return( void*)({ struct Cyc_Absyn_DoubleType_struct*
_temp247=( struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_DoubleType_struct)); _temp247[ 0]=({ struct Cyc_Absyn_DoubleType_struct
_temp248; _temp248.tag= Cyc_Absyn_DoubleType; _temp248.f1= _temp171; _temp248;});
_temp247;}); _LL110: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp249=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp249[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp250; _temp250.tag= Cyc_Absyn_ArrayType;
_temp250.f1=( void*) Cyc_Tcutil_copy_type( _temp177); _temp250.f2= _temp175;
_temp250.f3= _temp173; _temp250;}); _temp249;}); _LL112: { struct Cyc_List_List*
_temp251=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp195); struct
Cyc_Core_Opt* _temp252= _temp193 ==  0? 0:({ struct Cyc_Core_Opt* _temp262=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp262->v=(
void*) Cyc_Tcutil_copy_type(( void*) _temp193->v); _temp262;}); void* _temp253=
Cyc_Tcutil_copy_type( _temp191); struct Cyc_List_List* _temp254=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_copy_arg, _temp189); int _temp255= _temp187; struct Cyc_Absyn_VarargInfo*
cyc_varargs2= 0; if( _temp185 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp185); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp256=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp256->name= cv->name; _temp256->tq= cv->tq; _temp256->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp256->inject= cv->inject; _temp256;});}{ struct Cyc_List_List*
_temp257=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp183);
struct Cyc_List_List* _temp258= _temp181; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp259=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp259[ 0]=({ struct Cyc_Absyn_FnType_struct _temp260; _temp260.tag= Cyc_Absyn_FnType;
_temp260.f1=({ struct Cyc_Absyn_FnInfo _temp261; _temp261.tvars= _temp251;
_temp261.effect= _temp252; _temp261.ret_typ=( void*) _temp253; _temp261.args=
_temp254; _temp261.c_varargs= _temp255; _temp261.cyc_varargs= cyc_varargs2;
_temp261.rgn_po= _temp257; _temp261.attributes= _temp258; _temp261;}); _temp260;});
_temp259;});}} _LL114: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp263=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp263[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp264; _temp264.tag= Cyc_Absyn_TupleType;
_temp264.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp197);
_temp264;}); _temp263;}); _LL116: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp265=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp265[ 0]=({ struct Cyc_Absyn_StructType_struct _temp266; _temp266.tag= Cyc_Absyn_StructType;
_temp266.f1= _temp201; _temp266.f2= Cyc_Tcutil_copy_types( _temp199); _temp266.f3=
0; _temp266;}); _temp265;}); _LL118: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp267=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp267[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp268; _temp268.tag= Cyc_Absyn_UnionType;
_temp268.f1= _temp205; _temp268.f2= Cyc_Tcutil_copy_types( _temp203); _temp268.f3=
0; _temp268;}); _temp267;}); _LL120: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp269=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp269[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp270; _temp270.tag= Cyc_Absyn_AnonStructType; _temp270.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp207); _temp270;}); _temp269;});
_LL122: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp271=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp271[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp272; _temp272.tag=
Cyc_Absyn_AnonUnionType; _temp272.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp209); _temp272;}); _temp271;}); _LL124: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp273=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp273[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp274; _temp274.tag= Cyc_Absyn_EnumType;
_temp274.f1= _temp213; _temp274.f2= _temp211; _temp274;}); _temp273;}); _LL126:
return( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp275=( struct Cyc_Absyn_AnonEnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct)); _temp275[ 0]=({
struct Cyc_Absyn_AnonEnumType_struct _temp276; _temp276.tag= Cyc_Absyn_AnonEnumType;
_temp276.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Enumfield*(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_enumfield,
_temp215); _temp276;}); _temp275;}); _LL128: return( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp277=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp277[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp278; _temp278.tag=
Cyc_Absyn_RgnHandleType; _temp278.f1=( void*) Cyc_Tcutil_copy_type( _temp217);
_temp278;}); _temp277;}); _LL130: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp279=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp279[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp280; _temp280.tag= Cyc_Absyn_TypedefType;
_temp280.f1= _temp221; _temp280.f2= Cyc_Tcutil_copy_types( _temp219); _temp280.f3=
0; _temp280;}); _temp279;}); _LL132: return t; _LL134: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp281=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp281[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp282; _temp282.tag= Cyc_Absyn_AccessEff; _temp282.f1=(
void*) Cyc_Tcutil_copy_type( _temp223); _temp282;}); _temp281;}); _LL136: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp283=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp283[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp284; _temp284.tag= Cyc_Absyn_JoinEff; _temp284.f1= Cyc_Tcutil_copy_types(
_temp225); _temp284;}); _temp283;}); _LL138: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp285=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp285[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp286; _temp286.tag= Cyc_Absyn_RgnsEff;
_temp286.f1=( void*) Cyc_Tcutil_copy_type( _temp227); _temp286;}); _temp285;});
_LL90:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){
return 1;}{ struct _tuple8 _temp288=({ struct _tuple8 _temp287; _temp287.f1= k1;
_temp287.f2= k2; _temp287;}); void* _temp298; void* _temp300; void* _temp302;
void* _temp304; void* _temp306; void* _temp308; _LL290: _LL301: _temp300=
_temp288.f1; if( _temp300 == ( void*) Cyc_Absyn_BoxKind){ goto _LL299;} else{
goto _LL292;} _LL299: _temp298= _temp288.f2; if( _temp298 == ( void*) Cyc_Absyn_MemKind){
goto _LL291;} else{ goto _LL292;} _LL292: _LL305: _temp304= _temp288.f1; if(
_temp304 == ( void*) Cyc_Absyn_BoxKind){ goto _LL303;} else{ goto _LL294;}
_LL303: _temp302= _temp288.f2; if( _temp302 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL293;} else{ goto _LL294;} _LL294: _LL309: _temp308= _temp288.f1; if( _temp308
== ( void*) Cyc_Absyn_MemKind){ goto _LL307;} else{ goto _LL296;} _LL307:
_temp306= _temp288.f2; if( _temp306 == ( void*) Cyc_Absyn_AnyKind){ goto _LL295;}
else{ goto _LL296;} _LL296: goto _LL297; _LL291: goto _LL293; _LL293: goto
_LL295; _LL295: return 1; _LL297: return 0; _LL289:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp310= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp368;
struct Cyc_Core_Opt* _temp370; struct Cyc_Absyn_Tvar* _temp372; void* _temp374;
struct Cyc_Absyn_TunionFieldInfo _temp376; void* _temp378; struct Cyc_Absyn_Tunionfield*
_temp380; struct Cyc_Absyn_TunionFieldInfo _temp382; void* _temp384; struct Cyc_Absyn_Structdecl**
_temp386; struct Cyc_Absyn_Uniondecl** _temp388; struct Cyc_Absyn_Enumdecl*
_temp390; struct Cyc_Absyn_Enumdecl* _temp392; struct Cyc_Absyn_PtrInfo _temp394;
struct Cyc_Core_Opt* _temp396; _LL312: if(( unsigned int) _temp310 >  3u?*(( int*)
_temp310) ==  Cyc_Absyn_Evar: 0){ _LL371: _temp370=(( struct Cyc_Absyn_Evar_struct*)
_temp310)->f1; goto _LL369; _LL369: _temp368=(( struct Cyc_Absyn_Evar_struct*)
_temp310)->f2; goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int)
_temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_VarType: 0){ _LL373: _temp372=((
struct Cyc_Absyn_VarType_struct*) _temp310)->f1; goto _LL315;} else{ goto _LL316;}
_LL316: if( _temp310 == ( void*) Cyc_Absyn_VoidType){ goto _LL317;} else{ goto
_LL318;} _LL318: if(( unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_IntType:
0){ _LL375: _temp374=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp310)->f2;
goto _LL319;} else{ goto _LL320;} _LL320: if( _temp310 == ( void*) Cyc_Absyn_FloatType){
goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp310 >  3u?*((
int*) _temp310) ==  Cyc_Absyn_DoubleType: 0){ goto _LL323;} else{ goto _LL324;}
_LL324: if(( unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_FnType:
0){ goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int) _temp310 >  3u?*((
int*) _temp310) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL327;} else{ goto _LL328;}
_LL328: if( _temp310 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL329;} else{ goto
_LL330;} _LL330: if(( unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_TunionType:
0){ goto _LL331;} else{ goto _LL332;} _LL332: if(( unsigned int) _temp310 >  3u?*((
int*) _temp310) ==  Cyc_Absyn_TunionFieldType: 0){ _LL377: _temp376=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp310)->f1; _LL379: _temp378=( void*)
_temp376.field_info; if(*(( int*) _temp378) ==  Cyc_Absyn_KnownTunionfield){
_LL381: _temp380=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp378)->f2;
goto _LL333;} else{ goto _LL334;}} else{ goto _LL334;} _LL334: if(( unsigned int)
_temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_TunionFieldType: 0){ _LL383:
_temp382=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp310)->f1; _LL385:
_temp384=( void*) _temp382.field_info; if(*(( int*) _temp384) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL335;} else{ goto _LL336;}} else{ goto _LL336;} _LL336: if(( unsigned int)
_temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_StructType: 0){ _LL387:
_temp386=(( struct Cyc_Absyn_StructType_struct*) _temp310)->f3; if( _temp386 == 
0){ goto _LL337;} else{ goto _LL338;}} else{ goto _LL338;} _LL338: if((
unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_UnionType: 0){
_LL389: _temp388=(( struct Cyc_Absyn_UnionType_struct*) _temp310)->f3; if(
_temp388 ==  0){ goto _LL339;} else{ goto _LL340;}} else{ goto _LL340;} _LL340:
if(( unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_EnumType: 0){
_LL391: _temp390=(( struct Cyc_Absyn_EnumType_struct*) _temp310)->f2; if(
_temp390 ==  0){ goto _LL341;} else{ goto _LL342;}} else{ goto _LL342;} _LL342:
if(( unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_StructType:
0){ goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int) _temp310 >  3u?*((
int*) _temp310) ==  Cyc_Absyn_UnionType: 0){ goto _LL345;} else{ goto _LL346;}
_LL346: if(( unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp310 >  3u?*((
int*) _temp310) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL349;} else{ goto _LL350;}
_LL350: if(( unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int) _temp310 >  3u?*((
int*) _temp310) ==  Cyc_Absyn_EnumType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_EnumType_struct*)
_temp310)->f2; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_PointerType: 0){ _LL395:
_temp394=(( struct Cyc_Absyn_PointerType_struct*) _temp310)->f1; goto _LL355;}
else{ goto _LL356;} _LL356: if(( unsigned int) _temp310 >  3u?*(( int*) _temp310)
==  Cyc_Absyn_ArrayType: 0){ goto _LL357;} else{ goto _LL358;} _LL358: if((
unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_TupleType: 0){
goto _LL359;} else{ goto _LL360;} _LL360: if(( unsigned int) _temp310 >  3u?*((
int*) _temp310) ==  Cyc_Absyn_TypedefType: 0){ _LL397: _temp396=(( struct Cyc_Absyn_TypedefType_struct*)
_temp310)->f3; goto _LL361;} else{ goto _LL362;} _LL362: if(( unsigned int)
_temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_AccessEff: 0){ goto _LL363;}
else{ goto _LL364;} _LL364: if(( unsigned int) _temp310 >  3u?*(( int*) _temp310)
==  Cyc_Absyn_JoinEff: 0){ goto _LL365;} else{ goto _LL366;} _LL366: if((
unsigned int) _temp310 >  3u?*(( int*) _temp310) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL367;} else{ goto _LL311;} _LL313: return( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp370))->v; _LL315: return Cyc_Absyn_conref_val( _temp372->kind);
_LL317: return( void*) Cyc_Absyn_MemKind; _LL319: return _temp374 == ( void*)
Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL321: goto
_LL323; _LL323: goto _LL325; _LL325: return( void*) Cyc_Absyn_MemKind; _LL327:
return( void*) Cyc_Absyn_BoxKind; _LL329: return( void*) Cyc_Absyn_RgnKind;
_LL331: return( void*) Cyc_Absyn_BoxKind; _LL333: if( _temp380->typs ==  0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL335: return({ void* _temp398[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u), _tag_arr( _temp398, sizeof( void*), 0u));});
_LL337: goto _LL339; _LL339: goto _LL341; _LL341: return( void*) Cyc_Absyn_AnyKind;
_LL343: goto _LL345; _LL345: goto _LL347; _LL347: goto _LL349; _LL349: goto
_LL351; _LL351: return( void*) Cyc_Absyn_MemKind; _LL353: if( _temp392->fields
==  0){ return( void*) Cyc_Absyn_AnyKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL355: { void* _temp399=( void*)( Cyc_Absyn_compress_conref( _temp394.bounds))->v;
void* _temp409; void* _temp411; _LL401: if(( unsigned int) _temp399 >  1u?*((
int*) _temp399) ==  Cyc_Absyn_Eq_constr: 0){ _LL410: _temp409=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp399)->f1; if( _temp409 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL402;} else{ goto _LL403;}} else{ goto _LL403;} _LL403: if(( unsigned int)
_temp399 >  1u?*(( int*) _temp399) ==  Cyc_Absyn_Eq_constr: 0){ _LL412: _temp411=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp399)->f1; if(( unsigned int)
_temp411 >  1u?*(( int*) _temp411) ==  Cyc_Absyn_Upper_b: 0){ goto _LL404;}
else{ goto _LL405;}} else{ goto _LL405;} _LL405: if( _temp399 == ( void*) Cyc_Absyn_No_constr){
goto _LL406;} else{ goto _LL407;} _LL407: if(( unsigned int) _temp399 >  1u?*((
int*) _temp399) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL408;} else{ goto
_LL400;} _LL402: return( void*) Cyc_Absyn_MemKind; _LL404: return( void*) Cyc_Absyn_BoxKind;
_LL406: return( void*) Cyc_Absyn_MemKind; _LL408: return({ void* _temp413[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds", sizeof(
unsigned char), 43u), _tag_arr( _temp413, sizeof( void*), 0u));}); _LL400:;}
_LL357: goto _LL359; _LL359: return( void*) Cyc_Absyn_MemKind; _LL361: return({
struct Cyc_Std_String_pa_struct _temp415; _temp415.tag= Cyc_Std_String_pa;
_temp415.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp414[ 1u]={&
_temp415}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp414, sizeof( void*), 1u));}}); _LL363: goto
_LL365; _LL365: goto _LL367; _LL367: return( void*) Cyc_Absyn_EffKind; _LL311:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp416; _push_handler(& _temp416);{
int _temp418= 0; if( setjmp( _temp416.handler)){ _temp418= 1;} if( ! _temp418){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp419= 1; _npop_handler( 0u); return
_temp419;}; _pop_handler();} else{ void* _temp417=( void*) _exn_thrown; void*
_temp421= _temp417; _LL423: if( _temp421 ==  Cyc_Tcutil_Unify){ goto _LL424;}
else{ goto _LL425;} _LL425: goto _LL426; _LL424: return 0; _LL426:( void) _throw(
_temp421); _LL422:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp427= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp463; struct Cyc_Core_Opt* _temp465; struct Cyc_Core_Opt**
_temp467; struct Cyc_Core_Opt* _temp468; struct Cyc_Absyn_PtrInfo _temp470; void*
_temp472; struct Cyc_Absyn_FnInfo _temp474; struct Cyc_List_List* _temp476;
struct Cyc_List_List* _temp478; struct Cyc_Absyn_VarargInfo* _temp480; int
_temp482; struct Cyc_List_List* _temp484; void* _temp486; struct Cyc_Core_Opt*
_temp488; struct Cyc_List_List* _temp490; struct Cyc_List_List* _temp492; void*
_temp494; struct Cyc_Absyn_TunionInfo _temp496; void* _temp498; struct Cyc_List_List*
_temp500; struct Cyc_Core_Opt* _temp502; struct Cyc_List_List* _temp504; struct
Cyc_Absyn_TunionFieldInfo _temp506; struct Cyc_List_List* _temp508; struct Cyc_List_List*
_temp510; struct Cyc_List_List* _temp512; struct Cyc_List_List* _temp514; void*
_temp516; struct Cyc_List_List* _temp518; void* _temp520; _LL429: if((
unsigned int) _temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_VarType: 0){
_LL464: _temp463=(( struct Cyc_Absyn_VarType_struct*) _temp427)->f1; goto _LL430;}
else{ goto _LL431;} _LL431: if(( unsigned int) _temp427 >  3u?*(( int*) _temp427)
==  Cyc_Absyn_Evar: 0){ _LL469: _temp468=(( struct Cyc_Absyn_Evar_struct*)
_temp427)->f2; goto _LL466; _LL466: _temp465=(( struct Cyc_Absyn_Evar_struct*)
_temp427)->f4; _temp467=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp427)->f4; goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int)
_temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_PointerType: 0){ _LL471:
_temp470=(( struct Cyc_Absyn_PointerType_struct*) _temp427)->f1; goto _LL434;}
else{ goto _LL435;} _LL435: if(( unsigned int) _temp427 >  3u?*(( int*) _temp427)
==  Cyc_Absyn_ArrayType: 0){ _LL473: _temp472=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp427)->f1; goto _LL436;} else{ goto _LL437;} _LL437: if(( unsigned int)
_temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_FnType: 0){ _LL475: _temp474=((
struct Cyc_Absyn_FnType_struct*) _temp427)->f1; _LL491: _temp490= _temp474.tvars;
goto _LL489; _LL489: _temp488= _temp474.effect; goto _LL487; _LL487: _temp486=(
void*) _temp474.ret_typ; goto _LL485; _LL485: _temp484= _temp474.args; goto
_LL483; _LL483: _temp482= _temp474.c_varargs; goto _LL481; _LL481: _temp480=
_temp474.cyc_varargs; goto _LL479; _LL479: _temp478= _temp474.rgn_po; goto
_LL477; _LL477: _temp476= _temp474.attributes; goto _LL438;} else{ goto _LL439;}
_LL439: if(( unsigned int) _temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_TupleType:
0){ _LL493: _temp492=(( struct Cyc_Absyn_TupleType_struct*) _temp427)->f1; goto
_LL440;} else{ goto _LL441;} _LL441: if(( unsigned int) _temp427 >  3u?*(( int*)
_temp427) ==  Cyc_Absyn_RgnHandleType: 0){ _LL495: _temp494=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp427)->f1; goto _LL442;} else{ goto _LL443;}
_LL443: if(( unsigned int) _temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_TunionType:
0){ _LL497: _temp496=(( struct Cyc_Absyn_TunionType_struct*) _temp427)->f1;
_LL501: _temp500= _temp496.targs; goto _LL499; _LL499: _temp498=( void*)
_temp496.rgn; goto _LL444;} else{ goto _LL445;} _LL445: if(( unsigned int)
_temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_TypedefType: 0){ _LL505:
_temp504=(( struct Cyc_Absyn_TypedefType_struct*) _temp427)->f2; goto _LL503;
_LL503: _temp502=(( struct Cyc_Absyn_TypedefType_struct*) _temp427)->f3; goto
_LL446;} else{ goto _LL447;} _LL447: if(( unsigned int) _temp427 >  3u?*(( int*)
_temp427) ==  Cyc_Absyn_TunionFieldType: 0){ _LL507: _temp506=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp427)->f1; _LL509: _temp508= _temp506.targs; goto _LL448;} else{ goto _LL449;}
_LL449: if(( unsigned int) _temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_StructType:
0){ _LL511: _temp510=(( struct Cyc_Absyn_StructType_struct*) _temp427)->f2; goto
_LL450;} else{ goto _LL451;} _LL451: if(( unsigned int) _temp427 >  3u?*(( int*)
_temp427) ==  Cyc_Absyn_AnonStructType: 0){ _LL513: _temp512=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp427)->f1; goto _LL452;} else{ goto _LL453;} _LL453: if(( unsigned int)
_temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_AnonUnionType: 0){ _LL515:
_temp514=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp427)->f1; goto _LL454;}
else{ goto _LL455;} _LL455: if(( unsigned int) _temp427 >  3u?*(( int*) _temp427)
==  Cyc_Absyn_AccessEff: 0){ _LL517: _temp516=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp427)->f1; goto _LL456;} else{ goto _LL457;} _LL457: if(( unsigned int)
_temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_JoinEff: 0){ _LL519: _temp518=((
struct Cyc_Absyn_JoinEff_struct*) _temp427)->f1; goto _LL458;} else{ goto _LL459;}
_LL459: if(( unsigned int) _temp427 >  3u?*(( int*) _temp427) ==  Cyc_Absyn_RgnsEff:
0){ _LL521: _temp520=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp427)->f1;
goto _LL460;} else{ goto _LL461;} _LL461: goto _LL462; _LL430: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp463)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);} goto
_LL428; _LL432: if( t ==  evar){ Cyc_Tcutil_failure_reason= _tag_arr("(occurs check)",
sizeof( unsigned char), 15u);( int) _throw(( void*) Cyc_Tcutil_Unify);} else{
if( _temp468 !=  0){ Cyc_Tcutil_occurs( evar, env,( void*) _temp468->v);} else{
int problem= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp467))->v; for( 0; s !=  0; s= s->tl){ if( !(( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*) s->hd)){ problem= 1; break;}}} if( problem){ struct Cyc_List_List*
_temp522= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp467))->v; for( 0; s !=  0; s= s->tl){ if((( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*) s->hd)){ _temp522=({ struct Cyc_List_List* _temp523=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp523->hd=(
void*)(( struct Cyc_Absyn_Tvar*) s->hd); _temp523->tl= _temp522; _temp523;});}}}*
_temp467=({ struct Cyc_Core_Opt* _temp524=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp524->v=( void*) _temp522; _temp524;});}}}
goto _LL428; _LL434: Cyc_Tcutil_occurs( evar, env,( void*) _temp470.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp470.rgn_typ); goto _LL428; _LL436:
Cyc_Tcutil_occurs( evar, env, _temp472); goto _LL428; _LL438: if( _temp488 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*) _temp488->v);} Cyc_Tcutil_occurs( evar,
env, _temp486); for( 0; _temp484 !=  0; _temp484= _temp484->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*) _temp484->hd)).f3);} if( _temp480 !=  0){ void*
_temp527; struct Cyc_Absyn_VarargInfo _temp525=* _temp480; _LL528: _temp527=(
void*) _temp525.type; goto _LL526; _LL526: Cyc_Tcutil_occurs( evar, env,
_temp527);} for( 0; _temp478 !=  0; _temp478= _temp478->tl){ struct _tuple8
_temp531; void* _temp532; void* _temp534; struct _tuple8* _temp529=( struct
_tuple8*) _temp478->hd; _temp531=* _temp529; _LL535: _temp534= _temp531.f1; goto
_LL533; _LL533: _temp532= _temp531.f2; goto _LL530; _LL530: Cyc_Tcutil_occurs(
evar, env, _temp534); Cyc_Tcutil_occurs( evar, env, _temp532);} goto _LL428;
_LL440: for( 0; _temp492 !=  0; _temp492= _temp492->tl){ Cyc_Tcutil_occurs( evar,
env,(*(( struct _tuple4*) _temp492->hd)).f2);} goto _LL428; _LL442: Cyc_Tcutil_occurs(
evar, env, _temp494); goto _LL428; _LL444: Cyc_Tcutil_occurs( evar, env,
_temp498); Cyc_Tcutil_occurslist( evar, env, _temp500); goto _LL428; _LL446:
_temp508= _temp504; goto _LL448; _LL448: _temp510= _temp508; goto _LL450; _LL450:
Cyc_Tcutil_occurslist( evar, env, _temp510); goto _LL428; _LL452: _temp514=
_temp512; goto _LL454; _LL454: for( 0; _temp514 !=  0; _temp514= _temp514->tl){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*) _temp514->hd)->type);}
goto _LL428; _LL456: Cyc_Tcutil_occurs( evar, env, _temp516); goto _LL428;
_LL458: Cyc_Tcutil_occurslist( evar, env, _temp518); goto _LL428; _LL460: Cyc_Tcutil_occurs(
evar, env, _temp520); goto _LL428; _LL462: goto _LL428; _LL428:;} static void
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
y); if( x ==  y){ return;}{ void* _temp536=( void*) x->v; void* _temp544; _LL538:
if( _temp536 == ( void*) Cyc_Absyn_No_constr){ goto _LL539;} else{ goto _LL540;}
_LL540: if(( unsigned int) _temp536 >  1u?*(( int*) _temp536) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int) _temp536 >  1u?*((
int*) _temp536) ==  Cyc_Absyn_Eq_constr: 0){ _LL545: _temp544=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp536)->f1; goto _LL543;} else{ goto _LL537;}
_LL539:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp546=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp546[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp547; _temp547.tag= Cyc_Absyn_Forward_constr; _temp547.f1= y; _temp547;});
_temp546;}))); return; _LL541:( int) _throw(({ void* _temp548[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp548, sizeof( void*), 0u));})); _LL543: { void* _temp549=( void*)
y->v; void* _temp557; _LL551: if( _temp549 == ( void*) Cyc_Absyn_No_constr){
goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int) _temp549 >  1u?*((
int*) _temp549) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL554;} else{ goto
_LL555;} _LL555: if(( unsigned int) _temp549 >  1u?*(( int*) _temp549) ==  Cyc_Absyn_Eq_constr:
0){ _LL558: _temp557=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp549)->f1;
goto _LL556;} else{ goto _LL550;} _LL552:( void*)( y->v=( void*)(( void*) x->v));
return; _LL554:( int) _throw(({ void* _temp559[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp559, sizeof( void*), 0u));})); _LL556: if( cmp( _temp544,
_temp557) !=  0){ Cyc_Tcutil_failure_reason= reason;( int) _throw(( void*) Cyc_Tcutil_Unify);}
return; _LL550:;} _LL537:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp560; _push_handler(& _temp560);{ int _temp562= 0; if( setjmp(
_temp560.handler)){ _temp562= 1;} if( ! _temp562){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u));{ int _temp563= 1;
_npop_handler( 0u); return _temp563;}; _pop_handler();} else{ void* _temp561=(
void*) _exn_thrown; void* _temp565= _temp561; _LL567: if( _temp565 ==  Cyc_Tcutil_Unify){
goto _LL568;} else{ goto _LL569;} _LL569: goto _LL570; _LL568: return 0; _LL570:(
void) _throw( _temp565); _LL566:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp572=({ struct _tuple8 _temp571; _temp571.f1= b1;
_temp571.f2= b2; _temp571;}); void* _temp582; void* _temp584; void* _temp586;
void* _temp588; void* _temp590; struct Cyc_Absyn_Exp* _temp592; void* _temp594;
struct Cyc_Absyn_Exp* _temp596; _LL574: _LL585: _temp584= _temp572.f1; if(
_temp584 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL583;} else{ goto _LL576;}
_LL583: _temp582= _temp572.f2; if( _temp582 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL575;} else{ goto _LL576;} _LL576: _LL587: _temp586= _temp572.f1; if(
_temp586 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL577;} else{ goto _LL578;}
_LL578: _LL589: _temp588= _temp572.f2; if( _temp588 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL579;} else{ goto _LL580;} _LL580: _LL595: _temp594= _temp572.f1; if((
unsigned int) _temp594 >  1u?*(( int*) _temp594) ==  Cyc_Absyn_Upper_b: 0){
_LL597: _temp596=(( struct Cyc_Absyn_Upper_b_struct*) _temp594)->f1; goto _LL591;}
else{ goto _LL573;} _LL591: _temp590= _temp572.f2; if(( unsigned int) _temp590 > 
1u?*(( int*) _temp590) ==  Cyc_Absyn_Upper_b: 0){ _LL593: _temp592=(( struct Cyc_Absyn_Upper_b_struct*)
_temp590)->f1; goto _LL581;} else{ goto _LL573;} _LL575: return 0; _LL577:
return - 1; _LL579: return 1; _LL581: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp596); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp592); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL573:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp599=({
struct _tuple8 _temp598; _temp598.f1= a1; _temp598.f2= a2; _temp598;}); void*
_temp611; int _temp613; int _temp615; void* _temp617; void* _temp619; int
_temp621; int _temp623; void* _temp625; void* _temp627; int _temp629; void*
_temp631; int _temp633; void* _temp635; int _temp637; void* _temp639; int
_temp641; void* _temp643; struct _tagged_arr _temp645; void* _temp647; struct
_tagged_arr _temp649; _LL601: _LL620: _temp619= _temp599.f1; if(( unsigned int)
_temp619 >  16u?*(( int*) _temp619) ==  Cyc_Absyn_Format_att: 0){ _LL626:
_temp625=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp619)->f1; goto
_LL624; _LL624: _temp623=(( struct Cyc_Absyn_Format_att_struct*) _temp619)->f2;
goto _LL622; _LL622: _temp621=(( struct Cyc_Absyn_Format_att_struct*) _temp619)->f3;
goto _LL612;} else{ goto _LL603;} _LL612: _temp611= _temp599.f2; if((
unsigned int) _temp611 >  16u?*(( int*) _temp611) ==  Cyc_Absyn_Format_att: 0){
_LL618: _temp617=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp611)->f1;
goto _LL616; _LL616: _temp615=(( struct Cyc_Absyn_Format_att_struct*) _temp611)->f2;
goto _LL614; _LL614: _temp613=(( struct Cyc_Absyn_Format_att_struct*) _temp611)->f3;
goto _LL602;} else{ goto _LL603;} _LL603: _LL632: _temp631= _temp599.f1; if((
unsigned int) _temp631 >  16u?*(( int*) _temp631) ==  Cyc_Absyn_Regparm_att: 0){
_LL634: _temp633=(( struct Cyc_Absyn_Regparm_att_struct*) _temp631)->f1; goto
_LL628;} else{ goto _LL605;} _LL628: _temp627= _temp599.f2; if(( unsigned int)
_temp627 >  16u?*(( int*) _temp627) ==  Cyc_Absyn_Regparm_att: 0){ _LL630:
_temp629=(( struct Cyc_Absyn_Regparm_att_struct*) _temp627)->f1; goto _LL604;}
else{ goto _LL605;} _LL605: _LL640: _temp639= _temp599.f1; if(( unsigned int)
_temp639 >  16u?*(( int*) _temp639) ==  Cyc_Absyn_Aligned_att: 0){ _LL642:
_temp641=(( struct Cyc_Absyn_Aligned_att_struct*) _temp639)->f1; goto _LL636;}
else{ goto _LL607;} _LL636: _temp635= _temp599.f2; if(( unsigned int) _temp635 > 
16u?*(( int*) _temp635) ==  Cyc_Absyn_Aligned_att: 0){ _LL638: _temp637=((
struct Cyc_Absyn_Aligned_att_struct*) _temp635)->f1; goto _LL606;} else{ goto
_LL607;} _LL607: _LL648: _temp647= _temp599.f1; if(( unsigned int) _temp647 > 
16u?*(( int*) _temp647) ==  Cyc_Absyn_Section_att: 0){ _LL650: _temp649=((
struct Cyc_Absyn_Section_att_struct*) _temp647)->f1; goto _LL644;} else{ goto
_LL609;} _LL644: _temp643= _temp599.f2; if(( unsigned int) _temp643 >  16u?*((
int*) _temp643) ==  Cyc_Absyn_Section_att: 0){ _LL646: _temp645=(( struct Cyc_Absyn_Section_att_struct*)
_temp643)->f1; goto _LL608;} else{ goto _LL609;} _LL609: goto _LL610; _LL602:
return( _temp625 ==  _temp617? _temp623 ==  _temp615: 0)? _temp621 ==  _temp613:
0; _LL604: _temp641= _temp633; _temp637= _temp629; goto _LL606; _LL606: return
_temp641 ==  _temp637; _LL608: return Cyc_Std_strcmp( _temp649, _temp645) ==  0;
_LL610: return 0; _LL600:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a= a->tl){
if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*) a->hd, a2)){ return 0;}}}{
struct Cyc_List_List* a= a2; for( 0; a !=  0; a= a->tl){ if( ! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,( void*) a->hd, a1)){ return 0;}}} return 1;} static void*
Cyc_Tcutil_normalize_effect( void* e); static void* Cyc_Tcutil_rgns_of( void* t);
static void* Cyc_Tcutil_rgns_of_field( struct Cyc_Absyn_Structfield* sf){ return
Cyc_Tcutil_rgns_of(( void*) sf->type);} static struct _tuple5* Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar* tv){ void* t;{ void* _temp651= Cyc_Absyn_conref_val( tv->kind);
_LL653: if( _temp651 == ( void*) Cyc_Absyn_RgnKind){ goto _LL654;} else{ goto
_LL655;} _LL655: if( _temp651 == ( void*) Cyc_Absyn_EffKind){ goto _LL656;}
else{ goto _LL657;} _LL657: goto _LL658; _LL654: t=( void*) Cyc_Absyn_HeapRgn;
goto _LL652; _LL656: t= Cyc_Absyn_empty_effect; goto _LL652; _LL658: t= Cyc_Absyn_sint_t;
goto _LL652; _LL652:;} return({ struct _tuple5* _temp659=( struct _tuple5*)
_cycalloc( sizeof( struct _tuple5)); _temp659->f1= tv; _temp659->f2= t; _temp659;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp660= Cyc_Tcutil_compress(
t); void* _temp710; struct Cyc_Absyn_TunionInfo _temp712; void* _temp714; struct
Cyc_List_List* _temp716; struct Cyc_Absyn_PtrInfo _temp718; void* _temp720; void*
_temp722; void* _temp724; struct Cyc_List_List* _temp726; struct Cyc_Absyn_TunionFieldInfo
_temp728; struct Cyc_List_List* _temp730; struct Cyc_List_List* _temp732; struct
Cyc_List_List* _temp734; struct Cyc_List_List* _temp736; struct Cyc_List_List*
_temp738; struct Cyc_Absyn_FnInfo _temp740; struct Cyc_List_List* _temp742;
struct Cyc_Absyn_VarargInfo* _temp744; struct Cyc_List_List* _temp746; void*
_temp748; struct Cyc_Core_Opt* _temp750; struct Cyc_List_List* _temp752; void*
_temp754; _LL662: if( _temp660 == ( void*) Cyc_Absyn_VoidType){ goto _LL663;}
else{ goto _LL664;} _LL664: if( _temp660 == ( void*) Cyc_Absyn_FloatType){ goto
_LL665;} else{ goto _LL666;} _LL666: if(( unsigned int) _temp660 >  3u?*(( int*)
_temp660) ==  Cyc_Absyn_DoubleType: 0){ goto _LL667;} else{ goto _LL668;} _LL668:
if(( unsigned int) _temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_EnumType: 0){
goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int) _temp660 >  3u?*((
int*) _temp660) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL671;} else{ goto _LL672;}
_LL672: if(( unsigned int) _temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_IntType:
0){ goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int) _temp660 >  3u?*((
int*) _temp660) ==  Cyc_Absyn_Evar: 0){ goto _LL675;} else{ goto _LL676;} _LL676:
if(( unsigned int) _temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_VarType: 0){
goto _LL677;} else{ goto _LL678;} _LL678: if(( unsigned int) _temp660 >  3u?*((
int*) _temp660) ==  Cyc_Absyn_RgnHandleType: 0){ _LL711: _temp710=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp660)->f1; goto _LL679;} else{ goto
_LL680;} _LL680: if(( unsigned int) _temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_TunionType:
0){ _LL713: _temp712=(( struct Cyc_Absyn_TunionType_struct*) _temp660)->f1;
_LL717: _temp716= _temp712.targs; goto _LL715; _LL715: _temp714=( void*)
_temp712.rgn; goto _LL681;} else{ goto _LL682;} _LL682: if(( unsigned int)
_temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_PointerType: 0){ _LL719:
_temp718=(( struct Cyc_Absyn_PointerType_struct*) _temp660)->f1; _LL723:
_temp722=( void*) _temp718.elt_typ; goto _LL721; _LL721: _temp720=( void*)
_temp718.rgn_typ; goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_ArrayType: 0){ _LL725: _temp724=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp660)->f1; goto _LL685;} else{
goto _LL686;} _LL686: if(( unsigned int) _temp660 >  3u?*(( int*) _temp660) == 
Cyc_Absyn_TupleType: 0){ _LL727: _temp726=(( struct Cyc_Absyn_TupleType_struct*)
_temp660)->f1; goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int)
_temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_TunionFieldType: 0){ _LL729:
_temp728=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp660)->f1; _LL731:
_temp730= _temp728.targs; goto _LL689;} else{ goto _LL690;} _LL690: if((
unsigned int) _temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_StructType: 0){
_LL733: _temp732=(( struct Cyc_Absyn_StructType_struct*) _temp660)->f2; goto
_LL691;} else{ goto _LL692;} _LL692: if(( unsigned int) _temp660 >  3u?*(( int*)
_temp660) ==  Cyc_Absyn_UnionType: 0){ _LL735: _temp734=(( struct Cyc_Absyn_UnionType_struct*)
_temp660)->f2; goto _LL693;} else{ goto _LL694;} _LL694: if(( unsigned int)
_temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_AnonStructType: 0){ _LL737:
_temp736=(( struct Cyc_Absyn_AnonStructType_struct*) _temp660)->f1; goto _LL695;}
else{ goto _LL696;} _LL696: if(( unsigned int) _temp660 >  3u?*(( int*) _temp660)
==  Cyc_Absyn_AnonUnionType: 0){ _LL739: _temp738=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp660)->f1; goto _LL697;} else{ goto _LL698;} _LL698: if(( unsigned int)
_temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_FnType: 0){ _LL741: _temp740=((
struct Cyc_Absyn_FnType_struct*) _temp660)->f1; _LL753: _temp752= _temp740.tvars;
goto _LL751; _LL751: _temp750= _temp740.effect; goto _LL749; _LL749: _temp748=(
void*) _temp740.ret_typ; goto _LL747; _LL747: _temp746= _temp740.args; goto
_LL745; _LL745: _temp744= _temp740.cyc_varargs; goto _LL743; _LL743: _temp742=
_temp740.rgn_po; goto _LL699;} else{ goto _LL700;} _LL700: if( _temp660 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL701;} else{ goto _LL702;} _LL702: if((
unsigned int) _temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_AccessEff: 0){
goto _LL703;} else{ goto _LL704;} _LL704: if(( unsigned int) _temp660 >  3u?*((
int*) _temp660) ==  Cyc_Absyn_JoinEff: 0){ goto _LL705;} else{ goto _LL706;}
_LL706: if(( unsigned int) _temp660 >  3u?*(( int*) _temp660) ==  Cyc_Absyn_RgnsEff:
0){ _LL755: _temp754=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp660)->f1;
goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int) _temp660 >  3u?*((
int*) _temp660) ==  Cyc_Absyn_TypedefType: 0){ goto _LL709;} else{ goto _LL661;}
_LL663: goto _LL665; _LL665: goto _LL667; _LL667: goto _LL669; _LL669: goto
_LL671; _LL671: goto _LL673; _LL673: return Cyc_Absyn_empty_effect; _LL675: goto
_LL677; _LL677: { void* _temp756= Cyc_Tcutil_typ_kind( t); _LL758: if( _temp756
== ( void*) Cyc_Absyn_RgnKind){ goto _LL759;} else{ goto _LL760;} _LL760: if(
_temp756 == ( void*) Cyc_Absyn_EffKind){ goto _LL761;} else{ goto _LL762;}
_LL762: goto _LL763; _LL759: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp764=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp764[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp765; _temp765.tag= Cyc_Absyn_AccessEff;
_temp765.f1=( void*) t; _temp765;}); _temp764;}); _LL761: return t; _LL763:
return( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp766=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp766[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp767; _temp767.tag= Cyc_Absyn_RgnsEff; _temp767.f1=( void*) t; _temp767;});
_temp766;}); _LL757:;} _LL679: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp768=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp768[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp769; _temp769.tag= Cyc_Absyn_AccessEff;
_temp769.f1=( void*) _temp710; _temp769;}); _temp768;}); _LL681: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp772=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp772->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp773=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp773[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp774; _temp774.tag= Cyc_Absyn_AccessEff;
_temp774.f1=( void*) _temp714; _temp774;}); _temp773;})); _temp772->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp716); _temp772;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp770=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp770[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp771; _temp771.tag= Cyc_Absyn_JoinEff; _temp771.f1= ts; _temp771;});
_temp770;}));} _LL683: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp775=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp775[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp776; _temp776.tag= Cyc_Absyn_JoinEff;
_temp776.f1=({ void* _temp777[ 2u]; _temp777[ 1u]= Cyc_Tcutil_rgns_of( _temp722);
_temp777[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp778=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp778[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp779; _temp779.tag= Cyc_Absyn_AccessEff;
_temp779.f1=( void*) _temp720; _temp779;}); _temp778;}); Cyc_List_list( _tag_arr(
_temp777, sizeof( void*), 2u));}); _temp776;}); _temp775;})); _LL685: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp724)); _LL687: { struct Cyc_List_List* _temp780= 0;
for( 0; _temp726 !=  0; _temp726= _temp726->tl){ _temp780=({ struct Cyc_List_List*
_temp781=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp781->hd=( void*)(*(( struct _tuple4*) _temp726->hd)).f2; _temp781->tl=
_temp780; _temp781;});} _temp730= _temp780; goto _LL689;} _LL689: _temp732=
_temp730; goto _LL691; _LL691: _temp734= _temp732; goto _LL693; _LL693: return
Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp782=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp782[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp783; _temp783.tag= Cyc_Absyn_JoinEff;
_temp783.f1= Cyc_List_map( Cyc_Tcutil_rgns_of, _temp734); _temp783;}); _temp782;}));
_LL695: _temp738= _temp736; goto _LL697; _LL697: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp784=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp784[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp785; _temp785.tag= Cyc_Absyn_JoinEff; _temp785.f1=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_rgns_of_field, _temp738); _temp785;}); _temp784;})); _LL699: { void*
_temp786= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)( struct _tuple5*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst,
_temp752),( void*)(( struct Cyc_Core_Opt*) _check_null( _temp750))->v); return
Cyc_Tcutil_normalize_effect( _temp786);} _LL701: return Cyc_Absyn_empty_effect;
_LL703: goto _LL705; _LL705: return t; _LL707: return Cyc_Tcutil_rgns_of(
_temp754); _LL709: return({ void* _temp787[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp787, sizeof( void*), 0u));});
_LL661:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp788= e; struct Cyc_List_List* _temp796; struct Cyc_List_List**
_temp798; void* _temp799; _LL790: if(( unsigned int) _temp788 >  3u?*(( int*)
_temp788) ==  Cyc_Absyn_JoinEff: 0){ _LL797: _temp796=(( struct Cyc_Absyn_JoinEff_struct*)
_temp788)->f1; _temp798=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp788)->f1; goto _LL791;} else{ goto _LL792;} _LL792: if(( unsigned int)
_temp788 >  3u?*(( int*) _temp788) ==  Cyc_Absyn_RgnsEff: 0){ _LL800: _temp799=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp788)->f1; goto _LL793;} else{
goto _LL794;} _LL794: goto _LL795; _LL791: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp798; for( 0; effs !=  0; effs= effs->tl){ void* _temp801=( void*)
effs->hd;( void*)( effs->hd=( void*) Cyc_Tcutil_compress( Cyc_Tcutil_normalize_effect(
_temp801)));{ void* _temp802=( void*) effs->hd; _LL804: if(( unsigned int)
_temp802 >  3u?*(( int*) _temp802) ==  Cyc_Absyn_JoinEff: 0){ goto _LL805;}
else{ goto _LL806;} _LL806: goto _LL807; _LL805: nested_join= 1; goto _LL803;
_LL807: goto _LL803; _LL803:;}}} if( ! nested_join){ return e;}{ struct Cyc_List_List*
effects= 0;{ struct Cyc_List_List* effs=* _temp798; for( 0; effs !=  0; effs=
effs->tl){ void* _temp808= Cyc_Tcutil_compress(( void*) effs->hd); struct Cyc_List_List*
_temp816; void* _temp818; _LL810: if(( unsigned int) _temp808 >  3u?*(( int*)
_temp808) ==  Cyc_Absyn_JoinEff: 0){ _LL817: _temp816=(( struct Cyc_Absyn_JoinEff_struct*)
_temp808)->f1; goto _LL811;} else{ goto _LL812;} _LL812: if(( unsigned int)
_temp808 >  3u?*(( int*) _temp808) ==  Cyc_Absyn_AccessEff: 0){ _LL819: _temp818=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp808)->f1; if( _temp818 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL813;} else{ goto _LL814;}} else{ goto _LL814;}
_LL814: goto _LL815; _LL811: effects= Cyc_List_revappend( _temp816, effects);
goto _LL809; _LL813: goto _LL809; _LL815: effects=({ struct Cyc_List_List*
_temp820=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp820->hd=( void*) _temp808; _temp820->tl= effects; _temp820;}); goto _LL809;
_LL809:;}}* _temp798= Cyc_List_imp_rev( effects); return e;}} _LL793: return Cyc_Tcutil_rgns_of(
_temp799); _LL795: return e; _LL789:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp821= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp829; struct Cyc_List_List _temp831; struct Cyc_List_List* _temp832; void*
_temp834; struct Cyc_Core_Opt* _temp836; struct Cyc_Core_Opt* _temp838; _LL823:
if(( unsigned int) _temp821 >  3u?*(( int*) _temp821) ==  Cyc_Absyn_JoinEff: 0){
_LL830: _temp829=(( struct Cyc_Absyn_JoinEff_struct*) _temp821)->f1; if(
_temp829 ==  0){ goto _LL825;} else{ _temp831=* _temp829; _LL835: _temp834=(
void*) _temp831.hd; goto _LL833; _LL833: _temp832= _temp831.tl; goto _LL824;}}
else{ goto _LL825;} _LL825: if(( unsigned int) _temp821 >  3u?*(( int*) _temp821)
==  Cyc_Absyn_Evar: 0){ _LL839: _temp838=(( struct Cyc_Absyn_Evar_struct*)
_temp821)->f1; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_Evar_struct*)
_temp821)->f4; goto _LL826;} else{ goto _LL827;} _LL827: goto _LL828; _LL824: {
void* _temp840= Cyc_Tcutil_compress( _temp834); struct Cyc_Core_Opt* _temp846;
_LL842: if(( unsigned int) _temp840 >  3u?*(( int*) _temp840) ==  Cyc_Absyn_Evar:
0){ _LL847: _temp846=(( struct Cyc_Absyn_Evar_struct*) _temp840)->f4; goto
_LL843;} else{ goto _LL844;} _LL844: goto _LL845; _LL843: return({ struct
_tuple9* _temp848=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp848->f1= _temp834; _temp848->f2= _temp832; _temp848->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp846))->v; _temp848;}); _LL845: return 0;
_LL841:;} _LL826: if( _temp838 ==  0? 1:( void*) _temp838->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp849[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp849, sizeof( void*), 0u));});} return({ struct _tuple9*
_temp850=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp850->f1= t;
_temp850->f2= 0; _temp850->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp836))->v; _temp850;}); _LL828: return 0; _LL822:;} static
struct Cyc_Core_Opt Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp851=({ struct Cyc_Absyn_FnType_struct*
_temp852=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp852[ 0]=({ struct Cyc_Absyn_FnType_struct _temp853; _temp853.tag= Cyc_Absyn_FnType;
_temp853.f1=({ struct Cyc_Absyn_FnInfo _temp854; _temp854.tvars= 0; _temp854.effect=({
struct Cyc_Core_Opt* _temp855=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp855->v=( void*) eff; _temp855;}); _temp854.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp854.args= 0; _temp854.c_varargs= 0; _temp854.cyc_varargs=
0; _temp854.rgn_po= 0; _temp854.attributes= 0; _temp854;}); _temp853;});
_temp852;}); return Cyc_Absyn_atb_typ(( void*) _temp851,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp856= Cyc_Tcutil_compress( e); void*
_temp868; struct Cyc_List_List* _temp870; void* _temp872; struct Cyc_Core_Opt*
_temp874; struct Cyc_Core_Opt* _temp876; struct Cyc_Core_Opt** _temp878; struct
Cyc_Core_Opt* _temp879; _LL858: if(( unsigned int) _temp856 >  3u?*(( int*)
_temp856) ==  Cyc_Absyn_AccessEff: 0){ _LL869: _temp868=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp856)->f1; goto _LL859;} else{ goto _LL860;} _LL860: if(( unsigned int)
_temp856 >  3u?*(( int*) _temp856) ==  Cyc_Absyn_JoinEff: 0){ _LL871: _temp870=((
struct Cyc_Absyn_JoinEff_struct*) _temp856)->f1; goto _LL861;} else{ goto _LL862;}
_LL862: if(( unsigned int) _temp856 >  3u?*(( int*) _temp856) ==  Cyc_Absyn_RgnsEff:
0){ _LL873: _temp872=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp856)->f1;
goto _LL863;} else{ goto _LL864;} _LL864: if(( unsigned int) _temp856 >  3u?*((
int*) _temp856) ==  Cyc_Absyn_Evar: 0){ _LL880: _temp879=(( struct Cyc_Absyn_Evar_struct*)
_temp856)->f1; goto _LL877; _LL877: _temp876=(( struct Cyc_Absyn_Evar_struct*)
_temp856)->f2; _temp878=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp856)->f2; goto _LL875; _LL875: _temp874=(( struct Cyc_Absyn_Evar_struct*)
_temp856)->f4; goto _LL865;} else{ goto _LL866;} _LL866: goto _LL867; _LL859:
if( constrain){ return Cyc_Tcutil_unify( r, _temp868);} _temp868= Cyc_Tcutil_compress(
_temp868); if( r ==  _temp868){ return 1;}{ struct _tuple8 _temp882=({ struct
_tuple8 _temp881; _temp881.f1= r; _temp881.f2= _temp868; _temp881;}); void*
_temp888; struct Cyc_Absyn_Tvar* _temp890; void* _temp892; struct Cyc_Absyn_Tvar*
_temp894; _LL884: _LL893: _temp892= _temp882.f1; if(( unsigned int) _temp892 > 
3u?*(( int*) _temp892) ==  Cyc_Absyn_VarType: 0){ _LL895: _temp894=(( struct Cyc_Absyn_VarType_struct*)
_temp892)->f1; goto _LL889;} else{ goto _LL886;} _LL889: _temp888= _temp882.f2;
if(( unsigned int) _temp888 >  3u?*(( int*) _temp888) ==  Cyc_Absyn_VarType: 0){
_LL891: _temp890=(( struct Cyc_Absyn_VarType_struct*) _temp888)->f1; goto _LL885;}
else{ goto _LL886;} _LL886: goto _LL887; _LL885: return Cyc_Absyn_tvar_cmp(
_temp894, _temp890) ==  0; _LL887: return 0; _LL883:;} _LL861: for( 0; _temp870
!=  0; _temp870= _temp870->tl){ if( Cyc_Tcutil_region_in_effect( constrain, r,(
void*) _temp870->hd)){ return 1;}} return 0; _LL863: { void* _temp896= Cyc_Tcutil_rgns_of(
_temp872); void* _temp902; _LL898: if(( unsigned int) _temp896 >  3u?*(( int*)
_temp896) ==  Cyc_Absyn_RgnsEff: 0){ _LL903: _temp902=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp896)->f1; goto _LL899;} else{ goto _LL900;} _LL900: goto _LL901; _LL899:
if( ! constrain){ return 0;}{ void* _temp904= Cyc_Tcutil_compress( _temp902);
struct Cyc_Core_Opt* _temp910; struct Cyc_Core_Opt* _temp912; struct Cyc_Core_Opt**
_temp914; struct Cyc_Core_Opt* _temp915; _LL906: if(( unsigned int) _temp904 > 
3u?*(( int*) _temp904) ==  Cyc_Absyn_Evar: 0){ _LL916: _temp915=(( struct Cyc_Absyn_Evar_struct*)
_temp904)->f1; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_Evar_struct*)
_temp904)->f2; _temp914=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp904)->f2; goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_Evar_struct*)
_temp904)->f4; goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907: {
void* _temp917= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp910); Cyc_Tcutil_occurs( _temp917,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp910))->v, r);{ void* _temp918= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp920=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp920[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp921; _temp921.tag= Cyc_Absyn_JoinEff; _temp921.f1=({ void* _temp922[ 2u];
_temp922[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp923=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp923[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp924; _temp924.tag= Cyc_Absyn_AccessEff;
_temp924.f1=( void*) r; _temp924;}); _temp923;}); _temp922[ 0u]= _temp917; Cyc_List_list(
_tag_arr( _temp922, sizeof( void*), 2u));}); _temp921;}); _temp920;}));*
_temp914=({ struct Cyc_Core_Opt* _temp919=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp919->v=( void*) _temp918; _temp919;});
return 1;}} _LL909: return 0; _LL905:;} _LL901: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp896); _LL897:;} _LL865: if( _temp879 ==  0? 1:( void*)
_temp879->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp925[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp925, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp926= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp874); Cyc_Tcutil_occurs( _temp926,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp874))->v, r);{ struct Cyc_Absyn_JoinEff_struct*
_temp927=({ struct Cyc_Absyn_JoinEff_struct* _temp929=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp929[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp930; _temp930.tag= Cyc_Absyn_JoinEff; _temp930.f1=({ struct Cyc_List_List*
_temp931=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp931->hd=( void*) _temp926; _temp931->tl=({ struct Cyc_List_List* _temp932=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp932->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp933=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp933[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp934; _temp934.tag= Cyc_Absyn_AccessEff; _temp934.f1=(
void*) r; _temp934;}); _temp933;})); _temp932->tl= 0; _temp932;}); _temp931;});
_temp930;}); _temp929;});* _temp878=({ struct Cyc_Core_Opt* _temp928=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp928->v=( void*)((
void*) _temp927); _temp928;}); return 1;}} _LL867: return 0; _LL857:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp935= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp947;
void* _temp949; struct Cyc_Core_Opt* _temp951; struct Cyc_Core_Opt* _temp953;
struct Cyc_Core_Opt** _temp955; struct Cyc_Core_Opt* _temp956; _LL937: if((
unsigned int) _temp935 >  3u?*(( int*) _temp935) ==  Cyc_Absyn_AccessEff: 0){
goto _LL938;} else{ goto _LL939;} _LL939: if(( unsigned int) _temp935 >  3u?*((
int*) _temp935) ==  Cyc_Absyn_JoinEff: 0){ _LL948: _temp947=(( struct Cyc_Absyn_JoinEff_struct*)
_temp935)->f1; goto _LL940;} else{ goto _LL941;} _LL941: if(( unsigned int)
_temp935 >  3u?*(( int*) _temp935) ==  Cyc_Absyn_RgnsEff: 0){ _LL950: _temp949=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp935)->f1; goto _LL942;} else{
goto _LL943;} _LL943: if(( unsigned int) _temp935 >  3u?*(( int*) _temp935) == 
Cyc_Absyn_Evar: 0){ _LL957: _temp956=(( struct Cyc_Absyn_Evar_struct*) _temp935)->f1;
goto _LL954; _LL954: _temp953=(( struct Cyc_Absyn_Evar_struct*) _temp935)->f2;
_temp955=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp935)->f2;
goto _LL952; _LL952: _temp951=(( struct Cyc_Absyn_Evar_struct*) _temp935)->f4;
goto _LL944;} else{ goto _LL945;} _LL945: goto _LL946; _LL938: return 0; _LL940:
for( 0; _temp947 !=  0; _temp947= _temp947->tl){ if( Cyc_Tcutil_type_in_effect(
constrain, t,( void*) _temp947->hd)){ return 1;}} return 0; _LL942: _temp949=
Cyc_Tcutil_compress( _temp949); if( t ==  _temp949){ return 1;} if( constrain){
return Cyc_Tcutil_unify( t, _temp949);}{ void* _temp958= Cyc_Tcutil_rgns_of( t);
void* _temp964; _LL960: if(( unsigned int) _temp958 >  3u?*(( int*) _temp958) == 
Cyc_Absyn_RgnsEff: 0){ _LL965: _temp964=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp958)->f1; goto _LL961;} else{ goto _LL962;} _LL962: goto _LL963; _LL961: {
struct _tuple8 _temp967=({ struct _tuple8 _temp966; _temp966.f1= t; _temp966.f2=
Cyc_Tcutil_compress( _temp964); _temp966;}); void* _temp973; struct Cyc_Absyn_Tvar*
_temp975; void* _temp977; struct Cyc_Absyn_Tvar* _temp979; _LL969: _LL978:
_temp977= _temp967.f1; if(( unsigned int) _temp977 >  3u?*(( int*) _temp977) == 
Cyc_Absyn_VarType: 0){ _LL980: _temp979=(( struct Cyc_Absyn_VarType_struct*)
_temp977)->f1; goto _LL974;} else{ goto _LL971;} _LL974: _temp973= _temp967.f2;
if(( unsigned int) _temp973 >  3u?*(( int*) _temp973) ==  Cyc_Absyn_VarType: 0){
_LL976: _temp975=(( struct Cyc_Absyn_VarType_struct*) _temp973)->f1; goto _LL970;}
else{ goto _LL971;} _LL971: goto _LL972; _LL970: return Cyc_Tcutil_unify( t,
_temp964); _LL972: return t ==  _temp964; _LL968:;} _LL963: return Cyc_Tcutil_type_in_effect(
constrain, t, _temp958); _LL959:;} _LL944: if( _temp956 ==  0? 1:( void*)
_temp956->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp981[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp981, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp982= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp951); Cyc_Tcutil_occurs( _temp982,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp951))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp983=({ struct Cyc_Absyn_JoinEff_struct* _temp985=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp985[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp986; _temp986.tag= Cyc_Absyn_JoinEff; _temp986.f1=({ struct Cyc_List_List*
_temp987=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp987->hd=( void*) _temp982; _temp987->tl=({ struct Cyc_List_List* _temp988=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp988->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp989=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp989[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp990; _temp990.tag= Cyc_Absyn_RgnsEff; _temp990.f1=( void*) t; _temp990;});
_temp989;})); _temp988->tl= 0; _temp988;}); _temp987;}); _temp986;}); _temp985;});*
_temp955=({ struct Cyc_Core_Opt* _temp984=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp984->v=( void*)(( void*) _temp983); _temp984;});
return 1;}} _LL946: return 0; _LL936:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp991= e; struct Cyc_Absyn_Tvar* _temp1003; struct Cyc_List_List*
_temp1005; void* _temp1007; struct Cyc_Core_Opt* _temp1009; struct Cyc_Core_Opt*
_temp1011; struct Cyc_Core_Opt** _temp1013; struct Cyc_Core_Opt* _temp1014;
_LL993: if(( unsigned int) _temp991 >  3u?*(( int*) _temp991) ==  Cyc_Absyn_VarType:
0){ _LL1004: _temp1003=(( struct Cyc_Absyn_VarType_struct*) _temp991)->f1; goto
_LL994;} else{ goto _LL995;} _LL995: if(( unsigned int) _temp991 >  3u?*(( int*)
_temp991) ==  Cyc_Absyn_JoinEff: 0){ _LL1006: _temp1005=(( struct Cyc_Absyn_JoinEff_struct*)
_temp991)->f1; goto _LL996;} else{ goto _LL997;} _LL997: if(( unsigned int)
_temp991 >  3u?*(( int*) _temp991) ==  Cyc_Absyn_RgnsEff: 0){ _LL1008: _temp1007=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp991)->f1; goto _LL998;} else{
goto _LL999;} _LL999: if(( unsigned int) _temp991 >  3u?*(( int*) _temp991) == 
Cyc_Absyn_Evar: 0){ _LL1015: _temp1014=(( struct Cyc_Absyn_Evar_struct*)
_temp991)->f1; goto _LL1012; _LL1012: _temp1011=(( struct Cyc_Absyn_Evar_struct*)
_temp991)->f2; _temp1013=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp991)->f2; goto _LL1010; _LL1010: _temp1009=(( struct Cyc_Absyn_Evar_struct*)
_temp991)->f4; goto _LL1000;} else{ goto _LL1001;} _LL1001: goto _LL1002; _LL994:
return Cyc_Absyn_tvar_cmp( v, _temp1003) ==  0; _LL996: for( 0; _temp1005 !=  0;
_temp1005= _temp1005->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*) _temp1005->hd)){ return 1;}} return 0; _LL998: { void* _temp1016= Cyc_Tcutil_rgns_of(
_temp1007); void* _temp1022; _LL1018: if(( unsigned int) _temp1016 >  3u?*(( int*)
_temp1016) ==  Cyc_Absyn_RgnsEff: 0){ _LL1023: _temp1022=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1016)->f1; goto _LL1019;} else{ goto _LL1020;} _LL1020: goto _LL1021;
_LL1019: if( ! constrain){ return 0;}{ void* _temp1024= Cyc_Tcutil_compress(
_temp1022); struct Cyc_Core_Opt* _temp1030; struct Cyc_Core_Opt* _temp1032;
struct Cyc_Core_Opt** _temp1034; struct Cyc_Core_Opt* _temp1035; _LL1026: if((
unsigned int) _temp1024 >  3u?*(( int*) _temp1024) ==  Cyc_Absyn_Evar: 0){
_LL1036: _temp1035=(( struct Cyc_Absyn_Evar_struct*) _temp1024)->f1; goto
_LL1033; _LL1033: _temp1032=(( struct Cyc_Absyn_Evar_struct*) _temp1024)->f2;
_temp1034=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1024)->f2;
goto _LL1031; _LL1031: _temp1030=(( struct Cyc_Absyn_Evar_struct*) _temp1024)->f4;
goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029; _LL1027: { void*
_temp1037= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp1030);
if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1030))->v, v)){
return 0;}{ void* _temp1038= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1040=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1040[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1041; _temp1041.tag= Cyc_Absyn_JoinEff;
_temp1041.f1=({ void* _temp1042[ 2u]; _temp1042[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1043=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1043[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1044; _temp1044.tag= Cyc_Absyn_VarType;
_temp1044.f1= v; _temp1044;}); _temp1043;}); _temp1042[ 0u]= _temp1037; Cyc_List_list(
_tag_arr( _temp1042, sizeof( void*), 2u));}); _temp1041;}); _temp1040;}));*
_temp1034=({ struct Cyc_Core_Opt* _temp1039=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1039->v=( void*) _temp1038; _temp1039;});
return 1;}} _LL1029: return 0; _LL1025:;} _LL1021: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp1016); _LL1017:;} _LL1000: if( _temp1014 ==  0? 1:( void*)
_temp1014->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp1045[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp1045, sizeof( void*), 0u));});}{
void* _temp1046= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp1009); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1009))->v, v)){
return 0;}{ struct Cyc_Absyn_JoinEff_struct* _temp1047=({ struct Cyc_Absyn_JoinEff_struct*
_temp1049=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1049[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1050; _temp1050.tag= Cyc_Absyn_JoinEff;
_temp1050.f1=({ struct Cyc_List_List* _temp1051=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1051->hd=( void*) _temp1046;
_temp1051->tl=({ struct Cyc_List_List* _temp1052=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1052->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp1053=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1053[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1054; _temp1054.tag= Cyc_Absyn_VarType; _temp1054.f1=
v; _temp1054;}); _temp1053;})); _temp1052->tl= 0; _temp1052;}); _temp1051;});
_temp1050;}); _temp1049;});* _temp1013=({ struct Cyc_Core_Opt* _temp1048=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1048->v=(
void*)(( void*) _temp1047); _temp1048;}); return 1;}} _LL1002: return 0; _LL992:;}}
static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp1055= e; struct Cyc_List_List* _temp1065; void* _temp1067;
_LL1057: if(( unsigned int) _temp1055 >  3u?*(( int*) _temp1055) ==  Cyc_Absyn_JoinEff:
0){ _LL1066: _temp1065=(( struct Cyc_Absyn_JoinEff_struct*) _temp1055)->f1; goto
_LL1058;} else{ goto _LL1059;} _LL1059: if(( unsigned int) _temp1055 >  3u?*((
int*) _temp1055) ==  Cyc_Absyn_RgnsEff: 0){ _LL1068: _temp1067=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp1055)->f1; goto _LL1060;} else{ goto _LL1061;}
_LL1061: if(( unsigned int) _temp1055 >  3u?*(( int*) _temp1055) ==  Cyc_Absyn_Evar:
0){ goto _LL1062;} else{ goto _LL1063;} _LL1063: goto _LL1064; _LL1058: for( 0;
_temp1065 !=  0; _temp1065= _temp1065->tl){ if( Cyc_Tcutil_evar_in_effect( evar,(
void*) _temp1065->hd)){ return 1;}} return 0; _LL1060: { void* _temp1069= Cyc_Tcutil_rgns_of(
_temp1067); void* _temp1075; _LL1071: if(( unsigned int) _temp1069 >  3u?*(( int*)
_temp1069) ==  Cyc_Absyn_RgnsEff: 0){ _LL1076: _temp1075=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1069)->f1; goto _LL1072;} else{ goto _LL1073;} _LL1073: goto _LL1074;
_LL1072: return 0; _LL1074: return Cyc_Tcutil_evar_in_effect( evar, _temp1069);
_LL1070:;} _LL1062: return evar ==  e; _LL1064: return 0; _LL1056:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1077= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1091; void* _temp1093; struct Cyc_Absyn_Tvar*
_temp1095; void* _temp1097; struct Cyc_Core_Opt* _temp1099; struct Cyc_Core_Opt*
_temp1101; struct Cyc_Core_Opt** _temp1103; _LL1079: if(( unsigned int)
_temp1077 >  3u?*(( int*) _temp1077) ==  Cyc_Absyn_JoinEff: 0){ _LL1092:
_temp1091=(( struct Cyc_Absyn_JoinEff_struct*) _temp1077)->f1; goto _LL1080;}
else{ goto _LL1081;} _LL1081: if(( unsigned int) _temp1077 >  3u?*(( int*)
_temp1077) ==  Cyc_Absyn_AccessEff: 0){ _LL1094: _temp1093=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1077)->f1; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(( unsigned int)
_temp1077 >  3u?*(( int*) _temp1077) ==  Cyc_Absyn_VarType: 0){ _LL1096:
_temp1095=(( struct Cyc_Absyn_VarType_struct*) _temp1077)->f1; goto _LL1084;}
else{ goto _LL1085;} _LL1085: if(( unsigned int) _temp1077 >  3u?*(( int*)
_temp1077) ==  Cyc_Absyn_RgnsEff: 0){ _LL1098: _temp1097=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1077)->f1; goto _LL1086;} else{ goto _LL1087;} _LL1087: if(( unsigned int)
_temp1077 >  3u?*(( int*) _temp1077) ==  Cyc_Absyn_Evar: 0){ _LL1102: _temp1101=((
struct Cyc_Absyn_Evar_struct*) _temp1077)->f2; _temp1103=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1077)->f2; goto _LL1100; _LL1100: _temp1099=((
struct Cyc_Absyn_Evar_struct*) _temp1077)->f4; goto _LL1088;} else{ goto _LL1089;}
_LL1089: goto _LL1090; _LL1080: for( 0; _temp1091 !=  0; _temp1091= _temp1091->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*) _temp1091->hd, e2)){
return 0;}} return 1; _LL1082: return( Cyc_Tcutil_region_in_effect( 0, _temp1093,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp1093, e2))? 1: Cyc_Tcutil_unify(
_temp1093,( void*) Cyc_Absyn_HeapRgn); _LL1084: return Cyc_Tcutil_variable_in_effect(
0, _temp1095, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1095, e2); _LL1086: {
void* _temp1104= Cyc_Tcutil_rgns_of( _temp1097); void* _temp1110; _LL1106: if((
unsigned int) _temp1104 >  3u?*(( int*) _temp1104) ==  Cyc_Absyn_RgnsEff: 0){
_LL1111: _temp1110=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1104)->f1;
goto _LL1107;} else{ goto _LL1108;} _LL1108: goto _LL1109; _LL1107: return( Cyc_Tcutil_type_in_effect(
0, _temp1110, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1110, e2))? 1: Cyc_Tcutil_unify(
_temp1110, Cyc_Absyn_sint_t); _LL1109: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1104, e2); _LL1105:;} _LL1088: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1103=({ struct Cyc_Core_Opt* _temp1112=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1112->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1113=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1113[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1114; _temp1114.tag= Cyc_Absyn_JoinEff; _temp1114.f1=
0; _temp1114;}); _temp1113;})); _temp1112;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1099))->v, e2);*
_temp1103=({ struct Cyc_Core_Opt* _temp1115=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1115->v=( void*) e2; _temp1115;});}} return
1; _LL1090: return({ struct Cyc_Std_String_pa_struct _temp1117; _temp1117.tag=
Cyc_Std_String_pa; _temp1117.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1116[ 1u]={& _temp1117};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1116, sizeof( void*), 1u));}});
_LL1078:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1119=({ struct
_tuple10 _temp1118; _temp1118.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1118.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1118;}); _LL1121: goto _LL1122; _LL1122:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1120:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1= r1->tl){ struct _tuple8 _temp1125; void* _temp1126; void* _temp1128; struct
_tuple8* _temp1123=( struct _tuple8*) r1->hd; _temp1125=* _temp1123; _LL1129:
_temp1128= _temp1125.f1; goto _LL1127; _LL1127: _temp1126= _temp1125.f2; goto
_LL1124; _LL1124: { int found= _temp1128 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2= r2->tl){ struct
_tuple8 _temp1132; void* _temp1133; void* _temp1135; struct _tuple8* _temp1130=(
struct _tuple8*) r2->hd; _temp1132=* _temp1130; _LL1136: _temp1135= _temp1132.f1;
goto _LL1134; _LL1134: _temp1133= _temp1132.f2; goto _LL1131; _LL1131: if( Cyc_Tcutil_unify(
_temp1128, _temp1135)? Cyc_Tcutil_unify( _temp1126, _temp1133): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason=(
struct _tagged_arr) _tag_arr( 0u, 0u, 0u); t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2); if( t1 ==  t2){ return;}{ void* _temp1137= t1; struct Cyc_Core_Opt*
_temp1143; struct Cyc_Core_Opt* _temp1145; struct Cyc_Core_Opt** _temp1147;
struct Cyc_Core_Opt* _temp1148; _LL1139: if(( unsigned int) _temp1137 >  3u?*((
int*) _temp1137) ==  Cyc_Absyn_Evar: 0){ _LL1149: _temp1148=(( struct Cyc_Absyn_Evar_struct*)
_temp1137)->f1; goto _LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_Evar_struct*)
_temp1137)->f2; _temp1147=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1137)->f2; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_Evar_struct*)
_temp1137)->f4; goto _LL1140;} else{ goto _LL1141;} _LL1141: goto _LL1142;
_LL1140: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1143))->v, t2);{ void* _temp1150= Cyc_Tcutil_typ_kind( t2);
if( Cyc_Tcutil_kind_leq( _temp1150,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1148))->v)){* _temp1147=({ struct Cyc_Core_Opt* _temp1151=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1151->v=( void*) t2; _temp1151;});
return;} else{{ void* _temp1152= t2; struct Cyc_Core_Opt* _temp1160; struct Cyc_Core_Opt*
_temp1162; struct Cyc_Core_Opt** _temp1164; struct Cyc_Absyn_PtrInfo _temp1166;
_LL1154: if(( unsigned int) _temp1152 >  3u?*(( int*) _temp1152) ==  Cyc_Absyn_Evar:
0){ _LL1163: _temp1162=(( struct Cyc_Absyn_Evar_struct*) _temp1152)->f2;
_temp1164=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1152)->f2;
goto _LL1161; _LL1161: _temp1160=(( struct Cyc_Absyn_Evar_struct*) _temp1152)->f4;
goto _LL1155;} else{ goto _LL1156;} _LL1156: if(( unsigned int) _temp1152 >  3u?*((
int*) _temp1152) ==  Cyc_Absyn_PointerType: 0){ _LL1167: _temp1166=(( struct Cyc_Absyn_PointerType_struct*)
_temp1152)->f1; goto _LL1165;} else{ goto _LL1158;} _LL1165: if(( void*)
_temp1148->v == ( void*) Cyc_Absyn_BoxKind){ goto _LL1157;} else{ goto _LL1158;}
_LL1158: goto _LL1159; _LL1155: { struct Cyc_List_List* _temp1168=( struct Cyc_List_List*)
_temp1143->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1160))->v; for( 0; s2 !=  0; s2= s2->tl){ if( !(( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp1168,(
struct Cyc_Absyn_Tvar*) s2->hd)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if(
Cyc_Tcutil_kind_leq(( void*) _temp1148->v, _temp1150)){* _temp1164=({ struct Cyc_Core_Opt*
_temp1169=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1169->v=( void*) t1; _temp1169;}); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)", sizeof( unsigned char), 25u); goto _LL1153;}
_LL1157: { struct Cyc_Absyn_Conref* _temp1170= Cyc_Absyn_compress_conref(
_temp1166.bounds);{ void* _temp1171=( void*) _temp1170->v; _LL1173: if(
_temp1171 == ( void*) Cyc_Absyn_No_constr){ goto _LL1174;} else{ goto _LL1175;}
_LL1175: goto _LL1176; _LL1174:( void*)( _temp1170->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp1177=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1177[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1178; _temp1178.tag= Cyc_Absyn_Eq_constr;
_temp1178.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1179=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1179[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1180; _temp1180.tag= Cyc_Absyn_Upper_b;
_temp1180.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1180;}); _temp1179;}));
_temp1178;}); _temp1177;})));* _temp1147=({ struct Cyc_Core_Opt* _temp1181=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1181->v=(
void*) t2; _temp1181;}); return; _LL1176: goto _LL1172; _LL1172:;} goto _LL1153;}
_LL1159: goto _LL1153; _LL1153:;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} _LL1142:
goto _LL1138; _LL1138:;}{ struct _tuple8 _temp1183=({ struct _tuple8 _temp1182;
_temp1182.f1= t2; _temp1182.f2= t1; _temp1182;}); void* _temp1239; void*
_temp1241; void* _temp1243; void* _temp1245; struct Cyc_Absyn_Tvar* _temp1247;
void* _temp1249; struct Cyc_Absyn_Tvar* _temp1251; void* _temp1253; struct Cyc_Absyn_Structdecl**
_temp1255; struct Cyc_List_List* _temp1257; struct _tuple1* _temp1259; void*
_temp1261; struct Cyc_Absyn_Structdecl** _temp1263; struct Cyc_List_List*
_temp1265; struct _tuple1* _temp1267; void* _temp1269; struct _tuple1* _temp1271;
void* _temp1273; struct _tuple1* _temp1275; void* _temp1277; struct Cyc_List_List*
_temp1279; void* _temp1281; struct Cyc_List_List* _temp1283; void* _temp1285;
struct Cyc_Absyn_Uniondecl** _temp1287; struct Cyc_List_List* _temp1289; struct
_tuple1* _temp1291; void* _temp1293; struct Cyc_Absyn_Uniondecl** _temp1295;
struct Cyc_List_List* _temp1297; struct _tuple1* _temp1299; void* _temp1301;
struct Cyc_Absyn_TunionInfo _temp1303; void* _temp1305; struct Cyc_List_List*
_temp1307; void* _temp1309; struct Cyc_Absyn_Tuniondecl** _temp1311; struct Cyc_Absyn_Tuniondecl*
_temp1313; void* _temp1314; struct Cyc_Absyn_TunionInfo _temp1316; void*
_temp1318; struct Cyc_List_List* _temp1320; void* _temp1322; struct Cyc_Absyn_Tuniondecl**
_temp1324; struct Cyc_Absyn_Tuniondecl* _temp1326; void* _temp1327; struct Cyc_Absyn_TunionFieldInfo
_temp1329; struct Cyc_List_List* _temp1331; void* _temp1333; struct Cyc_Absyn_Tunionfield*
_temp1335; struct Cyc_Absyn_Tuniondecl* _temp1337; void* _temp1339; struct Cyc_Absyn_TunionFieldInfo
_temp1341; struct Cyc_List_List* _temp1343; void* _temp1345; struct Cyc_Absyn_Tunionfield*
_temp1347; struct Cyc_Absyn_Tuniondecl* _temp1349; void* _temp1351; struct Cyc_Absyn_PtrInfo
_temp1353; struct Cyc_Absyn_Conref* _temp1355; struct Cyc_Absyn_Tqual _temp1357;
struct Cyc_Absyn_Conref* _temp1359; void* _temp1361; void* _temp1363; void*
_temp1365; struct Cyc_Absyn_PtrInfo _temp1367; struct Cyc_Absyn_Conref*
_temp1369; struct Cyc_Absyn_Tqual _temp1371; struct Cyc_Absyn_Conref* _temp1373;
void* _temp1375; void* _temp1377; void* _temp1379; void* _temp1381; void*
_temp1383; void* _temp1385; void* _temp1387; void* _temp1389; void* _temp1391;
void* _temp1393; void* _temp1395; int _temp1397; void* _temp1399; int _temp1401;
void* _temp1403; struct Cyc_Absyn_Exp* _temp1405; struct Cyc_Absyn_Tqual
_temp1407; void* _temp1409; void* _temp1411; struct Cyc_Absyn_Exp* _temp1413;
struct Cyc_Absyn_Tqual _temp1415; void* _temp1417; void* _temp1419; struct Cyc_Absyn_FnInfo
_temp1421; struct Cyc_List_List* _temp1423; struct Cyc_List_List* _temp1425;
struct Cyc_Absyn_VarargInfo* _temp1427; int _temp1429; struct Cyc_List_List*
_temp1431; void* _temp1433; struct Cyc_Core_Opt* _temp1435; struct Cyc_List_List*
_temp1437; void* _temp1439; struct Cyc_Absyn_FnInfo _temp1441; struct Cyc_List_List*
_temp1443; struct Cyc_List_List* _temp1445; struct Cyc_Absyn_VarargInfo*
_temp1447; int _temp1449; struct Cyc_List_List* _temp1451; void* _temp1453;
struct Cyc_Core_Opt* _temp1455; struct Cyc_List_List* _temp1457; void* _temp1459;
struct Cyc_List_List* _temp1461; void* _temp1463; struct Cyc_List_List*
_temp1465; void* _temp1467; struct Cyc_List_List* _temp1469; void* _temp1471;
struct Cyc_List_List* _temp1473; void* _temp1475; struct Cyc_List_List*
_temp1477; void* _temp1479; struct Cyc_List_List* _temp1481; void* _temp1483;
void* _temp1485; void* _temp1487; void* _temp1489; void* _temp1491; void*
_temp1493; void* _temp1495; void* _temp1497; void* _temp1499; void* _temp1501;
void* _temp1503; void* _temp1505; _LL1185: _LL1240: _temp1239= _temp1183.f1; if((
unsigned int) _temp1239 >  3u?*(( int*) _temp1239) ==  Cyc_Absyn_Evar: 0){ goto
_LL1186;} else{ goto _LL1187;} _LL1187: _LL1244: _temp1243= _temp1183.f1; if(
_temp1243 == ( void*) Cyc_Absyn_VoidType){ goto _LL1242;} else{ goto _LL1189;}
_LL1242: _temp1241= _temp1183.f2; if( _temp1241 == ( void*) Cyc_Absyn_VoidType){
goto _LL1188;} else{ goto _LL1189;} _LL1189: _LL1250: _temp1249= _temp1183.f1;
if(( unsigned int) _temp1249 >  3u?*(( int*) _temp1249) ==  Cyc_Absyn_VarType: 0){
_LL1252: _temp1251=(( struct Cyc_Absyn_VarType_struct*) _temp1249)->f1; goto
_LL1246;} else{ goto _LL1191;} _LL1246: _temp1245= _temp1183.f2; if((
unsigned int) _temp1245 >  3u?*(( int*) _temp1245) ==  Cyc_Absyn_VarType: 0){
_LL1248: _temp1247=(( struct Cyc_Absyn_VarType_struct*) _temp1245)->f1; goto
_LL1190;} else{ goto _LL1191;} _LL1191: _LL1262: _temp1261= _temp1183.f1; if((
unsigned int) _temp1261 >  3u?*(( int*) _temp1261) ==  Cyc_Absyn_StructType: 0){
_LL1268: _temp1267=(( struct Cyc_Absyn_StructType_struct*) _temp1261)->f1; goto
_LL1266; _LL1266: _temp1265=(( struct Cyc_Absyn_StructType_struct*) _temp1261)->f2;
goto _LL1264; _LL1264: _temp1263=(( struct Cyc_Absyn_StructType_struct*)
_temp1261)->f3; goto _LL1254;} else{ goto _LL1193;} _LL1254: _temp1253=
_temp1183.f2; if(( unsigned int) _temp1253 >  3u?*(( int*) _temp1253) ==  Cyc_Absyn_StructType:
0){ _LL1260: _temp1259=(( struct Cyc_Absyn_StructType_struct*) _temp1253)->f1;
goto _LL1258; _LL1258: _temp1257=(( struct Cyc_Absyn_StructType_struct*)
_temp1253)->f2; goto _LL1256; _LL1256: _temp1255=(( struct Cyc_Absyn_StructType_struct*)
_temp1253)->f3; goto _LL1192;} else{ goto _LL1193;} _LL1193: _LL1274: _temp1273=
_temp1183.f1; if(( unsigned int) _temp1273 >  3u?*(( int*) _temp1273) ==  Cyc_Absyn_EnumType:
0){ _LL1276: _temp1275=(( struct Cyc_Absyn_EnumType_struct*) _temp1273)->f1;
goto _LL1270;} else{ goto _LL1195;} _LL1270: _temp1269= _temp1183.f2; if((
unsigned int) _temp1269 >  3u?*(( int*) _temp1269) ==  Cyc_Absyn_EnumType: 0){
_LL1272: _temp1271=(( struct Cyc_Absyn_EnumType_struct*) _temp1269)->f1; goto
_LL1194;} else{ goto _LL1195;} _LL1195: _LL1282: _temp1281= _temp1183.f1; if((
unsigned int) _temp1281 >  3u?*(( int*) _temp1281) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1284: _temp1283=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1281)->f1;
goto _LL1278;} else{ goto _LL1197;} _LL1278: _temp1277= _temp1183.f2; if((
unsigned int) _temp1277 >  3u?*(( int*) _temp1277) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1280: _temp1279=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1277)->f1;
goto _LL1196;} else{ goto _LL1197;} _LL1197: _LL1294: _temp1293= _temp1183.f1;
if(( unsigned int) _temp1293 >  3u?*(( int*) _temp1293) ==  Cyc_Absyn_UnionType:
0){ _LL1300: _temp1299=(( struct Cyc_Absyn_UnionType_struct*) _temp1293)->f1;
goto _LL1298; _LL1298: _temp1297=(( struct Cyc_Absyn_UnionType_struct*)
_temp1293)->f2; goto _LL1296; _LL1296: _temp1295=(( struct Cyc_Absyn_UnionType_struct*)
_temp1293)->f3; goto _LL1286;} else{ goto _LL1199;} _LL1286: _temp1285=
_temp1183.f2; if(( unsigned int) _temp1285 >  3u?*(( int*) _temp1285) ==  Cyc_Absyn_UnionType:
0){ _LL1292: _temp1291=(( struct Cyc_Absyn_UnionType_struct*) _temp1285)->f1;
goto _LL1290; _LL1290: _temp1289=(( struct Cyc_Absyn_UnionType_struct*)
_temp1285)->f2; goto _LL1288; _LL1288: _temp1287=(( struct Cyc_Absyn_UnionType_struct*)
_temp1285)->f3; goto _LL1198;} else{ goto _LL1199;} _LL1199: _LL1315: _temp1314=
_temp1183.f1; if(( unsigned int) _temp1314 >  3u?*(( int*) _temp1314) ==  Cyc_Absyn_TunionType:
0){ _LL1317: _temp1316=(( struct Cyc_Absyn_TunionType_struct*) _temp1314)->f1;
_LL1323: _temp1322=( void*) _temp1316.tunion_info; if(*(( int*) _temp1322) == 
Cyc_Absyn_KnownTunion){ _LL1325: _temp1324=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1322)->f1; _temp1326=* _temp1324; goto _LL1321;} else{ goto _LL1201;}
_LL1321: _temp1320= _temp1316.targs; goto _LL1319; _LL1319: _temp1318=( void*)
_temp1316.rgn; goto _LL1302;} else{ goto _LL1201;} _LL1302: _temp1301= _temp1183.f2;
if(( unsigned int) _temp1301 >  3u?*(( int*) _temp1301) ==  Cyc_Absyn_TunionType:
0){ _LL1304: _temp1303=(( struct Cyc_Absyn_TunionType_struct*) _temp1301)->f1;
_LL1310: _temp1309=( void*) _temp1303.tunion_info; if(*(( int*) _temp1309) == 
Cyc_Absyn_KnownTunion){ _LL1312: _temp1311=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1309)->f1; _temp1313=* _temp1311; goto _LL1308;} else{ goto _LL1201;}
_LL1308: _temp1307= _temp1303.targs; goto _LL1306; _LL1306: _temp1305=( void*)
_temp1303.rgn; goto _LL1200;} else{ goto _LL1201;} _LL1201: _LL1340: _temp1339=
_temp1183.f1; if(( unsigned int) _temp1339 >  3u?*(( int*) _temp1339) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1342: _temp1341=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1339)->f1;
_LL1346: _temp1345=( void*) _temp1341.field_info; if(*(( int*) _temp1345) == 
Cyc_Absyn_KnownTunionfield){ _LL1350: _temp1349=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1345)->f1; goto _LL1348; _LL1348: _temp1347=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1345)->f2; goto _LL1344;} else{ goto _LL1203;} _LL1344: _temp1343=
_temp1341.targs; goto _LL1328;} else{ goto _LL1203;} _LL1328: _temp1327=
_temp1183.f2; if(( unsigned int) _temp1327 >  3u?*(( int*) _temp1327) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1330: _temp1329=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1327)->f1;
_LL1334: _temp1333=( void*) _temp1329.field_info; if(*(( int*) _temp1333) == 
Cyc_Absyn_KnownTunionfield){ _LL1338: _temp1337=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1333)->f1; goto _LL1336; _LL1336: _temp1335=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1333)->f2; goto _LL1332;} else{ goto _LL1203;} _LL1332: _temp1331=
_temp1329.targs; goto _LL1202;} else{ goto _LL1203;} _LL1203: _LL1366: _temp1365=
_temp1183.f1; if(( unsigned int) _temp1365 >  3u?*(( int*) _temp1365) ==  Cyc_Absyn_PointerType:
0){ _LL1368: _temp1367=(( struct Cyc_Absyn_PointerType_struct*) _temp1365)->f1;
_LL1378: _temp1377=( void*) _temp1367.elt_typ; goto _LL1376; _LL1376: _temp1375=(
void*) _temp1367.rgn_typ; goto _LL1374; _LL1374: _temp1373= _temp1367.nullable;
goto _LL1372; _LL1372: _temp1371= _temp1367.tq; goto _LL1370; _LL1370: _temp1369=
_temp1367.bounds; goto _LL1352;} else{ goto _LL1205;} _LL1352: _temp1351=
_temp1183.f2; if(( unsigned int) _temp1351 >  3u?*(( int*) _temp1351) ==  Cyc_Absyn_PointerType:
0){ _LL1354: _temp1353=(( struct Cyc_Absyn_PointerType_struct*) _temp1351)->f1;
_LL1364: _temp1363=( void*) _temp1353.elt_typ; goto _LL1362; _LL1362: _temp1361=(
void*) _temp1353.rgn_typ; goto _LL1360; _LL1360: _temp1359= _temp1353.nullable;
goto _LL1358; _LL1358: _temp1357= _temp1353.tq; goto _LL1356; _LL1356: _temp1355=
_temp1353.bounds; goto _LL1204;} else{ goto _LL1205;} _LL1205: _LL1386:
_temp1385= _temp1183.f1; if(( unsigned int) _temp1385 >  3u?*(( int*) _temp1385)
==  Cyc_Absyn_IntType: 0){ _LL1390: _temp1389=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1385)->f1; goto _LL1388; _LL1388: _temp1387=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1385)->f2; goto _LL1380;} else{ goto _LL1207;} _LL1380: _temp1379=
_temp1183.f2; if(( unsigned int) _temp1379 >  3u?*(( int*) _temp1379) ==  Cyc_Absyn_IntType:
0){ _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1379)->f1;
goto _LL1382; _LL1382: _temp1381=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1379)->f2; goto _LL1206;} else{ goto _LL1207;} _LL1207: _LL1394: _temp1393=
_temp1183.f1; if( _temp1393 == ( void*) Cyc_Absyn_FloatType){ goto _LL1392;}
else{ goto _LL1209;} _LL1392: _temp1391= _temp1183.f2; if( _temp1391 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1208;} else{ goto _LL1209;} _LL1209: _LL1400:
_temp1399= _temp1183.f1; if(( unsigned int) _temp1399 >  3u?*(( int*) _temp1399)
==  Cyc_Absyn_DoubleType: 0){ _LL1402: _temp1401=(( struct Cyc_Absyn_DoubleType_struct*)
_temp1399)->f1; goto _LL1396;} else{ goto _LL1211;} _LL1396: _temp1395=
_temp1183.f2; if(( unsigned int) _temp1395 >  3u?*(( int*) _temp1395) ==  Cyc_Absyn_DoubleType:
0){ _LL1398: _temp1397=(( struct Cyc_Absyn_DoubleType_struct*) _temp1395)->f1;
goto _LL1210;} else{ goto _LL1211;} _LL1211: _LL1412: _temp1411= _temp1183.f1;
if(( unsigned int) _temp1411 >  3u?*(( int*) _temp1411) ==  Cyc_Absyn_ArrayType:
0){ _LL1418: _temp1417=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1411)->f1;
goto _LL1416; _LL1416: _temp1415=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1411)->f2; goto _LL1414; _LL1414: _temp1413=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1411)->f3; goto _LL1404;} else{ goto _LL1213;} _LL1404: _temp1403=
_temp1183.f2; if(( unsigned int) _temp1403 >  3u?*(( int*) _temp1403) ==  Cyc_Absyn_ArrayType:
0){ _LL1410: _temp1409=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1403)->f1;
goto _LL1408; _LL1408: _temp1407=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1403)->f2; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1403)->f3; goto _LL1212;} else{ goto _LL1213;} _LL1213: _LL1440: _temp1439=
_temp1183.f1; if(( unsigned int) _temp1439 >  3u?*(( int*) _temp1439) ==  Cyc_Absyn_FnType:
0){ _LL1442: _temp1441=(( struct Cyc_Absyn_FnType_struct*) _temp1439)->f1;
_LL1458: _temp1457= _temp1441.tvars; goto _LL1456; _LL1456: _temp1455= _temp1441.effect;
goto _LL1454; _LL1454: _temp1453=( void*) _temp1441.ret_typ; goto _LL1452;
_LL1452: _temp1451= _temp1441.args; goto _LL1450; _LL1450: _temp1449= _temp1441.c_varargs;
goto _LL1448; _LL1448: _temp1447= _temp1441.cyc_varargs; goto _LL1446; _LL1446:
_temp1445= _temp1441.rgn_po; goto _LL1444; _LL1444: _temp1443= _temp1441.attributes;
goto _LL1420;} else{ goto _LL1215;} _LL1420: _temp1419= _temp1183.f2; if((
unsigned int) _temp1419 >  3u?*(( int*) _temp1419) ==  Cyc_Absyn_FnType: 0){
_LL1422: _temp1421=(( struct Cyc_Absyn_FnType_struct*) _temp1419)->f1; _LL1438:
_temp1437= _temp1421.tvars; goto _LL1436; _LL1436: _temp1435= _temp1421.effect;
goto _LL1434; _LL1434: _temp1433=( void*) _temp1421.ret_typ; goto _LL1432;
_LL1432: _temp1431= _temp1421.args; goto _LL1430; _LL1430: _temp1429= _temp1421.c_varargs;
goto _LL1428; _LL1428: _temp1427= _temp1421.cyc_varargs; goto _LL1426; _LL1426:
_temp1425= _temp1421.rgn_po; goto _LL1424; _LL1424: _temp1423= _temp1421.attributes;
goto _LL1214;} else{ goto _LL1215;} _LL1215: _LL1464: _temp1463= _temp1183.f1;
if(( unsigned int) _temp1463 >  3u?*(( int*) _temp1463) ==  Cyc_Absyn_TupleType:
0){ _LL1466: _temp1465=(( struct Cyc_Absyn_TupleType_struct*) _temp1463)->f1;
goto _LL1460;} else{ goto _LL1217;} _LL1460: _temp1459= _temp1183.f2; if((
unsigned int) _temp1459 >  3u?*(( int*) _temp1459) ==  Cyc_Absyn_TupleType: 0){
_LL1462: _temp1461=(( struct Cyc_Absyn_TupleType_struct*) _temp1459)->f1; goto
_LL1216;} else{ goto _LL1217;} _LL1217: _LL1472: _temp1471= _temp1183.f1; if((
unsigned int) _temp1471 >  3u?*(( int*) _temp1471) ==  Cyc_Absyn_AnonStructType:
0){ _LL1474: _temp1473=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1471)->f1;
goto _LL1468;} else{ goto _LL1219;} _LL1468: _temp1467= _temp1183.f2; if((
unsigned int) _temp1467 >  3u?*(( int*) _temp1467) ==  Cyc_Absyn_AnonStructType:
0){ _LL1470: _temp1469=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1467)->f1;
goto _LL1218;} else{ goto _LL1219;} _LL1219: _LL1480: _temp1479= _temp1183.f1;
if(( unsigned int) _temp1479 >  3u?*(( int*) _temp1479) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1482: _temp1481=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1479)->f1;
goto _LL1476;} else{ goto _LL1221;} _LL1476: _temp1475= _temp1183.f2; if((
unsigned int) _temp1475 >  3u?*(( int*) _temp1475) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1478: _temp1477=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1475)->f1;
goto _LL1220;} else{ goto _LL1221;} _LL1221: _LL1486: _temp1485= _temp1183.f1;
if( _temp1485 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1484;} else{ goto _LL1223;}
_LL1484: _temp1483= _temp1183.f2; if( _temp1483 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1222;} else{ goto _LL1223;} _LL1223: _LL1492: _temp1491= _temp1183.f1;
if(( unsigned int) _temp1491 >  3u?*(( int*) _temp1491) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1494: _temp1493=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1491)->f1; goto _LL1488;} else{ goto _LL1225;} _LL1488: _temp1487=
_temp1183.f2; if(( unsigned int) _temp1487 >  3u?*(( int*) _temp1487) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1490: _temp1489=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1487)->f1; goto _LL1224;} else{ goto _LL1225;} _LL1225: _LL1496: _temp1495=
_temp1183.f1; if(( unsigned int) _temp1495 >  3u?*(( int*) _temp1495) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1226;} else{ goto _LL1227;} _LL1227: _LL1498: _temp1497= _temp1183.f2;
if(( unsigned int) _temp1497 >  3u?*(( int*) _temp1497) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1228;} else{ goto _LL1229;} _LL1229: _LL1500: _temp1499= _temp1183.f1;
if(( unsigned int) _temp1499 >  3u?*(( int*) _temp1499) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1230;} else{ goto _LL1231;} _LL1231: _LL1502: _temp1501= _temp1183.f1;
if(( unsigned int) _temp1501 >  3u?*(( int*) _temp1501) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1232;} else{ goto _LL1233;} _LL1233: _LL1504: _temp1503= _temp1183.f2;
if(( unsigned int) _temp1503 >  3u?*(( int*) _temp1503) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1234;} else{ goto _LL1235;} _LL1235: _LL1506: _temp1505= _temp1183.f2;
if(( unsigned int) _temp1505 >  3u?*(( int*) _temp1505) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1236;} else{ goto _LL1237;} _LL1237: goto _LL1238; _LL1186: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1188: return; _LL1190: { struct _tagged_arr* _temp1507=
_temp1251->name; struct _tagged_arr* _temp1508= _temp1247->name; int _temp1509=*((
int*) _check_null( _temp1251->identity)); int _temp1510=*(( int*) _check_null(
_temp1247->identity)); void* _temp1511= Cyc_Absyn_conref_val( _temp1251->kind);
void* _temp1512= Cyc_Absyn_conref_val( _temp1247->kind); if( _temp1510 == 
_temp1509? Cyc_Std_zstrptrcmp( _temp1507, _temp1508) ==  0: 0){ if( _temp1511 != 
_temp1512){({ struct Cyc_Std_String_pa_struct _temp1516; _temp1516.tag= Cyc_Std_String_pa;
_temp1516.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1512);{ struct
Cyc_Std_String_pa_struct _temp1515; _temp1515.tag= Cyc_Std_String_pa; _temp1515.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1511);{ struct Cyc_Std_String_pa_struct
_temp1514; _temp1514.tag= Cyc_Std_String_pa; _temp1514.f1=( struct _tagged_arr)*
_temp1507;{ void* _temp1513[ 3u]={& _temp1514,& _temp1515,& _temp1516};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1513, sizeof( void*), 3u));}}}});}
return;} Cyc_Tcutil_failure_reason= _tag_arr("(variable types are not the same)",
sizeof( unsigned char), 34u); goto _LL1184;} _LL1192: if((( _temp1259 !=  0?
_temp1267 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1259),( struct _tuple1*) _check_null( _temp1267)) ==  0: 0)? 1:( _temp1259
==  0? _temp1267 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1257, _temp1265);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different struct types)", sizeof(
unsigned char), 25u); goto _LL1184; _LL1194: if( Cyc_Absyn_qvar_cmp( _temp1275,
_temp1271) ==  0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different enum types)",
sizeof( unsigned char), 23u); goto _LL1184; _LL1196: { int bad= 0; for( 0;
_temp1283 !=  0? _temp1279 !=  0: 0;( _temp1283= _temp1283->tl, _temp1279=
_temp1279->tl)){ struct Cyc_Absyn_Enumfield* _temp1517=( struct Cyc_Absyn_Enumfield*)
_temp1283->hd; struct Cyc_Absyn_Enumfield* _temp1518=( struct Cyc_Absyn_Enumfield*)
_temp1279->hd; if( Cyc_Absyn_qvar_cmp( _temp1517->name, _temp1518->name) !=  0){
Cyc_Tcutil_failure_reason= _tag_arr("(different names for enum fields)", sizeof(
unsigned char), 34u); bad= 1; break;} if( _temp1517->tag ==  _temp1518->tag){
continue;} if( _temp1517->tag ==  0? 1: _temp1518->tag ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)", sizeof( unsigned char), 39u);
bad= 1; break;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1517->tag)) !=  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1518->tag))){ Cyc_Tcutil_failure_reason= _tag_arr("(different tag values for enum fields)",
sizeof( unsigned char), 39u); bad= 1; break;}} if( bad){ goto _LL1184;} if(
_temp1283 ==  0? _temp1279 ==  0: 0){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)", sizeof( unsigned char), 39u);
goto _LL1184;} _LL1198: if((( _temp1291 !=  0? _temp1299 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1291),( struct _tuple1*) _check_null(
_temp1299)) ==  0: 0)? 1:( _temp1291 ==  0? _temp1299 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1289, _temp1297); return;} Cyc_Tcutil_failure_reason= _tag_arr("(different union types)",
sizeof( unsigned char), 24u); goto _LL1184; _LL1200: if( _temp1326 ==  _temp1313?
1: Cyc_Absyn_qvar_cmp( _temp1326->name, _temp1313->name) ==  0){ Cyc_Tcutil_unify_it(
_temp1305, _temp1318); Cyc_Tcutil_unify_list( _temp1307, _temp1320); return;}
Cyc_Tcutil_failure_reason= _tag_arr("(different tunion types)", sizeof(
unsigned char), 25u); goto _LL1184; _LL1202: if(( _temp1349 ==  _temp1337? 1:
Cyc_Absyn_qvar_cmp( _temp1349->name, _temp1337->name) ==  0)? _temp1347 == 
_temp1335? 1: Cyc_Absyn_qvar_cmp( _temp1347->name, _temp1335->name) ==  0: 0){
Cyc_Tcutil_unify_list( _temp1331, _temp1343); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)", sizeof( unsigned char), 31u); goto
_LL1184; _LL1204: Cyc_Tcutil_unify_it( _temp1363, _temp1377); Cyc_Tcutil_unify_it(
_temp1375, _temp1361); Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_unify_tqual(
_temp1357, _temp1371); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1355, _temp1369, _tag_arr("(different pointer bounds)", sizeof(
unsigned char), 27u));{ void* _temp1519=( void*)( Cyc_Absyn_compress_conref(
_temp1355))->v; void* _temp1525; _LL1521: if(( unsigned int) _temp1519 >  1u?*((
int*) _temp1519) ==  Cyc_Absyn_Eq_constr: 0){ _LL1526: _temp1525=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1519)->f1; if( _temp1525 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1522;} else{ goto _LL1523;}} else{ goto _LL1523;}
_LL1523: goto _LL1524; _LL1522: return; _LL1524: goto _LL1520; _LL1520:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y,
struct _tagged_arr reason)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp1359, _temp1373, _tag_arr("(different pointer types)", sizeof(
unsigned char), 26u)); return; _LL1206: if( _temp1383 ==  _temp1389? _temp1381
==  _temp1387: 0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different integral types)",
sizeof( unsigned char), 27u); goto _LL1184; _LL1208: return; _LL1210: if(
_temp1401 ==  _temp1397){ return;} goto _LL1184; _LL1212: Cyc_Tcutil_unify_tqual(
_temp1407, _temp1415); Cyc_Tcutil_unify_it( _temp1409, _temp1417); if( _temp1413
==  _temp1405){ return;} if( _temp1413 ==  0? 1: _temp1405 ==  0){ goto _LL1184;}
if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1413)) ==  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1405))){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different array sizes)",
sizeof( unsigned char), 24u); goto _LL1184; _LL1214: { int done= 0;{ struct
_RegionHandle _temp1527= _new_region(); struct _RegionHandle* rgn=& _temp1527;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1437 !=  0) {
if( _temp1457 ==  0){ Cyc_Tcutil_failure_reason= _tag_arr("(second function type has too few type variables)",
sizeof( unsigned char), 50u); break;} inst=({ struct Cyc_List_List* _temp1528=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1528->hd=( void*)({ struct _tuple5* _temp1529=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1529->f1=( struct Cyc_Absyn_Tvar*)
_temp1457->hd; _temp1529->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1530=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1530[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1531; _temp1531.tag= Cyc_Absyn_VarType;
_temp1531.f1=( struct Cyc_Absyn_Tvar*) _temp1437->hd; _temp1531;}); _temp1530;});
_temp1529;}); _temp1528->tl= inst; _temp1528;}); _temp1437= _temp1437->tl;
_temp1457= _temp1457->tl;} if( _temp1457 !=  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too many type variables)", sizeof(
unsigned char), 51u); _npop_handler( 0u); goto _LL1184;} if( inst !=  0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1532=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1532[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1533; _temp1533.tag= Cyc_Absyn_FnType; _temp1533.f1=({ struct Cyc_Absyn_FnInfo
_temp1534; _temp1534.tvars= 0; _temp1534.effect= _temp1435; _temp1534.ret_typ=(
void*) _temp1433; _temp1534.args= _temp1431; _temp1534.c_varargs= _temp1429;
_temp1534.cyc_varargs= _temp1427; _temp1534.rgn_po= _temp1425; _temp1534.attributes=
_temp1423; _temp1534;}); _temp1533;}); _temp1532;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1535=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1535[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1536; _temp1536.tag= Cyc_Absyn_FnType; _temp1536.f1=({ struct Cyc_Absyn_FnInfo
_temp1537; _temp1537.tvars= 0; _temp1537.effect= _temp1455; _temp1537.ret_typ=(
void*) _temp1453; _temp1537.args= _temp1451; _temp1537.c_varargs= _temp1449;
_temp1537.cyc_varargs= _temp1447; _temp1537.rgn_po= _temp1445; _temp1537.attributes=
_temp1443; _temp1537;}); _temp1536;}); _temp1535;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1433, _temp1453); for( 0;
_temp1431 !=  0? _temp1451 !=  0: 0;( _temp1431= _temp1431->tl, _temp1451=
_temp1451->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*) _temp1431->hd)).f2,(*((
struct _tuple2*) _temp1451->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)
_temp1431->hd)).f3,(*(( struct _tuple2*) _temp1451->hd)).f3);} Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; if( _temp1431 !=  0? 1: _temp1451 !=  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)", sizeof(
unsigned char), 52u); goto _LL1184;} if( _temp1429 !=  _temp1449){ Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)", sizeof( unsigned char), 41u);
goto _LL1184;}{ int bad_cyc_vararg= 0;{ struct _tuple11 _temp1539=({ struct
_tuple11 _temp1538; _temp1538.f1= _temp1427; _temp1538.f2= _temp1447; _temp1538;});
struct Cyc_Absyn_VarargInfo* _temp1549; struct Cyc_Absyn_VarargInfo* _temp1551;
struct Cyc_Absyn_VarargInfo* _temp1553; struct Cyc_Absyn_VarargInfo* _temp1555;
struct Cyc_Absyn_VarargInfo* _temp1557; struct Cyc_Absyn_VarargInfo _temp1559;
int _temp1560; void* _temp1562; struct Cyc_Absyn_Tqual _temp1564; struct Cyc_Core_Opt*
_temp1566; struct Cyc_Absyn_VarargInfo* _temp1568; struct Cyc_Absyn_VarargInfo
_temp1570; int _temp1571; void* _temp1573; struct Cyc_Absyn_Tqual _temp1575;
struct Cyc_Core_Opt* _temp1577; _LL1541: _LL1552: _temp1551= _temp1539.f1; if(
_temp1551 ==  0){ goto _LL1550;} else{ goto _LL1543;} _LL1550: _temp1549=
_temp1539.f2; if( _temp1549 ==  0){ goto _LL1542;} else{ goto _LL1543;} _LL1543:
_LL1554: _temp1553= _temp1539.f1; if( _temp1553 ==  0){ goto _LL1544;} else{
goto _LL1545;} _LL1545: _LL1556: _temp1555= _temp1539.f2; if( _temp1555 ==  0){
goto _LL1546;} else{ goto _LL1547;} _LL1547: _LL1569: _temp1568= _temp1539.f1;
if( _temp1568 ==  0){ goto _LL1540;} else{ _temp1570=* _temp1568; _LL1578:
_temp1577= _temp1570.name; goto _LL1576; _LL1576: _temp1575= _temp1570.tq; goto
_LL1574; _LL1574: _temp1573=( void*) _temp1570.type; goto _LL1572; _LL1572:
_temp1571= _temp1570.inject; goto _LL1558;} _LL1558: _temp1557= _temp1539.f2;
if( _temp1557 ==  0){ goto _LL1540;} else{ _temp1559=* _temp1557; _LL1567:
_temp1566= _temp1559.name; goto _LL1565; _LL1565: _temp1564= _temp1559.tq; goto
_LL1563; _LL1563: _temp1562=( void*) _temp1559.type; goto _LL1561; _LL1561:
_temp1560= _temp1559.inject; goto _LL1548;} _LL1542: goto _LL1540; _LL1544: goto
_LL1546; _LL1546: bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type takes varargs)",
sizeof( unsigned char), 39u); goto _LL1540; _LL1548: Cyc_Tcutil_unify_tqual(
_temp1575, _temp1564); Cyc_Tcutil_unify_it( _temp1573, _temp1562); if( _temp1571
!=  _temp1560){ bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type injects varargs)",
sizeof( unsigned char), 41u);} goto _LL1540; _LL1540:;} if( bad_cyc_vararg){
goto _LL1184;}{ int bad_effect= 0;{ struct _tuple12 _temp1580=({ struct _tuple12
_temp1579; _temp1579.f1= _temp1435; _temp1579.f2= _temp1455; _temp1579;});
struct Cyc_Core_Opt* _temp1590; struct Cyc_Core_Opt* _temp1592; struct Cyc_Core_Opt*
_temp1594; struct Cyc_Core_Opt* _temp1596; _LL1582: _LL1593: _temp1592=
_temp1580.f1; if( _temp1592 ==  0){ goto _LL1591;} else{ goto _LL1584;} _LL1591:
_temp1590= _temp1580.f2; if( _temp1590 ==  0){ goto _LL1583;} else{ goto _LL1584;}
_LL1584: _LL1595: _temp1594= _temp1580.f1; if( _temp1594 ==  0){ goto _LL1585;}
else{ goto _LL1586;} _LL1586: _LL1597: _temp1596= _temp1580.f2; if( _temp1596 == 
0){ goto _LL1587;} else{ goto _LL1588;} _LL1588: goto _LL1589; _LL1583: goto
_LL1581; _LL1585: goto _LL1587; _LL1587: bad_effect= 1; goto _LL1581; _LL1589:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1435))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1455))->v); goto _LL1581; _LL1581:;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; if( bad_effect){ Cyc_Tcutil_failure_reason= _tag_arr("(function type effects do not unify)",
sizeof( unsigned char), 37u); goto _LL1184;} if( ! Cyc_Tcutil_same_atts(
_temp1443, _temp1423)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different attributes)",
sizeof( unsigned char), 43u); goto _LL1184;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1445, _temp1425)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different region lifetime orderings)",
sizeof( unsigned char), 58u); goto _LL1184;} return;}}} _LL1216: for( 0;
_temp1461 !=  0? _temp1465 !=  0: 0;( _temp1461= _temp1461->tl, _temp1465=
_temp1465->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*) _temp1461->hd)).f1,(*((
struct _tuple4*) _temp1465->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)
_temp1461->hd)).f2,(*(( struct _tuple4*) _temp1465->hd)).f2);} if( _temp1461 == 
0? _temp1465 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(tuple types have different numbers of components)",
sizeof( unsigned char), 51u); goto _LL1184; _LL1218: _temp1481= _temp1473;
_temp1477= _temp1469; goto _LL1220; _LL1220: for( 0; _temp1477 !=  0? _temp1481
!=  0: 0;( _temp1477= _temp1477->tl, _temp1481= _temp1481->tl)){ struct Cyc_Absyn_Structfield*
_temp1598=( struct Cyc_Absyn_Structfield*) _temp1477->hd; struct Cyc_Absyn_Structfield*
_temp1599=( struct Cyc_Absyn_Structfield*) _temp1481->hd; if( Cyc_Std_zstrptrcmp(
_temp1598->name, _temp1599->name) !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(different member names in anonymous structs/unions)",
sizeof( unsigned char), 53u);( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1598->tq, _temp1599->tq); Cyc_Tcutil_unify_it(( void*) _temp1598->type,(
void*) _temp1599->type); if( ! Cyc_Tcutil_same_atts( _temp1598->attributes,
_temp1599->attributes)){ Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2;
Cyc_Tcutil_failure_reason= _tag_arr("(different attributes on member)", sizeof(
unsigned char), 33u);( int) _throw(( void*) Cyc_Tcutil_Unify);} if((( _temp1598->width
!=  0? _temp1599->width ==  0: 0)? 1:( _temp1599->width !=  0? _temp1598->width
==  0: 0))? 1:(( _temp1598->width !=  0? _temp1599->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1598->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1599->width)): 0)){ Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason= _tag_arr("(different bitfield widths on member)",
sizeof( unsigned char), 38u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} if(
_temp1477 ==  0? _temp1481 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(different number of members)", sizeof(
unsigned char), 30u); goto _LL1184; _LL1222: return; _LL1224: Cyc_Tcutil_unify_it(
_temp1493, _temp1489); return; _LL1226: goto _LL1228; _LL1228: goto _LL1230;
_LL1230: goto _LL1232; _LL1232: goto _LL1234; _LL1234: goto _LL1236; _LL1236:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)", sizeof( unsigned char), 22u); goto _LL1184;
_LL1238: goto _LL1184; _LL1184:;}( int) _throw(( void*) Cyc_Tcutil_Unify);} int
Cyc_Tcutil_opt_cmp( int(* cmp)( void*, void*), struct Cyc_Core_Opt* a1, struct
Cyc_Core_Opt* a2){ if( a1 ==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return
- 1;} if( a1 !=  0? a2 ==  0: 0){ return 1;} return cmp(( void*)(( struct Cyc_Core_Opt*)
_check_null( a1))->v,( void*)(( struct Cyc_Core_Opt*) _check_null( a2))->v);}
int Cyc_Tcutil_star_cmp( int(* cmp)( void**, void**), void** a1, void** a2){ if(
a1 ==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return - 1;} if( a1 !=  0? a2
==  0: 0){ return 1;} return cmp(( void**) _check_null( a1),( void**)
_check_null( a2));} int Cyc_Tcutil_list_cmp( int(* cmp)( void*, void*), struct
Cyc_List_List* l1, struct Cyc_List_List* l2){ if( l1 ==  l2){ return 0;} for( 0;
l1 !=  0? l2 !=  0: 0;( l1= l1->tl, l2= l2->tl)){ int _temp1600= cmp(( void*) l1->hd,(
void*) l2->hd); if( _temp1600 !=  0){ return _temp1600;}} if( l2 !=  0){ return
- 1;} if( l1 !=  0){ return 1;} return 0;} static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ int _temp1601=( tq1.q_const
+ ( tq1.q_volatile <<  1)) + ( tq1.q_restrict <<  2); int _temp1602=( tq2.q_const
+ ( tq2.q_volatile <<  1)) + ( tq2.q_restrict <<  2); return Cyc_Core_intcmp(
_temp1601, _temp1602);} static int Cyc_Tcutil_conrefs_cmp( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ x= Cyc_Absyn_compress_conref(
x); y= Cyc_Absyn_compress_conref( y); if( x ==  y){ return 0;}{ void* _temp1603=(
void*) x->v; void* _temp1611; _LL1605: if( _temp1603 == ( void*) Cyc_Absyn_No_constr){
goto _LL1606;} else{ goto _LL1607;} _LL1607: if(( unsigned int) _temp1603 >  1u?*((
int*) _temp1603) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL1608;} else{ goto
_LL1609;} _LL1609: if(( unsigned int) _temp1603 >  1u?*(( int*) _temp1603) == 
Cyc_Absyn_Eq_constr: 0){ _LL1612: _temp1611=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1603)->f1; goto _LL1610;} else{ goto _LL1604;} _LL1606: return - 1; _LL1608:(
int) _throw(({ void* _temp1613[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress",
sizeof( unsigned char), 37u), _tag_arr( _temp1613, sizeof( void*), 0u));}));
_LL1610: { void* _temp1614=( void*) y->v; void* _temp1622; _LL1616: if(
_temp1614 == ( void*) Cyc_Absyn_No_constr){ goto _LL1617;} else{ goto _LL1618;}
_LL1618: if(( unsigned int) _temp1614 >  1u?*(( int*) _temp1614) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL1619;} else{ goto _LL1620;} _LL1620: if(( unsigned int) _temp1614 > 
1u?*(( int*) _temp1614) ==  Cyc_Absyn_Eq_constr: 0){ _LL1623: _temp1622=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1614)->f1; goto _LL1621;} else{ goto
_LL1615;} _LL1617: return 1; _LL1619:( int) _throw(({ void* _temp1624[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress(2)", sizeof(
unsigned char), 40u), _tag_arr( _temp1624, sizeof( void*), 0u));})); _LL1621:
return cmp( _temp1611, _temp1622); _LL1615:;} _LL1604:;}} static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4* tqt1, struct _tuple4* tqt2){ struct _tuple4 _temp1627; void*
_temp1628; struct Cyc_Absyn_Tqual _temp1630; struct _tuple4* _temp1625= tqt1;
_temp1627=* _temp1625; _LL1631: _temp1630= _temp1627.f1; goto _LL1629; _LL1629:
_temp1628= _temp1627.f2; goto _LL1626; _LL1626: { struct _tuple4 _temp1634; void*
_temp1635; struct Cyc_Absyn_Tqual _temp1637; struct _tuple4* _temp1632= tqt2;
_temp1634=* _temp1632; _LL1638: _temp1637= _temp1634.f1; goto _LL1636; _LL1636:
_temp1635= _temp1634.f2; goto _LL1633; _LL1633: { int _temp1639= Cyc_Tcutil_tqual_cmp(
_temp1630, _temp1637); if( _temp1639 !=  0){ return _temp1639;} return Cyc_Tcutil_typecmp(
_temp1628, _temp1635);}}} static int Cyc_Tcutil_const_uint_exp_cmp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ return Cyc_Core_intcmp(( int) Cyc_Evexp_eval_const_uint_exp(
e1),( int) Cyc_Evexp_eval_const_uint_exp( e2));} static int Cyc_Tcutil_attribute_case_number(
void* att){ void* _temp1640= att; _LL1642: if(( unsigned int) _temp1640 >  16u?*((
int*) _temp1640) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL1643;} else{ goto
_LL1644;} _LL1644: if( _temp1640 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1645;} else{ goto _LL1646;} _LL1646: if( _temp1640 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1647;} else{ goto _LL1648;} _LL1648: if( _temp1640 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1649;} else{ goto _LL1650;} _LL1650: if( _temp1640 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL1651;} else{ goto _LL1652;} _LL1652: if( _temp1640 == ( void*) Cyc_Absyn_Const_att){
goto _LL1653;} else{ goto _LL1654;} _LL1654: if(( unsigned int) _temp1640 >  16u?*((
int*) _temp1640) ==  Cyc_Absyn_Aligned_att: 0){ goto _LL1655;} else{ goto
_LL1656;} _LL1656: if( _temp1640 == ( void*) Cyc_Absyn_Packed_att){ goto _LL1657;}
else{ goto _LL1658;} _LL1658: if(( unsigned int) _temp1640 >  16u?*(( int*)
_temp1640) ==  Cyc_Absyn_Section_att: 0){ goto _LL1659;} else{ goto _LL1660;}
_LL1660: if( _temp1640 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL1661;} else{
goto _LL1662;} _LL1662: if( _temp1640 == ( void*) Cyc_Absyn_Shared_att){ goto
_LL1663;} else{ goto _LL1664;} _LL1664: if( _temp1640 == ( void*) Cyc_Absyn_Unused_att){
goto _LL1665;} else{ goto _LL1666;} _LL1666: if( _temp1640 == ( void*) Cyc_Absyn_Weak_att){
goto _LL1667;} else{ goto _LL1668;} _LL1668: if( _temp1640 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL1669;} else{ goto _LL1670;} _LL1670: if( _temp1640 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL1671;} else{ goto _LL1672;} _LL1672: if( _temp1640 == ( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL1673;} else{ goto _LL1674;} _LL1674: if( _temp1640 == ( void*) Cyc_Absyn_Constructor_att){
goto _LL1675;} else{ goto _LL1676;} _LL1676: if( _temp1640 == ( void*) Cyc_Absyn_Destructor_att){
goto _LL1677;} else{ goto _LL1678;} _LL1678: if( _temp1640 == ( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL1679;} else{ goto _LL1680;} _LL1680: if(( unsigned int) _temp1640 >  16u?*((
int*) _temp1640) ==  Cyc_Absyn_Format_att: 0){ goto _LL1681;} else{ goto _LL1641;}
_LL1643: return 0; _LL1645: return 1; _LL1647: return 2; _LL1649: return 3;
_LL1651: return 4; _LL1653: return 5; _LL1655: return 6; _LL1657: return 7;
_LL1659: return 8; _LL1661: return 9; _LL1663: return 10; _LL1665: return 11;
_LL1667: return 12; _LL1669: return 13; _LL1671: return 14; _LL1673: return 15;
_LL1675: return 16; _LL1677: return 17; _LL1679: return 18; _LL1681: return 19;
_LL1641:;} static int Cyc_Tcutil_attribute_cmp( void* att1, void* att2){ struct
_tuple8 _temp1683=({ struct _tuple8 _temp1682; _temp1682.f1= att1; _temp1682.f2=
att2; _temp1682;}); void* _temp1695; int _temp1697; void* _temp1699; int
_temp1701; void* _temp1703; int _temp1705; void* _temp1707; int _temp1709; void*
_temp1711; struct _tagged_arr _temp1713; void* _temp1715; struct _tagged_arr
_temp1717; void* _temp1719; int _temp1721; int _temp1723; void* _temp1725; void*
_temp1727; int _temp1729; int _temp1731; void* _temp1733; _LL1685: _LL1700:
_temp1699= _temp1683.f1; if(( unsigned int) _temp1699 >  16u?*(( int*) _temp1699)
==  Cyc_Absyn_Regparm_att: 0){ _LL1702: _temp1701=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp1699)->f1; goto _LL1696;} else{ goto _LL1687;} _LL1696: _temp1695=
_temp1683.f2; if(( unsigned int) _temp1695 >  16u?*(( int*) _temp1695) ==  Cyc_Absyn_Regparm_att:
0){ _LL1698: _temp1697=(( struct Cyc_Absyn_Regparm_att_struct*) _temp1695)->f1;
goto _LL1686;} else{ goto _LL1687;} _LL1687: _LL1708: _temp1707= _temp1683.f1;
if(( unsigned int) _temp1707 >  16u?*(( int*) _temp1707) ==  Cyc_Absyn_Aligned_att:
0){ _LL1710: _temp1709=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1707)->f1;
goto _LL1704;} else{ goto _LL1689;} _LL1704: _temp1703= _temp1683.f2; if((
unsigned int) _temp1703 >  16u?*(( int*) _temp1703) ==  Cyc_Absyn_Aligned_att: 0){
_LL1706: _temp1705=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1703)->f1; goto
_LL1688;} else{ goto _LL1689;} _LL1689: _LL1716: _temp1715= _temp1683.f1; if((
unsigned int) _temp1715 >  16u?*(( int*) _temp1715) ==  Cyc_Absyn_Section_att: 0){
_LL1718: _temp1717=(( struct Cyc_Absyn_Section_att_struct*) _temp1715)->f1; goto
_LL1712;} else{ goto _LL1691;} _LL1712: _temp1711= _temp1683.f2; if((
unsigned int) _temp1711 >  16u?*(( int*) _temp1711) ==  Cyc_Absyn_Section_att: 0){
_LL1714: _temp1713=(( struct Cyc_Absyn_Section_att_struct*) _temp1711)->f1; goto
_LL1690;} else{ goto _LL1691;} _LL1691: _LL1728: _temp1727= _temp1683.f1; if((
unsigned int) _temp1727 >  16u?*(( int*) _temp1727) ==  Cyc_Absyn_Format_att: 0){
_LL1734: _temp1733=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1727)->f1;
goto _LL1732; _LL1732: _temp1731=(( struct Cyc_Absyn_Format_att_struct*)
_temp1727)->f2; goto _LL1730; _LL1730: _temp1729=(( struct Cyc_Absyn_Format_att_struct*)
_temp1727)->f3; goto _LL1720;} else{ goto _LL1693;} _LL1720: _temp1719=
_temp1683.f2; if(( unsigned int) _temp1719 >  16u?*(( int*) _temp1719) ==  Cyc_Absyn_Format_att:
0){ _LL1726: _temp1725=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1719)->f1;
goto _LL1724; _LL1724: _temp1723=(( struct Cyc_Absyn_Format_att_struct*)
_temp1719)->f2; goto _LL1722; _LL1722: _temp1721=(( struct Cyc_Absyn_Format_att_struct*)
_temp1719)->f3; goto _LL1692;} else{ goto _LL1693;} _LL1693: goto _LL1694;
_LL1686: _temp1709= _temp1701; _temp1705= _temp1697; goto _LL1688; _LL1688:
return Cyc_Core_intcmp( _temp1709, _temp1705); _LL1690: return Cyc_Std_strcmp(
_temp1717, _temp1713); _LL1692: { int _temp1735= Cyc_Core_intcmp(( int)((
unsigned int) _temp1733),( int)(( unsigned int) _temp1725)); if( _temp1735 !=  0){
return _temp1735;}{ int _temp1736= Cyc_Core_intcmp( _temp1731, _temp1723); if(
_temp1736 !=  0){ return _temp1736;} return Cyc_Core_intcmp( _temp1729,
_temp1721);}} _LL1694: { int _temp1737= Cyc_Tcutil_attribute_case_number( att1);
int _temp1738= Cyc_Tcutil_attribute_case_number( att2); return Cyc_Core_intcmp(
_temp1737, _temp1738);} _LL1684:;} static int Cyc_Tcutil_structfield_cmp( struct
Cyc_Absyn_Structfield* f1, struct Cyc_Absyn_Structfield* f2){ int _temp1739= Cyc_Std_zstrptrcmp(
f1->name, f2->name); if( _temp1739 !=  0){ return _temp1739;}{ int _temp1740=
Cyc_Tcutil_tqual_cmp( f1->tq, f2->tq); if( _temp1740 !=  0){ return _temp1740;}{
int _temp1741= Cyc_Tcutil_typecmp(( void*) f1->type,( void*) f2->type); if(
_temp1741 !=  0){ return _temp1741;}{ int _temp1742= Cyc_Tcutil_list_cmp( Cyc_Tcutil_attribute_cmp,
f1->attributes, f2->attributes); if( _temp1742 !=  0){ return _temp1742;} return((
int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp*
a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
f1->width, f2->width);}}}} static int Cyc_Tcutil_enumfield_cmp( struct Cyc_Absyn_Enumfield*
e1, struct Cyc_Absyn_Enumfield* e2){ int _temp1743= Cyc_Absyn_qvar_cmp( e1->name,
e2->name); if( _temp1743 !=  0){ return _temp1743;} return(( int(*)( int(* cmp)(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp* a1, struct
Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp, e1->tag,
e2->tag);} extern int Cyc_Tcutil_typecmp( void* t1, void* t2); int Cyc_Tcutil_type_case_number(
void* t){ void* _temp1744= t; _LL1746: if( _temp1744 == ( void*) Cyc_Absyn_VoidType){
goto _LL1747;} else{ goto _LL1748;} _LL1748: if(( unsigned int) _temp1744 >  3u?*((
int*) _temp1744) ==  Cyc_Absyn_Evar: 0){ goto _LL1749;} else{ goto _LL1750;}
_LL1750: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744) ==  Cyc_Absyn_VarType:
0){ goto _LL1751;} else{ goto _LL1752;} _LL1752: if(( unsigned int) _temp1744 > 
3u?*(( int*) _temp1744) ==  Cyc_Absyn_TunionType: 0){ goto _LL1753;} else{ goto
_LL1754;} _LL1754: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1755;} else{ goto _LL1756;} _LL1756: if((
unsigned int) _temp1744 >  3u?*(( int*) _temp1744) ==  Cyc_Absyn_PointerType: 0){
goto _LL1757;} else{ goto _LL1758;} _LL1758: if(( unsigned int) _temp1744 >  3u?*((
int*) _temp1744) ==  Cyc_Absyn_IntType: 0){ goto _LL1759;} else{ goto _LL1760;}
_LL1760: if( _temp1744 == ( void*) Cyc_Absyn_FloatType){ goto _LL1761;} else{
goto _LL1762;} _LL1762: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744)
==  Cyc_Absyn_DoubleType: 0){ goto _LL1763;} else{ goto _LL1764;} _LL1764: if((
unsigned int) _temp1744 >  3u?*(( int*) _temp1744) ==  Cyc_Absyn_ArrayType: 0){
goto _LL1765;} else{ goto _LL1766;} _LL1766: if(( unsigned int) _temp1744 >  3u?*((
int*) _temp1744) ==  Cyc_Absyn_FnType: 0){ goto _LL1767;} else{ goto _LL1768;}
_LL1768: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744) ==  Cyc_Absyn_TupleType:
0){ goto _LL1769;} else{ goto _LL1770;} _LL1770: if(( unsigned int) _temp1744 > 
3u?*(( int*) _temp1744) ==  Cyc_Absyn_StructType: 0){ goto _LL1771;} else{ goto
_LL1772;} _LL1772: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744) == 
Cyc_Absyn_UnionType: 0){ goto _LL1773;} else{ goto _LL1774;} _LL1774: if((
unsigned int) _temp1744 >  3u?*(( int*) _temp1744) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL1775;} else{ goto _LL1776;} _LL1776: if(( unsigned int) _temp1744 > 
3u?*(( int*) _temp1744) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL1777;} else{
goto _LL1778;} _LL1778: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744)
==  Cyc_Absyn_EnumType: 0){ goto _LL1779;} else{ goto _LL1780;} _LL1780: if((
unsigned int) _temp1744 >  3u?*(( int*) _temp1744) ==  Cyc_Absyn_AnonEnumType: 0){
goto _LL1781;} else{ goto _LL1782;} _LL1782: if(( unsigned int) _temp1744 >  3u?*((
int*) _temp1744) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL1783;} else{ goto
_LL1784;} _LL1784: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744) == 
Cyc_Absyn_TypedefType: 0){ goto _LL1785;} else{ goto _LL1786;} _LL1786: if(
_temp1744 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1787;} else{ goto _LL1788;}
_LL1788: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1789;} else{ goto _LL1790;} _LL1790: if(( unsigned int) _temp1744 > 
3u?*(( int*) _temp1744) ==  Cyc_Absyn_JoinEff: 0){ goto _LL1791;} else{ goto
_LL1792;} _LL1792: if(( unsigned int) _temp1744 >  3u?*(( int*) _temp1744) == 
Cyc_Absyn_RgnsEff: 0){ goto _LL1793;} else{ goto _LL1745;} _LL1747: return 0;
_LL1749: return 1; _LL1751: return 2; _LL1753: return 3; _LL1755: return 4;
_LL1757: return 5; _LL1759: return 6; _LL1761: return 7; _LL1763: return 8;
_LL1765: return 9; _LL1767: return 10; _LL1769: return 11; _LL1771: return 12;
_LL1773: return 13; _LL1775: return 14; _LL1777: return 15; _LL1779: return 16;
_LL1781: return 17; _LL1783: return 18; _LL1785: return 19; _LL1787: return 20;
_LL1789: return 21; _LL1791: return 22; _LL1793: return 23; _LL1745:;} int Cyc_Tcutil_shallow_typecmp_it(
void* t1, void* t2){ return Cyc_Core_intcmp( Cyc_Tcutil_type_case_number( t1),
Cyc_Tcutil_type_case_number( t2));} int Cyc_Tcutil_typecmp( void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 ==  t2){
return 0;}{ int _temp1794= Cyc_Tcutil_shallow_typecmp_it( t1, t2); if( _temp1794
!=  0){ return _temp1794;}{ struct _tuple8 _temp1796=({ struct _tuple8 _temp1795;
_temp1795.f1= t2; _temp1795.f2= t1; _temp1795;}); void* _temp1852; void*
_temp1854; void* _temp1856; void* _temp1858; void* _temp1860; struct Cyc_Absyn_Tvar*
_temp1862; void* _temp1864; struct Cyc_Absyn_Tvar* _temp1866; void* _temp1868;
struct Cyc_List_List* _temp1870; struct _tuple1* _temp1872; void* _temp1874;
struct Cyc_List_List* _temp1876; struct _tuple1* _temp1878; void* _temp1880;
struct _tuple1* _temp1882; void* _temp1884; struct _tuple1* _temp1886; void*
_temp1888; struct Cyc_List_List* _temp1890; void* _temp1892; struct Cyc_List_List*
_temp1894; void* _temp1896; struct Cyc_Absyn_Uniondecl** _temp1898; struct Cyc_List_List*
_temp1900; struct _tuple1* _temp1902; void* _temp1904; struct Cyc_Absyn_Uniondecl**
_temp1906; struct Cyc_List_List* _temp1908; struct _tuple1* _temp1910; void*
_temp1912; struct Cyc_Absyn_TunionInfo _temp1914; void* _temp1916; struct Cyc_List_List*
_temp1918; void* _temp1920; struct Cyc_Absyn_Tuniondecl** _temp1922; struct Cyc_Absyn_Tuniondecl*
_temp1924; void* _temp1925; struct Cyc_Absyn_TunionInfo _temp1927; void*
_temp1929; struct Cyc_List_List* _temp1931; void* _temp1933; struct Cyc_Absyn_Tuniondecl**
_temp1935; struct Cyc_Absyn_Tuniondecl* _temp1937; void* _temp1938; struct Cyc_Absyn_TunionFieldInfo
_temp1940; struct Cyc_List_List* _temp1942; void* _temp1944; struct Cyc_Absyn_Tunionfield*
_temp1946; struct Cyc_Absyn_Tuniondecl* _temp1948; void* _temp1950; struct Cyc_Absyn_TunionFieldInfo
_temp1952; struct Cyc_List_List* _temp1954; void* _temp1956; struct Cyc_Absyn_Tunionfield*
_temp1958; struct Cyc_Absyn_Tuniondecl* _temp1960; void* _temp1962; struct Cyc_Absyn_PtrInfo
_temp1964; struct Cyc_Absyn_Conref* _temp1966; struct Cyc_Absyn_Tqual _temp1968;
struct Cyc_Absyn_Conref* _temp1970; void* _temp1972; void* _temp1974; void*
_temp1976; struct Cyc_Absyn_PtrInfo _temp1978; struct Cyc_Absyn_Conref*
_temp1980; struct Cyc_Absyn_Tqual _temp1982; struct Cyc_Absyn_Conref* _temp1984;
void* _temp1986; void* _temp1988; void* _temp1990; void* _temp1992; void*
_temp1994; void* _temp1996; void* _temp1998; void* _temp2000; void* _temp2002;
void* _temp2004; void* _temp2006; int _temp2008; void* _temp2010; int _temp2012;
void* _temp2014; struct Cyc_Absyn_Exp* _temp2016; struct Cyc_Absyn_Tqual
_temp2018; void* _temp2020; void* _temp2022; struct Cyc_Absyn_Exp* _temp2024;
struct Cyc_Absyn_Tqual _temp2026; void* _temp2028; void* _temp2030; struct Cyc_Absyn_FnInfo
_temp2032; struct Cyc_List_List* _temp2034; struct Cyc_List_List* _temp2036;
struct Cyc_Absyn_VarargInfo* _temp2038; int _temp2040; struct Cyc_List_List*
_temp2042; void* _temp2044; struct Cyc_Core_Opt* _temp2046; struct Cyc_List_List*
_temp2048; void* _temp2050; struct Cyc_Absyn_FnInfo _temp2052; struct Cyc_List_List*
_temp2054; struct Cyc_List_List* _temp2056; struct Cyc_Absyn_VarargInfo*
_temp2058; int _temp2060; struct Cyc_List_List* _temp2062; void* _temp2064;
struct Cyc_Core_Opt* _temp2066; struct Cyc_List_List* _temp2068; void* _temp2070;
struct Cyc_List_List* _temp2072; void* _temp2074; struct Cyc_List_List*
_temp2076; void* _temp2078; struct Cyc_List_List* _temp2080; void* _temp2082;
struct Cyc_List_List* _temp2084; void* _temp2086; struct Cyc_List_List*
_temp2088; void* _temp2090; struct Cyc_List_List* _temp2092; void* _temp2094;
void* _temp2096; void* _temp2098; void* _temp2100; void* _temp2102; void*
_temp2104; void* _temp2106; void* _temp2108; void* _temp2110; void* _temp2112;
void* _temp2114; void* _temp2116; _LL1798: _LL1855: _temp1854= _temp1796.f1; if((
unsigned int) _temp1854 >  3u?*(( int*) _temp1854) ==  Cyc_Absyn_Evar: 0){ goto
_LL1853;} else{ goto _LL1800;} _LL1853: _temp1852= _temp1796.f2; if((
unsigned int) _temp1852 >  3u?*(( int*) _temp1852) ==  Cyc_Absyn_Evar: 0){ goto
_LL1799;} else{ goto _LL1800;} _LL1800: _LL1859: _temp1858= _temp1796.f1; if(
_temp1858 == ( void*) Cyc_Absyn_VoidType){ goto _LL1857;} else{ goto _LL1802;}
_LL1857: _temp1856= _temp1796.f2; if( _temp1856 == ( void*) Cyc_Absyn_VoidType){
goto _LL1801;} else{ goto _LL1802;} _LL1802: _LL1865: _temp1864= _temp1796.f1;
if(( unsigned int) _temp1864 >  3u?*(( int*) _temp1864) ==  Cyc_Absyn_VarType: 0){
_LL1867: _temp1866=(( struct Cyc_Absyn_VarType_struct*) _temp1864)->f1; goto
_LL1861;} else{ goto _LL1804;} _LL1861: _temp1860= _temp1796.f2; if((
unsigned int) _temp1860 >  3u?*(( int*) _temp1860) ==  Cyc_Absyn_VarType: 0){
_LL1863: _temp1862=(( struct Cyc_Absyn_VarType_struct*) _temp1860)->f1; goto
_LL1803;} else{ goto _LL1804;} _LL1804: _LL1875: _temp1874= _temp1796.f1; if((
unsigned int) _temp1874 >  3u?*(( int*) _temp1874) ==  Cyc_Absyn_StructType: 0){
_LL1879: _temp1878=(( struct Cyc_Absyn_StructType_struct*) _temp1874)->f1; goto
_LL1877; _LL1877: _temp1876=(( struct Cyc_Absyn_StructType_struct*) _temp1874)->f2;
goto _LL1869;} else{ goto _LL1806;} _LL1869: _temp1868= _temp1796.f2; if((
unsigned int) _temp1868 >  3u?*(( int*) _temp1868) ==  Cyc_Absyn_StructType: 0){
_LL1873: _temp1872=(( struct Cyc_Absyn_StructType_struct*) _temp1868)->f1; goto
_LL1871; _LL1871: _temp1870=(( struct Cyc_Absyn_StructType_struct*) _temp1868)->f2;
goto _LL1805;} else{ goto _LL1806;} _LL1806: _LL1885: _temp1884= _temp1796.f1;
if(( unsigned int) _temp1884 >  3u?*(( int*) _temp1884) ==  Cyc_Absyn_EnumType:
0){ _LL1887: _temp1886=(( struct Cyc_Absyn_EnumType_struct*) _temp1884)->f1;
goto _LL1881;} else{ goto _LL1808;} _LL1881: _temp1880= _temp1796.f2; if((
unsigned int) _temp1880 >  3u?*(( int*) _temp1880) ==  Cyc_Absyn_EnumType: 0){
_LL1883: _temp1882=(( struct Cyc_Absyn_EnumType_struct*) _temp1880)->f1; goto
_LL1807;} else{ goto _LL1808;} _LL1808: _LL1893: _temp1892= _temp1796.f1; if((
unsigned int) _temp1892 >  3u?*(( int*) _temp1892) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1895: _temp1894=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1892)->f1;
goto _LL1889;} else{ goto _LL1810;} _LL1889: _temp1888= _temp1796.f2; if((
unsigned int) _temp1888 >  3u?*(( int*) _temp1888) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1891: _temp1890=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1888)->f1;
goto _LL1809;} else{ goto _LL1810;} _LL1810: _LL1905: _temp1904= _temp1796.f1;
if(( unsigned int) _temp1904 >  3u?*(( int*) _temp1904) ==  Cyc_Absyn_UnionType:
0){ _LL1911: _temp1910=(( struct Cyc_Absyn_UnionType_struct*) _temp1904)->f1;
goto _LL1909; _LL1909: _temp1908=(( struct Cyc_Absyn_UnionType_struct*)
_temp1904)->f2; goto _LL1907; _LL1907: _temp1906=(( struct Cyc_Absyn_UnionType_struct*)
_temp1904)->f3; goto _LL1897;} else{ goto _LL1812;} _LL1897: _temp1896=
_temp1796.f2; if(( unsigned int) _temp1896 >  3u?*(( int*) _temp1896) ==  Cyc_Absyn_UnionType:
0){ _LL1903: _temp1902=(( struct Cyc_Absyn_UnionType_struct*) _temp1896)->f1;
goto _LL1901; _LL1901: _temp1900=(( struct Cyc_Absyn_UnionType_struct*)
_temp1896)->f2; goto _LL1899; _LL1899: _temp1898=(( struct Cyc_Absyn_UnionType_struct*)
_temp1896)->f3; goto _LL1811;} else{ goto _LL1812;} _LL1812: _LL1926: _temp1925=
_temp1796.f1; if(( unsigned int) _temp1925 >  3u?*(( int*) _temp1925) ==  Cyc_Absyn_TunionType:
0){ _LL1928: _temp1927=(( struct Cyc_Absyn_TunionType_struct*) _temp1925)->f1;
_LL1934: _temp1933=( void*) _temp1927.tunion_info; if(*(( int*) _temp1933) == 
Cyc_Absyn_KnownTunion){ _LL1936: _temp1935=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1933)->f1; _temp1937=* _temp1935; goto _LL1932;} else{ goto _LL1814;}
_LL1932: _temp1931= _temp1927.targs; goto _LL1930; _LL1930: _temp1929=( void*)
_temp1927.rgn; goto _LL1913;} else{ goto _LL1814;} _LL1913: _temp1912= _temp1796.f2;
if(( unsigned int) _temp1912 >  3u?*(( int*) _temp1912) ==  Cyc_Absyn_TunionType:
0){ _LL1915: _temp1914=(( struct Cyc_Absyn_TunionType_struct*) _temp1912)->f1;
_LL1921: _temp1920=( void*) _temp1914.tunion_info; if(*(( int*) _temp1920) == 
Cyc_Absyn_KnownTunion){ _LL1923: _temp1922=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1920)->f1; _temp1924=* _temp1922; goto _LL1919;} else{ goto _LL1814;}
_LL1919: _temp1918= _temp1914.targs; goto _LL1917; _LL1917: _temp1916=( void*)
_temp1914.rgn; goto _LL1813;} else{ goto _LL1814;} _LL1814: _LL1951: _temp1950=
_temp1796.f1; if(( unsigned int) _temp1950 >  3u?*(( int*) _temp1950) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1953: _temp1952=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1950)->f1;
_LL1957: _temp1956=( void*) _temp1952.field_info; if(*(( int*) _temp1956) == 
Cyc_Absyn_KnownTunionfield){ _LL1961: _temp1960=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1956)->f1; goto _LL1959; _LL1959: _temp1958=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1956)->f2; goto _LL1955;} else{ goto _LL1816;} _LL1955: _temp1954=
_temp1952.targs; goto _LL1939;} else{ goto _LL1816;} _LL1939: _temp1938=
_temp1796.f2; if(( unsigned int) _temp1938 >  3u?*(( int*) _temp1938) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1941: _temp1940=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1938)->f1;
_LL1945: _temp1944=( void*) _temp1940.field_info; if(*(( int*) _temp1944) == 
Cyc_Absyn_KnownTunionfield){ _LL1949: _temp1948=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1944)->f1; goto _LL1947; _LL1947: _temp1946=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1944)->f2; goto _LL1943;} else{ goto _LL1816;} _LL1943: _temp1942=
_temp1940.targs; goto _LL1815;} else{ goto _LL1816;} _LL1816: _LL1977: _temp1976=
_temp1796.f1; if(( unsigned int) _temp1976 >  3u?*(( int*) _temp1976) ==  Cyc_Absyn_PointerType:
0){ _LL1979: _temp1978=(( struct Cyc_Absyn_PointerType_struct*) _temp1976)->f1;
_LL1989: _temp1988=( void*) _temp1978.elt_typ; goto _LL1987; _LL1987: _temp1986=(
void*) _temp1978.rgn_typ; goto _LL1985; _LL1985: _temp1984= _temp1978.nullable;
goto _LL1983; _LL1983: _temp1982= _temp1978.tq; goto _LL1981; _LL1981: _temp1980=
_temp1978.bounds; goto _LL1963;} else{ goto _LL1818;} _LL1963: _temp1962=
_temp1796.f2; if(( unsigned int) _temp1962 >  3u?*(( int*) _temp1962) ==  Cyc_Absyn_PointerType:
0){ _LL1965: _temp1964=(( struct Cyc_Absyn_PointerType_struct*) _temp1962)->f1;
_LL1975: _temp1974=( void*) _temp1964.elt_typ; goto _LL1973; _LL1973: _temp1972=(
void*) _temp1964.rgn_typ; goto _LL1971; _LL1971: _temp1970= _temp1964.nullable;
goto _LL1969; _LL1969: _temp1968= _temp1964.tq; goto _LL1967; _LL1967: _temp1966=
_temp1964.bounds; goto _LL1817;} else{ goto _LL1818;} _LL1818: _LL1997:
_temp1996= _temp1796.f1; if(( unsigned int) _temp1996 >  3u?*(( int*) _temp1996)
==  Cyc_Absyn_IntType: 0){ _LL2001: _temp2000=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1996)->f1; goto _LL1999; _LL1999: _temp1998=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1996)->f2; goto _LL1991;} else{ goto _LL1820;} _LL1991: _temp1990=
_temp1796.f2; if(( unsigned int) _temp1990 >  3u?*(( int*) _temp1990) ==  Cyc_Absyn_IntType:
0){ _LL1995: _temp1994=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1990)->f1;
goto _LL1993; _LL1993: _temp1992=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1990)->f2; goto _LL1819;} else{ goto _LL1820;} _LL1820: _LL2005: _temp2004=
_temp1796.f1; if( _temp2004 == ( void*) Cyc_Absyn_FloatType){ goto _LL2003;}
else{ goto _LL1822;} _LL2003: _temp2002= _temp1796.f2; if( _temp2002 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1821;} else{ goto _LL1822;} _LL1822: _LL2011:
_temp2010= _temp1796.f1; if(( unsigned int) _temp2010 >  3u?*(( int*) _temp2010)
==  Cyc_Absyn_DoubleType: 0){ _LL2013: _temp2012=(( struct Cyc_Absyn_DoubleType_struct*)
_temp2010)->f1; goto _LL2007;} else{ goto _LL1824;} _LL2007: _temp2006=
_temp1796.f2; if(( unsigned int) _temp2006 >  3u?*(( int*) _temp2006) ==  Cyc_Absyn_DoubleType:
0){ _LL2009: _temp2008=(( struct Cyc_Absyn_DoubleType_struct*) _temp2006)->f1;
goto _LL1823;} else{ goto _LL1824;} _LL1824: _LL2023: _temp2022= _temp1796.f1;
if(( unsigned int) _temp2022 >  3u?*(( int*) _temp2022) ==  Cyc_Absyn_ArrayType:
0){ _LL2029: _temp2028=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2022)->f1;
goto _LL2027; _LL2027: _temp2026=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2022)->f2; goto _LL2025; _LL2025: _temp2024=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2022)->f3; goto _LL2015;} else{ goto _LL1826;} _LL2015: _temp2014=
_temp1796.f2; if(( unsigned int) _temp2014 >  3u?*(( int*) _temp2014) ==  Cyc_Absyn_ArrayType:
0){ _LL2021: _temp2020=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2014)->f1;
goto _LL2019; _LL2019: _temp2018=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2014)->f2; goto _LL2017; _LL2017: _temp2016=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2014)->f3; goto _LL1825;} else{ goto _LL1826;} _LL1826: _LL2051: _temp2050=
_temp1796.f1; if(( unsigned int) _temp2050 >  3u?*(( int*) _temp2050) ==  Cyc_Absyn_FnType:
0){ _LL2053: _temp2052=(( struct Cyc_Absyn_FnType_struct*) _temp2050)->f1;
_LL2069: _temp2068= _temp2052.tvars; goto _LL2067; _LL2067: _temp2066= _temp2052.effect;
goto _LL2065; _LL2065: _temp2064=( void*) _temp2052.ret_typ; goto _LL2063;
_LL2063: _temp2062= _temp2052.args; goto _LL2061; _LL2061: _temp2060= _temp2052.c_varargs;
goto _LL2059; _LL2059: _temp2058= _temp2052.cyc_varargs; goto _LL2057; _LL2057:
_temp2056= _temp2052.rgn_po; goto _LL2055; _LL2055: _temp2054= _temp2052.attributes;
goto _LL2031;} else{ goto _LL1828;} _LL2031: _temp2030= _temp1796.f2; if((
unsigned int) _temp2030 >  3u?*(( int*) _temp2030) ==  Cyc_Absyn_FnType: 0){
_LL2033: _temp2032=(( struct Cyc_Absyn_FnType_struct*) _temp2030)->f1; _LL2049:
_temp2048= _temp2032.tvars; goto _LL2047; _LL2047: _temp2046= _temp2032.effect;
goto _LL2045; _LL2045: _temp2044=( void*) _temp2032.ret_typ; goto _LL2043;
_LL2043: _temp2042= _temp2032.args; goto _LL2041; _LL2041: _temp2040= _temp2032.c_varargs;
goto _LL2039; _LL2039: _temp2038= _temp2032.cyc_varargs; goto _LL2037; _LL2037:
_temp2036= _temp2032.rgn_po; goto _LL2035; _LL2035: _temp2034= _temp2032.attributes;
goto _LL1827;} else{ goto _LL1828;} _LL1828: _LL2075: _temp2074= _temp1796.f1;
if(( unsigned int) _temp2074 >  3u?*(( int*) _temp2074) ==  Cyc_Absyn_TupleType:
0){ _LL2077: _temp2076=(( struct Cyc_Absyn_TupleType_struct*) _temp2074)->f1;
goto _LL2071;} else{ goto _LL1830;} _LL2071: _temp2070= _temp1796.f2; if((
unsigned int) _temp2070 >  3u?*(( int*) _temp2070) ==  Cyc_Absyn_TupleType: 0){
_LL2073: _temp2072=(( struct Cyc_Absyn_TupleType_struct*) _temp2070)->f1; goto
_LL1829;} else{ goto _LL1830;} _LL1830: _LL2083: _temp2082= _temp1796.f1; if((
unsigned int) _temp2082 >  3u?*(( int*) _temp2082) ==  Cyc_Absyn_AnonStructType:
0){ _LL2085: _temp2084=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2082)->f1;
goto _LL2079;} else{ goto _LL1832;} _LL2079: _temp2078= _temp1796.f2; if((
unsigned int) _temp2078 >  3u?*(( int*) _temp2078) ==  Cyc_Absyn_AnonStructType:
0){ _LL2081: _temp2080=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2078)->f1;
goto _LL1831;} else{ goto _LL1832;} _LL1832: _LL2091: _temp2090= _temp1796.f1;
if(( unsigned int) _temp2090 >  3u?*(( int*) _temp2090) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2093: _temp2092=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2090)->f1;
goto _LL2087;} else{ goto _LL1834;} _LL2087: _temp2086= _temp1796.f2; if((
unsigned int) _temp2086 >  3u?*(( int*) _temp2086) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2089: _temp2088=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2086)->f1;
goto _LL1833;} else{ goto _LL1834;} _LL1834: _LL2097: _temp2096= _temp1796.f1;
if( _temp2096 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL2095;} else{ goto _LL1836;}
_LL2095: _temp2094= _temp1796.f2; if( _temp2094 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1835;} else{ goto _LL1836;} _LL1836: _LL2103: _temp2102= _temp1796.f1;
if(( unsigned int) _temp2102 >  3u?*(( int*) _temp2102) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2105: _temp2104=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2102)->f1; goto _LL2099;} else{ goto _LL1838;} _LL2099: _temp2098=
_temp1796.f2; if(( unsigned int) _temp2098 >  3u?*(( int*) _temp2098) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2101: _temp2100=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2098)->f1; goto _LL1837;} else{ goto _LL1838;} _LL1838: _LL2107: _temp2106=
_temp1796.f1; if(( unsigned int) _temp2106 >  3u?*(( int*) _temp2106) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1839;} else{ goto _LL1840;} _LL1840: _LL2109: _temp2108= _temp1796.f2;
if(( unsigned int) _temp2108 >  3u?*(( int*) _temp2108) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1841;} else{ goto _LL1842;} _LL1842: _LL2111: _temp2110= _temp1796.f1;
if(( unsigned int) _temp2110 >  3u?*(( int*) _temp2110) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1843;} else{ goto _LL1844;} _LL1844: _LL2113: _temp2112= _temp1796.f1;
if(( unsigned int) _temp2112 >  3u?*(( int*) _temp2112) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1845;} else{ goto _LL1846;} _LL1846: _LL2115: _temp2114= _temp1796.f2;
if(( unsigned int) _temp2114 >  3u?*(( int*) _temp2114) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1847;} else{ goto _LL1848;} _LL1848: _LL2117: _temp2116= _temp1796.f2;
if(( unsigned int) _temp2116 >  3u?*(( int*) _temp2116) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1849;} else{ goto _LL1850;} _LL1850: goto _LL1851; _LL1799:({ void*
_temp2118[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types", sizeof( unsigned char), 39u),
_tag_arr( _temp2118, sizeof( void*), 0u));}); return 0; _LL1801: return 0;
_LL1803: { struct _tagged_arr* _temp2119= _temp1862->name; struct _tagged_arr*
_temp2120= _temp1866->name; int _temp2121=*(( int*) _check_null( _temp1862->identity));
int _temp2122=*(( int*) _check_null( _temp1866->identity)); return Cyc_Core_intcmp(
_temp2121, _temp2122);} _LL1805: { int _temp2123=(( int(*)( int(* cmp)( struct
_tuple1*, struct _tuple1*), struct _tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)(
Cyc_Absyn_qvar_cmp, _temp1872, _temp1878); if( _temp2123 !=  0){ return
_temp2123;} else{ return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp1870,
_temp1876);}} _LL1807: { int _temp2124= Cyc_Absyn_qvar_cmp( _temp1886, _temp1882);
return _temp2124;} _LL1809: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l1, struct Cyc_List_List* l2))
Cyc_Tcutil_list_cmp)( Cyc_Tcutil_enumfield_cmp, _temp1894, _temp1890); _LL1811: {
int _temp2125=(( int(*)( int(* cmp)( struct _tuple1*, struct _tuple1*), struct
_tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)( Cyc_Absyn_qvar_cmp,
_temp1902, _temp1910); if( _temp2125 !=  0){ return _temp2125;} else{ return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1900, _temp1908);}} _LL1813: if( _temp1924 == 
_temp1937){ return 0;}{ int _temp2126= Cyc_Absyn_qvar_cmp( _temp1924->name,
_temp1937->name); if( _temp2126 !=  0){ return _temp2126;}{ int _temp2127= Cyc_Tcutil_typecmp(
_temp1916, _temp1929); if( _temp2127 !=  0){ return _temp2127;} return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1918, _temp1931);}} _LL1815: if( _temp1948 == 
_temp1960){ return 0;}{ int _temp2128= Cyc_Absyn_qvar_cmp( _temp1960->name,
_temp1948->name); if( _temp2128 !=  0){ return _temp2128;}{ int _temp2129= Cyc_Absyn_qvar_cmp(
_temp1958->name, _temp1946->name); if( _temp2129 !=  0){ return _temp2129;}
return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp1942, _temp1954);}} _LL1817: {
int _temp2130= Cyc_Tcutil_typecmp( _temp1974, _temp1988); if( _temp2130 !=  0){
return _temp2130;}{ int _temp2131= Cyc_Tcutil_typecmp( _temp1972, _temp1986);
if( _temp2131 !=  0){ return _temp2131;}{ int _temp2132= Cyc_Tcutil_tqual_cmp(
_temp1968, _temp1982); if( _temp2132 !=  0){ return _temp2132;}{ int _temp2133=
Cyc_Tcutil_conrefs_cmp( Cyc_Tcutil_boundscmp, _temp1966, _temp1980); if(
_temp2133 !=  0){ return _temp2133;}{ void* _temp2134=( void*)( Cyc_Absyn_compress_conref(
_temp1966))->v; void* _temp2140; _LL2136: if(( unsigned int) _temp2134 >  1u?*((
int*) _temp2134) ==  Cyc_Absyn_Eq_constr: 0){ _LL2141: _temp2140=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2134)->f1; if( _temp2140 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2137;} else{ goto _LL2138;}} else{ goto _LL2138;}
_LL2138: goto _LL2139; _LL2137: return 0; _LL2139: goto _LL2135; _LL2135:;}
return(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_conrefs_cmp)( Cyc_Core_intcmp, _temp1970, _temp1984);}}}} _LL1819:
if( _temp1994 !=  _temp2000){ return Cyc_Core_intcmp(( int)(( unsigned int)
_temp1994),( int)(( unsigned int) _temp2000));} if( _temp1992 !=  _temp1998){
return Cyc_Core_intcmp(( int)(( unsigned int) _temp1992),( int)(( unsigned int)
_temp1998));} return 0; _LL1821: return 0; _LL1823: if( _temp2012 ==  _temp2008){
return 0;} else{ if( _temp2012){ return - 1;} else{ return 1;}} _LL1825: { int
_temp2142= Cyc_Tcutil_tqual_cmp( _temp2018, _temp2026); if( _temp2142 !=  0){
return _temp2142;}{ int _temp2143= Cyc_Tcutil_typecmp( _temp2020, _temp2028);
if( _temp2143 !=  0){ return _temp2143;} if( _temp2024 ==  _temp2016){ return 0;}
if( _temp2024 ==  0? 1: _temp2016 ==  0){({ void* _temp2144[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("missing expression in array index",
sizeof( unsigned char), 34u), _tag_arr( _temp2144, sizeof( void*), 0u));});}
return(( int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp* a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
_temp2024, _temp2016);}} _LL1827:({ void* _temp2145[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: function types not handled",
sizeof( unsigned char), 36u), _tag_arr( _temp2145, sizeof( void*), 0u));}); goto
_LL1797; _LL1829: return(( int(*)( int(* cmp)( struct _tuple4*, struct _tuple4*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_tqual_type_cmp,
_temp2072, _temp2076); _LL1831: _temp2092= _temp2084; _temp2088= _temp2080; goto
_LL1833; _LL1833: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Structfield*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_structfield_cmp, _temp2088, _temp2092);
_LL1835: return 0; _LL1837: return Cyc_Tcutil_typecmp( _temp2104, _temp2100);
_LL1839: goto _LL1841; _LL1841: goto _LL1843; _LL1843: goto _LL1845; _LL1845:
goto _LL1847; _LL1847: goto _LL1849; _LL1849:({ void* _temp2146[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: effects not handled",
sizeof( unsigned char), 29u), _tag_arr( _temp2146, sizeof( void*), 0u));}); goto
_LL1797; _LL1851: goto _LL1797; _LL1797:;} return({ void* _temp2147[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("Unmatched case in typecmp", sizeof( unsigned char), 26u), _tag_arr(
_temp2147, sizeof( void*), 0u));});}} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp2148= Cyc_Tcutil_compress( t); _LL2150: if(( unsigned int) _temp2148
>  3u?*(( int*) _temp2148) ==  Cyc_Absyn_IntType: 0){ goto _LL2151;} else{ goto
_LL2152;} _LL2152: if( _temp2148 == ( void*) Cyc_Absyn_FloatType){ goto _LL2153;}
else{ goto _LL2154;} _LL2154: if(( unsigned int) _temp2148 >  3u?*(( int*)
_temp2148) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2155;} else{ goto _LL2156;}
_LL2156: if(( unsigned int) _temp2148 >  3u?*(( int*) _temp2148) ==  Cyc_Absyn_EnumType:
0){ goto _LL2157;} else{ goto _LL2158;} _LL2158: if(( unsigned int) _temp2148 > 
3u?*(( int*) _temp2148) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL2159;} else{
goto _LL2160;} _LL2160: goto _LL2161; _LL2151: goto _LL2153; _LL2153: goto
_LL2155; _LL2155: goto _LL2157; _LL2157: return 1; _LL2159: return 1; _LL2161:
return 0; _LL2149:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp2163=({ struct _tuple8 _temp2162; _temp2162.f1= t1; _temp2162.f2= t2;
_temp2162;}); void* _temp2187; int _temp2189; void* _temp2191; int _temp2193;
void* _temp2195; void* _temp2197; void* _temp2199; void* _temp2201; void*
_temp2203; void* _temp2205; void* _temp2207; void* _temp2209; void* _temp2211;
void* _temp2213; void* _temp2215; void* _temp2217; void* _temp2219; void*
_temp2221; void* _temp2223; void* _temp2225; void* _temp2227; void* _temp2229;
void* _temp2231; void* _temp2233; void* _temp2235; void* _temp2237; void*
_temp2239; void* _temp2241; void* _temp2243; void* _temp2245; void* _temp2247;
_LL2165: _LL2192: _temp2191= _temp2163.f1; if(( unsigned int) _temp2191 >  3u?*((
int*) _temp2191) ==  Cyc_Absyn_DoubleType: 0){ _LL2194: _temp2193=(( struct Cyc_Absyn_DoubleType_struct*)
_temp2191)->f1; goto _LL2188;} else{ goto _LL2167;} _LL2188: _temp2187=
_temp2163.f2; if(( unsigned int) _temp2187 >  3u?*(( int*) _temp2187) ==  Cyc_Absyn_DoubleType:
0){ _LL2190: _temp2189=(( struct Cyc_Absyn_DoubleType_struct*) _temp2187)->f1;
goto _LL2166;} else{ goto _LL2167;} _LL2167: _LL2198: _temp2197= _temp2163.f1;
if(( unsigned int) _temp2197 >  3u?*(( int*) _temp2197) ==  Cyc_Absyn_DoubleType:
0){ goto _LL2196;} else{ goto _LL2169;} _LL2196: _temp2195= _temp2163.f2; if(
_temp2195 == ( void*) Cyc_Absyn_FloatType){ goto _LL2168;} else{ goto _LL2169;}
_LL2169: _LL2202: _temp2201= _temp2163.f1; if(( unsigned int) _temp2201 >  3u?*((
int*) _temp2201) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2200;} else{ goto _LL2171;}
_LL2200: _temp2199= _temp2163.f2; if(( unsigned int) _temp2199 >  3u?*(( int*)
_temp2199) ==  Cyc_Absyn_IntType: 0){ goto _LL2170;} else{ goto _LL2171;}
_LL2171: _LL2206: _temp2205= _temp2163.f1; if( _temp2205 == ( void*) Cyc_Absyn_FloatType){
goto _LL2204;} else{ goto _LL2173;} _LL2204: _temp2203= _temp2163.f2; if((
unsigned int) _temp2203 >  3u?*(( int*) _temp2203) ==  Cyc_Absyn_IntType: 0){
goto _LL2172;} else{ goto _LL2173;} _LL2173: _LL2212: _temp2211= _temp2163.f1;
if(( unsigned int) _temp2211 >  3u?*(( int*) _temp2211) ==  Cyc_Absyn_IntType: 0){
_LL2214: _temp2213=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2211)->f2;
if( _temp2213 == ( void*) Cyc_Absyn_B8){ goto _LL2208;} else{ goto _LL2175;}}
else{ goto _LL2175;} _LL2208: _temp2207= _temp2163.f2; if(( unsigned int)
_temp2207 >  3u?*(( int*) _temp2207) ==  Cyc_Absyn_IntType: 0){ _LL2210:
_temp2209=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2207)->f2; if(
_temp2209 == ( void*) Cyc_Absyn_B8){ goto _LL2174;} else{ goto _LL2175;}} else{
goto _LL2175;} _LL2175: _LL2216: _temp2215= _temp2163.f1; if(( unsigned int)
_temp2215 >  3u?*(( int*) _temp2215) ==  Cyc_Absyn_IntType: 0){ _LL2218:
_temp2217=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2215)->f2; if(
_temp2217 == ( void*) Cyc_Absyn_B8){ goto _LL2176;} else{ goto _LL2177;}} else{
goto _LL2177;} _LL2177: _LL2222: _temp2221= _temp2163.f1; if(( unsigned int)
_temp2221 >  3u?*(( int*) _temp2221) ==  Cyc_Absyn_IntType: 0){ _LL2224:
_temp2223=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2221)->f2; if(
_temp2223 == ( void*) Cyc_Absyn_B4){ goto _LL2220;} else{ goto _LL2179;}} else{
goto _LL2179;} _LL2220: _temp2219= _temp2163.f2; if( _temp2219 == ( void*) Cyc_Absyn_FloatType){
goto _LL2178;} else{ goto _LL2179;} _LL2179: _LL2230: _temp2229= _temp2163.f1;
if(( unsigned int) _temp2229 >  3u?*(( int*) _temp2229) ==  Cyc_Absyn_IntType: 0){
_LL2232: _temp2231=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2229)->f2;
if( _temp2231 == ( void*) Cyc_Absyn_B4){ goto _LL2226;} else{ goto _LL2181;}}
else{ goto _LL2181;} _LL2226: _temp2225= _temp2163.f2; if(( unsigned int)
_temp2225 >  3u?*(( int*) _temp2225) ==  Cyc_Absyn_IntType: 0){ _LL2228:
_temp2227=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2225)->f2; if(
_temp2227 == ( void*) Cyc_Absyn_B2){ goto _LL2180;} else{ goto _LL2181;}} else{
goto _LL2181;} _LL2181: _LL2238: _temp2237= _temp2163.f1; if(( unsigned int)
_temp2237 >  3u?*(( int*) _temp2237) ==  Cyc_Absyn_IntType: 0){ _LL2240:
_temp2239=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2237)->f2; if(
_temp2239 == ( void*) Cyc_Absyn_B4){ goto _LL2234;} else{ goto _LL2183;}} else{
goto _LL2183;} _LL2234: _temp2233= _temp2163.f2; if(( unsigned int) _temp2233 > 
3u?*(( int*) _temp2233) ==  Cyc_Absyn_IntType: 0){ _LL2236: _temp2235=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2233)->f2; if( _temp2235 == ( void*) Cyc_Absyn_B1){
goto _LL2182;} else{ goto _LL2183;}} else{ goto _LL2183;} _LL2183: _LL2246:
_temp2245= _temp2163.f1; if(( unsigned int) _temp2245 >  3u?*(( int*) _temp2245)
==  Cyc_Absyn_IntType: 0){ _LL2248: _temp2247=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2245)->f2; if( _temp2247 == ( void*) Cyc_Absyn_B2){ goto _LL2242;} else{
goto _LL2185;}} else{ goto _LL2185;} _LL2242: _temp2241= _temp2163.f2; if((
unsigned int) _temp2241 >  3u?*(( int*) _temp2241) ==  Cyc_Absyn_IntType: 0){
_LL2244: _temp2243=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2241)->f2;
if( _temp2243 == ( void*) Cyc_Absyn_B1){ goto _LL2184;} else{ goto _LL2185;}}
else{ goto _LL2185;} _LL2185: goto _LL2186; _LL2166: return ! _temp2189?
_temp2193: 0; _LL2168: goto _LL2170; _LL2170: goto _LL2172; _LL2172: return 1;
_LL2174: return 0; _LL2176: goto _LL2178; _LL2178: goto _LL2180; _LL2180: goto
_LL2182; _LL2182: goto _LL2184; _LL2184: return 1; _LL2186: return 0; _LL2164:;}}
int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_List_List*
es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List* el= es; for(
0; el !=  0; el= el->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) el->hd)->topt))->v); if( Cyc_Tcutil_is_arithmetic_type(
t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)
max_arith_type->v)){ max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2249=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2249->v=( void*) t1; _temp2249;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify(
t,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*) el->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp2252; _temp2252.tag= Cyc_Std_String_pa; _temp2252.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*) el->hd)->topt))->v);{ struct Cyc_Std_String_pa_struct _temp2251;
_temp2251.tag= Cyc_Std_String_pa; _temp2251.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2250[ 2u]={& _temp2251,& _temp2252}; Cyc_Tcutil_terr((( struct
Cyc_Absyn_Exp*) el->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp2250, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp2253= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL2255: if(( unsigned int) _temp2253 >  3u?*(( int*) _temp2253) ==  Cyc_Absyn_PointerType:
0){ goto _LL2256;} else{ goto _LL2257;} _LL2257: goto _LL2258; _LL2256: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL2254; _LL2258: return 0; _LL2254:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp2259= Cyc_Tcutil_compress(
t); _LL2261: if(( unsigned int) _temp2259 >  3u?*(( int*) _temp2259) ==  Cyc_Absyn_IntType:
0){ goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int) _temp2259 > 
3u?*(( int*) _temp2259) ==  Cyc_Absyn_EnumType: 0){ goto _LL2264;} else{ goto
_LL2265;} _LL2265: if(( unsigned int) _temp2259 >  3u?*(( int*) _temp2259) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2266;} else{ goto _LL2267;} _LL2267: goto
_LL2268; _LL2262: goto _LL2264; _LL2264: return 1; _LL2266: return 1; _LL2268:
return 0; _LL2260:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp2269[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2269, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp2270[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp2270, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp2272=({ struct _tuple8 _temp2271; _temp2271.f1= t1; _temp2271.f2= t2;
_temp2271;}); void* _temp2284; struct Cyc_Absyn_PtrInfo _temp2286; void*
_temp2288; struct Cyc_Absyn_PtrInfo _temp2290; void* _temp2292; struct Cyc_Absyn_Exp*
_temp2294; struct Cyc_Absyn_Tqual _temp2296; void* _temp2298; void* _temp2300;
struct Cyc_Absyn_Exp* _temp2302; struct Cyc_Absyn_Tqual _temp2304; void*
_temp2306; void* _temp2308; struct Cyc_Absyn_TunionInfo _temp2310; struct Cyc_List_List*
_temp2312; void* _temp2314; struct Cyc_Absyn_Tuniondecl** _temp2316; struct Cyc_Absyn_Tuniondecl*
_temp2318; void* _temp2319; struct Cyc_Absyn_TunionFieldInfo _temp2321; struct
Cyc_List_List* _temp2323; void* _temp2325; struct Cyc_Absyn_Tunionfield*
_temp2327; struct Cyc_Absyn_Tuniondecl* _temp2329; void* _temp2331; struct Cyc_Absyn_TunionInfo
_temp2333; void* _temp2335; struct Cyc_List_List* _temp2337; void* _temp2339;
struct Cyc_Absyn_Tuniondecl** _temp2341; struct Cyc_Absyn_Tuniondecl* _temp2343;
void* _temp2344; struct Cyc_Absyn_PtrInfo _temp2346; struct Cyc_Absyn_Conref*
_temp2348; struct Cyc_Absyn_Tqual _temp2350; struct Cyc_Absyn_Conref* _temp2352;
void* _temp2354; void* _temp2356; _LL2274: _LL2289: _temp2288= _temp2272.f1; if((
unsigned int) _temp2288 >  3u?*(( int*) _temp2288) ==  Cyc_Absyn_PointerType: 0){
_LL2291: _temp2290=(( struct Cyc_Absyn_PointerType_struct*) _temp2288)->f1; goto
_LL2285;} else{ goto _LL2276;} _LL2285: _temp2284= _temp2272.f2; if((
unsigned int) _temp2284 >  3u?*(( int*) _temp2284) ==  Cyc_Absyn_PointerType: 0){
_LL2287: _temp2286=(( struct Cyc_Absyn_PointerType_struct*) _temp2284)->f1; goto
_LL2275;} else{ goto _LL2276;} _LL2276: _LL2301: _temp2300= _temp2272.f1; if((
unsigned int) _temp2300 >  3u?*(( int*) _temp2300) ==  Cyc_Absyn_ArrayType: 0){
_LL2307: _temp2306=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2300)->f1;
goto _LL2305; _LL2305: _temp2304=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2300)->f2; goto _LL2303; _LL2303: _temp2302=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2300)->f3; goto _LL2293;} else{ goto _LL2278;} _LL2293: _temp2292=
_temp2272.f2; if(( unsigned int) _temp2292 >  3u?*(( int*) _temp2292) ==  Cyc_Absyn_ArrayType:
0){ _LL2299: _temp2298=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2292)->f1;
goto _LL2297; _LL2297: _temp2296=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2292)->f2; goto _LL2295; _LL2295: _temp2294=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2292)->f3; goto _LL2277;} else{ goto _LL2278;} _LL2278: _LL2320: _temp2319=
_temp2272.f1; if(( unsigned int) _temp2319 >  3u?*(( int*) _temp2319) ==  Cyc_Absyn_TunionFieldType:
0){ _LL2322: _temp2321=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2319)->f1;
_LL2326: _temp2325=( void*) _temp2321.field_info; if(*(( int*) _temp2325) == 
Cyc_Absyn_KnownTunionfield){ _LL2330: _temp2329=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2325)->f1; goto _LL2328; _LL2328: _temp2327=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2325)->f2; goto _LL2324;} else{ goto _LL2280;} _LL2324: _temp2323=
_temp2321.targs; goto _LL2309;} else{ goto _LL2280;} _LL2309: _temp2308=
_temp2272.f2; if(( unsigned int) _temp2308 >  3u?*(( int*) _temp2308) ==  Cyc_Absyn_TunionType:
0){ _LL2311: _temp2310=(( struct Cyc_Absyn_TunionType_struct*) _temp2308)->f1;
_LL2315: _temp2314=( void*) _temp2310.tunion_info; if(*(( int*) _temp2314) == 
Cyc_Absyn_KnownTunion){ _LL2317: _temp2316=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2314)->f1; _temp2318=* _temp2316; goto _LL2313;} else{ goto _LL2280;}
_LL2313: _temp2312= _temp2310.targs; goto _LL2279;} else{ goto _LL2280;} _LL2280:
_LL2345: _temp2344= _temp2272.f1; if(( unsigned int) _temp2344 >  3u?*(( int*)
_temp2344) ==  Cyc_Absyn_PointerType: 0){ _LL2347: _temp2346=(( struct Cyc_Absyn_PointerType_struct*)
_temp2344)->f1; _LL2357: _temp2356=( void*) _temp2346.elt_typ; goto _LL2355;
_LL2355: _temp2354=( void*) _temp2346.rgn_typ; goto _LL2353; _LL2353: _temp2352=
_temp2346.nullable; goto _LL2351; _LL2351: _temp2350= _temp2346.tq; goto _LL2349;
_LL2349: _temp2348= _temp2346.bounds; goto _LL2332;} else{ goto _LL2282;}
_LL2332: _temp2331= _temp2272.f2; if(( unsigned int) _temp2331 >  3u?*(( int*)
_temp2331) ==  Cyc_Absyn_TunionType: 0){ _LL2334: _temp2333=(( struct Cyc_Absyn_TunionType_struct*)
_temp2331)->f1; _LL2340: _temp2339=( void*) _temp2333.tunion_info; if(*(( int*)
_temp2339) ==  Cyc_Absyn_KnownTunion){ _LL2342: _temp2341=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2339)->f1; _temp2343=* _temp2341; goto _LL2338;} else{ goto _LL2282;}
_LL2338: _temp2337= _temp2333.targs; goto _LL2336; _LL2336: _temp2335=( void*)
_temp2333.rgn; goto _LL2281;} else{ goto _LL2282;} _LL2282: goto _LL2283;
_LL2275: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp2290.nullable, _temp2286.nullable)){ void* _temp2358=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2290.nullable))->v;
int _temp2364; _LL2360: if(( unsigned int) _temp2358 >  1u?*(( int*) _temp2358)
==  Cyc_Absyn_Eq_constr: 0){ _LL2365: _temp2364=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2358)->f1; goto _LL2361;} else{ goto _LL2362;} _LL2362: goto _LL2363;
_LL2361: okay= ! _temp2364; goto _LL2359; _LL2363:( int) _throw(({ void*
_temp2366[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp2366, sizeof( void*), 0u));}));
_LL2359:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2290.bounds,
_temp2286.bounds)){ struct _tuple8 _temp2368=({ struct _tuple8 _temp2367;
_temp2367.f1=( void*)( Cyc_Absyn_compress_conref( _temp2290.bounds))->v;
_temp2367.f2=( void*)( Cyc_Absyn_compress_conref( _temp2286.bounds))->v;
_temp2367;}); void* _temp2378; void* _temp2380; void* _temp2382; void* _temp2384;
void* _temp2386; void* _temp2388; struct Cyc_Absyn_Exp* _temp2390; void*
_temp2392; void* _temp2394; struct Cyc_Absyn_Exp* _temp2396; void* _temp2398;
void* _temp2400; void* _temp2402; void* _temp2404; _LL2370: _LL2383: _temp2382=
_temp2368.f1; if(( unsigned int) _temp2382 >  1u?*(( int*) _temp2382) ==  Cyc_Absyn_Eq_constr:
0){ _LL2385: _temp2384=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2382)->f1;
if(( unsigned int) _temp2384 >  1u?*(( int*) _temp2384) ==  Cyc_Absyn_Upper_b: 0){
goto _LL2379;} else{ goto _LL2372;}} else{ goto _LL2372;} _LL2379: _temp2378=
_temp2368.f2; if(( unsigned int) _temp2378 >  1u?*(( int*) _temp2378) ==  Cyc_Absyn_Eq_constr:
0){ _LL2381: _temp2380=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2378)->f1;
if( _temp2380 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2371;} else{ goto
_LL2372;}} else{ goto _LL2372;} _LL2372: _LL2393: _temp2392= _temp2368.f1; if((
unsigned int) _temp2392 >  1u?*(( int*) _temp2392) ==  Cyc_Absyn_Eq_constr: 0){
_LL2395: _temp2394=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2392)->f1;
if(( unsigned int) _temp2394 >  1u?*(( int*) _temp2394) ==  Cyc_Absyn_Upper_b: 0){
_LL2397: _temp2396=(( struct Cyc_Absyn_Upper_b_struct*) _temp2394)->f1; goto
_LL2387;} else{ goto _LL2374;}} else{ goto _LL2374;} _LL2387: _temp2386=
_temp2368.f2; if(( unsigned int) _temp2386 >  1u?*(( int*) _temp2386) ==  Cyc_Absyn_Eq_constr:
0){ _LL2389: _temp2388=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2386)->f1;
if(( unsigned int) _temp2388 >  1u?*(( int*) _temp2388) ==  Cyc_Absyn_Upper_b: 0){
_LL2391: _temp2390=(( struct Cyc_Absyn_Upper_b_struct*) _temp2388)->f1; goto
_LL2373;} else{ goto _LL2374;}} else{ goto _LL2374;} _LL2374: _LL2403: _temp2402=
_temp2368.f1; if(( unsigned int) _temp2402 >  1u?*(( int*) _temp2402) ==  Cyc_Absyn_Eq_constr:
0){ _LL2405: _temp2404=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2402)->f1;
if( _temp2404 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2399;} else{ goto
_LL2376;}} else{ goto _LL2376;} _LL2399: _temp2398= _temp2368.f2; if((
unsigned int) _temp2398 >  1u?*(( int*) _temp2398) ==  Cyc_Absyn_Eq_constr: 0){
_LL2401: _temp2400=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2398)->f1;
if( _temp2400 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2375;} else{ goto
_LL2376;}} else{ goto _LL2376;} _LL2376: goto _LL2377; _LL2371: okay= 1; goto
_LL2369; _LL2373: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp2396) >=  Cyc_Evexp_eval_const_uint_exp(
_temp2390): 0;({ void* _temp2406[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp2406, sizeof( void*), 0u));}); goto
_LL2369; _LL2375: okay= 1; goto _LL2369; _LL2377: okay= 0; goto _LL2369; _LL2369:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp2290.elt_typ,( void*) _temp2286.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp2290.rgn_typ,( void*) _temp2286.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp2290.rgn_typ,( void*) _temp2286.rgn_typ):
0; okay= okay? !( _temp2290.tq).q_const? 1:( _temp2286.tq).q_const: 0; return
okay;} _LL2277: { int okay; okay=( _temp2302 !=  0? _temp2294 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2302)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2294)): 0; return( okay? Cyc_Tcutil_unify(
_temp2306, _temp2298): 0)? ! _temp2304.q_const? 1: _temp2296.q_const: 0;}
_LL2279: if(( _temp2329 ==  _temp2318? 1: Cyc_Absyn_qvar_cmp( _temp2329->name,
_temp2318->name) ==  0)? _temp2327->typs ==  0: 0){ for( 0; _temp2323 !=  0?
_temp2312 !=  0: 0;( _temp2323= _temp2323->tl, _temp2312= _temp2312->tl)){ if( !
Cyc_Tcutil_unify(( void*) _temp2323->hd,( void*) _temp2312->hd)){ break;}} if(
_temp2323 ==  0? _temp2312 ==  0: 0){ return 1;}} return 0; _LL2281:{ void*
_temp2407= Cyc_Tcutil_compress( _temp2356); struct Cyc_Absyn_TunionFieldInfo
_temp2413; struct Cyc_List_List* _temp2415; void* _temp2417; struct Cyc_Absyn_Tunionfield*
_temp2419; struct Cyc_Absyn_Tuniondecl* _temp2421; _LL2409: if(( unsigned int)
_temp2407 >  3u?*(( int*) _temp2407) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2414:
_temp2413=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2407)->f1; _LL2418:
_temp2417=( void*) _temp2413.field_info; if(*(( int*) _temp2417) ==  Cyc_Absyn_KnownTunionfield){
_LL2422: _temp2421=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2417)->f1;
goto _LL2420; _LL2420: _temp2419=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2417)->f2; goto _LL2416;} else{ goto _LL2411;} _LL2416: _temp2415=
_temp2413.targs; goto _LL2410;} else{ goto _LL2411;} _LL2411: goto _LL2412;
_LL2410: if( ! Cyc_Tcutil_unify( _temp2354, _temp2335)? ! Cyc_Tcenv_region_outlives(
te, _temp2354, _temp2335): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp2352,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2348,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp2343->name, _temp2421->name) ==  0? _temp2419->typs !=  0: 0){ int okay= 1;
for( 0; _temp2415 !=  0? _temp2337 !=  0: 0;( _temp2415= _temp2415->tl,
_temp2337= _temp2337->tl)){ if( ! Cyc_Tcutil_unify(( void*) _temp2415->hd,( void*)
_temp2337->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp2415 !=  0)? 1:
_temp2337 !=  0){ return 0;} return 1;} goto _LL2408; _LL2412: goto _LL2408;
_LL2408:;} return 0; _LL2283: return Cyc_Tcutil_unify( t1, t2); _LL2273:;}} int
Cyc_Tcutil_is_pointer_type( void* t){ void* _temp2423= Cyc_Tcutil_compress( t);
_LL2425: if(( unsigned int) _temp2423 >  3u?*(( int*) _temp2423) ==  Cyc_Absyn_PointerType:
0){ goto _LL2426;} else{ goto _LL2427;} _LL2427: goto _LL2428; _LL2426: return 1;
_LL2428: return 0; _LL2424:;} int Cyc_Tcutil_is_zero( struct Cyc_Absyn_Exp* e){
void* _temp2429=( void*) e->r; void* _temp2435; int _temp2437; _LL2431: if(*((
int*) _temp2429) ==  Cyc_Absyn_Const_e){ _LL2436: _temp2435=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp2429)->f1; if(( unsigned int) _temp2435 >  1u?*((
int*) _temp2435) ==  Cyc_Absyn_Int_c: 0){ _LL2438: _temp2437=(( struct Cyc_Absyn_Int_c_struct*)
_temp2435)->f2; if( _temp2437 ==  0){ goto _LL2432;} else{ goto _LL2433;}} else{
goto _LL2433;}} else{ goto _LL2433;} _LL2433: goto _LL2434; _LL2432: return 1;
_LL2434: return 0; _LL2430:;} struct Cyc_Core_Opt Cyc_Tcutil_rk={( void*)(( void*)
3u)}; struct Cyc_Core_Opt Cyc_Tcutil_ak={( void*)(( void*) 0u)}; struct Cyc_Core_Opt
Cyc_Tcutil_bk={( void*)(( void*) 2u)}; struct Cyc_Core_Opt Cyc_Tcutil_mk={( void*)((
void*) 1u)}; int Cyc_Tcutil_zero_to_null( struct Cyc_Tcenv_Tenv* te, void* t2,
struct Cyc_Absyn_Exp* e1){ if( Cyc_Tcutil_is_pointer_type( t2)? Cyc_Tcutil_is_zero(
e1): 0){( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2439=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2439[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2440; _temp2440.tag= Cyc_Absyn_Const_e;
_temp2440.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2440;}); _temp2439;})));{
struct Cyc_List_List* _temp2441= Cyc_Tcenv_lookup_type_vars( te); struct Cyc_Absyn_PointerType_struct*
_temp2442=({ struct Cyc_Absyn_PointerType_struct* _temp2443=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2443[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2444; _temp2444.tag= Cyc_Absyn_PointerType;
_temp2444.f1=({ struct Cyc_Absyn_PtrInfo _temp2445; _temp2445.elt_typ=( void*)
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt*
_temp2447=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2447->v=( void*) _temp2441; _temp2447;})); _temp2445.rgn_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp2446=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2446->v=( void*)
_temp2441; _temp2446;})); _temp2445.nullable=(( struct Cyc_Absyn_Conref*(*)( int
x)) Cyc_Absyn_new_conref)( 1); _temp2445.tq= Cyc_Absyn_empty_tqual(); _temp2445.bounds=
Cyc_Absyn_empty_conref(); _temp2445;}); _temp2444;}); _temp2443;});( void*)(((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v=( void*)(( void*) _temp2442));
return Cyc_Tcutil_coerce_arg( te, e1, t2);}} return 0;} int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type( t1)? Cyc_Tcutil_is_arithmetic_type(
t2): 0){ if( Cyc_Tcutil_will_lose_precision( t1, t2)){({ void* _temp2448[ 0u]={};
Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2448, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_zero_to_null(
te, t2, e)){ return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2);({ struct Cyc_Std_String_pa_struct _temp2451; _temp2451.tag= Cyc_Std_String_pa;
_temp2451.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp2450; _temp2450.tag= Cyc_Std_String_pa; _temp2450.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp2449[ 2u]={& _temp2450,& _temp2451};
Cyc_Tcutil_warn( e->loc, _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u), _tag_arr( _temp2449, sizeof( void*), 2u));}}}); return 1;}
else{ return 0;}}}}} int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg( te, e, t);} int
Cyc_Tcutil_coerceable( void* t){ void* _temp2452= Cyc_Tcutil_compress( t);
_LL2454: if(( unsigned int) _temp2452 >  3u?*(( int*) _temp2452) ==  Cyc_Absyn_IntType:
0){ goto _LL2455;} else{ goto _LL2456;} _LL2456: if( _temp2452 == ( void*) Cyc_Absyn_FloatType){
goto _LL2457;} else{ goto _LL2458;} _LL2458: if(( unsigned int) _temp2452 >  3u?*((
int*) _temp2452) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2459;} else{ goto _LL2460;}
_LL2460: goto _LL2461; _LL2455: goto _LL2457; _LL2457: goto _LL2459; _LL2459:
return 1; _LL2461: return 0; _LL2453:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp2462=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp2462->f1= x->tq; _temp2462->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp2462;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp2463= t1; struct Cyc_List_List*
_temp2475; struct Cyc_Absyn_Structdecl** _temp2477; struct Cyc_List_List*
_temp2479; struct _tuple1* _temp2481; struct Cyc_List_List* _temp2483; _LL2465:
if( _temp2463 == ( void*) Cyc_Absyn_VoidType){ goto _LL2466;} else{ goto _LL2467;}
_LL2467: if(( unsigned int) _temp2463 >  3u?*(( int*) _temp2463) ==  Cyc_Absyn_TupleType:
0){ _LL2476: _temp2475=(( struct Cyc_Absyn_TupleType_struct*) _temp2463)->f1;
goto _LL2468;} else{ goto _LL2469;} _LL2469: if(( unsigned int) _temp2463 >  3u?*((
int*) _temp2463) ==  Cyc_Absyn_StructType: 0){ _LL2482: _temp2481=(( struct Cyc_Absyn_StructType_struct*)
_temp2463)->f1; goto _LL2480; _LL2480: _temp2479=(( struct Cyc_Absyn_StructType_struct*)
_temp2463)->f2; goto _LL2478; _LL2478: _temp2477=(( struct Cyc_Absyn_StructType_struct*)
_temp2463)->f3; goto _LL2470;} else{ goto _LL2471;} _LL2471: if(( unsigned int)
_temp2463 >  3u?*(( int*) _temp2463) ==  Cyc_Absyn_AnonStructType: 0){ _LL2484:
_temp2483=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2463)->f1; goto
_LL2472;} else{ goto _LL2473;} _LL2473: goto _LL2474; _LL2466: return 0; _LL2468:
return _temp2475; _LL2470: if( _temp2477 ==  0? 1: _temp2481 ==  0){ return({
struct Cyc_List_List* _temp2485=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2485->hd=( void*)({ struct _tuple4* _temp2486=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2486->f1= Cyc_Absyn_empty_tqual();
_temp2486->f2= t1; _temp2486;}); _temp2485->tl= 0; _temp2485;});}{ struct Cyc_Absyn_Structdecl*
_temp2487=* _temp2477; struct _tuple1 _temp2488=* _temp2481; struct Cyc_List_List*
_temp2489=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp2487->tvs, _temp2479); if( _temp2487->fields ==  0){
return({ struct Cyc_List_List* _temp2490=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2490->hd=( void*)({ struct _tuple4*
_temp2491=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2491->f1=
Cyc_Absyn_empty_tqual(); _temp2491->f2= t1; _temp2491;}); _temp2490->tl= 0;
_temp2490;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp2489,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2487->fields))->v);}
_LL2472: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp2483); _LL2474: return({
struct Cyc_List_List* _temp2492=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2492->hd=( void*)({ struct _tuple4* _temp2493=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2493->f1= Cyc_Absyn_empty_tqual();
_temp2493->f2= t1; _temp2493;}); _temp2492->tl= 0; _temp2492;}); _LL2464:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a= a->tl){
if( Cyc_Tcutil_unify( t1,(*(( struct _tuple8*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*((
struct _tuple8*) a->hd)).f2): 0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp2495=({ struct _tuple8 _temp2494;
_temp2494.f1= t1; _temp2494.f2= t2; _temp2494;}); void* _temp2501; struct Cyc_Absyn_PtrInfo
_temp2503; struct Cyc_Absyn_Conref* _temp2505; struct Cyc_Absyn_Tqual _temp2507;
struct Cyc_Absyn_Conref* _temp2509; void* _temp2511; void* _temp2513; void*
_temp2515; struct Cyc_Absyn_PtrInfo _temp2517; struct Cyc_Absyn_Conref*
_temp2519; struct Cyc_Absyn_Tqual _temp2521; struct Cyc_Absyn_Conref* _temp2523;
void* _temp2525; void* _temp2527; _LL2497: _LL2516: _temp2515= _temp2495.f1; if((
unsigned int) _temp2515 >  3u?*(( int*) _temp2515) ==  Cyc_Absyn_PointerType: 0){
_LL2518: _temp2517=(( struct Cyc_Absyn_PointerType_struct*) _temp2515)->f1;
_LL2528: _temp2527=( void*) _temp2517.elt_typ; goto _LL2526; _LL2526: _temp2525=(
void*) _temp2517.rgn_typ; goto _LL2524; _LL2524: _temp2523= _temp2517.nullable;
goto _LL2522; _LL2522: _temp2521= _temp2517.tq; goto _LL2520; _LL2520: _temp2519=
_temp2517.bounds; goto _LL2502;} else{ goto _LL2499;} _LL2502: _temp2501=
_temp2495.f2; if(( unsigned int) _temp2501 >  3u?*(( int*) _temp2501) ==  Cyc_Absyn_PointerType:
0){ _LL2504: _temp2503=(( struct Cyc_Absyn_PointerType_struct*) _temp2501)->f1;
_LL2514: _temp2513=( void*) _temp2503.elt_typ; goto _LL2512; _LL2512: _temp2511=(
void*) _temp2503.rgn_typ; goto _LL2510; _LL2510: _temp2509= _temp2503.nullable;
goto _LL2508; _LL2508: _temp2507= _temp2503.tq; goto _LL2506; _LL2506: _temp2505=
_temp2503.bounds; goto _LL2498;} else{ goto _LL2499;} _LL2499: goto _LL2500;
_LL2498: if( _temp2521.q_const? ! _temp2507.q_const: 0){ return 0;} if(( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp2523, _temp2509)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2523): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2509): 0){ return 0;}
if( ! Cyc_Tcutil_unify( _temp2525, _temp2511)? ! Cyc_Tcenv_region_outlives( te,
_temp2525, _temp2511): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2519, _temp2505)){ struct _tuple8 _temp2530=({ struct _tuple8 _temp2529;
_temp2529.f1= Cyc_Absyn_conref_val( _temp2519); _temp2529.f2= Cyc_Absyn_conref_val(
_temp2505); _temp2529;}); void* _temp2538; void* _temp2540; void* _temp2542;
struct Cyc_Absyn_Exp* _temp2544; void* _temp2546; struct Cyc_Absyn_Exp*
_temp2548; _LL2532: _LL2541: _temp2540= _temp2530.f1; if(( unsigned int)
_temp2540 >  1u?*(( int*) _temp2540) ==  Cyc_Absyn_Upper_b: 0){ goto _LL2539;}
else{ goto _LL2534;} _LL2539: _temp2538= _temp2530.f2; if( _temp2538 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2533;} else{ goto _LL2534;} _LL2534: _LL2547:
_temp2546= _temp2530.f1; if(( unsigned int) _temp2546 >  1u?*(( int*) _temp2546)
==  Cyc_Absyn_Upper_b: 0){ _LL2549: _temp2548=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2546)->f1; goto _LL2543;} else{ goto _LL2536;} _LL2543: _temp2542=
_temp2530.f2; if(( unsigned int) _temp2542 >  1u?*(( int*) _temp2542) ==  Cyc_Absyn_Upper_b:
0){ _LL2545: _temp2544=(( struct Cyc_Absyn_Upper_b_struct*) _temp2542)->f1; goto
_LL2535;} else{ goto _LL2536;} _LL2536: goto _LL2537; _LL2533: goto _LL2531;
_LL2535: if( Cyc_Evexp_eval_const_uint_exp( _temp2548) <  Cyc_Evexp_eval_const_uint_exp(
_temp2544)){ return 0;} goto _LL2531; _LL2537: return 0; _LL2531:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp2550=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2550->hd=( void*)({ struct _tuple8*
_temp2551=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2551->f1=
t1; _temp2551->f2= t2; _temp2551;}); _temp2550->tl= assume; _temp2550;}),
_temp2527, _temp2513); _LL2500: return 0; _LL2496:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp2553=({ struct _tuple8 _temp2552;
_temp2552.f1= Cyc_Tcutil_compress( t1); _temp2552.f2= Cyc_Tcutil_compress( t2);
_temp2552;}); void* _temp2559; void* _temp2561; void* _temp2563; void* _temp2565;
_LL2555: _LL2564: _temp2563= _temp2553.f1; if(( unsigned int) _temp2563 >  3u?*((
int*) _temp2563) ==  Cyc_Absyn_IntType: 0){ _LL2566: _temp2565=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2563)->f2; goto _LL2560;} else{ goto _LL2557;}
_LL2560: _temp2559= _temp2553.f2; if(( unsigned int) _temp2559 >  3u?*(( int*)
_temp2559) ==  Cyc_Absyn_IntType: 0){ _LL2562: _temp2561=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2559)->f2; goto _LL2556;} else{ goto _LL2557;} _LL2557: goto _LL2558;
_LL2556: return _temp2565 ==  _temp2561; _LL2558: return 0; _LL2554:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2= tqs2->tl, tqs1= tqs1->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp2569; void* _temp2570; struct Cyc_Absyn_Tqual _temp2572;
struct _tuple4* _temp2567=( struct _tuple4*) tqs1->hd; _temp2569=* _temp2567;
_LL2573: _temp2572= _temp2569.f1; goto _LL2571; _LL2571: _temp2570= _temp2569.f2;
goto _LL2568; _LL2568: { struct _tuple4 _temp2576; void* _temp2577; struct Cyc_Absyn_Tqual
_temp2579; struct _tuple4* _temp2574=( struct _tuple4*) tqs2->hd; _temp2576=*
_temp2574; _LL2580: _temp2579= _temp2576.f1; goto _LL2578; _LL2578: _temp2577=
_temp2576.f2; goto _LL2575; _LL2575: if( _temp2579.q_const? Cyc_Tcutil_subtype(
te, assume, _temp2570, _temp2577): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp2570, _temp2577)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp2570,
_temp2577)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp2581= t2; struct Cyc_Absyn_Uniondecl** _temp2589; struct Cyc_List_List*
_temp2591; void* _temp2593; _LL2583: if(( unsigned int) _temp2581 >  3u?*(( int*)
_temp2581) ==  Cyc_Absyn_UnionType: 0){ _LL2592: _temp2591=(( struct Cyc_Absyn_UnionType_struct*)
_temp2581)->f2; goto _LL2590; _LL2590: _temp2589=(( struct Cyc_Absyn_UnionType_struct*)
_temp2581)->f3; goto _LL2584;} else{ goto _LL2585;} _LL2585: if(( unsigned int)
_temp2581 >  3u?*(( int*) _temp2581) ==  Cyc_Absyn_IntType: 0){ _LL2594:
_temp2593=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2581)->f2; if(
_temp2593 == ( void*) Cyc_Absyn_B4){ goto _LL2586;} else{ goto _LL2587;}} else{
goto _LL2587;} _LL2587: goto _LL2588; _LL2584: if( _temp2589 ==  0){ goto
_LL2582;}{ struct Cyc_Absyn_Uniondecl* _temp2595=* _temp2589; if( _temp2595->fields
==  0){ goto _LL2582;}{ struct Cyc_List_List* _temp2596=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2595->tvs,
_temp2591);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2595->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp2596,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ return 1;}}} goto _LL2582;}} _LL2586: if( Cyc_Tcutil_typ_kind(
t2) == ( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL2582; _LL2588: goto
_LL2582; _LL2582:;}{ void* _temp2597= t1; struct Cyc_Absyn_PtrInfo _temp2615;
struct Cyc_Absyn_Conref* _temp2617; struct Cyc_Absyn_Tqual _temp2619; struct Cyc_Absyn_Conref*
_temp2621; void* _temp2623; void* _temp2625; struct Cyc_Absyn_Exp* _temp2627;
struct Cyc_Absyn_Tqual _temp2629; void* _temp2631; struct Cyc_Absyn_Enumdecl*
_temp2633; _LL2599: if(( unsigned int) _temp2597 >  3u?*(( int*) _temp2597) == 
Cyc_Absyn_PointerType: 0){ _LL2616: _temp2615=(( struct Cyc_Absyn_PointerType_struct*)
_temp2597)->f1; _LL2626: _temp2625=( void*) _temp2615.elt_typ; goto _LL2624;
_LL2624: _temp2623=( void*) _temp2615.rgn_typ; goto _LL2622; _LL2622: _temp2621=
_temp2615.nullable; goto _LL2620; _LL2620: _temp2619= _temp2615.tq; goto _LL2618;
_LL2618: _temp2617= _temp2615.bounds; goto _LL2600;} else{ goto _LL2601;}
_LL2601: if(( unsigned int) _temp2597 >  3u?*(( int*) _temp2597) ==  Cyc_Absyn_ArrayType:
0){ _LL2632: _temp2631=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2597)->f1;
goto _LL2630; _LL2630: _temp2629=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2597)->f2; goto _LL2628; _LL2628: _temp2627=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2597)->f3; goto _LL2602;} else{ goto _LL2603;} _LL2603: if(( unsigned int)
_temp2597 >  3u?*(( int*) _temp2597) ==  Cyc_Absyn_EnumType: 0){ _LL2634:
_temp2633=(( struct Cyc_Absyn_EnumType_struct*) _temp2597)->f2; goto _LL2604;}
else{ goto _LL2605;} _LL2605: if(( unsigned int) _temp2597 >  3u?*(( int*)
_temp2597) ==  Cyc_Absyn_IntType: 0){ goto _LL2606;} else{ goto _LL2607;}
_LL2607: if( _temp2597 == ( void*) Cyc_Absyn_FloatType){ goto _LL2608;} else{
goto _LL2609;} _LL2609: if(( unsigned int) _temp2597 >  3u?*(( int*) _temp2597)
==  Cyc_Absyn_DoubleType: 0){ goto _LL2610;} else{ goto _LL2611;} _LL2611: if(
_temp2597 == ( void*) Cyc_Absyn_VoidType){ goto _LL2612;} else{ goto _LL2613;}
_LL2613: goto _LL2614; _LL2600:{ void* _temp2635= t2; struct Cyc_Absyn_PtrInfo
_temp2643; struct Cyc_Absyn_Conref* _temp2645; struct Cyc_Absyn_Tqual _temp2647;
struct Cyc_Absyn_Conref* _temp2649; void* _temp2651; void* _temp2653; _LL2637:
if(( unsigned int) _temp2635 >  3u?*(( int*) _temp2635) ==  Cyc_Absyn_PointerType:
0){ _LL2644: _temp2643=(( struct Cyc_Absyn_PointerType_struct*) _temp2635)->f1;
_LL2654: _temp2653=( void*) _temp2643.elt_typ; goto _LL2652; _LL2652: _temp2651=(
void*) _temp2643.rgn_typ; goto _LL2650; _LL2650: _temp2649= _temp2643.nullable;
goto _LL2648; _LL2648: _temp2647= _temp2643.tq; goto _LL2646; _LL2646: _temp2645=
_temp2643.bounds; goto _LL2638;} else{ goto _LL2639;} _LL2639: if( _temp2635 == (
void*) Cyc_Absyn_VoidType){ goto _LL2640;} else{ goto _LL2641;} _LL2641: goto
_LL2642; _LL2638: { struct Cyc_List_List* _temp2655=({ struct Cyc_List_List*
_temp2672=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2672->hd=( void*)({ struct _tuple8* _temp2673=( struct _tuple8*) _cycalloc(
sizeof( struct _tuple8)); _temp2673->f1= t1; _temp2673->f2= t2; _temp2673;});
_temp2672->tl= 0; _temp2672;}); int _temp2656= Cyc_Tcutil_ptrsubtype( te,
_temp2655, _temp2625, _temp2653)? ! _temp2619.q_const? 1: _temp2647.q_const: 0;
int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2617,
_temp2645); if( ! bounds_ok){ struct _tuple8 _temp2658=({ struct _tuple8
_temp2657; _temp2657.f1= Cyc_Absyn_conref_val( _temp2617); _temp2657.f2= Cyc_Absyn_conref_val(
_temp2645); _temp2657;}); void* _temp2664; struct Cyc_Absyn_Exp* _temp2666; void*
_temp2668; struct Cyc_Absyn_Exp* _temp2670; _LL2660: _LL2669: _temp2668=
_temp2658.f1; if(( unsigned int) _temp2668 >  1u?*(( int*) _temp2668) ==  Cyc_Absyn_Upper_b:
0){ _LL2671: _temp2670=(( struct Cyc_Absyn_Upper_b_struct*) _temp2668)->f1; goto
_LL2665;} else{ goto _LL2662;} _LL2665: _temp2664= _temp2658.f2; if((
unsigned int) _temp2664 >  1u?*(( int*) _temp2664) ==  Cyc_Absyn_Upper_b: 0){
_LL2667: _temp2666=(( struct Cyc_Absyn_Upper_b_struct*) _temp2664)->f1; goto
_LL2661;} else{ goto _LL2662;} _LL2662: goto _LL2663; _LL2661: if( Cyc_Evexp_eval_const_uint_exp(
_temp2670) >=  Cyc_Evexp_eval_const_uint_exp( _temp2666)){ bounds_ok= 1;} goto
_LL2659; _LL2663: bounds_ok= 1; goto _LL2659; _LL2659:;} return( bounds_ok?
_temp2656: 0)? Cyc_Tcutil_unify( _temp2623, _temp2651)? 1: Cyc_Tcenv_region_outlives(
te, _temp2623, _temp2651): 0;} _LL2640: return 1; _LL2642: goto _LL2636; _LL2636:;}
return 0; _LL2602:{ void* _temp2674= t2; struct Cyc_Absyn_Exp* _temp2680; struct
Cyc_Absyn_Tqual _temp2682; void* _temp2684; _LL2676: if(( unsigned int)
_temp2674 >  3u?*(( int*) _temp2674) ==  Cyc_Absyn_ArrayType: 0){ _LL2685:
_temp2684=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2674)->f1; goto
_LL2683; _LL2683: _temp2682=(( struct Cyc_Absyn_ArrayType_struct*) _temp2674)->f2;
goto _LL2681; _LL2681: _temp2680=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2674)->f3; goto _LL2677;} else{ goto _LL2678;} _LL2678: goto _LL2679;
_LL2677: { int okay; okay=( _temp2627 !=  0? _temp2680 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2627)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2680)): 0; return( okay? Cyc_Tcutil_unify(
_temp2631, _temp2684): 0)? ! _temp2629.q_const? 1: _temp2682.q_const: 0;}
_LL2679: return 0; _LL2675:;} return 0; _LL2604:{ void* _temp2686= t2; struct
Cyc_Absyn_Enumdecl* _temp2692; _LL2688: if(( unsigned int) _temp2686 >  3u?*((
int*) _temp2686) ==  Cyc_Absyn_EnumType: 0){ _LL2693: _temp2692=(( struct Cyc_Absyn_EnumType_struct*)
_temp2686)->f2; goto _LL2689;} else{ goto _LL2690;} _LL2690: goto _LL2691;
_LL2689: if(( _temp2633->fields !=  0? _temp2692->fields !=  0: 0)?(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2633->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2692->fields))->v):
0){ return 1;} goto _LL2687; _LL2691: goto _LL2687; _LL2687:;} goto _LL2606;
_LL2606: goto _LL2608; _LL2608: goto _LL2610; _LL2610: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType; _LL2612: return t2 == ( void*) Cyc_Absyn_VoidType;
_LL2614: return 0; _LL2598:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2694=
Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2695=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2695[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2696; _temp2696.tag= Cyc_Absyn_Cast_e;
_temp2696.f1=( void*) t; _temp2696.f2= _temp2694; _temp2696;}); _temp2695;})));
e->topt=({ struct Cyc_Core_Opt* _temp2697=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2697->v=( void*) t; _temp2697;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2698= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2700: if(( unsigned int) _temp2698
>  3u?*(( int*) _temp2698) ==  Cyc_Absyn_EnumType: 0){ goto _LL2701;} else{ goto
_LL2702;} _LL2702: if(( unsigned int) _temp2698 >  3u?*(( int*) _temp2698) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2703;} else{ goto _LL2704;} _LL2704: if((
unsigned int) _temp2698 >  3u?*(( int*) _temp2698) ==  Cyc_Absyn_IntType: 0){
goto _LL2705;} else{ goto _LL2706;} _LL2706: if(( unsigned int) _temp2698 >  3u?*((
int*) _temp2698) ==  Cyc_Absyn_Evar: 0){ goto _LL2707;} else{ goto _LL2708;}
_LL2708: goto _LL2709; _LL2701: goto _LL2703; _LL2703: goto _LL2705; _LL2705:
return 1; _LL2707: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2709: return 0; _LL2699:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2710= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2712: if( _temp2710 == ( void*) Cyc_Absyn_FloatType){
goto _LL2713;} else{ goto _LL2714;} _LL2714: if(( unsigned int) _temp2710 >  3u?*((
int*) _temp2710) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2715;} else{ goto _LL2716;}
_LL2716: goto _LL2717; _LL2713: goto _LL2715; _LL2715: return 1; _LL2717: return
0; _LL2711:;}} int Cyc_Tcutil_is_function_type( void* t){ void* _temp2718= Cyc_Tcutil_compress(
t); _LL2720: if(( unsigned int) _temp2718 >  3u?*(( int*) _temp2718) ==  Cyc_Absyn_FnType:
0){ goto _LL2721;} else{ goto _LL2722;} _LL2722: goto _LL2723; _LL2721: return 1;
_LL2723: return 0; _LL2719:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1,
void* t2){ struct _tuple8 _temp2725=({ struct _tuple8 _temp2724; _temp2724.f1=
t1; _temp2724.f2= t2; _temp2724;}); void* _temp2751; int _temp2753; void*
_temp2755; int _temp2757; void* _temp2759; void* _temp2761; void* _temp2763;
void* _temp2765; void* _temp2767; void* _temp2769; void* _temp2771; void*
_temp2773; void* _temp2775; void* _temp2777; void* _temp2779; void* _temp2781;
void* _temp2783; void* _temp2785; void* _temp2787; void* _temp2789; void*
_temp2791; void* _temp2793; void* _temp2795; void* _temp2797; void* _temp2799;
void* _temp2801; _LL2727: _LL2756: _temp2755= _temp2725.f1; if(( unsigned int)
_temp2755 >  3u?*(( int*) _temp2755) ==  Cyc_Absyn_DoubleType: 0){ _LL2758:
_temp2757=(( struct Cyc_Absyn_DoubleType_struct*) _temp2755)->f1; goto _LL2752;}
else{ goto _LL2729;} _LL2752: _temp2751= _temp2725.f2; if(( unsigned int)
_temp2751 >  3u?*(( int*) _temp2751) ==  Cyc_Absyn_DoubleType: 0){ _LL2754:
_temp2753=(( struct Cyc_Absyn_DoubleType_struct*) _temp2751)->f1; goto _LL2728;}
else{ goto _LL2729;} _LL2729: _LL2760: _temp2759= _temp2725.f1; if((
unsigned int) _temp2759 >  3u?*(( int*) _temp2759) ==  Cyc_Absyn_DoubleType: 0){
goto _LL2730;} else{ goto _LL2731;} _LL2731: _LL2762: _temp2761= _temp2725.f2;
if(( unsigned int) _temp2761 >  3u?*(( int*) _temp2761) ==  Cyc_Absyn_DoubleType:
0){ goto _LL2732;} else{ goto _LL2733;} _LL2733: _LL2764: _temp2763= _temp2725.f1;
if( _temp2763 == ( void*) Cyc_Absyn_FloatType){ goto _LL2734;} else{ goto
_LL2735;} _LL2735: _LL2766: _temp2765= _temp2725.f2; if( _temp2765 == ( void*)
Cyc_Absyn_FloatType){ goto _LL2736;} else{ goto _LL2737;} _LL2737: _LL2768:
_temp2767= _temp2725.f1; if(( unsigned int) _temp2767 >  3u?*(( int*) _temp2767)
==  Cyc_Absyn_IntType: 0){ _LL2772: _temp2771=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2767)->f1; if( _temp2771 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2770;}
else{ goto _LL2739;} _LL2770: _temp2769=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2767)->f2; if( _temp2769 == ( void*) Cyc_Absyn_B8){ goto _LL2738;} else{
goto _LL2739;}} else{ goto _LL2739;} _LL2739: _LL2774: _temp2773= _temp2725.f2;
if(( unsigned int) _temp2773 >  3u?*(( int*) _temp2773) ==  Cyc_Absyn_IntType: 0){
_LL2778: _temp2777=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2773)->f1;
if( _temp2777 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2776;} else{ goto _LL2741;}
_LL2776: _temp2775=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2773)->f2;
if( _temp2775 == ( void*) Cyc_Absyn_B8){ goto _LL2740;} else{ goto _LL2741;}}
else{ goto _LL2741;} _LL2741: _LL2780: _temp2779= _temp2725.f1; if((
unsigned int) _temp2779 >  3u?*(( int*) _temp2779) ==  Cyc_Absyn_IntType: 0){
_LL2784: _temp2783=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2779)->f1;
if( _temp2783 == ( void*) Cyc_Absyn_Signed){ goto _LL2782;} else{ goto _LL2743;}
_LL2782: _temp2781=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2779)->f2;
if( _temp2781 == ( void*) Cyc_Absyn_B8){ goto _LL2742;} else{ goto _LL2743;}}
else{ goto _LL2743;} _LL2743: _LL2786: _temp2785= _temp2725.f2; if((
unsigned int) _temp2785 >  3u?*(( int*) _temp2785) ==  Cyc_Absyn_IntType: 0){
_LL2790: _temp2789=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2785)->f1;
if( _temp2789 == ( void*) Cyc_Absyn_Signed){ goto _LL2788;} else{ goto _LL2745;}
_LL2788: _temp2787=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2785)->f2;
if( _temp2787 == ( void*) Cyc_Absyn_B8){ goto _LL2744;} else{ goto _LL2745;}}
else{ goto _LL2745;} _LL2745: _LL2792: _temp2791= _temp2725.f1; if((
unsigned int) _temp2791 >  3u?*(( int*) _temp2791) ==  Cyc_Absyn_IntType: 0){
_LL2796: _temp2795=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2791)->f1;
if( _temp2795 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2794;} else{ goto _LL2747;}
_LL2794: _temp2793=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2791)->f2;
if( _temp2793 == ( void*) Cyc_Absyn_B4){ goto _LL2746;} else{ goto _LL2747;}}
else{ goto _LL2747;} _LL2747: _LL2798: _temp2797= _temp2725.f2; if((
unsigned int) _temp2797 >  3u?*(( int*) _temp2797) ==  Cyc_Absyn_IntType: 0){
_LL2802: _temp2801=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2797)->f1;
if( _temp2801 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2800;} else{ goto _LL2749;}
_LL2800: _temp2799=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2797)->f2;
if( _temp2799 == ( void*) Cyc_Absyn_B4){ goto _LL2748;} else{ goto _LL2749;}}
else{ goto _LL2749;} _LL2749: goto _LL2750; _LL2728: if( _temp2757){ return t1;}
else{ return t2;} _LL2730: return t1; _LL2732: return t2; _LL2734: goto _LL2736;
_LL2736: return( void*) Cyc_Absyn_FloatType; _LL2738: goto _LL2740; _LL2740:
return Cyc_Absyn_ulonglong_t; _LL2742: goto _LL2744; _LL2744: return Cyc_Absyn_slonglong_t;
_LL2746: goto _LL2748; _LL2748: return Cyc_Absyn_uint_t; _LL2750: return Cyc_Absyn_sint_t;
_LL2726:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2803=( void*) e->r; struct Cyc_Core_Opt* _temp2809; _LL2805: if(*(( int*)
_temp2803) ==  Cyc_Absyn_AssignOp_e){ _LL2810: _temp2809=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2803)->f2; if( _temp2809 ==  0){ goto _LL2806;} else{ goto _LL2807;}} else{
goto _LL2807;} _LL2807: goto _LL2808; _LL2806:({ void* _temp2811[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2811, sizeof( void*), 0u));}); goto _LL2804; _LL2808: goto _LL2804; _LL2804:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2812=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2812[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2813; _temp2813.tag=
Cyc_Absyn_Forward_constr; _temp2813.f1= c2; _temp2813;}); _temp2812;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2814=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2814[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2815; _temp2815.tag= Cyc_Absyn_Forward_constr;
_temp2815.f1= c1; _temp2815;}); _temp2814;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2816=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2816[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2817; _temp2817.tag= Cyc_Absyn_Forward_constr; _temp2817.f1= c1; _temp2817;});
_temp2816;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2818=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2818[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2819; _temp2819.tag= Cyc_Absyn_Forward_constr;
_temp2819.f1= c2; _temp2819;}); _temp2818;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2820=( int*) _cycalloc_atomic( sizeof( int)); _temp2820[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2820;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2824;
_temp2824.tag= Cyc_Std_Int_pa; _temp2824.f1=( int)(( unsigned int) i);{ void*
_temp2823[ 1u]={& _temp2824}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2823, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2821=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2821->name=({ struct _tagged_arr* _temp2822=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2822[ 0]=
s; _temp2822;}); _temp2821->identity= 0; _temp2821->kind= Cyc_Absyn_new_conref(
k); _temp2821;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2825=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2825, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2827; _temp2827.tag= Cyc_Std_String_pa; _temp2827.f1=( struct _tagged_arr)*
t->name;{ void* _temp2826[ 1u]={& _temp2827}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2826, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2828= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2828, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2829=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2829[ 0]=( struct _tagged_arr) _temp2828; _temp2829;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2830=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2830->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2831=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2831->v=( void*)(*
x).f1; _temp2831;}); _temp2830->f2=(* x).f2; _temp2830->f3=(* x).f3; _temp2830;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2832= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts= atts->tl){ if( Cyc_Absyn_fntype_att(( void*) atts->hd)){
_temp2832=({ struct Cyc_List_List* _temp2833=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2833->hd=( void*)(( void*) atts->hd);
_temp2833->tl= _temp2832; _temp2833;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2834=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2834[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2835; _temp2835.tag= Cyc_Absyn_FnType;
_temp2835.f1=({ struct Cyc_Absyn_FnInfo _temp2836; _temp2836.tvars= fd->tvs;
_temp2836.effect= fd->effect; _temp2836.ret_typ=( void*)(( void*) fd->ret_type);
_temp2836.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2836.c_varargs= fd->c_varargs; _temp2836.cyc_varargs= fd->cyc_varargs;
_temp2836.rgn_po= fd->rgn_po; _temp2836.attributes= _temp2832; _temp2836;});
_temp2835;}); _temp2834;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp2837=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp2837->f1=(* pr).f1; _temp2837->f2= t; _temp2837;});} struct _tuple15{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct
_tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2838=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2838->f1=({ struct _tuple15* _temp2839=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2839->f1=(* y).f1; _temp2839->f2=(* y).f2;
_temp2839;}); _temp2838->f2=(* y).f3; _temp2838;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2842; struct _tuple15* _temp2844; struct
_tuple16 _temp2840=* w; _LL2845: _temp2844= _temp2840.f1; goto _LL2843; _LL2843:
_temp2842= _temp2840.f2; goto _LL2841; _LL2841: { struct Cyc_Absyn_Tqual
_temp2848; struct Cyc_Core_Opt* _temp2850; struct _tuple15 _temp2846=* _temp2844;
_LL2851: _temp2850= _temp2846.f1; goto _LL2849; _LL2849: _temp2848= _temp2846.f2;
goto _LL2847; _LL2847: return({ struct _tuple2* _temp2852=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2852->f1= _temp2850; _temp2852->f2=
_temp2848; _temp2852->f3= _temp2842; _temp2852;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2853=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2853->name= f->name;
_temp2853->tq= f->tq; _temp2853->type=( void*) t; _temp2853->width= f->width;
_temp2853->attributes= f->attributes; _temp2853;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2854= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2904; struct Cyc_Absyn_Structdecl** _temp2906;
struct Cyc_List_List* _temp2908; struct _tuple1* _temp2910; struct Cyc_Absyn_Uniondecl**
_temp2912; struct Cyc_List_List* _temp2914; struct _tuple1* _temp2916; struct
Cyc_Absyn_TunionInfo _temp2918; void* _temp2920; struct Cyc_List_List* _temp2922;
void* _temp2924; struct Cyc_Absyn_TunionFieldInfo _temp2926; struct Cyc_List_List*
_temp2928; void* _temp2930; struct Cyc_Core_Opt* _temp2932; struct Cyc_List_List*
_temp2934; struct _tuple1* _temp2936; struct Cyc_Absyn_Exp* _temp2938; struct
Cyc_Absyn_Tqual _temp2940; void* _temp2942; struct Cyc_Absyn_PtrInfo _temp2944;
struct Cyc_Absyn_Conref* _temp2946; struct Cyc_Absyn_Tqual _temp2948; struct Cyc_Absyn_Conref*
_temp2950; void* _temp2952; void* _temp2954; struct Cyc_Absyn_FnInfo _temp2956;
struct Cyc_List_List* _temp2958; struct Cyc_List_List* _temp2960; struct Cyc_Absyn_VarargInfo*
_temp2962; int _temp2964; struct Cyc_List_List* _temp2966; void* _temp2968;
struct Cyc_Core_Opt* _temp2970; struct Cyc_List_List* _temp2972; struct Cyc_List_List*
_temp2974; struct Cyc_List_List* _temp2976; struct Cyc_List_List* _temp2978;
struct Cyc_Core_Opt* _temp2980; void* _temp2982; void* _temp2984; void*
_temp2986; struct Cyc_List_List* _temp2988; _LL2856: if(( unsigned int)
_temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_VarType: 0){ _LL2905:
_temp2904=(( struct Cyc_Absyn_VarType_struct*) _temp2854)->f1; goto _LL2857;}
else{ goto _LL2858;} _LL2858: if(( unsigned int) _temp2854 >  3u?*(( int*)
_temp2854) ==  Cyc_Absyn_StructType: 0){ _LL2911: _temp2910=(( struct Cyc_Absyn_StructType_struct*)
_temp2854)->f1; goto _LL2909; _LL2909: _temp2908=(( struct Cyc_Absyn_StructType_struct*)
_temp2854)->f2; goto _LL2907; _LL2907: _temp2906=(( struct Cyc_Absyn_StructType_struct*)
_temp2854)->f3; goto _LL2859;} else{ goto _LL2860;} _LL2860: if(( unsigned int)
_temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_UnionType: 0){ _LL2917:
_temp2916=(( struct Cyc_Absyn_UnionType_struct*) _temp2854)->f1; goto _LL2915;
_LL2915: _temp2914=(( struct Cyc_Absyn_UnionType_struct*) _temp2854)->f2; goto
_LL2913; _LL2913: _temp2912=(( struct Cyc_Absyn_UnionType_struct*) _temp2854)->f3;
goto _LL2861;} else{ goto _LL2862;} _LL2862: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_TunionType: 0){ _LL2919: _temp2918=(( struct Cyc_Absyn_TunionType_struct*)
_temp2854)->f1; _LL2925: _temp2924=( void*) _temp2918.tunion_info; goto _LL2923;
_LL2923: _temp2922= _temp2918.targs; goto _LL2921; _LL2921: _temp2920=( void*)
_temp2918.rgn; goto _LL2863;} else{ goto _LL2864;} _LL2864: if(( unsigned int)
_temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2927:
_temp2926=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2854)->f1; _LL2931:
_temp2930=( void*) _temp2926.field_info; goto _LL2929; _LL2929: _temp2928=
_temp2926.targs; goto _LL2865;} else{ goto _LL2866;} _LL2866: if(( unsigned int)
_temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_TypedefType: 0){ _LL2937:
_temp2936=(( struct Cyc_Absyn_TypedefType_struct*) _temp2854)->f1; goto _LL2935;
_LL2935: _temp2934=(( struct Cyc_Absyn_TypedefType_struct*) _temp2854)->f2; goto
_LL2933; _LL2933: _temp2932=(( struct Cyc_Absyn_TypedefType_struct*) _temp2854)->f3;
goto _LL2867;} else{ goto _LL2868;} _LL2868: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_ArrayType: 0){ _LL2943: _temp2942=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2854)->f1; goto _LL2941; _LL2941:
_temp2940=(( struct Cyc_Absyn_ArrayType_struct*) _temp2854)->f2; goto _LL2939;
_LL2939: _temp2938=(( struct Cyc_Absyn_ArrayType_struct*) _temp2854)->f3; goto
_LL2869;} else{ goto _LL2870;} _LL2870: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_PointerType: 0){ _LL2945: _temp2944=(( struct Cyc_Absyn_PointerType_struct*)
_temp2854)->f1; _LL2955: _temp2954=( void*) _temp2944.elt_typ; goto _LL2953;
_LL2953: _temp2952=( void*) _temp2944.rgn_typ; goto _LL2951; _LL2951: _temp2950=
_temp2944.nullable; goto _LL2949; _LL2949: _temp2948= _temp2944.tq; goto _LL2947;
_LL2947: _temp2946= _temp2944.bounds; goto _LL2871;} else{ goto _LL2872;}
_LL2872: if(( unsigned int) _temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_FnType:
0){ _LL2957: _temp2956=(( struct Cyc_Absyn_FnType_struct*) _temp2854)->f1;
_LL2973: _temp2972= _temp2956.tvars; goto _LL2971; _LL2971: _temp2970= _temp2956.effect;
goto _LL2969; _LL2969: _temp2968=( void*) _temp2956.ret_typ; goto _LL2967;
_LL2967: _temp2966= _temp2956.args; goto _LL2965; _LL2965: _temp2964= _temp2956.c_varargs;
goto _LL2963; _LL2963: _temp2962= _temp2956.cyc_varargs; goto _LL2961; _LL2961:
_temp2960= _temp2956.rgn_po; goto _LL2959; _LL2959: _temp2958= _temp2956.attributes;
goto _LL2873;} else{ goto _LL2874;} _LL2874: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_TupleType: 0){ _LL2975: _temp2974=(( struct Cyc_Absyn_TupleType_struct*)
_temp2854)->f1; goto _LL2875;} else{ goto _LL2876;} _LL2876: if(( unsigned int)
_temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_AnonStructType: 0){ _LL2977:
_temp2976=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2854)->f1; goto
_LL2877;} else{ goto _LL2878;} _LL2878: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2979: _temp2978=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2854)->f1; goto _LL2879;} else{ goto
_LL2880;} _LL2880: if(( unsigned int) _temp2854 >  3u?*(( int*) _temp2854) == 
Cyc_Absyn_Evar: 0){ _LL2981: _temp2980=(( struct Cyc_Absyn_Evar_struct*)
_temp2854)->f2; goto _LL2881;} else{ goto _LL2882;} _LL2882: if(( unsigned int)
_temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2983:
_temp2982=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2854)->f1;
goto _LL2883;} else{ goto _LL2884;} _LL2884: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_EnumType: 0){ goto _LL2885;} else{ goto _LL2886;}
_LL2886: if(( unsigned int) _temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL2887;} else{ goto _LL2888;} _LL2888: if( _temp2854 == ( void*) Cyc_Absyn_VoidType){
goto _LL2889;} else{ goto _LL2890;} _LL2890: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_IntType: 0){ goto _LL2891;} else{ goto _LL2892;}
_LL2892: if( _temp2854 == ( void*) Cyc_Absyn_FloatType){ goto _LL2893;} else{
goto _LL2894;} _LL2894: if(( unsigned int) _temp2854 >  3u?*(( int*) _temp2854)
==  Cyc_Absyn_DoubleType: 0){ goto _LL2895;} else{ goto _LL2896;} _LL2896: if(
_temp2854 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL2897;} else{ goto _LL2898;}
_LL2898: if(( unsigned int) _temp2854 >  3u?*(( int*) _temp2854) ==  Cyc_Absyn_RgnsEff:
0){ _LL2985: _temp2984=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp2854)->f1;
goto _LL2899;} else{ goto _LL2900;} _LL2900: if(( unsigned int) _temp2854 >  3u?*((
int*) _temp2854) ==  Cyc_Absyn_AccessEff: 0){ _LL2987: _temp2986=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2854)->f1; goto _LL2901;} else{ goto
_LL2902;} _LL2902: if(( unsigned int) _temp2854 >  3u?*(( int*) _temp2854) == 
Cyc_Absyn_JoinEff: 0){ _LL2989: _temp2988=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2854)->f1; goto _LL2903;} else{ goto _LL2855;} _LL2857: { struct
_handler_cons _temp2990; _push_handler(& _temp2990);{ int _temp2992= 0; if(
setjmp( _temp2990.handler)){ _temp2992= 1;} if( ! _temp2992){{ void* _temp2993=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2904); _npop_handler( 0u); return _temp2993;}; _pop_handler();} else{
void* _temp2991=( void*) _exn_thrown; void* _temp2995= _temp2991; _LL2997: if(
_temp2995 ==  Cyc_Core_Not_found){ goto _LL2998;} else{ goto _LL2999;} _LL2999:
goto _LL3000; _LL2998: return t; _LL3000:( void) _throw( _temp2995); _LL2996:;}}}
_LL2859: { struct Cyc_List_List* _temp3001= Cyc_Tcutil_substs( rgn, inst,
_temp2908); return _temp3001 ==  _temp2908? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp3002=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3002[ 0]=({ struct Cyc_Absyn_StructType_struct _temp3003; _temp3003.tag=
Cyc_Absyn_StructType; _temp3003.f1= _temp2910; _temp3003.f2= _temp3001;
_temp3003.f3= _temp2906; _temp3003;}); _temp3002;});} _LL2861: { struct Cyc_List_List*
_temp3004= Cyc_Tcutil_substs( rgn, inst, _temp2914); return _temp3004 == 
_temp2914? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp3005=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3005[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3006; _temp3006.tag= Cyc_Absyn_UnionType;
_temp3006.f1= _temp2916; _temp3006.f2= _temp3004; _temp3006.f3= _temp2912;
_temp3006;}); _temp3005;});} _LL2863: { struct Cyc_List_List* _temp3007= Cyc_Tcutil_substs(
rgn, inst, _temp2922); void* _temp3008= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2920); return( _temp3007 ==  _temp2922? _temp3008 ==  _temp2920: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp3009=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3009[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3010; _temp3010.tag= Cyc_Absyn_TunionType;
_temp3010.f1=({ struct Cyc_Absyn_TunionInfo _temp3011; _temp3011.tunion_info=(
void*) _temp2924; _temp3011.targs= _temp3007; _temp3011.rgn=( void*) _temp3008;
_temp3011;}); _temp3010;}); _temp3009;});} _LL2865: { struct Cyc_List_List*
_temp3012= Cyc_Tcutil_substs( rgn, inst, _temp2928); return _temp3012 == 
_temp2928? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp3013=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp3013[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp3014; _temp3014.tag=
Cyc_Absyn_TunionFieldType; _temp3014.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp3015; _temp3015.field_info=( void*) _temp2930; _temp3015.targs= _temp3012;
_temp3015;}); _temp3014;}); _temp3013;});} _LL2867: { struct Cyc_List_List*
_temp3016= Cyc_Tcutil_substs( rgn, inst, _temp2934); return _temp3016 == 
_temp2934? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp3017=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3017[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3018; _temp3018.tag=
Cyc_Absyn_TypedefType; _temp3018.f1= _temp2936; _temp3018.f2= _temp3016;
_temp3018.f3= _temp2932; _temp3018;}); _temp3017;});} _LL2869: { void* _temp3019=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2942); return _temp3019 ==  _temp2942? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp3020=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp3020[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp3021; _temp3021.tag= Cyc_Absyn_ArrayType;
_temp3021.f1=( void*) _temp3019; _temp3021.f2= _temp2940; _temp3021.f3=
_temp2938; _temp3021;}); _temp3020;});} _LL2871: { void* _temp3022= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2954); void* _temp3023= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2952); if( _temp3022 ==  _temp2954? _temp3023 ==  _temp2952: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp3024=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp3024[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp3025; _temp3025.tag= Cyc_Absyn_PointerType;
_temp3025.f1=({ struct Cyc_Absyn_PtrInfo _temp3026; _temp3026.elt_typ=( void*)
_temp3022; _temp3026.rgn_typ=( void*) _temp3023; _temp3026.nullable= _temp2950;
_temp3026.tq= _temp2948; _temp3026.bounds= _temp2946; _temp3026;}); _temp3025;});
_temp3024;});} _LL2873:{ struct Cyc_List_List* _temp3027= _temp2972; for( 0;
_temp3027 !=  0; _temp3027= _temp3027->tl){ inst=({ struct Cyc_List_List*
_temp3028=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3028->hd=( void*)({ struct _tuple5* _temp3029=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp3029->f1=( struct Cyc_Absyn_Tvar*)
_temp3027->hd; _temp3029->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3030=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3030[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3031; _temp3031.tag= Cyc_Absyn_VarType;
_temp3031.f1=( struct Cyc_Absyn_Tvar*) _temp3027->hd; _temp3031;}); _temp3030;});
_temp3029;}); _temp3028->tl= inst; _temp3028;});}}{ struct Cyc_List_List*
_temp3034; struct Cyc_List_List* _temp3036; struct _tuple0 _temp3032=(( struct
_tuple0(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp2966)); _LL3037: _temp3036= _temp3032.f1; goto _LL3035; _LL3035:
_temp3034= _temp3032.f2; goto _LL3033; _LL3033: { struct Cyc_List_List*
_temp3038= Cyc_Tcutil_substs( rgn, inst, _temp3034); struct Cyc_List_List*
_temp3039=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple16*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3036, _temp3038));
struct Cyc_Core_Opt* eff2; if( _temp2970 ==  0){ eff2= 0;} else{ void* _temp3040=
Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp2970->v); if( _temp3040 == (
void*) _temp2970->v){ eff2= _temp2970;} else{ eff2=({ struct Cyc_Core_Opt*
_temp3041=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3041->v=( void*) _temp3040; _temp3041;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2962 ==  0){ cyc_varargs2= 0;} else{ int _temp3044; void*
_temp3046; struct Cyc_Absyn_Tqual _temp3048; struct Cyc_Core_Opt* _temp3050;
struct Cyc_Absyn_VarargInfo _temp3042=* _temp2962; _LL3051: _temp3050= _temp3042.name;
goto _LL3049; _LL3049: _temp3048= _temp3042.tq; goto _LL3047; _LL3047: _temp3046=(
void*) _temp3042.type; goto _LL3045; _LL3045: _temp3044= _temp3042.inject; goto
_LL3043; _LL3043: { void* _temp3052= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3046); if( _temp3052 ==  _temp3046){ cyc_varargs2= _temp2962;} else{
cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp3053=( struct Cyc_Absyn_VarargInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp3053->name= _temp3050;
_temp3053->tq= _temp3048; _temp3053->type=( void*) _temp3052; _temp3053->inject=
_temp3044; _temp3053;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List*
_temp3056; struct Cyc_List_List* _temp3058; struct _tuple0 _temp3054= Cyc_List_rsplit(
rgn, rgn, _temp2960); _LL3059: _temp3058= _temp3054.f1; goto _LL3057; _LL3057:
_temp3056= _temp3054.f2; goto _LL3055; _LL3055: { struct Cyc_List_List*
_temp3060= Cyc_Tcutil_substs( rgn, inst, _temp3058); struct Cyc_List_List*
_temp3061= Cyc_Tcutil_substs( rgn, inst, _temp3056); if( _temp3060 ==  _temp3058?
_temp3061 ==  _temp3056: 0){ rgn_po2= _temp2960;} else{ rgn_po2= Cyc_List_zip(
_temp3060, _temp3061);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp3062=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp3062[ 0]=({ struct Cyc_Absyn_FnType_struct _temp3063; _temp3063.tag= Cyc_Absyn_FnType;
_temp3063.f1=({ struct Cyc_Absyn_FnInfo _temp3064; _temp3064.tvars= _temp2972;
_temp3064.effect= eff2; _temp3064.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2968); _temp3064.args= _temp3039; _temp3064.c_varargs= _temp2964;
_temp3064.cyc_varargs= cyc_varargs2; _temp3064.rgn_po= rgn_po2; _temp3064.attributes=
_temp2958; _temp3064;}); _temp3063;}); _temp3062;});}}}}} _LL2875: { struct Cyc_List_List*
_temp3065=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2974); struct Cyc_List_List* _temp3066= Cyc_Tcutil_substs( rgn, inst,
_temp3065); if( _temp3066 ==  _temp3065){ return t;}{ struct Cyc_List_List*
_temp3067=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2974, _temp3066); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3068=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3068[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3069; _temp3069.tag= Cyc_Absyn_TupleType;
_temp3069.f1= _temp3067; _temp3069;}); _temp3068;});}} _LL2877: { struct Cyc_List_List*
_temp3070=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2976); struct Cyc_List_List* _temp3071= Cyc_Tcutil_substs( rgn, inst,
_temp3070); if( _temp3071 ==  _temp3070){ return t;}{ struct Cyc_List_List*
_temp3072=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2976, _temp3071); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3073=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3073[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3074; _temp3074.tag= Cyc_Absyn_AnonStructType;
_temp3074.f1= _temp3072; _temp3074;}); _temp3073;});}} _LL2879: { struct Cyc_List_List*
_temp3075=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2978); struct Cyc_List_List* _temp3076= Cyc_Tcutil_substs( rgn, inst,
_temp3075); if( _temp3076 ==  _temp3075){ return t;}{ struct Cyc_List_List*
_temp3077=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2978, _temp3076); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3078=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3078[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3079; _temp3079.tag= Cyc_Absyn_AnonStructType;
_temp3079.f1= _temp3077; _temp3079;}); _temp3078;});}} _LL2881: if( _temp2980 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp2980->v);} else{
return t;} _LL2883: { void* _temp3080= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2982); return _temp3080 ==  _temp2982? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3081=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3081[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3082; _temp3082.tag= Cyc_Absyn_RgnHandleType; _temp3082.f1=( void*)
_temp3080; _temp3082;}); _temp3081;});} _LL2885: return t; _LL2887: return t;
_LL2889: return t; _LL2891: return t; _LL2893: return t; _LL2895: return t;
_LL2897: return t; _LL2899: { void* _temp3083= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2984); return _temp3083 ==  _temp2984? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp3084=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3084[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3085; _temp3085.tag= Cyc_Absyn_RgnsEff;
_temp3085.f1=( void*) _temp3083; _temp3085;}); _temp3084;});} _LL2901: { void*
_temp3086= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2986); return _temp3086 == 
_temp2986? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3087=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3087[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3088; _temp3088.tag= Cyc_Absyn_AccessEff;
_temp3088.f1=( void*) _temp3086; _temp3088;}); _temp3087;});} _LL2903: { struct
Cyc_List_List* _temp3089= Cyc_Tcutil_substs( rgn, inst, _temp2988); return
_temp3089 ==  _temp2988? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3090=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3090[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3091; _temp3091.tag= Cyc_Absyn_JoinEff;
_temp3091.f1= _temp3089; _temp3091;}); _temp3090;});} _LL2855:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp3092=(
void*) ts->hd; struct Cyc_List_List* _temp3093= ts->tl; void* _temp3094= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3092); struct Cyc_List_List* _temp3095= Cyc_Tcutil_substs( rgn,
inst, _temp3093); if( _temp3092 ==  _temp3094? _temp3093 ==  _temp3095: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp3096=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3096->hd=(
void*) _temp3094; _temp3096->tl= _temp3095; _temp3096;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp3141=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3141[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3142; _temp3142.tag= Cyc_Absyn_Const_e; _temp3142.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp3142;}); _temp3141;}), loc);{ void*
_temp3097= Cyc_Tcutil_compress( t); void* _temp3111; void* _temp3113; void*
_temp3115; void* _temp3117; _LL3099: if(( unsigned int) _temp3097 >  3u?*(( int*)
_temp3097) ==  Cyc_Absyn_PointerType: 0){ goto _LL3100;} else{ goto _LL3101;}
_LL3101: if(( unsigned int) _temp3097 >  3u?*(( int*) _temp3097) ==  Cyc_Absyn_IntType:
0){ _LL3114: _temp3113=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3097)->f1;
goto _LL3112; _LL3112: _temp3111=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3097)->f2; if( _temp3111 == ( void*) Cyc_Absyn_B1){ goto _LL3102;} else{
goto _LL3103;}} else{ goto _LL3103;} _LL3103: if(( unsigned int) _temp3097 >  3u?*((
int*) _temp3097) ==  Cyc_Absyn_IntType: 0){ _LL3118: _temp3117=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp3097)->f1; goto _LL3116; _LL3116: _temp3115=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3097)->f2; goto _LL3104;} else{
goto _LL3105;} _LL3105: if( _temp3097 == ( void*) Cyc_Absyn_FloatType){ goto
_LL3106;} else{ goto _LL3107;} _LL3107: if(( unsigned int) _temp3097 >  3u?*((
int*) _temp3097) ==  Cyc_Absyn_DoubleType: 0){ goto _LL3108;} else{ goto _LL3109;}
_LL3109: goto _LL3110; _LL3100: goto _LL3098; _LL3102:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp3119=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3119[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3120; _temp3120.tag= Cyc_Absyn_Const_e; _temp3120.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp3121=( struct Cyc_Absyn_Char_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp3121[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp3122; _temp3122.tag= Cyc_Absyn_Char_c; _temp3122.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3122.f2='\000'; _temp3122;}); _temp3121;})); _temp3120;}); _temp3119;})));
goto _LL3098; _LL3104:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp3123=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3123[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3124; _temp3124.tag= Cyc_Absyn_Const_e;
_temp3124.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp3125=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp3125[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp3126; _temp3126.tag= Cyc_Absyn_Int_c;
_temp3126.f1=( void*) _temp3117; _temp3126.f2= 0; _temp3126;}); _temp3125;}));
_temp3124;}); _temp3123;}))); if( _temp3115 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp3127=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp3127[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp3128; _temp3128.tag= Cyc_Absyn_Cast_e; _temp3128.f1=( void*) t; _temp3128.f2=
e; _temp3128;}); _temp3127;}), loc);} goto _LL3098; _LL3106:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3129=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3129[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3130; _temp3130.tag= Cyc_Absyn_Const_e; _temp3130.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3131=( struct Cyc_Absyn_Float_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp3131[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp3132; _temp3132.tag= Cyc_Absyn_Float_c; _temp3132.f1=
_tag_arr("0.0", sizeof( unsigned char), 4u); _temp3132;}); _temp3131;}));
_temp3130;}); _temp3129;}))); goto _LL3098; _LL3108:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp3133=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp3133[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp3134; _temp3134.tag= Cyc_Absyn_Cast_e; _temp3134.f1=( void*) t; _temp3134.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3135=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3135[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3136; _temp3136.tag= Cyc_Absyn_Const_e;
_temp3136.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3137=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp3137[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp3138; _temp3138.tag= Cyc_Absyn_Float_c;
_temp3138.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp3138;});
_temp3137;})); _temp3136;}); _temp3135;}), loc); _temp3134;}); _temp3133;})));
goto _LL3098; _LL3110:({ struct Cyc_Std_String_pa_struct _temp3140; _temp3140.tag=
Cyc_Std_String_pa; _temp3140.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp3139[ 1u]={& _temp3140}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp3139, sizeof( void*), 1u));}});
goto _LL3098; _LL3098:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp3143=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp3143->f1= tv; _temp3143->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3144=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3144->v=( void*) k; _temp3144;}),({ struct Cyc_Core_Opt*
_temp3145=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3145->v=( void*) s; _temp3145;})); _temp3143;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp3148;
struct _RegionHandle* _temp3149; struct Cyc_List_List* _temp3151; struct _tuple6*
_temp3146= env; _temp3148=* _temp3146; _LL3152: _temp3151= _temp3148.f1; goto
_LL3150; _LL3150: _temp3149= _temp3148.f2; goto _LL3147; _LL3147: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp3153=( struct _tuple5*) _region_malloc(
_temp3149, sizeof( struct _tuple5)); _temp3153->f1= tv; _temp3153->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3154=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3154->v=( void*) k; _temp3154;}),({ struct Cyc_Core_Opt*
_temp3155=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3155->v=( void*) _temp3151; _temp3155;})); _temp3153;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if( Cyc_Std_zstrptrcmp((( struct
Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind; struct Cyc_Absyn_Conref* k2= tv->kind;
if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({ struct Cyc_Std_String_pa_struct
_temp3159; _temp3159.tag= Cyc_Std_String_pa; _temp3159.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct _temp3158;
_temp3158.tag= Cyc_Std_String_pa; _temp3158.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
k1);{ struct Cyc_Std_String_pa_struct _temp3157; _temp3157.tag= Cyc_Std_String_pa;
_temp3157.f1=( struct _tagged_arr)* tv->name;{ void* _temp3156[ 3u]={& _temp3157,&
_temp3158,& _temp3159}; Cyc_Tcutil_terr( loc, _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u), _tag_arr( _temp3156, sizeof( void*), 3u));}}}});}
if( tv->identity ==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) != *(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity))){({ void* _temp3160[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp3160, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp3161=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3161->hd=( void*) tv; _temp3161->tl= tvs; _temp3161;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp3162[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp3162, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity ==  0){({ void* _temp3163[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u),
_tag_arr( _temp3163, sizeof( void*), 0u));});} if(*(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp3164=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3164->hd=( void*) tv; _temp3164->tl=
tvs; _temp3164;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp3166; _temp3166.tag= Cyc_Std_String_pa;
_temp3166.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp3165[
1u]={& _temp3166};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp3165, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp3167=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3167->hd=(
void*) tv; _temp3167->tl= tvs; _temp3167;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp3168= Cyc_Tcutil_compress( e);
int _temp3174; _LL3170: if(( unsigned int) _temp3168 >  3u?*(( int*) _temp3168)
==  Cyc_Absyn_Evar: 0){ _LL3175: _temp3174=(( struct Cyc_Absyn_Evar_struct*)
_temp3168)->f3; goto _LL3171;} else{ goto _LL3172;} _LL3172: goto _LL3173;
_LL3171:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2= es2->tl){ void*
_temp3176= Cyc_Tcutil_compress(( void*) es2->hd); int _temp3182; _LL3178: if((
unsigned int) _temp3176 >  3u?*(( int*) _temp3176) ==  Cyc_Absyn_Evar: 0){
_LL3183: _temp3182=(( struct Cyc_Absyn_Evar_struct*) _temp3176)->f3; goto
_LL3179;} else{ goto _LL3180;} _LL3180: goto _LL3181; _LL3179: if( _temp3174 == 
_temp3182){ return es;} goto _LL3177; _LL3181: goto _LL3177; _LL3177:;}} return({
struct Cyc_List_List* _temp3184=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3184->hd=( void*) e; _temp3184->tl= es; _temp3184;});
_LL3173: return es; _LL3169:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs= tvs->tl){ int present= 0;{ struct Cyc_List_List* b=
btvs; for( 0; b !=  0; b= b->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)
tvs->hd)->identity)) == *(( int*) _check_null((( struct Cyc_Absyn_Tvar*) b->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp3185=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3185->hd=(
void*)(( struct Cyc_Absyn_Tvar*) tvs->hd); _temp3185->tl= r; _temp3185;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp3187; _temp3187.tag= Cyc_Std_String_pa; _temp3187.f1=( struct _tagged_arr)*
fn;{ void* _temp3186[ 1u]={& _temp3187}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp3186, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp3188= Cyc_Tcutil_compress( field_typ); void* _temp3194;
_LL3190: if(( unsigned int) _temp3188 >  3u?*(( int*) _temp3188) ==  Cyc_Absyn_IntType:
0){ _LL3195: _temp3194=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3188)->f2;
goto _LL3191;} else{ goto _LL3192;} _LL3192: goto _LL3193; _LL3191:{ void*
_temp3196= _temp3194; _LL3198: if( _temp3196 == ( void*) Cyc_Absyn_B1){ goto
_LL3199;} else{ goto _LL3200;} _LL3200: if( _temp3196 == ( void*) Cyc_Absyn_B2){
goto _LL3201;} else{ goto _LL3202;} _LL3202: if( _temp3196 == ( void*) Cyc_Absyn_B4){
goto _LL3203;} else{ goto _LL3204;} _LL3204: if( _temp3196 == ( void*) Cyc_Absyn_B8){
goto _LL3205;} else{ goto _LL3197;} _LL3199: if( w >  8){({ void* _temp3206[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp3206, sizeof( void*), 0u));});} goto
_LL3197; _LL3201: if( w >  16){({ void* _temp3207[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp3207, sizeof( void*), 0u));});} goto _LL3197; _LL3203: if( w >  32){({ void*
_temp3208[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp3208, sizeof( void*), 0u));});}
goto _LL3197; _LL3205: if( w >  64){({ void* _temp3209[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp3209, sizeof( void*), 0u));});} goto _LL3197; _LL3197:;} goto
_LL3189; _LL3193:({ struct Cyc_Std_String_pa_struct _temp3212; _temp3212.tag=
Cyc_Std_String_pa; _temp3212.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp3211; _temp3211.tag= Cyc_Std_String_pa;
_temp3211.f1=( struct _tagged_arr)* fn;{ void* _temp3210[ 2u]={& _temp3211,&
_temp3212}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3210, sizeof( void*), 2u));}}});
goto _LL3189; _LL3189:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts= atts->tl){ void* _temp3213=( void*) atts->hd; _LL3215: if( _temp3213 == (
void*) Cyc_Absyn_Packed_att){ goto _LL3216;} else{ goto _LL3217;} _LL3217: if((
unsigned int) _temp3213 >  16u?*(( int*) _temp3213) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL3218;} else{ goto _LL3219;} _LL3219: goto _LL3220; _LL3216: continue;
_LL3218: continue; _LL3220:({ struct Cyc_Std_String_pa_struct _temp3223;
_temp3223.tag= Cyc_Std_String_pa; _temp3223.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp3222; _temp3222.tag= Cyc_Std_String_pa;
_temp3222.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp3221[ 2u]={& _temp3222,& _temp3223}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp3221, sizeof( void*), 2u));}}});
goto _LL3214; _LL3214:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; struct _tuple17{ void* f1; int f2; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp3224= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3274; struct Cyc_Core_Opt** _temp3276; struct Cyc_Core_Opt* _temp3277;
struct Cyc_Core_Opt** _temp3279; struct Cyc_Absyn_Tvar* _temp3280; struct Cyc_List_List*
_temp3282; struct Cyc_Absyn_Enumdecl* _temp3284; struct Cyc_Absyn_Enumdecl**
_temp3286; struct _tuple1* _temp3287; struct Cyc_Absyn_TunionInfo _temp3289;
void* _temp3291; struct Cyc_List_List* _temp3293; void* _temp3295; void**
_temp3297; struct Cyc_Absyn_TunionFieldInfo _temp3298; struct Cyc_List_List*
_temp3300; void* _temp3302; void** _temp3304; struct Cyc_Absyn_PtrInfo _temp3305;
struct Cyc_Absyn_Conref* _temp3307; struct Cyc_Absyn_Tqual _temp3309; struct Cyc_Absyn_Conref*
_temp3311; void* _temp3313; void* _temp3315; struct Cyc_Absyn_Exp* _temp3317;
struct Cyc_Absyn_Tqual _temp3319; void* _temp3321; struct Cyc_Absyn_FnInfo
_temp3323; struct Cyc_List_List* _temp3325; struct Cyc_List_List* _temp3327;
struct Cyc_Absyn_VarargInfo* _temp3329; int _temp3331; struct Cyc_List_List*
_temp3333; void* _temp3335; struct Cyc_Core_Opt* _temp3337; struct Cyc_Core_Opt**
_temp3339; struct Cyc_List_List* _temp3340; struct Cyc_List_List** _temp3342;
struct Cyc_List_List* _temp3343; struct Cyc_List_List* _temp3345; struct Cyc_List_List*
_temp3347; struct Cyc_Absyn_Structdecl** _temp3349; struct Cyc_Absyn_Structdecl***
_temp3351; struct Cyc_List_List* _temp3352; struct _tuple1* _temp3354; struct
Cyc_Absyn_Uniondecl** _temp3356; struct Cyc_Absyn_Uniondecl*** _temp3358; struct
Cyc_List_List* _temp3359; struct _tuple1* _temp3361; struct Cyc_Core_Opt*
_temp3363; struct Cyc_Core_Opt** _temp3365; struct Cyc_List_List* _temp3366;
struct Cyc_List_List** _temp3368; struct _tuple1* _temp3369; void* _temp3371;
void* _temp3373; void* _temp3375; struct Cyc_List_List* _temp3377; _LL3226: if(
_temp3224 == ( void*) Cyc_Absyn_VoidType){ goto _LL3227;} else{ goto _LL3228;}
_LL3228: if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_Evar:
0){ _LL3278: _temp3277=(( struct Cyc_Absyn_Evar_struct*) _temp3224)->f1;
_temp3279=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3224)->f1;
goto _LL3275; _LL3275: _temp3274=(( struct Cyc_Absyn_Evar_struct*) _temp3224)->f2;
_temp3276=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3224)->f2;
goto _LL3229;} else{ goto _LL3230;} _LL3230: if(( unsigned int) _temp3224 >  3u?*((
int*) _temp3224) ==  Cyc_Absyn_VarType: 0){ _LL3281: _temp3280=(( struct Cyc_Absyn_VarType_struct*)
_temp3224)->f1; goto _LL3231;} else{ goto _LL3232;} _LL3232: if(( unsigned int)
_temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_AnonEnumType: 0){ _LL3283:
_temp3282=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp3224)->f1; goto _LL3233;}
else{ goto _LL3234;} _LL3234: if(( unsigned int) _temp3224 >  3u?*(( int*)
_temp3224) ==  Cyc_Absyn_EnumType: 0){ _LL3288: _temp3287=(( struct Cyc_Absyn_EnumType_struct*)
_temp3224)->f1; goto _LL3285; _LL3285: _temp3284=(( struct Cyc_Absyn_EnumType_struct*)
_temp3224)->f2; _temp3286=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp3224)->f2; goto _LL3235;} else{ goto _LL3236;} _LL3236: if(( unsigned int)
_temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_TunionType: 0){ _LL3290:
_temp3289=(( struct Cyc_Absyn_TunionType_struct*) _temp3224)->f1; _LL3296:
_temp3295=( void*) _temp3289.tunion_info; _temp3297=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp3224)->f1).tunion_info; goto _LL3294; _LL3294: _temp3293= _temp3289.targs;
goto _LL3292; _LL3292: _temp3291=( void*) _temp3289.rgn; goto _LL3237;} else{
goto _LL3238;} _LL3238: if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224)
==  Cyc_Absyn_TunionFieldType: 0){ _LL3299: _temp3298=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp3224)->f1; _LL3303: _temp3302=( void*) _temp3298.field_info; _temp3304=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp3224)->f1).field_info;
goto _LL3301; _LL3301: _temp3300= _temp3298.targs; goto _LL3239;} else{ goto
_LL3240;} _LL3240: if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224) == 
Cyc_Absyn_PointerType: 0){ _LL3306: _temp3305=(( struct Cyc_Absyn_PointerType_struct*)
_temp3224)->f1; _LL3316: _temp3315=( void*) _temp3305.elt_typ; goto _LL3314;
_LL3314: _temp3313=( void*) _temp3305.rgn_typ; goto _LL3312; _LL3312: _temp3311=
_temp3305.nullable; goto _LL3310; _LL3310: _temp3309= _temp3305.tq; goto _LL3308;
_LL3308: _temp3307= _temp3305.bounds; goto _LL3241;} else{ goto _LL3242;}
_LL3242: if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_IntType:
0){ goto _LL3243;} else{ goto _LL3244;} _LL3244: if( _temp3224 == ( void*) Cyc_Absyn_FloatType){
goto _LL3245;} else{ goto _LL3246;} _LL3246: if(( unsigned int) _temp3224 >  3u?*((
int*) _temp3224) ==  Cyc_Absyn_DoubleType: 0){ goto _LL3247;} else{ goto _LL3248;}
_LL3248: if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_ArrayType:
0){ _LL3322: _temp3321=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3224)->f1;
goto _LL3320; _LL3320: _temp3319=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3224)->f2; goto _LL3318; _LL3318: _temp3317=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3224)->f3; goto _LL3249;} else{ goto _LL3250;} _LL3250: if(( unsigned int)
_temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_FnType: 0){ _LL3324:
_temp3323=(( struct Cyc_Absyn_FnType_struct*) _temp3224)->f1; _LL3341: _temp3340=
_temp3323.tvars; _temp3342=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3224)->f1).tvars; goto _LL3338; _LL3338: _temp3337= _temp3323.effect;
_temp3339=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3224)->f1).effect; goto _LL3336; _LL3336: _temp3335=( void*) _temp3323.ret_typ;
goto _LL3334; _LL3334: _temp3333= _temp3323.args; goto _LL3332; _LL3332:
_temp3331= _temp3323.c_varargs; goto _LL3330; _LL3330: _temp3329= _temp3323.cyc_varargs;
goto _LL3328; _LL3328: _temp3327= _temp3323.rgn_po; goto _LL3326; _LL3326:
_temp3325= _temp3323.attributes; goto _LL3251;} else{ goto _LL3252;} _LL3252:
if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_TupleType:
0){ _LL3344: _temp3343=(( struct Cyc_Absyn_TupleType_struct*) _temp3224)->f1;
goto _LL3253;} else{ goto _LL3254;} _LL3254: if(( unsigned int) _temp3224 >  3u?*((
int*) _temp3224) ==  Cyc_Absyn_AnonStructType: 0){ _LL3346: _temp3345=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3224)->f1; goto _LL3255;} else{ goto
_LL3256;} _LL3256: if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224) == 
Cyc_Absyn_AnonUnionType: 0){ _LL3348: _temp3347=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3224)->f1; goto _LL3257;} else{ goto _LL3258;} _LL3258: if(( unsigned int)
_temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_StructType: 0){ _LL3355:
_temp3354=(( struct Cyc_Absyn_StructType_struct*) _temp3224)->f1; goto _LL3353;
_LL3353: _temp3352=(( struct Cyc_Absyn_StructType_struct*) _temp3224)->f2; goto
_LL3350; _LL3350: _temp3349=(( struct Cyc_Absyn_StructType_struct*) _temp3224)->f3;
_temp3351=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp3224)->f3; goto _LL3259;} else{ goto _LL3260;} _LL3260: if(( unsigned int)
_temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_UnionType: 0){ _LL3362:
_temp3361=(( struct Cyc_Absyn_UnionType_struct*) _temp3224)->f1; goto _LL3360;
_LL3360: _temp3359=(( struct Cyc_Absyn_UnionType_struct*) _temp3224)->f2; goto
_LL3357; _LL3357: _temp3356=(( struct Cyc_Absyn_UnionType_struct*) _temp3224)->f3;
_temp3358=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp3224)->f3; goto _LL3261;} else{ goto _LL3262;} _LL3262: if(( unsigned int)
_temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_TypedefType: 0){ _LL3370:
_temp3369=(( struct Cyc_Absyn_TypedefType_struct*) _temp3224)->f1; goto _LL3367;
_LL3367: _temp3366=(( struct Cyc_Absyn_TypedefType_struct*) _temp3224)->f2;
_temp3368=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3224)->f2; goto _LL3364; _LL3364: _temp3363=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3224)->f3; _temp3365=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3224)->f3; goto _LL3263;} else{ goto _LL3264;} _LL3264: if( _temp3224 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL3265;} else{ goto _LL3266;} _LL3266: if((
unsigned int) _temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_RgnHandleType:
0){ _LL3372: _temp3371=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp3224)->f1; goto _LL3267;} else{ goto _LL3268;} _LL3268: if(( unsigned int)
_temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_AccessEff: 0){ _LL3374:
_temp3373=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp3224)->f1; goto
_LL3269;} else{ goto _LL3270;} _LL3270: if(( unsigned int) _temp3224 >  3u?*((
int*) _temp3224) ==  Cyc_Absyn_RgnsEff: 0){ _LL3376: _temp3375=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp3224)->f1; goto _LL3271;} else{ goto _LL3272;}
_LL3272: if(( unsigned int) _temp3224 >  3u?*(( int*) _temp3224) ==  Cyc_Absyn_JoinEff:
0){ _LL3378: _temp3377=(( struct Cyc_Absyn_JoinEff_struct*) _temp3224)->f1; goto
_LL3273;} else{ goto _LL3225;} _LL3227: goto _LL3225; _LL3229: if(* _temp3279 == 
0){* _temp3279=({ struct Cyc_Core_Opt* _temp3379=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3379->v=( void*) expected_kind;
_temp3379;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp3276=({ struct Cyc_Core_Opt* _temp3380=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3380->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp3380;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp3381= Cyc_Tcutil_new_tvar( expected_kind);*
_temp3276=({ struct Cyc_Core_Opt* _temp3382=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3382->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3383=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3383[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3384; _temp3384.tag= Cyc_Absyn_VarType;
_temp3384.f1= _temp3381; _temp3384;}); _temp3383;})); _temp3382;}); _temp3280=
_temp3381; goto _LL3231;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL3225; _LL3231: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp3280->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3385=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3385[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3386; _temp3386.tag= Cyc_Absyn_Eq_constr;
_temp3386.f1=( void*) expected_kind; _temp3386;}); _temp3385;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp3280); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp3280); goto _LL3225;}
_LL3233: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);{ struct
_RegionHandle _temp3387= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3387; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; for( 0; _temp3282 !=  0; _temp3282= _temp3282->tl){
struct Cyc_Absyn_Enumfield* _temp3388=( struct Cyc_Absyn_Enumfield*) _temp3282->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp,
prev_fields,(* _temp3388->name).f2)){({ struct Cyc_Std_String_pa_struct
_temp3390; _temp3390.tag= Cyc_Std_String_pa; _temp3390.f1=( struct _tagged_arr)*(*
_temp3388->name).f2;{ void* _temp3389[ 1u]={& _temp3390}; Cyc_Tcutil_terr(
_temp3388->loc, _tag_arr("duplicate enum field name %s", sizeof( unsigned char),
29u), _tag_arr( _temp3389, sizeof( void*), 1u));}});} else{ prev_fields=({
struct Cyc_List_List* _temp3391=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp3391->hd=( void*)(* _temp3388->name).f2;
_temp3391->tl= prev_fields; _temp3391;});} if( _temp3388->tag ==  0){ _temp3388->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp3388->loc);} else{
if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null(
_temp3388->tag))){({ struct Cyc_Std_String_pa_struct _temp3393; _temp3393.tag=
Cyc_Std_String_pa; _temp3393.f1=( struct _tagged_arr)*(* _temp3388->name).f2;{
void* _temp3392[ 1u]={& _temp3393}; Cyc_Tcutil_terr( loc, _tag_arr("enum field %s: expression is not constant",
sizeof( unsigned char), 42u), _tag_arr( _temp3392, sizeof( void*), 1u));}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp3388->tag)); tag_count= t1 +  1;(* _temp3388->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp3394=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp3394[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp3395; _temp3395.tag= Cyc_Absyn_Abs_n; _temp3395.f1= te->ns; _temp3395;});
_temp3394;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple17* v)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp3388->name).f2,({ struct _tuple17* _temp3396=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp3396->f1=( void*)({ struct Cyc_Tcenv_AnonEnumRes_struct*
_temp3397=( struct Cyc_Tcenv_AnonEnumRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_AnonEnumRes_struct));
_temp3397[ 0]=({ struct Cyc_Tcenv_AnonEnumRes_struct _temp3398; _temp3398.tag=
Cyc_Tcenv_AnonEnumRes; _temp3398.f1=( void*) t; _temp3398.f2= _temp3388;
_temp3398;}); _temp3397;}); _temp3396->f2= 1; _temp3396;}));}}}; _pop_region(
uprev_rgn);} goto _LL3225;} _LL3235: if(* _temp3286 ==  0){ struct _handler_cons
_temp3399; _push_handler(& _temp3399);{ int _temp3401= 0; if( setjmp( _temp3399.handler)){
_temp3401= 1;} if( ! _temp3401){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3287);* _temp3286=( struct Cyc_Absyn_Enumdecl*)* ed;};
_pop_handler();} else{ void* _temp3400=( void*) _exn_thrown; void* _temp3403=
_temp3400; _LL3405: if( _temp3403 ==  Cyc_Dict_Absent){ goto _LL3406;} else{
goto _LL3407;} _LL3407: goto _LL3408; _LL3406: { struct Cyc_Tcenv_Genv*
_temp3409=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Enumdecl* _temp3410=({
struct Cyc_Absyn_Enumdecl* _temp3411=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp3411->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp3411->name= _temp3287; _temp3411->fields= 0; _temp3411;}); Cyc_Tc_tcEnumdecl(
te, _temp3409, loc, _temp3410);{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3287);* _temp3286=( struct Cyc_Absyn_Enumdecl*)* ed; goto _LL3404;}}
_LL3408:( void) _throw( _temp3403); _LL3404:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp3286);* _temp3287=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL3225;} _LL3237:{ void*
_temp3412=* _temp3297; struct Cyc_Absyn_UnknownTunionInfo _temp3418; int
_temp3420; struct _tuple1* _temp3422; struct Cyc_Absyn_Tuniondecl** _temp3424;
struct Cyc_Absyn_Tuniondecl* _temp3426; _LL3414: if(*(( int*) _temp3412) ==  Cyc_Absyn_UnknownTunion){
_LL3419: _temp3418=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp3412)->f1;
_LL3423: _temp3422= _temp3418.name; goto _LL3421; _LL3421: _temp3420= _temp3418.is_xtunion;
goto _LL3415;} else{ goto _LL3416;} _LL3416: if(*(( int*) _temp3412) ==  Cyc_Absyn_KnownTunion){
_LL3425: _temp3424=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3412)->f1;
_temp3426=* _temp3424; goto _LL3417;} else{ goto _LL3413;} _LL3415: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp3427; _push_handler(& _temp3427);{ int
_temp3429= 0; if( setjmp( _temp3427.handler)){ _temp3429= 1;} if( ! _temp3429){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp3422);; _pop_handler();} else{
void* _temp3428=( void*) _exn_thrown; void* _temp3431= _temp3428; _LL3433: if(
_temp3431 ==  Cyc_Dict_Absent){ goto _LL3434;} else{ goto _LL3435;} _LL3435:
goto _LL3436; _LL3434: { struct Cyc_Tcenv_Genv* _temp3437=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Tuniondecl* _temp3438=({ struct Cyc_Absyn_Tuniondecl* _temp3442=(
struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp3442->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3442->name= _temp3422;
_temp3442->tvs= 0; _temp3442->fields= 0; _temp3442->is_xtunion= _temp3420;
_temp3442;}); Cyc_Tc_tcTuniondecl( te, _temp3437, loc, _temp3438); tudp= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp3422); if( _temp3293 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3441; _temp3441.tag= Cyc_Std_String_pa; _temp3441.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3422);{ struct Cyc_Std_String_pa_struct _temp3440;
_temp3440.tag= Cyc_Std_String_pa; _temp3440.f1=( struct _tagged_arr)( _temp3420?
_tag_arr("xtunion", sizeof( unsigned char), 8u): _tag_arr("tunion", sizeof(
unsigned char), 7u));{ void* _temp3439[ 2u]={& _temp3440,& _temp3441}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized %s %s before using", sizeof(
unsigned char), 48u), _tag_arr( _temp3439, sizeof( void*), 2u));}}}); return
cvtenv;} goto _LL3432;} _LL3436:( void) _throw( _temp3431); _LL3432:;}}} if((*
tudp)->is_xtunion !=  _temp3420){({ struct Cyc_Std_String_pa_struct _temp3444;
_temp3444.tag= Cyc_Std_String_pa; _temp3444.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3422);{ void* _temp3443[ 1u]={& _temp3444}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp3443, sizeof( void*), 1u));}});}*
_temp3297=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp3445=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp3445[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp3446; _temp3446.tag= Cyc_Absyn_KnownTunion;
_temp3446.f1= tudp; _temp3446;}); _temp3445;}); _temp3426=* tudp; goto _LL3417;}
_LL3417: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3291);{ struct Cyc_List_List* tvs= _temp3426->tvs; for( 0; _temp3293 !=  0?
tvs !=  0: 0;( _temp3293= _temp3293->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3293->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*) tvs->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp3293
!=  0){({ struct Cyc_Std_String_pa_struct _temp3448; _temp3448.tag= Cyc_Std_String_pa;
_temp3448.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3426->name);{
void* _temp3447[ 1u]={& _temp3448}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp3447, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp3450; _temp3450.tag= Cyc_Std_String_pa;
_temp3450.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3426->name);{
void* _temp3449[ 1u]={& _temp3450}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp3449, sizeof( void*), 1u));}});}
goto _LL3413;} _LL3413:;} goto _LL3225; _LL3239:{ void* _temp3451=* _temp3304;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp3457; int _temp3459; struct _tuple1*
_temp3461; struct _tuple1* _temp3463; struct Cyc_Absyn_Tunionfield* _temp3465;
struct Cyc_Absyn_Tuniondecl* _temp3467; _LL3453: if(*(( int*) _temp3451) ==  Cyc_Absyn_UnknownTunionfield){
_LL3458: _temp3457=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp3451)->f1;
_LL3464: _temp3463= _temp3457.tunion_name; goto _LL3462; _LL3462: _temp3461=
_temp3457.field_name; goto _LL3460; _LL3460: _temp3459= _temp3457.is_xtunion;
goto _LL3454;} else{ goto _LL3455;} _LL3455: if(*(( int*) _temp3451) ==  Cyc_Absyn_KnownTunionfield){
_LL3468: _temp3467=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp3451)->f1;
goto _LL3466; _LL3466: _temp3465=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp3451)->f2; goto _LL3456;} else{ goto _LL3452;} _LL3454: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp3469;
_push_handler(& _temp3469);{ int _temp3471= 0; if( setjmp( _temp3469.handler)){
_temp3471= 1;} if( ! _temp3471){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp3463);; _pop_handler();} else{ void* _temp3470=( void*) _exn_thrown; void*
_temp3473= _temp3470; _LL3475: if( _temp3473 ==  Cyc_Dict_Absent){ goto _LL3476;}
else{ goto _LL3477;} _LL3477: goto _LL3478; _LL3476:({ struct Cyc_Std_String_pa_struct
_temp3480; _temp3480.tag= Cyc_Std_String_pa; _temp3480.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3463);{ void* _temp3479[ 1u]={& _temp3480}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp3479, sizeof( void*), 1u));}}); return cvtenv; _LL3478:( void) _throw(
_temp3473); _LL3474:;}}}{ struct _handler_cons _temp3481; _push_handler(&
_temp3481);{ int _temp3483= 0; if( setjmp( _temp3481.handler)){ _temp3483= 1;}
if( ! _temp3483){{ void* _temp3484= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp3461); struct Cyc_Absyn_Tunionfield* _temp3490; struct Cyc_Absyn_Tuniondecl*
_temp3492; _LL3486: if(*(( int*) _temp3484) ==  Cyc_Tcenv_TunionRes){ _LL3493:
_temp3492=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3484)->f1; goto _LL3491;
_LL3491: _temp3490=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3484)->f2; goto
_LL3487;} else{ goto _LL3488;} _LL3488: goto _LL3489; _LL3487: tuf= _temp3490;
tud= _temp3492; if( tud->is_xtunion !=  _temp3459){({ struct Cyc_Std_String_pa_struct
_temp3495; _temp3495.tag= Cyc_Std_String_pa; _temp3495.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3463);{ void* _temp3494[ 1u]={& _temp3495}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp3494, sizeof( void*), 1u));}});} goto
_LL3485; _LL3489:({ struct Cyc_Std_String_pa_struct _temp3498; _temp3498.tag=
Cyc_Std_String_pa; _temp3498.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3463);{ struct Cyc_Std_String_pa_struct _temp3497; _temp3497.tag= Cyc_Std_String_pa;
_temp3497.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3461);{ void*
_temp3496[ 2u]={& _temp3497,& _temp3498}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp3496, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp3499= cvtenv; _npop_handler( 0u); return _temp3499;}
_LL3485:;}; _pop_handler();} else{ void* _temp3482=( void*) _exn_thrown; void*
_temp3501= _temp3482; _LL3503: if( _temp3501 ==  Cyc_Dict_Absent){ goto _LL3504;}
else{ goto _LL3505;} _LL3505: goto _LL3506; _LL3504:({ struct Cyc_Std_String_pa_struct
_temp3509; _temp3509.tag= Cyc_Std_String_pa; _temp3509.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3463);{ struct Cyc_Std_String_pa_struct _temp3508;
_temp3508.tag= Cyc_Std_String_pa; _temp3508.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3461);{ void* _temp3507[ 2u]={& _temp3508,& _temp3509}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3507, sizeof( void*), 2u));}}}); return cvtenv; _LL3506:( void)
_throw( _temp3501); _LL3502:;}}}* _temp3304=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp3510=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp3510[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp3511; _temp3511.tag= Cyc_Absyn_KnownTunionfield; _temp3511.f1= tud;
_temp3511.f2= tuf; _temp3511;}); _temp3510;}); _temp3467= tud; _temp3465= tuf;
goto _LL3456;} _LL3456: { struct Cyc_List_List* tvs= _temp3467->tvs; for( 0;
_temp3300 !=  0? tvs !=  0: 0;( _temp3300= _temp3300->tl, tvs= tvs->tl)){ void*
t1=( void*) _temp3300->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)
tvs->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);}
if( _temp3300 !=  0){({ struct Cyc_Std_String_pa_struct _temp3514; _temp3514.tag=
Cyc_Std_String_pa; _temp3514.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3465->name);{ struct Cyc_Std_String_pa_struct _temp3513; _temp3513.tag= Cyc_Std_String_pa;
_temp3513.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3467->name);{
void* _temp3512[ 2u]={& _temp3513,& _temp3514}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp3512, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp3517; _temp3517.tag= Cyc_Std_String_pa;
_temp3517.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3465->name);{
struct Cyc_Std_String_pa_struct _temp3516; _temp3516.tag= Cyc_Std_String_pa;
_temp3516.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3467->name);{
void* _temp3515[ 2u]={& _temp3516,& _temp3517}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp3515, sizeof( void*), 2u));}}});}
goto _LL3452;} _LL3452:;} goto _LL3225; _LL3241: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3315); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp3313);{ void* _temp3518=( void*)(
Cyc_Absyn_compress_conref( _temp3307))->v; void* _temp3524; struct Cyc_Absyn_Exp*
_temp3526; _LL3520: if(( unsigned int) _temp3518 >  1u?*(( int*) _temp3518) == 
Cyc_Absyn_Eq_constr: 0){ _LL3525: _temp3524=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3518)->f1; if(( unsigned int) _temp3524 >  1u?*(( int*) _temp3524) ==  Cyc_Absyn_Upper_b:
0){ _LL3527: _temp3526=(( struct Cyc_Absyn_Upper_b_struct*) _temp3524)->f1; goto
_LL3521;} else{ goto _LL3522;}} else{ goto _LL3522;} _LL3522: goto _LL3523;
_LL3521: if( ! Cyc_Tcutil_is_const_exp( te, _temp3526)){({ void* _temp3528[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp3528, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp3526); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp3526)){({ void*
_temp3529[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp3529, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp3526); goto _LL3519; _LL3523: goto _LL3519; _LL3519:;} goto _LL3225;
_LL3243: goto _LL3225; _LL3245: goto _LL3225; _LL3247: goto _LL3225; _LL3249:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3321); if( _temp3317 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp3317))){({ void* _temp3530[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp3530, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp3317)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp3317))){({ void* _temp3531[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp3531, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp3317)); goto _LL3225; _LL3251: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp3325 !=  0; _temp3325= _temp3325->tl){ if(
! Cyc_Absyn_fntype_att(( void*) _temp3325->hd)){({ struct Cyc_Std_String_pa_struct
_temp3533; _temp3533.tag= Cyc_Std_String_pa; _temp3533.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*) _temp3325->hd);{ void* _temp3532[ 1u]={&
_temp3533}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp3532, sizeof( void*), 1u));}});}{
void* _temp3534=( void*) _temp3325->hd; int _temp3546; int _temp3548; void*
_temp3550; _LL3536: if( _temp3534 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL3537;} else{ goto _LL3538;} _LL3538: if( _temp3534 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL3539;} else{ goto _LL3540;} _LL3540: if( _temp3534 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL3541;} else{ goto _LL3542;} _LL3542: if(( unsigned int) _temp3534 >  16u?*((
int*) _temp3534) ==  Cyc_Absyn_Format_att: 0){ _LL3551: _temp3550=( void*)((
struct Cyc_Absyn_Format_att_struct*) _temp3534)->f1; goto _LL3549; _LL3549:
_temp3548=(( struct Cyc_Absyn_Format_att_struct*) _temp3534)->f2; goto _LL3547;
_LL3547: _temp3546=(( struct Cyc_Absyn_Format_att_struct*) _temp3534)->f3; goto
_LL3543;} else{ goto _LL3544;} _LL3544: goto _LL3545; _LL3537: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL3535; _LL3539: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL3535; _LL3541: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL3535; _LL3543: if( !
seen_format){ seen_format= 1; ft= _temp3550; fmt_desc_arg= _temp3548;
fmt_arg_start= _temp3546;} else{({ void* _temp3552[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp3552, sizeof( void*), 0u));});} goto _LL3535; _LL3545: goto
_LL3535; _LL3535:;}} if( num_convs >  1){({ void* _temp3553[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp3553, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp3342);{ struct Cyc_List_List* b=* _temp3342; for( 0; b !=  0; b= b->tl){((
struct Cyc_Absyn_Tvar*) b->hd)->identity= Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env=
Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*) b->hd);{
void* _temp3554=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*) b->hd)->kind))->v;
void* _temp3560; _LL3556: if(( unsigned int) _temp3554 >  1u?*(( int*) _temp3554)
==  Cyc_Absyn_Eq_constr: 0){ _LL3561: _temp3560=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3554)->f1; if( _temp3560 == ( void*) Cyc_Absyn_MemKind){ goto _LL3557;}
else{ goto _LL3558;}} else{ goto _LL3558;} _LL3558: goto _LL3559; _LL3557:({
struct Cyc_Std_String_pa_struct _temp3563; _temp3563.tag= Cyc_Std_String_pa;
_temp3563.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) b->hd)->name;{
void* _temp3562[ 1u]={& _temp3563}; Cyc_Tcutil_terr( loc, _tag_arr("function attempts to abstract Mem type variable %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3562, sizeof( void*), 1u));}});
goto _LL3555; _LL3559: goto _LL3555; _LL3555:;}}}{ struct Cyc_Tcutil_CVTEnv
_temp3564=({ struct Cyc_Tcutil_CVTEnv _temp3723; _temp3723.kind_env= cvtenv.kind_env;
_temp3723.free_vars= 0; _temp3723.free_evars= 0; _temp3723.generalize_evars=
cvtenv.generalize_evars; _temp3723.fn_result= 1; _temp3723;}); _temp3564= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3564,( void*) Cyc_Absyn_MemKind, _temp3335); _temp3564.fn_result=
0;{ struct Cyc_List_List* a= _temp3333; for( 0; a !=  0; a= a->tl){ _temp3564=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3564,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*) a->hd)).f3);}} if( _temp3329 !=  0){ if( _temp3331){({ void*
_temp3565[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp3565, sizeof( void*), 0u));});}{ int _temp3568; void* _temp3570; struct Cyc_Absyn_Tqual
_temp3572; struct Cyc_Core_Opt* _temp3574; struct Cyc_Absyn_VarargInfo _temp3566=*
_temp3329; _LL3575: _temp3574= _temp3566.name; goto _LL3573; _LL3573: _temp3572=
_temp3566.tq; goto _LL3571; _LL3571: _temp3570=( void*) _temp3566.type; goto
_LL3569; _LL3569: _temp3568= _temp3566.inject; goto _LL3567; _LL3567: _temp3564=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3564,( void*) Cyc_Absyn_MemKind,
_temp3570); if( _temp3568){ void* _temp3576= Cyc_Tcutil_compress( _temp3570);
struct Cyc_Absyn_TunionInfo _temp3582; void* _temp3584; void* _temp3586; _LL3578:
if(( unsigned int) _temp3576 >  3u?*(( int*) _temp3576) ==  Cyc_Absyn_TunionType:
0){ _LL3583: _temp3582=(( struct Cyc_Absyn_TunionType_struct*) _temp3576)->f1;
_LL3587: _temp3586=( void*) _temp3582.tunion_info; if(*(( int*) _temp3586) == 
Cyc_Absyn_KnownTunion){ goto _LL3585;} else{ goto _LL3580;} _LL3585: _temp3584=(
void*) _temp3582.rgn; goto _LL3579;} else{ goto _LL3580;} _LL3580: goto _LL3581;
_LL3579: goto _LL3577; _LL3581:({ void* _temp3588[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("can't inject a non-[x]tunion type", sizeof( unsigned char), 34u),
_tag_arr( _temp3588, sizeof( void*), 0u));}); goto _LL3577; _LL3577:;}}} if(
seen_format){ int _temp3589=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp3333); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg >  _temp3589)? 1:
fmt_arg_start <  0)? 1:( _temp3329 ==  0? fmt_arg_start !=  0: 0))? 1:(
_temp3329 !=  0? fmt_arg_start !=  _temp3589 +  1: 0)){({ void* _temp3590[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp3590, sizeof( void*), 0u));});} else{ void* _temp3593;
struct _tuple2 _temp3591=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp3333, fmt_desc_arg -  1); _LL3594: _temp3593= _temp3591.f3;
goto _LL3592; _LL3592:{ void* _temp3595= Cyc_Tcutil_compress( _temp3593); struct
Cyc_Absyn_PtrInfo _temp3601; struct Cyc_Absyn_Conref* _temp3603; void* _temp3605;
_LL3597: if(( unsigned int) _temp3595 >  3u?*(( int*) _temp3595) ==  Cyc_Absyn_PointerType:
0){ _LL3602: _temp3601=(( struct Cyc_Absyn_PointerType_struct*) _temp3595)->f1;
_LL3606: _temp3605=( void*) _temp3601.elt_typ; goto _LL3604; _LL3604: _temp3603=
_temp3601.bounds; goto _LL3598;} else{ goto _LL3599;} _LL3599: goto _LL3600;
_LL3598:{ struct _tuple8 _temp3608=({ struct _tuple8 _temp3607; _temp3607.f1=
Cyc_Tcutil_compress( _temp3605); _temp3607.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp3603); _temp3607;}); void* _temp3614; void* _temp3616; void* _temp3618;
void* _temp3620; _LL3610: _LL3617: _temp3616= _temp3608.f1; if(( unsigned int)
_temp3616 >  3u?*(( int*) _temp3616) ==  Cyc_Absyn_IntType: 0){ _LL3621:
_temp3620=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3616)->f1; if(
_temp3620 == ( void*) Cyc_Absyn_Unsigned){ goto _LL3619;} else{ goto _LL3612;}
_LL3619: _temp3618=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3616)->f2;
if( _temp3618 == ( void*) Cyc_Absyn_B1){ goto _LL3615;} else{ goto _LL3612;}}
else{ goto _LL3612;} _LL3615: _temp3614= _temp3608.f2; if( _temp3614 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL3611;} else{ goto _LL3612;} _LL3612: goto _LL3613;
_LL3611: goto _LL3609; _LL3613:({ void* _temp3622[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp3622, sizeof( void*), 0u));}); goto _LL3609; _LL3609:;} goto
_LL3596; _LL3600:({ void* _temp3623[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp3623, sizeof( void*), 0u));}); goto
_LL3596; _LL3596:;} if( fmt_arg_start !=  0){ void* _temp3624= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp3329))->type); int
problem;{ void* _temp3625= ft; _LL3627: if( _temp3625 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL3628;} else{ goto _LL3629;} _LL3629: if( _temp3625 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL3630;} else{ goto _LL3626;} _LL3628:{ void* _temp3631= _temp3624; struct
Cyc_Absyn_TunionInfo _temp3637; void* _temp3639; struct Cyc_Absyn_Tuniondecl**
_temp3641; struct Cyc_Absyn_Tuniondecl* _temp3643; _LL3633: if(( unsigned int)
_temp3631 >  3u?*(( int*) _temp3631) ==  Cyc_Absyn_TunionType: 0){ _LL3638:
_temp3637=(( struct Cyc_Absyn_TunionType_struct*) _temp3631)->f1; _LL3640:
_temp3639=( void*) _temp3637.tunion_info; if(*(( int*) _temp3639) ==  Cyc_Absyn_KnownTunion){
_LL3642: _temp3641=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3639)->f1;
_temp3643=* _temp3641; goto _LL3634;} else{ goto _LL3635;}} else{ goto _LL3635;}
_LL3635: goto _LL3636; _LL3634: problem= Cyc_Absyn_qvar_cmp( _temp3643->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL3632; _LL3636: problem= 1; goto
_LL3632; _LL3632:;} goto _LL3626; _LL3630:{ void* _temp3644= _temp3624; struct
Cyc_Absyn_TunionInfo _temp3650; void* _temp3652; struct Cyc_Absyn_Tuniondecl**
_temp3654; struct Cyc_Absyn_Tuniondecl* _temp3656; _LL3646: if(( unsigned int)
_temp3644 >  3u?*(( int*) _temp3644) ==  Cyc_Absyn_TunionType: 0){ _LL3651:
_temp3650=(( struct Cyc_Absyn_TunionType_struct*) _temp3644)->f1; _LL3653:
_temp3652=( void*) _temp3650.tunion_info; if(*(( int*) _temp3652) ==  Cyc_Absyn_KnownTunion){
_LL3655: _temp3654=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3652)->f1;
_temp3656=* _temp3654; goto _LL3647;} else{ goto _LL3648;}} else{ goto _LL3648;}
_LL3648: goto _LL3649; _LL3647: problem= Cyc_Absyn_qvar_cmp( _temp3656->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL3645; _LL3649: problem= 1; goto
_LL3645; _LL3645:;} goto _LL3626; _LL3626:;} if( problem){({ void* _temp3657[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3657, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp3327; for( 0; rpo !=  0; rpo= rpo->tl){ struct
_tuple8 _temp3660; void* _temp3661; void* _temp3663; struct _tuple8* _temp3658=(
struct _tuple8*) rpo->hd; _temp3660=* _temp3658; _LL3664: _temp3663= _temp3660.f1;
goto _LL3662; _LL3662: _temp3661= _temp3660.f2; goto _LL3659; _LL3659: _temp3564=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3564,( void*) Cyc_Absyn_RgnKind,
_temp3663); _temp3564= Cyc_Tcutil_i_check_valid_type( loc, te, _temp3564,( void*)
Cyc_Absyn_RgnKind, _temp3661);}} if(* _temp3339 !=  0){ _temp3564= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3564,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp3339))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp3564.free_vars; for(
0; tvs !=  0; tvs= tvs->tl){ void* _temp3665=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind))->v; void* _temp3673; void* _temp3675;
_LL3667: if(( unsigned int) _temp3665 >  1u?*(( int*) _temp3665) ==  Cyc_Absyn_Eq_constr:
0){ _LL3674: _temp3673=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3665)->f1;
if( _temp3673 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3668;} else{ goto _LL3669;}}
else{ goto _LL3669;} _LL3669: if(( unsigned int) _temp3665 >  1u?*(( int*)
_temp3665) ==  Cyc_Absyn_Eq_constr: 0){ _LL3676: _temp3675=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3665)->f1; if( _temp3675 == ( void*) Cyc_Absyn_EffKind){ goto _LL3670;}
else{ goto _LL3671;}} else{ goto _LL3671;} _LL3671: goto _LL3672; _LL3668:
effect=({ struct Cyc_List_List* _temp3677=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3677->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3678=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3678[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3679; _temp3679.tag= Cyc_Absyn_AccessEff;
_temp3679.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3680=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3680[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3681; _temp3681.tag= Cyc_Absyn_VarType;
_temp3681.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3681;}); _temp3680;}));
_temp3679;}); _temp3678;})); _temp3677->tl= effect; _temp3677;}); goto _LL3666;
_LL3670: effect=({ struct Cyc_List_List* _temp3682=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3682->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp3683=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3683[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3684; _temp3684.tag= Cyc_Absyn_VarType; _temp3684.f1=(
struct Cyc_Absyn_Tvar*) tvs->hd; _temp3684;}); _temp3683;})); _temp3682->tl=
effect; _temp3682;}); goto _LL3666; _LL3672: effect=({ struct Cyc_List_List*
_temp3685=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3685->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp3686=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3686[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3687; _temp3687.tag= Cyc_Absyn_RgnsEff;
_temp3687.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3688=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3688[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3689; _temp3689.tag= Cyc_Absyn_VarType;
_temp3689.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3689;}); _temp3688;}));
_temp3687;}); _temp3686;})); _temp3685->tl= effect; _temp3685;}); goto _LL3666;
_LL3666:;}} effect= Cyc_List_imp_rev( effect);{ struct Cyc_List_List* ts=
_temp3564.free_evars; for( 0; ts !=  0; ts= ts->tl){ void* _temp3690= Cyc_Tcutil_typ_kind((
void*) ts->hd); _LL3692: if( _temp3690 == ( void*) Cyc_Absyn_RgnKind){ goto
_LL3693;} else{ goto _LL3694;} _LL3694: if( _temp3690 == ( void*) Cyc_Absyn_EffKind){
goto _LL3695;} else{ goto _LL3696;} _LL3696: goto _LL3697; _LL3693: effect=({
struct Cyc_List_List* _temp3698=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3698->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3699=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3699[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3700; _temp3700.tag= Cyc_Absyn_AccessEff;
_temp3700.f1=( void*)(( void*) ts->hd); _temp3700;}); _temp3699;})); _temp3698->tl=
effect; _temp3698;}); goto _LL3691; _LL3695: effect=({ struct Cyc_List_List*
_temp3701=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3701->hd=( void*)(( void*) ts->hd); _temp3701->tl= effect; _temp3701;});
goto _LL3691; _LL3697: effect=({ struct Cyc_List_List* _temp3702=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3702->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3703=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3703[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3704; _temp3704.tag= Cyc_Absyn_RgnsEff; _temp3704.f1=(
void*)(( void*) ts->hd); _temp3704;}); _temp3703;})); _temp3702->tl= effect;
_temp3702;}); goto _LL3691; _LL3691:;}}* _temp3339=({ struct Cyc_Core_Opt*
_temp3705=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3705->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3706=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3706[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3707; _temp3707.tag= Cyc_Absyn_JoinEff;
_temp3707.f1= Cyc_List_imp_rev( effect); _temp3707;}); _temp3706;})); _temp3705;});}}
if(* _temp3342 !=  0){ struct Cyc_List_List* bs=* _temp3342; for( 0; bs !=  0;
bs= bs->tl){ struct Cyc_Absyn_Conref* _temp3708= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*) bs->hd)->kind); void* _temp3709=( void*) _temp3708->v;
void* _temp3717; _LL3711: if( _temp3709 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3712;} else{ goto _LL3713;} _LL3713: if(( unsigned int) _temp3709 >  1u?*((
int*) _temp3709) ==  Cyc_Absyn_Eq_constr: 0){ _LL3718: _temp3717=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3709)->f1; if( _temp3717 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3714;} else{ goto _LL3715;}} else{ goto _LL3715;}
_LL3715: goto _LL3716; _LL3712:({ struct Cyc_Std_String_pa_struct _temp3720;
_temp3720.tag= Cyc_Std_String_pa; _temp3720.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*) bs->hd)->name;{ void* _temp3719[ 1u]={& _temp3720}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3719, sizeof( void*), 1u));}}); goto
_LL3714; _LL3714:( void*)( _temp3708->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3721=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3721[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3722; _temp3722.tag= Cyc_Absyn_Eq_constr;
_temp3722.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3722;}); _temp3721;})));
goto _LL3710; _LL3716: goto _LL3710; _LL3710:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp3564.kind_env,* _temp3342); _temp3564.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp3564.free_vars,* _temp3342);{ struct Cyc_List_List* tvs= _temp3564.free_vars;
for( 0; tvs !=  0; tvs= tvs->tl){ cvtenv.free_vars= Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,( struct Cyc_Absyn_Tvar*) tvs->hd);}}{ struct Cyc_List_List*
evs= _temp3564.free_evars; for( 0; evs !=  0; evs= evs->tl){ cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars,( void*) evs->hd);}} goto _LL3225;}}
_LL3253: for( 0; _temp3343 !=  0; _temp3343= _temp3343->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp3343->hd)).f2);}
goto _LL3225; _LL3255:{ struct _RegionHandle _temp3724= _new_region(); struct
_RegionHandle* sprev_rgn=& _temp3724; _push_region( sprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp3345 !=  0; _temp3345= _temp3345->tl){ struct Cyc_Absyn_Structfield
_temp3727; struct Cyc_List_List* _temp3728; struct Cyc_Absyn_Exp* _temp3730;
void* _temp3732; struct Cyc_Absyn_Tqual _temp3734; struct _tagged_arr* _temp3736;
struct Cyc_Absyn_Structfield* _temp3725=( struct Cyc_Absyn_Structfield*)
_temp3345->hd; _temp3727=* _temp3725; _LL3737: _temp3736= _temp3727.name; goto
_LL3735; _LL3735: _temp3734= _temp3727.tq; goto _LL3733; _LL3733: _temp3732=(
void*) _temp3727.type; goto _LL3731; _LL3731: _temp3730= _temp3727.width; goto
_LL3729; _LL3729: _temp3728= _temp3727.attributes; goto _LL3726; _LL3726: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3736)){({ struct Cyc_Std_String_pa_struct _temp3739; _temp3739.tag= Cyc_Std_String_pa;
_temp3739.f1=( struct _tagged_arr)* _temp3736;{ void* _temp3738[ 1u]={&
_temp3739}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3738, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3736, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3740=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3740->hd=( void*)
_temp3736; _temp3740->tl= prev_fields; _temp3740;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3732); Cyc_Tcutil_check_bitfield(
loc, te, _temp3732, _temp3730, _temp3736); Cyc_Tcutil_check_field_atts( loc,
_temp3736, _temp3728);}}; _pop_region( sprev_rgn);} goto _LL3225; _LL3257:{
struct _RegionHandle _temp3741= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3741; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp3347 !=  0; _temp3347= _temp3347->tl){ struct Cyc_Absyn_Structfield
_temp3744; struct Cyc_List_List* _temp3745; struct Cyc_Absyn_Exp* _temp3747;
void* _temp3749; struct Cyc_Absyn_Tqual _temp3751; struct _tagged_arr* _temp3753;
struct Cyc_Absyn_Structfield* _temp3742=( struct Cyc_Absyn_Structfield*)
_temp3347->hd; _temp3744=* _temp3742; _LL3754: _temp3753= _temp3744.name; goto
_LL3752; _LL3752: _temp3751= _temp3744.tq; goto _LL3750; _LL3750: _temp3749=(
void*) _temp3744.type; goto _LL3748; _LL3748: _temp3747= _temp3744.width; goto
_LL3746; _LL3746: _temp3745= _temp3744.attributes; goto _LL3743; _LL3743: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3753)){({ struct Cyc_Std_String_pa_struct _temp3756; _temp3756.tag= Cyc_Std_String_pa;
_temp3756.f1=( struct _tagged_arr)* _temp3753;{ void* _temp3755[ 1u]={&
_temp3756}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3755, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3753, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3757=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3757->hd=( void*)
_temp3753; _temp3757->tl= prev_fields; _temp3757;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3749); if( ! Cyc_Tcutil_bits_only(
_temp3749)){({ struct Cyc_Std_String_pa_struct _temp3759; _temp3759.tag= Cyc_Std_String_pa;
_temp3759.f1=( struct _tagged_arr)* _temp3753;{ void* _temp3758[ 1u]={&
_temp3759}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3758, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3749, _temp3747, _temp3753); Cyc_Tcutil_check_field_atts(
loc, _temp3753, _temp3745);}}; _pop_region( uprev_rgn);} goto _LL3225; _LL3259:
if( _temp3354 ==  0){ if(* _temp3351 ==  0){({ void* _temp3760[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3760, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp3351));;}} if(*
_temp3351 ==  0){ struct _handler_cons _temp3761; _push_handler(& _temp3761);{
int _temp3763= 0; if( setjmp( _temp3761.handler)){ _temp3763= 1;} if( !
_temp3763){* _temp3351=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp3354));; _pop_handler();} else{
void* _temp3762=( void*) _exn_thrown; void* _temp3765= _temp3762; _LL3767: if(
_temp3765 ==  Cyc_Dict_Absent){ goto _LL3768;} else{ goto _LL3769;} _LL3769:
goto _LL3770; _LL3768: { struct _tuple1* tdn=( struct _tuple1*) _check_null(
_temp3354); struct Cyc_Tcenv_Genv* _temp3771=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Structdecl* _temp3772=({ struct Cyc_Absyn_Structdecl* _temp3775=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp3775->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3775->name=({ struct Cyc_Core_Opt*
_temp3776=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3776->v=( void*) tdn; _temp3776;}); _temp3775->tvs= 0; _temp3775->fields= 0;
_temp3775->attributes= 0; _temp3775;}); Cyc_Tc_tcStructdecl( te, _temp3771, loc,
_temp3772);* _temp3351=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc, tdn); if( _temp3352 !=  0){({ struct Cyc_Std_String_pa_struct _temp3774;
_temp3774.tag= Cyc_Std_String_pa; _temp3774.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tdn);{ void* _temp3773[ 1u]={& _temp3774}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized struct %s before using",
sizeof( unsigned char), 52u), _tag_arr( _temp3773, sizeof( void*), 1u));}});
return cvtenv;} goto _LL3766;} _LL3770:( void) _throw( _temp3765); _LL3766:;}}}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp3351)); if( sd->name !=  0){*(( struct _tuple1*) _check_null( _temp3354))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp3352); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3780; _temp3780.tag= Cyc_Std_Int_pa; _temp3780.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3779; _temp3779.tag= Cyc_Std_Int_pa;
_temp3779.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3778; _temp3778.tag= Cyc_Std_String_pa; _temp3778.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp3354));{ void*
_temp3777[ 3u]={& _temp3778,& _temp3779,& _temp3780}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3777, sizeof( void*), 3u));}}}});} for( 0;
_temp3352 !=  0;( _temp3352= _temp3352->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3352->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL3225;}} _LL3261: if( _temp3361 ==  0){({ void*
_temp3781[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("missing union name", sizeof(
unsigned char), 19u), _tag_arr( _temp3781, sizeof( void*), 0u));}); return
cvtenv;} if(* _temp3358 ==  0){ struct _handler_cons _temp3782; _push_handler(&
_temp3782);{ int _temp3784= 0; if( setjmp( _temp3782.handler)){ _temp3784= 1;}
if( ! _temp3784){* _temp3358=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp3361));; _pop_handler();} else{
void* _temp3783=( void*) _exn_thrown; void* _temp3786= _temp3783; _LL3788: if(
_temp3786 ==  Cyc_Dict_Absent){ goto _LL3789;} else{ goto _LL3790;} _LL3790:
goto _LL3791; _LL3789: { struct _tuple1* _temp3792=( struct _tuple1*)
_check_null( _temp3361); struct Cyc_Tcenv_Genv* _temp3793=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Uniondecl* _temp3794=({ struct Cyc_Absyn_Uniondecl* _temp3797=(
struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp3797->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3797->name=({ struct Cyc_Core_Opt*
_temp3798=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3798->v=( void*) _temp3792; _temp3798;}); _temp3797->tvs= 0; _temp3797->fields=
0; _temp3797->attributes= 0; _temp3797;}); Cyc_Tc_tcUniondecl( te, _temp3793,
loc, _temp3794);* _temp3358=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc, _temp3792); if( _temp3359 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3796; _temp3796.tag= Cyc_Std_String_pa; _temp3796.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3792);{ void* _temp3795[ 1u]={& _temp3796}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized union %s before using", sizeof(
unsigned char), 51u), _tag_arr( _temp3795, sizeof( void*), 1u));}}); return
cvtenv;} goto _LL3787;} _LL3791:( void) _throw( _temp3786); _LL3787:;}}}{ struct
Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp3358)); if( ud->name !=  0){* _temp3361=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp3359); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3802; _temp3802.tag= Cyc_Std_Int_pa; _temp3802.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3801; _temp3801.tag= Cyc_Std_Int_pa;
_temp3801.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3800; _temp3800.tag= Cyc_Std_String_pa; _temp3800.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp3361));{ void*
_temp3799[ 3u]={& _temp3800,& _temp3801,& _temp3802}; Cyc_Tcutil_terr( loc,
_tag_arr("union %s expects %d type arguments but was given %d", sizeof(
unsigned char), 52u), _tag_arr( _temp3799, sizeof( void*), 3u));}}}});} for( 0;
_temp3359 !=  0;( _temp3359= _temp3359->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3359->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL3225;}} _LL3263: { struct Cyc_List_List*
targs=* _temp3368; struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons
_temp3803; _push_handler(& _temp3803);{ int _temp3805= 0; if( setjmp( _temp3803.handler)){
_temp3805= 1;} if( ! _temp3805){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp3369);; _pop_handler();} else{ void* _temp3804=( void*) _exn_thrown; void*
_temp3807= _temp3804; _LL3809: if( _temp3807 ==  Cyc_Dict_Absent){ goto _LL3810;}
else{ goto _LL3811;} _LL3811: goto _LL3812; _LL3810:({ struct Cyc_Std_String_pa_struct
_temp3814; _temp3814.tag= Cyc_Std_String_pa; _temp3814.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3369);{ void* _temp3813[ 1u]={& _temp3814}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3813, sizeof( void*), 1u));}}); return cvtenv; _LL3812:( void) _throw(
_temp3807); _LL3808:;}}} _temp3369[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts= ts->tl, tvs= tvs->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp3815=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3815->hd=(
void*)({ struct _tuple5* _temp3816=( struct _tuple5*) _cycalloc( sizeof( struct
_tuple5)); _temp3816->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3816->f2=( void*)
ts->hd; _temp3816;}); _temp3815->tl= inst; _temp3815;});} if( ts !=  0){({
struct Cyc_Std_String_pa_struct _temp3818; _temp3818.tag= Cyc_Std_String_pa;
_temp3818.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3369);{ void*
_temp3817[ 1u]={& _temp3818}; Cyc_Tcutil_terr( loc, _tag_arr("too many parameters for typedef %s",
sizeof( unsigned char), 35u), _tag_arr( _temp3817, sizeof( void*), 1u));}});}
if( tvs !=  0){ struct Cyc_List_List* hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*) tvs->hd)->kind); void*
e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({ struct Cyc_List_List* _temp3819=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3819->hd=(
void*) e; _temp3819->tl= hidden_ts; _temp3819;}); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k, e); inst=({ struct Cyc_List_List* _temp3820=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3820->hd=( void*)({ struct
_tuple5* _temp3821=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3821->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3821->f2= e; _temp3821;});
_temp3820->tl= inst; _temp3820;});}* _temp3368= Cyc_List_imp_append( targs, Cyc_List_imp_rev(
hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);*
_temp3365=({ struct Cyc_Core_Opt* _temp3822=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3822->v=( void*) new_typ; _temp3822;}); goto
_LL3225;}}} _LL3265: goto _LL3225; _LL3267: _temp3373= _temp3371; goto _LL3269;
_LL3269: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3373); goto _LL3225; _LL3271: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3375); goto _LL3225; _LL3273: for( 0;
_temp3377 !=  0; _temp3377= _temp3377->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*) _temp3377->hd);} goto
_LL3225; _LL3225:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){({ struct Cyc_Std_String_pa_struct _temp3826; _temp3826.tag= Cyc_Std_String_pa;
_temp3826.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_Std_String_pa_struct _temp3825; _temp3825.tag= Cyc_Std_String_pa;
_temp3825.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_Std_String_pa_struct _temp3824; _temp3824.tag= Cyc_Std_String_pa;
_temp3824.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3823[
3u]={& _temp3824,& _temp3825,& _temp3826}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3823, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3827= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3842; _temp3842.kind_env= kind_env; _temp3842.free_vars= 0; _temp3842.free_evars=
0; _temp3842.generalize_evars= generalize_evars; _temp3842.fn_result= 0;
_temp3842;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3827.free_vars;
for( 0; vs !=  0; vs= vs->tl){ _temp3827.kind_env= Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*) vs->hd);}}{ struct Cyc_List_List* evs=
_temp3827.free_evars; for( 0; evs !=  0; evs= evs->tl){ void* _temp3828= Cyc_Tcutil_compress((
void*) evs->hd); struct Cyc_Core_Opt* _temp3834; struct Cyc_Core_Opt** _temp3836;
_LL3830: if(( unsigned int) _temp3828 >  3u?*(( int*) _temp3828) ==  Cyc_Absyn_Evar:
0){ _LL3835: _temp3834=(( struct Cyc_Absyn_Evar_struct*) _temp3828)->f4;
_temp3836=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3828)->f4;
goto _LL3831;} else{ goto _LL3832;} _LL3832: goto _LL3833; _LL3831: if(*
_temp3836 ==  0){* _temp3836=({ struct Cyc_Core_Opt* _temp3837=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3837->v=( void*) kind_env;
_temp3837;});} else{ struct Cyc_List_List* _temp3838=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3836))->v; struct Cyc_List_List*
_temp3839= 0; for( 0; _temp3838 !=  0; _temp3838= _temp3838->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, kind_env,(
struct Cyc_Absyn_Tvar*) _temp3838->hd)){ _temp3839=({ struct Cyc_List_List*
_temp3840=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3840->hd=( void*)(( struct Cyc_Absyn_Tvar*) _temp3838->hd); _temp3840->tl=
_temp3839; _temp3840;});}}* _temp3836=({ struct Cyc_Core_Opt* _temp3841=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3841->v=( void*)
_temp3839; _temp3841;});} goto _LL3829; _LL3833: goto _LL3829; _LL3829:;}}
return _temp3827;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ int generalize_evars= Cyc_Tcutil_is_function_type(
t); struct Cyc_Tcutil_CVTEnv _temp3843= Cyc_Tcutil_check_valid_type( loc, te, 0,(
void*) Cyc_Absyn_MemKind, generalize_evars, t); struct Cyc_List_List* _temp3844=
_temp3843.free_vars; struct Cyc_List_List* _temp3845= _temp3843.free_evars;{
struct Cyc_List_List* x= _temp3844; for( 0; x !=  0; x= x->tl){ struct Cyc_Absyn_Conref*
c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*) x->hd)->kind); void*
_temp3846=( void*) c->v; void* _temp3854; _LL3848: if( _temp3846 == ( void*) Cyc_Absyn_No_constr){
goto _LL3849;} else{ goto _LL3850;} _LL3850: if(( unsigned int) _temp3846 >  1u?*((
int*) _temp3846) ==  Cyc_Absyn_Eq_constr: 0){ _LL3855: _temp3854=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3846)->f1; if( _temp3854 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3851;} else{ goto _LL3852;}} else{ goto _LL3852;}
_LL3852: goto _LL3853; _LL3849: goto _LL3851; _LL3851:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3856=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3856[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3857; _temp3857.tag= Cyc_Absyn_Eq_constr;
_temp3857.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3857;}); _temp3856;})));
goto _LL3847; _LL3853: goto _LL3847; _LL3847:;}} if( _temp3844 !=  0? 1:
_temp3845 !=  0){{ void* _temp3858= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3864; struct Cyc_List_List* _temp3866; struct Cyc_List_List* _temp3868;
struct Cyc_Absyn_VarargInfo* _temp3870; int _temp3872; struct Cyc_List_List*
_temp3874; void* _temp3876; struct Cyc_Core_Opt* _temp3878; struct Cyc_List_List*
_temp3880; struct Cyc_List_List** _temp3882; _LL3860: if(( unsigned int)
_temp3858 >  3u?*(( int*) _temp3858) ==  Cyc_Absyn_FnType: 0){ _LL3865:
_temp3864=(( struct Cyc_Absyn_FnType_struct*) _temp3858)->f1; _LL3881: _temp3880=
_temp3864.tvars; _temp3882=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3858)->f1).tvars; goto _LL3879; _LL3879: _temp3878= _temp3864.effect; goto
_LL3877; _LL3877: _temp3876=( void*) _temp3864.ret_typ; goto _LL3875; _LL3875:
_temp3874= _temp3864.args; goto _LL3873; _LL3873: _temp3872= _temp3864.c_varargs;
goto _LL3871; _LL3871: _temp3870= _temp3864.cyc_varargs; goto _LL3869; _LL3869:
_temp3868= _temp3864.rgn_po; goto _LL3867; _LL3867: _temp3866= _temp3864.attributes;
goto _LL3861;} else{ goto _LL3862;} _LL3862: goto _LL3863; _LL3861: if(*
_temp3882 ==  0){* _temp3882= _temp3844; _temp3844= 0;} goto _LL3859; _LL3863:
goto _LL3859; _LL3859:;} if( _temp3844 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3884; _temp3884.tag= Cyc_Std_String_pa; _temp3884.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*) _temp3844->hd)->name;{ void* _temp3883[ 1u]={& _temp3884};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s", sizeof( unsigned char),
25u), _tag_arr( _temp3883, sizeof( void*), 1u));}});} if( _temp3845 !=  0){ for(
0; _temp3845 !=  0; _temp3845= _temp3845->tl){ void* e=( void*) _temp3845->hd;
void* _temp3885= Cyc_Tcutil_typ_kind( e); _LL3887: if( _temp3885 == ( void*) Cyc_Absyn_RgnKind){
goto _LL3888;} else{ goto _LL3889;} _LL3889: if( _temp3885 == ( void*) Cyc_Absyn_EffKind){
goto _LL3890;} else{ goto _LL3891;} _LL3891: goto _LL3892; _LL3888: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3893[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3893, sizeof( void*), 0u));});}
goto _LL3886; _LL3890: if( ! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({
void* _temp3894[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!", sizeof( unsigned char),
26u), _tag_arr( _temp3894, sizeof( void*), 0u));});} goto _LL3886; _LL3892:({
struct Cyc_Std_String_pa_struct _temp3897; _temp3897.tag= Cyc_Std_String_pa;
_temp3897.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp3896; _temp3896.tag= Cyc_Std_String_pa; _temp3896.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( e);{ void* _temp3895[ 2u]={& _temp3896,& _temp3897}; Cyc_Tcutil_terr(
loc, _tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3895, sizeof( void*), 2u));}}}); goto
_LL3886; _LL3886:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3898= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3904; struct Cyc_List_List* _temp3906; struct
Cyc_List_List* _temp3908; struct Cyc_Absyn_VarargInfo* _temp3910; int _temp3912;
struct Cyc_List_List* _temp3914; void* _temp3916; struct Cyc_Core_Opt* _temp3918;
struct Cyc_List_List* _temp3920; _LL3900: if(( unsigned int) _temp3898 >  3u?*((
int*) _temp3898) ==  Cyc_Absyn_FnType: 0){ _LL3905: _temp3904=(( struct Cyc_Absyn_FnType_struct*)
_temp3898)->f1; _LL3921: _temp3920= _temp3904.tvars; goto _LL3919; _LL3919:
_temp3918= _temp3904.effect; goto _LL3917; _LL3917: _temp3916=( void*) _temp3904.ret_typ;
goto _LL3915; _LL3915: _temp3914= _temp3904.args; goto _LL3913; _LL3913:
_temp3912= _temp3904.c_varargs; goto _LL3911; _LL3911: _temp3910= _temp3904.cyc_varargs;
goto _LL3909; _LL3909: _temp3908= _temp3904.rgn_po; goto _LL3907; _LL3907:
_temp3906= _temp3904.attributes; goto _LL3901;} else{ goto _LL3902;} _LL3902:
goto _LL3903; _LL3901: fd->tvs= _temp3920; fd->effect= _temp3918; goto _LL3899;
_LL3903:({ void* _temp3922[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3922, sizeof( void*), 0u));});
return; _LL3899:;}{ struct _RegionHandle _temp3923= _new_region(); struct
_RegionHandle* r=& _temp3923; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3924=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3924->v=(
void*) t; _temp3924;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3925=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3926= Cyc_Tcutil_remove_bound_tvars( _temp3925.free_vars,
bound_tvars); struct Cyc_List_List* _temp3927= _temp3925.free_evars;{ struct Cyc_List_List*
fs= _temp3926; for( 0; fs !=  0; fs= fs->tl){ struct _tagged_arr* _temp3928=((
struct Cyc_Absyn_Tvar*) fs->hd)->name;({ struct Cyc_Std_String_pa_struct
_temp3931; _temp3931.tag= Cyc_Std_String_pa; _temp3931.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3930;
_temp3930.tag= Cyc_Std_String_pa; _temp3930.f1=( struct _tagged_arr)* _temp3928;{
void* _temp3929[ 2u]={& _temp3930,& _temp3931}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3929, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3927 !=  0: 0){ for( 0; _temp3927 !=  0; _temp3927=
_temp3927->tl){ void* e=( void*) _temp3927->hd; void* _temp3932= Cyc_Tcutil_typ_kind(
e); _LL3934: if( _temp3932 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3935;} else{
goto _LL3936;} _LL3936: if( _temp3932 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3937;} else{ goto _LL3938;} _LL3938: goto _LL3939; _LL3935: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3940[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3940, sizeof( void*), 0u));});}
goto _LL3933; _LL3937: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3941=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3941[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3942; _temp3942.tag= Cyc_Absyn_JoinEff;
_temp3942.f1= 0; _temp3942;}); _temp3941;}))){({ void* _temp3943[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3943, sizeof( void*), 0u));});}
goto _LL3933; _LL3939:({ struct Cyc_Std_String_pa_struct _temp3946; _temp3946.tag=
Cyc_Std_String_pa; _temp3946.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3945; _temp3945.tag= Cyc_Std_String_pa;
_temp3945.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3944[
2u]={& _temp3945,& _temp3946}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3944, sizeof( void*), 2u));}}});
goto _LL3933; _LL3933:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs= vs->tl){
struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 !=  0; vs2= vs2->tl){ if( cmp((
void*) vs->hd,( void*) vs2->hd) ==  0){({ struct Cyc_Std_String_pa_struct
_temp3949; _temp3949.tag= Cyc_Std_String_pa; _temp3949.f1=( struct _tagged_arr)
a2string(( void*) vs->hd);{ struct Cyc_Std_String_pa_struct _temp3948; _temp3948.tag=
Cyc_Std_String_pa; _temp3948.f1=( struct _tagged_arr) msg;{ void* _temp3947[ 2u]={&
_temp3948,& _temp3949}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3947, sizeof( void*), 2u));}}});}}}} static
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
_temp3950=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3950->hd=( void*)({ struct _tuple18* _temp3951=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp3951->f1=( struct Cyc_Absyn_Structfield*)
sd_fields->hd; _temp3951->f2= 0; _temp3951;}); _temp3950->tl= fields; _temp3950;});}}}
fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0; des= des->tl){ struct
_tuple19 _temp3954; void* _temp3955; struct Cyc_List_List* _temp3957; struct
_tuple19* _temp3952=( struct _tuple19*) des->hd; _temp3954=* _temp3952; _LL3958:
_temp3957= _temp3954.f1; goto _LL3956; _LL3956: _temp3955= _temp3954.f2; goto
_LL3953; _LL3953: if( _temp3957 ==  0){ struct Cyc_List_List* _temp3959= fields;
for( 0; _temp3959 !=  0; _temp3959= _temp3959->tl){ if( !(*(( struct _tuple18*)
_temp3959->hd)).f2){(*(( struct _tuple18*) _temp3959->hd)).f2= 1;(*(( struct
_tuple19*) des->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3960=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3960->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3961=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3961[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3962; _temp3962.tag= Cyc_Absyn_FieldName;
_temp3962.f1=((*(( struct _tuple18*) _temp3959->hd)).f1)->name; _temp3962;});
_temp3961;})); _temp3960->tl= 0; _temp3960;}); ans=({ struct Cyc_List_List*
_temp3963=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3963->hd=( void*)({ struct _tuple20* _temp3964=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp3964->f1=(*(( struct
_tuple18*) _temp3959->hd)).f1; _temp3964->f2= _temp3955; _temp3964;}); _temp3963->tl=
ans; _temp3963;}); break;}} if( _temp3959 ==  0){({ void* _temp3965[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u),
_tag_arr( _temp3965, sizeof( void*), 0u));});}} else{ if( _temp3957->tl !=  0){({
void* _temp3966[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3966, sizeof( void*), 0u));});}
else{ void* _temp3967=( void*) _temp3957->hd; struct _tagged_arr* _temp3973;
_LL3969: if(*(( int*) _temp3967) ==  Cyc_Absyn_ArrayElement){ goto _LL3970;}
else{ goto _LL3971;} _LL3971: if(*(( int*) _temp3967) ==  Cyc_Absyn_FieldName){
_LL3974: _temp3973=(( struct Cyc_Absyn_FieldName_struct*) _temp3967)->f1; goto
_LL3972;} else{ goto _LL3968;} _LL3970:({ void* _temp3975[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3975, sizeof( void*), 0u));}); goto _LL3968;
_LL3972: { struct Cyc_List_List* _temp3976= fields; for( 0; _temp3976 !=  0;
_temp3976= _temp3976->tl){ if( Cyc_Std_zstrptrcmp( _temp3973,((*(( struct
_tuple18*) _temp3976->hd)).f1)->name) ==  0){ if((*(( struct _tuple18*)
_temp3976->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3978; _temp3978.tag=
Cyc_Std_String_pa; _temp3978.f1=( struct _tagged_arr)* _temp3973;{ void*
_temp3977[ 1u]={& _temp3978}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3977, sizeof( void*), 1u));}});}(*((
struct _tuple18*) _temp3976->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp3979=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3979->hd=( void*)({ struct _tuple20* _temp3980=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp3980->f1=(*(( struct
_tuple18*) _temp3976->hd)).f1; _temp3980->f2= _temp3955; _temp3980;}); _temp3979->tl=
ans; _temp3979;}); break;}} if( _temp3976 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3982; _temp3982.tag= Cyc_Std_String_pa; _temp3982.f1=( struct _tagged_arr)*
_temp3973;{ void* _temp3981[ 1u]={& _temp3982}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3981, sizeof( void*), 1u));}});}
goto _LL3968;} _LL3968:;}}} for( 0; fields !=  0; fields= fields->tl){ if( !(*((
struct _tuple18*) fields->hd)).f2){({ void* _temp3983[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments to struct", sizeof( unsigned char), 28u),
_tag_arr( _temp3983, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3984= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3990; struct Cyc_Absyn_Conref* _temp3992; void* _temp3994;
_LL3986: if(( unsigned int) _temp3984 >  3u?*(( int*) _temp3984) ==  Cyc_Absyn_PointerType:
0){ _LL3991: _temp3990=(( struct Cyc_Absyn_PointerType_struct*) _temp3984)->f1;
_LL3995: _temp3994=( void*) _temp3990.elt_typ; goto _LL3993; _LL3993: _temp3992=
_temp3990.bounds; goto _LL3987;} else{ goto _LL3988;} _LL3988: goto _LL3989;
_LL3987: { struct Cyc_Absyn_Conref* _temp3996= Cyc_Absyn_compress_conref(
_temp3992); void* _temp3997=( void*)( Cyc_Absyn_compress_conref( _temp3996))->v;
void* _temp4005; _LL3999: if(( unsigned int) _temp3997 >  1u?*(( int*) _temp3997)
==  Cyc_Absyn_Eq_constr: 0){ _LL4006: _temp4005=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3997)->f1; if( _temp4005 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4000;}
else{ goto _LL4001;}} else{ goto _LL4001;} _LL4001: if( _temp3997 == ( void*)
Cyc_Absyn_No_constr){ goto _LL4002;} else{ goto _LL4003;} _LL4003: goto _LL4004;
_LL4000:* elt_typ_dest= _temp3994; return 1; _LL4002:( void*)( _temp3996->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp4007=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp4007[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp4008; _temp4008.tag= Cyc_Absyn_Eq_constr;
_temp4008.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp4008;}); _temp4007;})));*
elt_typ_dest= _temp3994; return 1; _LL4004: return 0; _LL3998:;} _LL3989: return
0; _LL3985:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp4191;
_temp4191.f1= 0; _temp4191.f2=( void*) Cyc_Absyn_HeapRgn; _temp4191;}); void*
_temp4009=( void*) e->r; void* _temp4023; struct _tuple1* _temp4025; struct
_tagged_arr* _temp4027; struct Cyc_Absyn_Exp* _temp4029; struct _tagged_arr*
_temp4031; struct Cyc_Absyn_Exp* _temp4033; struct Cyc_Absyn_Exp* _temp4035;
struct Cyc_Absyn_Exp* _temp4037; struct Cyc_Absyn_Exp* _temp4039; _LL4011: if(*((
int*) _temp4009) ==  Cyc_Absyn_Var_e){ _LL4026: _temp4025=(( struct Cyc_Absyn_Var_e_struct*)
_temp4009)->f1; goto _LL4024; _LL4024: _temp4023=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp4009)->f2; goto _LL4012;} else{ goto _LL4013;} _LL4013: if(*(( int*)
_temp4009) ==  Cyc_Absyn_StructMember_e){ _LL4030: _temp4029=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp4009)->f1; goto _LL4028; _LL4028: _temp4027=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp4009)->f2; goto _LL4014;} else{ goto _LL4015;} _LL4015: if(*(( int*)
_temp4009) ==  Cyc_Absyn_StructArrow_e){ _LL4034: _temp4033=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp4009)->f1; goto _LL4032; _LL4032: _temp4031=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp4009)->f2; goto _LL4016;} else{ goto _LL4017;} _LL4017: if(*(( int*)
_temp4009) ==  Cyc_Absyn_Deref_e){ _LL4036: _temp4035=(( struct Cyc_Absyn_Deref_e_struct*)
_temp4009)->f1; goto _LL4018;} else{ goto _LL4019;} _LL4019: if(*(( int*)
_temp4009) ==  Cyc_Absyn_Subscript_e){ _LL4040: _temp4039=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp4009)->f1; goto _LL4038; _LL4038: _temp4037=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp4009)->f2; goto _LL4020;} else{ goto _LL4021;} _LL4021: goto _LL4022;
_LL4012: { void* _temp4041= _temp4023; struct Cyc_Absyn_Vardecl* _temp4055;
struct Cyc_Absyn_Vardecl* _temp4057; struct Cyc_Absyn_Vardecl* _temp4059; struct
Cyc_Absyn_Vardecl* _temp4061; _LL4043: if( _temp4041 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL4044;} else{ goto _LL4045;} _LL4045: if(( unsigned int) _temp4041 >  1u?*((
int*) _temp4041) ==  Cyc_Absyn_Funname_b: 0){ goto _LL4046;} else{ goto _LL4047;}
_LL4047: if(( unsigned int) _temp4041 >  1u?*(( int*) _temp4041) ==  Cyc_Absyn_Global_b:
0){ _LL4056: _temp4055=(( struct Cyc_Absyn_Global_b_struct*) _temp4041)->f1;
goto _LL4048;} else{ goto _LL4049;} _LL4049: if(( unsigned int) _temp4041 >  1u?*((
int*) _temp4041) ==  Cyc_Absyn_Local_b: 0){ _LL4058: _temp4057=(( struct Cyc_Absyn_Local_b_struct*)
_temp4041)->f1; goto _LL4050;} else{ goto _LL4051;} _LL4051: if(( unsigned int)
_temp4041 >  1u?*(( int*) _temp4041) ==  Cyc_Absyn_Pat_b: 0){ _LL4060: _temp4059=((
struct Cyc_Absyn_Pat_b_struct*) _temp4041)->f1; goto _LL4052;} else{ goto
_LL4053;} _LL4053: if(( unsigned int) _temp4041 >  1u?*(( int*) _temp4041) == 
Cyc_Absyn_Param_b: 0){ _LL4062: _temp4061=(( struct Cyc_Absyn_Param_b_struct*)
_temp4041)->f1; goto _LL4054;} else{ goto _LL4042;} _LL4044: return bogus_ans;
_LL4046: return({ struct _tuple7 _temp4063; _temp4063.f1= 0; _temp4063.f2=( void*)
Cyc_Absyn_HeapRgn; _temp4063;}); _LL4048: { void* _temp4064= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL4066: if((
unsigned int) _temp4064 >  3u?*(( int*) _temp4064) ==  Cyc_Absyn_ArrayType: 0){
goto _LL4067;} else{ goto _LL4068;} _LL4068: goto _LL4069; _LL4067: return({
struct _tuple7 _temp4070; _temp4070.f1= 1; _temp4070.f2=( void*) Cyc_Absyn_HeapRgn;
_temp4070;}); _LL4069: return({ struct _tuple7 _temp4071; _temp4071.f1=(
_temp4055->tq).q_const; _temp4071.f2=( void*) Cyc_Absyn_HeapRgn; _temp4071;});
_LL4065:;} _LL4050: { void* _temp4072= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL4074: if(( unsigned int) _temp4072 >  3u?*(( int*)
_temp4072) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4075;} else{ goto _LL4076;}
_LL4076: goto _LL4077; _LL4075: return({ struct _tuple7 _temp4078; _temp4078.f1=
1; _temp4078.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp4057->rgn))->v;
_temp4078;}); _LL4077: return({ struct _tuple7 _temp4079; _temp4079.f1=(
_temp4057->tq).q_const; _temp4079.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4057->rgn))->v; _temp4079;}); _LL4073:;} _LL4052: _temp4061=
_temp4059; goto _LL4054; _LL4054: return({ struct _tuple7 _temp4080; _temp4080.f1=(
_temp4061->tq).q_const; _temp4080.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4061->rgn))->v; _temp4080;}); _LL4042:;} _LL4014: { void*
_temp4081= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp4029->topt))->v); struct Cyc_List_List* _temp4093; struct Cyc_List_List*
_temp4095; struct Cyc_Absyn_Structdecl** _temp4097; struct Cyc_Absyn_Structdecl*
_temp4099; struct Cyc_Absyn_Uniondecl** _temp4100; struct Cyc_Absyn_Uniondecl*
_temp4102; _LL4083: if(( unsigned int) _temp4081 >  3u?*(( int*) _temp4081) == 
Cyc_Absyn_AnonStructType: 0){ _LL4094: _temp4093=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp4081)->f1; goto _LL4084;} else{ goto _LL4085;} _LL4085: if(( unsigned int)
_temp4081 >  3u?*(( int*) _temp4081) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4096:
_temp4095=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp4081)->f1; goto
_LL4086;} else{ goto _LL4087;} _LL4087: if(( unsigned int) _temp4081 >  3u?*((
int*) _temp4081) ==  Cyc_Absyn_StructType: 0){ _LL4098: _temp4097=(( struct Cyc_Absyn_StructType_struct*)
_temp4081)->f3; if( _temp4097 ==  0){ goto _LL4089;} else{ _temp4099=* _temp4097;
goto _LL4088;}} else{ goto _LL4089;} _LL4089: if(( unsigned int) _temp4081 >  3u?*((
int*) _temp4081) ==  Cyc_Absyn_UnionType: 0){ _LL4101: _temp4100=(( struct Cyc_Absyn_UnionType_struct*)
_temp4081)->f3; if( _temp4100 ==  0){ goto _LL4091;} else{ _temp4102=* _temp4100;
goto _LL4090;}} else{ goto _LL4091;} _LL4091: goto _LL4092; _LL4084: _temp4095=
_temp4093; goto _LL4086; _LL4086: { struct Cyc_Absyn_Structfield* _temp4103= Cyc_Absyn_lookup_field(
_temp4095, _temp4027); if( _temp4103 !=  0? _temp4103->width !=  0: 0){ return({
struct _tuple7 _temp4104; _temp4104.f1=( _temp4103->tq).q_const; _temp4104.f2=(
Cyc_Tcutil_addressof_props( te, _temp4029)).f2; _temp4104;});} return bogus_ans;}
_LL4088: { struct Cyc_Absyn_Structfield* _temp4105= Cyc_Absyn_lookup_struct_field(
_temp4099, _temp4027); if( _temp4105 !=  0? _temp4105->width !=  0: 0){ return({
struct _tuple7 _temp4106; _temp4106.f1=( _temp4105->tq).q_const; _temp4106.f2=(
Cyc_Tcutil_addressof_props( te, _temp4029)).f2; _temp4106;});} return bogus_ans;}
_LL4090: { struct Cyc_Absyn_Structfield* _temp4107= Cyc_Absyn_lookup_union_field(
_temp4102, _temp4027); if( _temp4107 !=  0){ return({ struct _tuple7 _temp4108;
_temp4108.f1=( _temp4107->tq).q_const; _temp4108.f2=( Cyc_Tcutil_addressof_props(
te, _temp4029)).f2; _temp4108;});} goto _LL4092;} _LL4092: return bogus_ans;
_LL4082:;} _LL4016: { void* _temp4109= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4033->topt))->v); struct Cyc_Absyn_PtrInfo _temp4115; void*
_temp4117; void* _temp4119; _LL4111: if(( unsigned int) _temp4109 >  3u?*(( int*)
_temp4109) ==  Cyc_Absyn_PointerType: 0){ _LL4116: _temp4115=(( struct Cyc_Absyn_PointerType_struct*)
_temp4109)->f1; _LL4120: _temp4119=( void*) _temp4115.elt_typ; goto _LL4118;
_LL4118: _temp4117=( void*) _temp4115.rgn_typ; goto _LL4112;} else{ goto _LL4113;}
_LL4113: goto _LL4114; _LL4112: { void* _temp4121= Cyc_Tcutil_compress(
_temp4119); struct Cyc_List_List* _temp4133; struct Cyc_List_List* _temp4135;
struct Cyc_Absyn_Structdecl** _temp4137; struct Cyc_Absyn_Structdecl* _temp4139;
struct Cyc_Absyn_Uniondecl** _temp4140; struct Cyc_Absyn_Uniondecl* _temp4142;
_LL4123: if(( unsigned int) _temp4121 >  3u?*(( int*) _temp4121) ==  Cyc_Absyn_AnonStructType:
0){ _LL4134: _temp4133=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4121)->f1;
goto _LL4124;} else{ goto _LL4125;} _LL4125: if(( unsigned int) _temp4121 >  3u?*((
int*) _temp4121) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4136: _temp4135=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4121)->f1; goto _LL4126;} else{ goto
_LL4127;} _LL4127: if(( unsigned int) _temp4121 >  3u?*(( int*) _temp4121) == 
Cyc_Absyn_StructType: 0){ _LL4138: _temp4137=(( struct Cyc_Absyn_StructType_struct*)
_temp4121)->f3; if( _temp4137 ==  0){ goto _LL4129;} else{ _temp4139=* _temp4137;
goto _LL4128;}} else{ goto _LL4129;} _LL4129: if(( unsigned int) _temp4121 >  3u?*((
int*) _temp4121) ==  Cyc_Absyn_UnionType: 0){ _LL4141: _temp4140=(( struct Cyc_Absyn_UnionType_struct*)
_temp4121)->f3; if( _temp4140 ==  0){ goto _LL4131;} else{ _temp4142=* _temp4140;
goto _LL4130;}} else{ goto _LL4131;} _LL4131: goto _LL4132; _LL4124: _temp4135=
_temp4133; goto _LL4126; _LL4126: { struct Cyc_Absyn_Structfield* _temp4143= Cyc_Absyn_lookup_field(
_temp4135, _temp4031); if( _temp4143 !=  0? _temp4143->width !=  0: 0){ return({
struct _tuple7 _temp4144; _temp4144.f1=( _temp4143->tq).q_const; _temp4144.f2=
_temp4117; _temp4144;});} return bogus_ans;} _LL4128: { struct Cyc_Absyn_Structfield*
_temp4145= Cyc_Absyn_lookup_struct_field( _temp4139, _temp4031); if( _temp4145
!=  0? _temp4145->width !=  0: 0){ return({ struct _tuple7 _temp4146; _temp4146.f1=(
_temp4145->tq).q_const; _temp4146.f2= _temp4117; _temp4146;});} return bogus_ans;}
_LL4130: { struct Cyc_Absyn_Structfield* _temp4147= Cyc_Absyn_lookup_union_field(
_temp4142, _temp4031); if( _temp4147 !=  0){ return({ struct _tuple7 _temp4148;
_temp4148.f1=( _temp4147->tq).q_const; _temp4148.f2= _temp4117; _temp4148;});}
return bogus_ans;} _LL4132: return bogus_ans; _LL4122:;} _LL4114: return
bogus_ans; _LL4110:;} _LL4018: { void* _temp4149= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp4035->topt))->v); struct Cyc_Absyn_PtrInfo
_temp4155; struct Cyc_Absyn_Tqual _temp4157; void* _temp4159; _LL4151: if((
unsigned int) _temp4149 >  3u?*(( int*) _temp4149) ==  Cyc_Absyn_PointerType: 0){
_LL4156: _temp4155=(( struct Cyc_Absyn_PointerType_struct*) _temp4149)->f1;
_LL4160: _temp4159=( void*) _temp4155.rgn_typ; goto _LL4158; _LL4158: _temp4157=
_temp4155.tq; goto _LL4152;} else{ goto _LL4153;} _LL4153: goto _LL4154; _LL4152:
return({ struct _tuple7 _temp4161; _temp4161.f1= _temp4157.q_const; _temp4161.f2=
_temp4159; _temp4161;}); _LL4154: return bogus_ans; _LL4150:;} _LL4020: { void*
t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp4039->topt))->v);
void* _temp4162= t; struct Cyc_Absyn_Tqual _temp4172; struct Cyc_List_List*
_temp4174; struct Cyc_Absyn_PtrInfo _temp4176; struct Cyc_Absyn_Conref*
_temp4178; struct Cyc_Absyn_Tqual _temp4180; void* _temp4182; void* _temp4184;
_LL4164: if(( unsigned int) _temp4162 >  3u?*(( int*) _temp4162) ==  Cyc_Absyn_ArrayType:
0){ _LL4173: _temp4172=(( struct Cyc_Absyn_ArrayType_struct*) _temp4162)->f2;
goto _LL4165;} else{ goto _LL4166;} _LL4166: if(( unsigned int) _temp4162 >  3u?*((
int*) _temp4162) ==  Cyc_Absyn_TupleType: 0){ _LL4175: _temp4174=(( struct Cyc_Absyn_TupleType_struct*)
_temp4162)->f1; goto _LL4167;} else{ goto _LL4168;} _LL4168: if(( unsigned int)
_temp4162 >  3u?*(( int*) _temp4162) ==  Cyc_Absyn_PointerType: 0){ _LL4177:
_temp4176=(( struct Cyc_Absyn_PointerType_struct*) _temp4162)->f1; _LL4185:
_temp4184=( void*) _temp4176.elt_typ; goto _LL4183; _LL4183: _temp4182=( void*)
_temp4176.rgn_typ; goto _LL4181; _LL4181: _temp4180= _temp4176.tq; goto _LL4179;
_LL4179: _temp4178= _temp4176.bounds; goto _LL4169;} else{ goto _LL4170;}
_LL4170: goto _LL4171; _LL4165: return({ struct _tuple7 _temp4186; _temp4186.f1=
_temp4172.q_const; _temp4186.f2=( Cyc_Tcutil_addressof_props( te, _temp4039)).f2;
_temp4186;}); _LL4167: { struct _tuple4* _temp4187= Cyc_Absyn_lookup_tuple_field(
_temp4174,( int) Cyc_Evexp_eval_const_uint_exp( _temp4037)); if( _temp4187 !=  0){
return({ struct _tuple7 _temp4188; _temp4188.f1=((* _temp4187).f1).q_const;
_temp4188.f2=( Cyc_Tcutil_addressof_props( te, _temp4039)).f2; _temp4188;});}
return bogus_ans;} _LL4169: return({ struct _tuple7 _temp4189; _temp4189.f1=
_temp4180.q_const; _temp4189.f2= _temp4182; _temp4189;}); _LL4171: return
bogus_ans; _LL4163:;} _LL4022:({ void* _temp4190[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u), _tag_arr(
_temp4190, sizeof( void*), 0u));}); return bogus_ans; _LL4010:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp4192= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Tqual _temp4198; void*
_temp4200; _LL4194: if(( unsigned int) _temp4192 >  3u?*(( int*) _temp4192) == 
Cyc_Absyn_ArrayType: 0){ _LL4201: _temp4200=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp4192)->f1; goto _LL4199; _LL4199: _temp4198=(( struct Cyc_Absyn_ArrayType_struct*)
_temp4192)->f2; goto _LL4195;} else{ goto _LL4196;} _LL4196: goto _LL4197;
_LL4195: { void* _temp4204; struct _tuple7 _temp4202= Cyc_Tcutil_addressof_props(
te, e); _LL4205: _temp4204= _temp4202.f2; goto _LL4203; _LL4203: return Cyc_Absyn_atb_typ(
_temp4200, _temp4204, _temp4198,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp4206=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4206[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4207; _temp4207.tag= Cyc_Absyn_Upper_b;
_temp4207.f1= e; _temp4207;}); _temp4206;}));} _LL4197: return e_typ; _LL4193:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp4208=(
void*) b->v; void* _temp4216; void* _temp4218; struct Cyc_Absyn_Exp* _temp4220;
_LL4210: if(( unsigned int) _temp4208 >  1u?*(( int*) _temp4208) ==  Cyc_Absyn_Eq_constr:
0){ _LL4217: _temp4216=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4208)->f1;
if( _temp4216 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4211;} else{ goto
_LL4212;}} else{ goto _LL4212;} _LL4212: if(( unsigned int) _temp4208 >  1u?*((
int*) _temp4208) ==  Cyc_Absyn_Eq_constr: 0){ _LL4219: _temp4218=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp4208)->f1; if(( unsigned int) _temp4218
>  1u?*(( int*) _temp4218) ==  Cyc_Absyn_Upper_b: 0){ _LL4221: _temp4220=((
struct Cyc_Absyn_Upper_b_struct*) _temp4218)->f1; goto _LL4213;} else{ goto
_LL4214;}} else{ goto _LL4214;} _LL4214: goto _LL4215; _LL4211: return; _LL4213:
if( Cyc_Evexp_eval_const_uint_exp( _temp4220) <=  i){({ struct Cyc_Std_Int_pa_struct
_temp4224; _temp4224.tag= Cyc_Std_Int_pa; _temp4224.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp4223; _temp4223.tag= Cyc_Std_Int_pa; _temp4223.f1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp4220);{ void* _temp4222[ 2u]={& _temp4223,& _temp4224}; Cyc_Tcutil_terr(
loc, _tag_arr("dereference is out of bounds: %d <= %d", sizeof( unsigned char),
39u), _tag_arr( _temp4222, sizeof( void*), 2u));}}});} return; _LL4215:( void*)(
b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp4225=( struct
Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp4225[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp4226; _temp4226.tag= Cyc_Absyn_Eq_constr;
_temp4226.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp4227=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4227[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4228; _temp4228.tag= Cyc_Absyn_Upper_b;
_temp4228.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp4228;}); _temp4227;}));
_temp4226;}); _temp4225;}))); return; _LL4209:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp4229=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp4235; struct
Cyc_Absyn_Exp* _temp4237; _LL4231: if(( unsigned int) _temp4229 >  1u?*(( int*)
_temp4229) ==  Cyc_Absyn_Eq_constr: 0){ _LL4236: _temp4235=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4229)->f1; if(( unsigned int) _temp4235 >  1u?*(( int*) _temp4235) ==  Cyc_Absyn_Upper_b:
0){ _LL4238: _temp4237=(( struct Cyc_Absyn_Upper_b_struct*) _temp4235)->f1; goto
_LL4232;} else{ goto _LL4233;}} else{ goto _LL4233;} _LL4233: goto _LL4234;
_LL4232: return Cyc_Evexp_eval_const_uint_exp( _temp4237) ==  1; _LL4234: return
0; _LL4230:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp4239= Cyc_Tcutil_compress(
t); void* _temp4267; struct Cyc_List_List* _temp4269; struct Cyc_Absyn_Structdecl**
_temp4271; struct Cyc_List_List* _temp4273; struct Cyc_Absyn_Uniondecl**
_temp4275; struct Cyc_List_List* _temp4277; struct Cyc_List_List* _temp4279;
struct Cyc_List_List* _temp4281; _LL4241: if( _temp4239 == ( void*) Cyc_Absyn_VoidType){
goto _LL4242;} else{ goto _LL4243;} _LL4243: if(( unsigned int) _temp4239 >  3u?*((
int*) _temp4239) ==  Cyc_Absyn_IntType: 0){ goto _LL4244;} else{ goto _LL4245;}
_LL4245: if( _temp4239 == ( void*) Cyc_Absyn_FloatType){ goto _LL4246;} else{
goto _LL4247;} _LL4247: if(( unsigned int) _temp4239 >  3u?*(( int*) _temp4239)
==  Cyc_Absyn_DoubleType: 0){ goto _LL4248;} else{ goto _LL4249;} _LL4249: if((
unsigned int) _temp4239 >  3u?*(( int*) _temp4239) ==  Cyc_Absyn_EnumType: 0){
goto _LL4250;} else{ goto _LL4251;} _LL4251: if(( unsigned int) _temp4239 >  3u?*((
int*) _temp4239) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL4252;} else{ goto
_LL4253;} _LL4253: if(( unsigned int) _temp4239 >  3u?*(( int*) _temp4239) == 
Cyc_Absyn_ArrayType: 0){ _LL4268: _temp4267=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp4239)->f1; goto _LL4254;} else{ goto _LL4255;} _LL4255: if(( unsigned int)
_temp4239 >  3u?*(( int*) _temp4239) ==  Cyc_Absyn_TupleType: 0){ _LL4270:
_temp4269=(( struct Cyc_Absyn_TupleType_struct*) _temp4239)->f1; goto _LL4256;}
else{ goto _LL4257;} _LL4257: if(( unsigned int) _temp4239 >  3u?*(( int*)
_temp4239) ==  Cyc_Absyn_StructType: 0){ _LL4274: _temp4273=(( struct Cyc_Absyn_StructType_struct*)
_temp4239)->f2; goto _LL4272; _LL4272: _temp4271=(( struct Cyc_Absyn_StructType_struct*)
_temp4239)->f3; goto _LL4258;} else{ goto _LL4259;} _LL4259: if(( unsigned int)
_temp4239 >  3u?*(( int*) _temp4239) ==  Cyc_Absyn_UnionType: 0){ _LL4278:
_temp4277=(( struct Cyc_Absyn_UnionType_struct*) _temp4239)->f2; goto _LL4276;
_LL4276: _temp4275=(( struct Cyc_Absyn_UnionType_struct*) _temp4239)->f3; goto
_LL4260;} else{ goto _LL4261;} _LL4261: if(( unsigned int) _temp4239 >  3u?*((
int*) _temp4239) ==  Cyc_Absyn_AnonStructType: 0){ _LL4280: _temp4279=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp4239)->f1; goto _LL4262;} else{ goto
_LL4263;} _LL4263: if(( unsigned int) _temp4239 >  3u?*(( int*) _temp4239) == 
Cyc_Absyn_AnonUnionType: 0){ _LL4282: _temp4281=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp4239)->f1; goto _LL4264;} else{ goto _LL4265;} _LL4265: goto _LL4266;
_LL4242: goto _LL4244; _LL4244: goto _LL4246; _LL4246: goto _LL4248; _LL4248:
return 1; _LL4250: return 0; _LL4252: return 0; _LL4254: return Cyc_Tcutil_bits_only(
_temp4267); _LL4256: for( 0; _temp4269 !=  0; _temp4269= _temp4269->tl){ if( !
Cyc_Tcutil_bits_only((*(( struct _tuple4*) _temp4269->hd)).f2)){ return 0;}}
return 1; _LL4258: if( _temp4271 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp4283=* _temp4271; if( _temp4283->fields ==  0){ return 0;}{ struct
_RegionHandle _temp4284= _new_region(); struct _RegionHandle* rgn=& _temp4284;
_push_region( rgn);{ struct Cyc_List_List* _temp4285=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp4283->tvs, _temp4273);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp4283->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp4285,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ int _temp4286= 0; _npop_handler( 0u); return _temp4286;}}}{
int _temp4287= 1; _npop_handler( 0u); return _temp4287;}}; _pop_region( rgn);}}
_LL4260: if( _temp4275 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* _temp4288=*
_temp4275; if( _temp4288->fields ==  0){ return 0;}{ struct _RegionHandle
_temp4289= _new_region(); struct _RegionHandle* rgn=& _temp4289; _push_region(
rgn);{ struct Cyc_List_List* _temp4290=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp4288->tvs, _temp4277);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp4288->fields))->v;
for( 0; fs !=  0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp4290,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ int
_temp4291= 0; _npop_handler( 0u); return _temp4291;}}}{ int _temp4292= 1;
_npop_handler( 0u); return _temp4292;}}; _pop_region( rgn);}} _LL4262: _temp4281=
_temp4279; goto _LL4264; _LL4264: for( 0; _temp4281 !=  0; _temp4281= _temp4281->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*) _temp4281->hd)->type)){
return 0;}} return 1; _LL4266: return 0; _LL4240:;} struct _tuple21{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp4293=( void*) e->r;
struct _tuple1* _temp4339; struct Cyc_Absyn_Exp* _temp4341; struct Cyc_Absyn_Exp*
_temp4343; struct Cyc_Absyn_Exp* _temp4345; struct Cyc_Absyn_Exp* _temp4347;
struct Cyc_Absyn_Exp* _temp4349; struct Cyc_Absyn_Exp* _temp4351; struct Cyc_Absyn_Exp*
_temp4353; struct Cyc_Absyn_Exp* _temp4355; void* _temp4357; struct Cyc_Absyn_Exp*
_temp4359; struct Cyc_Absyn_Exp* _temp4361; struct Cyc_Absyn_Exp* _temp4363;
struct Cyc_List_List* _temp4365; struct Cyc_List_List* _temp4367; struct Cyc_List_List*
_temp4369; struct Cyc_List_List* _temp4371; void* _temp4373; struct Cyc_List_List*
_temp4375; struct Cyc_List_List* _temp4377; _LL4295: if(*(( int*) _temp4293) == 
Cyc_Absyn_Const_e){ goto _LL4296;} else{ goto _LL4297;} _LL4297: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL4298;} else{ goto _LL4299;}
_LL4299: if(*(( int*) _temp4293) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL4300;}
else{ goto _LL4301;} _LL4301: if(*(( int*) _temp4293) ==  Cyc_Absyn_Offsetof_e){
goto _LL4302;} else{ goto _LL4303;} _LL4303: if(*(( int*) _temp4293) ==  Cyc_Absyn_Gentyp_e){
goto _LL4304;} else{ goto _LL4305;} _LL4305: if(*(( int*) _temp4293) ==  Cyc_Absyn_Enum_e){
goto _LL4306;} else{ goto _LL4307;} _LL4307: if(*(( int*) _temp4293) ==  Cyc_Absyn_AnonEnum_e){
goto _LL4308;} else{ goto _LL4309;} _LL4309: if(*(( int*) _temp4293) ==  Cyc_Absyn_Var_e){
_LL4340: _temp4339=(( struct Cyc_Absyn_Var_e_struct*) _temp4293)->f1; goto
_LL4310;} else{ goto _LL4311;} _LL4311: if(*(( int*) _temp4293) ==  Cyc_Absyn_Conditional_e){
_LL4346: _temp4345=(( struct Cyc_Absyn_Conditional_e_struct*) _temp4293)->f1;
goto _LL4344; _LL4344: _temp4343=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4293)->f2; goto _LL4342; _LL4342: _temp4341=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4293)->f3; goto _LL4312;} else{ goto _LL4313;} _LL4313: if(*(( int*)
_temp4293) ==  Cyc_Absyn_SeqExp_e){ _LL4350: _temp4349=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4293)->f1; goto _LL4348; _LL4348: _temp4347=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4293)->f2; goto _LL4314;} else{ goto _LL4315;} _LL4315: if(*(( int*)
_temp4293) ==  Cyc_Absyn_NoInstantiate_e){ _LL4352: _temp4351=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp4293)->f1; goto _LL4316;} else{ goto _LL4317;} _LL4317: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Instantiate_e){ _LL4354: _temp4353=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp4293)->f1; goto _LL4318;} else{ goto _LL4319;} _LL4319: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Cast_e){ _LL4358: _temp4357=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp4293)->f1; goto _LL4356; _LL4356: _temp4355=(( struct Cyc_Absyn_Cast_e_struct*)
_temp4293)->f2; goto _LL4320;} else{ goto _LL4321;} _LL4321: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Address_e){ _LL4360: _temp4359=(( struct Cyc_Absyn_Address_e_struct*)
_temp4293)->f1; goto _LL4322;} else{ goto _LL4323;} _LL4323: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Comprehension_e){ _LL4364: _temp4363=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4293)->f2; goto _LL4362; _LL4362: _temp4361=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4293)->f3; goto _LL4324;} else{ goto _LL4325;} _LL4325: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Array_e){ _LL4366: _temp4365=(( struct Cyc_Absyn_Array_e_struct*)
_temp4293)->f1; goto _LL4326;} else{ goto _LL4327;} _LL4327: if(*(( int*)
_temp4293) ==  Cyc_Absyn_AnonStruct_e){ _LL4368: _temp4367=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp4293)->f2; goto _LL4328;} else{ goto _LL4329;} _LL4329: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Struct_e){ _LL4370: _temp4369=(( struct Cyc_Absyn_Struct_e_struct*)
_temp4293)->f3; goto _LL4330;} else{ goto _LL4331;} _LL4331: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Primop_e){ _LL4374: _temp4373=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp4293)->f1; goto _LL4372; _LL4372: _temp4371=(( struct Cyc_Absyn_Primop_e_struct*)
_temp4293)->f2; goto _LL4332;} else{ goto _LL4333;} _LL4333: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Tuple_e){ _LL4376: _temp4375=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp4293)->f1; goto _LL4334;} else{ goto _LL4335;} _LL4335: if(*(( int*)
_temp4293) ==  Cyc_Absyn_Tunion_e){ _LL4378: _temp4377=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp4293)->f3; goto _LL4336;} else{ goto _LL4337;} _LL4337: goto _LL4338;
_LL4296: return 1; _LL4298: return 1; _LL4300: return 1; _LL4302: return 1;
_LL4304: return 1; _LL4306: return 1; _LL4308: return 1; _LL4310: { struct
_handler_cons _temp4379; _push_handler(& _temp4379);{ int _temp4381= 0; if(
setjmp( _temp4379.handler)){ _temp4381= 1;} if( ! _temp4381){{ void* _temp4382=
Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp4339); void* _temp4392; void*
_temp4394; struct Cyc_Absyn_Vardecl* _temp4396; void* _temp4398; _LL4384: if(*((
int*) _temp4382) ==  Cyc_Tcenv_VarRes){ _LL4393: _temp4392=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp4382)->f1; if(( unsigned int) _temp4392 >  1u?*(( int*) _temp4392) ==  Cyc_Absyn_Funname_b:
0){ goto _LL4385;} else{ goto _LL4386;}} else{ goto _LL4386;} _LL4386: if(*((
int*) _temp4382) ==  Cyc_Tcenv_VarRes){ _LL4395: _temp4394=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp4382)->f1; if(( unsigned int) _temp4394 >  1u?*(( int*) _temp4394) ==  Cyc_Absyn_Global_b:
0){ _LL4397: _temp4396=(( struct Cyc_Absyn_Global_b_struct*) _temp4394)->f1;
goto _LL4387;} else{ goto _LL4388;}} else{ goto _LL4388;} _LL4388: if(*(( int*)
_temp4382) ==  Cyc_Tcenv_VarRes){ _LL4399: _temp4398=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp4382)->f1; if( _temp4398 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL4389;}
else{ goto _LL4390;}} else{ goto _LL4390;} _LL4390: goto _LL4391; _LL4385: { int
_temp4400= 1; _npop_handler( 0u); return _temp4400;} _LL4387: { void* _temp4401=
Cyc_Tcutil_compress(( void*) _temp4396->type); _LL4403: if(( unsigned int)
_temp4401 >  3u?*(( int*) _temp4401) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4404;}
else{ goto _LL4405;} _LL4405: goto _LL4406; _LL4404: { int _temp4407= 1;
_npop_handler( 0u); return _temp4407;} _LL4406: { int _temp4408= var_okay;
_npop_handler( 0u); return _temp4408;} _LL4402:;} _LL4389: { int _temp4409= 0;
_npop_handler( 0u); return _temp4409;} _LL4391: { int _temp4410= var_okay;
_npop_handler( 0u); return _temp4410;} _LL4383:;}; _pop_handler();} else{ void*
_temp4380=( void*) _exn_thrown; void* _temp4412= _temp4380; _LL4414: if(
_temp4412 ==  Cyc_Dict_Absent){ goto _LL4415;} else{ goto _LL4416;} _LL4416:
goto _LL4417; _LL4415: return 0; _LL4417:( void) _throw( _temp4412); _LL4413:;}}}
_LL4312: return( Cyc_Tcutil_cnst_exp( te, 0, _temp4345)? Cyc_Tcutil_cnst_exp( te,
0, _temp4343): 0)? Cyc_Tcutil_cnst_exp( te, 0, _temp4341): 0; _LL4314: return
Cyc_Tcutil_cnst_exp( te, 0, _temp4349)? Cyc_Tcutil_cnst_exp( te, 0, _temp4347):
0; _LL4316: _temp4353= _temp4351; goto _LL4318; _LL4318: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp4353); _LL4320: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp4355); _LL4322: return Cyc_Tcutil_cnst_exp( te, 1, _temp4359); _LL4324:
return Cyc_Tcutil_cnst_exp( te, 0, _temp4363)? Cyc_Tcutil_cnst_exp( te, 0,
_temp4361): 0; _LL4326: _temp4367= _temp4365; goto _LL4328; _LL4328: _temp4369=
_temp4367; goto _LL4330; _LL4330: for( 0; _temp4369 !=  0; _temp4369= _temp4369->tl){
if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple21*) _temp4369->hd)).f2)){
return 0;}} return 1; _LL4332: _temp4375= _temp4371; goto _LL4334; _LL4334:
_temp4377= _temp4375; goto _LL4336; _LL4336: for( 0; _temp4377 !=  0; _temp4377=
_temp4377->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp4377->hd)){ return 0;}} return 1; _LL4338: return 0; _LL4294:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp4418= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp4448; struct Cyc_Absyn_Conref* _temp4450; struct Cyc_Absyn_Conref*
_temp4452; void* _temp4454; struct Cyc_List_List* _temp4456; struct Cyc_Absyn_Structdecl**
_temp4458; struct Cyc_List_List* _temp4460; struct Cyc_Absyn_Uniondecl**
_temp4462; struct Cyc_List_List* _temp4464; struct Cyc_List_List* _temp4466;
struct Cyc_List_List* _temp4468; _LL4420: if( _temp4418 == ( void*) Cyc_Absyn_VoidType){
goto _LL4421;} else{ goto _LL4422;} _LL4422: if(( unsigned int) _temp4418 >  3u?*((
int*) _temp4418) ==  Cyc_Absyn_IntType: 0){ goto _LL4423;} else{ goto _LL4424;}
_LL4424: if( _temp4418 == ( void*) Cyc_Absyn_FloatType){ goto _LL4425;} else{
goto _LL4426;} _LL4426: if(( unsigned int) _temp4418 >  3u?*(( int*) _temp4418)
==  Cyc_Absyn_DoubleType: 0){ goto _LL4427;} else{ goto _LL4428;} _LL4428: if((
unsigned int) _temp4418 >  3u?*(( int*) _temp4418) ==  Cyc_Absyn_PointerType: 0){
_LL4449: _temp4448=(( struct Cyc_Absyn_PointerType_struct*) _temp4418)->f1;
_LL4453: _temp4452= _temp4448.nullable; goto _LL4451; _LL4451: _temp4450=
_temp4448.bounds; goto _LL4429;} else{ goto _LL4430;} _LL4430: if(( unsigned int)
_temp4418 >  3u?*(( int*) _temp4418) ==  Cyc_Absyn_ArrayType: 0){ _LL4455:
_temp4454=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp4418)->f1; goto
_LL4431;} else{ goto _LL4432;} _LL4432: if(( unsigned int) _temp4418 >  3u?*((
int*) _temp4418) ==  Cyc_Absyn_TupleType: 0){ _LL4457: _temp4456=(( struct Cyc_Absyn_TupleType_struct*)
_temp4418)->f1; goto _LL4433;} else{ goto _LL4434;} _LL4434: if(( unsigned int)
_temp4418 >  3u?*(( int*) _temp4418) ==  Cyc_Absyn_StructType: 0){ _LL4461:
_temp4460=(( struct Cyc_Absyn_StructType_struct*) _temp4418)->f2; goto _LL4459;
_LL4459: _temp4458=(( struct Cyc_Absyn_StructType_struct*) _temp4418)->f3; goto
_LL4435;} else{ goto _LL4436;} _LL4436: if(( unsigned int) _temp4418 >  3u?*((
int*) _temp4418) ==  Cyc_Absyn_UnionType: 0){ _LL4465: _temp4464=(( struct Cyc_Absyn_UnionType_struct*)
_temp4418)->f2; goto _LL4463; _LL4463: _temp4462=(( struct Cyc_Absyn_UnionType_struct*)
_temp4418)->f3; goto _LL4437;} else{ goto _LL4438;} _LL4438: if(( unsigned int)
_temp4418 >  3u?*(( int*) _temp4418) ==  Cyc_Absyn_AnonStructType: 0){ _LL4467:
_temp4466=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4418)->f1; goto
_LL4439;} else{ goto _LL4440;} _LL4440: if(( unsigned int) _temp4418 >  3u?*((
int*) _temp4418) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4469: _temp4468=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4418)->f1; goto _LL4441;} else{ goto
_LL4442;} _LL4442: if(( unsigned int) _temp4418 >  3u?*(( int*) _temp4418) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL4443;} else{ goto _LL4444;} _LL4444: if((
unsigned int) _temp4418 >  3u?*(( int*) _temp4418) ==  Cyc_Absyn_EnumType: 0){
goto _LL4445;} else{ goto _LL4446;} _LL4446: goto _LL4447; _LL4421: goto _LL4423;
_LL4423: goto _LL4425; _LL4425: goto _LL4427; _LL4427: return 1; _LL4429: { void*
_temp4470=( void*)( Cyc_Absyn_compress_conref( _temp4450))->v; void* _temp4478;
void* _temp4480; _LL4472: if(( unsigned int) _temp4470 >  1u?*(( int*) _temp4470)
==  Cyc_Absyn_Eq_constr: 0){ _LL4479: _temp4478=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4470)->f1; if( _temp4478 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4473;}
else{ goto _LL4474;}} else{ goto _LL4474;} _LL4474: if(( unsigned int) _temp4470
>  1u?*(( int*) _temp4470) ==  Cyc_Absyn_Eq_constr: 0){ _LL4481: _temp4480=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4470)->f1; if(( unsigned int)
_temp4480 >  1u?*(( int*) _temp4480) ==  Cyc_Absyn_Upper_b: 0){ goto _LL4475;}
else{ goto _LL4476;}} else{ goto _LL4476;} _LL4476: goto _LL4477; _LL4473:
return 1; _LL4475: { void* _temp4482=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp4452))->v; int
_temp4488; _LL4484: if(( unsigned int) _temp4482 >  1u?*(( int*) _temp4482) == 
Cyc_Absyn_Eq_constr: 0){ _LL4489: _temp4488=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4482)->f1; goto _LL4485;} else{ goto _LL4486;} _LL4486: goto _LL4487;
_LL4485: return _temp4488; _LL4487: return 0; _LL4483:;} _LL4477: return 0;
_LL4471:;} _LL4431: return Cyc_Tcutil_supports_default( _temp4454); _LL4433:
for( 0; _temp4456 !=  0; _temp4456= _temp4456->tl){ if( ! Cyc_Tcutil_supports_default((*((
struct _tuple4*) _temp4456->hd)).f2)){ return 0;}} return 1; _LL4435: if(
_temp4458 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=* _temp4458; if(
sd->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs,
_temp4460,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);}
_LL4437: if( _temp4462 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*
_temp4462; if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp4464,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL4439: _temp4468= _temp4466; goto _LL4441; _LL4441: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp4468); _LL4443: goto _LL4445;
_LL4445: return 1; _LL4447: return 0; _LL4419:;} static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List* tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{
struct _RegionHandle _temp4490= _new_region(); struct _RegionHandle* rgn=&
_temp4490; _push_region( rgn);{ struct Cyc_List_List* _temp4491=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0;
fs= fs->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4491,( void*)(( struct
Cyc_Absyn_Structfield*) fs->hd)->type); if( ! Cyc_Tcutil_supports_default( t)){
int _temp4492= 0; _npop_handler( 0u); return _temp4492;}}}; _pop_region( rgn);}
return 1;}
