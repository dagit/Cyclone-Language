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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
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
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* cmp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d,
void* k); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d,
void* k, void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void*
k); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d,
void* k); struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty( int(*
cmp)( void*, void*)); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void*
elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_SlowDict_Dict;
extern unsigned char Cyc_SlowDict_Present[ 12u]; extern unsigned char Cyc_SlowDict_Absent[
11u]; extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* cmp)( void*,
void*)); extern int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d); extern
struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void*
k, void* v); extern struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict*
d, void* k); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict*
d, void* k); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; static const int
Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
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
15u]; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*);
extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct _tuple0*
Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud;
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tagged_arr*, struct Cyc_List_List*); extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds,
struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next(
struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_arr*, struct Cyc_Absyn_Stmt**);
struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_arr*, struct Cyc_Absyn_Stmt*); extern int
Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar* name); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region(
struct Cyc_Tcenv_Tenv* te, void* r); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern int Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv*,
void* r1, void* r2); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct
Cyc_List_List*); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k);
extern int* Cyc_Tcutil_new_tvar_id(); extern void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*);
unsigned char Cyc_Tcenv_Env_error[ 14u]="\000\000\000\000Env_error"; void* Cyc_Tcenv_env_err(
struct _tagged_arr msg){({ struct Cyc_Std_String_pa_struct _temp1; _temp1.tag=
Cyc_Std_String_pa; _temp1.f1=( struct _tagged_arr) msg;{ void* _temp0[ 1u]={&
_temp1}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: %s\n", sizeof(
unsigned char), 11u), _tag_arr( _temp0, sizeof( void*), 1u));}}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*) Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv; struct Cyc_Tcenv_CtrlEnv{ void*
continue_stmt; void* break_stmt; struct _tuple3* fallthru_clause; void*
next_stmt; int try_depth; } ; struct Cyc_Tcenv_SharedFenv{ void* return_typ;
struct Cyc_Dict_Dict* seen_labels; struct Cyc_SlowDict_Dict* needed_labels; } ;
struct Cyc_Tcenv_RgnPO{ struct Cyc_Absyn_Tvar* tvar; struct Cyc_List_List*
ancestors; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv* shared;
struct Cyc_List_List* type_vars; struct Cyc_List_List* region_order; struct Cyc_Dict_Dict*
locals; struct Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env;
void* capability; void* curr_rgn; } ; struct _tuple4{ void* f1; int f2; } ;
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({ struct
Cyc_Core_Opt* _temp2=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_zstrptrcmp); _temp2;});{ struct
Cyc_Tcenv_Genv* _temp3= Cyc_Tcenv_empty_genv(); struct Cyc_List_List* _temp4= 0;
_temp3->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k, struct Cyc_Absyn_Tuniondecl** v)) Cyc_Dict_insert)( _temp3->tuniondecls,(*
Cyc_Absyn_exn_name).f2,({ struct Cyc_Absyn_Tuniondecl** _temp5=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp5[ 0]= Cyc_Absyn_exn_tud;
_temp5;}));{ struct Cyc_List_List* _temp6=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Absyn_exn_tud->fields))->v; for( 0; _temp6 !=  0; _temp6=
_temp6->tl){ _temp3->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)( _temp3->ordinaries,(*((
struct Cyc_Absyn_Tunionfield*) _temp6->hd)->name).f2,({ struct _tuple4* _temp7=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp7->f1=( void*)({
struct Cyc_Tcenv_TunionRes_struct* _temp8=( struct Cyc_Tcenv_TunionRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp8[ 0]=({ struct Cyc_Tcenv_TunionRes_struct
_temp9; _temp9.tag= Cyc_Tcenv_TunionRes; _temp9.f1= Cyc_Absyn_exn_tud; _temp9.f2=(
struct Cyc_Absyn_Tunionfield*) _temp6->hd; _temp9;}); _temp8;}); _temp7->f2= 1;
_temp7;}));}}{ struct Cyc_Dict_Dict* ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k, struct Cyc_Tcenv_Genv* v)) Cyc_Dict_insert)(((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct Cyc_List_List*, struct Cyc_List_List*)))
Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), _temp4, _temp3); return({ struct Cyc_Tcenv_Tenv*
_temp10=( struct Cyc_Tcenv_Tenv*) _cycalloc( sizeof( struct Cyc_Tcenv_Tenv));
_temp10->ns= _temp4; _temp10->ae= ae; _temp10->le= 0; _temp10;});}}} static
struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr* n, struct Cyc_List_List*
ns){ return(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, Cyc_Tcenv_resolve_namespace( te, loc, n, ns));}
static struct Cyc_List_List* Cyc_Tcenv_outer_namespace( struct Cyc_List_List* ns){
if( ns ==  0){ return(( struct Cyc_List_List*(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv::outer_namespace", sizeof( unsigned char), 23u));} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
ns)))->tl);} static int Cyc_Tcenv_same_namespace( struct Cyc_List_List* n1,
struct Cyc_List_List* n2){ if( n1 ==  n2){ return 1;} for( 0; n1 !=  0; n1= n1->tl){
if( n2 ==  0){ return 0;} if( Cyc_Std_zstrptrcmp(( struct _tagged_arr*) n1->hd,(
struct _tagged_arr*) n2->hd) !=  0){ return 0;} n2= n2->tl;} if( n2 !=  0){
return 0;} return 1;} static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_arr*), struct _tagged_arr* v, struct Cyc_List_List* cns, struct
Cyc_List_List* nss){ for( 0; nss !=  0; nss= nss->tl){ if( ! Cyc_Tcenv_same_namespace(
cns,( struct Cyc_List_List*) nss->hd)){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*) nss->hd); struct _handler_cons _temp11; _push_handler(&
_temp11);{ int _temp13= 0; if( setjmp( _temp11.handler)){ _temp13= 1;} if( !
_temp13){ lookup( ge2, v);({ struct Cyc_Std_String_pa_struct _temp15; _temp15.tag=
Cyc_Std_String_pa; _temp15.f1=( struct _tagged_arr)* v;{ void* _temp14[ 1u]={&
_temp15}; Cyc_Tcutil_terr( loc, _tag_arr("%s is ambiguous", sizeof(
unsigned char), 16u), _tag_arr( _temp14, sizeof( void*), 1u));}});; _pop_handler();}
else{ void* _temp12=( void*) _exn_thrown; void* _temp17= _temp12; _LL19: if(
_temp17 ==  Cyc_Dict_Absent){ goto _LL20;} else{ goto _LL21;} _LL21: goto _LL22;
_LL20: goto _LL18; _LL22:( void) _throw( _temp17); _LL18:;}}}} return;} static
void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct
_tagged_arr* v){ struct Cyc_List_List* cns= te->ns; while( 1) { struct Cyc_Tcenv_Genv*
ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, cns);{ struct _handler_cons _temp23; _push_handler(&
_temp23);{ int _temp25= 0; if( setjmp( _temp23.handler)){ _temp25= 1;} if( !
_temp25){{ void* result= lookup( ge, v); Cyc_Tcenv_check_repeat( te, loc, lookup,
v, cns, ge->availables);{ void* _temp26= result; _npop_handler( 0u); return
_temp26;}}; _pop_handler();} else{ void* _temp24=( void*) _exn_thrown; void*
_temp28= _temp24; _LL30: if( _temp28 ==  Cyc_Dict_Absent){ goto _LL31;} else{
goto _LL32;} _LL32: goto _LL33; _LL31: goto _LL29; _LL33:( void) _throw( _temp28);
_LL29:;}}}{ struct Cyc_List_List* _temp34= ge->availables; for( 0; _temp34 !=  0;
_temp34= _temp34->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*) _temp34->hd); struct _handler_cons _temp35; _push_handler(&
_temp35);{ int _temp37= 0; if( setjmp( _temp35.handler)){ _temp37= 1;} if( !
_temp37){{ void* result= lookup( ge2, v); Cyc_Tcenv_check_repeat( te, loc,
lookup, v,( struct Cyc_List_List*) _temp34->hd, _temp34->tl);{ void* _temp38=
result; _npop_handler( 0u); return _temp38;}}; _pop_handler();} else{ void*
_temp36=( void*) _exn_thrown; void* _temp40= _temp36; _LL42: if( _temp40 ==  Cyc_Dict_Absent){
goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL43: goto _LL41; _LL45:(
void) _throw( _temp40); _LL41:;}}}} if( cns ==  0){( int) _throw(( void*) Cyc_Dict_Absent);}
cns= Cyc_Tcenv_outer_namespace( cns);}} static void* Cyc_Tcenv_lookup_ordinary_global_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ struct _tuple4* ans=(( struct
_tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
ge->ordinaries, v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp48; struct _tagged_arr* _temp49; void* _temp51; struct
_tuple0* _temp46= q; _temp48=* _temp46; _LL52: _temp51= _temp48.f1; goto _LL50;
_LL50: _temp49= _temp48.f2; goto _LL47; _LL47: { void* _temp53= _temp51; struct
Cyc_List_List* _temp63; struct Cyc_List_List* _temp65; struct Cyc_List_List
_temp67; struct Cyc_List_List* _temp68; struct _tagged_arr* _temp70; struct Cyc_List_List*
_temp72; _LL55: if( _temp53 == ( void*) Cyc_Absyn_Loc_n){ goto _LL56;} else{
goto _LL57;} _LL57: if(( unsigned int) _temp53 >  1u?*(( int*) _temp53) ==  Cyc_Absyn_Rel_n:
0){ _LL64: _temp63=(( struct Cyc_Absyn_Rel_n_struct*) _temp53)->f1; if( _temp63
==  0){ goto _LL58;} else{ goto _LL59;}} else{ goto _LL59;} _LL59: if((
unsigned int) _temp53 >  1u?*(( int*) _temp53) ==  Cyc_Absyn_Rel_n: 0){ _LL66:
_temp65=(( struct Cyc_Absyn_Rel_n_struct*) _temp53)->f1; if( _temp65 ==  0){
goto _LL61;} else{ _temp67=* _temp65; _LL71: _temp70=( struct _tagged_arr*)
_temp67.hd; goto _LL69; _LL69: _temp68= _temp67.tl; goto _LL60;}} else{ goto
_LL61;} _LL61: if(( unsigned int) _temp53 >  1u?*(( int*) _temp53) ==  Cyc_Absyn_Abs_n:
0){ _LL73: _temp72=(( struct Cyc_Absyn_Abs_n_struct*) _temp53)->f1; goto _LL62;}
else{ goto _LL54;} _LL56: goto _LL58; _LL58: return Cyc_Tcenv_scoped_lookup( te,
loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp49); _LL60: { struct Cyc_Tcenv_Genv*
_temp74= Cyc_Tcenv_lookup_namespace( te, loc, _temp70, _temp68); return Cyc_Tcenv_lookup_ordinary_global_f(
_temp74, _temp49);} _LL62: return Cyc_Tcenv_lookup_ordinary_global_f((( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)(
te->ae, _temp72), _temp49); _LL54:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp75= te->ns; struct Cyc_List_List*
_temp76= 0; while( 1) { struct Cyc_Tcenv_Genv* _temp77=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae,
_temp75);{ struct Cyc_List_List* _temp78= _temp77->availables; for( 0; _temp78
!=  0; _temp78= _temp78->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*) _temp78->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp76=({ struct Cyc_List_List*
_temp79=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp79->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp75,({ struct Cyc_List_List* _temp80=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp80->hd=(
void*) n; _temp80->tl= ns; _temp80;})); _temp79->tl= _temp76; _temp79;});}}} if(((
int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp77->namespaces, n)){ _temp76=({ struct Cyc_List_List* _temp81=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp81->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp75,({
struct Cyc_List_List* _temp82=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp82->hd=( void*) n; _temp82->tl= ns; _temp82;})); _temp81->tl=
_temp76; _temp81;});} if( _temp76 !=  0){ if( _temp76->tl !=  0){({ struct Cyc_Std_String_pa_struct
_temp84; _temp84.tag= Cyc_Std_String_pa; _temp84.f1=( struct _tagged_arr)* n;{
void* _temp83[ 1u]={& _temp84}; Cyc_Tcutil_terr( loc, _tag_arr("%s is ambiguous",
sizeof( unsigned char), 16u), _tag_arr( _temp83, sizeof( void*), 1u));}});}
return( struct Cyc_List_List*) _temp76->hd;} if( _temp75 ==  0){( int) _throw((
void*) Cyc_Dict_Absent);} _temp75= Cyc_Tcenv_outer_namespace( _temp75);}} static
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Structdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->structdecls, v);}
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp87;
struct _tagged_arr* _temp88; void* _temp90; struct _tuple0* _temp85= q; _temp87=*
_temp85; _LL91: _temp90= _temp87.f1; goto _LL89; _LL89: _temp88= _temp87.f2;
goto _LL86; _LL86: { void* _temp92= _temp90; struct Cyc_List_List* _temp102;
struct Cyc_List_List* _temp104; struct Cyc_List_List* _temp106; struct Cyc_List_List
_temp108; struct Cyc_List_List* _temp109; struct _tagged_arr* _temp111; _LL94:
if( _temp92 == ( void*) Cyc_Absyn_Loc_n){ goto _LL95;} else{ goto _LL96;} _LL96:
if(( unsigned int) _temp92 >  1u?*(( int*) _temp92) ==  Cyc_Absyn_Rel_n: 0){
_LL103: _temp102=(( struct Cyc_Absyn_Rel_n_struct*) _temp92)->f1; if( _temp102
==  0){ goto _LL97;} else{ goto _LL98;}} else{ goto _LL98;} _LL98: if((
unsigned int) _temp92 >  1u?*(( int*) _temp92) ==  Cyc_Absyn_Abs_n: 0){ _LL105:
_temp104=(( struct Cyc_Absyn_Abs_n_struct*) _temp92)->f1; goto _LL99;} else{
goto _LL100;} _LL100: if(( unsigned int) _temp92 >  1u?*(( int*) _temp92) == 
Cyc_Absyn_Rel_n: 0){ _LL107: _temp106=(( struct Cyc_Absyn_Rel_n_struct*) _temp92)->f1;
if( _temp106 ==  0){ goto _LL93;} else{ _temp108=* _temp106; _LL112: _temp111=(
struct _tagged_arr*) _temp108.hd; goto _LL110; _LL110: _temp109= _temp108.tl;
goto _LL101;}} else{ goto _LL93;} _LL95: goto _LL97; _LL97: return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp88);
_LL99: { struct Cyc_Tcenv_Genv* _temp113=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, _temp104);
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( _temp113->structdecls, _temp88);} _LL101: {
struct Cyc_Tcenv_Genv* _temp114= Cyc_Tcenv_lookup_namespace( te, loc, _temp111,
_temp109); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp114->structdecls, _temp88);}
_LL93:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp117; struct _tagged_arr* _temp118; void* _temp120; struct _tuple0* _temp115=
q; _temp117=* _temp115; _LL121: _temp120= _temp117.f1; goto _LL119; _LL119:
_temp118= _temp117.f2; goto _LL116; _LL116: { void* _temp122= _temp120; struct
Cyc_List_List* _temp132; struct Cyc_List_List* _temp134; struct Cyc_List_List*
_temp136; struct Cyc_List_List _temp138; struct Cyc_List_List* _temp139; struct
_tagged_arr* _temp141; _LL124: if( _temp122 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL125;} else{ goto _LL126;} _LL126: if(( unsigned int) _temp122 >  1u?*(( int*)
_temp122) ==  Cyc_Absyn_Rel_n: 0){ _LL133: _temp132=(( struct Cyc_Absyn_Rel_n_struct*)
_temp122)->f1; if( _temp132 ==  0){ goto _LL127;} else{ goto _LL128;}} else{
goto _LL128;} _LL128: if(( unsigned int) _temp122 >  1u?*(( int*) _temp122) == 
Cyc_Absyn_Abs_n: 0){ _LL135: _temp134=(( struct Cyc_Absyn_Abs_n_struct*)
_temp122)->f1; goto _LL129;} else{ goto _LL130;} _LL130: if(( unsigned int)
_temp122 >  1u?*(( int*) _temp122) ==  Cyc_Absyn_Rel_n: 0){ _LL137: _temp136=((
struct Cyc_Absyn_Rel_n_struct*) _temp122)->f1; if( _temp136 ==  0){ goto _LL123;}
else{ _temp138=* _temp136; _LL142: _temp141=( struct _tagged_arr*) _temp138.hd;
goto _LL140; _LL140: _temp139= _temp138.tl; goto _LL131;}} else{ goto _LL123;}
_LL125: goto _LL127; _LL127: return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp118);
_LL129: { struct Cyc_Tcenv_Genv* _temp143=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, _temp134);
return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( _temp143->uniondecls, _temp118);} _LL131: {
struct Cyc_Tcenv_Genv* _temp144= Cyc_Tcenv_lookup_namespace( te, loc, _temp141,
_temp139); return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp144->uniondecls, _temp118);}
_LL123:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp147; struct _tagged_arr* _temp148; void* _temp150; struct _tuple0* _temp145=
q; _temp147=* _temp145; _LL151: _temp150= _temp147.f1; goto _LL149; _LL149:
_temp148= _temp147.f2; goto _LL146; _LL146: { void* _temp152= _temp150; struct
Cyc_List_List* _temp162; struct Cyc_List_List* _temp164; struct Cyc_List_List
_temp166; struct Cyc_List_List* _temp167; struct _tagged_arr* _temp169; struct
Cyc_List_List* _temp171; _LL154: if( _temp152 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL155;} else{ goto _LL156;} _LL156: if(( unsigned int) _temp152 >  1u?*(( int*)
_temp152) ==  Cyc_Absyn_Rel_n: 0){ _LL163: _temp162=(( struct Cyc_Absyn_Rel_n_struct*)
_temp152)->f1; if( _temp162 ==  0){ goto _LL157;} else{ goto _LL158;}} else{
goto _LL158;} _LL158: if(( unsigned int) _temp152 >  1u?*(( int*) _temp152) == 
Cyc_Absyn_Rel_n: 0){ _LL165: _temp164=(( struct Cyc_Absyn_Rel_n_struct*)
_temp152)->f1; if( _temp164 ==  0){ goto _LL160;} else{ _temp166=* _temp164;
_LL170: _temp169=( struct _tagged_arr*) _temp166.hd; goto _LL168; _LL168:
_temp167= _temp166.tl; goto _LL159;}} else{ goto _LL160;} _LL160: if((
unsigned int) _temp152 >  1u?*(( int*) _temp152) ==  Cyc_Absyn_Abs_n: 0){ _LL172:
_temp171=(( struct Cyc_Absyn_Abs_n_struct*) _temp152)->f1; goto _LL161;} else{
goto _LL153;} _LL155: goto _LL157; _LL157: return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_tuniondecl_f, _temp148);
_LL159: { struct Cyc_Tcenv_Genv* _temp173= Cyc_Tcenv_lookup_namespace( te, loc,
_temp169, _temp167); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp173->tuniondecls, _temp148);}
_LL161: { struct Cyc_Tcenv_Genv* _temp174=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, _temp171);
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( _temp174->tuniondecls, _temp148);} _LL153:;}}
static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->tuniondecls, v);}
struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp177;
struct _tagged_arr* _temp178; void* _temp180; struct _tuple0* _temp175= q;
_temp177=* _temp175; _LL181: _temp180= _temp177.f1; goto _LL179; _LL179:
_temp178= _temp177.f2; goto _LL176; _LL176: { void* _temp182= _temp180; struct
Cyc_List_List* _temp192; struct Cyc_List_List* _temp194; struct Cyc_List_List
_temp196; struct Cyc_List_List* _temp197; struct _tagged_arr* _temp199; struct
Cyc_List_List* _temp201; _LL184: if(( unsigned int) _temp182 >  1u?*(( int*)
_temp182) ==  Cyc_Absyn_Rel_n: 0){ _LL193: _temp192=(( struct Cyc_Absyn_Rel_n_struct*)
_temp182)->f1; if( _temp192 ==  0){ goto _LL185;} else{ goto _LL186;}} else{
goto _LL186;} _LL186: if( _temp182 == ( void*) Cyc_Absyn_Loc_n){ goto _LL187;}
else{ goto _LL188;} _LL188: if(( unsigned int) _temp182 >  1u?*(( int*) _temp182)
==  Cyc_Absyn_Rel_n: 0){ _LL195: _temp194=(( struct Cyc_Absyn_Rel_n_struct*)
_temp182)->f1; if( _temp194 ==  0){ goto _LL190;} else{ _temp196=* _temp194;
_LL200: _temp199=( struct _tagged_arr*) _temp196.hd; goto _LL198; _LL198:
_temp197= _temp196.tl; goto _LL189;}} else{ goto _LL190;} _LL190: if((
unsigned int) _temp182 >  1u?*(( int*) _temp182) ==  Cyc_Absyn_Abs_n: 0){ _LL202:
_temp201=(( struct Cyc_Absyn_Abs_n_struct*) _temp182)->f1; goto _LL191;} else{
goto _LL183;} _LL185: { struct _handler_cons _temp203; _push_handler(& _temp203);{
int _temp205= 0; if( setjmp( _temp203.handler)){ _temp205= 1;} if( ! _temp205){{
struct Cyc_Core_Opt* _temp207=({ struct Cyc_Core_Opt* _temp206=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp206->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp178);
_temp206;}); _npop_handler( 0u); return _temp207;}; _pop_handler();} else{ void*
_temp204=( void*) _exn_thrown; void* _temp209= _temp204; _LL211: if( _temp209 == 
Cyc_Dict_Absent){ goto _LL212;} else{ goto _LL213;} _LL213: goto _LL214; _LL212:
return 0; _LL214:( void) _throw( _temp209); _LL210:;}}} _LL187:({ void* _temp215[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("lookup_xtuniondecl: impossible", sizeof(
unsigned char), 31u), _tag_arr( _temp215, sizeof( void*), 0u));}); return 0;
_LL189: { struct Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp216;
_push_handler(& _temp216);{ int _temp218= 0; if( setjmp( _temp216.handler)){
_temp218= 1;} if( ! _temp218){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp199,
_temp197);; _pop_handler();} else{ void* _temp217=( void*) _exn_thrown; void*
_temp220= _temp217; _LL222: if( _temp220 ==  Cyc_Dict_Absent){ goto _LL223;}
else{ goto _LL224;} _LL224: goto _LL225; _LL223:({ void* _temp226[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad qualified name for xtunion", sizeof( unsigned char), 31u),
_tag_arr( _temp226, sizeof( void*), 0u));});( int) _throw(( void*) Cyc_Dict_Absent);
_LL225:( void) _throw( _temp220); _LL221:;}}} return({ struct Cyc_Core_Opt*
_temp227=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp227->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->tuniondecls, _temp178); _temp227;});}
_LL191: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, _temp201); return({
struct Cyc_Core_Opt* _temp228=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp228->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp178); _temp228;});} _LL183:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp231; struct _tagged_arr* _temp232; void* _temp234; struct _tuple0* _temp229=
q; _temp231=* _temp229; _LL235: _temp234= _temp231.f1; goto _LL233; _LL233:
_temp232= _temp231.f2; goto _LL230; _LL230: { void* _temp236= _temp234; struct
Cyc_List_List* _temp246; struct Cyc_List_List* _temp248; struct Cyc_List_List
_temp250; struct Cyc_List_List* _temp251; struct _tagged_arr* _temp253; struct
Cyc_List_List* _temp255; _LL238: if( _temp236 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL239;} else{ goto _LL240;} _LL240: if(( unsigned int) _temp236 >  1u?*(( int*)
_temp236) ==  Cyc_Absyn_Rel_n: 0){ _LL247: _temp246=(( struct Cyc_Absyn_Rel_n_struct*)
_temp236)->f1; if( _temp246 ==  0){ goto _LL241;} else{ goto _LL242;}} else{
goto _LL242;} _LL242: if(( unsigned int) _temp236 >  1u?*(( int*) _temp236) == 
Cyc_Absyn_Rel_n: 0){ _LL249: _temp248=(( struct Cyc_Absyn_Rel_n_struct*)
_temp236)->f1; if( _temp248 ==  0){ goto _LL244;} else{ _temp250=* _temp248;
_LL254: _temp253=( struct _tagged_arr*) _temp250.hd; goto _LL252; _LL252:
_temp251= _temp250.tl; goto _LL243;}} else{ goto _LL244;} _LL244: if((
unsigned int) _temp236 >  1u?*(( int*) _temp236) ==  Cyc_Absyn_Abs_n: 0){ _LL256:
_temp255=(( struct Cyc_Absyn_Abs_n_struct*) _temp236)->f1; goto _LL245;} else{
goto _LL237;} _LL239: goto _LL241; _LL241: return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f, _temp232);
_LL243: { struct Cyc_Tcenv_Genv* _temp257= Cyc_Tcenv_lookup_namespace( te, loc,
_temp253, _temp251); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp257->enumdecls, _temp232);}
_LL245: { struct Cyc_Tcenv_Genv* _temp258=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, _temp255);
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( _temp258->enumdecls, _temp232);} _LL237:;}}
static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->typedefs, v);}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp261; struct _tagged_arr* _temp262; void* _temp264; struct _tuple0* _temp259=
q; _temp261=* _temp259; _LL265: _temp264= _temp261.f1; goto _LL263; _LL263:
_temp262= _temp261.f2; goto _LL260; _LL260: { void* _temp266= _temp264; struct
Cyc_List_List* _temp276; struct Cyc_List_List* _temp278; struct Cyc_List_List
_temp280; struct Cyc_List_List* _temp281; struct _tagged_arr* _temp283; struct
Cyc_List_List* _temp285; _LL268: if( _temp266 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL269;} else{ goto _LL270;} _LL270: if(( unsigned int) _temp266 >  1u?*(( int*)
_temp266) ==  Cyc_Absyn_Rel_n: 0){ _LL277: _temp276=(( struct Cyc_Absyn_Rel_n_struct*)
_temp266)->f1; if( _temp276 ==  0){ goto _LL271;} else{ goto _LL272;}} else{
goto _LL272;} _LL272: if(( unsigned int) _temp266 >  1u?*(( int*) _temp266) == 
Cyc_Absyn_Rel_n: 0){ _LL279: _temp278=(( struct Cyc_Absyn_Rel_n_struct*)
_temp266)->f1; if( _temp278 ==  0){ goto _LL274;} else{ _temp280=* _temp278;
_LL284: _temp283=( struct _tagged_arr*) _temp280.hd; goto _LL282; _LL282:
_temp281= _temp280.tl; goto _LL273;}} else{ goto _LL274;} _LL274: if((
unsigned int) _temp266 >  1u?*(( int*) _temp266) ==  Cyc_Absyn_Abs_n: 0){ _LL286:
_temp285=(( struct Cyc_Absyn_Abs_n_struct*) _temp266)->f1; goto _LL275;} else{
goto _LL267;} _LL269: goto _LL271; _LL271: return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_typedefdecl_f, _temp262);
_LL273: { struct Cyc_Tcenv_Genv* _temp287= Cyc_Tcenv_lookup_namespace( te, loc,
_temp283, _temp281); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp287->typedefs, _temp262);}
_LL275: { struct Cyc_Tcenv_Genv* _temp288=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, _temp285);
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( _temp288->typedefs, _temp262);} _LL267:;}}
struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp289=( struct Cyc_Tcenv_Genv*) _cycalloc( sizeof( struct Cyc_Tcenv_Genv));
_temp289->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp289->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp289->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp289->tuniondecls=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp289->enumdecls=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp289->typedefs=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp289->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp289->availables= 0; _temp289;});}
void* Cyc_Tcenv_get_visible( void* f){ while( 1) { void* _temp290= f; void*
_temp298; void* _temp300; void* _temp302; void* _temp304; _LL292: if(*(( int*)
_temp290) ==  Cyc_Tcenv_Outermost){ _LL299: _temp298=( void*)(( struct Cyc_Tcenv_Outermost_struct*)
_temp290)->f1; goto _LL293;} else{ goto _LL294;} _LL294: if(*(( int*) _temp290)
==  Cyc_Tcenv_Frame){ _LL301: _temp300=( void*)(( struct Cyc_Tcenv_Frame_struct*)
_temp290)->f1; goto _LL295;} else{ goto _LL296;} _LL296: if(*(( int*) _temp290)
==  Cyc_Tcenv_Hidden){ _LL305: _temp304=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp290)->f1; goto _LL303; _LL303: _temp302=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp290)->f2; goto _LL297;} else{ goto _LL291;} _LL293: return _temp298; _LL295:
return _temp300; _LL297: f= _temp302; goto _LL291; _LL291:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp306= f; void* _temp314; void* _temp316; void*
_temp318; void* _temp320; void* _temp322; _LL308: if(*(( int*) _temp306) ==  Cyc_Tcenv_Outermost){
_LL315: _temp314=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp306)->f1;
goto _LL309;} else{ goto _LL310;} _LL310: if(*(( int*) _temp306) ==  Cyc_Tcenv_Frame){
_LL319: _temp318=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp306)->f1; goto
_LL317; _LL317: _temp316=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp306)->f2;
goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp306) ==  Cyc_Tcenv_Hidden){
_LL323: _temp322=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp306)->f1; goto
_LL321; _LL321: _temp320=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp306)->f2;
goto _LL313;} else{ goto _LL307;} _LL309: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp324=( struct Cyc_Tcenv_Outermost_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp324[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp325; _temp325.tag= Cyc_Tcenv_Outermost;
_temp325.f1=( void*) x; _temp325;}); _temp324;}); _LL311: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp326=( struct Cyc_Tcenv_Frame_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp326[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp327; _temp327.tag= Cyc_Tcenv_Frame; _temp327.f1=( void*) x; _temp327.f2=(
void*) _temp316; _temp327;}); _temp326;}); _LL313: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp328=( struct Cyc_Tcenv_Hidden_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp328[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp329; _temp329.tag= Cyc_Tcenv_Hidden;
_temp329.f1=( void*) _temp322; _temp329.f2=( void*) Cyc_Tcenv_put_visible(
_temp320, x); _temp329;}); _temp328;}); _LL307:;} void* Cyc_Tcenv_apply_to_visible(
void*(* g)( void*), void* f){ return Cyc_Tcenv_put_visible( f, g( Cyc_Tcenv_get_visible(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp330=
f; void* _temp338; void* _temp340; void* _temp342; void* _temp344; void*
_temp346; _LL332: if(*(( int*) _temp330) ==  Cyc_Tcenv_Outermost){ _LL339:
_temp338=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp330)->f1; goto
_LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp330) ==  Cyc_Tcenv_Frame){
_LL343: _temp342=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp330)->f1; goto
_LL341; _LL341: _temp340=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp330)->f2;
goto _LL335;} else{ goto _LL336;} _LL336: if(*(( int*) _temp330) ==  Cyc_Tcenv_Hidden){
_LL347: _temp346=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp330)->f1; goto
_LL345; _LL345: _temp344=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp330)->f2;
goto _LL337;} else{ goto _LL331;} _LL333: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp348=( struct Cyc_Tcenv_Outermost_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp348[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp349; _temp349.tag= Cyc_Tcenv_Outermost;
_temp349.f1=( void*) g( _temp338); _temp349;}); _temp348;}); _LL335: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp350=( struct Cyc_Tcenv_Frame_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp350[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp351; _temp351.tag= Cyc_Tcenv_Frame; _temp351.f1=( void*) g( _temp342);
_temp351.f2=( void*) Cyc_Tcenv_map_frames( g, _temp340); _temp351;}); _temp350;});
_LL337: return( void*)({ struct Cyc_Tcenv_Hidden_struct* _temp352=( struct Cyc_Tcenv_Hidden_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_Hidden_struct)); _temp352[ 0]=({ struct Cyc_Tcenv_Hidden_struct
_temp353; _temp353.tag= Cyc_Tcenv_Hidden; _temp353.f1=( void*) g( _temp346);
_temp353.f2=( void*) Cyc_Tcenv_map_frames( g, _temp344); _temp353;}); _temp352;});
_LL331:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le ==  0){( int) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le ==  0){( int) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp354=( struct
Cyc_Tcenv_Tenv*) _cycalloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp354->ns= te->ns;
_temp354->ae= te->ae; _temp354->le=({ struct Cyc_Core_Opt* _temp355=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp355->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp355;}); _temp354;});} static int Cyc_Tcenv_region_ancestor(
struct Cyc_List_List* ancestors, struct Cyc_Absyn_Tvar* r){{ struct Cyc_List_List*
a= ancestors; for( 0; a !=  0; a= a->tl){ if( Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)
a->hd)->tvar, r) ==  0){ return 1;}}}{ struct Cyc_List_List* a= ancestors; for(
0; a !=  0; a= a->tl){ if( Cyc_Tcenv_region_ancestor((( struct Cyc_Tcenv_RgnPO*)
a->hd)->ancestors, r)){ return 1;}}} return 0;} static struct Cyc_Tcenv_RgnPO*
Cyc_Tcenv_find_region_in_po( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* r){
for( 0; po !=  0; po= po->tl){ if( Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)
po->hd)->tvar, r) ==  0){ return( struct Cyc_Tcenv_RgnPO*) po->hd;}} return((
struct Cyc_Tcenv_RgnPO*(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp357; _temp357.tag= Cyc_Std_String_pa;
_temp357.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp358=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp358[ 0]=({ struct Cyc_Absyn_VarType_struct _temp359; _temp359.tag= Cyc_Absyn_VarType;
_temp359.f1= r; _temp359;}); _temp358;}));{ void* _temp356[ 1u]={& _temp357};
Cyc_Std_aprintf( _tag_arr("can't find region variable %s in po", sizeof(
unsigned char), 36u), _tag_arr( _temp356, sizeof( void*), 1u));}}));} static int
Cyc_Tcenv_check_region_outlives( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar*
r1, struct Cyc_Absyn_Tvar* r2){ struct Cyc_List_List* r2_ancestors; while( po != 
0? Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*) po->hd)->tvar, r2) !=  0: 0) {
po= po->tl;} if( po ==  0){ return 0;} return Cyc_Tcenv_region_ancestor(((
struct Cyc_Tcenv_RgnPO*) po->hd)->ancestors, r1);} struct _tuple5{ void* f1;
void* f2; } ; static struct Cyc_List_List* Cyc_Tcenv_initial_region_po( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_List_List* po){
struct Cyc_List_List* result= 0; for( 0; tvs !=  0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar*
tv=( struct Cyc_Absyn_Tvar*) tvs->hd; if( Cyc_Tcutil_tvar_kind( tv) == ( void*)
Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp360=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp360->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp361=( struct Cyc_Tcenv_RgnPO*) _cycalloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp361->tvar= tv; _temp361->ancestors= 0; _temp361;}); _temp360->tl= result;
_temp360;});}} for( 0; po !=  0; po= po->tl){ struct _tuple5 _temp362=*(( struct
_tuple5*) po->hd); void* _temp370; void* _temp372; struct Cyc_Absyn_Tvar*
_temp374; void* _temp376; struct Cyc_Absyn_Tvar* _temp378; void* _temp380; void*
_temp382; _LL364: _LL371: _temp370= _temp362.f1; if( _temp370 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL365;} else{ goto _LL366;} _LL366: _LL377: _temp376= _temp362.f1; if((
unsigned int) _temp376 >  3u?*(( int*) _temp376) ==  Cyc_Absyn_VarType: 0){
_LL379: _temp378=(( struct Cyc_Absyn_VarType_struct*) _temp376)->f1; goto _LL373;}
else{ goto _LL368;} _LL373: _temp372= _temp362.f2; if(( unsigned int) _temp372 > 
3u?*(( int*) _temp372) ==  Cyc_Absyn_VarType: 0){ _LL375: _temp374=(( struct Cyc_Absyn_VarType_struct*)
_temp372)->f1; goto _LL367;} else{ goto _LL368;} _LL368: _LL383: _temp382=
_temp362.f1; goto _LL381; _LL381: _temp380= _temp362.f2; goto _LL369; _LL365:
goto _LL363; _LL367: if( Cyc_Absyn_tvar_cmp( _temp378, _temp374) ==  0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp374, _temp378)){({ struct Cyc_Std_String_pa_struct _temp386;
_temp386.tag= Cyc_Std_String_pa; _temp386.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp389=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp389[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp390; _temp390.tag= Cyc_Absyn_VarType; _temp390.f1= _temp374; _temp390;});
_temp389;}));{ struct Cyc_Std_String_pa_struct _temp385; _temp385.tag= Cyc_Std_String_pa;
_temp385.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp387=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp387[ 0]=({ struct Cyc_Absyn_VarType_struct _temp388; _temp388.tag= Cyc_Absyn_VarType;
_temp388.f1= _temp378; _temp388;}); _temp387;}));{ void* _temp384[ 2u]={&
_temp385,& _temp386}; Cyc_Tcutil_terr( loc, _tag_arr("bad region lifetime assertion (circularity):%s < %s",
sizeof( unsigned char), 52u), _tag_arr( _temp384, sizeof( void*), 2u));}}});}
else{ struct Cyc_Tcenv_RgnPO* _temp391= Cyc_Tcenv_find_region_in_po( result,
_temp378); struct Cyc_Tcenv_RgnPO* _temp392= Cyc_Tcenv_find_region_in_po( result,
_temp374); _temp392->ancestors=({ struct Cyc_List_List* _temp393=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp393->hd=( void*) _temp391;
_temp393->tl= _temp392->ancestors; _temp393;});} goto _LL363; _LL369:({ struct
Cyc_Std_String_pa_struct _temp396; _temp396.tag= Cyc_Std_String_pa; _temp396.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( _temp380);{ struct Cyc_Std_String_pa_struct
_temp395; _temp395.tag= Cyc_Std_String_pa; _temp395.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp382);{ void* _temp394[ 2u]={& _temp395,& _temp396}; Cyc_Tcutil_terr( loc,
_tag_arr("bad region lifetime assertion:: %s < %s", sizeof( unsigned char), 40u),
_tag_arr( _temp394, sizeof( void*), 2u));}}}); goto _LL363; _LL363:;} return
result;} static struct Cyc_List_List* Cyc_Tcenv_add_region_po( struct Cyc_List_List*
po, struct Cyc_Absyn_Tvar* tv){ return({ struct Cyc_List_List* _temp397=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp397->hd=( void*)({
struct Cyc_Tcenv_RgnPO* _temp398=( struct Cyc_Tcenv_RgnPO*) _cycalloc( sizeof(
struct Cyc_Tcenv_RgnPO)); _temp398->tvar= tv; _temp398->ancestors= po; _temp398;});
_temp397->tl= po; _temp397;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl(
struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp399=( struct Cyc_Tcenv_Fenv*)
_cycalloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp399->shared= f->shared;
_temp399->type_vars= f->type_vars; _temp399->region_order= f->region_order;
_temp399->locals= f->locals; _temp399->encloser= f->encloser; _temp399->ctrl_env=
f->ctrl_env; _temp399->capability=( void*)(( void*) f->capability); _temp399->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp399;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({
struct Cyc_Tcenv_Fenv* _temp400=( struct Cyc_Tcenv_Fenv*) _cycalloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp400->shared= f->shared; _temp400->type_vars= f->type_vars;
_temp400->region_order= f->region_order; _temp400->locals= f->locals; _temp400->encloser=
f->encloser; _temp400->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp401=( struct
Cyc_Tcenv_CtrlEnv*) _cycalloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp401->continue_stmt=(
void*)(( void*) c->continue_stmt); _temp401->break_stmt=( void*)(( void*) c->break_stmt);
_temp401->fallthru_clause= c->fallthru_clause; _temp401->next_stmt=( void*)((
void*) c->next_stmt); _temp401->try_depth= c->try_depth; _temp401;}); _temp400->capability=(
void*)(( void*) f->capability); _temp400->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp400;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected return_typ",
sizeof( unsigned char), 29u));} return( void*)(( Cyc_Tcenv_get_fenv( te))->shared)->return_typ;}
struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv* te){
if( te->le ==  0){ return 0;} return( Cyc_Tcenv_get_fenv( te))->type_vars;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le ==  0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_type_vars",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp402= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List*
_temp403=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp402->type_vars); Cyc_Tcutil_check_unique_tvars(
loc, _temp403); _temp402->type_vars= _temp403; return Cyc_Tcenv_put_fenv( te,
_temp402);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_local_var",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp404= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp405=(* vd->name).f2;{ void*
_temp406=(* vd->name).f1; _LL408: if( _temp406 == ( void*) Cyc_Absyn_Loc_n){
goto _LL409;} else{ goto _LL410;} _LL410: goto _LL411; _LL409: goto _LL407;
_LL411:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp412=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp412[ 0]=({ struct Cyc_Core_Impossible_struct _temp413; _temp413.tag= Cyc_Core_Impossible;
_temp413.f1= _tag_arr("add_local_var: called with Rel_n", sizeof( unsigned char),
33u); _temp413;}); _temp412;})); goto _LL407; _LL407:;} _temp404->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void*
v)) Cyc_Dict_insert)( _temp404->locals, _temp405,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp414=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp414[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp415; _temp415.tag= Cyc_Absyn_Local_b;
_temp415.f1= vd; _temp415;}); _temp414;})); return Cyc_Tcenv_put_fenv( te,
_temp404);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_pat_var",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* _temp416= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp417=(* vd->name).f2; _temp416->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void*
v)) Cyc_Dict_insert)( _temp416->locals, _temp417,( void*)({ struct Cyc_Absyn_Pat_b_struct*
_temp418=( struct Cyc_Absyn_Pat_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp418[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp419; _temp419.tag= Cyc_Absyn_Pat_b;
_temp419.f1= vd; _temp419;}); _temp418;})); return Cyc_Tcenv_put_fenv( te,
_temp416);}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp422; struct
_tagged_arr* _temp423; void* _temp425; struct _tuple0* _temp420= q; _temp422=*
_temp420; _LL426: _temp425= _temp422.f1; goto _LL424; _LL424: _temp423= _temp422.f2;
goto _LL421; _LL421: { void* _temp427= _temp425; struct Cyc_List_List* _temp436;
_LL429: if( _temp427 == ( void*) Cyc_Absyn_Loc_n){ goto _LL430;} else{ goto
_LL431;} _LL431: if(( unsigned int) _temp427 >  1u?*(( int*) _temp427) ==  Cyc_Absyn_Rel_n:
0){ _LL437: _temp436=(( struct Cyc_Absyn_Rel_n_struct*) _temp427)->f1; if(
_temp436 ==  0){ goto _LL435;} else{ goto _LL433;}} else{ goto _LL433;} _LL435:
if( te->le !=  0){ goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL430:
if( te->le ==  0){( int) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL432;}
_LL432: { struct Cyc_Tcenv_Fenv* _temp438= Cyc_Tcenv_get_fenv( te); struct
_handler_cons _temp439; _push_handler(& _temp439);{ int _temp441= 0; if( setjmp(
_temp439.handler)){ _temp441= 1;} if( ! _temp441){{ void* _temp444=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp442=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp442[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp443; _temp443.tag= Cyc_Tcenv_VarRes; _temp443.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp438->locals,
_temp423); _temp443;}); _temp442;}); _npop_handler( 0u); return _temp444;};
_pop_handler();} else{ void* _temp440=( void*) _exn_thrown; void* _temp446=
_temp440; _LL448: if( _temp446 ==  Cyc_Dict_Absent){ goto _LL449;} else{ goto
_LL450;} _LL450: goto _LL451; _LL449: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL451:( void) _throw( _temp446); _LL447:;}}} _LL434: { struct
_handler_cons _temp452; _push_handler(& _temp452);{ int _temp454= 0; if( setjmp(
_temp452.handler)){ _temp454= 1;} if( ! _temp454){{ void* _temp455= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp455;}; _pop_handler();} else{ void*
_temp453=( void*) _exn_thrown; void* _temp457= _temp453; _LL459: if( _temp457 == 
Cyc_Dict_Absent){ goto _LL460;} else{ goto _LL461;} _LL461: goto _LL462; _LL460:
return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp463=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp463[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp464; _temp464.tag= Cyc_Tcenv_VarRes; _temp464.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b);
_temp464;}); _temp463;}); _LL462:( void) _throw( _temp457); _LL458:;}}} _LL428:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le ==  0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_continue",
sizeof( unsigned char), 35u));}{ void* _temp465=( void*)(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->continue_stmt; struct Cyc_Absyn_Stmt* _temp475; _LL467: if((
unsigned int) _temp465 >  3u?*(( int*) _temp465) ==  Cyc_Tcenv_Stmt_j: 0){
_LL476: _temp475=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp465)->f1; goto _LL468;}
else{ goto _LL469;} _LL469: if( _temp465 == ( void*) Cyc_Tcenv_NotLoop_j){ goto
_LL470;} else{ goto _LL471;} _LL471: if( _temp465 == ( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL472;} else{ goto _LL473;} _LL473: if( _temp465 == ( void*) Cyc_Tcenv_FnEnd_j){
goto _LL474;} else{ goto _LL466;} _LL468: _temp475->non_local_preds=({ struct
Cyc_List_List* _temp477=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp477->hd=( void*) s; _temp477->tl= _temp475->non_local_preds; _temp477;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp475; return; _LL470:({ void* _temp478[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("continue not in a loop", sizeof(
unsigned char), 23u), _tag_arr( _temp478, sizeof( void*), 0u));}); return;
_LL472:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd",
sizeof( unsigned char), 32u)); return; _LL474:(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd", sizeof(
unsigned char), 32u)); return; _LL466:;}} void Cyc_Tcenv_process_break( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt** sopt){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_break",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); void* _temp479=( void*)( fe->ctrl_env)->break_stmt; struct Cyc_Absyn_Stmt*
_temp489; _LL481: if(( unsigned int) _temp479 >  3u?*(( int*) _temp479) ==  Cyc_Tcenv_Stmt_j:
0){ _LL490: _temp489=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp479)->f1; goto
_LL482;} else{ goto _LL483;} _LL483: if( _temp479 == ( void*) Cyc_Tcenv_NotLoop_j){
goto _LL484;} else{ goto _LL485;} _LL485: if( _temp479 == ( void*) Cyc_Tcenv_FnEnd_j){
goto _LL486;} else{ goto _LL487;} _LL487: if( _temp479 == ( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL488;} else{ goto _LL480;} _LL482: _temp489->non_local_preds=({ struct
Cyc_List_List* _temp491=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp491->hd=( void*) s; _temp491->tl= _temp489->non_local_preds; _temp491;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp489; return; _LL484:({ void* _temp492[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("break not in a loop or switch", sizeof(
unsigned char), 30u), _tag_arr( _temp492, sizeof( void*), 0u));}); return;
_LL486: if(( void*)( fe->shared)->return_typ != ( void*) Cyc_Absyn_VoidType){({
void* _temp493[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("break causes function not to return a value",
sizeof( unsigned char), 44u), _tag_arr( _temp493, sizeof( void*), 0u));});}
return; _LL488:({ void* _temp494[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof( unsigned char), 56u), _tag_arr( _temp494, sizeof( void*), 0u));});
return; _LL480:;}} void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, struct _tagged_arr* l, struct Cyc_Absyn_Stmt** sopt){
if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_goto", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te); struct Cyc_Core_Opt*
_temp495=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l); if( _temp495 ==  0){
struct Cyc_SlowDict_Dict* _temp496=( fe->shared)->needed_labels; struct Cyc_Core_Opt*
_temp497=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct
_tagged_arr* k)) Cyc_SlowDict_lookup_opt)( _temp496, l); if( _temp497 ==  0){
_temp497=({ struct Cyc_Core_Opt* _temp498=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp498->v=( void*) 0; _temp498;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_arr* k,
struct Cyc_List_List* v)) Cyc_SlowDict_insert)( _temp496, l,({ struct Cyc_List_List*
_temp499=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp499->hd=( void*) s; _temp499->tl=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp497))->v; _temp499;}));} else{(( struct Cyc_Absyn_Stmt*)
_temp495->v)->non_local_preds=({ struct Cyc_List_List* _temp500=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp500->hd=( void*) s; _temp500->tl=((
struct Cyc_Absyn_Stmt*) _temp495->v)->non_local_preds; _temp500;});* sopt=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Stmt*) _temp495->v);}}} struct
_tuple3* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Switch_clause*** clauseopt){ if( te->le ==  0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_fallthru",
sizeof( unsigned char), 35u));}{ struct Cyc_Tcenv_Fenv* _temp501= Cyc_Tcenv_get_fenv(
te); struct _tuple3* _temp502=( _temp501->ctrl_env)->fallthru_clause; if(
_temp502 !=  0){(((* _temp502).f1)->body)->non_local_preds=({ struct Cyc_List_List*
_temp503=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp503->hd=( void*) s; _temp503->tl=(((* _temp502).f1)->body)->non_local_preds;
_temp503;});* clauseopt=( struct Cyc_Absyn_Switch_clause**)({ struct Cyc_Absyn_Switch_clause**
_temp504=( struct Cyc_Absyn_Switch_clause**) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause*)
*  1); _temp504[ 0]=(* _temp502).f1; _temp504;});} return _temp502;}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs,
struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause* clause){ if( te->le
==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_fallthru",
sizeof( unsigned char), 31u));}{ struct Cyc_List_List* _temp505= 0; for( 0; vds
!=  0; vds= vds->tl){ _temp505=({ struct Cyc_List_List* _temp506=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp506->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*) vds->hd)->type); _temp506->tl= _temp505; _temp506;});}
_temp505= Cyc_List_imp_rev( _temp505);{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({ struct _tuple3*
_temp507=( struct _tuple3*) _cycalloc( sizeof( struct _tuple3)); _temp507->f1=
clause; _temp507->f2= new_tvs; _temp507->f3= _temp505; _temp507;}); return Cyc_Tcenv_put_fenv(
te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*
te){ if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected clear_fallthru", sizeof( unsigned char), 33u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
fe->ctrl_env)->fallthru_clause= 0; return Cyc_Tcenv_put_fenv( te, fe);}} struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest){ if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_in_loop", sizeof( unsigned char), 30u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp508=( struct Cyc_Tcenv_Stmt_j_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp508[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp509; _temp509.tag= Cyc_Tcenv_Stmt_j;
_temp509.f1= continue_dest; _temp509;}); _temp508;})));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp510=( struct Cyc_Tcenv_Stmt_j_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp510[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp511; _temp511.tag= Cyc_Tcenv_Stmt_j; _temp511.f1= continue_dest; _temp511;});
_temp510;}))); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te){ if( te->le ==  0){(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected enter_try", sizeof(
unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth; return Cyc_Tcenv_put_fenv(
te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv* te){ if( te->le
==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_try_depth",
sizeof( unsigned char), 32u));} return(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->try_depth;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv* te){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_switch",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)(
fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)
Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_next",
sizeof( unsigned char), 27u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=( void*) j); return
Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_label",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* _temp512= Cyc_Tcenv_get_fenv(
te); struct Cyc_SlowDict_Dict* _temp513=( _temp512->shared)->needed_labels;
struct Cyc_Core_Opt* _temp514=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* k)) Cyc_SlowDict_lookup_opt)( _temp513, v); if( _temp514
!=  0){( _temp512->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* k)) Cyc_SlowDict_delete)(
_temp513, v);{ struct Cyc_List_List* _temp515=( struct Cyc_List_List*) _temp514->v;
s->non_local_preds= _temp515; for( 0; _temp515 !=  0; _temp515= _temp515->tl){
void* _temp516=( void*)(( struct Cyc_Absyn_Stmt*) _temp515->hd)->r; struct Cyc_Absyn_Stmt*
_temp522; struct Cyc_Absyn_Stmt** _temp524; _LL518: if(( unsigned int) _temp516
>  1u?*(( int*) _temp516) ==  Cyc_Absyn_Goto_s: 0){ _LL523: _temp522=(( struct
Cyc_Absyn_Goto_s_struct*) _temp516)->f2; _temp524=( struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Goto_s_struct*) _temp516)->f2; goto _LL519;} else{ goto _LL520;}
_LL520: goto _LL521; _LL519:* _temp524=( struct Cyc_Absyn_Stmt*) s; goto _LL517;
_LL521:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: add_label backpatching of non-goto",
sizeof( unsigned char), 42u)); goto _LL517; _LL517:;}}} if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_member)(( _temp512->shared)->seen_labels, v)){({
struct Cyc_Std_String_pa_struct _temp526; _temp526.tag= Cyc_Std_String_pa;
_temp526.f1=( struct _tagged_arr)* v;{ void* _temp525[ 1u]={& _temp526}; Cyc_Tcutil_terr(
s->loc, _tag_arr("Repeated label: %s", sizeof( unsigned char), 19u), _tag_arr(
_temp525, sizeof( void*), 1u));}});}( _temp512->shared)->seen_labels=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Stmt*
v)) Cyc_Dict_insert)(( _temp512->shared)->seen_labels, v, s); return te;}} int
Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv* te){ return(( int(*)(
struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)((( Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);}
struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv* te){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_encloser",
sizeof( unsigned char), 31u));} return( Cyc_Tcenv_get_fenv( te))->encloser;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s){ if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_encloser", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));
fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::add_region: unexpected add_region",
sizeof( unsigned char), 41u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar* tv;{ void* _temp527= Cyc_Tcutil_compress(
r); struct Cyc_Absyn_Tvar* _temp533; _LL529: if(( unsigned int) _temp527 >  3u?*((
int*) _temp527) ==  Cyc_Absyn_VarType: 0){ _LL534: _temp533=(( struct Cyc_Absyn_VarType_struct*)
_temp527)->f1; goto _LL530;} else{ goto _LL531;} _LL531: goto _LL532; _LL530: tv=
_temp533; goto _LL528; _LL532: tv=(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("bad add region", sizeof(
unsigned char), 15u)); goto _LL528; _LL528:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp535=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp535[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp536; _temp536.tag= Cyc_Absyn_JoinEff;
_temp536.f1=({ struct Cyc_List_List* _temp537=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp537->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp539=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp539[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp540; _temp540.tag= Cyc_Absyn_AccessEff;
_temp540.f1=( void*) r; _temp540;}); _temp539;})); _temp537->tl=({ struct Cyc_List_List*
_temp538=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp538->hd=( void*)(( void*) fe->capability); _temp538->tl= 0; _temp538;});
_temp537;}); _temp536;}); _temp535;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le == 
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected new_block",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp548=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp548[ 0]=({ struct Cyc_Absyn_VarType_struct _temp549; _temp549.tag= Cyc_Absyn_VarType;
_temp549.f1= block_rgn; _temp549;}); _temp548;}); fe->type_vars=({ struct Cyc_List_List*
_temp541=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp541->hd=( void*) block_rgn; _temp541->tl= fe->type_vars; _temp541;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp542=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp542[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp543; _temp543.tag= Cyc_Absyn_JoinEff;
_temp543.f1=({ struct Cyc_List_List* _temp544=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp544->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp546=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp546[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp547; _temp547.tag= Cyc_Absyn_AccessEff;
_temp547.f1=( void*) block_typ; _temp547;}); _temp546;})); _temp544->tl=({
struct Cyc_List_List* _temp545=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp545->hd=( void*)(( void*) fe->capability); _temp545->tl=
0; _temp545;}); _temp544;}); _temp543;}); _temp542;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
struct Cyc_Absyn_Tvar* t= Cyc_Tcutil_new_tvar(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp550=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp550[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp551; _temp551.tag= Cyc_Absyn_Eq_kb;
_temp551.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp551;}); _temp550;})); Cyc_Tcutil_add_tvar_identity(
t); return Cyc_Tcenv_new_named_block( loc, te, t);} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le ==  0){ return( void*) Cyc_Absyn_HeapRgn;}
return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!=  0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}}({ struct Cyc_Std_String_pa_struct _temp553; _temp553.tag= Cyc_Std_String_pa;
_temp553.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( rgn);{ void* _temp552[
1u]={& _temp553}; Cyc_Tcutil_terr( loc, _tag_arr("Expression accesses unavailable region %s",
sizeof( unsigned char), 42u), _tag_arr( _temp552, sizeof( void*), 1u));}});} int
Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){
rt_a= Cyc_Tcutil_compress( rt_a); rt_b= Cyc_Tcutil_compress( rt_b); if( rt_a == (
void*) Cyc_Absyn_HeapRgn){ return 1;} if( te->le !=  0){ struct _tuple5 _temp555=({
struct _tuple5 _temp554; _temp554.f1= rt_a; _temp554.f2= rt_b; _temp554;}); void*
_temp561; struct Cyc_Absyn_Tvar* _temp563; void* _temp565; struct Cyc_Absyn_Tvar*
_temp567; _LL557: _LL566: _temp565= _temp555.f1; if(( unsigned int) _temp565 > 
3u?*(( int*) _temp565) ==  Cyc_Absyn_VarType: 0){ _LL568: _temp567=(( struct Cyc_Absyn_VarType_struct*)
_temp565)->f1; goto _LL562;} else{ goto _LL559;} _LL562: _temp561= _temp555.f2;
if(( unsigned int) _temp561 >  3u?*(( int*) _temp561) ==  Cyc_Absyn_VarType: 0){
_LL564: _temp563=(( struct Cyc_Absyn_VarType_struct*) _temp561)->f1; goto _LL558;}
else{ goto _LL559;} _LL559: goto _LL560; _LL558: return Cyc_Tcenv_check_region_outlives((
Cyc_Tcenv_get_fenv( te))->region_order, _temp567, _temp563); _LL560: return 0;
_LL556:;} return 0;} void Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* eff){ void* capability; if( te->le
==  0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp569=( struct
Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp569[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp570; _temp570.tag= Cyc_Absyn_JoinEff;
_temp570.f1= 0; _temp570;}); _temp569;});} else{ struct Cyc_Tcenv_Fenv* _temp571=
Cyc_Tcenv_get_fenv( te); capability=( void*) _temp571->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){({ struct Cyc_Std_String_pa_struct _temp574; _temp574.tag=
Cyc_Std_String_pa; _temp574.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( eff);{
struct Cyc_Std_String_pa_struct _temp573; _temp573.tag= Cyc_Std_String_pa;
_temp573.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( capability);{ void*
_temp572[ 2u]={& _temp573,& _temp574}; Cyc_Tcutil_terr( loc, _tag_arr("Capability %s does not cover function's effect %s",
sizeof( unsigned char), 50u), _tag_arr( _temp572, sizeof( void*), 2u));}}});}}
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po){ for( 0; po !=  0; po= po->tl){ struct _tuple5
_temp577; void* _temp578; void* _temp580; struct _tuple5* _temp575=( struct
_tuple5*) po->hd; _temp577=* _temp575; _LL581: _temp580= _temp577.f1; goto
_LL579; _LL579: _temp578= _temp577.f2; goto _LL576; _LL576: if( ! Cyc_Tcenv_region_outlives(
te, _temp580, _temp578)){({ struct Cyc_Std_String_pa_struct _temp584; _temp584.tag=
Cyc_Std_String_pa; _temp584.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp578);{ struct Cyc_Std_String_pa_struct _temp583; _temp583.tag= Cyc_Std_String_pa;
_temp583.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp580);{ void*
_temp582[ 2u]={& _temp583,& _temp584}; Cyc_Tcutil_terr( loc, _tag_arr("function requires that region %s outlive %s",
sizeof( unsigned char), 44u), _tag_arr( _temp582, sizeof( void*), 2u));}}});}}}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar* _temp632=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp632->name=({ struct _tagged_arr*
_temp635=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp635[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp637;
_temp637.tag= Cyc_Std_String_pa; _temp637.f1=( struct _tagged_arr)*(* fd->name).f2;{
void* _temp636[ 1u]={& _temp637}; Cyc_Std_aprintf( _tag_arr("`%s", sizeof(
unsigned char), 4u), _tag_arr( _temp636, sizeof( void*), 1u));}}); _temp635;});
_temp632->identity= Cyc_Tcutil_new_tvar_id(); _temp632->kind=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp633=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp633[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp634; _temp634.tag= Cyc_Absyn_Eq_kb; _temp634.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp634;}); _temp633;})); _temp632;}); struct Cyc_List_List* _temp585=({ struct
Cyc_List_List* _temp631=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp631->hd=( void*) rgn0; _temp631->tl= fd->tvs; _temp631;}); Cyc_Tcutil_check_unique_tvars(
loc, _temp585);{ struct Cyc_List_List* _temp586= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); void* param_rgn=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp629=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp629[ 0]=({ struct Cyc_Absyn_VarType_struct _temp630; _temp630.tag= Cyc_Absyn_VarType;
_temp630.f1= rgn0; _temp630;}); _temp629;}); struct Cyc_List_List* _temp587= 0;{
struct Cyc_List_List* _temp588= fd->args; for( 0; _temp588 !=  0; _temp588=
_temp588->tl){ struct Cyc_Absyn_Vardecl* _temp589=({ struct Cyc_Absyn_Vardecl*
_temp594=( struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl)
*  1); _temp594[ 0]=({ struct Cyc_Absyn_Vardecl _temp595; _temp595.sc=( void*)((
void*) Cyc_Absyn_Public); _temp595.name=({ struct _tuple0* _temp597=( struct
_tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp597->f1=( void*) Cyc_Absyn_Loc_n;
_temp597->f2=(*(( struct _tuple6*) _temp588->hd)).f1; _temp597;}); _temp595.tq=(*((
struct _tuple6*) _temp588->hd)).f2; _temp595.type=( void*)(*(( struct _tuple6*)
_temp588->hd)).f3; _temp595.initializer= 0; _temp595.rgn=({ struct Cyc_Core_Opt*
_temp596=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp596->v=( void*) param_rgn; _temp596;}); _temp595.attributes= 0; _temp595;});
_temp594;}); _temp587=({ struct Cyc_List_List* _temp590=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp590[ 0]=({ struct Cyc_List_List
_temp591; _temp591.hd=( void*) _temp589; _temp591.tl= _temp587; _temp591;});
_temp590;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k, void* v)) Cyc_Dict_insert)( locals,(*(( struct _tuple6*)
_temp588->hd)).f1,( void*)({ struct Cyc_Absyn_Param_b_struct* _temp592=( struct
Cyc_Absyn_Param_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp592[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp593; _temp593.tag= Cyc_Absyn_Param_b;
_temp593.f1= _temp589; _temp593;}); _temp592;}));}} if( fd->cyc_varargs !=  0){
int _temp600; void* _temp602; struct Cyc_Absyn_Tqual _temp604; struct Cyc_Core_Opt*
_temp606; struct Cyc_Absyn_VarargInfo _temp598=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( fd->cyc_varargs)); _LL607: _temp606= _temp598.name; goto _LL605;
_LL605: _temp604= _temp598.tq; goto _LL603; _LL603: _temp602=( void*) _temp598.type;
goto _LL601; _LL601: _temp600= _temp598.inject; goto _LL599; _LL599: if(
_temp606 !=  0){ void* _temp608= Cyc_Absyn_tagged_typ( _temp602, param_rgn,
_temp604); struct Cyc_Absyn_Vardecl* _temp609=({ struct Cyc_Absyn_Vardecl*
_temp613=( struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl)
*  1); _temp613[ 0]=({ struct Cyc_Absyn_Vardecl _temp614; _temp614.sc=( void*)((
void*) Cyc_Absyn_Public); _temp614.name=({ struct _tuple0* _temp616=( struct
_tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp616->f1=( void*) Cyc_Absyn_Loc_n;
_temp616->f2=( struct _tagged_arr*) _temp606->v; _temp616;}); _temp614.tq= Cyc_Absyn_empty_tqual();
_temp614.type=( void*) _temp608; _temp614.initializer= 0; _temp614.rgn=({ struct
Cyc_Core_Opt* _temp615=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp615->v=( void*) param_rgn; _temp615;}); _temp614.attributes= 0; _temp614;});
_temp613;}); _temp587=({ struct Cyc_List_List* _temp610=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp610->hd=( void*) _temp609;
_temp610->tl= _temp587; _temp610;}); locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( locals,(
struct _tagged_arr*) _temp606->v,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp611=( struct Cyc_Absyn_Param_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp611[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp612; _temp612.tag= Cyc_Absyn_Param_b;
_temp612.f1= _temp609; _temp612;}); _temp611;}));} else{({ void* _temp617[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("missing name for varargs", sizeof( unsigned char),
25u), _tag_arr( _temp617, sizeof( void*), 0u));});}} fd->param_vardecls=({
struct Cyc_Core_Opt* _temp618=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt) *  1); _temp618[ 0]=({ struct Cyc_Core_Opt _temp619; _temp619.v=(
void*) _temp587; _temp619;}); _temp618;}); return({ struct Cyc_Tcenv_Fenv*
_temp620=( struct Cyc_Tcenv_Fenv*) _cycalloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp620->shared=({ struct Cyc_Tcenv_SharedFenv* _temp628=( struct Cyc_Tcenv_SharedFenv*)
_cycalloc( sizeof( struct Cyc_Tcenv_SharedFenv)); _temp628->return_typ=( void*)((
void*) fd->ret_type); _temp628->seen_labels=(( struct Cyc_Dict_Dict*(*)( int(*
cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp628->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_SlowDict_empty)( Cyc_Std_zstrptrcmp);
_temp628;}); _temp620->type_vars= _temp585; _temp620->region_order= _temp586;
_temp620->locals= locals; _temp620->encloser= fd->body; _temp620->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp627=( struct Cyc_Tcenv_CtrlEnv*) _cycalloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp627->continue_stmt=( void*)(( void*)
Cyc_Tcenv_NotLoop_j); _temp627->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp627->fallthru_clause= 0; _temp627->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp627->try_depth= 0; _temp627;}); _temp620->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp621=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp621[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp622; _temp622.tag= Cyc_Absyn_JoinEff; _temp622.f1=({ struct Cyc_List_List*
_temp623=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp623->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp625=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp625[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp626; _temp626.tag= Cyc_Absyn_AccessEff;
_temp626.f1=( void*) param_rgn; _temp626;}); _temp625;})); _temp623->tl=({
struct Cyc_List_List* _temp624=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp624->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp624->tl= 0; _temp624;}); _temp623;});
_temp622;}); _temp621;})); _temp620->curr_rgn=( void*) param_rgn; _temp620;});}}
