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
struct Cyc_Dict_Dict* d, void* k, void** ans); extern void* Cyc_Dict_fold2_c(
void*(* f)( void*, void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2, void* accum); struct Cyc_Set_Set; extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2);
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{ struct
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
15u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern int Cyc_Tcutil_bits_only( void* t); struct
Cyc_CfFlowInfo_Place; enum  Cyc_CfFlowInfo_Escaped; struct Cyc_CfFlowInfo_InitState;
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
void* ae, int depth); extern void* Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkAddressOp(
void* ao); extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl* vd);
extern void* Cyc_CfAbsexp_mkMemberOp( void* ao, struct _tagged_arr* f); extern
void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkDerefOp(
void* ao); extern void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE();
extern void* Cyc_CfAbsexp_mkUseAE( void* ao); extern void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e);
extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s); extern void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2); extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g,
struct Cyc_List_List* ael); extern int Cyc_CfAbsexp_isUnknownOp( void*); extern
void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, void* ae, void* in_flow); unsigned char Cyc_CfAbsexp_BadAbsexp[
14u]="\000\000\000\000BadAbsexp"; static const int Cyc_CfAbsexp_BottomAE= 0;
static const int Cyc_CfAbsexp_SkipAE= 1; static const int Cyc_CfAbsexp_UseAE= 0;
struct Cyc_CfAbsexp_UseAE_struct{ int tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE=
1; struct Cyc_CfAbsexp_AssignAE_struct{ int tag; void* f1; void* f2; } ; static
const int Cyc_CfAbsexp_MallocAE= 2; struct Cyc_CfAbsexp_MallocAE_struct{ int tag;
void* f1; int f2; } ; static const int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE= 4;
struct Cyc_CfAbsexp_GroupAE_struct{ int tag; void* f1; struct Cyc_List_List* f2;
} ; static const int Cyc_CfAbsexp_UnknownOp= 0; static const int Cyc_CfAbsexp_AddressOp=
0; struct Cyc_CfAbsexp_AddressOp_struct{ int tag; void* f1; } ; static const int
Cyc_CfAbsexp_DerefOp= 1; struct Cyc_CfAbsexp_DerefOp_struct{ int tag; void* f1;
} ; static const int Cyc_CfAbsexp_MemberOp= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_CfAbsexp_LocalOp=
3; struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; }
; static const int Cyc_CfAbsexp_MallocOp= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; _LL2: if(( unsigned int) _temp0 >  1u?*(( int*)
_temp0) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL3;} else{ goto _LL4;} _LL4: if((
unsigned int) _temp0 >  1u?*(( int*) _temp0) ==  Cyc_CfAbsexp_MemberOp: 0){ goto
_LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 >  1u?*(( int*) _temp0)
==  Cyc_CfAbsexp_MallocOp: 0){ goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9;
_LL3: goto _LL5; _LL5: goto _LL7; _LL7: return; _LL9:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL1:;} static void Cyc_CfAbsexp_ok_member_arg( void* ao){ void* _temp10= ao;
_LL12: if(( unsigned int) _temp10 >  1u?*(( int*) _temp10) ==  Cyc_CfAbsexp_LocalOp:
0){ goto _LL13;} else{ goto _LL14;} _LL14: if(( unsigned int) _temp10 >  1u?*((
int*) _temp10) ==  Cyc_CfAbsexp_MemberOp: 0){ goto _LL15;} else{ goto _LL16;}
_LL16: if(( unsigned int) _temp10 >  1u?*(( int*) _temp10) ==  Cyc_CfAbsexp_MallocOp:
0){ goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp10 >  1u?*((
int*) _temp10) ==  Cyc_CfAbsexp_DerefOp: 0){ goto _LL19;} else{ goto _LL20;}
_LL20: goto _LL21; _LL13: goto _LL15; _LL15: goto _LL17; _LL17: goto _LL19;
_LL19: return; _LL21:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL11:;}
static void Cyc_CfAbsexp_ok_deref_arg( void* ao){ void* _temp22= ao; _LL24: if((
unsigned int) _temp22 >  1u?*(( int*) _temp22) ==  Cyc_CfAbsexp_LocalOp: 0){
goto _LL25;} else{ goto _LL26;} _LL26: if(( unsigned int) _temp22 >  1u?*(( int*)
_temp22) ==  Cyc_CfAbsexp_MemberOp: 0){ goto _LL27;} else{ goto _LL28;} _LL28:
if(( unsigned int) _temp22 >  1u?*(( int*) _temp22) ==  Cyc_CfAbsexp_DerefOp: 0){
goto _LL29;} else{ goto _LL30;} _LL30: goto _LL31; _LL25: goto _LL27; _LL27:
goto _LL29; _LL29: return; _LL31:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL23:;} static void Cyc_CfAbsexp_ok_lvalue( void* ao){ void* _temp32= ao; _LL34:
if( _temp32 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL35;} else{ goto _LL36;}
_LL36: if(( unsigned int) _temp32 >  1u?*(( int*) _temp32) ==  Cyc_CfAbsexp_AddressOp:
0){ goto _LL37;} else{ goto _LL38;} _LL38: goto _LL39; _LL35: goto _LL37; _LL37:(
int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL39: return; _LL33:;} static int
Cyc_CfAbsexp_num_mallocpts= 0; static struct Cyc_Dict_Dict** Cyc_CfAbsexp_mallocpt_dict=
0; int Cyc_CfAbsexp_mallocpt_int( struct Cyc_Absyn_Exp* e){ if( Cyc_CfAbsexp_mallocpt_dict
==  0){ struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*))) Cyc_Dict_empty)(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp); Cyc_CfAbsexp_mallocpt_dict=({ struct
Cyc_Dict_Dict** _temp40=( struct Cyc_Dict_Dict**) _cycalloc( sizeof( struct Cyc_Dict_Dict*)
*  1); _temp40[ 0]= d; _temp40;});}{ int i= 0; if( !(( int(*)( struct Cyc_Dict_Dict*
d, struct Cyc_Absyn_Exp* k, int* ans)) Cyc_Dict_lookup_bool)(*(( struct Cyc_Dict_Dict**)
_check_null( Cyc_CfAbsexp_mallocpt_dict)), e,& i)){*(( struct Cyc_Dict_Dict**)
_check_null( Cyc_CfAbsexp_mallocpt_dict))=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* k, int v)) Cyc_Dict_insert)(*(( struct
Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict)), e,( i= ++ Cyc_CfAbsexp_num_mallocpts));}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp41=( struct Cyc_CfAbsexp_AddressOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp41[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp42; _temp42.tag= Cyc_CfAbsexp_AddressOp;
_temp42.f1=( void*) ao; _temp42;}); _temp41;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp43=( struct Cyc_CfAbsexp_LocalOp_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp43[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp44; _temp44.tag= Cyc_CfAbsexp_LocalOp;
_temp44.f1= vd; _temp44;}); _temp43;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
struct _tagged_arr* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct
Cyc_CfAbsexp_MemberOp_struct* _temp45=( struct Cyc_CfAbsexp_MemberOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct)); _temp45[ 0]=({ struct
Cyc_CfAbsexp_MemberOp_struct _temp46; _temp46.tag= Cyc_CfAbsexp_MemberOp;
_temp46.f1=( void*) ao; _temp46.f2= f; _temp46;}); _temp45;});} void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp47=( struct Cyc_CfAbsexp_MallocOp_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp47[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp48; _temp48.tag= Cyc_CfAbsexp_MallocOp;
_temp48.f1= e; _temp48.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp48;}); _temp47;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp49=( struct Cyc_CfAbsexp_DerefOp_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp49[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp50; _temp50.tag= Cyc_CfAbsexp_DerefOp; _temp50.f1=(
void*) ao; _temp50;}); _temp49;});} void* Cyc_CfAbsexp_mkBottomAE(){ return(
void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return( void*) Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return( void*)({ struct
Cyc_CfAbsexp_MallocAE_struct* _temp51=( struct Cyc_CfAbsexp_MallocAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp51[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp52; _temp52.tag= Cyc_CfAbsexp_MallocAE;
_temp52.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp53=(
struct Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp53[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp54; _temp54.tag= Cyc_CfFlowInfo_MallocPt;
_temp54.f1= e; _temp54;}); _temp53;})); _temp52.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp52;}); _temp51;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp55=( struct Cyc_CfAbsexp_StmtAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp55[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp56; _temp56.tag= Cyc_CfAbsexp_StmtAE; _temp56.f1=
s; _temp56;}); _temp55;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp57=
ao; _LL59: if( _temp57 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL60;} else{
goto _LL61;} _LL61: goto _LL62; _LL60: return( void*) Cyc_CfAbsexp_SkipAE; _LL62:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp63=( struct Cyc_CfAbsexp_UseAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp63[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp64; _temp64.tag= Cyc_CfAbsexp_UseAE; _temp64.f1=( void*) ao; _temp64;});
_temp63;}); _LL58:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp66=({ struct _tuple3 _temp65; _temp65.f1=
l; _temp65.f2= r; _temp65;}); void* _temp74; void* _temp76; void* _temp78; _LL68:
_LL77: _temp76= _temp66.f1; if( _temp76 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL75;} else{ goto _LL70;} _LL75: _temp74= _temp66.f2; if( _temp74 == (
void*) Cyc_CfAbsexp_UnknownOp){ goto _LL69;} else{ goto _LL70;} _LL70: _LL79:
_temp78= _temp66.f1; if( _temp78 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL71;}
else{ goto _LL72;} _LL72: goto _LL73; _LL69: return( void*) Cyc_CfAbsexp_SkipAE;
_LL71: return Cyc_CfAbsexp_mkUseAE( r); _LL73: Cyc_CfAbsexp_ok_lvalue( l);
return( void*)({ struct Cyc_CfAbsexp_AssignAE_struct* _temp80=( struct Cyc_CfAbsexp_AssignAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp80[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp81; _temp81.tag= Cyc_CfAbsexp_AssignAE;
_temp81.f1=( void*) l; _temp81.f2=( void*) r; _temp81;}); _temp80;}); _LL67:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp83=({ struct _tuple4 _temp82;
_temp82.f1= g; _temp82.f2= ae1; _temp82.f3= ae2; _temp82;}); void* _temp106;
void* _temp109; void* _temp111; void* _temp113; void* _temp115; void* _temp117;
void* _temp119; void* _temp121; void* _temp124; struct Cyc_List_List* _temp126;
void* _temp128; void* _temp130; struct Cyc_List_List* _temp132; void* _temp134;
void* _temp136; void* _temp139; struct Cyc_List_List* _temp141; void* _temp143;
void* _temp145; void* _temp147; struct Cyc_List_List* _temp149; void* _temp151;
void* _temp153; void* _temp156; struct Cyc_List_List* _temp158; void* _temp160;
void* _temp162; _LL85: _LL107: _temp106= _temp83.f3; if( _temp106 == ( void*)
Cyc_CfAbsexp_SkipAE){ goto _LL105;} else{ goto _LL87;} _LL105: if( g != ( void*)
Cyc_CfAbsexp_OneofG){ goto _LL86;} else{ goto _LL87;} _LL87: _LL110: _temp109=
_temp83.f2; if( _temp109 == ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL108;} else{
goto _LL89;} _LL108: if( g != ( void*) Cyc_CfAbsexp_OneofG){ goto _LL88;} else{
goto _LL89;} _LL89: _LL114: _temp113= _temp83.f1; if( _temp113 == ( void*) Cyc_CfAbsexp_OneofG){
goto _LL112;} else{ goto _LL91;} _LL112: _temp111= _temp83.f3; if( _temp111 == (
void*) Cyc_CfAbsexp_BottomAE){ goto _LL90;} else{ goto _LL91;} _LL91: _LL118:
_temp117= _temp83.f1; if( _temp117 == ( void*) Cyc_CfAbsexp_OneofG){ goto _LL116;}
else{ goto _LL93;} _LL116: _temp115= _temp83.f2; if( _temp115 == ( void*) Cyc_CfAbsexp_BottomAE){
goto _LL92;} else{ goto _LL93;} _LL93: _LL122: _temp121= _temp83.f1; if(
_temp121 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL120;} else{ goto _LL95;}
_LL120: _temp119= _temp83.f2; if( _temp119 == ( void*) Cyc_CfAbsexp_BottomAE){
goto _LL94;} else{ goto _LL95;} _LL95: _LL137: _temp136= _temp83.f1; goto _LL131;
_LL131: _temp130= _temp83.f2; if(( unsigned int) _temp130 >  2u?*(( int*)
_temp130) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL135: _temp134=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f1; goto _LL133; _LL133: _temp132=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f2; goto _LL125;} else{ goto _LL97;} _LL125: _temp124= _temp83.f3;
if(( unsigned int) _temp124 >  2u?*(( int*) _temp124) ==  Cyc_CfAbsexp_GroupAE:
0){ _LL129: _temp128=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f1;
goto _LL127; _LL127: _temp126=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f2;
goto _LL123;} else{ goto _LL97;} _LL123: if( _temp136 ==  _temp134? _temp136 == 
_temp128: 0){ goto _LL96;} else{ goto _LL97;} _LL97: _LL146: _temp145= _temp83.f1;
goto _LL140; _LL140: _temp139= _temp83.f2; if(( unsigned int) _temp139 >  2u?*((
int*) _temp139) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL144: _temp143=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f1; goto _LL142; _LL142: _temp141=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f2; goto _LL138;} else{ goto
_LL99;} _LL138: if( _temp145 ==  _temp143? _temp145 != ( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL98;} else{ goto _LL99;} _LL99: _LL154: _temp153= _temp83.f1; if(
_temp153 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL148;} else{ goto _LL101;}
_LL148: _temp147= _temp83.f2; if(( unsigned int) _temp147 >  2u?*(( int*)
_temp147) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL152: _temp151=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f1; if( _temp151 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL150;}
else{ goto _LL101;} _LL150: _temp149=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f2; goto _LL100;} else{ goto _LL101;} _LL101: _LL163: _temp162=
_temp83.f1; goto _LL157; _LL157: _temp156= _temp83.f3; if(( unsigned int)
_temp156 >  2u?*(( int*) _temp156) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL161:
_temp160=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f1; goto
_LL159; _LL159: _temp158=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f2;
goto _LL155;} else{ goto _LL103;} _LL155: if( _temp162 ==  _temp160){ goto
_LL102;} else{ goto _LL103;} _LL103: goto _LL104; _LL86: return ae1; _LL88:
return ae2; _LL90: return ae1; _LL92: return ae2; _LL94: return( void*) Cyc_CfAbsexp_BottomAE;
_LL96: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp164=( struct
Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp164[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp165; _temp165.tag= Cyc_CfAbsexp_GroupAE;
_temp165.f1=( void*) _temp136; _temp165.f2= Cyc_List_append( _temp132, _temp126);
_temp165;}); _temp164;}); _LL98: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp166=( struct Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp166[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp167; _temp167.tag= Cyc_CfAbsexp_GroupAE;
_temp167.f1=( void*) _temp145; _temp167.f2=({ struct Cyc_List_List* _temp168=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List) *  1); _temp168[
0]=({ struct Cyc_List_List _temp169; _temp169.hd=( void*) ae2; _temp169.tl=
_temp141; _temp169;}); _temp168;}); _temp167;}); _temp166;}); _LL100: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp170=( struct Cyc_CfAbsexp_GroupAE_struct*)
_cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp170[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp171; _temp171.tag= Cyc_CfAbsexp_GroupAE;
_temp171.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp171.f2= Cyc_List_append(
_temp149,({ struct Cyc_List_List* _temp172=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp172->hd=( void*) ae2; _temp172->tl= 0;
_temp172;})); _temp171;}); _temp170;}); _LL102: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp173=( struct Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp173[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp174; _temp174.tag= Cyc_CfAbsexp_GroupAE;
_temp174.f1=( void*) _temp162; _temp174.f2=({ struct Cyc_List_List* _temp175=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp175->hd=(
void*) ae1; _temp175->tl= _temp158; _temp175;}); _temp174;}); _temp173;});
_LL104: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp176=( struct
Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp176[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp177; _temp177.tag= Cyc_CfAbsexp_GroupAE;
_temp177.f1=( void*) g; _temp177.f2=({ struct Cyc_List_List* _temp178=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp178->hd=( void*)
ae1; _temp178->tl=({ struct Cyc_List_List* _temp179=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp179->hd=( void*) ae2; _temp179->tl=
0; _temp179;}); _temp178;}); _temp177;}); _temp176;}); _LL84:;} void* Cyc_CfAbsexp_mkGroupAE_l(
void* g, struct Cyc_List_List* ael){ if( ael ==  0){ return( void*) Cyc_CfAbsexp_SkipAE;}
if((( struct Cyc_List_List*) _check_null( ael))->tl ==  0){ return( void*)((
struct Cyc_List_List*) _check_null( ael))->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp180=( struct Cyc_CfAbsexp_GroupAE_struct*) _cycalloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp180[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp181; _temp181.tag= Cyc_CfAbsexp_GroupAE;
_temp181.f1=( void*) g; _temp181.f2= ael; _temp181;}); _temp180;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao == ( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_arr Cyc_CfAbsexp_absop2string(
void* ao){ void* _temp182= ao; void* _temp196; struct Cyc_Absyn_Vardecl*
_temp198; struct _tagged_arr* _temp200; void* _temp202; int _temp204; struct Cyc_Absyn_Exp*
_temp206; void* _temp208; _LL184: if( _temp182 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL185;} else{ goto _LL186;} _LL186: if(( unsigned int) _temp182 >  1u?*((
int*) _temp182) ==  Cyc_CfAbsexp_AddressOp: 0){ _LL197: _temp196=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp182)->f1; goto _LL187;} else{ goto
_LL188;} _LL188: if(( unsigned int) _temp182 >  1u?*(( int*) _temp182) ==  Cyc_CfAbsexp_LocalOp:
0){ _LL199: _temp198=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp182)->f1; goto
_LL189;} else{ goto _LL190;} _LL190: if(( unsigned int) _temp182 >  1u?*(( int*)
_temp182) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL203: _temp202=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp182)->f1; goto _LL201; _LL201: _temp200=(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp182)->f2; goto _LL191;} else{ goto _LL192;} _LL192: if(( unsigned int)
_temp182 >  1u?*(( int*) _temp182) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL207:
_temp206=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp182)->f1; goto _LL205;
_LL205: _temp204=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp182)->f2; goto
_LL193;} else{ goto _LL194;} _LL194: if(( unsigned int) _temp182 >  1u?*(( int*)
_temp182) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL209: _temp208=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp182)->f1; goto _LL195;} else{ goto _LL183;} _LL185: return _tag_arr("Unknown",
sizeof( unsigned char), 8u); _LL187: return( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp211; _temp211.tag= Cyc_Std_String_pa; _temp211.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp196);{ void* _temp210[ 1u]={& _temp211}; Cyc_Std_aprintf( _tag_arr("& %s",
sizeof( unsigned char), 5u), _tag_arr( _temp210, sizeof( void*), 1u));}});
_LL189: return*(* _temp198->name).f2; _LL191: return( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp214; _temp214.tag= Cyc_Std_String_pa;
_temp214.f1=( struct _tagged_arr)* _temp200;{ struct Cyc_Std_String_pa_struct
_temp213; _temp213.tag= Cyc_Std_String_pa; _temp213.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp202);{ void* _temp212[ 2u]={& _temp213,& _temp214}; Cyc_Std_aprintf(
_tag_arr("%s.%s", sizeof( unsigned char), 6u), _tag_arr( _temp212, sizeof( void*),
2u));}}}); _LL193: return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp216; _temp216.tag= Cyc_Std_Int_pa; _temp216.f1=( int)(( unsigned int)
_temp204);{ void* _temp215[ 1u]={& _temp216}; Cyc_Std_aprintf( _tag_arr("<mpt%d>",
sizeof( unsigned char), 8u), _tag_arr( _temp215, sizeof( void*), 1u));}});
_LL195: return( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp218;
_temp218.tag= Cyc_Std_String_pa; _temp218.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp208);{ void* _temp217[ 1u]={& _temp218}; Cyc_Std_aprintf( _tag_arr("(*%s)",
sizeof( unsigned char), 6u), _tag_arr( _temp217, sizeof( void*), 1u));}});
_LL183:;} struct _tagged_arr Cyc_CfAbsexp_absexp2string( void* ae, int depth){
struct _tagged_arr ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0; i < 
_get_arr_size( ans, sizeof( unsigned char)); ++ i){*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char), i))=' ';}}{ void*
_temp219= ae; void* _temp237; int _temp239; void* _temp241; struct Cyc_Absyn_Exp*
_temp243; void* _temp245; void* _temp247; struct Cyc_List_List* _temp249; void*
_temp251; struct Cyc_Absyn_Stmt* _temp253; _LL221: if( _temp219 == ( void*) Cyc_CfAbsexp_SkipAE){
goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp219 >  2u?*((
int*) _temp219) ==  Cyc_CfAbsexp_UseAE: 0){ _LL238: _temp237=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp219)->f1; goto _LL224;} else{ goto _LL225;}
_LL225: if(( unsigned int) _temp219 >  2u?*(( int*) _temp219) ==  Cyc_CfAbsexp_MallocAE:
0){ _LL242: _temp241=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp219)->f1;
if(*(( int*) _temp241) ==  Cyc_CfFlowInfo_MallocPt){ _LL244: _temp243=(( struct
Cyc_CfFlowInfo_MallocPt_struct*) _temp241)->f1; goto _LL240;} else{ goto _LL227;}
_LL240: _temp239=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp219)->f2; goto
_LL226;} else{ goto _LL227;} _LL227: if(( unsigned int) _temp219 >  2u?*(( int*)
_temp219) ==  Cyc_CfAbsexp_AssignAE: 0){ _LL248: _temp247=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp219)->f1; goto _LL246; _LL246: _temp245=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp219)->f2; goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int)
_temp219 >  2u?*(( int*) _temp219) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL252:
_temp251=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp219)->f1; goto
_LL250; _LL250: _temp249=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp219)->f2;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp219 >  2u?*((
int*) _temp219) ==  Cyc_CfAbsexp_StmtAE: 0){ _LL254: _temp253=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp219)->f1; goto _LL232;} else{ goto _LL233;} _LL233: if( _temp219 == ( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL234;} else{ goto _LL235;} _LL235: goto _LL236;
_LL222: ans=({ struct Cyc_Std_String_pa_struct _temp256; _temp256.tag= Cyc_Std_String_pa;
_temp256.f1=( struct _tagged_arr) ans;{ void* _temp255[ 1u]={& _temp256}; Cyc_Std_aprintf(
_tag_arr("%sskip", sizeof( unsigned char), 7u), _tag_arr( _temp255, sizeof( void*),
1u));}}); goto _LL220; _LL224: ans=({ struct Cyc_Std_String_pa_struct _temp259;
_temp259.tag= Cyc_Std_String_pa; _temp259.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp237);{ struct Cyc_Std_String_pa_struct _temp258; _temp258.tag= Cyc_Std_String_pa;
_temp258.f1=( struct _tagged_arr) ans;{ void* _temp257[ 2u]={& _temp258,&
_temp259}; Cyc_Std_aprintf( _tag_arr("%suse %s", sizeof( unsigned char), 9u),
_tag_arr( _temp257, sizeof( void*), 2u));}}}); goto _LL220; _LL226: ans=({
struct Cyc_Std_Int_pa_struct _temp262; _temp262.tag= Cyc_Std_Int_pa; _temp262.f1=(
int)(( unsigned int) _temp239);{ struct Cyc_Std_String_pa_struct _temp261;
_temp261.tag= Cyc_Std_String_pa; _temp261.f1=( struct _tagged_arr) ans;{ void*
_temp260[ 2u]={& _temp261,& _temp262}; Cyc_Std_aprintf( _tag_arr("%smalloc %i",
sizeof( unsigned char), 12u), _tag_arr( _temp260, sizeof( void*), 2u));}}});
goto _LL220; _LL228: ans=({ struct Cyc_Std_String_pa_struct _temp266; _temp266.tag=
Cyc_Std_String_pa; _temp266.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp245);{ struct Cyc_Std_String_pa_struct _temp265; _temp265.tag= Cyc_Std_String_pa;
_temp265.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp247);{ struct
Cyc_Std_String_pa_struct _temp264; _temp264.tag= Cyc_Std_String_pa; _temp264.f1=(
struct _tagged_arr) ans;{ void* _temp263[ 3u]={& _temp264,& _temp265,& _temp266};
Cyc_Std_aprintf( _tag_arr("%sassign %s %s", sizeof( unsigned char), 15u),
_tag_arr( _temp263, sizeof( void*), 3u));}}}}); goto _LL220; _LL230:{ void*
_temp267= _temp251; _LL269: if( _temp267 == ( void*) Cyc_CfAbsexp_OrderedG){
goto _LL270;} else{ goto _LL271;} _LL271: if( _temp267 == ( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL272;} else{ goto _LL273;} _LL273: if( _temp267 == ( void*) Cyc_CfAbsexp_OneofG){
goto _LL274;} else{ goto _LL268;} _LL270: ans=({ struct Cyc_Std_String_pa_struct
_temp276; _temp276.tag= Cyc_Std_String_pa; _temp276.f1=( struct _tagged_arr) ans;{
void* _temp275[ 1u]={& _temp276}; Cyc_Std_aprintf( _tag_arr("%sordered", sizeof(
unsigned char), 10u), _tag_arr( _temp275, sizeof( void*), 1u));}}); goto _LL268;
_LL272: ans=({ struct Cyc_Std_String_pa_struct _temp278; _temp278.tag= Cyc_Std_String_pa;
_temp278.f1=( struct _tagged_arr) ans;{ void* _temp277[ 1u]={& _temp278}; Cyc_Std_aprintf(
_tag_arr("%sunordered", sizeof( unsigned char), 12u), _tag_arr( _temp277,
sizeof( void*), 1u));}}); goto _LL268; _LL274: ans=({ struct Cyc_Std_String_pa_struct
_temp280; _temp280.tag= Cyc_Std_String_pa; _temp280.f1=( struct _tagged_arr) ans;{
void* _temp279[ 1u]={& _temp280}; Cyc_Std_aprintf( _tag_arr("%soneof", sizeof(
unsigned char), 8u), _tag_arr( _temp279, sizeof( void*), 1u));}}); goto _LL268;
_LL268:;} for( 0; _temp249 !=  0; _temp249=(( struct Cyc_List_List*) _check_null(
_temp249))->tl){ ans=({ struct Cyc_Std_String_pa_struct _temp283; _temp283.tag=
Cyc_Std_String_pa; _temp283.f1=( struct _tagged_arr) Cyc_CfAbsexp_absexp2string((
void*)(( struct Cyc_List_List*) _check_null( _temp249))->hd, depth +  1);{
struct Cyc_Std_String_pa_struct _temp282; _temp282.tag= Cyc_Std_String_pa;
_temp282.f1=( struct _tagged_arr) ans;{ void* _temp281[ 2u]={& _temp282,&
_temp283}; Cyc_Std_aprintf( _tag_arr("%s\n%s", sizeof( unsigned char), 6u),
_tag_arr( _temp281, sizeof( void*), 2u));}}});} goto _LL220; _LL232: ans=({
struct Cyc_Std_String_pa_struct _temp285; _temp285.tag= Cyc_Std_String_pa;
_temp285.f1=( struct _tagged_arr) ans;{ void* _temp284[ 1u]={& _temp285}; Cyc_Std_aprintf(
_tag_arr("%s<<stmt>>", sizeof( unsigned char), 11u), _tag_arr( _temp284, sizeof(
void*), 1u));}}); goto _LL220; _LL234: ans=({ struct Cyc_Std_String_pa_struct
_temp287; _temp287.tag= Cyc_Std_String_pa; _temp287.f1=( struct _tagged_arr) ans;{
void* _temp286[ 1u]={& _temp287}; Cyc_Std_aprintf( _tag_arr("%sbottom", sizeof(
unsigned char), 9u), _tag_arr( _temp286, sizeof( void*), 1u));}}); goto _LL220;
_LL236:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL220:;} return( struct
_tagged_arr) ans;} static void Cyc_CfAbsexp_check_absop( void* ao){ void*
_temp288= ao; void* _temp302; void* _temp304; void* _temp306; _LL290: if(
_temp288 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL291;} else{ goto _LL292;}
_LL292: if(( unsigned int) _temp288 >  1u?*(( int*) _temp288) ==  Cyc_CfAbsexp_AddressOp:
0){ _LL303: _temp302=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp288)->f1;
goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int) _temp288 >  1u?*((
int*) _temp288) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL305: _temp304=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp288)->f1; goto _LL295;} else{ goto _LL296;}
_LL296: if(( unsigned int) _temp288 >  1u?*(( int*) _temp288) ==  Cyc_CfAbsexp_MemberOp:
0){ _LL307: _temp306=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp288)->f1;
goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int) _temp288 >  1u?*((
int*) _temp288) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL299;} else{ goto _LL300;}
_LL300: if(( unsigned int) _temp288 >  1u?*(( int*) _temp288) ==  Cyc_CfAbsexp_MallocOp:
0){ goto _LL301;} else{ goto _LL289;} _LL291: return; _LL293: Cyc_CfAbsexp_check_absop(
_temp302); Cyc_CfAbsexp_ok_address_arg( _temp302); return; _LL295: Cyc_CfAbsexp_check_absop(
_temp304); Cyc_CfAbsexp_ok_deref_arg( _temp304); return; _LL297: Cyc_CfAbsexp_check_absop(
_temp306); Cyc_CfAbsexp_ok_member_arg( _temp306); return; _LL299: return; _LL301:
return; _LL289:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp308= ae;
void* _temp326; struct Cyc_List_List* _temp328; void* _temp330; void* _temp332;
void* _temp334; _LL310: if( _temp308 == ( void*) Cyc_CfAbsexp_BottomAE){ goto
_LL311;} else{ goto _LL312;} _LL312: if( _temp308 == ( void*) Cyc_CfAbsexp_SkipAE){
goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int) _temp308 >  2u?*((
int*) _temp308) ==  Cyc_CfAbsexp_MallocAE: 0){ _LL327: _temp326=( void*)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp308)->f1; if(*(( int*) _temp326) == 
Cyc_CfFlowInfo_VarRoot){ goto _LL315;} else{ goto _LL316;}} else{ goto _LL316;}
_LL316: if(( unsigned int) _temp308 >  2u?*(( int*) _temp308) ==  Cyc_CfAbsexp_MallocAE:
0){ goto _LL317;} else{ goto _LL318;} _LL318: if(( unsigned int) _temp308 >  2u?*((
int*) _temp308) ==  Cyc_CfAbsexp_StmtAE: 0){ goto _LL319;} else{ goto _LL320;}
_LL320: if(( unsigned int) _temp308 >  2u?*(( int*) _temp308) ==  Cyc_CfAbsexp_GroupAE:
0){ _LL329: _temp328=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp308)->f2; goto
_LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp308 >  2u?*(( int*)
_temp308) ==  Cyc_CfAbsexp_UseAE: 0){ _LL331: _temp330=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*)
_temp308)->f1; goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int)
_temp308 >  2u?*(( int*) _temp308) ==  Cyc_CfAbsexp_AssignAE: 0){ _LL335:
_temp334=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp308)->f1; goto
_LL333; _LL333: _temp332=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp308)->f2; goto _LL325;} else{ goto _LL309;} _LL311: goto _LL313; _LL313:
return; _LL315:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL317: goto
_LL319; _LL319: return; _LL321: for( 0; _temp328 !=  0; _temp328=(( struct Cyc_List_List*)
_check_null( _temp328))->tl){ Cyc_CfAbsexp_check_absexp(( void*)(( struct Cyc_List_List*)
_check_null( _temp328))->hd);} return; _LL323: Cyc_CfAbsexp_check_absop(
_temp330); return; _LL325: Cyc_CfAbsexp_check_absop( _temp334); Cyc_CfAbsexp_check_absop(
_temp332); Cyc_CfAbsexp_ok_lvalue( _temp334); return; _LL309:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); struct Cyc_CfFlowInfo_InitState Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp336= ao; struct Cyc_Absyn_Vardecl*
_temp350; struct Cyc_Absyn_Exp* _temp352; struct _tagged_arr* _temp354; void*
_temp356; void* _temp358; _LL338: if(( unsigned int) _temp336 >  1u?*(( int*)
_temp336) ==  Cyc_CfAbsexp_LocalOp: 0){ _LL351: _temp350=(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp336)->f1; goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int)
_temp336 >  1u?*(( int*) _temp336) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL353:
_temp352=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp336)->f1; goto _LL341;}
else{ goto _LL342;} _LL342: if(( unsigned int) _temp336 >  1u?*(( int*) _temp336)
==  Cyc_CfAbsexp_MemberOp: 0){ _LL357: _temp356=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp336)->f1; goto _LL355; _LL355: _temp354=(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp336)->f2; goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int)
_temp336 >  1u?*(( int*) _temp336) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL359:
_temp358=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp336)->f1; goto
_LL345;} else{ goto _LL346;} _LL346: if( _temp336 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp336 >  1u?*((
int*) _temp336) ==  Cyc_CfAbsexp_AddressOp: 0){ goto _LL349;} else{ goto _LL337;}
_LL339: return({ struct Cyc_CfFlowInfo_InitState _temp360; _temp360.esc= Cyc_CfFlowInfo_Unesc;
_temp360.level=( void*)(( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp361=( struct Cyc_CfFlowInfo_MustPointTo_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp361[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp362; _temp362.tag= Cyc_CfFlowInfo_MustPointTo; _temp362.f1=({ struct Cyc_CfFlowInfo_Place*
_temp363=( struct Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp363->root=( void*)(( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp364=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp364[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp365; _temp365.tag= Cyc_CfFlowInfo_VarRoot;
_temp365.f1= _temp350; _temp365;}); _temp364;})); _temp363->fields= 0; _temp363;});
_temp362;}); _temp361;})); _temp360;}); _LL341: return({ struct Cyc_CfFlowInfo_InitState
_temp366; _temp366.esc= Cyc_CfFlowInfo_Unesc; _temp366.level=( void*)(( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp367=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp367[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp368; _temp368.tag= Cyc_CfFlowInfo_MustPointTo;
_temp368.f1=({ struct Cyc_CfFlowInfo_Place* _temp369=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp369->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp370=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp370[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp371; _temp371.tag= Cyc_CfFlowInfo_MallocPt;
_temp371.f1= _temp352; _temp371;}); _temp370;})); _temp369->fields= 0; _temp369;});
_temp368;}); _temp367;})); _temp366;}); _LL343: { struct Cyc_CfFlowInfo_InitState
_temp372= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp356); struct Cyc_CfFlowInfo_InitState
_temp373= _temp372; void* _temp379; struct Cyc_CfFlowInfo_Place* _temp381;
struct Cyc_CfFlowInfo_Place _temp383; struct Cyc_List_List* _temp384; void*
_temp386; _LL375: _LL380: _temp379=( void*) _temp373.level; if(( unsigned int)
_temp379 >  3u?*(( int*) _temp379) ==  Cyc_CfFlowInfo_MustPointTo: 0){ _LL382:
_temp381=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp379)->f1; _temp383=*
_temp381; _LL387: _temp386=( void*) _temp383.root; goto _LL385; _LL385: _temp384=
_temp383.fields; goto _LL376;} else{ goto _LL377;} _LL377: goto _LL378; _LL376:
_temp384=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp384,({ struct Cyc_List_List* _temp388=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp388->hd=( void*) _temp354;
_temp388->tl= 0; _temp388;})); return({ struct Cyc_CfFlowInfo_InitState _temp389;
_temp389.esc= Cyc_CfFlowInfo_Unesc; _temp389.level=( void*)(( void*)({ struct
Cyc_CfFlowInfo_MustPointTo_struct* _temp390=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp390[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp391; _temp391.tag= Cyc_CfFlowInfo_MustPointTo;
_temp391.f1=({ struct Cyc_CfFlowInfo_Place* _temp392=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp392->root=( void*)
_temp386; _temp392->fields= _temp384; _temp392;}); _temp391;}); _temp390;}));
_temp389;}); _LL378: return _temp372; _LL374:;} _LL345: { void* _temp393= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp358); void* _temp394= _temp393; struct Cyc_CfFlowInfo_InitState
_temp400; _LL396: if(*(( int*) _temp394) ==  Cyc_CfFlowInfo_LeafPI){ _LL401:
_temp400=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp394)->f1; goto _LL397;}
else{ goto _LL398;} _LL398: if(*(( int*) _temp394) ==  Cyc_CfFlowInfo_DictPI){
goto _LL399;} else{ goto _LL395;} _LL397: return _temp400; _LL399:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL395:;} _LL347: goto _LL349; _LL349:( int)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL337:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp402= ao; struct Cyc_Absyn_Vardecl*
_temp416; struct Cyc_Absyn_Exp* _temp418; struct _tagged_arr* _temp420; void*
_temp422; void* _temp424; void* _temp426; _LL404: if(( unsigned int) _temp402 > 
1u?*(( int*) _temp402) ==  Cyc_CfAbsexp_LocalOp: 0){ _LL417: _temp416=(( struct
Cyc_CfAbsexp_LocalOp_struct*) _temp402)->f1; goto _LL405;} else{ goto _LL406;}
_LL406: if(( unsigned int) _temp402 >  1u?*(( int*) _temp402) ==  Cyc_CfAbsexp_MallocOp:
0){ _LL419: _temp418=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp402)->f1;
goto _LL407;} else{ goto _LL408;} _LL408: if(( unsigned int) _temp402 >  1u?*((
int*) _temp402) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL423: _temp422=( void*)((
struct Cyc_CfAbsexp_MemberOp_struct*) _temp402)->f1; goto _LL421; _LL421:
_temp420=(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp402)->f2; goto _LL409;}
else{ goto _LL410;} _LL410: if(( unsigned int) _temp402 >  1u?*(( int*) _temp402)
==  Cyc_CfAbsexp_DerefOp: 0){ _LL425: _temp424=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp402)->f1; goto _LL411;} else{ goto _LL412;} _LL412: if( _temp402 == ( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL413;} else{ goto _LL414;} _LL414: if((
unsigned int) _temp402 >  1u?*(( int*) _temp402) ==  Cyc_CfAbsexp_AddressOp: 0){
_LL427: _temp426=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp402)->f1;
goto _LL415;} else{ goto _LL403;} _LL405: return Cyc_Dict_lookup( pinfo_dict,(
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp428=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp428[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp429; _temp429.tag= Cyc_CfFlowInfo_VarRoot;
_temp429.f1= _temp416; _temp429;}); _temp428;})); _LL407: return Cyc_Dict_lookup(
pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp430=( struct
Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp430[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp431; _temp431.tag=
Cyc_CfFlowInfo_MallocPt; _temp431.f1= _temp418; _temp431;}); _temp430;}));
_LL409: { void* _temp432= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp422); void*
_temp433= _temp432; struct Cyc_Dict_Dict* _temp439; _LL435: if(*(( int*)
_temp433) ==  Cyc_CfFlowInfo_LeafPI){ goto _LL436;} else{ goto _LL437;} _LL437:
if(*(( int*) _temp433) ==  Cyc_CfFlowInfo_DictPI){ _LL440: _temp439=(( struct
Cyc_CfFlowInfo_DictPI_struct*) _temp433)->f1; goto _LL438;} else{ goto _LL434;}
_LL436: return _temp432; _LL438: return(( void*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp439, _temp420); _LL434:;} _LL411: {
void* _temp441= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp424); void* _temp442=
_temp441; struct Cyc_CfFlowInfo_InitState _temp452; void* _temp454; struct Cyc_CfFlowInfo_InitState
_temp456; void* _temp458; struct Cyc_CfFlowInfo_Place* _temp460; struct Cyc_CfFlowInfo_InitState
_temp462; _LL444: if(*(( int*) _temp442) ==  Cyc_CfFlowInfo_LeafPI){ _LL453:
_temp452=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp442)->f1; _LL455:
_temp454=( void*) _temp452.level; if( _temp454 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL445;} else{ goto _LL446;}} else{ goto _LL446;} _LL446: if(*(( int*)
_temp442) ==  Cyc_CfFlowInfo_LeafPI){ _LL457: _temp456=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp442)->f1; _LL459: _temp458=( void*) _temp456.level; if(( unsigned int)
_temp458 >  3u?*(( int*) _temp458) ==  Cyc_CfFlowInfo_MustPointTo: 0){ _LL461:
_temp460=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp458)->f1; goto
_LL447;} else{ goto _LL448;}} else{ goto _LL448;} _LL448: if(*(( int*) _temp442)
==  Cyc_CfFlowInfo_LeafPI){ _LL463: _temp462=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp442)->f1; goto _LL449;} else{ goto _LL450;} _LL450: goto _LL451; _LL445:
return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_NoneIL);
_LL447: return Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp460); _LL449:
return _temp441; _LL451:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL443:;}
_LL413: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL);
_LL415: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct* _temp464=( struct
Cyc_CfFlowInfo_LeafPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp464[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp465; _temp465.tag= Cyc_CfFlowInfo_LeafPI;
_temp465.f1= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp426); _temp465;});
_temp464;}); _LL403:;} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo); struct Cyc_Dict_Dict* Cyc_CfAbsexp_assign_escape_f(
struct Cyc_List_List** escaping_states, void* key, void* b1, void* b2, struct
Cyc_Dict_Dict* accum){ return Cyc_Dict_insert( accum, key, Cyc_CfAbsexp_assign_escape(
escaping_states, b1, b2));} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp467=({
struct _tuple3 _temp466; _temp466.f1= old_pinfo; _temp466.f2= new_pinfo;
_temp466;}); void* _temp477; struct Cyc_CfFlowInfo_InitState _temp479; void*
_temp481; void* _temp483; struct Cyc_CfFlowInfo_InitState _temp485; enum  Cyc_CfFlowInfo_Escaped
_temp487; void* _temp489; void* _temp491; struct Cyc_CfFlowInfo_InitState
_temp493; enum  Cyc_CfFlowInfo_Escaped _temp495; void* _temp497; struct Cyc_Dict_Dict*
_temp499; void* _temp501; struct Cyc_Dict_Dict* _temp503; _LL469: _LL484:
_temp483= _temp467.f1; if(*(( int*) _temp483) ==  Cyc_CfFlowInfo_LeafPI){ _LL486:
_temp485=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp483)->f1; _LL488:
_temp487= _temp485.esc; if( _temp487 ==  Cyc_CfFlowInfo_Unesc){ goto _LL478;}
else{ goto _LL471;}} else{ goto _LL471;} _LL478: _temp477= _temp467.f2; if(*((
int*) _temp477) ==  Cyc_CfFlowInfo_LeafPI){ _LL480: _temp479=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp477)->f1; _LL482: _temp481=( void*) _temp479.level; goto _LL470;} else{
goto _LL471;} _LL471: _LL492: _temp491= _temp467.f1; if(*(( int*) _temp491) == 
Cyc_CfFlowInfo_LeafPI){ _LL494: _temp493=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp491)->f1; _LL496: _temp495= _temp493.esc; if( _temp495 ==  Cyc_CfFlowInfo_Esc){
goto _LL490;} else{ goto _LL473;}} else{ goto _LL473;} _LL490: _temp489=
_temp467.f2; if(*(( int*) _temp489) ==  Cyc_CfFlowInfo_LeafPI){ goto _LL472;}
else{ goto _LL473;} _LL473: _LL502: _temp501= _temp467.f1; if(*(( int*) _temp501)
==  Cyc_CfFlowInfo_DictPI){ _LL504: _temp503=(( struct Cyc_CfFlowInfo_DictPI_struct*)
_temp501)->f1; goto _LL498;} else{ goto _LL475;} _LL498: _temp497= _temp467.f2;
if(*(( int*) _temp497) ==  Cyc_CfFlowInfo_DictPI){ _LL500: _temp499=(( struct
Cyc_CfFlowInfo_DictPI_struct*) _temp497)->f1; goto _LL474;} else{ goto _LL475;}
_LL475: goto _LL476; _LL470: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc,
_temp481); _LL472:* escaping_states=({ struct Cyc_List_List* _temp505=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp505->hd=( void*)
new_pinfo; _temp505->tl=* escaping_states; _temp505;}); return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL474: return( void*)({
struct Cyc_CfFlowInfo_DictPI_struct* _temp506=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp506[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp507; _temp507.tag= Cyc_CfFlowInfo_DictPI;
_temp507.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
struct _tagged_arr*, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List**
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict*
accum)) Cyc_Dict_fold2_c)(( struct Cyc_Dict_Dict*(*)( struct Cyc_List_List**
escaping_states, struct _tagged_arr* key, void* b1, void* b2, struct Cyc_Dict_Dict*
accum)) Cyc_CfAbsexp_assign_escape_f, escaping_states, _temp503, _temp499,((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp)); _temp507;}); _temp506;}); _LL476:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp508=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp508[ 0]=({ struct
Cyc_Core_Impossible_struct _temp509; _temp509.tag= Cyc_Core_Impossible; _temp509.f1=
_tag_arr("bad pinfos in assign_escape", sizeof( unsigned char), 28u); _temp509;});
_temp508;})); _LL468:;} void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, void* ae, void* in_flow){ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp510= in_flow; struct Cyc_Dict_Dict* _temp516; _LL512:
if( _temp510 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL513;} else{ goto
_LL514;} _LL514: if(( unsigned int) _temp510 >  1u?*(( int*) _temp510) ==  Cyc_CfFlowInfo_InitsFL:
0){ _LL517: _temp516=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp510)->f1;
goto _LL515;} else{ goto _LL511;} _LL513: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL515: pinfo_dict= _temp516; goto _LL511; _LL511:;}{ void* _temp518= ae; void*
_temp538; void* _temp540; void* _temp542; void* _temp544; struct Cyc_Absyn_Stmt*
_temp546; struct Cyc_List_List* _temp548; void* _temp550; struct Cyc_List_List*
_temp552; void* _temp554; struct Cyc_List_List* _temp556; void* _temp558; _LL520:
if(( unsigned int) _temp518 >  2u?*(( int*) _temp518) ==  Cyc_CfAbsexp_AssignAE:
0){ _LL541: _temp540=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp518)->f1;
goto _LL539; _LL539: _temp538=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp518)->f2; goto _LL521;} else{ goto _LL522;} _LL522: if(( unsigned int)
_temp518 >  2u?*(( int*) _temp518) ==  Cyc_CfAbsexp_UseAE: 0){ _LL543: _temp542=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp518)->f1; goto _LL523;} else{
goto _LL524;} _LL524: if(( unsigned int) _temp518 >  2u?*(( int*) _temp518) == 
Cyc_CfAbsexp_MallocAE: 0){ _LL545: _temp544=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp518)->f1; goto _LL525;} else{ goto _LL526;} _LL526: if( _temp518 == ( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL527;} else{ goto _LL528;} _LL528: if( _temp518
== ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL529;} else{ goto _LL530;} _LL530: if((
unsigned int) _temp518 >  2u?*(( int*) _temp518) ==  Cyc_CfAbsexp_StmtAE: 0){
_LL547: _temp546=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp518)->f1; goto
_LL531;} else{ goto _LL532;} _LL532: if(( unsigned int) _temp518 >  2u?*(( int*)
_temp518) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL551: _temp550=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp518)->f1; if( _temp550 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL549;}
else{ goto _LL534;} _LL549: _temp548=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp518)->f2; goto _LL533;} else{ goto _LL534;} _LL534: if(( unsigned int)
_temp518 >  2u?*(( int*) _temp518) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL555:
_temp554=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp518)->f1; if(
_temp554 == ( void*) Cyc_CfAbsexp_OneofG){ goto _LL553;} else{ goto _LL536;}
_LL553: _temp552=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp518)->f2; goto
_LL535;} else{ goto _LL536;} _LL536: if(( unsigned int) _temp518 >  2u?*(( int*)
_temp518) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL559: _temp558=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp518)->f1; if( _temp558 == ( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL557;}
else{ goto _LL519;} _LL557: _temp556=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp518)->f2; goto _LL537;} else{ goto _LL519;} _LL521:{ struct Cyc_CfFlowInfo_InitState
_temp560= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp540); void* _temp566;
struct Cyc_CfFlowInfo_Place* _temp568; _LL562: _LL567: _temp566=( void*)
_temp560.level; if(( unsigned int) _temp566 >  3u?*(( int*) _temp566) ==  Cyc_CfFlowInfo_MustPointTo:
0){ _LL569: _temp568=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp566)->f1;
goto _LL563;} else{ goto _LL564;} _LL564: goto _LL565; _LL563: { void* _temp570=
Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp568); void* _temp571= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp538);{ struct _tuple3 _temp573=({ struct _tuple3 _temp572;
_temp572.f1= _temp570; _temp572.f2= _temp571; _temp572;}); void* _temp579;
struct Cyc_CfFlowInfo_InitState _temp581; void* _temp583; void* _temp585; _LL575:
_LL586: _temp585= _temp573.f1; if(*(( int*) _temp585) ==  Cyc_CfFlowInfo_DictPI){
goto _LL580;} else{ goto _LL577;} _LL580: _temp579= _temp573.f2; if(*(( int*)
_temp579) ==  Cyc_CfFlowInfo_LeafPI){ _LL582: _temp581=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp579)->f1; _LL584: _temp583=( void*) _temp581.level; goto _LL576;} else{
goto _LL577;} _LL577: goto _LL578; _LL576:{ void* _temp587= _temp583; _LL589:
if(( unsigned int) _temp587 >  3u?*(( int*) _temp587) ==  Cyc_CfFlowInfo_MustPointTo:
0){ goto _LL590;} else{ goto _LL591;} _LL591: goto _LL592; _LL590: goto _LL588;
_LL592: _temp571= Cyc_CfFlowInfo_assign_unknown_dict( _temp583, _temp570); goto
_LL588; _LL588:;} goto _LL574; _LL578: goto _LL574; _LL574:;} if( env->all_changed
!=  0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*((
struct Cyc_Set_Set**) _check_null( env->all_changed)), _temp568);}{ struct Cyc_List_List*
_temp593= 0; void* _temp594= Cyc_CfAbsexp_assign_escape(& _temp593, _temp570,
_temp571); for( 0; _temp593 !=  0; _temp593=(( struct Cyc_List_List*)
_check_null( _temp593))->tl){ void* _temp595=( void*)(( struct Cyc_List_List*)
_check_null( _temp593))->hd; if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict,
_temp595)){({ void* _temp596[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("uninitialized value assigned to escaped place",
sizeof( unsigned char), 46u), _tag_arr( _temp596, sizeof( void*), 0u));});
return( void*) Cyc_CfFlowInfo_BottomFL;} pinfo_dict= Cyc_CfFlowInfo_escape_pointsto(
_temp595, pinfo_dict, env->all_changed);} pinfo_dict= Cyc_CfFlowInfo_insert_place(
pinfo_dict, _temp568, _temp594);{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp597=({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp598=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp598[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp599; _temp599.tag= Cyc_CfFlowInfo_InitsFL;
_temp599.f1= pinfo_dict; _temp599;}); _temp598;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp597); return( void*) _temp597;}}} _LL565: goto _LL561; _LL561:;}
_temp542= _temp538; goto _LL523; _LL523: { void* _temp600= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp542); if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp600)){
void* _temp601= _temp542; struct Cyc_Absyn_Vardecl* _temp607; _LL603: if((
unsigned int) _temp601 >  1u?*(( int*) _temp601) ==  Cyc_CfAbsexp_LocalOp: 0){
_LL608: _temp607=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp601)->f1; goto
_LL604;} else{ goto _LL605;} _LL605: goto _LL606; _LL604: if( Cyc_Tcutil_bits_only((
void*) _temp607->type)){({ struct Cyc_Std_String_pa_struct _temp610; _temp610.tag=
Cyc_Std_String_pa; _temp610.f1=( struct _tagged_arr)*(* _temp607->name).f2;{
void* _temp609[ 1u]={& _temp610}; Cyc_Tcutil_warn( loc, _tag_arr("pointer-free %s may not be fully initialized",
sizeof( unsigned char), 45u), _tag_arr( _temp609, sizeof( void*), 1u));}}); goto
_LL602;} goto _LL606; _LL606:({ struct Cyc_Std_String_pa_struct _temp612;
_temp612.tag= Cyc_Std_String_pa; _temp612.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp542);{ void* _temp611[ 1u]={& _temp612}; Cyc_Tcutil_terr( loc, _tag_arr("%s must be fully initialized to be used here",
sizeof( unsigned char), 45u), _tag_arr( _temp611, sizeof( void*), 1u));}});
return( void*) Cyc_CfFlowInfo_BottomFL; _LL602:;}{ struct Cyc_Dict_Dict*
_temp613= Cyc_CfFlowInfo_escape_pointsto( _temp600, pinfo_dict, env->all_changed);
if( _temp613 ==  pinfo_dict){ return in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp614=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp615=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp615[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp616; _temp616.tag= Cyc_CfFlowInfo_InitsFL;
_temp616.f1= pinfo_dict; _temp616;}); _temp615;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp614); return( void*) _temp614;}}} _LL525: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp617=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp617[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp618; _temp618.tag= Cyc_CfFlowInfo_InitsFL;
_temp618.f1= Cyc_Dict_insert( pinfo_dict, _temp544, Cyc_Dict_lookup( env->roots,
_temp544)); _temp618;}); _temp617;}); _LL527: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL529: return in_flow; _LL531: return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp546, in_flow); _LL533: for( 0; _temp548 !=  0; _temp548=(( struct Cyc_List_List*)
_check_null( _temp548))->tl){ in_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)((
struct Cyc_List_List*) _check_null( _temp548))->hd, in_flow);} return in_flow;
_LL535: { void* out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp552 != 
0; _temp552=(( struct Cyc_List_List*) _check_null( _temp552))->tl){ out_flow=
Cyc_CfFlowInfo_join_flow( env->all_changed, out_flow, Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp552))->hd, in_flow));}
return out_flow;} _LL537: if( _temp556 ==  0){ return in_flow;}{ struct Cyc_Set_Set**
_temp619= env->all_changed; struct Cyc_Set_Set* this_all_changed; void*
old_in_flow; void* out_flow; do { this_all_changed= Cyc_CfFlowInfo_mt_place_set();
env->all_changed=({ struct Cyc_Set_Set** _temp620=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp620[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp620;}); out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp556))->hd, in_flow); this_all_changed=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));{
struct Cyc_List_List* ael2=(( struct Cyc_List_List*) _check_null( _temp556))->tl;
for( 0; ael2 !=  0; ael2=(( struct Cyc_List_List*) _check_null( ael2))->tl){ env->all_changed=({
struct Cyc_Set_Set** _temp621=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp621[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp621;});{ void*
_temp622= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( ael2))->hd, in_flow); out_flow= Cyc_CfFlowInfo_after_flow(( struct
Cyc_Set_Set**)& this_all_changed, out_flow, _temp622, this_all_changed,*((
struct Cyc_Set_Set**) _check_null( env->all_changed))); this_all_changed=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));}}}
old_in_flow= in_flow; in_flow= Cyc_CfFlowInfo_join_flow( _temp619, in_flow,
out_flow);} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( in_flow, old_in_flow));
if( _temp619 ==  0){ env->all_changed= 0;} else{ env->all_changed=({ struct Cyc_Set_Set**
_temp623=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp623[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)(*(( struct Cyc_Set_Set**) _check_null( _temp619)),
this_all_changed); _temp623;});} return out_flow;} _LL519:;}}
