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
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_vfprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr ap); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int
Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*,
void*), void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern void Cyc_List_iter_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle*, struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_forall(
int(* pred)( void*), struct Cyc_List_List* x); extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k,
void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
k); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
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
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_AnonEnumType=
14; struct Cyc_Absyn_AnonEnumType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnHandleType= 15; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 16; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null;
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void*
Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_arr* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_arr* v,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp*
init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0*
x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue( struct
Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field(
struct Cyc_List_List*, struct _tagged_arr*); extern struct _tagged_arr* Cyc_Absyn_fieldname(
int); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*); extern
struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*); extern
struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); static const int
Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern
void* Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List* Cyc_Toc_result_decls=
0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; static int Cyc_Toc_temp_var_counter= 0; static int Cyc_Toc_fresh_label_counter=
0; unsigned char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
unsigned char Cyc_Toc_Toc_Impossible[ 19u]="\000\000\000\000Toc_Impossible";
static void* Cyc_Toc_unimp( struct _tagged_arr fmt, struct _tagged_arr ap){ Cyc_Std_vfprintf(
Cyc_Std_stderr, fmt, ap);({ void* _temp0[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp0, sizeof( void*), 0u));});
Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*)
Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct _tagged_arr
fmt, struct _tagged_arr ap){ Cyc_Std_vfprintf( Cyc_Std_stderr, fmt, ap);({ void*
_temp1[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static
unsigned char _temp2[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={
_temp2, _temp2, _temp2 +  5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _temp3[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp3, _temp3, _temp3 +  4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp4[ 14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp4, _temp4, _temp4 +  14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp5[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp5, _temp5, _temp5 +  8u};
static struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp6[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp6, _temp6, _temp6 +  14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp7[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp7, _temp7, _temp7 +  7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)((
void*)& Cyc_Toc__throw_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=&
Cyc_Toc__throw_ev; static unsigned char _temp9[ 7u]="setjmp"; static struct
_tagged_arr Cyc_Toc_setjmp_str={ _temp9, _temp9, _temp9 +  7u}; static struct
_tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,& Cyc_Toc_setjmp_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={ 0,( void*)((
void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp11[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp11, _temp11, _temp11 +  14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_handler_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp13[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp13, _temp13, _temp13 +  13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__pop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=&
Cyc_Toc__pop_handler_ev; static unsigned char _temp15[ 12u]="_exn_thrown";
static struct _tagged_arr Cyc_Toc__exn_thrown_str={ _temp15, _temp15, _temp15 + 
12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp17[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp17, _temp17, _temp17 +  14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp19[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp19, _temp19, _temp19 + 
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1u,& Cyc_Toc__check_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_null_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;
static unsigned char _temp21[ 28u]="_check_known_subscript_null"; static struct
_tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp21, _temp21, _temp21
+  28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str}; static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,& Cyc_Toc__check_known_subscript_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re), 0}; static struct
Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp23[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp23, _temp23,
_temp23 +  31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp25[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp25, _temp25, _temp25 + 
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp27[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp27, _temp27, _temp27 +  12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__tagged_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=&
Cyc_Toc__tagged_arr_ev; static unsigned char _temp29[ 9u]="_tag_arr"; static
struct _tagged_arr Cyc_Toc__tag_arr_str={ _temp29, _temp29, _temp29 +  9u};
static struct _tuple0 Cyc_Toc__tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={
1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,( void*)(( void*)& Cyc_Toc__tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=&
Cyc_Toc__tag_arr_ev; static unsigned char _temp31[ 14u]="_init_tag_arr"; static
struct _tagged_arr Cyc_Toc__init_tag_arr_str={ _temp31, _temp31, _temp31 +  14u};
static struct _tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__init_tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={
1u,& Cyc_Toc__init_tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__init_tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=& Cyc_Toc__init_tag_arr_ev;
static unsigned char _temp33[ 11u]="_untag_arr"; static struct _tagged_arr Cyc_Toc__untag_arr_str={
_temp33, _temp33, _temp33 +  11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__untag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;
static unsigned char _temp35[ 14u]="_get_arr_size"; static struct _tagged_arr
Cyc_Toc__get_arr_size_str={ _temp35, _temp35, _temp35 +  14u}; static struct
_tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={ 1u,& Cyc_Toc__get_arr_size_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={ 0,(
void*)(( void*)& Cyc_Toc__get_arr_size_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev; static unsigned char _temp37[ 17u]="_tagged_arr_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={ _temp37, _temp37,
_temp37 +  17u}; static struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={ 1u,& Cyc_Toc__tagged_arr_plus_pr,( void*)(( void*)
0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_plus_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_plus_e=&
Cyc_Toc__tagged_arr_plus_ev; static unsigned char _temp39[ 25u]="_tagged_arr_inplace_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={ _temp39,
_temp39, _temp39 +  25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={ 1u,&
Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp41[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp41, _temp41,
_temp41 +  30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp43[ 10u]="_cycalloc"; static struct _tagged_arr Cyc_Toc__cycalloc_str={
_temp43, _temp43, _temp43 +  10u}; static struct _tuple0 Cyc_Toc__cycalloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={ 1u,& Cyc_Toc__cycalloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={ 0,( void*)(( void*)& Cyc_Toc__cycalloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static unsigned char _temp45[ 17u]="_cycalloc_atomic"; static struct _tagged_arr
Cyc_Toc__cycalloc_atomic_str={ _temp45, _temp45, _temp45 +  17u}; static struct
_tuple0 Cyc_Toc__cycalloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={ 1u,& Cyc_Toc__cycalloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc__cycalloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev; static unsigned char
_temp47[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp47, _temp47, _temp47 +  15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp49[ 13u]="_check_times"; static struct _tagged_arr Cyc_Toc__check_times_str={
_temp49, _temp49, _temp49 +  13u}; static struct _tuple0 Cyc_Toc__check_times_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={ 1u,& Cyc_Toc__check_times_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_times_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev; static unsigned char _temp51[ 12u]="_new_region";
static struct _tagged_arr Cyc_Toc__new_region_str={ _temp51, _temp51, _temp51 + 
12u}; static struct _tuple0 Cyc_Toc__new_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__new_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp53[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp53, _temp53, _temp53 +  13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__push_region_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev; static unsigned char _temp55[ 12u]="_pop_region";
static struct _tagged_arr Cyc_Toc__pop_region_str={ _temp55, _temp55, _temp55 + 
12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static struct Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={ 9u,( struct
_tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0}; static void* Cyc_Toc_tagged_arr_typ=(
void*)& Cyc_Toc_tagged_arr_typ_v; static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={
0, 0, 0}; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; }
; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp58=( void*) e->r;
void* _temp84; unsigned char _temp86; void* _temp88; short _temp90; void*
_temp92; int _temp94; void* _temp96; long long _temp98; void* _temp100; struct
Cyc_Absyn_Exp* _temp102; struct Cyc_List_List* _temp104; struct Cyc_List_List*
_temp106; struct Cyc_List_List* _temp108; struct Cyc_List_List* _temp110; struct
Cyc_List_List* _temp112; _LL60: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL85: _temp84=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp84 >  1u?*(( int*) _temp84) ==  Cyc_Absyn_Char_c: 0){ _LL87:
_temp86=(( struct Cyc_Absyn_Char_c_struct*) _temp84)->f2; goto _LL61;} else{
goto _LL62;}} else{ goto _LL62;} _LL62: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL89: _temp88=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp88 >  1u?*(( int*) _temp88) ==  Cyc_Absyn_Short_c: 0){ _LL91:
_temp90=(( struct Cyc_Absyn_Short_c_struct*) _temp88)->f2; goto _LL63;} else{
goto _LL64;}} else{ goto _LL64;} _LL64: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL93: _temp92=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp92 >  1u?*(( int*) _temp92) ==  Cyc_Absyn_Int_c: 0){ _LL95:
_temp94=(( struct Cyc_Absyn_Int_c_struct*) _temp92)->f2; goto _LL65;} else{ goto
_LL66;}} else{ goto _LL66;} _LL66: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL97: _temp96=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp96 >  1u?*(( int*) _temp96) ==  Cyc_Absyn_LongLong_c: 0){
_LL99: _temp98=(( struct Cyc_Absyn_LongLong_c_struct*) _temp96)->f2; goto _LL67;}
else{ goto _LL68;}} else{ goto _LL68;} _LL68: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if(
_temp100 == ( void*) Cyc_Absyn_Null_c){ goto _LL69;} else{ goto _LL70;}} else{
goto _LL70;} _LL70: if(*(( int*) _temp58) ==  Cyc_Absyn_Cast_e){ _LL103:
_temp102=(( struct Cyc_Absyn_Cast_e_struct*) _temp58)->f2; goto _LL71;} else{
goto _LL72;} _LL72: if(*(( int*) _temp58) ==  Cyc_Absyn_Tuple_e){ _LL105:
_temp104=(( struct Cyc_Absyn_Tuple_e_struct*) _temp58)->f1; goto _LL73;} else{
goto _LL74;} _LL74: if(*(( int*) _temp58) ==  Cyc_Absyn_Array_e){ _LL107:
_temp106=(( struct Cyc_Absyn_Array_e_struct*) _temp58)->f1; goto _LL75;} else{
goto _LL76;} _LL76: if(*(( int*) _temp58) ==  Cyc_Absyn_Struct_e){ _LL109:
_temp108=(( struct Cyc_Absyn_Struct_e_struct*) _temp58)->f3; goto _LL77;} else{
goto _LL78;} _LL78: if(*(( int*) _temp58) ==  Cyc_Absyn_CompoundLit_e){ _LL111:
_temp110=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp58)->f2; goto _LL79;}
else{ goto _LL80;} _LL80: if(*(( int*) _temp58) ==  Cyc_Absyn_UnresolvedMem_e){
_LL113: _temp112=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp58)->f2; goto
_LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL61: return _temp86 == '\000';
_LL63: return _temp90 ==  0; _LL65: return _temp94 ==  0; _LL67: return _temp98
==  0; _LL69: return 1; _LL71: return Cyc_Toc_is_zero( _temp102); _LL73: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp104); _LL75: _temp108= _temp106; goto _LL77; _LL77:
_temp110= _temp108; goto _LL79; _LL79: _temp112= _temp110; goto _LL81; _LL81:
for( 0; _temp112 !=  0; _temp112=(( struct Cyc_List_List*) _check_null( _temp112))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp112))->hd)).f2)){ return 0;}} return 1; _LL83: return 0; _LL59:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp114= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp120; struct Cyc_Absyn_Conref* _temp122; _LL116:
if(( unsigned int) _temp114 >  4u?*(( int*) _temp114) ==  Cyc_Absyn_PointerType:
0){ _LL121: _temp120=(( struct Cyc_Absyn_PointerType_struct*) _temp114)->f1;
_LL123: _temp122= _temp120.nullable; goto _LL117;} else{ goto _LL118;} _LL118:
goto _LL119; _LL117: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp122); _LL119:({ void* _temp124[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable", sizeof(
unsigned char), 12u), _tag_arr( _temp124, sizeof( void*), 0u));}); return 0;
_LL115:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x,
struct _tagged_arr tag){ return({ struct _tuple0* _temp125=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp125->f1=(* x).f1; _temp125->f2=({
struct _tagged_arr* _temp126=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp126[ 0]=( struct _tagged_arr) Cyc_Std_strconcat(*(* x).f2,
tag); _temp126;}); _temp125;});} struct _tuple5{ struct _tagged_arr* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct
Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp127= Cyc_Toc_tuple_types; for(
0; _temp127 !=  0; _temp127=(( struct Cyc_List_List*) _check_null( _temp127))->tl){
struct _tuple5 _temp130; struct Cyc_List_List* _temp131; struct _tagged_arr*
_temp133; struct _tuple5* _temp128=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp127))->hd; _temp130=* _temp128; _LL134: _temp133= _temp130.f1;
goto _LL132; _LL132: _temp131= _temp130.f2; goto _LL129; _LL129: { struct Cyc_List_List*
_temp135= tqs0; for( 0; _temp135 !=  0? _temp131 !=  0: 0;( _temp135=(( struct
Cyc_List_List*) _check_null( _temp135))->tl, _temp131=(( struct Cyc_List_List*)
_check_null( _temp131))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp135))->hd)).f2,( void*)(( struct Cyc_List_List*)
_check_null( _temp131))->hd)){ break;}} if( _temp135 ==  0? _temp131 ==  0: 0){
return _temp133;}}}}{ struct _tagged_arr* x=({ struct _tagged_arr* _temp149=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp149[ 0]=(
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp151; _temp151.tag= Cyc_Std_Int_pa;
_temp151.f1=( int)(( unsigned int) Cyc_Toc_tuple_type_counter ++);{ void*
_temp150[ 1u]={& _temp151}; Cyc_Std_aprintf( _tag_arr("_tuple%d", sizeof(
unsigned char), 9u), _tag_arr( _temp150, sizeof( void*), 1u));}}); _temp149;});
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List*
_temp136= 0; struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 !=  0;( ts2=((
struct Cyc_List_List*) _check_null( ts2))->tl, i ++)){ _temp136=({ struct Cyc_List_List*
_temp137=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp137->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp138=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp138->name= Cyc_Absyn_fieldname(
i); _temp138->tq= Cyc_Toc_mt_tq; _temp138->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp138->width= 0; _temp138->attributes= 0; _temp138;});
_temp137->tl= _temp136; _temp137;});}} _temp136=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp136);{ struct Cyc_Absyn_Structdecl*
_temp139=({ struct Cyc_Absyn_Structdecl* _temp145=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp145->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp145->name=({ struct Cyc_Core_Opt* _temp147=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp147->v=( void*)({ struct _tuple0*
_temp148=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp148->f1=
Cyc_Absyn_rel_ns_null; _temp148->f2= x; _temp148;}); _temp147;}); _temp145->tvs=
0; _temp145->fields=({ struct Cyc_Core_Opt* _temp146=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp146->v=( void*) _temp136;
_temp146;}); _temp145->attributes= 0; _temp145;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp140=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp140->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp141=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp141[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp142; _temp142.tag= Cyc_Absyn_Struct_d; _temp142.f1=
_temp139; _temp142;}); _temp141;}), 0); _temp140->tl= Cyc_Toc_result_decls;
_temp140;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp143=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp143->hd=( void*)({ struct
_tuple5* _temp144=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp144->f1= x; _temp144->f2= ts; _temp144;}); _temp143->tl= Cyc_Toc_tuple_types;
_temp143;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp152=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp152->f1=( void*) Cyc_Absyn_Loc_n; _temp152->f2=({ struct _tagged_arr*
_temp153=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp153[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp155;
_temp155.tag= Cyc_Std_Int_pa; _temp155.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp154[ 1u]={& _temp155}; Cyc_Std_aprintf( _tag_arr("_temp%d",
sizeof( unsigned char), 8u), _tag_arr( _temp154, sizeof( void*), 1u));}});
_temp153;}); _temp152;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp156=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp156[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp158; _temp158.tag= Cyc_Std_Int_pa; _temp158.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp157[ 1u]={& _temp158}; Cyc_Std_aprintf( _tag_arr("_LL%d",
sizeof( unsigned char), 6u), _tag_arr( _temp157, sizeof( void*), 1u));}});
_temp156;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp161; struct Cyc_Absyn_Tqual
_temp163; struct Cyc_Core_Opt* _temp165; struct _tuple1 _temp159=* a; _LL166:
_temp165= _temp159.f1; goto _LL164; _LL164: _temp163= _temp159.f2; goto _LL162;
_LL162: _temp161= _temp159.f3; goto _LL160; _LL160: return({ struct _tuple1*
_temp167=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp167->f1=
_temp165; _temp167->f2= _temp163; _temp167->f3= Cyc_Toc_typ_to_c( _temp161);
_temp167;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp170; struct Cyc_Absyn_Tqual _temp172; struct _tuple3 _temp168=* x;
_LL173: _temp172= _temp168.f1; goto _LL171; _LL171: _temp170= _temp168.f2; goto
_LL169; _LL169: return({ struct _tuple3* _temp174=( struct _tuple3*) _cycalloc(
sizeof( struct _tuple3)); _temp174->f1= _temp172; _temp174->f2= Cyc_Toc_typ_to_c(
_temp170); _temp174;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp175= t; struct Cyc_Absyn_Exp* _temp183; struct Cyc_Absyn_Tqual _temp185;
void* _temp187; struct Cyc_Core_Opt* _temp189; struct Cyc_Core_Opt _temp191;
void* _temp192; _LL177: if(( unsigned int) _temp175 >  4u?*(( int*) _temp175) == 
Cyc_Absyn_ArrayType: 0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f1; goto _LL186; _LL186: _temp185=(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f2; goto _LL184; _LL184: _temp183=(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f3; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp175 >  4u?*(( int*) _temp175) ==  Cyc_Absyn_Evar: 0){ _LL190: _temp189=((
struct Cyc_Absyn_Evar_struct*) _temp175)->f2; if( _temp189 ==  0){ goto _LL181;}
else{ _temp191=* _temp189; _LL193: _temp192=( void*) _temp191.v; goto _LL180;}}
else{ goto _LL181;} _LL181: goto _LL182; _LL178: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp194=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp194[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp195; _temp195.tag= Cyc_Absyn_ArrayType;
_temp195.f1=( void*) Cyc_Toc_typ_to_c_array( _temp187); _temp195.f2= _temp185;
_temp195.f3= _temp183; _temp195;}); _temp194;}); _LL180: return Cyc_Toc_typ_to_c_array(
_temp192); _LL182: return Cyc_Toc_typ_to_c( t); _LL176:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp196=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp196->name= f->name; _temp196->tq= f->tq; _temp196->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp196->width= f->width; _temp196->attributes= f->attributes;
_temp196;});} static void Cyc_Toc_enumfields_to_c( struct Cyc_List_List* fs){
struct Cyc_List_List* _temp197= fs; for( 0; _temp197 !=  0; _temp197=(( struct
Cyc_List_List*) _check_null( _temp197))->tl){ if((( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp197))->hd)->tag !=  0){ unsigned int i=
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp197))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp197))->hd)->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}} static void* Cyc_Toc_typ_to_c( void* t){ void*
_temp198= t; struct Cyc_Core_Opt* _temp254; struct Cyc_Core_Opt* _temp256;
struct Cyc_Core_Opt _temp258; void* _temp259; struct Cyc_Absyn_TunionInfo
_temp261; void* _temp263; struct Cyc_Absyn_TunionFieldInfo _temp265; void*
_temp267; struct Cyc_Absyn_Tunionfield* _temp269; struct Cyc_Absyn_Tuniondecl*
_temp271; struct Cyc_Absyn_PtrInfo _temp273; struct Cyc_Absyn_Conref* _temp275;
struct Cyc_Absyn_Tqual _temp277; void* _temp279; struct Cyc_Absyn_Exp* _temp281;
struct Cyc_Absyn_Tqual _temp283; void* _temp285; struct Cyc_Absyn_FnInfo
_temp287; struct Cyc_List_List* _temp289; struct Cyc_Absyn_VarargInfo* _temp291;
int _temp293; struct Cyc_List_List* _temp295; void* _temp297; struct Cyc_List_List*
_temp299; struct Cyc_List_List* _temp301; struct Cyc_List_List* _temp303; struct
_tuple0* _temp305; struct _tuple0* _temp307; struct _tuple0* _temp309; struct
Cyc_List_List* _temp311; struct Cyc_Core_Opt* _temp313; struct Cyc_List_List*
_temp315; struct _tuple0* _temp317; void* _temp319; _LL200: if( _temp198 == (
void*) Cyc_Absyn_VoidType){ goto _LL201;} else{ goto _LL202;} _LL202: if((
unsigned int) _temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_Evar: 0){ _LL255:
_temp254=(( struct Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp254 ==  0){
goto _LL203;} else{ goto _LL204;}} else{ goto _LL204;} _LL204: if(( unsigned int)
_temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_Evar: 0){ _LL257: _temp256=((
struct Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp256 ==  0){ goto _LL206;}
else{ _temp258=* _temp256; _LL260: _temp259=( void*) _temp258.v; goto _LL205;}}
else{ goto _LL206;} _LL206: if(( unsigned int) _temp198 >  4u?*(( int*) _temp198)
==  Cyc_Absyn_VarType: 0){ goto _LL207;} else{ goto _LL208;} _LL208: if((
unsigned int) _temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_TunionType: 0){
_LL262: _temp261=(( struct Cyc_Absyn_TunionType_struct*) _temp198)->f1; _LL264:
_temp263=( void*) _temp261.tunion_info; if(*(( int*) _temp263) ==  Cyc_Absyn_KnownTunion){
goto _LL209;} else{ goto _LL210;}} else{ goto _LL210;} _LL210: if(( unsigned int)
_temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_TunionType: 0){ goto _LL211;}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp198 >  4u?*(( int*) _temp198)
==  Cyc_Absyn_TunionFieldType: 0){ _LL266: _temp265=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp198)->f1; _LL268: _temp267=( void*) _temp265.field_info; if(*(( int*)
_temp267) ==  Cyc_Absyn_KnownTunionfield){ _LL272: _temp271=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp267)->f1; goto _LL270; _LL270: _temp269=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp267)->f2; goto _LL213;} else{ goto _LL214;}} else{ goto _LL214;} _LL214:
if(( unsigned int) _temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL215;} else{ goto _LL216;} _LL216: if(( unsigned int) _temp198 >  4u?*((
int*) _temp198) ==  Cyc_Absyn_PointerType: 0){ _LL274: _temp273=(( struct Cyc_Absyn_PointerType_struct*)
_temp198)->f1; _LL280: _temp279=( void*) _temp273.elt_typ; goto _LL278; _LL278:
_temp277= _temp273.tq; goto _LL276; _LL276: _temp275= _temp273.bounds; goto
_LL217;} else{ goto _LL218;} _LL218: if(( unsigned int) _temp198 >  4u?*(( int*)
_temp198) ==  Cyc_Absyn_IntType: 0){ goto _LL219;} else{ goto _LL220;} _LL220:
if( _temp198 == ( void*) Cyc_Absyn_FloatType){ goto _LL221;} else{ goto _LL222;}
_LL222: if( _temp198 == ( void*) Cyc_Absyn_DoubleType){ goto _LL223;} else{ goto
_LL224;} _LL224: if(( unsigned int) _temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_ArrayType:
0){ _LL286: _temp285=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp198)->f1;
goto _LL284; _LL284: _temp283=(( struct Cyc_Absyn_ArrayType_struct*) _temp198)->f2;
goto _LL282; _LL282: _temp281=(( struct Cyc_Absyn_ArrayType_struct*) _temp198)->f3;
goto _LL225;} else{ goto _LL226;} _LL226: if(( unsigned int) _temp198 >  4u?*((
int*) _temp198) ==  Cyc_Absyn_FnType: 0){ _LL288: _temp287=(( struct Cyc_Absyn_FnType_struct*)
_temp198)->f1; _LL298: _temp297=( void*) _temp287.ret_typ; goto _LL296; _LL296:
_temp295= _temp287.args; goto _LL294; _LL294: _temp293= _temp287.c_varargs; goto
_LL292; _LL292: _temp291= _temp287.cyc_varargs; goto _LL290; _LL290: _temp289=
_temp287.attributes; goto _LL227;} else{ goto _LL228;} _LL228: if(( unsigned int)
_temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_TupleType: 0){ _LL300: _temp299=((
struct Cyc_Absyn_TupleType_struct*) _temp198)->f1; goto _LL229;} else{ goto
_LL230;} _LL230: if(( unsigned int) _temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_AnonStructType:
0){ _LL302: _temp301=(( struct Cyc_Absyn_AnonStructType_struct*) _temp198)->f1;
goto _LL231;} else{ goto _LL232;} _LL232: if(( unsigned int) _temp198 >  4u?*((
int*) _temp198) ==  Cyc_Absyn_AnonUnionType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp198)->f1; goto _LL233;} else{ goto _LL234;} _LL234: if(( unsigned int)
_temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_UnionType: 0){ _LL306: _temp305=((
struct Cyc_Absyn_UnionType_struct*) _temp198)->f1; goto _LL235;} else{ goto
_LL236;} _LL236: if(( unsigned int) _temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_StructType:
0){ _LL308: _temp307=(( struct Cyc_Absyn_StructType_struct*) _temp198)->f1; goto
_LL237;} else{ goto _LL238;} _LL238: if(( unsigned int) _temp198 >  4u?*(( int*)
_temp198) ==  Cyc_Absyn_EnumType: 0){ _LL310: _temp309=(( struct Cyc_Absyn_EnumType_struct*)
_temp198)->f1; goto _LL239;} else{ goto _LL240;} _LL240: if(( unsigned int)
_temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_AnonEnumType: 0){ _LL312:
_temp311=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp198)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: if(( unsigned int) _temp198 >  4u?*(( int*) _temp198)
==  Cyc_Absyn_TypedefType: 0){ _LL318: _temp317=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f1; goto _LL316; _LL316: _temp315=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f2; goto _LL314; _LL314: _temp313=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f3; goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_RgnHandleType: 0){ _LL320:
_temp319=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp198)->f1; goto
_LL245;} else{ goto _LL246;} _LL246: if( _temp198 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int) _temp198 >  4u?*((
int*) _temp198) ==  Cyc_Absyn_AccessEff: 0){ goto _LL249;} else{ goto _LL250;}
_LL250: if(( unsigned int) _temp198 >  4u?*(( int*) _temp198) ==  Cyc_Absyn_JoinEff:
0){ goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp198 >  4u?*((
int*) _temp198) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL253;} else{ goto _LL199;}
_LL201: return t; _LL203: return Cyc_Absyn_sint_t; _LL205: return Cyc_Toc_typ_to_c(
_temp259); _LL207: return Cyc_Absyn_void_star_typ(); _LL209: return Cyc_Absyn_void_star_typ();
_LL211:( int) _throw(({ void* _temp321[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp321, sizeof( void*), 0u));}));
_LL213: if( _temp269->typs ==  0){ if( _temp271->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp269->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL215:( int) _throw(({ void*
_temp322[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp322, sizeof( void*), 0u));}));
_LL217: _temp279= Cyc_Toc_typ_to_c_array( _temp279);{ void* _temp323=( void*)(
Cyc_Absyn_compress_conref( _temp275))->v; void* _temp329; _LL325: if((
unsigned int) _temp323 >  1u?*(( int*) _temp323) ==  Cyc_Absyn_Eq_constr: 0){
_LL330: _temp329=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp323)->f1;
if( _temp329 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL326;} else{ goto _LL327;}}
else{ goto _LL327;} _LL327: goto _LL328; _LL326: return Cyc_Toc_tagged_arr_typ;
_LL328: return Cyc_Absyn_star_typ( _temp279,( void*) Cyc_Absyn_HeapRgn, _temp277);
_LL324:;} _LL219: return t; _LL221: return t; _LL223: return t; _LL225: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp331=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp331[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp332; _temp332.tag= Cyc_Absyn_ArrayType; _temp332.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp285); _temp332.f2= _temp283; _temp332.f3=
_temp281; _temp332;}); _temp331;}); _LL227: { struct Cyc_List_List* _temp333= 0;
for( 0; _temp289 !=  0; _temp289=(( struct Cyc_List_List*) _check_null( _temp289))->tl){
void* _temp334=( void*)(( struct Cyc_List_List*) _check_null( _temp289))->hd;
_LL336: if( _temp334 == ( void*) Cyc_Absyn_Noreturn_att){ goto _LL337;} else{
goto _LL338;} _LL338: if( _temp334 == ( void*) Cyc_Absyn_Const_att){ goto _LL339;}
else{ goto _LL340;} _LL340: if(( unsigned int) _temp334 >  16u?*(( int*)
_temp334) ==  Cyc_Absyn_Format_att: 0){ goto _LL341;} else{ goto _LL342;} _LL342:
goto _LL343; _LL337: goto _LL339; _LL339: goto _LL341; _LL341: continue; _LL343:
_temp333=({ struct Cyc_List_List* _temp344=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp344->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp289))->hd); _temp344->tl= _temp333; _temp344;}); goto _LL335;
_LL335:;}{ struct Cyc_List_List* _temp345=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp295); if( _temp291 !=  0){ void* _temp346= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp291))->type,( void*)
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp347=({ struct _tuple1*
_temp349=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp349->f1=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp291))->name; _temp349->f2=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp291))->tq; _temp349->f3=
_temp346; _temp349;}); _temp345=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp345,({ struct Cyc_List_List*
_temp348=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp348->hd=( void*) _temp347; _temp348->tl= 0; _temp348;}));} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp350=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp350[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp351; _temp351.tag= Cyc_Absyn_FnType; _temp351.f1=({ struct Cyc_Absyn_FnInfo
_temp352; _temp352.tvars= 0; _temp352.effect= 0; _temp352.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp297); _temp352.args= _temp345; _temp352.c_varargs= _temp293; _temp352.cyc_varargs=
0; _temp352.rgn_po= 0; _temp352.attributes= _temp333; _temp352;}); _temp351;});
_temp350;});}} _LL229: _temp299=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp299);{ struct _tagged_arr* _temp353= Cyc_Toc_add_tuple_type( _temp299);
return Cyc_Absyn_strct( _temp353);} _LL231: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp354=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp354[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp355; _temp355.tag= Cyc_Absyn_AnonStructType; _temp355.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp301); _temp355;}); _temp354;});
_LL233: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp356=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp356[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp357; _temp357.tag=
Cyc_Absyn_AnonUnionType; _temp357.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp303); _temp357;}); _temp356;}); _LL235: if( _temp305 ==  0){({ void*
_temp358[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp358,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp305)); _LL237: if( _temp307 ==  0){({ void* _temp359[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp359, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp307)); _LL239:
return t; _LL241: Cyc_Toc_enumfields_to_c( _temp311); return t; _LL243: if(
_temp313 ==  0){ return({ void* _temp360[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u), _tag_arr( _temp360, sizeof( void*), 0u));});}
return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp361=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp361[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp362; _temp362.tag= Cyc_Absyn_TypedefType;
_temp362.f1= _temp317; _temp362.f2= 0; _temp362.f3=({ struct Cyc_Core_Opt*
_temp363=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp363->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp313))->v); _temp363;}); _temp362;}); _temp361;}); _LL245:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL247: return({ void* _temp364[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp364, sizeof( void*), 0u));});
_LL249: return({ void* _temp365[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp365, sizeof( void*), 0u));});
_LL251: return({ void* _temp366[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp366, sizeof( void*), 0u));});
_LL253: return({ void* _temp367[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp367, sizeof( void*), 0u));});
_LL199:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp368= t;
struct Cyc_Absyn_Tqual _temp374; void* _temp376; _LL370: if(( unsigned int)
_temp368 >  4u?*(( int*) _temp368) ==  Cyc_Absyn_ArrayType: 0){ _LL377: _temp376=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp368)->f1; goto _LL375; _LL375:
_temp374=(( struct Cyc_Absyn_ArrayType_struct*) _temp368)->f2; goto _LL371;}
else{ goto _LL372;} _LL372: goto _LL373; _LL371: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp376,( void*) Cyc_Absyn_HeapRgn, _temp374), e, l); _LL373: return Cyc_Absyn_cast_exp(
t, e, l); _LL369:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp378= Cyc_Tcutil_compress(
t); void* _temp418; struct Cyc_Absyn_Structdecl** _temp420; struct Cyc_List_List*
_temp422; struct Cyc_List_List* _temp424; struct Cyc_Absyn_TunionFieldInfo
_temp426; void* _temp428; struct Cyc_Absyn_Tunionfield* _temp430; struct Cyc_Absyn_Tuniondecl*
_temp432; struct Cyc_List_List* _temp434; _LL380: if( _temp378 == ( void*) Cyc_Absyn_VoidType){
goto _LL381;} else{ goto _LL382;} _LL382: if(( unsigned int) _temp378 >  4u?*((
int*) _temp378) ==  Cyc_Absyn_VarType: 0){ goto _LL383;} else{ goto _LL384;}
_LL384: if(( unsigned int) _temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_IntType:
0){ goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp378 >  4u?*((
int*) _temp378) ==  Cyc_Absyn_EnumType: 0){ goto _LL387;} else{ goto _LL388;}
_LL388: if(( unsigned int) _temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL389;} else{ goto _LL390;} _LL390: if( _temp378 == ( void*) Cyc_Absyn_FloatType){
goto _LL391;} else{ goto _LL392;} _LL392: if( _temp378 == ( void*) Cyc_Absyn_DoubleType){
goto _LL393;} else{ goto _LL394;} _LL394: if(( unsigned int) _temp378 >  4u?*((
int*) _temp378) ==  Cyc_Absyn_FnType: 0){ goto _LL395;} else{ goto _LL396;}
_LL396: if(( unsigned int) _temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_ArrayType:
0){ _LL419: _temp418=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp378)->f1;
goto _LL397;} else{ goto _LL398;} _LL398: if(( unsigned int) _temp378 >  4u?*((
int*) _temp378) ==  Cyc_Absyn_StructType: 0){ _LL421: _temp420=(( struct Cyc_Absyn_StructType_struct*)
_temp378)->f3; goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int)
_temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_AnonStructType: 0){ _LL423:
_temp422=(( struct Cyc_Absyn_AnonStructType_struct*) _temp378)->f1; goto _LL401;}
else{ goto _LL402;} _LL402: if(( unsigned int) _temp378 >  4u?*(( int*) _temp378)
==  Cyc_Absyn_AnonUnionType: 0){ _LL425: _temp424=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp378)->f1; goto _LL403;} else{ goto _LL404;} _LL404: if(( unsigned int)
_temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_TunionFieldType: 0){ _LL427:
_temp426=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp378)->f1; _LL429:
_temp428=( void*) _temp426.field_info; if(*(( int*) _temp428) ==  Cyc_Absyn_KnownTunionfield){
_LL433: _temp432=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp428)->f1;
goto _LL431; _LL431: _temp430=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp428)->f2; goto _LL405;} else{ goto _LL406;}} else{ goto _LL406;} _LL406:
if(( unsigned int) _temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_TupleType: 0){
_LL435: _temp434=(( struct Cyc_Absyn_TupleType_struct*) _temp378)->f1; goto
_LL407;} else{ goto _LL408;} _LL408: if(( unsigned int) _temp378 >  4u?*(( int*)
_temp378) ==  Cyc_Absyn_TunionType: 0){ goto _LL409;} else{ goto _LL410;} _LL410:
if(( unsigned int) _temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_UnionType: 0){
goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int) _temp378 >  4u?*((
int*) _temp378) ==  Cyc_Absyn_PointerType: 0){ goto _LL413;} else{ goto _LL414;}
_LL414: if(( unsigned int) _temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL415;} else{ goto _LL416;} _LL416: goto _LL417; _LL381: return 1;
_LL383: return 0; _LL385: return 1; _LL387: return 1; _LL389: return 1; _LL391:
return 1; _LL393: return 1; _LL395: return 1; _LL397: return Cyc_Toc_atomic_typ(
_temp418); _LL399: if( _temp420 ==  0? 1:(*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp420)))->fields ==  0){ return 0;}{ struct Cyc_List_List*
_temp436=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp420)))->fields))->v; for( 0;
_temp436 !=  0; _temp436=(( struct Cyc_List_List*) _check_null( _temp436))->tl){
if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp436))->hd)->type)){ return 0;}}} return 1; _LL401: _temp424=
_temp422; goto _LL403; _LL403: for( 0; _temp424 !=  0; _temp424=(( struct Cyc_List_List*)
_check_null( _temp424))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp424))->hd)->type)){ return 0;}} return
1; _LL405: _temp434= _temp430->typs; goto _LL407; _LL407: for( 0; _temp434 !=  0;
_temp434=(( struct Cyc_List_List*) _check_null( _temp434))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp434))->hd)).f2)){
return 0;}} return 1; _LL409: return 0; _LL411: return 0; _LL413: return 0;
_LL415: return 0; _LL417:({ struct Cyc_Std_String_pa_struct _temp438; _temp438.tag=
Cyc_Std_String_pa; _temp438.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp437[ 1u]={& _temp438};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp437, sizeof( void*), 1u));}});
return 0; _LL379:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp439=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp445; void* _temp447;
_LL441: if(( unsigned int) _temp439 >  4u?*(( int*) _temp439) ==  Cyc_Absyn_PointerType:
0){ _LL446: _temp445=(( struct Cyc_Absyn_PointerType_struct*) _temp439)->f1;
_LL448: _temp447=( void*) _temp445.elt_typ; goto _LL442;} else{ goto _LL443;}
_LL443: goto _LL444; _LL442: { void* _temp449= Cyc_Tcutil_compress( _temp447);
_LL451: if( _temp449 == ( void*) Cyc_Absyn_VoidType){ goto _LL452;} else{ goto
_LL453;} _LL453: goto _LL454; _LL452: return 1; _LL454: return 0; _LL450:;}
_LL444: return 0; _LL440:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp455= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp467; struct Cyc_Absyn_Structdecl* _temp469; struct Cyc_List_List* _temp470;
_LL457: if(( unsigned int) _temp455 >  4u?*(( int*) _temp455) ==  Cyc_Absyn_StructType:
0){ _LL468: _temp467=(( struct Cyc_Absyn_StructType_struct*) _temp455)->f3; if(
_temp467 ==  0){ goto _LL459;} else{ _temp469=* _temp467; goto _LL458;}} else{
goto _LL459;} _LL459: if(( unsigned int) _temp455 >  4u?*(( int*) _temp455) == 
Cyc_Absyn_AnonStructType: 0){ _LL471: _temp470=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp455)->f1; goto _LL460;} else{ goto _LL461;} _LL461: if(( unsigned int)
_temp455 >  4u?*(( int*) _temp455) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL462;}
else{ goto _LL463;} _LL463: if(( unsigned int) _temp455 >  4u?*(( int*) _temp455)
==  Cyc_Absyn_UnionType: 0){ goto _LL464;} else{ goto _LL465;} _LL465: goto
_LL466; _LL458: if( _temp469->fields ==  0){( int) _throw(({ void* _temp472[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp472, sizeof( void*), 0u));}));} _temp470=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp469->fields))->v;
goto _LL460; _LL460: { struct Cyc_Absyn_Structfield* _temp473= Cyc_Absyn_lookup_field(
_temp470, f); if( _temp473 ==  0){({ struct Cyc_Std_String_pa_struct _temp475;
_temp475.tag= Cyc_Std_String_pa; _temp475.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp473))->name;{ void* _temp474[ 1u]={& _temp475};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_field: bad field %s",
sizeof( unsigned char), 28u), _tag_arr( _temp474, sizeof( void*), 1u));}});}
return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp473))->type);} _LL462: return 0; _LL464: return 0; _LL466:(
int) _throw(({ struct Cyc_Std_String_pa_struct _temp477; _temp477.tag= Cyc_Std_String_pa;
_temp477.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp476[ 1u]={&
_temp477}; Cyc_Toc_toc_impos( _tag_arr("is_poly_field: bad type %s", sizeof(
unsigned char), 27u), _tag_arr( _temp476, sizeof( void*), 1u));}})); _LL456:;}
static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp478=(
void*) e->r; struct _tagged_arr* _temp486; struct Cyc_Absyn_Exp* _temp488;
struct _tagged_arr* _temp490; struct Cyc_Absyn_Exp* _temp492; _LL480: if(*(( int*)
_temp478) ==  Cyc_Absyn_StructMember_e){ _LL489: _temp488=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp478)->f1; goto _LL487; _LL487: _temp486=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp478)->f2; goto _LL481;} else{ goto _LL482;} _LL482: if(*(( int*) _temp478)
==  Cyc_Absyn_StructArrow_e){ _LL493: _temp492=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp478)->f1; goto _LL491; _LL491: _temp490=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp478)->f2; goto _LL483;} else{ goto _LL484;} _LL484: goto _LL485; _LL481:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp488->topt))->v, _temp486); _LL483: { void* _temp494= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp492->topt))->v); struct Cyc_Absyn_PtrInfo
_temp500; void* _temp502; _LL496: if(( unsigned int) _temp494 >  4u?*(( int*)
_temp494) ==  Cyc_Absyn_PointerType: 0){ _LL501: _temp500=(( struct Cyc_Absyn_PointerType_struct*)
_temp494)->f1; _LL503: _temp502=( void*) _temp500.elt_typ; goto _LL497;} else{
goto _LL498;} _LL498: goto _LL499; _LL497: return Cyc_Toc_is_poly_field(
_temp502, _temp490); _LL499:({ struct Cyc_Std_String_pa_struct _temp505;
_temp505.tag= Cyc_Std_String_pa; _temp505.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp492->topt))->v);{ void*
_temp504[ 1u]={& _temp505};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s", sizeof(
unsigned char), 30u), _tag_arr( _temp504, sizeof( void*), 1u));}}); return 0;
_LL495:;} _LL485: return 0; _LL479:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_e,({
struct Cyc_List_List* _temp506=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp506->hd=( void*) s; _temp506->tl= 0; _temp506;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_atomic_e,({ struct Cyc_List_List*
_temp507=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp507->hd=( void*) s; _temp507->tl= 0; _temp507;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp508[ 2u]; _temp508[ 1u]= s; _temp508[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp508, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp509=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp509->hd=( void*) e; _temp509->tl= 0; _temp509;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp510=( void*) e->r; void* _temp516; _LL512: if(*((
int*) _temp510) ==  Cyc_Absyn_Const_e){ _LL517: _temp516=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp510)->f1; if(( unsigned int) _temp516 >  1u?*(( int*) _temp516) ==  Cyc_Absyn_String_c:
0){ goto _LL513;} else{ goto _LL514;}} else{ goto _LL514;} _LL514: goto _LL515;
_LL513: is_string= 1; goto _LL511; _LL515: goto _LL511; _LL511:;}{ struct Cyc_Absyn_Exp*
xexp; struct Cyc_Absyn_Exp* xplussz; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp521=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp521[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp522; _temp522.tag= Cyc_Absyn_ArrayType; _temp522.f1=(
void*) Cyc_Absyn_uchar_t; _temp522.f2= Cyc_Toc_mt_tq; _temp522.f3=( struct Cyc_Absyn_Exp*)
sz; _temp522;}); _temp521;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp518=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp518->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp519=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp519[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp520; _temp520.tag= Cyc_Absyn_Var_d;
_temp520.f1= vd; _temp520;}); _temp519;}), 0); _temp518->tl= Cyc_Toc_result_decls;
_temp518;}); xexp= Cyc_Absyn_var_exp( x, 0); xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0);} else{ xexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), e, 0);
xplussz= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_add_exp( e, sz,
0), 0);}{ struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp523[ 3u]; _temp523[ 2u]=({ struct _tuple4* _temp526=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp526->f1= 0; _temp526->f2=
xplussz; _temp526;}); _temp523[ 1u]=({ struct _tuple4* _temp525=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp525->f1= 0; _temp525->f2= xexp;
_temp525;}); _temp523[ 0u]=({ struct _tuple4* _temp524=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp524->f1= 0; _temp524->f2= xexp;
_temp524;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp523, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp527=( struct Cyc_Toc_Env*) _cycalloc( sizeof( struct
Cyc_Toc_Env)); _temp527->break_lab= 0; _temp527->continue_lab= 0; _temp527->fallthru_info=
0; _temp527->varmap=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp527->toplevel= 1;
_temp527;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp528=( struct Cyc_Toc_Env*) _cycalloc( sizeof(
struct Cyc_Toc_Env)); _temp528->break_lab= e->break_lab; _temp528->continue_lab=
e->continue_lab; _temp528->fallthru_info= e->fallthru_info; _temp528->varmap= e->varmap;
_temp528->toplevel= e->toplevel; _temp528;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp529=(* x).f1; _LL531:
if(( unsigned int) _temp529 >  1u?*(( int*) _temp529) ==  Cyc_Absyn_Rel_n: 0){
goto _LL532;} else{ goto _LL533;} _LL533: goto _LL534; _LL532:( int) _throw(({
struct Cyc_Std_String_pa_struct _temp536; _temp536.tag= Cyc_Std_String_pa;
_temp536.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp535[
1u]={& _temp536}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp535, sizeof( void*), 1u));}}));
_LL534: goto _LL530; _LL530:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
k, struct Cyc_Absyn_Exp* v)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict*
f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp537= 0; for( 0; fallthru_binders !=  0; fallthru_binders=(( struct Cyc_List_List*)
_check_null( fallthru_binders))->tl){ _temp537=({ struct Cyc_List_List* _temp538=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp538->hd=(
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
fallthru_binders))->hd)->name; _temp538->tl= _temp537; _temp538;});} _temp537=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp537);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp539=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp539->v=( void*) break_l; _temp539;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp540=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp540->v=( void*)({ struct _tuple6* _temp541=( struct _tuple6*) _cycalloc(
sizeof( struct _tuple6)); _temp541->f1= fallthru_l; _temp541->f2= _temp537;
_temp541->f3= next_case_env->varmap; _temp541;}); _temp540;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp542=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp542->v=( void*) break_l; _temp542;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp543=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp543->v=( void*)({
struct _tuple6* _temp544=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp544->f1= next_l; _temp544->f2= 0; _temp544->f3=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp544;}); _temp543;}); return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt ==  0){({ void* _temp545[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp545, sizeof( void*), 0u));});}
return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp546[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp546, sizeof( void*), 0u));});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp547=( struct _tuple3*) _cycalloc( sizeof(
struct _tuple3)); _temp547->f1= Cyc_Toc_mt_tq; _temp547->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp547;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp548=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp548->f1= 0; _temp548->f2= e; _temp548;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp549= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp549): Cyc_Toc_malloc_ptr( _temp549), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp549), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp550=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp550 !=  0; _temp550=((
struct Cyc_List_List*) _check_null( _temp550))->tl){ struct _tuple4 _temp553;
struct Cyc_Absyn_Exp* _temp554; struct Cyc_List_List* _temp556; struct _tuple4*
_temp551=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp550))->hd;
_temp553=* _temp551; _LL557: _temp556= _temp553.f1; goto _LL555; _LL555:
_temp554= _temp553.f2; goto _LL552; _LL552: { struct Cyc_Absyn_Exp* e_index; if(
_temp556 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp556))->tl !=  0){({ void* _temp558[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp558, sizeof( void*), 0u));});}{
void* _temp559=( void*)(( struct Cyc_List_List*) _check_null( _temp556))->hd;
void* _temp560= _temp559; struct Cyc_Absyn_Exp* _temp566; _LL562: if(*(( int*)
_temp560) ==  Cyc_Absyn_ArrayElement){ _LL567: _temp566=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp560)->f1; goto _LL563;} else{ goto _LL564;} _LL564: if(*(( int*) _temp560)
==  Cyc_Absyn_FieldName){ goto _LL565;} else{ goto _LL561;} _LL563: Cyc_Toc_exp_to_c(
nv, _temp566); e_index= _temp566; goto _LL561; _LL565: e_index=({ void* _temp568[
0u]={};(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_unimp)( _tag_arr("field name designators in array", sizeof(
unsigned char), 32u), _tag_arr( _temp568, sizeof( void*), 0u));}); goto _LL561;
_LL561:;}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0);
void* _temp569=( void*) _temp554->r; struct Cyc_List_List* _temp579; struct Cyc_Absyn_Exp*
_temp581; struct Cyc_Absyn_Exp* _temp583; struct Cyc_Absyn_Vardecl* _temp585;
struct Cyc_List_List* _temp587; void* _temp589; _LL571: if(*(( int*) _temp569)
==  Cyc_Absyn_Array_e){ _LL580: _temp579=(( struct Cyc_Absyn_Array_e_struct*)
_temp569)->f1; goto _LL572;} else{ goto _LL573;} _LL573: if(*(( int*) _temp569)
==  Cyc_Absyn_Comprehension_e){ _LL586: _temp585=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp569)->f1; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp569)->f2; goto _LL582; _LL582: _temp581=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp569)->f3; goto _LL574;} else{ goto _LL575;} _LL575: if(*(( int*) _temp569)
==  Cyc_Absyn_AnonStruct_e){ _LL590: _temp589=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp569)->f1; goto _LL588; _LL588: _temp587=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp569)->f2; goto _LL576;} else{ goto _LL577;} _LL577: goto _LL578; _LL572: s=
Cyc_Toc_init_array( nv, lval, _temp579, s); goto _LL570; _LL574: s= Cyc_Toc_init_comprehension(
nv, lval, _temp585, _temp583, _temp581, s, 0); goto _LL570; _LL576: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp589, _temp587, s); goto _LL570; _LL578: Cyc_Toc_exp_to_c( nv,
_temp554); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp554, 0), s, 0); goto _LL570; _LL570:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp591= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp591, Cyc_Absyn_varb_exp( _temp591,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp614=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp614[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp615; _temp615.tag= Cyc_Absyn_Local_b; _temp615.f1= vd; _temp615;});
_temp614;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp591, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp591, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp591, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp591, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp592=( void*) e2->r; struct Cyc_List_List* _temp602; struct Cyc_Absyn_Exp*
_temp604; struct Cyc_Absyn_Exp* _temp606; struct Cyc_Absyn_Vardecl* _temp608;
struct Cyc_List_List* _temp610; void* _temp612; _LL594: if(*(( int*) _temp592)
==  Cyc_Absyn_Array_e){ _LL603: _temp602=(( struct Cyc_Absyn_Array_e_struct*)
_temp592)->f1; goto _LL595;} else{ goto _LL596;} _LL596: if(*(( int*) _temp592)
==  Cyc_Absyn_Comprehension_e){ _LL609: _temp608=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp592)->f1; goto _LL607; _LL607: _temp606=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp592)->f2; goto _LL605; _LL605: _temp604=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp592)->f3; goto _LL597;} else{ goto _LL598;} _LL598: if(*(( int*) _temp592)
==  Cyc_Absyn_AnonStruct_e){ _LL613: _temp612=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp592)->f1; goto _LL611; _LL611: _temp610=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp592)->f2; goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL595:
body= Cyc_Toc_init_array( nv2, lval, _temp602, Cyc_Absyn_skip_stmt( 0)); goto
_LL593; _LL597: body= Cyc_Toc_init_comprehension( nv2, lval, _temp608, _temp606,
_temp604, Cyc_Absyn_skip_stmt( 0), 0); goto _LL593; _LL599: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp612, _temp610, Cyc_Absyn_skip_stmt( 0)); goto _LL593; _LL601: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL593; _LL593:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp591, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp616=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp616 !=  0; _temp616=(( struct Cyc_List_List*)
_check_null( _temp616))->tl){ struct _tuple4 _temp619; struct Cyc_Absyn_Exp*
_temp620; struct Cyc_List_List* _temp622; struct _tuple4* _temp617=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp616))->hd; _temp619=*
_temp617; _LL623: _temp622= _temp619.f1; goto _LL621; _LL621: _temp620= _temp619.f2;
goto _LL618; _LL618: if( _temp622 ==  0){({ void* _temp624[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp624, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp622))->tl !=  0){({ void* _temp625[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct", sizeof( unsigned char), 41u),
_tag_arr( _temp625, sizeof( void*), 0u));});}{ void* _temp626=( void*)(( struct
Cyc_List_List*) _check_null( _temp622))->hd; struct _tagged_arr* _temp632;
_LL628: if(*(( int*) _temp626) ==  Cyc_Absyn_FieldName){ _LL633: _temp632=((
struct Cyc_Absyn_FieldName_struct*) _temp626)->f1; goto _LL629;} else{ goto
_LL630;} _LL630: goto _LL631; _LL629: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp(
lhs, _temp632, 0);{ void* _temp634=( void*) _temp620->r; struct Cyc_List_List*
_temp644; struct Cyc_Absyn_Exp* _temp646; struct Cyc_Absyn_Exp* _temp648; struct
Cyc_Absyn_Vardecl* _temp650; struct Cyc_List_List* _temp652; void* _temp654;
_LL636: if(*(( int*) _temp634) ==  Cyc_Absyn_Array_e){ _LL645: _temp644=((
struct Cyc_Absyn_Array_e_struct*) _temp634)->f1; goto _LL637;} else{ goto _LL638;}
_LL638: if(*(( int*) _temp634) ==  Cyc_Absyn_Comprehension_e){ _LL651: _temp650=((
struct Cyc_Absyn_Comprehension_e_struct*) _temp634)->f1; goto _LL649; _LL649:
_temp648=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp634)->f2; goto _LL647;
_LL647: _temp646=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp634)->f3;
goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp634) ==  Cyc_Absyn_AnonStruct_e){
_LL655: _temp654=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp634)->f1;
goto _LL653; _LL653: _temp652=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp634)->f2;
goto _LL641;} else{ goto _LL642;} _LL642: goto _LL643; _LL637: s= Cyc_Toc_init_array(
nv, lval, _temp644, s); goto _LL635; _LL639: s= Cyc_Toc_init_comprehension( nv,
lval, _temp650, _temp648, _temp646, s, 0); goto _LL635; _LL641: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp654, _temp652, s); goto _LL635; _LL643: Cyc_Toc_exp_to_c( nv,
_temp620); if( Cyc_Toc_is_poly_field( struct_type, _temp632)){ _temp620= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp620, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp620, 0), 0), s, 0); goto _LL635; _LL635:;} goto
_LL627;} _LL631:( int) _throw(({ void* _temp656[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp656, sizeof( void*), 0u));})); _LL627:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp657= _new_region();
struct _RegionHandle* r=& _temp657; _push_region( r);{ struct Cyc_List_List*
_temp658=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp659= Cyc_Toc_add_tuple_type( _temp658); struct
_tuple0* _temp660= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp661= Cyc_Absyn_var_exp(
_temp660, 0); struct Cyc_Absyn_Stmt* _temp662= Cyc_Absyn_exp_stmt( _temp661, 0);
struct Cyc_Absyn_Exp*(* _temp663)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp664=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp664); for( 0; _temp664
!=  0;( _temp664=(( struct Cyc_List_List*) _check_null( _temp664))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp664))->hd; struct Cyc_Absyn_Exp* lval= _temp663( _temp661, Cyc_Absyn_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp665=( void*) e->r; struct Cyc_List_List*
_temp673; struct Cyc_Absyn_Exp* _temp675; struct Cyc_Absyn_Exp* _temp677; struct
Cyc_Absyn_Vardecl* _temp679; _LL667: if(*(( int*) _temp665) ==  Cyc_Absyn_Array_e){
_LL674: _temp673=(( struct Cyc_Absyn_Array_e_struct*) _temp665)->f1; goto _LL668;}
else{ goto _LL669;} _LL669: if(*(( int*) _temp665) ==  Cyc_Absyn_Comprehension_e){
_LL680: _temp679=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp665)->f1;
goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp665)->f2; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp665)->f3; goto _LL670;} else{ goto _LL671;} _LL671: goto _LL672; _LL668:
_temp662= Cyc_Toc_init_array( nv, lval, _temp673, _temp662); goto _LL666; _LL670:
_temp662= Cyc_Toc_init_comprehension( nv, lval, _temp679, _temp677, _temp675,
_temp662, 0); goto _LL666; _LL672: Cyc_Toc_exp_to_c( nv, e); _temp662= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp663( _temp661, Cyc_Absyn_fieldname(
i), 0), e, 0), 0), _temp662, 0); goto _LL666; _LL666:;}}}{ struct Cyc_Absyn_Exp*
_temp681= Cyc_Toc_make_struct( nv, _temp660, Cyc_Absyn_strct( _temp659),
_temp662, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp681;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp682= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp683= Cyc_Absyn_var_exp( _temp682, 0); struct Cyc_Absyn_Stmt*
_temp684= Cyc_Absyn_exp_stmt( _temp683, 0); struct Cyc_Absyn_Exp*(* _temp685)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp686= _new_region(); struct _RegionHandle* r=& _temp686;
_push_region( r);{ struct Cyc_List_List* _temp687=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp687 !=  0; _temp687=(( struct Cyc_List_List*) _check_null( _temp687))->tl){
struct _tuple4 _temp690; struct Cyc_Absyn_Exp* _temp691; struct Cyc_List_List*
_temp693; struct _tuple4* _temp688=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp687))->hd; _temp690=* _temp688; _LL694: _temp693= _temp690.f1;
goto _LL692; _LL692: _temp691= _temp690.f2; goto _LL689; _LL689: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp691->topt))->v): 0; if( _temp693 ==  0){({ void* _temp695[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp695, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp693))->tl !=  0){ struct _tuple0*
_temp696= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp697= Cyc_Absyn_var_exp(
_temp696, 0); for( 0; _temp693 !=  0; _temp693=(( struct Cyc_List_List*)
_check_null( _temp693))->tl){ void* _temp698=( void*)(( struct Cyc_List_List*)
_check_null( _temp693))->hd; struct _tagged_arr* _temp704; _LL700: if(*(( int*)
_temp698) ==  Cyc_Absyn_FieldName){ _LL705: _temp704=(( struct Cyc_Absyn_FieldName_struct*)
_temp698)->f1; goto _LL701;} else{ goto _LL702;} _LL702: goto _LL703; _LL701:
if( Cyc_Toc_is_poly_field( struct_type, _temp704)){ _temp697= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp697, 0);} _temp684= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp685( _temp683, _temp704, 0), _temp697, 0), 0),
_temp684, 0); goto _LL699; _LL703:( int) _throw(({ void* _temp706[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp706, sizeof( void*), 0u));})); _LL699:;} Cyc_Toc_exp_to_c( nv, _temp691);
_temp684= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp697,
_temp691, 0), 0), _temp684, 0);} else{ void* _temp707=( void*)(( struct Cyc_List_List*)
_check_null( _temp693))->hd; struct _tagged_arr* _temp713; _LL709: if(*(( int*)
_temp707) ==  Cyc_Absyn_FieldName){ _LL714: _temp713=(( struct Cyc_Absyn_FieldName_struct*)
_temp707)->f1; goto _LL710;} else{ goto _LL711;} _LL711: goto _LL712; _LL710: {
struct Cyc_Absyn_Exp* lval= _temp685( _temp683, _temp713, 0);{ void* _temp715=(
void*) _temp691->r; struct Cyc_List_List* _temp725; struct Cyc_Absyn_Exp*
_temp727; struct Cyc_Absyn_Exp* _temp729; struct Cyc_Absyn_Vardecl* _temp731;
struct Cyc_List_List* _temp733; void* _temp735; _LL717: if(*(( int*) _temp715)
==  Cyc_Absyn_Array_e){ _LL726: _temp725=(( struct Cyc_Absyn_Array_e_struct*)
_temp715)->f1; goto _LL718;} else{ goto _LL719;} _LL719: if(*(( int*) _temp715)
==  Cyc_Absyn_Comprehension_e){ _LL732: _temp731=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp715)->f1; goto _LL730; _LL730: _temp729=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp715)->f2; goto _LL728; _LL728: _temp727=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp715)->f3; goto _LL720;} else{ goto _LL721;} _LL721: if(*(( int*) _temp715)
==  Cyc_Absyn_AnonStruct_e){ _LL736: _temp735=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp715)->f1; goto _LL734; _LL734: _temp733=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp715)->f2; goto _LL722;} else{ goto _LL723;} _LL723: goto _LL724; _LL718:
_temp684= Cyc_Toc_init_array( nv, lval, _temp725, _temp684); goto _LL716; _LL720:
_temp684= Cyc_Toc_init_comprehension( nv, lval, _temp731, _temp729, _temp727,
_temp684, 0); goto _LL716; _LL722: _temp684= Cyc_Toc_init_anon_struct( nv, lval,
_temp735, _temp733, _temp684); goto _LL716; _LL724: Cyc_Toc_exp_to_c( nv,
_temp691); if( Cyc_Toc_is_poly_field( struct_type, _temp713)){ _temp691= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp691, 0);} _temp684= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp691, 0), 0), _temp684, 0); goto _LL716; _LL716:;}
goto _LL708;} _LL712:( int) _throw(({ void* _temp737[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp737, sizeof( void*), 0u));})); _LL708:;}}}{ struct Cyc_Absyn_Exp* _temp738=
Cyc_Toc_make_struct( nv, _temp682, Cyc_Absyn_strctq( tdn), _temp684, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp738;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp739=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp739[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp740; _temp740.tag= Cyc_Absyn_Increment_e;
_temp740.f1= e1; _temp740.f2=( void*) incr; _temp740;}); _temp739;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp741=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp751; struct Cyc_Absyn_Exp*
_temp753; void* _temp755; struct _tagged_arr* _temp757; struct Cyc_Absyn_Exp*
_temp759; _LL743: if(*(( int*) _temp741) ==  Cyc_Absyn_StmtExp_e){ _LL752:
_temp751=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp741)->f1; goto _LL744;}
else{ goto _LL745;} _LL745: if(*(( int*) _temp741) ==  Cyc_Absyn_Cast_e){ _LL756:
_temp755=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp741)->f1; goto _LL754;
_LL754: _temp753=(( struct Cyc_Absyn_Cast_e_struct*) _temp741)->f2; goto _LL746;}
else{ goto _LL747;} _LL747: if(*(( int*) _temp741) ==  Cyc_Absyn_StructMember_e){
_LL760: _temp759=(( struct Cyc_Absyn_StructMember_e_struct*) _temp741)->f1; goto
_LL758; _LL758: _temp757=(( struct Cyc_Absyn_StructMember_e_struct*) _temp741)->f2;
goto _LL748;} else{ goto _LL749;} _LL749: goto _LL750; _LL744: Cyc_Toc_lvalue_assign_stmt(
_temp751, fs, f, f_env); goto _LL742; _LL746: Cyc_Toc_lvalue_assign( _temp753,
fs, f, f_env); goto _LL742; _LL748:( void*)( e1->r=( void*)(( void*) _temp759->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp761=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp761->hd=( void*) _temp757; _temp761->tl= fs; _temp761;}), f, f_env); goto
_LL742; _LL750: if( ! Cyc_Absyn_is_lvalue( e1)){({ struct Cyc_Std_String_pa_struct
_temp763; _temp763.tag= Cyc_Std_String_pa; _temp763.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp762[ 1u]={& _temp763};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u), _tag_arr( _temp762, sizeof( void*), 1u));}});}{
struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs !=  0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd,
e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto
_LL742;} _LL742:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp764=( void*) s->r; struct Cyc_Absyn_Exp*
_temp774; struct Cyc_Absyn_Stmt* _temp776; struct Cyc_Absyn_Decl* _temp778;
struct Cyc_Absyn_Stmt* _temp780; _LL766: if(( unsigned int) _temp764 >  1u?*((
int*) _temp764) ==  Cyc_Absyn_Exp_s: 0){ _LL775: _temp774=(( struct Cyc_Absyn_Exp_s_struct*)
_temp764)->f1; goto _LL767;} else{ goto _LL768;} _LL768: if(( unsigned int)
_temp764 >  1u?*(( int*) _temp764) ==  Cyc_Absyn_Decl_s: 0){ _LL779: _temp778=((
struct Cyc_Absyn_Decl_s_struct*) _temp764)->f1; goto _LL777; _LL777: _temp776=((
struct Cyc_Absyn_Decl_s_struct*) _temp764)->f2; goto _LL769;} else{ goto _LL770;}
_LL770: if(( unsigned int) _temp764 >  1u?*(( int*) _temp764) ==  Cyc_Absyn_Seq_s:
0){ _LL781: _temp780=(( struct Cyc_Absyn_Seq_s_struct*) _temp764)->f2; goto
_LL771;} else{ goto _LL772;} _LL772: goto _LL773; _LL767: Cyc_Toc_lvalue_assign(
_temp774, fs, f, f_env); goto _LL765; _LL769: Cyc_Toc_lvalue_assign_stmt(
_temp776, fs, f, f_env); goto _LL765; _LL771: Cyc_Toc_lvalue_assign_stmt(
_temp780, fs, f, f_env); goto _LL765; _LL773:({ struct Cyc_Std_String_pa_struct
_temp783; _temp783.tag= Cyc_Std_String_pa; _temp783.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp782[ 1u]={& _temp783};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s", sizeof(
unsigned char), 23u), _tag_arr( _temp782, sizeof( void*), 1u));}}); goto _LL765;
_LL765:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2,
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x ==  0){ return 0;} result=({ struct
Cyc_List_List* _temp784=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp784->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp784->tl= 0; _temp784;}); prev= result; for( x=((
struct Cyc_List_List*) _check_null( x))->tl; x !=  0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp785=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp785->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp785->tl= 0; _temp785;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp786=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp786->f1= 0; _temp786->f2= e; _temp786;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp787=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp793; _LL789: if((
unsigned int) _temp787 >  4u?*(( int*) _temp787) ==  Cyc_Absyn_PointerType: 0){
_LL794: _temp793=(( struct Cyc_Absyn_PointerType_struct*) _temp787)->f1; goto
_LL790;} else{ goto _LL791;} _LL791: goto _LL792; _LL790: return _temp793;
_LL792:( int) _throw(({ void* _temp795[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("get_ptr_typ: not a pointer!",
sizeof( unsigned char), 28u), _tag_arr( _temp795, sizeof( void*), 0u));}));
_LL788:;} struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp796=( void*) e->r; if( e->topt ==  0){({
struct Cyc_Std_String_pa_struct _temp798; _temp798.tag= Cyc_Std_String_pa;
_temp798.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp797[ 1u]={&
_temp798};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u), _tag_arr(
_temp797, sizeof( void*), 1u));}}); return;}{ void* old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp799= _temp796; void*
_temp885; void* _temp887; struct _tuple0* _temp889; struct _tuple0* _temp891;
struct Cyc_List_List* _temp893; void* _temp895; void* _temp897; struct Cyc_Absyn_Exp*
_temp899; struct Cyc_Absyn_Exp* _temp901; struct Cyc_Core_Opt* _temp903; struct
Cyc_Absyn_Exp* _temp905; struct Cyc_Absyn_Exp* _temp907; struct Cyc_Absyn_Exp*
_temp909; struct Cyc_Absyn_Exp* _temp911; struct Cyc_Absyn_Exp* _temp913; struct
Cyc_Absyn_Exp* _temp915; struct Cyc_List_List* _temp917; struct Cyc_Absyn_Exp*
_temp919; struct Cyc_Absyn_VarargCallInfo* _temp921; struct Cyc_List_List*
_temp923; struct Cyc_Absyn_Exp* _temp925; struct Cyc_Absyn_VarargCallInfo*
_temp927; struct Cyc_Absyn_VarargCallInfo _temp929; struct Cyc_Absyn_VarargInfo*
_temp930; struct Cyc_List_List* _temp932; int _temp934; struct Cyc_List_List*
_temp936; struct Cyc_Absyn_Exp* _temp938; struct Cyc_Absyn_Exp* _temp940; struct
Cyc_Absyn_Exp* _temp942; struct Cyc_List_List* _temp944; struct Cyc_Absyn_Exp*
_temp946; struct Cyc_Absyn_Exp* _temp948; void* _temp950; void** _temp952;
struct Cyc_Absyn_Exp* _temp953; struct Cyc_Absyn_Exp* _temp955; struct Cyc_Absyn_Exp*
_temp957; struct Cyc_Absyn_Exp* _temp959; void* _temp961; void* _temp963; struct
_tagged_arr* _temp965; void* _temp967; void* _temp969; unsigned int _temp971;
void* _temp973; struct Cyc_Absyn_Exp* _temp975; struct _tagged_arr* _temp977;
struct Cyc_Absyn_Exp* _temp979; struct _tagged_arr* _temp981; struct Cyc_Absyn_Exp*
_temp983; struct Cyc_Absyn_Exp* _temp985; struct Cyc_Absyn_Exp* _temp987; struct
Cyc_List_List* _temp989; struct Cyc_List_List* _temp991; struct Cyc_Absyn_Exp*
_temp993; struct Cyc_Absyn_Exp* _temp995; struct Cyc_Absyn_Vardecl* _temp997;
struct Cyc_Absyn_Structdecl* _temp999; struct Cyc_List_List* _temp1001; struct
Cyc_Core_Opt* _temp1003; struct _tuple0* _temp1005; struct Cyc_List_List*
_temp1007; void* _temp1009; struct Cyc_Absyn_Tunionfield* _temp1011; struct Cyc_Absyn_Tuniondecl*
_temp1013; struct Cyc_List_List* _temp1015; struct Cyc_Absyn_Tunionfield*
_temp1017; struct Cyc_Absyn_Tuniondecl* _temp1019; struct Cyc_List_List*
_temp1021; void* _temp1023; struct Cyc_Absyn_Exp* _temp1025; struct Cyc_Absyn_Stmt*
_temp1027; struct Cyc_Absyn_Fndecl* _temp1029; _LL801: if(*(( int*) _temp799) == 
Cyc_Absyn_Const_e){ _LL886: _temp885=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp799)->f1; if( _temp885 == ( void*) Cyc_Absyn_Null_c){ goto _LL802;} else{
goto _LL803;}} else{ goto _LL803;} _LL803: if(*(( int*) _temp799) ==  Cyc_Absyn_Const_e){
goto _LL804;} else{ goto _LL805;} _LL805: if(*(( int*) _temp799) ==  Cyc_Absyn_Var_e){
_LL890: _temp889=(( struct Cyc_Absyn_Var_e_struct*) _temp799)->f1; goto _LL888;
_LL888: _temp887=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp799)->f2; goto
_LL806;} else{ goto _LL807;} _LL807: if(*(( int*) _temp799) ==  Cyc_Absyn_UnknownId_e){
_LL892: _temp891=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp799)->f1; goto
_LL808;} else{ goto _LL809;} _LL809: if(*(( int*) _temp799) ==  Cyc_Absyn_Primop_e){
_LL896: _temp895=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp799)->f1;
goto _LL894; _LL894: _temp893=(( struct Cyc_Absyn_Primop_e_struct*) _temp799)->f2;
goto _LL810;} else{ goto _LL811;} _LL811: if(*(( int*) _temp799) ==  Cyc_Absyn_Increment_e){
_LL900: _temp899=(( struct Cyc_Absyn_Increment_e_struct*) _temp799)->f1; goto
_LL898; _LL898: _temp897=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp799)->f2; goto _LL812;} else{ goto _LL813;} _LL813: if(*(( int*) _temp799)
==  Cyc_Absyn_AssignOp_e){ _LL906: _temp905=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp799)->f1; goto _LL904; _LL904: _temp903=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp799)->f2; goto _LL902; _LL902: _temp901=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp799)->f3; goto _LL814;} else{ goto _LL815;} _LL815: if(*(( int*) _temp799)
==  Cyc_Absyn_Conditional_e){ _LL912: _temp911=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp799)->f1; goto _LL910; _LL910: _temp909=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp799)->f2; goto _LL908; _LL908: _temp907=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp799)->f3; goto _LL816;} else{ goto _LL817;} _LL817: if(*(( int*) _temp799)
==  Cyc_Absyn_SeqExp_e){ _LL916: _temp915=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp799)->f1; goto _LL914; _LL914: _temp913=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp799)->f2; goto _LL818;} else{ goto _LL819;} _LL819: if(*(( int*) _temp799)
==  Cyc_Absyn_UnknownCall_e){ _LL920: _temp919=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp799)->f1; goto _LL918; _LL918: _temp917=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp799)->f2; goto _LL820;} else{ goto _LL821;} _LL821: if(*(( int*) _temp799)
==  Cyc_Absyn_FnCall_e){ _LL926: _temp925=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp799)->f1; goto _LL924; _LL924: _temp923=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp799)->f2; goto _LL922; _LL922: _temp921=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp799)->f3; if( _temp921 ==  0){ goto _LL822;} else{ goto _LL823;}} else{
goto _LL823;} _LL823: if(*(( int*) _temp799) ==  Cyc_Absyn_FnCall_e){ _LL939:
_temp938=(( struct Cyc_Absyn_FnCall_e_struct*) _temp799)->f1; goto _LL937;
_LL937: _temp936=(( struct Cyc_Absyn_FnCall_e_struct*) _temp799)->f2; goto
_LL928; _LL928: _temp927=(( struct Cyc_Absyn_FnCall_e_struct*) _temp799)->f3;
if( _temp927 ==  0){ goto _LL825;} else{ _temp929=* _temp927; _LL935: _temp934=
_temp929.num_varargs; goto _LL933; _LL933: _temp932= _temp929.injectors; goto
_LL931; _LL931: _temp930= _temp929.vai; goto _LL824;}} else{ goto _LL825;}
_LL825: if(*(( int*) _temp799) ==  Cyc_Absyn_Throw_e){ _LL941: _temp940=((
struct Cyc_Absyn_Throw_e_struct*) _temp799)->f1; goto _LL826;} else{ goto _LL827;}
_LL827: if(*(( int*) _temp799) ==  Cyc_Absyn_NoInstantiate_e){ _LL943: _temp942=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp799)->f1; goto _LL828;} else{
goto _LL829;} _LL829: if(*(( int*) _temp799) ==  Cyc_Absyn_Instantiate_e){
_LL947: _temp946=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp799)->f1; goto
_LL945; _LL945: _temp944=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp799)->f2;
goto _LL830;} else{ goto _LL831;} _LL831: if(*(( int*) _temp799) ==  Cyc_Absyn_Cast_e){
_LL951: _temp950=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp799)->f1;
_temp952=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp799)->f1);
goto _LL949; _LL949: _temp948=(( struct Cyc_Absyn_Cast_e_struct*) _temp799)->f2;
goto _LL832;} else{ goto _LL833;} _LL833: if(*(( int*) _temp799) ==  Cyc_Absyn_Address_e){
_LL954: _temp953=(( struct Cyc_Absyn_Address_e_struct*) _temp799)->f1; goto
_LL834;} else{ goto _LL835;} _LL835: if(*(( int*) _temp799) ==  Cyc_Absyn_New_e){
_LL958: _temp957=(( struct Cyc_Absyn_New_e_struct*) _temp799)->f1; goto _LL956;
_LL956: _temp955=(( struct Cyc_Absyn_New_e_struct*) _temp799)->f2; goto _LL836;}
else{ goto _LL837;} _LL837: if(*(( int*) _temp799) ==  Cyc_Absyn_Sizeofexp_e){
_LL960: _temp959=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp799)->f1; goto
_LL838;} else{ goto _LL839;} _LL839: if(*(( int*) _temp799) ==  Cyc_Absyn_Sizeoftyp_e){
_LL962: _temp961=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp799)->f1;
goto _LL840;} else{ goto _LL841;} _LL841: if(*(( int*) _temp799) ==  Cyc_Absyn_Gentyp_e){
goto _LL842;} else{ goto _LL843;} _LL843: if(*(( int*) _temp799) ==  Cyc_Absyn_Offsetof_e){
_LL968: _temp967=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp799)->f1;
goto _LL964; _LL964: _temp963=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp799)->f2; if(*(( int*) _temp963) ==  Cyc_Absyn_StructField){ _LL966:
_temp965=(( struct Cyc_Absyn_StructField_struct*) _temp963)->f1; goto _LL844;}
else{ goto _LL845;}} else{ goto _LL845;} _LL845: if(*(( int*) _temp799) ==  Cyc_Absyn_Offsetof_e){
_LL974: _temp973=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp799)->f1;
goto _LL970; _LL970: _temp969=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp799)->f2; if(*(( int*) _temp969) ==  Cyc_Absyn_TupleIndex){ _LL972:
_temp971=(( struct Cyc_Absyn_TupleIndex_struct*) _temp969)->f1; goto _LL846;}
else{ goto _LL847;}} else{ goto _LL847;} _LL847: if(*(( int*) _temp799) ==  Cyc_Absyn_Deref_e){
_LL976: _temp975=(( struct Cyc_Absyn_Deref_e_struct*) _temp799)->f1; goto _LL848;}
else{ goto _LL849;} _LL849: if(*(( int*) _temp799) ==  Cyc_Absyn_StructMember_e){
_LL980: _temp979=(( struct Cyc_Absyn_StructMember_e_struct*) _temp799)->f1; goto
_LL978; _LL978: _temp977=(( struct Cyc_Absyn_StructMember_e_struct*) _temp799)->f2;
goto _LL850;} else{ goto _LL851;} _LL851: if(*(( int*) _temp799) ==  Cyc_Absyn_StructArrow_e){
_LL984: _temp983=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp799)->f1; goto
_LL982; _LL982: _temp981=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp799)->f2;
goto _LL852;} else{ goto _LL853;} _LL853: if(*(( int*) _temp799) ==  Cyc_Absyn_Subscript_e){
_LL988: _temp987=(( struct Cyc_Absyn_Subscript_e_struct*) _temp799)->f1; goto
_LL986; _LL986: _temp985=(( struct Cyc_Absyn_Subscript_e_struct*) _temp799)->f2;
goto _LL854;} else{ goto _LL855;} _LL855: if(*(( int*) _temp799) ==  Cyc_Absyn_Tuple_e){
_LL990: _temp989=(( struct Cyc_Absyn_Tuple_e_struct*) _temp799)->f1; goto _LL856;}
else{ goto _LL857;} _LL857: if(*(( int*) _temp799) ==  Cyc_Absyn_Array_e){
_LL992: _temp991=(( struct Cyc_Absyn_Array_e_struct*) _temp799)->f1; goto _LL858;}
else{ goto _LL859;} _LL859: if(*(( int*) _temp799) ==  Cyc_Absyn_Comprehension_e){
_LL998: _temp997=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp799)->f1;
goto _LL996; _LL996: _temp995=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp799)->f2; goto _LL994; _LL994: _temp993=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp799)->f3; goto _LL860;} else{ goto _LL861;} _LL861: if(*(( int*) _temp799)
==  Cyc_Absyn_Struct_e){ _LL1006: _temp1005=(( struct Cyc_Absyn_Struct_e_struct*)
_temp799)->f1; goto _LL1004; _LL1004: _temp1003=(( struct Cyc_Absyn_Struct_e_struct*)
_temp799)->f2; goto _LL1002; _LL1002: _temp1001=(( struct Cyc_Absyn_Struct_e_struct*)
_temp799)->f3; goto _LL1000; _LL1000: _temp999=(( struct Cyc_Absyn_Struct_e_struct*)
_temp799)->f4; goto _LL862;} else{ goto _LL863;} _LL863: if(*(( int*) _temp799)
==  Cyc_Absyn_AnonStruct_e){ _LL1010: _temp1009=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp799)->f1; goto _LL1008; _LL1008: _temp1007=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp799)->f2; goto _LL864;} else{ goto _LL865;} _LL865: if(*(( int*) _temp799)
==  Cyc_Absyn_Tunion_e){ _LL1016: _temp1015=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp799)->f3; if( _temp1015 ==  0){ goto _LL1014;} else{ goto _LL867;} _LL1014:
_temp1013=(( struct Cyc_Absyn_Tunion_e_struct*) _temp799)->f4; goto _LL1012;
_LL1012: _temp1011=(( struct Cyc_Absyn_Tunion_e_struct*) _temp799)->f5; goto
_LL866;} else{ goto _LL867;} _LL867: if(*(( int*) _temp799) ==  Cyc_Absyn_Tunion_e){
_LL1022: _temp1021=(( struct Cyc_Absyn_Tunion_e_struct*) _temp799)->f3; goto
_LL1020; _LL1020: _temp1019=(( struct Cyc_Absyn_Tunion_e_struct*) _temp799)->f4;
goto _LL1018; _LL1018: _temp1017=(( struct Cyc_Absyn_Tunion_e_struct*) _temp799)->f5;
goto _LL868;} else{ goto _LL869;} _LL869: if(*(( int*) _temp799) ==  Cyc_Absyn_Enum_e){
goto _LL870;} else{ goto _LL871;} _LL871: if(*(( int*) _temp799) ==  Cyc_Absyn_AnonEnum_e){
goto _LL872;} else{ goto _LL873;} _LL873: if(*(( int*) _temp799) ==  Cyc_Absyn_Malloc_e){
_LL1026: _temp1025=(( struct Cyc_Absyn_Malloc_e_struct*) _temp799)->f1; goto
_LL1024; _LL1024: _temp1023=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp799)->f2; goto _LL874;} else{ goto _LL875;} _LL875: if(*(( int*) _temp799)
==  Cyc_Absyn_StmtExp_e){ _LL1028: _temp1027=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp799)->f1; goto _LL876;} else{ goto _LL877;} _LL877: if(*(( int*) _temp799)
==  Cyc_Absyn_UnresolvedMem_e){ goto _LL878;} else{ goto _LL879;} _LL879: if(*((
int*) _temp799) ==  Cyc_Absyn_CompoundLit_e){ goto _LL880;} else{ goto _LL881;}
_LL881: if(*(( int*) _temp799) ==  Cyc_Absyn_Codegen_e){ _LL1030: _temp1029=((
struct Cyc_Absyn_Codegen_e_struct*) _temp799)->f1; goto _LL882;} else{ goto
_LL883;} _LL883: if(*(( int*) _temp799) ==  Cyc_Absyn_Fill_e){ goto _LL884;}
else{ goto _LL800;} _LL802: { struct Cyc_Absyn_Exp* _temp1031= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1031,
_temp1031))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1032[ 3u]; _temp1032[ 2u]=
_temp1031; _temp1032[ 1u]= _temp1031; _temp1032[ 0u]= _temp1031;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1032, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL800;} _LL804: goto _LL800; _LL806:{ struct _handler_cons
_temp1033; _push_handler(& _temp1033);{ int _temp1035= 0; if( setjmp( _temp1033.handler)){
_temp1035= 1;} if( ! _temp1035){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp889))->r));; _pop_handler();} else{ void* _temp1034=( void*) _exn_thrown;
void* _temp1037= _temp1034; _LL1039: if( _temp1037 ==  Cyc_Dict_Absent){ goto
_LL1040;} else{ goto _LL1041;} _LL1041: goto _LL1042; _LL1040:({ struct Cyc_Std_String_pa_struct
_temp1044; _temp1044.tag= Cyc_Std_String_pa; _temp1044.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp889);{ void* _temp1043[ 1u]={& _temp1044};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1043, sizeof( void*), 1u));}});
return; _LL1042:( void) _throw( _temp1037); _LL1038:;}}} goto _LL800; _LL808:({
void* _temp1045[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1045, sizeof( void*), 0u));}); goto _LL800; _LL810: { struct Cyc_List_List*
_temp1046=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp893);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp893);{
void* _temp1047= _temp895; _LL1049: if( _temp1047 == ( void*) Cyc_Absyn_Size){
goto _LL1050;} else{ goto _LL1051;} _LL1051: if( _temp1047 == ( void*) Cyc_Absyn_Plus){
goto _LL1052;} else{ goto _LL1053;} _LL1053: if( _temp1047 == ( void*) Cyc_Absyn_Minus){
goto _LL1054;} else{ goto _LL1055;} _LL1055: if( _temp1047 == ( void*) Cyc_Absyn_Eq){
goto _LL1056;} else{ goto _LL1057;} _LL1057: if( _temp1047 == ( void*) Cyc_Absyn_Neq){
goto _LL1058;} else{ goto _LL1059;} _LL1059: if( _temp1047 == ( void*) Cyc_Absyn_Gt){
goto _LL1060;} else{ goto _LL1061;} _LL1061: if( _temp1047 == ( void*) Cyc_Absyn_Gte){
goto _LL1062;} else{ goto _LL1063;} _LL1063: if( _temp1047 == ( void*) Cyc_Absyn_Lt){
goto _LL1064;} else{ goto _LL1065;} _LL1065: if( _temp1047 == ( void*) Cyc_Absyn_Lte){
goto _LL1066;} else{ goto _LL1067;} _LL1067: goto _LL1068; _LL1050: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp893))->hd;{
void* _temp1069= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1077; struct Cyc_Absyn_PtrInfo
_temp1079; struct Cyc_Absyn_Conref* _temp1081; void* _temp1083; _LL1071: if((
unsigned int) _temp1069 >  4u?*(( int*) _temp1069) ==  Cyc_Absyn_ArrayType: 0){
_LL1078: _temp1077=(( struct Cyc_Absyn_ArrayType_struct*) _temp1069)->f3; goto
_LL1072;} else{ goto _LL1073;} _LL1073: if(( unsigned int) _temp1069 >  4u?*((
int*) _temp1069) ==  Cyc_Absyn_PointerType: 0){ _LL1080: _temp1079=(( struct Cyc_Absyn_PointerType_struct*)
_temp1069)->f1; _LL1084: _temp1083=( void*) _temp1079.elt_typ; goto _LL1082;
_LL1082: _temp1081= _temp1079.bounds; goto _LL1074;} else{ goto _LL1075;}
_LL1075: goto _LL1076; _LL1072:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1077))->r)); goto _LL1070; _LL1074:{ void* _temp1085= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1081); struct Cyc_Absyn_Exp* _temp1091; _LL1087: if(
_temp1085 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1088;} else{ goto _LL1089;}
_LL1089: if(( unsigned int) _temp1085 >  1u?*(( int*) _temp1085) ==  Cyc_Absyn_Upper_b:
0){ _LL1092: _temp1091=(( struct Cyc_Absyn_Upper_b_struct*) _temp1085)->f1; goto
_LL1090;} else{ goto _LL1086;} _LL1088:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1093[ 2u]; _temp1093[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1083), 0); _temp1093[ 0u]=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp893))->hd;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1093, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r)); goto _LL1086;
_LL1090:( void*)( e->r=( void*)(( void*) _temp1091->r)); goto _LL1086; _LL1086:;}
goto _LL1070; _LL1076:({ struct Cyc_Std_String_pa_struct _temp1096; _temp1096.tag=
Cyc_Std_String_pa; _temp1096.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp1095; _temp1095.tag= Cyc_Std_String_pa; _temp1095.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
void* _temp1094[ 2u]={& _temp1095,& _temp1096};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u), _tag_arr( _temp1094, sizeof( void*), 2u));}}});
return; _LL1070:;} goto _LL1048;} _LL1052: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1046))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp893))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp893))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1097[ 3u]; _temp1097[ 2u]=
e2; _temp1097[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1097[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1097, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1048;} _LL1054: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1046))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp893))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp893))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1046))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1098[ 3u]; _temp1098[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1098[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1098[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1098, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1048;} _LL1056: goto _LL1058; _LL1058: goto _LL1060; _LL1060: goto
_LL1062; _LL1062: goto _LL1064; _LL1064: goto _LL1066; _LL1066: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp893))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp893))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1046))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1046))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1048;} _LL1068: goto
_LL1048; _LL1048:;} goto _LL800;} _LL812: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp899->topt))->v; Cyc_Toc_exp_to_c( nv, _temp899);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1099=( _temp897 == ( void*) Cyc_Absyn_PostInc?
1: _temp897 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1100=( _temp897 == ( void*) Cyc_Absyn_PreInc?
1: _temp897 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1099,({ struct Cyc_Absyn_Exp* _temp1101[ 3u];
_temp1101[ 2u]= Cyc_Absyn_signed_int_exp( _temp1100, 0); _temp1101[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1101[ 0u]= Cyc_Absyn_address_exp( _temp899,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1101, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp899): 0){ Cyc_Toc_lvalue_assign(
_temp899, 0, Cyc_Toc_incr_lvalue, _temp897);( void*)( e->r=( void*)(( void*)
_temp899->r));} goto _LL800;}} _LL814: { int e1_poly= Cyc_Toc_is_poly_project(
_temp905); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp905->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp901->topt))->v; Cyc_Toc_exp_to_c( nv, _temp905); Cyc_Toc_exp_to_c(
nv, _temp901);{ int done= 0; if( _temp903 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1102=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp903))->v;
_LL1104: if( _temp1102 == ( void*) Cyc_Absyn_Plus){ goto _LL1105;} else{ goto
_LL1106;} _LL1106: if( _temp1102 == ( void*) Cyc_Absyn_Minus){ goto _LL1107;}
else{ goto _LL1108;} _LL1108: goto _LL1109; _LL1105: change= _temp901; goto
_LL1103; _LL1107: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp901,
0); goto _LL1103; _LL1109:( int) _throw(({ void* _temp1110[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u), _tag_arr(
_temp1110, sizeof( void*), 0u));})); goto _LL1103; _LL1103:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp* _temp1111[ 3u]; _temp1111[ 2u]= change; _temp1111[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); _temp1111[ 0u]= Cyc_Absyn_address_exp(
_temp905, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1111, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} if( !
done){ if( e1_poly){( void*)( _temp901->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*) _temp901->r, 0), 0))->r));}
if( ! Cyc_Absyn_is_lvalue( _temp905)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)( _temp905, 0,
Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1112=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7) *  1); _temp1112[ 0]=({ struct _tuple7
_temp1113; _temp1113.f1= _temp903; _temp1113.f2= _temp901; _temp1113;});
_temp1112;}));( void*)( e->r=( void*)(( void*) _temp905->r));}} goto _LL800;}}
_LL816: Cyc_Toc_exp_to_c( nv, _temp911); Cyc_Toc_exp_to_c( nv, _temp909); Cyc_Toc_exp_to_c(
nv, _temp907); goto _LL800; _LL818: Cyc_Toc_exp_to_c( nv, _temp915); Cyc_Toc_exp_to_c(
nv, _temp913); goto _LL800; _LL820: _temp925= _temp919; _temp923= _temp917; goto
_LL822; _LL822: Cyc_Toc_exp_to_c( nv, _temp925);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp923); goto _LL800; _LL824:{
struct _RegionHandle _temp1114= _new_region(); struct _RegionHandle* r=&
_temp1114; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp934, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp930->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1131=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1131[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1132; _temp1132.tag= Cyc_Absyn_ArrayType;
_temp1132.f1=( void*) cva_type; _temp1132.f2= Cyc_Toc_mt_tq; _temp1132.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1132;}); _temp1131;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp936); int num_normargs=
num_args -  _temp934; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp936=(( struct Cyc_List_List*) _check_null( _temp936))->tl)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp936))->hd); new_args=({ struct Cyc_List_List* _temp1115=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1115->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp936))->hd);
_temp1115->tl= new_args; _temp1115;});}} new_args=({ struct Cyc_List_List*
_temp1116=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1116->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1117[ 3u]; _temp1117[ 2u]= num_varargs_exp; _temp1117[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1117[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1117, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1116->tl= new_args; _temp1116;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp938);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp938, new_args, 0), 0); if( _temp930->inject){ struct _tagged_arr vs=({
unsigned int _temp1127=( unsigned int) _temp934; struct _tuple0** _temp1128=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1127)); struct _tagged_arr _temp1130= _tag_arr( _temp1128, sizeof( struct
_tuple0*),( unsigned int) _temp934);{ unsigned int _temp1129= _temp1127;
unsigned int i; for( i= 0; i <  _temp1129; i ++){ _temp1128[ i]= Cyc_Toc_temp_var();}};
_temp1130;}); struct Cyc_List_List* _temp1118= 0;{ int i= _temp934 -  1; for( 0;
i >=  0; -- i){ _temp1118=({ struct Cyc_List_List* _temp1119=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1119->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1119->tl=
_temp1118; _temp1119;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1118, 0), s, 0);{ int i= 0;
for( 0; _temp936 !=  0;((( _temp936=(( struct Cyc_List_List*) _check_null(
_temp936))->tl, _temp932=(( struct Cyc_List_List*) _check_null( _temp932))->tl)),
++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp936))->hd; void* arg_type=( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v; struct _tuple0* var=*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)); struct Cyc_Absyn_Exp*
varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield _temp1122;
struct Cyc_List_List* _temp1123; struct _tuple0* _temp1125; struct Cyc_Absyn_Tunionfield*
_temp1120=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp932))->hd; _temp1122=* _temp1120; _LL1126: _temp1125= _temp1122.name; goto
_LL1124; _LL1124: _temp1123= _temp1122.typs; goto _LL1121; _LL1121: { void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1123))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Absyn_fieldname(
1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1125, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1125, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp936
!=  0;( _temp936=(( struct Cyc_List_List*) _check_null( _temp936))->tl, ++ i)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp936))->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp936))->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL800; _LL826: Cyc_Toc_exp_to_c( nv,
_temp940);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp940), 0))->r)); goto _LL800; _LL828: Cyc_Toc_exp_to_c(
nv, _temp942); goto _LL800; _LL830: Cyc_Toc_exp_to_c( nv, _temp946); for( 0;
_temp944 !=  0; _temp944=(( struct Cyc_List_List*) _check_null( _temp944))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp944))->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1133= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp944))->hd); _LL1135: if(( unsigned int) _temp1133 >  4u?*((
int*) _temp1133) ==  Cyc_Absyn_VarType: 0){ goto _LL1136;} else{ goto _LL1137;}
_LL1137: if(( unsigned int) _temp1133 >  4u?*(( int*) _temp1133) ==  Cyc_Absyn_TunionType:
0){ goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140; _LL1136:
continue; _LL1138: continue; _LL1140:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp946, 0))->r)); goto _LL1134; _LL1134:;} break;}} goto _LL800; _LL832: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp948->topt))->v;
void* new_typ=* _temp952;* _temp952= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp948);{ struct _tuple8 _temp1142=({ struct _tuple8 _temp1141; _temp1141.f1=
Cyc_Tcutil_compress( old_t2); _temp1141.f2= Cyc_Tcutil_compress( new_typ);
_temp1141;}); void* _temp1150; struct Cyc_Absyn_PtrInfo _temp1152; void*
_temp1154; struct Cyc_Absyn_PtrInfo _temp1156; void* _temp1158; void* _temp1160;
struct Cyc_Absyn_PtrInfo _temp1162; _LL1144: _LL1155: _temp1154= _temp1142.f1;
if(( unsigned int) _temp1154 >  4u?*(( int*) _temp1154) ==  Cyc_Absyn_PointerType:
0){ _LL1157: _temp1156=(( struct Cyc_Absyn_PointerType_struct*) _temp1154)->f1;
goto _LL1151;} else{ goto _LL1146;} _LL1151: _temp1150= _temp1142.f2; if((
unsigned int) _temp1150 >  4u?*(( int*) _temp1150) ==  Cyc_Absyn_PointerType: 0){
_LL1153: _temp1152=(( struct Cyc_Absyn_PointerType_struct*) _temp1150)->f1; goto
_LL1145;} else{ goto _LL1146;} _LL1146: _LL1161: _temp1160= _temp1142.f1; if((
unsigned int) _temp1160 >  4u?*(( int*) _temp1160) ==  Cyc_Absyn_PointerType: 0){
_LL1163: _temp1162=(( struct Cyc_Absyn_PointerType_struct*) _temp1160)->f1; goto
_LL1159;} else{ goto _LL1148;} _LL1159: _temp1158= _temp1142.f2; if((
unsigned int) _temp1158 >  4u?*(( int*) _temp1158) ==  Cyc_Absyn_IntType: 0){
goto _LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149; _LL1145: { int
_temp1164=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1156.nullable); int _temp1165=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1152.nullable); void* _temp1166= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1156.bounds); void* _temp1167= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1152.bounds);{ struct _tuple8 _temp1169=({ struct
_tuple8 _temp1168; _temp1168.f1= _temp1166; _temp1168.f2= _temp1167; _temp1168;});
void* _temp1179; void* _temp1181; void* _temp1183; void* _temp1185; struct Cyc_Absyn_Exp*
_temp1187; void* _temp1189; struct Cyc_Absyn_Exp* _temp1191; void* _temp1193;
_LL1171: _LL1182: _temp1181= _temp1169.f1; if(( unsigned int) _temp1181 >  1u?*((
int*) _temp1181) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1180;} else{ goto _LL1173;}
_LL1180: _temp1179= _temp1169.f2; if(( unsigned int) _temp1179 >  1u?*(( int*)
_temp1179) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1172;} else{ goto _LL1173;}
_LL1173: _LL1186: _temp1185= _temp1169.f1; if(( unsigned int) _temp1185 >  1u?*((
int*) _temp1185) ==  Cyc_Absyn_Upper_b: 0){ _LL1188: _temp1187=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1185)->f1; goto _LL1184;} else{ goto _LL1175;} _LL1184: _temp1183=
_temp1169.f2; if( _temp1183 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1174;}
else{ goto _LL1175;} _LL1175: _LL1194: _temp1193= _temp1169.f1; if( _temp1193 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1190;} else{ goto _LL1177;} _LL1190:
_temp1189= _temp1169.f2; if(( unsigned int) _temp1189 >  1u?*(( int*) _temp1189)
==  Cyc_Absyn_Upper_b: 0){ _LL1192: _temp1191=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1189)->f1; goto _LL1176;} else{ goto _LL1177;} _LL1177: goto _LL1178;
_LL1172: if( _temp1164? ! _temp1165: 0){ if( nv->toplevel){({ void* _temp1195[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1195, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp952, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1196=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1196->hd=( void*) _temp948;
_temp1196->tl= 0; _temp1196;}), 0), 0))->r));} goto _LL1170; _LL1174: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1187, _temp948))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1197[ 3u]; _temp1197[ 2u]=
_temp1187; _temp1197[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1152.elt_typ), 0); _temp1197[ 0u]= _temp948;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1197, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1170; _LL1176: if( nv->toplevel){({ void* _temp1198[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1198, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1199= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1201[ 3u]; _temp1201[ 2u]= _temp1191; _temp1201[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1156.elt_typ), 0); _temp1201[ 0u]= _temp948;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1201, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1165){( void*)(
_temp1199->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1200=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1200->hd=( void*) Cyc_Absyn_copy_exp( _temp1199);
_temp1200->tl= 0; _temp1200;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp952, _temp1199, 0))->r)); goto _LL1170;} _LL1178: goto _LL1170; _LL1170:;}
goto _LL1143;} _LL1147:{ void* _temp1202= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1162.bounds); _LL1204: if( _temp1202 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1205;} else{ goto _LL1206;} _LL1206: goto _LL1207; _LL1205:( void*)( _temp948->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp948->r,
_temp948->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1203; _LL1207: goto _LL1203;
_LL1203:;} goto _LL1143; _LL1149: goto _LL1143; _LL1143:;} goto _LL800;} _LL834:{
void* _temp1208=( void*) _temp953->r; struct Cyc_List_List* _temp1216; struct
_tuple0* _temp1218; struct Cyc_List_List* _temp1220; _LL1210: if(*(( int*)
_temp1208) ==  Cyc_Absyn_Struct_e){ _LL1219: _temp1218=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1208)->f1; goto _LL1217; _LL1217: _temp1216=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1208)->f3; goto _LL1211;} else{ goto _LL1212;} _LL1212: if(*(( int*)
_temp1208) ==  Cyc_Absyn_Tuple_e){ _LL1221: _temp1220=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1208)->f1; goto _LL1213;} else{ goto _LL1214;} _LL1214: goto _LL1215;
_LL1211: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1223;
_temp1223.tag= Cyc_Std_String_pa; _temp1223.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp953->loc);{ void* _temp1222[ 1u]={& _temp1223};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1222, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp953->topt))->v, 1, 0, _temp1216, _temp1218))->r)); goto
_LL1209; _LL1213: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1225;
_temp1225.tag= Cyc_Std_String_pa; _temp1225.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp953->loc);{ void* _temp1224[ 1u]={& _temp1225};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1224, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1220))->r));
goto _LL1209; _LL1215: Cyc_Toc_exp_to_c( nv, _temp953); if( ! Cyc_Absyn_is_lvalue(
_temp953)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp953, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp953, 0))->r));} goto _LL1209; _LL1209:;} goto _LL800; _LL836: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1227; _temp1227.tag= Cyc_Std_String_pa;
_temp1227.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp955->loc);{
void* _temp1226[ 1u]={& _temp1227};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1226, sizeof( void*), 1u));}});}{ void*
_temp1228=( void*) _temp955->r; struct Cyc_List_List* _temp1240; struct Cyc_Absyn_Exp*
_temp1242; struct Cyc_Absyn_Exp* _temp1244; struct Cyc_Absyn_Vardecl* _temp1246;
struct Cyc_List_List* _temp1248; struct _tuple0* _temp1250; struct Cyc_List_List*
_temp1252; _LL1230: if(*(( int*) _temp1228) ==  Cyc_Absyn_Array_e){ _LL1241:
_temp1240=(( struct Cyc_Absyn_Array_e_struct*) _temp1228)->f1; goto _LL1231;}
else{ goto _LL1232;} _LL1232: if(*(( int*) _temp1228) ==  Cyc_Absyn_Comprehension_e){
_LL1247: _temp1246=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1228)->f1;
goto _LL1245; _LL1245: _temp1244=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1228)->f2; goto _LL1243; _LL1243: _temp1242=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1228)->f3; goto _LL1233;} else{ goto _LL1234;} _LL1234: if(*(( int*)
_temp1228) ==  Cyc_Absyn_Struct_e){ _LL1251: _temp1250=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1228)->f1; goto _LL1249; _LL1249: _temp1248=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1228)->f3; goto _LL1235;} else{ goto _LL1236;} _LL1236: if(*(( int*)
_temp1228) ==  Cyc_Absyn_Tuple_e){ _LL1253: _temp1252=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1228)->f1; goto _LL1237;} else{ goto _LL1238;} _LL1238: goto _LL1239;
_LL1231: { struct _tuple0* _temp1254= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1255= Cyc_Absyn_var_exp( _temp1254, 0); struct Cyc_Absyn_Stmt* _temp1256=
Cyc_Toc_init_array( nv, _temp1255, _temp1240, Cyc_Absyn_exp_stmt( _temp1255, 0));
void* old_elt_typ;{ void* _temp1257= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1263; struct Cyc_Absyn_Tqual _temp1265; void* _temp1267; _LL1259: if((
unsigned int) _temp1257 >  4u?*(( int*) _temp1257) ==  Cyc_Absyn_PointerType: 0){
_LL1264: _temp1263=(( struct Cyc_Absyn_PointerType_struct*) _temp1257)->f1;
_LL1268: _temp1267=( void*) _temp1263.elt_typ; goto _LL1266; _LL1266: _temp1265=
_temp1263.tq; goto _LL1260;} else{ goto _LL1261;} _LL1261: goto _LL1262; _LL1260:
old_elt_typ= _temp1267; goto _LL1258; _LL1262: old_elt_typ=({ void* _temp1269[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1269, sizeof( void*), 0u));}); goto
_LL1258; _LL1258:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1270= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1271= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1240), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp957 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1271);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp957); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1271);}{
struct Cyc_Absyn_Exp* _temp1272= Cyc_Absyn_cast_exp( _temp1270, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1254,
_temp1270,( struct Cyc_Absyn_Exp*) _temp1272, _temp1256, 0), 0))->r)); goto
_LL1229;}}} _LL1233: { int is_tagged_ptr= 0;{ void* _temp1273= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1279; struct Cyc_Absyn_Conref* _temp1281;
struct Cyc_Absyn_Tqual _temp1283; void* _temp1285; _LL1275: if(( unsigned int)
_temp1273 >  4u?*(( int*) _temp1273) ==  Cyc_Absyn_PointerType: 0){ _LL1280:
_temp1279=(( struct Cyc_Absyn_PointerType_struct*) _temp1273)->f1; _LL1286:
_temp1285=( void*) _temp1279.elt_typ; goto _LL1284; _LL1284: _temp1283=
_temp1279.tq; goto _LL1282; _LL1282: _temp1281= _temp1279.bounds; goto _LL1276;}
else{ goto _LL1277;} _LL1277: goto _LL1278; _LL1276: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1281) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1274;
_LL1278:({ void* _temp1287[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1287, sizeof( void*), 0u));}); goto
_LL1274; _LL1274:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1242->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1244);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1246, Cyc_Absyn_var_exp( max, 0), _temp1242, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1288= _new_region(); struct _RegionHandle* r=&
_temp1288; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1308=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1308->hd=( void*)({ struct _tuple9* _temp1309=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1309->f1= max; _temp1309->f2=
Cyc_Absyn_uint_t; _temp1309->f3=( struct Cyc_Absyn_Exp*) _temp1244; _temp1309;});
_temp1308->tl= 0; _temp1308;}); struct Cyc_Absyn_Exp* ai; if( _temp957 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1289[ 2u]; _temp1289[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1289[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1289, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp957); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1290[
2u]; _temp1290[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1290[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1290, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1291=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1291->hd=( void*)({ struct _tuple9* _temp1292=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1292->f1= a; _temp1292->f2=
ptr_typ; _temp1292->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1292;}); _temp1291->tl=
decls; _temp1291;}); if( is_tagged_ptr){ struct _tuple0* _temp1293= Cyc_Toc_temp_var();
void* _temp1294= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1295=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1298[ 3u];
_temp1298[ 2u]= _temp1244; _temp1298[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1298[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1298, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1296=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1296->hd=( void*)({
struct _tuple9* _temp1297=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1297->f1= _temp1293; _temp1297->f2= _temp1294; _temp1297->f3=(
struct Cyc_Absyn_Exp*) _temp1295; _temp1297;}); _temp1296->tl= decls; _temp1296;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1293, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1299= decls; for( 0; _temp1299 !=  0;
_temp1299=(( struct Cyc_List_List*) _check_null( _temp1299))->tl){ struct Cyc_Absyn_Exp*
_temp1302; void* _temp1304; struct _tuple0* _temp1306; struct _tuple9 _temp1300=*((
struct _tuple9*)(( struct Cyc_List_List*) _check_null( _temp1299))->hd); _LL1307:
_temp1306= _temp1300.f1; goto _LL1305; _LL1305: _temp1304= _temp1300.f2; goto
_LL1303; _LL1303: _temp1302= _temp1300.f3; goto _LL1301; _LL1301: s= Cyc_Absyn_declare_stmt(
_temp1306, _temp1304, _temp1302, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1229;}}} _LL1235:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp955->topt))->v, 1, _temp957, _temp1248, _temp1250))->r)); goto _LL1229;
_LL1237:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp957,
_temp1252))->r)); goto _LL1229; _LL1239: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp955->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp957 ==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp957); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1310=( void*)
_temp955->r; struct Cyc_Absyn_Exp* _temp1316; void* _temp1318; _LL1312: if(*((
int*) _temp1310) ==  Cyc_Absyn_Cast_e){ _LL1319: _temp1318=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1310)->f1; goto _LL1317; _LL1317: _temp1316=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1310)->f2; goto _LL1313;} else{ goto _LL1314;} _LL1314: goto _LL1315;
_LL1313:{ struct _tuple8 _temp1321=({ struct _tuple8 _temp1320; _temp1320.f1=
Cyc_Tcutil_compress( _temp1318); _temp1320.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1316->topt))->v); _temp1320;}); void*
_temp1327; struct Cyc_Absyn_PtrInfo _temp1329; struct Cyc_Absyn_Conref*
_temp1331; void* _temp1333; struct Cyc_Absyn_PtrInfo _temp1335; struct Cyc_Absyn_Conref*
_temp1337; void* _temp1339; _LL1323: _LL1334: _temp1333= _temp1321.f1; if((
unsigned int) _temp1333 >  4u?*(( int*) _temp1333) ==  Cyc_Absyn_PointerType: 0){
_LL1336: _temp1335=(( struct Cyc_Absyn_PointerType_struct*) _temp1333)->f1;
_LL1340: _temp1339=( void*) _temp1335.elt_typ; goto _LL1338; _LL1338: _temp1337=
_temp1335.bounds; goto _LL1328;} else{ goto _LL1325;} _LL1328: _temp1327=
_temp1321.f2; if(( unsigned int) _temp1327 >  4u?*(( int*) _temp1327) ==  Cyc_Absyn_PointerType:
0){ _LL1330: _temp1329=(( struct Cyc_Absyn_PointerType_struct*) _temp1327)->f1;
_LL1332: _temp1331= _temp1329.bounds; goto _LL1324;} else{ goto _LL1325;}
_LL1325: goto _LL1326; _LL1324:{ struct _tuple8 _temp1342=({ struct _tuple8
_temp1341; _temp1341.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1337);
_temp1341.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1331); _temp1341;});
void* _temp1348; struct Cyc_Absyn_Exp* _temp1350; void* _temp1352; _LL1344:
_LL1353: _temp1352= _temp1342.f1; if( _temp1352 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1349;} else{ goto _LL1346;} _LL1349: _temp1348= _temp1342.f2; if((
unsigned int) _temp1348 >  1u?*(( int*) _temp1348) ==  Cyc_Absyn_Upper_b: 0){
_LL1351: _temp1350=(( struct Cyc_Absyn_Upper_b_struct*) _temp1348)->f1; goto
_LL1345;} else{ goto _LL1346;} _LL1346: goto _LL1347; _LL1345: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1354[ 4u]; _temp1354[ 3u]= _temp1350; _temp1354[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1339, 0); _temp1354[ 1u]= _temp1316; _temp1354[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1354, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1343; _LL1347: goto _LL1343; _LL1343:;} goto _LL1322;
_LL1326: goto _LL1322; _LL1322:;} goto _LL1311; _LL1315: goto _LL1311; _LL1311:;}
if( ! done){ struct _tuple0* _temp1355= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1356= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1355, 0), 0); struct Cyc_Absyn_Exp*
_temp1357= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp955);
_temp1356= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1355, 0), _temp1357, 0), _temp955, 0), _temp1356, 0);{
void* _temp1358= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1355,
_temp1358,( struct Cyc_Absyn_Exp*) mexp, _temp1356, 0), 0))->r));}} goto _LL1229;}}
_LL1229:;} goto _LL800; _LL838: Cyc_Toc_exp_to_c( nv, _temp959); goto _LL800;
_LL840:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1359=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1359[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1360; _temp1360.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1360.f1=( void*) Cyc_Toc_typ_to_c_array( _temp961);
_temp1360;}); _temp1359;}))); goto _LL800; _LL842:( int) _throw(({ void*
_temp1361[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("__gen() in code generator",
sizeof( unsigned char), 26u), _tag_arr( _temp1361, sizeof( void*), 0u));}));
_LL844:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1362=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1362[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1363; _temp1363.tag=
Cyc_Absyn_Offsetof_e; _temp1363.f1=( void*) Cyc_Toc_typ_to_c_array( _temp967);
_temp1363.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1364=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1364[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1365; _temp1365.tag=
Cyc_Absyn_StructField; _temp1365.f1= _temp965; _temp1365;}); _temp1364;}));
_temp1363;}); _temp1362;}))); goto _LL800; _LL846:{ void* _temp1366= Cyc_Tcutil_compress(
_temp973); struct Cyc_Absyn_Structdecl** _temp1382; struct Cyc_List_List*
_temp1384; struct Cyc_Absyn_Uniondecl** _temp1386; struct Cyc_List_List*
_temp1388; _LL1368: if(( unsigned int) _temp1366 >  4u?*(( int*) _temp1366) == 
Cyc_Absyn_StructType: 0){ _LL1383: _temp1382=(( struct Cyc_Absyn_StructType_struct*)
_temp1366)->f3; goto _LL1369;} else{ goto _LL1370;} _LL1370: if(( unsigned int)
_temp1366 >  4u?*(( int*) _temp1366) ==  Cyc_Absyn_AnonStructType: 0){ _LL1385:
_temp1384=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1366)->f1; goto
_LL1371;} else{ goto _LL1372;} _LL1372: if(( unsigned int) _temp1366 >  4u?*((
int*) _temp1366) ==  Cyc_Absyn_UnionType: 0){ _LL1387: _temp1386=(( struct Cyc_Absyn_UnionType_struct*)
_temp1366)->f3; goto _LL1373;} else{ goto _LL1374;} _LL1374: if(( unsigned int)
_temp1366 >  4u?*(( int*) _temp1366) ==  Cyc_Absyn_AnonUnionType: 0){ _LL1389:
_temp1388=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1366)->f1; goto
_LL1375;} else{ goto _LL1376;} _LL1376: if(( unsigned int) _temp1366 >  4u?*((
int*) _temp1366) ==  Cyc_Absyn_TupleType: 0){ goto _LL1377;} else{ goto _LL1378;}
_LL1378: if(( unsigned int) _temp1366 >  4u?*(( int*) _temp1366) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL1379;} else{ goto _LL1380;} _LL1380: goto _LL1381; _LL1369: if(
_temp1382 ==  0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1382)))->fields
==  0){( int) _throw(({ void* _temp1390[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("struct fields must be known",
sizeof( unsigned char), 28u), _tag_arr( _temp1390, sizeof( void*), 0u));}));}
_temp1384=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp1382)))->fields))->v; goto
_LL1371; _LL1371: { struct Cyc_Absyn_Structfield* _temp1391=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1384,( int) _temp971);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1392=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1392[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1393; _temp1393.tag=
Cyc_Absyn_Offsetof_e; _temp1393.f1=( void*) Cyc_Toc_typ_to_c_array( _temp973);
_temp1393.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1394=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1394[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1395; _temp1395.tag=
Cyc_Absyn_StructField; _temp1395.f1= _temp1391->name; _temp1395;}); _temp1394;}));
_temp1393;}); _temp1392;}))); goto _LL1367;} _LL1373: if( _temp1386 ==  0? 1:(*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp1386)))->fields ==  0){( int)
_throw(({ void* _temp1396[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("union fields must be known",
sizeof( unsigned char), 27u), _tag_arr( _temp1396, sizeof( void*), 0u));}));}
_temp1388=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp1386)))->fields))->v; goto
_LL1375; _LL1375: { struct Cyc_Absyn_Structfield* _temp1397=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1388,( int) _temp971);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1398=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1398[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1399; _temp1399.tag=
Cyc_Absyn_Offsetof_e; _temp1399.f1=( void*) Cyc_Toc_typ_to_c_array( _temp973);
_temp1399.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1400=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1400[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1401; _temp1401.tag=
Cyc_Absyn_StructField; _temp1401.f1= _temp1397->name; _temp1401;}); _temp1400;}));
_temp1399;}); _temp1398;}))); goto _LL1367;} _LL1377:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1402=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1402[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1403; _temp1403.tag= Cyc_Absyn_Offsetof_e;
_temp1403.f1=( void*) Cyc_Toc_typ_to_c_array( _temp973); _temp1403.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1404=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1404[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1405; _temp1405.tag= Cyc_Absyn_StructField;
_temp1405.f1= Cyc_Absyn_fieldname(( int)( _temp971 +  1)); _temp1405;});
_temp1404;})); _temp1403;}); _temp1402;}))); goto _LL1367; _LL1379: if( _temp971
==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1406=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1406[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1407; _temp1407.tag=
Cyc_Absyn_Offsetof_e; _temp1407.f1=( void*) Cyc_Toc_typ_to_c_array( _temp973);
_temp1407.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1408=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1408[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1409; _temp1409.tag=
Cyc_Absyn_StructField; _temp1409.f1= Cyc_Toc_tag_sp; _temp1409;}); _temp1408;}));
_temp1407;}); _temp1406;})));} else{( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1410=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1410[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1411; _temp1411.tag= Cyc_Absyn_Offsetof_e;
_temp1411.f1=( void*) Cyc_Toc_typ_to_c_array( _temp973); _temp1411.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1412=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1412[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1413; _temp1413.tag= Cyc_Absyn_StructField;
_temp1413.f1= Cyc_Absyn_fieldname(( int) _temp971); _temp1413;}); _temp1412;}));
_temp1411;}); _temp1410;})));} goto _LL1367; _LL1381:( int) _throw(({ void*
_temp1414[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("impossible type for offsetof tuple index",
sizeof( unsigned char), 41u), _tag_arr( _temp1414, sizeof( void*), 0u));}));
_LL1367:;} goto _LL800; _LL848: { void* _temp1415= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp975->topt))->v);{ void* _temp1416=
_temp1415; struct Cyc_Absyn_PtrInfo _temp1422; struct Cyc_Absyn_Conref*
_temp1424; struct Cyc_Absyn_Tqual _temp1426; struct Cyc_Absyn_Conref* _temp1428;
void* _temp1430; void* _temp1432; _LL1418: if(( unsigned int) _temp1416 >  4u?*((
int*) _temp1416) ==  Cyc_Absyn_PointerType: 0){ _LL1423: _temp1422=(( struct Cyc_Absyn_PointerType_struct*)
_temp1416)->f1; _LL1433: _temp1432=( void*) _temp1422.elt_typ; goto _LL1431;
_LL1431: _temp1430=( void*) _temp1422.rgn_typ; goto _LL1429; _LL1429: _temp1428=
_temp1422.nullable; goto _LL1427; _LL1427: _temp1426= _temp1422.tq; goto _LL1425;
_LL1425: _temp1424= _temp1422.bounds; goto _LL1419;} else{ goto _LL1420;}
_LL1420: goto _LL1421; _LL1419:{ void* _temp1434= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1424); _LL1436: if(( unsigned int) _temp1434 >  1u?*(( int*) _temp1434) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1437;} else{ goto _LL1438;} _LL1438: if(
_temp1434 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1439;} else{ goto _LL1435;}
_LL1437: Cyc_Toc_exp_to_c( nv, _temp975); if( Cyc_Toc_is_nullable( _temp1415)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp975->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1440=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1440->hd=( void*) _temp975;
_temp1440->tl= 0; _temp1440;}), 0), 0), 0))->r));} goto _LL1435; _LL1439: {
struct Cyc_Absyn_Exp* _temp1441= Cyc_Absyn_uint_exp( 0, 0); _temp1441->topt=({
struct Cyc_Core_Opt* _temp1442=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1442->v=( void*) Cyc_Absyn_uint_t; _temp1442;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp975, _temp1441, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1435;} _LL1435:;} goto _LL1417; _LL1421:({ void* _temp1443[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1443, sizeof( void*), 0u));}); goto _LL1417; _LL1417:;} goto _LL800;}
_LL850: Cyc_Toc_exp_to_c( nv, _temp979); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL800; _LL852: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp983->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp983);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1446; struct Cyc_Absyn_Tqual _temp1448; struct Cyc_Absyn_Conref* _temp1450;
void* _temp1452; void* _temp1454; struct Cyc_Absyn_PtrInfo _temp1444= Cyc_Toc_get_ptr_type(
e1typ); _LL1455: _temp1454=( void*) _temp1444.elt_typ; goto _LL1453; _LL1453:
_temp1452=( void*) _temp1444.rgn_typ; goto _LL1451; _LL1451: _temp1450=
_temp1444.nullable; goto _LL1449; _LL1449: _temp1448= _temp1444.tq; goto _LL1447;
_LL1447: _temp1446= _temp1444.bounds; goto _LL1445; _LL1445:{ void* _temp1456=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1446); struct Cyc_Absyn_Exp*
_temp1462; _LL1458: if(( unsigned int) _temp1456 >  1u?*(( int*) _temp1456) == 
Cyc_Absyn_Upper_b: 0){ _LL1463: _temp1462=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1456)->f1; goto _LL1459;} else{ goto _LL1460;} _LL1460: if( _temp1456 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1461;} else{ goto _LL1457;} _LL1459: if(
Cyc_Evexp_eval_const_uint_exp( _temp1462) <  1){({ void* _temp1464[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1464, sizeof( void*), 0u));});}{ int possibly_null=(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1450); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp983->topt))->v)){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp983->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1465=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1465->hd=( void*) _temp983;
_temp1465->tl= 0; _temp1465;}), 0), 0), _temp981, 0))->r));} goto _LL1457;}
_LL1461: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1454);( void*)( _temp983->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1448), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1466[ 3u];
_temp1466[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1466[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1466[ 0u]= Cyc_Absyn_copy_exp( _temp983);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1466, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1457;} _LL1457:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL800;}} _LL854: { void* _temp1467= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp987->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp987); Cyc_Toc_exp_to_c( nv, _temp985);{ void* _temp1468= _temp1467; struct
Cyc_List_List* _temp1476; struct Cyc_Absyn_PtrInfo _temp1478; struct Cyc_Absyn_Conref*
_temp1480; struct Cyc_Absyn_Tqual _temp1482; struct Cyc_Absyn_Conref* _temp1484;
void* _temp1486; void* _temp1488; _LL1470: if(( unsigned int) _temp1468 >  4u?*((
int*) _temp1468) ==  Cyc_Absyn_TupleType: 0){ _LL1477: _temp1476=(( struct Cyc_Absyn_TupleType_struct*)
_temp1468)->f1; goto _LL1471;} else{ goto _LL1472;} _LL1472: if(( unsigned int)
_temp1468 >  4u?*(( int*) _temp1468) ==  Cyc_Absyn_PointerType: 0){ _LL1479:
_temp1478=(( struct Cyc_Absyn_PointerType_struct*) _temp1468)->f1; _LL1489:
_temp1488=( void*) _temp1478.elt_typ; goto _LL1487; _LL1487: _temp1486=( void*)
_temp1478.rgn_typ; goto _LL1485; _LL1485: _temp1484= _temp1478.nullable; goto
_LL1483; _LL1483: _temp1482= _temp1478.tq; goto _LL1481; _LL1481: _temp1480=
_temp1478.bounds; goto _LL1473;} else{ goto _LL1474;} _LL1474: goto _LL1475;
_LL1471: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp985) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp987, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1469;} _LL1473:{ void* _temp1490= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1480); struct Cyc_Absyn_Exp* _temp1496; _LL1492: if((
unsigned int) _temp1490 >  1u?*(( int*) _temp1490) ==  Cyc_Absyn_Upper_b: 0){
_LL1497: _temp1496=(( struct Cyc_Absyn_Upper_b_struct*) _temp1490)->f1; goto
_LL1493;} else{ goto _LL1494;} _LL1494: if( _temp1490 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1495;} else{ goto _LL1491;} _LL1493: _temp1496= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1496), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1484); void* ta1= Cyc_Toc_typ_to_c( _temp1488);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1482); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1498[ 4u];
_temp1498[ 3u]= _temp985; _temp1498[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1498[ 1u]= _temp1496; _temp1498[ 0u]= _temp987;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1498, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp985->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1499[ 2u];
_temp1499[ 1u]= Cyc_Absyn_copy_exp( _temp985); _temp1499[ 0u]= _temp1496;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1499, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1491;}
_LL1495: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1488);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1482), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1500[ 3u]; _temp1500[ 2u]= _temp985; _temp1500[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1500[ 0u]= _temp987;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1500, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1491;} _LL1491:;} goto _LL1469; _LL1475:({
void* _temp1501[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1501, sizeof( void*), 0u));}); goto
_LL1469; _LL1469:;} goto _LL800;} _LL856: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp989))->r));} else{ struct
Cyc_List_List* _temp1502=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp989); struct _tagged_arr* _temp1503= Cyc_Toc_add_tuple_type( _temp1502);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp989 !=  0;( _temp989=((
struct Cyc_List_List*) _check_null( _temp989))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp989))->hd);
dles=({ struct Cyc_List_List* _temp1504=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1504->hd=( void*)({ struct _tuple4*
_temp1505=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1505->f1=
0; _temp1505->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp989))->hd; _temp1505;}); _temp1504->tl= dles; _temp1504;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL800; _LL858:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp991, 0))->r));{ struct Cyc_List_List* _temp1506= _temp991; for( 0;
_temp1506 !=  0; _temp1506=(( struct Cyc_List_List*) _check_null( _temp1506))->tl){
struct _tuple4 _temp1509; struct Cyc_Absyn_Exp* _temp1510; struct _tuple4*
_temp1507=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1506))->hd;
_temp1509=* _temp1507; _LL1511: _temp1510= _temp1509.f2; goto _LL1508; _LL1508:
Cyc_Toc_exp_to_c( nv, _temp1510);}} goto _LL800; _LL860: { unsigned int
_temp1512= Cyc_Evexp_eval_const_uint_exp( _temp995); Cyc_Toc_exp_to_c( nv,
_temp993);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp993)){
unsigned int i= 0; for( 0; i <  _temp1512; i ++){ es=({ struct Cyc_List_List*
_temp1513=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1513->hd=( void*)({ struct _tuple4* _temp1514=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1514->f1= 0; _temp1514->f2= _temp993; _temp1514;});
_temp1513->tl= es; _temp1513;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL800;}} _LL862: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1001, _temp1005))->r));}
else{ if( _temp999 ==  0){({ void* _temp1515[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1515, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp999); struct _RegionHandle _temp1516= _new_region(); struct _RegionHandle*
rgn=& _temp1516; _push_region( rgn);{ struct Cyc_List_List* _temp1517=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1001,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1518= 0;{ struct Cyc_List_List*
_temp1519=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1519 !=  0; _temp1519=(( struct Cyc_List_List*) _check_null(
_temp1519))->tl){ struct Cyc_List_List* _temp1520= _temp1517; for( 0; _temp1520
!=  0; _temp1520=(( struct Cyc_List_List*) _check_null( _temp1520))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1520))->hd)).f1 == (
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1519))->hd){
struct _tuple10 _temp1523; struct Cyc_Absyn_Exp* _temp1524; struct Cyc_Absyn_Structfield*
_temp1526; struct _tuple10* _temp1521=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1520))->hd; _temp1523=* _temp1521; _LL1527: _temp1526=
_temp1523.f1; goto _LL1525; _LL1525: _temp1524= _temp1523.f2; goto _LL1522;
_LL1522: { void* _temp1528=( void*) _temp1526->type; Cyc_Toc_exp_to_c( nv,
_temp1524); if( Cyc_Toc_is_void_star( _temp1528)){( void*)( _temp1524->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1524->r, 0), 0))->r));} _temp1518=({ struct Cyc_List_List* _temp1529=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1529->hd=(
void*)({ struct _tuple4* _temp1530=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1530->f1= 0; _temp1530->f2= _temp1524; _temp1530;}); _temp1529->tl=
_temp1518; _temp1529;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1518), 0))->r));}; _pop_region( rgn);}} goto _LL800; _LL864: { struct Cyc_List_List*
fs;{ void* _temp1531= Cyc_Tcutil_compress( _temp1009); struct Cyc_List_List*
_temp1537; _LL1533: if(( unsigned int) _temp1531 >  4u?*(( int*) _temp1531) == 
Cyc_Absyn_AnonStructType: 0){ _LL1538: _temp1537=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1531)->f1; goto _LL1534;} else{ goto _LL1535;} _LL1535: goto _LL1536;
_LL1534: fs= _temp1537; goto _LL1532; _LL1536: fs=({ struct Cyc_Std_String_pa_struct
_temp1540; _temp1540.tag= Cyc_Std_String_pa; _temp1540.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp1009);{ void* _temp1539[ 1u]={& _temp1540};((
struct Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s", sizeof( unsigned char), 24u), _tag_arr(
_temp1539, sizeof( void*), 1u));}}); goto _LL1532; _LL1532:;}{ struct
_RegionHandle _temp1541= _new_region(); struct _RegionHandle* rgn=& _temp1541;
_push_region( rgn);{ struct Cyc_List_List* _temp1542=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1007, fs); for( 0; _temp1542 !=  0; _temp1542=(( struct Cyc_List_List*)
_check_null( _temp1542))->tl){ struct _tuple10 _temp1545; struct Cyc_Absyn_Exp*
_temp1546; struct Cyc_Absyn_Structfield* _temp1548; struct _tuple10* _temp1543=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1542))->hd;
_temp1545=* _temp1543; _LL1549: _temp1548= _temp1545.f1; goto _LL1547; _LL1547:
_temp1546= _temp1545.f2; goto _LL1544; _LL1544: { void* _temp1550=( void*)
_temp1548->type; Cyc_Toc_exp_to_c( nv, _temp1546); if( Cyc_Toc_is_void_star(
_temp1550)){( void*)( _temp1546->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1546->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1007, 0))->r));}; _pop_region( rgn);}
goto _LL800;} _LL866: { struct _tuple0* qv= _temp1011->name; if( _temp1013->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1551= _temp1013->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1013->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1551))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1551))->hd)->typs ==  0){ tag_count ++;}
_temp1551=(( struct Cyc_List_List*) _check_null( _temp1551))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL800;} _LL868: { struct _tuple0* _temp1552= _temp1017->name; struct Cyc_List_List*
_temp1553= _temp1017->typs; struct _tuple0* _temp1554= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1555= Cyc_Absyn_var_exp( _temp1554, 0); void*
_temp1556= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1552, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1019->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1552, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1557= _temp1019->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1019->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1552,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1557))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1557))->hd)->typs !=  0){ tag_count ++;}
_temp1557=(( struct Cyc_List_List*) _check_null( _temp1557))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1021
!=  0;( _temp1021=(( struct Cyc_List_List*) _check_null( _temp1021))->tl,
_temp1553=(( struct Cyc_List_List*) _check_null( _temp1553))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1021))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1553))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1558=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1558->hd=( void*)({ struct _tuple4* _temp1559=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1559->f1= 0; _temp1559->f2= cur_e; _temp1559;});
_temp1558->tl= dles; _temp1558;});} dles=({ struct Cyc_List_List* _temp1560=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1560->hd=(
void*)({ struct _tuple4* _temp1561=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1561->f1= 0; _temp1561->f2= tag_exp; _temp1561;}); _temp1560->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1560;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1562=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1562->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1552,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1562;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1563=({ struct Cyc_List_List* _temp1569=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1569->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1555, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1552, 0), 0); _temp1569->tl= 0; _temp1569;});{ int i=
1; for( 0; _temp1021 !=  0;((( _temp1021=(( struct Cyc_List_List*) _check_null(
_temp1021))->tl, i ++)), _temp1553=(( struct Cyc_List_List*) _check_null(
_temp1553))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1021))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1553))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp1564= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1555, Cyc_Absyn_fieldname( i), 0), cur_e, 0);
_temp1563=({ struct Cyc_List_List* _temp1565=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1565->hd=( void*) _temp1564; _temp1565->tl=
_temp1563; _temp1565;});}}}{ struct Cyc_Absyn_Stmt* _temp1566= Cyc_Absyn_exp_stmt(
_temp1555, 0); struct Cyc_Absyn_Stmt* _temp1567= Cyc_Absyn_seq_stmts((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List*
_temp1568=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1568->hd=( void*) _temp1566; _temp1568->tl= _temp1563; _temp1568;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1554, _temp1556, 0, _temp1567, 0), 0))->r));}} goto _LL800;} _LL870: goto
_LL800; _LL872: goto _LL800; _LL874: { void* t_c= Cyc_Toc_typ_to_c( _temp1023);
if( _temp1025 !=  0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)
_check_null( _temp1025); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1023, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL800;} _LL876: Cyc_Toc_stmt_to_c( nv, _temp1027); goto
_LL800; _LL878:({ void* _temp1570[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem", sizeof(
unsigned char), 14u), _tag_arr( _temp1570, sizeof( void*), 0u));}); goto _LL800;
_LL880:({ void* _temp1571[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof( unsigned char),
12u), _tag_arr( _temp1571, sizeof( void*), 0u));}); goto _LL800; _LL882:({ void*
_temp1572[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("codegen", sizeof( unsigned char), 8u), _tag_arr( _temp1572, sizeof(
void*), 0u));}); goto _LL800; _LL884:({ void* _temp1573[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u), _tag_arr( _temp1573, sizeof( void*), 0u));}); goto
_LL800; _LL800:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1574=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1612; struct Cyc_Absyn_Vardecl _temp1614; struct _tuple0* _temp1615; struct
Cyc_Absyn_Vardecl* _temp1617; int _temp1619; void* _temp1621; unsigned char
_temp1623; struct _tagged_arr _temp1625; struct Cyc_Absyn_Enumfield* _temp1627;
struct Cyc_Absyn_Enumdecl* _temp1629; struct Cyc_Absyn_Enumfield* _temp1631;
void* _temp1633; struct Cyc_List_List* _temp1635; struct Cyc_List_List*
_temp1637; struct Cyc_Absyn_Tunionfield* _temp1639; struct Cyc_Absyn_Tuniondecl*
_temp1641; struct Cyc_Absyn_Pat* _temp1644; struct Cyc_Absyn_Pat _temp1646; void*
_temp1647; struct Cyc_List_List* _temp1649; struct Cyc_List_List* _temp1651;
struct Cyc_Absyn_Tunionfield* _temp1653; struct Cyc_Absyn_Tuniondecl* _temp1655;
struct Cyc_List_List* _temp1657; struct Cyc_List_List* _temp1659; struct Cyc_List_List*
_temp1661; struct Cyc_List_List* _temp1663; struct Cyc_Core_Opt* _temp1665;
struct Cyc_Absyn_Structdecl* _temp1667; struct Cyc_Absyn_Pat* _temp1669; _LL1576:
if(( unsigned int) _temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_Var_p: 0){
_LL1613: _temp1612=(( struct Cyc_Absyn_Var_p_struct*) _temp1574)->f1; _temp1614=*
_temp1612; _LL1616: _temp1615= _temp1614.name; goto _LL1577;} else{ goto _LL1578;}
_LL1578: if( _temp1574 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1579;} else{ goto
_LL1580;} _LL1580: if(( unsigned int) _temp1574 >  2u?*(( int*) _temp1574) == 
Cyc_Absyn_Reference_p: 0){ _LL1618: _temp1617=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1574)->f1; goto _LL1581;} else{ goto _LL1582;} _LL1582: if( _temp1574 == (
void*) Cyc_Absyn_Null_p){ goto _LL1583;} else{ goto _LL1584;} _LL1584: if((
unsigned int) _temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_Int_p: 0){
_LL1622: _temp1621=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1574)->f1;
goto _LL1620; _LL1620: _temp1619=(( struct Cyc_Absyn_Int_p_struct*) _temp1574)->f2;
goto _LL1585;} else{ goto _LL1586;} _LL1586: if(( unsigned int) _temp1574 >  2u?*((
int*) _temp1574) ==  Cyc_Absyn_Char_p: 0){ _LL1624: _temp1623=(( struct Cyc_Absyn_Char_p_struct*)
_temp1574)->f1; goto _LL1587;} else{ goto _LL1588;} _LL1588: if(( unsigned int)
_temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_Float_p: 0){ _LL1626:
_temp1625=(( struct Cyc_Absyn_Float_p_struct*) _temp1574)->f1; goto _LL1589;}
else{ goto _LL1590;} _LL1590: if(( unsigned int) _temp1574 >  2u?*(( int*)
_temp1574) ==  Cyc_Absyn_Enum_p: 0){ _LL1630: _temp1629=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1574)->f1; goto _LL1628; _LL1628: _temp1627=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1574)->f2; goto _LL1591;} else{ goto _LL1592;} _LL1592: if(( unsigned int)
_temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1634:
_temp1633=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1574)->f1; goto
_LL1632; _LL1632: _temp1631=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1574)->f2;
goto _LL1593;} else{ goto _LL1594;} _LL1594: if(( unsigned int) _temp1574 >  2u?*((
int*) _temp1574) ==  Cyc_Absyn_Tunion_p: 0){ _LL1642: _temp1641=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1574)->f1; goto _LL1640; _LL1640: _temp1639=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1574)->f2; goto _LL1638; _LL1638: _temp1637=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1574)->f3; goto _LL1636; _LL1636: _temp1635=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1574)->f4; if( _temp1635 ==  0){ goto _LL1595;} else{ goto _LL1596;}} else{
goto _LL1596;} _LL1596: if(( unsigned int) _temp1574 >  2u?*(( int*) _temp1574)
==  Cyc_Absyn_Pointer_p: 0){ _LL1645: _temp1644=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1574)->f1; _temp1646=* _temp1644; _LL1648: _temp1647=( void*) _temp1646.r;
if(( unsigned int) _temp1647 >  2u?*(( int*) _temp1647) ==  Cyc_Absyn_Tunion_p:
0){ _LL1656: _temp1655=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1647)->f1;
goto _LL1654; _LL1654: _temp1653=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1647)->f2;
goto _LL1652; _LL1652: _temp1651=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1647)->f3;
goto _LL1650; _LL1650: _temp1649=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1647)->f4;
goto _LL1643;} else{ goto _LL1598;}} else{ goto _LL1598;} _LL1643: if( _temp1649
!=  0){ goto _LL1597;} else{ goto _LL1598;} _LL1598: if(( unsigned int)
_temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_Tunion_p: 0){ _LL1658:
_temp1657=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1574)->f4; goto _LL1599;}
else{ goto _LL1600;} _LL1600: if(( unsigned int) _temp1574 >  2u?*(( int*)
_temp1574) ==  Cyc_Absyn_Tuple_p: 0){ _LL1660: _temp1659=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1574)->f1; goto _LL1601;} else{ goto _LL1602;} _LL1602: if(( unsigned int)
_temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_Struct_p: 0){ _LL1668:
_temp1667=(( struct Cyc_Absyn_Struct_p_struct*) _temp1574)->f1; goto _LL1666;
_LL1666: _temp1665=(( struct Cyc_Absyn_Struct_p_struct*) _temp1574)->f2; goto
_LL1664; _LL1664: _temp1663=(( struct Cyc_Absyn_Struct_p_struct*) _temp1574)->f3;
goto _LL1662; _LL1662: _temp1661=(( struct Cyc_Absyn_Struct_p_struct*) _temp1574)->f4;
goto _LL1603;} else{ goto _LL1604;} _LL1604: if(( unsigned int) _temp1574 >  2u?*((
int*) _temp1574) ==  Cyc_Absyn_Pointer_p: 0){ _LL1670: _temp1669=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1574)->f1; goto _LL1605;} else{ goto _LL1606;} _LL1606: if(( unsigned int)
_temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1607;}
else{ goto _LL1608;} _LL1608: if(( unsigned int) _temp1574 >  2u?*(( int*)
_temp1574) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1609;} else{ goto _LL1610;}
_LL1610: if(( unsigned int) _temp1574 >  2u?*(( int*) _temp1574) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1611;} else{ goto _LL1575;} _LL1577: nv= Cyc_Toc_add_varmap( nv,
_temp1615, r); goto _LL1579; _LL1579: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1575; _LL1581: { struct _tuple0* _temp1671= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1672=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1672->hd=( void*)({ struct _tuple12*
_temp1673=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1673->f1= _temp1671; _temp1673->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1673;}); _temp1672->tl= decls; _temp1672;}); nv= Cyc_Toc_add_varmap(
nv, _temp1617->name, Cyc_Absyn_var_exp( _temp1671, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1671, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1575;} _LL1583: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1575; _LL1585:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1621, _temp1619, 0),
succ_lab, fail_lab); goto _LL1575; _LL1587: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1623, 0), succ_lab, fail_lab); goto _LL1575; _LL1589: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1625, 0), succ_lab, fail_lab); goto _LL1575;
_LL1591: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1674=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1674[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1675; _temp1675.tag= Cyc_Absyn_Enum_e;
_temp1675.f1= _temp1627->name; _temp1675.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1629; _temp1675.f3=( struct Cyc_Absyn_Enumfield*) _temp1627; _temp1675;});
_temp1674;}), 0), succ_lab, fail_lab); goto _LL1575; _LL1593: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_AnonEnum_e_struct* _temp1676=(
struct Cyc_Absyn_AnonEnum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_e_struct));
_temp1676[ 0]=({ struct Cyc_Absyn_AnonEnum_e_struct _temp1677; _temp1677.tag=
Cyc_Absyn_AnonEnum_e; _temp1677.f1= _temp1631->name; _temp1677.f2=( void*)
_temp1633; _temp1677.f3=( struct Cyc_Absyn_Enumfield*) _temp1631; _temp1677;});
_temp1676;}), 0), succ_lab, fail_lab); goto _LL1575; _LL1595: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1639->name, 0); if( ! _temp1641->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1575;} _LL1597: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1649);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1653->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1678=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1653->typs); struct Cyc_List_List*
_temp1679=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1649); for( 0; _temp1679 !=  0;((( _temp1679=((
struct Cyc_List_List*) _check_null( _temp1679))->tl, _temp1678=(( struct Cyc_List_List*)
_check_null( _temp1678))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1680=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1679))->hd;
if(( void*) _temp1680->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1681=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1678))->hd)).f2;
struct _tuple0* _temp1682= Cyc_Toc_temp_var(); void* _temp1683=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1680->topt))->v; void* _temp1684= Cyc_Toc_typ_to_c(
_temp1683); struct _tagged_arr* _temp1685= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1686= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1681))){ _temp1686= Cyc_Absyn_cast_exp(
_temp1684, _temp1686, 0);} decls=({ struct Cyc_List_List* _temp1687=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1687->hd=( void*)({
struct _tuple12* _temp1688=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1688->f1= _temp1682; _temp1688->f2= _temp1684; _temp1688;});
_temp1687->tl= decls; _temp1687;});{ struct _tuple11 _temp1689= Cyc_Toc_xlate_pat(
nv, rgn, _temp1683, Cyc_Absyn_var_exp( _temp1682, 0), _temp1686, _temp1680,
succ_lab, fail_lab, decls); nv= _temp1689.f1; decls= _temp1689.f2;{ struct Cyc_Absyn_Stmt*
_temp1690= _temp1689.f3; struct Cyc_Absyn_Stmt* _temp1691= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1682, 0), _temp1686, 0); struct Cyc_Absyn_Stmt*
_temp1692= Cyc_Absyn_seq_stmt( _temp1691, _temp1690, 0); ss=({ struct Cyc_List_List*
_temp1693=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1693->hd=( void*) Cyc_Absyn_label_stmt( _temp1685, _temp1692, 0); _temp1693->tl=
ss; _temp1693;}); succ_lab= _temp1685;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1694=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1694->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1694->tl= 0;
_temp1694;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1655->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1653->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1695=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1655->fields))->v; for( 0; _temp1695 !=  0; _temp1695=((
struct Cyc_List_List*) _check_null( _temp1695))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1696=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1695))->hd; if( _temp1696->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1653->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1575;}} _LL1599: _temp1659= _temp1657; goto _LL1601; _LL1601: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1659);{ struct Cyc_List_List* _temp1697=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1659); for(
0; _temp1697 !=  0;( _temp1697=(( struct Cyc_List_List*) _check_null( _temp1697))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1698=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1697))->hd; if(( void*) _temp1698->r == ( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1699= Cyc_Toc_temp_var(); void* _temp1700=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1698->topt))->v; struct
_tagged_arr* _temp1701= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1702=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1702->hd=( void*)({ struct _tuple12* _temp1703=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1703->f1= _temp1699;
_temp1703->f2= Cyc_Toc_typ_to_c( _temp1700); _temp1703;}); _temp1702->tl= decls;
_temp1702;});{ struct _tuple11 _temp1704= Cyc_Toc_xlate_pat( nv, rgn, _temp1700,
Cyc_Absyn_var_exp( _temp1699, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1698, succ_lab, fail_lab, decls); nv= _temp1704.f1; decls=
_temp1704.f2;{ struct Cyc_Absyn_Stmt* _temp1705= _temp1704.f3; struct Cyc_Absyn_Stmt*
_temp1706= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1699, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp1705, 0); ss=({ struct Cyc_List_List* _temp1707=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1707->hd=( void*) Cyc_Absyn_label_stmt(
_temp1701, _temp1706, 0); _temp1707->tl= ss; _temp1707;}); succ_lab= _temp1701;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1708=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1708->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1708->tl= 0; _temp1708;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1575;} _LL1603: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1709=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1661); for( 0; _temp1709 !=  0; _temp1709=((
struct Cyc_List_List*) _check_null( _temp1709))->tl){ struct _tuple13* _temp1710=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1709))->hd; struct
Cyc_Absyn_Pat* _temp1711=(* _temp1710).f2; if(( void*) _temp1711->r == ( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1712=( void*)((
struct Cyc_List_List*) _check_null((* _temp1710).f1))->hd; struct _tagged_arr*
_temp1718; _LL1714: if(*(( int*) _temp1712) ==  Cyc_Absyn_FieldName){ _LL1719:
_temp1718=(( struct Cyc_Absyn_FieldName_struct*) _temp1712)->f1; goto _LL1715;}
else{ goto _LL1716;} _LL1716: goto _LL1717; _LL1715: f= _temp1718; goto _LL1713;
_LL1717:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1713:;}{ struct _tuple0*
_temp1720= Cyc_Toc_temp_var(); void* _temp1721=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1711->topt))->v; void* _temp1722= Cyc_Toc_typ_to_c( _temp1721);
struct _tagged_arr* _temp1723= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1724=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1724->hd=( void*)({ struct _tuple12* _temp1725=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1725->f1= _temp1720;
_temp1725->f2= _temp1722; _temp1725;}); _temp1724->tl= decls; _temp1724;});{
struct _tuple11 _temp1726= Cyc_Toc_xlate_pat( nv, rgn, _temp1721, Cyc_Absyn_var_exp(
_temp1720, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1711, succ_lab,
fail_lab, decls); nv= _temp1726.f1; decls= _temp1726.f2;{ struct Cyc_Absyn_Exp*
_temp1727= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1667->fields))->v, f)))->type)){
_temp1727= Cyc_Absyn_cast_exp( _temp1722, _temp1727, 0);}{ struct Cyc_Absyn_Stmt*
_temp1728= _temp1726.f3; struct Cyc_Absyn_Stmt* _temp1729= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1720, 0), _temp1727, 0),
_temp1728, 0); ss=({ struct Cyc_List_List* _temp1730=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1730->hd=( void*) Cyc_Absyn_label_stmt(
_temp1723, _temp1729, 0); _temp1730->tl= ss; _temp1730;}); succ_lab= _temp1723;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1731=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1731->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1731->tl= 0; _temp1731;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1575;} _LL1605: { struct _tuple0* _temp1732= Cyc_Toc_temp_var(); void*
_temp1733=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1669->topt))->v;
decls=({ struct Cyc_List_List* _temp1734=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1734->hd=( void*)({ struct _tuple12*
_temp1735=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1735->f1= _temp1732; _temp1735->f2= Cyc_Toc_typ_to_c( _temp1733); _temp1735;});
_temp1734->tl= decls; _temp1734;});{ struct _tuple11 _temp1736= Cyc_Toc_xlate_pat(
nv, rgn, _temp1733, Cyc_Absyn_var_exp( _temp1732, 0), Cyc_Absyn_deref_exp( path,
0), _temp1669, succ_lab, fail_lab, decls); nv= _temp1736.f1; decls= _temp1736.f2;{
struct Cyc_Absyn_Stmt* _temp1737= _temp1736.f3; struct Cyc_Absyn_Stmt* _temp1738=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1732, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1737, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1738, 0);} else{ s= _temp1738;} goto _LL1575;}}} _LL1607: s=({
void* _temp1739[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1739, sizeof( void*), 0u));}); goto _LL1575;
_LL1609: s=({ void* _temp1740[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1740, sizeof( void*), 0u));}); goto
_LL1575; _LL1611: s=({ void* _temp1741[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1741, sizeof( void*), 0u));}); goto
_LL1575; _LL1575:;} return({ struct _tuple11 _temp1742; _temp1742.f1= nv;
_temp1742.f2= decls; _temp1742.f3= s; _temp1742;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1743=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1743->f1= Cyc_Toc_fresh_label(); _temp1743->f2=
Cyc_Toc_fresh_label(); _temp1743->f3= sc; _temp1743;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1744=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1745= Cyc_Tcutil_compress( _temp1744); _LL1747: if(( unsigned int)
_temp1745 >  4u?*(( int*) _temp1745) ==  Cyc_Absyn_IntType: 0){ goto _LL1748;}
else{ goto _LL1749;} _LL1749: if(( unsigned int) _temp1745 >  4u?*(( int*)
_temp1745) ==  Cyc_Absyn_EnumType: 0){ goto _LL1750;} else{ goto _LL1751;}
_LL1751: goto _LL1752; _LL1748: goto _LL1750; _LL1750: leave_as_switch= 1; goto
_LL1746; _LL1752: leave_as_switch= 0; goto _LL1746; _LL1746:;}{ struct Cyc_List_List*
_temp1753= scs; for( 0; _temp1753 !=  0; _temp1753=(( struct Cyc_List_List*)
_check_null( _temp1753))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1753))->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1753))->hd)->where_clause !=  0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1754= scs; for( 0; _temp1754
!=  0; _temp1754=(( struct Cyc_List_List*) _check_null( _temp1754))->tl){ struct
Cyc_Absyn_Stmt* _temp1755=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1754))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1754))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1755, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1755);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1756= _new_region(); struct _RegionHandle* rgn=&
_temp1756; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1757= lscs; for( 0;
_temp1757 !=  0; _temp1757=(( struct Cyc_List_List*) _check_null( _temp1757))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1757))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1757))->tl ==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1757))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1757))->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11
_temp1758= Cyc_Toc_xlate_pat( nv, rgn, _temp1744, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1759=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1759->hd=( void*)
_temp1758.f1; _temp1759->tl= nvs; _temp1759;}); decls= _temp1758.f2; test_stmts=({
struct Cyc_List_List* _temp1760=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1760->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1757))->hd)).f1,
_temp1758.f3, 0); _temp1760->tl= test_stmts; _temp1760;});} else{ struct Cyc_Absyn_Exp*
_temp1761=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1762= Cyc_Toc_fresh_label(); struct _tuple11 _temp1763= Cyc_Toc_xlate_pat(
nv, rgn, _temp1744, r, path, sc->pattern, _temp1762, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1763.f1, _temp1761); nvs=({ struct Cyc_List_List* _temp1764=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1764->hd=( void*)
_temp1763.f1; _temp1764->tl= nvs; _temp1764;}); decls= _temp1763.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1761, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1762, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1765=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1765->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1757))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1763.f3, s3, 0), 0); _temp1765->tl= test_stmts;
_temp1765;});}}}}{ struct Cyc_Absyn_Stmt* _temp1766= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs !=  0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=  0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1767=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1767, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1768=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1768, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1766, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls !=  0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1771; void* _temp1772; struct
_tuple0* _temp1774; struct _tuple12* _temp1769=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1771=* _temp1769; _LL1775: _temp1774= _temp1771.f1;
goto _LL1773; _LL1773: _temp1772= _temp1771.f2; goto _LL1770; _LL1770: res= Cyc_Absyn_declare_stmt(
_temp1774, _temp1772, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1776=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1776[ 0]=({ struct Cyc_List_List
_temp1777; _temp1777.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1777.tl= 0; _temp1777;}); _temp1776;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1778=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1820; struct Cyc_Absyn_Stmt* _temp1822; struct Cyc_Absyn_Stmt* _temp1824;
struct Cyc_Absyn_Exp* _temp1826; struct Cyc_Absyn_Stmt* _temp1828; struct Cyc_Absyn_Stmt*
_temp1830; struct Cyc_Absyn_Exp* _temp1832; struct Cyc_Absyn_Stmt* _temp1834;
struct _tuple2 _temp1836; struct Cyc_Absyn_Exp* _temp1838; struct Cyc_Absyn_Stmt*
_temp1840; struct Cyc_Absyn_Stmt* _temp1842; struct Cyc_Absyn_Stmt* _temp1844;
struct Cyc_Absyn_Stmt* _temp1846; struct _tuple2 _temp1848; struct Cyc_Absyn_Exp*
_temp1850; struct _tuple2 _temp1852; struct Cyc_Absyn_Exp* _temp1854; struct Cyc_Absyn_Exp*
_temp1856; struct Cyc_List_List* _temp1858; struct Cyc_Absyn_Exp* _temp1860;
struct Cyc_Absyn_Switch_clause** _temp1862; struct Cyc_List_List* _temp1864;
struct Cyc_Absyn_Stmt* _temp1866; struct Cyc_Absyn_Decl* _temp1868; struct Cyc_Absyn_Stmt*
_temp1870; struct _tagged_arr* _temp1872; struct _tuple2 _temp1874; struct Cyc_Absyn_Exp*
_temp1876; struct Cyc_Absyn_Stmt* _temp1878; struct Cyc_List_List* _temp1880;
struct Cyc_Absyn_Stmt* _temp1882; struct Cyc_Absyn_Stmt* _temp1884; struct Cyc_Absyn_Vardecl*
_temp1886; struct Cyc_Absyn_Tvar* _temp1888; _LL1780: if( _temp1778 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1781;} else{ goto _LL1782;} _LL1782: if((
unsigned int) _temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Exp_s: 0){
_LL1821: _temp1820=(( struct Cyc_Absyn_Exp_s_struct*) _temp1778)->f1; goto
_LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int) _temp1778 >  1u?*((
int*) _temp1778) ==  Cyc_Absyn_Seq_s: 0){ _LL1825: _temp1824=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1778)->f1; goto _LL1823; _LL1823: _temp1822=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1778)->f2; goto _LL1785;} else{ goto _LL1786;} _LL1786: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Return_s: 0){ _LL1827:
_temp1826=(( struct Cyc_Absyn_Return_s_struct*) _temp1778)->f1; goto _LL1787;}
else{ goto _LL1788;} _LL1788: if(( unsigned int) _temp1778 >  1u?*(( int*)
_temp1778) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1833: _temp1832=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1778)->f1; goto _LL1831; _LL1831: _temp1830=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1778)->f2; goto _LL1829; _LL1829: _temp1828=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1778)->f3; goto _LL1789;} else{ goto _LL1790;} _LL1790: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_While_s: 0){ _LL1837:
_temp1836=(( struct Cyc_Absyn_While_s_struct*) _temp1778)->f1; _LL1839:
_temp1838= _temp1836.f1; goto _LL1835; _LL1835: _temp1834=(( struct Cyc_Absyn_While_s_struct*)
_temp1778)->f2; goto _LL1791;} else{ goto _LL1792;} _LL1792: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Break_s: 0){ _LL1841:
_temp1840=(( struct Cyc_Absyn_Break_s_struct*) _temp1778)->f1; goto _LL1793;}
else{ goto _LL1794;} _LL1794: if(( unsigned int) _temp1778 >  1u?*(( int*)
_temp1778) ==  Cyc_Absyn_Continue_s: 0){ _LL1843: _temp1842=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1778)->f1; goto _LL1795;} else{ goto _LL1796;} _LL1796: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Goto_s: 0){ _LL1845:
_temp1844=(( struct Cyc_Absyn_Goto_s_struct*) _temp1778)->f2; goto _LL1797;}
else{ goto _LL1798;} _LL1798: if(( unsigned int) _temp1778 >  1u?*(( int*)
_temp1778) ==  Cyc_Absyn_For_s: 0){ _LL1857: _temp1856=(( struct Cyc_Absyn_For_s_struct*)
_temp1778)->f1; goto _LL1853; _LL1853: _temp1852=(( struct Cyc_Absyn_For_s_struct*)
_temp1778)->f2; _LL1855: _temp1854= _temp1852.f1; goto _LL1849; _LL1849:
_temp1848=(( struct Cyc_Absyn_For_s_struct*) _temp1778)->f3; _LL1851: _temp1850=
_temp1848.f1; goto _LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_For_s_struct*)
_temp1778)->f4; goto _LL1799;} else{ goto _LL1800;} _LL1800: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Switch_s: 0){ _LL1861:
_temp1860=(( struct Cyc_Absyn_Switch_s_struct*) _temp1778)->f1; goto _LL1859;
_LL1859: _temp1858=(( struct Cyc_Absyn_Switch_s_struct*) _temp1778)->f2; goto
_LL1801;} else{ goto _LL1802;} _LL1802: if(( unsigned int) _temp1778 >  1u?*((
int*) _temp1778) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1865: _temp1864=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1778)->f1; goto _LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1778)->f2; goto _LL1803;} else{ goto _LL1804;} _LL1804: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Decl_s: 0){ _LL1869:
_temp1868=(( struct Cyc_Absyn_Decl_s_struct*) _temp1778)->f1; goto _LL1867;
_LL1867: _temp1866=(( struct Cyc_Absyn_Decl_s_struct*) _temp1778)->f2; goto
_LL1805;} else{ goto _LL1806;} _LL1806: if(( unsigned int) _temp1778 >  1u?*((
int*) _temp1778) ==  Cyc_Absyn_Label_s: 0){ _LL1873: _temp1872=(( struct Cyc_Absyn_Label_s_struct*)
_temp1778)->f1; goto _LL1871; _LL1871: _temp1870=(( struct Cyc_Absyn_Label_s_struct*)
_temp1778)->f2; goto _LL1807;} else{ goto _LL1808;} _LL1808: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Do_s: 0){ _LL1879: _temp1878=((
struct Cyc_Absyn_Do_s_struct*) _temp1778)->f1; goto _LL1875; _LL1875: _temp1874=((
struct Cyc_Absyn_Do_s_struct*) _temp1778)->f2; _LL1877: _temp1876= _temp1874.f1;
goto _LL1809;} else{ goto _LL1810;} _LL1810: if(( unsigned int) _temp1778 >  1u?*((
int*) _temp1778) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1883: _temp1882=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1778)->f1; goto _LL1881; _LL1881: _temp1880=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1778)->f2; goto _LL1811;} else{ goto _LL1812;} _LL1812: if(( unsigned int)
_temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Region_s: 0){ _LL1889:
_temp1888=(( struct Cyc_Absyn_Region_s_struct*) _temp1778)->f1; goto _LL1887;
_LL1887: _temp1886=(( struct Cyc_Absyn_Region_s_struct*) _temp1778)->f2; goto
_LL1885; _LL1885: _temp1884=(( struct Cyc_Absyn_Region_s_struct*) _temp1778)->f3;
goto _LL1813;} else{ goto _LL1814;} _LL1814: if(( unsigned int) _temp1778 >  1u?*((
int*) _temp1778) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1815;} else{ goto _LL1816;}
_LL1816: if(( unsigned int) _temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1817;} else{ goto _LL1818;} _LL1818: if(( unsigned int) _temp1778 > 
1u?*(( int*) _temp1778) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1819;} else{ goto
_LL1779;} _LL1781: return; _LL1783: Cyc_Toc_exp_to_c( nv, _temp1820); return;
_LL1785: Cyc_Toc_stmt_to_c( nv, _temp1824); s= _temp1822; continue; _LL1787: {
struct Cyc_Core_Opt* topt= 0; if( _temp1826 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1890=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1890->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1826))->topt))->v);
_temp1890;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1826));} if( s->try_depth >  0){ if( topt !=  0){ struct _tuple0* _temp1891=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1892= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1891, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1891,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1826, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1892, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1789: Cyc_Toc_exp_to_c( nv, _temp1832); Cyc_Toc_stmt_to_c( nv,
_temp1830); s= _temp1828; continue; _LL1791: Cyc_Toc_exp_to_c( nv, _temp1838);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1834); return; _LL1793: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1840 ==  0? 0:(( struct Cyc_Absyn_Stmt*) _check_null(
_temp1840))->try_depth; Cyc_Toc_do_npop_before( s->try_depth -  dest_depth, s);
return;} _LL1795: if( nv->continue_lab !=  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
nv->continue_lab))->v, 0))->r));} _temp1844= _temp1842; goto _LL1797; _LL1797:
Cyc_Toc_do_npop_before( s->try_depth - (( struct Cyc_Absyn_Stmt*) _check_null(
_temp1844))->try_depth, s); return; _LL1799: Cyc_Toc_exp_to_c( nv, _temp1856);
Cyc_Toc_exp_to_c( nv, _temp1854); Cyc_Toc_exp_to_c( nv, _temp1850); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1846); return; _LL1801: Cyc_Toc_xlate_switch( nv, s,
_temp1860, _temp1858); return; _LL1803: if( nv->fallthru_info ==  0){( int)
_throw(({ void* _temp1893[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u), _tag_arr( _temp1893, sizeof( void*), 0u));}));}{
struct _tuple6 _temp1896; struct Cyc_Dict_Dict* _temp1897; struct Cyc_List_List*
_temp1899; struct _tagged_arr* _temp1901; struct _tuple6* _temp1894=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp1896=*
_temp1894; _LL1902: _temp1901= _temp1896.f1; goto _LL1900; _LL1900: _temp1899=
_temp1896.f2; goto _LL1898; _LL1898: _temp1897= _temp1896.f3; goto _LL1895;
_LL1895: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1901, 0); Cyc_Toc_do_npop_before(
s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp1862)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp1903=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1899); struct Cyc_List_List* _temp1904=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1864); for( 0; _temp1903 !=  0;(
_temp1903=(( struct Cyc_List_List*) _check_null( _temp1903))->tl, _temp1904=((
struct Cyc_List_List*) _check_null( _temp1904))->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1904))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp1897,(
struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp1903))->hd),( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1904))->hd, 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1805:{ void* _temp1905=(
void*) _temp1868->r; struct Cyc_Absyn_Vardecl* _temp1915; int _temp1917; struct
Cyc_Absyn_Exp* _temp1919; struct Cyc_Core_Opt* _temp1921; struct Cyc_Absyn_Pat*
_temp1923; struct Cyc_List_List* _temp1925; _LL1907: if(*(( int*) _temp1905) == 
Cyc_Absyn_Var_d){ _LL1916: _temp1915=(( struct Cyc_Absyn_Var_d_struct*)
_temp1905)->f1; goto _LL1908;} else{ goto _LL1909;} _LL1909: if(*(( int*)
_temp1905) ==  Cyc_Absyn_Let_d){ _LL1924: _temp1923=(( struct Cyc_Absyn_Let_d_struct*)
_temp1905)->f1; goto _LL1922; _LL1922: _temp1921=(( struct Cyc_Absyn_Let_d_struct*)
_temp1905)->f3; goto _LL1920; _LL1920: _temp1919=(( struct Cyc_Absyn_Let_d_struct*)
_temp1905)->f4; goto _LL1918; _LL1918: _temp1917=(( struct Cyc_Absyn_Let_d_struct*)
_temp1905)->f5; goto _LL1910;} else{ goto _LL1911;} _LL1911: if(*(( int*)
_temp1905) ==  Cyc_Absyn_Letv_d){ _LL1926: _temp1925=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1905)->f1; goto _LL1912;} else{ goto _LL1913;} _LL1913: goto _LL1914;
_LL1908: { struct Cyc_Toc_Env* _temp1927= Cyc_Toc_add_varmap( nv, _temp1915->name,
Cyc_Absyn_varb_exp( _temp1915->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1928=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1928[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1929; _temp1929.tag= Cyc_Absyn_Local_b;
_temp1929.f1= _temp1915; _temp1929;}); _temp1928;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1927, _temp1927, _temp1915, _temp1866); goto _LL1906;} _LL1910:{ void*
_temp1930=( void*) _temp1923->r; struct Cyc_Absyn_Vardecl* _temp1936; _LL1932:
if(( unsigned int) _temp1930 >  2u?*(( int*) _temp1930) ==  Cyc_Absyn_Var_p: 0){
_LL1937: _temp1936=(( struct Cyc_Absyn_Var_p_struct*) _temp1930)->f1; goto
_LL1933;} else{ goto _LL1934;} _LL1934: goto _LL1935; _LL1933: { struct _tuple0*
old_name= _temp1936->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1936->name= new_name; _temp1936->initializer=( struct Cyc_Absyn_Exp*)
_temp1919;( void*)( _temp1868->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1938=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1938[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1939; _temp1939.tag= Cyc_Absyn_Var_d;
_temp1939.f1= _temp1936; _temp1939;}); _temp1938;})));{ struct Cyc_Toc_Env*
_temp1940= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1941=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1941[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1942; _temp1942.tag= Cyc_Absyn_Local_b; _temp1942.f1=
_temp1936; _temp1942;}); _temp1941;}), 0)); Cyc_Toc_local_decl_to_c( _temp1940,
nv, _temp1936, _temp1866); goto _LL1931;}} _LL1935:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1923, _temp1921, _temp1919, _temp1917,
_temp1866))->r)); goto _LL1931; _LL1931:;} goto _LL1906; _LL1912: { struct Cyc_List_List*
_temp1943=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1925); if( _temp1943 ==  0){({ void* _temp1944[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1944, sizeof( void*), 0u));});}(
void*)( _temp1868->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1945=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1945[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1946; _temp1946.tag= Cyc_Absyn_Var_d;
_temp1946.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1943))->hd; _temp1946;}); _temp1945;}))); _temp1943=(( struct Cyc_List_List*)
_check_null( _temp1943))->tl; for( 0; _temp1943 !=  0; _temp1943=(( struct Cyc_List_List*)
_check_null( _temp1943))->tl){ struct Cyc_Absyn_Decl* _temp1947= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1948=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1948[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1949; _temp1949.tag= Cyc_Absyn_Var_d; _temp1949.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1943))->hd; _temp1949;}); _temp1948;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1947, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1906;} _LL1914:({
void* _temp1950[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1950, sizeof( void*), 0u));}); goto _LL1906;
_LL1906:;} return; _LL1807: s= _temp1870; continue; _LL1809: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1878); Cyc_Toc_exp_to_c( nv, _temp1876); return;
_LL1811: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1951=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1951->v=( void*) e_typ; _temp1951;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1882);{
struct Cyc_Absyn_Stmt* _temp1952= Cyc_Absyn_seq_stmt( _temp1882, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1953= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1954= Cyc_Absyn_var_exp(
_temp1953, 0); struct Cyc_Absyn_Vardecl* _temp1955= Cyc_Absyn_new_vardecl(
_temp1953, Cyc_Absyn_exn_typ, 0); _temp1954->topt=({ struct Cyc_Core_Opt*
_temp1956=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1956->v=( void*) Cyc_Absyn_exn_typ; _temp1956;});{ struct Cyc_Absyn_Pat*
_temp1957=({ struct Cyc_Absyn_Pat* _temp1973=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1973->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1975=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1975[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1976; _temp1976.tag= Cyc_Absyn_Var_p;
_temp1976.f1= _temp1955; _temp1976;}); _temp1975;})); _temp1973->topt=({ struct
Cyc_Core_Opt* _temp1974=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1974->v=( void*) Cyc_Absyn_exn_typ; _temp1974;}); _temp1973->loc= 0;
_temp1973;}); struct Cyc_Absyn_Exp* _temp1958= Cyc_Absyn_throw_exp( _temp1954, 0);
_temp1958->topt=({ struct Cyc_Core_Opt* _temp1959=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1959->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1959;});{ struct Cyc_Absyn_Stmt* _temp1960= Cyc_Absyn_exp_stmt( _temp1958,
0); struct Cyc_Absyn_Switch_clause* _temp1961=({ struct Cyc_Absyn_Switch_clause*
_temp1970=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1970->pattern= _temp1957; _temp1970->pat_vars=({ struct Cyc_Core_Opt*
_temp1971=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1971->v=( void*)({ struct Cyc_List_List* _temp1972=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1972->hd=( void*) _temp1955;
_temp1972->tl= 0; _temp1972;}); _temp1971;}); _temp1970->where_clause= 0;
_temp1970->body= _temp1960; _temp1970->loc= 0; _temp1970;}); struct Cyc_Absyn_Stmt*
_temp1962= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1880,({ struct Cyc_List_List*
_temp1969=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1969->hd=( void*) _temp1961; _temp1969->tl= 0; _temp1969;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1962);{ struct Cyc_Absyn_Exp* _temp1963= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1968=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1968->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1968->tl= 0; _temp1968;}), 0); struct Cyc_Absyn_Stmt*
_temp1964= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1967=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1967->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1967->tl= 0; _temp1967;}), 0), 0); struct Cyc_Absyn_Exp* _temp1965= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1966= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1964, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1965, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1963, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1966, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1952, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1962, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1813: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1886->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1884);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_Absyn_Exp* _temp1977[ 1u]; _temp1977[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1977, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), Cyc_Absyn_seq_stmt(
_temp1884, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp* _temp1978[ 1u]; _temp1978[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1978, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), 0), 0), 0), 0))->r)); return;} _LL1815:({ void* _temp1979[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("switchC_s", sizeof( unsigned char), 10u), _tag_arr( _temp1979, sizeof(
void*), 0u));}); return; _LL1817:({ void* _temp1980[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof(
unsigned char), 4u), _tag_arr( _temp1980, sizeof( void*), 0u));}); return;
_LL1819:({ void* _temp1981[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u),
_tag_arr( _temp1981, sizeof( void*), 0u));}); return; _LL1779:;}} struct
_tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp1982= f->args; for( 0;
_temp1982 !=  0; _temp1982=(( struct Cyc_List_List*) _check_null( _temp1982))->tl){(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1982))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1982))->hd)).f3);{ struct _tuple0* _temp1983=({ struct _tuple0* _temp1984=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp1984->f1=( void*) Cyc_Absyn_Loc_n;
_temp1984->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1982))->hd)).f1; _temp1984;}); nv= Cyc_Toc_add_varmap( nv, _temp1983, Cyc_Absyn_var_exp(
_temp1983, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp1987; void* _temp1989;
struct Cyc_Absyn_Tqual _temp1991; struct Cyc_Core_Opt* _temp1993; struct Cyc_Absyn_VarargInfo
_temp1985=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL1994: _temp1993= _temp1985.name; goto _LL1992; _LL1992: _temp1991= _temp1985.tq;
goto _LL1990; _LL1990: _temp1989=( void*) _temp1985.type; goto _LL1988; _LL1988:
_temp1987= _temp1985.inject; goto _LL1986; _LL1986: { void* _temp1995= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp1989,( void*) Cyc_Absyn_HeapRgn, _temp1991)); struct
_tuple0* _temp1996=({ struct _tuple0* _temp1999=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp1999->f1=( void*) Cyc_Absyn_Loc_n; _temp1999->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp1993))->v;
_temp1999;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp1997=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1997->hd=( void*)({ struct _tuple15* _temp1998=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp1998->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp1993))->v; _temp1998->f2= _temp1991;
_temp1998->f3= _temp1995; _temp1998;}); _temp1997->tl= 0; _temp1997;})); nv= Cyc_Toc_add_varmap(
nv, _temp1996, Cyc_Absyn_var_exp( _temp1996, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp2000=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2000 !=  0; _temp2000=((
struct Cyc_List_List*) _check_null( _temp2000))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2000))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2000))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2001= s; _LL2003: if(
_temp2001 == ( void*) Cyc_Absyn_Abstract){ goto _LL2004;} else{ goto _LL2005;}
_LL2005: if( _temp2001 == ( void*) Cyc_Absyn_ExternC){ goto _LL2006;} else{ goto
_LL2007;} _LL2007: goto _LL2008; _LL2004: return( void*) Cyc_Absyn_Public;
_LL2006: return( void*) Cyc_Absyn_Extern; _LL2008: return s; _LL2002:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2009=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2010=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2010->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2010;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2011=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2009).f2); if( _temp2011 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2009).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2011))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Structdecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2009).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2012=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2012->v=(
void*) _temp2009; _temp2012;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2013=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2013 !=  0; _temp2013=(( struct Cyc_List_List*)
_check_null( _temp2013))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2013))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2013))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2014=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2015=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2015->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp2015;});}{ int
seen_defn_before; struct Cyc_Core_Opt* _temp2016=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)(( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2014).f2); if( _temp2016 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2014).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2016))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Uniondecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2014).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2017=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2017->v=(
void*) _temp2014; _temp2017;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2018=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2018 !=  0; _temp2018=(( struct Cyc_List_List*)
_check_null( _temp2018))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2018))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2018))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2019=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2019->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2019;});}{ struct
_tuple0* _temp2020= tud->name; if( tud->fields ==  0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2020)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2020));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2021=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2021 !=  0; _temp2021=(( struct Cyc_List_List*) _check_null(
_temp2021))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2021))->hd; if( f->typs ==  0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2022; _temp2022.q_const= 1;
_temp2022.q_volatile= 0; _temp2022.q_restrict= 0; _temp2022;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2023=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2023->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2024=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2024[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2025; _temp2025.tag= Cyc_Absyn_Var_d; _temp2025.f1= vd; _temp2025;});
_temp2024;}), 0); _temp2023->tl= Cyc_Toc_result_decls; _temp2023;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2026; _temp2026.q_const=
1; _temp2026.q_volatile= 0; _temp2026.q_restrict= 0; _temp2026;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2027=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2027->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2028=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2028[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2029; _temp2029.tag= Cyc_Absyn_Var_d; _temp2029.f1= vd; _temp2029;});
_temp2028;}), 0); _temp2027->tl= Cyc_Toc_result_decls; _temp2027;});{ struct Cyc_List_List*
_temp2030= 0; int i= 1;{ struct Cyc_List_List* _temp2031= f->typs; for( 0;
_temp2031 !=  0;( _temp2031=(( struct Cyc_List_List*) _check_null( _temp2031))->tl,
i ++)){ struct _tagged_arr* _temp2032= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2033=({ struct Cyc_Absyn_Structfield* _temp2035=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2035->name= _temp2032;
_temp2035->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2031))->hd)).f1; _temp2035->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2031))->hd)).f2);
_temp2035->width= 0; _temp2035->attributes= 0; _temp2035;}); _temp2030=({ struct
Cyc_List_List* _temp2034=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2034->hd=( void*) _temp2033; _temp2034->tl= _temp2030; _temp2034;});}}
_temp2030=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2030); _temp2030=({ struct Cyc_List_List* _temp2036=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2036->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2037=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2037->name= Cyc_Toc_tag_sp; _temp2037->tq= Cyc_Toc_mt_tq; _temp2037->type=(
void*) Cyc_Absyn_sint_t; _temp2037->width= 0; _temp2037->attributes= 0;
_temp2037;}); _temp2036->tl= _temp2030; _temp2036;});{ struct Cyc_Absyn_Structdecl*
_temp2038=({ struct Cyc_Absyn_Structdecl* _temp2042=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2042->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2042->name=({ struct Cyc_Core_Opt* _temp2044=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2044->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2044;}); _temp2042->tvs= 0; _temp2042->fields=({ struct Cyc_Core_Opt*
_temp2043=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2043->v=( void*) _temp2030; _temp2043;}); _temp2042->attributes= 0;
_temp2042;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2039=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2039->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2040=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2040[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2041; _temp2041.tag= Cyc_Absyn_Struct_d;
_temp2041.f1= _temp2038; _temp2041;}); _temp2040;}), 0); _temp2039->tl= Cyc_Toc_result_decls;
_temp2039;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp2045=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2045->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2045;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp2046= xd->name; struct Cyc_List_List*
_temp2047=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2047 !=  0; _temp2047=(( struct Cyc_List_List*) _check_null(
_temp2047))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2047))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2048= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2049=({ struct
Cyc_Absyn_ArrayType_struct* _temp2099=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2099[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2100; _temp2100.tag= Cyc_Absyn_ArrayType;
_temp2100.f1=( void*) Cyc_Absyn_uchar_t; _temp2100.f2= Cyc_Toc_mt_tq; _temp2100.f3=(
struct Cyc_Absyn_Exp*) _temp2048; _temp2100;}); _temp2099;}); struct Cyc_Core_Opt*
_temp2050=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2058; int _temp2059; _LL2052: if( _temp2050 ==  0){ goto _LL2053;} else{
goto _LL2054;} _LL2054: if( _temp2050 ==  0){ goto _LL2056;} else{ _temp2058=*
_temp2050; _LL2060: _temp2059=( int) _temp2058.v; if( _temp2059 ==  0){ goto
_LL2055;} else{ goto _LL2056;}} _LL2056: goto _LL2057; _LL2053: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2066; _temp2066.tag= Cyc_Std_String_pa; _temp2066.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp2065; _temp2065.tag= Cyc_Std_Int_pa;
_temp2065.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2064;
_temp2064.tag= Cyc_Std_Int_pa; _temp2064.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp2063; _temp2063.tag= Cyc_Std_Int_pa; _temp2063.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2062; _temp2062.tag=
Cyc_Std_Int_pa; _temp2062.f1=( int)(( unsigned int) 0);{ void* _temp2061[ 5u]={&
_temp2062,& _temp2063,& _temp2064,& _temp2065,& _temp2066}; Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u), _tag_arr( _temp2061,
sizeof( void*), 5u));}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl* _temp2067= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2049, initopt);( void*)( _temp2067->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2068=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2068->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2069=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2069[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2070; _temp2070.tag= Cyc_Absyn_Var_d; _temp2070.f1= _temp2067; _temp2070;});
_temp2069;}), 0); _temp2068->tl= Cyc_Toc_result_decls; _temp2068;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2071= f->typs; for( 0; _temp2071 !=  0;( _temp2071=(( struct Cyc_List_List*)
_check_null( _temp2071))->tl, i ++)){ struct _tagged_arr* _temp2072=({ struct
_tagged_arr* _temp2076=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2076[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp2078; _temp2078.tag= Cyc_Std_Int_pa; _temp2078.f1=( int)(( unsigned int) i);{
void* _temp2077[ 1u]={& _temp2078}; Cyc_Std_aprintf( _tag_arr("f%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2077, sizeof( void*), 1u));}}); _temp2076;});
struct Cyc_Absyn_Structfield* _temp2073=({ struct Cyc_Absyn_Structfield*
_temp2075=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2075->name= _temp2072; _temp2075->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2071))->hd)).f1; _temp2075->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2071))->hd)).f2);
_temp2075->width= 0; _temp2075->attributes= 0; _temp2075;}); fields=({ struct
Cyc_List_List* _temp2074=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2074->hd=( void*) _temp2073; _temp2074->tl= fields; _temp2074;});}} fields=({
struct Cyc_List_List* _temp2079=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2079->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2080=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2080->name= Cyc_Toc_tag_sp; _temp2080->tq= Cyc_Toc_mt_tq; _temp2080->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2080->width=
0; _temp2080->attributes= 0; _temp2080;}); _temp2079->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2079;});{ struct Cyc_Absyn_Structdecl*
_temp2081=({ struct Cyc_Absyn_Structdecl* _temp2085=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2085->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2085->name=({ struct Cyc_Core_Opt* _temp2087=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2087->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2087;}); _temp2085->tvs= 0; _temp2085->fields=({ struct Cyc_Core_Opt*
_temp2086=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2086->v=( void*) fields; _temp2086;}); _temp2085->attributes= 0; _temp2085;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2082=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2082->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2083=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2083[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2084; _temp2084.tag= Cyc_Absyn_Struct_d;
_temp2084.f1= _temp2081; _temp2084;}); _temp2083;}), 0); _temp2082->tl= Cyc_Toc_result_decls;
_temp2082;});}} goto _LL2051;}} _LL2055: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2088= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2098; _temp2098.tag= Cyc_Std_String_pa;
_temp2098.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2097;
_temp2097.tag= Cyc_Std_Int_pa; _temp2097.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp2096; _temp2096.tag= Cyc_Std_Int_pa; _temp2096.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2095; _temp2095.tag=
Cyc_Std_Int_pa; _temp2095.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct
_temp2094; _temp2094.tag= Cyc_Std_Int_pa; _temp2094.f1=( int)(( unsigned int) 0);{
void* _temp2093[ 5u]={& _temp2094,& _temp2095,& _temp2096,& _temp2097,&
_temp2098}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u),
_tag_arr( _temp2093, sizeof( void*), 5u));}}}}}}), 0); struct Cyc_Absyn_Vardecl*
_temp2089= Cyc_Absyn_new_vardecl( f->name,( void*) _temp2049,( struct Cyc_Absyn_Exp*)
_temp2088);( void*)( _temp2089->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2090=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2090->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2091=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2091[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2092; _temp2092.tag= Cyc_Absyn_Var_d; _temp2092.f1= _temp2089; _temp2092;});
_temp2091;}), 0); _temp2090->tl= Cyc_Toc_result_decls; _temp2090;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2051; _LL2057: goto
_LL2051; _LL2051:;}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2101= ed->name;( void*)( ed->sc=(
void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v);}}
static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s){( void*)( vd->type=(
void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c( body_nv, s);
if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2102=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2108; struct Cyc_Absyn_Exp* _temp2110; struct Cyc_Absyn_Vardecl* _temp2112;
_LL2104: if(*(( int*) _temp2102) ==  Cyc_Absyn_Comprehension_e){ _LL2113:
_temp2112=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2102)->f1; goto
_LL2111; _LL2111: _temp2110=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2102)->f2; goto _LL2109; _LL2109: _temp2108=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2102)->f3; goto _LL2105;} else{ goto _LL2106;} _LL2106: goto _LL2107;
_LL2105: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2112, _temp2110, _temp2108, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2103; _LL2107: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2114= Cyc_Toc_copy_env( init_nv);
_temp2114->toplevel= 1; Cyc_Toc_exp_to_c( _temp2114, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2103; _LL2103:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2115= _new_region(); struct _RegionHandle* prgn=& _temp2115;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2118; struct Cyc_List_List*
_temp2120; struct Cyc_Toc_Env* _temp2122; struct _tuple11 _temp2116= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2123: _temp2122= _temp2116.f1; goto _LL2121; _LL2121: _temp2120=
_temp2116.f2; goto _LL2119; _LL2119: _temp2118= _temp2116.f3; goto _LL2117;
_LL2117: Cyc_Toc_stmt_to_c( _temp2122, s);{ struct Cyc_Absyn_Stmt* _temp2124=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2118, _temp2124, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2125= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2126= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2125, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2118,
Cyc_Absyn_seq_stmt( _temp2126, _temp2124, 0), 0), 0);} for( 0; _temp2120 !=  0;
_temp2120=(( struct Cyc_List_List*) _check_null( _temp2120))->tl){ struct
_tuple12 _temp2129; void* _temp2130; struct _tuple0* _temp2132; struct _tuple12*
_temp2127=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2120))->hd;
_temp2129=* _temp2127; _LL2133: _temp2132= _temp2129.f1; goto _LL2131; _LL2131:
_temp2130= _temp2129.f2; goto _LL2128; _LL2128: s= Cyc_Absyn_declare_stmt(
_temp2132, _temp2130, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ void* _temp2134[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u), _tag_arr( _temp2134, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2135=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2161; struct Cyc_Absyn_Fndecl* _temp2163; struct Cyc_Absyn_Structdecl*
_temp2165; struct Cyc_Absyn_Uniondecl* _temp2167; struct Cyc_Absyn_Tuniondecl*
_temp2169; struct Cyc_Absyn_Enumdecl* _temp2171; struct Cyc_Absyn_Typedefdecl*
_temp2173; struct Cyc_List_List* _temp2175; struct Cyc_List_List* _temp2177;
struct Cyc_List_List* _temp2179; _LL2137: if(*(( int*) _temp2135) ==  Cyc_Absyn_Var_d){
_LL2162: _temp2161=(( struct Cyc_Absyn_Var_d_struct*) _temp2135)->f1; goto
_LL2138;} else{ goto _LL2139;} _LL2139: if(*(( int*) _temp2135) ==  Cyc_Absyn_Fn_d){
_LL2164: _temp2163=(( struct Cyc_Absyn_Fn_d_struct*) _temp2135)->f1; goto
_LL2140;} else{ goto _LL2141;} _LL2141: if(*(( int*) _temp2135) ==  Cyc_Absyn_Let_d){
goto _LL2142;} else{ goto _LL2143;} _LL2143: if(*(( int*) _temp2135) ==  Cyc_Absyn_Letv_d){
goto _LL2144;} else{ goto _LL2145;} _LL2145: if(*(( int*) _temp2135) ==  Cyc_Absyn_Struct_d){
_LL2166: _temp2165=(( struct Cyc_Absyn_Struct_d_struct*) _temp2135)->f1; goto
_LL2146;} else{ goto _LL2147;} _LL2147: if(*(( int*) _temp2135) ==  Cyc_Absyn_Union_d){
_LL2168: _temp2167=(( struct Cyc_Absyn_Union_d_struct*) _temp2135)->f1; goto
_LL2148;} else{ goto _LL2149;} _LL2149: if(*(( int*) _temp2135) ==  Cyc_Absyn_Tunion_d){
_LL2170: _temp2169=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2135)->f1; goto
_LL2150;} else{ goto _LL2151;} _LL2151: if(*(( int*) _temp2135) ==  Cyc_Absyn_Enum_d){
_LL2172: _temp2171=(( struct Cyc_Absyn_Enum_d_struct*) _temp2135)->f1; goto
_LL2152;} else{ goto _LL2153;} _LL2153: if(*(( int*) _temp2135) ==  Cyc_Absyn_Typedef_d){
_LL2174: _temp2173=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2135)->f1; goto
_LL2154;} else{ goto _LL2155;} _LL2155: if(*(( int*) _temp2135) ==  Cyc_Absyn_Namespace_d){
_LL2176: _temp2175=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2135)->f2; goto
_LL2156;} else{ goto _LL2157;} _LL2157: if(*(( int*) _temp2135) ==  Cyc_Absyn_Using_d){
_LL2178: _temp2177=(( struct Cyc_Absyn_Using_d_struct*) _temp2135)->f2; goto
_LL2158;} else{ goto _LL2159;} _LL2159: if(*(( int*) _temp2135) ==  Cyc_Absyn_ExternC_d){
_LL2180: _temp2179=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2135)->f1; goto
_LL2160;} else{ goto _LL2136;} _LL2138: { struct _tuple0* _temp2181= _temp2161->name;
if(( void*) _temp2161->sc == ( void*) Cyc_Absyn_ExternC){ _temp2181=({ struct
_tuple0* _temp2182=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2182->f1= Cyc_Absyn_rel_ns_null; _temp2182->f2=(* _temp2181).f2; _temp2182;});}
if( _temp2161->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2161->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2161->name,
Cyc_Absyn_varb_exp( _temp2181,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2183=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2183[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2184; _temp2184.tag= Cyc_Absyn_Global_b;
_temp2184.f1= _temp2161; _temp2184;}); _temp2183;}), 0)); _temp2161->name=
_temp2181;( void*)( _temp2161->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2161->sc));(
void*)( _temp2161->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2161->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2185=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2185->hd=( void*) d; _temp2185->tl=
Cyc_Toc_result_decls; _temp2185;}); goto _LL2136;} _LL2140: { struct _tuple0*
_temp2186= _temp2163->name; if(( void*) _temp2163->sc == ( void*) Cyc_Absyn_ExternC){
_temp2186=({ struct _tuple0* _temp2187=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2187->f1= Cyc_Absyn_rel_ns_null; _temp2187->f2=(*
_temp2186).f2; _temp2187;});} nv= Cyc_Toc_add_varmap( nv, _temp2163->name, Cyc_Absyn_var_exp(
_temp2186, 0)); _temp2163->name= _temp2186; Cyc_Toc_fndecl_to_c( nv, _temp2163);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2188=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2188->hd=( void*) d; _temp2188->tl=
Cyc_Toc_result_decls; _temp2188;}); goto _LL2136;} _LL2142: goto _LL2144;
_LL2144:({ void* _temp2189[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2189, sizeof( void*), 0u));}); goto _LL2136;
_LL2146: Cyc_Toc_structdecl_to_c( _temp2165); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2190=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2190->hd=( void*) d; _temp2190->tl= Cyc_Toc_result_decls; _temp2190;});
goto _LL2136; _LL2148: Cyc_Toc_uniondecl_to_c( _temp2167); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2191=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2191->hd=( void*) d; _temp2191->tl= Cyc_Toc_result_decls;
_temp2191;}); goto _LL2136; _LL2150: if( _temp2169->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2169);} else{ Cyc_Toc_tuniondecl_to_c( _temp2169);} goto _LL2136; _LL2152:
Cyc_Toc_enumdecl_to_c( nv, _temp2171); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2192=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2192->hd=( void*) d; _temp2192->tl= Cyc_Toc_result_decls; _temp2192;});
goto _LL2136; _LL2154: _temp2173->name= _temp2173->name; _temp2173->tvs= 0;(
void*)( _temp2173->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2173->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2193=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2193->hd=( void*) d; _temp2193->tl=
Cyc_Toc_result_decls; _temp2193;}); goto _LL2136; _LL2156: _temp2177= _temp2175;
goto _LL2158; _LL2158: _temp2179= _temp2177; goto _LL2160; _LL2160: nv= Cyc_Toc_decls_to_c(
nv, _temp2179, top); goto _LL2136; _LL2136:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
