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
extern struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
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
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists(
int(* pred)( void*), struct Cyc_List_List* x); extern int Cyc_List_list_cmp( int(*
cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern
int Cyc_List_list_prefix( int(* cmp)( void*, void*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Std___sFILE*
f); extern struct _tagged_arr Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int w);
extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_arr s); extern struct Cyc_PP_Doc*
Cyc_PP_textptr( struct _tagged_arr* p); extern struct Cyc_PP_Doc* Cyc_PP_nest(
int k, struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)(
void*), struct _tagged_arr sep, struct Cyc_List_List* l); extern struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr
sep, struct Cyc_List_List* l); extern struct Cyc_PP_Doc* Cyc_PP_group( struct
_tagged_arr start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List*
l); extern struct Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_arr start, struct
_tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* l); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); struct Cyc_Buffer_t; extern int
Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2); extern
struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*, struct _tagged_arr);
extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*); struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r; extern struct
Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r; extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Std___sFILE*
f); extern struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d);
extern struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct
_tagged_arr Cyc_Absynpp_kind2string( void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_arr Cyc_Absynpp_decllist2string( struct
Cyc_List_List* tdl); extern struct _tagged_arr Cyc_Absynpp_prim2string( void* p);
extern struct _tagged_arr Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p);
extern struct _tagged_arr Cyc_Absynpp_scope2string( void* sc); extern struct
_tagged_arr Cyc_Absynpp_cyc_string; extern struct _tagged_arr* Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_arr Cyc_Absynpp_char_escape( unsigned char); extern struct _tagged_arr
Cyc_Absynpp_string_escape( struct _tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str(
void* p); extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s);
struct _tuple3{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
extern struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct
_tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ;
extern struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t);
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern
struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs); extern struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List* cs); extern struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp* e);
extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct _tuple0*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc(
void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc( int inprec, void* p,
struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de);
extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual tq, void*
t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
static int Cyc_Absynpp_expand_typedefs; static int Cyc_Absynpp_qvar_to_Cids;
static unsigned char _temp0[ 4u]="Cyc"; struct _tagged_arr Cyc_Absynpp_cyc_string={
_temp0, _temp0, _temp0 +  4u}; struct _tagged_arr* Cyc_Absynpp_cyc_stringptr=&
Cyc_Absynpp_cyc_string; static int Cyc_Absynpp_add_cyc_prefix; static int Cyc_Absynpp_to_VC;
static int Cyc_Absynpp_decls_first; static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars; static int Cyc_Absynpp_print_all_kinds;
static int Cyc_Absynpp_print_using_stmts; static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars; static int Cyc_Absynpp_use_curr_namespace;
static struct Cyc_List_List* Cyc_Absynpp_curr_namespace= 0; struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params* fs){ Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs; Cyc_Absynpp_qvar_to_Cids= fs->qvar_to_Cids; Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix; Cyc_Absynpp_to_VC= fs->to_VC; Cyc_Absynpp_decls_first= fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars= fs->rewrite_temp_tvars; Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars; Cyc_Absynpp_print_all_kinds= fs->print_all_kinds; Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts; Cyc_Absynpp_print_externC_stmts= fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars= fs->print_full_evars; Cyc_Absynpp_use_curr_namespace=
fs->use_curr_namespace; Cyc_Absynpp_curr_namespace= fs->curr_namespace;} struct
Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={ 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1,
0}; struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={ 1, 0, 0, 0, 0, 1, 0, 0,
1, 1, 0, 1, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={ 1, 1, 1, 0, 1,
0, 0, 0, 0, 0, 0, 0, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={ 0,
0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}; static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_arr* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp1=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp1->hd=( void*) v; _temp1->tl= 0; _temp1;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl ==  0){* l= 0;} else{ Cyc_Absynpp_suppr_last(&(( struct
Cyc_List_List*) _check_null(* l))->tl);}} static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)( struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a':
_LL2: return _tag_arr("\\a", sizeof( unsigned char), 3u); case '\b': _LL3:
return _tag_arr("\\b", sizeof( unsigned char), 3u); case '\f': _LL4: return
_tag_arr("\\f", sizeof( unsigned char), 3u); case '\n': _LL5: return _tag_arr("\\n",
sizeof( unsigned char), 3u); case '\r': _LL6: return _tag_arr("\\r", sizeof(
unsigned char), 3u); case '\t': _LL7: return _tag_arr("\\t", sizeof(
unsigned char), 3u); case '\v': _LL8: return _tag_arr("\\v", sizeof(
unsigned char), 3u); case '\\': _LL9: return _tag_arr("\\\\", sizeof(
unsigned char), 3u); case '"': _LL10: return _tag_arr("\"", sizeof(
unsigned char), 2u); case '\'': _LL11: return _tag_arr("\\'", sizeof(
unsigned char), 3u); default: _LL12: if( c >= ' '? c <= '~': 0){ struct
_tagged_arr _temp14= Cyc_Core_new_string( 1);*(( unsigned char*)
_check_unknown_subscript( _temp14, sizeof( unsigned char), 0))= c; return(
struct _tagged_arr) _temp14;} else{ struct _tagged_arr _temp15= Cyc_Core_new_string(
4); int j= 0;*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript(
_temp15, sizeof( unsigned char), j ++))=( unsigned char)('0' + (( unsigned char)
c >>  6 &  7));*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))=( unsigned char)('0' + ( c >>  3 &  7));*(( unsigned char*)
_check_unknown_subscript( _temp15, sizeof( unsigned char), j ++))=(
unsigned char)('0' + ( c &  7)); return( struct _tagged_arr) _temp15;}}} static
int Cyc_Absynpp_special( struct _tagged_arr s){ int sz=( int)( _get_arr_size( s,
sizeof( unsigned char)) -  1);{ int i= 0; for( 0; i <  sz; i ++){ unsigned char
c=*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i)); if((( c <= ' '? 1: c >= '~')? 1: c == '"')? 1: c == '\\'){ return 1;}}}
return 0;} struct _tagged_arr Cyc_Absynpp_string_escape( struct _tagged_arr s){
if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)( _get_arr_size( s,
sizeof( unsigned char)) -  1); if( n >  0?*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), n)) == '\000': 0){ n --;}{
int len= 0;{ int i= 0; for( 0; i <=  n; i ++){ unsigned char _temp16=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); _LL18:
if( _temp16 == '\a'){ goto _LL19;} else{ goto _LL20;} _LL20: if( _temp16 == '\b'){
goto _LL21;} else{ goto _LL22;} _LL22: if( _temp16 == '\f'){ goto _LL23;} else{
goto _LL24;} _LL24: if( _temp16 == '\n'){ goto _LL25;} else{ goto _LL26;} _LL26:
if( _temp16 == '\r'){ goto _LL27;} else{ goto _LL28;} _LL28: if( _temp16 == '\t'){
goto _LL29;} else{ goto _LL30;} _LL30: if( _temp16 == '\v'){ goto _LL31;} else{
goto _LL32;} _LL32: if( _temp16 == '\\'){ goto _LL33;} else{ goto _LL34;} _LL34:
if( _temp16 == '"'){ goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37; _LL19:
goto _LL21; _LL21: goto _LL23; _LL23: goto _LL25; _LL25: goto _LL27; _LL27: goto
_LL29; _LL29: goto _LL31; _LL31: goto _LL33; _LL33: goto _LL35; _LL35: len += 2;
goto _LL17; _LL37: if( _temp16 >= ' '? _temp16 <= '~': 0){ len ++;} else{ len +=
4;} goto _LL17; _LL17:;}}{ struct _tagged_arr t= Cyc_Core_new_string( len); int
j= 0;{ int i= 0; for( 0; i <=  n; i ++){ unsigned char _temp38=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); _LL40:
if( _temp38 == '\a'){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp38 == '\b'){
goto _LL43;} else{ goto _LL44;} _LL44: if( _temp38 == '\f'){ goto _LL45;} else{
goto _LL46;} _LL46: if( _temp38 == '\n'){ goto _LL47;} else{ goto _LL48;} _LL48:
if( _temp38 == '\r'){ goto _LL49;} else{ goto _LL50;} _LL50: if( _temp38 == '\t'){
goto _LL51;} else{ goto _LL52;} _LL52: if( _temp38 == '\v'){ goto _LL53;} else{
goto _LL54;} _LL54: if( _temp38 == '\\'){ goto _LL55;} else{ goto _LL56;} _LL56:
if( _temp38 == '"'){ goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL41:*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='a';
goto _LL39; _LL43:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='b'; goto _LL39; _LL45:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='f';
goto _LL39; _LL47:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='n'; goto _LL39; _LL49:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='r';
goto _LL39; _LL51:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='t'; goto _LL39; _LL53:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='v';
goto _LL39; _LL55:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='\\'; goto _LL39; _LL57:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='"';
goto _LL39; _LL59: if( _temp38 >= ' '? _temp38 <= '~': 0){*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))= _temp38;} else{*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))=(
unsigned char)('0' + ( _temp38 >>  6 &  7));*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))=( unsigned char)('0'
+ ( _temp38 >>  3 &  7));*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))=( unsigned char)('0' + ( _temp38 &  7));} goto
_LL39; _LL39:;}} return( struct _tagged_arr) t;}}}} static unsigned char _temp60[
9u]="restrict"; static struct _tagged_arr Cyc_Absynpp_restrict_string={ _temp60,
_temp60, _temp60 +  9u}; static unsigned char _temp61[ 9u]="volatile"; static
struct _tagged_arr Cyc_Absynpp_volatile_string={ _temp61, _temp61, _temp61 +  9u};
static unsigned char _temp62[ 6u]="const"; static struct _tagged_arr Cyc_Absynpp_const_str={
_temp62, _temp62, _temp62 +  6u}; static struct _tagged_arr* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_arr* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_arr* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_str; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp63=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp63->hd=( void*) Cyc_Absynpp_restrict_sp; _temp63->tl= l; _temp63;});} if(
tq.q_volatile){ l=({ struct Cyc_List_List* _temp64=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp64->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp64->tl= l; _temp64;});} if( tq.q_const){ l=({ struct Cyc_List_List* _temp65=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp65->hd=(
void*) Cyc_Absynpp_const_sp; _temp65->tl= l; _temp65;});} return Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char),
2u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_arr Cyc_Absynpp_kind2string( void* k){ void*
_temp66= k; _LL68: if( _temp66 == ( void*) Cyc_Absyn_AnyKind){ goto _LL69;}
else{ goto _LL70;} _LL70: if( _temp66 == ( void*) Cyc_Absyn_MemKind){ goto _LL71;}
else{ goto _LL72;} _LL72: if( _temp66 == ( void*) Cyc_Absyn_BoxKind){ goto _LL73;}
else{ goto _LL74;} _LL74: if( _temp66 == ( void*) Cyc_Absyn_RgnKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp66 == ( void*) Cyc_Absyn_EffKind){ goto _LL77;}
else{ goto _LL67;} _LL69: return _tag_arr("A", sizeof( unsigned char), 2u);
_LL71: return _tag_arr("M", sizeof( unsigned char), 2u); _LL73: return _tag_arr("B",
sizeof( unsigned char), 2u); _LL75: return _tag_arr("R", sizeof( unsigned char),
2u); _LL77: return _tag_arr("E", sizeof( unsigned char), 2u); _LL67:;} struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void* _temp78=(
void*)( Cyc_Absyn_compress_conref( c))->v; void* _temp84; _LL80: if((
unsigned int) _temp78 >  1u?*(( int*) _temp78) ==  Cyc_Absyn_Eq_constr: 0){
_LL85: _temp84=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp78)->f1; goto
_LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL81: return Cyc_Absynpp_kind2string(
_temp84); _LL83: return _tag_arr("?", sizeof( unsigned char), 2u); _LL79:;}
struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string(
k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){
return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_PP_egroup( _tag_arr("<", sizeof(
unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));}
struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void*
_temp86=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v; void* _temp94; void*
_temp96; _LL88: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) ==  Cyc_Absyn_Eq_constr:
0){ _LL95: _temp94=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp86)->f1;
if( _temp94 == ( void*) Cyc_Absyn_BoxKind){ goto _LL89;} else{ goto _LL90;}}
else{ goto _LL90;} _LL90: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) == 
Cyc_Absyn_Eq_constr: 0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp86)->f1; goto _LL91;} else{ goto _LL92;} _LL92: goto _LL93; _LL89: return
Cyc_PP_textptr( tv->name); _LL91: return({ struct Cyc_PP_Doc* _temp98[ 3u];
_temp98[ 2u]= Cyc_Absynpp_kind2doc( _temp96); _temp98[ 1u]= Cyc_PP_text(
_tag_arr("::", sizeof( unsigned char), 3u)); _temp98[ 0u]= Cyc_PP_textptr( tv->name);
Cyc_PP_cat( _tag_arr( _temp98, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL93:
return({ struct Cyc_PP_Doc* _temp99[ 2u]; _temp99[ 1u]= Cyc_PP_text( _tag_arr("/*::?*/",
sizeof( unsigned char), 8u)); _temp99[ 0u]= Cyc_PP_textptr( tv->name); Cyc_PP_cat(
_tag_arr( _temp99, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL87:;} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){ return Cyc_PP_egroup(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_arr* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup( _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr,(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_get_name, tvs)));} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_noncallatt2doc(
void* att){ void* _temp100= att; _LL102: if( _temp100 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL103;} else{ goto _LL104;} _LL104: if( _temp100 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL105;} else{ goto _LL106;} _LL106: if( _temp100 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL107;} else{ goto _LL108;} _LL108: goto _LL109; _LL103: return Cyc_PP_nil_doc();
_LL105: return Cyc_PP_nil_doc(); _LL107: return Cyc_PP_nil_doc(); _LL109: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL101:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp110=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL112: if( _temp110 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp110 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp110 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL113: return Cyc_PP_text(
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL115: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL117: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL119: goto _LL111;
_LL111:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 !=  0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp120=( void*)(( struct Cyc_List_List*) _check_null( atts2))->hd;
_LL122: if( _temp120 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL123;} else{
goto _LL124;} _LL124: if( _temp120 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL125;}
else{ goto _LL126;} _LL126: if( _temp120 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL127;} else{ goto _LL128;} _LL128: goto _LL129; _LL123: goto _LL121;
_LL125: goto _LL121; _LL127: goto _LL121; _LL129: hasatt= 1; goto _LL121; _LL121:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc* _temp130[ 3u];
_temp130[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp130[
1u]= Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); _temp130[ 0u]= Cyc_PP_text(
_tag_arr("__declspec(", sizeof( unsigned char), 12u)); Cyc_PP_cat( _tag_arr(
_temp130, sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts ==  0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc* _temp131[ 2u]; _temp131[ 1u]= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); _temp131[ 0u]= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp131, sizeof( struct Cyc_PP_Doc*),
2u));});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List* tms){ if( tms
==  0){ return 0;}{ void* _temp132=( void*)(( struct Cyc_List_List*) _check_null(
tms))->hd; _LL134: if(( unsigned int) _temp132 >  1u?*(( int*) _temp132) ==  Cyc_Absyn_Pointer_mod:
0){ goto _LL135;} else{ goto _LL136;} _LL136: if(( unsigned int) _temp132 >  1u?*((
int*) _temp132) ==  Cyc_Absyn_Attributes_mod: 0){ goto _LL137;} else{ goto
_LL138;} _LL138: goto _LL139; _LL135: return 1; _LL137: if( ! Cyc_Absynpp_to_VC){
return 0;} return Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl); _LL139: return 0; _LL133:;}} extern struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct
Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms ==  0){ return d;}{ struct
Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d,(( struct Cyc_List_List*) _check_null(
tms))->tl); struct Cyc_PP_Doc* p_rest=({ struct Cyc_PP_Doc* _temp222[ 3u];
_temp222[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp222[
1u]= rest; _temp222[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp222, sizeof( struct Cyc_PP_Doc*), 3u));}); void*
_temp140=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp154; void* _temp156; struct Cyc_List_List* _temp158; int _temp160; struct
Cyc_Position_Segment* _temp162; struct Cyc_List_List* _temp164; struct Cyc_Absyn_Tqual
_temp166; void* _temp168; void* _temp170; _LL142: if( _temp140 == ( void*) Cyc_Absyn_Carray_mod){
goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_ConstArray_mod: 0){ _LL155: _temp154=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp140)->f1; goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int)
_temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Function_mod: 0){ _LL157:
_temp156=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp140)->f1; goto
_LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp140 >  1u?*(( int*)
_temp140) ==  Cyc_Absyn_Attributes_mod: 0){ _LL159: _temp158=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp140)->f2; goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int)
_temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL165:
_temp164=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f1; goto _LL163;
_LL163: _temp162=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f2; goto
_LL161; _LL161: _temp160=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f3;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_Pointer_mod: 0){ _LL171: _temp170=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f1; goto _LL169; _LL169:
_temp168=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f2; goto
_LL167; _LL167: _temp166=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f3;
goto _LL153;} else{ goto _LL141;} _LL143: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc* _temp172[ 2u]; _temp172[ 1u]= Cyc_PP_text( _tag_arr("[]", sizeof(
unsigned char), 3u)); _temp172[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp172,
sizeof( struct Cyc_PP_Doc*), 2u));}); _LL145: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc* _temp173[ 4u]; _temp173[ 3u]= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); _temp173[ 2u]= Cyc_Absynpp_exp2doc( _temp154); _temp173[ 1u]=
Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); _temp173[ 0u]= rest;
Cyc_PP_cat( _tag_arr( _temp173, sizeof( struct Cyc_PP_Doc*), 4u));}); _LL147:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;}{ void* _temp174= _temp156; struct Cyc_List_List* _temp180; struct
Cyc_Core_Opt* _temp182; struct Cyc_Absyn_VarargInfo* _temp184; int _temp186;
struct Cyc_List_List* _temp188; struct Cyc_Position_Segment* _temp190; struct
Cyc_List_List* _temp192; _LL176: if(*(( int*) _temp174) ==  Cyc_Absyn_WithTypes){
_LL189: _temp188=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f1; goto
_LL187; _LL187: _temp186=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f2;
goto _LL185; _LL185: _temp184=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f3;
goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f4;
goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f5;
goto _LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp174) ==  Cyc_Absyn_NoTypes){
_LL193: _temp192=(( struct Cyc_Absyn_NoTypes_struct*) _temp174)->f1; goto _LL191;
_LL191: _temp190=(( struct Cyc_Absyn_NoTypes_struct*) _temp174)->f2; goto _LL179;}
else{ goto _LL175;} _LL177: return({ struct Cyc_PP_Doc* _temp194[ 2u]; _temp194[
1u]= Cyc_Absynpp_funargs2doc( _temp188, _temp186, _temp184, _temp182, _temp180);
_temp194[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp194, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL179: return({ struct Cyc_PP_Doc* _temp195[ 2u]; _temp195[ 1u]= Cyc_PP_group(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, _temp192)); _temp195[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp195,
sizeof( struct Cyc_PP_Doc*), 2u));}); _LL175:;} _LL149: if( ! Cyc_Absynpp_to_VC){
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc* _temp196[ 2u]; _temp196[ 1u]= Cyc_Absynpp_atts2doc(
_temp158); _temp196[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp196, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ return({ struct Cyc_PP_Doc* _temp197[ 2u]; _temp197[ 1u]=
rest; _temp197[ 0u]= Cyc_Absynpp_callconv2doc( _temp158); Cyc_PP_cat( _tag_arr(
_temp197, sizeof( struct Cyc_PP_Doc*), 2u));});} return rest;} _LL151: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} if( _temp160){
return({ struct Cyc_PP_Doc* _temp198[ 2u]; _temp198[ 1u]= Cyc_Absynpp_ktvars2doc(
_temp164); _temp198[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp198, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp199[ 2u];
_temp199[ 1u]= Cyc_Absynpp_tvars2doc( _temp164); _temp199[ 0u]= rest; Cyc_PP_cat(
_tag_arr( _temp199, sizeof( struct Cyc_PP_Doc*), 2u));});} _LL153: { struct Cyc_PP_Doc*
ptr;{ void* _temp200= _temp170; struct Cyc_Absyn_Exp* _temp208; struct Cyc_Absyn_Exp*
_temp210; _LL202: if(( unsigned int) _temp200 >  1u?*(( int*) _temp200) ==  Cyc_Absyn_Nullable_ps:
0){ _LL209: _temp208=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp200)->f1;
goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int) _temp200 >  1u?*((
int*) _temp200) ==  Cyc_Absyn_NonNullable_ps: 0){ _LL211: _temp210=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp200)->f1; goto _LL205;} else{ goto _LL206;} _LL206: if( _temp200 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL207;} else{ goto _LL201;} _LL203: if( Cyc_Evexp_eval_const_uint_exp(
_temp208) ==  1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc* _temp212[ 4u]; _temp212[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp212[ 2u]= Cyc_Absynpp_exp2doc(
_temp208); _temp212[ 1u]= Cyc_PP_text( _tag_arr("{", sizeof( unsigned char), 2u));
_temp212[ 0u]= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp212, sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL201; _LL205:
if( Cyc_Evexp_eval_const_uint_exp( _temp210) ==  1){ ptr= Cyc_PP_text( _tag_arr("@",
sizeof( unsigned char), 2u));} else{ ptr=({ struct Cyc_PP_Doc* _temp213[ 4u];
_temp213[ 3u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp213[
2u]= Cyc_Absynpp_exp2doc( _temp210); _temp213[ 1u]= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); _temp213[ 0u]= Cyc_PP_text( _tag_arr("@", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp213, sizeof( struct Cyc_PP_Doc*),
4u));});} goto _LL201; _LL207: ptr= Cyc_PP_text( _tag_arr("?", sizeof(
unsigned char), 2u)); goto _LL201; _LL201:;}{ void* _temp214= Cyc_Tcutil_compress(
_temp168); _LL216: if( _temp214 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL217;}
else{ goto _LL218;} _LL218: goto _LL219; _LL217: return({ struct Cyc_PP_Doc*
_temp220[ 2u]; _temp220[ 1u]= rest; _temp220[ 0u]= ptr; Cyc_PP_cat( _tag_arr(
_temp220, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL219: return({ struct Cyc_PP_Doc*
_temp221[ 4u]; _temp221[ 3u]= rest; _temp221[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp221[ 1u]= Cyc_Absynpp_typ2doc( _temp168);
_temp221[ 0u]= ptr; Cyc_PP_cat( _tag_arr( _temp221, sizeof( struct Cyc_PP_Doc*),
4u));}); _LL215:;}} _LL141:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){
void* _temp223= Cyc_Tcutil_compress( t); _LL225: if( _temp223 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL228: return Cyc_Absynpp_ntyp2doc(
t); _LL224:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
rgions, struct Cyc_List_List** effects, void* t){ void* _temp229= Cyc_Tcutil_compress(
t); void* _temp237; struct Cyc_List_List* _temp239; _LL231: if(( unsigned int)
_temp229 >  4u?*(( int*) _temp229) ==  Cyc_Absyn_AccessEff: 0){ _LL238: _temp237=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp229)->f1; goto _LL232;} else{
goto _LL233;} _LL233: if(( unsigned int) _temp229 >  4u?*(( int*) _temp229) == 
Cyc_Absyn_JoinEff: 0){ _LL240: _temp239=(( struct Cyc_Absyn_JoinEff_struct*)
_temp229)->f1; goto _LL234;} else{ goto _LL235;} _LL235: goto _LL236; _LL232:*
rgions=({ struct Cyc_List_List* _temp241=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp241->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp237); _temp241->tl=* rgions; _temp241;}); goto _LL230; _LL234: for( 0;
_temp239 !=  0; _temp239=(( struct Cyc_List_List*) _check_null( _temp239))->tl){
Cyc_Absynpp_effects2docs( rgions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp239))->hd);} goto _LL230; _LL236:* effects=({ struct Cyc_List_List*
_temp242=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp242->hd=( void*) Cyc_Absynpp_typ2doc( t); _temp242->tl=* effects; _temp242;});
goto _LL230; _LL230:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* rgions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
rgions,& effects, t); rgions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( rgions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( rgions ==  0? effects !=  0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp243= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), rgions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp244=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp244->hd=( void*) _temp243; _temp244->tl= 0;
_temp244;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp245= t; struct Cyc_Core_Opt* _temp293; int _temp295; struct Cyc_Core_Opt*
_temp297; struct Cyc_Core_Opt* _temp299; struct Cyc_Absyn_Tvar* _temp301; struct
Cyc_Absyn_TunionInfo _temp303; void* _temp305; struct Cyc_List_List* _temp307;
void* _temp309; struct Cyc_Absyn_TunionFieldInfo _temp311; struct Cyc_List_List*
_temp313; void* _temp315; void* _temp317; void* _temp319; struct Cyc_List_List*
_temp321; struct Cyc_List_List* _temp323; struct _tuple0* _temp325; struct Cyc_List_List*
_temp327; struct _tuple0* _temp329; struct Cyc_List_List* _temp331; struct Cyc_List_List*
_temp333; struct _tuple0* _temp335; struct Cyc_Core_Opt* _temp337; struct Cyc_List_List*
_temp339; struct _tuple0* _temp341; void* _temp343; void* _temp345; _LL247: if((
unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_ArrayType: 0){
goto _LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_FnType: 0){ goto _LL250;} else{ goto _LL251;}
_LL251: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_PointerType:
0){ goto _LL252;} else{ goto _LL253;} _LL253: if( _temp245 == ( void*) Cyc_Absyn_VoidType){
goto _LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_Evar: 0){ _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f1; goto _LL298; _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f2; goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f3; goto _LL294; _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f4; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_VarType: 0){ _LL302: _temp301=((
struct Cyc_Absyn_VarType_struct*) _temp245)->f1; goto _LL258;} else{ goto _LL259;}
_LL259: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TunionType:
0){ _LL304: _temp303=(( struct Cyc_Absyn_TunionType_struct*) _temp245)->f1;
_LL310: _temp309=( void*) _temp303.tunion_info; goto _LL308; _LL308: _temp307=
_temp303.targs; goto _LL306; _LL306: _temp305=( void*) _temp303.rgn; goto _LL260;}
else{ goto _LL261;} _LL261: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_TunionFieldType: 0){ _LL312: _temp311=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp245)->f1; _LL316: _temp315=( void*) _temp311.field_info; goto _LL314;
_LL314: _temp313= _temp311.targs; goto _LL262;} else{ goto _LL263;} _LL263: if((
unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_IntType: 0){
_LL320: _temp319=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp245)->f1;
goto _LL318; _LL318: _temp317=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp245)->f2; goto _LL264;} else{ goto _LL265;} _LL265: if( _temp245 == ( void*)
Cyc_Absyn_FloatType){ goto _LL266;} else{ goto _LL267;} _LL267: if( _temp245 == (
void*) Cyc_Absyn_DoubleType){ goto _LL268;} else{ goto _LL269;} _LL269: if((
unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TupleType: 0){
_LL322: _temp321=(( struct Cyc_Absyn_TupleType_struct*) _temp245)->f1; goto
_LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp245 >  4u?*(( int*)
_temp245) ==  Cyc_Absyn_StructType: 0){ _LL326: _temp325=(( struct Cyc_Absyn_StructType_struct*)
_temp245)->f1; goto _LL324; _LL324: _temp323=(( struct Cyc_Absyn_StructType_struct*)
_temp245)->f2; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_UnionType: 0){ _LL330: _temp329=((
struct Cyc_Absyn_UnionType_struct*) _temp245)->f1; goto _LL328; _LL328: _temp327=((
struct Cyc_Absyn_UnionType_struct*) _temp245)->f2; goto _LL274;} else{ goto
_LL275;} _LL275: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_AnonStructType:
0){ _LL332: _temp331=(( struct Cyc_Absyn_AnonStructType_struct*) _temp245)->f1;
goto _LL276;} else{ goto _LL277;} _LL277: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_AnonUnionType: 0){ _LL334: _temp333=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp245)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_EnumType: 0){ _LL336: _temp335=((
struct Cyc_Absyn_EnumType_struct*) _temp245)->f1; goto _LL280;} else{ goto
_LL281;} _LL281: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TypedefType:
0){ _LL342: _temp341=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1;
goto _LL340; _LL340: _temp339=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2;
goto _LL338; _LL338: _temp337=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_RgnHandleType: 0){ _LL344: _temp343=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp245)->f1; goto _LL284;} else{ goto
_LL285;} _LL285: if( _temp245 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL286;}
else{ goto _LL287;} _LL287: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_RgnsEff: 0){ _LL346: _temp345=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp245)->f1; goto _LL288;} else{ goto _LL289;} _LL289: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_AccessEff: 0){ goto _LL290;}
else{ goto _LL291;} _LL291: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_JoinEff: 0){ goto _LL292;} else{ goto _LL246;} _LL248: return Cyc_PP_text(
_tag_arr("[[[array]]]", sizeof( unsigned char), 12u)); _LL250: return Cyc_PP_nil_doc();
_LL252: return Cyc_PP_nil_doc(); _LL254: s= Cyc_PP_text( _tag_arr("void",
sizeof( unsigned char), 5u)); goto _LL246; _LL256: if( _temp297 !=  0){ return
Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp297))->v);}
else{ s=({ struct Cyc_PP_Doc* _temp347[ 6u]; _temp347[ 5u]= _temp299 ==  0? Cyc_PP_text(
_tag_arr("?", sizeof( unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp299))->v); _temp347[ 4u]= Cyc_PP_text(
_tag_arr(")::", sizeof( unsigned char), 4u)); _temp347[ 3u]=( ! Cyc_Absynpp_print_full_evars?
1: _temp293 ==  0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp293))->v);
_temp347[ 2u]= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp349; _temp349.tag= Cyc_Std_Int_pa; _temp349.f1=( int)(( unsigned int)
_temp295);{ void* _temp348[ 1u]={& _temp349}; Cyc_Std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp348, sizeof( void*), 1u));}}));
_temp347[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp347[
0u]= Cyc_PP_text( _tag_arr("%", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp347, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL246; _LL258:
s= Cyc_PP_textptr( _temp301->name); if( Cyc_Absynpp_print_all_kinds){ s=({
struct Cyc_PP_Doc* _temp350[ 3u]; _temp350[ 2u]= Cyc_Absynpp_ckind2doc( _temp301->kind);
_temp350[ 1u]= Cyc_PP_text( _tag_arr("::", sizeof( unsigned char), 3u));
_temp350[ 0u]= s; Cyc_PP_cat( _tag_arr( _temp350, sizeof( struct Cyc_PP_Doc*), 3u));});}
if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar( _temp301): 0){ s=({
struct Cyc_PP_Doc* _temp351[ 3u]; _temp351[ 2u]= Cyc_PP_text( _tag_arr(" */",
sizeof( unsigned char), 4u)); _temp351[ 1u]= s; _temp351[ 0u]= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp351,
sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL260:{ void* _temp352=
_temp309; struct Cyc_Absyn_UnknownTunionInfo _temp358; int _temp360; struct
_tuple0* _temp362; struct Cyc_Absyn_Tuniondecl** _temp364; struct Cyc_Absyn_Tuniondecl*
_temp366; struct Cyc_Absyn_Tuniondecl _temp367; int _temp368; struct _tuple0*
_temp370; _LL354: if(*(( int*) _temp352) ==  Cyc_Absyn_UnknownTunion){ _LL359:
_temp358=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp352)->f1; _LL363:
_temp362= _temp358.name; goto _LL361; _LL361: _temp360= _temp358.is_xtunion;
goto _LL355;} else{ goto _LL356;} _LL356: if(*(( int*) _temp352) ==  Cyc_Absyn_KnownTunion){
_LL365: _temp364=(( struct Cyc_Absyn_KnownTunion_struct*) _temp352)->f1;
_temp366=* _temp364; _temp367=* _temp366; _LL371: _temp370= _temp367.name; goto
_LL369; _LL369: _temp368= _temp367.is_xtunion; goto _LL357;} else{ goto _LL353;}
_LL355: _temp370= _temp362; _temp368= _temp360; goto _LL357; _LL357: { struct
Cyc_PP_Doc* _temp372= Cyc_PP_text( _temp368? _tag_arr("xtunion ", sizeof(
unsigned char), 9u): _tag_arr("tunion ", sizeof( unsigned char), 8u));{ void*
_temp373= Cyc_Tcutil_compress( _temp305); _LL375: if( _temp373 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL376;} else{ goto _LL377;} _LL377: goto _LL378; _LL376: s=({ struct Cyc_PP_Doc*
_temp379[ 3u]; _temp379[ 2u]= Cyc_Absynpp_tps2doc( _temp307); _temp379[ 1u]= Cyc_Absynpp_qvar2doc(
_temp370); _temp379[ 0u]= _temp372; Cyc_PP_cat( _tag_arr( _temp379, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL374; _LL378: s=({ struct Cyc_PP_Doc*
_temp380[ 5u]; _temp380[ 4u]= Cyc_Absynpp_tps2doc( _temp307); _temp380[ 3u]= Cyc_Absynpp_qvar2doc(
_temp370); _temp380[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp380[ 1u]= Cyc_Absynpp_typ2doc( _temp305); _temp380[ 0u]= _temp372; Cyc_PP_cat(
_tag_arr( _temp380, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL374; _LL374:;}
goto _LL353;} _LL353:;} goto _LL246; _LL262:{ void* _temp381= _temp315; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp387; int _temp389; struct _tuple0*
_temp391; struct _tuple0* _temp393; struct Cyc_Absyn_Tunionfield* _temp395;
struct Cyc_Absyn_Tunionfield _temp397; struct _tuple0* _temp398; struct Cyc_Absyn_Tuniondecl*
_temp400; struct Cyc_Absyn_Tuniondecl _temp402; int _temp403; struct _tuple0*
_temp405; _LL383: if(*(( int*) _temp381) ==  Cyc_Absyn_UnknownTunionfield){
_LL388: _temp387=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp381)->f1;
_LL394: _temp393= _temp387.tunion_name; goto _LL392; _LL392: _temp391= _temp387.field_name;
goto _LL390; _LL390: _temp389= _temp387.is_xtunion; goto _LL384;} else{ goto
_LL385;} _LL385: if(*(( int*) _temp381) ==  Cyc_Absyn_KnownTunionfield){ _LL401:
_temp400=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp381)->f1; _temp402=*
_temp400; _LL406: _temp405= _temp402.name; goto _LL404; _LL404: _temp403=
_temp402.is_xtunion; goto _LL396; _LL396: _temp395=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp381)->f2; _temp397=* _temp395; _LL399: _temp398= _temp397.name; goto _LL386;}
else{ goto _LL382;} _LL384: _temp405= _temp393; _temp403= _temp389; _temp398=
_temp391; goto _LL386; _LL386: { struct Cyc_PP_Doc* _temp407= Cyc_PP_text(
_temp403? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp408[ 4u]; _temp408[ 3u]=
Cyc_Absynpp_qvar2doc( _temp398); _temp408[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp408[ 1u]= Cyc_Absynpp_qvar2doc( _temp405);
_temp408[ 0u]= _temp407; Cyc_PP_cat( _tag_arr( _temp408, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL382;} _LL382:;} goto _LL246; _LL264: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp409= _temp319; _LL411: if( _temp409 == ( void*)
Cyc_Absyn_Signed){ goto _LL412;} else{ goto _LL413;} _LL413: if( _temp409 == (
void*) Cyc_Absyn_Unsigned){ goto _LL414;} else{ goto _LL410;} _LL412: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL410; _LL414: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL410; _LL410:;}{ void* _temp415= _temp317;
_LL417: if( _temp415 == ( void*) Cyc_Absyn_B1){ goto _LL418;} else{ goto _LL419;}
_LL419: if( _temp415 == ( void*) Cyc_Absyn_B2){ goto _LL420;} else{ goto _LL421;}
_LL421: if( _temp415 == ( void*) Cyc_Absyn_B4){ goto _LL422;} else{ goto _LL423;}
_LL423: if( _temp415 == ( void*) Cyc_Absyn_B8){ goto _LL424;} else{ goto _LL416;}
_LL418:{ void* _temp425= _temp319; _LL427: if( _temp425 == ( void*) Cyc_Absyn_Signed){
goto _LL428;} else{ goto _LL429;} _LL429: if( _temp425 == ( void*) Cyc_Absyn_Unsigned){
goto _LL430;} else{ goto _LL426;} _LL428: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL426; _LL430: goto _LL426; _LL426:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL416; _LL420: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL416; _LL422: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL416; _LL424: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL416; _LL416:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp433; _temp433.tag= Cyc_Std_String_pa; _temp433.f1=( struct _tagged_arr) ts;{
struct Cyc_Std_String_pa_struct _temp432; _temp432.tag= Cyc_Std_String_pa;
_temp432.f1=( struct _tagged_arr) sns;{ void* _temp431[ 2u]={& _temp432,&
_temp433}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp431, sizeof( void*), 2u));}}})); goto _LL246;} _LL266: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL246; _LL268: s= Cyc_PP_text(
_tag_arr("double", sizeof( unsigned char), 7u)); goto _LL246; _LL270: s=({
struct Cyc_PP_Doc* _temp434[ 2u]; _temp434[ 1u]= Cyc_Absynpp_args2doc( _temp321);
_temp434[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp434, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL272:
if( _temp325 ==  0){ s=({ struct Cyc_PP_Doc* _temp435[ 2u]; _temp435[ 1u]= Cyc_Absynpp_tps2doc(
_temp323); _temp435[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char),
8u)); Cyc_PP_cat( _tag_arr( _temp435, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ s=({ struct Cyc_PP_Doc* _temp436[ 3u]; _temp436[ 2u]= Cyc_Absynpp_tps2doc(
_temp323); _temp436[ 1u]= _temp325 ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp325)); _temp436[ 0u]= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp436, sizeof( struct Cyc_PP_Doc*),
3u));});} goto _LL246; _LL274: if( _temp329 ==  0){ s=({ struct Cyc_PP_Doc*
_temp437[ 2u]; _temp437[ 1u]= Cyc_Absynpp_tps2doc( _temp327); _temp437[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp437,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp438[ 3u];
_temp438[ 2u]= Cyc_Absynpp_tps2doc( _temp327); _temp438[ 1u]= _temp329 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp329));
_temp438[ 0u]= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp438, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL276:
s=({ struct Cyc_PP_Doc* _temp439[ 3u]; _temp439[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp439[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp331)); _temp439[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp439, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL278: s=({ struct Cyc_PP_Doc* _temp440[ 3u]; _temp440[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp440[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp333)); _temp440[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp440,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL280: s=({ struct Cyc_PP_Doc*
_temp441[ 2u]; _temp441[ 1u]= Cyc_Absynpp_qvar2doc( _temp335); _temp441[ 0u]=
Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp441, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL282: s=({
struct Cyc_PP_Doc* _temp442[ 2u]; _temp442[ 1u]= Cyc_Absynpp_tps2doc( _temp339);
_temp442[ 0u]= Cyc_Absynpp_qvar2doc( _temp341); Cyc_PP_cat( _tag_arr( _temp442,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL284: s=({ struct Cyc_PP_Doc*
_temp443[ 3u]; _temp443[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof( unsigned char),
2u)); _temp443[ 1u]= Cyc_Absynpp_rgn2doc( _temp343); _temp443[ 0u]= Cyc_PP_text(
_tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp443, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL286: s= Cyc_Absynpp_rgn2doc(
t); goto _LL246; _LL288: s=({ struct Cyc_PP_Doc* _temp444[ 3u]; _temp444[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp444[ 1u]= Cyc_Absynpp_typ2doc(
_temp345); _temp444[ 0u]= Cyc_PP_text( _tag_arr("regions(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp444, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL290: goto _LL292; _LL292: s= Cyc_Absynpp_eff2doc( t);
goto _LL246; _LL246:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
Cyc_Core_Opt* vo){ return vo ==  0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct _tuple8{
void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc( struct
_tuple8* cmp){ struct _tuple8 _temp447; void* _temp448; void* _temp450; struct
_tuple8* _temp445= cmp; _temp447=* _temp445; _LL451: _temp450= _temp447.f1; goto
_LL449; _LL449: _temp448= _temp447.f2; goto _LL446; _LL446: return({ struct Cyc_PP_Doc*
_temp452[ 3u]; _temp452[ 2u]= Cyc_Absynpp_rgn2doc( _temp448); _temp452[ 1u]= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); _temp452[ 0u]= Cyc_Absynpp_rgn2doc(
_temp450); Cyc_PP_cat( _tag_arr( _temp452, sizeof( struct Cyc_PP_Doc*), 3u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc( struct Cyc_List_List* po){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 ==  0? 0:({ struct Cyc_Core_Opt* _temp453=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp453->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp453;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp454=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp454=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp454,({
struct Cyc_List_List* _temp455=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp455->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp455->tl= 0; _temp455;}));} else{ if(
cyc_varargs !=  0){ struct Cyc_PP_Doc* _temp456=({ struct Cyc_PP_Doc* _temp458[
3u]; _temp458[ 2u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp459=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp459->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp459->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp459->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp459;})); _temp458[ 1u]=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp458[ 0u]= Cyc_PP_text( _tag_arr("...", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp458, sizeof( struct Cyc_PP_Doc*), 3u));}); _temp454=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp454,({
struct Cyc_List_List* _temp457=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp457->hd=( void*) _temp456; _temp457->tl= 0;
_temp457;}));}}{ struct Cyc_PP_Doc* _temp460= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp454); if( effopt !=  0){ _temp460=({ struct
Cyc_PP_Doc* _temp461[ 3u]; _temp461[ 2u]= Cyc_Absynpp_eff2doc(( void*)(( struct
Cyc_Core_Opt*) _check_null( effopt))->v); _temp461[ 1u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp461[ 0u]= _temp460; Cyc_PP_cat( _tag_arr(
_temp461, sizeof( struct Cyc_PP_Doc*), 3u));});} if( rgn_po !=  0){ _temp460=({
struct Cyc_PP_Doc* _temp462[ 3u]; _temp462[ 2u]= Cyc_Absynpp_rgnpo2doc( rgn_po);
_temp462[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char), 2u)); _temp462[
0u]= _temp460; Cyc_PP_cat( _tag_arr( _temp462, sizeof( struct Cyc_PP_Doc*), 3u));});}
return({ struct Cyc_PP_Doc* _temp463[ 3u]; _temp463[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp463[ 1u]= _temp460; _temp463[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp463,
sizeof( struct Cyc_PP_Doc*), 3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp464=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp464->f1=({ struct Cyc_Core_Opt*
_temp465=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp465->v=( void*)(* arg).f1; _temp465;}); _temp464->f2=(* arg).f2; _temp464->f3=(*
arg).f3; _temp464;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp466= 0; int match;{ void*
_temp467=(* q).f1; struct Cyc_List_List* _temp475; struct Cyc_List_List*
_temp477; _LL469: if( _temp467 == ( void*) Cyc_Absyn_Loc_n){ goto _LL470;} else{
goto _LL471;} _LL471: if(( unsigned int) _temp467 >  1u?*(( int*) _temp467) == 
Cyc_Absyn_Rel_n: 0){ _LL476: _temp475=(( struct Cyc_Absyn_Rel_n_struct*)
_temp467)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if(( unsigned int)
_temp467 >  1u?*(( int*) _temp467) ==  Cyc_Absyn_Abs_n: 0){ _LL478: _temp477=((
struct Cyc_Absyn_Abs_n_struct*) _temp467)->f1; goto _LL474;} else{ goto _LL468;}
_LL470: _temp475= 0; goto _LL472; _LL472: match= 0; _temp466= _temp475; goto
_LL468; _LL474: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_Std_strptrcmp, _temp477, Cyc_Absynpp_curr_namespace):
0; _temp466=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp479=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp479->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp479->tl= _temp477;
_temp479;}): _temp477; goto _LL468; _LL468:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp466,({
struct Cyc_List_List* _temp480=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp480->hd=( void*)(* q).f2; _temp480->tl= 0; _temp480;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp466,({ struct Cyc_List_List* _temp481=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp481->hd=( void*)(* q).f2; _temp481->tl= 0;
_temp481;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp482=(* v).f1; struct Cyc_List_List* _temp492; struct Cyc_List_List*
_temp494; _LL484: if( _temp482 == ( void*) Cyc_Absyn_Loc_n){ goto _LL485;} else{
goto _LL486;} _LL486: if(( unsigned int) _temp482 >  1u?*(( int*) _temp482) == 
Cyc_Absyn_Rel_n: 0){ _LL493: _temp492=(( struct Cyc_Absyn_Rel_n_struct*)
_temp482)->f1; if( _temp492 ==  0){ goto _LL487;} else{ goto _LL488;}} else{
goto _LL488;} _LL488: if(( unsigned int) _temp482 >  1u?*(( int*) _temp482) == 
Cyc_Absyn_Abs_n: 0){ _LL495: _temp494=(( struct Cyc_Absyn_Abs_n_struct*)
_temp482)->f1; goto _LL489;} else{ goto _LL490;} _LL490: goto _LL491; _LL485:
goto _LL487; _LL487: return Cyc_Absynpp_var2doc((* v).f2); _LL489: if((( int(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_strptrcmp, _temp494, Cyc_Absynpp_curr_namespace)
==  0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL491;} _LL491:
return({ struct Cyc_PP_Doc* _temp496[ 2u]; _temp496[ 1u]= Cyc_Absynpp_qvar2doc(
v); _temp496[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof(
unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp496, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL483:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp497=(
void*) e->r; void* _temp573; struct Cyc_Absyn_Exp* _temp575; struct Cyc_Absyn_Exp*
_temp577; _LL499: if(*(( int*) _temp497) ==  Cyc_Absyn_Const_e){ goto _LL500;}
else{ goto _LL501;} _LL501: if(*(( int*) _temp497) ==  Cyc_Absyn_Var_e){ goto
_LL502;} else{ goto _LL503;} _LL503: if(*(( int*) _temp497) ==  Cyc_Absyn_UnknownId_e){
goto _LL504;} else{ goto _LL505;} _LL505: if(*(( int*) _temp497) ==  Cyc_Absyn_Primop_e){
_LL574: _temp573=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp497)->f1;
goto _LL506;} else{ goto _LL507;} _LL507: if(*(( int*) _temp497) ==  Cyc_Absyn_AssignOp_e){
goto _LL508;} else{ goto _LL509;} _LL509: if(*(( int*) _temp497) ==  Cyc_Absyn_Increment_e){
goto _LL510;} else{ goto _LL511;} _LL511: if(*(( int*) _temp497) ==  Cyc_Absyn_Conditional_e){
goto _LL512;} else{ goto _LL513;} _LL513: if(*(( int*) _temp497) ==  Cyc_Absyn_SeqExp_e){
goto _LL514;} else{ goto _LL515;} _LL515: if(*(( int*) _temp497) ==  Cyc_Absyn_UnknownCall_e){
goto _LL516;} else{ goto _LL517;} _LL517: if(*(( int*) _temp497) ==  Cyc_Absyn_FnCall_e){
goto _LL518;} else{ goto _LL519;} _LL519: if(*(( int*) _temp497) ==  Cyc_Absyn_Throw_e){
goto _LL520;} else{ goto _LL521;} _LL521: if(*(( int*) _temp497) ==  Cyc_Absyn_NoInstantiate_e){
_LL576: _temp575=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp497)->f1;
goto _LL522;} else{ goto _LL523;} _LL523: if(*(( int*) _temp497) ==  Cyc_Absyn_Instantiate_e){
_LL578: _temp577=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp497)->f1; goto
_LL524;} else{ goto _LL525;} _LL525: if(*(( int*) _temp497) ==  Cyc_Absyn_Cast_e){
goto _LL526;} else{ goto _LL527;} _LL527: if(*(( int*) _temp497) ==  Cyc_Absyn_New_e){
goto _LL528;} else{ goto _LL529;} _LL529: if(*(( int*) _temp497) ==  Cyc_Absyn_Address_e){
goto _LL530;} else{ goto _LL531;} _LL531: if(*(( int*) _temp497) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL532;} else{ goto _LL533;} _LL533: if(*(( int*) _temp497) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL534;} else{ goto _LL535;} _LL535: if(*(( int*) _temp497) ==  Cyc_Absyn_Offsetof_e){
goto _LL536;} else{ goto _LL537;} _LL537: if(*(( int*) _temp497) ==  Cyc_Absyn_Gentyp_e){
goto _LL538;} else{ goto _LL539;} _LL539: if(*(( int*) _temp497) ==  Cyc_Absyn_Deref_e){
goto _LL540;} else{ goto _LL541;} _LL541: if(*(( int*) _temp497) ==  Cyc_Absyn_StructMember_e){
goto _LL542;} else{ goto _LL543;} _LL543: if(*(( int*) _temp497) ==  Cyc_Absyn_StructArrow_e){
goto _LL544;} else{ goto _LL545;} _LL545: if(*(( int*) _temp497) ==  Cyc_Absyn_Subscript_e){
goto _LL546;} else{ goto _LL547;} _LL547: if(*(( int*) _temp497) ==  Cyc_Absyn_Tuple_e){
goto _LL548;} else{ goto _LL549;} _LL549: if(*(( int*) _temp497) ==  Cyc_Absyn_CompoundLit_e){
goto _LL550;} else{ goto _LL551;} _LL551: if(*(( int*) _temp497) ==  Cyc_Absyn_Array_e){
goto _LL552;} else{ goto _LL553;} _LL553: if(*(( int*) _temp497) ==  Cyc_Absyn_Comprehension_e){
goto _LL554;} else{ goto _LL555;} _LL555: if(*(( int*) _temp497) ==  Cyc_Absyn_Struct_e){
goto _LL556;} else{ goto _LL557;} _LL557: if(*(( int*) _temp497) ==  Cyc_Absyn_AnonStruct_e){
goto _LL558;} else{ goto _LL559;} _LL559: if(*(( int*) _temp497) ==  Cyc_Absyn_Tunion_e){
goto _LL560;} else{ goto _LL561;} _LL561: if(*(( int*) _temp497) ==  Cyc_Absyn_Enum_e){
goto _LL562;} else{ goto _LL563;} _LL563: if(*(( int*) _temp497) ==  Cyc_Absyn_Malloc_e){
goto _LL564;} else{ goto _LL565;} _LL565: if(*(( int*) _temp497) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL566;} else{ goto _LL567;} _LL567: if(*(( int*) _temp497) ==  Cyc_Absyn_StmtExp_e){
goto _LL568;} else{ goto _LL569;} _LL569: if(*(( int*) _temp497) ==  Cyc_Absyn_Codegen_e){
goto _LL570;} else{ goto _LL571;} _LL571: if(*(( int*) _temp497) ==  Cyc_Absyn_Fill_e){
goto _LL572;} else{ goto _LL498;} _LL500: return 10000; _LL502: return 10000;
_LL504: return 10000; _LL506: { void* _temp579= _temp573; _LL581: if( _temp579
== ( void*) Cyc_Absyn_Plus){ goto _LL582;} else{ goto _LL583;} _LL583: if(
_temp579 == ( void*) Cyc_Absyn_Times){ goto _LL584;} else{ goto _LL585;} _LL585:
if( _temp579 == ( void*) Cyc_Absyn_Minus){ goto _LL586;} else{ goto _LL587;}
_LL587: if( _temp579 == ( void*) Cyc_Absyn_Div){ goto _LL588;} else{ goto _LL589;}
_LL589: if( _temp579 == ( void*) Cyc_Absyn_Mod){ goto _LL590;} else{ goto _LL591;}
_LL591: if( _temp579 == ( void*) Cyc_Absyn_Eq){ goto _LL592;} else{ goto _LL593;}
_LL593: if( _temp579 == ( void*) Cyc_Absyn_Neq){ goto _LL594;} else{ goto _LL595;}
_LL595: if( _temp579 == ( void*) Cyc_Absyn_Gt){ goto _LL596;} else{ goto _LL597;}
_LL597: if( _temp579 == ( void*) Cyc_Absyn_Lt){ goto _LL598;} else{ goto _LL599;}
_LL599: if( _temp579 == ( void*) Cyc_Absyn_Gte){ goto _LL600;} else{ goto _LL601;}
_LL601: if( _temp579 == ( void*) Cyc_Absyn_Lte){ goto _LL602;} else{ goto _LL603;}
_LL603: if( _temp579 == ( void*) Cyc_Absyn_Not){ goto _LL604;} else{ goto _LL605;}
_LL605: if( _temp579 == ( void*) Cyc_Absyn_Bitnot){ goto _LL606;} else{ goto
_LL607;} _LL607: if( _temp579 == ( void*) Cyc_Absyn_Bitand){ goto _LL608;} else{
goto _LL609;} _LL609: if( _temp579 == ( void*) Cyc_Absyn_Bitor){ goto _LL610;}
else{ goto _LL611;} _LL611: if( _temp579 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL612;} else{ goto _LL613;} _LL613: if( _temp579 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL614;} else{ goto _LL615;} _LL615: if( _temp579 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL616;} else{ goto _LL617;} _LL617: if( _temp579 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL618;} else{ goto _LL619;} _LL619: if( _temp579 == ( void*) Cyc_Absyn_Size){
goto _LL620;} else{ goto _LL580;} _LL582: return 100; _LL584: return 110; _LL586:
return 100; _LL588: return 110; _LL590: return 110; _LL592: return 70; _LL594:
return 70; _LL596: return 80; _LL598: return 80; _LL600: return 80; _LL602:
return 80; _LL604: return 130; _LL606: return 130; _LL608: return 60; _LL610:
return 40; _LL612: return 50; _LL614: return 90; _LL616: return 80; _LL618:
return 80; _LL620: return 140; _LL580:;} _LL508: return 20; _LL510: return 130;
_LL512: return 30; _LL514: return 10; _LL516: return 140; _LL518: return 140;
_LL520: return 130; _LL522: return Cyc_Absynpp_exp_prec( _temp575); _LL524:
return Cyc_Absynpp_exp_prec( _temp577); _LL526: return 120; _LL528: return 15;
_LL530: return 130; _LL532: return 130; _LL534: return 130; _LL536: return 130;
_LL538: return 130; _LL540: return 130; _LL542: return 140; _LL544: return 140;
_LL546: return 140; _LL548: return 150; _LL550: goto _LL552; _LL552: goto _LL554;
_LL554: goto _LL556; _LL556: goto _LL558; _LL558: goto _LL560; _LL560: goto
_LL562; _LL562: goto _LL564; _LL564: return 140; _LL566: return 140; _LL568:
return 10000; _LL570: return 140; _LL572: return 140; _LL498:;} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp621=(
void*) e->r; void* _temp699; struct _tuple0* _temp701; struct _tuple0* _temp703;
struct Cyc_List_List* _temp705; void* _temp707; struct Cyc_Absyn_Exp* _temp709;
struct Cyc_Core_Opt* _temp711; struct Cyc_Absyn_Exp* _temp713; void* _temp715;
struct Cyc_Absyn_Exp* _temp717; struct Cyc_Absyn_Exp* _temp719; struct Cyc_Absyn_Exp*
_temp721; struct Cyc_Absyn_Exp* _temp723; struct Cyc_Absyn_Exp* _temp725; struct
Cyc_Absyn_Exp* _temp727; struct Cyc_List_List* _temp729; struct Cyc_Absyn_Exp*
_temp731; struct Cyc_List_List* _temp733; struct Cyc_Absyn_Exp* _temp735; struct
Cyc_Absyn_Exp* _temp737; struct Cyc_Absyn_Exp* _temp739; struct Cyc_Absyn_Exp*
_temp741; struct Cyc_Absyn_Exp* _temp743; void* _temp745; struct Cyc_Absyn_Exp*
_temp747; struct Cyc_Absyn_Exp* _temp749; struct Cyc_Absyn_Exp* _temp751; void*
_temp753; struct Cyc_Absyn_Exp* _temp755; void* _temp757; struct _tagged_arr*
_temp759; void* _temp761; void* _temp763; unsigned int _temp765; void* _temp767;
void* _temp769; struct Cyc_List_List* _temp771; struct Cyc_Absyn_Exp* _temp773;
struct _tagged_arr* _temp775; struct Cyc_Absyn_Exp* _temp777; struct _tagged_arr*
_temp779; struct Cyc_Absyn_Exp* _temp781; struct Cyc_Absyn_Exp* _temp783; struct
Cyc_Absyn_Exp* _temp785; struct Cyc_List_List* _temp787; struct Cyc_List_List*
_temp789; struct _tuple1* _temp791; struct Cyc_List_List* _temp793; struct Cyc_Absyn_Exp*
_temp795; struct Cyc_Absyn_Exp* _temp797; struct Cyc_Absyn_Vardecl* _temp799;
struct Cyc_List_List* _temp801; struct _tuple0* _temp803; struct Cyc_List_List*
_temp805; struct Cyc_Absyn_Tunionfield* _temp807; struct Cyc_List_List* _temp809;
struct _tuple0* _temp811; void* _temp813; struct Cyc_Absyn_Exp* _temp815; struct
Cyc_List_List* _temp817; struct Cyc_Core_Opt* _temp819; struct Cyc_Absyn_Stmt*
_temp821; struct Cyc_Absyn_Fndecl* _temp823; struct Cyc_Absyn_Exp* _temp825;
_LL623: if(*(( int*) _temp621) ==  Cyc_Absyn_Const_e){ _LL700: _temp699=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp621)->f1; goto _LL624;} else{ goto _LL625;}
_LL625: if(*(( int*) _temp621) ==  Cyc_Absyn_Var_e){ _LL702: _temp701=(( struct
Cyc_Absyn_Var_e_struct*) _temp621)->f1; goto _LL626;} else{ goto _LL627;} _LL627:
if(*(( int*) _temp621) ==  Cyc_Absyn_UnknownId_e){ _LL704: _temp703=(( struct
Cyc_Absyn_UnknownId_e_struct*) _temp621)->f1; goto _LL628;} else{ goto _LL629;}
_LL629: if(*(( int*) _temp621) ==  Cyc_Absyn_Primop_e){ _LL708: _temp707=( void*)((
struct Cyc_Absyn_Primop_e_struct*) _temp621)->f1; goto _LL706; _LL706: _temp705=((
struct Cyc_Absyn_Primop_e_struct*) _temp621)->f2; goto _LL630;} else{ goto
_LL631;} _LL631: if(*(( int*) _temp621) ==  Cyc_Absyn_AssignOp_e){ _LL714:
_temp713=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp621)->f1; goto _LL712;
_LL712: _temp711=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp621)->f2; goto
_LL710; _LL710: _temp709=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp621)->f3;
goto _LL632;} else{ goto _LL633;} _LL633: if(*(( int*) _temp621) ==  Cyc_Absyn_Increment_e){
_LL718: _temp717=(( struct Cyc_Absyn_Increment_e_struct*) _temp621)->f1; goto
_LL716; _LL716: _temp715=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp621)->f2; goto _LL634;} else{ goto _LL635;} _LL635: if(*(( int*) _temp621)
==  Cyc_Absyn_Conditional_e){ _LL724: _temp723=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp621)->f1; goto _LL722; _LL722: _temp721=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp621)->f2; goto _LL720; _LL720: _temp719=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp621)->f3; goto _LL636;} else{ goto _LL637;} _LL637: if(*(( int*) _temp621)
==  Cyc_Absyn_SeqExp_e){ _LL728: _temp727=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp621)->f1; goto _LL726; _LL726: _temp725=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp621)->f2; goto _LL638;} else{ goto _LL639;} _LL639: if(*(( int*) _temp621)
==  Cyc_Absyn_UnknownCall_e){ _LL732: _temp731=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp621)->f1; goto _LL730; _LL730: _temp729=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp621)->f2; goto _LL640;} else{ goto _LL641;} _LL641: if(*(( int*) _temp621)
==  Cyc_Absyn_FnCall_e){ _LL736: _temp735=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp621)->f1; goto _LL734; _LL734: _temp733=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp621)->f2; goto _LL642;} else{ goto _LL643;} _LL643: if(*(( int*) _temp621)
==  Cyc_Absyn_Throw_e){ _LL738: _temp737=(( struct Cyc_Absyn_Throw_e_struct*)
_temp621)->f1; goto _LL644;} else{ goto _LL645;} _LL645: if(*(( int*) _temp621)
==  Cyc_Absyn_NoInstantiate_e){ _LL740: _temp739=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp621)->f1; goto _LL646;} else{ goto _LL647;} _LL647: if(*(( int*) _temp621)
==  Cyc_Absyn_Instantiate_e){ _LL742: _temp741=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp621)->f1; goto _LL648;} else{ goto _LL649;} _LL649: if(*(( int*) _temp621)
==  Cyc_Absyn_Cast_e){ _LL746: _temp745=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp621)->f1; goto _LL744; _LL744: _temp743=(( struct Cyc_Absyn_Cast_e_struct*)
_temp621)->f2; goto _LL650;} else{ goto _LL651;} _LL651: if(*(( int*) _temp621)
==  Cyc_Absyn_Address_e){ _LL748: _temp747=(( struct Cyc_Absyn_Address_e_struct*)
_temp621)->f1; goto _LL652;} else{ goto _LL653;} _LL653: if(*(( int*) _temp621)
==  Cyc_Absyn_New_e){ _LL752: _temp751=(( struct Cyc_Absyn_New_e_struct*)
_temp621)->f1; goto _LL750; _LL750: _temp749=(( struct Cyc_Absyn_New_e_struct*)
_temp621)->f2; goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*) _temp621)
==  Cyc_Absyn_Sizeoftyp_e){ _LL754: _temp753=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp621)->f1; goto _LL656;} else{ goto _LL657;} _LL657: if(*(( int*) _temp621)
==  Cyc_Absyn_Sizeofexp_e){ _LL756: _temp755=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp621)->f1; goto _LL658;} else{ goto _LL659;} _LL659: if(*(( int*) _temp621)
==  Cyc_Absyn_Offsetof_e){ _LL762: _temp761=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp621)->f1; goto _LL758; _LL758: _temp757=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp621)->f2; if(*(( int*) _temp757) ==  Cyc_Absyn_StructField){ _LL760:
_temp759=(( struct Cyc_Absyn_StructField_struct*) _temp757)->f1; goto _LL660;}
else{ goto _LL661;}} else{ goto _LL661;} _LL661: if(*(( int*) _temp621) ==  Cyc_Absyn_Offsetof_e){
_LL768: _temp767=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp621)->f1;
goto _LL764; _LL764: _temp763=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp621)->f2; if(*(( int*) _temp763) ==  Cyc_Absyn_TupleIndex){ _LL766:
_temp765=(( struct Cyc_Absyn_TupleIndex_struct*) _temp763)->f1; goto _LL662;}
else{ goto _LL663;}} else{ goto _LL663;} _LL663: if(*(( int*) _temp621) ==  Cyc_Absyn_Gentyp_e){
_LL772: _temp771=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp621)->f1; goto
_LL770; _LL770: _temp769=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp621)->f2;
goto _LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp621) ==  Cyc_Absyn_Deref_e){
_LL774: _temp773=(( struct Cyc_Absyn_Deref_e_struct*) _temp621)->f1; goto _LL666;}
else{ goto _LL667;} _LL667: if(*(( int*) _temp621) ==  Cyc_Absyn_StructMember_e){
_LL778: _temp777=(( struct Cyc_Absyn_StructMember_e_struct*) _temp621)->f1; goto
_LL776; _LL776: _temp775=(( struct Cyc_Absyn_StructMember_e_struct*) _temp621)->f2;
goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp621) ==  Cyc_Absyn_StructArrow_e){
_LL782: _temp781=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp621)->f1; goto
_LL780; _LL780: _temp779=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp621)->f2;
goto _LL670;} else{ goto _LL671;} _LL671: if(*(( int*) _temp621) ==  Cyc_Absyn_Subscript_e){
_LL786: _temp785=(( struct Cyc_Absyn_Subscript_e_struct*) _temp621)->f1; goto
_LL784; _LL784: _temp783=(( struct Cyc_Absyn_Subscript_e_struct*) _temp621)->f2;
goto _LL672;} else{ goto _LL673;} _LL673: if(*(( int*) _temp621) ==  Cyc_Absyn_Tuple_e){
_LL788: _temp787=(( struct Cyc_Absyn_Tuple_e_struct*) _temp621)->f1; goto _LL674;}
else{ goto _LL675;} _LL675: if(*(( int*) _temp621) ==  Cyc_Absyn_CompoundLit_e){
_LL792: _temp791=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp621)->f1; goto
_LL790; _LL790: _temp789=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp621)->f2;
goto _LL676;} else{ goto _LL677;} _LL677: if(*(( int*) _temp621) ==  Cyc_Absyn_Array_e){
_LL794: _temp793=(( struct Cyc_Absyn_Array_e_struct*) _temp621)->f1; goto _LL678;}
else{ goto _LL679;} _LL679: if(*(( int*) _temp621) ==  Cyc_Absyn_Comprehension_e){
_LL800: _temp799=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp621)->f1;
goto _LL798; _LL798: _temp797=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp621)->f2; goto _LL796; _LL796: _temp795=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp621)->f3; goto _LL680;} else{ goto _LL681;} _LL681: if(*(( int*) _temp621)
==  Cyc_Absyn_Struct_e){ _LL804: _temp803=(( struct Cyc_Absyn_Struct_e_struct*)
_temp621)->f1; goto _LL802; _LL802: _temp801=(( struct Cyc_Absyn_Struct_e_struct*)
_temp621)->f3; goto _LL682;} else{ goto _LL683;} _LL683: if(*(( int*) _temp621)
==  Cyc_Absyn_AnonStruct_e){ _LL806: _temp805=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp621)->f2; goto _LL684;} else{ goto _LL685;} _LL685: if(*(( int*) _temp621)
==  Cyc_Absyn_Tunion_e){ _LL810: _temp809=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp621)->f3; goto _LL808; _LL808: _temp807=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp621)->f5; goto _LL686;} else{ goto _LL687;} _LL687: if(*(( int*) _temp621)
==  Cyc_Absyn_Enum_e){ _LL812: _temp811=(( struct Cyc_Absyn_Enum_e_struct*)
_temp621)->f1; goto _LL688;} else{ goto _LL689;} _LL689: if(*(( int*) _temp621)
==  Cyc_Absyn_Malloc_e){ _LL816: _temp815=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp621)->f1; goto _LL814; _LL814: _temp813=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp621)->f2; goto _LL690;} else{ goto _LL691;} _LL691: if(*(( int*) _temp621)
==  Cyc_Absyn_UnresolvedMem_e){ _LL820: _temp819=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp621)->f1; goto _LL818; _LL818: _temp817=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp621)->f2; goto _LL692;} else{ goto _LL693;} _LL693: if(*(( int*) _temp621)
==  Cyc_Absyn_StmtExp_e){ _LL822: _temp821=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp621)->f1; goto _LL694;} else{ goto _LL695;} _LL695: if(*(( int*) _temp621)
==  Cyc_Absyn_Codegen_e){ _LL824: _temp823=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp621)->f1; goto _LL696;} else{ goto _LL697;} _LL697: if(*(( int*) _temp621)
==  Cyc_Absyn_Fill_e){ _LL826: _temp825=(( struct Cyc_Absyn_Fill_e_struct*)
_temp621)->f1; goto _LL698;} else{ goto _LL622;} _LL624: s= Cyc_Absynpp_cnst2doc(
_temp699); goto _LL622; _LL626: _temp703= _temp701; goto _LL628; _LL628: s= Cyc_Absynpp_qvar2doc(
_temp703); goto _LL622; _LL630: s= Cyc_Absynpp_primapp2doc( myprec, _temp707,
_temp705); goto _LL622; _LL632: s=({ struct Cyc_PP_Doc* _temp827[ 5u]; _temp827[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp709); _temp827[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp827[ 2u]= _temp711 ==  0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp711))->v); _temp827[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp827[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp713); Cyc_PP_cat( _tag_arr( _temp827, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL622; _LL634: { struct Cyc_PP_Doc* _temp828= Cyc_Absynpp_exp2doc_prec(
myprec, _temp717);{ void* _temp829= _temp715; _LL831: if( _temp829 == ( void*)
Cyc_Absyn_PreInc){ goto _LL832;} else{ goto _LL833;} _LL833: if( _temp829 == (
void*) Cyc_Absyn_PreDec){ goto _LL834;} else{ goto _LL835;} _LL835: if( _temp829
== ( void*) Cyc_Absyn_PostInc){ goto _LL836;} else{ goto _LL837;} _LL837: if(
_temp829 == ( void*) Cyc_Absyn_PostDec){ goto _LL838;} else{ goto _LL830;}
_LL832: s=({ struct Cyc_PP_Doc* _temp839[ 2u]; _temp839[ 1u]= _temp828; _temp839[
0u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp839, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL830; _LL834: s=({
struct Cyc_PP_Doc* _temp840[ 2u]; _temp840[ 1u]= _temp828; _temp840[ 0u]= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp840,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL830; _LL836: s=({ struct Cyc_PP_Doc*
_temp841[ 2u]; _temp841[ 1u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char),
3u)); _temp841[ 0u]= _temp828; Cyc_PP_cat( _tag_arr( _temp841, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL830; _LL838: s=({ struct Cyc_PP_Doc* _temp842[ 2u];
_temp842[ 1u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
_temp842[ 0u]= _temp828; Cyc_PP_cat( _tag_arr( _temp842, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL830; _LL830:;} goto _LL622;} _LL636:{ struct _tuple8 _temp844=({
struct _tuple8 _temp843; _temp843.f1=( void*) _temp721->r; _temp843.f2=( void*)
_temp719->r; _temp843;}); _LL846: goto _LL847; _LL847: s=({ struct Cyc_PP_Doc*
_temp848[ 5u]; _temp848[ 4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp719);
_temp848[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof( unsigned char), 4u));
_temp848[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp721); _temp848[ 1u]= Cyc_PP_text(
_tag_arr(" ? ", sizeof( unsigned char), 4u)); _temp848[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp723); Cyc_PP_cat( _tag_arr( _temp848, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL845; _LL845:;} goto _LL622; _LL638: s=({ struct Cyc_PP_Doc*
_temp849[ 5u]; _temp849[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp849[ 3u]= Cyc_Absynpp_exp2doc( _temp725); _temp849[ 2u]= Cyc_PP_text(
_tag_arr(", ", sizeof( unsigned char), 3u)); _temp849[ 1u]= Cyc_Absynpp_exp2doc(
_temp727); _temp849[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp849, sizeof( struct Cyc_PP_Doc*), 5u));}); goto
_LL622; _LL640: s=({ struct Cyc_PP_Doc* _temp850[ 4u]; _temp850[ 3u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp850[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp729); _temp850[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp850[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp731); Cyc_PP_cat(
_tag_arr( _temp850, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL622; _LL642: s=({
struct Cyc_PP_Doc* _temp851[ 4u]; _temp851[ 3u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp851[ 2u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp733); _temp851[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
_temp851[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp735); Cyc_PP_cat( _tag_arr(
_temp851, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL622; _LL644: s=({ struct
Cyc_PP_Doc* _temp852[ 2u]; _temp852[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp737); _temp852[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char),
7u)); Cyc_PP_cat( _tag_arr( _temp852, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL622; _LL646: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp739); goto _LL622;
_LL648: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp741); goto _LL622; _LL650: s=({
struct Cyc_PP_Doc* _temp853[ 4u]; _temp853[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp743); _temp853[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp853[ 1u]= Cyc_Absynpp_typ2doc( _temp745); _temp853[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp853, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL622; _LL652: s=({ struct
Cyc_PP_Doc* _temp854[ 2u]; _temp854[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp747); _temp854[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp854, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL622; _LL654: if( _temp751 ==  0){ s=({ struct Cyc_PP_Doc* _temp855[ 2u];
_temp855[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp749); _temp855[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp855,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp856[ 4u];
_temp856[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp749); _temp856[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp856[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp751)); _temp856[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp856,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL622; _LL656: s=({ struct Cyc_PP_Doc*
_temp857[ 3u]; _temp857[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp857[ 1u]= Cyc_Absynpp_typ2doc( _temp753); _temp857[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp857,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL622; _LL658: s=({ struct Cyc_PP_Doc*
_temp858[ 3u]; _temp858[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp858[ 1u]= Cyc_Absynpp_exp2doc( _temp755); _temp858[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp858,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL622; _LL660: s=({ struct Cyc_PP_Doc*
_temp859[ 5u]; _temp859[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp859[ 3u]= Cyc_PP_textptr( _temp759); _temp859[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp859[ 1u]= Cyc_Absynpp_typ2doc(
_temp761); _temp859[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp859, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL622; _LL662: s=({ struct Cyc_PP_Doc* _temp860[ 5u]; _temp860[ 4u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp860[ 3u]= Cyc_PP_text((
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp862; _temp862.tag= Cyc_Std_Int_pa;
_temp862.f1=( int) _temp765;{ void* _temp861[ 1u]={& _temp862}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp861, sizeof( void*),
1u));}})); _temp860[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp860[ 1u]= Cyc_Absynpp_typ2doc( _temp767); _temp860[ 0u]= Cyc_PP_text(
_tag_arr("offsetof(", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp860, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL622; _LL664: s=({ struct
Cyc_PP_Doc* _temp863[ 4u]; _temp863[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp863[ 2u]= Cyc_Absynpp_typ2doc( _temp769); _temp863[ 1u]=
Cyc_Absynpp_tvars2doc( _temp771); _temp863[ 0u]= Cyc_PP_text( _tag_arr("__gen(",
sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp863, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL622; _LL666: s=({ struct Cyc_PP_Doc* _temp864[ 2u]; _temp864[ 1u]=
Cyc_Absynpp_exp2doc_prec( myprec, _temp773); _temp864[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp864,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL622; _LL668: s=({ struct Cyc_PP_Doc*
_temp865[ 3u]; _temp865[ 2u]= Cyc_PP_textptr( _temp775); _temp865[ 1u]= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); _temp865[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp777); Cyc_PP_cat( _tag_arr( _temp865, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL622; _LL670: s=({ struct Cyc_PP_Doc* _temp866[ 3u]; _temp866[ 2u]=
Cyc_PP_textptr( _temp779); _temp866[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof(
unsigned char), 3u)); _temp866[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp781);
Cyc_PP_cat( _tag_arr( _temp866, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL622; _LL672: s=({ struct Cyc_PP_Doc* _temp867[ 4u]; _temp867[ 3u]= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); _temp867[ 2u]= Cyc_Absynpp_exp2doc(
_temp783); _temp867[ 1u]= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u));
_temp867[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp785); Cyc_PP_cat( _tag_arr(
_temp867, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL622; _LL674: s=({ struct
Cyc_PP_Doc* _temp868[ 3u]; _temp868[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp868[ 1u]= Cyc_Absynpp_exps2doc_prec( 20, _temp787);
_temp868[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp868, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL622; _LL676: s=({
struct Cyc_PP_Doc* _temp869[ 4u]; _temp869[ 3u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp789)); _temp869[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp869[ 1u]= Cyc_Absynpp_typ2doc((* _temp791).f3); _temp869[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp869,
sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL622; _LL678: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp793)); goto _LL622; _LL680: s=({ struct Cyc_PP_Doc*
_temp870[ 7u]; _temp870[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp870[ 5u]= Cyc_Absynpp_exp2doc( _temp795); _temp870[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp870[ 3u]= Cyc_Absynpp_exp2doc(
_temp797); _temp870[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp870[ 1u]= Cyc_PP_text(*(* _temp799->name).f2); _temp870[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp870,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL622; _LL682: s=({ struct Cyc_PP_Doc*
_temp871[ 2u]; _temp871[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp801));
_temp871[ 0u]= Cyc_Absynpp_qvar2doc( _temp803); Cyc_PP_cat( _tag_arr( _temp871,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL622; _LL684: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp805)); goto _LL622; _LL686: if( _temp809 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp807->name);} else{ s=({ struct Cyc_PP_Doc* _temp872[ 2u]; _temp872[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp809)); _temp872[ 0u]= Cyc_Absynpp_qvar2doc( _temp807->name);
Cyc_PP_cat( _tag_arr( _temp872, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL622; _LL688: s= Cyc_Absynpp_qvar2doc( _temp811); goto _LL622; _LL690: if(
_temp815 ==  0){ s=({ struct Cyc_PP_Doc* _temp873[ 3u]; _temp873[ 2u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp873[ 1u]= Cyc_Absynpp_typ2doc(
_temp813); _temp873[ 0u]= Cyc_PP_text( _tag_arr("malloc(sizeof(", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp873, sizeof( struct Cyc_PP_Doc*),
3u));});} else{ s=({ struct Cyc_PP_Doc* _temp874[ 6u]; _temp874[ 5u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp874[ 4u]= Cyc_Absynpp_typ2doc(
_temp813); _temp874[ 3u]= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char),
8u)); _temp874[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp874[ 1u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp815)); _temp874[ 0u]= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp874, sizeof( struct Cyc_PP_Doc*),
6u));});} goto _LL622; _LL692: s= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp817)); goto _LL622; _LL694: s=({ struct Cyc_PP_Doc* _temp875[ 3u]; _temp875[
2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char), 4u)); _temp875[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp821)); _temp875[ 0u]= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat( _tag_arr( _temp875,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL622; _LL696: s=({ struct Cyc_PP_Doc*
_temp876[ 3u]; _temp876[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp876[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl*
_temp877=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp877->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp878=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp878[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp879; _temp879.tag= Cyc_Absyn_Fn_d;
_temp879.f1= _temp823; _temp879;}); _temp878;})); _temp877->loc= e->loc;
_temp877;}))); _temp876[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp876, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL622; _LL698: s=({ struct Cyc_PP_Doc* _temp880[ 3u]; _temp880[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp880[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp825)); _temp880[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp880, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL622; _LL622:;} if( inprec >=  myprec){ s=({ struct Cyc_PP_Doc*
_temp881[ 3u]; _temp881[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp881[ 1u]= s; _temp881[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp881, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp882= d; struct Cyc_Absyn_Exp* _temp888; struct _tagged_arr* _temp890;
_LL884: if(*(( int*) _temp882) ==  Cyc_Absyn_ArrayElement){ _LL889: _temp888=((
struct Cyc_Absyn_ArrayElement_struct*) _temp882)->f1; goto _LL885;} else{ goto
_LL886;} _LL886: if(*(( int*) _temp882) ==  Cyc_Absyn_FieldName){ _LL891:
_temp890=(( struct Cyc_Absyn_FieldName_struct*) _temp882)->f1; goto _LL887;}
else{ goto _LL883;} _LL885: return({ struct Cyc_PP_Doc* _temp892[ 3u]; _temp892[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp892[ 1u]= Cyc_Absynpp_exp2doc(
_temp888); _temp892[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp892, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL887:
return({ struct Cyc_PP_Doc* _temp893[ 2u]; _temp893[ 1u]= Cyc_PP_textptr(
_temp890); _temp893[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp893, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL883:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 ==  0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp894[ 2u]; _temp894[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp894[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp894, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp895= c;
unsigned char _temp913; void* _temp915; short _temp917; void* _temp919; int
_temp921; void* _temp923; int _temp925; void* _temp927; long long _temp929; void*
_temp931; struct _tagged_arr _temp933; struct _tagged_arr _temp935; _LL897: if((
unsigned int) _temp895 >  1u?*(( int*) _temp895) ==  Cyc_Absyn_Char_c: 0){
_LL916: _temp915=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp895)->f1; goto
_LL914; _LL914: _temp913=(( struct Cyc_Absyn_Char_c_struct*) _temp895)->f2; goto
_LL898;} else{ goto _LL899;} _LL899: if(( unsigned int) _temp895 >  1u?*(( int*)
_temp895) ==  Cyc_Absyn_Short_c: 0){ _LL920: _temp919=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp895)->f1; goto _LL918; _LL918: _temp917=(( struct Cyc_Absyn_Short_c_struct*)
_temp895)->f2; goto _LL900;} else{ goto _LL901;} _LL901: if(( unsigned int)
_temp895 >  1u?*(( int*) _temp895) ==  Cyc_Absyn_Int_c: 0){ _LL924: _temp923=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp895)->f1; if( _temp923 == ( void*)
Cyc_Absyn_Signed){ goto _LL922;} else{ goto _LL903;} _LL922: _temp921=(( struct
Cyc_Absyn_Int_c_struct*) _temp895)->f2; goto _LL902;} else{ goto _LL903;} _LL903:
if(( unsigned int) _temp895 >  1u?*(( int*) _temp895) ==  Cyc_Absyn_Int_c: 0){
_LL928: _temp927=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp895)->f1; if(
_temp927 == ( void*) Cyc_Absyn_Unsigned){ goto _LL926;} else{ goto _LL905;}
_LL926: _temp925=(( struct Cyc_Absyn_Int_c_struct*) _temp895)->f2; goto _LL904;}
else{ goto _LL905;} _LL905: if(( unsigned int) _temp895 >  1u?*(( int*) _temp895)
==  Cyc_Absyn_LongLong_c: 0){ _LL932: _temp931=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp895)->f1; goto _LL930; _LL930: _temp929=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp895)->f2; goto _LL906;} else{ goto _LL907;} _LL907: if(( unsigned int)
_temp895 >  1u?*(( int*) _temp895) ==  Cyc_Absyn_Float_c: 0){ _LL934: _temp933=((
struct Cyc_Absyn_Float_c_struct*) _temp895)->f1; goto _LL908;} else{ goto _LL909;}
_LL909: if( _temp895 == ( void*) Cyc_Absyn_Null_c){ goto _LL910;} else{ goto
_LL911;} _LL911: if(( unsigned int) _temp895 >  1u?*(( int*) _temp895) ==  Cyc_Absyn_String_c:
0){ _LL936: _temp935=(( struct Cyc_Absyn_String_c_struct*) _temp895)->f1; goto
_LL912;} else{ goto _LL896;} _LL898: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp938; _temp938.tag= Cyc_Std_String_pa;
_temp938.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp913);{ void*
_temp937[ 1u]={& _temp938}; Cyc_Std_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp937, sizeof( void*), 1u));}})); _LL900:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp940;
_temp940.tag= Cyc_Std_Int_pa; _temp940.f1=( int)(( unsigned int)(( int) _temp917));{
void* _temp939[ 1u]={& _temp940}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp939, sizeof( void*), 1u));}})); _LL902:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp942;
_temp942.tag= Cyc_Std_Int_pa; _temp942.f1=( int)(( unsigned int) _temp921);{
void* _temp941[ 1u]={& _temp942}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp941, sizeof( void*), 1u));}})); _LL904:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp944;
_temp944.tag= Cyc_Std_Int_pa; _temp944.f1=( unsigned int) _temp925;{ void*
_temp943[ 1u]={& _temp944}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp943, sizeof( void*), 1u));}})); _LL906:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL908: return Cyc_PP_text( _temp933); _LL910: return Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); _LL912: return({ struct Cyc_PP_Doc*
_temp945[ 3u]; _temp945[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp945[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp935));
_temp945[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp945, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL896:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == ( void*) Cyc_Absyn_Size){ if(
es ==  0? 1:(( struct Cyc_List_List*) _check_null( es))->tl !=  0){( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp946=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp946[ 0]=({ struct Cyc_Core_Failure_struct
_temp947; _temp947.tag= Cyc_Core_Failure; _temp947.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp949; _temp949.tag= Cyc_Std_String_pa;
_temp949.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp948[
1u]={& _temp949}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp948, sizeof( void*), 1u));}});
_temp947;}); _temp946;}));}{ struct Cyc_PP_Doc* _temp950= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
return({ struct Cyc_PP_Doc* _temp951[ 2u]; _temp951[ 1u]= Cyc_PP_text( _tag_arr(".size",
sizeof( unsigned char), 6u)); _temp951[ 0u]= _temp950; Cyc_PP_cat( _tag_arr(
_temp951, sizeof( struct Cyc_PP_Doc*), 2u));});}} else{ struct Cyc_List_List* ds=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds ==  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp952=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp952[ 0]=({ struct Cyc_Core_Failure_struct _temp953; _temp953.tag= Cyc_Core_Failure;
_temp953.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp955;
_temp955.tag= Cyc_Std_String_pa; _temp955.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp954[ 1u]={& _temp955}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp954, sizeof( void*), 1u));}});
_temp953;}); _temp952;}));} else{ if((( struct Cyc_List_List*) _check_null( ds))->tl
==  0){ return({ struct Cyc_PP_Doc* _temp956[ 3u]; _temp956[ 2u]=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; _temp956[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp956[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp956, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl
!=  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp957=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp957[ 0]=({ struct Cyc_Core_Failure_struct _temp958; _temp958.tag= Cyc_Core_Failure;
_temp958.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp960;
_temp960.tag= Cyc_Std_String_pa; _temp960.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp959[ 1u]={& _temp960}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp959, sizeof( void*), 1u));}});
_temp958;}); _temp957;}));} else{ return({ struct Cyc_PP_Doc* _temp961[ 5u];
_temp961[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->hd; _temp961[ 3u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp961[ 2u]= ps; _temp961[ 1u]=
Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp961[ 0u]=( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd; Cyc_PP_cat(
_tag_arr( _temp961, sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct
_tagged_arr Cyc_Absynpp_prim2str( void* p){ void* _temp962= p; _LL964: if(
_temp962 == ( void*) Cyc_Absyn_Plus){ goto _LL965;} else{ goto _LL966;} _LL966:
if( _temp962 == ( void*) Cyc_Absyn_Times){ goto _LL967;} else{ goto _LL968;}
_LL968: if( _temp962 == ( void*) Cyc_Absyn_Minus){ goto _LL969;} else{ goto
_LL970;} _LL970: if( _temp962 == ( void*) Cyc_Absyn_Div){ goto _LL971;} else{
goto _LL972;} _LL972: if( _temp962 == ( void*) Cyc_Absyn_Mod){ goto _LL973;}
else{ goto _LL974;} _LL974: if( _temp962 == ( void*) Cyc_Absyn_Eq){ goto _LL975;}
else{ goto _LL976;} _LL976: if( _temp962 == ( void*) Cyc_Absyn_Neq){ goto _LL977;}
else{ goto _LL978;} _LL978: if( _temp962 == ( void*) Cyc_Absyn_Gt){ goto _LL979;}
else{ goto _LL980;} _LL980: if( _temp962 == ( void*) Cyc_Absyn_Lt){ goto _LL981;}
else{ goto _LL982;} _LL982: if( _temp962 == ( void*) Cyc_Absyn_Gte){ goto _LL983;}
else{ goto _LL984;} _LL984: if( _temp962 == ( void*) Cyc_Absyn_Lte){ goto _LL985;}
else{ goto _LL986;} _LL986: if( _temp962 == ( void*) Cyc_Absyn_Not){ goto _LL987;}
else{ goto _LL988;} _LL988: if( _temp962 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL989;} else{ goto _LL990;} _LL990: if( _temp962 == ( void*) Cyc_Absyn_Bitand){
goto _LL991;} else{ goto _LL992;} _LL992: if( _temp962 == ( void*) Cyc_Absyn_Bitor){
goto _LL993;} else{ goto _LL994;} _LL994: if( _temp962 == ( void*) Cyc_Absyn_Bitxor){
goto _LL995;} else{ goto _LL996;} _LL996: if( _temp962 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL997;} else{ goto _LL998;} _LL998: if( _temp962 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL999;} else{ goto _LL1000;} _LL1000: if( _temp962 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL1001;} else{ goto _LL1002;} _LL1002: if( _temp962 == ( void*) Cyc_Absyn_Size){
goto _LL1003;} else{ goto _LL963;} _LL965: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL967: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL969: return _tag_arr("-", sizeof( unsigned char), 2u); _LL971: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL973: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL975: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL977: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL979: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL981: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL983: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL985: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL987: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL989: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL991: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL993: return _tag_arr("|", sizeof( unsigned char), 2u); _LL995: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL997: return _tag_arr("<<", sizeof(
unsigned char), 3u); _LL999: return _tag_arr(">>", sizeof( unsigned char), 3u);
_LL1001: return _tag_arr(">>>", sizeof( unsigned char), 4u); _LL1003: return
_tag_arr("size", sizeof( unsigned char), 5u); _LL963:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc(
void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc(
struct _tuple9* t){ if((* t).f1 ==  0){ return Cyc_Absynpp_exp2doc((* t).f2);}
else{ return({ struct Cyc_PP_Doc* _temp1004[ 3u]; _temp1004[ 2u]= Cyc_Absynpp_exp2doc((*
t).f2); _temp1004[ 1u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1004[ 0u]= Cyc_PP_textptr(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v); Cyc_PP_cat( _tag_arr( _temp1004, sizeof( struct Cyc_PP_Doc*),
3u));});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp1005=( void*) s->r; _LL1007: if(( unsigned int) _temp1005 >  1u?*(( int*)
_temp1005) ==  Cyc_Absyn_Decl_s: 0){ goto _LL1008;} else{ goto _LL1009;} _LL1009:
goto _LL1010; _LL1008: return 1; _LL1010: return 0; _LL1006:;} struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void*
_temp1011=( void*) st->r; struct Cyc_Absyn_Exp* _temp1055; struct Cyc_Absyn_Stmt*
_temp1057; struct Cyc_Absyn_Stmt* _temp1059; struct Cyc_Absyn_Exp* _temp1061;
struct Cyc_Absyn_Stmt* _temp1063; struct Cyc_Absyn_Stmt* _temp1065; struct Cyc_Absyn_Exp*
_temp1067; struct Cyc_Absyn_Stmt* _temp1069; struct _tuple2 _temp1071; struct
Cyc_Absyn_Exp* _temp1073; struct _tagged_arr* _temp1075; struct Cyc_Absyn_Stmt*
_temp1077; struct _tuple2 _temp1079; struct Cyc_Absyn_Exp* _temp1081; struct
_tuple2 _temp1083; struct Cyc_Absyn_Exp* _temp1085; struct Cyc_Absyn_Exp*
_temp1087; struct Cyc_List_List* _temp1089; struct Cyc_Absyn_Exp* _temp1091;
struct Cyc_List_List* _temp1093; struct Cyc_Absyn_Exp* _temp1095; struct Cyc_List_List*
_temp1097; struct Cyc_List_List* _temp1099; struct Cyc_Absyn_Stmt* _temp1101;
struct Cyc_Absyn_Decl* _temp1103; struct Cyc_Absyn_Stmt* _temp1105; struct Cyc_Absyn_Stmt*
_temp1107; struct Cyc_Absyn_Stmt* _temp1109; struct _tagged_arr* _temp1111;
struct _tuple2 _temp1113; struct Cyc_Absyn_Exp* _temp1115; struct Cyc_Absyn_Stmt*
_temp1117; struct Cyc_List_List* _temp1119; struct Cyc_Absyn_Stmt* _temp1121;
struct Cyc_Absyn_Stmt* _temp1123; struct Cyc_Absyn_Vardecl* _temp1125; struct
Cyc_Absyn_Tvar* _temp1127; _LL1013: if( _temp1011 == ( void*) Cyc_Absyn_Skip_s){
goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int) _temp1011 >  1u?*((
int*) _temp1011) ==  Cyc_Absyn_Exp_s: 0){ _LL1056: _temp1055=(( struct Cyc_Absyn_Exp_s_struct*)
_temp1011)->f1; goto _LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Seq_s: 0){ _LL1060: _temp1059=((
struct Cyc_Absyn_Seq_s_struct*) _temp1011)->f1; goto _LL1058; _LL1058: _temp1057=((
struct Cyc_Absyn_Seq_s_struct*) _temp1011)->f2; goto _LL1018;} else{ goto
_LL1019;} _LL1019: if(( unsigned int) _temp1011 >  1u?*(( int*) _temp1011) == 
Cyc_Absyn_Return_s: 0){ _LL1062: _temp1061=(( struct Cyc_Absyn_Return_s_struct*)
_temp1011)->f1; goto _LL1020;} else{ goto _LL1021;} _LL1021: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1068:
_temp1067=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1011)->f1; goto _LL1066;
_LL1066: _temp1065=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1011)->f2;
goto _LL1064; _LL1064: _temp1063=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1011)->f3; goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_While_s: 0){ _LL1072:
_temp1071=(( struct Cyc_Absyn_While_s_struct*) _temp1011)->f1; _LL1074:
_temp1073= _temp1071.f1; goto _LL1070; _LL1070: _temp1069=(( struct Cyc_Absyn_While_s_struct*)
_temp1011)->f2; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Break_s: 0){ goto _LL1026;}
else{ goto _LL1027;} _LL1027: if(( unsigned int) _temp1011 >  1u?*(( int*)
_temp1011) ==  Cyc_Absyn_Continue_s: 0){ goto _LL1028;} else{ goto _LL1029;}
_LL1029: if(( unsigned int) _temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Goto_s:
0){ _LL1076: _temp1075=(( struct Cyc_Absyn_Goto_s_struct*) _temp1011)->f1; goto
_LL1030;} else{ goto _LL1031;} _LL1031: if(( unsigned int) _temp1011 >  1u?*((
int*) _temp1011) ==  Cyc_Absyn_For_s: 0){ _LL1088: _temp1087=(( struct Cyc_Absyn_For_s_struct*)
_temp1011)->f1; goto _LL1084; _LL1084: _temp1083=(( struct Cyc_Absyn_For_s_struct*)
_temp1011)->f2; _LL1086: _temp1085= _temp1083.f1; goto _LL1080; _LL1080:
_temp1079=(( struct Cyc_Absyn_For_s_struct*) _temp1011)->f3; _LL1082: _temp1081=
_temp1079.f1; goto _LL1078; _LL1078: _temp1077=(( struct Cyc_Absyn_For_s_struct*)
_temp1011)->f4; goto _LL1032;} else{ goto _LL1033;} _LL1033: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Switch_s: 0){ _LL1092:
_temp1091=(( struct Cyc_Absyn_Switch_s_struct*) _temp1011)->f1; goto _LL1090;
_LL1090: _temp1089=(( struct Cyc_Absyn_Switch_s_struct*) _temp1011)->f2; goto
_LL1034;} else{ goto _LL1035;} _LL1035: if(( unsigned int) _temp1011 >  1u?*((
int*) _temp1011) ==  Cyc_Absyn_SwitchC_s: 0){ _LL1096: _temp1095=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1011)->f1; goto _LL1094; _LL1094: _temp1093=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1011)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1098:
_temp1097=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1011)->f1; if( _temp1097
==  0){ goto _LL1038;} else{ goto _LL1039;}} else{ goto _LL1039;} _LL1039: if((
unsigned int) _temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Fallthru_s: 0){
_LL1100: _temp1099=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1011)->f1; goto
_LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int) _temp1011 >  1u?*((
int*) _temp1011) ==  Cyc_Absyn_Decl_s: 0){ _LL1104: _temp1103=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1011)->f1; goto _LL1102; _LL1102: _temp1101=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1011)->f2; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Cut_s: 0){ _LL1106: _temp1105=((
struct Cyc_Absyn_Cut_s_struct*) _temp1011)->f1; goto _LL1044;} else{ goto
_LL1045;} _LL1045: if(( unsigned int) _temp1011 >  1u?*(( int*) _temp1011) == 
Cyc_Absyn_Splice_s: 0){ _LL1108: _temp1107=(( struct Cyc_Absyn_Splice_s_struct*)
_temp1011)->f1; goto _LL1046;} else{ goto _LL1047;} _LL1047: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Label_s: 0){ _LL1112:
_temp1111=(( struct Cyc_Absyn_Label_s_struct*) _temp1011)->f1; goto _LL1110;
_LL1110: _temp1109=(( struct Cyc_Absyn_Label_s_struct*) _temp1011)->f2; goto
_LL1048;} else{ goto _LL1049;} _LL1049: if(( unsigned int) _temp1011 >  1u?*((
int*) _temp1011) ==  Cyc_Absyn_Do_s: 0){ _LL1118: _temp1117=(( struct Cyc_Absyn_Do_s_struct*)
_temp1011)->f1; goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_Do_s_struct*)
_temp1011)->f2; _LL1116: _temp1115= _temp1113.f1; goto _LL1050;} else{ goto
_LL1051;} _LL1051: if(( unsigned int) _temp1011 >  1u?*(( int*) _temp1011) == 
Cyc_Absyn_TryCatch_s: 0){ _LL1122: _temp1121=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1011)->f1; goto _LL1120; _LL1120: _temp1119=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1011)->f2; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(( unsigned int)
_temp1011 >  1u?*(( int*) _temp1011) ==  Cyc_Absyn_Region_s: 0){ _LL1128:
_temp1127=(( struct Cyc_Absyn_Region_s_struct*) _temp1011)->f1; goto _LL1126;
_LL1126: _temp1125=(( struct Cyc_Absyn_Region_s_struct*) _temp1011)->f2; goto
_LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_Region_s_struct*) _temp1011)->f3;
goto _LL1054;} else{ goto _LL1012;} _LL1014: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1012; _LL1016: s=({ struct Cyc_PP_Doc*
_temp1129[ 2u]; _temp1129[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1129[ 0u]= Cyc_Absynpp_exp2doc( _temp1055); Cyc_PP_cat( _tag_arr(
_temp1129, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1012; _LL1018: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1059)){ s=({ struct Cyc_PP_Doc* _temp1130[
6u]; _temp1130[ 5u]= Cyc_Absynpp_is_declaration( _temp1057)?({ struct Cyc_PP_Doc*
_temp1131[ 4u]; _temp1131[ 3u]= Cyc_PP_line_doc(); _temp1131[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1131[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1057)); _temp1131[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1131, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1057); _temp1130[ 4u]= Cyc_PP_line_doc(); _temp1130[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1130[ 2u]= Cyc_PP_line_doc();
_temp1130[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1059)); _temp1130[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1130, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1057)){ s=({ struct Cyc_PP_Doc* _temp1132[ 6u]; _temp1132[ 5u]= Cyc_PP_line_doc();
_temp1132[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1132[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1057)); _temp1132[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1132[ 1u]= Cyc_PP_line_doc();
_temp1132[ 0u]= Cyc_Absynpp_stmt2doc( _temp1059); Cyc_PP_cat( _tag_arr(
_temp1132, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1133=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1133->hd=( void*) _temp1059;
_temp1133->tl=({ struct Cyc_List_List* _temp1134=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1134->hd=( void*) _temp1057;
_temp1134->tl= 0; _temp1134;}); _temp1133;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1135=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1135->hd=( void*) _temp1059;
_temp1135->tl=({ struct Cyc_List_List* _temp1136=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1136->hd=( void*) _temp1057;
_temp1136->tl= 0; _temp1136;}); _temp1135;}));} goto _LL1012; _LL1020: if(
_temp1061 ==  0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1137[ 3u]; _temp1137[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1137[ 1u]= _temp1061 ==  0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1061)); _temp1137[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1137, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1012;
_LL1022: { int print_else;{ void* _temp1138=( void*) _temp1063->r; _LL1140: if(
_temp1138 == ( void*) Cyc_Absyn_Skip_s){ goto _LL1141;} else{ goto _LL1142;}
_LL1142: goto _LL1143; _LL1141: print_else= 0; goto _LL1139; _LL1143: print_else=
1; goto _LL1139; _LL1139:;} s=({ struct Cyc_PP_Doc* _temp1144[ 7u]; _temp1144[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1146[ 5u]; _temp1146[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1146[ 3u]= Cyc_PP_line_doc();
_temp1146[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1147[ 2u]; _temp1147[
1u]= Cyc_Absynpp_stmt2doc( _temp1063); _temp1147[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1147, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1146[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1146[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1146, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1144[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1144[ 4u]= Cyc_PP_line_doc(); _temp1144[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1145[ 2u]; _temp1145[ 1u]= Cyc_Absynpp_stmt2doc( _temp1065); _temp1145[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1145, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1144[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1144[ 1u]= Cyc_Absynpp_exp2doc( _temp1067); _temp1144[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1144,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1012;} _LL1024: s=({ struct Cyc_PP_Doc*
_temp1148[ 6u]; _temp1148[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1148[ 4u]= Cyc_PP_line_doc(); _temp1148[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1149[ 2u]; _temp1149[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1069); _temp1149[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1149,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1148[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1148[ 1u]= Cyc_Absynpp_exp2doc( _temp1073);
_temp1148[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1148, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL1012; _LL1026: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL1012; _LL1028: s= Cyc_PP_text( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL1012; _LL1030: s= Cyc_PP_text(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp1151; _temp1151.tag= Cyc_Std_String_pa;
_temp1151.f1=( struct _tagged_arr)* _temp1075;{ void* _temp1150[ 1u]={&
_temp1151}; Cyc_Std_aprintf( _tag_arr("goto %s;", sizeof( unsigned char), 9u),
_tag_arr( _temp1150, sizeof( void*), 1u));}})); goto _LL1012; _LL1032: s=({
struct Cyc_PP_Doc* _temp1152[ 10u]; _temp1152[ 9u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1152[ 8u]= Cyc_PP_line_doc(); _temp1152[ 7u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1153[ 2u]; _temp1153[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1077); _temp1153[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1153,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1152[ 6u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1152[ 5u]= Cyc_Absynpp_exp2doc( _temp1081);
_temp1152[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char), 3u));
_temp1152[ 3u]= Cyc_Absynpp_exp2doc( _temp1085); _temp1152[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1152[ 1u]= Cyc_Absynpp_exp2doc(
_temp1087); _temp1152[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1152, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL1012; _LL1034: s=({ struct Cyc_PP_Doc* _temp1154[ 7u]; _temp1154[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1154[ 5u]= Cyc_PP_line_doc();
_temp1154[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1089); _temp1154[ 3u]= Cyc_PP_line_doc();
_temp1154[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1154[ 1u]= Cyc_Absynpp_exp2doc( _temp1091); _temp1154[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1154, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1012; _LL1036: s=({
struct Cyc_PP_Doc* _temp1155[ 7u]; _temp1155[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1155[ 5u]= Cyc_PP_line_doc(); _temp1155[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1093); _temp1155[ 3u]= Cyc_PP_line_doc();
_temp1155[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1155[ 1u]= Cyc_Absynpp_exp2doc( _temp1095); _temp1155[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1155, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1012; _LL1038: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL1012; _LL1040: s=({
struct Cyc_PP_Doc* _temp1156[ 3u]; _temp1156[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1156[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1099); _temp1156[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1156, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL1012; _LL1042: s=({ struct Cyc_PP_Doc* _temp1157[ 3u];
_temp1157[ 2u]= Cyc_Absynpp_stmt2doc( _temp1101); _temp1157[ 1u]= Cyc_PP_line_doc();
_temp1157[ 0u]= Cyc_Absynpp_decl2doc( _temp1103); Cyc_PP_cat( _tag_arr(
_temp1157, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1012; _LL1044: s=({
struct Cyc_PP_Doc* _temp1158[ 2u]; _temp1158[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1105)); _temp1158[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1158, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL1012; _LL1046: s=({ struct Cyc_PP_Doc* _temp1159[ 2u]; _temp1159[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1107)); _temp1159[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1159, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1012; _LL1048: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1109): 0){ s=({ struct Cyc_PP_Doc* _temp1160[ 6u];
_temp1160[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1160[ 4u]= Cyc_PP_line_doc(); _temp1160[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1109)); _temp1160[ 2u]= Cyc_PP_line_doc(); _temp1160[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1160[ 0u]= Cyc_PP_textptr(
_temp1111); Cyc_PP_cat( _tag_arr( _temp1160, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1161[ 3u]; _temp1161[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1109); _temp1161[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1161[ 0u]= Cyc_PP_textptr( _temp1111); Cyc_PP_cat( _tag_arr(
_temp1161, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1012; _LL1050: s=({
struct Cyc_PP_Doc* _temp1162[ 7u]; _temp1162[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1162[ 5u]= Cyc_Absynpp_exp2doc( _temp1115);
_temp1162[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1162[ 3u]= Cyc_PP_line_doc(); _temp1162[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1117)); _temp1162[ 1u]= Cyc_PP_line_doc(); _temp1162[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1162,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1012; _LL1052: s=({ struct Cyc_PP_Doc*
_temp1163[ 9u]; _temp1163[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1163[ 7u]= Cyc_PP_line_doc(); _temp1163[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1119)); _temp1163[ 5u]= Cyc_PP_line_doc(); _temp1163[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1163[ 3u]= Cyc_PP_line_doc();
_temp1163[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1121)); _temp1163[ 1u]=
Cyc_PP_line_doc(); _temp1163[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1163, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1012; _LL1054: s=({ struct Cyc_PP_Doc* _temp1164[ 9u];
_temp1164[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1164[ 7u]= Cyc_PP_line_doc(); _temp1164[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1123)); _temp1164[ 5u]= Cyc_PP_line_doc(); _temp1164[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1164[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1125->name); _temp1164[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1164[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1127)); _temp1164[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1164, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1012; _LL1012:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1165=( void*) p->r;
int _temp1197; void* _temp1199; unsigned char _temp1201; struct _tagged_arr
_temp1203; struct Cyc_Absyn_Vardecl* _temp1205; struct Cyc_List_List* _temp1207;
struct Cyc_Absyn_Pat* _temp1209; struct Cyc_Absyn_Vardecl* _temp1211; struct
_tuple0* _temp1213; struct Cyc_List_List* _temp1215; struct Cyc_List_List*
_temp1217; struct _tuple0* _temp1219; struct Cyc_List_List* _temp1221; struct
Cyc_List_List* _temp1223; struct _tuple0* _temp1225; struct Cyc_List_List*
_temp1227; struct Cyc_List_List* _temp1229; struct Cyc_Absyn_Structdecl*
_temp1231; struct Cyc_Absyn_Enumfield* _temp1233; struct Cyc_List_List*
_temp1235; struct Cyc_List_List* _temp1237; struct Cyc_Absyn_Tunionfield*
_temp1239; _LL1167: if( _temp1165 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1168;}
else{ goto _LL1169;} _LL1169: if( _temp1165 == ( void*) Cyc_Absyn_Null_p){ goto
_LL1170;} else{ goto _LL1171;} _LL1171: if(( unsigned int) _temp1165 >  2u?*((
int*) _temp1165) ==  Cyc_Absyn_Int_p: 0){ _LL1200: _temp1199=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1165)->f1; goto _LL1198; _LL1198: _temp1197=((
struct Cyc_Absyn_Int_p_struct*) _temp1165)->f2; goto _LL1172;} else{ goto
_LL1173;} _LL1173: if(( unsigned int) _temp1165 >  2u?*(( int*) _temp1165) == 
Cyc_Absyn_Char_p: 0){ _LL1202: _temp1201=(( struct Cyc_Absyn_Char_p_struct*)
_temp1165)->f1; goto _LL1174;} else{ goto _LL1175;} _LL1175: if(( unsigned int)
_temp1165 >  2u?*(( int*) _temp1165) ==  Cyc_Absyn_Float_p: 0){ _LL1204:
_temp1203=(( struct Cyc_Absyn_Float_p_struct*) _temp1165)->f1; goto _LL1176;}
else{ goto _LL1177;} _LL1177: if(( unsigned int) _temp1165 >  2u?*(( int*)
_temp1165) ==  Cyc_Absyn_Var_p: 0){ _LL1206: _temp1205=(( struct Cyc_Absyn_Var_p_struct*)
_temp1165)->f1; goto _LL1178;} else{ goto _LL1179;} _LL1179: if(( unsigned int)
_temp1165 >  2u?*(( int*) _temp1165) ==  Cyc_Absyn_Tuple_p: 0){ _LL1208:
_temp1207=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1165)->f1; goto _LL1180;}
else{ goto _LL1181;} _LL1181: if(( unsigned int) _temp1165 >  2u?*(( int*)
_temp1165) ==  Cyc_Absyn_Pointer_p: 0){ _LL1210: _temp1209=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1165)->f1; goto _LL1182;} else{ goto _LL1183;} _LL1183: if(( unsigned int)
_temp1165 >  2u?*(( int*) _temp1165) ==  Cyc_Absyn_Reference_p: 0){ _LL1212:
_temp1211=(( struct Cyc_Absyn_Reference_p_struct*) _temp1165)->f1; goto _LL1184;}
else{ goto _LL1185;} _LL1185: if(( unsigned int) _temp1165 >  2u?*(( int*)
_temp1165) ==  Cyc_Absyn_UnknownId_p: 0){ _LL1214: _temp1213=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1165)->f1; goto _LL1186;} else{ goto _LL1187;} _LL1187: if(( unsigned int)
_temp1165 >  2u?*(( int*) _temp1165) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL1220:
_temp1219=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1165)->f1; goto
_LL1218; _LL1218: _temp1217=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1165)->f2;
goto _LL1216; _LL1216: _temp1215=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1165)->f3; goto _LL1188;} else{ goto _LL1189;} _LL1189: if(( unsigned int)
_temp1165 >  2u?*(( int*) _temp1165) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL1226:
_temp1225=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1165)->f1; goto
_LL1224; _LL1224: _temp1223=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1165)->f2; goto _LL1222; _LL1222: _temp1221=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1165)->f3; goto _LL1190;} else{ goto _LL1191;} _LL1191: if(( unsigned int)
_temp1165 >  2u?*(( int*) _temp1165) ==  Cyc_Absyn_Struct_p: 0){ _LL1232:
_temp1231=(( struct Cyc_Absyn_Struct_p_struct*) _temp1165)->f1; goto _LL1230;
_LL1230: _temp1229=(( struct Cyc_Absyn_Struct_p_struct*) _temp1165)->f3; goto
_LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_Struct_p_struct*) _temp1165)->f4;
goto _LL1192;} else{ goto _LL1193;} _LL1193: if(( unsigned int) _temp1165 >  2u?*((
int*) _temp1165) ==  Cyc_Absyn_Enum_p: 0){ _LL1234: _temp1233=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1165)->f2; goto _LL1194;} else{ goto _LL1195;} _LL1195: if(( unsigned int)
_temp1165 >  2u?*(( int*) _temp1165) ==  Cyc_Absyn_Tunion_p: 0){ _LL1240:
_temp1239=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1165)->f2; goto _LL1238;
_LL1238: _temp1237=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1165)->f3; goto
_LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1165)->f4;
goto _LL1196;} else{ goto _LL1166;} _LL1168: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1166; _LL1170: s= Cyc_PP_text( _tag_arr("NULL",
sizeof( unsigned char), 5u)); goto _LL1166; _LL1172: if( _temp1199 == ( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1242; _temp1242.tag= Cyc_Std_Int_pa; _temp1242.f1=( int)(( unsigned int)
_temp1197);{ void* _temp1241[ 1u]={& _temp1242}; Cyc_Std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp1241, sizeof( void*), 1u));}}));}
else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1244; _temp1244.tag= Cyc_Std_Int_pa; _temp1244.f1=( unsigned int) _temp1197;{
void* _temp1243[ 1u]={& _temp1244}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp1243, sizeof( void*), 1u));}}));} goto
_LL1166; _LL1174: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1246; _temp1246.tag= Cyc_Std_String_pa; _temp1246.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1201);{ void* _temp1245[ 1u]={& _temp1246}; Cyc_Std_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1245, sizeof( void*),
1u));}})); goto _LL1166; _LL1176: s= Cyc_PP_text( _temp1203); goto _LL1166;
_LL1178: s= Cyc_Absynpp_qvar2doc( _temp1205->name); goto _LL1166; _LL1180: s=({
struct Cyc_PP_Doc* _temp1247[ 3u]; _temp1247[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1247[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1207); _temp1247[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1247, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1166; _LL1182: s=({ struct Cyc_PP_Doc* _temp1248[ 2u]; _temp1248[ 1u]=
Cyc_Absynpp_pat2doc( _temp1209); _temp1248[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1248, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1166; _LL1184: s=({ struct Cyc_PP_Doc* _temp1249[
2u]; _temp1249[ 1u]= Cyc_Absynpp_qvar2doc( _temp1211->name); _temp1249[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1249,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1166; _LL1186: s= Cyc_Absynpp_qvar2doc(
_temp1213); goto _LL1166; _LL1188: s=({ struct Cyc_PP_Doc* _temp1250[ 3u];
_temp1250[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1215));
_temp1250[ 1u]= Cyc_Absynpp_tvars2doc( _temp1217); _temp1250[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1219); Cyc_PP_cat( _tag_arr( _temp1250, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1166; _LL1190: s=({ struct Cyc_PP_Doc* _temp1251[ 3u]; _temp1251[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1221)); _temp1251[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1223); _temp1251[ 0u]= Cyc_Absynpp_qvar2doc( _temp1225); Cyc_PP_cat(
_tag_arr( _temp1251, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1166; _LL1192:
s=({ struct Cyc_PP_Doc* _temp1252[ 3u]; _temp1252[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1227)); _temp1252[ 1u]= Cyc_Absynpp_tvars2doc( _temp1229); _temp1252[ 0u]=
_temp1231->name ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1231->name))->v); Cyc_PP_cat( _tag_arr(
_temp1252, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1166; _LL1194: s= Cyc_Absynpp_qvar2doc(
_temp1233->name); goto _LL1166; _LL1196: if( _temp1235 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp1239->name);} else{ s=({ struct Cyc_PP_Doc* _temp1253[ 3u]; _temp1253[ 2u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1235)); _temp1253[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1237); _temp1253[ 0u]= Cyc_Absynpp_qvar2doc( _temp1239->name); Cyc_PP_cat(
_tag_arr( _temp1253, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1166;
_LL1166:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1254[ 2u]; _temp1254[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1254[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1254, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause ==  0?( void*)( c->pattern)->r
== ( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1255[ 2u];
_temp1255[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1256[ 2u]; _temp1256[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1256[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1256, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1255[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1255, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 
0){ return({ struct Cyc_PP_Doc* _temp1257[ 4u]; _temp1257[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1258[ 2u]; _temp1258[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1258[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1258, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1257[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1257[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1257[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1257, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1259[ 6u]; _temp1259[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1260[ 2u]; _temp1260[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1260[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1260, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1259[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1259[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1259[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1259[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1259[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1259,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1263;
struct Cyc_Absyn_Stmt* _temp1264; struct Cyc_Absyn_Exp* _temp1266; struct Cyc_Absyn_SwitchC_clause*
_temp1261= c; _temp1263=* _temp1261; _LL1267: _temp1266= _temp1263.cnst_exp;
goto _LL1265; _LL1265: _temp1264= _temp1263.body; goto _LL1262; _LL1262: if(
_temp1266 ==  0){ return({ struct Cyc_PP_Doc* _temp1268[ 2u]; _temp1268[ 1u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1269[ 2u]; _temp1269[ 1u]= Cyc_Absynpp_stmt2doc(
c->body); _temp1269[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1269,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1268[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1268, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1270[ 4u];
_temp1270[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1271[ 2u]; _temp1271[
1u]= Cyc_Absynpp_stmt2doc( _temp1264); _temp1271[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1271, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1270[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1270[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1266)); _temp1270[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1270,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag ==  0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1272[ 3u]; _temp1272[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1272[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1272[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1272, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1273=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1299; struct Cyc_Absyn_Structdecl* _temp1301;
struct Cyc_Absyn_Uniondecl* _temp1303; struct Cyc_Absyn_Vardecl* _temp1305;
struct Cyc_Absyn_Vardecl _temp1307; struct Cyc_List_List* _temp1308; struct Cyc_Absyn_Exp*
_temp1310; void* _temp1312; struct Cyc_Absyn_Tqual _temp1314; struct _tuple0*
_temp1316; void* _temp1318; struct Cyc_Absyn_Tuniondecl* _temp1320; struct Cyc_Absyn_Tuniondecl
_temp1322; int _temp1323; struct Cyc_Core_Opt* _temp1325; struct Cyc_List_List*
_temp1327; struct _tuple0* _temp1329; void* _temp1331; struct Cyc_Absyn_Exp*
_temp1333; struct Cyc_Absyn_Pat* _temp1335; struct Cyc_List_List* _temp1337;
struct Cyc_Absyn_Enumdecl* _temp1339; struct Cyc_Absyn_Enumdecl _temp1341;
struct Cyc_Core_Opt* _temp1342; struct _tuple0* _temp1344; void* _temp1346;
struct Cyc_Absyn_Typedefdecl* _temp1348; struct Cyc_List_List* _temp1350; struct
_tagged_arr* _temp1352; struct Cyc_List_List* _temp1354; struct _tuple0*
_temp1356; struct Cyc_List_List* _temp1358; _LL1275: if(*(( int*) _temp1273) == 
Cyc_Absyn_Fn_d){ _LL1300: _temp1299=(( struct Cyc_Absyn_Fn_d_struct*) _temp1273)->f1;
goto _LL1276;} else{ goto _LL1277;} _LL1277: if(*(( int*) _temp1273) ==  Cyc_Absyn_Struct_d){
_LL1302: _temp1301=(( struct Cyc_Absyn_Struct_d_struct*) _temp1273)->f1; goto
_LL1278;} else{ goto _LL1279;} _LL1279: if(*(( int*) _temp1273) ==  Cyc_Absyn_Union_d){
_LL1304: _temp1303=(( struct Cyc_Absyn_Union_d_struct*) _temp1273)->f1; goto
_LL1280;} else{ goto _LL1281;} _LL1281: if(*(( int*) _temp1273) ==  Cyc_Absyn_Var_d){
_LL1306: _temp1305=(( struct Cyc_Absyn_Var_d_struct*) _temp1273)->f1; _temp1307=*
_temp1305; _LL1319: _temp1318=( void*) _temp1307.sc; goto _LL1317; _LL1317:
_temp1316= _temp1307.name; goto _LL1315; _LL1315: _temp1314= _temp1307.tq; goto
_LL1313; _LL1313: _temp1312=( void*) _temp1307.type; goto _LL1311; _LL1311:
_temp1310= _temp1307.initializer; goto _LL1309; _LL1309: _temp1308= _temp1307.attributes;
goto _LL1282;} else{ goto _LL1283;} _LL1283: if(*(( int*) _temp1273) ==  Cyc_Absyn_Tunion_d){
_LL1321: _temp1320=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1273)->f1;
_temp1322=* _temp1320; _LL1332: _temp1331=( void*) _temp1322.sc; goto _LL1330;
_LL1330: _temp1329= _temp1322.name; goto _LL1328; _LL1328: _temp1327= _temp1322.tvs;
goto _LL1326; _LL1326: _temp1325= _temp1322.fields; goto _LL1324; _LL1324:
_temp1323= _temp1322.is_xtunion; goto _LL1284;} else{ goto _LL1285;} _LL1285:
if(*(( int*) _temp1273) ==  Cyc_Absyn_Let_d){ _LL1336: _temp1335=(( struct Cyc_Absyn_Let_d_struct*)
_temp1273)->f1; goto _LL1334; _LL1334: _temp1333=(( struct Cyc_Absyn_Let_d_struct*)
_temp1273)->f4; goto _LL1286;} else{ goto _LL1287;} _LL1287: if(*(( int*)
_temp1273) ==  Cyc_Absyn_Letv_d){ _LL1338: _temp1337=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1273)->f1; goto _LL1288;} else{ goto _LL1289;} _LL1289: if(*(( int*)
_temp1273) ==  Cyc_Absyn_Enum_d){ _LL1340: _temp1339=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1273)->f1; _temp1341=* _temp1339; _LL1347: _temp1346=( void*) _temp1341.sc;
goto _LL1345; _LL1345: _temp1344= _temp1341.name; goto _LL1343; _LL1343:
_temp1342= _temp1341.fields; goto _LL1290;} else{ goto _LL1291;} _LL1291: if(*((
int*) _temp1273) ==  Cyc_Absyn_Typedef_d){ _LL1349: _temp1348=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1273)->f1; goto _LL1292;} else{ goto _LL1293;} _LL1293: if(*(( int*)
_temp1273) ==  Cyc_Absyn_Namespace_d){ _LL1353: _temp1352=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1273)->f1; goto _LL1351; _LL1351: _temp1350=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1273)->f2; goto _LL1294;} else{ goto _LL1295;} _LL1295: if(*(( int*)
_temp1273) ==  Cyc_Absyn_Using_d){ _LL1357: _temp1356=(( struct Cyc_Absyn_Using_d_struct*)
_temp1273)->f1; goto _LL1355; _LL1355: _temp1354=(( struct Cyc_Absyn_Using_d_struct*)
_temp1273)->f2; goto _LL1296;} else{ goto _LL1297;} _LL1297: if(*(( int*)
_temp1273) ==  Cyc_Absyn_ExternC_d){ _LL1359: _temp1358=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1273)->f1; goto _LL1298;} else{ goto _LL1274;} _LL1276: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1367=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1367[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1368; _temp1368.tag= Cyc_Absyn_FnType; _temp1368.f1=({ struct Cyc_Absyn_FnInfo
_temp1369; _temp1369.tvars= _temp1299->tvs; _temp1369.effect= _temp1299->effect;
_temp1369.ret_typ=( void*)(( void*) _temp1299->ret_type); _temp1369.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1299->args); _temp1369.c_varargs=
_temp1299->c_varargs; _temp1369.cyc_varargs= _temp1299->cyc_varargs; _temp1369.rgn_po=
_temp1299->rgn_po; _temp1369.attributes= 0; _temp1369;}); _temp1368;});
_temp1367;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1299->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1299->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1299->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1299->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1299->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1364; _temp1364.q_const= 0; _temp1364.q_volatile= 0; _temp1364.q_restrict=
0; _temp1364;}), t,({ struct Cyc_Core_Opt* _temp1365=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1365->v=( void*)({ struct Cyc_PP_Doc*
_temp1366[ 2u]; _temp1366[ 1u]= namedoc; _temp1366[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1366, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1365;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1362[ 4u]; _temp1362[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1362[ 2u]= Cyc_PP_line_doc();
_temp1362[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1363[ 2u]; _temp1363[
1u]= Cyc_Absynpp_stmt2doc( _temp1299->body); _temp1363[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1363, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1362[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1362, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1360[ 4u]; _temp1360[ 3u]= bodydoc; _temp1360[ 2u]= tqtddoc; _temp1360[ 1u]=
scopedoc; _temp1360[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1360, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1361[ 2u]; _temp1361[ 1u]= s; _temp1361[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1361, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1274;}} _LL1278: if(
_temp1301->fields ==  0){ s=({ struct Cyc_PP_Doc* _temp1370[ 5u]; _temp1370[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1370[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1301->tvs); _temp1370[ 2u]= _temp1301->name ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1301->name))->v); _temp1370[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1370[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1301->sc); Cyc_PP_cat( _tag_arr( _temp1370, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1371[ 10u]; _temp1371[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1371[ 8u]= Cyc_Absynpp_atts2doc(
_temp1301->attributes); _temp1371[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1371[ 6u]= Cyc_PP_line_doc(); _temp1371[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1372[ 2u]; _temp1372[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1301->fields))->v);
_temp1372[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1372, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1371[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1371[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1301->tvs);
_temp1371[ 2u]= _temp1301->name ==  0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1301->name))->v); _temp1371[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1371[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1301->sc); Cyc_PP_cat( _tag_arr( _temp1371, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1274; _LL1280: if( _temp1303->fields ==  0){ s=({ struct Cyc_PP_Doc*
_temp1373[ 5u]; _temp1373[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1373[ 3u]= Cyc_Absynpp_tvars2doc( _temp1303->tvs); _temp1373[ 2u]=
_temp1303->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1303->name))->v); _temp1373[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1373[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1303->sc);
Cyc_PP_cat( _tag_arr( _temp1373, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1374[ 10u]; _temp1374[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1374[ 8u]= Cyc_Absynpp_atts2doc( _temp1303->attributes);
_temp1374[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1374[ 6u]= Cyc_PP_line_doc(); _temp1374[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1375[ 2u]; _temp1375[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1303->fields))->v); _temp1375[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1375, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1374[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1374[ 3u]= Cyc_Absynpp_tvars2doc( _temp1303->tvs); _temp1374[ 2u]=
_temp1303->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1303->name))->v); _temp1374[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1374[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1303->sc);
Cyc_PP_cat( _tag_arr( _temp1374, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1274; _LL1282: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1316); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1308); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1376= Cyc_Tcutil_compress( _temp1312); struct Cyc_Absyn_FnInfo
_temp1382; struct Cyc_List_List* _temp1384; _LL1378: if(( unsigned int)
_temp1376 >  4u?*(( int*) _temp1376) ==  Cyc_Absyn_FnType: 0){ _LL1383:
_temp1382=(( struct Cyc_Absyn_FnType_struct*) _temp1376)->f1; _LL1385: _temp1384=
_temp1382.attributes; goto _LL1379;} else{ goto _LL1380;} _LL1380: goto _LL1381;
_LL1379: beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1384); goto _LL1377;
_LL1381: beforenamedoc= Cyc_PP_nil_doc(); goto _LL1377; _LL1377:;} s=({ struct
Cyc_PP_Doc* _temp1386[ 5u]; _temp1386[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); _temp1386[ 3u]= _temp1310 ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1389[ 2u]; _temp1389[ 1u]=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1310)); _temp1389[
0u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp1389, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1386[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1314, _temp1312,({ struct Cyc_Core_Opt* _temp1387=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1387->v=( void*)({ struct Cyc_PP_Doc*
_temp1388[ 2u]; _temp1388[ 1u]= sn; _temp1388[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1388, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1387;}));
_temp1386[ 1u]= Cyc_Absynpp_scope2doc( _temp1318); _temp1386[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1386, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1274;} _LL1284: if( _temp1325 ==  0){ s=({ struct Cyc_PP_Doc*
_temp1390[ 5u]; _temp1390[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1390[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1327); _temp1390[ 2u]=
_temp1323? Cyc_Absynpp_qvar2doc( _temp1329): Cyc_Absynpp_typedef_name2doc(
_temp1329); _temp1390[ 1u]= _temp1323? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1390[ 0u]= Cyc_Absynpp_scope2doc( _temp1331); Cyc_PP_cat( _tag_arr(
_temp1390, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1391[ 8u]; _temp1391[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1391[ 6u]= Cyc_PP_line_doc(); _temp1391[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1392[ 2u]; _temp1392[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1325))->v);
_temp1392[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1392, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1391[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1391[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1327);
_temp1391[ 2u]= _temp1323? Cyc_Absynpp_qvar2doc( _temp1329): Cyc_Absynpp_typedef_name2doc(
_temp1329); _temp1391[ 1u]= _temp1323? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1391[ 0u]= Cyc_Absynpp_scope2doc( _temp1331); Cyc_PP_cat( _tag_arr(
_temp1391, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1274; _LL1286: s=({
struct Cyc_PP_Doc* _temp1393[ 5u]; _temp1393[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1393[ 3u]= Cyc_Absynpp_exp2doc( _temp1333);
_temp1393[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1393[ 1u]= Cyc_Absynpp_pat2doc( _temp1335); _temp1393[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1393,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1274; _LL1288: s=({ struct Cyc_PP_Doc*
_temp1394[ 3u]; _temp1394[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1394[ 1u]= Cyc_Absynpp_ids2doc( _temp1337); _temp1394[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1394,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1274; _LL1290: if( _temp1342 ==  0){
s=({ struct Cyc_PP_Doc* _temp1395[ 4u]; _temp1395[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1395[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1344); _temp1395[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1395[ 0u]= Cyc_Absynpp_scope2doc( _temp1346); Cyc_PP_cat( _tag_arr(
_temp1395, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1396[ 7u]; _temp1396[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1396[ 5u]= Cyc_PP_line_doc(); _temp1396[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1397[ 2u]; _temp1397[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1342))->v);
_temp1397[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1397, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1396[ 3u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1396[ 2u]= Cyc_Absynpp_qvar2doc( _temp1344);
_temp1396[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u));
_temp1396[ 0u]= Cyc_Absynpp_scope2doc( _temp1346); Cyc_PP_cat( _tag_arr(
_temp1396, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1274; _LL1292: s=({
struct Cyc_PP_Doc* _temp1398[ 3u]; _temp1398[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1398[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1399; _temp1399.q_const= 0; _temp1399.q_volatile= 0; _temp1399.q_restrict=
0; _temp1399;}),( void*) _temp1348->defn,({ struct Cyc_Core_Opt* _temp1400=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1400->v=(
void*)({ struct Cyc_PP_Doc* _temp1401[ 2u]; _temp1401[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1348->tvs); _temp1401[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1348->name);
Cyc_PP_cat( _tag_arr( _temp1401, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1400;}));
_temp1398[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1398, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1274; _LL1294: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1352);} s=({ struct Cyc_PP_Doc* _temp1402[ 7u]; _temp1402[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1402[ 5u]= Cyc_PP_line_doc();
_temp1402[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1350); _temp1402[ 3u]= Cyc_PP_line_doc();
_temp1402[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1402[ 1u]= Cyc_PP_textptr( _temp1352); _temp1402[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1402, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1274; _LL1296: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1403[ 7u]; _temp1403[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1403[ 5u]= Cyc_PP_line_doc(); _temp1403[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1354); _temp1403[ 3u]= Cyc_PP_line_doc();
_temp1403[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1403[ 1u]= Cyc_Absynpp_qvar2doc( _temp1356); _temp1403[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1403,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1404[
7u]; _temp1404[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1404[ 5u]= Cyc_PP_line_doc(); _temp1404[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1354); _temp1404[ 3u]= Cyc_PP_line_doc();
_temp1404[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1404[ 1u]= Cyc_Absynpp_qvar2doc( _temp1356); _temp1404[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1404, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1274; _LL1298: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1405[ 5u]; _temp1405[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1405[ 3u]= Cyc_PP_line_doc(); _temp1405[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1358); _temp1405[ 1u]= Cyc_PP_line_doc();
_temp1405[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1405, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1406[ 5u]; _temp1406[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1406[ 3u]= Cyc_PP_line_doc();
_temp1406[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1358); _temp1406[ 1u]= Cyc_PP_line_doc();
_temp1406[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1406, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1274; _LL1274:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1407= sc; _LL1409: if( _temp1407 == ( void*) Cyc_Absyn_Static){
goto _LL1410;} else{ goto _LL1411;} _LL1411: if( _temp1407 == ( void*) Cyc_Absyn_Public){
goto _LL1412;} else{ goto _LL1413;} _LL1413: if( _temp1407 == ( void*) Cyc_Absyn_Extern){
goto _LL1414;} else{ goto _LL1415;} _LL1415: if( _temp1407 == ( void*) Cyc_Absyn_ExternC){
goto _LL1416;} else{ goto _LL1417;} _LL1417: if( _temp1407 == ( void*) Cyc_Absyn_Abstract){
goto _LL1418;} else{ goto _LL1408;} _LL1410: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1412: return Cyc_PP_nil_doc(); _LL1414: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1416: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1418:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1408:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1419= t; struct
Cyc_Absyn_Tvar* _temp1427; struct Cyc_List_List* _temp1429; _LL1421: if((
unsigned int) _temp1419 >  4u?*(( int*) _temp1419) ==  Cyc_Absyn_VarType: 0){
_LL1428: _temp1427=(( struct Cyc_Absyn_VarType_struct*) _temp1419)->f1; goto
_LL1422;} else{ goto _LL1423;} _LL1423: if(( unsigned int) _temp1419 >  4u?*((
int*) _temp1419) ==  Cyc_Absyn_JoinEff: 0){ _LL1430: _temp1429=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1419)->f1; goto _LL1424;} else{ goto _LL1425;} _LL1425: goto _LL1426;
_LL1422: return Cyc_Tcutil_is_temp_tvar( _temp1427); _LL1424: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1429); _LL1426: return 0; _LL1420:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1431= t; struct Cyc_Absyn_Exp* _temp1445; struct Cyc_Absyn_Tqual _temp1447;
void* _temp1449; struct Cyc_Absyn_PtrInfo _temp1451; struct Cyc_Absyn_Conref*
_temp1453; struct Cyc_Absyn_Tqual _temp1455; struct Cyc_Absyn_Conref* _temp1457;
void* _temp1459; void* _temp1461; struct Cyc_Absyn_FnInfo _temp1463; struct Cyc_List_List*
_temp1465; struct Cyc_List_List* _temp1467; struct Cyc_Absyn_VarargInfo*
_temp1469; int _temp1471; struct Cyc_List_List* _temp1473; void* _temp1475;
struct Cyc_Core_Opt* _temp1477; struct Cyc_List_List* _temp1479; int _temp1481;
struct Cyc_Core_Opt* _temp1483; struct Cyc_Core_Opt* _temp1485; struct Cyc_Core_Opt*
_temp1487; struct Cyc_List_List* _temp1489; struct _tuple0* _temp1491; _LL1433:
if(( unsigned int) _temp1431 >  4u?*(( int*) _temp1431) ==  Cyc_Absyn_ArrayType:
0){ _LL1450: _temp1449=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1431)->f1;
goto _LL1448; _LL1448: _temp1447=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1431)->f2; goto _LL1446; _LL1446: _temp1445=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1431)->f3; goto _LL1434;} else{ goto _LL1435;} _LL1435: if(( unsigned int)
_temp1431 >  4u?*(( int*) _temp1431) ==  Cyc_Absyn_PointerType: 0){ _LL1452:
_temp1451=(( struct Cyc_Absyn_PointerType_struct*) _temp1431)->f1; _LL1462:
_temp1461=( void*) _temp1451.elt_typ; goto _LL1460; _LL1460: _temp1459=( void*)
_temp1451.rgn_typ; goto _LL1458; _LL1458: _temp1457= _temp1451.nullable; goto
_LL1456; _LL1456: _temp1455= _temp1451.tq; goto _LL1454; _LL1454: _temp1453=
_temp1451.bounds; goto _LL1436;} else{ goto _LL1437;} _LL1437: if(( unsigned int)
_temp1431 >  4u?*(( int*) _temp1431) ==  Cyc_Absyn_FnType: 0){ _LL1464:
_temp1463=(( struct Cyc_Absyn_FnType_struct*) _temp1431)->f1; _LL1480: _temp1479=
_temp1463.tvars; goto _LL1478; _LL1478: _temp1477= _temp1463.effect; goto
_LL1476; _LL1476: _temp1475=( void*) _temp1463.ret_typ; goto _LL1474; _LL1474:
_temp1473= _temp1463.args; goto _LL1472; _LL1472: _temp1471= _temp1463.c_varargs;
goto _LL1470; _LL1470: _temp1469= _temp1463.cyc_varargs; goto _LL1468; _LL1468:
_temp1467= _temp1463.rgn_po; goto _LL1466; _LL1466: _temp1465= _temp1463.attributes;
goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( unsigned int) _temp1431 >  4u?*((
int*) _temp1431) ==  Cyc_Absyn_Evar: 0){ _LL1486: _temp1485=(( struct Cyc_Absyn_Evar_struct*)
_temp1431)->f1; goto _LL1484; _LL1484: _temp1483=(( struct Cyc_Absyn_Evar_struct*)
_temp1431)->f2; goto _LL1482; _LL1482: _temp1481=(( struct Cyc_Absyn_Evar_struct*)
_temp1431)->f3; goto _LL1440;} else{ goto _LL1441;} _LL1441: if(( unsigned int)
_temp1431 >  4u?*(( int*) _temp1431) ==  Cyc_Absyn_TypedefType: 0){ _LL1492:
_temp1491=(( struct Cyc_Absyn_TypedefType_struct*) _temp1431)->f1; goto _LL1490;
_LL1490: _temp1489=(( struct Cyc_Absyn_TypedefType_struct*) _temp1431)->f2; goto
_LL1488; _LL1488: _temp1487=(( struct Cyc_Absyn_TypedefType_struct*) _temp1431)->f3;
goto _LL1442;} else{ goto _LL1443;} _LL1443: goto _LL1444; _LL1434: { struct Cyc_List_List*
_temp1495; void* _temp1497; struct Cyc_Absyn_Tqual _temp1499; struct _tuple4
_temp1493= Cyc_Absynpp_to_tms( _temp1447, _temp1449); _LL1500: _temp1499=
_temp1493.f1; goto _LL1498; _LL1498: _temp1497= _temp1493.f2; goto _LL1496;
_LL1496: _temp1495= _temp1493.f3; goto _LL1494; _LL1494: { void* tm; if(
_temp1445 ==  0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1501=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1501[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1502; _temp1502.tag= Cyc_Absyn_ConstArray_mod;
_temp1502.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1445); _temp1502;});
_temp1501;});} return({ struct _tuple4 _temp1503; _temp1503.f1= _temp1499;
_temp1503.f2= _temp1497; _temp1503.f3=({ struct Cyc_List_List* _temp1504=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1504->hd=(
void*) tm; _temp1504->tl= _temp1495; _temp1504;}); _temp1503;});}} _LL1436: {
struct Cyc_List_List* _temp1507; void* _temp1509; struct Cyc_Absyn_Tqual
_temp1511; struct _tuple4 _temp1505= Cyc_Absynpp_to_tms( _temp1455, _temp1461);
_LL1512: _temp1511= _temp1505.f1; goto _LL1510; _LL1510: _temp1509= _temp1505.f2;
goto _LL1508; _LL1508: _temp1507= _temp1505.f3; goto _LL1506; _LL1506: { void*
ps;{ struct _tuple8 _temp1514=({ struct _tuple8 _temp1513; _temp1513.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1457))->v; _temp1513.f2=( void*)( Cyc_Absyn_compress_conref( _temp1453))->v;
_temp1513;}); void* _temp1524; void* _temp1526; struct Cyc_Absyn_Exp* _temp1528;
void* _temp1530; int _temp1532; void* _temp1534; void* _temp1536; struct Cyc_Absyn_Exp*
_temp1538; void* _temp1540; int _temp1542; void* _temp1544; void* _temp1546;
_LL1516: _LL1531: _temp1530= _temp1514.f1; if(( unsigned int) _temp1530 >  1u?*((
int*) _temp1530) ==  Cyc_Absyn_Eq_constr: 0){ _LL1533: _temp1532=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1530)->f1; if( _temp1532 ==  1){ goto _LL1525;}
else{ goto _LL1518;}} else{ goto _LL1518;} _LL1525: _temp1524= _temp1514.f2; if((
unsigned int) _temp1524 >  1u?*(( int*) _temp1524) ==  Cyc_Absyn_Eq_constr: 0){
_LL1527: _temp1526=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1524)->f1;
if(( unsigned int) _temp1526 >  1u?*(( int*) _temp1526) ==  Cyc_Absyn_Upper_b: 0){
_LL1529: _temp1528=(( struct Cyc_Absyn_Upper_b_struct*) _temp1526)->f1; goto
_LL1517;} else{ goto _LL1518;}} else{ goto _LL1518;} _LL1518: _LL1541: _temp1540=
_temp1514.f1; if(( unsigned int) _temp1540 >  1u?*(( int*) _temp1540) ==  Cyc_Absyn_Eq_constr:
0){ _LL1543: _temp1542=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1540)->f1;
if( _temp1542 ==  0){ goto _LL1535;} else{ goto _LL1520;}} else{ goto _LL1520;}
_LL1535: _temp1534= _temp1514.f2; if(( unsigned int) _temp1534 >  1u?*(( int*)
_temp1534) ==  Cyc_Absyn_Eq_constr: 0){ _LL1537: _temp1536=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1534)->f1; if(( unsigned int) _temp1536 >  1u?*(( int*) _temp1536) ==  Cyc_Absyn_Upper_b:
0){ _LL1539: _temp1538=(( struct Cyc_Absyn_Upper_b_struct*) _temp1536)->f1; goto
_LL1519;} else{ goto _LL1520;}} else{ goto _LL1520;} _LL1520: _LL1545: _temp1544=
_temp1514.f2; if(( unsigned int) _temp1544 >  1u?*(( int*) _temp1544) ==  Cyc_Absyn_Eq_constr:
0){ _LL1547: _temp1546=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1544)->f1;
if( _temp1546 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1521;} else{ goto
_LL1522;}} else{ goto _LL1522;} _LL1522: goto _LL1523; _LL1517: ps=( void*)({
struct Cyc_Absyn_Nullable_ps_struct* _temp1548=( struct Cyc_Absyn_Nullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp1548[ 0]=({
struct Cyc_Absyn_Nullable_ps_struct _temp1549; _temp1549.tag= Cyc_Absyn_Nullable_ps;
_temp1549.f1= _temp1528; _temp1549;}); _temp1548;}); goto _LL1515; _LL1519: ps=(
void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp1550=( struct Cyc_Absyn_NonNullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp1550[ 0]=({
struct Cyc_Absyn_NonNullable_ps_struct _temp1551; _temp1551.tag= Cyc_Absyn_NonNullable_ps;
_temp1551.f1= _temp1538; _temp1551;}); _temp1550;}); goto _LL1515; _LL1521: ps=(
void*) Cyc_Absyn_TaggedArray_ps; goto _LL1515; _LL1523: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1515; _LL1515:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp1554=( struct Cyc_Absyn_Pointer_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1554[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1555; _temp1555.tag=
Cyc_Absyn_Pointer_mod; _temp1555.f1=( void*) ps; _temp1555.f2=( void*) _temp1459;
_temp1555.f3= tq; _temp1555;}); _temp1554;}); return({ struct _tuple4 _temp1552;
_temp1552.f1= _temp1511; _temp1552.f2= _temp1509; _temp1552.f3=({ struct Cyc_List_List*
_temp1553=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1553->hd=( void*) tm; _temp1553->tl= _temp1507; _temp1553;}); _temp1552;});}}}
_LL1438: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1477 ==  0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1477))->v)){ _temp1477= 0;
_temp1479= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_rewrite_temp_tvar,
_temp1479);}}{ struct Cyc_List_List* _temp1558; void* _temp1560; struct Cyc_Absyn_Tqual
_temp1562; struct _tuple4 _temp1556= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp1475); _LL1563: _temp1562= _temp1556.f1; goto _LL1561; _LL1561: _temp1560=
_temp1556.f2; goto _LL1559; _LL1559: _temp1558= _temp1556.f3; goto _LL1557;
_LL1557: { struct Cyc_List_List* tms= _temp1558; if( _temp1465 !=  0? ! Cyc_Absynpp_to_VC:
0){ tms=({ struct Cyc_List_List* _temp1564=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1564->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp1565=( struct Cyc_Absyn_Attributes_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp1565[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp1566; _temp1566.tag= Cyc_Absyn_Attributes_mod; _temp1566.f1= 0; _temp1566.f2=
_temp1465; _temp1566;}); _temp1565;})); _temp1564->tl= tms; _temp1564;});} tms=({
struct Cyc_List_List* _temp1567=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1567->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp1568=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp1568[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp1569; _temp1569.tag=
Cyc_Absyn_Function_mod; _temp1569.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp1570=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1570[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1571; _temp1571.tag= Cyc_Absyn_WithTypes;
_temp1571.f1= _temp1473; _temp1571.f2= _temp1471; _temp1571.f3= _temp1469;
_temp1571.f4= _temp1477; _temp1571.f5= _temp1467; _temp1571;}); _temp1570;}));
_temp1569;}); _temp1568;})); _temp1567->tl= tms; _temp1567;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1465 !=  0; _temp1465=(( struct Cyc_List_List*) _check_null(
_temp1465))->tl){ void* _temp1572=( void*)(( struct Cyc_List_List*) _check_null(
_temp1465))->hd; _LL1574: if( _temp1572 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1575;} else{ goto _LL1576;} _LL1576: if( _temp1572 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1577;} else{ goto _LL1578;} _LL1578: if( _temp1572 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1579;} else{ goto _LL1580;} _LL1580: goto _LL1581; _LL1575: goto _LL1577;
_LL1577: goto _LL1579; _LL1579: tms=({ struct Cyc_List_List* _temp1582=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1582->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1583=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1583[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1584; _temp1584.tag= Cyc_Absyn_Attributes_mod;
_temp1584.f1= 0; _temp1584.f2=({ struct Cyc_List_List* _temp1585=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1585->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp1465))->hd); _temp1585->tl= 0;
_temp1585;}); _temp1584;}); _temp1583;})); _temp1582->tl= tms; _temp1582;});
goto AfterAtts; _LL1581: goto _LL1573; _LL1573:;}} AfterAtts: if( _temp1479 != 
0){ tms=({ struct Cyc_List_List* _temp1586=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1586->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp1587=( struct Cyc_Absyn_TypeParams_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp1587[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp1588; _temp1588.tag= Cyc_Absyn_TypeParams_mod; _temp1588.f1= _temp1479;
_temp1588.f2= 0; _temp1588.f3= 1; _temp1588;}); _temp1587;})); _temp1586->tl=
tms; _temp1586;});} return({ struct _tuple4 _temp1589; _temp1589.f1= _temp1562;
_temp1589.f2= _temp1560; _temp1589.f3= tms; _temp1589;});}} _LL1440: if(
_temp1483 ==  0){ return({ struct _tuple4 _temp1590; _temp1590.f1= tq; _temp1590.f2=
t; _temp1590.f3= 0; _temp1590;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1483))->v);} _LL1442: if( _temp1487 == 
0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp1591;
_temp1591.f1= tq; _temp1591.f2= t; _temp1591.f3= 0; _temp1591;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1487))->v);}
_LL1444: return({ struct _tuple4 _temp1592; _temp1592.f1= tq; _temp1592.f2= t;
_temp1592.f3= 0; _temp1592;}); _LL1432:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp1595; void* _temp1597; struct Cyc_Absyn_Tqual _temp1599; struct _tuple4
_temp1593= Cyc_Absynpp_to_tms( tq, t); _LL1600: _temp1599= _temp1593.f1; goto
_LL1598; _LL1598: _temp1597= _temp1593.f2; goto _LL1596; _LL1596: _temp1595=
_temp1593.f3; goto _LL1594; _LL1594: _temp1595= Cyc_List_imp_rev( _temp1595);
if( _temp1595 ==  0? dopt ==  0: 0){ return({ struct Cyc_PP_Doc* _temp1601[ 2u];
_temp1601[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1597); _temp1601[ 0u]= Cyc_Absynpp_tqual2doc(
_temp1599); Cyc_PP_cat( _tag_arr( _temp1601, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ return({ struct Cyc_PP_Doc* _temp1602[ 4u]; _temp1602[ 3u]= Cyc_Absynpp_dtms2doc(
dopt ==  0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( dopt))->v, _temp1595); _temp1602[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp1602[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1597);
_temp1602[ 0u]= Cyc_Absynpp_tqual2doc( _temp1599); Cyc_PP_cat( _tag_arr(
_temp1602, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width !=  0){ return({ struct Cyc_PP_Doc*
_temp1603[ 5u]; _temp1603[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1603[ 3u]= Cyc_Absynpp_atts2doc( f->attributes); _temp1603[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->width)); _temp1603[ 1u]= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); _temp1603[ 0u]= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp1604=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1604->v=( void*) Cyc_PP_textptr(
f->name); _temp1604;})); Cyc_PP_cat( _tag_arr( _temp1603, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ return({ struct Cyc_PP_Doc* _temp1605[ 3u]; _temp1605[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1605[ 1u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1605[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1606=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1606->v=( void*) Cyc_PP_textptr( f->name); _temp1606;}));
Cyc_PP_cat( _tag_arr( _temp1605, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,
_tag_arr("", sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1607[ 4u];
_temp1607[ 3u]= f->typs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1607[ 2u]= f->tvs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1607[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1607[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1607, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f){ for( 0; tdl !=  0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);({
void* _temp1608[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1608, sizeof( void*), 0u));});}} struct
_tagged_arr Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc(
Cyc_PP_seql( _tag_arr("", sizeof( unsigned char), 1u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_arr
Cyc_Absynpp_typ2string( void* t){ return Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72);} struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0* v){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_arr Cyc_Absynpp_prim2string(
void* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_prim2doc( p), 72);} struct
_tagged_arr Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc( p), 72);} struct _tagged_arr Cyc_Absynpp_scope2string( void*
sc){ void* _temp1609= sc; _LL1611: if( _temp1609 == ( void*) Cyc_Absyn_Static){
goto _LL1612;} else{ goto _LL1613;} _LL1613: if( _temp1609 == ( void*) Cyc_Absyn_Public){
goto _LL1614;} else{ goto _LL1615;} _LL1615: if( _temp1609 == ( void*) Cyc_Absyn_Extern){
goto _LL1616;} else{ goto _LL1617;} _LL1617: if( _temp1609 == ( void*) Cyc_Absyn_ExternC){
goto _LL1618;} else{ goto _LL1619;} _LL1619: if( _temp1609 == ( void*) Cyc_Absyn_Abstract){
goto _LL1620;} else{ goto _LL1610;} _LL1612: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1614: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1616: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1618:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1620: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1610:;}
