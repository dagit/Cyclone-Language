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
Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);({ struct Cyc_Std_String_pa_struct
_temp2; _temp2.tag= Cyc_Std_String_pa; _temp2.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure);{ struct Cyc_Std_String_pa_struct _temp1; _temp1.tag= Cyc_Std_String_pa;
_temp1.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( Cyc_Tcutil_t1_failure);{
void* _temp0[ 2u]={& _temp1,& _temp2}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\t%s and \n\t%s\n\t failed to unify.\n",
sizeof( unsigned char), 33u), _tag_arr( _temp0, sizeof( void*), 2u));}}}); if(
Cyc_Tcutil_failure_reason.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){({
struct Cyc_Std_String_pa_struct _temp4; _temp4.tag= Cyc_Std_String_pa; _temp4.f1=(
struct _tagged_arr) Cyc_Tcutil_failure_reason;{ void* _temp3[ 1u]={& _temp4};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\t%s\n", sizeof( unsigned char), 5u),
_tag_arr( _temp3, sizeof( void*), 1u));}});} Cyc_Std_fflush(( struct Cyc_Std___sFILE*)
Cyc_Std_stderr);} void Cyc_Tcutil_terr( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc,( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap)));}
void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap){ struct
_tagged_arr msg=( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap);({ struct Cyc_Std_String_pa_struct _temp6; _temp6.tag= Cyc_Std_String_pa;
_temp6.f1=( struct _tagged_arr) msg;{ void* _temp5[ 1u]={& _temp6}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp5, sizeof( void*), 1u));}}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);(
int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){({ struct Cyc_Std_String_pa_struct _temp9; _temp9.tag=
Cyc_Std_String_pa; _temp9.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);{
struct Cyc_Std_String_pa_struct _temp8; _temp8.tag= Cyc_Std_String_pa; _temp8.f1=(
struct _tagged_arr) Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd);{ void* _temp7[ 2u]={& _temp8,& _temp9};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::%s ", sizeof( unsigned char), 8u),
_tag_arr( _temp7, sizeof( void*), 2u));}}});}({ void* _temp10[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp10,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_arr fmt, struct _tagged_arr ap){ struct _tagged_arr msg=(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap); Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp11=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp11->hd=( void*) sg; _temp11->tl= Cyc_Tcutil_warning_segs;
_temp11;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp12=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp12->hd=( void*)({
struct _tagged_arr* _temp13=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp13[ 0]= msg; _temp13;}); _temp12->tl= Cyc_Tcutil_warning_msgs;
_temp12;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs ==  0){
return;}({ void* _temp14[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("***Warnings***\n",
sizeof( unsigned char), 16u), _tag_arr( _temp14, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp15= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs !=  0) {({ struct Cyc_Std_String_pa_struct _temp18;
_temp18.tag= Cyc_Std_String_pa; _temp18.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Std_String_pa_struct _temp17; _temp17.tag= Cyc_Std_String_pa; _temp17.f1=(
struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
_temp15))->hd);{ void* _temp16[ 2u]={& _temp17,& _temp18}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp16, sizeof(
void*), 2u));}}}); _temp15=(( struct Cyc_List_List*) _check_null( _temp15))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}({
void* _temp19[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("**************\n",
sizeof( unsigned char), 16u), _tag_arr( _temp19, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) - *(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp20= t; struct Cyc_Core_Opt*
_temp32; struct Cyc_Core_Opt* _temp34; struct Cyc_Core_Opt** _temp36; struct Cyc_Core_Opt*
_temp37; struct Cyc_Core_Opt* _temp39; struct Cyc_Core_Opt** _temp41; _LL22: if((
unsigned int) _temp20 >  4u?*(( int*) _temp20) ==  Cyc_Absyn_Evar: 0){ _LL33:
_temp32=(( struct Cyc_Absyn_Evar_struct*) _temp20)->f2; if( _temp32 ==  0){ goto
_LL23;} else{ goto _LL24;}} else{ goto _LL24;} _LL24: if(( unsigned int) _temp20
>  4u?*(( int*) _temp20) ==  Cyc_Absyn_Evar: 0){ _LL35: _temp34=(( struct Cyc_Absyn_Evar_struct*)
_temp20)->f2; _temp36=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp20)->f2; goto _LL25;} else{ goto _LL26;} _LL26: if(( unsigned int) _temp20
>  4u?*(( int*) _temp20) ==  Cyc_Absyn_TypedefType: 0){ _LL38: _temp37=(( struct
Cyc_Absyn_TypedefType_struct*) _temp20)->f3; if( _temp37 ==  0){ goto _LL27;}
else{ goto _LL28;}} else{ goto _LL28;} _LL28: if(( unsigned int) _temp20 >  4u?*((
int*) _temp20) ==  Cyc_Absyn_TypedefType: 0){ _LL40: _temp39=(( struct Cyc_Absyn_TypedefType_struct*)
_temp20)->f3; _temp41=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp20)->f3; goto _LL29;} else{ goto _LL30;} _LL30: goto _LL31; _LL23: return t;
_LL25: { void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp36))->v); if( t2 != ( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp36))->v){* _temp36=({ struct Cyc_Core_Opt* _temp42=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp42->v=( void*) t2;
_temp42;});} return t2;} _LL27: return t; _LL29: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp41))->v); if( t2 != ( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp41))->v){* _temp41=({ struct Cyc_Core_Opt*
_temp43=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp43->v=( void*) t2; _temp43;});} return t2;} _LL31: return t; _LL21:;} void*
Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp44=( void*) c->v; void* _temp52; struct Cyc_Absyn_Conref* _temp54;
_LL46: if( _temp44 == ( void*) Cyc_Absyn_No_constr){ goto _LL47;} else{ goto
_LL48;} _LL48: if(( unsigned int) _temp44 >  1u?*(( int*) _temp44) ==  Cyc_Absyn_Eq_constr:
0){ _LL53: _temp52=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp44)->f1;
goto _LL49;} else{ goto _LL50;} _LL50: if(( unsigned int) _temp44 >  1u?*(( int*)
_temp44) ==  Cyc_Absyn_Forward_constr: 0){ _LL55: _temp54=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp44)->f1; goto _LL51;} else{ goto _LL45;} _LL47: return Cyc_Absyn_empty_conref();
_LL49: return Cyc_Absyn_new_conref( _temp52); _LL51: return Cyc_Tcutil_copy_conref(
_temp54); _LL45:;} static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_Absyn_Tvar* _temp56=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp56->name= tv->name; _temp56->identity=
0; _temp56->kind= Cyc_Tcutil_copy_conref( tv->kind); _temp56;});} static struct
_tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp59; struct Cyc_Absyn_Tqual
_temp61; struct Cyc_Core_Opt* _temp63; struct _tuple2 _temp57=* arg; _LL64:
_temp63= _temp57.f1; goto _LL62; _LL62: _temp61= _temp57.f2; goto _LL60; _LL60:
_temp59= _temp57.f3; goto _LL58; _LL58: return({ struct _tuple2* _temp65=(
struct _tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp65->f1= _temp63;
_temp65->f2= _temp61; _temp65->f3= Cyc_Tcutil_copy_type( _temp59); _temp65;});}
static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){ void* _temp68;
struct Cyc_Absyn_Tqual _temp70; struct _tuple4 _temp66=* arg; _LL71: _temp70=
_temp66.f1; goto _LL69; _LL69: _temp68= _temp66.f2; goto _LL67; _LL67: return({
struct _tuple4* _temp72=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp72->f1= _temp70; _temp72->f2= Cyc_Tcutil_copy_type( _temp68); _temp72;});}
static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field( struct Cyc_Absyn_Structfield*
f){ return({ struct Cyc_Absyn_Structfield* _temp73=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp73->name= f->name;
_temp73->tq= f->tq; _temp73->type=( void*) Cyc_Tcutil_copy_type(( void*) f->type);
_temp73->width= f->width; _temp73->attributes= f->attributes; _temp73;});}
struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp76; void* _temp77; void* _temp79; struct
_tuple8* _temp74= x; _temp76=* _temp74; _LL80: _temp79= _temp76.f1; goto _LL78;
_LL78: _temp77= _temp76.f2; goto _LL75; _LL75: return({ struct _tuple8* _temp81=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp81->f1= Cyc_Tcutil_copy_type(
_temp79); _temp81->f2= Cyc_Tcutil_copy_type( _temp77); _temp81;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp82= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp130; struct Cyc_Absyn_TunionInfo _temp132; void* _temp134; struct Cyc_List_List*
_temp136; void* _temp138; struct Cyc_Absyn_TunionFieldInfo _temp140; struct Cyc_List_List*
_temp142; void* _temp144; struct Cyc_Absyn_PtrInfo _temp146; struct Cyc_Absyn_Conref*
_temp148; struct Cyc_Absyn_Tqual _temp150; struct Cyc_Absyn_Conref* _temp152;
void* _temp154; void* _temp156; void* _temp158; void* _temp160; struct Cyc_Absyn_Exp*
_temp162; struct Cyc_Absyn_Tqual _temp164; void* _temp166; struct Cyc_Absyn_FnInfo
_temp168; struct Cyc_List_List* _temp170; struct Cyc_List_List* _temp172; struct
Cyc_Absyn_VarargInfo* _temp174; int _temp176; struct Cyc_List_List* _temp178;
void* _temp180; struct Cyc_Core_Opt* _temp182; struct Cyc_List_List* _temp184;
struct Cyc_List_List* _temp186; struct Cyc_List_List* _temp188; struct _tuple1*
_temp190; struct Cyc_List_List* _temp192; struct _tuple1* _temp194; struct Cyc_List_List*
_temp196; struct Cyc_List_List* _temp198; struct _tuple1* _temp200; void*
_temp202; struct Cyc_List_List* _temp204; struct _tuple1* _temp206; void*
_temp208; struct Cyc_List_List* _temp210; void* _temp212; _LL84: if( _temp82 == (
void*) Cyc_Absyn_VoidType){ goto _LL85;} else{ goto _LL86;} _LL86: if((
unsigned int) _temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_Evar: 0){ goto
_LL87;} else{ goto _LL88;} _LL88: if(( unsigned int) _temp82 >  4u?*(( int*)
_temp82) ==  Cyc_Absyn_VarType: 0){ _LL131: _temp130=(( struct Cyc_Absyn_VarType_struct*)
_temp82)->f1; goto _LL89;} else{ goto _LL90;} _LL90: if(( unsigned int) _temp82
>  4u?*(( int*) _temp82) ==  Cyc_Absyn_TunionType: 0){ _LL133: _temp132=((
struct Cyc_Absyn_TunionType_struct*) _temp82)->f1; _LL139: _temp138=( void*)
_temp132.tunion_info; goto _LL137; _LL137: _temp136= _temp132.targs; goto _LL135;
_LL135: _temp134=( void*) _temp132.rgn; goto _LL91;} else{ goto _LL92;} _LL92:
if(( unsigned int) _temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_TunionFieldType:
0){ _LL141: _temp140=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp82)->f1;
_LL145: _temp144=( void*) _temp140.field_info; goto _LL143; _LL143: _temp142=
_temp140.targs; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int)
_temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_PointerType: 0){ _LL147: _temp146=((
struct Cyc_Absyn_PointerType_struct*) _temp82)->f1; _LL157: _temp156=( void*)
_temp146.elt_typ; goto _LL155; _LL155: _temp154=( void*) _temp146.rgn_typ; goto
_LL153; _LL153: _temp152= _temp146.nullable; goto _LL151; _LL151: _temp150=
_temp146.tq; goto _LL149; _LL149: _temp148= _temp146.bounds; goto _LL95;} else{
goto _LL96;} _LL96: if(( unsigned int) _temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_IntType:
0){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp82)->f1;
goto _LL159; _LL159: _temp158=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp82)->f2; goto _LL97;} else{ goto _LL98;} _LL98: if( _temp82 == ( void*) Cyc_Absyn_FloatType){
goto _LL99;} else{ goto _LL100;} _LL100: if( _temp82 == ( void*) Cyc_Absyn_DoubleType){
goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int) _temp82 >  4u?*((
int*) _temp82) ==  Cyc_Absyn_ArrayType: 0){ _LL167: _temp166=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp82)->f1; goto _LL165; _LL165: _temp164=((
struct Cyc_Absyn_ArrayType_struct*) _temp82)->f2; goto _LL163; _LL163: _temp162=((
struct Cyc_Absyn_ArrayType_struct*) _temp82)->f3; goto _LL103;} else{ goto
_LL104;} _LL104: if(( unsigned int) _temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_FnType:
0){ _LL169: _temp168=(( struct Cyc_Absyn_FnType_struct*) _temp82)->f1; _LL185:
_temp184= _temp168.tvars; goto _LL183; _LL183: _temp182= _temp168.effect; goto
_LL181; _LL181: _temp180=( void*) _temp168.ret_typ; goto _LL179; _LL179:
_temp178= _temp168.args; goto _LL177; _LL177: _temp176= _temp168.c_varargs; goto
_LL175; _LL175: _temp174= _temp168.cyc_varargs; goto _LL173; _LL173: _temp172=
_temp168.rgn_po; goto _LL171; _LL171: _temp170= _temp168.attributes; goto _LL105;}
else{ goto _LL106;} _LL106: if(( unsigned int) _temp82 >  4u?*(( int*) _temp82)
==  Cyc_Absyn_TupleType: 0){ _LL187: _temp186=(( struct Cyc_Absyn_TupleType_struct*)
_temp82)->f1; goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int)
_temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_StructType: 0){ _LL191: _temp190=((
struct Cyc_Absyn_StructType_struct*) _temp82)->f1; goto _LL189; _LL189: _temp188=((
struct Cyc_Absyn_StructType_struct*) _temp82)->f2; goto _LL109;} else{ goto
_LL110;} _LL110: if(( unsigned int) _temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_UnionType:
0){ _LL195: _temp194=(( struct Cyc_Absyn_UnionType_struct*) _temp82)->f1; goto
_LL193; _LL193: _temp192=(( struct Cyc_Absyn_UnionType_struct*) _temp82)->f2;
goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp82 >  4u?*((
int*) _temp82) ==  Cyc_Absyn_AnonStructType: 0){ _LL197: _temp196=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp82)->f1; goto _LL113;} else{ goto _LL114;} _LL114: if(( unsigned int)
_temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_AnonUnionType: 0){ _LL199:
_temp198=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp82)->f1; goto _LL115;}
else{ goto _LL116;} _LL116: if(( unsigned int) _temp82 >  4u?*(( int*) _temp82)
==  Cyc_Absyn_EnumType: 0){ _LL201: _temp200=(( struct Cyc_Absyn_EnumType_struct*)
_temp82)->f1; goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int)
_temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_RgnHandleType: 0){ _LL203:
_temp202=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp82)->f1; goto
_LL119;} else{ goto _LL120;} _LL120: if(( unsigned int) _temp82 >  4u?*(( int*)
_temp82) ==  Cyc_Absyn_TypedefType: 0){ _LL207: _temp206=(( struct Cyc_Absyn_TypedefType_struct*)
_temp82)->f1; goto _LL205; _LL205: _temp204=(( struct Cyc_Absyn_TypedefType_struct*)
_temp82)->f2; goto _LL121;} else{ goto _LL122;} _LL122: if( _temp82 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_AccessEff: 0){ _LL209: _temp208=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp82)->f1; goto _LL125;} else{
goto _LL126;} _LL126: if(( unsigned int) _temp82 >  4u?*(( int*) _temp82) == 
Cyc_Absyn_JoinEff: 0){ _LL211: _temp210=(( struct Cyc_Absyn_JoinEff_struct*)
_temp82)->f1; goto _LL127;} else{ goto _LL128;} _LL128: if(( unsigned int)
_temp82 >  4u?*(( int*) _temp82) ==  Cyc_Absyn_RgnsEff: 0){ _LL213: _temp212=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp82)->f1; goto _LL129;} else{
goto _LL83;} _LL85: goto _LL87; _LL87: return t; _LL89: return( void*)({ struct
Cyc_Absyn_VarType_struct* _temp214=( struct Cyc_Absyn_VarType_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp214[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp215; _temp215.tag= Cyc_Absyn_VarType; _temp215.f1= Cyc_Tcutil_copy_tvar(
_temp130); _temp215;}); _temp214;}); _LL91: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp216=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp216[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp217; _temp217.tag= Cyc_Absyn_TunionType;
_temp217.f1=({ struct Cyc_Absyn_TunionInfo _temp218; _temp218.tunion_info=( void*)
_temp138; _temp218.targs= Cyc_Tcutil_copy_types( _temp136); _temp218.rgn=( void*)
Cyc_Tcutil_copy_type( _temp134); _temp218;}); _temp217;}); _temp216;}); _LL93:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp219=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp219[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp220; _temp220.tag= Cyc_Absyn_TunionFieldType;
_temp220.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp221; _temp221.field_info=(
void*) _temp144; _temp221.targs= Cyc_Tcutil_copy_types( _temp142); _temp221;});
_temp220;}); _temp219;}); _LL95: { void* _temp222= Cyc_Tcutil_copy_type(
_temp156); void* _temp223= Cyc_Tcutil_copy_type( _temp154); struct Cyc_Absyn_Conref*
_temp224=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp152); struct Cyc_Absyn_Tqual _temp225= _temp150; struct Cyc_Absyn_Conref*
_temp226= Cyc_Tcutil_copy_conref( _temp148); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp227=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp227[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp228; _temp228.tag= Cyc_Absyn_PointerType;
_temp228.f1=({ struct Cyc_Absyn_PtrInfo _temp229; _temp229.elt_typ=( void*)
_temp222; _temp229.rgn_typ=( void*) _temp223; _temp229.nullable= _temp224;
_temp229.tq= _temp225; _temp229.bounds= _temp226; _temp229;}); _temp228;});
_temp227;});} _LL97: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp230=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp230[ 0]=({ struct Cyc_Absyn_IntType_struct _temp231; _temp231.tag= Cyc_Absyn_IntType;
_temp231.f1=( void*) _temp160; _temp231.f2=( void*) _temp158; _temp231;});
_temp230;}); _LL99: goto _LL101; _LL101: return t; _LL103: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp232=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp232[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp233; _temp233.tag= Cyc_Absyn_ArrayType; _temp233.f1=(
void*) Cyc_Tcutil_copy_type( _temp166); _temp233.f2= _temp164; _temp233.f3=
_temp162; _temp233;}); _temp232;}); _LL105: { struct Cyc_List_List* _temp234=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp184); struct
Cyc_Core_Opt* _temp235= _temp182 ==  0? 0:({ struct Cyc_Core_Opt* _temp245=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp245->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp182))->v); _temp245;}); void* _temp236= Cyc_Tcutil_copy_type( _temp180);
struct Cyc_List_List* _temp237=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp178); int _temp238= _temp176; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp174 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp174); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp239=(
struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp239->name= cv->name; _temp239->tq= cv->tq; _temp239->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp239->inject= cv->inject; _temp239;});}{ struct Cyc_List_List*
_temp240=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp172);
struct Cyc_List_List* _temp241= _temp170; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp242=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp242[ 0]=({ struct Cyc_Absyn_FnType_struct _temp243; _temp243.tag= Cyc_Absyn_FnType;
_temp243.f1=({ struct Cyc_Absyn_FnInfo _temp244; _temp244.tvars= _temp234;
_temp244.effect= _temp235; _temp244.ret_typ=( void*) _temp236; _temp244.args=
_temp237; _temp244.c_varargs= _temp238; _temp244.cyc_varargs= cyc_varargs2;
_temp244.rgn_po= _temp240; _temp244.attributes= _temp241; _temp244;}); _temp243;});
_temp242;});}} _LL107: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp246=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp246[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp247; _temp247.tag= Cyc_Absyn_TupleType;
_temp247.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp186);
_temp247;}); _temp246;}); _LL109: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp248=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp248[ 0]=({ struct Cyc_Absyn_StructType_struct _temp249; _temp249.tag= Cyc_Absyn_StructType;
_temp249.f1= _temp190; _temp249.f2= Cyc_Tcutil_copy_types( _temp188); _temp249.f3=
0; _temp249;}); _temp248;}); _LL111: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp250=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp250[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp251; _temp251.tag= Cyc_Absyn_UnionType;
_temp251.f1= _temp194; _temp251.f2= Cyc_Tcutil_copy_types( _temp192); _temp251.f3=
0; _temp251;}); _temp250;}); _LL113: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp252=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp252[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp253; _temp253.tag= Cyc_Absyn_AnonStructType; _temp253.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp196); _temp253;}); _temp252;});
_LL115: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp254=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp254[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp255; _temp255.tag=
Cyc_Absyn_AnonUnionType; _temp255.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp198); _temp255;}); _temp254;}); _LL117: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp256=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp256[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp257; _temp257.tag= Cyc_Absyn_EnumType;
_temp257.f1= _temp200; _temp257.f2= 0; _temp257;}); _temp256;}); _LL119: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp258=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp258[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp259; _temp259.tag= Cyc_Absyn_RgnHandleType;
_temp259.f1=( void*) Cyc_Tcutil_copy_type( _temp202); _temp259;}); _temp258;});
_LL121: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp260=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp260[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp261; _temp261.tag= Cyc_Absyn_TypedefType;
_temp261.f1= _temp206; _temp261.f2= Cyc_Tcutil_copy_types( _temp204); _temp261.f3=
0; _temp261;}); _temp260;}); _LL123: return t; _LL125: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp262=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp262[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp263; _temp263.tag= Cyc_Absyn_AccessEff; _temp263.f1=(
void*) Cyc_Tcutil_copy_type( _temp208); _temp263;}); _temp262;}); _LL127: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp264=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp264[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp265; _temp265.tag= Cyc_Absyn_JoinEff; _temp265.f1= Cyc_Tcutil_copy_types(
_temp210); _temp265;}); _temp264;}); _LL129: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp266=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp266[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp267; _temp267.tag= Cyc_Absyn_RgnsEff;
_temp267.f1=( void*) Cyc_Tcutil_copy_type( _temp212); _temp267;}); _temp266;});
_LL83:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){
return 1;}{ struct _tuple8 _temp269=({ struct _tuple8 _temp268; _temp268.f1= k1;
_temp268.f2= k2; _temp268;}); void* _temp279; void* _temp281; void* _temp283;
void* _temp285; void* _temp287; void* _temp289; _LL271: _LL282: _temp281=
_temp269.f1; if( _temp281 == ( void*) Cyc_Absyn_BoxKind){ goto _LL280;} else{
goto _LL273;} _LL280: _temp279= _temp269.f2; if( _temp279 == ( void*) Cyc_Absyn_MemKind){
goto _LL272;} else{ goto _LL273;} _LL273: _LL286: _temp285= _temp269.f1; if(
_temp285 == ( void*) Cyc_Absyn_BoxKind){ goto _LL284;} else{ goto _LL275;}
_LL284: _temp283= _temp269.f2; if( _temp283 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL274;} else{ goto _LL275;} _LL275: _LL290: _temp289= _temp269.f1; if( _temp289
== ( void*) Cyc_Absyn_MemKind){ goto _LL288;} else{ goto _LL277;} _LL288:
_temp287= _temp269.f2; if( _temp287 == ( void*) Cyc_Absyn_AnyKind){ goto _LL276;}
else{ goto _LL277;} _LL277: goto _LL278; _LL272: goto _LL274; _LL274: goto
_LL276; _LL276: return 1; _LL278: return 0; _LL270:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp291= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp345;
struct Cyc_Core_Opt* _temp347; struct Cyc_Absyn_Tvar* _temp349; void* _temp351;
struct Cyc_Absyn_TunionFieldInfo _temp353; void* _temp355; struct Cyc_Absyn_Tunionfield*
_temp357; struct Cyc_Absyn_TunionFieldInfo _temp359; void* _temp361; struct Cyc_Absyn_Structdecl**
_temp363; struct Cyc_Absyn_Uniondecl** _temp365; struct Cyc_Absyn_PtrInfo
_temp367; struct Cyc_Core_Opt* _temp369; _LL293: if(( unsigned int) _temp291 > 
4u?*(( int*) _temp291) ==  Cyc_Absyn_Evar: 0){ _LL348: _temp347=(( struct Cyc_Absyn_Evar_struct*)
_temp291)->f1; goto _LL346; _LL346: _temp345=(( struct Cyc_Absyn_Evar_struct*)
_temp291)->f2; goto _LL294;} else{ goto _LL295;} _LL295: if(( unsigned int)
_temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_VarType: 0){ _LL350: _temp349=((
struct Cyc_Absyn_VarType_struct*) _temp291)->f1; goto _LL296;} else{ goto _LL297;}
_LL297: if( _temp291 == ( void*) Cyc_Absyn_VoidType){ goto _LL298;} else{ goto
_LL299;} _LL299: if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_IntType:
0){ _LL352: _temp351=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp291)->f2;
goto _LL300;} else{ goto _LL301;} _LL301: if( _temp291 == ( void*) Cyc_Absyn_FloatType){
goto _LL302;} else{ goto _LL303;} _LL303: if( _temp291 == ( void*) Cyc_Absyn_DoubleType){
goto _LL304;} else{ goto _LL305;} _LL305: if(( unsigned int) _temp291 >  4u?*((
int*) _temp291) ==  Cyc_Absyn_FnType: 0){ goto _LL306;} else{ goto _LL307;}
_LL307: if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL308;} else{ goto _LL309;} _LL309: if( _temp291 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp291 >  4u?*((
int*) _temp291) ==  Cyc_Absyn_TunionType: 0){ goto _LL312;} else{ goto _LL313;}
_LL313: if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_TunionFieldType:
0){ _LL354: _temp353=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp291)->f1;
_LL356: _temp355=( void*) _temp353.field_info; if(*(( int*) _temp355) ==  Cyc_Absyn_KnownTunionfield){
_LL358: _temp357=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp355)->f2;
goto _LL314;} else{ goto _LL315;}} else{ goto _LL315;} _LL315: if(( unsigned int)
_temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_TunionFieldType: 0){ _LL360:
_temp359=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp291)->f1; _LL362:
_temp361=( void*) _temp359.field_info; if(*(( int*) _temp361) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL316;} else{ goto _LL317;}} else{ goto _LL317;} _LL317: if(( unsigned int)
_temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_StructType: 0){ _LL364:
_temp363=(( struct Cyc_Absyn_StructType_struct*) _temp291)->f3; if( _temp363 == 
0){ goto _LL318;} else{ goto _LL319;}} else{ goto _LL319;} _LL319: if((
unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_UnionType: 0){
_LL366: _temp365=(( struct Cyc_Absyn_UnionType_struct*) _temp291)->f3; if(
_temp365 ==  0){ goto _LL320;} else{ goto _LL321;}} else{ goto _LL321;} _LL321:
if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_StructType:
0){ goto _LL322;} else{ goto _LL323;} _LL323: if(( unsigned int) _temp291 >  4u?*((
int*) _temp291) ==  Cyc_Absyn_UnionType: 0){ goto _LL324;} else{ goto _LL325;}
_LL325: if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int) _temp291 >  4u?*((
int*) _temp291) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL328;} else{ goto _LL329;}
_LL329: if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_EnumType:
0){ goto _LL330;} else{ goto _LL331;} _LL331: if(( unsigned int) _temp291 >  4u?*((
int*) _temp291) ==  Cyc_Absyn_PointerType: 0){ _LL368: _temp367=(( struct Cyc_Absyn_PointerType_struct*)
_temp291)->f1; goto _LL332;} else{ goto _LL333;} _LL333: if(( unsigned int)
_temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_ArrayType: 0){ goto _LL334;}
else{ goto _LL335;} _LL335: if(( unsigned int) _temp291 >  4u?*(( int*) _temp291)
==  Cyc_Absyn_TupleType: 0){ goto _LL336;} else{ goto _LL337;} _LL337: if((
unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_TypedefType: 0){
_LL370: _temp369=(( struct Cyc_Absyn_TypedefType_struct*) _temp291)->f3; goto
_LL338;} else{ goto _LL339;} _LL339: if(( unsigned int) _temp291 >  4u?*(( int*)
_temp291) ==  Cyc_Absyn_AccessEff: 0){ goto _LL340;} else{ goto _LL341;} _LL341:
if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_JoinEff: 0){
goto _LL342;} else{ goto _LL343;} _LL343: if(( unsigned int) _temp291 >  4u?*((
int*) _temp291) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL344;} else{ goto _LL292;}
_LL294: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp347))->v;
_LL296: return Cyc_Absyn_conref_val( _temp349->kind); _LL298: return( void*) Cyc_Absyn_MemKind;
_LL300: return _temp351 == ( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL302: goto _LL304; _LL304: goto _LL306; _LL306:
return( void*) Cyc_Absyn_MemKind; _LL308: return( void*) Cyc_Absyn_BoxKind;
_LL310: return( void*) Cyc_Absyn_RgnKind; _LL312: return( void*) Cyc_Absyn_BoxKind;
_LL314: if( _temp357->typs ==  0){ return( void*) Cyc_Absyn_BoxKind;} else{
return( void*) Cyc_Absyn_MemKind;} _LL316: return({ void* _temp371[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("typ_kind: Unresolved TunionFieldType", sizeof( unsigned char), 37u),
_tag_arr( _temp371, sizeof( void*), 0u));}); _LL318: goto _LL320; _LL320: return(
void*) Cyc_Absyn_AnyKind; _LL322: goto _LL324; _LL324: goto _LL326; _LL326: goto
_LL328; _LL328: return( void*) Cyc_Absyn_MemKind; _LL330: return( void*) Cyc_Absyn_BoxKind;
_LL332: { void* _temp372=( void*)( Cyc_Absyn_compress_conref( _temp367.bounds))->v;
void* _temp382; void* _temp384; _LL374: if(( unsigned int) _temp372 >  1u?*((
int*) _temp372) ==  Cyc_Absyn_Eq_constr: 0){ _LL383: _temp382=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp372)->f1; if( _temp382 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL375;} else{ goto _LL376;}} else{ goto _LL376;} _LL376: if(( unsigned int)
_temp372 >  1u?*(( int*) _temp372) ==  Cyc_Absyn_Eq_constr: 0){ _LL385: _temp384=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp372)->f1; if(( unsigned int)
_temp384 >  1u?*(( int*) _temp384) ==  Cyc_Absyn_Upper_b: 0){ goto _LL377;}
else{ goto _LL378;}} else{ goto _LL378;} _LL378: if( _temp372 == ( void*) Cyc_Absyn_No_constr){
goto _LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp372 >  1u?*((
int*) _temp372) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL381;} else{ goto
_LL373;} _LL375: return( void*) Cyc_Absyn_MemKind; _LL377: return( void*) Cyc_Absyn_BoxKind;
_LL379: return( void*) Cyc_Absyn_MemKind; _LL381: return({ void* _temp386[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds", sizeof(
unsigned char), 43u), _tag_arr( _temp386, sizeof( void*), 0u));}); _LL373:;}
_LL334: goto _LL336; _LL336: return( void*) Cyc_Absyn_MemKind; _LL338: return({
struct Cyc_Std_String_pa_struct _temp388; _temp388.tag= Cyc_Std_String_pa;
_temp388.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp387[ 1u]={&
_temp388}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp387, sizeof( void*), 1u));}}); _LL340: goto
_LL342; _LL342: goto _LL344; _LL344: return( void*) Cyc_Absyn_EffKind; _LL292:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp389; _push_handler(& _temp389);{
int _temp391= 0; if( setjmp( _temp389.handler)){ _temp391= 1;} if( ! _temp391){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp392= 1; _npop_handler( 0u); return
_temp392;}; _pop_handler();} else{ void* _temp390=( void*) _exn_thrown; void*
_temp394= _temp390; _LL396: if( _temp394 ==  Cyc_Tcutil_Unify){ goto _LL397;}
else{ goto _LL398;} _LL398: goto _LL399; _LL397: return 0; _LL399:( void) _throw(
_temp394); _LL395:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp400= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp436; struct Cyc_Core_Opt* _temp438; struct Cyc_Core_Opt**
_temp440; struct Cyc_Core_Opt* _temp441; struct Cyc_Absyn_PtrInfo _temp443; void*
_temp445; struct Cyc_Absyn_FnInfo _temp447; struct Cyc_List_List* _temp449;
struct Cyc_List_List* _temp451; struct Cyc_Absyn_VarargInfo* _temp453; int
_temp455; struct Cyc_List_List* _temp457; void* _temp459; struct Cyc_Core_Opt*
_temp461; struct Cyc_List_List* _temp463; struct Cyc_List_List* _temp465; void*
_temp467; struct Cyc_Absyn_TunionInfo _temp469; void* _temp471; struct Cyc_List_List*
_temp473; struct Cyc_Core_Opt* _temp475; struct Cyc_List_List* _temp477; struct
Cyc_Absyn_TunionFieldInfo _temp479; struct Cyc_List_List* _temp481; struct Cyc_List_List*
_temp483; struct Cyc_List_List* _temp485; struct Cyc_List_List* _temp487; void*
_temp489; struct Cyc_List_List* _temp491; void* _temp493; _LL402: if((
unsigned int) _temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_VarType: 0){
_LL437: _temp436=(( struct Cyc_Absyn_VarType_struct*) _temp400)->f1; goto _LL403;}
else{ goto _LL404;} _LL404: if(( unsigned int) _temp400 >  4u?*(( int*) _temp400)
==  Cyc_Absyn_Evar: 0){ _LL442: _temp441=(( struct Cyc_Absyn_Evar_struct*)
_temp400)->f2; goto _LL439; _LL439: _temp438=(( struct Cyc_Absyn_Evar_struct*)
_temp400)->f4; _temp440=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp400)->f4; goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int)
_temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_PointerType: 0){ _LL444:
_temp443=(( struct Cyc_Absyn_PointerType_struct*) _temp400)->f1; goto _LL407;}
else{ goto _LL408;} _LL408: if(( unsigned int) _temp400 >  4u?*(( int*) _temp400)
==  Cyc_Absyn_ArrayType: 0){ _LL446: _temp445=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp400)->f1; goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int)
_temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_FnType: 0){ _LL448: _temp447=((
struct Cyc_Absyn_FnType_struct*) _temp400)->f1; _LL464: _temp463= _temp447.tvars;
goto _LL462; _LL462: _temp461= _temp447.effect; goto _LL460; _LL460: _temp459=(
void*) _temp447.ret_typ; goto _LL458; _LL458: _temp457= _temp447.args; goto
_LL456; _LL456: _temp455= _temp447.c_varargs; goto _LL454; _LL454: _temp453=
_temp447.cyc_varargs; goto _LL452; _LL452: _temp451= _temp447.rgn_po; goto
_LL450; _LL450: _temp449= _temp447.attributes; goto _LL411;} else{ goto _LL412;}
_LL412: if(( unsigned int) _temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_TupleType:
0){ _LL466: _temp465=(( struct Cyc_Absyn_TupleType_struct*) _temp400)->f1; goto
_LL413;} else{ goto _LL414;} _LL414: if(( unsigned int) _temp400 >  4u?*(( int*)
_temp400) ==  Cyc_Absyn_RgnHandleType: 0){ _LL468: _temp467=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp400)->f1; goto _LL415;} else{ goto _LL416;}
_LL416: if(( unsigned int) _temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_TunionType:
0){ _LL470: _temp469=(( struct Cyc_Absyn_TunionType_struct*) _temp400)->f1;
_LL474: _temp473= _temp469.targs; goto _LL472; _LL472: _temp471=( void*)
_temp469.rgn; goto _LL417;} else{ goto _LL418;} _LL418: if(( unsigned int)
_temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_TypedefType: 0){ _LL478:
_temp477=(( struct Cyc_Absyn_TypedefType_struct*) _temp400)->f2; goto _LL476;
_LL476: _temp475=(( struct Cyc_Absyn_TypedefType_struct*) _temp400)->f3; goto
_LL419;} else{ goto _LL420;} _LL420: if(( unsigned int) _temp400 >  4u?*(( int*)
_temp400) ==  Cyc_Absyn_TunionFieldType: 0){ _LL480: _temp479=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp400)->f1; _LL482: _temp481= _temp479.targs; goto _LL421;} else{ goto _LL422;}
_LL422: if(( unsigned int) _temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_StructType:
0){ _LL484: _temp483=(( struct Cyc_Absyn_StructType_struct*) _temp400)->f2; goto
_LL423;} else{ goto _LL424;} _LL424: if(( unsigned int) _temp400 >  4u?*(( int*)
_temp400) ==  Cyc_Absyn_AnonStructType: 0){ _LL486: _temp485=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp400)->f1; goto _LL425;} else{ goto _LL426;} _LL426: if(( unsigned int)
_temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_AnonUnionType: 0){ _LL488:
_temp487=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp400)->f1; goto _LL427;}
else{ goto _LL428;} _LL428: if(( unsigned int) _temp400 >  4u?*(( int*) _temp400)
==  Cyc_Absyn_AccessEff: 0){ _LL490: _temp489=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp400)->f1; goto _LL429;} else{ goto _LL430;} _LL430: if(( unsigned int)
_temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_JoinEff: 0){ _LL492: _temp491=((
struct Cyc_Absyn_JoinEff_struct*) _temp400)->f1; goto _LL431;} else{ goto _LL432;}
_LL432: if(( unsigned int) _temp400 >  4u?*(( int*) _temp400) ==  Cyc_Absyn_RgnsEff:
0){ _LL494: _temp493=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp400)->f1;
goto _LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL403: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp436)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);} goto
_LL401; _LL405: if( t ==  evar){ Cyc_Tcutil_failure_reason= _tag_arr("(occurs check)",
sizeof( unsigned char), 15u);( int) _throw(( void*) Cyc_Tcutil_Unify);} else{
if( _temp441 !=  0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp441))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp440))->v; for(
0; s !=  0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp495= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp440))->v;
for( 0; s !=  0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp495=({ struct Cyc_List_List* _temp496=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp496->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp496->tl= _temp495; _temp496;});}}}*
_temp440=({ struct Cyc_Core_Opt* _temp497=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp497->v=( void*) _temp495; _temp497;});}}}
goto _LL401; _LL407: Cyc_Tcutil_occurs( evar, env,( void*) _temp443.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp443.rgn_typ); goto _LL401; _LL409:
Cyc_Tcutil_occurs( evar, env, _temp445); goto _LL401; _LL411: if( _temp461 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp461))->v);} Cyc_Tcutil_occurs( evar, env, _temp459); for( 0; _temp457 !=  0;
_temp457=(( struct Cyc_List_List*) _check_null( _temp457))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp457))->hd)).f3);}
if( _temp453 !=  0){ void* _temp500; struct Cyc_Absyn_VarargInfo _temp498=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp453)); _LL501: _temp500=( void*)
_temp498.type; goto _LL499; _LL499: Cyc_Tcutil_occurs( evar, env, _temp500);}
for( 0; _temp451 !=  0; _temp451=(( struct Cyc_List_List*) _check_null( _temp451))->tl){
struct _tuple8 _temp504; void* _temp505; void* _temp507; struct _tuple8*
_temp502=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp451))->hd;
_temp504=* _temp502; _LL508: _temp507= _temp504.f1; goto _LL506; _LL506:
_temp505= _temp504.f2; goto _LL503; _LL503: Cyc_Tcutil_occurs( evar, env,
_temp507); Cyc_Tcutil_occurs( evar, env, _temp505);} goto _LL401; _LL413: for( 0;
_temp465 !=  0; _temp465=(( struct Cyc_List_List*) _check_null( _temp465))->tl){
Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp465))->hd)).f2);} goto _LL401; _LL415: Cyc_Tcutil_occurs( evar,
env, _temp467); goto _LL401; _LL417: Cyc_Tcutil_occurs( evar, env, _temp471);
Cyc_Tcutil_occurslist( evar, env, _temp473); goto _LL401; _LL419: _temp481=
_temp477; goto _LL421; _LL421: _temp483= _temp481; goto _LL423; _LL423: Cyc_Tcutil_occurslist(
evar, env, _temp483); goto _LL401; _LL425: _temp487= _temp485; goto _LL427;
_LL427: for( 0; _temp487 !=  0; _temp487=(( struct Cyc_List_List*) _check_null(
_temp487))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp487))->hd)->type);} goto _LL401; _LL429:
Cyc_Tcutil_occurs( evar, env, _temp489); goto _LL401; _LL431: Cyc_Tcutil_occurslist(
evar, env, _temp491); goto _LL401; _LL433: Cyc_Tcutil_occurs( evar, env,
_temp493); goto _LL401; _LL435: goto _LL401; _LL401:;} static void Cyc_Tcutil_occurslist(
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
return;}{ void* _temp509=( void*) x->v; void* _temp517; _LL511: if( _temp509 == (
void*) Cyc_Absyn_No_constr){ goto _LL512;} else{ goto _LL513;} _LL513: if((
unsigned int) _temp509 >  1u?*(( int*) _temp509) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL514;} else{ goto _LL515;} _LL515: if(( unsigned int) _temp509 >  1u?*((
int*) _temp509) ==  Cyc_Absyn_Eq_constr: 0){ _LL518: _temp517=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp509)->f1; goto _LL516;} else{ goto _LL510;}
_LL512:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp519=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp519[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp520; _temp520.tag= Cyc_Absyn_Forward_constr; _temp520.f1= y; _temp520;});
_temp519;}))); return; _LL514:( int) _throw(({ void* _temp521[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp521, sizeof( void*), 0u));})); _LL516: { void* _temp522=( void*)
y->v; void* _temp530; _LL524: if( _temp522 == ( void*) Cyc_Absyn_No_constr){
goto _LL525;} else{ goto _LL526;} _LL526: if(( unsigned int) _temp522 >  1u?*((
int*) _temp522) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL527;} else{ goto
_LL528;} _LL528: if(( unsigned int) _temp522 >  1u?*(( int*) _temp522) ==  Cyc_Absyn_Eq_constr:
0){ _LL531: _temp530=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp522)->f1;
goto _LL529;} else{ goto _LL523;} _LL525:( void*)( y->v=( void*)(( void*) x->v));
return; _LL527:( int) _throw(({ void* _temp532[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp532, sizeof( void*), 0u));})); _LL529: if( cmp( _temp517,
_temp530) !=  0){ Cyc_Tcutil_failure_reason= reason;( int) _throw(( void*) Cyc_Tcutil_Unify);}
return; _LL523:;} _LL510:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp533; _push_handler(& _temp533);{ int _temp535= 0; if( setjmp(
_temp533.handler)){ _temp535= 1;} if( ! _temp535){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u));{ int _temp536= 1;
_npop_handler( 0u); return _temp536;}; _pop_handler();} else{ void* _temp534=(
void*) _exn_thrown; void* _temp538= _temp534; _LL540: if( _temp538 ==  Cyc_Tcutil_Unify){
goto _LL541;} else{ goto _LL542;} _LL542: goto _LL543; _LL541: return 0; _LL543:(
void) _throw( _temp538); _LL539:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp545=({ struct _tuple8 _temp544; _temp544.f1= b1;
_temp544.f2= b2; _temp544;}); void* _temp555; void* _temp557; void* _temp559;
void* _temp561; void* _temp563; struct Cyc_Absyn_Exp* _temp565; void* _temp567;
struct Cyc_Absyn_Exp* _temp569; _LL547: _LL558: _temp557= _temp545.f1; if(
_temp557 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL556;} else{ goto _LL549;}
_LL556: _temp555= _temp545.f2; if( _temp555 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL548;} else{ goto _LL549;} _LL549: _LL560: _temp559= _temp545.f1; if(
_temp559 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL550;} else{ goto _LL551;}
_LL551: _LL562: _temp561= _temp545.f2; if( _temp561 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL552;} else{ goto _LL553;} _LL553: _LL568: _temp567= _temp545.f1; if((
unsigned int) _temp567 >  1u?*(( int*) _temp567) ==  Cyc_Absyn_Upper_b: 0){
_LL570: _temp569=(( struct Cyc_Absyn_Upper_b_struct*) _temp567)->f1; goto _LL564;}
else{ goto _LL546;} _LL564: _temp563= _temp545.f2; if(( unsigned int) _temp563 > 
1u?*(( int*) _temp563) ==  Cyc_Absyn_Upper_b: 0){ _LL566: _temp565=(( struct Cyc_Absyn_Upper_b_struct*)
_temp563)->f1; goto _LL554;} else{ goto _LL546;} _LL548: return 0; _LL550:
return - 1; _LL552: return 1; _LL554: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp569); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp565); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL546:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp572=({
struct _tuple8 _temp571; _temp571.f1= a1; _temp571.f2= a2; _temp571;}); void*
_temp584; int _temp586; int _temp588; void* _temp590; void* _temp592; int
_temp594; int _temp596; void* _temp598; void* _temp600; int _temp602; void*
_temp604; int _temp606; void* _temp608; int _temp610; void* _temp612; int
_temp614; void* _temp616; struct _tagged_arr _temp618; void* _temp620; struct
_tagged_arr _temp622; _LL574: _LL593: _temp592= _temp572.f1; if(( unsigned int)
_temp592 >  16u?*(( int*) _temp592) ==  Cyc_Absyn_Format_att: 0){ _LL599:
_temp598=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp592)->f1; goto
_LL597; _LL597: _temp596=(( struct Cyc_Absyn_Format_att_struct*) _temp592)->f2;
goto _LL595; _LL595: _temp594=(( struct Cyc_Absyn_Format_att_struct*) _temp592)->f3;
goto _LL585;} else{ goto _LL576;} _LL585: _temp584= _temp572.f2; if((
unsigned int) _temp584 >  16u?*(( int*) _temp584) ==  Cyc_Absyn_Format_att: 0){
_LL591: _temp590=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp584)->f1;
goto _LL589; _LL589: _temp588=(( struct Cyc_Absyn_Format_att_struct*) _temp584)->f2;
goto _LL587; _LL587: _temp586=(( struct Cyc_Absyn_Format_att_struct*) _temp584)->f3;
goto _LL575;} else{ goto _LL576;} _LL576: _LL605: _temp604= _temp572.f1; if((
unsigned int) _temp604 >  16u?*(( int*) _temp604) ==  Cyc_Absyn_Regparm_att: 0){
_LL607: _temp606=(( struct Cyc_Absyn_Regparm_att_struct*) _temp604)->f1; goto
_LL601;} else{ goto _LL578;} _LL601: _temp600= _temp572.f2; if(( unsigned int)
_temp600 >  16u?*(( int*) _temp600) ==  Cyc_Absyn_Regparm_att: 0){ _LL603:
_temp602=(( struct Cyc_Absyn_Regparm_att_struct*) _temp600)->f1; goto _LL577;}
else{ goto _LL578;} _LL578: _LL613: _temp612= _temp572.f1; if(( unsigned int)
_temp612 >  16u?*(( int*) _temp612) ==  Cyc_Absyn_Aligned_att: 0){ _LL615:
_temp614=(( struct Cyc_Absyn_Aligned_att_struct*) _temp612)->f1; goto _LL609;}
else{ goto _LL580;} _LL609: _temp608= _temp572.f2; if(( unsigned int) _temp608 > 
16u?*(( int*) _temp608) ==  Cyc_Absyn_Aligned_att: 0){ _LL611: _temp610=((
struct Cyc_Absyn_Aligned_att_struct*) _temp608)->f1; goto _LL579;} else{ goto
_LL580;} _LL580: _LL621: _temp620= _temp572.f1; if(( unsigned int) _temp620 > 
16u?*(( int*) _temp620) ==  Cyc_Absyn_Section_att: 0){ _LL623: _temp622=((
struct Cyc_Absyn_Section_att_struct*) _temp620)->f1; goto _LL617;} else{ goto
_LL582;} _LL617: _temp616= _temp572.f2; if(( unsigned int) _temp616 >  16u?*((
int*) _temp616) ==  Cyc_Absyn_Section_att: 0){ _LL619: _temp618=(( struct Cyc_Absyn_Section_att_struct*)
_temp616)->f1; goto _LL581;} else{ goto _LL582;} _LL582: goto _LL583; _LL575:
return( _temp598 ==  _temp590? _temp596 ==  _temp588: 0)? _temp594 ==  _temp586:
0; _LL577: _temp614= _temp606; _temp610= _temp602; goto _LL579; _LL579: return
_temp614 ==  _temp610; _LL581: return Cyc_Std_strcmp( _temp622, _temp618) ==  0;
_LL583: return 0; _LL573:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,(
void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct
Cyc_List_List* a= a2; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null(
a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void* Cyc_Tcutil_normalize_effect(
void* e); static void* Cyc_Tcutil_rgns_of( void* t); static void* Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Structfield* sf){ return Cyc_Tcutil_rgns_of(( void*) sf->type);}
static struct _tuple5* Cyc_Tcutil_region_free_subst( struct Cyc_Absyn_Tvar* tv){
void* t;{ void* _temp624= Cyc_Absyn_conref_val( tv->kind); _LL626: if( _temp624
== ( void*) Cyc_Absyn_RgnKind){ goto _LL627;} else{ goto _LL628;} _LL628: if(
_temp624 == ( void*) Cyc_Absyn_EffKind){ goto _LL629;} else{ goto _LL630;}
_LL630: goto _LL631; _LL627: t=( void*) Cyc_Absyn_HeapRgn; goto _LL625; _LL629:
t= Cyc_Absyn_empty_effect; goto _LL625; _LL631: t= Cyc_Absyn_sint_t; goto _LL625;
_LL625:;} return({ struct _tuple5* _temp632=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp632->f1= tv; _temp632->f2= t; _temp632;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp633= Cyc_Tcutil_compress(
t); void* _temp681; struct Cyc_Absyn_TunionInfo _temp683; void* _temp685; struct
Cyc_List_List* _temp687; struct Cyc_Absyn_PtrInfo _temp689; void* _temp691; void*
_temp693; void* _temp695; struct Cyc_List_List* _temp697; struct Cyc_Absyn_TunionFieldInfo
_temp699; struct Cyc_List_List* _temp701; struct Cyc_List_List* _temp703; struct
Cyc_List_List* _temp705; struct Cyc_List_List* _temp707; struct Cyc_List_List*
_temp709; struct Cyc_Absyn_FnInfo _temp711; struct Cyc_List_List* _temp713;
struct Cyc_Absyn_VarargInfo* _temp715; struct Cyc_List_List* _temp717; void*
_temp719; struct Cyc_Core_Opt* _temp721; struct Cyc_List_List* _temp723; void*
_temp725; _LL635: if( _temp633 == ( void*) Cyc_Absyn_VoidType){ goto _LL636;}
else{ goto _LL637;} _LL637: if( _temp633 == ( void*) Cyc_Absyn_FloatType){ goto
_LL638;} else{ goto _LL639;} _LL639: if( _temp633 == ( void*) Cyc_Absyn_DoubleType){
goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int) _temp633 >  4u?*((
int*) _temp633) ==  Cyc_Absyn_EnumType: 0){ goto _LL642;} else{ goto _LL643;}
_LL643: if(( unsigned int) _temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_IntType:
0){ goto _LL644;} else{ goto _LL645;} _LL645: if(( unsigned int) _temp633 >  4u?*((
int*) _temp633) ==  Cyc_Absyn_Evar: 0){ goto _LL646;} else{ goto _LL647;} _LL647:
if(( unsigned int) _temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_VarType: 0){
goto _LL648;} else{ goto _LL649;} _LL649: if(( unsigned int) _temp633 >  4u?*((
int*) _temp633) ==  Cyc_Absyn_RgnHandleType: 0){ _LL682: _temp681=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp633)->f1; goto _LL650;} else{ goto
_LL651;} _LL651: if(( unsigned int) _temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_TunionType:
0){ _LL684: _temp683=(( struct Cyc_Absyn_TunionType_struct*) _temp633)->f1;
_LL688: _temp687= _temp683.targs; goto _LL686; _LL686: _temp685=( void*)
_temp683.rgn; goto _LL652;} else{ goto _LL653;} _LL653: if(( unsigned int)
_temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_PointerType: 0){ _LL690:
_temp689=(( struct Cyc_Absyn_PointerType_struct*) _temp633)->f1; _LL694:
_temp693=( void*) _temp689.elt_typ; goto _LL692; _LL692: _temp691=( void*)
_temp689.rgn_typ; goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int)
_temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_ArrayType: 0){ _LL696: _temp695=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp633)->f1; goto _LL656;} else{
goto _LL657;} _LL657: if(( unsigned int) _temp633 >  4u?*(( int*) _temp633) == 
Cyc_Absyn_TupleType: 0){ _LL698: _temp697=(( struct Cyc_Absyn_TupleType_struct*)
_temp633)->f1; goto _LL658;} else{ goto _LL659;} _LL659: if(( unsigned int)
_temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_TunionFieldType: 0){ _LL700:
_temp699=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp633)->f1; _LL702:
_temp701= _temp699.targs; goto _LL660;} else{ goto _LL661;} _LL661: if((
unsigned int) _temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_StructType: 0){
_LL704: _temp703=(( struct Cyc_Absyn_StructType_struct*) _temp633)->f2; goto
_LL662;} else{ goto _LL663;} _LL663: if(( unsigned int) _temp633 >  4u?*(( int*)
_temp633) ==  Cyc_Absyn_UnionType: 0){ _LL706: _temp705=(( struct Cyc_Absyn_UnionType_struct*)
_temp633)->f2; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_AnonStructType: 0){ _LL708:
_temp707=(( struct Cyc_Absyn_AnonStructType_struct*) _temp633)->f1; goto _LL666;}
else{ goto _LL667;} _LL667: if(( unsigned int) _temp633 >  4u?*(( int*) _temp633)
==  Cyc_Absyn_AnonUnionType: 0){ _LL710: _temp709=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp633)->f1; goto _LL668;} else{ goto _LL669;} _LL669: if(( unsigned int)
_temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_FnType: 0){ _LL712: _temp711=((
struct Cyc_Absyn_FnType_struct*) _temp633)->f1; _LL724: _temp723= _temp711.tvars;
goto _LL722; _LL722: _temp721= _temp711.effect; goto _LL720; _LL720: _temp719=(
void*) _temp711.ret_typ; goto _LL718; _LL718: _temp717= _temp711.args; goto
_LL716; _LL716: _temp715= _temp711.cyc_varargs; goto _LL714; _LL714: _temp713=
_temp711.rgn_po; goto _LL670;} else{ goto _LL671;} _LL671: if( _temp633 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL672;} else{ goto _LL673;} _LL673: if((
unsigned int) _temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_AccessEff: 0){
goto _LL674;} else{ goto _LL675;} _LL675: if(( unsigned int) _temp633 >  4u?*((
int*) _temp633) ==  Cyc_Absyn_JoinEff: 0){ goto _LL676;} else{ goto _LL677;}
_LL677: if(( unsigned int) _temp633 >  4u?*(( int*) _temp633) ==  Cyc_Absyn_RgnsEff:
0){ _LL726: _temp725=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp633)->f1;
goto _LL678;} else{ goto _LL679;} _LL679: if(( unsigned int) _temp633 >  4u?*((
int*) _temp633) ==  Cyc_Absyn_TypedefType: 0){ goto _LL680;} else{ goto _LL634;}
_LL636: goto _LL638; _LL638: goto _LL640; _LL640: goto _LL642; _LL642: goto
_LL644; _LL644: return Cyc_Absyn_empty_effect; _LL646: goto _LL648; _LL648: {
void* _temp727= Cyc_Tcutil_typ_kind( t); _LL729: if( _temp727 == ( void*) Cyc_Absyn_RgnKind){
goto _LL730;} else{ goto _LL731;} _LL731: if( _temp727 == ( void*) Cyc_Absyn_EffKind){
goto _LL732;} else{ goto _LL733;} _LL733: goto _LL734; _LL730: return( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp735=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp735[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp736; _temp736.tag= Cyc_Absyn_AccessEff; _temp736.f1=(
void*) t; _temp736;}); _temp735;}); _LL732: return t; _LL734: return( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp737=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp737[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp738; _temp738.tag= Cyc_Absyn_RgnsEff; _temp738.f1=( void*) t; _temp738;});
_temp737;}); _LL728:;} _LL650: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp739=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp739[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp740; _temp740.tag= Cyc_Absyn_AccessEff;
_temp740.f1=( void*) _temp681; _temp740;}); _temp739;}); _LL652: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp743=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp743->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp744=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp744[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp745; _temp745.tag= Cyc_Absyn_AccessEff;
_temp745.f1=( void*) _temp685; _temp745;}); _temp744;})); _temp743->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp687); _temp743;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp741=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp741[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp742; _temp742.tag= Cyc_Absyn_JoinEff; _temp742.f1= ts; _temp742;});
_temp741;}));} _LL654: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp746=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp746[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp747; _temp747.tag= Cyc_Absyn_JoinEff;
_temp747.f1=({ void* _temp748[ 2u]; _temp748[ 1u]= Cyc_Tcutil_rgns_of( _temp693);
_temp748[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp749=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp749[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp750; _temp750.tag= Cyc_Absyn_AccessEff;
_temp750.f1=( void*) _temp691; _temp750;}); _temp749;}); Cyc_List_list( _tag_arr(
_temp748, sizeof( void*), 2u));}); _temp747;}); _temp746;})); _LL656: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp695)); _LL658: { struct Cyc_List_List* _temp751= 0;
for( 0; _temp697 !=  0; _temp697=(( struct Cyc_List_List*) _check_null( _temp697))->tl){
_temp751=({ struct Cyc_List_List* _temp752=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp752->hd=( void*)(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp697))->hd)).f2; _temp752->tl= _temp751;
_temp752;});} _temp701= _temp751; goto _LL660;} _LL660: _temp703= _temp701; goto
_LL662; _LL662: _temp705= _temp703; goto _LL664; _LL664: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp753=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp753[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp754; _temp754.tag= Cyc_Absyn_JoinEff; _temp754.f1= Cyc_List_map( Cyc_Tcutil_rgns_of,
_temp705); _temp754;}); _temp753;})); _LL666: _temp709= _temp707; goto _LL668;
_LL668: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp755=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp755[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp756; _temp756.tag= Cyc_Absyn_JoinEff;
_temp756.f1=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_rgns_of_field, _temp709);
_temp756;}); _temp755;})); _LL670: { void* _temp757= Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst, _temp723),( void*)((
struct Cyc_Core_Opt*) _check_null( _temp721))->v); return Cyc_Tcutil_normalize_effect(
_temp757);} _LL672: return Cyc_Absyn_empty_effect; _LL674: goto _LL676; _LL676:
return t; _LL678: return Cyc_Tcutil_rgns_of( _temp725); _LL680: return({ void*
_temp758[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp758, sizeof( void*), 0u));});
_LL634:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp759= e; struct Cyc_List_List* _temp767; struct Cyc_List_List**
_temp769; void* _temp770; _LL761: if(( unsigned int) _temp759 >  4u?*(( int*)
_temp759) ==  Cyc_Absyn_JoinEff: 0){ _LL768: _temp767=(( struct Cyc_Absyn_JoinEff_struct*)
_temp759)->f1; _temp769=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp759)->f1; goto _LL762;} else{ goto _LL763;} _LL763: if(( unsigned int)
_temp759 >  4u?*(( int*) _temp759) ==  Cyc_Absyn_RgnsEff: 0){ _LL771: _temp770=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp759)->f1; goto _LL764;} else{
goto _LL765;} _LL765: goto _LL766; _LL762: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp769; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp772=( void*)(( struct Cyc_List_List*) _check_null( effs))->hd;(
void*)((( struct Cyc_List_List*) _check_null( effs))->hd=( void*) Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect( _temp772)));{ void* _temp773=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; _LL775: if(( unsigned int) _temp773 >  4u?*(( int*)
_temp773) ==  Cyc_Absyn_JoinEff: 0){ goto _LL776;} else{ goto _LL777;} _LL777:
goto _LL778; _LL776: nested_join= 1; goto _LL774; _LL778: goto _LL774; _LL774:;}}}
if( ! nested_join){ return e;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp769; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp779= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp787; void* _temp789; _LL781:
if(( unsigned int) _temp779 >  4u?*(( int*) _temp779) ==  Cyc_Absyn_JoinEff: 0){
_LL788: _temp787=(( struct Cyc_Absyn_JoinEff_struct*) _temp779)->f1; goto _LL782;}
else{ goto _LL783;} _LL783: if(( unsigned int) _temp779 >  4u?*(( int*) _temp779)
==  Cyc_Absyn_AccessEff: 0){ _LL790: _temp789=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp779)->f1; if( _temp789 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL784;} else{
goto _LL785;}} else{ goto _LL785;} _LL785: goto _LL786; _LL782: effects= Cyc_List_revappend(
_temp787, effects); goto _LL780; _LL784: goto _LL780; _LL786: effects=({ struct
Cyc_List_List* _temp791=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp791->hd=( void*) _temp779; _temp791->tl= effects; _temp791;}); goto _LL780;
_LL780:;}}* _temp769= Cyc_List_imp_rev( effects); return e;}} _LL764: return Cyc_Tcutil_rgns_of(
_temp770); _LL766: return e; _LL760:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp792= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp800; struct Cyc_List_List _temp802; struct Cyc_List_List* _temp803; void*
_temp805; struct Cyc_Core_Opt* _temp807; struct Cyc_Core_Opt* _temp809; _LL794:
if(( unsigned int) _temp792 >  4u?*(( int*) _temp792) ==  Cyc_Absyn_JoinEff: 0){
_LL801: _temp800=(( struct Cyc_Absyn_JoinEff_struct*) _temp792)->f1; if(
_temp800 ==  0){ goto _LL796;} else{ _temp802=* _temp800; _LL806: _temp805=(
void*) _temp802.hd; goto _LL804; _LL804: _temp803= _temp802.tl; goto _LL795;}}
else{ goto _LL796;} _LL796: if(( unsigned int) _temp792 >  4u?*(( int*) _temp792)
==  Cyc_Absyn_Evar: 0){ _LL810: _temp809=(( struct Cyc_Absyn_Evar_struct*)
_temp792)->f1; goto _LL808; _LL808: _temp807=(( struct Cyc_Absyn_Evar_struct*)
_temp792)->f4; goto _LL797;} else{ goto _LL798;} _LL798: goto _LL799; _LL795: {
void* _temp811= Cyc_Tcutil_compress( _temp805); struct Cyc_Core_Opt* _temp817;
_LL813: if(( unsigned int) _temp811 >  4u?*(( int*) _temp811) ==  Cyc_Absyn_Evar:
0){ _LL818: _temp817=(( struct Cyc_Absyn_Evar_struct*) _temp811)->f4; goto
_LL814;} else{ goto _LL815;} _LL815: goto _LL816; _LL814: return({ struct
_tuple9* _temp819=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp819->f1= _temp805; _temp819->f2= _temp803; _temp819->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp817))->v; _temp819;}); _LL816: return 0;
_LL812:;} _LL797: if( _temp809 ==  0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp809))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp820[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp820, sizeof( void*), 0u));});} return({ struct _tuple9* _temp821=( struct
_tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp821->f1= t; _temp821->f2= 0;
_temp821->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp807))->v; _temp821;}); _LL799: return 0; _LL793:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp822=({ struct Cyc_Absyn_FnType_struct*
_temp823=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp823[ 0]=({ struct Cyc_Absyn_FnType_struct _temp824; _temp824.tag= Cyc_Absyn_FnType;
_temp824.f1=({ struct Cyc_Absyn_FnInfo _temp825; _temp825.tvars= 0; _temp825.effect=({
struct Cyc_Core_Opt* _temp826=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp826->v=( void*) eff; _temp826;}); _temp825.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp825.args= 0; _temp825.c_varargs= 0; _temp825.cyc_varargs=
0; _temp825.rgn_po= 0; _temp825.attributes= 0; _temp825;}); _temp824;});
_temp823;}); return Cyc_Absyn_atb_typ(( void*) _temp822,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp827= Cyc_Tcutil_compress( e); void*
_temp839; struct Cyc_List_List* _temp841; void* _temp843; struct Cyc_Core_Opt*
_temp845; struct Cyc_Core_Opt* _temp847; struct Cyc_Core_Opt** _temp849; struct
Cyc_Core_Opt* _temp850; _LL829: if(( unsigned int) _temp827 >  4u?*(( int*)
_temp827) ==  Cyc_Absyn_AccessEff: 0){ _LL840: _temp839=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp827)->f1; goto _LL830;} else{ goto _LL831;} _LL831: if(( unsigned int)
_temp827 >  4u?*(( int*) _temp827) ==  Cyc_Absyn_JoinEff: 0){ _LL842: _temp841=((
struct Cyc_Absyn_JoinEff_struct*) _temp827)->f1; goto _LL832;} else{ goto _LL833;}
_LL833: if(( unsigned int) _temp827 >  4u?*(( int*) _temp827) ==  Cyc_Absyn_RgnsEff:
0){ _LL844: _temp843=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp827)->f1;
goto _LL834;} else{ goto _LL835;} _LL835: if(( unsigned int) _temp827 >  4u?*((
int*) _temp827) ==  Cyc_Absyn_Evar: 0){ _LL851: _temp850=(( struct Cyc_Absyn_Evar_struct*)
_temp827)->f1; goto _LL848; _LL848: _temp847=(( struct Cyc_Absyn_Evar_struct*)
_temp827)->f2; _temp849=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp827)->f2; goto _LL846; _LL846: _temp845=(( struct Cyc_Absyn_Evar_struct*)
_temp827)->f4; goto _LL836;} else{ goto _LL837;} _LL837: goto _LL838; _LL830:
if( constrain){ return Cyc_Tcutil_unify( r, _temp839);} _temp839= Cyc_Tcutil_compress(
_temp839); if( r ==  _temp839){ return 1;}{ struct _tuple8 _temp853=({ struct
_tuple8 _temp852; _temp852.f1= r; _temp852.f2= _temp839; _temp852;}); void*
_temp859; struct Cyc_Absyn_Tvar* _temp861; void* _temp863; struct Cyc_Absyn_Tvar*
_temp865; _LL855: _LL864: _temp863= _temp853.f1; if(( unsigned int) _temp863 > 
4u?*(( int*) _temp863) ==  Cyc_Absyn_VarType: 0){ _LL866: _temp865=(( struct Cyc_Absyn_VarType_struct*)
_temp863)->f1; goto _LL860;} else{ goto _LL857;} _LL860: _temp859= _temp853.f2;
if(( unsigned int) _temp859 >  4u?*(( int*) _temp859) ==  Cyc_Absyn_VarType: 0){
_LL862: _temp861=(( struct Cyc_Absyn_VarType_struct*) _temp859)->f1; goto _LL856;}
else{ goto _LL857;} _LL857: goto _LL858; _LL856: return Cyc_Absyn_tvar_cmp(
_temp865, _temp861) ==  0; _LL858: return 0; _LL854:;} _LL832: for( 0; _temp841
!=  0; _temp841=(( struct Cyc_List_List*) _check_null( _temp841))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp841))->hd)){
return 1;}} return 0; _LL834: { void* _temp867= Cyc_Tcutil_rgns_of( _temp843);
void* _temp873; _LL869: if(( unsigned int) _temp867 >  4u?*(( int*) _temp867) == 
Cyc_Absyn_RgnsEff: 0){ _LL874: _temp873=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp867)->f1; goto _LL870;} else{ goto _LL871;} _LL871: goto _LL872; _LL870:
if( ! constrain){ return 0;}{ void* _temp875= Cyc_Tcutil_compress( _temp873);
struct Cyc_Core_Opt* _temp881; struct Cyc_Core_Opt* _temp883; struct Cyc_Core_Opt**
_temp885; struct Cyc_Core_Opt* _temp886; _LL877: if(( unsigned int) _temp875 > 
4u?*(( int*) _temp875) ==  Cyc_Absyn_Evar: 0){ _LL887: _temp886=(( struct Cyc_Absyn_Evar_struct*)
_temp875)->f1; goto _LL884; _LL884: _temp883=(( struct Cyc_Absyn_Evar_struct*)
_temp875)->f2; _temp885=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp875)->f2; goto _LL882; _LL882: _temp881=(( struct Cyc_Absyn_Evar_struct*)
_temp875)->f4; goto _LL878;} else{ goto _LL879;} _LL879: goto _LL880; _LL878: {
void* _temp888= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp881); Cyc_Tcutil_occurs( _temp888,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp881))->v, r);{ void* _temp889= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp891=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp891[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp892; _temp892.tag= Cyc_Absyn_JoinEff; _temp892.f1=({ void* _temp893[ 2u];
_temp893[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp894=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp894[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp895; _temp895.tag= Cyc_Absyn_AccessEff;
_temp895.f1=( void*) r; _temp895;}); _temp894;}); _temp893[ 0u]= _temp888; Cyc_List_list(
_tag_arr( _temp893, sizeof( void*), 2u));}); _temp892;}); _temp891;}));*
_temp885=({ struct Cyc_Core_Opt* _temp890=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp890->v=( void*) _temp889; _temp890;});
return 1;}} _LL880: return 0; _LL876:;} _LL872: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp867); _LL868:;} _LL836: if( _temp850 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp850))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp896[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp896, sizeof( void*), 0u));});} if( ! constrain){ return 0;}{
void* _temp897= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp845); Cyc_Tcutil_occurs( _temp897,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp845))->v, r);{ struct Cyc_Absyn_JoinEff_struct* _temp898=({
struct Cyc_Absyn_JoinEff_struct* _temp900=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp900[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp901; _temp901.tag= Cyc_Absyn_JoinEff; _temp901.f1=({ struct Cyc_List_List*
_temp902=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp902->hd=( void*) _temp897; _temp902->tl=({ struct Cyc_List_List* _temp903=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp903->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp904=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp904[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp905; _temp905.tag= Cyc_Absyn_AccessEff; _temp905.f1=(
void*) r; _temp905;}); _temp904;})); _temp903->tl= 0; _temp903;}); _temp902;});
_temp901;}); _temp900;});* _temp849=({ struct Cyc_Core_Opt* _temp899=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp899->v=( void*)((
void*) _temp898); _temp899;}); return 1;}} _LL838: return 0; _LL828:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp906= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp918;
void* _temp920; struct Cyc_Core_Opt* _temp922; struct Cyc_Core_Opt* _temp924;
struct Cyc_Core_Opt** _temp926; struct Cyc_Core_Opt* _temp927; _LL908: if((
unsigned int) _temp906 >  4u?*(( int*) _temp906) ==  Cyc_Absyn_AccessEff: 0){
goto _LL909;} else{ goto _LL910;} _LL910: if(( unsigned int) _temp906 >  4u?*((
int*) _temp906) ==  Cyc_Absyn_JoinEff: 0){ _LL919: _temp918=(( struct Cyc_Absyn_JoinEff_struct*)
_temp906)->f1; goto _LL911;} else{ goto _LL912;} _LL912: if(( unsigned int)
_temp906 >  4u?*(( int*) _temp906) ==  Cyc_Absyn_RgnsEff: 0){ _LL921: _temp920=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp906)->f1; goto _LL913;} else{
goto _LL914;} _LL914: if(( unsigned int) _temp906 >  4u?*(( int*) _temp906) == 
Cyc_Absyn_Evar: 0){ _LL928: _temp927=(( struct Cyc_Absyn_Evar_struct*) _temp906)->f1;
goto _LL925; _LL925: _temp924=(( struct Cyc_Absyn_Evar_struct*) _temp906)->f2;
_temp926=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp906)->f2;
goto _LL923; _LL923: _temp922=(( struct Cyc_Absyn_Evar_struct*) _temp906)->f4;
goto _LL915;} else{ goto _LL916;} _LL916: goto _LL917; _LL909: return 0; _LL911:
for( 0; _temp918 !=  0; _temp918=(( struct Cyc_List_List*) _check_null( _temp918))->tl){
if( Cyc_Tcutil_type_in_effect( constrain, t,( void*)(( struct Cyc_List_List*)
_check_null( _temp918))->hd)){ return 1;}} return 0; _LL913: _temp920= Cyc_Tcutil_compress(
_temp920); if( t ==  _temp920){ return 1;} if( constrain){ return Cyc_Tcutil_unify(
t, _temp920);}{ void* _temp929= Cyc_Tcutil_rgns_of( t); void* _temp935; _LL931:
if(( unsigned int) _temp929 >  4u?*(( int*) _temp929) ==  Cyc_Absyn_RgnsEff: 0){
_LL936: _temp935=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp929)->f1;
goto _LL932;} else{ goto _LL933;} _LL933: goto _LL934; _LL932: { struct _tuple8
_temp938=({ struct _tuple8 _temp937; _temp937.f1= t; _temp937.f2= Cyc_Tcutil_compress(
_temp935); _temp937;}); void* _temp944; struct Cyc_Absyn_Tvar* _temp946; void*
_temp948; struct Cyc_Absyn_Tvar* _temp950; _LL940: _LL949: _temp948= _temp938.f1;
if(( unsigned int) _temp948 >  4u?*(( int*) _temp948) ==  Cyc_Absyn_VarType: 0){
_LL951: _temp950=(( struct Cyc_Absyn_VarType_struct*) _temp948)->f1; goto _LL945;}
else{ goto _LL942;} _LL945: _temp944= _temp938.f2; if(( unsigned int) _temp944 > 
4u?*(( int*) _temp944) ==  Cyc_Absyn_VarType: 0){ _LL947: _temp946=(( struct Cyc_Absyn_VarType_struct*)
_temp944)->f1; goto _LL941;} else{ goto _LL942;} _LL942: goto _LL943; _LL941:
return Cyc_Tcutil_unify( t, _temp935); _LL943: return t ==  _temp935; _LL939:;}
_LL934: return Cyc_Tcutil_type_in_effect( constrain, t, _temp929); _LL930:;}
_LL915: if( _temp927 ==  0? 1:( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp927))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp952[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp952, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp953= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp922); Cyc_Tcutil_occurs( _temp953,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp922))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp954=({ struct Cyc_Absyn_JoinEff_struct* _temp956=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp956[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp957; _temp957.tag= Cyc_Absyn_JoinEff; _temp957.f1=({ struct Cyc_List_List*
_temp958=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp958->hd=( void*) _temp953; _temp958->tl=({ struct Cyc_List_List* _temp959=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp959->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp960=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp960[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp961; _temp961.tag= Cyc_Absyn_RgnsEff; _temp961.f1=( void*) t; _temp961;});
_temp960;})); _temp959->tl= 0; _temp959;}); _temp958;}); _temp957;}); _temp956;});*
_temp926=({ struct Cyc_Core_Opt* _temp955=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp955->v=( void*)(( void*) _temp954); _temp955;});
return 1;}} _LL917: return 0; _LL907:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp962= e; struct Cyc_Absyn_Tvar* _temp974; struct Cyc_List_List*
_temp976; void* _temp978; struct Cyc_Core_Opt* _temp980; struct Cyc_Core_Opt*
_temp982; struct Cyc_Core_Opt** _temp984; struct Cyc_Core_Opt* _temp985; _LL964:
if(( unsigned int) _temp962 >  4u?*(( int*) _temp962) ==  Cyc_Absyn_VarType: 0){
_LL975: _temp974=(( struct Cyc_Absyn_VarType_struct*) _temp962)->f1; goto _LL965;}
else{ goto _LL966;} _LL966: if(( unsigned int) _temp962 >  4u?*(( int*) _temp962)
==  Cyc_Absyn_JoinEff: 0){ _LL977: _temp976=(( struct Cyc_Absyn_JoinEff_struct*)
_temp962)->f1; goto _LL967;} else{ goto _LL968;} _LL968: if(( unsigned int)
_temp962 >  4u?*(( int*) _temp962) ==  Cyc_Absyn_RgnsEff: 0){ _LL979: _temp978=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp962)->f1; goto _LL969;} else{
goto _LL970;} _LL970: if(( unsigned int) _temp962 >  4u?*(( int*) _temp962) == 
Cyc_Absyn_Evar: 0){ _LL986: _temp985=(( struct Cyc_Absyn_Evar_struct*) _temp962)->f1;
goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_Evar_struct*) _temp962)->f2;
_temp984=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp962)->f2;
goto _LL981; _LL981: _temp980=(( struct Cyc_Absyn_Evar_struct*) _temp962)->f4;
goto _LL971;} else{ goto _LL972;} _LL972: goto _LL973; _LL965: return Cyc_Absyn_tvar_cmp(
v, _temp974) ==  0; _LL967: for( 0; _temp976 !=  0; _temp976=(( struct Cyc_List_List*)
_check_null( _temp976))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp976))->hd)){ return 1;}}
return 0; _LL969: { void* _temp987= Cyc_Tcutil_rgns_of( _temp978); void*
_temp993; _LL989: if(( unsigned int) _temp987 >  4u?*(( int*) _temp987) ==  Cyc_Absyn_RgnsEff:
0){ _LL994: _temp993=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp987)->f1;
goto _LL990;} else{ goto _LL991;} _LL991: goto _LL992; _LL990: if( ! constrain){
return 0;}{ void* _temp995= Cyc_Tcutil_compress( _temp993); struct Cyc_Core_Opt*
_temp1001; struct Cyc_Core_Opt* _temp1003; struct Cyc_Core_Opt** _temp1005;
struct Cyc_Core_Opt* _temp1006; _LL997: if(( unsigned int) _temp995 >  4u?*((
int*) _temp995) ==  Cyc_Absyn_Evar: 0){ _LL1007: _temp1006=(( struct Cyc_Absyn_Evar_struct*)
_temp995)->f1; goto _LL1004; _LL1004: _temp1003=(( struct Cyc_Absyn_Evar_struct*)
_temp995)->f2; _temp1005=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp995)->f2; goto _LL1002; _LL1002: _temp1001=(( struct Cyc_Absyn_Evar_struct*)
_temp995)->f4; goto _LL998;} else{ goto _LL999;} _LL999: goto _LL1000; _LL998: {
void* _temp1008= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp1001); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1001))->v, v)){
return 0;}{ void* _temp1009= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1011=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1011[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1012; _temp1012.tag= Cyc_Absyn_JoinEff;
_temp1012.f1=({ void* _temp1013[ 2u]; _temp1013[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1014=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1014[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1015; _temp1015.tag= Cyc_Absyn_VarType;
_temp1015.f1= v; _temp1015;}); _temp1014;}); _temp1013[ 0u]= _temp1008; Cyc_List_list(
_tag_arr( _temp1013, sizeof( void*), 2u));}); _temp1012;}); _temp1011;}));*
_temp1005=({ struct Cyc_Core_Opt* _temp1010=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1010->v=( void*) _temp1009; _temp1010;});
return 1;}} _LL1000: return 0; _LL996:;} _LL992: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp987); _LL988:;} _LL971: if( _temp985 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp985))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp1016[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp1016, sizeof( void*), 0u));});}{ void* _temp1017= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp980); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp980))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp1018=({ struct Cyc_Absyn_JoinEff_struct* _temp1020=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1020[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1021; _temp1021.tag= Cyc_Absyn_JoinEff; _temp1021.f1=({
struct Cyc_List_List* _temp1022=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1022->hd=( void*) _temp1017; _temp1022->tl=({
struct Cyc_List_List* _temp1023=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1023->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1024=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1024[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1025; _temp1025.tag= Cyc_Absyn_VarType;
_temp1025.f1= v; _temp1025;}); _temp1024;})); _temp1023->tl= 0; _temp1023;});
_temp1022;}); _temp1021;}); _temp1020;});* _temp984=({ struct Cyc_Core_Opt*
_temp1019=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1019->v=( void*)(( void*) _temp1018); _temp1019;}); return 1;}} _LL973:
return 0; _LL963:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp1026= e; struct Cyc_List_List* _temp1036;
void* _temp1038; _LL1028: if(( unsigned int) _temp1026 >  4u?*(( int*) _temp1026)
==  Cyc_Absyn_JoinEff: 0){ _LL1037: _temp1036=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1026)->f1; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(( unsigned int)
_temp1026 >  4u?*(( int*) _temp1026) ==  Cyc_Absyn_RgnsEff: 0){ _LL1039:
_temp1038=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1026)->f1; goto
_LL1031;} else{ goto _LL1032;} _LL1032: if(( unsigned int) _temp1026 >  4u?*((
int*) _temp1026) ==  Cyc_Absyn_Evar: 0){ goto _LL1033;} else{ goto _LL1034;}
_LL1034: goto _LL1035; _LL1029: for( 0; _temp1036 !=  0; _temp1036=(( struct Cyc_List_List*)
_check_null( _temp1036))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp1036))->hd)){ return 1;}} return 0;
_LL1031: { void* _temp1040= Cyc_Tcutil_rgns_of( _temp1038); void* _temp1046;
_LL1042: if(( unsigned int) _temp1040 >  4u?*(( int*) _temp1040) ==  Cyc_Absyn_RgnsEff:
0){ _LL1047: _temp1046=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1040)->f1;
goto _LL1043;} else{ goto _LL1044;} _LL1044: goto _LL1045; _LL1043: return 0;
_LL1045: return Cyc_Tcutil_evar_in_effect( evar, _temp1040); _LL1041:;} _LL1033:
return evar ==  e; _LL1035: return 0; _LL1027:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1048= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1062; void* _temp1064; struct Cyc_Absyn_Tvar*
_temp1066; void* _temp1068; struct Cyc_Core_Opt* _temp1070; struct Cyc_Core_Opt*
_temp1072; struct Cyc_Core_Opt** _temp1074; _LL1050: if(( unsigned int)
_temp1048 >  4u?*(( int*) _temp1048) ==  Cyc_Absyn_JoinEff: 0){ _LL1063:
_temp1062=(( struct Cyc_Absyn_JoinEff_struct*) _temp1048)->f1; goto _LL1051;}
else{ goto _LL1052;} _LL1052: if(( unsigned int) _temp1048 >  4u?*(( int*)
_temp1048) ==  Cyc_Absyn_AccessEff: 0){ _LL1065: _temp1064=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1048)->f1; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(( unsigned int)
_temp1048 >  4u?*(( int*) _temp1048) ==  Cyc_Absyn_VarType: 0){ _LL1067:
_temp1066=(( struct Cyc_Absyn_VarType_struct*) _temp1048)->f1; goto _LL1055;}
else{ goto _LL1056;} _LL1056: if(( unsigned int) _temp1048 >  4u?*(( int*)
_temp1048) ==  Cyc_Absyn_RgnsEff: 0){ _LL1069: _temp1068=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1048)->f1; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(( unsigned int)
_temp1048 >  4u?*(( int*) _temp1048) ==  Cyc_Absyn_Evar: 0){ _LL1073: _temp1072=((
struct Cyc_Absyn_Evar_struct*) _temp1048)->f2; _temp1074=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1048)->f2; goto _LL1071; _LL1071: _temp1070=((
struct Cyc_Absyn_Evar_struct*) _temp1048)->f4; goto _LL1059;} else{ goto _LL1060;}
_LL1060: goto _LL1061; _LL1051: for( 0; _temp1062 !=  0; _temp1062=(( struct Cyc_List_List*)
_check_null( _temp1062))->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*)(( struct Cyc_List_List*) _check_null( _temp1062))->hd, e2)){ return 0;}}
return 1; _LL1053: return( Cyc_Tcutil_region_in_effect( 0, _temp1064, e2)? 1:
Cyc_Tcutil_region_in_effect( 1, _temp1064, e2))? 1: Cyc_Tcutil_unify( _temp1064,(
void*) Cyc_Absyn_HeapRgn); _LL1055: return Cyc_Tcutil_variable_in_effect( 0,
_temp1066, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1066, e2); _LL1057: {
void* _temp1075= Cyc_Tcutil_rgns_of( _temp1068); void* _temp1081; _LL1077: if((
unsigned int) _temp1075 >  4u?*(( int*) _temp1075) ==  Cyc_Absyn_RgnsEff: 0){
_LL1082: _temp1081=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1075)->f1;
goto _LL1078;} else{ goto _LL1079;} _LL1079: goto _LL1080; _LL1078: return( Cyc_Tcutil_type_in_effect(
0, _temp1081, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1081, e2))? 1: Cyc_Tcutil_unify(
_temp1081, Cyc_Absyn_sint_t); _LL1080: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1075, e2); _LL1076:;} _LL1059: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1074=({ struct Cyc_Core_Opt* _temp1083=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1083->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1084=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1084[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1085; _temp1085.tag= Cyc_Absyn_JoinEff; _temp1085.f1=
0; _temp1085;}); _temp1084;})); _temp1083;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1070))->v, e2);*
_temp1074=({ struct Cyc_Core_Opt* _temp1086=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1086->v=( void*) e2; _temp1086;});}} return
1; _LL1061: return({ struct Cyc_Std_String_pa_struct _temp1088; _temp1088.tag=
Cyc_Std_String_pa; _temp1088.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1087[ 1u]={& _temp1088};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1087, sizeof( void*), 1u));}});
_LL1049:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1090=({ struct
_tuple10 _temp1089; _temp1089.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1089.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1089;}); _LL1092: goto _LL1093; _LL1093:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1091:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8 _temp1096;
void* _temp1097; void* _temp1099; struct _tuple8* _temp1094=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( r1))->hd; _temp1096=* _temp1094; _LL1100:
_temp1099= _temp1096.f1; goto _LL1098; _LL1098: _temp1097= _temp1096.f2; goto
_LL1095; _LL1095: { int found= _temp1099 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2=(( struct Cyc_List_List*)
_check_null( r2))->tl){ struct _tuple8 _temp1103; void* _temp1104; void*
_temp1106; struct _tuple8* _temp1101=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( r2))->hd; _temp1103=* _temp1101; _LL1107: _temp1106= _temp1103.f1;
goto _LL1105; _LL1105: _temp1104= _temp1103.f2; goto _LL1102; _LL1102: if( Cyc_Tcutil_unify(
_temp1099, _temp1106)? Cyc_Tcutil_unify( _temp1097, _temp1104): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason=(
struct _tagged_arr) _tag_arr( 0u, 0u, 0u); t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2); if( t1 ==  t2){ return;}{ void* _temp1108= t1; struct Cyc_Core_Opt*
_temp1114; struct Cyc_Core_Opt* _temp1116; struct Cyc_Core_Opt** _temp1118;
struct Cyc_Core_Opt* _temp1119; _LL1110: if(( unsigned int) _temp1108 >  4u?*((
int*) _temp1108) ==  Cyc_Absyn_Evar: 0){ _LL1120: _temp1119=(( struct Cyc_Absyn_Evar_struct*)
_temp1108)->f1; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_Evar_struct*)
_temp1108)->f2; _temp1118=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1108)->f2; goto _LL1115; _LL1115: _temp1114=(( struct Cyc_Absyn_Evar_struct*)
_temp1108)->f4; goto _LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113;
_LL1111: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1114))->v, t2);{ void* _temp1121= Cyc_Tcutil_typ_kind( t2);
if( Cyc_Tcutil_kind_leq( _temp1121,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1119))->v)){* _temp1118=({ struct Cyc_Core_Opt* _temp1122=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1122->v=( void*) t2; _temp1122;});
return;} else{{ void* _temp1123= t2; struct Cyc_Core_Opt* _temp1131; struct Cyc_Core_Opt*
_temp1133; struct Cyc_Core_Opt** _temp1135; struct Cyc_Absyn_PtrInfo _temp1137;
_LL1125: if(( unsigned int) _temp1123 >  4u?*(( int*) _temp1123) ==  Cyc_Absyn_Evar:
0){ _LL1134: _temp1133=(( struct Cyc_Absyn_Evar_struct*) _temp1123)->f2;
_temp1135=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1123)->f2;
goto _LL1132; _LL1132: _temp1131=(( struct Cyc_Absyn_Evar_struct*) _temp1123)->f4;
goto _LL1126;} else{ goto _LL1127;} _LL1127: if(( unsigned int) _temp1123 >  4u?*((
int*) _temp1123) ==  Cyc_Absyn_PointerType: 0){ _LL1138: _temp1137=(( struct Cyc_Absyn_PointerType_struct*)
_temp1123)->f1; goto _LL1136;} else{ goto _LL1129;} _LL1136: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1119))->v == ( void*) Cyc_Absyn_BoxKind){
goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130; _LL1126: { struct Cyc_List_List*
_temp1139=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1114))->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1131))->v; for( 0; s2 !=  0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1139,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if(
Cyc_Tcutil_kind_leq(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1119))->v,
_temp1121)){* _temp1135=({ struct Cyc_Core_Opt* _temp1140=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1140->v=( void*) t1; _temp1140;});
return;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)", sizeof(
unsigned char), 25u); goto _LL1124;} _LL1128: { struct Cyc_Absyn_Conref*
_temp1141= Cyc_Absyn_compress_conref( _temp1137.bounds);{ void* _temp1142=( void*)
_temp1141->v; _LL1144: if( _temp1142 == ( void*) Cyc_Absyn_No_constr){ goto
_LL1145;} else{ goto _LL1146;} _LL1146: goto _LL1147; _LL1145:( void*)(
_temp1141->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1148=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1148[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp1149; _temp1149.tag= Cyc_Absyn_Eq_constr;
_temp1149.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1150=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1150[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1151; _temp1151.tag= Cyc_Absyn_Upper_b;
_temp1151.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1151;}); _temp1150;}));
_temp1149;}); _temp1148;})));* _temp1118=({ struct Cyc_Core_Opt* _temp1152=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1152->v=(
void*) t2; _temp1152;}); return; _LL1147: goto _LL1143; _LL1143:;} goto _LL1124;}
_LL1130: goto _LL1124; _LL1124:;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} _LL1113:
goto _LL1109; _LL1109:;}{ struct _tuple8 _temp1154=({ struct _tuple8 _temp1153;
_temp1153.f1= t2; _temp1153.f2= t1; _temp1153;}); void* _temp1208; void*
_temp1210; void* _temp1212; void* _temp1214; struct Cyc_Absyn_Tvar* _temp1216;
void* _temp1218; struct Cyc_Absyn_Tvar* _temp1220; void* _temp1222; struct Cyc_Absyn_Structdecl**
_temp1224; struct Cyc_List_List* _temp1226; struct _tuple1* _temp1228; void*
_temp1230; struct Cyc_Absyn_Structdecl** _temp1232; struct Cyc_List_List*
_temp1234; struct _tuple1* _temp1236; void* _temp1238; struct _tuple1* _temp1240;
void* _temp1242; struct _tuple1* _temp1244; void* _temp1246; struct Cyc_Absyn_Uniondecl**
_temp1248; struct Cyc_List_List* _temp1250; struct _tuple1* _temp1252; void*
_temp1254; struct Cyc_Absyn_Uniondecl** _temp1256; struct Cyc_List_List*
_temp1258; struct _tuple1* _temp1260; void* _temp1262; struct Cyc_Absyn_TunionInfo
_temp1264; void* _temp1266; struct Cyc_List_List* _temp1268; void* _temp1270;
struct Cyc_Absyn_Tuniondecl** _temp1272; struct Cyc_Absyn_Tuniondecl* _temp1274;
void* _temp1275; struct Cyc_Absyn_TunionInfo _temp1277; void* _temp1279; struct
Cyc_List_List* _temp1281; void* _temp1283; struct Cyc_Absyn_Tuniondecl**
_temp1285; struct Cyc_Absyn_Tuniondecl* _temp1287; void* _temp1288; struct Cyc_Absyn_TunionFieldInfo
_temp1290; struct Cyc_List_List* _temp1292; void* _temp1294; struct Cyc_Absyn_Tunionfield*
_temp1296; struct Cyc_Absyn_Tuniondecl* _temp1298; void* _temp1300; struct Cyc_Absyn_TunionFieldInfo
_temp1302; struct Cyc_List_List* _temp1304; void* _temp1306; struct Cyc_Absyn_Tunionfield*
_temp1308; struct Cyc_Absyn_Tuniondecl* _temp1310; void* _temp1312; struct Cyc_Absyn_PtrInfo
_temp1314; struct Cyc_Absyn_Conref* _temp1316; struct Cyc_Absyn_Tqual _temp1318;
struct Cyc_Absyn_Conref* _temp1320; void* _temp1322; void* _temp1324; void*
_temp1326; struct Cyc_Absyn_PtrInfo _temp1328; struct Cyc_Absyn_Conref*
_temp1330; struct Cyc_Absyn_Tqual _temp1332; struct Cyc_Absyn_Conref* _temp1334;
void* _temp1336; void* _temp1338; void* _temp1340; void* _temp1342; void*
_temp1344; void* _temp1346; void* _temp1348; void* _temp1350; void* _temp1352;
void* _temp1354; void* _temp1356; void* _temp1358; void* _temp1360; struct Cyc_Absyn_Exp*
_temp1362; struct Cyc_Absyn_Tqual _temp1364; void* _temp1366; void* _temp1368;
struct Cyc_Absyn_Exp* _temp1370; struct Cyc_Absyn_Tqual _temp1372; void*
_temp1374; void* _temp1376; struct Cyc_Absyn_FnInfo _temp1378; struct Cyc_List_List*
_temp1380; struct Cyc_List_List* _temp1382; struct Cyc_Absyn_VarargInfo*
_temp1384; int _temp1386; struct Cyc_List_List* _temp1388; void* _temp1390;
struct Cyc_Core_Opt* _temp1392; struct Cyc_List_List* _temp1394; void* _temp1396;
struct Cyc_Absyn_FnInfo _temp1398; struct Cyc_List_List* _temp1400; struct Cyc_List_List*
_temp1402; struct Cyc_Absyn_VarargInfo* _temp1404; int _temp1406; struct Cyc_List_List*
_temp1408; void* _temp1410; struct Cyc_Core_Opt* _temp1412; struct Cyc_List_List*
_temp1414; void* _temp1416; struct Cyc_List_List* _temp1418; void* _temp1420;
struct Cyc_List_List* _temp1422; void* _temp1424; struct Cyc_List_List*
_temp1426; void* _temp1428; struct Cyc_List_List* _temp1430; void* _temp1432;
struct Cyc_List_List* _temp1434; void* _temp1436; struct Cyc_List_List*
_temp1438; void* _temp1440; void* _temp1442; void* _temp1444; void* _temp1446;
void* _temp1448; void* _temp1450; void* _temp1452; void* _temp1454; void*
_temp1456; void* _temp1458; void* _temp1460; void* _temp1462; _LL1156: _LL1209:
_temp1208= _temp1154.f1; if(( unsigned int) _temp1208 >  4u?*(( int*) _temp1208)
==  Cyc_Absyn_Evar: 0){ goto _LL1157;} else{ goto _LL1158;} _LL1158: _LL1213:
_temp1212= _temp1154.f1; if( _temp1212 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1211;} else{ goto _LL1160;} _LL1211: _temp1210= _temp1154.f2; if( _temp1210
== ( void*) Cyc_Absyn_VoidType){ goto _LL1159;} else{ goto _LL1160;} _LL1160:
_LL1219: _temp1218= _temp1154.f1; if(( unsigned int) _temp1218 >  4u?*(( int*)
_temp1218) ==  Cyc_Absyn_VarType: 0){ _LL1221: _temp1220=(( struct Cyc_Absyn_VarType_struct*)
_temp1218)->f1; goto _LL1215;} else{ goto _LL1162;} _LL1215: _temp1214=
_temp1154.f2; if(( unsigned int) _temp1214 >  4u?*(( int*) _temp1214) ==  Cyc_Absyn_VarType:
0){ _LL1217: _temp1216=(( struct Cyc_Absyn_VarType_struct*) _temp1214)->f1; goto
_LL1161;} else{ goto _LL1162;} _LL1162: _LL1231: _temp1230= _temp1154.f1; if((
unsigned int) _temp1230 >  4u?*(( int*) _temp1230) ==  Cyc_Absyn_StructType: 0){
_LL1237: _temp1236=(( struct Cyc_Absyn_StructType_struct*) _temp1230)->f1; goto
_LL1235; _LL1235: _temp1234=(( struct Cyc_Absyn_StructType_struct*) _temp1230)->f2;
goto _LL1233; _LL1233: _temp1232=(( struct Cyc_Absyn_StructType_struct*)
_temp1230)->f3; goto _LL1223;} else{ goto _LL1164;} _LL1223: _temp1222=
_temp1154.f2; if(( unsigned int) _temp1222 >  4u?*(( int*) _temp1222) ==  Cyc_Absyn_StructType:
0){ _LL1229: _temp1228=(( struct Cyc_Absyn_StructType_struct*) _temp1222)->f1;
goto _LL1227; _LL1227: _temp1226=(( struct Cyc_Absyn_StructType_struct*)
_temp1222)->f2; goto _LL1225; _LL1225: _temp1224=(( struct Cyc_Absyn_StructType_struct*)
_temp1222)->f3; goto _LL1163;} else{ goto _LL1164;} _LL1164: _LL1243: _temp1242=
_temp1154.f1; if(( unsigned int) _temp1242 >  4u?*(( int*) _temp1242) ==  Cyc_Absyn_EnumType:
0){ _LL1245: _temp1244=(( struct Cyc_Absyn_EnumType_struct*) _temp1242)->f1;
goto _LL1239;} else{ goto _LL1166;} _LL1239: _temp1238= _temp1154.f2; if((
unsigned int) _temp1238 >  4u?*(( int*) _temp1238) ==  Cyc_Absyn_EnumType: 0){
_LL1241: _temp1240=(( struct Cyc_Absyn_EnumType_struct*) _temp1238)->f1; goto
_LL1165;} else{ goto _LL1166;} _LL1166: _LL1255: _temp1254= _temp1154.f1; if((
unsigned int) _temp1254 >  4u?*(( int*) _temp1254) ==  Cyc_Absyn_UnionType: 0){
_LL1261: _temp1260=(( struct Cyc_Absyn_UnionType_struct*) _temp1254)->f1; goto
_LL1259; _LL1259: _temp1258=(( struct Cyc_Absyn_UnionType_struct*) _temp1254)->f2;
goto _LL1257; _LL1257: _temp1256=(( struct Cyc_Absyn_UnionType_struct*)
_temp1254)->f3; goto _LL1247;} else{ goto _LL1168;} _LL1247: _temp1246=
_temp1154.f2; if(( unsigned int) _temp1246 >  4u?*(( int*) _temp1246) ==  Cyc_Absyn_UnionType:
0){ _LL1253: _temp1252=(( struct Cyc_Absyn_UnionType_struct*) _temp1246)->f1;
goto _LL1251; _LL1251: _temp1250=(( struct Cyc_Absyn_UnionType_struct*)
_temp1246)->f2; goto _LL1249; _LL1249: _temp1248=(( struct Cyc_Absyn_UnionType_struct*)
_temp1246)->f3; goto _LL1167;} else{ goto _LL1168;} _LL1168: _LL1276: _temp1275=
_temp1154.f1; if(( unsigned int) _temp1275 >  4u?*(( int*) _temp1275) ==  Cyc_Absyn_TunionType:
0){ _LL1278: _temp1277=(( struct Cyc_Absyn_TunionType_struct*) _temp1275)->f1;
_LL1284: _temp1283=( void*) _temp1277.tunion_info; if(*(( int*) _temp1283) == 
Cyc_Absyn_KnownTunion){ _LL1286: _temp1285=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1283)->f1; _temp1287=* _temp1285; goto _LL1282;} else{ goto _LL1170;}
_LL1282: _temp1281= _temp1277.targs; goto _LL1280; _LL1280: _temp1279=( void*)
_temp1277.rgn; goto _LL1263;} else{ goto _LL1170;} _LL1263: _temp1262= _temp1154.f2;
if(( unsigned int) _temp1262 >  4u?*(( int*) _temp1262) ==  Cyc_Absyn_TunionType:
0){ _LL1265: _temp1264=(( struct Cyc_Absyn_TunionType_struct*) _temp1262)->f1;
_LL1271: _temp1270=( void*) _temp1264.tunion_info; if(*(( int*) _temp1270) == 
Cyc_Absyn_KnownTunion){ _LL1273: _temp1272=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1270)->f1; _temp1274=* _temp1272; goto _LL1269;} else{ goto _LL1170;}
_LL1269: _temp1268= _temp1264.targs; goto _LL1267; _LL1267: _temp1266=( void*)
_temp1264.rgn; goto _LL1169;} else{ goto _LL1170;} _LL1170: _LL1301: _temp1300=
_temp1154.f1; if(( unsigned int) _temp1300 >  4u?*(( int*) _temp1300) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1303: _temp1302=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1300)->f1;
_LL1307: _temp1306=( void*) _temp1302.field_info; if(*(( int*) _temp1306) == 
Cyc_Absyn_KnownTunionfield){ _LL1311: _temp1310=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1306)->f1; goto _LL1309; _LL1309: _temp1308=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1306)->f2; goto _LL1305;} else{ goto _LL1172;} _LL1305: _temp1304=
_temp1302.targs; goto _LL1289;} else{ goto _LL1172;} _LL1289: _temp1288=
_temp1154.f2; if(( unsigned int) _temp1288 >  4u?*(( int*) _temp1288) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1291: _temp1290=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1288)->f1;
_LL1295: _temp1294=( void*) _temp1290.field_info; if(*(( int*) _temp1294) == 
Cyc_Absyn_KnownTunionfield){ _LL1299: _temp1298=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1294)->f1; goto _LL1297; _LL1297: _temp1296=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1294)->f2; goto _LL1293;} else{ goto _LL1172;} _LL1293: _temp1292=
_temp1290.targs; goto _LL1171;} else{ goto _LL1172;} _LL1172: _LL1327: _temp1326=
_temp1154.f1; if(( unsigned int) _temp1326 >  4u?*(( int*) _temp1326) ==  Cyc_Absyn_PointerType:
0){ _LL1329: _temp1328=(( struct Cyc_Absyn_PointerType_struct*) _temp1326)->f1;
_LL1339: _temp1338=( void*) _temp1328.elt_typ; goto _LL1337; _LL1337: _temp1336=(
void*) _temp1328.rgn_typ; goto _LL1335; _LL1335: _temp1334= _temp1328.nullable;
goto _LL1333; _LL1333: _temp1332= _temp1328.tq; goto _LL1331; _LL1331: _temp1330=
_temp1328.bounds; goto _LL1313;} else{ goto _LL1174;} _LL1313: _temp1312=
_temp1154.f2; if(( unsigned int) _temp1312 >  4u?*(( int*) _temp1312) ==  Cyc_Absyn_PointerType:
0){ _LL1315: _temp1314=(( struct Cyc_Absyn_PointerType_struct*) _temp1312)->f1;
_LL1325: _temp1324=( void*) _temp1314.elt_typ; goto _LL1323; _LL1323: _temp1322=(
void*) _temp1314.rgn_typ; goto _LL1321; _LL1321: _temp1320= _temp1314.nullable;
goto _LL1319; _LL1319: _temp1318= _temp1314.tq; goto _LL1317; _LL1317: _temp1316=
_temp1314.bounds; goto _LL1173;} else{ goto _LL1174;} _LL1174: _LL1347:
_temp1346= _temp1154.f1; if(( unsigned int) _temp1346 >  4u?*(( int*) _temp1346)
==  Cyc_Absyn_IntType: 0){ _LL1351: _temp1350=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1346)->f1; goto _LL1349; _LL1349: _temp1348=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1346)->f2; goto _LL1341;} else{ goto _LL1176;} _LL1341: _temp1340=
_temp1154.f2; if(( unsigned int) _temp1340 >  4u?*(( int*) _temp1340) ==  Cyc_Absyn_IntType:
0){ _LL1345: _temp1344=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1340)->f1;
goto _LL1343; _LL1343: _temp1342=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1340)->f2; goto _LL1175;} else{ goto _LL1176;} _LL1176: _LL1355: _temp1354=
_temp1154.f1; if( _temp1354 == ( void*) Cyc_Absyn_FloatType){ goto _LL1353;}
else{ goto _LL1178;} _LL1353: _temp1352= _temp1154.f2; if( _temp1352 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1177;} else{ goto _LL1178;} _LL1178: _LL1359:
_temp1358= _temp1154.f1; if( _temp1358 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1357;} else{ goto _LL1180;} _LL1357: _temp1356= _temp1154.f2; if( _temp1356
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1179;} else{ goto _LL1180;} _LL1180:
_LL1369: _temp1368= _temp1154.f1; if(( unsigned int) _temp1368 >  4u?*(( int*)
_temp1368) ==  Cyc_Absyn_ArrayType: 0){ _LL1375: _temp1374=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1368)->f1; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1368)->f2; goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1368)->f3; goto _LL1361;} else{ goto _LL1182;} _LL1361: _temp1360=
_temp1154.f2; if(( unsigned int) _temp1360 >  4u?*(( int*) _temp1360) ==  Cyc_Absyn_ArrayType:
0){ _LL1367: _temp1366=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1360)->f1;
goto _LL1365; _LL1365: _temp1364=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1360)->f2; goto _LL1363; _LL1363: _temp1362=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1360)->f3; goto _LL1181;} else{ goto _LL1182;} _LL1182: _LL1397: _temp1396=
_temp1154.f1; if(( unsigned int) _temp1396 >  4u?*(( int*) _temp1396) ==  Cyc_Absyn_FnType:
0){ _LL1399: _temp1398=(( struct Cyc_Absyn_FnType_struct*) _temp1396)->f1;
_LL1415: _temp1414= _temp1398.tvars; goto _LL1413; _LL1413: _temp1412= _temp1398.effect;
goto _LL1411; _LL1411: _temp1410=( void*) _temp1398.ret_typ; goto _LL1409;
_LL1409: _temp1408= _temp1398.args; goto _LL1407; _LL1407: _temp1406= _temp1398.c_varargs;
goto _LL1405; _LL1405: _temp1404= _temp1398.cyc_varargs; goto _LL1403; _LL1403:
_temp1402= _temp1398.rgn_po; goto _LL1401; _LL1401: _temp1400= _temp1398.attributes;
goto _LL1377;} else{ goto _LL1184;} _LL1377: _temp1376= _temp1154.f2; if((
unsigned int) _temp1376 >  4u?*(( int*) _temp1376) ==  Cyc_Absyn_FnType: 0){
_LL1379: _temp1378=(( struct Cyc_Absyn_FnType_struct*) _temp1376)->f1; _LL1395:
_temp1394= _temp1378.tvars; goto _LL1393; _LL1393: _temp1392= _temp1378.effect;
goto _LL1391; _LL1391: _temp1390=( void*) _temp1378.ret_typ; goto _LL1389;
_LL1389: _temp1388= _temp1378.args; goto _LL1387; _LL1387: _temp1386= _temp1378.c_varargs;
goto _LL1385; _LL1385: _temp1384= _temp1378.cyc_varargs; goto _LL1383; _LL1383:
_temp1382= _temp1378.rgn_po; goto _LL1381; _LL1381: _temp1380= _temp1378.attributes;
goto _LL1183;} else{ goto _LL1184;} _LL1184: _LL1421: _temp1420= _temp1154.f1;
if(( unsigned int) _temp1420 >  4u?*(( int*) _temp1420) ==  Cyc_Absyn_TupleType:
0){ _LL1423: _temp1422=(( struct Cyc_Absyn_TupleType_struct*) _temp1420)->f1;
goto _LL1417;} else{ goto _LL1186;} _LL1417: _temp1416= _temp1154.f2; if((
unsigned int) _temp1416 >  4u?*(( int*) _temp1416) ==  Cyc_Absyn_TupleType: 0){
_LL1419: _temp1418=(( struct Cyc_Absyn_TupleType_struct*) _temp1416)->f1; goto
_LL1185;} else{ goto _LL1186;} _LL1186: _LL1429: _temp1428= _temp1154.f1; if((
unsigned int) _temp1428 >  4u?*(( int*) _temp1428) ==  Cyc_Absyn_AnonStructType:
0){ _LL1431: _temp1430=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1428)->f1;
goto _LL1425;} else{ goto _LL1188;} _LL1425: _temp1424= _temp1154.f2; if((
unsigned int) _temp1424 >  4u?*(( int*) _temp1424) ==  Cyc_Absyn_AnonStructType:
0){ _LL1427: _temp1426=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1424)->f1;
goto _LL1187;} else{ goto _LL1188;} _LL1188: _LL1437: _temp1436= _temp1154.f1;
if(( unsigned int) _temp1436 >  4u?*(( int*) _temp1436) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1439: _temp1438=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1436)->f1;
goto _LL1433;} else{ goto _LL1190;} _LL1433: _temp1432= _temp1154.f2; if((
unsigned int) _temp1432 >  4u?*(( int*) _temp1432) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1435: _temp1434=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1432)->f1;
goto _LL1189;} else{ goto _LL1190;} _LL1190: _LL1443: _temp1442= _temp1154.f1;
if( _temp1442 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1441;} else{ goto _LL1192;}
_LL1441: _temp1440= _temp1154.f2; if( _temp1440 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1191;} else{ goto _LL1192;} _LL1192: _LL1449: _temp1448= _temp1154.f1;
if(( unsigned int) _temp1448 >  4u?*(( int*) _temp1448) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1451: _temp1450=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1448)->f1; goto _LL1445;} else{ goto _LL1194;} _LL1445: _temp1444=
_temp1154.f2; if(( unsigned int) _temp1444 >  4u?*(( int*) _temp1444) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1447: _temp1446=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1444)->f1; goto _LL1193;} else{ goto _LL1194;} _LL1194: _LL1453: _temp1452=
_temp1154.f1; if(( unsigned int) _temp1452 >  4u?*(( int*) _temp1452) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1195;} else{ goto _LL1196;} _LL1196: _LL1455: _temp1454= _temp1154.f2;
if(( unsigned int) _temp1454 >  4u?*(( int*) _temp1454) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1197;} else{ goto _LL1198;} _LL1198: _LL1457: _temp1456= _temp1154.f1;
if(( unsigned int) _temp1456 >  4u?*(( int*) _temp1456) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1199;} else{ goto _LL1200;} _LL1200: _LL1459: _temp1458= _temp1154.f1;
if(( unsigned int) _temp1458 >  4u?*(( int*) _temp1458) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1201;} else{ goto _LL1202;} _LL1202: _LL1461: _temp1460= _temp1154.f2;
if(( unsigned int) _temp1460 >  4u?*(( int*) _temp1460) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1203;} else{ goto _LL1204;} _LL1204: _LL1463: _temp1462= _temp1154.f2;
if(( unsigned int) _temp1462 >  4u?*(( int*) _temp1462) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1205;} else{ goto _LL1206;} _LL1206: goto _LL1207; _LL1157: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1159: return; _LL1161: { struct _tagged_arr* _temp1464=
_temp1220->name; struct _tagged_arr* _temp1465= _temp1216->name; int _temp1466=*((
int*) _check_null( _temp1220->identity)); int _temp1467=*(( int*) _check_null(
_temp1216->identity)); void* _temp1468= Cyc_Absyn_conref_val( _temp1220->kind);
void* _temp1469= Cyc_Absyn_conref_val( _temp1216->kind); if( _temp1467 == 
_temp1466? Cyc_Std_zstrptrcmp( _temp1464, _temp1465) ==  0: 0){ if( _temp1468 != 
_temp1469){({ struct Cyc_Std_String_pa_struct _temp1473; _temp1473.tag= Cyc_Std_String_pa;
_temp1473.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1469);{ struct
Cyc_Std_String_pa_struct _temp1472; _temp1472.tag= Cyc_Std_String_pa; _temp1472.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1468);{ struct Cyc_Std_String_pa_struct
_temp1471; _temp1471.tag= Cyc_Std_String_pa; _temp1471.f1=( struct _tagged_arr)*
_temp1464;{ void* _temp1470[ 3u]={& _temp1471,& _temp1472,& _temp1473};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1470, sizeof( void*), 3u));}}}});}
return;} Cyc_Tcutil_failure_reason= _tag_arr("(variable types are not the same)",
sizeof( unsigned char), 34u); goto _LL1155;} _LL1163: if((( _temp1228 !=  0?
_temp1236 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1228),( struct _tuple1*) _check_null( _temp1236)) ==  0: 0)? 1:( _temp1228
==  0? _temp1236 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1226, _temp1234);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different struct types)", sizeof(
unsigned char), 25u); goto _LL1155; _LL1165: if( Cyc_Absyn_qvar_cmp( _temp1244,
_temp1240) ==  0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different enum types)",
sizeof( unsigned char), 23u); goto _LL1155; _LL1167: if((( _temp1252 !=  0?
_temp1260 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1252),( struct _tuple1*) _check_null( _temp1260)) ==  0: 0)? 1:( _temp1252
==  0? _temp1260 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1250, _temp1258);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different union types)", sizeof(
unsigned char), 24u); goto _LL1155; _LL1169: if( _temp1287 ==  _temp1274? 1: Cyc_Absyn_qvar_cmp(
_temp1287->name, _temp1274->name) ==  0){ Cyc_Tcutil_unify_it( _temp1266,
_temp1279); Cyc_Tcutil_unify_list( _temp1268, _temp1281); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)", sizeof( unsigned char), 25u); goto _LL1155;
_LL1171: if(( _temp1310 ==  _temp1298? 1: Cyc_Absyn_qvar_cmp( _temp1310->name,
_temp1298->name) ==  0)? _temp1308 ==  _temp1296? 1: Cyc_Absyn_qvar_cmp(
_temp1308->name, _temp1296->name) ==  0: 0){ Cyc_Tcutil_unify_list( _temp1292,
_temp1304); return;} Cyc_Tcutil_failure_reason= _tag_arr("(different tunion field types)",
sizeof( unsigned char), 31u); goto _LL1155; _LL1173: Cyc_Tcutil_unify_it(
_temp1324, _temp1338); Cyc_Tcutil_unify_it( _temp1336, _temp1322); Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_unify_tqual( _temp1318, _temp1332);
Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp, _temp1316, _temp1330,
_tag_arr("(different pointer bounds)", sizeof( unsigned char), 27u));{ void*
_temp1474=( void*)( Cyc_Absyn_compress_conref( _temp1316))->v; void* _temp1480;
_LL1476: if(( unsigned int) _temp1474 >  1u?*(( int*) _temp1474) ==  Cyc_Absyn_Eq_constr:
0){ _LL1481: _temp1480=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1474)->f1;
if( _temp1480 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1477;} else{ goto
_LL1478;}} else{ goto _LL1478;} _LL1478: goto _LL1479; _LL1477: return; _LL1479:
goto _LL1475; _LL1475:;}(( void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y, struct _tagged_arr reason)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1320, _temp1334, _tag_arr("(different pointer types)",
sizeof( unsigned char), 26u)); return; _LL1175: if( _temp1344 ==  _temp1350?
_temp1342 ==  _temp1348: 0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different integral types)",
sizeof( unsigned char), 27u); goto _LL1155; _LL1177: return; _LL1179: return;
_LL1181: Cyc_Tcutil_unify_tqual( _temp1364, _temp1372); Cyc_Tcutil_unify_it(
_temp1366, _temp1374); if( _temp1370 ==  _temp1362){ return;} if( _temp1370 == 
0? 1: _temp1362 ==  0){ goto _LL1155;} if( Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1370)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1362))){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)", sizeof( unsigned char), 24u); goto _LL1155;
_LL1183: { int done= 0;{ struct _RegionHandle _temp1482= _new_region(); struct
_RegionHandle* rgn=& _temp1482; _push_region( rgn);{ struct Cyc_List_List* inst=
0; while( _temp1394 !=  0) { if( _temp1414 ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too few type variables)", sizeof(
unsigned char), 50u); break;} inst=({ struct Cyc_List_List* _temp1483=( struct
Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1483->hd=(
void*)({ struct _tuple5* _temp1484=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp1484->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1414))->hd; _temp1484->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1485=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1485[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1486; _temp1486.tag= Cyc_Absyn_VarType;
_temp1486.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1394))->hd; _temp1486;}); _temp1485;}); _temp1484;}); _temp1483->tl= inst;
_temp1483;}); _temp1394=(( struct Cyc_List_List*) _check_null( _temp1394))->tl;
_temp1414=(( struct Cyc_List_List*) _check_null( _temp1414))->tl;} if( _temp1414
!=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(second function type has too many type variables)",
sizeof( unsigned char), 51u); _npop_handler( 0u); goto _LL1155;} if( inst !=  0){
Cyc_Tcutil_unify_it(( void*)({ struct Cyc_Absyn_FnType_struct* _temp1487=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1487[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1488; _temp1488.tag= Cyc_Absyn_FnType;
_temp1488.f1=({ struct Cyc_Absyn_FnInfo _temp1489; _temp1489.tvars= 0; _temp1489.effect=
_temp1392; _temp1489.ret_typ=( void*) _temp1390; _temp1489.args= _temp1388;
_temp1489.c_varargs= _temp1386; _temp1489.cyc_varargs= _temp1384; _temp1489.rgn_po=
_temp1382; _temp1489.attributes= _temp1380; _temp1489;}); _temp1488;});
_temp1487;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1490=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1490[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1491; _temp1491.tag= Cyc_Absyn_FnType;
_temp1491.f1=({ struct Cyc_Absyn_FnInfo _temp1492; _temp1492.tvars= 0; _temp1492.effect=
_temp1412; _temp1492.ret_typ=( void*) _temp1410; _temp1492.args= _temp1408;
_temp1492.c_varargs= _temp1406; _temp1492.cyc_varargs= _temp1404; _temp1492.rgn_po=
_temp1402; _temp1492.attributes= _temp1400; _temp1492;}); _temp1491;});
_temp1490;}))); done= 1;}}; _pop_region( rgn);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp1390, _temp1410); for( 0; _temp1388 !=  0? _temp1408 !=  0: 0;( _temp1388=((
struct Cyc_List_List*) _check_null( _temp1388))->tl, _temp1408=(( struct Cyc_List_List*)
_check_null( _temp1408))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1388))->hd)).f2,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1408))->hd)).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1388))->hd)).f3,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1408))->hd)).f3);}
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; if( _temp1388 !=  0? 1:
_temp1408 !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different number of arguments)",
sizeof( unsigned char), 52u); goto _LL1155;} if( _temp1386 !=  _temp1406){ Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)", sizeof( unsigned char), 41u);
goto _LL1155;}{ int bad_cyc_vararg= 0;{ struct _tuple11 _temp1494=({ struct
_tuple11 _temp1493; _temp1493.f1= _temp1384; _temp1493.f2= _temp1404; _temp1493;});
struct Cyc_Absyn_VarargInfo* _temp1504; struct Cyc_Absyn_VarargInfo* _temp1506;
struct Cyc_Absyn_VarargInfo* _temp1508; struct Cyc_Absyn_VarargInfo* _temp1510;
struct Cyc_Absyn_VarargInfo* _temp1512; struct Cyc_Absyn_VarargInfo _temp1514;
int _temp1515; void* _temp1517; struct Cyc_Absyn_Tqual _temp1519; struct Cyc_Core_Opt*
_temp1521; struct Cyc_Absyn_VarargInfo* _temp1523; struct Cyc_Absyn_VarargInfo
_temp1525; int _temp1526; void* _temp1528; struct Cyc_Absyn_Tqual _temp1530;
struct Cyc_Core_Opt* _temp1532; _LL1496: _LL1507: _temp1506= _temp1494.f1; if(
_temp1506 ==  0){ goto _LL1505;} else{ goto _LL1498;} _LL1505: _temp1504=
_temp1494.f2; if( _temp1504 ==  0){ goto _LL1497;} else{ goto _LL1498;} _LL1498:
_LL1509: _temp1508= _temp1494.f1; if( _temp1508 ==  0){ goto _LL1499;} else{
goto _LL1500;} _LL1500: _LL1511: _temp1510= _temp1494.f2; if( _temp1510 ==  0){
goto _LL1501;} else{ goto _LL1502;} _LL1502: _LL1524: _temp1523= _temp1494.f1;
if( _temp1523 ==  0){ goto _LL1495;} else{ _temp1525=* _temp1523; _LL1533:
_temp1532= _temp1525.name; goto _LL1531; _LL1531: _temp1530= _temp1525.tq; goto
_LL1529; _LL1529: _temp1528=( void*) _temp1525.type; goto _LL1527; _LL1527:
_temp1526= _temp1525.inject; goto _LL1513;} _LL1513: _temp1512= _temp1494.f2;
if( _temp1512 ==  0){ goto _LL1495;} else{ _temp1514=* _temp1512; _LL1522:
_temp1521= _temp1514.name; goto _LL1520; _LL1520: _temp1519= _temp1514.tq; goto
_LL1518; _LL1518: _temp1517=( void*) _temp1514.type; goto _LL1516; _LL1516:
_temp1515= _temp1514.inject; goto _LL1503;} _LL1497: goto _LL1495; _LL1499: goto
_LL1501; _LL1501: bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type takes varargs)",
sizeof( unsigned char), 39u); goto _LL1495; _LL1503: Cyc_Tcutil_unify_tqual(
_temp1530, _temp1519); Cyc_Tcutil_unify_it( _temp1528, _temp1517); if( _temp1526
!=  _temp1515){ bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type injects varargs)",
sizeof( unsigned char), 41u);} goto _LL1495; _LL1495:;} if( bad_cyc_vararg){
goto _LL1155;}{ int bad_effect= 0;{ struct _tuple12 _temp1535=({ struct _tuple12
_temp1534; _temp1534.f1= _temp1392; _temp1534.f2= _temp1412; _temp1534;});
struct Cyc_Core_Opt* _temp1545; struct Cyc_Core_Opt* _temp1547; struct Cyc_Core_Opt*
_temp1549; struct Cyc_Core_Opt* _temp1551; _LL1537: _LL1548: _temp1547=
_temp1535.f1; if( _temp1547 ==  0){ goto _LL1546;} else{ goto _LL1539;} _LL1546:
_temp1545= _temp1535.f2; if( _temp1545 ==  0){ goto _LL1538;} else{ goto _LL1539;}
_LL1539: _LL1550: _temp1549= _temp1535.f1; if( _temp1549 ==  0){ goto _LL1540;}
else{ goto _LL1541;} _LL1541: _LL1552: _temp1551= _temp1535.f2; if( _temp1551 == 
0){ goto _LL1542;} else{ goto _LL1543;} _LL1543: goto _LL1544; _LL1538: goto
_LL1536; _LL1540: goto _LL1542; _LL1542: bad_effect= 1; goto _LL1536; _LL1544:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1392))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1412))->v); goto _LL1536; _LL1536:;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; if( bad_effect){ Cyc_Tcutil_failure_reason= _tag_arr("(function type effects do not unify)",
sizeof( unsigned char), 37u); goto _LL1155;} if( ! Cyc_Tcutil_same_atts(
_temp1400, _temp1380)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different attributes)",
sizeof( unsigned char), 43u); goto _LL1155;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1402, _temp1382)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different region lifetime orderings)",
sizeof( unsigned char), 58u); goto _LL1155;} return;}}} _LL1185: for( 0;
_temp1418 !=  0? _temp1422 !=  0: 0;( _temp1418=(( struct Cyc_List_List*)
_check_null( _temp1418))->tl, _temp1422=(( struct Cyc_List_List*) _check_null(
_temp1422))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1418))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1422))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1418))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1422))->hd)).f2);} if( _temp1418 ==  0?
_temp1422 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(tuple types have different numbers of components)",
sizeof( unsigned char), 51u); goto _LL1155; _LL1187: _temp1438= _temp1430;
_temp1434= _temp1426; goto _LL1189; _LL1189: for( 0; _temp1434 !=  0? _temp1438
!=  0: 0;( _temp1434=(( struct Cyc_List_List*) _check_null( _temp1434))->tl,
_temp1438=(( struct Cyc_List_List*) _check_null( _temp1438))->tl)){ struct Cyc_Absyn_Structfield*
_temp1553=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1434))->hd; struct Cyc_Absyn_Structfield* _temp1554=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1438))->hd; if( Cyc_Std_zstrptrcmp(
_temp1553->name, _temp1554->name) !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(different member names in anonymous structs/unions)",
sizeof( unsigned char), 53u);( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1553->tq, _temp1554->tq); Cyc_Tcutil_unify_it(( void*) _temp1553->type,(
void*) _temp1554->type); if( ! Cyc_Tcutil_same_atts( _temp1553->attributes,
_temp1554->attributes)){ Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2;
Cyc_Tcutil_failure_reason= _tag_arr("(different attributes on member)", sizeof(
unsigned char), 33u);( int) _throw(( void*) Cyc_Tcutil_Unify);} if((( _temp1553->width
!=  0? _temp1554->width ==  0: 0)? 1:( _temp1554->width !=  0? _temp1553->width
==  0: 0))? 1:(( _temp1553->width !=  0? _temp1554->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1553->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1554->width)): 0)){ Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason= _tag_arr("(different bitfield widths on member)",
sizeof( unsigned char), 38u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} if(
_temp1434 ==  0? _temp1438 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(different number of members)", sizeof(
unsigned char), 30u); goto _LL1155; _LL1191: return; _LL1193: Cyc_Tcutil_unify_it(
_temp1450, _temp1446); return; _LL1195: goto _LL1197; _LL1197: goto _LL1199;
_LL1199: goto _LL1201; _LL1201: goto _LL1203; _LL1203: goto _LL1205; _LL1205:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)", sizeof( unsigned char), 22u); goto _LL1155;
_LL1207: goto _LL1155; _LL1155:;}( int) _throw(( void*) Cyc_Tcutil_Unify);} int
Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1555= Cyc_Tcutil_compress( t);
_LL1557: if(( unsigned int) _temp1555 >  4u?*(( int*) _temp1555) ==  Cyc_Absyn_IntType:
0){ goto _LL1558;} else{ goto _LL1559;} _LL1559: if( _temp1555 == ( void*) Cyc_Absyn_FloatType){
goto _LL1560;} else{ goto _LL1561;} _LL1561: if( _temp1555 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1562;} else{ goto _LL1563;} _LL1563: if(( unsigned int) _temp1555 >  4u?*((
int*) _temp1555) ==  Cyc_Absyn_EnumType: 0){ goto _LL1564;} else{ goto _LL1565;}
_LL1565: goto _LL1566; _LL1558: goto _LL1560; _LL1560: goto _LL1562; _LL1562:
goto _LL1564; _LL1564: return 1; _LL1566: return 0; _LL1556:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1568=({ struct _tuple8 _temp1567; _temp1567.f1= t1;
_temp1567.f2= t2; _temp1567;}); void* _temp1590; void* _temp1592; void*
_temp1594; void* _temp1596; void* _temp1598; void* _temp1600; void* _temp1602;
void* _temp1604; void* _temp1606; void* _temp1608; void* _temp1610; void*
_temp1612; void* _temp1614; void* _temp1616; void* _temp1618; void* _temp1620;
void* _temp1622; void* _temp1624; void* _temp1626; void* _temp1628; void*
_temp1630; void* _temp1632; void* _temp1634; void* _temp1636; void* _temp1638;
void* _temp1640; void* _temp1642; _LL1570: _LL1593: _temp1592= _temp1568.f1; if(
_temp1592 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1591;} else{ goto _LL1572;}
_LL1591: _temp1590= _temp1568.f2; if( _temp1590 == ( void*) Cyc_Absyn_FloatType){
goto _LL1571;} else{ goto _LL1572;} _LL1572: _LL1597: _temp1596= _temp1568.f1;
if( _temp1596 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1595;} else{ goto
_LL1574;} _LL1595: _temp1594= _temp1568.f2; if(( unsigned int) _temp1594 >  4u?*((
int*) _temp1594) ==  Cyc_Absyn_IntType: 0){ goto _LL1573;} else{ goto _LL1574;}
_LL1574: _LL1601: _temp1600= _temp1568.f1; if( _temp1600 == ( void*) Cyc_Absyn_FloatType){
goto _LL1599;} else{ goto _LL1576;} _LL1599: _temp1598= _temp1568.f2; if((
unsigned int) _temp1598 >  4u?*(( int*) _temp1598) ==  Cyc_Absyn_IntType: 0){
goto _LL1575;} else{ goto _LL1576;} _LL1576: _LL1607: _temp1606= _temp1568.f1;
if(( unsigned int) _temp1606 >  4u?*(( int*) _temp1606) ==  Cyc_Absyn_IntType: 0){
_LL1609: _temp1608=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1606)->f2;
if( _temp1608 == ( void*) Cyc_Absyn_B8){ goto _LL1603;} else{ goto _LL1578;}}
else{ goto _LL1578;} _LL1603: _temp1602= _temp1568.f2; if(( unsigned int)
_temp1602 >  4u?*(( int*) _temp1602) ==  Cyc_Absyn_IntType: 0){ _LL1605:
_temp1604=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1602)->f2; if(
_temp1604 == ( void*) Cyc_Absyn_B8){ goto _LL1577;} else{ goto _LL1578;}} else{
goto _LL1578;} _LL1578: _LL1611: _temp1610= _temp1568.f1; if(( unsigned int)
_temp1610 >  4u?*(( int*) _temp1610) ==  Cyc_Absyn_IntType: 0){ _LL1613:
_temp1612=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1610)->f2; if(
_temp1612 == ( void*) Cyc_Absyn_B8){ goto _LL1579;} else{ goto _LL1580;}} else{
goto _LL1580;} _LL1580: _LL1617: _temp1616= _temp1568.f1; if(( unsigned int)
_temp1616 >  4u?*(( int*) _temp1616) ==  Cyc_Absyn_IntType: 0){ _LL1619:
_temp1618=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1616)->f2; if(
_temp1618 == ( void*) Cyc_Absyn_B4){ goto _LL1615;} else{ goto _LL1582;}} else{
goto _LL1582;} _LL1615: _temp1614= _temp1568.f2; if( _temp1614 == ( void*) Cyc_Absyn_FloatType){
goto _LL1581;} else{ goto _LL1582;} _LL1582: _LL1625: _temp1624= _temp1568.f1;
if(( unsigned int) _temp1624 >  4u?*(( int*) _temp1624) ==  Cyc_Absyn_IntType: 0){
_LL1627: _temp1626=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1624)->f2;
if( _temp1626 == ( void*) Cyc_Absyn_B4){ goto _LL1621;} else{ goto _LL1584;}}
else{ goto _LL1584;} _LL1621: _temp1620= _temp1568.f2; if(( unsigned int)
_temp1620 >  4u?*(( int*) _temp1620) ==  Cyc_Absyn_IntType: 0){ _LL1623:
_temp1622=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1620)->f2; if(
_temp1622 == ( void*) Cyc_Absyn_B2){ goto _LL1583;} else{ goto _LL1584;}} else{
goto _LL1584;} _LL1584: _LL1633: _temp1632= _temp1568.f1; if(( unsigned int)
_temp1632 >  4u?*(( int*) _temp1632) ==  Cyc_Absyn_IntType: 0){ _LL1635:
_temp1634=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1632)->f2; if(
_temp1634 == ( void*) Cyc_Absyn_B4){ goto _LL1629;} else{ goto _LL1586;}} else{
goto _LL1586;} _LL1629: _temp1628= _temp1568.f2; if(( unsigned int) _temp1628 > 
4u?*(( int*) _temp1628) ==  Cyc_Absyn_IntType: 0){ _LL1631: _temp1630=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1628)->f2; if( _temp1630 == ( void*) Cyc_Absyn_B1){
goto _LL1585;} else{ goto _LL1586;}} else{ goto _LL1586;} _LL1586: _LL1641:
_temp1640= _temp1568.f1; if(( unsigned int) _temp1640 >  4u?*(( int*) _temp1640)
==  Cyc_Absyn_IntType: 0){ _LL1643: _temp1642=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1640)->f2; if( _temp1642 == ( void*) Cyc_Absyn_B2){ goto _LL1637;} else{
goto _LL1588;}} else{ goto _LL1588;} _LL1637: _temp1636= _temp1568.f2; if((
unsigned int) _temp1636 >  4u?*(( int*) _temp1636) ==  Cyc_Absyn_IntType: 0){
_LL1639: _temp1638=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1636)->f2;
if( _temp1638 == ( void*) Cyc_Absyn_B1){ goto _LL1587;} else{ goto _LL1588;}}
else{ goto _LL1588;} _LL1588: goto _LL1589; _LL1571: goto _LL1573; _LL1573: goto
_LL1575; _LL1575: return 1; _LL1577: return 0; _LL1579: goto _LL1581; _LL1581:
goto _LL1583; _LL1583: goto _LL1585; _LL1585: goto _LL1587; _LL1587: return 1;
_LL1589: return 0; _LL1569:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1644=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1644->v=( void*)
t1; _temp1644;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify( t,(
void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp1647; _temp1647.tag= Cyc_Std_String_pa; _temp1647.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1646; _temp1646.tag= Cyc_Std_String_pa;
_temp1646.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1645[
2u]={& _temp1646,& _temp1647}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1645, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1648= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1650: if(( unsigned int) _temp1648 >  4u?*(( int*) _temp1648) ==  Cyc_Absyn_PointerType:
0){ goto _LL1651;} else{ goto _LL1652;} _LL1652: goto _LL1653; _LL1651: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1649; _LL1653: return 0; _LL1649:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1654= Cyc_Tcutil_compress(
t); _LL1656: if(( unsigned int) _temp1654 >  4u?*(( int*) _temp1654) ==  Cyc_Absyn_IntType:
0){ goto _LL1657;} else{ goto _LL1658;} _LL1658: if(( unsigned int) _temp1654 > 
4u?*(( int*) _temp1654) ==  Cyc_Absyn_EnumType: 0){ goto _LL1659;} else{ goto
_LL1660;} _LL1660: goto _LL1661; _LL1657: goto _LL1659; _LL1659: return 1;
_LL1661: return 0; _LL1655:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1662[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1662, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1663[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1663, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1665=({ struct _tuple8 _temp1664; _temp1664.f1= t1; _temp1664.f2= t2;
_temp1664;}); void* _temp1677; struct Cyc_Absyn_PtrInfo _temp1679; void*
_temp1681; struct Cyc_Absyn_PtrInfo _temp1683; void* _temp1685; struct Cyc_Absyn_Exp*
_temp1687; struct Cyc_Absyn_Tqual _temp1689; void* _temp1691; void* _temp1693;
struct Cyc_Absyn_Exp* _temp1695; struct Cyc_Absyn_Tqual _temp1697; void*
_temp1699; void* _temp1701; struct Cyc_Absyn_TunionInfo _temp1703; struct Cyc_List_List*
_temp1705; void* _temp1707; struct Cyc_Absyn_Tuniondecl** _temp1709; struct Cyc_Absyn_Tuniondecl*
_temp1711; void* _temp1712; struct Cyc_Absyn_TunionFieldInfo _temp1714; struct
Cyc_List_List* _temp1716; void* _temp1718; struct Cyc_Absyn_Tunionfield*
_temp1720; struct Cyc_Absyn_Tuniondecl* _temp1722; void* _temp1724; struct Cyc_Absyn_TunionInfo
_temp1726; void* _temp1728; struct Cyc_List_List* _temp1730; void* _temp1732;
struct Cyc_Absyn_Tuniondecl** _temp1734; struct Cyc_Absyn_Tuniondecl* _temp1736;
void* _temp1737; struct Cyc_Absyn_PtrInfo _temp1739; struct Cyc_Absyn_Conref*
_temp1741; struct Cyc_Absyn_Tqual _temp1743; struct Cyc_Absyn_Conref* _temp1745;
void* _temp1747; void* _temp1749; _LL1667: _LL1682: _temp1681= _temp1665.f1; if((
unsigned int) _temp1681 >  4u?*(( int*) _temp1681) ==  Cyc_Absyn_PointerType: 0){
_LL1684: _temp1683=(( struct Cyc_Absyn_PointerType_struct*) _temp1681)->f1; goto
_LL1678;} else{ goto _LL1669;} _LL1678: _temp1677= _temp1665.f2; if((
unsigned int) _temp1677 >  4u?*(( int*) _temp1677) ==  Cyc_Absyn_PointerType: 0){
_LL1680: _temp1679=(( struct Cyc_Absyn_PointerType_struct*) _temp1677)->f1; goto
_LL1668;} else{ goto _LL1669;} _LL1669: _LL1694: _temp1693= _temp1665.f1; if((
unsigned int) _temp1693 >  4u?*(( int*) _temp1693) ==  Cyc_Absyn_ArrayType: 0){
_LL1700: _temp1699=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1693)->f1;
goto _LL1698; _LL1698: _temp1697=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1693)->f2; goto _LL1696; _LL1696: _temp1695=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1693)->f3; goto _LL1686;} else{ goto _LL1671;} _LL1686: _temp1685=
_temp1665.f2; if(( unsigned int) _temp1685 >  4u?*(( int*) _temp1685) ==  Cyc_Absyn_ArrayType:
0){ _LL1692: _temp1691=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1685)->f1;
goto _LL1690; _LL1690: _temp1689=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1685)->f2; goto _LL1688; _LL1688: _temp1687=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1685)->f3; goto _LL1670;} else{ goto _LL1671;} _LL1671: _LL1713: _temp1712=
_temp1665.f1; if(( unsigned int) _temp1712 >  4u?*(( int*) _temp1712) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1715: _temp1714=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1712)->f1;
_LL1719: _temp1718=( void*) _temp1714.field_info; if(*(( int*) _temp1718) == 
Cyc_Absyn_KnownTunionfield){ _LL1723: _temp1722=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1718)->f1; goto _LL1721; _LL1721: _temp1720=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1718)->f2; goto _LL1717;} else{ goto _LL1673;} _LL1717: _temp1716=
_temp1714.targs; goto _LL1702;} else{ goto _LL1673;} _LL1702: _temp1701=
_temp1665.f2; if(( unsigned int) _temp1701 >  4u?*(( int*) _temp1701) ==  Cyc_Absyn_TunionType:
0){ _LL1704: _temp1703=(( struct Cyc_Absyn_TunionType_struct*) _temp1701)->f1;
_LL1708: _temp1707=( void*) _temp1703.tunion_info; if(*(( int*) _temp1707) == 
Cyc_Absyn_KnownTunion){ _LL1710: _temp1709=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1707)->f1; _temp1711=* _temp1709; goto _LL1706;} else{ goto _LL1673;}
_LL1706: _temp1705= _temp1703.targs; goto _LL1672;} else{ goto _LL1673;} _LL1673:
_LL1738: _temp1737= _temp1665.f1; if(( unsigned int) _temp1737 >  4u?*(( int*)
_temp1737) ==  Cyc_Absyn_PointerType: 0){ _LL1740: _temp1739=(( struct Cyc_Absyn_PointerType_struct*)
_temp1737)->f1; _LL1750: _temp1749=( void*) _temp1739.elt_typ; goto _LL1748;
_LL1748: _temp1747=( void*) _temp1739.rgn_typ; goto _LL1746; _LL1746: _temp1745=
_temp1739.nullable; goto _LL1744; _LL1744: _temp1743= _temp1739.tq; goto _LL1742;
_LL1742: _temp1741= _temp1739.bounds; goto _LL1725;} else{ goto _LL1675;}
_LL1725: _temp1724= _temp1665.f2; if(( unsigned int) _temp1724 >  4u?*(( int*)
_temp1724) ==  Cyc_Absyn_TunionType: 0){ _LL1727: _temp1726=(( struct Cyc_Absyn_TunionType_struct*)
_temp1724)->f1; _LL1733: _temp1732=( void*) _temp1726.tunion_info; if(*(( int*)
_temp1732) ==  Cyc_Absyn_KnownTunion){ _LL1735: _temp1734=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1732)->f1; _temp1736=* _temp1734; goto _LL1731;} else{ goto _LL1675;}
_LL1731: _temp1730= _temp1726.targs; goto _LL1729; _LL1729: _temp1728=( void*)
_temp1726.rgn; goto _LL1674;} else{ goto _LL1675;} _LL1675: goto _LL1676;
_LL1668: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1683.nullable, _temp1679.nullable)){ void* _temp1751=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1683.nullable))->v;
int _temp1757; _LL1753: if(( unsigned int) _temp1751 >  1u?*(( int*) _temp1751)
==  Cyc_Absyn_Eq_constr: 0){ _LL1758: _temp1757=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1751)->f1; goto _LL1754;} else{ goto _LL1755;} _LL1755: goto _LL1756;
_LL1754: okay= ! _temp1757; goto _LL1752; _LL1756:( int) _throw(({ void*
_temp1759[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp1759, sizeof( void*), 0u));}));
_LL1752:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1683.bounds,
_temp1679.bounds)){ struct _tuple8 _temp1761=({ struct _tuple8 _temp1760;
_temp1760.f1=( void*)( Cyc_Absyn_compress_conref( _temp1683.bounds))->v;
_temp1760.f2=( void*)( Cyc_Absyn_compress_conref( _temp1679.bounds))->v;
_temp1760;}); void* _temp1771; void* _temp1773; void* _temp1775; void* _temp1777;
void* _temp1779; void* _temp1781; struct Cyc_Absyn_Exp* _temp1783; void*
_temp1785; void* _temp1787; struct Cyc_Absyn_Exp* _temp1789; void* _temp1791;
void* _temp1793; void* _temp1795; void* _temp1797; _LL1763: _LL1776: _temp1775=
_temp1761.f1; if(( unsigned int) _temp1775 >  1u?*(( int*) _temp1775) ==  Cyc_Absyn_Eq_constr:
0){ _LL1778: _temp1777=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1775)->f1;
if(( unsigned int) _temp1777 >  1u?*(( int*) _temp1777) ==  Cyc_Absyn_Upper_b: 0){
goto _LL1772;} else{ goto _LL1765;}} else{ goto _LL1765;} _LL1772: _temp1771=
_temp1761.f2; if(( unsigned int) _temp1771 >  1u?*(( int*) _temp1771) ==  Cyc_Absyn_Eq_constr:
0){ _LL1774: _temp1773=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1771)->f1;
if( _temp1773 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1764;} else{ goto
_LL1765;}} else{ goto _LL1765;} _LL1765: _LL1786: _temp1785= _temp1761.f1; if((
unsigned int) _temp1785 >  1u?*(( int*) _temp1785) ==  Cyc_Absyn_Eq_constr: 0){
_LL1788: _temp1787=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1785)->f1;
if(( unsigned int) _temp1787 >  1u?*(( int*) _temp1787) ==  Cyc_Absyn_Upper_b: 0){
_LL1790: _temp1789=(( struct Cyc_Absyn_Upper_b_struct*) _temp1787)->f1; goto
_LL1780;} else{ goto _LL1767;}} else{ goto _LL1767;} _LL1780: _temp1779=
_temp1761.f2; if(( unsigned int) _temp1779 >  1u?*(( int*) _temp1779) ==  Cyc_Absyn_Eq_constr:
0){ _LL1782: _temp1781=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1779)->f1;
if(( unsigned int) _temp1781 >  1u?*(( int*) _temp1781) ==  Cyc_Absyn_Upper_b: 0){
_LL1784: _temp1783=(( struct Cyc_Absyn_Upper_b_struct*) _temp1781)->f1; goto
_LL1766;} else{ goto _LL1767;}} else{ goto _LL1767;} _LL1767: _LL1796: _temp1795=
_temp1761.f1; if(( unsigned int) _temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Eq_constr:
0){ _LL1798: _temp1797=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1795)->f1;
if( _temp1797 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1792;} else{ goto
_LL1769;}} else{ goto _LL1769;} _LL1792: _temp1791= _temp1761.f2; if((
unsigned int) _temp1791 >  1u?*(( int*) _temp1791) ==  Cyc_Absyn_Eq_constr: 0){
_LL1794: _temp1793=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1791)->f1;
if( _temp1793 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1768;} else{ goto
_LL1769;}} else{ goto _LL1769;} _LL1769: goto _LL1770; _LL1764: okay= 1; goto
_LL1762; _LL1766: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1789) >=  Cyc_Evexp_eval_const_uint_exp(
_temp1783): 0;({ void* _temp1799[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1799, sizeof( void*), 0u));}); goto
_LL1762; _LL1768: okay= 1; goto _LL1762; _LL1770: okay= 0; goto _LL1762; _LL1762:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1683.elt_typ,( void*) _temp1679.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1683.rgn_typ,( void*) _temp1679.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1683.rgn_typ,( void*) _temp1679.rgn_typ):
0; okay= okay? !( _temp1683.tq).q_const? 1:( _temp1679.tq).q_const: 0; return
okay;} _LL1670: { int okay; okay=( _temp1695 !=  0? _temp1687 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1695)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1687)): 0; return( okay? Cyc_Tcutil_unify(
_temp1699, _temp1691): 0)? ! _temp1697.q_const? 1: _temp1689.q_const: 0;}
_LL1672: if(( _temp1722 ==  _temp1711? 1: Cyc_Absyn_qvar_cmp( _temp1722->name,
_temp1711->name) ==  0)? _temp1720->typs ==  0: 0){ for( 0; _temp1716 !=  0?
_temp1705 !=  0: 0;( _temp1716=(( struct Cyc_List_List*) _check_null( _temp1716))->tl,
_temp1705=(( struct Cyc_List_List*) _check_null( _temp1705))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1716))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1705))->hd)){ break;}} if( _temp1716 ==  0?
_temp1705 ==  0: 0){ return 1;}} return 0; _LL1674:{ void* _temp1800= Cyc_Tcutil_compress(
_temp1749); struct Cyc_Absyn_TunionFieldInfo _temp1806; struct Cyc_List_List*
_temp1808; void* _temp1810; struct Cyc_Absyn_Tunionfield* _temp1812; struct Cyc_Absyn_Tuniondecl*
_temp1814; _LL1802: if(( unsigned int) _temp1800 >  4u?*(( int*) _temp1800) == 
Cyc_Absyn_TunionFieldType: 0){ _LL1807: _temp1806=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1800)->f1; _LL1811: _temp1810=( void*) _temp1806.field_info; if(*(( int*)
_temp1810) ==  Cyc_Absyn_KnownTunionfield){ _LL1815: _temp1814=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1810)->f1; goto _LL1813; _LL1813: _temp1812=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1810)->f2; goto _LL1809;} else{ goto _LL1804;} _LL1809: _temp1808=
_temp1806.targs; goto _LL1803;} else{ goto _LL1804;} _LL1804: goto _LL1805;
_LL1803: if( ! Cyc_Tcutil_unify( _temp1747, _temp1728)? ! Cyc_Tcenv_region_outlives(
te, _temp1747, _temp1728): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1745,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1741,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1736->name, _temp1814->name) ==  0? _temp1812->typs !=  0: 0){ int okay= 1;
for( 0; _temp1808 !=  0? _temp1730 !=  0: 0;( _temp1808=(( struct Cyc_List_List*)
_check_null( _temp1808))->tl, _temp1730=(( struct Cyc_List_List*) _check_null(
_temp1730))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1808))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1730))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1808 !=  0)? 1:
_temp1730 !=  0){ return 0;} return 1;} goto _LL1801; _LL1805: goto _LL1801;
_LL1801:;} return 0; _LL1676: return Cyc_Tcutil_unify( t1, t2); _LL1666:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1816[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1816, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Std_String_pa_struct
_temp1819; _temp1819.tag= Cyc_Std_String_pa; _temp1819.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct _temp1818;
_temp1818.tag= Cyc_Std_String_pa; _temp1818.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1817[ 2u]={& _temp1818,& _temp1819}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1817, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} int Cyc_Tcutil_coerceable( void* t){ void* _temp1820= Cyc_Tcutil_compress(
t); _LL1822: if(( unsigned int) _temp1820 >  4u?*(( int*) _temp1820) ==  Cyc_Absyn_IntType:
0){ goto _LL1823;} else{ goto _LL1824;} _LL1824: if( _temp1820 == ( void*) Cyc_Absyn_FloatType){
goto _LL1825;} else{ goto _LL1826;} _LL1826: if( _temp1820 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1827;} else{ goto _LL1828;} _LL1828: goto _LL1829; _LL1823: goto _LL1825;
_LL1825: goto _LL1827; _LL1827: return 1; _LL1829: return 0; _LL1821:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1830=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp1830->f1= x->tq; _temp1830->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1830;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1831= t1; struct Cyc_List_List* _temp1843; struct Cyc_Absyn_Structdecl**
_temp1845; struct Cyc_List_List* _temp1847; struct _tuple1* _temp1849; struct
Cyc_List_List* _temp1851; _LL1833: if( _temp1831 == ( void*) Cyc_Absyn_VoidType){
goto _LL1834;} else{ goto _LL1835;} _LL1835: if(( unsigned int) _temp1831 >  4u?*((
int*) _temp1831) ==  Cyc_Absyn_TupleType: 0){ _LL1844: _temp1843=(( struct Cyc_Absyn_TupleType_struct*)
_temp1831)->f1; goto _LL1836;} else{ goto _LL1837;} _LL1837: if(( unsigned int)
_temp1831 >  4u?*(( int*) _temp1831) ==  Cyc_Absyn_StructType: 0){ _LL1850:
_temp1849=(( struct Cyc_Absyn_StructType_struct*) _temp1831)->f1; goto _LL1848;
_LL1848: _temp1847=(( struct Cyc_Absyn_StructType_struct*) _temp1831)->f2; goto
_LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_StructType_struct*) _temp1831)->f3;
goto _LL1838;} else{ goto _LL1839;} _LL1839: if(( unsigned int) _temp1831 >  4u?*((
int*) _temp1831) ==  Cyc_Absyn_AnonStructType: 0){ _LL1852: _temp1851=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1831)->f1; goto _LL1840;} else{ goto
_LL1841;} _LL1841: goto _LL1842; _LL1834: return 0; _LL1836: return _temp1843;
_LL1838: if( _temp1845 ==  0? 1: _temp1849 ==  0){ return({ struct Cyc_List_List*
_temp1853=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1853->hd=( void*)({ struct _tuple4* _temp1854=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1854->f1= Cyc_Absyn_empty_tqual(); _temp1854->f2=
t1; _temp1854;}); _temp1853->tl= 0; _temp1853;});}{ struct Cyc_Absyn_Structdecl*
_temp1855=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1845)); struct
_tuple1 _temp1856=*(( struct _tuple1*) _check_null( _temp1849)); struct Cyc_List_List*
_temp1857=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1855->tvs, _temp1847); if( _temp1855->fields ==  0){
return({ struct Cyc_List_List* _temp1858=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1858->hd=( void*)({ struct _tuple4*
_temp1859=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1859->f1=
Cyc_Absyn_empty_tqual(); _temp1859->f2= t1; _temp1859;}); _temp1858->tl= 0;
_temp1858;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1857,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1855->fields))->v);}
_LL1840: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1851); _LL1842: return({
struct Cyc_List_List* _temp1860=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1860->hd=( void*)({ struct _tuple4* _temp1861=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1861->f1= Cyc_Absyn_empty_tqual();
_temp1861->f2= t1; _temp1861;}); _temp1860->tl= 0; _temp1860;}); _LL1832:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1863=({ struct _tuple8 _temp1862; _temp1862.f1= t1; _temp1862.f2=
t2; _temp1862;}); void* _temp1869; struct Cyc_Absyn_PtrInfo _temp1871; struct
Cyc_Absyn_Conref* _temp1873; struct Cyc_Absyn_Tqual _temp1875; struct Cyc_Absyn_Conref*
_temp1877; void* _temp1879; void* _temp1881; void* _temp1883; struct Cyc_Absyn_PtrInfo
_temp1885; struct Cyc_Absyn_Conref* _temp1887; struct Cyc_Absyn_Tqual _temp1889;
struct Cyc_Absyn_Conref* _temp1891; void* _temp1893; void* _temp1895; _LL1865:
_LL1884: _temp1883= _temp1863.f1; if(( unsigned int) _temp1883 >  4u?*(( int*)
_temp1883) ==  Cyc_Absyn_PointerType: 0){ _LL1886: _temp1885=(( struct Cyc_Absyn_PointerType_struct*)
_temp1883)->f1; _LL1896: _temp1895=( void*) _temp1885.elt_typ; goto _LL1894;
_LL1894: _temp1893=( void*) _temp1885.rgn_typ; goto _LL1892; _LL1892: _temp1891=
_temp1885.nullable; goto _LL1890; _LL1890: _temp1889= _temp1885.tq; goto _LL1888;
_LL1888: _temp1887= _temp1885.bounds; goto _LL1870;} else{ goto _LL1867;}
_LL1870: _temp1869= _temp1863.f2; if(( unsigned int) _temp1869 >  4u?*(( int*)
_temp1869) ==  Cyc_Absyn_PointerType: 0){ _LL1872: _temp1871=(( struct Cyc_Absyn_PointerType_struct*)
_temp1869)->f1; _LL1882: _temp1881=( void*) _temp1871.elt_typ; goto _LL1880;
_LL1880: _temp1879=( void*) _temp1871.rgn_typ; goto _LL1878; _LL1878: _temp1877=
_temp1871.nullable; goto _LL1876; _LL1876: _temp1875= _temp1871.tq; goto _LL1874;
_LL1874: _temp1873= _temp1871.bounds; goto _LL1866;} else{ goto _LL1867;}
_LL1867: goto _LL1868; _LL1866: if( _temp1889.q_const? ! _temp1875.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1891, _temp1877)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1891): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1877): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1893, _temp1879)? ! Cyc_Tcenv_region_outlives(
te, _temp1893, _temp1879): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1887, _temp1873)){ struct _tuple8 _temp1898=({ struct _tuple8 _temp1897;
_temp1897.f1= Cyc_Absyn_conref_val( _temp1887); _temp1897.f2= Cyc_Absyn_conref_val(
_temp1873); _temp1897;}); void* _temp1906; void* _temp1908; void* _temp1910;
struct Cyc_Absyn_Exp* _temp1912; void* _temp1914; struct Cyc_Absyn_Exp*
_temp1916; _LL1900: _LL1909: _temp1908= _temp1898.f1; if(( unsigned int)
_temp1908 >  1u?*(( int*) _temp1908) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1907;}
else{ goto _LL1902;} _LL1907: _temp1906= _temp1898.f2; if( _temp1906 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1901;} else{ goto _LL1902;} _LL1902: _LL1915:
_temp1914= _temp1898.f1; if(( unsigned int) _temp1914 >  1u?*(( int*) _temp1914)
==  Cyc_Absyn_Upper_b: 0){ _LL1917: _temp1916=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1914)->f1; goto _LL1911;} else{ goto _LL1904;} _LL1911: _temp1910=
_temp1898.f2; if(( unsigned int) _temp1910 >  1u?*(( int*) _temp1910) ==  Cyc_Absyn_Upper_b:
0){ _LL1913: _temp1912=(( struct Cyc_Absyn_Upper_b_struct*) _temp1910)->f1; goto
_LL1903;} else{ goto _LL1904;} _LL1904: goto _LL1905; _LL1901: goto _LL1899;
_LL1903: if( Cyc_Evexp_eval_const_uint_exp( _temp1916) <  Cyc_Evexp_eval_const_uint_exp(
_temp1912)){ return 0;} goto _LL1899; _LL1905: return 0; _LL1899:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1918=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1918->hd=( void*)({ struct _tuple8*
_temp1919=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1919->f1=
t1; _temp1919->f2= t2; _temp1919;}); _temp1918->tl= assume; _temp1918;}),
_temp1895, _temp1881); _LL1868: return 0; _LL1864:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1921=({ struct _tuple8 _temp1920;
_temp1920.f1= Cyc_Tcutil_compress( t1); _temp1920.f2= Cyc_Tcutil_compress( t2);
_temp1920;}); void* _temp1927; void* _temp1929; void* _temp1931; void* _temp1933;
_LL1923: _LL1932: _temp1931= _temp1921.f1; if(( unsigned int) _temp1931 >  4u?*((
int*) _temp1931) ==  Cyc_Absyn_IntType: 0){ _LL1934: _temp1933=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1931)->f2; goto _LL1928;} else{ goto _LL1925;}
_LL1928: _temp1927= _temp1921.f2; if(( unsigned int) _temp1927 >  4u?*(( int*)
_temp1927) ==  Cyc_Absyn_IntType: 0){ _LL1930: _temp1929=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1927)->f2; goto _LL1924;} else{ goto _LL1925;} _LL1925: goto _LL1926;
_LL1924: return _temp1933 ==  _temp1929; _LL1926: return 0; _LL1922:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=((
struct Cyc_List_List*) _check_null( tqs1))->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1937; void* _temp1938; struct Cyc_Absyn_Tqual _temp1940;
struct _tuple4* _temp1935=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs1))->hd; _temp1937=* _temp1935; _LL1941: _temp1940= _temp1937.f1;
goto _LL1939; _LL1939: _temp1938= _temp1937.f2; goto _LL1936; _LL1936: { struct
_tuple4 _temp1944; void* _temp1945; struct Cyc_Absyn_Tqual _temp1947; struct
_tuple4* _temp1942=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs2))->hd;
_temp1944=* _temp1942; _LL1948: _temp1947= _temp1944.f1; goto _LL1946; _LL1946:
_temp1945= _temp1944.f2; goto _LL1943; _LL1943: if( _temp1947.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1938, _temp1945): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1938, _temp1945)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1938,
_temp1945)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1949= t2; struct Cyc_Absyn_Uniondecl** _temp1957; struct Cyc_List_List*
_temp1959; void* _temp1961; _LL1951: if(( unsigned int) _temp1949 >  4u?*(( int*)
_temp1949) ==  Cyc_Absyn_UnionType: 0){ _LL1960: _temp1959=(( struct Cyc_Absyn_UnionType_struct*)
_temp1949)->f2; goto _LL1958; _LL1958: _temp1957=(( struct Cyc_Absyn_UnionType_struct*)
_temp1949)->f3; goto _LL1952;} else{ goto _LL1953;} _LL1953: if(( unsigned int)
_temp1949 >  4u?*(( int*) _temp1949) ==  Cyc_Absyn_IntType: 0){ _LL1962:
_temp1961=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1949)->f2; if(
_temp1961 == ( void*) Cyc_Absyn_B4){ goto _LL1954;} else{ goto _LL1955;}} else{
goto _LL1955;} _LL1955: goto _LL1956; _LL1952: if( _temp1957 ==  0){ goto
_LL1950;}{ struct Cyc_Absyn_Uniondecl* _temp1963=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1957)); if( _temp1963->fields ==  0){ goto _LL1950;}{ struct
Cyc_List_List* _temp1964=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1963->tvs, _temp1959);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1963->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp1964,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1950;}} _LL1954: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1950; _LL1956: goto _LL1950; _LL1950:;}{ void* _temp1965= t1;
struct Cyc_Absyn_PtrInfo _temp1983; struct Cyc_Absyn_Conref* _temp1985; struct
Cyc_Absyn_Tqual _temp1987; struct Cyc_Absyn_Conref* _temp1989; void* _temp1991;
void* _temp1993; struct Cyc_Absyn_Exp* _temp1995; struct Cyc_Absyn_Tqual
_temp1997; void* _temp1999; struct Cyc_Absyn_Enumdecl* _temp2001; _LL1967: if((
unsigned int) _temp1965 >  4u?*(( int*) _temp1965) ==  Cyc_Absyn_PointerType: 0){
_LL1984: _temp1983=(( struct Cyc_Absyn_PointerType_struct*) _temp1965)->f1;
_LL1994: _temp1993=( void*) _temp1983.elt_typ; goto _LL1992; _LL1992: _temp1991=(
void*) _temp1983.rgn_typ; goto _LL1990; _LL1990: _temp1989= _temp1983.nullable;
goto _LL1988; _LL1988: _temp1987= _temp1983.tq; goto _LL1986; _LL1986: _temp1985=
_temp1983.bounds; goto _LL1968;} else{ goto _LL1969;} _LL1969: if(( unsigned int)
_temp1965 >  4u?*(( int*) _temp1965) ==  Cyc_Absyn_ArrayType: 0){ _LL2000:
_temp1999=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1965)->f1; goto
_LL1998; _LL1998: _temp1997=(( struct Cyc_Absyn_ArrayType_struct*) _temp1965)->f2;
goto _LL1996; _LL1996: _temp1995=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1965)->f3; goto _LL1970;} else{ goto _LL1971;} _LL1971: if(( unsigned int)
_temp1965 >  4u?*(( int*) _temp1965) ==  Cyc_Absyn_EnumType: 0){ _LL2002:
_temp2001=(( struct Cyc_Absyn_EnumType_struct*) _temp1965)->f2; goto _LL1972;}
else{ goto _LL1973;} _LL1973: if(( unsigned int) _temp1965 >  4u?*(( int*)
_temp1965) ==  Cyc_Absyn_IntType: 0){ goto _LL1974;} else{ goto _LL1975;}
_LL1975: if( _temp1965 == ( void*) Cyc_Absyn_FloatType){ goto _LL1976;} else{
goto _LL1977;} _LL1977: if( _temp1965 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1978;} else{ goto _LL1979;} _LL1979: if( _temp1965 == ( void*) Cyc_Absyn_VoidType){
goto _LL1980;} else{ goto _LL1981;} _LL1981: goto _LL1982; _LL1968:{ void*
_temp2003= t2; struct Cyc_Absyn_PtrInfo _temp2011; struct Cyc_Absyn_Conref*
_temp2013; struct Cyc_Absyn_Tqual _temp2015; struct Cyc_Absyn_Conref* _temp2017;
void* _temp2019; void* _temp2021; _LL2005: if(( unsigned int) _temp2003 >  4u?*((
int*) _temp2003) ==  Cyc_Absyn_PointerType: 0){ _LL2012: _temp2011=(( struct Cyc_Absyn_PointerType_struct*)
_temp2003)->f1; _LL2022: _temp2021=( void*) _temp2011.elt_typ; goto _LL2020;
_LL2020: _temp2019=( void*) _temp2011.rgn_typ; goto _LL2018; _LL2018: _temp2017=
_temp2011.nullable; goto _LL2016; _LL2016: _temp2015= _temp2011.tq; goto _LL2014;
_LL2014: _temp2013= _temp2011.bounds; goto _LL2006;} else{ goto _LL2007;}
_LL2007: if( _temp2003 == ( void*) Cyc_Absyn_VoidType){ goto _LL2008;} else{
goto _LL2009;} _LL2009: goto _LL2010; _LL2006: { struct Cyc_List_List* _temp2023=({
struct Cyc_List_List* _temp2040=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2040->hd=( void*)({ struct _tuple8* _temp2041=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2041->f1= t1;
_temp2041->f2= t2; _temp2041;}); _temp2040->tl= 0; _temp2040;}); int _temp2024=
Cyc_Tcutil_ptrsubtype( te, _temp2023, _temp1993, _temp2021)? ! _temp1987.q_const?
1: _temp2015.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1985, _temp2013); if( ! bounds_ok){ struct _tuple8 _temp2026=({ struct
_tuple8 _temp2025; _temp2025.f1= Cyc_Absyn_conref_val( _temp1985); _temp2025.f2=
Cyc_Absyn_conref_val( _temp2013); _temp2025;}); void* _temp2032; struct Cyc_Absyn_Exp*
_temp2034; void* _temp2036; struct Cyc_Absyn_Exp* _temp2038; _LL2028: _LL2037:
_temp2036= _temp2026.f1; if(( unsigned int) _temp2036 >  1u?*(( int*) _temp2036)
==  Cyc_Absyn_Upper_b: 0){ _LL2039: _temp2038=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2036)->f1; goto _LL2033;} else{ goto _LL2030;} _LL2033: _temp2032=
_temp2026.f2; if(( unsigned int) _temp2032 >  1u?*(( int*) _temp2032) ==  Cyc_Absyn_Upper_b:
0){ _LL2035: _temp2034=(( struct Cyc_Absyn_Upper_b_struct*) _temp2032)->f1; goto
_LL2029;} else{ goto _LL2030;} _LL2030: goto _LL2031; _LL2029: if( Cyc_Evexp_eval_const_uint_exp(
_temp2038) >=  Cyc_Evexp_eval_const_uint_exp( _temp2034)){ bounds_ok= 1;} goto
_LL2027; _LL2031: bounds_ok= 1; goto _LL2027; _LL2027:;} return( bounds_ok?
_temp2024: 0)? Cyc_Tcutil_unify( _temp1991, _temp2019)? 1: Cyc_Tcenv_region_outlives(
te, _temp1991, _temp2019): 0;} _LL2008: return 1; _LL2010: goto _LL2004; _LL2004:;}
return 0; _LL1970:{ void* _temp2042= t2; struct Cyc_Absyn_Exp* _temp2048; struct
Cyc_Absyn_Tqual _temp2050; void* _temp2052; _LL2044: if(( unsigned int)
_temp2042 >  4u?*(( int*) _temp2042) ==  Cyc_Absyn_ArrayType: 0){ _LL2053:
_temp2052=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2042)->f1; goto
_LL2051; _LL2051: _temp2050=(( struct Cyc_Absyn_ArrayType_struct*) _temp2042)->f2;
goto _LL2049; _LL2049: _temp2048=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2042)->f3; goto _LL2045;} else{ goto _LL2046;} _LL2046: goto _LL2047;
_LL2045: { int okay; okay=( _temp1995 !=  0? _temp2048 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1995)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2048)): 0; return( okay? Cyc_Tcutil_unify(
_temp1999, _temp2052): 0)? ! _temp1997.q_const? 1: _temp2050.q_const: 0;}
_LL2047: return 0; _LL2043:;} return 0; _LL1972:{ void* _temp2054= t2; struct
Cyc_Absyn_Enumdecl* _temp2060; _LL2056: if(( unsigned int) _temp2054 >  4u?*((
int*) _temp2054) ==  Cyc_Absyn_EnumType: 0){ _LL2061: _temp2060=(( struct Cyc_Absyn_EnumType_struct*)
_temp2054)->f2; goto _LL2057;} else{ goto _LL2058;} _LL2058: goto _LL2059;
_LL2057: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2001))->fields != 
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2060))->fields !=  0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp2001))->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2060))->fields))->v): 0){ return 1;} goto _LL2055; _LL2059:
goto _LL2055; _LL2055:;} goto _LL1974; _LL1974: goto _LL1976; _LL1976: goto
_LL1978; _LL1978: return Cyc_Tcutil_coerceable( t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType;
_LL1980: return t2 == ( void*) Cyc_Absyn_VoidType; _LL1982: return 0; _LL1966:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2062= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2063=(
struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2063[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2064; _temp2064.tag= Cyc_Absyn_Cast_e;
_temp2064.f1=( void*) t; _temp2064.f2= _temp2062; _temp2064;}); _temp2063;})));
e->topt=({ struct Cyc_Core_Opt* _temp2065=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2065->v=( void*) t; _temp2065;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2066= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2068: if(( unsigned int) _temp2066
>  4u?*(( int*) _temp2066) ==  Cyc_Absyn_EnumType: 0){ goto _LL2069;} else{ goto
_LL2070;} _LL2070: if(( unsigned int) _temp2066 >  4u?*(( int*) _temp2066) == 
Cyc_Absyn_IntType: 0){ goto _LL2071;} else{ goto _LL2072;} _LL2072: if((
unsigned int) _temp2066 >  4u?*(( int*) _temp2066) ==  Cyc_Absyn_Evar: 0){ goto
_LL2073;} else{ goto _LL2074;} _LL2074: goto _LL2075; _LL2069: goto _LL2071;
_LL2071: return 1; _LL2073: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2075: return 0; _LL2067:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2076= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2078: if( _temp2076 == ( void*) Cyc_Absyn_FloatType){
goto _LL2079;} else{ goto _LL2080;} _LL2080: if( _temp2076 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2081;} else{ goto _LL2082;} _LL2082: goto _LL2083; _LL2079: goto _LL2081;
_LL2081: return 1; _LL2083: return 0; _LL2077:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp2084= Cyc_Tcutil_compress( t); _LL2086: if(( unsigned int)
_temp2084 >  4u?*(( int*) _temp2084) ==  Cyc_Absyn_FnType: 0){ goto _LL2087;}
else{ goto _LL2088;} _LL2088: goto _LL2089; _LL2087: return 1; _LL2089: return 0;
_LL2085:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp2091=({ struct _tuple8 _temp2090; _temp2090.f1= t1; _temp2090.f2=
t2; _temp2090;}); void* _temp2115; void* _temp2117; void* _temp2119; void*
_temp2121; void* _temp2123; void* _temp2125; void* _temp2127; void* _temp2129;
void* _temp2131; void* _temp2133; void* _temp2135; void* _temp2137; void*
_temp2139; void* _temp2141; void* _temp2143; void* _temp2145; void* _temp2147;
void* _temp2149; void* _temp2151; void* _temp2153; void* _temp2155; void*
_temp2157; _LL2093: _LL2116: _temp2115= _temp2091.f1; if( _temp2115 == ( void*)
Cyc_Absyn_DoubleType){ goto _LL2094;} else{ goto _LL2095;} _LL2095: _LL2118:
_temp2117= _temp2091.f2; if( _temp2117 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2096;} else{ goto _LL2097;} _LL2097: _LL2120: _temp2119= _temp2091.f1; if(
_temp2119 == ( void*) Cyc_Absyn_FloatType){ goto _LL2098;} else{ goto _LL2099;}
_LL2099: _LL2122: _temp2121= _temp2091.f2; if( _temp2121 == ( void*) Cyc_Absyn_FloatType){
goto _LL2100;} else{ goto _LL2101;} _LL2101: _LL2124: _temp2123= _temp2091.f1;
if(( unsigned int) _temp2123 >  4u?*(( int*) _temp2123) ==  Cyc_Absyn_IntType: 0){
_LL2128: _temp2127=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2123)->f1;
if( _temp2127 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2126;} else{ goto _LL2103;}
_LL2126: _temp2125=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2123)->f2;
if( _temp2125 == ( void*) Cyc_Absyn_B8){ goto _LL2102;} else{ goto _LL2103;}}
else{ goto _LL2103;} _LL2103: _LL2130: _temp2129= _temp2091.f2; if((
unsigned int) _temp2129 >  4u?*(( int*) _temp2129) ==  Cyc_Absyn_IntType: 0){
_LL2134: _temp2133=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2129)->f1;
if( _temp2133 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2132;} else{ goto _LL2105;}
_LL2132: _temp2131=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2129)->f2;
if( _temp2131 == ( void*) Cyc_Absyn_B8){ goto _LL2104;} else{ goto _LL2105;}}
else{ goto _LL2105;} _LL2105: _LL2136: _temp2135= _temp2091.f1; if((
unsigned int) _temp2135 >  4u?*(( int*) _temp2135) ==  Cyc_Absyn_IntType: 0){
_LL2140: _temp2139=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2135)->f1;
if( _temp2139 == ( void*) Cyc_Absyn_Signed){ goto _LL2138;} else{ goto _LL2107;}
_LL2138: _temp2137=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2135)->f2;
if( _temp2137 == ( void*) Cyc_Absyn_B8){ goto _LL2106;} else{ goto _LL2107;}}
else{ goto _LL2107;} _LL2107: _LL2142: _temp2141= _temp2091.f2; if((
unsigned int) _temp2141 >  4u?*(( int*) _temp2141) ==  Cyc_Absyn_IntType: 0){
_LL2146: _temp2145=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2141)->f1;
if( _temp2145 == ( void*) Cyc_Absyn_Signed){ goto _LL2144;} else{ goto _LL2109;}
_LL2144: _temp2143=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2141)->f2;
if( _temp2143 == ( void*) Cyc_Absyn_B8){ goto _LL2108;} else{ goto _LL2109;}}
else{ goto _LL2109;} _LL2109: _LL2148: _temp2147= _temp2091.f1; if((
unsigned int) _temp2147 >  4u?*(( int*) _temp2147) ==  Cyc_Absyn_IntType: 0){
_LL2152: _temp2151=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2147)->f1;
if( _temp2151 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2150;} else{ goto _LL2111;}
_LL2150: _temp2149=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2147)->f2;
if( _temp2149 == ( void*) Cyc_Absyn_B4){ goto _LL2110;} else{ goto _LL2111;}}
else{ goto _LL2111;} _LL2111: _LL2154: _temp2153= _temp2091.f2; if((
unsigned int) _temp2153 >  4u?*(( int*) _temp2153) ==  Cyc_Absyn_IntType: 0){
_LL2158: _temp2157=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2153)->f1;
if( _temp2157 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2156;} else{ goto _LL2113;}
_LL2156: _temp2155=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2153)->f2;
if( _temp2155 == ( void*) Cyc_Absyn_B4){ goto _LL2112;} else{ goto _LL2113;}}
else{ goto _LL2113;} _LL2113: goto _LL2114; _LL2094: goto _LL2096; _LL2096:
return( void*) Cyc_Absyn_DoubleType; _LL2098: goto _LL2100; _LL2100: return(
void*) Cyc_Absyn_FloatType; _LL2102: goto _LL2104; _LL2104: return Cyc_Absyn_ulonglong_t;
_LL2106: goto _LL2108; _LL2108: return Cyc_Absyn_slonglong_t; _LL2110: goto
_LL2112; _LL2112: return Cyc_Absyn_uint_t; _LL2114: return Cyc_Absyn_sint_t;
_LL2092:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2159=( void*) e->r; struct Cyc_Core_Opt* _temp2165; _LL2161: if(*(( int*)
_temp2159) ==  Cyc_Absyn_AssignOp_e){ _LL2166: _temp2165=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2159)->f2; if( _temp2165 ==  0){ goto _LL2162;} else{ goto _LL2163;}} else{
goto _LL2163;} _LL2163: goto _LL2164; _LL2162:({ void* _temp2167[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2167, sizeof( void*), 0u));}); goto _LL2160; _LL2164: goto _LL2160; _LL2160:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2168=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2168[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2169; _temp2169.tag=
Cyc_Absyn_Forward_constr; _temp2169.f1= c2; _temp2169;}); _temp2168;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2170=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2170[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2171; _temp2171.tag= Cyc_Absyn_Forward_constr;
_temp2171.f1= c1; _temp2171;}); _temp2170;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2172=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2172[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2173; _temp2173.tag= Cyc_Absyn_Forward_constr; _temp2173.f1= c1; _temp2173;});
_temp2172;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2174=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2174[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2175; _temp2175.tag= Cyc_Absyn_Forward_constr;
_temp2175.f1= c2; _temp2175;}); _temp2174;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2176=( int*) _cycalloc_atomic( sizeof( int)); _temp2176[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2176;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2180;
_temp2180.tag= Cyc_Std_Int_pa; _temp2180.f1=( int)(( unsigned int) i);{ void*
_temp2179[ 1u]={& _temp2180}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2179, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2177=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2177->name=({ struct _tagged_arr* _temp2178=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2178[ 0]=
s; _temp2178;}); _temp2177->identity= 0; _temp2177->kind= Cyc_Absyn_new_conref(
k); _temp2177;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2181=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2181, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2183; _temp2183.tag= Cyc_Std_String_pa; _temp2183.f1=( struct _tagged_arr)*
t->name;{ void* _temp2182[ 1u]={& _temp2183}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2182, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2184= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2184, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2185=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2185[ 0]=( struct _tagged_arr) _temp2184; _temp2185;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2186=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2186->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2187=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2187->v=( void*)(*
x).f1; _temp2187;}); _temp2186->f2=(* x).f2; _temp2186->f3=(* x).f3; _temp2186;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2188= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2188=({ struct Cyc_List_List* _temp2189=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2189->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2189->tl= _temp2188; _temp2189;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2190=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2190[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2191; _temp2191.tag= Cyc_Absyn_FnType; _temp2191.f1=({ struct Cyc_Absyn_FnInfo
_temp2192; _temp2192.tvars= fd->tvs; _temp2192.effect= fd->effect; _temp2192.ret_typ=(
void*)(( void*) fd->ret_type); _temp2192.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2192.c_varargs= fd->c_varargs;
_temp2192.cyc_varargs= fd->cyc_varargs; _temp2192.rgn_po= fd->rgn_po; _temp2192.attributes=
_temp2188; _temp2192;}); _temp2191;}); _temp2190;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2193=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp2193->f1=(* pr).f1; _temp2193->f2= t; _temp2193;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2194=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2194->f1=({ struct _tuple15* _temp2195=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2195->f1=(* y).f1; _temp2195->f2=(* y).f2;
_temp2195;}); _temp2194->f2=(* y).f3; _temp2194;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2198; struct _tuple15* _temp2200; struct
_tuple16 _temp2196=* w; _LL2201: _temp2200= _temp2196.f1; goto _LL2199; _LL2199:
_temp2198= _temp2196.f2; goto _LL2197; _LL2197: { struct Cyc_Absyn_Tqual
_temp2204; struct Cyc_Core_Opt* _temp2206; struct _tuple15 _temp2202=* _temp2200;
_LL2207: _temp2206= _temp2202.f1; goto _LL2205; _LL2205: _temp2204= _temp2202.f2;
goto _LL2203; _LL2203: return({ struct _tuple2* _temp2208=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2208->f1= _temp2206; _temp2208->f2=
_temp2204; _temp2208->f3= _temp2198; _temp2208;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2209=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2209->name= f->name;
_temp2209->tq= f->tq; _temp2209->type=( void*) t; _temp2209->width= f->width;
_temp2209->attributes= f->attributes; _temp2209;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2210= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2258; struct Cyc_Absyn_Structdecl** _temp2260;
struct Cyc_List_List* _temp2262; struct _tuple1* _temp2264; struct Cyc_Absyn_Uniondecl**
_temp2266; struct Cyc_List_List* _temp2268; struct _tuple1* _temp2270; struct
Cyc_Absyn_TunionInfo _temp2272; void* _temp2274; struct Cyc_List_List* _temp2276;
void* _temp2278; struct Cyc_Absyn_TunionFieldInfo _temp2280; struct Cyc_List_List*
_temp2282; void* _temp2284; struct Cyc_Core_Opt* _temp2286; struct Cyc_List_List*
_temp2288; struct _tuple1* _temp2290; struct Cyc_Absyn_Exp* _temp2292; struct
Cyc_Absyn_Tqual _temp2294; void* _temp2296; struct Cyc_Absyn_PtrInfo _temp2298;
struct Cyc_Absyn_Conref* _temp2300; struct Cyc_Absyn_Tqual _temp2302; struct Cyc_Absyn_Conref*
_temp2304; void* _temp2306; void* _temp2308; struct Cyc_Absyn_FnInfo _temp2310;
struct Cyc_List_List* _temp2312; struct Cyc_List_List* _temp2314; struct Cyc_Absyn_VarargInfo*
_temp2316; int _temp2318; struct Cyc_List_List* _temp2320; void* _temp2322;
struct Cyc_Core_Opt* _temp2324; struct Cyc_List_List* _temp2326; struct Cyc_List_List*
_temp2328; struct Cyc_List_List* _temp2330; struct Cyc_List_List* _temp2332;
struct Cyc_Core_Opt* _temp2334; void* _temp2336; void* _temp2338; void*
_temp2340; struct Cyc_List_List* _temp2342; _LL2212: if(( unsigned int)
_temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_VarType: 0){ _LL2259:
_temp2258=(( struct Cyc_Absyn_VarType_struct*) _temp2210)->f1; goto _LL2213;}
else{ goto _LL2214;} _LL2214: if(( unsigned int) _temp2210 >  4u?*(( int*)
_temp2210) ==  Cyc_Absyn_StructType: 0){ _LL2265: _temp2264=(( struct Cyc_Absyn_StructType_struct*)
_temp2210)->f1; goto _LL2263; _LL2263: _temp2262=(( struct Cyc_Absyn_StructType_struct*)
_temp2210)->f2; goto _LL2261; _LL2261: _temp2260=(( struct Cyc_Absyn_StructType_struct*)
_temp2210)->f3; goto _LL2215;} else{ goto _LL2216;} _LL2216: if(( unsigned int)
_temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_UnionType: 0){ _LL2271:
_temp2270=(( struct Cyc_Absyn_UnionType_struct*) _temp2210)->f1; goto _LL2269;
_LL2269: _temp2268=(( struct Cyc_Absyn_UnionType_struct*) _temp2210)->f2; goto
_LL2267; _LL2267: _temp2266=(( struct Cyc_Absyn_UnionType_struct*) _temp2210)->f3;
goto _LL2217;} else{ goto _LL2218;} _LL2218: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_TunionType: 0){ _LL2273: _temp2272=(( struct Cyc_Absyn_TunionType_struct*)
_temp2210)->f1; _LL2279: _temp2278=( void*) _temp2272.tunion_info; goto _LL2277;
_LL2277: _temp2276= _temp2272.targs; goto _LL2275; _LL2275: _temp2274=( void*)
_temp2272.rgn; goto _LL2219;} else{ goto _LL2220;} _LL2220: if(( unsigned int)
_temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2281:
_temp2280=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2210)->f1; _LL2285:
_temp2284=( void*) _temp2280.field_info; goto _LL2283; _LL2283: _temp2282=
_temp2280.targs; goto _LL2221;} else{ goto _LL2222;} _LL2222: if(( unsigned int)
_temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_TypedefType: 0){ _LL2291:
_temp2290=(( struct Cyc_Absyn_TypedefType_struct*) _temp2210)->f1; goto _LL2289;
_LL2289: _temp2288=(( struct Cyc_Absyn_TypedefType_struct*) _temp2210)->f2; goto
_LL2287; _LL2287: _temp2286=(( struct Cyc_Absyn_TypedefType_struct*) _temp2210)->f3;
goto _LL2223;} else{ goto _LL2224;} _LL2224: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_ArrayType: 0){ _LL2297: _temp2296=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2210)->f1; goto _LL2295; _LL2295:
_temp2294=(( struct Cyc_Absyn_ArrayType_struct*) _temp2210)->f2; goto _LL2293;
_LL2293: _temp2292=(( struct Cyc_Absyn_ArrayType_struct*) _temp2210)->f3; goto
_LL2225;} else{ goto _LL2226;} _LL2226: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_PointerType: 0){ _LL2299: _temp2298=(( struct Cyc_Absyn_PointerType_struct*)
_temp2210)->f1; _LL2309: _temp2308=( void*) _temp2298.elt_typ; goto _LL2307;
_LL2307: _temp2306=( void*) _temp2298.rgn_typ; goto _LL2305; _LL2305: _temp2304=
_temp2298.nullable; goto _LL2303; _LL2303: _temp2302= _temp2298.tq; goto _LL2301;
_LL2301: _temp2300= _temp2298.bounds; goto _LL2227;} else{ goto _LL2228;}
_LL2228: if(( unsigned int) _temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_FnType:
0){ _LL2311: _temp2310=(( struct Cyc_Absyn_FnType_struct*) _temp2210)->f1;
_LL2327: _temp2326= _temp2310.tvars; goto _LL2325; _LL2325: _temp2324= _temp2310.effect;
goto _LL2323; _LL2323: _temp2322=( void*) _temp2310.ret_typ; goto _LL2321;
_LL2321: _temp2320= _temp2310.args; goto _LL2319; _LL2319: _temp2318= _temp2310.c_varargs;
goto _LL2317; _LL2317: _temp2316= _temp2310.cyc_varargs; goto _LL2315; _LL2315:
_temp2314= _temp2310.rgn_po; goto _LL2313; _LL2313: _temp2312= _temp2310.attributes;
goto _LL2229;} else{ goto _LL2230;} _LL2230: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_TupleType: 0){ _LL2329: _temp2328=(( struct Cyc_Absyn_TupleType_struct*)
_temp2210)->f1; goto _LL2231;} else{ goto _LL2232;} _LL2232: if(( unsigned int)
_temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_AnonStructType: 0){ _LL2331:
_temp2330=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2210)->f1; goto
_LL2233;} else{ goto _LL2234;} _LL2234: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2333: _temp2332=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2210)->f1; goto _LL2235;} else{ goto
_LL2236;} _LL2236: if(( unsigned int) _temp2210 >  4u?*(( int*) _temp2210) == 
Cyc_Absyn_Evar: 0){ _LL2335: _temp2334=(( struct Cyc_Absyn_Evar_struct*)
_temp2210)->f2; goto _LL2237;} else{ goto _LL2238;} _LL2238: if(( unsigned int)
_temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2337:
_temp2336=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2210)->f1;
goto _LL2239;} else{ goto _LL2240;} _LL2240: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_EnumType: 0){ goto _LL2241;} else{ goto _LL2242;}
_LL2242: if( _temp2210 == ( void*) Cyc_Absyn_VoidType){ goto _LL2243;} else{
goto _LL2244;} _LL2244: if(( unsigned int) _temp2210 >  4u?*(( int*) _temp2210)
==  Cyc_Absyn_IntType: 0){ goto _LL2245;} else{ goto _LL2246;} _LL2246: if(
_temp2210 == ( void*) Cyc_Absyn_FloatType){ goto _LL2247;} else{ goto _LL2248;}
_LL2248: if( _temp2210 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2249;} else{
goto _LL2250;} _LL2250: if( _temp2210 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL2251;} else{ goto _LL2252;} _LL2252: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_RgnsEff: 0){ _LL2339: _temp2338=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2210)->f1; goto _LL2253;} else{ goto _LL2254;}
_LL2254: if(( unsigned int) _temp2210 >  4u?*(( int*) _temp2210) ==  Cyc_Absyn_AccessEff:
0){ _LL2341: _temp2340=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2210)->f1;
goto _LL2255;} else{ goto _LL2256;} _LL2256: if(( unsigned int) _temp2210 >  4u?*((
int*) _temp2210) ==  Cyc_Absyn_JoinEff: 0){ _LL2343: _temp2342=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2210)->f1; goto _LL2257;} else{ goto _LL2211;} _LL2213: { struct
_handler_cons _temp2344; _push_handler(& _temp2344);{ int _temp2346= 0; if(
setjmp( _temp2344.handler)){ _temp2346= 1;} if( ! _temp2346){{ void* _temp2347=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2258); _npop_handler( 0u); return _temp2347;}; _pop_handler();} else{
void* _temp2345=( void*) _exn_thrown; void* _temp2349= _temp2345; _LL2351: if(
_temp2349 ==  Cyc_Core_Not_found){ goto _LL2352;} else{ goto _LL2353;} _LL2353:
goto _LL2354; _LL2352: return t; _LL2354:( void) _throw( _temp2349); _LL2350:;}}}
_LL2215: { struct Cyc_List_List* _temp2355= Cyc_Tcutil_substs( rgn, inst,
_temp2262); return _temp2355 ==  _temp2262? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2356=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2356[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2357; _temp2357.tag=
Cyc_Absyn_StructType; _temp2357.f1= _temp2264; _temp2357.f2= _temp2355;
_temp2357.f3= _temp2260; _temp2357;}); _temp2356;});} _LL2217: { struct Cyc_List_List*
_temp2358= Cyc_Tcutil_substs( rgn, inst, _temp2268); return _temp2358 == 
_temp2268? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2359=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2359[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2360; _temp2360.tag= Cyc_Absyn_UnionType;
_temp2360.f1= _temp2270; _temp2360.f2= _temp2358; _temp2360.f3= _temp2266;
_temp2360;}); _temp2359;});} _LL2219: { struct Cyc_List_List* _temp2361= Cyc_Tcutil_substs(
rgn, inst, _temp2276); void* _temp2362= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2274); return( _temp2361 ==  _temp2276? _temp2362 ==  _temp2274: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2363=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2363[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2364; _temp2364.tag= Cyc_Absyn_TunionType;
_temp2364.f1=({ struct Cyc_Absyn_TunionInfo _temp2365; _temp2365.tunion_info=(
void*) _temp2278; _temp2365.targs= _temp2361; _temp2365.rgn=( void*) _temp2362;
_temp2365;}); _temp2364;}); _temp2363;});} _LL2221: { struct Cyc_List_List*
_temp2366= Cyc_Tcutil_substs( rgn, inst, _temp2282); return _temp2366 == 
_temp2282? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2367=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2367[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2368; _temp2368.tag=
Cyc_Absyn_TunionFieldType; _temp2368.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2369; _temp2369.field_info=( void*) _temp2284; _temp2369.targs= _temp2366;
_temp2369;}); _temp2368;}); _temp2367;});} _LL2223: { struct Cyc_List_List*
_temp2370= Cyc_Tcutil_substs( rgn, inst, _temp2288); return _temp2370 == 
_temp2288? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2371=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2371[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2372; _temp2372.tag=
Cyc_Absyn_TypedefType; _temp2372.f1= _temp2290; _temp2372.f2= _temp2370;
_temp2372.f3= _temp2286; _temp2372;}); _temp2371;});} _LL2225: { void* _temp2373=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2296); return _temp2373 ==  _temp2296? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2374=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2374[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2375; _temp2375.tag= Cyc_Absyn_ArrayType;
_temp2375.f1=( void*) _temp2373; _temp2375.f2= _temp2294; _temp2375.f3=
_temp2292; _temp2375;}); _temp2374;});} _LL2227: { void* _temp2376= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2308); void* _temp2377= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2306); if( _temp2376 ==  _temp2308? _temp2377 ==  _temp2306: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2378=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2378[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2379; _temp2379.tag= Cyc_Absyn_PointerType;
_temp2379.f1=({ struct Cyc_Absyn_PtrInfo _temp2380; _temp2380.elt_typ=( void*)
_temp2376; _temp2380.rgn_typ=( void*) _temp2377; _temp2380.nullable= _temp2304;
_temp2380.tq= _temp2302; _temp2380.bounds= _temp2300; _temp2380;}); _temp2379;});
_temp2378;});} _LL2229:{ struct Cyc_List_List* _temp2381= _temp2326; for( 0;
_temp2381 !=  0; _temp2381=(( struct Cyc_List_List*) _check_null( _temp2381))->tl){
inst=({ struct Cyc_List_List* _temp2382=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2382->hd=( void*)({ struct _tuple5*
_temp2383=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2383->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2381))->hd; _temp2383->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2384=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2384[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2385; _temp2385.tag= Cyc_Absyn_VarType;
_temp2385.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2381))->hd; _temp2385;}); _temp2384;}); _temp2383;}); _temp2382->tl= inst;
_temp2382;});}}{ struct Cyc_List_List* _temp2388; struct Cyc_List_List*
_temp2390; struct _tuple0 _temp2386=(( struct _tuple0(*)( struct _RegionHandle*
r1, struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2320)); _LL2391:
_temp2390= _temp2386.f1; goto _LL2389; _LL2389: _temp2388= _temp2386.f2; goto
_LL2387; _LL2387: { struct Cyc_List_List* _temp2392= Cyc_Tcutil_substs( rgn,
inst, _temp2388); struct Cyc_List_List* _temp2393=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r1,
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2390, _temp2392)); struct Cyc_Core_Opt* eff2; if( _temp2324 ==  0){
eff2= 0;} else{ void* _temp2394= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2324))->v); if( _temp2394 == ( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2324))->v){ eff2= _temp2324;} else{ eff2=({
struct Cyc_Core_Opt* _temp2395=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2395->v=( void*) _temp2394; _temp2395;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2316 ==  0){ cyc_varargs2= 0;} else{ int _temp2398; void*
_temp2400; struct Cyc_Absyn_Tqual _temp2402; struct Cyc_Core_Opt* _temp2404;
struct Cyc_Absyn_VarargInfo _temp2396=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2316)); _LL2405: _temp2404= _temp2396.name; goto _LL2403;
_LL2403: _temp2402= _temp2396.tq; goto _LL2401; _LL2401: _temp2400=( void*)
_temp2396.type; goto _LL2399; _LL2399: _temp2398= _temp2396.inject; goto _LL2397;
_LL2397: { void* _temp2406= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2400); if(
_temp2406 ==  _temp2400){ cyc_varargs2= _temp2316;} else{ cyc_varargs2=({ struct
Cyc_Absyn_VarargInfo* _temp2407=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2407->name= _temp2404; _temp2407->tq=
_temp2402; _temp2407->type=( void*) _temp2406; _temp2407->inject= _temp2398;
_temp2407;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2410;
struct Cyc_List_List* _temp2412; struct _tuple0 _temp2408= Cyc_List_rsplit( rgn,
rgn, _temp2314); _LL2413: _temp2412= _temp2408.f1; goto _LL2411; _LL2411:
_temp2410= _temp2408.f2; goto _LL2409; _LL2409: { struct Cyc_List_List*
_temp2414= Cyc_Tcutil_substs( rgn, inst, _temp2412); struct Cyc_List_List*
_temp2415= Cyc_Tcutil_substs( rgn, inst, _temp2410); if( _temp2414 ==  _temp2412?
_temp2415 ==  _temp2410: 0){ rgn_po2= _temp2314;} else{ rgn_po2= Cyc_List_zip(
_temp2414, _temp2415);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2416=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2416[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2417; _temp2417.tag= Cyc_Absyn_FnType;
_temp2417.f1=({ struct Cyc_Absyn_FnInfo _temp2418; _temp2418.tvars= _temp2326;
_temp2418.effect= eff2; _temp2418.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2322); _temp2418.args= _temp2393; _temp2418.c_varargs= _temp2318;
_temp2418.cyc_varargs= cyc_varargs2; _temp2418.rgn_po= rgn_po2; _temp2418.attributes=
_temp2312; _temp2418;}); _temp2417;}); _temp2416;});}}}}} _LL2231: { struct Cyc_List_List*
_temp2419=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2328); struct Cyc_List_List* _temp2420= Cyc_Tcutil_substs( rgn, inst,
_temp2419); if( _temp2420 ==  _temp2419){ return t;}{ struct Cyc_List_List*
_temp2421=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2328, _temp2420); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2422=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2422[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2423; _temp2423.tag= Cyc_Absyn_TupleType;
_temp2423.f1= _temp2421; _temp2423;}); _temp2422;});}} _LL2233: { struct Cyc_List_List*
_temp2424=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2330); struct Cyc_List_List* _temp2425= Cyc_Tcutil_substs( rgn, inst,
_temp2424); if( _temp2425 ==  _temp2424){ return t;}{ struct Cyc_List_List*
_temp2426=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2330, _temp2425); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2427=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2427[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2428; _temp2428.tag= Cyc_Absyn_AnonStructType;
_temp2428.f1= _temp2426; _temp2428;}); _temp2427;});}} _LL2235: { struct Cyc_List_List*
_temp2429=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2332); struct Cyc_List_List* _temp2430= Cyc_Tcutil_substs( rgn, inst,
_temp2429); if( _temp2430 ==  _temp2429){ return t;}{ struct Cyc_List_List*
_temp2431=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2332, _temp2430); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2432=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2432[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2433; _temp2433.tag= Cyc_Absyn_AnonStructType;
_temp2433.f1= _temp2431; _temp2433;}); _temp2432;});}} _LL2237: if( _temp2334 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2334))->v);} else{ return t;} _LL2239: { void* _temp2434= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2336); return _temp2434 ==  _temp2336? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2435=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2435[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2436; _temp2436.tag= Cyc_Absyn_RgnHandleType; _temp2436.f1=( void*)
_temp2434; _temp2436;}); _temp2435;});} _LL2241: return t; _LL2243: return t;
_LL2245: return t; _LL2247: return t; _LL2249: return t; _LL2251: return t;
_LL2253: { void* _temp2437= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2338);
return _temp2437 ==  _temp2338? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2438=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2438[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2439; _temp2439.tag= Cyc_Absyn_RgnsEff;
_temp2439.f1=( void*) _temp2437; _temp2439;}); _temp2438;});} _LL2255: { void*
_temp2440= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2340); return _temp2440 == 
_temp2340? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2441=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2441[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2442; _temp2442.tag= Cyc_Absyn_AccessEff;
_temp2442.f1=( void*) _temp2440; _temp2442;}); _temp2441;});} _LL2257: { struct
Cyc_List_List* _temp2443= Cyc_Tcutil_substs( rgn, inst, _temp2342); return
_temp2443 ==  _temp2342? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2444=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2444[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2445; _temp2445.tag= Cyc_Absyn_JoinEff;
_temp2445.f1= _temp2443; _temp2445;}); _temp2444;});} _LL2211:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2446=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2447=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2448= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2446); struct Cyc_List_List* _temp2449= Cyc_Tcutil_substs( rgn,
inst, _temp2447); if( _temp2446 ==  _temp2448? _temp2447 ==  _temp2449: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2450=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2450->hd=(
void*) _temp2448; _temp2450->tl= _temp2449; _temp2450;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2495=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2495[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2496; _temp2496.tag= Cyc_Absyn_Const_e; _temp2496.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2496;}); _temp2495;}), loc);{ void*
_temp2451= Cyc_Tcutil_compress( t); void* _temp2465; void* _temp2467; void*
_temp2469; void* _temp2471; _LL2453: if(( unsigned int) _temp2451 >  4u?*(( int*)
_temp2451) ==  Cyc_Absyn_PointerType: 0){ goto _LL2454;} else{ goto _LL2455;}
_LL2455: if(( unsigned int) _temp2451 >  4u?*(( int*) _temp2451) ==  Cyc_Absyn_IntType:
0){ _LL2468: _temp2467=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2451)->f1;
goto _LL2466; _LL2466: _temp2465=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2451)->f2; if( _temp2465 == ( void*) Cyc_Absyn_B1){ goto _LL2456;} else{
goto _LL2457;}} else{ goto _LL2457;} _LL2457: if(( unsigned int) _temp2451 >  4u?*((
int*) _temp2451) ==  Cyc_Absyn_IntType: 0){ _LL2472: _temp2471=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2451)->f1; goto _LL2470; _LL2470: _temp2469=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2451)->f2; goto _LL2458;} else{
goto _LL2459;} _LL2459: if( _temp2451 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2460;} else{ goto _LL2461;} _LL2461: if( _temp2451 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2462;} else{ goto _LL2463;} _LL2463: goto _LL2464; _LL2454: goto _LL2452;
_LL2456:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2473=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2473[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2474; _temp2474.tag= Cyc_Absyn_Const_e;
_temp2474.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2475=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2475[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2476; _temp2476.tag= Cyc_Absyn_Char_c;
_temp2476.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2476.f2='\000'; _temp2476;});
_temp2475;})); _temp2474;}); _temp2473;}))); goto _LL2452; _LL2458:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2477=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2477[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2478; _temp2478.tag= Cyc_Absyn_Const_e; _temp2478.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2479=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2479[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2480; _temp2480.tag= Cyc_Absyn_Int_c; _temp2480.f1=( void*) _temp2471;
_temp2480.f2= 0; _temp2480;}); _temp2479;})); _temp2478;}); _temp2477;}))); if(
_temp2469 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2481=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2481[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2482; _temp2482.tag= Cyc_Absyn_Cast_e;
_temp2482.f1=( void*) t; _temp2482.f2= e; _temp2482;}); _temp2481;}), loc);}
goto _LL2452; _LL2460:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2483=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2483[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2484; _temp2484.tag= Cyc_Absyn_Const_e;
_temp2484.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2485=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2485[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2486; _temp2486.tag= Cyc_Absyn_Float_c;
_temp2486.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2486;});
_temp2485;})); _temp2484;}); _temp2483;}))); goto _LL2452; _LL2462:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2487=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2487[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2488; _temp2488.tag= Cyc_Absyn_Cast_e; _temp2488.f1=( void*) t; _temp2488.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2489=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2489[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2490; _temp2490.tag= Cyc_Absyn_Const_e;
_temp2490.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2491=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2491[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2492; _temp2492.tag= Cyc_Absyn_Float_c;
_temp2492.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2492;});
_temp2491;})); _temp2490;}); _temp2489;}), loc); _temp2488;}); _temp2487;})));
goto _LL2452; _LL2464:({ struct Cyc_Std_String_pa_struct _temp2494; _temp2494.tag=
Cyc_Std_String_pa; _temp2494.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2493[ 1u]={& _temp2494}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2493, sizeof( void*), 1u));}});
goto _LL2452; _LL2452:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2497=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp2497->f1= tv; _temp2497->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2498=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2498->v=( void*) k; _temp2498;}),({ struct Cyc_Core_Opt*
_temp2499=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2499->v=( void*) s; _temp2499;})); _temp2497;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2502;
struct _RegionHandle* _temp2503; struct Cyc_List_List* _temp2505; struct _tuple6*
_temp2500= env; _temp2502=* _temp2500; _LL2506: _temp2505= _temp2502.f1; goto
_LL2504; _LL2504: _temp2503= _temp2502.f2; goto _LL2501; _LL2501: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2507=( struct _tuple5*) _region_malloc(
_temp2503, sizeof( struct _tuple5)); _temp2507->f1= tv; _temp2507->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2508=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2508->v=( void*) k; _temp2508;}),({ struct Cyc_Core_Opt*
_temp2509=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2509->v=( void*) _temp2505; _temp2509;})); _temp2507;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_Std_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_Std_String_pa_struct _temp2513; _temp2513.tag= Cyc_Std_String_pa;
_temp2513.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct
_temp2512; _temp2512.tag= Cyc_Std_String_pa; _temp2512.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_Std_String_pa_struct _temp2511;
_temp2511.tag= Cyc_Std_String_pa; _temp2511.f1=( struct _tagged_arr)* tv->name;{
void* _temp2510[ 3u]={& _temp2511,& _temp2512,& _temp2513}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2510, sizeof( void*), 3u));}}}});} if( tv->identity
==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!= *(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2514[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2514, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2515=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2515->hd=( void*) tv; _temp2515->tl= tvs; _temp2515;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2516[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2516, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity ==  0){({ void* _temp2517[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2517, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2518=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2518->hd=( void*) tv; _temp2518->tl=
tvs; _temp2518;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp2520; _temp2520.tag= Cyc_Std_String_pa;
_temp2520.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2519[
1u]={& _temp2520};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2519, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2521=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2521->hd=(
void*) tv; _temp2521->tl= tvs; _temp2521;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2522= Cyc_Tcutil_compress( e);
int _temp2528; _LL2524: if(( unsigned int) _temp2522 >  4u?*(( int*) _temp2522)
==  Cyc_Absyn_Evar: 0){ _LL2529: _temp2528=(( struct Cyc_Absyn_Evar_struct*)
_temp2522)->f3; goto _LL2525;} else{ goto _LL2526;} _LL2526: goto _LL2527;
_LL2525:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2530= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2536; _LL2532: if((
unsigned int) _temp2530 >  4u?*(( int*) _temp2530) ==  Cyc_Absyn_Evar: 0){
_LL2537: _temp2536=(( struct Cyc_Absyn_Evar_struct*) _temp2530)->f3; goto
_LL2533;} else{ goto _LL2534;} _LL2534: goto _LL2535; _LL2533: if( _temp2528 == 
_temp2536){ return es;} goto _LL2531; _LL2535: goto _LL2531; _LL2531:;}} return({
struct Cyc_List_List* _temp2538=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2538->hd=( void*) e; _temp2538->tl= es; _temp2538;});
_LL2527: return es; _LL2523:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b !=  0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) == *(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2539=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2539->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2539->tl= r; _temp2539;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp2541; _temp2541.tag= Cyc_Std_String_pa; _temp2541.f1=( struct _tagged_arr)*
fn;{ void* _temp2540[ 1u]={& _temp2541}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2540, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2542= Cyc_Tcutil_compress( field_typ); void* _temp2548;
_LL2544: if(( unsigned int) _temp2542 >  4u?*(( int*) _temp2542) ==  Cyc_Absyn_IntType:
0){ _LL2549: _temp2548=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2542)->f2;
goto _LL2545;} else{ goto _LL2546;} _LL2546: goto _LL2547; _LL2545:{ void*
_temp2550= _temp2548; _LL2552: if( _temp2550 == ( void*) Cyc_Absyn_B1){ goto
_LL2553;} else{ goto _LL2554;} _LL2554: if( _temp2550 == ( void*) Cyc_Absyn_B2){
goto _LL2555;} else{ goto _LL2556;} _LL2556: if( _temp2550 == ( void*) Cyc_Absyn_B4){
goto _LL2557;} else{ goto _LL2558;} _LL2558: if( _temp2550 == ( void*) Cyc_Absyn_B8){
goto _LL2559;} else{ goto _LL2551;} _LL2553: if( w >  8){({ void* _temp2560[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2560, sizeof( void*), 0u));});} goto
_LL2551; _LL2555: if( w >  16){({ void* _temp2561[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2561, sizeof( void*), 0u));});} goto _LL2551; _LL2557: if( w >  32){({ void*
_temp2562[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2562, sizeof( void*), 0u));});}
goto _LL2551; _LL2559: if( w >  64){({ void* _temp2563[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2563, sizeof( void*), 0u));});} goto _LL2551; _LL2551:;} goto
_LL2543; _LL2547:({ struct Cyc_Std_String_pa_struct _temp2566; _temp2566.tag=
Cyc_Std_String_pa; _temp2566.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp2565; _temp2565.tag= Cyc_Std_String_pa;
_temp2565.f1=( struct _tagged_arr)* fn;{ void* _temp2564[ 2u]={& _temp2565,&
_temp2566}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2564, sizeof( void*), 2u));}}});
goto _LL2543; _LL2543:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp2567=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL2569: if( _temp2567 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL2570;} else{ goto _LL2571;} _LL2571: if((
unsigned int) _temp2567 >  16u?*(( int*) _temp2567) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2572;} else{ goto _LL2573;} _LL2573: goto _LL2574; _LL2570: continue;
_LL2572: continue; _LL2574:({ struct Cyc_Std_String_pa_struct _temp2577;
_temp2577.tag= Cyc_Std_String_pa; _temp2577.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp2576; _temp2576.tag= Cyc_Std_String_pa;
_temp2576.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp2575[ 2u]={& _temp2576,&
_temp2577}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2575, sizeof( void*), 2u));}}});
goto _LL2568; _LL2568:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv
cvtenv, void* expected_kind, void* t){{ void* _temp2578= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp2626; struct Cyc_Core_Opt** _temp2628; struct Cyc_Core_Opt*
_temp2629; struct Cyc_Core_Opt** _temp2631; struct Cyc_Absyn_Tvar* _temp2632;
struct Cyc_Absyn_Enumdecl* _temp2634; struct Cyc_Absyn_Enumdecl** _temp2636;
struct _tuple1* _temp2637; struct Cyc_Absyn_TunionInfo _temp2639; void*
_temp2641; struct Cyc_List_List* _temp2643; void* _temp2645; void** _temp2647;
struct Cyc_Absyn_TunionFieldInfo _temp2648; struct Cyc_List_List* _temp2650;
void* _temp2652; void** _temp2654; struct Cyc_Absyn_PtrInfo _temp2655; struct
Cyc_Absyn_Conref* _temp2657; struct Cyc_Absyn_Tqual _temp2659; struct Cyc_Absyn_Conref*
_temp2661; void* _temp2663; void* _temp2665; struct Cyc_Absyn_Exp* _temp2667;
struct Cyc_Absyn_Tqual _temp2669; void* _temp2671; struct Cyc_Absyn_FnInfo
_temp2673; struct Cyc_List_List* _temp2675; struct Cyc_List_List* _temp2677;
struct Cyc_Absyn_VarargInfo* _temp2679; int _temp2681; struct Cyc_List_List*
_temp2683; void* _temp2685; struct Cyc_Core_Opt* _temp2687; struct Cyc_Core_Opt**
_temp2689; struct Cyc_List_List* _temp2690; struct Cyc_List_List** _temp2692;
struct Cyc_List_List* _temp2693; struct Cyc_List_List* _temp2695; struct Cyc_List_List*
_temp2697; struct Cyc_Absyn_Structdecl** _temp2699; struct Cyc_Absyn_Structdecl***
_temp2701; struct Cyc_List_List* _temp2702; struct _tuple1* _temp2704; struct
Cyc_Absyn_Uniondecl** _temp2706; struct Cyc_Absyn_Uniondecl*** _temp2708; struct
Cyc_List_List* _temp2709; struct _tuple1* _temp2711; struct Cyc_Core_Opt*
_temp2713; struct Cyc_Core_Opt** _temp2715; struct Cyc_List_List* _temp2716;
struct Cyc_List_List** _temp2718; struct _tuple1* _temp2719; void* _temp2721;
void* _temp2723; void* _temp2725; struct Cyc_List_List* _temp2727; _LL2580: if(
_temp2578 == ( void*) Cyc_Absyn_VoidType){ goto _LL2581;} else{ goto _LL2582;}
_LL2582: if(( unsigned int) _temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_Evar:
0){ _LL2630: _temp2629=(( struct Cyc_Absyn_Evar_struct*) _temp2578)->f1;
_temp2631=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2578)->f1;
goto _LL2627; _LL2627: _temp2626=(( struct Cyc_Absyn_Evar_struct*) _temp2578)->f2;
_temp2628=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2578)->f2;
goto _LL2583;} else{ goto _LL2584;} _LL2584: if(( unsigned int) _temp2578 >  4u?*((
int*) _temp2578) ==  Cyc_Absyn_VarType: 0){ _LL2633: _temp2632=(( struct Cyc_Absyn_VarType_struct*)
_temp2578)->f1; goto _LL2585;} else{ goto _LL2586;} _LL2586: if(( unsigned int)
_temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_EnumType: 0){ _LL2638:
_temp2637=(( struct Cyc_Absyn_EnumType_struct*) _temp2578)->f1; goto _LL2635;
_LL2635: _temp2634=(( struct Cyc_Absyn_EnumType_struct*) _temp2578)->f2;
_temp2636=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2578)->f2; goto _LL2587;} else{ goto _LL2588;} _LL2588: if(( unsigned int)
_temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_TunionType: 0){ _LL2640:
_temp2639=(( struct Cyc_Absyn_TunionType_struct*) _temp2578)->f1; _LL2646:
_temp2645=( void*) _temp2639.tunion_info; _temp2647=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2578)->f1).tunion_info; goto _LL2644; _LL2644: _temp2643= _temp2639.targs;
goto _LL2642; _LL2642: _temp2641=( void*) _temp2639.rgn; goto _LL2589;} else{
goto _LL2590;} _LL2590: if(( unsigned int) _temp2578 >  4u?*(( int*) _temp2578)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2649: _temp2648=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2578)->f1; _LL2653: _temp2652=( void*) _temp2648.field_info; _temp2654=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2578)->f1).field_info;
goto _LL2651; _LL2651: _temp2650= _temp2648.targs; goto _LL2591;} else{ goto
_LL2592;} _LL2592: if(( unsigned int) _temp2578 >  4u?*(( int*) _temp2578) == 
Cyc_Absyn_PointerType: 0){ _LL2656: _temp2655=(( struct Cyc_Absyn_PointerType_struct*)
_temp2578)->f1; _LL2666: _temp2665=( void*) _temp2655.elt_typ; goto _LL2664;
_LL2664: _temp2663=( void*) _temp2655.rgn_typ; goto _LL2662; _LL2662: _temp2661=
_temp2655.nullable; goto _LL2660; _LL2660: _temp2659= _temp2655.tq; goto _LL2658;
_LL2658: _temp2657= _temp2655.bounds; goto _LL2593;} else{ goto _LL2594;}
_LL2594: if(( unsigned int) _temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_IntType:
0){ goto _LL2595;} else{ goto _LL2596;} _LL2596: if( _temp2578 == ( void*) Cyc_Absyn_FloatType){
goto _LL2597;} else{ goto _LL2598;} _LL2598: if( _temp2578 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2599;} else{ goto _LL2600;} _LL2600: if(( unsigned int) _temp2578 >  4u?*((
int*) _temp2578) ==  Cyc_Absyn_ArrayType: 0){ _LL2672: _temp2671=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2578)->f1; goto _LL2670; _LL2670:
_temp2669=(( struct Cyc_Absyn_ArrayType_struct*) _temp2578)->f2; goto _LL2668;
_LL2668: _temp2667=(( struct Cyc_Absyn_ArrayType_struct*) _temp2578)->f3; goto
_LL2601;} else{ goto _LL2602;} _LL2602: if(( unsigned int) _temp2578 >  4u?*((
int*) _temp2578) ==  Cyc_Absyn_FnType: 0){ _LL2674: _temp2673=(( struct Cyc_Absyn_FnType_struct*)
_temp2578)->f1; _LL2691: _temp2690= _temp2673.tvars; _temp2692=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2578)->f1).tvars; goto _LL2688; _LL2688:
_temp2687= _temp2673.effect; _temp2689=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2578)->f1).effect; goto _LL2686; _LL2686: _temp2685=( void*) _temp2673.ret_typ;
goto _LL2684; _LL2684: _temp2683= _temp2673.args; goto _LL2682; _LL2682:
_temp2681= _temp2673.c_varargs; goto _LL2680; _LL2680: _temp2679= _temp2673.cyc_varargs;
goto _LL2678; _LL2678: _temp2677= _temp2673.rgn_po; goto _LL2676; _LL2676:
_temp2675= _temp2673.attributes; goto _LL2603;} else{ goto _LL2604;} _LL2604:
if(( unsigned int) _temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_TupleType:
0){ _LL2694: _temp2693=(( struct Cyc_Absyn_TupleType_struct*) _temp2578)->f1;
goto _LL2605;} else{ goto _LL2606;} _LL2606: if(( unsigned int) _temp2578 >  4u?*((
int*) _temp2578) ==  Cyc_Absyn_AnonStructType: 0){ _LL2696: _temp2695=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2578)->f1; goto _LL2607;} else{ goto
_LL2608;} _LL2608: if(( unsigned int) _temp2578 >  4u?*(( int*) _temp2578) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2698: _temp2697=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2578)->f1; goto _LL2609;} else{ goto _LL2610;} _LL2610: if(( unsigned int)
_temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_StructType: 0){ _LL2705:
_temp2704=(( struct Cyc_Absyn_StructType_struct*) _temp2578)->f1; goto _LL2703;
_LL2703: _temp2702=(( struct Cyc_Absyn_StructType_struct*) _temp2578)->f2; goto
_LL2700; _LL2700: _temp2699=(( struct Cyc_Absyn_StructType_struct*) _temp2578)->f3;
_temp2701=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2578)->f3; goto _LL2611;} else{ goto _LL2612;} _LL2612: if(( unsigned int)
_temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_UnionType: 0){ _LL2712:
_temp2711=(( struct Cyc_Absyn_UnionType_struct*) _temp2578)->f1; goto _LL2710;
_LL2710: _temp2709=(( struct Cyc_Absyn_UnionType_struct*) _temp2578)->f2; goto
_LL2707; _LL2707: _temp2706=(( struct Cyc_Absyn_UnionType_struct*) _temp2578)->f3;
_temp2708=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2578)->f3; goto _LL2613;} else{ goto _LL2614;} _LL2614: if(( unsigned int)
_temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_TypedefType: 0){ _LL2720:
_temp2719=(( struct Cyc_Absyn_TypedefType_struct*) _temp2578)->f1; goto _LL2717;
_LL2717: _temp2716=(( struct Cyc_Absyn_TypedefType_struct*) _temp2578)->f2;
_temp2718=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2578)->f2; goto _LL2714; _LL2714: _temp2713=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2578)->f3; _temp2715=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2578)->f3; goto _LL2615;} else{ goto _LL2616;} _LL2616: if( _temp2578 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2617;} else{ goto _LL2618;} _LL2618: if((
unsigned int) _temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2722: _temp2721=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2578)->f1; goto _LL2619;} else{ goto _LL2620;} _LL2620: if(( unsigned int)
_temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_AccessEff: 0){ _LL2724:
_temp2723=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2578)->f1; goto
_LL2621;} else{ goto _LL2622;} _LL2622: if(( unsigned int) _temp2578 >  4u?*((
int*) _temp2578) ==  Cyc_Absyn_RgnsEff: 0){ _LL2726: _temp2725=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2578)->f1; goto _LL2623;} else{ goto _LL2624;}
_LL2624: if(( unsigned int) _temp2578 >  4u?*(( int*) _temp2578) ==  Cyc_Absyn_JoinEff:
0){ _LL2728: _temp2727=(( struct Cyc_Absyn_JoinEff_struct*) _temp2578)->f1; goto
_LL2625;} else{ goto _LL2579;} _LL2581: goto _LL2579; _LL2583: if(* _temp2631 == 
0){* _temp2631=({ struct Cyc_Core_Opt* _temp2729=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2729->v=( void*) expected_kind;
_temp2729;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2628=({ struct Cyc_Core_Opt* _temp2730=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2730->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2730;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2731= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2628=({ struct Cyc_Core_Opt* _temp2732=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2732->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2733=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2733[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2734; _temp2734.tag= Cyc_Absyn_VarType;
_temp2734.f1= _temp2731; _temp2734;}); _temp2733;})); _temp2732;}); _temp2632=
_temp2731; goto _LL2585;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2579; _LL2585: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2632->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2735=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2735[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2736; _temp2736.tag= Cyc_Absyn_Eq_constr;
_temp2736.f1=( void*) expected_kind; _temp2736;}); _temp2735;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2632); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2632); goto _LL2579;}
_LL2587: if(* _temp2636 ==  0){ struct _handler_cons _temp2737; _push_handler(&
_temp2737);{ int _temp2739= 0; if( setjmp( _temp2737.handler)){ _temp2739= 1;}
if( ! _temp2739){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2637);* _temp2636=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2738=( void*) _exn_thrown; void* _temp2741= _temp2738; _LL2743:
if( _temp2741 ==  Cyc_Dict_Absent){ goto _LL2744;} else{ goto _LL2745;} _LL2745:
goto _LL2746; _LL2744:({ struct Cyc_Std_String_pa_struct _temp2748; _temp2748.tag=
Cyc_Std_String_pa; _temp2748.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2637);{ void* _temp2747[ 1u]={& _temp2748}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u), _tag_arr( _temp2747, sizeof( void*), 1u));}});
return cvtenv; _LL2746:( void) _throw( _temp2741); _LL2742:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2636);* _temp2637=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2579;} _LL2589:{ void*
_temp2749=* _temp2647; struct Cyc_Absyn_UnknownTunionInfo _temp2755; int
_temp2757; struct _tuple1* _temp2759; struct Cyc_Absyn_Tuniondecl** _temp2761;
struct Cyc_Absyn_Tuniondecl* _temp2763; _LL2751: if(*(( int*) _temp2749) ==  Cyc_Absyn_UnknownTunion){
_LL2756: _temp2755=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2749)->f1;
_LL2760: _temp2759= _temp2755.name; goto _LL2758; _LL2758: _temp2757= _temp2755.is_xtunion;
goto _LL2752;} else{ goto _LL2753;} _LL2753: if(*(( int*) _temp2749) ==  Cyc_Absyn_KnownTunion){
_LL2762: _temp2761=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2749)->f1;
_temp2763=* _temp2761; goto _LL2754;} else{ goto _LL2750;} _LL2752: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp2764; _push_handler(& _temp2764);{ int
_temp2766= 0; if( setjmp( _temp2764.handler)){ _temp2766= 1;} if( ! _temp2766){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp2759);; _pop_handler();} else{
void* _temp2765=( void*) _exn_thrown; void* _temp2768= _temp2765; _LL2770: if(
_temp2768 ==  Cyc_Dict_Absent){ goto _LL2771;} else{ goto _LL2772;} _LL2772:
goto _LL2773; _LL2771:({ struct Cyc_Std_String_pa_struct _temp2775; _temp2775.tag=
Cyc_Std_String_pa; _temp2775.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2759);{ void* _temp2774[ 1u]={& _temp2775}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type [x]tunion %s",
sizeof( unsigned char), 26u), _tag_arr( _temp2774, sizeof( void*), 1u));}});
return cvtenv; _LL2773:( void) _throw( _temp2768); _LL2769:;}}} if((* tudp)->is_xtunion
!=  _temp2757){({ struct Cyc_Std_String_pa_struct _temp2777; _temp2777.tag= Cyc_Std_String_pa;
_temp2777.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2759);{ void*
_temp2776[ 1u]={& _temp2777}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2776, sizeof( void*), 1u));}});}*
_temp2647=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2778=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2778[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2779; _temp2779.tag= Cyc_Absyn_KnownTunion;
_temp2779.f1= tudp; _temp2779;}); _temp2778;}); _temp2763=* tudp; goto _LL2754;}
_LL2754: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2641);{ struct Cyc_List_List* tvs= _temp2763->tvs; for( 0; _temp2643 !=  0?
tvs !=  0: 0;( _temp2643=(( struct Cyc_List_List*) _check_null( _temp2643))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2643))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2643
!=  0){({ struct Cyc_Std_String_pa_struct _temp2781; _temp2781.tag= Cyc_Std_String_pa;
_temp2781.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2763->name);{
void* _temp2780[ 1u]={& _temp2781}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2780, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2783; _temp2783.tag= Cyc_Std_String_pa;
_temp2783.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2763->name);{
void* _temp2782[ 1u]={& _temp2783}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2782, sizeof( void*), 1u));}});}
goto _LL2750;} _LL2750:;} goto _LL2579; _LL2591:{ void* _temp2784=* _temp2654;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2790; int _temp2792; struct _tuple1*
_temp2794; struct _tuple1* _temp2796; struct Cyc_Absyn_Tunionfield* _temp2798;
struct Cyc_Absyn_Tuniondecl* _temp2800; _LL2786: if(*(( int*) _temp2784) ==  Cyc_Absyn_UnknownTunionfield){
_LL2791: _temp2790=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2784)->f1;
_LL2797: _temp2796= _temp2790.tunion_name; goto _LL2795; _LL2795: _temp2794=
_temp2790.field_name; goto _LL2793; _LL2793: _temp2792= _temp2790.is_xtunion;
goto _LL2787;} else{ goto _LL2788;} _LL2788: if(*(( int*) _temp2784) ==  Cyc_Absyn_KnownTunionfield){
_LL2801: _temp2800=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2784)->f1;
goto _LL2799; _LL2799: _temp2798=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2784)->f2; goto _LL2789;} else{ goto _LL2785;} _LL2787: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2802;
_push_handler(& _temp2802);{ int _temp2804= 0; if( setjmp( _temp2802.handler)){
_temp2804= 1;} if( ! _temp2804){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2796);; _pop_handler();} else{ void* _temp2803=( void*) _exn_thrown; void*
_temp2806= _temp2803; _LL2808: if( _temp2806 ==  Cyc_Dict_Absent){ goto _LL2809;}
else{ goto _LL2810;} _LL2810: goto _LL2811; _LL2809:({ struct Cyc_Std_String_pa_struct
_temp2813; _temp2813.tag= Cyc_Std_String_pa; _temp2813.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2796);{ void* _temp2812[ 1u]={& _temp2813}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2812, sizeof( void*), 1u));}}); return cvtenv; _LL2811:( void) _throw(
_temp2806); _LL2807:;}}}{ struct _handler_cons _temp2814; _push_handler(&
_temp2814);{ int _temp2816= 0; if( setjmp( _temp2814.handler)){ _temp2816= 1;}
if( ! _temp2816){{ void* _temp2817= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2794); struct Cyc_Absyn_Tunionfield* _temp2823; struct Cyc_Absyn_Tuniondecl*
_temp2825; _LL2819: if(*(( int*) _temp2817) ==  Cyc_Tcenv_TunionRes){ _LL2826:
_temp2825=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2817)->f1; goto _LL2824;
_LL2824: _temp2823=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2817)->f2; goto
_LL2820;} else{ goto _LL2821;} _LL2821: goto _LL2822; _LL2820: tuf= _temp2823;
tud= _temp2825; if( tud->is_xtunion !=  _temp2792){({ struct Cyc_Std_String_pa_struct
_temp2828; _temp2828.tag= Cyc_Std_String_pa; _temp2828.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2796);{ void* _temp2827[ 1u]={& _temp2828}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2827, sizeof( void*), 1u));}});} goto
_LL2818; _LL2822:({ struct Cyc_Std_String_pa_struct _temp2831; _temp2831.tag=
Cyc_Std_String_pa; _temp2831.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2796);{ struct Cyc_Std_String_pa_struct _temp2830; _temp2830.tag= Cyc_Std_String_pa;
_temp2830.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2794);{ void*
_temp2829[ 2u]={& _temp2830,& _temp2831}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2829, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2832= cvtenv; _npop_handler( 0u); return _temp2832;}
_LL2818:;}; _pop_handler();} else{ void* _temp2815=( void*) _exn_thrown; void*
_temp2834= _temp2815; _LL2836: if( _temp2834 ==  Cyc_Dict_Absent){ goto _LL2837;}
else{ goto _LL2838;} _LL2838: goto _LL2839; _LL2837:({ struct Cyc_Std_String_pa_struct
_temp2842; _temp2842.tag= Cyc_Std_String_pa; _temp2842.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2796);{ struct Cyc_Std_String_pa_struct _temp2841;
_temp2841.tag= Cyc_Std_String_pa; _temp2841.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2794);{ void* _temp2840[ 2u]={& _temp2841,& _temp2842}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2840, sizeof( void*), 2u));}}}); return cvtenv; _LL2839:( void)
_throw( _temp2834); _LL2835:;}}}* _temp2654=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2843=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2843[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2844; _temp2844.tag= Cyc_Absyn_KnownTunionfield; _temp2844.f1= tud;
_temp2844.f2= tuf; _temp2844;}); _temp2843;}); _temp2800= tud; _temp2798= tuf;
goto _LL2789;} _LL2789: { struct Cyc_List_List* tvs= _temp2800->tvs; for( 0;
_temp2650 !=  0? tvs !=  0: 0;( _temp2650=(( struct Cyc_List_List*) _check_null(
_temp2650))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2650))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2650
!=  0){({ struct Cyc_Std_String_pa_struct _temp2847; _temp2847.tag= Cyc_Std_String_pa;
_temp2847.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2798->name);{
struct Cyc_Std_String_pa_struct _temp2846; _temp2846.tag= Cyc_Std_String_pa;
_temp2846.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2800->name);{
void* _temp2845[ 2u]={& _temp2846,& _temp2847}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2845, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2850; _temp2850.tag= Cyc_Std_String_pa;
_temp2850.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2798->name);{
struct Cyc_Std_String_pa_struct _temp2849; _temp2849.tag= Cyc_Std_String_pa;
_temp2849.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2800->name);{
void* _temp2848[ 2u]={& _temp2849,& _temp2850}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2848, sizeof( void*), 2u));}}});}
goto _LL2785;} _LL2785:;} goto _LL2579; _LL2593: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2665); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2663);{ void* _temp2851=( void*)(
Cyc_Absyn_compress_conref( _temp2657))->v; void* _temp2857; struct Cyc_Absyn_Exp*
_temp2859; _LL2853: if(( unsigned int) _temp2851 >  1u?*(( int*) _temp2851) == 
Cyc_Absyn_Eq_constr: 0){ _LL2858: _temp2857=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2851)->f1; if(( unsigned int) _temp2857 >  1u?*(( int*) _temp2857) ==  Cyc_Absyn_Upper_b:
0){ _LL2860: _temp2859=(( struct Cyc_Absyn_Upper_b_struct*) _temp2857)->f1; goto
_LL2854;} else{ goto _LL2855;}} else{ goto _LL2855;} _LL2855: goto _LL2856;
_LL2854: if( ! Cyc_Tcutil_is_const_exp( te, _temp2859)){({ void* _temp2861[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2861, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2859); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2859)){({ void*
_temp2862[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2862, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2859); goto _LL2852; _LL2856: goto _LL2852; _LL2852:;} goto _LL2579;
_LL2595: goto _LL2579; _LL2597: goto _LL2579; _LL2599: goto _LL2579; _LL2601:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2671); if( _temp2667 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2667))){({ void* _temp2863[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2863, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2667)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2667))){({ void* _temp2864[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2864, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2667)); goto _LL2579; _LL2603: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2675 !=  0; _temp2675=(( struct Cyc_List_List*)
_check_null( _temp2675))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2675))->hd)){({ struct Cyc_Std_String_pa_struct _temp2866;
_temp2866.tag= Cyc_Std_String_pa; _temp2866.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2675))->hd);{ void* _temp2865[
1u]={& _temp2866}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2865, sizeof( void*), 1u));}});}{
void* _temp2867=( void*)(( struct Cyc_List_List*) _check_null( _temp2675))->hd;
int _temp2879; int _temp2881; void* _temp2883; _LL2869: if( _temp2867 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2870;} else{ goto _LL2871;} _LL2871: if(
_temp2867 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL2872;} else{ goto _LL2873;}
_LL2873: if( _temp2867 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL2874;} else{
goto _LL2875;} _LL2875: if(( unsigned int) _temp2867 >  16u?*(( int*) _temp2867)
==  Cyc_Absyn_Format_att: 0){ _LL2884: _temp2883=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2867)->f1; goto _LL2882; _LL2882: _temp2881=(( struct Cyc_Absyn_Format_att_struct*)
_temp2867)->f2; goto _LL2880; _LL2880: _temp2879=(( struct Cyc_Absyn_Format_att_struct*)
_temp2867)->f3; goto _LL2876;} else{ goto _LL2877;} _LL2877: goto _LL2878;
_LL2870: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2868;
_LL2872: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2868; _LL2874:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2868; _LL2876:
if( ! seen_format){ seen_format= 1; ft= _temp2883; fmt_desc_arg= _temp2881;
fmt_arg_start= _temp2879;} else{({ void* _temp2885[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2885, sizeof( void*), 0u));});} goto _LL2868; _LL2878: goto
_LL2868; _LL2868:;}} if( num_convs >  1){({ void* _temp2886[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2886, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2692);{ struct Cyc_List_List* b=* _temp2692; for( 0; b !=  0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2887=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2893; _LL2889: if(( unsigned int) _temp2887 >  1u?*(( int*) _temp2887)
==  Cyc_Absyn_Eq_constr: 0){ _LL2894: _temp2893=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2887)->f1; if( _temp2893 == ( void*) Cyc_Absyn_MemKind){ goto _LL2890;}
else{ goto _LL2891;}} else{ goto _LL2891;} _LL2891: goto _LL2892; _LL2890:({
struct Cyc_Std_String_pa_struct _temp2896; _temp2896.tag= Cyc_Std_String_pa;
_temp2896.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2895[ 1u]={& _temp2896}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2895, sizeof( void*), 1u));}}); goto
_LL2888; _LL2892: goto _LL2888; _LL2888:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2897=({
struct Cyc_Tcutil_CVTEnv _temp3056; _temp3056.kind_env= cvtenv.kind_env;
_temp3056.free_vars= 0; _temp3056.free_evars= 0; _temp3056.generalize_evars=
cvtenv.generalize_evars; _temp3056.fn_result= 1; _temp3056;}); _temp2897= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2897,( void*) Cyc_Absyn_MemKind, _temp2685); _temp2897.fn_result=
0;{ struct Cyc_List_List* a= _temp2683; for( 0; a !=  0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ _temp2897= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2897,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( a))->hd)).f3);}} if( _temp2679 !=  0){ if( _temp2681){({ void*
_temp2898[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2898, sizeof( void*), 0u));});}{ int _temp2901; void* _temp2903; struct Cyc_Absyn_Tqual
_temp2905; struct Cyc_Core_Opt* _temp2907; struct Cyc_Absyn_VarargInfo _temp2899=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp2679)); _LL2908: _temp2907=
_temp2899.name; goto _LL2906; _LL2906: _temp2905= _temp2899.tq; goto _LL2904;
_LL2904: _temp2903=( void*) _temp2899.type; goto _LL2902; _LL2902: _temp2901=
_temp2899.inject; goto _LL2900; _LL2900: _temp2897= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2897,( void*) Cyc_Absyn_MemKind, _temp2903); if( _temp2901){ void*
_temp2909= Cyc_Tcutil_compress( _temp2903); struct Cyc_Absyn_TunionInfo
_temp2915; void* _temp2917; void* _temp2919; _LL2911: if(( unsigned int)
_temp2909 >  4u?*(( int*) _temp2909) ==  Cyc_Absyn_TunionType: 0){ _LL2916:
_temp2915=(( struct Cyc_Absyn_TunionType_struct*) _temp2909)->f1; _LL2920:
_temp2919=( void*) _temp2915.tunion_info; if(*(( int*) _temp2919) ==  Cyc_Absyn_KnownTunion){
goto _LL2918;} else{ goto _LL2913;} _LL2918: _temp2917=( void*) _temp2915.rgn;
goto _LL2912;} else{ goto _LL2913;} _LL2913: goto _LL2914; _LL2912: goto _LL2910;
_LL2914:({ void* _temp2921[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u), _tag_arr( _temp2921, sizeof( void*), 0u));}); goto
_LL2910; _LL2910:;}}} if( seen_format){ int _temp2922=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2683); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg > 
_temp2922)? 1: fmt_arg_start <  0)? 1:( _temp2679 ==  0? fmt_arg_start !=  0: 0))?
1:( _temp2679 !=  0? fmt_arg_start !=  _temp2922 +  1: 0)){({ void* _temp2923[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2923, sizeof( void*), 0u));});} else{ void* _temp2926;
struct _tuple2 _temp2924=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp2683, fmt_desc_arg -  1); _LL2927: _temp2926= _temp2924.f3;
goto _LL2925; _LL2925:{ void* _temp2928= Cyc_Tcutil_compress( _temp2926); struct
Cyc_Absyn_PtrInfo _temp2934; struct Cyc_Absyn_Conref* _temp2936; void* _temp2938;
_LL2930: if(( unsigned int) _temp2928 >  4u?*(( int*) _temp2928) ==  Cyc_Absyn_PointerType:
0){ _LL2935: _temp2934=(( struct Cyc_Absyn_PointerType_struct*) _temp2928)->f1;
_LL2939: _temp2938=( void*) _temp2934.elt_typ; goto _LL2937; _LL2937: _temp2936=
_temp2934.bounds; goto _LL2931;} else{ goto _LL2932;} _LL2932: goto _LL2933;
_LL2931:{ struct _tuple8 _temp2941=({ struct _tuple8 _temp2940; _temp2940.f1=
Cyc_Tcutil_compress( _temp2938); _temp2940.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2936); _temp2940;}); void* _temp2947; void* _temp2949; void* _temp2951;
void* _temp2953; _LL2943: _LL2950: _temp2949= _temp2941.f1; if(( unsigned int)
_temp2949 >  4u?*(( int*) _temp2949) ==  Cyc_Absyn_IntType: 0){ _LL2954:
_temp2953=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2949)->f1; if(
_temp2953 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2952;} else{ goto _LL2945;}
_LL2952: _temp2951=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2949)->f2;
if( _temp2951 == ( void*) Cyc_Absyn_B1){ goto _LL2948;} else{ goto _LL2945;}}
else{ goto _LL2945;} _LL2948: _temp2947= _temp2941.f2; if( _temp2947 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2944;} else{ goto _LL2945;} _LL2945: goto _LL2946;
_LL2944: goto _LL2942; _LL2946:({ void* _temp2955[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp2955, sizeof( void*), 0u));}); goto _LL2942; _LL2942:;} goto
_LL2929; _LL2933:({ void* _temp2956[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2956, sizeof( void*), 0u));}); goto
_LL2929; _LL2929:;} if( fmt_arg_start !=  0){ void* _temp2957= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2679))->type); int
problem;{ void* _temp2958= ft; _LL2960: if( _temp2958 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL2961;} else{ goto _LL2962;} _LL2962: if( _temp2958 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL2963;} else{ goto _LL2959;} _LL2961:{ void* _temp2964= _temp2957; struct
Cyc_Absyn_TunionInfo _temp2970; void* _temp2972; struct Cyc_Absyn_Tuniondecl**
_temp2974; struct Cyc_Absyn_Tuniondecl* _temp2976; _LL2966: if(( unsigned int)
_temp2964 >  4u?*(( int*) _temp2964) ==  Cyc_Absyn_TunionType: 0){ _LL2971:
_temp2970=(( struct Cyc_Absyn_TunionType_struct*) _temp2964)->f1; _LL2973:
_temp2972=( void*) _temp2970.tunion_info; if(*(( int*) _temp2972) ==  Cyc_Absyn_KnownTunion){
_LL2975: _temp2974=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2972)->f1;
_temp2976=* _temp2974; goto _LL2967;} else{ goto _LL2968;}} else{ goto _LL2968;}
_LL2968: goto _LL2969; _LL2967: problem= Cyc_Absyn_qvar_cmp( _temp2976->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL2965; _LL2969: problem= 1; goto
_LL2965; _LL2965:;} goto _LL2959; _LL2963:{ void* _temp2977= _temp2957; struct
Cyc_Absyn_TunionInfo _temp2983; void* _temp2985; struct Cyc_Absyn_Tuniondecl**
_temp2987; struct Cyc_Absyn_Tuniondecl* _temp2989; _LL2979: if(( unsigned int)
_temp2977 >  4u?*(( int*) _temp2977) ==  Cyc_Absyn_TunionType: 0){ _LL2984:
_temp2983=(( struct Cyc_Absyn_TunionType_struct*) _temp2977)->f1; _LL2986:
_temp2985=( void*) _temp2983.tunion_info; if(*(( int*) _temp2985) ==  Cyc_Absyn_KnownTunion){
_LL2988: _temp2987=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2985)->f1;
_temp2989=* _temp2987; goto _LL2980;} else{ goto _LL2981;}} else{ goto _LL2981;}
_LL2981: goto _LL2982; _LL2980: problem= Cyc_Absyn_qvar_cmp( _temp2989->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL2978; _LL2982: problem= 1; goto
_LL2978; _LL2978:;} goto _LL2959; _LL2959:;} if( problem){({ void* _temp2990[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp2990, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2677; for( 0; rpo !=  0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp2993; void* _temp2994; void*
_temp2996; struct _tuple8* _temp2991=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp2993=* _temp2991; _LL2997: _temp2996= _temp2993.f1;
goto _LL2995; _LL2995: _temp2994= _temp2993.f2; goto _LL2992; _LL2992: _temp2897=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2897,( void*) Cyc_Absyn_RgnKind,
_temp2996); _temp2897= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2897,( void*)
Cyc_Absyn_RgnKind, _temp2994);}} if(* _temp2689 !=  0){ _temp2897= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2897,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2689))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2897.free_vars; for(
0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void*
_temp2998=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3006; void*
_temp3008; _LL3000: if(( unsigned int) _temp2998 >  1u?*(( int*) _temp2998) == 
Cyc_Absyn_Eq_constr: 0){ _LL3007: _temp3006=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2998)->f1; if( _temp3006 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3001;}
else{ goto _LL3002;}} else{ goto _LL3002;} _LL3002: if(( unsigned int) _temp2998
>  1u?*(( int*) _temp2998) ==  Cyc_Absyn_Eq_constr: 0){ _LL3009: _temp3008=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2998)->f1; if( _temp3008 == (
void*) Cyc_Absyn_EffKind){ goto _LL3003;} else{ goto _LL3004;}} else{ goto
_LL3004;} _LL3004: goto _LL3005; _LL3001: effect=({ struct Cyc_List_List*
_temp3010=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3010->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3011=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3011[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3012; _temp3012.tag= Cyc_Absyn_AccessEff;
_temp3012.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3013=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3013[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3014; _temp3014.tag= Cyc_Absyn_VarType;
_temp3014.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3014;}); _temp3013;})); _temp3012;}); _temp3011;})); _temp3010->tl= effect;
_temp3010;}); goto _LL2999; _LL3003: effect=({ struct Cyc_List_List* _temp3015=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3015->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3016=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3016[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3017; _temp3017.tag= Cyc_Absyn_VarType; _temp3017.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3017;}); _temp3016;})); _temp3015->tl= effect; _temp3015;}); goto _LL2999;
_LL3005: effect=({ struct Cyc_List_List* _temp3018=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3018->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3019=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3019[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3020; _temp3020.tag= Cyc_Absyn_RgnsEff; _temp3020.f1=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3021=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3021[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3022; _temp3022.tag= Cyc_Absyn_VarType; _temp3022.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3022;}); _temp3021;})); _temp3020;}); _temp3019;})); _temp3018->tl= effect;
_temp3018;}); goto _LL2999; _LL2999:;}} effect= Cyc_List_imp_rev( effect);{
struct Cyc_List_List* ts= _temp2897.free_evars; for( 0; ts !=  0; ts=(( struct
Cyc_List_List*) _check_null( ts))->tl){ void* _temp3023= Cyc_Tcutil_typ_kind((
void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _LL3025: if( _temp3023
== ( void*) Cyc_Absyn_RgnKind){ goto _LL3026;} else{ goto _LL3027;} _LL3027: if(
_temp3023 == ( void*) Cyc_Absyn_EffKind){ goto _LL3028;} else{ goto _LL3029;}
_LL3029: goto _LL3030; _LL3026: effect=({ struct Cyc_List_List* _temp3031=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3031->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3032=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3032[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3033; _temp3033.tag= Cyc_Absyn_AccessEff;
_temp3033.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3033;}); _temp3032;})); _temp3031->tl= effect; _temp3031;}); goto _LL3024;
_LL3028: effect=({ struct Cyc_List_List* _temp3034=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3034->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3034->tl= effect; _temp3034;});
goto _LL3024; _LL3030: effect=({ struct Cyc_List_List* _temp3035=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3035->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3036=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3036[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3037; _temp3037.tag= Cyc_Absyn_RgnsEff; _temp3037.f1=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _temp3037;});
_temp3036;})); _temp3035->tl= effect; _temp3035;}); goto _LL3024; _LL3024:;}}*
_temp2689=({ struct Cyc_Core_Opt* _temp3038=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3038->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3039=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3039[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3040; _temp3040.tag= Cyc_Absyn_JoinEff;
_temp3040.f1= Cyc_List_imp_rev( effect); _temp3040;}); _temp3039;})); _temp3038;});}}
if(* _temp2692 !=  0){ struct Cyc_List_List* bs=* _temp2692; for( 0; bs !=  0;
bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3041= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3042=( void*) _temp3041->v; void*
_temp3050; _LL3044: if( _temp3042 == ( void*) Cyc_Absyn_No_constr){ goto _LL3045;}
else{ goto _LL3046;} _LL3046: if(( unsigned int) _temp3042 >  1u?*(( int*)
_temp3042) ==  Cyc_Absyn_Eq_constr: 0){ _LL3051: _temp3050=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3042)->f1; if( _temp3050 == ( void*) Cyc_Absyn_MemKind){ goto _LL3047;}
else{ goto _LL3048;}} else{ goto _LL3048;} _LL3048: goto _LL3049; _LL3045:({
struct Cyc_Std_String_pa_struct _temp3053; _temp3053.tag= Cyc_Std_String_pa;
_temp3053.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp3052[ 1u]={& _temp3053}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3052, sizeof( void*), 1u));}}); goto
_LL3047; _LL3047:( void*)( _temp3041->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3054=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3054[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3055; _temp3055.tag= Cyc_Absyn_Eq_constr;
_temp3055.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3055;}); _temp3054;})));
goto _LL3043; _LL3049: goto _LL3043; _LL3043:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2897.kind_env,* _temp2692); _temp2897.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2897.free_vars,* _temp2692);{ struct Cyc_List_List* tvs= _temp2897.free_vars;
for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2897.free_evars; for( 0; evs !=  0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2579;}}
_LL2605: for( 0; _temp2693 !=  0; _temp2693=(( struct Cyc_List_List*)
_check_null( _temp2693))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp2693))->hd)).f2);} goto _LL2579; _LL2607:{ struct
_RegionHandle _temp3057= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp3057; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2695 !=  0; _temp2695=(( struct Cyc_List_List*) _check_null( _temp2695))->tl){
struct Cyc_Absyn_Structfield _temp3060; struct Cyc_List_List* _temp3061; struct
Cyc_Absyn_Exp* _temp3063; void* _temp3065; struct Cyc_Absyn_Tqual _temp3067;
struct _tagged_arr* _temp3069; struct Cyc_Absyn_Structfield* _temp3058=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2695))->hd;
_temp3060=* _temp3058; _LL3070: _temp3069= _temp3060.name; goto _LL3068; _LL3068:
_temp3067= _temp3060.tq; goto _LL3066; _LL3066: _temp3065=( void*) _temp3060.type;
goto _LL3064; _LL3064: _temp3063= _temp3060.width; goto _LL3062; _LL3062:
_temp3061= _temp3060.attributes; goto _LL3059; _LL3059: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3069)){({ struct Cyc_Std_String_pa_struct _temp3072; _temp3072.tag= Cyc_Std_String_pa;
_temp3072.f1=( struct _tagged_arr)* _temp3069;{ void* _temp3071[ 1u]={&
_temp3072}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3071, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3069, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3073=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3073->hd=( void*)
_temp3069; _temp3073->tl= prev_fields; _temp3073;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3065); Cyc_Tcutil_check_bitfield(
loc, te, _temp3065, _temp3063, _temp3069); Cyc_Tcutil_check_field_atts( loc,
_temp3069, _temp3061);}}; _pop_region( sprev_rgn);} goto _LL2579; _LL2609:{
struct _RegionHandle _temp3074= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3074; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2697 !=  0; _temp2697=(( struct Cyc_List_List*) _check_null( _temp2697))->tl){
struct Cyc_Absyn_Structfield _temp3077; struct Cyc_List_List* _temp3078; struct
Cyc_Absyn_Exp* _temp3080; void* _temp3082; struct Cyc_Absyn_Tqual _temp3084;
struct _tagged_arr* _temp3086; struct Cyc_Absyn_Structfield* _temp3075=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2697))->hd;
_temp3077=* _temp3075; _LL3087: _temp3086= _temp3077.name; goto _LL3085; _LL3085:
_temp3084= _temp3077.tq; goto _LL3083; _LL3083: _temp3082=( void*) _temp3077.type;
goto _LL3081; _LL3081: _temp3080= _temp3077.width; goto _LL3079; _LL3079:
_temp3078= _temp3077.attributes; goto _LL3076; _LL3076: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3086)){({ struct Cyc_Std_String_pa_struct _temp3089; _temp3089.tag= Cyc_Std_String_pa;
_temp3089.f1=( struct _tagged_arr)* _temp3086;{ void* _temp3088[ 1u]={&
_temp3089}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3088, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3086, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3090=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3090->hd=( void*)
_temp3086; _temp3090->tl= prev_fields; _temp3090;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3082); if( ! Cyc_Tcutil_bits_only(
_temp3082)){({ struct Cyc_Std_String_pa_struct _temp3092; _temp3092.tag= Cyc_Std_String_pa;
_temp3092.f1=( struct _tagged_arr)* _temp3086;{ void* _temp3091[ 1u]={&
_temp3092}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3091, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3082, _temp3080, _temp3086); Cyc_Tcutil_check_field_atts(
loc, _temp3086, _temp3078);}}; _pop_region( uprev_rgn);} goto _LL2579; _LL2611:
if( _temp2704 ==  0){ if(* _temp2701 ==  0){({ void* _temp3093[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3093, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2701));;}} if(*
_temp2701 ==  0){ struct _handler_cons _temp3094; _push_handler(& _temp3094);{
int _temp3096= 0; if( setjmp( _temp3094.handler)){ _temp3096= 1;} if( !
_temp3096){* _temp2701=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2704));; _pop_handler();} else{
void* _temp3095=( void*) _exn_thrown; void* _temp3098= _temp3095; _LL3100: if(
_temp3098 ==  Cyc_Dict_Absent){ goto _LL3101;} else{ goto _LL3102;} _LL3102:
goto _LL3103; _LL3101:({ struct Cyc_Std_String_pa_struct _temp3105; _temp3105.tag=
Cyc_Std_String_pa; _temp3105.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2704));{ void* _temp3104[ 1u]={& _temp3105};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type struct %s", sizeof( unsigned char),
23u), _tag_arr( _temp3104, sizeof( void*), 1u));}}); return cvtenv; _LL3103:(
void) _throw( _temp3098); _LL3099:;}}}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2701)); if( sd->name !=  0){*((
struct _tuple1*) _check_null( _temp2704))=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2702); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3109; _temp3109.tag= Cyc_Std_Int_pa; _temp3109.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3108; _temp3108.tag= Cyc_Std_Int_pa;
_temp3108.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3107; _temp3107.tag= Cyc_Std_String_pa; _temp3107.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2704));{ void*
_temp3106[ 3u]={& _temp3107,& _temp3108,& _temp3109}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3106, sizeof( void*), 3u));}}}});} for( 0;
_temp2702 !=  0;( _temp2702=(( struct Cyc_List_List*) _check_null( _temp2702))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2702))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2579;}}
_LL2613: if( _temp2711 ==  0){({ void* _temp3110[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp3110,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2708 ==  0){ struct
_handler_cons _temp3111; _push_handler(& _temp3111);{ int _temp3113= 0; if(
setjmp( _temp3111.handler)){ _temp3113= 1;} if( ! _temp3113){* _temp2708=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2711));; _pop_handler();} else{ void* _temp3112=(
void*) _exn_thrown; void* _temp3115= _temp3112; _LL3117: if( _temp3115 ==  Cyc_Dict_Absent){
goto _LL3118;} else{ goto _LL3119;} _LL3119: goto _LL3120; _LL3118:({ struct Cyc_Std_String_pa_struct
_temp3122; _temp3122.tag= Cyc_Std_String_pa; _temp3122.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2711));{ void*
_temp3121[ 1u]={& _temp3122}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type union %s",
sizeof( unsigned char), 22u), _tag_arr( _temp3121, sizeof( void*), 1u));}});
return cvtenv; _LL3120:( void) _throw( _temp3115); _LL3116:;}}}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(* _temp2708)); if( ud->name != 
0){*(( struct _tuple1*) _check_null( _temp2711))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs= Cyc_List_length( _temp2709); if( lvs != 
largs){({ struct Cyc_Std_Int_pa_struct _temp3126; _temp3126.tag= Cyc_Std_Int_pa;
_temp3126.f1=( int)(( unsigned int) largs);{ struct Cyc_Std_Int_pa_struct
_temp3125; _temp3125.tag= Cyc_Std_Int_pa; _temp3125.f1=( int)(( unsigned int)
lvs);{ struct Cyc_Std_String_pa_struct _temp3124; _temp3124.tag= Cyc_Std_String_pa;
_temp3124.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2711));{ void* _temp3123[ 3u]={& _temp3124,& _temp3125,&
_temp3126}; Cyc_Tcutil_terr( loc, _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u), _tag_arr( _temp3123, sizeof( void*), 3u));}}}});}
for( 0; _temp2709 !=  0;( _temp2709=(( struct Cyc_List_List*) _check_null(
_temp2709))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2709))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2579;}}
_LL2615: { struct Cyc_List_List* targs=* _temp2718; struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3127; _push_handler(& _temp3127);{ int _temp3129=
0; if( setjmp( _temp3127.handler)){ _temp3129= 1;} if( ! _temp3129){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2719);; _pop_handler();} else{ void* _temp3128=( void*)
_exn_thrown; void* _temp3131= _temp3128; _LL3133: if( _temp3131 ==  Cyc_Dict_Absent){
goto _LL3134;} else{ goto _LL3135;} _LL3135: goto _LL3136; _LL3134:({ struct Cyc_Std_String_pa_struct
_temp3138; _temp3138.tag= Cyc_Std_String_pa; _temp3138.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2719);{ void* _temp3137[ 1u]={& _temp3138}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3137, sizeof( void*), 1u));}}); return cvtenv; _LL3136:( void) _throw(
_temp3131); _LL3132:;}}} _temp2719[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3139=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3139->hd=( void*)({ struct
_tuple5* _temp3140=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3140->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3140->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3140;}); _temp3139->tl= inst; _temp3139;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp3142; _temp3142.tag= Cyc_Std_String_pa; _temp3142.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2719);{ void* _temp3141[ 1u]={& _temp3142}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3141, sizeof( void*), 1u));}});} if( tvs !=  0){ struct Cyc_List_List*
hidden_ts= 0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({
struct Cyc_List_List* _temp3143=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3143->hd=( void*) e; _temp3143->tl= hidden_ts;
_temp3143;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k, e);
inst=({ struct Cyc_List_List* _temp3144=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3144->hd=( void*)({ struct _tuple5*
_temp3145=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp3145->f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3145->f2= e; _temp3145;}); _temp3144->tl= inst; _temp3144;});}* _temp2718=
Cyc_List_imp_append( targs, Cyc_List_imp_rev( hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute(
inst,( void*) td->defn);* _temp2715=({ struct Cyc_Core_Opt* _temp3146=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3146->v=( void*)
new_typ; _temp3146;}); goto _LL2579;}}} _LL2617: goto _LL2579; _LL2619:
_temp2723= _temp2721; goto _LL2621; _LL2621: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2723); goto _LL2579; _LL2623:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2725); goto _LL2579; _LL2625: for( 0; _temp2727 !=  0; _temp2727=(( struct
Cyc_List_List*) _check_null( _temp2727))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2727))->hd);} goto _LL2579; _LL2579:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){({ struct Cyc_Std_String_pa_struct
_temp3150; _temp3150.tag= Cyc_Std_String_pa; _temp3150.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( expected_kind);{ struct Cyc_Std_String_pa_struct
_temp3149; _temp3149.tag= Cyc_Std_String_pa; _temp3149.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ struct Cyc_Std_String_pa_struct
_temp3148; _temp3148.tag= Cyc_Std_String_pa; _temp3148.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp3147[ 3u]={& _temp3148,& _temp3149,&
_temp3150}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3147, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3151= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3166; _temp3166.kind_env= kind_env; _temp3166.free_vars= 0; _temp3166.free_evars=
0; _temp3166.generalize_evars= generalize_evars; _temp3166.fn_result= 0;
_temp3166;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3151.free_vars;
for( 0; vs !=  0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ _temp3151.kind_env=
Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( vs))->hd);}}{ struct Cyc_List_List* evs= _temp3151.free_evars; for(
0; evs !=  0; evs=(( struct Cyc_List_List*) _check_null( evs))->tl){ void*
_temp3152= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null(
evs))->hd); struct Cyc_Core_Opt* _temp3158; struct Cyc_Core_Opt** _temp3160;
_LL3154: if(( unsigned int) _temp3152 >  4u?*(( int*) _temp3152) ==  Cyc_Absyn_Evar:
0){ _LL3159: _temp3158=(( struct Cyc_Absyn_Evar_struct*) _temp3152)->f4;
_temp3160=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3152)->f4;
goto _LL3155;} else{ goto _LL3156;} _LL3156: goto _LL3157; _LL3155: if(*
_temp3160 ==  0){* _temp3160=({ struct Cyc_Core_Opt* _temp3161=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3161->v=( void*) kind_env;
_temp3161;});} else{ struct Cyc_List_List* _temp3162=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3160))->v; struct Cyc_List_List*
_temp3163= 0; for( 0; _temp3162 !=  0; _temp3162=(( struct Cyc_List_List*)
_check_null( _temp3162))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3162))->hd)){ _temp3163=({ struct Cyc_List_List* _temp3164=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3164->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3162))->hd);
_temp3164->tl= _temp3163; _temp3164;});}}* _temp3160=({ struct Cyc_Core_Opt*
_temp3165=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3165->v=( void*) _temp3163; _temp3165;});} goto _LL3153; _LL3157: goto
_LL3153; _LL3153:;}} return _temp3151;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ int
generalize_evars= Cyc_Tcutil_is_function_type( t); struct Cyc_Tcutil_CVTEnv
_temp3167= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
generalize_evars, t); struct Cyc_List_List* _temp3168= _temp3167.free_vars;
struct Cyc_List_List* _temp3169= _temp3167.free_evars;{ struct Cyc_List_List* x=
_temp3168; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3170=( void*) c->v;
void* _temp3178; _LL3172: if( _temp3170 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3173;} else{ goto _LL3174;} _LL3174: if(( unsigned int) _temp3170 >  1u?*((
int*) _temp3170) ==  Cyc_Absyn_Eq_constr: 0){ _LL3179: _temp3178=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3170)->f1; if( _temp3178 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3175;} else{ goto _LL3176;}} else{ goto _LL3176;}
_LL3176: goto _LL3177; _LL3173: goto _LL3175; _LL3175:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3180=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3180[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3181; _temp3181.tag= Cyc_Absyn_Eq_constr;
_temp3181.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3181;}); _temp3180;})));
goto _LL3171; _LL3177: goto _LL3171; _LL3171:;}} if( _temp3168 !=  0? 1:
_temp3169 !=  0){{ void* _temp3182= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3188; struct Cyc_List_List* _temp3190; struct Cyc_List_List* _temp3192;
struct Cyc_Absyn_VarargInfo* _temp3194; int _temp3196; struct Cyc_List_List*
_temp3198; void* _temp3200; struct Cyc_Core_Opt* _temp3202; struct Cyc_List_List*
_temp3204; struct Cyc_List_List** _temp3206; _LL3184: if(( unsigned int)
_temp3182 >  4u?*(( int*) _temp3182) ==  Cyc_Absyn_FnType: 0){ _LL3189:
_temp3188=(( struct Cyc_Absyn_FnType_struct*) _temp3182)->f1; _LL3205: _temp3204=
_temp3188.tvars; _temp3206=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3182)->f1).tvars; goto _LL3203; _LL3203: _temp3202= _temp3188.effect; goto
_LL3201; _LL3201: _temp3200=( void*) _temp3188.ret_typ; goto _LL3199; _LL3199:
_temp3198= _temp3188.args; goto _LL3197; _LL3197: _temp3196= _temp3188.c_varargs;
goto _LL3195; _LL3195: _temp3194= _temp3188.cyc_varargs; goto _LL3193; _LL3193:
_temp3192= _temp3188.rgn_po; goto _LL3191; _LL3191: _temp3190= _temp3188.attributes;
goto _LL3185;} else{ goto _LL3186;} _LL3186: goto _LL3187; _LL3185: if(*
_temp3206 ==  0){* _temp3206= _temp3168; _temp3168= 0;} goto _LL3183; _LL3187:
goto _LL3183; _LL3183:;} if( _temp3168 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3208; _temp3208.tag= Cyc_Std_String_pa; _temp3208.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3168))->hd)->name;{
void* _temp3207[ 1u]={& _temp3208}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp3207, sizeof( void*), 1u));}});}
if( _temp3169 !=  0){ for( 0; _temp3169 !=  0; _temp3169=(( struct Cyc_List_List*)
_check_null( _temp3169))->tl){ void* e=( void*)(( struct Cyc_List_List*)
_check_null( _temp3169))->hd; void* _temp3209= Cyc_Tcutil_typ_kind( e); _LL3211:
if( _temp3209 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3212;} else{ goto _LL3213;}
_LL3213: if( _temp3209 == ( void*) Cyc_Absyn_EffKind){ goto _LL3214;} else{ goto
_LL3215;} _LL3215: goto _LL3216; _LL3212: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp3217[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp3217, sizeof( void*), 0u));});} goto _LL3210; _LL3214: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp3218[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp3218, sizeof( void*), 0u));});} goto _LL3210; _LL3216:({ struct Cyc_Std_String_pa_struct
_temp3221; _temp3221.tag= Cyc_Std_String_pa; _temp3221.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3220;
_temp3220.tag= Cyc_Std_String_pa; _temp3220.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp3219[ 2u]={& _temp3220,& _temp3221}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3219, sizeof( void*), 2u));}}}); goto
_LL3210; _LL3210:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3222= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3228; struct Cyc_List_List* _temp3230; struct
Cyc_List_List* _temp3232; struct Cyc_Absyn_VarargInfo* _temp3234; int _temp3236;
struct Cyc_List_List* _temp3238; void* _temp3240; struct Cyc_Core_Opt* _temp3242;
struct Cyc_List_List* _temp3244; _LL3224: if(( unsigned int) _temp3222 >  4u?*((
int*) _temp3222) ==  Cyc_Absyn_FnType: 0){ _LL3229: _temp3228=(( struct Cyc_Absyn_FnType_struct*)
_temp3222)->f1; _LL3245: _temp3244= _temp3228.tvars; goto _LL3243; _LL3243:
_temp3242= _temp3228.effect; goto _LL3241; _LL3241: _temp3240=( void*) _temp3228.ret_typ;
goto _LL3239; _LL3239: _temp3238= _temp3228.args; goto _LL3237; _LL3237:
_temp3236= _temp3228.c_varargs; goto _LL3235; _LL3235: _temp3234= _temp3228.cyc_varargs;
goto _LL3233; _LL3233: _temp3232= _temp3228.rgn_po; goto _LL3231; _LL3231:
_temp3230= _temp3228.attributes; goto _LL3225;} else{ goto _LL3226;} _LL3226:
goto _LL3227; _LL3225: fd->tvs= _temp3244; fd->effect= _temp3242; goto _LL3223;
_LL3227:({ void* _temp3246[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3246, sizeof( void*), 0u));});
return; _LL3223:;}{ struct _RegionHandle _temp3247= _new_region(); struct
_RegionHandle* r=& _temp3247; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3248=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3248->v=(
void*) t; _temp3248;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3249=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3250= Cyc_Tcutil_remove_bound_tvars( _temp3249.free_vars,
bound_tvars); struct Cyc_List_List* _temp3251= _temp3249.free_evars;{ struct Cyc_List_List*
fs= _temp3250; for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3252=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name;({ struct Cyc_Std_String_pa_struct _temp3255;
_temp3255.tag= Cyc_Std_String_pa; _temp3255.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp3254; _temp3254.tag= Cyc_Std_String_pa;
_temp3254.f1=( struct _tagged_arr)* _temp3252;{ void* _temp3253[ 2u]={&
_temp3254,& _temp3255}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3253, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3251 !=  0: 0){ for( 0; _temp3251 !=  0; _temp3251=((
struct Cyc_List_List*) _check_null( _temp3251))->tl){ void* e=( void*)(( struct
Cyc_List_List*) _check_null( _temp3251))->hd; void* _temp3256= Cyc_Tcutil_typ_kind(
e); _LL3258: if( _temp3256 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3259;} else{
goto _LL3260;} _LL3260: if( _temp3256 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3261;} else{ goto _LL3262;} _LL3262: goto _LL3263; _LL3259: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3264[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3264, sizeof( void*), 0u));});}
goto _LL3257; _LL3261: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3265=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3265[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3266; _temp3266.tag= Cyc_Absyn_JoinEff;
_temp3266.f1= 0; _temp3266;}); _temp3265;}))){({ void* _temp3267[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3267, sizeof( void*), 0u));});}
goto _LL3257; _LL3263:({ struct Cyc_Std_String_pa_struct _temp3270; _temp3270.tag=
Cyc_Std_String_pa; _temp3270.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3269; _temp3269.tag= Cyc_Std_String_pa;
_temp3269.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3268[
2u]={& _temp3269,& _temp3270}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3268, sizeof( void*), 2u));}}});
goto _LL3257; _LL3257:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
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
struct Cyc_Std_String_pa_struct _temp3273; _temp3273.tag= Cyc_Std_String_pa;
_temp3273.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_Std_String_pa_struct _temp3272; _temp3272.tag=
Cyc_Std_String_pa; _temp3272.f1=( struct _tagged_arr) msg;{ void* _temp3271[ 2u]={&
_temp3272,& _temp3273}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3271, sizeof( void*), 2u));}}});}}}} static
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
unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List* _temp3274=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3274->hd=( void*)({
struct _tuple17* _temp3275=( struct _tuple17*) _cycalloc( sizeof( struct
_tuple17)); _temp3275->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3275->f2= 0; _temp3275;}); _temp3274->tl=
fields; _temp3274;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3278;
void* _temp3279; struct Cyc_List_List* _temp3281; struct _tuple18* _temp3276=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3278=*
_temp3276; _LL3282: _temp3281= _temp3278.f1; goto _LL3280; _LL3280: _temp3279=
_temp3278.f2; goto _LL3277; _LL3277: if( _temp3281 ==  0){ struct Cyc_List_List*
_temp3283= fields; for( 0; _temp3283 !=  0; _temp3283=(( struct Cyc_List_List*)
_check_null( _temp3283))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3283))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3283))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3284=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3284->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3285=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3285[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3286; _temp3286.tag= Cyc_Absyn_FieldName;
_temp3286.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3283))->hd)).f1)->name; _temp3286;}); _temp3285;})); _temp3284->tl= 0;
_temp3284;}); ans=({ struct Cyc_List_List* _temp3287=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3287->hd=( void*)({
struct _tuple19* _temp3288=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3288->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3283))->hd)).f1; _temp3288->f2= _temp3279; _temp3288;});
_temp3287->tl= ans; _temp3287;}); break;}} if( _temp3283 ==  0){({ void*
_temp3289[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3289, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3281))->tl !=  0){({ void*
_temp3290[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3290, sizeof( void*), 0u));});}
else{ void* _temp3291=( void*)(( struct Cyc_List_List*) _check_null( _temp3281))->hd;
struct _tagged_arr* _temp3297; _LL3293: if(*(( int*) _temp3291) ==  Cyc_Absyn_ArrayElement){
goto _LL3294;} else{ goto _LL3295;} _LL3295: if(*(( int*) _temp3291) ==  Cyc_Absyn_FieldName){
_LL3298: _temp3297=(( struct Cyc_Absyn_FieldName_struct*) _temp3291)->f1; goto
_LL3296;} else{ goto _LL3292;} _LL3294:({ void* _temp3299[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3299, sizeof( void*), 0u));}); goto _LL3292;
_LL3296: { struct Cyc_List_List* _temp3300= fields; for( 0; _temp3300 !=  0;
_temp3300=(( struct Cyc_List_List*) _check_null( _temp3300))->tl){ if( Cyc_Std_zstrptrcmp(
_temp3297,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3300))->hd)).f1)->name) ==  0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3300))->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3302;
_temp3302.tag= Cyc_Std_String_pa; _temp3302.f1=( struct _tagged_arr)* _temp3297;{
void* _temp3301[ 1u]={& _temp3302}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3301, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3300))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3303=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3303->hd=( void*)({ struct _tuple19*
_temp3304=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3304->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3300))->hd)).f1; _temp3304->f2= _temp3279; _temp3304;}); _temp3303->tl= ans;
_temp3303;}); break;}} if( _temp3300 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3306; _temp3306.tag= Cyc_Std_String_pa; _temp3306.f1=( struct _tagged_arr)*
_temp3297;{ void* _temp3305[ 1u]={& _temp3306}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3305, sizeof( void*), 1u));}});}
goto _LL3292;} _LL3292:;}}} for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3307[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3307, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3308= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3314; struct Cyc_Absyn_Conref* _temp3316; void* _temp3318;
_LL3310: if(( unsigned int) _temp3308 >  4u?*(( int*) _temp3308) ==  Cyc_Absyn_PointerType:
0){ _LL3315: _temp3314=(( struct Cyc_Absyn_PointerType_struct*) _temp3308)->f1;
_LL3319: _temp3318=( void*) _temp3314.elt_typ; goto _LL3317; _LL3317: _temp3316=
_temp3314.bounds; goto _LL3311;} else{ goto _LL3312;} _LL3312: goto _LL3313;
_LL3311: { struct Cyc_Absyn_Conref* _temp3320= Cyc_Absyn_compress_conref(
_temp3316); void* _temp3321=( void*)( Cyc_Absyn_compress_conref( _temp3320))->v;
void* _temp3329; _LL3323: if(( unsigned int) _temp3321 >  1u?*(( int*) _temp3321)
==  Cyc_Absyn_Eq_constr: 0){ _LL3330: _temp3329=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3321)->f1; if( _temp3329 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3324;}
else{ goto _LL3325;}} else{ goto _LL3325;} _LL3325: if( _temp3321 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3326;} else{ goto _LL3327;} _LL3327: goto _LL3328;
_LL3324:* elt_typ_dest= _temp3318; return 1; _LL3326:( void*)( _temp3320->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3331=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3331[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3332; _temp3332.tag= Cyc_Absyn_Eq_constr;
_temp3332.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3332;}); _temp3331;})));*
elt_typ_dest= _temp3318; return 1; _LL3328: return 0; _LL3322:;} _LL3313: return
0; _LL3309:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3515;
_temp3515.f1= 0; _temp3515.f2=( void*) Cyc_Absyn_HeapRgn; _temp3515;}); void*
_temp3333=( void*) e->r; void* _temp3347; struct _tuple1* _temp3349; struct
_tagged_arr* _temp3351; struct Cyc_Absyn_Exp* _temp3353; struct _tagged_arr*
_temp3355; struct Cyc_Absyn_Exp* _temp3357; struct Cyc_Absyn_Exp* _temp3359;
struct Cyc_Absyn_Exp* _temp3361; struct Cyc_Absyn_Exp* _temp3363; _LL3335: if(*((
int*) _temp3333) ==  Cyc_Absyn_Var_e){ _LL3350: _temp3349=(( struct Cyc_Absyn_Var_e_struct*)
_temp3333)->f1; goto _LL3348; _LL3348: _temp3347=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3333)->f2; goto _LL3336;} else{ goto _LL3337;} _LL3337: if(*(( int*)
_temp3333) ==  Cyc_Absyn_StructMember_e){ _LL3354: _temp3353=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3333)->f1; goto _LL3352; _LL3352: _temp3351=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3333)->f2; goto _LL3338;} else{ goto _LL3339;} _LL3339: if(*(( int*)
_temp3333) ==  Cyc_Absyn_StructArrow_e){ _LL3358: _temp3357=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3333)->f1; goto _LL3356; _LL3356: _temp3355=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3333)->f2; goto _LL3340;} else{ goto _LL3341;} _LL3341: if(*(( int*)
_temp3333) ==  Cyc_Absyn_Deref_e){ _LL3360: _temp3359=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3333)->f1; goto _LL3342;} else{ goto _LL3343;} _LL3343: if(*(( int*)
_temp3333) ==  Cyc_Absyn_Subscript_e){ _LL3364: _temp3363=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3333)->f1; goto _LL3362; _LL3362: _temp3361=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3333)->f2; goto _LL3344;} else{ goto _LL3345;} _LL3345: goto _LL3346;
_LL3336: { void* _temp3365= _temp3347; struct Cyc_Absyn_Vardecl* _temp3379;
struct Cyc_Absyn_Vardecl* _temp3381; struct Cyc_Absyn_Vardecl* _temp3383; struct
Cyc_Absyn_Vardecl* _temp3385; _LL3367: if( _temp3365 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3368;} else{ goto _LL3369;} _LL3369: if(( unsigned int) _temp3365 >  1u?*((
int*) _temp3365) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3370;} else{ goto _LL3371;}
_LL3371: if(( unsigned int) _temp3365 >  1u?*(( int*) _temp3365) ==  Cyc_Absyn_Global_b:
0){ _LL3380: _temp3379=(( struct Cyc_Absyn_Global_b_struct*) _temp3365)->f1;
goto _LL3372;} else{ goto _LL3373;} _LL3373: if(( unsigned int) _temp3365 >  1u?*((
int*) _temp3365) ==  Cyc_Absyn_Local_b: 0){ _LL3382: _temp3381=(( struct Cyc_Absyn_Local_b_struct*)
_temp3365)->f1; goto _LL3374;} else{ goto _LL3375;} _LL3375: if(( unsigned int)
_temp3365 >  1u?*(( int*) _temp3365) ==  Cyc_Absyn_Pat_b: 0){ _LL3384: _temp3383=((
struct Cyc_Absyn_Pat_b_struct*) _temp3365)->f1; goto _LL3376;} else{ goto
_LL3377;} _LL3377: if(( unsigned int) _temp3365 >  1u?*(( int*) _temp3365) == 
Cyc_Absyn_Param_b: 0){ _LL3386: _temp3385=(( struct Cyc_Absyn_Param_b_struct*)
_temp3365)->f1; goto _LL3378;} else{ goto _LL3366;} _LL3368: return bogus_ans;
_LL3370: return({ struct _tuple7 _temp3387; _temp3387.f1= 0; _temp3387.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3387;}); _LL3372: { void* _temp3388= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3390: if((
unsigned int) _temp3388 >  4u?*(( int*) _temp3388) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3391;} else{ goto _LL3392;} _LL3392: goto _LL3393; _LL3391: return({
struct _tuple7 _temp3394; _temp3394.f1= 1; _temp3394.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3394;}); _LL3393: return({ struct _tuple7 _temp3395; _temp3395.f1=(
_temp3379->tq).q_const; _temp3395.f2=( void*) Cyc_Absyn_HeapRgn; _temp3395;});
_LL3389:;} _LL3374: { void* _temp3396= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3398: if(( unsigned int) _temp3396 >  4u?*(( int*)
_temp3396) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3399;} else{ goto _LL3400;}
_LL3400: goto _LL3401; _LL3399: return({ struct _tuple7 _temp3402; _temp3402.f1=
1; _temp3402.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3381->rgn))->v;
_temp3402;}); _LL3401: return({ struct _tuple7 _temp3403; _temp3403.f1=(
_temp3381->tq).q_const; _temp3403.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3381->rgn))->v; _temp3403;}); _LL3397:;} _LL3376: _temp3385=
_temp3383; goto _LL3378; _LL3378: return({ struct _tuple7 _temp3404; _temp3404.f1=(
_temp3385->tq).q_const; _temp3404.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3385->rgn))->v; _temp3404;}); _LL3366:;} _LL3338: { void*
_temp3405= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3353->topt))->v); struct Cyc_List_List* _temp3417; struct Cyc_List_List*
_temp3419; struct Cyc_Absyn_Structdecl** _temp3421; struct Cyc_Absyn_Structdecl*
_temp3423; struct Cyc_Absyn_Uniondecl** _temp3424; struct Cyc_Absyn_Uniondecl*
_temp3426; _LL3407: if(( unsigned int) _temp3405 >  4u?*(( int*) _temp3405) == 
Cyc_Absyn_AnonStructType: 0){ _LL3418: _temp3417=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3405)->f1; goto _LL3408;} else{ goto _LL3409;} _LL3409: if(( unsigned int)
_temp3405 >  4u?*(( int*) _temp3405) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3420:
_temp3419=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3405)->f1; goto
_LL3410;} else{ goto _LL3411;} _LL3411: if(( unsigned int) _temp3405 >  4u?*((
int*) _temp3405) ==  Cyc_Absyn_StructType: 0){ _LL3422: _temp3421=(( struct Cyc_Absyn_StructType_struct*)
_temp3405)->f3; if( _temp3421 ==  0){ goto _LL3413;} else{ _temp3423=* _temp3421;
goto _LL3412;}} else{ goto _LL3413;} _LL3413: if(( unsigned int) _temp3405 >  4u?*((
int*) _temp3405) ==  Cyc_Absyn_UnionType: 0){ _LL3425: _temp3424=(( struct Cyc_Absyn_UnionType_struct*)
_temp3405)->f3; if( _temp3424 ==  0){ goto _LL3415;} else{ _temp3426=* _temp3424;
goto _LL3414;}} else{ goto _LL3415;} _LL3415: goto _LL3416; _LL3408: _temp3419=
_temp3417; goto _LL3410; _LL3410: { struct Cyc_Absyn_Structfield* _temp3427= Cyc_Absyn_lookup_field(
_temp3419, _temp3351); if( _temp3427 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3427))->width !=  0: 0){ return({ struct _tuple7 _temp3428;
_temp3428.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3427))->tq).q_const;
_temp3428.f2=( Cyc_Tcutil_addressof_props( te, _temp3353)).f2; _temp3428;});}
return bogus_ans;} _LL3412: { struct Cyc_Absyn_Structfield* _temp3429= Cyc_Absyn_lookup_struct_field(
_temp3423, _temp3351); if( _temp3429 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3429))->width !=  0: 0){ return({ struct _tuple7 _temp3430;
_temp3430.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3429))->tq).q_const;
_temp3430.f2=( Cyc_Tcutil_addressof_props( te, _temp3353)).f2; _temp3430;});}
return bogus_ans;} _LL3414: { struct Cyc_Absyn_Structfield* _temp3431= Cyc_Absyn_lookup_union_field(
_temp3426, _temp3351); if( _temp3431 !=  0){ return({ struct _tuple7 _temp3432;
_temp3432.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3431))->tq).q_const;
_temp3432.f2=( Cyc_Tcutil_addressof_props( te, _temp3353)).f2; _temp3432;});}
goto _LL3416;} _LL3416: return bogus_ans; _LL3406:;} _LL3340: { void* _temp3433=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3357->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3439; void* _temp3441; void* _temp3443; _LL3435:
if(( unsigned int) _temp3433 >  4u?*(( int*) _temp3433) ==  Cyc_Absyn_PointerType:
0){ _LL3440: _temp3439=(( struct Cyc_Absyn_PointerType_struct*) _temp3433)->f1;
_LL3444: _temp3443=( void*) _temp3439.elt_typ; goto _LL3442; _LL3442: _temp3441=(
void*) _temp3439.rgn_typ; goto _LL3436;} else{ goto _LL3437;} _LL3437: goto
_LL3438; _LL3436: { void* _temp3445= Cyc_Tcutil_compress( _temp3443); struct Cyc_List_List*
_temp3457; struct Cyc_List_List* _temp3459; struct Cyc_Absyn_Structdecl**
_temp3461; struct Cyc_Absyn_Structdecl* _temp3463; struct Cyc_Absyn_Uniondecl**
_temp3464; struct Cyc_Absyn_Uniondecl* _temp3466; _LL3447: if(( unsigned int)
_temp3445 >  4u?*(( int*) _temp3445) ==  Cyc_Absyn_AnonStructType: 0){ _LL3458:
_temp3457=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3445)->f1; goto
_LL3448;} else{ goto _LL3449;} _LL3449: if(( unsigned int) _temp3445 >  4u?*((
int*) _temp3445) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3460: _temp3459=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3445)->f1; goto _LL3450;} else{ goto
_LL3451;} _LL3451: if(( unsigned int) _temp3445 >  4u?*(( int*) _temp3445) == 
Cyc_Absyn_StructType: 0){ _LL3462: _temp3461=(( struct Cyc_Absyn_StructType_struct*)
_temp3445)->f3; if( _temp3461 ==  0){ goto _LL3453;} else{ _temp3463=* _temp3461;
goto _LL3452;}} else{ goto _LL3453;} _LL3453: if(( unsigned int) _temp3445 >  4u?*((
int*) _temp3445) ==  Cyc_Absyn_UnionType: 0){ _LL3465: _temp3464=(( struct Cyc_Absyn_UnionType_struct*)
_temp3445)->f3; if( _temp3464 ==  0){ goto _LL3455;} else{ _temp3466=* _temp3464;
goto _LL3454;}} else{ goto _LL3455;} _LL3455: goto _LL3456; _LL3448: _temp3459=
_temp3457; goto _LL3450; _LL3450: { struct Cyc_Absyn_Structfield* _temp3467= Cyc_Absyn_lookup_field(
_temp3459, _temp3355); if( _temp3467 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3467))->width !=  0: 0){ return({ struct _tuple7 _temp3468;
_temp3468.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3467))->tq).q_const;
_temp3468.f2= _temp3441; _temp3468;});} return bogus_ans;} _LL3452: { struct Cyc_Absyn_Structfield*
_temp3469= Cyc_Absyn_lookup_struct_field( _temp3463, _temp3355); if( _temp3469
!=  0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3469))->width !=  0: 0){
return({ struct _tuple7 _temp3470; _temp3470.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3469))->tq).q_const; _temp3470.f2= _temp3441; _temp3470;});}
return bogus_ans;} _LL3454: { struct Cyc_Absyn_Structfield* _temp3471= Cyc_Absyn_lookup_union_field(
_temp3466, _temp3355); if( _temp3471 !=  0){ return({ struct _tuple7 _temp3472;
_temp3472.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3471))->tq).q_const;
_temp3472.f2= _temp3441; _temp3472;});} return bogus_ans;} _LL3456: return
bogus_ans; _LL3446:;} _LL3438: return bogus_ans; _LL3434:;} _LL3342: { void*
_temp3473= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3359->topt))->v); struct Cyc_Absyn_PtrInfo _temp3479; struct Cyc_Absyn_Tqual
_temp3481; void* _temp3483; _LL3475: if(( unsigned int) _temp3473 >  4u?*(( int*)
_temp3473) ==  Cyc_Absyn_PointerType: 0){ _LL3480: _temp3479=(( struct Cyc_Absyn_PointerType_struct*)
_temp3473)->f1; _LL3484: _temp3483=( void*) _temp3479.rgn_typ; goto _LL3482;
_LL3482: _temp3481= _temp3479.tq; goto _LL3476;} else{ goto _LL3477;} _LL3477:
goto _LL3478; _LL3476: return({ struct _tuple7 _temp3485; _temp3485.f1=
_temp3481.q_const; _temp3485.f2= _temp3483; _temp3485;}); _LL3478: return
bogus_ans; _LL3474:;} _LL3344: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3363->topt))->v); void* _temp3486= t; struct
Cyc_Absyn_Tqual _temp3496; struct Cyc_List_List* _temp3498; struct Cyc_Absyn_PtrInfo
_temp3500; struct Cyc_Absyn_Conref* _temp3502; struct Cyc_Absyn_Tqual _temp3504;
void* _temp3506; void* _temp3508; _LL3488: if(( unsigned int) _temp3486 >  4u?*((
int*) _temp3486) ==  Cyc_Absyn_ArrayType: 0){ _LL3497: _temp3496=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3486)->f2; goto _LL3489;} else{ goto _LL3490;} _LL3490: if(( unsigned int)
_temp3486 >  4u?*(( int*) _temp3486) ==  Cyc_Absyn_TupleType: 0){ _LL3499:
_temp3498=(( struct Cyc_Absyn_TupleType_struct*) _temp3486)->f1; goto _LL3491;}
else{ goto _LL3492;} _LL3492: if(( unsigned int) _temp3486 >  4u?*(( int*)
_temp3486) ==  Cyc_Absyn_PointerType: 0){ _LL3501: _temp3500=(( struct Cyc_Absyn_PointerType_struct*)
_temp3486)->f1; _LL3509: _temp3508=( void*) _temp3500.elt_typ; goto _LL3507;
_LL3507: _temp3506=( void*) _temp3500.rgn_typ; goto _LL3505; _LL3505: _temp3504=
_temp3500.tq; goto _LL3503; _LL3503: _temp3502= _temp3500.bounds; goto _LL3493;}
else{ goto _LL3494;} _LL3494: goto _LL3495; _LL3489: return({ struct _tuple7
_temp3510; _temp3510.f1= _temp3496.q_const; _temp3510.f2=( Cyc_Tcutil_addressof_props(
te, _temp3363)).f2; _temp3510;}); _LL3491: { struct _tuple4* _temp3511= Cyc_Absyn_lookup_tuple_field(
_temp3498,( int) Cyc_Evexp_eval_const_uint_exp( _temp3361)); if( _temp3511 !=  0){
return({ struct _tuple7 _temp3512; _temp3512.f1=((*(( struct _tuple4*)
_check_null( _temp3511))).f1).q_const; _temp3512.f2=( Cyc_Tcutil_addressof_props(
te, _temp3363)).f2; _temp3512;});} return bogus_ans;} _LL3493: return({ struct
_tuple7 _temp3513; _temp3513.f1= _temp3504.q_const; _temp3513.f2= _temp3506;
_temp3513;}); _LL3495: return bogus_ans; _LL3487:;} _LL3346:({ void* _temp3514[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3514, sizeof( void*), 0u));});
return bogus_ans; _LL3334:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3516= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3522; void* _temp3524; _LL3518: if((
unsigned int) _temp3516 >  4u?*(( int*) _temp3516) ==  Cyc_Absyn_ArrayType: 0){
_LL3525: _temp3524=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3516)->f1;
goto _LL3523; _LL3523: _temp3522=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3516)->f2; goto _LL3519;} else{ goto _LL3520;} _LL3520: goto _LL3521;
_LL3519: { void* _temp3528; struct _tuple7 _temp3526= Cyc_Tcutil_addressof_props(
te, e); _LL3529: _temp3528= _temp3526.f2; goto _LL3527; _LL3527: return Cyc_Absyn_atb_typ(
_temp3524, _temp3528, _temp3522,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3530=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3530[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3531; _temp3531.tag= Cyc_Absyn_Upper_b;
_temp3531.f1= e; _temp3531;}); _temp3530;}));} _LL3521: return e_typ; _LL3517:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3532=(
void*) b->v; void* _temp3540; void* _temp3542; struct Cyc_Absyn_Exp* _temp3544;
_LL3534: if(( unsigned int) _temp3532 >  1u?*(( int*) _temp3532) ==  Cyc_Absyn_Eq_constr:
0){ _LL3541: _temp3540=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3532)->f1;
if( _temp3540 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3535;} else{ goto
_LL3536;}} else{ goto _LL3536;} _LL3536: if(( unsigned int) _temp3532 >  1u?*((
int*) _temp3532) ==  Cyc_Absyn_Eq_constr: 0){ _LL3543: _temp3542=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3532)->f1; if(( unsigned int) _temp3542
>  1u?*(( int*) _temp3542) ==  Cyc_Absyn_Upper_b: 0){ _LL3545: _temp3544=((
struct Cyc_Absyn_Upper_b_struct*) _temp3542)->f1; goto _LL3537;} else{ goto
_LL3538;}} else{ goto _LL3538;} _LL3538: goto _LL3539; _LL3535: return; _LL3537:
if( Cyc_Evexp_eval_const_uint_exp( _temp3544) <=  i){({ void* _temp3546[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3546, sizeof( void*), 0u));});} return;
_LL3539:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3547=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3547[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3548; _temp3548.tag= Cyc_Absyn_Eq_constr;
_temp3548.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3549=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3549[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3550; _temp3550.tag= Cyc_Absyn_Upper_b;
_temp3550.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3550;}); _temp3549;}));
_temp3548;}); _temp3547;}))); return; _LL3533:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp3551=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp3557; struct
Cyc_Absyn_Exp* _temp3559; _LL3553: if(( unsigned int) _temp3551 >  1u?*(( int*)
_temp3551) ==  Cyc_Absyn_Eq_constr: 0){ _LL3558: _temp3557=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3551)->f1; if(( unsigned int) _temp3557 >  1u?*(( int*) _temp3557) ==  Cyc_Absyn_Upper_b:
0){ _LL3560: _temp3559=(( struct Cyc_Absyn_Upper_b_struct*) _temp3557)->f1; goto
_LL3554;} else{ goto _LL3555;}} else{ goto _LL3555;} _LL3555: goto _LL3556;
_LL3554: return Cyc_Evexp_eval_const_uint_exp( _temp3559) ==  1; _LL3556: return
0; _LL3552:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp3561= Cyc_Tcutil_compress(
t); void* _temp3587; struct Cyc_List_List* _temp3589; struct Cyc_Absyn_Structdecl**
_temp3591; struct Cyc_List_List* _temp3593; struct Cyc_Absyn_Uniondecl**
_temp3595; struct Cyc_List_List* _temp3597; struct Cyc_List_List* _temp3599;
struct Cyc_List_List* _temp3601; _LL3563: if( _temp3561 == ( void*) Cyc_Absyn_VoidType){
goto _LL3564;} else{ goto _LL3565;} _LL3565: if(( unsigned int) _temp3561 >  4u?*((
int*) _temp3561) ==  Cyc_Absyn_IntType: 0){ goto _LL3566;} else{ goto _LL3567;}
_LL3567: if( _temp3561 == ( void*) Cyc_Absyn_FloatType){ goto _LL3568;} else{
goto _LL3569;} _LL3569: if( _temp3561 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3570;} else{ goto _LL3571;} _LL3571: if(( unsigned int) _temp3561 >  4u?*((
int*) _temp3561) ==  Cyc_Absyn_EnumType: 0){ goto _LL3572;} else{ goto _LL3573;}
_LL3573: if(( unsigned int) _temp3561 >  4u?*(( int*) _temp3561) ==  Cyc_Absyn_ArrayType:
0){ _LL3588: _temp3587=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3561)->f1;
goto _LL3574;} else{ goto _LL3575;} _LL3575: if(( unsigned int) _temp3561 >  4u?*((
int*) _temp3561) ==  Cyc_Absyn_TupleType: 0){ _LL3590: _temp3589=(( struct Cyc_Absyn_TupleType_struct*)
_temp3561)->f1; goto _LL3576;} else{ goto _LL3577;} _LL3577: if(( unsigned int)
_temp3561 >  4u?*(( int*) _temp3561) ==  Cyc_Absyn_StructType: 0){ _LL3594:
_temp3593=(( struct Cyc_Absyn_StructType_struct*) _temp3561)->f2; goto _LL3592;
_LL3592: _temp3591=(( struct Cyc_Absyn_StructType_struct*) _temp3561)->f3; goto
_LL3578;} else{ goto _LL3579;} _LL3579: if(( unsigned int) _temp3561 >  4u?*((
int*) _temp3561) ==  Cyc_Absyn_UnionType: 0){ _LL3598: _temp3597=(( struct Cyc_Absyn_UnionType_struct*)
_temp3561)->f2; goto _LL3596; _LL3596: _temp3595=(( struct Cyc_Absyn_UnionType_struct*)
_temp3561)->f3; goto _LL3580;} else{ goto _LL3581;} _LL3581: if(( unsigned int)
_temp3561 >  4u?*(( int*) _temp3561) ==  Cyc_Absyn_AnonStructType: 0){ _LL3600:
_temp3599=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3561)->f1; goto
_LL3582;} else{ goto _LL3583;} _LL3583: if(( unsigned int) _temp3561 >  4u?*((
int*) _temp3561) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3602: _temp3601=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3561)->f1; goto _LL3584;} else{ goto
_LL3585;} _LL3585: goto _LL3586; _LL3564: goto _LL3566; _LL3566: goto _LL3568;
_LL3568: goto _LL3570; _LL3570: return 1; _LL3572: return 0; _LL3574: return Cyc_Tcutil_bits_only(
_temp3587); _LL3576: for( 0; _temp3589 !=  0; _temp3589=(( struct Cyc_List_List*)
_check_null( _temp3589))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3589))->hd)).f2)){ return 0;}} return 1;
_LL3578: if( _temp3591 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp3603=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp3591)); if(
_temp3603->fields ==  0){ return 0;}{ struct _RegionHandle _temp3604=
_new_region(); struct _RegionHandle* rgn=& _temp3604; _push_region( rgn);{
struct Cyc_List_List* _temp3605=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3603->tvs, _temp3593);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp3603->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp3605,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ int _temp3606= 0;
_npop_handler( 0u); return _temp3606;}}}{ int _temp3607= 1; _npop_handler( 0u);
return _temp3607;}}; _pop_region( rgn);}} _LL3580: if( _temp3595 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3608=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3595)); if( _temp3608->fields ==  0){ return 0;}{ struct
_RegionHandle _temp3609= _new_region(); struct _RegionHandle* rgn=& _temp3609;
_push_region( rgn);{ struct Cyc_List_List* _temp3610=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3608->tvs, _temp3597);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3608->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3610,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3611= 0; _npop_handler( 0u); return
_temp3611;}}}{ int _temp3612= 1; _npop_handler( 0u); return _temp3612;}};
_pop_region( rgn);}} _LL3582: _temp3601= _temp3599; goto _LL3584; _LL3584: for(
0; _temp3601 !=  0; _temp3601=(( struct Cyc_List_List*) _check_null( _temp3601))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3601))->hd)->type)){ return 0;}} return 1;
_LL3586: return 0; _LL3562:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3613=( void*) e->r; struct
_tuple1* _temp3657; struct Cyc_Absyn_Exp* _temp3659; struct Cyc_Absyn_Exp*
_temp3661; struct Cyc_Absyn_Exp* _temp3663; struct Cyc_Absyn_Exp* _temp3665;
struct Cyc_Absyn_Exp* _temp3667; struct Cyc_Absyn_Exp* _temp3669; struct Cyc_Absyn_Exp*
_temp3671; struct Cyc_Absyn_Exp* _temp3673; void* _temp3675; struct Cyc_Absyn_Exp*
_temp3677; struct Cyc_Absyn_Exp* _temp3679; struct Cyc_Absyn_Exp* _temp3681;
struct Cyc_List_List* _temp3683; struct Cyc_List_List* _temp3685; struct Cyc_List_List*
_temp3687; struct Cyc_List_List* _temp3689; void* _temp3691; struct Cyc_List_List*
_temp3693; struct Cyc_List_List* _temp3695; _LL3615: if(*(( int*) _temp3613) == 
Cyc_Absyn_Const_e){ goto _LL3616;} else{ goto _LL3617;} _LL3617: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3618;} else{ goto _LL3619;}
_LL3619: if(*(( int*) _temp3613) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3620;}
else{ goto _LL3621;} _LL3621: if(*(( int*) _temp3613) ==  Cyc_Absyn_Offsetof_e){
goto _LL3622;} else{ goto _LL3623;} _LL3623: if(*(( int*) _temp3613) ==  Cyc_Absyn_Gentyp_e){
goto _LL3624;} else{ goto _LL3625;} _LL3625: if(*(( int*) _temp3613) ==  Cyc_Absyn_Enum_e){
goto _LL3626;} else{ goto _LL3627;} _LL3627: if(*(( int*) _temp3613) ==  Cyc_Absyn_Var_e){
_LL3658: _temp3657=(( struct Cyc_Absyn_Var_e_struct*) _temp3613)->f1; goto
_LL3628;} else{ goto _LL3629;} _LL3629: if(*(( int*) _temp3613) ==  Cyc_Absyn_Conditional_e){
_LL3664: _temp3663=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3613)->f1;
goto _LL3662; _LL3662: _temp3661=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3613)->f2; goto _LL3660; _LL3660: _temp3659=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3613)->f3; goto _LL3630;} else{ goto _LL3631;} _LL3631: if(*(( int*)
_temp3613) ==  Cyc_Absyn_SeqExp_e){ _LL3668: _temp3667=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3613)->f1; goto _LL3666; _LL3666: _temp3665=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3613)->f2; goto _LL3632;} else{ goto _LL3633;} _LL3633: if(*(( int*)
_temp3613) ==  Cyc_Absyn_NoInstantiate_e){ _LL3670: _temp3669=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3613)->f1; goto _LL3634;} else{ goto _LL3635;} _LL3635: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Instantiate_e){ _LL3672: _temp3671=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3613)->f1; goto _LL3636;} else{ goto _LL3637;} _LL3637: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Cast_e){ _LL3676: _temp3675=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3613)->f1; goto _LL3674; _LL3674: _temp3673=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3613)->f2; goto _LL3638;} else{ goto _LL3639;} _LL3639: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Address_e){ _LL3678: _temp3677=(( struct Cyc_Absyn_Address_e_struct*)
_temp3613)->f1; goto _LL3640;} else{ goto _LL3641;} _LL3641: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Comprehension_e){ _LL3682: _temp3681=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3613)->f2; goto _LL3680; _LL3680: _temp3679=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3613)->f3; goto _LL3642;} else{ goto _LL3643;} _LL3643: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Array_e){ _LL3684: _temp3683=(( struct Cyc_Absyn_Array_e_struct*)
_temp3613)->f1; goto _LL3644;} else{ goto _LL3645;} _LL3645: if(*(( int*)
_temp3613) ==  Cyc_Absyn_AnonStruct_e){ _LL3686: _temp3685=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3613)->f2; goto _LL3646;} else{ goto _LL3647;} _LL3647: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Struct_e){ _LL3688: _temp3687=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3613)->f3; goto _LL3648;} else{ goto _LL3649;} _LL3649: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Primop_e){ _LL3692: _temp3691=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3613)->f1; goto _LL3690; _LL3690: _temp3689=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3613)->f2; goto _LL3650;} else{ goto _LL3651;} _LL3651: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Tuple_e){ _LL3694: _temp3693=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3613)->f1; goto _LL3652;} else{ goto _LL3653;} _LL3653: if(*(( int*)
_temp3613) ==  Cyc_Absyn_Tunion_e){ _LL3696: _temp3695=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3613)->f3; goto _LL3654;} else{ goto _LL3655;} _LL3655: goto _LL3656;
_LL3616: return 1; _LL3618: return 1; _LL3620: return 1; _LL3622: return 1;
_LL3624: return 1; _LL3626: return 1; _LL3628: { struct _handler_cons _temp3697;
_push_handler(& _temp3697);{ int _temp3699= 0; if( setjmp( _temp3697.handler)){
_temp3699= 1;} if( ! _temp3699){{ void* _temp3700= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3657); void* _temp3710; void* _temp3712; struct Cyc_Absyn_Vardecl*
_temp3714; void* _temp3716; _LL3702: if(*(( int*) _temp3700) ==  Cyc_Tcenv_VarRes){
_LL3711: _temp3710=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3700)->f1;
if(( unsigned int) _temp3710 >  1u?*(( int*) _temp3710) ==  Cyc_Absyn_Funname_b:
0){ goto _LL3703;} else{ goto _LL3704;}} else{ goto _LL3704;} _LL3704: if(*((
int*) _temp3700) ==  Cyc_Tcenv_VarRes){ _LL3713: _temp3712=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3700)->f1; if(( unsigned int) _temp3712 >  1u?*(( int*) _temp3712) ==  Cyc_Absyn_Global_b:
0){ _LL3715: _temp3714=(( struct Cyc_Absyn_Global_b_struct*) _temp3712)->f1;
goto _LL3705;} else{ goto _LL3706;}} else{ goto _LL3706;} _LL3706: if(*(( int*)
_temp3700) ==  Cyc_Tcenv_VarRes){ _LL3717: _temp3716=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3700)->f1; if( _temp3716 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL3707;}
else{ goto _LL3708;}} else{ goto _LL3708;} _LL3708: goto _LL3709; _LL3703: { int
_temp3718= 1; _npop_handler( 0u); return _temp3718;} _LL3705: { void* _temp3719=
Cyc_Tcutil_compress(( void*) _temp3714->type); _LL3721: if(( unsigned int)
_temp3719 >  4u?*(( int*) _temp3719) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3722;}
else{ goto _LL3723;} _LL3723: goto _LL3724; _LL3722: { int _temp3725= 1;
_npop_handler( 0u); return _temp3725;} _LL3724: { int _temp3726= var_okay;
_npop_handler( 0u); return _temp3726;} _LL3720:;} _LL3707: { int _temp3727= 0;
_npop_handler( 0u); return _temp3727;} _LL3709: { int _temp3728= var_okay;
_npop_handler( 0u); return _temp3728;} _LL3701:;}; _pop_handler();} else{ void*
_temp3698=( void*) _exn_thrown; void* _temp3730= _temp3698; _LL3732: if(
_temp3730 ==  Cyc_Dict_Absent){ goto _LL3733;} else{ goto _LL3734;} _LL3734:
goto _LL3735; _LL3733: return 0; _LL3735:( void) _throw( _temp3730); _LL3731:;}}}
_LL3630: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3663)? Cyc_Tcutil_cnst_exp( te,
0, _temp3661): 0)? Cyc_Tcutil_cnst_exp( te, 0, _temp3659): 0; _LL3632: return
Cyc_Tcutil_cnst_exp( te, 0, _temp3667)? Cyc_Tcutil_cnst_exp( te, 0, _temp3665):
0; _LL3634: _temp3671= _temp3669; goto _LL3636; _LL3636: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3671); _LL3638: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3673); _LL3640: return Cyc_Tcutil_cnst_exp( te, 1, _temp3677); _LL3642:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3681)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3679): 0; _LL3644: _temp3685= _temp3683; goto _LL3646; _LL3646: _temp3687=
_temp3685; goto _LL3648; _LL3648: for( 0; _temp3687 !=  0; _temp3687=(( struct
Cyc_List_List*) _check_null( _temp3687))->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*((
struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3687))->hd)).f2)){
return 0;}} return 1; _LL3650: _temp3693= _temp3689; goto _LL3652; _LL3652:
_temp3695= _temp3693; goto _LL3654; _LL3654: for( 0; _temp3695 !=  0; _temp3695=((
struct Cyc_List_List*) _check_null( _temp3695))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3695))->hd)){
return 0;}} return 1; _LL3656: return 0; _LL3614:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3736= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3764; struct Cyc_Absyn_Conref* _temp3766; struct Cyc_Absyn_Conref*
_temp3768; void* _temp3770; struct Cyc_List_List* _temp3772; struct Cyc_Absyn_Structdecl**
_temp3774; struct Cyc_List_List* _temp3776; struct Cyc_Absyn_Uniondecl**
_temp3778; struct Cyc_List_List* _temp3780; struct Cyc_List_List* _temp3782;
struct Cyc_List_List* _temp3784; _LL3738: if( _temp3736 == ( void*) Cyc_Absyn_VoidType){
goto _LL3739;} else{ goto _LL3740;} _LL3740: if(( unsigned int) _temp3736 >  4u?*((
int*) _temp3736) ==  Cyc_Absyn_IntType: 0){ goto _LL3741;} else{ goto _LL3742;}
_LL3742: if( _temp3736 == ( void*) Cyc_Absyn_FloatType){ goto _LL3743;} else{
goto _LL3744;} _LL3744: if( _temp3736 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3745;} else{ goto _LL3746;} _LL3746: if(( unsigned int) _temp3736 >  4u?*((
int*) _temp3736) ==  Cyc_Absyn_PointerType: 0){ _LL3765: _temp3764=(( struct Cyc_Absyn_PointerType_struct*)
_temp3736)->f1; _LL3769: _temp3768= _temp3764.nullable; goto _LL3767; _LL3767:
_temp3766= _temp3764.bounds; goto _LL3747;} else{ goto _LL3748;} _LL3748: if((
unsigned int) _temp3736 >  4u?*(( int*) _temp3736) ==  Cyc_Absyn_ArrayType: 0){
_LL3771: _temp3770=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3736)->f1;
goto _LL3749;} else{ goto _LL3750;} _LL3750: if(( unsigned int) _temp3736 >  4u?*((
int*) _temp3736) ==  Cyc_Absyn_TupleType: 0){ _LL3773: _temp3772=(( struct Cyc_Absyn_TupleType_struct*)
_temp3736)->f1; goto _LL3751;} else{ goto _LL3752;} _LL3752: if(( unsigned int)
_temp3736 >  4u?*(( int*) _temp3736) ==  Cyc_Absyn_StructType: 0){ _LL3777:
_temp3776=(( struct Cyc_Absyn_StructType_struct*) _temp3736)->f2; goto _LL3775;
_LL3775: _temp3774=(( struct Cyc_Absyn_StructType_struct*) _temp3736)->f3; goto
_LL3753;} else{ goto _LL3754;} _LL3754: if(( unsigned int) _temp3736 >  4u?*((
int*) _temp3736) ==  Cyc_Absyn_UnionType: 0){ _LL3781: _temp3780=(( struct Cyc_Absyn_UnionType_struct*)
_temp3736)->f2; goto _LL3779; _LL3779: _temp3778=(( struct Cyc_Absyn_UnionType_struct*)
_temp3736)->f3; goto _LL3755;} else{ goto _LL3756;} _LL3756: if(( unsigned int)
_temp3736 >  4u?*(( int*) _temp3736) ==  Cyc_Absyn_AnonStructType: 0){ _LL3783:
_temp3782=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3736)->f1; goto
_LL3757;} else{ goto _LL3758;} _LL3758: if(( unsigned int) _temp3736 >  4u?*((
int*) _temp3736) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3785: _temp3784=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3736)->f1; goto _LL3759;} else{ goto
_LL3760;} _LL3760: if(( unsigned int) _temp3736 >  4u?*(( int*) _temp3736) == 
Cyc_Absyn_EnumType: 0){ goto _LL3761;} else{ goto _LL3762;} _LL3762: goto
_LL3763; _LL3739: goto _LL3741; _LL3741: goto _LL3743; _LL3743: goto _LL3745;
_LL3745: return 1; _LL3747: { void* _temp3786=( void*)( Cyc_Absyn_compress_conref(
_temp3766))->v; void* _temp3794; void* _temp3796; _LL3788: if(( unsigned int)
_temp3786 >  1u?*(( int*) _temp3786) ==  Cyc_Absyn_Eq_constr: 0){ _LL3795:
_temp3794=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3786)->f1; if(
_temp3794 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3789;} else{ goto _LL3790;}}
else{ goto _LL3790;} _LL3790: if(( unsigned int) _temp3786 >  1u?*(( int*)
_temp3786) ==  Cyc_Absyn_Eq_constr: 0){ _LL3797: _temp3796=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3786)->f1; if(( unsigned int) _temp3796 >  1u?*(( int*) _temp3796) ==  Cyc_Absyn_Upper_b:
0){ goto _LL3791;} else{ goto _LL3792;}} else{ goto _LL3792;} _LL3792: goto
_LL3793; _LL3789: return 1; _LL3791: { void* _temp3798=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3768))->v; int
_temp3804; _LL3800: if(( unsigned int) _temp3798 >  1u?*(( int*) _temp3798) == 
Cyc_Absyn_Eq_constr: 0){ _LL3805: _temp3804=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3798)->f1; goto _LL3801;} else{ goto _LL3802;} _LL3802: goto _LL3803;
_LL3801: return _temp3804; _LL3803: return 0; _LL3799:;} _LL3793: return 0;
_LL3787:;} _LL3749: return Cyc_Tcutil_supports_default( _temp3770); _LL3751:
for( 0; _temp3772 !=  0; _temp3772=(( struct Cyc_List_List*) _check_null(
_temp3772))->tl){ if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3772))->hd)).f2)){ return 0;}} return 1;
_LL3753: if( _temp3774 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3774)); if( sd->fields ==  0){
return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs, _temp3776,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);} _LL3755:
if( _temp3778 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3778)); if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp3780,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL3757: _temp3784= _temp3782; goto _LL3759; _LL3759: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp3784); _LL3761: return 1; _LL3763:
return 0; _LL3737:;} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{ struct _RegionHandle
_temp3806= _new_region(); struct _RegionHandle* rgn=& _temp3806; _push_region(
rgn);{ struct Cyc_List_List* _temp3807=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3807,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3808= 0; _npop_handler( 0u);
return _temp3808;}}}; _pop_region( rgn);} return 1;}
