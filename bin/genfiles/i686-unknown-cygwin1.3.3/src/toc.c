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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Gentyp_e=
19; struct Cyc_Absyn_Gentyp_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_Deref_e= 20; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_StructMember_e= 21; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 22; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
23; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 24; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
25; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 26; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
27; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
28; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 29; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 30;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
xexp; struct Cyc_Absyn_Exp* xplussz; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp516=( struct Cyc_Absyn_ArrayType_struct*)
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
_temp513;}); xexp= Cyc_Absyn_var_exp( x, 0); xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0);} else{ xexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), e, 0);
xplussz= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_add_exp( e, sz,
0), 0);}{ struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
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
_temp876; void* _temp878; struct _tuple0* _temp880; struct _tuple0* _temp882;
struct Cyc_List_List* _temp884; void* _temp886; void* _temp888; struct Cyc_Absyn_Exp*
_temp890; struct Cyc_Absyn_Exp* _temp892; struct Cyc_Core_Opt* _temp894; struct
Cyc_Absyn_Exp* _temp896; struct Cyc_Absyn_Exp* _temp898; struct Cyc_Absyn_Exp*
_temp900; struct Cyc_Absyn_Exp* _temp902; struct Cyc_Absyn_Exp* _temp904; struct
Cyc_Absyn_Exp* _temp906; struct Cyc_List_List* _temp908; struct Cyc_Absyn_Exp*
_temp910; struct Cyc_Absyn_VarargCallInfo* _temp912; struct Cyc_List_List*
_temp914; struct Cyc_Absyn_Exp* _temp916; struct Cyc_Absyn_VarargCallInfo*
_temp918; struct Cyc_Absyn_VarargCallInfo _temp920; struct Cyc_Absyn_VarargInfo*
_temp921; struct Cyc_List_List* _temp923; int _temp925; struct Cyc_List_List*
_temp927; struct Cyc_Absyn_Exp* _temp929; struct Cyc_Absyn_Exp* _temp931; struct
Cyc_Absyn_Exp* _temp933; struct Cyc_List_List* _temp935; struct Cyc_Absyn_Exp*
_temp937; struct Cyc_Absyn_Exp* _temp939; void* _temp941; void** _temp943;
struct Cyc_Absyn_Exp* _temp944; struct Cyc_Absyn_Exp* _temp946; struct Cyc_Absyn_Exp*
_temp948; struct Cyc_Absyn_Exp* _temp950; void* _temp952; void* _temp954; struct
_tagged_arr* _temp956; void* _temp958; struct Cyc_Absyn_Exp* _temp960; struct
_tagged_arr* _temp962; struct Cyc_Absyn_Exp* _temp964; struct _tagged_arr*
_temp966; struct Cyc_Absyn_Exp* _temp968; struct Cyc_Absyn_Exp* _temp970; struct
Cyc_Absyn_Exp* _temp972; struct Cyc_List_List* _temp974; struct Cyc_List_List*
_temp976; struct Cyc_Absyn_Exp* _temp978; struct Cyc_Absyn_Exp* _temp980; struct
Cyc_Absyn_Vardecl* _temp982; struct Cyc_Absyn_Structdecl* _temp984; struct Cyc_List_List*
_temp986; struct Cyc_Core_Opt* _temp988; struct _tuple0* _temp990; struct Cyc_List_List*
_temp992; void* _temp994; struct Cyc_Absyn_Tunionfield* _temp996; struct Cyc_Absyn_Tuniondecl*
_temp998; struct Cyc_List_List* _temp1000; struct Cyc_Absyn_Tunionfield*
_temp1002; struct Cyc_Absyn_Tuniondecl* _temp1004; struct Cyc_List_List*
_temp1006; void* _temp1008; struct Cyc_Absyn_Exp* _temp1010; struct Cyc_Absyn_Stmt*
_temp1012; struct Cyc_Absyn_Fndecl* _temp1014; _LL796: if(*(( int*) _temp794) == 
Cyc_Absyn_Const_e){ _LL877: _temp876=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp794)->f1; if( _temp876 == ( void*) Cyc_Absyn_Null_c){ goto _LL797;} else{
goto _LL798;}} else{ goto _LL798;} _LL798: if(*(( int*) _temp794) ==  Cyc_Absyn_Const_e){
goto _LL799;} else{ goto _LL800;} _LL800: if(*(( int*) _temp794) ==  Cyc_Absyn_Var_e){
_LL881: _temp880=(( struct Cyc_Absyn_Var_e_struct*) _temp794)->f1; goto _LL879;
_LL879: _temp878=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp794)->f2; goto
_LL801;} else{ goto _LL802;} _LL802: if(*(( int*) _temp794) ==  Cyc_Absyn_UnknownId_e){
_LL883: _temp882=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp794)->f1; goto
_LL803;} else{ goto _LL804;} _LL804: if(*(( int*) _temp794) ==  Cyc_Absyn_Primop_e){
_LL887: _temp886=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp794)->f1;
goto _LL885; _LL885: _temp884=(( struct Cyc_Absyn_Primop_e_struct*) _temp794)->f2;
goto _LL805;} else{ goto _LL806;} _LL806: if(*(( int*) _temp794) ==  Cyc_Absyn_Increment_e){
_LL891: _temp890=(( struct Cyc_Absyn_Increment_e_struct*) _temp794)->f1; goto
_LL889; _LL889: _temp888=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp794)->f2; goto _LL807;} else{ goto _LL808;} _LL808: if(*(( int*) _temp794)
==  Cyc_Absyn_AssignOp_e){ _LL897: _temp896=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp794)->f1; goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp794)->f2; goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp794)->f3; goto _LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp794)
==  Cyc_Absyn_Conditional_e){ _LL903: _temp902=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp794)->f1; goto _LL901; _LL901: _temp900=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp794)->f2; goto _LL899; _LL899: _temp898=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp794)->f3; goto _LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp794)
==  Cyc_Absyn_SeqExp_e){ _LL907: _temp906=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp794)->f1; goto _LL905; _LL905: _temp904=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp794)->f2; goto _LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp794)
==  Cyc_Absyn_UnknownCall_e){ _LL911: _temp910=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp794)->f1; goto _LL909; _LL909: _temp908=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp794)->f2; goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp794)
==  Cyc_Absyn_FnCall_e){ _LL917: _temp916=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp794)->f1; goto _LL915; _LL915: _temp914=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp794)->f2; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp794)->f3; if( _temp912 ==  0){ goto _LL817;} else{ goto _LL818;}} else{
goto _LL818;} _LL818: if(*(( int*) _temp794) ==  Cyc_Absyn_FnCall_e){ _LL930:
_temp929=(( struct Cyc_Absyn_FnCall_e_struct*) _temp794)->f1; goto _LL928;
_LL928: _temp927=(( struct Cyc_Absyn_FnCall_e_struct*) _temp794)->f2; goto
_LL919; _LL919: _temp918=(( struct Cyc_Absyn_FnCall_e_struct*) _temp794)->f3;
if( _temp918 ==  0){ goto _LL820;} else{ _temp920=* _temp918; _LL926: _temp925=
_temp920.num_varargs; goto _LL924; _LL924: _temp923= _temp920.injectors; goto
_LL922; _LL922: _temp921= _temp920.vai; goto _LL819;}} else{ goto _LL820;}
_LL820: if(*(( int*) _temp794) ==  Cyc_Absyn_Throw_e){ _LL932: _temp931=((
struct Cyc_Absyn_Throw_e_struct*) _temp794)->f1; goto _LL821;} else{ goto _LL822;}
_LL822: if(*(( int*) _temp794) ==  Cyc_Absyn_NoInstantiate_e){ _LL934: _temp933=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp794)->f1; goto _LL823;} else{
goto _LL824;} _LL824: if(*(( int*) _temp794) ==  Cyc_Absyn_Instantiate_e){
_LL938: _temp937=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp794)->f1; goto
_LL936; _LL936: _temp935=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp794)->f2;
goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp794) ==  Cyc_Absyn_Cast_e){
_LL942: _temp941=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp794)->f1;
_temp943=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp794)->f1);
goto _LL940; _LL940: _temp939=(( struct Cyc_Absyn_Cast_e_struct*) _temp794)->f2;
goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp794) ==  Cyc_Absyn_Address_e){
_LL945: _temp944=(( struct Cyc_Absyn_Address_e_struct*) _temp794)->f1; goto
_LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp794) ==  Cyc_Absyn_New_e){
_LL949: _temp948=(( struct Cyc_Absyn_New_e_struct*) _temp794)->f1; goto _LL947;
_LL947: _temp946=(( struct Cyc_Absyn_New_e_struct*) _temp794)->f2; goto _LL831;}
else{ goto _LL832;} _LL832: if(*(( int*) _temp794) ==  Cyc_Absyn_Sizeofexp_e){
_LL951: _temp950=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp794)->f1; goto
_LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp794) ==  Cyc_Absyn_Sizeoftyp_e){
_LL953: _temp952=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp794)->f1;
goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp794) ==  Cyc_Absyn_Gentyp_e){
_LL955: _temp954=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp794)->f1;
goto _LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp794) ==  Cyc_Absyn_Offsetof_e){
_LL959: _temp958=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp794)->f1;
goto _LL957; _LL957: _temp956=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp794)->f2;
goto _LL839;} else{ goto _LL840;} _LL840: if(*(( int*) _temp794) ==  Cyc_Absyn_Deref_e){
_LL961: _temp960=(( struct Cyc_Absyn_Deref_e_struct*) _temp794)->f1; goto _LL841;}
else{ goto _LL842;} _LL842: if(*(( int*) _temp794) ==  Cyc_Absyn_StructMember_e){
_LL965: _temp964=(( struct Cyc_Absyn_StructMember_e_struct*) _temp794)->f1; goto
_LL963; _LL963: _temp962=(( struct Cyc_Absyn_StructMember_e_struct*) _temp794)->f2;
goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp794) ==  Cyc_Absyn_StructArrow_e){
_LL969: _temp968=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp794)->f1; goto
_LL967; _LL967: _temp966=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp794)->f2;
goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp794) ==  Cyc_Absyn_Subscript_e){
_LL973: _temp972=(( struct Cyc_Absyn_Subscript_e_struct*) _temp794)->f1; goto
_LL971; _LL971: _temp970=(( struct Cyc_Absyn_Subscript_e_struct*) _temp794)->f2;
goto _LL847;} else{ goto _LL848;} _LL848: if(*(( int*) _temp794) ==  Cyc_Absyn_Tuple_e){
_LL975: _temp974=(( struct Cyc_Absyn_Tuple_e_struct*) _temp794)->f1; goto _LL849;}
else{ goto _LL850;} _LL850: if(*(( int*) _temp794) ==  Cyc_Absyn_Array_e){
_LL977: _temp976=(( struct Cyc_Absyn_Array_e_struct*) _temp794)->f1; goto _LL851;}
else{ goto _LL852;} _LL852: if(*(( int*) _temp794) ==  Cyc_Absyn_Comprehension_e){
_LL983: _temp982=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp794)->f1;
goto _LL981; _LL981: _temp980=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp794)->f2; goto _LL979; _LL979: _temp978=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp794)->f3; goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp794)
==  Cyc_Absyn_Struct_e){ _LL991: _temp990=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f1; goto _LL989; _LL989: _temp988=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f2; goto _LL987; _LL987: _temp986=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f3; goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_Struct_e_struct*)
_temp794)->f4; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp794)
==  Cyc_Absyn_AnonStruct_e){ _LL995: _temp994=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp794)->f1; goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp794)->f2; goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp794)
==  Cyc_Absyn_Tunion_e){ _LL1001: _temp1000=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp794)->f3; if( _temp1000 ==  0){ goto _LL999;} else{ goto _LL860;} _LL999:
_temp998=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f4; goto _LL997;
_LL997: _temp996=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f5; goto
_LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp794) ==  Cyc_Absyn_Tunion_e){
_LL1007: _temp1006=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f3; goto
_LL1005; _LL1005: _temp1004=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f4;
goto _LL1003; _LL1003: _temp1002=(( struct Cyc_Absyn_Tunion_e_struct*) _temp794)->f5;
goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp794) ==  Cyc_Absyn_Enum_e){
goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp794) ==  Cyc_Absyn_Malloc_e){
_LL1011: _temp1010=(( struct Cyc_Absyn_Malloc_e_struct*) _temp794)->f1; goto
_LL1009; _LL1009: _temp1008=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp794)->f2; goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp794)
==  Cyc_Absyn_StmtExp_e){ _LL1013: _temp1012=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp794)->f1; goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp794)
==  Cyc_Absyn_UnresolvedMem_e){ goto _LL869;} else{ goto _LL870;} _LL870: if(*((
int*) _temp794) ==  Cyc_Absyn_CompoundLit_e){ goto _LL871;} else{ goto _LL872;}
_LL872: if(*(( int*) _temp794) ==  Cyc_Absyn_Codegen_e){ _LL1015: _temp1014=((
struct Cyc_Absyn_Codegen_e_struct*) _temp794)->f1; goto _LL873;} else{ goto
_LL874;} _LL874: if(*(( int*) _temp794) ==  Cyc_Absyn_Fill_e){ goto _LL875;}
else{ goto _LL795;} _LL797: { struct Cyc_Absyn_Exp* _temp1016= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1016,
_temp1016))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1017[ 3u]; _temp1017[ 2u]=
_temp1016; _temp1017[ 1u]= _temp1016; _temp1017[ 0u]= _temp1016;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1017, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL795;} _LL799: goto _LL795; _LL801:{ struct _handler_cons
_temp1018; _push_handler(& _temp1018);{ int _temp1020= 0; if( setjmp( _temp1018.handler)){
_temp1020= 1;} if( ! _temp1020){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp880))->r));; _pop_handler();} else{ void* _temp1019=( void*) _exn_thrown;
void* _temp1022= _temp1019; _LL1024: if( _temp1022 ==  Cyc_Dict_Absent){ goto
_LL1025;} else{ goto _LL1026;} _LL1026: goto _LL1027; _LL1025:({ struct Cyc_Std_String_pa_struct
_temp1029; _temp1029.tag= Cyc_Std_String_pa; _temp1029.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp880);{ void* _temp1028[ 1u]={& _temp1029};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1028, sizeof( void*), 1u));}});
return; _LL1027:( void) _throw( _temp1022); _LL1023:;}}} goto _LL795; _LL803:({
void* _temp1030[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1030, sizeof( void*), 0u));}); goto _LL795; _LL805: { struct Cyc_List_List*
_temp1031=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp884);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp884);{
void* _temp1032= _temp886; _LL1034: if( _temp1032 == ( void*) Cyc_Absyn_Size){
goto _LL1035;} else{ goto _LL1036;} _LL1036: if( _temp1032 == ( void*) Cyc_Absyn_Plus){
goto _LL1037;} else{ goto _LL1038;} _LL1038: if( _temp1032 == ( void*) Cyc_Absyn_Minus){
goto _LL1039;} else{ goto _LL1040;} _LL1040: if( _temp1032 == ( void*) Cyc_Absyn_Eq){
goto _LL1041;} else{ goto _LL1042;} _LL1042: if( _temp1032 == ( void*) Cyc_Absyn_Neq){
goto _LL1043;} else{ goto _LL1044;} _LL1044: if( _temp1032 == ( void*) Cyc_Absyn_Gt){
goto _LL1045;} else{ goto _LL1046;} _LL1046: if( _temp1032 == ( void*) Cyc_Absyn_Gte){
goto _LL1047;} else{ goto _LL1048;} _LL1048: if( _temp1032 == ( void*) Cyc_Absyn_Lt){
goto _LL1049;} else{ goto _LL1050;} _LL1050: if( _temp1032 == ( void*) Cyc_Absyn_Lte){
goto _LL1051;} else{ goto _LL1052;} _LL1052: goto _LL1053; _LL1035: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp884))->hd;{
void* _temp1054= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1062; struct Cyc_Absyn_PtrInfo
_temp1064; struct Cyc_Absyn_Conref* _temp1066; void* _temp1068; _LL1056: if((
unsigned int) _temp1054 >  4u?*(( int*) _temp1054) ==  Cyc_Absyn_ArrayType: 0){
_LL1063: _temp1062=(( struct Cyc_Absyn_ArrayType_struct*) _temp1054)->f3; goto
_LL1057;} else{ goto _LL1058;} _LL1058: if(( unsigned int) _temp1054 >  4u?*((
int*) _temp1054) ==  Cyc_Absyn_PointerType: 0){ _LL1065: _temp1064=(( struct Cyc_Absyn_PointerType_struct*)
_temp1054)->f1; _LL1069: _temp1068=( void*) _temp1064.elt_typ; goto _LL1067;
_LL1067: _temp1066= _temp1064.bounds; goto _LL1059;} else{ goto _LL1060;}
_LL1060: goto _LL1061; _LL1057:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1062))->r)); goto _LL1055; _LL1059:{ void* _temp1070= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1066); struct Cyc_Absyn_Exp* _temp1076; _LL1072: if(
_temp1070 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1073;} else{ goto _LL1074;}
_LL1074: if(( unsigned int) _temp1070 >  1u?*(( int*) _temp1070) ==  Cyc_Absyn_Upper_b:
0){ _LL1077: _temp1076=(( struct Cyc_Absyn_Upper_b_struct*) _temp1070)->f1; goto
_LL1075;} else{ goto _LL1071;} _LL1073:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1078[ 2u]; _temp1078[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1068), 0); _temp1078[ 0u]=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp884))->hd;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1078, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r)); goto _LL1071;
_LL1075:( void*)( e->r=( void*)(( void*) _temp1076->r)); goto _LL1071; _LL1071:;}
goto _LL1055; _LL1061:({ struct Cyc_Std_String_pa_struct _temp1081; _temp1081.tag=
Cyc_Std_String_pa; _temp1081.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp1080; _temp1080.tag= Cyc_Std_String_pa; _temp1080.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
void* _temp1079[ 2u]={& _temp1080,& _temp1081};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u), _tag_arr( _temp1079, sizeof( void*), 2u));}}});
return; _LL1055:;} goto _LL1033;} _LL1037: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1031))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp884))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp884))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1082[ 3u]; _temp1082[ 2u]=
e2; _temp1082[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1082[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1082, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1033;} _LL1039: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1031))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp884))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp884))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1031))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1083[ 3u]; _temp1083[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1083[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1083[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1083, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1033;} _LL1041: goto _LL1043; _LL1043: goto _LL1045; _LL1045: goto
_LL1047; _LL1047: goto _LL1049; _LL1049: goto _LL1051; _LL1051: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp884))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp884))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1031))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1031))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1033;} _LL1053: goto
_LL1033; _LL1033:;} goto _LL795;} _LL807: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp890->topt))->v; Cyc_Toc_exp_to_c( nv, _temp890);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1084=( _temp888 == ( void*) Cyc_Absyn_PostInc?
1: _temp888 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1085=( _temp888 == ( void*) Cyc_Absyn_PreInc?
1: _temp888 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1084,({ struct Cyc_Absyn_Exp* _temp1086[ 3u];
_temp1086[ 2u]= Cyc_Absyn_signed_int_exp( _temp1085, 0); _temp1086[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1086[ 0u]= Cyc_Absyn_address_exp( _temp890,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1086, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp890): 0){ Cyc_Toc_lvalue_assign(
_temp890, 0, Cyc_Toc_incr_lvalue, _temp888);( void*)( e->r=( void*)(( void*)
_temp890->r));} goto _LL795;}} _LL809: { int e1_poly= Cyc_Toc_is_poly_project(
_temp896); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp896->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp892->topt))->v; Cyc_Toc_exp_to_c( nv, _temp896); Cyc_Toc_exp_to_c(
nv, _temp892);{ int done= 0; if( _temp894 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1087=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp894))->v;
_LL1089: if( _temp1087 == ( void*) Cyc_Absyn_Plus){ goto _LL1090;} else{ goto
_LL1091;} _LL1091: if( _temp1087 == ( void*) Cyc_Absyn_Minus){ goto _LL1092;}
else{ goto _LL1093;} _LL1093: goto _LL1094; _LL1090: change= _temp892; goto
_LL1088; _LL1092: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp892,
0); goto _LL1088; _LL1094:( int) _throw(({ void* _temp1095[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u), _tag_arr(
_temp1095, sizeof( void*), 0u));})); goto _LL1088; _LL1088:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp* _temp1096[ 3u]; _temp1096[ 2u]= change; _temp1096[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); _temp1096[ 0u]= Cyc_Absyn_address_exp(
_temp896, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1096, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} if( !
done){ if( e1_poly){( void*)( _temp892->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*) _temp892->r, 0), 0))->r));}
if( ! Cyc_Absyn_is_lvalue( _temp896)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)( _temp896, 0,
Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1097=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7) *  1); _temp1097[ 0]=({ struct _tuple7
_temp1098; _temp1098.f1= _temp894; _temp1098.f2= _temp892; _temp1098;});
_temp1097;}));( void*)( e->r=( void*)(( void*) _temp896->r));}} goto _LL795;}}
_LL811: Cyc_Toc_exp_to_c( nv, _temp902); Cyc_Toc_exp_to_c( nv, _temp900); Cyc_Toc_exp_to_c(
nv, _temp898); goto _LL795; _LL813: Cyc_Toc_exp_to_c( nv, _temp906); Cyc_Toc_exp_to_c(
nv, _temp904); goto _LL795; _LL815: _temp916= _temp910; _temp914= _temp908; goto
_LL817; _LL817: Cyc_Toc_exp_to_c( nv, _temp916);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp914); goto _LL795; _LL819:{
struct _RegionHandle _temp1099= _new_region(); struct _RegionHandle* r=&
_temp1099; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp925, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp921->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1116=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1116[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1117; _temp1117.tag= Cyc_Absyn_ArrayType;
_temp1117.f1=( void*) cva_type; _temp1117.f2= Cyc_Toc_mt_tq; _temp1117.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1117;}); _temp1116;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp927); int num_normargs=
num_args -  _temp925; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp927=(( struct Cyc_List_List*) _check_null( _temp927))->tl)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd); new_args=({ struct Cyc_List_List* _temp1100=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1100->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp927))->hd);
_temp1100->tl= new_args; _temp1100;});}} new_args=({ struct Cyc_List_List*
_temp1101=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1101->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1102[ 3u]; _temp1102[ 2u]= num_varargs_exp; _temp1102[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1102[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1102, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1101->tl= new_args; _temp1101;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp929);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp929, new_args, 0), 0); if( _temp921->inject){ struct _tagged_arr vs=({
unsigned int _temp1112=( unsigned int) _temp925; struct _tuple0** _temp1113=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1112)); struct _tagged_arr _temp1115= _tag_arr( _temp1113, sizeof( struct
_tuple0*),( unsigned int) _temp925);{ unsigned int _temp1114= _temp1112;
unsigned int i; for( i= 0; i <  _temp1114; i ++){ _temp1113[ i]= Cyc_Toc_temp_var();}};
_temp1115;}); struct Cyc_List_List* _temp1103= 0;{ int i= _temp925 -  1; for( 0;
i >=  0; -- i){ _temp1103=({ struct Cyc_List_List* _temp1104=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1104->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1104->tl=
_temp1103; _temp1104;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1103, 0), s, 0);{ int i= 0;
for( 0; _temp927 !=  0;((( _temp927=(( struct Cyc_List_List*) _check_null(
_temp927))->tl, _temp923=(( struct Cyc_List_List*) _check_null( _temp923))->tl)),
++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd; void* arg_type=( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v; struct _tuple0* var=*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)); struct Cyc_Absyn_Exp*
varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield _temp1107;
struct Cyc_List_List* _temp1108; struct _tuple0* _temp1110; struct Cyc_Absyn_Tunionfield*
_temp1105=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp923))->hd; _temp1107=* _temp1105; _LL1111: _temp1110= _temp1107.name; goto
_LL1109; _LL1109: _temp1108= _temp1107.typs; goto _LL1106; _LL1106: { void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1108))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Absyn_fieldname(
1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1110, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1110, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp927
!=  0;( _temp927=(( struct Cyc_List_List*) _check_null( _temp927))->tl, ++ i)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp927))->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL795; _LL821: Cyc_Toc_exp_to_c( nv,
_temp931);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp931), 0))->r)); goto _LL795; _LL823: Cyc_Toc_exp_to_c(
nv, _temp933); goto _LL795; _LL825: Cyc_Toc_exp_to_c( nv, _temp937); for( 0;
_temp935 !=  0; _temp935=(( struct Cyc_List_List*) _check_null( _temp935))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp935))->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1118= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp935))->hd); _LL1120: if(( unsigned int) _temp1118 >  4u?*((
int*) _temp1118) ==  Cyc_Absyn_VarType: 0){ goto _LL1121;} else{ goto _LL1122;}
_LL1122: if(( unsigned int) _temp1118 >  4u?*(( int*) _temp1118) ==  Cyc_Absyn_TunionType:
0){ goto _LL1123;} else{ goto _LL1124;} _LL1124: goto _LL1125; _LL1121:
continue; _LL1123: continue; _LL1125:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp937, 0))->r)); goto _LL1119; _LL1119:;} break;}} goto _LL795; _LL827: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp939->topt))->v;
void* new_typ=* _temp943;* _temp943= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp939);{ struct _tuple8 _temp1127=({ struct _tuple8 _temp1126; _temp1126.f1=
Cyc_Tcutil_compress( old_t2); _temp1126.f2= Cyc_Tcutil_compress( new_typ);
_temp1126;}); void* _temp1135; struct Cyc_Absyn_PtrInfo _temp1137; void*
_temp1139; struct Cyc_Absyn_PtrInfo _temp1141; void* _temp1143; void* _temp1145;
struct Cyc_Absyn_PtrInfo _temp1147; _LL1129: _LL1140: _temp1139= _temp1127.f1;
if(( unsigned int) _temp1139 >  4u?*(( int*) _temp1139) ==  Cyc_Absyn_PointerType:
0){ _LL1142: _temp1141=(( struct Cyc_Absyn_PointerType_struct*) _temp1139)->f1;
goto _LL1136;} else{ goto _LL1131;} _LL1136: _temp1135= _temp1127.f2; if((
unsigned int) _temp1135 >  4u?*(( int*) _temp1135) ==  Cyc_Absyn_PointerType: 0){
_LL1138: _temp1137=(( struct Cyc_Absyn_PointerType_struct*) _temp1135)->f1; goto
_LL1130;} else{ goto _LL1131;} _LL1131: _LL1146: _temp1145= _temp1127.f1; if((
unsigned int) _temp1145 >  4u?*(( int*) _temp1145) ==  Cyc_Absyn_PointerType: 0){
_LL1148: _temp1147=(( struct Cyc_Absyn_PointerType_struct*) _temp1145)->f1; goto
_LL1144;} else{ goto _LL1133;} _LL1144: _temp1143= _temp1127.f2; if((
unsigned int) _temp1143 >  4u?*(( int*) _temp1143) ==  Cyc_Absyn_IntType: 0){
goto _LL1132;} else{ goto _LL1133;} _LL1133: goto _LL1134; _LL1130: { int
_temp1149=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1141.nullable); int _temp1150=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1137.nullable); void* _temp1151= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1141.bounds); void* _temp1152= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1137.bounds);{ struct _tuple8 _temp1154=({ struct
_tuple8 _temp1153; _temp1153.f1= _temp1151; _temp1153.f2= _temp1152; _temp1153;});
void* _temp1164; void* _temp1166; void* _temp1168; void* _temp1170; struct Cyc_Absyn_Exp*
_temp1172; void* _temp1174; struct Cyc_Absyn_Exp* _temp1176; void* _temp1178;
_LL1156: _LL1167: _temp1166= _temp1154.f1; if(( unsigned int) _temp1166 >  1u?*((
int*) _temp1166) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1165;} else{ goto _LL1158;}
_LL1165: _temp1164= _temp1154.f2; if(( unsigned int) _temp1164 >  1u?*(( int*)
_temp1164) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1157;} else{ goto _LL1158;}
_LL1158: _LL1171: _temp1170= _temp1154.f1; if(( unsigned int) _temp1170 >  1u?*((
int*) _temp1170) ==  Cyc_Absyn_Upper_b: 0){ _LL1173: _temp1172=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1170)->f1; goto _LL1169;} else{ goto _LL1160;} _LL1169: _temp1168=
_temp1154.f2; if( _temp1168 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1159;}
else{ goto _LL1160;} _LL1160: _LL1179: _temp1178= _temp1154.f1; if( _temp1178 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1175;} else{ goto _LL1162;} _LL1175:
_temp1174= _temp1154.f2; if(( unsigned int) _temp1174 >  1u?*(( int*) _temp1174)
==  Cyc_Absyn_Upper_b: 0){ _LL1177: _temp1176=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1174)->f1; goto _LL1161;} else{ goto _LL1162;} _LL1162: goto _LL1163;
_LL1157: if( _temp1149? ! _temp1150: 0){ if( nv->toplevel){({ void* _temp1180[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1180, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp943, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1181=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1181->hd=( void*) _temp939;
_temp1181->tl= 0; _temp1181;}), 0), 0))->r));} goto _LL1155; _LL1159: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1172, _temp939))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1182[ 3u]; _temp1182[ 2u]=
_temp1172; _temp1182[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1137.elt_typ), 0); _temp1182[ 0u]= _temp939;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1182, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1155; _LL1161: if( nv->toplevel){({ void* _temp1183[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1183, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1184= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1186[ 3u]; _temp1186[ 2u]= _temp1176; _temp1186[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1141.elt_typ), 0); _temp1186[ 0u]= _temp939;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1186, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1150){( void*)(
_temp1184->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1185=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1185->hd=( void*) Cyc_Absyn_copy_exp( _temp1184);
_temp1185->tl= 0; _temp1185;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp943, _temp1184, 0))->r)); goto _LL1155;} _LL1163: goto _LL1155; _LL1155:;}
goto _LL1128;} _LL1132:{ void* _temp1187= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1147.bounds); _LL1189: if( _temp1187 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1190;} else{ goto _LL1191;} _LL1191: goto _LL1192; _LL1190:( void*)( _temp939->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp939->r,
_temp939->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1188; _LL1192: goto _LL1188;
_LL1188:;} goto _LL1128; _LL1134: goto _LL1128; _LL1128:;} goto _LL795;} _LL829:{
void* _temp1193=( void*) _temp944->r; struct Cyc_List_List* _temp1201; struct
_tuple0* _temp1203; struct Cyc_List_List* _temp1205; _LL1195: if(*(( int*)
_temp1193) ==  Cyc_Absyn_Struct_e){ _LL1204: _temp1203=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1193)->f1; goto _LL1202; _LL1202: _temp1201=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1193)->f3; goto _LL1196;} else{ goto _LL1197;} _LL1197: if(*(( int*)
_temp1193) ==  Cyc_Absyn_Tuple_e){ _LL1206: _temp1205=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1193)->f1; goto _LL1198;} else{ goto _LL1199;} _LL1199: goto _LL1200;
_LL1196: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1208;
_temp1208.tag= Cyc_Std_String_pa; _temp1208.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp944->loc);{ void* _temp1207[ 1u]={& _temp1208};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1207, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp944->topt))->v, 1, 0, _temp1201, _temp1203))->r)); goto
_LL1194; _LL1198: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1210;
_temp1210.tag= Cyc_Std_String_pa; _temp1210.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp944->loc);{ void* _temp1209[ 1u]={& _temp1210};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1209, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1205))->r));
goto _LL1194; _LL1200: Cyc_Toc_exp_to_c( nv, _temp944); if( ! Cyc_Absyn_is_lvalue(
_temp944)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp944, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp944, 0))->r));} goto _LL1194; _LL1194:;} goto _LL795; _LL831: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1212; _temp1212.tag= Cyc_Std_String_pa;
_temp1212.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp946->loc);{
void* _temp1211[ 1u]={& _temp1212};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1211, sizeof( void*), 1u));}});}{ void*
_temp1213=( void*) _temp946->r; struct Cyc_List_List* _temp1225; struct Cyc_Absyn_Exp*
_temp1227; struct Cyc_Absyn_Exp* _temp1229; struct Cyc_Absyn_Vardecl* _temp1231;
struct Cyc_List_List* _temp1233; struct _tuple0* _temp1235; struct Cyc_List_List*
_temp1237; _LL1215: if(*(( int*) _temp1213) ==  Cyc_Absyn_Array_e){ _LL1226:
_temp1225=(( struct Cyc_Absyn_Array_e_struct*) _temp1213)->f1; goto _LL1216;}
else{ goto _LL1217;} _LL1217: if(*(( int*) _temp1213) ==  Cyc_Absyn_Comprehension_e){
_LL1232: _temp1231=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1213)->f1;
goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1213)->f2; goto _LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1213)->f3; goto _LL1218;} else{ goto _LL1219;} _LL1219: if(*(( int*)
_temp1213) ==  Cyc_Absyn_Struct_e){ _LL1236: _temp1235=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1213)->f1; goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1213)->f3; goto _LL1220;} else{ goto _LL1221;} _LL1221: if(*(( int*)
_temp1213) ==  Cyc_Absyn_Tuple_e){ _LL1238: _temp1237=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1213)->f1; goto _LL1222;} else{ goto _LL1223;} _LL1223: goto _LL1224;
_LL1216: { struct _tuple0* _temp1239= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1240= Cyc_Absyn_var_exp( _temp1239, 0); struct Cyc_Absyn_Stmt* _temp1241=
Cyc_Toc_init_array( nv, _temp1240, _temp1225, Cyc_Absyn_exp_stmt( _temp1240, 0));
void* old_elt_typ;{ void* _temp1242= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1248; struct Cyc_Absyn_Tqual _temp1250; void* _temp1252; _LL1244: if((
unsigned int) _temp1242 >  4u?*(( int*) _temp1242) ==  Cyc_Absyn_PointerType: 0){
_LL1249: _temp1248=(( struct Cyc_Absyn_PointerType_struct*) _temp1242)->f1;
_LL1253: _temp1252=( void*) _temp1248.elt_typ; goto _LL1251; _LL1251: _temp1250=
_temp1248.tq; goto _LL1245;} else{ goto _LL1246;} _LL1246: goto _LL1247; _LL1245:
old_elt_typ= _temp1252; goto _LL1243; _LL1247: old_elt_typ=({ void* _temp1254[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1254, sizeof( void*), 0u));}); goto
_LL1243; _LL1243:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1255= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1256= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1225), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp948 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1256);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp948); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1256);}{
struct Cyc_Absyn_Exp* _temp1257= Cyc_Absyn_cast_exp( _temp1255, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1239,
_temp1255,( struct Cyc_Absyn_Exp*) _temp1257, _temp1241, 0), 0))->r)); goto
_LL1214;}}} _LL1218: { int is_tagged_ptr= 0;{ void* _temp1258= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1264; struct Cyc_Absyn_Conref* _temp1266;
struct Cyc_Absyn_Tqual _temp1268; void* _temp1270; _LL1260: if(( unsigned int)
_temp1258 >  4u?*(( int*) _temp1258) ==  Cyc_Absyn_PointerType: 0){ _LL1265:
_temp1264=(( struct Cyc_Absyn_PointerType_struct*) _temp1258)->f1; _LL1271:
_temp1270=( void*) _temp1264.elt_typ; goto _LL1269; _LL1269: _temp1268=
_temp1264.tq; goto _LL1267; _LL1267: _temp1266= _temp1264.bounds; goto _LL1261;}
else{ goto _LL1262;} _LL1262: goto _LL1263; _LL1261: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1266) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1259;
_LL1263:({ void* _temp1272[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1272, sizeof( void*), 0u));}); goto
_LL1259; _LL1259:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1227->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1229);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1231, Cyc_Absyn_var_exp( max, 0), _temp1227, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1273= _new_region(); struct _RegionHandle* r=&
_temp1273; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1293=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1293->hd=( void*)({ struct _tuple9* _temp1294=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1294->f1= max; _temp1294->f2=
Cyc_Absyn_uint_t; _temp1294->f3=( struct Cyc_Absyn_Exp*) _temp1229; _temp1294;});
_temp1293->tl= 0; _temp1293;}); struct Cyc_Absyn_Exp* ai; if( _temp948 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1274[ 2u]; _temp1274[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1274[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1274, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp948); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1275[
2u]; _temp1275[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1275[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1275, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1276=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1276->hd=( void*)({ struct _tuple9* _temp1277=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1277->f1= a; _temp1277->f2=
ptr_typ; _temp1277->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1277;}); _temp1276->tl=
decls; _temp1276;}); if( is_tagged_ptr){ struct _tuple0* _temp1278= Cyc_Toc_temp_var();
void* _temp1279= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1280=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1283[ 3u];
_temp1283[ 2u]= _temp1229; _temp1283[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1283[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1283, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1281=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1281->hd=( void*)({
struct _tuple9* _temp1282=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1282->f1= _temp1278; _temp1282->f2= _temp1279; _temp1282->f3=(
struct Cyc_Absyn_Exp*) _temp1280; _temp1282;}); _temp1281->tl= decls; _temp1281;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1278, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1284= decls; for( 0; _temp1284 !=  0;
_temp1284=(( struct Cyc_List_List*) _check_null( _temp1284))->tl){ struct Cyc_Absyn_Exp*
_temp1287; void* _temp1289; struct _tuple0* _temp1291; struct _tuple9 _temp1285=*((
struct _tuple9*)(( struct Cyc_List_List*) _check_null( _temp1284))->hd); _LL1292:
_temp1291= _temp1285.f1; goto _LL1290; _LL1290: _temp1289= _temp1285.f2; goto
_LL1288; _LL1288: _temp1287= _temp1285.f3; goto _LL1286; _LL1286: s= Cyc_Absyn_declare_stmt(
_temp1291, _temp1289, _temp1287, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1214;}}} _LL1220:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp946->topt))->v, 1, _temp948, _temp1233, _temp1235))->r)); goto _LL1214;
_LL1222:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp948,
_temp1237))->r)); goto _LL1214; _LL1224: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp946->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp948 ==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp948); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1295=( void*)
_temp946->r; struct Cyc_Absyn_Exp* _temp1301; void* _temp1303; _LL1297: if(*((
int*) _temp1295) ==  Cyc_Absyn_Cast_e){ _LL1304: _temp1303=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1295)->f1; goto _LL1302; _LL1302: _temp1301=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1295)->f2; goto _LL1298;} else{ goto _LL1299;} _LL1299: goto _LL1300;
_LL1298:{ struct _tuple8 _temp1306=({ struct _tuple8 _temp1305; _temp1305.f1=
Cyc_Tcutil_compress( _temp1303); _temp1305.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1301->topt))->v); _temp1305;}); void*
_temp1312; struct Cyc_Absyn_PtrInfo _temp1314; struct Cyc_Absyn_Conref*
_temp1316; void* _temp1318; struct Cyc_Absyn_PtrInfo _temp1320; struct Cyc_Absyn_Conref*
_temp1322; void* _temp1324; _LL1308: _LL1319: _temp1318= _temp1306.f1; if((
unsigned int) _temp1318 >  4u?*(( int*) _temp1318) ==  Cyc_Absyn_PointerType: 0){
_LL1321: _temp1320=(( struct Cyc_Absyn_PointerType_struct*) _temp1318)->f1;
_LL1325: _temp1324=( void*) _temp1320.elt_typ; goto _LL1323; _LL1323: _temp1322=
_temp1320.bounds; goto _LL1313;} else{ goto _LL1310;} _LL1313: _temp1312=
_temp1306.f2; if(( unsigned int) _temp1312 >  4u?*(( int*) _temp1312) ==  Cyc_Absyn_PointerType:
0){ _LL1315: _temp1314=(( struct Cyc_Absyn_PointerType_struct*) _temp1312)->f1;
_LL1317: _temp1316= _temp1314.bounds; goto _LL1309;} else{ goto _LL1310;}
_LL1310: goto _LL1311; _LL1309:{ struct _tuple8 _temp1327=({ struct _tuple8
_temp1326; _temp1326.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1322);
_temp1326.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1316); _temp1326;});
void* _temp1333; struct Cyc_Absyn_Exp* _temp1335; void* _temp1337; _LL1329:
_LL1338: _temp1337= _temp1327.f1; if( _temp1337 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1334;} else{ goto _LL1331;} _LL1334: _temp1333= _temp1327.f2; if((
unsigned int) _temp1333 >  1u?*(( int*) _temp1333) ==  Cyc_Absyn_Upper_b: 0){
_LL1336: _temp1335=(( struct Cyc_Absyn_Upper_b_struct*) _temp1333)->f1; goto
_LL1330;} else{ goto _LL1331;} _LL1331: goto _LL1332; _LL1330: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1339[ 4u]; _temp1339[ 3u]= _temp1335; _temp1339[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1324, 0); _temp1339[ 1u]= _temp1301; _temp1339[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1339, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1328; _LL1332: goto _LL1328; _LL1328:;} goto _LL1307;
_LL1311: goto _LL1307; _LL1307:;} goto _LL1296; _LL1300: goto _LL1296; _LL1296:;}
if( ! done){ struct _tuple0* _temp1340= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1341= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1340, 0), 0); struct Cyc_Absyn_Exp*
_temp1342= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp946);
_temp1341= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1340, 0), _temp1342, 0), _temp946, 0), _temp1341, 0);{
void* _temp1343= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1340,
_temp1343,( struct Cyc_Absyn_Exp*) mexp, _temp1341, 0), 0))->r));}} goto _LL1214;}}
_LL1214:;} goto _LL795; _LL833: Cyc_Toc_exp_to_c( nv, _temp950); goto _LL795;
_LL835:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1344=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1344[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1345; _temp1345.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1345.f1=( void*) Cyc_Toc_typ_to_c_array( _temp952);
_temp1345;}); _temp1344;}))); goto _LL795; _LL837:( int) _throw(({ void*
_temp1346[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("__gen() in code generator",
sizeof( unsigned char), 26u), _tag_arr( _temp1346, sizeof( void*), 0u));}));
_LL839:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1347=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1347[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1348; _temp1348.tag=
Cyc_Absyn_Offsetof_e; _temp1348.f1=( void*) Cyc_Toc_typ_to_c_array( _temp958);
_temp1348.f2= _temp956; _temp1348;}); _temp1347;}))); goto _LL795; _LL841: {
void* _temp1349= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp960->topt))->v);{ void* _temp1350= _temp1349; struct Cyc_Absyn_PtrInfo
_temp1356; struct Cyc_Absyn_Conref* _temp1358; struct Cyc_Absyn_Tqual _temp1360;
struct Cyc_Absyn_Conref* _temp1362; void* _temp1364; void* _temp1366; _LL1352:
if(( unsigned int) _temp1350 >  4u?*(( int*) _temp1350) ==  Cyc_Absyn_PointerType:
0){ _LL1357: _temp1356=(( struct Cyc_Absyn_PointerType_struct*) _temp1350)->f1;
_LL1367: _temp1366=( void*) _temp1356.elt_typ; goto _LL1365; _LL1365: _temp1364=(
void*) _temp1356.rgn_typ; goto _LL1363; _LL1363: _temp1362= _temp1356.nullable;
goto _LL1361; _LL1361: _temp1360= _temp1356.tq; goto _LL1359; _LL1359: _temp1358=
_temp1356.bounds; goto _LL1353;} else{ goto _LL1354;} _LL1354: goto _LL1355;
_LL1353:{ void* _temp1368= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1358);
_LL1370: if(( unsigned int) _temp1368 >  1u?*(( int*) _temp1368) ==  Cyc_Absyn_Upper_b:
0){ goto _LL1371;} else{ goto _LL1372;} _LL1372: if( _temp1368 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1373;} else{ goto _LL1369;} _LL1371: Cyc_Toc_exp_to_c( nv, _temp960);
if( Cyc_Toc_is_nullable( _temp1349)){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp960->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1374=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1374->hd=( void*) _temp960; _temp1374->tl= 0;
_temp1374;}), 0), 0), 0))->r));} goto _LL1369; _LL1373: { struct Cyc_Absyn_Exp*
_temp1375= Cyc_Absyn_uint_exp( 0, 0); _temp1375->topt=({ struct Cyc_Core_Opt*
_temp1376=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1376->v=( void*) Cyc_Absyn_uint_t; _temp1376;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp960, _temp1375, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1369;} _LL1369:;} goto _LL1351; _LL1355:({ void* _temp1377[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1377, sizeof( void*), 0u));}); goto _LL1351; _LL1351:;} goto _LL795;}
_LL843: Cyc_Toc_exp_to_c( nv, _temp964); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL795; _LL845: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp968->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp968);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1380; struct Cyc_Absyn_Tqual _temp1382; struct Cyc_Absyn_Conref* _temp1384;
void* _temp1386; void* _temp1388; struct Cyc_Absyn_PtrInfo _temp1378= Cyc_Toc_get_ptr_type(
e1typ); _LL1389: _temp1388=( void*) _temp1378.elt_typ; goto _LL1387; _LL1387:
_temp1386=( void*) _temp1378.rgn_typ; goto _LL1385; _LL1385: _temp1384=
_temp1378.nullable; goto _LL1383; _LL1383: _temp1382= _temp1378.tq; goto _LL1381;
_LL1381: _temp1380= _temp1378.bounds; goto _LL1379; _LL1379:{ void* _temp1390=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1380); struct Cyc_Absyn_Exp*
_temp1396; _LL1392: if(( unsigned int) _temp1390 >  1u?*(( int*) _temp1390) == 
Cyc_Absyn_Upper_b: 0){ _LL1397: _temp1396=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1390)->f1; goto _LL1393;} else{ goto _LL1394;} _LL1394: if( _temp1390 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1395;} else{ goto _LL1391;} _LL1393: if(
Cyc_Evexp_eval_const_uint_exp( _temp1396) <  1){({ void* _temp1398[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1398, sizeof( void*), 0u));});}{ int possibly_null=(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1384); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp968->topt))->v)){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp968->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1399=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1399->hd=( void*) _temp968;
_temp1399->tl= 0; _temp1399;}), 0), 0), _temp966, 0))->r));} goto _LL1391;}
_LL1395: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1388);( void*)( _temp968->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1382), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1400[ 3u];
_temp1400[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1400[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1400[ 0u]= Cyc_Absyn_copy_exp( _temp968);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1400, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1391;} _LL1391:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL795;}} _LL847: { void* _temp1401= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp972->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp972); Cyc_Toc_exp_to_c( nv, _temp970);{ void* _temp1402= _temp1401; struct
Cyc_List_List* _temp1410; struct Cyc_Absyn_PtrInfo _temp1412; struct Cyc_Absyn_Conref*
_temp1414; struct Cyc_Absyn_Tqual _temp1416; struct Cyc_Absyn_Conref* _temp1418;
void* _temp1420; void* _temp1422; _LL1404: if(( unsigned int) _temp1402 >  4u?*((
int*) _temp1402) ==  Cyc_Absyn_TupleType: 0){ _LL1411: _temp1410=(( struct Cyc_Absyn_TupleType_struct*)
_temp1402)->f1; goto _LL1405;} else{ goto _LL1406;} _LL1406: if(( unsigned int)
_temp1402 >  4u?*(( int*) _temp1402) ==  Cyc_Absyn_PointerType: 0){ _LL1413:
_temp1412=(( struct Cyc_Absyn_PointerType_struct*) _temp1402)->f1; _LL1423:
_temp1422=( void*) _temp1412.elt_typ; goto _LL1421; _LL1421: _temp1420=( void*)
_temp1412.rgn_typ; goto _LL1419; _LL1419: _temp1418= _temp1412.nullable; goto
_LL1417; _LL1417: _temp1416= _temp1412.tq; goto _LL1415; _LL1415: _temp1414=
_temp1412.bounds; goto _LL1407;} else{ goto _LL1408;} _LL1408: goto _LL1409;
_LL1405: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp970) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp972, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1403;} _LL1407:{ void* _temp1424= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1414); struct Cyc_Absyn_Exp* _temp1430; _LL1426: if((
unsigned int) _temp1424 >  1u?*(( int*) _temp1424) ==  Cyc_Absyn_Upper_b: 0){
_LL1431: _temp1430=(( struct Cyc_Absyn_Upper_b_struct*) _temp1424)->f1; goto
_LL1427;} else{ goto _LL1428;} _LL1428: if( _temp1424 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1429;} else{ goto _LL1425;} _LL1427: _temp1430= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1430), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1418); void* ta1= Cyc_Toc_typ_to_c( _temp1422);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1416); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1432[ 4u];
_temp1432[ 3u]= _temp970; _temp1432[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1432[ 1u]= _temp1430; _temp1432[ 0u]= _temp972;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1432, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp970->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1433[ 2u];
_temp1433[ 1u]= Cyc_Absyn_copy_exp( _temp970); _temp1433[ 0u]= _temp1430;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1433, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1425;}
_LL1429: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1422);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1416), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1434[ 3u]; _temp1434[ 2u]= _temp970; _temp1434[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1434[ 0u]= _temp972;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1434, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1425;} _LL1425:;} goto _LL1403; _LL1409:({
void* _temp1435[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1435, sizeof( void*), 0u));}); goto
_LL1403; _LL1403:;} goto _LL795;} _LL849: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp974))->r));} else{ struct
Cyc_List_List* _temp1436=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp974); struct _tagged_arr* _temp1437= Cyc_Toc_add_tuple_type( _temp1436);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp974 !=  0;( _temp974=((
struct Cyc_List_List*) _check_null( _temp974))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp974))->hd);
dles=({ struct Cyc_List_List* _temp1438=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1438->hd=( void*)({ struct _tuple4*
_temp1439=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1439->f1=
0; _temp1439->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp974))->hd; _temp1439;}); _temp1438->tl= dles; _temp1438;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL795; _LL851:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp976, 0))->r));{ struct Cyc_List_List* _temp1440= _temp976; for( 0;
_temp1440 !=  0; _temp1440=(( struct Cyc_List_List*) _check_null( _temp1440))->tl){
struct _tuple4 _temp1443; struct Cyc_Absyn_Exp* _temp1444; struct _tuple4*
_temp1441=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1440))->hd;
_temp1443=* _temp1441; _LL1445: _temp1444= _temp1443.f2; goto _LL1442; _LL1442:
Cyc_Toc_exp_to_c( nv, _temp1444);}} goto _LL795; _LL853: { unsigned int
_temp1446= Cyc_Evexp_eval_const_uint_exp( _temp980); Cyc_Toc_exp_to_c( nv,
_temp978);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp978)){
unsigned int i= 0; for( 0; i <  _temp1446; i ++){ es=({ struct Cyc_List_List*
_temp1447=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1447->hd=( void*)({ struct _tuple4* _temp1448=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1448->f1= 0; _temp1448->f2= _temp978; _temp1448;});
_temp1447->tl= es; _temp1447;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL795;}} _LL855: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp986, _temp990))->r));}
else{ if( _temp984 ==  0){({ void* _temp1449[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1449, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp984); struct _RegionHandle _temp1450= _new_region(); struct _RegionHandle*
rgn=& _temp1450; _push_region( rgn);{ struct Cyc_List_List* _temp1451=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp986,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1452= 0;{ struct Cyc_List_List*
_temp1453=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1453 !=  0; _temp1453=(( struct Cyc_List_List*) _check_null(
_temp1453))->tl){ struct Cyc_List_List* _temp1454= _temp1451; for( 0; _temp1454
!=  0; _temp1454=(( struct Cyc_List_List*) _check_null( _temp1454))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1454))->hd)).f1 == (
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1453))->hd){
struct _tuple10 _temp1457; struct Cyc_Absyn_Exp* _temp1458; struct Cyc_Absyn_Structfield*
_temp1460; struct _tuple10* _temp1455=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1454))->hd; _temp1457=* _temp1455; _LL1461: _temp1460=
_temp1457.f1; goto _LL1459; _LL1459: _temp1458= _temp1457.f2; goto _LL1456;
_LL1456: { void* _temp1462=( void*) _temp1460->type; Cyc_Toc_exp_to_c( nv,
_temp1458); if( Cyc_Toc_is_void_star( _temp1462)){( void*)( _temp1458->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1458->r, 0), 0))->r));} _temp1452=({ struct Cyc_List_List* _temp1463=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1463->hd=(
void*)({ struct _tuple4* _temp1464=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1464->f1= 0; _temp1464->f2= _temp1458; _temp1464;}); _temp1463->tl=
_temp1452; _temp1463;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1452), 0))->r));}; _pop_region( rgn);}} goto _LL795; _LL857: { struct Cyc_List_List*
fs;{ void* _temp1465= Cyc_Tcutil_compress( _temp994); struct Cyc_List_List*
_temp1471; _LL1467: if(( unsigned int) _temp1465 >  4u?*(( int*) _temp1465) == 
Cyc_Absyn_AnonStructType: 0){ _LL1472: _temp1471=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1465)->f1; goto _LL1468;} else{ goto _LL1469;} _LL1469: goto _LL1470;
_LL1468: fs= _temp1471; goto _LL1466; _LL1470: fs=({ struct Cyc_Std_String_pa_struct
_temp1474; _temp1474.tag= Cyc_Std_String_pa; _temp1474.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp994);{ void* _temp1473[ 1u]={& _temp1474};(( struct
Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s", sizeof( unsigned char), 24u), _tag_arr(
_temp1473, sizeof( void*), 1u));}}); goto _LL1466; _LL1466:;}{ struct
_RegionHandle _temp1475= _new_region(); struct _RegionHandle* rgn=& _temp1475;
_push_region( rgn);{ struct Cyc_List_List* _temp1476=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp992, fs); for( 0; _temp1476 !=  0; _temp1476=(( struct Cyc_List_List*)
_check_null( _temp1476))->tl){ struct _tuple10 _temp1479; struct Cyc_Absyn_Exp*
_temp1480; struct Cyc_Absyn_Structfield* _temp1482; struct _tuple10* _temp1477=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1476))->hd;
_temp1479=* _temp1477; _LL1483: _temp1482= _temp1479.f1; goto _LL1481; _LL1481:
_temp1480= _temp1479.f2; goto _LL1478; _LL1478: { void* _temp1484=( void*)
_temp1482->type; Cyc_Toc_exp_to_c( nv, _temp1480); if( Cyc_Toc_is_void_star(
_temp1484)){( void*)( _temp1480->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1480->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp992, 0))->r));}; _pop_region( rgn);}
goto _LL795;} _LL859: { struct _tuple0* qv= _temp996->name; if( _temp998->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1485= _temp998->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp998->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1485))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1485))->hd)->typs ==  0){ tag_count ++;}
_temp1485=(( struct Cyc_List_List*) _check_null( _temp1485))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL795;} _LL861: { struct _tuple0* _temp1486= _temp1002->name; struct Cyc_List_List*
_temp1487= _temp1002->typs; struct _tuple0* _temp1488= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1489= Cyc_Absyn_var_exp( _temp1488, 0); void*
_temp1490= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1486, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1004->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1486, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1491= _temp1004->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1004->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1486,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1491))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1491))->hd)->typs !=  0){ tag_count ++;}
_temp1491=(( struct Cyc_List_List*) _check_null( _temp1491))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1006
!=  0;( _temp1006=(( struct Cyc_List_List*) _check_null( _temp1006))->tl,
_temp1487=(( struct Cyc_List_List*) _check_null( _temp1487))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1006))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1487))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1492=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1492->hd=( void*)({ struct _tuple4* _temp1493=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1493->f1= 0; _temp1493->f2= cur_e; _temp1493;});
_temp1492->tl= dles; _temp1492;});} dles=({ struct Cyc_List_List* _temp1494=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1494->hd=(
void*)({ struct _tuple4* _temp1495=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1495->f1= 0; _temp1495->f2= tag_exp; _temp1495;}); _temp1494->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1494;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1496=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1496->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1486,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1496;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1497=({ struct Cyc_List_List* _temp1503=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1503->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1489, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1486, 0), 0); _temp1503->tl= 0; _temp1503;});{ int i=
1; for( 0; _temp1006 !=  0;((( _temp1006=(( struct Cyc_List_List*) _check_null(
_temp1006))->tl, i ++)), _temp1487=(( struct Cyc_List_List*) _check_null(
_temp1487))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1006))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1487))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp1498= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1489, Cyc_Absyn_fieldname( i), 0), cur_e, 0);
_temp1497=({ struct Cyc_List_List* _temp1499=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1499->hd=( void*) _temp1498; _temp1499->tl=
_temp1497; _temp1499;});}}}{ struct Cyc_Absyn_Stmt* _temp1500= Cyc_Absyn_exp_stmt(
_temp1489, 0); struct Cyc_Absyn_Stmt* _temp1501= Cyc_Absyn_seq_stmts((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List*
_temp1502=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1502->hd=( void*) _temp1500; _temp1502->tl= _temp1497; _temp1502;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1488, _temp1490, 0, _temp1501, 0), 0))->r));}} goto _LL795;} _LL863: goto
_LL795; _LL865: { void* t_c= Cyc_Toc_typ_to_c( _temp1008); if( _temp1010 !=  0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1010); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1008, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL795;} _LL867: Cyc_Toc_stmt_to_c(
nv, _temp1012); goto _LL795; _LL869:({ void* _temp1504[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem",
sizeof( unsigned char), 14u), _tag_arr( _temp1504, sizeof( void*), 0u));}); goto
_LL795; _LL871:({ void* _temp1505[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u), _tag_arr( _temp1505, sizeof( void*), 0u));}); goto _LL795;
_LL873:({ void* _temp1506[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u),
_tag_arr( _temp1506, sizeof( void*), 0u));}); goto _LL795; _LL875:({ void*
_temp1507[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("fill", sizeof( unsigned char), 5u), _tag_arr( _temp1507, sizeof( void*),
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
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1508=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1544; struct Cyc_Absyn_Vardecl _temp1546; struct _tuple0* _temp1547; struct
Cyc_Absyn_Vardecl* _temp1549; int _temp1551; void* _temp1553; unsigned char
_temp1555; struct _tagged_arr _temp1557; struct Cyc_Absyn_Enumfield* _temp1559;
struct Cyc_Absyn_Enumdecl* _temp1561; struct Cyc_List_List* _temp1563; struct
Cyc_List_List* _temp1565; struct Cyc_Absyn_Tunionfield* _temp1567; struct Cyc_Absyn_Tuniondecl*
_temp1569; struct Cyc_Absyn_Pat* _temp1572; struct Cyc_Absyn_Pat _temp1574; void*
_temp1575; struct Cyc_List_List* _temp1577; struct Cyc_List_List* _temp1579;
struct Cyc_Absyn_Tunionfield* _temp1581; struct Cyc_Absyn_Tuniondecl* _temp1583;
struct Cyc_List_List* _temp1585; struct Cyc_List_List* _temp1587; struct Cyc_List_List*
_temp1589; struct Cyc_List_List* _temp1591; struct Cyc_Core_Opt* _temp1593;
struct Cyc_Absyn_Structdecl* _temp1595; struct Cyc_Absyn_Pat* _temp1597; _LL1510:
if(( unsigned int) _temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_Var_p: 0){
_LL1545: _temp1544=(( struct Cyc_Absyn_Var_p_struct*) _temp1508)->f1; _temp1546=*
_temp1544; _LL1548: _temp1547= _temp1546.name; goto _LL1511;} else{ goto _LL1512;}
_LL1512: if( _temp1508 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1513;} else{ goto
_LL1514;} _LL1514: if(( unsigned int) _temp1508 >  2u?*(( int*) _temp1508) == 
Cyc_Absyn_Reference_p: 0){ _LL1550: _temp1549=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1508)->f1; goto _LL1515;} else{ goto _LL1516;} _LL1516: if( _temp1508 == (
void*) Cyc_Absyn_Null_p){ goto _LL1517;} else{ goto _LL1518;} _LL1518: if((
unsigned int) _temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_Int_p: 0){
_LL1554: _temp1553=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1508)->f1;
goto _LL1552; _LL1552: _temp1551=(( struct Cyc_Absyn_Int_p_struct*) _temp1508)->f2;
goto _LL1519;} else{ goto _LL1520;} _LL1520: if(( unsigned int) _temp1508 >  2u?*((
int*) _temp1508) ==  Cyc_Absyn_Char_p: 0){ _LL1556: _temp1555=(( struct Cyc_Absyn_Char_p_struct*)
_temp1508)->f1; goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( unsigned int)
_temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_Float_p: 0){ _LL1558:
_temp1557=(( struct Cyc_Absyn_Float_p_struct*) _temp1508)->f1; goto _LL1523;}
else{ goto _LL1524;} _LL1524: if(( unsigned int) _temp1508 >  2u?*(( int*)
_temp1508) ==  Cyc_Absyn_Enum_p: 0){ _LL1562: _temp1561=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1508)->f1; goto _LL1560; _LL1560: _temp1559=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1508)->f2; goto _LL1525;} else{ goto _LL1526;} _LL1526: if(( unsigned int)
_temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_Tunion_p: 0){ _LL1570:
_temp1569=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1508)->f1; goto _LL1568;
_LL1568: _temp1567=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1508)->f2; goto
_LL1566; _LL1566: _temp1565=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1508)->f3;
goto _LL1564; _LL1564: _temp1563=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1508)->f4;
if( _temp1563 ==  0){ goto _LL1527;} else{ goto _LL1528;}} else{ goto _LL1528;}
_LL1528: if(( unsigned int) _temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_Pointer_p:
0){ _LL1573: _temp1572=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1508)->f1;
_temp1574=* _temp1572; _LL1576: _temp1575=( void*) _temp1574.r; if((
unsigned int) _temp1575 >  2u?*(( int*) _temp1575) ==  Cyc_Absyn_Tunion_p: 0){
_LL1584: _temp1583=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1575)->f1; goto
_LL1582; _LL1582: _temp1581=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1575)->f2;
goto _LL1580; _LL1580: _temp1579=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1575)->f3;
goto _LL1578; _LL1578: _temp1577=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1575)->f4;
goto _LL1571;} else{ goto _LL1530;}} else{ goto _LL1530;} _LL1571: if( _temp1577
!=  0){ goto _LL1529;} else{ goto _LL1530;} _LL1530: if(( unsigned int)
_temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_Tunion_p: 0){ _LL1586:
_temp1585=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1508)->f4; goto _LL1531;}
else{ goto _LL1532;} _LL1532: if(( unsigned int) _temp1508 >  2u?*(( int*)
_temp1508) ==  Cyc_Absyn_Tuple_p: 0){ _LL1588: _temp1587=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1508)->f1; goto _LL1533;} else{ goto _LL1534;} _LL1534: if(( unsigned int)
_temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_Struct_p: 0){ _LL1596:
_temp1595=(( struct Cyc_Absyn_Struct_p_struct*) _temp1508)->f1; goto _LL1594;
_LL1594: _temp1593=(( struct Cyc_Absyn_Struct_p_struct*) _temp1508)->f2; goto
_LL1592; _LL1592: _temp1591=(( struct Cyc_Absyn_Struct_p_struct*) _temp1508)->f3;
goto _LL1590; _LL1590: _temp1589=(( struct Cyc_Absyn_Struct_p_struct*) _temp1508)->f4;
goto _LL1535;} else{ goto _LL1536;} _LL1536: if(( unsigned int) _temp1508 >  2u?*((
int*) _temp1508) ==  Cyc_Absyn_Pointer_p: 0){ _LL1598: _temp1597=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1508)->f1; goto _LL1537;} else{ goto _LL1538;} _LL1538: if(( unsigned int)
_temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1539;}
else{ goto _LL1540;} _LL1540: if(( unsigned int) _temp1508 >  2u?*(( int*)
_temp1508) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1541;} else{ goto _LL1542;}
_LL1542: if(( unsigned int) _temp1508 >  2u?*(( int*) _temp1508) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1543;} else{ goto _LL1509;} _LL1511: nv= Cyc_Toc_add_varmap( nv,
_temp1547, r); goto _LL1513; _LL1513: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1509; _LL1515: { struct _tuple0* _temp1599= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1600=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1600->hd=( void*)({ struct _tuple12*
_temp1601=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1601->f1= _temp1599; _temp1601->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1601;}); _temp1600->tl= decls; _temp1600;}); nv= Cyc_Toc_add_varmap(
nv, _temp1549->name, Cyc_Absyn_var_exp( _temp1599, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1599, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1509;} _LL1517: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1509; _LL1519:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1553, _temp1551, 0),
succ_lab, fail_lab); goto _LL1509; _LL1521: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1555, 0), succ_lab, fail_lab); goto _LL1509; _LL1523: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1557, 0), succ_lab, fail_lab); goto _LL1509;
_LL1525: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1602=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1602[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1603; _temp1603.tag= Cyc_Absyn_Enum_e;
_temp1603.f1= _temp1559->name; _temp1603.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1561; _temp1603.f3=( struct Cyc_Absyn_Enumfield*) _temp1559; _temp1603;});
_temp1602;}), 0), succ_lab, fail_lab); goto _LL1509; _LL1527: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1567->name, 0); if( ! _temp1569->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1509;} _LL1529: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1577);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1581->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1604=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1581->typs); struct Cyc_List_List*
_temp1605=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1577); for( 0; _temp1605 !=  0;((( _temp1605=((
struct Cyc_List_List*) _check_null( _temp1605))->tl, _temp1604=(( struct Cyc_List_List*)
_check_null( _temp1604))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1606=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1605))->hd;
if(( void*) _temp1606->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1607=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1604))->hd)).f2;
struct _tuple0* _temp1608= Cyc_Toc_temp_var(); void* _temp1609=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1606->topt))->v; void* _temp1610= Cyc_Toc_typ_to_c(
_temp1609); struct _tagged_arr* _temp1611= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1612= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1607))){ _temp1612= Cyc_Absyn_cast_exp(
_temp1610, _temp1612, 0);} decls=({ struct Cyc_List_List* _temp1613=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1613->hd=( void*)({
struct _tuple12* _temp1614=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1614->f1= _temp1608; _temp1614->f2= _temp1610; _temp1614;});
_temp1613->tl= decls; _temp1613;});{ struct _tuple11 _temp1615= Cyc_Toc_xlate_pat(
nv, rgn, _temp1609, Cyc_Absyn_var_exp( _temp1608, 0), _temp1612, _temp1606,
succ_lab, fail_lab, decls); nv= _temp1615.f1; decls= _temp1615.f2;{ struct Cyc_Absyn_Stmt*
_temp1616= _temp1615.f3; struct Cyc_Absyn_Stmt* _temp1617= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1608, 0), _temp1612, 0); struct Cyc_Absyn_Stmt*
_temp1618= Cyc_Absyn_seq_stmt( _temp1617, _temp1616, 0); ss=({ struct Cyc_List_List*
_temp1619=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1619->hd=( void*) Cyc_Absyn_label_stmt( _temp1611, _temp1618, 0); _temp1619->tl=
ss; _temp1619;}); succ_lab= _temp1611;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1620=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1620->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1620->tl= 0;
_temp1620;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1583->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1581->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1621=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1583->fields))->v; for( 0; _temp1621 !=  0; _temp1621=((
struct Cyc_List_List*) _check_null( _temp1621))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1622=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1621))->hd; if( _temp1622->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1581->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1509;}} _LL1531: _temp1587= _temp1585; goto _LL1533; _LL1533: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1587);{ struct Cyc_List_List* _temp1623=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1587); for(
0; _temp1623 !=  0;( _temp1623=(( struct Cyc_List_List*) _check_null( _temp1623))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1624=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1623))->hd; if(( void*) _temp1624->r == ( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1625= Cyc_Toc_temp_var(); void* _temp1626=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1624->topt))->v; struct
_tagged_arr* _temp1627= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1628=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1628->hd=( void*)({ struct _tuple12* _temp1629=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1629->f1= _temp1625;
_temp1629->f2= Cyc_Toc_typ_to_c( _temp1626); _temp1629;}); _temp1628->tl= decls;
_temp1628;});{ struct _tuple11 _temp1630= Cyc_Toc_xlate_pat( nv, rgn, _temp1626,
Cyc_Absyn_var_exp( _temp1625, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1624, succ_lab, fail_lab, decls); nv= _temp1630.f1; decls=
_temp1630.f2;{ struct Cyc_Absyn_Stmt* _temp1631= _temp1630.f3; struct Cyc_Absyn_Stmt*
_temp1632= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1625, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp1631, 0); ss=({ struct Cyc_List_List* _temp1633=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1633->hd=( void*) Cyc_Absyn_label_stmt(
_temp1627, _temp1632, 0); _temp1633->tl= ss; _temp1633;}); succ_lab= _temp1627;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1634=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1634->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1634->tl= 0; _temp1634;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1509;} _LL1535: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1635=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1589); for( 0; _temp1635 !=  0; _temp1635=((
struct Cyc_List_List*) _check_null( _temp1635))->tl){ struct _tuple13* _temp1636=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1635))->hd; struct
Cyc_Absyn_Pat* _temp1637=(* _temp1636).f2; if(( void*) _temp1637->r == ( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1638=( void*)((
struct Cyc_List_List*) _check_null((* _temp1636).f1))->hd; struct _tagged_arr*
_temp1644; _LL1640: if(*(( int*) _temp1638) ==  Cyc_Absyn_FieldName){ _LL1645:
_temp1644=(( struct Cyc_Absyn_FieldName_struct*) _temp1638)->f1; goto _LL1641;}
else{ goto _LL1642;} _LL1642: goto _LL1643; _LL1641: f= _temp1644; goto _LL1639;
_LL1643:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1639:;}{ struct _tuple0*
_temp1646= Cyc_Toc_temp_var(); void* _temp1647=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1637->topt))->v; void* _temp1648= Cyc_Toc_typ_to_c( _temp1647);
struct _tagged_arr* _temp1649= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1650=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1650->hd=( void*)({ struct _tuple12* _temp1651=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1651->f1= _temp1646;
_temp1651->f2= _temp1648; _temp1651;}); _temp1650->tl= decls; _temp1650;});{
struct _tuple11 _temp1652= Cyc_Toc_xlate_pat( nv, rgn, _temp1647, Cyc_Absyn_var_exp(
_temp1646, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1637, succ_lab,
fail_lab, decls); nv= _temp1652.f1; decls= _temp1652.f2;{ struct Cyc_Absyn_Exp*
_temp1653= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1595->fields))->v, f)))->type)){
_temp1653= Cyc_Absyn_cast_exp( _temp1648, _temp1653, 0);}{ struct Cyc_Absyn_Stmt*
_temp1654= _temp1652.f3; struct Cyc_Absyn_Stmt* _temp1655= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1646, 0), _temp1653, 0),
_temp1654, 0); ss=({ struct Cyc_List_List* _temp1656=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1656->hd=( void*) Cyc_Absyn_label_stmt(
_temp1649, _temp1655, 0); _temp1656->tl= ss; _temp1656;}); succ_lab= _temp1649;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1657=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1657->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1657->tl= 0; _temp1657;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1509;} _LL1537: { struct _tuple0* _temp1658= Cyc_Toc_temp_var(); void*
_temp1659=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1597->topt))->v;
decls=({ struct Cyc_List_List* _temp1660=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1660->hd=( void*)({ struct _tuple12*
_temp1661=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1661->f1= _temp1658; _temp1661->f2= Cyc_Toc_typ_to_c( _temp1659); _temp1661;});
_temp1660->tl= decls; _temp1660;});{ struct _tuple11 _temp1662= Cyc_Toc_xlate_pat(
nv, rgn, _temp1659, Cyc_Absyn_var_exp( _temp1658, 0), Cyc_Absyn_deref_exp( path,
0), _temp1597, succ_lab, fail_lab, decls); nv= _temp1662.f1; decls= _temp1662.f2;{
struct Cyc_Absyn_Stmt* _temp1663= _temp1662.f3; struct Cyc_Absyn_Stmt* _temp1664=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1658, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1663, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1664, 0);} else{ s= _temp1664;} goto _LL1509;}}} _LL1539: s=({
void* _temp1665[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1665, sizeof( void*), 0u));}); goto _LL1509;
_LL1541: s=({ void* _temp1666[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1666, sizeof( void*), 0u));}); goto
_LL1509; _LL1543: s=({ void* _temp1667[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1667, sizeof( void*), 0u));}); goto
_LL1509; _LL1509:;} return({ struct _tuple11 _temp1668; _temp1668.f1= nv;
_temp1668.f2= decls; _temp1668.f3= s; _temp1668;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1669=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1669->f1= Cyc_Toc_fresh_label(); _temp1669->f2=
Cyc_Toc_fresh_label(); _temp1669->f3= sc; _temp1669;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1670=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1671= Cyc_Tcutil_compress( _temp1670); _LL1673: if(( unsigned int)
_temp1671 >  4u?*(( int*) _temp1671) ==  Cyc_Absyn_IntType: 0){ goto _LL1674;}
else{ goto _LL1675;} _LL1675: if(( unsigned int) _temp1671 >  4u?*(( int*)
_temp1671) ==  Cyc_Absyn_EnumType: 0){ goto _LL1676;} else{ goto _LL1677;}
_LL1677: goto _LL1678; _LL1674: goto _LL1676; _LL1676: leave_as_switch= 1; goto
_LL1672; _LL1678: leave_as_switch= 0; goto _LL1672; _LL1672:;}{ struct Cyc_List_List*
_temp1679= scs; for( 0; _temp1679 !=  0; _temp1679=(( struct Cyc_List_List*)
_check_null( _temp1679))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1679))->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1679))->hd)->where_clause !=  0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1680= scs; for( 0; _temp1680
!=  0; _temp1680=(( struct Cyc_List_List*) _check_null( _temp1680))->tl){ struct
Cyc_Absyn_Stmt* _temp1681=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1680))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1680))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1681, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1681);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1682= _new_region(); struct _RegionHandle* rgn=&
_temp1682; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1683= lscs; for( 0;
_temp1683 !=  0; _temp1683=(( struct Cyc_List_List*) _check_null( _temp1683))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1683))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1683))->tl ==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1683))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1683))->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11
_temp1684= Cyc_Toc_xlate_pat( nv, rgn, _temp1670, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1685=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1685->hd=( void*)
_temp1684.f1; _temp1685->tl= nvs; _temp1685;}); decls= _temp1684.f2; test_stmts=({
struct Cyc_List_List* _temp1686=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1686->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1683))->hd)).f1,
_temp1684.f3, 0); _temp1686->tl= test_stmts; _temp1686;});} else{ struct Cyc_Absyn_Exp*
_temp1687=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1688= Cyc_Toc_fresh_label(); struct _tuple11 _temp1689= Cyc_Toc_xlate_pat(
nv, rgn, _temp1670, r, path, sc->pattern, _temp1688, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1689.f1, _temp1687); nvs=({ struct Cyc_List_List* _temp1690=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1690->hd=( void*)
_temp1689.f1; _temp1690->tl= nvs; _temp1690;}); decls= _temp1689.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1687, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1688, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1691=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1691->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1683))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1689.f3, s3, 0), 0); _temp1691->tl= test_stmts;
_temp1691;});}}}}{ struct Cyc_Absyn_Stmt* _temp1692= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs !=  0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=  0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1693=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1693, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1694=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1694, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1692, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls !=  0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1697; void* _temp1698; struct
_tuple0* _temp1700; struct _tuple12* _temp1695=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1697=* _temp1695; _LL1701: _temp1700= _temp1697.f1;
goto _LL1699; _LL1699: _temp1698= _temp1697.f2; goto _LL1696; _LL1696: res= Cyc_Absyn_declare_stmt(
_temp1700, _temp1698, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1702=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1702[ 0]=({ struct Cyc_List_List
_temp1703; _temp1703.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1703.tl= 0; _temp1703;}); _temp1702;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1704=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1746; struct Cyc_Absyn_Stmt* _temp1748; struct Cyc_Absyn_Stmt* _temp1750;
struct Cyc_Absyn_Exp* _temp1752; struct Cyc_Absyn_Stmt* _temp1754; struct Cyc_Absyn_Stmt*
_temp1756; struct Cyc_Absyn_Exp* _temp1758; struct Cyc_Absyn_Stmt* _temp1760;
struct _tuple2 _temp1762; struct Cyc_Absyn_Exp* _temp1764; struct Cyc_Absyn_Stmt*
_temp1766; struct Cyc_Absyn_Stmt* _temp1768; struct Cyc_Absyn_Stmt* _temp1770;
struct Cyc_Absyn_Stmt* _temp1772; struct _tuple2 _temp1774; struct Cyc_Absyn_Exp*
_temp1776; struct _tuple2 _temp1778; struct Cyc_Absyn_Exp* _temp1780; struct Cyc_Absyn_Exp*
_temp1782; struct Cyc_List_List* _temp1784; struct Cyc_Absyn_Exp* _temp1786;
struct Cyc_Absyn_Switch_clause** _temp1788; struct Cyc_List_List* _temp1790;
struct Cyc_Absyn_Stmt* _temp1792; struct Cyc_Absyn_Decl* _temp1794; struct Cyc_Absyn_Stmt*
_temp1796; struct _tagged_arr* _temp1798; struct _tuple2 _temp1800; struct Cyc_Absyn_Exp*
_temp1802; struct Cyc_Absyn_Stmt* _temp1804; struct Cyc_List_List* _temp1806;
struct Cyc_Absyn_Stmt* _temp1808; struct Cyc_Absyn_Stmt* _temp1810; struct Cyc_Absyn_Vardecl*
_temp1812; struct Cyc_Absyn_Tvar* _temp1814; _LL1706: if( _temp1704 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1707;} else{ goto _LL1708;} _LL1708: if((
unsigned int) _temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Exp_s: 0){
_LL1747: _temp1746=(( struct Cyc_Absyn_Exp_s_struct*) _temp1704)->f1; goto
_LL1709;} else{ goto _LL1710;} _LL1710: if(( unsigned int) _temp1704 >  1u?*((
int*) _temp1704) ==  Cyc_Absyn_Seq_s: 0){ _LL1751: _temp1750=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1704)->f1; goto _LL1749; _LL1749: _temp1748=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1704)->f2; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Return_s: 0){ _LL1753:
_temp1752=(( struct Cyc_Absyn_Return_s_struct*) _temp1704)->f1; goto _LL1713;}
else{ goto _LL1714;} _LL1714: if(( unsigned int) _temp1704 >  1u?*(( int*)
_temp1704) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1759: _temp1758=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1704)->f1; goto _LL1757; _LL1757: _temp1756=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1704)->f2; goto _LL1755; _LL1755: _temp1754=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1704)->f3; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_While_s: 0){ _LL1763:
_temp1762=(( struct Cyc_Absyn_While_s_struct*) _temp1704)->f1; _LL1765:
_temp1764= _temp1762.f1; goto _LL1761; _LL1761: _temp1760=(( struct Cyc_Absyn_While_s_struct*)
_temp1704)->f2; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Break_s: 0){ _LL1767:
_temp1766=(( struct Cyc_Absyn_Break_s_struct*) _temp1704)->f1; goto _LL1719;}
else{ goto _LL1720;} _LL1720: if(( unsigned int) _temp1704 >  1u?*(( int*)
_temp1704) ==  Cyc_Absyn_Continue_s: 0){ _LL1769: _temp1768=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1704)->f1; goto _LL1721;} else{ goto _LL1722;} _LL1722: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Goto_s: 0){ _LL1771:
_temp1770=(( struct Cyc_Absyn_Goto_s_struct*) _temp1704)->f2; goto _LL1723;}
else{ goto _LL1724;} _LL1724: if(( unsigned int) _temp1704 >  1u?*(( int*)
_temp1704) ==  Cyc_Absyn_For_s: 0){ _LL1783: _temp1782=(( struct Cyc_Absyn_For_s_struct*)
_temp1704)->f1; goto _LL1779; _LL1779: _temp1778=(( struct Cyc_Absyn_For_s_struct*)
_temp1704)->f2; _LL1781: _temp1780= _temp1778.f1; goto _LL1775; _LL1775:
_temp1774=(( struct Cyc_Absyn_For_s_struct*) _temp1704)->f3; _LL1777: _temp1776=
_temp1774.f1; goto _LL1773; _LL1773: _temp1772=(( struct Cyc_Absyn_For_s_struct*)
_temp1704)->f4; goto _LL1725;} else{ goto _LL1726;} _LL1726: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Switch_s: 0){ _LL1787:
_temp1786=(( struct Cyc_Absyn_Switch_s_struct*) _temp1704)->f1; goto _LL1785;
_LL1785: _temp1784=(( struct Cyc_Absyn_Switch_s_struct*) _temp1704)->f2; goto
_LL1727;} else{ goto _LL1728;} _LL1728: if(( unsigned int) _temp1704 >  1u?*((
int*) _temp1704) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1791: _temp1790=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1704)->f1; goto _LL1789; _LL1789: _temp1788=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1704)->f2; goto _LL1729;} else{ goto _LL1730;} _LL1730: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Decl_s: 0){ _LL1795:
_temp1794=(( struct Cyc_Absyn_Decl_s_struct*) _temp1704)->f1; goto _LL1793;
_LL1793: _temp1792=(( struct Cyc_Absyn_Decl_s_struct*) _temp1704)->f2; goto
_LL1731;} else{ goto _LL1732;} _LL1732: if(( unsigned int) _temp1704 >  1u?*((
int*) _temp1704) ==  Cyc_Absyn_Label_s: 0){ _LL1799: _temp1798=(( struct Cyc_Absyn_Label_s_struct*)
_temp1704)->f1; goto _LL1797; _LL1797: _temp1796=(( struct Cyc_Absyn_Label_s_struct*)
_temp1704)->f2; goto _LL1733;} else{ goto _LL1734;} _LL1734: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Do_s: 0){ _LL1805: _temp1804=((
struct Cyc_Absyn_Do_s_struct*) _temp1704)->f1; goto _LL1801; _LL1801: _temp1800=((
struct Cyc_Absyn_Do_s_struct*) _temp1704)->f2; _LL1803: _temp1802= _temp1800.f1;
goto _LL1735;} else{ goto _LL1736;} _LL1736: if(( unsigned int) _temp1704 >  1u?*((
int*) _temp1704) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1809: _temp1808=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1704)->f1; goto _LL1807; _LL1807: _temp1806=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1704)->f2; goto _LL1737;} else{ goto _LL1738;} _LL1738: if(( unsigned int)
_temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Region_s: 0){ _LL1815:
_temp1814=(( struct Cyc_Absyn_Region_s_struct*) _temp1704)->f1; goto _LL1813;
_LL1813: _temp1812=(( struct Cyc_Absyn_Region_s_struct*) _temp1704)->f2; goto
_LL1811; _LL1811: _temp1810=(( struct Cyc_Absyn_Region_s_struct*) _temp1704)->f3;
goto _LL1739;} else{ goto _LL1740;} _LL1740: if(( unsigned int) _temp1704 >  1u?*((
int*) _temp1704) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1741;} else{ goto _LL1742;}
_LL1742: if(( unsigned int) _temp1704 >  1u?*(( int*) _temp1704) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1743;} else{ goto _LL1744;} _LL1744: if(( unsigned int) _temp1704 > 
1u?*(( int*) _temp1704) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1745;} else{ goto
_LL1705;} _LL1707: return; _LL1709: Cyc_Toc_exp_to_c( nv, _temp1746); return;
_LL1711: Cyc_Toc_stmt_to_c( nv, _temp1750); s= _temp1748; continue; _LL1713: {
struct Cyc_Core_Opt* topt= 0; if( _temp1752 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1816=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1816->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1752))->topt))->v);
_temp1816;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1752));} if( s->try_depth >  0){ if( topt !=  0){ struct _tuple0* _temp1817=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1818= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1817, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1817,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1752, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1818, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1715: Cyc_Toc_exp_to_c( nv, _temp1758); Cyc_Toc_stmt_to_c( nv,
_temp1756); s= _temp1754; continue; _LL1717: Cyc_Toc_exp_to_c( nv, _temp1764);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1760); return; _LL1719: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1766 ==  0? 0:(( struct Cyc_Absyn_Stmt*) _check_null(
_temp1766))->try_depth; Cyc_Toc_do_npop_before( s->try_depth -  dest_depth, s);
return;} _LL1721: if( nv->continue_lab !=  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
nv->continue_lab))->v, 0))->r));} _temp1770= _temp1768; goto _LL1723; _LL1723:
Cyc_Toc_do_npop_before( s->try_depth - (( struct Cyc_Absyn_Stmt*) _check_null(
_temp1770))->try_depth, s); return; _LL1725: Cyc_Toc_exp_to_c( nv, _temp1782);
Cyc_Toc_exp_to_c( nv, _temp1780); Cyc_Toc_exp_to_c( nv, _temp1776); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1772); return; _LL1727: Cyc_Toc_xlate_switch( nv, s,
_temp1786, _temp1784); return; _LL1729: if( nv->fallthru_info ==  0){( int)
_throw(({ void* _temp1819[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u), _tag_arr( _temp1819, sizeof( void*), 0u));}));}{
struct _tuple6 _temp1822; struct Cyc_Dict_Dict* _temp1823; struct Cyc_List_List*
_temp1825; struct _tagged_arr* _temp1827; struct _tuple6* _temp1820=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp1822=*
_temp1820; _LL1828: _temp1827= _temp1822.f1; goto _LL1826; _LL1826: _temp1825=
_temp1822.f2; goto _LL1824; _LL1824: _temp1823= _temp1822.f3; goto _LL1821;
_LL1821: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1827, 0); Cyc_Toc_do_npop_before(
s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp1788)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp1829=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1825); struct Cyc_List_List* _temp1830=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1790); for( 0; _temp1829 !=  0;(
_temp1829=(( struct Cyc_List_List*) _check_null( _temp1829))->tl, _temp1830=((
struct Cyc_List_List*) _check_null( _temp1830))->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1830))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp1823,(
struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp1829))->hd),( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1830))->hd, 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1731:{ void* _temp1831=(
void*) _temp1794->r; struct Cyc_Absyn_Vardecl* _temp1841; int _temp1843; struct
Cyc_Absyn_Exp* _temp1845; struct Cyc_Core_Opt* _temp1847; struct Cyc_Absyn_Pat*
_temp1849; struct Cyc_List_List* _temp1851; _LL1833: if(*(( int*) _temp1831) == 
Cyc_Absyn_Var_d){ _LL1842: _temp1841=(( struct Cyc_Absyn_Var_d_struct*)
_temp1831)->f1; goto _LL1834;} else{ goto _LL1835;} _LL1835: if(*(( int*)
_temp1831) ==  Cyc_Absyn_Let_d){ _LL1850: _temp1849=(( struct Cyc_Absyn_Let_d_struct*)
_temp1831)->f1; goto _LL1848; _LL1848: _temp1847=(( struct Cyc_Absyn_Let_d_struct*)
_temp1831)->f3; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Let_d_struct*)
_temp1831)->f4; goto _LL1844; _LL1844: _temp1843=(( struct Cyc_Absyn_Let_d_struct*)
_temp1831)->f5; goto _LL1836;} else{ goto _LL1837;} _LL1837: if(*(( int*)
_temp1831) ==  Cyc_Absyn_Letv_d){ _LL1852: _temp1851=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1831)->f1; goto _LL1838;} else{ goto _LL1839;} _LL1839: goto _LL1840;
_LL1834: { struct Cyc_Toc_Env* _temp1853= Cyc_Toc_add_varmap( nv, _temp1841->name,
Cyc_Absyn_varb_exp( _temp1841->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1854=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1854[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1855; _temp1855.tag= Cyc_Absyn_Local_b;
_temp1855.f1= _temp1841; _temp1855;}); _temp1854;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1853, _temp1853, _temp1841, _temp1792); goto _LL1832;} _LL1836:{ void*
_temp1856=( void*) _temp1849->r; struct Cyc_Absyn_Vardecl* _temp1862; _LL1858:
if(( unsigned int) _temp1856 >  2u?*(( int*) _temp1856) ==  Cyc_Absyn_Var_p: 0){
_LL1863: _temp1862=(( struct Cyc_Absyn_Var_p_struct*) _temp1856)->f1; goto
_LL1859;} else{ goto _LL1860;} _LL1860: goto _LL1861; _LL1859: { struct _tuple0*
old_name= _temp1862->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1862->name= new_name; _temp1862->initializer=( struct Cyc_Absyn_Exp*)
_temp1845;( void*)( _temp1794->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1864=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1864[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1865; _temp1865.tag= Cyc_Absyn_Var_d;
_temp1865.f1= _temp1862; _temp1865;}); _temp1864;})));{ struct Cyc_Toc_Env*
_temp1866= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1867=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1867[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1868; _temp1868.tag= Cyc_Absyn_Local_b; _temp1868.f1=
_temp1862; _temp1868;}); _temp1867;}), 0)); Cyc_Toc_local_decl_to_c( _temp1866,
nv, _temp1862, _temp1792); goto _LL1857;}} _LL1861:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1849, _temp1847, _temp1845, _temp1843,
_temp1792))->r)); goto _LL1857; _LL1857:;} goto _LL1832; _LL1838: { struct Cyc_List_List*
_temp1869=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1851); if( _temp1869 ==  0){({ void* _temp1870[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1870, sizeof( void*), 0u));});}(
void*)( _temp1794->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1871=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1871[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1872; _temp1872.tag= Cyc_Absyn_Var_d;
_temp1872.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1869))->hd; _temp1872;}); _temp1871;}))); _temp1869=(( struct Cyc_List_List*)
_check_null( _temp1869))->tl; for( 0; _temp1869 !=  0; _temp1869=(( struct Cyc_List_List*)
_check_null( _temp1869))->tl){ struct Cyc_Absyn_Decl* _temp1873= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1874=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1874[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1875; _temp1875.tag= Cyc_Absyn_Var_d; _temp1875.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1869))->hd; _temp1875;}); _temp1874;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1873, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1832;} _LL1840:({
void* _temp1876[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1876, sizeof( void*), 0u));}); goto _LL1832;
_LL1832:;} return; _LL1733: s= _temp1796; continue; _LL1735: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1804); Cyc_Toc_exp_to_c( nv, _temp1802); return;
_LL1737: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1877=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1877->v=( void*) e_typ; _temp1877;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1808);{
struct Cyc_Absyn_Stmt* _temp1878= Cyc_Absyn_seq_stmt( _temp1808, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1879= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1880= Cyc_Absyn_var_exp(
_temp1879, 0); struct Cyc_Absyn_Vardecl* _temp1881= Cyc_Absyn_new_vardecl(
_temp1879, Cyc_Absyn_exn_typ, 0); _temp1880->topt=({ struct Cyc_Core_Opt*
_temp1882=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1882->v=( void*) Cyc_Absyn_exn_typ; _temp1882;});{ struct Cyc_Absyn_Pat*
_temp1883=({ struct Cyc_Absyn_Pat* _temp1899=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1899->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1901=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1901[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1902; _temp1902.tag= Cyc_Absyn_Var_p;
_temp1902.f1= _temp1881; _temp1902;}); _temp1901;})); _temp1899->topt=({ struct
Cyc_Core_Opt* _temp1900=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1900->v=( void*) Cyc_Absyn_exn_typ; _temp1900;}); _temp1899->loc= 0;
_temp1899;}); struct Cyc_Absyn_Exp* _temp1884= Cyc_Absyn_throw_exp( _temp1880, 0);
_temp1884->topt=({ struct Cyc_Core_Opt* _temp1885=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1885->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1885;});{ struct Cyc_Absyn_Stmt* _temp1886= Cyc_Absyn_exp_stmt( _temp1884,
0); struct Cyc_Absyn_Switch_clause* _temp1887=({ struct Cyc_Absyn_Switch_clause*
_temp1896=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1896->pattern= _temp1883; _temp1896->pat_vars=({ struct Cyc_Core_Opt*
_temp1897=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1897->v=( void*)({ struct Cyc_List_List* _temp1898=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1898->hd=( void*) _temp1881;
_temp1898->tl= 0; _temp1898;}); _temp1897;}); _temp1896->where_clause= 0;
_temp1896->body= _temp1886; _temp1896->loc= 0; _temp1896;}); struct Cyc_Absyn_Stmt*
_temp1888= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1806,({ struct Cyc_List_List*
_temp1895=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1895->hd=( void*) _temp1887; _temp1895->tl= 0; _temp1895;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1888);{ struct Cyc_Absyn_Exp* _temp1889= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1894=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1894->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1894->tl= 0; _temp1894;}), 0); struct Cyc_Absyn_Stmt*
_temp1890= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1893=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1893->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1893->tl= 0; _temp1893;}), 0), 0); struct Cyc_Absyn_Exp* _temp1891= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1892= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1890, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1891, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1889, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1892, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1878, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1888, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1739: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1812->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1810);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_Absyn_Exp* _temp1903[ 1u]; _temp1903[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1903, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), Cyc_Absyn_seq_stmt(
_temp1810, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp* _temp1904[ 1u]; _temp1904[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1904, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), 0), 0), 0), 0))->r)); return;} _LL1741:({ void* _temp1905[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("switchC_s", sizeof( unsigned char), 10u), _tag_arr( _temp1905, sizeof(
void*), 0u));}); return; _LL1743:({ void* _temp1906[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof(
unsigned char), 4u), _tag_arr( _temp1906, sizeof( void*), 0u));}); return;
_LL1745:({ void* _temp1907[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u),
_tag_arr( _temp1907, sizeof( void*), 0u));}); return; _LL1705:;}} struct
_tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp1908= f->args; for( 0;
_temp1908 !=  0; _temp1908=(( struct Cyc_List_List*) _check_null( _temp1908))->tl){(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1908))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1908))->hd)).f3);{ struct _tuple0* _temp1909=({ struct _tuple0* _temp1910=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp1910->f1=( void*) Cyc_Absyn_Loc_n;
_temp1910->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1908))->hd)).f1; _temp1910;}); nv= Cyc_Toc_add_varmap( nv, _temp1909, Cyc_Absyn_var_exp(
_temp1909, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp1913; void* _temp1915;
struct Cyc_Absyn_Tqual _temp1917; struct Cyc_Core_Opt* _temp1919; struct Cyc_Absyn_VarargInfo
_temp1911=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL1920: _temp1919= _temp1911.name; goto _LL1918; _LL1918: _temp1917= _temp1911.tq;
goto _LL1916; _LL1916: _temp1915=( void*) _temp1911.type; goto _LL1914; _LL1914:
_temp1913= _temp1911.inject; goto _LL1912; _LL1912: { void* _temp1921= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp1915,( void*) Cyc_Absyn_HeapRgn, _temp1917)); struct
_tuple0* _temp1922=({ struct _tuple0* _temp1925=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp1925->f1=( void*) Cyc_Absyn_Loc_n; _temp1925->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp1919))->v;
_temp1925;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp1923=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1923->hd=( void*)({ struct _tuple15* _temp1924=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp1924->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp1919))->v; _temp1924->f2= _temp1917;
_temp1924->f3= _temp1921; _temp1924;}); _temp1923->tl= 0; _temp1923;})); nv= Cyc_Toc_add_varmap(
nv, _temp1922, Cyc_Absyn_var_exp( _temp1922, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp1926=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp1926 !=  0; _temp1926=((
struct Cyc_List_List*) _check_null( _temp1926))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1926))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1926))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1927= s; _LL1929: if(
_temp1927 == ( void*) Cyc_Absyn_Abstract){ goto _LL1930;} else{ goto _LL1931;}
_LL1931: if( _temp1927 == ( void*) Cyc_Absyn_ExternC){ goto _LL1932;} else{ goto
_LL1933;} _LL1933: goto _LL1934; _LL1930: return( void*) Cyc_Absyn_Public;
_LL1932: return( void*) Cyc_Absyn_Extern; _LL1934: return s; _LL1928:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp1935=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1936=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1936->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp1936;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1937=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp1935).f2); if( _temp1937 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp1935).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1937))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Structdecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp1935).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp1938=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1938->v=(
void*) _temp1935; _temp1938;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp1939=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp1939 !=  0; _temp1939=(( struct Cyc_List_List*)
_check_null( _temp1939))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1939))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1939))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp1940=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp1941=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1941->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp1941;});}{ int
seen_defn_before; struct Cyc_Core_Opt* _temp1942=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)(( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp1940).f2); if( _temp1942 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp1940).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1942))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Uniondecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp1940).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp1943=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1943->v=(
void*) _temp1940; _temp1943;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp1944=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp1944 !=  0; _temp1944=(( struct Cyc_List_List*)
_check_null( _temp1944))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1944))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1944))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp1945=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1945->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp1945;});}{ struct
_tuple0* _temp1946= tud->name; if( tud->fields ==  0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp1946)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp1946));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp1947=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp1947 !=  0; _temp1947=(( struct Cyc_List_List*) _check_null(
_temp1947))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1947))->hd; if( f->typs ==  0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1948; _temp1948.q_const= 1;
_temp1948.q_volatile= 0; _temp1948.q_restrict= 0; _temp1948;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1949=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1949->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1950=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1950[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1951; _temp1951.tag= Cyc_Absyn_Var_d; _temp1951.f1= vd; _temp1951;});
_temp1950;}), 0); _temp1949->tl= Cyc_Toc_result_decls; _temp1949;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1952; _temp1952.q_const=
1; _temp1952.q_volatile= 0; _temp1952.q_restrict= 0; _temp1952;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1953=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1953->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1954=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1954[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1955; _temp1955.tag= Cyc_Absyn_Var_d; _temp1955.f1= vd; _temp1955;});
_temp1954;}), 0); _temp1953->tl= Cyc_Toc_result_decls; _temp1953;});{ struct Cyc_List_List*
_temp1956= 0; int i= 1;{ struct Cyc_List_List* _temp1957= f->typs; for( 0;
_temp1957 !=  0;( _temp1957=(( struct Cyc_List_List*) _check_null( _temp1957))->tl,
i ++)){ struct _tagged_arr* _temp1958= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield*
_temp1959=({ struct Cyc_Absyn_Structfield* _temp1961=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1961->name= _temp1958;
_temp1961->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp1957))->hd)).f1; _temp1961->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1957))->hd)).f2);
_temp1961->width= 0; _temp1961->attributes= 0; _temp1961;}); _temp1956=({ struct
Cyc_List_List* _temp1960=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1960->hd=( void*) _temp1959; _temp1960->tl= _temp1956; _temp1960;});}}
_temp1956=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1956); _temp1956=({ struct Cyc_List_List* _temp1962=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1962->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1963=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1963->name= Cyc_Toc_tag_sp; _temp1963->tq= Cyc_Toc_mt_tq; _temp1963->type=(
void*) Cyc_Absyn_sint_t; _temp1963->width= 0; _temp1963->attributes= 0;
_temp1963;}); _temp1962->tl= _temp1956; _temp1962;});{ struct Cyc_Absyn_Structdecl*
_temp1964=({ struct Cyc_Absyn_Structdecl* _temp1968=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp1968->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp1968->name=({ struct Cyc_Core_Opt* _temp1970=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1970->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp1970;}); _temp1968->tvs= 0; _temp1968->fields=({ struct Cyc_Core_Opt*
_temp1969=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1969->v=( void*) _temp1956; _temp1969;}); _temp1968->attributes= 0;
_temp1968;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1965=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1965->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp1966=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1966[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp1967; _temp1967.tag= Cyc_Absyn_Struct_d;
_temp1967.f1= _temp1964; _temp1967;}); _temp1966;}), 0); _temp1965->tl= Cyc_Toc_result_decls;
_temp1965;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp1971=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1971->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp1971;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp1972= xd->name; struct Cyc_List_List*
_temp1973=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp1973 !=  0; _temp1973=(( struct Cyc_List_List*) _check_null(
_temp1973))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1973))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp1974= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp1975=({ struct
Cyc_Absyn_ArrayType_struct* _temp2025=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2025[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2026; _temp2026.tag= Cyc_Absyn_ArrayType;
_temp2026.f1=( void*) Cyc_Absyn_uchar_t; _temp2026.f2= Cyc_Toc_mt_tq; _temp2026.f3=(
struct Cyc_Absyn_Exp*) _temp1974; _temp2026;}); _temp2025;}); struct Cyc_Core_Opt*
_temp1976=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp1984; int _temp1985; _LL1978: if( _temp1976 ==  0){ goto _LL1979;} else{
goto _LL1980;} _LL1980: if( _temp1976 ==  0){ goto _LL1982;} else{ _temp1984=*
_temp1976; _LL1986: _temp1985=( int) _temp1984.v; if( _temp1985 ==  0){ goto
_LL1981;} else{ goto _LL1982;}} _LL1982: goto _LL1983; _LL1979: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1992; _temp1992.tag= Cyc_Std_String_pa; _temp1992.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp1991; _temp1991.tag= Cyc_Std_Int_pa;
_temp1991.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp1990;
_temp1990.tag= Cyc_Std_Int_pa; _temp1990.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp1989; _temp1989.tag= Cyc_Std_Int_pa; _temp1989.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp1988; _temp1988.tag=
Cyc_Std_Int_pa; _temp1988.f1=( int)(( unsigned int) 0);{ void* _temp1987[ 5u]={&
_temp1988,& _temp1989,& _temp1990,& _temp1991,& _temp1992}; Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u), _tag_arr( _temp1987,
sizeof( void*), 5u));}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl* _temp1993= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp1975, initopt);( void*)( _temp1993->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1994=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1994->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1995=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1995[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1996; _temp1996.tag= Cyc_Absyn_Var_d; _temp1996.f1= _temp1993; _temp1996;});
_temp1995;}), 0); _temp1994->tl= Cyc_Toc_result_decls; _temp1994;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp1997= f->typs; for( 0; _temp1997 !=  0;( _temp1997=(( struct Cyc_List_List*)
_check_null( _temp1997))->tl, i ++)){ struct _tagged_arr* _temp1998=({ struct
_tagged_arr* _temp2002=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2002[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp2004; _temp2004.tag= Cyc_Std_Int_pa; _temp2004.f1=( int)(( unsigned int) i);{
void* _temp2003[ 1u]={& _temp2004}; Cyc_Std_aprintf( _tag_arr("f%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2003, sizeof( void*), 1u));}}); _temp2002;});
struct Cyc_Absyn_Structfield* _temp1999=({ struct Cyc_Absyn_Structfield*
_temp2001=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2001->name= _temp1998; _temp2001->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1997))->hd)).f1; _temp2001->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1997))->hd)).f2);
_temp2001->width= 0; _temp2001->attributes= 0; _temp2001;}); fields=({ struct
Cyc_List_List* _temp2000=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2000->hd=( void*) _temp1999; _temp2000->tl= fields; _temp2000;});}} fields=({
struct Cyc_List_List* _temp2005=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2005->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2006=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2006->name= Cyc_Toc_tag_sp; _temp2006->tq= Cyc_Toc_mt_tq; _temp2006->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2006->width=
0; _temp2006->attributes= 0; _temp2006;}); _temp2005->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2005;});{ struct Cyc_Absyn_Structdecl*
_temp2007=({ struct Cyc_Absyn_Structdecl* _temp2011=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2011->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2011->name=({ struct Cyc_Core_Opt* _temp2013=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2013->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2013;}); _temp2011->tvs= 0; _temp2011->fields=({ struct Cyc_Core_Opt*
_temp2012=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2012->v=( void*) fields; _temp2012;}); _temp2011->attributes= 0; _temp2011;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2008=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2008->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2009=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2009[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2010; _temp2010.tag= Cyc_Absyn_Struct_d;
_temp2010.f1= _temp2007; _temp2010;}); _temp2009;}), 0); _temp2008->tl= Cyc_Toc_result_decls;
_temp2008;});}} goto _LL1977;}} _LL1981: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2014= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2024; _temp2024.tag= Cyc_Std_String_pa;
_temp2024.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2023;
_temp2023.tag= Cyc_Std_Int_pa; _temp2023.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp2022; _temp2022.tag= Cyc_Std_Int_pa; _temp2022.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2021; _temp2021.tag=
Cyc_Std_Int_pa; _temp2021.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct
_temp2020; _temp2020.tag= Cyc_Std_Int_pa; _temp2020.f1=( int)(( unsigned int) 0);{
void* _temp2019[ 5u]={& _temp2020,& _temp2021,& _temp2022,& _temp2023,&
_temp2024}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u),
_tag_arr( _temp2019, sizeof( void*), 5u));}}}}}}), 0); struct Cyc_Absyn_Vardecl*
_temp2015= Cyc_Absyn_new_vardecl( f->name,( void*) _temp1975,( struct Cyc_Absyn_Exp*)
_temp2014);( void*)( _temp2015->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2016=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2016->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2017=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2017[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2018; _temp2018.tag= Cyc_Absyn_Var_d; _temp2018.f1= _temp2015; _temp2018;});
_temp2017;}), 0); _temp2016->tl= Cyc_Toc_result_decls; _temp2016;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL1977; _LL1983: goto
_LL1977; _LL1977:;}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2027= ed->name;( void*)( ed->sc=(
void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ struct Cyc_List_List*
_temp2028=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; _temp2028 !=  0; _temp2028=(( struct Cyc_List_List*) _check_null(
_temp2028))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2028))->hd)->tag !=  0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2028))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2028))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2029=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2035; struct Cyc_Absyn_Exp* _temp2037; struct Cyc_Absyn_Vardecl* _temp2039;
_LL2031: if(*(( int*) _temp2029) ==  Cyc_Absyn_Comprehension_e){ _LL2040:
_temp2039=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2029)->f1; goto
_LL2038; _LL2038: _temp2037=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2029)->f2; goto _LL2036; _LL2036: _temp2035=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2029)->f3; goto _LL2032;} else{ goto _LL2033;} _LL2033: goto _LL2034;
_LL2032: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2039, _temp2037, _temp2035, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2030; _LL2034: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2041= Cyc_Toc_copy_env( init_nv);
_temp2041->toplevel= 1; Cyc_Toc_exp_to_c( _temp2041, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2030; _LL2030:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2042= _new_region(); struct _RegionHandle* prgn=& _temp2042;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2045; struct Cyc_List_List*
_temp2047; struct Cyc_Toc_Env* _temp2049; struct _tuple11 _temp2043= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2050: _temp2049= _temp2043.f1; goto _LL2048; _LL2048: _temp2047=
_temp2043.f2; goto _LL2046; _LL2046: _temp2045= _temp2043.f3; goto _LL2044;
_LL2044: Cyc_Toc_stmt_to_c( _temp2049, s);{ struct Cyc_Absyn_Stmt* _temp2051=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2045, _temp2051, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2052= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2053= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2052, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2045,
Cyc_Absyn_seq_stmt( _temp2053, _temp2051, 0), 0), 0);} for( 0; _temp2047 !=  0;
_temp2047=(( struct Cyc_List_List*) _check_null( _temp2047))->tl){ struct
_tuple12 _temp2056; void* _temp2057; struct _tuple0* _temp2059; struct _tuple12*
_temp2054=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2047))->hd;
_temp2056=* _temp2054; _LL2060: _temp2059= _temp2056.f1; goto _LL2058; _LL2058:
_temp2057= _temp2056.f2; goto _LL2055; _LL2055: s= Cyc_Absyn_declare_stmt(
_temp2059, _temp2057, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ void* _temp2061[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u), _tag_arr( _temp2061, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2062=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2088; struct Cyc_Absyn_Fndecl* _temp2090; struct Cyc_Absyn_Structdecl*
_temp2092; struct Cyc_Absyn_Uniondecl* _temp2094; struct Cyc_Absyn_Tuniondecl*
_temp2096; struct Cyc_Absyn_Enumdecl* _temp2098; struct Cyc_Absyn_Typedefdecl*
_temp2100; struct Cyc_List_List* _temp2102; struct Cyc_List_List* _temp2104;
struct Cyc_List_List* _temp2106; _LL2064: if(*(( int*) _temp2062) ==  Cyc_Absyn_Var_d){
_LL2089: _temp2088=(( struct Cyc_Absyn_Var_d_struct*) _temp2062)->f1; goto
_LL2065;} else{ goto _LL2066;} _LL2066: if(*(( int*) _temp2062) ==  Cyc_Absyn_Fn_d){
_LL2091: _temp2090=(( struct Cyc_Absyn_Fn_d_struct*) _temp2062)->f1; goto
_LL2067;} else{ goto _LL2068;} _LL2068: if(*(( int*) _temp2062) ==  Cyc_Absyn_Let_d){
goto _LL2069;} else{ goto _LL2070;} _LL2070: if(*(( int*) _temp2062) ==  Cyc_Absyn_Letv_d){
goto _LL2071;} else{ goto _LL2072;} _LL2072: if(*(( int*) _temp2062) ==  Cyc_Absyn_Struct_d){
_LL2093: _temp2092=(( struct Cyc_Absyn_Struct_d_struct*) _temp2062)->f1; goto
_LL2073;} else{ goto _LL2074;} _LL2074: if(*(( int*) _temp2062) ==  Cyc_Absyn_Union_d){
_LL2095: _temp2094=(( struct Cyc_Absyn_Union_d_struct*) _temp2062)->f1; goto
_LL2075;} else{ goto _LL2076;} _LL2076: if(*(( int*) _temp2062) ==  Cyc_Absyn_Tunion_d){
_LL2097: _temp2096=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2062)->f1; goto
_LL2077;} else{ goto _LL2078;} _LL2078: if(*(( int*) _temp2062) ==  Cyc_Absyn_Enum_d){
_LL2099: _temp2098=(( struct Cyc_Absyn_Enum_d_struct*) _temp2062)->f1; goto
_LL2079;} else{ goto _LL2080;} _LL2080: if(*(( int*) _temp2062) ==  Cyc_Absyn_Typedef_d){
_LL2101: _temp2100=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2062)->f1; goto
_LL2081;} else{ goto _LL2082;} _LL2082: if(*(( int*) _temp2062) ==  Cyc_Absyn_Namespace_d){
_LL2103: _temp2102=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2062)->f2; goto
_LL2083;} else{ goto _LL2084;} _LL2084: if(*(( int*) _temp2062) ==  Cyc_Absyn_Using_d){
_LL2105: _temp2104=(( struct Cyc_Absyn_Using_d_struct*) _temp2062)->f2; goto
_LL2085;} else{ goto _LL2086;} _LL2086: if(*(( int*) _temp2062) ==  Cyc_Absyn_ExternC_d){
_LL2107: _temp2106=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2062)->f1; goto
_LL2087;} else{ goto _LL2063;} _LL2065: { struct _tuple0* _temp2108= _temp2088->name;
if(( void*) _temp2088->sc == ( void*) Cyc_Absyn_ExternC){ _temp2108=({ struct
_tuple0* _temp2109=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2109->f1= Cyc_Absyn_rel_ns_null; _temp2109->f2=(* _temp2108).f2; _temp2109;});}
if( _temp2088->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2088->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2088->name,
Cyc_Absyn_varb_exp( _temp2108,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2110=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2110[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2111; _temp2111.tag= Cyc_Absyn_Global_b;
_temp2111.f1= _temp2088; _temp2111;}); _temp2110;}), 0)); _temp2088->name=
_temp2108;( void*)( _temp2088->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2088->sc));(
void*)( _temp2088->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2088->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2112=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2112->hd=( void*) d; _temp2112->tl=
Cyc_Toc_result_decls; _temp2112;}); goto _LL2063;} _LL2067: { struct _tuple0*
_temp2113= _temp2090->name; if(( void*) _temp2090->sc == ( void*) Cyc_Absyn_ExternC){
_temp2113=({ struct _tuple0* _temp2114=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2114->f1= Cyc_Absyn_rel_ns_null; _temp2114->f2=(*
_temp2113).f2; _temp2114;});} nv= Cyc_Toc_add_varmap( nv, _temp2090->name, Cyc_Absyn_var_exp(
_temp2113, 0)); _temp2090->name= _temp2113; Cyc_Toc_fndecl_to_c( nv, _temp2090);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2115->hd=( void*) d; _temp2115->tl=
Cyc_Toc_result_decls; _temp2115;}); goto _LL2063;} _LL2069: goto _LL2071;
_LL2071:({ void* _temp2116[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2116, sizeof( void*), 0u));}); goto _LL2063;
_LL2073: Cyc_Toc_structdecl_to_c( _temp2092); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2117=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2117->hd=( void*) d; _temp2117->tl= Cyc_Toc_result_decls; _temp2117;});
goto _LL2063; _LL2075: Cyc_Toc_uniondecl_to_c( _temp2094); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2118=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2118->hd=( void*) d; _temp2118->tl= Cyc_Toc_result_decls;
_temp2118;}); goto _LL2063; _LL2077: if( _temp2096->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2096);} else{ Cyc_Toc_tuniondecl_to_c( _temp2096);} goto _LL2063; _LL2079:
Cyc_Toc_enumdecl_to_c( nv, _temp2098); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2119=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2119->hd=( void*) d; _temp2119->tl= Cyc_Toc_result_decls; _temp2119;});
goto _LL2063; _LL2081: _temp2100->name= _temp2100->name; _temp2100->tvs= 0;(
void*)( _temp2100->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2100->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2120=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2120->hd=( void*) d; _temp2120->tl=
Cyc_Toc_result_decls; _temp2120;}); goto _LL2063; _LL2083: _temp2104= _temp2102;
goto _LL2085; _LL2085: _temp2106= _temp2104; goto _LL2087; _LL2087: nv= Cyc_Toc_decls_to_c(
nv, _temp2106, top); goto _LL2063; _LL2063:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
