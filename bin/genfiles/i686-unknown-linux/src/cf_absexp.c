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
extern struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_ptrcmp(
void**, void**); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct
Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct _tagged_arr f1; } ;
extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern unsigned char
Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u];
struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{
int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_Std_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct
Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict* d, void* k, void** ans); extern void Cyc_Dict_iter_c( void(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern void* Cyc_Dict_fold2_c(
void*(* f)( void*, void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2, void* accum); extern struct Cyc_Dict_Dict* Cyc_Dict_map(
void*(* f)( void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt);
extern struct Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple0{ void* f1;
struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_SwitchC_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Static= 0;
static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; int inject; } ; struct Cyc_Absyn_FnInfo{
struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct
Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs;
struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
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
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
22; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 23; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
24; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 31; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
32; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 33; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 34; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 35; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
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
15u]; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
struct Cyc_CfFlowInfo_Place; enum  Cyc_CfFlowInfo_Escaped; struct Cyc_CfFlowInfo_InitState;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ; enum 
Cyc_CfFlowInfo_Escaped{ Cyc_CfFlowInfo_Esc  =  0u, Cyc_CfFlowInfo_Unesc  =  1u};
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_MustPointTo=
0; struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; struct Cyc_CfFlowInfo_InitState{ enum  Cyc_CfFlowInfo_Escaped esc; void*
level; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; struct Cyc_CfFlowInfo_InitState f1; } ; static const int Cyc_CfFlowInfo_DictPI=
1; struct Cyc_CfFlowInfo_DictPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set(); extern void* Cyc_CfFlowInfo_mkLeafPI(
enum  Cyc_CfFlowInfo_Escaped esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed); extern
void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**, void*, void*); extern void* Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**, void*, void*, struct Cyc_Set_Set*, struct Cyc_Set_Set*);
extern int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; struct Cyc_Set_Set**
all_changed; } ; extern void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv*
env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt( struct
Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern
unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]; static const int Cyc_CfAbsexp_OrderedG=
0; static const int Cyc_CfAbsexp_UnorderedG= 1; static const int Cyc_CfAbsexp_OneofG=
2; extern void Cyc_CfAbsexp_check_absexp( void*); extern struct _tagged_arr Cyc_CfAbsexp_absexp2string(
void* ae, int depth); extern void* Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl*); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp*);
extern void* Cyc_CfAbsexp_mkDictOp( struct Cyc_Dict_Dict*); extern void* Cyc_CfAbsexp_mkAddressOp(
void*); extern void* Cyc_CfAbsexp_mkMemberOp( void*, struct _tagged_arr* f);
extern void* Cyc_CfAbsexp_mkDerefOp( void*); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkComprehensionAE( struct Cyc_Absyn_Vardecl* vd,
void* size_true_exp, void* size_false_exp, void* size_aop, void* body_exp, void*
body_op); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2);
extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael);
extern void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, void* ae, void* in_flow); unsigned char Cyc_CfAbsexp_BadAbsexp[
14u]="\000\000\000\000BadAbsexp"; static const int Cyc_CfAbsexp_BottomAE= 0;
static const int Cyc_CfAbsexp_SkipAE= 1; static const int Cyc_CfAbsexp_UseAE= 0;
struct Cyc_CfAbsexp_UseAE_struct{ int tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE=
1; struct Cyc_CfAbsexp_AssignAE_struct{ int tag; void* f1; void* f2; } ; static
const int Cyc_CfAbsexp_MallocAE= 2; struct Cyc_CfAbsexp_MallocAE_struct{ int tag;
void* f1; int f2; } ; static const int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_ComprehensionAE=
4; struct Cyc_CfAbsexp_ComprehensionAE_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; void* f2; void* f3; void* f4; void* f5; void* f6; } ; static const int Cyc_CfAbsexp_GroupAE=
5; struct Cyc_CfAbsexp_GroupAE_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_CfAbsexp_UnknownOp= 0; static const int Cyc_CfAbsexp_LocalOp=
0; struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; }
; static const int Cyc_CfAbsexp_MallocOp= 1; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static const int Cyc_CfAbsexp_DictOp=
2; struct Cyc_CfAbsexp_DictOp_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfAbsexp_AddressOp= 3; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_DerefOp= 4; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_MemberOp= 5; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static int Cyc_CfAbsexp_num_mallocpts=
0; static struct Cyc_Dict_Dict** Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int(
struct Cyc_Absyn_Exp* e){ if( Cyc_CfAbsexp_mallocpt_dict ==  0){ struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)))
Cyc_Dict_empty)(( int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp);
Cyc_CfAbsexp_mallocpt_dict=({ struct Cyc_Dict_Dict** _temp0=( struct Cyc_Dict_Dict**)
_cycalloc( sizeof( struct Cyc_Dict_Dict*) *  1); _temp0[ 0]= d; _temp0;});}{ int
i= 0; if( !(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* k, int* ans))
Cyc_Dict_lookup_bool)(*(( struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict)),
e,& i)){*(( struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict))=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* k, int
v)) Cyc_Dict_insert)(*(( struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict)),
e,( i= ++ Cyc_CfAbsexp_num_mallocpts));} return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){
return( void*) Cyc_CfAbsexp_UnknownOp;} void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl*
vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct* _temp1=( struct Cyc_CfAbsexp_LocalOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct)); _temp1[ 0]=({ struct
Cyc_CfAbsexp_LocalOp_struct _temp2; _temp2.tag= Cyc_CfAbsexp_LocalOp; _temp2.f1=
vd; _temp2;}); _temp1;});} void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp*
e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct* _temp3=( struct Cyc_CfAbsexp_MallocOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct)); _temp3[ 0]=({ struct
Cyc_CfAbsexp_MallocOp_struct _temp4; _temp4.tag= Cyc_CfAbsexp_MallocOp; _temp4.f1=
e; _temp4.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp4;}); _temp3;});} void* Cyc_CfAbsexp_mkDictOp(
struct Cyc_Dict_Dict* d){ return( void*)({ struct Cyc_CfAbsexp_DictOp_struct*
_temp5=( struct Cyc_CfAbsexp_DictOp_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_DictOp_struct));
_temp5[ 0]=({ struct Cyc_CfAbsexp_DictOp_struct _temp6; _temp6.tag= Cyc_CfAbsexp_DictOp;
_temp6.f1= d; _temp6;}); _temp5;});} void* Cyc_CfAbsexp_mkAddressOp( void* ao){
void* _temp7= ao; void* _temp17; _LL9: if( _temp7 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL10;} else{ goto _LL11;} _LL11: if(( unsigned int) _temp7 >  1u?*(( int*)
_temp7) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL18: _temp17=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp7)->f1; goto _LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp7 > 
1u?*(( int*) _temp7) ==  Cyc_CfAbsexp_AddressOp: 0){ goto _LL14;} else{ goto
_LL15;} _LL15: goto _LL16; _LL10: return( void*) Cyc_CfAbsexp_UnknownOp; _LL12:
return _temp17; _LL14:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL16:
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp19=( struct Cyc_CfAbsexp_AddressOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp19[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp20; _temp20.tag= Cyc_CfAbsexp_AddressOp;
_temp20.f1=( void*) ao; _temp20;}); _temp19;}); _LL8:;} void* Cyc_CfAbsexp_mkDerefOp(
void* ao){ void* _temp21= ao; void* _temp31; _LL23: if( _temp21 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp21 >  1u?*(( int*)
_temp21) ==  Cyc_CfAbsexp_AddressOp: 0){ _LL32: _temp31=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp21)->f1; goto _LL26;} else{ goto _LL27;} _LL27: if(( unsigned int) _temp21
>  1u?*(( int*) _temp21) ==  Cyc_CfAbsexp_MallocOp: 0){ goto _LL28;} else{ goto
_LL29;} _LL29: goto _LL30; _LL24: return( void*) Cyc_CfAbsexp_UnknownOp; _LL26:
return _temp31; _LL28:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL30:
return( void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp33=( struct Cyc_CfAbsexp_DerefOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp33[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp34; _temp34.tag= Cyc_CfAbsexp_DerefOp; _temp34.f1=(
void*) ao; _temp34;}); _temp33;}); _LL22:;} void* Cyc_CfAbsexp_mkMemberOp( void*
ao, struct _tagged_arr* f){ void* _temp35= ao; _LL37: if( _temp35 == ( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL38;} else{ goto _LL39;} _LL39: if((
unsigned int) _temp35 >  1u?*(( int*) _temp35) ==  Cyc_CfAbsexp_AddressOp: 0){
goto _LL40;} else{ goto _LL41;} _LL41: goto _LL42; _LL38: return( void*) Cyc_CfAbsexp_UnknownOp;
_LL40:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL42: return( void*)({
struct Cyc_CfAbsexp_MemberOp_struct* _temp43=( struct Cyc_CfAbsexp_MemberOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct)); _temp43[ 0]=({ struct
Cyc_CfAbsexp_MemberOp_struct _temp44; _temp44.tag= Cyc_CfAbsexp_MemberOp;
_temp44.f1=( void*) ao; _temp44.f2= f; _temp44;}); _temp43;}); _LL36:;} void*
Cyc_CfAbsexp_mkBottomAE(){ return( void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){
return( void*) Cyc_CfAbsexp_SkipAE;} void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp*
e){ return( void*)({ struct Cyc_CfAbsexp_MallocAE_struct* _temp45=( struct Cyc_CfAbsexp_MallocAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp45[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp46; _temp46.tag= Cyc_CfAbsexp_MallocAE;
_temp46.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp47=(
struct Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp47[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp48; _temp48.tag= Cyc_CfFlowInfo_MallocPt;
_temp48.f1= e; _temp48;}); _temp47;})); _temp46.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp46;}); _temp45;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp49=( struct Cyc_CfAbsexp_StmtAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp49[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp50; _temp50.tag= Cyc_CfAbsexp_StmtAE; _temp50.f1=
s; _temp50;}); _temp49;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp51=
ao; _LL53: if( _temp51 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL54;} else{
goto _LL55;} _LL55: goto _LL56; _LL54: return( void*) Cyc_CfAbsexp_SkipAE; _LL56:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp57=( struct Cyc_CfAbsexp_UseAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp57[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp58; _temp58.tag= Cyc_CfAbsexp_UseAE; _temp58.f1=( void*) ao; _temp58;});
_temp57;}); _LL52:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp60=({ struct _tuple3 _temp59; _temp59.f1=
l; _temp59.f2= r; _temp59;}); void* _temp68; void* _temp70; void* _temp72; _LL62:
_LL71: _temp70= _temp60.f1; if( _temp70 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL69;} else{ goto _LL64;} _LL69: _temp68= _temp60.f2; if( _temp68 == (
void*) Cyc_CfAbsexp_UnknownOp){ goto _LL63;} else{ goto _LL64;} _LL64: _LL73:
_temp72= _temp60.f1; if( _temp72 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL65;}
else{ goto _LL66;} _LL66: goto _LL67; _LL63: return( void*) Cyc_CfAbsexp_SkipAE;
_LL65: return Cyc_CfAbsexp_mkUseAE( r); _LL67: return( void*)({ struct Cyc_CfAbsexp_AssignAE_struct*
_temp74=( struct Cyc_CfAbsexp_AssignAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct));
_temp74[ 0]=({ struct Cyc_CfAbsexp_AssignAE_struct _temp75; _temp75.tag= Cyc_CfAbsexp_AssignAE;
_temp75.f1=( void*) l; _temp75.f2=( void*) r; _temp75;}); _temp74;}); _LL61:;}
void* Cyc_CfAbsexp_mkComprehensionAE( struct Cyc_Absyn_Vardecl* vd, void*
size_true_exp, void* size_false_exp, void* size_aop, void* body_exp, void*
body_op){ return( void*)({ struct Cyc_CfAbsexp_ComprehensionAE_struct* _temp76=(
struct Cyc_CfAbsexp_ComprehensionAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_ComprehensionAE_struct));
_temp76[ 0]=({ struct Cyc_CfAbsexp_ComprehensionAE_struct _temp77; _temp77.tag=
Cyc_CfAbsexp_ComprehensionAE; _temp77.f1= vd; _temp77.f2=( void*) size_true_exp;
_temp77.f3=( void*) size_false_exp; _temp77.f4=( void*) size_aop; _temp77.f5=(
void*) body_exp; _temp77.f6=( void*) body_op; _temp77;}); _temp76;});} struct
_tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE( void* g,
void* ae1, void* ae2){ struct _tuple4 _temp79=({ struct _tuple4 _temp78; _temp78.f1=
g; _temp78.f2= ae1; _temp78.f3= ae2; _temp78;}); void* _temp102; void* _temp105;
void* _temp107; void* _temp109; void* _temp111; void* _temp113; void* _temp115;
void* _temp117; void* _temp120; struct Cyc_List_List* _temp122; void* _temp124;
void* _temp126; struct Cyc_List_List* _temp128; void* _temp130; void* _temp132;
void* _temp135; struct Cyc_List_List* _temp137; void* _temp139; void* _temp141;
void* _temp143; struct Cyc_List_List* _temp145; void* _temp147; void* _temp149;
void* _temp152; struct Cyc_List_List* _temp154; void* _temp156; void* _temp158;
_LL81: _LL103: _temp102= _temp79.f3; if( _temp102 == ( void*) Cyc_CfAbsexp_SkipAE){
goto _LL101;} else{ goto _LL83;} _LL101: if( g != ( void*) Cyc_CfAbsexp_OneofG){
goto _LL82;} else{ goto _LL83;} _LL83: _LL106: _temp105= _temp79.f2; if(
_temp105 == ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL104;} else{ goto _LL85;}
_LL104: if( g != ( void*) Cyc_CfAbsexp_OneofG){ goto _LL84;} else{ goto _LL85;}
_LL85: _LL110: _temp109= _temp79.f1; if( _temp109 == ( void*) Cyc_CfAbsexp_OneofG){
goto _LL108;} else{ goto _LL87;} _LL108: _temp107= _temp79.f3; if( _temp107 == (
void*) Cyc_CfAbsexp_BottomAE){ goto _LL86;} else{ goto _LL87;} _LL87: _LL114:
_temp113= _temp79.f1; if( _temp113 == ( void*) Cyc_CfAbsexp_OneofG){ goto _LL112;}
else{ goto _LL89;} _LL112: _temp111= _temp79.f2; if( _temp111 == ( void*) Cyc_CfAbsexp_BottomAE){
goto _LL88;} else{ goto _LL89;} _LL89: _LL118: _temp117= _temp79.f1; if(
_temp117 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL116;} else{ goto _LL91;}
_LL116: _temp115= _temp79.f2; if( _temp115 == ( void*) Cyc_CfAbsexp_BottomAE){
goto _LL90;} else{ goto _LL91;} _LL91: _LL133: _temp132= _temp79.f1; goto _LL127;
_LL127: _temp126= _temp79.f2; if(( unsigned int) _temp126 >  2u?*(( int*)
_temp126) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL131: _temp130=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp126)->f1; goto _LL129; _LL129: _temp128=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp126)->f2; goto _LL121;} else{ goto _LL93;} _LL121: _temp120= _temp79.f3;
if(( unsigned int) _temp120 >  2u?*(( int*) _temp120) ==  Cyc_CfAbsexp_GroupAE:
0){ _LL125: _temp124=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp120)->f1;
goto _LL123; _LL123: _temp122=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp120)->f2;
goto _LL119;} else{ goto _LL93;} _LL119: if( _temp132 ==  _temp130? _temp132 == 
_temp124: 0){ goto _LL92;} else{ goto _LL93;} _LL93: _LL142: _temp141= _temp79.f1;
goto _LL136; _LL136: _temp135= _temp79.f2; if(( unsigned int) _temp135 >  2u?*((
int*) _temp135) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL140: _temp139=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp135)->f1; goto _LL138; _LL138: _temp137=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp135)->f2; goto _LL134;} else{ goto
_LL95;} _LL134: if( _temp141 ==  _temp139? _temp141 != ( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL94;} else{ goto _LL95;} _LL95: _LL150: _temp149= _temp79.f1; if(
_temp149 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL144;} else{ goto _LL97;}
_LL144: _temp143= _temp79.f2; if(( unsigned int) _temp143 >  2u?*(( int*)
_temp143) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL148: _temp147=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp143)->f1; if( _temp147 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL146;}
else{ goto _LL97;} _LL146: _temp145=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp143)->f2; goto _LL96;} else{ goto _LL97;} _LL97: _LL159: _temp158= _temp79.f1;
goto _LL153; _LL153: _temp152= _temp79.f3; if(( unsigned int) _temp152 >  2u?*((
int*) _temp152) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL157: _temp156=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp152)->f1; goto _LL155; _LL155: _temp154=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp152)->f2; goto _LL151;} else{ goto
_LL99;} _LL151: if( _temp158 ==  _temp156){ goto _LL98;} else{ goto _LL99;}
_LL99: goto _LL100; _LL82: return ae1; _LL84: return ae2; _LL86: return ae1;
_LL88: return ae2; _LL90: return( void*) Cyc_CfAbsexp_BottomAE; _LL92: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp160=( struct Cyc_CfAbsexp_GroupAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp160[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp161; _temp161.tag= Cyc_CfAbsexp_GroupAE;
_temp161.f1=( void*) _temp132; _temp161.f2= Cyc_List_append( _temp128, _temp122);
_temp161;}); _temp160;}); _LL94: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp162=( struct Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp162[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp163; _temp163.tag= Cyc_CfAbsexp_GroupAE;
_temp163.f1=( void*) _temp141; _temp163.f2=({ struct Cyc_List_List* _temp164=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp164->hd=(
void*) ae2; _temp164->tl= _temp137; _temp164;}); _temp163;}); _temp162;}); _LL96:
return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp165=( struct Cyc_CfAbsexp_GroupAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp165[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp166; _temp166.tag= Cyc_CfAbsexp_GroupAE;
_temp166.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp166.f2= Cyc_List_append(
_temp145,({ struct Cyc_List_List* _temp167=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp167->hd=( void*) ae2; _temp167->tl= 0;
_temp167;})); _temp166;}); _temp165;}); _LL98: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp168=( struct Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp168[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp169; _temp169.tag= Cyc_CfAbsexp_GroupAE;
_temp169.f1=( void*) _temp158; _temp169.f2=({ struct Cyc_List_List* _temp170=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp170->hd=(
void*) ae1; _temp170->tl= _temp154; _temp170;}); _temp169;}); _temp168;});
_LL100: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp171=( struct
Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp171[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp172; _temp172.tag= Cyc_CfAbsexp_GroupAE;
_temp172.f1=( void*) g; _temp172.f2=({ struct Cyc_List_List* _temp173=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp173->hd=( void*)
ae1; _temp173->tl=({ struct Cyc_List_List* _temp174=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp174->hd=( void*) ae2; _temp174->tl=
0; _temp174;}); _temp173;}); _temp172;}); _temp171;}); _LL80:;} void* Cyc_CfAbsexp_mkGroupAE_l(
void* g, struct Cyc_List_List* ael){ if( ael ==  0){ return( void*) Cyc_CfAbsexp_SkipAE;}
if((( struct Cyc_List_List*) _check_null( ael))->tl ==  0){ return( void*)((
struct Cyc_List_List*) _check_null( ael))->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp175=( struct Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp175[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp176; _temp176.tag= Cyc_CfAbsexp_GroupAE;
_temp176.f1=( void*) g; _temp176.f2= ael; _temp176;}); _temp175;});} struct
_tagged_arr Cyc_CfAbsexp_absop2string( void* ao); void Cyc_CfAbsexp_field2string(
struct _tagged_arr* accum, struct _tagged_arr* f, void* ao){* accum=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp180; _temp180.tag= Cyc_Std_String_pa;
_temp180.f1=( struct _tagged_arr)* accum;{ struct Cyc_Std_String_pa_struct
_temp179; _temp179.tag= Cyc_Std_String_pa; _temp179.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
ao);{ struct Cyc_Std_String_pa_struct _temp178; _temp178.tag= Cyc_Std_String_pa;
_temp178.f1=( struct _tagged_arr)* f;{ void* _temp177[ 3u]={& _temp178,&
_temp179,& _temp180}; Cyc_Std_aprintf( _tag_arr(" (%s: %s) %s ", sizeof(
unsigned char), 14u), _tag_arr( _temp177, sizeof( void*), 3u));}}}});} struct
_tagged_arr Cyc_CfAbsexp_absop2string( void* ao){ void* _temp181= ao; void*
_temp197; struct Cyc_Absyn_Vardecl* _temp199; struct _tagged_arr* _temp201; void*
_temp203; int _temp205; struct Cyc_Absyn_Exp* _temp207; void* _temp209; struct
Cyc_Dict_Dict* _temp211; _LL183: if( _temp181 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL184;} else{ goto _LL185;} _LL185: if(( unsigned int) _temp181 >  1u?*((
int*) _temp181) ==  Cyc_CfAbsexp_AddressOp: 0){ _LL198: _temp197=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp181)->f1; goto _LL186;} else{ goto
_LL187;} _LL187: if(( unsigned int) _temp181 >  1u?*(( int*) _temp181) ==  Cyc_CfAbsexp_LocalOp:
0){ _LL200: _temp199=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp181)->f1; goto
_LL188;} else{ goto _LL189;} _LL189: if(( unsigned int) _temp181 >  1u?*(( int*)
_temp181) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL204: _temp203=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp181)->f1; goto _LL202; _LL202: _temp201=(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp181)->f2; goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int)
_temp181 >  1u?*(( int*) _temp181) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL208:
_temp207=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp181)->f1; goto _LL206;
_LL206: _temp205=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp181)->f2; goto
_LL192;} else{ goto _LL193;} _LL193: if(( unsigned int) _temp181 >  1u?*(( int*)
_temp181) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL210: _temp209=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp181)->f1; goto _LL194;} else{ goto _LL195;} _LL195: if(( unsigned int)
_temp181 >  1u?*(( int*) _temp181) ==  Cyc_CfAbsexp_DictOp: 0){ _LL212: _temp211=((
struct Cyc_CfAbsexp_DictOp_struct*) _temp181)->f1; goto _LL196;} else{ goto
_LL182;} _LL184: return _tag_arr("Unknown", sizeof( unsigned char), 8u); _LL186:
return( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp214; _temp214.tag=
Cyc_Std_String_pa; _temp214.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp197);{ void* _temp213[ 1u]={& _temp214}; Cyc_Std_aprintf( _tag_arr("& %s",
sizeof( unsigned char), 5u), _tag_arr( _temp213, sizeof( void*), 1u));}});
_LL188: return*(* _temp199->name).f2; _LL190: return( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp217; _temp217.tag= Cyc_Std_String_pa;
_temp217.f1=( struct _tagged_arr)* _temp201;{ struct Cyc_Std_String_pa_struct
_temp216; _temp216.tag= Cyc_Std_String_pa; _temp216.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp203);{ void* _temp215[ 2u]={& _temp216,& _temp217}; Cyc_Std_aprintf(
_tag_arr("%s.%s", sizeof( unsigned char), 6u), _tag_arr( _temp215, sizeof( void*),
2u));}}}); _LL192: return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp219; _temp219.tag= Cyc_Std_Int_pa; _temp219.f1=( int)(( unsigned int)
_temp205);{ void* _temp218[ 1u]={& _temp219}; Cyc_Std_aprintf( _tag_arr("<mpt%d>",
sizeof( unsigned char), 8u), _tag_arr( _temp218, sizeof( void*), 1u));}});
_LL194: return( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp221;
_temp221.tag= Cyc_Std_String_pa; _temp221.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp209);{ void* _temp220[ 1u]={& _temp221}; Cyc_Std_aprintf( _tag_arr("(*%s)",
sizeof( unsigned char), 6u), _tag_arr( _temp220, sizeof( void*), 1u));}});
_LL196: { struct _tagged_arr accum= _tag_arr("", sizeof( unsigned char), 1u);((
void(*)( void(* f)( struct _tagged_arr*, struct _tagged_arr*, void*), struct
_tagged_arr* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_CfAbsexp_field2string,&
accum, _temp211); return accum;} _LL182:;} struct _tagged_arr Cyc_CfAbsexp_absexp2string(
void* ae, int depth){ struct _tagged_arr ans= Cyc_Core_new_string( depth);{ int
i= 0; for( 0; i <  _get_arr_size( ans, sizeof( unsigned char)); ++ i){*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), i))=' ';}}{
void* _temp222= ae; void* _temp242; int _temp244; void* _temp246; struct Cyc_Absyn_Exp*
_temp248; void* _temp250; void* _temp252; struct Cyc_List_List* _temp254; void*
_temp256; struct Cyc_Absyn_Stmt* _temp258; _LL224: if( _temp222 == ( void*) Cyc_CfAbsexp_SkipAE){
goto _LL225;} else{ goto _LL226;} _LL226: if(( unsigned int) _temp222 >  2u?*((
int*) _temp222) ==  Cyc_CfAbsexp_UseAE: 0){ _LL243: _temp242=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp222)->f1; goto _LL227;} else{ goto _LL228;}
_LL228: if(( unsigned int) _temp222 >  2u?*(( int*) _temp222) ==  Cyc_CfAbsexp_MallocAE:
0){ _LL247: _temp246=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp222)->f1;
if(*(( int*) _temp246) ==  Cyc_CfFlowInfo_MallocPt){ _LL249: _temp248=(( struct
Cyc_CfFlowInfo_MallocPt_struct*) _temp246)->f1; goto _LL245;} else{ goto _LL230;}
_LL245: _temp244=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp222)->f2; goto
_LL229;} else{ goto _LL230;} _LL230: if(( unsigned int) _temp222 >  2u?*(( int*)
_temp222) ==  Cyc_CfAbsexp_AssignAE: 0){ _LL253: _temp252=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp222)->f1; goto _LL251; _LL251: _temp250=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp222)->f2; goto _LL231;} else{ goto _LL232;} _LL232: if(( unsigned int)
_temp222 >  2u?*(( int*) _temp222) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL257:
_temp256=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp222)->f1; goto
_LL255; _LL255: _temp254=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp222)->f2;
goto _LL233;} else{ goto _LL234;} _LL234: if(( unsigned int) _temp222 >  2u?*((
int*) _temp222) ==  Cyc_CfAbsexp_StmtAE: 0){ _LL259: _temp258=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp222)->f1; goto _LL235;} else{ goto _LL236;} _LL236: if( _temp222 == ( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL237;} else{ goto _LL238;} _LL238: if((
unsigned int) _temp222 >  2u?*(( int*) _temp222) ==  Cyc_CfAbsexp_ComprehensionAE:
0){ goto _LL239;} else{ goto _LL240;} _LL240: goto _LL241; _LL225: ans=({ struct
Cyc_Std_String_pa_struct _temp261; _temp261.tag= Cyc_Std_String_pa; _temp261.f1=(
struct _tagged_arr) ans;{ void* _temp260[ 1u]={& _temp261}; Cyc_Std_aprintf(
_tag_arr("%sskip", sizeof( unsigned char), 7u), _tag_arr( _temp260, sizeof( void*),
1u));}}); goto _LL223; _LL227: ans=({ struct Cyc_Std_String_pa_struct _temp264;
_temp264.tag= Cyc_Std_String_pa; _temp264.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp242);{ struct Cyc_Std_String_pa_struct _temp263; _temp263.tag= Cyc_Std_String_pa;
_temp263.f1=( struct _tagged_arr) ans;{ void* _temp262[ 2u]={& _temp263,&
_temp264}; Cyc_Std_aprintf( _tag_arr("%suse %s", sizeof( unsigned char), 9u),
_tag_arr( _temp262, sizeof( void*), 2u));}}}); goto _LL223; _LL229: ans=({
struct Cyc_Std_Int_pa_struct _temp267; _temp267.tag= Cyc_Std_Int_pa; _temp267.f1=(
int)(( unsigned int) _temp244);{ struct Cyc_Std_String_pa_struct _temp266;
_temp266.tag= Cyc_Std_String_pa; _temp266.f1=( struct _tagged_arr) ans;{ void*
_temp265[ 2u]={& _temp266,& _temp267}; Cyc_Std_aprintf( _tag_arr("%smalloc %i",
sizeof( unsigned char), 12u), _tag_arr( _temp265, sizeof( void*), 2u));}}});
goto _LL223; _LL231: ans=({ struct Cyc_Std_String_pa_struct _temp271; _temp271.tag=
Cyc_Std_String_pa; _temp271.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp250);{ struct Cyc_Std_String_pa_struct _temp270; _temp270.tag= Cyc_Std_String_pa;
_temp270.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp252);{ struct
Cyc_Std_String_pa_struct _temp269; _temp269.tag= Cyc_Std_String_pa; _temp269.f1=(
struct _tagged_arr) ans;{ void* _temp268[ 3u]={& _temp269,& _temp270,& _temp271};
Cyc_Std_aprintf( _tag_arr("%sassign %s %s", sizeof( unsigned char), 15u),
_tag_arr( _temp268, sizeof( void*), 3u));}}}}); goto _LL223; _LL233:{ void*
_temp272= _temp256; _LL274: if( _temp272 == ( void*) Cyc_CfAbsexp_OrderedG){
goto _LL275;} else{ goto _LL276;} _LL276: if( _temp272 == ( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL277;} else{ goto _LL278;} _LL278: if( _temp272 == ( void*) Cyc_CfAbsexp_OneofG){
goto _LL279;} else{ goto _LL273;} _LL275: ans=({ struct Cyc_Std_String_pa_struct
_temp281; _temp281.tag= Cyc_Std_String_pa; _temp281.f1=( struct _tagged_arr) ans;{
void* _temp280[ 1u]={& _temp281}; Cyc_Std_aprintf( _tag_arr("%sordered", sizeof(
unsigned char), 10u), _tag_arr( _temp280, sizeof( void*), 1u));}}); goto _LL273;
_LL277: ans=({ struct Cyc_Std_String_pa_struct _temp283; _temp283.tag= Cyc_Std_String_pa;
_temp283.f1=( struct _tagged_arr) ans;{ void* _temp282[ 1u]={& _temp283}; Cyc_Std_aprintf(
_tag_arr("%sunordered", sizeof( unsigned char), 12u), _tag_arr( _temp282,
sizeof( void*), 1u));}}); goto _LL273; _LL279: ans=({ struct Cyc_Std_String_pa_struct
_temp285; _temp285.tag= Cyc_Std_String_pa; _temp285.f1=( struct _tagged_arr) ans;{
void* _temp284[ 1u]={& _temp285}; Cyc_Std_aprintf( _tag_arr("%soneof", sizeof(
unsigned char), 8u), _tag_arr( _temp284, sizeof( void*), 1u));}}); goto _LL273;
_LL273:;} for( 0; _temp254 !=  0; _temp254=(( struct Cyc_List_List*) _check_null(
_temp254))->tl){ ans=({ struct Cyc_Std_String_pa_struct _temp288; _temp288.tag=
Cyc_Std_String_pa; _temp288.f1=( struct _tagged_arr) Cyc_CfAbsexp_absexp2string((
void*)(( struct Cyc_List_List*) _check_null( _temp254))->hd, depth +  1);{
struct Cyc_Std_String_pa_struct _temp287; _temp287.tag= Cyc_Std_String_pa;
_temp287.f1=( struct _tagged_arr) ans;{ void* _temp286[ 2u]={& _temp287,&
_temp288}; Cyc_Std_aprintf( _tag_arr("%s\n%s", sizeof( unsigned char), 6u),
_tag_arr( _temp286, sizeof( void*), 2u));}}});} goto _LL223; _LL235: ans=({
struct Cyc_Std_String_pa_struct _temp290; _temp290.tag= Cyc_Std_String_pa;
_temp290.f1=( struct _tagged_arr) ans;{ void* _temp289[ 1u]={& _temp290}; Cyc_Std_aprintf(
_tag_arr("%s<<stmt>>", sizeof( unsigned char), 11u), _tag_arr( _temp289, sizeof(
void*), 1u));}}); goto _LL223; _LL237: ans=({ struct Cyc_Std_String_pa_struct
_temp292; _temp292.tag= Cyc_Std_String_pa; _temp292.f1=( struct _tagged_arr) ans;{
void* _temp291[ 1u]={& _temp292}; Cyc_Std_aprintf( _tag_arr("%sbottom", sizeof(
unsigned char), 9u), _tag_arr( _temp291, sizeof( void*), 1u));}}); goto _LL223;
_LL239: ans=({ struct Cyc_Std_String_pa_struct _temp294; _temp294.tag= Cyc_Std_String_pa;
_temp294.f1=( struct _tagged_arr) ans;{ void* _temp293[ 1u]={& _temp294}; Cyc_Std_aprintf(
_tag_arr("%scomprehension", sizeof( unsigned char), 16u), _tag_arr( _temp293,
sizeof( void*), 1u));}}); goto _LL223; _LL241:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL223:;} return( struct _tagged_arr) ans;} static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp295= ao; _LL297: if(( unsigned int) _temp295 >  1u?*(( int*)
_temp295) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL298;} else{ goto _LL299;} _LL299:
if(( unsigned int) _temp295 >  1u?*(( int*) _temp295) ==  Cyc_CfAbsexp_MemberOp:
0){ goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int) _temp295 >  1u?*((
int*) _temp295) ==  Cyc_CfAbsexp_MallocOp: 0){ goto _LL302;} else{ goto _LL303;}
_LL303: goto _LL304; _LL298: goto _LL300; _LL300: goto _LL302; _LL302: return;
_LL304:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL296:;} static void Cyc_CfAbsexp_ok_member_arg(
void* ao){ void* _temp305= ao; _LL307: if(( unsigned int) _temp305 >  1u?*(( int*)
_temp305) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL308;} else{ goto _LL309;} _LL309:
if(( unsigned int) _temp305 >  1u?*(( int*) _temp305) ==  Cyc_CfAbsexp_MemberOp:
0){ goto _LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp305 >  1u?*((
int*) _temp305) ==  Cyc_CfAbsexp_DictOp: 0){ goto _LL312;} else{ goto _LL313;}
_LL313: if(( unsigned int) _temp305 >  1u?*(( int*) _temp305) ==  Cyc_CfAbsexp_MallocOp:
0){ goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int) _temp305 >  1u?*((
int*) _temp305) ==  Cyc_CfAbsexp_DerefOp: 0){ goto _LL316;} else{ goto _LL317;}
_LL317: goto _LL318; _LL308: goto _LL310; _LL310: goto _LL312; _LL312: goto
_LL314; _LL314: goto _LL316; _LL316: return; _LL318:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL306:;} static void Cyc_CfAbsexp_ok_deref_arg( void* ao){ void* _temp319= ao;
_LL321: if(( unsigned int) _temp319 >  1u?*(( int*) _temp319) ==  Cyc_CfAbsexp_LocalOp:
0){ goto _LL322;} else{ goto _LL323;} _LL323: if(( unsigned int) _temp319 >  1u?*((
int*) _temp319) ==  Cyc_CfAbsexp_MemberOp: 0){ goto _LL324;} else{ goto _LL325;}
_LL325: if(( unsigned int) _temp319 >  1u?*(( int*) _temp319) ==  Cyc_CfAbsexp_DerefOp:
0){ goto _LL326;} else{ goto _LL327;} _LL327: goto _LL328; _LL322: goto _LL324;
_LL324: goto _LL326; _LL326: return; _LL328:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL320:;} static void Cyc_CfAbsexp_ok_lvalue( void* ao){ void* _temp329= ao;
_LL331: if( _temp329 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL332;} else{
goto _LL333;} _LL333: if(( unsigned int) _temp329 >  1u?*(( int*) _temp329) == 
Cyc_CfAbsexp_DictOp: 0){ goto _LL334;} else{ goto _LL335;} _LL335: if((
unsigned int) _temp329 >  1u?*(( int*) _temp329) ==  Cyc_CfAbsexp_AddressOp: 0){
goto _LL336;} else{ goto _LL337;} _LL337: goto _LL338; _LL332: goto _LL334;
_LL334: goto _LL336; _LL336:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL338: return; _LL330:;} static void Cyc_CfAbsexp_check_absop( void* ao){ void*
_temp339= ao; void* _temp355; void* _temp357; void* _temp359; _LL341: if(
_temp339 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL342;} else{ goto _LL343;}
_LL343: if(( unsigned int) _temp339 >  1u?*(( int*) _temp339) ==  Cyc_CfAbsexp_AddressOp:
0){ _LL356: _temp355=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp339)->f1;
goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int) _temp339 >  1u?*((
int*) _temp339) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL358: _temp357=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp339)->f1; goto _LL346;} else{ goto _LL347;}
_LL347: if(( unsigned int) _temp339 >  1u?*(( int*) _temp339) ==  Cyc_CfAbsexp_MemberOp:
0){ _LL360: _temp359=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp339)->f1;
goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp339 >  1u?*((
int*) _temp339) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL350;} else{ goto _LL351;}
_LL351: if(( unsigned int) _temp339 >  1u?*(( int*) _temp339) ==  Cyc_CfAbsexp_MallocOp:
0){ goto _LL352;} else{ goto _LL353;} _LL353: if(( unsigned int) _temp339 >  1u?*((
int*) _temp339) ==  Cyc_CfAbsexp_DictOp: 0){ goto _LL354;} else{ goto _LL340;}
_LL342: return; _LL344: Cyc_CfAbsexp_check_absop( _temp355); Cyc_CfAbsexp_ok_address_arg(
_temp355); return; _LL346: Cyc_CfAbsexp_check_absop( _temp357); Cyc_CfAbsexp_ok_deref_arg(
_temp357); return; _LL348: Cyc_CfAbsexp_check_absop( _temp359); Cyc_CfAbsexp_ok_member_arg(
_temp359); return; _LL350: return; _LL352: return; _LL354: return; _LL340:;}
void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp361= ae; void* _temp381;
struct Cyc_List_List* _temp383; void* _temp385; void* _temp387; void* _temp389;
void* _temp391; void* _temp393; void* _temp395; void* _temp397; void* _temp399;
_LL363: if( _temp361 == ( void*) Cyc_CfAbsexp_BottomAE){ goto _LL364;} else{
goto _LL365;} _LL365: if( _temp361 == ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL366;}
else{ goto _LL367;} _LL367: if(( unsigned int) _temp361 >  2u?*(( int*) _temp361)
==  Cyc_CfAbsexp_MallocAE: 0){ _LL382: _temp381=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp361)->f1; if(*(( int*) _temp381) ==  Cyc_CfFlowInfo_VarRoot){ goto _LL368;}
else{ goto _LL369;}} else{ goto _LL369;} _LL369: if(( unsigned int) _temp361 > 
2u?*(( int*) _temp361) ==  Cyc_CfAbsexp_MallocAE: 0){ goto _LL370;} else{ goto
_LL371;} _LL371: if(( unsigned int) _temp361 >  2u?*(( int*) _temp361) ==  Cyc_CfAbsexp_StmtAE:
0){ goto _LL372;} else{ goto _LL373;} _LL373: if(( unsigned int) _temp361 >  2u?*((
int*) _temp361) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL384: _temp383=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp361)->f2; goto _LL374;} else{ goto _LL375;} _LL375: if(( unsigned int)
_temp361 >  2u?*(( int*) _temp361) ==  Cyc_CfAbsexp_UseAE: 0){ _LL386: _temp385=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp361)->f1; goto _LL376;} else{
goto _LL377;} _LL377: if(( unsigned int) _temp361 >  2u?*(( int*) _temp361) == 
Cyc_CfAbsexp_AssignAE: 0){ _LL390: _temp389=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp361)->f1; goto _LL388; _LL388: _temp387=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp361)->f2; goto _LL378;} else{ goto _LL379;} _LL379: if(( unsigned int)
_temp361 >  2u?*(( int*) _temp361) ==  Cyc_CfAbsexp_ComprehensionAE: 0){ _LL400:
_temp399=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*) _temp361)->f2;
goto _LL398; _LL398: _temp397=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp361)->f3; goto _LL396; _LL396: _temp395=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp361)->f4; goto _LL394; _LL394: _temp393=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp361)->f5; goto _LL392; _LL392: _temp391=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp361)->f6; goto _LL380;} else{ goto _LL362;} _LL364: goto _LL366; _LL366:
return; _LL368:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL370: goto
_LL372; _LL372: return; _LL374: for( 0; _temp383 !=  0; _temp383=(( struct Cyc_List_List*)
_check_null( _temp383))->tl){ Cyc_CfAbsexp_check_absexp(( void*)(( struct Cyc_List_List*)
_check_null( _temp383))->hd);} return; _LL376: Cyc_CfAbsexp_check_absop(
_temp385); return; _LL378: Cyc_CfAbsexp_check_absop( _temp389); Cyc_CfAbsexp_check_absop(
_temp387); Cyc_CfAbsexp_ok_lvalue( _temp389); return; _LL380: Cyc_CfAbsexp_check_absop(
_temp395); Cyc_CfAbsexp_check_absop( _temp391); Cyc_CfAbsexp_check_absexp(
_temp399); Cyc_CfAbsexp_check_absexp( _temp397); Cyc_CfAbsexp_check_absexp(
_temp393); return; _LL362:;} void* Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict*
pinfo_dict, void* ao); struct Cyc_CfFlowInfo_InitState Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp401= ao; struct Cyc_Absyn_Vardecl*
_temp417; struct Cyc_Absyn_Exp* _temp419; struct _tagged_arr* _temp421; void*
_temp423; void* _temp425; _LL403: if(( unsigned int) _temp401 >  1u?*(( int*)
_temp401) ==  Cyc_CfAbsexp_LocalOp: 0){ _LL418: _temp417=(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp401)->f1; goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int)
_temp401 >  1u?*(( int*) _temp401) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL420:
_temp419=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp401)->f1; goto _LL406;}
else{ goto _LL407;} _LL407: if(( unsigned int) _temp401 >  1u?*(( int*) _temp401)
==  Cyc_CfAbsexp_MemberOp: 0){ _LL424: _temp423=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp401)->f1; goto _LL422; _LL422: _temp421=(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp401)->f2; goto _LL408;} else{ goto _LL409;} _LL409: if(( unsigned int)
_temp401 >  1u?*(( int*) _temp401) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL426:
_temp425=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp401)->f1; goto
_LL410;} else{ goto _LL411;} _LL411: if(( unsigned int) _temp401 >  1u?*(( int*)
_temp401) ==  Cyc_CfAbsexp_DictOp: 0){ goto _LL412;} else{ goto _LL413;} _LL413:
if( _temp401 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL414;} else{ goto
_LL415;} _LL415: if(( unsigned int) _temp401 >  1u?*(( int*) _temp401) ==  Cyc_CfAbsexp_AddressOp:
0){ goto _LL416;} else{ goto _LL402;} _LL404: return({ struct Cyc_CfFlowInfo_InitState
_temp427; _temp427.esc= Cyc_CfFlowInfo_Unesc; _temp427.level=( void*)(( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp428=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp428[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp429; _temp429.tag= Cyc_CfFlowInfo_MustPointTo;
_temp429.f1=({ struct Cyc_CfFlowInfo_Place* _temp430=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp430->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp431=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp431[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp432; _temp432.tag= Cyc_CfFlowInfo_VarRoot;
_temp432.f1= _temp417; _temp432;}); _temp431;})); _temp430->fields= 0; _temp430;});
_temp429;}); _temp428;})); _temp427;}); _LL406: return({ struct Cyc_CfFlowInfo_InitState
_temp433; _temp433.esc= Cyc_CfFlowInfo_Unesc; _temp433.level=( void*)(( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp434=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp434[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp435; _temp435.tag= Cyc_CfFlowInfo_MustPointTo;
_temp435.f1=({ struct Cyc_CfFlowInfo_Place* _temp436=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp436->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp437=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp437[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp438; _temp438.tag= Cyc_CfFlowInfo_MallocPt;
_temp438.f1= _temp419; _temp438;}); _temp437;})); _temp436->fields= 0; _temp436;});
_temp435;}); _temp434;})); _temp433;}); _LL408: { struct Cyc_CfFlowInfo_InitState
_temp439= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp423); struct Cyc_CfFlowInfo_InitState
_temp440= _temp439; void* _temp446; struct Cyc_CfFlowInfo_Place* _temp448;
struct Cyc_CfFlowInfo_Place _temp450; struct Cyc_List_List* _temp451; void*
_temp453; _LL442: _LL447: _temp446=( void*) _temp440.level; if(( unsigned int)
_temp446 >  3u?*(( int*) _temp446) ==  Cyc_CfFlowInfo_MustPointTo: 0){ _LL449:
_temp448=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp446)->f1; _temp450=*
_temp448; _LL454: _temp453=( void*) _temp450.root; goto _LL452; _LL452: _temp451=
_temp450.fields; goto _LL443;} else{ goto _LL444;} _LL444: goto _LL445; _LL443:
_temp451=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp451,({ struct Cyc_List_List* _temp455=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp455->hd=( void*) _temp421;
_temp455->tl= 0; _temp455;})); return({ struct Cyc_CfFlowInfo_InitState _temp456;
_temp456.esc= Cyc_CfFlowInfo_Unesc; _temp456.level=( void*)(( void*)({ struct
Cyc_CfFlowInfo_MustPointTo_struct* _temp457=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp457[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp458; _temp458.tag= Cyc_CfFlowInfo_MustPointTo;
_temp458.f1=({ struct Cyc_CfFlowInfo_Place* _temp459=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp459->root=( void*)
_temp453; _temp459->fields= _temp451; _temp459;}); _temp458;}); _temp457;}));
_temp456;}); _LL445: return _temp439; _LL441:;} _LL410: { void* _temp460= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp425); void* _temp461= _temp460; struct Cyc_CfFlowInfo_InitState
_temp467; _LL463: if(*(( int*) _temp461) ==  Cyc_CfFlowInfo_LeafPI){ _LL468:
_temp467=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp461)->f1; goto _LL464;}
else{ goto _LL465;} _LL465: if(*(( int*) _temp461) ==  Cyc_CfFlowInfo_DictPI){
goto _LL466;} else{ goto _LL462;} _LL464: return _temp467; _LL466:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL462:;} _LL412: goto _LL414; _LL414: goto
_LL416; _LL416:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL402:;} void*
Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void*
_temp469= ao; struct Cyc_Absyn_Vardecl* _temp485; struct Cyc_Absyn_Exp* _temp487;
struct Cyc_Dict_Dict* _temp489; struct _tagged_arr* _temp491; void* _temp493;
void* _temp495; void* _temp497; _LL471: if( _temp469 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL472;} else{ goto _LL473;} _LL473: if(( unsigned int) _temp469 >  1u?*((
int*) _temp469) ==  Cyc_CfAbsexp_LocalOp: 0){ _LL486: _temp485=(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp469)->f1; goto _LL474;} else{ goto _LL475;} _LL475: if(( unsigned int)
_temp469 >  1u?*(( int*) _temp469) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL488:
_temp487=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp469)->f1; goto _LL476;}
else{ goto _LL477;} _LL477: if(( unsigned int) _temp469 >  1u?*(( int*) _temp469)
==  Cyc_CfAbsexp_DictOp: 0){ _LL490: _temp489=(( struct Cyc_CfAbsexp_DictOp_struct*)
_temp469)->f1; goto _LL478;} else{ goto _LL479;} _LL479: if(( unsigned int)
_temp469 >  1u?*(( int*) _temp469) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL494:
_temp493=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp469)->f1; goto
_LL492; _LL492: _temp491=(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp469)->f2;
goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int) _temp469 >  1u?*((
int*) _temp469) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL496: _temp495=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp469)->f1; goto _LL482;} else{ goto _LL483;}
_LL483: if(( unsigned int) _temp469 >  1u?*(( int*) _temp469) ==  Cyc_CfAbsexp_AddressOp:
0){ _LL498: _temp497=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp469)->f1;
goto _LL484;} else{ goto _LL470;} _LL472: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_AllIL); _LL474: return Cyc_Dict_lookup( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp499=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp499[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp500; _temp500.tag= Cyc_CfFlowInfo_VarRoot;
_temp500.f1= _temp485; _temp500;}); _temp499;})); _LL476: return Cyc_Dict_lookup(
pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp501=( struct
Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp501[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp502; _temp502.tag=
Cyc_CfFlowInfo_MallocPt; _temp502.f1= _temp487; _temp502;}); _temp501;}));
_LL478: return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct* _temp503=( struct
Cyc_CfFlowInfo_DictPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct));
_temp503[ 0]=({ struct Cyc_CfFlowInfo_DictPI_struct _temp504; _temp504.tag= Cyc_CfFlowInfo_DictPI;
_temp504.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_Dict_Dict*, void*),
struct Cyc_Dict_Dict* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfAbsexp_eval_absop_r,
pinfo_dict, _temp489); _temp504;}); _temp503;}); _LL480: { void* _temp505= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp493); void* _temp506= _temp505; struct Cyc_Dict_Dict* _temp512;
_LL508: if(*(( int*) _temp506) ==  Cyc_CfFlowInfo_LeafPI){ goto _LL509;} else{
goto _LL510;} _LL510: if(*(( int*) _temp506) ==  Cyc_CfFlowInfo_DictPI){ _LL513:
_temp512=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp506)->f1; goto _LL511;}
else{ goto _LL507;} _LL509: return _temp505; _LL511: return(( void*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp512, _temp491);
_LL507:;} _LL482: { void* _temp514= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp495); void* _temp515= _temp514; struct Cyc_CfFlowInfo_InitState _temp525;
void* _temp527; struct Cyc_CfFlowInfo_InitState _temp529; void* _temp531; struct
Cyc_CfFlowInfo_Place* _temp533; struct Cyc_CfFlowInfo_InitState _temp535; _LL517:
if(*(( int*) _temp515) ==  Cyc_CfFlowInfo_LeafPI){ _LL526: _temp525=(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp515)->f1; _LL528: _temp527=( void*) _temp525.level;
if( _temp527 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL518;} else{ goto _LL519;}}
else{ goto _LL519;} _LL519: if(*(( int*) _temp515) ==  Cyc_CfFlowInfo_LeafPI){
_LL530: _temp529=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp515)->f1; _LL532:
_temp531=( void*) _temp529.level; if(( unsigned int) _temp531 >  3u?*(( int*)
_temp531) ==  Cyc_CfFlowInfo_MustPointTo: 0){ _LL534: _temp533=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp531)->f1; goto _LL520;} else{ goto _LL521;}} else{ goto _LL521;} _LL521:
if(*(( int*) _temp515) ==  Cyc_CfFlowInfo_LeafPI){ _LL536: _temp535=(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp515)->f1; goto _LL522;} else{ goto _LL523;}
_LL523: goto _LL524; _LL518: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_NoneIL); _LL520: return Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp533); _LL522: return _temp514; _LL524:( int) _throw(( void*)
Cyc_CfAbsexp_BadAbsexp); _LL516:;} _LL484: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp537=( struct Cyc_CfFlowInfo_LeafPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp537[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp538; _temp538.tag= Cyc_CfFlowInfo_LeafPI;
_temp538.f1= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp497); _temp538;});
_temp537;}); _LL470:;} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo); struct Cyc_Dict_Dict* Cyc_CfAbsexp_assign_escape_f(
struct Cyc_List_List** escaping_states, struct _tagged_arr* key, void* b1, void*
b2, struct Cyc_Dict_Dict* accum){ return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( accum, key, Cyc_CfAbsexp_assign_escape(
escaping_states, b1, b2));} void* Cyc_CfAbsexp_all_init_dict( void* old){ void*
_temp539= old; struct Cyc_Dict_Dict* _temp545; _LL541: if(*(( int*) _temp539) == 
Cyc_CfFlowInfo_LeafPI){ goto _LL542;} else{ goto _LL543;} _LL543: if(*(( int*)
_temp539) ==  Cyc_CfFlowInfo_DictPI){ _LL546: _temp545=(( struct Cyc_CfFlowInfo_DictPI_struct*)
_temp539)->f1; goto _LL544;} else{ goto _LL540;} _LL542: return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL544: return( void*)({
struct Cyc_CfFlowInfo_DictPI_struct* _temp547=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp547[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp548; _temp548.tag= Cyc_CfFlowInfo_DictPI;
_temp548.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*), struct Cyc_Dict_Dict*
d)) Cyc_Dict_map)( Cyc_CfAbsexp_all_init_dict, _temp545); _temp548;}); _temp547;});
_LL540:;} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp550=({
struct _tuple3 _temp549; _temp549.f1= old_pinfo; _temp549.f2= new_pinfo;
_temp549;}); void* _temp562; struct Cyc_CfFlowInfo_InitState _temp564; void*
_temp566; void* _temp568; struct Cyc_CfFlowInfo_InitState _temp570; enum  Cyc_CfFlowInfo_Escaped
_temp572; void* _temp574; void* _temp576; struct Cyc_CfFlowInfo_InitState
_temp578; enum  Cyc_CfFlowInfo_Escaped _temp580; void* _temp582; struct Cyc_Dict_Dict*
_temp584; void* _temp586; struct Cyc_Dict_Dict* _temp588; void* _temp590; struct
Cyc_CfFlowInfo_InitState _temp592; void* _temp594; void* _temp596; struct Cyc_Dict_Dict*
_temp598; _LL552: _LL569: _temp568= _temp550.f1; if(*(( int*) _temp568) ==  Cyc_CfFlowInfo_LeafPI){
_LL571: _temp570=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp568)->f1; _LL573:
_temp572= _temp570.esc; if( _temp572 ==  Cyc_CfFlowInfo_Unesc){ goto _LL563;}
else{ goto _LL554;}} else{ goto _LL554;} _LL563: _temp562= _temp550.f2; if(*((
int*) _temp562) ==  Cyc_CfFlowInfo_LeafPI){ _LL565: _temp564=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp562)->f1; _LL567: _temp566=( void*) _temp564.level; goto _LL553;} else{
goto _LL554;} _LL554: _LL577: _temp576= _temp550.f1; if(*(( int*) _temp576) == 
Cyc_CfFlowInfo_LeafPI){ _LL579: _temp578=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp576)->f1; _LL581: _temp580= _temp578.esc; if( _temp580 ==  Cyc_CfFlowInfo_Esc){
goto _LL575;} else{ goto _LL556;}} else{ goto _LL556;} _LL575: _temp574=
_temp550.f2; if(*(( int*) _temp574) ==  Cyc_CfFlowInfo_LeafPI){ goto _LL555;}
else{ goto _LL556;} _LL556: _LL587: _temp586= _temp550.f1; if(*(( int*) _temp586)
==  Cyc_CfFlowInfo_DictPI){ _LL589: _temp588=(( struct Cyc_CfFlowInfo_DictPI_struct*)
_temp586)->f1; goto _LL583;} else{ goto _LL558;} _LL583: _temp582= _temp550.f2;
if(*(( int*) _temp582) ==  Cyc_CfFlowInfo_DictPI){ _LL585: _temp584=(( struct
Cyc_CfFlowInfo_DictPI_struct*) _temp582)->f1; goto _LL557;} else{ goto _LL558;}
_LL558: _LL597: _temp596= _temp550.f1; if(*(( int*) _temp596) ==  Cyc_CfFlowInfo_DictPI){
_LL599: _temp598=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp596)->f1; goto
_LL591;} else{ goto _LL560;} _LL591: _temp590= _temp550.f2; if(*(( int*)
_temp590) ==  Cyc_CfFlowInfo_LeafPI){ _LL593: _temp592=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp590)->f1; _LL595: _temp594=( void*) _temp592.level; if( _temp594 == ( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL559;} else{ goto _LL560;}} else{ goto _LL560;}
_LL560: goto _LL561; _LL553: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc,
_temp566); _LL555:* escaping_states=({ struct Cyc_List_List* _temp600=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp600->hd=( void*)
new_pinfo; _temp600->tl=* escaping_states; _temp600;}); return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL557: return( void*)({
struct Cyc_CfFlowInfo_DictPI_struct* _temp601=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp601[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp602; _temp602.tag= Cyc_CfFlowInfo_DictPI;
_temp602.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
struct _tagged_arr*, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List**
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict*
accum)) Cyc_Dict_fold2_c)( Cyc_CfAbsexp_assign_escape_f, escaping_states,
_temp588, _temp584,(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp)); _temp602;});
_temp601;}); _LL559: return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct*
_temp603=( struct Cyc_CfFlowInfo_DictPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct));
_temp603[ 0]=({ struct Cyc_CfFlowInfo_DictPI_struct _temp604; _temp604.tag= Cyc_CfFlowInfo_DictPI;
_temp604.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*), struct Cyc_Dict_Dict*
d)) Cyc_Dict_map)( Cyc_CfAbsexp_all_init_dict, _temp598); _temp604;}); _temp603;});
_LL561:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp605=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp605[ 0]=({ struct Cyc_Core_Impossible_struct _temp606; _temp606.tag= Cyc_Core_Impossible;
_temp606.f1= _tag_arr("bad pinfos in assign_escape", sizeof( unsigned char), 28u);
_temp606;}); _temp605;})); _LL551:;} void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, void* ae, void* in_flow){ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp607= in_flow; struct Cyc_Dict_Dict* _temp613; _LL609:
if( _temp607 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL610;} else{ goto
_LL611;} _LL611: if(( unsigned int) _temp607 >  1u?*(( int*) _temp607) ==  Cyc_CfFlowInfo_InitsFL:
0){ _LL614: _temp613=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp607)->f1;
goto _LL612;} else{ goto _LL608;} _LL610: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL612: pinfo_dict= _temp613; goto _LL608; _LL608:;}{ void* _temp615= ae; void*
_temp637; void* _temp639; void* _temp641; void* _temp643; struct Cyc_Absyn_Stmt*
_temp645; void* _temp647; void* _temp649; void* _temp651; void* _temp653; void*
_temp655; struct Cyc_Absyn_Vardecl* _temp657; struct Cyc_List_List* _temp659;
void* _temp661; struct Cyc_List_List* _temp663; void* _temp665; struct Cyc_List_List*
_temp667; void* _temp669; _LL617: if(( unsigned int) _temp615 >  2u?*(( int*)
_temp615) ==  Cyc_CfAbsexp_AssignAE: 0){ _LL640: _temp639=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp615)->f1; goto _LL638; _LL638: _temp637=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp615)->f2; goto _LL618;} else{ goto _LL619;} _LL619: if(( unsigned int)
_temp615 >  2u?*(( int*) _temp615) ==  Cyc_CfAbsexp_UseAE: 0){ _LL642: _temp641=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp615)->f1; goto _LL620;} else{
goto _LL621;} _LL621: if(( unsigned int) _temp615 >  2u?*(( int*) _temp615) == 
Cyc_CfAbsexp_MallocAE: 0){ _LL644: _temp643=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp615)->f1; goto _LL622;} else{ goto _LL623;} _LL623: if( _temp615 == ( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL624;} else{ goto _LL625;} _LL625: if( _temp615
== ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL626;} else{ goto _LL627;} _LL627: if((
unsigned int) _temp615 >  2u?*(( int*) _temp615) ==  Cyc_CfAbsexp_StmtAE: 0){
_LL646: _temp645=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp615)->f1; goto
_LL628;} else{ goto _LL629;} _LL629: if(( unsigned int) _temp615 >  2u?*(( int*)
_temp615) ==  Cyc_CfAbsexp_ComprehensionAE: 0){ _LL658: _temp657=(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp615)->f1; goto _LL656; _LL656: _temp655=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp615)->f2; goto _LL654; _LL654: _temp653=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp615)->f3; goto _LL652; _LL652: _temp651=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp615)->f4; goto _LL650; _LL650: _temp649=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp615)->f5; goto _LL648; _LL648: _temp647=( void*)(( struct Cyc_CfAbsexp_ComprehensionAE_struct*)
_temp615)->f6; goto _LL630;} else{ goto _LL631;} _LL631: if(( unsigned int)
_temp615 >  2u?*(( int*) _temp615) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL662:
_temp661=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp615)->f1; if(
_temp661 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL660;} else{ goto _LL633;}
_LL660: _temp659=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp615)->f2; goto
_LL632;} else{ goto _LL633;} _LL633: if(( unsigned int) _temp615 >  2u?*(( int*)
_temp615) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL666: _temp665=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp615)->f1; if( _temp665 == ( void*) Cyc_CfAbsexp_OneofG){ goto _LL664;}
else{ goto _LL635;} _LL664: _temp663=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp615)->f2; goto _LL634;} else{ goto _LL635;} _LL635: if(( unsigned int)
_temp615 >  2u?*(( int*) _temp615) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL670:
_temp669=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp615)->f1; if(
_temp669 == ( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL668;} else{ goto _LL616;}
_LL668: _temp667=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp615)->f2; goto
_LL636;} else{ goto _LL616;} _LL618:{ struct Cyc_CfFlowInfo_InitState _temp671=
Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp639); void* _temp677; struct Cyc_CfFlowInfo_Place*
_temp679; _LL673: _LL678: _temp677=( void*) _temp671.level; if(( unsigned int)
_temp677 >  3u?*(( int*) _temp677) ==  Cyc_CfFlowInfo_MustPointTo: 0){ _LL680:
_temp679=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp677)->f1; goto
_LL674;} else{ goto _LL675;} _LL675: goto _LL676; _LL674: { void* _temp681= Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp679); void* _temp682= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp637);{ struct _tuple3 _temp684=({ struct _tuple3 _temp683; _temp683.f1=
_temp681; _temp683.f2= _temp682; _temp683;}); void* _temp690; struct Cyc_CfFlowInfo_InitState
_temp692; void* _temp694; void* _temp696; _LL686: _LL697: _temp696= _temp684.f1;
if(*(( int*) _temp696) ==  Cyc_CfFlowInfo_DictPI){ goto _LL691;} else{ goto
_LL688;} _LL691: _temp690= _temp684.f2; if(*(( int*) _temp690) ==  Cyc_CfFlowInfo_LeafPI){
_LL693: _temp692=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp690)->f1; _LL695:
_temp694=( void*) _temp692.level; goto _LL687;} else{ goto _LL688;} _LL688: goto
_LL689; _LL687:{ void* _temp698= _temp694; _LL700: if(( unsigned int) _temp698 > 
3u?*(( int*) _temp698) ==  Cyc_CfFlowInfo_MustPointTo: 0){ goto _LL701;} else{
goto _LL702;} _LL702: goto _LL703; _LL701: goto _LL699; _LL703: _temp682= Cyc_CfFlowInfo_assign_unknown_dict(
_temp694, _temp681); goto _LL699; _LL699:;} goto _LL685; _LL689: goto _LL685;
_LL685:;} if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)),
_temp679);}{ struct Cyc_List_List* _temp704= 0; void* _temp705= Cyc_CfAbsexp_assign_escape(&
_temp704, _temp681, _temp682); for( 0; _temp704 !=  0; _temp704=(( struct Cyc_List_List*)
_check_null( _temp704))->tl){ void* _temp706=( void*)(( struct Cyc_List_List*)
_check_null( _temp704))->hd; if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict,
_temp706)){({ void* _temp707[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("uninitialized value assigned to escaped place",
sizeof( unsigned char), 46u), _tag_arr( _temp707, sizeof( void*), 0u));});
return( void*) Cyc_CfFlowInfo_BottomFL;} pinfo_dict= Cyc_CfFlowInfo_escape_pointsto(
_temp706, pinfo_dict, env->all_changed);} pinfo_dict= Cyc_CfFlowInfo_insert_place(
pinfo_dict, _temp679, _temp705);{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp708=({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp709=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp709[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp710; _temp710.tag= Cyc_CfFlowInfo_InitsFL;
_temp710.f1= pinfo_dict; _temp710;}); _temp709;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp708); return( void*) _temp708;}}} _LL676: goto _LL672; _LL672:;}
_temp641= _temp637; goto _LL620; _LL620: { void* _temp711= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp641); if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp711)){({
struct Cyc_Std_String_pa_struct _temp713; _temp713.tag= Cyc_Std_String_pa;
_temp713.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp641);{ void*
_temp712[ 1u]={& _temp713}; Cyc_Tcutil_terr( loc, _tag_arr("%s must be fully initialized to be used here",
sizeof( unsigned char), 45u), _tag_arr( _temp712, sizeof( void*), 1u));}});
return( void*) Cyc_CfFlowInfo_BottomFL;}{ struct Cyc_Dict_Dict* _temp714= Cyc_CfFlowInfo_escape_pointsto(
_temp711, pinfo_dict, env->all_changed); if( _temp714 ==  pinfo_dict){ return
in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp715=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp716=( struct Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp716[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp717; _temp717.tag= Cyc_CfFlowInfo_InitsFL;
_temp717.f1= pinfo_dict; _temp717;}); _temp716;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp715); return( void*) _temp715;}}} _LL622: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp718=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp718[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp719; _temp719.tag= Cyc_CfFlowInfo_InitsFL;
_temp719.f1= Cyc_Dict_insert( pinfo_dict, _temp643, Cyc_Dict_lookup( env->roots,
_temp643)); _temp719;}); _temp718;}); _LL624: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL626: return in_flow; _LL628: return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp645, in_flow); _LL630: { void* _temp720= Cyc_CfAbsexp_eval_absexp( env, loc,
_temp655, in_flow); void* _temp721= Cyc_CfAbsexp_eval_absexp( env, loc, _temp653,
in_flow); void* _temp722= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp651); if(
! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp722)){({ struct Cyc_Std_String_pa_struct
_temp724; _temp724.tag= Cyc_Std_String_pa; _temp724.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp651);{ void* _temp723[ 1u]={& _temp724}; Cyc_Tcutil_terr( loc, _tag_arr("%s must be fully initialized to be used here",
sizeof( unsigned char), 45u), _tag_arr( _temp723, sizeof( void*), 1u));}});
return( void*) Cyc_CfFlowInfo_BottomFL;}{ void* body_in_flow;{ void* _temp725=
_temp720; struct Cyc_Dict_Dict* _temp731; _LL727: if( _temp725 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL728;} else{ goto _LL729;} _LL729: if(( unsigned int) _temp725 >  1u?*((
int*) _temp725) ==  Cyc_CfFlowInfo_InitsFL: 0){ _LL732: _temp731=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp725)->f1; goto _LL730;} else{ goto _LL726;} _LL728: body_in_flow=( void*)
Cyc_CfFlowInfo_BottomFL; goto _LL726; _LL730: { struct Cyc_CfFlowInfo_VarRoot_struct*
_temp733=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp737=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp737[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp738; _temp738.tag= Cyc_CfFlowInfo_VarRoot;
_temp738.f1= _temp657; _temp738;}); _temp737;}); void* _temp734= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL, Cyc_Dict_lookup( env->roots,( void*) _temp733));
body_in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp735=( struct
Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp735[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp736; _temp736.tag= Cyc_CfFlowInfo_InitsFL;
_temp736.f1= Cyc_Dict_insert( _temp731,( void*) _temp733, _temp734); _temp736;});
_temp735;});} _LL726:;}{ void* old_body_in_flow; void* body_out_flow; do {
body_out_flow= Cyc_CfAbsexp_eval_absexp( env, loc, _temp649, body_in_flow);{
void* _temp739= body_out_flow; struct Cyc_Dict_Dict* _temp745; _LL741: if(
_temp739 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL742;} else{ goto _LL743;}
_LL743: if(( unsigned int) _temp739 >  1u?*(( int*) _temp739) ==  Cyc_CfFlowInfo_InitsFL:
0){ _LL746: _temp745=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp739)->f1;
goto _LL744;} else{ goto _LL740;} _LL742: goto _LL740; _LL744: { void* _temp747=
Cyc_CfAbsexp_eval_absop_r( _temp745, _temp647); if( ! Cyc_CfFlowInfo_isAllInit(
_temp745, _temp747)){({ struct Cyc_Std_String_pa_struct _temp749; _temp749.tag=
Cyc_Std_String_pa; _temp749.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp647);{ void* _temp748[ 1u]={& _temp749}; Cyc_Tcutil_terr( loc, _tag_arr("%s must be fully initialized to be used here",
sizeof( unsigned char), 45u), _tag_arr( _temp748, sizeof( void*), 1u));}});
return( void*) Cyc_CfFlowInfo_BottomFL;} body_out_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp750=( struct Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp750[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp751; _temp751.tag= Cyc_CfFlowInfo_InitsFL;
_temp751.f1= Cyc_CfFlowInfo_escape_pointsto( _temp747, _temp745, env->all_changed);
_temp751;}); _temp750;});} _LL740:;} old_body_in_flow= body_in_flow;
body_in_flow= Cyc_CfFlowInfo_join_flow( env->all_changed, body_in_flow,
body_out_flow);} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( body_in_flow,
old_body_in_flow)); return Cyc_CfFlowInfo_join_flow( env->all_changed, _temp721,
body_out_flow);}}} _LL632: for( 0; _temp659 !=  0; _temp659=(( struct Cyc_List_List*)
_check_null( _temp659))->tl){ in_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)((
struct Cyc_List_List*) _check_null( _temp659))->hd, in_flow);} return in_flow;
_LL634: { void* out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp663 != 
0; _temp663=(( struct Cyc_List_List*) _check_null( _temp663))->tl){ out_flow=
Cyc_CfFlowInfo_join_flow( env->all_changed, out_flow, Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp663))->hd, in_flow));}
return out_flow;} _LL636: if( _temp667 ==  0){ return in_flow;}{ struct Cyc_Set_Set**
_temp752= env->all_changed; struct Cyc_Set_Set* this_all_changed; void*
old_in_flow; void* out_flow; do { this_all_changed= Cyc_CfFlowInfo_mt_place_set();
env->all_changed=({ struct Cyc_Set_Set** _temp753=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp753[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp753;}); out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp667))->hd, in_flow); this_all_changed=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));{
struct Cyc_List_List* ael2=(( struct Cyc_List_List*) _check_null( _temp667))->tl;
for( 0; ael2 !=  0; ael2=(( struct Cyc_List_List*) _check_null( ael2))->tl){ env->all_changed=({
struct Cyc_Set_Set** _temp754=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp754[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp754;});{ void*
_temp755= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( ael2))->hd, in_flow); out_flow= Cyc_CfFlowInfo_after_flow(( struct
Cyc_Set_Set**)& this_all_changed, out_flow, _temp755, this_all_changed,*((
struct Cyc_Set_Set**) _check_null( env->all_changed))); this_all_changed=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));}}}
old_in_flow= in_flow; in_flow= Cyc_CfFlowInfo_join_flow( _temp752, in_flow,
out_flow);} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( in_flow, old_in_flow));
if( _temp752 ==  0){ env->all_changed= 0;} else{ env->all_changed=({ struct Cyc_Set_Set**
_temp756=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp756[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)(*(( struct Cyc_Set_Set**) _check_null( _temp752)),
this_all_changed); _temp756;});} return out_flow;} _LL616:;}}
