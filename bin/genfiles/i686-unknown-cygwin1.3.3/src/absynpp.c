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
_tuple0* _temp362; struct Cyc_Absyn_Tuniondecl* _temp364; struct Cyc_Absyn_Tuniondecl
_temp366; int _temp367; struct _tuple0* _temp369; _LL354: if(*(( int*) _temp352)
==  Cyc_Absyn_UnknownTunion){ _LL359: _temp358=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp352)->f1; _LL363: _temp362= _temp358.name; goto _LL361; _LL361: _temp360=
_temp358.is_xtunion; goto _LL355;} else{ goto _LL356;} _LL356: if(*(( int*)
_temp352) ==  Cyc_Absyn_KnownTunion){ _LL365: _temp364=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp352)->f1; _temp366=* _temp364; _LL370: _temp369= _temp366.name; goto _LL368;
_LL368: _temp367= _temp366.is_xtunion; goto _LL357;} else{ goto _LL353;} _LL355:
_temp369= _temp362; _temp367= _temp360; goto _LL357; _LL357: { struct Cyc_PP_Doc*
_temp371= Cyc_PP_text( _temp367? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u));{ void* _temp372= Cyc_Tcutil_compress(
_temp305); _LL374: if( _temp372 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL375;}
else{ goto _LL376;} _LL376: goto _LL377; _LL375: s=({ struct Cyc_PP_Doc*
_temp378[ 3u]; _temp378[ 2u]= Cyc_Absynpp_tps2doc( _temp307); _temp378[ 1u]= Cyc_Absynpp_qvar2doc(
_temp369); _temp378[ 0u]= _temp371; Cyc_PP_cat( _tag_arr( _temp378, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL373; _LL377: s=({ struct Cyc_PP_Doc*
_temp379[ 5u]; _temp379[ 4u]= Cyc_Absynpp_tps2doc( _temp307); _temp379[ 3u]= Cyc_Absynpp_qvar2doc(
_temp369); _temp379[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp379[ 1u]= Cyc_Absynpp_typ2doc( _temp305); _temp379[ 0u]= _temp371; Cyc_PP_cat(
_tag_arr( _temp379, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL373; _LL373:;}
goto _LL353;} _LL353:;} goto _LL246; _LL262:{ void* _temp380= _temp315; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp386; int _temp388; struct _tuple0*
_temp390; struct _tuple0* _temp392; struct Cyc_Absyn_Tunionfield* _temp394;
struct Cyc_Absyn_Tunionfield _temp396; struct _tuple0* _temp397; struct Cyc_Absyn_Tuniondecl*
_temp399; struct Cyc_Absyn_Tuniondecl _temp401; int _temp402; struct _tuple0*
_temp404; _LL382: if(*(( int*) _temp380) ==  Cyc_Absyn_UnknownTunionfield){
_LL387: _temp386=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp380)->f1;
_LL393: _temp392= _temp386.tunion_name; goto _LL391; _LL391: _temp390= _temp386.field_name;
goto _LL389; _LL389: _temp388= _temp386.is_xtunion; goto _LL383;} else{ goto
_LL384;} _LL384: if(*(( int*) _temp380) ==  Cyc_Absyn_KnownTunionfield){ _LL400:
_temp399=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp380)->f1; _temp401=*
_temp399; _LL405: _temp404= _temp401.name; goto _LL403; _LL403: _temp402=
_temp401.is_xtunion; goto _LL395; _LL395: _temp394=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp380)->f2; _temp396=* _temp394; _LL398: _temp397= _temp396.name; goto _LL385;}
else{ goto _LL381;} _LL383: _temp404= _temp392; _temp402= _temp388; _temp397=
_temp390; goto _LL385; _LL385: { struct Cyc_PP_Doc* _temp406= Cyc_PP_text(
_temp402? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp407[ 4u]; _temp407[ 3u]=
Cyc_Absynpp_qvar2doc( _temp397); _temp407[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp407[ 1u]= Cyc_Absynpp_qvar2doc( _temp404);
_temp407[ 0u]= _temp406; Cyc_PP_cat( _tag_arr( _temp407, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL381;} _LL381:;} goto _LL246; _LL264: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp408= _temp319; _LL410: if( _temp408 == ( void*)
Cyc_Absyn_Signed){ goto _LL411;} else{ goto _LL412;} _LL412: if( _temp408 == (
void*) Cyc_Absyn_Unsigned){ goto _LL413;} else{ goto _LL409;} _LL411: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL409; _LL413: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL409; _LL409:;}{ void* _temp414= _temp317;
_LL416: if( _temp414 == ( void*) Cyc_Absyn_B1){ goto _LL417;} else{ goto _LL418;}
_LL418: if( _temp414 == ( void*) Cyc_Absyn_B2){ goto _LL419;} else{ goto _LL420;}
_LL420: if( _temp414 == ( void*) Cyc_Absyn_B4){ goto _LL421;} else{ goto _LL422;}
_LL422: if( _temp414 == ( void*) Cyc_Absyn_B8){ goto _LL423;} else{ goto _LL415;}
_LL417:{ void* _temp424= _temp319; _LL426: if( _temp424 == ( void*) Cyc_Absyn_Signed){
goto _LL427;} else{ goto _LL428;} _LL428: if( _temp424 == ( void*) Cyc_Absyn_Unsigned){
goto _LL429;} else{ goto _LL425;} _LL427: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL425; _LL429: goto _LL425; _LL425:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL415; _LL419: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL415; _LL421: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL415; _LL423: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL415; _LL415:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp432; _temp432.tag= Cyc_Std_String_pa; _temp432.f1=( struct _tagged_arr) ts;{
struct Cyc_Std_String_pa_struct _temp431; _temp431.tag= Cyc_Std_String_pa;
_temp431.f1=( struct _tagged_arr) sns;{ void* _temp430[ 2u]={& _temp431,&
_temp432}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp430, sizeof( void*), 2u));}}})); goto _LL246;} _LL266: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL246; _LL268: s= Cyc_PP_text(
_tag_arr("double", sizeof( unsigned char), 7u)); goto _LL246; _LL270: s=({
struct Cyc_PP_Doc* _temp433[ 2u]; _temp433[ 1u]= Cyc_Absynpp_args2doc( _temp321);
_temp433[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp433, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL272:
if( _temp325 ==  0){ s=({ struct Cyc_PP_Doc* _temp434[ 2u]; _temp434[ 1u]= Cyc_Absynpp_tps2doc(
_temp323); _temp434[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char),
8u)); Cyc_PP_cat( _tag_arr( _temp434, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ s=({ struct Cyc_PP_Doc* _temp435[ 3u]; _temp435[ 2u]= Cyc_Absynpp_tps2doc(
_temp323); _temp435[ 1u]= _temp325 ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp325)); _temp435[ 0u]= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp435, sizeof( struct Cyc_PP_Doc*),
3u));});} goto _LL246; _LL274: if( _temp329 ==  0){ s=({ struct Cyc_PP_Doc*
_temp436[ 2u]; _temp436[ 1u]= Cyc_Absynpp_tps2doc( _temp327); _temp436[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp436,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp437[ 3u];
_temp437[ 2u]= Cyc_Absynpp_tps2doc( _temp327); _temp437[ 1u]= _temp329 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp329));
_temp437[ 0u]= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp437, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL276:
s=({ struct Cyc_PP_Doc* _temp438[ 3u]; _temp438[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp438[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp331)); _temp438[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp438, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL278: s=({ struct Cyc_PP_Doc* _temp439[ 3u]; _temp439[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp439[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp333)); _temp439[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp439,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL280: s=({ struct Cyc_PP_Doc*
_temp440[ 2u]; _temp440[ 1u]= Cyc_Absynpp_qvar2doc( _temp335); _temp440[ 0u]=
Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp440, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL282: s=({
struct Cyc_PP_Doc* _temp441[ 2u]; _temp441[ 1u]= Cyc_Absynpp_tps2doc( _temp339);
_temp441[ 0u]= Cyc_Absynpp_qvar2doc( _temp341); Cyc_PP_cat( _tag_arr( _temp441,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL284: s=({ struct Cyc_PP_Doc*
_temp442[ 3u]; _temp442[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof( unsigned char),
2u)); _temp442[ 1u]= Cyc_Absynpp_rgn2doc( _temp343); _temp442[ 0u]= Cyc_PP_text(
_tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp442, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL286: s= Cyc_Absynpp_rgn2doc(
t); goto _LL246; _LL288: s=({ struct Cyc_PP_Doc* _temp443[ 3u]; _temp443[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp443[ 1u]= Cyc_Absynpp_typ2doc(
_temp345); _temp443[ 0u]= Cyc_PP_text( _tag_arr("regions(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp443, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL290: goto _LL292; _LL292: s= Cyc_Absynpp_eff2doc( t);
goto _LL246; _LL246:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
Cyc_Core_Opt* vo){ return vo ==  0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct _tuple8{
void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc( struct
_tuple8* cmp){ struct _tuple8 _temp446; void* _temp447; void* _temp449; struct
_tuple8* _temp444= cmp; _temp446=* _temp444; _LL450: _temp449= _temp446.f1; goto
_LL448; _LL448: _temp447= _temp446.f2; goto _LL445; _LL445: return({ struct Cyc_PP_Doc*
_temp451[ 3u]; _temp451[ 2u]= Cyc_Absynpp_rgn2doc( _temp447); _temp451[ 1u]= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); _temp451[ 0u]= Cyc_Absynpp_rgn2doc(
_temp449); Cyc_PP_cat( _tag_arr( _temp451, sizeof( struct Cyc_PP_Doc*), 3u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc( struct Cyc_List_List* po){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 ==  0? 0:({ struct Cyc_Core_Opt* _temp452=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp452->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp452;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp453=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp453=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp453,({
struct Cyc_List_List* _temp454=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp454->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp454->tl= 0; _temp454;}));} else{ if(
cyc_varargs !=  0){ struct Cyc_PP_Doc* _temp455=({ struct Cyc_PP_Doc* _temp457[
3u]; _temp457[ 2u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp458=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp458->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp458->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp458->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp458;})); _temp457[ 1u]=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp457[ 0u]= Cyc_PP_text( _tag_arr("...", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp457, sizeof( struct Cyc_PP_Doc*), 3u));}); _temp453=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp453,({
struct Cyc_List_List* _temp456=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp456->hd=( void*) _temp455; _temp456->tl= 0;
_temp456;}));}}{ struct Cyc_PP_Doc* _temp459= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp453); if( effopt !=  0){ _temp459=({ struct
Cyc_PP_Doc* _temp460[ 3u]; _temp460[ 2u]= Cyc_Absynpp_eff2doc(( void*)(( struct
Cyc_Core_Opt*) _check_null( effopt))->v); _temp460[ 1u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp460[ 0u]= _temp459; Cyc_PP_cat( _tag_arr(
_temp460, sizeof( struct Cyc_PP_Doc*), 3u));});} if( rgn_po !=  0){ _temp459=({
struct Cyc_PP_Doc* _temp461[ 3u]; _temp461[ 2u]= Cyc_Absynpp_rgnpo2doc( rgn_po);
_temp461[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char), 2u)); _temp461[
0u]= _temp459; Cyc_PP_cat( _tag_arr( _temp461, sizeof( struct Cyc_PP_Doc*), 3u));});}
return({ struct Cyc_PP_Doc* _temp462[ 3u]; _temp462[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp462[ 1u]= _temp459; _temp462[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp462,
sizeof( struct Cyc_PP_Doc*), 3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp463=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp463->f1=({ struct Cyc_Core_Opt*
_temp464=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp464->v=( void*)(* arg).f1; _temp464;}); _temp463->f2=(* arg).f2; _temp463->f3=(*
arg).f3; _temp463;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp465= 0; int match;{ void*
_temp466=(* q).f1; struct Cyc_List_List* _temp474; struct Cyc_List_List*
_temp476; _LL468: if( _temp466 == ( void*) Cyc_Absyn_Loc_n){ goto _LL469;} else{
goto _LL470;} _LL470: if(( unsigned int) _temp466 >  1u?*(( int*) _temp466) == 
Cyc_Absyn_Rel_n: 0){ _LL475: _temp474=(( struct Cyc_Absyn_Rel_n_struct*)
_temp466)->f1; goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int)
_temp466 >  1u?*(( int*) _temp466) ==  Cyc_Absyn_Abs_n: 0){ _LL477: _temp476=((
struct Cyc_Absyn_Abs_n_struct*) _temp466)->f1; goto _LL473;} else{ goto _LL467;}
_LL469: _temp474= 0; goto _LL471; _LL471: match= 0; _temp465= _temp474; goto
_LL467; _LL473: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_Std_strptrcmp, _temp476, Cyc_Absynpp_curr_namespace):
0; _temp465=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp478=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp478->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp478->tl= _temp476;
_temp478;}): _temp476; goto _LL467; _LL467:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp465,({
struct Cyc_List_List* _temp479=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp479->hd=( void*)(* q).f2; _temp479->tl= 0; _temp479;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp465,({ struct Cyc_List_List* _temp480=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp480->hd=( void*)(* q).f2; _temp480->tl= 0;
_temp480;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp481=(* v).f1; struct Cyc_List_List* _temp491; struct Cyc_List_List*
_temp493; _LL483: if( _temp481 == ( void*) Cyc_Absyn_Loc_n){ goto _LL484;} else{
goto _LL485;} _LL485: if(( unsigned int) _temp481 >  1u?*(( int*) _temp481) == 
Cyc_Absyn_Rel_n: 0){ _LL492: _temp491=(( struct Cyc_Absyn_Rel_n_struct*)
_temp481)->f1; if( _temp491 ==  0){ goto _LL486;} else{ goto _LL487;}} else{
goto _LL487;} _LL487: if(( unsigned int) _temp481 >  1u?*(( int*) _temp481) == 
Cyc_Absyn_Abs_n: 0){ _LL494: _temp493=(( struct Cyc_Absyn_Abs_n_struct*)
_temp481)->f1; goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL484:
goto _LL486; _LL486: return Cyc_Absynpp_var2doc((* v).f2); _LL488: if((( int(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_strptrcmp, _temp493, Cyc_Absynpp_curr_namespace)
==  0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL490;} _LL490:
return({ struct Cyc_PP_Doc* _temp495[ 2u]; _temp495[ 1u]= Cyc_Absynpp_qvar2doc(
v); _temp495[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof(
unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp495, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL482:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp496=(
void*) e->r; void* _temp572; struct Cyc_Absyn_Exp* _temp574; struct Cyc_Absyn_Exp*
_temp576; _LL498: if(*(( int*) _temp496) ==  Cyc_Absyn_Const_e){ goto _LL499;}
else{ goto _LL500;} _LL500: if(*(( int*) _temp496) ==  Cyc_Absyn_Var_e){ goto
_LL501;} else{ goto _LL502;} _LL502: if(*(( int*) _temp496) ==  Cyc_Absyn_UnknownId_e){
goto _LL503;} else{ goto _LL504;} _LL504: if(*(( int*) _temp496) ==  Cyc_Absyn_Primop_e){
_LL573: _temp572=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp496)->f1;
goto _LL505;} else{ goto _LL506;} _LL506: if(*(( int*) _temp496) ==  Cyc_Absyn_AssignOp_e){
goto _LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp496) ==  Cyc_Absyn_Increment_e){
goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp496) ==  Cyc_Absyn_Conditional_e){
goto _LL511;} else{ goto _LL512;} _LL512: if(*(( int*) _temp496) ==  Cyc_Absyn_SeqExp_e){
goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp496) ==  Cyc_Absyn_UnknownCall_e){
goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp496) ==  Cyc_Absyn_FnCall_e){
goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp496) ==  Cyc_Absyn_Throw_e){
goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp496) ==  Cyc_Absyn_NoInstantiate_e){
_LL575: _temp574=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp496)->f1;
goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp496) ==  Cyc_Absyn_Instantiate_e){
_LL577: _temp576=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp496)->f1; goto
_LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp496) ==  Cyc_Absyn_Cast_e){
goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp496) ==  Cyc_Absyn_New_e){
goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp496) ==  Cyc_Absyn_Address_e){
goto _LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp496) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp496) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL533;} else{ goto _LL534;} _LL534: if(*(( int*) _temp496) ==  Cyc_Absyn_Offsetof_e){
goto _LL535;} else{ goto _LL536;} _LL536: if(*(( int*) _temp496) ==  Cyc_Absyn_Gentyp_e){
goto _LL537;} else{ goto _LL538;} _LL538: if(*(( int*) _temp496) ==  Cyc_Absyn_Deref_e){
goto _LL539;} else{ goto _LL540;} _LL540: if(*(( int*) _temp496) ==  Cyc_Absyn_StructMember_e){
goto _LL541;} else{ goto _LL542;} _LL542: if(*(( int*) _temp496) ==  Cyc_Absyn_StructArrow_e){
goto _LL543;} else{ goto _LL544;} _LL544: if(*(( int*) _temp496) ==  Cyc_Absyn_Subscript_e){
goto _LL545;} else{ goto _LL546;} _LL546: if(*(( int*) _temp496) ==  Cyc_Absyn_Tuple_e){
goto _LL547;} else{ goto _LL548;} _LL548: if(*(( int*) _temp496) ==  Cyc_Absyn_CompoundLit_e){
goto _LL549;} else{ goto _LL550;} _LL550: if(*(( int*) _temp496) ==  Cyc_Absyn_Array_e){
goto _LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp496) ==  Cyc_Absyn_Comprehension_e){
goto _LL553;} else{ goto _LL554;} _LL554: if(*(( int*) _temp496) ==  Cyc_Absyn_Struct_e){
goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp496) ==  Cyc_Absyn_AnonStruct_e){
goto _LL557;} else{ goto _LL558;} _LL558: if(*(( int*) _temp496) ==  Cyc_Absyn_Tunion_e){
goto _LL559;} else{ goto _LL560;} _LL560: if(*(( int*) _temp496) ==  Cyc_Absyn_Enum_e){
goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp496) ==  Cyc_Absyn_Malloc_e){
goto _LL563;} else{ goto _LL564;} _LL564: if(*(( int*) _temp496) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp496) ==  Cyc_Absyn_StmtExp_e){
goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp496) ==  Cyc_Absyn_Codegen_e){
goto _LL569;} else{ goto _LL570;} _LL570: if(*(( int*) _temp496) ==  Cyc_Absyn_Fill_e){
goto _LL571;} else{ goto _LL497;} _LL499: return 10000; _LL501: return 10000;
_LL503: return 10000; _LL505: { void* _temp578= _temp572; _LL580: if( _temp578
== ( void*) Cyc_Absyn_Plus){ goto _LL581;} else{ goto _LL582;} _LL582: if(
_temp578 == ( void*) Cyc_Absyn_Times){ goto _LL583;} else{ goto _LL584;} _LL584:
if( _temp578 == ( void*) Cyc_Absyn_Minus){ goto _LL585;} else{ goto _LL586;}
_LL586: if( _temp578 == ( void*) Cyc_Absyn_Div){ goto _LL587;} else{ goto _LL588;}
_LL588: if( _temp578 == ( void*) Cyc_Absyn_Mod){ goto _LL589;} else{ goto _LL590;}
_LL590: if( _temp578 == ( void*) Cyc_Absyn_Eq){ goto _LL591;} else{ goto _LL592;}
_LL592: if( _temp578 == ( void*) Cyc_Absyn_Neq){ goto _LL593;} else{ goto _LL594;}
_LL594: if( _temp578 == ( void*) Cyc_Absyn_Gt){ goto _LL595;} else{ goto _LL596;}
_LL596: if( _temp578 == ( void*) Cyc_Absyn_Lt){ goto _LL597;} else{ goto _LL598;}
_LL598: if( _temp578 == ( void*) Cyc_Absyn_Gte){ goto _LL599;} else{ goto _LL600;}
_LL600: if( _temp578 == ( void*) Cyc_Absyn_Lte){ goto _LL601;} else{ goto _LL602;}
_LL602: if( _temp578 == ( void*) Cyc_Absyn_Not){ goto _LL603;} else{ goto _LL604;}
_LL604: if( _temp578 == ( void*) Cyc_Absyn_Bitnot){ goto _LL605;} else{ goto
_LL606;} _LL606: if( _temp578 == ( void*) Cyc_Absyn_Bitand){ goto _LL607;} else{
goto _LL608;} _LL608: if( _temp578 == ( void*) Cyc_Absyn_Bitor){ goto _LL609;}
else{ goto _LL610;} _LL610: if( _temp578 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL611;} else{ goto _LL612;} _LL612: if( _temp578 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL613;} else{ goto _LL614;} _LL614: if( _temp578 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL615;} else{ goto _LL616;} _LL616: if( _temp578 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL617;} else{ goto _LL618;} _LL618: if( _temp578 == ( void*) Cyc_Absyn_Size){
goto _LL619;} else{ goto _LL579;} _LL581: return 100; _LL583: return 110; _LL585:
return 100; _LL587: return 110; _LL589: return 110; _LL591: return 70; _LL593:
return 70; _LL595: return 80; _LL597: return 80; _LL599: return 80; _LL601:
return 80; _LL603: return 130; _LL605: return 130; _LL607: return 60; _LL609:
return 40; _LL611: return 50; _LL613: return 90; _LL615: return 80; _LL617:
return 80; _LL619: return 140; _LL579:;} _LL507: return 20; _LL509: return 130;
_LL511: return 30; _LL513: return 10; _LL515: return 140; _LL517: return 140;
_LL519: return 130; _LL521: return Cyc_Absynpp_exp_prec( _temp574); _LL523:
return Cyc_Absynpp_exp_prec( _temp576); _LL525: return 120; _LL527: return 15;
_LL529: return 130; _LL531: return 130; _LL533: return 130; _LL535: return 130;
_LL537: return 130; _LL539: return 130; _LL541: return 140; _LL543: return 140;
_LL545: return 140; _LL547: return 150; _LL549: goto _LL551; _LL551: goto _LL553;
_LL553: goto _LL555; _LL555: goto _LL557; _LL557: goto _LL559; _LL559: goto
_LL561; _LL561: goto _LL563; _LL563: return 140; _LL565: return 140; _LL567:
return 10000; _LL569: return 140; _LL571: return 140; _LL497:;} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp620=(
void*) e->r; void* _temp696; struct _tuple0* _temp698; struct _tuple0* _temp700;
struct Cyc_List_List* _temp702; void* _temp704; struct Cyc_Absyn_Exp* _temp706;
struct Cyc_Core_Opt* _temp708; struct Cyc_Absyn_Exp* _temp710; void* _temp712;
struct Cyc_Absyn_Exp* _temp714; struct Cyc_Absyn_Exp* _temp716; struct Cyc_Absyn_Exp*
_temp718; struct Cyc_Absyn_Exp* _temp720; struct Cyc_Absyn_Exp* _temp722; struct
Cyc_Absyn_Exp* _temp724; struct Cyc_List_List* _temp726; struct Cyc_Absyn_Exp*
_temp728; struct Cyc_List_List* _temp730; struct Cyc_Absyn_Exp* _temp732; struct
Cyc_Absyn_Exp* _temp734; struct Cyc_Absyn_Exp* _temp736; struct Cyc_Absyn_Exp*
_temp738; struct Cyc_Absyn_Exp* _temp740; void* _temp742; struct Cyc_Absyn_Exp*
_temp744; struct Cyc_Absyn_Exp* _temp746; struct Cyc_Absyn_Exp* _temp748; void*
_temp750; struct Cyc_Absyn_Exp* _temp752; struct _tagged_arr* _temp754; void*
_temp756; void* _temp758; struct Cyc_Absyn_Exp* _temp760; struct _tagged_arr*
_temp762; struct Cyc_Absyn_Exp* _temp764; struct _tagged_arr* _temp766; struct
Cyc_Absyn_Exp* _temp768; struct Cyc_Absyn_Exp* _temp770; struct Cyc_Absyn_Exp*
_temp772; struct Cyc_List_List* _temp774; struct Cyc_List_List* _temp776; struct
_tuple1* _temp778; struct Cyc_List_List* _temp780; struct Cyc_Absyn_Exp*
_temp782; struct Cyc_Absyn_Exp* _temp784; struct Cyc_Absyn_Vardecl* _temp786;
struct Cyc_List_List* _temp788; struct _tuple0* _temp790; struct Cyc_List_List*
_temp792; struct Cyc_Absyn_Tunionfield* _temp794; struct Cyc_List_List* _temp796;
struct _tuple0* _temp798; void* _temp800; struct Cyc_Absyn_Exp* _temp802; struct
Cyc_List_List* _temp804; struct Cyc_Core_Opt* _temp806; struct Cyc_Absyn_Stmt*
_temp808; struct Cyc_Absyn_Fndecl* _temp810; struct Cyc_Absyn_Exp* _temp812;
_LL622: if(*(( int*) _temp620) ==  Cyc_Absyn_Const_e){ _LL697: _temp696=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp620)->f1; goto _LL623;} else{ goto _LL624;}
_LL624: if(*(( int*) _temp620) ==  Cyc_Absyn_Var_e){ _LL699: _temp698=(( struct
Cyc_Absyn_Var_e_struct*) _temp620)->f1; goto _LL625;} else{ goto _LL626;} _LL626:
if(*(( int*) _temp620) ==  Cyc_Absyn_UnknownId_e){ _LL701: _temp700=(( struct
Cyc_Absyn_UnknownId_e_struct*) _temp620)->f1; goto _LL627;} else{ goto _LL628;}
_LL628: if(*(( int*) _temp620) ==  Cyc_Absyn_Primop_e){ _LL705: _temp704=( void*)((
struct Cyc_Absyn_Primop_e_struct*) _temp620)->f1; goto _LL703; _LL703: _temp702=((
struct Cyc_Absyn_Primop_e_struct*) _temp620)->f2; goto _LL629;} else{ goto
_LL630;} _LL630: if(*(( int*) _temp620) ==  Cyc_Absyn_AssignOp_e){ _LL711:
_temp710=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp620)->f1; goto _LL709;
_LL709: _temp708=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp620)->f2; goto
_LL707; _LL707: _temp706=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp620)->f3;
goto _LL631;} else{ goto _LL632;} _LL632: if(*(( int*) _temp620) ==  Cyc_Absyn_Increment_e){
_LL715: _temp714=(( struct Cyc_Absyn_Increment_e_struct*) _temp620)->f1; goto
_LL713; _LL713: _temp712=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp620)->f2; goto _LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp620)
==  Cyc_Absyn_Conditional_e){ _LL721: _temp720=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp620)->f1; goto _LL719; _LL719: _temp718=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp620)->f2; goto _LL717; _LL717: _temp716=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp620)->f3; goto _LL635;} else{ goto _LL636;} _LL636: if(*(( int*) _temp620)
==  Cyc_Absyn_SeqExp_e){ _LL725: _temp724=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp620)->f1; goto _LL723; _LL723: _temp722=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp620)->f2; goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp620)
==  Cyc_Absyn_UnknownCall_e){ _LL729: _temp728=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp620)->f1; goto _LL727; _LL727: _temp726=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp620)->f2; goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp620)
==  Cyc_Absyn_FnCall_e){ _LL733: _temp732=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp620)->f1; goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp620)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp620)
==  Cyc_Absyn_Throw_e){ _LL735: _temp734=(( struct Cyc_Absyn_Throw_e_struct*)
_temp620)->f1; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp620)
==  Cyc_Absyn_NoInstantiate_e){ _LL737: _temp736=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp620)->f1; goto _LL645;} else{ goto _LL646;} _LL646: if(*(( int*) _temp620)
==  Cyc_Absyn_Instantiate_e){ _LL739: _temp738=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp620)->f1; goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp620)
==  Cyc_Absyn_Cast_e){ _LL743: _temp742=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp620)->f1; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_Cast_e_struct*)
_temp620)->f2; goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp620)
==  Cyc_Absyn_Address_e){ _LL745: _temp744=(( struct Cyc_Absyn_Address_e_struct*)
_temp620)->f1; goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp620)
==  Cyc_Absyn_New_e){ _LL749: _temp748=(( struct Cyc_Absyn_New_e_struct*)
_temp620)->f1; goto _LL747; _LL747: _temp746=(( struct Cyc_Absyn_New_e_struct*)
_temp620)->f2; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp620)
==  Cyc_Absyn_Sizeoftyp_e){ _LL751: _temp750=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp620)->f1; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp620)
==  Cyc_Absyn_Sizeofexp_e){ _LL753: _temp752=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp620)->f1; goto _LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp620)
==  Cyc_Absyn_Offsetof_e){ _LL757: _temp756=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp620)->f1; goto _LL755; _LL755: _temp754=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp620)->f2; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp620)
==  Cyc_Absyn_Gentyp_e){ _LL759: _temp758=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*)
_temp620)->f1; goto _LL661;} else{ goto _LL662;} _LL662: if(*(( int*) _temp620)
==  Cyc_Absyn_Deref_e){ _LL761: _temp760=(( struct Cyc_Absyn_Deref_e_struct*)
_temp620)->f1; goto _LL663;} else{ goto _LL664;} _LL664: if(*(( int*) _temp620)
==  Cyc_Absyn_StructMember_e){ _LL765: _temp764=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp620)->f1; goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp620)->f2; goto _LL665;} else{ goto _LL666;} _LL666: if(*(( int*) _temp620)
==  Cyc_Absyn_StructArrow_e){ _LL769: _temp768=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp620)->f1; goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp620)->f2; goto _LL667;} else{ goto _LL668;} _LL668: if(*(( int*) _temp620)
==  Cyc_Absyn_Subscript_e){ _LL773: _temp772=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp620)->f1; goto _LL771; _LL771: _temp770=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp620)->f2; goto _LL669;} else{ goto _LL670;} _LL670: if(*(( int*) _temp620)
==  Cyc_Absyn_Tuple_e){ _LL775: _temp774=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp620)->f1; goto _LL671;} else{ goto _LL672;} _LL672: if(*(( int*) _temp620)
==  Cyc_Absyn_CompoundLit_e){ _LL779: _temp778=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp620)->f1; goto _LL777; _LL777: _temp776=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp620)->f2; goto _LL673;} else{ goto _LL674;} _LL674: if(*(( int*) _temp620)
==  Cyc_Absyn_Array_e){ _LL781: _temp780=(( struct Cyc_Absyn_Array_e_struct*)
_temp620)->f1; goto _LL675;} else{ goto _LL676;} _LL676: if(*(( int*) _temp620)
==  Cyc_Absyn_Comprehension_e){ _LL787: _temp786=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp620)->f1; goto _LL785; _LL785: _temp784=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp620)->f2; goto _LL783; _LL783: _temp782=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp620)->f3; goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp620)
==  Cyc_Absyn_Struct_e){ _LL791: _temp790=(( struct Cyc_Absyn_Struct_e_struct*)
_temp620)->f1; goto _LL789; _LL789: _temp788=(( struct Cyc_Absyn_Struct_e_struct*)
_temp620)->f3; goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp620)
==  Cyc_Absyn_AnonStruct_e){ _LL793: _temp792=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp620)->f2; goto _LL681;} else{ goto _LL682;} _LL682: if(*(( int*) _temp620)
==  Cyc_Absyn_Tunion_e){ _LL797: _temp796=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp620)->f3; goto _LL795; _LL795: _temp794=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp620)->f5; goto _LL683;} else{ goto _LL684;} _LL684: if(*(( int*) _temp620)
==  Cyc_Absyn_Enum_e){ _LL799: _temp798=(( struct Cyc_Absyn_Enum_e_struct*)
_temp620)->f1; goto _LL685;} else{ goto _LL686;} _LL686: if(*(( int*) _temp620)
==  Cyc_Absyn_Malloc_e){ _LL803: _temp802=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp620)->f1; goto _LL801; _LL801: _temp800=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp620)->f2; goto _LL687;} else{ goto _LL688;} _LL688: if(*(( int*) _temp620)
==  Cyc_Absyn_UnresolvedMem_e){ _LL807: _temp806=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp620)->f1; goto _LL805; _LL805: _temp804=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp620)->f2; goto _LL689;} else{ goto _LL690;} _LL690: if(*(( int*) _temp620)
==  Cyc_Absyn_StmtExp_e){ _LL809: _temp808=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp620)->f1; goto _LL691;} else{ goto _LL692;} _LL692: if(*(( int*) _temp620)
==  Cyc_Absyn_Codegen_e){ _LL811: _temp810=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp620)->f1; goto _LL693;} else{ goto _LL694;} _LL694: if(*(( int*) _temp620)
==  Cyc_Absyn_Fill_e){ _LL813: _temp812=(( struct Cyc_Absyn_Fill_e_struct*)
_temp620)->f1; goto _LL695;} else{ goto _LL621;} _LL623: s= Cyc_Absynpp_cnst2doc(
_temp696); goto _LL621; _LL625: _temp700= _temp698; goto _LL627; _LL627: s= Cyc_Absynpp_qvar2doc(
_temp700); goto _LL621; _LL629: s= Cyc_Absynpp_primapp2doc( myprec, _temp704,
_temp702); goto _LL621; _LL631: s=({ struct Cyc_PP_Doc* _temp814[ 5u]; _temp814[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp706); _temp814[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp814[ 2u]= _temp708 ==  0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp708))->v); _temp814[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp814[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp710); Cyc_PP_cat( _tag_arr( _temp814, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL621; _LL633: { struct Cyc_PP_Doc* _temp815= Cyc_Absynpp_exp2doc_prec(
myprec, _temp714);{ void* _temp816= _temp712; _LL818: if( _temp816 == ( void*)
Cyc_Absyn_PreInc){ goto _LL819;} else{ goto _LL820;} _LL820: if( _temp816 == (
void*) Cyc_Absyn_PreDec){ goto _LL821;} else{ goto _LL822;} _LL822: if( _temp816
== ( void*) Cyc_Absyn_PostInc){ goto _LL823;} else{ goto _LL824;} _LL824: if(
_temp816 == ( void*) Cyc_Absyn_PostDec){ goto _LL825;} else{ goto _LL817;}
_LL819: s=({ struct Cyc_PP_Doc* _temp826[ 2u]; _temp826[ 1u]= _temp815; _temp826[
0u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp826, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL817; _LL821: s=({
struct Cyc_PP_Doc* _temp827[ 2u]; _temp827[ 1u]= _temp815; _temp827[ 0u]= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp827,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL817; _LL823: s=({ struct Cyc_PP_Doc*
_temp828[ 2u]; _temp828[ 1u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char),
3u)); _temp828[ 0u]= _temp815; Cyc_PP_cat( _tag_arr( _temp828, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL817; _LL825: s=({ struct Cyc_PP_Doc* _temp829[ 2u];
_temp829[ 1u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
_temp829[ 0u]= _temp815; Cyc_PP_cat( _tag_arr( _temp829, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL817; _LL817:;} goto _LL621;} _LL635:{ struct _tuple8 _temp831=({
struct _tuple8 _temp830; _temp830.f1=( void*) _temp718->r; _temp830.f2=( void*)
_temp716->r; _temp830;}); _LL833: goto _LL834; _LL834: s=({ struct Cyc_PP_Doc*
_temp835[ 5u]; _temp835[ 4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp716);
_temp835[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof( unsigned char), 4u));
_temp835[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp718); _temp835[ 1u]= Cyc_PP_text(
_tag_arr(" ? ", sizeof( unsigned char), 4u)); _temp835[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp720); Cyc_PP_cat( _tag_arr( _temp835, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL832; _LL832:;} goto _LL621; _LL637: s=({ struct Cyc_PP_Doc*
_temp836[ 5u]; _temp836[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp836[ 3u]= Cyc_Absynpp_exp2doc( _temp722); _temp836[ 2u]= Cyc_PP_text(
_tag_arr(", ", sizeof( unsigned char), 3u)); _temp836[ 1u]= Cyc_Absynpp_exp2doc(
_temp724); _temp836[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp836, sizeof( struct Cyc_PP_Doc*), 5u));}); goto
_LL621; _LL639: s=({ struct Cyc_PP_Doc* _temp837[ 4u]; _temp837[ 3u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp837[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp726); _temp837[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp837[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp728); Cyc_PP_cat(
_tag_arr( _temp837, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL621; _LL641: s=({
struct Cyc_PP_Doc* _temp838[ 4u]; _temp838[ 3u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp838[ 2u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp730); _temp838[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
_temp838[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp732); Cyc_PP_cat( _tag_arr(
_temp838, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL621; _LL643: s=({ struct
Cyc_PP_Doc* _temp839[ 2u]; _temp839[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp734); _temp839[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char),
7u)); Cyc_PP_cat( _tag_arr( _temp839, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL621; _LL645: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp736); goto _LL621;
_LL647: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp738); goto _LL621; _LL649: s=({
struct Cyc_PP_Doc* _temp840[ 4u]; _temp840[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp740); _temp840[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp840[ 1u]= Cyc_Absynpp_typ2doc( _temp742); _temp840[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp840, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL621; _LL651: s=({ struct
Cyc_PP_Doc* _temp841[ 2u]; _temp841[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp744); _temp841[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp841, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL621; _LL653: if( _temp748 ==  0){ s=({ struct Cyc_PP_Doc* _temp842[ 2u];
_temp842[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp746); _temp842[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp842,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp843[ 4u];
_temp843[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp746); _temp843[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp843[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp748)); _temp843[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp843,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL621; _LL655: s=({ struct Cyc_PP_Doc*
_temp844[ 3u]; _temp844[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp844[ 1u]= Cyc_Absynpp_typ2doc( _temp750); _temp844[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp844,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL621; _LL657: s=({ struct Cyc_PP_Doc*
_temp845[ 3u]; _temp845[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp845[ 1u]= Cyc_Absynpp_exp2doc( _temp752); _temp845[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp845,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL621; _LL659: s=({ struct Cyc_PP_Doc*
_temp846[ 5u]; _temp846[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp846[ 3u]= Cyc_PP_textptr( _temp754); _temp846[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp846[ 1u]= Cyc_Absynpp_typ2doc(
_temp756); _temp846[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp846, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL621; _LL661: s=({ struct Cyc_PP_Doc* _temp847[ 3u]; _temp847[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp847[ 1u]= Cyc_Absynpp_typ2doc(
_temp758); _temp847[ 0u]= Cyc_PP_text( _tag_arr("__gen(", sizeof( unsigned char),
7u)); Cyc_PP_cat( _tag_arr( _temp847, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL621; _LL663: s=({ struct Cyc_PP_Doc* _temp848[ 2u]; _temp848[ 1u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp760); _temp848[ 0u]= Cyc_PP_text( _tag_arr("*", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp848, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL621; _LL665: s=({ struct Cyc_PP_Doc* _temp849[ 3u]; _temp849[ 2u]=
Cyc_PP_textptr( _temp762); _temp849[ 1u]= Cyc_PP_text( _tag_arr(".", sizeof(
unsigned char), 2u)); _temp849[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp764);
Cyc_PP_cat( _tag_arr( _temp849, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL621; _LL667: s=({ struct Cyc_PP_Doc* _temp850[ 3u]; _temp850[ 2u]= Cyc_PP_textptr(
_temp766); _temp850[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof( unsigned char), 3u));
_temp850[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp768); Cyc_PP_cat( _tag_arr(
_temp850, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL621; _LL669: s=({ struct
Cyc_PP_Doc* _temp851[ 4u]; _temp851[ 3u]= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); _temp851[ 2u]= Cyc_Absynpp_exp2doc( _temp770); _temp851[ 1u]=
Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); _temp851[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp772); Cyc_PP_cat( _tag_arr( _temp851, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL621; _LL671: s=({ struct Cyc_PP_Doc* _temp852[ 3u]; _temp852[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp852[ 1u]= Cyc_Absynpp_exps2doc_prec(
20, _temp774); _temp852[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp852, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL621; _LL673: s=({ struct Cyc_PP_Doc* _temp853[ 4u]; _temp853[ 3u]= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp776)); _temp853[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp853[ 1u]= Cyc_Absynpp_typ2doc((* _temp778).f3);
_temp853[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp853, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL621; _LL675: s=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp780)); goto _LL621; _LL677: s=({ struct Cyc_PP_Doc*
_temp854[ 7u]; _temp854[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp854[ 5u]= Cyc_Absynpp_exp2doc( _temp782); _temp854[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp854[ 3u]= Cyc_Absynpp_exp2doc(
_temp784); _temp854[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp854[ 1u]= Cyc_PP_text(*(* _temp786->name).f2); _temp854[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp854,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL621; _LL679: s=({ struct Cyc_PP_Doc*
_temp855[ 2u]; _temp855[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp788));
_temp855[ 0u]= Cyc_Absynpp_qvar2doc( _temp790); Cyc_PP_cat( _tag_arr( _temp855,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL621; _LL681: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp792)); goto _LL621; _LL683: if( _temp796 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp794->name);} else{ s=({ struct Cyc_PP_Doc* _temp856[ 2u]; _temp856[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp796)); _temp856[ 0u]= Cyc_Absynpp_qvar2doc( _temp794->name);
Cyc_PP_cat( _tag_arr( _temp856, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL621; _LL685: s= Cyc_Absynpp_qvar2doc( _temp798); goto _LL621; _LL687: if(
_temp802 ==  0){ s=({ struct Cyc_PP_Doc* _temp857[ 3u]; _temp857[ 2u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp857[ 1u]= Cyc_Absynpp_typ2doc(
_temp800); _temp857[ 0u]= Cyc_PP_text( _tag_arr("malloc(sizeof(", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp857, sizeof( struct Cyc_PP_Doc*),
3u));});} else{ s=({ struct Cyc_PP_Doc* _temp858[ 6u]; _temp858[ 5u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp858[ 4u]= Cyc_Absynpp_typ2doc(
_temp800); _temp858[ 3u]= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char),
8u)); _temp858[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp858[ 1u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp802)); _temp858[ 0u]= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp858, sizeof( struct Cyc_PP_Doc*),
6u));});} goto _LL621; _LL689: s= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp804)); goto _LL621; _LL691: s=({ struct Cyc_PP_Doc* _temp859[ 3u]; _temp859[
2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char), 4u)); _temp859[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp808)); _temp859[ 0u]= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat( _tag_arr( _temp859,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL621; _LL693: s=({ struct Cyc_PP_Doc*
_temp860[ 3u]; _temp860[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp860[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl*
_temp861=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp861->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp862=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp862[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp863; _temp863.tag= Cyc_Absyn_Fn_d;
_temp863.f1= _temp810; _temp863;}); _temp862;})); _temp861->loc= e->loc;
_temp861;}))); _temp860[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp860, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL621; _LL695: s=({ struct Cyc_PP_Doc* _temp864[ 3u]; _temp864[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp864[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp812)); _temp864[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp864, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL621; _LL621:;} if( inprec >=  myprec){ s=({ struct Cyc_PP_Doc*
_temp865[ 3u]; _temp865[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp865[ 1u]= s; _temp865[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp865, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp866= d; struct Cyc_Absyn_Exp* _temp872; struct _tagged_arr* _temp874;
_LL868: if(*(( int*) _temp866) ==  Cyc_Absyn_ArrayElement){ _LL873: _temp872=((
struct Cyc_Absyn_ArrayElement_struct*) _temp866)->f1; goto _LL869;} else{ goto
_LL870;} _LL870: if(*(( int*) _temp866) ==  Cyc_Absyn_FieldName){ _LL875:
_temp874=(( struct Cyc_Absyn_FieldName_struct*) _temp866)->f1; goto _LL871;}
else{ goto _LL867;} _LL869: return({ struct Cyc_PP_Doc* _temp876[ 3u]; _temp876[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp876[ 1u]= Cyc_Absynpp_exp2doc(
_temp872); _temp876[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp876, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL871:
return({ struct Cyc_PP_Doc* _temp877[ 2u]; _temp877[ 1u]= Cyc_PP_textptr(
_temp874); _temp877[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp877, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL867:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 ==  0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp878[ 2u]; _temp878[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp878[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp878, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp879= c;
unsigned char _temp897; void* _temp899; short _temp901; void* _temp903; int
_temp905; void* _temp907; int _temp909; void* _temp911; long long _temp913; void*
_temp915; struct _tagged_arr _temp917; struct _tagged_arr _temp919; _LL881: if((
unsigned int) _temp879 >  1u?*(( int*) _temp879) ==  Cyc_Absyn_Char_c: 0){
_LL900: _temp899=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp879)->f1; goto
_LL898; _LL898: _temp897=(( struct Cyc_Absyn_Char_c_struct*) _temp879)->f2; goto
_LL882;} else{ goto _LL883;} _LL883: if(( unsigned int) _temp879 >  1u?*(( int*)
_temp879) ==  Cyc_Absyn_Short_c: 0){ _LL904: _temp903=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp879)->f1; goto _LL902; _LL902: _temp901=(( struct Cyc_Absyn_Short_c_struct*)
_temp879)->f2; goto _LL884;} else{ goto _LL885;} _LL885: if(( unsigned int)
_temp879 >  1u?*(( int*) _temp879) ==  Cyc_Absyn_Int_c: 0){ _LL908: _temp907=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp879)->f1; if( _temp907 == ( void*)
Cyc_Absyn_Signed){ goto _LL906;} else{ goto _LL887;} _LL906: _temp905=(( struct
Cyc_Absyn_Int_c_struct*) _temp879)->f2; goto _LL886;} else{ goto _LL887;} _LL887:
if(( unsigned int) _temp879 >  1u?*(( int*) _temp879) ==  Cyc_Absyn_Int_c: 0){
_LL912: _temp911=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp879)->f1; if(
_temp911 == ( void*) Cyc_Absyn_Unsigned){ goto _LL910;} else{ goto _LL889;}
_LL910: _temp909=(( struct Cyc_Absyn_Int_c_struct*) _temp879)->f2; goto _LL888;}
else{ goto _LL889;} _LL889: if(( unsigned int) _temp879 >  1u?*(( int*) _temp879)
==  Cyc_Absyn_LongLong_c: 0){ _LL916: _temp915=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp879)->f1; goto _LL914; _LL914: _temp913=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp879)->f2; goto _LL890;} else{ goto _LL891;} _LL891: if(( unsigned int)
_temp879 >  1u?*(( int*) _temp879) ==  Cyc_Absyn_Float_c: 0){ _LL918: _temp917=((
struct Cyc_Absyn_Float_c_struct*) _temp879)->f1; goto _LL892;} else{ goto _LL893;}
_LL893: if( _temp879 == ( void*) Cyc_Absyn_Null_c){ goto _LL894;} else{ goto
_LL895;} _LL895: if(( unsigned int) _temp879 >  1u?*(( int*) _temp879) ==  Cyc_Absyn_String_c:
0){ _LL920: _temp919=(( struct Cyc_Absyn_String_c_struct*) _temp879)->f1; goto
_LL896;} else{ goto _LL880;} _LL882: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp922; _temp922.tag= Cyc_Std_String_pa;
_temp922.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp897);{ void*
_temp921[ 1u]={& _temp922}; Cyc_Std_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp921, sizeof( void*), 1u));}})); _LL884:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp924;
_temp924.tag= Cyc_Std_Int_pa; _temp924.f1=( int)(( unsigned int)(( int) _temp901));{
void* _temp923[ 1u]={& _temp924}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp923, sizeof( void*), 1u));}})); _LL886:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp926;
_temp926.tag= Cyc_Std_Int_pa; _temp926.f1=( int)(( unsigned int) _temp905);{
void* _temp925[ 1u]={& _temp926}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp925, sizeof( void*), 1u));}})); _LL888:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp928;
_temp928.tag= Cyc_Std_Int_pa; _temp928.f1=( unsigned int) _temp909;{ void*
_temp927[ 1u]={& _temp928}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp927, sizeof( void*), 1u));}})); _LL890:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL892: return Cyc_PP_text( _temp917); _LL894: return Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); _LL896: return({ struct Cyc_PP_Doc*
_temp929[ 3u]; _temp929[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp929[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp919));
_temp929[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp929, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL880:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == ( void*) Cyc_Absyn_Size){ if(
es ==  0? 1:(( struct Cyc_List_List*) _check_null( es))->tl !=  0){( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp930=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp930[ 0]=({ struct Cyc_Core_Failure_struct
_temp931; _temp931.tag= Cyc_Core_Failure; _temp931.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp933; _temp933.tag= Cyc_Std_String_pa;
_temp933.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp932[
1u]={& _temp933}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp932, sizeof( void*), 1u));}});
_temp931;}); _temp930;}));}{ struct Cyc_PP_Doc* _temp934= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
return({ struct Cyc_PP_Doc* _temp935[ 2u]; _temp935[ 1u]= Cyc_PP_text( _tag_arr(".size",
sizeof( unsigned char), 6u)); _temp935[ 0u]= _temp934; Cyc_PP_cat( _tag_arr(
_temp935, sizeof( struct Cyc_PP_Doc*), 2u));});}} else{ struct Cyc_List_List* ds=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds ==  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp936=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp936[ 0]=({ struct Cyc_Core_Failure_struct _temp937; _temp937.tag= Cyc_Core_Failure;
_temp937.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp939;
_temp939.tag= Cyc_Std_String_pa; _temp939.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp938[ 1u]={& _temp939}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp938, sizeof( void*), 1u));}});
_temp937;}); _temp936;}));} else{ if((( struct Cyc_List_List*) _check_null( ds))->tl
==  0){ return({ struct Cyc_PP_Doc* _temp940[ 3u]; _temp940[ 2u]=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; _temp940[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp940[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp940, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl
!=  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp941=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp941[ 0]=({ struct Cyc_Core_Failure_struct _temp942; _temp942.tag= Cyc_Core_Failure;
_temp942.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp944;
_temp944.tag= Cyc_Std_String_pa; _temp944.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp943[ 1u]={& _temp944}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp943, sizeof( void*), 1u));}});
_temp942;}); _temp941;}));} else{ return({ struct Cyc_PP_Doc* _temp945[ 5u];
_temp945[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->hd; _temp945[ 3u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp945[ 2u]= ps; _temp945[ 1u]=
Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp945[ 0u]=( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd; Cyc_PP_cat(
_tag_arr( _temp945, sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct
_tagged_arr Cyc_Absynpp_prim2str( void* p){ void* _temp946= p; _LL948: if(
_temp946 == ( void*) Cyc_Absyn_Plus){ goto _LL949;} else{ goto _LL950;} _LL950:
if( _temp946 == ( void*) Cyc_Absyn_Times){ goto _LL951;} else{ goto _LL952;}
_LL952: if( _temp946 == ( void*) Cyc_Absyn_Minus){ goto _LL953;} else{ goto
_LL954;} _LL954: if( _temp946 == ( void*) Cyc_Absyn_Div){ goto _LL955;} else{
goto _LL956;} _LL956: if( _temp946 == ( void*) Cyc_Absyn_Mod){ goto _LL957;}
else{ goto _LL958;} _LL958: if( _temp946 == ( void*) Cyc_Absyn_Eq){ goto _LL959;}
else{ goto _LL960;} _LL960: if( _temp946 == ( void*) Cyc_Absyn_Neq){ goto _LL961;}
else{ goto _LL962;} _LL962: if( _temp946 == ( void*) Cyc_Absyn_Gt){ goto _LL963;}
else{ goto _LL964;} _LL964: if( _temp946 == ( void*) Cyc_Absyn_Lt){ goto _LL965;}
else{ goto _LL966;} _LL966: if( _temp946 == ( void*) Cyc_Absyn_Gte){ goto _LL967;}
else{ goto _LL968;} _LL968: if( _temp946 == ( void*) Cyc_Absyn_Lte){ goto _LL969;}
else{ goto _LL970;} _LL970: if( _temp946 == ( void*) Cyc_Absyn_Not){ goto _LL971;}
else{ goto _LL972;} _LL972: if( _temp946 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL973;} else{ goto _LL974;} _LL974: if( _temp946 == ( void*) Cyc_Absyn_Bitand){
goto _LL975;} else{ goto _LL976;} _LL976: if( _temp946 == ( void*) Cyc_Absyn_Bitor){
goto _LL977;} else{ goto _LL978;} _LL978: if( _temp946 == ( void*) Cyc_Absyn_Bitxor){
goto _LL979;} else{ goto _LL980;} _LL980: if( _temp946 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL981;} else{ goto _LL982;} _LL982: if( _temp946 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL983;} else{ goto _LL984;} _LL984: if( _temp946 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL985;} else{ goto _LL986;} _LL986: if( _temp946 == ( void*) Cyc_Absyn_Size){
goto _LL987;} else{ goto _LL947;} _LL949: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL951: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL953: return _tag_arr("-", sizeof( unsigned char), 2u); _LL955: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL957: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL959: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL961: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL963: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL965: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL967: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL969: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL971: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL973: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL975: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL977: return _tag_arr("|", sizeof( unsigned char), 2u); _LL979: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL981: return _tag_arr("<<", sizeof(
unsigned char), 3u); _LL983: return _tag_arr(">>", sizeof( unsigned char), 3u);
_LL985: return _tag_arr(">>>", sizeof( unsigned char), 4u); _LL987: return
_tag_arr("size", sizeof( unsigned char), 5u); _LL947:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc(
void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc(
struct _tuple9* t){ if((* t).f1 ==  0){ return Cyc_Absynpp_exp2doc((* t).f2);}
else{ return({ struct Cyc_PP_Doc* _temp988[ 3u]; _temp988[ 2u]= Cyc_Absynpp_exp2doc((*
t).f2); _temp988[ 1u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp988[ 0u]= Cyc_PP_textptr(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v); Cyc_PP_cat( _tag_arr( _temp988, sizeof( struct Cyc_PP_Doc*),
3u));});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp989=( void*) s->r; _LL991: if(( unsigned int) _temp989 >  1u?*(( int*)
_temp989) ==  Cyc_Absyn_Decl_s: 0){ goto _LL992;} else{ goto _LL993;} _LL993:
goto _LL994; _LL992: return 1; _LL994: return 0; _LL990:;} struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void*
_temp995=( void*) st->r; struct Cyc_Absyn_Exp* _temp1039; struct Cyc_Absyn_Stmt*
_temp1041; struct Cyc_Absyn_Stmt* _temp1043; struct Cyc_Absyn_Exp* _temp1045;
struct Cyc_Absyn_Stmt* _temp1047; struct Cyc_Absyn_Stmt* _temp1049; struct Cyc_Absyn_Exp*
_temp1051; struct Cyc_Absyn_Stmt* _temp1053; struct _tuple2 _temp1055; struct
Cyc_Absyn_Exp* _temp1057; struct _tagged_arr* _temp1059; struct Cyc_Absyn_Stmt*
_temp1061; struct _tuple2 _temp1063; struct Cyc_Absyn_Exp* _temp1065; struct
_tuple2 _temp1067; struct Cyc_Absyn_Exp* _temp1069; struct Cyc_Absyn_Exp*
_temp1071; struct Cyc_List_List* _temp1073; struct Cyc_Absyn_Exp* _temp1075;
struct Cyc_List_List* _temp1077; struct Cyc_Absyn_Exp* _temp1079; struct Cyc_List_List*
_temp1081; struct Cyc_List_List* _temp1083; struct Cyc_Absyn_Stmt* _temp1085;
struct Cyc_Absyn_Decl* _temp1087; struct Cyc_Absyn_Stmt* _temp1089; struct Cyc_Absyn_Stmt*
_temp1091; struct Cyc_Absyn_Stmt* _temp1093; struct _tagged_arr* _temp1095;
struct _tuple2 _temp1097; struct Cyc_Absyn_Exp* _temp1099; struct Cyc_Absyn_Stmt*
_temp1101; struct Cyc_List_List* _temp1103; struct Cyc_Absyn_Stmt* _temp1105;
struct Cyc_Absyn_Stmt* _temp1107; struct Cyc_Absyn_Vardecl* _temp1109; struct
Cyc_Absyn_Tvar* _temp1111; _LL997: if( _temp995 == ( void*) Cyc_Absyn_Skip_s){
goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int) _temp995 >  1u?*((
int*) _temp995) ==  Cyc_Absyn_Exp_s: 0){ _LL1040: _temp1039=(( struct Cyc_Absyn_Exp_s_struct*)
_temp995)->f1; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(( unsigned int)
_temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_Seq_s: 0){ _LL1044: _temp1043=((
struct Cyc_Absyn_Seq_s_struct*) _temp995)->f1; goto _LL1042; _LL1042: _temp1041=((
struct Cyc_Absyn_Seq_s_struct*) _temp995)->f2; goto _LL1002;} else{ goto _LL1003;}
_LL1003: if(( unsigned int) _temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_Return_s:
0){ _LL1046: _temp1045=(( struct Cyc_Absyn_Return_s_struct*) _temp995)->f1; goto
_LL1004;} else{ goto _LL1005;} _LL1005: if(( unsigned int) _temp995 >  1u?*((
int*) _temp995) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1052: _temp1051=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp995)->f1; goto _LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp995)->f2; goto _LL1048; _LL1048: _temp1047=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp995)->f3; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(( unsigned int)
_temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_While_s: 0){ _LL1056: _temp1055=((
struct Cyc_Absyn_While_s_struct*) _temp995)->f1; _LL1058: _temp1057= _temp1055.f1;
goto _LL1054; _LL1054: _temp1053=(( struct Cyc_Absyn_While_s_struct*) _temp995)->f2;
goto _LL1008;} else{ goto _LL1009;} _LL1009: if(( unsigned int) _temp995 >  1u?*((
int*) _temp995) ==  Cyc_Absyn_Break_s: 0){ goto _LL1010;} else{ goto _LL1011;}
_LL1011: if(( unsigned int) _temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_Continue_s:
0){ goto _LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int) _temp995 > 
1u?*(( int*) _temp995) ==  Cyc_Absyn_Goto_s: 0){ _LL1060: _temp1059=(( struct
Cyc_Absyn_Goto_s_struct*) _temp995)->f1; goto _LL1014;} else{ goto _LL1015;}
_LL1015: if(( unsigned int) _temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_For_s:
0){ _LL1072: _temp1071=(( struct Cyc_Absyn_For_s_struct*) _temp995)->f1; goto
_LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_For_s_struct*) _temp995)->f2;
_LL1070: _temp1069= _temp1067.f1; goto _LL1064; _LL1064: _temp1063=(( struct Cyc_Absyn_For_s_struct*)
_temp995)->f3; _LL1066: _temp1065= _temp1063.f1; goto _LL1062; _LL1062:
_temp1061=(( struct Cyc_Absyn_For_s_struct*) _temp995)->f4; goto _LL1016;} else{
goto _LL1017;} _LL1017: if(( unsigned int) _temp995 >  1u?*(( int*) _temp995) == 
Cyc_Absyn_Switch_s: 0){ _LL1076: _temp1075=(( struct Cyc_Absyn_Switch_s_struct*)
_temp995)->f1; goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_Switch_s_struct*)
_temp995)->f2; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int)
_temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_SwitchC_s: 0){ _LL1080:
_temp1079=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp995)->f1; goto _LL1078;
_LL1078: _temp1077=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp995)->f2; goto
_LL1020;} else{ goto _LL1021;} _LL1021: if(( unsigned int) _temp995 >  1u?*((
int*) _temp995) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1082: _temp1081=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp995)->f1; if( _temp1081 ==  0){ goto _LL1022;} else{ goto _LL1023;}} else{
goto _LL1023;} _LL1023: if(( unsigned int) _temp995 >  1u?*(( int*) _temp995) == 
Cyc_Absyn_Fallthru_s: 0){ _LL1084: _temp1083=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp995)->f1; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int)
_temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_Decl_s: 0){ _LL1088: _temp1087=((
struct Cyc_Absyn_Decl_s_struct*) _temp995)->f1; goto _LL1086; _LL1086: _temp1085=((
struct Cyc_Absyn_Decl_s_struct*) _temp995)->f2; goto _LL1026;} else{ goto
_LL1027;} _LL1027: if(( unsigned int) _temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_Cut_s:
0){ _LL1090: _temp1089=(( struct Cyc_Absyn_Cut_s_struct*) _temp995)->f1; goto
_LL1028;} else{ goto _LL1029;} _LL1029: if(( unsigned int) _temp995 >  1u?*((
int*) _temp995) ==  Cyc_Absyn_Splice_s: 0){ _LL1092: _temp1091=(( struct Cyc_Absyn_Splice_s_struct*)
_temp995)->f1; goto _LL1030;} else{ goto _LL1031;} _LL1031: if(( unsigned int)
_temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_Label_s: 0){ _LL1096: _temp1095=((
struct Cyc_Absyn_Label_s_struct*) _temp995)->f1; goto _LL1094; _LL1094:
_temp1093=(( struct Cyc_Absyn_Label_s_struct*) _temp995)->f2; goto _LL1032;}
else{ goto _LL1033;} _LL1033: if(( unsigned int) _temp995 >  1u?*(( int*)
_temp995) ==  Cyc_Absyn_Do_s: 0){ _LL1102: _temp1101=(( struct Cyc_Absyn_Do_s_struct*)
_temp995)->f1; goto _LL1098; _LL1098: _temp1097=(( struct Cyc_Absyn_Do_s_struct*)
_temp995)->f2; _LL1100: _temp1099= _temp1097.f1; goto _LL1034;} else{ goto
_LL1035;} _LL1035: if(( unsigned int) _temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_TryCatch_s:
0){ _LL1106: _temp1105=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp995)->f1;
goto _LL1104; _LL1104: _temp1103=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp995)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int)
_temp995 >  1u?*(( int*) _temp995) ==  Cyc_Absyn_Region_s: 0){ _LL1112:
_temp1111=(( struct Cyc_Absyn_Region_s_struct*) _temp995)->f1; goto _LL1110;
_LL1110: _temp1109=(( struct Cyc_Absyn_Region_s_struct*) _temp995)->f2; goto
_LL1108; _LL1108: _temp1107=(( struct Cyc_Absyn_Region_s_struct*) _temp995)->f3;
goto _LL1038;} else{ goto _LL996;} _LL998: s= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); goto _LL996; _LL1000: s=({ struct Cyc_PP_Doc* _temp1113[ 2u];
_temp1113[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
_temp1113[ 0u]= Cyc_Absynpp_exp2doc( _temp1039); Cyc_PP_cat( _tag_arr( _temp1113,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL996; _LL1002: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1043)){ s=({ struct Cyc_PP_Doc* _temp1114[
6u]; _temp1114[ 5u]= Cyc_Absynpp_is_declaration( _temp1041)?({ struct Cyc_PP_Doc*
_temp1115[ 4u]; _temp1115[ 3u]= Cyc_PP_line_doc(); _temp1115[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1115[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1041)); _temp1115[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1115, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1041); _temp1114[ 4u]= Cyc_PP_line_doc(); _temp1114[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1114[ 2u]= Cyc_PP_line_doc();
_temp1114[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1043)); _temp1114[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1114, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1041)){ s=({ struct Cyc_PP_Doc* _temp1116[ 6u]; _temp1116[ 5u]= Cyc_PP_line_doc();
_temp1116[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1116[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1041)); _temp1116[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1116[ 1u]= Cyc_PP_line_doc();
_temp1116[ 0u]= Cyc_Absynpp_stmt2doc( _temp1043); Cyc_PP_cat( _tag_arr(
_temp1116, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1117=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1117->hd=( void*) _temp1043;
_temp1117->tl=({ struct Cyc_List_List* _temp1118=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1118->hd=( void*) _temp1041;
_temp1118->tl= 0; _temp1118;}); _temp1117;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1119=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1119->hd=( void*) _temp1043;
_temp1119->tl=({ struct Cyc_List_List* _temp1120=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1120->hd=( void*) _temp1041;
_temp1120->tl= 0; _temp1120;}); _temp1119;}));} goto _LL996; _LL1004: if(
_temp1045 ==  0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1121[ 3u]; _temp1121[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1121[ 1u]= _temp1045 ==  0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1045)); _temp1121[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1121, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL996; _LL1006: {
int print_else;{ void* _temp1122=( void*) _temp1047->r; _LL1124: if( _temp1122
== ( void*) Cyc_Absyn_Skip_s){ goto _LL1125;} else{ goto _LL1126;} _LL1126: goto
_LL1127; _LL1125: print_else= 0; goto _LL1123; _LL1127: print_else= 1; goto
_LL1123; _LL1123:;} s=({ struct Cyc_PP_Doc* _temp1128[ 7u]; _temp1128[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1130[ 5u]; _temp1130[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1130[ 3u]= Cyc_PP_line_doc();
_temp1130[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1131[ 2u]; _temp1131[
1u]= Cyc_Absynpp_stmt2doc( _temp1047); _temp1131[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1131, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1130[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1130[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1130, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1128[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1128[ 4u]= Cyc_PP_line_doc(); _temp1128[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1129[ 2u]; _temp1129[ 1u]= Cyc_Absynpp_stmt2doc( _temp1049); _temp1129[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1129, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1128[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1128[ 1u]= Cyc_Absynpp_exp2doc( _temp1051); _temp1128[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1128,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL996;} _LL1008: s=({ struct Cyc_PP_Doc*
_temp1132[ 6u]; _temp1132[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1132[ 4u]= Cyc_PP_line_doc(); _temp1132[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1133[ 2u]; _temp1133[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1053); _temp1133[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1133,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1132[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1132[ 1u]= Cyc_Absynpp_exp2doc( _temp1057);
_temp1132[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1132, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL996; _LL1010: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL996; _LL1012: s= Cyc_PP_text( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL996; _LL1014: s= Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp1135; _temp1135.tag= Cyc_Std_String_pa;
_temp1135.f1=( struct _tagged_arr)* _temp1059;{ void* _temp1134[ 1u]={&
_temp1135}; Cyc_Std_aprintf( _tag_arr("goto %s;", sizeof( unsigned char), 9u),
_tag_arr( _temp1134, sizeof( void*), 1u));}})); goto _LL996; _LL1016: s=({
struct Cyc_PP_Doc* _temp1136[ 10u]; _temp1136[ 9u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1136[ 8u]= Cyc_PP_line_doc(); _temp1136[ 7u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1137[ 2u]; _temp1137[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1061); _temp1137[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1137,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1136[ 6u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1136[ 5u]= Cyc_Absynpp_exp2doc( _temp1065);
_temp1136[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char), 3u));
_temp1136[ 3u]= Cyc_Absynpp_exp2doc( _temp1069); _temp1136[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1136[ 1u]= Cyc_Absynpp_exp2doc(
_temp1071); _temp1136[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1136, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL996; _LL1018: s=({ struct Cyc_PP_Doc* _temp1138[ 7u]; _temp1138[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1138[ 5u]= Cyc_PP_line_doc();
_temp1138[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1073); _temp1138[ 3u]= Cyc_PP_line_doc();
_temp1138[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1138[ 1u]= Cyc_Absynpp_exp2doc( _temp1075); _temp1138[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1138, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL996; _LL1020: s=({
struct Cyc_PP_Doc* _temp1139[ 7u]; _temp1139[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1139[ 5u]= Cyc_PP_line_doc(); _temp1139[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1077); _temp1139[ 3u]= Cyc_PP_line_doc();
_temp1139[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1139[ 1u]= Cyc_Absynpp_exp2doc( _temp1079); _temp1139[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1139, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL996; _LL1022: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL996; _LL1024: s=({
struct Cyc_PP_Doc* _temp1140[ 3u]; _temp1140[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1140[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1083); _temp1140[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1140, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL996; _LL1026: s=({ struct Cyc_PP_Doc* _temp1141[ 3u]; _temp1141[
2u]= Cyc_Absynpp_stmt2doc( _temp1085); _temp1141[ 1u]= Cyc_PP_line_doc();
_temp1141[ 0u]= Cyc_Absynpp_decl2doc( _temp1087); Cyc_PP_cat( _tag_arr(
_temp1141, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL996; _LL1028: s=({
struct Cyc_PP_Doc* _temp1142[ 2u]; _temp1142[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1089)); _temp1142[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1142, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL996; _LL1030: s=({ struct Cyc_PP_Doc* _temp1143[ 2u]; _temp1143[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1091)); _temp1143[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1143, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL996; _LL1032: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1093): 0){ s=({ struct Cyc_PP_Doc* _temp1144[ 6u];
_temp1144[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1144[ 4u]= Cyc_PP_line_doc(); _temp1144[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1093)); _temp1144[ 2u]= Cyc_PP_line_doc(); _temp1144[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1144[ 0u]= Cyc_PP_textptr(
_temp1095); Cyc_PP_cat( _tag_arr( _temp1144, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1145[ 3u]; _temp1145[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1093); _temp1145[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1145[ 0u]= Cyc_PP_textptr( _temp1095); Cyc_PP_cat( _tag_arr(
_temp1145, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL996; _LL1034: s=({
struct Cyc_PP_Doc* _temp1146[ 7u]; _temp1146[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1146[ 5u]= Cyc_Absynpp_exp2doc( _temp1099);
_temp1146[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1146[ 3u]= Cyc_PP_line_doc(); _temp1146[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1101)); _temp1146[ 1u]= Cyc_PP_line_doc(); _temp1146[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1146,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL996; _LL1036: s=({ struct Cyc_PP_Doc*
_temp1147[ 9u]; _temp1147[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1147[ 7u]= Cyc_PP_line_doc(); _temp1147[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1103)); _temp1147[ 5u]= Cyc_PP_line_doc(); _temp1147[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1147[ 3u]= Cyc_PP_line_doc();
_temp1147[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1105)); _temp1147[ 1u]=
Cyc_PP_line_doc(); _temp1147[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1147, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL996; _LL1038: s=({ struct Cyc_PP_Doc* _temp1148[ 9u]; _temp1148[
8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1148[ 7u]=
Cyc_PP_line_doc(); _temp1148[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1107)); _temp1148[ 5u]= Cyc_PP_line_doc(); _temp1148[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1148[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1109->name); _temp1148[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1148[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1111)); _temp1148[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1148, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL996; _LL996:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1149=( void*) p->r;
int _temp1181; void* _temp1183; unsigned char _temp1185; struct _tagged_arr
_temp1187; struct Cyc_Absyn_Vardecl* _temp1189; struct Cyc_List_List* _temp1191;
struct Cyc_Absyn_Pat* _temp1193; struct Cyc_Absyn_Vardecl* _temp1195; struct
_tuple0* _temp1197; struct Cyc_List_List* _temp1199; struct Cyc_List_List*
_temp1201; struct _tuple0* _temp1203; struct Cyc_List_List* _temp1205; struct
Cyc_List_List* _temp1207; struct _tuple0* _temp1209; struct Cyc_List_List*
_temp1211; struct Cyc_List_List* _temp1213; struct Cyc_Absyn_Structdecl*
_temp1215; struct Cyc_Absyn_Enumfield* _temp1217; struct Cyc_List_List*
_temp1219; struct Cyc_List_List* _temp1221; struct Cyc_Absyn_Tunionfield*
_temp1223; _LL1151: if( _temp1149 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1152;}
else{ goto _LL1153;} _LL1153: if( _temp1149 == ( void*) Cyc_Absyn_Null_p){ goto
_LL1154;} else{ goto _LL1155;} _LL1155: if(( unsigned int) _temp1149 >  2u?*((
int*) _temp1149) ==  Cyc_Absyn_Int_p: 0){ _LL1184: _temp1183=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1149)->f1; goto _LL1182; _LL1182: _temp1181=((
struct Cyc_Absyn_Int_p_struct*) _temp1149)->f2; goto _LL1156;} else{ goto
_LL1157;} _LL1157: if(( unsigned int) _temp1149 >  2u?*(( int*) _temp1149) == 
Cyc_Absyn_Char_p: 0){ _LL1186: _temp1185=(( struct Cyc_Absyn_Char_p_struct*)
_temp1149)->f1; goto _LL1158;} else{ goto _LL1159;} _LL1159: if(( unsigned int)
_temp1149 >  2u?*(( int*) _temp1149) ==  Cyc_Absyn_Float_p: 0){ _LL1188:
_temp1187=(( struct Cyc_Absyn_Float_p_struct*) _temp1149)->f1; goto _LL1160;}
else{ goto _LL1161;} _LL1161: if(( unsigned int) _temp1149 >  2u?*(( int*)
_temp1149) ==  Cyc_Absyn_Var_p: 0){ _LL1190: _temp1189=(( struct Cyc_Absyn_Var_p_struct*)
_temp1149)->f1; goto _LL1162;} else{ goto _LL1163;} _LL1163: if(( unsigned int)
_temp1149 >  2u?*(( int*) _temp1149) ==  Cyc_Absyn_Tuple_p: 0){ _LL1192:
_temp1191=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1149)->f1; goto _LL1164;}
else{ goto _LL1165;} _LL1165: if(( unsigned int) _temp1149 >  2u?*(( int*)
_temp1149) ==  Cyc_Absyn_Pointer_p: 0){ _LL1194: _temp1193=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1149)->f1; goto _LL1166;} else{ goto _LL1167;} _LL1167: if(( unsigned int)
_temp1149 >  2u?*(( int*) _temp1149) ==  Cyc_Absyn_Reference_p: 0){ _LL1196:
_temp1195=(( struct Cyc_Absyn_Reference_p_struct*) _temp1149)->f1; goto _LL1168;}
else{ goto _LL1169;} _LL1169: if(( unsigned int) _temp1149 >  2u?*(( int*)
_temp1149) ==  Cyc_Absyn_UnknownId_p: 0){ _LL1198: _temp1197=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1149)->f1; goto _LL1170;} else{ goto _LL1171;} _LL1171: if(( unsigned int)
_temp1149 >  2u?*(( int*) _temp1149) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL1204:
_temp1203=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1149)->f1; goto
_LL1202; _LL1202: _temp1201=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1149)->f2;
goto _LL1200; _LL1200: _temp1199=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1149)->f3; goto _LL1172;} else{ goto _LL1173;} _LL1173: if(( unsigned int)
_temp1149 >  2u?*(( int*) _temp1149) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL1210:
_temp1209=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1149)->f1; goto
_LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1149)->f2; goto _LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1149)->f3; goto _LL1174;} else{ goto _LL1175;} _LL1175: if(( unsigned int)
_temp1149 >  2u?*(( int*) _temp1149) ==  Cyc_Absyn_Struct_p: 0){ _LL1216:
_temp1215=(( struct Cyc_Absyn_Struct_p_struct*) _temp1149)->f1; goto _LL1214;
_LL1214: _temp1213=(( struct Cyc_Absyn_Struct_p_struct*) _temp1149)->f3; goto
_LL1212; _LL1212: _temp1211=(( struct Cyc_Absyn_Struct_p_struct*) _temp1149)->f4;
goto _LL1176;} else{ goto _LL1177;} _LL1177: if(( unsigned int) _temp1149 >  2u?*((
int*) _temp1149) ==  Cyc_Absyn_Enum_p: 0){ _LL1218: _temp1217=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1149)->f2; goto _LL1178;} else{ goto _LL1179;} _LL1179: if(( unsigned int)
_temp1149 >  2u?*(( int*) _temp1149) ==  Cyc_Absyn_Tunion_p: 0){ _LL1224:
_temp1223=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1149)->f2; goto _LL1222;
_LL1222: _temp1221=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1149)->f3; goto
_LL1220; _LL1220: _temp1219=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1149)->f4;
goto _LL1180;} else{ goto _LL1150;} _LL1152: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1150; _LL1154: s= Cyc_PP_text( _tag_arr("NULL",
sizeof( unsigned char), 5u)); goto _LL1150; _LL1156: if( _temp1183 == ( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1226; _temp1226.tag= Cyc_Std_Int_pa; _temp1226.f1=( int)(( unsigned int)
_temp1181);{ void* _temp1225[ 1u]={& _temp1226}; Cyc_Std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp1225, sizeof( void*), 1u));}}));}
else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1228; _temp1228.tag= Cyc_Std_Int_pa; _temp1228.f1=( unsigned int) _temp1181;{
void* _temp1227[ 1u]={& _temp1228}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp1227, sizeof( void*), 1u));}}));} goto
_LL1150; _LL1158: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1230; _temp1230.tag= Cyc_Std_String_pa; _temp1230.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1185);{ void* _temp1229[ 1u]={& _temp1230}; Cyc_Std_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1229, sizeof( void*),
1u));}})); goto _LL1150; _LL1160: s= Cyc_PP_text( _temp1187); goto _LL1150;
_LL1162: s= Cyc_Absynpp_qvar2doc( _temp1189->name); goto _LL1150; _LL1164: s=({
struct Cyc_PP_Doc* _temp1231[ 3u]; _temp1231[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1231[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1191); _temp1231[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1231, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1150; _LL1166: s=({ struct Cyc_PP_Doc* _temp1232[ 2u]; _temp1232[ 1u]=
Cyc_Absynpp_pat2doc( _temp1193); _temp1232[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1232, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1150; _LL1168: s=({ struct Cyc_PP_Doc* _temp1233[
2u]; _temp1233[ 1u]= Cyc_Absynpp_qvar2doc( _temp1195->name); _temp1233[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1233,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1150; _LL1170: s= Cyc_Absynpp_qvar2doc(
_temp1197); goto _LL1150; _LL1172: s=({ struct Cyc_PP_Doc* _temp1234[ 3u];
_temp1234[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1199));
_temp1234[ 1u]= Cyc_Absynpp_tvars2doc( _temp1201); _temp1234[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1203); Cyc_PP_cat( _tag_arr( _temp1234, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1150; _LL1174: s=({ struct Cyc_PP_Doc* _temp1235[ 3u]; _temp1235[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1205)); _temp1235[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1207); _temp1235[ 0u]= Cyc_Absynpp_qvar2doc( _temp1209); Cyc_PP_cat(
_tag_arr( _temp1235, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1150; _LL1176:
s=({ struct Cyc_PP_Doc* _temp1236[ 3u]; _temp1236[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1211)); _temp1236[ 1u]= Cyc_Absynpp_tvars2doc( _temp1213); _temp1236[ 0u]=
_temp1215->name ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1215->name))->v); Cyc_PP_cat( _tag_arr(
_temp1236, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1150; _LL1178: s= Cyc_Absynpp_qvar2doc(
_temp1217->name); goto _LL1150; _LL1180: if( _temp1219 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp1223->name);} else{ s=({ struct Cyc_PP_Doc* _temp1237[ 3u]; _temp1237[ 2u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1219)); _temp1237[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1221); _temp1237[ 0u]= Cyc_Absynpp_qvar2doc( _temp1223->name); Cyc_PP_cat(
_tag_arr( _temp1237, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1150;
_LL1150:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1238[ 2u]; _temp1238[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1238[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1238, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause ==  0?( void*)( c->pattern)->r
== ( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1239[ 2u];
_temp1239[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1240[ 2u]; _temp1240[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1240[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1240, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1239[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1239, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 
0){ return({ struct Cyc_PP_Doc* _temp1241[ 4u]; _temp1241[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1242[ 2u]; _temp1242[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1242[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1242, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1241[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1241[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1241[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1241, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1243[ 6u]; _temp1243[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1244[ 2u]; _temp1244[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1244[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1244, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1243[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1243[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1243[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1243[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1243[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1243,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1247;
struct Cyc_Absyn_Stmt* _temp1248; struct Cyc_Absyn_Exp* _temp1250; struct Cyc_Absyn_SwitchC_clause*
_temp1245= c; _temp1247=* _temp1245; _LL1251: _temp1250= _temp1247.cnst_exp;
goto _LL1249; _LL1249: _temp1248= _temp1247.body; goto _LL1246; _LL1246: if(
_temp1250 ==  0){ return({ struct Cyc_PP_Doc* _temp1252[ 2u]; _temp1252[ 1u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1253[ 2u]; _temp1253[ 1u]= Cyc_Absynpp_stmt2doc(
c->body); _temp1253[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1253,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1252[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1252, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1254[ 4u];
_temp1254[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1255[ 2u]; _temp1255[
1u]= Cyc_Absynpp_stmt2doc( _temp1248); _temp1255[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1255, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1254[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1254[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1250)); _temp1254[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1254,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag ==  0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1256[ 3u]; _temp1256[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1256[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1256[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1256, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1257=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1283; struct Cyc_Absyn_Structdecl* _temp1285;
struct Cyc_Absyn_Uniondecl* _temp1287; struct Cyc_Absyn_Vardecl* _temp1289;
struct Cyc_Absyn_Vardecl _temp1291; struct Cyc_List_List* _temp1292; struct Cyc_Absyn_Exp*
_temp1294; void* _temp1296; struct Cyc_Absyn_Tqual _temp1298; struct _tuple0*
_temp1300; void* _temp1302; struct Cyc_Absyn_Tuniondecl* _temp1304; struct Cyc_Absyn_Tuniondecl
_temp1306; int _temp1307; struct Cyc_Core_Opt* _temp1309; struct Cyc_List_List*
_temp1311; struct _tuple0* _temp1313; void* _temp1315; struct Cyc_Absyn_Exp*
_temp1317; struct Cyc_Absyn_Pat* _temp1319; struct Cyc_List_List* _temp1321;
struct Cyc_Absyn_Enumdecl* _temp1323; struct Cyc_Absyn_Enumdecl _temp1325;
struct Cyc_Core_Opt* _temp1326; struct _tuple0* _temp1328; void* _temp1330;
struct Cyc_Absyn_Typedefdecl* _temp1332; struct Cyc_List_List* _temp1334; struct
_tagged_arr* _temp1336; struct Cyc_List_List* _temp1338; struct _tuple0*
_temp1340; struct Cyc_List_List* _temp1342; _LL1259: if(*(( int*) _temp1257) == 
Cyc_Absyn_Fn_d){ _LL1284: _temp1283=(( struct Cyc_Absyn_Fn_d_struct*) _temp1257)->f1;
goto _LL1260;} else{ goto _LL1261;} _LL1261: if(*(( int*) _temp1257) ==  Cyc_Absyn_Struct_d){
_LL1286: _temp1285=(( struct Cyc_Absyn_Struct_d_struct*) _temp1257)->f1; goto
_LL1262;} else{ goto _LL1263;} _LL1263: if(*(( int*) _temp1257) ==  Cyc_Absyn_Union_d){
_LL1288: _temp1287=(( struct Cyc_Absyn_Union_d_struct*) _temp1257)->f1; goto
_LL1264;} else{ goto _LL1265;} _LL1265: if(*(( int*) _temp1257) ==  Cyc_Absyn_Var_d){
_LL1290: _temp1289=(( struct Cyc_Absyn_Var_d_struct*) _temp1257)->f1; _temp1291=*
_temp1289; _LL1303: _temp1302=( void*) _temp1291.sc; goto _LL1301; _LL1301:
_temp1300= _temp1291.name; goto _LL1299; _LL1299: _temp1298= _temp1291.tq; goto
_LL1297; _LL1297: _temp1296=( void*) _temp1291.type; goto _LL1295; _LL1295:
_temp1294= _temp1291.initializer; goto _LL1293; _LL1293: _temp1292= _temp1291.attributes;
goto _LL1266;} else{ goto _LL1267;} _LL1267: if(*(( int*) _temp1257) ==  Cyc_Absyn_Tunion_d){
_LL1305: _temp1304=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1257)->f1;
_temp1306=* _temp1304; _LL1316: _temp1315=( void*) _temp1306.sc; goto _LL1314;
_LL1314: _temp1313= _temp1306.name; goto _LL1312; _LL1312: _temp1311= _temp1306.tvs;
goto _LL1310; _LL1310: _temp1309= _temp1306.fields; goto _LL1308; _LL1308:
_temp1307= _temp1306.is_xtunion; goto _LL1268;} else{ goto _LL1269;} _LL1269:
if(*(( int*) _temp1257) ==  Cyc_Absyn_Let_d){ _LL1320: _temp1319=(( struct Cyc_Absyn_Let_d_struct*)
_temp1257)->f1; goto _LL1318; _LL1318: _temp1317=(( struct Cyc_Absyn_Let_d_struct*)
_temp1257)->f4; goto _LL1270;} else{ goto _LL1271;} _LL1271: if(*(( int*)
_temp1257) ==  Cyc_Absyn_Letv_d){ _LL1322: _temp1321=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1257)->f1; goto _LL1272;} else{ goto _LL1273;} _LL1273: if(*(( int*)
_temp1257) ==  Cyc_Absyn_Enum_d){ _LL1324: _temp1323=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1257)->f1; _temp1325=* _temp1323; _LL1331: _temp1330=( void*) _temp1325.sc;
goto _LL1329; _LL1329: _temp1328= _temp1325.name; goto _LL1327; _LL1327:
_temp1326= _temp1325.fields; goto _LL1274;} else{ goto _LL1275;} _LL1275: if(*((
int*) _temp1257) ==  Cyc_Absyn_Typedef_d){ _LL1333: _temp1332=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1257)->f1; goto _LL1276;} else{ goto _LL1277;} _LL1277: if(*(( int*)
_temp1257) ==  Cyc_Absyn_Namespace_d){ _LL1337: _temp1336=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1257)->f1; goto _LL1335; _LL1335: _temp1334=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1257)->f2; goto _LL1278;} else{ goto _LL1279;} _LL1279: if(*(( int*)
_temp1257) ==  Cyc_Absyn_Using_d){ _LL1341: _temp1340=(( struct Cyc_Absyn_Using_d_struct*)
_temp1257)->f1; goto _LL1339; _LL1339: _temp1338=(( struct Cyc_Absyn_Using_d_struct*)
_temp1257)->f2; goto _LL1280;} else{ goto _LL1281;} _LL1281: if(*(( int*)
_temp1257) ==  Cyc_Absyn_ExternC_d){ _LL1343: _temp1342=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1257)->f1; goto _LL1282;} else{ goto _LL1258;} _LL1260: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1351=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1351[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1352; _temp1352.tag= Cyc_Absyn_FnType; _temp1352.f1=({ struct Cyc_Absyn_FnInfo
_temp1353; _temp1353.tvars= _temp1283->tvs; _temp1353.effect= _temp1283->effect;
_temp1353.ret_typ=( void*)(( void*) _temp1283->ret_type); _temp1353.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1283->args); _temp1353.c_varargs=
_temp1283->c_varargs; _temp1353.cyc_varargs= _temp1283->cyc_varargs; _temp1353.rgn_po=
_temp1283->rgn_po; _temp1353.attributes= 0; _temp1353;}); _temp1352;});
_temp1351;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1283->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1283->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1283->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1283->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1283->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1348; _temp1348.q_const= 0; _temp1348.q_volatile= 0; _temp1348.q_restrict=
0; _temp1348;}), t,({ struct Cyc_Core_Opt* _temp1349=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1349->v=( void*)({ struct Cyc_PP_Doc*
_temp1350[ 2u]; _temp1350[ 1u]= namedoc; _temp1350[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1350, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1349;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1346[ 4u]; _temp1346[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1346[ 2u]= Cyc_PP_line_doc();
_temp1346[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1347[ 2u]; _temp1347[
1u]= Cyc_Absynpp_stmt2doc( _temp1283->body); _temp1347[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1347, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1346[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1346, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1344[ 4u]; _temp1344[ 3u]= bodydoc; _temp1344[ 2u]= tqtddoc; _temp1344[ 1u]=
scopedoc; _temp1344[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1344, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1345[ 2u]; _temp1345[ 1u]= s; _temp1345[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1345, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1258;}} _LL1262: if(
_temp1285->fields ==  0){ s=({ struct Cyc_PP_Doc* _temp1354[ 5u]; _temp1354[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1354[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1285->tvs); _temp1354[ 2u]= _temp1285->name ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1285->name))->v); _temp1354[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1354[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1285->sc); Cyc_PP_cat( _tag_arr( _temp1354, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1355[ 10u]; _temp1355[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1355[ 8u]= Cyc_Absynpp_atts2doc(
_temp1285->attributes); _temp1355[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1355[ 6u]= Cyc_PP_line_doc(); _temp1355[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1356[ 2u]; _temp1356[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1285->fields))->v);
_temp1356[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1356, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1355[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1355[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1285->tvs);
_temp1355[ 2u]= _temp1285->name ==  0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1285->name))->v); _temp1355[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1355[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1285->sc); Cyc_PP_cat( _tag_arr( _temp1355, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1258; _LL1264: if( _temp1287->fields ==  0){ s=({ struct Cyc_PP_Doc*
_temp1357[ 5u]; _temp1357[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1357[ 3u]= Cyc_Absynpp_tvars2doc( _temp1287->tvs); _temp1357[ 2u]=
_temp1287->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1287->name))->v); _temp1357[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1357[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1287->sc);
Cyc_PP_cat( _tag_arr( _temp1357, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1358[ 10u]; _temp1358[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1358[ 8u]= Cyc_Absynpp_atts2doc( _temp1287->attributes);
_temp1358[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1358[ 6u]= Cyc_PP_line_doc(); _temp1358[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1359[ 2u]; _temp1359[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1287->fields))->v); _temp1359[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1359, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1358[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1358[ 3u]= Cyc_Absynpp_tvars2doc( _temp1287->tvs); _temp1358[ 2u]=
_temp1287->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1287->name))->v); _temp1358[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1358[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1287->sc);
Cyc_PP_cat( _tag_arr( _temp1358, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1258; _LL1266: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1300); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1292); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1360= Cyc_Tcutil_compress( _temp1296); struct Cyc_Absyn_FnInfo
_temp1366; struct Cyc_List_List* _temp1368; _LL1362: if(( unsigned int)
_temp1360 >  4u?*(( int*) _temp1360) ==  Cyc_Absyn_FnType: 0){ _LL1367:
_temp1366=(( struct Cyc_Absyn_FnType_struct*) _temp1360)->f1; _LL1369: _temp1368=
_temp1366.attributes; goto _LL1363;} else{ goto _LL1364;} _LL1364: goto _LL1365;
_LL1363: beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1368); goto _LL1361;
_LL1365: beforenamedoc= Cyc_PP_nil_doc(); goto _LL1361; _LL1361:;} s=({ struct
Cyc_PP_Doc* _temp1370[ 5u]; _temp1370[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); _temp1370[ 3u]= _temp1294 ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1373[ 2u]; _temp1373[ 1u]=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1294)); _temp1373[
0u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp1373, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1370[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1298, _temp1296,({ struct Cyc_Core_Opt* _temp1371=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1371->v=( void*)({ struct Cyc_PP_Doc*
_temp1372[ 2u]; _temp1372[ 1u]= sn; _temp1372[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1372, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1371;}));
_temp1370[ 1u]= Cyc_Absynpp_scope2doc( _temp1302); _temp1370[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1370, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1258;} _LL1268: if( _temp1309 ==  0){ s=({ struct Cyc_PP_Doc*
_temp1374[ 5u]; _temp1374[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1374[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1311); _temp1374[ 2u]=
_temp1307? Cyc_Absynpp_qvar2doc( _temp1313): Cyc_Absynpp_typedef_name2doc(
_temp1313); _temp1374[ 1u]= _temp1307? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1374[ 0u]= Cyc_Absynpp_scope2doc( _temp1315); Cyc_PP_cat( _tag_arr(
_temp1374, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1375[ 8u]; _temp1375[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1375[ 6u]= Cyc_PP_line_doc(); _temp1375[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1376[ 2u]; _temp1376[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1309))->v);
_temp1376[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1376, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1375[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1375[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1311);
_temp1375[ 2u]= _temp1307? Cyc_Absynpp_qvar2doc( _temp1313): Cyc_Absynpp_typedef_name2doc(
_temp1313); _temp1375[ 1u]= _temp1307? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1375[ 0u]= Cyc_Absynpp_scope2doc( _temp1315); Cyc_PP_cat( _tag_arr(
_temp1375, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1258; _LL1270: s=({
struct Cyc_PP_Doc* _temp1377[ 5u]; _temp1377[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1377[ 3u]= Cyc_Absynpp_exp2doc( _temp1317);
_temp1377[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1377[ 1u]= Cyc_Absynpp_pat2doc( _temp1319); _temp1377[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1377,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1258; _LL1272: s=({ struct Cyc_PP_Doc*
_temp1378[ 3u]; _temp1378[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1378[ 1u]= Cyc_Absynpp_ids2doc( _temp1321); _temp1378[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1378,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1258; _LL1274: if( _temp1326 ==  0){
s=({ struct Cyc_PP_Doc* _temp1379[ 4u]; _temp1379[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1379[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1328); _temp1379[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1379[ 0u]= Cyc_Absynpp_scope2doc( _temp1330); Cyc_PP_cat( _tag_arr(
_temp1379, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1380[ 7u]; _temp1380[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1380[ 5u]= Cyc_PP_line_doc(); _temp1380[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1381[ 2u]; _temp1381[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1326))->v);
_temp1381[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1381, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1380[ 3u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1380[ 2u]= Cyc_Absynpp_qvar2doc( _temp1328);
_temp1380[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u));
_temp1380[ 0u]= Cyc_Absynpp_scope2doc( _temp1330); Cyc_PP_cat( _tag_arr(
_temp1380, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1258; _LL1276: s=({
struct Cyc_PP_Doc* _temp1382[ 3u]; _temp1382[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1382[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1383; _temp1383.q_const= 0; _temp1383.q_volatile= 0; _temp1383.q_restrict=
0; _temp1383;}),( void*) _temp1332->defn,({ struct Cyc_Core_Opt* _temp1384=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1384->v=(
void*)({ struct Cyc_PP_Doc* _temp1385[ 2u]; _temp1385[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1332->tvs); _temp1385[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1332->name);
Cyc_PP_cat( _tag_arr( _temp1385, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1384;}));
_temp1382[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1382, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1258; _LL1278: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1336);} s=({ struct Cyc_PP_Doc* _temp1386[ 7u]; _temp1386[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1386[ 5u]= Cyc_PP_line_doc();
_temp1386[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1334); _temp1386[ 3u]= Cyc_PP_line_doc();
_temp1386[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1386[ 1u]= Cyc_PP_textptr( _temp1336); _temp1386[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1386, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1258; _LL1280: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1387[ 7u]; _temp1387[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1387[ 5u]= Cyc_PP_line_doc(); _temp1387[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1338); _temp1387[ 3u]= Cyc_PP_line_doc();
_temp1387[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1387[ 1u]= Cyc_Absynpp_qvar2doc( _temp1340); _temp1387[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1387,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1388[
7u]; _temp1388[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1388[ 5u]= Cyc_PP_line_doc(); _temp1388[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1338); _temp1388[ 3u]= Cyc_PP_line_doc();
_temp1388[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1388[ 1u]= Cyc_Absynpp_qvar2doc( _temp1340); _temp1388[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1388, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1258; _LL1282: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1389[ 5u]; _temp1389[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1389[ 3u]= Cyc_PP_line_doc(); _temp1389[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1342); _temp1389[ 1u]= Cyc_PP_line_doc();
_temp1389[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1389, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1390[ 5u]; _temp1390[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1390[ 3u]= Cyc_PP_line_doc();
_temp1390[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1342); _temp1390[ 1u]= Cyc_PP_line_doc();
_temp1390[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1390, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1258; _LL1258:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1391= sc; _LL1393: if( _temp1391 == ( void*) Cyc_Absyn_Static){
goto _LL1394;} else{ goto _LL1395;} _LL1395: if( _temp1391 == ( void*) Cyc_Absyn_Public){
goto _LL1396;} else{ goto _LL1397;} _LL1397: if( _temp1391 == ( void*) Cyc_Absyn_Extern){
goto _LL1398;} else{ goto _LL1399;} _LL1399: if( _temp1391 == ( void*) Cyc_Absyn_ExternC){
goto _LL1400;} else{ goto _LL1401;} _LL1401: if( _temp1391 == ( void*) Cyc_Absyn_Abstract){
goto _LL1402;} else{ goto _LL1392;} _LL1394: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1396: return Cyc_PP_nil_doc(); _LL1398: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1400: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1402:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1392:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1403= t; struct
Cyc_Absyn_Tvar* _temp1411; struct Cyc_List_List* _temp1413; _LL1405: if((
unsigned int) _temp1403 >  4u?*(( int*) _temp1403) ==  Cyc_Absyn_VarType: 0){
_LL1412: _temp1411=(( struct Cyc_Absyn_VarType_struct*) _temp1403)->f1; goto
_LL1406;} else{ goto _LL1407;} _LL1407: if(( unsigned int) _temp1403 >  4u?*((
int*) _temp1403) ==  Cyc_Absyn_JoinEff: 0){ _LL1414: _temp1413=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1403)->f1; goto _LL1408;} else{ goto _LL1409;} _LL1409: goto _LL1410;
_LL1406: return Cyc_Tcutil_is_temp_tvar( _temp1411); _LL1408: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1413); _LL1410: return 0; _LL1404:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1415= t; struct Cyc_Absyn_Exp* _temp1429; struct Cyc_Absyn_Tqual _temp1431;
void* _temp1433; struct Cyc_Absyn_PtrInfo _temp1435; struct Cyc_Absyn_Conref*
_temp1437; struct Cyc_Absyn_Tqual _temp1439; struct Cyc_Absyn_Conref* _temp1441;
void* _temp1443; void* _temp1445; struct Cyc_Absyn_FnInfo _temp1447; struct Cyc_List_List*
_temp1449; struct Cyc_List_List* _temp1451; struct Cyc_Absyn_VarargInfo*
_temp1453; int _temp1455; struct Cyc_List_List* _temp1457; void* _temp1459;
struct Cyc_Core_Opt* _temp1461; struct Cyc_List_List* _temp1463; int _temp1465;
struct Cyc_Core_Opt* _temp1467; struct Cyc_Core_Opt* _temp1469; struct Cyc_Core_Opt*
_temp1471; struct Cyc_List_List* _temp1473; struct _tuple0* _temp1475; _LL1417:
if(( unsigned int) _temp1415 >  4u?*(( int*) _temp1415) ==  Cyc_Absyn_ArrayType:
0){ _LL1434: _temp1433=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1415)->f1;
goto _LL1432; _LL1432: _temp1431=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1415)->f2; goto _LL1430; _LL1430: _temp1429=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1415)->f3; goto _LL1418;} else{ goto _LL1419;} _LL1419: if(( unsigned int)
_temp1415 >  4u?*(( int*) _temp1415) ==  Cyc_Absyn_PointerType: 0){ _LL1436:
_temp1435=(( struct Cyc_Absyn_PointerType_struct*) _temp1415)->f1; _LL1446:
_temp1445=( void*) _temp1435.elt_typ; goto _LL1444; _LL1444: _temp1443=( void*)
_temp1435.rgn_typ; goto _LL1442; _LL1442: _temp1441= _temp1435.nullable; goto
_LL1440; _LL1440: _temp1439= _temp1435.tq; goto _LL1438; _LL1438: _temp1437=
_temp1435.bounds; goto _LL1420;} else{ goto _LL1421;} _LL1421: if(( unsigned int)
_temp1415 >  4u?*(( int*) _temp1415) ==  Cyc_Absyn_FnType: 0){ _LL1448:
_temp1447=(( struct Cyc_Absyn_FnType_struct*) _temp1415)->f1; _LL1464: _temp1463=
_temp1447.tvars; goto _LL1462; _LL1462: _temp1461= _temp1447.effect; goto
_LL1460; _LL1460: _temp1459=( void*) _temp1447.ret_typ; goto _LL1458; _LL1458:
_temp1457= _temp1447.args; goto _LL1456; _LL1456: _temp1455= _temp1447.c_varargs;
goto _LL1454; _LL1454: _temp1453= _temp1447.cyc_varargs; goto _LL1452; _LL1452:
_temp1451= _temp1447.rgn_po; goto _LL1450; _LL1450: _temp1449= _temp1447.attributes;
goto _LL1422;} else{ goto _LL1423;} _LL1423: if(( unsigned int) _temp1415 >  4u?*((
int*) _temp1415) ==  Cyc_Absyn_Evar: 0){ _LL1470: _temp1469=(( struct Cyc_Absyn_Evar_struct*)
_temp1415)->f1; goto _LL1468; _LL1468: _temp1467=(( struct Cyc_Absyn_Evar_struct*)
_temp1415)->f2; goto _LL1466; _LL1466: _temp1465=(( struct Cyc_Absyn_Evar_struct*)
_temp1415)->f3; goto _LL1424;} else{ goto _LL1425;} _LL1425: if(( unsigned int)
_temp1415 >  4u?*(( int*) _temp1415) ==  Cyc_Absyn_TypedefType: 0){ _LL1476:
_temp1475=(( struct Cyc_Absyn_TypedefType_struct*) _temp1415)->f1; goto _LL1474;
_LL1474: _temp1473=(( struct Cyc_Absyn_TypedefType_struct*) _temp1415)->f2; goto
_LL1472; _LL1472: _temp1471=(( struct Cyc_Absyn_TypedefType_struct*) _temp1415)->f3;
goto _LL1426;} else{ goto _LL1427;} _LL1427: goto _LL1428; _LL1418: { struct Cyc_List_List*
_temp1479; void* _temp1481; struct Cyc_Absyn_Tqual _temp1483; struct _tuple4
_temp1477= Cyc_Absynpp_to_tms( _temp1431, _temp1433); _LL1484: _temp1483=
_temp1477.f1; goto _LL1482; _LL1482: _temp1481= _temp1477.f2; goto _LL1480;
_LL1480: _temp1479= _temp1477.f3; goto _LL1478; _LL1478: { void* tm; if(
_temp1429 ==  0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1485=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1485[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1486; _temp1486.tag= Cyc_Absyn_ConstArray_mod;
_temp1486.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1429); _temp1486;});
_temp1485;});} return({ struct _tuple4 _temp1487; _temp1487.f1= _temp1483;
_temp1487.f2= _temp1481; _temp1487.f3=({ struct Cyc_List_List* _temp1488=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1488->hd=(
void*) tm; _temp1488->tl= _temp1479; _temp1488;}); _temp1487;});}} _LL1420: {
struct Cyc_List_List* _temp1491; void* _temp1493; struct Cyc_Absyn_Tqual
_temp1495; struct _tuple4 _temp1489= Cyc_Absynpp_to_tms( _temp1439, _temp1445);
_LL1496: _temp1495= _temp1489.f1; goto _LL1494; _LL1494: _temp1493= _temp1489.f2;
goto _LL1492; _LL1492: _temp1491= _temp1489.f3; goto _LL1490; _LL1490: { void*
ps;{ struct _tuple8 _temp1498=({ struct _tuple8 _temp1497; _temp1497.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1441))->v; _temp1497.f2=( void*)( Cyc_Absyn_compress_conref( _temp1437))->v;
_temp1497;}); void* _temp1508; void* _temp1510; struct Cyc_Absyn_Exp* _temp1512;
void* _temp1514; int _temp1516; void* _temp1518; void* _temp1520; struct Cyc_Absyn_Exp*
_temp1522; void* _temp1524; int _temp1526; void* _temp1528; void* _temp1530;
_LL1500: _LL1515: _temp1514= _temp1498.f1; if(( unsigned int) _temp1514 >  1u?*((
int*) _temp1514) ==  Cyc_Absyn_Eq_constr: 0){ _LL1517: _temp1516=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1514)->f1; if( _temp1516 ==  1){ goto _LL1509;}
else{ goto _LL1502;}} else{ goto _LL1502;} _LL1509: _temp1508= _temp1498.f2; if((
unsigned int) _temp1508 >  1u?*(( int*) _temp1508) ==  Cyc_Absyn_Eq_constr: 0){
_LL1511: _temp1510=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1508)->f1;
if(( unsigned int) _temp1510 >  1u?*(( int*) _temp1510) ==  Cyc_Absyn_Upper_b: 0){
_LL1513: _temp1512=(( struct Cyc_Absyn_Upper_b_struct*) _temp1510)->f1; goto
_LL1501;} else{ goto _LL1502;}} else{ goto _LL1502;} _LL1502: _LL1525: _temp1524=
_temp1498.f1; if(( unsigned int) _temp1524 >  1u?*(( int*) _temp1524) ==  Cyc_Absyn_Eq_constr:
0){ _LL1527: _temp1526=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1524)->f1;
if( _temp1526 ==  0){ goto _LL1519;} else{ goto _LL1504;}} else{ goto _LL1504;}
_LL1519: _temp1518= _temp1498.f2; if(( unsigned int) _temp1518 >  1u?*(( int*)
_temp1518) ==  Cyc_Absyn_Eq_constr: 0){ _LL1521: _temp1520=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1518)->f1; if(( unsigned int) _temp1520 >  1u?*(( int*) _temp1520) ==  Cyc_Absyn_Upper_b:
0){ _LL1523: _temp1522=(( struct Cyc_Absyn_Upper_b_struct*) _temp1520)->f1; goto
_LL1503;} else{ goto _LL1504;}} else{ goto _LL1504;} _LL1504: _LL1529: _temp1528=
_temp1498.f2; if(( unsigned int) _temp1528 >  1u?*(( int*) _temp1528) ==  Cyc_Absyn_Eq_constr:
0){ _LL1531: _temp1530=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1528)->f1;
if( _temp1530 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1505;} else{ goto
_LL1506;}} else{ goto _LL1506;} _LL1506: goto _LL1507; _LL1501: ps=( void*)({
struct Cyc_Absyn_Nullable_ps_struct* _temp1532=( struct Cyc_Absyn_Nullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp1532[ 0]=({
struct Cyc_Absyn_Nullable_ps_struct _temp1533; _temp1533.tag= Cyc_Absyn_Nullable_ps;
_temp1533.f1= _temp1512; _temp1533;}); _temp1532;}); goto _LL1499; _LL1503: ps=(
void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp1534=( struct Cyc_Absyn_NonNullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp1534[ 0]=({
struct Cyc_Absyn_NonNullable_ps_struct _temp1535; _temp1535.tag= Cyc_Absyn_NonNullable_ps;
_temp1535.f1= _temp1522; _temp1535;}); _temp1534;}); goto _LL1499; _LL1505: ps=(
void*) Cyc_Absyn_TaggedArray_ps; goto _LL1499; _LL1507: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1499; _LL1499:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp1538=( struct Cyc_Absyn_Pointer_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1538[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1539; _temp1539.tag=
Cyc_Absyn_Pointer_mod; _temp1539.f1=( void*) ps; _temp1539.f2=( void*) _temp1443;
_temp1539.f3= tq; _temp1539;}); _temp1538;}); return({ struct _tuple4 _temp1536;
_temp1536.f1= _temp1495; _temp1536.f2= _temp1493; _temp1536.f3=({ struct Cyc_List_List*
_temp1537=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1537->hd=( void*) tm; _temp1537->tl= _temp1491; _temp1537;}); _temp1536;});}}}
_LL1422: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1461 ==  0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1461))->v)){ _temp1461= 0;
_temp1463= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_rewrite_temp_tvar,
_temp1463);}}{ struct Cyc_List_List* _temp1542; void* _temp1544; struct Cyc_Absyn_Tqual
_temp1546; struct _tuple4 _temp1540= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp1459); _LL1547: _temp1546= _temp1540.f1; goto _LL1545; _LL1545: _temp1544=
_temp1540.f2; goto _LL1543; _LL1543: _temp1542= _temp1540.f3; goto _LL1541;
_LL1541: { struct Cyc_List_List* tms= _temp1542; if( _temp1449 !=  0? ! Cyc_Absynpp_to_VC:
0){ tms=({ struct Cyc_List_List* _temp1548=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1548->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp1549=( struct Cyc_Absyn_Attributes_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp1549[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp1550; _temp1550.tag= Cyc_Absyn_Attributes_mod; _temp1550.f1= 0; _temp1550.f2=
_temp1449; _temp1550;}); _temp1549;})); _temp1548->tl= tms; _temp1548;});} tms=({
struct Cyc_List_List* _temp1551=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1551->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp1552=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp1552[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp1553; _temp1553.tag=
Cyc_Absyn_Function_mod; _temp1553.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp1554=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1554[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1555; _temp1555.tag= Cyc_Absyn_WithTypes;
_temp1555.f1= _temp1457; _temp1555.f2= _temp1455; _temp1555.f3= _temp1453;
_temp1555.f4= _temp1461; _temp1555.f5= _temp1451; _temp1555;}); _temp1554;}));
_temp1553;}); _temp1552;})); _temp1551->tl= tms; _temp1551;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1449 !=  0; _temp1449=(( struct Cyc_List_List*) _check_null(
_temp1449))->tl){ void* _temp1556=( void*)(( struct Cyc_List_List*) _check_null(
_temp1449))->hd; _LL1558: if( _temp1556 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1559;} else{ goto _LL1560;} _LL1560: if( _temp1556 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1561;} else{ goto _LL1562;} _LL1562: if( _temp1556 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1563;} else{ goto _LL1564;} _LL1564: goto _LL1565; _LL1559: goto _LL1561;
_LL1561: goto _LL1563; _LL1563: tms=({ struct Cyc_List_List* _temp1566=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1566->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1567=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1567[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1568; _temp1568.tag= Cyc_Absyn_Attributes_mod;
_temp1568.f1= 0; _temp1568.f2=({ struct Cyc_List_List* _temp1569=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1569->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp1449))->hd); _temp1569->tl= 0;
_temp1569;}); _temp1568;}); _temp1567;})); _temp1566->tl= tms; _temp1566;});
goto AfterAtts; _LL1565: goto _LL1557; _LL1557:;}} AfterAtts: if( _temp1463 != 
0){ tms=({ struct Cyc_List_List* _temp1570=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1570->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp1571=( struct Cyc_Absyn_TypeParams_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp1571[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp1572; _temp1572.tag= Cyc_Absyn_TypeParams_mod; _temp1572.f1= _temp1463;
_temp1572.f2= 0; _temp1572.f3= 1; _temp1572;}); _temp1571;})); _temp1570->tl=
tms; _temp1570;});} return({ struct _tuple4 _temp1573; _temp1573.f1= _temp1546;
_temp1573.f2= _temp1544; _temp1573.f3= tms; _temp1573;});}} _LL1424: if(
_temp1467 ==  0){ return({ struct _tuple4 _temp1574; _temp1574.f1= tq; _temp1574.f2=
t; _temp1574.f3= 0; _temp1574;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1467))->v);} _LL1426: if( _temp1471 == 
0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp1575;
_temp1575.f1= tq; _temp1575.f2= t; _temp1575.f3= 0; _temp1575;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1471))->v);}
_LL1428: return({ struct _tuple4 _temp1576; _temp1576.f1= tq; _temp1576.f2= t;
_temp1576.f3= 0; _temp1576;}); _LL1416:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp1579; void* _temp1581; struct Cyc_Absyn_Tqual _temp1583; struct _tuple4
_temp1577= Cyc_Absynpp_to_tms( tq, t); _LL1584: _temp1583= _temp1577.f1; goto
_LL1582; _LL1582: _temp1581= _temp1577.f2; goto _LL1580; _LL1580: _temp1579=
_temp1577.f3; goto _LL1578; _LL1578: _temp1579= Cyc_List_imp_rev( _temp1579);
if( _temp1579 ==  0? dopt ==  0: 0){ return({ struct Cyc_PP_Doc* _temp1585[ 2u];
_temp1585[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1581); _temp1585[ 0u]= Cyc_Absynpp_tqual2doc(
_temp1583); Cyc_PP_cat( _tag_arr( _temp1585, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ return({ struct Cyc_PP_Doc* _temp1586[ 4u]; _temp1586[ 3u]= Cyc_Absynpp_dtms2doc(
dopt ==  0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( dopt))->v, _temp1579); _temp1586[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp1586[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1581);
_temp1586[ 0u]= Cyc_Absynpp_tqual2doc( _temp1583); Cyc_PP_cat( _tag_arr(
_temp1586, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width !=  0){ return({ struct Cyc_PP_Doc*
_temp1587[ 5u]; _temp1587[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1587[ 3u]= Cyc_Absynpp_atts2doc( f->attributes); _temp1587[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->width)); _temp1587[ 1u]= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); _temp1587[ 0u]= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp1588=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1588->v=( void*) Cyc_PP_textptr(
f->name); _temp1588;})); Cyc_PP_cat( _tag_arr( _temp1587, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ return({ struct Cyc_PP_Doc* _temp1589[ 3u]; _temp1589[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1589[ 1u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1589[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1590=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1590->v=( void*) Cyc_PP_textptr( f->name); _temp1590;}));
Cyc_PP_cat( _tag_arr( _temp1589, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,
_tag_arr("", sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1591[ 4u];
_temp1591[ 3u]= f->typs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1591[ 2u]= f->tvs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1591[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1591[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1591, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f){ for( 0; tdl !=  0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);({
void* _temp1592[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1592, sizeof( void*), 0u));});}} struct
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
sc){ void* _temp1593= sc; _LL1595: if( _temp1593 == ( void*) Cyc_Absyn_Static){
goto _LL1596;} else{ goto _LL1597;} _LL1597: if( _temp1593 == ( void*) Cyc_Absyn_Public){
goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1593 == ( void*) Cyc_Absyn_Extern){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1593 == ( void*) Cyc_Absyn_ExternC){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if( _temp1593 == ( void*) Cyc_Absyn_Abstract){
goto _LL1604;} else{ goto _LL1594;} _LL1596: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1598: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1600: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1602:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1604: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1594:;}
