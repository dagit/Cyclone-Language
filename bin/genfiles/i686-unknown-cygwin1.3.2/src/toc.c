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
8u]; extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List* x);
extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); extern
struct _tagged_arr Cyc_Std_strconcat( struct _tagged_arr, struct _tagged_arr);
struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* cmp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct
Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern struct
Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* k); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
extern struct _tagged_arr Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
_temp196;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp197= t; struct
Cyc_Core_Opt* _temp251; struct Cyc_Core_Opt* _temp253; struct Cyc_Core_Opt
_temp255; void* _temp256; struct Cyc_Absyn_TunionInfo _temp258; void* _temp260;
struct Cyc_Absyn_Tuniondecl* _temp262; struct Cyc_Absyn_TunionFieldInfo _temp264;
void* _temp266; struct Cyc_Absyn_Tunionfield* _temp268; struct Cyc_Absyn_Tuniondecl*
_temp270; struct Cyc_Absyn_PtrInfo _temp272; struct Cyc_Absyn_Conref* _temp274;
struct Cyc_Absyn_Tqual _temp276; void* _temp278; struct Cyc_Absyn_Exp* _temp280;
struct Cyc_Absyn_Tqual _temp282; void* _temp284; struct Cyc_Absyn_FnInfo
_temp286; struct Cyc_List_List* _temp288; struct Cyc_Absyn_VarargInfo* _temp290;
int _temp292; struct Cyc_List_List* _temp294; void* _temp296; struct Cyc_List_List*
_temp298; struct Cyc_List_List* _temp300; struct Cyc_List_List* _temp302; struct
_tuple0* _temp304; struct _tuple0* _temp306; struct _tuple0* _temp308; struct
Cyc_Core_Opt* _temp310; struct Cyc_List_List* _temp312; struct _tuple0* _temp314;
void* _temp316; _LL199: if( _temp197 == ( void*) Cyc_Absyn_VoidType){ goto
_LL200;} else{ goto _LL201;} _LL201: if(( unsigned int) _temp197 >  4u?*(( int*)
_temp197) ==  Cyc_Absyn_Evar: 0){ _LL252: _temp251=(( struct Cyc_Absyn_Evar_struct*)
_temp197)->f2; if( _temp251 ==  0){ goto _LL202;} else{ goto _LL203;}} else{
goto _LL203;} _LL203: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) == 
Cyc_Absyn_Evar: 0){ _LL254: _temp253=(( struct Cyc_Absyn_Evar_struct*) _temp197)->f2;
if( _temp253 ==  0){ goto _LL205;} else{ _temp255=* _temp253; _LL257: _temp256=(
void*) _temp255.v; goto _LL204;}} else{ goto _LL205;} _LL205: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_VarType: 0){ goto _LL206;}
else{ goto _LL207;} _LL207: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197)
==  Cyc_Absyn_TunionType: 0){ _LL259: _temp258=(( struct Cyc_Absyn_TunionType_struct*)
_temp197)->f1; _LL261: _temp260=( void*) _temp258.tunion_info; if(*(( int*)
_temp260) ==  Cyc_Absyn_KnownTunion){ _LL263: _temp262=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp260)->f1; goto _LL208;} else{ goto _LL209;}} else{ goto _LL209;} _LL209:
if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TunionType:
0){ goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_TunionFieldType: 0){ _LL265: _temp264=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp197)->f1; _LL267: _temp266=( void*)
_temp264.field_info; if(*(( int*) _temp266) ==  Cyc_Absyn_KnownTunionfield){
_LL271: _temp270=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp266)->f1;
goto _LL269; _LL269: _temp268=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp266)->f2; goto _LL212;} else{ goto _LL213;}} else{ goto _LL213;} _LL213:
if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_PointerType: 0){ _LL273: _temp272=(( struct Cyc_Absyn_PointerType_struct*)
_temp197)->f1; _LL279: _temp278=( void*) _temp272.elt_typ; goto _LL277; _LL277:
_temp276= _temp272.tq; goto _LL275; _LL275: _temp274= _temp272.bounds; goto
_LL216;} else{ goto _LL217;} _LL217: if(( unsigned int) _temp197 >  4u?*(( int*)
_temp197) ==  Cyc_Absyn_IntType: 0){ goto _LL218;} else{ goto _LL219;} _LL219:
if( _temp197 == ( void*) Cyc_Absyn_FloatType){ goto _LL220;} else{ goto _LL221;}
_LL221: if( _temp197 == ( void*) Cyc_Absyn_DoubleType){ goto _LL222;} else{ goto
_LL223;} _LL223: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_ArrayType:
0){ _LL285: _temp284=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp197)->f1;
goto _LL283; _LL283: _temp282=(( struct Cyc_Absyn_ArrayType_struct*) _temp197)->f2;
goto _LL281; _LL281: _temp280=(( struct Cyc_Absyn_ArrayType_struct*) _temp197)->f3;
goto _LL224;} else{ goto _LL225;} _LL225: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_FnType: 0){ _LL287: _temp286=(( struct Cyc_Absyn_FnType_struct*)
_temp197)->f1; _LL297: _temp296=( void*) _temp286.ret_typ; goto _LL295; _LL295:
_temp294= _temp286.args; goto _LL293; _LL293: _temp292= _temp286.c_varargs; goto
_LL291; _LL291: _temp290= _temp286.cyc_varargs; goto _LL289; _LL289: _temp288=
_temp286.attributes; goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TupleType: 0){ _LL299: _temp298=((
struct Cyc_Absyn_TupleType_struct*) _temp197)->f1; goto _LL228;} else{ goto
_LL229;} _LL229: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_AnonStructType:
0){ _LL301: _temp300=(( struct Cyc_Absyn_AnonStructType_struct*) _temp197)->f1;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_AnonUnionType: 0){ _LL303: _temp302=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp197)->f1; goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_UnionType: 0){ _LL305: _temp304=((
struct Cyc_Absyn_UnionType_struct*) _temp197)->f1; goto _LL234;} else{ goto
_LL235;} _LL235: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_StructType:
0){ _LL307: _temp306=(( struct Cyc_Absyn_StructType_struct*) _temp197)->f1; goto
_LL236;} else{ goto _LL237;} _LL237: if(( unsigned int) _temp197 >  4u?*(( int*)
_temp197) ==  Cyc_Absyn_EnumType: 0){ _LL309: _temp308=(( struct Cyc_Absyn_EnumType_struct*)
_temp197)->f1; goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TypedefType: 0){ _LL315:
_temp314=(( struct Cyc_Absyn_TypedefType_struct*) _temp197)->f1; goto _LL313;
_LL313: _temp312=(( struct Cyc_Absyn_TypedefType_struct*) _temp197)->f2; goto
_LL311; _LL311: _temp310=(( struct Cyc_Absyn_TypedefType_struct*) _temp197)->f3;
goto _LL240;} else{ goto _LL241;} _LL241: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_RgnHandleType: 0){ _LL317: _temp316=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp197)->f1; goto _LL242;} else{ goto
_LL243;} _LL243: if( _temp197 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL244;}
else{ goto _LL245;} _LL245: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197)
==  Cyc_Absyn_AccessEff: 0){ goto _LL246;} else{ goto _LL247;} _LL247: if((
unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_JoinEff: 0){ goto
_LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp197 >  4u?*(( int*)
_temp197) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL250;} else{ goto _LL198;} _LL200:
return t; _LL202: return Cyc_Absyn_sint_t; _LL204: return Cyc_Toc_typ_to_c(
_temp256); _LL206: return Cyc_Absyn_void_star_typ(); _LL208: return Cyc_Absyn_void_star_typ();
_LL210:( int) _throw(({ void* _temp318[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp318, sizeof( void*), 0u));}));
_LL212: if( _temp268->typs ==  0){ if( _temp270->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp268->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL214:( int) _throw(({ void*
_temp319[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp319, sizeof( void*), 0u));}));
_LL216: _temp278= Cyc_Toc_typ_to_c_array( _temp278);{ void* _temp320=( void*)(
Cyc_Absyn_compress_conref( _temp274))->v; void* _temp326; _LL322: if((
unsigned int) _temp320 >  1u?*(( int*) _temp320) ==  Cyc_Absyn_Eq_constr: 0){
_LL327: _temp326=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp320)->f1;
if( _temp326 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL323;} else{ goto _LL324;}}
else{ goto _LL324;} _LL324: goto _LL325; _LL323: return Cyc_Toc_tagged_arr_typ;
_LL325: return Cyc_Absyn_star_typ( _temp278,( void*) Cyc_Absyn_HeapRgn, _temp276);
_LL321:;} _LL218: return t; _LL220: return t; _LL222: return t; _LL224: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp328=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp328[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp329; _temp329.tag= Cyc_Absyn_ArrayType; _temp329.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp284); _temp329.f2= _temp282; _temp329.f3=
_temp280; _temp329;}); _temp328;}); _LL226: { struct Cyc_List_List* _temp330= 0;
for( 0; _temp288 !=  0; _temp288=(( struct Cyc_List_List*) _check_null( _temp288))->tl){
void* _temp331=( void*)(( struct Cyc_List_List*) _check_null( _temp288))->hd;
_LL333: if( _temp331 == ( void*) Cyc_Absyn_Noreturn_att){ goto _LL334;} else{
goto _LL335;} _LL335: if( _temp331 == ( void*) Cyc_Absyn_Const_att){ goto _LL336;}
else{ goto _LL337;} _LL337: if(( unsigned int) _temp331 >  16u?*(( int*)
_temp331) ==  Cyc_Absyn_Format_att: 0){ goto _LL338;} else{ goto _LL339;} _LL339:
goto _LL340; _LL334: goto _LL336; _LL336: goto _LL338; _LL338: continue; _LL340:
_temp330=({ struct Cyc_List_List* _temp341=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp341->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp288))->hd); _temp341->tl= _temp330; _temp341;}); goto _LL332;
_LL332:;}{ struct Cyc_List_List* _temp342=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp294); if( _temp290 !=  0){ void* _temp343= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp290))->type,( void*)
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp344=({ struct _tuple1*
_temp346=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp346->f1=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp290))->name; _temp346->f2=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp290))->tq; _temp346->f3=
_temp343; _temp346;}); _temp342=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp342,({ struct Cyc_List_List*
_temp345=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp345->hd=( void*) _temp344; _temp345->tl= 0; _temp345;}));} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp347=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp347[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp348; _temp348.tag= Cyc_Absyn_FnType; _temp348.f1=({ struct Cyc_Absyn_FnInfo
_temp349; _temp349.tvars= 0; _temp349.effect= 0; _temp349.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp296); _temp349.args= _temp342; _temp349.c_varargs= _temp292; _temp349.cyc_varargs=
0; _temp349.rgn_po= 0; _temp349.attributes= _temp330; _temp349;}); _temp348;});
_temp347;});}} _LL228: _temp298=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp298);{ struct _tagged_arr* _temp350= Cyc_Toc_add_tuple_type( _temp298);
return Cyc_Absyn_strct( _temp350);} _LL230: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp351=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp351[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp352; _temp352.tag= Cyc_Absyn_AnonStructType; _temp352.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp300); _temp352;}); _temp351;});
_LL232: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp353=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp353[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp354; _temp354.tag=
Cyc_Absyn_AnonUnionType; _temp354.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp302); _temp354;}); _temp353;}); _LL234: if( _temp304 ==  0){({ void*
_temp355[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp355,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp304)); _LL236: if( _temp306 ==  0){({ void* _temp356[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp356, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp306)); _LL238:
return t; _LL240: if( _temp310 ==  0){ return({ void* _temp357[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u),
_tag_arr( _temp357, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp358=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp358[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp359; _temp359.tag= Cyc_Absyn_TypedefType;
_temp359.f1= _temp314; _temp359.f2= 0; _temp359.f3=({ struct Cyc_Core_Opt*
_temp360=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp360->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp310))->v); _temp360;}); _temp359;}); _temp358;}); _LL242:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL244: return({ void* _temp361[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp361, sizeof( void*), 0u));});
_LL246: return({ void* _temp362[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp362, sizeof( void*), 0u));});
_LL248: return({ void* _temp363[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp363, sizeof( void*), 0u));});
_LL250: return({ void* _temp364[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp364, sizeof( void*), 0u));});
_LL198:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp365= t;
struct Cyc_Absyn_Tqual _temp371; void* _temp373; _LL367: if(( unsigned int)
_temp365 >  4u?*(( int*) _temp365) ==  Cyc_Absyn_ArrayType: 0){ _LL374: _temp373=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp365)->f1; goto _LL372; _LL372:
_temp371=(( struct Cyc_Absyn_ArrayType_struct*) _temp365)->f2; goto _LL368;}
else{ goto _LL369;} _LL369: goto _LL370; _LL368: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp373,( void*) Cyc_Absyn_HeapRgn, _temp371), e, l); _LL370: return Cyc_Absyn_cast_exp(
t, e, l); _LL366:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp375= Cyc_Tcutil_compress(
t); void* _temp413; struct Cyc_Absyn_Structdecl** _temp415; struct Cyc_List_List*
_temp417; struct Cyc_List_List* _temp419; struct Cyc_Absyn_TunionFieldInfo
_temp421; void* _temp423; struct Cyc_Absyn_Tunionfield* _temp425; struct Cyc_Absyn_Tuniondecl*
_temp427; struct Cyc_List_List* _temp429; _LL377: if( _temp375 == ( void*) Cyc_Absyn_VoidType){
goto _LL378;} else{ goto _LL379;} _LL379: if(( unsigned int) _temp375 >  4u?*((
int*) _temp375) ==  Cyc_Absyn_VarType: 0){ goto _LL380;} else{ goto _LL381;}
_LL381: if(( unsigned int) _temp375 >  4u?*(( int*) _temp375) ==  Cyc_Absyn_IntType:
0){ goto _LL382;} else{ goto _LL383;} _LL383: if(( unsigned int) _temp375 >  4u?*((
int*) _temp375) ==  Cyc_Absyn_EnumType: 0){ goto _LL384;} else{ goto _LL385;}
_LL385: if( _temp375 == ( void*) Cyc_Absyn_FloatType){ goto _LL386;} else{ goto
_LL387;} _LL387: if( _temp375 == ( void*) Cyc_Absyn_DoubleType){ goto _LL388;}
else{ goto _LL389;} _LL389: if(( unsigned int) _temp375 >  4u?*(( int*) _temp375)
==  Cyc_Absyn_FnType: 0){ goto _LL390;} else{ goto _LL391;} _LL391: if((
unsigned int) _temp375 >  4u?*(( int*) _temp375) ==  Cyc_Absyn_ArrayType: 0){
_LL414: _temp413=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp375)->f1;
goto _LL392;} else{ goto _LL393;} _LL393: if(( unsigned int) _temp375 >  4u?*((
int*) _temp375) ==  Cyc_Absyn_StructType: 0){ _LL416: _temp415=(( struct Cyc_Absyn_StructType_struct*)
_temp375)->f3; goto _LL394;} else{ goto _LL395;} _LL395: if(( unsigned int)
_temp375 >  4u?*(( int*) _temp375) ==  Cyc_Absyn_AnonStructType: 0){ _LL418:
_temp417=(( struct Cyc_Absyn_AnonStructType_struct*) _temp375)->f1; goto _LL396;}
else{ goto _LL397;} _LL397: if(( unsigned int) _temp375 >  4u?*(( int*) _temp375)
==  Cyc_Absyn_AnonUnionType: 0){ _LL420: _temp419=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp375)->f1; goto _LL398;} else{ goto _LL399;} _LL399: if(( unsigned int)
_temp375 >  4u?*(( int*) _temp375) ==  Cyc_Absyn_TunionFieldType: 0){ _LL422:
_temp421=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp375)->f1; _LL424:
_temp423=( void*) _temp421.field_info; if(*(( int*) _temp423) ==  Cyc_Absyn_KnownTunionfield){
_LL428: _temp427=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp423)->f1;
goto _LL426; _LL426: _temp425=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp423)->f2; goto _LL400;} else{ goto _LL401;}} else{ goto _LL401;} _LL401:
if(( unsigned int) _temp375 >  4u?*(( int*) _temp375) ==  Cyc_Absyn_TupleType: 0){
_LL430: _temp429=(( struct Cyc_Absyn_TupleType_struct*) _temp375)->f1; goto
_LL402;} else{ goto _LL403;} _LL403: if(( unsigned int) _temp375 >  4u?*(( int*)
_temp375) ==  Cyc_Absyn_TunionType: 0){ goto _LL404;} else{ goto _LL405;} _LL405:
if(( unsigned int) _temp375 >  4u?*(( int*) _temp375) ==  Cyc_Absyn_UnionType: 0){
goto _LL406;} else{ goto _LL407;} _LL407: if(( unsigned int) _temp375 >  4u?*((
int*) _temp375) ==  Cyc_Absyn_PointerType: 0){ goto _LL408;} else{ goto _LL409;}
_LL409: if(( unsigned int) _temp375 >  4u?*(( int*) _temp375) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL410;} else{ goto _LL411;} _LL411: goto _LL412; _LL378: return 1;
_LL380: return 0; _LL382: return 1; _LL384: return 1; _LL386: return 1; _LL388:
return 1; _LL390: return 1; _LL392: return Cyc_Toc_atomic_typ( _temp413); _LL394:
if( _temp415 ==  0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp415)))->fields
==  0){ return 0;}{ struct Cyc_List_List* _temp431=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp415)))->fields))->v; for( 0; _temp431 !=  0; _temp431=((
struct Cyc_List_List*) _check_null( _temp431))->tl){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp431))->hd)->type)){ return 0;}}} return 1; _LL396: _temp419= _temp417; goto
_LL398; _LL398: for( 0; _temp419 !=  0; _temp419=(( struct Cyc_List_List*)
_check_null( _temp419))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp419))->hd)->type)){ return 0;}} return
1; _LL400: _temp429= _temp425->typs; goto _LL402; _LL402: for( 0; _temp429 !=  0;
_temp429=(( struct Cyc_List_List*) _check_null( _temp429))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp429))->hd)).f2)){
return 0;}} return 1; _LL404: return 0; _LL406: return 0; _LL408: return 0;
_LL410: return 0; _LL412:({ struct Cyc_Std_String_pa_struct _temp433; _temp433.tag=
Cyc_Std_String_pa; _temp433.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp432[ 1u]={& _temp433};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp432, sizeof( void*), 1u));}});
return 0; _LL376:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp434=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp440; void* _temp442;
_LL436: if(( unsigned int) _temp434 >  4u?*(( int*) _temp434) ==  Cyc_Absyn_PointerType:
0){ _LL441: _temp440=(( struct Cyc_Absyn_PointerType_struct*) _temp434)->f1;
_LL443: _temp442=( void*) _temp440.elt_typ; goto _LL437;} else{ goto _LL438;}
_LL438: goto _LL439; _LL437: { void* _temp444= Cyc_Tcutil_compress( _temp442);
_LL446: if( _temp444 == ( void*) Cyc_Absyn_VoidType){ goto _LL447;} else{ goto
_LL448;} _LL448: goto _LL449; _LL447: return 1; _LL449: return 0; _LL445:;}
_LL439: return 0; _LL435:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp450= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp462; struct Cyc_Absyn_Structdecl* _temp464; struct Cyc_List_List* _temp465;
_LL452: if(( unsigned int) _temp450 >  4u?*(( int*) _temp450) ==  Cyc_Absyn_StructType:
0){ _LL463: _temp462=(( struct Cyc_Absyn_StructType_struct*) _temp450)->f3; if(
_temp462 ==  0){ goto _LL454;} else{ _temp464=* _temp462; goto _LL453;}} else{
goto _LL454;} _LL454: if(( unsigned int) _temp450 >  4u?*(( int*) _temp450) == 
Cyc_Absyn_AnonStructType: 0){ _LL466: _temp465=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp450)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp450 >  4u?*(( int*) _temp450) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL457;}
else{ goto _LL458;} _LL458: if(( unsigned int) _temp450 >  4u?*(( int*) _temp450)
==  Cyc_Absyn_UnionType: 0){ goto _LL459;} else{ goto _LL460;} _LL460: goto
_LL461; _LL453: if( _temp464->fields ==  0){( int) _throw(({ void* _temp467[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp467, sizeof( void*), 0u));}));} _temp465=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp464->fields))->v;
goto _LL455; _LL455: { struct Cyc_Absyn_Structfield* _temp468= Cyc_Absyn_lookup_field(
_temp465, f); if( _temp468 ==  0){({ struct Cyc_Std_String_pa_struct _temp470;
_temp470.tag= Cyc_Std_String_pa; _temp470.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp468))->name;{ void* _temp469[ 1u]={& _temp470};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_field: bad field %s",
sizeof( unsigned char), 28u), _tag_arr( _temp469, sizeof( void*), 1u));}});}
return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp468))->type);} _LL457: return 0; _LL459: return 0; _LL461:(
int) _throw(({ struct Cyc_Std_String_pa_struct _temp472; _temp472.tag= Cyc_Std_String_pa;
_temp472.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp471[ 1u]={&
_temp472}; Cyc_Toc_toc_impos( _tag_arr("is_poly_field: bad type %s", sizeof(
unsigned char), 27u), _tag_arr( _temp471, sizeof( void*), 1u));}})); _LL451:;}
static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp473=(
void*) e->r; struct _tagged_arr* _temp481; struct Cyc_Absyn_Exp* _temp483;
struct _tagged_arr* _temp485; struct Cyc_Absyn_Exp* _temp487; _LL475: if(*(( int*)
_temp473) ==  Cyc_Absyn_StructMember_e){ _LL484: _temp483=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp473)->f1; goto _LL482; _LL482: _temp481=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp473)->f2; goto _LL476;} else{ goto _LL477;} _LL477: if(*(( int*) _temp473)
==  Cyc_Absyn_StructArrow_e){ _LL488: _temp487=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp473)->f1; goto _LL486; _LL486: _temp485=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp473)->f2; goto _LL478;} else{ goto _LL479;} _LL479: goto _LL480; _LL476:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp483->topt))->v, _temp481); _LL478: { void* _temp489= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp487->topt))->v); struct Cyc_Absyn_PtrInfo
_temp495; void* _temp497; _LL491: if(( unsigned int) _temp489 >  4u?*(( int*)
_temp489) ==  Cyc_Absyn_PointerType: 0){ _LL496: _temp495=(( struct Cyc_Absyn_PointerType_struct*)
_temp489)->f1; _LL498: _temp497=( void*) _temp495.elt_typ; goto _LL492;} else{
goto _LL493;} _LL493: goto _LL494; _LL492: return Cyc_Toc_is_poly_field(
_temp497, _temp485); _LL494:({ struct Cyc_Std_String_pa_struct _temp500;
_temp500.tag= Cyc_Std_String_pa; _temp500.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp487->topt))->v);{ void*
_temp499[ 1u]={& _temp500};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s", sizeof(
unsigned char), 30u), _tag_arr( _temp499, sizeof( void*), 1u));}}); return 0;
_LL490:;} _LL480: return 0; _LL474:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_e,({
struct Cyc_List_List* _temp501=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp501->hd=( void*) s; _temp501->tl= 0; _temp501;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_atomic_e,({ struct Cyc_List_List*
_temp502=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp502->hd=( void*) s; _temp502->tl= 0; _temp502;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp503[ 2u]; _temp503[ 1u]= s; _temp503[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp503, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp504=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp504->hd=( void*) e; _temp504->tl= 0; _temp504;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp505=( void*) e->r; void* _temp511; _LL507: if(*((
int*) _temp505) ==  Cyc_Absyn_Const_e){ _LL512: _temp511=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp505)->f1; if(( unsigned int) _temp511 >  1u?*(( int*) _temp511) ==  Cyc_Absyn_String_c:
0){ goto _LL508;} else{ goto _LL509;}} else{ goto _LL509;} _LL509: goto _LL510;
_LL508: is_string= 1; goto _LL506; _LL510: goto _LL506; _LL506:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp516=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp516[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp517; _temp517.tag= Cyc_Absyn_ArrayType; _temp517.f1=(
void*) Cyc_Absyn_uchar_t; _temp517.f2= Cyc_Toc_mt_tq; _temp517.f3=( struct Cyc_Absyn_Exp*)
sz; _temp517;}); _temp516;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp513=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp513->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp514=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp514[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp515; _temp515.tag= Cyc_Absyn_Var_d;
_temp515.f1= vd; _temp515;}); _temp514;}), 0); _temp513->tl= Cyc_Toc_result_decls;
_temp513;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp518[ 3u]; _temp518[ 2u]=({ struct _tuple4* _temp521=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp521->f1= 0; _temp521->f2=
xplussz; _temp521;}); _temp518[ 1u]=({ struct _tuple4* _temp520=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp520->f1= 0; _temp520->f2= xexp;
_temp520;}); _temp518[ 0u]=({ struct _tuple4* _temp519=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp519->f1= 0; _temp519->f2= xexp;
_temp519;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp518, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp522=( struct Cyc_Toc_Env*) _cycalloc( sizeof( struct
Cyc_Toc_Env)); _temp522->break_lab= 0; _temp522->continue_lab= 0; _temp522->fallthru_info=
0; _temp522->varmap=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp522->toplevel= 1;
_temp522;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp523=( struct Cyc_Toc_Env*) _cycalloc( sizeof(
struct Cyc_Toc_Env)); _temp523->break_lab= e->break_lab; _temp523->continue_lab=
e->continue_lab; _temp523->fallthru_info= e->fallthru_info; _temp523->varmap= e->varmap;
_temp523->toplevel= e->toplevel; _temp523;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp524=(* x).f1; _LL526:
if(( unsigned int) _temp524 >  1u?*(( int*) _temp524) ==  Cyc_Absyn_Rel_n: 0){
goto _LL527;} else{ goto _LL528;} _LL528: goto _LL529; _LL527:( int) _throw(({
struct Cyc_Std_String_pa_struct _temp531; _temp531.tag= Cyc_Std_String_pa;
_temp531.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp530[
1u]={& _temp531}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp530, sizeof( void*), 1u));}}));
_LL529: goto _LL525; _LL525:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
k, struct Cyc_Absyn_Exp* v)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict*
f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp532= 0; for( 0; fallthru_binders !=  0; fallthru_binders=(( struct Cyc_List_List*)
_check_null( fallthru_binders))->tl){ _temp532=({ struct Cyc_List_List* _temp533=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp533->hd=(
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
fallthru_binders))->hd)->name; _temp533->tl= _temp532; _temp533;});} _temp532=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp532);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp534=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp534->v=( void*) break_l; _temp534;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp535=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp535->v=( void*)({ struct _tuple6* _temp536=( struct _tuple6*) _cycalloc(
sizeof( struct _tuple6)); _temp536->f1= fallthru_l; _temp536->f2= _temp532;
_temp536->f3= next_case_env->varmap; _temp536;}); _temp535;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp537=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp537->v=( void*) break_l; _temp537;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp538=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp538->v=( void*)({
struct _tuple6* _temp539=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp539->f1= next_l; _temp539->f2= 0; _temp539->f3=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp539;}); _temp538;}); return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt ==  0){({ void* _temp540[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp540, sizeof( void*), 0u));});}
return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp541[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp541, sizeof( void*), 0u));});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp542=( struct _tuple3*) _cycalloc( sizeof(
struct _tuple3)); _temp542->f1= Cyc_Toc_mt_tq; _temp542->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp542;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp543=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp543->f1= 0; _temp543->f2= e; _temp543;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp544= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp544): Cyc_Toc_malloc_ptr( _temp544), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp544), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp545=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp545 !=  0; _temp545=((
struct Cyc_List_List*) _check_null( _temp545))->tl){ struct _tuple4 _temp548;
struct Cyc_Absyn_Exp* _temp549; struct Cyc_List_List* _temp551; struct _tuple4*
_temp546=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp545))->hd;
_temp548=* _temp546; _LL552: _temp551= _temp548.f1; goto _LL550; _LL550:
_temp549= _temp548.f2; goto _LL547; _LL547: { struct Cyc_Absyn_Exp* e_index; if(
_temp551 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp551))->tl !=  0){({ void* _temp553[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp553, sizeof( void*), 0u));});}{
void* _temp554=( void*)(( struct Cyc_List_List*) _check_null( _temp551))->hd;
void* _temp555= _temp554; struct Cyc_Absyn_Exp* _temp561; _LL557: if(*(( int*)
_temp555) ==  Cyc_Absyn_ArrayElement){ _LL562: _temp561=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp555)->f1; goto _LL558;} else{ goto _LL559;} _LL559: if(*(( int*) _temp555)
==  Cyc_Absyn_FieldName){ goto _LL560;} else{ goto _LL556;} _LL558: Cyc_Toc_exp_to_c(
nv, _temp561); e_index= _temp561; goto _LL556; _LL560: e_index=({ void* _temp563[
0u]={};(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_unimp)( _tag_arr("field name designators in array", sizeof(
unsigned char), 32u), _tag_arr( _temp563, sizeof( void*), 0u));}); goto _LL556;
_LL556:;}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0);
void* _temp564=( void*) _temp549->r; struct Cyc_List_List* _temp574; struct Cyc_Absyn_Exp*
_temp576; struct Cyc_Absyn_Exp* _temp578; struct Cyc_Absyn_Vardecl* _temp580;
struct Cyc_List_List* _temp582; void* _temp584; _LL566: if(*(( int*) _temp564)
==  Cyc_Absyn_Array_e){ _LL575: _temp574=(( struct Cyc_Absyn_Array_e_struct*)
_temp564)->f1; goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp564)
==  Cyc_Absyn_Comprehension_e){ _LL581: _temp580=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp564)->f1; goto _LL579; _LL579: _temp578=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp564)->f2; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp564)->f3; goto _LL569;} else{ goto _LL570;} _LL570: if(*(( int*) _temp564)
==  Cyc_Absyn_AnonStruct_e){ _LL585: _temp584=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp564)->f1; goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp564)->f2; goto _LL571;} else{ goto _LL572;} _LL572: goto _LL573; _LL567: s=
Cyc_Toc_init_array( nv, lval, _temp574, s); goto _LL565; _LL569: s= Cyc_Toc_init_comprehension(
nv, lval, _temp580, _temp578, _temp576, s, 0); goto _LL565; _LL571: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp584, _temp582, s); goto _LL565; _LL573: Cyc_Toc_exp_to_c( nv,
_temp549); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp549, 0), s, 0); goto _LL565; _LL565:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp586= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp586, Cyc_Absyn_varb_exp( _temp586,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp609=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp609[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp610; _temp610.tag= Cyc_Absyn_Local_b; _temp610.f1= vd; _temp610;});
_temp609;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp586, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp586, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp586, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp586, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp587=( void*) e2->r; struct Cyc_List_List* _temp597; struct Cyc_Absyn_Exp*
_temp599; struct Cyc_Absyn_Exp* _temp601; struct Cyc_Absyn_Vardecl* _temp603;
struct Cyc_List_List* _temp605; void* _temp607; _LL589: if(*(( int*) _temp587)
==  Cyc_Absyn_Array_e){ _LL598: _temp597=(( struct Cyc_Absyn_Array_e_struct*)
_temp587)->f1; goto _LL590;} else{ goto _LL591;} _LL591: if(*(( int*) _temp587)
==  Cyc_Absyn_Comprehension_e){ _LL604: _temp603=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp587)->f1; goto _LL602; _LL602: _temp601=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp587)->f2; goto _LL600; _LL600: _temp599=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp587)->f3; goto _LL592;} else{ goto _LL593;} _LL593: if(*(( int*) _temp587)
==  Cyc_Absyn_AnonStruct_e){ _LL608: _temp607=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp587)->f1; goto _LL606; _LL606: _temp605=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp587)->f2; goto _LL594;} else{ goto _LL595;} _LL595: goto _LL596; _LL590:
body= Cyc_Toc_init_array( nv2, lval, _temp597, Cyc_Absyn_skip_stmt( 0)); goto
_LL588; _LL592: body= Cyc_Toc_init_comprehension( nv2, lval, _temp603, _temp601,
_temp599, Cyc_Absyn_skip_stmt( 0), 0); goto _LL588; _LL594: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp607, _temp605, Cyc_Absyn_skip_stmt( 0)); goto _LL588; _LL596: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL588; _LL588:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp586, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp611=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp611 !=  0; _temp611=(( struct Cyc_List_List*)
_check_null( _temp611))->tl){ struct _tuple4 _temp614; struct Cyc_Absyn_Exp*
_temp615; struct Cyc_List_List* _temp617; struct _tuple4* _temp612=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp611))->hd; _temp614=*
_temp612; _LL618: _temp617= _temp614.f1; goto _LL616; _LL616: _temp615= _temp614.f2;
goto _LL613; _LL613: if( _temp617 ==  0){({ void* _temp619[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp619, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp617))->tl !=  0){({ void* _temp620[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct", sizeof( unsigned char), 41u),
_tag_arr( _temp620, sizeof( void*), 0u));});}{ void* _temp621=( void*)(( struct
Cyc_List_List*) _check_null( _temp617))->hd; struct _tagged_arr* _temp627;
_LL623: if(*(( int*) _temp621) ==  Cyc_Absyn_FieldName){ _LL628: _temp627=((
struct Cyc_Absyn_FieldName_struct*) _temp621)->f1; goto _LL624;} else{ goto
_LL625;} _LL625: goto _LL626; _LL624: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp(
lhs, _temp627, 0);{ void* _temp629=( void*) _temp615->r; struct Cyc_List_List*
_temp639; struct Cyc_Absyn_Exp* _temp641; struct Cyc_Absyn_Exp* _temp643; struct
Cyc_Absyn_Vardecl* _temp645; struct Cyc_List_List* _temp647; void* _temp649;
_LL631: if(*(( int*) _temp629) ==  Cyc_Absyn_Array_e){ _LL640: _temp639=((
struct Cyc_Absyn_Array_e_struct*) _temp629)->f1; goto _LL632;} else{ goto _LL633;}
_LL633: if(*(( int*) _temp629) ==  Cyc_Absyn_Comprehension_e){ _LL646: _temp645=((
struct Cyc_Absyn_Comprehension_e_struct*) _temp629)->f1; goto _LL644; _LL644:
_temp643=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp629)->f2; goto _LL642;
_LL642: _temp641=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp629)->f3;
goto _LL634;} else{ goto _LL635;} _LL635: if(*(( int*) _temp629) ==  Cyc_Absyn_AnonStruct_e){
_LL650: _temp649=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp629)->f1;
goto _LL648; _LL648: _temp647=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp629)->f2;
goto _LL636;} else{ goto _LL637;} _LL637: goto _LL638; _LL632: s= Cyc_Toc_init_array(
nv, lval, _temp639, s); goto _LL630; _LL634: s= Cyc_Toc_init_comprehension( nv,
lval, _temp645, _temp643, _temp641, s, 0); goto _LL630; _LL636: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp649, _temp647, s); goto _LL630; _LL638: Cyc_Toc_exp_to_c( nv,
_temp615); if( Cyc_Toc_is_poly_field( struct_type, _temp627)){ _temp615= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp615, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp615, 0), 0), s, 0); goto _LL630; _LL630:;} goto
_LL622;} _LL626:( int) _throw(({ void* _temp651[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp651, sizeof( void*), 0u));})); _LL622:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp652= _new_region();
struct _RegionHandle* r=& _temp652; _push_region( r);{ struct Cyc_List_List*
_temp653=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp654= Cyc_Toc_add_tuple_type( _temp653); struct
_tuple0* _temp655= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp656= Cyc_Absyn_var_exp(
_temp655, 0); struct Cyc_Absyn_Stmt* _temp657= Cyc_Absyn_exp_stmt( _temp656, 0);
struct Cyc_Absyn_Exp*(* _temp658)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp659=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp659); for( 0; _temp659
!=  0;( _temp659=(( struct Cyc_List_List*) _check_null( _temp659))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp659))->hd; struct Cyc_Absyn_Exp* lval= _temp658( _temp656, Cyc_Absyn_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp660=( void*) e->r; struct Cyc_List_List*
_temp668; struct Cyc_Absyn_Exp* _temp670; struct Cyc_Absyn_Exp* _temp672; struct
Cyc_Absyn_Vardecl* _temp674; _LL662: if(*(( int*) _temp660) ==  Cyc_Absyn_Array_e){
_LL669: _temp668=(( struct Cyc_Absyn_Array_e_struct*) _temp660)->f1; goto _LL663;}
else{ goto _LL664;} _LL664: if(*(( int*) _temp660) ==  Cyc_Absyn_Comprehension_e){
_LL675: _temp674=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp660)->f1;
goto _LL673; _LL673: _temp672=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp660)->f2; goto _LL671; _LL671: _temp670=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp660)->f3; goto _LL665;} else{ goto _LL666;} _LL666: goto _LL667; _LL663:
_temp657= Cyc_Toc_init_array( nv, lval, _temp668, _temp657); goto _LL661; _LL665:
_temp657= Cyc_Toc_init_comprehension( nv, lval, _temp674, _temp672, _temp670,
_temp657, 0); goto _LL661; _LL667: Cyc_Toc_exp_to_c( nv, e); _temp657= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp658( _temp656, Cyc_Absyn_fieldname(
i), 0), e, 0), 0), _temp657, 0); goto _LL661; _LL661:;}}}{ struct Cyc_Absyn_Exp*
_temp676= Cyc_Toc_make_struct( nv, _temp655, Cyc_Absyn_strct( _temp654),
_temp657, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp676;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp677= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp678= Cyc_Absyn_var_exp( _temp677, 0); struct Cyc_Absyn_Stmt*
_temp679= Cyc_Absyn_exp_stmt( _temp678, 0); struct Cyc_Absyn_Exp*(* _temp680)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp681= _new_region(); struct _RegionHandle* r=& _temp681;
_push_region( r);{ struct Cyc_List_List* _temp682=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp682 !=  0; _temp682=(( struct Cyc_List_List*) _check_null( _temp682))->tl){
struct _tuple4 _temp685; struct Cyc_Absyn_Exp* _temp686; struct Cyc_List_List*
_temp688; struct _tuple4* _temp683=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp682))->hd; _temp685=* _temp683; _LL689: _temp688= _temp685.f1;
goto _LL687; _LL687: _temp686= _temp685.f2; goto _LL684; _LL684: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp686->topt))->v): 0; if( _temp688 ==  0){({ void* _temp690[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp690, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp688))->tl !=  0){ struct _tuple0*
_temp691= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp692= Cyc_Absyn_var_exp(
_temp691, 0); for( 0; _temp688 !=  0; _temp688=(( struct Cyc_List_List*)
_check_null( _temp688))->tl){ void* _temp693=( void*)(( struct Cyc_List_List*)
_check_null( _temp688))->hd; struct _tagged_arr* _temp699; _LL695: if(*(( int*)
_temp693) ==  Cyc_Absyn_FieldName){ _LL700: _temp699=(( struct Cyc_Absyn_FieldName_struct*)
_temp693)->f1; goto _LL696;} else{ goto _LL697;} _LL697: goto _LL698; _LL696:
if( Cyc_Toc_is_poly_field( struct_type, _temp699)){ _temp692= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp692, 0);} _temp679= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp680( _temp678, _temp699, 0), _temp692, 0), 0),
_temp679, 0); goto _LL694; _LL698:( int) _throw(({ void* _temp701[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp701, sizeof( void*), 0u));})); _LL694:;} Cyc_Toc_exp_to_c( nv, _temp686);
_temp679= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp692,
_temp686, 0), 0), _temp679, 0);} else{ void* _temp702=( void*)(( struct Cyc_List_List*)
_check_null( _temp688))->hd; struct _tagged_arr* _temp708; _LL704: if(*(( int*)
_temp702) ==  Cyc_Absyn_FieldName){ _LL709: _temp708=(( struct Cyc_Absyn_FieldName_struct*)
_temp702)->f1; goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL705: {
struct Cyc_Absyn_Exp* lval= _temp680( _temp678, _temp708, 0);{ void* _temp710=(
void*) _temp686->r; struct Cyc_List_List* _temp720; struct Cyc_Absyn_Exp*
_temp722; struct Cyc_Absyn_Exp* _temp724; struct Cyc_Absyn_Vardecl* _temp726;
struct Cyc_List_List* _temp728; void* _temp730; _LL712: if(*(( int*) _temp710)
==  Cyc_Absyn_Array_e){ _LL721: _temp720=(( struct Cyc_Absyn_Array_e_struct*)
_temp710)->f1; goto _LL713;} else{ goto _LL714;} _LL714: if(*(( int*) _temp710)
==  Cyc_Absyn_Comprehension_e){ _LL727: _temp726=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp710)->f1; goto _LL725; _LL725: _temp724=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp710)->f2; goto _LL723; _LL723: _temp722=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp710)->f3; goto _LL715;} else{ goto _LL716;} _LL716: if(*(( int*) _temp710)
==  Cyc_Absyn_AnonStruct_e){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp710)->f1; goto _LL729; _LL729: _temp728=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp710)->f2; goto _LL717;} else{ goto _LL718;} _LL718: goto _LL719; _LL713:
_temp679= Cyc_Toc_init_array( nv, lval, _temp720, _temp679); goto _LL711; _LL715:
_temp679= Cyc_Toc_init_comprehension( nv, lval, _temp726, _temp724, _temp722,
_temp679, 0); goto _LL711; _LL717: _temp679= Cyc_Toc_init_anon_struct( nv, lval,
_temp730, _temp728, _temp679); goto _LL711; _LL719: Cyc_Toc_exp_to_c( nv,
_temp686); if( Cyc_Toc_is_poly_field( struct_type, _temp708)){ _temp686= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp686, 0);} _temp679= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp686, 0), 0), _temp679, 0); goto _LL711; _LL711:;}
goto _LL703;} _LL707:( int) _throw(({ void* _temp732[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp732, sizeof( void*), 0u));})); _LL703:;}}}{ struct Cyc_Absyn_Exp* _temp733=
Cyc_Toc_make_struct( nv, _temp677, Cyc_Absyn_strctq( tdn), _temp679, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp733;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp734=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp734[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp735; _temp735.tag= Cyc_Absyn_Increment_e;
_temp735.f1= e1; _temp735.f2=( void*) incr; _temp735;}); _temp734;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp736=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp746; struct Cyc_Absyn_Exp*
_temp748; void* _temp750; struct _tagged_arr* _temp752; struct Cyc_Absyn_Exp*
_temp754; _LL738: if(*(( int*) _temp736) ==  Cyc_Absyn_StmtExp_e){ _LL747:
_temp746=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp736)->f1; goto _LL739;}
else{ goto _LL740;} _LL740: if(*(( int*) _temp736) ==  Cyc_Absyn_Cast_e){ _LL751:
_temp750=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp736)->f1; goto _LL749;
_LL749: _temp748=(( struct Cyc_Absyn_Cast_e_struct*) _temp736)->f2; goto _LL741;}
else{ goto _LL742;} _LL742: if(*(( int*) _temp736) ==  Cyc_Absyn_StructMember_e){
_LL755: _temp754=(( struct Cyc_Absyn_StructMember_e_struct*) _temp736)->f1; goto
_LL753; _LL753: _temp752=(( struct Cyc_Absyn_StructMember_e_struct*) _temp736)->f2;
goto _LL743;} else{ goto _LL744;} _LL744: goto _LL745; _LL739: Cyc_Toc_lvalue_assign_stmt(
_temp746, fs, f, f_env); goto _LL737; _LL741: Cyc_Toc_lvalue_assign( _temp748,
fs, f, f_env); goto _LL737; _LL743:( void*)( e1->r=( void*)(( void*) _temp754->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp756=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp756->hd=( void*) _temp752; _temp756->tl= fs; _temp756;}), f, f_env); goto
_LL737; _LL745: if( ! Cyc_Absyn_is_lvalue( e1)){({ struct Cyc_Std_String_pa_struct
_temp758; _temp758.tag= Cyc_Std_String_pa; _temp758.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp757[ 1u]={& _temp758};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u), _tag_arr( _temp757, sizeof( void*), 1u));}});}{
struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs !=  0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd,
e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto
_LL737;} _LL737:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp759=( void*) s->r; struct Cyc_Absyn_Exp*
_temp769; struct Cyc_Absyn_Stmt* _temp771; struct Cyc_Absyn_Decl* _temp773;
struct Cyc_Absyn_Stmt* _temp775; _LL761: if(( unsigned int) _temp759 >  1u?*((
int*) _temp759) ==  Cyc_Absyn_Exp_s: 0){ _LL770: _temp769=(( struct Cyc_Absyn_Exp_s_struct*)
_temp759)->f1; goto _LL762;} else{ goto _LL763;} _LL763: if(( unsigned int)
_temp759 >  1u?*(( int*) _temp759) ==  Cyc_Absyn_Decl_s: 0){ _LL774: _temp773=((
struct Cyc_Absyn_Decl_s_struct*) _temp759)->f1; goto _LL772; _LL772: _temp771=((
struct Cyc_Absyn_Decl_s_struct*) _temp759)->f2; goto _LL764;} else{ goto _LL765;}
_LL765: if(( unsigned int) _temp759 >  1u?*(( int*) _temp759) ==  Cyc_Absyn_Seq_s:
0){ _LL776: _temp775=(( struct Cyc_Absyn_Seq_s_struct*) _temp759)->f2; goto
_LL766;} else{ goto _LL767;} _LL767: goto _LL768; _LL762: Cyc_Toc_lvalue_assign(
_temp769, fs, f, f_env); goto _LL760; _LL764: Cyc_Toc_lvalue_assign_stmt(
_temp771, fs, f, f_env); goto _LL760; _LL766: Cyc_Toc_lvalue_assign_stmt(
_temp775, fs, f, f_env); goto _LL760; _LL768:({ struct Cyc_Std_String_pa_struct
_temp778; _temp778.tag= Cyc_Std_String_pa; _temp778.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp777[ 1u]={& _temp778};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s", sizeof(
unsigned char), 23u), _tag_arr( _temp777, sizeof( void*), 1u));}}); goto _LL760;
_LL760:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2,
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x ==  0){ return 0;} result=({ struct
Cyc_List_List* _temp779=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp779->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp779->tl= 0; _temp779;}); prev= result; for( x=((
struct Cyc_List_List*) _check_null( x))->tl; x !=  0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp780=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp780->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp780->tl= 0; _temp780;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp781=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp781->f1= 0; _temp781->f2= e; _temp781;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp782=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp788; _LL784: if((
unsigned int) _temp782 >  4u?*(( int*) _temp782) ==  Cyc_Absyn_PointerType: 0){
_LL789: _temp788=(( struct Cyc_Absyn_PointerType_struct*) _temp782)->f1; goto
_LL785;} else{ goto _LL786;} _LL786: goto _LL787; _LL785: return _temp788;
_LL787:( int) _throw(({ void* _temp790[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("get_ptr_typ: not a pointer!",
sizeof( unsigned char), 28u), _tag_arr( _temp790, sizeof( void*), 0u));}));
_LL783:;} struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp791=( void*) e->r; if( e->topt ==  0){({
struct Cyc_Std_String_pa_struct _temp793; _temp793.tag= Cyc_Std_String_pa;
_temp793.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp792[ 1u]={&
_temp793};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u), _tag_arr(
_temp792, sizeof( void*), 1u));}}); return;}{ void* old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp794= _temp791; void*
_temp874; void* _temp876; struct _tuple0* _temp878; struct _tuple0* _temp880;
struct Cyc_List_List* _temp882; void* _temp884; void* _temp886; struct Cyc_Absyn_Exp*
_temp888; struct Cyc_Absyn_Exp* _temp890; struct Cyc_Core_Opt* _temp892; struct
Cyc_Absyn_Exp* _temp894; struct Cyc_Absyn_Exp* _temp896; struct Cyc_Absyn_Exp*
_temp898; struct Cyc_Absyn_Exp* _temp900; struct Cyc_Absyn_Exp* _temp902; struct
Cyc_Absyn_Exp* _temp904; struct Cyc_List_List* _temp906; struct Cyc_Absyn_Exp*
_temp908; struct Cyc_Absyn_VarargCallInfo* _temp910; struct Cyc_List_List*
_temp912; struct Cyc_Absyn_Exp* _temp914; struct Cyc_Absyn_VarargCallInfo*
_temp916; struct Cyc_Absyn_VarargCallInfo _temp918; struct Cyc_Absyn_VarargInfo*
_temp919; struct Cyc_List_List* _temp921; int _temp923; struct Cyc_List_List*
_temp925; struct Cyc_Absyn_Exp* _temp927; struct Cyc_Absyn_Exp* _temp929; struct
Cyc_Absyn_Exp* _temp931; struct Cyc_List_List* _temp933; struct Cyc_Absyn_Exp*
_temp935; struct Cyc_Absyn_Exp* _temp937; void* _temp939; void** _temp941;
struct Cyc_Absyn_Exp* _temp942; struct Cyc_Absyn_Exp* _temp944; struct Cyc_Absyn_Exp*
_temp946; struct Cyc_Absyn_Exp* _temp948; void* _temp950; struct _tagged_arr*
_temp952; void* _temp954; struct Cyc_Absyn_Exp* _temp956; struct _tagged_arr*
_temp958; struct Cyc_Absyn_Exp* _temp960; struct _tagged_arr* _temp962; struct
Cyc_Absyn_Exp* _temp964; struct Cyc_Absyn_Exp* _temp966; struct Cyc_Absyn_Exp*
_temp968; struct Cyc_List_List* _temp970; struct Cyc_List_List* _temp972; struct
Cyc_Absyn_Exp* _temp974; struct Cyc_Absyn_Exp* _temp976; struct Cyc_Absyn_Vardecl*
_temp978; struct Cyc_Absyn_Structdecl* _temp980; struct Cyc_List_List* _temp982;
struct Cyc_Core_Opt* _temp984; struct _tuple0* _temp986; struct Cyc_List_List*
_temp988; void* _temp990; struct Cyc_Absyn_Tunionfield* _temp992; struct Cyc_Absyn_Tuniondecl*
_temp994; struct Cyc_List_List* _temp996; struct Cyc_Absyn_Tunionfield* _temp998;
struct Cyc_Absyn_Tuniondecl* _temp1000; struct Cyc_List_List* _temp1002; void*
_temp1004; struct Cyc_Absyn_Exp* _temp1006; struct Cyc_Absyn_Stmt* _temp1008;
struct Cyc_Absyn_Fndecl* _temp1010; _LL796: if(*(( int*) _temp794) ==  Cyc_Absyn_Const_e){
_LL875: _temp874=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp794)->f1; if(
_temp874 == ( void*) Cyc_Absyn_Null_c){ goto _LL797;} else{ goto _LL798;}} else{
goto _LL798;} _LL798: if(*(( int*) _temp794) ==  Cyc_Absyn_Const_e){ goto _LL799;}
else{ goto _LL800;} _LL800: if(*(( int*) _temp794) ==  Cyc_Absyn_Var_e){ _LL879:
_temp878=(( struct Cyc_Absyn_Var_e_struct*) _temp794)->f1; goto _LL877; _LL877:
_temp876=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp794)->f2; goto _LL801;}
else{ goto _LL802;} _LL802: if(*(( int*) _temp794) ==  Cyc_Absyn_UnknownId_e){
_LL881: _temp880=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp794)->f1; goto
_LL803;} else{ goto _LL804;} _LL804: if(*(( int*) _temp794) ==  Cyc_Absyn_Primop_e){
_LL885: _temp884=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp794)->f1;
goto _LL883; _LL883: _temp882=(( struct Cyc_Absyn_Primop_e_struct*) _temp794)->f2;
goto _LL805;} else{ goto _LL806;} _LL806: if(*(( int*) _temp794) ==  Cyc_Absyn_Increment_e){
_LL889: _temp888=(( struct Cyc_Absyn_Increment_e_struct*) _temp794)->f1; goto
_LL887; _LL887: _temp886=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp794)->f2; goto _LL807;} else{ goto _LL808;} _LL808: if(*(( int*) _temp794)
==  Cyc_Absyn_AssignOp_e){ _LL895: _temp894=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp794)->f1; goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp794)->f2; goto _LL891; _LL891: _temp890=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp794)->f3; goto _LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp794)
==  Cyc_Absyn_Conditional_e){ _LL901: _temp900=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp794)->f1; goto _LL899; _LL899: _temp898=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp794)->f2; goto _LL897; _LL897: _temp896=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp794)->f3; goto _LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp794)
==  Cyc_Absyn_SeqExp_e){ _LL905: _temp904=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp794)->f1; goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp794)->f2; goto _LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp794)
==  Cyc_Absyn_UnknownCall_e){ _LL909: _temp908=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp794)->f1; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp794)->f2; goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp794)
==  Cyc_Absyn_FnCall_e){ _LL915: _temp914=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp794)->f1; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp794)->f2; goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp794)->f3; if( _temp910 ==  0){ goto _LL817;} else{ goto _LL818;}} else{
goto _LL818;} _LL818: if(*(( int*) _temp794) ==  Cyc_Absyn_FnCall_e){ _LL928:
_temp927=(( struct Cyc_Absyn_FnCall_e_struct*) _temp794)->f1; goto _LL926;
_LL926: _temp925=(( struct Cyc_Absyn_FnCall_e_struct*) _temp794)->f2; goto
_LL917; _LL917: _temp916=(( struct Cyc_Absyn_FnCall_e_struct*) _temp794)->f3;
if( _temp916 ==  0){ goto _LL820;} else{ _temp918=* _temp916; _LL924: _temp923=
_temp918.num_varargs; goto _LL922; _LL922: _temp921= _temp918.injectors; goto
_LL920; _LL920: _temp919= _temp918.vai; goto _LL819;}} else{ goto _LL820;}
_LL820: if(*(( int*) _temp794) ==  Cyc_Absyn_Throw_e){ _LL930: _temp929=((
struct Cyc_Absyn_Throw_e_struct*) _temp794)->f1; goto _LL821;} else{ goto _LL822;}
_LL822: if(*(( int*) _temp794) ==  Cyc_Absyn_NoInstantiate_e){ _LL932: _temp931=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp794)->f1; goto _LL823;} else{
goto _LL824;} _LL824: if(*(( int*) _temp794) ==  Cyc_Absyn_Instantiate_e){
_LL936: _temp935=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp794)->f1; goto
_LL934; _LL934: _temp933=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp794)->f2;
goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp794) ==  Cyc_Absyn_Cast_e){
_LL940: _temp939=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp794)->f1;
_temp941=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp794)->f1);
goto _LL938; _LL938: _temp937=(( struct Cyc_Absyn_Cast_e_struct*) _temp794)->f2;
goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp794) ==  Cyc_Absyn_Address_e){
_LL943: _temp942=(( struct Cyc_Absyn_Address_e_struct*) _temp794)->f1; goto
_LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp794) ==  Cyc_Absyn_New_e){
_LL947: _temp946=(( struct Cyc_Absyn_New_e_struct*) _temp794)->f1; goto _LL945;
_LL945: _temp944=(( struct Cyc_Absyn_New_e_struct*) _temp794)->f2; goto _LL831;}
else{ goto _LL832;} _LL832: if(*(( int*) _temp794) ==  Cyc_Absyn_Sizeofexp_e){
_LL949: _temp948=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp794)->f1; goto
_LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp794) ==  Cyc_Absyn_Sizeoftyp_e){
_LL951: _temp950=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp794)->f1;
goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp794) ==  Cyc_Absyn_Offsetof_e){
_LL955: _temp954=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp794)->f1;
goto _LL953; _LL953: _temp952=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp794)->f2;
goto _LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp794) ==  Cyc_Absyn_Deref_e){
_LL957: _temp956=(( struct Cyc_Absyn_Deref_e_struct*) _temp794)->f1; goto _LL839;}
else{ goto _LL840;} _LL840: if(*(( int*) _temp794) ==  Cyc_Absyn_StructMember_e){
_LL961: _temp960=(( struct Cyc_Absyn_StructMember_e_struct*) _temp794)->f1; goto
_LL959; _LL959: _temp958=(( struct Cyc_Absyn_StructMember_e_struct*) _temp794)->f2;
goto _LL841;} else{ goto _LL842;} _LL842: if(*(( int*) _temp794) ==  Cyc_Absyn_StructArrow_e){
_LL965: _temp964=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp794)->f1; goto
_LL963; _LL963: _temp962=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp794)->f2;
goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp794) ==  Cyc_Absyn_Subscript_e){
_LL969: _temp968=(( struct Cyc_Absyn_Subscript_e_struct*) _temp794)->f1; goto
_LL967; _LL967: _temp966=(( struct Cyc_Absyn_Subscript_e_struct*) _temp794)->f2;
goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp794) ==  Cyc_Absyn_Tuple_e){
_LL971: _temp970=(( struct Cyc_Absyn_Tuple_e_struct*) _temp794)->f1; goto _LL847;}
else{ goto _LL848;} _LL848: if(*(( int*) _temp794) ==  Cyc_Absyn_Array_e){
_LL973: _temp972=(( struct Cyc_Absyn_Array_e_struct*) _temp794)->f1; goto _LL849;}
else{ goto _LL850;} _LL850: if(*(( int*) _temp794) ==  Cyc_Absyn_Comprehension_e){
_LL979: _temp978=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp794)->f1;
goto _LL977; _LL977: _temp976=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp794)->f2; goto _LL975; _LL975: _temp974=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp794)->f3; goto _LL851;} else{ goto _LL852;} _LL852: if(*(( int*) _temp794)
==  Cyc_Absyn_Struct_e){ _LL987: _temp986=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f1; goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f2; goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f3; goto _LL981; _LL981: _temp980=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f4; goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp794)
==  Cyc_Absyn_AnonStruct_e){ _LL991: _temp990=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp794)->f1; goto _LL989; _LL989: _temp988=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp794)->f2; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp794)
==  Cyc_Absyn_Tunion_e){ _LL997: _temp996=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp794)->f3; if( _temp996 ==  0){ goto _LL995;} else{ goto _LL858;} _LL995:
_temp994=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f4; goto _LL993;
_LL993: _temp992=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f5; goto
_LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp794) ==  Cyc_Absyn_Tunion_e){
_LL1003: _temp1002=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f3; goto
_LL1001; _LL1001: _temp1000=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f4;
goto _LL999; _LL999: _temp998=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f5;
goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp794) ==  Cyc_Absyn_Enum_e){
goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp794) ==  Cyc_Absyn_Malloc_e){
_LL1007: _temp1006=(( struct Cyc_Absyn_Malloc_e_struct*) _temp794)->f1; goto
_LL1005; _LL1005: _temp1004=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp794)->f2; goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp794)
==  Cyc_Absyn_StmtExp_e){ _LL1009: _temp1008=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp794)->f1; goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp794)
==  Cyc_Absyn_UnresolvedMem_e){ goto _LL867;} else{ goto _LL868;} _LL868: if(*((
int*) _temp794) ==  Cyc_Absyn_CompoundLit_e){ goto _LL869;} else{ goto _LL870;}
_LL870: if(*(( int*) _temp794) ==  Cyc_Absyn_Codegen_e){ _LL1011: _temp1010=((
struct Cyc_Absyn_Codegen_e_struct*) _temp794)->f1; goto _LL871;} else{ goto
_LL872;} _LL872: if(*(( int*) _temp794) ==  Cyc_Absyn_Fill_e){ goto _LL873;}
else{ goto _LL795;} _LL797: { struct Cyc_Absyn_Exp* _temp1012= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1012,
_temp1012))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1013[ 3u]; _temp1013[ 2u]=
_temp1012; _temp1013[ 1u]= _temp1012; _temp1013[ 0u]= _temp1012;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1013, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL795;} _LL799: goto _LL795; _LL801:{ struct _handler_cons
_temp1014; _push_handler(& _temp1014);{ int _temp1016= 0; if( setjmp( _temp1014.handler)){
_temp1016= 1;} if( ! _temp1016){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp878))->r));; _pop_handler();} else{ void* _temp1015=( void*) _exn_thrown;
void* _temp1018= _temp1015; _LL1020: if( _temp1018 ==  Cyc_Dict_Absent){ goto
_LL1021;} else{ goto _LL1022;} _LL1022: goto _LL1023; _LL1021:({ struct Cyc_Std_String_pa_struct
_temp1025; _temp1025.tag= Cyc_Std_String_pa; _temp1025.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp878);{ void* _temp1024[ 1u]={& _temp1025};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1024, sizeof( void*), 1u));}});
return; _LL1023:( void) _throw( _temp1018); _LL1019:;}}} goto _LL795; _LL803:({
void* _temp1026[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1026, sizeof( void*), 0u));}); goto _LL795; _LL805: { struct Cyc_List_List*
_temp1027=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp882);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp882);{
void* _temp1028= _temp884; _LL1030: if( _temp1028 == ( void*) Cyc_Absyn_Size){
goto _LL1031;} else{ goto _LL1032;} _LL1032: if( _temp1028 == ( void*) Cyc_Absyn_Plus){
goto _LL1033;} else{ goto _LL1034;} _LL1034: if( _temp1028 == ( void*) Cyc_Absyn_Minus){
goto _LL1035;} else{ goto _LL1036;} _LL1036: if( _temp1028 == ( void*) Cyc_Absyn_Eq){
goto _LL1037;} else{ goto _LL1038;} _LL1038: if( _temp1028 == ( void*) Cyc_Absyn_Neq){
goto _LL1039;} else{ goto _LL1040;} _LL1040: if( _temp1028 == ( void*) Cyc_Absyn_Gt){
goto _LL1041;} else{ goto _LL1042;} _LL1042: if( _temp1028 == ( void*) Cyc_Absyn_Gte){
goto _LL1043;} else{ goto _LL1044;} _LL1044: if( _temp1028 == ( void*) Cyc_Absyn_Lt){
goto _LL1045;} else{ goto _LL1046;} _LL1046: if( _temp1028 == ( void*) Cyc_Absyn_Lte){
goto _LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049; _LL1031: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp882))->hd;{
void* _temp1050= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1058; struct Cyc_Absyn_PtrInfo
_temp1060; struct Cyc_Absyn_Conref* _temp1062; void* _temp1064; _LL1052: if((
unsigned int) _temp1050 >  4u?*(( int*) _temp1050) ==  Cyc_Absyn_ArrayType: 0){
_LL1059: _temp1058=(( struct Cyc_Absyn_ArrayType_struct*) _temp1050)->f3; goto
_LL1053;} else{ goto _LL1054;} _LL1054: if(( unsigned int) _temp1050 >  4u?*((
int*) _temp1050) ==  Cyc_Absyn_PointerType: 0){ _LL1061: _temp1060=(( struct Cyc_Absyn_PointerType_struct*)
_temp1050)->f1; _LL1065: _temp1064=( void*) _temp1060.elt_typ; goto _LL1063;
_LL1063: _temp1062= _temp1060.bounds; goto _LL1055;} else{ goto _LL1056;}
_LL1056: goto _LL1057; _LL1053:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1058))->r)); goto _LL1051; _LL1055:{ void* _temp1066= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1062); struct Cyc_Absyn_Exp* _temp1072; _LL1068: if(
_temp1066 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1069;} else{ goto _LL1070;}
_LL1070: if(( unsigned int) _temp1066 >  1u?*(( int*) _temp1066) ==  Cyc_Absyn_Upper_b:
0){ _LL1073: _temp1072=(( struct Cyc_Absyn_Upper_b_struct*) _temp1066)->f1; goto
_LL1071;} else{ goto _LL1067;} _LL1069:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1074[ 2u]; _temp1074[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1064), 0); _temp1074[ 0u]=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp882))->hd;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1074, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r)); goto _LL1067;
_LL1071:( void*)( e->r=( void*)(( void*) _temp1072->r)); goto _LL1067; _LL1067:;}
goto _LL1051; _LL1057:({ struct Cyc_Std_String_pa_struct _temp1077; _temp1077.tag=
Cyc_Std_String_pa; _temp1077.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp1076; _temp1076.tag= Cyc_Std_String_pa; _temp1076.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
void* _temp1075[ 2u]={& _temp1076,& _temp1077};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u), _tag_arr( _temp1075, sizeof( void*), 2u));}}});
return; _LL1051:;} goto _LL1029;} _LL1033: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1027))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp882))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp882))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1078[ 3u]; _temp1078[ 2u]=
e2; _temp1078[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1078[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1078, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1029;} _LL1035: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1027))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp882))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp882))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1027))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1079[ 3u]; _temp1079[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1079[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1079[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1079, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1029;} _LL1037: goto _LL1039; _LL1039: goto _LL1041; _LL1041: goto
_LL1043; _LL1043: goto _LL1045; _LL1045: goto _LL1047; _LL1047: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp882))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp882))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1027))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1027))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1029;} _LL1049: goto
_LL1029; _LL1029:;} goto _LL795;} _LL807: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp888->topt))->v; Cyc_Toc_exp_to_c( nv, _temp888);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1080=( _temp886 == ( void*) Cyc_Absyn_PostInc?
1: _temp886 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1081=( _temp886 == ( void*) Cyc_Absyn_PreInc?
1: _temp886 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1080,({ struct Cyc_Absyn_Exp* _temp1082[ 3u];
_temp1082[ 2u]= Cyc_Absyn_signed_int_exp( _temp1081, 0); _temp1082[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1082[ 0u]= Cyc_Absyn_address_exp( _temp888,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1082, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp888): 0){ Cyc_Toc_lvalue_assign(
_temp888, 0, Cyc_Toc_incr_lvalue, _temp886);( void*)( e->r=( void*)(( void*)
_temp888->r));} goto _LL795;}} _LL809: { int e1_poly= Cyc_Toc_is_poly_project(
_temp894); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp894->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp890->topt))->v; Cyc_Toc_exp_to_c( nv, _temp894); Cyc_Toc_exp_to_c(
nv, _temp890);{ int done= 0; if( _temp892 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1083=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp892))->v;
_LL1085: if( _temp1083 == ( void*) Cyc_Absyn_Plus){ goto _LL1086;} else{ goto
_LL1087;} _LL1087: if( _temp1083 == ( void*) Cyc_Absyn_Minus){ goto _LL1088;}
else{ goto _LL1089;} _LL1089: goto _LL1090; _LL1086: change= _temp890; goto
_LL1084; _LL1088: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp890,
0); goto _LL1084; _LL1090:( int) _throw(({ void* _temp1091[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u), _tag_arr(
_temp1091, sizeof( void*), 0u));})); goto _LL1084; _LL1084:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp* _temp1092[ 3u]; _temp1092[ 2u]= change; _temp1092[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); _temp1092[ 0u]= Cyc_Absyn_address_exp(
_temp894, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1092, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} if( !
done){ if( e1_poly){( void*)( _temp890->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*) _temp890->r, 0), 0))->r));}
if( ! Cyc_Absyn_is_lvalue( _temp894)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)( _temp894, 0,
Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1093=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7) *  1); _temp1093[ 0]=({ struct _tuple7
_temp1094; _temp1094.f1= _temp892; _temp1094.f2= _temp890; _temp1094;});
_temp1093;}));( void*)( e->r=( void*)(( void*) _temp894->r));}} goto _LL795;}}
_LL811: Cyc_Toc_exp_to_c( nv, _temp900); Cyc_Toc_exp_to_c( nv, _temp898); Cyc_Toc_exp_to_c(
nv, _temp896); goto _LL795; _LL813: Cyc_Toc_exp_to_c( nv, _temp904); Cyc_Toc_exp_to_c(
nv, _temp902); goto _LL795; _LL815: _temp914= _temp908; _temp912= _temp906; goto
_LL817; _LL817: Cyc_Toc_exp_to_c( nv, _temp914);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp912); goto _LL795; _LL819:{
struct _RegionHandle _temp1095= _new_region(); struct _RegionHandle* r=&
_temp1095; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp923, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp919->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1112=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1112[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1113; _temp1113.tag= Cyc_Absyn_ArrayType;
_temp1113.f1=( void*) cva_type; _temp1113.f2= Cyc_Toc_mt_tq; _temp1113.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1113;}); _temp1112;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp925); int num_normargs=
num_args -  _temp923; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp925=(( struct Cyc_List_List*) _check_null( _temp925))->tl)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp925))->hd); new_args=({ struct Cyc_List_List* _temp1096=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1096->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp925))->hd);
_temp1096->tl= new_args; _temp1096;});}} new_args=({ struct Cyc_List_List*
_temp1097=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1097->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1098[ 3u]; _temp1098[ 2u]= num_varargs_exp; _temp1098[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1098[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1098, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1097->tl= new_args; _temp1097;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp927);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp927, new_args, 0), 0); if( _temp919->inject){ struct _tagged_arr vs=({
unsigned int _temp1108=( unsigned int) _temp923; struct _tuple0** _temp1109=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1108)); struct _tagged_arr _temp1111= _tag_arr( _temp1109, sizeof( struct
_tuple0*),( unsigned int) _temp923);{ unsigned int _temp1110= _temp1108;
unsigned int i; for( i= 0; i <  _temp1110; i ++){ _temp1109[ i]= Cyc_Toc_temp_var();}};
_temp1111;}); struct Cyc_List_List* _temp1099= 0;{ int i= _temp923 -  1; for( 0;
i >=  0; -- i){ _temp1099=({ struct Cyc_List_List* _temp1100=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1100->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1100->tl=
_temp1099; _temp1100;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1099, 0), s, 0);{ int i= 0;
for( 0; _temp925 !=  0;((( _temp925=(( struct Cyc_List_List*) _check_null(
_temp925))->tl, _temp921=(( struct Cyc_List_List*) _check_null( _temp921))->tl)),
++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp925))->hd; void* arg_type=( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v; struct _tuple0* var=*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)); struct Cyc_Absyn_Exp*
varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield _temp1103;
struct Cyc_List_List* _temp1104; struct _tuple0* _temp1106; struct Cyc_Absyn_Tunionfield*
_temp1101=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp921))->hd; _temp1103=* _temp1101; _LL1107: _temp1106= _temp1103.name; goto
_LL1105; _LL1105: _temp1104= _temp1103.typs; goto _LL1102; _LL1102: { void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1104))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Absyn_fieldname(
1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1106, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1106, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp925
!=  0;( _temp925=(( struct Cyc_List_List*) _check_null( _temp925))->tl, ++ i)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp925))->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp925))->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL795; _LL821: Cyc_Toc_exp_to_c( nv,
_temp929);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp929), 0))->r)); goto _LL795; _LL823: Cyc_Toc_exp_to_c(
nv, _temp931); goto _LL795; _LL825: Cyc_Toc_exp_to_c( nv, _temp935); for( 0;
_temp933 !=  0; _temp933=(( struct Cyc_List_List*) _check_null( _temp933))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp933))->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1114= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp933))->hd); _LL1116: if(( unsigned int) _temp1114 >  4u?*((
int*) _temp1114) ==  Cyc_Absyn_VarType: 0){ goto _LL1117;} else{ goto _LL1118;}
_LL1118: if(( unsigned int) _temp1114 >  4u?*(( int*) _temp1114) ==  Cyc_Absyn_TunionType:
0){ goto _LL1119;} else{ goto _LL1120;} _LL1120: goto _LL1121; _LL1117:
continue; _LL1119: continue; _LL1121:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp935, 0))->r)); goto _LL1115; _LL1115:;} break;}} goto _LL795; _LL827: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp937->topt))->v;
void* new_typ=* _temp941;* _temp941= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp937);{ struct _tuple8 _temp1123=({ struct _tuple8 _temp1122; _temp1122.f1=
Cyc_Tcutil_compress( old_t2); _temp1122.f2= Cyc_Tcutil_compress( new_typ);
_temp1122;}); void* _temp1131; struct Cyc_Absyn_PtrInfo _temp1133; void*
_temp1135; struct Cyc_Absyn_PtrInfo _temp1137; void* _temp1139; void* _temp1141;
struct Cyc_Absyn_PtrInfo _temp1143; _LL1125: _LL1136: _temp1135= _temp1123.f1;
if(( unsigned int) _temp1135 >  4u?*(( int*) _temp1135) ==  Cyc_Absyn_PointerType:
0){ _LL1138: _temp1137=(( struct Cyc_Absyn_PointerType_struct*) _temp1135)->f1;
goto _LL1132;} else{ goto _LL1127;} _LL1132: _temp1131= _temp1123.f2; if((
unsigned int) _temp1131 >  4u?*(( int*) _temp1131) ==  Cyc_Absyn_PointerType: 0){
_LL1134: _temp1133=(( struct Cyc_Absyn_PointerType_struct*) _temp1131)->f1; goto
_LL1126;} else{ goto _LL1127;} _LL1127: _LL1142: _temp1141= _temp1123.f1; if((
unsigned int) _temp1141 >  4u?*(( int*) _temp1141) ==  Cyc_Absyn_PointerType: 0){
_LL1144: _temp1143=(( struct Cyc_Absyn_PointerType_struct*) _temp1141)->f1; goto
_LL1140;} else{ goto _LL1129;} _LL1140: _temp1139= _temp1123.f2; if((
unsigned int) _temp1139 >  4u?*(( int*) _temp1139) ==  Cyc_Absyn_IntType: 0){
goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130; _LL1126: { int
_temp1145=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1137.nullable); int _temp1146=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1133.nullable); void* _temp1147= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1137.bounds); void* _temp1148= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1133.bounds);{ struct _tuple8 _temp1150=({ struct
_tuple8 _temp1149; _temp1149.f1= _temp1147; _temp1149.f2= _temp1148; _temp1149;});
void* _temp1160; void* _temp1162; void* _temp1164; void* _temp1166; struct Cyc_Absyn_Exp*
_temp1168; void* _temp1170; struct Cyc_Absyn_Exp* _temp1172; void* _temp1174;
_LL1152: _LL1163: _temp1162= _temp1150.f1; if(( unsigned int) _temp1162 >  1u?*((
int*) _temp1162) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1161;} else{ goto _LL1154;}
_LL1161: _temp1160= _temp1150.f2; if(( unsigned int) _temp1160 >  1u?*(( int*)
_temp1160) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1153;} else{ goto _LL1154;}
_LL1154: _LL1167: _temp1166= _temp1150.f1; if(( unsigned int) _temp1166 >  1u?*((
int*) _temp1166) ==  Cyc_Absyn_Upper_b: 0){ _LL1169: _temp1168=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1166)->f1; goto _LL1165;} else{ goto _LL1156;} _LL1165: _temp1164=
_temp1150.f2; if( _temp1164 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1155;}
else{ goto _LL1156;} _LL1156: _LL1175: _temp1174= _temp1150.f1; if( _temp1174 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1171;} else{ goto _LL1158;} _LL1171:
_temp1170= _temp1150.f2; if(( unsigned int) _temp1170 >  1u?*(( int*) _temp1170)
==  Cyc_Absyn_Upper_b: 0){ _LL1173: _temp1172=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1170)->f1; goto _LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159;
_LL1153: if( _temp1145? ! _temp1146: 0){ if( nv->toplevel){({ void* _temp1176[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1176, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp941, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1177=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1177->hd=( void*) _temp937;
_temp1177->tl= 0; _temp1177;}), 0), 0))->r));} goto _LL1151; _LL1155: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1168, _temp937))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1178[ 3u]; _temp1178[ 2u]=
_temp1168; _temp1178[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1133.elt_typ), 0); _temp1178[ 0u]= _temp937;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1178, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1151; _LL1157: if( nv->toplevel){({ void* _temp1179[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1179, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1180= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1182[ 3u]; _temp1182[ 2u]= _temp1172; _temp1182[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1137.elt_typ), 0); _temp1182[ 0u]= _temp937;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1182, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1146){( void*)(
_temp1180->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1181=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1181->hd=( void*) Cyc_Absyn_copy_exp( _temp1180);
_temp1181->tl= 0; _temp1181;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp941, _temp1180, 0))->r)); goto _LL1151;} _LL1159: goto _LL1151; _LL1151:;}
goto _LL1124;} _LL1128:{ void* _temp1183= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1143.bounds); _LL1185: if( _temp1183 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1186;} else{ goto _LL1187;} _LL1187: goto _LL1188; _LL1186:( void*)( _temp937->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp937->r,
_temp937->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1184; _LL1188: goto _LL1184;
_LL1184:;} goto _LL1124; _LL1130: goto _LL1124; _LL1124:;} goto _LL795;} _LL829:{
void* _temp1189=( void*) _temp942->r; struct Cyc_List_List* _temp1197; struct
_tuple0* _temp1199; struct Cyc_List_List* _temp1201; _LL1191: if(*(( int*)
_temp1189) ==  Cyc_Absyn_Struct_e){ _LL1200: _temp1199=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1189)->f1; goto _LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1189)->f3; goto _LL1192;} else{ goto _LL1193;} _LL1193: if(*(( int*)
_temp1189) ==  Cyc_Absyn_Tuple_e){ _LL1202: _temp1201=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1189)->f1; goto _LL1194;} else{ goto _LL1195;} _LL1195: goto _LL1196;
_LL1192: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1204;
_temp1204.tag= Cyc_Std_String_pa; _temp1204.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp942->loc);{ void* _temp1203[ 1u]={& _temp1204};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1203, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp942->topt))->v, 1, 0, _temp1197, _temp1199))->r)); goto
_LL1190; _LL1194: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1206;
_temp1206.tag= Cyc_Std_String_pa; _temp1206.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp942->loc);{ void* _temp1205[ 1u]={& _temp1206};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1205, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1201))->r));
goto _LL1190; _LL1196: Cyc_Toc_exp_to_c( nv, _temp942); if( ! Cyc_Absyn_is_lvalue(
_temp942)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp942, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp942, 0))->r));} goto _LL1190; _LL1190:;} goto _LL795; _LL831: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1208; _temp1208.tag= Cyc_Std_String_pa;
_temp1208.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp944->loc);{
void* _temp1207[ 1u]={& _temp1208};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1207, sizeof( void*), 1u));}});}{ void*
_temp1209=( void*) _temp944->r; struct Cyc_List_List* _temp1221; struct Cyc_Absyn_Exp*
_temp1223; struct Cyc_Absyn_Exp* _temp1225; struct Cyc_Absyn_Vardecl* _temp1227;
struct Cyc_List_List* _temp1229; struct _tuple0* _temp1231; struct Cyc_List_List*
_temp1233; _LL1211: if(*(( int*) _temp1209) ==  Cyc_Absyn_Array_e){ _LL1222:
_temp1221=(( struct Cyc_Absyn_Array_e_struct*) _temp1209)->f1; goto _LL1212;}
else{ goto _LL1213;} _LL1213: if(*(( int*) _temp1209) ==  Cyc_Absyn_Comprehension_e){
_LL1228: _temp1227=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1209)->f1;
goto _LL1226; _LL1226: _temp1225=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1209)->f2; goto _LL1224; _LL1224: _temp1223=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1209)->f3; goto _LL1214;} else{ goto _LL1215;} _LL1215: if(*(( int*)
_temp1209) ==  Cyc_Absyn_Struct_e){ _LL1232: _temp1231=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1209)->f1; goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1209)->f3; goto _LL1216;} else{ goto _LL1217;} _LL1217: if(*(( int*)
_temp1209) ==  Cyc_Absyn_Tuple_e){ _LL1234: _temp1233=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1209)->f1; goto _LL1218;} else{ goto _LL1219;} _LL1219: goto _LL1220;
_LL1212: { struct _tuple0* _temp1235= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1236= Cyc_Absyn_var_exp( _temp1235, 0); struct Cyc_Absyn_Stmt* _temp1237=
Cyc_Toc_init_array( nv, _temp1236, _temp1221, Cyc_Absyn_exp_stmt( _temp1236, 0));
void* old_elt_typ;{ void* _temp1238= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1244; struct Cyc_Absyn_Tqual _temp1246; void* _temp1248; _LL1240: if((
unsigned int) _temp1238 >  4u?*(( int*) _temp1238) ==  Cyc_Absyn_PointerType: 0){
_LL1245: _temp1244=(( struct Cyc_Absyn_PointerType_struct*) _temp1238)->f1;
_LL1249: _temp1248=( void*) _temp1244.elt_typ; goto _LL1247; _LL1247: _temp1246=
_temp1244.tq; goto _LL1241;} else{ goto _LL1242;} _LL1242: goto _LL1243; _LL1241:
old_elt_typ= _temp1248; goto _LL1239; _LL1243: old_elt_typ=({ void* _temp1250[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1250, sizeof( void*), 0u));}); goto
_LL1239; _LL1239:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1251= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1252= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1221), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp946 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1252);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp946); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1252);}{
struct Cyc_Absyn_Exp* _temp1253= Cyc_Absyn_cast_exp( _temp1251, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1235,
_temp1251,( struct Cyc_Absyn_Exp*) _temp1253, _temp1237, 0), 0))->r)); goto
_LL1210;}}} _LL1214: { int is_tagged_ptr= 0;{ void* _temp1254= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1260; struct Cyc_Absyn_Conref* _temp1262;
struct Cyc_Absyn_Tqual _temp1264; void* _temp1266; _LL1256: if(( unsigned int)
_temp1254 >  4u?*(( int*) _temp1254) ==  Cyc_Absyn_PointerType: 0){ _LL1261:
_temp1260=(( struct Cyc_Absyn_PointerType_struct*) _temp1254)->f1; _LL1267:
_temp1266=( void*) _temp1260.elt_typ; goto _LL1265; _LL1265: _temp1264=
_temp1260.tq; goto _LL1263; _LL1263: _temp1262= _temp1260.bounds; goto _LL1257;}
else{ goto _LL1258;} _LL1258: goto _LL1259; _LL1257: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1262) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1255;
_LL1259:({ void* _temp1268[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1268, sizeof( void*), 0u));}); goto
_LL1255; _LL1255:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1223->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1225);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1227, Cyc_Absyn_var_exp( max, 0), _temp1223, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1269= _new_region(); struct _RegionHandle* r=&
_temp1269; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1289=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1289->hd=( void*)({ struct _tuple9* _temp1290=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1290->f1= max; _temp1290->f2=
Cyc_Absyn_uint_t; _temp1290->f3=( struct Cyc_Absyn_Exp*) _temp1225; _temp1290;});
_temp1289->tl= 0; _temp1289;}); struct Cyc_Absyn_Exp* ai; if( _temp946 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1270[ 2u]; _temp1270[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1270[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1270, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp946); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1271[
2u]; _temp1271[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1271[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1271, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1272=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1272->hd=( void*)({ struct _tuple9* _temp1273=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1273->f1= a; _temp1273->f2=
ptr_typ; _temp1273->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1273;}); _temp1272->tl=
decls; _temp1272;}); if( is_tagged_ptr){ struct _tuple0* _temp1274= Cyc_Toc_temp_var();
void* _temp1275= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1276=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1279[ 3u];
_temp1279[ 2u]= _temp1225; _temp1279[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1279[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1279, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1277=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1277->hd=( void*)({
struct _tuple9* _temp1278=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1278->f1= _temp1274; _temp1278->f2= _temp1275; _temp1278->f3=(
struct Cyc_Absyn_Exp*) _temp1276; _temp1278;}); _temp1277->tl= decls; _temp1277;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1274, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1280= decls; for( 0; _temp1280 !=  0;
_temp1280=(( struct Cyc_List_List*) _check_null( _temp1280))->tl){ struct Cyc_Absyn_Exp*
_temp1283; void* _temp1285; struct _tuple0* _temp1287; struct _tuple9 _temp1281=*((
struct _tuple9*)(( struct Cyc_List_List*) _check_null( _temp1280))->hd); _LL1288:
_temp1287= _temp1281.f1; goto _LL1286; _LL1286: _temp1285= _temp1281.f2; goto
_LL1284; _LL1284: _temp1283= _temp1281.f3; goto _LL1282; _LL1282: s= Cyc_Absyn_declare_stmt(
_temp1287, _temp1285, _temp1283, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1210;}}} _LL1216:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp944->topt))->v, 1, _temp946, _temp1229, _temp1231))->r)); goto _LL1210;
_LL1218:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp946,
_temp1233))->r)); goto _LL1210; _LL1220: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp944->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp946 ==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp946); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1291=( void*)
_temp944->r; struct Cyc_Absyn_Exp* _temp1297; void* _temp1299; _LL1293: if(*((
int*) _temp1291) ==  Cyc_Absyn_Cast_e){ _LL1300: _temp1299=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1291)->f1; goto _LL1298; _LL1298: _temp1297=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1291)->f2; goto _LL1294;} else{ goto _LL1295;} _LL1295: goto _LL1296;
_LL1294:{ struct _tuple8 _temp1302=({ struct _tuple8 _temp1301; _temp1301.f1=
Cyc_Tcutil_compress( _temp1299); _temp1301.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1297->topt))->v); _temp1301;}); void*
_temp1308; struct Cyc_Absyn_PtrInfo _temp1310; struct Cyc_Absyn_Conref*
_temp1312; void* _temp1314; struct Cyc_Absyn_PtrInfo _temp1316; struct Cyc_Absyn_Conref*
_temp1318; void* _temp1320; _LL1304: _LL1315: _temp1314= _temp1302.f1; if((
unsigned int) _temp1314 >  4u?*(( int*) _temp1314) ==  Cyc_Absyn_PointerType: 0){
_LL1317: _temp1316=(( struct Cyc_Absyn_PointerType_struct*) _temp1314)->f1;
_LL1321: _temp1320=( void*) _temp1316.elt_typ; goto _LL1319; _LL1319: _temp1318=
_temp1316.bounds; goto _LL1309;} else{ goto _LL1306;} _LL1309: _temp1308=
_temp1302.f2; if(( unsigned int) _temp1308 >  4u?*(( int*) _temp1308) ==  Cyc_Absyn_PointerType:
0){ _LL1311: _temp1310=(( struct Cyc_Absyn_PointerType_struct*) _temp1308)->f1;
_LL1313: _temp1312= _temp1310.bounds; goto _LL1305;} else{ goto _LL1306;}
_LL1306: goto _LL1307; _LL1305:{ struct _tuple8 _temp1323=({ struct _tuple8
_temp1322; _temp1322.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1318);
_temp1322.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1312); _temp1322;});
void* _temp1329; struct Cyc_Absyn_Exp* _temp1331; void* _temp1333; _LL1325:
_LL1334: _temp1333= _temp1323.f1; if( _temp1333 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1330;} else{ goto _LL1327;} _LL1330: _temp1329= _temp1323.f2; if((
unsigned int) _temp1329 >  1u?*(( int*) _temp1329) ==  Cyc_Absyn_Upper_b: 0){
_LL1332: _temp1331=(( struct Cyc_Absyn_Upper_b_struct*) _temp1329)->f1; goto
_LL1326;} else{ goto _LL1327;} _LL1327: goto _LL1328; _LL1326: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1335[ 4u]; _temp1335[ 3u]= _temp1331; _temp1335[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1320, 0); _temp1335[ 1u]= _temp1297; _temp1335[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1335, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1324; _LL1328: goto _LL1324; _LL1324:;} goto _LL1303;
_LL1307: goto _LL1303; _LL1303:;} goto _LL1292; _LL1296: goto _LL1292; _LL1292:;}
if( ! done){ struct _tuple0* _temp1336= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1337= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1336, 0), 0); struct Cyc_Absyn_Exp*
_temp1338= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp944);
_temp1337= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1336, 0), _temp1338, 0), _temp944, 0), _temp1337, 0);{
void* _temp1339= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1336,
_temp1339,( struct Cyc_Absyn_Exp*) mexp, _temp1337, 0), 0))->r));}} goto _LL1210;}}
_LL1210:;} goto _LL795; _LL833: Cyc_Toc_exp_to_c( nv, _temp948); goto _LL795;
_LL835:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1340=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1340[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1341; _temp1341.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1341.f1=( void*) Cyc_Toc_typ_to_c_array( _temp950);
_temp1341;}); _temp1340;}))); goto _LL795; _LL837:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1342=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1342[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1343; _temp1343.tag= Cyc_Absyn_Offsetof_e;
_temp1343.f1=( void*) Cyc_Toc_typ_to_c_array( _temp954); _temp1343.f2= _temp952;
_temp1343;}); _temp1342;}))); goto _LL795; _LL839: { void* _temp1344= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp956->topt))->v);{ void*
_temp1345= _temp1344; struct Cyc_Absyn_PtrInfo _temp1351; struct Cyc_Absyn_Conref*
_temp1353; struct Cyc_Absyn_Tqual _temp1355; struct Cyc_Absyn_Conref* _temp1357;
void* _temp1359; void* _temp1361; _LL1347: if(( unsigned int) _temp1345 >  4u?*((
int*) _temp1345) ==  Cyc_Absyn_PointerType: 0){ _LL1352: _temp1351=(( struct Cyc_Absyn_PointerType_struct*)
_temp1345)->f1; _LL1362: _temp1361=( void*) _temp1351.elt_typ; goto _LL1360;
_LL1360: _temp1359=( void*) _temp1351.rgn_typ; goto _LL1358; _LL1358: _temp1357=
_temp1351.nullable; goto _LL1356; _LL1356: _temp1355= _temp1351.tq; goto _LL1354;
_LL1354: _temp1353= _temp1351.bounds; goto _LL1348;} else{ goto _LL1349;}
_LL1349: goto _LL1350; _LL1348:{ void* _temp1363= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1353); _LL1365: if(( unsigned int) _temp1363 >  1u?*(( int*) _temp1363) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1366;} else{ goto _LL1367;} _LL1367: if(
_temp1363 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1368;} else{ goto _LL1364;}
_LL1366: Cyc_Toc_exp_to_c( nv, _temp956); if( Cyc_Toc_is_nullable( _temp1344)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp956->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1369=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1369->hd=( void*) _temp956;
_temp1369->tl= 0; _temp1369;}), 0), 0), 0))->r));} goto _LL1364; _LL1368: {
struct Cyc_Absyn_Exp* _temp1370= Cyc_Absyn_uint_exp( 0, 0); _temp1370->topt=({
struct Cyc_Core_Opt* _temp1371=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1371->v=( void*) Cyc_Absyn_uint_t; _temp1371;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp956, _temp1370, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1364;} _LL1364:;} goto _LL1346; _LL1350:({ void* _temp1372[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1372, sizeof( void*), 0u));}); goto _LL1346; _LL1346:;} goto _LL795;}
_LL841: Cyc_Toc_exp_to_c( nv, _temp960); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL795; _LL843: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp964->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp964);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1375; struct Cyc_Absyn_Tqual _temp1377; struct Cyc_Absyn_Conref* _temp1379;
void* _temp1381; void* _temp1383; struct Cyc_Absyn_PtrInfo _temp1373= Cyc_Toc_get_ptr_type(
e1typ); _LL1384: _temp1383=( void*) _temp1373.elt_typ; goto _LL1382; _LL1382:
_temp1381=( void*) _temp1373.rgn_typ; goto _LL1380; _LL1380: _temp1379=
_temp1373.nullable; goto _LL1378; _LL1378: _temp1377= _temp1373.tq; goto _LL1376;
_LL1376: _temp1375= _temp1373.bounds; goto _LL1374; _LL1374:{ void* _temp1385=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1375); struct Cyc_Absyn_Exp*
_temp1391; _LL1387: if(( unsigned int) _temp1385 >  1u?*(( int*) _temp1385) == 
Cyc_Absyn_Upper_b: 0){ _LL1392: _temp1391=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1385)->f1; goto _LL1388;} else{ goto _LL1389;} _LL1389: if( _temp1385 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1390;} else{ goto _LL1386;} _LL1388: if(
Cyc_Evexp_eval_const_uint_exp( _temp1391) <  1){({ void* _temp1393[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1393, sizeof( void*), 0u));});}{ int possibly_null=(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1379); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp964->topt))->v)){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp964->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1394=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1394->hd=( void*) _temp964;
_temp1394->tl= 0; _temp1394;}), 0), 0), _temp962, 0))->r));} goto _LL1386;}
_LL1390: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1383);( void*)( _temp964->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1377), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1395[ 3u];
_temp1395[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1395[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1395[ 0u]= Cyc_Absyn_copy_exp( _temp964);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1395, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1386;} _LL1386:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL795;}} _LL845: { void* _temp1396= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp968->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp968); Cyc_Toc_exp_to_c( nv, _temp966);{ void* _temp1397= _temp1396; struct
Cyc_List_List* _temp1405; struct Cyc_Absyn_PtrInfo _temp1407; struct Cyc_Absyn_Conref*
_temp1409; struct Cyc_Absyn_Tqual _temp1411; struct Cyc_Absyn_Conref* _temp1413;
void* _temp1415; void* _temp1417; _LL1399: if(( unsigned int) _temp1397 >  4u?*((
int*) _temp1397) ==  Cyc_Absyn_TupleType: 0){ _LL1406: _temp1405=(( struct Cyc_Absyn_TupleType_struct*)
_temp1397)->f1; goto _LL1400;} else{ goto _LL1401;} _LL1401: if(( unsigned int)
_temp1397 >  4u?*(( int*) _temp1397) ==  Cyc_Absyn_PointerType: 0){ _LL1408:
_temp1407=(( struct Cyc_Absyn_PointerType_struct*) _temp1397)->f1; _LL1418:
_temp1417=( void*) _temp1407.elt_typ; goto _LL1416; _LL1416: _temp1415=( void*)
_temp1407.rgn_typ; goto _LL1414; _LL1414: _temp1413= _temp1407.nullable; goto
_LL1412; _LL1412: _temp1411= _temp1407.tq; goto _LL1410; _LL1410: _temp1409=
_temp1407.bounds; goto _LL1402;} else{ goto _LL1403;} _LL1403: goto _LL1404;
_LL1400: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp966) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp968, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1398;} _LL1402:{ void* _temp1419= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1409); struct Cyc_Absyn_Exp* _temp1425; _LL1421: if((
unsigned int) _temp1419 >  1u?*(( int*) _temp1419) ==  Cyc_Absyn_Upper_b: 0){
_LL1426: _temp1425=(( struct Cyc_Absyn_Upper_b_struct*) _temp1419)->f1; goto
_LL1422;} else{ goto _LL1423;} _LL1423: if( _temp1419 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1424;} else{ goto _LL1420;} _LL1422: _temp1425= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1425), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1413); void* ta1= Cyc_Toc_typ_to_c( _temp1417);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1411); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1427[ 4u];
_temp1427[ 3u]= _temp966; _temp1427[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1427[ 1u]= _temp1425; _temp1427[ 0u]= _temp968;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1427, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp966->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1428[ 2u];
_temp1428[ 1u]= Cyc_Absyn_copy_exp( _temp966); _temp1428[ 0u]= _temp1425;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1428, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1420;}
_LL1424: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1417);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1411), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1429[ 3u]; _temp1429[ 2u]= _temp966; _temp1429[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1429[ 0u]= _temp968;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1429, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1420;} _LL1420:;} goto _LL1398; _LL1404:({
void* _temp1430[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1430, sizeof( void*), 0u));}); goto
_LL1398; _LL1398:;} goto _LL795;} _LL847: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp970))->r));} else{ struct
Cyc_List_List* _temp1431=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp970); struct _tagged_arr* _temp1432= Cyc_Toc_add_tuple_type( _temp1431);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp970 !=  0;( _temp970=((
struct Cyc_List_List*) _check_null( _temp970))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp970))->hd);
dles=({ struct Cyc_List_List* _temp1433=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1433->hd=( void*)({ struct _tuple4*
_temp1434=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1434->f1=
0; _temp1434->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp970))->hd; _temp1434;}); _temp1433->tl= dles; _temp1433;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL795; _LL849:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp972, 0))->r));{ struct Cyc_List_List* _temp1435= _temp972; for( 0;
_temp1435 !=  0; _temp1435=(( struct Cyc_List_List*) _check_null( _temp1435))->tl){
struct _tuple4 _temp1438; struct Cyc_Absyn_Exp* _temp1439; struct _tuple4*
_temp1436=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1435))->hd;
_temp1438=* _temp1436; _LL1440: _temp1439= _temp1438.f2; goto _LL1437; _LL1437:
Cyc_Toc_exp_to_c( nv, _temp1439);}} goto _LL795; _LL851: { unsigned int
_temp1441= Cyc_Evexp_eval_const_uint_exp( _temp976); Cyc_Toc_exp_to_c( nv,
_temp974);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp974)){
unsigned int i= 0; for( 0; i <  _temp1441; i ++){ es=({ struct Cyc_List_List*
_temp1442=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1442->hd=( void*)({ struct _tuple4* _temp1443=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1443->f1= 0; _temp1443->f2= _temp974; _temp1443;});
_temp1442->tl= es; _temp1442;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL795;}} _LL853: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp982, _temp986))->r));}
else{ if( _temp980 ==  0){({ void* _temp1444[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1444, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp980); struct _RegionHandle _temp1445= _new_region(); struct _RegionHandle*
rgn=& _temp1445; _push_region( rgn);{ struct Cyc_List_List* _temp1446=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp982,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1447= 0;{ struct Cyc_List_List*
_temp1448=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1448 !=  0; _temp1448=(( struct Cyc_List_List*) _check_null(
_temp1448))->tl){ struct Cyc_List_List* _temp1449= _temp1446; for( 0; _temp1449
!=  0; _temp1449=(( struct Cyc_List_List*) _check_null( _temp1449))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1449))->hd)).f1 == (
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1448))->hd){
struct _tuple10 _temp1452; struct Cyc_Absyn_Exp* _temp1453; struct Cyc_Absyn_Structfield*
_temp1455; struct _tuple10* _temp1450=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1449))->hd; _temp1452=* _temp1450; _LL1456: _temp1455=
_temp1452.f1; goto _LL1454; _LL1454: _temp1453= _temp1452.f2; goto _LL1451;
_LL1451: { void* _temp1457=( void*) _temp1455->type; Cyc_Toc_exp_to_c( nv,
_temp1453); if( Cyc_Toc_is_void_star( _temp1457)){( void*)( _temp1453->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1453->r, 0), 0))->r));} _temp1447=({ struct Cyc_List_List* _temp1458=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1458->hd=(
void*)({ struct _tuple4* _temp1459=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1459->f1= 0; _temp1459->f2= _temp1453; _temp1459;}); _temp1458->tl=
_temp1447; _temp1458;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1447), 0))->r));}; _pop_region( rgn);}} goto _LL795; _LL855: { struct Cyc_List_List*
fs;{ void* _temp1460= Cyc_Tcutil_compress( _temp990); struct Cyc_List_List*
_temp1466; _LL1462: if(( unsigned int) _temp1460 >  4u?*(( int*) _temp1460) == 
Cyc_Absyn_AnonStructType: 0){ _LL1467: _temp1466=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1460)->f1; goto _LL1463;} else{ goto _LL1464;} _LL1464: goto _LL1465;
_LL1463: fs= _temp1466; goto _LL1461; _LL1465: fs=({ struct Cyc_Std_String_pa_struct
_temp1469; _temp1469.tag= Cyc_Std_String_pa; _temp1469.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp990);{ void* _temp1468[ 1u]={& _temp1469};(( struct
Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s", sizeof( unsigned char), 24u), _tag_arr(
_temp1468, sizeof( void*), 1u));}}); goto _LL1461; _LL1461:;}{ struct
_RegionHandle _temp1470= _new_region(); struct _RegionHandle* rgn=& _temp1470;
_push_region( rgn);{ struct Cyc_List_List* _temp1471=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp988, fs); for( 0; _temp1471 !=  0; _temp1471=(( struct Cyc_List_List*)
_check_null( _temp1471))->tl){ struct _tuple10 _temp1474; struct Cyc_Absyn_Exp*
_temp1475; struct Cyc_Absyn_Structfield* _temp1477; struct _tuple10* _temp1472=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1471))->hd;
_temp1474=* _temp1472; _LL1478: _temp1477= _temp1474.f1; goto _LL1476; _LL1476:
_temp1475= _temp1474.f2; goto _LL1473; _LL1473: { void* _temp1479=( void*)
_temp1477->type; Cyc_Toc_exp_to_c( nv, _temp1475); if( Cyc_Toc_is_void_star(
_temp1479)){( void*)( _temp1475->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1475->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp988, 0))->r));}; _pop_region( rgn);}
goto _LL795;} _LL857: { struct _tuple0* qv= _temp992->name; if( _temp994->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1480= _temp994->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp994->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1480))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1480))->hd)->typs ==  0){ tag_count ++;}
_temp1480=(( struct Cyc_List_List*) _check_null( _temp1480))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL795;} _LL859: { struct _tuple0* _temp1481= _temp998->name; struct Cyc_List_List*
_temp1482= _temp998->typs; struct _tuple0* _temp1483= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp1484= Cyc_Absyn_var_exp( _temp1483, 0); void* _temp1485= Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag( _temp1481, _tag_arr("_struct", sizeof( unsigned char),
8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1000->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp1481, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp1486= _temp1000->fields ==  0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1000->fields))->v; while( Cyc_Absyn_qvar_cmp( _temp1481,((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp1486))->hd)->name)
!=  0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1486))->hd)->typs !=  0){ tag_count ++;} _temp1486=(( struct
Cyc_List_List*) _check_null( _temp1486))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1002
!=  0;( _temp1002=(( struct Cyc_List_List*) _check_null( _temp1002))->tl,
_temp1482=(( struct Cyc_List_List*) _check_null( _temp1482))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1002))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1482))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1487=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1487->hd=( void*)({ struct _tuple4* _temp1488=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1488->f1= 0; _temp1488->f2= cur_e; _temp1488;});
_temp1487->tl= dles; _temp1487;});} dles=({ struct Cyc_List_List* _temp1489=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1489->hd=(
void*)({ struct _tuple4* _temp1490=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1490->f1= 0; _temp1490->f2= tag_exp; _temp1490;}); _temp1489->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1489;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1491=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1491->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1481,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1491;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1492=({ struct Cyc_List_List* _temp1498=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1498->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1484, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1481, 0), 0); _temp1498->tl= 0; _temp1498;});{ int i=
1; for( 0; _temp1002 !=  0;((( _temp1002=(( struct Cyc_List_List*) _check_null(
_temp1002))->tl, i ++)), _temp1482=(( struct Cyc_List_List*) _check_null(
_temp1482))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1002))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1482))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp1493= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1484, Cyc_Absyn_fieldname( i), 0), cur_e, 0);
_temp1492=({ struct Cyc_List_List* _temp1494=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1494->hd=( void*) _temp1493; _temp1494->tl=
_temp1492; _temp1494;});}}}{ struct Cyc_Absyn_Stmt* _temp1495= Cyc_Absyn_exp_stmt(
_temp1484, 0); struct Cyc_Absyn_Stmt* _temp1496= Cyc_Absyn_seq_stmts((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List*
_temp1497=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1497->hd=( void*) _temp1495; _temp1497->tl= _temp1492; _temp1497;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1483, _temp1485, 0, _temp1496, 0), 0))->r));}} goto _LL795;} _LL861: goto
_LL795; _LL863: { void* t_c= Cyc_Toc_typ_to_c( _temp1004); if( _temp1006 !=  0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1006); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1004, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL795;} _LL865: Cyc_Toc_stmt_to_c(
nv, _temp1008); goto _LL795; _LL867:({ void* _temp1499[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem",
sizeof( unsigned char), 14u), _tag_arr( _temp1499, sizeof( void*), 0u));}); goto
_LL795; _LL869:({ void* _temp1500[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u), _tag_arr( _temp1500, sizeof( void*), 0u));}); goto _LL795;
_LL871:({ void* _temp1501[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u),
_tag_arr( _temp1501, sizeof( void*), 0u));}); goto _LL795; _LL873:({ void*
_temp1502[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("fill", sizeof( unsigned char), 5u), _tag_arr( _temp1502, sizeof( void*),
0u));}); goto _LL795; _LL795:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1503=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1539; struct Cyc_Absyn_Vardecl _temp1541; struct _tuple0* _temp1542; struct
Cyc_Absyn_Vardecl* _temp1544; int _temp1546; void* _temp1548; unsigned char
_temp1550; struct _tagged_arr _temp1552; struct Cyc_Absyn_Enumfield* _temp1554;
struct Cyc_Absyn_Enumdecl* _temp1556; struct Cyc_List_List* _temp1558; struct
Cyc_List_List* _temp1560; struct Cyc_Absyn_Tunionfield* _temp1562; struct Cyc_Absyn_Tuniondecl*
_temp1564; struct Cyc_Absyn_Pat* _temp1567; struct Cyc_Absyn_Pat _temp1569; void*
_temp1570; struct Cyc_List_List* _temp1572; struct Cyc_List_List* _temp1574;
struct Cyc_Absyn_Tunionfield* _temp1576; struct Cyc_Absyn_Tuniondecl* _temp1578;
struct Cyc_List_List* _temp1580; struct Cyc_List_List* _temp1582; struct Cyc_List_List*
_temp1584; struct Cyc_List_List* _temp1586; struct Cyc_Core_Opt* _temp1588;
struct Cyc_Absyn_Structdecl* _temp1590; struct Cyc_Absyn_Pat* _temp1592; _LL1505:
if(( unsigned int) _temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_Var_p: 0){
_LL1540: _temp1539=(( struct Cyc_Absyn_Var_p_struct*) _temp1503)->f1; _temp1541=*
_temp1539; _LL1543: _temp1542= _temp1541.name; goto _LL1506;} else{ goto _LL1507;}
_LL1507: if( _temp1503 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1508;} else{ goto
_LL1509;} _LL1509: if(( unsigned int) _temp1503 >  2u?*(( int*) _temp1503) == 
Cyc_Absyn_Reference_p: 0){ _LL1545: _temp1544=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1503)->f1; goto _LL1510;} else{ goto _LL1511;} _LL1511: if( _temp1503 == (
void*) Cyc_Absyn_Null_p){ goto _LL1512;} else{ goto _LL1513;} _LL1513: if((
unsigned int) _temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_Int_p: 0){
_LL1549: _temp1548=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1503)->f1;
goto _LL1547; _LL1547: _temp1546=(( struct Cyc_Absyn_Int_p_struct*) _temp1503)->f2;
goto _LL1514;} else{ goto _LL1515;} _LL1515: if(( unsigned int) _temp1503 >  2u?*((
int*) _temp1503) ==  Cyc_Absyn_Char_p: 0){ _LL1551: _temp1550=(( struct Cyc_Absyn_Char_p_struct*)
_temp1503)->f1; goto _LL1516;} else{ goto _LL1517;} _LL1517: if(( unsigned int)
_temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_Float_p: 0){ _LL1553:
_temp1552=(( struct Cyc_Absyn_Float_p_struct*) _temp1503)->f1; goto _LL1518;}
else{ goto _LL1519;} _LL1519: if(( unsigned int) _temp1503 >  2u?*(( int*)
_temp1503) ==  Cyc_Absyn_Enum_p: 0){ _LL1557: _temp1556=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1503)->f1; goto _LL1555; _LL1555: _temp1554=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1503)->f2; goto _LL1520;} else{ goto _LL1521;} _LL1521: if(( unsigned int)
_temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_Tunion_p: 0){ _LL1565:
_temp1564=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1503)->f1; goto _LL1563;
_LL1563: _temp1562=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1503)->f2; goto
_LL1561; _LL1561: _temp1560=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1503)->f3;
goto _LL1559; _LL1559: _temp1558=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1503)->f4;
if( _temp1558 ==  0){ goto _LL1522;} else{ goto _LL1523;}} else{ goto _LL1523;}
_LL1523: if(( unsigned int) _temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_Pointer_p:
0){ _LL1568: _temp1567=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1503)->f1;
_temp1569=* _temp1567; _LL1571: _temp1570=( void*) _temp1569.r; if((
unsigned int) _temp1570 >  2u?*(( int*) _temp1570) ==  Cyc_Absyn_Tunion_p: 0){
_LL1579: _temp1578=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1570)->f1; goto
_LL1577; _LL1577: _temp1576=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1570)->f2;
goto _LL1575; _LL1575: _temp1574=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1570)->f3;
goto _LL1573; _LL1573: _temp1572=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1570)->f4;
goto _LL1566;} else{ goto _LL1525;}} else{ goto _LL1525;} _LL1566: if( _temp1572
!=  0){ goto _LL1524;} else{ goto _LL1525;} _LL1525: if(( unsigned int)
_temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_Tunion_p: 0){ _LL1581:
_temp1580=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1503)->f4; goto _LL1526;}
else{ goto _LL1527;} _LL1527: if(( unsigned int) _temp1503 >  2u?*(( int*)
_temp1503) ==  Cyc_Absyn_Tuple_p: 0){ _LL1583: _temp1582=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1503)->f1; goto _LL1528;} else{ goto _LL1529;} _LL1529: if(( unsigned int)
_temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_Struct_p: 0){ _LL1591:
_temp1590=(( struct Cyc_Absyn_Struct_p_struct*) _temp1503)->f1; goto _LL1589;
_LL1589: _temp1588=(( struct Cyc_Absyn_Struct_p_struct*) _temp1503)->f2; goto
_LL1587; _LL1587: _temp1586=(( struct Cyc_Absyn_Struct_p_struct*) _temp1503)->f3;
goto _LL1585; _LL1585: _temp1584=(( struct Cyc_Absyn_Struct_p_struct*) _temp1503)->f4;
goto _LL1530;} else{ goto _LL1531;} _LL1531: if(( unsigned int) _temp1503 >  2u?*((
int*) _temp1503) ==  Cyc_Absyn_Pointer_p: 0){ _LL1593: _temp1592=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1503)->f1; goto _LL1532;} else{ goto _LL1533;} _LL1533: if(( unsigned int)
_temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1534;}
else{ goto _LL1535;} _LL1535: if(( unsigned int) _temp1503 >  2u?*(( int*)
_temp1503) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1536;} else{ goto _LL1537;}
_LL1537: if(( unsigned int) _temp1503 >  2u?*(( int*) _temp1503) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1538;} else{ goto _LL1504;} _LL1506: nv= Cyc_Toc_add_varmap( nv,
_temp1542, r); goto _LL1508; _LL1508: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1504; _LL1510: { struct _tuple0* _temp1594= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1595=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1595->hd=( void*)({ struct _tuple12*
_temp1596=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1596->f1= _temp1594; _temp1596->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1596;}); _temp1595->tl= decls; _temp1595;}); nv= Cyc_Toc_add_varmap(
nv, _temp1544->name, Cyc_Absyn_var_exp( _temp1594, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1594, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1504;} _LL1512: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1504; _LL1514:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1548, _temp1546, 0),
succ_lab, fail_lab); goto _LL1504; _LL1516: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1550, 0), succ_lab, fail_lab); goto _LL1504; _LL1518: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1552, 0), succ_lab, fail_lab); goto _LL1504;
_LL1520: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1597=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1597[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1598; _temp1598.tag= Cyc_Absyn_Enum_e;
_temp1598.f1= _temp1554->name; _temp1598.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1556; _temp1598.f3=( struct Cyc_Absyn_Enumfield*) _temp1554; _temp1598;});
_temp1597;}), 0), succ_lab, fail_lab); goto _LL1504; _LL1522: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1562->name, 0); if( ! _temp1564->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1504;} _LL1524: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1572);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1576->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1599=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1576->typs); struct Cyc_List_List*
_temp1600=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1572); for( 0; _temp1600 !=  0;((( _temp1600=((
struct Cyc_List_List*) _check_null( _temp1600))->tl, _temp1599=(( struct Cyc_List_List*)
_check_null( _temp1599))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1601=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1600))->hd;
if(( void*) _temp1601->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1602=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1599))->hd)).f2;
struct _tuple0* _temp1603= Cyc_Toc_temp_var(); void* _temp1604=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1601->topt))->v; void* _temp1605= Cyc_Toc_typ_to_c(
_temp1604); struct _tagged_arr* _temp1606= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1607= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1602))){ _temp1607= Cyc_Absyn_cast_exp(
_temp1605, _temp1607, 0);} decls=({ struct Cyc_List_List* _temp1608=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1608->hd=( void*)({
struct _tuple12* _temp1609=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1609->f1= _temp1603; _temp1609->f2= _temp1605; _temp1609;});
_temp1608->tl= decls; _temp1608;});{ struct _tuple11 _temp1610= Cyc_Toc_xlate_pat(
nv, rgn, _temp1604, Cyc_Absyn_var_exp( _temp1603, 0), _temp1607, _temp1601,
succ_lab, fail_lab, decls); nv= _temp1610.f1; decls= _temp1610.f2;{ struct Cyc_Absyn_Stmt*
_temp1611= _temp1610.f3; struct Cyc_Absyn_Stmt* _temp1612= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1603, 0), _temp1607, 0); struct Cyc_Absyn_Stmt*
_temp1613= Cyc_Absyn_seq_stmt( _temp1612, _temp1611, 0); ss=({ struct Cyc_List_List*
_temp1614=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1614->hd=( void*) Cyc_Absyn_label_stmt( _temp1606, _temp1613, 0); _temp1614->tl=
ss; _temp1614;}); succ_lab= _temp1606;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1615=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1615->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1615->tl= 0;
_temp1615;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1578->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1576->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1616=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1578->fields))->v; for( 0; _temp1616 !=  0; _temp1616=((
struct Cyc_List_List*) _check_null( _temp1616))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1617=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1616))->hd; if( _temp1617->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1576->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1504;}} _LL1526: _temp1582= _temp1580; goto _LL1528; _LL1528: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1582);{ struct Cyc_List_List* _temp1618=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1582); for(
0; _temp1618 !=  0;( _temp1618=(( struct Cyc_List_List*) _check_null( _temp1618))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1619=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1618))->hd; if(( void*) _temp1619->r == ( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1620= Cyc_Toc_temp_var(); void* _temp1621=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1619->topt))->v; struct
_tagged_arr* _temp1622= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1623=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1623->hd=( void*)({ struct _tuple12* _temp1624=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1624->f1= _temp1620;
_temp1624->f2= Cyc_Toc_typ_to_c( _temp1621); _temp1624;}); _temp1623->tl= decls;
_temp1623;});{ struct _tuple11 _temp1625= Cyc_Toc_xlate_pat( nv, rgn, _temp1621,
Cyc_Absyn_var_exp( _temp1620, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1619, succ_lab, fail_lab, decls); nv= _temp1625.f1; decls=
_temp1625.f2;{ struct Cyc_Absyn_Stmt* _temp1626= _temp1625.f3; struct Cyc_Absyn_Stmt*
_temp1627= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1620, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp1626, 0); ss=({ struct Cyc_List_List* _temp1628=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1628->hd=( void*) Cyc_Absyn_label_stmt(
_temp1622, _temp1627, 0); _temp1628->tl= ss; _temp1628;}); succ_lab= _temp1622;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1629=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1629->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1629->tl= 0; _temp1629;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1504;} _LL1530: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1630=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1584); for( 0; _temp1630 !=  0; _temp1630=((
struct Cyc_List_List*) _check_null( _temp1630))->tl){ struct _tuple13* _temp1631=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1630))->hd; struct
Cyc_Absyn_Pat* _temp1632=(* _temp1631).f2; if(( void*) _temp1632->r == ( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1633=( void*)((
struct Cyc_List_List*) _check_null((* _temp1631).f1))->hd; struct _tagged_arr*
_temp1639; _LL1635: if(*(( int*) _temp1633) ==  Cyc_Absyn_FieldName){ _LL1640:
_temp1639=(( struct Cyc_Absyn_FieldName_struct*) _temp1633)->f1; goto _LL1636;}
else{ goto _LL1637;} _LL1637: goto _LL1638; _LL1636: f= _temp1639; goto _LL1634;
_LL1638:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1634:;}{ struct _tuple0*
_temp1641= Cyc_Toc_temp_var(); void* _temp1642=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1632->topt))->v; void* _temp1643= Cyc_Toc_typ_to_c( _temp1642);
struct _tagged_arr* _temp1644= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1645=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1645->hd=( void*)({ struct _tuple12* _temp1646=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1646->f1= _temp1641;
_temp1646->f2= _temp1643; _temp1646;}); _temp1645->tl= decls; _temp1645;});{
struct _tuple11 _temp1647= Cyc_Toc_xlate_pat( nv, rgn, _temp1642, Cyc_Absyn_var_exp(
_temp1641, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1632, succ_lab,
fail_lab, decls); nv= _temp1647.f1; decls= _temp1647.f2;{ struct Cyc_Absyn_Exp*
_temp1648= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1590->fields))->v, f)))->type)){
_temp1648= Cyc_Absyn_cast_exp( _temp1643, _temp1648, 0);}{ struct Cyc_Absyn_Stmt*
_temp1649= _temp1647.f3; struct Cyc_Absyn_Stmt* _temp1650= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1641, 0), _temp1648, 0),
_temp1649, 0); ss=({ struct Cyc_List_List* _temp1651=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1651->hd=( void*) Cyc_Absyn_label_stmt(
_temp1644, _temp1650, 0); _temp1651->tl= ss; _temp1651;}); succ_lab= _temp1644;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1652=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1652->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1652->tl= 0; _temp1652;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1504;} _LL1532: { struct _tuple0* _temp1653= Cyc_Toc_temp_var(); void*
_temp1654=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1592->topt))->v;
decls=({ struct Cyc_List_List* _temp1655=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1655->hd=( void*)({ struct _tuple12*
_temp1656=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1656->f1= _temp1653; _temp1656->f2= Cyc_Toc_typ_to_c( _temp1654); _temp1656;});
_temp1655->tl= decls; _temp1655;});{ struct _tuple11 _temp1657= Cyc_Toc_xlate_pat(
nv, rgn, _temp1654, Cyc_Absyn_var_exp( _temp1653, 0), Cyc_Absyn_deref_exp( path,
0), _temp1592, succ_lab, fail_lab, decls); nv= _temp1657.f1; decls= _temp1657.f2;{
struct Cyc_Absyn_Stmt* _temp1658= _temp1657.f3; struct Cyc_Absyn_Stmt* _temp1659=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1653, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1658, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1659, 0);} else{ s= _temp1659;} goto _LL1504;}}} _LL1534: s=({
void* _temp1660[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1660, sizeof( void*), 0u));}); goto _LL1504;
_LL1536: s=({ void* _temp1661[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1661, sizeof( void*), 0u));}); goto
_LL1504; _LL1538: s=({ void* _temp1662[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1662, sizeof( void*), 0u));}); goto
_LL1504; _LL1504:;} return({ struct _tuple11 _temp1663; _temp1663.f1= nv;
_temp1663.f2= decls; _temp1663.f3= s; _temp1663;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1664=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1664->f1= Cyc_Toc_fresh_label(); _temp1664->f2=
Cyc_Toc_fresh_label(); _temp1664->f3= sc; _temp1664;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1665=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1666= Cyc_Tcutil_compress( _temp1665); _LL1668: if(( unsigned int)
_temp1666 >  4u?*(( int*) _temp1666) ==  Cyc_Absyn_IntType: 0){ goto _LL1669;}
else{ goto _LL1670;} _LL1670: if(( unsigned int) _temp1666 >  4u?*(( int*)
_temp1666) ==  Cyc_Absyn_EnumType: 0){ goto _LL1671;} else{ goto _LL1672;}
_LL1672: goto _LL1673; _LL1669: goto _LL1671; _LL1671: leave_as_switch= 1; goto
_LL1667; _LL1673: leave_as_switch= 0; goto _LL1667; _LL1667:;}{ struct Cyc_List_List*
_temp1674= scs; for( 0; _temp1674 !=  0; _temp1674=(( struct Cyc_List_List*)
_check_null( _temp1674))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1674))->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1674))->hd)->where_clause !=  0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1675= scs; for( 0; _temp1675
!=  0; _temp1675=(( struct Cyc_List_List*) _check_null( _temp1675))->tl){ struct
Cyc_Absyn_Stmt* _temp1676=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1675))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1675))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1676, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1676);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1677= _new_region(); struct _RegionHandle* rgn=&
_temp1677; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1678= lscs; for( 0;
_temp1678 !=  0; _temp1678=(( struct Cyc_List_List*) _check_null( _temp1678))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1678))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1678))->tl ==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1678))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1678))->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11
_temp1679= Cyc_Toc_xlate_pat( nv, rgn, _temp1665, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1680=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1680->hd=( void*)
_temp1679.f1; _temp1680->tl= nvs; _temp1680;}); decls= _temp1679.f2; test_stmts=({
struct Cyc_List_List* _temp1681=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1681->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1678))->hd)).f1,
_temp1679.f3, 0); _temp1681->tl= test_stmts; _temp1681;});} else{ struct Cyc_Absyn_Exp*
_temp1682=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1683= Cyc_Toc_fresh_label(); struct _tuple11 _temp1684= Cyc_Toc_xlate_pat(
nv, rgn, _temp1665, r, path, sc->pattern, _temp1683, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1684.f1, _temp1682); nvs=({ struct Cyc_List_List* _temp1685=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1685->hd=( void*)
_temp1684.f1; _temp1685->tl= nvs; _temp1685;}); decls= _temp1684.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1682, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1683, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1686=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1686->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1678))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1684.f3, s3, 0), 0); _temp1686->tl= test_stmts;
_temp1686;});}}}}{ struct Cyc_Absyn_Stmt* _temp1687= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs !=  0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=  0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1688=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1688, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1689=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1689, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1687, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls !=  0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1692; void* _temp1693; struct
_tuple0* _temp1695; struct _tuple12* _temp1690=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1692=* _temp1690; _LL1696: _temp1695= _temp1692.f1;
goto _LL1694; _LL1694: _temp1693= _temp1692.f2; goto _LL1691; _LL1691: res= Cyc_Absyn_declare_stmt(
_temp1695, _temp1693, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1697=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1697[ 0]=({ struct Cyc_List_List
_temp1698; _temp1698.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1698.tl= 0; _temp1698;}); _temp1697;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1699=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1741; struct Cyc_Absyn_Stmt* _temp1743; struct Cyc_Absyn_Stmt* _temp1745;
struct Cyc_Absyn_Exp* _temp1747; struct Cyc_Absyn_Stmt* _temp1749; struct Cyc_Absyn_Stmt*
_temp1751; struct Cyc_Absyn_Exp* _temp1753; struct Cyc_Absyn_Stmt* _temp1755;
struct _tuple2 _temp1757; struct Cyc_Absyn_Exp* _temp1759; struct Cyc_Absyn_Stmt*
_temp1761; struct Cyc_Absyn_Stmt* _temp1763; struct Cyc_Absyn_Stmt* _temp1765;
struct Cyc_Absyn_Stmt* _temp1767; struct _tuple2 _temp1769; struct Cyc_Absyn_Exp*
_temp1771; struct _tuple2 _temp1773; struct Cyc_Absyn_Exp* _temp1775; struct Cyc_Absyn_Exp*
_temp1777; struct Cyc_List_List* _temp1779; struct Cyc_Absyn_Exp* _temp1781;
struct Cyc_Absyn_Switch_clause** _temp1783; struct Cyc_List_List* _temp1785;
struct Cyc_Absyn_Stmt* _temp1787; struct Cyc_Absyn_Decl* _temp1789; struct Cyc_Absyn_Stmt*
_temp1791; struct _tagged_arr* _temp1793; struct _tuple2 _temp1795; struct Cyc_Absyn_Exp*
_temp1797; struct Cyc_Absyn_Stmt* _temp1799; struct Cyc_List_List* _temp1801;
struct Cyc_Absyn_Stmt* _temp1803; struct Cyc_Absyn_Stmt* _temp1805; struct Cyc_Absyn_Vardecl*
_temp1807; struct Cyc_Absyn_Tvar* _temp1809; _LL1701: if( _temp1699 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1702;} else{ goto _LL1703;} _LL1703: if((
unsigned int) _temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Exp_s: 0){
_LL1742: _temp1741=(( struct Cyc_Absyn_Exp_s_struct*) _temp1699)->f1; goto
_LL1704;} else{ goto _LL1705;} _LL1705: if(( unsigned int) _temp1699 >  1u?*((
int*) _temp1699) ==  Cyc_Absyn_Seq_s: 0){ _LL1746: _temp1745=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1699)->f1; goto _LL1744; _LL1744: _temp1743=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1699)->f2; goto _LL1706;} else{ goto _LL1707;} _LL1707: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Return_s: 0){ _LL1748:
_temp1747=(( struct Cyc_Absyn_Return_s_struct*) _temp1699)->f1; goto _LL1708;}
else{ goto _LL1709;} _LL1709: if(( unsigned int) _temp1699 >  1u?*(( int*)
_temp1699) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1754: _temp1753=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1699)->f1; goto _LL1752; _LL1752: _temp1751=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1699)->f2; goto _LL1750; _LL1750: _temp1749=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1699)->f3; goto _LL1710;} else{ goto _LL1711;} _LL1711: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_While_s: 0){ _LL1758:
_temp1757=(( struct Cyc_Absyn_While_s_struct*) _temp1699)->f1; _LL1760:
_temp1759= _temp1757.f1; goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_While_s_struct*)
_temp1699)->f2; goto _LL1712;} else{ goto _LL1713;} _LL1713: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Break_s: 0){ _LL1762:
_temp1761=(( struct Cyc_Absyn_Break_s_struct*) _temp1699)->f1; goto _LL1714;}
else{ goto _LL1715;} _LL1715: if(( unsigned int) _temp1699 >  1u?*(( int*)
_temp1699) ==  Cyc_Absyn_Continue_s: 0){ _LL1764: _temp1763=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1699)->f1; goto _LL1716;} else{ goto _LL1717;} _LL1717: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Goto_s: 0){ _LL1766:
_temp1765=(( struct Cyc_Absyn_Goto_s_struct*) _temp1699)->f2; goto _LL1718;}
else{ goto _LL1719;} _LL1719: if(( unsigned int) _temp1699 >  1u?*(( int*)
_temp1699) ==  Cyc_Absyn_For_s: 0){ _LL1778: _temp1777=(( struct Cyc_Absyn_For_s_struct*)
_temp1699)->f1; goto _LL1774; _LL1774: _temp1773=(( struct Cyc_Absyn_For_s_struct*)
_temp1699)->f2; _LL1776: _temp1775= _temp1773.f1; goto _LL1770; _LL1770:
_temp1769=(( struct Cyc_Absyn_For_s_struct*) _temp1699)->f3; _LL1772: _temp1771=
_temp1769.f1; goto _LL1768; _LL1768: _temp1767=(( struct Cyc_Absyn_For_s_struct*)
_temp1699)->f4; goto _LL1720;} else{ goto _LL1721;} _LL1721: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Switch_s: 0){ _LL1782:
_temp1781=(( struct Cyc_Absyn_Switch_s_struct*) _temp1699)->f1; goto _LL1780;
_LL1780: _temp1779=(( struct Cyc_Absyn_Switch_s_struct*) _temp1699)->f2; goto
_LL1722;} else{ goto _LL1723;} _LL1723: if(( unsigned int) _temp1699 >  1u?*((
int*) _temp1699) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1786: _temp1785=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1699)->f1; goto _LL1784; _LL1784: _temp1783=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1699)->f2; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Decl_s: 0){ _LL1790:
_temp1789=(( struct Cyc_Absyn_Decl_s_struct*) _temp1699)->f1; goto _LL1788;
_LL1788: _temp1787=(( struct Cyc_Absyn_Decl_s_struct*) _temp1699)->f2; goto
_LL1726;} else{ goto _LL1727;} _LL1727: if(( unsigned int) _temp1699 >  1u?*((
int*) _temp1699) ==  Cyc_Absyn_Label_s: 0){ _LL1794: _temp1793=(( struct Cyc_Absyn_Label_s_struct*)
_temp1699)->f1; goto _LL1792; _LL1792: _temp1791=(( struct Cyc_Absyn_Label_s_struct*)
_temp1699)->f2; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Do_s: 0){ _LL1800: _temp1799=((
struct Cyc_Absyn_Do_s_struct*) _temp1699)->f1; goto _LL1796; _LL1796: _temp1795=((
struct Cyc_Absyn_Do_s_struct*) _temp1699)->f2; _LL1798: _temp1797= _temp1795.f1;
goto _LL1730;} else{ goto _LL1731;} _LL1731: if(( unsigned int) _temp1699 >  1u?*((
int*) _temp1699) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1804: _temp1803=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1699)->f1; goto _LL1802; _LL1802: _temp1801=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1699)->f2; goto _LL1732;} else{ goto _LL1733;} _LL1733: if(( unsigned int)
_temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Region_s: 0){ _LL1810:
_temp1809=(( struct Cyc_Absyn_Region_s_struct*) _temp1699)->f1; goto _LL1808;
_LL1808: _temp1807=(( struct Cyc_Absyn_Region_s_struct*) _temp1699)->f2; goto
_LL1806; _LL1806: _temp1805=(( struct Cyc_Absyn_Region_s_struct*) _temp1699)->f3;
goto _LL1734;} else{ goto _LL1735;} _LL1735: if(( unsigned int) _temp1699 >  1u?*((
int*) _temp1699) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1736;} else{ goto _LL1737;}
_LL1737: if(( unsigned int) _temp1699 >  1u?*(( int*) _temp1699) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1738;} else{ goto _LL1739;} _LL1739: if(( unsigned int) _temp1699 > 
1u?*(( int*) _temp1699) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1740;} else{ goto
_LL1700;} _LL1702: return; _LL1704: Cyc_Toc_exp_to_c( nv, _temp1741); return;
_LL1706: Cyc_Toc_stmt_to_c( nv, _temp1745); s= _temp1743; continue; _LL1708: {
struct Cyc_Core_Opt* topt= 0; if( _temp1747 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1811=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1811->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1747))->topt))->v);
_temp1811;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1747));} if( s->try_depth >  0){ if( topt !=  0){ struct _tuple0* _temp1812=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1813= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1812, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1812,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1747, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1813, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1710: Cyc_Toc_exp_to_c( nv, _temp1753); Cyc_Toc_stmt_to_c( nv,
_temp1751); s= _temp1749; continue; _LL1712: Cyc_Toc_exp_to_c( nv, _temp1759);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1755); return; _LL1714: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1761 ==  0? 0:(( struct Cyc_Absyn_Stmt*) _check_null(
_temp1761))->try_depth; Cyc_Toc_do_npop_before( s->try_depth -  dest_depth, s);
return;} _LL1716: if( nv->continue_lab !=  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
nv->continue_lab))->v, 0))->r));} _temp1765= _temp1763; goto _LL1718; _LL1718:
Cyc_Toc_do_npop_before( s->try_depth - (( struct Cyc_Absyn_Stmt*) _check_null(
_temp1765))->try_depth, s); return; _LL1720: Cyc_Toc_exp_to_c( nv, _temp1777);
Cyc_Toc_exp_to_c( nv, _temp1775); Cyc_Toc_exp_to_c( nv, _temp1771); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1767); return; _LL1722: Cyc_Toc_xlate_switch( nv, s,
_temp1781, _temp1779); return; _LL1724: if( nv->fallthru_info ==  0){( int)
_throw(({ void* _temp1814[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u), _tag_arr( _temp1814, sizeof( void*), 0u));}));}{
struct _tuple6 _temp1817; struct Cyc_Dict_Dict* _temp1818; struct Cyc_List_List*
_temp1820; struct _tagged_arr* _temp1822; struct _tuple6* _temp1815=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp1817=*
_temp1815; _LL1823: _temp1822= _temp1817.f1; goto _LL1821; _LL1821: _temp1820=
_temp1817.f2; goto _LL1819; _LL1819: _temp1818= _temp1817.f3; goto _LL1816;
_LL1816: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1822, 0); Cyc_Toc_do_npop_before(
s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp1783)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp1824=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1820); struct Cyc_List_List* _temp1825=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1785); for( 0; _temp1824 !=  0;(
_temp1824=(( struct Cyc_List_List*) _check_null( _temp1824))->tl, _temp1825=((
struct Cyc_List_List*) _check_null( _temp1825))->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1825))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp1818,(
struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp1824))->hd),( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1825))->hd, 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1726:{ void* _temp1826=(
void*) _temp1789->r; struct Cyc_Absyn_Vardecl* _temp1836; int _temp1838; struct
Cyc_Absyn_Exp* _temp1840; struct Cyc_Core_Opt* _temp1842; struct Cyc_Absyn_Pat*
_temp1844; struct Cyc_List_List* _temp1846; _LL1828: if(*(( int*) _temp1826) == 
Cyc_Absyn_Var_d){ _LL1837: _temp1836=(( struct Cyc_Absyn_Var_d_struct*)
_temp1826)->f1; goto _LL1829;} else{ goto _LL1830;} _LL1830: if(*(( int*)
_temp1826) ==  Cyc_Absyn_Let_d){ _LL1845: _temp1844=(( struct Cyc_Absyn_Let_d_struct*)
_temp1826)->f1; goto _LL1843; _LL1843: _temp1842=(( struct Cyc_Absyn_Let_d_struct*)
_temp1826)->f3; goto _LL1841; _LL1841: _temp1840=(( struct Cyc_Absyn_Let_d_struct*)
_temp1826)->f4; goto _LL1839; _LL1839: _temp1838=(( struct Cyc_Absyn_Let_d_struct*)
_temp1826)->f5; goto _LL1831;} else{ goto _LL1832;} _LL1832: if(*(( int*)
_temp1826) ==  Cyc_Absyn_Letv_d){ _LL1847: _temp1846=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1826)->f1; goto _LL1833;} else{ goto _LL1834;} _LL1834: goto _LL1835;
_LL1829: { struct Cyc_Toc_Env* _temp1848= Cyc_Toc_add_varmap( nv, _temp1836->name,
Cyc_Absyn_varb_exp( _temp1836->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1849=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1849[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1850; _temp1850.tag= Cyc_Absyn_Local_b;
_temp1850.f1= _temp1836; _temp1850;}); _temp1849;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1848, _temp1848, _temp1836, _temp1787); goto _LL1827;} _LL1831:{ void*
_temp1851=( void*) _temp1844->r; struct Cyc_Absyn_Vardecl* _temp1857; _LL1853:
if(( unsigned int) _temp1851 >  2u?*(( int*) _temp1851) ==  Cyc_Absyn_Var_p: 0){
_LL1858: _temp1857=(( struct Cyc_Absyn_Var_p_struct*) _temp1851)->f1; goto
_LL1854;} else{ goto _LL1855;} _LL1855: goto _LL1856; _LL1854: { struct _tuple0*
old_name= _temp1857->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1857->name= new_name; _temp1857->initializer=( struct Cyc_Absyn_Exp*)
_temp1840;( void*)( _temp1789->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1859=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1859[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1860; _temp1860.tag= Cyc_Absyn_Var_d;
_temp1860.f1= _temp1857; _temp1860;}); _temp1859;})));{ struct Cyc_Toc_Env*
_temp1861= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1862=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1862[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1863; _temp1863.tag= Cyc_Absyn_Local_b; _temp1863.f1=
_temp1857; _temp1863;}); _temp1862;}), 0)); Cyc_Toc_local_decl_to_c( _temp1861,
nv, _temp1857, _temp1787); goto _LL1852;}} _LL1856:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1844, _temp1842, _temp1840, _temp1838,
_temp1787))->r)); goto _LL1852; _LL1852:;} goto _LL1827; _LL1833: { struct Cyc_List_List*
_temp1864=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1846); if( _temp1864 ==  0){({ void* _temp1865[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1865, sizeof( void*), 0u));});}(
void*)( _temp1789->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1866=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1866[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1867; _temp1867.tag= Cyc_Absyn_Var_d;
_temp1867.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1864))->hd; _temp1867;}); _temp1866;}))); _temp1864=(( struct Cyc_List_List*)
_check_null( _temp1864))->tl; for( 0; _temp1864 !=  0; _temp1864=(( struct Cyc_List_List*)
_check_null( _temp1864))->tl){ struct Cyc_Absyn_Decl* _temp1868= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1869=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1869[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1870; _temp1870.tag= Cyc_Absyn_Var_d; _temp1870.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1864))->hd; _temp1870;}); _temp1869;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1868, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1827;} _LL1835:({
void* _temp1871[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1871, sizeof( void*), 0u));}); goto _LL1827;
_LL1827:;} return; _LL1728: s= _temp1791; continue; _LL1730: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1799); Cyc_Toc_exp_to_c( nv, _temp1797); return;
_LL1732: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1872=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1872->v=( void*) e_typ; _temp1872;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1803);{
struct Cyc_Absyn_Stmt* _temp1873= Cyc_Absyn_seq_stmt( _temp1803, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1874= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1875= Cyc_Absyn_var_exp(
_temp1874, 0); struct Cyc_Absyn_Vardecl* _temp1876= Cyc_Absyn_new_vardecl(
_temp1874, Cyc_Absyn_exn_typ, 0); _temp1875->topt=({ struct Cyc_Core_Opt*
_temp1877=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1877->v=( void*) Cyc_Absyn_exn_typ; _temp1877;});{ struct Cyc_Absyn_Pat*
_temp1878=({ struct Cyc_Absyn_Pat* _temp1894=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1894->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1896=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1896[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1897; _temp1897.tag= Cyc_Absyn_Var_p;
_temp1897.f1= _temp1876; _temp1897;}); _temp1896;})); _temp1894->topt=({ struct
Cyc_Core_Opt* _temp1895=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1895->v=( void*) Cyc_Absyn_exn_typ; _temp1895;}); _temp1894->loc= 0;
_temp1894;}); struct Cyc_Absyn_Exp* _temp1879= Cyc_Absyn_throw_exp( _temp1875, 0);
_temp1879->topt=({ struct Cyc_Core_Opt* _temp1880=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1880->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1880;});{ struct Cyc_Absyn_Stmt* _temp1881= Cyc_Absyn_exp_stmt( _temp1879,
0); struct Cyc_Absyn_Switch_clause* _temp1882=({ struct Cyc_Absyn_Switch_clause*
_temp1891=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1891->pattern= _temp1878; _temp1891->pat_vars=({ struct Cyc_Core_Opt*
_temp1892=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1892->v=( void*)({ struct Cyc_List_List* _temp1893=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1893->hd=( void*) _temp1876;
_temp1893->tl= 0; _temp1893;}); _temp1892;}); _temp1891->where_clause= 0;
_temp1891->body= _temp1881; _temp1891->loc= 0; _temp1891;}); struct Cyc_Absyn_Stmt*
_temp1883= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1801,({ struct Cyc_List_List*
_temp1890=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1890->hd=( void*) _temp1882; _temp1890->tl= 0; _temp1890;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1883);{ struct Cyc_Absyn_Exp* _temp1884= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1889=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1889->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1889->tl= 0; _temp1889;}), 0); struct Cyc_Absyn_Stmt*
_temp1885= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1888=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1888->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1888->tl= 0; _temp1888;}), 0), 0); struct Cyc_Absyn_Exp* _temp1886= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1887= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1885, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1886, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1884, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1887, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1873, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1883, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1734: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1807->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1805);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_Absyn_Exp* _temp1898[ 1u]; _temp1898[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1898, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), Cyc_Absyn_seq_stmt(
_temp1805, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp* _temp1899[ 1u]; _temp1899[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1899, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), 0), 0), 0), 0))->r)); return;} _LL1736:({ void* _temp1900[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("switchC_s", sizeof( unsigned char), 10u), _tag_arr( _temp1900, sizeof(
void*), 0u));}); return; _LL1738:({ void* _temp1901[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof(
unsigned char), 4u), _tag_arr( _temp1901, sizeof( void*), 0u));}); return;
_LL1740:({ void* _temp1902[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u),
_tag_arr( _temp1902, sizeof( void*), 0u));}); return; _LL1700:;}} struct
_tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp1903= f->args; for( 0;
_temp1903 !=  0; _temp1903=(( struct Cyc_List_List*) _check_null( _temp1903))->tl){(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1903))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1903))->hd)).f3);{ struct _tuple0* _temp1904=({ struct _tuple0* _temp1905=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp1905->f1=( void*) Cyc_Absyn_Loc_n;
_temp1905->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1903))->hd)).f1; _temp1905;}); nv= Cyc_Toc_add_varmap( nv, _temp1904, Cyc_Absyn_var_exp(
_temp1904, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp1908; void* _temp1910;
struct Cyc_Absyn_Tqual _temp1912; struct Cyc_Core_Opt* _temp1914; struct Cyc_Absyn_VarargInfo
_temp1906=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL1915: _temp1914= _temp1906.name; goto _LL1913; _LL1913: _temp1912= _temp1906.tq;
goto _LL1911; _LL1911: _temp1910=( void*) _temp1906.type; goto _LL1909; _LL1909:
_temp1908= _temp1906.inject; goto _LL1907; _LL1907: { void* _temp1916= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp1910,( void*) Cyc_Absyn_HeapRgn, _temp1912)); struct
_tuple0* _temp1917=({ struct _tuple0* _temp1920=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp1920->f1=( void*) Cyc_Absyn_Loc_n; _temp1920->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp1914))->v;
_temp1920;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp1918=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1918->hd=( void*)({ struct _tuple15* _temp1919=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp1919->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp1914))->v; _temp1919->f2= _temp1912;
_temp1919->f3= _temp1916; _temp1919;}); _temp1918->tl= 0; _temp1918;})); nv= Cyc_Toc_add_varmap(
nv, _temp1917, Cyc_Absyn_var_exp( _temp1917, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp1921=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp1921 !=  0; _temp1921=((
struct Cyc_List_List*) _check_null( _temp1921))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1921))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1921))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1922= s; _LL1924: if(
_temp1922 == ( void*) Cyc_Absyn_Abstract){ goto _LL1925;} else{ goto _LL1926;}
_LL1926: if( _temp1922 == ( void*) Cyc_Absyn_ExternC){ goto _LL1927;} else{ goto
_LL1928;} _LL1928: goto _LL1929; _LL1925: return( void*) Cyc_Absyn_Public;
_LL1927: return( void*) Cyc_Absyn_Extern; _LL1929: return s; _LL1923:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp1930=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1931=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1931->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp1931;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1932=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp1930).f2); if( _temp1932 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp1930).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1932))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Structdecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp1930).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp1933=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1933->v=(
void*) _temp1930; _temp1933;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp1934=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp1934 !=  0; _temp1934=(( struct Cyc_List_List*)
_check_null( _temp1934))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1934))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1934))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp1935=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp1936=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1936->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp1936;});}{ int
seen_defn_before; struct Cyc_Core_Opt* _temp1937=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)(( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp1935).f2); if( _temp1937 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp1935).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1937))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Uniondecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp1935).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp1938=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1938->v=(
void*) _temp1935; _temp1938;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp1939=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp1939 !=  0; _temp1939=(( struct Cyc_List_List*)
_check_null( _temp1939))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1939))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1939))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp1940=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1940->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp1940;});}{ struct
_tuple0* _temp1941= tud->name; if( tud->fields ==  0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp1941)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp1941));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp1942=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp1942 !=  0; _temp1942=(( struct Cyc_List_List*) _check_null(
_temp1942))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1942))->hd; if( f->typs ==  0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1943; _temp1943.q_const= 1;
_temp1943.q_volatile= 0; _temp1943.q_restrict= 0; _temp1943;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1944=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1944->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1945=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1945[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1946; _temp1946.tag= Cyc_Absyn_Var_d; _temp1946.f1= vd; _temp1946;});
_temp1945;}), 0); _temp1944->tl= Cyc_Toc_result_decls; _temp1944;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1947; _temp1947.q_const=
1; _temp1947.q_volatile= 0; _temp1947.q_restrict= 0; _temp1947;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1948=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1948->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1949=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1949[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1950; _temp1950.tag= Cyc_Absyn_Var_d; _temp1950.f1= vd; _temp1950;});
_temp1949;}), 0); _temp1948->tl= Cyc_Toc_result_decls; _temp1948;});{ struct Cyc_List_List*
_temp1951= 0; int i= 1;{ struct Cyc_List_List* _temp1952= f->typs; for( 0;
_temp1952 !=  0;( _temp1952=(( struct Cyc_List_List*) _check_null( _temp1952))->tl,
i ++)){ struct _tagged_arr* _temp1953= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield*
_temp1954=({ struct Cyc_Absyn_Structfield* _temp1956=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1956->name= _temp1953;
_temp1956->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp1952))->hd)).f1; _temp1956->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1952))->hd)).f2);
_temp1956->width= 0; _temp1956->attributes= 0; _temp1956;}); _temp1951=({ struct
Cyc_List_List* _temp1955=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1955->hd=( void*) _temp1954; _temp1955->tl= _temp1951; _temp1955;});}}
_temp1951=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1951); _temp1951=({ struct Cyc_List_List* _temp1957=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1957->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1958=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1958->name= Cyc_Toc_tag_sp; _temp1958->tq= Cyc_Toc_mt_tq; _temp1958->type=(
void*) Cyc_Absyn_sint_t; _temp1958->width= 0; _temp1958->attributes= 0;
_temp1958;}); _temp1957->tl= _temp1951; _temp1957;});{ struct Cyc_Absyn_Structdecl*
_temp1959=({ struct Cyc_Absyn_Structdecl* _temp1963=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp1963->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp1963->name=({ struct Cyc_Core_Opt* _temp1965=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1965->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp1965;}); _temp1963->tvs= 0; _temp1963->fields=({ struct Cyc_Core_Opt*
_temp1964=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1964->v=( void*) _temp1951; _temp1964;}); _temp1963->attributes= 0;
_temp1963;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1960=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1960->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp1961=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1961[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp1962; _temp1962.tag= Cyc_Absyn_Struct_d;
_temp1962.f1= _temp1959; _temp1962;}); _temp1961;}), 0); _temp1960->tl= Cyc_Toc_result_decls;
_temp1960;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp1966=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1966->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp1966;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp1967= xd->name; struct Cyc_List_List*
_temp1968=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp1968 !=  0; _temp1968=(( struct Cyc_List_List*) _check_null(
_temp1968))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1968))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp1969= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp1970=({ struct
Cyc_Absyn_ArrayType_struct* _temp2020=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2020[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2021; _temp2021.tag= Cyc_Absyn_ArrayType;
_temp2021.f1=( void*) Cyc_Absyn_uchar_t; _temp2021.f2= Cyc_Toc_mt_tq; _temp2021.f3=(
struct Cyc_Absyn_Exp*) _temp1969; _temp2021;}); _temp2020;}); struct Cyc_Core_Opt*
_temp1971=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp1979; int _temp1980; _LL1973: if( _temp1971 ==  0){ goto _LL1974;} else{
goto _LL1975;} _LL1975: if( _temp1971 ==  0){ goto _LL1977;} else{ _temp1979=*
_temp1971; _LL1981: _temp1980=( int) _temp1979.v; if( _temp1980 ==  0){ goto
_LL1976;} else{ goto _LL1977;}} _LL1977: goto _LL1978; _LL1974: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1987; _temp1987.tag= Cyc_Std_String_pa; _temp1987.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp1986; _temp1986.tag= Cyc_Std_Int_pa;
_temp1986.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp1985;
_temp1985.tag= Cyc_Std_Int_pa; _temp1985.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp1984; _temp1984.tag= Cyc_Std_Int_pa; _temp1984.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp1983; _temp1983.tag=
Cyc_Std_Int_pa; _temp1983.f1=( int)(( unsigned int) 0);{ void* _temp1982[ 5u]={&
_temp1983,& _temp1984,& _temp1985,& _temp1986,& _temp1987}; Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u), _tag_arr( _temp1982,
sizeof( void*), 5u));}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl* _temp1988= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp1970, initopt);( void*)( _temp1988->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1989=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1989->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1990=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1990[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1991; _temp1991.tag= Cyc_Absyn_Var_d; _temp1991.f1= _temp1988; _temp1991;});
_temp1990;}), 0); _temp1989->tl= Cyc_Toc_result_decls; _temp1989;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp1992= f->typs; for( 0; _temp1992 !=  0;( _temp1992=(( struct Cyc_List_List*)
_check_null( _temp1992))->tl, i ++)){ struct _tagged_arr* _temp1993=({ struct
_tagged_arr* _temp1997=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1997[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1999; _temp1999.tag= Cyc_Std_Int_pa; _temp1999.f1=( int)(( unsigned int) i);{
void* _temp1998[ 1u]={& _temp1999}; Cyc_Std_aprintf( _tag_arr("f%d", sizeof(
unsigned char), 4u), _tag_arr( _temp1998, sizeof( void*), 1u));}}); _temp1997;});
struct Cyc_Absyn_Structfield* _temp1994=({ struct Cyc_Absyn_Structfield*
_temp1996=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1996->name= _temp1993; _temp1996->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1992))->hd)).f1; _temp1996->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1992))->hd)).f2);
_temp1996->width= 0; _temp1996->attributes= 0; _temp1996;}); fields=({ struct
Cyc_List_List* _temp1995=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1995->hd=( void*) _temp1994; _temp1995->tl= fields; _temp1995;});}} fields=({
struct Cyc_List_List* _temp2000=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2000->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2001=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2001->name= Cyc_Toc_tag_sp; _temp2001->tq= Cyc_Toc_mt_tq; _temp2001->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2001->width=
0; _temp2001->attributes= 0; _temp2001;}); _temp2000->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2000;});{ struct Cyc_Absyn_Structdecl*
_temp2002=({ struct Cyc_Absyn_Structdecl* _temp2006=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2006->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2006->name=({ struct Cyc_Core_Opt* _temp2008=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2008->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2008;}); _temp2006->tvs= 0; _temp2006->fields=({ struct Cyc_Core_Opt*
_temp2007=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2007->v=( void*) fields; _temp2007;}); _temp2006->attributes= 0; _temp2006;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2003=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2003->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2004=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2004[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2005; _temp2005.tag= Cyc_Absyn_Struct_d;
_temp2005.f1= _temp2002; _temp2005;}); _temp2004;}), 0); _temp2003->tl= Cyc_Toc_result_decls;
_temp2003;});}} goto _LL1972;}} _LL1976: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2009= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2019; _temp2019.tag= Cyc_Std_String_pa;
_temp2019.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2018;
_temp2018.tag= Cyc_Std_Int_pa; _temp2018.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp2017; _temp2017.tag= Cyc_Std_Int_pa; _temp2017.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2016; _temp2016.tag=
Cyc_Std_Int_pa; _temp2016.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct
_temp2015; _temp2015.tag= Cyc_Std_Int_pa; _temp2015.f1=( int)(( unsigned int) 0);{
void* _temp2014[ 5u]={& _temp2015,& _temp2016,& _temp2017,& _temp2018,&
_temp2019}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u),
_tag_arr( _temp2014, sizeof( void*), 5u));}}}}}}), 0); struct Cyc_Absyn_Vardecl*
_temp2010= Cyc_Absyn_new_vardecl( f->name,( void*) _temp1970,( struct Cyc_Absyn_Exp*)
_temp2009);( void*)( _temp2010->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2011=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2011->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2012=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2012[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2013; _temp2013.tag= Cyc_Absyn_Var_d; _temp2013.f1= _temp2010; _temp2013;});
_temp2012;}), 0); _temp2011->tl= Cyc_Toc_result_decls; _temp2011;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL1972; _LL1978: goto
_LL1972; _LL1972:;}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2022= ed->name;( void*)( ed->sc=(
void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ struct Cyc_List_List*
_temp2023=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; _temp2023 !=  0; _temp2023=(( struct Cyc_List_List*) _check_null(
_temp2023))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2023))->hd)->tag !=  0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2023))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2023))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2024=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2030; struct Cyc_Absyn_Exp* _temp2032; struct Cyc_Absyn_Vardecl* _temp2034;
_LL2026: if(*(( int*) _temp2024) ==  Cyc_Absyn_Comprehension_e){ _LL2035:
_temp2034=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2024)->f1; goto
_LL2033; _LL2033: _temp2032=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2024)->f2; goto _LL2031; _LL2031: _temp2030=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2024)->f3; goto _LL2027;} else{ goto _LL2028;} _LL2028: goto _LL2029;
_LL2027: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2034, _temp2032, _temp2030, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2025; _LL2029: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2036= Cyc_Toc_copy_env( init_nv);
_temp2036->toplevel= 1; Cyc_Toc_exp_to_c( _temp2036, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2025; _LL2025:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2037= _new_region(); struct _RegionHandle* prgn=& _temp2037;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2040; struct Cyc_List_List*
_temp2042; struct Cyc_Toc_Env* _temp2044; struct _tuple11 _temp2038= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2045: _temp2044= _temp2038.f1; goto _LL2043; _LL2043: _temp2042=
_temp2038.f2; goto _LL2041; _LL2041: _temp2040= _temp2038.f3; goto _LL2039;
_LL2039: Cyc_Toc_stmt_to_c( _temp2044, s);{ struct Cyc_Absyn_Stmt* _temp2046=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2040, _temp2046, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2047= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2048= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2047, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2040,
Cyc_Absyn_seq_stmt( _temp2048, _temp2046, 0), 0), 0);} for( 0; _temp2042 !=  0;
_temp2042=(( struct Cyc_List_List*) _check_null( _temp2042))->tl){ struct
_tuple12 _temp2051; void* _temp2052; struct _tuple0* _temp2054; struct _tuple12*
_temp2049=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2042))->hd;
_temp2051=* _temp2049; _LL2055: _temp2054= _temp2051.f1; goto _LL2053; _LL2053:
_temp2052= _temp2051.f2; goto _LL2050; _LL2050: s= Cyc_Absyn_declare_stmt(
_temp2054, _temp2052, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ void* _temp2056[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u), _tag_arr( _temp2056, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2057=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2083; struct Cyc_Absyn_Fndecl* _temp2085; struct Cyc_Absyn_Structdecl*
_temp2087; struct Cyc_Absyn_Uniondecl* _temp2089; struct Cyc_Absyn_Tuniondecl*
_temp2091; struct Cyc_Absyn_Enumdecl* _temp2093; struct Cyc_Absyn_Typedefdecl*
_temp2095; struct Cyc_List_List* _temp2097; struct Cyc_List_List* _temp2099;
struct Cyc_List_List* _temp2101; _LL2059: if(*(( int*) _temp2057) ==  Cyc_Absyn_Var_d){
_LL2084: _temp2083=(( struct Cyc_Absyn_Var_d_struct*) _temp2057)->f1; goto
_LL2060;} else{ goto _LL2061;} _LL2061: if(*(( int*) _temp2057) ==  Cyc_Absyn_Fn_d){
_LL2086: _temp2085=(( struct Cyc_Absyn_Fn_d_struct*) _temp2057)->f1; goto
_LL2062;} else{ goto _LL2063;} _LL2063: if(*(( int*) _temp2057) ==  Cyc_Absyn_Let_d){
goto _LL2064;} else{ goto _LL2065;} _LL2065: if(*(( int*) _temp2057) ==  Cyc_Absyn_Letv_d){
goto _LL2066;} else{ goto _LL2067;} _LL2067: if(*(( int*) _temp2057) ==  Cyc_Absyn_Struct_d){
_LL2088: _temp2087=(( struct Cyc_Absyn_Struct_d_struct*) _temp2057)->f1; goto
_LL2068;} else{ goto _LL2069;} _LL2069: if(*(( int*) _temp2057) ==  Cyc_Absyn_Union_d){
_LL2090: _temp2089=(( struct Cyc_Absyn_Union_d_struct*) _temp2057)->f1; goto
_LL2070;} else{ goto _LL2071;} _LL2071: if(*(( int*) _temp2057) ==  Cyc_Absyn_Tunion_d){
_LL2092: _temp2091=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2057)->f1; goto
_LL2072;} else{ goto _LL2073;} _LL2073: if(*(( int*) _temp2057) ==  Cyc_Absyn_Enum_d){
_LL2094: _temp2093=(( struct Cyc_Absyn_Enum_d_struct*) _temp2057)->f1; goto
_LL2074;} else{ goto _LL2075;} _LL2075: if(*(( int*) _temp2057) ==  Cyc_Absyn_Typedef_d){
_LL2096: _temp2095=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2057)->f1; goto
_LL2076;} else{ goto _LL2077;} _LL2077: if(*(( int*) _temp2057) ==  Cyc_Absyn_Namespace_d){
_LL2098: _temp2097=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2057)->f2; goto
_LL2078;} else{ goto _LL2079;} _LL2079: if(*(( int*) _temp2057) ==  Cyc_Absyn_Using_d){
_LL2100: _temp2099=(( struct Cyc_Absyn_Using_d_struct*) _temp2057)->f2; goto
_LL2080;} else{ goto _LL2081;} _LL2081: if(*(( int*) _temp2057) ==  Cyc_Absyn_ExternC_d){
_LL2102: _temp2101=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2057)->f1; goto
_LL2082;} else{ goto _LL2058;} _LL2060: { struct _tuple0* _temp2103= _temp2083->name;
if(( void*) _temp2083->sc == ( void*) Cyc_Absyn_ExternC){ _temp2103=({ struct
_tuple0* _temp2104=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2104->f1= Cyc_Absyn_rel_ns_null; _temp2104->f2=(* _temp2103).f2; _temp2104;});}
if( _temp2083->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2083->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2083->name,
Cyc_Absyn_varb_exp( _temp2103,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2105=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2105[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2106; _temp2106.tag= Cyc_Absyn_Global_b;
_temp2106.f1= _temp2083; _temp2106;}); _temp2105;}), 0)); _temp2083->name=
_temp2103;( void*)( _temp2083->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2083->sc));(
void*)( _temp2083->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2083->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2107=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2107->hd=( void*) d; _temp2107->tl=
Cyc_Toc_result_decls; _temp2107;}); goto _LL2058;} _LL2062: { struct _tuple0*
_temp2108= _temp2085->name; if(( void*) _temp2085->sc == ( void*) Cyc_Absyn_ExternC){
_temp2108=({ struct _tuple0* _temp2109=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2109->f1= Cyc_Absyn_rel_ns_null; _temp2109->f2=(*
_temp2108).f2; _temp2109;});} nv= Cyc_Toc_add_varmap( nv, _temp2085->name, Cyc_Absyn_var_exp(
_temp2108, 0)); _temp2085->name= _temp2108; Cyc_Toc_fndecl_to_c( nv, _temp2085);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2110=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2110->hd=( void*) d; _temp2110->tl=
Cyc_Toc_result_decls; _temp2110;}); goto _LL2058;} _LL2064: goto _LL2066;
_LL2066:({ void* _temp2111[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2111, sizeof( void*), 0u));}); goto _LL2058;
_LL2068: Cyc_Toc_structdecl_to_c( _temp2087); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2112=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2112->hd=( void*) d; _temp2112->tl= Cyc_Toc_result_decls; _temp2112;});
goto _LL2058; _LL2070: Cyc_Toc_uniondecl_to_c( _temp2089); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2113=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2113->hd=( void*) d; _temp2113->tl= Cyc_Toc_result_decls;
_temp2113;}); goto _LL2058; _LL2072: if( _temp2091->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2091);} else{ Cyc_Toc_tuniondecl_to_c( _temp2091);} goto _LL2058; _LL2074:
Cyc_Toc_enumdecl_to_c( nv, _temp2093); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2114=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2114->hd=( void*) d; _temp2114->tl= Cyc_Toc_result_decls; _temp2114;});
goto _LL2058; _LL2076: _temp2095->name= _temp2095->name; _temp2095->tvs= 0;(
void*)( _temp2095->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2095->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2115->hd=( void*) d; _temp2115->tl=
Cyc_Toc_result_decls; _temp2115;}); goto _LL2058; _LL2078: _temp2099= _temp2097;
goto _LL2080; _LL2080: _temp2101= _temp2099; goto _LL2082; _LL2082: nv= Cyc_Toc_decls_to_c(
nv, _temp2101, top); goto _LL2058; _LL2058:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
