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
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
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
extern void Cyc_List_iter2( void(* f)( void*, void*), struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_revappend(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y); struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r1, struct _RegionHandle* r2,
struct Cyc_List_List* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(* cmp)(
void*, void*), struct Cyc_List_List* l, void* k); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; extern
struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
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
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
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
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
extern void* Cyc_Absyn_empty_effect; extern struct _tuple1* Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple1* Cyc_Absyn_tunion_scanf_arg_qvar; extern void* Cyc_Absyn_bounds_one;
extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq,
void* b); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple4*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct
Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_kind2string( void*); extern struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*); extern struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
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
extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*); extern int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int
Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct Cyc_Absyn_Exp*
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
int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest);
extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*, void* t, struct
Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern struct _tuple7
Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e);
extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int* Cyc_Tcutil_new_tvar_id();
extern void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*); extern void
Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_snd_tqt(
struct _tuple4*); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
unsigned char Cyc_Tcutil_TypeErr[ 12u]="\000\000\000\000TypeErr"; extern void
Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap){ Cyc_Position_post_error(
Cyc_Position_mk_err_elab( loc,( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap)));} void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr
ap){ struct _tagged_arr msg=( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap);({ struct Cyc_Std_String_pa_struct _temp1; _temp1.tag= Cyc_Std_String_pa;
_temp1.f1=( struct _tagged_arr) msg;{ void* _temp0[ 1u]={& _temp1}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp0, sizeof( void*), 1u));}}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);(
int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){({ struct Cyc_Std_String_pa_struct _temp4; _temp4.tag=
Cyc_Std_String_pa; _temp4.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);{
struct Cyc_Std_String_pa_struct _temp3; _temp3.tag= Cyc_Std_String_pa; _temp3.f1=(
struct _tagged_arr) Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd);{ void* _temp2[ 2u]={& _temp3,& _temp4};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::%s ", sizeof( unsigned char), 8u),
_tag_arr( _temp2, sizeof( void*), 2u));}}});}({ void* _temp5[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp5,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_arr fmt, struct _tagged_arr ap){ struct _tagged_arr msg=(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap); Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp6=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp6->hd=( void*) sg; _temp6->tl= Cyc_Tcutil_warning_segs;
_temp6;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp7->hd=( void*)({ struct
_tagged_arr* _temp8=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp8[ 0]= msg; _temp8;}); _temp7->tl= Cyc_Tcutil_warning_msgs; _temp7;});}
void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs ==  0){ return;}({
void* _temp9[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("***Warnings***\n",
sizeof( unsigned char), 16u), _tag_arr( _temp9, sizeof( void*), 0u));});{ struct
Cyc_List_List* _temp10= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs !=  0) {({ struct Cyc_Std_String_pa_struct _temp13;
_temp13.tag= Cyc_Std_String_pa; _temp13.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Std_String_pa_struct _temp12; _temp12.tag= Cyc_Std_String_pa; _temp12.f1=(
struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
_temp10))->hd);{ void* _temp11[ 2u]={& _temp12,& _temp13}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp11, sizeof(
void*), 2u));}}}); _temp10=(( struct Cyc_List_List*) _check_null( _temp10))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}({
void* _temp14[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("**************\n",
sizeof( unsigned char), 16u), _tag_arr( _temp14, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) - *(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp15= t; struct Cyc_Core_Opt*
_temp27; struct Cyc_Core_Opt* _temp29; struct Cyc_Core_Opt** _temp31; struct Cyc_Core_Opt*
_temp32; struct Cyc_Core_Opt* _temp34; struct Cyc_Core_Opt** _temp36; _LL17: if((
unsigned int) _temp15 >  4u?*(( int*) _temp15) ==  Cyc_Absyn_Evar: 0){ _LL28:
_temp27=(( struct Cyc_Absyn_Evar_struct*) _temp15)->f2; if( _temp27 ==  0){ goto
_LL18;} else{ goto _LL19;}} else{ goto _LL19;} _LL19: if(( unsigned int) _temp15
>  4u?*(( int*) _temp15) ==  Cyc_Absyn_Evar: 0){ _LL30: _temp29=(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f2; _temp31=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f2; goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp15
>  4u?*(( int*) _temp15) ==  Cyc_Absyn_TypedefType: 0){ _LL33: _temp32=(( struct
Cyc_Absyn_TypedefType_struct*) _temp15)->f3; if( _temp32 ==  0){ goto _LL22;}
else{ goto _LL23;}} else{ goto _LL23;} _LL23: if(( unsigned int) _temp15 >  4u?*((
int*) _temp15) ==  Cyc_Absyn_TypedefType: 0){ _LL35: _temp34=(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f3; _temp36=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f3; goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL18: return t;
_LL20: { void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp31))->v); if( t2 != ( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp31))->v){* _temp31=({ struct Cyc_Core_Opt* _temp37=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp37->v=( void*) t2;
_temp37;});} return t2;} _LL22: return t; _LL24: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp36))->v); if( t2 != ( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp36))->v){* _temp36=({ struct Cyc_Core_Opt*
_temp38=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp38->v=( void*) t2; _temp38;});} return t2;} _LL26: return t; _LL16:;} void*
Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp39=( void*) c->v; void* _temp47; struct Cyc_Absyn_Conref* _temp49;
_LL41: if( _temp39 == ( void*) Cyc_Absyn_No_constr){ goto _LL42;} else{ goto
_LL43;} _LL43: if(( unsigned int) _temp39 >  1u?*(( int*) _temp39) ==  Cyc_Absyn_Eq_constr:
0){ _LL48: _temp47=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp39)->f1;
goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp39 >  1u?*(( int*)
_temp39) ==  Cyc_Absyn_Forward_constr: 0){ _LL50: _temp49=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp39)->f1; goto _LL46;} else{ goto _LL40;} _LL42: return Cyc_Absyn_empty_conref();
_LL44: return Cyc_Absyn_new_conref( _temp47); _LL46: return Cyc_Tcutil_copy_conref(
_temp49); _LL40:;} static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_Absyn_Tvar* _temp51=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp51->name= tv->name; _temp51->identity=
0; _temp51->kind= Cyc_Tcutil_copy_conref( tv->kind); _temp51;});} static struct
_tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp54; struct Cyc_Absyn_Tqual
_temp56; struct Cyc_Core_Opt* _temp58; struct _tuple2 _temp52=* arg; _LL59:
_temp58= _temp52.f1; goto _LL57; _LL57: _temp56= _temp52.f2; goto _LL55; _LL55:
_temp54= _temp52.f3; goto _LL53; _LL53: return({ struct _tuple2* _temp60=(
struct _tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp60->f1= _temp58;
_temp60->f2= _temp56; _temp60->f3= Cyc_Tcutil_copy_type( _temp54); _temp60;});}
static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){ void* _temp63;
struct Cyc_Absyn_Tqual _temp65; struct _tuple4 _temp61=* arg; _LL66: _temp65=
_temp61.f1; goto _LL64; _LL64: _temp63= _temp61.f2; goto _LL62; _LL62: return({
struct _tuple4* _temp67=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp67->f1= _temp65; _temp67->f2= Cyc_Tcutil_copy_type( _temp63); _temp67;});}
static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field( struct Cyc_Absyn_Structfield*
f){ return({ struct Cyc_Absyn_Structfield* _temp68=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp68->name= f->name;
_temp68->tq= f->tq; _temp68->type=( void*) Cyc_Tcutil_copy_type(( void*) f->type);
_temp68->width= f->width; _temp68->attributes= f->attributes; _temp68;});}
struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp71; void* _temp72; void* _temp74; struct
_tuple8* _temp69= x; _temp71=* _temp69; _LL75: _temp74= _temp71.f1; goto _LL73;
_LL73: _temp72= _temp71.f2; goto _LL70; _LL70: return({ struct _tuple8* _temp76=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp76->f1= Cyc_Tcutil_copy_type(
_temp74); _temp76->f2= Cyc_Tcutil_copy_type( _temp72); _temp76;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp77= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp125; struct Cyc_Absyn_TunionInfo _temp127; void* _temp129; struct Cyc_List_List*
_temp131; void* _temp133; struct Cyc_Absyn_TunionFieldInfo _temp135; struct Cyc_List_List*
_temp137; void* _temp139; struct Cyc_Absyn_PtrInfo _temp141; struct Cyc_Absyn_Conref*
_temp143; struct Cyc_Absyn_Tqual _temp145; struct Cyc_Absyn_Conref* _temp147;
void* _temp149; void* _temp151; void* _temp153; void* _temp155; struct Cyc_Absyn_Exp*
_temp157; struct Cyc_Absyn_Tqual _temp159; void* _temp161; struct Cyc_Absyn_FnInfo
_temp163; struct Cyc_List_List* _temp165; struct Cyc_List_List* _temp167; struct
Cyc_Absyn_VarargInfo* _temp169; int _temp171; struct Cyc_List_List* _temp173;
void* _temp175; struct Cyc_Core_Opt* _temp177; struct Cyc_List_List* _temp179;
struct Cyc_List_List* _temp181; struct Cyc_List_List* _temp183; struct _tuple1*
_temp185; struct Cyc_List_List* _temp187; struct _tuple1* _temp189; struct Cyc_List_List*
_temp191; struct Cyc_List_List* _temp193; struct _tuple1* _temp195; void*
_temp197; struct Cyc_List_List* _temp199; struct _tuple1* _temp201; void*
_temp203; struct Cyc_List_List* _temp205; void* _temp207; _LL79: if( _temp77 == (
void*) Cyc_Absyn_VoidType){ goto _LL80;} else{ goto _LL81;} _LL81: if((
unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_Evar: 0){ goto
_LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp77 >  4u?*(( int*)
_temp77) ==  Cyc_Absyn_VarType: 0){ _LL126: _temp125=(( struct Cyc_Absyn_VarType_struct*)
_temp77)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp77
>  4u?*(( int*) _temp77) ==  Cyc_Absyn_TunionType: 0){ _LL128: _temp127=((
struct Cyc_Absyn_TunionType_struct*) _temp77)->f1; _LL134: _temp133=( void*)
_temp127.tunion_info; goto _LL132; _LL132: _temp131= _temp127.targs; goto _LL130;
_LL130: _temp129=( void*) _temp127.rgn; goto _LL86;} else{ goto _LL87;} _LL87:
if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_TunionFieldType:
0){ _LL136: _temp135=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp77)->f1;
_LL140: _temp139=( void*) _temp135.field_info; goto _LL138; _LL138: _temp137=
_temp135.targs; goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_PointerType: 0){ _LL142: _temp141=((
struct Cyc_Absyn_PointerType_struct*) _temp77)->f1; _LL152: _temp151=( void*)
_temp141.elt_typ; goto _LL150; _LL150: _temp149=( void*) _temp141.rgn_typ; goto
_LL148; _LL148: _temp147= _temp141.nullable; goto _LL146; _LL146: _temp145=
_temp141.tq; goto _LL144; _LL144: _temp143= _temp141.bounds; goto _LL90;} else{
goto _LL91;} _LL91: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_IntType:
0){ _LL156: _temp155=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp77)->f1;
goto _LL154; _LL154: _temp153=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp77)->f2; goto _LL92;} else{ goto _LL93;} _LL93: if( _temp77 == ( void*) Cyc_Absyn_FloatType){
goto _LL94;} else{ goto _LL95;} _LL95: if( _temp77 == ( void*) Cyc_Absyn_DoubleType){
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp77 >  4u?*(( int*)
_temp77) ==  Cyc_Absyn_ArrayType: 0){ _LL162: _temp161=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f1; goto _LL160; _LL160: _temp159=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f2; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f3; goto _LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp77
>  4u?*(( int*) _temp77) ==  Cyc_Absyn_FnType: 0){ _LL164: _temp163=(( struct
Cyc_Absyn_FnType_struct*) _temp77)->f1; _LL180: _temp179= _temp163.tvars; goto
_LL178; _LL178: _temp177= _temp163.effect; goto _LL176; _LL176: _temp175=( void*)
_temp163.ret_typ; goto _LL174; _LL174: _temp173= _temp163.args; goto _LL172;
_LL172: _temp171= _temp163.c_varargs; goto _LL170; _LL170: _temp169= _temp163.cyc_varargs;
goto _LL168; _LL168: _temp167= _temp163.rgn_po; goto _LL166; _LL166: _temp165=
_temp163.attributes; goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_TupleType: 0){ _LL182: _temp181=((
struct Cyc_Absyn_TupleType_struct*) _temp77)->f1; goto _LL102;} else{ goto
_LL103;} _LL103: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_StructType:
0){ _LL186: _temp185=(( struct Cyc_Absyn_StructType_struct*) _temp77)->f1; goto
_LL184; _LL184: _temp183=(( struct Cyc_Absyn_StructType_struct*) _temp77)->f2;
goto _LL104;} else{ goto _LL105;} _LL105: if(( unsigned int) _temp77 >  4u?*((
int*) _temp77) ==  Cyc_Absyn_UnionType: 0){ _LL190: _temp189=(( struct Cyc_Absyn_UnionType_struct*)
_temp77)->f1; goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_UnionType_struct*)
_temp77)->f2; goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_AnonStructType: 0){ _LL192:
_temp191=(( struct Cyc_Absyn_AnonStructType_struct*) _temp77)->f1; goto _LL108;}
else{ goto _LL109;} _LL109: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77)
==  Cyc_Absyn_AnonUnionType: 0){ _LL194: _temp193=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp77)->f1; goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_EnumType: 0){ _LL196: _temp195=((
struct Cyc_Absyn_EnumType_struct*) _temp77)->f1; goto _LL112;} else{ goto _LL113;}
_LL113: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_RgnHandleType:
0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp77)->f1;
goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int) _temp77 >  4u?*((
int*) _temp77) ==  Cyc_Absyn_TypedefType: 0){ _LL202: _temp201=(( struct Cyc_Absyn_TypedefType_struct*)
_temp77)->f1; goto _LL200; _LL200: _temp199=(( struct Cyc_Absyn_TypedefType_struct*)
_temp77)->f2; goto _LL116;} else{ goto _LL117;} _LL117: if( _temp77 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_AccessEff: 0){ _LL204: _temp203=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp77)->f1; goto _LL120;} else{
goto _LL121;} _LL121: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) == 
Cyc_Absyn_JoinEff: 0){ _LL206: _temp205=(( struct Cyc_Absyn_JoinEff_struct*)
_temp77)->f1; goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_RgnsEff: 0){ _LL208: _temp207=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp77)->f1; goto _LL124;} else{
goto _LL78;} _LL80: return t; _LL82: return t; _LL84: return( void*)({ struct
Cyc_Absyn_VarType_struct* _temp209=( struct Cyc_Absyn_VarType_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp209[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp210; _temp210.tag= Cyc_Absyn_VarType; _temp210.f1= Cyc_Tcutil_copy_tvar(
_temp125); _temp210;}); _temp209;}); _LL86: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp211=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp211[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp212; _temp212.tag= Cyc_Absyn_TunionType;
_temp212.f1=({ struct Cyc_Absyn_TunionInfo _temp213; _temp213.tunion_info=( void*)
_temp133; _temp213.targs= Cyc_Tcutil_copy_types( _temp131); _temp213.rgn=( void*)
Cyc_Tcutil_copy_type( _temp129); _temp213;}); _temp212;}); _temp211;}); _LL88:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp214=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp214[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp215; _temp215.tag= Cyc_Absyn_TunionFieldType;
_temp215.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp216; _temp216.field_info=(
void*) _temp139; _temp216.targs= Cyc_Tcutil_copy_types( _temp137); _temp216;});
_temp215;}); _temp214;}); _LL90: { void* _temp217= Cyc_Tcutil_copy_type(
_temp151); void* _temp218= Cyc_Tcutil_copy_type( _temp149); struct Cyc_Absyn_Conref*
_temp219=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp147); struct Cyc_Absyn_Tqual _temp220= _temp145; struct Cyc_Absyn_Conref*
_temp221= Cyc_Tcutil_copy_conref( _temp143); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp222=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp222[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp223; _temp223.tag= Cyc_Absyn_PointerType;
_temp223.f1=({ struct Cyc_Absyn_PtrInfo _temp224; _temp224.elt_typ=( void*)
_temp217; _temp224.rgn_typ=( void*) _temp218; _temp224.nullable= _temp219;
_temp224.tq= _temp220; _temp224.bounds= _temp221; _temp224;}); _temp223;});
_temp222;});} _LL92: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp225=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp225[ 0]=({ struct Cyc_Absyn_IntType_struct _temp226; _temp226.tag= Cyc_Absyn_IntType;
_temp226.f1=( void*) _temp155; _temp226.f2=( void*) _temp153; _temp226;});
_temp225;}); _LL94: goto _LL96; _LL96: return t; _LL98: return( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp227=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp227[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp228; _temp228.tag= Cyc_Absyn_ArrayType; _temp228.f1=(
void*) Cyc_Tcutil_copy_type( _temp161); _temp228.f2= _temp159; _temp228.f3=
_temp157; _temp228;}); _temp227;}); _LL100: { struct Cyc_List_List* _temp229=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp179); struct
Cyc_Core_Opt* _temp230= _temp177 ==  0? 0:({ struct Cyc_Core_Opt* _temp240=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp240->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp177))->v); _temp240;}); void* _temp231= Cyc_Tcutil_copy_type( _temp175);
struct Cyc_List_List* _temp232=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp173); int _temp233= _temp171; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp169 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp169); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp234=(
struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp234->name= cv->name; _temp234->tq= cv->tq; _temp234->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp234->inject= cv->inject; _temp234;});}{ struct Cyc_List_List*
_temp235=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp167);
struct Cyc_List_List* _temp236= _temp165; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp237=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp237[ 0]=({ struct Cyc_Absyn_FnType_struct _temp238; _temp238.tag= Cyc_Absyn_FnType;
_temp238.f1=({ struct Cyc_Absyn_FnInfo _temp239; _temp239.tvars= _temp229;
_temp239.effect= _temp230; _temp239.ret_typ=( void*) _temp231; _temp239.args=
_temp232; _temp239.c_varargs= _temp233; _temp239.cyc_varargs= cyc_varargs2;
_temp239.rgn_po= _temp235; _temp239.attributes= _temp236; _temp239;}); _temp238;});
_temp237;});}} _LL102: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp241=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp241[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp242; _temp242.tag= Cyc_Absyn_TupleType;
_temp242.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp181);
_temp242;}); _temp241;}); _LL104: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp243=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp243[ 0]=({ struct Cyc_Absyn_StructType_struct _temp244; _temp244.tag= Cyc_Absyn_StructType;
_temp244.f1= _temp185; _temp244.f2= Cyc_Tcutil_copy_types( _temp183); _temp244.f3=
0; _temp244;}); _temp243;}); _LL106: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp245=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp245[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp246; _temp246.tag= Cyc_Absyn_UnionType;
_temp246.f1= _temp189; _temp246.f2= Cyc_Tcutil_copy_types( _temp187); _temp246.f3=
0; _temp246;}); _temp245;}); _LL108: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp247=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp247[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp248; _temp248.tag= Cyc_Absyn_AnonStructType; _temp248.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp191); _temp248;}); _temp247;});
_LL110: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp249=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp249[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp250; _temp250.tag=
Cyc_Absyn_AnonUnionType; _temp250.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp193); _temp250;}); _temp249;}); _LL112: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp251=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp251[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp252; _temp252.tag= Cyc_Absyn_EnumType;
_temp252.f1= _temp195; _temp252.f2= 0; _temp252;}); _temp251;}); _LL114: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp253=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp253[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp254; _temp254.tag= Cyc_Absyn_RgnHandleType;
_temp254.f1=( void*) Cyc_Tcutil_copy_type( _temp197); _temp254;}); _temp253;});
_LL116: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp255=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp255[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp256; _temp256.tag= Cyc_Absyn_TypedefType;
_temp256.f1= _temp201; _temp256.f2= Cyc_Tcutil_copy_types( _temp199); _temp256.f3=
0; _temp256;}); _temp255;}); _LL118: return t; _LL120: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp257=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp257[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp258; _temp258.tag= Cyc_Absyn_AccessEff; _temp258.f1=(
void*) Cyc_Tcutil_copy_type( _temp203); _temp258;}); _temp257;}); _LL122: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp259=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp259[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp260; _temp260.tag= Cyc_Absyn_JoinEff; _temp260.f1= Cyc_Tcutil_copy_types(
_temp205); _temp260;}); _temp259;}); _LL124: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp261=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp261[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp262; _temp262.tag= Cyc_Absyn_RgnsEff;
_temp262.f1=( void*) Cyc_Tcutil_copy_type( _temp207); _temp262;}); _temp261;});
_LL78:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){
return 1;}{ struct _tuple8 _temp264=({ struct _tuple8 _temp263; _temp263.f1= k1;
_temp263.f2= k2; _temp263;}); void* _temp274; void* _temp276; void* _temp278;
void* _temp280; void* _temp282; void* _temp284; _LL266: _LL277: _temp276=
_temp264.f1; if( _temp276 == ( void*) Cyc_Absyn_BoxKind){ goto _LL275;} else{
goto _LL268;} _LL275: _temp274= _temp264.f2; if( _temp274 == ( void*) Cyc_Absyn_MemKind){
goto _LL267;} else{ goto _LL268;} _LL268: _LL281: _temp280= _temp264.f1; if(
_temp280 == ( void*) Cyc_Absyn_BoxKind){ goto _LL279;} else{ goto _LL270;}
_LL279: _temp278= _temp264.f2; if( _temp278 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL269;} else{ goto _LL270;} _LL270: _LL285: _temp284= _temp264.f1; if( _temp284
== ( void*) Cyc_Absyn_MemKind){ goto _LL283;} else{ goto _LL272;} _LL283:
_temp282= _temp264.f2; if( _temp282 == ( void*) Cyc_Absyn_AnyKind){ goto _LL271;}
else{ goto _LL272;} _LL272: goto _LL273; _LL267: return 1; _LL269: return 1;
_LL271: return 1; _LL273: return 0; _LL265:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp286= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp340;
struct Cyc_Core_Opt* _temp342; struct Cyc_Absyn_Tvar* _temp344; void* _temp346;
struct Cyc_Absyn_TunionFieldInfo _temp348; void* _temp350; struct Cyc_Absyn_Tunionfield*
_temp352; struct Cyc_Absyn_TunionFieldInfo _temp354; void* _temp356; struct Cyc_Absyn_Structdecl**
_temp358; struct Cyc_Absyn_Uniondecl** _temp360; struct Cyc_Absyn_PtrInfo
_temp362; void* _temp364; struct Cyc_List_List* _temp366; struct Cyc_Core_Opt*
_temp368; _LL288: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_Evar:
0){ _LL343: _temp342=(( struct Cyc_Absyn_Evar_struct*) _temp286)->f1; goto
_LL341; _LL341: _temp340=(( struct Cyc_Absyn_Evar_struct*) _temp286)->f2; goto
_LL289;} else{ goto _LL290;} _LL290: if(( unsigned int) _temp286 >  4u?*(( int*)
_temp286) ==  Cyc_Absyn_VarType: 0){ _LL345: _temp344=(( struct Cyc_Absyn_VarType_struct*)
_temp286)->f1; goto _LL291;} else{ goto _LL292;} _LL292: if( _temp286 == ( void*)
Cyc_Absyn_VoidType){ goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_IntType: 0){ _LL347: _temp346=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp286)->f2; goto _LL295;} else{
goto _LL296;} _LL296: if( _temp286 == ( void*) Cyc_Absyn_FloatType){ goto _LL297;}
else{ goto _LL298;} _LL298: if( _temp286 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL299;} else{ goto _LL300;} _LL300: if(( unsigned int) _temp286 >  4u?*(( int*)
_temp286) ==  Cyc_Absyn_FnType: 0){ goto _LL301;} else{ goto _LL302;} _LL302:
if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL303;} else{ goto _LL304;} _LL304: if( _temp286 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL305;} else{ goto _LL306;} _LL306: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_TunionType: 0){ goto _LL307;} else{ goto _LL308;}
_LL308: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_TunionFieldType:
0){ _LL349: _temp348=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp286)->f1;
_LL351: _temp350=( void*) _temp348.field_info; if(*(( int*) _temp350) ==  Cyc_Absyn_KnownTunionfield){
_LL353: _temp352=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp350)->f2;
goto _LL309;} else{ goto _LL310;}} else{ goto _LL310;} _LL310: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_TunionFieldType: 0){ _LL355:
_temp354=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp286)->f1; _LL357:
_temp356=( void*) _temp354.field_info; if(*(( int*) _temp356) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL311;} else{ goto _LL312;}} else{ goto _LL312;} _LL312: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_StructType: 0){ _LL359:
_temp358=(( struct Cyc_Absyn_StructType_struct*) _temp286)->f3; if( _temp358 == 
0){ goto _LL313;} else{ goto _LL314;}} else{ goto _LL314;} _LL314: if((
unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_UnionType: 0){
_LL361: _temp360=(( struct Cyc_Absyn_UnionType_struct*) _temp286)->f3; if(
_temp360 ==  0){ goto _LL315;} else{ goto _LL316;}} else{ goto _LL316;} _LL316:
if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_StructType:
0){ goto _LL317;} else{ goto _LL318;} _LL318: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_UnionType: 0){ goto _LL319;} else{ goto _LL320;}
_LL320: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL323;} else{ goto _LL324;}
_LL324: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_EnumType:
0){ goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_PointerType: 0){ _LL363: _temp362=(( struct Cyc_Absyn_PointerType_struct*)
_temp286)->f1; goto _LL327;} else{ goto _LL328;} _LL328: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_ArrayType: 0){ _LL365: _temp364=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp286)->f1; goto _LL329;} else{
goto _LL330;} _LL330: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) == 
Cyc_Absyn_TupleType: 0){ _LL367: _temp366=(( struct Cyc_Absyn_TupleType_struct*)
_temp286)->f1; goto _LL331;} else{ goto _LL332;} _LL332: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_TypedefType: 0){ _LL369:
_temp368=(( struct Cyc_Absyn_TypedefType_struct*) _temp286)->f3; goto _LL333;}
else{ goto _LL334;} _LL334: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286)
==  Cyc_Absyn_AccessEff: 0){ goto _LL335;} else{ goto _LL336;} _LL336: if((
unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_JoinEff: 0){ goto
_LL337;} else{ goto _LL338;} _LL338: if(( unsigned int) _temp286 >  4u?*(( int*)
_temp286) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL339;} else{ goto _LL287;} _LL289:
return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp342))->v; _LL291:
return Cyc_Absyn_conref_val( _temp344->kind); _LL293: return( void*) Cyc_Absyn_MemKind;
_LL295: return _temp346 == ( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL297: return( void*) Cyc_Absyn_MemKind; _LL299:
return( void*) Cyc_Absyn_MemKind; _LL301: return( void*) Cyc_Absyn_MemKind;
_LL303: return( void*) Cyc_Absyn_BoxKind; _LL305: return( void*) Cyc_Absyn_RgnKind;
_LL307: return( void*) Cyc_Absyn_BoxKind; _LL309: if( _temp352->typs ==  0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL311: return({ void* _temp370[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u), _tag_arr( _temp370, sizeof( void*), 0u));});
_LL313: return( void*) Cyc_Absyn_AnyKind; _LL315: return( void*) Cyc_Absyn_AnyKind;
_LL317: return( void*) Cyc_Absyn_MemKind; _LL319: return( void*) Cyc_Absyn_MemKind;
_LL321: return( void*) Cyc_Absyn_MemKind; _LL323: return( void*) Cyc_Absyn_MemKind;
_LL325: return( void*) Cyc_Absyn_BoxKind; _LL327: { void* _temp371=( void*)( Cyc_Absyn_compress_conref(
_temp362.bounds))->v; void* _temp381; void* _temp383; _LL373: if(( unsigned int)
_temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Eq_constr: 0){ _LL382: _temp381=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp371)->f1; if( _temp381 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL374;} else{ goto _LL375;}} else{ goto
_LL375;} _LL375: if(( unsigned int) _temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Eq_constr:
0){ _LL384: _temp383=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp371)->f1;
if(( unsigned int) _temp383 >  1u?*(( int*) _temp383) ==  Cyc_Absyn_Upper_b: 0){
goto _LL376;} else{ goto _LL377;}} else{ goto _LL377;} _LL377: if( _temp371 == (
void*) Cyc_Absyn_No_constr){ goto _LL378;} else{ goto _LL379;} _LL379: if((
unsigned int) _temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL380;} else{ goto _LL372;} _LL374: return( void*) Cyc_Absyn_MemKind;
_LL376: return( void*) Cyc_Absyn_BoxKind; _LL378: return( void*) Cyc_Absyn_MemKind;
_LL380: return({ void* _temp385[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof( unsigned char), 43u), _tag_arr( _temp385, sizeof( void*), 0u));});
_LL372:;} _LL329: return( void*) Cyc_Absyn_MemKind; _LL331: return( void*) Cyc_Absyn_MemKind;
_LL333: return({ struct Cyc_Std_String_pa_struct _temp387; _temp387.tag= Cyc_Std_String_pa;
_temp387.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp386[ 1u]={&
_temp387}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp386, sizeof( void*), 1u));}}); _LL335:
return( void*) Cyc_Absyn_EffKind; _LL337: return( void*) Cyc_Absyn_EffKind;
_LL339: return( void*) Cyc_Absyn_EffKind; _LL287:;} unsigned char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp388; _push_handler(& _temp388);{ int _temp390= 0; if( setjmp(
_temp388.handler)){ _temp390= 1;} if( ! _temp390){ Cyc_Tcutil_unify_it( t1, t2);{
int _temp391= 1; _npop_handler( 0u); return _temp391;}; _pop_handler();} else{
void* _temp389=( void*) _exn_thrown; void* _temp393= _temp389; _LL395: if(
_temp393 ==  Cyc_Tcutil_Unify){ goto _LL396;} else{ goto _LL397;} _LL397: goto
_LL398; _LL396: return 0; _LL398:( void) _throw( _temp393); _LL394:;}}} static
void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, struct Cyc_List_List* env, void*
t){ void* _temp399= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp435;
struct Cyc_Core_Opt* _temp437; struct Cyc_Core_Opt** _temp439; struct Cyc_Core_Opt*
_temp440; struct Cyc_Absyn_PtrInfo _temp442; void* _temp444; struct Cyc_Absyn_FnInfo
_temp446; struct Cyc_List_List* _temp448; struct Cyc_List_List* _temp450; struct
Cyc_Absyn_VarargInfo* _temp452; int _temp454; struct Cyc_List_List* _temp456;
void* _temp458; struct Cyc_Core_Opt* _temp460; struct Cyc_List_List* _temp462;
struct Cyc_List_List* _temp464; void* _temp466; struct Cyc_Absyn_TunionInfo
_temp468; void* _temp470; struct Cyc_List_List* _temp472; struct Cyc_Core_Opt*
_temp474; struct Cyc_List_List* _temp476; struct Cyc_Absyn_TunionFieldInfo
_temp478; struct Cyc_List_List* _temp480; struct Cyc_List_List* _temp482; struct
Cyc_List_List* _temp484; struct Cyc_List_List* _temp486; void* _temp488; struct
Cyc_List_List* _temp490; void* _temp492; _LL401: if(( unsigned int) _temp399 > 
4u?*(( int*) _temp399) ==  Cyc_Absyn_VarType: 0){ _LL436: _temp435=(( struct Cyc_Absyn_VarType_struct*)
_temp399)->f1; goto _LL402;} else{ goto _LL403;} _LL403: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_Evar: 0){ _LL441: _temp440=((
struct Cyc_Absyn_Evar_struct*) _temp399)->f2; goto _LL438; _LL438: _temp437=((
struct Cyc_Absyn_Evar_struct*) _temp399)->f4; _temp439=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp399)->f4; goto _LL404;} else{ goto _LL405;}
_LL405: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_PointerType:
0){ _LL443: _temp442=(( struct Cyc_Absyn_PointerType_struct*) _temp399)->f1;
goto _LL406;} else{ goto _LL407;} _LL407: if(( unsigned int) _temp399 >  4u?*((
int*) _temp399) ==  Cyc_Absyn_ArrayType: 0){ _LL445: _temp444=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp399)->f1; goto _LL408;} else{ goto _LL409;}
_LL409: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_FnType:
0){ _LL447: _temp446=(( struct Cyc_Absyn_FnType_struct*) _temp399)->f1; _LL463:
_temp462= _temp446.tvars; goto _LL461; _LL461: _temp460= _temp446.effect; goto
_LL459; _LL459: _temp458=( void*) _temp446.ret_typ; goto _LL457; _LL457:
_temp456= _temp446.args; goto _LL455; _LL455: _temp454= _temp446.c_varargs; goto
_LL453; _LL453: _temp452= _temp446.cyc_varargs; goto _LL451; _LL451: _temp450=
_temp446.rgn_po; goto _LL449; _LL449: _temp448= _temp446.attributes; goto _LL410;}
else{ goto _LL411;} _LL411: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399)
==  Cyc_Absyn_TupleType: 0){ _LL465: _temp464=(( struct Cyc_Absyn_TupleType_struct*)
_temp399)->f1; goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_RgnHandleType: 0){ _LL467:
_temp466=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp399)->f1; goto
_LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp399 >  4u?*(( int*)
_temp399) ==  Cyc_Absyn_TunionType: 0){ _LL469: _temp468=(( struct Cyc_Absyn_TunionType_struct*)
_temp399)->f1; _LL473: _temp472= _temp468.targs; goto _LL471; _LL471: _temp470=(
void*) _temp468.rgn; goto _LL416;} else{ goto _LL417;} _LL417: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_TypedefType: 0){ _LL477:
_temp476=(( struct Cyc_Absyn_TypedefType_struct*) _temp399)->f2; goto _LL475;
_LL475: _temp474=(( struct Cyc_Absyn_TypedefType_struct*) _temp399)->f3; goto
_LL418;} else{ goto _LL419;} _LL419: if(( unsigned int) _temp399 >  4u?*(( int*)
_temp399) ==  Cyc_Absyn_TunionFieldType: 0){ _LL479: _temp478=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp399)->f1; _LL481: _temp480= _temp478.targs; goto _LL420;} else{ goto _LL421;}
_LL421: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_StructType:
0){ _LL483: _temp482=(( struct Cyc_Absyn_StructType_struct*) _temp399)->f2; goto
_LL422;} else{ goto _LL423;} _LL423: if(( unsigned int) _temp399 >  4u?*(( int*)
_temp399) ==  Cyc_Absyn_AnonStructType: 0){ _LL485: _temp484=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp399)->f1; goto _LL424;} else{ goto _LL425;} _LL425: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_AnonUnionType: 0){ _LL487:
_temp486=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp399)->f1; goto _LL426;}
else{ goto _LL427;} _LL427: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399)
==  Cyc_Absyn_AccessEff: 0){ _LL489: _temp488=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp399)->f1; goto _LL428;} else{ goto _LL429;} _LL429: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_JoinEff: 0){ _LL491: _temp490=((
struct Cyc_Absyn_JoinEff_struct*) _temp399)->f1; goto _LL430;} else{ goto _LL431;}
_LL431: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_RgnsEff:
0){ _LL493: _temp492=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp399)->f1;
goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL402: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp435)){( int) _throw(( void*) Cyc_Tcutil_Unify);} goto _LL400; _LL404: if( t
==  evar){( int) _throw(( void*) Cyc_Tcutil_Unify);} else{ if( _temp440 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp440))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp439))->v; for( 0; s !=  0; s=(( struct
Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)( int(* compare)( struct
Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar*
x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd)){ problem= 1; break;}}} if( problem){
struct Cyc_List_List* _temp494= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp439))->v; for( 0; s !=  0; s=(( struct
Cyc_List_List*) _check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp494=({ struct Cyc_List_List* _temp495=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp495->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp495->tl= _temp494; _temp495;});}}}*
_temp439=({ struct Cyc_Core_Opt* _temp496=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp496->v=( void*) _temp494; _temp496;});}}}
goto _LL400; _LL406: Cyc_Tcutil_occurs( evar, env,( void*) _temp442.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp442.rgn_typ); goto _LL400; _LL408:
Cyc_Tcutil_occurs( evar, env, _temp444); goto _LL400; _LL410: if( _temp460 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp460))->v);} Cyc_Tcutil_occurs( evar, env, _temp458); for( 0; _temp456 !=  0;
_temp456=(( struct Cyc_List_List*) _check_null( _temp456))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp456))->hd)).f3);}
if( _temp452 !=  0){ void* _temp499; struct Cyc_Absyn_VarargInfo _temp497=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp452)); _LL500: _temp499=( void*)
_temp497.type; goto _LL498; _LL498: Cyc_Tcutil_occurs( evar, env, _temp499);}
for( 0; _temp450 !=  0; _temp450=(( struct Cyc_List_List*) _check_null( _temp450))->tl){
struct _tuple8 _temp503; void* _temp504; void* _temp506; struct _tuple8*
_temp501=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp450))->hd;
_temp503=* _temp501; _LL507: _temp506= _temp503.f1; goto _LL505; _LL505:
_temp504= _temp503.f2; goto _LL502; _LL502: Cyc_Tcutil_occurs( evar, env,
_temp506); Cyc_Tcutil_occurs( evar, env, _temp504);} goto _LL400; _LL412: for( 0;
_temp464 !=  0; _temp464=(( struct Cyc_List_List*) _check_null( _temp464))->tl){
Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp464))->hd)).f2);} goto _LL400; _LL414: Cyc_Tcutil_occurs( evar,
env, _temp466); goto _LL400; _LL416: Cyc_Tcutil_occurs( evar, env, _temp470);
Cyc_Tcutil_occurslist( evar, env, _temp472); goto _LL400; _LL418: _temp480=
_temp476; goto _LL420; _LL420: _temp482= _temp480; goto _LL422; _LL422: Cyc_Tcutil_occurslist(
evar, env, _temp482); goto _LL400; _LL424: _temp486= _temp484; goto _LL426;
_LL426: for( 0; _temp486 !=  0; _temp486=(( struct Cyc_List_List*) _check_null(
_temp486))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp486))->hd)->type);} goto _LL400; _LL428:
Cyc_Tcutil_occurs( evar, env, _temp488); goto _LL400; _LL430: Cyc_Tcutil_occurslist(
evar, env, _temp490); goto _LL400; _LL432: Cyc_Tcutil_occurs( evar, env,
_temp492); goto _LL400; _LL434: goto _LL400; _LL400:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts != 
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp508; _push_handler(& _temp508);{ int _temp510= 0; if( setjmp( _temp508.handler)){
_temp510= 1;} if( ! _temp510){ Cyc_List_iter2( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp509=( void*) _exn_thrown; void* _temp512=
_temp509; _LL514: if( _temp512 ==  Cyc_List_List_mismatch){ goto _LL515;} else{
goto _LL516;} _LL516: goto _LL517; _LL515:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL517:( void) _throw( _temp512); _LL513:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != 
tq2.q_const? 1: tq1.q_volatile !=  tq2.q_volatile)? 1: tq1.q_restrict !=  tq2.q_restrict){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const ==  tq2.q_const? tq1.q_volatile
==  tq2.q_volatile: 0)? tq1.q_restrict ==  tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x == 
y){ return;}{ void* _temp518=( void*) x->v; void* _temp526; _LL520: if( _temp518
== ( void*) Cyc_Absyn_No_constr){ goto _LL521;} else{ goto _LL522;} _LL522: if((
unsigned int) _temp518 >  1u?*(( int*) _temp518) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL523;} else{ goto _LL524;} _LL524: if(( unsigned int) _temp518 >  1u?*((
int*) _temp518) ==  Cyc_Absyn_Eq_constr: 0){ _LL527: _temp526=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp518)->f1; goto _LL525;} else{ goto _LL519;}
_LL521:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp528=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp528[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp529; _temp529.tag= Cyc_Absyn_Forward_constr; _temp529.f1= y; _temp529;});
_temp528;}))); return; _LL523:( int) _throw(({ void* _temp530[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp530, sizeof( void*), 0u));})); _LL525: { void* _temp531=( void*)
y->v; void* _temp539; _LL533: if( _temp531 == ( void*) Cyc_Absyn_No_constr){
goto _LL534;} else{ goto _LL535;} _LL535: if(( unsigned int) _temp531 >  1u?*((
int*) _temp531) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL536;} else{ goto
_LL537;} _LL537: if(( unsigned int) _temp531 >  1u?*(( int*) _temp531) ==  Cyc_Absyn_Eq_constr:
0){ _LL540: _temp539=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp531)->f1;
goto _LL538;} else{ goto _LL532;} _LL534:( void*)( y->v=( void*)(( void*) x->v));
return; _LL536:( int) _throw(({ void* _temp541[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp541, sizeof( void*), 0u));})); _LL538: if( cmp( _temp526,
_temp539) !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL532:;}
_LL519:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp542; _push_handler(& _temp542);{ int _temp544= 0; if( setjmp( _temp542.handler)){
_temp544= 1;} if( ! _temp544){ Cyc_Tcutil_unify_it_conrefs( cmp, x, y);{ int
_temp545= 1; _npop_handler( 0u); return _temp545;}; _pop_handler();} else{ void*
_temp543=( void*) _exn_thrown; void* _temp547= _temp543; _LL549: if( _temp547 == 
Cyc_Tcutil_Unify){ goto _LL550;} else{ goto _LL551;} _LL551: goto _LL552; _LL550:
return 0; _LL552:( void) _throw( _temp547); _LL548:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple8 _temp554=({ struct _tuple8 _temp553;
_temp553.f1= b1; _temp553.f2= b2; _temp553;}); void* _temp564; void* _temp566;
void* _temp568; void* _temp570; void* _temp572; struct Cyc_Absyn_Exp* _temp574;
void* _temp576; struct Cyc_Absyn_Exp* _temp578; _LL556: _LL567: _temp566=
_temp554.f1; if( _temp566 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL565;} else{
goto _LL558;} _LL565: _temp564= _temp554.f2; if( _temp564 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL557;} else{ goto _LL558;} _LL558: _LL569: _temp568= _temp554.f1; if(
_temp568 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL559;} else{ goto _LL560;}
_LL560: _LL571: _temp570= _temp554.f2; if( _temp570 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL561;} else{ goto _LL562;} _LL562: _LL577: _temp576= _temp554.f1; if((
unsigned int) _temp576 >  1u?*(( int*) _temp576) ==  Cyc_Absyn_Upper_b: 0){
_LL579: _temp578=(( struct Cyc_Absyn_Upper_b_struct*) _temp576)->f1; goto _LL573;}
else{ goto _LL555;} _LL573: _temp572= _temp554.f2; if(( unsigned int) _temp572 > 
1u?*(( int*) _temp572) ==  Cyc_Absyn_Upper_b: 0){ _LL575: _temp574=(( struct Cyc_Absyn_Upper_b_struct*)
_temp572)->f1; goto _LL563;} else{ goto _LL555;} _LL557: return 0; _LL559:
return - 1; _LL561: return 1; _LL563: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp578); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp574); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL555:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp581=({
struct _tuple8 _temp580; _temp580.f1= a1; _temp580.f2= a2; _temp580;}); void*
_temp593; int _temp595; int _temp597; void* _temp599; void* _temp601; int
_temp603; int _temp605; void* _temp607; void* _temp609; int _temp611; void*
_temp613; int _temp615; void* _temp617; int _temp619; void* _temp621; int
_temp623; void* _temp625; struct _tagged_arr _temp627; void* _temp629; struct
_tagged_arr _temp631; _LL583: _LL602: _temp601= _temp581.f1; if(( unsigned int)
_temp601 >  16u?*(( int*) _temp601) ==  Cyc_Absyn_Format_att: 0){ _LL608:
_temp607=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp601)->f1; goto
_LL606; _LL606: _temp605=(( struct Cyc_Absyn_Format_att_struct*) _temp601)->f2;
goto _LL604; _LL604: _temp603=(( struct Cyc_Absyn_Format_att_struct*) _temp601)->f3;
goto _LL594;} else{ goto _LL585;} _LL594: _temp593= _temp581.f2; if((
unsigned int) _temp593 >  16u?*(( int*) _temp593) ==  Cyc_Absyn_Format_att: 0){
_LL600: _temp599=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp593)->f1;
goto _LL598; _LL598: _temp597=(( struct Cyc_Absyn_Format_att_struct*) _temp593)->f2;
goto _LL596; _LL596: _temp595=(( struct Cyc_Absyn_Format_att_struct*) _temp593)->f3;
goto _LL584;} else{ goto _LL585;} _LL585: _LL614: _temp613= _temp581.f1; if((
unsigned int) _temp613 >  16u?*(( int*) _temp613) ==  Cyc_Absyn_Regparm_att: 0){
_LL616: _temp615=(( struct Cyc_Absyn_Regparm_att_struct*) _temp613)->f1; goto
_LL610;} else{ goto _LL587;} _LL610: _temp609= _temp581.f2; if(( unsigned int)
_temp609 >  16u?*(( int*) _temp609) ==  Cyc_Absyn_Regparm_att: 0){ _LL612:
_temp611=(( struct Cyc_Absyn_Regparm_att_struct*) _temp609)->f1; goto _LL586;}
else{ goto _LL587;} _LL587: _LL622: _temp621= _temp581.f1; if(( unsigned int)
_temp621 >  16u?*(( int*) _temp621) ==  Cyc_Absyn_Aligned_att: 0){ _LL624:
_temp623=(( struct Cyc_Absyn_Aligned_att_struct*) _temp621)->f1; goto _LL618;}
else{ goto _LL589;} _LL618: _temp617= _temp581.f2; if(( unsigned int) _temp617 > 
16u?*(( int*) _temp617) ==  Cyc_Absyn_Aligned_att: 0){ _LL620: _temp619=((
struct Cyc_Absyn_Aligned_att_struct*) _temp617)->f1; goto _LL588;} else{ goto
_LL589;} _LL589: _LL630: _temp629= _temp581.f1; if(( unsigned int) _temp629 > 
16u?*(( int*) _temp629) ==  Cyc_Absyn_Section_att: 0){ _LL632: _temp631=((
struct Cyc_Absyn_Section_att_struct*) _temp629)->f1; goto _LL626;} else{ goto
_LL591;} _LL626: _temp625= _temp581.f2; if(( unsigned int) _temp625 >  16u?*((
int*) _temp625) ==  Cyc_Absyn_Section_att: 0){ _LL628: _temp627=(( struct Cyc_Absyn_Section_att_struct*)
_temp625)->f1; goto _LL590;} else{ goto _LL591;} _LL591: goto _LL592; _LL584:
return( _temp607 ==  _temp599? _temp605 ==  _temp597: 0)? _temp603 ==  _temp595:
0; _LL586: _temp623= _temp615; _temp619= _temp611; goto _LL588; _LL588: return
_temp623 ==  _temp619; _LL590: return Cyc_Std_strcmp( _temp631, _temp627) ==  0;
_LL592: return 0; _LL582:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,(
void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct
Cyc_List_List* a= a2; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null(
a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void* Cyc_Tcutil_normalize_effect(
void* e); static void* Cyc_Tcutil_rgns_of( void* t); static void* Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Structfield* sf){ return Cyc_Tcutil_rgns_of(( void*) sf->type);}
static struct _tuple5* Cyc_Tcutil_region_free_subst( struct Cyc_Absyn_Tvar* tv){
void* t;{ void* _temp633= Cyc_Absyn_conref_val( tv->kind); _LL635: if( _temp633
== ( void*) Cyc_Absyn_RgnKind){ goto _LL636;} else{ goto _LL637;} _LL637: if(
_temp633 == ( void*) Cyc_Absyn_EffKind){ goto _LL638;} else{ goto _LL639;}
_LL639: goto _LL640; _LL636: t=( void*) Cyc_Absyn_HeapRgn; goto _LL634; _LL638:
t= Cyc_Absyn_empty_effect; goto _LL634; _LL640: t= Cyc_Absyn_sint_t; goto _LL634;
_LL634:;} return({ struct _tuple5* _temp641=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp641->f1= tv; _temp641->f2= t; _temp641;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp642= Cyc_Tcutil_compress(
t); void* _temp690; struct Cyc_Absyn_TunionInfo _temp692; void* _temp694; struct
Cyc_List_List* _temp696; struct Cyc_Absyn_PtrInfo _temp698; void* _temp700; void*
_temp702; void* _temp704; struct Cyc_List_List* _temp706; struct Cyc_Absyn_TunionFieldInfo
_temp708; struct Cyc_List_List* _temp710; struct Cyc_List_List* _temp712; struct
Cyc_List_List* _temp714; struct Cyc_List_List* _temp716; struct Cyc_List_List*
_temp718; struct Cyc_Absyn_FnInfo _temp720; struct Cyc_List_List* _temp722;
struct Cyc_Absyn_VarargInfo* _temp724; struct Cyc_List_List* _temp726; void*
_temp728; struct Cyc_Core_Opt* _temp730; struct Cyc_List_List* _temp732; void*
_temp734; _LL644: if( _temp642 == ( void*) Cyc_Absyn_VoidType){ goto _LL645;}
else{ goto _LL646;} _LL646: if( _temp642 == ( void*) Cyc_Absyn_FloatType){ goto
_LL647;} else{ goto _LL648;} _LL648: if( _temp642 == ( void*) Cyc_Absyn_DoubleType){
goto _LL649;} else{ goto _LL650;} _LL650: if(( unsigned int) _temp642 >  4u?*((
int*) _temp642) ==  Cyc_Absyn_EnumType: 0){ goto _LL651;} else{ goto _LL652;}
_LL652: if(( unsigned int) _temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_IntType:
0){ goto _LL653;} else{ goto _LL654;} _LL654: if(( unsigned int) _temp642 >  4u?*((
int*) _temp642) ==  Cyc_Absyn_Evar: 0){ goto _LL655;} else{ goto _LL656;} _LL656:
if(( unsigned int) _temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_VarType: 0){
goto _LL657;} else{ goto _LL658;} _LL658: if(( unsigned int) _temp642 >  4u?*((
int*) _temp642) ==  Cyc_Absyn_RgnHandleType: 0){ _LL691: _temp690=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp642)->f1; goto _LL659;} else{ goto
_LL660;} _LL660: if(( unsigned int) _temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_TunionType:
0){ _LL693: _temp692=(( struct Cyc_Absyn_TunionType_struct*) _temp642)->f1;
_LL697: _temp696= _temp692.targs; goto _LL695; _LL695: _temp694=( void*)
_temp692.rgn; goto _LL661;} else{ goto _LL662;} _LL662: if(( unsigned int)
_temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_PointerType: 0){ _LL699:
_temp698=(( struct Cyc_Absyn_PointerType_struct*) _temp642)->f1; _LL703:
_temp702=( void*) _temp698.elt_typ; goto _LL701; _LL701: _temp700=( void*)
_temp698.rgn_typ; goto _LL663;} else{ goto _LL664;} _LL664: if(( unsigned int)
_temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_ArrayType: 0){ _LL705: _temp704=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp642)->f1; goto _LL665;} else{
goto _LL666;} _LL666: if(( unsigned int) _temp642 >  4u?*(( int*) _temp642) == 
Cyc_Absyn_TupleType: 0){ _LL707: _temp706=(( struct Cyc_Absyn_TupleType_struct*)
_temp642)->f1; goto _LL667;} else{ goto _LL668;} _LL668: if(( unsigned int)
_temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_TunionFieldType: 0){ _LL709:
_temp708=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp642)->f1; _LL711:
_temp710= _temp708.targs; goto _LL669;} else{ goto _LL670;} _LL670: if((
unsigned int) _temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_StructType: 0){
_LL713: _temp712=(( struct Cyc_Absyn_StructType_struct*) _temp642)->f2; goto
_LL671;} else{ goto _LL672;} _LL672: if(( unsigned int) _temp642 >  4u?*(( int*)
_temp642) ==  Cyc_Absyn_UnionType: 0){ _LL715: _temp714=(( struct Cyc_Absyn_UnionType_struct*)
_temp642)->f2; goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int)
_temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_AnonStructType: 0){ _LL717:
_temp716=(( struct Cyc_Absyn_AnonStructType_struct*) _temp642)->f1; goto _LL675;}
else{ goto _LL676;} _LL676: if(( unsigned int) _temp642 >  4u?*(( int*) _temp642)
==  Cyc_Absyn_AnonUnionType: 0){ _LL719: _temp718=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp642)->f1; goto _LL677;} else{ goto _LL678;} _LL678: if(( unsigned int)
_temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_FnType: 0){ _LL721: _temp720=((
struct Cyc_Absyn_FnType_struct*) _temp642)->f1; _LL733: _temp732= _temp720.tvars;
goto _LL731; _LL731: _temp730= _temp720.effect; goto _LL729; _LL729: _temp728=(
void*) _temp720.ret_typ; goto _LL727; _LL727: _temp726= _temp720.args; goto
_LL725; _LL725: _temp724= _temp720.cyc_varargs; goto _LL723; _LL723: _temp722=
_temp720.rgn_po; goto _LL679;} else{ goto _LL680;} _LL680: if( _temp642 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL681;} else{ goto _LL682;} _LL682: if((
unsigned int) _temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_AccessEff: 0){
goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int) _temp642 >  4u?*((
int*) _temp642) ==  Cyc_Absyn_JoinEff: 0){ goto _LL685;} else{ goto _LL686;}
_LL686: if(( unsigned int) _temp642 >  4u?*(( int*) _temp642) ==  Cyc_Absyn_RgnsEff:
0){ _LL735: _temp734=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp642)->f1;
goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int) _temp642 >  4u?*((
int*) _temp642) ==  Cyc_Absyn_TypedefType: 0){ goto _LL689;} else{ goto _LL643;}
_LL645: goto _LL647; _LL647: goto _LL649; _LL649: goto _LL651; _LL651: goto
_LL653; _LL653: return Cyc_Absyn_empty_effect; _LL655: goto _LL657; _LL657: {
void* _temp736= Cyc_Tcutil_typ_kind( t); _LL738: if( _temp736 == ( void*) Cyc_Absyn_RgnKind){
goto _LL739;} else{ goto _LL740;} _LL740: if( _temp736 == ( void*) Cyc_Absyn_EffKind){
goto _LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL739: return( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp744=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp744[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp745; _temp745.tag= Cyc_Absyn_AccessEff; _temp745.f1=(
void*) t; _temp745;}); _temp744;}); _LL741: return t; _LL743: return( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp746=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp746[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp747; _temp747.tag= Cyc_Absyn_RgnsEff; _temp747.f1=( void*) t; _temp747;});
_temp746;}); _LL737:;} _LL659: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp748=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp748[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp749; _temp749.tag= Cyc_Absyn_AccessEff;
_temp749.f1=( void*) _temp690; _temp749;}); _temp748;}); _LL661: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp752=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp752->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp753=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp753[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp754; _temp754.tag= Cyc_Absyn_AccessEff;
_temp754.f1=( void*) _temp694; _temp754;}); _temp753;})); _temp752->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp696); _temp752;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp750=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp750[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp751; _temp751.tag= Cyc_Absyn_JoinEff; _temp751.f1= ts; _temp751;});
_temp750;}));} _LL663: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp755=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp755[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp756; _temp756.tag= Cyc_Absyn_JoinEff;
_temp756.f1=({ void* _temp757[ 2u]; _temp757[ 1u]= Cyc_Tcutil_rgns_of( _temp702);
_temp757[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp758=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp758[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp759; _temp759.tag= Cyc_Absyn_AccessEff;
_temp759.f1=( void*) _temp700; _temp759;}); _temp758;}); Cyc_List_list( _tag_arr(
_temp757, sizeof( void*), 2u));}); _temp756;}); _temp755;})); _LL665: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp704)); _LL667: { struct Cyc_List_List* _temp760= 0;
for( 0; _temp706 !=  0; _temp706=(( struct Cyc_List_List*) _check_null( _temp706))->tl){
_temp760=({ struct Cyc_List_List* _temp761=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp761->hd=( void*)(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp706))->hd)).f2; _temp761->tl= _temp760;
_temp761;});} _temp710= _temp760; goto _LL669;} _LL669: _temp712= _temp710; goto
_LL671; _LL671: _temp714= _temp712; goto _LL673; _LL673: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp762=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp762[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp763; _temp763.tag= Cyc_Absyn_JoinEff; _temp763.f1= Cyc_List_map( Cyc_Tcutil_rgns_of,
_temp714); _temp763;}); _temp762;})); _LL675: _temp718= _temp716; goto _LL677;
_LL677: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp764=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp764[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp765; _temp765.tag= Cyc_Absyn_JoinEff;
_temp765.f1=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_rgns_of_field, _temp718);
_temp765;}); _temp764;})); _LL679: { void* _temp766= Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst, _temp732),( void*)((
struct Cyc_Core_Opt*) _check_null( _temp730))->v); return Cyc_Tcutil_normalize_effect(
_temp766);} _LL681: return Cyc_Absyn_empty_effect; _LL683: return t; _LL685:
return t; _LL687: return Cyc_Tcutil_rgns_of( _temp734); _LL689: return({ void*
_temp767[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp767, sizeof( void*), 0u));});
_LL643:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp768= e; struct Cyc_List_List* _temp776; struct Cyc_List_List**
_temp778; void* _temp779; _LL770: if(( unsigned int) _temp768 >  4u?*(( int*)
_temp768) ==  Cyc_Absyn_JoinEff: 0){ _LL777: _temp776=(( struct Cyc_Absyn_JoinEff_struct*)
_temp768)->f1; _temp778=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp768)->f1; goto _LL771;} else{ goto _LL772;} _LL772: if(( unsigned int)
_temp768 >  4u?*(( int*) _temp768) ==  Cyc_Absyn_RgnsEff: 0){ _LL780: _temp779=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp768)->f1; goto _LL773;} else{
goto _LL774;} _LL774: goto _LL775; _LL771: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp778; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp781=( void*)(( struct Cyc_List_List*) _check_null( effs))->hd;(
void*)((( struct Cyc_List_List*) _check_null( effs))->hd=( void*) Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect( _temp781)));{ void* _temp782=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; _LL784: if(( unsigned int) _temp782 >  4u?*(( int*)
_temp782) ==  Cyc_Absyn_JoinEff: 0){ goto _LL785;} else{ goto _LL786;} _LL786:
goto _LL787; _LL785: nested_join= 1; goto _LL783; _LL787: goto _LL783; _LL783:;}}}
if( ! nested_join){ return e;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp778; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp788= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp796; void* _temp798; _LL790:
if(( unsigned int) _temp788 >  4u?*(( int*) _temp788) ==  Cyc_Absyn_JoinEff: 0){
_LL797: _temp796=(( struct Cyc_Absyn_JoinEff_struct*) _temp788)->f1; goto _LL791;}
else{ goto _LL792;} _LL792: if(( unsigned int) _temp788 >  4u?*(( int*) _temp788)
==  Cyc_Absyn_AccessEff: 0){ _LL799: _temp798=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp788)->f1; if( _temp798 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL793;} else{
goto _LL794;}} else{ goto _LL794;} _LL794: goto _LL795; _LL791: effects= Cyc_List_revappend(
_temp796, effects); goto _LL789; _LL793: goto _LL789; _LL795: effects=({ struct
Cyc_List_List* _temp800=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp800->hd=( void*) _temp788; _temp800->tl= effects; _temp800;}); goto _LL789;
_LL789:;}}* _temp778= Cyc_List_imp_rev( effects); return e;}} _LL773: return Cyc_Tcutil_rgns_of(
_temp779); _LL775: return e; _LL769:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp801= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp809; struct Cyc_List_List _temp811; struct Cyc_List_List* _temp812; void*
_temp814; struct Cyc_Core_Opt* _temp816; struct Cyc_Core_Opt* _temp818; _LL803:
if(( unsigned int) _temp801 >  4u?*(( int*) _temp801) ==  Cyc_Absyn_JoinEff: 0){
_LL810: _temp809=(( struct Cyc_Absyn_JoinEff_struct*) _temp801)->f1; if(
_temp809 ==  0){ goto _LL805;} else{ _temp811=* _temp809; _LL815: _temp814=(
void*) _temp811.hd; goto _LL813; _LL813: _temp812= _temp811.tl; goto _LL804;}}
else{ goto _LL805;} _LL805: if(( unsigned int) _temp801 >  4u?*(( int*) _temp801)
==  Cyc_Absyn_Evar: 0){ _LL819: _temp818=(( struct Cyc_Absyn_Evar_struct*)
_temp801)->f1; goto _LL817; _LL817: _temp816=(( struct Cyc_Absyn_Evar_struct*)
_temp801)->f4; goto _LL806;} else{ goto _LL807;} _LL807: goto _LL808; _LL804: {
void* _temp820= Cyc_Tcutil_compress( _temp814); struct Cyc_Core_Opt* _temp826;
_LL822: if(( unsigned int) _temp820 >  4u?*(( int*) _temp820) ==  Cyc_Absyn_Evar:
0){ _LL827: _temp826=(( struct Cyc_Absyn_Evar_struct*) _temp820)->f4; goto
_LL823;} else{ goto _LL824;} _LL824: goto _LL825; _LL823: return({ struct
_tuple9* _temp828=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp828->f1= _temp814; _temp828->f2= _temp812; _temp828->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp826))->v; _temp828;}); _LL825: return 0;
_LL821:;} _LL806: if( _temp818 ==  0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp818))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp829[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp829, sizeof( void*), 0u));});} return({ struct _tuple9* _temp830=( struct
_tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp830->f1= t; _temp830->f2= 0;
_temp830->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp816))->v; _temp830;}); _LL808: return 0; _LL802:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp831=({ struct Cyc_Absyn_FnType_struct*
_temp832=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp832[ 0]=({ struct Cyc_Absyn_FnType_struct _temp833; _temp833.tag= Cyc_Absyn_FnType;
_temp833.f1=({ struct Cyc_Absyn_FnInfo _temp834; _temp834.tvars= 0; _temp834.effect=({
struct Cyc_Core_Opt* _temp835=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp835->v=( void*) eff; _temp835;}); _temp834.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp834.args= 0; _temp834.c_varargs= 0; _temp834.cyc_varargs=
0; _temp834.rgn_po= 0; _temp834.attributes= 0; _temp834;}); _temp833;});
_temp832;}); return Cyc_Absyn_atb_typ(( void*) _temp831,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp836= Cyc_Tcutil_compress( e); void*
_temp848; struct Cyc_List_List* _temp850; void* _temp852; struct Cyc_Core_Opt*
_temp854; struct Cyc_Core_Opt* _temp856; struct Cyc_Core_Opt** _temp858; struct
Cyc_Core_Opt* _temp859; _LL838: if(( unsigned int) _temp836 >  4u?*(( int*)
_temp836) ==  Cyc_Absyn_AccessEff: 0){ _LL849: _temp848=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp836)->f1; goto _LL839;} else{ goto _LL840;} _LL840: if(( unsigned int)
_temp836 >  4u?*(( int*) _temp836) ==  Cyc_Absyn_JoinEff: 0){ _LL851: _temp850=((
struct Cyc_Absyn_JoinEff_struct*) _temp836)->f1; goto _LL841;} else{ goto _LL842;}
_LL842: if(( unsigned int) _temp836 >  4u?*(( int*) _temp836) ==  Cyc_Absyn_RgnsEff:
0){ _LL853: _temp852=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp836)->f1;
goto _LL843;} else{ goto _LL844;} _LL844: if(( unsigned int) _temp836 >  4u?*((
int*) _temp836) ==  Cyc_Absyn_Evar: 0){ _LL860: _temp859=(( struct Cyc_Absyn_Evar_struct*)
_temp836)->f1; goto _LL857; _LL857: _temp856=(( struct Cyc_Absyn_Evar_struct*)
_temp836)->f2; _temp858=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp836)->f2; goto _LL855; _LL855: _temp854=(( struct Cyc_Absyn_Evar_struct*)
_temp836)->f4; goto _LL845;} else{ goto _LL846;} _LL846: goto _LL847; _LL839:
if( constrain){ return Cyc_Tcutil_unify( r, _temp848);} _temp848= Cyc_Tcutil_compress(
_temp848); if( r ==  _temp848){ return 1;}{ struct _tuple8 _temp862=({ struct
_tuple8 _temp861; _temp861.f1= r; _temp861.f2= _temp848; _temp861;}); void*
_temp868; struct Cyc_Absyn_Tvar* _temp870; void* _temp872; struct Cyc_Absyn_Tvar*
_temp874; _LL864: _LL873: _temp872= _temp862.f1; if(( unsigned int) _temp872 > 
4u?*(( int*) _temp872) ==  Cyc_Absyn_VarType: 0){ _LL875: _temp874=(( struct Cyc_Absyn_VarType_struct*)
_temp872)->f1; goto _LL869;} else{ goto _LL866;} _LL869: _temp868= _temp862.f2;
if(( unsigned int) _temp868 >  4u?*(( int*) _temp868) ==  Cyc_Absyn_VarType: 0){
_LL871: _temp870=(( struct Cyc_Absyn_VarType_struct*) _temp868)->f1; goto _LL865;}
else{ goto _LL866;} _LL866: goto _LL867; _LL865: return Cyc_Absyn_tvar_cmp(
_temp874, _temp870) ==  0; _LL867: return 0; _LL863:;} _LL841: for( 0; _temp850
!=  0; _temp850=(( struct Cyc_List_List*) _check_null( _temp850))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp850))->hd)){
return 1;}} return 0; _LL843: { void* _temp876= Cyc_Tcutil_rgns_of( _temp852);
void* _temp882; _LL878: if(( unsigned int) _temp876 >  4u?*(( int*) _temp876) == 
Cyc_Absyn_RgnsEff: 0){ _LL883: _temp882=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp876)->f1; goto _LL879;} else{ goto _LL880;} _LL880: goto _LL881; _LL879:
if( ! constrain){ return 0;}{ void* _temp884= Cyc_Tcutil_compress( _temp882);
struct Cyc_Core_Opt* _temp890; struct Cyc_Core_Opt* _temp892; struct Cyc_Core_Opt**
_temp894; struct Cyc_Core_Opt* _temp895; _LL886: if(( unsigned int) _temp884 > 
4u?*(( int*) _temp884) ==  Cyc_Absyn_Evar: 0){ _LL896: _temp895=(( struct Cyc_Absyn_Evar_struct*)
_temp884)->f1; goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_Evar_struct*)
_temp884)->f2; _temp894=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp884)->f2; goto _LL891; _LL891: _temp890=(( struct Cyc_Absyn_Evar_struct*)
_temp884)->f4; goto _LL887;} else{ goto _LL888;} _LL888: goto _LL889; _LL887: {
void* _temp897= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp890); Cyc_Tcutil_occurs( _temp897,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp890))->v, r);{ void* _temp898= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp900=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp900[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp901; _temp901.tag= Cyc_Absyn_JoinEff; _temp901.f1=({ void* _temp902[ 2u];
_temp902[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp903=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp903[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp904; _temp904.tag= Cyc_Absyn_AccessEff;
_temp904.f1=( void*) r; _temp904;}); _temp903;}); _temp902[ 0u]= _temp897; Cyc_List_list(
_tag_arr( _temp902, sizeof( void*), 2u));}); _temp901;}); _temp900;}));*
_temp894=({ struct Cyc_Core_Opt* _temp899=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp899->v=( void*) _temp898; _temp899;});
return 1;}} _LL889: return 0; _LL885:;} _LL881: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp876); _LL877:;} _LL845: if( _temp859 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp859))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp905[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp905, sizeof( void*), 0u));});} if( ! constrain){ return 0;}{
void* _temp906= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp854); Cyc_Tcutil_occurs( _temp906,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp854))->v, r);{ struct Cyc_Absyn_JoinEff_struct* _temp907=({
struct Cyc_Absyn_JoinEff_struct* _temp909=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp909[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp910; _temp910.tag= Cyc_Absyn_JoinEff; _temp910.f1=({ struct Cyc_List_List*
_temp911=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp911->hd=( void*) _temp906; _temp911->tl=({ struct Cyc_List_List* _temp912=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp912->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp913=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp913[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp914; _temp914.tag= Cyc_Absyn_AccessEff; _temp914.f1=(
void*) r; _temp914;}); _temp913;})); _temp912->tl= 0; _temp912;}); _temp911;});
_temp910;}); _temp909;});* _temp858=({ struct Cyc_Core_Opt* _temp908=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp908->v=( void*)((
void*) _temp907); _temp908;}); return 1;}} _LL847: return 0; _LL837:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp915= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp927;
void* _temp929; struct Cyc_Core_Opt* _temp931; struct Cyc_Core_Opt* _temp933;
struct Cyc_Core_Opt** _temp935; struct Cyc_Core_Opt* _temp936; _LL917: if((
unsigned int) _temp915 >  4u?*(( int*) _temp915) ==  Cyc_Absyn_AccessEff: 0){
goto _LL918;} else{ goto _LL919;} _LL919: if(( unsigned int) _temp915 >  4u?*((
int*) _temp915) ==  Cyc_Absyn_JoinEff: 0){ _LL928: _temp927=(( struct Cyc_Absyn_JoinEff_struct*)
_temp915)->f1; goto _LL920;} else{ goto _LL921;} _LL921: if(( unsigned int)
_temp915 >  4u?*(( int*) _temp915) ==  Cyc_Absyn_RgnsEff: 0){ _LL930: _temp929=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp915)->f1; goto _LL922;} else{
goto _LL923;} _LL923: if(( unsigned int) _temp915 >  4u?*(( int*) _temp915) == 
Cyc_Absyn_Evar: 0){ _LL937: _temp936=(( struct Cyc_Absyn_Evar_struct*) _temp915)->f1;
goto _LL934; _LL934: _temp933=(( struct Cyc_Absyn_Evar_struct*) _temp915)->f2;
_temp935=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp915)->f2;
goto _LL932; _LL932: _temp931=(( struct Cyc_Absyn_Evar_struct*) _temp915)->f4;
goto _LL924;} else{ goto _LL925;} _LL925: goto _LL926; _LL918: return 0; _LL920:
for( 0; _temp927 !=  0; _temp927=(( struct Cyc_List_List*) _check_null( _temp927))->tl){
if( Cyc_Tcutil_type_in_effect( constrain, t,( void*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd)){ return 1;}} return 0; _LL922: _temp929= Cyc_Tcutil_compress(
_temp929); if( t ==  _temp929){ return 1;} if( constrain){ return Cyc_Tcutil_unify(
t, _temp929);}{ void* _temp938= Cyc_Tcutil_rgns_of( t); void* _temp944; _LL940:
if(( unsigned int) _temp938 >  4u?*(( int*) _temp938) ==  Cyc_Absyn_RgnsEff: 0){
_LL945: _temp944=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp938)->f1;
goto _LL941;} else{ goto _LL942;} _LL942: goto _LL943; _LL941: { struct _tuple8
_temp947=({ struct _tuple8 _temp946; _temp946.f1= t; _temp946.f2= Cyc_Tcutil_compress(
_temp944); _temp946;}); void* _temp953; struct Cyc_Absyn_Tvar* _temp955; void*
_temp957; struct Cyc_Absyn_Tvar* _temp959; _LL949: _LL958: _temp957= _temp947.f1;
if(( unsigned int) _temp957 >  4u?*(( int*) _temp957) ==  Cyc_Absyn_VarType: 0){
_LL960: _temp959=(( struct Cyc_Absyn_VarType_struct*) _temp957)->f1; goto _LL954;}
else{ goto _LL951;} _LL954: _temp953= _temp947.f2; if(( unsigned int) _temp953 > 
4u?*(( int*) _temp953) ==  Cyc_Absyn_VarType: 0){ _LL956: _temp955=(( struct Cyc_Absyn_VarType_struct*)
_temp953)->f1; goto _LL950;} else{ goto _LL951;} _LL951: goto _LL952; _LL950:
return Cyc_Tcutil_unify( t, _temp944); _LL952: return t ==  _temp944; _LL948:;}
_LL943: return Cyc_Tcutil_type_in_effect( constrain, t, _temp938); _LL939:;}
_LL924: if( _temp936 ==  0? 1:( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp936))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp961[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp961, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp962= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp931); Cyc_Tcutil_occurs( _temp962,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp931))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp963=({ struct Cyc_Absyn_JoinEff_struct* _temp965=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp965[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp966; _temp966.tag= Cyc_Absyn_JoinEff; _temp966.f1=({ struct Cyc_List_List*
_temp967=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp967->hd=( void*) _temp962; _temp967->tl=({ struct Cyc_List_List* _temp968=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp968->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp969=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp969[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp970; _temp970.tag= Cyc_Absyn_RgnsEff; _temp970.f1=( void*) t; _temp970;});
_temp969;})); _temp968->tl= 0; _temp968;}); _temp967;}); _temp966;}); _temp965;});*
_temp935=({ struct Cyc_Core_Opt* _temp964=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp964->v=( void*)(( void*) _temp963); _temp964;});
return 1;}} _LL926: return 0; _LL916:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp971= e; struct Cyc_Absyn_Tvar* _temp983; struct Cyc_List_List*
_temp985; void* _temp987; struct Cyc_Core_Opt* _temp989; struct Cyc_Core_Opt*
_temp991; struct Cyc_Core_Opt** _temp993; struct Cyc_Core_Opt* _temp994; _LL973:
if(( unsigned int) _temp971 >  4u?*(( int*) _temp971) ==  Cyc_Absyn_VarType: 0){
_LL984: _temp983=(( struct Cyc_Absyn_VarType_struct*) _temp971)->f1; goto _LL974;}
else{ goto _LL975;} _LL975: if(( unsigned int) _temp971 >  4u?*(( int*) _temp971)
==  Cyc_Absyn_JoinEff: 0){ _LL986: _temp985=(( struct Cyc_Absyn_JoinEff_struct*)
_temp971)->f1; goto _LL976;} else{ goto _LL977;} _LL977: if(( unsigned int)
_temp971 >  4u?*(( int*) _temp971) ==  Cyc_Absyn_RgnsEff: 0){ _LL988: _temp987=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp971)->f1; goto _LL978;} else{
goto _LL979;} _LL979: if(( unsigned int) _temp971 >  4u?*(( int*) _temp971) == 
Cyc_Absyn_Evar: 0){ _LL995: _temp994=(( struct Cyc_Absyn_Evar_struct*) _temp971)->f1;
goto _LL992; _LL992: _temp991=(( struct Cyc_Absyn_Evar_struct*) _temp971)->f2;
_temp993=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp971)->f2;
goto _LL990; _LL990: _temp989=(( struct Cyc_Absyn_Evar_struct*) _temp971)->f4;
goto _LL980;} else{ goto _LL981;} _LL981: goto _LL982; _LL974: return Cyc_Absyn_tvar_cmp(
v, _temp983) ==  0; _LL976: for( 0; _temp985 !=  0; _temp985=(( struct Cyc_List_List*)
_check_null( _temp985))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp985))->hd)){ return 1;}}
return 0; _LL978: { void* _temp996= Cyc_Tcutil_rgns_of( _temp987); void*
_temp1002; _LL998: if(( unsigned int) _temp996 >  4u?*(( int*) _temp996) ==  Cyc_Absyn_RgnsEff:
0){ _LL1003: _temp1002=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp996)->f1;
goto _LL999;} else{ goto _LL1000;} _LL1000: goto _LL1001; _LL999: if( !
constrain){ return 0;}{ void* _temp1004= Cyc_Tcutil_compress( _temp1002); struct
Cyc_Core_Opt* _temp1010; struct Cyc_Core_Opt* _temp1012; struct Cyc_Core_Opt**
_temp1014; struct Cyc_Core_Opt* _temp1015; _LL1006: if(( unsigned int) _temp1004
>  4u?*(( int*) _temp1004) ==  Cyc_Absyn_Evar: 0){ _LL1016: _temp1015=(( struct
Cyc_Absyn_Evar_struct*) _temp1004)->f1; goto _LL1013; _LL1013: _temp1012=((
struct Cyc_Absyn_Evar_struct*) _temp1004)->f2; _temp1014=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1004)->f2; goto _LL1011; _LL1011: _temp1010=((
struct Cyc_Absyn_Evar_struct*) _temp1004)->f4; goto _LL1007;} else{ goto _LL1008;}
_LL1008: goto _LL1009; _LL1007: { void* _temp1017= Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp1010); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1010))->v, v)){ return 0;}{ void*
_temp1018= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1020=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1020[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1021; _temp1021.tag= Cyc_Absyn_JoinEff;
_temp1021.f1=({ void* _temp1022[ 2u]; _temp1022[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1023=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1023[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1024; _temp1024.tag= Cyc_Absyn_VarType;
_temp1024.f1= v; _temp1024;}); _temp1023;}); _temp1022[ 0u]= _temp1017; Cyc_List_list(
_tag_arr( _temp1022, sizeof( void*), 2u));}); _temp1021;}); _temp1020;}));*
_temp1014=({ struct Cyc_Core_Opt* _temp1019=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1019->v=( void*) _temp1018; _temp1019;});
return 1;}} _LL1009: return 0; _LL1005:;} _LL1001: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp996); _LL997:;} _LL980: if( _temp994 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp994))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp1025[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp1025, sizeof( void*), 0u));});}{ void* _temp1026= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp989); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp989))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp1027=({ struct Cyc_Absyn_JoinEff_struct* _temp1029=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1029[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1030; _temp1030.tag= Cyc_Absyn_JoinEff; _temp1030.f1=({
struct Cyc_List_List* _temp1031=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1031->hd=( void*) _temp1026; _temp1031->tl=({
struct Cyc_List_List* _temp1032=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1032->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1033=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1033[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1034; _temp1034.tag= Cyc_Absyn_VarType;
_temp1034.f1= v; _temp1034;}); _temp1033;})); _temp1032->tl= 0; _temp1032;});
_temp1031;}); _temp1030;}); _temp1029;});* _temp993=({ struct Cyc_Core_Opt*
_temp1028=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1028->v=( void*)(( void*) _temp1027); _temp1028;}); return 1;}} _LL982:
return 0; _LL972:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp1035= e; struct Cyc_List_List* _temp1045;
void* _temp1047; _LL1037: if(( unsigned int) _temp1035 >  4u?*(( int*) _temp1035)
==  Cyc_Absyn_JoinEff: 0){ _LL1046: _temp1045=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1035)->f1; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(( unsigned int)
_temp1035 >  4u?*(( int*) _temp1035) ==  Cyc_Absyn_RgnsEff: 0){ _LL1048:
_temp1047=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1035)->f1; goto
_LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int) _temp1035 >  4u?*((
int*) _temp1035) ==  Cyc_Absyn_Evar: 0){ goto _LL1042;} else{ goto _LL1043;}
_LL1043: goto _LL1044; _LL1038: for( 0; _temp1045 !=  0; _temp1045=(( struct Cyc_List_List*)
_check_null( _temp1045))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp1045))->hd)){ return 1;}} return 0;
_LL1040: { void* _temp1049= Cyc_Tcutil_rgns_of( _temp1047); void* _temp1055;
_LL1051: if(( unsigned int) _temp1049 >  4u?*(( int*) _temp1049) ==  Cyc_Absyn_RgnsEff:
0){ _LL1056: _temp1055=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1049)->f1;
goto _LL1052;} else{ goto _LL1053;} _LL1053: goto _LL1054; _LL1052: return 0;
_LL1054: return Cyc_Tcutil_evar_in_effect( evar, _temp1049); _LL1050:;} _LL1042:
return evar ==  e; _LL1044: return 0; _LL1036:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1057= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1071; void* _temp1073; struct Cyc_Absyn_Tvar*
_temp1075; void* _temp1077; struct Cyc_Core_Opt* _temp1079; struct Cyc_Core_Opt*
_temp1081; struct Cyc_Core_Opt** _temp1083; _LL1059: if(( unsigned int)
_temp1057 >  4u?*(( int*) _temp1057) ==  Cyc_Absyn_JoinEff: 0){ _LL1072:
_temp1071=(( struct Cyc_Absyn_JoinEff_struct*) _temp1057)->f1; goto _LL1060;}
else{ goto _LL1061;} _LL1061: if(( unsigned int) _temp1057 >  4u?*(( int*)
_temp1057) ==  Cyc_Absyn_AccessEff: 0){ _LL1074: _temp1073=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1057)->f1; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(( unsigned int)
_temp1057 >  4u?*(( int*) _temp1057) ==  Cyc_Absyn_VarType: 0){ _LL1076:
_temp1075=(( struct Cyc_Absyn_VarType_struct*) _temp1057)->f1; goto _LL1064;}
else{ goto _LL1065;} _LL1065: if(( unsigned int) _temp1057 >  4u?*(( int*)
_temp1057) ==  Cyc_Absyn_RgnsEff: 0){ _LL1078: _temp1077=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1057)->f1; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(( unsigned int)
_temp1057 >  4u?*(( int*) _temp1057) ==  Cyc_Absyn_Evar: 0){ _LL1082: _temp1081=((
struct Cyc_Absyn_Evar_struct*) _temp1057)->f2; _temp1083=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1057)->f2; goto _LL1080; _LL1080: _temp1079=((
struct Cyc_Absyn_Evar_struct*) _temp1057)->f4; goto _LL1068;} else{ goto _LL1069;}
_LL1069: goto _LL1070; _LL1060: for( 0; _temp1071 !=  0; _temp1071=(( struct Cyc_List_List*)
_check_null( _temp1071))->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*)(( struct Cyc_List_List*) _check_null( _temp1071))->hd, e2)){ return 0;}}
return 1; _LL1062: return( Cyc_Tcutil_region_in_effect( 0, _temp1073, e2)? 1:
Cyc_Tcutil_region_in_effect( 1, _temp1073, e2))? 1: Cyc_Tcutil_unify( _temp1073,(
void*) Cyc_Absyn_HeapRgn); _LL1064: return Cyc_Tcutil_variable_in_effect( 0,
_temp1075, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1075, e2); _LL1066: {
void* _temp1084= Cyc_Tcutil_rgns_of( _temp1077); void* _temp1090; _LL1086: if((
unsigned int) _temp1084 >  4u?*(( int*) _temp1084) ==  Cyc_Absyn_RgnsEff: 0){
_LL1091: _temp1090=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1084)->f1;
goto _LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089; _LL1087: return( Cyc_Tcutil_type_in_effect(
0, _temp1090, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1090, e2))? 1: Cyc_Tcutil_unify(
_temp1090, Cyc_Absyn_sint_t); _LL1089: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1084, e2); _LL1085:;} _LL1068: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1083=({ struct Cyc_Core_Opt* _temp1092=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1092->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1093=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1093[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1094; _temp1094.tag= Cyc_Absyn_JoinEff; _temp1094.f1=
0; _temp1094;}); _temp1093;})); _temp1092;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1079))->v, e2);*
_temp1083=({ struct Cyc_Core_Opt* _temp1095=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1095->v=( void*) e2; _temp1095;});}} return
1; _LL1070: return({ struct Cyc_Std_String_pa_struct _temp1097; _temp1097.tag=
Cyc_Std_String_pa; _temp1097.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1096[ 1u]={& _temp1097};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1096, sizeof( void*), 1u));}});
_LL1058:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1099=({ struct
_tuple10 _temp1098; _temp1098.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1098.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1098;}); _LL1101: goto _LL1102; _LL1102:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1100:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8 _temp1105;
void* _temp1106; void* _temp1108; struct _tuple8* _temp1103=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( r1))->hd; _temp1105=* _temp1103; _LL1109:
_temp1108= _temp1105.f1; goto _LL1107; _LL1107: _temp1106= _temp1105.f2; goto
_LL1104; _LL1104: { int found= _temp1108 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2=(( struct Cyc_List_List*)
_check_null( r2))->tl){ struct _tuple8 _temp1112; void* _temp1113; void*
_temp1115; struct _tuple8* _temp1110=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( r2))->hd; _temp1112=* _temp1110; _LL1116: _temp1115= _temp1112.f1;
goto _LL1114; _LL1114: _temp1113= _temp1112.f2; goto _LL1111; _LL1111: if( Cyc_Tcutil_unify(
_temp1108, _temp1115)? Cyc_Tcutil_unify( _temp1106, _temp1113): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 ==  t2){
return;}{ void* _temp1117= t1; struct Cyc_Core_Opt* _temp1123; struct Cyc_Core_Opt*
_temp1125; struct Cyc_Core_Opt** _temp1127; struct Cyc_Core_Opt* _temp1128;
_LL1119: if(( unsigned int) _temp1117 >  4u?*(( int*) _temp1117) ==  Cyc_Absyn_Evar:
0){ _LL1129: _temp1128=(( struct Cyc_Absyn_Evar_struct*) _temp1117)->f1; goto
_LL1126; _LL1126: _temp1125=(( struct Cyc_Absyn_Evar_struct*) _temp1117)->f2;
_temp1127=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1117)->f2;
goto _LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_Evar_struct*) _temp1117)->f4;
goto _LL1120;} else{ goto _LL1121;} _LL1121: goto _LL1122; _LL1120: Cyc_Tcutil_occurs(
t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1123))->v,
t2);{ void* _temp1130= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq(
_temp1130,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1128))->v)){*
_temp1127=({ struct Cyc_Core_Opt* _temp1131=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1131->v=( void*) t2; _temp1131;}); return;}
else{{ void* _temp1132= t2; struct Cyc_Core_Opt* _temp1140; struct Cyc_Core_Opt*
_temp1142; struct Cyc_Core_Opt** _temp1144; struct Cyc_Absyn_PtrInfo _temp1146;
_LL1134: if(( unsigned int) _temp1132 >  4u?*(( int*) _temp1132) ==  Cyc_Absyn_Evar:
0){ _LL1143: _temp1142=(( struct Cyc_Absyn_Evar_struct*) _temp1132)->f2;
_temp1144=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1132)->f2;
goto _LL1141; _LL1141: _temp1140=(( struct Cyc_Absyn_Evar_struct*) _temp1132)->f4;
goto _LL1135;} else{ goto _LL1136;} _LL1136: if(( unsigned int) _temp1132 >  4u?*((
int*) _temp1132) ==  Cyc_Absyn_PointerType: 0){ _LL1147: _temp1146=(( struct Cyc_Absyn_PointerType_struct*)
_temp1132)->f1; goto _LL1145;} else{ goto _LL1138;} _LL1145: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1128))->v == ( void*) Cyc_Absyn_BoxKind){
goto _LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139; _LL1135: { struct Cyc_List_List*
_temp1148=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1123))->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1140))->v; for( 0; s2 !=  0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1148,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1128))->v, _temp1130)){*
_temp1144=({ struct Cyc_Core_Opt* _temp1149=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1149->v=( void*) t1; _temp1149;}); return;}
goto _LL1133;} _LL1137: { struct Cyc_Absyn_Conref* _temp1150= Cyc_Absyn_compress_conref(
_temp1146.bounds);{ void* _temp1151=( void*) _temp1150->v; _LL1153: if(
_temp1151 == ( void*) Cyc_Absyn_No_constr){ goto _LL1154;} else{ goto _LL1155;}
_LL1155: goto _LL1156; _LL1154:( void*)( _temp1150->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp1157=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1157[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1158; _temp1158.tag= Cyc_Absyn_Eq_constr;
_temp1158.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1159=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1159[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1160; _temp1160.tag= Cyc_Absyn_Upper_b;
_temp1160.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1160;}); _temp1159;}));
_temp1158;}); _temp1157;})));* _temp1127=({ struct Cyc_Core_Opt* _temp1161=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1161->v=(
void*) t2; _temp1161;}); return; _LL1156: goto _LL1152; _LL1152:;} goto _LL1133;}
_LL1139: goto _LL1133; _LL1133:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}}
_LL1122: goto _LL1118; _LL1118:;}{ struct _tuple8 _temp1163=({ struct _tuple8
_temp1162; _temp1162.f1= t2; _temp1162.f2= t1; _temp1162;}); void* _temp1217;
void* _temp1219; void* _temp1221; void* _temp1223; struct Cyc_Absyn_Tvar*
_temp1225; void* _temp1227; struct Cyc_Absyn_Tvar* _temp1229; void* _temp1231;
struct Cyc_Absyn_Structdecl** _temp1233; struct Cyc_List_List* _temp1235; struct
_tuple1* _temp1237; void* _temp1239; struct Cyc_Absyn_Structdecl** _temp1241;
struct Cyc_List_List* _temp1243; struct _tuple1* _temp1245; void* _temp1247;
struct _tuple1* _temp1249; void* _temp1251; struct _tuple1* _temp1253; void*
_temp1255; struct Cyc_Absyn_Uniondecl** _temp1257; struct Cyc_List_List*
_temp1259; struct _tuple1* _temp1261; void* _temp1263; struct Cyc_Absyn_Uniondecl**
_temp1265; struct Cyc_List_List* _temp1267; struct _tuple1* _temp1269; void*
_temp1271; struct Cyc_Absyn_TunionInfo _temp1273; void* _temp1275; struct Cyc_List_List*
_temp1277; void* _temp1279; struct Cyc_Absyn_Tuniondecl* _temp1281; void*
_temp1283; struct Cyc_Absyn_TunionInfo _temp1285; void* _temp1287; struct Cyc_List_List*
_temp1289; void* _temp1291; struct Cyc_Absyn_Tuniondecl* _temp1293; void*
_temp1295; struct Cyc_Absyn_TunionFieldInfo _temp1297; struct Cyc_List_List*
_temp1299; void* _temp1301; struct Cyc_Absyn_Tunionfield* _temp1303; struct Cyc_Absyn_Tuniondecl*
_temp1305; void* _temp1307; struct Cyc_Absyn_TunionFieldInfo _temp1309; struct
Cyc_List_List* _temp1311; void* _temp1313; struct Cyc_Absyn_Tunionfield*
_temp1315; struct Cyc_Absyn_Tuniondecl* _temp1317; void* _temp1319; struct Cyc_Absyn_PtrInfo
_temp1321; struct Cyc_Absyn_Conref* _temp1323; struct Cyc_Absyn_Tqual _temp1325;
struct Cyc_Absyn_Conref* _temp1327; void* _temp1329; void* _temp1331; void*
_temp1333; struct Cyc_Absyn_PtrInfo _temp1335; struct Cyc_Absyn_Conref*
_temp1337; struct Cyc_Absyn_Tqual _temp1339; struct Cyc_Absyn_Conref* _temp1341;
void* _temp1343; void* _temp1345; void* _temp1347; void* _temp1349; void*
_temp1351; void* _temp1353; void* _temp1355; void* _temp1357; void* _temp1359;
void* _temp1361; void* _temp1363; void* _temp1365; void* _temp1367; struct Cyc_Absyn_Exp*
_temp1369; struct Cyc_Absyn_Tqual _temp1371; void* _temp1373; void* _temp1375;
struct Cyc_Absyn_Exp* _temp1377; struct Cyc_Absyn_Tqual _temp1379; void*
_temp1381; void* _temp1383; struct Cyc_Absyn_FnInfo _temp1385; struct Cyc_List_List*
_temp1387; struct Cyc_List_List* _temp1389; struct Cyc_Absyn_VarargInfo*
_temp1391; int _temp1393; struct Cyc_List_List* _temp1395; void* _temp1397;
struct Cyc_Core_Opt* _temp1399; struct Cyc_List_List* _temp1401; void* _temp1403;
struct Cyc_Absyn_FnInfo _temp1405; struct Cyc_List_List* _temp1407; struct Cyc_List_List*
_temp1409; struct Cyc_Absyn_VarargInfo* _temp1411; int _temp1413; struct Cyc_List_List*
_temp1415; void* _temp1417; struct Cyc_Core_Opt* _temp1419; struct Cyc_List_List*
_temp1421; void* _temp1423; struct Cyc_List_List* _temp1425; void* _temp1427;
struct Cyc_List_List* _temp1429; void* _temp1431; struct Cyc_List_List*
_temp1433; void* _temp1435; struct Cyc_List_List* _temp1437; void* _temp1439;
struct Cyc_List_List* _temp1441; void* _temp1443; struct Cyc_List_List*
_temp1445; void* _temp1447; void* _temp1449; void* _temp1451; void* _temp1453;
void* _temp1455; void* _temp1457; void* _temp1459; void* _temp1461; void*
_temp1463; void* _temp1465; void* _temp1467; void* _temp1469; _LL1165: _LL1218:
_temp1217= _temp1163.f1; if(( unsigned int) _temp1217 >  4u?*(( int*) _temp1217)
==  Cyc_Absyn_Evar: 0){ goto _LL1166;} else{ goto _LL1167;} _LL1167: _LL1222:
_temp1221= _temp1163.f1; if( _temp1221 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1220;} else{ goto _LL1169;} _LL1220: _temp1219= _temp1163.f2; if( _temp1219
== ( void*) Cyc_Absyn_VoidType){ goto _LL1168;} else{ goto _LL1169;} _LL1169:
_LL1228: _temp1227= _temp1163.f1; if(( unsigned int) _temp1227 >  4u?*(( int*)
_temp1227) ==  Cyc_Absyn_VarType: 0){ _LL1230: _temp1229=(( struct Cyc_Absyn_VarType_struct*)
_temp1227)->f1; goto _LL1224;} else{ goto _LL1171;} _LL1224: _temp1223=
_temp1163.f2; if(( unsigned int) _temp1223 >  4u?*(( int*) _temp1223) ==  Cyc_Absyn_VarType:
0){ _LL1226: _temp1225=(( struct Cyc_Absyn_VarType_struct*) _temp1223)->f1; goto
_LL1170;} else{ goto _LL1171;} _LL1171: _LL1240: _temp1239= _temp1163.f1; if((
unsigned int) _temp1239 >  4u?*(( int*) _temp1239) ==  Cyc_Absyn_StructType: 0){
_LL1246: _temp1245=(( struct Cyc_Absyn_StructType_struct*) _temp1239)->f1; goto
_LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_StructType_struct*) _temp1239)->f2;
goto _LL1242; _LL1242: _temp1241=(( struct Cyc_Absyn_StructType_struct*)
_temp1239)->f3; goto _LL1232;} else{ goto _LL1173;} _LL1232: _temp1231=
_temp1163.f2; if(( unsigned int) _temp1231 >  4u?*(( int*) _temp1231) ==  Cyc_Absyn_StructType:
0){ _LL1238: _temp1237=(( struct Cyc_Absyn_StructType_struct*) _temp1231)->f1;
goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_StructType_struct*)
_temp1231)->f2; goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_StructType_struct*)
_temp1231)->f3; goto _LL1172;} else{ goto _LL1173;} _LL1173: _LL1252: _temp1251=
_temp1163.f1; if(( unsigned int) _temp1251 >  4u?*(( int*) _temp1251) ==  Cyc_Absyn_EnumType:
0){ _LL1254: _temp1253=(( struct Cyc_Absyn_EnumType_struct*) _temp1251)->f1;
goto _LL1248;} else{ goto _LL1175;} _LL1248: _temp1247= _temp1163.f2; if((
unsigned int) _temp1247 >  4u?*(( int*) _temp1247) ==  Cyc_Absyn_EnumType: 0){
_LL1250: _temp1249=(( struct Cyc_Absyn_EnumType_struct*) _temp1247)->f1; goto
_LL1174;} else{ goto _LL1175;} _LL1175: _LL1264: _temp1263= _temp1163.f1; if((
unsigned int) _temp1263 >  4u?*(( int*) _temp1263) ==  Cyc_Absyn_UnionType: 0){
_LL1270: _temp1269=(( struct Cyc_Absyn_UnionType_struct*) _temp1263)->f1; goto
_LL1268; _LL1268: _temp1267=(( struct Cyc_Absyn_UnionType_struct*) _temp1263)->f2;
goto _LL1266; _LL1266: _temp1265=(( struct Cyc_Absyn_UnionType_struct*)
_temp1263)->f3; goto _LL1256;} else{ goto _LL1177;} _LL1256: _temp1255=
_temp1163.f2; if(( unsigned int) _temp1255 >  4u?*(( int*) _temp1255) ==  Cyc_Absyn_UnionType:
0){ _LL1262: _temp1261=(( struct Cyc_Absyn_UnionType_struct*) _temp1255)->f1;
goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_UnionType_struct*)
_temp1255)->f2; goto _LL1258; _LL1258: _temp1257=(( struct Cyc_Absyn_UnionType_struct*)
_temp1255)->f3; goto _LL1176;} else{ goto _LL1177;} _LL1177: _LL1284: _temp1283=
_temp1163.f1; if(( unsigned int) _temp1283 >  4u?*(( int*) _temp1283) ==  Cyc_Absyn_TunionType:
0){ _LL1286: _temp1285=(( struct Cyc_Absyn_TunionType_struct*) _temp1283)->f1;
_LL1292: _temp1291=( void*) _temp1285.tunion_info; if(*(( int*) _temp1291) == 
Cyc_Absyn_KnownTunion){ _LL1294: _temp1293=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1291)->f1; goto _LL1290;} else{ goto _LL1179;} _LL1290: _temp1289=
_temp1285.targs; goto _LL1288; _LL1288: _temp1287=( void*) _temp1285.rgn; goto
_LL1272;} else{ goto _LL1179;} _LL1272: _temp1271= _temp1163.f2; if((
unsigned int) _temp1271 >  4u?*(( int*) _temp1271) ==  Cyc_Absyn_TunionType: 0){
_LL1274: _temp1273=(( struct Cyc_Absyn_TunionType_struct*) _temp1271)->f1;
_LL1280: _temp1279=( void*) _temp1273.tunion_info; if(*(( int*) _temp1279) == 
Cyc_Absyn_KnownTunion){ _LL1282: _temp1281=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1279)->f1; goto _LL1278;} else{ goto _LL1179;} _LL1278: _temp1277=
_temp1273.targs; goto _LL1276; _LL1276: _temp1275=( void*) _temp1273.rgn; goto
_LL1178;} else{ goto _LL1179;} _LL1179: _LL1308: _temp1307= _temp1163.f1; if((
unsigned int) _temp1307 >  4u?*(( int*) _temp1307) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1310: _temp1309=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1307)->f1;
_LL1314: _temp1313=( void*) _temp1309.field_info; if(*(( int*) _temp1313) == 
Cyc_Absyn_KnownTunionfield){ _LL1318: _temp1317=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1313)->f1; goto _LL1316; _LL1316: _temp1315=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1313)->f2; goto _LL1312;} else{ goto _LL1181;} _LL1312: _temp1311=
_temp1309.targs; goto _LL1296;} else{ goto _LL1181;} _LL1296: _temp1295=
_temp1163.f2; if(( unsigned int) _temp1295 >  4u?*(( int*) _temp1295) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1298: _temp1297=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1295)->f1;
_LL1302: _temp1301=( void*) _temp1297.field_info; if(*(( int*) _temp1301) == 
Cyc_Absyn_KnownTunionfield){ _LL1306: _temp1305=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1301)->f1; goto _LL1304; _LL1304: _temp1303=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1301)->f2; goto _LL1300;} else{ goto _LL1181;} _LL1300: _temp1299=
_temp1297.targs; goto _LL1180;} else{ goto _LL1181;} _LL1181: _LL1334: _temp1333=
_temp1163.f1; if(( unsigned int) _temp1333 >  4u?*(( int*) _temp1333) ==  Cyc_Absyn_PointerType:
0){ _LL1336: _temp1335=(( struct Cyc_Absyn_PointerType_struct*) _temp1333)->f1;
_LL1346: _temp1345=( void*) _temp1335.elt_typ; goto _LL1344; _LL1344: _temp1343=(
void*) _temp1335.rgn_typ; goto _LL1342; _LL1342: _temp1341= _temp1335.nullable;
goto _LL1340; _LL1340: _temp1339= _temp1335.tq; goto _LL1338; _LL1338: _temp1337=
_temp1335.bounds; goto _LL1320;} else{ goto _LL1183;} _LL1320: _temp1319=
_temp1163.f2; if(( unsigned int) _temp1319 >  4u?*(( int*) _temp1319) ==  Cyc_Absyn_PointerType:
0){ _LL1322: _temp1321=(( struct Cyc_Absyn_PointerType_struct*) _temp1319)->f1;
_LL1332: _temp1331=( void*) _temp1321.elt_typ; goto _LL1330; _LL1330: _temp1329=(
void*) _temp1321.rgn_typ; goto _LL1328; _LL1328: _temp1327= _temp1321.nullable;
goto _LL1326; _LL1326: _temp1325= _temp1321.tq; goto _LL1324; _LL1324: _temp1323=
_temp1321.bounds; goto _LL1182;} else{ goto _LL1183;} _LL1183: _LL1354:
_temp1353= _temp1163.f1; if(( unsigned int) _temp1353 >  4u?*(( int*) _temp1353)
==  Cyc_Absyn_IntType: 0){ _LL1358: _temp1357=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1353)->f1; goto _LL1356; _LL1356: _temp1355=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1353)->f2; goto _LL1348;} else{ goto _LL1185;} _LL1348: _temp1347=
_temp1163.f2; if(( unsigned int) _temp1347 >  4u?*(( int*) _temp1347) ==  Cyc_Absyn_IntType:
0){ _LL1352: _temp1351=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1347)->f1;
goto _LL1350; _LL1350: _temp1349=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1347)->f2; goto _LL1184;} else{ goto _LL1185;} _LL1185: _LL1362: _temp1361=
_temp1163.f1; if( _temp1361 == ( void*) Cyc_Absyn_FloatType){ goto _LL1360;}
else{ goto _LL1187;} _LL1360: _temp1359= _temp1163.f2; if( _temp1359 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1186;} else{ goto _LL1187;} _LL1187: _LL1366:
_temp1365= _temp1163.f1; if( _temp1365 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1364;} else{ goto _LL1189;} _LL1364: _temp1363= _temp1163.f2; if( _temp1363
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1188;} else{ goto _LL1189;} _LL1189:
_LL1376: _temp1375= _temp1163.f1; if(( unsigned int) _temp1375 >  4u?*(( int*)
_temp1375) ==  Cyc_Absyn_ArrayType: 0){ _LL1382: _temp1381=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1375)->f1; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1375)->f2; goto _LL1378; _LL1378: _temp1377=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1375)->f3; goto _LL1368;} else{ goto _LL1191;} _LL1368: _temp1367=
_temp1163.f2; if(( unsigned int) _temp1367 >  4u?*(( int*) _temp1367) ==  Cyc_Absyn_ArrayType:
0){ _LL1374: _temp1373=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1367)->f1;
goto _LL1372; _LL1372: _temp1371=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1367)->f2; goto _LL1370; _LL1370: _temp1369=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1367)->f3; goto _LL1190;} else{ goto _LL1191;} _LL1191: _LL1404: _temp1403=
_temp1163.f1; if(( unsigned int) _temp1403 >  4u?*(( int*) _temp1403) ==  Cyc_Absyn_FnType:
0){ _LL1406: _temp1405=(( struct Cyc_Absyn_FnType_struct*) _temp1403)->f1;
_LL1422: _temp1421= _temp1405.tvars; goto _LL1420; _LL1420: _temp1419= _temp1405.effect;
goto _LL1418; _LL1418: _temp1417=( void*) _temp1405.ret_typ; goto _LL1416;
_LL1416: _temp1415= _temp1405.args; goto _LL1414; _LL1414: _temp1413= _temp1405.c_varargs;
goto _LL1412; _LL1412: _temp1411= _temp1405.cyc_varargs; goto _LL1410; _LL1410:
_temp1409= _temp1405.rgn_po; goto _LL1408; _LL1408: _temp1407= _temp1405.attributes;
goto _LL1384;} else{ goto _LL1193;} _LL1384: _temp1383= _temp1163.f2; if((
unsigned int) _temp1383 >  4u?*(( int*) _temp1383) ==  Cyc_Absyn_FnType: 0){
_LL1386: _temp1385=(( struct Cyc_Absyn_FnType_struct*) _temp1383)->f1; _LL1402:
_temp1401= _temp1385.tvars; goto _LL1400; _LL1400: _temp1399= _temp1385.effect;
goto _LL1398; _LL1398: _temp1397=( void*) _temp1385.ret_typ; goto _LL1396;
_LL1396: _temp1395= _temp1385.args; goto _LL1394; _LL1394: _temp1393= _temp1385.c_varargs;
goto _LL1392; _LL1392: _temp1391= _temp1385.cyc_varargs; goto _LL1390; _LL1390:
_temp1389= _temp1385.rgn_po; goto _LL1388; _LL1388: _temp1387= _temp1385.attributes;
goto _LL1192;} else{ goto _LL1193;} _LL1193: _LL1428: _temp1427= _temp1163.f1;
if(( unsigned int) _temp1427 >  4u?*(( int*) _temp1427) ==  Cyc_Absyn_TupleType:
0){ _LL1430: _temp1429=(( struct Cyc_Absyn_TupleType_struct*) _temp1427)->f1;
goto _LL1424;} else{ goto _LL1195;} _LL1424: _temp1423= _temp1163.f2; if((
unsigned int) _temp1423 >  4u?*(( int*) _temp1423) ==  Cyc_Absyn_TupleType: 0){
_LL1426: _temp1425=(( struct Cyc_Absyn_TupleType_struct*) _temp1423)->f1; goto
_LL1194;} else{ goto _LL1195;} _LL1195: _LL1436: _temp1435= _temp1163.f1; if((
unsigned int) _temp1435 >  4u?*(( int*) _temp1435) ==  Cyc_Absyn_AnonStructType:
0){ _LL1438: _temp1437=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1435)->f1;
goto _LL1432;} else{ goto _LL1197;} _LL1432: _temp1431= _temp1163.f2; if((
unsigned int) _temp1431 >  4u?*(( int*) _temp1431) ==  Cyc_Absyn_AnonStructType:
0){ _LL1434: _temp1433=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1431)->f1;
goto _LL1196;} else{ goto _LL1197;} _LL1197: _LL1444: _temp1443= _temp1163.f1;
if(( unsigned int) _temp1443 >  4u?*(( int*) _temp1443) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1446: _temp1445=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1443)->f1;
goto _LL1440;} else{ goto _LL1199;} _LL1440: _temp1439= _temp1163.f2; if((
unsigned int) _temp1439 >  4u?*(( int*) _temp1439) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1442: _temp1441=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1439)->f1;
goto _LL1198;} else{ goto _LL1199;} _LL1199: _LL1450: _temp1449= _temp1163.f1;
if( _temp1449 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1448;} else{ goto _LL1201;}
_LL1448: _temp1447= _temp1163.f2; if( _temp1447 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1200;} else{ goto _LL1201;} _LL1201: _LL1456: _temp1455= _temp1163.f1;
if(( unsigned int) _temp1455 >  4u?*(( int*) _temp1455) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1458: _temp1457=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1455)->f1; goto _LL1452;} else{ goto _LL1203;} _LL1452: _temp1451=
_temp1163.f2; if(( unsigned int) _temp1451 >  4u?*(( int*) _temp1451) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1454: _temp1453=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1451)->f1; goto _LL1202;} else{ goto _LL1203;} _LL1203: _LL1460: _temp1459=
_temp1163.f1; if(( unsigned int) _temp1459 >  4u?*(( int*) _temp1459) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1204;} else{ goto _LL1205;} _LL1205: _LL1462: _temp1461= _temp1163.f2;
if(( unsigned int) _temp1461 >  4u?*(( int*) _temp1461) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1206;} else{ goto _LL1207;} _LL1207: _LL1464: _temp1463= _temp1163.f1;
if(( unsigned int) _temp1463 >  4u?*(( int*) _temp1463) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1208;} else{ goto _LL1209;} _LL1209: _LL1466: _temp1465= _temp1163.f1;
if(( unsigned int) _temp1465 >  4u?*(( int*) _temp1465) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1210;} else{ goto _LL1211;} _LL1211: _LL1468: _temp1467= _temp1163.f2;
if(( unsigned int) _temp1467 >  4u?*(( int*) _temp1467) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1212;} else{ goto _LL1213;} _LL1213: _LL1470: _temp1469= _temp1163.f2;
if(( unsigned int) _temp1469 >  4u?*(( int*) _temp1469) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1214;} else{ goto _LL1215;} _LL1215: goto _LL1216; _LL1166: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1168: return; _LL1170: { struct _tagged_arr* _temp1471=
_temp1229->name; struct _tagged_arr* _temp1472= _temp1225->name; int _temp1473=*((
int*) _check_null( _temp1229->identity)); int _temp1474=*(( int*) _check_null(
_temp1225->identity)); void* _temp1475= Cyc_Absyn_conref_val( _temp1229->kind);
void* _temp1476= Cyc_Absyn_conref_val( _temp1225->kind); if( _temp1474 == 
_temp1473? Cyc_Std_zstrptrcmp( _temp1471, _temp1472) ==  0: 0){ if( _temp1475 != 
_temp1476){({ struct Cyc_Std_String_pa_struct _temp1480; _temp1480.tag= Cyc_Std_String_pa;
_temp1480.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1476);{ struct
Cyc_Std_String_pa_struct _temp1479; _temp1479.tag= Cyc_Std_String_pa; _temp1479.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1475);{ struct Cyc_Std_String_pa_struct
_temp1478; _temp1478.tag= Cyc_Std_String_pa; _temp1478.f1=( struct _tagged_arr)*
_temp1471;{ void* _temp1477[ 3u]={& _temp1478,& _temp1479,& _temp1480};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1477, sizeof( void*), 3u));}}}});}
return;} goto _LL1164;} _LL1172: if((( _temp1237 !=  0? _temp1245 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1237),( struct _tuple1*) _check_null(
_temp1245)) ==  0: 0)? 1:( _temp1237 ==  0? _temp1245 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1235, _temp1243); return;} goto _LL1164; _LL1174: if( Cyc_Absyn_qvar_cmp(
_temp1253, _temp1249) ==  0){ return;} goto _LL1164; _LL1176: if((( _temp1261 != 
0? _temp1269 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1261),( struct _tuple1*) _check_null( _temp1269)) ==  0: 0)? 1:( _temp1261
==  0? _temp1269 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1259, _temp1267);
return;} goto _LL1164; _LL1178: if( _temp1293 ==  _temp1281? 1: Cyc_Absyn_qvar_cmp(
_temp1293->name, _temp1281->name) ==  0){ Cyc_Tcutil_unify_it( _temp1275,
_temp1287); Cyc_Tcutil_unify_list( _temp1277, _temp1289); return;} goto _LL1164;
_LL1180: if(( _temp1317 ==  _temp1305? 1: Cyc_Absyn_qvar_cmp( _temp1317->name,
_temp1305->name) ==  0)? _temp1315 ==  _temp1303? 1: Cyc_Absyn_qvar_cmp(
_temp1315->name, _temp1303->name) ==  0: 0){ Cyc_Tcutil_unify_list( _temp1299,
_temp1311); return;} goto _LL1164; _LL1182: Cyc_Tcutil_unify_it( _temp1331,
_temp1345); Cyc_Tcutil_unify_it( _temp1343, _temp1329); Cyc_Tcutil_unify_tqual(
_temp1325, _temp1339); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1323, _temp1337);{ void* _temp1481=( void*)( Cyc_Absyn_compress_conref(
_temp1323))->v; void* _temp1487; _LL1483: if(( unsigned int) _temp1481 >  1u?*((
int*) _temp1481) ==  Cyc_Absyn_Eq_constr: 0){ _LL1488: _temp1487=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1481)->f1; if( _temp1487 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1484;} else{ goto _LL1485;}} else{ goto _LL1485;}
_LL1485: goto _LL1486; _LL1484: return; _LL1486: goto _LL1482; _LL1482:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp, _temp1327, _temp1341); return;
_LL1184: if( _temp1351 ==  _temp1357? _temp1349 ==  _temp1355: 0){ return;} goto
_LL1164; _LL1186: return; _LL1188: return; _LL1190: Cyc_Tcutil_unify_it(
_temp1373, _temp1381); Cyc_Tcutil_unify_tqual( _temp1371, _temp1379); if(
_temp1377 ==  _temp1369){ return;} if( _temp1377 ==  0? 1: _temp1369 ==  0){
goto _LL1164;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1377)) ==  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1369))){ return;} goto _LL1164; _LL1192: { int done= 0;{
struct _RegionHandle _temp1489= _new_region(); struct _RegionHandle* rgn=&
_temp1489; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1401
!=  0) { if( _temp1421 ==  0){ break;} inst=({ struct Cyc_List_List* _temp1490=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1490->hd=( void*)({ struct _tuple5* _temp1491=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1491->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1421))->hd; _temp1491->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1492=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1492[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1493; _temp1493.tag= Cyc_Absyn_VarType; _temp1493.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1401))->hd;
_temp1493;}); _temp1492;}); _temp1491;}); _temp1490->tl= inst; _temp1490;});
_temp1401=(( struct Cyc_List_List*) _check_null( _temp1401))->tl; _temp1421=((
struct Cyc_List_List*) _check_null( _temp1421))->tl;} if( _temp1421 !=  0){
_npop_handler( 0u); goto _LL1164;} if( inst !=  0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1494=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1494[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1495; _temp1495.tag= Cyc_Absyn_FnType; _temp1495.f1=({ struct Cyc_Absyn_FnInfo
_temp1496; _temp1496.tvars= 0; _temp1496.effect= _temp1399; _temp1496.ret_typ=(
void*) _temp1397; _temp1496.args= _temp1395; _temp1496.c_varargs= _temp1393;
_temp1496.cyc_varargs= _temp1391; _temp1496.rgn_po= _temp1389; _temp1496.attributes=
_temp1387; _temp1496;}); _temp1495;}); _temp1494;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1497=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1497[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1498; _temp1498.tag= Cyc_Absyn_FnType; _temp1498.f1=({ struct Cyc_Absyn_FnInfo
_temp1499; _temp1499.tvars= 0; _temp1499.effect= _temp1419; _temp1499.ret_typ=(
void*) _temp1417; _temp1499.args= _temp1415; _temp1499.c_varargs= _temp1413;
_temp1499.cyc_varargs= _temp1411; _temp1499.rgn_po= _temp1409; _temp1499.attributes=
_temp1407; _temp1499;}); _temp1498;}); _temp1497;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1397, _temp1417); for( 0;
_temp1395 !=  0? _temp1415 !=  0: 0;( _temp1395=(( struct Cyc_List_List*)
_check_null( _temp1395))->tl, _temp1415=(( struct Cyc_List_List*) _check_null(
_temp1415))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1395))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1415))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1395))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1415))->hd)).f3);} if( _temp1395 !=  0?
1: _temp1415 !=  0){ goto _LL1164;} if( _temp1393 !=  _temp1413){ goto _LL1164;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1501=({ struct _tuple11 _temp1500;
_temp1500.f1= _temp1391; _temp1500.f2= _temp1411; _temp1500;}); struct Cyc_Absyn_VarargInfo*
_temp1511; struct Cyc_Absyn_VarargInfo* _temp1513; struct Cyc_Absyn_VarargInfo*
_temp1515; struct Cyc_Absyn_VarargInfo* _temp1517; struct Cyc_Absyn_VarargInfo*
_temp1519; struct Cyc_Absyn_VarargInfo _temp1521; int _temp1522; void* _temp1524;
struct Cyc_Absyn_Tqual _temp1526; struct Cyc_Core_Opt* _temp1528; struct Cyc_Absyn_VarargInfo*
_temp1530; struct Cyc_Absyn_VarargInfo _temp1532; int _temp1533; void* _temp1535;
struct Cyc_Absyn_Tqual _temp1537; struct Cyc_Core_Opt* _temp1539; _LL1503:
_LL1514: _temp1513= _temp1501.f1; if( _temp1513 ==  0){ goto _LL1512;} else{
goto _LL1505;} _LL1512: _temp1511= _temp1501.f2; if( _temp1511 ==  0){ goto
_LL1504;} else{ goto _LL1505;} _LL1505: _LL1516: _temp1515= _temp1501.f1; if(
_temp1515 ==  0){ goto _LL1506;} else{ goto _LL1507;} _LL1507: _LL1518:
_temp1517= _temp1501.f2; if( _temp1517 ==  0){ goto _LL1508;} else{ goto _LL1509;}
_LL1509: _LL1531: _temp1530= _temp1501.f1; if( _temp1530 ==  0){ goto _LL1502;}
else{ _temp1532=* _temp1530; _LL1540: _temp1539= _temp1532.name; goto _LL1538;
_LL1538: _temp1537= _temp1532.tq; goto _LL1536; _LL1536: _temp1535=( void*)
_temp1532.type; goto _LL1534; _LL1534: _temp1533= _temp1532.inject; goto _LL1520;}
_LL1520: _temp1519= _temp1501.f2; if( _temp1519 ==  0){ goto _LL1502;} else{
_temp1521=* _temp1519; _LL1529: _temp1528= _temp1521.name; goto _LL1527; _LL1527:
_temp1526= _temp1521.tq; goto _LL1525; _LL1525: _temp1524=( void*) _temp1521.type;
goto _LL1523; _LL1523: _temp1522= _temp1521.inject; goto _LL1510;} _LL1504: goto
_LL1502; _LL1506: goto _LL1508; _LL1508: bad_cyc_vararg= 1; goto _LL1502;
_LL1510: Cyc_Tcutil_unify_tqual( _temp1537, _temp1526); Cyc_Tcutil_unify_it(
_temp1535, _temp1524); if( _temp1533 !=  _temp1522){ bad_cyc_vararg= 1;} goto
_LL1502; _LL1502:;} if( bad_cyc_vararg){ goto _LL1164;}{ int bad_effect= 0;{
struct _tuple12 _temp1542=({ struct _tuple12 _temp1541; _temp1541.f1= _temp1399;
_temp1541.f2= _temp1419; _temp1541;}); struct Cyc_Core_Opt* _temp1552; struct
Cyc_Core_Opt* _temp1554; struct Cyc_Core_Opt* _temp1556; struct Cyc_Core_Opt*
_temp1558; _LL1544: _LL1555: _temp1554= _temp1542.f1; if( _temp1554 ==  0){ goto
_LL1553;} else{ goto _LL1546;} _LL1553: _temp1552= _temp1542.f2; if( _temp1552
==  0){ goto _LL1545;} else{ goto _LL1546;} _LL1546: _LL1557: _temp1556=
_temp1542.f1; if( _temp1556 ==  0){ goto _LL1547;} else{ goto _LL1548;} _LL1548:
_LL1559: _temp1558= _temp1542.f2; if( _temp1558 ==  0){ goto _LL1549;} else{
goto _LL1550;} _LL1550: goto _LL1551; _LL1545: goto _LL1543; _LL1547: goto
_LL1549; _LL1549: bad_effect= 1; goto _LL1543; _LL1551: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1399))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1419))->v); goto _LL1543; _LL1543:;} if( bad_effect){ goto
_LL1164;} if( ! Cyc_Tcutil_same_atts( _temp1407, _temp1387)){ goto _LL1164;} if(
! Cyc_Tcutil_same_rgn_po( _temp1409, _temp1389)){ goto _LL1164;} return;}}}
_LL1194: for( 0; _temp1425 !=  0? _temp1429 !=  0: 0;( _temp1425=(( struct Cyc_List_List*)
_check_null( _temp1425))->tl, _temp1429=(( struct Cyc_List_List*) _check_null(
_temp1429))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1425))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1429))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1425))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1429))->hd)).f2);} if( _temp1425 ==  0?
_temp1429 ==  0: 0){ return;} goto _LL1164; _LL1196: _temp1445= _temp1437;
_temp1441= _temp1433; goto _LL1198; _LL1198: for( 0; _temp1441 !=  0? _temp1445
!=  0: 0;( _temp1441=(( struct Cyc_List_List*) _check_null( _temp1441))->tl,
_temp1445=(( struct Cyc_List_List*) _check_null( _temp1445))->tl)){ struct Cyc_Absyn_Structfield*
_temp1560=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1441))->hd; struct Cyc_Absyn_Structfield* _temp1561=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1445))->hd; if( Cyc_Std_zstrptrcmp(
_temp1560->name, _temp1561->name) !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual( _temp1560->tq, _temp1561->tq); Cyc_Tcutil_unify_it((
void*) _temp1560->type,( void*) _temp1561->type); if((( ! Cyc_Tcutil_same_atts(
_temp1560->attributes, _temp1561->attributes)? 1:( _temp1560->width !=  0?
_temp1561->width ==  0: 0))? 1:( _temp1561->width !=  0? _temp1560->width ==  0:
0))? 1:(( _temp1560->width !=  0? _temp1561->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1560->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1561->width)): 0)){( int) _throw(( void*)
Cyc_Tcutil_Unify);}} if( _temp1441 ==  0? _temp1445 ==  0: 0){ return;} goto
_LL1164; _LL1200: return; _LL1202: Cyc_Tcutil_unify_it( _temp1457, _temp1453);
return; _LL1204: goto _LL1206; _LL1206: goto _LL1208; _LL1208: goto _LL1210;
_LL1210: goto _LL1212; _LL1212: goto _LL1214; _LL1214: if( Cyc_Tcutil_unify_effect(
t1, t2)){ return;} else{ goto _LL1164;} _LL1216: goto _LL1164; _LL1164:;}( int)
_throw(( void*) Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp1562= Cyc_Tcutil_compress( t); _LL1564: if(( unsigned int) _temp1562
>  4u?*(( int*) _temp1562) ==  Cyc_Absyn_IntType: 0){ goto _LL1565;} else{ goto
_LL1566;} _LL1566: if( _temp1562 == ( void*) Cyc_Absyn_FloatType){ goto _LL1567;}
else{ goto _LL1568;} _LL1568: if( _temp1562 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1569;} else{ goto _LL1570;} _LL1570: if(( unsigned int) _temp1562 >  4u?*((
int*) _temp1562) ==  Cyc_Absyn_EnumType: 0){ goto _LL1571;} else{ goto _LL1572;}
_LL1572: goto _LL1573; _LL1565: return 1; _LL1567: return 1; _LL1569: return 1;
_LL1571: return 1; _LL1573: return 0; _LL1563:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1575=({ struct _tuple8 _temp1574; _temp1574.f1= t1;
_temp1574.f2= t2; _temp1574;}); void* _temp1597; void* _temp1599; void*
_temp1601; void* _temp1603; void* _temp1605; void* _temp1607; void* _temp1609;
void* _temp1611; void* _temp1613; void* _temp1615; void* _temp1617; void*
_temp1619; void* _temp1621; void* _temp1623; void* _temp1625; void* _temp1627;
void* _temp1629; void* _temp1631; void* _temp1633; void* _temp1635; void*
_temp1637; void* _temp1639; void* _temp1641; void* _temp1643; void* _temp1645;
void* _temp1647; void* _temp1649; _LL1577: _LL1600: _temp1599= _temp1575.f1; if(
_temp1599 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1598;} else{ goto _LL1579;}
_LL1598: _temp1597= _temp1575.f2; if( _temp1597 == ( void*) Cyc_Absyn_FloatType){
goto _LL1578;} else{ goto _LL1579;} _LL1579: _LL1604: _temp1603= _temp1575.f1;
if( _temp1603 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1602;} else{ goto
_LL1581;} _LL1602: _temp1601= _temp1575.f2; if(( unsigned int) _temp1601 >  4u?*((
int*) _temp1601) ==  Cyc_Absyn_IntType: 0){ goto _LL1580;} else{ goto _LL1581;}
_LL1581: _LL1608: _temp1607= _temp1575.f1; if( _temp1607 == ( void*) Cyc_Absyn_FloatType){
goto _LL1606;} else{ goto _LL1583;} _LL1606: _temp1605= _temp1575.f2; if((
unsigned int) _temp1605 >  4u?*(( int*) _temp1605) ==  Cyc_Absyn_IntType: 0){
goto _LL1582;} else{ goto _LL1583;} _LL1583: _LL1614: _temp1613= _temp1575.f1;
if(( unsigned int) _temp1613 >  4u?*(( int*) _temp1613) ==  Cyc_Absyn_IntType: 0){
_LL1616: _temp1615=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1613)->f2;
if( _temp1615 == ( void*) Cyc_Absyn_B8){ goto _LL1610;} else{ goto _LL1585;}}
else{ goto _LL1585;} _LL1610: _temp1609= _temp1575.f2; if(( unsigned int)
_temp1609 >  4u?*(( int*) _temp1609) ==  Cyc_Absyn_IntType: 0){ _LL1612:
_temp1611=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1609)->f2; if(
_temp1611 == ( void*) Cyc_Absyn_B8){ goto _LL1584;} else{ goto _LL1585;}} else{
goto _LL1585;} _LL1585: _LL1618: _temp1617= _temp1575.f1; if(( unsigned int)
_temp1617 >  4u?*(( int*) _temp1617) ==  Cyc_Absyn_IntType: 0){ _LL1620:
_temp1619=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1617)->f2; if(
_temp1619 == ( void*) Cyc_Absyn_B8){ goto _LL1586;} else{ goto _LL1587;}} else{
goto _LL1587;} _LL1587: _LL1624: _temp1623= _temp1575.f1; if(( unsigned int)
_temp1623 >  4u?*(( int*) _temp1623) ==  Cyc_Absyn_IntType: 0){ _LL1626:
_temp1625=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1623)->f2; if(
_temp1625 == ( void*) Cyc_Absyn_B4){ goto _LL1622;} else{ goto _LL1589;}} else{
goto _LL1589;} _LL1622: _temp1621= _temp1575.f2; if( _temp1621 == ( void*) Cyc_Absyn_FloatType){
goto _LL1588;} else{ goto _LL1589;} _LL1589: _LL1632: _temp1631= _temp1575.f1;
if(( unsigned int) _temp1631 >  4u?*(( int*) _temp1631) ==  Cyc_Absyn_IntType: 0){
_LL1634: _temp1633=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1631)->f2;
if( _temp1633 == ( void*) Cyc_Absyn_B4){ goto _LL1628;} else{ goto _LL1591;}}
else{ goto _LL1591;} _LL1628: _temp1627= _temp1575.f2; if(( unsigned int)
_temp1627 >  4u?*(( int*) _temp1627) ==  Cyc_Absyn_IntType: 0){ _LL1630:
_temp1629=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1627)->f2; if(
_temp1629 == ( void*) Cyc_Absyn_B2){ goto _LL1590;} else{ goto _LL1591;}} else{
goto _LL1591;} _LL1591: _LL1640: _temp1639= _temp1575.f1; if(( unsigned int)
_temp1639 >  4u?*(( int*) _temp1639) ==  Cyc_Absyn_IntType: 0){ _LL1642:
_temp1641=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1639)->f2; if(
_temp1641 == ( void*) Cyc_Absyn_B4){ goto _LL1636;} else{ goto _LL1593;}} else{
goto _LL1593;} _LL1636: _temp1635= _temp1575.f2; if(( unsigned int) _temp1635 > 
4u?*(( int*) _temp1635) ==  Cyc_Absyn_IntType: 0){ _LL1638: _temp1637=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1635)->f2; if( _temp1637 == ( void*) Cyc_Absyn_B1){
goto _LL1592;} else{ goto _LL1593;}} else{ goto _LL1593;} _LL1593: _LL1648:
_temp1647= _temp1575.f1; if(( unsigned int) _temp1647 >  4u?*(( int*) _temp1647)
==  Cyc_Absyn_IntType: 0){ _LL1650: _temp1649=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1647)->f2; if( _temp1649 == ( void*) Cyc_Absyn_B2){ goto _LL1644;} else{
goto _LL1595;}} else{ goto _LL1595;} _LL1644: _temp1643= _temp1575.f2; if((
unsigned int) _temp1643 >  4u?*(( int*) _temp1643) ==  Cyc_Absyn_IntType: 0){
_LL1646: _temp1645=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1643)->f2;
if( _temp1645 == ( void*) Cyc_Absyn_B1){ goto _LL1594;} else{ goto _LL1595;}}
else{ goto _LL1595;} _LL1595: goto _LL1596; _LL1578: return 1; _LL1580: return 1;
_LL1582: return 1; _LL1584: return 0; _LL1586: return 1; _LL1588: return 1;
_LL1590: return 1; _LL1592: return 1; _LL1594: return 1; _LL1596: return 0;
_LL1576:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t,
struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List*
el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*) _check_null( el))->tl){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1651=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1651->v=( void*)
t1; _temp1651;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify( t,(
void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp1654; _temp1654.tag= Cyc_Std_String_pa; _temp1654.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1653; _temp1653.tag= Cyc_Std_String_pa;
_temp1653.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1652[
2u]={& _temp1653,& _temp1654}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1652, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1655= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1657: if(( unsigned int) _temp1655 >  4u?*(( int*) _temp1655) ==  Cyc_Absyn_PointerType:
0){ goto _LL1658;} else{ goto _LL1659;} _LL1659: goto _LL1660; _LL1658: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1656; _LL1660: return 0; _LL1656:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1661= Cyc_Tcutil_compress(
t); _LL1663: if(( unsigned int) _temp1661 >  4u?*(( int*) _temp1661) ==  Cyc_Absyn_IntType:
0){ goto _LL1664;} else{ goto _LL1665;} _LL1665: if(( unsigned int) _temp1661 > 
4u?*(( int*) _temp1661) ==  Cyc_Absyn_EnumType: 0){ goto _LL1666;} else{ goto
_LL1667;} _LL1667: goto _LL1668; _LL1664: return 1; _LL1666: return 1; _LL1668:
return 0; _LL1662:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1669[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1669, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1670[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1670, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1672=({ struct _tuple8 _temp1671; _temp1671.f1= t1; _temp1671.f2= t2;
_temp1671;}); void* _temp1684; struct Cyc_Absyn_PtrInfo _temp1686; void*
_temp1688; struct Cyc_Absyn_PtrInfo _temp1690; void* _temp1692; struct Cyc_Absyn_Exp*
_temp1694; struct Cyc_Absyn_Tqual _temp1696; void* _temp1698; void* _temp1700;
struct Cyc_Absyn_Exp* _temp1702; struct Cyc_Absyn_Tqual _temp1704; void*
_temp1706; void* _temp1708; struct Cyc_Absyn_TunionInfo _temp1710; struct Cyc_List_List*
_temp1712; void* _temp1714; struct Cyc_Absyn_Tuniondecl* _temp1716; void*
_temp1718; struct Cyc_Absyn_TunionFieldInfo _temp1720; struct Cyc_List_List*
_temp1722; void* _temp1724; struct Cyc_Absyn_Tunionfield* _temp1726; struct Cyc_Absyn_Tuniondecl*
_temp1728; void* _temp1730; struct Cyc_Absyn_TunionInfo _temp1732; void*
_temp1734; struct Cyc_List_List* _temp1736; void* _temp1738; struct Cyc_Absyn_Tuniondecl*
_temp1740; void* _temp1742; struct Cyc_Absyn_PtrInfo _temp1744; struct Cyc_Absyn_Conref*
_temp1746; struct Cyc_Absyn_Tqual _temp1748; struct Cyc_Absyn_Conref* _temp1750;
void* _temp1752; void* _temp1754; _LL1674: _LL1689: _temp1688= _temp1672.f1; if((
unsigned int) _temp1688 >  4u?*(( int*) _temp1688) ==  Cyc_Absyn_PointerType: 0){
_LL1691: _temp1690=(( struct Cyc_Absyn_PointerType_struct*) _temp1688)->f1; goto
_LL1685;} else{ goto _LL1676;} _LL1685: _temp1684= _temp1672.f2; if((
unsigned int) _temp1684 >  4u?*(( int*) _temp1684) ==  Cyc_Absyn_PointerType: 0){
_LL1687: _temp1686=(( struct Cyc_Absyn_PointerType_struct*) _temp1684)->f1; goto
_LL1675;} else{ goto _LL1676;} _LL1676: _LL1701: _temp1700= _temp1672.f1; if((
unsigned int) _temp1700 >  4u?*(( int*) _temp1700) ==  Cyc_Absyn_ArrayType: 0){
_LL1707: _temp1706=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1700)->f1;
goto _LL1705; _LL1705: _temp1704=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1700)->f2; goto _LL1703; _LL1703: _temp1702=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1700)->f3; goto _LL1693;} else{ goto _LL1678;} _LL1693: _temp1692=
_temp1672.f2; if(( unsigned int) _temp1692 >  4u?*(( int*) _temp1692) ==  Cyc_Absyn_ArrayType:
0){ _LL1699: _temp1698=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1692)->f1;
goto _LL1697; _LL1697: _temp1696=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1692)->f2; goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1692)->f3; goto _LL1677;} else{ goto _LL1678;} _LL1678: _LL1719: _temp1718=
_temp1672.f1; if(( unsigned int) _temp1718 >  4u?*(( int*) _temp1718) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1721: _temp1720=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1718)->f1;
_LL1725: _temp1724=( void*) _temp1720.field_info; if(*(( int*) _temp1724) == 
Cyc_Absyn_KnownTunionfield){ _LL1729: _temp1728=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1724)->f1; goto _LL1727; _LL1727: _temp1726=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1724)->f2; goto _LL1723;} else{ goto _LL1680;} _LL1723: _temp1722=
_temp1720.targs; goto _LL1709;} else{ goto _LL1680;} _LL1709: _temp1708=
_temp1672.f2; if(( unsigned int) _temp1708 >  4u?*(( int*) _temp1708) ==  Cyc_Absyn_TunionType:
0){ _LL1711: _temp1710=(( struct Cyc_Absyn_TunionType_struct*) _temp1708)->f1;
_LL1715: _temp1714=( void*) _temp1710.tunion_info; if(*(( int*) _temp1714) == 
Cyc_Absyn_KnownTunion){ _LL1717: _temp1716=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1714)->f1; goto _LL1713;} else{ goto _LL1680;} _LL1713: _temp1712=
_temp1710.targs; goto _LL1679;} else{ goto _LL1680;} _LL1680: _LL1743: _temp1742=
_temp1672.f1; if(( unsigned int) _temp1742 >  4u?*(( int*) _temp1742) ==  Cyc_Absyn_PointerType:
0){ _LL1745: _temp1744=(( struct Cyc_Absyn_PointerType_struct*) _temp1742)->f1;
_LL1755: _temp1754=( void*) _temp1744.elt_typ; goto _LL1753; _LL1753: _temp1752=(
void*) _temp1744.rgn_typ; goto _LL1751; _LL1751: _temp1750= _temp1744.nullable;
goto _LL1749; _LL1749: _temp1748= _temp1744.tq; goto _LL1747; _LL1747: _temp1746=
_temp1744.bounds; goto _LL1731;} else{ goto _LL1682;} _LL1731: _temp1730=
_temp1672.f2; if(( unsigned int) _temp1730 >  4u?*(( int*) _temp1730) ==  Cyc_Absyn_TunionType:
0){ _LL1733: _temp1732=(( struct Cyc_Absyn_TunionType_struct*) _temp1730)->f1;
_LL1739: _temp1738=( void*) _temp1732.tunion_info; if(*(( int*) _temp1738) == 
Cyc_Absyn_KnownTunion){ _LL1741: _temp1740=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1738)->f1; goto _LL1737;} else{ goto _LL1682;} _LL1737: _temp1736=
_temp1732.targs; goto _LL1735; _LL1735: _temp1734=( void*) _temp1732.rgn; goto
_LL1681;} else{ goto _LL1682;} _LL1682: goto _LL1683; _LL1675: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1690.nullable, _temp1686.nullable)){
void* _temp1756=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1690.nullable))->v; int _temp1762; _LL1758:
if(( unsigned int) _temp1756 >  1u?*(( int*) _temp1756) ==  Cyc_Absyn_Eq_constr:
0){ _LL1763: _temp1762=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1756)->f1;
goto _LL1759;} else{ goto _LL1760;} _LL1760: goto _LL1761; _LL1759: okay= !
_temp1762; goto _LL1757; _LL1761:( int) _throw(({ void* _temp1764[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("silent_castable conref not eq", sizeof( unsigned char), 30u), _tag_arr(
_temp1764, sizeof( void*), 0u));})); _LL1757:;} if( ! Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1690.bounds, _temp1686.bounds)){ struct _tuple8
_temp1766=({ struct _tuple8 _temp1765; _temp1765.f1=( void*)( Cyc_Absyn_compress_conref(
_temp1690.bounds))->v; _temp1765.f2=( void*)( Cyc_Absyn_compress_conref(
_temp1686.bounds))->v; _temp1765;}); void* _temp1776; void* _temp1778; void*
_temp1780; void* _temp1782; void* _temp1784; void* _temp1786; struct Cyc_Absyn_Exp*
_temp1788; void* _temp1790; void* _temp1792; struct Cyc_Absyn_Exp* _temp1794;
void* _temp1796; void* _temp1798; void* _temp1800; void* _temp1802; _LL1768:
_LL1781: _temp1780= _temp1766.f1; if(( unsigned int) _temp1780 >  1u?*(( int*)
_temp1780) ==  Cyc_Absyn_Eq_constr: 0){ _LL1783: _temp1782=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1780)->f1; if(( unsigned int) _temp1782 >  1u?*(( int*) _temp1782) ==  Cyc_Absyn_Upper_b:
0){ goto _LL1777;} else{ goto _LL1770;}} else{ goto _LL1770;} _LL1777: _temp1776=
_temp1766.f2; if(( unsigned int) _temp1776 >  1u?*(( int*) _temp1776) ==  Cyc_Absyn_Eq_constr:
0){ _LL1779: _temp1778=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1776)->f1;
if( _temp1778 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1769;} else{ goto
_LL1770;}} else{ goto _LL1770;} _LL1770: _LL1791: _temp1790= _temp1766.f1; if((
unsigned int) _temp1790 >  1u?*(( int*) _temp1790) ==  Cyc_Absyn_Eq_constr: 0){
_LL1793: _temp1792=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1790)->f1;
if(( unsigned int) _temp1792 >  1u?*(( int*) _temp1792) ==  Cyc_Absyn_Upper_b: 0){
_LL1795: _temp1794=(( struct Cyc_Absyn_Upper_b_struct*) _temp1792)->f1; goto
_LL1785;} else{ goto _LL1772;}} else{ goto _LL1772;} _LL1785: _temp1784=
_temp1766.f2; if(( unsigned int) _temp1784 >  1u?*(( int*) _temp1784) ==  Cyc_Absyn_Eq_constr:
0){ _LL1787: _temp1786=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1784)->f1;
if(( unsigned int) _temp1786 >  1u?*(( int*) _temp1786) ==  Cyc_Absyn_Upper_b: 0){
_LL1789: _temp1788=(( struct Cyc_Absyn_Upper_b_struct*) _temp1786)->f1; goto
_LL1771;} else{ goto _LL1772;}} else{ goto _LL1772;} _LL1772: _LL1801: _temp1800=
_temp1766.f1; if(( unsigned int) _temp1800 >  1u?*(( int*) _temp1800) ==  Cyc_Absyn_Eq_constr:
0){ _LL1803: _temp1802=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1800)->f1;
if( _temp1802 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1797;} else{ goto
_LL1774;}} else{ goto _LL1774;} _LL1797: _temp1796= _temp1766.f2; if((
unsigned int) _temp1796 >  1u?*(( int*) _temp1796) ==  Cyc_Absyn_Eq_constr: 0){
_LL1799: _temp1798=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1796)->f1;
if( _temp1798 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1773;} else{ goto
_LL1774;}} else{ goto _LL1774;} _LL1774: goto _LL1775; _LL1769: okay= 1; goto
_LL1767; _LL1771: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1794) >=  Cyc_Evexp_eval_const_uint_exp(
_temp1788): 0;({ void* _temp1804[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1804, sizeof( void*), 0u));}); goto
_LL1767; _LL1773: okay= 1; goto _LL1767; _LL1775: okay= 0; goto _LL1767; _LL1767:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1690.elt_typ,( void*) _temp1686.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1690.rgn_typ,( void*) _temp1686.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1690.rgn_typ,( void*) _temp1686.rgn_typ):
0; okay= okay? !( _temp1690.tq).q_const? 1:( _temp1686.tq).q_const: 0; return
okay;} _LL1677: { int okay; okay=( _temp1702 !=  0? _temp1694 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1702)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1694)): 0; return( okay? Cyc_Tcutil_unify(
_temp1706, _temp1698): 0)? ! _temp1704.q_const? 1: _temp1696.q_const: 0;}
_LL1679: if(( _temp1728 ==  _temp1716? 1: Cyc_Absyn_qvar_cmp( _temp1728->name,
_temp1716->name) ==  0)? _temp1726->typs ==  0: 0){ for( 0; _temp1722 !=  0?
_temp1712 !=  0: 0;( _temp1722=(( struct Cyc_List_List*) _check_null( _temp1722))->tl,
_temp1712=(( struct Cyc_List_List*) _check_null( _temp1712))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1722))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1712))->hd)){ break;}} if( _temp1722 ==  0?
_temp1712 ==  0: 0){ return 1;}} return 0; _LL1681:{ void* _temp1805= Cyc_Tcutil_compress(
_temp1754); struct Cyc_Absyn_TunionFieldInfo _temp1811; struct Cyc_List_List*
_temp1813; void* _temp1815; struct Cyc_Absyn_Tunionfield* _temp1817; struct Cyc_Absyn_Tuniondecl*
_temp1819; _LL1807: if(( unsigned int) _temp1805 >  4u?*(( int*) _temp1805) == 
Cyc_Absyn_TunionFieldType: 0){ _LL1812: _temp1811=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1805)->f1; _LL1816: _temp1815=( void*) _temp1811.field_info; if(*(( int*)
_temp1815) ==  Cyc_Absyn_KnownTunionfield){ _LL1820: _temp1819=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1815)->f1; goto _LL1818; _LL1818: _temp1817=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1815)->f2; goto _LL1814;} else{ goto _LL1809;} _LL1814: _temp1813=
_temp1811.targs; goto _LL1808;} else{ goto _LL1809;} _LL1809: goto _LL1810;
_LL1808: if( ! Cyc_Tcutil_unify( _temp1752, _temp1734)? ! Cyc_Tcenv_region_outlives(
te, _temp1752, _temp1734): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1750,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1746,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1740->name, _temp1819->name) ==  0? _temp1817->typs !=  0: 0){ int okay= 1;
for( 0; _temp1813 !=  0? _temp1736 !=  0: 0;( _temp1813=(( struct Cyc_List_List*)
_check_null( _temp1813))->tl, _temp1736=(( struct Cyc_List_List*) _check_null(
_temp1736))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1813))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1736))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1813 !=  0)? 1:
_temp1736 !=  0){ return 0;} return 1;} goto _LL1806; _LL1810: goto _LL1806;
_LL1806:;} return 0; _LL1683: return Cyc_Tcutil_unify( t1, t2); _LL1673:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1821[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1821, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Std_String_pa_struct
_temp1824; _temp1824.tag= Cyc_Std_String_pa; _temp1824.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct _temp1823;
_temp1823.tag= Cyc_Std_String_pa; _temp1823.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1822[ 2u]={& _temp1823,& _temp1824}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1822, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1825= old_typ; void* _temp1837;
void* _temp1839; _LL1827: if(( unsigned int) _temp1825 >  4u?*(( int*) _temp1825)
==  Cyc_Absyn_EnumType: 0){ goto _LL1828;} else{ goto _LL1829;} _LL1829: if((
unsigned int) _temp1825 >  4u?*(( int*) _temp1825) ==  Cyc_Absyn_IntType: 0){
_LL1840: _temp1839=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1825)->f1;
goto _LL1838; _LL1838: _temp1837=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1825)->f2; goto _LL1830;} else{ goto _LL1831;} _LL1831: if( _temp1825 == (
void*) Cyc_Absyn_FloatType){ goto _LL1832;} else{ goto _LL1833;} _LL1833: if(
_temp1825 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1834;} else{ goto _LL1835;}
_LL1835: goto _LL1836; _LL1828: t= Cyc_Absyn_sint_t; goto _LL1826; _LL1830:{
void* _temp1841= _temp1837; _LL1843: if( _temp1841 == ( void*) Cyc_Absyn_B1){
goto _LL1844;} else{ goto _LL1845;} _LL1845: if( _temp1841 == ( void*) Cyc_Absyn_B2){
goto _LL1846;} else{ goto _LL1847;} _LL1847: if( _temp1841 == ( void*) Cyc_Absyn_B4){
goto _LL1848;} else{ goto _LL1849;} _LL1849: if( _temp1841 == ( void*) Cyc_Absyn_B8){
goto _LL1850;} else{ goto _LL1842;} _LL1844: t= Cyc_Absyn_sint_t; goto _LL1842;
_LL1846: t= Cyc_Absyn_sint_t; goto _LL1842; _LL1848: t= _temp1839 == ( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1842; _LL1850: t=
_temp1839 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t;
goto _LL1842; _LL1842:;} goto _LL1826; _LL1832: t= Cyc_Absyn_float_t; goto
_LL1826; _LL1834: t= Cyc_Absyn_double_t; goto _LL1826; _LL1836: return 0;
_LL1826:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;} int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{
if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e,
t2);({ struct Cyc_Std_String_pa_struct _temp1853; _temp1853.tag= Cyc_Std_String_pa;
_temp1853.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp1852; _temp1852.tag= Cyc_Std_String_pa; _temp1852.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp1851[ 2u]={& _temp1852,& _temp1853};
Cyc_Tcutil_warn( e->loc, _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u), _tag_arr( _temp1851, sizeof( void*), 2u));}}}); return 1;}
else{ return 0;}}}} int Cyc_Tcutil_coerceable( void* t){ void* _temp1854= Cyc_Tcutil_compress(
t); _LL1856: if(( unsigned int) _temp1854 >  4u?*(( int*) _temp1854) ==  Cyc_Absyn_IntType:
0){ goto _LL1857;} else{ goto _LL1858;} _LL1858: if( _temp1854 == ( void*) Cyc_Absyn_FloatType){
goto _LL1859;} else{ goto _LL1860;} _LL1860: if( _temp1854 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1861;} else{ goto _LL1862;} _LL1862: goto _LL1863; _LL1857: return 1;
_LL1859: return 1; _LL1861: return 1; _LL1863: return 0; _LL1855:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1864=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp1864->f1= x->tq; _temp1864->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1864;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1865= t1; struct Cyc_List_List* _temp1877; struct Cyc_Absyn_Structdecl**
_temp1879; struct Cyc_List_List* _temp1881; struct _tuple1* _temp1883; struct
Cyc_List_List* _temp1885; _LL1867: if( _temp1865 == ( void*) Cyc_Absyn_VoidType){
goto _LL1868;} else{ goto _LL1869;} _LL1869: if(( unsigned int) _temp1865 >  4u?*((
int*) _temp1865) ==  Cyc_Absyn_TupleType: 0){ _LL1878: _temp1877=(( struct Cyc_Absyn_TupleType_struct*)
_temp1865)->f1; goto _LL1870;} else{ goto _LL1871;} _LL1871: if(( unsigned int)
_temp1865 >  4u?*(( int*) _temp1865) ==  Cyc_Absyn_StructType: 0){ _LL1884:
_temp1883=(( struct Cyc_Absyn_StructType_struct*) _temp1865)->f1; goto _LL1882;
_LL1882: _temp1881=(( struct Cyc_Absyn_StructType_struct*) _temp1865)->f2; goto
_LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_StructType_struct*) _temp1865)->f3;
goto _LL1872;} else{ goto _LL1873;} _LL1873: if(( unsigned int) _temp1865 >  4u?*((
int*) _temp1865) ==  Cyc_Absyn_AnonStructType: 0){ _LL1886: _temp1885=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1865)->f1; goto _LL1874;} else{ goto
_LL1875;} _LL1875: goto _LL1876; _LL1868: return 0; _LL1870: return _temp1877;
_LL1872: if( _temp1879 ==  0? 1: _temp1883 ==  0){ return({ struct Cyc_List_List*
_temp1887=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1887->hd=( void*)({ struct _tuple4* _temp1888=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1888->f1= Cyc_Absyn_empty_tqual(); _temp1888->f2=
t1; _temp1888;}); _temp1887->tl= 0; _temp1887;});}{ struct Cyc_Absyn_Structdecl*
_temp1889=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1879)); struct
_tuple1 _temp1890=*(( struct _tuple1*) _check_null( _temp1883)); struct Cyc_List_List*
_temp1891=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1889->tvs, _temp1881); if( _temp1889->fields ==  0){
return({ struct Cyc_List_List* _temp1892=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1892->hd=( void*)({ struct _tuple4*
_temp1893=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1893->f1=
Cyc_Absyn_empty_tqual(); _temp1893->f2= t1; _temp1893;}); _temp1892->tl= 0;
_temp1892;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1891,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1889->fields))->v);}
_LL1874: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1885); _LL1876: return({
struct Cyc_List_List* _temp1894=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1894->hd=( void*)({ struct _tuple4* _temp1895=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1895->f1= Cyc_Absyn_empty_tqual();
_temp1895->f2= t1; _temp1895;}); _temp1894->tl= 0; _temp1894;}); _LL1866:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1897=({ struct _tuple8 _temp1896; _temp1896.f1= t1; _temp1896.f2=
t2; _temp1896;}); void* _temp1903; struct Cyc_Absyn_PtrInfo _temp1905; struct
Cyc_Absyn_Conref* _temp1907; struct Cyc_Absyn_Tqual _temp1909; struct Cyc_Absyn_Conref*
_temp1911; void* _temp1913; void* _temp1915; void* _temp1917; struct Cyc_Absyn_PtrInfo
_temp1919; struct Cyc_Absyn_Conref* _temp1921; struct Cyc_Absyn_Tqual _temp1923;
struct Cyc_Absyn_Conref* _temp1925; void* _temp1927; void* _temp1929; _LL1899:
_LL1918: _temp1917= _temp1897.f1; if(( unsigned int) _temp1917 >  4u?*(( int*)
_temp1917) ==  Cyc_Absyn_PointerType: 0){ _LL1920: _temp1919=(( struct Cyc_Absyn_PointerType_struct*)
_temp1917)->f1; _LL1930: _temp1929=( void*) _temp1919.elt_typ; goto _LL1928;
_LL1928: _temp1927=( void*) _temp1919.rgn_typ; goto _LL1926; _LL1926: _temp1925=
_temp1919.nullable; goto _LL1924; _LL1924: _temp1923= _temp1919.tq; goto _LL1922;
_LL1922: _temp1921= _temp1919.bounds; goto _LL1904;} else{ goto _LL1901;}
_LL1904: _temp1903= _temp1897.f2; if(( unsigned int) _temp1903 >  4u?*(( int*)
_temp1903) ==  Cyc_Absyn_PointerType: 0){ _LL1906: _temp1905=(( struct Cyc_Absyn_PointerType_struct*)
_temp1903)->f1; _LL1916: _temp1915=( void*) _temp1905.elt_typ; goto _LL1914;
_LL1914: _temp1913=( void*) _temp1905.rgn_typ; goto _LL1912; _LL1912: _temp1911=
_temp1905.nullable; goto _LL1910; _LL1910: _temp1909= _temp1905.tq; goto _LL1908;
_LL1908: _temp1907= _temp1905.bounds; goto _LL1900;} else{ goto _LL1901;}
_LL1901: goto _LL1902; _LL1900: if( _temp1923.q_const? ! _temp1909.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1925, _temp1911)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1925): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1911): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1927, _temp1913)? ! Cyc_Tcenv_region_outlives(
te, _temp1927, _temp1913): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1921, _temp1907)){ struct _tuple8 _temp1932=({ struct _tuple8 _temp1931;
_temp1931.f1= Cyc_Absyn_conref_val( _temp1921); _temp1931.f2= Cyc_Absyn_conref_val(
_temp1907); _temp1931;}); void* _temp1940; void* _temp1942; void* _temp1944;
struct Cyc_Absyn_Exp* _temp1946; void* _temp1948; struct Cyc_Absyn_Exp*
_temp1950; _LL1934: _LL1943: _temp1942= _temp1932.f1; if(( unsigned int)
_temp1942 >  1u?*(( int*) _temp1942) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1941;}
else{ goto _LL1936;} _LL1941: _temp1940= _temp1932.f2; if( _temp1940 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1935;} else{ goto _LL1936;} _LL1936: _LL1949:
_temp1948= _temp1932.f1; if(( unsigned int) _temp1948 >  1u?*(( int*) _temp1948)
==  Cyc_Absyn_Upper_b: 0){ _LL1951: _temp1950=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1948)->f1; goto _LL1945;} else{ goto _LL1938;} _LL1945: _temp1944=
_temp1932.f2; if(( unsigned int) _temp1944 >  1u?*(( int*) _temp1944) ==  Cyc_Absyn_Upper_b:
0){ _LL1947: _temp1946=(( struct Cyc_Absyn_Upper_b_struct*) _temp1944)->f1; goto
_LL1937;} else{ goto _LL1938;} _LL1938: goto _LL1939; _LL1935: goto _LL1933;
_LL1937: if( Cyc_Evexp_eval_const_uint_exp( _temp1950) <  Cyc_Evexp_eval_const_uint_exp(
_temp1946)){ return 0;} goto _LL1933; _LL1939: return 0; _LL1933:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1952=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1952->hd=( void*)({ struct _tuple8*
_temp1953=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1953->f1=
t1; _temp1953->f2= t2; _temp1953;}); _temp1952->tl= assume; _temp1952;}),
_temp1929, _temp1915); _LL1902: return 0; _LL1898:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1955=({ struct _tuple8 _temp1954;
_temp1954.f1= Cyc_Tcutil_compress( t1); _temp1954.f2= Cyc_Tcutil_compress( t2);
_temp1954;}); void* _temp1961; void* _temp1963; void* _temp1965; void* _temp1967;
_LL1957: _LL1966: _temp1965= _temp1955.f1; if(( unsigned int) _temp1965 >  4u?*((
int*) _temp1965) ==  Cyc_Absyn_IntType: 0){ _LL1968: _temp1967=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1965)->f2; goto _LL1962;} else{ goto _LL1959;}
_LL1962: _temp1961= _temp1955.f2; if(( unsigned int) _temp1961 >  4u?*(( int*)
_temp1961) ==  Cyc_Absyn_IntType: 0){ _LL1964: _temp1963=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1961)->f2; goto _LL1958;} else{ goto _LL1959;} _LL1959: goto _LL1960;
_LL1958: return _temp1967 ==  _temp1963; _LL1960: return 0; _LL1956:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=((
struct Cyc_List_List*) _check_null( tqs1))->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1971; void* _temp1972; struct Cyc_Absyn_Tqual _temp1974;
struct _tuple4* _temp1969=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs1))->hd; _temp1971=* _temp1969; _LL1975: _temp1974= _temp1971.f1;
goto _LL1973; _LL1973: _temp1972= _temp1971.f2; goto _LL1970; _LL1970: { struct
_tuple4 _temp1978; void* _temp1979; struct Cyc_Absyn_Tqual _temp1981; struct
_tuple4* _temp1976=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs2))->hd;
_temp1978=* _temp1976; _LL1982: _temp1981= _temp1978.f1; goto _LL1980; _LL1980:
_temp1979= _temp1978.f2; goto _LL1977; _LL1977: if( _temp1981.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1972, _temp1979): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1972, _temp1979)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1972,
_temp1979)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1983= t2; struct Cyc_Absyn_Uniondecl** _temp1991; struct Cyc_List_List*
_temp1993; void* _temp1995; _LL1985: if(( unsigned int) _temp1983 >  4u?*(( int*)
_temp1983) ==  Cyc_Absyn_UnionType: 0){ _LL1994: _temp1993=(( struct Cyc_Absyn_UnionType_struct*)
_temp1983)->f2; goto _LL1992; _LL1992: _temp1991=(( struct Cyc_Absyn_UnionType_struct*)
_temp1983)->f3; goto _LL1986;} else{ goto _LL1987;} _LL1987: if(( unsigned int)
_temp1983 >  4u?*(( int*) _temp1983) ==  Cyc_Absyn_IntType: 0){ _LL1996:
_temp1995=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1983)->f2; if(
_temp1995 == ( void*) Cyc_Absyn_B4){ goto _LL1988;} else{ goto _LL1989;}} else{
goto _LL1989;} _LL1989: goto _LL1990; _LL1986: if( _temp1991 ==  0){ goto
_LL1984;}{ struct Cyc_Absyn_Uniondecl* _temp1997=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1991)); if( _temp1997->fields ==  0){ goto _LL1984;}{ struct
Cyc_List_List* _temp1998=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1997->tvs, _temp1993);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1997->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp1998,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1984;}} _LL1988: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1984; _LL1990: goto _LL1984; _LL1984:;}{ void* _temp1999= t1;
struct Cyc_Absyn_PtrInfo _temp2017; struct Cyc_Absyn_Conref* _temp2019; struct
Cyc_Absyn_Tqual _temp2021; struct Cyc_Absyn_Conref* _temp2023; void* _temp2025;
void* _temp2027; struct Cyc_Absyn_Exp* _temp2029; struct Cyc_Absyn_Tqual
_temp2031; void* _temp2033; struct Cyc_Absyn_Enumdecl* _temp2035; _LL2001: if((
unsigned int) _temp1999 >  4u?*(( int*) _temp1999) ==  Cyc_Absyn_PointerType: 0){
_LL2018: _temp2017=(( struct Cyc_Absyn_PointerType_struct*) _temp1999)->f1;
_LL2028: _temp2027=( void*) _temp2017.elt_typ; goto _LL2026; _LL2026: _temp2025=(
void*) _temp2017.rgn_typ; goto _LL2024; _LL2024: _temp2023= _temp2017.nullable;
goto _LL2022; _LL2022: _temp2021= _temp2017.tq; goto _LL2020; _LL2020: _temp2019=
_temp2017.bounds; goto _LL2002;} else{ goto _LL2003;} _LL2003: if(( unsigned int)
_temp1999 >  4u?*(( int*) _temp1999) ==  Cyc_Absyn_ArrayType: 0){ _LL2034:
_temp2033=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1999)->f1; goto
_LL2032; _LL2032: _temp2031=(( struct Cyc_Absyn_ArrayType_struct*) _temp1999)->f2;
goto _LL2030; _LL2030: _temp2029=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1999)->f3; goto _LL2004;} else{ goto _LL2005;} _LL2005: if(( unsigned int)
_temp1999 >  4u?*(( int*) _temp1999) ==  Cyc_Absyn_EnumType: 0){ _LL2036:
_temp2035=(( struct Cyc_Absyn_EnumType_struct*) _temp1999)->f2; goto _LL2006;}
else{ goto _LL2007;} _LL2007: if(( unsigned int) _temp1999 >  4u?*(( int*)
_temp1999) ==  Cyc_Absyn_IntType: 0){ goto _LL2008;} else{ goto _LL2009;}
_LL2009: if( _temp1999 == ( void*) Cyc_Absyn_FloatType){ goto _LL2010;} else{
goto _LL2011;} _LL2011: if( _temp1999 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2012;} else{ goto _LL2013;} _LL2013: if( _temp1999 == ( void*) Cyc_Absyn_VoidType){
goto _LL2014;} else{ goto _LL2015;} _LL2015: goto _LL2016; _LL2002:{ void*
_temp2037= t2; struct Cyc_Absyn_PtrInfo _temp2045; struct Cyc_Absyn_Conref*
_temp2047; struct Cyc_Absyn_Tqual _temp2049; struct Cyc_Absyn_Conref* _temp2051;
void* _temp2053; void* _temp2055; _LL2039: if(( unsigned int) _temp2037 >  4u?*((
int*) _temp2037) ==  Cyc_Absyn_PointerType: 0){ _LL2046: _temp2045=(( struct Cyc_Absyn_PointerType_struct*)
_temp2037)->f1; _LL2056: _temp2055=( void*) _temp2045.elt_typ; goto _LL2054;
_LL2054: _temp2053=( void*) _temp2045.rgn_typ; goto _LL2052; _LL2052: _temp2051=
_temp2045.nullable; goto _LL2050; _LL2050: _temp2049= _temp2045.tq; goto _LL2048;
_LL2048: _temp2047= _temp2045.bounds; goto _LL2040;} else{ goto _LL2041;}
_LL2041: if( _temp2037 == ( void*) Cyc_Absyn_VoidType){ goto _LL2042;} else{
goto _LL2043;} _LL2043: goto _LL2044; _LL2040: { struct Cyc_List_List* _temp2057=({
struct Cyc_List_List* _temp2074=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2074->hd=( void*)({ struct _tuple8* _temp2075=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2075->f1= t1;
_temp2075->f2= t2; _temp2075;}); _temp2074->tl= 0; _temp2074;}); int _temp2058=
Cyc_Tcutil_ptrsubtype( te, _temp2057, _temp2027, _temp2055)? ! _temp2021.q_const?
1: _temp2049.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2019, _temp2047); if( ! bounds_ok){ struct _tuple8 _temp2060=({ struct
_tuple8 _temp2059; _temp2059.f1= Cyc_Absyn_conref_val( _temp2019); _temp2059.f2=
Cyc_Absyn_conref_val( _temp2047); _temp2059;}); void* _temp2066; struct Cyc_Absyn_Exp*
_temp2068; void* _temp2070; struct Cyc_Absyn_Exp* _temp2072; _LL2062: _LL2071:
_temp2070= _temp2060.f1; if(( unsigned int) _temp2070 >  1u?*(( int*) _temp2070)
==  Cyc_Absyn_Upper_b: 0){ _LL2073: _temp2072=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2070)->f1; goto _LL2067;} else{ goto _LL2064;} _LL2067: _temp2066=
_temp2060.f2; if(( unsigned int) _temp2066 >  1u?*(( int*) _temp2066) ==  Cyc_Absyn_Upper_b:
0){ _LL2069: _temp2068=(( struct Cyc_Absyn_Upper_b_struct*) _temp2066)->f1; goto
_LL2063;} else{ goto _LL2064;} _LL2064: goto _LL2065; _LL2063: if( Cyc_Evexp_eval_const_uint_exp(
_temp2072) >=  Cyc_Evexp_eval_const_uint_exp( _temp2068)){ bounds_ok= 1;} goto
_LL2061; _LL2065: bounds_ok= 1; goto _LL2061; _LL2061:;} return( bounds_ok?
_temp2058: 0)? Cyc_Tcutil_unify( _temp2025, _temp2053)? 1: Cyc_Tcenv_region_outlives(
te, _temp2025, _temp2053): 0;} _LL2042: return 1; _LL2044: goto _LL2038; _LL2038:;}
return 0; _LL2004:{ void* _temp2076= t2; struct Cyc_Absyn_Exp* _temp2082; struct
Cyc_Absyn_Tqual _temp2084; void* _temp2086; _LL2078: if(( unsigned int)
_temp2076 >  4u?*(( int*) _temp2076) ==  Cyc_Absyn_ArrayType: 0){ _LL2087:
_temp2086=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2076)->f1; goto
_LL2085; _LL2085: _temp2084=(( struct Cyc_Absyn_ArrayType_struct*) _temp2076)->f2;
goto _LL2083; _LL2083: _temp2082=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2076)->f3; goto _LL2079;} else{ goto _LL2080;} _LL2080: goto _LL2081;
_LL2079: { int okay; okay=( _temp2029 !=  0? _temp2082 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2029)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2082)): 0; return( okay? Cyc_Tcutil_unify(
_temp2033, _temp2086): 0)? ! _temp2031.q_const? 1: _temp2084.q_const: 0;}
_LL2081: return 0; _LL2077:;} return 0; _LL2006:{ void* _temp2088= t2; struct
Cyc_Absyn_Enumdecl* _temp2094; _LL2090: if(( unsigned int) _temp2088 >  4u?*((
int*) _temp2088) ==  Cyc_Absyn_EnumType: 0){ _LL2095: _temp2094=(( struct Cyc_Absyn_EnumType_struct*)
_temp2088)->f2; goto _LL2091;} else{ goto _LL2092;} _LL2092: goto _LL2093;
_LL2091: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2035))->fields != 
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2094))->fields !=  0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp2035))->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2094))->fields))->v): 0){ return 1;} goto _LL2089; _LL2093:
goto _LL2089; _LL2089:;} goto _LL2008; _LL2008: goto _LL2010; _LL2010: goto
_LL2012; _LL2012: return Cyc_Tcutil_coerceable( t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType;
_LL2014: return t2 == ( void*) Cyc_Absyn_VoidType; _LL2016: return 0; _LL2000:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2096= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2097=(
struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2097[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2098; _temp2098.tag= Cyc_Absyn_Cast_e;
_temp2098.f1=( void*) t; _temp2098.f2= _temp2096; _temp2098;}); _temp2097;})));
e->topt=({ struct Cyc_Core_Opt* _temp2099=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2099->v=( void*) t; _temp2099;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2100= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2102: if(( unsigned int) _temp2100
>  4u?*(( int*) _temp2100) ==  Cyc_Absyn_EnumType: 0){ goto _LL2103;} else{ goto
_LL2104;} _LL2104: if(( unsigned int) _temp2100 >  4u?*(( int*) _temp2100) == 
Cyc_Absyn_IntType: 0){ goto _LL2105;} else{ goto _LL2106;} _LL2106: if((
unsigned int) _temp2100 >  4u?*(( int*) _temp2100) ==  Cyc_Absyn_Evar: 0){ goto
_LL2107;} else{ goto _LL2108;} _LL2108: goto _LL2109; _LL2103: goto _LL2105;
_LL2105: return 1; _LL2107: if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} return 0; _LL2109:
return 0; _LL2101:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if(
Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp2110= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2112: if(
_temp2110 == ( void*) Cyc_Absyn_FloatType){ goto _LL2113;} else{ goto _LL2114;}
_LL2114: if( _temp2110 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2115;} else{
goto _LL2116;} _LL2116: goto _LL2117; _LL2113: goto _LL2115; _LL2115: return 1;
_LL2117: return 0; _LL2111:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp2118= Cyc_Tcutil_compress( t); _LL2120: if(( unsigned int) _temp2118 >  4u?*((
int*) _temp2118) ==  Cyc_Absyn_FnType: 0){ goto _LL2121;} else{ goto _LL2122;}
_LL2122: goto _LL2123; _LL2121: return 1; _LL2123: return 0; _LL2119:;} void*
Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct _tuple8 _temp2125=({
struct _tuple8 _temp2124; _temp2124.f1= t1; _temp2124.f2= t2; _temp2124;}); void*
_temp2149; void* _temp2151; void* _temp2153; void* _temp2155; void* _temp2157;
void* _temp2159; void* _temp2161; void* _temp2163; void* _temp2165; void*
_temp2167; void* _temp2169; void* _temp2171; void* _temp2173; void* _temp2175;
void* _temp2177; void* _temp2179; void* _temp2181; void* _temp2183; void*
_temp2185; void* _temp2187; void* _temp2189; void* _temp2191; _LL2127: _LL2150:
_temp2149= _temp2125.f1; if( _temp2149 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2128;} else{ goto _LL2129;} _LL2129: _LL2152: _temp2151= _temp2125.f2; if(
_temp2151 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2130;} else{ goto _LL2131;}
_LL2131: _LL2154: _temp2153= _temp2125.f1; if( _temp2153 == ( void*) Cyc_Absyn_FloatType){
goto _LL2132;} else{ goto _LL2133;} _LL2133: _LL2156: _temp2155= _temp2125.f2;
if( _temp2155 == ( void*) Cyc_Absyn_FloatType){ goto _LL2134;} else{ goto
_LL2135;} _LL2135: _LL2158: _temp2157= _temp2125.f1; if(( unsigned int)
_temp2157 >  4u?*(( int*) _temp2157) ==  Cyc_Absyn_IntType: 0){ _LL2162:
_temp2161=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2157)->f1; if(
_temp2161 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2160;} else{ goto _LL2137;}
_LL2160: _temp2159=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2157)->f2;
if( _temp2159 == ( void*) Cyc_Absyn_B8){ goto _LL2136;} else{ goto _LL2137;}}
else{ goto _LL2137;} _LL2137: _LL2164: _temp2163= _temp2125.f2; if((
unsigned int) _temp2163 >  4u?*(( int*) _temp2163) ==  Cyc_Absyn_IntType: 0){
_LL2168: _temp2167=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2163)->f1;
if( _temp2167 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2166;} else{ goto _LL2139;}
_LL2166: _temp2165=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2163)->f2;
if( _temp2165 == ( void*) Cyc_Absyn_B8){ goto _LL2138;} else{ goto _LL2139;}}
else{ goto _LL2139;} _LL2139: _LL2170: _temp2169= _temp2125.f1; if((
unsigned int) _temp2169 >  4u?*(( int*) _temp2169) ==  Cyc_Absyn_IntType: 0){
_LL2174: _temp2173=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2169)->f1;
if( _temp2173 == ( void*) Cyc_Absyn_Signed){ goto _LL2172;} else{ goto _LL2141;}
_LL2172: _temp2171=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2169)->f2;
if( _temp2171 == ( void*) Cyc_Absyn_B8){ goto _LL2140;} else{ goto _LL2141;}}
else{ goto _LL2141;} _LL2141: _LL2176: _temp2175= _temp2125.f2; if((
unsigned int) _temp2175 >  4u?*(( int*) _temp2175) ==  Cyc_Absyn_IntType: 0){
_LL2180: _temp2179=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2175)->f1;
if( _temp2179 == ( void*) Cyc_Absyn_Signed){ goto _LL2178;} else{ goto _LL2143;}
_LL2178: _temp2177=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2175)->f2;
if( _temp2177 == ( void*) Cyc_Absyn_B8){ goto _LL2142;} else{ goto _LL2143;}}
else{ goto _LL2143;} _LL2143: _LL2182: _temp2181= _temp2125.f1; if((
unsigned int) _temp2181 >  4u?*(( int*) _temp2181) ==  Cyc_Absyn_IntType: 0){
_LL2186: _temp2185=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2181)->f1;
if( _temp2185 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2184;} else{ goto _LL2145;}
_LL2184: _temp2183=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2181)->f2;
if( _temp2183 == ( void*) Cyc_Absyn_B4){ goto _LL2144;} else{ goto _LL2145;}}
else{ goto _LL2145;} _LL2145: _LL2188: _temp2187= _temp2125.f2; if((
unsigned int) _temp2187 >  4u?*(( int*) _temp2187) ==  Cyc_Absyn_IntType: 0){
_LL2192: _temp2191=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2187)->f1;
if( _temp2191 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2190;} else{ goto _LL2147;}
_LL2190: _temp2189=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2187)->f2;
if( _temp2189 == ( void*) Cyc_Absyn_B4){ goto _LL2146;} else{ goto _LL2147;}}
else{ goto _LL2147;} _LL2147: goto _LL2148; _LL2128: goto _LL2130; _LL2130:
return( void*) Cyc_Absyn_DoubleType; _LL2132: goto _LL2134; _LL2134: return(
void*) Cyc_Absyn_FloatType; _LL2136: goto _LL2138; _LL2138: return Cyc_Absyn_ulonglong_t;
_LL2140: goto _LL2142; _LL2142: return Cyc_Absyn_slonglong_t; _LL2144: goto
_LL2146; _LL2146: return Cyc_Absyn_uint_t; _LL2148: return Cyc_Absyn_sint_t;
_LL2126:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2193=( void*) e->r; struct Cyc_Core_Opt* _temp2199; _LL2195: if(*(( int*)
_temp2193) ==  Cyc_Absyn_AssignOp_e){ _LL2200: _temp2199=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2193)->f2; if( _temp2199 ==  0){ goto _LL2196;} else{ goto _LL2197;}} else{
goto _LL2197;} _LL2197: goto _LL2198; _LL2196:({ void* _temp2201[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2201, sizeof( void*), 0u));}); goto _LL2194; _LL2198: goto _LL2194; _LL2194:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2202=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2202[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2203; _temp2203.tag=
Cyc_Absyn_Forward_constr; _temp2203.f1= c2; _temp2203;}); _temp2202;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2204=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2204[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2205; _temp2205.tag= Cyc_Absyn_Forward_constr;
_temp2205.f1= c1; _temp2205;}); _temp2204;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2206=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2206[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2207; _temp2207.tag= Cyc_Absyn_Forward_constr; _temp2207.f1= c1; _temp2207;});
_temp2206;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2208=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2208[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2209; _temp2209.tag= Cyc_Absyn_Forward_constr;
_temp2209.f1= c2; _temp2209;}); _temp2208;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2210=( int*) _cycalloc_atomic( sizeof( int)); _temp2210[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2210;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2214;
_temp2214.tag= Cyc_Std_Int_pa; _temp2214.f1=( int)(( unsigned int) i);{ void*
_temp2213[ 1u]={& _temp2214}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2213, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2211=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2211->name=({ struct _tagged_arr* _temp2212=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2212[ 0]=
s; _temp2212;}); _temp2211->identity= 0; _temp2211->kind= Cyc_Absyn_new_conref(
k); _temp2211;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2215=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2215, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_arr _temp2216= Cyc_Std_strconcat( _tag_arr("`",
sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2216, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2217=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2217[ 0]=( struct _tagged_arr) _temp2216; _temp2217;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2218=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2218->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2219=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2219->v=( void*)(*
x).f1; _temp2219;}); _temp2218->f2=(* x).f2; _temp2218->f3=(* x).f3; _temp2218;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2220= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2220=({ struct Cyc_List_List* _temp2221=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2221->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2221->tl= _temp2220; _temp2221;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2222=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2222[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2223; _temp2223.tag= Cyc_Absyn_FnType; _temp2223.f1=({ struct Cyc_Absyn_FnInfo
_temp2224; _temp2224.tvars= fd->tvs; _temp2224.effect= fd->effect; _temp2224.ret_typ=(
void*)(( void*) fd->ret_type); _temp2224.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2224.c_varargs= fd->c_varargs;
_temp2224.cyc_varargs= fd->cyc_varargs; _temp2224.rgn_po= fd->rgn_po; _temp2224.attributes=
_temp2220; _temp2224;}); _temp2223;}); _temp2222;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2225=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp2225->f1=(* pr).f1; _temp2225->f2= t; _temp2225;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2226=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2226->f1=({ struct _tuple15* _temp2227=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2227->f1=(* y).f1; _temp2227->f2=(* y).f2;
_temp2227;}); _temp2226->f2=(* y).f3; _temp2226;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2230; struct _tuple15* _temp2232; struct
_tuple16 _temp2228=* w; _LL2233: _temp2232= _temp2228.f1; goto _LL2231; _LL2231:
_temp2230= _temp2228.f2; goto _LL2229; _LL2229: { struct Cyc_Absyn_Tqual
_temp2236; struct Cyc_Core_Opt* _temp2238; struct _tuple15 _temp2234=* _temp2232;
_LL2239: _temp2238= _temp2234.f1; goto _LL2237; _LL2237: _temp2236= _temp2234.f2;
goto _LL2235; _LL2235: return({ struct _tuple2* _temp2240=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2240->f1= _temp2238; _temp2240->f2=
_temp2236; _temp2240->f3= _temp2230; _temp2240;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2241=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2241->name= f->name;
_temp2241->tq= f->tq; _temp2241->type=( void*) t; _temp2241->width= f->width;
_temp2241->attributes= f->attributes; _temp2241;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2242= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2290; struct Cyc_Absyn_Structdecl** _temp2292;
struct Cyc_List_List* _temp2294; struct _tuple1* _temp2296; struct Cyc_Absyn_Uniondecl**
_temp2298; struct Cyc_List_List* _temp2300; struct _tuple1* _temp2302; struct
Cyc_Absyn_TunionInfo _temp2304; void* _temp2306; struct Cyc_List_List* _temp2308;
void* _temp2310; struct Cyc_Absyn_TunionFieldInfo _temp2312; struct Cyc_List_List*
_temp2314; void* _temp2316; struct Cyc_Core_Opt* _temp2318; struct Cyc_List_List*
_temp2320; struct _tuple1* _temp2322; struct Cyc_Absyn_Exp* _temp2324; struct
Cyc_Absyn_Tqual _temp2326; void* _temp2328; struct Cyc_Absyn_PtrInfo _temp2330;
struct Cyc_Absyn_Conref* _temp2332; struct Cyc_Absyn_Tqual _temp2334; struct Cyc_Absyn_Conref*
_temp2336; void* _temp2338; void* _temp2340; struct Cyc_Absyn_FnInfo _temp2342;
struct Cyc_List_List* _temp2344; struct Cyc_List_List* _temp2346; struct Cyc_Absyn_VarargInfo*
_temp2348; int _temp2350; struct Cyc_List_List* _temp2352; void* _temp2354;
struct Cyc_Core_Opt* _temp2356; struct Cyc_List_List* _temp2358; struct Cyc_List_List*
_temp2360; struct Cyc_List_List* _temp2362; struct Cyc_List_List* _temp2364;
struct Cyc_Core_Opt* _temp2366; void* _temp2368; void* _temp2370; void*
_temp2372; struct Cyc_List_List* _temp2374; _LL2244: if(( unsigned int)
_temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_VarType: 0){ _LL2291:
_temp2290=(( struct Cyc_Absyn_VarType_struct*) _temp2242)->f1; goto _LL2245;}
else{ goto _LL2246;} _LL2246: if(( unsigned int) _temp2242 >  4u?*(( int*)
_temp2242) ==  Cyc_Absyn_StructType: 0){ _LL2297: _temp2296=(( struct Cyc_Absyn_StructType_struct*)
_temp2242)->f1; goto _LL2295; _LL2295: _temp2294=(( struct Cyc_Absyn_StructType_struct*)
_temp2242)->f2; goto _LL2293; _LL2293: _temp2292=(( struct Cyc_Absyn_StructType_struct*)
_temp2242)->f3; goto _LL2247;} else{ goto _LL2248;} _LL2248: if(( unsigned int)
_temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_UnionType: 0){ _LL2303:
_temp2302=(( struct Cyc_Absyn_UnionType_struct*) _temp2242)->f1; goto _LL2301;
_LL2301: _temp2300=(( struct Cyc_Absyn_UnionType_struct*) _temp2242)->f2; goto
_LL2299; _LL2299: _temp2298=(( struct Cyc_Absyn_UnionType_struct*) _temp2242)->f3;
goto _LL2249;} else{ goto _LL2250;} _LL2250: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_TunionType: 0){ _LL2305: _temp2304=(( struct Cyc_Absyn_TunionType_struct*)
_temp2242)->f1; _LL2311: _temp2310=( void*) _temp2304.tunion_info; goto _LL2309;
_LL2309: _temp2308= _temp2304.targs; goto _LL2307; _LL2307: _temp2306=( void*)
_temp2304.rgn; goto _LL2251;} else{ goto _LL2252;} _LL2252: if(( unsigned int)
_temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2313:
_temp2312=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2242)->f1; _LL2317:
_temp2316=( void*) _temp2312.field_info; goto _LL2315; _LL2315: _temp2314=
_temp2312.targs; goto _LL2253;} else{ goto _LL2254;} _LL2254: if(( unsigned int)
_temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_TypedefType: 0){ _LL2323:
_temp2322=(( struct Cyc_Absyn_TypedefType_struct*) _temp2242)->f1; goto _LL2321;
_LL2321: _temp2320=(( struct Cyc_Absyn_TypedefType_struct*) _temp2242)->f2; goto
_LL2319; _LL2319: _temp2318=(( struct Cyc_Absyn_TypedefType_struct*) _temp2242)->f3;
goto _LL2255;} else{ goto _LL2256;} _LL2256: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_ArrayType: 0){ _LL2329: _temp2328=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2242)->f1; goto _LL2327; _LL2327:
_temp2326=(( struct Cyc_Absyn_ArrayType_struct*) _temp2242)->f2; goto _LL2325;
_LL2325: _temp2324=(( struct Cyc_Absyn_ArrayType_struct*) _temp2242)->f3; goto
_LL2257;} else{ goto _LL2258;} _LL2258: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_PointerType: 0){ _LL2331: _temp2330=(( struct Cyc_Absyn_PointerType_struct*)
_temp2242)->f1; _LL2341: _temp2340=( void*) _temp2330.elt_typ; goto _LL2339;
_LL2339: _temp2338=( void*) _temp2330.rgn_typ; goto _LL2337; _LL2337: _temp2336=
_temp2330.nullable; goto _LL2335; _LL2335: _temp2334= _temp2330.tq; goto _LL2333;
_LL2333: _temp2332= _temp2330.bounds; goto _LL2259;} else{ goto _LL2260;}
_LL2260: if(( unsigned int) _temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_FnType:
0){ _LL2343: _temp2342=(( struct Cyc_Absyn_FnType_struct*) _temp2242)->f1;
_LL2359: _temp2358= _temp2342.tvars; goto _LL2357; _LL2357: _temp2356= _temp2342.effect;
goto _LL2355; _LL2355: _temp2354=( void*) _temp2342.ret_typ; goto _LL2353;
_LL2353: _temp2352= _temp2342.args; goto _LL2351; _LL2351: _temp2350= _temp2342.c_varargs;
goto _LL2349; _LL2349: _temp2348= _temp2342.cyc_varargs; goto _LL2347; _LL2347:
_temp2346= _temp2342.rgn_po; goto _LL2345; _LL2345: _temp2344= _temp2342.attributes;
goto _LL2261;} else{ goto _LL2262;} _LL2262: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_TupleType: 0){ _LL2361: _temp2360=(( struct Cyc_Absyn_TupleType_struct*)
_temp2242)->f1; goto _LL2263;} else{ goto _LL2264;} _LL2264: if(( unsigned int)
_temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_AnonStructType: 0){ _LL2363:
_temp2362=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2242)->f1; goto
_LL2265;} else{ goto _LL2266;} _LL2266: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2365: _temp2364=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2242)->f1; goto _LL2267;} else{ goto
_LL2268;} _LL2268: if(( unsigned int) _temp2242 >  4u?*(( int*) _temp2242) == 
Cyc_Absyn_Evar: 0){ _LL2367: _temp2366=(( struct Cyc_Absyn_Evar_struct*)
_temp2242)->f2; goto _LL2269;} else{ goto _LL2270;} _LL2270: if(( unsigned int)
_temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2369:
_temp2368=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2242)->f1;
goto _LL2271;} else{ goto _LL2272;} _LL2272: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_EnumType: 0){ goto _LL2273;} else{ goto _LL2274;}
_LL2274: if( _temp2242 == ( void*) Cyc_Absyn_VoidType){ goto _LL2275;} else{
goto _LL2276;} _LL2276: if(( unsigned int) _temp2242 >  4u?*(( int*) _temp2242)
==  Cyc_Absyn_IntType: 0){ goto _LL2277;} else{ goto _LL2278;} _LL2278: if(
_temp2242 == ( void*) Cyc_Absyn_FloatType){ goto _LL2279;} else{ goto _LL2280;}
_LL2280: if( _temp2242 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2281;} else{
goto _LL2282;} _LL2282: if( _temp2242 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL2283;} else{ goto _LL2284;} _LL2284: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_RgnsEff: 0){ _LL2371: _temp2370=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2242)->f1; goto _LL2285;} else{ goto _LL2286;}
_LL2286: if(( unsigned int) _temp2242 >  4u?*(( int*) _temp2242) ==  Cyc_Absyn_AccessEff:
0){ _LL2373: _temp2372=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2242)->f1;
goto _LL2287;} else{ goto _LL2288;} _LL2288: if(( unsigned int) _temp2242 >  4u?*((
int*) _temp2242) ==  Cyc_Absyn_JoinEff: 0){ _LL2375: _temp2374=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2242)->f1; goto _LL2289;} else{ goto _LL2243;} _LL2245: { struct
_handler_cons _temp2376; _push_handler(& _temp2376);{ int _temp2378= 0; if(
setjmp( _temp2376.handler)){ _temp2378= 1;} if( ! _temp2378){{ void* _temp2379=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2290); _npop_handler( 0u); return _temp2379;}; _pop_handler();} else{
void* _temp2377=( void*) _exn_thrown; void* _temp2381= _temp2377; _LL2383: if(
_temp2381 ==  Cyc_Core_Not_found){ goto _LL2384;} else{ goto _LL2385;} _LL2385:
goto _LL2386; _LL2384: return t; _LL2386:( void) _throw( _temp2381); _LL2382:;}}}
_LL2247: { struct Cyc_List_List* _temp2387= Cyc_Tcutil_substs( rgn, inst,
_temp2294); return _temp2387 ==  _temp2294? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2388=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2388[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2389; _temp2389.tag=
Cyc_Absyn_StructType; _temp2389.f1= _temp2296; _temp2389.f2= _temp2387;
_temp2389.f3= _temp2292; _temp2389;}); _temp2388;});} _LL2249: { struct Cyc_List_List*
_temp2390= Cyc_Tcutil_substs( rgn, inst, _temp2300); return _temp2390 == 
_temp2300? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2391=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2391[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2392; _temp2392.tag= Cyc_Absyn_UnionType;
_temp2392.f1= _temp2302; _temp2392.f2= _temp2390; _temp2392.f3= _temp2298;
_temp2392;}); _temp2391;});} _LL2251: { struct Cyc_List_List* _temp2393= Cyc_Tcutil_substs(
rgn, inst, _temp2308); void* _temp2394= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2306); return( _temp2393 ==  _temp2308? _temp2394 ==  _temp2306: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2395=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2395[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2396; _temp2396.tag= Cyc_Absyn_TunionType;
_temp2396.f1=({ struct Cyc_Absyn_TunionInfo _temp2397; _temp2397.tunion_info=(
void*) _temp2310; _temp2397.targs= _temp2393; _temp2397.rgn=( void*) _temp2394;
_temp2397;}); _temp2396;}); _temp2395;});} _LL2253: { struct Cyc_List_List*
_temp2398= Cyc_Tcutil_substs( rgn, inst, _temp2314); return _temp2398 == 
_temp2314? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2399=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2399[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2400; _temp2400.tag=
Cyc_Absyn_TunionFieldType; _temp2400.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2401; _temp2401.field_info=( void*) _temp2316; _temp2401.targs= _temp2398;
_temp2401;}); _temp2400;}); _temp2399;});} _LL2255: { struct Cyc_List_List*
_temp2402= Cyc_Tcutil_substs( rgn, inst, _temp2320); return _temp2402 == 
_temp2320? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2403=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2403[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2404; _temp2404.tag=
Cyc_Absyn_TypedefType; _temp2404.f1= _temp2322; _temp2404.f2= _temp2402;
_temp2404.f3= _temp2318; _temp2404;}); _temp2403;});} _LL2257: { void* _temp2405=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2328); return _temp2405 ==  _temp2328? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2406=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2406[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2407; _temp2407.tag= Cyc_Absyn_ArrayType;
_temp2407.f1=( void*) _temp2405; _temp2407.f2= _temp2326; _temp2407.f3=
_temp2324; _temp2407;}); _temp2406;});} _LL2259: { void* _temp2408= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2340); void* _temp2409= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2338); if( _temp2408 ==  _temp2340? _temp2409 ==  _temp2338: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2410=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2410[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2411; _temp2411.tag= Cyc_Absyn_PointerType;
_temp2411.f1=({ struct Cyc_Absyn_PtrInfo _temp2412; _temp2412.elt_typ=( void*)
_temp2408; _temp2412.rgn_typ=( void*) _temp2409; _temp2412.nullable= _temp2336;
_temp2412.tq= _temp2334; _temp2412.bounds= _temp2332; _temp2412;}); _temp2411;});
_temp2410;});} _LL2261:{ struct Cyc_List_List* _temp2413= _temp2358; for( 0;
_temp2413 !=  0; _temp2413=(( struct Cyc_List_List*) _check_null( _temp2413))->tl){
inst=({ struct Cyc_List_List* _temp2414=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2414->hd=( void*)({ struct _tuple5*
_temp2415=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2415->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2413))->hd; _temp2415->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2416=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2416[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2417; _temp2417.tag= Cyc_Absyn_VarType;
_temp2417.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2413))->hd; _temp2417;}); _temp2416;}); _temp2415;}); _temp2414->tl= inst;
_temp2414;});}}{ struct Cyc_List_List* _temp2420; struct Cyc_List_List*
_temp2422; struct _tuple0 _temp2418=(( struct _tuple0(*)( struct _RegionHandle*
r1, struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2352)); _LL2423:
_temp2422= _temp2418.f1; goto _LL2421; _LL2421: _temp2420= _temp2418.f2; goto
_LL2419; _LL2419: { struct Cyc_List_List* _temp2424= Cyc_Tcutil_substs( rgn,
inst, _temp2420); struct Cyc_List_List* _temp2425=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r1,
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2422, _temp2424)); struct Cyc_Core_Opt* eff2; if( _temp2356 ==  0){
eff2= 0;} else{ void* _temp2426= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2356))->v); if( _temp2426 == ( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2356))->v){ eff2= _temp2356;} else{ eff2=({
struct Cyc_Core_Opt* _temp2427=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2427->v=( void*) _temp2426; _temp2427;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2348 ==  0){ cyc_varargs2= 0;} else{ int _temp2430; void*
_temp2432; struct Cyc_Absyn_Tqual _temp2434; struct Cyc_Core_Opt* _temp2436;
struct Cyc_Absyn_VarargInfo _temp2428=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2348)); _LL2437: _temp2436= _temp2428.name; goto _LL2435;
_LL2435: _temp2434= _temp2428.tq; goto _LL2433; _LL2433: _temp2432=( void*)
_temp2428.type; goto _LL2431; _LL2431: _temp2430= _temp2428.inject; goto _LL2429;
_LL2429: { void* _temp2438= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2432); if(
_temp2438 ==  _temp2432){ cyc_varargs2= _temp2348;} else{ cyc_varargs2=({ struct
Cyc_Absyn_VarargInfo* _temp2439=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2439->name= _temp2436; _temp2439->tq=
_temp2434; _temp2439->type=( void*) _temp2438; _temp2439->inject= _temp2430;
_temp2439;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2442;
struct Cyc_List_List* _temp2444; struct _tuple0 _temp2440= Cyc_List_rsplit( rgn,
rgn, _temp2346); _LL2445: _temp2444= _temp2440.f1; goto _LL2443; _LL2443:
_temp2442= _temp2440.f2; goto _LL2441; _LL2441: { struct Cyc_List_List*
_temp2446= Cyc_Tcutil_substs( rgn, inst, _temp2444); struct Cyc_List_List*
_temp2447= Cyc_Tcutil_substs( rgn, inst, _temp2442); if( _temp2446 ==  _temp2444?
_temp2447 ==  _temp2442: 0){ rgn_po2= _temp2346;} else{ rgn_po2= Cyc_List_zip(
_temp2446, _temp2447);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2448=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2448[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2449; _temp2449.tag= Cyc_Absyn_FnType;
_temp2449.f1=({ struct Cyc_Absyn_FnInfo _temp2450; _temp2450.tvars= _temp2358;
_temp2450.effect= eff2; _temp2450.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2354); _temp2450.args= _temp2425; _temp2450.c_varargs= _temp2350;
_temp2450.cyc_varargs= cyc_varargs2; _temp2450.rgn_po= rgn_po2; _temp2450.attributes=
_temp2344; _temp2450;}); _temp2449;}); _temp2448;});}}}}} _LL2263: { struct Cyc_List_List*
_temp2451=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2360); struct Cyc_List_List* _temp2452= Cyc_Tcutil_substs( rgn, inst,
_temp2451); if( _temp2452 ==  _temp2451){ return t;}{ struct Cyc_List_List*
_temp2453=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2360, _temp2452); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2454=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2454[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2455; _temp2455.tag= Cyc_Absyn_TupleType;
_temp2455.f1= _temp2453; _temp2455;}); _temp2454;});}} _LL2265: { struct Cyc_List_List*
_temp2456=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2362); struct Cyc_List_List* _temp2457= Cyc_Tcutil_substs( rgn, inst,
_temp2456); if( _temp2457 ==  _temp2456){ return t;}{ struct Cyc_List_List*
_temp2458=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2362, _temp2457); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2459=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2459[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2460; _temp2460.tag= Cyc_Absyn_AnonStructType;
_temp2460.f1= _temp2458; _temp2460;}); _temp2459;});}} _LL2267: { struct Cyc_List_List*
_temp2461=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2364); struct Cyc_List_List* _temp2462= Cyc_Tcutil_substs( rgn, inst,
_temp2461); if( _temp2462 ==  _temp2461){ return t;}{ struct Cyc_List_List*
_temp2463=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2364, _temp2462); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2464=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2464[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2465; _temp2465.tag= Cyc_Absyn_AnonStructType;
_temp2465.f1= _temp2463; _temp2465;}); _temp2464;});}} _LL2269: if( _temp2366 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2366))->v);} else{ return t;} _LL2271: { void* _temp2466= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2368); return _temp2466 ==  _temp2368? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2467=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2467[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2468; _temp2468.tag= Cyc_Absyn_RgnHandleType; _temp2468.f1=( void*)
_temp2466; _temp2468;}); _temp2467;});} _LL2273: return t; _LL2275: return t;
_LL2277: return t; _LL2279: return t; _LL2281: return t; _LL2283: return t;
_LL2285: { void* _temp2469= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2370);
return _temp2469 ==  _temp2370? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2470=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2470[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2471; _temp2471.tag= Cyc_Absyn_RgnsEff;
_temp2471.f1=( void*) _temp2469; _temp2471;}); _temp2470;});} _LL2287: { void*
_temp2472= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2372); return _temp2472 == 
_temp2372? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2473=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2473[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2474; _temp2474.tag= Cyc_Absyn_AccessEff;
_temp2474.f1=( void*) _temp2472; _temp2474;}); _temp2473;});} _LL2289: { struct
Cyc_List_List* _temp2475= Cyc_Tcutil_substs( rgn, inst, _temp2374); return
_temp2475 ==  _temp2374? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2476=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2476[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2477; _temp2477.tag= Cyc_Absyn_JoinEff;
_temp2477.f1= _temp2475; _temp2477;}); _temp2476;});} _LL2243:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2478=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2479=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2480= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2478); struct Cyc_List_List* _temp2481= Cyc_Tcutil_substs( rgn,
inst, _temp2479); if( _temp2478 ==  _temp2480? _temp2479 ==  _temp2481: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2482=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2482->hd=(
void*) _temp2480; _temp2482->tl= _temp2481; _temp2482;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2527=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2527[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2528; _temp2528.tag= Cyc_Absyn_Const_e; _temp2528.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2528;}); _temp2527;}), loc);{ void*
_temp2483= Cyc_Tcutil_compress( t); void* _temp2497; void* _temp2499; void*
_temp2501; void* _temp2503; _LL2485: if(( unsigned int) _temp2483 >  4u?*(( int*)
_temp2483) ==  Cyc_Absyn_PointerType: 0){ goto _LL2486;} else{ goto _LL2487;}
_LL2487: if(( unsigned int) _temp2483 >  4u?*(( int*) _temp2483) ==  Cyc_Absyn_IntType:
0){ _LL2500: _temp2499=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2483)->f1;
goto _LL2498; _LL2498: _temp2497=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2483)->f2; if( _temp2497 == ( void*) Cyc_Absyn_B1){ goto _LL2488;} else{
goto _LL2489;}} else{ goto _LL2489;} _LL2489: if(( unsigned int) _temp2483 >  4u?*((
int*) _temp2483) ==  Cyc_Absyn_IntType: 0){ _LL2504: _temp2503=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2483)->f1; goto _LL2502; _LL2502: _temp2501=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2483)->f2; goto _LL2490;} else{
goto _LL2491;} _LL2491: if( _temp2483 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2492;} else{ goto _LL2493;} _LL2493: if( _temp2483 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2494;} else{ goto _LL2495;} _LL2495: goto _LL2496; _LL2486: goto _LL2484;
_LL2488:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2505=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2505[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2506; _temp2506.tag= Cyc_Absyn_Const_e;
_temp2506.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2507=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2507[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2508; _temp2508.tag= Cyc_Absyn_Char_c;
_temp2508.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2508.f2='\000'; _temp2508;});
_temp2507;})); _temp2506;}); _temp2505;}))); goto _LL2484; _LL2490:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2509=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2509[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2510; _temp2510.tag= Cyc_Absyn_Const_e; _temp2510.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2511=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2511[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2512; _temp2512.tag= Cyc_Absyn_Int_c; _temp2512.f1=( void*) _temp2503;
_temp2512.f2= 0; _temp2512;}); _temp2511;})); _temp2510;}); _temp2509;}))); if(
_temp2501 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2513=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2513[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2514; _temp2514.tag= Cyc_Absyn_Cast_e;
_temp2514.f1=( void*) t; _temp2514.f2= e; _temp2514;}); _temp2513;}), loc);}
goto _LL2484; _LL2492:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2515=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2515[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2516; _temp2516.tag= Cyc_Absyn_Const_e;
_temp2516.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2517=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2517[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2518; _temp2518.tag= Cyc_Absyn_Float_c;
_temp2518.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2518;});
_temp2517;})); _temp2516;}); _temp2515;}))); goto _LL2484; _LL2494:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2519=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2519[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2520; _temp2520.tag= Cyc_Absyn_Cast_e; _temp2520.f1=( void*) t; _temp2520.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2521=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2521[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2522; _temp2522.tag= Cyc_Absyn_Const_e;
_temp2522.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2523=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2523[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2524; _temp2524.tag= Cyc_Absyn_Float_c;
_temp2524.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2524;});
_temp2523;})); _temp2522;}); _temp2521;}), loc); _temp2520;}); _temp2519;})));
goto _LL2484; _LL2496:({ struct Cyc_Std_String_pa_struct _temp2526; _temp2526.tag=
Cyc_Std_String_pa; _temp2526.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2525[ 1u]={& _temp2526}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2525, sizeof( void*), 1u));}});
goto _LL2484; _LL2484:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2529=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp2529->f1= tv; _temp2529->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2530=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2530->v=( void*) k; _temp2530;}),({ struct Cyc_Core_Opt*
_temp2531=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2531->v=( void*) s; _temp2531;})); _temp2529;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2534;
struct _RegionHandle* _temp2535; struct Cyc_List_List* _temp2537; struct _tuple6*
_temp2532= env; _temp2534=* _temp2532; _LL2538: _temp2537= _temp2534.f1; goto
_LL2536; _LL2536: _temp2535= _temp2534.f2; goto _LL2533; _LL2533: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2539=( struct _tuple5*) _region_malloc(
_temp2535, sizeof( struct _tuple5)); _temp2539->f1= tv; _temp2539->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2540=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2540->v=( void*) k; _temp2540;}),({ struct Cyc_Core_Opt*
_temp2541=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2541->v=( void*) _temp2537; _temp2541;})); _temp2539;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_Std_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_Std_String_pa_struct _temp2545; _temp2545.tag= Cyc_Std_String_pa;
_temp2545.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct
_temp2544; _temp2544.tag= Cyc_Std_String_pa; _temp2544.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_Std_String_pa_struct _temp2543;
_temp2543.tag= Cyc_Std_String_pa; _temp2543.f1=( struct _tagged_arr)* tv->name;{
void* _temp2542[ 3u]={& _temp2543,& _temp2544,& _temp2545}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2542, sizeof( void*), 3u));}}}});} if( tv->identity
==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!= *(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2546[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2546, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2547=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2547->hd=( void*) tv; _temp2547->tl= tvs; _temp2547;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2548[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2548, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity ==  0){({ void* _temp2549[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2549, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2550=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2550->hd=( void*) tv; _temp2550->tl=
tvs; _temp2550;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp2552; _temp2552.tag= Cyc_Std_String_pa;
_temp2552.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2551[
1u]={& _temp2552};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2551, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2553=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2553->hd=(
void*) tv; _temp2553->tl= tvs; _temp2553;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2554= Cyc_Tcutil_compress( e);
int _temp2560; _LL2556: if(( unsigned int) _temp2554 >  4u?*(( int*) _temp2554)
==  Cyc_Absyn_Evar: 0){ _LL2561: _temp2560=(( struct Cyc_Absyn_Evar_struct*)
_temp2554)->f3; goto _LL2557;} else{ goto _LL2558;} _LL2558: goto _LL2559;
_LL2557:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2562= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2568; _LL2564: if((
unsigned int) _temp2562 >  4u?*(( int*) _temp2562) ==  Cyc_Absyn_Evar: 0){
_LL2569: _temp2568=(( struct Cyc_Absyn_Evar_struct*) _temp2562)->f3; goto
_LL2565;} else{ goto _LL2566;} _LL2566: goto _LL2567; _LL2565: if( _temp2560 == 
_temp2568){ return es;} goto _LL2563; _LL2567: goto _LL2563; _LL2563:;}} return({
struct Cyc_List_List* _temp2570=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2570->hd=( void*) e; _temp2570->tl= es; _temp2570;});
_LL2559: return es; _LL2555:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b !=  0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) == *(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2571=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2571->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2571->tl= r; _temp2571;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp2573; _temp2573.tag= Cyc_Std_String_pa; _temp2573.f1=( struct _tagged_arr)*
fn;{ void* _temp2572[ 1u]={& _temp2573}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2572, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2574= Cyc_Tcutil_compress( field_typ); void* _temp2580;
_LL2576: if(( unsigned int) _temp2574 >  4u?*(( int*) _temp2574) ==  Cyc_Absyn_IntType:
0){ _LL2581: _temp2580=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2574)->f2;
goto _LL2577;} else{ goto _LL2578;} _LL2578: goto _LL2579; _LL2577:{ void*
_temp2582= _temp2580; _LL2584: if( _temp2582 == ( void*) Cyc_Absyn_B1){ goto
_LL2585;} else{ goto _LL2586;} _LL2586: if( _temp2582 == ( void*) Cyc_Absyn_B2){
goto _LL2587;} else{ goto _LL2588;} _LL2588: if( _temp2582 == ( void*) Cyc_Absyn_B4){
goto _LL2589;} else{ goto _LL2590;} _LL2590: if( _temp2582 == ( void*) Cyc_Absyn_B8){
goto _LL2591;} else{ goto _LL2583;} _LL2585: if( w >  8){({ void* _temp2592[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2592, sizeof( void*), 0u));});} goto
_LL2583; _LL2587: if( w >  16){({ void* _temp2593[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2593, sizeof( void*), 0u));});} goto _LL2583; _LL2589: if( w >  32){({ void*
_temp2594[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2594, sizeof( void*), 0u));});}
goto _LL2583; _LL2591: if( w >  64){({ void* _temp2595[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2595, sizeof( void*), 0u));});} goto _LL2583; _LL2583:;} goto
_LL2575; _LL2579:({ struct Cyc_Std_String_pa_struct _temp2598; _temp2598.tag=
Cyc_Std_String_pa; _temp2598.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp2597; _temp2597.tag= Cyc_Std_String_pa;
_temp2597.f1=( struct _tagged_arr)* fn;{ void* _temp2596[ 2u]={& _temp2597,&
_temp2598}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2596, sizeof( void*), 2u));}}});
goto _LL2575; _LL2575:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp2599=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL2601: if( _temp2599 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL2602;} else{ goto _LL2603;} _LL2603: if((
unsigned int) _temp2599 >  16u?*(( int*) _temp2599) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2604;} else{ goto _LL2605;} _LL2605: goto _LL2606; _LL2602: continue;
_LL2604: continue; _LL2606:({ struct Cyc_Std_String_pa_struct _temp2609;
_temp2609.tag= Cyc_Std_String_pa; _temp2609.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp2608; _temp2608.tag= Cyc_Std_String_pa;
_temp2608.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp2607[ 2u]={& _temp2608,&
_temp2609}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2607, sizeof( void*), 2u));}}});
goto _LL2600; _LL2600:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv
cvtenv, void* expected_kind, void* t){{ void* _temp2610= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp2658; struct Cyc_Core_Opt** _temp2660; struct Cyc_Core_Opt*
_temp2661; struct Cyc_Core_Opt** _temp2663; struct Cyc_Absyn_Tvar* _temp2664;
struct Cyc_Absyn_Enumdecl* _temp2666; struct Cyc_Absyn_Enumdecl** _temp2668;
struct _tuple1* _temp2669; struct Cyc_Absyn_TunionInfo _temp2671; void*
_temp2673; struct Cyc_List_List* _temp2675; void* _temp2677; void** _temp2679;
struct Cyc_Absyn_TunionFieldInfo _temp2680; struct Cyc_List_List* _temp2682;
void* _temp2684; void** _temp2686; struct Cyc_Absyn_PtrInfo _temp2687; struct
Cyc_Absyn_Conref* _temp2689; struct Cyc_Absyn_Tqual _temp2691; struct Cyc_Absyn_Conref*
_temp2693; void* _temp2695; void* _temp2697; struct Cyc_Absyn_Exp* _temp2699;
struct Cyc_Absyn_Tqual _temp2701; void* _temp2703; struct Cyc_Absyn_FnInfo
_temp2705; struct Cyc_List_List* _temp2707; struct Cyc_List_List* _temp2709;
struct Cyc_Absyn_VarargInfo* _temp2711; int _temp2713; struct Cyc_List_List*
_temp2715; void* _temp2717; struct Cyc_Core_Opt* _temp2719; struct Cyc_Core_Opt**
_temp2721; struct Cyc_List_List* _temp2722; struct Cyc_List_List** _temp2724;
struct Cyc_List_List* _temp2725; struct Cyc_List_List* _temp2727; struct Cyc_List_List*
_temp2729; struct Cyc_Absyn_Structdecl** _temp2731; struct Cyc_Absyn_Structdecl***
_temp2733; struct Cyc_List_List* _temp2734; struct _tuple1* _temp2736; struct
Cyc_Absyn_Uniondecl** _temp2738; struct Cyc_Absyn_Uniondecl*** _temp2740; struct
Cyc_List_List* _temp2741; struct _tuple1* _temp2743; struct Cyc_Core_Opt*
_temp2745; struct Cyc_Core_Opt** _temp2747; struct Cyc_List_List* _temp2748;
struct _tuple1* _temp2750; void* _temp2752; void* _temp2754; void* _temp2756;
struct Cyc_List_List* _temp2758; _LL2612: if( _temp2610 == ( void*) Cyc_Absyn_VoidType){
goto _LL2613;} else{ goto _LL2614;} _LL2614: if(( unsigned int) _temp2610 >  4u?*((
int*) _temp2610) ==  Cyc_Absyn_Evar: 0){ _LL2662: _temp2661=(( struct Cyc_Absyn_Evar_struct*)
_temp2610)->f1; _temp2663=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp2610)->f1; goto _LL2659; _LL2659: _temp2658=(( struct Cyc_Absyn_Evar_struct*)
_temp2610)->f2; _temp2660=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp2610)->f2; goto _LL2615;} else{ goto _LL2616;} _LL2616: if(( unsigned int)
_temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_VarType: 0){ _LL2665:
_temp2664=(( struct Cyc_Absyn_VarType_struct*) _temp2610)->f1; goto _LL2617;}
else{ goto _LL2618;} _LL2618: if(( unsigned int) _temp2610 >  4u?*(( int*)
_temp2610) ==  Cyc_Absyn_EnumType: 0){ _LL2670: _temp2669=(( struct Cyc_Absyn_EnumType_struct*)
_temp2610)->f1; goto _LL2667; _LL2667: _temp2666=(( struct Cyc_Absyn_EnumType_struct*)
_temp2610)->f2; _temp2668=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2610)->f2; goto _LL2619;} else{ goto _LL2620;} _LL2620: if(( unsigned int)
_temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_TunionType: 0){ _LL2672:
_temp2671=(( struct Cyc_Absyn_TunionType_struct*) _temp2610)->f1; _LL2678:
_temp2677=( void*) _temp2671.tunion_info; _temp2679=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2610)->f1).tunion_info; goto _LL2676; _LL2676: _temp2675= _temp2671.targs;
goto _LL2674; _LL2674: _temp2673=( void*) _temp2671.rgn; goto _LL2621;} else{
goto _LL2622;} _LL2622: if(( unsigned int) _temp2610 >  4u?*(( int*) _temp2610)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2681: _temp2680=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2610)->f1; _LL2685: _temp2684=( void*) _temp2680.field_info; _temp2686=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2610)->f1).field_info;
goto _LL2683; _LL2683: _temp2682= _temp2680.targs; goto _LL2623;} else{ goto
_LL2624;} _LL2624: if(( unsigned int) _temp2610 >  4u?*(( int*) _temp2610) == 
Cyc_Absyn_PointerType: 0){ _LL2688: _temp2687=(( struct Cyc_Absyn_PointerType_struct*)
_temp2610)->f1; _LL2698: _temp2697=( void*) _temp2687.elt_typ; goto _LL2696;
_LL2696: _temp2695=( void*) _temp2687.rgn_typ; goto _LL2694; _LL2694: _temp2693=
_temp2687.nullable; goto _LL2692; _LL2692: _temp2691= _temp2687.tq; goto _LL2690;
_LL2690: _temp2689= _temp2687.bounds; goto _LL2625;} else{ goto _LL2626;}
_LL2626: if(( unsigned int) _temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_IntType:
0){ goto _LL2627;} else{ goto _LL2628;} _LL2628: if( _temp2610 == ( void*) Cyc_Absyn_FloatType){
goto _LL2629;} else{ goto _LL2630;} _LL2630: if( _temp2610 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2631;} else{ goto _LL2632;} _LL2632: if(( unsigned int) _temp2610 >  4u?*((
int*) _temp2610) ==  Cyc_Absyn_ArrayType: 0){ _LL2704: _temp2703=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2610)->f1; goto _LL2702; _LL2702:
_temp2701=(( struct Cyc_Absyn_ArrayType_struct*) _temp2610)->f2; goto _LL2700;
_LL2700: _temp2699=(( struct Cyc_Absyn_ArrayType_struct*) _temp2610)->f3; goto
_LL2633;} else{ goto _LL2634;} _LL2634: if(( unsigned int) _temp2610 >  4u?*((
int*) _temp2610) ==  Cyc_Absyn_FnType: 0){ _LL2706: _temp2705=(( struct Cyc_Absyn_FnType_struct*)
_temp2610)->f1; _LL2723: _temp2722= _temp2705.tvars; _temp2724=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2610)->f1).tvars; goto _LL2720; _LL2720:
_temp2719= _temp2705.effect; _temp2721=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2610)->f1).effect; goto _LL2718; _LL2718: _temp2717=( void*) _temp2705.ret_typ;
goto _LL2716; _LL2716: _temp2715= _temp2705.args; goto _LL2714; _LL2714:
_temp2713= _temp2705.c_varargs; goto _LL2712; _LL2712: _temp2711= _temp2705.cyc_varargs;
goto _LL2710; _LL2710: _temp2709= _temp2705.rgn_po; goto _LL2708; _LL2708:
_temp2707= _temp2705.attributes; goto _LL2635;} else{ goto _LL2636;} _LL2636:
if(( unsigned int) _temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_TupleType:
0){ _LL2726: _temp2725=(( struct Cyc_Absyn_TupleType_struct*) _temp2610)->f1;
goto _LL2637;} else{ goto _LL2638;} _LL2638: if(( unsigned int) _temp2610 >  4u?*((
int*) _temp2610) ==  Cyc_Absyn_AnonStructType: 0){ _LL2728: _temp2727=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2610)->f1; goto _LL2639;} else{ goto
_LL2640;} _LL2640: if(( unsigned int) _temp2610 >  4u?*(( int*) _temp2610) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2730: _temp2729=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2610)->f1; goto _LL2641;} else{ goto _LL2642;} _LL2642: if(( unsigned int)
_temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_StructType: 0){ _LL2737:
_temp2736=(( struct Cyc_Absyn_StructType_struct*) _temp2610)->f1; goto _LL2735;
_LL2735: _temp2734=(( struct Cyc_Absyn_StructType_struct*) _temp2610)->f2; goto
_LL2732; _LL2732: _temp2731=(( struct Cyc_Absyn_StructType_struct*) _temp2610)->f3;
_temp2733=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2610)->f3; goto _LL2643;} else{ goto _LL2644;} _LL2644: if(( unsigned int)
_temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_UnionType: 0){ _LL2744:
_temp2743=(( struct Cyc_Absyn_UnionType_struct*) _temp2610)->f1; goto _LL2742;
_LL2742: _temp2741=(( struct Cyc_Absyn_UnionType_struct*) _temp2610)->f2; goto
_LL2739; _LL2739: _temp2738=(( struct Cyc_Absyn_UnionType_struct*) _temp2610)->f3;
_temp2740=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2610)->f3; goto _LL2645;} else{ goto _LL2646;} _LL2646: if(( unsigned int)
_temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_TypedefType: 0){ _LL2751:
_temp2750=(( struct Cyc_Absyn_TypedefType_struct*) _temp2610)->f1; goto _LL2749;
_LL2749: _temp2748=(( struct Cyc_Absyn_TypedefType_struct*) _temp2610)->f2; goto
_LL2746; _LL2746: _temp2745=(( struct Cyc_Absyn_TypedefType_struct*) _temp2610)->f3;
_temp2747=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2610)->f3; goto _LL2647;} else{ goto _LL2648;} _LL2648: if( _temp2610 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2649;} else{ goto _LL2650;} _LL2650: if((
unsigned int) _temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2753: _temp2752=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2610)->f1; goto _LL2651;} else{ goto _LL2652;} _LL2652: if(( unsigned int)
_temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_AccessEff: 0){ _LL2755:
_temp2754=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2610)->f1; goto
_LL2653;} else{ goto _LL2654;} _LL2654: if(( unsigned int) _temp2610 >  4u?*((
int*) _temp2610) ==  Cyc_Absyn_RgnsEff: 0){ _LL2757: _temp2756=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2610)->f1; goto _LL2655;} else{ goto _LL2656;}
_LL2656: if(( unsigned int) _temp2610 >  4u?*(( int*) _temp2610) ==  Cyc_Absyn_JoinEff:
0){ _LL2759: _temp2758=(( struct Cyc_Absyn_JoinEff_struct*) _temp2610)->f1; goto
_LL2657;} else{ goto _LL2611;} _LL2613: goto _LL2611; _LL2615: if(* _temp2663 == 
0){* _temp2663=({ struct Cyc_Core_Opt* _temp2760=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2760->v=( void*) expected_kind;
_temp2760;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2660=({ struct Cyc_Core_Opt* _temp2761=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2761->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2761;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2762= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2660=({ struct Cyc_Core_Opt* _temp2763=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2763->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2764=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2764[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2765; _temp2765.tag= Cyc_Absyn_VarType;
_temp2765.f1= _temp2762; _temp2765;}); _temp2764;})); _temp2763;}); _temp2664=
_temp2762; goto _LL2617;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2611; _LL2617: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2664->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2766=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2766[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2767; _temp2767.tag= Cyc_Absyn_Eq_constr;
_temp2767.f1=( void*) expected_kind; _temp2767;}); _temp2766;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2664); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2664); goto _LL2611;}
_LL2619: if(* _temp2668 ==  0){ struct _handler_cons _temp2768; _push_handler(&
_temp2768);{ int _temp2770= 0; if( setjmp( _temp2768.handler)){ _temp2770= 1;}
if( ! _temp2770){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2669);* _temp2668=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2769=( void*) _exn_thrown; void* _temp2772= _temp2769; _LL2774:
if( _temp2772 ==  Cyc_Dict_Absent){ goto _LL2775;} else{ goto _LL2776;} _LL2776:
goto _LL2777; _LL2775:({ struct Cyc_Std_String_pa_struct _temp2779; _temp2779.tag=
Cyc_Std_String_pa; _temp2779.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2669);{ void* _temp2778[ 1u]={& _temp2779}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u), _tag_arr( _temp2778, sizeof( void*), 1u));}});
return cvtenv; _LL2777:( void) _throw( _temp2772); _LL2773:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2668);* _temp2669=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2611;} _LL2621:{ void*
_temp2780=* _temp2679; struct Cyc_Absyn_UnknownTunionInfo _temp2786; int
_temp2788; struct _tuple1* _temp2790; struct Cyc_Absyn_Tuniondecl* _temp2792;
_LL2782: if(*(( int*) _temp2780) ==  Cyc_Absyn_UnknownTunion){ _LL2787:
_temp2786=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2780)->f1; _LL2791:
_temp2790= _temp2786.name; goto _LL2789; _LL2789: _temp2788= _temp2786.is_xtunion;
goto _LL2783;} else{ goto _LL2784;} _LL2784: if(*(( int*) _temp2780) ==  Cyc_Absyn_KnownTunion){
_LL2793: _temp2792=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2780)->f1; goto
_LL2785;} else{ goto _LL2781;} _LL2783: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2794; _push_handler(& _temp2794);{ int _temp2796= 0;
if( setjmp( _temp2794.handler)){ _temp2796= 1;} if( ! _temp2796){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2790);; _pop_handler();} else{ void* _temp2795=( void*)
_exn_thrown; void* _temp2798= _temp2795; _LL2800: if( _temp2798 ==  Cyc_Dict_Absent){
goto _LL2801;} else{ goto _LL2802;} _LL2802: goto _LL2803; _LL2801:({ struct Cyc_Std_String_pa_struct
_temp2805; _temp2805.tag= Cyc_Std_String_pa; _temp2805.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2790);{ void* _temp2804[ 1u]={& _temp2805}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type [x]tunion %s", sizeof( unsigned char), 26u),
_tag_arr( _temp2804, sizeof( void*), 1u));}}); return cvtenv; _LL2803:( void)
_throw( _temp2798); _LL2799:;}}} if( tud->is_xtunion !=  _temp2788){({ struct
Cyc_Std_String_pa_struct _temp2807; _temp2807.tag= Cyc_Std_String_pa; _temp2807.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2790);{ void* _temp2806[ 1u]={&
_temp2807}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2806, sizeof( void*), 1u));}});}*
_temp2679=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2808=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2808[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2809; _temp2809.tag= Cyc_Absyn_KnownTunion;
_temp2809.f1= tud; _temp2809;}); _temp2808;}); _temp2792= tud; goto _LL2785;}
_LL2785: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2673);{ struct Cyc_List_List* tvs= _temp2792->tvs; for( 0; _temp2675 !=  0?
tvs !=  0: 0;( _temp2675=(( struct Cyc_List_List*) _check_null( _temp2675))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2675))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2675
!=  0){({ struct Cyc_Std_String_pa_struct _temp2811; _temp2811.tag= Cyc_Std_String_pa;
_temp2811.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2792->name);{
void* _temp2810[ 1u]={& _temp2811}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2810, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2813; _temp2813.tag= Cyc_Std_String_pa;
_temp2813.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2792->name);{
void* _temp2812[ 1u]={& _temp2813}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2812, sizeof( void*), 1u));}});}
goto _LL2781;} _LL2781:;} goto _LL2611; _LL2623:{ void* _temp2814=* _temp2686;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2820; int _temp2822; struct _tuple1*
_temp2824; struct _tuple1* _temp2826; struct Cyc_Absyn_Tunionfield* _temp2828;
struct Cyc_Absyn_Tuniondecl* _temp2830; _LL2816: if(*(( int*) _temp2814) ==  Cyc_Absyn_UnknownTunionfield){
_LL2821: _temp2820=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2814)->f1;
_LL2827: _temp2826= _temp2820.tunion_name; goto _LL2825; _LL2825: _temp2824=
_temp2820.field_name; goto _LL2823; _LL2823: _temp2822= _temp2820.is_xtunion;
goto _LL2817;} else{ goto _LL2818;} _LL2818: if(*(( int*) _temp2814) ==  Cyc_Absyn_KnownTunionfield){
_LL2831: _temp2830=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2814)->f1;
goto _LL2829; _LL2829: _temp2828=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2814)->f2; goto _LL2819;} else{ goto _LL2815;} _LL2817: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2832;
_push_handler(& _temp2832);{ int _temp2834= 0; if( setjmp( _temp2832.handler)){
_temp2834= 1;} if( ! _temp2834){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2826);; _pop_handler();} else{ void* _temp2833=( void*) _exn_thrown; void*
_temp2836= _temp2833; _LL2838: if( _temp2836 ==  Cyc_Dict_Absent){ goto _LL2839;}
else{ goto _LL2840;} _LL2840: goto _LL2841; _LL2839:({ struct Cyc_Std_String_pa_struct
_temp2843; _temp2843.tag= Cyc_Std_String_pa; _temp2843.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2826);{ void* _temp2842[ 1u]={& _temp2843}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2842, sizeof( void*), 1u));}}); return cvtenv; _LL2841:( void) _throw(
_temp2836); _LL2837:;}}}{ struct _handler_cons _temp2844; _push_handler(&
_temp2844);{ int _temp2846= 0; if( setjmp( _temp2844.handler)){ _temp2846= 1;}
if( ! _temp2846){{ void* _temp2847= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2824); struct Cyc_Absyn_Tunionfield* _temp2853; struct Cyc_Absyn_Tuniondecl*
_temp2855; _LL2849: if(*(( int*) _temp2847) ==  Cyc_Tcenv_TunionRes){ _LL2856:
_temp2855=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2847)->f1; goto _LL2854;
_LL2854: _temp2853=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2847)->f2; goto
_LL2850;} else{ goto _LL2851;} _LL2851: goto _LL2852; _LL2850: tuf= _temp2853;
tud= _temp2855; if( tud->is_xtunion !=  _temp2822){({ struct Cyc_Std_String_pa_struct
_temp2858; _temp2858.tag= Cyc_Std_String_pa; _temp2858.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2826);{ void* _temp2857[ 1u]={& _temp2858}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2857, sizeof( void*), 1u));}});} goto
_LL2848; _LL2852:({ struct Cyc_Std_String_pa_struct _temp2861; _temp2861.tag=
Cyc_Std_String_pa; _temp2861.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2826);{ struct Cyc_Std_String_pa_struct _temp2860; _temp2860.tag= Cyc_Std_String_pa;
_temp2860.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2824);{ void*
_temp2859[ 2u]={& _temp2860,& _temp2861}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2859, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2862= cvtenv; _npop_handler( 0u); return _temp2862;}
_LL2848:;}; _pop_handler();} else{ void* _temp2845=( void*) _exn_thrown; void*
_temp2864= _temp2845; _LL2866: if( _temp2864 ==  Cyc_Dict_Absent){ goto _LL2867;}
else{ goto _LL2868;} _LL2868: goto _LL2869; _LL2867:({ struct Cyc_Std_String_pa_struct
_temp2872; _temp2872.tag= Cyc_Std_String_pa; _temp2872.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2826);{ struct Cyc_Std_String_pa_struct _temp2871;
_temp2871.tag= Cyc_Std_String_pa; _temp2871.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2824);{ void* _temp2870[ 2u]={& _temp2871,& _temp2872}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2870, sizeof( void*), 2u));}}}); return cvtenv; _LL2869:( void)
_throw( _temp2864); _LL2865:;}}}* _temp2686=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2873=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2873[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2874; _temp2874.tag= Cyc_Absyn_KnownTunionfield; _temp2874.f1= tud;
_temp2874.f2= tuf; _temp2874;}); _temp2873;}); _temp2830= tud; _temp2828= tuf;
goto _LL2819;} _LL2819: { struct Cyc_List_List* tvs= _temp2830->tvs; for( 0;
_temp2682 !=  0? tvs !=  0: 0;( _temp2682=(( struct Cyc_List_List*) _check_null(
_temp2682))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2682))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2682
!=  0){({ struct Cyc_Std_String_pa_struct _temp2877; _temp2877.tag= Cyc_Std_String_pa;
_temp2877.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2828->name);{
struct Cyc_Std_String_pa_struct _temp2876; _temp2876.tag= Cyc_Std_String_pa;
_temp2876.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2830->name);{
void* _temp2875[ 2u]={& _temp2876,& _temp2877}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2875, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2880; _temp2880.tag= Cyc_Std_String_pa;
_temp2880.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2828->name);{
struct Cyc_Std_String_pa_struct _temp2879; _temp2879.tag= Cyc_Std_String_pa;
_temp2879.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2830->name);{
void* _temp2878[ 2u]={& _temp2879,& _temp2880}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2878, sizeof( void*), 2u));}}});}
goto _LL2815;} _LL2815:;} goto _LL2611; _LL2625: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2697); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2695);{ void* _temp2881=( void*)(
Cyc_Absyn_compress_conref( _temp2689))->v; void* _temp2887; struct Cyc_Absyn_Exp*
_temp2889; _LL2883: if(( unsigned int) _temp2881 >  1u?*(( int*) _temp2881) == 
Cyc_Absyn_Eq_constr: 0){ _LL2888: _temp2887=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2881)->f1; if(( unsigned int) _temp2887 >  1u?*(( int*) _temp2887) ==  Cyc_Absyn_Upper_b:
0){ _LL2890: _temp2889=(( struct Cyc_Absyn_Upper_b_struct*) _temp2887)->f1; goto
_LL2884;} else{ goto _LL2885;}} else{ goto _LL2885;} _LL2885: goto _LL2886;
_LL2884: if( ! Cyc_Tcutil_is_const_exp( te, _temp2889)){({ void* _temp2891[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2891, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2889); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2889)){({ void*
_temp2892[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2892, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2889); goto _LL2882; _LL2886: goto _LL2882; _LL2882:;} goto _LL2611;
_LL2627: goto _LL2611; _LL2629: goto _LL2611; _LL2631: goto _LL2611; _LL2633:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2703); if( _temp2699 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2699))){({ void* _temp2893[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2893, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2699)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2699))){({ void* _temp2894[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2894, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2699)); goto _LL2611; _LL2635: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2707 !=  0; _temp2707=(( struct Cyc_List_List*)
_check_null( _temp2707))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2707))->hd)){({ struct Cyc_Std_String_pa_struct _temp2896;
_temp2896.tag= Cyc_Std_String_pa; _temp2896.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2707))->hd);{ void* _temp2895[
1u]={& _temp2896}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2895, sizeof( void*), 1u));}});}{
void* _temp2897=( void*)(( struct Cyc_List_List*) _check_null( _temp2707))->hd;
int _temp2909; int _temp2911; void* _temp2913; _LL2899: if( _temp2897 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2900;} else{ goto _LL2901;} _LL2901: if(
_temp2897 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL2902;} else{ goto _LL2903;}
_LL2903: if( _temp2897 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL2904;} else{
goto _LL2905;} _LL2905: if(( unsigned int) _temp2897 >  16u?*(( int*) _temp2897)
==  Cyc_Absyn_Format_att: 0){ _LL2914: _temp2913=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2897)->f1; goto _LL2912; _LL2912: _temp2911=(( struct Cyc_Absyn_Format_att_struct*)
_temp2897)->f2; goto _LL2910; _LL2910: _temp2909=(( struct Cyc_Absyn_Format_att_struct*)
_temp2897)->f3; goto _LL2906;} else{ goto _LL2907;} _LL2907: goto _LL2908;
_LL2900: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2898;
_LL2902: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2898; _LL2904:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2898; _LL2906:
if( ! seen_format){ seen_format= 1; ft= _temp2913; fmt_desc_arg= _temp2911;
fmt_arg_start= _temp2909;} else{({ void* _temp2915[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2915, sizeof( void*), 0u));});} goto _LL2898; _LL2908: goto
_LL2898; _LL2898:;}} if( num_convs >  1){({ void* _temp2916[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2916, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2724);{ struct Cyc_List_List* b=* _temp2724; for( 0; b !=  0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2917=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2923; _LL2919: if(( unsigned int) _temp2917 >  1u?*(( int*) _temp2917)
==  Cyc_Absyn_Eq_constr: 0){ _LL2924: _temp2923=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2917)->f1; if( _temp2923 == ( void*) Cyc_Absyn_MemKind){ goto _LL2920;}
else{ goto _LL2921;}} else{ goto _LL2921;} _LL2921: goto _LL2922; _LL2920:({
struct Cyc_Std_String_pa_struct _temp2926; _temp2926.tag= Cyc_Std_String_pa;
_temp2926.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2925[ 1u]={& _temp2926}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2925, sizeof( void*), 1u));}}); goto
_LL2918; _LL2922: goto _LL2918; _LL2918:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2927=({
struct Cyc_Tcutil_CVTEnv _temp3084; _temp3084.kind_env= cvtenv.kind_env;
_temp3084.free_vars= 0; _temp3084.free_evars= 0; _temp3084.generalize_evars=
cvtenv.generalize_evars; _temp3084.fn_result= 1; _temp3084;}); _temp2927= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2927,( void*) Cyc_Absyn_MemKind, _temp2717); _temp2927.fn_result=
0;{ struct Cyc_List_List* a= _temp2715; for( 0; a !=  0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ _temp2927= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2927,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( a))->hd)).f3);}} if( _temp2711 !=  0){ if( _temp2713){({ void*
_temp2928[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2928, sizeof( void*), 0u));});}{ int _temp2931; void* _temp2933; struct Cyc_Absyn_Tqual
_temp2935; struct Cyc_Core_Opt* _temp2937; struct Cyc_Absyn_VarargInfo _temp2929=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp2711)); _LL2938: _temp2937=
_temp2929.name; goto _LL2936; _LL2936: _temp2935= _temp2929.tq; goto _LL2934;
_LL2934: _temp2933=( void*) _temp2929.type; goto _LL2932; _LL2932: _temp2931=
_temp2929.inject; goto _LL2930; _LL2930: _temp2927= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2927,( void*) Cyc_Absyn_MemKind, _temp2933); if( _temp2931){ void*
_temp2939= Cyc_Tcutil_compress( _temp2933); struct Cyc_Absyn_TunionInfo
_temp2945; void* _temp2947; void* _temp2949; _LL2941: if(( unsigned int)
_temp2939 >  4u?*(( int*) _temp2939) ==  Cyc_Absyn_TunionType: 0){ _LL2946:
_temp2945=(( struct Cyc_Absyn_TunionType_struct*) _temp2939)->f1; _LL2950:
_temp2949=( void*) _temp2945.tunion_info; if(*(( int*) _temp2949) ==  Cyc_Absyn_KnownTunion){
goto _LL2948;} else{ goto _LL2943;} _LL2948: _temp2947=( void*) _temp2945.rgn;
goto _LL2942;} else{ goto _LL2943;} _LL2943: goto _LL2944; _LL2942: goto _LL2940;
_LL2944:({ void* _temp2951[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u), _tag_arr( _temp2951, sizeof( void*), 0u));}); goto
_LL2940; _LL2940:;}}} if( seen_format){ int _temp2952=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2715); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg > 
_temp2952)? 1: fmt_arg_start <  0)? 1:( _temp2711 ==  0? fmt_arg_start !=  0: 0))?
1:( _temp2711 !=  0? fmt_arg_start !=  _temp2952 +  1: 0)){({ void* _temp2953[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2953, sizeof( void*), 0u));});} else{ void* _temp2956;
struct _tuple2 _temp2954=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp2715, fmt_desc_arg -  1); _LL2957: _temp2956= _temp2954.f3;
goto _LL2955; _LL2955:{ void* _temp2958= Cyc_Tcutil_compress( _temp2956); struct
Cyc_Absyn_PtrInfo _temp2964; struct Cyc_Absyn_Conref* _temp2966; void* _temp2968;
_LL2960: if(( unsigned int) _temp2958 >  4u?*(( int*) _temp2958) ==  Cyc_Absyn_PointerType:
0){ _LL2965: _temp2964=(( struct Cyc_Absyn_PointerType_struct*) _temp2958)->f1;
_LL2969: _temp2968=( void*) _temp2964.elt_typ; goto _LL2967; _LL2967: _temp2966=
_temp2964.bounds; goto _LL2961;} else{ goto _LL2962;} _LL2962: goto _LL2963;
_LL2961:{ struct _tuple8 _temp2971=({ struct _tuple8 _temp2970; _temp2970.f1=
Cyc_Tcutil_compress( _temp2968); _temp2970.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2966); _temp2970;}); void* _temp2977; void* _temp2979; void* _temp2981;
void* _temp2983; _LL2973: _LL2980: _temp2979= _temp2971.f1; if(( unsigned int)
_temp2979 >  4u?*(( int*) _temp2979) ==  Cyc_Absyn_IntType: 0){ _LL2984:
_temp2983=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2979)->f1; if(
_temp2983 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2982;} else{ goto _LL2975;}
_LL2982: _temp2981=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2979)->f2;
if( _temp2981 == ( void*) Cyc_Absyn_B1){ goto _LL2978;} else{ goto _LL2975;}}
else{ goto _LL2975;} _LL2978: _temp2977= _temp2971.f2; if( _temp2977 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2974;} else{ goto _LL2975;} _LL2975: goto _LL2976;
_LL2974: goto _LL2972; _LL2976:({ void* _temp2985[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp2985, sizeof( void*), 0u));}); goto _LL2972; _LL2972:;} goto
_LL2959; _LL2963:({ void* _temp2986[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2986, sizeof( void*), 0u));}); goto
_LL2959; _LL2959:;} if( fmt_arg_start !=  0){ void* _temp2987= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2711))->type); int
problem;{ void* _temp2988= ft; _LL2990: if( _temp2988 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL2991;} else{ goto _LL2992;} _LL2992: if( _temp2988 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL2993;} else{ goto _LL2989;} _LL2991:{ void* _temp2994= _temp2987; struct
Cyc_Absyn_TunionInfo _temp3000; void* _temp3002; struct Cyc_Absyn_Tuniondecl*
_temp3004; _LL2996: if(( unsigned int) _temp2994 >  4u?*(( int*) _temp2994) == 
Cyc_Absyn_TunionType: 0){ _LL3001: _temp3000=(( struct Cyc_Absyn_TunionType_struct*)
_temp2994)->f1; _LL3003: _temp3002=( void*) _temp3000.tunion_info; if(*(( int*)
_temp3002) ==  Cyc_Absyn_KnownTunion){ _LL3005: _temp3004=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp3002)->f1; goto _LL2997;} else{ goto _LL2998;}} else{ goto _LL2998;}
_LL2998: goto _LL2999; _LL2997: problem= Cyc_Absyn_qvar_cmp( _temp3004->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL2995; _LL2999: problem= 1; goto
_LL2995; _LL2995:;} goto _LL2989; _LL2993:{ void* _temp3006= _temp2987; struct
Cyc_Absyn_TunionInfo _temp3012; void* _temp3014; struct Cyc_Absyn_Tuniondecl*
_temp3016; _LL3008: if(( unsigned int) _temp3006 >  4u?*(( int*) _temp3006) == 
Cyc_Absyn_TunionType: 0){ _LL3013: _temp3012=(( struct Cyc_Absyn_TunionType_struct*)
_temp3006)->f1; _LL3015: _temp3014=( void*) _temp3012.tunion_info; if(*(( int*)
_temp3014) ==  Cyc_Absyn_KnownTunion){ _LL3017: _temp3016=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp3014)->f1; goto _LL3009;} else{ goto _LL3010;}} else{ goto _LL3010;}
_LL3010: goto _LL3011; _LL3009: problem= Cyc_Absyn_qvar_cmp( _temp3016->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL3007; _LL3011: problem= 1; goto
_LL3007; _LL3007:;} goto _LL2989; _LL2989:;} if( problem){({ void* _temp3018[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3018, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2709; for( 0; rpo !=  0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp3021; void* _temp3022; void*
_temp3024; struct _tuple8* _temp3019=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp3021=* _temp3019; _LL3025: _temp3024= _temp3021.f1;
goto _LL3023; _LL3023: _temp3022= _temp3021.f2; goto _LL3020; _LL3020: _temp2927=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2927,( void*) Cyc_Absyn_RgnKind,
_temp3024); _temp2927= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2927,( void*)
Cyc_Absyn_RgnKind, _temp3022);}} if(* _temp2721 !=  0){ _temp2927= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2927,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2721))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2927.free_vars; for(
0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void*
_temp3026=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3034; void*
_temp3036; _LL3028: if(( unsigned int) _temp3026 >  1u?*(( int*) _temp3026) == 
Cyc_Absyn_Eq_constr: 0){ _LL3035: _temp3034=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3026)->f1; if( _temp3034 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3029;}
else{ goto _LL3030;}} else{ goto _LL3030;} _LL3030: if(( unsigned int) _temp3026
>  1u?*(( int*) _temp3026) ==  Cyc_Absyn_Eq_constr: 0){ _LL3037: _temp3036=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3026)->f1; if( _temp3036 == (
void*) Cyc_Absyn_EffKind){ goto _LL3031;} else{ goto _LL3032;}} else{ goto
_LL3032;} _LL3032: goto _LL3033; _LL3029: effect=({ struct Cyc_List_List*
_temp3038=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3038->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3039=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3039[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3040; _temp3040.tag= Cyc_Absyn_AccessEff;
_temp3040.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3041=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3041[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3042; _temp3042.tag= Cyc_Absyn_VarType;
_temp3042.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3042;}); _temp3041;})); _temp3040;}); _temp3039;})); _temp3038->tl= effect;
_temp3038;}); goto _LL3027; _LL3031: effect=({ struct Cyc_List_List* _temp3043=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3043->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3044=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3044[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3045; _temp3045.tag= Cyc_Absyn_VarType; _temp3045.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3045;}); _temp3044;})); _temp3043->tl= effect; _temp3043;}); goto _LL3027;
_LL3033: effect=({ struct Cyc_List_List* _temp3046=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3046->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3047=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3047[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3048; _temp3048.tag= Cyc_Absyn_RgnsEff; _temp3048.f1=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3049=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3049[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3050; _temp3050.tag= Cyc_Absyn_VarType; _temp3050.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3050;}); _temp3049;})); _temp3048;}); _temp3047;})); _temp3046->tl= effect;
_temp3046;}); goto _LL3027; _LL3027:;}} effect= Cyc_List_imp_rev( effect);{
struct Cyc_List_List* ts= _temp2927.free_evars; for( 0; ts !=  0; ts=(( struct
Cyc_List_List*) _check_null( ts))->tl){ void* _temp3051= Cyc_Tcutil_typ_kind((
void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _LL3053: if( _temp3051
== ( void*) Cyc_Absyn_RgnKind){ goto _LL3054;} else{ goto _LL3055;} _LL3055: if(
_temp3051 == ( void*) Cyc_Absyn_EffKind){ goto _LL3056;} else{ goto _LL3057;}
_LL3057: goto _LL3058; _LL3054: effect=({ struct Cyc_List_List* _temp3059=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3059->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3060=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3060[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3061; _temp3061.tag= Cyc_Absyn_AccessEff;
_temp3061.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3061;}); _temp3060;})); _temp3059->tl= effect; _temp3059;}); goto _LL3052;
_LL3056: effect=({ struct Cyc_List_List* _temp3062=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3062->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3062->tl= effect; _temp3062;});
goto _LL3052; _LL3058: effect=({ struct Cyc_List_List* _temp3063=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3063->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3064=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3064[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3065; _temp3065.tag= Cyc_Absyn_RgnsEff; _temp3065.f1=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _temp3065;});
_temp3064;})); _temp3063->tl= effect; _temp3063;}); goto _LL3052; _LL3052:;}}*
_temp2721=({ struct Cyc_Core_Opt* _temp3066=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3066->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3067=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3067[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3068; _temp3068.tag= Cyc_Absyn_JoinEff;
_temp3068.f1= Cyc_List_imp_rev( effect); _temp3068;}); _temp3067;})); _temp3066;});}}
if(* _temp2724 !=  0){ struct Cyc_List_List* bs=* _temp2724; for( 0; bs !=  0;
bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3069= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3070=( void*) _temp3069->v; void*
_temp3078; _LL3072: if( _temp3070 == ( void*) Cyc_Absyn_No_constr){ goto _LL3073;}
else{ goto _LL3074;} _LL3074: if(( unsigned int) _temp3070 >  1u?*(( int*)
_temp3070) ==  Cyc_Absyn_Eq_constr: 0){ _LL3079: _temp3078=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3070)->f1; if( _temp3078 == ( void*) Cyc_Absyn_MemKind){ goto _LL3075;}
else{ goto _LL3076;}} else{ goto _LL3076;} _LL3076: goto _LL3077; _LL3073:({
struct Cyc_Std_String_pa_struct _temp3081; _temp3081.tag= Cyc_Std_String_pa;
_temp3081.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp3080[ 1u]={& _temp3081}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3080, sizeof( void*), 1u));}}); goto
_LL3075; _LL3075:( void*)( _temp3069->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3082=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3082[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3083; _temp3083.tag= Cyc_Absyn_Eq_constr;
_temp3083.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3083;}); _temp3082;})));
goto _LL3071; _LL3077: goto _LL3071; _LL3071:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2927.kind_env,* _temp2724); _temp2927.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2927.free_vars,* _temp2724);{ struct Cyc_List_List* tvs= _temp2927.free_vars;
for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2927.free_evars; for( 0; evs !=  0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2611;}}
_LL2637: for( 0; _temp2725 !=  0; _temp2725=(( struct Cyc_List_List*)
_check_null( _temp2725))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp2725))->hd)).f2);} goto _LL2611; _LL2639:{ struct
_RegionHandle _temp3085= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp3085; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2727 !=  0; _temp2727=(( struct Cyc_List_List*) _check_null( _temp2727))->tl){
struct Cyc_Absyn_Structfield _temp3088; struct Cyc_List_List* _temp3089; struct
Cyc_Absyn_Exp* _temp3091; void* _temp3093; struct Cyc_Absyn_Tqual _temp3095;
struct _tagged_arr* _temp3097; struct Cyc_Absyn_Structfield* _temp3086=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2727))->hd;
_temp3088=* _temp3086; _LL3098: _temp3097= _temp3088.name; goto _LL3096; _LL3096:
_temp3095= _temp3088.tq; goto _LL3094; _LL3094: _temp3093=( void*) _temp3088.type;
goto _LL3092; _LL3092: _temp3091= _temp3088.width; goto _LL3090; _LL3090:
_temp3089= _temp3088.attributes; goto _LL3087; _LL3087: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3097)){({ struct Cyc_Std_String_pa_struct _temp3100; _temp3100.tag= Cyc_Std_String_pa;
_temp3100.f1=( struct _tagged_arr)* _temp3097;{ void* _temp3099[ 1u]={&
_temp3100}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3099, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3097, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3101=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3101->hd=( void*)
_temp3097; _temp3101->tl= prev_fields; _temp3101;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3093); Cyc_Tcutil_check_bitfield(
loc, te, _temp3093, _temp3091, _temp3097); Cyc_Tcutil_check_field_atts( loc,
_temp3097, _temp3089);}}; _pop_region( sprev_rgn);} goto _LL2611; _LL2641:{
struct _RegionHandle _temp3102= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3102; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2729 !=  0; _temp2729=(( struct Cyc_List_List*) _check_null( _temp2729))->tl){
struct Cyc_Absyn_Structfield _temp3105; struct Cyc_List_List* _temp3106; struct
Cyc_Absyn_Exp* _temp3108; void* _temp3110; struct Cyc_Absyn_Tqual _temp3112;
struct _tagged_arr* _temp3114; struct Cyc_Absyn_Structfield* _temp3103=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2729))->hd;
_temp3105=* _temp3103; _LL3115: _temp3114= _temp3105.name; goto _LL3113; _LL3113:
_temp3112= _temp3105.tq; goto _LL3111; _LL3111: _temp3110=( void*) _temp3105.type;
goto _LL3109; _LL3109: _temp3108= _temp3105.width; goto _LL3107; _LL3107:
_temp3106= _temp3105.attributes; goto _LL3104; _LL3104: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3114)){({ struct Cyc_Std_String_pa_struct _temp3117; _temp3117.tag= Cyc_Std_String_pa;
_temp3117.f1=( struct _tagged_arr)* _temp3114;{ void* _temp3116[ 1u]={&
_temp3117}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3116, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3114, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3118=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3118->hd=( void*)
_temp3114; _temp3118->tl= prev_fields; _temp3118;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3110); if( ! Cyc_Tcutil_bits_only(
_temp3110)){({ struct Cyc_Std_String_pa_struct _temp3120; _temp3120.tag= Cyc_Std_String_pa;
_temp3120.f1=( struct _tagged_arr)* _temp3114;{ void* _temp3119[ 1u]={&
_temp3120}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3119, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3110, _temp3108, _temp3114); Cyc_Tcutil_check_field_atts(
loc, _temp3114, _temp3106);}}; _pop_region( uprev_rgn);} goto _LL2611; _LL2643:
if( _temp2736 ==  0){ if(* _temp2733 ==  0){({ void* _temp3121[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3121, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2733));;}} if(*
_temp2733 ==  0){ struct _handler_cons _temp3122; _push_handler(& _temp3122);{
int _temp3124= 0; if( setjmp( _temp3122.handler)){ _temp3124= 1;} if( !
_temp3124){* _temp2733=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2736));; _pop_handler();} else{
void* _temp3123=( void*) _exn_thrown; void* _temp3126= _temp3123; _LL3128: if(
_temp3126 ==  Cyc_Dict_Absent){ goto _LL3129;} else{ goto _LL3130;} _LL3130:
goto _LL3131; _LL3129:({ struct Cyc_Std_String_pa_struct _temp3133; _temp3133.tag=
Cyc_Std_String_pa; _temp3133.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2736));{ void* _temp3132[ 1u]={& _temp3133};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type struct %s", sizeof( unsigned char),
23u), _tag_arr( _temp3132, sizeof( void*), 1u));}}); return cvtenv; _LL3131:(
void) _throw( _temp3126); _LL3127:;}}}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2733)); if( sd->name !=  0){*((
struct _tuple1*) _check_null( _temp2736))=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2734); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3137; _temp3137.tag= Cyc_Std_Int_pa; _temp3137.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3136; _temp3136.tag= Cyc_Std_Int_pa;
_temp3136.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3135; _temp3135.tag= Cyc_Std_String_pa; _temp3135.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2736));{ void*
_temp3134[ 3u]={& _temp3135,& _temp3136,& _temp3137}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3134, sizeof( void*), 3u));}}}});} for( 0;
_temp2734 !=  0;( _temp2734=(( struct Cyc_List_List*) _check_null( _temp2734))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2734))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2611;}}
_LL2645: if( _temp2743 ==  0){({ void* _temp3138[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp3138,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2740 ==  0){ struct
_handler_cons _temp3139; _push_handler(& _temp3139);{ int _temp3141= 0; if(
setjmp( _temp3139.handler)){ _temp3141= 1;} if( ! _temp3141){* _temp2740=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2743));; _pop_handler();} else{ void* _temp3140=(
void*) _exn_thrown; void* _temp3143= _temp3140; _LL3145: if( _temp3143 ==  Cyc_Dict_Absent){
goto _LL3146;} else{ goto _LL3147;} _LL3147: goto _LL3148; _LL3146:({ struct Cyc_Std_String_pa_struct
_temp3150; _temp3150.tag= Cyc_Std_String_pa; _temp3150.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2743));{ void*
_temp3149[ 1u]={& _temp3150}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type union %s",
sizeof( unsigned char), 22u), _tag_arr( _temp3149, sizeof( void*), 1u));}});
return cvtenv; _LL3148:( void) _throw( _temp3143); _LL3144:;}}}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(* _temp2740)); if( ud->name != 
0){*(( struct _tuple1*) _check_null( _temp2743))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs= Cyc_List_length( _temp2741); if( lvs != 
largs){({ struct Cyc_Std_Int_pa_struct _temp3154; _temp3154.tag= Cyc_Std_Int_pa;
_temp3154.f1=( int)(( unsigned int) largs);{ struct Cyc_Std_Int_pa_struct
_temp3153; _temp3153.tag= Cyc_Std_Int_pa; _temp3153.f1=( int)(( unsigned int)
lvs);{ struct Cyc_Std_String_pa_struct _temp3152; _temp3152.tag= Cyc_Std_String_pa;
_temp3152.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2743));{ void* _temp3151[ 3u]={& _temp3152,& _temp3153,&
_temp3154}; Cyc_Tcutil_terr( loc, _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u), _tag_arr( _temp3151, sizeof( void*), 3u));}}}});}
for( 0; _temp2741 !=  0;( _temp2741=(( struct Cyc_List_List*) _check_null(
_temp2741))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2741))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2611;}}
_LL2647: { struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons _temp3155;
_push_handler(& _temp3155);{ int _temp3157= 0; if( setjmp( _temp3155.handler)){
_temp3157= 1;} if( ! _temp3157){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp2750);; _pop_handler();} else{ void* _temp3156=( void*) _exn_thrown; void*
_temp3159= _temp3156; _LL3161: if( _temp3159 ==  Cyc_Dict_Absent){ goto _LL3162;}
else{ goto _LL3163;} _LL3163: goto _LL3164; _LL3162:({ struct Cyc_Std_String_pa_struct
_temp3166; _temp3166.tag= Cyc_Std_String_pa; _temp3166.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2750);{ void* _temp3165[ 1u]={& _temp3166}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3165, sizeof( void*), 1u));}}); return cvtenv; _LL3164:( void) _throw(
_temp3159); _LL3160:;}}} _temp2750[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2748; struct Cyc_List_List* inst= 0; for( 0; ts
!=  0? tvs !=  0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3167=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3167->hd=( void*)({ struct
_tuple5* _temp3168=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3168->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3168->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3168;}); _temp3167->tl= inst; _temp3167;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp3170; _temp3170.tag= Cyc_Std_String_pa; _temp3170.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2750);{ void* _temp3169[ 1u]={& _temp3170}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3169, sizeof( void*), 1u));}});} if( tvs !=  0){ for( 0; tvs != 
0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
void* e= Cyc_Absyn_new_evar( 0, 0); cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv, k, e); inst=({ struct Cyc_List_List* _temp3171=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3171->hd=( void*)({ struct
_tuple5* _temp3172=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3172->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3172->f2= e; _temp3172;}); _temp3171->tl= inst; _temp3171;});}}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2747=({
struct Cyc_Core_Opt* _temp3173=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3173->v=( void*) new_typ; _temp3173;}); goto _LL2611;}}}
_LL2649: goto _LL2611; _LL2651: _temp2754= _temp2752; goto _LL2653; _LL2653:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2754); goto _LL2611; _LL2655: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2756); goto _LL2611; _LL2657: for( 0;
_temp2758 !=  0; _temp2758=(( struct Cyc_List_List*) _check_null( _temp2758))->tl){
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_List_List*) _check_null( _temp2758))->hd);} goto _LL2611;
_LL2611:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){({
struct Cyc_Std_String_pa_struct _temp3177; _temp3177.tag= Cyc_Std_String_pa;
_temp3177.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_Std_String_pa_struct _temp3176; _temp3176.tag= Cyc_Std_String_pa;
_temp3176.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_Std_String_pa_struct _temp3175; _temp3175.tag= Cyc_Std_String_pa;
_temp3175.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3174[
3u]={& _temp3175,& _temp3176,& _temp3177}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3174, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3178= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3193; _temp3193.kind_env= kind_env; _temp3193.free_vars= 0; _temp3193.free_evars=
0; _temp3193.generalize_evars= generalize_evars; _temp3193.fn_result= 0;
_temp3193;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3178.free_vars;
for( 0; vs !=  0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ _temp3178.kind_env=
Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( vs))->hd);}}{ struct Cyc_List_List* evs= _temp3178.free_evars; for(
0; evs !=  0; evs=(( struct Cyc_List_List*) _check_null( evs))->tl){ void*
_temp3179= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null(
evs))->hd); struct Cyc_Core_Opt* _temp3185; struct Cyc_Core_Opt** _temp3187;
_LL3181: if(( unsigned int) _temp3179 >  4u?*(( int*) _temp3179) ==  Cyc_Absyn_Evar:
0){ _LL3186: _temp3185=(( struct Cyc_Absyn_Evar_struct*) _temp3179)->f4;
_temp3187=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3179)->f4;
goto _LL3182;} else{ goto _LL3183;} _LL3183: goto _LL3184; _LL3182: if(*
_temp3187 ==  0){* _temp3187=({ struct Cyc_Core_Opt* _temp3188=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3188->v=( void*) kind_env;
_temp3188;});} else{ struct Cyc_List_List* _temp3189=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3187))->v; struct Cyc_List_List*
_temp3190= 0; for( 0; _temp3189 !=  0; _temp3189=(( struct Cyc_List_List*)
_check_null( _temp3189))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3189))->hd)){ _temp3190=({ struct Cyc_List_List* _temp3191=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3191->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3189))->hd);
_temp3191->tl= _temp3190; _temp3191;});}}* _temp3187=({ struct Cyc_Core_Opt*
_temp3192=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3192->v=( void*) _temp3190; _temp3192;});} goto _LL3180; _LL3184: goto
_LL3180; _LL3180:;}} return _temp3178;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ int
generalize_evars= Cyc_Tcutil_is_function_type( t); struct Cyc_Tcutil_CVTEnv
_temp3194= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
generalize_evars, t); struct Cyc_List_List* _temp3195= _temp3194.free_vars;
struct Cyc_List_List* _temp3196= _temp3194.free_evars;{ struct Cyc_List_List* x=
_temp3195; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3197=( void*) c->v;
void* _temp3205; _LL3199: if( _temp3197 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3200;} else{ goto _LL3201;} _LL3201: if(( unsigned int) _temp3197 >  1u?*((
int*) _temp3197) ==  Cyc_Absyn_Eq_constr: 0){ _LL3206: _temp3205=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3197)->f1; if( _temp3205 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3202;} else{ goto _LL3203;}} else{ goto _LL3203;}
_LL3203: goto _LL3204; _LL3200: goto _LL3202; _LL3202:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3207=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3207[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3208; _temp3208.tag= Cyc_Absyn_Eq_constr;
_temp3208.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3208;}); _temp3207;})));
goto _LL3198; _LL3204: goto _LL3198; _LL3198:;}} if( _temp3195 !=  0? 1:
_temp3196 !=  0){{ void* _temp3209= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3215; struct Cyc_List_List* _temp3217; struct Cyc_List_List* _temp3219;
struct Cyc_Absyn_VarargInfo* _temp3221; int _temp3223; struct Cyc_List_List*
_temp3225; void* _temp3227; struct Cyc_Core_Opt* _temp3229; struct Cyc_List_List*
_temp3231; struct Cyc_List_List** _temp3233; _LL3211: if(( unsigned int)
_temp3209 >  4u?*(( int*) _temp3209) ==  Cyc_Absyn_FnType: 0){ _LL3216:
_temp3215=(( struct Cyc_Absyn_FnType_struct*) _temp3209)->f1; _LL3232: _temp3231=
_temp3215.tvars; _temp3233=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3209)->f1).tvars; goto _LL3230; _LL3230: _temp3229= _temp3215.effect; goto
_LL3228; _LL3228: _temp3227=( void*) _temp3215.ret_typ; goto _LL3226; _LL3226:
_temp3225= _temp3215.args; goto _LL3224; _LL3224: _temp3223= _temp3215.c_varargs;
goto _LL3222; _LL3222: _temp3221= _temp3215.cyc_varargs; goto _LL3220; _LL3220:
_temp3219= _temp3215.rgn_po; goto _LL3218; _LL3218: _temp3217= _temp3215.attributes;
goto _LL3212;} else{ goto _LL3213;} _LL3213: goto _LL3214; _LL3212: if(*
_temp3233 ==  0){* _temp3233= _temp3195; _temp3195= 0;} goto _LL3210; _LL3214:
goto _LL3210; _LL3210:;} if( _temp3195 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3235; _temp3235.tag= Cyc_Std_String_pa; _temp3235.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3195))->hd)->name;{
void* _temp3234[ 1u]={& _temp3235}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp3234, sizeof( void*), 1u));}});}
if( _temp3196 !=  0){ for( 0; _temp3196 !=  0; _temp3196=(( struct Cyc_List_List*)
_check_null( _temp3196))->tl){ void* e=( void*)(( struct Cyc_List_List*)
_check_null( _temp3196))->hd; void* _temp3236= Cyc_Tcutil_typ_kind( e); _LL3238:
if( _temp3236 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3239;} else{ goto _LL3240;}
_LL3240: if( _temp3236 == ( void*) Cyc_Absyn_EffKind){ goto _LL3241;} else{ goto
_LL3242;} _LL3242: goto _LL3243; _LL3239: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp3244[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp3244, sizeof( void*), 0u));});} goto _LL3237; _LL3241: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp3245[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp3245, sizeof( void*), 0u));});} goto _LL3237; _LL3243:({ struct Cyc_Std_String_pa_struct
_temp3248; _temp3248.tag= Cyc_Std_String_pa; _temp3248.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3247;
_temp3247.tag= Cyc_Std_String_pa; _temp3247.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp3246[ 2u]={& _temp3247,& _temp3248}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3246, sizeof( void*), 2u));}}}); goto
_LL3237; _LL3237:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3249= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3255; struct Cyc_List_List* _temp3257; struct
Cyc_List_List* _temp3259; struct Cyc_Absyn_VarargInfo* _temp3261; int _temp3263;
struct Cyc_List_List* _temp3265; void* _temp3267; struct Cyc_Core_Opt* _temp3269;
struct Cyc_List_List* _temp3271; _LL3251: if(( unsigned int) _temp3249 >  4u?*((
int*) _temp3249) ==  Cyc_Absyn_FnType: 0){ _LL3256: _temp3255=(( struct Cyc_Absyn_FnType_struct*)
_temp3249)->f1; _LL3272: _temp3271= _temp3255.tvars; goto _LL3270; _LL3270:
_temp3269= _temp3255.effect; goto _LL3268; _LL3268: _temp3267=( void*) _temp3255.ret_typ;
goto _LL3266; _LL3266: _temp3265= _temp3255.args; goto _LL3264; _LL3264:
_temp3263= _temp3255.c_varargs; goto _LL3262; _LL3262: _temp3261= _temp3255.cyc_varargs;
goto _LL3260; _LL3260: _temp3259= _temp3255.rgn_po; goto _LL3258; _LL3258:
_temp3257= _temp3255.attributes; goto _LL3252;} else{ goto _LL3253;} _LL3253:
goto _LL3254; _LL3252: fd->tvs= _temp3271; fd->effect= _temp3269; goto _LL3250;
_LL3254:({ void* _temp3273[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3273, sizeof( void*), 0u));});
return; _LL3250:;}{ struct _RegionHandle _temp3274= _new_region(); struct
_RegionHandle* r=& _temp3274; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3275=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3275->v=(
void*) t; _temp3275;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3276=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3277= Cyc_Tcutil_remove_bound_tvars( _temp3276.free_vars,
bound_tvars); struct Cyc_List_List* _temp3278= _temp3276.free_evars;{ struct Cyc_List_List*
fs= _temp3277; for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3279=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name;({ struct Cyc_Std_String_pa_struct _temp3282;
_temp3282.tag= Cyc_Std_String_pa; _temp3282.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp3281; _temp3281.tag= Cyc_Std_String_pa;
_temp3281.f1=( struct _tagged_arr)* _temp3279;{ void* _temp3280[ 2u]={&
_temp3281,& _temp3282}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3280, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3278 !=  0: 0){ for( 0; _temp3278 !=  0; _temp3278=((
struct Cyc_List_List*) _check_null( _temp3278))->tl){ void* e=( void*)(( struct
Cyc_List_List*) _check_null( _temp3278))->hd; void* _temp3283= Cyc_Tcutil_typ_kind(
e); _LL3285: if( _temp3283 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3286;} else{
goto _LL3287;} _LL3287: if( _temp3283 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3288;} else{ goto _LL3289;} _LL3289: goto _LL3290; _LL3286: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3291[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3291, sizeof( void*), 0u));});}
goto _LL3284; _LL3288: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3292=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3292[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3293; _temp3293.tag= Cyc_Absyn_JoinEff;
_temp3293.f1= 0; _temp3293;}); _temp3292;}))){({ void* _temp3294[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3294, sizeof( void*), 0u));});}
goto _LL3284; _LL3290:({ struct Cyc_Std_String_pa_struct _temp3297; _temp3297.tag=
Cyc_Std_String_pa; _temp3297.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3296; _temp3296.tag= Cyc_Std_String_pa;
_temp3296.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3295[
2u]={& _temp3296,& _temp3297}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3295, sizeof( void*), 2u));}}});
goto _LL3284; _LL3284:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
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
struct Cyc_Std_String_pa_struct _temp3300; _temp3300.tag= Cyc_Std_String_pa;
_temp3300.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_Std_String_pa_struct _temp3299; _temp3299.tag=
Cyc_Std_String_pa; _temp3299.f1=( struct _tagged_arr) msg;{ void* _temp3298[ 2u]={&
_temp3299,& _temp3300}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3298, sizeof( void*), 2u));}}});}}}} static
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
unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List* _temp3301=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3301->hd=( void*)({
struct _tuple17* _temp3302=( struct _tuple17*) _cycalloc( sizeof( struct
_tuple17)); _temp3302->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3302->f2= 0; _temp3302;}); _temp3301->tl=
fields; _temp3301;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3305;
void* _temp3306; struct Cyc_List_List* _temp3308; struct _tuple18* _temp3303=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3305=*
_temp3303; _LL3309: _temp3308= _temp3305.f1; goto _LL3307; _LL3307: _temp3306=
_temp3305.f2; goto _LL3304; _LL3304: if( _temp3308 ==  0){ struct Cyc_List_List*
_temp3310= fields; for( 0; _temp3310 !=  0; _temp3310=(( struct Cyc_List_List*)
_check_null( _temp3310))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3310))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3310))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3311=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3311->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3312=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3312[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3313; _temp3313.tag= Cyc_Absyn_FieldName;
_temp3313.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3310))->hd)).f1)->name; _temp3313;}); _temp3312;})); _temp3311->tl= 0;
_temp3311;}); ans=({ struct Cyc_List_List* _temp3314=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3314->hd=( void*)({
struct _tuple19* _temp3315=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3315->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3310))->hd)).f1; _temp3315->f2= _temp3306; _temp3315;});
_temp3314->tl= ans; _temp3314;}); break;}} if( _temp3310 ==  0){({ void*
_temp3316[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3316, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3308))->tl !=  0){({ void*
_temp3317[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3317, sizeof( void*), 0u));});}
else{ void* _temp3318=( void*)(( struct Cyc_List_List*) _check_null( _temp3308))->hd;
struct _tagged_arr* _temp3324; _LL3320: if(*(( int*) _temp3318) ==  Cyc_Absyn_ArrayElement){
goto _LL3321;} else{ goto _LL3322;} _LL3322: if(*(( int*) _temp3318) ==  Cyc_Absyn_FieldName){
_LL3325: _temp3324=(( struct Cyc_Absyn_FieldName_struct*) _temp3318)->f1; goto
_LL3323;} else{ goto _LL3319;} _LL3321:({ void* _temp3326[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3326, sizeof( void*), 0u));}); goto _LL3319;
_LL3323: { struct Cyc_List_List* _temp3327= fields; for( 0; _temp3327 !=  0;
_temp3327=(( struct Cyc_List_List*) _check_null( _temp3327))->tl){ if( Cyc_Std_zstrptrcmp(
_temp3324,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3327))->hd)).f1)->name) ==  0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3327))->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3329;
_temp3329.tag= Cyc_Std_String_pa; _temp3329.f1=( struct _tagged_arr)* _temp3324;{
void* _temp3328[ 1u]={& _temp3329}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3328, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3327))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3330=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3330->hd=( void*)({ struct _tuple19*
_temp3331=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3331->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3327))->hd)).f1; _temp3331->f2= _temp3306; _temp3331;}); _temp3330->tl= ans;
_temp3330;}); break;}} if( _temp3327 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3333; _temp3333.tag= Cyc_Std_String_pa; _temp3333.f1=( struct _tagged_arr)*
_temp3324;{ void* _temp3332[ 1u]={& _temp3333}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3332, sizeof( void*), 1u));}});}
goto _LL3319;} _LL3319:;}}} for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3334[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3334, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3335= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3341; struct Cyc_Absyn_Conref* _temp3343; void* _temp3345;
_LL3337: if(( unsigned int) _temp3335 >  4u?*(( int*) _temp3335) ==  Cyc_Absyn_PointerType:
0){ _LL3342: _temp3341=(( struct Cyc_Absyn_PointerType_struct*) _temp3335)->f1;
_LL3346: _temp3345=( void*) _temp3341.elt_typ; goto _LL3344; _LL3344: _temp3343=
_temp3341.bounds; goto _LL3338;} else{ goto _LL3339;} _LL3339: goto _LL3340;
_LL3338: { struct Cyc_Absyn_Conref* _temp3347= Cyc_Absyn_compress_conref(
_temp3343); void* _temp3348=( void*)( Cyc_Absyn_compress_conref( _temp3347))->v;
void* _temp3356; _LL3350: if(( unsigned int) _temp3348 >  1u?*(( int*) _temp3348)
==  Cyc_Absyn_Eq_constr: 0){ _LL3357: _temp3356=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3348)->f1; if( _temp3356 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3351;}
else{ goto _LL3352;}} else{ goto _LL3352;} _LL3352: if( _temp3348 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3353;} else{ goto _LL3354;} _LL3354: goto _LL3355;
_LL3351:* elt_typ_dest= _temp3345; return 1; _LL3353:( void*)( _temp3347->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3358=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3358[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3359; _temp3359.tag= Cyc_Absyn_Eq_constr;
_temp3359.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3359;}); _temp3358;})));*
elt_typ_dest= _temp3345; return 1; _LL3355: return 0; _LL3349:;} _LL3340: return
0; _LL3336:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3542;
_temp3542.f1= 0; _temp3542.f2=( void*) Cyc_Absyn_HeapRgn; _temp3542;}); void*
_temp3360=( void*) e->r; void* _temp3374; struct _tuple1* _temp3376; struct
_tagged_arr* _temp3378; struct Cyc_Absyn_Exp* _temp3380; struct _tagged_arr*
_temp3382; struct Cyc_Absyn_Exp* _temp3384; struct Cyc_Absyn_Exp* _temp3386;
struct Cyc_Absyn_Exp* _temp3388; struct Cyc_Absyn_Exp* _temp3390; _LL3362: if(*((
int*) _temp3360) ==  Cyc_Absyn_Var_e){ _LL3377: _temp3376=(( struct Cyc_Absyn_Var_e_struct*)
_temp3360)->f1; goto _LL3375; _LL3375: _temp3374=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3360)->f2; goto _LL3363;} else{ goto _LL3364;} _LL3364: if(*(( int*)
_temp3360) ==  Cyc_Absyn_StructMember_e){ _LL3381: _temp3380=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3360)->f1; goto _LL3379; _LL3379: _temp3378=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3360)->f2; goto _LL3365;} else{ goto _LL3366;} _LL3366: if(*(( int*)
_temp3360) ==  Cyc_Absyn_StructArrow_e){ _LL3385: _temp3384=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3360)->f1; goto _LL3383; _LL3383: _temp3382=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3360)->f2; goto _LL3367;} else{ goto _LL3368;} _LL3368: if(*(( int*)
_temp3360) ==  Cyc_Absyn_Deref_e){ _LL3387: _temp3386=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3360)->f1; goto _LL3369;} else{ goto _LL3370;} _LL3370: if(*(( int*)
_temp3360) ==  Cyc_Absyn_Subscript_e){ _LL3391: _temp3390=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3360)->f1; goto _LL3389; _LL3389: _temp3388=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3360)->f2; goto _LL3371;} else{ goto _LL3372;} _LL3372: goto _LL3373;
_LL3363: { void* _temp3392= _temp3374; struct Cyc_Absyn_Vardecl* _temp3406;
struct Cyc_Absyn_Vardecl* _temp3408; struct Cyc_Absyn_Vardecl* _temp3410; struct
Cyc_Absyn_Vardecl* _temp3412; _LL3394: if( _temp3392 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3395;} else{ goto _LL3396;} _LL3396: if(( unsigned int) _temp3392 >  1u?*((
int*) _temp3392) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3397;} else{ goto _LL3398;}
_LL3398: if(( unsigned int) _temp3392 >  1u?*(( int*) _temp3392) ==  Cyc_Absyn_Global_b:
0){ _LL3407: _temp3406=(( struct Cyc_Absyn_Global_b_struct*) _temp3392)->f1;
goto _LL3399;} else{ goto _LL3400;} _LL3400: if(( unsigned int) _temp3392 >  1u?*((
int*) _temp3392) ==  Cyc_Absyn_Local_b: 0){ _LL3409: _temp3408=(( struct Cyc_Absyn_Local_b_struct*)
_temp3392)->f1; goto _LL3401;} else{ goto _LL3402;} _LL3402: if(( unsigned int)
_temp3392 >  1u?*(( int*) _temp3392) ==  Cyc_Absyn_Pat_b: 0){ _LL3411: _temp3410=((
struct Cyc_Absyn_Pat_b_struct*) _temp3392)->f1; goto _LL3403;} else{ goto
_LL3404;} _LL3404: if(( unsigned int) _temp3392 >  1u?*(( int*) _temp3392) == 
Cyc_Absyn_Param_b: 0){ _LL3413: _temp3412=(( struct Cyc_Absyn_Param_b_struct*)
_temp3392)->f1; goto _LL3405;} else{ goto _LL3393;} _LL3395: return bogus_ans;
_LL3397: return({ struct _tuple7 _temp3414; _temp3414.f1= 0; _temp3414.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3414;}); _LL3399: { void* _temp3415= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3417: if((
unsigned int) _temp3415 >  4u?*(( int*) _temp3415) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3418;} else{ goto _LL3419;} _LL3419: goto _LL3420; _LL3418: return({
struct _tuple7 _temp3421; _temp3421.f1= 1; _temp3421.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3421;}); _LL3420: return({ struct _tuple7 _temp3422; _temp3422.f1=(
_temp3406->tq).q_const; _temp3422.f2=( void*) Cyc_Absyn_HeapRgn; _temp3422;});
_LL3416:;} _LL3401: { void* _temp3423= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3425: if(( unsigned int) _temp3423 >  4u?*(( int*)
_temp3423) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3426;} else{ goto _LL3427;}
_LL3427: goto _LL3428; _LL3426: return({ struct _tuple7 _temp3429; _temp3429.f1=
1; _temp3429.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3408->rgn))->v;
_temp3429;}); _LL3428: return({ struct _tuple7 _temp3430; _temp3430.f1=(
_temp3408->tq).q_const; _temp3430.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3408->rgn))->v; _temp3430;}); _LL3424:;} _LL3403: _temp3412=
_temp3410; goto _LL3405; _LL3405: return({ struct _tuple7 _temp3431; _temp3431.f1=(
_temp3412->tq).q_const; _temp3431.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3412->rgn))->v; _temp3431;}); _LL3393:;} _LL3365: { void*
_temp3432= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3380->topt))->v); struct Cyc_List_List* _temp3444; struct Cyc_List_List*
_temp3446; struct Cyc_Absyn_Structdecl** _temp3448; struct Cyc_Absyn_Structdecl*
_temp3450; struct Cyc_Absyn_Uniondecl** _temp3451; struct Cyc_Absyn_Uniondecl*
_temp3453; _LL3434: if(( unsigned int) _temp3432 >  4u?*(( int*) _temp3432) == 
Cyc_Absyn_AnonStructType: 0){ _LL3445: _temp3444=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3432)->f1; goto _LL3435;} else{ goto _LL3436;} _LL3436: if(( unsigned int)
_temp3432 >  4u?*(( int*) _temp3432) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3447:
_temp3446=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3432)->f1; goto
_LL3437;} else{ goto _LL3438;} _LL3438: if(( unsigned int) _temp3432 >  4u?*((
int*) _temp3432) ==  Cyc_Absyn_StructType: 0){ _LL3449: _temp3448=(( struct Cyc_Absyn_StructType_struct*)
_temp3432)->f3; if( _temp3448 ==  0){ goto _LL3440;} else{ _temp3450=* _temp3448;
goto _LL3439;}} else{ goto _LL3440;} _LL3440: if(( unsigned int) _temp3432 >  4u?*((
int*) _temp3432) ==  Cyc_Absyn_UnionType: 0){ _LL3452: _temp3451=(( struct Cyc_Absyn_UnionType_struct*)
_temp3432)->f3; if( _temp3451 ==  0){ goto _LL3442;} else{ _temp3453=* _temp3451;
goto _LL3441;}} else{ goto _LL3442;} _LL3442: goto _LL3443; _LL3435: _temp3446=
_temp3444; goto _LL3437; _LL3437: { struct Cyc_Absyn_Structfield* _temp3454= Cyc_Absyn_lookup_field(
_temp3446, _temp3378); if( _temp3454 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3454))->width !=  0: 0){ return({ struct _tuple7 _temp3455;
_temp3455.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3454))->tq).q_const;
_temp3455.f2=( Cyc_Tcutil_addressof_props( te, _temp3380)).f2; _temp3455;});}
return bogus_ans;} _LL3439: { struct Cyc_Absyn_Structfield* _temp3456= Cyc_Absyn_lookup_struct_field(
_temp3450, _temp3378); if( _temp3456 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3456))->width !=  0: 0){ return({ struct _tuple7 _temp3457;
_temp3457.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3456))->tq).q_const;
_temp3457.f2=( Cyc_Tcutil_addressof_props( te, _temp3380)).f2; _temp3457;});}
return bogus_ans;} _LL3441: { struct Cyc_Absyn_Structfield* _temp3458= Cyc_Absyn_lookup_union_field(
_temp3453, _temp3378); if( _temp3458 !=  0){ return({ struct _tuple7 _temp3459;
_temp3459.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3458))->tq).q_const;
_temp3459.f2=( Cyc_Tcutil_addressof_props( te, _temp3380)).f2; _temp3459;});}
goto _LL3443;} _LL3443: return bogus_ans; _LL3433:;} _LL3367: { void* _temp3460=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3384->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3466; void* _temp3468; void* _temp3470; _LL3462:
if(( unsigned int) _temp3460 >  4u?*(( int*) _temp3460) ==  Cyc_Absyn_PointerType:
0){ _LL3467: _temp3466=(( struct Cyc_Absyn_PointerType_struct*) _temp3460)->f1;
_LL3471: _temp3470=( void*) _temp3466.elt_typ; goto _LL3469; _LL3469: _temp3468=(
void*) _temp3466.rgn_typ; goto _LL3463;} else{ goto _LL3464;} _LL3464: goto
_LL3465; _LL3463: { void* _temp3472= Cyc_Tcutil_compress( _temp3470); struct Cyc_List_List*
_temp3484; struct Cyc_List_List* _temp3486; struct Cyc_Absyn_Structdecl**
_temp3488; struct Cyc_Absyn_Structdecl* _temp3490; struct Cyc_Absyn_Uniondecl**
_temp3491; struct Cyc_Absyn_Uniondecl* _temp3493; _LL3474: if(( unsigned int)
_temp3472 >  4u?*(( int*) _temp3472) ==  Cyc_Absyn_AnonStructType: 0){ _LL3485:
_temp3484=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3472)->f1; goto
_LL3475;} else{ goto _LL3476;} _LL3476: if(( unsigned int) _temp3472 >  4u?*((
int*) _temp3472) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3487: _temp3486=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3472)->f1; goto _LL3477;} else{ goto
_LL3478;} _LL3478: if(( unsigned int) _temp3472 >  4u?*(( int*) _temp3472) == 
Cyc_Absyn_StructType: 0){ _LL3489: _temp3488=(( struct Cyc_Absyn_StructType_struct*)
_temp3472)->f3; if( _temp3488 ==  0){ goto _LL3480;} else{ _temp3490=* _temp3488;
goto _LL3479;}} else{ goto _LL3480;} _LL3480: if(( unsigned int) _temp3472 >  4u?*((
int*) _temp3472) ==  Cyc_Absyn_UnionType: 0){ _LL3492: _temp3491=(( struct Cyc_Absyn_UnionType_struct*)
_temp3472)->f3; if( _temp3491 ==  0){ goto _LL3482;} else{ _temp3493=* _temp3491;
goto _LL3481;}} else{ goto _LL3482;} _LL3482: goto _LL3483; _LL3475: _temp3486=
_temp3484; goto _LL3477; _LL3477: { struct Cyc_Absyn_Structfield* _temp3494= Cyc_Absyn_lookup_field(
_temp3486, _temp3382); if( _temp3494 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3494))->width !=  0: 0){ return({ struct _tuple7 _temp3495;
_temp3495.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3494))->tq).q_const;
_temp3495.f2= _temp3468; _temp3495;});} return bogus_ans;} _LL3479: { struct Cyc_Absyn_Structfield*
_temp3496= Cyc_Absyn_lookup_struct_field( _temp3490, _temp3382); if( _temp3496
!=  0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3496))->width !=  0: 0){
return({ struct _tuple7 _temp3497; _temp3497.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3496))->tq).q_const; _temp3497.f2= _temp3468; _temp3497;});}
return bogus_ans;} _LL3481: { struct Cyc_Absyn_Structfield* _temp3498= Cyc_Absyn_lookup_union_field(
_temp3493, _temp3382); if( _temp3498 !=  0){ return({ struct _tuple7 _temp3499;
_temp3499.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3498))->tq).q_const;
_temp3499.f2= _temp3468; _temp3499;});} return bogus_ans;} _LL3483: return
bogus_ans; _LL3473:;} _LL3465: return bogus_ans; _LL3461:;} _LL3369: { void*
_temp3500= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3386->topt))->v); struct Cyc_Absyn_PtrInfo _temp3506; struct Cyc_Absyn_Tqual
_temp3508; void* _temp3510; _LL3502: if(( unsigned int) _temp3500 >  4u?*(( int*)
_temp3500) ==  Cyc_Absyn_PointerType: 0){ _LL3507: _temp3506=(( struct Cyc_Absyn_PointerType_struct*)
_temp3500)->f1; _LL3511: _temp3510=( void*) _temp3506.rgn_typ; goto _LL3509;
_LL3509: _temp3508= _temp3506.tq; goto _LL3503;} else{ goto _LL3504;} _LL3504:
goto _LL3505; _LL3503: return({ struct _tuple7 _temp3512; _temp3512.f1=
_temp3508.q_const; _temp3512.f2= _temp3510; _temp3512;}); _LL3505: return
bogus_ans; _LL3501:;} _LL3371: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3390->topt))->v); void* _temp3513= t; struct
Cyc_Absyn_Tqual _temp3523; struct Cyc_List_List* _temp3525; struct Cyc_Absyn_PtrInfo
_temp3527; struct Cyc_Absyn_Conref* _temp3529; struct Cyc_Absyn_Tqual _temp3531;
void* _temp3533; void* _temp3535; _LL3515: if(( unsigned int) _temp3513 >  4u?*((
int*) _temp3513) ==  Cyc_Absyn_ArrayType: 0){ _LL3524: _temp3523=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3513)->f2; goto _LL3516;} else{ goto _LL3517;} _LL3517: if(( unsigned int)
_temp3513 >  4u?*(( int*) _temp3513) ==  Cyc_Absyn_TupleType: 0){ _LL3526:
_temp3525=(( struct Cyc_Absyn_TupleType_struct*) _temp3513)->f1; goto _LL3518;}
else{ goto _LL3519;} _LL3519: if(( unsigned int) _temp3513 >  4u?*(( int*)
_temp3513) ==  Cyc_Absyn_PointerType: 0){ _LL3528: _temp3527=(( struct Cyc_Absyn_PointerType_struct*)
_temp3513)->f1; _LL3536: _temp3535=( void*) _temp3527.elt_typ; goto _LL3534;
_LL3534: _temp3533=( void*) _temp3527.rgn_typ; goto _LL3532; _LL3532: _temp3531=
_temp3527.tq; goto _LL3530; _LL3530: _temp3529= _temp3527.bounds; goto _LL3520;}
else{ goto _LL3521;} _LL3521: goto _LL3522; _LL3516: return({ struct _tuple7
_temp3537; _temp3537.f1= _temp3523.q_const; _temp3537.f2=( Cyc_Tcutil_addressof_props(
te, _temp3390)).f2; _temp3537;}); _LL3518: { struct _tuple4* _temp3538= Cyc_Absyn_lookup_tuple_field(
_temp3525,( int) Cyc_Evexp_eval_const_uint_exp( _temp3388)); if( _temp3538 !=  0){
return({ struct _tuple7 _temp3539; _temp3539.f1=((*(( struct _tuple4*)
_check_null( _temp3538))).f1).q_const; _temp3539.f2=( Cyc_Tcutil_addressof_props(
te, _temp3390)).f2; _temp3539;});} return bogus_ans;} _LL3520: return({ struct
_tuple7 _temp3540; _temp3540.f1= _temp3531.q_const; _temp3540.f2= _temp3533;
_temp3540;}); _LL3522: return bogus_ans; _LL3514:;} _LL3373:({ void* _temp3541[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3541, sizeof( void*), 0u));});
return bogus_ans; _LL3361:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3543= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3549; void* _temp3551; _LL3545: if((
unsigned int) _temp3543 >  4u?*(( int*) _temp3543) ==  Cyc_Absyn_ArrayType: 0){
_LL3552: _temp3551=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3543)->f1;
goto _LL3550; _LL3550: _temp3549=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3543)->f2; goto _LL3546;} else{ goto _LL3547;} _LL3547: goto _LL3548;
_LL3546: { void* _temp3555; struct _tuple7 _temp3553= Cyc_Tcutil_addressof_props(
te, e); _LL3556: _temp3555= _temp3553.f2; goto _LL3554; _LL3554: return Cyc_Absyn_atb_typ(
_temp3551, _temp3555, _temp3549,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3557=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3557[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3558; _temp3558.tag= Cyc_Absyn_Upper_b;
_temp3558.f1= e; _temp3558;}); _temp3557;}));} _LL3548: return e_typ; _LL3544:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3559=(
void*) b->v; void* _temp3567; void* _temp3569; struct Cyc_Absyn_Exp* _temp3571;
_LL3561: if(( unsigned int) _temp3559 >  1u?*(( int*) _temp3559) ==  Cyc_Absyn_Eq_constr:
0){ _LL3568: _temp3567=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3559)->f1;
if( _temp3567 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3562;} else{ goto
_LL3563;}} else{ goto _LL3563;} _LL3563: if(( unsigned int) _temp3559 >  1u?*((
int*) _temp3559) ==  Cyc_Absyn_Eq_constr: 0){ _LL3570: _temp3569=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3559)->f1; if(( unsigned int) _temp3569
>  1u?*(( int*) _temp3569) ==  Cyc_Absyn_Upper_b: 0){ _LL3572: _temp3571=((
struct Cyc_Absyn_Upper_b_struct*) _temp3569)->f1; goto _LL3564;} else{ goto
_LL3565;}} else{ goto _LL3565;} _LL3565: goto _LL3566; _LL3562: return; _LL3564:
if( Cyc_Evexp_eval_const_uint_exp( _temp3571) <=  i){({ void* _temp3573[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3573, sizeof( void*), 0u));});} return;
_LL3566:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3574=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3574[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3575; _temp3575.tag= Cyc_Absyn_Eq_constr;
_temp3575.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3576=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3576[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3577; _temp3577.tag= Cyc_Absyn_Upper_b;
_temp3577.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3577;}); _temp3576;}));
_temp3575;}); _temp3574;}))); return; _LL3560:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3578= Cyc_Tcutil_compress(
t); void* _temp3604; struct Cyc_List_List* _temp3606; struct Cyc_Absyn_Structdecl**
_temp3608; struct Cyc_List_List* _temp3610; struct Cyc_Absyn_Uniondecl**
_temp3612; struct Cyc_List_List* _temp3614; struct Cyc_List_List* _temp3616;
struct Cyc_List_List* _temp3618; _LL3580: if( _temp3578 == ( void*) Cyc_Absyn_VoidType){
goto _LL3581;} else{ goto _LL3582;} _LL3582: if(( unsigned int) _temp3578 >  4u?*((
int*) _temp3578) ==  Cyc_Absyn_IntType: 0){ goto _LL3583;} else{ goto _LL3584;}
_LL3584: if( _temp3578 == ( void*) Cyc_Absyn_FloatType){ goto _LL3585;} else{
goto _LL3586;} _LL3586: if( _temp3578 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3587;} else{ goto _LL3588;} _LL3588: if(( unsigned int) _temp3578 >  4u?*((
int*) _temp3578) ==  Cyc_Absyn_EnumType: 0){ goto _LL3589;} else{ goto _LL3590;}
_LL3590: if(( unsigned int) _temp3578 >  4u?*(( int*) _temp3578) ==  Cyc_Absyn_ArrayType:
0){ _LL3605: _temp3604=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3578)->f1;
goto _LL3591;} else{ goto _LL3592;} _LL3592: if(( unsigned int) _temp3578 >  4u?*((
int*) _temp3578) ==  Cyc_Absyn_TupleType: 0){ _LL3607: _temp3606=(( struct Cyc_Absyn_TupleType_struct*)
_temp3578)->f1; goto _LL3593;} else{ goto _LL3594;} _LL3594: if(( unsigned int)
_temp3578 >  4u?*(( int*) _temp3578) ==  Cyc_Absyn_StructType: 0){ _LL3611:
_temp3610=(( struct Cyc_Absyn_StructType_struct*) _temp3578)->f2; goto _LL3609;
_LL3609: _temp3608=(( struct Cyc_Absyn_StructType_struct*) _temp3578)->f3; goto
_LL3595;} else{ goto _LL3596;} _LL3596: if(( unsigned int) _temp3578 >  4u?*((
int*) _temp3578) ==  Cyc_Absyn_UnionType: 0){ _LL3615: _temp3614=(( struct Cyc_Absyn_UnionType_struct*)
_temp3578)->f2; goto _LL3613; _LL3613: _temp3612=(( struct Cyc_Absyn_UnionType_struct*)
_temp3578)->f3; goto _LL3597;} else{ goto _LL3598;} _LL3598: if(( unsigned int)
_temp3578 >  4u?*(( int*) _temp3578) ==  Cyc_Absyn_AnonStructType: 0){ _LL3617:
_temp3616=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3578)->f1; goto
_LL3599;} else{ goto _LL3600;} _LL3600: if(( unsigned int) _temp3578 >  4u?*((
int*) _temp3578) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3619: _temp3618=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3578)->f1; goto _LL3601;} else{ goto
_LL3602;} _LL3602: goto _LL3603; _LL3581: return 1; _LL3583: return 1; _LL3585:
return 1; _LL3587: return 1; _LL3589: return 0; _LL3591: return Cyc_Tcutil_bits_only(
_temp3604); _LL3593: for( 0; _temp3606 !=  0; _temp3606=(( struct Cyc_List_List*)
_check_null( _temp3606))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3606))->hd)).f2)){ return 0;}} return 1;
_LL3595: if( _temp3608 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp3620=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp3608)); if(
_temp3620->fields ==  0){ return 0;}{ struct _RegionHandle _temp3621=
_new_region(); struct _RegionHandle* rgn=& _temp3621; _push_region( rgn);{
struct Cyc_List_List* _temp3622=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3620->tvs, _temp3610);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp3620->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp3622,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ int _temp3623= 0;
_npop_handler( 0u); return _temp3623;}}}{ int _temp3624= 1; _npop_handler( 0u);
return _temp3624;}}; _pop_region( rgn);}} _LL3597: if( _temp3612 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3625=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3612)); if( _temp3625->fields ==  0){ return 0;}{ struct
_RegionHandle _temp3626= _new_region(); struct _RegionHandle* rgn=& _temp3626;
_push_region( rgn);{ struct Cyc_List_List* _temp3627=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3625->tvs, _temp3614);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3625->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3627,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3628= 0; _npop_handler( 0u); return
_temp3628;}}}{ int _temp3629= 1; _npop_handler( 0u); return _temp3629;}};
_pop_region( rgn);}} _LL3599: _temp3618= _temp3616; goto _LL3601; _LL3601: for(
0; _temp3618 !=  0; _temp3618=(( struct Cyc_List_List*) _check_null( _temp3618))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3618))->hd)->type)){ return 0;}} return 1;
_LL3603: return 0; _LL3579:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3630=( void*) e->r; struct
_tuple1* _temp3670; struct Cyc_Absyn_Exp* _temp3672; struct Cyc_Absyn_Exp*
_temp3674; struct Cyc_Absyn_Exp* _temp3676; struct Cyc_Absyn_Exp* _temp3678;
struct Cyc_Absyn_Exp* _temp3680; struct Cyc_Absyn_Exp* _temp3682; struct Cyc_Absyn_Exp*
_temp3684; struct Cyc_Absyn_Exp* _temp3686; void* _temp3688; struct Cyc_Absyn_Exp*
_temp3690; struct Cyc_Absyn_Exp* _temp3692; struct Cyc_Absyn_Exp* _temp3694;
struct Cyc_List_List* _temp3696; struct Cyc_List_List* _temp3698; struct Cyc_List_List*
_temp3700; struct Cyc_List_List* _temp3702; void* _temp3704; struct Cyc_List_List*
_temp3706; struct Cyc_List_List* _temp3708; _LL3632: if(*(( int*) _temp3630) == 
Cyc_Absyn_Const_e){ goto _LL3633;} else{ goto _LL3634;} _LL3634: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3635;} else{ goto _LL3636;}
_LL3636: if(*(( int*) _temp3630) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3637;}
else{ goto _LL3638;} _LL3638: if(*(( int*) _temp3630) ==  Cyc_Absyn_Enum_e){
goto _LL3639;} else{ goto _LL3640;} _LL3640: if(*(( int*) _temp3630) ==  Cyc_Absyn_Var_e){
_LL3671: _temp3670=(( struct Cyc_Absyn_Var_e_struct*) _temp3630)->f1; goto
_LL3641;} else{ goto _LL3642;} _LL3642: if(*(( int*) _temp3630) ==  Cyc_Absyn_Conditional_e){
_LL3677: _temp3676=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3630)->f1;
goto _LL3675; _LL3675: _temp3674=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3630)->f2; goto _LL3673; _LL3673: _temp3672=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3630)->f3; goto _LL3643;} else{ goto _LL3644;} _LL3644: if(*(( int*)
_temp3630) ==  Cyc_Absyn_SeqExp_e){ _LL3681: _temp3680=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3630)->f1; goto _LL3679; _LL3679: _temp3678=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3630)->f2; goto _LL3645;} else{ goto _LL3646;} _LL3646: if(*(( int*)
_temp3630) ==  Cyc_Absyn_NoInstantiate_e){ _LL3683: _temp3682=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3630)->f1; goto _LL3647;} else{ goto _LL3648;} _LL3648: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Instantiate_e){ _LL3685: _temp3684=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3630)->f1; goto _LL3649;} else{ goto _LL3650;} _LL3650: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Cast_e){ _LL3689: _temp3688=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3630)->f1; goto _LL3687; _LL3687: _temp3686=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3630)->f2; goto _LL3651;} else{ goto _LL3652;} _LL3652: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Address_e){ _LL3691: _temp3690=(( struct Cyc_Absyn_Address_e_struct*)
_temp3630)->f1; goto _LL3653;} else{ goto _LL3654;} _LL3654: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Comprehension_e){ _LL3695: _temp3694=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3630)->f2; goto _LL3693; _LL3693: _temp3692=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3630)->f3; goto _LL3655;} else{ goto _LL3656;} _LL3656: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Array_e){ _LL3697: _temp3696=(( struct Cyc_Absyn_Array_e_struct*)
_temp3630)->f1; goto _LL3657;} else{ goto _LL3658;} _LL3658: if(*(( int*)
_temp3630) ==  Cyc_Absyn_AnonStruct_e){ _LL3699: _temp3698=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3630)->f2; goto _LL3659;} else{ goto _LL3660;} _LL3660: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Struct_e){ _LL3701: _temp3700=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3630)->f3; goto _LL3661;} else{ goto _LL3662;} _LL3662: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Primop_e){ _LL3705: _temp3704=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3630)->f1; goto _LL3703; _LL3703: _temp3702=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3630)->f2; goto _LL3663;} else{ goto _LL3664;} _LL3664: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Tuple_e){ _LL3707: _temp3706=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3630)->f1; goto _LL3665;} else{ goto _LL3666;} _LL3666: if(*(( int*)
_temp3630) ==  Cyc_Absyn_Tunion_e){ _LL3709: _temp3708=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3630)->f3; goto _LL3667;} else{ goto _LL3668;} _LL3668: goto _LL3669;
_LL3633: return 1; _LL3635: return 1; _LL3637: return 1; _LL3639: return 1;
_LL3641: { struct _handler_cons _temp3710; _push_handler(& _temp3710);{ int
_temp3712= 0; if( setjmp( _temp3710.handler)){ _temp3712= 1;} if( ! _temp3712){{
void* _temp3713= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3670); void*
_temp3721; void* _temp3723; struct Cyc_Absyn_Vardecl* _temp3725; _LL3715: if(*((
int*) _temp3713) ==  Cyc_Tcenv_VarRes){ _LL3722: _temp3721=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3713)->f1; if(( unsigned int) _temp3721 >  1u?*(( int*) _temp3721) ==  Cyc_Absyn_Funname_b:
0){ goto _LL3716;} else{ goto _LL3717;}} else{ goto _LL3717;} _LL3717: if(*((
int*) _temp3713) ==  Cyc_Tcenv_VarRes){ _LL3724: _temp3723=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3713)->f1; if(( unsigned int) _temp3723 >  1u?*(( int*) _temp3723) ==  Cyc_Absyn_Global_b:
0){ _LL3726: _temp3725=(( struct Cyc_Absyn_Global_b_struct*) _temp3723)->f1;
goto _LL3718;} else{ goto _LL3719;}} else{ goto _LL3719;} _LL3719: goto _LL3720;
_LL3716: { int _temp3727= 1; _npop_handler( 0u); return _temp3727;} _LL3718: {
void* _temp3728= Cyc_Tcutil_compress(( void*) _temp3725->type); _LL3730: if((
unsigned int) _temp3728 >  4u?*(( int*) _temp3728) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3731;} else{ goto _LL3732;} _LL3732: goto _LL3733; _LL3731: { int
_temp3734= 1; _npop_handler( 0u); return _temp3734;} _LL3733: { int _temp3735=
var_okay; _npop_handler( 0u); return _temp3735;} _LL3729:;} _LL3720: { int
_temp3736= var_okay; _npop_handler( 0u); return _temp3736;} _LL3714:;};
_pop_handler();} else{ void* _temp3711=( void*) _exn_thrown; void* _temp3738=
_temp3711; _LL3740: if( _temp3738 ==  Cyc_Dict_Absent){ goto _LL3741;} else{
goto _LL3742;} _LL3742: goto _LL3743; _LL3741: return 0; _LL3743:( void) _throw(
_temp3738); _LL3739:;}}} _LL3643: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3676)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3674): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3672): 0; _LL3645: return Cyc_Tcutil_cnst_exp( te, 0, _temp3680)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3678): 0; _LL3647: _temp3684= _temp3682; goto _LL3649; _LL3649:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3684); _LL3651: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3686); _LL3653: return Cyc_Tcutil_cnst_exp( te, 1, _temp3690);
_LL3655: return Cyc_Tcutil_cnst_exp( te, 0, _temp3694)? Cyc_Tcutil_cnst_exp( te,
0, _temp3692): 0; _LL3657: _temp3698= _temp3696; goto _LL3659; _LL3659:
_temp3700= _temp3698; goto _LL3661; _LL3661: for( 0; _temp3700 !=  0; _temp3700=((
struct Cyc_List_List*) _check_null( _temp3700))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3700))->hd)).f2)){
return 0;}} return 1; _LL3663: _temp3706= _temp3702; goto _LL3665; _LL3665:
_temp3708= _temp3706; goto _LL3667; _LL3667: for( 0; _temp3708 !=  0; _temp3708=((
struct Cyc_List_List*) _check_null( _temp3708))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3708))->hd)){
return 0;}} return 1; _LL3669: return 0; _LL3631:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3744= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3782; struct Cyc_Absyn_Conref* _temp3784; struct Cyc_Absyn_Conref*
_temp3786; void* _temp3788; void* _temp3790; struct Cyc_List_List* _temp3792;
struct Cyc_Absyn_Structdecl** _temp3794; struct Cyc_List_List* _temp3796; struct
Cyc_Absyn_Uniondecl** _temp3798; struct Cyc_List_List* _temp3800; struct Cyc_List_List*
_temp3802; struct Cyc_List_List* _temp3804; _LL3746: if( _temp3744 == ( void*)
Cyc_Absyn_VoidType){ goto _LL3747;} else{ goto _LL3748;} _LL3748: if((
unsigned int) _temp3744 >  4u?*(( int*) _temp3744) ==  Cyc_Absyn_Evar: 0){ goto
_LL3749;} else{ goto _LL3750;} _LL3750: if(( unsigned int) _temp3744 >  4u?*((
int*) _temp3744) ==  Cyc_Absyn_VarType: 0){ goto _LL3751;} else{ goto _LL3752;}
_LL3752: if(( unsigned int) _temp3744 >  4u?*(( int*) _temp3744) ==  Cyc_Absyn_TunionType:
0){ goto _LL3753;} else{ goto _LL3754;} _LL3754: if(( unsigned int) _temp3744 > 
4u?*(( int*) _temp3744) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL3755;} else{
goto _LL3756;} _LL3756: if(( unsigned int) _temp3744 >  4u?*(( int*) _temp3744)
==  Cyc_Absyn_PointerType: 0){ _LL3783: _temp3782=(( struct Cyc_Absyn_PointerType_struct*)
_temp3744)->f1; _LL3789: _temp3788=( void*) _temp3782.elt_typ; goto _LL3787;
_LL3787: _temp3786= _temp3782.nullable; goto _LL3785; _LL3785: _temp3784=
_temp3782.bounds; goto _LL3757;} else{ goto _LL3758;} _LL3758: if(( unsigned int)
_temp3744 >  4u?*(( int*) _temp3744) ==  Cyc_Absyn_IntType: 0){ goto _LL3759;}
else{ goto _LL3760;} _LL3760: if( _temp3744 == ( void*) Cyc_Absyn_FloatType){
goto _LL3761;} else{ goto _LL3762;} _LL3762: if( _temp3744 == ( void*) Cyc_Absyn_DoubleType){
goto _LL3763;} else{ goto _LL3764;} _LL3764: if(( unsigned int) _temp3744 >  4u?*((
int*) _temp3744) ==  Cyc_Absyn_ArrayType: 0){ _LL3791: _temp3790=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3744)->f1; goto _LL3765;} else{ goto
_LL3766;} _LL3766: if(( unsigned int) _temp3744 >  4u?*(( int*) _temp3744) == 
Cyc_Absyn_FnType: 0){ goto _LL3767;} else{ goto _LL3768;} _LL3768: if((
unsigned int) _temp3744 >  4u?*(( int*) _temp3744) ==  Cyc_Absyn_TupleType: 0){
_LL3793: _temp3792=(( struct Cyc_Absyn_TupleType_struct*) _temp3744)->f1; goto
_LL3769;} else{ goto _LL3770;} _LL3770: if(( unsigned int) _temp3744 >  4u?*((
int*) _temp3744) ==  Cyc_Absyn_StructType: 0){ _LL3797: _temp3796=(( struct Cyc_Absyn_StructType_struct*)
_temp3744)->f2; goto _LL3795; _LL3795: _temp3794=(( struct Cyc_Absyn_StructType_struct*)
_temp3744)->f3; goto _LL3771;} else{ goto _LL3772;} _LL3772: if(( unsigned int)
_temp3744 >  4u?*(( int*) _temp3744) ==  Cyc_Absyn_UnionType: 0){ _LL3801:
_temp3800=(( struct Cyc_Absyn_UnionType_struct*) _temp3744)->f2; goto _LL3799;
_LL3799: _temp3798=(( struct Cyc_Absyn_UnionType_struct*) _temp3744)->f3; goto
_LL3773;} else{ goto _LL3774;} _LL3774: if(( unsigned int) _temp3744 >  4u?*((
int*) _temp3744) ==  Cyc_Absyn_AnonStructType: 0){ _LL3803: _temp3802=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3744)->f1; goto _LL3775;} else{ goto
_LL3776;} _LL3776: if(( unsigned int) _temp3744 >  4u?*(( int*) _temp3744) == 
Cyc_Absyn_AnonUnionType: 0){ _LL3805: _temp3804=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3744)->f1; goto _LL3777;} else{ goto _LL3778;} _LL3778: if(( unsigned int)
_temp3744 >  4u?*(( int*) _temp3744) ==  Cyc_Absyn_EnumType: 0){ goto _LL3779;}
else{ goto _LL3780;} _LL3780: goto _LL3781; _LL3747: return 1; _LL3749: return 0;
_LL3751: return 0; _LL3753: return 0; _LL3755: return 0; _LL3757: { void*
_temp3806=( void*)( Cyc_Absyn_compress_conref( _temp3784))->v; void* _temp3814;
void* _temp3816; _LL3808: if(( unsigned int) _temp3806 >  1u?*(( int*) _temp3806)
==  Cyc_Absyn_Eq_constr: 0){ _LL3815: _temp3814=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3806)->f1; if( _temp3814 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3809;}
else{ goto _LL3810;}} else{ goto _LL3810;} _LL3810: if(( unsigned int) _temp3806
>  1u?*(( int*) _temp3806) ==  Cyc_Absyn_Eq_constr: 0){ _LL3817: _temp3816=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3806)->f1; if(( unsigned int)
_temp3816 >  1u?*(( int*) _temp3816) ==  Cyc_Absyn_Upper_b: 0){ goto _LL3811;}
else{ goto _LL3812;}} else{ goto _LL3812;} _LL3812: goto _LL3813; _LL3809:
return 1; _LL3811: { void* _temp3818=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3786))->v; int
_temp3824; _LL3820: if(( unsigned int) _temp3818 >  1u?*(( int*) _temp3818) == 
Cyc_Absyn_Eq_constr: 0){ _LL3825: _temp3824=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3818)->f1; goto _LL3821;} else{ goto _LL3822;} _LL3822: goto _LL3823;
_LL3821: return _temp3824; _LL3823: return 0; _LL3819:;} _LL3813: return 0;
_LL3807:;} _LL3759: return 1; _LL3761: return 1; _LL3763: return 1; _LL3765:
return Cyc_Tcutil_supports_default( _temp3790); _LL3767: return 0; _LL3769: for(
0; _temp3792 !=  0; _temp3792=(( struct Cyc_List_List*) _check_null( _temp3792))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3792))->hd)).f2)){ return 0;}} return 1; _LL3771: if(
_temp3794 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3794)); if( sd->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3796,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3773: if( _temp3798 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3798)); if( ud->fields == 
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3800,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3775: _temp3804= _temp3802; goto _LL3777; _LL3777: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3804); _LL3779: return 1; _LL3781: return 0; _LL3745:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3826= _new_region();
struct _RegionHandle* rgn=& _temp3826; _push_region( rgn);{ struct Cyc_List_List*
_temp3827=(( struct Cyc_List_List*(*)( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3827,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp3828= 0; _npop_handler( 0u); return _temp3828;}}}; _pop_region(
rgn);} return 1;}
