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
goto _LL78;} _LL80: goto _LL82; _LL82: return t; _LL84: return( void*)({ struct
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
else{ goto _LL272;} _LL272: goto _LL273; _LL267: goto _LL269; _LL269: goto
_LL271; _LL271: return 1; _LL273: return 0; _LL265:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp286= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp340;
struct Cyc_Core_Opt* _temp342; struct Cyc_Absyn_Tvar* _temp344; void* _temp346;
struct Cyc_Absyn_TunionFieldInfo _temp348; void* _temp350; struct Cyc_Absyn_Tunionfield*
_temp352; struct Cyc_Absyn_TunionFieldInfo _temp354; void* _temp356; struct Cyc_Absyn_Structdecl**
_temp358; struct Cyc_Absyn_Uniondecl** _temp360; struct Cyc_Absyn_PtrInfo
_temp362; struct Cyc_Core_Opt* _temp364; _LL288: if(( unsigned int) _temp286 > 
4u?*(( int*) _temp286) ==  Cyc_Absyn_Evar: 0){ _LL343: _temp342=(( struct Cyc_Absyn_Evar_struct*)
_temp286)->f1; goto _LL341; _LL341: _temp340=(( struct Cyc_Absyn_Evar_struct*)
_temp286)->f2; goto _LL289;} else{ goto _LL290;} _LL290: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_VarType: 0){ _LL345: _temp344=((
struct Cyc_Absyn_VarType_struct*) _temp286)->f1; goto _LL291;} else{ goto _LL292;}
_LL292: if( _temp286 == ( void*) Cyc_Absyn_VoidType){ goto _LL293;} else{ goto
_LL294;} _LL294: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_IntType:
0){ _LL347: _temp346=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp286)->f2;
goto _LL295;} else{ goto _LL296;} _LL296: if( _temp286 == ( void*) Cyc_Absyn_FloatType){
goto _LL297;} else{ goto _LL298;} _LL298: if( _temp286 == ( void*) Cyc_Absyn_DoubleType){
goto _LL299;} else{ goto _LL300;} _LL300: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_FnType: 0){ goto _LL301;} else{ goto _LL302;}
_LL302: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_RgnHandleType:
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
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_ArrayType: 0){ goto _LL329;}
else{ goto _LL330;} _LL330: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286)
==  Cyc_Absyn_TupleType: 0){ goto _LL331;} else{ goto _LL332;} _LL332: if((
unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_TypedefType: 0){
_LL365: _temp364=(( struct Cyc_Absyn_TypedefType_struct*) _temp286)->f3; goto
_LL333;} else{ goto _LL334;} _LL334: if(( unsigned int) _temp286 >  4u?*(( int*)
_temp286) ==  Cyc_Absyn_AccessEff: 0){ goto _LL335;} else{ goto _LL336;} _LL336:
if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_JoinEff: 0){
goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL339;} else{ goto _LL287;}
_LL289: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp342))->v;
_LL291: return Cyc_Absyn_conref_val( _temp344->kind); _LL293: return( void*) Cyc_Absyn_MemKind;
_LL295: return _temp346 == ( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL297: goto _LL299; _LL299: goto _LL301; _LL301:
return( void*) Cyc_Absyn_MemKind; _LL303: return( void*) Cyc_Absyn_BoxKind;
_LL305: return( void*) Cyc_Absyn_RgnKind; _LL307: return( void*) Cyc_Absyn_BoxKind;
_LL309: if( _temp352->typs ==  0){ return( void*) Cyc_Absyn_BoxKind;} else{
return( void*) Cyc_Absyn_MemKind;} _LL311: return({ void* _temp366[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("typ_kind: Unresolved TunionFieldType", sizeof( unsigned char), 37u),
_tag_arr( _temp366, sizeof( void*), 0u));}); _LL313: goto _LL315; _LL315: return(
void*) Cyc_Absyn_AnyKind; _LL317: goto _LL319; _LL319: goto _LL321; _LL321: goto
_LL323; _LL323: return( void*) Cyc_Absyn_MemKind; _LL325: return( void*) Cyc_Absyn_BoxKind;
_LL327: { void* _temp367=( void*)( Cyc_Absyn_compress_conref( _temp362.bounds))->v;
void* _temp377; void* _temp379; _LL369: if(( unsigned int) _temp367 >  1u?*((
int*) _temp367) ==  Cyc_Absyn_Eq_constr: 0){ _LL378: _temp377=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp367)->f1; if( _temp377 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL370;} else{ goto _LL371;}} else{ goto _LL371;} _LL371: if(( unsigned int)
_temp367 >  1u?*(( int*) _temp367) ==  Cyc_Absyn_Eq_constr: 0){ _LL380: _temp379=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp367)->f1; if(( unsigned int)
_temp379 >  1u?*(( int*) _temp379) ==  Cyc_Absyn_Upper_b: 0){ goto _LL372;}
else{ goto _LL373;}} else{ goto _LL373;} _LL373: if( _temp367 == ( void*) Cyc_Absyn_No_constr){
goto _LL374;} else{ goto _LL375;} _LL375: if(( unsigned int) _temp367 >  1u?*((
int*) _temp367) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL376;} else{ goto
_LL368;} _LL370: return( void*) Cyc_Absyn_MemKind; _LL372: return( void*) Cyc_Absyn_BoxKind;
_LL374: return( void*) Cyc_Absyn_MemKind; _LL376: return({ void* _temp381[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds", sizeof(
unsigned char), 43u), _tag_arr( _temp381, sizeof( void*), 0u));}); _LL368:;}
_LL329: goto _LL331; _LL331: return( void*) Cyc_Absyn_MemKind; _LL333: return({
struct Cyc_Std_String_pa_struct _temp383; _temp383.tag= Cyc_Std_String_pa;
_temp383.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp382[ 1u]={&
_temp383}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp382, sizeof( void*), 1u));}}); _LL335: goto
_LL337; _LL337: goto _LL339; _LL339: return( void*) Cyc_Absyn_EffKind; _LL287:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp384; _push_handler(& _temp384);{
int _temp386= 0; if( setjmp( _temp384.handler)){ _temp386= 1;} if( ! _temp386){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp387= 1; _npop_handler( 0u); return
_temp387;}; _pop_handler();} else{ void* _temp385=( void*) _exn_thrown; void*
_temp389= _temp385; _LL391: if( _temp389 ==  Cyc_Tcutil_Unify){ goto _LL392;}
else{ goto _LL393;} _LL393: goto _LL394; _LL392: return 0; _LL394:( void) _throw(
_temp389); _LL390:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp395= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp431; struct Cyc_Core_Opt* _temp433; struct Cyc_Core_Opt**
_temp435; struct Cyc_Core_Opt* _temp436; struct Cyc_Absyn_PtrInfo _temp438; void*
_temp440; struct Cyc_Absyn_FnInfo _temp442; struct Cyc_List_List* _temp444;
struct Cyc_List_List* _temp446; struct Cyc_Absyn_VarargInfo* _temp448; int
_temp450; struct Cyc_List_List* _temp452; void* _temp454; struct Cyc_Core_Opt*
_temp456; struct Cyc_List_List* _temp458; struct Cyc_List_List* _temp460; void*
_temp462; struct Cyc_Absyn_TunionInfo _temp464; void* _temp466; struct Cyc_List_List*
_temp468; struct Cyc_Core_Opt* _temp470; struct Cyc_List_List* _temp472; struct
Cyc_Absyn_TunionFieldInfo _temp474; struct Cyc_List_List* _temp476; struct Cyc_List_List*
_temp478; struct Cyc_List_List* _temp480; struct Cyc_List_List* _temp482; void*
_temp484; struct Cyc_List_List* _temp486; void* _temp488; _LL397: if((
unsigned int) _temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_VarType: 0){
_LL432: _temp431=(( struct Cyc_Absyn_VarType_struct*) _temp395)->f1; goto _LL398;}
else{ goto _LL399;} _LL399: if(( unsigned int) _temp395 >  4u?*(( int*) _temp395)
==  Cyc_Absyn_Evar: 0){ _LL437: _temp436=(( struct Cyc_Absyn_Evar_struct*)
_temp395)->f2; goto _LL434; _LL434: _temp433=(( struct Cyc_Absyn_Evar_struct*)
_temp395)->f4; _temp435=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp395)->f4; goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int)
_temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_PointerType: 0){ _LL439:
_temp438=(( struct Cyc_Absyn_PointerType_struct*) _temp395)->f1; goto _LL402;}
else{ goto _LL403;} _LL403: if(( unsigned int) _temp395 >  4u?*(( int*) _temp395)
==  Cyc_Absyn_ArrayType: 0){ _LL441: _temp440=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp395)->f1; goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int)
_temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_FnType: 0){ _LL443: _temp442=((
struct Cyc_Absyn_FnType_struct*) _temp395)->f1; _LL459: _temp458= _temp442.tvars;
goto _LL457; _LL457: _temp456= _temp442.effect; goto _LL455; _LL455: _temp454=(
void*) _temp442.ret_typ; goto _LL453; _LL453: _temp452= _temp442.args; goto
_LL451; _LL451: _temp450= _temp442.c_varargs; goto _LL449; _LL449: _temp448=
_temp442.cyc_varargs; goto _LL447; _LL447: _temp446= _temp442.rgn_po; goto
_LL445; _LL445: _temp444= _temp442.attributes; goto _LL406;} else{ goto _LL407;}
_LL407: if(( unsigned int) _temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_TupleType:
0){ _LL461: _temp460=(( struct Cyc_Absyn_TupleType_struct*) _temp395)->f1; goto
_LL408;} else{ goto _LL409;} _LL409: if(( unsigned int) _temp395 >  4u?*(( int*)
_temp395) ==  Cyc_Absyn_RgnHandleType: 0){ _LL463: _temp462=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp395)->f1; goto _LL410;} else{ goto _LL411;}
_LL411: if(( unsigned int) _temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_TunionType:
0){ _LL465: _temp464=(( struct Cyc_Absyn_TunionType_struct*) _temp395)->f1;
_LL469: _temp468= _temp464.targs; goto _LL467; _LL467: _temp466=( void*)
_temp464.rgn; goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int)
_temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_TypedefType: 0){ _LL473:
_temp472=(( struct Cyc_Absyn_TypedefType_struct*) _temp395)->f2; goto _LL471;
_LL471: _temp470=(( struct Cyc_Absyn_TypedefType_struct*) _temp395)->f3; goto
_LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp395 >  4u?*(( int*)
_temp395) ==  Cyc_Absyn_TunionFieldType: 0){ _LL475: _temp474=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp395)->f1; _LL477: _temp476= _temp474.targs; goto _LL416;} else{ goto _LL417;}
_LL417: if(( unsigned int) _temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_StructType:
0){ _LL479: _temp478=(( struct Cyc_Absyn_StructType_struct*) _temp395)->f2; goto
_LL418;} else{ goto _LL419;} _LL419: if(( unsigned int) _temp395 >  4u?*(( int*)
_temp395) ==  Cyc_Absyn_AnonStructType: 0){ _LL481: _temp480=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp395)->f1; goto _LL420;} else{ goto _LL421;} _LL421: if(( unsigned int)
_temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_AnonUnionType: 0){ _LL483:
_temp482=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp395)->f1; goto _LL422;}
else{ goto _LL423;} _LL423: if(( unsigned int) _temp395 >  4u?*(( int*) _temp395)
==  Cyc_Absyn_AccessEff: 0){ _LL485: _temp484=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp395)->f1; goto _LL424;} else{ goto _LL425;} _LL425: if(( unsigned int)
_temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_JoinEff: 0){ _LL487: _temp486=((
struct Cyc_Absyn_JoinEff_struct*) _temp395)->f1; goto _LL426;} else{ goto _LL427;}
_LL427: if(( unsigned int) _temp395 >  4u?*(( int*) _temp395) ==  Cyc_Absyn_RgnsEff:
0){ _LL489: _temp488=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp395)->f1;
goto _LL428;} else{ goto _LL429;} _LL429: goto _LL430; _LL398: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp431)){( int) _throw(( void*) Cyc_Tcutil_Unify);} goto _LL396; _LL400: if( t
==  evar){( int) _throw(( void*) Cyc_Tcutil_Unify);} else{ if( _temp436 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp436))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp435))->v; for( 0; s !=  0; s=(( struct
Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)( int(* compare)( struct
Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar*
x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd)){ problem= 1; break;}}} if( problem){
struct Cyc_List_List* _temp490= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp435))->v; for( 0; s !=  0; s=(( struct
Cyc_List_List*) _check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp490=({ struct Cyc_List_List* _temp491=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp491->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp491->tl= _temp490; _temp491;});}}}*
_temp435=({ struct Cyc_Core_Opt* _temp492=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp492->v=( void*) _temp490; _temp492;});}}}
goto _LL396; _LL402: Cyc_Tcutil_occurs( evar, env,( void*) _temp438.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp438.rgn_typ); goto _LL396; _LL404:
Cyc_Tcutil_occurs( evar, env, _temp440); goto _LL396; _LL406: if( _temp456 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp456))->v);} Cyc_Tcutil_occurs( evar, env, _temp454); for( 0; _temp452 !=  0;
_temp452=(( struct Cyc_List_List*) _check_null( _temp452))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp452))->hd)).f3);}
if( _temp448 !=  0){ void* _temp495; struct Cyc_Absyn_VarargInfo _temp493=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp448)); _LL496: _temp495=( void*)
_temp493.type; goto _LL494; _LL494: Cyc_Tcutil_occurs( evar, env, _temp495);}
for( 0; _temp446 !=  0; _temp446=(( struct Cyc_List_List*) _check_null( _temp446))->tl){
struct _tuple8 _temp499; void* _temp500; void* _temp502; struct _tuple8*
_temp497=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp446))->hd;
_temp499=* _temp497; _LL503: _temp502= _temp499.f1; goto _LL501; _LL501:
_temp500= _temp499.f2; goto _LL498; _LL498: Cyc_Tcutil_occurs( evar, env,
_temp502); Cyc_Tcutil_occurs( evar, env, _temp500);} goto _LL396; _LL408: for( 0;
_temp460 !=  0; _temp460=(( struct Cyc_List_List*) _check_null( _temp460))->tl){
Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp460))->hd)).f2);} goto _LL396; _LL410: Cyc_Tcutil_occurs( evar,
env, _temp462); goto _LL396; _LL412: Cyc_Tcutil_occurs( evar, env, _temp466);
Cyc_Tcutil_occurslist( evar, env, _temp468); goto _LL396; _LL414: _temp476=
_temp472; goto _LL416; _LL416: _temp478= _temp476; goto _LL418; _LL418: Cyc_Tcutil_occurslist(
evar, env, _temp478); goto _LL396; _LL420: _temp482= _temp480; goto _LL422;
_LL422: for( 0; _temp482 !=  0; _temp482=(( struct Cyc_List_List*) _check_null(
_temp482))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp482))->hd)->type);} goto _LL396; _LL424:
Cyc_Tcutil_occurs( evar, env, _temp484); goto _LL396; _LL426: Cyc_Tcutil_occurslist(
evar, env, _temp486); goto _LL396; _LL428: Cyc_Tcutil_occurs( evar, env,
_temp488); goto _LL396; _LL430: goto _LL396; _LL396:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts != 
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp504; _push_handler(& _temp504);{ int _temp506= 0; if( setjmp( _temp504.handler)){
_temp506= 1;} if( ! _temp506){ Cyc_List_iter2( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp505=( void*) _exn_thrown; void* _temp508=
_temp505; _LL510: if( _temp508 ==  Cyc_List_List_mismatch){ goto _LL511;} else{
goto _LL512;} _LL512: goto _LL513; _LL511:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL513:( void) _throw( _temp508); _LL509:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != 
tq2.q_const? 1: tq1.q_volatile !=  tq2.q_volatile)? 1: tq1.q_restrict !=  tq2.q_restrict){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const ==  tq2.q_const? tq1.q_volatile
==  tq2.q_volatile: 0)? tq1.q_restrict ==  tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x == 
y){ return;}{ void* _temp514=( void*) x->v; void* _temp522; _LL516: if( _temp514
== ( void*) Cyc_Absyn_No_constr){ goto _LL517;} else{ goto _LL518;} _LL518: if((
unsigned int) _temp514 >  1u?*(( int*) _temp514) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL519;} else{ goto _LL520;} _LL520: if(( unsigned int) _temp514 >  1u?*((
int*) _temp514) ==  Cyc_Absyn_Eq_constr: 0){ _LL523: _temp522=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp514)->f1; goto _LL521;} else{ goto _LL515;}
_LL517:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp524=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp524[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp525; _temp525.tag= Cyc_Absyn_Forward_constr; _temp525.f1= y; _temp525;});
_temp524;}))); return; _LL519:( int) _throw(({ void* _temp526[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp526, sizeof( void*), 0u));})); _LL521: { void* _temp527=( void*)
y->v; void* _temp535; _LL529: if( _temp527 == ( void*) Cyc_Absyn_No_constr){
goto _LL530;} else{ goto _LL531;} _LL531: if(( unsigned int) _temp527 >  1u?*((
int*) _temp527) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL532;} else{ goto
_LL533;} _LL533: if(( unsigned int) _temp527 >  1u?*(( int*) _temp527) ==  Cyc_Absyn_Eq_constr:
0){ _LL536: _temp535=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp527)->f1;
goto _LL534;} else{ goto _LL528;} _LL530:( void*)( y->v=( void*)(( void*) x->v));
return; _LL532:( int) _throw(({ void* _temp537[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp537, sizeof( void*), 0u));})); _LL534: if( cmp( _temp522,
_temp535) !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL528:;}
_LL515:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp538; _push_handler(& _temp538);{ int _temp540= 0; if( setjmp( _temp538.handler)){
_temp540= 1;} if( ! _temp540){ Cyc_Tcutil_unify_it_conrefs( cmp, x, y);{ int
_temp541= 1; _npop_handler( 0u); return _temp541;}; _pop_handler();} else{ void*
_temp539=( void*) _exn_thrown; void* _temp543= _temp539; _LL545: if( _temp543 == 
Cyc_Tcutil_Unify){ goto _LL546;} else{ goto _LL547;} _LL547: goto _LL548; _LL546:
return 0; _LL548:( void) _throw( _temp543); _LL544:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple8 _temp550=({ struct _tuple8 _temp549;
_temp549.f1= b1; _temp549.f2= b2; _temp549;}); void* _temp560; void* _temp562;
void* _temp564; void* _temp566; void* _temp568; struct Cyc_Absyn_Exp* _temp570;
void* _temp572; struct Cyc_Absyn_Exp* _temp574; _LL552: _LL563: _temp562=
_temp550.f1; if( _temp562 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL561;} else{
goto _LL554;} _LL561: _temp560= _temp550.f2; if( _temp560 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL553;} else{ goto _LL554;} _LL554: _LL565: _temp564= _temp550.f1; if(
_temp564 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL555;} else{ goto _LL556;}
_LL556: _LL567: _temp566= _temp550.f2; if( _temp566 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL557;} else{ goto _LL558;} _LL558: _LL573: _temp572= _temp550.f1; if((
unsigned int) _temp572 >  1u?*(( int*) _temp572) ==  Cyc_Absyn_Upper_b: 0){
_LL575: _temp574=(( struct Cyc_Absyn_Upper_b_struct*) _temp572)->f1; goto _LL569;}
else{ goto _LL551;} _LL569: _temp568= _temp550.f2; if(( unsigned int) _temp568 > 
1u?*(( int*) _temp568) ==  Cyc_Absyn_Upper_b: 0){ _LL571: _temp570=(( struct Cyc_Absyn_Upper_b_struct*)
_temp568)->f1; goto _LL559;} else{ goto _LL551;} _LL553: return 0; _LL555:
return - 1; _LL557: return 1; _LL559: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp574); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp570); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL551:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp577=({
struct _tuple8 _temp576; _temp576.f1= a1; _temp576.f2= a2; _temp576;}); void*
_temp589; int _temp591; int _temp593; void* _temp595; void* _temp597; int
_temp599; int _temp601; void* _temp603; void* _temp605; int _temp607; void*
_temp609; int _temp611; void* _temp613; int _temp615; void* _temp617; int
_temp619; void* _temp621; struct _tagged_arr _temp623; void* _temp625; struct
_tagged_arr _temp627; _LL579: _LL598: _temp597= _temp577.f1; if(( unsigned int)
_temp597 >  16u?*(( int*) _temp597) ==  Cyc_Absyn_Format_att: 0){ _LL604:
_temp603=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp597)->f1; goto
_LL602; _LL602: _temp601=(( struct Cyc_Absyn_Format_att_struct*) _temp597)->f2;
goto _LL600; _LL600: _temp599=(( struct Cyc_Absyn_Format_att_struct*) _temp597)->f3;
goto _LL590;} else{ goto _LL581;} _LL590: _temp589= _temp577.f2; if((
unsigned int) _temp589 >  16u?*(( int*) _temp589) ==  Cyc_Absyn_Format_att: 0){
_LL596: _temp595=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp589)->f1;
goto _LL594; _LL594: _temp593=(( struct Cyc_Absyn_Format_att_struct*) _temp589)->f2;
goto _LL592; _LL592: _temp591=(( struct Cyc_Absyn_Format_att_struct*) _temp589)->f3;
goto _LL580;} else{ goto _LL581;} _LL581: _LL610: _temp609= _temp577.f1; if((
unsigned int) _temp609 >  16u?*(( int*) _temp609) ==  Cyc_Absyn_Regparm_att: 0){
_LL612: _temp611=(( struct Cyc_Absyn_Regparm_att_struct*) _temp609)->f1; goto
_LL606;} else{ goto _LL583;} _LL606: _temp605= _temp577.f2; if(( unsigned int)
_temp605 >  16u?*(( int*) _temp605) ==  Cyc_Absyn_Regparm_att: 0){ _LL608:
_temp607=(( struct Cyc_Absyn_Regparm_att_struct*) _temp605)->f1; goto _LL582;}
else{ goto _LL583;} _LL583: _LL618: _temp617= _temp577.f1; if(( unsigned int)
_temp617 >  16u?*(( int*) _temp617) ==  Cyc_Absyn_Aligned_att: 0){ _LL620:
_temp619=(( struct Cyc_Absyn_Aligned_att_struct*) _temp617)->f1; goto _LL614;}
else{ goto _LL585;} _LL614: _temp613= _temp577.f2; if(( unsigned int) _temp613 > 
16u?*(( int*) _temp613) ==  Cyc_Absyn_Aligned_att: 0){ _LL616: _temp615=((
struct Cyc_Absyn_Aligned_att_struct*) _temp613)->f1; goto _LL584;} else{ goto
_LL585;} _LL585: _LL626: _temp625= _temp577.f1; if(( unsigned int) _temp625 > 
16u?*(( int*) _temp625) ==  Cyc_Absyn_Section_att: 0){ _LL628: _temp627=((
struct Cyc_Absyn_Section_att_struct*) _temp625)->f1; goto _LL622;} else{ goto
_LL587;} _LL622: _temp621= _temp577.f2; if(( unsigned int) _temp621 >  16u?*((
int*) _temp621) ==  Cyc_Absyn_Section_att: 0){ _LL624: _temp623=(( struct Cyc_Absyn_Section_att_struct*)
_temp621)->f1; goto _LL586;} else{ goto _LL587;} _LL587: goto _LL588; _LL580:
return( _temp603 ==  _temp595? _temp601 ==  _temp593: 0)? _temp599 ==  _temp591:
0; _LL582: _temp619= _temp611; _temp615= _temp607; goto _LL584; _LL584: return
_temp619 ==  _temp615; _LL586: return Cyc_Std_strcmp( _temp627, _temp623) ==  0;
_LL588: return 0; _LL578:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,(
void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct
Cyc_List_List* a= a2; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null(
a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void* Cyc_Tcutil_normalize_effect(
void* e); static void* Cyc_Tcutil_rgns_of( void* t); static void* Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Structfield* sf){ return Cyc_Tcutil_rgns_of(( void*) sf->type);}
static struct _tuple5* Cyc_Tcutil_region_free_subst( struct Cyc_Absyn_Tvar* tv){
void* t;{ void* _temp629= Cyc_Absyn_conref_val( tv->kind); _LL631: if( _temp629
== ( void*) Cyc_Absyn_RgnKind){ goto _LL632;} else{ goto _LL633;} _LL633: if(
_temp629 == ( void*) Cyc_Absyn_EffKind){ goto _LL634;} else{ goto _LL635;}
_LL635: goto _LL636; _LL632: t=( void*) Cyc_Absyn_HeapRgn; goto _LL630; _LL634:
t= Cyc_Absyn_empty_effect; goto _LL630; _LL636: t= Cyc_Absyn_sint_t; goto _LL630;
_LL630:;} return({ struct _tuple5* _temp637=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp637->f1= tv; _temp637->f2= t; _temp637;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp638= Cyc_Tcutil_compress(
t); void* _temp686; struct Cyc_Absyn_TunionInfo _temp688; void* _temp690; struct
Cyc_List_List* _temp692; struct Cyc_Absyn_PtrInfo _temp694; void* _temp696; void*
_temp698; void* _temp700; struct Cyc_List_List* _temp702; struct Cyc_Absyn_TunionFieldInfo
_temp704; struct Cyc_List_List* _temp706; struct Cyc_List_List* _temp708; struct
Cyc_List_List* _temp710; struct Cyc_List_List* _temp712; struct Cyc_List_List*
_temp714; struct Cyc_Absyn_FnInfo _temp716; struct Cyc_List_List* _temp718;
struct Cyc_Absyn_VarargInfo* _temp720; struct Cyc_List_List* _temp722; void*
_temp724; struct Cyc_Core_Opt* _temp726; struct Cyc_List_List* _temp728; void*
_temp730; _LL640: if( _temp638 == ( void*) Cyc_Absyn_VoidType){ goto _LL641;}
else{ goto _LL642;} _LL642: if( _temp638 == ( void*) Cyc_Absyn_FloatType){ goto
_LL643;} else{ goto _LL644;} _LL644: if( _temp638 == ( void*) Cyc_Absyn_DoubleType){
goto _LL645;} else{ goto _LL646;} _LL646: if(( unsigned int) _temp638 >  4u?*((
int*) _temp638) ==  Cyc_Absyn_EnumType: 0){ goto _LL647;} else{ goto _LL648;}
_LL648: if(( unsigned int) _temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_IntType:
0){ goto _LL649;} else{ goto _LL650;} _LL650: if(( unsigned int) _temp638 >  4u?*((
int*) _temp638) ==  Cyc_Absyn_Evar: 0){ goto _LL651;} else{ goto _LL652;} _LL652:
if(( unsigned int) _temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_VarType: 0){
goto _LL653;} else{ goto _LL654;} _LL654: if(( unsigned int) _temp638 >  4u?*((
int*) _temp638) ==  Cyc_Absyn_RgnHandleType: 0){ _LL687: _temp686=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp638)->f1; goto _LL655;} else{ goto
_LL656;} _LL656: if(( unsigned int) _temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_TunionType:
0){ _LL689: _temp688=(( struct Cyc_Absyn_TunionType_struct*) _temp638)->f1;
_LL693: _temp692= _temp688.targs; goto _LL691; _LL691: _temp690=( void*)
_temp688.rgn; goto _LL657;} else{ goto _LL658;} _LL658: if(( unsigned int)
_temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_PointerType: 0){ _LL695:
_temp694=(( struct Cyc_Absyn_PointerType_struct*) _temp638)->f1; _LL699:
_temp698=( void*) _temp694.elt_typ; goto _LL697; _LL697: _temp696=( void*)
_temp694.rgn_typ; goto _LL659;} else{ goto _LL660;} _LL660: if(( unsigned int)
_temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_ArrayType: 0){ _LL701: _temp700=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp638)->f1; goto _LL661;} else{
goto _LL662;} _LL662: if(( unsigned int) _temp638 >  4u?*(( int*) _temp638) == 
Cyc_Absyn_TupleType: 0){ _LL703: _temp702=(( struct Cyc_Absyn_TupleType_struct*)
_temp638)->f1; goto _LL663;} else{ goto _LL664;} _LL664: if(( unsigned int)
_temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_TunionFieldType: 0){ _LL705:
_temp704=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp638)->f1; _LL707:
_temp706= _temp704.targs; goto _LL665;} else{ goto _LL666;} _LL666: if((
unsigned int) _temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_StructType: 0){
_LL709: _temp708=(( struct Cyc_Absyn_StructType_struct*) _temp638)->f2; goto
_LL667;} else{ goto _LL668;} _LL668: if(( unsigned int) _temp638 >  4u?*(( int*)
_temp638) ==  Cyc_Absyn_UnionType: 0){ _LL711: _temp710=(( struct Cyc_Absyn_UnionType_struct*)
_temp638)->f2; goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int)
_temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_AnonStructType: 0){ _LL713:
_temp712=(( struct Cyc_Absyn_AnonStructType_struct*) _temp638)->f1; goto _LL671;}
else{ goto _LL672;} _LL672: if(( unsigned int) _temp638 >  4u?*(( int*) _temp638)
==  Cyc_Absyn_AnonUnionType: 0){ _LL715: _temp714=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp638)->f1; goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int)
_temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_FnType: 0){ _LL717: _temp716=((
struct Cyc_Absyn_FnType_struct*) _temp638)->f1; _LL729: _temp728= _temp716.tvars;
goto _LL727; _LL727: _temp726= _temp716.effect; goto _LL725; _LL725: _temp724=(
void*) _temp716.ret_typ; goto _LL723; _LL723: _temp722= _temp716.args; goto
_LL721; _LL721: _temp720= _temp716.cyc_varargs; goto _LL719; _LL719: _temp718=
_temp716.rgn_po; goto _LL675;} else{ goto _LL676;} _LL676: if( _temp638 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL677;} else{ goto _LL678;} _LL678: if((
unsigned int) _temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_AccessEff: 0){
goto _LL679;} else{ goto _LL680;} _LL680: if(( unsigned int) _temp638 >  4u?*((
int*) _temp638) ==  Cyc_Absyn_JoinEff: 0){ goto _LL681;} else{ goto _LL682;}
_LL682: if(( unsigned int) _temp638 >  4u?*(( int*) _temp638) ==  Cyc_Absyn_RgnsEff:
0){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp638)->f1;
goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int) _temp638 >  4u?*((
int*) _temp638) ==  Cyc_Absyn_TypedefType: 0){ goto _LL685;} else{ goto _LL639;}
_LL641: goto _LL643; _LL643: goto _LL645; _LL645: goto _LL647; _LL647: goto
_LL649; _LL649: return Cyc_Absyn_empty_effect; _LL651: goto _LL653; _LL653: {
void* _temp732= Cyc_Tcutil_typ_kind( t); _LL734: if( _temp732 == ( void*) Cyc_Absyn_RgnKind){
goto _LL735;} else{ goto _LL736;} _LL736: if( _temp732 == ( void*) Cyc_Absyn_EffKind){
goto _LL737;} else{ goto _LL738;} _LL738: goto _LL739; _LL735: return( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp740=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp740[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp741; _temp741.tag= Cyc_Absyn_AccessEff; _temp741.f1=(
void*) t; _temp741;}); _temp740;}); _LL737: return t; _LL739: return( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp742=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp742[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp743; _temp743.tag= Cyc_Absyn_RgnsEff; _temp743.f1=( void*) t; _temp743;});
_temp742;}); _LL733:;} _LL655: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp744=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp744[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp745; _temp745.tag= Cyc_Absyn_AccessEff;
_temp745.f1=( void*) _temp686; _temp745;}); _temp744;}); _LL657: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp748=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp748->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp749=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp749[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp750; _temp750.tag= Cyc_Absyn_AccessEff;
_temp750.f1=( void*) _temp690; _temp750;}); _temp749;})); _temp748->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp692); _temp748;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp746=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp746[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp747; _temp747.tag= Cyc_Absyn_JoinEff; _temp747.f1= ts; _temp747;});
_temp746;}));} _LL659: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp751=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp751[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp752; _temp752.tag= Cyc_Absyn_JoinEff;
_temp752.f1=({ void* _temp753[ 2u]; _temp753[ 1u]= Cyc_Tcutil_rgns_of( _temp698);
_temp753[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp754=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp754[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp755; _temp755.tag= Cyc_Absyn_AccessEff;
_temp755.f1=( void*) _temp696; _temp755;}); _temp754;}); Cyc_List_list( _tag_arr(
_temp753, sizeof( void*), 2u));}); _temp752;}); _temp751;})); _LL661: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp700)); _LL663: { struct Cyc_List_List* _temp756= 0;
for( 0; _temp702 !=  0; _temp702=(( struct Cyc_List_List*) _check_null( _temp702))->tl){
_temp756=({ struct Cyc_List_List* _temp757=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp757->hd=( void*)(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp702))->hd)).f2; _temp757->tl= _temp756;
_temp757;});} _temp706= _temp756; goto _LL665;} _LL665: _temp708= _temp706; goto
_LL667; _LL667: _temp710= _temp708; goto _LL669; _LL669: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp758=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp758[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp759; _temp759.tag= Cyc_Absyn_JoinEff; _temp759.f1= Cyc_List_map( Cyc_Tcutil_rgns_of,
_temp710); _temp759;}); _temp758;})); _LL671: _temp714= _temp712; goto _LL673;
_LL673: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp760=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp760[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp761; _temp761.tag= Cyc_Absyn_JoinEff;
_temp761.f1=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_rgns_of_field, _temp714);
_temp761;}); _temp760;})); _LL675: { void* _temp762= Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst, _temp728),( void*)((
struct Cyc_Core_Opt*) _check_null( _temp726))->v); return Cyc_Tcutil_normalize_effect(
_temp762);} _LL677: return Cyc_Absyn_empty_effect; _LL679: goto _LL681; _LL681:
return t; _LL683: return Cyc_Tcutil_rgns_of( _temp730); _LL685: return({ void*
_temp763[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp763, sizeof( void*), 0u));});
_LL639:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp764= e; struct Cyc_List_List* _temp772; struct Cyc_List_List**
_temp774; void* _temp775; _LL766: if(( unsigned int) _temp764 >  4u?*(( int*)
_temp764) ==  Cyc_Absyn_JoinEff: 0){ _LL773: _temp772=(( struct Cyc_Absyn_JoinEff_struct*)
_temp764)->f1; _temp774=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp764)->f1; goto _LL767;} else{ goto _LL768;} _LL768: if(( unsigned int)
_temp764 >  4u?*(( int*) _temp764) ==  Cyc_Absyn_RgnsEff: 0){ _LL776: _temp775=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp764)->f1; goto _LL769;} else{
goto _LL770;} _LL770: goto _LL771; _LL767: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp774; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp777=( void*)(( struct Cyc_List_List*) _check_null( effs))->hd;(
void*)((( struct Cyc_List_List*) _check_null( effs))->hd=( void*) Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect( _temp777)));{ void* _temp778=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; _LL780: if(( unsigned int) _temp778 >  4u?*(( int*)
_temp778) ==  Cyc_Absyn_JoinEff: 0){ goto _LL781;} else{ goto _LL782;} _LL782:
goto _LL783; _LL781: nested_join= 1; goto _LL779; _LL783: goto _LL779; _LL779:;}}}
if( ! nested_join){ return e;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp774; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp784= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp792; void* _temp794; _LL786:
if(( unsigned int) _temp784 >  4u?*(( int*) _temp784) ==  Cyc_Absyn_JoinEff: 0){
_LL793: _temp792=(( struct Cyc_Absyn_JoinEff_struct*) _temp784)->f1; goto _LL787;}
else{ goto _LL788;} _LL788: if(( unsigned int) _temp784 >  4u?*(( int*) _temp784)
==  Cyc_Absyn_AccessEff: 0){ _LL795: _temp794=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp784)->f1; if( _temp794 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL789;} else{
goto _LL790;}} else{ goto _LL790;} _LL790: goto _LL791; _LL787: effects= Cyc_List_revappend(
_temp792, effects); goto _LL785; _LL789: goto _LL785; _LL791: effects=({ struct
Cyc_List_List* _temp796=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp796->hd=( void*) _temp784; _temp796->tl= effects; _temp796;}); goto _LL785;
_LL785:;}}* _temp774= Cyc_List_imp_rev( effects); return e;}} _LL769: return Cyc_Tcutil_rgns_of(
_temp775); _LL771: return e; _LL765:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp797= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp805; struct Cyc_List_List _temp807; struct Cyc_List_List* _temp808; void*
_temp810; struct Cyc_Core_Opt* _temp812; struct Cyc_Core_Opt* _temp814; _LL799:
if(( unsigned int) _temp797 >  4u?*(( int*) _temp797) ==  Cyc_Absyn_JoinEff: 0){
_LL806: _temp805=(( struct Cyc_Absyn_JoinEff_struct*) _temp797)->f1; if(
_temp805 ==  0){ goto _LL801;} else{ _temp807=* _temp805; _LL811: _temp810=(
void*) _temp807.hd; goto _LL809; _LL809: _temp808= _temp807.tl; goto _LL800;}}
else{ goto _LL801;} _LL801: if(( unsigned int) _temp797 >  4u?*(( int*) _temp797)
==  Cyc_Absyn_Evar: 0){ _LL815: _temp814=(( struct Cyc_Absyn_Evar_struct*)
_temp797)->f1; goto _LL813; _LL813: _temp812=(( struct Cyc_Absyn_Evar_struct*)
_temp797)->f4; goto _LL802;} else{ goto _LL803;} _LL803: goto _LL804; _LL800: {
void* _temp816= Cyc_Tcutil_compress( _temp810); struct Cyc_Core_Opt* _temp822;
_LL818: if(( unsigned int) _temp816 >  4u?*(( int*) _temp816) ==  Cyc_Absyn_Evar:
0){ _LL823: _temp822=(( struct Cyc_Absyn_Evar_struct*) _temp816)->f4; goto
_LL819;} else{ goto _LL820;} _LL820: goto _LL821; _LL819: return({ struct
_tuple9* _temp824=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp824->f1= _temp810; _temp824->f2= _temp808; _temp824->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp822))->v; _temp824;}); _LL821: return 0;
_LL817:;} _LL802: if( _temp814 ==  0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp814))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp825[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp825, sizeof( void*), 0u));});} return({ struct _tuple9* _temp826=( struct
_tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp826->f1= t; _temp826->f2= 0;
_temp826->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp812))->v; _temp826;}); _LL804: return 0; _LL798:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp827=({ struct Cyc_Absyn_FnType_struct*
_temp828=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp828[ 0]=({ struct Cyc_Absyn_FnType_struct _temp829; _temp829.tag= Cyc_Absyn_FnType;
_temp829.f1=({ struct Cyc_Absyn_FnInfo _temp830; _temp830.tvars= 0; _temp830.effect=({
struct Cyc_Core_Opt* _temp831=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp831->v=( void*) eff; _temp831;}); _temp830.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp830.args= 0; _temp830.c_varargs= 0; _temp830.cyc_varargs=
0; _temp830.rgn_po= 0; _temp830.attributes= 0; _temp830;}); _temp829;});
_temp828;}); return Cyc_Absyn_atb_typ(( void*) _temp827,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp832= Cyc_Tcutil_compress( e); void*
_temp844; struct Cyc_List_List* _temp846; void* _temp848; struct Cyc_Core_Opt*
_temp850; struct Cyc_Core_Opt* _temp852; struct Cyc_Core_Opt** _temp854; struct
Cyc_Core_Opt* _temp855; _LL834: if(( unsigned int) _temp832 >  4u?*(( int*)
_temp832) ==  Cyc_Absyn_AccessEff: 0){ _LL845: _temp844=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp832)->f1; goto _LL835;} else{ goto _LL836;} _LL836: if(( unsigned int)
_temp832 >  4u?*(( int*) _temp832) ==  Cyc_Absyn_JoinEff: 0){ _LL847: _temp846=((
struct Cyc_Absyn_JoinEff_struct*) _temp832)->f1; goto _LL837;} else{ goto _LL838;}
_LL838: if(( unsigned int) _temp832 >  4u?*(( int*) _temp832) ==  Cyc_Absyn_RgnsEff:
0){ _LL849: _temp848=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp832)->f1;
goto _LL839;} else{ goto _LL840;} _LL840: if(( unsigned int) _temp832 >  4u?*((
int*) _temp832) ==  Cyc_Absyn_Evar: 0){ _LL856: _temp855=(( struct Cyc_Absyn_Evar_struct*)
_temp832)->f1; goto _LL853; _LL853: _temp852=(( struct Cyc_Absyn_Evar_struct*)
_temp832)->f2; _temp854=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp832)->f2; goto _LL851; _LL851: _temp850=(( struct Cyc_Absyn_Evar_struct*)
_temp832)->f4; goto _LL841;} else{ goto _LL842;} _LL842: goto _LL843; _LL835:
if( constrain){ return Cyc_Tcutil_unify( r, _temp844);} _temp844= Cyc_Tcutil_compress(
_temp844); if( r ==  _temp844){ return 1;}{ struct _tuple8 _temp858=({ struct
_tuple8 _temp857; _temp857.f1= r; _temp857.f2= _temp844; _temp857;}); void*
_temp864; struct Cyc_Absyn_Tvar* _temp866; void* _temp868; struct Cyc_Absyn_Tvar*
_temp870; _LL860: _LL869: _temp868= _temp858.f1; if(( unsigned int) _temp868 > 
4u?*(( int*) _temp868) ==  Cyc_Absyn_VarType: 0){ _LL871: _temp870=(( struct Cyc_Absyn_VarType_struct*)
_temp868)->f1; goto _LL865;} else{ goto _LL862;} _LL865: _temp864= _temp858.f2;
if(( unsigned int) _temp864 >  4u?*(( int*) _temp864) ==  Cyc_Absyn_VarType: 0){
_LL867: _temp866=(( struct Cyc_Absyn_VarType_struct*) _temp864)->f1; goto _LL861;}
else{ goto _LL862;} _LL862: goto _LL863; _LL861: return Cyc_Absyn_tvar_cmp(
_temp870, _temp866) ==  0; _LL863: return 0; _LL859:;} _LL837: for( 0; _temp846
!=  0; _temp846=(( struct Cyc_List_List*) _check_null( _temp846))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp846))->hd)){
return 1;}} return 0; _LL839: { void* _temp872= Cyc_Tcutil_rgns_of( _temp848);
void* _temp878; _LL874: if(( unsigned int) _temp872 >  4u?*(( int*) _temp872) == 
Cyc_Absyn_RgnsEff: 0){ _LL879: _temp878=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp872)->f1; goto _LL875;} else{ goto _LL876;} _LL876: goto _LL877; _LL875:
if( ! constrain){ return 0;}{ void* _temp880= Cyc_Tcutil_compress( _temp878);
struct Cyc_Core_Opt* _temp886; struct Cyc_Core_Opt* _temp888; struct Cyc_Core_Opt**
_temp890; struct Cyc_Core_Opt* _temp891; _LL882: if(( unsigned int) _temp880 > 
4u?*(( int*) _temp880) ==  Cyc_Absyn_Evar: 0){ _LL892: _temp891=(( struct Cyc_Absyn_Evar_struct*)
_temp880)->f1; goto _LL889; _LL889: _temp888=(( struct Cyc_Absyn_Evar_struct*)
_temp880)->f2; _temp890=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp880)->f2; goto _LL887; _LL887: _temp886=(( struct Cyc_Absyn_Evar_struct*)
_temp880)->f4; goto _LL883;} else{ goto _LL884;} _LL884: goto _LL885; _LL883: {
void* _temp893= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp886); Cyc_Tcutil_occurs( _temp893,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp886))->v, r);{ void* _temp894= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp896=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp896[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp897; _temp897.tag= Cyc_Absyn_JoinEff; _temp897.f1=({ void* _temp898[ 2u];
_temp898[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp899=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp899[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp900; _temp900.tag= Cyc_Absyn_AccessEff;
_temp900.f1=( void*) r; _temp900;}); _temp899;}); _temp898[ 0u]= _temp893; Cyc_List_list(
_tag_arr( _temp898, sizeof( void*), 2u));}); _temp897;}); _temp896;}));*
_temp890=({ struct Cyc_Core_Opt* _temp895=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp895->v=( void*) _temp894; _temp895;});
return 1;}} _LL885: return 0; _LL881:;} _LL877: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp872); _LL873:;} _LL841: if( _temp855 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp855))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp901[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp901, sizeof( void*), 0u));});} if( ! constrain){ return 0;}{
void* _temp902= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp850); Cyc_Tcutil_occurs( _temp902,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp850))->v, r);{ struct Cyc_Absyn_JoinEff_struct* _temp903=({
struct Cyc_Absyn_JoinEff_struct* _temp905=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp905[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp906; _temp906.tag= Cyc_Absyn_JoinEff; _temp906.f1=({ struct Cyc_List_List*
_temp907=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp907->hd=( void*) _temp902; _temp907->tl=({ struct Cyc_List_List* _temp908=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp908->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp909=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp909[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp910; _temp910.tag= Cyc_Absyn_AccessEff; _temp910.f1=(
void*) r; _temp910;}); _temp909;})); _temp908->tl= 0; _temp908;}); _temp907;});
_temp906;}); _temp905;});* _temp854=({ struct Cyc_Core_Opt* _temp904=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp904->v=( void*)((
void*) _temp903); _temp904;}); return 1;}} _LL843: return 0; _LL833:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp911= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp923;
void* _temp925; struct Cyc_Core_Opt* _temp927; struct Cyc_Core_Opt* _temp929;
struct Cyc_Core_Opt** _temp931; struct Cyc_Core_Opt* _temp932; _LL913: if((
unsigned int) _temp911 >  4u?*(( int*) _temp911) ==  Cyc_Absyn_AccessEff: 0){
goto _LL914;} else{ goto _LL915;} _LL915: if(( unsigned int) _temp911 >  4u?*((
int*) _temp911) ==  Cyc_Absyn_JoinEff: 0){ _LL924: _temp923=(( struct Cyc_Absyn_JoinEff_struct*)
_temp911)->f1; goto _LL916;} else{ goto _LL917;} _LL917: if(( unsigned int)
_temp911 >  4u?*(( int*) _temp911) ==  Cyc_Absyn_RgnsEff: 0){ _LL926: _temp925=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp911)->f1; goto _LL918;} else{
goto _LL919;} _LL919: if(( unsigned int) _temp911 >  4u?*(( int*) _temp911) == 
Cyc_Absyn_Evar: 0){ _LL933: _temp932=(( struct Cyc_Absyn_Evar_struct*) _temp911)->f1;
goto _LL930; _LL930: _temp929=(( struct Cyc_Absyn_Evar_struct*) _temp911)->f2;
_temp931=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp911)->f2;
goto _LL928; _LL928: _temp927=(( struct Cyc_Absyn_Evar_struct*) _temp911)->f4;
goto _LL920;} else{ goto _LL921;} _LL921: goto _LL922; _LL914: return 0; _LL916:
for( 0; _temp923 !=  0; _temp923=(( struct Cyc_List_List*) _check_null( _temp923))->tl){
if( Cyc_Tcutil_type_in_effect( constrain, t,( void*)(( struct Cyc_List_List*)
_check_null( _temp923))->hd)){ return 1;}} return 0; _LL918: _temp925= Cyc_Tcutil_compress(
_temp925); if( t ==  _temp925){ return 1;} if( constrain){ return Cyc_Tcutil_unify(
t, _temp925);}{ void* _temp934= Cyc_Tcutil_rgns_of( t); void* _temp940; _LL936:
if(( unsigned int) _temp934 >  4u?*(( int*) _temp934) ==  Cyc_Absyn_RgnsEff: 0){
_LL941: _temp940=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp934)->f1;
goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937: { struct _tuple8
_temp943=({ struct _tuple8 _temp942; _temp942.f1= t; _temp942.f2= Cyc_Tcutil_compress(
_temp940); _temp942;}); void* _temp949; struct Cyc_Absyn_Tvar* _temp951; void*
_temp953; struct Cyc_Absyn_Tvar* _temp955; _LL945: _LL954: _temp953= _temp943.f1;
if(( unsigned int) _temp953 >  4u?*(( int*) _temp953) ==  Cyc_Absyn_VarType: 0){
_LL956: _temp955=(( struct Cyc_Absyn_VarType_struct*) _temp953)->f1; goto _LL950;}
else{ goto _LL947;} _LL950: _temp949= _temp943.f2; if(( unsigned int) _temp949 > 
4u?*(( int*) _temp949) ==  Cyc_Absyn_VarType: 0){ _LL952: _temp951=(( struct Cyc_Absyn_VarType_struct*)
_temp949)->f1; goto _LL946;} else{ goto _LL947;} _LL947: goto _LL948; _LL946:
return Cyc_Tcutil_unify( t, _temp940); _LL948: return t ==  _temp940; _LL944:;}
_LL939: return Cyc_Tcutil_type_in_effect( constrain, t, _temp934); _LL935:;}
_LL920: if( _temp932 ==  0? 1:( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp932))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp957[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp957, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp958= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp927); Cyc_Tcutil_occurs( _temp958,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp927))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp959=({ struct Cyc_Absyn_JoinEff_struct* _temp961=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp961[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp962; _temp962.tag= Cyc_Absyn_JoinEff; _temp962.f1=({ struct Cyc_List_List*
_temp963=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp963->hd=( void*) _temp958; _temp963->tl=({ struct Cyc_List_List* _temp964=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp964->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp965=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp965[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp966; _temp966.tag= Cyc_Absyn_RgnsEff; _temp966.f1=( void*) t; _temp966;});
_temp965;})); _temp964->tl= 0; _temp964;}); _temp963;}); _temp962;}); _temp961;});*
_temp931=({ struct Cyc_Core_Opt* _temp960=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp960->v=( void*)(( void*) _temp959); _temp960;});
return 1;}} _LL922: return 0; _LL912:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp967= e; struct Cyc_Absyn_Tvar* _temp979; struct Cyc_List_List*
_temp981; void* _temp983; struct Cyc_Core_Opt* _temp985; struct Cyc_Core_Opt*
_temp987; struct Cyc_Core_Opt** _temp989; struct Cyc_Core_Opt* _temp990; _LL969:
if(( unsigned int) _temp967 >  4u?*(( int*) _temp967) ==  Cyc_Absyn_VarType: 0){
_LL980: _temp979=(( struct Cyc_Absyn_VarType_struct*) _temp967)->f1; goto _LL970;}
else{ goto _LL971;} _LL971: if(( unsigned int) _temp967 >  4u?*(( int*) _temp967)
==  Cyc_Absyn_JoinEff: 0){ _LL982: _temp981=(( struct Cyc_Absyn_JoinEff_struct*)
_temp967)->f1; goto _LL972;} else{ goto _LL973;} _LL973: if(( unsigned int)
_temp967 >  4u?*(( int*) _temp967) ==  Cyc_Absyn_RgnsEff: 0){ _LL984: _temp983=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp967)->f1; goto _LL974;} else{
goto _LL975;} _LL975: if(( unsigned int) _temp967 >  4u?*(( int*) _temp967) == 
Cyc_Absyn_Evar: 0){ _LL991: _temp990=(( struct Cyc_Absyn_Evar_struct*) _temp967)->f1;
goto _LL988; _LL988: _temp987=(( struct Cyc_Absyn_Evar_struct*) _temp967)->f2;
_temp989=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp967)->f2;
goto _LL986; _LL986: _temp985=(( struct Cyc_Absyn_Evar_struct*) _temp967)->f4;
goto _LL976;} else{ goto _LL977;} _LL977: goto _LL978; _LL970: return Cyc_Absyn_tvar_cmp(
v, _temp979) ==  0; _LL972: for( 0; _temp981 !=  0; _temp981=(( struct Cyc_List_List*)
_check_null( _temp981))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp981))->hd)){ return 1;}}
return 0; _LL974: { void* _temp992= Cyc_Tcutil_rgns_of( _temp983); void*
_temp998; _LL994: if(( unsigned int) _temp992 >  4u?*(( int*) _temp992) ==  Cyc_Absyn_RgnsEff:
0){ _LL999: _temp998=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp992)->f1;
goto _LL995;} else{ goto _LL996;} _LL996: goto _LL997; _LL995: if( ! constrain){
return 0;}{ void* _temp1000= Cyc_Tcutil_compress( _temp998); struct Cyc_Core_Opt*
_temp1006; struct Cyc_Core_Opt* _temp1008; struct Cyc_Core_Opt** _temp1010;
struct Cyc_Core_Opt* _temp1011; _LL1002: if(( unsigned int) _temp1000 >  4u?*((
int*) _temp1000) ==  Cyc_Absyn_Evar: 0){ _LL1012: _temp1011=(( struct Cyc_Absyn_Evar_struct*)
_temp1000)->f1; goto _LL1009; _LL1009: _temp1008=(( struct Cyc_Absyn_Evar_struct*)
_temp1000)->f2; _temp1010=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1000)->f2; goto _LL1007; _LL1007: _temp1006=(( struct Cyc_Absyn_Evar_struct*)
_temp1000)->f4; goto _LL1003;} else{ goto _LL1004;} _LL1004: goto _LL1005;
_LL1003: { void* _temp1013= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp1006); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1006))->v, v)){
return 0;}{ void* _temp1014= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1016=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1016[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1017; _temp1017.tag= Cyc_Absyn_JoinEff;
_temp1017.f1=({ void* _temp1018[ 2u]; _temp1018[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1019=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1019[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1020; _temp1020.tag= Cyc_Absyn_VarType;
_temp1020.f1= v; _temp1020;}); _temp1019;}); _temp1018[ 0u]= _temp1013; Cyc_List_list(
_tag_arr( _temp1018, sizeof( void*), 2u));}); _temp1017;}); _temp1016;}));*
_temp1010=({ struct Cyc_Core_Opt* _temp1015=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1015->v=( void*) _temp1014; _temp1015;});
return 1;}} _LL1005: return 0; _LL1001:;} _LL997: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp992); _LL993:;} _LL976: if( _temp990 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp990))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp1021[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp1021, sizeof( void*), 0u));});}{ void* _temp1022= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp985); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp985))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp1023=({ struct Cyc_Absyn_JoinEff_struct* _temp1025=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1025[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1026; _temp1026.tag= Cyc_Absyn_JoinEff; _temp1026.f1=({
struct Cyc_List_List* _temp1027=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1027->hd=( void*) _temp1022; _temp1027->tl=({
struct Cyc_List_List* _temp1028=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1028->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1029=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1029[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1030; _temp1030.tag= Cyc_Absyn_VarType;
_temp1030.f1= v; _temp1030;}); _temp1029;})); _temp1028->tl= 0; _temp1028;});
_temp1027;}); _temp1026;}); _temp1025;});* _temp989=({ struct Cyc_Core_Opt*
_temp1024=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1024->v=( void*)(( void*) _temp1023); _temp1024;}); return 1;}} _LL978:
return 0; _LL968:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp1031= e; struct Cyc_List_List* _temp1041;
void* _temp1043; _LL1033: if(( unsigned int) _temp1031 >  4u?*(( int*) _temp1031)
==  Cyc_Absyn_JoinEff: 0){ _LL1042: _temp1041=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1031)->f1; goto _LL1034;} else{ goto _LL1035;} _LL1035: if(( unsigned int)
_temp1031 >  4u?*(( int*) _temp1031) ==  Cyc_Absyn_RgnsEff: 0){ _LL1044:
_temp1043=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1031)->f1; goto
_LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int) _temp1031 >  4u?*((
int*) _temp1031) ==  Cyc_Absyn_Evar: 0){ goto _LL1038;} else{ goto _LL1039;}
_LL1039: goto _LL1040; _LL1034: for( 0; _temp1041 !=  0; _temp1041=(( struct Cyc_List_List*)
_check_null( _temp1041))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp1041))->hd)){ return 1;}} return 0;
_LL1036: { void* _temp1045= Cyc_Tcutil_rgns_of( _temp1043); void* _temp1051;
_LL1047: if(( unsigned int) _temp1045 >  4u?*(( int*) _temp1045) ==  Cyc_Absyn_RgnsEff:
0){ _LL1052: _temp1051=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1045)->f1;
goto _LL1048;} else{ goto _LL1049;} _LL1049: goto _LL1050; _LL1048: return 0;
_LL1050: return Cyc_Tcutil_evar_in_effect( evar, _temp1045); _LL1046:;} _LL1038:
return evar ==  e; _LL1040: return 0; _LL1032:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1053= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1067; void* _temp1069; struct Cyc_Absyn_Tvar*
_temp1071; void* _temp1073; struct Cyc_Core_Opt* _temp1075; struct Cyc_Core_Opt*
_temp1077; struct Cyc_Core_Opt** _temp1079; _LL1055: if(( unsigned int)
_temp1053 >  4u?*(( int*) _temp1053) ==  Cyc_Absyn_JoinEff: 0){ _LL1068:
_temp1067=(( struct Cyc_Absyn_JoinEff_struct*) _temp1053)->f1; goto _LL1056;}
else{ goto _LL1057;} _LL1057: if(( unsigned int) _temp1053 >  4u?*(( int*)
_temp1053) ==  Cyc_Absyn_AccessEff: 0){ _LL1070: _temp1069=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1053)->f1; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(( unsigned int)
_temp1053 >  4u?*(( int*) _temp1053) ==  Cyc_Absyn_VarType: 0){ _LL1072:
_temp1071=(( struct Cyc_Absyn_VarType_struct*) _temp1053)->f1; goto _LL1060;}
else{ goto _LL1061;} _LL1061: if(( unsigned int) _temp1053 >  4u?*(( int*)
_temp1053) ==  Cyc_Absyn_RgnsEff: 0){ _LL1074: _temp1073=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1053)->f1; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(( unsigned int)
_temp1053 >  4u?*(( int*) _temp1053) ==  Cyc_Absyn_Evar: 0){ _LL1078: _temp1077=((
struct Cyc_Absyn_Evar_struct*) _temp1053)->f2; _temp1079=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1053)->f2; goto _LL1076; _LL1076: _temp1075=((
struct Cyc_Absyn_Evar_struct*) _temp1053)->f4; goto _LL1064;} else{ goto _LL1065;}
_LL1065: goto _LL1066; _LL1056: for( 0; _temp1067 !=  0; _temp1067=(( struct Cyc_List_List*)
_check_null( _temp1067))->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*)(( struct Cyc_List_List*) _check_null( _temp1067))->hd, e2)){ return 0;}}
return 1; _LL1058: return( Cyc_Tcutil_region_in_effect( 0, _temp1069, e2)? 1:
Cyc_Tcutil_region_in_effect( 1, _temp1069, e2))? 1: Cyc_Tcutil_unify( _temp1069,(
void*) Cyc_Absyn_HeapRgn); _LL1060: return Cyc_Tcutil_variable_in_effect( 0,
_temp1071, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1071, e2); _LL1062: {
void* _temp1080= Cyc_Tcutil_rgns_of( _temp1073); void* _temp1086; _LL1082: if((
unsigned int) _temp1080 >  4u?*(( int*) _temp1080) ==  Cyc_Absyn_RgnsEff: 0){
_LL1087: _temp1086=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1080)->f1;
goto _LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085; _LL1083: return( Cyc_Tcutil_type_in_effect(
0, _temp1086, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1086, e2))? 1: Cyc_Tcutil_unify(
_temp1086, Cyc_Absyn_sint_t); _LL1085: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1080, e2); _LL1081:;} _LL1064: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1079=({ struct Cyc_Core_Opt* _temp1088=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1088->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1089=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1089[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1090; _temp1090.tag= Cyc_Absyn_JoinEff; _temp1090.f1=
0; _temp1090;}); _temp1089;})); _temp1088;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1075))->v, e2);*
_temp1079=({ struct Cyc_Core_Opt* _temp1091=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1091->v=( void*) e2; _temp1091;});}} return
1; _LL1066: return({ struct Cyc_Std_String_pa_struct _temp1093; _temp1093.tag=
Cyc_Std_String_pa; _temp1093.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1092[ 1u]={& _temp1093};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1092, sizeof( void*), 1u));}});
_LL1054:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1095=({ struct
_tuple10 _temp1094; _temp1094.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1094.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1094;}); _LL1097: goto _LL1098; _LL1098:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1096:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8 _temp1101;
void* _temp1102; void* _temp1104; struct _tuple8* _temp1099=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( r1))->hd; _temp1101=* _temp1099; _LL1105:
_temp1104= _temp1101.f1; goto _LL1103; _LL1103: _temp1102= _temp1101.f2; goto
_LL1100; _LL1100: { int found= _temp1104 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2=(( struct Cyc_List_List*)
_check_null( r2))->tl){ struct _tuple8 _temp1108; void* _temp1109; void*
_temp1111; struct _tuple8* _temp1106=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( r2))->hd; _temp1108=* _temp1106; _LL1112: _temp1111= _temp1108.f1;
goto _LL1110; _LL1110: _temp1109= _temp1108.f2; goto _LL1107; _LL1107: if( Cyc_Tcutil_unify(
_temp1104, _temp1111)? Cyc_Tcutil_unify( _temp1102, _temp1109): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 ==  t2){
return;}{ void* _temp1113= t1; struct Cyc_Core_Opt* _temp1119; struct Cyc_Core_Opt*
_temp1121; struct Cyc_Core_Opt** _temp1123; struct Cyc_Core_Opt* _temp1124;
_LL1115: if(( unsigned int) _temp1113 >  4u?*(( int*) _temp1113) ==  Cyc_Absyn_Evar:
0){ _LL1125: _temp1124=(( struct Cyc_Absyn_Evar_struct*) _temp1113)->f1; goto
_LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_Evar_struct*) _temp1113)->f2;
_temp1123=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1113)->f2;
goto _LL1120; _LL1120: _temp1119=(( struct Cyc_Absyn_Evar_struct*) _temp1113)->f4;
goto _LL1116;} else{ goto _LL1117;} _LL1117: goto _LL1118; _LL1116: Cyc_Tcutil_occurs(
t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1119))->v,
t2);{ void* _temp1126= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq(
_temp1126,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1124))->v)){*
_temp1123=({ struct Cyc_Core_Opt* _temp1127=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1127->v=( void*) t2; _temp1127;}); return;}
else{{ void* _temp1128= t2; struct Cyc_Core_Opt* _temp1136; struct Cyc_Core_Opt*
_temp1138; struct Cyc_Core_Opt** _temp1140; struct Cyc_Absyn_PtrInfo _temp1142;
_LL1130: if(( unsigned int) _temp1128 >  4u?*(( int*) _temp1128) ==  Cyc_Absyn_Evar:
0){ _LL1139: _temp1138=(( struct Cyc_Absyn_Evar_struct*) _temp1128)->f2;
_temp1140=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1128)->f2;
goto _LL1137; _LL1137: _temp1136=(( struct Cyc_Absyn_Evar_struct*) _temp1128)->f4;
goto _LL1131;} else{ goto _LL1132;} _LL1132: if(( unsigned int) _temp1128 >  4u?*((
int*) _temp1128) ==  Cyc_Absyn_PointerType: 0){ _LL1143: _temp1142=(( struct Cyc_Absyn_PointerType_struct*)
_temp1128)->f1; goto _LL1141;} else{ goto _LL1134;} _LL1141: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1124))->v == ( void*) Cyc_Absyn_BoxKind){
goto _LL1133;} else{ goto _LL1134;} _LL1134: goto _LL1135; _LL1131: { struct Cyc_List_List*
_temp1144=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1119))->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1136))->v; for( 0; s2 !=  0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1144,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1124))->v, _temp1126)){*
_temp1140=({ struct Cyc_Core_Opt* _temp1145=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1145->v=( void*) t1; _temp1145;}); return;}
goto _LL1129;} _LL1133: { struct Cyc_Absyn_Conref* _temp1146= Cyc_Absyn_compress_conref(
_temp1142.bounds);{ void* _temp1147=( void*) _temp1146->v; _LL1149: if(
_temp1147 == ( void*) Cyc_Absyn_No_constr){ goto _LL1150;} else{ goto _LL1151;}
_LL1151: goto _LL1152; _LL1150:( void*)( _temp1146->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp1153=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1153[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1154; _temp1154.tag= Cyc_Absyn_Eq_constr;
_temp1154.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1155=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1155[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1156; _temp1156.tag= Cyc_Absyn_Upper_b;
_temp1156.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1156;}); _temp1155;}));
_temp1154;}); _temp1153;})));* _temp1123=({ struct Cyc_Core_Opt* _temp1157=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1157->v=(
void*) t2; _temp1157;}); return; _LL1152: goto _LL1148; _LL1148:;} goto _LL1129;}
_LL1135: goto _LL1129; _LL1129:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}}
_LL1118: goto _LL1114; _LL1114:;}{ struct _tuple8 _temp1159=({ struct _tuple8
_temp1158; _temp1158.f1= t2; _temp1158.f2= t1; _temp1158;}); void* _temp1213;
void* _temp1215; void* _temp1217; void* _temp1219; struct Cyc_Absyn_Tvar*
_temp1221; void* _temp1223; struct Cyc_Absyn_Tvar* _temp1225; void* _temp1227;
struct Cyc_Absyn_Structdecl** _temp1229; struct Cyc_List_List* _temp1231; struct
_tuple1* _temp1233; void* _temp1235; struct Cyc_Absyn_Structdecl** _temp1237;
struct Cyc_List_List* _temp1239; struct _tuple1* _temp1241; void* _temp1243;
struct _tuple1* _temp1245; void* _temp1247; struct _tuple1* _temp1249; void*
_temp1251; struct Cyc_Absyn_Uniondecl** _temp1253; struct Cyc_List_List*
_temp1255; struct _tuple1* _temp1257; void* _temp1259; struct Cyc_Absyn_Uniondecl**
_temp1261; struct Cyc_List_List* _temp1263; struct _tuple1* _temp1265; void*
_temp1267; struct Cyc_Absyn_TunionInfo _temp1269; void* _temp1271; struct Cyc_List_List*
_temp1273; void* _temp1275; struct Cyc_Absyn_Tuniondecl* _temp1277; void*
_temp1279; struct Cyc_Absyn_TunionInfo _temp1281; void* _temp1283; struct Cyc_List_List*
_temp1285; void* _temp1287; struct Cyc_Absyn_Tuniondecl* _temp1289; void*
_temp1291; struct Cyc_Absyn_TunionFieldInfo _temp1293; struct Cyc_List_List*
_temp1295; void* _temp1297; struct Cyc_Absyn_Tunionfield* _temp1299; struct Cyc_Absyn_Tuniondecl*
_temp1301; void* _temp1303; struct Cyc_Absyn_TunionFieldInfo _temp1305; struct
Cyc_List_List* _temp1307; void* _temp1309; struct Cyc_Absyn_Tunionfield*
_temp1311; struct Cyc_Absyn_Tuniondecl* _temp1313; void* _temp1315; struct Cyc_Absyn_PtrInfo
_temp1317; struct Cyc_Absyn_Conref* _temp1319; struct Cyc_Absyn_Tqual _temp1321;
struct Cyc_Absyn_Conref* _temp1323; void* _temp1325; void* _temp1327; void*
_temp1329; struct Cyc_Absyn_PtrInfo _temp1331; struct Cyc_Absyn_Conref*
_temp1333; struct Cyc_Absyn_Tqual _temp1335; struct Cyc_Absyn_Conref* _temp1337;
void* _temp1339; void* _temp1341; void* _temp1343; void* _temp1345; void*
_temp1347; void* _temp1349; void* _temp1351; void* _temp1353; void* _temp1355;
void* _temp1357; void* _temp1359; void* _temp1361; void* _temp1363; struct Cyc_Absyn_Exp*
_temp1365; struct Cyc_Absyn_Tqual _temp1367; void* _temp1369; void* _temp1371;
struct Cyc_Absyn_Exp* _temp1373; struct Cyc_Absyn_Tqual _temp1375; void*
_temp1377; void* _temp1379; struct Cyc_Absyn_FnInfo _temp1381; struct Cyc_List_List*
_temp1383; struct Cyc_List_List* _temp1385; struct Cyc_Absyn_VarargInfo*
_temp1387; int _temp1389; struct Cyc_List_List* _temp1391; void* _temp1393;
struct Cyc_Core_Opt* _temp1395; struct Cyc_List_List* _temp1397; void* _temp1399;
struct Cyc_Absyn_FnInfo _temp1401; struct Cyc_List_List* _temp1403; struct Cyc_List_List*
_temp1405; struct Cyc_Absyn_VarargInfo* _temp1407; int _temp1409; struct Cyc_List_List*
_temp1411; void* _temp1413; struct Cyc_Core_Opt* _temp1415; struct Cyc_List_List*
_temp1417; void* _temp1419; struct Cyc_List_List* _temp1421; void* _temp1423;
struct Cyc_List_List* _temp1425; void* _temp1427; struct Cyc_List_List*
_temp1429; void* _temp1431; struct Cyc_List_List* _temp1433; void* _temp1435;
struct Cyc_List_List* _temp1437; void* _temp1439; struct Cyc_List_List*
_temp1441; void* _temp1443; void* _temp1445; void* _temp1447; void* _temp1449;
void* _temp1451; void* _temp1453; void* _temp1455; void* _temp1457; void*
_temp1459; void* _temp1461; void* _temp1463; void* _temp1465; _LL1161: _LL1214:
_temp1213= _temp1159.f1; if(( unsigned int) _temp1213 >  4u?*(( int*) _temp1213)
==  Cyc_Absyn_Evar: 0){ goto _LL1162;} else{ goto _LL1163;} _LL1163: _LL1218:
_temp1217= _temp1159.f1; if( _temp1217 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1216;} else{ goto _LL1165;} _LL1216: _temp1215= _temp1159.f2; if( _temp1215
== ( void*) Cyc_Absyn_VoidType){ goto _LL1164;} else{ goto _LL1165;} _LL1165:
_LL1224: _temp1223= _temp1159.f1; if(( unsigned int) _temp1223 >  4u?*(( int*)
_temp1223) ==  Cyc_Absyn_VarType: 0){ _LL1226: _temp1225=(( struct Cyc_Absyn_VarType_struct*)
_temp1223)->f1; goto _LL1220;} else{ goto _LL1167;} _LL1220: _temp1219=
_temp1159.f2; if(( unsigned int) _temp1219 >  4u?*(( int*) _temp1219) ==  Cyc_Absyn_VarType:
0){ _LL1222: _temp1221=(( struct Cyc_Absyn_VarType_struct*) _temp1219)->f1; goto
_LL1166;} else{ goto _LL1167;} _LL1167: _LL1236: _temp1235= _temp1159.f1; if((
unsigned int) _temp1235 >  4u?*(( int*) _temp1235) ==  Cyc_Absyn_StructType: 0){
_LL1242: _temp1241=(( struct Cyc_Absyn_StructType_struct*) _temp1235)->f1; goto
_LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_StructType_struct*) _temp1235)->f2;
goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_StructType_struct*)
_temp1235)->f3; goto _LL1228;} else{ goto _LL1169;} _LL1228: _temp1227=
_temp1159.f2; if(( unsigned int) _temp1227 >  4u?*(( int*) _temp1227) ==  Cyc_Absyn_StructType:
0){ _LL1234: _temp1233=(( struct Cyc_Absyn_StructType_struct*) _temp1227)->f1;
goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_StructType_struct*)
_temp1227)->f2; goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_StructType_struct*)
_temp1227)->f3; goto _LL1168;} else{ goto _LL1169;} _LL1169: _LL1248: _temp1247=
_temp1159.f1; if(( unsigned int) _temp1247 >  4u?*(( int*) _temp1247) ==  Cyc_Absyn_EnumType:
0){ _LL1250: _temp1249=(( struct Cyc_Absyn_EnumType_struct*) _temp1247)->f1;
goto _LL1244;} else{ goto _LL1171;} _LL1244: _temp1243= _temp1159.f2; if((
unsigned int) _temp1243 >  4u?*(( int*) _temp1243) ==  Cyc_Absyn_EnumType: 0){
_LL1246: _temp1245=(( struct Cyc_Absyn_EnumType_struct*) _temp1243)->f1; goto
_LL1170;} else{ goto _LL1171;} _LL1171: _LL1260: _temp1259= _temp1159.f1; if((
unsigned int) _temp1259 >  4u?*(( int*) _temp1259) ==  Cyc_Absyn_UnionType: 0){
_LL1266: _temp1265=(( struct Cyc_Absyn_UnionType_struct*) _temp1259)->f1; goto
_LL1264; _LL1264: _temp1263=(( struct Cyc_Absyn_UnionType_struct*) _temp1259)->f2;
goto _LL1262; _LL1262: _temp1261=(( struct Cyc_Absyn_UnionType_struct*)
_temp1259)->f3; goto _LL1252;} else{ goto _LL1173;} _LL1252: _temp1251=
_temp1159.f2; if(( unsigned int) _temp1251 >  4u?*(( int*) _temp1251) ==  Cyc_Absyn_UnionType:
0){ _LL1258: _temp1257=(( struct Cyc_Absyn_UnionType_struct*) _temp1251)->f1;
goto _LL1256; _LL1256: _temp1255=(( struct Cyc_Absyn_UnionType_struct*)
_temp1251)->f2; goto _LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_UnionType_struct*)
_temp1251)->f3; goto _LL1172;} else{ goto _LL1173;} _LL1173: _LL1280: _temp1279=
_temp1159.f1; if(( unsigned int) _temp1279 >  4u?*(( int*) _temp1279) ==  Cyc_Absyn_TunionType:
0){ _LL1282: _temp1281=(( struct Cyc_Absyn_TunionType_struct*) _temp1279)->f1;
_LL1288: _temp1287=( void*) _temp1281.tunion_info; if(*(( int*) _temp1287) == 
Cyc_Absyn_KnownTunion){ _LL1290: _temp1289=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1287)->f1; goto _LL1286;} else{ goto _LL1175;} _LL1286: _temp1285=
_temp1281.targs; goto _LL1284; _LL1284: _temp1283=( void*) _temp1281.rgn; goto
_LL1268;} else{ goto _LL1175;} _LL1268: _temp1267= _temp1159.f2; if((
unsigned int) _temp1267 >  4u?*(( int*) _temp1267) ==  Cyc_Absyn_TunionType: 0){
_LL1270: _temp1269=(( struct Cyc_Absyn_TunionType_struct*) _temp1267)->f1;
_LL1276: _temp1275=( void*) _temp1269.tunion_info; if(*(( int*) _temp1275) == 
Cyc_Absyn_KnownTunion){ _LL1278: _temp1277=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1275)->f1; goto _LL1274;} else{ goto _LL1175;} _LL1274: _temp1273=
_temp1269.targs; goto _LL1272; _LL1272: _temp1271=( void*) _temp1269.rgn; goto
_LL1174;} else{ goto _LL1175;} _LL1175: _LL1304: _temp1303= _temp1159.f1; if((
unsigned int) _temp1303 >  4u?*(( int*) _temp1303) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1306: _temp1305=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1303)->f1;
_LL1310: _temp1309=( void*) _temp1305.field_info; if(*(( int*) _temp1309) == 
Cyc_Absyn_KnownTunionfield){ _LL1314: _temp1313=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1309)->f1; goto _LL1312; _LL1312: _temp1311=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1309)->f2; goto _LL1308;} else{ goto _LL1177;} _LL1308: _temp1307=
_temp1305.targs; goto _LL1292;} else{ goto _LL1177;} _LL1292: _temp1291=
_temp1159.f2; if(( unsigned int) _temp1291 >  4u?*(( int*) _temp1291) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1294: _temp1293=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1291)->f1;
_LL1298: _temp1297=( void*) _temp1293.field_info; if(*(( int*) _temp1297) == 
Cyc_Absyn_KnownTunionfield){ _LL1302: _temp1301=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1297)->f1; goto _LL1300; _LL1300: _temp1299=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1297)->f2; goto _LL1296;} else{ goto _LL1177;} _LL1296: _temp1295=
_temp1293.targs; goto _LL1176;} else{ goto _LL1177;} _LL1177: _LL1330: _temp1329=
_temp1159.f1; if(( unsigned int) _temp1329 >  4u?*(( int*) _temp1329) ==  Cyc_Absyn_PointerType:
0){ _LL1332: _temp1331=(( struct Cyc_Absyn_PointerType_struct*) _temp1329)->f1;
_LL1342: _temp1341=( void*) _temp1331.elt_typ; goto _LL1340; _LL1340: _temp1339=(
void*) _temp1331.rgn_typ; goto _LL1338; _LL1338: _temp1337= _temp1331.nullable;
goto _LL1336; _LL1336: _temp1335= _temp1331.tq; goto _LL1334; _LL1334: _temp1333=
_temp1331.bounds; goto _LL1316;} else{ goto _LL1179;} _LL1316: _temp1315=
_temp1159.f2; if(( unsigned int) _temp1315 >  4u?*(( int*) _temp1315) ==  Cyc_Absyn_PointerType:
0){ _LL1318: _temp1317=(( struct Cyc_Absyn_PointerType_struct*) _temp1315)->f1;
_LL1328: _temp1327=( void*) _temp1317.elt_typ; goto _LL1326; _LL1326: _temp1325=(
void*) _temp1317.rgn_typ; goto _LL1324; _LL1324: _temp1323= _temp1317.nullable;
goto _LL1322; _LL1322: _temp1321= _temp1317.tq; goto _LL1320; _LL1320: _temp1319=
_temp1317.bounds; goto _LL1178;} else{ goto _LL1179;} _LL1179: _LL1350:
_temp1349= _temp1159.f1; if(( unsigned int) _temp1349 >  4u?*(( int*) _temp1349)
==  Cyc_Absyn_IntType: 0){ _LL1354: _temp1353=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1349)->f1; goto _LL1352; _LL1352: _temp1351=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1349)->f2; goto _LL1344;} else{ goto _LL1181;} _LL1344: _temp1343=
_temp1159.f2; if(( unsigned int) _temp1343 >  4u?*(( int*) _temp1343) ==  Cyc_Absyn_IntType:
0){ _LL1348: _temp1347=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1343)->f1;
goto _LL1346; _LL1346: _temp1345=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1343)->f2; goto _LL1180;} else{ goto _LL1181;} _LL1181: _LL1358: _temp1357=
_temp1159.f1; if( _temp1357 == ( void*) Cyc_Absyn_FloatType){ goto _LL1356;}
else{ goto _LL1183;} _LL1356: _temp1355= _temp1159.f2; if( _temp1355 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1182;} else{ goto _LL1183;} _LL1183: _LL1362:
_temp1361= _temp1159.f1; if( _temp1361 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1360;} else{ goto _LL1185;} _LL1360: _temp1359= _temp1159.f2; if( _temp1359
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1184;} else{ goto _LL1185;} _LL1185:
_LL1372: _temp1371= _temp1159.f1; if(( unsigned int) _temp1371 >  4u?*(( int*)
_temp1371) ==  Cyc_Absyn_ArrayType: 0){ _LL1378: _temp1377=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1371)->f1; goto _LL1376; _LL1376: _temp1375=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1371)->f2; goto _LL1374; _LL1374: _temp1373=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1371)->f3; goto _LL1364;} else{ goto _LL1187;} _LL1364: _temp1363=
_temp1159.f2; if(( unsigned int) _temp1363 >  4u?*(( int*) _temp1363) ==  Cyc_Absyn_ArrayType:
0){ _LL1370: _temp1369=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1363)->f1;
goto _LL1368; _LL1368: _temp1367=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1363)->f2; goto _LL1366; _LL1366: _temp1365=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1363)->f3; goto _LL1186;} else{ goto _LL1187;} _LL1187: _LL1400: _temp1399=
_temp1159.f1; if(( unsigned int) _temp1399 >  4u?*(( int*) _temp1399) ==  Cyc_Absyn_FnType:
0){ _LL1402: _temp1401=(( struct Cyc_Absyn_FnType_struct*) _temp1399)->f1;
_LL1418: _temp1417= _temp1401.tvars; goto _LL1416; _LL1416: _temp1415= _temp1401.effect;
goto _LL1414; _LL1414: _temp1413=( void*) _temp1401.ret_typ; goto _LL1412;
_LL1412: _temp1411= _temp1401.args; goto _LL1410; _LL1410: _temp1409= _temp1401.c_varargs;
goto _LL1408; _LL1408: _temp1407= _temp1401.cyc_varargs; goto _LL1406; _LL1406:
_temp1405= _temp1401.rgn_po; goto _LL1404; _LL1404: _temp1403= _temp1401.attributes;
goto _LL1380;} else{ goto _LL1189;} _LL1380: _temp1379= _temp1159.f2; if((
unsigned int) _temp1379 >  4u?*(( int*) _temp1379) ==  Cyc_Absyn_FnType: 0){
_LL1382: _temp1381=(( struct Cyc_Absyn_FnType_struct*) _temp1379)->f1; _LL1398:
_temp1397= _temp1381.tvars; goto _LL1396; _LL1396: _temp1395= _temp1381.effect;
goto _LL1394; _LL1394: _temp1393=( void*) _temp1381.ret_typ; goto _LL1392;
_LL1392: _temp1391= _temp1381.args; goto _LL1390; _LL1390: _temp1389= _temp1381.c_varargs;
goto _LL1388; _LL1388: _temp1387= _temp1381.cyc_varargs; goto _LL1386; _LL1386:
_temp1385= _temp1381.rgn_po; goto _LL1384; _LL1384: _temp1383= _temp1381.attributes;
goto _LL1188;} else{ goto _LL1189;} _LL1189: _LL1424: _temp1423= _temp1159.f1;
if(( unsigned int) _temp1423 >  4u?*(( int*) _temp1423) ==  Cyc_Absyn_TupleType:
0){ _LL1426: _temp1425=(( struct Cyc_Absyn_TupleType_struct*) _temp1423)->f1;
goto _LL1420;} else{ goto _LL1191;} _LL1420: _temp1419= _temp1159.f2; if((
unsigned int) _temp1419 >  4u?*(( int*) _temp1419) ==  Cyc_Absyn_TupleType: 0){
_LL1422: _temp1421=(( struct Cyc_Absyn_TupleType_struct*) _temp1419)->f1; goto
_LL1190;} else{ goto _LL1191;} _LL1191: _LL1432: _temp1431= _temp1159.f1; if((
unsigned int) _temp1431 >  4u?*(( int*) _temp1431) ==  Cyc_Absyn_AnonStructType:
0){ _LL1434: _temp1433=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1431)->f1;
goto _LL1428;} else{ goto _LL1193;} _LL1428: _temp1427= _temp1159.f2; if((
unsigned int) _temp1427 >  4u?*(( int*) _temp1427) ==  Cyc_Absyn_AnonStructType:
0){ _LL1430: _temp1429=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1427)->f1;
goto _LL1192;} else{ goto _LL1193;} _LL1193: _LL1440: _temp1439= _temp1159.f1;
if(( unsigned int) _temp1439 >  4u?*(( int*) _temp1439) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1442: _temp1441=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1439)->f1;
goto _LL1436;} else{ goto _LL1195;} _LL1436: _temp1435= _temp1159.f2; if((
unsigned int) _temp1435 >  4u?*(( int*) _temp1435) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1438: _temp1437=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1435)->f1;
goto _LL1194;} else{ goto _LL1195;} _LL1195: _LL1446: _temp1445= _temp1159.f1;
if( _temp1445 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1444;} else{ goto _LL1197;}
_LL1444: _temp1443= _temp1159.f2; if( _temp1443 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1196;} else{ goto _LL1197;} _LL1197: _LL1452: _temp1451= _temp1159.f1;
if(( unsigned int) _temp1451 >  4u?*(( int*) _temp1451) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1454: _temp1453=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1451)->f1; goto _LL1448;} else{ goto _LL1199;} _LL1448: _temp1447=
_temp1159.f2; if(( unsigned int) _temp1447 >  4u?*(( int*) _temp1447) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1450: _temp1449=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1447)->f1; goto _LL1198;} else{ goto _LL1199;} _LL1199: _LL1456: _temp1455=
_temp1159.f1; if(( unsigned int) _temp1455 >  4u?*(( int*) _temp1455) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1200;} else{ goto _LL1201;} _LL1201: _LL1458: _temp1457= _temp1159.f2;
if(( unsigned int) _temp1457 >  4u?*(( int*) _temp1457) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1202;} else{ goto _LL1203;} _LL1203: _LL1460: _temp1459= _temp1159.f1;
if(( unsigned int) _temp1459 >  4u?*(( int*) _temp1459) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1204;} else{ goto _LL1205;} _LL1205: _LL1462: _temp1461= _temp1159.f1;
if(( unsigned int) _temp1461 >  4u?*(( int*) _temp1461) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1206;} else{ goto _LL1207;} _LL1207: _LL1464: _temp1463= _temp1159.f2;
if(( unsigned int) _temp1463 >  4u?*(( int*) _temp1463) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1208;} else{ goto _LL1209;} _LL1209: _LL1466: _temp1465= _temp1159.f2;
if(( unsigned int) _temp1465 >  4u?*(( int*) _temp1465) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1210;} else{ goto _LL1211;} _LL1211: goto _LL1212; _LL1162: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1164: return; _LL1166: { struct _tagged_arr* _temp1467=
_temp1225->name; struct _tagged_arr* _temp1468= _temp1221->name; int _temp1469=*((
int*) _check_null( _temp1225->identity)); int _temp1470=*(( int*) _check_null(
_temp1221->identity)); void* _temp1471= Cyc_Absyn_conref_val( _temp1225->kind);
void* _temp1472= Cyc_Absyn_conref_val( _temp1221->kind); if( _temp1470 == 
_temp1469? Cyc_Std_zstrptrcmp( _temp1467, _temp1468) ==  0: 0){ if( _temp1471 != 
_temp1472){({ struct Cyc_Std_String_pa_struct _temp1476; _temp1476.tag= Cyc_Std_String_pa;
_temp1476.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1472);{ struct
Cyc_Std_String_pa_struct _temp1475; _temp1475.tag= Cyc_Std_String_pa; _temp1475.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1471);{ struct Cyc_Std_String_pa_struct
_temp1474; _temp1474.tag= Cyc_Std_String_pa; _temp1474.f1=( struct _tagged_arr)*
_temp1467;{ void* _temp1473[ 3u]={& _temp1474,& _temp1475,& _temp1476};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1473, sizeof( void*), 3u));}}}});}
return;} goto _LL1160;} _LL1168: if((( _temp1233 !=  0? _temp1241 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1233),( struct _tuple1*) _check_null(
_temp1241)) ==  0: 0)? 1:( _temp1233 ==  0? _temp1241 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1231, _temp1239); return;} goto _LL1160; _LL1170: if( Cyc_Absyn_qvar_cmp(
_temp1249, _temp1245) ==  0){ return;} goto _LL1160; _LL1172: if((( _temp1257 != 
0? _temp1265 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1257),( struct _tuple1*) _check_null( _temp1265)) ==  0: 0)? 1:( _temp1257
==  0? _temp1265 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1255, _temp1263);
return;} goto _LL1160; _LL1174: if( _temp1289 ==  _temp1277? 1: Cyc_Absyn_qvar_cmp(
_temp1289->name, _temp1277->name) ==  0){ Cyc_Tcutil_unify_it( _temp1271,
_temp1283); Cyc_Tcutil_unify_list( _temp1273, _temp1285); return;} goto _LL1160;
_LL1176: if(( _temp1313 ==  _temp1301? 1: Cyc_Absyn_qvar_cmp( _temp1313->name,
_temp1301->name) ==  0)? _temp1311 ==  _temp1299? 1: Cyc_Absyn_qvar_cmp(
_temp1311->name, _temp1299->name) ==  0: 0){ Cyc_Tcutil_unify_list( _temp1295,
_temp1307); return;} goto _LL1160; _LL1178: Cyc_Tcutil_unify_it( _temp1327,
_temp1341); Cyc_Tcutil_unify_it( _temp1339, _temp1325); Cyc_Tcutil_unify_tqual(
_temp1321, _temp1335); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1319, _temp1333);{ void* _temp1477=( void*)( Cyc_Absyn_compress_conref(
_temp1319))->v; void* _temp1483; _LL1479: if(( unsigned int) _temp1477 >  1u?*((
int*) _temp1477) ==  Cyc_Absyn_Eq_constr: 0){ _LL1484: _temp1483=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1477)->f1; if( _temp1483 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1480;} else{ goto _LL1481;}} else{ goto _LL1481;}
_LL1481: goto _LL1482; _LL1480: return; _LL1482: goto _LL1478; _LL1478:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp, _temp1323, _temp1337); return;
_LL1180: if( _temp1347 ==  _temp1353? _temp1345 ==  _temp1351: 0){ return;} goto
_LL1160; _LL1182: return; _LL1184: return; _LL1186: Cyc_Tcutil_unify_it(
_temp1369, _temp1377); Cyc_Tcutil_unify_tqual( _temp1367, _temp1375); if(
_temp1373 ==  _temp1365){ return;} if( _temp1373 ==  0? 1: _temp1365 ==  0){
goto _LL1160;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1373)) ==  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1365))){ return;} goto _LL1160; _LL1188: { int done= 0;{
struct _RegionHandle _temp1485= _new_region(); struct _RegionHandle* rgn=&
_temp1485; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1397
!=  0) { if( _temp1417 ==  0){ break;} inst=({ struct Cyc_List_List* _temp1486=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1486->hd=( void*)({ struct _tuple5* _temp1487=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1487->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1417))->hd; _temp1487->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1488=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1488[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1489; _temp1489.tag= Cyc_Absyn_VarType; _temp1489.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1397))->hd;
_temp1489;}); _temp1488;}); _temp1487;}); _temp1486->tl= inst; _temp1486;});
_temp1397=(( struct Cyc_List_List*) _check_null( _temp1397))->tl; _temp1417=((
struct Cyc_List_List*) _check_null( _temp1417))->tl;} if( _temp1417 !=  0){
_npop_handler( 0u); goto _LL1160;} if( inst !=  0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1490=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1490[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1491; _temp1491.tag= Cyc_Absyn_FnType; _temp1491.f1=({ struct Cyc_Absyn_FnInfo
_temp1492; _temp1492.tvars= 0; _temp1492.effect= _temp1395; _temp1492.ret_typ=(
void*) _temp1393; _temp1492.args= _temp1391; _temp1492.c_varargs= _temp1389;
_temp1492.cyc_varargs= _temp1387; _temp1492.rgn_po= _temp1385; _temp1492.attributes=
_temp1383; _temp1492;}); _temp1491;}); _temp1490;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1493=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1493[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1494; _temp1494.tag= Cyc_Absyn_FnType; _temp1494.f1=({ struct Cyc_Absyn_FnInfo
_temp1495; _temp1495.tvars= 0; _temp1495.effect= _temp1415; _temp1495.ret_typ=(
void*) _temp1413; _temp1495.args= _temp1411; _temp1495.c_varargs= _temp1409;
_temp1495.cyc_varargs= _temp1407; _temp1495.rgn_po= _temp1405; _temp1495.attributes=
_temp1403; _temp1495;}); _temp1494;}); _temp1493;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1393, _temp1413); for( 0;
_temp1391 !=  0? _temp1411 !=  0: 0;( _temp1391=(( struct Cyc_List_List*)
_check_null( _temp1391))->tl, _temp1411=(( struct Cyc_List_List*) _check_null(
_temp1411))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1391))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1411))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1391))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1411))->hd)).f3);} if( _temp1391 !=  0?
1: _temp1411 !=  0){ goto _LL1160;} if( _temp1389 !=  _temp1409){ goto _LL1160;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1497=({ struct _tuple11 _temp1496;
_temp1496.f1= _temp1387; _temp1496.f2= _temp1407; _temp1496;}); struct Cyc_Absyn_VarargInfo*
_temp1507; struct Cyc_Absyn_VarargInfo* _temp1509; struct Cyc_Absyn_VarargInfo*
_temp1511; struct Cyc_Absyn_VarargInfo* _temp1513; struct Cyc_Absyn_VarargInfo*
_temp1515; struct Cyc_Absyn_VarargInfo _temp1517; int _temp1518; void* _temp1520;
struct Cyc_Absyn_Tqual _temp1522; struct Cyc_Core_Opt* _temp1524; struct Cyc_Absyn_VarargInfo*
_temp1526; struct Cyc_Absyn_VarargInfo _temp1528; int _temp1529; void* _temp1531;
struct Cyc_Absyn_Tqual _temp1533; struct Cyc_Core_Opt* _temp1535; _LL1499:
_LL1510: _temp1509= _temp1497.f1; if( _temp1509 ==  0){ goto _LL1508;} else{
goto _LL1501;} _LL1508: _temp1507= _temp1497.f2; if( _temp1507 ==  0){ goto
_LL1500;} else{ goto _LL1501;} _LL1501: _LL1512: _temp1511= _temp1497.f1; if(
_temp1511 ==  0){ goto _LL1502;} else{ goto _LL1503;} _LL1503: _LL1514:
_temp1513= _temp1497.f2; if( _temp1513 ==  0){ goto _LL1504;} else{ goto _LL1505;}
_LL1505: _LL1527: _temp1526= _temp1497.f1; if( _temp1526 ==  0){ goto _LL1498;}
else{ _temp1528=* _temp1526; _LL1536: _temp1535= _temp1528.name; goto _LL1534;
_LL1534: _temp1533= _temp1528.tq; goto _LL1532; _LL1532: _temp1531=( void*)
_temp1528.type; goto _LL1530; _LL1530: _temp1529= _temp1528.inject; goto _LL1516;}
_LL1516: _temp1515= _temp1497.f2; if( _temp1515 ==  0){ goto _LL1498;} else{
_temp1517=* _temp1515; _LL1525: _temp1524= _temp1517.name; goto _LL1523; _LL1523:
_temp1522= _temp1517.tq; goto _LL1521; _LL1521: _temp1520=( void*) _temp1517.type;
goto _LL1519; _LL1519: _temp1518= _temp1517.inject; goto _LL1506;} _LL1500: goto
_LL1498; _LL1502: goto _LL1504; _LL1504: bad_cyc_vararg= 1; goto _LL1498;
_LL1506: Cyc_Tcutil_unify_tqual( _temp1533, _temp1522); Cyc_Tcutil_unify_it(
_temp1531, _temp1520); if( _temp1529 !=  _temp1518){ bad_cyc_vararg= 1;} goto
_LL1498; _LL1498:;} if( bad_cyc_vararg){ goto _LL1160;}{ int bad_effect= 0;{
struct _tuple12 _temp1538=({ struct _tuple12 _temp1537; _temp1537.f1= _temp1395;
_temp1537.f2= _temp1415; _temp1537;}); struct Cyc_Core_Opt* _temp1548; struct
Cyc_Core_Opt* _temp1550; struct Cyc_Core_Opt* _temp1552; struct Cyc_Core_Opt*
_temp1554; _LL1540: _LL1551: _temp1550= _temp1538.f1; if( _temp1550 ==  0){ goto
_LL1549;} else{ goto _LL1542;} _LL1549: _temp1548= _temp1538.f2; if( _temp1548
==  0){ goto _LL1541;} else{ goto _LL1542;} _LL1542: _LL1553: _temp1552=
_temp1538.f1; if( _temp1552 ==  0){ goto _LL1543;} else{ goto _LL1544;} _LL1544:
_LL1555: _temp1554= _temp1538.f2; if( _temp1554 ==  0){ goto _LL1545;} else{
goto _LL1546;} _LL1546: goto _LL1547; _LL1541: goto _LL1539; _LL1543: goto
_LL1545; _LL1545: bad_effect= 1; goto _LL1539; _LL1547: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1395))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1415))->v); goto _LL1539; _LL1539:;} if( bad_effect){ goto
_LL1160;} if( ! Cyc_Tcutil_same_atts( _temp1403, _temp1383)){ goto _LL1160;} if(
! Cyc_Tcutil_same_rgn_po( _temp1405, _temp1385)){ goto _LL1160;} return;}}}
_LL1190: for( 0; _temp1421 !=  0? _temp1425 !=  0: 0;( _temp1421=(( struct Cyc_List_List*)
_check_null( _temp1421))->tl, _temp1425=(( struct Cyc_List_List*) _check_null(
_temp1425))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1421))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1425))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1421))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1425))->hd)).f2);} if( _temp1421 ==  0?
_temp1425 ==  0: 0){ return;} goto _LL1160; _LL1192: _temp1441= _temp1433;
_temp1437= _temp1429; goto _LL1194; _LL1194: for( 0; _temp1437 !=  0? _temp1441
!=  0: 0;( _temp1437=(( struct Cyc_List_List*) _check_null( _temp1437))->tl,
_temp1441=(( struct Cyc_List_List*) _check_null( _temp1441))->tl)){ struct Cyc_Absyn_Structfield*
_temp1556=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1437))->hd; struct Cyc_Absyn_Structfield* _temp1557=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1441))->hd; if( Cyc_Std_zstrptrcmp(
_temp1556->name, _temp1557->name) !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual( _temp1556->tq, _temp1557->tq); Cyc_Tcutil_unify_it((
void*) _temp1556->type,( void*) _temp1557->type); if((( ! Cyc_Tcutil_same_atts(
_temp1556->attributes, _temp1557->attributes)? 1:( _temp1556->width !=  0?
_temp1557->width ==  0: 0))? 1:( _temp1557->width !=  0? _temp1556->width ==  0:
0))? 1:(( _temp1556->width !=  0? _temp1557->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1556->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1557->width)): 0)){( int) _throw(( void*)
Cyc_Tcutil_Unify);}} if( _temp1437 ==  0? _temp1441 ==  0: 0){ return;} goto
_LL1160; _LL1196: return; _LL1198: Cyc_Tcutil_unify_it( _temp1453, _temp1449);
return; _LL1200: goto _LL1202; _LL1202: goto _LL1204; _LL1204: goto _LL1206;
_LL1206: goto _LL1208; _LL1208: goto _LL1210; _LL1210: if( Cyc_Tcutil_unify_effect(
t1, t2)){ return;} else{ goto _LL1160;} _LL1212: goto _LL1160; _LL1160:;}( int)
_throw(( void*) Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp1558= Cyc_Tcutil_compress( t); _LL1560: if(( unsigned int) _temp1558
>  4u?*(( int*) _temp1558) ==  Cyc_Absyn_IntType: 0){ goto _LL1561;} else{ goto
_LL1562;} _LL1562: if( _temp1558 == ( void*) Cyc_Absyn_FloatType){ goto _LL1563;}
else{ goto _LL1564;} _LL1564: if( _temp1558 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1565;} else{ goto _LL1566;} _LL1566: if(( unsigned int) _temp1558 >  4u?*((
int*) _temp1558) ==  Cyc_Absyn_EnumType: 0){ goto _LL1567;} else{ goto _LL1568;}
_LL1568: goto _LL1569; _LL1561: goto _LL1563; _LL1563: goto _LL1565; _LL1565:
goto _LL1567; _LL1567: return 1; _LL1569: return 0; _LL1559:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1571=({ struct _tuple8 _temp1570; _temp1570.f1= t1;
_temp1570.f2= t2; _temp1570;}); void* _temp1593; void* _temp1595; void*
_temp1597; void* _temp1599; void* _temp1601; void* _temp1603; void* _temp1605;
void* _temp1607; void* _temp1609; void* _temp1611; void* _temp1613; void*
_temp1615; void* _temp1617; void* _temp1619; void* _temp1621; void* _temp1623;
void* _temp1625; void* _temp1627; void* _temp1629; void* _temp1631; void*
_temp1633; void* _temp1635; void* _temp1637; void* _temp1639; void* _temp1641;
void* _temp1643; void* _temp1645; _LL1573: _LL1596: _temp1595= _temp1571.f1; if(
_temp1595 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1594;} else{ goto _LL1575;}
_LL1594: _temp1593= _temp1571.f2; if( _temp1593 == ( void*) Cyc_Absyn_FloatType){
goto _LL1574;} else{ goto _LL1575;} _LL1575: _LL1600: _temp1599= _temp1571.f1;
if( _temp1599 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1598;} else{ goto
_LL1577;} _LL1598: _temp1597= _temp1571.f2; if(( unsigned int) _temp1597 >  4u?*((
int*) _temp1597) ==  Cyc_Absyn_IntType: 0){ goto _LL1576;} else{ goto _LL1577;}
_LL1577: _LL1604: _temp1603= _temp1571.f1; if( _temp1603 == ( void*) Cyc_Absyn_FloatType){
goto _LL1602;} else{ goto _LL1579;} _LL1602: _temp1601= _temp1571.f2; if((
unsigned int) _temp1601 >  4u?*(( int*) _temp1601) ==  Cyc_Absyn_IntType: 0){
goto _LL1578;} else{ goto _LL1579;} _LL1579: _LL1610: _temp1609= _temp1571.f1;
if(( unsigned int) _temp1609 >  4u?*(( int*) _temp1609) ==  Cyc_Absyn_IntType: 0){
_LL1612: _temp1611=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1609)->f2;
if( _temp1611 == ( void*) Cyc_Absyn_B8){ goto _LL1606;} else{ goto _LL1581;}}
else{ goto _LL1581;} _LL1606: _temp1605= _temp1571.f2; if(( unsigned int)
_temp1605 >  4u?*(( int*) _temp1605) ==  Cyc_Absyn_IntType: 0){ _LL1608:
_temp1607=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1605)->f2; if(
_temp1607 == ( void*) Cyc_Absyn_B8){ goto _LL1580;} else{ goto _LL1581;}} else{
goto _LL1581;} _LL1581: _LL1614: _temp1613= _temp1571.f1; if(( unsigned int)
_temp1613 >  4u?*(( int*) _temp1613) ==  Cyc_Absyn_IntType: 0){ _LL1616:
_temp1615=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1613)->f2; if(
_temp1615 == ( void*) Cyc_Absyn_B8){ goto _LL1582;} else{ goto _LL1583;}} else{
goto _LL1583;} _LL1583: _LL1620: _temp1619= _temp1571.f1; if(( unsigned int)
_temp1619 >  4u?*(( int*) _temp1619) ==  Cyc_Absyn_IntType: 0){ _LL1622:
_temp1621=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1619)->f2; if(
_temp1621 == ( void*) Cyc_Absyn_B4){ goto _LL1618;} else{ goto _LL1585;}} else{
goto _LL1585;} _LL1618: _temp1617= _temp1571.f2; if( _temp1617 == ( void*) Cyc_Absyn_FloatType){
goto _LL1584;} else{ goto _LL1585;} _LL1585: _LL1628: _temp1627= _temp1571.f1;
if(( unsigned int) _temp1627 >  4u?*(( int*) _temp1627) ==  Cyc_Absyn_IntType: 0){
_LL1630: _temp1629=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1627)->f2;
if( _temp1629 == ( void*) Cyc_Absyn_B4){ goto _LL1624;} else{ goto _LL1587;}}
else{ goto _LL1587;} _LL1624: _temp1623= _temp1571.f2; if(( unsigned int)
_temp1623 >  4u?*(( int*) _temp1623) ==  Cyc_Absyn_IntType: 0){ _LL1626:
_temp1625=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1623)->f2; if(
_temp1625 == ( void*) Cyc_Absyn_B2){ goto _LL1586;} else{ goto _LL1587;}} else{
goto _LL1587;} _LL1587: _LL1636: _temp1635= _temp1571.f1; if(( unsigned int)
_temp1635 >  4u?*(( int*) _temp1635) ==  Cyc_Absyn_IntType: 0){ _LL1638:
_temp1637=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1635)->f2; if(
_temp1637 == ( void*) Cyc_Absyn_B4){ goto _LL1632;} else{ goto _LL1589;}} else{
goto _LL1589;} _LL1632: _temp1631= _temp1571.f2; if(( unsigned int) _temp1631 > 
4u?*(( int*) _temp1631) ==  Cyc_Absyn_IntType: 0){ _LL1634: _temp1633=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1631)->f2; if( _temp1633 == ( void*) Cyc_Absyn_B1){
goto _LL1588;} else{ goto _LL1589;}} else{ goto _LL1589;} _LL1589: _LL1644:
_temp1643= _temp1571.f1; if(( unsigned int) _temp1643 >  4u?*(( int*) _temp1643)
==  Cyc_Absyn_IntType: 0){ _LL1646: _temp1645=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1643)->f2; if( _temp1645 == ( void*) Cyc_Absyn_B2){ goto _LL1640;} else{
goto _LL1591;}} else{ goto _LL1591;} _LL1640: _temp1639= _temp1571.f2; if((
unsigned int) _temp1639 >  4u?*(( int*) _temp1639) ==  Cyc_Absyn_IntType: 0){
_LL1642: _temp1641=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1639)->f2;
if( _temp1641 == ( void*) Cyc_Absyn_B1){ goto _LL1590;} else{ goto _LL1591;}}
else{ goto _LL1591;} _LL1591: goto _LL1592; _LL1574: goto _LL1576; _LL1576: goto
_LL1578; _LL1578: return 1; _LL1580: return 0; _LL1582: goto _LL1584; _LL1584:
goto _LL1586; _LL1586: goto _LL1588; _LL1588: goto _LL1590; _LL1590: return 1;
_LL1592: return 0; _LL1572:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1647=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1647->v=( void*)
t1; _temp1647;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify( t,(
void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp1650; _temp1650.tag= Cyc_Std_String_pa; _temp1650.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1649; _temp1649.tag= Cyc_Std_String_pa;
_temp1649.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1648[
2u]={& _temp1649,& _temp1650}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1648, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1651= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1653: if(( unsigned int) _temp1651 >  4u?*(( int*) _temp1651) ==  Cyc_Absyn_PointerType:
0){ goto _LL1654;} else{ goto _LL1655;} _LL1655: goto _LL1656; _LL1654: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1652; _LL1656: return 0; _LL1652:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1657= Cyc_Tcutil_compress(
t); _LL1659: if(( unsigned int) _temp1657 >  4u?*(( int*) _temp1657) ==  Cyc_Absyn_IntType:
0){ goto _LL1660;} else{ goto _LL1661;} _LL1661: if(( unsigned int) _temp1657 > 
4u?*(( int*) _temp1657) ==  Cyc_Absyn_EnumType: 0){ goto _LL1662;} else{ goto
_LL1663;} _LL1663: goto _LL1664; _LL1660: goto _LL1662; _LL1662: return 1;
_LL1664: return 0; _LL1658:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1665[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1665, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1666[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1666, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1668=({ struct _tuple8 _temp1667; _temp1667.f1= t1; _temp1667.f2= t2;
_temp1667;}); void* _temp1680; struct Cyc_Absyn_PtrInfo _temp1682; void*
_temp1684; struct Cyc_Absyn_PtrInfo _temp1686; void* _temp1688; struct Cyc_Absyn_Exp*
_temp1690; struct Cyc_Absyn_Tqual _temp1692; void* _temp1694; void* _temp1696;
struct Cyc_Absyn_Exp* _temp1698; struct Cyc_Absyn_Tqual _temp1700; void*
_temp1702; void* _temp1704; struct Cyc_Absyn_TunionInfo _temp1706; struct Cyc_List_List*
_temp1708; void* _temp1710; struct Cyc_Absyn_Tuniondecl* _temp1712; void*
_temp1714; struct Cyc_Absyn_TunionFieldInfo _temp1716; struct Cyc_List_List*
_temp1718; void* _temp1720; struct Cyc_Absyn_Tunionfield* _temp1722; struct Cyc_Absyn_Tuniondecl*
_temp1724; void* _temp1726; struct Cyc_Absyn_TunionInfo _temp1728; void*
_temp1730; struct Cyc_List_List* _temp1732; void* _temp1734; struct Cyc_Absyn_Tuniondecl*
_temp1736; void* _temp1738; struct Cyc_Absyn_PtrInfo _temp1740; struct Cyc_Absyn_Conref*
_temp1742; struct Cyc_Absyn_Tqual _temp1744; struct Cyc_Absyn_Conref* _temp1746;
void* _temp1748; void* _temp1750; _LL1670: _LL1685: _temp1684= _temp1668.f1; if((
unsigned int) _temp1684 >  4u?*(( int*) _temp1684) ==  Cyc_Absyn_PointerType: 0){
_LL1687: _temp1686=(( struct Cyc_Absyn_PointerType_struct*) _temp1684)->f1; goto
_LL1681;} else{ goto _LL1672;} _LL1681: _temp1680= _temp1668.f2; if((
unsigned int) _temp1680 >  4u?*(( int*) _temp1680) ==  Cyc_Absyn_PointerType: 0){
_LL1683: _temp1682=(( struct Cyc_Absyn_PointerType_struct*) _temp1680)->f1; goto
_LL1671;} else{ goto _LL1672;} _LL1672: _LL1697: _temp1696= _temp1668.f1; if((
unsigned int) _temp1696 >  4u?*(( int*) _temp1696) ==  Cyc_Absyn_ArrayType: 0){
_LL1703: _temp1702=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1696)->f1;
goto _LL1701; _LL1701: _temp1700=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1696)->f2; goto _LL1699; _LL1699: _temp1698=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1696)->f3; goto _LL1689;} else{ goto _LL1674;} _LL1689: _temp1688=
_temp1668.f2; if(( unsigned int) _temp1688 >  4u?*(( int*) _temp1688) ==  Cyc_Absyn_ArrayType:
0){ _LL1695: _temp1694=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1688)->f1;
goto _LL1693; _LL1693: _temp1692=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1688)->f2; goto _LL1691; _LL1691: _temp1690=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1688)->f3; goto _LL1673;} else{ goto _LL1674;} _LL1674: _LL1715: _temp1714=
_temp1668.f1; if(( unsigned int) _temp1714 >  4u?*(( int*) _temp1714) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1717: _temp1716=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1714)->f1;
_LL1721: _temp1720=( void*) _temp1716.field_info; if(*(( int*) _temp1720) == 
Cyc_Absyn_KnownTunionfield){ _LL1725: _temp1724=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1720)->f1; goto _LL1723; _LL1723: _temp1722=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1720)->f2; goto _LL1719;} else{ goto _LL1676;} _LL1719: _temp1718=
_temp1716.targs; goto _LL1705;} else{ goto _LL1676;} _LL1705: _temp1704=
_temp1668.f2; if(( unsigned int) _temp1704 >  4u?*(( int*) _temp1704) ==  Cyc_Absyn_TunionType:
0){ _LL1707: _temp1706=(( struct Cyc_Absyn_TunionType_struct*) _temp1704)->f1;
_LL1711: _temp1710=( void*) _temp1706.tunion_info; if(*(( int*) _temp1710) == 
Cyc_Absyn_KnownTunion){ _LL1713: _temp1712=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1710)->f1; goto _LL1709;} else{ goto _LL1676;} _LL1709: _temp1708=
_temp1706.targs; goto _LL1675;} else{ goto _LL1676;} _LL1676: _LL1739: _temp1738=
_temp1668.f1; if(( unsigned int) _temp1738 >  4u?*(( int*) _temp1738) ==  Cyc_Absyn_PointerType:
0){ _LL1741: _temp1740=(( struct Cyc_Absyn_PointerType_struct*) _temp1738)->f1;
_LL1751: _temp1750=( void*) _temp1740.elt_typ; goto _LL1749; _LL1749: _temp1748=(
void*) _temp1740.rgn_typ; goto _LL1747; _LL1747: _temp1746= _temp1740.nullable;
goto _LL1745; _LL1745: _temp1744= _temp1740.tq; goto _LL1743; _LL1743: _temp1742=
_temp1740.bounds; goto _LL1727;} else{ goto _LL1678;} _LL1727: _temp1726=
_temp1668.f2; if(( unsigned int) _temp1726 >  4u?*(( int*) _temp1726) ==  Cyc_Absyn_TunionType:
0){ _LL1729: _temp1728=(( struct Cyc_Absyn_TunionType_struct*) _temp1726)->f1;
_LL1735: _temp1734=( void*) _temp1728.tunion_info; if(*(( int*) _temp1734) == 
Cyc_Absyn_KnownTunion){ _LL1737: _temp1736=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1734)->f1; goto _LL1733;} else{ goto _LL1678;} _LL1733: _temp1732=
_temp1728.targs; goto _LL1731; _LL1731: _temp1730=( void*) _temp1728.rgn; goto
_LL1677;} else{ goto _LL1678;} _LL1678: goto _LL1679; _LL1671: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1686.nullable, _temp1682.nullable)){
void* _temp1752=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1686.nullable))->v; int _temp1758; _LL1754:
if(( unsigned int) _temp1752 >  1u?*(( int*) _temp1752) ==  Cyc_Absyn_Eq_constr:
0){ _LL1759: _temp1758=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1752)->f1;
goto _LL1755;} else{ goto _LL1756;} _LL1756: goto _LL1757; _LL1755: okay= !
_temp1758; goto _LL1753; _LL1757:( int) _throw(({ void* _temp1760[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("silent_castable conref not eq", sizeof( unsigned char), 30u), _tag_arr(
_temp1760, sizeof( void*), 0u));})); _LL1753:;} if( ! Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1686.bounds, _temp1682.bounds)){ struct _tuple8
_temp1762=({ struct _tuple8 _temp1761; _temp1761.f1=( void*)( Cyc_Absyn_compress_conref(
_temp1686.bounds))->v; _temp1761.f2=( void*)( Cyc_Absyn_compress_conref(
_temp1682.bounds))->v; _temp1761;}); void* _temp1772; void* _temp1774; void*
_temp1776; void* _temp1778; void* _temp1780; void* _temp1782; struct Cyc_Absyn_Exp*
_temp1784; void* _temp1786; void* _temp1788; struct Cyc_Absyn_Exp* _temp1790;
void* _temp1792; void* _temp1794; void* _temp1796; void* _temp1798; _LL1764:
_LL1777: _temp1776= _temp1762.f1; if(( unsigned int) _temp1776 >  1u?*(( int*)
_temp1776) ==  Cyc_Absyn_Eq_constr: 0){ _LL1779: _temp1778=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1776)->f1; if(( unsigned int) _temp1778 >  1u?*(( int*) _temp1778) ==  Cyc_Absyn_Upper_b:
0){ goto _LL1773;} else{ goto _LL1766;}} else{ goto _LL1766;} _LL1773: _temp1772=
_temp1762.f2; if(( unsigned int) _temp1772 >  1u?*(( int*) _temp1772) ==  Cyc_Absyn_Eq_constr:
0){ _LL1775: _temp1774=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1772)->f1;
if( _temp1774 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1765;} else{ goto
_LL1766;}} else{ goto _LL1766;} _LL1766: _LL1787: _temp1786= _temp1762.f1; if((
unsigned int) _temp1786 >  1u?*(( int*) _temp1786) ==  Cyc_Absyn_Eq_constr: 0){
_LL1789: _temp1788=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1786)->f1;
if(( unsigned int) _temp1788 >  1u?*(( int*) _temp1788) ==  Cyc_Absyn_Upper_b: 0){
_LL1791: _temp1790=(( struct Cyc_Absyn_Upper_b_struct*) _temp1788)->f1; goto
_LL1781;} else{ goto _LL1768;}} else{ goto _LL1768;} _LL1781: _temp1780=
_temp1762.f2; if(( unsigned int) _temp1780 >  1u?*(( int*) _temp1780) ==  Cyc_Absyn_Eq_constr:
0){ _LL1783: _temp1782=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1780)->f1;
if(( unsigned int) _temp1782 >  1u?*(( int*) _temp1782) ==  Cyc_Absyn_Upper_b: 0){
_LL1785: _temp1784=(( struct Cyc_Absyn_Upper_b_struct*) _temp1782)->f1; goto
_LL1767;} else{ goto _LL1768;}} else{ goto _LL1768;} _LL1768: _LL1797: _temp1796=
_temp1762.f1; if(( unsigned int) _temp1796 >  1u?*(( int*) _temp1796) ==  Cyc_Absyn_Eq_constr:
0){ _LL1799: _temp1798=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1796)->f1;
if( _temp1798 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1793;} else{ goto
_LL1770;}} else{ goto _LL1770;} _LL1793: _temp1792= _temp1762.f2; if((
unsigned int) _temp1792 >  1u?*(( int*) _temp1792) ==  Cyc_Absyn_Eq_constr: 0){
_LL1795: _temp1794=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1792)->f1;
if( _temp1794 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1769;} else{ goto
_LL1770;}} else{ goto _LL1770;} _LL1770: goto _LL1771; _LL1765: okay= 1; goto
_LL1763; _LL1767: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1790) >=  Cyc_Evexp_eval_const_uint_exp(
_temp1784): 0;({ void* _temp1800[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1800, sizeof( void*), 0u));}); goto
_LL1763; _LL1769: okay= 1; goto _LL1763; _LL1771: okay= 0; goto _LL1763; _LL1763:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1686.elt_typ,( void*) _temp1682.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1686.rgn_typ,( void*) _temp1682.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1686.rgn_typ,( void*) _temp1682.rgn_typ):
0; okay= okay? !( _temp1686.tq).q_const? 1:( _temp1682.tq).q_const: 0; return
okay;} _LL1673: { int okay; okay=( _temp1698 !=  0? _temp1690 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1698)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1690)): 0; return( okay? Cyc_Tcutil_unify(
_temp1702, _temp1694): 0)? ! _temp1700.q_const? 1: _temp1692.q_const: 0;}
_LL1675: if(( _temp1724 ==  _temp1712? 1: Cyc_Absyn_qvar_cmp( _temp1724->name,
_temp1712->name) ==  0)? _temp1722->typs ==  0: 0){ for( 0; _temp1718 !=  0?
_temp1708 !=  0: 0;( _temp1718=(( struct Cyc_List_List*) _check_null( _temp1718))->tl,
_temp1708=(( struct Cyc_List_List*) _check_null( _temp1708))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1718))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1708))->hd)){ break;}} if( _temp1718 ==  0?
_temp1708 ==  0: 0){ return 1;}} return 0; _LL1677:{ void* _temp1801= Cyc_Tcutil_compress(
_temp1750); struct Cyc_Absyn_TunionFieldInfo _temp1807; struct Cyc_List_List*
_temp1809; void* _temp1811; struct Cyc_Absyn_Tunionfield* _temp1813; struct Cyc_Absyn_Tuniondecl*
_temp1815; _LL1803: if(( unsigned int) _temp1801 >  4u?*(( int*) _temp1801) == 
Cyc_Absyn_TunionFieldType: 0){ _LL1808: _temp1807=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1801)->f1; _LL1812: _temp1811=( void*) _temp1807.field_info; if(*(( int*)
_temp1811) ==  Cyc_Absyn_KnownTunionfield){ _LL1816: _temp1815=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1811)->f1; goto _LL1814; _LL1814: _temp1813=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1811)->f2; goto _LL1810;} else{ goto _LL1805;} _LL1810: _temp1809=
_temp1807.targs; goto _LL1804;} else{ goto _LL1805;} _LL1805: goto _LL1806;
_LL1804: if( ! Cyc_Tcutil_unify( _temp1748, _temp1730)? ! Cyc_Tcenv_region_outlives(
te, _temp1748, _temp1730): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1746,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1742,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1736->name, _temp1815->name) ==  0? _temp1813->typs !=  0: 0){ int okay= 1;
for( 0; _temp1809 !=  0? _temp1732 !=  0: 0;( _temp1809=(( struct Cyc_List_List*)
_check_null( _temp1809))->tl, _temp1732=(( struct Cyc_List_List*) _check_null(
_temp1732))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1809))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1732))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1809 !=  0)? 1:
_temp1732 !=  0){ return 0;} return 1;} goto _LL1802; _LL1806: goto _LL1802;
_LL1802:;} return 0; _LL1679: return Cyc_Tcutil_unify( t1, t2); _LL1669:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1817[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1817, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Std_String_pa_struct
_temp1820; _temp1820.tag= Cyc_Std_String_pa; _temp1820.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct _temp1819;
_temp1819.tag= Cyc_Std_String_pa; _temp1819.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1818[ 2u]={& _temp1819,& _temp1820}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1818, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} int Cyc_Tcutil_coerceable( void* t){ void* _temp1821= Cyc_Tcutil_compress(
t); _LL1823: if(( unsigned int) _temp1821 >  4u?*(( int*) _temp1821) ==  Cyc_Absyn_IntType:
0){ goto _LL1824;} else{ goto _LL1825;} _LL1825: if( _temp1821 == ( void*) Cyc_Absyn_FloatType){
goto _LL1826;} else{ goto _LL1827;} _LL1827: if( _temp1821 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1828;} else{ goto _LL1829;} _LL1829: goto _LL1830; _LL1824: goto _LL1826;
_LL1826: goto _LL1828; _LL1828: return 1; _LL1830: return 0; _LL1822:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1831=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp1831->f1= x->tq; _temp1831->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1831;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1832= t1; struct Cyc_List_List* _temp1844; struct Cyc_Absyn_Structdecl**
_temp1846; struct Cyc_List_List* _temp1848; struct _tuple1* _temp1850; struct
Cyc_List_List* _temp1852; _LL1834: if( _temp1832 == ( void*) Cyc_Absyn_VoidType){
goto _LL1835;} else{ goto _LL1836;} _LL1836: if(( unsigned int) _temp1832 >  4u?*((
int*) _temp1832) ==  Cyc_Absyn_TupleType: 0){ _LL1845: _temp1844=(( struct Cyc_Absyn_TupleType_struct*)
_temp1832)->f1; goto _LL1837;} else{ goto _LL1838;} _LL1838: if(( unsigned int)
_temp1832 >  4u?*(( int*) _temp1832) ==  Cyc_Absyn_StructType: 0){ _LL1851:
_temp1850=(( struct Cyc_Absyn_StructType_struct*) _temp1832)->f1; goto _LL1849;
_LL1849: _temp1848=(( struct Cyc_Absyn_StructType_struct*) _temp1832)->f2; goto
_LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_StructType_struct*) _temp1832)->f3;
goto _LL1839;} else{ goto _LL1840;} _LL1840: if(( unsigned int) _temp1832 >  4u?*((
int*) _temp1832) ==  Cyc_Absyn_AnonStructType: 0){ _LL1853: _temp1852=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1832)->f1; goto _LL1841;} else{ goto
_LL1842;} _LL1842: goto _LL1843; _LL1835: return 0; _LL1837: return _temp1844;
_LL1839: if( _temp1846 ==  0? 1: _temp1850 ==  0){ return({ struct Cyc_List_List*
_temp1854=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1854->hd=( void*)({ struct _tuple4* _temp1855=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1855->f1= Cyc_Absyn_empty_tqual(); _temp1855->f2=
t1; _temp1855;}); _temp1854->tl= 0; _temp1854;});}{ struct Cyc_Absyn_Structdecl*
_temp1856=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1846)); struct
_tuple1 _temp1857=*(( struct _tuple1*) _check_null( _temp1850)); struct Cyc_List_List*
_temp1858=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1856->tvs, _temp1848); if( _temp1856->fields ==  0){
return({ struct Cyc_List_List* _temp1859=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1859->hd=( void*)({ struct _tuple4*
_temp1860=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1860->f1=
Cyc_Absyn_empty_tqual(); _temp1860->f2= t1; _temp1860;}); _temp1859->tl= 0;
_temp1859;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1858,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1856->fields))->v);}
_LL1841: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1852); _LL1843: return({
struct Cyc_List_List* _temp1861=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1861->hd=( void*)({ struct _tuple4* _temp1862=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1862->f1= Cyc_Absyn_empty_tqual();
_temp1862->f2= t1; _temp1862;}); _temp1861->tl= 0; _temp1861;}); _LL1833:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1864=({ struct _tuple8 _temp1863; _temp1863.f1= t1; _temp1863.f2=
t2; _temp1863;}); void* _temp1870; struct Cyc_Absyn_PtrInfo _temp1872; struct
Cyc_Absyn_Conref* _temp1874; struct Cyc_Absyn_Tqual _temp1876; struct Cyc_Absyn_Conref*
_temp1878; void* _temp1880; void* _temp1882; void* _temp1884; struct Cyc_Absyn_PtrInfo
_temp1886; struct Cyc_Absyn_Conref* _temp1888; struct Cyc_Absyn_Tqual _temp1890;
struct Cyc_Absyn_Conref* _temp1892; void* _temp1894; void* _temp1896; _LL1866:
_LL1885: _temp1884= _temp1864.f1; if(( unsigned int) _temp1884 >  4u?*(( int*)
_temp1884) ==  Cyc_Absyn_PointerType: 0){ _LL1887: _temp1886=(( struct Cyc_Absyn_PointerType_struct*)
_temp1884)->f1; _LL1897: _temp1896=( void*) _temp1886.elt_typ; goto _LL1895;
_LL1895: _temp1894=( void*) _temp1886.rgn_typ; goto _LL1893; _LL1893: _temp1892=
_temp1886.nullable; goto _LL1891; _LL1891: _temp1890= _temp1886.tq; goto _LL1889;
_LL1889: _temp1888= _temp1886.bounds; goto _LL1871;} else{ goto _LL1868;}
_LL1871: _temp1870= _temp1864.f2; if(( unsigned int) _temp1870 >  4u?*(( int*)
_temp1870) ==  Cyc_Absyn_PointerType: 0){ _LL1873: _temp1872=(( struct Cyc_Absyn_PointerType_struct*)
_temp1870)->f1; _LL1883: _temp1882=( void*) _temp1872.elt_typ; goto _LL1881;
_LL1881: _temp1880=( void*) _temp1872.rgn_typ; goto _LL1879; _LL1879: _temp1878=
_temp1872.nullable; goto _LL1877; _LL1877: _temp1876= _temp1872.tq; goto _LL1875;
_LL1875: _temp1874= _temp1872.bounds; goto _LL1867;} else{ goto _LL1868;}
_LL1868: goto _LL1869; _LL1867: if( _temp1890.q_const? ! _temp1876.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1892, _temp1878)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1892): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1878): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1894, _temp1880)? ! Cyc_Tcenv_region_outlives(
te, _temp1894, _temp1880): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1888, _temp1874)){ struct _tuple8 _temp1899=({ struct _tuple8 _temp1898;
_temp1898.f1= Cyc_Absyn_conref_val( _temp1888); _temp1898.f2= Cyc_Absyn_conref_val(
_temp1874); _temp1898;}); void* _temp1907; void* _temp1909; void* _temp1911;
struct Cyc_Absyn_Exp* _temp1913; void* _temp1915; struct Cyc_Absyn_Exp*
_temp1917; _LL1901: _LL1910: _temp1909= _temp1899.f1; if(( unsigned int)
_temp1909 >  1u?*(( int*) _temp1909) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1908;}
else{ goto _LL1903;} _LL1908: _temp1907= _temp1899.f2; if( _temp1907 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1902;} else{ goto _LL1903;} _LL1903: _LL1916:
_temp1915= _temp1899.f1; if(( unsigned int) _temp1915 >  1u?*(( int*) _temp1915)
==  Cyc_Absyn_Upper_b: 0){ _LL1918: _temp1917=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1915)->f1; goto _LL1912;} else{ goto _LL1905;} _LL1912: _temp1911=
_temp1899.f2; if(( unsigned int) _temp1911 >  1u?*(( int*) _temp1911) ==  Cyc_Absyn_Upper_b:
0){ _LL1914: _temp1913=(( struct Cyc_Absyn_Upper_b_struct*) _temp1911)->f1; goto
_LL1904;} else{ goto _LL1905;} _LL1905: goto _LL1906; _LL1902: goto _LL1900;
_LL1904: if( Cyc_Evexp_eval_const_uint_exp( _temp1917) <  Cyc_Evexp_eval_const_uint_exp(
_temp1913)){ return 0;} goto _LL1900; _LL1906: return 0; _LL1900:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1919=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1919->hd=( void*)({ struct _tuple8*
_temp1920=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1920->f1=
t1; _temp1920->f2= t2; _temp1920;}); _temp1919->tl= assume; _temp1919;}),
_temp1896, _temp1882); _LL1869: return 0; _LL1865:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1922=({ struct _tuple8 _temp1921;
_temp1921.f1= Cyc_Tcutil_compress( t1); _temp1921.f2= Cyc_Tcutil_compress( t2);
_temp1921;}); void* _temp1928; void* _temp1930; void* _temp1932; void* _temp1934;
_LL1924: _LL1933: _temp1932= _temp1922.f1; if(( unsigned int) _temp1932 >  4u?*((
int*) _temp1932) ==  Cyc_Absyn_IntType: 0){ _LL1935: _temp1934=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1932)->f2; goto _LL1929;} else{ goto _LL1926;}
_LL1929: _temp1928= _temp1922.f2; if(( unsigned int) _temp1928 >  4u?*(( int*)
_temp1928) ==  Cyc_Absyn_IntType: 0){ _LL1931: _temp1930=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1928)->f2; goto _LL1925;} else{ goto _LL1926;} _LL1926: goto _LL1927;
_LL1925: return _temp1934 ==  _temp1930; _LL1927: return 0; _LL1923:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=((
struct Cyc_List_List*) _check_null( tqs1))->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1938; void* _temp1939; struct Cyc_Absyn_Tqual _temp1941;
struct _tuple4* _temp1936=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs1))->hd; _temp1938=* _temp1936; _LL1942: _temp1941= _temp1938.f1;
goto _LL1940; _LL1940: _temp1939= _temp1938.f2; goto _LL1937; _LL1937: { struct
_tuple4 _temp1945; void* _temp1946; struct Cyc_Absyn_Tqual _temp1948; struct
_tuple4* _temp1943=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs2))->hd;
_temp1945=* _temp1943; _LL1949: _temp1948= _temp1945.f1; goto _LL1947; _LL1947:
_temp1946= _temp1945.f2; goto _LL1944; _LL1944: if( _temp1948.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1939, _temp1946): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1939, _temp1946)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1939,
_temp1946)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1950= t2; struct Cyc_Absyn_Uniondecl** _temp1958; struct Cyc_List_List*
_temp1960; void* _temp1962; _LL1952: if(( unsigned int) _temp1950 >  4u?*(( int*)
_temp1950) ==  Cyc_Absyn_UnionType: 0){ _LL1961: _temp1960=(( struct Cyc_Absyn_UnionType_struct*)
_temp1950)->f2; goto _LL1959; _LL1959: _temp1958=(( struct Cyc_Absyn_UnionType_struct*)
_temp1950)->f3; goto _LL1953;} else{ goto _LL1954;} _LL1954: if(( unsigned int)
_temp1950 >  4u?*(( int*) _temp1950) ==  Cyc_Absyn_IntType: 0){ _LL1963:
_temp1962=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1950)->f2; if(
_temp1962 == ( void*) Cyc_Absyn_B4){ goto _LL1955;} else{ goto _LL1956;}} else{
goto _LL1956;} _LL1956: goto _LL1957; _LL1953: if( _temp1958 ==  0){ goto
_LL1951;}{ struct Cyc_Absyn_Uniondecl* _temp1964=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1958)); if( _temp1964->fields ==  0){ goto _LL1951;}{ struct
Cyc_List_List* _temp1965=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1964->tvs, _temp1960);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1964->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp1965,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1951;}} _LL1955: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1951; _LL1957: goto _LL1951; _LL1951:;}{ void* _temp1966= t1;
struct Cyc_Absyn_PtrInfo _temp1984; struct Cyc_Absyn_Conref* _temp1986; struct
Cyc_Absyn_Tqual _temp1988; struct Cyc_Absyn_Conref* _temp1990; void* _temp1992;
void* _temp1994; struct Cyc_Absyn_Exp* _temp1996; struct Cyc_Absyn_Tqual
_temp1998; void* _temp2000; struct Cyc_Absyn_Enumdecl* _temp2002; _LL1968: if((
unsigned int) _temp1966 >  4u?*(( int*) _temp1966) ==  Cyc_Absyn_PointerType: 0){
_LL1985: _temp1984=(( struct Cyc_Absyn_PointerType_struct*) _temp1966)->f1;
_LL1995: _temp1994=( void*) _temp1984.elt_typ; goto _LL1993; _LL1993: _temp1992=(
void*) _temp1984.rgn_typ; goto _LL1991; _LL1991: _temp1990= _temp1984.nullable;
goto _LL1989; _LL1989: _temp1988= _temp1984.tq; goto _LL1987; _LL1987: _temp1986=
_temp1984.bounds; goto _LL1969;} else{ goto _LL1970;} _LL1970: if(( unsigned int)
_temp1966 >  4u?*(( int*) _temp1966) ==  Cyc_Absyn_ArrayType: 0){ _LL2001:
_temp2000=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1966)->f1; goto
_LL1999; _LL1999: _temp1998=(( struct Cyc_Absyn_ArrayType_struct*) _temp1966)->f2;
goto _LL1997; _LL1997: _temp1996=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1966)->f3; goto _LL1971;} else{ goto _LL1972;} _LL1972: if(( unsigned int)
_temp1966 >  4u?*(( int*) _temp1966) ==  Cyc_Absyn_EnumType: 0){ _LL2003:
_temp2002=(( struct Cyc_Absyn_EnumType_struct*) _temp1966)->f2; goto _LL1973;}
else{ goto _LL1974;} _LL1974: if(( unsigned int) _temp1966 >  4u?*(( int*)
_temp1966) ==  Cyc_Absyn_IntType: 0){ goto _LL1975;} else{ goto _LL1976;}
_LL1976: if( _temp1966 == ( void*) Cyc_Absyn_FloatType){ goto _LL1977;} else{
goto _LL1978;} _LL1978: if( _temp1966 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1979;} else{ goto _LL1980;} _LL1980: if( _temp1966 == ( void*) Cyc_Absyn_VoidType){
goto _LL1981;} else{ goto _LL1982;} _LL1982: goto _LL1983; _LL1969:{ void*
_temp2004= t2; struct Cyc_Absyn_PtrInfo _temp2012; struct Cyc_Absyn_Conref*
_temp2014; struct Cyc_Absyn_Tqual _temp2016; struct Cyc_Absyn_Conref* _temp2018;
void* _temp2020; void* _temp2022; _LL2006: if(( unsigned int) _temp2004 >  4u?*((
int*) _temp2004) ==  Cyc_Absyn_PointerType: 0){ _LL2013: _temp2012=(( struct Cyc_Absyn_PointerType_struct*)
_temp2004)->f1; _LL2023: _temp2022=( void*) _temp2012.elt_typ; goto _LL2021;
_LL2021: _temp2020=( void*) _temp2012.rgn_typ; goto _LL2019; _LL2019: _temp2018=
_temp2012.nullable; goto _LL2017; _LL2017: _temp2016= _temp2012.tq; goto _LL2015;
_LL2015: _temp2014= _temp2012.bounds; goto _LL2007;} else{ goto _LL2008;}
_LL2008: if( _temp2004 == ( void*) Cyc_Absyn_VoidType){ goto _LL2009;} else{
goto _LL2010;} _LL2010: goto _LL2011; _LL2007: { struct Cyc_List_List* _temp2024=({
struct Cyc_List_List* _temp2041=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2041->hd=( void*)({ struct _tuple8* _temp2042=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2042->f1= t1;
_temp2042->f2= t2; _temp2042;}); _temp2041->tl= 0; _temp2041;}); int _temp2025=
Cyc_Tcutil_ptrsubtype( te, _temp2024, _temp1994, _temp2022)? ! _temp1988.q_const?
1: _temp2016.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1986, _temp2014); if( ! bounds_ok){ struct _tuple8 _temp2027=({ struct
_tuple8 _temp2026; _temp2026.f1= Cyc_Absyn_conref_val( _temp1986); _temp2026.f2=
Cyc_Absyn_conref_val( _temp2014); _temp2026;}); void* _temp2033; struct Cyc_Absyn_Exp*
_temp2035; void* _temp2037; struct Cyc_Absyn_Exp* _temp2039; _LL2029: _LL2038:
_temp2037= _temp2027.f1; if(( unsigned int) _temp2037 >  1u?*(( int*) _temp2037)
==  Cyc_Absyn_Upper_b: 0){ _LL2040: _temp2039=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2037)->f1; goto _LL2034;} else{ goto _LL2031;} _LL2034: _temp2033=
_temp2027.f2; if(( unsigned int) _temp2033 >  1u?*(( int*) _temp2033) ==  Cyc_Absyn_Upper_b:
0){ _LL2036: _temp2035=(( struct Cyc_Absyn_Upper_b_struct*) _temp2033)->f1; goto
_LL2030;} else{ goto _LL2031;} _LL2031: goto _LL2032; _LL2030: if( Cyc_Evexp_eval_const_uint_exp(
_temp2039) >=  Cyc_Evexp_eval_const_uint_exp( _temp2035)){ bounds_ok= 1;} goto
_LL2028; _LL2032: bounds_ok= 1; goto _LL2028; _LL2028:;} return( bounds_ok?
_temp2025: 0)? Cyc_Tcutil_unify( _temp1992, _temp2020)? 1: Cyc_Tcenv_region_outlives(
te, _temp1992, _temp2020): 0;} _LL2009: return 1; _LL2011: goto _LL2005; _LL2005:;}
return 0; _LL1971:{ void* _temp2043= t2; struct Cyc_Absyn_Exp* _temp2049; struct
Cyc_Absyn_Tqual _temp2051; void* _temp2053; _LL2045: if(( unsigned int)
_temp2043 >  4u?*(( int*) _temp2043) ==  Cyc_Absyn_ArrayType: 0){ _LL2054:
_temp2053=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2043)->f1; goto
_LL2052; _LL2052: _temp2051=(( struct Cyc_Absyn_ArrayType_struct*) _temp2043)->f2;
goto _LL2050; _LL2050: _temp2049=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2043)->f3; goto _LL2046;} else{ goto _LL2047;} _LL2047: goto _LL2048;
_LL2046: { int okay; okay=( _temp1996 !=  0? _temp2049 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1996)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2049)): 0; return( okay? Cyc_Tcutil_unify(
_temp2000, _temp2053): 0)? ! _temp1998.q_const? 1: _temp2051.q_const: 0;}
_LL2048: return 0; _LL2044:;} return 0; _LL1973:{ void* _temp2055= t2; struct
Cyc_Absyn_Enumdecl* _temp2061; _LL2057: if(( unsigned int) _temp2055 >  4u?*((
int*) _temp2055) ==  Cyc_Absyn_EnumType: 0){ _LL2062: _temp2061=(( struct Cyc_Absyn_EnumType_struct*)
_temp2055)->f2; goto _LL2058;} else{ goto _LL2059;} _LL2059: goto _LL2060;
_LL2058: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2002))->fields != 
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2061))->fields !=  0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp2002))->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2061))->fields))->v): 0){ return 1;} goto _LL2056; _LL2060:
goto _LL2056; _LL2056:;} goto _LL1975; _LL1975: goto _LL1977; _LL1977: goto
_LL1979; _LL1979: return Cyc_Tcutil_coerceable( t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType;
_LL1981: return t2 == ( void*) Cyc_Absyn_VoidType; _LL1983: return 0; _LL1967:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2063= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2064=(
struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2064[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2065; _temp2065.tag= Cyc_Absyn_Cast_e;
_temp2065.f1=( void*) t; _temp2065.f2= _temp2063; _temp2065;}); _temp2064;})));
e->topt=({ struct Cyc_Core_Opt* _temp2066=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2066->v=( void*) t; _temp2066;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2067= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2069: if(( unsigned int) _temp2067
>  4u?*(( int*) _temp2067) ==  Cyc_Absyn_EnumType: 0){ goto _LL2070;} else{ goto
_LL2071;} _LL2071: if(( unsigned int) _temp2067 >  4u?*(( int*) _temp2067) == 
Cyc_Absyn_IntType: 0){ goto _LL2072;} else{ goto _LL2073;} _LL2073: if((
unsigned int) _temp2067 >  4u?*(( int*) _temp2067) ==  Cyc_Absyn_Evar: 0){ goto
_LL2074;} else{ goto _LL2075;} _LL2075: goto _LL2076; _LL2070: goto _LL2072;
_LL2072: return 1; _LL2074: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2076: return 0; _LL2068:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2077= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2079: if( _temp2077 == ( void*) Cyc_Absyn_FloatType){
goto _LL2080;} else{ goto _LL2081;} _LL2081: if( _temp2077 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2082;} else{ goto _LL2083;} _LL2083: goto _LL2084; _LL2080: goto _LL2082;
_LL2082: return 1; _LL2084: return 0; _LL2078:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp2085= Cyc_Tcutil_compress( t); _LL2087: if(( unsigned int)
_temp2085 >  4u?*(( int*) _temp2085) ==  Cyc_Absyn_FnType: 0){ goto _LL2088;}
else{ goto _LL2089;} _LL2089: goto _LL2090; _LL2088: return 1; _LL2090: return 0;
_LL2086:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp2092=({ struct _tuple8 _temp2091; _temp2091.f1= t1; _temp2091.f2=
t2; _temp2091;}); void* _temp2116; void* _temp2118; void* _temp2120; void*
_temp2122; void* _temp2124; void* _temp2126; void* _temp2128; void* _temp2130;
void* _temp2132; void* _temp2134; void* _temp2136; void* _temp2138; void*
_temp2140; void* _temp2142; void* _temp2144; void* _temp2146; void* _temp2148;
void* _temp2150; void* _temp2152; void* _temp2154; void* _temp2156; void*
_temp2158; _LL2094: _LL2117: _temp2116= _temp2092.f1; if( _temp2116 == ( void*)
Cyc_Absyn_DoubleType){ goto _LL2095;} else{ goto _LL2096;} _LL2096: _LL2119:
_temp2118= _temp2092.f2; if( _temp2118 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2097;} else{ goto _LL2098;} _LL2098: _LL2121: _temp2120= _temp2092.f1; if(
_temp2120 == ( void*) Cyc_Absyn_FloatType){ goto _LL2099;} else{ goto _LL2100;}
_LL2100: _LL2123: _temp2122= _temp2092.f2; if( _temp2122 == ( void*) Cyc_Absyn_FloatType){
goto _LL2101;} else{ goto _LL2102;} _LL2102: _LL2125: _temp2124= _temp2092.f1;
if(( unsigned int) _temp2124 >  4u?*(( int*) _temp2124) ==  Cyc_Absyn_IntType: 0){
_LL2129: _temp2128=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2124)->f1;
if( _temp2128 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2127;} else{ goto _LL2104;}
_LL2127: _temp2126=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2124)->f2;
if( _temp2126 == ( void*) Cyc_Absyn_B8){ goto _LL2103;} else{ goto _LL2104;}}
else{ goto _LL2104;} _LL2104: _LL2131: _temp2130= _temp2092.f2; if((
unsigned int) _temp2130 >  4u?*(( int*) _temp2130) ==  Cyc_Absyn_IntType: 0){
_LL2135: _temp2134=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2130)->f1;
if( _temp2134 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2133;} else{ goto _LL2106;}
_LL2133: _temp2132=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2130)->f2;
if( _temp2132 == ( void*) Cyc_Absyn_B8){ goto _LL2105;} else{ goto _LL2106;}}
else{ goto _LL2106;} _LL2106: _LL2137: _temp2136= _temp2092.f1; if((
unsigned int) _temp2136 >  4u?*(( int*) _temp2136) ==  Cyc_Absyn_IntType: 0){
_LL2141: _temp2140=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2136)->f1;
if( _temp2140 == ( void*) Cyc_Absyn_Signed){ goto _LL2139;} else{ goto _LL2108;}
_LL2139: _temp2138=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2136)->f2;
if( _temp2138 == ( void*) Cyc_Absyn_B8){ goto _LL2107;} else{ goto _LL2108;}}
else{ goto _LL2108;} _LL2108: _LL2143: _temp2142= _temp2092.f2; if((
unsigned int) _temp2142 >  4u?*(( int*) _temp2142) ==  Cyc_Absyn_IntType: 0){
_LL2147: _temp2146=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2142)->f1;
if( _temp2146 == ( void*) Cyc_Absyn_Signed){ goto _LL2145;} else{ goto _LL2110;}
_LL2145: _temp2144=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2142)->f2;
if( _temp2144 == ( void*) Cyc_Absyn_B8){ goto _LL2109;} else{ goto _LL2110;}}
else{ goto _LL2110;} _LL2110: _LL2149: _temp2148= _temp2092.f1; if((
unsigned int) _temp2148 >  4u?*(( int*) _temp2148) ==  Cyc_Absyn_IntType: 0){
_LL2153: _temp2152=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2148)->f1;
if( _temp2152 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2151;} else{ goto _LL2112;}
_LL2151: _temp2150=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2148)->f2;
if( _temp2150 == ( void*) Cyc_Absyn_B4){ goto _LL2111;} else{ goto _LL2112;}}
else{ goto _LL2112;} _LL2112: _LL2155: _temp2154= _temp2092.f2; if((
unsigned int) _temp2154 >  4u?*(( int*) _temp2154) ==  Cyc_Absyn_IntType: 0){
_LL2159: _temp2158=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2154)->f1;
if( _temp2158 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2157;} else{ goto _LL2114;}
_LL2157: _temp2156=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2154)->f2;
if( _temp2156 == ( void*) Cyc_Absyn_B4){ goto _LL2113;} else{ goto _LL2114;}}
else{ goto _LL2114;} _LL2114: goto _LL2115; _LL2095: goto _LL2097; _LL2097:
return( void*) Cyc_Absyn_DoubleType; _LL2099: goto _LL2101; _LL2101: return(
void*) Cyc_Absyn_FloatType; _LL2103: goto _LL2105; _LL2105: return Cyc_Absyn_ulonglong_t;
_LL2107: goto _LL2109; _LL2109: return Cyc_Absyn_slonglong_t; _LL2111: goto
_LL2113; _LL2113: return Cyc_Absyn_uint_t; _LL2115: return Cyc_Absyn_sint_t;
_LL2093:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2160=( void*) e->r; struct Cyc_Core_Opt* _temp2166; _LL2162: if(*(( int*)
_temp2160) ==  Cyc_Absyn_AssignOp_e){ _LL2167: _temp2166=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2160)->f2; if( _temp2166 ==  0){ goto _LL2163;} else{ goto _LL2164;}} else{
goto _LL2164;} _LL2164: goto _LL2165; _LL2163:({ void* _temp2168[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2168, sizeof( void*), 0u));}); goto _LL2161; _LL2165: goto _LL2161; _LL2161:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2169=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2169[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2170; _temp2170.tag=
Cyc_Absyn_Forward_constr; _temp2170.f1= c2; _temp2170;}); _temp2169;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2171=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2171[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2172; _temp2172.tag= Cyc_Absyn_Forward_constr;
_temp2172.f1= c1; _temp2172;}); _temp2171;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2173=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2173[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2174; _temp2174.tag= Cyc_Absyn_Forward_constr; _temp2174.f1= c1; _temp2174;});
_temp2173;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2175=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2175[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2176; _temp2176.tag= Cyc_Absyn_Forward_constr;
_temp2176.f1= c2; _temp2176;}); _temp2175;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2177=( int*) _cycalloc_atomic( sizeof( int)); _temp2177[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2177;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2181;
_temp2181.tag= Cyc_Std_Int_pa; _temp2181.f1=( int)(( unsigned int) i);{ void*
_temp2180[ 1u]={& _temp2181}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2180, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2178=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2178->name=({ struct _tagged_arr* _temp2179=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2179[ 0]=
s; _temp2179;}); _temp2178->identity= 0; _temp2178->kind= Cyc_Absyn_new_conref(
k); _temp2178;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2182=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2182, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2184; _temp2184.tag= Cyc_Std_String_pa; _temp2184.f1=( struct _tagged_arr)*
t->name;{ void* _temp2183[ 1u]={& _temp2184}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2183, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2185= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2185, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2186=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2186[ 0]=( struct _tagged_arr) _temp2185; _temp2186;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2187=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2187->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2188=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2188->v=( void*)(*
x).f1; _temp2188;}); _temp2187->f2=(* x).f2; _temp2187->f3=(* x).f3; _temp2187;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2189= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2189=({ struct Cyc_List_List* _temp2190=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2190->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2190->tl= _temp2189; _temp2190;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2191=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2191[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2192; _temp2192.tag= Cyc_Absyn_FnType; _temp2192.f1=({ struct Cyc_Absyn_FnInfo
_temp2193; _temp2193.tvars= fd->tvs; _temp2193.effect= fd->effect; _temp2193.ret_typ=(
void*)(( void*) fd->ret_type); _temp2193.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2193.c_varargs= fd->c_varargs;
_temp2193.cyc_varargs= fd->cyc_varargs; _temp2193.rgn_po= fd->rgn_po; _temp2193.attributes=
_temp2189; _temp2193;}); _temp2192;}); _temp2191;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2194=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp2194->f1=(* pr).f1; _temp2194->f2= t; _temp2194;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2195=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2195->f1=({ struct _tuple15* _temp2196=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2196->f1=(* y).f1; _temp2196->f2=(* y).f2;
_temp2196;}); _temp2195->f2=(* y).f3; _temp2195;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2199; struct _tuple15* _temp2201; struct
_tuple16 _temp2197=* w; _LL2202: _temp2201= _temp2197.f1; goto _LL2200; _LL2200:
_temp2199= _temp2197.f2; goto _LL2198; _LL2198: { struct Cyc_Absyn_Tqual
_temp2205; struct Cyc_Core_Opt* _temp2207; struct _tuple15 _temp2203=* _temp2201;
_LL2208: _temp2207= _temp2203.f1; goto _LL2206; _LL2206: _temp2205= _temp2203.f2;
goto _LL2204; _LL2204: return({ struct _tuple2* _temp2209=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2209->f1= _temp2207; _temp2209->f2=
_temp2205; _temp2209->f3= _temp2199; _temp2209;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2210=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2210->name= f->name;
_temp2210->tq= f->tq; _temp2210->type=( void*) t; _temp2210->width= f->width;
_temp2210->attributes= f->attributes; _temp2210;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2211= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2259; struct Cyc_Absyn_Structdecl** _temp2261;
struct Cyc_List_List* _temp2263; struct _tuple1* _temp2265; struct Cyc_Absyn_Uniondecl**
_temp2267; struct Cyc_List_List* _temp2269; struct _tuple1* _temp2271; struct
Cyc_Absyn_TunionInfo _temp2273; void* _temp2275; struct Cyc_List_List* _temp2277;
void* _temp2279; struct Cyc_Absyn_TunionFieldInfo _temp2281; struct Cyc_List_List*
_temp2283; void* _temp2285; struct Cyc_Core_Opt* _temp2287; struct Cyc_List_List*
_temp2289; struct _tuple1* _temp2291; struct Cyc_Absyn_Exp* _temp2293; struct
Cyc_Absyn_Tqual _temp2295; void* _temp2297; struct Cyc_Absyn_PtrInfo _temp2299;
struct Cyc_Absyn_Conref* _temp2301; struct Cyc_Absyn_Tqual _temp2303; struct Cyc_Absyn_Conref*
_temp2305; void* _temp2307; void* _temp2309; struct Cyc_Absyn_FnInfo _temp2311;
struct Cyc_List_List* _temp2313; struct Cyc_List_List* _temp2315; struct Cyc_Absyn_VarargInfo*
_temp2317; int _temp2319; struct Cyc_List_List* _temp2321; void* _temp2323;
struct Cyc_Core_Opt* _temp2325; struct Cyc_List_List* _temp2327; struct Cyc_List_List*
_temp2329; struct Cyc_List_List* _temp2331; struct Cyc_List_List* _temp2333;
struct Cyc_Core_Opt* _temp2335; void* _temp2337; void* _temp2339; void*
_temp2341; struct Cyc_List_List* _temp2343; _LL2213: if(( unsigned int)
_temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_VarType: 0){ _LL2260:
_temp2259=(( struct Cyc_Absyn_VarType_struct*) _temp2211)->f1; goto _LL2214;}
else{ goto _LL2215;} _LL2215: if(( unsigned int) _temp2211 >  4u?*(( int*)
_temp2211) ==  Cyc_Absyn_StructType: 0){ _LL2266: _temp2265=(( struct Cyc_Absyn_StructType_struct*)
_temp2211)->f1; goto _LL2264; _LL2264: _temp2263=(( struct Cyc_Absyn_StructType_struct*)
_temp2211)->f2; goto _LL2262; _LL2262: _temp2261=(( struct Cyc_Absyn_StructType_struct*)
_temp2211)->f3; goto _LL2216;} else{ goto _LL2217;} _LL2217: if(( unsigned int)
_temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_UnionType: 0){ _LL2272:
_temp2271=(( struct Cyc_Absyn_UnionType_struct*) _temp2211)->f1; goto _LL2270;
_LL2270: _temp2269=(( struct Cyc_Absyn_UnionType_struct*) _temp2211)->f2; goto
_LL2268; _LL2268: _temp2267=(( struct Cyc_Absyn_UnionType_struct*) _temp2211)->f3;
goto _LL2218;} else{ goto _LL2219;} _LL2219: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_TunionType: 0){ _LL2274: _temp2273=(( struct Cyc_Absyn_TunionType_struct*)
_temp2211)->f1; _LL2280: _temp2279=( void*) _temp2273.tunion_info; goto _LL2278;
_LL2278: _temp2277= _temp2273.targs; goto _LL2276; _LL2276: _temp2275=( void*)
_temp2273.rgn; goto _LL2220;} else{ goto _LL2221;} _LL2221: if(( unsigned int)
_temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2282:
_temp2281=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2211)->f1; _LL2286:
_temp2285=( void*) _temp2281.field_info; goto _LL2284; _LL2284: _temp2283=
_temp2281.targs; goto _LL2222;} else{ goto _LL2223;} _LL2223: if(( unsigned int)
_temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_TypedefType: 0){ _LL2292:
_temp2291=(( struct Cyc_Absyn_TypedefType_struct*) _temp2211)->f1; goto _LL2290;
_LL2290: _temp2289=(( struct Cyc_Absyn_TypedefType_struct*) _temp2211)->f2; goto
_LL2288; _LL2288: _temp2287=(( struct Cyc_Absyn_TypedefType_struct*) _temp2211)->f3;
goto _LL2224;} else{ goto _LL2225;} _LL2225: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_ArrayType: 0){ _LL2298: _temp2297=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2211)->f1; goto _LL2296; _LL2296:
_temp2295=(( struct Cyc_Absyn_ArrayType_struct*) _temp2211)->f2; goto _LL2294;
_LL2294: _temp2293=(( struct Cyc_Absyn_ArrayType_struct*) _temp2211)->f3; goto
_LL2226;} else{ goto _LL2227;} _LL2227: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_PointerType: 0){ _LL2300: _temp2299=(( struct Cyc_Absyn_PointerType_struct*)
_temp2211)->f1; _LL2310: _temp2309=( void*) _temp2299.elt_typ; goto _LL2308;
_LL2308: _temp2307=( void*) _temp2299.rgn_typ; goto _LL2306; _LL2306: _temp2305=
_temp2299.nullable; goto _LL2304; _LL2304: _temp2303= _temp2299.tq; goto _LL2302;
_LL2302: _temp2301= _temp2299.bounds; goto _LL2228;} else{ goto _LL2229;}
_LL2229: if(( unsigned int) _temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_FnType:
0){ _LL2312: _temp2311=(( struct Cyc_Absyn_FnType_struct*) _temp2211)->f1;
_LL2328: _temp2327= _temp2311.tvars; goto _LL2326; _LL2326: _temp2325= _temp2311.effect;
goto _LL2324; _LL2324: _temp2323=( void*) _temp2311.ret_typ; goto _LL2322;
_LL2322: _temp2321= _temp2311.args; goto _LL2320; _LL2320: _temp2319= _temp2311.c_varargs;
goto _LL2318; _LL2318: _temp2317= _temp2311.cyc_varargs; goto _LL2316; _LL2316:
_temp2315= _temp2311.rgn_po; goto _LL2314; _LL2314: _temp2313= _temp2311.attributes;
goto _LL2230;} else{ goto _LL2231;} _LL2231: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_TupleType: 0){ _LL2330: _temp2329=(( struct Cyc_Absyn_TupleType_struct*)
_temp2211)->f1; goto _LL2232;} else{ goto _LL2233;} _LL2233: if(( unsigned int)
_temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_AnonStructType: 0){ _LL2332:
_temp2331=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2211)->f1; goto
_LL2234;} else{ goto _LL2235;} _LL2235: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2334: _temp2333=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2211)->f1; goto _LL2236;} else{ goto
_LL2237;} _LL2237: if(( unsigned int) _temp2211 >  4u?*(( int*) _temp2211) == 
Cyc_Absyn_Evar: 0){ _LL2336: _temp2335=(( struct Cyc_Absyn_Evar_struct*)
_temp2211)->f2; goto _LL2238;} else{ goto _LL2239;} _LL2239: if(( unsigned int)
_temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2338:
_temp2337=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2211)->f1;
goto _LL2240;} else{ goto _LL2241;} _LL2241: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_EnumType: 0){ goto _LL2242;} else{ goto _LL2243;}
_LL2243: if( _temp2211 == ( void*) Cyc_Absyn_VoidType){ goto _LL2244;} else{
goto _LL2245;} _LL2245: if(( unsigned int) _temp2211 >  4u?*(( int*) _temp2211)
==  Cyc_Absyn_IntType: 0){ goto _LL2246;} else{ goto _LL2247;} _LL2247: if(
_temp2211 == ( void*) Cyc_Absyn_FloatType){ goto _LL2248;} else{ goto _LL2249;}
_LL2249: if( _temp2211 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2250;} else{
goto _LL2251;} _LL2251: if( _temp2211 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL2252;} else{ goto _LL2253;} _LL2253: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_RgnsEff: 0){ _LL2340: _temp2339=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2211)->f1; goto _LL2254;} else{ goto _LL2255;}
_LL2255: if(( unsigned int) _temp2211 >  4u?*(( int*) _temp2211) ==  Cyc_Absyn_AccessEff:
0){ _LL2342: _temp2341=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2211)->f1;
goto _LL2256;} else{ goto _LL2257;} _LL2257: if(( unsigned int) _temp2211 >  4u?*((
int*) _temp2211) ==  Cyc_Absyn_JoinEff: 0){ _LL2344: _temp2343=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2211)->f1; goto _LL2258;} else{ goto _LL2212;} _LL2214: { struct
_handler_cons _temp2345; _push_handler(& _temp2345);{ int _temp2347= 0; if(
setjmp( _temp2345.handler)){ _temp2347= 1;} if( ! _temp2347){{ void* _temp2348=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2259); _npop_handler( 0u); return _temp2348;}; _pop_handler();} else{
void* _temp2346=( void*) _exn_thrown; void* _temp2350= _temp2346; _LL2352: if(
_temp2350 ==  Cyc_Core_Not_found){ goto _LL2353;} else{ goto _LL2354;} _LL2354:
goto _LL2355; _LL2353: return t; _LL2355:( void) _throw( _temp2350); _LL2351:;}}}
_LL2216: { struct Cyc_List_List* _temp2356= Cyc_Tcutil_substs( rgn, inst,
_temp2263); return _temp2356 ==  _temp2263? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2357=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2357[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2358; _temp2358.tag=
Cyc_Absyn_StructType; _temp2358.f1= _temp2265; _temp2358.f2= _temp2356;
_temp2358.f3= _temp2261; _temp2358;}); _temp2357;});} _LL2218: { struct Cyc_List_List*
_temp2359= Cyc_Tcutil_substs( rgn, inst, _temp2269); return _temp2359 == 
_temp2269? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2360=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2360[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2361; _temp2361.tag= Cyc_Absyn_UnionType;
_temp2361.f1= _temp2271; _temp2361.f2= _temp2359; _temp2361.f3= _temp2267;
_temp2361;}); _temp2360;});} _LL2220: { struct Cyc_List_List* _temp2362= Cyc_Tcutil_substs(
rgn, inst, _temp2277); void* _temp2363= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2275); return( _temp2362 ==  _temp2277? _temp2363 ==  _temp2275: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2364=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2364[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2365; _temp2365.tag= Cyc_Absyn_TunionType;
_temp2365.f1=({ struct Cyc_Absyn_TunionInfo _temp2366; _temp2366.tunion_info=(
void*) _temp2279; _temp2366.targs= _temp2362; _temp2366.rgn=( void*) _temp2363;
_temp2366;}); _temp2365;}); _temp2364;});} _LL2222: { struct Cyc_List_List*
_temp2367= Cyc_Tcutil_substs( rgn, inst, _temp2283); return _temp2367 == 
_temp2283? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2368=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2368[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2369; _temp2369.tag=
Cyc_Absyn_TunionFieldType; _temp2369.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2370; _temp2370.field_info=( void*) _temp2285; _temp2370.targs= _temp2367;
_temp2370;}); _temp2369;}); _temp2368;});} _LL2224: { struct Cyc_List_List*
_temp2371= Cyc_Tcutil_substs( rgn, inst, _temp2289); return _temp2371 == 
_temp2289? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2372=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2372[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2373; _temp2373.tag=
Cyc_Absyn_TypedefType; _temp2373.f1= _temp2291; _temp2373.f2= _temp2371;
_temp2373.f3= _temp2287; _temp2373;}); _temp2372;});} _LL2226: { void* _temp2374=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2297); return _temp2374 ==  _temp2297? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2375=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2375[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2376; _temp2376.tag= Cyc_Absyn_ArrayType;
_temp2376.f1=( void*) _temp2374; _temp2376.f2= _temp2295; _temp2376.f3=
_temp2293; _temp2376;}); _temp2375;});} _LL2228: { void* _temp2377= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2309); void* _temp2378= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2307); if( _temp2377 ==  _temp2309? _temp2378 ==  _temp2307: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2379=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2379[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2380; _temp2380.tag= Cyc_Absyn_PointerType;
_temp2380.f1=({ struct Cyc_Absyn_PtrInfo _temp2381; _temp2381.elt_typ=( void*)
_temp2377; _temp2381.rgn_typ=( void*) _temp2378; _temp2381.nullable= _temp2305;
_temp2381.tq= _temp2303; _temp2381.bounds= _temp2301; _temp2381;}); _temp2380;});
_temp2379;});} _LL2230:{ struct Cyc_List_List* _temp2382= _temp2327; for( 0;
_temp2382 !=  0; _temp2382=(( struct Cyc_List_List*) _check_null( _temp2382))->tl){
inst=({ struct Cyc_List_List* _temp2383=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2383->hd=( void*)({ struct _tuple5*
_temp2384=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2384->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2382))->hd; _temp2384->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2385=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2385[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2386; _temp2386.tag= Cyc_Absyn_VarType;
_temp2386.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2382))->hd; _temp2386;}); _temp2385;}); _temp2384;}); _temp2383->tl= inst;
_temp2383;});}}{ struct Cyc_List_List* _temp2389; struct Cyc_List_List*
_temp2391; struct _tuple0 _temp2387=(( struct _tuple0(*)( struct _RegionHandle*
r1, struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2321)); _LL2392:
_temp2391= _temp2387.f1; goto _LL2390; _LL2390: _temp2389= _temp2387.f2; goto
_LL2388; _LL2388: { struct Cyc_List_List* _temp2393= Cyc_Tcutil_substs( rgn,
inst, _temp2389); struct Cyc_List_List* _temp2394=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r1,
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2391, _temp2393)); struct Cyc_Core_Opt* eff2; if( _temp2325 ==  0){
eff2= 0;} else{ void* _temp2395= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2325))->v); if( _temp2395 == ( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2325))->v){ eff2= _temp2325;} else{ eff2=({
struct Cyc_Core_Opt* _temp2396=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2396->v=( void*) _temp2395; _temp2396;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2317 ==  0){ cyc_varargs2= 0;} else{ int _temp2399; void*
_temp2401; struct Cyc_Absyn_Tqual _temp2403; struct Cyc_Core_Opt* _temp2405;
struct Cyc_Absyn_VarargInfo _temp2397=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2317)); _LL2406: _temp2405= _temp2397.name; goto _LL2404;
_LL2404: _temp2403= _temp2397.tq; goto _LL2402; _LL2402: _temp2401=( void*)
_temp2397.type; goto _LL2400; _LL2400: _temp2399= _temp2397.inject; goto _LL2398;
_LL2398: { void* _temp2407= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2401); if(
_temp2407 ==  _temp2401){ cyc_varargs2= _temp2317;} else{ cyc_varargs2=({ struct
Cyc_Absyn_VarargInfo* _temp2408=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2408->name= _temp2405; _temp2408->tq=
_temp2403; _temp2408->type=( void*) _temp2407; _temp2408->inject= _temp2399;
_temp2408;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2411;
struct Cyc_List_List* _temp2413; struct _tuple0 _temp2409= Cyc_List_rsplit( rgn,
rgn, _temp2315); _LL2414: _temp2413= _temp2409.f1; goto _LL2412; _LL2412:
_temp2411= _temp2409.f2; goto _LL2410; _LL2410: { struct Cyc_List_List*
_temp2415= Cyc_Tcutil_substs( rgn, inst, _temp2413); struct Cyc_List_List*
_temp2416= Cyc_Tcutil_substs( rgn, inst, _temp2411); if( _temp2415 ==  _temp2413?
_temp2416 ==  _temp2411: 0){ rgn_po2= _temp2315;} else{ rgn_po2= Cyc_List_zip(
_temp2415, _temp2416);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2417=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2417[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2418; _temp2418.tag= Cyc_Absyn_FnType;
_temp2418.f1=({ struct Cyc_Absyn_FnInfo _temp2419; _temp2419.tvars= _temp2327;
_temp2419.effect= eff2; _temp2419.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2323); _temp2419.args= _temp2394; _temp2419.c_varargs= _temp2319;
_temp2419.cyc_varargs= cyc_varargs2; _temp2419.rgn_po= rgn_po2; _temp2419.attributes=
_temp2313; _temp2419;}); _temp2418;}); _temp2417;});}}}}} _LL2232: { struct Cyc_List_List*
_temp2420=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2329); struct Cyc_List_List* _temp2421= Cyc_Tcutil_substs( rgn, inst,
_temp2420); if( _temp2421 ==  _temp2420){ return t;}{ struct Cyc_List_List*
_temp2422=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2329, _temp2421); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2423=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2423[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2424; _temp2424.tag= Cyc_Absyn_TupleType;
_temp2424.f1= _temp2422; _temp2424;}); _temp2423;});}} _LL2234: { struct Cyc_List_List*
_temp2425=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2331); struct Cyc_List_List* _temp2426= Cyc_Tcutil_substs( rgn, inst,
_temp2425); if( _temp2426 ==  _temp2425){ return t;}{ struct Cyc_List_List*
_temp2427=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2331, _temp2426); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2428=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2428[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2429; _temp2429.tag= Cyc_Absyn_AnonStructType;
_temp2429.f1= _temp2427; _temp2429;}); _temp2428;});}} _LL2236: { struct Cyc_List_List*
_temp2430=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2333); struct Cyc_List_List* _temp2431= Cyc_Tcutil_substs( rgn, inst,
_temp2430); if( _temp2431 ==  _temp2430){ return t;}{ struct Cyc_List_List*
_temp2432=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2333, _temp2431); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2433=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2433[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2434; _temp2434.tag= Cyc_Absyn_AnonStructType;
_temp2434.f1= _temp2432; _temp2434;}); _temp2433;});}} _LL2238: if( _temp2335 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2335))->v);} else{ return t;} _LL2240: { void* _temp2435= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2337); return _temp2435 ==  _temp2337? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2436=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2436[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2437; _temp2437.tag= Cyc_Absyn_RgnHandleType; _temp2437.f1=( void*)
_temp2435; _temp2437;}); _temp2436;});} _LL2242: return t; _LL2244: return t;
_LL2246: return t; _LL2248: return t; _LL2250: return t; _LL2252: return t;
_LL2254: { void* _temp2438= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2339);
return _temp2438 ==  _temp2339? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2439=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2439[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2440; _temp2440.tag= Cyc_Absyn_RgnsEff;
_temp2440.f1=( void*) _temp2438; _temp2440;}); _temp2439;});} _LL2256: { void*
_temp2441= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2341); return _temp2441 == 
_temp2341? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2442=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2442[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2443; _temp2443.tag= Cyc_Absyn_AccessEff;
_temp2443.f1=( void*) _temp2441; _temp2443;}); _temp2442;});} _LL2258: { struct
Cyc_List_List* _temp2444= Cyc_Tcutil_substs( rgn, inst, _temp2343); return
_temp2444 ==  _temp2343? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2445=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2445[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2446; _temp2446.tag= Cyc_Absyn_JoinEff;
_temp2446.f1= _temp2444; _temp2446;}); _temp2445;});} _LL2212:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2447=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2448=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2449= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2447); struct Cyc_List_List* _temp2450= Cyc_Tcutil_substs( rgn,
inst, _temp2448); if( _temp2447 ==  _temp2449? _temp2448 ==  _temp2450: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2451=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2451->hd=(
void*) _temp2449; _temp2451->tl= _temp2450; _temp2451;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2496=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2496[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2497; _temp2497.tag= Cyc_Absyn_Const_e; _temp2497.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2497;}); _temp2496;}), loc);{ void*
_temp2452= Cyc_Tcutil_compress( t); void* _temp2466; void* _temp2468; void*
_temp2470; void* _temp2472; _LL2454: if(( unsigned int) _temp2452 >  4u?*(( int*)
_temp2452) ==  Cyc_Absyn_PointerType: 0){ goto _LL2455;} else{ goto _LL2456;}
_LL2456: if(( unsigned int) _temp2452 >  4u?*(( int*) _temp2452) ==  Cyc_Absyn_IntType:
0){ _LL2469: _temp2468=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2452)->f1;
goto _LL2467; _LL2467: _temp2466=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2452)->f2; if( _temp2466 == ( void*) Cyc_Absyn_B1){ goto _LL2457;} else{
goto _LL2458;}} else{ goto _LL2458;} _LL2458: if(( unsigned int) _temp2452 >  4u?*((
int*) _temp2452) ==  Cyc_Absyn_IntType: 0){ _LL2473: _temp2472=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2452)->f1; goto _LL2471; _LL2471: _temp2470=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2452)->f2; goto _LL2459;} else{
goto _LL2460;} _LL2460: if( _temp2452 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2461;} else{ goto _LL2462;} _LL2462: if( _temp2452 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2463;} else{ goto _LL2464;} _LL2464: goto _LL2465; _LL2455: goto _LL2453;
_LL2457:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2474=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2474[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2475; _temp2475.tag= Cyc_Absyn_Const_e;
_temp2475.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2476=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2476[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2477; _temp2477.tag= Cyc_Absyn_Char_c;
_temp2477.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2477.f2='\000'; _temp2477;});
_temp2476;})); _temp2475;}); _temp2474;}))); goto _LL2453; _LL2459:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2478=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2478[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2479; _temp2479.tag= Cyc_Absyn_Const_e; _temp2479.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2480=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2480[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2481; _temp2481.tag= Cyc_Absyn_Int_c; _temp2481.f1=( void*) _temp2472;
_temp2481.f2= 0; _temp2481;}); _temp2480;})); _temp2479;}); _temp2478;}))); if(
_temp2470 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2482=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2482[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2483; _temp2483.tag= Cyc_Absyn_Cast_e;
_temp2483.f1=( void*) t; _temp2483.f2= e; _temp2483;}); _temp2482;}), loc);}
goto _LL2453; _LL2461:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2484=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2484[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2485; _temp2485.tag= Cyc_Absyn_Const_e;
_temp2485.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2486=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2486[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2487; _temp2487.tag= Cyc_Absyn_Float_c;
_temp2487.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2487;});
_temp2486;})); _temp2485;}); _temp2484;}))); goto _LL2453; _LL2463:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2488=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2488[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2489; _temp2489.tag= Cyc_Absyn_Cast_e; _temp2489.f1=( void*) t; _temp2489.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2490=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2490[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2491; _temp2491.tag= Cyc_Absyn_Const_e;
_temp2491.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2492=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2492[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2493; _temp2493.tag= Cyc_Absyn_Float_c;
_temp2493.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2493;});
_temp2492;})); _temp2491;}); _temp2490;}), loc); _temp2489;}); _temp2488;})));
goto _LL2453; _LL2465:({ struct Cyc_Std_String_pa_struct _temp2495; _temp2495.tag=
Cyc_Std_String_pa; _temp2495.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2494[ 1u]={& _temp2495}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2494, sizeof( void*), 1u));}});
goto _LL2453; _LL2453:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2498=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp2498->f1= tv; _temp2498->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2499=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2499->v=( void*) k; _temp2499;}),({ struct Cyc_Core_Opt*
_temp2500=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2500->v=( void*) s; _temp2500;})); _temp2498;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2503;
struct _RegionHandle* _temp2504; struct Cyc_List_List* _temp2506; struct _tuple6*
_temp2501= env; _temp2503=* _temp2501; _LL2507: _temp2506= _temp2503.f1; goto
_LL2505; _LL2505: _temp2504= _temp2503.f2; goto _LL2502; _LL2502: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2508=( struct _tuple5*) _region_malloc(
_temp2504, sizeof( struct _tuple5)); _temp2508->f1= tv; _temp2508->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2509=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2509->v=( void*) k; _temp2509;}),({ struct Cyc_Core_Opt*
_temp2510=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2510->v=( void*) _temp2506; _temp2510;})); _temp2508;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_Std_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_Std_String_pa_struct _temp2514; _temp2514.tag= Cyc_Std_String_pa;
_temp2514.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct
_temp2513; _temp2513.tag= Cyc_Std_String_pa; _temp2513.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_Std_String_pa_struct _temp2512;
_temp2512.tag= Cyc_Std_String_pa; _temp2512.f1=( struct _tagged_arr)* tv->name;{
void* _temp2511[ 3u]={& _temp2512,& _temp2513,& _temp2514}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2511, sizeof( void*), 3u));}}}});} if( tv->identity
==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!= *(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2515[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2515, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2516=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2516->hd=( void*) tv; _temp2516->tl= tvs; _temp2516;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2517[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2517, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity ==  0){({ void* _temp2518[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2518, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2519=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2519->hd=( void*) tv; _temp2519->tl=
tvs; _temp2519;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp2521; _temp2521.tag= Cyc_Std_String_pa;
_temp2521.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2520[
1u]={& _temp2521};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2520, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2522=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2522->hd=(
void*) tv; _temp2522->tl= tvs; _temp2522;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2523= Cyc_Tcutil_compress( e);
int _temp2529; _LL2525: if(( unsigned int) _temp2523 >  4u?*(( int*) _temp2523)
==  Cyc_Absyn_Evar: 0){ _LL2530: _temp2529=(( struct Cyc_Absyn_Evar_struct*)
_temp2523)->f3; goto _LL2526;} else{ goto _LL2527;} _LL2527: goto _LL2528;
_LL2526:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2531= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2537; _LL2533: if((
unsigned int) _temp2531 >  4u?*(( int*) _temp2531) ==  Cyc_Absyn_Evar: 0){
_LL2538: _temp2537=(( struct Cyc_Absyn_Evar_struct*) _temp2531)->f3; goto
_LL2534;} else{ goto _LL2535;} _LL2535: goto _LL2536; _LL2534: if( _temp2529 == 
_temp2537){ return es;} goto _LL2532; _LL2536: goto _LL2532; _LL2532:;}} return({
struct Cyc_List_List* _temp2539=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2539->hd=( void*) e; _temp2539->tl= es; _temp2539;});
_LL2528: return es; _LL2524:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b !=  0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) == *(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2540=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2540->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2540->tl= r; _temp2540;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp2542; _temp2542.tag= Cyc_Std_String_pa; _temp2542.f1=( struct _tagged_arr)*
fn;{ void* _temp2541[ 1u]={& _temp2542}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2541, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2543= Cyc_Tcutil_compress( field_typ); void* _temp2549;
_LL2545: if(( unsigned int) _temp2543 >  4u?*(( int*) _temp2543) ==  Cyc_Absyn_IntType:
0){ _LL2550: _temp2549=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2543)->f2;
goto _LL2546;} else{ goto _LL2547;} _LL2547: goto _LL2548; _LL2546:{ void*
_temp2551= _temp2549; _LL2553: if( _temp2551 == ( void*) Cyc_Absyn_B1){ goto
_LL2554;} else{ goto _LL2555;} _LL2555: if( _temp2551 == ( void*) Cyc_Absyn_B2){
goto _LL2556;} else{ goto _LL2557;} _LL2557: if( _temp2551 == ( void*) Cyc_Absyn_B4){
goto _LL2558;} else{ goto _LL2559;} _LL2559: if( _temp2551 == ( void*) Cyc_Absyn_B8){
goto _LL2560;} else{ goto _LL2552;} _LL2554: if( w >  8){({ void* _temp2561[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2561, sizeof( void*), 0u));});} goto
_LL2552; _LL2556: if( w >  16){({ void* _temp2562[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2562, sizeof( void*), 0u));});} goto _LL2552; _LL2558: if( w >  32){({ void*
_temp2563[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2563, sizeof( void*), 0u));});}
goto _LL2552; _LL2560: if( w >  64){({ void* _temp2564[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2564, sizeof( void*), 0u));});} goto _LL2552; _LL2552:;} goto
_LL2544; _LL2548:({ struct Cyc_Std_String_pa_struct _temp2567; _temp2567.tag=
Cyc_Std_String_pa; _temp2567.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp2566; _temp2566.tag= Cyc_Std_String_pa;
_temp2566.f1=( struct _tagged_arr)* fn;{ void* _temp2565[ 2u]={& _temp2566,&
_temp2567}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2565, sizeof( void*), 2u));}}});
goto _LL2544; _LL2544:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp2568=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL2570: if( _temp2568 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL2571;} else{ goto _LL2572;} _LL2572: if((
unsigned int) _temp2568 >  16u?*(( int*) _temp2568) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2573;} else{ goto _LL2574;} _LL2574: goto _LL2575; _LL2571: continue;
_LL2573: continue; _LL2575:({ struct Cyc_Std_String_pa_struct _temp2578;
_temp2578.tag= Cyc_Std_String_pa; _temp2578.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp2577; _temp2577.tag= Cyc_Std_String_pa;
_temp2577.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp2576[ 2u]={& _temp2577,&
_temp2578}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2576, sizeof( void*), 2u));}}});
goto _LL2569; _LL2569:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv
cvtenv, void* expected_kind, void* t){{ void* _temp2579= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp2627; struct Cyc_Core_Opt** _temp2629; struct Cyc_Core_Opt*
_temp2630; struct Cyc_Core_Opt** _temp2632; struct Cyc_Absyn_Tvar* _temp2633;
struct Cyc_Absyn_Enumdecl* _temp2635; struct Cyc_Absyn_Enumdecl** _temp2637;
struct _tuple1* _temp2638; struct Cyc_Absyn_TunionInfo _temp2640; void*
_temp2642; struct Cyc_List_List* _temp2644; void* _temp2646; void** _temp2648;
struct Cyc_Absyn_TunionFieldInfo _temp2649; struct Cyc_List_List* _temp2651;
void* _temp2653; void** _temp2655; struct Cyc_Absyn_PtrInfo _temp2656; struct
Cyc_Absyn_Conref* _temp2658; struct Cyc_Absyn_Tqual _temp2660; struct Cyc_Absyn_Conref*
_temp2662; void* _temp2664; void* _temp2666; struct Cyc_Absyn_Exp* _temp2668;
struct Cyc_Absyn_Tqual _temp2670; void* _temp2672; struct Cyc_Absyn_FnInfo
_temp2674; struct Cyc_List_List* _temp2676; struct Cyc_List_List* _temp2678;
struct Cyc_Absyn_VarargInfo* _temp2680; int _temp2682; struct Cyc_List_List*
_temp2684; void* _temp2686; struct Cyc_Core_Opt* _temp2688; struct Cyc_Core_Opt**
_temp2690; struct Cyc_List_List* _temp2691; struct Cyc_List_List** _temp2693;
struct Cyc_List_List* _temp2694; struct Cyc_List_List* _temp2696; struct Cyc_List_List*
_temp2698; struct Cyc_Absyn_Structdecl** _temp2700; struct Cyc_Absyn_Structdecl***
_temp2702; struct Cyc_List_List* _temp2703; struct _tuple1* _temp2705; struct
Cyc_Absyn_Uniondecl** _temp2707; struct Cyc_Absyn_Uniondecl*** _temp2709; struct
Cyc_List_List* _temp2710; struct _tuple1* _temp2712; struct Cyc_Core_Opt*
_temp2714; struct Cyc_Core_Opt** _temp2716; struct Cyc_List_List* _temp2717;
struct _tuple1* _temp2719; void* _temp2721; void* _temp2723; void* _temp2725;
struct Cyc_List_List* _temp2727; _LL2581: if( _temp2579 == ( void*) Cyc_Absyn_VoidType){
goto _LL2582;} else{ goto _LL2583;} _LL2583: if(( unsigned int) _temp2579 >  4u?*((
int*) _temp2579) ==  Cyc_Absyn_Evar: 0){ _LL2631: _temp2630=(( struct Cyc_Absyn_Evar_struct*)
_temp2579)->f1; _temp2632=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp2579)->f1; goto _LL2628; _LL2628: _temp2627=(( struct Cyc_Absyn_Evar_struct*)
_temp2579)->f2; _temp2629=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp2579)->f2; goto _LL2584;} else{ goto _LL2585;} _LL2585: if(( unsigned int)
_temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_VarType: 0){ _LL2634:
_temp2633=(( struct Cyc_Absyn_VarType_struct*) _temp2579)->f1; goto _LL2586;}
else{ goto _LL2587;} _LL2587: if(( unsigned int) _temp2579 >  4u?*(( int*)
_temp2579) ==  Cyc_Absyn_EnumType: 0){ _LL2639: _temp2638=(( struct Cyc_Absyn_EnumType_struct*)
_temp2579)->f1; goto _LL2636; _LL2636: _temp2635=(( struct Cyc_Absyn_EnumType_struct*)
_temp2579)->f2; _temp2637=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2579)->f2; goto _LL2588;} else{ goto _LL2589;} _LL2589: if(( unsigned int)
_temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_TunionType: 0){ _LL2641:
_temp2640=(( struct Cyc_Absyn_TunionType_struct*) _temp2579)->f1; _LL2647:
_temp2646=( void*) _temp2640.tunion_info; _temp2648=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2579)->f1).tunion_info; goto _LL2645; _LL2645: _temp2644= _temp2640.targs;
goto _LL2643; _LL2643: _temp2642=( void*) _temp2640.rgn; goto _LL2590;} else{
goto _LL2591;} _LL2591: if(( unsigned int) _temp2579 >  4u?*(( int*) _temp2579)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2650: _temp2649=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2579)->f1; _LL2654: _temp2653=( void*) _temp2649.field_info; _temp2655=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2579)->f1).field_info;
goto _LL2652; _LL2652: _temp2651= _temp2649.targs; goto _LL2592;} else{ goto
_LL2593;} _LL2593: if(( unsigned int) _temp2579 >  4u?*(( int*) _temp2579) == 
Cyc_Absyn_PointerType: 0){ _LL2657: _temp2656=(( struct Cyc_Absyn_PointerType_struct*)
_temp2579)->f1; _LL2667: _temp2666=( void*) _temp2656.elt_typ; goto _LL2665;
_LL2665: _temp2664=( void*) _temp2656.rgn_typ; goto _LL2663; _LL2663: _temp2662=
_temp2656.nullable; goto _LL2661; _LL2661: _temp2660= _temp2656.tq; goto _LL2659;
_LL2659: _temp2658= _temp2656.bounds; goto _LL2594;} else{ goto _LL2595;}
_LL2595: if(( unsigned int) _temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_IntType:
0){ goto _LL2596;} else{ goto _LL2597;} _LL2597: if( _temp2579 == ( void*) Cyc_Absyn_FloatType){
goto _LL2598;} else{ goto _LL2599;} _LL2599: if( _temp2579 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2600;} else{ goto _LL2601;} _LL2601: if(( unsigned int) _temp2579 >  4u?*((
int*) _temp2579) ==  Cyc_Absyn_ArrayType: 0){ _LL2673: _temp2672=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2579)->f1; goto _LL2671; _LL2671:
_temp2670=(( struct Cyc_Absyn_ArrayType_struct*) _temp2579)->f2; goto _LL2669;
_LL2669: _temp2668=(( struct Cyc_Absyn_ArrayType_struct*) _temp2579)->f3; goto
_LL2602;} else{ goto _LL2603;} _LL2603: if(( unsigned int) _temp2579 >  4u?*((
int*) _temp2579) ==  Cyc_Absyn_FnType: 0){ _LL2675: _temp2674=(( struct Cyc_Absyn_FnType_struct*)
_temp2579)->f1; _LL2692: _temp2691= _temp2674.tvars; _temp2693=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2579)->f1).tvars; goto _LL2689; _LL2689:
_temp2688= _temp2674.effect; _temp2690=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2579)->f1).effect; goto _LL2687; _LL2687: _temp2686=( void*) _temp2674.ret_typ;
goto _LL2685; _LL2685: _temp2684= _temp2674.args; goto _LL2683; _LL2683:
_temp2682= _temp2674.c_varargs; goto _LL2681; _LL2681: _temp2680= _temp2674.cyc_varargs;
goto _LL2679; _LL2679: _temp2678= _temp2674.rgn_po; goto _LL2677; _LL2677:
_temp2676= _temp2674.attributes; goto _LL2604;} else{ goto _LL2605;} _LL2605:
if(( unsigned int) _temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_TupleType:
0){ _LL2695: _temp2694=(( struct Cyc_Absyn_TupleType_struct*) _temp2579)->f1;
goto _LL2606;} else{ goto _LL2607;} _LL2607: if(( unsigned int) _temp2579 >  4u?*((
int*) _temp2579) ==  Cyc_Absyn_AnonStructType: 0){ _LL2697: _temp2696=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2579)->f1; goto _LL2608;} else{ goto
_LL2609;} _LL2609: if(( unsigned int) _temp2579 >  4u?*(( int*) _temp2579) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2699: _temp2698=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2579)->f1; goto _LL2610;} else{ goto _LL2611;} _LL2611: if(( unsigned int)
_temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_StructType: 0){ _LL2706:
_temp2705=(( struct Cyc_Absyn_StructType_struct*) _temp2579)->f1; goto _LL2704;
_LL2704: _temp2703=(( struct Cyc_Absyn_StructType_struct*) _temp2579)->f2; goto
_LL2701; _LL2701: _temp2700=(( struct Cyc_Absyn_StructType_struct*) _temp2579)->f3;
_temp2702=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2579)->f3; goto _LL2612;} else{ goto _LL2613;} _LL2613: if(( unsigned int)
_temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_UnionType: 0){ _LL2713:
_temp2712=(( struct Cyc_Absyn_UnionType_struct*) _temp2579)->f1; goto _LL2711;
_LL2711: _temp2710=(( struct Cyc_Absyn_UnionType_struct*) _temp2579)->f2; goto
_LL2708; _LL2708: _temp2707=(( struct Cyc_Absyn_UnionType_struct*) _temp2579)->f3;
_temp2709=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2579)->f3; goto _LL2614;} else{ goto _LL2615;} _LL2615: if(( unsigned int)
_temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_TypedefType: 0){ _LL2720:
_temp2719=(( struct Cyc_Absyn_TypedefType_struct*) _temp2579)->f1; goto _LL2718;
_LL2718: _temp2717=(( struct Cyc_Absyn_TypedefType_struct*) _temp2579)->f2; goto
_LL2715; _LL2715: _temp2714=(( struct Cyc_Absyn_TypedefType_struct*) _temp2579)->f3;
_temp2716=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2579)->f3; goto _LL2616;} else{ goto _LL2617;} _LL2617: if( _temp2579 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2618;} else{ goto _LL2619;} _LL2619: if((
unsigned int) _temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2722: _temp2721=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2579)->f1; goto _LL2620;} else{ goto _LL2621;} _LL2621: if(( unsigned int)
_temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_AccessEff: 0){ _LL2724:
_temp2723=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2579)->f1; goto
_LL2622;} else{ goto _LL2623;} _LL2623: if(( unsigned int) _temp2579 >  4u?*((
int*) _temp2579) ==  Cyc_Absyn_RgnsEff: 0){ _LL2726: _temp2725=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2579)->f1; goto _LL2624;} else{ goto _LL2625;}
_LL2625: if(( unsigned int) _temp2579 >  4u?*(( int*) _temp2579) ==  Cyc_Absyn_JoinEff:
0){ _LL2728: _temp2727=(( struct Cyc_Absyn_JoinEff_struct*) _temp2579)->f1; goto
_LL2626;} else{ goto _LL2580;} _LL2582: goto _LL2580; _LL2584: if(* _temp2632 == 
0){* _temp2632=({ struct Cyc_Core_Opt* _temp2729=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2729->v=( void*) expected_kind;
_temp2729;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2629=({ struct Cyc_Core_Opt* _temp2730=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2730->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2730;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2731= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2629=({ struct Cyc_Core_Opt* _temp2732=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2732->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2733=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2733[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2734; _temp2734.tag= Cyc_Absyn_VarType;
_temp2734.f1= _temp2731; _temp2734;}); _temp2733;})); _temp2732;}); _temp2633=
_temp2731; goto _LL2586;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2580; _LL2586: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2633->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2735=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2735[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2736; _temp2736.tag= Cyc_Absyn_Eq_constr;
_temp2736.f1=( void*) expected_kind; _temp2736;}); _temp2735;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2633); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2633); goto _LL2580;}
_LL2588: if(* _temp2637 ==  0){ struct _handler_cons _temp2737; _push_handler(&
_temp2737);{ int _temp2739= 0; if( setjmp( _temp2737.handler)){ _temp2739= 1;}
if( ! _temp2739){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2638);* _temp2637=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2738=( void*) _exn_thrown; void* _temp2741= _temp2738; _LL2743:
if( _temp2741 ==  Cyc_Dict_Absent){ goto _LL2744;} else{ goto _LL2745;} _LL2745:
goto _LL2746; _LL2744:({ struct Cyc_Std_String_pa_struct _temp2748; _temp2748.tag=
Cyc_Std_String_pa; _temp2748.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2638);{ void* _temp2747[ 1u]={& _temp2748}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u), _tag_arr( _temp2747, sizeof( void*), 1u));}});
return cvtenv; _LL2746:( void) _throw( _temp2741); _LL2742:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2637);* _temp2638=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2580;} _LL2590:{ void*
_temp2749=* _temp2648; struct Cyc_Absyn_UnknownTunionInfo _temp2755; int
_temp2757; struct _tuple1* _temp2759; struct Cyc_Absyn_Tuniondecl* _temp2761;
_LL2751: if(*(( int*) _temp2749) ==  Cyc_Absyn_UnknownTunion){ _LL2756:
_temp2755=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2749)->f1; _LL2760:
_temp2759= _temp2755.name; goto _LL2758; _LL2758: _temp2757= _temp2755.is_xtunion;
goto _LL2752;} else{ goto _LL2753;} _LL2753: if(*(( int*) _temp2749) ==  Cyc_Absyn_KnownTunion){
_LL2762: _temp2761=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2749)->f1; goto
_LL2754;} else{ goto _LL2750;} _LL2752: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2763; _push_handler(& _temp2763);{ int _temp2765= 0;
if( setjmp( _temp2763.handler)){ _temp2765= 1;} if( ! _temp2765){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2759);; _pop_handler();} else{ void* _temp2764=( void*)
_exn_thrown; void* _temp2767= _temp2764; _LL2769: if( _temp2767 ==  Cyc_Dict_Absent){
goto _LL2770;} else{ goto _LL2771;} _LL2771: goto _LL2772; _LL2770:({ struct Cyc_Std_String_pa_struct
_temp2774; _temp2774.tag= Cyc_Std_String_pa; _temp2774.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2759);{ void* _temp2773[ 1u]={& _temp2774}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type [x]tunion %s", sizeof( unsigned char), 26u),
_tag_arr( _temp2773, sizeof( void*), 1u));}}); return cvtenv; _LL2772:( void)
_throw( _temp2767); _LL2768:;}}} if( tud->is_xtunion !=  _temp2757){({ struct
Cyc_Std_String_pa_struct _temp2776; _temp2776.tag= Cyc_Std_String_pa; _temp2776.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2759);{ void* _temp2775[ 1u]={&
_temp2776}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2775, sizeof( void*), 1u));}});}*
_temp2648=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2777=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2777[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2778; _temp2778.tag= Cyc_Absyn_KnownTunion;
_temp2778.f1= tud; _temp2778;}); _temp2777;}); _temp2761= tud; goto _LL2754;}
_LL2754: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2642);{ struct Cyc_List_List* tvs= _temp2761->tvs; for( 0; _temp2644 !=  0?
tvs !=  0: 0;( _temp2644=(( struct Cyc_List_List*) _check_null( _temp2644))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2644))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2644
!=  0){({ struct Cyc_Std_String_pa_struct _temp2780; _temp2780.tag= Cyc_Std_String_pa;
_temp2780.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2761->name);{
void* _temp2779[ 1u]={& _temp2780}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2779, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2782; _temp2782.tag= Cyc_Std_String_pa;
_temp2782.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2761->name);{
void* _temp2781[ 1u]={& _temp2782}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2781, sizeof( void*), 1u));}});}
goto _LL2750;} _LL2750:;} goto _LL2580; _LL2592:{ void* _temp2783=* _temp2655;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2789; int _temp2791; struct _tuple1*
_temp2793; struct _tuple1* _temp2795; struct Cyc_Absyn_Tunionfield* _temp2797;
struct Cyc_Absyn_Tuniondecl* _temp2799; _LL2785: if(*(( int*) _temp2783) ==  Cyc_Absyn_UnknownTunionfield){
_LL2790: _temp2789=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2783)->f1;
_LL2796: _temp2795= _temp2789.tunion_name; goto _LL2794; _LL2794: _temp2793=
_temp2789.field_name; goto _LL2792; _LL2792: _temp2791= _temp2789.is_xtunion;
goto _LL2786;} else{ goto _LL2787;} _LL2787: if(*(( int*) _temp2783) ==  Cyc_Absyn_KnownTunionfield){
_LL2800: _temp2799=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2783)->f1;
goto _LL2798; _LL2798: _temp2797=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2783)->f2; goto _LL2788;} else{ goto _LL2784;} _LL2786: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2801;
_push_handler(& _temp2801);{ int _temp2803= 0; if( setjmp( _temp2801.handler)){
_temp2803= 1;} if( ! _temp2803){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2795);; _pop_handler();} else{ void* _temp2802=( void*) _exn_thrown; void*
_temp2805= _temp2802; _LL2807: if( _temp2805 ==  Cyc_Dict_Absent){ goto _LL2808;}
else{ goto _LL2809;} _LL2809: goto _LL2810; _LL2808:({ struct Cyc_Std_String_pa_struct
_temp2812; _temp2812.tag= Cyc_Std_String_pa; _temp2812.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2795);{ void* _temp2811[ 1u]={& _temp2812}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2811, sizeof( void*), 1u));}}); return cvtenv; _LL2810:( void) _throw(
_temp2805); _LL2806:;}}}{ struct _handler_cons _temp2813; _push_handler(&
_temp2813);{ int _temp2815= 0; if( setjmp( _temp2813.handler)){ _temp2815= 1;}
if( ! _temp2815){{ void* _temp2816= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2793); struct Cyc_Absyn_Tunionfield* _temp2822; struct Cyc_Absyn_Tuniondecl*
_temp2824; _LL2818: if(*(( int*) _temp2816) ==  Cyc_Tcenv_TunionRes){ _LL2825:
_temp2824=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2816)->f1; goto _LL2823;
_LL2823: _temp2822=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2816)->f2; goto
_LL2819;} else{ goto _LL2820;} _LL2820: goto _LL2821; _LL2819: tuf= _temp2822;
tud= _temp2824; if( tud->is_xtunion !=  _temp2791){({ struct Cyc_Std_String_pa_struct
_temp2827; _temp2827.tag= Cyc_Std_String_pa; _temp2827.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2795);{ void* _temp2826[ 1u]={& _temp2827}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2826, sizeof( void*), 1u));}});} goto
_LL2817; _LL2821:({ struct Cyc_Std_String_pa_struct _temp2830; _temp2830.tag=
Cyc_Std_String_pa; _temp2830.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2795);{ struct Cyc_Std_String_pa_struct _temp2829; _temp2829.tag= Cyc_Std_String_pa;
_temp2829.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2793);{ void*
_temp2828[ 2u]={& _temp2829,& _temp2830}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2828, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2831= cvtenv; _npop_handler( 0u); return _temp2831;}
_LL2817:;}; _pop_handler();} else{ void* _temp2814=( void*) _exn_thrown; void*
_temp2833= _temp2814; _LL2835: if( _temp2833 ==  Cyc_Dict_Absent){ goto _LL2836;}
else{ goto _LL2837;} _LL2837: goto _LL2838; _LL2836:({ struct Cyc_Std_String_pa_struct
_temp2841; _temp2841.tag= Cyc_Std_String_pa; _temp2841.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2795);{ struct Cyc_Std_String_pa_struct _temp2840;
_temp2840.tag= Cyc_Std_String_pa; _temp2840.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2793);{ void* _temp2839[ 2u]={& _temp2840,& _temp2841}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2839, sizeof( void*), 2u));}}}); return cvtenv; _LL2838:( void)
_throw( _temp2833); _LL2834:;}}}* _temp2655=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2842=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2842[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2843; _temp2843.tag= Cyc_Absyn_KnownTunionfield; _temp2843.f1= tud;
_temp2843.f2= tuf; _temp2843;}); _temp2842;}); _temp2799= tud; _temp2797= tuf;
goto _LL2788;} _LL2788: { struct Cyc_List_List* tvs= _temp2799->tvs; for( 0;
_temp2651 !=  0? tvs !=  0: 0;( _temp2651=(( struct Cyc_List_List*) _check_null(
_temp2651))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2651))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2651
!=  0){({ struct Cyc_Std_String_pa_struct _temp2846; _temp2846.tag= Cyc_Std_String_pa;
_temp2846.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2797->name);{
struct Cyc_Std_String_pa_struct _temp2845; _temp2845.tag= Cyc_Std_String_pa;
_temp2845.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2799->name);{
void* _temp2844[ 2u]={& _temp2845,& _temp2846}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2844, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2849; _temp2849.tag= Cyc_Std_String_pa;
_temp2849.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2797->name);{
struct Cyc_Std_String_pa_struct _temp2848; _temp2848.tag= Cyc_Std_String_pa;
_temp2848.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2799->name);{
void* _temp2847[ 2u]={& _temp2848,& _temp2849}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2847, sizeof( void*), 2u));}}});}
goto _LL2784;} _LL2784:;} goto _LL2580; _LL2594: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2666); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2664);{ void* _temp2850=( void*)(
Cyc_Absyn_compress_conref( _temp2658))->v; void* _temp2856; struct Cyc_Absyn_Exp*
_temp2858; _LL2852: if(( unsigned int) _temp2850 >  1u?*(( int*) _temp2850) == 
Cyc_Absyn_Eq_constr: 0){ _LL2857: _temp2856=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2850)->f1; if(( unsigned int) _temp2856 >  1u?*(( int*) _temp2856) ==  Cyc_Absyn_Upper_b:
0){ _LL2859: _temp2858=(( struct Cyc_Absyn_Upper_b_struct*) _temp2856)->f1; goto
_LL2853;} else{ goto _LL2854;}} else{ goto _LL2854;} _LL2854: goto _LL2855;
_LL2853: if( ! Cyc_Tcutil_is_const_exp( te, _temp2858)){({ void* _temp2860[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2860, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2858); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2858)){({ void*
_temp2861[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2861, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2858); goto _LL2851; _LL2855: goto _LL2851; _LL2851:;} goto _LL2580;
_LL2596: goto _LL2580; _LL2598: goto _LL2580; _LL2600: goto _LL2580; _LL2602:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2672); if( _temp2668 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2668))){({ void* _temp2862[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2862, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2668)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2668))){({ void* _temp2863[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2863, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2668)); goto _LL2580; _LL2604: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2676 !=  0; _temp2676=(( struct Cyc_List_List*)
_check_null( _temp2676))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2676))->hd)){({ struct Cyc_Std_String_pa_struct _temp2865;
_temp2865.tag= Cyc_Std_String_pa; _temp2865.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2676))->hd);{ void* _temp2864[
1u]={& _temp2865}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2864, sizeof( void*), 1u));}});}{
void* _temp2866=( void*)(( struct Cyc_List_List*) _check_null( _temp2676))->hd;
int _temp2878; int _temp2880; void* _temp2882; _LL2868: if( _temp2866 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2869;} else{ goto _LL2870;} _LL2870: if(
_temp2866 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL2871;} else{ goto _LL2872;}
_LL2872: if( _temp2866 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL2873;} else{
goto _LL2874;} _LL2874: if(( unsigned int) _temp2866 >  16u?*(( int*) _temp2866)
==  Cyc_Absyn_Format_att: 0){ _LL2883: _temp2882=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2866)->f1; goto _LL2881; _LL2881: _temp2880=(( struct Cyc_Absyn_Format_att_struct*)
_temp2866)->f2; goto _LL2879; _LL2879: _temp2878=(( struct Cyc_Absyn_Format_att_struct*)
_temp2866)->f3; goto _LL2875;} else{ goto _LL2876;} _LL2876: goto _LL2877;
_LL2869: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2867;
_LL2871: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2867; _LL2873:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2867; _LL2875:
if( ! seen_format){ seen_format= 1; ft= _temp2882; fmt_desc_arg= _temp2880;
fmt_arg_start= _temp2878;} else{({ void* _temp2884[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2884, sizeof( void*), 0u));});} goto _LL2867; _LL2877: goto
_LL2867; _LL2867:;}} if( num_convs >  1){({ void* _temp2885[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2885, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2693);{ struct Cyc_List_List* b=* _temp2693; for( 0; b !=  0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2886=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2892; _LL2888: if(( unsigned int) _temp2886 >  1u?*(( int*) _temp2886)
==  Cyc_Absyn_Eq_constr: 0){ _LL2893: _temp2892=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2886)->f1; if( _temp2892 == ( void*) Cyc_Absyn_MemKind){ goto _LL2889;}
else{ goto _LL2890;}} else{ goto _LL2890;} _LL2890: goto _LL2891; _LL2889:({
struct Cyc_Std_String_pa_struct _temp2895; _temp2895.tag= Cyc_Std_String_pa;
_temp2895.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2894[ 1u]={& _temp2895}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2894, sizeof( void*), 1u));}}); goto
_LL2887; _LL2891: goto _LL2887; _LL2887:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2896=({
struct Cyc_Tcutil_CVTEnv _temp3053; _temp3053.kind_env= cvtenv.kind_env;
_temp3053.free_vars= 0; _temp3053.free_evars= 0; _temp3053.generalize_evars=
cvtenv.generalize_evars; _temp3053.fn_result= 1; _temp3053;}); _temp2896= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2896,( void*) Cyc_Absyn_MemKind, _temp2686); _temp2896.fn_result=
0;{ struct Cyc_List_List* a= _temp2684; for( 0; a !=  0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ _temp2896= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2896,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( a))->hd)).f3);}} if( _temp2680 !=  0){ if( _temp2682){({ void*
_temp2897[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2897, sizeof( void*), 0u));});}{ int _temp2900; void* _temp2902; struct Cyc_Absyn_Tqual
_temp2904; struct Cyc_Core_Opt* _temp2906; struct Cyc_Absyn_VarargInfo _temp2898=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp2680)); _LL2907: _temp2906=
_temp2898.name; goto _LL2905; _LL2905: _temp2904= _temp2898.tq; goto _LL2903;
_LL2903: _temp2902=( void*) _temp2898.type; goto _LL2901; _LL2901: _temp2900=
_temp2898.inject; goto _LL2899; _LL2899: _temp2896= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2896,( void*) Cyc_Absyn_MemKind, _temp2902); if( _temp2900){ void*
_temp2908= Cyc_Tcutil_compress( _temp2902); struct Cyc_Absyn_TunionInfo
_temp2914; void* _temp2916; void* _temp2918; _LL2910: if(( unsigned int)
_temp2908 >  4u?*(( int*) _temp2908) ==  Cyc_Absyn_TunionType: 0){ _LL2915:
_temp2914=(( struct Cyc_Absyn_TunionType_struct*) _temp2908)->f1; _LL2919:
_temp2918=( void*) _temp2914.tunion_info; if(*(( int*) _temp2918) ==  Cyc_Absyn_KnownTunion){
goto _LL2917;} else{ goto _LL2912;} _LL2917: _temp2916=( void*) _temp2914.rgn;
goto _LL2911;} else{ goto _LL2912;} _LL2912: goto _LL2913; _LL2911: goto _LL2909;
_LL2913:({ void* _temp2920[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u), _tag_arr( _temp2920, sizeof( void*), 0u));}); goto
_LL2909; _LL2909:;}}} if( seen_format){ int _temp2921=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2684); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg > 
_temp2921)? 1: fmt_arg_start <  0)? 1:( _temp2680 ==  0? fmt_arg_start !=  0: 0))?
1:( _temp2680 !=  0? fmt_arg_start !=  _temp2921 +  1: 0)){({ void* _temp2922[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2922, sizeof( void*), 0u));});} else{ void* _temp2925;
struct _tuple2 _temp2923=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp2684, fmt_desc_arg -  1); _LL2926: _temp2925= _temp2923.f3;
goto _LL2924; _LL2924:{ void* _temp2927= Cyc_Tcutil_compress( _temp2925); struct
Cyc_Absyn_PtrInfo _temp2933; struct Cyc_Absyn_Conref* _temp2935; void* _temp2937;
_LL2929: if(( unsigned int) _temp2927 >  4u?*(( int*) _temp2927) ==  Cyc_Absyn_PointerType:
0){ _LL2934: _temp2933=(( struct Cyc_Absyn_PointerType_struct*) _temp2927)->f1;
_LL2938: _temp2937=( void*) _temp2933.elt_typ; goto _LL2936; _LL2936: _temp2935=
_temp2933.bounds; goto _LL2930;} else{ goto _LL2931;} _LL2931: goto _LL2932;
_LL2930:{ struct _tuple8 _temp2940=({ struct _tuple8 _temp2939; _temp2939.f1=
Cyc_Tcutil_compress( _temp2937); _temp2939.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2935); _temp2939;}); void* _temp2946; void* _temp2948; void* _temp2950;
void* _temp2952; _LL2942: _LL2949: _temp2948= _temp2940.f1; if(( unsigned int)
_temp2948 >  4u?*(( int*) _temp2948) ==  Cyc_Absyn_IntType: 0){ _LL2953:
_temp2952=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2948)->f1; if(
_temp2952 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2951;} else{ goto _LL2944;}
_LL2951: _temp2950=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2948)->f2;
if( _temp2950 == ( void*) Cyc_Absyn_B1){ goto _LL2947;} else{ goto _LL2944;}}
else{ goto _LL2944;} _LL2947: _temp2946= _temp2940.f2; if( _temp2946 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2943;} else{ goto _LL2944;} _LL2944: goto _LL2945;
_LL2943: goto _LL2941; _LL2945:({ void* _temp2954[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp2954, sizeof( void*), 0u));}); goto _LL2941; _LL2941:;} goto
_LL2928; _LL2932:({ void* _temp2955[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2955, sizeof( void*), 0u));}); goto
_LL2928; _LL2928:;} if( fmt_arg_start !=  0){ void* _temp2956= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2680))->type); int
problem;{ void* _temp2957= ft; _LL2959: if( _temp2957 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL2960;} else{ goto _LL2961;} _LL2961: if( _temp2957 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL2962;} else{ goto _LL2958;} _LL2960:{ void* _temp2963= _temp2956; struct
Cyc_Absyn_TunionInfo _temp2969; void* _temp2971; struct Cyc_Absyn_Tuniondecl*
_temp2973; _LL2965: if(( unsigned int) _temp2963 >  4u?*(( int*) _temp2963) == 
Cyc_Absyn_TunionType: 0){ _LL2970: _temp2969=(( struct Cyc_Absyn_TunionType_struct*)
_temp2963)->f1; _LL2972: _temp2971=( void*) _temp2969.tunion_info; if(*(( int*)
_temp2971) ==  Cyc_Absyn_KnownTunion){ _LL2974: _temp2973=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2971)->f1; goto _LL2966;} else{ goto _LL2967;}} else{ goto _LL2967;}
_LL2967: goto _LL2968; _LL2966: problem= Cyc_Absyn_qvar_cmp( _temp2973->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL2964; _LL2968: problem= 1; goto
_LL2964; _LL2964:;} goto _LL2958; _LL2962:{ void* _temp2975= _temp2956; struct
Cyc_Absyn_TunionInfo _temp2981; void* _temp2983; struct Cyc_Absyn_Tuniondecl*
_temp2985; _LL2977: if(( unsigned int) _temp2975 >  4u?*(( int*) _temp2975) == 
Cyc_Absyn_TunionType: 0){ _LL2982: _temp2981=(( struct Cyc_Absyn_TunionType_struct*)
_temp2975)->f1; _LL2984: _temp2983=( void*) _temp2981.tunion_info; if(*(( int*)
_temp2983) ==  Cyc_Absyn_KnownTunion){ _LL2986: _temp2985=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2983)->f1; goto _LL2978;} else{ goto _LL2979;}} else{ goto _LL2979;}
_LL2979: goto _LL2980; _LL2978: problem= Cyc_Absyn_qvar_cmp( _temp2985->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL2976; _LL2980: problem= 1; goto
_LL2976; _LL2976:;} goto _LL2958; _LL2958:;} if( problem){({ void* _temp2987[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp2987, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2678; for( 0; rpo !=  0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp2990; void* _temp2991; void*
_temp2993; struct _tuple8* _temp2988=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp2990=* _temp2988; _LL2994: _temp2993= _temp2990.f1;
goto _LL2992; _LL2992: _temp2991= _temp2990.f2; goto _LL2989; _LL2989: _temp2896=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2896,( void*) Cyc_Absyn_RgnKind,
_temp2993); _temp2896= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2896,( void*)
Cyc_Absyn_RgnKind, _temp2991);}} if(* _temp2690 !=  0){ _temp2896= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2896,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2690))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2896.free_vars; for(
0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void*
_temp2995=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3003; void*
_temp3005; _LL2997: if(( unsigned int) _temp2995 >  1u?*(( int*) _temp2995) == 
Cyc_Absyn_Eq_constr: 0){ _LL3004: _temp3003=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2995)->f1; if( _temp3003 == ( void*) Cyc_Absyn_RgnKind){ goto _LL2998;}
else{ goto _LL2999;}} else{ goto _LL2999;} _LL2999: if(( unsigned int) _temp2995
>  1u?*(( int*) _temp2995) ==  Cyc_Absyn_Eq_constr: 0){ _LL3006: _temp3005=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2995)->f1; if( _temp3005 == (
void*) Cyc_Absyn_EffKind){ goto _LL3000;} else{ goto _LL3001;}} else{ goto
_LL3001;} _LL3001: goto _LL3002; _LL2998: effect=({ struct Cyc_List_List*
_temp3007=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3007->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3008=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3008[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3009; _temp3009.tag= Cyc_Absyn_AccessEff;
_temp3009.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3010=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3010[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3011; _temp3011.tag= Cyc_Absyn_VarType;
_temp3011.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3011;}); _temp3010;})); _temp3009;}); _temp3008;})); _temp3007->tl= effect;
_temp3007;}); goto _LL2996; _LL3000: effect=({ struct Cyc_List_List* _temp3012=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3012->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3013=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3013[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3014; _temp3014.tag= Cyc_Absyn_VarType; _temp3014.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3014;}); _temp3013;})); _temp3012->tl= effect; _temp3012;}); goto _LL2996;
_LL3002: effect=({ struct Cyc_List_List* _temp3015=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3015->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3016=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3016[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3017; _temp3017.tag= Cyc_Absyn_RgnsEff; _temp3017.f1=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3018=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3018[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3019; _temp3019.tag= Cyc_Absyn_VarType; _temp3019.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3019;}); _temp3018;})); _temp3017;}); _temp3016;})); _temp3015->tl= effect;
_temp3015;}); goto _LL2996; _LL2996:;}} effect= Cyc_List_imp_rev( effect);{
struct Cyc_List_List* ts= _temp2896.free_evars; for( 0; ts !=  0; ts=(( struct
Cyc_List_List*) _check_null( ts))->tl){ void* _temp3020= Cyc_Tcutil_typ_kind((
void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _LL3022: if( _temp3020
== ( void*) Cyc_Absyn_RgnKind){ goto _LL3023;} else{ goto _LL3024;} _LL3024: if(
_temp3020 == ( void*) Cyc_Absyn_EffKind){ goto _LL3025;} else{ goto _LL3026;}
_LL3026: goto _LL3027; _LL3023: effect=({ struct Cyc_List_List* _temp3028=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3028->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3029=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3029[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3030; _temp3030.tag= Cyc_Absyn_AccessEff;
_temp3030.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3030;}); _temp3029;})); _temp3028->tl= effect; _temp3028;}); goto _LL3021;
_LL3025: effect=({ struct Cyc_List_List* _temp3031=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3031->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3031->tl= effect; _temp3031;});
goto _LL3021; _LL3027: effect=({ struct Cyc_List_List* _temp3032=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3032->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3033=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3033[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3034; _temp3034.tag= Cyc_Absyn_RgnsEff; _temp3034.f1=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _temp3034;});
_temp3033;})); _temp3032->tl= effect; _temp3032;}); goto _LL3021; _LL3021:;}}*
_temp2690=({ struct Cyc_Core_Opt* _temp3035=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3035->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3036=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3036[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3037; _temp3037.tag= Cyc_Absyn_JoinEff;
_temp3037.f1= Cyc_List_imp_rev( effect); _temp3037;}); _temp3036;})); _temp3035;});}}
if(* _temp2693 !=  0){ struct Cyc_List_List* bs=* _temp2693; for( 0; bs !=  0;
bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3038= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3039=( void*) _temp3038->v; void*
_temp3047; _LL3041: if( _temp3039 == ( void*) Cyc_Absyn_No_constr){ goto _LL3042;}
else{ goto _LL3043;} _LL3043: if(( unsigned int) _temp3039 >  1u?*(( int*)
_temp3039) ==  Cyc_Absyn_Eq_constr: 0){ _LL3048: _temp3047=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3039)->f1; if( _temp3047 == ( void*) Cyc_Absyn_MemKind){ goto _LL3044;}
else{ goto _LL3045;}} else{ goto _LL3045;} _LL3045: goto _LL3046; _LL3042:({
struct Cyc_Std_String_pa_struct _temp3050; _temp3050.tag= Cyc_Std_String_pa;
_temp3050.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp3049[ 1u]={& _temp3050}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3049, sizeof( void*), 1u));}}); goto
_LL3044; _LL3044:( void*)( _temp3038->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3051=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3051[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3052; _temp3052.tag= Cyc_Absyn_Eq_constr;
_temp3052.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3052;}); _temp3051;})));
goto _LL3040; _LL3046: goto _LL3040; _LL3040:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2896.kind_env,* _temp2693); _temp2896.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2896.free_vars,* _temp2693);{ struct Cyc_List_List* tvs= _temp2896.free_vars;
for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2896.free_evars; for( 0; evs !=  0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2580;}}
_LL2606: for( 0; _temp2694 !=  0; _temp2694=(( struct Cyc_List_List*)
_check_null( _temp2694))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp2694))->hd)).f2);} goto _LL2580; _LL2608:{ struct
_RegionHandle _temp3054= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp3054; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2696 !=  0; _temp2696=(( struct Cyc_List_List*) _check_null( _temp2696))->tl){
struct Cyc_Absyn_Structfield _temp3057; struct Cyc_List_List* _temp3058; struct
Cyc_Absyn_Exp* _temp3060; void* _temp3062; struct Cyc_Absyn_Tqual _temp3064;
struct _tagged_arr* _temp3066; struct Cyc_Absyn_Structfield* _temp3055=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2696))->hd;
_temp3057=* _temp3055; _LL3067: _temp3066= _temp3057.name; goto _LL3065; _LL3065:
_temp3064= _temp3057.tq; goto _LL3063; _LL3063: _temp3062=( void*) _temp3057.type;
goto _LL3061; _LL3061: _temp3060= _temp3057.width; goto _LL3059; _LL3059:
_temp3058= _temp3057.attributes; goto _LL3056; _LL3056: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3066)){({ struct Cyc_Std_String_pa_struct _temp3069; _temp3069.tag= Cyc_Std_String_pa;
_temp3069.f1=( struct _tagged_arr)* _temp3066;{ void* _temp3068[ 1u]={&
_temp3069}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3068, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3066, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3070=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3070->hd=( void*)
_temp3066; _temp3070->tl= prev_fields; _temp3070;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3062); Cyc_Tcutil_check_bitfield(
loc, te, _temp3062, _temp3060, _temp3066); Cyc_Tcutil_check_field_atts( loc,
_temp3066, _temp3058);}}; _pop_region( sprev_rgn);} goto _LL2580; _LL2610:{
struct _RegionHandle _temp3071= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3071; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2698 !=  0; _temp2698=(( struct Cyc_List_List*) _check_null( _temp2698))->tl){
struct Cyc_Absyn_Structfield _temp3074; struct Cyc_List_List* _temp3075; struct
Cyc_Absyn_Exp* _temp3077; void* _temp3079; struct Cyc_Absyn_Tqual _temp3081;
struct _tagged_arr* _temp3083; struct Cyc_Absyn_Structfield* _temp3072=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2698))->hd;
_temp3074=* _temp3072; _LL3084: _temp3083= _temp3074.name; goto _LL3082; _LL3082:
_temp3081= _temp3074.tq; goto _LL3080; _LL3080: _temp3079=( void*) _temp3074.type;
goto _LL3078; _LL3078: _temp3077= _temp3074.width; goto _LL3076; _LL3076:
_temp3075= _temp3074.attributes; goto _LL3073; _LL3073: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3083)){({ struct Cyc_Std_String_pa_struct _temp3086; _temp3086.tag= Cyc_Std_String_pa;
_temp3086.f1=( struct _tagged_arr)* _temp3083;{ void* _temp3085[ 1u]={&
_temp3086}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3085, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3083, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3087=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3087->hd=( void*)
_temp3083; _temp3087->tl= prev_fields; _temp3087;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3079); if( ! Cyc_Tcutil_bits_only(
_temp3079)){({ struct Cyc_Std_String_pa_struct _temp3089; _temp3089.tag= Cyc_Std_String_pa;
_temp3089.f1=( struct _tagged_arr)* _temp3083;{ void* _temp3088[ 1u]={&
_temp3089}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3088, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3079, _temp3077, _temp3083); Cyc_Tcutil_check_field_atts(
loc, _temp3083, _temp3075);}}; _pop_region( uprev_rgn);} goto _LL2580; _LL2612:
if( _temp2705 ==  0){ if(* _temp2702 ==  0){({ void* _temp3090[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3090, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2702));;}} if(*
_temp2702 ==  0){ struct _handler_cons _temp3091; _push_handler(& _temp3091);{
int _temp3093= 0; if( setjmp( _temp3091.handler)){ _temp3093= 1;} if( !
_temp3093){* _temp2702=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2705));; _pop_handler();} else{
void* _temp3092=( void*) _exn_thrown; void* _temp3095= _temp3092; _LL3097: if(
_temp3095 ==  Cyc_Dict_Absent){ goto _LL3098;} else{ goto _LL3099;} _LL3099:
goto _LL3100; _LL3098:({ struct Cyc_Std_String_pa_struct _temp3102; _temp3102.tag=
Cyc_Std_String_pa; _temp3102.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2705));{ void* _temp3101[ 1u]={& _temp3102};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type struct %s", sizeof( unsigned char),
23u), _tag_arr( _temp3101, sizeof( void*), 1u));}}); return cvtenv; _LL3100:(
void) _throw( _temp3095); _LL3096:;}}}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2702)); if( sd->name !=  0){*((
struct _tuple1*) _check_null( _temp2705))=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2703); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3106; _temp3106.tag= Cyc_Std_Int_pa; _temp3106.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3105; _temp3105.tag= Cyc_Std_Int_pa;
_temp3105.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3104; _temp3104.tag= Cyc_Std_String_pa; _temp3104.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2705));{ void*
_temp3103[ 3u]={& _temp3104,& _temp3105,& _temp3106}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3103, sizeof( void*), 3u));}}}});} for( 0;
_temp2703 !=  0;( _temp2703=(( struct Cyc_List_List*) _check_null( _temp2703))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2703))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2580;}}
_LL2614: if( _temp2712 ==  0){({ void* _temp3107[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp3107,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2709 ==  0){ struct
_handler_cons _temp3108; _push_handler(& _temp3108);{ int _temp3110= 0; if(
setjmp( _temp3108.handler)){ _temp3110= 1;} if( ! _temp3110){* _temp2709=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2712));; _pop_handler();} else{ void* _temp3109=(
void*) _exn_thrown; void* _temp3112= _temp3109; _LL3114: if( _temp3112 ==  Cyc_Dict_Absent){
goto _LL3115;} else{ goto _LL3116;} _LL3116: goto _LL3117; _LL3115:({ struct Cyc_Std_String_pa_struct
_temp3119; _temp3119.tag= Cyc_Std_String_pa; _temp3119.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2712));{ void*
_temp3118[ 1u]={& _temp3119}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type union %s",
sizeof( unsigned char), 22u), _tag_arr( _temp3118, sizeof( void*), 1u));}});
return cvtenv; _LL3117:( void) _throw( _temp3112); _LL3113:;}}}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(* _temp2709)); if( ud->name != 
0){*(( struct _tuple1*) _check_null( _temp2712))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs= Cyc_List_length( _temp2710); if( lvs != 
largs){({ struct Cyc_Std_Int_pa_struct _temp3123; _temp3123.tag= Cyc_Std_Int_pa;
_temp3123.f1=( int)(( unsigned int) largs);{ struct Cyc_Std_Int_pa_struct
_temp3122; _temp3122.tag= Cyc_Std_Int_pa; _temp3122.f1=( int)(( unsigned int)
lvs);{ struct Cyc_Std_String_pa_struct _temp3121; _temp3121.tag= Cyc_Std_String_pa;
_temp3121.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2712));{ void* _temp3120[ 3u]={& _temp3121,& _temp3122,&
_temp3123}; Cyc_Tcutil_terr( loc, _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u), _tag_arr( _temp3120, sizeof( void*), 3u));}}}});}
for( 0; _temp2710 !=  0;( _temp2710=(( struct Cyc_List_List*) _check_null(
_temp2710))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2710))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2580;}}
_LL2616: { struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons _temp3124;
_push_handler(& _temp3124);{ int _temp3126= 0; if( setjmp( _temp3124.handler)){
_temp3126= 1;} if( ! _temp3126){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp2719);; _pop_handler();} else{ void* _temp3125=( void*) _exn_thrown; void*
_temp3128= _temp3125; _LL3130: if( _temp3128 ==  Cyc_Dict_Absent){ goto _LL3131;}
else{ goto _LL3132;} _LL3132: goto _LL3133; _LL3131:({ struct Cyc_Std_String_pa_struct
_temp3135; _temp3135.tag= Cyc_Std_String_pa; _temp3135.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2719);{ void* _temp3134[ 1u]={& _temp3135}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3134, sizeof( void*), 1u));}}); return cvtenv; _LL3133:( void) _throw(
_temp3128); _LL3129:;}}} _temp2719[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2717; struct Cyc_List_List* inst= 0; for( 0; ts
!=  0? tvs !=  0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3136=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3136->hd=( void*)({ struct
_tuple5* _temp3137=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3137->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3137->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3137;}); _temp3136->tl= inst; _temp3136;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp3139; _temp3139.tag= Cyc_Std_String_pa; _temp3139.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2719);{ void* _temp3138[ 1u]={& _temp3139}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3138, sizeof( void*), 1u));}});} if( tvs !=  0){ for( 0; tvs != 
0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
void* e= Cyc_Absyn_new_evar( 0, 0); cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv, k, e); inst=({ struct Cyc_List_List* _temp3140=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3140->hd=( void*)({ struct
_tuple5* _temp3141=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3141->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3141->f2= e; _temp3141;}); _temp3140->tl= inst; _temp3140;});}}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2716=({
struct Cyc_Core_Opt* _temp3142=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3142->v=( void*) new_typ; _temp3142;}); goto _LL2580;}}}
_LL2618: goto _LL2580; _LL2620: _temp2723= _temp2721; goto _LL2622; _LL2622:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2723); goto _LL2580; _LL2624: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2725); goto _LL2580; _LL2626: for( 0;
_temp2727 !=  0; _temp2727=(( struct Cyc_List_List*) _check_null( _temp2727))->tl){
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_List_List*) _check_null( _temp2727))->hd);} goto _LL2580;
_LL2580:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){({
struct Cyc_Std_String_pa_struct _temp3146; _temp3146.tag= Cyc_Std_String_pa;
_temp3146.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_Std_String_pa_struct _temp3145; _temp3145.tag= Cyc_Std_String_pa;
_temp3145.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_Std_String_pa_struct _temp3144; _temp3144.tag= Cyc_Std_String_pa;
_temp3144.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3143[
3u]={& _temp3144,& _temp3145,& _temp3146}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3143, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3147= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3162; _temp3162.kind_env= kind_env; _temp3162.free_vars= 0; _temp3162.free_evars=
0; _temp3162.generalize_evars= generalize_evars; _temp3162.fn_result= 0;
_temp3162;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3147.free_vars;
for( 0; vs !=  0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ _temp3147.kind_env=
Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( vs))->hd);}}{ struct Cyc_List_List* evs= _temp3147.free_evars; for(
0; evs !=  0; evs=(( struct Cyc_List_List*) _check_null( evs))->tl){ void*
_temp3148= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null(
evs))->hd); struct Cyc_Core_Opt* _temp3154; struct Cyc_Core_Opt** _temp3156;
_LL3150: if(( unsigned int) _temp3148 >  4u?*(( int*) _temp3148) ==  Cyc_Absyn_Evar:
0){ _LL3155: _temp3154=(( struct Cyc_Absyn_Evar_struct*) _temp3148)->f4;
_temp3156=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3148)->f4;
goto _LL3151;} else{ goto _LL3152;} _LL3152: goto _LL3153; _LL3151: if(*
_temp3156 ==  0){* _temp3156=({ struct Cyc_Core_Opt* _temp3157=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3157->v=( void*) kind_env;
_temp3157;});} else{ struct Cyc_List_List* _temp3158=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3156))->v; struct Cyc_List_List*
_temp3159= 0; for( 0; _temp3158 !=  0; _temp3158=(( struct Cyc_List_List*)
_check_null( _temp3158))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3158))->hd)){ _temp3159=({ struct Cyc_List_List* _temp3160=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3160->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3158))->hd);
_temp3160->tl= _temp3159; _temp3160;});}}* _temp3156=({ struct Cyc_Core_Opt*
_temp3161=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3161->v=( void*) _temp3159; _temp3161;});} goto _LL3149; _LL3153: goto
_LL3149; _LL3149:;}} return _temp3147;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ int
generalize_evars= Cyc_Tcutil_is_function_type( t); struct Cyc_Tcutil_CVTEnv
_temp3163= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
generalize_evars, t); struct Cyc_List_List* _temp3164= _temp3163.free_vars;
struct Cyc_List_List* _temp3165= _temp3163.free_evars;{ struct Cyc_List_List* x=
_temp3164; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3166=( void*) c->v;
void* _temp3174; _LL3168: if( _temp3166 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3169;} else{ goto _LL3170;} _LL3170: if(( unsigned int) _temp3166 >  1u?*((
int*) _temp3166) ==  Cyc_Absyn_Eq_constr: 0){ _LL3175: _temp3174=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3166)->f1; if( _temp3174 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3171;} else{ goto _LL3172;}} else{ goto _LL3172;}
_LL3172: goto _LL3173; _LL3169: goto _LL3171; _LL3171:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3176=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3176[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3177; _temp3177.tag= Cyc_Absyn_Eq_constr;
_temp3177.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3177;}); _temp3176;})));
goto _LL3167; _LL3173: goto _LL3167; _LL3167:;}} if( _temp3164 !=  0? 1:
_temp3165 !=  0){{ void* _temp3178= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3184; struct Cyc_List_List* _temp3186; struct Cyc_List_List* _temp3188;
struct Cyc_Absyn_VarargInfo* _temp3190; int _temp3192; struct Cyc_List_List*
_temp3194; void* _temp3196; struct Cyc_Core_Opt* _temp3198; struct Cyc_List_List*
_temp3200; struct Cyc_List_List** _temp3202; _LL3180: if(( unsigned int)
_temp3178 >  4u?*(( int*) _temp3178) ==  Cyc_Absyn_FnType: 0){ _LL3185:
_temp3184=(( struct Cyc_Absyn_FnType_struct*) _temp3178)->f1; _LL3201: _temp3200=
_temp3184.tvars; _temp3202=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3178)->f1).tvars; goto _LL3199; _LL3199: _temp3198= _temp3184.effect; goto
_LL3197; _LL3197: _temp3196=( void*) _temp3184.ret_typ; goto _LL3195; _LL3195:
_temp3194= _temp3184.args; goto _LL3193; _LL3193: _temp3192= _temp3184.c_varargs;
goto _LL3191; _LL3191: _temp3190= _temp3184.cyc_varargs; goto _LL3189; _LL3189:
_temp3188= _temp3184.rgn_po; goto _LL3187; _LL3187: _temp3186= _temp3184.attributes;
goto _LL3181;} else{ goto _LL3182;} _LL3182: goto _LL3183; _LL3181: if(*
_temp3202 ==  0){* _temp3202= _temp3164; _temp3164= 0;} goto _LL3179; _LL3183:
goto _LL3179; _LL3179:;} if( _temp3164 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3204; _temp3204.tag= Cyc_Std_String_pa; _temp3204.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3164))->hd)->name;{
void* _temp3203[ 1u]={& _temp3204}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp3203, sizeof( void*), 1u));}});}
if( _temp3165 !=  0){ for( 0; _temp3165 !=  0; _temp3165=(( struct Cyc_List_List*)
_check_null( _temp3165))->tl){ void* e=( void*)(( struct Cyc_List_List*)
_check_null( _temp3165))->hd; void* _temp3205= Cyc_Tcutil_typ_kind( e); _LL3207:
if( _temp3205 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3208;} else{ goto _LL3209;}
_LL3209: if( _temp3205 == ( void*) Cyc_Absyn_EffKind){ goto _LL3210;} else{ goto
_LL3211;} _LL3211: goto _LL3212; _LL3208: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp3213[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp3213, sizeof( void*), 0u));});} goto _LL3206; _LL3210: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp3214[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp3214, sizeof( void*), 0u));});} goto _LL3206; _LL3212:({ struct Cyc_Std_String_pa_struct
_temp3217; _temp3217.tag= Cyc_Std_String_pa; _temp3217.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3216;
_temp3216.tag= Cyc_Std_String_pa; _temp3216.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp3215[ 2u]={& _temp3216,& _temp3217}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3215, sizeof( void*), 2u));}}}); goto
_LL3206; _LL3206:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3218= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3224; struct Cyc_List_List* _temp3226; struct
Cyc_List_List* _temp3228; struct Cyc_Absyn_VarargInfo* _temp3230; int _temp3232;
struct Cyc_List_List* _temp3234; void* _temp3236; struct Cyc_Core_Opt* _temp3238;
struct Cyc_List_List* _temp3240; _LL3220: if(( unsigned int) _temp3218 >  4u?*((
int*) _temp3218) ==  Cyc_Absyn_FnType: 0){ _LL3225: _temp3224=(( struct Cyc_Absyn_FnType_struct*)
_temp3218)->f1; _LL3241: _temp3240= _temp3224.tvars; goto _LL3239; _LL3239:
_temp3238= _temp3224.effect; goto _LL3237; _LL3237: _temp3236=( void*) _temp3224.ret_typ;
goto _LL3235; _LL3235: _temp3234= _temp3224.args; goto _LL3233; _LL3233:
_temp3232= _temp3224.c_varargs; goto _LL3231; _LL3231: _temp3230= _temp3224.cyc_varargs;
goto _LL3229; _LL3229: _temp3228= _temp3224.rgn_po; goto _LL3227; _LL3227:
_temp3226= _temp3224.attributes; goto _LL3221;} else{ goto _LL3222;} _LL3222:
goto _LL3223; _LL3221: fd->tvs= _temp3240; fd->effect= _temp3238; goto _LL3219;
_LL3223:({ void* _temp3242[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3242, sizeof( void*), 0u));});
return; _LL3219:;}{ struct _RegionHandle _temp3243= _new_region(); struct
_RegionHandle* r=& _temp3243; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3244=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3244->v=(
void*) t; _temp3244;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3245=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3246= Cyc_Tcutil_remove_bound_tvars( _temp3245.free_vars,
bound_tvars); struct Cyc_List_List* _temp3247= _temp3245.free_evars;{ struct Cyc_List_List*
fs= _temp3246; for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3248=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name;({ struct Cyc_Std_String_pa_struct _temp3251;
_temp3251.tag= Cyc_Std_String_pa; _temp3251.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp3250; _temp3250.tag= Cyc_Std_String_pa;
_temp3250.f1=( struct _tagged_arr)* _temp3248;{ void* _temp3249[ 2u]={&
_temp3250,& _temp3251}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3249, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3247 !=  0: 0){ for( 0; _temp3247 !=  0; _temp3247=((
struct Cyc_List_List*) _check_null( _temp3247))->tl){ void* e=( void*)(( struct
Cyc_List_List*) _check_null( _temp3247))->hd; void* _temp3252= Cyc_Tcutil_typ_kind(
e); _LL3254: if( _temp3252 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3255;} else{
goto _LL3256;} _LL3256: if( _temp3252 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3257;} else{ goto _LL3258;} _LL3258: goto _LL3259; _LL3255: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3260[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3260, sizeof( void*), 0u));});}
goto _LL3253; _LL3257: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3261=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3261[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3262; _temp3262.tag= Cyc_Absyn_JoinEff;
_temp3262.f1= 0; _temp3262;}); _temp3261;}))){({ void* _temp3263[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3263, sizeof( void*), 0u));});}
goto _LL3253; _LL3259:({ struct Cyc_Std_String_pa_struct _temp3266; _temp3266.tag=
Cyc_Std_String_pa; _temp3266.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3265; _temp3265.tag= Cyc_Std_String_pa;
_temp3265.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3264[
2u]={& _temp3265,& _temp3266}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3264, sizeof( void*), 2u));}}});
goto _LL3253; _LL3253:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
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
struct Cyc_Std_String_pa_struct _temp3269; _temp3269.tag= Cyc_Std_String_pa;
_temp3269.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_Std_String_pa_struct _temp3268; _temp3268.tag=
Cyc_Std_String_pa; _temp3268.f1=( struct _tagged_arr) msg;{ void* _temp3267[ 2u]={&
_temp3268,& _temp3269}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3267, sizeof( void*), 2u));}}});}}}} static
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
unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List* _temp3270=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3270->hd=( void*)({
struct _tuple17* _temp3271=( struct _tuple17*) _cycalloc( sizeof( struct
_tuple17)); _temp3271->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3271->f2= 0; _temp3271;}); _temp3270->tl=
fields; _temp3270;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3274;
void* _temp3275; struct Cyc_List_List* _temp3277; struct _tuple18* _temp3272=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3274=*
_temp3272; _LL3278: _temp3277= _temp3274.f1; goto _LL3276; _LL3276: _temp3275=
_temp3274.f2; goto _LL3273; _LL3273: if( _temp3277 ==  0){ struct Cyc_List_List*
_temp3279= fields; for( 0; _temp3279 !=  0; _temp3279=(( struct Cyc_List_List*)
_check_null( _temp3279))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3279))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3279))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3280=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3280->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3281=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3281[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3282; _temp3282.tag= Cyc_Absyn_FieldName;
_temp3282.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3279))->hd)).f1)->name; _temp3282;}); _temp3281;})); _temp3280->tl= 0;
_temp3280;}); ans=({ struct Cyc_List_List* _temp3283=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3283->hd=( void*)({
struct _tuple19* _temp3284=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3284->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3279))->hd)).f1; _temp3284->f2= _temp3275; _temp3284;});
_temp3283->tl= ans; _temp3283;}); break;}} if( _temp3279 ==  0){({ void*
_temp3285[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3285, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3277))->tl !=  0){({ void*
_temp3286[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3286, sizeof( void*), 0u));});}
else{ void* _temp3287=( void*)(( struct Cyc_List_List*) _check_null( _temp3277))->hd;
struct _tagged_arr* _temp3293; _LL3289: if(*(( int*) _temp3287) ==  Cyc_Absyn_ArrayElement){
goto _LL3290;} else{ goto _LL3291;} _LL3291: if(*(( int*) _temp3287) ==  Cyc_Absyn_FieldName){
_LL3294: _temp3293=(( struct Cyc_Absyn_FieldName_struct*) _temp3287)->f1; goto
_LL3292;} else{ goto _LL3288;} _LL3290:({ void* _temp3295[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3295, sizeof( void*), 0u));}); goto _LL3288;
_LL3292: { struct Cyc_List_List* _temp3296= fields; for( 0; _temp3296 !=  0;
_temp3296=(( struct Cyc_List_List*) _check_null( _temp3296))->tl){ if( Cyc_Std_zstrptrcmp(
_temp3293,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3296))->hd)).f1)->name) ==  0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3296))->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3298;
_temp3298.tag= Cyc_Std_String_pa; _temp3298.f1=( struct _tagged_arr)* _temp3293;{
void* _temp3297[ 1u]={& _temp3298}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3297, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3296))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3299=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3299->hd=( void*)({ struct _tuple19*
_temp3300=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3300->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3296))->hd)).f1; _temp3300->f2= _temp3275; _temp3300;}); _temp3299->tl= ans;
_temp3299;}); break;}} if( _temp3296 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3302; _temp3302.tag= Cyc_Std_String_pa; _temp3302.f1=( struct _tagged_arr)*
_temp3293;{ void* _temp3301[ 1u]={& _temp3302}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3301, sizeof( void*), 1u));}});}
goto _LL3288;} _LL3288:;}}} for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3303[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3303, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3304= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3310; struct Cyc_Absyn_Conref* _temp3312; void* _temp3314;
_LL3306: if(( unsigned int) _temp3304 >  4u?*(( int*) _temp3304) ==  Cyc_Absyn_PointerType:
0){ _LL3311: _temp3310=(( struct Cyc_Absyn_PointerType_struct*) _temp3304)->f1;
_LL3315: _temp3314=( void*) _temp3310.elt_typ; goto _LL3313; _LL3313: _temp3312=
_temp3310.bounds; goto _LL3307;} else{ goto _LL3308;} _LL3308: goto _LL3309;
_LL3307: { struct Cyc_Absyn_Conref* _temp3316= Cyc_Absyn_compress_conref(
_temp3312); void* _temp3317=( void*)( Cyc_Absyn_compress_conref( _temp3316))->v;
void* _temp3325; _LL3319: if(( unsigned int) _temp3317 >  1u?*(( int*) _temp3317)
==  Cyc_Absyn_Eq_constr: 0){ _LL3326: _temp3325=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3317)->f1; if( _temp3325 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3320;}
else{ goto _LL3321;}} else{ goto _LL3321;} _LL3321: if( _temp3317 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3322;} else{ goto _LL3323;} _LL3323: goto _LL3324;
_LL3320:* elt_typ_dest= _temp3314; return 1; _LL3322:( void*)( _temp3316->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3327=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3327[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3328; _temp3328.tag= Cyc_Absyn_Eq_constr;
_temp3328.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3328;}); _temp3327;})));*
elt_typ_dest= _temp3314; return 1; _LL3324: return 0; _LL3318:;} _LL3309: return
0; _LL3305:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3511;
_temp3511.f1= 0; _temp3511.f2=( void*) Cyc_Absyn_HeapRgn; _temp3511;}); void*
_temp3329=( void*) e->r; void* _temp3343; struct _tuple1* _temp3345; struct
_tagged_arr* _temp3347; struct Cyc_Absyn_Exp* _temp3349; struct _tagged_arr*
_temp3351; struct Cyc_Absyn_Exp* _temp3353; struct Cyc_Absyn_Exp* _temp3355;
struct Cyc_Absyn_Exp* _temp3357; struct Cyc_Absyn_Exp* _temp3359; _LL3331: if(*((
int*) _temp3329) ==  Cyc_Absyn_Var_e){ _LL3346: _temp3345=(( struct Cyc_Absyn_Var_e_struct*)
_temp3329)->f1; goto _LL3344; _LL3344: _temp3343=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3329)->f2; goto _LL3332;} else{ goto _LL3333;} _LL3333: if(*(( int*)
_temp3329) ==  Cyc_Absyn_StructMember_e){ _LL3350: _temp3349=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3329)->f1; goto _LL3348; _LL3348: _temp3347=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3329)->f2; goto _LL3334;} else{ goto _LL3335;} _LL3335: if(*(( int*)
_temp3329) ==  Cyc_Absyn_StructArrow_e){ _LL3354: _temp3353=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3329)->f1; goto _LL3352; _LL3352: _temp3351=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3329)->f2; goto _LL3336;} else{ goto _LL3337;} _LL3337: if(*(( int*)
_temp3329) ==  Cyc_Absyn_Deref_e){ _LL3356: _temp3355=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3329)->f1; goto _LL3338;} else{ goto _LL3339;} _LL3339: if(*(( int*)
_temp3329) ==  Cyc_Absyn_Subscript_e){ _LL3360: _temp3359=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3329)->f1; goto _LL3358; _LL3358: _temp3357=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3329)->f2; goto _LL3340;} else{ goto _LL3341;} _LL3341: goto _LL3342;
_LL3332: { void* _temp3361= _temp3343; struct Cyc_Absyn_Vardecl* _temp3375;
struct Cyc_Absyn_Vardecl* _temp3377; struct Cyc_Absyn_Vardecl* _temp3379; struct
Cyc_Absyn_Vardecl* _temp3381; _LL3363: if( _temp3361 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3364;} else{ goto _LL3365;} _LL3365: if(( unsigned int) _temp3361 >  1u?*((
int*) _temp3361) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3366;} else{ goto _LL3367;}
_LL3367: if(( unsigned int) _temp3361 >  1u?*(( int*) _temp3361) ==  Cyc_Absyn_Global_b:
0){ _LL3376: _temp3375=(( struct Cyc_Absyn_Global_b_struct*) _temp3361)->f1;
goto _LL3368;} else{ goto _LL3369;} _LL3369: if(( unsigned int) _temp3361 >  1u?*((
int*) _temp3361) ==  Cyc_Absyn_Local_b: 0){ _LL3378: _temp3377=(( struct Cyc_Absyn_Local_b_struct*)
_temp3361)->f1; goto _LL3370;} else{ goto _LL3371;} _LL3371: if(( unsigned int)
_temp3361 >  1u?*(( int*) _temp3361) ==  Cyc_Absyn_Pat_b: 0){ _LL3380: _temp3379=((
struct Cyc_Absyn_Pat_b_struct*) _temp3361)->f1; goto _LL3372;} else{ goto
_LL3373;} _LL3373: if(( unsigned int) _temp3361 >  1u?*(( int*) _temp3361) == 
Cyc_Absyn_Param_b: 0){ _LL3382: _temp3381=(( struct Cyc_Absyn_Param_b_struct*)
_temp3361)->f1; goto _LL3374;} else{ goto _LL3362;} _LL3364: return bogus_ans;
_LL3366: return({ struct _tuple7 _temp3383; _temp3383.f1= 0; _temp3383.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3383;}); _LL3368: { void* _temp3384= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3386: if((
unsigned int) _temp3384 >  4u?*(( int*) _temp3384) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3387;} else{ goto _LL3388;} _LL3388: goto _LL3389; _LL3387: return({
struct _tuple7 _temp3390; _temp3390.f1= 1; _temp3390.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3390;}); _LL3389: return({ struct _tuple7 _temp3391; _temp3391.f1=(
_temp3375->tq).q_const; _temp3391.f2=( void*) Cyc_Absyn_HeapRgn; _temp3391;});
_LL3385:;} _LL3370: { void* _temp3392= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3394: if(( unsigned int) _temp3392 >  4u?*(( int*)
_temp3392) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3395;} else{ goto _LL3396;}
_LL3396: goto _LL3397; _LL3395: return({ struct _tuple7 _temp3398; _temp3398.f1=
1; _temp3398.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3377->rgn))->v;
_temp3398;}); _LL3397: return({ struct _tuple7 _temp3399; _temp3399.f1=(
_temp3377->tq).q_const; _temp3399.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3377->rgn))->v; _temp3399;}); _LL3393:;} _LL3372: _temp3381=
_temp3379; goto _LL3374; _LL3374: return({ struct _tuple7 _temp3400; _temp3400.f1=(
_temp3381->tq).q_const; _temp3400.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3381->rgn))->v; _temp3400;}); _LL3362:;} _LL3334: { void*
_temp3401= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3349->topt))->v); struct Cyc_List_List* _temp3413; struct Cyc_List_List*
_temp3415; struct Cyc_Absyn_Structdecl** _temp3417; struct Cyc_Absyn_Structdecl*
_temp3419; struct Cyc_Absyn_Uniondecl** _temp3420; struct Cyc_Absyn_Uniondecl*
_temp3422; _LL3403: if(( unsigned int) _temp3401 >  4u?*(( int*) _temp3401) == 
Cyc_Absyn_AnonStructType: 0){ _LL3414: _temp3413=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3401)->f1; goto _LL3404;} else{ goto _LL3405;} _LL3405: if(( unsigned int)
_temp3401 >  4u?*(( int*) _temp3401) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3416:
_temp3415=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3401)->f1; goto
_LL3406;} else{ goto _LL3407;} _LL3407: if(( unsigned int) _temp3401 >  4u?*((
int*) _temp3401) ==  Cyc_Absyn_StructType: 0){ _LL3418: _temp3417=(( struct Cyc_Absyn_StructType_struct*)
_temp3401)->f3; if( _temp3417 ==  0){ goto _LL3409;} else{ _temp3419=* _temp3417;
goto _LL3408;}} else{ goto _LL3409;} _LL3409: if(( unsigned int) _temp3401 >  4u?*((
int*) _temp3401) ==  Cyc_Absyn_UnionType: 0){ _LL3421: _temp3420=(( struct Cyc_Absyn_UnionType_struct*)
_temp3401)->f3; if( _temp3420 ==  0){ goto _LL3411;} else{ _temp3422=* _temp3420;
goto _LL3410;}} else{ goto _LL3411;} _LL3411: goto _LL3412; _LL3404: _temp3415=
_temp3413; goto _LL3406; _LL3406: { struct Cyc_Absyn_Structfield* _temp3423= Cyc_Absyn_lookup_field(
_temp3415, _temp3347); if( _temp3423 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3423))->width !=  0: 0){ return({ struct _tuple7 _temp3424;
_temp3424.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3423))->tq).q_const;
_temp3424.f2=( Cyc_Tcutil_addressof_props( te, _temp3349)).f2; _temp3424;});}
return bogus_ans;} _LL3408: { struct Cyc_Absyn_Structfield* _temp3425= Cyc_Absyn_lookup_struct_field(
_temp3419, _temp3347); if( _temp3425 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3425))->width !=  0: 0){ return({ struct _tuple7 _temp3426;
_temp3426.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3425))->tq).q_const;
_temp3426.f2=( Cyc_Tcutil_addressof_props( te, _temp3349)).f2; _temp3426;});}
return bogus_ans;} _LL3410: { struct Cyc_Absyn_Structfield* _temp3427= Cyc_Absyn_lookup_union_field(
_temp3422, _temp3347); if( _temp3427 !=  0){ return({ struct _tuple7 _temp3428;
_temp3428.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3427))->tq).q_const;
_temp3428.f2=( Cyc_Tcutil_addressof_props( te, _temp3349)).f2; _temp3428;});}
goto _LL3412;} _LL3412: return bogus_ans; _LL3402:;} _LL3336: { void* _temp3429=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3353->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3435; void* _temp3437; void* _temp3439; _LL3431:
if(( unsigned int) _temp3429 >  4u?*(( int*) _temp3429) ==  Cyc_Absyn_PointerType:
0){ _LL3436: _temp3435=(( struct Cyc_Absyn_PointerType_struct*) _temp3429)->f1;
_LL3440: _temp3439=( void*) _temp3435.elt_typ; goto _LL3438; _LL3438: _temp3437=(
void*) _temp3435.rgn_typ; goto _LL3432;} else{ goto _LL3433;} _LL3433: goto
_LL3434; _LL3432: { void* _temp3441= Cyc_Tcutil_compress( _temp3439); struct Cyc_List_List*
_temp3453; struct Cyc_List_List* _temp3455; struct Cyc_Absyn_Structdecl**
_temp3457; struct Cyc_Absyn_Structdecl* _temp3459; struct Cyc_Absyn_Uniondecl**
_temp3460; struct Cyc_Absyn_Uniondecl* _temp3462; _LL3443: if(( unsigned int)
_temp3441 >  4u?*(( int*) _temp3441) ==  Cyc_Absyn_AnonStructType: 0){ _LL3454:
_temp3453=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3441)->f1; goto
_LL3444;} else{ goto _LL3445;} _LL3445: if(( unsigned int) _temp3441 >  4u?*((
int*) _temp3441) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3456: _temp3455=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3441)->f1; goto _LL3446;} else{ goto
_LL3447;} _LL3447: if(( unsigned int) _temp3441 >  4u?*(( int*) _temp3441) == 
Cyc_Absyn_StructType: 0){ _LL3458: _temp3457=(( struct Cyc_Absyn_StructType_struct*)
_temp3441)->f3; if( _temp3457 ==  0){ goto _LL3449;} else{ _temp3459=* _temp3457;
goto _LL3448;}} else{ goto _LL3449;} _LL3449: if(( unsigned int) _temp3441 >  4u?*((
int*) _temp3441) ==  Cyc_Absyn_UnionType: 0){ _LL3461: _temp3460=(( struct Cyc_Absyn_UnionType_struct*)
_temp3441)->f3; if( _temp3460 ==  0){ goto _LL3451;} else{ _temp3462=* _temp3460;
goto _LL3450;}} else{ goto _LL3451;} _LL3451: goto _LL3452; _LL3444: _temp3455=
_temp3453; goto _LL3446; _LL3446: { struct Cyc_Absyn_Structfield* _temp3463= Cyc_Absyn_lookup_field(
_temp3455, _temp3351); if( _temp3463 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3463))->width !=  0: 0){ return({ struct _tuple7 _temp3464;
_temp3464.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3463))->tq).q_const;
_temp3464.f2= _temp3437; _temp3464;});} return bogus_ans;} _LL3448: { struct Cyc_Absyn_Structfield*
_temp3465= Cyc_Absyn_lookup_struct_field( _temp3459, _temp3351); if( _temp3465
!=  0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3465))->width !=  0: 0){
return({ struct _tuple7 _temp3466; _temp3466.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3465))->tq).q_const; _temp3466.f2= _temp3437; _temp3466;});}
return bogus_ans;} _LL3450: { struct Cyc_Absyn_Structfield* _temp3467= Cyc_Absyn_lookup_union_field(
_temp3462, _temp3351); if( _temp3467 !=  0){ return({ struct _tuple7 _temp3468;
_temp3468.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3467))->tq).q_const;
_temp3468.f2= _temp3437; _temp3468;});} return bogus_ans;} _LL3452: return
bogus_ans; _LL3442:;} _LL3434: return bogus_ans; _LL3430:;} _LL3338: { void*
_temp3469= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3355->topt))->v); struct Cyc_Absyn_PtrInfo _temp3475; struct Cyc_Absyn_Tqual
_temp3477; void* _temp3479; _LL3471: if(( unsigned int) _temp3469 >  4u?*(( int*)
_temp3469) ==  Cyc_Absyn_PointerType: 0){ _LL3476: _temp3475=(( struct Cyc_Absyn_PointerType_struct*)
_temp3469)->f1; _LL3480: _temp3479=( void*) _temp3475.rgn_typ; goto _LL3478;
_LL3478: _temp3477= _temp3475.tq; goto _LL3472;} else{ goto _LL3473;} _LL3473:
goto _LL3474; _LL3472: return({ struct _tuple7 _temp3481; _temp3481.f1=
_temp3477.q_const; _temp3481.f2= _temp3479; _temp3481;}); _LL3474: return
bogus_ans; _LL3470:;} _LL3340: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3359->topt))->v); void* _temp3482= t; struct
Cyc_Absyn_Tqual _temp3492; struct Cyc_List_List* _temp3494; struct Cyc_Absyn_PtrInfo
_temp3496; struct Cyc_Absyn_Conref* _temp3498; struct Cyc_Absyn_Tqual _temp3500;
void* _temp3502; void* _temp3504; _LL3484: if(( unsigned int) _temp3482 >  4u?*((
int*) _temp3482) ==  Cyc_Absyn_ArrayType: 0){ _LL3493: _temp3492=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3482)->f2; goto _LL3485;} else{ goto _LL3486;} _LL3486: if(( unsigned int)
_temp3482 >  4u?*(( int*) _temp3482) ==  Cyc_Absyn_TupleType: 0){ _LL3495:
_temp3494=(( struct Cyc_Absyn_TupleType_struct*) _temp3482)->f1; goto _LL3487;}
else{ goto _LL3488;} _LL3488: if(( unsigned int) _temp3482 >  4u?*(( int*)
_temp3482) ==  Cyc_Absyn_PointerType: 0){ _LL3497: _temp3496=(( struct Cyc_Absyn_PointerType_struct*)
_temp3482)->f1; _LL3505: _temp3504=( void*) _temp3496.elt_typ; goto _LL3503;
_LL3503: _temp3502=( void*) _temp3496.rgn_typ; goto _LL3501; _LL3501: _temp3500=
_temp3496.tq; goto _LL3499; _LL3499: _temp3498= _temp3496.bounds; goto _LL3489;}
else{ goto _LL3490;} _LL3490: goto _LL3491; _LL3485: return({ struct _tuple7
_temp3506; _temp3506.f1= _temp3492.q_const; _temp3506.f2=( Cyc_Tcutil_addressof_props(
te, _temp3359)).f2; _temp3506;}); _LL3487: { struct _tuple4* _temp3507= Cyc_Absyn_lookup_tuple_field(
_temp3494,( int) Cyc_Evexp_eval_const_uint_exp( _temp3357)); if( _temp3507 !=  0){
return({ struct _tuple7 _temp3508; _temp3508.f1=((*(( struct _tuple4*)
_check_null( _temp3507))).f1).q_const; _temp3508.f2=( Cyc_Tcutil_addressof_props(
te, _temp3359)).f2; _temp3508;});} return bogus_ans;} _LL3489: return({ struct
_tuple7 _temp3509; _temp3509.f1= _temp3500.q_const; _temp3509.f2= _temp3502;
_temp3509;}); _LL3491: return bogus_ans; _LL3483:;} _LL3342:({ void* _temp3510[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3510, sizeof( void*), 0u));});
return bogus_ans; _LL3330:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3512= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3518; void* _temp3520; _LL3514: if((
unsigned int) _temp3512 >  4u?*(( int*) _temp3512) ==  Cyc_Absyn_ArrayType: 0){
_LL3521: _temp3520=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3512)->f1;
goto _LL3519; _LL3519: _temp3518=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3512)->f2; goto _LL3515;} else{ goto _LL3516;} _LL3516: goto _LL3517;
_LL3515: { void* _temp3524; struct _tuple7 _temp3522= Cyc_Tcutil_addressof_props(
te, e); _LL3525: _temp3524= _temp3522.f2; goto _LL3523; _LL3523: return Cyc_Absyn_atb_typ(
_temp3520, _temp3524, _temp3518,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3526=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3526[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3527; _temp3527.tag= Cyc_Absyn_Upper_b;
_temp3527.f1= e; _temp3527;}); _temp3526;}));} _LL3517: return e_typ; _LL3513:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3528=(
void*) b->v; void* _temp3536; void* _temp3538; struct Cyc_Absyn_Exp* _temp3540;
_LL3530: if(( unsigned int) _temp3528 >  1u?*(( int*) _temp3528) ==  Cyc_Absyn_Eq_constr:
0){ _LL3537: _temp3536=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3528)->f1;
if( _temp3536 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3531;} else{ goto
_LL3532;}} else{ goto _LL3532;} _LL3532: if(( unsigned int) _temp3528 >  1u?*((
int*) _temp3528) ==  Cyc_Absyn_Eq_constr: 0){ _LL3539: _temp3538=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3528)->f1; if(( unsigned int) _temp3538
>  1u?*(( int*) _temp3538) ==  Cyc_Absyn_Upper_b: 0){ _LL3541: _temp3540=((
struct Cyc_Absyn_Upper_b_struct*) _temp3538)->f1; goto _LL3533;} else{ goto
_LL3534;}} else{ goto _LL3534;} _LL3534: goto _LL3535; _LL3531: return; _LL3533:
if( Cyc_Evexp_eval_const_uint_exp( _temp3540) <=  i){({ void* _temp3542[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3542, sizeof( void*), 0u));});} return;
_LL3535:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3543=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3543[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3544; _temp3544.tag= Cyc_Absyn_Eq_constr;
_temp3544.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3545=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3545[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3546; _temp3546.tag= Cyc_Absyn_Upper_b;
_temp3546.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3546;}); _temp3545;}));
_temp3544;}); _temp3543;}))); return; _LL3529:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp3547=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp3553; struct
Cyc_Absyn_Exp* _temp3555; _LL3549: if(( unsigned int) _temp3547 >  1u?*(( int*)
_temp3547) ==  Cyc_Absyn_Eq_constr: 0){ _LL3554: _temp3553=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3547)->f1; if(( unsigned int) _temp3553 >  1u?*(( int*) _temp3553) ==  Cyc_Absyn_Upper_b:
0){ _LL3556: _temp3555=(( struct Cyc_Absyn_Upper_b_struct*) _temp3553)->f1; goto
_LL3550;} else{ goto _LL3551;}} else{ goto _LL3551;} _LL3551: goto _LL3552;
_LL3550: return Cyc_Evexp_eval_const_uint_exp( _temp3555) ==  1; _LL3552: return
0; _LL3548:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp3557= Cyc_Tcutil_compress(
t); void* _temp3583; struct Cyc_List_List* _temp3585; struct Cyc_Absyn_Structdecl**
_temp3587; struct Cyc_List_List* _temp3589; struct Cyc_Absyn_Uniondecl**
_temp3591; struct Cyc_List_List* _temp3593; struct Cyc_List_List* _temp3595;
struct Cyc_List_List* _temp3597; _LL3559: if( _temp3557 == ( void*) Cyc_Absyn_VoidType){
goto _LL3560;} else{ goto _LL3561;} _LL3561: if(( unsigned int) _temp3557 >  4u?*((
int*) _temp3557) ==  Cyc_Absyn_IntType: 0){ goto _LL3562;} else{ goto _LL3563;}
_LL3563: if( _temp3557 == ( void*) Cyc_Absyn_FloatType){ goto _LL3564;} else{
goto _LL3565;} _LL3565: if( _temp3557 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3566;} else{ goto _LL3567;} _LL3567: if(( unsigned int) _temp3557 >  4u?*((
int*) _temp3557) ==  Cyc_Absyn_EnumType: 0){ goto _LL3568;} else{ goto _LL3569;}
_LL3569: if(( unsigned int) _temp3557 >  4u?*(( int*) _temp3557) ==  Cyc_Absyn_ArrayType:
0){ _LL3584: _temp3583=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3557)->f1;
goto _LL3570;} else{ goto _LL3571;} _LL3571: if(( unsigned int) _temp3557 >  4u?*((
int*) _temp3557) ==  Cyc_Absyn_TupleType: 0){ _LL3586: _temp3585=(( struct Cyc_Absyn_TupleType_struct*)
_temp3557)->f1; goto _LL3572;} else{ goto _LL3573;} _LL3573: if(( unsigned int)
_temp3557 >  4u?*(( int*) _temp3557) ==  Cyc_Absyn_StructType: 0){ _LL3590:
_temp3589=(( struct Cyc_Absyn_StructType_struct*) _temp3557)->f2; goto _LL3588;
_LL3588: _temp3587=(( struct Cyc_Absyn_StructType_struct*) _temp3557)->f3; goto
_LL3574;} else{ goto _LL3575;} _LL3575: if(( unsigned int) _temp3557 >  4u?*((
int*) _temp3557) ==  Cyc_Absyn_UnionType: 0){ _LL3594: _temp3593=(( struct Cyc_Absyn_UnionType_struct*)
_temp3557)->f2; goto _LL3592; _LL3592: _temp3591=(( struct Cyc_Absyn_UnionType_struct*)
_temp3557)->f3; goto _LL3576;} else{ goto _LL3577;} _LL3577: if(( unsigned int)
_temp3557 >  4u?*(( int*) _temp3557) ==  Cyc_Absyn_AnonStructType: 0){ _LL3596:
_temp3595=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3557)->f1; goto
_LL3578;} else{ goto _LL3579;} _LL3579: if(( unsigned int) _temp3557 >  4u?*((
int*) _temp3557) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3598: _temp3597=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3557)->f1; goto _LL3580;} else{ goto
_LL3581;} _LL3581: goto _LL3582; _LL3560: goto _LL3562; _LL3562: goto _LL3564;
_LL3564: goto _LL3566; _LL3566: return 1; _LL3568: return 0; _LL3570: return Cyc_Tcutil_bits_only(
_temp3583); _LL3572: for( 0; _temp3585 !=  0; _temp3585=(( struct Cyc_List_List*)
_check_null( _temp3585))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3585))->hd)).f2)){ return 0;}} return 1;
_LL3574: if( _temp3587 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp3599=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp3587)); if(
_temp3599->fields ==  0){ return 0;}{ struct _RegionHandle _temp3600=
_new_region(); struct _RegionHandle* rgn=& _temp3600; _push_region( rgn);{
struct Cyc_List_List* _temp3601=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3599->tvs, _temp3589);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp3599->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp3601,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ int _temp3602= 0;
_npop_handler( 0u); return _temp3602;}}}{ int _temp3603= 1; _npop_handler( 0u);
return _temp3603;}}; _pop_region( rgn);}} _LL3576: if( _temp3591 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3604=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3591)); if( _temp3604->fields ==  0){ return 0;}{ struct
_RegionHandle _temp3605= _new_region(); struct _RegionHandle* rgn=& _temp3605;
_push_region( rgn);{ struct Cyc_List_List* _temp3606=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3604->tvs, _temp3593);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3604->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3606,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3607= 0; _npop_handler( 0u); return
_temp3607;}}}{ int _temp3608= 1; _npop_handler( 0u); return _temp3608;}};
_pop_region( rgn);}} _LL3578: _temp3597= _temp3595; goto _LL3580; _LL3580: for(
0; _temp3597 !=  0; _temp3597=(( struct Cyc_List_List*) _check_null( _temp3597))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3597))->hd)->type)){ return 0;}} return 1;
_LL3582: return 0; _LL3558:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3609=( void*) e->r; struct
_tuple1* _temp3649; struct Cyc_Absyn_Exp* _temp3651; struct Cyc_Absyn_Exp*
_temp3653; struct Cyc_Absyn_Exp* _temp3655; struct Cyc_Absyn_Exp* _temp3657;
struct Cyc_Absyn_Exp* _temp3659; struct Cyc_Absyn_Exp* _temp3661; struct Cyc_Absyn_Exp*
_temp3663; struct Cyc_Absyn_Exp* _temp3665; void* _temp3667; struct Cyc_Absyn_Exp*
_temp3669; struct Cyc_Absyn_Exp* _temp3671; struct Cyc_Absyn_Exp* _temp3673;
struct Cyc_List_List* _temp3675; struct Cyc_List_List* _temp3677; struct Cyc_List_List*
_temp3679; struct Cyc_List_List* _temp3681; void* _temp3683; struct Cyc_List_List*
_temp3685; struct Cyc_List_List* _temp3687; _LL3611: if(*(( int*) _temp3609) == 
Cyc_Absyn_Const_e){ goto _LL3612;} else{ goto _LL3613;} _LL3613: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3614;} else{ goto _LL3615;}
_LL3615: if(*(( int*) _temp3609) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3616;}
else{ goto _LL3617;} _LL3617: if(*(( int*) _temp3609) ==  Cyc_Absyn_Enum_e){
goto _LL3618;} else{ goto _LL3619;} _LL3619: if(*(( int*) _temp3609) ==  Cyc_Absyn_Var_e){
_LL3650: _temp3649=(( struct Cyc_Absyn_Var_e_struct*) _temp3609)->f1; goto
_LL3620;} else{ goto _LL3621;} _LL3621: if(*(( int*) _temp3609) ==  Cyc_Absyn_Conditional_e){
_LL3656: _temp3655=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3609)->f1;
goto _LL3654; _LL3654: _temp3653=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3609)->f2; goto _LL3652; _LL3652: _temp3651=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3609)->f3; goto _LL3622;} else{ goto _LL3623;} _LL3623: if(*(( int*)
_temp3609) ==  Cyc_Absyn_SeqExp_e){ _LL3660: _temp3659=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3609)->f1; goto _LL3658; _LL3658: _temp3657=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3609)->f2; goto _LL3624;} else{ goto _LL3625;} _LL3625: if(*(( int*)
_temp3609) ==  Cyc_Absyn_NoInstantiate_e){ _LL3662: _temp3661=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3609)->f1; goto _LL3626;} else{ goto _LL3627;} _LL3627: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Instantiate_e){ _LL3664: _temp3663=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3609)->f1; goto _LL3628;} else{ goto _LL3629;} _LL3629: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Cast_e){ _LL3668: _temp3667=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3609)->f1; goto _LL3666; _LL3666: _temp3665=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3609)->f2; goto _LL3630;} else{ goto _LL3631;} _LL3631: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Address_e){ _LL3670: _temp3669=(( struct Cyc_Absyn_Address_e_struct*)
_temp3609)->f1; goto _LL3632;} else{ goto _LL3633;} _LL3633: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Comprehension_e){ _LL3674: _temp3673=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3609)->f2; goto _LL3672; _LL3672: _temp3671=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3609)->f3; goto _LL3634;} else{ goto _LL3635;} _LL3635: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Array_e){ _LL3676: _temp3675=(( struct Cyc_Absyn_Array_e_struct*)
_temp3609)->f1; goto _LL3636;} else{ goto _LL3637;} _LL3637: if(*(( int*)
_temp3609) ==  Cyc_Absyn_AnonStruct_e){ _LL3678: _temp3677=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3609)->f2; goto _LL3638;} else{ goto _LL3639;} _LL3639: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Struct_e){ _LL3680: _temp3679=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3609)->f3; goto _LL3640;} else{ goto _LL3641;} _LL3641: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Primop_e){ _LL3684: _temp3683=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3609)->f1; goto _LL3682; _LL3682: _temp3681=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3609)->f2; goto _LL3642;} else{ goto _LL3643;} _LL3643: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Tuple_e){ _LL3686: _temp3685=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3609)->f1; goto _LL3644;} else{ goto _LL3645;} _LL3645: if(*(( int*)
_temp3609) ==  Cyc_Absyn_Tunion_e){ _LL3688: _temp3687=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3609)->f3; goto _LL3646;} else{ goto _LL3647;} _LL3647: goto _LL3648;
_LL3612: return 1; _LL3614: return 1; _LL3616: return 1; _LL3618: return 1;
_LL3620: { struct _handler_cons _temp3689; _push_handler(& _temp3689);{ int
_temp3691= 0; if( setjmp( _temp3689.handler)){ _temp3691= 1;} if( ! _temp3691){{
void* _temp3692= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3649); void*
_temp3700; void* _temp3702; struct Cyc_Absyn_Vardecl* _temp3704; _LL3694: if(*((
int*) _temp3692) ==  Cyc_Tcenv_VarRes){ _LL3701: _temp3700=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3692)->f1; if(( unsigned int) _temp3700 >  1u?*(( int*) _temp3700) ==  Cyc_Absyn_Funname_b:
0){ goto _LL3695;} else{ goto _LL3696;}} else{ goto _LL3696;} _LL3696: if(*((
int*) _temp3692) ==  Cyc_Tcenv_VarRes){ _LL3703: _temp3702=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3692)->f1; if(( unsigned int) _temp3702 >  1u?*(( int*) _temp3702) ==  Cyc_Absyn_Global_b:
0){ _LL3705: _temp3704=(( struct Cyc_Absyn_Global_b_struct*) _temp3702)->f1;
goto _LL3697;} else{ goto _LL3698;}} else{ goto _LL3698;} _LL3698: goto _LL3699;
_LL3695: { int _temp3706= 1; _npop_handler( 0u); return _temp3706;} _LL3697: {
void* _temp3707= Cyc_Tcutil_compress(( void*) _temp3704->type); _LL3709: if((
unsigned int) _temp3707 >  4u?*(( int*) _temp3707) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3710;} else{ goto _LL3711;} _LL3711: goto _LL3712; _LL3710: { int
_temp3713= 1; _npop_handler( 0u); return _temp3713;} _LL3712: { int _temp3714=
var_okay; _npop_handler( 0u); return _temp3714;} _LL3708:;} _LL3699: { int
_temp3715= var_okay; _npop_handler( 0u); return _temp3715;} _LL3693:;};
_pop_handler();} else{ void* _temp3690=( void*) _exn_thrown; void* _temp3717=
_temp3690; _LL3719: if( _temp3717 ==  Cyc_Dict_Absent){ goto _LL3720;} else{
goto _LL3721;} _LL3721: goto _LL3722; _LL3720: return 0; _LL3722:( void) _throw(
_temp3717); _LL3718:;}}} _LL3622: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3655)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3653): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3651): 0; _LL3624: return Cyc_Tcutil_cnst_exp( te, 0, _temp3659)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3657): 0; _LL3626: _temp3663= _temp3661; goto _LL3628; _LL3628:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3663); _LL3630: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3665); _LL3632: return Cyc_Tcutil_cnst_exp( te, 1, _temp3669);
_LL3634: return Cyc_Tcutil_cnst_exp( te, 0, _temp3673)? Cyc_Tcutil_cnst_exp( te,
0, _temp3671): 0; _LL3636: _temp3677= _temp3675; goto _LL3638; _LL3638:
_temp3679= _temp3677; goto _LL3640; _LL3640: for( 0; _temp3679 !=  0; _temp3679=((
struct Cyc_List_List*) _check_null( _temp3679))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3679))->hd)).f2)){
return 0;}} return 1; _LL3642: _temp3685= _temp3681; goto _LL3644; _LL3644:
_temp3687= _temp3685; goto _LL3646; _LL3646: for( 0; _temp3687 !=  0; _temp3687=((
struct Cyc_List_List*) _check_null( _temp3687))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3687))->hd)){
return 0;}} return 1; _LL3648: return 0; _LL3610:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3723= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3751; struct Cyc_Absyn_Conref* _temp3753; struct Cyc_Absyn_Conref*
_temp3755; void* _temp3757; struct Cyc_List_List* _temp3759; struct Cyc_Absyn_Structdecl**
_temp3761; struct Cyc_List_List* _temp3763; struct Cyc_Absyn_Uniondecl**
_temp3765; struct Cyc_List_List* _temp3767; struct Cyc_List_List* _temp3769;
struct Cyc_List_List* _temp3771; _LL3725: if( _temp3723 == ( void*) Cyc_Absyn_VoidType){
goto _LL3726;} else{ goto _LL3727;} _LL3727: if(( unsigned int) _temp3723 >  4u?*((
int*) _temp3723) ==  Cyc_Absyn_IntType: 0){ goto _LL3728;} else{ goto _LL3729;}
_LL3729: if( _temp3723 == ( void*) Cyc_Absyn_FloatType){ goto _LL3730;} else{
goto _LL3731;} _LL3731: if( _temp3723 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3732;} else{ goto _LL3733;} _LL3733: if(( unsigned int) _temp3723 >  4u?*((
int*) _temp3723) ==  Cyc_Absyn_PointerType: 0){ _LL3752: _temp3751=(( struct Cyc_Absyn_PointerType_struct*)
_temp3723)->f1; _LL3756: _temp3755= _temp3751.nullable; goto _LL3754; _LL3754:
_temp3753= _temp3751.bounds; goto _LL3734;} else{ goto _LL3735;} _LL3735: if((
unsigned int) _temp3723 >  4u?*(( int*) _temp3723) ==  Cyc_Absyn_ArrayType: 0){
_LL3758: _temp3757=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3723)->f1;
goto _LL3736;} else{ goto _LL3737;} _LL3737: if(( unsigned int) _temp3723 >  4u?*((
int*) _temp3723) ==  Cyc_Absyn_TupleType: 0){ _LL3760: _temp3759=(( struct Cyc_Absyn_TupleType_struct*)
_temp3723)->f1; goto _LL3738;} else{ goto _LL3739;} _LL3739: if(( unsigned int)
_temp3723 >  4u?*(( int*) _temp3723) ==  Cyc_Absyn_StructType: 0){ _LL3764:
_temp3763=(( struct Cyc_Absyn_StructType_struct*) _temp3723)->f2; goto _LL3762;
_LL3762: _temp3761=(( struct Cyc_Absyn_StructType_struct*) _temp3723)->f3; goto
_LL3740;} else{ goto _LL3741;} _LL3741: if(( unsigned int) _temp3723 >  4u?*((
int*) _temp3723) ==  Cyc_Absyn_UnionType: 0){ _LL3768: _temp3767=(( struct Cyc_Absyn_UnionType_struct*)
_temp3723)->f2; goto _LL3766; _LL3766: _temp3765=(( struct Cyc_Absyn_UnionType_struct*)
_temp3723)->f3; goto _LL3742;} else{ goto _LL3743;} _LL3743: if(( unsigned int)
_temp3723 >  4u?*(( int*) _temp3723) ==  Cyc_Absyn_AnonStructType: 0){ _LL3770:
_temp3769=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3723)->f1; goto
_LL3744;} else{ goto _LL3745;} _LL3745: if(( unsigned int) _temp3723 >  4u?*((
int*) _temp3723) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3772: _temp3771=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3723)->f1; goto _LL3746;} else{ goto
_LL3747;} _LL3747: if(( unsigned int) _temp3723 >  4u?*(( int*) _temp3723) == 
Cyc_Absyn_EnumType: 0){ goto _LL3748;} else{ goto _LL3749;} _LL3749: goto
_LL3750; _LL3726: goto _LL3728; _LL3728: goto _LL3730; _LL3730: goto _LL3732;
_LL3732: return 1; _LL3734: { void* _temp3773=( void*)( Cyc_Absyn_compress_conref(
_temp3753))->v; void* _temp3781; void* _temp3783; _LL3775: if(( unsigned int)
_temp3773 >  1u?*(( int*) _temp3773) ==  Cyc_Absyn_Eq_constr: 0){ _LL3782:
_temp3781=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3773)->f1; if(
_temp3781 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3776;} else{ goto _LL3777;}}
else{ goto _LL3777;} _LL3777: if(( unsigned int) _temp3773 >  1u?*(( int*)
_temp3773) ==  Cyc_Absyn_Eq_constr: 0){ _LL3784: _temp3783=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3773)->f1; if(( unsigned int) _temp3783 >  1u?*(( int*) _temp3783) ==  Cyc_Absyn_Upper_b:
0){ goto _LL3778;} else{ goto _LL3779;}} else{ goto _LL3779;} _LL3779: goto
_LL3780; _LL3776: return 1; _LL3778: { void* _temp3785=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3755))->v; int
_temp3791; _LL3787: if(( unsigned int) _temp3785 >  1u?*(( int*) _temp3785) == 
Cyc_Absyn_Eq_constr: 0){ _LL3792: _temp3791=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3785)->f1; goto _LL3788;} else{ goto _LL3789;} _LL3789: goto _LL3790;
_LL3788: return _temp3791; _LL3790: return 0; _LL3786:;} _LL3780: return 0;
_LL3774:;} _LL3736: return Cyc_Tcutil_supports_default( _temp3757); _LL3738:
for( 0; _temp3759 !=  0; _temp3759=(( struct Cyc_List_List*) _check_null(
_temp3759))->tl){ if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3759))->hd)).f2)){ return 0;}} return 1;
_LL3740: if( _temp3761 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3761)); if( sd->fields ==  0){
return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs, _temp3763,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);} _LL3742:
if( _temp3765 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3765)); if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp3767,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL3744: _temp3771= _temp3769; goto _LL3746; _LL3746: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp3771); _LL3748: return 1; _LL3750:
return 0; _LL3724:;} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{ struct _RegionHandle
_temp3793= _new_region(); struct _RegionHandle* rgn=& _temp3793; _push_region(
rgn);{ struct Cyc_List_List* _temp3794=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3794,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3795= 0; _npop_handler( 0u);
return _temp3795;}}}; _pop_region( rgn);} return 1;}
