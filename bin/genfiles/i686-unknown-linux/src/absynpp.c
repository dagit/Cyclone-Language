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
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
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
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 19;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
20; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 21; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
print_full_evars: 1; int generate_line_directives: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern struct Cyc_Absynpp_Params
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
void* sc); extern int Cyc_Absynpp_is_anon_structtype( void* t); extern struct
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
extern struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs);
static int Cyc_Absynpp_expand_typedefs; static int Cyc_Absynpp_qvar_to_Cids;
static unsigned char _temp0[ 4u]="Cyc"; struct _tagged_arr Cyc_Absynpp_cyc_string={
_temp0, _temp0, _temp0 +  4u}; struct _tagged_arr* Cyc_Absynpp_cyc_stringptr=&
Cyc_Absynpp_cyc_string; static int Cyc_Absynpp_add_cyc_prefix; static int Cyc_Absynpp_to_VC;
static int Cyc_Absynpp_decls_first; static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars; static int Cyc_Absynpp_print_all_kinds;
static int Cyc_Absynpp_print_using_stmts; static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars; static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace; static struct Cyc_List_List* Cyc_Absynpp_curr_namespace=
0; struct Cyc_Absynpp_Params; void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs){ Cyc_Absynpp_expand_typedefs= fs->expand_typedefs; Cyc_Absynpp_qvar_to_Cids=
fs->qvar_to_Cids; Cyc_Absynpp_add_cyc_prefix= fs->add_cyc_prefix; Cyc_Absynpp_to_VC=
fs->to_VC; Cyc_Absynpp_decls_first= fs->decls_first; Cyc_Absynpp_rewrite_temp_tvars=
fs->rewrite_temp_tvars; Cyc_Absynpp_print_all_tvars= fs->print_all_tvars; Cyc_Absynpp_print_all_kinds=
fs->print_all_kinds; Cyc_Absynpp_print_using_stmts= fs->print_using_stmts; Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts; Cyc_Absynpp_print_full_evars= fs->print_full_evars; Cyc_Absynpp_generate_line_directives=
fs->generate_line_directives; Cyc_Absynpp_use_curr_namespace= fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace= fs->curr_namespace;} struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={
0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={
1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={
1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={
0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0}; static void Cyc_Absynpp_curr_namespace_add(
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
s;{ void* _temp252= t; struct Cyc_Core_Opt* _temp304; int _temp306; struct Cyc_Core_Opt*
_temp308; struct Cyc_Core_Opt* _temp310; struct Cyc_Absyn_Tvar* _temp312; struct
Cyc_Absyn_TunionInfo _temp314; void* _temp316; struct Cyc_List_List* _temp318;
void* _temp320; struct Cyc_Absyn_TunionFieldInfo _temp322; struct Cyc_List_List*
_temp324; void* _temp326; void* _temp328; void* _temp330; int _temp332; struct
Cyc_List_List* _temp334; struct Cyc_List_List* _temp336; struct _tuple0*
_temp338; struct Cyc_List_List* _temp340; struct _tuple0* _temp342; struct Cyc_List_List*
_temp344; struct Cyc_List_List* _temp346; struct Cyc_List_List* _temp348; struct
_tuple0* _temp350; void* _temp352; struct Cyc_Core_Opt* _temp354; struct Cyc_List_List*
_temp356; struct _tuple0* _temp358; void* _temp360; void* _temp362; _LL254: if((
unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_ArrayType: 0){
goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int) _temp252 >  3u?*((
int*) _temp252) ==  Cyc_Absyn_FnType: 0){ goto _LL257;} else{ goto _LL258;}
_LL258: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_PointerType:
0){ goto _LL259;} else{ goto _LL260;} _LL260: if( _temp252 == ( void*) Cyc_Absyn_VoidType){
goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp252 >  3u?*((
int*) _temp252) ==  Cyc_Absyn_Evar: 0){ _LL311: _temp310=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f1; goto _LL309; _LL309: _temp308=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f2; goto _LL307; _LL307: _temp306=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f3; goto _LL305; _LL305: _temp304=(( struct Cyc_Absyn_Evar_struct*)
_temp252)->f4; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_VarType: 0){ _LL313: _temp312=((
struct Cyc_Absyn_VarType_struct*) _temp252)->f1; goto _LL265;} else{ goto _LL266;}
_LL266: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_TunionType:
0){ _LL315: _temp314=(( struct Cyc_Absyn_TunionType_struct*) _temp252)->f1;
_LL321: _temp320=( void*) _temp314.tunion_info; goto _LL319; _LL319: _temp318=
_temp314.targs; goto _LL317; _LL317: _temp316=( void*) _temp314.rgn; goto _LL267;}
else{ goto _LL268;} _LL268: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_TunionFieldType: 0){ _LL323: _temp322=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp252)->f1; _LL327: _temp326=( void*) _temp322.field_info; goto _LL325;
_LL325: _temp324= _temp322.targs; goto _LL269;} else{ goto _LL270;} _LL270: if((
unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_IntType: 0){
_LL331: _temp330=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp252)->f1;
goto _LL329; _LL329: _temp328=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp252)->f2; goto _LL271;} else{ goto _LL272;} _LL272: if( _temp252 == ( void*)
Cyc_Absyn_FloatType){ goto _LL273;} else{ goto _LL274;} _LL274: if((
unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_DoubleType: 0){
_LL333: _temp332=(( struct Cyc_Absyn_DoubleType_struct*) _temp252)->f1; goto
_LL275;} else{ goto _LL276;} _LL276: if(( unsigned int) _temp252 >  3u?*(( int*)
_temp252) ==  Cyc_Absyn_TupleType: 0){ _LL335: _temp334=(( struct Cyc_Absyn_TupleType_struct*)
_temp252)->f1; goto _LL277;} else{ goto _LL278;} _LL278: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_StructType: 0){ _LL339:
_temp338=(( struct Cyc_Absyn_StructType_struct*) _temp252)->f1; goto _LL337;
_LL337: _temp336=(( struct Cyc_Absyn_StructType_struct*) _temp252)->f2; goto
_LL279;} else{ goto _LL280;} _LL280: if(( unsigned int) _temp252 >  3u?*(( int*)
_temp252) ==  Cyc_Absyn_UnionType: 0){ _LL343: _temp342=(( struct Cyc_Absyn_UnionType_struct*)
_temp252)->f1; goto _LL341; _LL341: _temp340=(( struct Cyc_Absyn_UnionType_struct*)
_temp252)->f2; goto _LL281;} else{ goto _LL282;} _LL282: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_AnonStructType: 0){ _LL345:
_temp344=(( struct Cyc_Absyn_AnonStructType_struct*) _temp252)->f1; goto _LL283;}
else{ goto _LL284;} _LL284: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_AnonUnionType: 0){ _LL347: _temp346=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp252)->f1; goto _LL285;} else{ goto _LL286;} _LL286: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_AnonEnumType: 0){ _LL349:
_temp348=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp252)->f1; goto _LL287;}
else{ goto _LL288;} _LL288: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252)
==  Cyc_Absyn_EnumType: 0){ _LL351: _temp350=(( struct Cyc_Absyn_EnumType_struct*)
_temp252)->f1; goto _LL289;} else{ goto _LL290;} _LL290: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_SizeofType: 0){ _LL353:
_temp352=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp252)->f1; goto
_LL291;} else{ goto _LL292;} _LL292: if(( unsigned int) _temp252 >  3u?*(( int*)
_temp252) ==  Cyc_Absyn_TypedefType: 0){ _LL359: _temp358=(( struct Cyc_Absyn_TypedefType_struct*)
_temp252)->f1; goto _LL357; _LL357: _temp356=(( struct Cyc_Absyn_TypedefType_struct*)
_temp252)->f2; goto _LL355; _LL355: _temp354=(( struct Cyc_Absyn_TypedefType_struct*)
_temp252)->f3; goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int)
_temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_RgnHandleType: 0){ _LL361:
_temp360=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp252)->f1; goto
_LL295;} else{ goto _LL296;} _LL296: if( _temp252 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int) _temp252 >  3u?*((
int*) _temp252) ==  Cyc_Absyn_RgnsEff: 0){ _LL363: _temp362=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp252)->f1; goto _LL299;} else{ goto _LL300;}
_LL300: if(( unsigned int) _temp252 >  3u?*(( int*) _temp252) ==  Cyc_Absyn_AccessEff:
0){ goto _LL301;} else{ goto _LL302;} _LL302: if(( unsigned int) _temp252 >  3u?*((
int*) _temp252) ==  Cyc_Absyn_JoinEff: 0){ goto _LL303;} else{ goto _LL253;}
_LL255: return Cyc_PP_text( _tag_arr("[[[array]]]", sizeof( unsigned char), 12u));
_LL257: return Cyc_PP_nil_doc(); _LL259: return Cyc_PP_nil_doc(); _LL261: s= Cyc_PP_text(
_tag_arr("void", sizeof( unsigned char), 5u)); goto _LL253; _LL263: if( _temp308
!=  0){ return Cyc_Absynpp_ntyp2doc(( void*) _temp308->v);} else{ s=({ struct
Cyc_PP_Doc* _temp364[ 6u]; _temp364[ 5u]= _temp310 ==  0? Cyc_PP_text( _tag_arr("?",
sizeof( unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*) _temp310->v);
_temp364[ 4u]= Cyc_PP_text( _tag_arr(")::", sizeof( unsigned char), 4u));
_temp364[ 3u]=( ! Cyc_Absynpp_print_full_evars? 1: _temp304 ==  0)? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc(( struct Cyc_List_List*)
_temp304->v); _temp364[ 2u]= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp366; _temp366.tag= Cyc_Std_Int_pa; _temp366.f1=( int)(( unsigned int)
_temp306);{ void* _temp365[ 1u]={& _temp366}; Cyc_Std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp365, sizeof( void*), 1u));}}));
_temp364[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp364[
0u]= Cyc_PP_text( _tag_arr("%", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp364, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL253; _LL265:
s= Cyc_PP_textptr( _temp312->name); if( Cyc_Absynpp_print_all_kinds){ s=({
struct Cyc_PP_Doc* _temp367[ 3u]; _temp367[ 2u]= Cyc_Absynpp_kindbound2doc((
void*) _temp312->kind); _temp367[ 1u]= Cyc_PP_text( _tag_arr("::", sizeof(
unsigned char), 3u)); _temp367[ 0u]= s; Cyc_PP_cat( _tag_arr( _temp367, sizeof(
struct Cyc_PP_Doc*), 3u));});} if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp312): 0){ s=({ struct Cyc_PP_Doc* _temp368[ 3u]; _temp368[ 2u]= Cyc_PP_text(
_tag_arr(" */", sizeof( unsigned char), 4u)); _temp368[ 1u]= s; _temp368[ 0u]=
Cyc_PP_text( _tag_arr("_ /* ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp368, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL253; _LL267:{
void* _temp369= _temp320; struct Cyc_Absyn_UnknownTunionInfo _temp375; int
_temp377; struct _tuple0* _temp379; struct Cyc_Absyn_Tuniondecl** _temp381;
struct Cyc_Absyn_Tuniondecl* _temp383; struct Cyc_Absyn_Tuniondecl _temp384; int
_temp385; struct _tuple0* _temp387; _LL371: if(*(( int*) _temp369) ==  Cyc_Absyn_UnknownTunion){
_LL376: _temp375=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp369)->f1;
_LL380: _temp379= _temp375.name; goto _LL378; _LL378: _temp377= _temp375.is_xtunion;
goto _LL372;} else{ goto _LL373;} _LL373: if(*(( int*) _temp369) ==  Cyc_Absyn_KnownTunion){
_LL382: _temp381=(( struct Cyc_Absyn_KnownTunion_struct*) _temp369)->f1;
_temp383=* _temp381; _temp384=* _temp383; _LL388: _temp387= _temp384.name; goto
_LL386; _LL386: _temp385= _temp384.is_xtunion; goto _LL374;} else{ goto _LL370;}
_LL372: _temp387= _temp379; _temp385= _temp377; goto _LL374; _LL374: { struct
Cyc_PP_Doc* _temp389= Cyc_PP_text( _temp385? _tag_arr("xtunion ", sizeof(
unsigned char), 9u): _tag_arr("tunion ", sizeof( unsigned char), 8u));{ void*
_temp390= Cyc_Tcutil_compress( _temp316); _LL392: if( _temp390 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL393;} else{ goto _LL394;} _LL394: goto _LL395; _LL393: s=({ struct Cyc_PP_Doc*
_temp396[ 3u]; _temp396[ 2u]= Cyc_Absynpp_tps2doc( _temp318); _temp396[ 1u]= Cyc_Absynpp_qvar2doc(
_temp387); _temp396[ 0u]= _temp389; Cyc_PP_cat( _tag_arr( _temp396, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL391; _LL395: s=({ struct Cyc_PP_Doc*
_temp397[ 5u]; _temp397[ 4u]= Cyc_Absynpp_tps2doc( _temp318); _temp397[ 3u]= Cyc_Absynpp_qvar2doc(
_temp387); _temp397[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp397[ 1u]= Cyc_Absynpp_typ2doc( _temp316); _temp397[ 0u]= _temp389; Cyc_PP_cat(
_tag_arr( _temp397, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL391; _LL391:;}
goto _LL370;} _LL370:;} goto _LL253; _LL269:{ void* _temp398= _temp326; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp404; int _temp406; struct _tuple0*
_temp408; struct _tuple0* _temp410; struct Cyc_Absyn_Tunionfield* _temp412;
struct Cyc_Absyn_Tunionfield _temp414; struct _tuple0* _temp415; struct Cyc_Absyn_Tuniondecl*
_temp417; struct Cyc_Absyn_Tuniondecl _temp419; int _temp420; struct _tuple0*
_temp422; _LL400: if(*(( int*) _temp398) ==  Cyc_Absyn_UnknownTunionfield){
_LL405: _temp404=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp398)->f1;
_LL411: _temp410= _temp404.tunion_name; goto _LL409; _LL409: _temp408= _temp404.field_name;
goto _LL407; _LL407: _temp406= _temp404.is_xtunion; goto _LL401;} else{ goto
_LL402;} _LL402: if(*(( int*) _temp398) ==  Cyc_Absyn_KnownTunionfield){ _LL418:
_temp417=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp398)->f1; _temp419=*
_temp417; _LL423: _temp422= _temp419.name; goto _LL421; _LL421: _temp420=
_temp419.is_xtunion; goto _LL413; _LL413: _temp412=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp398)->f2; _temp414=* _temp412; _LL416: _temp415= _temp414.name; goto _LL403;}
else{ goto _LL399;} _LL401: _temp422= _temp410; _temp420= _temp406; _temp415=
_temp408; goto _LL403; _LL403: { struct Cyc_PP_Doc* _temp424= Cyc_PP_text(
_temp420? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp425[ 4u]; _temp425[ 3u]=
Cyc_Absynpp_qvar2doc( _temp415); _temp425[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp425[ 1u]= Cyc_Absynpp_qvar2doc( _temp422);
_temp425[ 0u]= _temp424; Cyc_PP_cat( _tag_arr( _temp425, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL399;} _LL399:;} goto _LL253; _LL271: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp426= _temp330; _LL428: if( _temp426 == ( void*)
Cyc_Absyn_Signed){ goto _LL429;} else{ goto _LL430;} _LL430: if( _temp426 == (
void*) Cyc_Absyn_Unsigned){ goto _LL431;} else{ goto _LL427;} _LL429: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL427; _LL431: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL427; _LL427:;}{ void* _temp432= _temp328;
_LL434: if( _temp432 == ( void*) Cyc_Absyn_B1){ goto _LL435;} else{ goto _LL436;}
_LL436: if( _temp432 == ( void*) Cyc_Absyn_B2){ goto _LL437;} else{ goto _LL438;}
_LL438: if( _temp432 == ( void*) Cyc_Absyn_B4){ goto _LL439;} else{ goto _LL440;}
_LL440: if( _temp432 == ( void*) Cyc_Absyn_B8){ goto _LL441;} else{ goto _LL433;}
_LL435:{ void* _temp442= _temp330; _LL444: if( _temp442 == ( void*) Cyc_Absyn_Signed){
goto _LL445;} else{ goto _LL446;} _LL446: if( _temp442 == ( void*) Cyc_Absyn_Unsigned){
goto _LL447;} else{ goto _LL443;} _LL445: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL443; _LL447: goto _LL443; _LL443:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL433; _LL437: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL433; _LL439: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL433; _LL441: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL433; _LL433:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp450; _temp450.tag= Cyc_Std_String_pa; _temp450.f1=( struct _tagged_arr) ts;{
struct Cyc_Std_String_pa_struct _temp449; _temp449.tag= Cyc_Std_String_pa;
_temp449.f1=( struct _tagged_arr) sns;{ void* _temp448[ 2u]={& _temp449,&
_temp450}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp448, sizeof( void*), 2u));}}})); goto _LL253;} _LL273: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL253; _LL275: if(
_temp332){ s= Cyc_PP_text( _tag_arr("long double", sizeof( unsigned char), 12u));}
else{ s= Cyc_PP_text( _tag_arr("double", sizeof( unsigned char), 7u));} goto
_LL253; _LL277: s=({ struct Cyc_PP_Doc* _temp451[ 2u]; _temp451[ 1u]= Cyc_Absynpp_args2doc(
_temp334); _temp451[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp451, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL253; _LL279: if( _temp338 ==  0){ s=({ struct Cyc_PP_Doc* _temp452[ 2u];
_temp452[ 1u]= Cyc_Absynpp_tps2doc( _temp336); _temp452[ 0u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp452,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp453[ 3u];
_temp453[ 2u]= Cyc_Absynpp_tps2doc( _temp336); _temp453[ 1u]= _temp338 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp338));
_temp453[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp453, sizeof( struct Cyc_PP_Doc*), 3u));});} goto
_LL253; _LL281: if( _temp342 ==  0){ s=({ struct Cyc_PP_Doc* _temp454[ 2u];
_temp454[ 1u]= Cyc_Absynpp_tps2doc( _temp340); _temp454[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp454,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp455[ 3u];
_temp455[ 2u]= Cyc_Absynpp_tps2doc( _temp340); _temp455[ 1u]= _temp342 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp342));
_temp455[ 0u]= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp455, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL253; _LL283:
s=({ struct Cyc_PP_Doc* _temp456[ 3u]; _temp456[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp456[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp344)); _temp456[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp456, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL253; _LL285: s=({ struct Cyc_PP_Doc* _temp457[ 3u]; _temp457[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp457[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp346)); _temp457[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp457,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL253; _LL287: s=({ struct Cyc_PP_Doc*
_temp458[ 3u]; _temp458[ 2u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp458[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_enumfields2doc( _temp348));
_temp458[ 0u]= Cyc_PP_text( _tag_arr("enum {", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp458, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL253; _LL289: s=({
struct Cyc_PP_Doc* _temp459[ 2u]; _temp459[ 1u]= Cyc_Absynpp_qvar2doc( _temp350);
_temp459[ 0u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp459, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL253; _LL291: s=({
struct Cyc_PP_Doc* _temp460[ 3u]; _temp460[ 2u]= Cyc_PP_text( _tag_arr(">",
sizeof( unsigned char), 2u)); _temp460[ 1u]= Cyc_Absynpp_typ2doc( _temp352);
_temp460[ 0u]= Cyc_PP_text( _tag_arr("sizeof_t<", sizeof( unsigned char), 10u));
Cyc_PP_cat( _tag_arr( _temp460, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL253; _LL293: s=({ struct Cyc_PP_Doc* _temp461[ 2u]; _temp461[ 1u]= Cyc_Absynpp_tps2doc(
_temp356); _temp461[ 0u]= Cyc_Absynpp_qvar2doc( _temp358); Cyc_PP_cat( _tag_arr(
_temp461, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL253; _LL295: s=({ struct
Cyc_PP_Doc* _temp462[ 3u]; _temp462[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp462[ 1u]= Cyc_Absynpp_rgn2doc( _temp360); _temp462[ 0u]=
Cyc_PP_text( _tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat(
_tag_arr( _temp462, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL253; _LL297: s=
Cyc_Absynpp_rgn2doc( t); goto _LL253; _LL299: s=({ struct Cyc_PP_Doc* _temp463[
3u]; _temp463[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp463[ 1u]= Cyc_Absynpp_typ2doc( _temp362); _temp463[ 0u]= Cyc_PP_text(
_tag_arr("regions(", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp463, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL253; _LL301: goto _LL303;
_LL303: s= Cyc_Absynpp_eff2doc( t); goto _LL253; _LL253:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){ return vo ==  0? Cyc_PP_nil_doc():
Cyc_PP_text(*(( struct _tagged_arr*) vo->v));} struct _tuple8{ void* f1; void*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc( struct _tuple8* cmp){ struct
_tuple8 _temp466; void* _temp467; void* _temp469; struct _tuple8* _temp464= cmp;
_temp466=* _temp464; _LL470: _temp469= _temp466.f1; goto _LL468; _LL468:
_temp467= _temp466.f2; goto _LL465; _LL465: return({ struct Cyc_PP_Doc* _temp471[
3u]; _temp471[ 2u]= Cyc_Absynpp_rgn2doc( _temp467); _temp471[ 1u]= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); _temp471[ 0u]= Cyc_Absynpp_rgn2doc(
_temp469); Cyc_PP_cat( _tag_arr( _temp471, sizeof( struct Cyc_PP_Doc*), 3u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc( struct Cyc_List_List* po){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 ==  0? 0:({ struct Cyc_Core_Opt* _temp472=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp472->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp472;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp473=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp473=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp473,({
struct Cyc_List_List* _temp474=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp474->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp474->tl= 0; _temp474;}));} else{ if(
cyc_varargs !=  0){ struct Cyc_PP_Doc* _temp475=({ struct Cyc_PP_Doc* _temp477[
3u]; _temp477[ 2u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp478=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp478->f1= cyc_varargs->name;
_temp478->f2= cyc_varargs->tq; _temp478->f3=( void*) cyc_varargs->type; _temp478;}));
_temp477[ 1u]= cyc_varargs->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp477[ 0u]= Cyc_PP_text( _tag_arr("...", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp477, sizeof( struct Cyc_PP_Doc*), 3u));}); _temp473=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp473,({
struct Cyc_List_List* _temp476=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp476->hd=( void*) _temp475; _temp476->tl= 0;
_temp476;}));}}{ struct Cyc_PP_Doc* _temp479= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp473); if( effopt !=  0){ _temp479=({ struct
Cyc_PP_Doc* _temp480[ 3u]; _temp480[ 2u]= Cyc_Absynpp_eff2doc(( void*) effopt->v);
_temp480[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp480[
0u]= _temp479; Cyc_PP_cat( _tag_arr( _temp480, sizeof( struct Cyc_PP_Doc*), 3u));});}
if( rgn_po !=  0){ _temp479=({ struct Cyc_PP_Doc* _temp481[ 3u]; _temp481[ 2u]=
Cyc_Absynpp_rgnpo2doc( rgn_po); _temp481[ 1u]= Cyc_PP_text( _tag_arr(":",
sizeof( unsigned char), 2u)); _temp481[ 0u]= _temp479; Cyc_PP_cat( _tag_arr(
_temp481, sizeof( struct Cyc_PP_Doc*), 3u));});} return({ struct Cyc_PP_Doc*
_temp482[ 3u]; _temp482[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp482[ 1u]= _temp479; _temp482[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp482, sizeof( struct Cyc_PP_Doc*),
3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({
struct _tuple1* _temp483=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp483->f1=({ struct Cyc_Core_Opt* _temp484=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp484->v=( void*)(* arg).f1; _temp484;});
_temp483->f2=(* arg).f2; _temp483->f3=(* arg).f3; _temp483;});} struct Cyc_PP_Doc*
Cyc_Absynpp_var2doc( struct _tagged_arr* v){ return Cyc_PP_text(* v);} struct
Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp485= 0; int match;{ void* _temp486=(* q).f1; struct Cyc_List_List* _temp494;
struct Cyc_List_List* _temp496; _LL488: if( _temp486 == ( void*) Cyc_Absyn_Loc_n){
goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int) _temp486 >  1u?*((
int*) _temp486) ==  Cyc_Absyn_Rel_n: 0){ _LL495: _temp494=(( struct Cyc_Absyn_Rel_n_struct*)
_temp486)->f1; goto _LL491;} else{ goto _LL492;} _LL492: if(( unsigned int)
_temp486 >  1u?*(( int*) _temp486) ==  Cyc_Absyn_Abs_n: 0){ _LL497: _temp496=((
struct Cyc_Absyn_Abs_n_struct*) _temp486)->f1; goto _LL493;} else{ goto _LL487;}
_LL489: _temp494= 0; goto _LL491; _LL491: match= 0; _temp485= _temp494; goto
_LL487; _LL493: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_Std_strptrcmp, _temp496, Cyc_Absynpp_curr_namespace):
0; _temp485=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp498=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp498->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp498->tl= _temp496;
_temp498;}): _temp496; goto _LL487; _LL487:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp485,({
struct Cyc_List_List* _temp499=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp499->hd=( void*)(* q).f2; _temp499->tl= 0; _temp499;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp485,({ struct Cyc_List_List* _temp500=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp500->hd=( void*)(* q).f2; _temp500->tl= 0;
_temp500;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp501=(* v).f1; struct Cyc_List_List* _temp511; struct Cyc_List_List*
_temp513; _LL503: if( _temp501 == ( void*) Cyc_Absyn_Loc_n){ goto _LL504;} else{
goto _LL505;} _LL505: if(( unsigned int) _temp501 >  1u?*(( int*) _temp501) == 
Cyc_Absyn_Rel_n: 0){ _LL512: _temp511=(( struct Cyc_Absyn_Rel_n_struct*)
_temp501)->f1; if( _temp511 ==  0){ goto _LL506;} else{ goto _LL507;}} else{
goto _LL507;} _LL507: if(( unsigned int) _temp501 >  1u?*(( int*) _temp501) == 
Cyc_Absyn_Abs_n: 0){ _LL514: _temp513=(( struct Cyc_Absyn_Abs_n_struct*)
_temp501)->f1; goto _LL508;} else{ goto _LL509;} _LL509: goto _LL510; _LL504:
goto _LL506; _LL506: return Cyc_Absynpp_var2doc((* v).f2); _LL508: if((( int(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_strptrcmp, _temp513, Cyc_Absynpp_curr_namespace)
==  0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL510;} _LL510:
return({ struct Cyc_PP_Doc* _temp515[ 2u]; _temp515[ 1u]= Cyc_Absynpp_qvar2doc(
v); _temp515[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof(
unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp515, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL502:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp516=(
void*) e->r; void* _temp594; struct Cyc_Absyn_Exp* _temp596; struct Cyc_Absyn_Exp*
_temp598; _LL518: if(*(( int*) _temp516) ==  Cyc_Absyn_Const_e){ goto _LL519;}
else{ goto _LL520;} _LL520: if(*(( int*) _temp516) ==  Cyc_Absyn_Var_e){ goto
_LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp516) ==  Cyc_Absyn_UnknownId_e){
goto _LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp516) ==  Cyc_Absyn_Primop_e){
_LL595: _temp594=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp516)->f1;
goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp516) ==  Cyc_Absyn_AssignOp_e){
goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp516) ==  Cyc_Absyn_Increment_e){
goto _LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp516) ==  Cyc_Absyn_Conditional_e){
goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp516) ==  Cyc_Absyn_SeqExp_e){
goto _LL533;} else{ goto _LL534;} _LL534: if(*(( int*) _temp516) ==  Cyc_Absyn_UnknownCall_e){
goto _LL535;} else{ goto _LL536;} _LL536: if(*(( int*) _temp516) ==  Cyc_Absyn_FnCall_e){
goto _LL537;} else{ goto _LL538;} _LL538: if(*(( int*) _temp516) ==  Cyc_Absyn_Throw_e){
goto _LL539;} else{ goto _LL540;} _LL540: if(*(( int*) _temp516) ==  Cyc_Absyn_NoInstantiate_e){
_LL597: _temp596=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp516)->f1;
goto _LL541;} else{ goto _LL542;} _LL542: if(*(( int*) _temp516) ==  Cyc_Absyn_Instantiate_e){
_LL599: _temp598=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp516)->f1; goto
_LL543;} else{ goto _LL544;} _LL544: if(*(( int*) _temp516) ==  Cyc_Absyn_Cast_e){
goto _LL545;} else{ goto _LL546;} _LL546: if(*(( int*) _temp516) ==  Cyc_Absyn_New_e){
goto _LL547;} else{ goto _LL548;} _LL548: if(*(( int*) _temp516) ==  Cyc_Absyn_Address_e){
goto _LL549;} else{ goto _LL550;} _LL550: if(*(( int*) _temp516) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp516) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL553;} else{ goto _LL554;} _LL554: if(*(( int*) _temp516) ==  Cyc_Absyn_Offsetof_e){
goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp516) ==  Cyc_Absyn_Gentyp_e){
goto _LL557;} else{ goto _LL558;} _LL558: if(*(( int*) _temp516) ==  Cyc_Absyn_Deref_e){
goto _LL559;} else{ goto _LL560;} _LL560: if(*(( int*) _temp516) ==  Cyc_Absyn_StructMember_e){
goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp516) ==  Cyc_Absyn_StructArrow_e){
goto _LL563;} else{ goto _LL564;} _LL564: if(*(( int*) _temp516) ==  Cyc_Absyn_Subscript_e){
goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp516) ==  Cyc_Absyn_Tuple_e){
goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp516) ==  Cyc_Absyn_CompoundLit_e){
goto _LL569;} else{ goto _LL570;} _LL570: if(*(( int*) _temp516) ==  Cyc_Absyn_Array_e){
goto _LL571;} else{ goto _LL572;} _LL572: if(*(( int*) _temp516) ==  Cyc_Absyn_Comprehension_e){
goto _LL573;} else{ goto _LL574;} _LL574: if(*(( int*) _temp516) ==  Cyc_Absyn_Struct_e){
goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp516) ==  Cyc_Absyn_AnonStruct_e){
goto _LL577;} else{ goto _LL578;} _LL578: if(*(( int*) _temp516) ==  Cyc_Absyn_Tunion_e){
goto _LL579;} else{ goto _LL580;} _LL580: if(*(( int*) _temp516) ==  Cyc_Absyn_Enum_e){
goto _LL581;} else{ goto _LL582;} _LL582: if(*(( int*) _temp516) ==  Cyc_Absyn_AnonEnum_e){
goto _LL583;} else{ goto _LL584;} _LL584: if(*(( int*) _temp516) ==  Cyc_Absyn_Malloc_e){
goto _LL585;} else{ goto _LL586;} _LL586: if(*(( int*) _temp516) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL587;} else{ goto _LL588;} _LL588: if(*(( int*) _temp516) ==  Cyc_Absyn_StmtExp_e){
goto _LL589;} else{ goto _LL590;} _LL590: if(*(( int*) _temp516) ==  Cyc_Absyn_Codegen_e){
goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*) _temp516) ==  Cyc_Absyn_Fill_e){
goto _LL593;} else{ goto _LL517;} _LL519: return 10000; _LL521: return 10000;
_LL523: return 10000; _LL525: { void* _temp600= _temp594; _LL602: if( _temp600
== ( void*) Cyc_Absyn_Plus){ goto _LL603;} else{ goto _LL604;} _LL604: if(
_temp600 == ( void*) Cyc_Absyn_Times){ goto _LL605;} else{ goto _LL606;} _LL606:
if( _temp600 == ( void*) Cyc_Absyn_Minus){ goto _LL607;} else{ goto _LL608;}
_LL608: if( _temp600 == ( void*) Cyc_Absyn_Div){ goto _LL609;} else{ goto _LL610;}
_LL610: if( _temp600 == ( void*) Cyc_Absyn_Mod){ goto _LL611;} else{ goto _LL612;}
_LL612: if( _temp600 == ( void*) Cyc_Absyn_Eq){ goto _LL613;} else{ goto _LL614;}
_LL614: if( _temp600 == ( void*) Cyc_Absyn_Neq){ goto _LL615;} else{ goto _LL616;}
_LL616: if( _temp600 == ( void*) Cyc_Absyn_Gt){ goto _LL617;} else{ goto _LL618;}
_LL618: if( _temp600 == ( void*) Cyc_Absyn_Lt){ goto _LL619;} else{ goto _LL620;}
_LL620: if( _temp600 == ( void*) Cyc_Absyn_Gte){ goto _LL621;} else{ goto _LL622;}
_LL622: if( _temp600 == ( void*) Cyc_Absyn_Lte){ goto _LL623;} else{ goto _LL624;}
_LL624: if( _temp600 == ( void*) Cyc_Absyn_Not){ goto _LL625;} else{ goto _LL626;}
_LL626: if( _temp600 == ( void*) Cyc_Absyn_Bitnot){ goto _LL627;} else{ goto
_LL628;} _LL628: if( _temp600 == ( void*) Cyc_Absyn_Bitand){ goto _LL629;} else{
goto _LL630;} _LL630: if( _temp600 == ( void*) Cyc_Absyn_Bitor){ goto _LL631;}
else{ goto _LL632;} _LL632: if( _temp600 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL633;} else{ goto _LL634;} _LL634: if( _temp600 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL635;} else{ goto _LL636;} _LL636: if( _temp600 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL637;} else{ goto _LL638;} _LL638: if( _temp600 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL639;} else{ goto _LL640;} _LL640: if( _temp600 == ( void*) Cyc_Absyn_Size){
goto _LL641;} else{ goto _LL601;} _LL603: return 100; _LL605: return 110; _LL607:
return 100; _LL609: return 110; _LL611: return 110; _LL613: return 70; _LL615:
return 70; _LL617: return 80; _LL619: return 80; _LL621: return 80; _LL623:
return 80; _LL625: return 130; _LL627: return 130; _LL629: return 60; _LL631:
return 40; _LL633: return 50; _LL635: return 90; _LL637: return 80; _LL639:
return 80; _LL641: return 140; _LL601:;} _LL527: return 20; _LL529: return 130;
_LL531: return 30; _LL533: return 10; _LL535: return 140; _LL537: return 140;
_LL539: return 130; _LL541: return Cyc_Absynpp_exp_prec( _temp596); _LL543:
return Cyc_Absynpp_exp_prec( _temp598); _LL545: return 120; _LL547: return 15;
_LL549: return 130; _LL551: return 130; _LL553: return 130; _LL555: return 130;
_LL557: return 130; _LL559: return 130; _LL561: return 140; _LL563: return 140;
_LL565: return 140; _LL567: return 150; _LL569: goto _LL571; _LL571: goto _LL573;
_LL573: goto _LL575; _LL575: goto _LL577; _LL577: goto _LL579; _LL579: goto
_LL581; _LL581: goto _LL583; _LL583: goto _LL585; _LL585: return 140; _LL587:
return 140; _LL589: return 10000; _LL591: return 140; _LL593: return 140; _LL517:;}
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp642=(
void*) e->r; void* _temp722; struct _tuple0* _temp724; struct _tuple0* _temp726;
struct Cyc_List_List* _temp728; void* _temp730; struct Cyc_Absyn_Exp* _temp732;
struct Cyc_Core_Opt* _temp734; struct Cyc_Absyn_Exp* _temp736; void* _temp738;
struct Cyc_Absyn_Exp* _temp740; struct Cyc_Absyn_Exp* _temp742; struct Cyc_Absyn_Exp*
_temp744; struct Cyc_Absyn_Exp* _temp746; struct Cyc_Absyn_Exp* _temp748; struct
Cyc_Absyn_Exp* _temp750; struct Cyc_List_List* _temp752; struct Cyc_Absyn_Exp*
_temp754; struct Cyc_List_List* _temp756; struct Cyc_Absyn_Exp* _temp758; struct
Cyc_Absyn_Exp* _temp760; struct Cyc_Absyn_Exp* _temp762; struct Cyc_Absyn_Exp*
_temp764; struct Cyc_Absyn_Exp* _temp766; void* _temp768; struct Cyc_Absyn_Exp*
_temp770; struct Cyc_Absyn_Exp* _temp772; struct Cyc_Absyn_Exp* _temp774; void*
_temp776; struct Cyc_Absyn_Exp* _temp778; void* _temp780; struct _tagged_arr*
_temp782; void* _temp784; void* _temp786; unsigned int _temp788; void* _temp790;
void* _temp792; struct Cyc_List_List* _temp794; struct Cyc_Absyn_Exp* _temp796;
struct _tagged_arr* _temp798; struct Cyc_Absyn_Exp* _temp800; struct _tagged_arr*
_temp802; struct Cyc_Absyn_Exp* _temp804; struct Cyc_Absyn_Exp* _temp806; struct
Cyc_Absyn_Exp* _temp808; struct Cyc_List_List* _temp810; struct Cyc_List_List*
_temp812; struct _tuple1* _temp814; struct Cyc_List_List* _temp816; struct Cyc_Absyn_Exp*
_temp818; struct Cyc_Absyn_Exp* _temp820; struct Cyc_Absyn_Vardecl* _temp822;
struct Cyc_List_List* _temp824; struct _tuple0* _temp826; struct Cyc_List_List*
_temp828; struct Cyc_Absyn_Tunionfield* _temp830; struct Cyc_List_List* _temp832;
struct _tuple0* _temp834; struct _tuple0* _temp836; void* _temp838; struct Cyc_Absyn_Exp*
_temp840; struct Cyc_List_List* _temp842; struct Cyc_Core_Opt* _temp844; struct
Cyc_Absyn_Stmt* _temp846; struct Cyc_Absyn_Fndecl* _temp848; struct Cyc_Absyn_Exp*
_temp850; _LL644: if(*(( int*) _temp642) ==  Cyc_Absyn_Const_e){ _LL723:
_temp722=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp642)->f1; goto _LL645;}
else{ goto _LL646;} _LL646: if(*(( int*) _temp642) ==  Cyc_Absyn_Var_e){ _LL725:
_temp724=(( struct Cyc_Absyn_Var_e_struct*) _temp642)->f1; goto _LL647;} else{
goto _LL648;} _LL648: if(*(( int*) _temp642) ==  Cyc_Absyn_UnknownId_e){ _LL727:
_temp726=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp642)->f1; goto _LL649;}
else{ goto _LL650;} _LL650: if(*(( int*) _temp642) ==  Cyc_Absyn_Primop_e){
_LL731: _temp730=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp642)->f1;
goto _LL729; _LL729: _temp728=(( struct Cyc_Absyn_Primop_e_struct*) _temp642)->f2;
goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp642) ==  Cyc_Absyn_AssignOp_e){
_LL737: _temp736=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp642)->f1; goto
_LL735; _LL735: _temp734=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp642)->f2;
goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp642)->f3;
goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp642) ==  Cyc_Absyn_Increment_e){
_LL741: _temp740=(( struct Cyc_Absyn_Increment_e_struct*) _temp642)->f1; goto
_LL739; _LL739: _temp738=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp642)->f2; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp642)
==  Cyc_Absyn_Conditional_e){ _LL747: _temp746=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp642)->f1; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp642)->f2; goto _LL743; _LL743: _temp742=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp642)->f3; goto _LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp642)
==  Cyc_Absyn_SeqExp_e){ _LL751: _temp750=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp642)->f1; goto _LL749; _LL749: _temp748=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp642)->f2; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp642)
==  Cyc_Absyn_UnknownCall_e){ _LL755: _temp754=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp642)->f1; goto _LL753; _LL753: _temp752=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp642)->f2; goto _LL661;} else{ goto _LL662;} _LL662: if(*(( int*) _temp642)
==  Cyc_Absyn_FnCall_e){ _LL759: _temp758=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp642)->f1; goto _LL757; _LL757: _temp756=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp642)->f2; goto _LL663;} else{ goto _LL664;} _LL664: if(*(( int*) _temp642)
==  Cyc_Absyn_Throw_e){ _LL761: _temp760=(( struct Cyc_Absyn_Throw_e_struct*)
_temp642)->f1; goto _LL665;} else{ goto _LL666;} _LL666: if(*(( int*) _temp642)
==  Cyc_Absyn_NoInstantiate_e){ _LL763: _temp762=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp642)->f1; goto _LL667;} else{ goto _LL668;} _LL668: if(*(( int*) _temp642)
==  Cyc_Absyn_Instantiate_e){ _LL765: _temp764=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp642)->f1; goto _LL669;} else{ goto _LL670;} _LL670: if(*(( int*) _temp642)
==  Cyc_Absyn_Cast_e){ _LL769: _temp768=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp642)->f1; goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_Cast_e_struct*)
_temp642)->f2; goto _LL671;} else{ goto _LL672;} _LL672: if(*(( int*) _temp642)
==  Cyc_Absyn_Address_e){ _LL771: _temp770=(( struct Cyc_Absyn_Address_e_struct*)
_temp642)->f1; goto _LL673;} else{ goto _LL674;} _LL674: if(*(( int*) _temp642)
==  Cyc_Absyn_New_e){ _LL775: _temp774=(( struct Cyc_Absyn_New_e_struct*)
_temp642)->f1; goto _LL773; _LL773: _temp772=(( struct Cyc_Absyn_New_e_struct*)
_temp642)->f2; goto _LL675;} else{ goto _LL676;} _LL676: if(*(( int*) _temp642)
==  Cyc_Absyn_Sizeoftyp_e){ _LL777: _temp776=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp642)->f1; goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp642)
==  Cyc_Absyn_Sizeofexp_e){ _LL779: _temp778=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp642)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp642)
==  Cyc_Absyn_Offsetof_e){ _LL785: _temp784=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp642)->f1; goto _LL781; _LL781: _temp780=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp642)->f2; if(*(( int*) _temp780) ==  Cyc_Absyn_StructField){ _LL783:
_temp782=(( struct Cyc_Absyn_StructField_struct*) _temp780)->f1; goto _LL681;}
else{ goto _LL682;}} else{ goto _LL682;} _LL682: if(*(( int*) _temp642) ==  Cyc_Absyn_Offsetof_e){
_LL791: _temp790=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp642)->f1;
goto _LL787; _LL787: _temp786=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp642)->f2; if(*(( int*) _temp786) ==  Cyc_Absyn_TupleIndex){ _LL789:
_temp788=(( struct Cyc_Absyn_TupleIndex_struct*) _temp786)->f1; goto _LL683;}
else{ goto _LL684;}} else{ goto _LL684;} _LL684: if(*(( int*) _temp642) ==  Cyc_Absyn_Gentyp_e){
_LL795: _temp794=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp642)->f1; goto
_LL793; _LL793: _temp792=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp642)->f2;
goto _LL685;} else{ goto _LL686;} _LL686: if(*(( int*) _temp642) ==  Cyc_Absyn_Deref_e){
_LL797: _temp796=(( struct Cyc_Absyn_Deref_e_struct*) _temp642)->f1; goto _LL687;}
else{ goto _LL688;} _LL688: if(*(( int*) _temp642) ==  Cyc_Absyn_StructMember_e){
_LL801: _temp800=(( struct Cyc_Absyn_StructMember_e_struct*) _temp642)->f1; goto
_LL799; _LL799: _temp798=(( struct Cyc_Absyn_StructMember_e_struct*) _temp642)->f2;
goto _LL689;} else{ goto _LL690;} _LL690: if(*(( int*) _temp642) ==  Cyc_Absyn_StructArrow_e){
_LL805: _temp804=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp642)->f1; goto
_LL803; _LL803: _temp802=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp642)->f2;
goto _LL691;} else{ goto _LL692;} _LL692: if(*(( int*) _temp642) ==  Cyc_Absyn_Subscript_e){
_LL809: _temp808=(( struct Cyc_Absyn_Subscript_e_struct*) _temp642)->f1; goto
_LL807; _LL807: _temp806=(( struct Cyc_Absyn_Subscript_e_struct*) _temp642)->f2;
goto _LL693;} else{ goto _LL694;} _LL694: if(*(( int*) _temp642) ==  Cyc_Absyn_Tuple_e){
_LL811: _temp810=(( struct Cyc_Absyn_Tuple_e_struct*) _temp642)->f1; goto _LL695;}
else{ goto _LL696;} _LL696: if(*(( int*) _temp642) ==  Cyc_Absyn_CompoundLit_e){
_LL815: _temp814=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp642)->f1; goto
_LL813; _LL813: _temp812=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp642)->f2;
goto _LL697;} else{ goto _LL698;} _LL698: if(*(( int*) _temp642) ==  Cyc_Absyn_Array_e){
_LL817: _temp816=(( struct Cyc_Absyn_Array_e_struct*) _temp642)->f1; goto _LL699;}
else{ goto _LL700;} _LL700: if(*(( int*) _temp642) ==  Cyc_Absyn_Comprehension_e){
_LL823: _temp822=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp642)->f1;
goto _LL821; _LL821: _temp820=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp642)->f2; goto _LL819; _LL819: _temp818=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp642)->f3; goto _LL701;} else{ goto _LL702;} _LL702: if(*(( int*) _temp642)
==  Cyc_Absyn_Struct_e){ _LL827: _temp826=(( struct Cyc_Absyn_Struct_e_struct*)
_temp642)->f1; goto _LL825; _LL825: _temp824=(( struct Cyc_Absyn_Struct_e_struct*)
_temp642)->f3; goto _LL703;} else{ goto _LL704;} _LL704: if(*(( int*) _temp642)
==  Cyc_Absyn_AnonStruct_e){ _LL829: _temp828=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp642)->f2; goto _LL705;} else{ goto _LL706;} _LL706: if(*(( int*) _temp642)
==  Cyc_Absyn_Tunion_e){ _LL833: _temp832=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp642)->f3; goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp642)->f5; goto _LL707;} else{ goto _LL708;} _LL708: if(*(( int*) _temp642)
==  Cyc_Absyn_Enum_e){ _LL835: _temp834=(( struct Cyc_Absyn_Enum_e_struct*)
_temp642)->f1; goto _LL709;} else{ goto _LL710;} _LL710: if(*(( int*) _temp642)
==  Cyc_Absyn_AnonEnum_e){ _LL837: _temp836=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp642)->f1; goto _LL711;} else{ goto _LL712;} _LL712: if(*(( int*) _temp642)
==  Cyc_Absyn_Malloc_e){ _LL841: _temp840=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp642)->f1; goto _LL839; _LL839: _temp838=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp642)->f2; goto _LL713;} else{ goto _LL714;} _LL714: if(*(( int*) _temp642)
==  Cyc_Absyn_UnresolvedMem_e){ _LL845: _temp844=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp642)->f1; goto _LL843; _LL843: _temp842=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp642)->f2; goto _LL715;} else{ goto _LL716;} _LL716: if(*(( int*) _temp642)
==  Cyc_Absyn_StmtExp_e){ _LL847: _temp846=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp642)->f1; goto _LL717;} else{ goto _LL718;} _LL718: if(*(( int*) _temp642)
==  Cyc_Absyn_Codegen_e){ _LL849: _temp848=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp642)->f1; goto _LL719;} else{ goto _LL720;} _LL720: if(*(( int*) _temp642)
==  Cyc_Absyn_Fill_e){ _LL851: _temp850=(( struct Cyc_Absyn_Fill_e_struct*)
_temp642)->f1; goto _LL721;} else{ goto _LL643;} _LL645: s= Cyc_Absynpp_cnst2doc(
_temp722); goto _LL643; _LL647: _temp726= _temp724; goto _LL649; _LL649: s= Cyc_Absynpp_qvar2doc(
_temp726); goto _LL643; _LL651: s= Cyc_Absynpp_primapp2doc( myprec, _temp730,
_temp728); goto _LL643; _LL653: s=({ struct Cyc_PP_Doc* _temp852[ 5u]; _temp852[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp732); _temp852[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp852[ 2u]= _temp734 ==  0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)
_temp734->v); _temp852[ 1u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); _temp852[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp736); Cyc_PP_cat(
_tag_arr( _temp852, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL643; _LL655: {
struct Cyc_PP_Doc* _temp853= Cyc_Absynpp_exp2doc_prec( myprec, _temp740);{ void*
_temp854= _temp738; _LL856: if( _temp854 == ( void*) Cyc_Absyn_PreInc){ goto
_LL857;} else{ goto _LL858;} _LL858: if( _temp854 == ( void*) Cyc_Absyn_PreDec){
goto _LL859;} else{ goto _LL860;} _LL860: if( _temp854 == ( void*) Cyc_Absyn_PostInc){
goto _LL861;} else{ goto _LL862;} _LL862: if( _temp854 == ( void*) Cyc_Absyn_PostDec){
goto _LL863;} else{ goto _LL855;} _LL857: s=({ struct Cyc_PP_Doc* _temp864[ 2u];
_temp864[ 1u]= _temp853; _temp864[ 0u]= Cyc_PP_text( _tag_arr("++", sizeof(
unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp864, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL855; _LL859: s=({ struct Cyc_PP_Doc* _temp865[ 2u]; _temp865[ 1u]=
_temp853; _temp865[ 0u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp865, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL855; _LL861: s=({ struct Cyc_PP_Doc* _temp866[ 2u]; _temp866[ 1u]= Cyc_PP_text(
_tag_arr("++", sizeof( unsigned char), 3u)); _temp866[ 0u]= _temp853; Cyc_PP_cat(
_tag_arr( _temp866, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL855; _LL863: s=({
struct Cyc_PP_Doc* _temp867[ 2u]; _temp867[ 1u]= Cyc_PP_text( _tag_arr("--",
sizeof( unsigned char), 3u)); _temp867[ 0u]= _temp853; Cyc_PP_cat( _tag_arr(
_temp867, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL855; _LL855:;} goto
_LL643;} _LL657:{ struct _tuple8 _temp869=({ struct _tuple8 _temp868; _temp868.f1=(
void*) _temp744->r; _temp868.f2=( void*) _temp742->r; _temp868;}); _LL871: goto
_LL872; _LL872: s=({ struct Cyc_PP_Doc* _temp873[ 5u]; _temp873[ 4u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp742); _temp873[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); _temp873[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp744);
_temp873[ 1u]= Cyc_PP_text( _tag_arr(" ? ", sizeof( unsigned char), 4u));
_temp873[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp746); Cyc_PP_cat( _tag_arr(
_temp873, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL870; _LL870:;} goto
_LL643; _LL659: s=({ struct Cyc_PP_Doc* _temp874[ 5u]; _temp874[ 4u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp874[ 3u]= Cyc_Absynpp_exp2doc(
_temp748); _temp874[ 2u]= Cyc_PP_text( _tag_arr(", ", sizeof( unsigned char), 3u));
_temp874[ 1u]= Cyc_Absynpp_exp2doc( _temp750); _temp874[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp874,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL643; _LL661: s=({ struct Cyc_PP_Doc*
_temp875[ 4u]; _temp875[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp875[ 2u]= Cyc_Absynpp_exps2doc_prec( 20, _temp752); _temp875[ 1u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp875[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp754); Cyc_PP_cat( _tag_arr( _temp875, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL643; _LL663: s=({ struct Cyc_PP_Doc* _temp876[ 4u]; _temp876[ 3u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp876[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp756); _temp876[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp876[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp758); Cyc_PP_cat(
_tag_arr( _temp876, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL643; _LL665: s=({
struct Cyc_PP_Doc* _temp877[ 2u]; _temp877[ 1u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp760); _temp877[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof(
unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp877, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL643; _LL667: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp762);
goto _LL643; _LL669: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp764); goto _LL643;
_LL671: s=({ struct Cyc_PP_Doc* _temp878[ 4u]; _temp878[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp766); _temp878[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp878[ 1u]= Cyc_Absynpp_typ2doc( _temp768); _temp878[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp878, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL643; _LL673: s=({ struct
Cyc_PP_Doc* _temp879[ 2u]; _temp879[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp770); _temp879[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp879, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL643; _LL675: if( _temp774 ==  0){ s=({ struct Cyc_PP_Doc* _temp880[ 2u];
_temp880[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp772); _temp880[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp880,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp881[ 4u];
_temp881[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp772); _temp881[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp881[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp774)); _temp881[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp881,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL643; _LL677: s=({ struct Cyc_PP_Doc*
_temp882[ 3u]; _temp882[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp882[ 1u]= Cyc_Absynpp_typ2doc( _temp776); _temp882[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp882,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL643; _LL679: s=({ struct Cyc_PP_Doc*
_temp883[ 3u]; _temp883[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp883[ 1u]= Cyc_Absynpp_exp2doc( _temp778); _temp883[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp883,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL643; _LL681: s=({ struct Cyc_PP_Doc*
_temp884[ 5u]; _temp884[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp884[ 3u]= Cyc_PP_textptr( _temp782); _temp884[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp884[ 1u]= Cyc_Absynpp_typ2doc(
_temp784); _temp884[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp884, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL643; _LL683: s=({ struct Cyc_PP_Doc* _temp885[ 5u]; _temp885[ 4u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp885[ 3u]= Cyc_PP_text((
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp887; _temp887.tag= Cyc_Std_Int_pa;
_temp887.f1=( int) _temp788;{ void* _temp886[ 1u]={& _temp887}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp886, sizeof( void*),
1u));}})); _temp885[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp885[ 1u]= Cyc_Absynpp_typ2doc( _temp790); _temp885[ 0u]= Cyc_PP_text(
_tag_arr("offsetof(", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp885, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL643; _LL685: s=({ struct
Cyc_PP_Doc* _temp888[ 4u]; _temp888[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp888[ 2u]= Cyc_Absynpp_typ2doc( _temp792); _temp888[ 1u]=
Cyc_Absynpp_tvars2doc( _temp794); _temp888[ 0u]= Cyc_PP_text( _tag_arr("__gen(",
sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp888, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL643; _LL687: s=({ struct Cyc_PP_Doc* _temp889[ 2u]; _temp889[ 1u]=
Cyc_Absynpp_exp2doc_prec( myprec, _temp796); _temp889[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp889,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL643; _LL689: s=({ struct Cyc_PP_Doc*
_temp890[ 3u]; _temp890[ 2u]= Cyc_PP_textptr( _temp798); _temp890[ 1u]= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); _temp890[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp800); Cyc_PP_cat( _tag_arr( _temp890, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL643; _LL691: s=({ struct Cyc_PP_Doc* _temp891[ 3u]; _temp891[ 2u]=
Cyc_PP_textptr( _temp802); _temp891[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof(
unsigned char), 3u)); _temp891[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp804);
Cyc_PP_cat( _tag_arr( _temp891, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL643; _LL693: s=({ struct Cyc_PP_Doc* _temp892[ 4u]; _temp892[ 3u]= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); _temp892[ 2u]= Cyc_Absynpp_exp2doc(
_temp806); _temp892[ 1u]= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u));
_temp892[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp808); Cyc_PP_cat( _tag_arr(
_temp892, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL643; _LL695: s=({ struct
Cyc_PP_Doc* _temp893[ 3u]; _temp893[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp893[ 1u]= Cyc_Absynpp_exps2doc_prec( 20, _temp810);
_temp893[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp893, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL643; _LL697: s=({
struct Cyc_PP_Doc* _temp894[ 4u]; _temp894[ 3u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp812)); _temp894[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp894[ 1u]= Cyc_Absynpp_typ2doc((* _temp814).f3); _temp894[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp894,
sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL643; _LL699: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp816)); goto _LL643; _LL701: s=({ struct Cyc_PP_Doc*
_temp895[ 7u]; _temp895[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp895[ 5u]= Cyc_Absynpp_exp2doc( _temp818); _temp895[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp895[ 3u]= Cyc_Absynpp_exp2doc(
_temp820); _temp895[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp895[ 1u]= Cyc_PP_text(*(* _temp822->name).f2); _temp895[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp895,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL643; _LL703: s=({ struct Cyc_PP_Doc*
_temp896[ 2u]; _temp896[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp824));
_temp896[ 0u]= Cyc_Absynpp_qvar2doc( _temp826); Cyc_PP_cat( _tag_arr( _temp896,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL643; _LL705: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp828)); goto _LL643; _LL707: if( _temp832 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp830->name);} else{ s=({ struct Cyc_PP_Doc* _temp897[ 2u]; _temp897[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp832)); _temp897[ 0u]= Cyc_Absynpp_qvar2doc( _temp830->name);
Cyc_PP_cat( _tag_arr( _temp897, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL643; _LL709: s= Cyc_Absynpp_qvar2doc( _temp834); goto _LL643; _LL711: s= Cyc_Absynpp_qvar2doc(
_temp836); goto _LL643; _LL713: if( _temp840 ==  0){ s=({ struct Cyc_PP_Doc*
_temp898[ 3u]; _temp898[ 2u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp898[ 1u]= Cyc_Absynpp_typ2doc( _temp838); _temp898[ 0u]= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); Cyc_PP_cat( _tag_arr(
_temp898, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ s=({ struct Cyc_PP_Doc*
_temp899[ 6u]; _temp899[ 5u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp899[ 4u]= Cyc_Absynpp_typ2doc( _temp838); _temp899[ 3u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); _temp899[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp899[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp840)); _temp899[ 0u]= Cyc_PP_text(
_tag_arr("rmalloc(", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp899, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL643; _LL715: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp842)); goto _LL643; _LL717: s=({ struct Cyc_PP_Doc*
_temp900[ 3u]; _temp900[ 2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char),
4u)); _temp900[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp846)); _temp900[
0u]= Cyc_PP_text( _tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp900, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL643; _LL719: s=({
struct Cyc_PP_Doc* _temp901[ 3u]; _temp901[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp901[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp902=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp902->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp903=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp903[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp904; _temp904.tag= Cyc_Absyn_Fn_d;
_temp904.f1= _temp848; _temp904;}); _temp903;})); _temp902->loc= e->loc;
_temp902;}))); _temp901[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp901, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL643; _LL721: s=({ struct Cyc_PP_Doc* _temp905[ 3u]; _temp905[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp905[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp850)); _temp905[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp905, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL643; _LL643:;} if( inprec >=  myprec){ s=({ struct Cyc_PP_Doc*
_temp906[ 3u]; _temp906[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp906[ 1u]= s; _temp906[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp906, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp907= d; struct Cyc_Absyn_Exp* _temp913; struct _tagged_arr* _temp915;
_LL909: if(*(( int*) _temp907) ==  Cyc_Absyn_ArrayElement){ _LL914: _temp913=((
struct Cyc_Absyn_ArrayElement_struct*) _temp907)->f1; goto _LL910;} else{ goto
_LL911;} _LL911: if(*(( int*) _temp907) ==  Cyc_Absyn_FieldName){ _LL916:
_temp915=(( struct Cyc_Absyn_FieldName_struct*) _temp907)->f1; goto _LL912;}
else{ goto _LL908;} _LL910: return({ struct Cyc_PP_Doc* _temp917[ 3u]; _temp917[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp917[ 1u]= Cyc_Absynpp_exp2doc(
_temp913); _temp917[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp917, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL912:
return({ struct Cyc_PP_Doc* _temp918[ 2u]; _temp918[ 1u]= Cyc_PP_textptr(
_temp915); _temp918[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp918, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL908:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 ==  0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp919[ 2u]; _temp919[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp919[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp919, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp920= c;
unsigned char _temp938; void* _temp940; short _temp942; void* _temp944; int
_temp946; void* _temp948; int _temp950; void* _temp952; long long _temp954; void*
_temp956; struct _tagged_arr _temp958; struct _tagged_arr _temp960; _LL922: if((
unsigned int) _temp920 >  1u?*(( int*) _temp920) ==  Cyc_Absyn_Char_c: 0){
_LL941: _temp940=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp920)->f1; goto
_LL939; _LL939: _temp938=(( struct Cyc_Absyn_Char_c_struct*) _temp920)->f2; goto
_LL923;} else{ goto _LL924;} _LL924: if(( unsigned int) _temp920 >  1u?*(( int*)
_temp920) ==  Cyc_Absyn_Short_c: 0){ _LL945: _temp944=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp920)->f1; goto _LL943; _LL943: _temp942=(( struct Cyc_Absyn_Short_c_struct*)
_temp920)->f2; goto _LL925;} else{ goto _LL926;} _LL926: if(( unsigned int)
_temp920 >  1u?*(( int*) _temp920) ==  Cyc_Absyn_Int_c: 0){ _LL949: _temp948=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp920)->f1; if( _temp948 == ( void*)
Cyc_Absyn_Signed){ goto _LL947;} else{ goto _LL928;} _LL947: _temp946=(( struct
Cyc_Absyn_Int_c_struct*) _temp920)->f2; goto _LL927;} else{ goto _LL928;} _LL928:
if(( unsigned int) _temp920 >  1u?*(( int*) _temp920) ==  Cyc_Absyn_Int_c: 0){
_LL953: _temp952=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp920)->f1; if(
_temp952 == ( void*) Cyc_Absyn_Unsigned){ goto _LL951;} else{ goto _LL930;}
_LL951: _temp950=(( struct Cyc_Absyn_Int_c_struct*) _temp920)->f2; goto _LL929;}
else{ goto _LL930;} _LL930: if(( unsigned int) _temp920 >  1u?*(( int*) _temp920)
==  Cyc_Absyn_LongLong_c: 0){ _LL957: _temp956=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp920)->f1; goto _LL955; _LL955: _temp954=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp920)->f2; goto _LL931;} else{ goto _LL932;} _LL932: if(( unsigned int)
_temp920 >  1u?*(( int*) _temp920) ==  Cyc_Absyn_Float_c: 0){ _LL959: _temp958=((
struct Cyc_Absyn_Float_c_struct*) _temp920)->f1; goto _LL933;} else{ goto _LL934;}
_LL934: if( _temp920 == ( void*) Cyc_Absyn_Null_c){ goto _LL935;} else{ goto
_LL936;} _LL936: if(( unsigned int) _temp920 >  1u?*(( int*) _temp920) ==  Cyc_Absyn_String_c:
0){ _LL961: _temp960=(( struct Cyc_Absyn_String_c_struct*) _temp920)->f1; goto
_LL937;} else{ goto _LL921;} _LL923: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp963; _temp963.tag= Cyc_Std_String_pa;
_temp963.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp938);{ void*
_temp962[ 1u]={& _temp963}; Cyc_Std_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp962, sizeof( void*), 1u));}})); _LL925:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp965;
_temp965.tag= Cyc_Std_Int_pa; _temp965.f1=( int)(( unsigned int)(( int) _temp942));{
void* _temp964[ 1u]={& _temp965}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp964, sizeof( void*), 1u));}})); _LL927:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp967;
_temp967.tag= Cyc_Std_Int_pa; _temp967.f1=( int)(( unsigned int) _temp946);{
void* _temp966[ 1u]={& _temp967}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp966, sizeof( void*), 1u));}})); _LL929:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp969;
_temp969.tag= Cyc_Std_Int_pa; _temp969.f1=( unsigned int) _temp950;{ void*
_temp968[ 1u]={& _temp969}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp968, sizeof( void*), 1u));}})); _LL931:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL933: return Cyc_PP_text( _temp958); _LL935: return Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); _LL937: return({ struct Cyc_PP_Doc*
_temp970[ 3u]; _temp970[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp970[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp960));
_temp970[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp970, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL921:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == ( void*) Cyc_Absyn_Size){ if(
es ==  0? 1: es->tl !=  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp971=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp971[ 0]=({ struct Cyc_Core_Failure_struct _temp972; _temp972.tag= Cyc_Core_Failure;
_temp972.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp974;
_temp974.tag= Cyc_Std_String_pa; _temp974.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp973[ 1u]={& _temp974}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp973, sizeof( void*), 1u));}});
_temp972;}); _temp971;}));}{ struct Cyc_PP_Doc* _temp975= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*) es->hd); return({ struct Cyc_PP_Doc* _temp976[ 2u];
_temp976[ 1u]= Cyc_PP_text( _tag_arr(".size", sizeof( unsigned char), 6u));
_temp976[ 0u]= _temp975; Cyc_PP_cat( _tag_arr( _temp976, sizeof( struct Cyc_PP_Doc*),
2u));});}} else{ struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds ==  0){( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp977=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp977[ 0]=({ struct Cyc_Core_Failure_struct
_temp978; _temp978.tag= Cyc_Core_Failure; _temp978.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp980; _temp980.tag= Cyc_Std_String_pa;
_temp980.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp979[
1u]={& _temp980}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp979, sizeof( void*), 1u));}});
_temp978;}); _temp977;}));} else{ if( ds->tl ==  0){ return({ struct Cyc_PP_Doc*
_temp981[ 3u]; _temp981[ 2u]=( struct Cyc_PP_Doc*) ds->hd; _temp981[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp981[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp981, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null( ds->tl))->tl !=  0){( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp982=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp982[ 0]=({ struct Cyc_Core_Failure_struct
_temp983; _temp983.tag= Cyc_Core_Failure; _temp983.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp985; _temp985.tag= Cyc_Std_String_pa;
_temp985.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp984[
1u]={& _temp985}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp984, sizeof( void*), 1u));}});
_temp983;}); _temp982;}));} else{ return({ struct Cyc_PP_Doc* _temp986[ 5u];
_temp986[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds->tl))->hd;
_temp986[ 3u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp986[
2u]= ps; _temp986[ 1u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp986[ 0u]=( struct Cyc_PP_Doc*) ds->hd; Cyc_PP_cat( _tag_arr( _temp986,
sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct _tagged_arr Cyc_Absynpp_prim2str(
void* p){ void* _temp987= p; _LL989: if( _temp987 == ( void*) Cyc_Absyn_Plus){
goto _LL990;} else{ goto _LL991;} _LL991: if( _temp987 == ( void*) Cyc_Absyn_Times){
goto _LL992;} else{ goto _LL993;} _LL993: if( _temp987 == ( void*) Cyc_Absyn_Minus){
goto _LL994;} else{ goto _LL995;} _LL995: if( _temp987 == ( void*) Cyc_Absyn_Div){
goto _LL996;} else{ goto _LL997;} _LL997: if( _temp987 == ( void*) Cyc_Absyn_Mod){
goto _LL998;} else{ goto _LL999;} _LL999: if( _temp987 == ( void*) Cyc_Absyn_Eq){
goto _LL1000;} else{ goto _LL1001;} _LL1001: if( _temp987 == ( void*) Cyc_Absyn_Neq){
goto _LL1002;} else{ goto _LL1003;} _LL1003: if( _temp987 == ( void*) Cyc_Absyn_Gt){
goto _LL1004;} else{ goto _LL1005;} _LL1005: if( _temp987 == ( void*) Cyc_Absyn_Lt){
goto _LL1006;} else{ goto _LL1007;} _LL1007: if( _temp987 == ( void*) Cyc_Absyn_Gte){
goto _LL1008;} else{ goto _LL1009;} _LL1009: if( _temp987 == ( void*) Cyc_Absyn_Lte){
goto _LL1010;} else{ goto _LL1011;} _LL1011: if( _temp987 == ( void*) Cyc_Absyn_Not){
goto _LL1012;} else{ goto _LL1013;} _LL1013: if( _temp987 == ( void*) Cyc_Absyn_Bitnot){
goto _LL1014;} else{ goto _LL1015;} _LL1015: if( _temp987 == ( void*) Cyc_Absyn_Bitand){
goto _LL1016;} else{ goto _LL1017;} _LL1017: if( _temp987 == ( void*) Cyc_Absyn_Bitor){
goto _LL1018;} else{ goto _LL1019;} _LL1019: if( _temp987 == ( void*) Cyc_Absyn_Bitxor){
goto _LL1020;} else{ goto _LL1021;} _LL1021: if( _temp987 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL1022;} else{ goto _LL1023;} _LL1023: if( _temp987 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL1024;} else{ goto _LL1025;} _LL1025: if( _temp987 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL1026;} else{ goto _LL1027;} _LL1027: if( _temp987 == ( void*) Cyc_Absyn_Size){
goto _LL1028;} else{ goto _LL988;} _LL990: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL992: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL994: return _tag_arr("-", sizeof( unsigned char), 2u); _LL996: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL998: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL1000: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL1002: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL1004: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL1006: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL1008: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL1010: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL1012: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1014: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1016: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1018: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1020: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1022: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1024: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1026: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1028: return _tag_arr("size", sizeof( unsigned char), 5u); _LL988:;} struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 == 
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*
_temp1029[ 3u]; _temp1029[ 2u]= Cyc_Absynpp_exp2doc((* t).f2); _temp1029[ 1u]=
Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1029[ 0u]= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); Cyc_PP_cat(
_tag_arr( _temp1029, sizeof( struct Cyc_PP_Doc*), 3u));});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp1030=( void*) s->r; _LL1032: if((
unsigned int) _temp1030 >  1u?*(( int*) _temp1030) ==  Cyc_Absyn_Decl_s: 0){
goto _LL1033;} else{ goto _LL1034;} _LL1034: goto _LL1035; _LL1033: return 1;
_LL1035: return 0; _LL1031:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1036=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1080; struct Cyc_Absyn_Stmt* _temp1082; struct Cyc_Absyn_Stmt*
_temp1084; struct Cyc_Absyn_Exp* _temp1086; struct Cyc_Absyn_Stmt* _temp1088;
struct Cyc_Absyn_Stmt* _temp1090; struct Cyc_Absyn_Exp* _temp1092; struct Cyc_Absyn_Stmt*
_temp1094; struct _tuple2 _temp1096; struct Cyc_Absyn_Exp* _temp1098; struct
_tagged_arr* _temp1100; struct Cyc_Absyn_Stmt* _temp1102; struct _tuple2
_temp1104; struct Cyc_Absyn_Exp* _temp1106; struct _tuple2 _temp1108; struct Cyc_Absyn_Exp*
_temp1110; struct Cyc_Absyn_Exp* _temp1112; struct Cyc_List_List* _temp1114;
struct Cyc_Absyn_Exp* _temp1116; struct Cyc_List_List* _temp1118; struct Cyc_Absyn_Exp*
_temp1120; struct Cyc_List_List* _temp1122; struct Cyc_List_List* _temp1124;
struct Cyc_Absyn_Stmt* _temp1126; struct Cyc_Absyn_Decl* _temp1128; struct Cyc_Absyn_Stmt*
_temp1130; struct Cyc_Absyn_Stmt* _temp1132; struct Cyc_Absyn_Stmt* _temp1134;
struct _tagged_arr* _temp1136; struct _tuple2 _temp1138; struct Cyc_Absyn_Exp*
_temp1140; struct Cyc_Absyn_Stmt* _temp1142; struct Cyc_List_List* _temp1144;
struct Cyc_Absyn_Stmt* _temp1146; struct Cyc_Absyn_Stmt* _temp1148; struct Cyc_Absyn_Vardecl*
_temp1150; struct Cyc_Absyn_Tvar* _temp1152; _LL1038: if( _temp1036 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1039;} else{ goto _LL1040;} _LL1040: if((
unsigned int) _temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Exp_s: 0){
_LL1081: _temp1080=(( struct Cyc_Absyn_Exp_s_struct*) _temp1036)->f1; goto
_LL1041;} else{ goto _LL1042;} _LL1042: if(( unsigned int) _temp1036 >  1u?*((
int*) _temp1036) ==  Cyc_Absyn_Seq_s: 0){ _LL1085: _temp1084=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1036)->f1; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1036)->f2; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Return_s: 0){ _LL1087:
_temp1086=(( struct Cyc_Absyn_Return_s_struct*) _temp1036)->f1; goto _LL1045;}
else{ goto _LL1046;} _LL1046: if(( unsigned int) _temp1036 >  1u?*(( int*)
_temp1036) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1093: _temp1092=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1036)->f1; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1036)->f2; goto _LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1036)->f3; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_While_s: 0){ _LL1097:
_temp1096=(( struct Cyc_Absyn_While_s_struct*) _temp1036)->f1; _LL1099:
_temp1098= _temp1096.f1; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_While_s_struct*)
_temp1036)->f2; goto _LL1049;} else{ goto _LL1050;} _LL1050: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Break_s: 0){ goto _LL1051;}
else{ goto _LL1052;} _LL1052: if(( unsigned int) _temp1036 >  1u?*(( int*)
_temp1036) ==  Cyc_Absyn_Continue_s: 0){ goto _LL1053;} else{ goto _LL1054;}
_LL1054: if(( unsigned int) _temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Goto_s:
0){ _LL1101: _temp1100=(( struct Cyc_Absyn_Goto_s_struct*) _temp1036)->f1; goto
_LL1055;} else{ goto _LL1056;} _LL1056: if(( unsigned int) _temp1036 >  1u?*((
int*) _temp1036) ==  Cyc_Absyn_For_s: 0){ _LL1113: _temp1112=(( struct Cyc_Absyn_For_s_struct*)
_temp1036)->f1; goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_For_s_struct*)
_temp1036)->f2; _LL1111: _temp1110= _temp1108.f1; goto _LL1105; _LL1105:
_temp1104=(( struct Cyc_Absyn_For_s_struct*) _temp1036)->f3; _LL1107: _temp1106=
_temp1104.f1; goto _LL1103; _LL1103: _temp1102=(( struct Cyc_Absyn_For_s_struct*)
_temp1036)->f4; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Switch_s: 0){ _LL1117:
_temp1116=(( struct Cyc_Absyn_Switch_s_struct*) _temp1036)->f1; goto _LL1115;
_LL1115: _temp1114=(( struct Cyc_Absyn_Switch_s_struct*) _temp1036)->f2; goto
_LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int) _temp1036 >  1u?*((
int*) _temp1036) ==  Cyc_Absyn_SwitchC_s: 0){ _LL1121: _temp1120=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1036)->f1; goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1036)->f2; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1123:
_temp1122=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1036)->f1; if( _temp1122
==  0){ goto _LL1063;} else{ goto _LL1064;}} else{ goto _LL1064;} _LL1064: if((
unsigned int) _temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Fallthru_s: 0){
_LL1125: _temp1124=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1036)->f1; goto
_LL1065;} else{ goto _LL1066;} _LL1066: if(( unsigned int) _temp1036 >  1u?*((
int*) _temp1036) ==  Cyc_Absyn_Decl_s: 0){ _LL1129: _temp1128=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1036)->f1; goto _LL1127; _LL1127: _temp1126=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1036)->f2; goto _LL1067;} else{ goto _LL1068;} _LL1068: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Cut_s: 0){ _LL1131: _temp1130=((
struct Cyc_Absyn_Cut_s_struct*) _temp1036)->f1; goto _LL1069;} else{ goto
_LL1070;} _LL1070: if(( unsigned int) _temp1036 >  1u?*(( int*) _temp1036) == 
Cyc_Absyn_Splice_s: 0){ _LL1133: _temp1132=(( struct Cyc_Absyn_Splice_s_struct*)
_temp1036)->f1; goto _LL1071;} else{ goto _LL1072;} _LL1072: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Label_s: 0){ _LL1137:
_temp1136=(( struct Cyc_Absyn_Label_s_struct*) _temp1036)->f1; goto _LL1135;
_LL1135: _temp1134=(( struct Cyc_Absyn_Label_s_struct*) _temp1036)->f2; goto
_LL1073;} else{ goto _LL1074;} _LL1074: if(( unsigned int) _temp1036 >  1u?*((
int*) _temp1036) ==  Cyc_Absyn_Do_s: 0){ _LL1143: _temp1142=(( struct Cyc_Absyn_Do_s_struct*)
_temp1036)->f1; goto _LL1139; _LL1139: _temp1138=(( struct Cyc_Absyn_Do_s_struct*)
_temp1036)->f2; _LL1141: _temp1140= _temp1138.f1; goto _LL1075;} else{ goto
_LL1076;} _LL1076: if(( unsigned int) _temp1036 >  1u?*(( int*) _temp1036) == 
Cyc_Absyn_TryCatch_s: 0){ _LL1147: _temp1146=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1036)->f1; goto _LL1145; _LL1145: _temp1144=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1036)->f2; goto _LL1077;} else{ goto _LL1078;} _LL1078: if(( unsigned int)
_temp1036 >  1u?*(( int*) _temp1036) ==  Cyc_Absyn_Region_s: 0){ _LL1153:
_temp1152=(( struct Cyc_Absyn_Region_s_struct*) _temp1036)->f1; goto _LL1151;
_LL1151: _temp1150=(( struct Cyc_Absyn_Region_s_struct*) _temp1036)->f2; goto
_LL1149; _LL1149: _temp1148=(( struct Cyc_Absyn_Region_s_struct*) _temp1036)->f3;
goto _LL1079;} else{ goto _LL1037;} _LL1039: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1037; _LL1041: s=({ struct Cyc_PP_Doc*
_temp1154[ 2u]; _temp1154[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1154[ 0u]= Cyc_Absynpp_exp2doc( _temp1080); Cyc_PP_cat( _tag_arr(
_temp1154, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1037; _LL1043: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1084)){ s=({ struct Cyc_PP_Doc* _temp1155[
6u]; _temp1155[ 5u]= Cyc_Absynpp_is_declaration( _temp1082)?({ struct Cyc_PP_Doc*
_temp1156[ 4u]; _temp1156[ 3u]= Cyc_PP_line_doc(); _temp1156[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1156[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1082)); _temp1156[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1156, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1082); _temp1155[ 4u]= Cyc_PP_line_doc(); _temp1155[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1155[ 2u]= Cyc_PP_line_doc();
_temp1155[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1084)); _temp1155[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1155, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1082)){ s=({ struct Cyc_PP_Doc* _temp1157[ 6u]; _temp1157[ 5u]= Cyc_PP_line_doc();
_temp1157[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1157[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1082)); _temp1157[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1157[ 1u]= Cyc_PP_line_doc();
_temp1157[ 0u]= Cyc_Absynpp_stmt2doc( _temp1084); Cyc_PP_cat( _tag_arr(
_temp1157, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1158=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1158->hd=( void*) _temp1084;
_temp1158->tl=({ struct Cyc_List_List* _temp1159=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1159->hd=( void*) _temp1082;
_temp1159->tl= 0; _temp1159;}); _temp1158;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1160=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1160->hd=( void*) _temp1084;
_temp1160->tl=({ struct Cyc_List_List* _temp1161=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1161->hd=( void*) _temp1082;
_temp1161->tl= 0; _temp1161;}); _temp1160;}));} goto _LL1037; _LL1045: if(
_temp1086 ==  0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1162[ 3u]; _temp1162[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1162[ 1u]= _temp1086 ==  0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1086)); _temp1162[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1162, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1037;
_LL1047: { int print_else;{ void* _temp1163=( void*) _temp1088->r; _LL1165: if(
_temp1163 == ( void*) Cyc_Absyn_Skip_s){ goto _LL1166;} else{ goto _LL1167;}
_LL1167: goto _LL1168; _LL1166: print_else= 0; goto _LL1164; _LL1168: print_else=
1; goto _LL1164; _LL1164:;} s=({ struct Cyc_PP_Doc* _temp1169[ 7u]; _temp1169[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1171[ 5u]; _temp1171[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1171[ 3u]= Cyc_PP_line_doc();
_temp1171[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1172[ 2u]; _temp1172[
1u]= Cyc_Absynpp_stmt2doc( _temp1088); _temp1172[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1172, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1171[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1171[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1171, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1169[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1169[ 4u]= Cyc_PP_line_doc(); _temp1169[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1170[ 2u]; _temp1170[ 1u]= Cyc_Absynpp_stmt2doc( _temp1090); _temp1170[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1170, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1169[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1169[ 1u]= Cyc_Absynpp_exp2doc( _temp1092); _temp1169[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1169,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1037;} _LL1049: s=({ struct Cyc_PP_Doc*
_temp1173[ 6u]; _temp1173[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1173[ 4u]= Cyc_PP_line_doc(); _temp1173[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1174[ 2u]; _temp1174[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1094); _temp1174[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1174,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1173[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1173[ 1u]= Cyc_Absynpp_exp2doc( _temp1098);
_temp1173[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1173, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL1037; _LL1051: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL1037; _LL1053: s= Cyc_PP_text( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL1037; _LL1055: s= Cyc_PP_text(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp1176; _temp1176.tag= Cyc_Std_String_pa;
_temp1176.f1=( struct _tagged_arr)* _temp1100;{ void* _temp1175[ 1u]={&
_temp1176}; Cyc_Std_aprintf( _tag_arr("goto %s;", sizeof( unsigned char), 9u),
_tag_arr( _temp1175, sizeof( void*), 1u));}})); goto _LL1037; _LL1057: s=({
struct Cyc_PP_Doc* _temp1177[ 10u]; _temp1177[ 9u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1177[ 8u]= Cyc_PP_line_doc(); _temp1177[ 7u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1178[ 2u]; _temp1178[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1102); _temp1178[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1178,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1177[ 6u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1177[ 5u]= Cyc_Absynpp_exp2doc( _temp1106);
_temp1177[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char), 3u));
_temp1177[ 3u]= Cyc_Absynpp_exp2doc( _temp1110); _temp1177[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1177[ 1u]= Cyc_Absynpp_exp2doc(
_temp1112); _temp1177[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1177, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL1037; _LL1059: s=({ struct Cyc_PP_Doc* _temp1179[ 7u]; _temp1179[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1179[ 5u]= Cyc_PP_line_doc();
_temp1179[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1114); _temp1179[ 3u]= Cyc_PP_line_doc();
_temp1179[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1179[ 1u]= Cyc_Absynpp_exp2doc( _temp1116); _temp1179[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1179, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1037; _LL1061: s=({
struct Cyc_PP_Doc* _temp1180[ 7u]; _temp1180[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1180[ 5u]= Cyc_PP_line_doc(); _temp1180[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1118); _temp1180[ 3u]= Cyc_PP_line_doc();
_temp1180[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1180[ 1u]= Cyc_Absynpp_exp2doc( _temp1120); _temp1180[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1180, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1037; _LL1063: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL1037; _LL1065: s=({
struct Cyc_PP_Doc* _temp1181[ 3u]; _temp1181[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1181[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1124); _temp1181[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1181, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL1037; _LL1067: s=({ struct Cyc_PP_Doc* _temp1182[ 3u];
_temp1182[ 2u]= Cyc_Absynpp_stmt2doc( _temp1126); _temp1182[ 1u]= Cyc_PP_line_doc();
_temp1182[ 0u]= Cyc_Absynpp_decl2doc( _temp1128); Cyc_PP_cat( _tag_arr(
_temp1182, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1037; _LL1069: s=({
struct Cyc_PP_Doc* _temp1183[ 2u]; _temp1183[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1130)); _temp1183[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1183, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL1037; _LL1071: s=({ struct Cyc_PP_Doc* _temp1184[ 2u]; _temp1184[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1132)); _temp1184[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1184, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1037; _LL1073: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1134): 0){ s=({ struct Cyc_PP_Doc* _temp1185[ 6u];
_temp1185[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1185[ 4u]= Cyc_PP_line_doc(); _temp1185[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1134)); _temp1185[ 2u]= Cyc_PP_line_doc(); _temp1185[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1185[ 0u]= Cyc_PP_textptr(
_temp1136); Cyc_PP_cat( _tag_arr( _temp1185, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1186[ 3u]; _temp1186[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1134); _temp1186[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1186[ 0u]= Cyc_PP_textptr( _temp1136); Cyc_PP_cat( _tag_arr(
_temp1186, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1037; _LL1075: s=({
struct Cyc_PP_Doc* _temp1187[ 7u]; _temp1187[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1187[ 5u]= Cyc_Absynpp_exp2doc( _temp1140);
_temp1187[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1187[ 3u]= Cyc_PP_line_doc(); _temp1187[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1142)); _temp1187[ 1u]= Cyc_PP_line_doc(); _temp1187[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1187,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1037; _LL1077: s=({ struct Cyc_PP_Doc*
_temp1188[ 9u]; _temp1188[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1188[ 7u]= Cyc_PP_line_doc(); _temp1188[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1144)); _temp1188[ 5u]= Cyc_PP_line_doc(); _temp1188[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1188[ 3u]= Cyc_PP_line_doc();
_temp1188[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1146)); _temp1188[ 1u]=
Cyc_PP_line_doc(); _temp1188[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1188, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1037; _LL1079: s=({ struct Cyc_PP_Doc* _temp1189[ 9u];
_temp1189[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1189[ 7u]= Cyc_PP_line_doc(); _temp1189[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1148)); _temp1189[ 5u]= Cyc_PP_line_doc(); _temp1189[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1189[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1150->name); _temp1189[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1189[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1152)); _temp1189[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1189, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1037; _LL1037:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1190=( void*) p->r;
int _temp1224; void* _temp1226; unsigned char _temp1228; struct _tagged_arr
_temp1230; struct Cyc_Absyn_Vardecl* _temp1232; struct Cyc_List_List* _temp1234;
struct Cyc_Absyn_Pat* _temp1236; struct Cyc_Absyn_Vardecl* _temp1238; struct
_tuple0* _temp1240; struct Cyc_List_List* _temp1242; struct Cyc_List_List*
_temp1244; struct _tuple0* _temp1246; struct Cyc_List_List* _temp1248; struct
Cyc_List_List* _temp1250; struct _tuple0* _temp1252; struct Cyc_List_List*
_temp1254; struct Cyc_List_List* _temp1256; struct Cyc_Absyn_Structdecl*
_temp1258; struct Cyc_Absyn_Enumfield* _temp1260; struct Cyc_Absyn_Enumfield*
_temp1262; struct Cyc_List_List* _temp1264; struct Cyc_List_List* _temp1266;
struct Cyc_Absyn_Tunionfield* _temp1268; _LL1192: if( _temp1190 == ( void*) Cyc_Absyn_Wild_p){
goto _LL1193;} else{ goto _LL1194;} _LL1194: if( _temp1190 == ( void*) Cyc_Absyn_Null_p){
goto _LL1195;} else{ goto _LL1196;} _LL1196: if(( unsigned int) _temp1190 >  2u?*((
int*) _temp1190) ==  Cyc_Absyn_Int_p: 0){ _LL1227: _temp1226=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1190)->f1; goto _LL1225; _LL1225: _temp1224=((
struct Cyc_Absyn_Int_p_struct*) _temp1190)->f2; goto _LL1197;} else{ goto
_LL1198;} _LL1198: if(( unsigned int) _temp1190 >  2u?*(( int*) _temp1190) == 
Cyc_Absyn_Char_p: 0){ _LL1229: _temp1228=(( struct Cyc_Absyn_Char_p_struct*)
_temp1190)->f1; goto _LL1199;} else{ goto _LL1200;} _LL1200: if(( unsigned int)
_temp1190 >  2u?*(( int*) _temp1190) ==  Cyc_Absyn_Float_p: 0){ _LL1231:
_temp1230=(( struct Cyc_Absyn_Float_p_struct*) _temp1190)->f1; goto _LL1201;}
else{ goto _LL1202;} _LL1202: if(( unsigned int) _temp1190 >  2u?*(( int*)
_temp1190) ==  Cyc_Absyn_Var_p: 0){ _LL1233: _temp1232=(( struct Cyc_Absyn_Var_p_struct*)
_temp1190)->f1; goto _LL1203;} else{ goto _LL1204;} _LL1204: if(( unsigned int)
_temp1190 >  2u?*(( int*) _temp1190) ==  Cyc_Absyn_Tuple_p: 0){ _LL1235:
_temp1234=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1190)->f1; goto _LL1205;}
else{ goto _LL1206;} _LL1206: if(( unsigned int) _temp1190 >  2u?*(( int*)
_temp1190) ==  Cyc_Absyn_Pointer_p: 0){ _LL1237: _temp1236=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1190)->f1; goto _LL1207;} else{ goto _LL1208;} _LL1208: if(( unsigned int)
_temp1190 >  2u?*(( int*) _temp1190) ==  Cyc_Absyn_Reference_p: 0){ _LL1239:
_temp1238=(( struct Cyc_Absyn_Reference_p_struct*) _temp1190)->f1; goto _LL1209;}
else{ goto _LL1210;} _LL1210: if(( unsigned int) _temp1190 >  2u?*(( int*)
_temp1190) ==  Cyc_Absyn_UnknownId_p: 0){ _LL1241: _temp1240=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1190)->f1; goto _LL1211;} else{ goto _LL1212;} _LL1212: if(( unsigned int)
_temp1190 >  2u?*(( int*) _temp1190) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL1247:
_temp1246=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1190)->f1; goto
_LL1245; _LL1245: _temp1244=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1190)->f2;
goto _LL1243; _LL1243: _temp1242=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1190)->f3; goto _LL1213;} else{ goto _LL1214;} _LL1214: if(( unsigned int)
_temp1190 >  2u?*(( int*) _temp1190) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL1253:
_temp1252=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1190)->f1; goto
_LL1251; _LL1251: _temp1250=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1190)->f2; goto _LL1249; _LL1249: _temp1248=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1190)->f3; goto _LL1215;} else{ goto _LL1216;} _LL1216: if(( unsigned int)
_temp1190 >  2u?*(( int*) _temp1190) ==  Cyc_Absyn_Struct_p: 0){ _LL1259:
_temp1258=(( struct Cyc_Absyn_Struct_p_struct*) _temp1190)->f1; goto _LL1257;
_LL1257: _temp1256=(( struct Cyc_Absyn_Struct_p_struct*) _temp1190)->f3; goto
_LL1255; _LL1255: _temp1254=(( struct Cyc_Absyn_Struct_p_struct*) _temp1190)->f4;
goto _LL1217;} else{ goto _LL1218;} _LL1218: if(( unsigned int) _temp1190 >  2u?*((
int*) _temp1190) ==  Cyc_Absyn_Enum_p: 0){ _LL1261: _temp1260=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1190)->f2; goto _LL1219;} else{ goto _LL1220;} _LL1220: if(( unsigned int)
_temp1190 >  2u?*(( int*) _temp1190) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1263:
_temp1262=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1190)->f2; goto _LL1221;}
else{ goto _LL1222;} _LL1222: if(( unsigned int) _temp1190 >  2u?*(( int*)
_temp1190) ==  Cyc_Absyn_Tunion_p: 0){ _LL1269: _temp1268=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1190)->f2; goto _LL1267; _LL1267: _temp1266=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1190)->f3; goto _LL1265; _LL1265: _temp1264=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1190)->f4; goto _LL1223;} else{ goto _LL1191;} _LL1193: s= Cyc_PP_text(
_tag_arr("_", sizeof( unsigned char), 2u)); goto _LL1191; _LL1195: s= Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); goto _LL1191; _LL1197: if(
_temp1226 == ( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp1271; _temp1271.tag= Cyc_Std_Int_pa; _temp1271.f1=(
int)(( unsigned int) _temp1224);{ void* _temp1270[ 1u]={& _temp1271}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1270, sizeof( void*),
1u));}}));} else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1273; _temp1273.tag= Cyc_Std_Int_pa; _temp1273.f1=( unsigned int) _temp1224;{
void* _temp1272[ 1u]={& _temp1273}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp1272, sizeof( void*), 1u));}}));} goto
_LL1191; _LL1199: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1275; _temp1275.tag= Cyc_Std_String_pa; _temp1275.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1228);{ void* _temp1274[ 1u]={& _temp1275}; Cyc_Std_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1274, sizeof( void*),
1u));}})); goto _LL1191; _LL1201: s= Cyc_PP_text( _temp1230); goto _LL1191;
_LL1203: s= Cyc_Absynpp_qvar2doc( _temp1232->name); goto _LL1191; _LL1205: s=({
struct Cyc_PP_Doc* _temp1276[ 3u]; _temp1276[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1276[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1234); _temp1276[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1276, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1191; _LL1207: s=({ struct Cyc_PP_Doc* _temp1277[ 2u]; _temp1277[ 1u]=
Cyc_Absynpp_pat2doc( _temp1236); _temp1277[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1277, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1191; _LL1209: s=({ struct Cyc_PP_Doc* _temp1278[
2u]; _temp1278[ 1u]= Cyc_Absynpp_qvar2doc( _temp1238->name); _temp1278[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1278,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1191; _LL1211: s= Cyc_Absynpp_qvar2doc(
_temp1240); goto _LL1191; _LL1213: s=({ struct Cyc_PP_Doc* _temp1279[ 3u];
_temp1279[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1242));
_temp1279[ 1u]= Cyc_Absynpp_tvars2doc( _temp1244); _temp1279[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1246); Cyc_PP_cat( _tag_arr( _temp1279, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1191; _LL1215: s=({ struct Cyc_PP_Doc* _temp1280[ 3u]; _temp1280[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1248)); _temp1280[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1250); _temp1280[ 0u]= Cyc_Absynpp_qvar2doc( _temp1252); Cyc_PP_cat(
_tag_arr( _temp1280, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1191; _LL1217:
s=({ struct Cyc_PP_Doc* _temp1281[ 3u]; _temp1281[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1254)); _temp1281[ 1u]= Cyc_Absynpp_tvars2doc( _temp1256); _temp1281[ 0u]=
_temp1258->name ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1258->name))->v); Cyc_PP_cat( _tag_arr(
_temp1281, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1191; _LL1219: s= Cyc_Absynpp_qvar2doc(
_temp1260->name); goto _LL1191; _LL1221: s= Cyc_Absynpp_qvar2doc( _temp1262->name);
goto _LL1191; _LL1223: if( _temp1264 ==  0){ s= Cyc_Absynpp_qvar2doc( _temp1268->name);}
else{ s=({ struct Cyc_PP_Doc* _temp1282[ 3u]; _temp1282[ 2u]= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1264)); _temp1282[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1266); _temp1282[ 0u]= Cyc_Absynpp_qvar2doc( _temp1268->name); Cyc_PP_cat(
_tag_arr( _temp1282, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1191;
_LL1191:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1283[ 2u]; _temp1283[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1283[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1283, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause ==  0?( void*)( c->pattern)->r
== ( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1284[ 2u];
_temp1284[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1285[ 2u]; _temp1285[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1285[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1285, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1284[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1284, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 
0){ return({ struct Cyc_PP_Doc* _temp1286[ 4u]; _temp1286[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1287[ 2u]; _temp1287[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1287[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1287, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1286[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1286[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1286[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1286, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1288[ 6u]; _temp1288[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1289[ 2u]; _temp1289[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1289[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1289, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1288[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1288[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1288[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1288[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1288[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1288,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1292;
struct Cyc_Absyn_Stmt* _temp1293; struct Cyc_Absyn_Exp* _temp1295; struct Cyc_Absyn_SwitchC_clause*
_temp1290= c; _temp1292=* _temp1290; _LL1296: _temp1295= _temp1292.cnst_exp;
goto _LL1294; _LL1294: _temp1293= _temp1292.body; goto _LL1291; _LL1291: if(
_temp1295 ==  0){ return({ struct Cyc_PP_Doc* _temp1297[ 2u]; _temp1297[ 1u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1298[ 2u]; _temp1298[ 1u]= Cyc_Absynpp_stmt2doc(
c->body); _temp1298[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1298,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1297[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1297, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1299[ 4u];
_temp1299[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1300[ 2u]; _temp1300[
1u]= Cyc_Absynpp_stmt2doc( _temp1293); _temp1300[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1300, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1299[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1299[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1295)); _temp1299[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1299,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag ==  0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1301[ 3u]; _temp1301[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1301[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1301[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1301, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1302=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1328; struct Cyc_Absyn_Structdecl* _temp1330;
struct Cyc_Absyn_Uniondecl* _temp1332; struct Cyc_Absyn_Vardecl* _temp1334;
struct Cyc_Absyn_Vardecl _temp1336; struct Cyc_List_List* _temp1337; struct Cyc_Absyn_Exp*
_temp1339; void* _temp1341; struct Cyc_Absyn_Tqual _temp1343; struct _tuple0*
_temp1345; void* _temp1347; struct Cyc_Absyn_Tuniondecl* _temp1349; struct Cyc_Absyn_Tuniondecl
_temp1351; int _temp1352; struct Cyc_Core_Opt* _temp1354; struct Cyc_List_List*
_temp1356; struct _tuple0* _temp1358; void* _temp1360; struct Cyc_Absyn_Exp*
_temp1362; struct Cyc_Absyn_Pat* _temp1364; struct Cyc_List_List* _temp1366;
struct Cyc_Absyn_Enumdecl* _temp1368; struct Cyc_Absyn_Enumdecl _temp1370;
struct Cyc_Core_Opt* _temp1371; struct _tuple0* _temp1373; void* _temp1375;
struct Cyc_Absyn_Typedefdecl* _temp1377; struct Cyc_List_List* _temp1379; struct
_tagged_arr* _temp1381; struct Cyc_List_List* _temp1383; struct _tuple0*
_temp1385; struct Cyc_List_List* _temp1387; _LL1304: if(*(( int*) _temp1302) == 
Cyc_Absyn_Fn_d){ _LL1329: _temp1328=(( struct Cyc_Absyn_Fn_d_struct*) _temp1302)->f1;
goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*) _temp1302) ==  Cyc_Absyn_Struct_d){
_LL1331: _temp1330=(( struct Cyc_Absyn_Struct_d_struct*) _temp1302)->f1; goto
_LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*) _temp1302) ==  Cyc_Absyn_Union_d){
_LL1333: _temp1332=(( struct Cyc_Absyn_Union_d_struct*) _temp1302)->f1; goto
_LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*) _temp1302) ==  Cyc_Absyn_Var_d){
_LL1335: _temp1334=(( struct Cyc_Absyn_Var_d_struct*) _temp1302)->f1; _temp1336=*
_temp1334; _LL1348: _temp1347=( void*) _temp1336.sc; goto _LL1346; _LL1346:
_temp1345= _temp1336.name; goto _LL1344; _LL1344: _temp1343= _temp1336.tq; goto
_LL1342; _LL1342: _temp1341=( void*) _temp1336.type; goto _LL1340; _LL1340:
_temp1339= _temp1336.initializer; goto _LL1338; _LL1338: _temp1337= _temp1336.attributes;
goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*) _temp1302) ==  Cyc_Absyn_Tunion_d){
_LL1350: _temp1349=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1302)->f1;
_temp1351=* _temp1349; _LL1361: _temp1360=( void*) _temp1351.sc; goto _LL1359;
_LL1359: _temp1358= _temp1351.name; goto _LL1357; _LL1357: _temp1356= _temp1351.tvs;
goto _LL1355; _LL1355: _temp1354= _temp1351.fields; goto _LL1353; _LL1353:
_temp1352= _temp1351.is_xtunion; goto _LL1313;} else{ goto _LL1314;} _LL1314:
if(*(( int*) _temp1302) ==  Cyc_Absyn_Let_d){ _LL1365: _temp1364=(( struct Cyc_Absyn_Let_d_struct*)
_temp1302)->f1; goto _LL1363; _LL1363: _temp1362=(( struct Cyc_Absyn_Let_d_struct*)
_temp1302)->f4; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1302) ==  Cyc_Absyn_Letv_d){ _LL1367: _temp1366=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1302)->f1; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1302) ==  Cyc_Absyn_Enum_d){ _LL1369: _temp1368=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1302)->f1; _temp1370=* _temp1368; _LL1376: _temp1375=( void*) _temp1370.sc;
goto _LL1374; _LL1374: _temp1373= _temp1370.name; goto _LL1372; _LL1372:
_temp1371= _temp1370.fields; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*((
int*) _temp1302) ==  Cyc_Absyn_Typedef_d){ _LL1378: _temp1377=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1302)->f1; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1302) ==  Cyc_Absyn_Namespace_d){ _LL1382: _temp1381=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1302)->f1; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1302)->f2; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1302) ==  Cyc_Absyn_Using_d){ _LL1386: _temp1385=(( struct Cyc_Absyn_Using_d_struct*)
_temp1302)->f1; goto _LL1384; _LL1384: _temp1383=(( struct Cyc_Absyn_Using_d_struct*)
_temp1302)->f2; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1302) ==  Cyc_Absyn_ExternC_d){ _LL1388: _temp1387=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1302)->f1; goto _LL1327;} else{ goto _LL1303;} _LL1305: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1396=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1396[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1397; _temp1397.tag= Cyc_Absyn_FnType; _temp1397.f1=({ struct Cyc_Absyn_FnInfo
_temp1398; _temp1398.tvars= _temp1328->tvs; _temp1398.effect= _temp1328->effect;
_temp1398.ret_typ=( void*)(( void*) _temp1328->ret_type); _temp1398.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1328->args); _temp1398.c_varargs=
_temp1328->c_varargs; _temp1398.cyc_varargs= _temp1328->cyc_varargs; _temp1398.rgn_po=
_temp1328->rgn_po; _temp1398.attributes= 0; _temp1398;}); _temp1397;});
_temp1396;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1328->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1328->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1328->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1328->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1328->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1393; _temp1393.q_const= 0; _temp1393.q_volatile= 0; _temp1393.q_restrict=
0; _temp1393;}), t,({ struct Cyc_Core_Opt* _temp1394=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1394->v=( void*)({ struct Cyc_PP_Doc*
_temp1395[ 2u]; _temp1395[ 1u]= namedoc; _temp1395[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1395, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1394;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1391[ 4u]; _temp1391[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1391[ 2u]= Cyc_PP_line_doc();
_temp1391[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1392[ 2u]; _temp1392[
1u]= Cyc_Absynpp_stmt2doc( _temp1328->body); _temp1392[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1392, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1391[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1391, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1389[ 4u]; _temp1389[ 3u]= bodydoc; _temp1389[ 2u]= tqtddoc; _temp1389[ 1u]=
scopedoc; _temp1389[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1389, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1390[ 2u]; _temp1390[ 1u]= s; _temp1390[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1390, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1303;}} _LL1307: if(
_temp1330->fields ==  0){ s=({ struct Cyc_PP_Doc* _temp1399[ 5u]; _temp1399[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1399[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1330->tvs); _temp1399[ 2u]= _temp1330->name ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1330->name))->v); _temp1399[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1399[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1330->sc); Cyc_PP_cat( _tag_arr( _temp1399, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1400[ 10u]; _temp1400[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1400[ 8u]= Cyc_Absynpp_atts2doc(
_temp1330->attributes); _temp1400[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1400[ 6u]= Cyc_PP_line_doc(); _temp1400[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1401[ 2u]; _temp1401[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1330->fields))->v);
_temp1401[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1401, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1400[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1400[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1330->tvs);
_temp1400[ 2u]= _temp1330->name ==  0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1330->name))->v); _temp1400[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1400[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1330->sc); Cyc_PP_cat( _tag_arr( _temp1400, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1303; _LL1309: if( _temp1332->fields ==  0){ s=({ struct Cyc_PP_Doc*
_temp1402[ 5u]; _temp1402[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1402[ 3u]= Cyc_Absynpp_tvars2doc( _temp1332->tvs); _temp1402[ 2u]=
_temp1332->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1332->name))->v); _temp1402[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1402[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1332->sc);
Cyc_PP_cat( _tag_arr( _temp1402, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1403[ 10u]; _temp1403[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1403[ 8u]= Cyc_Absynpp_atts2doc( _temp1332->attributes);
_temp1403[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1403[ 6u]= Cyc_PP_line_doc(); _temp1403[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1404[ 2u]; _temp1404[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1332->fields))->v); _temp1404[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1404, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1403[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1403[ 3u]= Cyc_Absynpp_tvars2doc( _temp1332->tvs); _temp1403[ 2u]=
_temp1332->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1332->name))->v); _temp1403[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1403[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1332->sc);
Cyc_PP_cat( _tag_arr( _temp1403, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1303; _LL1311: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1345); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1337); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1405= Cyc_Tcutil_compress( _temp1341); struct Cyc_Absyn_FnInfo
_temp1411; struct Cyc_List_List* _temp1413; _LL1407: if(( unsigned int)
_temp1405 >  3u?*(( int*) _temp1405) ==  Cyc_Absyn_FnType: 0){ _LL1412:
_temp1411=(( struct Cyc_Absyn_FnType_struct*) _temp1405)->f1; _LL1414: _temp1413=
_temp1411.attributes; goto _LL1408;} else{ goto _LL1409;} _LL1409: goto _LL1410;
_LL1408: beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1413); goto _LL1406;
_LL1410: beforenamedoc= Cyc_PP_nil_doc(); goto _LL1406; _LL1406:;} s=({ struct
Cyc_PP_Doc* _temp1415[ 5u]; _temp1415[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); _temp1415[ 3u]= _temp1339 ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1418[ 2u]; _temp1418[ 1u]=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1339)); _temp1418[
0u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp1418, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1415[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1343, _temp1341,({ struct Cyc_Core_Opt* _temp1416=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1416->v=( void*)({ struct Cyc_PP_Doc*
_temp1417[ 2u]; _temp1417[ 1u]= sn; _temp1417[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1417, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1416;}));
_temp1415[ 1u]= Cyc_Absynpp_scope2doc( _temp1347); _temp1415[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1415, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1303;} _LL1313: if( _temp1354 ==  0){ s=({ struct Cyc_PP_Doc*
_temp1419[ 5u]; _temp1419[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1419[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1356); _temp1419[ 2u]=
_temp1352? Cyc_Absynpp_qvar2doc( _temp1358): Cyc_Absynpp_typedef_name2doc(
_temp1358); _temp1419[ 1u]= _temp1352? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1419[ 0u]= Cyc_Absynpp_scope2doc( _temp1360); Cyc_PP_cat( _tag_arr(
_temp1419, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1420[ 8u]; _temp1420[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1420[ 6u]= Cyc_PP_line_doc(); _temp1420[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1421[ 2u]; _temp1421[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*) _temp1354->v); _temp1421[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1421, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1420[ 4u]= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); _temp1420[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1356); _temp1420[ 2u]= _temp1352? Cyc_Absynpp_qvar2doc( _temp1358): Cyc_Absynpp_typedef_name2doc(
_temp1358); _temp1420[ 1u]= _temp1352? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1420[ 0u]= Cyc_Absynpp_scope2doc( _temp1360); Cyc_PP_cat( _tag_arr(
_temp1420, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1303; _LL1315: s=({
struct Cyc_PP_Doc* _temp1422[ 5u]; _temp1422[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1422[ 3u]= Cyc_Absynpp_exp2doc( _temp1362);
_temp1422[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1422[ 1u]= Cyc_Absynpp_pat2doc( _temp1364); _temp1422[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1422,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1303; _LL1317: s=({ struct Cyc_PP_Doc*
_temp1423[ 3u]; _temp1423[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1423[ 1u]= Cyc_Absynpp_ids2doc( _temp1366); _temp1423[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1423,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1303; _LL1319: if( _temp1371 ==  0){
s=({ struct Cyc_PP_Doc* _temp1424[ 4u]; _temp1424[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1424[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1373); _temp1424[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1424[ 0u]= Cyc_Absynpp_scope2doc( _temp1375); Cyc_PP_cat( _tag_arr(
_temp1424, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1425[ 7u]; _temp1425[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1425[ 5u]= Cyc_PP_line_doc(); _temp1425[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1426[ 2u]; _temp1426[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*) _temp1371->v); _temp1426[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1426, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1425[ 3u]= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); _temp1425[ 2u]= Cyc_Absynpp_qvar2doc(
_temp1373); _temp1425[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1425[ 0u]= Cyc_Absynpp_scope2doc( _temp1375); Cyc_PP_cat( _tag_arr(
_temp1425, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1303; _LL1321: s=({
struct Cyc_PP_Doc* _temp1427[ 3u]; _temp1427[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1427[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1428; _temp1428.q_const= 0; _temp1428.q_volatile= 0; _temp1428.q_restrict=
0; _temp1428;}),( void*) _temp1377->defn,({ struct Cyc_Core_Opt* _temp1429=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1429->v=(
void*)({ struct Cyc_PP_Doc* _temp1430[ 2u]; _temp1430[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1377->tvs); _temp1430[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1377->name);
Cyc_PP_cat( _tag_arr( _temp1430, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1429;}));
_temp1427[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1427, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1303; _LL1323: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1381);} s=({ struct Cyc_PP_Doc* _temp1431[ 7u]; _temp1431[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1431[ 5u]= Cyc_PP_line_doc();
_temp1431[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1379); _temp1431[ 3u]= Cyc_PP_line_doc();
_temp1431[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1431[ 1u]= Cyc_PP_textptr( _temp1381); _temp1431[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1431, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1303; _LL1325: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1432[ 7u]; _temp1432[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1432[ 5u]= Cyc_PP_line_doc(); _temp1432[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1383); _temp1432[ 3u]= Cyc_PP_line_doc();
_temp1432[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1432[ 1u]= Cyc_Absynpp_qvar2doc( _temp1385); _temp1432[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1432,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1433[
7u]; _temp1433[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1433[ 5u]= Cyc_PP_line_doc(); _temp1433[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1383); _temp1433[ 3u]= Cyc_PP_line_doc();
_temp1433[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1433[ 1u]= Cyc_Absynpp_qvar2doc( _temp1385); _temp1433[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1433, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1303; _LL1327: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1434[ 5u]; _temp1434[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1434[ 3u]= Cyc_PP_line_doc(); _temp1434[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1387); _temp1434[ 1u]= Cyc_PP_line_doc();
_temp1434[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1434, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1435[ 5u]; _temp1435[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1435[ 3u]= Cyc_PP_line_doc();
_temp1435[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1387); _temp1435[ 1u]= Cyc_PP_line_doc();
_temp1435[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1435, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1303; _LL1303:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1436= sc; _LL1438: if( _temp1436 == ( void*) Cyc_Absyn_Static){
goto _LL1439;} else{ goto _LL1440;} _LL1440: if( _temp1436 == ( void*) Cyc_Absyn_Public){
goto _LL1441;} else{ goto _LL1442;} _LL1442: if( _temp1436 == ( void*) Cyc_Absyn_Extern){
goto _LL1443;} else{ goto _LL1444;} _LL1444: if( _temp1436 == ( void*) Cyc_Absyn_ExternC){
goto _LL1445;} else{ goto _LL1446;} _LL1446: if( _temp1436 == ( void*) Cyc_Absyn_Abstract){
goto _LL1447;} else{ goto _LL1437;} _LL1439: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1441: return Cyc_PP_nil_doc(); _LL1443: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1445: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1447:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1437:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1448= t; struct
Cyc_Absyn_Tvar* _temp1456; struct Cyc_List_List* _temp1458; _LL1450: if((
unsigned int) _temp1448 >  3u?*(( int*) _temp1448) ==  Cyc_Absyn_VarType: 0){
_LL1457: _temp1456=(( struct Cyc_Absyn_VarType_struct*) _temp1448)->f1; goto
_LL1451;} else{ goto _LL1452;} _LL1452: if(( unsigned int) _temp1448 >  3u?*((
int*) _temp1448) ==  Cyc_Absyn_JoinEff: 0){ _LL1459: _temp1458=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1448)->f1; goto _LL1453;} else{ goto _LL1454;} _LL1454: goto _LL1455;
_LL1451: return Cyc_Tcutil_is_temp_tvar( _temp1456); _LL1453: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1458); _LL1455: return 0; _LL1449:;}
int Cyc_Absynpp_is_anon_structtype( void* t){ void* _temp1460= t; struct _tuple0*
_temp1472; struct Cyc_Core_Opt* _temp1474; struct Cyc_Core_Opt* _temp1476;
_LL1462: if(( unsigned int) _temp1460 >  3u?*(( int*) _temp1460) ==  Cyc_Absyn_StructType:
0){ _LL1473: _temp1472=(( struct Cyc_Absyn_StructType_struct*) _temp1460)->f1;
if( _temp1472 ==  0){ goto _LL1463;} else{ goto _LL1464;}} else{ goto _LL1464;}
_LL1464: if(( unsigned int) _temp1460 >  3u?*(( int*) _temp1460) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL1465;} else{ goto _LL1466;} _LL1466: if(( unsigned int) _temp1460 > 
3u?*(( int*) _temp1460) ==  Cyc_Absyn_TypedefType: 0){ _LL1475: _temp1474=((
struct Cyc_Absyn_TypedefType_struct*) _temp1460)->f3; if( _temp1474 ==  0){ goto
_LL1467;} else{ goto _LL1468;}} else{ goto _LL1468;} _LL1468: if(( unsigned int)
_temp1460 >  3u?*(( int*) _temp1460) ==  Cyc_Absyn_TypedefType: 0){ _LL1477:
_temp1476=(( struct Cyc_Absyn_TypedefType_struct*) _temp1460)->f3; goto _LL1469;}
else{ goto _LL1470;} _LL1470: goto _LL1471; _LL1463: return 1; _LL1465: return 1;
_LL1467: return 0; _LL1469: return Cyc_Absynpp_is_anon_structtype(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1476))->v); _LL1471: return 0; _LL1461:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1478= t; struct Cyc_Absyn_Exp* _temp1492; struct Cyc_Absyn_Tqual _temp1494;
void* _temp1496; struct Cyc_Absyn_PtrInfo _temp1498; struct Cyc_Absyn_Conref*
_temp1500; struct Cyc_Absyn_Tqual _temp1502; struct Cyc_Absyn_Conref* _temp1504;
void* _temp1506; void* _temp1508; struct Cyc_Absyn_FnInfo _temp1510; struct Cyc_List_List*
_temp1512; struct Cyc_List_List* _temp1514; struct Cyc_Absyn_VarargInfo*
_temp1516; int _temp1518; struct Cyc_List_List* _temp1520; void* _temp1522;
struct Cyc_Core_Opt* _temp1524; struct Cyc_List_List* _temp1526; int _temp1528;
struct Cyc_Core_Opt* _temp1530; struct Cyc_Core_Opt* _temp1532; struct Cyc_Core_Opt*
_temp1534; struct Cyc_List_List* _temp1536; struct _tuple0* _temp1538; _LL1480:
if(( unsigned int) _temp1478 >  3u?*(( int*) _temp1478) ==  Cyc_Absyn_ArrayType:
0){ _LL1497: _temp1496=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1478)->f1;
goto _LL1495; _LL1495: _temp1494=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1478)->f2; goto _LL1493; _LL1493: _temp1492=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1478)->f3; goto _LL1481;} else{ goto _LL1482;} _LL1482: if(( unsigned int)
_temp1478 >  3u?*(( int*) _temp1478) ==  Cyc_Absyn_PointerType: 0){ _LL1499:
_temp1498=(( struct Cyc_Absyn_PointerType_struct*) _temp1478)->f1; _LL1509:
_temp1508=( void*) _temp1498.elt_typ; goto _LL1507; _LL1507: _temp1506=( void*)
_temp1498.rgn_typ; goto _LL1505; _LL1505: _temp1504= _temp1498.nullable; goto
_LL1503; _LL1503: _temp1502= _temp1498.tq; goto _LL1501; _LL1501: _temp1500=
_temp1498.bounds; goto _LL1483;} else{ goto _LL1484;} _LL1484: if(( unsigned int)
_temp1478 >  3u?*(( int*) _temp1478) ==  Cyc_Absyn_FnType: 0){ _LL1511:
_temp1510=(( struct Cyc_Absyn_FnType_struct*) _temp1478)->f1; _LL1527: _temp1526=
_temp1510.tvars; goto _LL1525; _LL1525: _temp1524= _temp1510.effect; goto
_LL1523; _LL1523: _temp1522=( void*) _temp1510.ret_typ; goto _LL1521; _LL1521:
_temp1520= _temp1510.args; goto _LL1519; _LL1519: _temp1518= _temp1510.c_varargs;
goto _LL1517; _LL1517: _temp1516= _temp1510.cyc_varargs; goto _LL1515; _LL1515:
_temp1514= _temp1510.rgn_po; goto _LL1513; _LL1513: _temp1512= _temp1510.attributes;
goto _LL1485;} else{ goto _LL1486;} _LL1486: if(( unsigned int) _temp1478 >  3u?*((
int*) _temp1478) ==  Cyc_Absyn_Evar: 0){ _LL1533: _temp1532=(( struct Cyc_Absyn_Evar_struct*)
_temp1478)->f1; goto _LL1531; _LL1531: _temp1530=(( struct Cyc_Absyn_Evar_struct*)
_temp1478)->f2; goto _LL1529; _LL1529: _temp1528=(( struct Cyc_Absyn_Evar_struct*)
_temp1478)->f3; goto _LL1487;} else{ goto _LL1488;} _LL1488: if(( unsigned int)
_temp1478 >  3u?*(( int*) _temp1478) ==  Cyc_Absyn_TypedefType: 0){ _LL1539:
_temp1538=(( struct Cyc_Absyn_TypedefType_struct*) _temp1478)->f1; goto _LL1537;
_LL1537: _temp1536=(( struct Cyc_Absyn_TypedefType_struct*) _temp1478)->f2; goto
_LL1535; _LL1535: _temp1534=(( struct Cyc_Absyn_TypedefType_struct*) _temp1478)->f3;
goto _LL1489;} else{ goto _LL1490;} _LL1490: goto _LL1491; _LL1481: { struct Cyc_List_List*
_temp1542; void* _temp1544; struct Cyc_Absyn_Tqual _temp1546; struct _tuple4
_temp1540= Cyc_Absynpp_to_tms( _temp1494, _temp1496); _LL1547: _temp1546=
_temp1540.f1; goto _LL1545; _LL1545: _temp1544= _temp1540.f2; goto _LL1543;
_LL1543: _temp1542= _temp1540.f3; goto _LL1541; _LL1541: { void* tm; if(
_temp1492 ==  0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1548=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1548[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1549; _temp1549.tag= Cyc_Absyn_ConstArray_mod;
_temp1549.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1492); _temp1549;});
_temp1548;});} return({ struct _tuple4 _temp1550; _temp1550.f1= _temp1546;
_temp1550.f2= _temp1544; _temp1550.f3=({ struct Cyc_List_List* _temp1551=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1551->hd=(
void*) tm; _temp1551->tl= _temp1542; _temp1551;}); _temp1550;});}} _LL1483: {
struct Cyc_List_List* _temp1554; void* _temp1556; struct Cyc_Absyn_Tqual
_temp1558; struct _tuple4 _temp1552= Cyc_Absynpp_to_tms( _temp1502, _temp1508);
_LL1559: _temp1558= _temp1552.f1; goto _LL1557; _LL1557: _temp1556= _temp1552.f2;
goto _LL1555; _LL1555: _temp1554= _temp1552.f3; goto _LL1553; _LL1553: { void*
ps;{ struct _tuple8 _temp1561=({ struct _tuple8 _temp1560; _temp1560.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1504))->v; _temp1560.f2=( void*)( Cyc_Absyn_compress_conref( _temp1500))->v;
_temp1560;}); void* _temp1571; void* _temp1573; struct Cyc_Absyn_Exp* _temp1575;
void* _temp1577; int _temp1579; void* _temp1581; void* _temp1583; struct Cyc_Absyn_Exp*
_temp1585; void* _temp1587; int _temp1589; void* _temp1591; void* _temp1593;
_LL1563: _LL1578: _temp1577= _temp1561.f1; if(( unsigned int) _temp1577 >  1u?*((
int*) _temp1577) ==  Cyc_Absyn_Eq_constr: 0){ _LL1580: _temp1579=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1577)->f1; if( _temp1579 ==  1){ goto _LL1572;}
else{ goto _LL1565;}} else{ goto _LL1565;} _LL1572: _temp1571= _temp1561.f2; if((
unsigned int) _temp1571 >  1u?*(( int*) _temp1571) ==  Cyc_Absyn_Eq_constr: 0){
_LL1574: _temp1573=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1571)->f1;
if(( unsigned int) _temp1573 >  1u?*(( int*) _temp1573) ==  Cyc_Absyn_Upper_b: 0){
_LL1576: _temp1575=(( struct Cyc_Absyn_Upper_b_struct*) _temp1573)->f1; goto
_LL1564;} else{ goto _LL1565;}} else{ goto _LL1565;} _LL1565: _LL1588: _temp1587=
_temp1561.f1; if(( unsigned int) _temp1587 >  1u?*(( int*) _temp1587) ==  Cyc_Absyn_Eq_constr:
0){ _LL1590: _temp1589=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1587)->f1;
if( _temp1589 ==  0){ goto _LL1582;} else{ goto _LL1567;}} else{ goto _LL1567;}
_LL1582: _temp1581= _temp1561.f2; if(( unsigned int) _temp1581 >  1u?*(( int*)
_temp1581) ==  Cyc_Absyn_Eq_constr: 0){ _LL1584: _temp1583=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1581)->f1; if(( unsigned int) _temp1583 >  1u?*(( int*) _temp1583) ==  Cyc_Absyn_Upper_b:
0){ _LL1586: _temp1585=(( struct Cyc_Absyn_Upper_b_struct*) _temp1583)->f1; goto
_LL1566;} else{ goto _LL1567;}} else{ goto _LL1567;} _LL1567: _LL1592: _temp1591=
_temp1561.f2; if(( unsigned int) _temp1591 >  1u?*(( int*) _temp1591) ==  Cyc_Absyn_Eq_constr:
0){ _LL1594: _temp1593=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1591)->f1;
if( _temp1593 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1568;} else{ goto
_LL1569;}} else{ goto _LL1569;} _LL1569: goto _LL1570; _LL1564: ps=( void*)({
struct Cyc_Absyn_Nullable_ps_struct* _temp1595=( struct Cyc_Absyn_Nullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp1595[ 0]=({
struct Cyc_Absyn_Nullable_ps_struct _temp1596; _temp1596.tag= Cyc_Absyn_Nullable_ps;
_temp1596.f1= _temp1575; _temp1596;}); _temp1595;}); goto _LL1562; _LL1566: ps=(
void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp1597=( struct Cyc_Absyn_NonNullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp1597[ 0]=({
struct Cyc_Absyn_NonNullable_ps_struct _temp1598; _temp1598.tag= Cyc_Absyn_NonNullable_ps;
_temp1598.f1= _temp1585; _temp1598;}); _temp1597;}); goto _LL1562; _LL1568: ps=(
void*) Cyc_Absyn_TaggedArray_ps; goto _LL1562; _LL1570: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1562; _LL1562:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp1601=( struct Cyc_Absyn_Pointer_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1601[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1602; _temp1602.tag=
Cyc_Absyn_Pointer_mod; _temp1602.f1=( void*) ps; _temp1602.f2=( void*) _temp1506;
_temp1602.f3= tq; _temp1602;}); _temp1601;}); return({ struct _tuple4 _temp1599;
_temp1599.f1= _temp1558; _temp1599.f2= _temp1556; _temp1599.f3=({ struct Cyc_List_List*
_temp1600=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1600->hd=( void*) tm; _temp1600->tl= _temp1554; _temp1600;}); _temp1599;});}}}
_LL1485: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1524 ==  0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*) _temp1524->v)){ _temp1524= 0; _temp1526= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp1526);}}{ struct Cyc_List_List* _temp1605;
void* _temp1607; struct Cyc_Absyn_Tqual _temp1609; struct _tuple4 _temp1603= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp1522); _LL1610: _temp1609= _temp1603.f1; goto
_LL1608; _LL1608: _temp1607= _temp1603.f2; goto _LL1606; _LL1606: _temp1605=
_temp1603.f3; goto _LL1604; _LL1604: { struct Cyc_List_List* tms= _temp1605; if(
_temp1512 !=  0? ! Cyc_Absynpp_to_VC: 0){ tms=({ struct Cyc_List_List* _temp1611=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1611->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1612=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1612[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1613; _temp1613.tag= Cyc_Absyn_Attributes_mod;
_temp1613.f1= 0; _temp1613.f2= _temp1512; _temp1613;}); _temp1612;})); _temp1611->tl=
tms; _temp1611;});} tms=({ struct Cyc_List_List* _temp1614=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1614->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp1615=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp1615[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp1616; _temp1616.tag= Cyc_Absyn_Function_mod;
_temp1616.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp1617=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1617[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1618; _temp1618.tag= Cyc_Absyn_WithTypes;
_temp1618.f1= _temp1520; _temp1618.f2= _temp1518; _temp1618.f3= _temp1516;
_temp1618.f4= _temp1524; _temp1618.f5= _temp1514; _temp1618;}); _temp1617;}));
_temp1616;}); _temp1615;})); _temp1614->tl= tms; _temp1614;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1512 !=  0; _temp1512= _temp1512->tl){ void* _temp1619=( void*)
_temp1512->hd; _LL1621: if( _temp1619 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1622;} else{ goto _LL1623;} _LL1623: if( _temp1619 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1624;} else{ goto _LL1625;} _LL1625: if( _temp1619 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1626;} else{ goto _LL1627;} _LL1627: goto _LL1628; _LL1622: goto _LL1624;
_LL1624: goto _LL1626; _LL1626: tms=({ struct Cyc_List_List* _temp1629=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1629->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1630=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1630[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1631; _temp1631.tag= Cyc_Absyn_Attributes_mod;
_temp1631.f1= 0; _temp1631.f2=({ struct Cyc_List_List* _temp1632=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1632->hd=( void*)(( void*)
_temp1512->hd); _temp1632->tl= 0; _temp1632;}); _temp1631;}); _temp1630;}));
_temp1629->tl= tms; _temp1629;}); goto AfterAtts; _LL1628: goto _LL1620; _LL1620:;}}
AfterAtts: if( _temp1526 !=  0){ tms=({ struct Cyc_List_List* _temp1633=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1633->hd=( void*)((
void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp1634=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp1634[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp1635; _temp1635.tag= Cyc_Absyn_TypeParams_mod;
_temp1635.f1= _temp1526; _temp1635.f2= 0; _temp1635.f3= 1; _temp1635;});
_temp1634;})); _temp1633->tl= tms; _temp1633;});} return({ struct _tuple4
_temp1636; _temp1636.f1= _temp1609; _temp1636.f2= _temp1607; _temp1636.f3= tms;
_temp1636;});}} _LL1487: if( _temp1530 ==  0){ return({ struct _tuple4 _temp1637;
_temp1637.f1= tq; _temp1637.f2= t; _temp1637.f3= 0; _temp1637;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*) _temp1530->v);} _LL1489: if(( _temp1534 ==  0? 1:
! Cyc_Absynpp_expand_typedefs)? 1: Cyc_Absynpp_is_anon_structtype(( void*)
_temp1534->v)){ return({ struct _tuple4 _temp1638; _temp1638.f1= tq; _temp1638.f2=
t; _temp1638.f3= 0; _temp1638;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)
_temp1534->v);} _LL1491: return({ struct _tuple4 _temp1639; _temp1639.f1= tq;
_temp1639.f2= t; _temp1639.f3= 0; _temp1639;}); _LL1479:;} struct Cyc_PP_Doc*
Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt*
dopt){ struct Cyc_List_List* _temp1642; void* _temp1644; struct Cyc_Absyn_Tqual
_temp1646; struct _tuple4 _temp1640= Cyc_Absynpp_to_tms( tq, t); _LL1647:
_temp1646= _temp1640.f1; goto _LL1645; _LL1645: _temp1644= _temp1640.f2; goto
_LL1643; _LL1643: _temp1642= _temp1640.f3; goto _LL1641; _LL1641: _temp1642= Cyc_List_imp_rev(
_temp1642); if( _temp1642 ==  0? dopt ==  0: 0){ return({ struct Cyc_PP_Doc*
_temp1648[ 2u]; _temp1648[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1644); _temp1648[ 0u]=
Cyc_Absynpp_tqual2doc( _temp1646); Cyc_PP_cat( _tag_arr( _temp1648, sizeof(
struct Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1649[ 4u];
_temp1649[ 3u]= Cyc_Absynpp_dtms2doc( dopt ==  0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)
dopt->v, _temp1642); _temp1649[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof(
unsigned char), 2u)); _temp1649[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1644);
_temp1649[ 0u]= Cyc_Absynpp_tqual2doc( _temp1646); Cyc_PP_cat( _tag_arr(
_temp1649, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width !=  0){ return({ struct Cyc_PP_Doc*
_temp1650[ 5u]; _temp1650[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1650[ 3u]= Cyc_Absynpp_atts2doc( f->attributes); _temp1650[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->width)); _temp1650[ 1u]= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); _temp1650[ 0u]= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp1651=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1651->v=( void*) Cyc_PP_textptr(
f->name); _temp1651;})); Cyc_PP_cat( _tag_arr( _temp1650, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ return({ struct Cyc_PP_Doc* _temp1652[ 3u]; _temp1652[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1652[ 1u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1652[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1653=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1653->v=( void*) Cyc_PP_textptr( f->name); _temp1653;}));
Cyc_PP_cat( _tag_arr( _temp1652, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,
_tag_arr("", sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1654[ 4u];
_temp1654[ 3u]= f->typs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1654[ 2u]= f->tvs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1654[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1654[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1654, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f){ for( 0; tdl !=  0; tdl=
tdl->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd),
72, f);({ void* _temp1655[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1655, sizeof( void*), 0u));});}} struct
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
72);} struct _tagged_arr Cyc_Absynpp_scope2string( void* sc){ void* _temp1656=
sc; _LL1658: if( _temp1656 == ( void*) Cyc_Absyn_Static){ goto _LL1659;} else{
goto _LL1660;} _LL1660: if( _temp1656 == ( void*) Cyc_Absyn_Public){ goto
_LL1661;} else{ goto _LL1662;} _LL1662: if( _temp1656 == ( void*) Cyc_Absyn_Extern){
goto _LL1663;} else{ goto _LL1664;} _LL1664: if( _temp1656 == ( void*) Cyc_Absyn_ExternC){
goto _LL1665;} else{ goto _LL1666;} _LL1666: if( _temp1656 == ( void*) Cyc_Absyn_Abstract){
goto _LL1667;} else{ goto _LL1657;} _LL1659: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1661: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1663: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1665:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1667: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1657:;}
