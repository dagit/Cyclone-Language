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
extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_fflush( struct
Cyc_Std___sFILE*); extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_vrprintf(
struct _RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static
const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int tag;
short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int
Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y); extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern void*
Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_exists_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct
Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct _RegionHandle* r2,
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_rsplit( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x); extern
int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l, void*
x); extern void* Cyc_List_assoc_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l, void* k); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List*); static const int Cyc_Position_Lex= 0; static const int
Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl** f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 25; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
26; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 27; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 28; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 29;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 30; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_sint_t; extern void*
Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_empty_effect; extern struct
_tuple1* Cyc_Absyn_tunion_print_arg_qvar; extern struct _tuple1* Cyc_Absyn_tunion_scanf_arg_qvar;
extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_atb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq, void* b); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct _tagged_arr*); extern
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*); struct _tuple4{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple4* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*);
extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*, void* r1, void* r2); extern int Cyc_Std_strcmp( struct
_tagged_arr s1, struct _tagged_arr s2); extern int Cyc_Std_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_copy_type(
void* t); extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern
void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress( void* t);
extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*); extern int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern
void Cyc_Tcutil_explain_failure(); extern int Cyc_Tcutil_unify( void*, void*);
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*); extern void*
Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*, void*);
extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void* e2);
extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e); extern
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_Position_Segment*);
struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5*
Cyc_Tcutil_make_inst_var( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*); struct
_tuple6{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; extern struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc, struct
Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn); extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr err_msg); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*, unsigned int i, struct Cyc_Absyn_Conref*); extern
int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*,
void* t, struct Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern
struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int* Cyc_Tcutil_new_tvar_id();
extern void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*); extern void
Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_snd_tqt(
struct _tuple4*); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
unsigned char Cyc_Tcutil_TypeErr[ 12u]="\000\000\000\000TypeErr"; extern void
Cyc_Tcutil_unify_it( void* t1, void* t2); void* Cyc_Tcutil_t1_failure=( void*) 0u;
void* Cyc_Tcutil_t2_failure=( void*) 0u; struct _tagged_arr Cyc_Tcutil_failure_reason=(
struct _tagged_arr){( void*) 0u,( void*) 0u,( void*)( 0u +  0u)}; void Cyc_Tcutil_explain_failure(){
Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);{ struct _tagged_arr
s1= Cyc_Absynpp_typ2string( Cyc_Tcutil_t1_failure); struct _tagged_arr s2= Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure); int pos= 8;({ struct Cyc_Std_String_pa_struct _temp1;
_temp1.tag= Cyc_Std_String_pa; _temp1.f1=( struct _tagged_arr) s1;{ void* _temp0[
1u]={& _temp1}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\t%s and ", sizeof(
unsigned char), 9u), _tag_arr( _temp0, sizeof( void*), 1u));}}); pos +=
_get_arr_size( s1, sizeof( unsigned char)) +  5; if( pos >=  80){({ void* _temp2[
0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n\t", sizeof( unsigned char),
3u), _tag_arr( _temp2, sizeof( void*), 0u));}); pos= 8;}({ struct Cyc_Std_String_pa_struct
_temp4; _temp4.tag= Cyc_Std_String_pa; _temp4.f1=( struct _tagged_arr) s2;{ void*
_temp3[ 1u]={& _temp4}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s ", sizeof(
unsigned char), 4u), _tag_arr( _temp3, sizeof( void*), 1u));}}); pos +=
_get_arr_size( s2, sizeof( unsigned char)) +  1; if( pos >=  80){({ void* _temp5[
0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n\t", sizeof( unsigned char),
3u), _tag_arr( _temp5, sizeof( void*), 0u));}); pos= 8;}({ void* _temp6[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("failed to unify. ", sizeof(
unsigned char), 18u), _tag_arr( _temp6, sizeof( void*), 0u));}); pos += 17; if(
Cyc_Tcutil_failure_reason.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
if( pos >=  80){({ void* _temp7[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\n\t", sizeof( unsigned char), 3u), _tag_arr( _temp7, sizeof( void*),
0u));}); pos= 8;}({ struct Cyc_Std_String_pa_struct _temp9; _temp9.tag= Cyc_Std_String_pa;
_temp9.f1=( struct _tagged_arr) Cyc_Tcutil_failure_reason;{ void* _temp8[ 1u]={&
_temp9}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp8, sizeof( void*), 1u));}});}({ void* _temp10[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp10,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}}
void Cyc_Tcutil_terr( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap){ Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap)));} void*
Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap){ struct
_tagged_arr msg=( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap);({ struct Cyc_Std_String_pa_struct _temp12; _temp12.tag= Cyc_Std_String_pa;
_temp12.f1=( struct _tagged_arr) msg;{ void* _temp11[ 1u]={& _temp12}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp11, sizeof( void*), 1u));}}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);(
int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){({ struct Cyc_Std_String_pa_struct _temp15; _temp15.tag=
Cyc_Std_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);{
struct Cyc_Std_String_pa_struct _temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=(
struct _tagged_arr) Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd);{ void* _temp13[ 2u]={& _temp14,&
_temp15}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::%s ", sizeof(
unsigned char), 8u), _tag_arr( _temp13, sizeof( void*), 2u));}}});}({ void*
_temp16[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp16, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs=
0; static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_arr fmt, struct _tagged_arr ap){
struct _tagged_arr msg=( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap); Cyc_Tcutil_warning_segs=({ struct Cyc_List_List* _temp17=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp17->hd=( void*) sg; _temp17->tl=
Cyc_Tcutil_warning_segs; _temp17;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List*
_temp18=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp18->hd=( void*)({ struct _tagged_arr* _temp19=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp19[ 0]= msg; _temp19;}); _temp18->tl=
Cyc_Tcutil_warning_msgs; _temp18;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs
==  0){ return;}({ void* _temp20[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("***Warnings***\n", sizeof( unsigned char), 16u), _tag_arr( _temp20,
sizeof( void*), 0u));});{ struct Cyc_List_List* _temp21= Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs);
while( Cyc_Tcutil_warning_msgs !=  0) {({ struct Cyc_Std_String_pa_struct
_temp24; _temp24.tag= Cyc_Std_String_pa; _temp24.f1=( struct _tagged_arr)*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Std_String_pa_struct _temp23; _temp23.tag= Cyc_Std_String_pa; _temp23.f1=(
struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
_temp21))->hd);{ void* _temp22[ 2u]={& _temp23,& _temp24}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp22, sizeof(
void*), 2u));}}}); _temp21=(( struct Cyc_List_List*) _check_null( _temp21))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}({
void* _temp25[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("**************\n",
sizeof( unsigned char), 16u), _tag_arr( _temp25, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) - *(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp26= t; struct Cyc_Core_Opt*
_temp38; struct Cyc_Core_Opt* _temp40; struct Cyc_Core_Opt** _temp42; struct Cyc_Core_Opt*
_temp43; struct Cyc_Core_Opt* _temp45; struct Cyc_Core_Opt** _temp47; _LL28: if((
unsigned int) _temp26 >  4u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL39:
_temp38=(( struct Cyc_Absyn_Evar_struct*) _temp26)->f2; if( _temp38 ==  0){ goto
_LL29;} else{ goto _LL30;}} else{ goto _LL30;} _LL30: if(( unsigned int) _temp26
>  4u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL41: _temp40=(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; _temp42=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; goto _LL31;} else{ goto _LL32;} _LL32: if(( unsigned int) _temp26
>  4u?*(( int*) _temp26) ==  Cyc_Absyn_TypedefType: 0){ _LL44: _temp43=(( struct
Cyc_Absyn_TypedefType_struct*) _temp26)->f3; if( _temp43 ==  0){ goto _LL33;}
else{ goto _LL34;}} else{ goto _LL34;} _LL34: if(( unsigned int) _temp26 >  4u?*((
int*) _temp26) ==  Cyc_Absyn_TypedefType: 0){ _LL46: _temp45=(( struct Cyc_Absyn_TypedefType_struct*)
_temp26)->f3; _temp47=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp26)->f3; goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37; _LL29: return t;
_LL31: { void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp42))->v); if( t2 != ( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp42))->v){* _temp42=({ struct Cyc_Core_Opt* _temp48=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp48->v=( void*) t2;
_temp48;});} return t2;} _LL33: return t; _LL35: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp47))->v); if( t2 != ( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp47))->v){* _temp47=({ struct Cyc_Core_Opt*
_temp49=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp49->v=( void*) t2; _temp49;});} return t2;} _LL37: return t; _LL27:;} void*
Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp50=( void*) c->v; void* _temp58; struct Cyc_Absyn_Conref* _temp60;
_LL52: if( _temp50 == ( void*) Cyc_Absyn_No_constr){ goto _LL53;} else{ goto
_LL54;} _LL54: if(( unsigned int) _temp50 >  1u?*(( int*) _temp50) ==  Cyc_Absyn_Eq_constr:
0){ _LL59: _temp58=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp50)->f1;
goto _LL55;} else{ goto _LL56;} _LL56: if(( unsigned int) _temp50 >  1u?*(( int*)
_temp50) ==  Cyc_Absyn_Forward_constr: 0){ _LL61: _temp60=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp50)->f1; goto _LL57;} else{ goto _LL51;} _LL53: return Cyc_Absyn_empty_conref();
_LL55: return Cyc_Absyn_new_conref( _temp58); _LL57: return Cyc_Tcutil_copy_conref(
_temp60); _LL51:;} static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_Absyn_Tvar* _temp62=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp62->name= tv->name; _temp62->identity=
0; _temp62->kind= Cyc_Tcutil_copy_conref( tv->kind); _temp62;});} static struct
_tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp65; struct Cyc_Absyn_Tqual
_temp67; struct Cyc_Core_Opt* _temp69; struct _tuple2 _temp63=* arg; _LL70:
_temp69= _temp63.f1; goto _LL68; _LL68: _temp67= _temp63.f2; goto _LL66; _LL66:
_temp65= _temp63.f3; goto _LL64; _LL64: return({ struct _tuple2* _temp71=(
struct _tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp71->f1= _temp69;
_temp71->f2= _temp67; _temp71->f3= Cyc_Tcutil_copy_type( _temp65); _temp71;});}
static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){ void* _temp74;
struct Cyc_Absyn_Tqual _temp76; struct _tuple4 _temp72=* arg; _LL77: _temp76=
_temp72.f1; goto _LL75; _LL75: _temp74= _temp72.f2; goto _LL73; _LL73: return({
struct _tuple4* _temp78=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp78->f1= _temp76; _temp78->f2= Cyc_Tcutil_copy_type( _temp74); _temp78;});}
static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field( struct Cyc_Absyn_Structfield*
f){ return({ struct Cyc_Absyn_Structfield* _temp79=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp79->name= f->name;
_temp79->tq= f->tq; _temp79->type=( void*) Cyc_Tcutil_copy_type(( void*) f->type);
_temp79->width= f->width; _temp79->attributes= f->attributes; _temp79;});}
struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp82; void* _temp83; void* _temp85; struct
_tuple8* _temp80= x; _temp82=* _temp80; _LL86: _temp85= _temp82.f1; goto _LL84;
_LL84: _temp83= _temp82.f2; goto _LL81; _LL81: return({ struct _tuple8* _temp87=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp87->f1= Cyc_Tcutil_copy_type(
_temp85); _temp87->f2= Cyc_Tcutil_copy_type( _temp83); _temp87;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp88= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp136; struct Cyc_Absyn_TunionInfo _temp138; void* _temp140; struct Cyc_List_List*
_temp142; void* _temp144; struct Cyc_Absyn_TunionFieldInfo _temp146; struct Cyc_List_List*
_temp148; void* _temp150; struct Cyc_Absyn_PtrInfo _temp152; struct Cyc_Absyn_Conref*
_temp154; struct Cyc_Absyn_Tqual _temp156; struct Cyc_Absyn_Conref* _temp158;
void* _temp160; void* _temp162; void* _temp164; void* _temp166; struct Cyc_Absyn_Exp*
_temp168; struct Cyc_Absyn_Tqual _temp170; void* _temp172; struct Cyc_Absyn_FnInfo
_temp174; struct Cyc_List_List* _temp176; struct Cyc_List_List* _temp178; struct
Cyc_Absyn_VarargInfo* _temp180; int _temp182; struct Cyc_List_List* _temp184;
void* _temp186; struct Cyc_Core_Opt* _temp188; struct Cyc_List_List* _temp190;
struct Cyc_List_List* _temp192; struct Cyc_List_List* _temp194; struct _tuple1*
_temp196; struct Cyc_List_List* _temp198; struct _tuple1* _temp200; struct Cyc_List_List*
_temp202; struct Cyc_List_List* _temp204; struct _tuple1* _temp206; void*
_temp208; struct Cyc_List_List* _temp210; struct _tuple1* _temp212; void*
_temp214; struct Cyc_List_List* _temp216; void* _temp218; _LL90: if( _temp88 == (
void*) Cyc_Absyn_VoidType){ goto _LL91;} else{ goto _LL92;} _LL92: if((
unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_Evar: 0){ goto
_LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp88 >  4u?*(( int*)
_temp88) ==  Cyc_Absyn_VarType: 0){ _LL137: _temp136=(( struct Cyc_Absyn_VarType_struct*)
_temp88)->f1; goto _LL95;} else{ goto _LL96;} _LL96: if(( unsigned int) _temp88
>  4u?*(( int*) _temp88) ==  Cyc_Absyn_TunionType: 0){ _LL139: _temp138=((
struct Cyc_Absyn_TunionType_struct*) _temp88)->f1; _LL145: _temp144=( void*)
_temp138.tunion_info; goto _LL143; _LL143: _temp142= _temp138.targs; goto _LL141;
_LL141: _temp140=( void*) _temp138.rgn; goto _LL97;} else{ goto _LL98;} _LL98:
if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_TunionFieldType:
0){ _LL147: _temp146=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp88)->f1;
_LL151: _temp150=( void*) _temp146.field_info; goto _LL149; _LL149: _temp148=
_temp146.targs; goto _LL99;} else{ goto _LL100;} _LL100: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_PointerType: 0){ _LL153: _temp152=((
struct Cyc_Absyn_PointerType_struct*) _temp88)->f1; _LL163: _temp162=( void*)
_temp152.elt_typ; goto _LL161; _LL161: _temp160=( void*) _temp152.rgn_typ; goto
_LL159; _LL159: _temp158= _temp152.nullable; goto _LL157; _LL157: _temp156=
_temp152.tq; goto _LL155; _LL155: _temp154= _temp152.bounds; goto _LL101;} else{
goto _LL102;} _LL102: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) == 
Cyc_Absyn_IntType: 0){ _LL167: _temp166=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp88)->f1; goto _LL165; _LL165: _temp164=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp88)->f2; goto _LL103;} else{ goto _LL104;} _LL104: if( _temp88 == ( void*)
Cyc_Absyn_FloatType){ goto _LL105;} else{ goto _LL106;} _LL106: if( _temp88 == (
void*) Cyc_Absyn_DoubleType){ goto _LL107;} else{ goto _LL108;} _LL108: if((
unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_ArrayType: 0){
_LL173: _temp172=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f1;
goto _LL171; _LL171: _temp170=(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f2;
goto _LL169; _LL169: _temp168=(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f3;
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp88 >  4u?*((
int*) _temp88) ==  Cyc_Absyn_FnType: 0){ _LL175: _temp174=(( struct Cyc_Absyn_FnType_struct*)
_temp88)->f1; _LL191: _temp190= _temp174.tvars; goto _LL189; _LL189: _temp188=
_temp174.effect; goto _LL187; _LL187: _temp186=( void*) _temp174.ret_typ; goto
_LL185; _LL185: _temp184= _temp174.args; goto _LL183; _LL183: _temp182= _temp174.c_varargs;
goto _LL181; _LL181: _temp180= _temp174.cyc_varargs; goto _LL179; _LL179:
_temp178= _temp174.rgn_po; goto _LL177; _LL177: _temp176= _temp174.attributes;
goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp88 >  4u?*((
int*) _temp88) ==  Cyc_Absyn_TupleType: 0){ _LL193: _temp192=(( struct Cyc_Absyn_TupleType_struct*)
_temp88)->f1; goto _LL113;} else{ goto _LL114;} _LL114: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_StructType: 0){ _LL197: _temp196=((
struct Cyc_Absyn_StructType_struct*) _temp88)->f1; goto _LL195; _LL195: _temp194=((
struct Cyc_Absyn_StructType_struct*) _temp88)->f2; goto _LL115;} else{ goto
_LL116;} _LL116: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_UnionType:
0){ _LL201: _temp200=(( struct Cyc_Absyn_UnionType_struct*) _temp88)->f1; goto
_LL199; _LL199: _temp198=(( struct Cyc_Absyn_UnionType_struct*) _temp88)->f2;
goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int) _temp88 >  4u?*((
int*) _temp88) ==  Cyc_Absyn_AnonStructType: 0){ _LL203: _temp202=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp88)->f1; goto _LL119;} else{ goto _LL120;} _LL120: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_AnonUnionType: 0){ _LL205:
_temp204=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp88)->f1; goto _LL121;}
else{ goto _LL122;} _LL122: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88)
==  Cyc_Absyn_EnumType: 0){ _LL207: _temp206=(( struct Cyc_Absyn_EnumType_struct*)
_temp88)->f1; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_RgnHandleType: 0){ _LL209:
_temp208=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp88)->f1; goto
_LL125;} else{ goto _LL126;} _LL126: if(( unsigned int) _temp88 >  4u?*(( int*)
_temp88) ==  Cyc_Absyn_TypedefType: 0){ _LL213: _temp212=(( struct Cyc_Absyn_TypedefType_struct*)
_temp88)->f1; goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_TypedefType_struct*)
_temp88)->f2; goto _LL127;} else{ goto _LL128;} _LL128: if( _temp88 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL129;} else{ goto _LL130;} _LL130: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_AccessEff: 0){ _LL215: _temp214=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp88)->f1; goto _LL131;} else{
goto _LL132;} _LL132: if(( unsigned int) _temp88 >  4u?*(( int*) _temp88) == 
Cyc_Absyn_JoinEff: 0){ _LL217: _temp216=(( struct Cyc_Absyn_JoinEff_struct*)
_temp88)->f1; goto _LL133;} else{ goto _LL134;} _LL134: if(( unsigned int)
_temp88 >  4u?*(( int*) _temp88) ==  Cyc_Absyn_RgnsEff: 0){ _LL219: _temp218=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp88)->f1; goto _LL135;} else{
goto _LL89;} _LL91: goto _LL93; _LL93: return t; _LL95: return( void*)({ struct
Cyc_Absyn_VarType_struct* _temp220=( struct Cyc_Absyn_VarType_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp220[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp221; _temp221.tag= Cyc_Absyn_VarType; _temp221.f1= Cyc_Tcutil_copy_tvar(
_temp136); _temp221;}); _temp220;}); _LL97: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp222=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp222[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp223; _temp223.tag= Cyc_Absyn_TunionType;
_temp223.f1=({ struct Cyc_Absyn_TunionInfo _temp224; _temp224.tunion_info=( void*)
_temp144; _temp224.targs= Cyc_Tcutil_copy_types( _temp142); _temp224.rgn=( void*)
Cyc_Tcutil_copy_type( _temp140); _temp224;}); _temp223;}); _temp222;}); _LL99:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp225=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp225[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp226; _temp226.tag= Cyc_Absyn_TunionFieldType;
_temp226.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp227; _temp227.field_info=(
void*) _temp150; _temp227.targs= Cyc_Tcutil_copy_types( _temp148); _temp227;});
_temp226;}); _temp225;}); _LL101: { void* _temp228= Cyc_Tcutil_copy_type(
_temp162); void* _temp229= Cyc_Tcutil_copy_type( _temp160); struct Cyc_Absyn_Conref*
_temp230=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp158); struct Cyc_Absyn_Tqual _temp231= _temp156; struct Cyc_Absyn_Conref*
_temp232= Cyc_Tcutil_copy_conref( _temp154); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp233=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp234; _temp234.tag= Cyc_Absyn_PointerType;
_temp234.f1=({ struct Cyc_Absyn_PtrInfo _temp235; _temp235.elt_typ=( void*)
_temp228; _temp235.rgn_typ=( void*) _temp229; _temp235.nullable= _temp230;
_temp235.tq= _temp231; _temp235.bounds= _temp232; _temp235;}); _temp234;});
_temp233;});} _LL103: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp236=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp236[ 0]=({ struct Cyc_Absyn_IntType_struct _temp237; _temp237.tag= Cyc_Absyn_IntType;
_temp237.f1=( void*) _temp166; _temp237.f2=( void*) _temp164; _temp237;});
_temp236;}); _LL105: goto _LL107; _LL107: return t; _LL109: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp238=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp238[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp239; _temp239.tag= Cyc_Absyn_ArrayType; _temp239.f1=(
void*) Cyc_Tcutil_copy_type( _temp172); _temp239.f2= _temp170; _temp239.f3=
_temp168; _temp239;}); _temp238;}); _LL111: { struct Cyc_List_List* _temp240=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp190); struct
Cyc_Core_Opt* _temp241= _temp188 ==  0? 0:({ struct Cyc_Core_Opt* _temp251=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp251->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp188))->v); _temp251;}); void* _temp242= Cyc_Tcutil_copy_type( _temp186);
struct Cyc_List_List* _temp243=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp184); int _temp244= _temp182; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp180 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp180); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp245=(
struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp245->name= cv->name; _temp245->tq= cv->tq; _temp245->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp245->inject= cv->inject; _temp245;});}{ struct Cyc_List_List*
_temp246=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp178);
struct Cyc_List_List* _temp247= _temp176; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp248=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp248[ 0]=({ struct Cyc_Absyn_FnType_struct _temp249; _temp249.tag= Cyc_Absyn_FnType;
_temp249.f1=({ struct Cyc_Absyn_FnInfo _temp250; _temp250.tvars= _temp240;
_temp250.effect= _temp241; _temp250.ret_typ=( void*) _temp242; _temp250.args=
_temp243; _temp250.c_varargs= _temp244; _temp250.cyc_varargs= cyc_varargs2;
_temp250.rgn_po= _temp246; _temp250.attributes= _temp247; _temp250;}); _temp249;});
_temp248;});}} _LL113: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp252=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp252[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp253; _temp253.tag= Cyc_Absyn_TupleType;
_temp253.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp192);
_temp253;}); _temp252;}); _LL115: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp254=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp254[ 0]=({ struct Cyc_Absyn_StructType_struct _temp255; _temp255.tag= Cyc_Absyn_StructType;
_temp255.f1= _temp196; _temp255.f2= Cyc_Tcutil_copy_types( _temp194); _temp255.f3=
0; _temp255;}); _temp254;}); _LL117: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp256=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp256[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp257; _temp257.tag= Cyc_Absyn_UnionType;
_temp257.f1= _temp200; _temp257.f2= Cyc_Tcutil_copy_types( _temp198); _temp257.f3=
0; _temp257;}); _temp256;}); _LL119: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp258=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp258[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp259; _temp259.tag= Cyc_Absyn_AnonStructType; _temp259.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp202); _temp259;}); _temp258;});
_LL121: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp260=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp260[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp261; _temp261.tag=
Cyc_Absyn_AnonUnionType; _temp261.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp204); _temp261;}); _temp260;}); _LL123: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp262=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp262[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp263; _temp263.tag= Cyc_Absyn_EnumType;
_temp263.f1= _temp206; _temp263.f2= 0; _temp263;}); _temp262;}); _LL125: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp264=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp264[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp265; _temp265.tag= Cyc_Absyn_RgnHandleType;
_temp265.f1=( void*) Cyc_Tcutil_copy_type( _temp208); _temp265;}); _temp264;});
_LL127: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp266=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp266[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp267; _temp267.tag= Cyc_Absyn_TypedefType;
_temp267.f1= _temp212; _temp267.f2= Cyc_Tcutil_copy_types( _temp210); _temp267.f3=
0; _temp267;}); _temp266;}); _LL129: return t; _LL131: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp268=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp268[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp269; _temp269.tag= Cyc_Absyn_AccessEff; _temp269.f1=(
void*) Cyc_Tcutil_copy_type( _temp214); _temp269;}); _temp268;}); _LL133: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp270=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp270[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp271; _temp271.tag= Cyc_Absyn_JoinEff; _temp271.f1= Cyc_Tcutil_copy_types(
_temp216); _temp271;}); _temp270;}); _LL135: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp272=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp272[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp273; _temp273.tag= Cyc_Absyn_RgnsEff;
_temp273.f1=( void*) Cyc_Tcutil_copy_type( _temp218); _temp273;}); _temp272;});
_LL89:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){
return 1;}{ struct _tuple8 _temp275=({ struct _tuple8 _temp274; _temp274.f1= k1;
_temp274.f2= k2; _temp274;}); void* _temp285; void* _temp287; void* _temp289;
void* _temp291; void* _temp293; void* _temp295; _LL277: _LL288: _temp287=
_temp275.f1; if( _temp287 == ( void*) Cyc_Absyn_BoxKind){ goto _LL286;} else{
goto _LL279;} _LL286: _temp285= _temp275.f2; if( _temp285 == ( void*) Cyc_Absyn_MemKind){
goto _LL278;} else{ goto _LL279;} _LL279: _LL292: _temp291= _temp275.f1; if(
_temp291 == ( void*) Cyc_Absyn_BoxKind){ goto _LL290;} else{ goto _LL281;}
_LL290: _temp289= _temp275.f2; if( _temp289 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL280;} else{ goto _LL281;} _LL281: _LL296: _temp295= _temp275.f1; if( _temp295
== ( void*) Cyc_Absyn_MemKind){ goto _LL294;} else{ goto _LL283;} _LL294:
_temp293= _temp275.f2; if( _temp293 == ( void*) Cyc_Absyn_AnyKind){ goto _LL282;}
else{ goto _LL283;} _LL283: goto _LL284; _LL278: goto _LL280; _LL280: goto
_LL282; _LL282: return 1; _LL284: return 0; _LL276:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp297= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp351;
struct Cyc_Core_Opt* _temp353; struct Cyc_Absyn_Tvar* _temp355; void* _temp357;
struct Cyc_Absyn_TunionFieldInfo _temp359; void* _temp361; struct Cyc_Absyn_Tunionfield*
_temp363; struct Cyc_Absyn_TunionFieldInfo _temp365; void* _temp367; struct Cyc_Absyn_Structdecl**
_temp369; struct Cyc_Absyn_Uniondecl** _temp371; struct Cyc_Absyn_PtrInfo
_temp373; struct Cyc_Core_Opt* _temp375; _LL299: if(( unsigned int) _temp297 > 
4u?*(( int*) _temp297) ==  Cyc_Absyn_Evar: 0){ _LL354: _temp353=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f1; goto _LL352; _LL352: _temp351=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f2; goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_VarType: 0){ _LL356: _temp355=((
struct Cyc_Absyn_VarType_struct*) _temp297)->f1; goto _LL302;} else{ goto _LL303;}
_LL303: if( _temp297 == ( void*) Cyc_Absyn_VoidType){ goto _LL304;} else{ goto
_LL305;} _LL305: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_IntType:
0){ _LL358: _temp357=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp297)->f2;
goto _LL306;} else{ goto _LL307;} _LL307: if( _temp297 == ( void*) Cyc_Absyn_FloatType){
goto _LL308;} else{ goto _LL309;} _LL309: if( _temp297 == ( void*) Cyc_Absyn_DoubleType){
goto _LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_FnType: 0){ goto _LL312;} else{ goto _LL313;}
_LL313: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL314;} else{ goto _LL315;} _LL315: if( _temp297 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL316;} else{ goto _LL317;} _LL317: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_TunionType: 0){ goto _LL318;} else{ goto _LL319;}
_LL319: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_TunionFieldType:
0){ _LL360: _temp359=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp297)->f1;
_LL362: _temp361=( void*) _temp359.field_info; if(*(( int*) _temp361) ==  Cyc_Absyn_KnownTunionfield){
_LL364: _temp363=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp361)->f2;
goto _LL320;} else{ goto _LL321;}} else{ goto _LL321;} _LL321: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_TunionFieldType: 0){ _LL366:
_temp365=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp297)->f1; _LL368:
_temp367=( void*) _temp365.field_info; if(*(( int*) _temp367) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL322;} else{ goto _LL323;}} else{ goto _LL323;} _LL323: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_StructType: 0){ _LL370:
_temp369=(( struct Cyc_Absyn_StructType_struct*) _temp297)->f3; if( _temp369 == 
0){ goto _LL324;} else{ goto _LL325;}} else{ goto _LL325;} _LL325: if((
unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_UnionType: 0){
_LL372: _temp371=(( struct Cyc_Absyn_UnionType_struct*) _temp297)->f3; if(
_temp371 ==  0){ goto _LL326;} else{ goto _LL327;}} else{ goto _LL327;} _LL327:
if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_StructType:
0){ goto _LL328;} else{ goto _LL329;} _LL329: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_UnionType: 0){ goto _LL330;} else{ goto _LL331;}
_LL331: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL332;} else{ goto _LL333;} _LL333: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL334;} else{ goto _LL335;}
_LL335: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_EnumType:
0){ goto _LL336;} else{ goto _LL337;} _LL337: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_PointerType: 0){ _LL374: _temp373=(( struct Cyc_Absyn_PointerType_struct*)
_temp297)->f1; goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int)
_temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_ArrayType: 0){ goto _LL340;}
else{ goto _LL341;} _LL341: if(( unsigned int) _temp297 >  4u?*(( int*) _temp297)
==  Cyc_Absyn_TupleType: 0){ goto _LL342;} else{ goto _LL343;} _LL343: if((
unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_TypedefType: 0){
_LL376: _temp375=(( struct Cyc_Absyn_TypedefType_struct*) _temp297)->f3; goto
_LL344;} else{ goto _LL345;} _LL345: if(( unsigned int) _temp297 >  4u?*(( int*)
_temp297) ==  Cyc_Absyn_AccessEff: 0){ goto _LL346;} else{ goto _LL347;} _LL347:
if(( unsigned int) _temp297 >  4u?*(( int*) _temp297) ==  Cyc_Absyn_JoinEff: 0){
goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp297 >  4u?*((
int*) _temp297) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL350;} else{ goto _LL298;}
_LL300: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp353))->v;
_LL302: return Cyc_Absyn_conref_val( _temp355->kind); _LL304: return( void*) Cyc_Absyn_MemKind;
_LL306: return _temp357 == ( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL308: goto _LL310; _LL310: goto _LL312; _LL312:
return( void*) Cyc_Absyn_MemKind; _LL314: return( void*) Cyc_Absyn_BoxKind;
_LL316: return( void*) Cyc_Absyn_RgnKind; _LL318: return( void*) Cyc_Absyn_BoxKind;
_LL320: if( _temp363->typs ==  0){ return( void*) Cyc_Absyn_BoxKind;} else{
return( void*) Cyc_Absyn_MemKind;} _LL322: return({ void* _temp377[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("typ_kind: Unresolved TunionFieldType", sizeof( unsigned char), 37u),
_tag_arr( _temp377, sizeof( void*), 0u));}); _LL324: goto _LL326; _LL326: return(
void*) Cyc_Absyn_AnyKind; _LL328: goto _LL330; _LL330: goto _LL332; _LL332: goto
_LL334; _LL334: return( void*) Cyc_Absyn_MemKind; _LL336: return( void*) Cyc_Absyn_BoxKind;
_LL338: { void* _temp378=( void*)( Cyc_Absyn_compress_conref( _temp373.bounds))->v;
void* _temp388; void* _temp390; _LL380: if(( unsigned int) _temp378 >  1u?*((
int*) _temp378) ==  Cyc_Absyn_Eq_constr: 0){ _LL389: _temp388=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp378)->f1; if( _temp388 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL381;} else{ goto _LL382;}} else{ goto _LL382;} _LL382: if(( unsigned int)
_temp378 >  1u?*(( int*) _temp378) ==  Cyc_Absyn_Eq_constr: 0){ _LL391: _temp390=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp378)->f1; if(( unsigned int)
_temp390 >  1u?*(( int*) _temp390) ==  Cyc_Absyn_Upper_b: 0){ goto _LL383;}
else{ goto _LL384;}} else{ goto _LL384;} _LL384: if( _temp378 == ( void*) Cyc_Absyn_No_constr){
goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp378 >  1u?*((
int*) _temp378) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL387;} else{ goto
_LL379;} _LL381: return( void*) Cyc_Absyn_MemKind; _LL383: return( void*) Cyc_Absyn_BoxKind;
_LL385: return( void*) Cyc_Absyn_MemKind; _LL387: return({ void* _temp392[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds", sizeof(
unsigned char), 43u), _tag_arr( _temp392, sizeof( void*), 0u));}); _LL379:;}
_LL340: goto _LL342; _LL342: return( void*) Cyc_Absyn_MemKind; _LL344: return({
struct Cyc_Std_String_pa_struct _temp394; _temp394.tag= Cyc_Std_String_pa;
_temp394.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp393[ 1u]={&
_temp394}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp393, sizeof( void*), 1u));}}); _LL346: goto
_LL348; _LL348: goto _LL350; _LL350: return( void*) Cyc_Absyn_EffKind; _LL298:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp395; _push_handler(& _temp395);{
int _temp397= 0; if( setjmp( _temp395.handler)){ _temp397= 1;} if( ! _temp397){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp398= 1; _npop_handler( 0u); return
_temp398;}; _pop_handler();} else{ void* _temp396=( void*) _exn_thrown; void*
_temp400= _temp396; _LL402: if( _temp400 ==  Cyc_Tcutil_Unify){ goto _LL403;}
else{ goto _LL404;} _LL404: goto _LL405; _LL403: return 0; _LL405:( void) _throw(
_temp400); _LL401:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp406= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp442; struct Cyc_Core_Opt* _temp444; struct Cyc_Core_Opt**
_temp446; struct Cyc_Core_Opt* _temp447; struct Cyc_Absyn_PtrInfo _temp449; void*
_temp451; struct Cyc_Absyn_FnInfo _temp453; struct Cyc_List_List* _temp455;
struct Cyc_List_List* _temp457; struct Cyc_Absyn_VarargInfo* _temp459; int
_temp461; struct Cyc_List_List* _temp463; void* _temp465; struct Cyc_Core_Opt*
_temp467; struct Cyc_List_List* _temp469; struct Cyc_List_List* _temp471; void*
_temp473; struct Cyc_Absyn_TunionInfo _temp475; void* _temp477; struct Cyc_List_List*
_temp479; struct Cyc_Core_Opt* _temp481; struct Cyc_List_List* _temp483; struct
Cyc_Absyn_TunionFieldInfo _temp485; struct Cyc_List_List* _temp487; struct Cyc_List_List*
_temp489; struct Cyc_List_List* _temp491; struct Cyc_List_List* _temp493; void*
_temp495; struct Cyc_List_List* _temp497; void* _temp499; _LL408: if((
unsigned int) _temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_VarType: 0){
_LL443: _temp442=(( struct Cyc_Absyn_VarType_struct*) _temp406)->f1; goto _LL409;}
else{ goto _LL410;} _LL410: if(( unsigned int) _temp406 >  4u?*(( int*) _temp406)
==  Cyc_Absyn_Evar: 0){ _LL448: _temp447=(( struct Cyc_Absyn_Evar_struct*)
_temp406)->f2; goto _LL445; _LL445: _temp444=(( struct Cyc_Absyn_Evar_struct*)
_temp406)->f4; _temp446=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp406)->f4; goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int)
_temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_PointerType: 0){ _LL450:
_temp449=(( struct Cyc_Absyn_PointerType_struct*) _temp406)->f1; goto _LL413;}
else{ goto _LL414;} _LL414: if(( unsigned int) _temp406 >  4u?*(( int*) _temp406)
==  Cyc_Absyn_ArrayType: 0){ _LL452: _temp451=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp406)->f1; goto _LL415;} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_FnType: 0){ _LL454: _temp453=((
struct Cyc_Absyn_FnType_struct*) _temp406)->f1; _LL470: _temp469= _temp453.tvars;
goto _LL468; _LL468: _temp467= _temp453.effect; goto _LL466; _LL466: _temp465=(
void*) _temp453.ret_typ; goto _LL464; _LL464: _temp463= _temp453.args; goto
_LL462; _LL462: _temp461= _temp453.c_varargs; goto _LL460; _LL460: _temp459=
_temp453.cyc_varargs; goto _LL458; _LL458: _temp457= _temp453.rgn_po; goto
_LL456; _LL456: _temp455= _temp453.attributes; goto _LL417;} else{ goto _LL418;}
_LL418: if(( unsigned int) _temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_TupleType:
0){ _LL472: _temp471=(( struct Cyc_Absyn_TupleType_struct*) _temp406)->f1; goto
_LL419;} else{ goto _LL420;} _LL420: if(( unsigned int) _temp406 >  4u?*(( int*)
_temp406) ==  Cyc_Absyn_RgnHandleType: 0){ _LL474: _temp473=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp406)->f1; goto _LL421;} else{ goto _LL422;}
_LL422: if(( unsigned int) _temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_TunionType:
0){ _LL476: _temp475=(( struct Cyc_Absyn_TunionType_struct*) _temp406)->f1;
_LL480: _temp479= _temp475.targs; goto _LL478; _LL478: _temp477=( void*)
_temp475.rgn; goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int)
_temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_TypedefType: 0){ _LL484:
_temp483=(( struct Cyc_Absyn_TypedefType_struct*) _temp406)->f2; goto _LL482;
_LL482: _temp481=(( struct Cyc_Absyn_TypedefType_struct*) _temp406)->f3; goto
_LL425;} else{ goto _LL426;} _LL426: if(( unsigned int) _temp406 >  4u?*(( int*)
_temp406) ==  Cyc_Absyn_TunionFieldType: 0){ _LL486: _temp485=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp406)->f1; _LL488: _temp487= _temp485.targs; goto _LL427;} else{ goto _LL428;}
_LL428: if(( unsigned int) _temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_StructType:
0){ _LL490: _temp489=(( struct Cyc_Absyn_StructType_struct*) _temp406)->f2; goto
_LL429;} else{ goto _LL430;} _LL430: if(( unsigned int) _temp406 >  4u?*(( int*)
_temp406) ==  Cyc_Absyn_AnonStructType: 0){ _LL492: _temp491=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp406)->f1; goto _LL431;} else{ goto _LL432;} _LL432: if(( unsigned int)
_temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_AnonUnionType: 0){ _LL494:
_temp493=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp406)->f1; goto _LL433;}
else{ goto _LL434;} _LL434: if(( unsigned int) _temp406 >  4u?*(( int*) _temp406)
==  Cyc_Absyn_AccessEff: 0){ _LL496: _temp495=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp406)->f1; goto _LL435;} else{ goto _LL436;} _LL436: if(( unsigned int)
_temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_JoinEff: 0){ _LL498: _temp497=((
struct Cyc_Absyn_JoinEff_struct*) _temp406)->f1; goto _LL437;} else{ goto _LL438;}
_LL438: if(( unsigned int) _temp406 >  4u?*(( int*) _temp406) ==  Cyc_Absyn_RgnsEff:
0){ _LL500: _temp499=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp406)->f1;
goto _LL439;} else{ goto _LL440;} _LL440: goto _LL441; _LL409: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp442)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);} goto
_LL407; _LL411: if( t ==  evar){ Cyc_Tcutil_failure_reason= _tag_arr("(occurs check)",
sizeof( unsigned char), 15u);( int) _throw(( void*) Cyc_Tcutil_Unify);} else{
if( _temp447 !=  0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp447))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp446))->v; for(
0; s !=  0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp501= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp446))->v;
for( 0; s !=  0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp501=({ struct Cyc_List_List* _temp502=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp502->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp502->tl= _temp501; _temp502;});}}}*
_temp446=({ struct Cyc_Core_Opt* _temp503=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp503->v=( void*) _temp501; _temp503;});}}}
goto _LL407; _LL413: Cyc_Tcutil_occurs( evar, env,( void*) _temp449.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp449.rgn_typ); goto _LL407; _LL415:
Cyc_Tcutil_occurs( evar, env, _temp451); goto _LL407; _LL417: if( _temp467 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp467))->v);} Cyc_Tcutil_occurs( evar, env, _temp465); for( 0; _temp463 !=  0;
_temp463=(( struct Cyc_List_List*) _check_null( _temp463))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp463))->hd)).f3);}
if( _temp459 !=  0){ void* _temp506; struct Cyc_Absyn_VarargInfo _temp504=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp459)); _LL507: _temp506=( void*)
_temp504.type; goto _LL505; _LL505: Cyc_Tcutil_occurs( evar, env, _temp506);}
for( 0; _temp457 !=  0; _temp457=(( struct Cyc_List_List*) _check_null( _temp457))->tl){
struct _tuple8 _temp510; void* _temp511; void* _temp513; struct _tuple8*
_temp508=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp457))->hd;
_temp510=* _temp508; _LL514: _temp513= _temp510.f1; goto _LL512; _LL512:
_temp511= _temp510.f2; goto _LL509; _LL509: Cyc_Tcutil_occurs( evar, env,
_temp513); Cyc_Tcutil_occurs( evar, env, _temp511);} goto _LL407; _LL419: for( 0;
_temp471 !=  0; _temp471=(( struct Cyc_List_List*) _check_null( _temp471))->tl){
Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp471))->hd)).f2);} goto _LL407; _LL421: Cyc_Tcutil_occurs( evar,
env, _temp473); goto _LL407; _LL423: Cyc_Tcutil_occurs( evar, env, _temp477);
Cyc_Tcutil_occurslist( evar, env, _temp479); goto _LL407; _LL425: _temp487=
_temp483; goto _LL427; _LL427: _temp489= _temp487; goto _LL429; _LL429: Cyc_Tcutil_occurslist(
evar, env, _temp489); goto _LL407; _LL431: _temp493= _temp491; goto _LL433;
_LL433: for( 0; _temp493 !=  0; _temp493=(( struct Cyc_List_List*) _check_null(
_temp493))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp493))->hd)->type);} goto _LL407; _LL435:
Cyc_Tcutil_occurs( evar, env, _temp495); goto _LL407; _LL437: Cyc_Tcutil_occurslist(
evar, env, _temp497); goto _LL407; _LL439: Cyc_Tcutil_occurs( evar, env,
_temp499); goto _LL407; _LL441: goto _LL407; _LL407:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts != 
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ for( 0; t1 !=  0? t2 !=  0:
0;( t1=(( struct Cyc_List_List*) _check_null( t1))->tl, t2=(( struct Cyc_List_List*)
_check_null( t2))->tl)){ Cyc_Tcutil_unify_it(( void*)(( struct Cyc_List_List*)
_check_null( t1))->hd,( void*)(( struct Cyc_List_List*) _check_null( t2))->hd);}
if( t1 !=  0? 1: t2 !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);}} static
void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ if(( tq1.q_const !=  tq2.q_const? 1: tq1.q_volatile !=  tq2.q_volatile)? 1:
tq1.q_restrict !=  tq2.q_restrict){ Cyc_Tcutil_failure_reason= _tag_arr("(qualifiers don't match)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == 
tq2.q_const? tq1.q_volatile ==  tq2.q_volatile: 0)? tq1.q_restrict ==  tq2.q_restrict:
0;} static void Cyc_Tcutil_unify_it_conrefs( int(* cmp)( void*, void*), struct
Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y, struct _tagged_arr reason){ x=
Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x ==  y){
return;}{ void* _temp515=( void*) x->v; void* _temp523; _LL517: if( _temp515 == (
void*) Cyc_Absyn_No_constr){ goto _LL518;} else{ goto _LL519;} _LL519: if((
unsigned int) _temp515 >  1u?*(( int*) _temp515) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL520;} else{ goto _LL521;} _LL521: if(( unsigned int) _temp515 >  1u?*((
int*) _temp515) ==  Cyc_Absyn_Eq_constr: 0){ _LL524: _temp523=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp515)->f1; goto _LL522;} else{ goto _LL516;}
_LL518:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp525=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp525[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp526; _temp526.tag= Cyc_Absyn_Forward_constr; _temp526.f1= y; _temp526;});
_temp525;}))); return; _LL520:( int) _throw(({ void* _temp527[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp527, sizeof( void*), 0u));})); _LL522: { void* _temp528=( void*)
y->v; void* _temp536; _LL530: if( _temp528 == ( void*) Cyc_Absyn_No_constr){
goto _LL531;} else{ goto _LL532;} _LL532: if(( unsigned int) _temp528 >  1u?*((
int*) _temp528) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL533;} else{ goto
_LL534;} _LL534: if(( unsigned int) _temp528 >  1u?*(( int*) _temp528) ==  Cyc_Absyn_Eq_constr:
0){ _LL537: _temp536=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp528)->f1;
goto _LL535;} else{ goto _LL529;} _LL531:( void*)( y->v=( void*)(( void*) x->v));
return; _LL533:( int) _throw(({ void* _temp538[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp538, sizeof( void*), 0u));})); _LL535: if( cmp( _temp523,
_temp536) !=  0){ Cyc_Tcutil_failure_reason= reason;( int) _throw(( void*) Cyc_Tcutil_Unify);}
return; _LL529:;} _LL516:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp539; _push_handler(& _temp539);{ int _temp541= 0; if( setjmp(
_temp539.handler)){ _temp541= 1;} if( ! _temp541){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u));{ int _temp542= 1;
_npop_handler( 0u); return _temp542;}; _pop_handler();} else{ void* _temp540=(
void*) _exn_thrown; void* _temp544= _temp540; _LL546: if( _temp544 ==  Cyc_Tcutil_Unify){
goto _LL547;} else{ goto _LL548;} _LL548: goto _LL549; _LL547: return 0; _LL549:(
void) _throw( _temp544); _LL545:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp551=({ struct _tuple8 _temp550; _temp550.f1= b1;
_temp550.f2= b2; _temp550;}); void* _temp561; void* _temp563; void* _temp565;
void* _temp567; void* _temp569; struct Cyc_Absyn_Exp* _temp571; void* _temp573;
struct Cyc_Absyn_Exp* _temp575; _LL553: _LL564: _temp563= _temp551.f1; if(
_temp563 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL562;} else{ goto _LL555;}
_LL562: _temp561= _temp551.f2; if( _temp561 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL554;} else{ goto _LL555;} _LL555: _LL566: _temp565= _temp551.f1; if(
_temp565 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL556;} else{ goto _LL557;}
_LL557: _LL568: _temp567= _temp551.f2; if( _temp567 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL558;} else{ goto _LL559;} _LL559: _LL574: _temp573= _temp551.f1; if((
unsigned int) _temp573 >  1u?*(( int*) _temp573) ==  Cyc_Absyn_Upper_b: 0){
_LL576: _temp575=(( struct Cyc_Absyn_Upper_b_struct*) _temp573)->f1; goto _LL570;}
else{ goto _LL552;} _LL570: _temp569= _temp551.f2; if(( unsigned int) _temp569 > 
1u?*(( int*) _temp569) ==  Cyc_Absyn_Upper_b: 0){ _LL572: _temp571=(( struct Cyc_Absyn_Upper_b_struct*)
_temp569)->f1; goto _LL560;} else{ goto _LL552;} _LL554: return 0; _LL556:
return - 1; _LL558: return 1; _LL560: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp575); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp571); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL552:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp578=({
struct _tuple8 _temp577; _temp577.f1= a1; _temp577.f2= a2; _temp577;}); void*
_temp590; int _temp592; int _temp594; void* _temp596; void* _temp598; int
_temp600; int _temp602; void* _temp604; void* _temp606; int _temp608; void*
_temp610; int _temp612; void* _temp614; int _temp616; void* _temp618; int
_temp620; void* _temp622; struct _tagged_arr _temp624; void* _temp626; struct
_tagged_arr _temp628; _LL580: _LL599: _temp598= _temp578.f1; if(( unsigned int)
_temp598 >  16u?*(( int*) _temp598) ==  Cyc_Absyn_Format_att: 0){ _LL605:
_temp604=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp598)->f1; goto
_LL603; _LL603: _temp602=(( struct Cyc_Absyn_Format_att_struct*) _temp598)->f2;
goto _LL601; _LL601: _temp600=(( struct Cyc_Absyn_Format_att_struct*) _temp598)->f3;
goto _LL591;} else{ goto _LL582;} _LL591: _temp590= _temp578.f2; if((
unsigned int) _temp590 >  16u?*(( int*) _temp590) ==  Cyc_Absyn_Format_att: 0){
_LL597: _temp596=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp590)->f1;
goto _LL595; _LL595: _temp594=(( struct Cyc_Absyn_Format_att_struct*) _temp590)->f2;
goto _LL593; _LL593: _temp592=(( struct Cyc_Absyn_Format_att_struct*) _temp590)->f3;
goto _LL581;} else{ goto _LL582;} _LL582: _LL611: _temp610= _temp578.f1; if((
unsigned int) _temp610 >  16u?*(( int*) _temp610) ==  Cyc_Absyn_Regparm_att: 0){
_LL613: _temp612=(( struct Cyc_Absyn_Regparm_att_struct*) _temp610)->f1; goto
_LL607;} else{ goto _LL584;} _LL607: _temp606= _temp578.f2; if(( unsigned int)
_temp606 >  16u?*(( int*) _temp606) ==  Cyc_Absyn_Regparm_att: 0){ _LL609:
_temp608=(( struct Cyc_Absyn_Regparm_att_struct*) _temp606)->f1; goto _LL583;}
else{ goto _LL584;} _LL584: _LL619: _temp618= _temp578.f1; if(( unsigned int)
_temp618 >  16u?*(( int*) _temp618) ==  Cyc_Absyn_Aligned_att: 0){ _LL621:
_temp620=(( struct Cyc_Absyn_Aligned_att_struct*) _temp618)->f1; goto _LL615;}
else{ goto _LL586;} _LL615: _temp614= _temp578.f2; if(( unsigned int) _temp614 > 
16u?*(( int*) _temp614) ==  Cyc_Absyn_Aligned_att: 0){ _LL617: _temp616=((
struct Cyc_Absyn_Aligned_att_struct*) _temp614)->f1; goto _LL585;} else{ goto
_LL586;} _LL586: _LL627: _temp626= _temp578.f1; if(( unsigned int) _temp626 > 
16u?*(( int*) _temp626) ==  Cyc_Absyn_Section_att: 0){ _LL629: _temp628=((
struct Cyc_Absyn_Section_att_struct*) _temp626)->f1; goto _LL623;} else{ goto
_LL588;} _LL623: _temp622= _temp578.f2; if(( unsigned int) _temp622 >  16u?*((
int*) _temp622) ==  Cyc_Absyn_Section_att: 0){ _LL625: _temp624=(( struct Cyc_Absyn_Section_att_struct*)
_temp622)->f1; goto _LL587;} else{ goto _LL588;} _LL588: goto _LL589; _LL581:
return( _temp604 ==  _temp596? _temp602 ==  _temp594: 0)? _temp600 ==  _temp592:
0; _LL583: _temp620= _temp612; _temp616= _temp608; goto _LL585; _LL585: return
_temp620 ==  _temp616; _LL587: return Cyc_Std_strcmp( _temp628, _temp624) ==  0;
_LL589: return 0; _LL579:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,(
void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct
Cyc_List_List* a= a2; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null(
a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void* Cyc_Tcutil_normalize_effect(
void* e); static void* Cyc_Tcutil_rgns_of( void* t); static void* Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Structfield* sf){ return Cyc_Tcutil_rgns_of(( void*) sf->type);}
static struct _tuple5* Cyc_Tcutil_region_free_subst( struct Cyc_Absyn_Tvar* tv){
void* t;{ void* _temp630= Cyc_Absyn_conref_val( tv->kind); _LL632: if( _temp630
== ( void*) Cyc_Absyn_RgnKind){ goto _LL633;} else{ goto _LL634;} _LL634: if(
_temp630 == ( void*) Cyc_Absyn_EffKind){ goto _LL635;} else{ goto _LL636;}
_LL636: goto _LL637; _LL633: t=( void*) Cyc_Absyn_HeapRgn; goto _LL631; _LL635:
t= Cyc_Absyn_empty_effect; goto _LL631; _LL637: t= Cyc_Absyn_sint_t; goto _LL631;
_LL631:;} return({ struct _tuple5* _temp638=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp638->f1= tv; _temp638->f2= t; _temp638;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp639= Cyc_Tcutil_compress(
t); void* _temp687; struct Cyc_Absyn_TunionInfo _temp689; void* _temp691; struct
Cyc_List_List* _temp693; struct Cyc_Absyn_PtrInfo _temp695; void* _temp697; void*
_temp699; void* _temp701; struct Cyc_List_List* _temp703; struct Cyc_Absyn_TunionFieldInfo
_temp705; struct Cyc_List_List* _temp707; struct Cyc_List_List* _temp709; struct
Cyc_List_List* _temp711; struct Cyc_List_List* _temp713; struct Cyc_List_List*
_temp715; struct Cyc_Absyn_FnInfo _temp717; struct Cyc_List_List* _temp719;
struct Cyc_Absyn_VarargInfo* _temp721; struct Cyc_List_List* _temp723; void*
_temp725; struct Cyc_Core_Opt* _temp727; struct Cyc_List_List* _temp729; void*
_temp731; _LL641: if( _temp639 == ( void*) Cyc_Absyn_VoidType){ goto _LL642;}
else{ goto _LL643;} _LL643: if( _temp639 == ( void*) Cyc_Absyn_FloatType){ goto
_LL644;} else{ goto _LL645;} _LL645: if( _temp639 == ( void*) Cyc_Absyn_DoubleType){
goto _LL646;} else{ goto _LL647;} _LL647: if(( unsigned int) _temp639 >  4u?*((
int*) _temp639) ==  Cyc_Absyn_EnumType: 0){ goto _LL648;} else{ goto _LL649;}
_LL649: if(( unsigned int) _temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_IntType:
0){ goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int) _temp639 >  4u?*((
int*) _temp639) ==  Cyc_Absyn_Evar: 0){ goto _LL652;} else{ goto _LL653;} _LL653:
if(( unsigned int) _temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_VarType: 0){
goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int) _temp639 >  4u?*((
int*) _temp639) ==  Cyc_Absyn_RgnHandleType: 0){ _LL688: _temp687=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp639)->f1; goto _LL656;} else{ goto
_LL657;} _LL657: if(( unsigned int) _temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_TunionType:
0){ _LL690: _temp689=(( struct Cyc_Absyn_TunionType_struct*) _temp639)->f1;
_LL694: _temp693= _temp689.targs; goto _LL692; _LL692: _temp691=( void*)
_temp689.rgn; goto _LL658;} else{ goto _LL659;} _LL659: if(( unsigned int)
_temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_PointerType: 0){ _LL696:
_temp695=(( struct Cyc_Absyn_PointerType_struct*) _temp639)->f1; _LL700:
_temp699=( void*) _temp695.elt_typ; goto _LL698; _LL698: _temp697=( void*)
_temp695.rgn_typ; goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_ArrayType: 0){ _LL702: _temp701=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp639)->f1; goto _LL662;} else{
goto _LL663;} _LL663: if(( unsigned int) _temp639 >  4u?*(( int*) _temp639) == 
Cyc_Absyn_TupleType: 0){ _LL704: _temp703=(( struct Cyc_Absyn_TupleType_struct*)
_temp639)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_TunionFieldType: 0){ _LL706:
_temp705=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp639)->f1; _LL708:
_temp707= _temp705.targs; goto _LL666;} else{ goto _LL667;} _LL667: if((
unsigned int) _temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_StructType: 0){
_LL710: _temp709=(( struct Cyc_Absyn_StructType_struct*) _temp639)->f2; goto
_LL668;} else{ goto _LL669;} _LL669: if(( unsigned int) _temp639 >  4u?*(( int*)
_temp639) ==  Cyc_Absyn_UnionType: 0){ _LL712: _temp711=(( struct Cyc_Absyn_UnionType_struct*)
_temp639)->f2; goto _LL670;} else{ goto _LL671;} _LL671: if(( unsigned int)
_temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_AnonStructType: 0){ _LL714:
_temp713=(( struct Cyc_Absyn_AnonStructType_struct*) _temp639)->f1; goto _LL672;}
else{ goto _LL673;} _LL673: if(( unsigned int) _temp639 >  4u?*(( int*) _temp639)
==  Cyc_Absyn_AnonUnionType: 0){ _LL716: _temp715=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp639)->f1; goto _LL674;} else{ goto _LL675;} _LL675: if(( unsigned int)
_temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_FnType: 0){ _LL718: _temp717=((
struct Cyc_Absyn_FnType_struct*) _temp639)->f1; _LL730: _temp729= _temp717.tvars;
goto _LL728; _LL728: _temp727= _temp717.effect; goto _LL726; _LL726: _temp725=(
void*) _temp717.ret_typ; goto _LL724; _LL724: _temp723= _temp717.args; goto
_LL722; _LL722: _temp721= _temp717.cyc_varargs; goto _LL720; _LL720: _temp719=
_temp717.rgn_po; goto _LL676;} else{ goto _LL677;} _LL677: if( _temp639 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL678;} else{ goto _LL679;} _LL679: if((
unsigned int) _temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_AccessEff: 0){
goto _LL680;} else{ goto _LL681;} _LL681: if(( unsigned int) _temp639 >  4u?*((
int*) _temp639) ==  Cyc_Absyn_JoinEff: 0){ goto _LL682;} else{ goto _LL683;}
_LL683: if(( unsigned int) _temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_RgnsEff:
0){ _LL732: _temp731=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp639)->f1;
goto _LL684;} else{ goto _LL685;} _LL685: if(( unsigned int) _temp639 >  4u?*((
int*) _temp639) ==  Cyc_Absyn_TypedefType: 0){ goto _LL686;} else{ goto _LL640;}
_LL642: goto _LL644; _LL644: goto _LL646; _LL646: goto _LL648; _LL648: goto
_LL650; _LL650: return Cyc_Absyn_empty_effect; _LL652: goto _LL654; _LL654: {
void* _temp733= Cyc_Tcutil_typ_kind( t); _LL735: if( _temp733 == ( void*) Cyc_Absyn_RgnKind){
goto _LL736;} else{ goto _LL737;} _LL737: if( _temp733 == ( void*) Cyc_Absyn_EffKind){
goto _LL738;} else{ goto _LL739;} _LL739: goto _LL740; _LL736: return( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp741=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp741[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp742; _temp742.tag= Cyc_Absyn_AccessEff; _temp742.f1=(
void*) t; _temp742;}); _temp741;}); _LL738: return t; _LL740: return( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp743=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp743[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp744; _temp744.tag= Cyc_Absyn_RgnsEff; _temp744.f1=( void*) t; _temp744;});
_temp743;}); _LL734:;} _LL656: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp745=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp745[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp746; _temp746.tag= Cyc_Absyn_AccessEff;
_temp746.f1=( void*) _temp687; _temp746;}); _temp745;}); _LL658: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp749=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp749->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp750=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp750[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp751; _temp751.tag= Cyc_Absyn_AccessEff;
_temp751.f1=( void*) _temp691; _temp751;}); _temp750;})); _temp749->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp693); _temp749;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp747=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp747[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp748; _temp748.tag= Cyc_Absyn_JoinEff; _temp748.f1= ts; _temp748;});
_temp747;}));} _LL660: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp752=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp752[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp753; _temp753.tag= Cyc_Absyn_JoinEff;
_temp753.f1=({ void* _temp754[ 2u]; _temp754[ 1u]= Cyc_Tcutil_rgns_of( _temp699);
_temp754[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp755=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp755[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp756; _temp756.tag= Cyc_Absyn_AccessEff;
_temp756.f1=( void*) _temp697; _temp756;}); _temp755;}); Cyc_List_list( _tag_arr(
_temp754, sizeof( void*), 2u));}); _temp753;}); _temp752;})); _LL662: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp701)); _LL664: { struct Cyc_List_List* _temp757= 0;
for( 0; _temp703 !=  0; _temp703=(( struct Cyc_List_List*) _check_null( _temp703))->tl){
_temp757=({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp758->hd=( void*)(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp703))->hd)).f2; _temp758->tl= _temp757;
_temp758;});} _temp707= _temp757; goto _LL666;} _LL666: _temp709= _temp707; goto
_LL668; _LL668: _temp711= _temp709; goto _LL670; _LL670: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp759=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp759[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp760; _temp760.tag= Cyc_Absyn_JoinEff; _temp760.f1= Cyc_List_map( Cyc_Tcutil_rgns_of,
_temp711); _temp760;}); _temp759;})); _LL672: _temp715= _temp713; goto _LL674;
_LL674: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp761=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp761[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp762; _temp762.tag= Cyc_Absyn_JoinEff;
_temp762.f1=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_rgns_of_field, _temp715);
_temp762;}); _temp761;})); _LL676: { void* _temp763= Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst, _temp729),( void*)((
struct Cyc_Core_Opt*) _check_null( _temp727))->v); return Cyc_Tcutil_normalize_effect(
_temp763);} _LL678: return Cyc_Absyn_empty_effect; _LL680: goto _LL682; _LL682:
return t; _LL684: return Cyc_Tcutil_rgns_of( _temp731); _LL686: return({ void*
_temp764[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp764, sizeof( void*), 0u));});
_LL640:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp765= e; struct Cyc_List_List* _temp773; struct Cyc_List_List**
_temp775; void* _temp776; _LL767: if(( unsigned int) _temp765 >  4u?*(( int*)
_temp765) ==  Cyc_Absyn_JoinEff: 0){ _LL774: _temp773=(( struct Cyc_Absyn_JoinEff_struct*)
_temp765)->f1; _temp775=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp765)->f1; goto _LL768;} else{ goto _LL769;} _LL769: if(( unsigned int)
_temp765 >  4u?*(( int*) _temp765) ==  Cyc_Absyn_RgnsEff: 0){ _LL777: _temp776=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp765)->f1; goto _LL770;} else{
goto _LL771;} _LL771: goto _LL772; _LL768: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp775; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp778=( void*)(( struct Cyc_List_List*) _check_null( effs))->hd;(
void*)((( struct Cyc_List_List*) _check_null( effs))->hd=( void*) Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect( _temp778)));{ void* _temp779=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; _LL781: if(( unsigned int) _temp779 >  4u?*(( int*)
_temp779) ==  Cyc_Absyn_JoinEff: 0){ goto _LL782;} else{ goto _LL783;} _LL783:
goto _LL784; _LL782: nested_join= 1; goto _LL780; _LL784: goto _LL780; _LL780:;}}}
if( ! nested_join){ return e;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp775; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp785= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp793; void* _temp795; _LL787:
if(( unsigned int) _temp785 >  4u?*(( int*) _temp785) ==  Cyc_Absyn_JoinEff: 0){
_LL794: _temp793=(( struct Cyc_Absyn_JoinEff_struct*) _temp785)->f1; goto _LL788;}
else{ goto _LL789;} _LL789: if(( unsigned int) _temp785 >  4u?*(( int*) _temp785)
==  Cyc_Absyn_AccessEff: 0){ _LL796: _temp795=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp785)->f1; if( _temp795 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL790;} else{
goto _LL791;}} else{ goto _LL791;} _LL791: goto _LL792; _LL788: effects= Cyc_List_revappend(
_temp793, effects); goto _LL786; _LL790: goto _LL786; _LL792: effects=({ struct
Cyc_List_List* _temp797=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp797->hd=( void*) _temp785; _temp797->tl= effects; _temp797;}); goto _LL786;
_LL786:;}}* _temp775= Cyc_List_imp_rev( effects); return e;}} _LL770: return Cyc_Tcutil_rgns_of(
_temp776); _LL772: return e; _LL766:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp798= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp806; struct Cyc_List_List _temp808; struct Cyc_List_List* _temp809; void*
_temp811; struct Cyc_Core_Opt* _temp813; struct Cyc_Core_Opt* _temp815; _LL800:
if(( unsigned int) _temp798 >  4u?*(( int*) _temp798) ==  Cyc_Absyn_JoinEff: 0){
_LL807: _temp806=(( struct Cyc_Absyn_JoinEff_struct*) _temp798)->f1; if(
_temp806 ==  0){ goto _LL802;} else{ _temp808=* _temp806; _LL812: _temp811=(
void*) _temp808.hd; goto _LL810; _LL810: _temp809= _temp808.tl; goto _LL801;}}
else{ goto _LL802;} _LL802: if(( unsigned int) _temp798 >  4u?*(( int*) _temp798)
==  Cyc_Absyn_Evar: 0){ _LL816: _temp815=(( struct Cyc_Absyn_Evar_struct*)
_temp798)->f1; goto _LL814; _LL814: _temp813=(( struct Cyc_Absyn_Evar_struct*)
_temp798)->f4; goto _LL803;} else{ goto _LL804;} _LL804: goto _LL805; _LL801: {
void* _temp817= Cyc_Tcutil_compress( _temp811); struct Cyc_Core_Opt* _temp823;
_LL819: if(( unsigned int) _temp817 >  4u?*(( int*) _temp817) ==  Cyc_Absyn_Evar:
0){ _LL824: _temp823=(( struct Cyc_Absyn_Evar_struct*) _temp817)->f4; goto
_LL820;} else{ goto _LL821;} _LL821: goto _LL822; _LL820: return({ struct
_tuple9* _temp825=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp825->f1= _temp811; _temp825->f2= _temp809; _temp825->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp823))->v; _temp825;}); _LL822: return 0;
_LL818:;} _LL803: if( _temp815 ==  0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp815))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp826[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp826, sizeof( void*), 0u));});} return({ struct _tuple9* _temp827=( struct
_tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp827->f1= t; _temp827->f2= 0;
_temp827->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp813))->v; _temp827;}); _LL805: return 0; _LL799:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp828=({ struct Cyc_Absyn_FnType_struct*
_temp829=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp829[ 0]=({ struct Cyc_Absyn_FnType_struct _temp830; _temp830.tag= Cyc_Absyn_FnType;
_temp830.f1=({ struct Cyc_Absyn_FnInfo _temp831; _temp831.tvars= 0; _temp831.effect=({
struct Cyc_Core_Opt* _temp832=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp832->v=( void*) eff; _temp832;}); _temp831.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp831.args= 0; _temp831.c_varargs= 0; _temp831.cyc_varargs=
0; _temp831.rgn_po= 0; _temp831.attributes= 0; _temp831;}); _temp830;});
_temp829;}); return Cyc_Absyn_atb_typ(( void*) _temp828,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp833= Cyc_Tcutil_compress( e); void*
_temp845; struct Cyc_List_List* _temp847; void* _temp849; struct Cyc_Core_Opt*
_temp851; struct Cyc_Core_Opt* _temp853; struct Cyc_Core_Opt** _temp855; struct
Cyc_Core_Opt* _temp856; _LL835: if(( unsigned int) _temp833 >  4u?*(( int*)
_temp833) ==  Cyc_Absyn_AccessEff: 0){ _LL846: _temp845=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp833)->f1; goto _LL836;} else{ goto _LL837;} _LL837: if(( unsigned int)
_temp833 >  4u?*(( int*) _temp833) ==  Cyc_Absyn_JoinEff: 0){ _LL848: _temp847=((
struct Cyc_Absyn_JoinEff_struct*) _temp833)->f1; goto _LL838;} else{ goto _LL839;}
_LL839: if(( unsigned int) _temp833 >  4u?*(( int*) _temp833) ==  Cyc_Absyn_RgnsEff:
0){ _LL850: _temp849=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp833)->f1;
goto _LL840;} else{ goto _LL841;} _LL841: if(( unsigned int) _temp833 >  4u?*((
int*) _temp833) ==  Cyc_Absyn_Evar: 0){ _LL857: _temp856=(( struct Cyc_Absyn_Evar_struct*)
_temp833)->f1; goto _LL854; _LL854: _temp853=(( struct Cyc_Absyn_Evar_struct*)
_temp833)->f2; _temp855=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp833)->f2; goto _LL852; _LL852: _temp851=(( struct Cyc_Absyn_Evar_struct*)
_temp833)->f4; goto _LL842;} else{ goto _LL843;} _LL843: goto _LL844; _LL836:
if( constrain){ return Cyc_Tcutil_unify( r, _temp845);} _temp845= Cyc_Tcutil_compress(
_temp845); if( r ==  _temp845){ return 1;}{ struct _tuple8 _temp859=({ struct
_tuple8 _temp858; _temp858.f1= r; _temp858.f2= _temp845; _temp858;}); void*
_temp865; struct Cyc_Absyn_Tvar* _temp867; void* _temp869; struct Cyc_Absyn_Tvar*
_temp871; _LL861: _LL870: _temp869= _temp859.f1; if(( unsigned int) _temp869 > 
4u?*(( int*) _temp869) ==  Cyc_Absyn_VarType: 0){ _LL872: _temp871=(( struct Cyc_Absyn_VarType_struct*)
_temp869)->f1; goto _LL866;} else{ goto _LL863;} _LL866: _temp865= _temp859.f2;
if(( unsigned int) _temp865 >  4u?*(( int*) _temp865) ==  Cyc_Absyn_VarType: 0){
_LL868: _temp867=(( struct Cyc_Absyn_VarType_struct*) _temp865)->f1; goto _LL862;}
else{ goto _LL863;} _LL863: goto _LL864; _LL862: return Cyc_Absyn_tvar_cmp(
_temp871, _temp867) ==  0; _LL864: return 0; _LL860:;} _LL838: for( 0; _temp847
!=  0; _temp847=(( struct Cyc_List_List*) _check_null( _temp847))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp847))->hd)){
return 1;}} return 0; _LL840: { void* _temp873= Cyc_Tcutil_rgns_of( _temp849);
void* _temp879; _LL875: if(( unsigned int) _temp873 >  4u?*(( int*) _temp873) == 
Cyc_Absyn_RgnsEff: 0){ _LL880: _temp879=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp873)->f1; goto _LL876;} else{ goto _LL877;} _LL877: goto _LL878; _LL876:
if( ! constrain){ return 0;}{ void* _temp881= Cyc_Tcutil_compress( _temp879);
struct Cyc_Core_Opt* _temp887; struct Cyc_Core_Opt* _temp889; struct Cyc_Core_Opt**
_temp891; struct Cyc_Core_Opt* _temp892; _LL883: if(( unsigned int) _temp881 > 
4u?*(( int*) _temp881) ==  Cyc_Absyn_Evar: 0){ _LL893: _temp892=(( struct Cyc_Absyn_Evar_struct*)
_temp881)->f1; goto _LL890; _LL890: _temp889=(( struct Cyc_Absyn_Evar_struct*)
_temp881)->f2; _temp891=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp881)->f2; goto _LL888; _LL888: _temp887=(( struct Cyc_Absyn_Evar_struct*)
_temp881)->f4; goto _LL884;} else{ goto _LL885;} _LL885: goto _LL886; _LL884: {
void* _temp894= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp887); Cyc_Tcutil_occurs( _temp894,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp887))->v, r);{ void* _temp895= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp897=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp897[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp898; _temp898.tag= Cyc_Absyn_JoinEff; _temp898.f1=({ void* _temp899[ 2u];
_temp899[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp900=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp900[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp901; _temp901.tag= Cyc_Absyn_AccessEff;
_temp901.f1=( void*) r; _temp901;}); _temp900;}); _temp899[ 0u]= _temp894; Cyc_List_list(
_tag_arr( _temp899, sizeof( void*), 2u));}); _temp898;}); _temp897;}));*
_temp891=({ struct Cyc_Core_Opt* _temp896=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp896->v=( void*) _temp895; _temp896;});
return 1;}} _LL886: return 0; _LL882:;} _LL878: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp873); _LL874:;} _LL842: if( _temp856 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp856))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp902[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp902, sizeof( void*), 0u));});} if( ! constrain){ return 0;}{
void* _temp903= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp851); Cyc_Tcutil_occurs( _temp903,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp851))->v, r);{ struct Cyc_Absyn_JoinEff_struct* _temp904=({
struct Cyc_Absyn_JoinEff_struct* _temp906=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp906[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp907; _temp907.tag= Cyc_Absyn_JoinEff; _temp907.f1=({ struct Cyc_List_List*
_temp908=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp908->hd=( void*) _temp903; _temp908->tl=({ struct Cyc_List_List* _temp909=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp909->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp910=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp910[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp911; _temp911.tag= Cyc_Absyn_AccessEff; _temp911.f1=(
void*) r; _temp911;}); _temp910;})); _temp909->tl= 0; _temp909;}); _temp908;});
_temp907;}); _temp906;});* _temp855=({ struct Cyc_Core_Opt* _temp905=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp905->v=( void*)((
void*) _temp904); _temp905;}); return 1;}} _LL844: return 0; _LL834:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp912= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp924;
void* _temp926; struct Cyc_Core_Opt* _temp928; struct Cyc_Core_Opt* _temp930;
struct Cyc_Core_Opt** _temp932; struct Cyc_Core_Opt* _temp933; _LL914: if((
unsigned int) _temp912 >  4u?*(( int*) _temp912) ==  Cyc_Absyn_AccessEff: 0){
goto _LL915;} else{ goto _LL916;} _LL916: if(( unsigned int) _temp912 >  4u?*((
int*) _temp912) ==  Cyc_Absyn_JoinEff: 0){ _LL925: _temp924=(( struct Cyc_Absyn_JoinEff_struct*)
_temp912)->f1; goto _LL917;} else{ goto _LL918;} _LL918: if(( unsigned int)
_temp912 >  4u?*(( int*) _temp912) ==  Cyc_Absyn_RgnsEff: 0){ _LL927: _temp926=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp912)->f1; goto _LL919;} else{
goto _LL920;} _LL920: if(( unsigned int) _temp912 >  4u?*(( int*) _temp912) == 
Cyc_Absyn_Evar: 0){ _LL934: _temp933=(( struct Cyc_Absyn_Evar_struct*) _temp912)->f1;
goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_Evar_struct*) _temp912)->f2;
_temp932=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp912)->f2;
goto _LL929; _LL929: _temp928=(( struct Cyc_Absyn_Evar_struct*) _temp912)->f4;
goto _LL921;} else{ goto _LL922;} _LL922: goto _LL923; _LL915: return 0; _LL917:
for( 0; _temp924 !=  0; _temp924=(( struct Cyc_List_List*) _check_null( _temp924))->tl){
if( Cyc_Tcutil_type_in_effect( constrain, t,( void*)(( struct Cyc_List_List*)
_check_null( _temp924))->hd)){ return 1;}} return 0; _LL919: _temp926= Cyc_Tcutil_compress(
_temp926); if( t ==  _temp926){ return 1;} if( constrain){ return Cyc_Tcutil_unify(
t, _temp926);}{ void* _temp935= Cyc_Tcutil_rgns_of( t); void* _temp941; _LL937:
if(( unsigned int) _temp935 >  4u?*(( int*) _temp935) ==  Cyc_Absyn_RgnsEff: 0){
_LL942: _temp941=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp935)->f1;
goto _LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938: { struct _tuple8
_temp944=({ struct _tuple8 _temp943; _temp943.f1= t; _temp943.f2= Cyc_Tcutil_compress(
_temp941); _temp943;}); void* _temp950; struct Cyc_Absyn_Tvar* _temp952; void*
_temp954; struct Cyc_Absyn_Tvar* _temp956; _LL946: _LL955: _temp954= _temp944.f1;
if(( unsigned int) _temp954 >  4u?*(( int*) _temp954) ==  Cyc_Absyn_VarType: 0){
_LL957: _temp956=(( struct Cyc_Absyn_VarType_struct*) _temp954)->f1; goto _LL951;}
else{ goto _LL948;} _LL951: _temp950= _temp944.f2; if(( unsigned int) _temp950 > 
4u?*(( int*) _temp950) ==  Cyc_Absyn_VarType: 0){ _LL953: _temp952=(( struct Cyc_Absyn_VarType_struct*)
_temp950)->f1; goto _LL947;} else{ goto _LL948;} _LL948: goto _LL949; _LL947:
return Cyc_Tcutil_unify( t, _temp941); _LL949: return t ==  _temp941; _LL945:;}
_LL940: return Cyc_Tcutil_type_in_effect( constrain, t, _temp935); _LL936:;}
_LL921: if( _temp933 ==  0? 1:( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp933))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp958[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp958, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp959= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp928); Cyc_Tcutil_occurs( _temp959,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp928))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp960=({ struct Cyc_Absyn_JoinEff_struct* _temp962=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp962[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp963; _temp963.tag= Cyc_Absyn_JoinEff; _temp963.f1=({ struct Cyc_List_List*
_temp964=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp964->hd=( void*) _temp959; _temp964->tl=({ struct Cyc_List_List* _temp965=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp965->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp966=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp966[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp967; _temp967.tag= Cyc_Absyn_RgnsEff; _temp967.f1=( void*) t; _temp967;});
_temp966;})); _temp965->tl= 0; _temp965;}); _temp964;}); _temp963;}); _temp962;});*
_temp932=({ struct Cyc_Core_Opt* _temp961=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp961->v=( void*)(( void*) _temp960); _temp961;});
return 1;}} _LL923: return 0; _LL913:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp968= e; struct Cyc_Absyn_Tvar* _temp980; struct Cyc_List_List*
_temp982; void* _temp984; struct Cyc_Core_Opt* _temp986; struct Cyc_Core_Opt*
_temp988; struct Cyc_Core_Opt** _temp990; struct Cyc_Core_Opt* _temp991; _LL970:
if(( unsigned int) _temp968 >  4u?*(( int*) _temp968) ==  Cyc_Absyn_VarType: 0){
_LL981: _temp980=(( struct Cyc_Absyn_VarType_struct*) _temp968)->f1; goto _LL971;}
else{ goto _LL972;} _LL972: if(( unsigned int) _temp968 >  4u?*(( int*) _temp968)
==  Cyc_Absyn_JoinEff: 0){ _LL983: _temp982=(( struct Cyc_Absyn_JoinEff_struct*)
_temp968)->f1; goto _LL973;} else{ goto _LL974;} _LL974: if(( unsigned int)
_temp968 >  4u?*(( int*) _temp968) ==  Cyc_Absyn_RgnsEff: 0){ _LL985: _temp984=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp968)->f1; goto _LL975;} else{
goto _LL976;} _LL976: if(( unsigned int) _temp968 >  4u?*(( int*) _temp968) == 
Cyc_Absyn_Evar: 0){ _LL992: _temp991=(( struct Cyc_Absyn_Evar_struct*) _temp968)->f1;
goto _LL989; _LL989: _temp988=(( struct Cyc_Absyn_Evar_struct*) _temp968)->f2;
_temp990=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp968)->f2;
goto _LL987; _LL987: _temp986=(( struct Cyc_Absyn_Evar_struct*) _temp968)->f4;
goto _LL977;} else{ goto _LL978;} _LL978: goto _LL979; _LL971: return Cyc_Absyn_tvar_cmp(
v, _temp980) ==  0; _LL973: for( 0; _temp982 !=  0; _temp982=(( struct Cyc_List_List*)
_check_null( _temp982))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp982))->hd)){ return 1;}}
return 0; _LL975: { void* _temp993= Cyc_Tcutil_rgns_of( _temp984); void*
_temp999; _LL995: if(( unsigned int) _temp993 >  4u?*(( int*) _temp993) ==  Cyc_Absyn_RgnsEff:
0){ _LL1000: _temp999=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp993)->f1;
goto _LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL996: if( ! constrain){
return 0;}{ void* _temp1001= Cyc_Tcutil_compress( _temp999); struct Cyc_Core_Opt*
_temp1007; struct Cyc_Core_Opt* _temp1009; struct Cyc_Core_Opt** _temp1011;
struct Cyc_Core_Opt* _temp1012; _LL1003: if(( unsigned int) _temp1001 >  4u?*((
int*) _temp1001) ==  Cyc_Absyn_Evar: 0){ _LL1013: _temp1012=(( struct Cyc_Absyn_Evar_struct*)
_temp1001)->f1; goto _LL1010; _LL1010: _temp1009=(( struct Cyc_Absyn_Evar_struct*)
_temp1001)->f2; _temp1011=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1001)->f2; goto _LL1008; _LL1008: _temp1007=(( struct Cyc_Absyn_Evar_struct*)
_temp1001)->f4; goto _LL1004;} else{ goto _LL1005;} _LL1005: goto _LL1006;
_LL1004: { void* _temp1014= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp1007); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1007))->v, v)){
return 0;}{ void* _temp1015= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1017=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1017[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1018; _temp1018.tag= Cyc_Absyn_JoinEff;
_temp1018.f1=({ void* _temp1019[ 2u]; _temp1019[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1020=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1020[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1021; _temp1021.tag= Cyc_Absyn_VarType;
_temp1021.f1= v; _temp1021;}); _temp1020;}); _temp1019[ 0u]= _temp1014; Cyc_List_list(
_tag_arr( _temp1019, sizeof( void*), 2u));}); _temp1018;}); _temp1017;}));*
_temp1011=({ struct Cyc_Core_Opt* _temp1016=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1016->v=( void*) _temp1015; _temp1016;});
return 1;}} _LL1006: return 0; _LL1002:;} _LL998: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp993); _LL994:;} _LL977: if( _temp991 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp991))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp1022[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp1022, sizeof( void*), 0u));});}{ void* _temp1023= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp986); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp986))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp1024=({ struct Cyc_Absyn_JoinEff_struct* _temp1026=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1026[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1027; _temp1027.tag= Cyc_Absyn_JoinEff; _temp1027.f1=({
struct Cyc_List_List* _temp1028=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1028->hd=( void*) _temp1023; _temp1028->tl=({
struct Cyc_List_List* _temp1029=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1029->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1030=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1030[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1031; _temp1031.tag= Cyc_Absyn_VarType;
_temp1031.f1= v; _temp1031;}); _temp1030;})); _temp1029->tl= 0; _temp1029;});
_temp1028;}); _temp1027;}); _temp1026;});* _temp990=({ struct Cyc_Core_Opt*
_temp1025=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1025->v=( void*)(( void*) _temp1024); _temp1025;}); return 1;}} _LL979:
return 0; _LL969:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp1032= e; struct Cyc_List_List* _temp1042;
void* _temp1044; _LL1034: if(( unsigned int) _temp1032 >  4u?*(( int*) _temp1032)
==  Cyc_Absyn_JoinEff: 0){ _LL1043: _temp1042=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1032)->f1; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(( unsigned int)
_temp1032 >  4u?*(( int*) _temp1032) ==  Cyc_Absyn_RgnsEff: 0){ _LL1045:
_temp1044=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1032)->f1; goto
_LL1037;} else{ goto _LL1038;} _LL1038: if(( unsigned int) _temp1032 >  4u?*((
int*) _temp1032) ==  Cyc_Absyn_Evar: 0){ goto _LL1039;} else{ goto _LL1040;}
_LL1040: goto _LL1041; _LL1035: for( 0; _temp1042 !=  0; _temp1042=(( struct Cyc_List_List*)
_check_null( _temp1042))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp1042))->hd)){ return 1;}} return 0;
_LL1037: { void* _temp1046= Cyc_Tcutil_rgns_of( _temp1044); void* _temp1052;
_LL1048: if(( unsigned int) _temp1046 >  4u?*(( int*) _temp1046) ==  Cyc_Absyn_RgnsEff:
0){ _LL1053: _temp1052=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1046)->f1;
goto _LL1049;} else{ goto _LL1050;} _LL1050: goto _LL1051; _LL1049: return 0;
_LL1051: return Cyc_Tcutil_evar_in_effect( evar, _temp1046); _LL1047:;} _LL1039:
return evar ==  e; _LL1041: return 0; _LL1033:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1054= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1068; void* _temp1070; struct Cyc_Absyn_Tvar*
_temp1072; void* _temp1074; struct Cyc_Core_Opt* _temp1076; struct Cyc_Core_Opt*
_temp1078; struct Cyc_Core_Opt** _temp1080; _LL1056: if(( unsigned int)
_temp1054 >  4u?*(( int*) _temp1054) ==  Cyc_Absyn_JoinEff: 0){ _LL1069:
_temp1068=(( struct Cyc_Absyn_JoinEff_struct*) _temp1054)->f1; goto _LL1057;}
else{ goto _LL1058;} _LL1058: if(( unsigned int) _temp1054 >  4u?*(( int*)
_temp1054) ==  Cyc_Absyn_AccessEff: 0){ _LL1071: _temp1070=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1054)->f1; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int)
_temp1054 >  4u?*(( int*) _temp1054) ==  Cyc_Absyn_VarType: 0){ _LL1073:
_temp1072=(( struct Cyc_Absyn_VarType_struct*) _temp1054)->f1; goto _LL1061;}
else{ goto _LL1062;} _LL1062: if(( unsigned int) _temp1054 >  4u?*(( int*)
_temp1054) ==  Cyc_Absyn_RgnsEff: 0){ _LL1075: _temp1074=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1054)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(( unsigned int)
_temp1054 >  4u?*(( int*) _temp1054) ==  Cyc_Absyn_Evar: 0){ _LL1079: _temp1078=((
struct Cyc_Absyn_Evar_struct*) _temp1054)->f2; _temp1080=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1054)->f2; goto _LL1077; _LL1077: _temp1076=((
struct Cyc_Absyn_Evar_struct*) _temp1054)->f4; goto _LL1065;} else{ goto _LL1066;}
_LL1066: goto _LL1067; _LL1057: for( 0; _temp1068 !=  0; _temp1068=(( struct Cyc_List_List*)
_check_null( _temp1068))->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*)(( struct Cyc_List_List*) _check_null( _temp1068))->hd, e2)){ return 0;}}
return 1; _LL1059: return( Cyc_Tcutil_region_in_effect( 0, _temp1070, e2)? 1:
Cyc_Tcutil_region_in_effect( 1, _temp1070, e2))? 1: Cyc_Tcutil_unify( _temp1070,(
void*) Cyc_Absyn_HeapRgn); _LL1061: return Cyc_Tcutil_variable_in_effect( 0,
_temp1072, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1072, e2); _LL1063: {
void* _temp1081= Cyc_Tcutil_rgns_of( _temp1074); void* _temp1087; _LL1083: if((
unsigned int) _temp1081 >  4u?*(( int*) _temp1081) ==  Cyc_Absyn_RgnsEff: 0){
_LL1088: _temp1087=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1081)->f1;
goto _LL1084;} else{ goto _LL1085;} _LL1085: goto _LL1086; _LL1084: return( Cyc_Tcutil_type_in_effect(
0, _temp1087, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1087, e2))? 1: Cyc_Tcutil_unify(
_temp1087, Cyc_Absyn_sint_t); _LL1086: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1081, e2); _LL1082:;} _LL1065: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1080=({ struct Cyc_Core_Opt* _temp1089=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1089->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1090=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1090[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1091; _temp1091.tag= Cyc_Absyn_JoinEff; _temp1091.f1=
0; _temp1091;}); _temp1090;})); _temp1089;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1076))->v, e2);*
_temp1080=({ struct Cyc_Core_Opt* _temp1092=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1092->v=( void*) e2; _temp1092;});}} return
1; _LL1067: return({ struct Cyc_Std_String_pa_struct _temp1094; _temp1094.tag=
Cyc_Std_String_pa; _temp1094.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1093[ 1u]={& _temp1094};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1093, sizeof( void*), 1u));}});
_LL1055:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1096=({ struct
_tuple10 _temp1095; _temp1095.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1095.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1095;}); _LL1098: goto _LL1099; _LL1099:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1097:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8 _temp1102;
void* _temp1103; void* _temp1105; struct _tuple8* _temp1100=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( r1))->hd; _temp1102=* _temp1100; _LL1106:
_temp1105= _temp1102.f1; goto _LL1104; _LL1104: _temp1103= _temp1102.f2; goto
_LL1101; _LL1101: { int found= _temp1105 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2=(( struct Cyc_List_List*)
_check_null( r2))->tl){ struct _tuple8 _temp1109; void* _temp1110; void*
_temp1112; struct _tuple8* _temp1107=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( r2))->hd; _temp1109=* _temp1107; _LL1113: _temp1112= _temp1109.f1;
goto _LL1111; _LL1111: _temp1110= _temp1109.f2; goto _LL1108; _LL1108: if( Cyc_Tcutil_unify(
_temp1105, _temp1112)? Cyc_Tcutil_unify( _temp1103, _temp1110): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason=(
struct _tagged_arr) _tag_arr( 0u, 0u, 0u); t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2); if( t1 ==  t2){ return;}{ void* _temp1114= t1; struct Cyc_Core_Opt*
_temp1120; struct Cyc_Core_Opt* _temp1122; struct Cyc_Core_Opt** _temp1124;
struct Cyc_Core_Opt* _temp1125; _LL1116: if(( unsigned int) _temp1114 >  4u?*((
int*) _temp1114) ==  Cyc_Absyn_Evar: 0){ _LL1126: _temp1125=(( struct Cyc_Absyn_Evar_struct*)
_temp1114)->f1; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_Evar_struct*)
_temp1114)->f2; _temp1124=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1114)->f2; goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_Evar_struct*)
_temp1114)->f4; goto _LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119;
_LL1117: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1120))->v, t2);{ void* _temp1127= Cyc_Tcutil_typ_kind( t2);
if( Cyc_Tcutil_kind_leq( _temp1127,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1125))->v)){* _temp1124=({ struct Cyc_Core_Opt* _temp1128=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1128->v=( void*) t2; _temp1128;});
return;} else{{ void* _temp1129= t2; struct Cyc_Core_Opt* _temp1137; struct Cyc_Core_Opt*
_temp1139; struct Cyc_Core_Opt** _temp1141; struct Cyc_Absyn_PtrInfo _temp1143;
_LL1131: if(( unsigned int) _temp1129 >  4u?*(( int*) _temp1129) ==  Cyc_Absyn_Evar:
0){ _LL1140: _temp1139=(( struct Cyc_Absyn_Evar_struct*) _temp1129)->f2;
_temp1141=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1129)->f2;
goto _LL1138; _LL1138: _temp1137=(( struct Cyc_Absyn_Evar_struct*) _temp1129)->f4;
goto _LL1132;} else{ goto _LL1133;} _LL1133: if(( unsigned int) _temp1129 >  4u?*((
int*) _temp1129) ==  Cyc_Absyn_PointerType: 0){ _LL1144: _temp1143=(( struct Cyc_Absyn_PointerType_struct*)
_temp1129)->f1; goto _LL1142;} else{ goto _LL1135;} _LL1142: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1125))->v == ( void*) Cyc_Absyn_BoxKind){
goto _LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136; _LL1132: { struct Cyc_List_List*
_temp1145=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1120))->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1137))->v; for( 0; s2 !=  0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1145,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if(
Cyc_Tcutil_kind_leq(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1125))->v,
_temp1127)){* _temp1141=({ struct Cyc_Core_Opt* _temp1146=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1146->v=( void*) t1; _temp1146;});
return;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)", sizeof(
unsigned char), 25u); goto _LL1130;} _LL1134: { struct Cyc_Absyn_Conref*
_temp1147= Cyc_Absyn_compress_conref( _temp1143.bounds);{ void* _temp1148=( void*)
_temp1147->v; _LL1150: if( _temp1148 == ( void*) Cyc_Absyn_No_constr){ goto
_LL1151;} else{ goto _LL1152;} _LL1152: goto _LL1153; _LL1151:( void*)(
_temp1147->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1154=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1154[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp1155; _temp1155.tag= Cyc_Absyn_Eq_constr;
_temp1155.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1156=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1156[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1157; _temp1157.tag= Cyc_Absyn_Upper_b;
_temp1157.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1157;}); _temp1156;}));
_temp1155;}); _temp1154;})));* _temp1124=({ struct Cyc_Core_Opt* _temp1158=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1158->v=(
void*) t2; _temp1158;}); return; _LL1153: goto _LL1149; _LL1149:;} goto _LL1130;}
_LL1136: goto _LL1130; _LL1130:;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} _LL1119:
goto _LL1115; _LL1115:;}{ struct _tuple8 _temp1160=({ struct _tuple8 _temp1159;
_temp1159.f1= t2; _temp1159.f2= t1; _temp1159;}); void* _temp1214; void*
_temp1216; void* _temp1218; void* _temp1220; struct Cyc_Absyn_Tvar* _temp1222;
void* _temp1224; struct Cyc_Absyn_Tvar* _temp1226; void* _temp1228; struct Cyc_Absyn_Structdecl**
_temp1230; struct Cyc_List_List* _temp1232; struct _tuple1* _temp1234; void*
_temp1236; struct Cyc_Absyn_Structdecl** _temp1238; struct Cyc_List_List*
_temp1240; struct _tuple1* _temp1242; void* _temp1244; struct _tuple1* _temp1246;
void* _temp1248; struct _tuple1* _temp1250; void* _temp1252; struct Cyc_Absyn_Uniondecl**
_temp1254; struct Cyc_List_List* _temp1256; struct _tuple1* _temp1258; void*
_temp1260; struct Cyc_Absyn_Uniondecl** _temp1262; struct Cyc_List_List*
_temp1264; struct _tuple1* _temp1266; void* _temp1268; struct Cyc_Absyn_TunionInfo
_temp1270; void* _temp1272; struct Cyc_List_List* _temp1274; void* _temp1276;
struct Cyc_Absyn_Tuniondecl** _temp1278; struct Cyc_Absyn_Tuniondecl* _temp1280;
void* _temp1281; struct Cyc_Absyn_TunionInfo _temp1283; void* _temp1285; struct
Cyc_List_List* _temp1287; void* _temp1289; struct Cyc_Absyn_Tuniondecl**
_temp1291; struct Cyc_Absyn_Tuniondecl* _temp1293; void* _temp1294; struct Cyc_Absyn_TunionFieldInfo
_temp1296; struct Cyc_List_List* _temp1298; void* _temp1300; struct Cyc_Absyn_Tunionfield*
_temp1302; struct Cyc_Absyn_Tuniondecl* _temp1304; void* _temp1306; struct Cyc_Absyn_TunionFieldInfo
_temp1308; struct Cyc_List_List* _temp1310; void* _temp1312; struct Cyc_Absyn_Tunionfield*
_temp1314; struct Cyc_Absyn_Tuniondecl* _temp1316; void* _temp1318; struct Cyc_Absyn_PtrInfo
_temp1320; struct Cyc_Absyn_Conref* _temp1322; struct Cyc_Absyn_Tqual _temp1324;
struct Cyc_Absyn_Conref* _temp1326; void* _temp1328; void* _temp1330; void*
_temp1332; struct Cyc_Absyn_PtrInfo _temp1334; struct Cyc_Absyn_Conref*
_temp1336; struct Cyc_Absyn_Tqual _temp1338; struct Cyc_Absyn_Conref* _temp1340;
void* _temp1342; void* _temp1344; void* _temp1346; void* _temp1348; void*
_temp1350; void* _temp1352; void* _temp1354; void* _temp1356; void* _temp1358;
void* _temp1360; void* _temp1362; void* _temp1364; void* _temp1366; struct Cyc_Absyn_Exp*
_temp1368; struct Cyc_Absyn_Tqual _temp1370; void* _temp1372; void* _temp1374;
struct Cyc_Absyn_Exp* _temp1376; struct Cyc_Absyn_Tqual _temp1378; void*
_temp1380; void* _temp1382; struct Cyc_Absyn_FnInfo _temp1384; struct Cyc_List_List*
_temp1386; struct Cyc_List_List* _temp1388; struct Cyc_Absyn_VarargInfo*
_temp1390; int _temp1392; struct Cyc_List_List* _temp1394; void* _temp1396;
struct Cyc_Core_Opt* _temp1398; struct Cyc_List_List* _temp1400; void* _temp1402;
struct Cyc_Absyn_FnInfo _temp1404; struct Cyc_List_List* _temp1406; struct Cyc_List_List*
_temp1408; struct Cyc_Absyn_VarargInfo* _temp1410; int _temp1412; struct Cyc_List_List*
_temp1414; void* _temp1416; struct Cyc_Core_Opt* _temp1418; struct Cyc_List_List*
_temp1420; void* _temp1422; struct Cyc_List_List* _temp1424; void* _temp1426;
struct Cyc_List_List* _temp1428; void* _temp1430; struct Cyc_List_List*
_temp1432; void* _temp1434; struct Cyc_List_List* _temp1436; void* _temp1438;
struct Cyc_List_List* _temp1440; void* _temp1442; struct Cyc_List_List*
_temp1444; void* _temp1446; void* _temp1448; void* _temp1450; void* _temp1452;
void* _temp1454; void* _temp1456; void* _temp1458; void* _temp1460; void*
_temp1462; void* _temp1464; void* _temp1466; void* _temp1468; _LL1162: _LL1215:
_temp1214= _temp1160.f1; if(( unsigned int) _temp1214 >  4u?*(( int*) _temp1214)
==  Cyc_Absyn_Evar: 0){ goto _LL1163;} else{ goto _LL1164;} _LL1164: _LL1219:
_temp1218= _temp1160.f1; if( _temp1218 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1217;} else{ goto _LL1166;} _LL1217: _temp1216= _temp1160.f2; if( _temp1216
== ( void*) Cyc_Absyn_VoidType){ goto _LL1165;} else{ goto _LL1166;} _LL1166:
_LL1225: _temp1224= _temp1160.f1; if(( unsigned int) _temp1224 >  4u?*(( int*)
_temp1224) ==  Cyc_Absyn_VarType: 0){ _LL1227: _temp1226=(( struct Cyc_Absyn_VarType_struct*)
_temp1224)->f1; goto _LL1221;} else{ goto _LL1168;} _LL1221: _temp1220=
_temp1160.f2; if(( unsigned int) _temp1220 >  4u?*(( int*) _temp1220) ==  Cyc_Absyn_VarType:
0){ _LL1223: _temp1222=(( struct Cyc_Absyn_VarType_struct*) _temp1220)->f1; goto
_LL1167;} else{ goto _LL1168;} _LL1168: _LL1237: _temp1236= _temp1160.f1; if((
unsigned int) _temp1236 >  4u?*(( int*) _temp1236) ==  Cyc_Absyn_StructType: 0){
_LL1243: _temp1242=(( struct Cyc_Absyn_StructType_struct*) _temp1236)->f1; goto
_LL1241; _LL1241: _temp1240=(( struct Cyc_Absyn_StructType_struct*) _temp1236)->f2;
goto _LL1239; _LL1239: _temp1238=(( struct Cyc_Absyn_StructType_struct*)
_temp1236)->f3; goto _LL1229;} else{ goto _LL1170;} _LL1229: _temp1228=
_temp1160.f2; if(( unsigned int) _temp1228 >  4u?*(( int*) _temp1228) ==  Cyc_Absyn_StructType:
0){ _LL1235: _temp1234=(( struct Cyc_Absyn_StructType_struct*) _temp1228)->f1;
goto _LL1233; _LL1233: _temp1232=(( struct Cyc_Absyn_StructType_struct*)
_temp1228)->f2; goto _LL1231; _LL1231: _temp1230=(( struct Cyc_Absyn_StructType_struct*)
_temp1228)->f3; goto _LL1169;} else{ goto _LL1170;} _LL1170: _LL1249: _temp1248=
_temp1160.f1; if(( unsigned int) _temp1248 >  4u?*(( int*) _temp1248) ==  Cyc_Absyn_EnumType:
0){ _LL1251: _temp1250=(( struct Cyc_Absyn_EnumType_struct*) _temp1248)->f1;
goto _LL1245;} else{ goto _LL1172;} _LL1245: _temp1244= _temp1160.f2; if((
unsigned int) _temp1244 >  4u?*(( int*) _temp1244) ==  Cyc_Absyn_EnumType: 0){
_LL1247: _temp1246=(( struct Cyc_Absyn_EnumType_struct*) _temp1244)->f1; goto
_LL1171;} else{ goto _LL1172;} _LL1172: _LL1261: _temp1260= _temp1160.f1; if((
unsigned int) _temp1260 >  4u?*(( int*) _temp1260) ==  Cyc_Absyn_UnionType: 0){
_LL1267: _temp1266=(( struct Cyc_Absyn_UnionType_struct*) _temp1260)->f1; goto
_LL1265; _LL1265: _temp1264=(( struct Cyc_Absyn_UnionType_struct*) _temp1260)->f2;
goto _LL1263; _LL1263: _temp1262=(( struct Cyc_Absyn_UnionType_struct*)
_temp1260)->f3; goto _LL1253;} else{ goto _LL1174;} _LL1253: _temp1252=
_temp1160.f2; if(( unsigned int) _temp1252 >  4u?*(( int*) _temp1252) ==  Cyc_Absyn_UnionType:
0){ _LL1259: _temp1258=(( struct Cyc_Absyn_UnionType_struct*) _temp1252)->f1;
goto _LL1257; _LL1257: _temp1256=(( struct Cyc_Absyn_UnionType_struct*)
_temp1252)->f2; goto _LL1255; _LL1255: _temp1254=(( struct Cyc_Absyn_UnionType_struct*)
_temp1252)->f3; goto _LL1173;} else{ goto _LL1174;} _LL1174: _LL1282: _temp1281=
_temp1160.f1; if(( unsigned int) _temp1281 >  4u?*(( int*) _temp1281) ==  Cyc_Absyn_TunionType:
0){ _LL1284: _temp1283=(( struct Cyc_Absyn_TunionType_struct*) _temp1281)->f1;
_LL1290: _temp1289=( void*) _temp1283.tunion_info; if(*(( int*) _temp1289) == 
Cyc_Absyn_KnownTunion){ _LL1292: _temp1291=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1289)->f1; _temp1293=* _temp1291; goto _LL1288;} else{ goto _LL1176;}
_LL1288: _temp1287= _temp1283.targs; goto _LL1286; _LL1286: _temp1285=( void*)
_temp1283.rgn; goto _LL1269;} else{ goto _LL1176;} _LL1269: _temp1268= _temp1160.f2;
if(( unsigned int) _temp1268 >  4u?*(( int*) _temp1268) ==  Cyc_Absyn_TunionType:
0){ _LL1271: _temp1270=(( struct Cyc_Absyn_TunionType_struct*) _temp1268)->f1;
_LL1277: _temp1276=( void*) _temp1270.tunion_info; if(*(( int*) _temp1276) == 
Cyc_Absyn_KnownTunion){ _LL1279: _temp1278=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1276)->f1; _temp1280=* _temp1278; goto _LL1275;} else{ goto _LL1176;}
_LL1275: _temp1274= _temp1270.targs; goto _LL1273; _LL1273: _temp1272=( void*)
_temp1270.rgn; goto _LL1175;} else{ goto _LL1176;} _LL1176: _LL1307: _temp1306=
_temp1160.f1; if(( unsigned int) _temp1306 >  4u?*(( int*) _temp1306) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1309: _temp1308=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1306)->f1;
_LL1313: _temp1312=( void*) _temp1308.field_info; if(*(( int*) _temp1312) == 
Cyc_Absyn_KnownTunionfield){ _LL1317: _temp1316=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1312)->f1; goto _LL1315; _LL1315: _temp1314=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1312)->f2; goto _LL1311;} else{ goto _LL1178;} _LL1311: _temp1310=
_temp1308.targs; goto _LL1295;} else{ goto _LL1178;} _LL1295: _temp1294=
_temp1160.f2; if(( unsigned int) _temp1294 >  4u?*(( int*) _temp1294) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1297: _temp1296=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1294)->f1;
_LL1301: _temp1300=( void*) _temp1296.field_info; if(*(( int*) _temp1300) == 
Cyc_Absyn_KnownTunionfield){ _LL1305: _temp1304=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1300)->f1; goto _LL1303; _LL1303: _temp1302=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1300)->f2; goto _LL1299;} else{ goto _LL1178;} _LL1299: _temp1298=
_temp1296.targs; goto _LL1177;} else{ goto _LL1178;} _LL1178: _LL1333: _temp1332=
_temp1160.f1; if(( unsigned int) _temp1332 >  4u?*(( int*) _temp1332) ==  Cyc_Absyn_PointerType:
0){ _LL1335: _temp1334=(( struct Cyc_Absyn_PointerType_struct*) _temp1332)->f1;
_LL1345: _temp1344=( void*) _temp1334.elt_typ; goto _LL1343; _LL1343: _temp1342=(
void*) _temp1334.rgn_typ; goto _LL1341; _LL1341: _temp1340= _temp1334.nullable;
goto _LL1339; _LL1339: _temp1338= _temp1334.tq; goto _LL1337; _LL1337: _temp1336=
_temp1334.bounds; goto _LL1319;} else{ goto _LL1180;} _LL1319: _temp1318=
_temp1160.f2; if(( unsigned int) _temp1318 >  4u?*(( int*) _temp1318) ==  Cyc_Absyn_PointerType:
0){ _LL1321: _temp1320=(( struct Cyc_Absyn_PointerType_struct*) _temp1318)->f1;
_LL1331: _temp1330=( void*) _temp1320.elt_typ; goto _LL1329; _LL1329: _temp1328=(
void*) _temp1320.rgn_typ; goto _LL1327; _LL1327: _temp1326= _temp1320.nullable;
goto _LL1325; _LL1325: _temp1324= _temp1320.tq; goto _LL1323; _LL1323: _temp1322=
_temp1320.bounds; goto _LL1179;} else{ goto _LL1180;} _LL1180: _LL1353:
_temp1352= _temp1160.f1; if(( unsigned int) _temp1352 >  4u?*(( int*) _temp1352)
==  Cyc_Absyn_IntType: 0){ _LL1357: _temp1356=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1352)->f1; goto _LL1355; _LL1355: _temp1354=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1352)->f2; goto _LL1347;} else{ goto _LL1182;} _LL1347: _temp1346=
_temp1160.f2; if(( unsigned int) _temp1346 >  4u?*(( int*) _temp1346) ==  Cyc_Absyn_IntType:
0){ _LL1351: _temp1350=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1346)->f1;
goto _LL1349; _LL1349: _temp1348=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1346)->f2; goto _LL1181;} else{ goto _LL1182;} _LL1182: _LL1361: _temp1360=
_temp1160.f1; if( _temp1360 == ( void*) Cyc_Absyn_FloatType){ goto _LL1359;}
else{ goto _LL1184;} _LL1359: _temp1358= _temp1160.f2; if( _temp1358 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1183;} else{ goto _LL1184;} _LL1184: _LL1365:
_temp1364= _temp1160.f1; if( _temp1364 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1363;} else{ goto _LL1186;} _LL1363: _temp1362= _temp1160.f2; if( _temp1362
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1185;} else{ goto _LL1186;} _LL1186:
_LL1375: _temp1374= _temp1160.f1; if(( unsigned int) _temp1374 >  4u?*(( int*)
_temp1374) ==  Cyc_Absyn_ArrayType: 0){ _LL1381: _temp1380=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1374)->f1; goto _LL1379; _LL1379: _temp1378=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1374)->f2; goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1374)->f3; goto _LL1367;} else{ goto _LL1188;} _LL1367: _temp1366=
_temp1160.f2; if(( unsigned int) _temp1366 >  4u?*(( int*) _temp1366) ==  Cyc_Absyn_ArrayType:
0){ _LL1373: _temp1372=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1366)->f1;
goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1366)->f2; goto _LL1369; _LL1369: _temp1368=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1366)->f3; goto _LL1187;} else{ goto _LL1188;} _LL1188: _LL1403: _temp1402=
_temp1160.f1; if(( unsigned int) _temp1402 >  4u?*(( int*) _temp1402) ==  Cyc_Absyn_FnType:
0){ _LL1405: _temp1404=(( struct Cyc_Absyn_FnType_struct*) _temp1402)->f1;
_LL1421: _temp1420= _temp1404.tvars; goto _LL1419; _LL1419: _temp1418= _temp1404.effect;
goto _LL1417; _LL1417: _temp1416=( void*) _temp1404.ret_typ; goto _LL1415;
_LL1415: _temp1414= _temp1404.args; goto _LL1413; _LL1413: _temp1412= _temp1404.c_varargs;
goto _LL1411; _LL1411: _temp1410= _temp1404.cyc_varargs; goto _LL1409; _LL1409:
_temp1408= _temp1404.rgn_po; goto _LL1407; _LL1407: _temp1406= _temp1404.attributes;
goto _LL1383;} else{ goto _LL1190;} _LL1383: _temp1382= _temp1160.f2; if((
unsigned int) _temp1382 >  4u?*(( int*) _temp1382) ==  Cyc_Absyn_FnType: 0){
_LL1385: _temp1384=(( struct Cyc_Absyn_FnType_struct*) _temp1382)->f1; _LL1401:
_temp1400= _temp1384.tvars; goto _LL1399; _LL1399: _temp1398= _temp1384.effect;
goto _LL1397; _LL1397: _temp1396=( void*) _temp1384.ret_typ; goto _LL1395;
_LL1395: _temp1394= _temp1384.args; goto _LL1393; _LL1393: _temp1392= _temp1384.c_varargs;
goto _LL1391; _LL1391: _temp1390= _temp1384.cyc_varargs; goto _LL1389; _LL1389:
_temp1388= _temp1384.rgn_po; goto _LL1387; _LL1387: _temp1386= _temp1384.attributes;
goto _LL1189;} else{ goto _LL1190;} _LL1190: _LL1427: _temp1426= _temp1160.f1;
if(( unsigned int) _temp1426 >  4u?*(( int*) _temp1426) ==  Cyc_Absyn_TupleType:
0){ _LL1429: _temp1428=(( struct Cyc_Absyn_TupleType_struct*) _temp1426)->f1;
goto _LL1423;} else{ goto _LL1192;} _LL1423: _temp1422= _temp1160.f2; if((
unsigned int) _temp1422 >  4u?*(( int*) _temp1422) ==  Cyc_Absyn_TupleType: 0){
_LL1425: _temp1424=(( struct Cyc_Absyn_TupleType_struct*) _temp1422)->f1; goto
_LL1191;} else{ goto _LL1192;} _LL1192: _LL1435: _temp1434= _temp1160.f1; if((
unsigned int) _temp1434 >  4u?*(( int*) _temp1434) ==  Cyc_Absyn_AnonStructType:
0){ _LL1437: _temp1436=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1434)->f1;
goto _LL1431;} else{ goto _LL1194;} _LL1431: _temp1430= _temp1160.f2; if((
unsigned int) _temp1430 >  4u?*(( int*) _temp1430) ==  Cyc_Absyn_AnonStructType:
0){ _LL1433: _temp1432=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1430)->f1;
goto _LL1193;} else{ goto _LL1194;} _LL1194: _LL1443: _temp1442= _temp1160.f1;
if(( unsigned int) _temp1442 >  4u?*(( int*) _temp1442) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1445: _temp1444=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1442)->f1;
goto _LL1439;} else{ goto _LL1196;} _LL1439: _temp1438= _temp1160.f2; if((
unsigned int) _temp1438 >  4u?*(( int*) _temp1438) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1441: _temp1440=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1438)->f1;
goto _LL1195;} else{ goto _LL1196;} _LL1196: _LL1449: _temp1448= _temp1160.f1;
if( _temp1448 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1447;} else{ goto _LL1198;}
_LL1447: _temp1446= _temp1160.f2; if( _temp1446 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1197;} else{ goto _LL1198;} _LL1198: _LL1455: _temp1454= _temp1160.f1;
if(( unsigned int) _temp1454 >  4u?*(( int*) _temp1454) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1457: _temp1456=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1454)->f1; goto _LL1451;} else{ goto _LL1200;} _LL1451: _temp1450=
_temp1160.f2; if(( unsigned int) _temp1450 >  4u?*(( int*) _temp1450) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1453: _temp1452=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1450)->f1; goto _LL1199;} else{ goto _LL1200;} _LL1200: _LL1459: _temp1458=
_temp1160.f1; if(( unsigned int) _temp1458 >  4u?*(( int*) _temp1458) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1201;} else{ goto _LL1202;} _LL1202: _LL1461: _temp1460= _temp1160.f2;
if(( unsigned int) _temp1460 >  4u?*(( int*) _temp1460) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1203;} else{ goto _LL1204;} _LL1204: _LL1463: _temp1462= _temp1160.f1;
if(( unsigned int) _temp1462 >  4u?*(( int*) _temp1462) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1205;} else{ goto _LL1206;} _LL1206: _LL1465: _temp1464= _temp1160.f1;
if(( unsigned int) _temp1464 >  4u?*(( int*) _temp1464) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1207;} else{ goto _LL1208;} _LL1208: _LL1467: _temp1466= _temp1160.f2;
if(( unsigned int) _temp1466 >  4u?*(( int*) _temp1466) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1209;} else{ goto _LL1210;} _LL1210: _LL1469: _temp1468= _temp1160.f2;
if(( unsigned int) _temp1468 >  4u?*(( int*) _temp1468) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1211;} else{ goto _LL1212;} _LL1212: goto _LL1213; _LL1163: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1165: return; _LL1167: { struct _tagged_arr* _temp1470=
_temp1226->name; struct _tagged_arr* _temp1471= _temp1222->name; int _temp1472=*((
int*) _check_null( _temp1226->identity)); int _temp1473=*(( int*) _check_null(
_temp1222->identity)); void* _temp1474= Cyc_Absyn_conref_val( _temp1226->kind);
void* _temp1475= Cyc_Absyn_conref_val( _temp1222->kind); if( _temp1473 == 
_temp1472? Cyc_Std_zstrptrcmp( _temp1470, _temp1471) ==  0: 0){ if( _temp1474 != 
_temp1475){({ struct Cyc_Std_String_pa_struct _temp1479; _temp1479.tag= Cyc_Std_String_pa;
_temp1479.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1475);{ struct
Cyc_Std_String_pa_struct _temp1478; _temp1478.tag= Cyc_Std_String_pa; _temp1478.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1474);{ struct Cyc_Std_String_pa_struct
_temp1477; _temp1477.tag= Cyc_Std_String_pa; _temp1477.f1=( struct _tagged_arr)*
_temp1470;{ void* _temp1476[ 3u]={& _temp1477,& _temp1478,& _temp1479};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1476, sizeof( void*), 3u));}}}});}
return;} Cyc_Tcutil_failure_reason= _tag_arr("(variable types are not the same)",
sizeof( unsigned char), 34u); goto _LL1161;} _LL1169: if((( _temp1234 !=  0?
_temp1242 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1234),( struct _tuple1*) _check_null( _temp1242)) ==  0: 0)? 1:( _temp1234
==  0? _temp1242 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1232, _temp1240);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different struct types)", sizeof(
unsigned char), 25u); goto _LL1161; _LL1171: if( Cyc_Absyn_qvar_cmp( _temp1250,
_temp1246) ==  0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different enum types)",
sizeof( unsigned char), 23u); goto _LL1161; _LL1173: if((( _temp1258 !=  0?
_temp1266 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1258),( struct _tuple1*) _check_null( _temp1266)) ==  0: 0)? 1:( _temp1258
==  0? _temp1266 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1256, _temp1264);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different union types)", sizeof(
unsigned char), 24u); goto _LL1161; _LL1175: if( _temp1293 ==  _temp1280? 1: Cyc_Absyn_qvar_cmp(
_temp1293->name, _temp1280->name) ==  0){ Cyc_Tcutil_unify_it( _temp1272,
_temp1285); Cyc_Tcutil_unify_list( _temp1274, _temp1287); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)", sizeof( unsigned char), 25u); goto _LL1161;
_LL1177: if(( _temp1316 ==  _temp1304? 1: Cyc_Absyn_qvar_cmp( _temp1316->name,
_temp1304->name) ==  0)? _temp1314 ==  _temp1302? 1: Cyc_Absyn_qvar_cmp(
_temp1314->name, _temp1302->name) ==  0: 0){ Cyc_Tcutil_unify_list( _temp1298,
_temp1310); return;} Cyc_Tcutil_failure_reason= _tag_arr("(different tunion field types)",
sizeof( unsigned char), 31u); goto _LL1161; _LL1179: Cyc_Tcutil_unify_it(
_temp1330, _temp1344); Cyc_Tcutil_unify_it( _temp1342, _temp1328); Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_unify_tqual( _temp1324, _temp1338);
Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp, _temp1322, _temp1336,
_tag_arr("(different pointer bounds)", sizeof( unsigned char), 27u));{ void*
_temp1480=( void*)( Cyc_Absyn_compress_conref( _temp1322))->v; void* _temp1486;
_LL1482: if(( unsigned int) _temp1480 >  1u?*(( int*) _temp1480) ==  Cyc_Absyn_Eq_constr:
0){ _LL1487: _temp1486=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1480)->f1;
if( _temp1486 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1483;} else{ goto
_LL1484;}} else{ goto _LL1484;} _LL1484: goto _LL1485; _LL1483: return; _LL1485:
goto _LL1481; _LL1481:;}(( void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y, struct _tagged_arr reason)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1326, _temp1340, _tag_arr("(different pointer types)",
sizeof( unsigned char), 26u)); return; _LL1181: if( _temp1350 ==  _temp1356?
_temp1348 ==  _temp1354: 0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different integral types)",
sizeof( unsigned char), 27u); goto _LL1161; _LL1183: return; _LL1185: return;
_LL1187: Cyc_Tcutil_unify_tqual( _temp1370, _temp1378); Cyc_Tcutil_unify_it(
_temp1372, _temp1380); if( _temp1376 ==  _temp1368){ return;} if( _temp1376 == 
0? 1: _temp1368 ==  0){ goto _LL1161;} if( Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1376)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1368))){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)", sizeof( unsigned char), 24u); goto _LL1161;
_LL1189: { int done= 0;{ struct _RegionHandle _temp1488= _new_region(); struct
_RegionHandle* rgn=& _temp1488; _push_region( rgn);{ struct Cyc_List_List* inst=
0; while( _temp1400 !=  0) { if( _temp1420 ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too few type variables)", sizeof(
unsigned char), 50u); break;} inst=({ struct Cyc_List_List* _temp1489=( struct
Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1489->hd=(
void*)({ struct _tuple5* _temp1490=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp1490->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1420))->hd; _temp1490->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1491=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1491[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1492; _temp1492.tag= Cyc_Absyn_VarType;
_temp1492.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1400))->hd; _temp1492;}); _temp1491;}); _temp1490;}); _temp1489->tl= inst;
_temp1489;}); _temp1400=(( struct Cyc_List_List*) _check_null( _temp1400))->tl;
_temp1420=(( struct Cyc_List_List*) _check_null( _temp1420))->tl;} if( _temp1420
!=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(second function type has too many type variables)",
sizeof( unsigned char), 51u); _npop_handler( 0u); goto _LL1161;} if( inst !=  0){
Cyc_Tcutil_unify_it(( void*)({ struct Cyc_Absyn_FnType_struct* _temp1493=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1493[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1494; _temp1494.tag= Cyc_Absyn_FnType;
_temp1494.f1=({ struct Cyc_Absyn_FnInfo _temp1495; _temp1495.tvars= 0; _temp1495.effect=
_temp1398; _temp1495.ret_typ=( void*) _temp1396; _temp1495.args= _temp1394;
_temp1495.c_varargs= _temp1392; _temp1495.cyc_varargs= _temp1390; _temp1495.rgn_po=
_temp1388; _temp1495.attributes= _temp1386; _temp1495;}); _temp1494;});
_temp1493;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1496=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1496[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1497; _temp1497.tag= Cyc_Absyn_FnType;
_temp1497.f1=({ struct Cyc_Absyn_FnInfo _temp1498; _temp1498.tvars= 0; _temp1498.effect=
_temp1418; _temp1498.ret_typ=( void*) _temp1416; _temp1498.args= _temp1414;
_temp1498.c_varargs= _temp1412; _temp1498.cyc_varargs= _temp1410; _temp1498.rgn_po=
_temp1408; _temp1498.attributes= _temp1406; _temp1498;}); _temp1497;});
_temp1496;}))); done= 1;}}; _pop_region( rgn);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp1396, _temp1416); for( 0; _temp1394 !=  0? _temp1414 !=  0: 0;( _temp1394=((
struct Cyc_List_List*) _check_null( _temp1394))->tl, _temp1414=(( struct Cyc_List_List*)
_check_null( _temp1414))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1394))->hd)).f2,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1414))->hd)).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1394))->hd)).f3,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1414))->hd)).f3);}
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; if( _temp1394 !=  0? 1:
_temp1414 !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different number of arguments)",
sizeof( unsigned char), 52u); goto _LL1161;} if( _temp1392 !=  _temp1412){ Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)", sizeof( unsigned char), 41u);
goto _LL1161;}{ int bad_cyc_vararg= 0;{ struct _tuple11 _temp1500=({ struct
_tuple11 _temp1499; _temp1499.f1= _temp1390; _temp1499.f2= _temp1410; _temp1499;});
struct Cyc_Absyn_VarargInfo* _temp1510; struct Cyc_Absyn_VarargInfo* _temp1512;
struct Cyc_Absyn_VarargInfo* _temp1514; struct Cyc_Absyn_VarargInfo* _temp1516;
struct Cyc_Absyn_VarargInfo* _temp1518; struct Cyc_Absyn_VarargInfo _temp1520;
int _temp1521; void* _temp1523; struct Cyc_Absyn_Tqual _temp1525; struct Cyc_Core_Opt*
_temp1527; struct Cyc_Absyn_VarargInfo* _temp1529; struct Cyc_Absyn_VarargInfo
_temp1531; int _temp1532; void* _temp1534; struct Cyc_Absyn_Tqual _temp1536;
struct Cyc_Core_Opt* _temp1538; _LL1502: _LL1513: _temp1512= _temp1500.f1; if(
_temp1512 ==  0){ goto _LL1511;} else{ goto _LL1504;} _LL1511: _temp1510=
_temp1500.f2; if( _temp1510 ==  0){ goto _LL1503;} else{ goto _LL1504;} _LL1504:
_LL1515: _temp1514= _temp1500.f1; if( _temp1514 ==  0){ goto _LL1505;} else{
goto _LL1506;} _LL1506: _LL1517: _temp1516= _temp1500.f2; if( _temp1516 ==  0){
goto _LL1507;} else{ goto _LL1508;} _LL1508: _LL1530: _temp1529= _temp1500.f1;
if( _temp1529 ==  0){ goto _LL1501;} else{ _temp1531=* _temp1529; _LL1539:
_temp1538= _temp1531.name; goto _LL1537; _LL1537: _temp1536= _temp1531.tq; goto
_LL1535; _LL1535: _temp1534=( void*) _temp1531.type; goto _LL1533; _LL1533:
_temp1532= _temp1531.inject; goto _LL1519;} _LL1519: _temp1518= _temp1500.f2;
if( _temp1518 ==  0){ goto _LL1501;} else{ _temp1520=* _temp1518; _LL1528:
_temp1527= _temp1520.name; goto _LL1526; _LL1526: _temp1525= _temp1520.tq; goto
_LL1524; _LL1524: _temp1523=( void*) _temp1520.type; goto _LL1522; _LL1522:
_temp1521= _temp1520.inject; goto _LL1509;} _LL1503: goto _LL1501; _LL1505: goto
_LL1507; _LL1507: bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type takes varargs)",
sizeof( unsigned char), 39u); goto _LL1501; _LL1509: Cyc_Tcutil_unify_tqual(
_temp1536, _temp1525); Cyc_Tcutil_unify_it( _temp1534, _temp1523); if( _temp1532
!=  _temp1521){ bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type injects varargs)",
sizeof( unsigned char), 41u);} goto _LL1501; _LL1501:;} if( bad_cyc_vararg){
goto _LL1161;}{ int bad_effect= 0;{ struct _tuple12 _temp1541=({ struct _tuple12
_temp1540; _temp1540.f1= _temp1398; _temp1540.f2= _temp1418; _temp1540;});
struct Cyc_Core_Opt* _temp1551; struct Cyc_Core_Opt* _temp1553; struct Cyc_Core_Opt*
_temp1555; struct Cyc_Core_Opt* _temp1557; _LL1543: _LL1554: _temp1553=
_temp1541.f1; if( _temp1553 ==  0){ goto _LL1552;} else{ goto _LL1545;} _LL1552:
_temp1551= _temp1541.f2; if( _temp1551 ==  0){ goto _LL1544;} else{ goto _LL1545;}
_LL1545: _LL1556: _temp1555= _temp1541.f1; if( _temp1555 ==  0){ goto _LL1546;}
else{ goto _LL1547;} _LL1547: _LL1558: _temp1557= _temp1541.f2; if( _temp1557 == 
0){ goto _LL1548;} else{ goto _LL1549;} _LL1549: goto _LL1550; _LL1544: goto
_LL1542; _LL1546: goto _LL1548; _LL1548: bad_effect= 1; goto _LL1542; _LL1550:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1398))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1418))->v); goto _LL1542; _LL1542:;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; if( bad_effect){ Cyc_Tcutil_failure_reason= _tag_arr("(function type effects do not unify)",
sizeof( unsigned char), 37u); goto _LL1161;} if( ! Cyc_Tcutil_same_atts(
_temp1406, _temp1386)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different attributes)",
sizeof( unsigned char), 43u); goto _LL1161;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1408, _temp1388)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different region lifetime orderings)",
sizeof( unsigned char), 58u); goto _LL1161;} return;}}} _LL1191: for( 0;
_temp1424 !=  0? _temp1428 !=  0: 0;( _temp1424=(( struct Cyc_List_List*)
_check_null( _temp1424))->tl, _temp1428=(( struct Cyc_List_List*) _check_null(
_temp1428))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1424))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1428))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1424))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1428))->hd)).f2);} if( _temp1424 ==  0?
_temp1428 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(tuple types have different numbers of components)",
sizeof( unsigned char), 51u); goto _LL1161; _LL1193: _temp1444= _temp1436;
_temp1440= _temp1432; goto _LL1195; _LL1195: for( 0; _temp1440 !=  0? _temp1444
!=  0: 0;( _temp1440=(( struct Cyc_List_List*) _check_null( _temp1440))->tl,
_temp1444=(( struct Cyc_List_List*) _check_null( _temp1444))->tl)){ struct Cyc_Absyn_Structfield*
_temp1559=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1440))->hd; struct Cyc_Absyn_Structfield* _temp1560=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1444))->hd; if( Cyc_Std_zstrptrcmp(
_temp1559->name, _temp1560->name) !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(different member names in anonymous structs/unions)",
sizeof( unsigned char), 53u);( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1559->tq, _temp1560->tq); Cyc_Tcutil_unify_it(( void*) _temp1559->type,(
void*) _temp1560->type); if( ! Cyc_Tcutil_same_atts( _temp1559->attributes,
_temp1560->attributes)){ Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2;
Cyc_Tcutil_failure_reason= _tag_arr("(different attributes on member)", sizeof(
unsigned char), 33u);( int) _throw(( void*) Cyc_Tcutil_Unify);} if((( _temp1559->width
!=  0? _temp1560->width ==  0: 0)? 1:( _temp1560->width !=  0? _temp1559->width
==  0: 0))? 1:(( _temp1559->width !=  0? _temp1560->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1559->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1560->width)): 0)){ Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason= _tag_arr("(different bitfield widths on member)",
sizeof( unsigned char), 38u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} if(
_temp1440 ==  0? _temp1444 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(different number of members)", sizeof(
unsigned char), 30u); goto _LL1161; _LL1197: return; _LL1199: Cyc_Tcutil_unify_it(
_temp1456, _temp1452); return; _LL1201: goto _LL1203; _LL1203: goto _LL1205;
_LL1205: goto _LL1207; _LL1207: goto _LL1209; _LL1209: goto _LL1211; _LL1211:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)", sizeof( unsigned char), 22u); goto _LL1161;
_LL1213: goto _LL1161; _LL1161:;}( int) _throw(( void*) Cyc_Tcutil_Unify);} int
Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1561= Cyc_Tcutil_compress( t);
_LL1563: if(( unsigned int) _temp1561 >  4u?*(( int*) _temp1561) ==  Cyc_Absyn_IntType:
0){ goto _LL1564;} else{ goto _LL1565;} _LL1565: if( _temp1561 == ( void*) Cyc_Absyn_FloatType){
goto _LL1566;} else{ goto _LL1567;} _LL1567: if( _temp1561 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1568;} else{ goto _LL1569;} _LL1569: if(( unsigned int) _temp1561 >  4u?*((
int*) _temp1561) ==  Cyc_Absyn_EnumType: 0){ goto _LL1570;} else{ goto _LL1571;}
_LL1571: goto _LL1572; _LL1564: goto _LL1566; _LL1566: goto _LL1568; _LL1568:
goto _LL1570; _LL1570: return 1; _LL1572: return 0; _LL1562:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1574=({ struct _tuple8 _temp1573; _temp1573.f1= t1;
_temp1573.f2= t2; _temp1573;}); void* _temp1596; void* _temp1598; void*
_temp1600; void* _temp1602; void* _temp1604; void* _temp1606; void* _temp1608;
void* _temp1610; void* _temp1612; void* _temp1614; void* _temp1616; void*
_temp1618; void* _temp1620; void* _temp1622; void* _temp1624; void* _temp1626;
void* _temp1628; void* _temp1630; void* _temp1632; void* _temp1634; void*
_temp1636; void* _temp1638; void* _temp1640; void* _temp1642; void* _temp1644;
void* _temp1646; void* _temp1648; _LL1576: _LL1599: _temp1598= _temp1574.f1; if(
_temp1598 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1597;} else{ goto _LL1578;}
_LL1597: _temp1596= _temp1574.f2; if( _temp1596 == ( void*) Cyc_Absyn_FloatType){
goto _LL1577;} else{ goto _LL1578;} _LL1578: _LL1603: _temp1602= _temp1574.f1;
if( _temp1602 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1601;} else{ goto
_LL1580;} _LL1601: _temp1600= _temp1574.f2; if(( unsigned int) _temp1600 >  4u?*((
int*) _temp1600) ==  Cyc_Absyn_IntType: 0){ goto _LL1579;} else{ goto _LL1580;}
_LL1580: _LL1607: _temp1606= _temp1574.f1; if( _temp1606 == ( void*) Cyc_Absyn_FloatType){
goto _LL1605;} else{ goto _LL1582;} _LL1605: _temp1604= _temp1574.f2; if((
unsigned int) _temp1604 >  4u?*(( int*) _temp1604) ==  Cyc_Absyn_IntType: 0){
goto _LL1581;} else{ goto _LL1582;} _LL1582: _LL1613: _temp1612= _temp1574.f1;
if(( unsigned int) _temp1612 >  4u?*(( int*) _temp1612) ==  Cyc_Absyn_IntType: 0){
_LL1615: _temp1614=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1612)->f2;
if( _temp1614 == ( void*) Cyc_Absyn_B8){ goto _LL1609;} else{ goto _LL1584;}}
else{ goto _LL1584;} _LL1609: _temp1608= _temp1574.f2; if(( unsigned int)
_temp1608 >  4u?*(( int*) _temp1608) ==  Cyc_Absyn_IntType: 0){ _LL1611:
_temp1610=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1608)->f2; if(
_temp1610 == ( void*) Cyc_Absyn_B8){ goto _LL1583;} else{ goto _LL1584;}} else{
goto _LL1584;} _LL1584: _LL1617: _temp1616= _temp1574.f1; if(( unsigned int)
_temp1616 >  4u?*(( int*) _temp1616) ==  Cyc_Absyn_IntType: 0){ _LL1619:
_temp1618=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1616)->f2; if(
_temp1618 == ( void*) Cyc_Absyn_B8){ goto _LL1585;} else{ goto _LL1586;}} else{
goto _LL1586;} _LL1586: _LL1623: _temp1622= _temp1574.f1; if(( unsigned int)
_temp1622 >  4u?*(( int*) _temp1622) ==  Cyc_Absyn_IntType: 0){ _LL1625:
_temp1624=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1622)->f2; if(
_temp1624 == ( void*) Cyc_Absyn_B4){ goto _LL1621;} else{ goto _LL1588;}} else{
goto _LL1588;} _LL1621: _temp1620= _temp1574.f2; if( _temp1620 == ( void*) Cyc_Absyn_FloatType){
goto _LL1587;} else{ goto _LL1588;} _LL1588: _LL1631: _temp1630= _temp1574.f1;
if(( unsigned int) _temp1630 >  4u?*(( int*) _temp1630) ==  Cyc_Absyn_IntType: 0){
_LL1633: _temp1632=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1630)->f2;
if( _temp1632 == ( void*) Cyc_Absyn_B4){ goto _LL1627;} else{ goto _LL1590;}}
else{ goto _LL1590;} _LL1627: _temp1626= _temp1574.f2; if(( unsigned int)
_temp1626 >  4u?*(( int*) _temp1626) ==  Cyc_Absyn_IntType: 0){ _LL1629:
_temp1628=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1626)->f2; if(
_temp1628 == ( void*) Cyc_Absyn_B2){ goto _LL1589;} else{ goto _LL1590;}} else{
goto _LL1590;} _LL1590: _LL1639: _temp1638= _temp1574.f1; if(( unsigned int)
_temp1638 >  4u?*(( int*) _temp1638) ==  Cyc_Absyn_IntType: 0){ _LL1641:
_temp1640=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1638)->f2; if(
_temp1640 == ( void*) Cyc_Absyn_B4){ goto _LL1635;} else{ goto _LL1592;}} else{
goto _LL1592;} _LL1635: _temp1634= _temp1574.f2; if(( unsigned int) _temp1634 > 
4u?*(( int*) _temp1634) ==  Cyc_Absyn_IntType: 0){ _LL1637: _temp1636=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1634)->f2; if( _temp1636 == ( void*) Cyc_Absyn_B1){
goto _LL1591;} else{ goto _LL1592;}} else{ goto _LL1592;} _LL1592: _LL1647:
_temp1646= _temp1574.f1; if(( unsigned int) _temp1646 >  4u?*(( int*) _temp1646)
==  Cyc_Absyn_IntType: 0){ _LL1649: _temp1648=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1646)->f2; if( _temp1648 == ( void*) Cyc_Absyn_B2){ goto _LL1643;} else{
goto _LL1594;}} else{ goto _LL1594;} _LL1643: _temp1642= _temp1574.f2; if((
unsigned int) _temp1642 >  4u?*(( int*) _temp1642) ==  Cyc_Absyn_IntType: 0){
_LL1645: _temp1644=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1642)->f2;
if( _temp1644 == ( void*) Cyc_Absyn_B1){ goto _LL1593;} else{ goto _LL1594;}}
else{ goto _LL1594;} _LL1594: goto _LL1595; _LL1577: goto _LL1579; _LL1579: goto
_LL1581; _LL1581: return 1; _LL1583: return 0; _LL1585: goto _LL1587; _LL1587:
goto _LL1589; _LL1589: goto _LL1591; _LL1591: goto _LL1593; _LL1593: return 1;
_LL1595: return 0; _LL1575:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1650=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1650->v=( void*)
t1; _temp1650;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify( t,(
void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp1653; _temp1653.tag= Cyc_Std_String_pa; _temp1653.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1652; _temp1652.tag= Cyc_Std_String_pa;
_temp1652.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1651[
2u]={& _temp1652,& _temp1653}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1651, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1654= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1656: if(( unsigned int) _temp1654 >  4u?*(( int*) _temp1654) ==  Cyc_Absyn_PointerType:
0){ goto _LL1657;} else{ goto _LL1658;} _LL1658: goto _LL1659; _LL1657: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1655; _LL1659: return 0; _LL1655:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1660= Cyc_Tcutil_compress(
t); _LL1662: if(( unsigned int) _temp1660 >  4u?*(( int*) _temp1660) ==  Cyc_Absyn_IntType:
0){ goto _LL1663;} else{ goto _LL1664;} _LL1664: if(( unsigned int) _temp1660 > 
4u?*(( int*) _temp1660) ==  Cyc_Absyn_EnumType: 0){ goto _LL1665;} else{ goto
_LL1666;} _LL1666: goto _LL1667; _LL1663: goto _LL1665; _LL1665: return 1;
_LL1667: return 0; _LL1661:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1668[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1668, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1669[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1669, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1671=({ struct _tuple8 _temp1670; _temp1670.f1= t1; _temp1670.f2= t2;
_temp1670;}); void* _temp1683; struct Cyc_Absyn_PtrInfo _temp1685; void*
_temp1687; struct Cyc_Absyn_PtrInfo _temp1689; void* _temp1691; struct Cyc_Absyn_Exp*
_temp1693; struct Cyc_Absyn_Tqual _temp1695; void* _temp1697; void* _temp1699;
struct Cyc_Absyn_Exp* _temp1701; struct Cyc_Absyn_Tqual _temp1703; void*
_temp1705; void* _temp1707; struct Cyc_Absyn_TunionInfo _temp1709; struct Cyc_List_List*
_temp1711; void* _temp1713; struct Cyc_Absyn_Tuniondecl** _temp1715; struct Cyc_Absyn_Tuniondecl*
_temp1717; void* _temp1718; struct Cyc_Absyn_TunionFieldInfo _temp1720; struct
Cyc_List_List* _temp1722; void* _temp1724; struct Cyc_Absyn_Tunionfield*
_temp1726; struct Cyc_Absyn_Tuniondecl* _temp1728; void* _temp1730; struct Cyc_Absyn_TunionInfo
_temp1732; void* _temp1734; struct Cyc_List_List* _temp1736; void* _temp1738;
struct Cyc_Absyn_Tuniondecl** _temp1740; struct Cyc_Absyn_Tuniondecl* _temp1742;
void* _temp1743; struct Cyc_Absyn_PtrInfo _temp1745; struct Cyc_Absyn_Conref*
_temp1747; struct Cyc_Absyn_Tqual _temp1749; struct Cyc_Absyn_Conref* _temp1751;
void* _temp1753; void* _temp1755; _LL1673: _LL1688: _temp1687= _temp1671.f1; if((
unsigned int) _temp1687 >  4u?*(( int*) _temp1687) ==  Cyc_Absyn_PointerType: 0){
_LL1690: _temp1689=(( struct Cyc_Absyn_PointerType_struct*) _temp1687)->f1; goto
_LL1684;} else{ goto _LL1675;} _LL1684: _temp1683= _temp1671.f2; if((
unsigned int) _temp1683 >  4u?*(( int*) _temp1683) ==  Cyc_Absyn_PointerType: 0){
_LL1686: _temp1685=(( struct Cyc_Absyn_PointerType_struct*) _temp1683)->f1; goto
_LL1674;} else{ goto _LL1675;} _LL1675: _LL1700: _temp1699= _temp1671.f1; if((
unsigned int) _temp1699 >  4u?*(( int*) _temp1699) ==  Cyc_Absyn_ArrayType: 0){
_LL1706: _temp1705=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1699)->f1;
goto _LL1704; _LL1704: _temp1703=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1699)->f2; goto _LL1702; _LL1702: _temp1701=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1699)->f3; goto _LL1692;} else{ goto _LL1677;} _LL1692: _temp1691=
_temp1671.f2; if(( unsigned int) _temp1691 >  4u?*(( int*) _temp1691) ==  Cyc_Absyn_ArrayType:
0){ _LL1698: _temp1697=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1691)->f1;
goto _LL1696; _LL1696: _temp1695=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1691)->f2; goto _LL1694; _LL1694: _temp1693=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1691)->f3; goto _LL1676;} else{ goto _LL1677;} _LL1677: _LL1719: _temp1718=
_temp1671.f1; if(( unsigned int) _temp1718 >  4u?*(( int*) _temp1718) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1721: _temp1720=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1718)->f1;
_LL1725: _temp1724=( void*) _temp1720.field_info; if(*(( int*) _temp1724) == 
Cyc_Absyn_KnownTunionfield){ _LL1729: _temp1728=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1724)->f1; goto _LL1727; _LL1727: _temp1726=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1724)->f2; goto _LL1723;} else{ goto _LL1679;} _LL1723: _temp1722=
_temp1720.targs; goto _LL1708;} else{ goto _LL1679;} _LL1708: _temp1707=
_temp1671.f2; if(( unsigned int) _temp1707 >  4u?*(( int*) _temp1707) ==  Cyc_Absyn_TunionType:
0){ _LL1710: _temp1709=(( struct Cyc_Absyn_TunionType_struct*) _temp1707)->f1;
_LL1714: _temp1713=( void*) _temp1709.tunion_info; if(*(( int*) _temp1713) == 
Cyc_Absyn_KnownTunion){ _LL1716: _temp1715=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1713)->f1; _temp1717=* _temp1715; goto _LL1712;} else{ goto _LL1679;}
_LL1712: _temp1711= _temp1709.targs; goto _LL1678;} else{ goto _LL1679;} _LL1679:
_LL1744: _temp1743= _temp1671.f1; if(( unsigned int) _temp1743 >  4u?*(( int*)
_temp1743) ==  Cyc_Absyn_PointerType: 0){ _LL1746: _temp1745=(( struct Cyc_Absyn_PointerType_struct*)
_temp1743)->f1; _LL1756: _temp1755=( void*) _temp1745.elt_typ; goto _LL1754;
_LL1754: _temp1753=( void*) _temp1745.rgn_typ; goto _LL1752; _LL1752: _temp1751=
_temp1745.nullable; goto _LL1750; _LL1750: _temp1749= _temp1745.tq; goto _LL1748;
_LL1748: _temp1747= _temp1745.bounds; goto _LL1731;} else{ goto _LL1681;}
_LL1731: _temp1730= _temp1671.f2; if(( unsigned int) _temp1730 >  4u?*(( int*)
_temp1730) ==  Cyc_Absyn_TunionType: 0){ _LL1733: _temp1732=(( struct Cyc_Absyn_TunionType_struct*)
_temp1730)->f1; _LL1739: _temp1738=( void*) _temp1732.tunion_info; if(*(( int*)
_temp1738) ==  Cyc_Absyn_KnownTunion){ _LL1741: _temp1740=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1738)->f1; _temp1742=* _temp1740; goto _LL1737;} else{ goto _LL1681;}
_LL1737: _temp1736= _temp1732.targs; goto _LL1735; _LL1735: _temp1734=( void*)
_temp1732.rgn; goto _LL1680;} else{ goto _LL1681;} _LL1681: goto _LL1682;
_LL1674: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1689.nullable, _temp1685.nullable)){ void* _temp1757=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1689.nullable))->v;
int _temp1763; _LL1759: if(( unsigned int) _temp1757 >  1u?*(( int*) _temp1757)
==  Cyc_Absyn_Eq_constr: 0){ _LL1764: _temp1763=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1757)->f1; goto _LL1760;} else{ goto _LL1761;} _LL1761: goto _LL1762;
_LL1760: okay= ! _temp1763; goto _LL1758; _LL1762:( int) _throw(({ void*
_temp1765[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp1765, sizeof( void*), 0u));}));
_LL1758:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1689.bounds,
_temp1685.bounds)){ struct _tuple8 _temp1767=({ struct _tuple8 _temp1766;
_temp1766.f1=( void*)( Cyc_Absyn_compress_conref( _temp1689.bounds))->v;
_temp1766.f2=( void*)( Cyc_Absyn_compress_conref( _temp1685.bounds))->v;
_temp1766;}); void* _temp1777; void* _temp1779; void* _temp1781; void* _temp1783;
void* _temp1785; void* _temp1787; struct Cyc_Absyn_Exp* _temp1789; void*
_temp1791; void* _temp1793; struct Cyc_Absyn_Exp* _temp1795; void* _temp1797;
void* _temp1799; void* _temp1801; void* _temp1803; _LL1769: _LL1782: _temp1781=
_temp1767.f1; if(( unsigned int) _temp1781 >  1u?*(( int*) _temp1781) ==  Cyc_Absyn_Eq_constr:
0){ _LL1784: _temp1783=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1781)->f1;
if(( unsigned int) _temp1783 >  1u?*(( int*) _temp1783) ==  Cyc_Absyn_Upper_b: 0){
goto _LL1778;} else{ goto _LL1771;}} else{ goto _LL1771;} _LL1778: _temp1777=
_temp1767.f2; if(( unsigned int) _temp1777 >  1u?*(( int*) _temp1777) ==  Cyc_Absyn_Eq_constr:
0){ _LL1780: _temp1779=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1777)->f1;
if( _temp1779 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1770;} else{ goto
_LL1771;}} else{ goto _LL1771;} _LL1771: _LL1792: _temp1791= _temp1767.f1; if((
unsigned int) _temp1791 >  1u?*(( int*) _temp1791) ==  Cyc_Absyn_Eq_constr: 0){
_LL1794: _temp1793=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1791)->f1;
if(( unsigned int) _temp1793 >  1u?*(( int*) _temp1793) ==  Cyc_Absyn_Upper_b: 0){
_LL1796: _temp1795=(( struct Cyc_Absyn_Upper_b_struct*) _temp1793)->f1; goto
_LL1786;} else{ goto _LL1773;}} else{ goto _LL1773;} _LL1786: _temp1785=
_temp1767.f2; if(( unsigned int) _temp1785 >  1u?*(( int*) _temp1785) ==  Cyc_Absyn_Eq_constr:
0){ _LL1788: _temp1787=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1785)->f1;
if(( unsigned int) _temp1787 >  1u?*(( int*) _temp1787) ==  Cyc_Absyn_Upper_b: 0){
_LL1790: _temp1789=(( struct Cyc_Absyn_Upper_b_struct*) _temp1787)->f1; goto
_LL1772;} else{ goto _LL1773;}} else{ goto _LL1773;} _LL1773: _LL1802: _temp1801=
_temp1767.f1; if(( unsigned int) _temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Eq_constr:
0){ _LL1804: _temp1803=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1801)->f1;
if( _temp1803 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1798;} else{ goto
_LL1775;}} else{ goto _LL1775;} _LL1798: _temp1797= _temp1767.f2; if((
unsigned int) _temp1797 >  1u?*(( int*) _temp1797) ==  Cyc_Absyn_Eq_constr: 0){
_LL1800: _temp1799=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1797)->f1;
if( _temp1799 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1774;} else{ goto
_LL1775;}} else{ goto _LL1775;} _LL1775: goto _LL1776; _LL1770: okay= 1; goto
_LL1768; _LL1772: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1795) >=  Cyc_Evexp_eval_const_uint_exp(
_temp1789): 0;({ void* _temp1805[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1805, sizeof( void*), 0u));}); goto
_LL1768; _LL1774: okay= 1; goto _LL1768; _LL1776: okay= 0; goto _LL1768; _LL1768:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1689.elt_typ,( void*) _temp1685.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1689.rgn_typ,( void*) _temp1685.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1689.rgn_typ,( void*) _temp1685.rgn_typ):
0; okay= okay? !( _temp1689.tq).q_const? 1:( _temp1685.tq).q_const: 0; return
okay;} _LL1676: { int okay; okay=( _temp1701 !=  0? _temp1693 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1701)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1693)): 0; return( okay? Cyc_Tcutil_unify(
_temp1705, _temp1697): 0)? ! _temp1703.q_const? 1: _temp1695.q_const: 0;}
_LL1678: if(( _temp1728 ==  _temp1717? 1: Cyc_Absyn_qvar_cmp( _temp1728->name,
_temp1717->name) ==  0)? _temp1726->typs ==  0: 0){ for( 0; _temp1722 !=  0?
_temp1711 !=  0: 0;( _temp1722=(( struct Cyc_List_List*) _check_null( _temp1722))->tl,
_temp1711=(( struct Cyc_List_List*) _check_null( _temp1711))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1722))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1711))->hd)){ break;}} if( _temp1722 ==  0?
_temp1711 ==  0: 0){ return 1;}} return 0; _LL1680:{ void* _temp1806= Cyc_Tcutil_compress(
_temp1755); struct Cyc_Absyn_TunionFieldInfo _temp1812; struct Cyc_List_List*
_temp1814; void* _temp1816; struct Cyc_Absyn_Tunionfield* _temp1818; struct Cyc_Absyn_Tuniondecl*
_temp1820; _LL1808: if(( unsigned int) _temp1806 >  4u?*(( int*) _temp1806) == 
Cyc_Absyn_TunionFieldType: 0){ _LL1813: _temp1812=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1806)->f1; _LL1817: _temp1816=( void*) _temp1812.field_info; if(*(( int*)
_temp1816) ==  Cyc_Absyn_KnownTunionfield){ _LL1821: _temp1820=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1816)->f1; goto _LL1819; _LL1819: _temp1818=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1816)->f2; goto _LL1815;} else{ goto _LL1810;} _LL1815: _temp1814=
_temp1812.targs; goto _LL1809;} else{ goto _LL1810;} _LL1810: goto _LL1811;
_LL1809: if( ! Cyc_Tcutil_unify( _temp1753, _temp1734)? ! Cyc_Tcenv_region_outlives(
te, _temp1753, _temp1734): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1751,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1747,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1742->name, _temp1820->name) ==  0? _temp1818->typs !=  0: 0){ int okay= 1;
for( 0; _temp1814 !=  0? _temp1736 !=  0: 0;( _temp1814=(( struct Cyc_List_List*)
_check_null( _temp1814))->tl, _temp1736=(( struct Cyc_List_List*) _check_null(
_temp1736))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1814))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1736))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1814 !=  0)? 1:
_temp1736 !=  0){ return 0;} return 1;} goto _LL1807; _LL1811: goto _LL1807;
_LL1807:;} return 0; _LL1682: return Cyc_Tcutil_unify( t1, t2); _LL1672:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1822[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1822, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Std_String_pa_struct
_temp1825; _temp1825.tag= Cyc_Std_String_pa; _temp1825.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct _temp1824;
_temp1824.tag= Cyc_Std_String_pa; _temp1824.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1823[ 2u]={& _temp1824,& _temp1825}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1823, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} int Cyc_Tcutil_coerceable( void* t){ void* _temp1826= Cyc_Tcutil_compress(
t); _LL1828: if(( unsigned int) _temp1826 >  4u?*(( int*) _temp1826) ==  Cyc_Absyn_IntType:
0){ goto _LL1829;} else{ goto _LL1830;} _LL1830: if( _temp1826 == ( void*) Cyc_Absyn_FloatType){
goto _LL1831;} else{ goto _LL1832;} _LL1832: if( _temp1826 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1833;} else{ goto _LL1834;} _LL1834: goto _LL1835; _LL1829: goto _LL1831;
_LL1831: goto _LL1833; _LL1833: return 1; _LL1835: return 0; _LL1827:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1836=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp1836->f1= x->tq; _temp1836->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1836;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1837= t1; struct Cyc_List_List* _temp1849; struct Cyc_Absyn_Structdecl**
_temp1851; struct Cyc_List_List* _temp1853; struct _tuple1* _temp1855; struct
Cyc_List_List* _temp1857; _LL1839: if( _temp1837 == ( void*) Cyc_Absyn_VoidType){
goto _LL1840;} else{ goto _LL1841;} _LL1841: if(( unsigned int) _temp1837 >  4u?*((
int*) _temp1837) ==  Cyc_Absyn_TupleType: 0){ _LL1850: _temp1849=(( struct Cyc_Absyn_TupleType_struct*)
_temp1837)->f1; goto _LL1842;} else{ goto _LL1843;} _LL1843: if(( unsigned int)
_temp1837 >  4u?*(( int*) _temp1837) ==  Cyc_Absyn_StructType: 0){ _LL1856:
_temp1855=(( struct Cyc_Absyn_StructType_struct*) _temp1837)->f1; goto _LL1854;
_LL1854: _temp1853=(( struct Cyc_Absyn_StructType_struct*) _temp1837)->f2; goto
_LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_StructType_struct*) _temp1837)->f3;
goto _LL1844;} else{ goto _LL1845;} _LL1845: if(( unsigned int) _temp1837 >  4u?*((
int*) _temp1837) ==  Cyc_Absyn_AnonStructType: 0){ _LL1858: _temp1857=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1837)->f1; goto _LL1846;} else{ goto
_LL1847;} _LL1847: goto _LL1848; _LL1840: return 0; _LL1842: return _temp1849;
_LL1844: if( _temp1851 ==  0? 1: _temp1855 ==  0){ return({ struct Cyc_List_List*
_temp1859=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1859->hd=( void*)({ struct _tuple4* _temp1860=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1860->f1= Cyc_Absyn_empty_tqual(); _temp1860->f2=
t1; _temp1860;}); _temp1859->tl= 0; _temp1859;});}{ struct Cyc_Absyn_Structdecl*
_temp1861=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1851)); struct
_tuple1 _temp1862=*(( struct _tuple1*) _check_null( _temp1855)); struct Cyc_List_List*
_temp1863=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1861->tvs, _temp1853); if( _temp1861->fields ==  0){
return({ struct Cyc_List_List* _temp1864=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1864->hd=( void*)({ struct _tuple4*
_temp1865=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1865->f1=
Cyc_Absyn_empty_tqual(); _temp1865->f2= t1; _temp1865;}); _temp1864->tl= 0;
_temp1864;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1863,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1861->fields))->v);}
_LL1846: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1857); _LL1848: return({
struct Cyc_List_List* _temp1866=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1866->hd=( void*)({ struct _tuple4* _temp1867=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1867->f1= Cyc_Absyn_empty_tqual();
_temp1867->f2= t1; _temp1867;}); _temp1866->tl= 0; _temp1866;}); _LL1838:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1869=({ struct _tuple8 _temp1868; _temp1868.f1= t1; _temp1868.f2=
t2; _temp1868;}); void* _temp1875; struct Cyc_Absyn_PtrInfo _temp1877; struct
Cyc_Absyn_Conref* _temp1879; struct Cyc_Absyn_Tqual _temp1881; struct Cyc_Absyn_Conref*
_temp1883; void* _temp1885; void* _temp1887; void* _temp1889; struct Cyc_Absyn_PtrInfo
_temp1891; struct Cyc_Absyn_Conref* _temp1893; struct Cyc_Absyn_Tqual _temp1895;
struct Cyc_Absyn_Conref* _temp1897; void* _temp1899; void* _temp1901; _LL1871:
_LL1890: _temp1889= _temp1869.f1; if(( unsigned int) _temp1889 >  4u?*(( int*)
_temp1889) ==  Cyc_Absyn_PointerType: 0){ _LL1892: _temp1891=(( struct Cyc_Absyn_PointerType_struct*)
_temp1889)->f1; _LL1902: _temp1901=( void*) _temp1891.elt_typ; goto _LL1900;
_LL1900: _temp1899=( void*) _temp1891.rgn_typ; goto _LL1898; _LL1898: _temp1897=
_temp1891.nullable; goto _LL1896; _LL1896: _temp1895= _temp1891.tq; goto _LL1894;
_LL1894: _temp1893= _temp1891.bounds; goto _LL1876;} else{ goto _LL1873;}
_LL1876: _temp1875= _temp1869.f2; if(( unsigned int) _temp1875 >  4u?*(( int*)
_temp1875) ==  Cyc_Absyn_PointerType: 0){ _LL1878: _temp1877=(( struct Cyc_Absyn_PointerType_struct*)
_temp1875)->f1; _LL1888: _temp1887=( void*) _temp1877.elt_typ; goto _LL1886;
_LL1886: _temp1885=( void*) _temp1877.rgn_typ; goto _LL1884; _LL1884: _temp1883=
_temp1877.nullable; goto _LL1882; _LL1882: _temp1881= _temp1877.tq; goto _LL1880;
_LL1880: _temp1879= _temp1877.bounds; goto _LL1872;} else{ goto _LL1873;}
_LL1873: goto _LL1874; _LL1872: if( _temp1895.q_const? ! _temp1881.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1897, _temp1883)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1897): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1883): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1899, _temp1885)? ! Cyc_Tcenv_region_outlives(
te, _temp1899, _temp1885): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1893, _temp1879)){ struct _tuple8 _temp1904=({ struct _tuple8 _temp1903;
_temp1903.f1= Cyc_Absyn_conref_val( _temp1893); _temp1903.f2= Cyc_Absyn_conref_val(
_temp1879); _temp1903;}); void* _temp1912; void* _temp1914; void* _temp1916;
struct Cyc_Absyn_Exp* _temp1918; void* _temp1920; struct Cyc_Absyn_Exp*
_temp1922; _LL1906: _LL1915: _temp1914= _temp1904.f1; if(( unsigned int)
_temp1914 >  1u?*(( int*) _temp1914) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1913;}
else{ goto _LL1908;} _LL1913: _temp1912= _temp1904.f2; if( _temp1912 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1907;} else{ goto _LL1908;} _LL1908: _LL1921:
_temp1920= _temp1904.f1; if(( unsigned int) _temp1920 >  1u?*(( int*) _temp1920)
==  Cyc_Absyn_Upper_b: 0){ _LL1923: _temp1922=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1920)->f1; goto _LL1917;} else{ goto _LL1910;} _LL1917: _temp1916=
_temp1904.f2; if(( unsigned int) _temp1916 >  1u?*(( int*) _temp1916) ==  Cyc_Absyn_Upper_b:
0){ _LL1919: _temp1918=(( struct Cyc_Absyn_Upper_b_struct*) _temp1916)->f1; goto
_LL1909;} else{ goto _LL1910;} _LL1910: goto _LL1911; _LL1907: goto _LL1905;
_LL1909: if( Cyc_Evexp_eval_const_uint_exp( _temp1922) <  Cyc_Evexp_eval_const_uint_exp(
_temp1918)){ return 0;} goto _LL1905; _LL1911: return 0; _LL1905:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1924=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1924->hd=( void*)({ struct _tuple8*
_temp1925=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1925->f1=
t1; _temp1925->f2= t2; _temp1925;}); _temp1924->tl= assume; _temp1924;}),
_temp1901, _temp1887); _LL1874: return 0; _LL1870:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1927=({ struct _tuple8 _temp1926;
_temp1926.f1= Cyc_Tcutil_compress( t1); _temp1926.f2= Cyc_Tcutil_compress( t2);
_temp1926;}); void* _temp1933; void* _temp1935; void* _temp1937; void* _temp1939;
_LL1929: _LL1938: _temp1937= _temp1927.f1; if(( unsigned int) _temp1937 >  4u?*((
int*) _temp1937) ==  Cyc_Absyn_IntType: 0){ _LL1940: _temp1939=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1937)->f2; goto _LL1934;} else{ goto _LL1931;}
_LL1934: _temp1933= _temp1927.f2; if(( unsigned int) _temp1933 >  4u?*(( int*)
_temp1933) ==  Cyc_Absyn_IntType: 0){ _LL1936: _temp1935=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1933)->f2; goto _LL1930;} else{ goto _LL1931;} _LL1931: goto _LL1932;
_LL1930: return _temp1939 ==  _temp1935; _LL1932: return 0; _LL1928:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=((
struct Cyc_List_List*) _check_null( tqs1))->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1943; void* _temp1944; struct Cyc_Absyn_Tqual _temp1946;
struct _tuple4* _temp1941=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs1))->hd; _temp1943=* _temp1941; _LL1947: _temp1946= _temp1943.f1;
goto _LL1945; _LL1945: _temp1944= _temp1943.f2; goto _LL1942; _LL1942: { struct
_tuple4 _temp1950; void* _temp1951; struct Cyc_Absyn_Tqual _temp1953; struct
_tuple4* _temp1948=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs2))->hd;
_temp1950=* _temp1948; _LL1954: _temp1953= _temp1950.f1; goto _LL1952; _LL1952:
_temp1951= _temp1950.f2; goto _LL1949; _LL1949: if( _temp1953.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1944, _temp1951): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1944, _temp1951)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1944,
_temp1951)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1955= t2; struct Cyc_Absyn_Uniondecl** _temp1963; struct Cyc_List_List*
_temp1965; void* _temp1967; _LL1957: if(( unsigned int) _temp1955 >  4u?*(( int*)
_temp1955) ==  Cyc_Absyn_UnionType: 0){ _LL1966: _temp1965=(( struct Cyc_Absyn_UnionType_struct*)
_temp1955)->f2; goto _LL1964; _LL1964: _temp1963=(( struct Cyc_Absyn_UnionType_struct*)
_temp1955)->f3; goto _LL1958;} else{ goto _LL1959;} _LL1959: if(( unsigned int)
_temp1955 >  4u?*(( int*) _temp1955) ==  Cyc_Absyn_IntType: 0){ _LL1968:
_temp1967=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1955)->f2; if(
_temp1967 == ( void*) Cyc_Absyn_B4){ goto _LL1960;} else{ goto _LL1961;}} else{
goto _LL1961;} _LL1961: goto _LL1962; _LL1958: if( _temp1963 ==  0){ goto
_LL1956;}{ struct Cyc_Absyn_Uniondecl* _temp1969=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1963)); if( _temp1969->fields ==  0){ goto _LL1956;}{ struct
Cyc_List_List* _temp1970=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1969->tvs, _temp1965);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1969->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp1970,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1956;}} _LL1960: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1956; _LL1962: goto _LL1956; _LL1956:;}{ void* _temp1971= t1;
struct Cyc_Absyn_PtrInfo _temp1989; struct Cyc_Absyn_Conref* _temp1991; struct
Cyc_Absyn_Tqual _temp1993; struct Cyc_Absyn_Conref* _temp1995; void* _temp1997;
void* _temp1999; struct Cyc_Absyn_Exp* _temp2001; struct Cyc_Absyn_Tqual
_temp2003; void* _temp2005; struct Cyc_Absyn_Enumdecl* _temp2007; _LL1973: if((
unsigned int) _temp1971 >  4u?*(( int*) _temp1971) ==  Cyc_Absyn_PointerType: 0){
_LL1990: _temp1989=(( struct Cyc_Absyn_PointerType_struct*) _temp1971)->f1;
_LL2000: _temp1999=( void*) _temp1989.elt_typ; goto _LL1998; _LL1998: _temp1997=(
void*) _temp1989.rgn_typ; goto _LL1996; _LL1996: _temp1995= _temp1989.nullable;
goto _LL1994; _LL1994: _temp1993= _temp1989.tq; goto _LL1992; _LL1992: _temp1991=
_temp1989.bounds; goto _LL1974;} else{ goto _LL1975;} _LL1975: if(( unsigned int)
_temp1971 >  4u?*(( int*) _temp1971) ==  Cyc_Absyn_ArrayType: 0){ _LL2006:
_temp2005=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1971)->f1; goto
_LL2004; _LL2004: _temp2003=(( struct Cyc_Absyn_ArrayType_struct*) _temp1971)->f2;
goto _LL2002; _LL2002: _temp2001=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1971)->f3; goto _LL1976;} else{ goto _LL1977;} _LL1977: if(( unsigned int)
_temp1971 >  4u?*(( int*) _temp1971) ==  Cyc_Absyn_EnumType: 0){ _LL2008:
_temp2007=(( struct Cyc_Absyn_EnumType_struct*) _temp1971)->f2; goto _LL1978;}
else{ goto _LL1979;} _LL1979: if(( unsigned int) _temp1971 >  4u?*(( int*)
_temp1971) ==  Cyc_Absyn_IntType: 0){ goto _LL1980;} else{ goto _LL1981;}
_LL1981: if( _temp1971 == ( void*) Cyc_Absyn_FloatType){ goto _LL1982;} else{
goto _LL1983;} _LL1983: if( _temp1971 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1984;} else{ goto _LL1985;} _LL1985: if( _temp1971 == ( void*) Cyc_Absyn_VoidType){
goto _LL1986;} else{ goto _LL1987;} _LL1987: goto _LL1988; _LL1974:{ void*
_temp2009= t2; struct Cyc_Absyn_PtrInfo _temp2017; struct Cyc_Absyn_Conref*
_temp2019; struct Cyc_Absyn_Tqual _temp2021; struct Cyc_Absyn_Conref* _temp2023;
void* _temp2025; void* _temp2027; _LL2011: if(( unsigned int) _temp2009 >  4u?*((
int*) _temp2009) ==  Cyc_Absyn_PointerType: 0){ _LL2018: _temp2017=(( struct Cyc_Absyn_PointerType_struct*)
_temp2009)->f1; _LL2028: _temp2027=( void*) _temp2017.elt_typ; goto _LL2026;
_LL2026: _temp2025=( void*) _temp2017.rgn_typ; goto _LL2024; _LL2024: _temp2023=
_temp2017.nullable; goto _LL2022; _LL2022: _temp2021= _temp2017.tq; goto _LL2020;
_LL2020: _temp2019= _temp2017.bounds; goto _LL2012;} else{ goto _LL2013;}
_LL2013: if( _temp2009 == ( void*) Cyc_Absyn_VoidType){ goto _LL2014;} else{
goto _LL2015;} _LL2015: goto _LL2016; _LL2012: { struct Cyc_List_List* _temp2029=({
struct Cyc_List_List* _temp2046=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2046->hd=( void*)({ struct _tuple8* _temp2047=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2047->f1= t1;
_temp2047->f2= t2; _temp2047;}); _temp2046->tl= 0; _temp2046;}); int _temp2030=
Cyc_Tcutil_ptrsubtype( te, _temp2029, _temp1999, _temp2027)? ! _temp1993.q_const?
1: _temp2021.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1991, _temp2019); if( ! bounds_ok){ struct _tuple8 _temp2032=({ struct
_tuple8 _temp2031; _temp2031.f1= Cyc_Absyn_conref_val( _temp1991); _temp2031.f2=
Cyc_Absyn_conref_val( _temp2019); _temp2031;}); void* _temp2038; struct Cyc_Absyn_Exp*
_temp2040; void* _temp2042; struct Cyc_Absyn_Exp* _temp2044; _LL2034: _LL2043:
_temp2042= _temp2032.f1; if(( unsigned int) _temp2042 >  1u?*(( int*) _temp2042)
==  Cyc_Absyn_Upper_b: 0){ _LL2045: _temp2044=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2042)->f1; goto _LL2039;} else{ goto _LL2036;} _LL2039: _temp2038=
_temp2032.f2; if(( unsigned int) _temp2038 >  1u?*(( int*) _temp2038) ==  Cyc_Absyn_Upper_b:
0){ _LL2041: _temp2040=(( struct Cyc_Absyn_Upper_b_struct*) _temp2038)->f1; goto
_LL2035;} else{ goto _LL2036;} _LL2036: goto _LL2037; _LL2035: if( Cyc_Evexp_eval_const_uint_exp(
_temp2044) >=  Cyc_Evexp_eval_const_uint_exp( _temp2040)){ bounds_ok= 1;} goto
_LL2033; _LL2037: bounds_ok= 1; goto _LL2033; _LL2033:;} return( bounds_ok?
_temp2030: 0)? Cyc_Tcutil_unify( _temp1997, _temp2025)? 1: Cyc_Tcenv_region_outlives(
te, _temp1997, _temp2025): 0;} _LL2014: return 1; _LL2016: goto _LL2010; _LL2010:;}
return 0; _LL1976:{ void* _temp2048= t2; struct Cyc_Absyn_Exp* _temp2054; struct
Cyc_Absyn_Tqual _temp2056; void* _temp2058; _LL2050: if(( unsigned int)
_temp2048 >  4u?*(( int*) _temp2048) ==  Cyc_Absyn_ArrayType: 0){ _LL2059:
_temp2058=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2048)->f1; goto
_LL2057; _LL2057: _temp2056=(( struct Cyc_Absyn_ArrayType_struct*) _temp2048)->f2;
goto _LL2055; _LL2055: _temp2054=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2048)->f3; goto _LL2051;} else{ goto _LL2052;} _LL2052: goto _LL2053;
_LL2051: { int okay; okay=( _temp2001 !=  0? _temp2054 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2001)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2054)): 0; return( okay? Cyc_Tcutil_unify(
_temp2005, _temp2058): 0)? ! _temp2003.q_const? 1: _temp2056.q_const: 0;}
_LL2053: return 0; _LL2049:;} return 0; _LL1978:{ void* _temp2060= t2; struct
Cyc_Absyn_Enumdecl* _temp2066; _LL2062: if(( unsigned int) _temp2060 >  4u?*((
int*) _temp2060) ==  Cyc_Absyn_EnumType: 0){ _LL2067: _temp2066=(( struct Cyc_Absyn_EnumType_struct*)
_temp2060)->f2; goto _LL2063;} else{ goto _LL2064;} _LL2064: goto _LL2065;
_LL2063: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2007))->fields != 
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2066))->fields !=  0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp2007))->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2066))->fields))->v): 0){ return 1;} goto _LL2061; _LL2065:
goto _LL2061; _LL2061:;} goto _LL1980; _LL1980: goto _LL1982; _LL1982: goto
_LL1984; _LL1984: return Cyc_Tcutil_coerceable( t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType;
_LL1986: return t2 == ( void*) Cyc_Absyn_VoidType; _LL1988: return 0; _LL1972:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2068= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2069=(
struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2069[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2070; _temp2070.tag= Cyc_Absyn_Cast_e;
_temp2070.f1=( void*) t; _temp2070.f2= _temp2068; _temp2070;}); _temp2069;})));
e->topt=({ struct Cyc_Core_Opt* _temp2071=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2071->v=( void*) t; _temp2071;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2072= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2074: if(( unsigned int) _temp2072
>  4u?*(( int*) _temp2072) ==  Cyc_Absyn_EnumType: 0){ goto _LL2075;} else{ goto
_LL2076;} _LL2076: if(( unsigned int) _temp2072 >  4u?*(( int*) _temp2072) == 
Cyc_Absyn_IntType: 0){ goto _LL2077;} else{ goto _LL2078;} _LL2078: if((
unsigned int) _temp2072 >  4u?*(( int*) _temp2072) ==  Cyc_Absyn_Evar: 0){ goto
_LL2079;} else{ goto _LL2080;} _LL2080: goto _LL2081; _LL2075: goto _LL2077;
_LL2077: return 1; _LL2079: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2081: return 0; _LL2073:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2082= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2084: if( _temp2082 == ( void*) Cyc_Absyn_FloatType){
goto _LL2085;} else{ goto _LL2086;} _LL2086: if( _temp2082 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2087;} else{ goto _LL2088;} _LL2088: goto _LL2089; _LL2085: goto _LL2087;
_LL2087: return 1; _LL2089: return 0; _LL2083:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp2090= Cyc_Tcutil_compress( t); _LL2092: if(( unsigned int)
_temp2090 >  4u?*(( int*) _temp2090) ==  Cyc_Absyn_FnType: 0){ goto _LL2093;}
else{ goto _LL2094;} _LL2094: goto _LL2095; _LL2093: return 1; _LL2095: return 0;
_LL2091:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp2097=({ struct _tuple8 _temp2096; _temp2096.f1= t1; _temp2096.f2=
t2; _temp2096;}); void* _temp2121; void* _temp2123; void* _temp2125; void*
_temp2127; void* _temp2129; void* _temp2131; void* _temp2133; void* _temp2135;
void* _temp2137; void* _temp2139; void* _temp2141; void* _temp2143; void*
_temp2145; void* _temp2147; void* _temp2149; void* _temp2151; void* _temp2153;
void* _temp2155; void* _temp2157; void* _temp2159; void* _temp2161; void*
_temp2163; _LL2099: _LL2122: _temp2121= _temp2097.f1; if( _temp2121 == ( void*)
Cyc_Absyn_DoubleType){ goto _LL2100;} else{ goto _LL2101;} _LL2101: _LL2124:
_temp2123= _temp2097.f2; if( _temp2123 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2102;} else{ goto _LL2103;} _LL2103: _LL2126: _temp2125= _temp2097.f1; if(
_temp2125 == ( void*) Cyc_Absyn_FloatType){ goto _LL2104;} else{ goto _LL2105;}
_LL2105: _LL2128: _temp2127= _temp2097.f2; if( _temp2127 == ( void*) Cyc_Absyn_FloatType){
goto _LL2106;} else{ goto _LL2107;} _LL2107: _LL2130: _temp2129= _temp2097.f1;
if(( unsigned int) _temp2129 >  4u?*(( int*) _temp2129) ==  Cyc_Absyn_IntType: 0){
_LL2134: _temp2133=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2129)->f1;
if( _temp2133 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2132;} else{ goto _LL2109;}
_LL2132: _temp2131=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2129)->f2;
if( _temp2131 == ( void*) Cyc_Absyn_B8){ goto _LL2108;} else{ goto _LL2109;}}
else{ goto _LL2109;} _LL2109: _LL2136: _temp2135= _temp2097.f2; if((
unsigned int) _temp2135 >  4u?*(( int*) _temp2135) ==  Cyc_Absyn_IntType: 0){
_LL2140: _temp2139=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2135)->f1;
if( _temp2139 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2138;} else{ goto _LL2111;}
_LL2138: _temp2137=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2135)->f2;
if( _temp2137 == ( void*) Cyc_Absyn_B8){ goto _LL2110;} else{ goto _LL2111;}}
else{ goto _LL2111;} _LL2111: _LL2142: _temp2141= _temp2097.f1; if((
unsigned int) _temp2141 >  4u?*(( int*) _temp2141) ==  Cyc_Absyn_IntType: 0){
_LL2146: _temp2145=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2141)->f1;
if( _temp2145 == ( void*) Cyc_Absyn_Signed){ goto _LL2144;} else{ goto _LL2113;}
_LL2144: _temp2143=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2141)->f2;
if( _temp2143 == ( void*) Cyc_Absyn_B8){ goto _LL2112;} else{ goto _LL2113;}}
else{ goto _LL2113;} _LL2113: _LL2148: _temp2147= _temp2097.f2; if((
unsigned int) _temp2147 >  4u?*(( int*) _temp2147) ==  Cyc_Absyn_IntType: 0){
_LL2152: _temp2151=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2147)->f1;
if( _temp2151 == ( void*) Cyc_Absyn_Signed){ goto _LL2150;} else{ goto _LL2115;}
_LL2150: _temp2149=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2147)->f2;
if( _temp2149 == ( void*) Cyc_Absyn_B8){ goto _LL2114;} else{ goto _LL2115;}}
else{ goto _LL2115;} _LL2115: _LL2154: _temp2153= _temp2097.f1; if((
unsigned int) _temp2153 >  4u?*(( int*) _temp2153) ==  Cyc_Absyn_IntType: 0){
_LL2158: _temp2157=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2153)->f1;
if( _temp2157 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2156;} else{ goto _LL2117;}
_LL2156: _temp2155=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2153)->f2;
if( _temp2155 == ( void*) Cyc_Absyn_B4){ goto _LL2116;} else{ goto _LL2117;}}
else{ goto _LL2117;} _LL2117: _LL2160: _temp2159= _temp2097.f2; if((
unsigned int) _temp2159 >  4u?*(( int*) _temp2159) ==  Cyc_Absyn_IntType: 0){
_LL2164: _temp2163=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2159)->f1;
if( _temp2163 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2162;} else{ goto _LL2119;}
_LL2162: _temp2161=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2159)->f2;
if( _temp2161 == ( void*) Cyc_Absyn_B4){ goto _LL2118;} else{ goto _LL2119;}}
else{ goto _LL2119;} _LL2119: goto _LL2120; _LL2100: goto _LL2102; _LL2102:
return( void*) Cyc_Absyn_DoubleType; _LL2104: goto _LL2106; _LL2106: return(
void*) Cyc_Absyn_FloatType; _LL2108: goto _LL2110; _LL2110: return Cyc_Absyn_ulonglong_t;
_LL2112: goto _LL2114; _LL2114: return Cyc_Absyn_slonglong_t; _LL2116: goto
_LL2118; _LL2118: return Cyc_Absyn_uint_t; _LL2120: return Cyc_Absyn_sint_t;
_LL2098:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2165=( void*) e->r; struct Cyc_Core_Opt* _temp2171; _LL2167: if(*(( int*)
_temp2165) ==  Cyc_Absyn_AssignOp_e){ _LL2172: _temp2171=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2165)->f2; if( _temp2171 ==  0){ goto _LL2168;} else{ goto _LL2169;}} else{
goto _LL2169;} _LL2169: goto _LL2170; _LL2168:({ void* _temp2173[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2173, sizeof( void*), 0u));}); goto _LL2166; _LL2170: goto _LL2166; _LL2166:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2174=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2174[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2175; _temp2175.tag=
Cyc_Absyn_Forward_constr; _temp2175.f1= c2; _temp2175;}); _temp2174;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2176=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2176[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2177; _temp2177.tag= Cyc_Absyn_Forward_constr;
_temp2177.f1= c1; _temp2177;}); _temp2176;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2178=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2178[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2179; _temp2179.tag= Cyc_Absyn_Forward_constr; _temp2179.f1= c1; _temp2179;});
_temp2178;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2180=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2180[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2181; _temp2181.tag= Cyc_Absyn_Forward_constr;
_temp2181.f1= c2; _temp2181;}); _temp2180;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2182=( int*) _cycalloc_atomic( sizeof( int)); _temp2182[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2182;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2186;
_temp2186.tag= Cyc_Std_Int_pa; _temp2186.f1=( int)(( unsigned int) i);{ void*
_temp2185[ 1u]={& _temp2186}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2185, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2183=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2183->name=({ struct _tagged_arr* _temp2184=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2184[ 0]=
s; _temp2184;}); _temp2183->identity= 0; _temp2183->kind= Cyc_Absyn_new_conref(
k); _temp2183;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2187=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2187, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2189; _temp2189.tag= Cyc_Std_String_pa; _temp2189.f1=( struct _tagged_arr)*
t->name;{ void* _temp2188[ 1u]={& _temp2189}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2188, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2190= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2190, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2191=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2191[ 0]=( struct _tagged_arr) _temp2190; _temp2191;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2192=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2192->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2193=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2193->v=( void*)(*
x).f1; _temp2193;}); _temp2192->f2=(* x).f2; _temp2192->f3=(* x).f3; _temp2192;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2194= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2194=({ struct Cyc_List_List* _temp2195=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2195->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2195->tl= _temp2194; _temp2195;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2196=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2196[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2197; _temp2197.tag= Cyc_Absyn_FnType; _temp2197.f1=({ struct Cyc_Absyn_FnInfo
_temp2198; _temp2198.tvars= fd->tvs; _temp2198.effect= fd->effect; _temp2198.ret_typ=(
void*)(( void*) fd->ret_type); _temp2198.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2198.c_varargs= fd->c_varargs;
_temp2198.cyc_varargs= fd->cyc_varargs; _temp2198.rgn_po= fd->rgn_po; _temp2198.attributes=
_temp2194; _temp2198;}); _temp2197;}); _temp2196;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2199=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp2199->f1=(* pr).f1; _temp2199->f2= t; _temp2199;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2200=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2200->f1=({ struct _tuple15* _temp2201=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2201->f1=(* y).f1; _temp2201->f2=(* y).f2;
_temp2201;}); _temp2200->f2=(* y).f3; _temp2200;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2204; struct _tuple15* _temp2206; struct
_tuple16 _temp2202=* w; _LL2207: _temp2206= _temp2202.f1; goto _LL2205; _LL2205:
_temp2204= _temp2202.f2; goto _LL2203; _LL2203: { struct Cyc_Absyn_Tqual
_temp2210; struct Cyc_Core_Opt* _temp2212; struct _tuple15 _temp2208=* _temp2206;
_LL2213: _temp2212= _temp2208.f1; goto _LL2211; _LL2211: _temp2210= _temp2208.f2;
goto _LL2209; _LL2209: return({ struct _tuple2* _temp2214=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2214->f1= _temp2212; _temp2214->f2=
_temp2210; _temp2214->f3= _temp2204; _temp2214;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2215=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2215->name= f->name;
_temp2215->tq= f->tq; _temp2215->type=( void*) t; _temp2215->width= f->width;
_temp2215->attributes= f->attributes; _temp2215;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2216= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2264; struct Cyc_Absyn_Structdecl** _temp2266;
struct Cyc_List_List* _temp2268; struct _tuple1* _temp2270; struct Cyc_Absyn_Uniondecl**
_temp2272; struct Cyc_List_List* _temp2274; struct _tuple1* _temp2276; struct
Cyc_Absyn_TunionInfo _temp2278; void* _temp2280; struct Cyc_List_List* _temp2282;
void* _temp2284; struct Cyc_Absyn_TunionFieldInfo _temp2286; struct Cyc_List_List*
_temp2288; void* _temp2290; struct Cyc_Core_Opt* _temp2292; struct Cyc_List_List*
_temp2294; struct _tuple1* _temp2296; struct Cyc_Absyn_Exp* _temp2298; struct
Cyc_Absyn_Tqual _temp2300; void* _temp2302; struct Cyc_Absyn_PtrInfo _temp2304;
struct Cyc_Absyn_Conref* _temp2306; struct Cyc_Absyn_Tqual _temp2308; struct Cyc_Absyn_Conref*
_temp2310; void* _temp2312; void* _temp2314; struct Cyc_Absyn_FnInfo _temp2316;
struct Cyc_List_List* _temp2318; struct Cyc_List_List* _temp2320; struct Cyc_Absyn_VarargInfo*
_temp2322; int _temp2324; struct Cyc_List_List* _temp2326; void* _temp2328;
struct Cyc_Core_Opt* _temp2330; struct Cyc_List_List* _temp2332; struct Cyc_List_List*
_temp2334; struct Cyc_List_List* _temp2336; struct Cyc_List_List* _temp2338;
struct Cyc_Core_Opt* _temp2340; void* _temp2342; void* _temp2344; void*
_temp2346; struct Cyc_List_List* _temp2348; _LL2218: if(( unsigned int)
_temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_VarType: 0){ _LL2265:
_temp2264=(( struct Cyc_Absyn_VarType_struct*) _temp2216)->f1; goto _LL2219;}
else{ goto _LL2220;} _LL2220: if(( unsigned int) _temp2216 >  4u?*(( int*)
_temp2216) ==  Cyc_Absyn_StructType: 0){ _LL2271: _temp2270=(( struct Cyc_Absyn_StructType_struct*)
_temp2216)->f1; goto _LL2269; _LL2269: _temp2268=(( struct Cyc_Absyn_StructType_struct*)
_temp2216)->f2; goto _LL2267; _LL2267: _temp2266=(( struct Cyc_Absyn_StructType_struct*)
_temp2216)->f3; goto _LL2221;} else{ goto _LL2222;} _LL2222: if(( unsigned int)
_temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_UnionType: 0){ _LL2277:
_temp2276=(( struct Cyc_Absyn_UnionType_struct*) _temp2216)->f1; goto _LL2275;
_LL2275: _temp2274=(( struct Cyc_Absyn_UnionType_struct*) _temp2216)->f2; goto
_LL2273; _LL2273: _temp2272=(( struct Cyc_Absyn_UnionType_struct*) _temp2216)->f3;
goto _LL2223;} else{ goto _LL2224;} _LL2224: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_TunionType: 0){ _LL2279: _temp2278=(( struct Cyc_Absyn_TunionType_struct*)
_temp2216)->f1; _LL2285: _temp2284=( void*) _temp2278.tunion_info; goto _LL2283;
_LL2283: _temp2282= _temp2278.targs; goto _LL2281; _LL2281: _temp2280=( void*)
_temp2278.rgn; goto _LL2225;} else{ goto _LL2226;} _LL2226: if(( unsigned int)
_temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2287:
_temp2286=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2216)->f1; _LL2291:
_temp2290=( void*) _temp2286.field_info; goto _LL2289; _LL2289: _temp2288=
_temp2286.targs; goto _LL2227;} else{ goto _LL2228;} _LL2228: if(( unsigned int)
_temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_TypedefType: 0){ _LL2297:
_temp2296=(( struct Cyc_Absyn_TypedefType_struct*) _temp2216)->f1; goto _LL2295;
_LL2295: _temp2294=(( struct Cyc_Absyn_TypedefType_struct*) _temp2216)->f2; goto
_LL2293; _LL2293: _temp2292=(( struct Cyc_Absyn_TypedefType_struct*) _temp2216)->f3;
goto _LL2229;} else{ goto _LL2230;} _LL2230: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_ArrayType: 0){ _LL2303: _temp2302=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2216)->f1; goto _LL2301; _LL2301:
_temp2300=(( struct Cyc_Absyn_ArrayType_struct*) _temp2216)->f2; goto _LL2299;
_LL2299: _temp2298=(( struct Cyc_Absyn_ArrayType_struct*) _temp2216)->f3; goto
_LL2231;} else{ goto _LL2232;} _LL2232: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_PointerType: 0){ _LL2305: _temp2304=(( struct Cyc_Absyn_PointerType_struct*)
_temp2216)->f1; _LL2315: _temp2314=( void*) _temp2304.elt_typ; goto _LL2313;
_LL2313: _temp2312=( void*) _temp2304.rgn_typ; goto _LL2311; _LL2311: _temp2310=
_temp2304.nullable; goto _LL2309; _LL2309: _temp2308= _temp2304.tq; goto _LL2307;
_LL2307: _temp2306= _temp2304.bounds; goto _LL2233;} else{ goto _LL2234;}
_LL2234: if(( unsigned int) _temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_FnType:
0){ _LL2317: _temp2316=(( struct Cyc_Absyn_FnType_struct*) _temp2216)->f1;
_LL2333: _temp2332= _temp2316.tvars; goto _LL2331; _LL2331: _temp2330= _temp2316.effect;
goto _LL2329; _LL2329: _temp2328=( void*) _temp2316.ret_typ; goto _LL2327;
_LL2327: _temp2326= _temp2316.args; goto _LL2325; _LL2325: _temp2324= _temp2316.c_varargs;
goto _LL2323; _LL2323: _temp2322= _temp2316.cyc_varargs; goto _LL2321; _LL2321:
_temp2320= _temp2316.rgn_po; goto _LL2319; _LL2319: _temp2318= _temp2316.attributes;
goto _LL2235;} else{ goto _LL2236;} _LL2236: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_TupleType: 0){ _LL2335: _temp2334=(( struct Cyc_Absyn_TupleType_struct*)
_temp2216)->f1; goto _LL2237;} else{ goto _LL2238;} _LL2238: if(( unsigned int)
_temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_AnonStructType: 0){ _LL2337:
_temp2336=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2216)->f1; goto
_LL2239;} else{ goto _LL2240;} _LL2240: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2339: _temp2338=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2216)->f1; goto _LL2241;} else{ goto
_LL2242;} _LL2242: if(( unsigned int) _temp2216 >  4u?*(( int*) _temp2216) == 
Cyc_Absyn_Evar: 0){ _LL2341: _temp2340=(( struct Cyc_Absyn_Evar_struct*)
_temp2216)->f2; goto _LL2243;} else{ goto _LL2244;} _LL2244: if(( unsigned int)
_temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2343:
_temp2342=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2216)->f1;
goto _LL2245;} else{ goto _LL2246;} _LL2246: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_EnumType: 0){ goto _LL2247;} else{ goto _LL2248;}
_LL2248: if( _temp2216 == ( void*) Cyc_Absyn_VoidType){ goto _LL2249;} else{
goto _LL2250;} _LL2250: if(( unsigned int) _temp2216 >  4u?*(( int*) _temp2216)
==  Cyc_Absyn_IntType: 0){ goto _LL2251;} else{ goto _LL2252;} _LL2252: if(
_temp2216 == ( void*) Cyc_Absyn_FloatType){ goto _LL2253;} else{ goto _LL2254;}
_LL2254: if( _temp2216 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2255;} else{
goto _LL2256;} _LL2256: if( _temp2216 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL2257;} else{ goto _LL2258;} _LL2258: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_RgnsEff: 0){ _LL2345: _temp2344=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2216)->f1; goto _LL2259;} else{ goto _LL2260;}
_LL2260: if(( unsigned int) _temp2216 >  4u?*(( int*) _temp2216) ==  Cyc_Absyn_AccessEff:
0){ _LL2347: _temp2346=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2216)->f1;
goto _LL2261;} else{ goto _LL2262;} _LL2262: if(( unsigned int) _temp2216 >  4u?*((
int*) _temp2216) ==  Cyc_Absyn_JoinEff: 0){ _LL2349: _temp2348=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2216)->f1; goto _LL2263;} else{ goto _LL2217;} _LL2219: { struct
_handler_cons _temp2350; _push_handler(& _temp2350);{ int _temp2352= 0; if(
setjmp( _temp2350.handler)){ _temp2352= 1;} if( ! _temp2352){{ void* _temp2353=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2264); _npop_handler( 0u); return _temp2353;}; _pop_handler();} else{
void* _temp2351=( void*) _exn_thrown; void* _temp2355= _temp2351; _LL2357: if(
_temp2355 ==  Cyc_Core_Not_found){ goto _LL2358;} else{ goto _LL2359;} _LL2359:
goto _LL2360; _LL2358: return t; _LL2360:( void) _throw( _temp2355); _LL2356:;}}}
_LL2221: { struct Cyc_List_List* _temp2361= Cyc_Tcutil_substs( rgn, inst,
_temp2268); return _temp2361 ==  _temp2268? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2362=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2362[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2363; _temp2363.tag=
Cyc_Absyn_StructType; _temp2363.f1= _temp2270; _temp2363.f2= _temp2361;
_temp2363.f3= _temp2266; _temp2363;}); _temp2362;});} _LL2223: { struct Cyc_List_List*
_temp2364= Cyc_Tcutil_substs( rgn, inst, _temp2274); return _temp2364 == 
_temp2274? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2365=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2365[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2366; _temp2366.tag= Cyc_Absyn_UnionType;
_temp2366.f1= _temp2276; _temp2366.f2= _temp2364; _temp2366.f3= _temp2272;
_temp2366;}); _temp2365;});} _LL2225: { struct Cyc_List_List* _temp2367= Cyc_Tcutil_substs(
rgn, inst, _temp2282); void* _temp2368= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2280); return( _temp2367 ==  _temp2282? _temp2368 ==  _temp2280: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2369=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2369[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2370; _temp2370.tag= Cyc_Absyn_TunionType;
_temp2370.f1=({ struct Cyc_Absyn_TunionInfo _temp2371; _temp2371.tunion_info=(
void*) _temp2284; _temp2371.targs= _temp2367; _temp2371.rgn=( void*) _temp2368;
_temp2371;}); _temp2370;}); _temp2369;});} _LL2227: { struct Cyc_List_List*
_temp2372= Cyc_Tcutil_substs( rgn, inst, _temp2288); return _temp2372 == 
_temp2288? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2373=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2373[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2374; _temp2374.tag=
Cyc_Absyn_TunionFieldType; _temp2374.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2375; _temp2375.field_info=( void*) _temp2290; _temp2375.targs= _temp2372;
_temp2375;}); _temp2374;}); _temp2373;});} _LL2229: { struct Cyc_List_List*
_temp2376= Cyc_Tcutil_substs( rgn, inst, _temp2294); return _temp2376 == 
_temp2294? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2377=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2377[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2378; _temp2378.tag=
Cyc_Absyn_TypedefType; _temp2378.f1= _temp2296; _temp2378.f2= _temp2376;
_temp2378.f3= _temp2292; _temp2378;}); _temp2377;});} _LL2231: { void* _temp2379=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2302); return _temp2379 ==  _temp2302? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2380=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2380[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2381; _temp2381.tag= Cyc_Absyn_ArrayType;
_temp2381.f1=( void*) _temp2379; _temp2381.f2= _temp2300; _temp2381.f3=
_temp2298; _temp2381;}); _temp2380;});} _LL2233: { void* _temp2382= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2314); void* _temp2383= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2312); if( _temp2382 ==  _temp2314? _temp2383 ==  _temp2312: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2384=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2384[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2385; _temp2385.tag= Cyc_Absyn_PointerType;
_temp2385.f1=({ struct Cyc_Absyn_PtrInfo _temp2386; _temp2386.elt_typ=( void*)
_temp2382; _temp2386.rgn_typ=( void*) _temp2383; _temp2386.nullable= _temp2310;
_temp2386.tq= _temp2308; _temp2386.bounds= _temp2306; _temp2386;}); _temp2385;});
_temp2384;});} _LL2235:{ struct Cyc_List_List* _temp2387= _temp2332; for( 0;
_temp2387 !=  0; _temp2387=(( struct Cyc_List_List*) _check_null( _temp2387))->tl){
inst=({ struct Cyc_List_List* _temp2388=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2388->hd=( void*)({ struct _tuple5*
_temp2389=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2389->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2387))->hd; _temp2389->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2390=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2390[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2391; _temp2391.tag= Cyc_Absyn_VarType;
_temp2391.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2387))->hd; _temp2391;}); _temp2390;}); _temp2389;}); _temp2388->tl= inst;
_temp2388;});}}{ struct Cyc_List_List* _temp2394; struct Cyc_List_List*
_temp2396; struct _tuple0 _temp2392=(( struct _tuple0(*)( struct _RegionHandle*
r1, struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2326)); _LL2397:
_temp2396= _temp2392.f1; goto _LL2395; _LL2395: _temp2394= _temp2392.f2; goto
_LL2393; _LL2393: { struct Cyc_List_List* _temp2398= Cyc_Tcutil_substs( rgn,
inst, _temp2394); struct Cyc_List_List* _temp2399=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r1,
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2396, _temp2398)); struct Cyc_Core_Opt* eff2; if( _temp2330 ==  0){
eff2= 0;} else{ void* _temp2400= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2330))->v); if( _temp2400 == ( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2330))->v){ eff2= _temp2330;} else{ eff2=({
struct Cyc_Core_Opt* _temp2401=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2401->v=( void*) _temp2400; _temp2401;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2322 ==  0){ cyc_varargs2= 0;} else{ int _temp2404; void*
_temp2406; struct Cyc_Absyn_Tqual _temp2408; struct Cyc_Core_Opt* _temp2410;
struct Cyc_Absyn_VarargInfo _temp2402=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2322)); _LL2411: _temp2410= _temp2402.name; goto _LL2409;
_LL2409: _temp2408= _temp2402.tq; goto _LL2407; _LL2407: _temp2406=( void*)
_temp2402.type; goto _LL2405; _LL2405: _temp2404= _temp2402.inject; goto _LL2403;
_LL2403: { void* _temp2412= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2406); if(
_temp2412 ==  _temp2406){ cyc_varargs2= _temp2322;} else{ cyc_varargs2=({ struct
Cyc_Absyn_VarargInfo* _temp2413=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2413->name= _temp2410; _temp2413->tq=
_temp2408; _temp2413->type=( void*) _temp2412; _temp2413->inject= _temp2404;
_temp2413;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2416;
struct Cyc_List_List* _temp2418; struct _tuple0 _temp2414= Cyc_List_rsplit( rgn,
rgn, _temp2320); _LL2419: _temp2418= _temp2414.f1; goto _LL2417; _LL2417:
_temp2416= _temp2414.f2; goto _LL2415; _LL2415: { struct Cyc_List_List*
_temp2420= Cyc_Tcutil_substs( rgn, inst, _temp2418); struct Cyc_List_List*
_temp2421= Cyc_Tcutil_substs( rgn, inst, _temp2416); if( _temp2420 ==  _temp2418?
_temp2421 ==  _temp2416: 0){ rgn_po2= _temp2320;} else{ rgn_po2= Cyc_List_zip(
_temp2420, _temp2421);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2422=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2422[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2423; _temp2423.tag= Cyc_Absyn_FnType;
_temp2423.f1=({ struct Cyc_Absyn_FnInfo _temp2424; _temp2424.tvars= _temp2332;
_temp2424.effect= eff2; _temp2424.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2328); _temp2424.args= _temp2399; _temp2424.c_varargs= _temp2324;
_temp2424.cyc_varargs= cyc_varargs2; _temp2424.rgn_po= rgn_po2; _temp2424.attributes=
_temp2318; _temp2424;}); _temp2423;}); _temp2422;});}}}}} _LL2237: { struct Cyc_List_List*
_temp2425=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2334); struct Cyc_List_List* _temp2426= Cyc_Tcutil_substs( rgn, inst,
_temp2425); if( _temp2426 ==  _temp2425){ return t;}{ struct Cyc_List_List*
_temp2427=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2334, _temp2426); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2428=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2428[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2429; _temp2429.tag= Cyc_Absyn_TupleType;
_temp2429.f1= _temp2427; _temp2429;}); _temp2428;});}} _LL2239: { struct Cyc_List_List*
_temp2430=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2336); struct Cyc_List_List* _temp2431= Cyc_Tcutil_substs( rgn, inst,
_temp2430); if( _temp2431 ==  _temp2430){ return t;}{ struct Cyc_List_List*
_temp2432=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2336, _temp2431); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2433=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2433[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2434; _temp2434.tag= Cyc_Absyn_AnonStructType;
_temp2434.f1= _temp2432; _temp2434;}); _temp2433;});}} _LL2241: { struct Cyc_List_List*
_temp2435=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2338); struct Cyc_List_List* _temp2436= Cyc_Tcutil_substs( rgn, inst,
_temp2435); if( _temp2436 ==  _temp2435){ return t;}{ struct Cyc_List_List*
_temp2437=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2338, _temp2436); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2438=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2438[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2439; _temp2439.tag= Cyc_Absyn_AnonStructType;
_temp2439.f1= _temp2437; _temp2439;}); _temp2438;});}} _LL2243: if( _temp2340 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2340))->v);} else{ return t;} _LL2245: { void* _temp2440= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2342); return _temp2440 ==  _temp2342? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2441=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2441[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2442; _temp2442.tag= Cyc_Absyn_RgnHandleType; _temp2442.f1=( void*)
_temp2440; _temp2442;}); _temp2441;});} _LL2247: return t; _LL2249: return t;
_LL2251: return t; _LL2253: return t; _LL2255: return t; _LL2257: return t;
_LL2259: { void* _temp2443= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2344);
return _temp2443 ==  _temp2344? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2444=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2444[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2445; _temp2445.tag= Cyc_Absyn_RgnsEff;
_temp2445.f1=( void*) _temp2443; _temp2445;}); _temp2444;});} _LL2261: { void*
_temp2446= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2346); return _temp2446 == 
_temp2346? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2447=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2447[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2448; _temp2448.tag= Cyc_Absyn_AccessEff;
_temp2448.f1=( void*) _temp2446; _temp2448;}); _temp2447;});} _LL2263: { struct
Cyc_List_List* _temp2449= Cyc_Tcutil_substs( rgn, inst, _temp2348); return
_temp2449 ==  _temp2348? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2450=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2450[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2451; _temp2451.tag= Cyc_Absyn_JoinEff;
_temp2451.f1= _temp2449; _temp2451;}); _temp2450;});} _LL2217:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2452=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2453=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2454= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2452); struct Cyc_List_List* _temp2455= Cyc_Tcutil_substs( rgn,
inst, _temp2453); if( _temp2452 ==  _temp2454? _temp2453 ==  _temp2455: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2456=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2456->hd=(
void*) _temp2454; _temp2456->tl= _temp2455; _temp2456;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2501=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2501[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2502; _temp2502.tag= Cyc_Absyn_Const_e; _temp2502.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2502;}); _temp2501;}), loc);{ void*
_temp2457= Cyc_Tcutil_compress( t); void* _temp2471; void* _temp2473; void*
_temp2475; void* _temp2477; _LL2459: if(( unsigned int) _temp2457 >  4u?*(( int*)
_temp2457) ==  Cyc_Absyn_PointerType: 0){ goto _LL2460;} else{ goto _LL2461;}
_LL2461: if(( unsigned int) _temp2457 >  4u?*(( int*) _temp2457) ==  Cyc_Absyn_IntType:
0){ _LL2474: _temp2473=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2457)->f1;
goto _LL2472; _LL2472: _temp2471=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2457)->f2; if( _temp2471 == ( void*) Cyc_Absyn_B1){ goto _LL2462;} else{
goto _LL2463;}} else{ goto _LL2463;} _LL2463: if(( unsigned int) _temp2457 >  4u?*((
int*) _temp2457) ==  Cyc_Absyn_IntType: 0){ _LL2478: _temp2477=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2457)->f1; goto _LL2476; _LL2476: _temp2475=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2457)->f2; goto _LL2464;} else{
goto _LL2465;} _LL2465: if( _temp2457 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2466;} else{ goto _LL2467;} _LL2467: if( _temp2457 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2468;} else{ goto _LL2469;} _LL2469: goto _LL2470; _LL2460: goto _LL2458;
_LL2462:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2479=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2479[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2480; _temp2480.tag= Cyc_Absyn_Const_e;
_temp2480.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2481=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2481[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2482; _temp2482.tag= Cyc_Absyn_Char_c;
_temp2482.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2482.f2='\000'; _temp2482;});
_temp2481;})); _temp2480;}); _temp2479;}))); goto _LL2458; _LL2464:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2483=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2483[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2484; _temp2484.tag= Cyc_Absyn_Const_e; _temp2484.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2485=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2485[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2486; _temp2486.tag= Cyc_Absyn_Int_c; _temp2486.f1=( void*) _temp2477;
_temp2486.f2= 0; _temp2486;}); _temp2485;})); _temp2484;}); _temp2483;}))); if(
_temp2475 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2487=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2487[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2488; _temp2488.tag= Cyc_Absyn_Cast_e;
_temp2488.f1=( void*) t; _temp2488.f2= e; _temp2488;}); _temp2487;}), loc);}
goto _LL2458; _LL2466:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2489=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2489[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2490; _temp2490.tag= Cyc_Absyn_Const_e;
_temp2490.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2491=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2491[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2492; _temp2492.tag= Cyc_Absyn_Float_c;
_temp2492.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2492;});
_temp2491;})); _temp2490;}); _temp2489;}))); goto _LL2458; _LL2468:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2493=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2493[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2494; _temp2494.tag= Cyc_Absyn_Cast_e; _temp2494.f1=( void*) t; _temp2494.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2495=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2495[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2496; _temp2496.tag= Cyc_Absyn_Const_e;
_temp2496.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2497=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2497[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2498; _temp2498.tag= Cyc_Absyn_Float_c;
_temp2498.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2498;});
_temp2497;})); _temp2496;}); _temp2495;}), loc); _temp2494;}); _temp2493;})));
goto _LL2458; _LL2470:({ struct Cyc_Std_String_pa_struct _temp2500; _temp2500.tag=
Cyc_Std_String_pa; _temp2500.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2499[ 1u]={& _temp2500}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2499, sizeof( void*), 1u));}});
goto _LL2458; _LL2458:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2503=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp2503->f1= tv; _temp2503->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2504=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2504->v=( void*) k; _temp2504;}),({ struct Cyc_Core_Opt*
_temp2505=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2505->v=( void*) s; _temp2505;})); _temp2503;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2508;
struct _RegionHandle* _temp2509; struct Cyc_List_List* _temp2511; struct _tuple6*
_temp2506= env; _temp2508=* _temp2506; _LL2512: _temp2511= _temp2508.f1; goto
_LL2510; _LL2510: _temp2509= _temp2508.f2; goto _LL2507; _LL2507: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2513=( struct _tuple5*) _region_malloc(
_temp2509, sizeof( struct _tuple5)); _temp2513->f1= tv; _temp2513->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2514=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2514->v=( void*) k; _temp2514;}),({ struct Cyc_Core_Opt*
_temp2515=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2515->v=( void*) _temp2511; _temp2515;})); _temp2513;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_Std_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_Std_String_pa_struct _temp2519; _temp2519.tag= Cyc_Std_String_pa;
_temp2519.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct
_temp2518; _temp2518.tag= Cyc_Std_String_pa; _temp2518.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_Std_String_pa_struct _temp2517;
_temp2517.tag= Cyc_Std_String_pa; _temp2517.f1=( struct _tagged_arr)* tv->name;{
void* _temp2516[ 3u]={& _temp2517,& _temp2518,& _temp2519}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2516, sizeof( void*), 3u));}}}});} if( tv->identity
==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!= *(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2520[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2520, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2521=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2521->hd=( void*) tv; _temp2521->tl= tvs; _temp2521;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2522[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2522, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity ==  0){({ void* _temp2523[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2523, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2524=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2524->hd=( void*) tv; _temp2524->tl=
tvs; _temp2524;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp2526; _temp2526.tag= Cyc_Std_String_pa;
_temp2526.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2525[
1u]={& _temp2526};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2525, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2527=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2527->hd=(
void*) tv; _temp2527->tl= tvs; _temp2527;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2528= Cyc_Tcutil_compress( e);
int _temp2534; _LL2530: if(( unsigned int) _temp2528 >  4u?*(( int*) _temp2528)
==  Cyc_Absyn_Evar: 0){ _LL2535: _temp2534=(( struct Cyc_Absyn_Evar_struct*)
_temp2528)->f3; goto _LL2531;} else{ goto _LL2532;} _LL2532: goto _LL2533;
_LL2531:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2536= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2542; _LL2538: if((
unsigned int) _temp2536 >  4u?*(( int*) _temp2536) ==  Cyc_Absyn_Evar: 0){
_LL2543: _temp2542=(( struct Cyc_Absyn_Evar_struct*) _temp2536)->f3; goto
_LL2539;} else{ goto _LL2540;} _LL2540: goto _LL2541; _LL2539: if( _temp2534 == 
_temp2542){ return es;} goto _LL2537; _LL2541: goto _LL2537; _LL2537:;}} return({
struct Cyc_List_List* _temp2544=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2544->hd=( void*) e; _temp2544->tl= es; _temp2544;});
_LL2533: return es; _LL2529:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b !=  0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) == *(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2545=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2545->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2545->tl= r; _temp2545;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp2547; _temp2547.tag= Cyc_Std_String_pa; _temp2547.f1=( struct _tagged_arr)*
fn;{ void* _temp2546[ 1u]={& _temp2547}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2546, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2548= Cyc_Tcutil_compress( field_typ); void* _temp2554;
_LL2550: if(( unsigned int) _temp2548 >  4u?*(( int*) _temp2548) ==  Cyc_Absyn_IntType:
0){ _LL2555: _temp2554=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2548)->f2;
goto _LL2551;} else{ goto _LL2552;} _LL2552: goto _LL2553; _LL2551:{ void*
_temp2556= _temp2554; _LL2558: if( _temp2556 == ( void*) Cyc_Absyn_B1){ goto
_LL2559;} else{ goto _LL2560;} _LL2560: if( _temp2556 == ( void*) Cyc_Absyn_B2){
goto _LL2561;} else{ goto _LL2562;} _LL2562: if( _temp2556 == ( void*) Cyc_Absyn_B4){
goto _LL2563;} else{ goto _LL2564;} _LL2564: if( _temp2556 == ( void*) Cyc_Absyn_B8){
goto _LL2565;} else{ goto _LL2557;} _LL2559: if( w >  8){({ void* _temp2566[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2566, sizeof( void*), 0u));});} goto
_LL2557; _LL2561: if( w >  16){({ void* _temp2567[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2567, sizeof( void*), 0u));});} goto _LL2557; _LL2563: if( w >  32){({ void*
_temp2568[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2568, sizeof( void*), 0u));});}
goto _LL2557; _LL2565: if( w >  64){({ void* _temp2569[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2569, sizeof( void*), 0u));});} goto _LL2557; _LL2557:;} goto
_LL2549; _LL2553:({ struct Cyc_Std_String_pa_struct _temp2572; _temp2572.tag=
Cyc_Std_String_pa; _temp2572.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp2571; _temp2571.tag= Cyc_Std_String_pa;
_temp2571.f1=( struct _tagged_arr)* fn;{ void* _temp2570[ 2u]={& _temp2571,&
_temp2572}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2570, sizeof( void*), 2u));}}});
goto _LL2549; _LL2549:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp2573=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL2575: if( _temp2573 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL2576;} else{ goto _LL2577;} _LL2577: if((
unsigned int) _temp2573 >  16u?*(( int*) _temp2573) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2578;} else{ goto _LL2579;} _LL2579: goto _LL2580; _LL2576: continue;
_LL2578: continue; _LL2580:({ struct Cyc_Std_String_pa_struct _temp2583;
_temp2583.tag= Cyc_Std_String_pa; _temp2583.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp2582; _temp2582.tag= Cyc_Std_String_pa;
_temp2582.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp2581[ 2u]={& _temp2582,&
_temp2583}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2581, sizeof( void*), 2u));}}});
goto _LL2574; _LL2574:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv
cvtenv, void* expected_kind, void* t){{ void* _temp2584= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp2632; struct Cyc_Core_Opt** _temp2634; struct Cyc_Core_Opt*
_temp2635; struct Cyc_Core_Opt** _temp2637; struct Cyc_Absyn_Tvar* _temp2638;
struct Cyc_Absyn_Enumdecl* _temp2640; struct Cyc_Absyn_Enumdecl** _temp2642;
struct _tuple1* _temp2643; struct Cyc_Absyn_TunionInfo _temp2645; void*
_temp2647; struct Cyc_List_List* _temp2649; void* _temp2651; void** _temp2653;
struct Cyc_Absyn_TunionFieldInfo _temp2654; struct Cyc_List_List* _temp2656;
void* _temp2658; void** _temp2660; struct Cyc_Absyn_PtrInfo _temp2661; struct
Cyc_Absyn_Conref* _temp2663; struct Cyc_Absyn_Tqual _temp2665; struct Cyc_Absyn_Conref*
_temp2667; void* _temp2669; void* _temp2671; struct Cyc_Absyn_Exp* _temp2673;
struct Cyc_Absyn_Tqual _temp2675; void* _temp2677; struct Cyc_Absyn_FnInfo
_temp2679; struct Cyc_List_List* _temp2681; struct Cyc_List_List* _temp2683;
struct Cyc_Absyn_VarargInfo* _temp2685; int _temp2687; struct Cyc_List_List*
_temp2689; void* _temp2691; struct Cyc_Core_Opt* _temp2693; struct Cyc_Core_Opt**
_temp2695; struct Cyc_List_List* _temp2696; struct Cyc_List_List** _temp2698;
struct Cyc_List_List* _temp2699; struct Cyc_List_List* _temp2701; struct Cyc_List_List*
_temp2703; struct Cyc_Absyn_Structdecl** _temp2705; struct Cyc_Absyn_Structdecl***
_temp2707; struct Cyc_List_List* _temp2708; struct _tuple1* _temp2710; struct
Cyc_Absyn_Uniondecl** _temp2712; struct Cyc_Absyn_Uniondecl*** _temp2714; struct
Cyc_List_List* _temp2715; struct _tuple1* _temp2717; struct Cyc_Core_Opt*
_temp2719; struct Cyc_Core_Opt** _temp2721; struct Cyc_List_List* _temp2722;
struct Cyc_List_List** _temp2724; struct _tuple1* _temp2725; void* _temp2727;
void* _temp2729; void* _temp2731; struct Cyc_List_List* _temp2733; _LL2586: if(
_temp2584 == ( void*) Cyc_Absyn_VoidType){ goto _LL2587;} else{ goto _LL2588;}
_LL2588: if(( unsigned int) _temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_Evar:
0){ _LL2636: _temp2635=(( struct Cyc_Absyn_Evar_struct*) _temp2584)->f1;
_temp2637=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2584)->f1;
goto _LL2633; _LL2633: _temp2632=(( struct Cyc_Absyn_Evar_struct*) _temp2584)->f2;
_temp2634=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2584)->f2;
goto _LL2589;} else{ goto _LL2590;} _LL2590: if(( unsigned int) _temp2584 >  4u?*((
int*) _temp2584) ==  Cyc_Absyn_VarType: 0){ _LL2639: _temp2638=(( struct Cyc_Absyn_VarType_struct*)
_temp2584)->f1; goto _LL2591;} else{ goto _LL2592;} _LL2592: if(( unsigned int)
_temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_EnumType: 0){ _LL2644:
_temp2643=(( struct Cyc_Absyn_EnumType_struct*) _temp2584)->f1; goto _LL2641;
_LL2641: _temp2640=(( struct Cyc_Absyn_EnumType_struct*) _temp2584)->f2;
_temp2642=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2584)->f2; goto _LL2593;} else{ goto _LL2594;} _LL2594: if(( unsigned int)
_temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_TunionType: 0){ _LL2646:
_temp2645=(( struct Cyc_Absyn_TunionType_struct*) _temp2584)->f1; _LL2652:
_temp2651=( void*) _temp2645.tunion_info; _temp2653=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2584)->f1).tunion_info; goto _LL2650; _LL2650: _temp2649= _temp2645.targs;
goto _LL2648; _LL2648: _temp2647=( void*) _temp2645.rgn; goto _LL2595;} else{
goto _LL2596;} _LL2596: if(( unsigned int) _temp2584 >  4u?*(( int*) _temp2584)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2655: _temp2654=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2584)->f1; _LL2659: _temp2658=( void*) _temp2654.field_info; _temp2660=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2584)->f1).field_info;
goto _LL2657; _LL2657: _temp2656= _temp2654.targs; goto _LL2597;} else{ goto
_LL2598;} _LL2598: if(( unsigned int) _temp2584 >  4u?*(( int*) _temp2584) == 
Cyc_Absyn_PointerType: 0){ _LL2662: _temp2661=(( struct Cyc_Absyn_PointerType_struct*)
_temp2584)->f1; _LL2672: _temp2671=( void*) _temp2661.elt_typ; goto _LL2670;
_LL2670: _temp2669=( void*) _temp2661.rgn_typ; goto _LL2668; _LL2668: _temp2667=
_temp2661.nullable; goto _LL2666; _LL2666: _temp2665= _temp2661.tq; goto _LL2664;
_LL2664: _temp2663= _temp2661.bounds; goto _LL2599;} else{ goto _LL2600;}
_LL2600: if(( unsigned int) _temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_IntType:
0){ goto _LL2601;} else{ goto _LL2602;} _LL2602: if( _temp2584 == ( void*) Cyc_Absyn_FloatType){
goto _LL2603;} else{ goto _LL2604;} _LL2604: if( _temp2584 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2605;} else{ goto _LL2606;} _LL2606: if(( unsigned int) _temp2584 >  4u?*((
int*) _temp2584) ==  Cyc_Absyn_ArrayType: 0){ _LL2678: _temp2677=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2584)->f1; goto _LL2676; _LL2676:
_temp2675=(( struct Cyc_Absyn_ArrayType_struct*) _temp2584)->f2; goto _LL2674;
_LL2674: _temp2673=(( struct Cyc_Absyn_ArrayType_struct*) _temp2584)->f3; goto
_LL2607;} else{ goto _LL2608;} _LL2608: if(( unsigned int) _temp2584 >  4u?*((
int*) _temp2584) ==  Cyc_Absyn_FnType: 0){ _LL2680: _temp2679=(( struct Cyc_Absyn_FnType_struct*)
_temp2584)->f1; _LL2697: _temp2696= _temp2679.tvars; _temp2698=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2584)->f1).tvars; goto _LL2694; _LL2694:
_temp2693= _temp2679.effect; _temp2695=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2584)->f1).effect; goto _LL2692; _LL2692: _temp2691=( void*) _temp2679.ret_typ;
goto _LL2690; _LL2690: _temp2689= _temp2679.args; goto _LL2688; _LL2688:
_temp2687= _temp2679.c_varargs; goto _LL2686; _LL2686: _temp2685= _temp2679.cyc_varargs;
goto _LL2684; _LL2684: _temp2683= _temp2679.rgn_po; goto _LL2682; _LL2682:
_temp2681= _temp2679.attributes; goto _LL2609;} else{ goto _LL2610;} _LL2610:
if(( unsigned int) _temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_TupleType:
0){ _LL2700: _temp2699=(( struct Cyc_Absyn_TupleType_struct*) _temp2584)->f1;
goto _LL2611;} else{ goto _LL2612;} _LL2612: if(( unsigned int) _temp2584 >  4u?*((
int*) _temp2584) ==  Cyc_Absyn_AnonStructType: 0){ _LL2702: _temp2701=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2584)->f1; goto _LL2613;} else{ goto
_LL2614;} _LL2614: if(( unsigned int) _temp2584 >  4u?*(( int*) _temp2584) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2704: _temp2703=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2584)->f1; goto _LL2615;} else{ goto _LL2616;} _LL2616: if(( unsigned int)
_temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_StructType: 0){ _LL2711:
_temp2710=(( struct Cyc_Absyn_StructType_struct*) _temp2584)->f1; goto _LL2709;
_LL2709: _temp2708=(( struct Cyc_Absyn_StructType_struct*) _temp2584)->f2; goto
_LL2706; _LL2706: _temp2705=(( struct Cyc_Absyn_StructType_struct*) _temp2584)->f3;
_temp2707=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2584)->f3; goto _LL2617;} else{ goto _LL2618;} _LL2618: if(( unsigned int)
_temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_UnionType: 0){ _LL2718:
_temp2717=(( struct Cyc_Absyn_UnionType_struct*) _temp2584)->f1; goto _LL2716;
_LL2716: _temp2715=(( struct Cyc_Absyn_UnionType_struct*) _temp2584)->f2; goto
_LL2713; _LL2713: _temp2712=(( struct Cyc_Absyn_UnionType_struct*) _temp2584)->f3;
_temp2714=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2584)->f3; goto _LL2619;} else{ goto _LL2620;} _LL2620: if(( unsigned int)
_temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_TypedefType: 0){ _LL2726:
_temp2725=(( struct Cyc_Absyn_TypedefType_struct*) _temp2584)->f1; goto _LL2723;
_LL2723: _temp2722=(( struct Cyc_Absyn_TypedefType_struct*) _temp2584)->f2;
_temp2724=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2584)->f2; goto _LL2720; _LL2720: _temp2719=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2584)->f3; _temp2721=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2584)->f3; goto _LL2621;} else{ goto _LL2622;} _LL2622: if( _temp2584 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2623;} else{ goto _LL2624;} _LL2624: if((
unsigned int) _temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2728: _temp2727=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2584)->f1; goto _LL2625;} else{ goto _LL2626;} _LL2626: if(( unsigned int)
_temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_AccessEff: 0){ _LL2730:
_temp2729=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2584)->f1; goto
_LL2627;} else{ goto _LL2628;} _LL2628: if(( unsigned int) _temp2584 >  4u?*((
int*) _temp2584) ==  Cyc_Absyn_RgnsEff: 0){ _LL2732: _temp2731=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2584)->f1; goto _LL2629;} else{ goto _LL2630;}
_LL2630: if(( unsigned int) _temp2584 >  4u?*(( int*) _temp2584) ==  Cyc_Absyn_JoinEff:
0){ _LL2734: _temp2733=(( struct Cyc_Absyn_JoinEff_struct*) _temp2584)->f1; goto
_LL2631;} else{ goto _LL2585;} _LL2587: goto _LL2585; _LL2589: if(* _temp2637 == 
0){* _temp2637=({ struct Cyc_Core_Opt* _temp2735=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2735->v=( void*) expected_kind;
_temp2735;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2634=({ struct Cyc_Core_Opt* _temp2736=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2736->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2736;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2737= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2634=({ struct Cyc_Core_Opt* _temp2738=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2738->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2739=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2739[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2740; _temp2740.tag= Cyc_Absyn_VarType;
_temp2740.f1= _temp2737; _temp2740;}); _temp2739;})); _temp2738;}); _temp2638=
_temp2737; goto _LL2591;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2585; _LL2591: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2638->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2741=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2741[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2742; _temp2742.tag= Cyc_Absyn_Eq_constr;
_temp2742.f1=( void*) expected_kind; _temp2742;}); _temp2741;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2638); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2638); goto _LL2585;}
_LL2593: if(* _temp2642 ==  0){ struct _handler_cons _temp2743; _push_handler(&
_temp2743);{ int _temp2745= 0; if( setjmp( _temp2743.handler)){ _temp2745= 1;}
if( ! _temp2745){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2643);* _temp2642=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2744=( void*) _exn_thrown; void* _temp2747= _temp2744; _LL2749:
if( _temp2747 ==  Cyc_Dict_Absent){ goto _LL2750;} else{ goto _LL2751;} _LL2751:
goto _LL2752; _LL2750:({ struct Cyc_Std_String_pa_struct _temp2754; _temp2754.tag=
Cyc_Std_String_pa; _temp2754.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2643);{ void* _temp2753[ 1u]={& _temp2754}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u), _tag_arr( _temp2753, sizeof( void*), 1u));}});
return cvtenv; _LL2752:( void) _throw( _temp2747); _LL2748:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2642);* _temp2643=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2585;} _LL2595:{ void*
_temp2755=* _temp2653; struct Cyc_Absyn_UnknownTunionInfo _temp2761; int
_temp2763; struct _tuple1* _temp2765; struct Cyc_Absyn_Tuniondecl** _temp2767;
struct Cyc_Absyn_Tuniondecl* _temp2769; _LL2757: if(*(( int*) _temp2755) ==  Cyc_Absyn_UnknownTunion){
_LL2762: _temp2761=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2755)->f1;
_LL2766: _temp2765= _temp2761.name; goto _LL2764; _LL2764: _temp2763= _temp2761.is_xtunion;
goto _LL2758;} else{ goto _LL2759;} _LL2759: if(*(( int*) _temp2755) ==  Cyc_Absyn_KnownTunion){
_LL2768: _temp2767=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2755)->f1;
_temp2769=* _temp2767; goto _LL2760;} else{ goto _LL2756;} _LL2758: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp2770; _push_handler(& _temp2770);{ int
_temp2772= 0; if( setjmp( _temp2770.handler)){ _temp2772= 1;} if( ! _temp2772){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp2765);; _pop_handler();} else{
void* _temp2771=( void*) _exn_thrown; void* _temp2774= _temp2771; _LL2776: if(
_temp2774 ==  Cyc_Dict_Absent){ goto _LL2777;} else{ goto _LL2778;} _LL2778:
goto _LL2779; _LL2777:({ struct Cyc_Std_String_pa_struct _temp2781; _temp2781.tag=
Cyc_Std_String_pa; _temp2781.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2765);{ void* _temp2780[ 1u]={& _temp2781}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type [x]tunion %s",
sizeof( unsigned char), 26u), _tag_arr( _temp2780, sizeof( void*), 1u));}});
return cvtenv; _LL2779:( void) _throw( _temp2774); _LL2775:;}}} if((* tudp)->is_xtunion
!=  _temp2763){({ struct Cyc_Std_String_pa_struct _temp2783; _temp2783.tag= Cyc_Std_String_pa;
_temp2783.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2765);{ void*
_temp2782[ 1u]={& _temp2783}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2782, sizeof( void*), 1u));}});}*
_temp2653=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2784=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2784[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2785; _temp2785.tag= Cyc_Absyn_KnownTunion;
_temp2785.f1= tudp; _temp2785;}); _temp2784;}); _temp2769=* tudp; goto _LL2760;}
_LL2760: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2647);{ struct Cyc_List_List* tvs= _temp2769->tvs; for( 0; _temp2649 !=  0?
tvs !=  0: 0;( _temp2649=(( struct Cyc_List_List*) _check_null( _temp2649))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2649))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2649
!=  0){({ struct Cyc_Std_String_pa_struct _temp2787; _temp2787.tag= Cyc_Std_String_pa;
_temp2787.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2769->name);{
void* _temp2786[ 1u]={& _temp2787}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2786, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2789; _temp2789.tag= Cyc_Std_String_pa;
_temp2789.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2769->name);{
void* _temp2788[ 1u]={& _temp2789}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2788, sizeof( void*), 1u));}});}
goto _LL2756;} _LL2756:;} goto _LL2585; _LL2597:{ void* _temp2790=* _temp2660;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2796; int _temp2798; struct _tuple1*
_temp2800; struct _tuple1* _temp2802; struct Cyc_Absyn_Tunionfield* _temp2804;
struct Cyc_Absyn_Tuniondecl* _temp2806; _LL2792: if(*(( int*) _temp2790) ==  Cyc_Absyn_UnknownTunionfield){
_LL2797: _temp2796=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2790)->f1;
_LL2803: _temp2802= _temp2796.tunion_name; goto _LL2801; _LL2801: _temp2800=
_temp2796.field_name; goto _LL2799; _LL2799: _temp2798= _temp2796.is_xtunion;
goto _LL2793;} else{ goto _LL2794;} _LL2794: if(*(( int*) _temp2790) ==  Cyc_Absyn_KnownTunionfield){
_LL2807: _temp2806=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2790)->f1;
goto _LL2805; _LL2805: _temp2804=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2790)->f2; goto _LL2795;} else{ goto _LL2791;} _LL2793: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2808;
_push_handler(& _temp2808);{ int _temp2810= 0; if( setjmp( _temp2808.handler)){
_temp2810= 1;} if( ! _temp2810){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2802);; _pop_handler();} else{ void* _temp2809=( void*) _exn_thrown; void*
_temp2812= _temp2809; _LL2814: if( _temp2812 ==  Cyc_Dict_Absent){ goto _LL2815;}
else{ goto _LL2816;} _LL2816: goto _LL2817; _LL2815:({ struct Cyc_Std_String_pa_struct
_temp2819; _temp2819.tag= Cyc_Std_String_pa; _temp2819.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2802);{ void* _temp2818[ 1u]={& _temp2819}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2818, sizeof( void*), 1u));}}); return cvtenv; _LL2817:( void) _throw(
_temp2812); _LL2813:;}}}{ struct _handler_cons _temp2820; _push_handler(&
_temp2820);{ int _temp2822= 0; if( setjmp( _temp2820.handler)){ _temp2822= 1;}
if( ! _temp2822){{ void* _temp2823= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2800); struct Cyc_Absyn_Tunionfield* _temp2829; struct Cyc_Absyn_Tuniondecl*
_temp2831; _LL2825: if(*(( int*) _temp2823) ==  Cyc_Tcenv_TunionRes){ _LL2832:
_temp2831=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2823)->f1; goto _LL2830;
_LL2830: _temp2829=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2823)->f2; goto
_LL2826;} else{ goto _LL2827;} _LL2827: goto _LL2828; _LL2826: tuf= _temp2829;
tud= _temp2831; if( tud->is_xtunion !=  _temp2798){({ struct Cyc_Std_String_pa_struct
_temp2834; _temp2834.tag= Cyc_Std_String_pa; _temp2834.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2802);{ void* _temp2833[ 1u]={& _temp2834}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2833, sizeof( void*), 1u));}});} goto
_LL2824; _LL2828:({ struct Cyc_Std_String_pa_struct _temp2837; _temp2837.tag=
Cyc_Std_String_pa; _temp2837.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2802);{ struct Cyc_Std_String_pa_struct _temp2836; _temp2836.tag= Cyc_Std_String_pa;
_temp2836.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2800);{ void*
_temp2835[ 2u]={& _temp2836,& _temp2837}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2835, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2838= cvtenv; _npop_handler( 0u); return _temp2838;}
_LL2824:;}; _pop_handler();} else{ void* _temp2821=( void*) _exn_thrown; void*
_temp2840= _temp2821; _LL2842: if( _temp2840 ==  Cyc_Dict_Absent){ goto _LL2843;}
else{ goto _LL2844;} _LL2844: goto _LL2845; _LL2843:({ struct Cyc_Std_String_pa_struct
_temp2848; _temp2848.tag= Cyc_Std_String_pa; _temp2848.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2802);{ struct Cyc_Std_String_pa_struct _temp2847;
_temp2847.tag= Cyc_Std_String_pa; _temp2847.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2800);{ void* _temp2846[ 2u]={& _temp2847,& _temp2848}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2846, sizeof( void*), 2u));}}}); return cvtenv; _LL2845:( void)
_throw( _temp2840); _LL2841:;}}}* _temp2660=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2849=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2849[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2850; _temp2850.tag= Cyc_Absyn_KnownTunionfield; _temp2850.f1= tud;
_temp2850.f2= tuf; _temp2850;}); _temp2849;}); _temp2806= tud; _temp2804= tuf;
goto _LL2795;} _LL2795: { struct Cyc_List_List* tvs= _temp2806->tvs; for( 0;
_temp2656 !=  0? tvs !=  0: 0;( _temp2656=(( struct Cyc_List_List*) _check_null(
_temp2656))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2656))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2656
!=  0){({ struct Cyc_Std_String_pa_struct _temp2853; _temp2853.tag= Cyc_Std_String_pa;
_temp2853.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2804->name);{
struct Cyc_Std_String_pa_struct _temp2852; _temp2852.tag= Cyc_Std_String_pa;
_temp2852.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2806->name);{
void* _temp2851[ 2u]={& _temp2852,& _temp2853}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2851, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2856; _temp2856.tag= Cyc_Std_String_pa;
_temp2856.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2804->name);{
struct Cyc_Std_String_pa_struct _temp2855; _temp2855.tag= Cyc_Std_String_pa;
_temp2855.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2806->name);{
void* _temp2854[ 2u]={& _temp2855,& _temp2856}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2854, sizeof( void*), 2u));}}});}
goto _LL2791;} _LL2791:;} goto _LL2585; _LL2599: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2671); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2669);{ void* _temp2857=( void*)(
Cyc_Absyn_compress_conref( _temp2663))->v; void* _temp2863; struct Cyc_Absyn_Exp*
_temp2865; _LL2859: if(( unsigned int) _temp2857 >  1u?*(( int*) _temp2857) == 
Cyc_Absyn_Eq_constr: 0){ _LL2864: _temp2863=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2857)->f1; if(( unsigned int) _temp2863 >  1u?*(( int*) _temp2863) ==  Cyc_Absyn_Upper_b:
0){ _LL2866: _temp2865=(( struct Cyc_Absyn_Upper_b_struct*) _temp2863)->f1; goto
_LL2860;} else{ goto _LL2861;}} else{ goto _LL2861;} _LL2861: goto _LL2862;
_LL2860: if( ! Cyc_Tcutil_is_const_exp( te, _temp2865)){({ void* _temp2867[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2867, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2865); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2865)){({ void*
_temp2868[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2868, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2865); goto _LL2858; _LL2862: goto _LL2858; _LL2858:;} goto _LL2585;
_LL2601: goto _LL2585; _LL2603: goto _LL2585; _LL2605: goto _LL2585; _LL2607:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2677); if( _temp2673 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2673))){({ void* _temp2869[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2869, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2673)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2673))){({ void* _temp2870[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2870, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2673)); goto _LL2585; _LL2609: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2681 !=  0; _temp2681=(( struct Cyc_List_List*)
_check_null( _temp2681))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2681))->hd)){({ struct Cyc_Std_String_pa_struct _temp2872;
_temp2872.tag= Cyc_Std_String_pa; _temp2872.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2681))->hd);{ void* _temp2871[
1u]={& _temp2872}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2871, sizeof( void*), 1u));}});}{
void* _temp2873=( void*)(( struct Cyc_List_List*) _check_null( _temp2681))->hd;
int _temp2885; int _temp2887; void* _temp2889; _LL2875: if( _temp2873 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2876;} else{ goto _LL2877;} _LL2877: if(
_temp2873 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL2878;} else{ goto _LL2879;}
_LL2879: if( _temp2873 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL2880;} else{
goto _LL2881;} _LL2881: if(( unsigned int) _temp2873 >  16u?*(( int*) _temp2873)
==  Cyc_Absyn_Format_att: 0){ _LL2890: _temp2889=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2873)->f1; goto _LL2888; _LL2888: _temp2887=(( struct Cyc_Absyn_Format_att_struct*)
_temp2873)->f2; goto _LL2886; _LL2886: _temp2885=(( struct Cyc_Absyn_Format_att_struct*)
_temp2873)->f3; goto _LL2882;} else{ goto _LL2883;} _LL2883: goto _LL2884;
_LL2876: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2874;
_LL2878: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2874; _LL2880:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2874; _LL2882:
if( ! seen_format){ seen_format= 1; ft= _temp2889; fmt_desc_arg= _temp2887;
fmt_arg_start= _temp2885;} else{({ void* _temp2891[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2891, sizeof( void*), 0u));});} goto _LL2874; _LL2884: goto
_LL2874; _LL2874:;}} if( num_convs >  1){({ void* _temp2892[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2892, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2698);{ struct Cyc_List_List* b=* _temp2698; for( 0; b !=  0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2893=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2899; _LL2895: if(( unsigned int) _temp2893 >  1u?*(( int*) _temp2893)
==  Cyc_Absyn_Eq_constr: 0){ _LL2900: _temp2899=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2893)->f1; if( _temp2899 == ( void*) Cyc_Absyn_MemKind){ goto _LL2896;}
else{ goto _LL2897;}} else{ goto _LL2897;} _LL2897: goto _LL2898; _LL2896:({
struct Cyc_Std_String_pa_struct _temp2902; _temp2902.tag= Cyc_Std_String_pa;
_temp2902.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2901[ 1u]={& _temp2902}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2901, sizeof( void*), 1u));}}); goto
_LL2894; _LL2898: goto _LL2894; _LL2894:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2903=({
struct Cyc_Tcutil_CVTEnv _temp3062; _temp3062.kind_env= cvtenv.kind_env;
_temp3062.free_vars= 0; _temp3062.free_evars= 0; _temp3062.generalize_evars=
cvtenv.generalize_evars; _temp3062.fn_result= 1; _temp3062;}); _temp2903= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2903,( void*) Cyc_Absyn_MemKind, _temp2691); _temp2903.fn_result=
0;{ struct Cyc_List_List* a= _temp2689; for( 0; a !=  0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ _temp2903= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2903,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( a))->hd)).f3);}} if( _temp2685 !=  0){ if( _temp2687){({ void*
_temp2904[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2904, sizeof( void*), 0u));});}{ int _temp2907; void* _temp2909; struct Cyc_Absyn_Tqual
_temp2911; struct Cyc_Core_Opt* _temp2913; struct Cyc_Absyn_VarargInfo _temp2905=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp2685)); _LL2914: _temp2913=
_temp2905.name; goto _LL2912; _LL2912: _temp2911= _temp2905.tq; goto _LL2910;
_LL2910: _temp2909=( void*) _temp2905.type; goto _LL2908; _LL2908: _temp2907=
_temp2905.inject; goto _LL2906; _LL2906: _temp2903= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2903,( void*) Cyc_Absyn_MemKind, _temp2909); if( _temp2907){ void*
_temp2915= Cyc_Tcutil_compress( _temp2909); struct Cyc_Absyn_TunionInfo
_temp2921; void* _temp2923; void* _temp2925; _LL2917: if(( unsigned int)
_temp2915 >  4u?*(( int*) _temp2915) ==  Cyc_Absyn_TunionType: 0){ _LL2922:
_temp2921=(( struct Cyc_Absyn_TunionType_struct*) _temp2915)->f1; _LL2926:
_temp2925=( void*) _temp2921.tunion_info; if(*(( int*) _temp2925) ==  Cyc_Absyn_KnownTunion){
goto _LL2924;} else{ goto _LL2919;} _LL2924: _temp2923=( void*) _temp2921.rgn;
goto _LL2918;} else{ goto _LL2919;} _LL2919: goto _LL2920; _LL2918: goto _LL2916;
_LL2920:({ void* _temp2927[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u), _tag_arr( _temp2927, sizeof( void*), 0u));}); goto
_LL2916; _LL2916:;}}} if( seen_format){ int _temp2928=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2689); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg > 
_temp2928)? 1: fmt_arg_start <  0)? 1:( _temp2685 ==  0? fmt_arg_start !=  0: 0))?
1:( _temp2685 !=  0? fmt_arg_start !=  _temp2928 +  1: 0)){({ void* _temp2929[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2929, sizeof( void*), 0u));});} else{ void* _temp2932;
struct _tuple2 _temp2930=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp2689, fmt_desc_arg -  1); _LL2933: _temp2932= _temp2930.f3;
goto _LL2931; _LL2931:{ void* _temp2934= Cyc_Tcutil_compress( _temp2932); struct
Cyc_Absyn_PtrInfo _temp2940; struct Cyc_Absyn_Conref* _temp2942; void* _temp2944;
_LL2936: if(( unsigned int) _temp2934 >  4u?*(( int*) _temp2934) ==  Cyc_Absyn_PointerType:
0){ _LL2941: _temp2940=(( struct Cyc_Absyn_PointerType_struct*) _temp2934)->f1;
_LL2945: _temp2944=( void*) _temp2940.elt_typ; goto _LL2943; _LL2943: _temp2942=
_temp2940.bounds; goto _LL2937;} else{ goto _LL2938;} _LL2938: goto _LL2939;
_LL2937:{ struct _tuple8 _temp2947=({ struct _tuple8 _temp2946; _temp2946.f1=
Cyc_Tcutil_compress( _temp2944); _temp2946.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2942); _temp2946;}); void* _temp2953; void* _temp2955; void* _temp2957;
void* _temp2959; _LL2949: _LL2956: _temp2955= _temp2947.f1; if(( unsigned int)
_temp2955 >  4u?*(( int*) _temp2955) ==  Cyc_Absyn_IntType: 0){ _LL2960:
_temp2959=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2955)->f1; if(
_temp2959 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2958;} else{ goto _LL2951;}
_LL2958: _temp2957=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2955)->f2;
if( _temp2957 == ( void*) Cyc_Absyn_B1){ goto _LL2954;} else{ goto _LL2951;}}
else{ goto _LL2951;} _LL2954: _temp2953= _temp2947.f2; if( _temp2953 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2950;} else{ goto _LL2951;} _LL2951: goto _LL2952;
_LL2950: goto _LL2948; _LL2952:({ void* _temp2961[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp2961, sizeof( void*), 0u));}); goto _LL2948; _LL2948:;} goto
_LL2935; _LL2939:({ void* _temp2962[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2962, sizeof( void*), 0u));}); goto
_LL2935; _LL2935:;} if( fmt_arg_start !=  0){ void* _temp2963= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2685))->type); int
problem;{ void* _temp2964= ft; _LL2966: if( _temp2964 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL2967;} else{ goto _LL2968;} _LL2968: if( _temp2964 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL2969;} else{ goto _LL2965;} _LL2967:{ void* _temp2970= _temp2963; struct
Cyc_Absyn_TunionInfo _temp2976; void* _temp2978; struct Cyc_Absyn_Tuniondecl**
_temp2980; struct Cyc_Absyn_Tuniondecl* _temp2982; _LL2972: if(( unsigned int)
_temp2970 >  4u?*(( int*) _temp2970) ==  Cyc_Absyn_TunionType: 0){ _LL2977:
_temp2976=(( struct Cyc_Absyn_TunionType_struct*) _temp2970)->f1; _LL2979:
_temp2978=( void*) _temp2976.tunion_info; if(*(( int*) _temp2978) ==  Cyc_Absyn_KnownTunion){
_LL2981: _temp2980=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2978)->f1;
_temp2982=* _temp2980; goto _LL2973;} else{ goto _LL2974;}} else{ goto _LL2974;}
_LL2974: goto _LL2975; _LL2973: problem= Cyc_Absyn_qvar_cmp( _temp2982->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL2971; _LL2975: problem= 1; goto
_LL2971; _LL2971:;} goto _LL2965; _LL2969:{ void* _temp2983= _temp2963; struct
Cyc_Absyn_TunionInfo _temp2989; void* _temp2991; struct Cyc_Absyn_Tuniondecl**
_temp2993; struct Cyc_Absyn_Tuniondecl* _temp2995; _LL2985: if(( unsigned int)
_temp2983 >  4u?*(( int*) _temp2983) ==  Cyc_Absyn_TunionType: 0){ _LL2990:
_temp2989=(( struct Cyc_Absyn_TunionType_struct*) _temp2983)->f1; _LL2992:
_temp2991=( void*) _temp2989.tunion_info; if(*(( int*) _temp2991) ==  Cyc_Absyn_KnownTunion){
_LL2994: _temp2993=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2991)->f1;
_temp2995=* _temp2993; goto _LL2986;} else{ goto _LL2987;}} else{ goto _LL2987;}
_LL2987: goto _LL2988; _LL2986: problem= Cyc_Absyn_qvar_cmp( _temp2995->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL2984; _LL2988: problem= 1; goto
_LL2984; _LL2984:;} goto _LL2965; _LL2965:;} if( problem){({ void* _temp2996[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp2996, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2683; for( 0; rpo !=  0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp2999; void* _temp3000; void*
_temp3002; struct _tuple8* _temp2997=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp2999=* _temp2997; _LL3003: _temp3002= _temp2999.f1;
goto _LL3001; _LL3001: _temp3000= _temp2999.f2; goto _LL2998; _LL2998: _temp2903=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2903,( void*) Cyc_Absyn_RgnKind,
_temp3002); _temp2903= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2903,( void*)
Cyc_Absyn_RgnKind, _temp3000);}} if(* _temp2695 !=  0){ _temp2903= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2903,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2695))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2903.free_vars; for(
0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void*
_temp3004=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3012; void*
_temp3014; _LL3006: if(( unsigned int) _temp3004 >  1u?*(( int*) _temp3004) == 
Cyc_Absyn_Eq_constr: 0){ _LL3013: _temp3012=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3004)->f1; if( _temp3012 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3007;}
else{ goto _LL3008;}} else{ goto _LL3008;} _LL3008: if(( unsigned int) _temp3004
>  1u?*(( int*) _temp3004) ==  Cyc_Absyn_Eq_constr: 0){ _LL3015: _temp3014=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3004)->f1; if( _temp3014 == (
void*) Cyc_Absyn_EffKind){ goto _LL3009;} else{ goto _LL3010;}} else{ goto
_LL3010;} _LL3010: goto _LL3011; _LL3007: effect=({ struct Cyc_List_List*
_temp3016=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3016->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3017=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3017[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3018; _temp3018.tag= Cyc_Absyn_AccessEff;
_temp3018.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3019=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3019[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3020; _temp3020.tag= Cyc_Absyn_VarType;
_temp3020.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3020;}); _temp3019;})); _temp3018;}); _temp3017;})); _temp3016->tl= effect;
_temp3016;}); goto _LL3005; _LL3009: effect=({ struct Cyc_List_List* _temp3021=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3021->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3022=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3022[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3023; _temp3023.tag= Cyc_Absyn_VarType; _temp3023.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3023;}); _temp3022;})); _temp3021->tl= effect; _temp3021;}); goto _LL3005;
_LL3011: effect=({ struct Cyc_List_List* _temp3024=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3024->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3025=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3025[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3026; _temp3026.tag= Cyc_Absyn_RgnsEff; _temp3026.f1=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3027=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3027[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3028; _temp3028.tag= Cyc_Absyn_VarType; _temp3028.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3028;}); _temp3027;})); _temp3026;}); _temp3025;})); _temp3024->tl= effect;
_temp3024;}); goto _LL3005; _LL3005:;}} effect= Cyc_List_imp_rev( effect);{
struct Cyc_List_List* ts= _temp2903.free_evars; for( 0; ts !=  0; ts=(( struct
Cyc_List_List*) _check_null( ts))->tl){ void* _temp3029= Cyc_Tcutil_typ_kind((
void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _LL3031: if( _temp3029
== ( void*) Cyc_Absyn_RgnKind){ goto _LL3032;} else{ goto _LL3033;} _LL3033: if(
_temp3029 == ( void*) Cyc_Absyn_EffKind){ goto _LL3034;} else{ goto _LL3035;}
_LL3035: goto _LL3036; _LL3032: effect=({ struct Cyc_List_List* _temp3037=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3037->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3038=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3038[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3039; _temp3039.tag= Cyc_Absyn_AccessEff;
_temp3039.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3039;}); _temp3038;})); _temp3037->tl= effect; _temp3037;}); goto _LL3030;
_LL3034: effect=({ struct Cyc_List_List* _temp3040=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3040->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3040->tl= effect; _temp3040;});
goto _LL3030; _LL3036: effect=({ struct Cyc_List_List* _temp3041=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3041->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3042=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3042[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3043; _temp3043.tag= Cyc_Absyn_RgnsEff; _temp3043.f1=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _temp3043;});
_temp3042;})); _temp3041->tl= effect; _temp3041;}); goto _LL3030; _LL3030:;}}*
_temp2695=({ struct Cyc_Core_Opt* _temp3044=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3044->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3045=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3045[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3046; _temp3046.tag= Cyc_Absyn_JoinEff;
_temp3046.f1= Cyc_List_imp_rev( effect); _temp3046;}); _temp3045;})); _temp3044;});}}
if(* _temp2698 !=  0){ struct Cyc_List_List* bs=* _temp2698; for( 0; bs !=  0;
bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3047= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3048=( void*) _temp3047->v; void*
_temp3056; _LL3050: if( _temp3048 == ( void*) Cyc_Absyn_No_constr){ goto _LL3051;}
else{ goto _LL3052;} _LL3052: if(( unsigned int) _temp3048 >  1u?*(( int*)
_temp3048) ==  Cyc_Absyn_Eq_constr: 0){ _LL3057: _temp3056=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3048)->f1; if( _temp3056 == ( void*) Cyc_Absyn_MemKind){ goto _LL3053;}
else{ goto _LL3054;}} else{ goto _LL3054;} _LL3054: goto _LL3055; _LL3051:({
struct Cyc_Std_String_pa_struct _temp3059; _temp3059.tag= Cyc_Std_String_pa;
_temp3059.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp3058[ 1u]={& _temp3059}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3058, sizeof( void*), 1u));}}); goto
_LL3053; _LL3053:( void*)( _temp3047->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3060=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3060[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3061; _temp3061.tag= Cyc_Absyn_Eq_constr;
_temp3061.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3061;}); _temp3060;})));
goto _LL3049; _LL3055: goto _LL3049; _LL3049:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2903.kind_env,* _temp2698); _temp2903.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2903.free_vars,* _temp2698);{ struct Cyc_List_List* tvs= _temp2903.free_vars;
for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2903.free_evars; for( 0; evs !=  0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2585;}}
_LL2611: for( 0; _temp2699 !=  0; _temp2699=(( struct Cyc_List_List*)
_check_null( _temp2699))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp2699))->hd)).f2);} goto _LL2585; _LL2613:{ struct
_RegionHandle _temp3063= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp3063; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2701 !=  0; _temp2701=(( struct Cyc_List_List*) _check_null( _temp2701))->tl){
struct Cyc_Absyn_Structfield _temp3066; struct Cyc_List_List* _temp3067; struct
Cyc_Absyn_Exp* _temp3069; void* _temp3071; struct Cyc_Absyn_Tqual _temp3073;
struct _tagged_arr* _temp3075; struct Cyc_Absyn_Structfield* _temp3064=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2701))->hd;
_temp3066=* _temp3064; _LL3076: _temp3075= _temp3066.name; goto _LL3074; _LL3074:
_temp3073= _temp3066.tq; goto _LL3072; _LL3072: _temp3071=( void*) _temp3066.type;
goto _LL3070; _LL3070: _temp3069= _temp3066.width; goto _LL3068; _LL3068:
_temp3067= _temp3066.attributes; goto _LL3065; _LL3065: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3075)){({ struct Cyc_Std_String_pa_struct _temp3078; _temp3078.tag= Cyc_Std_String_pa;
_temp3078.f1=( struct _tagged_arr)* _temp3075;{ void* _temp3077[ 1u]={&
_temp3078}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3077, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3075, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3079=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3079->hd=( void*)
_temp3075; _temp3079->tl= prev_fields; _temp3079;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3071); Cyc_Tcutil_check_bitfield(
loc, te, _temp3071, _temp3069, _temp3075); Cyc_Tcutil_check_field_atts( loc,
_temp3075, _temp3067);}}; _pop_region( sprev_rgn);} goto _LL2585; _LL2615:{
struct _RegionHandle _temp3080= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3080; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2703 !=  0; _temp2703=(( struct Cyc_List_List*) _check_null( _temp2703))->tl){
struct Cyc_Absyn_Structfield _temp3083; struct Cyc_List_List* _temp3084; struct
Cyc_Absyn_Exp* _temp3086; void* _temp3088; struct Cyc_Absyn_Tqual _temp3090;
struct _tagged_arr* _temp3092; struct Cyc_Absyn_Structfield* _temp3081=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2703))->hd;
_temp3083=* _temp3081; _LL3093: _temp3092= _temp3083.name; goto _LL3091; _LL3091:
_temp3090= _temp3083.tq; goto _LL3089; _LL3089: _temp3088=( void*) _temp3083.type;
goto _LL3087; _LL3087: _temp3086= _temp3083.width; goto _LL3085; _LL3085:
_temp3084= _temp3083.attributes; goto _LL3082; _LL3082: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3092)){({ struct Cyc_Std_String_pa_struct _temp3095; _temp3095.tag= Cyc_Std_String_pa;
_temp3095.f1=( struct _tagged_arr)* _temp3092;{ void* _temp3094[ 1u]={&
_temp3095}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3094, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3092, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3096=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3096->hd=( void*)
_temp3092; _temp3096->tl= prev_fields; _temp3096;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3088); if( ! Cyc_Tcutil_bits_only(
_temp3088)){({ struct Cyc_Std_String_pa_struct _temp3098; _temp3098.tag= Cyc_Std_String_pa;
_temp3098.f1=( struct _tagged_arr)* _temp3092;{ void* _temp3097[ 1u]={&
_temp3098}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3097, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3088, _temp3086, _temp3092); Cyc_Tcutil_check_field_atts(
loc, _temp3092, _temp3084);}}; _pop_region( uprev_rgn);} goto _LL2585; _LL2617:
if( _temp2710 ==  0){ if(* _temp2707 ==  0){({ void* _temp3099[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3099, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2707));;}} if(*
_temp2707 ==  0){ struct _handler_cons _temp3100; _push_handler(& _temp3100);{
int _temp3102= 0; if( setjmp( _temp3100.handler)){ _temp3102= 1;} if( !
_temp3102){* _temp2707=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2710));; _pop_handler();} else{
void* _temp3101=( void*) _exn_thrown; void* _temp3104= _temp3101; _LL3106: if(
_temp3104 ==  Cyc_Dict_Absent){ goto _LL3107;} else{ goto _LL3108;} _LL3108:
goto _LL3109; _LL3107:({ struct Cyc_Std_String_pa_struct _temp3111; _temp3111.tag=
Cyc_Std_String_pa; _temp3111.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2710));{ void* _temp3110[ 1u]={& _temp3111};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type struct %s", sizeof( unsigned char),
23u), _tag_arr( _temp3110, sizeof( void*), 1u));}}); return cvtenv; _LL3109:(
void) _throw( _temp3104); _LL3105:;}}}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2707)); if( sd->name !=  0){*((
struct _tuple1*) _check_null( _temp2710))=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2708); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3115; _temp3115.tag= Cyc_Std_Int_pa; _temp3115.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3114; _temp3114.tag= Cyc_Std_Int_pa;
_temp3114.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3113; _temp3113.tag= Cyc_Std_String_pa; _temp3113.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2710));{ void*
_temp3112[ 3u]={& _temp3113,& _temp3114,& _temp3115}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3112, sizeof( void*), 3u));}}}});} for( 0;
_temp2708 !=  0;( _temp2708=(( struct Cyc_List_List*) _check_null( _temp2708))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2708))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2585;}}
_LL2619: if( _temp2717 ==  0){({ void* _temp3116[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp3116,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2714 ==  0){ struct
_handler_cons _temp3117; _push_handler(& _temp3117);{ int _temp3119= 0; if(
setjmp( _temp3117.handler)){ _temp3119= 1;} if( ! _temp3119){* _temp2714=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2717));; _pop_handler();} else{ void* _temp3118=(
void*) _exn_thrown; void* _temp3121= _temp3118; _LL3123: if( _temp3121 ==  Cyc_Dict_Absent){
goto _LL3124;} else{ goto _LL3125;} _LL3125: goto _LL3126; _LL3124:({ struct Cyc_Std_String_pa_struct
_temp3128; _temp3128.tag= Cyc_Std_String_pa; _temp3128.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2717));{ void*
_temp3127[ 1u]={& _temp3128}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type union %s",
sizeof( unsigned char), 22u), _tag_arr( _temp3127, sizeof( void*), 1u));}});
return cvtenv; _LL3126:( void) _throw( _temp3121); _LL3122:;}}}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(* _temp2714)); if( ud->name != 
0){*(( struct _tuple1*) _check_null( _temp2717))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs= Cyc_List_length( _temp2715); if( lvs != 
largs){({ struct Cyc_Std_Int_pa_struct _temp3132; _temp3132.tag= Cyc_Std_Int_pa;
_temp3132.f1=( int)(( unsigned int) largs);{ struct Cyc_Std_Int_pa_struct
_temp3131; _temp3131.tag= Cyc_Std_Int_pa; _temp3131.f1=( int)(( unsigned int)
lvs);{ struct Cyc_Std_String_pa_struct _temp3130; _temp3130.tag= Cyc_Std_String_pa;
_temp3130.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2717));{ void* _temp3129[ 3u]={& _temp3130,& _temp3131,&
_temp3132}; Cyc_Tcutil_terr( loc, _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u), _tag_arr( _temp3129, sizeof( void*), 3u));}}}});}
for( 0; _temp2715 !=  0;( _temp2715=(( struct Cyc_List_List*) _check_null(
_temp2715))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2715))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2585;}}
_LL2621: { struct Cyc_List_List* targs=* _temp2724; struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3133; _push_handler(& _temp3133);{ int _temp3135=
0; if( setjmp( _temp3133.handler)){ _temp3135= 1;} if( ! _temp3135){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2725);; _pop_handler();} else{ void* _temp3134=( void*)
_exn_thrown; void* _temp3137= _temp3134; _LL3139: if( _temp3137 ==  Cyc_Dict_Absent){
goto _LL3140;} else{ goto _LL3141;} _LL3141: goto _LL3142; _LL3140:({ struct Cyc_Std_String_pa_struct
_temp3144; _temp3144.tag= Cyc_Std_String_pa; _temp3144.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2725);{ void* _temp3143[ 1u]={& _temp3144}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3143, sizeof( void*), 1u));}}); return cvtenv; _LL3142:( void) _throw(
_temp3137); _LL3138:;}}} _temp2725[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3145=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3145->hd=( void*)({ struct
_tuple5* _temp3146=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3146->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3146->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3146;}); _temp3145->tl= inst; _temp3145;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp3148; _temp3148.tag= Cyc_Std_String_pa; _temp3148.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2725);{ void* _temp3147[ 1u]={& _temp3148}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3147, sizeof( void*), 1u));}});} if( tvs !=  0){ struct Cyc_List_List*
hidden_ts= 0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({
struct Cyc_List_List* _temp3149=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3149->hd=( void*) e; _temp3149->tl= hidden_ts;
_temp3149;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k, e);
inst=({ struct Cyc_List_List* _temp3150=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3150->hd=( void*)({ struct _tuple5*
_temp3151=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp3151->f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3151->f2= e; _temp3151;}); _temp3150->tl= inst; _temp3150;});}* _temp2724=
Cyc_List_imp_append( targs, Cyc_List_imp_rev( hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute(
inst,( void*) td->defn);* _temp2721=({ struct Cyc_Core_Opt* _temp3152=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3152->v=( void*)
new_typ; _temp3152;}); goto _LL2585;}}} _LL2623: goto _LL2585; _LL2625:
_temp2729= _temp2727; goto _LL2627; _LL2627: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2729); goto _LL2585; _LL2629:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2731); goto _LL2585; _LL2631: for( 0; _temp2733 !=  0; _temp2733=(( struct
Cyc_List_List*) _check_null( _temp2733))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2733))->hd);} goto _LL2585; _LL2585:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){({ struct Cyc_Std_String_pa_struct
_temp3156; _temp3156.tag= Cyc_Std_String_pa; _temp3156.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( expected_kind);{ struct Cyc_Std_String_pa_struct
_temp3155; _temp3155.tag= Cyc_Std_String_pa; _temp3155.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ struct Cyc_Std_String_pa_struct
_temp3154; _temp3154.tag= Cyc_Std_String_pa; _temp3154.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp3153[ 3u]={& _temp3154,& _temp3155,&
_temp3156}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3153, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3157= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3172; _temp3172.kind_env= kind_env; _temp3172.free_vars= 0; _temp3172.free_evars=
0; _temp3172.generalize_evars= generalize_evars; _temp3172.fn_result= 0;
_temp3172;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3157.free_vars;
for( 0; vs !=  0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ _temp3157.kind_env=
Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( vs))->hd);}}{ struct Cyc_List_List* evs= _temp3157.free_evars; for(
0; evs !=  0; evs=(( struct Cyc_List_List*) _check_null( evs))->tl){ void*
_temp3158= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null(
evs))->hd); struct Cyc_Core_Opt* _temp3164; struct Cyc_Core_Opt** _temp3166;
_LL3160: if(( unsigned int) _temp3158 >  4u?*(( int*) _temp3158) ==  Cyc_Absyn_Evar:
0){ _LL3165: _temp3164=(( struct Cyc_Absyn_Evar_struct*) _temp3158)->f4;
_temp3166=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3158)->f4;
goto _LL3161;} else{ goto _LL3162;} _LL3162: goto _LL3163; _LL3161: if(*
_temp3166 ==  0){* _temp3166=({ struct Cyc_Core_Opt* _temp3167=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3167->v=( void*) kind_env;
_temp3167;});} else{ struct Cyc_List_List* _temp3168=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3166))->v; struct Cyc_List_List*
_temp3169= 0; for( 0; _temp3168 !=  0; _temp3168=(( struct Cyc_List_List*)
_check_null( _temp3168))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3168))->hd)){ _temp3169=({ struct Cyc_List_List* _temp3170=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3170->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3168))->hd);
_temp3170->tl= _temp3169; _temp3170;});}}* _temp3166=({ struct Cyc_Core_Opt*
_temp3171=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3171->v=( void*) _temp3169; _temp3171;});} goto _LL3159; _LL3163: goto
_LL3159; _LL3159:;}} return _temp3157;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ int
generalize_evars= Cyc_Tcutil_is_function_type( t); struct Cyc_Tcutil_CVTEnv
_temp3173= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
generalize_evars, t); struct Cyc_List_List* _temp3174= _temp3173.free_vars;
struct Cyc_List_List* _temp3175= _temp3173.free_evars;{ struct Cyc_List_List* x=
_temp3174; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3176=( void*) c->v;
void* _temp3184; _LL3178: if( _temp3176 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3179;} else{ goto _LL3180;} _LL3180: if(( unsigned int) _temp3176 >  1u?*((
int*) _temp3176) ==  Cyc_Absyn_Eq_constr: 0){ _LL3185: _temp3184=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3176)->f1; if( _temp3184 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3181;} else{ goto _LL3182;}} else{ goto _LL3182;}
_LL3182: goto _LL3183; _LL3179: goto _LL3181; _LL3181:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3186=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3186[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3187; _temp3187.tag= Cyc_Absyn_Eq_constr;
_temp3187.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3187;}); _temp3186;})));
goto _LL3177; _LL3183: goto _LL3177; _LL3177:;}} if( _temp3174 !=  0? 1:
_temp3175 !=  0){{ void* _temp3188= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3194; struct Cyc_List_List* _temp3196; struct Cyc_List_List* _temp3198;
struct Cyc_Absyn_VarargInfo* _temp3200; int _temp3202; struct Cyc_List_List*
_temp3204; void* _temp3206; struct Cyc_Core_Opt* _temp3208; struct Cyc_List_List*
_temp3210; struct Cyc_List_List** _temp3212; _LL3190: if(( unsigned int)
_temp3188 >  4u?*(( int*) _temp3188) ==  Cyc_Absyn_FnType: 0){ _LL3195:
_temp3194=(( struct Cyc_Absyn_FnType_struct*) _temp3188)->f1; _LL3211: _temp3210=
_temp3194.tvars; _temp3212=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3188)->f1).tvars; goto _LL3209; _LL3209: _temp3208= _temp3194.effect; goto
_LL3207; _LL3207: _temp3206=( void*) _temp3194.ret_typ; goto _LL3205; _LL3205:
_temp3204= _temp3194.args; goto _LL3203; _LL3203: _temp3202= _temp3194.c_varargs;
goto _LL3201; _LL3201: _temp3200= _temp3194.cyc_varargs; goto _LL3199; _LL3199:
_temp3198= _temp3194.rgn_po; goto _LL3197; _LL3197: _temp3196= _temp3194.attributes;
goto _LL3191;} else{ goto _LL3192;} _LL3192: goto _LL3193; _LL3191: if(*
_temp3212 ==  0){* _temp3212= _temp3174; _temp3174= 0;} goto _LL3189; _LL3193:
goto _LL3189; _LL3189:;} if( _temp3174 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3214; _temp3214.tag= Cyc_Std_String_pa; _temp3214.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3174))->hd)->name;{
void* _temp3213[ 1u]={& _temp3214}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp3213, sizeof( void*), 1u));}});}
if( _temp3175 !=  0){ for( 0; _temp3175 !=  0; _temp3175=(( struct Cyc_List_List*)
_check_null( _temp3175))->tl){ void* e=( void*)(( struct Cyc_List_List*)
_check_null( _temp3175))->hd; void* _temp3215= Cyc_Tcutil_typ_kind( e); _LL3217:
if( _temp3215 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3218;} else{ goto _LL3219;}
_LL3219: if( _temp3215 == ( void*) Cyc_Absyn_EffKind){ goto _LL3220;} else{ goto
_LL3221;} _LL3221: goto _LL3222; _LL3218: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp3223[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp3223, sizeof( void*), 0u));});} goto _LL3216; _LL3220: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp3224[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp3224, sizeof( void*), 0u));});} goto _LL3216; _LL3222:({ struct Cyc_Std_String_pa_struct
_temp3227; _temp3227.tag= Cyc_Std_String_pa; _temp3227.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3226;
_temp3226.tag= Cyc_Std_String_pa; _temp3226.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp3225[ 2u]={& _temp3226,& _temp3227}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3225, sizeof( void*), 2u));}}}); goto
_LL3216; _LL3216:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3228= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3234; struct Cyc_List_List* _temp3236; struct
Cyc_List_List* _temp3238; struct Cyc_Absyn_VarargInfo* _temp3240; int _temp3242;
struct Cyc_List_List* _temp3244; void* _temp3246; struct Cyc_Core_Opt* _temp3248;
struct Cyc_List_List* _temp3250; _LL3230: if(( unsigned int) _temp3228 >  4u?*((
int*) _temp3228) ==  Cyc_Absyn_FnType: 0){ _LL3235: _temp3234=(( struct Cyc_Absyn_FnType_struct*)
_temp3228)->f1; _LL3251: _temp3250= _temp3234.tvars; goto _LL3249; _LL3249:
_temp3248= _temp3234.effect; goto _LL3247; _LL3247: _temp3246=( void*) _temp3234.ret_typ;
goto _LL3245; _LL3245: _temp3244= _temp3234.args; goto _LL3243; _LL3243:
_temp3242= _temp3234.c_varargs; goto _LL3241; _LL3241: _temp3240= _temp3234.cyc_varargs;
goto _LL3239; _LL3239: _temp3238= _temp3234.rgn_po; goto _LL3237; _LL3237:
_temp3236= _temp3234.attributes; goto _LL3231;} else{ goto _LL3232;} _LL3232:
goto _LL3233; _LL3231: fd->tvs= _temp3250; fd->effect= _temp3248; goto _LL3229;
_LL3233:({ void* _temp3252[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3252, sizeof( void*), 0u));});
return; _LL3229:;}{ struct _RegionHandle _temp3253= _new_region(); struct
_RegionHandle* r=& _temp3253; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3254=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3254->v=(
void*) t; _temp3254;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3255=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3256= Cyc_Tcutil_remove_bound_tvars( _temp3255.free_vars,
bound_tvars); struct Cyc_List_List* _temp3257= _temp3255.free_evars;{ struct Cyc_List_List*
fs= _temp3256; for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3258=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name;({ struct Cyc_Std_String_pa_struct _temp3261;
_temp3261.tag= Cyc_Std_String_pa; _temp3261.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp3260; _temp3260.tag= Cyc_Std_String_pa;
_temp3260.f1=( struct _tagged_arr)* _temp3258;{ void* _temp3259[ 2u]={&
_temp3260,& _temp3261}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3259, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3257 !=  0: 0){ for( 0; _temp3257 !=  0; _temp3257=((
struct Cyc_List_List*) _check_null( _temp3257))->tl){ void* e=( void*)(( struct
Cyc_List_List*) _check_null( _temp3257))->hd; void* _temp3262= Cyc_Tcutil_typ_kind(
e); _LL3264: if( _temp3262 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3265;} else{
goto _LL3266;} _LL3266: if( _temp3262 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3267;} else{ goto _LL3268;} _LL3268: goto _LL3269; _LL3265: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3270[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3270, sizeof( void*), 0u));});}
goto _LL3263; _LL3267: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3271=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3271[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3272; _temp3272.tag= Cyc_Absyn_JoinEff;
_temp3272.f1= 0; _temp3272;}); _temp3271;}))){({ void* _temp3273[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3273, sizeof( void*), 0u));});}
goto _LL3263; _LL3269:({ struct Cyc_Std_String_pa_struct _temp3276; _temp3276.tag=
Cyc_Std_String_pa; _temp3276.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3275; _temp3275.tag= Cyc_Std_String_pa;
_temp3275.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3274[
2u]={& _temp3275,& _temp3276}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3274, sizeof( void*), 2u));}}});
goto _LL3263; _LL3263:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ struct Cyc_List_List* vs2=(( struct Cyc_List_List*)
_check_null( vs))->tl; for( 0; vs2 !=  0; vs2=(( struct Cyc_List_List*)
_check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*) _check_null(
vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd) ==  0){({
struct Cyc_Std_String_pa_struct _temp3279; _temp3279.tag= Cyc_Std_String_pa;
_temp3279.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_Std_String_pa_struct _temp3278; _temp3278.tag=
Cyc_Std_String_pa; _temp3278.f1=( struct _tagged_arr) msg;{ void* _temp3277[ 2u]={&
_temp3278,& _temp3279}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3277, sizeof( void*), 2u));}}});}}}} static
struct _tagged_arr Cyc_Tcutil_strptr2string( struct _tagged_arr* s){ return* s;}
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){(( void(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( struct _tagged_arr*), struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)( Cyc_Std_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string,
msg);} void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs){(( void(*)( int(* cmp)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( struct Cyc_Absyn_Tvar*), struct _tagged_arr
msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp, tvs, loc, Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable", sizeof( unsigned char), 24u));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields !=  0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_Std_strcmp(*(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd)->name, _tag_arr("", sizeof(
unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List* _temp3280=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3280->hd=( void*)({
struct _tuple17* _temp3281=( struct _tuple17*) _cycalloc( sizeof( struct
_tuple17)); _temp3281->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3281->f2= 0; _temp3281;}); _temp3280->tl=
fields; _temp3280;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3284;
void* _temp3285; struct Cyc_List_List* _temp3287; struct _tuple18* _temp3282=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3284=*
_temp3282; _LL3288: _temp3287= _temp3284.f1; goto _LL3286; _LL3286: _temp3285=
_temp3284.f2; goto _LL3283; _LL3283: if( _temp3287 ==  0){ struct Cyc_List_List*
_temp3289= fields; for( 0; _temp3289 !=  0; _temp3289=(( struct Cyc_List_List*)
_check_null( _temp3289))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3289))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3289))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3290=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3290->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3291=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3291[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3292; _temp3292.tag= Cyc_Absyn_FieldName;
_temp3292.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3289))->hd)).f1)->name; _temp3292;}); _temp3291;})); _temp3290->tl= 0;
_temp3290;}); ans=({ struct Cyc_List_List* _temp3293=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3293->hd=( void*)({
struct _tuple19* _temp3294=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3294->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3289))->hd)).f1; _temp3294->f2= _temp3285; _temp3294;});
_temp3293->tl= ans; _temp3293;}); break;}} if( _temp3289 ==  0){({ void*
_temp3295[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3295, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3287))->tl !=  0){({ void*
_temp3296[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3296, sizeof( void*), 0u));});}
else{ void* _temp3297=( void*)(( struct Cyc_List_List*) _check_null( _temp3287))->hd;
struct _tagged_arr* _temp3303; _LL3299: if(*(( int*) _temp3297) ==  Cyc_Absyn_ArrayElement){
goto _LL3300;} else{ goto _LL3301;} _LL3301: if(*(( int*) _temp3297) ==  Cyc_Absyn_FieldName){
_LL3304: _temp3303=(( struct Cyc_Absyn_FieldName_struct*) _temp3297)->f1; goto
_LL3302;} else{ goto _LL3298;} _LL3300:({ void* _temp3305[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3305, sizeof( void*), 0u));}); goto _LL3298;
_LL3302: { struct Cyc_List_List* _temp3306= fields; for( 0; _temp3306 !=  0;
_temp3306=(( struct Cyc_List_List*) _check_null( _temp3306))->tl){ if( Cyc_Std_zstrptrcmp(
_temp3303,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3306))->hd)).f1)->name) ==  0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3306))->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3308;
_temp3308.tag= Cyc_Std_String_pa; _temp3308.f1=( struct _tagged_arr)* _temp3303;{
void* _temp3307[ 1u]={& _temp3308}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3307, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3306))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3309=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3309->hd=( void*)({ struct _tuple19*
_temp3310=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3310->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3306))->hd)).f1; _temp3310->f2= _temp3285; _temp3310;}); _temp3309->tl= ans;
_temp3309;}); break;}} if( _temp3306 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3312; _temp3312.tag= Cyc_Std_String_pa; _temp3312.f1=( struct _tagged_arr)*
_temp3303;{ void* _temp3311[ 1u]={& _temp3312}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3311, sizeof( void*), 1u));}});}
goto _LL3298;} _LL3298:;}}} for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3313[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3313, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3314= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3320; struct Cyc_Absyn_Conref* _temp3322; void* _temp3324;
_LL3316: if(( unsigned int) _temp3314 >  4u?*(( int*) _temp3314) ==  Cyc_Absyn_PointerType:
0){ _LL3321: _temp3320=(( struct Cyc_Absyn_PointerType_struct*) _temp3314)->f1;
_LL3325: _temp3324=( void*) _temp3320.elt_typ; goto _LL3323; _LL3323: _temp3322=
_temp3320.bounds; goto _LL3317;} else{ goto _LL3318;} _LL3318: goto _LL3319;
_LL3317: { struct Cyc_Absyn_Conref* _temp3326= Cyc_Absyn_compress_conref(
_temp3322); void* _temp3327=( void*)( Cyc_Absyn_compress_conref( _temp3326))->v;
void* _temp3335; _LL3329: if(( unsigned int) _temp3327 >  1u?*(( int*) _temp3327)
==  Cyc_Absyn_Eq_constr: 0){ _LL3336: _temp3335=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3327)->f1; if( _temp3335 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3330;}
else{ goto _LL3331;}} else{ goto _LL3331;} _LL3331: if( _temp3327 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3332;} else{ goto _LL3333;} _LL3333: goto _LL3334;
_LL3330:* elt_typ_dest= _temp3324; return 1; _LL3332:( void*)( _temp3326->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3337=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3337[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3338; _temp3338.tag= Cyc_Absyn_Eq_constr;
_temp3338.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3338;}); _temp3337;})));*
elt_typ_dest= _temp3324; return 1; _LL3334: return 0; _LL3328:;} _LL3319: return
0; _LL3315:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3521;
_temp3521.f1= 0; _temp3521.f2=( void*) Cyc_Absyn_HeapRgn; _temp3521;}); void*
_temp3339=( void*) e->r; void* _temp3353; struct _tuple1* _temp3355; struct
_tagged_arr* _temp3357; struct Cyc_Absyn_Exp* _temp3359; struct _tagged_arr*
_temp3361; struct Cyc_Absyn_Exp* _temp3363; struct Cyc_Absyn_Exp* _temp3365;
struct Cyc_Absyn_Exp* _temp3367; struct Cyc_Absyn_Exp* _temp3369; _LL3341: if(*((
int*) _temp3339) ==  Cyc_Absyn_Var_e){ _LL3356: _temp3355=(( struct Cyc_Absyn_Var_e_struct*)
_temp3339)->f1; goto _LL3354; _LL3354: _temp3353=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3339)->f2; goto _LL3342;} else{ goto _LL3343;} _LL3343: if(*(( int*)
_temp3339) ==  Cyc_Absyn_StructMember_e){ _LL3360: _temp3359=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3339)->f1; goto _LL3358; _LL3358: _temp3357=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3339)->f2; goto _LL3344;} else{ goto _LL3345;} _LL3345: if(*(( int*)
_temp3339) ==  Cyc_Absyn_StructArrow_e){ _LL3364: _temp3363=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3339)->f1; goto _LL3362; _LL3362: _temp3361=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3339)->f2; goto _LL3346;} else{ goto _LL3347;} _LL3347: if(*(( int*)
_temp3339) ==  Cyc_Absyn_Deref_e){ _LL3366: _temp3365=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3339)->f1; goto _LL3348;} else{ goto _LL3349;} _LL3349: if(*(( int*)
_temp3339) ==  Cyc_Absyn_Subscript_e){ _LL3370: _temp3369=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3339)->f1; goto _LL3368; _LL3368: _temp3367=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3339)->f2; goto _LL3350;} else{ goto _LL3351;} _LL3351: goto _LL3352;
_LL3342: { void* _temp3371= _temp3353; struct Cyc_Absyn_Vardecl* _temp3385;
struct Cyc_Absyn_Vardecl* _temp3387; struct Cyc_Absyn_Vardecl* _temp3389; struct
Cyc_Absyn_Vardecl* _temp3391; _LL3373: if( _temp3371 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3374;} else{ goto _LL3375;} _LL3375: if(( unsigned int) _temp3371 >  1u?*((
int*) _temp3371) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3376;} else{ goto _LL3377;}
_LL3377: if(( unsigned int) _temp3371 >  1u?*(( int*) _temp3371) ==  Cyc_Absyn_Global_b:
0){ _LL3386: _temp3385=(( struct Cyc_Absyn_Global_b_struct*) _temp3371)->f1;
goto _LL3378;} else{ goto _LL3379;} _LL3379: if(( unsigned int) _temp3371 >  1u?*((
int*) _temp3371) ==  Cyc_Absyn_Local_b: 0){ _LL3388: _temp3387=(( struct Cyc_Absyn_Local_b_struct*)
_temp3371)->f1; goto _LL3380;} else{ goto _LL3381;} _LL3381: if(( unsigned int)
_temp3371 >  1u?*(( int*) _temp3371) ==  Cyc_Absyn_Pat_b: 0){ _LL3390: _temp3389=((
struct Cyc_Absyn_Pat_b_struct*) _temp3371)->f1; goto _LL3382;} else{ goto
_LL3383;} _LL3383: if(( unsigned int) _temp3371 >  1u?*(( int*) _temp3371) == 
Cyc_Absyn_Param_b: 0){ _LL3392: _temp3391=(( struct Cyc_Absyn_Param_b_struct*)
_temp3371)->f1; goto _LL3384;} else{ goto _LL3372;} _LL3374: return bogus_ans;
_LL3376: return({ struct _tuple7 _temp3393; _temp3393.f1= 0; _temp3393.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3393;}); _LL3378: { void* _temp3394= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3396: if((
unsigned int) _temp3394 >  4u?*(( int*) _temp3394) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3397;} else{ goto _LL3398;} _LL3398: goto _LL3399; _LL3397: return({
struct _tuple7 _temp3400; _temp3400.f1= 1; _temp3400.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3400;}); _LL3399: return({ struct _tuple7 _temp3401; _temp3401.f1=(
_temp3385->tq).q_const; _temp3401.f2=( void*) Cyc_Absyn_HeapRgn; _temp3401;});
_LL3395:;} _LL3380: { void* _temp3402= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3404: if(( unsigned int) _temp3402 >  4u?*(( int*)
_temp3402) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3405;} else{ goto _LL3406;}
_LL3406: goto _LL3407; _LL3405: return({ struct _tuple7 _temp3408; _temp3408.f1=
1; _temp3408.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3387->rgn))->v;
_temp3408;}); _LL3407: return({ struct _tuple7 _temp3409; _temp3409.f1=(
_temp3387->tq).q_const; _temp3409.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3387->rgn))->v; _temp3409;}); _LL3403:;} _LL3382: _temp3391=
_temp3389; goto _LL3384; _LL3384: return({ struct _tuple7 _temp3410; _temp3410.f1=(
_temp3391->tq).q_const; _temp3410.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3391->rgn))->v; _temp3410;}); _LL3372:;} _LL3344: { void*
_temp3411= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3359->topt))->v); struct Cyc_List_List* _temp3423; struct Cyc_List_List*
_temp3425; struct Cyc_Absyn_Structdecl** _temp3427; struct Cyc_Absyn_Structdecl*
_temp3429; struct Cyc_Absyn_Uniondecl** _temp3430; struct Cyc_Absyn_Uniondecl*
_temp3432; _LL3413: if(( unsigned int) _temp3411 >  4u?*(( int*) _temp3411) == 
Cyc_Absyn_AnonStructType: 0){ _LL3424: _temp3423=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3411)->f1; goto _LL3414;} else{ goto _LL3415;} _LL3415: if(( unsigned int)
_temp3411 >  4u?*(( int*) _temp3411) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3426:
_temp3425=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3411)->f1; goto
_LL3416;} else{ goto _LL3417;} _LL3417: if(( unsigned int) _temp3411 >  4u?*((
int*) _temp3411) ==  Cyc_Absyn_StructType: 0){ _LL3428: _temp3427=(( struct Cyc_Absyn_StructType_struct*)
_temp3411)->f3; if( _temp3427 ==  0){ goto _LL3419;} else{ _temp3429=* _temp3427;
goto _LL3418;}} else{ goto _LL3419;} _LL3419: if(( unsigned int) _temp3411 >  4u?*((
int*) _temp3411) ==  Cyc_Absyn_UnionType: 0){ _LL3431: _temp3430=(( struct Cyc_Absyn_UnionType_struct*)
_temp3411)->f3; if( _temp3430 ==  0){ goto _LL3421;} else{ _temp3432=* _temp3430;
goto _LL3420;}} else{ goto _LL3421;} _LL3421: goto _LL3422; _LL3414: _temp3425=
_temp3423; goto _LL3416; _LL3416: { struct Cyc_Absyn_Structfield* _temp3433= Cyc_Absyn_lookup_field(
_temp3425, _temp3357); if( _temp3433 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3433))->width !=  0: 0){ return({ struct _tuple7 _temp3434;
_temp3434.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3433))->tq).q_const;
_temp3434.f2=( Cyc_Tcutil_addressof_props( te, _temp3359)).f2; _temp3434;});}
return bogus_ans;} _LL3418: { struct Cyc_Absyn_Structfield* _temp3435= Cyc_Absyn_lookup_struct_field(
_temp3429, _temp3357); if( _temp3435 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3435))->width !=  0: 0){ return({ struct _tuple7 _temp3436;
_temp3436.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3435))->tq).q_const;
_temp3436.f2=( Cyc_Tcutil_addressof_props( te, _temp3359)).f2; _temp3436;});}
return bogus_ans;} _LL3420: { struct Cyc_Absyn_Structfield* _temp3437= Cyc_Absyn_lookup_union_field(
_temp3432, _temp3357); if( _temp3437 !=  0){ return({ struct _tuple7 _temp3438;
_temp3438.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3437))->tq).q_const;
_temp3438.f2=( Cyc_Tcutil_addressof_props( te, _temp3359)).f2; _temp3438;});}
goto _LL3422;} _LL3422: return bogus_ans; _LL3412:;} _LL3346: { void* _temp3439=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3363->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3445; void* _temp3447; void* _temp3449; _LL3441:
if(( unsigned int) _temp3439 >  4u?*(( int*) _temp3439) ==  Cyc_Absyn_PointerType:
0){ _LL3446: _temp3445=(( struct Cyc_Absyn_PointerType_struct*) _temp3439)->f1;
_LL3450: _temp3449=( void*) _temp3445.elt_typ; goto _LL3448; _LL3448: _temp3447=(
void*) _temp3445.rgn_typ; goto _LL3442;} else{ goto _LL3443;} _LL3443: goto
_LL3444; _LL3442: { void* _temp3451= Cyc_Tcutil_compress( _temp3449); struct Cyc_List_List*
_temp3463; struct Cyc_List_List* _temp3465; struct Cyc_Absyn_Structdecl**
_temp3467; struct Cyc_Absyn_Structdecl* _temp3469; struct Cyc_Absyn_Uniondecl**
_temp3470; struct Cyc_Absyn_Uniondecl* _temp3472; _LL3453: if(( unsigned int)
_temp3451 >  4u?*(( int*) _temp3451) ==  Cyc_Absyn_AnonStructType: 0){ _LL3464:
_temp3463=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3451)->f1; goto
_LL3454;} else{ goto _LL3455;} _LL3455: if(( unsigned int) _temp3451 >  4u?*((
int*) _temp3451) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3466: _temp3465=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3451)->f1; goto _LL3456;} else{ goto
_LL3457;} _LL3457: if(( unsigned int) _temp3451 >  4u?*(( int*) _temp3451) == 
Cyc_Absyn_StructType: 0){ _LL3468: _temp3467=(( struct Cyc_Absyn_StructType_struct*)
_temp3451)->f3; if( _temp3467 ==  0){ goto _LL3459;} else{ _temp3469=* _temp3467;
goto _LL3458;}} else{ goto _LL3459;} _LL3459: if(( unsigned int) _temp3451 >  4u?*((
int*) _temp3451) ==  Cyc_Absyn_UnionType: 0){ _LL3471: _temp3470=(( struct Cyc_Absyn_UnionType_struct*)
_temp3451)->f3; if( _temp3470 ==  0){ goto _LL3461;} else{ _temp3472=* _temp3470;
goto _LL3460;}} else{ goto _LL3461;} _LL3461: goto _LL3462; _LL3454: _temp3465=
_temp3463; goto _LL3456; _LL3456: { struct Cyc_Absyn_Structfield* _temp3473= Cyc_Absyn_lookup_field(
_temp3465, _temp3361); if( _temp3473 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3473))->width !=  0: 0){ return({ struct _tuple7 _temp3474;
_temp3474.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3473))->tq).q_const;
_temp3474.f2= _temp3447; _temp3474;});} return bogus_ans;} _LL3458: { struct Cyc_Absyn_Structfield*
_temp3475= Cyc_Absyn_lookup_struct_field( _temp3469, _temp3361); if( _temp3475
!=  0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3475))->width !=  0: 0){
return({ struct _tuple7 _temp3476; _temp3476.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3475))->tq).q_const; _temp3476.f2= _temp3447; _temp3476;});}
return bogus_ans;} _LL3460: { struct Cyc_Absyn_Structfield* _temp3477= Cyc_Absyn_lookup_union_field(
_temp3472, _temp3361); if( _temp3477 !=  0){ return({ struct _tuple7 _temp3478;
_temp3478.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3477))->tq).q_const;
_temp3478.f2= _temp3447; _temp3478;});} return bogus_ans;} _LL3462: return
bogus_ans; _LL3452:;} _LL3444: return bogus_ans; _LL3440:;} _LL3348: { void*
_temp3479= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3365->topt))->v); struct Cyc_Absyn_PtrInfo _temp3485; struct Cyc_Absyn_Tqual
_temp3487; void* _temp3489; _LL3481: if(( unsigned int) _temp3479 >  4u?*(( int*)
_temp3479) ==  Cyc_Absyn_PointerType: 0){ _LL3486: _temp3485=(( struct Cyc_Absyn_PointerType_struct*)
_temp3479)->f1; _LL3490: _temp3489=( void*) _temp3485.rgn_typ; goto _LL3488;
_LL3488: _temp3487= _temp3485.tq; goto _LL3482;} else{ goto _LL3483;} _LL3483:
goto _LL3484; _LL3482: return({ struct _tuple7 _temp3491; _temp3491.f1=
_temp3487.q_const; _temp3491.f2= _temp3489; _temp3491;}); _LL3484: return
bogus_ans; _LL3480:;} _LL3350: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3369->topt))->v); void* _temp3492= t; struct
Cyc_Absyn_Tqual _temp3502; struct Cyc_List_List* _temp3504; struct Cyc_Absyn_PtrInfo
_temp3506; struct Cyc_Absyn_Conref* _temp3508; struct Cyc_Absyn_Tqual _temp3510;
void* _temp3512; void* _temp3514; _LL3494: if(( unsigned int) _temp3492 >  4u?*((
int*) _temp3492) ==  Cyc_Absyn_ArrayType: 0){ _LL3503: _temp3502=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3492)->f2; goto _LL3495;} else{ goto _LL3496;} _LL3496: if(( unsigned int)
_temp3492 >  4u?*(( int*) _temp3492) ==  Cyc_Absyn_TupleType: 0){ _LL3505:
_temp3504=(( struct Cyc_Absyn_TupleType_struct*) _temp3492)->f1; goto _LL3497;}
else{ goto _LL3498;} _LL3498: if(( unsigned int) _temp3492 >  4u?*(( int*)
_temp3492) ==  Cyc_Absyn_PointerType: 0){ _LL3507: _temp3506=(( struct Cyc_Absyn_PointerType_struct*)
_temp3492)->f1; _LL3515: _temp3514=( void*) _temp3506.elt_typ; goto _LL3513;
_LL3513: _temp3512=( void*) _temp3506.rgn_typ; goto _LL3511; _LL3511: _temp3510=
_temp3506.tq; goto _LL3509; _LL3509: _temp3508= _temp3506.bounds; goto _LL3499;}
else{ goto _LL3500;} _LL3500: goto _LL3501; _LL3495: return({ struct _tuple7
_temp3516; _temp3516.f1= _temp3502.q_const; _temp3516.f2=( Cyc_Tcutil_addressof_props(
te, _temp3369)).f2; _temp3516;}); _LL3497: { struct _tuple4* _temp3517= Cyc_Absyn_lookup_tuple_field(
_temp3504,( int) Cyc_Evexp_eval_const_uint_exp( _temp3367)); if( _temp3517 !=  0){
return({ struct _tuple7 _temp3518; _temp3518.f1=((*(( struct _tuple4*)
_check_null( _temp3517))).f1).q_const; _temp3518.f2=( Cyc_Tcutil_addressof_props(
te, _temp3369)).f2; _temp3518;});} return bogus_ans;} _LL3499: return({ struct
_tuple7 _temp3519; _temp3519.f1= _temp3510.q_const; _temp3519.f2= _temp3512;
_temp3519;}); _LL3501: return bogus_ans; _LL3493:;} _LL3352:({ void* _temp3520[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3520, sizeof( void*), 0u));});
return bogus_ans; _LL3340:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3522= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3528; void* _temp3530; _LL3524: if((
unsigned int) _temp3522 >  4u?*(( int*) _temp3522) ==  Cyc_Absyn_ArrayType: 0){
_LL3531: _temp3530=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3522)->f1;
goto _LL3529; _LL3529: _temp3528=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3522)->f2; goto _LL3525;} else{ goto _LL3526;} _LL3526: goto _LL3527;
_LL3525: { void* _temp3534; struct _tuple7 _temp3532= Cyc_Tcutil_addressof_props(
te, e); _LL3535: _temp3534= _temp3532.f2; goto _LL3533; _LL3533: return Cyc_Absyn_atb_typ(
_temp3530, _temp3534, _temp3528,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3536=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3536[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3537; _temp3537.tag= Cyc_Absyn_Upper_b;
_temp3537.f1= e; _temp3537;}); _temp3536;}));} _LL3527: return e_typ; _LL3523:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3538=(
void*) b->v; void* _temp3546; void* _temp3548; struct Cyc_Absyn_Exp* _temp3550;
_LL3540: if(( unsigned int) _temp3538 >  1u?*(( int*) _temp3538) ==  Cyc_Absyn_Eq_constr:
0){ _LL3547: _temp3546=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3538)->f1;
if( _temp3546 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3541;} else{ goto
_LL3542;}} else{ goto _LL3542;} _LL3542: if(( unsigned int) _temp3538 >  1u?*((
int*) _temp3538) ==  Cyc_Absyn_Eq_constr: 0){ _LL3549: _temp3548=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3538)->f1; if(( unsigned int) _temp3548
>  1u?*(( int*) _temp3548) ==  Cyc_Absyn_Upper_b: 0){ _LL3551: _temp3550=((
struct Cyc_Absyn_Upper_b_struct*) _temp3548)->f1; goto _LL3543;} else{ goto
_LL3544;}} else{ goto _LL3544;} _LL3544: goto _LL3545; _LL3541: return; _LL3543:
if( Cyc_Evexp_eval_const_uint_exp( _temp3550) <=  i){({ void* _temp3552[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3552, sizeof( void*), 0u));});} return;
_LL3545:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3553=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3553[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3554; _temp3554.tag= Cyc_Absyn_Eq_constr;
_temp3554.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3555=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3555[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3556; _temp3556.tag= Cyc_Absyn_Upper_b;
_temp3556.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3556;}); _temp3555;}));
_temp3554;}); _temp3553;}))); return; _LL3539:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp3557=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp3563; struct
Cyc_Absyn_Exp* _temp3565; _LL3559: if(( unsigned int) _temp3557 >  1u?*(( int*)
_temp3557) ==  Cyc_Absyn_Eq_constr: 0){ _LL3564: _temp3563=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3557)->f1; if(( unsigned int) _temp3563 >  1u?*(( int*) _temp3563) ==  Cyc_Absyn_Upper_b:
0){ _LL3566: _temp3565=(( struct Cyc_Absyn_Upper_b_struct*) _temp3563)->f1; goto
_LL3560;} else{ goto _LL3561;}} else{ goto _LL3561;} _LL3561: goto _LL3562;
_LL3560: return Cyc_Evexp_eval_const_uint_exp( _temp3565) ==  1; _LL3562: return
0; _LL3558:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp3567= Cyc_Tcutil_compress(
t); void* _temp3593; struct Cyc_List_List* _temp3595; struct Cyc_Absyn_Structdecl**
_temp3597; struct Cyc_List_List* _temp3599; struct Cyc_Absyn_Uniondecl**
_temp3601; struct Cyc_List_List* _temp3603; struct Cyc_List_List* _temp3605;
struct Cyc_List_List* _temp3607; _LL3569: if( _temp3567 == ( void*) Cyc_Absyn_VoidType){
goto _LL3570;} else{ goto _LL3571;} _LL3571: if(( unsigned int) _temp3567 >  4u?*((
int*) _temp3567) ==  Cyc_Absyn_IntType: 0){ goto _LL3572;} else{ goto _LL3573;}
_LL3573: if( _temp3567 == ( void*) Cyc_Absyn_FloatType){ goto _LL3574;} else{
goto _LL3575;} _LL3575: if( _temp3567 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3576;} else{ goto _LL3577;} _LL3577: if(( unsigned int) _temp3567 >  4u?*((
int*) _temp3567) ==  Cyc_Absyn_EnumType: 0){ goto _LL3578;} else{ goto _LL3579;}
_LL3579: if(( unsigned int) _temp3567 >  4u?*(( int*) _temp3567) ==  Cyc_Absyn_ArrayType:
0){ _LL3594: _temp3593=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3567)->f1;
goto _LL3580;} else{ goto _LL3581;} _LL3581: if(( unsigned int) _temp3567 >  4u?*((
int*) _temp3567) ==  Cyc_Absyn_TupleType: 0){ _LL3596: _temp3595=(( struct Cyc_Absyn_TupleType_struct*)
_temp3567)->f1; goto _LL3582;} else{ goto _LL3583;} _LL3583: if(( unsigned int)
_temp3567 >  4u?*(( int*) _temp3567) ==  Cyc_Absyn_StructType: 0){ _LL3600:
_temp3599=(( struct Cyc_Absyn_StructType_struct*) _temp3567)->f2; goto _LL3598;
_LL3598: _temp3597=(( struct Cyc_Absyn_StructType_struct*) _temp3567)->f3; goto
_LL3584;} else{ goto _LL3585;} _LL3585: if(( unsigned int) _temp3567 >  4u?*((
int*) _temp3567) ==  Cyc_Absyn_UnionType: 0){ _LL3604: _temp3603=(( struct Cyc_Absyn_UnionType_struct*)
_temp3567)->f2; goto _LL3602; _LL3602: _temp3601=(( struct Cyc_Absyn_UnionType_struct*)
_temp3567)->f3; goto _LL3586;} else{ goto _LL3587;} _LL3587: if(( unsigned int)
_temp3567 >  4u?*(( int*) _temp3567) ==  Cyc_Absyn_AnonStructType: 0){ _LL3606:
_temp3605=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3567)->f1; goto
_LL3588;} else{ goto _LL3589;} _LL3589: if(( unsigned int) _temp3567 >  4u?*((
int*) _temp3567) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3608: _temp3607=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3567)->f1; goto _LL3590;} else{ goto
_LL3591;} _LL3591: goto _LL3592; _LL3570: goto _LL3572; _LL3572: goto _LL3574;
_LL3574: goto _LL3576; _LL3576: return 1; _LL3578: return 0; _LL3580: return Cyc_Tcutil_bits_only(
_temp3593); _LL3582: for( 0; _temp3595 !=  0; _temp3595=(( struct Cyc_List_List*)
_check_null( _temp3595))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3595))->hd)).f2)){ return 0;}} return 1;
_LL3584: if( _temp3597 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp3609=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp3597)); if(
_temp3609->fields ==  0){ return 0;}{ struct _RegionHandle _temp3610=
_new_region(); struct _RegionHandle* rgn=& _temp3610; _push_region( rgn);{
struct Cyc_List_List* _temp3611=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3609->tvs, _temp3599);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp3609->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp3611,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ int _temp3612= 0;
_npop_handler( 0u); return _temp3612;}}}{ int _temp3613= 1; _npop_handler( 0u);
return _temp3613;}}; _pop_region( rgn);}} _LL3586: if( _temp3601 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3614=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3601)); if( _temp3614->fields ==  0){ return 0;}{ struct
_RegionHandle _temp3615= _new_region(); struct _RegionHandle* rgn=& _temp3615;
_push_region( rgn);{ struct Cyc_List_List* _temp3616=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3614->tvs, _temp3603);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3614->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3616,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3617= 0; _npop_handler( 0u); return
_temp3617;}}}{ int _temp3618= 1; _npop_handler( 0u); return _temp3618;}};
_pop_region( rgn);}} _LL3588: _temp3607= _temp3605; goto _LL3590; _LL3590: for(
0; _temp3607 !=  0; _temp3607=(( struct Cyc_List_List*) _check_null( _temp3607))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3607))->hd)->type)){ return 0;}} return 1;
_LL3592: return 0; _LL3568:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3619=( void*) e->r; struct
_tuple1* _temp3663; struct Cyc_Absyn_Exp* _temp3665; struct Cyc_Absyn_Exp*
_temp3667; struct Cyc_Absyn_Exp* _temp3669; struct Cyc_Absyn_Exp* _temp3671;
struct Cyc_Absyn_Exp* _temp3673; struct Cyc_Absyn_Exp* _temp3675; struct Cyc_Absyn_Exp*
_temp3677; struct Cyc_Absyn_Exp* _temp3679; void* _temp3681; struct Cyc_Absyn_Exp*
_temp3683; struct Cyc_Absyn_Exp* _temp3685; struct Cyc_Absyn_Exp* _temp3687;
struct Cyc_List_List* _temp3689; struct Cyc_List_List* _temp3691; struct Cyc_List_List*
_temp3693; struct Cyc_List_List* _temp3695; void* _temp3697; struct Cyc_List_List*
_temp3699; struct Cyc_List_List* _temp3701; _LL3621: if(*(( int*) _temp3619) == 
Cyc_Absyn_Const_e){ goto _LL3622;} else{ goto _LL3623;} _LL3623: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3624;} else{ goto _LL3625;}
_LL3625: if(*(( int*) _temp3619) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3626;}
else{ goto _LL3627;} _LL3627: if(*(( int*) _temp3619) ==  Cyc_Absyn_Offsetof_e){
goto _LL3628;} else{ goto _LL3629;} _LL3629: if(*(( int*) _temp3619) ==  Cyc_Absyn_Gentyp_e){
goto _LL3630;} else{ goto _LL3631;} _LL3631: if(*(( int*) _temp3619) ==  Cyc_Absyn_Enum_e){
goto _LL3632;} else{ goto _LL3633;} _LL3633: if(*(( int*) _temp3619) ==  Cyc_Absyn_Var_e){
_LL3664: _temp3663=(( struct Cyc_Absyn_Var_e_struct*) _temp3619)->f1; goto
_LL3634;} else{ goto _LL3635;} _LL3635: if(*(( int*) _temp3619) ==  Cyc_Absyn_Conditional_e){
_LL3670: _temp3669=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3619)->f1;
goto _LL3668; _LL3668: _temp3667=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3619)->f2; goto _LL3666; _LL3666: _temp3665=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3619)->f3; goto _LL3636;} else{ goto _LL3637;} _LL3637: if(*(( int*)
_temp3619) ==  Cyc_Absyn_SeqExp_e){ _LL3674: _temp3673=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3619)->f1; goto _LL3672; _LL3672: _temp3671=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3619)->f2; goto _LL3638;} else{ goto _LL3639;} _LL3639: if(*(( int*)
_temp3619) ==  Cyc_Absyn_NoInstantiate_e){ _LL3676: _temp3675=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3619)->f1; goto _LL3640;} else{ goto _LL3641;} _LL3641: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Instantiate_e){ _LL3678: _temp3677=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3619)->f1; goto _LL3642;} else{ goto _LL3643;} _LL3643: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Cast_e){ _LL3682: _temp3681=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3619)->f1; goto _LL3680; _LL3680: _temp3679=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3619)->f2; goto _LL3644;} else{ goto _LL3645;} _LL3645: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Address_e){ _LL3684: _temp3683=(( struct Cyc_Absyn_Address_e_struct*)
_temp3619)->f1; goto _LL3646;} else{ goto _LL3647;} _LL3647: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Comprehension_e){ _LL3688: _temp3687=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3619)->f2; goto _LL3686; _LL3686: _temp3685=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3619)->f3; goto _LL3648;} else{ goto _LL3649;} _LL3649: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Array_e){ _LL3690: _temp3689=(( struct Cyc_Absyn_Array_e_struct*)
_temp3619)->f1; goto _LL3650;} else{ goto _LL3651;} _LL3651: if(*(( int*)
_temp3619) ==  Cyc_Absyn_AnonStruct_e){ _LL3692: _temp3691=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3619)->f2; goto _LL3652;} else{ goto _LL3653;} _LL3653: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Struct_e){ _LL3694: _temp3693=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3619)->f3; goto _LL3654;} else{ goto _LL3655;} _LL3655: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Primop_e){ _LL3698: _temp3697=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3619)->f1; goto _LL3696; _LL3696: _temp3695=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3619)->f2; goto _LL3656;} else{ goto _LL3657;} _LL3657: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Tuple_e){ _LL3700: _temp3699=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3619)->f1; goto _LL3658;} else{ goto _LL3659;} _LL3659: if(*(( int*)
_temp3619) ==  Cyc_Absyn_Tunion_e){ _LL3702: _temp3701=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3619)->f3; goto _LL3660;} else{ goto _LL3661;} _LL3661: goto _LL3662;
_LL3622: return 1; _LL3624: return 1; _LL3626: return 1; _LL3628: return 1;
_LL3630: return 1; _LL3632: return 1; _LL3634: { struct _handler_cons _temp3703;
_push_handler(& _temp3703);{ int _temp3705= 0; if( setjmp( _temp3703.handler)){
_temp3705= 1;} if( ! _temp3705){{ void* _temp3706= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3663); void* _temp3716; void* _temp3718; struct Cyc_Absyn_Vardecl*
_temp3720; void* _temp3722; _LL3708: if(*(( int*) _temp3706) ==  Cyc_Tcenv_VarRes){
_LL3717: _temp3716=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3706)->f1;
if(( unsigned int) _temp3716 >  1u?*(( int*) _temp3716) ==  Cyc_Absyn_Funname_b:
0){ goto _LL3709;} else{ goto _LL3710;}} else{ goto _LL3710;} _LL3710: if(*((
int*) _temp3706) ==  Cyc_Tcenv_VarRes){ _LL3719: _temp3718=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3706)->f1; if(( unsigned int) _temp3718 >  1u?*(( int*) _temp3718) ==  Cyc_Absyn_Global_b:
0){ _LL3721: _temp3720=(( struct Cyc_Absyn_Global_b_struct*) _temp3718)->f1;
goto _LL3711;} else{ goto _LL3712;}} else{ goto _LL3712;} _LL3712: if(*(( int*)
_temp3706) ==  Cyc_Tcenv_VarRes){ _LL3723: _temp3722=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3706)->f1; if( _temp3722 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL3713;}
else{ goto _LL3714;}} else{ goto _LL3714;} _LL3714: goto _LL3715; _LL3709: { int
_temp3724= 1; _npop_handler( 0u); return _temp3724;} _LL3711: { void* _temp3725=
Cyc_Tcutil_compress(( void*) _temp3720->type); _LL3727: if(( unsigned int)
_temp3725 >  4u?*(( int*) _temp3725) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3728;}
else{ goto _LL3729;} _LL3729: goto _LL3730; _LL3728: { int _temp3731= 1;
_npop_handler( 0u); return _temp3731;} _LL3730: { int _temp3732= var_okay;
_npop_handler( 0u); return _temp3732;} _LL3726:;} _LL3713: { int _temp3733= 0;
_npop_handler( 0u); return _temp3733;} _LL3715: { int _temp3734= var_okay;
_npop_handler( 0u); return _temp3734;} _LL3707:;}; _pop_handler();} else{ void*
_temp3704=( void*) _exn_thrown; void* _temp3736= _temp3704; _LL3738: if(
_temp3736 ==  Cyc_Dict_Absent){ goto _LL3739;} else{ goto _LL3740;} _LL3740:
goto _LL3741; _LL3739: return 0; _LL3741:( void) _throw( _temp3736); _LL3737:;}}}
_LL3636: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3669)? Cyc_Tcutil_cnst_exp( te,
0, _temp3667): 0)? Cyc_Tcutil_cnst_exp( te, 0, _temp3665): 0; _LL3638: return
Cyc_Tcutil_cnst_exp( te, 0, _temp3673)? Cyc_Tcutil_cnst_exp( te, 0, _temp3671):
0; _LL3640: _temp3677= _temp3675; goto _LL3642; _LL3642: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3677); _LL3644: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3679); _LL3646: return Cyc_Tcutil_cnst_exp( te, 1, _temp3683); _LL3648:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3687)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3685): 0; _LL3650: _temp3691= _temp3689; goto _LL3652; _LL3652: _temp3693=
_temp3691; goto _LL3654; _LL3654: for( 0; _temp3693 !=  0; _temp3693=(( struct
Cyc_List_List*) _check_null( _temp3693))->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*((
struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3693))->hd)).f2)){
return 0;}} return 1; _LL3656: _temp3699= _temp3695; goto _LL3658; _LL3658:
_temp3701= _temp3699; goto _LL3660; _LL3660: for( 0; _temp3701 !=  0; _temp3701=((
struct Cyc_List_List*) _check_null( _temp3701))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3701))->hd)){
return 0;}} return 1; _LL3662: return 0; _LL3620:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3742= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3770; struct Cyc_Absyn_Conref* _temp3772; struct Cyc_Absyn_Conref*
_temp3774; void* _temp3776; struct Cyc_List_List* _temp3778; struct Cyc_Absyn_Structdecl**
_temp3780; struct Cyc_List_List* _temp3782; struct Cyc_Absyn_Uniondecl**
_temp3784; struct Cyc_List_List* _temp3786; struct Cyc_List_List* _temp3788;
struct Cyc_List_List* _temp3790; _LL3744: if( _temp3742 == ( void*) Cyc_Absyn_VoidType){
goto _LL3745;} else{ goto _LL3746;} _LL3746: if(( unsigned int) _temp3742 >  4u?*((
int*) _temp3742) ==  Cyc_Absyn_IntType: 0){ goto _LL3747;} else{ goto _LL3748;}
_LL3748: if( _temp3742 == ( void*) Cyc_Absyn_FloatType){ goto _LL3749;} else{
goto _LL3750;} _LL3750: if( _temp3742 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3751;} else{ goto _LL3752;} _LL3752: if(( unsigned int) _temp3742 >  4u?*((
int*) _temp3742) ==  Cyc_Absyn_PointerType: 0){ _LL3771: _temp3770=(( struct Cyc_Absyn_PointerType_struct*)
_temp3742)->f1; _LL3775: _temp3774= _temp3770.nullable; goto _LL3773; _LL3773:
_temp3772= _temp3770.bounds; goto _LL3753;} else{ goto _LL3754;} _LL3754: if((
unsigned int) _temp3742 >  4u?*(( int*) _temp3742) ==  Cyc_Absyn_ArrayType: 0){
_LL3777: _temp3776=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3742)->f1;
goto _LL3755;} else{ goto _LL3756;} _LL3756: if(( unsigned int) _temp3742 >  4u?*((
int*) _temp3742) ==  Cyc_Absyn_TupleType: 0){ _LL3779: _temp3778=(( struct Cyc_Absyn_TupleType_struct*)
_temp3742)->f1; goto _LL3757;} else{ goto _LL3758;} _LL3758: if(( unsigned int)
_temp3742 >  4u?*(( int*) _temp3742) ==  Cyc_Absyn_StructType: 0){ _LL3783:
_temp3782=(( struct Cyc_Absyn_StructType_struct*) _temp3742)->f2; goto _LL3781;
_LL3781: _temp3780=(( struct Cyc_Absyn_StructType_struct*) _temp3742)->f3; goto
_LL3759;} else{ goto _LL3760;} _LL3760: if(( unsigned int) _temp3742 >  4u?*((
int*) _temp3742) ==  Cyc_Absyn_UnionType: 0){ _LL3787: _temp3786=(( struct Cyc_Absyn_UnionType_struct*)
_temp3742)->f2; goto _LL3785; _LL3785: _temp3784=(( struct Cyc_Absyn_UnionType_struct*)
_temp3742)->f3; goto _LL3761;} else{ goto _LL3762;} _LL3762: if(( unsigned int)
_temp3742 >  4u?*(( int*) _temp3742) ==  Cyc_Absyn_AnonStructType: 0){ _LL3789:
_temp3788=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3742)->f1; goto
_LL3763;} else{ goto _LL3764;} _LL3764: if(( unsigned int) _temp3742 >  4u?*((
int*) _temp3742) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3791: _temp3790=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3742)->f1; goto _LL3765;} else{ goto
_LL3766;} _LL3766: if(( unsigned int) _temp3742 >  4u?*(( int*) _temp3742) == 
Cyc_Absyn_EnumType: 0){ goto _LL3767;} else{ goto _LL3768;} _LL3768: goto
_LL3769; _LL3745: goto _LL3747; _LL3747: goto _LL3749; _LL3749: goto _LL3751;
_LL3751: return 1; _LL3753: { void* _temp3792=( void*)( Cyc_Absyn_compress_conref(
_temp3772))->v; void* _temp3800; void* _temp3802; _LL3794: if(( unsigned int)
_temp3792 >  1u?*(( int*) _temp3792) ==  Cyc_Absyn_Eq_constr: 0){ _LL3801:
_temp3800=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3792)->f1; if(
_temp3800 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3795;} else{ goto _LL3796;}}
else{ goto _LL3796;} _LL3796: if(( unsigned int) _temp3792 >  1u?*(( int*)
_temp3792) ==  Cyc_Absyn_Eq_constr: 0){ _LL3803: _temp3802=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3792)->f1; if(( unsigned int) _temp3802 >  1u?*(( int*) _temp3802) ==  Cyc_Absyn_Upper_b:
0){ goto _LL3797;} else{ goto _LL3798;}} else{ goto _LL3798;} _LL3798: goto
_LL3799; _LL3795: return 1; _LL3797: { void* _temp3804=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3774))->v; int
_temp3810; _LL3806: if(( unsigned int) _temp3804 >  1u?*(( int*) _temp3804) == 
Cyc_Absyn_Eq_constr: 0){ _LL3811: _temp3810=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3804)->f1; goto _LL3807;} else{ goto _LL3808;} _LL3808: goto _LL3809;
_LL3807: return _temp3810; _LL3809: return 0; _LL3805:;} _LL3799: return 0;
_LL3793:;} _LL3755: return Cyc_Tcutil_supports_default( _temp3776); _LL3757:
for( 0; _temp3778 !=  0; _temp3778=(( struct Cyc_List_List*) _check_null(
_temp3778))->tl){ if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3778))->hd)).f2)){ return 0;}} return 1;
_LL3759: if( _temp3780 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3780)); if( sd->fields ==  0){
return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs, _temp3782,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);} _LL3761:
if( _temp3784 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3784)); if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp3786,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL3763: _temp3790= _temp3788; goto _LL3765; _LL3765: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp3790); _LL3767: return 1; _LL3769:
return 0; _LL3743:;} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{ struct _RegionHandle
_temp3812= _new_region(); struct _RegionHandle* rgn=& _temp3812; _push_region(
rgn);{ struct Cyc_List_List* _temp3813=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3813,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3814= 0; _npop_handler( 0u);
return _temp3814;}}}; _pop_region( rgn);} return 1;}
