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
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
//  extern struct _RegionHandle _new_region();
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

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
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
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
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
0; static const int Cyc_Absyn_Eq_kb= 0; struct Cyc_Absyn_Eq_kb_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Unknown_kb= 1; struct Cyc_Absyn_Unknown_kb_struct{
int tag; struct Cyc_Core_Opt* f1; } ; static const int Cyc_Absyn_Less_kb= 2;
struct Cyc_Absyn_Less_kb_struct{ int tag; struct Cyc_Core_Opt* f1; void* f2; } ;
struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; void* kind; } ;
static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
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
const int Cyc_Absyn_DoubleType= 6; struct Cyc_Absyn_DoubleType_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_ArrayType= 7; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ;
static const int Cyc_Absyn_FnType= 8; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 9; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static const
int Cyc_Absyn_StructType= 10; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_AnonStructType= 13; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 14;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 15; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
16; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 17; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; static const
int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_IfThenElse_s=
3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; static const int Cyc_Absyn_While_s=
4; struct _tuple2{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
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
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s=
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_compress_kb(
void*); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*); struct Cyc_Buffer_t;
extern int Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2);
extern struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*, struct
_tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
_tagged_arr Cyc_Absynpp_typ2cstring( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_kindbound2string( void*); extern
struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*); extern struct
_tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*); extern struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern struct _tagged_arr
Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl); extern struct
_tagged_arr Cyc_Absynpp_prim2string( void* p); extern struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p); extern struct _tagged_arr Cyc_Absynpp_scope2string(
void* sc); extern struct _tagged_arr Cyc_Absynpp_cyc_string; extern struct
_tagged_arr* Cyc_Absynpp_cyc_stringptr; extern int Cyc_Absynpp_exp_prec( struct
Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char); extern struct _tagged_arr Cyc_Absynpp_string_escape( struct
_tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str( void* p); extern
int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{ struct
Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
extern struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs);
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
_tagged_arr Cyc_Absynpp_kindbound2string( void* c){ void* _temp78= Cyc_Absyn_compress_kb(
c); void* _temp86; void* _temp88; _LL80: if(*(( int*) _temp78) ==  Cyc_Absyn_Eq_kb){
_LL87: _temp86=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp78)->f1; goto
_LL81;} else{ goto _LL82;} _LL82: if(*(( int*) _temp78) ==  Cyc_Absyn_Unknown_kb){
goto _LL83;} else{ goto _LL84;} _LL84: if(*(( int*) _temp78) ==  Cyc_Absyn_Less_kb){
_LL89: _temp88=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp78)->f2; goto
_LL85;} else{ goto _LL79;} _LL81: return Cyc_Absynpp_kind2string( _temp86);
_LL83: return _tag_arr("?", sizeof( unsigned char), 2u); _LL85: return Cyc_Absynpp_kind2string(
_temp88); _LL79:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text(
Cyc_Absynpp_kind2string( k));} struct Cyc_PP_Doc* Cyc_Absynpp_kindbound2doc(
void* c){ return Cyc_PP_text( Cyc_Absynpp_kindbound2string( c));} struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc( struct Cyc_List_List* ts){ return Cyc_PP_egroup( _tag_arr("<",
sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));}
struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void*
_temp90= Cyc_Absyn_compress_kb(( void*) tv->kind); void* _temp100; void*
_temp102; void* _temp104; _LL92: if(*(( int*) _temp90) ==  Cyc_Absyn_Unknown_kb){
goto _LL93;} else{ goto _LL94;} _LL94: if(*(( int*) _temp90) ==  Cyc_Absyn_Eq_kb){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp90)->f1; if(
_temp100 == ( void*) Cyc_Absyn_BoxKind){ goto _LL95;} else{ goto _LL96;}} else{
goto _LL96;} _LL96: if(*(( int*) _temp90) ==  Cyc_Absyn_Less_kb){ _LL103:
_temp102=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp90)->f2; goto _LL97;}
else{ goto _LL98;} _LL98: if(*(( int*) _temp90) ==  Cyc_Absyn_Eq_kb){ _LL105:
_temp104=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp90)->f1; goto _LL99;}
else{ goto _LL91;} _LL93: goto _LL95; _LL95: return Cyc_PP_textptr( tv->name);
_LL97: _temp104= _temp102; goto _LL99; _LL99: return({ struct Cyc_PP_Doc*
_temp106[ 3u]; _temp106[ 2u]= Cyc_Absynpp_kind2doc( _temp104); _temp106[ 1u]=
Cyc_PP_text( _tag_arr("::", sizeof( unsigned char), 3u)); _temp106[ 0u]= Cyc_PP_textptr(
tv->name); Cyc_PP_cat( _tag_arr( _temp106, sizeof( struct Cyc_PP_Doc*), 3u));});
_LL91:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){
return Cyc_PP_egroup( _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_arr* Cyc_Absynpp_get_name(
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
void* att){ void* _temp107= att; _LL109: if( _temp107 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL110;} else{ goto _LL111;} _LL111: if( _temp107 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL112;} else{ goto _LL113;} _LL113: if( _temp107 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL114;} else{ goto _LL115;} _LL115: goto _LL116; _LL110: return Cyc_PP_nil_doc();
_LL112: return Cyc_PP_nil_doc(); _LL114: return Cyc_PP_nil_doc(); _LL116: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL108:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts= atts->tl){ void* _temp117=(
void*) atts->hd; _LL119: if( _temp117 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL120;} else{ goto _LL121;} _LL121: if( _temp117 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL122;} else{ goto _LL123;} _LL123: if( _temp117 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL120: return Cyc_PP_text(
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL122: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL124: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL126: goto _LL118;
_LL118:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 !=  0; atts2= atts2->tl){ void* _temp127=( void*) atts2->hd;
_LL129: if( _temp127 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL130;} else{
goto _LL131;} _LL131: if( _temp127 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL132;}
else{ goto _LL133;} _LL133: if( _temp127 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL134;} else{ goto _LL135;} _LL135: goto _LL136; _LL130: goto _LL128;
_LL132: goto _LL128; _LL134: goto _LL128; _LL136: hasatt= 1; goto _LL128; _LL128:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc* _temp137[ 3u];
_temp137[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp137[
1u]= Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); _temp137[ 0u]= Cyc_PP_text(
_tag_arr("__declspec(", sizeof( unsigned char), 12u)); Cyc_PP_cat( _tag_arr(
_temp137, sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts ==  0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc* _temp138[ 2u]; _temp138[ 1u]= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); _temp138[ 0u]= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp138, sizeof( struct Cyc_PP_Doc*),
2u));});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List* tms){ if( tms
==  0){ return 0;}{ void* _temp139=( void*) tms->hd; _LL141: if(( unsigned int)
_temp139 >  1u?*(( int*) _temp139) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL142;}
else{ goto _LL143;} _LL143: if(( unsigned int) _temp139 >  1u?*(( int*) _temp139)
==  Cyc_Absyn_Attributes_mod: 0){ goto _LL144;} else{ goto _LL145;} _LL145: goto
_LL146; _LL142: return 1; _LL144: if( ! Cyc_Absynpp_to_VC){ return 0;} return
Cyc_Absynpp_next_is_pointer( tms->tl); _LL146: return 0; _LL140:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms ==  0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d, tms->tl); struct Cyc_PP_Doc*
p_rest=({ struct Cyc_PP_Doc* _temp229[ 3u]; _temp229[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp229[ 1u]= rest; _temp229[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp229,
sizeof( struct Cyc_PP_Doc*), 3u));}); void* _temp147=( void*) tms->hd; struct
Cyc_Absyn_Exp* _temp161; void* _temp163; struct Cyc_List_List* _temp165; int
_temp167; struct Cyc_Position_Segment* _temp169; struct Cyc_List_List* _temp171;
struct Cyc_Absyn_Tqual _temp173; void* _temp175; void* _temp177; _LL149: if(
_temp147 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL150;} else{ goto _LL151;}
_LL151: if(( unsigned int) _temp147 >  1u?*(( int*) _temp147) ==  Cyc_Absyn_ConstArray_mod:
0){ _LL162: _temp161=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp147)->f1;
goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int) _temp147 >  1u?*((
int*) _temp147) ==  Cyc_Absyn_Function_mod: 0){ _LL164: _temp163=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp147)->f1; goto _LL154;} else{ goto
_LL155;} _LL155: if(( unsigned int) _temp147 >  1u?*(( int*) _temp147) ==  Cyc_Absyn_Attributes_mod:
0){ _LL166: _temp165=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp147)->f2;
goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int) _temp147 >  1u?*((
int*) _temp147) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL172: _temp171=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp147)->f1; goto _LL170; _LL170: _temp169=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp147)->f2; goto _LL168; _LL168: _temp167=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp147)->f3; goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp147 >  1u?*(( int*) _temp147) ==  Cyc_Absyn_Pointer_mod: 0){ _LL178:
_temp177=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp147)->f1; goto
_LL176; _LL176: _temp175=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp147)->f2; goto _LL174; _LL174: _temp173=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp147)->f3; goto _LL160;} else{ goto _LL148;} _LL150: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc* _temp179[ 2u]; _temp179[ 1u]=
Cyc_PP_text( _tag_arr("[]", sizeof( unsigned char), 3u)); _temp179[ 0u]= rest;
Cyc_PP_cat( _tag_arr( _temp179, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL152:
if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*
_temp180[ 4u]; _temp180[ 3u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char),
2u)); _temp180[ 2u]= Cyc_Absynpp_exp2doc( _temp161); _temp180[ 1u]= Cyc_PP_text(
_tag_arr("[", sizeof( unsigned char), 2u)); _temp180[ 0u]= rest; Cyc_PP_cat(
_tag_arr( _temp180, sizeof( struct Cyc_PP_Doc*), 4u));}); _LL154: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;}{ void* _temp181= _temp163; struct Cyc_List_List*
_temp187; struct Cyc_Core_Opt* _temp189; struct Cyc_Absyn_VarargInfo* _temp191;
int _temp193; struct Cyc_List_List* _temp195; struct Cyc_Position_Segment*
_temp197; struct Cyc_List_List* _temp199; _LL183: if(*(( int*) _temp181) ==  Cyc_Absyn_WithTypes){
_LL196: _temp195=(( struct Cyc_Absyn_WithTypes_struct*) _temp181)->f1; goto
_LL194; _LL194: _temp193=(( struct Cyc_Absyn_WithTypes_struct*) _temp181)->f2;
goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_WithTypes_struct*) _temp181)->f3;
goto _LL190; _LL190: _temp189=(( struct Cyc_Absyn_WithTypes_struct*) _temp181)->f4;
goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_WithTypes_struct*) _temp181)->f5;
goto _LL184;} else{ goto _LL185;} _LL185: if(*(( int*) _temp181) ==  Cyc_Absyn_NoTypes){
_LL200: _temp199=(( struct Cyc_Absyn_NoTypes_struct*) _temp181)->f1; goto _LL198;
_LL198: _temp197=(( struct Cyc_Absyn_NoTypes_struct*) _temp181)->f2; goto _LL186;}
else{ goto _LL182;} _LL184: return({ struct Cyc_PP_Doc* _temp201[ 2u]; _temp201[
1u]= Cyc_Absynpp_funargs2doc( _temp195, _temp193, _temp191, _temp189, _temp187);
_temp201[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp201, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL186: return({ struct Cyc_PP_Doc* _temp202[ 2u]; _temp202[ 1u]= Cyc_PP_group(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, _temp199)); _temp202[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp202,
sizeof( struct Cyc_PP_Doc*), 2u));}); _LL182:;} _LL156: if( ! Cyc_Absynpp_to_VC){
if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*
_temp203[ 2u]; _temp203[ 1u]= Cyc_Absynpp_atts2doc( _temp165); _temp203[ 0u]=
rest; Cyc_PP_cat( _tag_arr( _temp203, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ if( Cyc_Absynpp_next_is_pointer( tms->tl)){ return({ struct Cyc_PP_Doc*
_temp204[ 2u]; _temp204[ 1u]= rest; _temp204[ 0u]= Cyc_Absynpp_callconv2doc(
_temp165); Cyc_PP_cat( _tag_arr( _temp204, sizeof( struct Cyc_PP_Doc*), 2u));});}
return rest;} _LL158: if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;}
if( _temp167){ return({ struct Cyc_PP_Doc* _temp205[ 2u]; _temp205[ 1u]= Cyc_Absynpp_ktvars2doc(
_temp171); _temp205[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp205, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp206[ 2u];
_temp206[ 1u]= Cyc_Absynpp_tvars2doc( _temp171); _temp206[ 0u]= rest; Cyc_PP_cat(
_tag_arr( _temp206, sizeof( struct Cyc_PP_Doc*), 2u));});} _LL160: { struct Cyc_PP_Doc*
ptr;{ void* _temp207= _temp177; struct Cyc_Absyn_Exp* _temp215; struct Cyc_Absyn_Exp*
_temp217; _LL209: if(( unsigned int) _temp207 >  1u?*(( int*) _temp207) ==  Cyc_Absyn_Nullable_ps:
0){ _LL216: _temp215=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp207)->f1;
goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int) _temp207 >  1u?*((
int*) _temp207) ==  Cyc_Absyn_NonNullable_ps: 0){ _LL218: _temp217=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp207)->f1; goto _LL212;} else{ goto _LL213;} _LL213: if( _temp207 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL214;} else{ goto _LL208;} _LL210: if( Cyc_Evexp_eval_const_uint_exp(
_temp215) ==  1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc* _temp219[ 4u]; _temp219[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp219[ 2u]= Cyc_Absynpp_exp2doc(
_temp215); _temp219[ 1u]= Cyc_PP_text( _tag_arr("{", sizeof( unsigned char), 2u));
_temp219[ 0u]= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp219, sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL208; _LL212:
if( Cyc_Evexp_eval_const_uint_exp( _temp217) ==  1){ ptr= Cyc_PP_text( _tag_arr("@",
sizeof( unsigned char), 2u));} else{ ptr=({ struct Cyc_PP_Doc* _temp220[ 4u];
_temp220[ 3u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp220[
2u]= Cyc_Absynpp_exp2doc( _temp217); _temp220[ 1u]= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); _temp220[ 0u]= Cyc_PP_text( _tag_arr("@", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp220, sizeof( struct Cyc_PP_Doc*),
4u));});} goto _LL208; _LL214: ptr= Cyc_PP_text( _tag_arr("?", sizeof(
unsigned char), 2u)); goto _LL208; _LL208:;}{ void* _temp221= Cyc_Tcutil_compress(
_temp175); _LL223: if( _temp221 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL224;}
else{ goto _LL225;} _LL225: goto _LL226; _LL224: return({ struct Cyc_PP_Doc*
_temp227[ 2u]; _temp227[ 1u]= rest; _temp227[ 0u]= ptr; Cyc_PP_cat( _tag_arr(
_temp227, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL226: return({ struct Cyc_PP_Doc*
_temp228[ 4u]; _temp228[ 3u]= rest; _temp228[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp228[ 1u]= Cyc_Absynpp_typ2doc( _temp175);
_temp228[ 0u]= ptr; Cyc_PP_cat( _tag_arr( _temp228, sizeof( struct Cyc_PP_Doc*),
4u));}); _LL222:;}} _LL148:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){
void* _temp230= Cyc_Tcutil_compress( t); _LL232: if( _temp230 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL233;} else{ goto _LL234;} _LL234: goto _LL235; _LL233: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL235: return Cyc_Absynpp_ntyp2doc(
t); _LL231:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
rgions, struct Cyc_List_List** effects, void* t){ void* _temp236= Cyc_Tcutil_compress(
t); void* _temp244; struct Cyc_List_List* _temp246; _LL238: if(( unsigned int)
_temp236 >  3u?*(( int*) _temp236) ==  Cyc_Absyn_AccessEff: 0){ _LL245: _temp244=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp236)->f1; goto _LL239;} else{
goto _LL240;} _LL240: if(( unsigned int) _temp236 >  3u?*(( int*) _temp236) == 
Cyc_Absyn_JoinEff: 0){ _LL247: _temp246=(( struct Cyc_Absyn_JoinEff_struct*)
_temp236)->f1; goto _LL241;} else{ goto _LL242;} _LL242: goto _LL243; _LL239:*
rgions=({ struct Cyc_List_List* _temp248=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp248->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp244); _temp248->tl=* rgions; _temp248;}); goto _LL237; _LL241: for( 0;
_temp246 !=  0; _temp246= _temp246->tl){ Cyc_Absynpp_effects2docs( rgions,
effects,( void*) _temp246->hd);} goto _LL237; _LL243:* effects=({ struct Cyc_List_List*
_temp249=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp249->hd=( void*) Cyc_Absynpp_typ2doc( t); _temp249->tl=* effects; _temp249;});
goto _LL237; _LL237:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* rgions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
rgions,& effects, t); rgions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( rgions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( rgions ==  0? effects !=  0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp250= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), rgions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp251=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp251->hd=( void*) _temp250; _temp251->tl= 0;
_temp251;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp252= t; struct Cyc_Core_Opt* _temp302; int _temp304; struct Cyc_Core_Opt*
_temp306; struct Cyc_Core_Opt* _temp308; struct Cyc_Absyn_Tvar* _temp310; struct
Cyc_Absyn_TunionInfo _temp312; void* _temp314; struct Cyc_List_List* _temp316;
void* _temp318; struct Cyc_Absyn_TunionFieldInfo _temp320; struct Cyc_List_List*
_temp322; void* _temp324; void* _temp326; void* _temp328; int _temp330; struct
Cyc_List_List* _temp332; struct Cyc_List_List* _temp334; struct _tuple0*
_temp336; struct Cyc_List_List* _temp338; struct _tuple0* _temp340; struct Cyc_List_List*
_temp342; struct Cyc_List_List* _temp344; struct Cyc_List_List* _temp346; struct
_tuple0* _temp348; struct Cyc_Core_Opt* _temp350; struct Cyc_List_List* _temp352;
struct _tuple0* _temp354; void* _temp356; void* _temp358; _LL254: if((
unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_ArrayType: 0){
goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int) _temp252 >  3u?*((
int*) _temp252) ==  Cyc_Absyn_FnType: 0){ goto _LL257;} else{ goto _LL258;}
_LL258: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_PointerType:
0){ goto _LL259;} else{ goto _LL260;} _LL260: if( _temp252 == ( void*) Cyc_Absyn_VoidType){
goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp252 >  3u?*((
int*) _temp252) ==  Cyc_Absyn_Evar: 0){ _LL309: _temp308=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f1; goto _LL307; _LL307: _temp306=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f2; goto _LL305; _LL305: _temp304=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f3; goto _LL303; _LL303: _temp302=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f4; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_VarType: 0){ _LL311: _temp310=((
struct Cyc_Absyn_VarType_struct*) _temp252)->f1; goto _LL265;} else{ goto _LL266;}
_LL266: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_TunionType:
0){ _LL313: _temp312=(( struct Cyc_Absyn_TunionType_struct*) _temp252)->f1;
_LL319: _temp318=( void*) _temp312.tunion_info; goto _LL317; _LL317: _temp316=
_temp312.targs; goto _LL315; _LL315: _temp314=( void*) _temp312.rgn; goto _LL267;}
else{ goto _LL268;} _LL268: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_TunionFieldType: 0){ _LL321: _temp320=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp252)->f1; _LL325: _temp324=( void*) _temp320.field_info; goto _LL323;
_LL323: _temp322= _temp320.targs; goto _LL269;} else{ goto _LL270;} _LL270: if((
unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_IntType: 0){
_LL329: _temp328=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp252)->f1;
goto _LL327; _LL327: _temp326=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp252)->f2; goto _LL271;} else{ goto _LL272;} _LL272: if( _temp252 == ( void*)
Cyc_Absyn_FloatType){ goto _LL273;} else{ goto _LL274;} _LL274: if((
unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_DoubleType: 0){
_LL331: _temp330=(( struct Cyc_Absyn_DoubleType_struct*) _temp252)->f1; goto
_LL275;} else{ goto _LL276;} _LL276: if(( unsigned int) _temp252 >  3u?*(( int*)
_temp252) ==  Cyc_Absyn_TupleType: 0){ _LL333: _temp332=(( struct Cyc_Absyn_TupleType_struct*)
_temp252)->f1; goto _LL277;} else{ goto _LL278;} _LL278: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_StructType: 0){ _LL337:
_temp336=(( struct Cyc_Absyn_StructType_struct*) _temp252)->f1; goto _LL335;
_LL335: _temp334=(( struct Cyc_Absyn_StructType_struct*) _temp252)->f2; goto
_LL279;} else{ goto _LL280;} _LL280: if(( unsigned int) _temp252 >  3u?*(( int*)
_temp252) ==  Cyc_Absyn_UnionType: 0){ _LL341: _temp340=(( struct Cyc_Absyn_UnionType_struct*)
_temp252)->f1; goto _LL339; _LL339: _temp338=(( struct Cyc_Absyn_UnionType_struct*)
_temp252)->f2; goto _LL281;} else{ goto _LL282;} _LL282: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_AnonStructType: 0){ _LL343:
_temp342=(( struct Cyc_Absyn_AnonStructType_struct*) _temp252)->f1; goto _LL283;}
else{ goto _LL284;} _LL284: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_AnonUnionType: 0){ _LL345: _temp344=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp252)->f1; goto _LL285;} else{ goto _LL286;} _LL286: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_AnonEnumType: 0){ _LL347:
_temp346=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp252)->f1; goto _LL287;}
else{ goto _LL288;} _LL288: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_EnumType: 0){ _LL349: _temp348=(( struct Cyc_Absyn_EnumType_struct*)
_temp252)->f1; goto _LL289;} else{ goto _LL290;} _LL290: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_TypedefType: 0){ _LL355:
_temp354=(( struct Cyc_Absyn_TypedefType_struct*) _temp252)->f1; goto _LL353;
_LL353: _temp352=(( struct Cyc_Absyn_TypedefType_struct*) _temp252)->f2; goto
_LL351; _LL351: _temp350=(( struct Cyc_Absyn_TypedefType_struct*) _temp252)->f3;
goto _LL291;} else{ goto _LL292;} _LL292: if(( unsigned int) _temp252 >  3u?*((
int*) _temp252) ==  Cyc_Absyn_RgnHandleType: 0){ _LL357: _temp356=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp252)->f1; goto _LL293;} else{ goto
_LL294;} _LL294: if( _temp252 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL295;}
else{ goto _LL296;} _LL296: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_RgnsEff: 0){ _LL359: _temp358=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp252)->f1; goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_AccessEff: 0){ goto _LL299;}
else{ goto _LL300;} _LL300: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_JoinEff: 0){ goto _LL301;} else{ goto _LL253;} _LL255: return Cyc_PP_text(
_tag_arr("[[[array]]]", sizeof( unsigned char), 12u)); _LL257: return Cyc_PP_nil_doc();
_LL259: return Cyc_PP_nil_doc(); _LL261: s= Cyc_PP_text( _tag_arr("void",
sizeof( unsigned char), 5u)); goto _LL253; _LL263: if( _temp306 !=  0){ return
Cyc_Absynpp_ntyp2doc(( void*) _temp306->v);} else{ s=({ struct Cyc_PP_Doc*
_temp360[ 6u]; _temp360[ 5u]= _temp308 ==  0? Cyc_PP_text( _tag_arr("?", sizeof(
unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*) _temp308->v); _temp360[ 4u]=
Cyc_PP_text( _tag_arr(")::", sizeof( unsigned char), 4u)); _temp360[ 3u]=( ! Cyc_Absynpp_print_full_evars?
1: _temp302 ==  0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*) _temp302->v); _temp360[ 2u]= Cyc_PP_text(( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp362; _temp362.tag= Cyc_Std_Int_pa;
_temp362.f1=( int)(( unsigned int) _temp304);{ void* _temp361[ 1u]={& _temp362};
Cyc_Std_aprintf( _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp361,
sizeof( void*), 1u));}})); _temp360[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); _temp360[ 0u]= Cyc_PP_text( _tag_arr("%", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp360, sizeof( struct Cyc_PP_Doc*),
6u));});} goto _LL253; _LL265: s= Cyc_PP_textptr( _temp310->name); if( Cyc_Absynpp_print_all_kinds){
s=({ struct Cyc_PP_Doc* _temp363[ 3u]; _temp363[ 2u]= Cyc_Absynpp_kindbound2doc((
void*) _temp310->kind); _temp363[ 1u]= Cyc_PP_text( _tag_arr("::", sizeof(
unsigned char), 3u)); _temp363[ 0u]= s; Cyc_PP_cat( _tag_arr( _temp363, sizeof(
struct Cyc_PP_Doc*), 3u));});} if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp310): 0){ s=({ struct Cyc_PP_Doc* _temp364[ 3u]; _temp364[ 2u]= Cyc_PP_text(
_tag_arr(" */", sizeof( unsigned char), 4u)); _temp364[ 1u]= s; _temp364[ 0u]=
Cyc_PP_text( _tag_arr("_ /* ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp364, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL253; _LL267:{
void* _temp365= _temp318; struct Cyc_Absyn_UnknownTunionInfo _temp371; int
_temp373; struct _tuple0* _temp375; struct Cyc_Absyn_Tuniondecl** _temp377;
struct Cyc_Absyn_Tuniondecl* _temp379; struct Cyc_Absyn_Tuniondecl _temp380; int
_temp381; struct _tuple0* _temp383; _LL367: if(*(( int*) _temp365) ==  Cyc_Absyn_UnknownTunion){
_LL372: _temp371=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp365)->f1;
_LL376: _temp375= _temp371.name; goto _LL374; _LL374: _temp373= _temp371.is_xtunion;
goto _LL368;} else{ goto _LL369;} _LL369: if(*(( int*) _temp365) ==  Cyc_Absyn_KnownTunion){
_LL378: _temp377=(( struct Cyc_Absyn_KnownTunion_struct*) _temp365)->f1;
_temp379=* _temp377; _temp380=* _temp379; _LL384: _temp383= _temp380.name; goto
_LL382; _LL382: _temp381= _temp380.is_xtunion; goto _LL370;} else{ goto _LL366;}
_LL368: _temp383= _temp375; _temp381= _temp373; goto _LL370; _LL370: { struct
Cyc_PP_Doc* _temp385= Cyc_PP_text( _temp381? _tag_arr("xtunion ", sizeof(
unsigned char), 9u): _tag_arr("tunion ", sizeof( unsigned char), 8u));{ void*
_temp386= Cyc_Tcutil_compress( _temp314); _LL388: if( _temp386 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL389;} else{ goto _LL390;} _LL390: goto _LL391; _LL389: s=({ struct Cyc_PP_Doc*
_temp392[ 3u]; _temp392[ 2u]= Cyc_Absynpp_tps2doc( _temp316); _temp392[ 1u]= Cyc_Absynpp_qvar2doc(
_temp383); _temp392[ 0u]= _temp385; Cyc_PP_cat( _tag_arr( _temp392, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL387; _LL391: s=({ struct Cyc_PP_Doc*
_temp393[ 5u]; _temp393[ 4u]= Cyc_Absynpp_tps2doc( _temp316); _temp393[ 3u]= Cyc_Absynpp_qvar2doc(
_temp383); _temp393[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp393[ 1u]= Cyc_Absynpp_typ2doc( _temp314); _temp393[ 0u]= _temp385; Cyc_PP_cat(
_tag_arr( _temp393, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL387; _LL387:;}
goto _LL366;} _LL366:;} goto _LL253; _LL269:{ void* _temp394= _temp324; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp400; int _temp402; struct _tuple0*
_temp404; struct _tuple0* _temp406; struct Cyc_Absyn_Tunionfield* _temp408;
struct Cyc_Absyn_Tunionfield _temp410; struct _tuple0* _temp411; struct Cyc_Absyn_Tuniondecl*
_temp413; struct Cyc_Absyn_Tuniondecl _temp415; int _temp416; struct _tuple0*
_temp418; _LL396: if(*(( int*) _temp394) ==  Cyc_Absyn_UnknownTunionfield){
_LL401: _temp400=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp394)->f1;
_LL407: _temp406= _temp400.tunion_name; goto _LL405; _LL405: _temp404= _temp400.field_name;
goto _LL403; _LL403: _temp402= _temp400.is_xtunion; goto _LL397;} else{ goto
_LL398;} _LL398: if(*(( int*) _temp394) ==  Cyc_Absyn_KnownTunionfield){ _LL414:
_temp413=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp394)->f1; _temp415=*
_temp413; _LL419: _temp418= _temp415.name; goto _LL417; _LL417: _temp416=
_temp415.is_xtunion; goto _LL409; _LL409: _temp408=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp394)->f2; _temp410=* _temp408; _LL412: _temp411= _temp410.name; goto _LL399;}
else{ goto _LL395;} _LL397: _temp418= _temp406; _temp416= _temp402; _temp411=
_temp404; goto _LL399; _LL399: { struct Cyc_PP_Doc* _temp420= Cyc_PP_text(
_temp416? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp421[ 4u]; _temp421[ 3u]=
Cyc_Absynpp_qvar2doc( _temp411); _temp421[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp421[ 1u]= Cyc_Absynpp_qvar2doc( _temp418);
_temp421[ 0u]= _temp420; Cyc_PP_cat( _tag_arr( _temp421, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL395;} _LL395:;} goto _LL253; _LL271: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp422= _temp328; _LL424: if( _temp422 == ( void*)
Cyc_Absyn_Signed){ goto _LL425;} else{ goto _LL426;} _LL426: if( _temp422 == (
void*) Cyc_Absyn_Unsigned){ goto _LL427;} else{ goto _LL423;} _LL425: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL423; _LL427: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL423; _LL423:;}{ void* _temp428= _temp326;
_LL430: if( _temp428 == ( void*) Cyc_Absyn_B1){ goto _LL431;} else{ goto _LL432;}
_LL432: if( _temp428 == ( void*) Cyc_Absyn_B2){ goto _LL433;} else{ goto _LL434;}
_LL434: if( _temp428 == ( void*) Cyc_Absyn_B4){ goto _LL435;} else{ goto _LL436;}
_LL436: if( _temp428 == ( void*) Cyc_Absyn_B8){ goto _LL437;} else{ goto _LL429;}
_LL431:{ void* _temp438= _temp328; _LL440: if( _temp438 == ( void*) Cyc_Absyn_Signed){
goto _LL441;} else{ goto _LL442;} _LL442: if( _temp438 == ( void*) Cyc_Absyn_Unsigned){
goto _LL443;} else{ goto _LL439;} _LL441: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL439; _LL443: goto _LL439; _LL439:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL429; _LL433: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL429; _LL435: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL429; _LL437: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL429; _LL429:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp446; _temp446.tag= Cyc_Std_String_pa; _temp446.f1=( struct _tagged_arr) ts;{
struct Cyc_Std_String_pa_struct _temp445; _temp445.tag= Cyc_Std_String_pa;
_temp445.f1=( struct _tagged_arr) sns;{ void* _temp444[ 2u]={& _temp445,&
_temp446}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp444, sizeof( void*), 2u));}}})); goto _LL253;} _LL273: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL253; _LL275: if(
_temp330){ s= Cyc_PP_text( _tag_arr("long double", sizeof( unsigned char), 12u));}
else{ s= Cyc_PP_text( _tag_arr("double", sizeof( unsigned char), 7u));} goto
_LL253; _LL277: s=({ struct Cyc_PP_Doc* _temp447[ 2u]; _temp447[ 1u]= Cyc_Absynpp_args2doc(
_temp332); _temp447[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp447, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL253; _LL279: if( _temp336 ==  0){ s=({ struct Cyc_PP_Doc* _temp448[ 2u];
_temp448[ 1u]= Cyc_Absynpp_tps2doc( _temp334); _temp448[ 0u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp448,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp449[ 3u];
_temp449[ 2u]= Cyc_Absynpp_tps2doc( _temp334); _temp449[ 1u]= _temp336 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp336));
_temp449[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp449, sizeof( struct Cyc_PP_Doc*), 3u));});} goto
_LL253; _LL281: if( _temp340 ==  0){ s=({ struct Cyc_PP_Doc* _temp450[ 2u];
_temp450[ 1u]= Cyc_Absynpp_tps2doc( _temp338); _temp450[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp450,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp451[ 3u];
_temp451[ 2u]= Cyc_Absynpp_tps2doc( _temp338); _temp451[ 1u]= _temp340 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp340));
_temp451[ 0u]= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp451, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL253; _LL283:
s=({ struct Cyc_PP_Doc* _temp452[ 3u]; _temp452[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp452[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp342)); _temp452[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp452, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL253; _LL285: s=({ struct Cyc_PP_Doc* _temp453[ 3u]; _temp453[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp453[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp344)); _temp453[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp453,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL253; _LL287: s=({ struct Cyc_PP_Doc*
_temp454[ 3u]; _temp454[ 2u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp454[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_enumfields2doc( _temp346));
_temp454[ 0u]= Cyc_PP_text( _tag_arr("enum {", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp454, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL253; _LL289: s=({
struct Cyc_PP_Doc* _temp455[ 2u]; _temp455[ 1u]= Cyc_Absynpp_qvar2doc( _temp348);
_temp455[ 0u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp455, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL253; _LL291: s=({
struct Cyc_PP_Doc* _temp456[ 2u]; _temp456[ 1u]= Cyc_Absynpp_tps2doc( _temp352);
_temp456[ 0u]= Cyc_Absynpp_qvar2doc( _temp354); Cyc_PP_cat( _tag_arr( _temp456,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL253; _LL293: s=({ struct Cyc_PP_Doc*
_temp457[ 3u]; _temp457[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof( unsigned char),
2u)); _temp457[ 1u]= Cyc_Absynpp_rgn2doc( _temp356); _temp457[ 0u]= Cyc_PP_text(
_tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp457, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL253; _LL295: s= Cyc_Absynpp_rgn2doc(
t); goto _LL253; _LL297: s=({ struct Cyc_PP_Doc* _temp458[ 3u]; _temp458[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp458[ 1u]= Cyc_Absynpp_typ2doc(
_temp358); _temp458[ 0u]= Cyc_PP_text( _tag_arr("regions(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp458, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL253; _LL299: goto _LL301; _LL301: s= Cyc_Absynpp_eff2doc( t);
goto _LL253; _LL253:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
Cyc_Core_Opt* vo){ return vo ==  0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct
_tagged_arr*) vo->v));} struct _tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc*
Cyc_Absynpp_rgn_cmp2doc( struct _tuple8* cmp){ struct _tuple8 _temp461; void*
_temp462; void* _temp464; struct _tuple8* _temp459= cmp; _temp461=* _temp459;
_LL465: _temp464= _temp461.f1; goto _LL463; _LL463: _temp462= _temp461.f2; goto
_LL460; _LL460: return({ struct Cyc_PP_Doc* _temp466[ 3u]; _temp466[ 2u]= Cyc_Absynpp_rgn2doc(
_temp462); _temp466[ 1u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp466[ 0u]= Cyc_Absynpp_rgn2doc( _temp464); Cyc_PP_cat( _tag_arr( _temp466,
sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List* po){ return Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 ==  0? 0:({ struct Cyc_Core_Opt* _temp467=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp467->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp467;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp468=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp468=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp468,({
struct Cyc_List_List* _temp469=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp469->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp469->tl= 0; _temp469;}));} else{ if(
cyc_varargs !=  0){ struct Cyc_PP_Doc* _temp470=({ struct Cyc_PP_Doc* _temp472[
3u]; _temp472[ 2u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp473=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp473->f1= cyc_varargs->name;
_temp473->f2= cyc_varargs->tq; _temp473->f3=( void*) cyc_varargs->type; _temp473;}));
_temp472[ 1u]= cyc_varargs->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp472[ 0u]= Cyc_PP_text( _tag_arr("...", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp472, sizeof( struct Cyc_PP_Doc*), 3u));}); _temp468=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp468,({
struct Cyc_List_List* _temp471=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp471->hd=( void*) _temp470; _temp471->tl= 0;
_temp471;}));}}{ struct Cyc_PP_Doc* _temp474= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp468); if( effopt !=  0){ _temp474=({ struct
Cyc_PP_Doc* _temp475[ 3u]; _temp475[ 2u]= Cyc_Absynpp_eff2doc(( void*) effopt->v);
_temp475[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp475[
0u]= _temp474; Cyc_PP_cat( _tag_arr( _temp475, sizeof( struct Cyc_PP_Doc*), 3u));});}
if( rgn_po !=  0){ _temp474=({ struct Cyc_PP_Doc* _temp476[ 3u]; _temp476[ 2u]=
Cyc_Absynpp_rgnpo2doc( rgn_po); _temp476[ 1u]= Cyc_PP_text( _tag_arr(":",
sizeof( unsigned char), 2u)); _temp476[ 0u]= _temp474; Cyc_PP_cat( _tag_arr(
_temp476, sizeof( struct Cyc_PP_Doc*), 3u));});} return({ struct Cyc_PP_Doc*
_temp477[ 3u]; _temp477[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp477[ 1u]= _temp474; _temp477[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp477, sizeof( struct Cyc_PP_Doc*),
3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({
struct _tuple1* _temp478=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp478->f1=({ struct Cyc_Core_Opt* _temp479=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp479->v=( void*)(* arg).f1; _temp479;});
_temp478->f2=(* arg).f2; _temp478->f3=(* arg).f3; _temp478;});} struct Cyc_PP_Doc*
Cyc_Absynpp_var2doc( struct _tagged_arr* v){ return Cyc_PP_text(* v);} struct
Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp480= 0; int match;{ void* _temp481=(* q).f1; struct Cyc_List_List* _temp489;
struct Cyc_List_List* _temp491; _LL483: if( _temp481 == ( void*) Cyc_Absyn_Loc_n){
goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int) _temp481 >  1u?*((
int*) _temp481) ==  Cyc_Absyn_Rel_n: 0){ _LL490: _temp489=(( struct Cyc_Absyn_Rel_n_struct*)
_temp481)->f1; goto _LL486;} else{ goto _LL487;} _LL487: if(( unsigned int)
_temp481 >  1u?*(( int*) _temp481) ==  Cyc_Absyn_Abs_n: 0){ _LL492: _temp491=((
struct Cyc_Absyn_Abs_n_struct*) _temp481)->f1; goto _LL488;} else{ goto _LL482;}
_LL484: _temp489= 0; goto _LL486; _LL486: match= 0; _temp480= _temp489; goto
_LL482; _LL488: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_Std_strptrcmp, _temp491, Cyc_Absynpp_curr_namespace):
0; _temp480=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp493=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp493->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp493->tl= _temp491;
_temp493;}): _temp491; goto _LL482; _LL482:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp480,({
struct Cyc_List_List* _temp494=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp494->hd=( void*)(* q).f2; _temp494->tl= 0; _temp494;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp480,({ struct Cyc_List_List* _temp495=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp495->hd=( void*)(* q).f2; _temp495->tl= 0;
_temp495;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp496=(* v).f1; struct Cyc_List_List* _temp506; struct Cyc_List_List*
_temp508; _LL498: if( _temp496 == ( void*) Cyc_Absyn_Loc_n){ goto _LL499;} else{
goto _LL500;} _LL500: if(( unsigned int) _temp496 >  1u?*(( int*) _temp496) == 
Cyc_Absyn_Rel_n: 0){ _LL507: _temp506=(( struct Cyc_Absyn_Rel_n_struct*)
_temp496)->f1; if( _temp506 ==  0){ goto _LL501;} else{ goto _LL502;}} else{
goto _LL502;} _LL502: if(( unsigned int) _temp496 >  1u?*(( int*) _temp496) == 
Cyc_Absyn_Abs_n: 0){ _LL509: _temp508=(( struct Cyc_Absyn_Abs_n_struct*)
_temp496)->f1; goto _LL503;} else{ goto _LL504;} _LL504: goto _LL505; _LL499:
goto _LL501; _LL501: return Cyc_Absynpp_var2doc((* v).f2); _LL503: if((( int(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_strptrcmp, _temp508, Cyc_Absynpp_curr_namespace)
==  0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL505;} _LL505:
return({ struct Cyc_PP_Doc* _temp510[ 2u]; _temp510[ 1u]= Cyc_Absynpp_qvar2doc(
v); _temp510[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof(
unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp510, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL497:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp511=(
void*) e->r; void* _temp589; struct Cyc_Absyn_Exp* _temp591; struct Cyc_Absyn_Exp*
_temp593; _LL513: if(*(( int*) _temp511) ==  Cyc_Absyn_Const_e){ goto _LL514;}
else{ goto _LL515;} _LL515: if(*(( int*) _temp511) ==  Cyc_Absyn_Var_e){ goto
_LL516;} else{ goto _LL517;} _LL517: if(*(( int*) _temp511) ==  Cyc_Absyn_UnknownId_e){
goto _LL518;} else{ goto _LL519;} _LL519: if(*(( int*) _temp511) ==  Cyc_Absyn_Primop_e){
_LL590: _temp589=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp511)->f1;
goto _LL520;} else{ goto _LL521;} _LL521: if(*(( int*) _temp511) ==  Cyc_Absyn_AssignOp_e){
goto _LL522;} else{ goto _LL523;} _LL523: if(*(( int*) _temp511) ==  Cyc_Absyn_Increment_e){
goto _LL524;} else{ goto _LL525;} _LL525: if(*(( int*) _temp511) ==  Cyc_Absyn_Conditional_e){
goto _LL526;} else{ goto _LL527;} _LL527: if(*(( int*) _temp511) ==  Cyc_Absyn_SeqExp_e){
goto _LL528;} else{ goto _LL529;} _LL529: if(*(( int*) _temp511) ==  Cyc_Absyn_UnknownCall_e){
goto _LL530;} else{ goto _LL531;} _LL531: if(*(( int*) _temp511) ==  Cyc_Absyn_FnCall_e){
goto _LL532;} else{ goto _LL533;} _LL533: if(*(( int*) _temp511) ==  Cyc_Absyn_Throw_e){
goto _LL534;} else{ goto _LL535;} _LL535: if(*(( int*) _temp511) ==  Cyc_Absyn_NoInstantiate_e){
_LL592: _temp591=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp511)->f1;
goto _LL536;} else{ goto _LL537;} _LL537: if(*(( int*) _temp511) ==  Cyc_Absyn_Instantiate_e){
_LL594: _temp593=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp511)->f1; goto
_LL538;} else{ goto _LL539;} _LL539: if(*(( int*) _temp511) ==  Cyc_Absyn_Cast_e){
goto _LL540;} else{ goto _LL541;} _LL541: if(*(( int*) _temp511) ==  Cyc_Absyn_New_e){
goto _LL542;} else{ goto _LL543;} _LL543: if(*(( int*) _temp511) ==  Cyc_Absyn_Address_e){
goto _LL544;} else{ goto _LL545;} _LL545: if(*(( int*) _temp511) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL546;} else{ goto _LL547;} _LL547: if(*(( int*) _temp511) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL548;} else{ goto _LL549;} _LL549: if(*(( int*) _temp511) ==  Cyc_Absyn_Offsetof_e){
goto _LL550;} else{ goto _LL551;} _LL551: if(*(( int*) _temp511) ==  Cyc_Absyn_Gentyp_e){
goto _LL552;} else{ goto _LL553;} _LL553: if(*(( int*) _temp511) ==  Cyc_Absyn_Deref_e){
goto _LL554;} else{ goto _LL555;} _LL555: if(*(( int*) _temp511) ==  Cyc_Absyn_StructMember_e){
goto _LL556;} else{ goto _LL557;} _LL557: if(*(( int*) _temp511) ==  Cyc_Absyn_StructArrow_e){
goto _LL558;} else{ goto _LL559;} _LL559: if(*(( int*) _temp511) ==  Cyc_Absyn_Subscript_e){
goto _LL560;} else{ goto _LL561;} _LL561: if(*(( int*) _temp511) ==  Cyc_Absyn_Tuple_e){
goto _LL562;} else{ goto _LL563;} _LL563: if(*(( int*) _temp511) ==  Cyc_Absyn_CompoundLit_e){
goto _LL564;} else{ goto _LL565;} _LL565: if(*(( int*) _temp511) ==  Cyc_Absyn_Array_e){
goto _LL566;} else{ goto _LL567;} _LL567: if(*(( int*) _temp511) ==  Cyc_Absyn_Comprehension_e){
goto _LL568;} else{ goto _LL569;} _LL569: if(*(( int*) _temp511) ==  Cyc_Absyn_Struct_e){
goto _LL570;} else{ goto _LL571;} _LL571: if(*(( int*) _temp511) ==  Cyc_Absyn_AnonStruct_e){
goto _LL572;} else{ goto _LL573;} _LL573: if(*(( int*) _temp511) ==  Cyc_Absyn_Tunion_e){
goto _LL574;} else{ goto _LL575;} _LL575: if(*(( int*) _temp511) ==  Cyc_Absyn_Enum_e){
goto _LL576;} else{ goto _LL577;} _LL577: if(*(( int*) _temp511) ==  Cyc_Absyn_AnonEnum_e){
goto _LL578;} else{ goto _LL579;} _LL579: if(*(( int*) _temp511) ==  Cyc_Absyn_Malloc_e){
goto _LL580;} else{ goto _LL581;} _LL581: if(*(( int*) _temp511) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL582;} else{ goto _LL583;} _LL583: if(*(( int*) _temp511) ==  Cyc_Absyn_StmtExp_e){
goto _LL584;} else{ goto _LL585;} _LL585: if(*(( int*) _temp511) ==  Cyc_Absyn_Codegen_e){
goto _LL586;} else{ goto _LL587;} _LL587: if(*(( int*) _temp511) ==  Cyc_Absyn_Fill_e){
goto _LL588;} else{ goto _LL512;} _LL514: return 10000; _LL516: return 10000;
_LL518: return 10000; _LL520: { void* _temp595= _temp589; _LL597: if( _temp595
== ( void*) Cyc_Absyn_Plus){ goto _LL598;} else{ goto _LL599;} _LL599: if(
_temp595 == ( void*) Cyc_Absyn_Times){ goto _LL600;} else{ goto _LL601;} _LL601:
if( _temp595 == ( void*) Cyc_Absyn_Minus){ goto _LL602;} else{ goto _LL603;}
_LL603: if( _temp595 == ( void*) Cyc_Absyn_Div){ goto _LL604;} else{ goto _LL605;}
_LL605: if( _temp595 == ( void*) Cyc_Absyn_Mod){ goto _LL606;} else{ goto _LL607;}
_LL607: if( _temp595 == ( void*) Cyc_Absyn_Eq){ goto _LL608;} else{ goto _LL609;}
_LL609: if( _temp595 == ( void*) Cyc_Absyn_Neq){ goto _LL610;} else{ goto _LL611;}
_LL611: if( _temp595 == ( void*) Cyc_Absyn_Gt){ goto _LL612;} else{ goto _LL613;}
_LL613: if( _temp595 == ( void*) Cyc_Absyn_Lt){ goto _LL614;} else{ goto _LL615;}
_LL615: if( _temp595 == ( void*) Cyc_Absyn_Gte){ goto _LL616;} else{ goto _LL617;}
_LL617: if( _temp595 == ( void*) Cyc_Absyn_Lte){ goto _LL618;} else{ goto _LL619;}
_LL619: if( _temp595 == ( void*) Cyc_Absyn_Not){ goto _LL620;} else{ goto _LL621;}
_LL621: if( _temp595 == ( void*) Cyc_Absyn_Bitnot){ goto _LL622;} else{ goto
_LL623;} _LL623: if( _temp595 == ( void*) Cyc_Absyn_Bitand){ goto _LL624;} else{
goto _LL625;} _LL625: if( _temp595 == ( void*) Cyc_Absyn_Bitor){ goto _LL626;}
else{ goto _LL627;} _LL627: if( _temp595 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL628;} else{ goto _LL629;} _LL629: if( _temp595 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL630;} else{ goto _LL631;} _LL631: if( _temp595 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL632;} else{ goto _LL633;} _LL633: if( _temp595 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL634;} else{ goto _LL635;} _LL635: if( _temp595 == ( void*) Cyc_Absyn_Size){
goto _LL636;} else{ goto _LL596;} _LL598: return 100; _LL600: return 110; _LL602:
return 100; _LL604: return 110; _LL606: return 110; _LL608: return 70; _LL610:
return 70; _LL612: return 80; _LL614: return 80; _LL616: return 80; _LL618:
return 80; _LL620: return 130; _LL622: return 130; _LL624: return 60; _LL626:
return 40; _LL628: return 50; _LL630: return 90; _LL632: return 80; _LL634:
return 80; _LL636: return 140; _LL596:;} _LL522: return 20; _LL524: return 130;
_LL526: return 30; _LL528: return 10; _LL530: return 140; _LL532: return 140;
_LL534: return 130; _LL536: return Cyc_Absynpp_exp_prec( _temp591); _LL538:
return Cyc_Absynpp_exp_prec( _temp593); _LL540: return 120; _LL542: return 15;
_LL544: return 130; _LL546: return 130; _LL548: return 130; _LL550: return 130;
_LL552: return 130; _LL554: return 130; _LL556: return 140; _LL558: return 140;
_LL560: return 140; _LL562: return 150; _LL564: goto _LL566; _LL566: goto _LL568;
_LL568: goto _LL570; _LL570: goto _LL572; _LL572: goto _LL574; _LL574: goto
_LL576; _LL576: goto _LL578; _LL578: goto _LL580; _LL580: return 140; _LL582:
return 140; _LL584: return 10000; _LL586: return 140; _LL588: return 140; _LL512:;}
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp637=(
void*) e->r; void* _temp717; struct _tuple0* _temp719; struct _tuple0* _temp721;
struct Cyc_List_List* _temp723; void* _temp725; struct Cyc_Absyn_Exp* _temp727;
struct Cyc_Core_Opt* _temp729; struct Cyc_Absyn_Exp* _temp731; void* _temp733;
struct Cyc_Absyn_Exp* _temp735; struct Cyc_Absyn_Exp* _temp737; struct Cyc_Absyn_Exp*
_temp739; struct Cyc_Absyn_Exp* _temp741; struct Cyc_Absyn_Exp* _temp743; struct
Cyc_Absyn_Exp* _temp745; struct Cyc_List_List* _temp747; struct Cyc_Absyn_Exp*
_temp749; struct Cyc_List_List* _temp751; struct Cyc_Absyn_Exp* _temp753; struct
Cyc_Absyn_Exp* _temp755; struct Cyc_Absyn_Exp* _temp757; struct Cyc_Absyn_Exp*
_temp759; struct Cyc_Absyn_Exp* _temp761; void* _temp763; struct Cyc_Absyn_Exp*
_temp765; struct Cyc_Absyn_Exp* _temp767; struct Cyc_Absyn_Exp* _temp769; void*
_temp771; struct Cyc_Absyn_Exp* _temp773; void* _temp775; struct _tagged_arr*
_temp777; void* _temp779; void* _temp781; unsigned int _temp783; void* _temp785;
void* _temp787; struct Cyc_List_List* _temp789; struct Cyc_Absyn_Exp* _temp791;
struct _tagged_arr* _temp793; struct Cyc_Absyn_Exp* _temp795; struct _tagged_arr*
_temp797; struct Cyc_Absyn_Exp* _temp799; struct Cyc_Absyn_Exp* _temp801; struct
Cyc_Absyn_Exp* _temp803; struct Cyc_List_List* _temp805; struct Cyc_List_List*
_temp807; struct _tuple1* _temp809; struct Cyc_List_List* _temp811; struct Cyc_Absyn_Exp*
_temp813; struct Cyc_Absyn_Exp* _temp815; struct Cyc_Absyn_Vardecl* _temp817;
struct Cyc_List_List* _temp819; struct _tuple0* _temp821; struct Cyc_List_List*
_temp823; struct Cyc_Absyn_Tunionfield* _temp825; struct Cyc_List_List* _temp827;
struct _tuple0* _temp829; struct _tuple0* _temp831; void* _temp833; struct Cyc_Absyn_Exp*
_temp835; struct Cyc_List_List* _temp837; struct Cyc_Core_Opt* _temp839; struct
Cyc_Absyn_Stmt* _temp841; struct Cyc_Absyn_Fndecl* _temp843; struct Cyc_Absyn_Exp*
_temp845; _LL639: if(*(( int*) _temp637) ==  Cyc_Absyn_Const_e){ _LL718:
_temp717=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp637)->f1; goto _LL640;}
else{ goto _LL641;} _LL641: if(*(( int*) _temp637) ==  Cyc_Absyn_Var_e){ _LL720:
_temp719=(( struct Cyc_Absyn_Var_e_struct*) _temp637)->f1; goto _LL642;} else{
goto _LL643;} _LL643: if(*(( int*) _temp637) ==  Cyc_Absyn_UnknownId_e){ _LL722:
_temp721=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp637)->f1; goto _LL644;}
else{ goto _LL645;} _LL645: if(*(( int*) _temp637) ==  Cyc_Absyn_Primop_e){
_LL726: _temp725=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp637)->f1;
goto _LL724; _LL724: _temp723=(( struct Cyc_Absyn_Primop_e_struct*) _temp637)->f2;
goto _LL646;} else{ goto _LL647;} _LL647: if(*(( int*) _temp637) ==  Cyc_Absyn_AssignOp_e){
_LL732: _temp731=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp637)->f1; goto
_LL730; _LL730: _temp729=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp637)->f2;
goto _LL728; _LL728: _temp727=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp637)->f3;
goto _LL648;} else{ goto _LL649;} _LL649: if(*(( int*) _temp637) ==  Cyc_Absyn_Increment_e){
_LL736: _temp735=(( struct Cyc_Absyn_Increment_e_struct*) _temp637)->f1; goto
_LL734; _LL734: _temp733=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp637)->f2; goto _LL650;} else{ goto _LL651;} _LL651: if(*(( int*) _temp637)
==  Cyc_Absyn_Conditional_e){ _LL742: _temp741=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp637)->f1; goto _LL740; _LL740: _temp739=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp637)->f2; goto _LL738; _LL738: _temp737=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp637)->f3; goto _LL652;} else{ goto _LL653;} _LL653: if(*(( int*) _temp637)
==  Cyc_Absyn_SeqExp_e){ _LL746: _temp745=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp637)->f1; goto _LL744; _LL744: _temp743=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp637)->f2; goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*) _temp637)
==  Cyc_Absyn_UnknownCall_e){ _LL750: _temp749=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp637)->f1; goto _LL748; _LL748: _temp747=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp637)->f2; goto _LL656;} else{ goto _LL657;} _LL657: if(*(( int*) _temp637)
==  Cyc_Absyn_FnCall_e){ _LL754: _temp753=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp637)->f1; goto _LL752; _LL752: _temp751=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp637)->f2; goto _LL658;} else{ goto _LL659;} _LL659: if(*(( int*) _temp637)
==  Cyc_Absyn_Throw_e){ _LL756: _temp755=(( struct Cyc_Absyn_Throw_e_struct*)
_temp637)->f1; goto _LL660;} else{ goto _LL661;} _LL661: if(*(( int*) _temp637)
==  Cyc_Absyn_NoInstantiate_e){ _LL758: _temp757=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp637)->f1; goto _LL662;} else{ goto _LL663;} _LL663: if(*(( int*) _temp637)
==  Cyc_Absyn_Instantiate_e){ _LL760: _temp759=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp637)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp637)
==  Cyc_Absyn_Cast_e){ _LL764: _temp763=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp637)->f1; goto _LL762; _LL762: _temp761=(( struct Cyc_Absyn_Cast_e_struct*)
_temp637)->f2; goto _LL666;} else{ goto _LL667;} _LL667: if(*(( int*) _temp637)
==  Cyc_Absyn_Address_e){ _LL766: _temp765=(( struct Cyc_Absyn_Address_e_struct*)
_temp637)->f1; goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp637)
==  Cyc_Absyn_New_e){ _LL770: _temp769=(( struct Cyc_Absyn_New_e_struct*)
_temp637)->f1; goto _LL768; _LL768: _temp767=(( struct Cyc_Absyn_New_e_struct*)
_temp637)->f2; goto _LL670;} else{ goto _LL671;} _LL671: if(*(( int*) _temp637)
==  Cyc_Absyn_Sizeoftyp_e){ _LL772: _temp771=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp637)->f1; goto _LL672;} else{ goto _LL673;} _LL673: if(*(( int*) _temp637)
==  Cyc_Absyn_Sizeofexp_e){ _LL774: _temp773=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp637)->f1; goto _LL674;} else{ goto _LL675;} _LL675: if(*(( int*) _temp637)
==  Cyc_Absyn_Offsetof_e){ _LL780: _temp779=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp637)->f1; goto _LL776; _LL776: _temp775=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp637)->f2; if(*(( int*) _temp775) ==  Cyc_Absyn_StructField){ _LL778:
_temp777=(( struct Cyc_Absyn_StructField_struct*) _temp775)->f1; goto _LL676;}
else{ goto _LL677;}} else{ goto _LL677;} _LL677: if(*(( int*) _temp637) ==  Cyc_Absyn_Offsetof_e){
_LL786: _temp785=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp637)->f1;
goto _LL782; _LL782: _temp781=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp637)->f2; if(*(( int*) _temp781) ==  Cyc_Absyn_TupleIndex){ _LL784:
_temp783=(( struct Cyc_Absyn_TupleIndex_struct*) _temp781)->f1; goto _LL678;}
else{ goto _LL679;}} else{ goto _LL679;} _LL679: if(*(( int*) _temp637) ==  Cyc_Absyn_Gentyp_e){
_LL790: _temp789=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp637)->f1; goto
_LL788; _LL788: _temp787=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp637)->f2;
goto _LL680;} else{ goto _LL681;} _LL681: if(*(( int*) _temp637) ==  Cyc_Absyn_Deref_e){
_LL792: _temp791=(( struct Cyc_Absyn_Deref_e_struct*) _temp637)->f1; goto _LL682;}
else{ goto _LL683;} _LL683: if(*(( int*) _temp637) ==  Cyc_Absyn_StructMember_e){
_LL796: _temp795=(( struct Cyc_Absyn_StructMember_e_struct*) _temp637)->f1; goto
_LL794; _LL794: _temp793=(( struct Cyc_Absyn_StructMember_e_struct*) _temp637)->f2;
goto _LL684;} else{ goto _LL685;} _LL685: if(*(( int*) _temp637) ==  Cyc_Absyn_StructArrow_e){
_LL800: _temp799=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp637)->f1; goto
_LL798; _LL798: _temp797=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp637)->f2;
goto _LL686;} else{ goto _LL687;} _LL687: if(*(( int*) _temp637) ==  Cyc_Absyn_Subscript_e){
_LL804: _temp803=(( struct Cyc_Absyn_Subscript_e_struct*) _temp637)->f1; goto
_LL802; _LL802: _temp801=(( struct Cyc_Absyn_Subscript_e_struct*) _temp637)->f2;
goto _LL688;} else{ goto _LL689;} _LL689: if(*(( int*) _temp637) ==  Cyc_Absyn_Tuple_e){
_LL806: _temp805=(( struct Cyc_Absyn_Tuple_e_struct*) _temp637)->f1; goto _LL690;}
else{ goto _LL691;} _LL691: if(*(( int*) _temp637) ==  Cyc_Absyn_CompoundLit_e){
_LL810: _temp809=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp637)->f1; goto
_LL808; _LL808: _temp807=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp637)->f2;
goto _LL692;} else{ goto _LL693;} _LL693: if(*(( int*) _temp637) ==  Cyc_Absyn_Array_e){
_LL812: _temp811=(( struct Cyc_Absyn_Array_e_struct*) _temp637)->f1; goto _LL694;}
else{ goto _LL695;} _LL695: if(*(( int*) _temp637) ==  Cyc_Absyn_Comprehension_e){
_LL818: _temp817=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp637)->f1;
goto _LL816; _LL816: _temp815=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp637)->f2; goto _LL814; _LL814: _temp813=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp637)->f3; goto _LL696;} else{ goto _LL697;} _LL697: if(*(( int*) _temp637)
==  Cyc_Absyn_Struct_e){ _LL822: _temp821=(( struct Cyc_Absyn_Struct_e_struct*)
_temp637)->f1; goto _LL820; _LL820: _temp819=(( struct Cyc_Absyn_Struct_e_struct*)
_temp637)->f3; goto _LL698;} else{ goto _LL699;} _LL699: if(*(( int*) _temp637)
==  Cyc_Absyn_AnonStruct_e){ _LL824: _temp823=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp637)->f2; goto _LL700;} else{ goto _LL701;} _LL701: if(*(( int*) _temp637)
==  Cyc_Absyn_Tunion_e){ _LL828: _temp827=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp637)->f3; goto _LL826; _LL826: _temp825=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp637)->f5; goto _LL702;} else{ goto _LL703;} _LL703: if(*(( int*) _temp637)
==  Cyc_Absyn_Enum_e){ _LL830: _temp829=(( struct Cyc_Absyn_Enum_e_struct*)
_temp637)->f1; goto _LL704;} else{ goto _LL705;} _LL705: if(*(( int*) _temp637)
==  Cyc_Absyn_AnonEnum_e){ _LL832: _temp831=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp637)->f1; goto _LL706;} else{ goto _LL707;} _LL707: if(*(( int*) _temp637)
==  Cyc_Absyn_Malloc_e){ _LL836: _temp835=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp637)->f1; goto _LL834; _LL834: _temp833=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp637)->f2; goto _LL708;} else{ goto _LL709;} _LL709: if(*(( int*) _temp637)
==  Cyc_Absyn_UnresolvedMem_e){ _LL840: _temp839=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp637)->f1; goto _LL838; _LL838: _temp837=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp637)->f2; goto _LL710;} else{ goto _LL711;} _LL711: if(*(( int*) _temp637)
==  Cyc_Absyn_StmtExp_e){ _LL842: _temp841=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp637)->f1; goto _LL712;} else{ goto _LL713;} _LL713: if(*(( int*) _temp637)
==  Cyc_Absyn_Codegen_e){ _LL844: _temp843=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp637)->f1; goto _LL714;} else{ goto _LL715;} _LL715: if(*(( int*) _temp637)
==  Cyc_Absyn_Fill_e){ _LL846: _temp845=(( struct Cyc_Absyn_Fill_e_struct*)
_temp637)->f1; goto _LL716;} else{ goto _LL638;} _LL640: s= Cyc_Absynpp_cnst2doc(
_temp717); goto _LL638; _LL642: _temp721= _temp719; goto _LL644; _LL644: s= Cyc_Absynpp_qvar2doc(
_temp721); goto _LL638; _LL646: s= Cyc_Absynpp_primapp2doc( myprec, _temp725,
_temp723); goto _LL638; _LL648: s=({ struct Cyc_PP_Doc* _temp847[ 5u]; _temp847[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp727); _temp847[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp847[ 2u]= _temp729 ==  0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)
_temp729->v); _temp847[ 1u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); _temp847[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp731); Cyc_PP_cat(
_tag_arr( _temp847, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL638; _LL650: {
struct Cyc_PP_Doc* _temp848= Cyc_Absynpp_exp2doc_prec( myprec, _temp735);{ void*
_temp849= _temp733; _LL851: if( _temp849 == ( void*) Cyc_Absyn_PreInc){ goto
_LL852;} else{ goto _LL853;} _LL853: if( _temp849 == ( void*) Cyc_Absyn_PreDec){
goto _LL854;} else{ goto _LL855;} _LL855: if( _temp849 == ( void*) Cyc_Absyn_PostInc){
goto _LL856;} else{ goto _LL857;} _LL857: if( _temp849 == ( void*) Cyc_Absyn_PostDec){
goto _LL858;} else{ goto _LL850;} _LL852: s=({ struct Cyc_PP_Doc* _temp859[ 2u];
_temp859[ 1u]= _temp848; _temp859[ 0u]= Cyc_PP_text( _tag_arr("++", sizeof(
unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp859, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL850; _LL854: s=({ struct Cyc_PP_Doc* _temp860[ 2u]; _temp860[ 1u]=
_temp848; _temp860[ 0u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp860, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL850; _LL856: s=({ struct Cyc_PP_Doc* _temp861[ 2u]; _temp861[ 1u]= Cyc_PP_text(
_tag_arr("++", sizeof( unsigned char), 3u)); _temp861[ 0u]= _temp848; Cyc_PP_cat(
_tag_arr( _temp861, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL850; _LL858: s=({
struct Cyc_PP_Doc* _temp862[ 2u]; _temp862[ 1u]= Cyc_PP_text( _tag_arr("--",
sizeof( unsigned char), 3u)); _temp862[ 0u]= _temp848; Cyc_PP_cat( _tag_arr(
_temp862, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL850; _LL850:;} goto
_LL638;} _LL652:{ struct _tuple8 _temp864=({ struct _tuple8 _temp863; _temp863.f1=(
void*) _temp739->r; _temp863.f2=( void*) _temp737->r; _temp863;}); _LL866: goto
_LL867; _LL867: s=({ struct Cyc_PP_Doc* _temp868[ 5u]; _temp868[ 4u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp737); _temp868[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); _temp868[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp739);
_temp868[ 1u]= Cyc_PP_text( _tag_arr(" ? ", sizeof( unsigned char), 4u));
_temp868[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp741); Cyc_PP_cat( _tag_arr(
_temp868, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL865; _LL865:;} goto
_LL638; _LL654: s=({ struct Cyc_PP_Doc* _temp869[ 5u]; _temp869[ 4u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp869[ 3u]= Cyc_Absynpp_exp2doc(
_temp743); _temp869[ 2u]= Cyc_PP_text( _tag_arr(", ", sizeof( unsigned char), 3u));
_temp869[ 1u]= Cyc_Absynpp_exp2doc( _temp745); _temp869[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp869,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL638; _LL656: s=({ struct Cyc_PP_Doc*
_temp870[ 4u]; _temp870[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp870[ 2u]= Cyc_Absynpp_exps2doc_prec( 20, _temp747); _temp870[ 1u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp870[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp749); Cyc_PP_cat( _tag_arr( _temp870, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL638; _LL658: s=({ struct Cyc_PP_Doc* _temp871[ 4u]; _temp871[ 3u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp871[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp751); _temp871[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp871[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp753); Cyc_PP_cat(
_tag_arr( _temp871, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL638; _LL660: s=({
struct Cyc_PP_Doc* _temp872[ 2u]; _temp872[ 1u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp755); _temp872[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof(
unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp872, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL638; _LL662: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp757);
goto _LL638; _LL664: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp759); goto _LL638;
_LL666: s=({ struct Cyc_PP_Doc* _temp873[ 4u]; _temp873[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp761); _temp873[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp873[ 1u]= Cyc_Absynpp_typ2doc( _temp763); _temp873[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp873, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL638; _LL668: s=({ struct
Cyc_PP_Doc* _temp874[ 2u]; _temp874[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp765); _temp874[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp874, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL638; _LL670: if( _temp769 ==  0){ s=({ struct Cyc_PP_Doc* _temp875[ 2u];
_temp875[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp767); _temp875[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp875,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp876[ 4u];
_temp876[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp767); _temp876[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp876[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp769)); _temp876[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp876,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL638; _LL672: s=({ struct Cyc_PP_Doc*
_temp877[ 3u]; _temp877[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp877[ 1u]= Cyc_Absynpp_typ2doc( _temp771); _temp877[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp877,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL638; _LL674: s=({ struct Cyc_PP_Doc*
_temp878[ 3u]; _temp878[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp878[ 1u]= Cyc_Absynpp_exp2doc( _temp773); _temp878[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp878,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL638; _LL676: s=({ struct Cyc_PP_Doc*
_temp879[ 5u]; _temp879[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp879[ 3u]= Cyc_PP_textptr( _temp777); _temp879[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp879[ 1u]= Cyc_Absynpp_typ2doc(
_temp779); _temp879[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp879, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL638; _LL678: s=({ struct Cyc_PP_Doc* _temp880[ 5u]; _temp880[ 4u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp880[ 3u]= Cyc_PP_text((
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp882; _temp882.tag= Cyc_Std_Int_pa;
_temp882.f1=( int) _temp783;{ void* _temp881[ 1u]={& _temp882}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp881, sizeof( void*),
1u));}})); _temp880[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp880[ 1u]= Cyc_Absynpp_typ2doc( _temp785); _temp880[ 0u]= Cyc_PP_text(
_tag_arr("offsetof(", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp880, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL638; _LL680: s=({ struct
Cyc_PP_Doc* _temp883[ 4u]; _temp883[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp883[ 2u]= Cyc_Absynpp_typ2doc( _temp787); _temp883[ 1u]=
Cyc_Absynpp_tvars2doc( _temp789); _temp883[ 0u]= Cyc_PP_text( _tag_arr("__gen(",
sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp883, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL638; _LL682: s=({ struct Cyc_PP_Doc* _temp884[ 2u]; _temp884[ 1u]=
Cyc_Absynpp_exp2doc_prec( myprec, _temp791); _temp884[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp884,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL638; _LL684: s=({ struct Cyc_PP_Doc*
_temp885[ 3u]; _temp885[ 2u]= Cyc_PP_textptr( _temp793); _temp885[ 1u]= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); _temp885[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp795); Cyc_PP_cat( _tag_arr( _temp885, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL638; _LL686: s=({ struct Cyc_PP_Doc* _temp886[ 3u]; _temp886[ 2u]=
Cyc_PP_textptr( _temp797); _temp886[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof(
unsigned char), 3u)); _temp886[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp799);
Cyc_PP_cat( _tag_arr( _temp886, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL638; _LL688: s=({ struct Cyc_PP_Doc* _temp887[ 4u]; _temp887[ 3u]= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); _temp887[ 2u]= Cyc_Absynpp_exp2doc(
_temp801); _temp887[ 1u]= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u));
_temp887[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp803); Cyc_PP_cat( _tag_arr(
_temp887, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL638; _LL690: s=({ struct
Cyc_PP_Doc* _temp888[ 3u]; _temp888[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp888[ 1u]= Cyc_Absynpp_exps2doc_prec( 20, _temp805);
_temp888[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp888, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL638; _LL692: s=({
struct Cyc_PP_Doc* _temp889[ 4u]; _temp889[ 3u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp807)); _temp889[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp889[ 1u]= Cyc_Absynpp_typ2doc((* _temp809).f3); _temp889[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp889,
sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL638; _LL694: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp811)); goto _LL638; _LL696: s=({ struct Cyc_PP_Doc*
_temp890[ 7u]; _temp890[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp890[ 5u]= Cyc_Absynpp_exp2doc( _temp813); _temp890[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp890[ 3u]= Cyc_Absynpp_exp2doc(
_temp815); _temp890[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp890[ 1u]= Cyc_PP_text(*(* _temp817->name).f2); _temp890[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp890,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL638; _LL698: s=({ struct Cyc_PP_Doc*
_temp891[ 2u]; _temp891[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp819));
_temp891[ 0u]= Cyc_Absynpp_qvar2doc( _temp821); Cyc_PP_cat( _tag_arr( _temp891,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL638; _LL700: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp823)); goto _LL638; _LL702: if( _temp827 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp825->name);} else{ s=({ struct Cyc_PP_Doc* _temp892[ 2u]; _temp892[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp827)); _temp892[ 0u]= Cyc_Absynpp_qvar2doc( _temp825->name);
Cyc_PP_cat( _tag_arr( _temp892, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL638; _LL704: s= Cyc_Absynpp_qvar2doc( _temp829); goto _LL638; _LL706: s= Cyc_Absynpp_qvar2doc(
_temp831); goto _LL638; _LL708: if( _temp835 ==  0){ s=({ struct Cyc_PP_Doc*
_temp893[ 3u]; _temp893[ 2u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp893[ 1u]= Cyc_Absynpp_typ2doc( _temp833); _temp893[ 0u]= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); Cyc_PP_cat( _tag_arr(
_temp893, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ s=({ struct Cyc_PP_Doc*
_temp894[ 6u]; _temp894[ 5u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp894[ 4u]= Cyc_Absynpp_typ2doc( _temp833); _temp894[ 3u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); _temp894[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp894[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp835)); _temp894[ 0u]= Cyc_PP_text(
_tag_arr("rmalloc(", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp894, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL638; _LL710: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp837)); goto _LL638; _LL712: s=({ struct Cyc_PP_Doc*
_temp895[ 3u]; _temp895[ 2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char),
4u)); _temp895[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp841)); _temp895[
0u]= Cyc_PP_text( _tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp895, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL638; _LL714: s=({
struct Cyc_PP_Doc* _temp896[ 3u]; _temp896[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp896[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp897=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp897->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp898=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp898[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp899; _temp899.tag= Cyc_Absyn_Fn_d;
_temp899.f1= _temp843; _temp899;}); _temp898;})); _temp897->loc= e->loc;
_temp897;}))); _temp896[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp896, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL638; _LL716: s=({ struct Cyc_PP_Doc* _temp900[ 3u]; _temp900[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp900[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp845)); _temp900[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp900, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL638; _LL638:;} if( inprec >=  myprec){ s=({ struct Cyc_PP_Doc*
_temp901[ 3u]; _temp901[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp901[ 1u]= s; _temp901[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp901, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp902= d; struct Cyc_Absyn_Exp* _temp908; struct _tagged_arr* _temp910;
_LL904: if(*(( int*) _temp902) ==  Cyc_Absyn_ArrayElement){ _LL909: _temp908=((
struct Cyc_Absyn_ArrayElement_struct*) _temp902)->f1; goto _LL905;} else{ goto
_LL906;} _LL906: if(*(( int*) _temp902) ==  Cyc_Absyn_FieldName){ _LL911:
_temp910=(( struct Cyc_Absyn_FieldName_struct*) _temp902)->f1; goto _LL907;}
else{ goto _LL903;} _LL905: return({ struct Cyc_PP_Doc* _temp912[ 3u]; _temp912[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp912[ 1u]= Cyc_Absynpp_exp2doc(
_temp908); _temp912[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp912, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL907:
return({ struct Cyc_PP_Doc* _temp913[ 2u]; _temp913[ 1u]= Cyc_PP_textptr(
_temp910); _temp913[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp913, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL903:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 ==  0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp914[ 2u]; _temp914[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp914[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp914, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp915= c;
unsigned char _temp933; void* _temp935; short _temp937; void* _temp939; int
_temp941; void* _temp943; int _temp945; void* _temp947; long long _temp949; void*
_temp951; struct _tagged_arr _temp953; struct _tagged_arr _temp955; _LL917: if((
unsigned int) _temp915 >  1u?*(( int*) _temp915) ==  Cyc_Absyn_Char_c: 0){
_LL936: _temp935=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp915)->f1; goto
_LL934; _LL934: _temp933=(( struct Cyc_Absyn_Char_c_struct*) _temp915)->f2; goto
_LL918;} else{ goto _LL919;} _LL919: if(( unsigned int) _temp915 >  1u?*(( int*)
_temp915) ==  Cyc_Absyn_Short_c: 0){ _LL940: _temp939=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp915)->f1; goto _LL938; _LL938: _temp937=(( struct Cyc_Absyn_Short_c_struct*)
_temp915)->f2; goto _LL920;} else{ goto _LL921;} _LL921: if(( unsigned int)
_temp915 >  1u?*(( int*) _temp915) ==  Cyc_Absyn_Int_c: 0){ _LL944: _temp943=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp915)->f1; if( _temp943 == ( void*)
Cyc_Absyn_Signed){ goto _LL942;} else{ goto _LL923;} _LL942: _temp941=(( struct
Cyc_Absyn_Int_c_struct*) _temp915)->f2; goto _LL922;} else{ goto _LL923;} _LL923:
if(( unsigned int) _temp915 >  1u?*(( int*) _temp915) ==  Cyc_Absyn_Int_c: 0){
_LL948: _temp947=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp915)->f1; if(
_temp947 == ( void*) Cyc_Absyn_Unsigned){ goto _LL946;} else{ goto _LL925;}
_LL946: _temp945=(( struct Cyc_Absyn_Int_c_struct*) _temp915)->f2; goto _LL924;}
else{ goto _LL925;} _LL925: if(( unsigned int) _temp915 >  1u?*(( int*) _temp915)
==  Cyc_Absyn_LongLong_c: 0){ _LL952: _temp951=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp915)->f1; goto _LL950; _LL950: _temp949=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp915)->f2; goto _LL926;} else{ goto _LL927;} _LL927: if(( unsigned int)
_temp915 >  1u?*(( int*) _temp915) ==  Cyc_Absyn_Float_c: 0){ _LL954: _temp953=((
struct Cyc_Absyn_Float_c_struct*) _temp915)->f1; goto _LL928;} else{ goto _LL929;}
_LL929: if( _temp915 == ( void*) Cyc_Absyn_Null_c){ goto _LL930;} else{ goto
_LL931;} _LL931: if(( unsigned int) _temp915 >  1u?*(( int*) _temp915) ==  Cyc_Absyn_String_c:
0){ _LL956: _temp955=(( struct Cyc_Absyn_String_c_struct*) _temp915)->f1; goto
_LL932;} else{ goto _LL916;} _LL918: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp958; _temp958.tag= Cyc_Std_String_pa;
_temp958.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp933);{ void*
_temp957[ 1u]={& _temp958}; Cyc_Std_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp957, sizeof( void*), 1u));}})); _LL920:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp960;
_temp960.tag= Cyc_Std_Int_pa; _temp960.f1=( int)(( unsigned int)(( int) _temp937));{
void* _temp959[ 1u]={& _temp960}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp959, sizeof( void*), 1u));}})); _LL922:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp962;
_temp962.tag= Cyc_Std_Int_pa; _temp962.f1=( int)(( unsigned int) _temp941);{
void* _temp961[ 1u]={& _temp962}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp961, sizeof( void*), 1u));}})); _LL924:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp964;
_temp964.tag= Cyc_Std_Int_pa; _temp964.f1=( unsigned int) _temp945;{ void*
_temp963[ 1u]={& _temp964}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp963, sizeof( void*), 1u));}})); _LL926:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL928: return Cyc_PP_text( _temp953); _LL930: return Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); _LL932: return({ struct Cyc_PP_Doc*
_temp965[ 3u]; _temp965[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp965[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp955));
_temp965[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp965, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL916:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == ( void*) Cyc_Absyn_Size){ if(
es ==  0? 1: es->tl !=  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp966=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp966[ 0]=({ struct Cyc_Core_Failure_struct _temp967; _temp967.tag= Cyc_Core_Failure;
_temp967.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp969;
_temp969.tag= Cyc_Std_String_pa; _temp969.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp968[ 1u]={& _temp969}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp968, sizeof( void*), 1u));}});
_temp967;}); _temp966;}));}{ struct Cyc_PP_Doc* _temp970= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*) es->hd); return({ struct Cyc_PP_Doc* _temp971[ 2u];
_temp971[ 1u]= Cyc_PP_text( _tag_arr(".size", sizeof( unsigned char), 6u));
_temp971[ 0u]= _temp970; Cyc_PP_cat( _tag_arr( _temp971, sizeof( struct Cyc_PP_Doc*),
2u));});}} else{ struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds ==  0){( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp972=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp972[ 0]=({ struct Cyc_Core_Failure_struct
_temp973; _temp973.tag= Cyc_Core_Failure; _temp973.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp975; _temp975.tag= Cyc_Std_String_pa;
_temp975.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp974[
1u]={& _temp975}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp974, sizeof( void*), 1u));}});
_temp973;}); _temp972;}));} else{ if( ds->tl ==  0){ return({ struct Cyc_PP_Doc*
_temp976[ 3u]; _temp976[ 2u]=( struct Cyc_PP_Doc*) ds->hd; _temp976[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp976[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp976, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null( ds->tl))->tl !=  0){( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp977=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp977[ 0]=({ struct Cyc_Core_Failure_struct
_temp978; _temp978.tag= Cyc_Core_Failure; _temp978.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp980; _temp980.tag= Cyc_Std_String_pa;
_temp980.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp979[
1u]={& _temp980}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp979, sizeof( void*), 1u));}});
_temp978;}); _temp977;}));} else{ return({ struct Cyc_PP_Doc* _temp981[ 5u];
_temp981[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds->tl))->hd;
_temp981[ 3u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp981[
2u]= ps; _temp981[ 1u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp981[ 0u]=( struct Cyc_PP_Doc*) ds->hd; Cyc_PP_cat( _tag_arr( _temp981,
sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct _tagged_arr Cyc_Absynpp_prim2str(
void* p){ void* _temp982= p; _LL984: if( _temp982 == ( void*) Cyc_Absyn_Plus){
goto _LL985;} else{ goto _LL986;} _LL986: if( _temp982 == ( void*) Cyc_Absyn_Times){
goto _LL987;} else{ goto _LL988;} _LL988: if( _temp982 == ( void*) Cyc_Absyn_Minus){
goto _LL989;} else{ goto _LL990;} _LL990: if( _temp982 == ( void*) Cyc_Absyn_Div){
goto _LL991;} else{ goto _LL992;} _LL992: if( _temp982 == ( void*) Cyc_Absyn_Mod){
goto _LL993;} else{ goto _LL994;} _LL994: if( _temp982 == ( void*) Cyc_Absyn_Eq){
goto _LL995;} else{ goto _LL996;} _LL996: if( _temp982 == ( void*) Cyc_Absyn_Neq){
goto _LL997;} else{ goto _LL998;} _LL998: if( _temp982 == ( void*) Cyc_Absyn_Gt){
goto _LL999;} else{ goto _LL1000;} _LL1000: if( _temp982 == ( void*) Cyc_Absyn_Lt){
goto _LL1001;} else{ goto _LL1002;} _LL1002: if( _temp982 == ( void*) Cyc_Absyn_Gte){
goto _LL1003;} else{ goto _LL1004;} _LL1004: if( _temp982 == ( void*) Cyc_Absyn_Lte){
goto _LL1005;} else{ goto _LL1006;} _LL1006: if( _temp982 == ( void*) Cyc_Absyn_Not){
goto _LL1007;} else{ goto _LL1008;} _LL1008: if( _temp982 == ( void*) Cyc_Absyn_Bitnot){
goto _LL1009;} else{ goto _LL1010;} _LL1010: if( _temp982 == ( void*) Cyc_Absyn_Bitand){
goto _LL1011;} else{ goto _LL1012;} _LL1012: if( _temp982 == ( void*) Cyc_Absyn_Bitor){
goto _LL1013;} else{ goto _LL1014;} _LL1014: if( _temp982 == ( void*) Cyc_Absyn_Bitxor){
goto _LL1015;} else{ goto _LL1016;} _LL1016: if( _temp982 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL1017;} else{ goto _LL1018;} _LL1018: if( _temp982 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL1019;} else{ goto _LL1020;} _LL1020: if( _temp982 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL1021;} else{ goto _LL1022;} _LL1022: if( _temp982 == ( void*) Cyc_Absyn_Size){
goto _LL1023;} else{ goto _LL983;} _LL985: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL987: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL989: return _tag_arr("-", sizeof( unsigned char), 2u); _LL991: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL993: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL995: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL997: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL999: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL1001: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL1003: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL1005: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL1007: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1009: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1011: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1013: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1015: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1017: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1019: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1021: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1023: return _tag_arr("size", sizeof( unsigned char), 5u); _LL983:;} struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 == 
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*
_temp1024[ 3u]; _temp1024[ 2u]= Cyc_Absynpp_exp2doc((* t).f2); _temp1024[ 1u]=
Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1024[ 0u]= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); Cyc_PP_cat(
_tag_arr( _temp1024, sizeof( struct Cyc_PP_Doc*), 3u));});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp1025=( void*) s->r; _LL1027: if((
unsigned int) _temp1025 >  1u?*(( int*) _temp1025) ==  Cyc_Absyn_Decl_s: 0){
goto _LL1028;} else{ goto _LL1029;} _LL1029: goto _LL1030; _LL1028: return 1;
_LL1030: return 0; _LL1026:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1031=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1075; struct Cyc_Absyn_Stmt* _temp1077; struct Cyc_Absyn_Stmt*
_temp1079; struct Cyc_Absyn_Exp* _temp1081; struct Cyc_Absyn_Stmt* _temp1083;
struct Cyc_Absyn_Stmt* _temp1085; struct Cyc_Absyn_Exp* _temp1087; struct Cyc_Absyn_Stmt*
_temp1089; struct _tuple2 _temp1091; struct Cyc_Absyn_Exp* _temp1093; struct
_tagged_arr* _temp1095; struct Cyc_Absyn_Stmt* _temp1097; struct _tuple2
_temp1099; struct Cyc_Absyn_Exp* _temp1101; struct _tuple2 _temp1103; struct Cyc_Absyn_Exp*
_temp1105; struct Cyc_Absyn_Exp* _temp1107; struct Cyc_List_List* _temp1109;
struct Cyc_Absyn_Exp* _temp1111; struct Cyc_List_List* _temp1113; struct Cyc_Absyn_Exp*
_temp1115; struct Cyc_List_List* _temp1117; struct Cyc_List_List* _temp1119;
struct Cyc_Absyn_Stmt* _temp1121; struct Cyc_Absyn_Decl* _temp1123; struct Cyc_Absyn_Stmt*
_temp1125; struct Cyc_Absyn_Stmt* _temp1127; struct Cyc_Absyn_Stmt* _temp1129;
struct _tagged_arr* _temp1131; struct _tuple2 _temp1133; struct Cyc_Absyn_Exp*
_temp1135; struct Cyc_Absyn_Stmt* _temp1137; struct Cyc_List_List* _temp1139;
struct Cyc_Absyn_Stmt* _temp1141; struct Cyc_Absyn_Stmt* _temp1143; struct Cyc_Absyn_Vardecl*
_temp1145; struct Cyc_Absyn_Tvar* _temp1147; _LL1033: if( _temp1031 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1034;} else{ goto _LL1035;} _LL1035: if((
unsigned int) _temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Exp_s: 0){
_LL1076: _temp1075=(( struct Cyc_Absyn_Exp_s_struct*) _temp1031)->f1; goto
_LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int) _temp1031 >  1u?*((
int*) _temp1031) ==  Cyc_Absyn_Seq_s: 0){ _LL1080: _temp1079=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1031)->f1; goto _LL1078; _LL1078: _temp1077=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1031)->f2; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Return_s: 0){ _LL1082:
_temp1081=(( struct Cyc_Absyn_Return_s_struct*) _temp1031)->f1; goto _LL1040;}
else{ goto _LL1041;} _LL1041: if(( unsigned int) _temp1031 >  1u?*(( int*)
_temp1031) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1088: _temp1087=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1031)->f1; goto _LL1086; _LL1086: _temp1085=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1031)->f2; goto _LL1084; _LL1084: _temp1083=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1031)->f3; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_While_s: 0){ _LL1092:
_temp1091=(( struct Cyc_Absyn_While_s_struct*) _temp1031)->f1; _LL1094:
_temp1093= _temp1091.f1; goto _LL1090; _LL1090: _temp1089=(( struct Cyc_Absyn_While_s_struct*)
_temp1031)->f2; goto _LL1044;} else{ goto _LL1045;} _LL1045: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Break_s: 0){ goto _LL1046;}
else{ goto _LL1047;} _LL1047: if(( unsigned int) _temp1031 >  1u?*(( int*)
_temp1031) ==  Cyc_Absyn_Continue_s: 0){ goto _LL1048;} else{ goto _LL1049;}
_LL1049: if(( unsigned int) _temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Goto_s:
0){ _LL1096: _temp1095=(( struct Cyc_Absyn_Goto_s_struct*) _temp1031)->f1; goto
_LL1050;} else{ goto _LL1051;} _LL1051: if(( unsigned int) _temp1031 >  1u?*((
int*) _temp1031) ==  Cyc_Absyn_For_s: 0){ _LL1108: _temp1107=(( struct Cyc_Absyn_For_s_struct*)
_temp1031)->f1; goto _LL1104; _LL1104: _temp1103=(( struct Cyc_Absyn_For_s_struct*)
_temp1031)->f2; _LL1106: _temp1105= _temp1103.f1; goto _LL1100; _LL1100:
_temp1099=(( struct Cyc_Absyn_For_s_struct*) _temp1031)->f3; _LL1102: _temp1101=
_temp1099.f1; goto _LL1098; _LL1098: _temp1097=(( struct Cyc_Absyn_For_s_struct*)
_temp1031)->f4; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Switch_s: 0){ _LL1112:
_temp1111=(( struct Cyc_Absyn_Switch_s_struct*) _temp1031)->f1; goto _LL1110;
_LL1110: _temp1109=(( struct Cyc_Absyn_Switch_s_struct*) _temp1031)->f2; goto
_LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int) _temp1031 >  1u?*((
int*) _temp1031) ==  Cyc_Absyn_SwitchC_s: 0){ _LL1116: _temp1115=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1031)->f1; goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1031)->f2; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1118:
_temp1117=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1031)->f1; if( _temp1117
==  0){ goto _LL1058;} else{ goto _LL1059;}} else{ goto _LL1059;} _LL1059: if((
unsigned int) _temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Fallthru_s: 0){
_LL1120: _temp1119=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1031)->f1; goto
_LL1060;} else{ goto _LL1061;} _LL1061: if(( unsigned int) _temp1031 >  1u?*((
int*) _temp1031) ==  Cyc_Absyn_Decl_s: 0){ _LL1124: _temp1123=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1031)->f1; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1031)->f2; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Cut_s: 0){ _LL1126: _temp1125=((
struct Cyc_Absyn_Cut_s_struct*) _temp1031)->f1; goto _LL1064;} else{ goto
_LL1065;} _LL1065: if(( unsigned int) _temp1031 >  1u?*(( int*) _temp1031) == 
Cyc_Absyn_Splice_s: 0){ _LL1128: _temp1127=(( struct Cyc_Absyn_Splice_s_struct*)
_temp1031)->f1; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Label_s: 0){ _LL1132:
_temp1131=(( struct Cyc_Absyn_Label_s_struct*) _temp1031)->f1; goto _LL1130;
_LL1130: _temp1129=(( struct Cyc_Absyn_Label_s_struct*) _temp1031)->f2; goto
_LL1068;} else{ goto _LL1069;} _LL1069: if(( unsigned int) _temp1031 >  1u?*((
int*) _temp1031) ==  Cyc_Absyn_Do_s: 0){ _LL1138: _temp1137=(( struct Cyc_Absyn_Do_s_struct*)
_temp1031)->f1; goto _LL1134; _LL1134: _temp1133=(( struct Cyc_Absyn_Do_s_struct*)
_temp1031)->f2; _LL1136: _temp1135= _temp1133.f1; goto _LL1070;} else{ goto
_LL1071;} _LL1071: if(( unsigned int) _temp1031 >  1u?*(( int*) _temp1031) == 
Cyc_Absyn_TryCatch_s: 0){ _LL1142: _temp1141=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1031)->f1; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1031)->f2; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Region_s: 0){ _LL1148:
_temp1147=(( struct Cyc_Absyn_Region_s_struct*) _temp1031)->f1; goto _LL1146;
_LL1146: _temp1145=(( struct Cyc_Absyn_Region_s_struct*) _temp1031)->f2; goto
_LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_Region_s_struct*) _temp1031)->f3;
goto _LL1074;} else{ goto _LL1032;} _LL1034: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1032; _LL1036: s=({ struct Cyc_PP_Doc*
_temp1149[ 2u]; _temp1149[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1149[ 0u]= Cyc_Absynpp_exp2doc( _temp1075); Cyc_PP_cat( _tag_arr(
_temp1149, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1032; _LL1038: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1079)){ s=({ struct Cyc_PP_Doc* _temp1150[
6u]; _temp1150[ 5u]= Cyc_Absynpp_is_declaration( _temp1077)?({ struct Cyc_PP_Doc*
_temp1151[ 4u]; _temp1151[ 3u]= Cyc_PP_line_doc(); _temp1151[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1151[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1077)); _temp1151[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1151, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1077); _temp1150[ 4u]= Cyc_PP_line_doc(); _temp1150[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1150[ 2u]= Cyc_PP_line_doc();
_temp1150[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1079)); _temp1150[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1150, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1077)){ s=({ struct Cyc_PP_Doc* _temp1152[ 6u]; _temp1152[ 5u]= Cyc_PP_line_doc();
_temp1152[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1152[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1077)); _temp1152[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1152[ 1u]= Cyc_PP_line_doc();
_temp1152[ 0u]= Cyc_Absynpp_stmt2doc( _temp1079); Cyc_PP_cat( _tag_arr(
_temp1152, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1153=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1153->hd=( void*) _temp1079;
_temp1153->tl=({ struct Cyc_List_List* _temp1154=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1154->hd=( void*) _temp1077;
_temp1154->tl= 0; _temp1154;}); _temp1153;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1155=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1155->hd=( void*) _temp1079;
_temp1155->tl=({ struct Cyc_List_List* _temp1156=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1156->hd=( void*) _temp1077;
_temp1156->tl= 0; _temp1156;}); _temp1155;}));} goto _LL1032; _LL1040: if(
_temp1081 ==  0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1157[ 3u]; _temp1157[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1157[ 1u]= _temp1081 ==  0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1081)); _temp1157[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1157, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1032;
_LL1042: { int print_else;{ void* _temp1158=( void*) _temp1083->r; _LL1160: if(
_temp1158 == ( void*) Cyc_Absyn_Skip_s){ goto _LL1161;} else{ goto _LL1162;}
_LL1162: goto _LL1163; _LL1161: print_else= 0; goto _LL1159; _LL1163: print_else=
1; goto _LL1159; _LL1159:;} s=({ struct Cyc_PP_Doc* _temp1164[ 7u]; _temp1164[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1166[ 5u]; _temp1166[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1166[ 3u]= Cyc_PP_line_doc();
_temp1166[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1167[ 2u]; _temp1167[
1u]= Cyc_Absynpp_stmt2doc( _temp1083); _temp1167[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1167, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1166[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1166[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1166, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1164[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1164[ 4u]= Cyc_PP_line_doc(); _temp1164[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1165[ 2u]; _temp1165[ 1u]= Cyc_Absynpp_stmt2doc( _temp1085); _temp1165[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1165, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1164[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1164[ 1u]= Cyc_Absynpp_exp2doc( _temp1087); _temp1164[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1164,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1032;} _LL1044: s=({ struct Cyc_PP_Doc*
_temp1168[ 6u]; _temp1168[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1168[ 4u]= Cyc_PP_line_doc(); _temp1168[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1169[ 2u]; _temp1169[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1089); _temp1169[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1169,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1168[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1168[ 1u]= Cyc_Absynpp_exp2doc( _temp1093);
_temp1168[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1168, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL1032; _LL1046: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL1032; _LL1048: s= Cyc_PP_text( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL1032; _LL1050: s= Cyc_PP_text(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp1171; _temp1171.tag= Cyc_Std_String_pa;
_temp1171.f1=( struct _tagged_arr)* _temp1095;{ void* _temp1170[ 1u]={&
_temp1171}; Cyc_Std_aprintf( _tag_arr("goto %s;", sizeof( unsigned char), 9u),
_tag_arr( _temp1170, sizeof( void*), 1u));}})); goto _LL1032; _LL1052: s=({
struct Cyc_PP_Doc* _temp1172[ 10u]; _temp1172[ 9u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1172[ 8u]= Cyc_PP_line_doc(); _temp1172[ 7u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1173[ 2u]; _temp1173[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1097); _temp1173[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1173,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1172[ 6u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1172[ 5u]= Cyc_Absynpp_exp2doc( _temp1101);
_temp1172[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char), 3u));
_temp1172[ 3u]= Cyc_Absynpp_exp2doc( _temp1105); _temp1172[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1172[ 1u]= Cyc_Absynpp_exp2doc(
_temp1107); _temp1172[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1172, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL1032; _LL1054: s=({ struct Cyc_PP_Doc* _temp1174[ 7u]; _temp1174[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1174[ 5u]= Cyc_PP_line_doc();
_temp1174[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1109); _temp1174[ 3u]= Cyc_PP_line_doc();
_temp1174[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1174[ 1u]= Cyc_Absynpp_exp2doc( _temp1111); _temp1174[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1174, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1032; _LL1056: s=({
struct Cyc_PP_Doc* _temp1175[ 7u]; _temp1175[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1175[ 5u]= Cyc_PP_line_doc(); _temp1175[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1113); _temp1175[ 3u]= Cyc_PP_line_doc();
_temp1175[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1175[ 1u]= Cyc_Absynpp_exp2doc( _temp1115); _temp1175[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1175, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1032; _LL1058: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL1032; _LL1060: s=({
struct Cyc_PP_Doc* _temp1176[ 3u]; _temp1176[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1176[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1119); _temp1176[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1176, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL1032; _LL1062: s=({ struct Cyc_PP_Doc* _temp1177[ 3u];
_temp1177[ 2u]= Cyc_Absynpp_stmt2doc( _temp1121); _temp1177[ 1u]= Cyc_PP_line_doc();
_temp1177[ 0u]= Cyc_Absynpp_decl2doc( _temp1123); Cyc_PP_cat( _tag_arr(
_temp1177, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1032; _LL1064: s=({
struct Cyc_PP_Doc* _temp1178[ 2u]; _temp1178[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1125)); _temp1178[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1178, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL1032; _LL1066: s=({ struct Cyc_PP_Doc* _temp1179[ 2u]; _temp1179[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1127)); _temp1179[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1179, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1032; _LL1068: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1129): 0){ s=({ struct Cyc_PP_Doc* _temp1180[ 6u];
_temp1180[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1180[ 4u]= Cyc_PP_line_doc(); _temp1180[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1129)); _temp1180[ 2u]= Cyc_PP_line_doc(); _temp1180[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1180[ 0u]= Cyc_PP_textptr(
_temp1131); Cyc_PP_cat( _tag_arr( _temp1180, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1181[ 3u]; _temp1181[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1129); _temp1181[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1181[ 0u]= Cyc_PP_textptr( _temp1131); Cyc_PP_cat( _tag_arr(
_temp1181, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1032; _LL1070: s=({
struct Cyc_PP_Doc* _temp1182[ 7u]; _temp1182[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1182[ 5u]= Cyc_Absynpp_exp2doc( _temp1135);
_temp1182[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1182[ 3u]= Cyc_PP_line_doc(); _temp1182[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1137)); _temp1182[ 1u]= Cyc_PP_line_doc(); _temp1182[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1182,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1032; _LL1072: s=({ struct Cyc_PP_Doc*
_temp1183[ 9u]; _temp1183[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1183[ 7u]= Cyc_PP_line_doc(); _temp1183[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1139)); _temp1183[ 5u]= Cyc_PP_line_doc(); _temp1183[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1183[ 3u]= Cyc_PP_line_doc();
_temp1183[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1141)); _temp1183[ 1u]=
Cyc_PP_line_doc(); _temp1183[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1183, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1032; _LL1074: s=({ struct Cyc_PP_Doc* _temp1184[ 9u];
_temp1184[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1184[ 7u]= Cyc_PP_line_doc(); _temp1184[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1143)); _temp1184[ 5u]= Cyc_PP_line_doc(); _temp1184[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1184[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1145->name); _temp1184[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1184[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1147)); _temp1184[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1184, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1032; _LL1032:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1185=( void*) p->r;
int _temp1219; void* _temp1221; unsigned char _temp1223; struct _tagged_arr
_temp1225; struct Cyc_Absyn_Vardecl* _temp1227; struct Cyc_List_List* _temp1229;
struct Cyc_Absyn_Pat* _temp1231; struct Cyc_Absyn_Vardecl* _temp1233; struct
_tuple0* _temp1235; struct Cyc_List_List* _temp1237; struct Cyc_List_List*
_temp1239; struct _tuple0* _temp1241; struct Cyc_List_List* _temp1243; struct
Cyc_List_List* _temp1245; struct _tuple0* _temp1247; struct Cyc_List_List*
_temp1249; struct Cyc_List_List* _temp1251; struct Cyc_Absyn_Structdecl*
_temp1253; struct Cyc_Absyn_Enumfield* _temp1255; struct Cyc_Absyn_Enumfield*
_temp1257; struct Cyc_List_List* _temp1259; struct Cyc_List_List* _temp1261;
struct Cyc_Absyn_Tunionfield* _temp1263; _LL1187: if( _temp1185 == ( void*) Cyc_Absyn_Wild_p){
goto _LL1188;} else{ goto _LL1189;} _LL1189: if( _temp1185 == ( void*) Cyc_Absyn_Null_p){
goto _LL1190;} else{ goto _LL1191;} _LL1191: if(( unsigned int) _temp1185 >  2u?*((
int*) _temp1185) ==  Cyc_Absyn_Int_p: 0){ _LL1222: _temp1221=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1185)->f1; goto _LL1220; _LL1220: _temp1219=((
struct Cyc_Absyn_Int_p_struct*) _temp1185)->f2; goto _LL1192;} else{ goto
_LL1193;} _LL1193: if(( unsigned int) _temp1185 >  2u?*(( int*) _temp1185) == 
Cyc_Absyn_Char_p: 0){ _LL1224: _temp1223=(( struct Cyc_Absyn_Char_p_struct*)
_temp1185)->f1; goto _LL1194;} else{ goto _LL1195;} _LL1195: if(( unsigned int)
_temp1185 >  2u?*(( int*) _temp1185) ==  Cyc_Absyn_Float_p: 0){ _LL1226:
_temp1225=(( struct Cyc_Absyn_Float_p_struct*) _temp1185)->f1; goto _LL1196;}
else{ goto _LL1197;} _LL1197: if(( unsigned int) _temp1185 >  2u?*(( int*)
_temp1185) ==  Cyc_Absyn_Var_p: 0){ _LL1228: _temp1227=(( struct Cyc_Absyn_Var_p_struct*)
_temp1185)->f1; goto _LL1198;} else{ goto _LL1199;} _LL1199: if(( unsigned int)
_temp1185 >  2u?*(( int*) _temp1185) ==  Cyc_Absyn_Tuple_p: 0){ _LL1230:
_temp1229=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1185)->f1; goto _LL1200;}
else{ goto _LL1201;} _LL1201: if(( unsigned int) _temp1185 >  2u?*(( int*)
_temp1185) ==  Cyc_Absyn_Pointer_p: 0){ _LL1232: _temp1231=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1185)->f1; goto _LL1202;} else{ goto _LL1203;} _LL1203: if(( unsigned int)
_temp1185 >  2u?*(( int*) _temp1185) ==  Cyc_Absyn_Reference_p: 0){ _LL1234:
_temp1233=(( struct Cyc_Absyn_Reference_p_struct*) _temp1185)->f1; goto _LL1204;}
else{ goto _LL1205;} _LL1205: if(( unsigned int) _temp1185 >  2u?*(( int*)
_temp1185) ==  Cyc_Absyn_UnknownId_p: 0){ _LL1236: _temp1235=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1185)->f1; goto _LL1206;} else{ goto _LL1207;} _LL1207: if(( unsigned int)
_temp1185 >  2u?*(( int*) _temp1185) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL1242:
_temp1241=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1185)->f1; goto
_LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1185)->f2;
goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1185)->f3; goto _LL1208;} else{ goto _LL1209;} _LL1209: if(( unsigned int)
_temp1185 >  2u?*(( int*) _temp1185) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL1248:
_temp1247=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1185)->f1; goto
_LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1185)->f2; goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1185)->f3; goto _LL1210;} else{ goto _LL1211;} _LL1211: if(( unsigned int)
_temp1185 >  2u?*(( int*) _temp1185) ==  Cyc_Absyn_Struct_p: 0){ _LL1254:
_temp1253=(( struct Cyc_Absyn_Struct_p_struct*) _temp1185)->f1; goto _LL1252;
_LL1252: _temp1251=(( struct Cyc_Absyn_Struct_p_struct*) _temp1185)->f3; goto
_LL1250; _LL1250: _temp1249=(( struct Cyc_Absyn_Struct_p_struct*) _temp1185)->f4;
goto _LL1212;} else{ goto _LL1213;} _LL1213: if(( unsigned int) _temp1185 >  2u?*((
int*) _temp1185) ==  Cyc_Absyn_Enum_p: 0){ _LL1256: _temp1255=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1185)->f2; goto _LL1214;} else{ goto _LL1215;} _LL1215: if(( unsigned int)
_temp1185 >  2u?*(( int*) _temp1185) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1258:
_temp1257=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1185)->f2; goto _LL1216;}
else{ goto _LL1217;} _LL1217: if(( unsigned int) _temp1185 >  2u?*(( int*)
_temp1185) ==  Cyc_Absyn_Tunion_p: 0){ _LL1264: _temp1263=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1185)->f2; goto _LL1262; _LL1262: _temp1261=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1185)->f3; goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1185)->f4; goto _LL1218;} else{ goto _LL1186;} _LL1188: s= Cyc_PP_text(
_tag_arr("_", sizeof( unsigned char), 2u)); goto _LL1186; _LL1190: s= Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); goto _LL1186; _LL1192: if(
_temp1221 == ( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp1266; _temp1266.tag= Cyc_Std_Int_pa; _temp1266.f1=(
int)(( unsigned int) _temp1219);{ void* _temp1265[ 1u]={& _temp1266}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1265, sizeof( void*),
1u));}}));} else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1268; _temp1268.tag= Cyc_Std_Int_pa; _temp1268.f1=( unsigned int) _temp1219;{
void* _temp1267[ 1u]={& _temp1268}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp1267, sizeof( void*), 1u));}}));} goto
_LL1186; _LL1194: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1270; _temp1270.tag= Cyc_Std_String_pa; _temp1270.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1223);{ void* _temp1269[ 1u]={& _temp1270}; Cyc_Std_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1269, sizeof( void*),
1u));}})); goto _LL1186; _LL1196: s= Cyc_PP_text( _temp1225); goto _LL1186;
_LL1198: s= Cyc_Absynpp_qvar2doc( _temp1227->name); goto _LL1186; _LL1200: s=({
struct Cyc_PP_Doc* _temp1271[ 3u]; _temp1271[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1271[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1229); _temp1271[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1271, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1186; _LL1202: s=({ struct Cyc_PP_Doc* _temp1272[ 2u]; _temp1272[ 1u]=
Cyc_Absynpp_pat2doc( _temp1231); _temp1272[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1272, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1186; _LL1204: s=({ struct Cyc_PP_Doc* _temp1273[
2u]; _temp1273[ 1u]= Cyc_Absynpp_qvar2doc( _temp1233->name); _temp1273[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1273,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1186; _LL1206: s= Cyc_Absynpp_qvar2doc(
_temp1235); goto _LL1186; _LL1208: s=({ struct Cyc_PP_Doc* _temp1274[ 3u];
_temp1274[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1237));
_temp1274[ 1u]= Cyc_Absynpp_tvars2doc( _temp1239); _temp1274[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1241); Cyc_PP_cat( _tag_arr( _temp1274, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1186; _LL1210: s=({ struct Cyc_PP_Doc* _temp1275[ 3u]; _temp1275[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1243)); _temp1275[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1245); _temp1275[ 0u]= Cyc_Absynpp_qvar2doc( _temp1247); Cyc_PP_cat(
_tag_arr( _temp1275, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1186; _LL1212:
s=({ struct Cyc_PP_Doc* _temp1276[ 3u]; _temp1276[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1249)); _temp1276[ 1u]= Cyc_Absynpp_tvars2doc( _temp1251); _temp1276[ 0u]=
_temp1253->name ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1253->name))->v); Cyc_PP_cat( _tag_arr(
_temp1276, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1186; _LL1214: s= Cyc_Absynpp_qvar2doc(
_temp1255->name); goto _LL1186; _LL1216: s= Cyc_Absynpp_qvar2doc( _temp1257->name);
goto _LL1186; _LL1218: if( _temp1259 ==  0){ s= Cyc_Absynpp_qvar2doc( _temp1263->name);}
else{ s=({ struct Cyc_PP_Doc* _temp1277[ 3u]; _temp1277[ 2u]= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1259)); _temp1277[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1261); _temp1277[ 0u]= Cyc_Absynpp_qvar2doc( _temp1263->name); Cyc_PP_cat(
_tag_arr( _temp1277, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1186;
_LL1186:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1278[ 2u]; _temp1278[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1278[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1278, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause ==  0?( void*)( c->pattern)->r
== ( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1279[ 2u];
_temp1279[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1280[ 2u]; _temp1280[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1280[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1280, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1279[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1279, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 
0){ return({ struct Cyc_PP_Doc* _temp1281[ 4u]; _temp1281[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1282[ 2u]; _temp1282[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1282[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1282, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1281[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1281[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1281[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1281, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1283[ 6u]; _temp1283[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1284[ 2u]; _temp1284[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1284[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1284, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1283[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1283[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1283[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1283[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1283[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1283,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1287;
struct Cyc_Absyn_Stmt* _temp1288; struct Cyc_Absyn_Exp* _temp1290; struct Cyc_Absyn_SwitchC_clause*
_temp1285= c; _temp1287=* _temp1285; _LL1291: _temp1290= _temp1287.cnst_exp;
goto _LL1289; _LL1289: _temp1288= _temp1287.body; goto _LL1286; _LL1286: if(
_temp1290 ==  0){ return({ struct Cyc_PP_Doc* _temp1292[ 2u]; _temp1292[ 1u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1293[ 2u]; _temp1293[ 1u]= Cyc_Absynpp_stmt2doc(
c->body); _temp1293[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1293,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1292[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1292, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1294[ 4u];
_temp1294[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1295[ 2u]; _temp1295[
1u]= Cyc_Absynpp_stmt2doc( _temp1288); _temp1295[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1295, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1294[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1294[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1290)); _temp1294[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1294,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag ==  0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1296[ 3u]; _temp1296[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1296[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1296[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1296, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1297=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1323; struct Cyc_Absyn_Structdecl* _temp1325;
struct Cyc_Absyn_Uniondecl* _temp1327; struct Cyc_Absyn_Vardecl* _temp1329;
struct Cyc_Absyn_Vardecl _temp1331; struct Cyc_List_List* _temp1332; struct Cyc_Absyn_Exp*
_temp1334; void* _temp1336; struct Cyc_Absyn_Tqual _temp1338; struct _tuple0*
_temp1340; void* _temp1342; struct Cyc_Absyn_Tuniondecl* _temp1344; struct Cyc_Absyn_Tuniondecl
_temp1346; int _temp1347; struct Cyc_Core_Opt* _temp1349; struct Cyc_List_List*
_temp1351; struct _tuple0* _temp1353; void* _temp1355; struct Cyc_Absyn_Exp*
_temp1357; struct Cyc_Absyn_Pat* _temp1359; struct Cyc_List_List* _temp1361;
struct Cyc_Absyn_Enumdecl* _temp1363; struct Cyc_Absyn_Enumdecl _temp1365;
struct Cyc_Core_Opt* _temp1366; struct _tuple0* _temp1368; void* _temp1370;
struct Cyc_Absyn_Typedefdecl* _temp1372; struct Cyc_List_List* _temp1374; struct
_tagged_arr* _temp1376; struct Cyc_List_List* _temp1378; struct _tuple0*
_temp1380; struct Cyc_List_List* _temp1382; _LL1299: if(*(( int*) _temp1297) == 
Cyc_Absyn_Fn_d){ _LL1324: _temp1323=(( struct Cyc_Absyn_Fn_d_struct*) _temp1297)->f1;
goto _LL1300;} else{ goto _LL1301;} _LL1301: if(*(( int*) _temp1297) ==  Cyc_Absyn_Struct_d){
_LL1326: _temp1325=(( struct Cyc_Absyn_Struct_d_struct*) _temp1297)->f1; goto
_LL1302;} else{ goto _LL1303;} _LL1303: if(*(( int*) _temp1297) ==  Cyc_Absyn_Union_d){
_LL1328: _temp1327=(( struct Cyc_Absyn_Union_d_struct*) _temp1297)->f1; goto
_LL1304;} else{ goto _LL1305;} _LL1305: if(*(( int*) _temp1297) ==  Cyc_Absyn_Var_d){
_LL1330: _temp1329=(( struct Cyc_Absyn_Var_d_struct*) _temp1297)->f1; _temp1331=*
_temp1329; _LL1343: _temp1342=( void*) _temp1331.sc; goto _LL1341; _LL1341:
_temp1340= _temp1331.name; goto _LL1339; _LL1339: _temp1338= _temp1331.tq; goto
_LL1337; _LL1337: _temp1336=( void*) _temp1331.type; goto _LL1335; _LL1335:
_temp1334= _temp1331.initializer; goto _LL1333; _LL1333: _temp1332= _temp1331.attributes;
goto _LL1306;} else{ goto _LL1307;} _LL1307: if(*(( int*) _temp1297) ==  Cyc_Absyn_Tunion_d){
_LL1345: _temp1344=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1297)->f1;
_temp1346=* _temp1344; _LL1356: _temp1355=( void*) _temp1346.sc; goto _LL1354;
_LL1354: _temp1353= _temp1346.name; goto _LL1352; _LL1352: _temp1351= _temp1346.tvs;
goto _LL1350; _LL1350: _temp1349= _temp1346.fields; goto _LL1348; _LL1348:
_temp1347= _temp1346.is_xtunion; goto _LL1308;} else{ goto _LL1309;} _LL1309:
if(*(( int*) _temp1297) ==  Cyc_Absyn_Let_d){ _LL1360: _temp1359=(( struct Cyc_Absyn_Let_d_struct*)
_temp1297)->f1; goto _LL1358; _LL1358: _temp1357=(( struct Cyc_Absyn_Let_d_struct*)
_temp1297)->f4; goto _LL1310;} else{ goto _LL1311;} _LL1311: if(*(( int*)
_temp1297) ==  Cyc_Absyn_Letv_d){ _LL1362: _temp1361=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1297)->f1; goto _LL1312;} else{ goto _LL1313;} _LL1313: if(*(( int*)
_temp1297) ==  Cyc_Absyn_Enum_d){ _LL1364: _temp1363=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1297)->f1; _temp1365=* _temp1363; _LL1371: _temp1370=( void*) _temp1365.sc;
goto _LL1369; _LL1369: _temp1368= _temp1365.name; goto _LL1367; _LL1367:
_temp1366= _temp1365.fields; goto _LL1314;} else{ goto _LL1315;} _LL1315: if(*((
int*) _temp1297) ==  Cyc_Absyn_Typedef_d){ _LL1373: _temp1372=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1297)->f1; goto _LL1316;} else{ goto _LL1317;} _LL1317: if(*(( int*)
_temp1297) ==  Cyc_Absyn_Namespace_d){ _LL1377: _temp1376=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1297)->f1; goto _LL1375; _LL1375: _temp1374=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1297)->f2; goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*)
_temp1297) ==  Cyc_Absyn_Using_d){ _LL1381: _temp1380=(( struct Cyc_Absyn_Using_d_struct*)
_temp1297)->f1; goto _LL1379; _LL1379: _temp1378=(( struct Cyc_Absyn_Using_d_struct*)
_temp1297)->f2; goto _LL1320;} else{ goto _LL1321;} _LL1321: if(*(( int*)
_temp1297) ==  Cyc_Absyn_ExternC_d){ _LL1383: _temp1382=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1297)->f1; goto _LL1322;} else{ goto _LL1298;} _LL1300: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1391=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1391[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1392; _temp1392.tag= Cyc_Absyn_FnType; _temp1392.f1=({ struct Cyc_Absyn_FnInfo
_temp1393; _temp1393.tvars= _temp1323->tvs; _temp1393.effect= _temp1323->effect;
_temp1393.ret_typ=( void*)(( void*) _temp1323->ret_type); _temp1393.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1323->args); _temp1393.c_varargs=
_temp1323->c_varargs; _temp1393.cyc_varargs= _temp1323->cyc_varargs; _temp1393.rgn_po=
_temp1323->rgn_po; _temp1393.attributes= 0; _temp1393;}); _temp1392;});
_temp1391;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1323->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1323->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1323->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1323->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1323->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1388; _temp1388.q_const= 0; _temp1388.q_volatile= 0; _temp1388.q_restrict=
0; _temp1388;}), t,({ struct Cyc_Core_Opt* _temp1389=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1389->v=( void*)({ struct Cyc_PP_Doc*
_temp1390[ 2u]; _temp1390[ 1u]= namedoc; _temp1390[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1390, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1389;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1386[ 4u]; _temp1386[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1386[ 2u]= Cyc_PP_line_doc();
_temp1386[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1387[ 2u]; _temp1387[
1u]= Cyc_Absynpp_stmt2doc( _temp1323->body); _temp1387[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1387, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1386[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1386, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1384[ 4u]; _temp1384[ 3u]= bodydoc; _temp1384[ 2u]= tqtddoc; _temp1384[ 1u]=
scopedoc; _temp1384[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1384, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1385[ 2u]; _temp1385[ 1u]= s; _temp1385[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1385, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1298;}} _LL1302: if(
_temp1325->fields ==  0){ s=({ struct Cyc_PP_Doc* _temp1394[ 5u]; _temp1394[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1394[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1325->tvs); _temp1394[ 2u]= _temp1325->name ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1325->name))->v); _temp1394[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1394[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1325->sc); Cyc_PP_cat( _tag_arr( _temp1394, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1395[ 10u]; _temp1395[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1395[ 8u]= Cyc_Absynpp_atts2doc(
_temp1325->attributes); _temp1395[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1395[ 6u]= Cyc_PP_line_doc(); _temp1395[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1396[ 2u]; _temp1396[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1325->fields))->v);
_temp1396[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1396, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1395[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1395[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1325->tvs);
_temp1395[ 2u]= _temp1325->name ==  0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1325->name))->v); _temp1395[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1395[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1325->sc); Cyc_PP_cat( _tag_arr( _temp1395, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1298; _LL1304: if( _temp1327->fields ==  0){ s=({ struct Cyc_PP_Doc*
_temp1397[ 5u]; _temp1397[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1397[ 3u]= Cyc_Absynpp_tvars2doc( _temp1327->tvs); _temp1397[ 2u]=
_temp1327->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1327->name))->v); _temp1397[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1397[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1327->sc);
Cyc_PP_cat( _tag_arr( _temp1397, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1398[ 10u]; _temp1398[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1398[ 8u]= Cyc_Absynpp_atts2doc( _temp1327->attributes);
_temp1398[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1398[ 6u]= Cyc_PP_line_doc(); _temp1398[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1399[ 2u]; _temp1399[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1327->fields))->v); _temp1399[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1399, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1398[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1398[ 3u]= Cyc_Absynpp_tvars2doc( _temp1327->tvs); _temp1398[ 2u]=
_temp1327->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1327->name))->v); _temp1398[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1398[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1327->sc);
Cyc_PP_cat( _tag_arr( _temp1398, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1298; _LL1306: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1340); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1332); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1400= Cyc_Tcutil_compress( _temp1336); struct Cyc_Absyn_FnInfo
_temp1406; struct Cyc_List_List* _temp1408; _LL1402: if(( unsigned int)
_temp1400 >  3u?*(( int*) _temp1400) ==  Cyc_Absyn_FnType: 0){ _LL1407:
_temp1406=(( struct Cyc_Absyn_FnType_struct*) _temp1400)->f1; _LL1409: _temp1408=
_temp1406.attributes; goto _LL1403;} else{ goto _LL1404;} _LL1404: goto _LL1405;
_LL1403: beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1408); goto _LL1401;
_LL1405: beforenamedoc= Cyc_PP_nil_doc(); goto _LL1401; _LL1401:;} s=({ struct
Cyc_PP_Doc* _temp1410[ 5u]; _temp1410[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); _temp1410[ 3u]= _temp1334 ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1413[ 2u]; _temp1413[ 1u]=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1334)); _temp1413[
0u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp1413, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1410[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1338, _temp1336,({ struct Cyc_Core_Opt* _temp1411=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1411->v=( void*)({ struct Cyc_PP_Doc*
_temp1412[ 2u]; _temp1412[ 1u]= sn; _temp1412[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1412, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1411;}));
_temp1410[ 1u]= Cyc_Absynpp_scope2doc( _temp1342); _temp1410[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1410, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1298;} _LL1308: if( _temp1349 ==  0){ s=({ struct Cyc_PP_Doc*
_temp1414[ 5u]; _temp1414[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1414[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1351); _temp1414[ 2u]=
_temp1347? Cyc_Absynpp_qvar2doc( _temp1353): Cyc_Absynpp_typedef_name2doc(
_temp1353); _temp1414[ 1u]= _temp1347? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1414[ 0u]= Cyc_Absynpp_scope2doc( _temp1355); Cyc_PP_cat( _tag_arr(
_temp1414, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1415[ 8u]; _temp1415[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1415[ 6u]= Cyc_PP_line_doc(); _temp1415[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1416[ 2u]; _temp1416[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*) _temp1349->v); _temp1416[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1416, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1415[ 4u]= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); _temp1415[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1351); _temp1415[ 2u]= _temp1347? Cyc_Absynpp_qvar2doc( _temp1353): Cyc_Absynpp_typedef_name2doc(
_temp1353); _temp1415[ 1u]= _temp1347? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1415[ 0u]= Cyc_Absynpp_scope2doc( _temp1355); Cyc_PP_cat( _tag_arr(
_temp1415, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1298; _LL1310: s=({
struct Cyc_PP_Doc* _temp1417[ 5u]; _temp1417[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1417[ 3u]= Cyc_Absynpp_exp2doc( _temp1357);
_temp1417[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1417[ 1u]= Cyc_Absynpp_pat2doc( _temp1359); _temp1417[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1417,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1298; _LL1312: s=({ struct Cyc_PP_Doc*
_temp1418[ 3u]; _temp1418[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1418[ 1u]= Cyc_Absynpp_ids2doc( _temp1361); _temp1418[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1418,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1298; _LL1314: if( _temp1366 ==  0){
s=({ struct Cyc_PP_Doc* _temp1419[ 4u]; _temp1419[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1419[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1368); _temp1419[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1419[ 0u]= Cyc_Absynpp_scope2doc( _temp1370); Cyc_PP_cat( _tag_arr(
_temp1419, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1420[ 7u]; _temp1420[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1420[ 5u]= Cyc_PP_line_doc(); _temp1420[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1421[ 2u]; _temp1421[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*) _temp1366->v); _temp1421[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1421, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1420[ 3u]= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); _temp1420[ 2u]= Cyc_Absynpp_qvar2doc(
_temp1368); _temp1420[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1420[ 0u]= Cyc_Absynpp_scope2doc( _temp1370); Cyc_PP_cat( _tag_arr(
_temp1420, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1298; _LL1316: s=({
struct Cyc_PP_Doc* _temp1422[ 3u]; _temp1422[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1422[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1423; _temp1423.q_const= 0; _temp1423.q_volatile= 0; _temp1423.q_restrict=
0; _temp1423;}),( void*) _temp1372->defn,({ struct Cyc_Core_Opt* _temp1424=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1424->v=(
void*)({ struct Cyc_PP_Doc* _temp1425[ 2u]; _temp1425[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1372->tvs); _temp1425[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1372->name);
Cyc_PP_cat( _tag_arr( _temp1425, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1424;}));
_temp1422[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1422, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1298; _LL1318: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1376);} s=({ struct Cyc_PP_Doc* _temp1426[ 7u]; _temp1426[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1426[ 5u]= Cyc_PP_line_doc();
_temp1426[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1374); _temp1426[ 3u]= Cyc_PP_line_doc();
_temp1426[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1426[ 1u]= Cyc_PP_textptr( _temp1376); _temp1426[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1426, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1298; _LL1320: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1427[ 7u]; _temp1427[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1427[ 5u]= Cyc_PP_line_doc(); _temp1427[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1378); _temp1427[ 3u]= Cyc_PP_line_doc();
_temp1427[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1427[ 1u]= Cyc_Absynpp_qvar2doc( _temp1380); _temp1427[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1427,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1428[
7u]; _temp1428[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1428[ 5u]= Cyc_PP_line_doc(); _temp1428[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1378); _temp1428[ 3u]= Cyc_PP_line_doc();
_temp1428[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1428[ 1u]= Cyc_Absynpp_qvar2doc( _temp1380); _temp1428[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1428, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1298; _LL1322: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1429[ 5u]; _temp1429[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1429[ 3u]= Cyc_PP_line_doc(); _temp1429[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1382); _temp1429[ 1u]= Cyc_PP_line_doc();
_temp1429[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1429, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1430[ 5u]; _temp1430[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1430[ 3u]= Cyc_PP_line_doc();
_temp1430[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1382); _temp1430[ 1u]= Cyc_PP_line_doc();
_temp1430[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1430, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1298; _LL1298:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1431= sc; _LL1433: if( _temp1431 == ( void*) Cyc_Absyn_Static){
goto _LL1434;} else{ goto _LL1435;} _LL1435: if( _temp1431 == ( void*) Cyc_Absyn_Public){
goto _LL1436;} else{ goto _LL1437;} _LL1437: if( _temp1431 == ( void*) Cyc_Absyn_Extern){
goto _LL1438;} else{ goto _LL1439;} _LL1439: if( _temp1431 == ( void*) Cyc_Absyn_ExternC){
goto _LL1440;} else{ goto _LL1441;} _LL1441: if( _temp1431 == ( void*) Cyc_Absyn_Abstract){
goto _LL1442;} else{ goto _LL1432;} _LL1434: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1436: return Cyc_PP_nil_doc(); _LL1438: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1440: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1442:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1432:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1443= t; struct
Cyc_Absyn_Tvar* _temp1451; struct Cyc_List_List* _temp1453; _LL1445: if((
unsigned int) _temp1443 >  3u?*(( int*) _temp1443) ==  Cyc_Absyn_VarType: 0){
_LL1452: _temp1451=(( struct Cyc_Absyn_VarType_struct*) _temp1443)->f1; goto
_LL1446;} else{ goto _LL1447;} _LL1447: if(( unsigned int) _temp1443 >  3u?*((
int*) _temp1443) ==  Cyc_Absyn_JoinEff: 0){ _LL1454: _temp1453=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1443)->f1; goto _LL1448;} else{ goto _LL1449;} _LL1449: goto _LL1450;
_LL1446: return Cyc_Tcutil_is_temp_tvar( _temp1451); _LL1448: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1453); _LL1450: return 0; _LL1444:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1455= t; struct Cyc_Absyn_Exp* _temp1469; struct Cyc_Absyn_Tqual _temp1471;
void* _temp1473; struct Cyc_Absyn_PtrInfo _temp1475; struct Cyc_Absyn_Conref*
_temp1477; struct Cyc_Absyn_Tqual _temp1479; struct Cyc_Absyn_Conref* _temp1481;
void* _temp1483; void* _temp1485; struct Cyc_Absyn_FnInfo _temp1487; struct Cyc_List_List*
_temp1489; struct Cyc_List_List* _temp1491; struct Cyc_Absyn_VarargInfo*
_temp1493; int _temp1495; struct Cyc_List_List* _temp1497; void* _temp1499;
struct Cyc_Core_Opt* _temp1501; struct Cyc_List_List* _temp1503; int _temp1505;
struct Cyc_Core_Opt* _temp1507; struct Cyc_Core_Opt* _temp1509; struct Cyc_Core_Opt*
_temp1511; struct Cyc_List_List* _temp1513; struct _tuple0* _temp1515; _LL1457:
if(( unsigned int) _temp1455 >  3u?*(( int*) _temp1455) ==  Cyc_Absyn_ArrayType:
0){ _LL1474: _temp1473=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1455)->f1;
goto _LL1472; _LL1472: _temp1471=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1455)->f2; goto _LL1470; _LL1470: _temp1469=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1455)->f3; goto _LL1458;} else{ goto _LL1459;} _LL1459: if(( unsigned int)
_temp1455 >  3u?*(( int*) _temp1455) ==  Cyc_Absyn_PointerType: 0){ _LL1476:
_temp1475=(( struct Cyc_Absyn_PointerType_struct*) _temp1455)->f1; _LL1486:
_temp1485=( void*) _temp1475.elt_typ; goto _LL1484; _LL1484: _temp1483=( void*)
_temp1475.rgn_typ; goto _LL1482; _LL1482: _temp1481= _temp1475.nullable; goto
_LL1480; _LL1480: _temp1479= _temp1475.tq; goto _LL1478; _LL1478: _temp1477=
_temp1475.bounds; goto _LL1460;} else{ goto _LL1461;} _LL1461: if(( unsigned int)
_temp1455 >  3u?*(( int*) _temp1455) ==  Cyc_Absyn_FnType: 0){ _LL1488:
_temp1487=(( struct Cyc_Absyn_FnType_struct*) _temp1455)->f1; _LL1504: _temp1503=
_temp1487.tvars; goto _LL1502; _LL1502: _temp1501= _temp1487.effect; goto
_LL1500; _LL1500: _temp1499=( void*) _temp1487.ret_typ; goto _LL1498; _LL1498:
_temp1497= _temp1487.args; goto _LL1496; _LL1496: _temp1495= _temp1487.c_varargs;
goto _LL1494; _LL1494: _temp1493= _temp1487.cyc_varargs; goto _LL1492; _LL1492:
_temp1491= _temp1487.rgn_po; goto _LL1490; _LL1490: _temp1489= _temp1487.attributes;
goto _LL1462;} else{ goto _LL1463;} _LL1463: if(( unsigned int) _temp1455 >  3u?*((
int*) _temp1455) ==  Cyc_Absyn_Evar: 0){ _LL1510: _temp1509=(( struct Cyc_Absyn_Evar_struct*)
_temp1455)->f1; goto _LL1508; _LL1508: _temp1507=(( struct Cyc_Absyn_Evar_struct*)
_temp1455)->f2; goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_Evar_struct*)
_temp1455)->f3; goto _LL1464;} else{ goto _LL1465;} _LL1465: if(( unsigned int)
_temp1455 >  3u?*(( int*) _temp1455) ==  Cyc_Absyn_TypedefType: 0){ _LL1516:
_temp1515=(( struct Cyc_Absyn_TypedefType_struct*) _temp1455)->f1; goto _LL1514;
_LL1514: _temp1513=(( struct Cyc_Absyn_TypedefType_struct*) _temp1455)->f2; goto
_LL1512; _LL1512: _temp1511=(( struct Cyc_Absyn_TypedefType_struct*) _temp1455)->f3;
goto _LL1466;} else{ goto _LL1467;} _LL1467: goto _LL1468; _LL1458: { struct Cyc_List_List*
_temp1519; void* _temp1521; struct Cyc_Absyn_Tqual _temp1523; struct _tuple4
_temp1517= Cyc_Absynpp_to_tms( _temp1471, _temp1473); _LL1524: _temp1523=
_temp1517.f1; goto _LL1522; _LL1522: _temp1521= _temp1517.f2; goto _LL1520;
_LL1520: _temp1519= _temp1517.f3; goto _LL1518; _LL1518: { void* tm; if(
_temp1469 ==  0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1525=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1525[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1526; _temp1526.tag= Cyc_Absyn_ConstArray_mod;
_temp1526.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1469); _temp1526;});
_temp1525;});} return({ struct _tuple4 _temp1527; _temp1527.f1= _temp1523;
_temp1527.f2= _temp1521; _temp1527.f3=({ struct Cyc_List_List* _temp1528=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1528->hd=(
void*) tm; _temp1528->tl= _temp1519; _temp1528;}); _temp1527;});}} _LL1460: {
struct Cyc_List_List* _temp1531; void* _temp1533; struct Cyc_Absyn_Tqual
_temp1535; struct _tuple4 _temp1529= Cyc_Absynpp_to_tms( _temp1479, _temp1485);
_LL1536: _temp1535= _temp1529.f1; goto _LL1534; _LL1534: _temp1533= _temp1529.f2;
goto _LL1532; _LL1532: _temp1531= _temp1529.f3; goto _LL1530; _LL1530: { void*
ps;{ struct _tuple8 _temp1538=({ struct _tuple8 _temp1537; _temp1537.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1481))->v; _temp1537.f2=( void*)( Cyc_Absyn_compress_conref( _temp1477))->v;
_temp1537;}); void* _temp1548; void* _temp1550; struct Cyc_Absyn_Exp* _temp1552;
void* _temp1554; int _temp1556; void* _temp1558; void* _temp1560; struct Cyc_Absyn_Exp*
_temp1562; void* _temp1564; int _temp1566; void* _temp1568; void* _temp1570;
_LL1540: _LL1555: _temp1554= _temp1538.f1; if(( unsigned int) _temp1554 >  1u?*((
int*) _temp1554) ==  Cyc_Absyn_Eq_constr: 0){ _LL1557: _temp1556=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1554)->f1; if( _temp1556 ==  1){ goto _LL1549;}
else{ goto _LL1542;}} else{ goto _LL1542;} _LL1549: _temp1548= _temp1538.f2; if((
unsigned int) _temp1548 >  1u?*(( int*) _temp1548) ==  Cyc_Absyn_Eq_constr: 0){
_LL1551: _temp1550=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1548)->f1;
if(( unsigned int) _temp1550 >  1u?*(( int*) _temp1550) ==  Cyc_Absyn_Upper_b: 0){
_LL1553: _temp1552=(( struct Cyc_Absyn_Upper_b_struct*) _temp1550)->f1; goto
_LL1541;} else{ goto _LL1542;}} else{ goto _LL1542;} _LL1542: _LL1565: _temp1564=
_temp1538.f1; if(( unsigned int) _temp1564 >  1u?*(( int*) _temp1564) ==  Cyc_Absyn_Eq_constr:
0){ _LL1567: _temp1566=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1564)->f1;
if( _temp1566 ==  0){ goto _LL1559;} else{ goto _LL1544;}} else{ goto _LL1544;}
_LL1559: _temp1558= _temp1538.f2; if(( unsigned int) _temp1558 >  1u?*(( int*)
_temp1558) ==  Cyc_Absyn_Eq_constr: 0){ _LL1561: _temp1560=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1558)->f1; if(( unsigned int) _temp1560 >  1u?*(( int*) _temp1560) ==  Cyc_Absyn_Upper_b:
0){ _LL1563: _temp1562=(( struct Cyc_Absyn_Upper_b_struct*) _temp1560)->f1; goto
_LL1543;} else{ goto _LL1544;}} else{ goto _LL1544;} _LL1544: _LL1569: _temp1568=
_temp1538.f2; if(( unsigned int) _temp1568 >  1u?*(( int*) _temp1568) ==  Cyc_Absyn_Eq_constr:
0){ _LL1571: _temp1570=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1568)->f1;
if( _temp1570 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1545;} else{ goto
_LL1546;}} else{ goto _LL1546;} _LL1546: goto _LL1547; _LL1541: ps=( void*)({
struct Cyc_Absyn_Nullable_ps_struct* _temp1572=( struct Cyc_Absyn_Nullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp1572[ 0]=({
struct Cyc_Absyn_Nullable_ps_struct _temp1573; _temp1573.tag= Cyc_Absyn_Nullable_ps;
_temp1573.f1= _temp1552; _temp1573;}); _temp1572;}); goto _LL1539; _LL1543: ps=(
void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp1574=( struct Cyc_Absyn_NonNullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp1574[ 0]=({
struct Cyc_Absyn_NonNullable_ps_struct _temp1575; _temp1575.tag= Cyc_Absyn_NonNullable_ps;
_temp1575.f1= _temp1562; _temp1575;}); _temp1574;}); goto _LL1539; _LL1545: ps=(
void*) Cyc_Absyn_TaggedArray_ps; goto _LL1539; _LL1547: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1539; _LL1539:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp1578=( struct Cyc_Absyn_Pointer_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1578[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1579; _temp1579.tag=
Cyc_Absyn_Pointer_mod; _temp1579.f1=( void*) ps; _temp1579.f2=( void*) _temp1483;
_temp1579.f3= tq; _temp1579;}); _temp1578;}); return({ struct _tuple4 _temp1576;
_temp1576.f1= _temp1535; _temp1576.f2= _temp1533; _temp1576.f3=({ struct Cyc_List_List*
_temp1577=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1577->hd=( void*) tm; _temp1577->tl= _temp1531; _temp1577;}); _temp1576;});}}}
_LL1462: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1501 ==  0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*) _temp1501->v)){ _temp1501= 0; _temp1503= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp1503);}}{ struct Cyc_List_List* _temp1582;
void* _temp1584; struct Cyc_Absyn_Tqual _temp1586; struct _tuple4 _temp1580= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp1499); _LL1587: _temp1586= _temp1580.f1; goto
_LL1585; _LL1585: _temp1584= _temp1580.f2; goto _LL1583; _LL1583: _temp1582=
_temp1580.f3; goto _LL1581; _LL1581: { struct Cyc_List_List* tms= _temp1582; if(
_temp1489 !=  0? ! Cyc_Absynpp_to_VC: 0){ tms=({ struct Cyc_List_List* _temp1588=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1588->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1589=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1589[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1590; _temp1590.tag= Cyc_Absyn_Attributes_mod;
_temp1590.f1= 0; _temp1590.f2= _temp1489; _temp1590;}); _temp1589;})); _temp1588->tl=
tms; _temp1588;});} tms=({ struct Cyc_List_List* _temp1591=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1591->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp1592=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp1592[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp1593; _temp1593.tag= Cyc_Absyn_Function_mod;
_temp1593.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp1594=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1594[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1595; _temp1595.tag= Cyc_Absyn_WithTypes;
_temp1595.f1= _temp1497; _temp1595.f2= _temp1495; _temp1595.f3= _temp1493;
_temp1595.f4= _temp1501; _temp1595.f5= _temp1491; _temp1595;}); _temp1594;}));
_temp1593;}); _temp1592;})); _temp1591->tl= tms; _temp1591;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1489 !=  0; _temp1489= _temp1489->tl){ void* _temp1596=( void*)
_temp1489->hd; _LL1598: if( _temp1596 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1599;} else{ goto _LL1600;} _LL1600: if( _temp1596 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1601;} else{ goto _LL1602;} _LL1602: if( _temp1596 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1603;} else{ goto _LL1604;} _LL1604: goto _LL1605; _LL1599: goto _LL1601;
_LL1601: goto _LL1603; _LL1603: tms=({ struct Cyc_List_List* _temp1606=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1606->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1607=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1607[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1608; _temp1608.tag= Cyc_Absyn_Attributes_mod;
_temp1608.f1= 0; _temp1608.f2=({ struct Cyc_List_List* _temp1609=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1609->hd=( void*)(( void*)
_temp1489->hd); _temp1609->tl= 0; _temp1609;}); _temp1608;}); _temp1607;}));
_temp1606->tl= tms; _temp1606;}); goto AfterAtts; _LL1605: goto _LL1597; _LL1597:;}}
AfterAtts: if( _temp1503 !=  0){ tms=({ struct Cyc_List_List* _temp1610=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1610->hd=( void*)((
void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp1611=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp1611[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp1612; _temp1612.tag= Cyc_Absyn_TypeParams_mod;
_temp1612.f1= _temp1503; _temp1612.f2= 0; _temp1612.f3= 1; _temp1612;});
_temp1611;})); _temp1610->tl= tms; _temp1610;});} return({ struct _tuple4
_temp1613; _temp1613.f1= _temp1586; _temp1613.f2= _temp1584; _temp1613.f3= tms;
_temp1613;});}} _LL1464: if( _temp1507 ==  0){ return({ struct _tuple4 _temp1614;
_temp1614.f1= tq; _temp1614.f2= t; _temp1614.f3= 0; _temp1614;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*) _temp1507->v);} _LL1466: if( _temp1511 ==  0? 1:
! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp1615; _temp1615.f1=
tq; _temp1615.f2= t; _temp1615.f3= 0; _temp1615;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*) _temp1511->v);} _LL1468: return({ struct _tuple4 _temp1616;
_temp1616.f1= tq; _temp1616.f2= t; _temp1616.f3= 0; _temp1616;}); _LL1456:;}
struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual tq, void* t,
struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp1619; void* _temp1621;
struct Cyc_Absyn_Tqual _temp1623; struct _tuple4 _temp1617= Cyc_Absynpp_to_tms(
tq, t); _LL1624: _temp1623= _temp1617.f1; goto _LL1622; _LL1622: _temp1621=
_temp1617.f2; goto _LL1620; _LL1620: _temp1619= _temp1617.f3; goto _LL1618;
_LL1618: _temp1619= Cyc_List_imp_rev( _temp1619); if( _temp1619 ==  0? dopt == 
0: 0){ return({ struct Cyc_PP_Doc* _temp1625[ 2u]; _temp1625[ 1u]= Cyc_Absynpp_ntyp2doc(
_temp1621); _temp1625[ 0u]= Cyc_Absynpp_tqual2doc( _temp1623); Cyc_PP_cat(
_tag_arr( _temp1625, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ return({
struct Cyc_PP_Doc* _temp1626[ 4u]; _temp1626[ 3u]= Cyc_Absynpp_dtms2doc( dopt == 
0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*) dopt->v, _temp1619); _temp1626[ 2u]=
Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp1626[ 1u]= Cyc_Absynpp_ntyp2doc(
_temp1621); _temp1626[ 0u]= Cyc_Absynpp_tqual2doc( _temp1623); Cyc_PP_cat(
_tag_arr( _temp1626, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield* f){ if( f->width != 
0){ return({ struct Cyc_PP_Doc* _temp1627[ 5u]; _temp1627[ 4u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1627[ 3u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1627[ 2u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( f->width)); _temp1627[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof(
unsigned char), 2u)); _temp1627[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1628=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1628->v=( void*) Cyc_PP_textptr( f->name); _temp1628;}));
Cyc_PP_cat( _tag_arr( _temp1627, sizeof( struct Cyc_PP_Doc*), 5u));});} else{
return({ struct Cyc_PP_Doc* _temp1629[ 3u]; _temp1629[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1629[ 1u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1629[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1630=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1630->v=( void*) Cyc_PP_textptr( f->name); _temp1630;}));
Cyc_PP_cat( _tag_arr( _temp1629, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,
_tag_arr("", sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1631[ 4u];
_temp1631[ 3u]= f->typs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1631[ 2u]= f->tvs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1631[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1631[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1631, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f){ for( 0; tdl !=  0; tdl=
tdl->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd),
72, f);({ void* _temp1632[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1632, sizeof( void*), 0u));});}} struct
_tagged_arr Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc(
Cyc_PP_seql( _tag_arr("", sizeof( unsigned char), 1u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_arr
Cyc_Absynpp_typ2string( void* t){ return Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72);} struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0* v){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_arr Cyc_Absynpp_typ2cstring(
void* t){ int old_qvar_to_Cids= Cyc_Absynpp_qvar_to_Cids; int old_add_cyc_prefix=
Cyc_Absynpp_add_cyc_prefix; Cyc_Absynpp_qvar_to_Cids= 1; Cyc_Absynpp_add_cyc_prefix=
0;{ struct _tagged_arr s= Cyc_Absynpp_typ2string( t); Cyc_Absynpp_qvar_to_Cids=
old_qvar_to_Cids; Cyc_Absynpp_add_cyc_prefix= old_add_cyc_prefix; return s;}}
struct _tagged_arr Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_arr Cyc_Absynpp_scope2string( void* sc){ void* _temp1633=
sc; _LL1635: if( _temp1633 == ( void*) Cyc_Absyn_Static){ goto _LL1636;} else{
goto _LL1637;} _LL1637: if( _temp1633 == ( void*) Cyc_Absyn_Public){ goto
_LL1638;} else{ goto _LL1639;} _LL1639: if( _temp1633 == ( void*) Cyc_Absyn_Extern){
goto _LL1640;} else{ goto _LL1641;} _LL1641: if( _temp1633 == ( void*) Cyc_Absyn_ExternC){
goto _LL1642;} else{ goto _LL1643;} _LL1643: if( _temp1633 == ( void*) Cyc_Absyn_Abstract){
goto _LL1644;} else{ goto _LL1634;} _LL1636: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1638: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1640: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1642:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1644: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1634:;}
